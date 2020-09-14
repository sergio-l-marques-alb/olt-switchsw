/** \file dnxc_sw_state_plain.h
 * This is the main dnxc sw state module.
 * Module is implementing the access itself.
 * Plain implementation means that it is only access, without 
 * warmboot hooks or any other hooks. Note that Error recovery 
 * hooks always exist for all implementations, also in plain 
 * implementation. 
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_PLAIN_H
/* { */
#define _DNXC_SW_STATE_PLAIN_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

#define DNXC_SW_STATE_START_SIZE_MEASURMENT(unit)\
    SHR_IF_ERR_EXIT(dnxc_sw_state_plain_start_size_measurement_point(unit))

#define DNXC_SW_STATE_SIZE_MEASURMENT_GET(unit)\
    dnxc_sw_state_plain_size_measurement_get(unit)

typedef enum
{
    DNXC_SW_STATE_DLL_POINTER,
    DNXC_SW_STATE_DLL_MUTEX,
    DNXC_SW_STATE_DLL_SEM
} dnxc_sw_state_dll_entry_type_e;

typedef struct dnxc_sw_state_allocation_dll_entry_s
{
    uint32 start_santinel;
    struct dnxc_sw_state_allocation_dll_entry_s *prev;
    struct dnxc_sw_state_allocation_dll_entry_s *next;
    uint8 *ptr;
    uint8 **ptr_location;
    dnxc_sw_state_dll_entry_type_e type;
    uint32 end_santinel;
} dnxc_sw_state_allocation_dll_entry_t;

typedef struct dnxc_sw_state_allocation_dll_s
{
    uint32 entry_counter;
    dnxc_sw_state_allocation_dll_entry_t *head;
    dnxc_sw_state_allocation_dll_entry_t *tail;
} dnxc_sw_state_allocation_dll_t;

typedef struct dnxc_sw_state_allocation_data_prefix_s
{
    uint32 start_santinel;
    dnxc_sw_state_allocation_dll_entry_t *dll_entry;
    uint32 nof_elements;
    uint32 element_size;
} dnxc_sw_state_allocation_data_prefix_t;

typedef struct dnxc_sw_state_allocation_data_suffix_s
{
    uint32 end_santinel;
} dnxc_sw_state_allocation_data_suffix_t;

/**
 * \brief
 *  used to remove entry from the double ended linked list of
 *  allocated pointers, that stores all of the sw state pointers
 *  and mutexes
 * \param [in] unit - Device Id
 * \param [in] ptr_location - a pointer to the freed pointer.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_dll_entry_free_by_ptr_location(
    int unit,
    uint8 **ptr_location);

/**
 * \brief
 *  used to add entry to the double ended linked list of
 *  allocated pointers, that stores all of the sw state pointers
 *  and mutexes
 * \param [in] unit - Device Id
 * \param [in] ptr_location - a pointer to the freed pointer. 
 * \param [in] ptr_prefix - prefix for the pointer.
 * \param [in] type - pointer or mutex.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_dll_entry_add(
    int unit,
    uint8 **ptr_location,
    dnxc_sw_state_allocation_data_prefix_t * ptr_prefix,
    dnxc_sw_state_dll_entry_type_e type);

/**
 * \brief
 *  Initialize dnxc sw state for plain implementation
 * \param [in] unit - Device Id
 * \param [in] flags - initialization flag flags.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_init_plain(
    int unit,
    uint32 flags);

/**
 * \brief
 *  Deinitialize dnxc sw state for plain implementation
 * \param [in] unit - Device Id
 * \param [in] flags - initialization flag flags.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_deinit_plain(
    int unit,
    uint32 flags);

/**
 * \brief
 *  Initialize a sw state module with plain implementation
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
int dnxc_sw_state_module_init_plain(
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Deinitialize a sw state module with plain implementation
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
int dnxc_sw_state_module_deinit_plain(
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state using plain
 *  implementation
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
int dnxc_sw_state_alloc_plain(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Free memory in the sw state using plain implementation
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
int dnxc_sw_state_free_plain(
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
int dnxc_sw_state_alloc_size_plain(
    int unit,
    uint32 module,
    uint8 *ptr,
    uint32 *nof_elements,
    uint32 *element_size);

/**
 * \brief
 *  Start measuring point for the sw state size.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * This function is not thread safe. it is assumed
 *     allocations are done only at init only by main thread.
 * \see
 *   * None
 */
int dnxc_sw_state_plain_start_size_measurement_point(
    int unit);

/**
 * \brief
 *  Returns size of the allocated sw state between size_mesurement_point and the current moment.
 * \param [in] unit - Device Id
 * \return
 *   uint32
 * \remark
 *   * This function is not thread safe. it is assumed
 *     allocations are done only at init only by main thread.
 * \see
 *   * None
 */
uint32 dnxc_sw_state_plain_size_measurement_get(
    int unit);

/**
 * \brief
 *  For diagnostic sake, return the total memory consumption of
 *  the sw state.
 * \param [in]  unit - Device Id 
 * \param [out] size - the total size
 * \return
 *   uint32
 * \see
 *   * None
 */
int dnxc_sw_state_total_size_get_plain(
    int unit,
    uint32 *size);

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
uint8 *dnxc_sw_state_plain_calc_pointer(
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
uint8 **dnxc_sw_state_plain_calc_double_pointer(
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
sal_vaddr_t dnxc_sw_state_plain_calc_offset(
    int unit,
    uint8 *ptr);

/**
 * \brief
 * Get offset from a given pointer
 * \param [in] unit     - Module Id
 * \param [in] ptr   - Doubleointer used calculate the offset
 * \return
 *   uint32 - Calculated offset
 * \remark
 *   * None
 * \see
 *   * None
 */
sal_vaddr_t dnxc_sw_state_plain_calc_offset_from_dptr(
    int unit,
    uint8 **ptr);

/* } */
#endif /* _DNXC_SW_STATE_PLAIN_H */
