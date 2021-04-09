#ifndef DBINDEX_FDTREE_H
#define DBINDEX_FDTREE_H

/*
    FDTree Cost Model

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com
    LICENCE GPL 3.0
*/


#include <stddef.h>
#include <sys/types.h>
#include <ssd.h>

#define DBINDEX_FDTREE_MAX_LVL    10
#define DBINDEX_FDTREE_RUNS_RATIO 50

typedef struct FDLvl
{
    size_t num_entries;
    size_t num_entries_to_delete;
    size_t max_entries;
} FDLvl;

typedef struct FDHead
{
    size_t num_entries;
    size_t num_entries_to_delete;
    size_t max_entries;
} FDHead;

typedef struct DB_index_fdtree
{
    size_t num_entries;
    size_t entry_size; /* in bytes */
    size_t key_size; /* in bytes */
    size_t height;
    size_t runs_ratio;

    SSD* ssd;

    FDHead headtree;
    FDLvl sortedruns[DBINDEX_FDTREE_MAX_LVL];
} DB_index_fdtree;


/*
    Create empty index

    PARAMS
    @IN SSD - ssd
    @IN key_size - size of key in Bytes
    @IN entry_size - size of entry in Bytes

    RETURN
    Pointer to new index
*/
DB_index_fdtree *db_index_fdtree_create(SSD *ssd, size_t key_size, size_t entry_size, size_t runs_ratio);

/*
    Destroy index

    PARAMS
    @IN index - pointer to index

    RETURN
    This is a void function
*/
void db_index_fdtree_destroy(DB_index_fdtree *index);

/*
    Insert entries to index

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to insert

    RETURN
    Insert time
*/
double db_index_fdtree_insert(DB_index_fdtree *index, size_t entries);

/*
    Insert entries via bulkload method

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to insert

    RETURN
    Insert time
*/
double db_index_fdtree_bulkload(DB_index_fdtree *index, size_t entries);

/*
    Find entries by point search

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to find

    RETURN
    Search time
*/
double db_index_fdtree_point_search(DB_index_fdtree *index, size_t entries);

/*
    Find entries by range search

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to find

    RETURN
    Search time
*/
double db_index_fdtree_range_search(DB_index_fdtree *index, size_t entries);

/*
    Delete entries from index

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to delete

    RETURN
    Delete time
*/
double db_index_fdtree_delete(DB_index_fdtree *index, size_t entries);

/*
    Update entries in index

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to update

    RETURN
    Update Time
*/
double db_index_fdtree_update(DB_index_fdtree *index, size_t entries);

#endif