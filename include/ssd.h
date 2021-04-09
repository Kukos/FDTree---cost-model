#ifndef SSD_H
#define SSD_H

/*
    SSD disk definision
    Author: Michal Kukowski
    email: michalkukowski10@gmail.com
    LICENCE GPL 3.0
*/

#include <stddef.h>

#define SSD_INT_CEIL_DIV(n, k) (((n) + (k) - 1) / (k))

typedef struct SSD
{
    /* random access time (seconds per page) */
    double r_read_time;
    double r_write_time;

    /* sequential access time (seconds per page) */
    double s_read_time;
    double s_write_time;

    /* erase time (seconds per block) */
    double erase_time;

    size_t page_size; /* in bytes */
    size_t block_size; /* in bytes */

    size_t dirty_pages;

    const char *name;
} SSD;

/*
    Create Samsung 840

    PARAMS
    NO PARAMS

    RETURN
    Pointer to new SSD
*/
SSD *ssd_create_samsung840(void);

/*
    Create Intel DC P4511

    PARAMS
    NO PARAMS

    RETURN
    Pointer to new SSD
*/
SSD *ssd_create_intelDCP4511(void);

/*
    Create Toshiba VX500

    PARAMS
    NO PARAMS

    RETURN
    Pointer to new SSD
*/
SSD *ssd_create_toshibaVX500(void);

/*
    Destroy SSD

    PARAMS
    @IN ssd - pointer to SSD

    RETURN
    This is a void function
*/
void ssd_destroy(SSD *ssd);

/*
    Get number of pages per block in SSD

    PARAMS
    @IN ssd - pointer to SSD

    RETURN
    Number of pages per block
*/
static inline size_t ssd_pages_per_block(SSD *ssd);

/*
    Read in random way pages from SSD.
    Use it if you are assuming that you know only 1 page adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN pages - number of pages to read

    RETURN
    Time spent on reading
*/
static inline double ssd_rread_pages(SSD *ssd, size_t pages);

/*
    Read in random way bytes from SSD.
    Use it if you are assuming that you know only 1 page adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN bvtes - number of bytes to read

    RETURN
    Time spent on reading
*/
static inline double ssd_rread(SSD *ssd, size_t bytes);

/*
    Read in sequential way pages from SSD.
    Use it if you are assuming that you know >1 adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN pages - number of pages to read

    RETURN
    Time spent on reading
*/
static inline double ssd_sread_pages(SSD *ssd, size_t pages);

/*
    Read in sequential way bytes from SSD.
    Use it if you are assuming that you know >1 adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN bytes - number of bytes to read

    RETURN
    Time spent on reading
*/
static inline double ssd_sread(SSD *ssd, size_t bytes);

/*
    Erase block (use only when you need erase block without writing or updating).
    NOTE: Update has own erasing if dirty pages are full

    PARAMS
    @IN ssd - pointer to SSD
    @IN blocks - number of blocks to erase

    RETURN
    Time spent on erasing
*/
static inline double ssd_erase_blocks(SSD *ssd, size_t blocks);

/*
    Write in random way pages on SSD.
    Use it if you are assuming that you know only 1 page adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN pages - number of pages to write

    RETURN
    Time spent on writing
*/
static inline double ssd_rwrite_pages(SSD *ssd, size_t pages);

/*
    Write in random way bytes on SSD.
    Use it if you are assuming that you know only 1 page adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN bytes - number of bytes to write

    RETURN
    Time spent on writing
*/
static inline double ssd_rwrite(SSD *ssd, size_t bytes);

/*
    Write in sequential way pages on SSD.
    Use it if you are assuming that you know >1 adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN pages - number of pages to write

    RETURN
    Time spent on writing
*/
static inline double ssd_swrite_pages(SSD *ssd, size_t pages);

/*
    Write in sequential way bytes on SSD.
    Use it if you are assuming that you know >1 adress in time

    PARAMS
    @IN ssd - pointer to SSD
    @IN bytes - number of bytes to write

    RETURN
    Time spent on writing
*/
static inline double ssd_swrite(SSD *ssd, size_t bytes);

/*
    Clean dirty pages (use when you need to reset SSD)
    NOTE Update has own cleaing

    PARAMS
    @IN ssd - pointer to SSD

    RETURN
    Time spent on cleaning
*/
static inline double ssd_clean_dirty_pages(SSD *ssd);

/*
    Update data on pages

    PARAMS
    @IN ssd - pointer to SSD
    @IN pages - number of pages to update

    RETURN
    Time spent on updating
*/
static inline double ssd_update_pages(SSD *ssd, size_t pages);

/*
    Update data on bytes

    PARAMS
    @IN ssd - pointer to SSD
    @IN bytes - number of bytes to update

    RETURN
    Time spent on updating
*/
static inline double ssd_update(SSD *ssd, size_t bytes);

static inline size_t ssd_pages_per_block(SSD *ssd)
{
    return ssd->block_size / ssd->page_size;
}

static inline double ssd_rread_pages(SSD *ssd, size_t pages)
{
    const double time = ssd->r_read_time * (double)pages;
    return time;
}

static inline double ssd_rread(SSD *ssd, size_t bytes)
{
    return ssd_rread_pages(ssd, SSD_INT_CEIL_DIV(bytes, ssd->page_size));
}

static inline double ssd_sread_pages(SSD *ssd, size_t pages)
{
    const double time = ssd->s_read_time * (double)pages;
    return time;
}

static inline double ssd_sread(SSD *ssd, size_t bytes)
{
    return ssd_sread_pages(ssd, SSD_INT_CEIL_DIV(bytes, ssd->page_size));
}

static inline double ssd_erase_blocks(SSD *ssd, size_t blocks)
{
    const double time = ssd->erase_time * (double)blocks;
    return time;
}

static inline double ssd_rwrite_pages(SSD *ssd, size_t pages)
{
    const double time = ssd->r_write_time * (double)pages;
    return time;
}

static inline double ssd_rwrite(SSD *ssd, size_t bytes)
{
    return ssd_rwrite_pages(ssd, SSD_INT_CEIL_DIV(bytes, ssd->page_size));
}

static inline double ssd_swrite_pages(SSD *ssd, size_t pages)
{
    const double time = ssd->s_write_time * (double)pages;
    return time;
}

static inline double ssd_swrite(SSD *ssd, size_t bytes)
{
    return ssd_swrite_pages(ssd, SSD_INT_CEIL_DIV(bytes, ssd->page_size));
}


static inline double ssd_clean_dirty_pages(SSD *ssd)
{
    const size_t blocks = (ssd->dirty_pages + ssd_pages_per_block(ssd) - 1) / ssd_pages_per_block(ssd);
    const double time = ssd_erase_blocks(ssd, blocks);
    ssd->dirty_pages = 0;

    return time;
}

static inline double ssd_update_pages(SSD *ssd, size_t pages)
{
    double time = 0.0;

    /* read pages to bufor */
    time += ssd_rread_pages(ssd, pages);

    /* mark as dirty */
    ssd->dirty_pages += pages;

    /* write with changes to block / another block */
    time += ssd_rwrite_pages(ssd, pages);

    /* clean if needed */
    if (ssd->dirty_pages >= ssd_pages_per_block(ssd))
        time += ssd_clean_dirty_pages(ssd);

    return time;
}

static inline double ssd_update(SSD *ssd, size_t bytes)
{
    return ssd_update_pages(ssd, SSD_INT_CEIL_DIV(bytes, ssd->page_size));
}


#endif