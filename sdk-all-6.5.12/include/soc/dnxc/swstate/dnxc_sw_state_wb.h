/** \file dnxc_sw_state_wb.h
 * This is the wb dnxc sw state module.
 * Module is implementing the wb access itself.
 * wb implementation
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_WB_H
/* { */
#define _DNXC_SW_STATE_WB_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/scache.h>
/*
 * }
 */

typedef enum
{
    DNXC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK
} DNXC_SW_STATE_SCACHE_HANDLES;

typedef enum dnxc_sw_state_data_block_init_mode_e
{
    socDnxcSwStateDataBlockRegularInit,
    socDnxcSwStateDataBlockRestoreAndOveride
} dnxc_sw_state_init_mode_t;

typedef struct dnxc_sw_state_alloc_element_s
{
    uint32 ptr_offset;
    uint8 *ptr_value;
} dnxc_sw_state_alloc_element_t;

typedef struct dnxc_sw_state_data_block_header_s
{
    uint8 is_init;
    uint32 total_buffer_size;
    uint32 data_size;
    uint32 size_left;
    void **roots_array;
    void *data_ptr;
    void *next_free_data_slot;
    dnxc_sw_state_alloc_element_t *ptr_offsets_sp;      /* stack pointer */
    dnxc_sw_state_alloc_element_t *ptr_offsets_stack_base;
} dnxc_sw_state_data_block_header_t;

/**
 * \brief
 *  Initialize dnxc sw state
 * \param [in] unit - Device Id
 * \param [in] flags - initialization flag flags. 
 * \param [in] sw_state_max_size - size to pre-allocate for sw state 
 *             use. 
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_init_wb(
    int unit,
    uint32 flags,
    uint32 sw_state_max_size);

/**
 * \brief
 *  Deinitialize dnxc sw state
 * \param [in] unit - Device Id
 * \param [in] flags - initialization flag flags.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_deinit_wb(
    int unit,
    uint32 flags);

/**
 * \brief
 *  Initialize a sw state module
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] size - allocation size
 * \param [in] flags - initialization flag flags.
 * \param [in] dbg_string - debug string.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_module_init_wb(
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Deinitialize a sw state module
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] flags - deinitialization flag flags.
 * \param [in] dbg_string - debug string.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_module_deinit_wb(
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - Module id
 * \param [out] ptr_location - Pointer to the location that holds the pointer to the data that was allocated
 *
 * \param [in] nof_elements - Number of elements to allocate
 * \param [in] element_size - Size in bytes of the element type.
 * \param [in] flags - Allocation flags.
 * \param [in] dbg_string - Allocation dbg string.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_alloc_wb(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Free memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - Module id
 * \param [out] ptr_location - The pointer to the location that holds the pointer to the data to be freed.
 * \param [in] flags - Free flags.
 * \param [in] dbg_string - Allocation dbg string.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_free_wb(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Get the allocation size of a sw state allocaiton
 * \param [in] unit - Device Id
 * \param [in] module - Module Id
 * \param [in] ptr - Pointer to sw state data allocation
 * \param [out] nof_elements - Number of elements allocated
 * \param [out] element_size - Size in bytes of the element type
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_alloc_size_wb(
    int unit,
    uint32 module,
    uint8 *ptr,
    uint32 *nof_elements,
    uint32 *element_size);

/**
 * \brief
 *  Copy memory data to destination sw state location
 * \param [in] unit - Device Id
 * \param [in] module - Module Id
 * \param [in] size - The size of the of the data to be copied
 * \param [in] dest - Pointer to the destination in the sw state
 * \param [in] src -  Pointer to the source data
 * \param [in] flags - Memcpy flags.
 * \param [in] dbg_string - Debug strings.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_memcpy_wb(
    int unit,
    uint32 module,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Memset sw state data
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] dest - Pointer to the destination in the sw state
 * \param [in] value - Byte pattern to be used
 * \param [in] size - Number of bytes to be set
 * \param [in] flags - Memset flags.
 * \param [in] dbg_string - Debug strings.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_memset_wb(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Increment a sw state counter
 * \param [in] unit - Device Id
 * \param [in] module_id - Device Id
 * \param [in] ptr_location - Pointer to sw state counter
 * \param [in] inc_value - Value with which to increment the counter
 * \param [in] type_size - Size of type of the counter
 * \param [in] flags - Counter inc flags.
 * \param [in] dbg_string - Debug strings.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_counter_inc_wb(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Decrement a sw state counter
 * \param [in] unit - Device Id
 * \param [in] module_id - Device Id
 * \param [in] ptr_location - Pointer to sw state counter
 * \param [in] dec_value - Value with which to decrement the counter
 * \param [in] type_size - Size of type of the counter
 * \param [in] flags - Counter dec flags.
 * \param [in] dbg_string - Debug strings.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_counter_dec_wb(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

/* } */
#endif /* _DNXC_SW_STATE_WB_H */
