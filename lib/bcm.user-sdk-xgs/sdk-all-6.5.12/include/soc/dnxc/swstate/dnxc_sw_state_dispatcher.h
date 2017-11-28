/** \file dnxc_sw_state_dispatcher.h
 * This contains the sw state dispatcher
 * Auto-generated functions utilize this module in order to access plain or wb implementation.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_DISPATCHER_H
/* { */
#define _DNXC_SW_STATE_DISPATCHER_H

/*
 * Include files
 * {
 */
#include <sal/core/sync.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

typedef enum
{
    DNXC_SW_STATE_IMPLEMENTATION_PLAIN,
    DNXC_SW_STATE_IMPLEMENTATION_WB
} dnxc_sw_state_implementation_e;

/**
 * implemented by: dnxc_sw_state_init_plain
 */
typedef int (
    *dnxc_sw_state_init_cb) (
    int unit,
    uint32 flags);

/**
 * implemented by: dnxc_sw_state_deinit_plain_plain
 */
typedef int (
    *dnxc_sw_state_deinit_cb) (
    int unit,
    uint32 flags);

/**
 * implemented by: dnxc_sw_state_module_init_plain
 */
typedef int (
    *dnxc_sw_state_module_init_cb) (
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_module_deinit_plain
 */
typedef int (
    *dnxc_sw_state_module_deinit_cb) (
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_alloc_plain
 */
typedef int (
    *dnxc_sw_state_alloc_cb) (
    int unit,
    uint32 module_id,
    uint8 **ptr_to_ptr,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_free_plain
 */
typedef int (
    *dnxc_sw_state_free_cb) (
    int unit,
    uint32 module_id,
    uint8 **ptr_to_ptr,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_memcpy_plain
 */
typedef int (
    *dnxc_sw_state_memcpy_cb) (
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_memset_plain
 */
typedef int (
    *dnxc_sw_state_memset_cb) (
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_alloc_size_plain
 */
typedef int (
    *dnxc_sw_state_alloc_size_cb) (
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *nof_elements,
    uint32 *element_size);

/**
 * implemented by: dnx_sw_state_counter_inc_plain
 */
typedef int (
    *dnx_sw_state_counter_inc_cb) (
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnx_sw_state_counter_dec_plain
 */
typedef int (
    *dnx_sw_state_counter_dec_cb) (
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

/**
 * implemented by: dnxc_sw_state_mutex_create_plain
 */
typedef int (
    *dnxc_sw_state_mutex_create_cb) (
    int unit,
    uint32 module_id,
    sal_mutex_t * mtx,
    char *desc,
    uint32 flags);

/**
* implemented by: dnxc_sw_state_mutex_destroy
*/
typedef int (
    *dnxc_sw_state_mutex_destroy_cb) (
    int unit,
    uint32 module_id,
    sal_mutex_t * ptr_mtx,
    uint32 flags);

/**
* implemented by: dnxc_sw_state_mutex_take
*/
typedef int (
    *dnxc_sw_state_mutex_take_cb) (
    int unit,
    uint32 module_id,
    sal_mutex_t mtx,
    int usec,
    uint32 flags);

/**
* implemented by: dnxc_sw_state_mutex_give
*/
typedef int (
    *dnxc_sw_state_mutex_give_cb) (
    int unit,
    uint32 module_id,
    sal_mutex_t mtx,
    uint32 flags);

/*
 * }
 */

/*
 * STRUCTs
 * {
 */
typedef struct
{
    dnxc_sw_state_module_init_cb module_init;
    dnxc_sw_state_module_deinit_cb module_deinit;
    dnxc_sw_state_alloc_cb alloc;
    dnxc_sw_state_free_cb free;
    dnxc_sw_state_alloc_size_cb alloc_size;
    dnxc_sw_state_memcpy_cb memcpy;
    dnxc_sw_state_memset_cb memset;
    dnxc_sw_state_mutex_create_cb mutex_create;
    dnxc_sw_state_mutex_destroy_cb mutex_destroy;
    dnxc_sw_state_mutex_take_cb mutex_take;
    dnxc_sw_state_mutex_give_cb mutex_give;
    dnx_sw_state_counter_inc_cb counter_inc;
    dnx_sw_state_counter_dec_cb counter_dec;
} dnxc_sw_state_dispatcher_cb;

/*
 * }
 */

/*
 * EXTERNs
 * {
 */
extern dnxc_sw_state_dispatcher_cb dnxc_sw_state_dispatcher[SOC_MAX_NUM_DEVICES][NOF_MODULE_ID];
/*
 * }
 */

/**
 * \brief
 *  Initialize dnxc sw state dispatcher module
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] implementation - Choose plain or wb implementation
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_dispatcher_module_init(
    int unit,
    uint32 module_id,
    dnxc_sw_state_implementation_e implementation);

/**
 * \brief
 *  Deinitialize dnxc sw state dispatcher module
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_dispatcher_module_deinit(
    int unit);

/* } */
#endif /* _DNXC_SW_STATE_DISPATCHER_H */
