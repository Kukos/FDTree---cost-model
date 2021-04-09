#ifndef DBSTAT_H
#define DBSTAT_H

/*
    Data base statistics included total time consumptions, current query time
    Author: Michal Kukowski
    email: michalkukowski10@gmail.com
    LICENCE GPL 3.0
*/

#include <stddef.h>
#include <sys/types.h>

typedef struct DB_snapshot
{
    /* time in seconds */
    double query_time;
} DB_snapshot;

extern DB_snapshot db_current_query;
extern DB_snapshot db_total;

/*
    Private function, do not use directly

    PARAMS
    @IN sh - pointer to snapshot
    RETURN

    Sum of all fields related to time
*/
static inline double __db_stat_get_time(DB_snapshot *sh);


static inline double __db_stat_get_time(DB_snapshot *sh)
{
    return  sh->query_time;
}

/*
    Reset whole DB Stat

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_reset(void);

/*
    Reset only current query statictics

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_reset_query(void);

/*
    Start new query

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_start_query(void);

/*
    Finish current query and update total statistics

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_finish_query(void);

/*
    Print on stdout info about current query

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_current_print(void);

/*
    Print on stdout summary info

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_summary_print(void);

/*
    Update statistics  for current query

    PARAMS
    @IN s - seconds

    RETURN
    This is a void function
*/
static inline void db_stat_update_query_time(double s);

/*
    Get query time  current query and total time

    PARAMS
    NO PARAMS

    RETURN
    Query time
*/
static inline double db_stat_get_current_time(void);
static inline double db_stat_get_total_time(void);


static inline void db_stat_update_query_time(double s)
{
    db_current_query.query_time += s;
}

static inline double db_stat_get_current_time(void)
{
    return __db_stat_get_time(&db_current_query);
}

static inline double db_stat_get_total_time(void)
{
    return __db_stat_get_time(&db_total);
}

#endif