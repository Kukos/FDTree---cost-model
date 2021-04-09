#include <dbindex_fdtree.h>
#include <ssd.h>
#include <math.h>
#include <stdlib.h>
#include <dbutils.h>
#include <dbstat.h>

/*
    PARAMS
    @IN index - pointer to index

    RETURN
    Maximum number of entries that can be written on one page
*/
static inline size_t db_index_fdtree_entries_per_page(DB_index_fdtree *index);

/*
    PARAMS
    @IN index - pointer to index
    @IN entries - number of entries

    RETURN
    Number of pages used by entries
*/
static inline size_t db_index_fdtree_pages_for_entries(DB_index_fdtree *index, size_t entries);

/*
    Merge 2 lvls into 1 (Copy all entries from lvl1 to lvl2 and sort them like list)
    IF lvl2 will be full, then we will merge lvl2 with lvl2 + 1 and so on

    PARAMS
    @IN index - index
    @IN lvl1 - first lvl
    @IN lvl2 - second lvl

    RETURN
    Time spent for merging
*/
static double db_index_fdtree_merge_runs(DB_index_fdtree* index, size_t lvl1, size_t lvl2);

/*
    Merge HeadTree with Lvl0
    IF LVl0 will be full, then we will merge lvl0 with lvl1 and so on

    PARAMS
    @IN index - index

    RETURN
    Time spent for merging
*/
static double db_index_fdtree_merge_headtree(DB_index_fdtree* index);

static inline size_t db_index_fdtree_entries_per_page(DB_index_fdtree *index)
{
    return db_utils_entries_per_page(index->ssd->page_size, index->entry_size);
}

static inline size_t db_index_fdtree_pages_for_entries(DB_index_fdtree *index, size_t entries)
{
    const size_t pages_for_entries = db_utils_pages_for_entries(index->ssd->page_size, index->entry_size, entries);
    const size_t pages_for_pointers = db_utils_pages_for_entries(index->ssd->page_size, index->key_size + sizeof(void *), pages_for_entries);

    return pages_for_entries + pages_for_pointers;
}

static double db_index_fdtree_merge_headtree(DB_index_fdtree* index)
{
    double time = 0.0;

    FDLvl* fdlvl1 = &index->sortedruns[0];
    FDHead* headtree = &index->headtree;

    ssize_t entries_in_lvl1_after_merge = (ssize_t)(fdlvl1->num_entries + headtree->num_entries - headtree->num_entries_to_delete);

    /* First merge lvl0 with lvl1 to make space for entries from headtree */
    if (entries_in_lvl1_after_merge + (ssize_t)fdlvl1->num_entries_to_delete >= (ssize_t)fdlvl1->max_entries)
        time += db_index_fdtree_merge_runs(index, 0, 1);

    /* UPDATE entries in fdlvl1 */
    size_t entries_to_delete_after_merge = (headtree->num_entries_to_delete > fdlvl1->num_entries ? headtree->num_entries_to_delete - fdlvl1->num_entries : 0);
    entries_in_lvl1_after_merge = (ssize_t)(fdlvl1->num_entries + headtree->num_entries - headtree->num_entries_to_delete);

    /* reading entries from headtree is free */

    /* read entries from lvl0 */
    time += ssd_sread_pages(index->ssd, db_index_fdtree_pages_for_entries(index, fdlvl1->num_entries + fdlvl1->num_entries_to_delete));

    /* write entries to lvl0 */
    if (entries_in_lvl1_after_merge > 0)
        time += ssd_swrite_pages(index->ssd, db_index_fdtree_pages_for_entries(index, (size_t)entries_in_lvl1_after_merge + fdlvl1->num_entries_to_delete));
    else if (entries_to_delete_after_merge > 0)
        time += ssd_swrite_pages(index->ssd, db_index_fdtree_pages_for_entries(index, entries_to_delete_after_merge));

    headtree->num_entries_to_delete = 0;
    headtree->num_entries = 0;

    if (entries_in_lvl1_after_merge > 0)
        fdlvl1->num_entries = (size_t)entries_in_lvl1_after_merge;
    else
        fdlvl1->num_entries = 0;

    fdlvl1->num_entries_to_delete += entries_to_delete_after_merge;

    return time;
}

static double db_index_fdtree_merge_runs(DB_index_fdtree* index, size_t lvl1, size_t lvl2)
{
    double time = 0.0;

    /* cannot merge, mark as invalid */
    if (lvl2 >= DBINDEX_FDTREE_MAX_LVL)
        return (double)9999999999;

    FDLvl* fdlvl1 = &index->sortedruns[lvl1];
    FDLvl* fdlvl2 = &index->sortedruns[lvl2];

    ssize_t entries_in_lvl2_after_merge = (ssize_t)(fdlvl1->num_entries + fdlvl2->num_entries - fdlvl1->num_entries_to_delete);

    /* we need merge lvl2 with lvl2 + 1 to make space for lvl1 entries */
    if (entries_in_lvl2_after_merge + (ssize_t)fdlvl2->num_entries_to_delete >= (ssize_t)fdlvl2->max_entries)
        time += db_index_fdtree_merge_runs(index, lvl2, lvl2 + 1);

    entries_in_lvl2_after_merge = (ssize_t)(fdlvl1->num_entries + fdlvl2->num_entries - fdlvl1->num_entries_to_delete);
    size_t entries_to_delete_after_merge = (fdlvl1->num_entries_to_delete > fdlvl2->num_entries ? fdlvl1->num_entries_to_delete - fdlvl2->num_entries : 0);

    /* First time when we reach lvl2, so height++  */
    if (index->height < (lvl2 + 1) && fdlvl2->num_entries == 0)
        ++index->height;

    /* read lvl1 and lvl2 */
    time += ssd_sread_pages(index->ssd, db_index_fdtree_pages_for_entries(index, fdlvl1->num_entries + fdlvl1->num_entries_to_delete));
    time += ssd_sread_pages(index->ssd, db_index_fdtree_pages_for_entries(index, fdlvl2->num_entries + fdlvl2->num_entries_to_delete));

    /* write down merged lvl1 and lvl2 */
    if (entries_in_lvl2_after_merge > 0)
        time += ssd_swrite_pages(index->ssd, db_index_fdtree_pages_for_entries(index, (size_t)entries_in_lvl2_after_merge + fdlvl2->num_entries_to_delete));
    else if (entries_to_delete_after_merge > 0)
         time += ssd_swrite_pages(index->ssd, db_index_fdtree_pages_for_entries(index, entries_to_delete_after_merge));

    // write fences into lvl1
    time += ssd_swrite_pages(index->ssd, 1);

    if (entries_in_lvl2_after_merge > 0)
        fdlvl2->num_entries = (size_t)entries_in_lvl2_after_merge;
    else
        fdlvl2->num_entries = 0;

    fdlvl2->num_entries_to_delete += entries_to_delete_after_merge;
    fdlvl1->num_entries = 0;
    fdlvl1->num_entries_to_delete = 0;

    return time;
}

DB_index_fdtree *db_index_fdtree_create(SSD *ssd, size_t key_size, size_t entry_size, size_t runs_ratio)
{
    DB_index_fdtree *index;

    index = (DB_index_fdtree *)calloc(1, sizeof(DB_index_fdtree));
    if (index == NULL)
        return NULL;

    index->ssd = ssd;
    index->key_size = key_size;
    index->entry_size = entry_size;
    index->runs_ratio = runs_ratio;
    index->height = 1;

    index->headtree.max_entries = db_index_fdtree_entries_per_page(index);
    index->sortedruns[0].max_entries = index->headtree.max_entries * index->runs_ratio;

    for (size_t i = 1; i < DBINDEX_FDTREE_MAX_LVL; ++i)
        index->sortedruns[i].max_entries = index->sortedruns[i - 1].max_entries * index->runs_ratio;

    return index;
}

void db_index_fdtree_destroy(DB_index_fdtree *index)
{
    if (index == NULL)
        return;

    free(index);
}

double db_index_fdtree_insert(DB_index_fdtree *index, size_t entries)
{
    double time = 0.0;
    FDHead* headtree = &index->headtree;

    for (size_t i = 0; i < entries; ++i)
    {
        ++index->num_entries;

        /* insert into HEAD is free (head tree is in RAM) */
        ++headtree->num_entries;

        /* merge with LVL0 */
        if (headtree->num_entries + headtree->num_entries_to_delete >= headtree->max_entries)
            time += db_index_fdtree_merge_headtree(index);
    }

    db_stat_update_query_time(time);
    return time;
}

double db_index_fdtree_bulkload(DB_index_fdtree *index, size_t entries)
{
    /* Bulkload is the same as insert */
    return db_index_fdtree_insert(index, entries);
}

double db_index_fdtree_point_search(DB_index_fdtree *index, size_t entries)
{
    double time = 0.0;

    time += ssd_rread_pages(index->ssd, index->height) * (double)entries;

    db_stat_update_query_time(time);
    return time;
}

double db_index_fdtree_range_search(DB_index_fdtree *index, size_t entries)
{
    double time = 0.0;

    /* find start point */
    time += ssd_rread_pages(index->ssd, index->height);

    /* read all entries */
    time += ssd_sread_pages(index->ssd, db_index_fdtree_pages_for_entries(index, entries));

    db_stat_update_query_time(time);
    return time;
}

double db_index_fdtree_delete(DB_index_fdtree *index, size_t entries)
{
    double time = 0.0;
    FDHead* headtree = &index->headtree;

    for (size_t i = 0; i < entries; ++i)
    {
        --index->num_entries;

        /* insert into HEAD is free (head tree is in RAM) */
        ++headtree->num_entries_to_delete;

        /* merge with LVL0 */
        if (headtree->num_entries + headtree->num_entries_to_delete >= headtree->max_entries)
            time += db_index_fdtree_merge_headtree(index);
    }

    db_stat_update_query_time(time);
    return time;
}

double db_index_fdtree_update(DB_index_fdtree *index, size_t entries)
{
    double time = 0.0;

    for (size_t i = 0; i < entries; ++i)
    {
        time += db_index_fdtree_delete(index, 1);
        time += db_index_fdtree_insert(index, 1);
    }

    return time;
}