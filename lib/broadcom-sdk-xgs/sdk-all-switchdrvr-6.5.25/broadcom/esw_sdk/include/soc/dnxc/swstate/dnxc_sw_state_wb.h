/** \file dnxc_sw_state_wb.h
 * This is the wb dnxc sw state module.
 * Module is implementing the wb access itself.
 * wb implementation
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
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
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
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

typedef enum
{
    SW_STATE_ALLOC_ELEMENT_POINTER,
    SW_STATE_ALLOC_ELEMENT_MUTEX,
    SW_STATE_ALLOC_ELEMENT_SEM,
} dnxc_sw_state_alloc_element_type_e;

typedef struct dnxc_sw_state_alloc_element_s
{
    uint32 ptr_offset;
    uint8 *ptr_value;
    dnxc_sw_state_alloc_element_type_e type;

} dnxc_sw_state_alloc_element_t;

typedef struct dnxc_sw_state_data_block_header_s
{
    uint8 is_init;
    uint32 total_buffer_size;
    uint32 data_size;
    uint32 size_left;
    uint32 module_sizes[NOF_MODULE_ID];

    void *data_ptr;
    void *next_free_data_slot;
    dnxc_sw_state_alloc_element_t *ptr_offsets_sp;      /* stack pointer */
    dnxc_sw_state_alloc_element_t *ptr_offsets_stack_base;
    sal_mutex_t alloc_mtx;
    void **roots_array;
} dnxc_sw_state_data_block_header_t;

uint8 dnxc_sw_state_data_block_header_is_init(
    int unit);

uint8 dnxc_sw_state_data_block_is_in_range(
    int unit,
    uint8 *ptr);

uint8 dnxc_sw_state_data_block_is_enough_space(
    int unit,
    uint32 size);

int dnxc_sw_state_wb_sizes_get(
    int unit,
    uint32 *in_use,
    uint32 *left);

int dnxc_sw_state_data_block_pointers_stack_push(
    int unit,
    uint8 **ptr_location,
    dnxc_sw_state_alloc_element_type_e type);

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

int dnxc_sw_state_wb_start_size_measurement_point(
    int unit);

uint32 dnxc_sw_state_wb_size_measurement_get(
    int unit);

/**
 * \brief
 * Get pointer from a given offset
 * \param [in] unit     - Module Id
 * \param [in] offset   - Offset used calculate the pointer
 * \return
 *   uint8* - Calculated pointer
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 *dnxc_sw_state_wb_calc_pointer(
    int unit,
    sal_vaddr_t offset);

/**
 * \brief
 * Get double pointer from a given offset
 * \param [in] unit     - Module Id
 * \param [in] offset   - Offset used calculate the pointer
 * \return
 *   uint8** - Calculated double pointer
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 **dnxc_sw_state_wb_calc_double_pointer(
    int unit,
    sal_vaddr_t offset);

/**
 * \brief
 * Get offset from a given pointer
 * \param [in] unit     - Module Id
 * \param [in] ptr   - Pointer used calculate the offset
 * \return
 *   uint32 - Calculated offset
 * \remark
 *   * None
 * \see
 *   * None
 */
sal_vaddr_t dnxc_sw_state_wb_calc_offset(
    int unit,
    uint8 *ptr);

/**
 * \brief
 * Get offset from a given pointer
 * \param [in] unit     - Module Id
 * \param [in] ptr   - Doublepointer used calculate the offset
 * \return
 *   uint32 - Calculated offset
 * \remark
 *   * None
 * \see
 *   * None
 */
sal_vaddr_t dnxc_sw_state_wb_calc_offset_from_dptr(
    int unit,
    uint8 **ptr);

/**
 *  \brief
 * Updating the size of each module
 *  \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] allocated_size - allocation size
 * \param [in] flags - initialization flag flags.
  * \return
  *   * None
  * \remark
 *   * None
 * \see
 *   * None
 */
void dnxc_sw_state_wb_module_size_update(
    int unit,
    uint32 module_id,
    uint32 allocated_size,
    uint32 flags);
/**
 * \brief
 * Get the size of the given module
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 *  \return
 *    uint32 - Module size
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 dnxc_sw_state_wb_module_size_get(
    int unit,
    uint32 module_id);

/* } */
#endif /* _DNXC_SW_STATE_WB_H */
