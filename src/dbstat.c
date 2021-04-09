#include <dbstat.h>
#include <string.h>
#include <stdio.h>

DB_snapshot db_current_query;
DB_snapshot db_total;

/*
    Print on stdout info about snapshot

    PARAMS
    @IN sh - pointer to snapshot

    RETURN
    This is a void function
*/
static inline void __db_stat_print(DB_snapshot *sh);

static inline void __db_stat_print(DB_snapshot *sh)
{
    printf("\tQUERY         TIME     = %lfs\n", sh->query_time);
    printf("\tTOTAL         TIME     = %lfs\n", __db_stat_get_time(sh));
}

void db_stat_reset(void)
{
    (void)memset(&db_current_query, 0, sizeof(db_current_query));
    (void)memset(&db_total, 0, sizeof(db_total));
}

void db_stat_reset_query(void)
{
    (void)memset(&db_current_query, 0, sizeof(db_current_query));
}

void db_stat_start_query(void)
{
    db_stat_reset_query();
}

void db_stat_finish_query(void)
{
    /* update total */
    db_total.query_time += db_current_query.query_time;
}

void db_stat_current_print(void)
{
    printf("CURRENT QUERY\n");
    __db_stat_print(&db_current_query);
}

void db_stat_summary_print(void)
{
    printf("TOTAL\n");
    __db_stat_print(&db_total);
}