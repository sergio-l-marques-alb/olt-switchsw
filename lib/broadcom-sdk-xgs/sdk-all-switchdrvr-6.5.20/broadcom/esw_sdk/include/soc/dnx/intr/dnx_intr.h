/** \file soc/dnx/intr/dnx_intr.h
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SOC_DNX_DNX_INTR_H_INCLUDE
/*
 * { 
 */
#define SOC_DNX_DNX_INTR_H_INCLUDE

#if !defined(BCM_DNX_SUPPORT)
#error "This file is for use by DNX family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <sal/types.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/mem.h>
#include <soc/intr.h>
#include <soc/defs.h>
#include <soc/iproc.h>
/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */
/*
 * MACROs:
 * {
 */
/*
 * }
 */

/** 
 * \brief 
 * Callback to get custom default value. 
 * For simple custom mode all the input parameters should be ignored (one table for all the memory)
 */
typedef int (
    *dnx_intr_exceptional_get_f) (
    int unit,
    uint32 *entry_data);

/**
 * \brief - get entry_data if dram is present
 *
 * \param [in] unit - unit number
 * \param [in] is_exception - returned value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_intr_exceptional_dram_not_present(
    int unit,
    uint32 *is_exception);

/*
 * Function:
 *    soc_dnx_interrupt_enable
 * Description:
 *    Enable interruupt with some exception
 * Parameters:
 *    unit              - Device unit number
 *    block_instance    - interrupt block instance.
 *    inter             - interrupt db
 * Returns:
 *      _SHR_E_xxx
 */
int soc_dnx_interrupt_enable(
    int unit,
    int intr_id,
    int block_instance,
    const soc_interrupt_db_t * inter);

int soc_dnx_int_name_to_id(
    int unit,
    char *name);
int soc_dnx_interrupts_array_init(
    int unit);
int soc_dnx_interrupts_array_deinit(
    int unit);
int soc_dnx_interrupt_cb_init(
    int unit);
int soc_dnx_is_block_eci_intr_assert(
    int unit,
    int blk,
    soc_reg_above_64_val_t eci_interrupt);
int soc_dnx_ser_init(
    int unit);

/*
 * Function:
 *    soc_dnx_is_interrupt_support
 * Description:
 *    If interruupt is supported
 * Parameters:
 *    unit              - Device unit number
 *    intr_id           - interrupt id
 *    is_support        - if interrupt support
 * Returns:
 *      _SHR_E_xxx
 */
int soc_dnx_is_interrupt_support(
    int unit,
    int intr_id,
    int *is_support);

/**
 * \brief
 *  Enable/Disable kaps tcam ecc protect machine
 *
 * \param [in] unit - unit
 * \param [in] is_init - pass TRUE if init, else FALSE. Used to init swstate.
 * \param [in] enable - pass TRUE to enable, FALSE to pause
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_ser_kaps_tcam_scan_machine_enable(
    int unit,
    int is_init,
    int enable);

#endif /* SOC_DNX_DNX_INTR_H_INCLUDE */
