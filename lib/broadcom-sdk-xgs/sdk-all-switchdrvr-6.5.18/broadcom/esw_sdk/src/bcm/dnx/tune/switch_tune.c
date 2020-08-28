/** \file switch_tune.c
 * Tune trap module file
 * 
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $Id$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files. {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include "switch_tune.h"

/*
 * }  Include files.
 */

/**
 * \brief - Init Split Horizon trap.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
static shr_error_e
dnx_tune_switch_split_horizon_init(
    int unit)
{
    bcm_switch_network_group_t source_network_group_id;
    bcm_switch_network_group_config_t group_config;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialization Split Horizon:
     * Only HUB-HUB is trapped.
     */
    source_network_group_id = 1;
    group_config.dest_network_group_id = 1;
    group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;

    SHR_IF_ERR_EXIT(bcm_switch_network_group_config_set(unit, source_network_group_id, &group_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init default forwarding destination to be notFound trap
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_tune_switch_fwd_not_found_trap_init(
    int unit)
{
    int trap_unknown_dst;
    bcm_rx_trap_config_t config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUnknownDest, &trap_unknown_dst));
    bcm_rx_trap_config_t_init(&config);
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_unknown_dst, &config));
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchForwardLookupNotFoundTrap, trap_unknown_dst));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See switch_tune.h file
 */
shr_error_e
dnx_tune_switch_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialization Split Horizon:
     */
    SHR_IF_ERR_EXIT(dnx_tune_switch_split_horizon_init(unit));

    /*
     * Initialize not found trap destination.
     */
    SHR_IF_ERR_EXIT(dnx_tune_switch_fwd_not_found_trap_init(unit));

exit:
    SHR_FUNC_EXIT;
}
