/**
 * \file nif_tune.c $Id$ NIF Tuning procedures for DNX. this 
 *       file is meant for internal functions related to tuning.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx_dispatch.h>

/*
 * }
 */

/**
 * \brief - set the default priority configuration for every new 
 *        port. this configuration can be later overriden by
 *        bcm_port_prioroity_config_set API.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_tune_port_add_priority_config(
    int unit,
    bcm_port_t port)
{
    bcm_port_prio_config_t priority_config;
    dnx_algo_port_type_e port_type;
    int is_master_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, FALSE, FALSE) && is_master_port)
    {
        /*
         * by default, there should be only 1 priority group in the Low scheduler with all PRD priorities 
         */
        priority_config.nof_priority_groups = 1;
        priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
        priority_config.priority_groups[0].source_priority = IMB_PRD_PRIORITY_ALL;
        /*
         * take all entries to a single RMC (max fifo size)
         */
        priority_config.priority_groups[0].num_of_entries = -1;

        SHR_IF_ERR_EXIT(bcm_dnx_port_priority_config_set(unit, port, &priority_config));
    }

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
