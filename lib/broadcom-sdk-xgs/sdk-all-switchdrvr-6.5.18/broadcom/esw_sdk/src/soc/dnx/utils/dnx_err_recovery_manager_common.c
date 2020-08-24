/** \file dnx_err_recovery_manager_common.c
 * This module is a common module between
 * the Error Recovery manager and
 * the State Snapshot manager.
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED

/*
 * state of managers
 */
static dnx_err_recovery_common_root_t dnx_err_recovery_common_root[SOC_MAX_NUM_DEVICES] = { {0} };

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_is_on(
    int unit,
    dnxc_rollback_journal_type_e mngr_id)
{
    return (dnxc_rollback_journal_is_done_init(unit)
            && !SOC_IS_DETACHING(unit)
            && !dnxc_rollback_journal_is_any_journal_rolling_back(unit)
            && DNX_ERR_RECOVERY_COMMON_IS_ON_ALL == dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap);
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_flag_is_on(
    int unit,
    dnxc_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    return (dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap & flag) ? TRUE : FALSE;
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_flag_mask_is_on(
    int unit,
    dnxc_rollback_journal_type_e mngr_id,
    uint32 mask)
{
    return ((dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap & mask) == mask) ? TRUE : FALSE;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_common_flag_set(
    int unit,
    dnxc_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap |= flag;

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
    dnxc_rollback_journal_type_e mngr_id,
    uint32 flag)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].managers[mngr_id].bitmap &= ~(flag);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_common_is_dbal_access_region(
    int unit)
{
    return (dnx_err_recovery_common_root[unit].dbal_access_counter > 0);
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_common_dbal_access_region_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_common_root[unit].dbal_access_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_common_dbal_access_region_end(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (0 == dnx_err_recovery_common_root[unit].dbal_access_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "err recovery manager ERROR: attempted to end dbal access region without start.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    dnx_err_recovery_common_root[unit].dbal_access_counter--;

exit:
    SHR_FUNC_EXIT;
}

#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

#undef _ERR_MSG_MODULE_NAME
