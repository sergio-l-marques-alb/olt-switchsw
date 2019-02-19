/*
 * $Id: sand_mem.h,v $
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_SAND_MEM_H
#define _SOC_SAND_MEM_H
#include <soc/mcm/allenum.h>

#define SAND_MAX_U32S_IN_MEM_ENTRY 20

/* Allocate memory of a given size, and store its location in the given pointer */
uint32 sand_alloc_mem(
    const int unit,
    void      *mem_ptr,        /* output: Will hold the pointer to the allocated memory, must be NULL. The real type of the argument is void** is not used to avoid compilation warnings */
    const unsigned size,       /* memory size in bytes to be allocated */
    const char     *alloc_name /* name of the memory allocation, used for debugging */
);

/* deallocate memory of a given size, and store its location in the given pointer */
uint32 sand_free_mem(
    const int unit,
    void **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
);

/* deallocate memory of a given size, and store its location in the given pointer */
void sand_free_mem_if_not_null(
    const int unit,
    void **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
);


/*
 * Allocate memory of a given size, for DMA access to a given.
 * If DMA is enabled for the device, a DMA buffer will be allocated, otherwise regular memory will be allocated.
 * The allocated buffer is stored in the given pointer */

uint32 sand_alloc_dma_mem(
    const int unit,
    const uint8     is_slam,    /* If not FALSE, DMA enabled will be tested for SLAM DMA and not for table DMA */
    void            **mem_ptr,  /* Will hold the pointer to the allocated memory, must be NULL */
    const unsigned  size,       /* memory size in bytes to be allocated */
    const char      *alloc_name /* name of the memory allocation, used for debugging */
);

/* deallocate memory of a given size, and store its location in the given pointer */
uint32 sand_free_dma_mem(
    const int unit,
    const uint8 is_slam, /* If not FALSE, DMA enabled will be tested for SLAM DMA and not for table DMA */
    void  **mem_ptr      /* holds the pointer to the allocated memory, will be set to NULL */
);


/*
 * Functions to fill memories using SLAM DMA if possible, using a pre-allocated DMA
 * buffer per device, to which the given entry is copied.
 */

/* Init the dcmn fill table mechanism for a given unit */
uint32 sand_init_fill_table(
    const  int unit
);

/* De-init the dcmn fill table mechanism for a given unit */
uint32 sand_deinit_fill_table(
    const  int unit
);

/* Fill the whole table with the given entry, uses fast DMA filling when run on real hardware */
uint32 sand_fill_table_with_entry(
    const int       unit,
    const soc_mem_t mem,        /* memory/table to fill */
    const int       copyno,     /* Memory/table block to fill */
    const void      *entry_data /* The contents of the entry to fill the table with. Does not have to be DMA memory */
  );

/* Fill the specified part of the table with the given entry, uses fast DMA filling when run on real hardware */
uint32 sand_fill_partial_table_with_entry(
    const int       unit,
    const soc_mem_t mem,               /* memory/table to fill */
    const unsigned  array_index_start, /* First array index to fill */
    const unsigned  array_index_end,   /* Last array index to fill */
    const int       copyno,            /* Memory/table block to fill */
    const int       index_start,       /* First table/memory index to fill */
    const int       index_end,         /* Last table/memory index to fill */
    const void      *entry_data        /* The contents of the entry to fill the table with. Does not have to be DMA memory */
  );

/* 
 * Read or write wide memory, supports Jericho and RAMON 
 * Uses the indirect memory access registers in the memory's block,
 * and should also work for non-wide memories
 */
int sand_mem_array_wide_access(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data,
    unsigned do_read  /* do_read should be non-zero for read and 0 for write */
  );

#endif /*_SOC_SAND_MEM_H*/
