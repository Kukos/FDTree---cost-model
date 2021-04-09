#include <ssd.h>
#include <stdlib.h>

#define SSD_MICROSEC(n) ((n) / 1000000.0)

SSD *ssd_create_samsung840(void)
{
    SSD *ssd;
    const size_t pages_per_block = 64;
    const size_t page_size = 8192;

    ssd = (SSD *)malloc(sizeof(SSD));
    if (ssd == NULL)
        return NULL;

    ssd->block_size = pages_per_block * page_size;
    ssd->dirty_pages = 0;
    ssd->page_size = page_size;
    ssd->r_read_time = SSD_MICROSEC(21);
    ssd->r_write_time = SSD_MICROSEC(45);
    ssd->s_read_time = SSD_MICROSEC(14);
    ssd->s_write_time = SSD_MICROSEC(15.3);
    ssd->erase_time = ssd->r_write_time * 10.0 * (double)pages_per_block;
    ssd->name = "samsung840";

    return ssd;
}

SSD *ssd_create_intelDCP4511(void)
{
    SSD *ssd;
    const size_t pages_per_block = 64;
    const size_t page_size = 4096;

    ssd = (SSD *)malloc(sizeof(SSD));
    if (ssd == NULL)
        return NULL;

    ssd->block_size = pages_per_block * page_size;
    ssd->dirty_pages = 0;
    ssd->page_size = page_size;
    ssd->r_read_time = SSD_MICROSEC(3.3);
    ssd->r_write_time = SSD_MICROSEC(27.7);
    ssd->s_read_time = SSD_MICROSEC(2);
    ssd->s_write_time = SSD_MICROSEC(2.75);
    ssd->erase_time = ssd->r_write_time * 10.0 * (double)pages_per_block;
    ssd->name = "intelDCP4511";

    return ssd;
}

SSD *ssd_create_toshibaVX500(void)
{
    SSD *ssd;
    const size_t pages_per_block = 64;
    const size_t page_size = 4096;

    ssd = (SSD *)malloc(sizeof(SSD));
    if (ssd == NULL)
        return NULL;

    ssd->block_size = pages_per_block * page_size;
    ssd->dirty_pages = 0;
    ssd->page_size = page_size;
    ssd->r_read_time = SSD_MICROSEC(10.8);
    ssd->r_write_time = SSD_MICROSEC(15.3);
    ssd->s_read_time = SSD_MICROSEC(7.2);
    ssd->s_write_time = SSD_MICROSEC(7.8);
    ssd->erase_time = ssd->r_write_time * 10.0 * (double)pages_per_block;
    ssd->name = "toshibaVX500";

    return ssd;
}

void ssd_destroy(SSD *ssd)
{
    if (ssd == NULL)
        return;

    free(ssd);
}