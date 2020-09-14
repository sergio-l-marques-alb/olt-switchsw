/** \file dnxc_sw_state_common.h
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_COMMON_H
/* { */
#define _DNXC_SW_STATE_COMMON_H

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
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/recovery/rollback_journal_utils.h>
#endif /* BCM_DNX_SUPPORT */
/*
 * }
 */

/**
 * \brief
 *  Copy memory data to destination sw state location
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
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
int dnxc_sw_state_memcpy_common(
    int unit,
    uint32 module_id,
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
int dnxc_sw_state_memset_common(
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
int dnxc_sw_state_counter_inc_common(
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
int dnxc_sw_state_counter_dec_common(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Get size allocated
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id
 * \param [in] ptr     - Pointer to data allocated
 * \param [out] allocated_size     - allocation size retrieved
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_dynamic_allocated_size_get(
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *allocated_size);

/* } */
#endif /* _DNXC_SW_STATE_COMMON_H */
