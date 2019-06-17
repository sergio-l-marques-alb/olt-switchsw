/** \file dbal_actions_cb.c
 * $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * cb for other modules to use before and after accessing the HW
 *
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"

/** include per cb user */
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
/**
 * see .h file file for description
 */
shr_error_e
dbal_action_access_error_recovery_invalidate(
    int unit,
    dbal_action_access_func_e action_access_func)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (action_access_func)
    {
        case DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR:
            if (_SHR_E_NONE != dnx_err_recovery_transaction_invalidate(unit))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal table clear not supported in Error Recovery transactions!\n");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Function cannot invalidate the Error Recovery transaction!\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file file for description
 */
shr_error_e
dbal_action_access_pre_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func)
{
    SHR_FUNC_INIT_VARS(unit);

    if(0 != (entry_handle->er_flags & DNX_DBAL_JOURNAL_ROLLBACK_FLAG))
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_manager_dbal_access_region_inc(unit, action_access_type, action_access_func));
    }

    if(0 != (entry_handle->er_flags & DNX_DBAL_JOURNAL_COMPARE_FLAG))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_dbal_access_region_inc(unit));
    }


    if (dnxc_ha_is_access_disabled(unit, DNXC_HA_ALLOW_DBAL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "It is not allowed to access DBAL during deinit or warmboot\n");
    }

    /*
     * since we are enforcing no-dbal-access no point in enforcing no-schan and no-sw-state access inside dbal 
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SCHAN));
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SW_STATE));

    DNXC_MTA(dnxc_multithread_analyzer_mark_dbal_region(unit, TRUE));

    switch (action_access_func)
    {
        case DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add(unit, get_entry_handle, entry_handle));
            DNXC_MTA(SHR_IF_ERR_EXIT
                     (dnxc_multithread_analyzer_log_resource_use
                      (unit, MTA_RESOURCE_DBAL, entry_handle->table_id, TRUE)));
            break;
        case DBAL_ACTION_ACCESS_FUNC_ENTRY_GET:
            DNXC_MTA(SHR_IF_ERR_EXIT
                     (dnxc_multithread_analyzer_log_resource_use
                      (unit, MTA_RESOURCE_DBAL, entry_handle->table_id, FALSE)));
            break;
        case DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_log_clear(unit, get_entry_handle));
            DNXC_MTA(SHR_IF_ERR_EXIT
                     (dnxc_multithread_analyzer_log_resource_use
                      (unit, MTA_RESOURCE_DBAL, entry_handle->table_id, TRUE)));
            break;
        case DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR:
            SHR_IF_ERR_EXIT(dbal_action_access_error_recovery_invalidate(unit, DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file for description
 */
void
dbal_action_access_post_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func,
    int rv)
{
    int rc = SOC_E_NONE;
    DNXC_MTA(dnxc_multithread_analyzer_mark_dbal_region(unit, FALSE));

    if(0 != (entry_handle->er_flags & DNX_DBAL_JOURNAL_COMPARE_FLAG))
    {
        dnx_state_comparison_dbal_access_region_dec(unit);
    }

    if(0 != (entry_handle->er_flags & DNX_DBAL_JOURNAL_ROLLBACK_FLAG))
    {
        dnx_err_recovery_manager_dbal_access_region_dec(unit, action_access_type, action_access_func);
        dnx_dbal_journal_invalidate_last_on_err(unit, action_access_type, action_access_func, rv);
    }

    /*
     * since we are enforcing no-dbal-access no point in enforcing no-schan and no-sw-state access inside dbal 
     */
    if (!dnxc_ha_is_access_disabled(unit, DNXC_HA_ALLOW_DBAL))
    {
        rc = dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SCHAN);
        assert(rc == SOC_E_NONE);

        rc = dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SW_STATE);
        assert(rc == SOC_E_NONE);
    }

}
