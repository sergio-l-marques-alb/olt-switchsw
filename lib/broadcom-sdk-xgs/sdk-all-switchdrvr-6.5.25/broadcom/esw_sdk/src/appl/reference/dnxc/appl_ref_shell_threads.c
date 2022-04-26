/*
 * $Id: appl_ref_shell_threads.c, v1 16/06/2014 09:55:39 db889754 $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/shell.h>
#include <appl/diag/dnxc/diag_dnxc_phy_prbsstat.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>

/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COUNTER

/*************
 * TYPE DEFS *
 *************/

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief - Disable all threads created after init using shell commands
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_shell_threads_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_stop(unit, DIAG_DNXC_PHY_PRBSSTAT_DEINIT));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
