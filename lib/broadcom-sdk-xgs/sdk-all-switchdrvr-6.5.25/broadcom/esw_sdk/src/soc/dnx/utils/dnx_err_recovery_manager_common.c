/** \file dnx_err_recovery_manager_common.c
 * This module is a common module between
 * the Error Recovery manager and
 * the State Snapshot manager.
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <soc/drv.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY


dnx_err_recovery_common_root_t dnx_err_recovery_common_root[SOC_MAX_NUM_DEVICES] = { {0}
};

/*
 * \brief - error recovery common init.
 */
shr_error_e
dnx_err_recovery_common_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * consider both "enable" and "debug" modes as TRUE 
     */
    dnx_err_recovery_common_root[unit].is_enable =
        (dnx_data_dev_init.ha.error_recovery_support_get(unit) > 0) ? TRUE : FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_err_recovery_common_enable_disable(
    int unit,
    uint32 is_enable)
{
    SHR_FUNC_INIT_VARS(unit);
    dnx_err_recovery_common_root[unit].is_enable = is_enable;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_common_validation_enable_disable_set(
    int unit,
    uint32 validation_enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    dnx_err_recovery_common_root[unit].validation_enabled = validation_enabled;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_is_enabled(
    int unit)
{
    return dnx_err_recovery_common_root[unit].is_enable || dnx_rollback_journal_is_error_recovery_bypassed(unit);
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_is_validation_enabled(
    int unit)
{
    return dnx_err_recovery_common_root[unit].validation_enabled;
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id)
{
    return (er_thread_id != -1
            && dnx_err_recovery_common_is_enabled(unit)
            && dnx_rollback_journal_is_done_init(unit)
            && !SOC_IS_DETACHING(unit)
            && !dnx_rollback_journal_is_any_journal_rolling_back(unit, er_thread_id)
            && DNX_ERR_RECOVERY_COMMON_IS_ON_ALL ==
            dnx_err_recovery_common_root[unit].managers[er_thread_id][mngr_id].bitmap);
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_flag_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    return (dnx_err_recovery_common_root[unit].managers[er_thread_id][mngr_id].bitmap & flag) ? TRUE : FALSE;
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_flag_mask_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 mask)
{
    return ((dnx_err_recovery_common_root[unit].managers[er_thread_id][mngr_id].bitmap & mask) == mask) ? TRUE : FALSE;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_common_flag_set(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].managers[er_thread_id][mngr_id].bitmap |= flag;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_common_flag_clear(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].managers[er_thread_id][mngr_id].bitmap &= ~(flag);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
