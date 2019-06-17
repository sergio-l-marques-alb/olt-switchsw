/** \file tune.c
 * main Tune module file
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/tune/iqs_tune.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include "ingress_congestion_tune.h"
#include "egr_queuing_tune.h"
#include "scheduler_tune.h"
#include "nif_tune.h"
#include "trap_tune.h"
#include "oam_tune.h"
#include "policer_tune.h"
#include "ptp_tune.h"
#include "fabric_tune.h"
#include "fabric_cgm_tune.h"
#include "switch_tune.h"
#include "ecgm_tune.h"
#include <soc/sand/sand_aux_access.h>
#include <soc/drv.h>

/*
 * }
 * Include files.
 */
/*
 * Defines.
 * {
 */

/*
 * }
 * Defines.
 */

/*
 * See .h file
 */
shr_error_e
dnx_tune_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_trap_init(unit));
    
    if (!SOC_IS_J2C(unit))
    {
        SHR_IF_ERR_EXIT(dnx_tune_ingress_congestion_set(unit));

        SHR_IF_ERR_EXIT(dnx_tune_egr_queuing_rate_tune(unit));

        SHR_IF_ERR_EXIT(dnx_tune_scheduler_tune(unit));

        SHR_IF_ERR_EXIT(dnx_policer_tune_init(unit));

        SHR_IF_ERR_EXIT(dnx_ptp_tune_init(unit));

        SHR_IF_ERR_EXIT(dnx_tune_oam_init(unit));

#ifdef DNX_EMULATION_1_CORE
#endif
        SHR_IF_ERR_EXIT(dnx_tune_fabric_set(unit));

        SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_set(unit));

        SHR_IF_ERR_EXIT(dnx_tune_iqs_init(unit));

        SHR_IF_ERR_EXIT(dnx_ecgm_tune_init(unit));
    }
    SHR_IF_ERR_EXIT(dnx_tune_switch_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tune_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** nothing to be done */

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tune_port_add(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        SHR_EXIT();
    }

    /*
     * priority config
     */
    SHR_IF_ERR_EXIT(dnx_port_tune_port_add_priority_config(unit, port));

    /*
     * egress queuing tune
     */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        SHR_IF_ERR_EXIT(dnx_tune_egr_queuing_port_add(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tune_port_remove(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Placeholder for tuning that should be done when removing port.
     * Currently - nothing to tune during port removal.
     */

    SHR_FUNC_EXIT;

}
