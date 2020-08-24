/*
 * $Id: sand_mem.h,v $
 * 
 * $Copyright: (c) 2019 Broadcom.
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
 * Fill the whole table with the given entry, uses fast DMA filling when run on real hardware.
 * Do not check errors in emulation
 */
uint32 sand_fill_table_with_entry_skip_emul_err(
    const int       unit,
    const soc_mem_t mem,        /* memory/table to fill */
    const int       copyno,     /* Memory/table block to fill */
    const void      *entry_data /* The contents of the entry to fill the table with. Does not have to be DMA memory */
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


/*  check if dnx table is dynamic */
int sand_tbl_is_dynamic(int unit,soc_mem_t mem);

/* check if a given mem contain one of the fields apear in given list*/
int sand_mem_contain_one_of_the_fields(int unit,const soc_mem_t mem,soc_field_t *fields);

/* 
 * This function reads from all cached memories in order to detect and fix SER errors
 */
int soc_sand_cache_table_update_all(int unit);

/*
 * Structures and prototypes related to PEM block access.
 */
int sand_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
                  int index, int count, uint32 *entry_ptr);

/*
 * Enable/disable dynamic memory writes using the given dynamic memory write control registers
 */
int soc_sand_control_dynamic_mem_writes(
    const int unit,
    const soc_reg_t *regs, /* control registers to write to, terminated by INVALIDr */
    const uint32 val); /* value (0/1) to write to the registers */
/* Check if a given memory disabled cache or enabled cache by specific */
int soc_sand_mem_is_in_soc_property(int unit, soc_mem_t mem, int en);

/* enable/disable cache shadow of a memory table */
int sand_tbl_mem_cache_mem_set(int unit, soc_mem_t mem, void* en);

/**
* \brief - Polling until last indriect command over the specified block is done
*
* \param [in] unit - unit #
* \param [in] blk_id - specify the required block
* \param [in] time_out - maximum time to wait for operation done - usec
* \return
*   See shr_error_e
* \remark
*   * None
* \see
*   * None
*/
int
sand_mem_indirect_poll_done(
        int unit,
        int blk_id,
        int time_out);
/**
* \brief - Perform indrect write to a table.
*          This function used to initialize all device memories simultaneously
*          (Making sure last operation done, but do not wait to current operation done)
*
* \param [in] unit - unit #
* \param [in] mem - mem #
* \param [in] blk_id - specify the required block
* \param [in] array_index - array index of memory
* \param [in] entry_index - entry index of memory
* \param [in] nof_entries - nof entries to set
* \param [in] value - value of a single entry
* \return
*   See shr_error_e
* \remark
*   * None
* \see
*   * None
*/
int
sand_mem_indirect_reset_write(
        int unit,
        soc_mem_t mem,
        int blk_id,
        int array_index,
        int entry_index,
        int nof_entries,
        uint32 *value);

#endif /*_SOC_SAND_MEM_H*/
