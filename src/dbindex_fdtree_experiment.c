#include <dbindex_fdtree.h>
#include <experiments.h>
#include <dbstat.h>
#include <math.h>

#define LOG2(n) floor(((log((double)n)) / (log(2.0))))

void db_index_fdtree_experiment_workload(size_t queries)
{
    DB_index_fdtree *index;
    SSD *ssd;
    size_t i;
    double _sqrt_n = ceil(sqrt((double)queries));
    size_t sqrt_n = (size_t)_sqrt_n;
    size_t nlogn = (size_t)((double)queries * LOG2(queries));

    ssd = ssd_create_samsung840();
    index = db_index_fdtree_create(ssd, sizeof(long), 140, DBINDEX_FDTREE_RUNS_RATIO);
    db_stat_reset();

    /* bukload N / 2 */
    db_stat_start_query();
    db_index_fdtree_bulkload(index, queries / 2);
    db_stat_finish_query();

    /* sqrt(N) point search */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_point_search(index, 1);
        db_stat_finish_query();
    }

    /* Normal Insert N / 2 */
    for (i = 0; i < queries / 2; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_insert(index, 1);
        db_stat_finish_query();
    }

    /* NlogN range search with 10% selecivity */
    for (i = 0; i < nlogn; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_range_search(index, (index->num_entries + 9) / 10);
        db_stat_finish_query();
    }

    /* sqrt(N) delete */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_delete(index, 1);
        db_stat_finish_query();
    }

    /* sqrt(N) point search */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_point_search(index, 1);
        db_stat_finish_query();
    }

    /* sqrt(N) update */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_update(index, 1);
        db_stat_finish_query();
    }

    /* NlogN range search with 5% selectivity */
    for (i = 0; i < nlogn; ++i)
    {
        db_stat_start_query();
        db_index_fdtree_range_search(index, (index->num_entries + 19) / 20);
        db_stat_finish_query();
    }

    db_stat_summary_print();
    db_index_fdtree_destroy(index);
    ssd_destroy(ssd);
}