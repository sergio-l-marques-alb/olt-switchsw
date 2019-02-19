/** \file dnxc_err_recovery_manager.c
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/drv.h>

#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/dnxc_err_recovery_manager.h>

#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

err_recovery_manager_t dnxc_err_recovery_manager[SOC_MAX_NUM_DEVICES];

#define DNXC_ERR_RECOVERY_ENSURE_STARTED(unit)\
    do { \
        if(TRUE != dnxc_err_recovery_manager[unit].is_started) { \
            return SOC_E_NONE; \
        } \
    } \
    while(0)

#define DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit)\
    do{\
        if (!sal_thread_is_journaling_allowed()) {\
            return SOC_E_NONE;\
        }\
    }while(0)

/*
 * see .h file for description
 */
int
dnxc_err_recovery_start(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (TRUE == dnxc_err_recovery_manager[unit].is_started)
    {
        return SOC_E_NONE;
    }

    DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_start(unit));

    

    /*
     * write down for which API we are doing error recovery 
     */
    dnxc_err_recovery_manager[unit].start_api = dnxc_err_recovery_manager[unit].api_counter;
    dnxc_err_recovery_manager[unit].is_started = TRUE;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_api_counter_inc(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit);

    if (!(SOC_UNIT_VALID(unit)))
    {
        return SOC_E_NONE;
    }

    dnxc_err_recovery_manager[unit].api_counter++;

    if (dnxc_err_recovery_manager[unit].api_counter >= 4)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "recovery ERROR: API counter cannot be more than 3.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_api_counter_dec(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit);

    if (!(SOC_UNIT_VALID(unit)))
    {
        return SOC_E_NONE;
    }

    dnxc_err_recovery_manager[unit].api_counter--;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_commit(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNXC_ERR_RECOVERY_ENSURE_STARTED(unit);
    DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit));

    

    dnxc_err_recovery_manager[unit].is_started = FALSE;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_abort(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNXC_ERR_RECOVERY_ENSURE_STARTED(unit);
    DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back(unit));
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit));
    

    dnxc_err_recovery_manager[unit].is_started = FALSE;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
uint8
dnxc_err_recovery_is_on(
    int unit)
{
    /*
     * Error recovery is considered to be on only if:
     * 1. It is started
     * 2. It is not suppressed
     * 3. This is the journaling thread
     * 4. We are currently in the first API that opted in for error recovery 
     * or an internal API
     */
    return (dnxc_err_recovery_manager[unit].is_started
            && (!dnxc_err_recovery_manager[unit].is_suppressed)
            && (sal_thread_is_journaling_allowed())
            && (dnxc_err_recovery_manager[unit].start_api >= dnxc_err_recovery_manager[unit].api_counter)
            && (dnxc_err_recovery_manager[unit].start_api != 0));
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_tmp_allow(
    int unit,
    uint8 is_on)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNXC_ERR_RECOVERY_ENSURE_STARTED(unit);
    DNXC_ERR_RECOVERY_ENSURE_JOURNALING_THREAD(unit);

    dnxc_err_recovery_manager[unit].is_suppressed = !(is_on);

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_test_support_start(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_err_recovery_api_counter_inc(unit));
    SHR_IF_ERR_EXIT(dnxc_err_recovery_start(unit));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int
dnxc_err_recovery_test_support_roll_back(
    int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_err_recovery_abort(unit));
    SHR_IF_ERR_EXIT(dnxc_err_recovery_api_counter_dec(unit));

    DNX_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
