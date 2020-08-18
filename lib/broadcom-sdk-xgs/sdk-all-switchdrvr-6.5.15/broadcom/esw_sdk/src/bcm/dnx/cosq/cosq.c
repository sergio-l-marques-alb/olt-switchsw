/**
 * \file cosq.c
 * $Id$
 *
 * cosq API functions for DNX. \n
 * Note - some of the code is currently ported and is located under legacy folder
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm/cosq.h>
#include <shared/gport.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <bcm_int/dnx/legacy/fabric.h>

#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/iqs_api.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/cosq_stat.h>
#include <bcm_int/dnx/cosq/cosq_latency.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>

#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/cosq/cosq_latency.h>
#include <bcm_int/dnx/tune/iqs_tune.h>
#include <bcm_int/dnx/tune/tune.h>

#include <bcm_int/dnx/cosq/ingress/compensation.h>
#include "flow_control_imp.h"
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_mngr.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
/*
 * Static (local) procedures.
 * {
 */

static int
dnx_cosq_user_port_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_t * user_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, user_port));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Returns the FAP port(TM port) associated with the given mod:port gport
 */
static int
dnx_cosq_fap_port_get(
    int unit,
    bcm_gport_t gport,
    uint32 *tm_port,
    int *core)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, core, tm_port));
    LOG_DEBUG(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "unit(%d) port(%d) fap_port(%d)\n"), unit, logical_port, *tm_port));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Convert from BCM cosq to dnx EGR Q Prio
 */
static int
dnx_cosq_bcm_cosq_to_egr_q_prio(
    int unit,
    int cosq,
    dnx_egr_q_prio_e * prio)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (cosq)
    {
        case BCM_COSQ_HIGH_PRIORITY:
        case 0:
        {
            *prio = DNX_EGR_Q_PRIO_0;
            break;
        }
        case BCM_COSQ_LOW_PRIORITY:
        case 1:
        {
            *prio = DNX_EGR_Q_PRIO_1;
            break;
        }
        case 2:
        {
            *prio = DNX_EGR_Q_PRIO_2;
            break;
        }
        case 3:
        {
            *prio = DNX_EGR_Q_PRIO_3;
            break;
        }
        case 4:
        {
            *prio = DNX_EGR_Q_PRIO_4;
            break;
        }
        case 5:
        {
            *prio = DNX_EGR_Q_PRIO_5;
            break;
        }
        case 6:
        {
            *prio = DNX_EGR_Q_PRIO_6;
            break;
        }
        case 7:
        {
            *prio = DNX_EGR_Q_PRIO_7;
            break;
        }
        case -1:
        {
            *prio = DNX_EGR_Q_PRIO_ALL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d: invalid cosq %d", unit, cosq);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose:
 * Convert from BCM SP modes to dnx EGR OFP interface prio modes
 */
static int
dnx_cosq_mode_sp_to_egr_ofp_interface_prio(
    int unit,
    int mode,
    dnx_egr_ofp_interface_prio_e * nif_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case BCM_COSQ_SP0:
            *nif_priority = DNX_EGR_OFP_INTERFACE_PRIO_HIGH;
            break;
        case BCM_COSQ_SP1:
            *nif_priority = DNX_EGR_OFP_INTERFACE_PRIO_LOW;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, invalid mode %d", unit, mode);
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_cosq_egq_port_priority_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    int nof_priorities;
    int port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * retrieve port 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_user_port_get(unit, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    if (cosq >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "cosq is out of range. Should be in range of 0-%d. gport: 0x%x cosq %d failed",
                     nof_priorities - 1, gport, cosq);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Purpose:
 * Schedule mode for Channelized NIF port and CPU interface.
 * Choose Strict priority to Connect to. SP range: H/L and
 * Medium (PB only)
 */
static int
dnx_cosq_gport_egress_scheduler_queue_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    uint32 tm_port;
    int core;
    dnx_egr_ofp_sch_info_t info;
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * clear 
     */
    dnx_egr_ofp_sch_info_clear(&info);
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    /*
     * valid 
     */
    if (!(mode >= BCM_COSQ_SP0 && mode <= BCM_COSQ_SP1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode range is SP0-SP1");
    }
    /*
     * Get existed configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_get(unit, core, tm_port, cosq, &info));
    /*
     * Set egress ofp scheduler info 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_mode_sp_to_egr_ofp_interface_prio(unit, mode, &(info.nif_priority)));
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_set(unit, core, tm_port, cosq, &info));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose: 
 * Schedule mode for Channelized NIF port and CPU interface. 
 * Get the strict priority that the port is connected to. 
 * SP range: H/L and Medium (PB only)
 */
static int
dnx_cosq_gport_egress_scheduler_queue_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    uint32 tm_port;
    int core, is_high_priority;
    dnx_egr_ofp_sch_info_t info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    if (cosq != 0)
    {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid parameter: cosq: %d\n"), cosq));
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cosq must be set to 0 in case of Local gport");
    }
    /*
     * clear 
     */
    dnx_egr_ofp_sch_info_clear(&info);
    SHR_IF_ERR_EXIT(dnx_egr_queuing_is_high_priority_queue_get(unit, core, tm_port, cosq, &is_high_priority));
    if (is_high_priority)
    {
        *mode = BCM_COSQ_SP0;
    }
    else
    {
        *mode = BCM_COSQ_SP1;
    }

    *weight = -1;
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose: Conversion from cosq parameter to egress queue id
 */
static int
dnx_egress_queue_from_cosq(
    int unit,
    int *queue_id,
    int cosq)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (cosq)
    {
        case BCM_COSQ_HIGH_PRIORITY:
        case 0:
        {
            *queue_id = 0;
            break;
        }
        case BCM_COSQ_LOW_PRIORITY:
        case 1:
        {
            *queue_id = 1;
            break;
        }
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        {
            *queue_id = cosq;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Not supported cosq: %d in this API", unit, cosq);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Purpose:
 * Set new profile for egress queue mapping
 */
static int
dnx_cosq_gport_egress_queue_mapping_hw_set(
    int unit,
    int profile,
    int core,
    CONST dnx_cosq_egress_queue_mapping_info_t * mapping_info)
{
    int index_dp, index_pri, index_type;
    dnx_egr_q_priority_t egr_prio;

    SHR_FUNC_INIT_VARS(unit);
    dnx_egr_q_priority_clear(&egr_prio);
    /*
     * Run over all data and commit to hw 
     */
    for (index_type = 0; index_type < DNX_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES; index_type++)
    {
        for (index_pri = 0; index_pri < DNX_DEVICE_COSQ_ING_NOF_TC; index_pri++)
        {
            for (index_dp = 0; index_dp < DNX_DEVICE_COSQ_ING_NOF_DP; index_dp++)
            {
                egr_prio.dp = mapping_info->queue_mapping[index_type][index_pri][index_dp].dp;
                egr_prio.tc = mapping_info->queue_mapping[index_type][index_pri][index_dp].tc;
                SHR_IF_ERR_EXIT(dnx_egr_q_prio_set(unit, core, index_type, index_pri, index_dp, profile, &egr_prio));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Get mapping egress queue
 *
 */
static int
dnx_cosq_gport_egress_queue_map_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t * offset)
{
    int core;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    dnx_egr_q_prio_mapping_type_e map_type;
    dnx_egr_q_priority_t egr_prio;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    dnx_egr_q_priority_clear(&egr_prio);
    /*
     * Validate input params 
     */
    SHR_NULL_CHECK(offset, _SHR_E_PARAM, "offset");
    if (ingress_pri < 0 || ingress_pri >= DNX_DEVICE_COSQ_ING_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_DEVICE_COSQ_ING_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get(unit, tm_port, core, &mapping_info));
    /*
     * Unicast type 
     */
    map_type = DNX_EGR_UCAST_TO_SCHED;
    *offset = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc;
exit:
    SHR_FUNC_EXIT;
}

/*
 * Purpose:
 * Map egress queue to a specific priority (offset)
 */
static int
dnx_cosq_gport_egress_queue_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t offset)
{
    shr_error_e rc = _SHR_E_NONE;
    int core;
    int is_last, is_allocated;
    int old_profile, new_profile;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    dnx_egr_q_prio_mapping_type_e map_type;
    uint32 tm_port;
    dnx_egr_q_prio_e tc;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * Validate input params 
     */
    if (ingress_pri < 0 || ingress_pri >= DNX_DEVICE_COSQ_ING_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_DEVICE_COSQ_ING_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }

    if (offset < 0 || offset >= DNX_EGR_NOF_Q_PRIO)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid offset parameter %d", offset);
    }
    /*
     * Get Old data 
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get(unit, tm_port, core, &mapping_info));
    /*
     * Change specific entry - preserve dp, change new tc (offset) 
     */
    /*
     * Unicast type and Scheduled to Multicast 
     */
    map_type = DNX_EGR_UCAST_TO_SCHED;
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, offset, &tc));
    mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc = tc;
    /*
     * Exchange data 
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange
                    (unit, tm_port, core, &mapping_info, &old_profile, &is_last, &new_profile, &is_allocated));
    if (is_allocated)
    {
        /*
         * Create new profile 
         */
        rc = dnx_cosq_gport_egress_queue_mapping_hw_set(unit, new_profile, core, &mapping_info);
        SHR_IF_ERR_EXIT(rc);
    }
    /*
     * Map port to new profile 
     */
    SHR_IF_ERR_EXIT(dnx_egr_q_profile_map_set(unit, core, tm_port, new_profile));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Support Functions - Flow 
 */
static int
dnx_cosq_bcm_cosq_to_tcg(
    int unit,
    bcm_port_t port,
    int cosq,
    dnx_tcg_ndx_t * tcg)
{
    int base_q_pair, nof_priorities;
    SHR_FUNC_INIT_VARS(unit);
    if (cosq < DNX_TCG_MIN || cosq > DNX_TCG_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid cosq parameter %d", unit, cosq);
    }
    /*
     * for the 4 priority port which takes the 4 upper qpairs of the PS, TCG index needs to be shifted by 4
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    if (cosq > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cosq parameter (%d) is bigger than nof priorities (%u)", cosq, nof_priorities);
    }
    *tcg = cosq + (base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT);
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_cosq_port_egq_tcg_validate(
    int unit,
    bcm_gport_t gport,
    dnx_tcg_ndx_t tcg)
{
    int nof_priorities, base_q_pair;
    int port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * retrieve port 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_user_port_get(unit, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    tcg -= base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT;
    if (tcg >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "cosq is out of range. Should be in range of 0-%d. gport: 0x%x cosq %d failed",
                     nof_priorities, gport, tcg);
    }
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d TCG is not supported for given port 0x%x. ", unit, gport);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * utility functions
 */
/* 
 * Purpose: 
 * Schedule mode for Channelized NIF port and CPU interface. 
 * Choose Strict priority to Connect to. SP range: H/L
 */
static int
dnx_cosq_gport_egress_scheduler_port_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int num_priorities;
    bcm_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    if (cosq != 0)
    {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid parameter: cosq: %d\n"), cosq));
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cosq must be set to 0 in case of Local gport");
    }
    port = BCM_GPORT_LOCAL_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_scheduler_queue_sched_set(unit, gport, cosq, mode, weight));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Purpose:
 * Schedule mode for Channelized NIF port and CPU interface.
 * Choose Strict priority to Connect to. SP range: H/L and
 * Medium (PB only)
 */
static int
dnx_cosq_gport_egress_scheduler_port_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    if (cosq != 0)
    {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid parameter: cosq: %d\n"), cosq));
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cosq must be set to 0 in case of Local gport");
    }
    SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_scheduler_queue_sched_get(unit, gport, cosq, mode, weight));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Fill in the bcm mode (SP) and weight from structure of wfq scheduler
 * 
 */
static int
dnx_cosq_egress_fill_mode_weight_from_ofp_wfq_sch(
    uint8 is_ucast_egress_queue,
    dnx_egr_ofp_sch_wfq_t * sch_wfq_p,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SHR_NULL_CHECK(sch_wfq_p, _SHR_E_PARAM, "sch_wfq_p");
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");
    if (is_ucast_egress_queue == TRUE)
    {
        if (sch_wfq_p->sched_weight == 0)
        {
            *mode = DNX_COSQ_EGR_OFP_SCH_SP_HIGH;
            *weight = -1;
        }
        else
        {
            if (sch_wfq_p->unsched_weight == 0)
            {
                *mode = DNX_COSQ_EGR_OFP_SCH_SP_LOW;
                *weight = -1;
            }
            else
            {
                /*
                 * Take weight into consideration 
                 */
                *mode = -1;
                *weight = sch_wfq_p->sched_weight;
            }
        }
    }
    else
    {
        if (sch_wfq_p->unsched_weight == 0)
        {
            *mode = DNX_COSQ_EGR_OFP_SCH_SP_HIGH;
            *weight = -1;
        }
        else
        {
            if (sch_wfq_p->sched_weight == 0)
            {
                *mode = DNX_COSQ_EGR_OFP_SCH_SP_LOW;
                *weight = -1;
            }
            else
            {
                /*
                 * Take weight into consideration 
                 */
                *mode = -1;
                *weight = sch_wfq_p->unsched_weight;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - helper function decoding COSQ threshold index into different parameters building the index
 * Opposite direction is done by BCM macros.
 */
static int
dnx_cosq_threshold_index_decode(
    int unit,
    bcm_cosq_threshold_index_t index,
    dnx_cosq_threshold_index_type_t * index_type,
    dnx_cosq_threshold_index_info_t * index_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * set all fields to invalid
     */
    index_info->dp = -1;
    index_info->tc = -1;
    index_info->pool_id = -1;
    index_info->priority = -1;
    index_info->cast = -1;
    index_info->fwd_action = -1;

    switch (index & _SHR_COSQ_TH_INDEX_INDICATION_MASK)
    {
        case _SHR_COSQ_TH_INDEX_DP_INDICATION:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_DP;
            index_info->dp = (index & _SHR_COSQ_TH_INDEX_FIELD_MASK);
            break;
        case _SHR_COSQ_TH_INDEX_TC_INDICATION:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_TC;
            index_info->tc = (index & _SHR_COSQ_TH_INDEX_FIELD_MASK);
            break;
        case _SHR_COSQ_TH_INDEX_POOL_DP_INDICATION:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_POOL_DP;
            index_info->dp = (index & _SHR_COSQ_TH_INDEX_FIELD_MASK);
            index_info->pool_id = (index >> _SHR_COSQ_TH_INDEX_FIELD_NOF_BITS) & _SHR_COSQ_TH_INDEX_FIELD_MASK;
            break;
        case _SHR_COSQ_TH_INDEX_CLASS_FWDACT_INDICATION:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_CAST_PRIO_FDWACT;
            index_info->fwd_action = (index & _SHR_COSQ_TH_INDEX_FIELD_MASK);

            index_info->priority = ((index >> _SHR_COSQ_TH_INDEX_FIELD_NOF_BITS) & _SHR_COSQ_TH_INDEX_FIELD_MASK);
            /** priority can be bcmCosqIngrFwdPriorityNone which is mapped to -1, so need to restore it with sign extension */
            index_info->priority = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(index_info->priority,
                                                                         _SHR_COSQ_TH_INDEX_FIELD_NOF_BITS);

            index_info->cast = (index >> 2 * _SHR_COSQ_TH_INDEX_FIELD_NOF_BITS) & _SHR_COSQ_TH_INDEX_FIELD_MASK;
            break;
        case _SHR_COSQ_TH_INDEX_PRIO_INDICATION:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_PRIO;
            index_info->priority = (index & _SHR_COSQ_TH_INDEX_FIELD_MASK);
            break;
        case _SHR_COSQ_TH_INDEX_POOL_PRIO_INDICATION:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO;
            index_info->priority = (index & _SHR_COSQ_TH_INDEX_FIELD_MASK);
            index_info->pool_id = (index >> _SHR_COSQ_TH_INDEX_FIELD_NOF_BITS) & _SHR_COSQ_TH_INDEX_FIELD_MASK;
            break;
        default:
            *index_type = DNX_COSQ_THRESHOLD_INDEX_INVALID;
            break;
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/**
* \brief - function for API dnx_cosq_control_set
*          for a mc egress queue gport - checks type and
*          calls the approriate function.
*/
static shr_error_e
dnx_cosq_control_mcast_egress_queue_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case (bcmCosqControlMulticastQueueToPdSpMap):
            SHR_IF_ERR_EXIT(dnx_ecgm_map_mc_queue_to_pd_sp_set(unit, port, cosq, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - function for API dnx_cosq_control_get
*          for a mc egress queue gport - checks type and
*          calls the approriate function.
*/
static shr_error_e
dnx_cosq_control_mcast_egress_queue_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case (bcmCosqControlMulticastQueueToPdSpMap):
            SHR_IF_ERR_EXIT(dnx_ecgm_map_mc_queue_to_pd_sp_get(unit, port, cosq, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - function for API dnx_cosq_control_get
*          for a UC queue gport - checks type and
*          calls the approriate function.
*/
static shr_error_e
dnx_cosq_control_queue_get(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int core, queue_id;
    dbal_enum_value_field_dram_bound_state_e dram_bound_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, voq_gport, cosq, FALSE, &core, &queue_id));

    switch (type)
    {
        case (bcmCosqControlIsInDram):

            if (core == BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Info cannot be retrieved for BCM_CORE_ALL. VOQ gport core id has to be specific!");
            }

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_dram_bound_state_get
                            (unit, core, queue_id, &dram_bound_status));

            /**
             *  ALL values different than DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F
             *  means that the VOQ is in DRAM
             */
            if (dram_bound_status == DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F)
            {
                *arg = FALSE;
            }
            else
            {
                *arg = TRUE;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Global VSQ static (non FADT) threshold
 *
 */
static shr_error_e
dnx_cosq_global_vsq_static_threshold_set(
    int unit,
    int core_id,
    uint32 flags,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (thresh_info->threshold_action)
    {
        case bcmCosqThreshActionDrop:
            /** VSQ gl tail drop thresholds */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_gl_drop_threshold_set(unit, core_id,
                                                                             index_type, index_info,
                                                                             thresh_info, threshold));
            break;
        case bcmCosqThreshActionFc:
            /** Global recourses Flow Control thresholds set */
            SHR_IF_ERR_EXIT(dnx_fc_glb_rsc_threshold_set(unit, core_id,
                                                         index_type, index_info, thresh_info, threshold));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported threshold_action %d\n", thresh_info->threshold_action);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Global VSQ static (non FADT) threshold
 *
 */
static shr_error_e
dnx_cosq_global_vsq_static_threshold_get(
    int unit,
    int core_id,
    uint32 flags,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (thresh_info->threshold_action)
    {
        case bcmCosqThreshActionDrop:
            /** VSQ gl tail drop thresholds */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_gl_drop_threshold_get(unit, core_id,
                                                                             index_type, index_info,
                                                                             thresh_info, threshold));
            break;
        case bcmCosqThreshActionFc:
            /** Global recourses Flow Control thresholds get */
            SHR_IF_ERR_EXIT(dnx_fc_glb_rsc_threshold_get(unit, core_id,
                                                         index_type, index_info, thresh_info, threshold));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported threshold_action %d\n", thresh_info->threshold_action);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for bcm_dnx_cosq_gport_static_threshold_set/get
 */
static int
dnx_cosq_gport_static_threshold_verify(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    dnx_cosq_threshold_index_type_t index_type;
    dnx_cosq_threshold_index_info_t index_info;
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(thresh_info, _SHR_E_PARAM, "thresh_info");
    SHR_IF_ERR_EXIT(dnx_cosq_threshold_index_decode(unit, thresh_info->index, &index_type, &index_info));

    gport = thresh_info->gport;
    if (BCM_COSQ_GPORT_IS_VSQ_GL(gport))
    {
        if ((index_type == DNX_COSQ_THRESHOLD_INDEX_DP) && (thresh_info->threshold_action == bcmCosqThreshActionDrop))
        {
            /**
             * DNX_COSQ_THRESHOLD_INDEX_DP should not be accessible through BCM API.
             * Access to this index type should be done only during init and should be updated with reasonable defaults.
             * The reasoning behind it is to prevent thresholds from colliding when defining guaranteed(reserved) resources.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index type %d\n", index_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Procedures imported from legacy/cosq.c
 * {
 */
int
bcm_dnx_cosq_resource_gport_port_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_t * port)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported, instead use API bcm_cosq_gport_parent_get()");

exit:
    SHR_FUNC_EXIT;
}
int
bcm_dnx_cosq_gport_egress_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t offset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_queue_map_set(unit, gport, ingress_pri, ingress_dp, offset));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported gport type parameter 0x%x", gport);
    }
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_gport_egress_map_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t * offset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_queue_map_get(unit, gport, ingress_pri, ingress_dp, offset));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported gport type parameter 0x%x", gport);
    }
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_port_mapping_multi_get(
    int unit,
    bcm_port_t port,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{
    return _SHR_E_UNAVAIL;
}

int
bcm_dnx_cosq_port_mapping_multi_set(
    int unit,
    bcm_port_t port,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{
    return _SHR_E_UNAVAIL;
}
/* 
 * Purpose:
 * Fill in the structure of wfq scheduler given
 * bcm mode (SP) and weight
 */
static int
dnx_cosq_egress_fill_ofp_wfq_sch_from_mode_weight(
    uint8 is_ucast_egress_queue,
    int mode,
    int weight,
    dnx_egr_ofp_sch_wfq_t * sch_wfq_p)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(sch_wfq_p, _SHR_E_PARAM, "sch_wfq_p");
    if (mode != -1)
    {
        /*
         * SP 
         */
        /*
         * 0 - indicate SP High 
         */
        /*
         * > 0 - indicate lower priority 
         */
        if (is_ucast_egress_queue == TRUE)
        {
            sch_wfq_p->sched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 0 : 1;
            sch_wfq_p->unsched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 1 : 0;
        }
        else
        {
            sch_wfq_p->sched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 1 : 0;
            sch_wfq_p->unsched_weight = (mode == DNX_COSQ_EGR_OFP_SCH_SP_HIGH) ? 0 : 1;
        }
    }
    else
    {
        /*
         * Take weight into consideration 
         */
        if (is_ucast_egress_queue == TRUE)
        {
            sch_wfq_p->sched_weight = weight;
        }
        else
        {
            sch_wfq_p->unsched_weight = weight;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose: 
 * Attach Port-Priority of a given port priority TC-X to the TCG-Y
 */
static int
dnx_cosq_gport_egress_port_tc_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    uint32 tm_port;
    int core;
    bcm_port_t port;
    int base_q_pair, nof_priorities;
    dnx_egr_q_prio_e prio;
    dnx_tcg_ndx_t tcg;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only port tc gport. gport: 0x%x", gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    if ((!(mode >= BCM_COSQ_SP0 && mode <= BCM_COSQ_SP7)) && (!(mode >= DNX_TCG_MIN && mode <= DNX_TCG_MAX)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode range must be tcg_ndx (0-7) or SP0-SP7");
    }
    tcg = (mode >= BCM_COSQ_SP0) ? (mode - BCM_COSQ_SP0) : mode;
    /*
     * for the 4 priority port which takes the 4 upper qpairs of the PS, TCG index needs to be shifted by 4
     */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    if (tcg > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcg parameter (%d) is bigger than nof priorities (%u)", tcg, nof_priorities);
    }
    tcg += base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT;
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_ofp_tcg_single_set(unit, core, tm_port, prio, tcg));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose: 
 * Get TCG-Y Port-Priority of a given port priority TC-X.
 */
static int
dnx_cosq_gport_egress_port_tc_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    uint32 tm_port;
    int core;
    bcm_port_t port;
    int base_q_pair;
    dnx_tcg_ndx_t tcg_ndx;
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only port tc gport. gport: 0x%x", gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    /*
     * Clear
     */
    tcg_ndx = 0;
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * Get existed configuration.
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_ofp_tcg_single_get(unit, core, tm_port, prio, &(tcg_ndx)));
    /*
     * for the 4 priority port which takes the 4 upper qpairs of the PS, TCG index needs to be shifted by 4
     */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    *mode = tcg_ndx + BCM_COSQ_SP0 - (base_q_pair % DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT);
exit:
    SHR_FUNC_EXIT;
}
static int
dnx_cosq_egress_fill_tcg_weight_from_mode_weight(
    int unit,
    int mode,
    int weight,
    uint32 *tcg_weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mode == BCM_COSQ_SP0)
    {
        *tcg_weight = 0;        /* SP */
    }
    else if (mode == -1)
    {
        /*
         * Take weight into consideration 
         */
        *tcg_weight = weight;
    }
    else if (mode == BCM_COSQ_NONE)
    {
        *tcg_weight = -1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - invalid mode given. mode: %d, weight: %d", mode, weight);
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose: 
 * Set EIR Weight for Egress Port TCG
 */
static int
dnx_cosq_gport_egress_port_tcg_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    uint32 tm_port;
    bcm_port_t port;
    int core;
    dnx_egr_tcg_sch_wfq_t info;
    dnx_tcg_ndx_t tcg_ndx;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only port tc gport. gport: 0x%x", gport);
    }
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_tcg(unit, port, cosq, &tcg_ndx));
    SHR_IF_ERR_EXIT(dnx_cosq_port_egq_tcg_validate(unit, gport, tcg_ndx));
    /*
     * Verify 
     */
    if (!((mode == -1) || (mode == BCM_COSQ_SP0) || (mode == BCM_COSQ_NONE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode can be -1 or BCM_COSQ_SP0 or NONE ");
    }
    if (!((mode == -1) || (mode == BCM_COSQ_NONE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode can be -1 or NONE ");
    }
    if (mode == -1)
    {
        /*
         * Weight is taken into consideration 
         */
        if (!((weight) >= 0 && ((weight) <= (DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight given %d", weight);
        }
    }
    /*
     * Clear 
     */
    dnx_egr_tcg_sch_wfq_clear(&info);
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * Get existed configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_weight_get(unit, core, tm_port, tcg_ndx, &info));
    info.tcg_weight_valid = (mode == BCM_COSQ_NONE) ? FALSE : TRUE;
    SHR_IF_ERR_EXIT(dnx_cosq_egress_fill_tcg_weight_from_mode_weight(unit, mode, weight, &info.tcg_weight));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_weight_set(unit, core, tm_port, tcg_ndx, &info));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose: 
 * Get EIR Weight for Egress Port TCG.
 */
int
dnx_cosq_gport_egress_port_tcg_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    uint32 tm_port;
    int core;
    bcm_port_t port;
    dnx_egr_tcg_sch_wfq_t info;
    dnx_tcg_ndx_t tcg_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");
    /*
     * Validate 
     */
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - function support only port tc gport. gport: 0x%x", gport);
    }

    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_tcg(unit, port, cosq, &tcg_ndx));
    SHR_IF_ERR_EXIT(dnx_cosq_port_egq_tcg_validate(unit, gport, tcg_ndx));
    /*
     * Clear 
     */
    dnx_egr_tcg_sch_wfq_clear(&info);
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * Get existed configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_weight_get(unit, core, tm_port, tcg_ndx, &info));
    *mode = info.tcg_weight_valid ? -1 : BCM_COSQ_NONE;
    if (info.tcg_weight_valid)
    {
        *weight = info.tcg_weight;
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Schedule mode in OTM port scheduler. 
 * UC and MC Egress queues are handled here.
 * Configure the SP to connect to and the weighted distribution 
 * between the other queue on the same priority. 
 */
static int
dnx_cosq_gport_egress_queue_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    uint32 tm_port;
    int core;
    dnx_egr_ofp_sch_info_t info;
    dnx_egr_ofp_sch_wfq_t *sch_wfq_p;
    uint8 is_ucast_egress_queue;
    int queue_id;
    int max_cosq_num;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * clear 
     */
    dnx_egr_ofp_sch_info_clear(&info);
    max_cosq_num = DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit);
    if (!((mode == -1) || (mode >= BCM_COSQ_SP0 && mode <= BCM_COSQ_SP1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parameter error - mode can be -1 or BCM_COSQ_SP0-1 ");
    }
    SHR_IF_ERR_EXIT(dnx_egress_queue_from_cosq(unit, &queue_id, cosq));
    if (!((queue_id >= 0 && queue_id <= max_cosq_num)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue_id parameter %d, max_cosq_num for this ofp: %d", queue_id,
                     max_cosq_num);
    }
    if (mode == -1)
    {
        /*
         * Weight is taken into consideration 
         */
        if (!((weight) >= 1 && ((weight) <= (DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight given %d", weight);
        }
    }
    /*
     * Get existed configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_get(unit, core, tm_port, queue_id, &info));
    sch_wfq_p = &(info.ofp_wfq_q_pair[queue_id]);
    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        /*
         * UC Egress queue fill 
         */
        is_ucast_egress_queue = TRUE;
    }
    else
    {
        /*
         * MC Egress queue fill 
         */
        is_ucast_egress_queue = FALSE;
    }
    /*
     * Fill WFQ SCH structure 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egress_fill_ofp_wfq_sch_from_mode_weight(is_ucast_egress_queue, mode, weight, sch_wfq_p));
    /*
     * Set configuration 
     */
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_set(unit, core, tm_port, queue_id, &info));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Schedule mode in OTM port scheduler. 
 * UC and MC Egress queues are handled here.
 * Get the SP that the port connect to and the weighted distribution 
 * between the other queue on the same priority. 
 */
static int
dnx_cosq_gport_egress_queue_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    uint32 tm_port;
    int core;
    dnx_egr_ofp_sch_wfq_t *sch_wfq_p;
    dnx_egr_ofp_sch_info_t info;
    uint8 is_ucast_egress_queue;
    int max_cosq_num, queue_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * clear 
     */
    dnx_egr_ofp_sch_info_clear(&info);
    max_cosq_num = DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit);
    SHR_IF_ERR_EXIT(dnx_egress_queue_from_cosq(unit, &queue_id, cosq));
    if (!((queue_id >= 0 && queue_id <= max_cosq_num)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid queue_id parameter %d, queue_id can be in range of 0-1 or High/Low priorities", queue_id);
    }
    /*
     * Get existed configuration. 
     */
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_get(unit, core, tm_port, cosq, &info));
    /*
     * Fill mode, weight 
     */
    sch_wfq_p = &(info.ofp_wfq_q_pair[queue_id]);
    if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        /*
         * UC Egress queue fill 
         */
        is_ucast_egress_queue = TRUE;
    }
    else
    {
        /*
         * MC Egress queue fill 
         */
        is_ucast_egress_queue = FALSE;
    }
    SHR_IF_ERR_EXIT(dnx_cosq_egress_fill_mode_weight_from_ofp_wfq_sch(is_ucast_egress_queue, sch_wfq_p, mode, weight));
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose:
 * Configure OFP shaping rate, set single port rate.
 */
static int
dnx_cosq_gport_egress_port_scheduler_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 tm_port;
    bcm_port_t port;
    int core;
    int channelized;
    int egr_if;
    uint8 if_recalc;
    uint32 cal_is_modified;
    uint32 nof_instances = 0;
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get port information
     */
    SHR_IF_ERR_EXIT(dnx_cosq_user_port_get(unit, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
    /*
     * Verify 
     */
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of Local gport", cosq);
    }
    /*
     * Currently only BCM_COSQ_BW_NOT_COMMIT flag is supported 
     */
    if ((flags & ~(BCM_COSQ_BW_NOT_COMMIT)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: ?%d\nFlags must be set to 0", flags);
    }
    if (kbits_sec_max > DNX_IF_MAX_RATE_KBPS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_max: %d", kbits_sec_max);
    }
    /*
     * Mark calendar as modified 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.set(unit, core, egr_if, TRUE));
    /*
     * Setting rate to sw 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_single_port_rate_sw_set(unit, core, tm_port, kbits_sec_max));
    /*
     * For the non-channelized interface the rate will be used for calcal calculation
     */
    if (!channelized)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.set(unit, core, egr_if, kbits_sec_max));
    }
    if (!(flags & BCM_COSQ_BW_NOT_COMMIT))
    {
        /*
         * Iterate over cores. !BCM_COSQ_BW_NOT_COMMIT affect all cores.
         */
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            /*
             * Iterate over all ports to figure out whether calendar is changed on core and if
             * non-channelized interface rate is changed on core.
             * if_recalc is used to indicate if we need to recalculate calcal and interface shapers.
             * It will be set to TRUE in case of a non-channalizeed port, or setting rate for the first time(on init)
             */
            if_recalc = FALSE;
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
            SOC_PBMP_ITER(pbmp, port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
                SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.modified.get(unit, core, egr_if, &cal_is_modified));
                if (cal_is_modified)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
                    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                                    nof_calcal_instances.get(unit, core, egr_if, &nof_instances));
                    if (!channelized || (nof_instances <= 0))
                    {
                        /*
                         * For non-channalizeed port, or setting rate for the first time(on init)
                         * set if_recalc to TRUE
                         */
                        if_recalc = TRUE;
                        break;
                    }
                }
            }
            /*
             * Recalculate and set OTM shapers.
             * If if_recalc is TRUE need to recalculate calcal and interface shapers.
             * Else copy calcal to the active calendar and recalculate only port shapers
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_otm_shapers_set(unit, core, if_recalc));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose:
 * Get OFP shaping rate, get single port rate.
 */
static int
dnx_cosq_gport_egress_port_scheduler_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    int core;
    uint32 tm_port;
    soc_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of Local gport", cosq);
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_single_port_rate_hw_get(unit, core, tm_port, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Configure Port-TC (q-pair) shaping rate, set single port rate.
 */
static int
dnx_cosq_gport_egress_port_tc_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 tm_port;
    int core;
    dnx_egr_q_prio_e prio;
    soc_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
    /*
     * Validate gport 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: %d\nFlags must be set to 0", flags);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));
    if (kbits_sec_max > DNX_IF_MAX_RATE_KBPS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_max: %d", kbits_sec_max);
    }
    /*
     * Set configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_rate_sw_set(unit, core, tm_port, prio, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_rate_hw_set(unit));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Get Port-TC (q-pair) shaping rate
 */
static int
dnx_cosq_gport_egress_port_tc_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    uint32 tm_port;
    int core;
    dnx_egr_q_prio_e prio;
    soc_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    if (!BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, gport);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
    /*
     * Validate gport 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * Get existing configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_rate_hw_get(unit, core, tm_port, prio, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));
    /*
     * fill rates 
     */
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose:
 * Configure TCG shaping rate
 */
static int
dnx_cosq_gport_egress_port_tcg_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 tm_port;
    dnx_tcg_ndx_t tcg;
    int core;
    soc_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tcg", unit, gport);
    }
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_tcg(unit, port, cosq, &tcg));
    SHR_IF_ERR_EXIT(dnx_cosq_port_egq_tcg_validate(unit, gport, tcg));
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: %d\nFlags must be set to 0", flags);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    if (kbits_sec_max > DNX_IF_MAX_RATE_KBPS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_max: %d", kbits_sec_max);
    }
    /*
     * Set configuration 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_rate_sw_set(unit, core, tm_port, tcg, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_rate_hw_set(unit));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Get Port-TCG shapring rate
 */
static int
dnx_cosq_gport_egress_port_tcg_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    uint32 tm_port;
    int core;
    dnx_tcg_ndx_t tcg;
    soc_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tcg", unit, gport);
    }
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_tcg(unit, port, cosq, &tcg));
    SHR_IF_ERR_EXIT(dnx_cosq_port_egq_tcg_validate(unit, gport, tcg));
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_rate_hw_get(unit, core, tm_port, tcg, kbits_sec_max));
    /*
     * fill rates 
     */
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose:
 * Configure OFP shaping rate, set configuration per interface.
 */
static int
dnx_cosq_gport_egress_interface_scheduler_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    int core;
    int channelized;
    bcm_port_t port;
    int egr_if;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_cosq_user_port_get(unit, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    /*
     * Verify 
     */
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit(%d) invalid cosq %d", unit, cosq);
    }
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d", kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: flags: %d\nFlags must be set to 0", flags);
    }
    if (!channelized)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bandwidth configuration for non-channnelized interface is not allowed");
    }
    /*
     * update calendar shaper rate at SW database 
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.rate.set(unit, core, egr_if, kbits_sec_max));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_otm_shapers_set(unit, core, TRUE));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Get configuration of OFP shaping rate, get configuration per interface.
 */
static int
dnx_cosq_gport_egress_interface_scheduler_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    int core;
    uint32 tm_port;
    int channelized;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_cosq_user_port_get(unit, gport, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of interface gport",
                     cosq);
    }
    if (!channelized)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bandwidth configuration for non-channnelized interface is not allowed");
    }
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_interface_shaper_get(unit, core, tm_port, kbits_sec_max));
    *kbits_sec_min = 0;
    *flags = 0;
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Set various features depnded control type
 * for the egress port scheduler 
 */
static int
dnx_cosq_control_egress_port_scheduler_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    int port_index;

    SHR_FUNC_INIT_VARS(unit);
    if (cosq != 0)
    {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid parameter: cosq: %d\n"), cosq));
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cosq must be set to 0 in case of Local gport");
    }
    switch (type)
    {
        case bcmCosqControlPacketLengthAdjust:
        {
            if (arg <= DNX_COSQ_EGRESS_CR_DISCOUNT_MIN_VAL || arg >= DNX_COSQ_EGRESS_CR_DISCOUNT_MAX_VAL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid argument discount %d parameter", arg);
            }
            SHR_IF_ERR_EXIT(dnx_egr_port_compensation_set(unit, BCM_GPORT_LOCAL_GET(port), arg));
            break;
        }
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            int core;
            uint32 tm_port;

            SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, port, &tm_port, &core));
            /*
             * Verify 
             */
            if (arg < 0)
            {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid arg parameter: negative parameter\n")));
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            if (arg > DNX_COSQ_PB_EGR_OFP_RATE_MAX_BURST_LIMIT)
            {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "Invalid arg parameter: more than max burst\n")));
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            SHR_IF_ERR_EXIT(dnx_ofp_rates_single_port_max_burst_set(unit, core, tm_port, arg));
            break;
        }
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        {
            if (!SOC_IS_ARAD_B1_AND_BELOW(unit))
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_max_credit_empty_port_set(unit, arg));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter: device is below Arad Plus\n");
            }
            break;
        }
        case bcmCosqControlEgressPriorityOverCast:
        {
            /*
             * Get SP before WFQ mode (per OTM port) 
             */
            port_index = BCM_GPORT_LOCAL_GET(port);
            SHR_IF_ERR_EXIT(dnx_egr_queuing_prio_over_cast_set(unit, port_index, arg));
            break;
        }
        default:
        {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "unit %d, Invalid type %d\n"), unit, type));
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Set various features depnded control type
 * for the egress port tc scheduler 
 */
static int
dnx_cosq_control_egress_port_tc_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{

    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TC(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, port);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        case bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            int core;
            uint32 tm_port;
            dnx_egr_q_prio_e prio;
            /*
             * Validating device type
             */
            if ((type == bcmCosqControlBandwidthBurstMaxEmptyQueue
                 || type == bcmCosqControlBandwidthBurstMaxFlowControlledQueue) && SOC_IS_ARAD_B1_AND_BELOW(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter: device is below Arad Plus\n");
            }
            SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, port, prio));
            SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, port, &tm_port, &core));
            if (arg > DNX_COSQ_PB_EGR_OFP_RATE_MAX_BURST_LIMIT)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter: more than max burst\n");
            }
            /*
             * Set Port-TC rate Info 
             */
            /*
             * At this point, 'type' can only be:
             *   bcmCosqControlBandwidthBurstMax:
             *   bcmCosqControlBandwidthBurstMaxEmptyQueue:
             *   bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
             * Compiler remarks on missing 'type's and
             * Coverity does not accept 'default' then (although it is better practice).
             */
            if (type == bcmCosqControlBandwidthBurstMax)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_max_burst_set(unit, core, tm_port, prio, arg));
            }
            else if (type == bcmCosqControlBandwidthBurstMaxEmptyQueue)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_port_priority_max_burst_for_empty_queues_set(unit, arg));
            }
            else
            {
                /*
                 * This is case: type == bcmCosqControlBandwidthBurstMaxFlowControlledQueue
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_port_priority_max_burst_for_fc_queues_set(unit, arg));
            }
            break;
        }
        case bcmCosqControlPrioritySelect:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_scheduler_queue_sched_set(unit, port, cosq, arg, -1));
            break;
        }
        default:
        {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "unit %d, Invalid type %d\n"), unit, type));
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Set various features depnded control type
 * for the egress port tc scheduler 
 */
static int
dnx_cosq_control_egress_port_tcg_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tcg", unit, type);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Set burst max for egq rate shaper 
             */
            int core;
            uint32 tm_port;
            dnx_tcg_ndx_t tcg;
            bcm_port_t logical_port;
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));
            SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg));
            SHR_IF_ERR_EXIT(dnx_cosq_port_egq_tcg_validate(unit, port, tcg));
            SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, port, &tm_port, &core));
            if (arg > DNX_COSQ_PB_EGR_OFP_RATE_MAX_BURST_LIMIT || arg < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter: more than max burst, %d\n", arg);
            }
            /*
             * Set conifguration 
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_max_burst_set(unit, core, tm_port, tcg, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Get various features depnded control type
 * for the egress port pc 
 */
static int
dnx_cosq_control_egress_port_tc_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int dummy;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
    if (!BCM_COSQ_GPORT_IS_PORT_TC(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tc", unit, port);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        case bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
        {
            /*
             * Get burst max for egq rate shaper 
             */
            int core;
            uint32 tm_port;
            dnx_egr_q_prio_e prio;
            /*
             * Validating device type
             */
            if ((type == bcmCosqControlBandwidthBurstMaxEmptyQueue
                 || type == bcmCosqControlBandwidthBurstMaxFlowControlledQueue) && SOC_IS_ARAD_B1_AND_BELOW(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg parameter: device is below Arad Plus\n");
            }
            SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_egr_q_prio(unit, cosq, &prio));
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, port, prio));
            SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, port, &tm_port, &core));
            /*
             * Set arg
             */
            /*
             * At this point, 'type' can only be:
             *   bcmCosqControlBandwidthBurstMax:
             *   bcmCosqControlBandwidthBurstMaxEmptyQueue:
             *   bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
             * Compiler remarks on missing 'type's and
             * Coverity does not accept 'default' then (although it is better practice).
             */
            if (type == bcmCosqControlBandwidthBurstMax)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_port_priority_max_burst_get
                                (unit, core, tm_port, prio, (uint32 *) arg));
            }
            else if (type == bcmCosqControlBandwidthBurstMaxEmptyQueue)
            {
                SHR_IF_ERR_EXIT(dnx_ofp_rates_port_priority_max_burst_for_empty_queues_get(unit, (uint32 *) arg));
            }
            else
            {
                /*
                 * This is: case bcmCosqControlBandwidthBurstMaxFlowControlledQueue
                 */
                SHR_IF_ERR_EXIT(dnx_ofp_rates_port_priority_max_burst_for_fc_queues_get(unit, (uint32 *) arg));
            }
            break;
        }
        case bcmCosqControlPrioritySelect:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_scheduler_queue_sched_get(unit, port, cosq, arg, &dummy));
            break;
        }
        default:
        {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit, "unit %d, Invalid type %d\n"), unit, type));
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Get various features depnded control type
 * for the e2e port tc
 */
static int
dnx_cosq_control_egress_port_tcg_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
    if (!BCM_COSQ_GPORT_IS_PORT_TCG(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid port parameter 0x%x, function support only port tcg", unit, type);
    }
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Get burst max for egq rate shaper 
             */
            int core;
            uint32 tm_port;
            dnx_tcg_ndx_t tcg;
            bcm_port_t logical_port;
            /*
             * Validate gport 
             */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));
            SHR_IF_ERR_EXIT(dnx_cosq_bcm_cosq_to_tcg(unit, logical_port, cosq, &tcg));
            SHR_IF_ERR_EXIT(dnx_cosq_port_egq_tcg_validate(unit, port, tcg));
            SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, port, &tm_port, &core));
            /*
             * Get existing configuration 
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_tcg_max_burst_get(unit, core, tm_port, tcg, (uint32 *) arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose:
 * Get various features depnded control type
 * for the egress port scheduler 
 */
int
dnx_cosq_control_egress_port_scheduler_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int port_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d\nCosq must be set to 0 in case of Local gport", cosq);
    }
    switch (type)
    {
        case bcmCosqControlPacketLengthAdjust:
        {
            SHR_IF_ERR_EXIT(dnx_egr_port_compensation_get(unit, BCM_GPORT_LOCAL_GET(port), arg));
            break;
        }
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Get burst max for egq rate shaper 
             */
            int core;
            uint32 tm_port;

            SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, port, &tm_port, &core));
            /*
             * Verify 
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_single_port_max_burst_get(unit, core, tm_port, (uint32 *) arg));
            break;
        }
        case bcmCosqControlBandwidthBurstMaxEmptyQueue:
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rates_max_credit_empty_port_get(unit, arg));
            break;
        }
        case bcmCosqControlEgressPriorityOverCast:
        {
            /*
             * Get SP before WFQ mode (per OTM port) 
             */
            port_index = BCM_GPORT_LOCAL_GET(port);
            SHR_IF_ERR_EXIT(dnx_egr_queuing_prio_over_cast_get(unit, port_index, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Set various features depnded control type
 * for the egress interface scheduler (father of port)
 */
int
dnx_cosq_control_egress_interface_scheduler_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid cosq control type %d\n", type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Map egress port to a specific multicast configuration (out_tc,out_dp)
 */
static int
dnx_cosq_gport_egress_multicast_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    uint32 tm_port;
    int is_last, is_allocated, core;
    int old_profile, new_profile;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    dnx_egr_q_prio_mapping_type_e map_type;
    int queue_id;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&mapping_info, 0x0, sizeof(dnx_cosq_egress_queue_mapping_info_t));
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    /*
     * Validate input params 
     */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    if (ingress_pri < 0 || ingress_pri >= DNX_DEVICE_COSQ_ING_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_DEVICE_COSQ_ING_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }
    if (!(flags == BCM_COSQ_MULTICAST_SCHEDULED))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported flag parameter %08X", flags);
    }
    if (config->scheduled_dp < 0 || config->scheduled_dp > DNX_DEVICE_COSQ_EGR_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid scheduled_dp parameter %d", config->scheduled_dp);
    }
    SHR_IF_ERR_EXIT(dnx_egress_queue_from_cosq(unit, &queue_id, config->priority));
    if (queue_id < 0 || queue_id >= DNX_EGR_NOF_Q_PRIO)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid config->priority parameter %d", queue_id);
    }
    /*
     * Get Old data 
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get(unit, tm_port, core, &mapping_info));
    /*
     * Change specific entry - change new tc, new dp (scheduled_tc,scheduled_dp) 
     */
    /*
     * Schedule Multicast type 
     */
    map_type = DNX_EGR_MCAST_TO_UNSCHED;
    mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].dp = config->scheduled_dp;
    mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc = queue_id;
    /*
     * Exchange data 
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange
                    (unit, tm_port, core, &mapping_info, &old_profile, &is_last, &new_profile, &is_allocated));
    if (is_allocated)
    {
        /*
         * Create new profile 
         */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_queue_mapping_hw_set(unit, new_profile, core, &mapping_info));
    }
    /*
     * Map port to new profile 
     */
    SHR_IF_ERR_EXIT(dnx_egr_q_profile_map_set(unit, core, tm_port, new_profile));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * Retrieve specific multicast configuration (out_dp) from port
 */
static int
dnx_cosq_gport_egress_multicast_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    int core;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    dnx_egr_q_prio_mapping_type_e map_type;
    dnx_egr_q_priority_t egr_prio;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate input params 
     */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    dnx_egr_q_priority_clear(&egr_prio);
    if (ingress_pri < 0 || ingress_pri >= DNX_DEVICE_COSQ_ING_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_pri parameter %d", ingress_pri);
    }
    if (ingress_dp < 0 || ingress_dp >= DNX_DEVICE_COSQ_ING_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ingress_dp parameter %d", ingress_dp);
    }
    if (flags & BCM_COSQ_MULTICAST_UNSCHEDULED)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported flag parameter %08X", flags);
    }
    SHR_IF_ERR_EXIT(dnx_cosq_fap_port_get(unit, gport, &tm_port, &core));
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get(unit, tm_port, core, &mapping_info));
    /*
     * Multicast type 
     */
    map_type = DNX_EGR_MCAST_TO_UNSCHED;
    config->scheduled_dp = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].dp;
    config->priority = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc;
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * }
 */

int
dnx_cosq_bit_count_get(
    uint32 value)
{
    int count = 0;

    SHR_BITCOUNT_RANGE(&value, count, 0, 32);
    return count;
}
/* 
 * Purpose:
 * Get various features depnded control type
 * for the egress interface scheduler (father of port)
 */
int
dnx_cosq_control_egress_interface_scheduler_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (type)
    {
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d\n", unit, type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Purpose:
 * init egress port to a specific configuration (out_tc,out_dp)
 */
int
dnx_egr_queuing_default_tc_dp_map_set(
    int unit,
    bcm_port_t port)
{
    uint32 tm_port;
    int nof_priorities;
    int is_last, is_allocated, core;
    int old_profile, new_profile;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    int tc, dp, priority;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    /*
     * Get Old data 
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get(unit, tm_port, core, &mapping_info));
    for (tc = 0; tc < DNX_DEVICE_COSQ_EGR_NOF_TC; tc++)
    {
        /*
         * select Q
         */
        switch (nof_priorities)
        {
            case 1:
            {
                priority = 0;
                break;
            }
            case 2:
            {
                priority = (tc < 4 ? 0 : 1);
                break;
            }
            case 4:
            {
                priority = (tc >> 1);
                break;
            }
            case 8:
            default:
            {
                priority = tc;
                break;
            }
        }
        for (dp = 0; dp < DNX_DEVICE_COSQ_EGR_NOF_DP; dp++)
        {
            /*
             * set unsched traffic mapping 
             */
            mapping_info.queue_mapping[DNX_EGR_MCAST_TO_UNSCHED][tc][dp].dp = dp;
            mapping_info.queue_mapping[DNX_EGR_MCAST_TO_UNSCHED][tc][dp].tc = priority;
            /*
             * set sched traffic mapping
             */
            mapping_info.queue_mapping[DNX_EGR_UCAST_TO_SCHED][tc][dp].dp = dp;
            mapping_info.queue_mapping[DNX_EGR_UCAST_TO_SCHED][tc][dp].tc = priority;
        }
    }
    /*
     * Exchange data 
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange
                    (unit, tm_port, core, &mapping_info, &old_profile, &is_last, &new_profile, &is_allocated));
    if (is_allocated)
    {
        /*
         * Create new profile 
         */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_queue_mapping_hw_set(unit, new_profile, core, &mapping_info));
    }
    /*
     * Map port to new profile 
     */
    SHR_IF_ERR_EXIT(dnx_egr_q_profile_map_set(unit, core, tm_port, new_profile));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - BCM API function setting static (non FADT) threshold
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - additional flags. currently not in use
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in] threshold - threshold value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_static_threshold_set(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_static_threshold_verify(unit, flags, thresh_info, threshold));
    SHR_IF_ERR_EXIT(dnx_cosq_gport_static_threshold_internal_set(unit, flags, thresh_info, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify gport_static_threshold_set/get parameters
 */
int
dnx_cosq_gport_static_threshold_internal_verify(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(thresh_info, _SHR_E_PARAM, "thresh_info");
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "flags must be 0\n");

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
int
dnx_cosq_gport_static_threshold_internal_set(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    bcm_gport_t gport;
    dnx_cosq_threshold_index_type_t index_type;
    dnx_cosq_threshold_index_info_t index_info;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_static_threshold_internal_verify(unit, flags, thresh_info, threshold));

    gport = thresh_info->gport;
    SHR_IF_ERR_EXIT(dnx_cosq_threshold_index_decode(unit, thresh_info->index, &index_type, &index_info));

    if (BCM_COSQ_GPORT_IS_VSQ(gport))
    {
        if (BCM_COSQ_GPORT_IS_VSQ_GL(gport))
        {
            core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(gport);
            SHR_IF_ERR_EXIT(dnx_cosq_global_vsq_static_threshold_set(unit, core_id,
                                                                     flags,
                                                                     index_type, &index_info, thresh_info, threshold));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported vsq gport 0x%x\n", gport);
        }
    }
    else if (gport == 0)
    {
        /** global device thresholds */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_device_static_threshold_set(unit, flags,
                                                                                  index_type, &index_info,
                                                                                  thresh_info, threshold));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported  gport 0x%x\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - BCM API function getting static (non FADT) threshold
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - additional flags. currently not in use
 *   \param [in] thresh_info - complementary information describing the threshold to be get
 *   \param [in] threshold - obtained threshold value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_static_threshold_get(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_static_threshold_verify(unit, flags, thresh_info, threshold));
    SHR_IF_ERR_EXIT(dnx_cosq_gport_static_threshold_internal_get(unit, flags, thresh_info, threshold));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
int
dnx_cosq_gport_static_threshold_internal_get(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    bcm_gport_t gport;
    dnx_cosq_threshold_index_type_t index_type;
    dnx_cosq_threshold_index_info_t index_info;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_static_threshold_internal_verify(unit, flags, thresh_info, threshold));

    gport = thresh_info->gport;
    SHR_IF_ERR_EXIT(dnx_cosq_threshold_index_decode(unit, thresh_info->index, &index_type, &index_info));

    if (BCM_COSQ_GPORT_IS_VSQ(gport))
    {
        if (BCM_COSQ_GPORT_IS_VSQ_GL(gport))
        {
            core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(gport);
            SHR_IF_ERR_EXIT(dnx_cosq_global_vsq_static_threshold_get(unit, core_id,
                                                                     flags,
                                                                     index_type, &index_info, thresh_info, threshold));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported vsq gport 0x%x\n", gport);
        }
    }
    else if (gport == 0)
    {
        /** global device thresholds */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_global_device_static_threshold_get(unit, flags,
                                                                                  index_type, &index_info,
                                                                                  thresh_info, threshold));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported  gport 0x%x\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify pfc_config_set/get parameters
 */
static shr_error_e
dnx_cosq_pfc_config_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    if (!BCM_COSQ_GPORT_IS_VSQ(gport))
    {
        /** not supported gport */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected gport type 0x%x\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Configure FC thresholds for VSQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - VSQ gport
 * \param [in] cosq - irrelevant for regular VSQ, priority for global VSQ
 * \param [in] flags - flags
 * \param [in] config - FC configuration
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_pfc_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_pfc_config_verify(unit, gport, cosq, flags, config));

    /*
     *  PFC FC settings include the following:
     * 1. Global thresholds (VSQ global) 
     * 2. PFC VSQ FC thresholds (VSQ PFC)
     * 3. LLFC VSQ FC thresholds (VSQ LLFC)                              
     */
    if (BCM_COSQ_GPORT_IS_VSQ_GL(gport))
    {
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_vsq_gl_fc_threshold_set(unit, gport, cosq, flags, config));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_fc_threshold_set(unit, gport, cosq, flags, config));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get FC thresholds for VSQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - VSQ gport
 * \param [in] cosq - irrelevant for regular VSQ, priority for global VSQ
 * \param [in] flags - flags
 * \param [out] config - FC configuration
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_pfc_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * config)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_pfc_config_verify(unit, gport, cosq, flags, config));

    if (BCM_COSQ_GPORT_IS_VSQ_GL(gport))
    {
        /** vsq gl flow control thresholds */
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_vsq_gl_fc_threshold_get(unit, gport, cosq, flags, config));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_fc_threshold_get(unit, gport, cosq, flags, config));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuring ranges.
 *
 * Use cases:
 * ** Use case: "credit watchdog range"
 * Range of queues supporting credit watchdog mechanism.
 * Parameters:
 * - port - not used, set to 0, the configuration is symmetric (identical configuration for both cores)
 * - flags - not used, set to 0.
 * - type - set to bcmCosqWatchdogQueue.
 * - range.is_enabled - enable / disable watchdog mechanism
 * - range.range_start / range_end - min and max queue id 
 *  
 * * ** Use case: "Statistic Interface queues range"
 * Range of queues that produce statistics reports in queue size
 * mode for STIF. 
 * Parameters: 
 * - port - used to exctact the core
 * - flags - not used, set to 0.
 * - type - set to bcmCosqStatIfQueues.
 * - range.is_enabled - not in use, set to 1
 * - range.range_start / range_end - min and max queue id 
 *  
 * * ** Use case: "Statistic Interface scrubber queues range"
 * Range of queues that produce statistics scrubber reports in 
 * queue size mode for STIF. 
 * Parameters: 
 * - port - used to exctact the core
 * - flags - not used, set to 0.
 * - type - set to bcmCosqStatIfScrubberQueues.
 * - range.is_enabled - not in use, set to 1
 * - range.range_start / range_end - min and max queue id 
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - description per use case.
 * \param [in] flags - description per use case.
 * \param [in] type - description per use case.
 * \param [in] range - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_control_range_set(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_cosq_control_range_type_t type,
    bcm_cosq_range_t * range)
{
    int core;
    SHR_FUNC_INIT_VARS(unit);
    /** High Level Verify */
    SHR_NULL_CHECK(range, _SHR_E_PARAM, "range");

    /** Implementation per use case (type) */
    switch (type)
    {
            /*
             * Use case: "Credit Watchdog range"
             */
        case bcmCosqWatchdogQueue:
        {
            /** verify */
            SHR_VAL_VERIFY(port, 0, _SHR_E_PARAM, "expecting port == 0");
            SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "expecting flags == 0");

            /** set watchdog mechanism */
            SHR_IF_ERR_EXIT(dnx_iqs_api_wd_range_set(unit, range));

            break;
        }
            /*
             * Use case: "Statistic Interface queues range"
             */
        case bcmCosqStatIfQueues:
            /*
             * Use case: "Statistic Interface scrubber queues range"
             */
        case bcmCosqStatIfScrubberQueues:
        {
            /** verify */
            SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "expecting flags == 0");
            SHR_INVOKE_VERIFY_DNX(dnx_stif_mgmt_control_range_verify(unit, STIF_MGMT_VERIFY_SET, port, type, range));
            core = BCM_COSQ_GPORT_CORE_GET(port);
            /** set scrubber queues range */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_queues_range_dbal_set
                            (unit, core, type, range->range_start, range->range_end));
            break;
        }
            /*
             * Not supported types.
             */
        case bcmCosqThresholdDramMixDbuff:
        case bcmCosqOcbCommittedMulticast_1:
        case bcmCosqOcbCommittedMulticast_2:
        case bcmCosqOcbEligibleMulticast_1:
        case bcmCosqOcbEligibleMulticast_2:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "unit %d, type %d: buffer management controls not supported by device, See compatibility document for details.\n",
                         unit, type);
            break;
        case bcmCosqRangeMulticastQueue:
        case bcmCosqRangeShaperQueue:
        case bcmCosqRangeFabricQueue:
        case bcmCosqRecycleQueue:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unit %d, type %d is not supported on this device.\n", unit, type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for getting range configuration.
 * For detailed description refer to bcm_dnx_cosq_control_range_set()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - description per use case.
 * \param [in] flags - description per use case.
 * \param [in] type - description per use case.
 * \param [out] range - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_control_range_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_cosq_control_range_type_t type,
    bcm_cosq_range_t * range)
{
    int core;
    SHR_FUNC_INIT_VARS(unit);

    /** High Level Verify */
    SHR_NULL_CHECK(range, _SHR_E_PARAM, "range");

    /** Clear structure */
    sal_memset(range, 0, sizeof(bcm_cosq_range_t));

    /** Implementation per use case (type) */
    switch (type)
    {
            /*
             * Use case: "Credit Watchdog range"
             */
        case bcmCosqWatchdogQueue:
        {
            /** verify */
            SHR_VAL_VERIFY(port, 0, _SHR_E_PARAM, "expecting port == 0");
            SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "expecting flags == 0");

            /** set watchdog mechanism */
            SHR_IF_ERR_EXIT(dnx_iqs_api_wd_range_get(unit, range));
            break;
        }
            /*
             * Use case: "Statistic Interface queues range"
             */
        case bcmCosqStatIfQueues:
            /*
             * Use case: "Statistic Interface scrubber queues range"
             */
        case bcmCosqStatIfScrubberQueues:
        {
            /** verify */
            SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "expecting flags == 0");
            SHR_INVOKE_VERIFY_DNX(dnx_stif_mgmt_control_range_verify(unit, STIF_MGMT_VERIFY_GET, port, type, range));
            core = BCM_COSQ_GPORT_CORE_GET(port);
            /** get stif scrubber queues range */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_queues_range_dbal_get
                            (unit, core, type, &range->range_start, &range->range_end));
            break;
        }
        case bcmCosqThresholdDramMixDbuff:
        case bcmCosqOcbCommittedMulticast_1:
        case bcmCosqOcbCommittedMulticast_2:
        case bcmCosqOcbEligibleMulticast_1:
        case bcmCosqOcbEligibleMulticast_2:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "unit %d, type %d: buffer management controls not supported by device, See compatibility document for details.\n",
                         unit, type);
            break;

        case bcmCosqRangeMulticastQueue:
        case bcmCosqRangeShaperQueue:
        case bcmCosqRangeFabricQueue:
        case bcmCosqRecycleQueue:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unit %d, type %d is not supported on this device.\n", unit, type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate params for dnx_cosq_fabric_gport_sched_set.
 */
static shr_error_e
dnx_cosq_fabric_gport_sched_verify(
    int unit,
    int pipe,
    bcm_cos_queue_t cosq)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(pipe, 0, dnx_data_fabric.pipes.nof_pipes_get(unit) - 1, _SHR_E_PARAM, "pipe %d is not supported",
                     pipe);

    SHR_VAL_VERIFY(cosq, -1, _SHR_E_PARAM, "expecting cosq == -1");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set weight for WFQs in fabric pipes.
 * \param [in] unit -
 *   The unit number.
 * \param [in] gport -
 *   Contains the pipe number (0/1/2) to configure.
 * \param [in] cosq -
 *   Should be set to -1.
 * \param [in] mode -
 *   Should be set to 0.
 * \param [in] weight -
 *   The weight to configure.
 * \param [in] fabric_wfq_type -
 *   Which WFQ to configure:
 *     Ingress (FDT)
 *     Egress (FDR)
 *     All (Ingress+Egress)
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_cosq_fabric_gport_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight,
    dnx_fabric_wfq_type_e fabric_wfq_type)
{
    uint32 max_weight = 0;
    int pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fabric_gport_sched_verify(unit, pipe, cosq));
    SHR_VAL_VERIFY(mode, 0, _SHR_E_PARAM, "expecting mode == 0");
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_FABRIC_WFQ_CONTEXTS_WEIGHTS, DBAL_FIELD_WEIGHT, FALSE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &max_weight));
    SHR_RANGE_VERIFY(weight, 1, max_weight, _SHR_E_PARAM, "Weight %d is invalid. Weight must be between 1 and %d",
                     weight, max_weight);

    SHR_IF_ERR_EXIT(dnx_fabric_wfq_set(unit, pipe, weight, fabric_wfq_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get weight for WFQs in fabric pipes.
 * \param [in] unit -
 *   The unit number.
 * \param [in] gport -
 *   Contains the pipe number (0/1/2) to get configuration.
 * \param [in] cosq -
 *   Should be set to -1.
 * \param [out] mode -
 *   Would be set to 0.
 * \param [out] weight -
 *   The configured weight.
 * \param [in] fabric_wfq_type -
 *   Which WFQ to get configuration:
 *     Ingress (FDT)
 *     Egress (FDR)
 *     All (Ingress+Egress)
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_cosq_fabric_gport_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight,
    dnx_fabric_wfq_type_e fabric_wfq_type)
{
    int pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_fabric_gport_sched_verify(unit, pipe, cosq));
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");

    SHR_IF_ERR_EXIT(dnx_fabric_wfq_get(unit, pipe, weight, fabric_wfq_type));

    if (mode != NULL)
    {
        *mode = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuring scheduling elements.
 *
 * Use cases:
 *  ** Use case: "Ingress Queue to credit request profile"
 *  Assign ingress queue to credit request profile.
 *  When creating a ingress queue using API bcm_cosq_ingress_queue_bundle_gport_add() the queue assigned to credit request profile according to user input.
 *  This API will allow modifying the credit request profile.
 *  Parameters:
 *  - gport - Queue bundle gport. Created by API bcm_cosq_ingress_queue_bundle_gport_add().
 *  - cosq - queue offset in the bundle
 *  - mode - credit request profile (defined by API bcm_cosq_delay_tolerance_level_set())
 *  - weight - Not relevant (set to 0)
 *
 *  ** Use case: "Best Effort MC credit generation"
 *  Set weights of fabric multicast best effort credit generation contexts.
 *  By default will be set to strict priority.
 *  (Will not allow asymmetric configuration)
 *  Parameters:
 *  - gport - supported gports
 *              For best effort context 0 weight - use BCM_COSQ_GPORT_FMQ_BESTEFFORT0_SET.
 *              For best effort context 1 weight - use BCM_COSQ_GPORT_FMQ_BESTEFFORT1_SET.
 *              For best effort context 2 weight - use BCM_COSQ_GPORT_FMQ_BESTEFFORT1_SET.
 *  - cosq - not relevant, set to 0
 *  - mode - Set to -1 for weighted fair queue mode or 0 for strict priority mode
 *  - weight  - set required weight - supported weights 1 - 15. Lower value means higher priority.
 *
 *  ** Use case: "Scheduler TC"
 *  Attach TC to TCG for scheduler TC.
 *  Parameters:
 *  - gport - E2E_PORT_TC
 *  - cosq - priority (TC)
 *  - mode - TCG id
 *  - weight - Not relevant (set to -1)
 *
 *  ** Use case: "Scheduler TCG"
 *  Configure EIR weight for each TCG.
 *  Parameters:
 *  - gport - E2E_PORT_TCG
 *  - cosq - TCG id
 *  - mode - Not relevant (set to -1)
 *  - weight - TCG EIR weight
 *
 *  ** Use case: "E2E hierarchy"
 *  Define how child flow is connected to parent SE
 *  Parameters:
 *  - gport - scheduler gport.
 *  - cosq - for connector - offset of the connector from base connector, for SE - 0 
 *  - mode - the parent priority (FQ/WFQ) to connect to (BCM_COSQ_SP0, BCM_COSQ_SP1, etc.)
 *  - weight - weight for  connecting  to WFQ
 *
 *
 *  ** Use case: "fabric pipes WFQ"
 *  Configure various WFQs between pipes in various locations in the Fabric.
 *  Parameters:
 *  - gport - holds:
 *              * FIFO for WFQ:
 *                  bcm_cosq_gport_handle_set/get handler can set the following types:
 *                      bcmCosqGportTypeFabricPipeIngress
 *                      bcmCosqGportTypeFabricPipeEgress
 *                      bcmCosqGportTypeFabricPipe (both ingress & egress)
 *              * Pipe-id.
 *  - cosq - Not relevant (set to -1).
 *  - mode - Not relevant (set to 0).
 *  - weight - the weight to configure.
 *             Supported weights: 1-127.
 *
 *  ** Use case: "Egress queue weights"
 *  queue (unicast or multicast) weight configuration.
 *  Parameters:
 *  - gport - For unicast queue - BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET
 *            For multicast queue - BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET
 *  - cosq - TC priority.
 *  - mode - BCM_COSQ_SP0 - designates the corresponding queue (either unicast or multicast) as having strict priority over the other queue in the queue-pair.
 *           BCM_COSQ_SP1 - designates the corresponding queue (either unicast or multicast) as having strict priority over the other queue in the queue-pair.
 *           -1 - weight taken into consideration
 *  - weight - qpair weight. Range is between 1:256.
 *
 *  ** Use case: "RCY EIR shaper weights"
 *  Configure recycle EIR shaper weights.
 *  Parameters:
 *  - gport - BCM_COSQ_GPORT_RCY_SET
 *  - cosq - Not relevant (set to 0).
 *  - mode - -1, not relevant.
 *  - weight - Range is 1-255. To disable EIR set weight=0.
 *
 *  ** Use case: "Mirror RCY EIR shaper weights"
 *  Configure mirror recycle EIR shaper weights.
 *  Parameters:
 *  - gport - BCM_COSQ_GPORT_RCY_MIRR_SET
 *  - cosq - Not relevant (set to 0).
 *  - mode - -1, not relevant.
 *  - weight - Range is 1-255. To disable EIR set weight=0.
 *
 ** Use case: "TC <> TCG Mapping"
 *  Configure TC <> TCG Mapping.
 *  Parameters:
 *  - gport - BCM_COSQ_GPORT_PORT_TC_SET
 *  - cosq - TC priority.
 *  - mode - TCG id.
 *  - weight - -1, not relevant.
 *
 ** Use case: "TCG EIR weights"
 *  Configure TCG EIR weights.
 *  Parameters:
 *  - gport - BCM_COSQ_GPORT_PORT_TCG_SET
 *  - cosq - TCG id.
 *  - mode - Not relevant (set to 0).
 *  - weight - TCG weight. Range is 1;1K. To disable EIR set weight=0.
 *
 ** Use case: "Port priority"
 *  configure port priority.
 *  Parameters:
 *  - gport - BCM_GPORT_LOCAL_SET
 *  - cosq - 0.
 *  - mode - BCM_COSQ_SP0
 *           BCM_COSQ_SP1.
 *  - weight - -1, not relevant.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - description per use case.
 * \param [in] cosq - description per use case.
 * \param [in] mode - description per use case.
 * \param [in] weight - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Implementation per use case (gport type) */

    /*
     * Use case: "Ingress Queue to credit request profile"
     */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        /** Verify */
        SHR_VAL_VERIFY(weight, 0, _SHR_E_PARAM, "expecting weight == 0");

        /** map queue to profile */
        SHR_IF_ERR_EXIT(dnx_iqs_api_queue_to_request_profile_set(unit, gport, cosq, mode));
    }
    /*
     * Use case: "Best Effort MC credit generation"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(gport)
             || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(gport) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(gport))
    {
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_gport_fmq_sched_set(unit, gport, cosq, mode, weight));
    }
    /** unsupported gport */
    else if (BCM_GPORT_IS_FABRIC_CLOS(gport) || BCM_GPORT_IS_FABRIC_MESH(gport))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */

        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Ingress transmit scheduler is not supported on this device\n");
    }
    /*
     * Use case: "E2E hierarchy"
     */
    else if ((BCM_GPORT_IS_SCHEDULER(gport)) || (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) ||
             (BCM_COSQ_GPORT_IS_SCHED_CIR(gport)) || (BCM_COSQ_GPORT_IS_SCHED_PIR(gport)) ||
             (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport)) || (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport)))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_sched_set(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case "Scheduler TC": attach TC to TCG
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_sched_set(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case "Scheduler TCG": Configure EIR weight for each TCG
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_sched_set(unit, gport, cosq, mode, weight));
    }

    /*
     * Use case: "Egress queue weights"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_queue_sched_set(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case: "RCY EIR shaper weights"
     */
    else if (BCM_COSQ_GPORT_IS_RCY(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_weight_set(unit, BCM_COSQ_GPORT_CORE_GET(gport), 0, weight));
    }
    /*
     * Use case: "Mirror RCY EIR shaper weights"
     */
    else if (BCM_COSQ_GPORT_IS_RCY_MIRR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_weight_set(unit, BCM_COSQ_GPORT_CORE_GET(gport), 1, weight));
    }
    /*
     * Use case: "tc <> TCG mapping"
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tc_sched_set(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case: "TCG EIR weights"
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tcg_sched_set(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case: "fabric pipes WFQ"
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_fabric_gport_sched_set
                        (unit, gport, cosq, mode, weight, DNX_FABRIC_WFQ_TYPE_CONTEXT_INGRESS));
    }
    /*
     * Use case: "fabric pipes WFQ"
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_fabric_gport_sched_set
                        (unit, gport, cosq, mode, weight, DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS));
    }
    /*
     * Use case: "fabric pipes WFQ"
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_fabric_gport_sched_set(unit, gport, cosq, mode, weight, DNX_FABRIC_WFQ_TYPE_ALL));
    }
    /*
     * Use case: "port priority"
     * (Legacy implementation)
     */
    else if (BCM_GPORT_IS_LOCAL(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_scheduler_port_sched_set(unit, gport, cosq, mode, weight));
    }
    /*
     * Gport not supported
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(gport) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(gport) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(gport) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Function is not supported. Please use bcm_cosq_gport_dynamic_sched_set() instead.");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "gport not supported 0x%x", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for getting configuration of scheduling elements.
 * For detailed description refer to \ref bcm_dnx_cosq_gport_sched_get()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - description per use case.
 * \param [in] cosq - description per use case.
 * \param [out] mode - description per use case.
 * \param [out] weight - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Implementation per use case (gport type) */

    /*
     * Use case: "Ingress Queue to credit request profile"
     */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        /** map queue to profile */
        SHR_IF_ERR_EXIT(dnx_iqs_api_queue_to_request_profile_get(unit, gport, cosq, mode));
    }
    /*
     * Use case: "Best Effort MC credit generation"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(gport)
             || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(gport) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(gport))
    {
        /** High level verify */
        SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
        SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");

        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_gport_fmq_sched_get(unit, gport, cosq, mode, weight));
    }
    /** unsupported gport */
    else if (BCM_GPORT_IS_FABRIC_CLOS(gport) || BCM_GPORT_IS_FABRIC_MESH(gport))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */

        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Ingress transmit scheduler is not supported on this device\n");
    }
    /*
     * Use case: "E2E hierarchy"
     * (Legacy implementation)
     */
    else if ((BCM_GPORT_IS_SCHEDULER(gport)) || (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) ||
             (BCM_COSQ_GPORT_IS_SCHED_CIR(gport)) || (BCM_COSQ_GPORT_IS_SCHED_PIR(gport)) ||
             (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport)) || (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport)))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_sched_get(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case "Scheduler TC": attach TC to TCG
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_sched_get(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case "Scheduler TCG": Configure EIR weight for each TCG
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_sched_get(unit, gport, cosq, mode, weight));
    }

    /*
     * Use case: "Egress queue weights"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) || BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_queue_sched_get(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case: "RCY EIR shaper weights"
     */
    else if (BCM_COSQ_GPORT_IS_RCY(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_weight_get(unit, BCM_COSQ_GPORT_CORE_GET(gport), 0, (uint32 *) weight));
    }
    /*
     * Use case: "Mirror RCY EIR shaper weights"
     */
    else if (BCM_COSQ_GPORT_IS_RCY_MIRR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_weight_get(unit, BCM_COSQ_GPORT_CORE_GET(gport), 1, (uint32 *) weight));
    }
    /*
     * Use case: "tc <> TCG mapping"
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tc_sched_get(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case: "TCG EIR weights"
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tcg_sched_get(unit, gport, cosq, mode, weight));
    }
    /*
     * Use case 4: Pipe Scheduling
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_fabric_gport_sched_get
                        (unit, gport, cosq, mode, weight, DNX_FABRIC_WFQ_TYPE_CONTEXT_INGRESS));
    }
    /*
     * Use case: "fabric pipes WFQ"
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_fabric_gport_sched_get
                        (unit, gport, cosq, mode, weight, DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS));
    }
    /*
     * Use case: "fabric pipes WFQ"
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_fabric_gport_sched_get(unit, gport, cosq, mode, weight, DNX_FABRIC_WFQ_TYPE_ALL));
    }
    /*
     * Use case: "port priority"
     * (Legacy implementation)
     */
    else if (BCM_GPORT_IS_LOCAL(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_scheduler_port_sched_get(unit, gport, cosq, mode, weight));
    }
    /*
     * Gport not supported
     */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(gport) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(gport) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(gport) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Function is not supported. Please use bcm_cosq_gport_dynamic_sched_set() instead.");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "gport not supported 0x%x", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate params for bcm_dnx_cosq_gport_dynamic_sched_set.
 */
static shr_error_e
dnx_cosq_gport_dynamic_sched_set_verify(
    int unit,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int weight)
{
    uint32 max_weight = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_VAL_VERIFY(cosq, -1, _SHR_E_PARAM, "expecting cosq == -1");

    if (state != bcmCosqDynamicStateNormal && state != bcmCosqDynamicStateCongested)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "state must be either bcmCosqDynamicStateNormal or bcmCosqDynamicStateCongested");
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_FABRIC_WFQ_CONTEXTS_WEIGHTS, DBAL_FIELD_WEIGHT, FALSE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &max_weight));
    SHR_RANGE_VERIFY(weight, 1, max_weight, _SHR_E_PARAM, "Weight %d is invalid. Weight must be between 1 and %d",
                     weight, max_weight);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set configuration of dynamic fabric WFQs.
 * For dynamic WFQs there are 2 sets of configurations, a configuration for
 * normal state and a configuration of congested state. When the relevant threshold
 * is crossed, the WFQ configuration switches from 'normal' to 'congested' and vice
 * versa.
 *
 * \param [in] unit -
 *   Unit-ID.
 * \param [in] gport -
 *   Which entity to set the WFQ weight upon:
 *     BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS - Configure a weight for Egress pipe.
 *     BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST - Configure a weight for local Unicast.
 *     BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_MCAST - Configure a weight for Mesh Multicast.
 * \param [in] cosq -
 *   not in use. should be set to -1.
 * \param [in] state -
 *   Which of the WFQ states to configure:
 *     bcmCosqDynamicStateNormal - Configure WFQ for normal state.
 *     bcmCosqDynamicStateCongested - Configure WFQ for congested state.
 * \param [in] weight -
 *   The weight to configure.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_dynamic_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int weight)
{
    int is_congested = (state == bcmCosqDynamicStateCongested) ? 1 : 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Input validation
     */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_dynamic_sched_set_verify(unit, cosq, state, weight));

    /** Configure FDA WFQ weight for a pipe */
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        dbal_enum_value_field_fabric_wfq_fifo_e wfq_config = DBAL_NOF_ENUM_FABRIC_WFQ_FIFO_VALUES;
        int pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);

        /** validate pipe */
        SHR_RANGE_VERIFY(pipe, 0, dnx_data_fabric.pipes.nof_pipes_get(unit) - 1, _SHR_E_PARAM,
                         "pipe %d is not supported", pipe);

        switch (pipe)
        {
            case 0:
            {
                wfq_config = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_0;
                break;
            }
            case 1:
            {
                wfq_config = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_1;
                break;
            }
            case 2:
            {
                wfq_config = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_2;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Pipe ID %d is not supported", pipe);
            }
        }
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_set(unit, wfq_config, is_congested, weight));
    }
    /** Configure FDA WFQ weight for local unicast */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(gport))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_set
                        (unit, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_LOCAL_UC, is_congested, weight));
    }
    /** Configure FDA WFQ weight for Mesh multicast */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_MCAST(gport))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_set(unit, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_MESH_MC, is_congested, weight));
    }
    /*
     * Gport not supported
     */
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "gport not supported 0x%x", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate params for bcm_dnx_cosq_gport_dynamic_sched_get.
 */
static shr_error_e
dnx_cosq_gport_dynamic_sched_get_verify(
    int unit,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_VAL_VERIFY(cosq, -1, _SHR_E_PARAM, "expecting cosq == -1");

    if (state != bcmCosqDynamicStateNormal && state != bcmCosqDynamicStateCongested)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "state must be either bcmCosqDynamicStateNormal or bcmCosqDynamicStateCongested");
    }

    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_gport_dynamic_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int *weight)
{
    int is_congested = (state == bcmCosqDynamicStateCongested) ? 1 : 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Input validation
     */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_dynamic_sched_get_verify(unit, cosq, state, weight));

    /** Get FDA WFQ weight for a pipe */
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        dbal_enum_value_field_fabric_wfq_fifo_e wfq_config = DBAL_NOF_ENUM_FABRIC_WFQ_FIFO_VALUES;
        int pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);

        /** validate pipe */
        SHR_RANGE_VERIFY(pipe, 0, dnx_data_fabric.pipes.nof_pipes_get(unit) - 1, _SHR_E_PARAM,
                         "pipe %d is not supported", pipe);

        switch (pipe)
        {
            case 0:
            {
                wfq_config = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_0;
                break;
            }
            case 1:
            {
                wfq_config = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_1;
                break;
            }
            case 2:
            {
                wfq_config = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_2;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Pipe ID %d is not supported", pipe);
            }
        }
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_get(unit, wfq_config, is_congested, weight));
    }
    /** Configure FDA WFQ weight for any pipes (return pipe 0 weight) */
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_get
                        (unit, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_0, is_congested, weight));
    }
    /** Get FDA WFQ weight for local unicast */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(gport))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_get
                        (unit, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_LOCAL_UC, is_congested, weight));
    }
    /** Get FDA WFQ weight for Mesh multicast */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_MCAST(gport))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_wfq_dynamic_get(unit, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_MESH_MC, is_congested, weight));
    }
    /*
     * Gport not supported
     */
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "gport not supported 0x%x", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic traverse function performing user specified callback
 * on all active VOQs, VOQ connectors and Scheduling elements
 *
 * \param [in] unit -  Unit-ID
 * \param [in] cb - the callback to be performed
 * \param [in] user_data - additional data to the callback
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_traverse(
    int unit,
    bcm_cosq_gport_traverse_cb cb,
    void *user_data)
{
    int voq = 0, nbr_cos = 0;
    bcm_gport_t gport = 0;
    int is_multicast = FALSE;
    int flow_id = 0, flow_id_temp = 0, flow_region;
    int is_interdigitated;
    int first_se_flow_id;
    int core = 0;
    uint8 sw_state_num_cos;
    int region_index;
    int has_connector;
    int is_allocated;
    int is_reserved;
    int i, is_se, is_composite;
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
    int region;
    int skip[8];
    int granularity;

    SHR_FUNC_INIT_VARS(unit);

    first_se_flow_id = dnx_data_sch.flow.first_se_flow_id_get(unit);

    /*
     * walk through all the queues in the system, create gport and call callback function for each unicast or multicast 
     * gport, passing user data to cb function 
     */
    for (voq = 0; voq < dnx_data_ipq.queues.nof_queues_get(unit); voq += dnx_data_ipq.queues.min_bundle_size_get(unit))
    {
        /*
         * VOQ are always symmetrical
         * use CORE_ALL for core id.
         */

        SHR_IF_ERR_EXIT(dnx_ipq_queue_num_cos_get(unit, voq, &nbr_cos));

        /*
         * Queue not in use, go to the next 
         */
        if (nbr_cos == 0)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_ipq_queue_is_multicast(unit, voq, &is_multicast));

        if (is_multicast)
        {
            /** multicast */
            BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(gport, BCM_CORE_ALL, voq);
        }
        else
        {
            /** unicast */
            BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport, BCM_CORE_ALL, voq);
        }

        /** call the callback */
        SHR_IF_ERR_EXIT(cb(unit, 0 /* port */ , nbr_cos, 0 /* flags */ , gport, user_data));

    }
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (region_index = 0; region_index < dnx_data_sch.flow.nof_regions_get(unit); region_index++)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_region_has_connector(unit, core, region_index, &has_connector));

            if (!has_connector)
            {
                continue;
            }

            /** Iterate all valid connectors */
            for (flow_id_temp = 0; flow_id_temp < dnx_data_sch.flow.region_size_get(unit);
                 flow_id_temp += dnx_data_sch.flow.min_connector_bundle_size_get(unit))
            {

                flow_id = flow_id_temp + region_index * dnx_data_sch.flow.region_size_get(unit);

                if (flow_id > first_se_flow_id)
                {
                    flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);
                    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core, flow_region,
                                                                               &is_interdigitated));
                    if (is_interdigitated)
                    {
                        flow_id += 2;       /** interdigitated connectors */
                    }
                }
                SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.get(unit, core, flow_id, &sw_state_num_cos));
                nbr_cos = sw_state_num_cos;

                /** Connector not in use, go to the next */
                if (nbr_cos == 0)
                {
                    continue;
                }

                BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport, flow_id, core);

                /** call the callback */
                SHR_IF_ERR_EXIT(cb(unit, 0 /* port */ , nbr_cos, 0 /* flags */ , gport, user_data));
            }
        }
        /** Iterate all valid SEs */
        for (flow_id = first_se_flow_id; flow_id < dnx_data_sch.flow.nof_flows_get(unit); flow_id += 8)
        {
            SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, core, flow_id, &shared_shaper_mode));

            switch (shared_shaper_mode)
            {
                case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL:
                    granularity = 2;

                    break;
                case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD:
                    granularity = 4;

                    break;
                case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA:
                    granularity = 8;
                    break;
                default:
                    granularity = 1;
                    break;
            }

            for (i = 0; i < 8; i++)
            {
                skip[i] = (i % granularity == 0) ? FALSE : TRUE;
            }

            for (i = 0; i < 8; i++)
            {
                if (skip[i])
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id + i, &is_allocated));

                if (!is_allocated)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, flow_id + i, &is_se));

                if (!is_se)
                {
                    continue;
                }

                /** verify this is not reverved element */
                SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_reserved_element(unit, core, flow_id + i, &is_reserved));
                if (is_reserved)
                {
                    continue;
                }

                BCM_GPORT_SCHEDULER_CORE_SET(gport, flow_id + i, core);

                /** call the callback */
                SHR_IF_ERR_EXIT(cb(unit, 0 /* port */ , 1, 0 /* flags */ , gport, user_data));

                /** determine is composite flow attributes */
                SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, core, flow_id + i, &is_composite));

                if (is_composite)
                {
                    region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id + i);
                    SHR_IF_ERR_EXIT(dnx_scheduler_region_odd_even_mode_get(unit, core, region, &odd_even_mode));
                    if (odd_even_mode == DNX_SCHEDULER_REGION_ODD)
                    {
                        /** odd + even */
                        skip[i + 1] = TRUE;
                    }
                    else
                    {
                        /** odd + odd, even + even */
                        skip[i + 2] = TRUE;

                    }
                }

            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_cosq_control_device_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {

        case bcmCosqControlFlowControlEnable:
            if (arg != 0 && arg != 1) /** i.e. invalid value */
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid set argument %d, should be 0 or 1", unit, arg);
            }
            SHR_IF_ERR_EXIT(dnx_fc_cfc_global_enable_set(unit, arg));
            break;
        case bcmCosqControlEgressBandwidthEnable:
            if (arg != 0 && arg != 1) /** i.e. invalid value */
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid set argument %d, should be 0 or 1", unit, arg);
            }
            SHR_IF_ERR_EXIT(dnx_dbg_egress_shaping_enable_set(unit, arg));
            break;

        case bcmCosqControlSourcePortSchedCompensationEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_scheduler_per_port_enable_set(unit, port, cosq, arg));
            break;

        case bcmCosqControlEgressTCGCirSpEnable:
            if (arg != 0 && arg != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid set argument %d, should be 0 or 1", unit, arg);
            }
            SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_cir_sp_en_set(unit, port, arg));
            break;
        case bcmCosqControlMulticastPriorityIngressScheduling:
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_to_multicast_prio_map_set(unit, cosq, arg));
            break;
        case bcmCosqControlPriorityPropagationEnable:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Control bcmCosqControlPriorityPropagationEnable not supported by device. See UM for the supported set of APIs\n");
            break;
        case bcmCosqControlFlowSlowRate1:
        case bcmCosqControlFlowSlowRate2:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "The type %d is not supported on this device. Please use bcm_cosq_slow_profile_set() instead",
                         type) break;
        case bcmCosqControlIngressPortDropTpid1:
        case bcmCosqControlIngressPortDropTpid2:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "The type %d is not supported for core gport, only for local port.", type) break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_cosq_control_device_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlFlowControlEnable:
        {
            uint8 enable;
            SHR_IF_ERR_EXIT(dnx_fc_cfc_global_enable_get(unit, &enable));
            *arg = enable ? 1 : 0;
            break;
        }
        case bcmCosqControlEgressBandwidthEnable:
        {
            uint8 enable;
            SHR_IF_ERR_EXIT(dnx_dbg_egress_shaping_enable_get(unit, &enable));
            *arg = enable ? 1 : 0;
            break;
        }
        case bcmCosqControlSourcePortSchedCompensationEnable:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_scheduler_per_port_enable_get(unit, port, cosq, arg));
            break;
        }
        case bcmCosqControlEgressTCGCirSpEnable:
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_cir_sp_en_get(unit, port, (uint32 *) arg));
            break;
        }
        case bcmCosqControlMulticastPriorityIngressScheduling:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_tc_to_multicast_prio_map_get(unit, cosq, arg));
            break;
        }
        case bcmCosqControlPriorityPropagationEnable:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Control bcmCosqControlPriorityPropagationEnable not supported by device. See UM for the supported set of APIs\n");
            break;
        }
        case bcmCosqControlFlowSlowRate1:
        case bcmCosqControlFlowSlowRate2:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "The type %d is not supported on this device. Please use bcm_cosq_slow_profile_set() instead\n",
                         type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_control_scheduler_fabric_cgm_control_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    bcm_fabric_cgm_control_type_t * control_type,
    bcm_fabric_cgm_control_id_t * control_id)
{
    bcm_fabric_mc_shaper_t fmc_shaper = -1;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(port))
    {
        fmc_shaper = bcmFabricMcShaperGfmc;
    }
    else if (BCM_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(port))
    {
        fmc_shaper = bcmFabricMcShaperBfmc;
    }
    else if (BCM_GPORT_IS_FABRIC_MESH_MC(port))
    {
        fmc_shaper = bcmFabricMcShaperTotalFmc;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Slow Start is not supported to gport %u", port);
    }

    switch (type)
    {
        case bcmCosqControlFlowSlowRate:
        {
            /** Enable/Disable slow start mechanism for multicast queues */
            *control_type = bcmFabricCgmSlowStartEn;
            *control_id = fmc_shaper;
            break;
        }
        case bcmCosqControlFlowSlowRate1:
        {
            /*
             * Configure first rate of slow start mechanism
             */
            *control_type = bcmFabricCgmSlowStartRateShaperPhaseParam;
            *control_id = BCM_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SET(fmc_shaper, bcmFabricPhase0);
            break;
        }
        case bcmCosqControlFlowSlowRate2:
        {
            /*
             * Configure first rate of slow start mechanism
             */
            *control_type = bcmFabricCgmSlowStartRateShaperPhaseParam;
            *control_id = BCM_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SET(fmc_shaper, bcmFabricPhase1);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported control type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_control_scheduler_fabric_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    bcm_fabric_cgm_control_type_t control_type = -1;
    bcm_fabric_cgm_control_id_t control_id = -1;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlFlowSlowRate:
        case bcmCosqControlFlowSlowRate1:
        case bcmCosqControlFlowSlowRate2:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_control_scheduler_fabric_cgm_control_get
                            (unit, port, cosq, type, &control_type, &control_id));
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, 0, control_type, control_id, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_control_scheduler_fabric_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    bcm_fabric_cgm_control_type_t control_type = -1;
    bcm_fabric_cgm_control_id_t control_id = -1;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlFlowSlowRate:
        case bcmCosqControlFlowSlowRate1:
        case bcmCosqControlFlowSlowRate2:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_control_scheduler_fabric_cgm_control_get
                            (unit, port, cosq, type, &control_type, &control_id));
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get(unit, 0, control_type, control_id, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_control_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    int is_vsq_allowed = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_rate_class_gport_verify(unit, port, cosq, is_vsq_allowed));

    switch (type)
    {
        case bcmCosqControlOCBOnly:
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_ocb_only_set(unit, port, cosq, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported control type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_control_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int is_vsq_allowed = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_rate_class_gport_verify(unit, port, cosq, is_vsq_allowed));

    switch (type)
    {
        case bcmCosqControlOCBOnly:
        {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_ocb_only_get(unit, port, cosq, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported control type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Validate function for bcm_dnx_cosq_control_set/get APIs
 */
static shr_error_e
dnx_cosq_control_validate(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlDefaultInvalidQueue)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Default Invalid Queue mapping is not supported on this device");
    }

    /**
     * Validations specific only for bcm_dnx_cosq_control_set API
     */
    if (is_set)
    {
        if (type == bcmCosqControlIsInDram)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcmCosqControlIsInDram can be used only in bcm_cosq_control_get");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuring various cosq controls
 *
 * Use cases:
 * 
 * ** Use case: "E2E_PORT_TC Max Burst"
 * Set Max Burst for a shaper on e2e port TC
 *  Parameters:
 *  - port - E2E_PORT_TC
 *  - cosq - TC
 *  - type - bcmCosqControlBandwidthBurstMax
 *  - arg - maximal burst
 *
 * ** Use case: "E2E_PORT_TCG Max Burst"
 * Set Max Burst for a shaper on e2e port TCG
 *  Parameters:
 *  - port - E2E_PORT_TCG
 *  - cosq - TCG
 *  - type - bcmCosqControlBandwidthBurstMax
 *  - arg - maximal burst
 *
 * ** Use case: "Flow Max Burst"
 * Set Max Burst for a shaper on a flow
 *  Parameters:
 *  - port - SE or voq connector gport
 *  - cosq - not relevant, should be 0 or -1
 *  - type - bcmCosqControlBandwidthBurstMax
 *  - arg - maximal burst
 *
 * ** Use case: "High Priority Flow"
 * Set flow to be High Priority
 *  Parameters:
 *  - port - SE or voq connector gport
 *  - cosq - not relevant, should be 0 or -1
 *  - type - bcmCosqControlIsHighPriority
 *  - arg - 1, for high priority, 0, for low priority 
 *
 * ** Use case: "Remote Flow"
 * Set flow to be remote
 *  Parameters:
 *  - port - SE or voq connector gport
 *  - cosq - not relevant, should be 0 or -1
 *  - type - bcmCosqControlIsFabric
 *  - arg - 1, for remote, 0, for local
 *
 * ** Use case: "CL Discrete Weight"
 * Configure weight for Discrete CL
 *  Parameters:
 *  - port - SE  gport of CL
 *  - cosq - not relevant, should be 0 or -1
 *  - type - 
 *         * bcmCosqControlDiscreteWeightLevel0
 *         * bcmCosqControlDiscreteWeightLevel1
 *         * bcmCosqControlDiscreteWeightLevel2
 *         * bcmCosqControlDiscreteWeightLevel3
 *  - arg - weight
 *
 * ** Use case: "Connector Slow Rate Profile"
 * Configure VOQ connector to use specified slow rate profile
 *  Parameters:
 *  - port - base voq connector gport
 *  - cosq - connector number in bundle
 *  - type - bcmCosqControlFlowSlowRate
 *  - arg 
 *          * 0 - slow disable
 *          * 1 - profile defined by bcmCosqControlFlowSlowRate1
 *          * 2 - profile defined by bcmCosqControlFlowSlowRate1
 *
 * ** Use case: "OCB only"
 * Set VOQ to be OCB-only. Done by setting rate class ocb-only attribute.
 *  Parameters:
 *  - port - rate class gport (profile gport)
 *  - cosq - not relevant, set to 0
 *  - type - bcmCosqControlOCBOnly
 *  - arg - 0 or 1
 *
 *  ** Use case: "Port scheduler compensation"
 *  Enable  Per In-Port Scheduler compensation
 *  Parameters:
 *  - port - 0 - global device configuration
 *  - cosq - not relevant, set to 0
 *  - type - bcmCosqControlSourcePortSchedCompensationEnable
 *  - arg - 0 or 1 (disable or enable)
 *
 * ** Use case: "Slow Start Enable"
 * Enable FMQs Slow Start
 *  Parameters:
 *  - port - one of bcmCosqGportTypeGlobalFabricClos{FmcGuaranteed|FmcBestEffort|MeshMc}
 *  - cosq - not relevant, set to 0
 *  - type - bcmCosqControlFlowSlowRate
 *  - arg - 0 or 1 (disable or enable)
 *
 * ** Use case: "Slow Start Rate"
 * Set FMQs Slow Start Rate
 *  Parameters:
 *  - port - one of bcmCosqGportTypeGlobalFabricClos{FmcGuaranteed|FmcBestEffort|MeshMc}
 *  - cosq - not relevant, set to 0
 *  - type - bcmCosqControlFlowSlowRate{1|2}
 *  - arg - 0-100 - percentage from full rate.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - description per use case.
 * \param [in] cosq - description per use case.
 * \param [in] type - description per use case.
 * \param [in] arg  - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_cosq_control_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_control_validate(unit, port, cosq, type, TRUE));
    /*
     * Use cases: "Port scheduler compensation" , ....
     */
    if (port == 0 || BCM_COSQ_GPORT_IS_CORE(port))
    {
        /** device wide settings */
        SHR_IF_ERR_EXIT(dnx_cosq_control_device_set(unit, port, cosq, type, arg));
    }
    else if (!BCM_GPORT_IS_SET(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_set(unit, port, type, arg));
    }
    /** FMQ */
    else if (BCM_COSQ_GPORT_IS_FMQ_ROOT(port) /** FMQ_ROOT */
             || BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(port)  /** FMQ_GUARANTEED */
             || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(port))  /** FMQ_BESTEFFORT_AGR */
    {
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_control_fmq_set(unit, port, cosq, type, arg));
    }

    /** Egress transmit scheduler */
    /** Egress scheduler port */
    else if (BCM_GPORT_IS_LOCAL(port))
    {
        /** Egress scheduler port */
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_port_scheduler_set(unit, port, cosq, type, arg));

    }
    /** Egress transmit scheduler */
    /** Egress scheduler port */
    else if (BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        /** Egress scheduler interface */
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_interface_scheduler_set(unit, port, cosq, type, arg));

    }
    else if (BCM_COSQ_GPORT_IS_RCY(port))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_set(unit, BCM_COSQ_GPORT_CORE_GET(port), 0, arg));
    }
    else if (BCM_COSQ_GPORT_IS_RCY_MIRR(port))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_set(unit, BCM_COSQ_GPORT_CORE_GET(port), 1, arg));
    }
    else if (BCM_COSQ_GPORT_IS_PORT_TC(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_port_tc_set(unit, port, cosq, type, arg));
    }
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_port_tcg_set(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "Slow Start Enable", "Slow Start Rate" 
     */
    else if ((BCM_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(port) || BCM_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(port) ||
              BCM_GPORT_IS_FABRIC_MESH_MC(port)) &&
             (type == bcmCosqControlFlowSlowRate ||
              type == bcmCosqControlFlowSlowRate1 || type == bcmCosqControlFlowSlowRate2))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */
        SHR_IF_ERR_EXIT(dnx_cosq_control_scheduler_fabric_set(unit, port, cosq, type, arg));
    }
    /** unsupported gport */
    else if (BCM_GPORT_IS_FABRIC_CLOS(port) || BCM_GPORT_IS_FABRIC_MESH(port))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */

        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Ingress transmit scheduler is not supported on this device\n");
    }
    /*
     * Use cases: "Flow Max Burst", "High Priority Flow", "Remote Flow", "CL Discrete Weight", "Connector Slow Rate Profile",
     */
    else if ((BCM_GPORT_IS_SCHEDULER(port)) ||  /** SCHEDULER */
             (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(port)) || /** VOQ_CONNECTOR */
             (BCM_COSQ_GPORT_IS_SCHED_CIR(port)) || /** CIR */
             (BCM_COSQ_GPORT_IS_SCHED_PIR(port)) || /** PIR */
             (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(port)) || /** Shared shaper element */
             (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(port)))  /** COMPOSITE_SF2 */
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_cosq_control_flow_set(unit, port, cosq, type, arg));
    }
    /*
     * E2E port - non backward compatibile message only
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_cosq_control_e2e_set(unit, port, cosq, type, arg));
    }

    /*
     * Use cases: "E2E_PORT_TC Max Burst"
     */
    else if ((BCM_COSQ_GPORT_IS_E2E_PORT_TC(port)))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_control_e2e_port_tc_set(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "E2E_PORT_TCG Max Burst"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_control_e2e_port_tcg_set(unit, port, cosq, type, arg));
    }
    /*
     * latency profile
     */
    else if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port) || BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_profile_set(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "OCB Only"
     * (Legacy implementation)
     */
    else if (BCM_GPORT_IS_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_profile_set(unit, port, cosq, type, arg));
    }

    else if (BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_mcast_egress_queue_set(unit, port, cosq, type, arg));
    }
    /** not supported gport */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(port) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(port) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(port) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Function is not supported. Please use bcm_cosq_gport_dynamic_sched_set() instead.");
    }
    else if (BCM_GPORT_IS_CONGESTION(port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "HCFC is not supported by device.");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port(0x%08x) not supported \n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for getting configuration of cosq controls
 * For detailed description refer to \ref bcm_dnx_cosq_control_set()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - description per use case.
 * \param [in] cosq - description per use case.
 * \param [in] type - description per use case.
 * \param [out] arg - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_cosq_control_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    SHR_IF_ERR_EXIT(dnx_cosq_control_validate(unit, port, cosq, type, FALSE));
    /*
     * Use cases: "Port scheduler compensation" , ....
     */
    if (port == 0 || BCM_COSQ_GPORT_IS_CORE(port))
    {
        /** device wide settings */
        SHR_IF_ERR_EXIT(dnx_cosq_control_device_get(unit, port, cosq, type, arg));
    }
    else if (!BCM_GPORT_IS_SET(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_get(unit, port, type, arg));
    }

    /** FMQ */
    else if (BCM_COSQ_GPORT_IS_FMQ_ROOT(port) /** FMQ_ROOT */
             || BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(port) /** FMQ_GUARANTEED */
             || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(port)) /** FMQ_BESTEFFORT_AGR */
    {
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_control_fmq_get(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "Slow Start Enable", "Slow Start Rate" 
     */
    else if ((BCM_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(port) || BCM_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(port) ||
              BCM_GPORT_IS_FABRIC_MESH_MC(port)) &&
             (type == bcmCosqControlFlowSlowRate ||
              type == bcmCosqControlFlowSlowRate1 || type == bcmCosqControlFlowSlowRate2))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */
        SHR_IF_ERR_EXIT(dnx_cosq_control_scheduler_fabric_get(unit, port, cosq, type, arg));
    }
    /** unsupported gport */
    else if (BCM_GPORT_IS_FABRIC_CLOS(port) || BCM_GPORT_IS_FABRIC_MESH(port))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */

        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Ingress transmit scheduler is not supported on this device\n");
    }
    /*
     * Use cases: "Flow Max Burst", "High Priority Flow", "Remote Flow", "CL Discrete Weight", "Connector Slow Rate Profile"
     */
    else if ((BCM_GPORT_IS_SCHEDULER(port)) || /** SCHEDULER */
             (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(port)) || /** VOQ_CONNECTOR */
             (BCM_COSQ_GPORT_IS_SCHED_CIR(port)) ||  /** CIR */
             (BCM_COSQ_GPORT_IS_SCHED_PIR(port)) || /** PIR */
             (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(port)) || /** shared shaper element */
             (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(port))) /** COMPOSITE_SF2 */
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_cosq_control_flow_get(unit, port, cosq, type, arg));
    }
    /*
     * E2E port - non backward compatibile message only
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_cosq_control_e2e_get(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "E2E_PORT_TC Max Burst"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_control_e2e_port_tc_get(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "E2E_PORT_TCG Max Burst"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_control_e2e_port_tcg_get(unit, port, cosq, type, arg));
    }
    /** Egress transmit scheduelr */
    /** Egress scheudler port */
    else if (BCM_COSQ_GPORT_IS_RCY(port))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_get(unit, BCM_COSQ_GPORT_CORE_GET(port), 0, (uint32 *) arg));
    }
    else if (BCM_COSQ_GPORT_IS_RCY_MIRR(port))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_get(unit, BCM_COSQ_GPORT_CORE_GET(port), 1, (uint32 *) arg));
    }
    else if (BCM_GPORT_IS_LOCAL(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_port_scheduler_get(unit, port, cosq, type, arg));
    }
    else if (BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        /** Egress scheduler interface */
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_interface_scheduler_get(unit, port, cosq, type, arg));
    }
    else if (BCM_COSQ_GPORT_IS_PORT_TC(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_port_tc_get(unit, port, cosq, type, arg));
    }
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_egress_port_tcg_get(unit, port, cosq, type, arg));
    }
    /*
     * latency profile
     */
    else if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port) || BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_profile_get(unit, port, cosq, type, arg));
    }
    /*
     * Use cases: "OCB Only"
     * (Legacy implementation)
     */
    else if (BCM_GPORT_IS_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_profile_get(unit, port, cosq, type, arg));
    }

    else if (BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_mcast_egress_queue_get(unit, port, cosq, type, arg));
    }
    /**
     * UCAST/MCAST queue handler
     */
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_control_queue_get(unit, port, cosq, type, arg));
    }
    /** not supported gport */
    else if (BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(port) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(port) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(port) ||
             BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Function is not supported. Please use bcm_cosq_gport_dynamic_sched_set() instead.");
    }
    else if (BCM_GPORT_IS_CONGESTION(port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "HCFC is not supported by device.");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port(0x%08x) not supported \n", port);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - this API is used to set multicast configuration.
*          two configurtion options are accessed through this API:
*          1. flags = BCM_COSQ_MULTICAST_UNSCHEDULED and
*             gport = 0 or BCM_CORE_ALL (using BCM_COSQ_GPORT_CORE_SET)
*               this option will map the ingress_pri (system TC)
*               and ingress_dp (system DP) to multicast TC,
*               shared resources eligibility, and DB service pool.
*
*          2. flags = BCM_COSQ_MULTICAST_SCHEDULED and
*             gport = LOCAL / MODPORT
*               this option maps ingress_pri and ingress_dp to
*               multicast TC and multicast DP.
*
* \param [in] unit -  Unit-ID
* \param [in] gport - must be suitable to flags, see brief.
*                     valid values: CORE, LOCAL, MODPORT, or 0.
* \param [in] ingress_pri - priority (system TC)
* \param [in] ingress_dp - color (system DP)
* \param [in] flags - BCM_COSQ_MULTICAST_UNSCHEDULED or BCM_COSQ_MULTICAST_SCHEDULED
* \param [in] config - configuration struct. (see cosq.h)
*
* \return
*   See shr_error_e
*
* \remark
*   The configuration can be set only if traffic is not enabled,
*   or if "allow modifications during traffic" soc property is enabled.
*   if traffic is enabled and the property is disabled - the function will fail.
*/
shr_error_e
bcm_dnx_cosq_gport_egress_multicast_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (((BCM_COSQ_GPORT_IS_CORE(gport) && BCM_COSQ_GPORT_CORE_GET(gport) == BCM_CORE_ALL) || (gport == 0)) &&
        (flags == BCM_COSQ_MULTICAST_UNSCHEDULED))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_core_multicast_config_set(unit, ingress_pri, ingress_dp, config));
    }

    /*
     * map system TC and system DP to MC DP and offset from base queue pair
     */
    else if ((BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_MODPORT(gport)) && (flags & BCM_COSQ_MULTICAST_SCHEDULED))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_multicast_config_set
                        (unit, gport, ingress_pri, ingress_dp, flags, config));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "invalid port parameter");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - this API is used to get multicast configuration.
*          two configurtion options are accessed through this API,
*          for more info see bcm_dnx_cosq_gport_egress_multicast_config_set.
*
* \param [in] unit -  Unit-ID
* \param [in] gport - must be suitable to flags, see brief.
*                     valid values: CORE, LOCAL, MODPORT.
*                     NOTE: on get gport cannot be set to 0.
* \param [in] ingress_pri - priority (system TC)
* \param [in] ingress_dp - color (system DP)
* \param [in] flags - BCM_COSQ_MULTICAST_UNSCHEDULED or BCM_COSQ_MULTICAST_SCHEDULED
* \param [out] config - will be set with the configured values. (see cosq.h)
*
* \return
*   See shr_error_e
*
* \remark
*   * None
*/
shr_error_e
bcm_dnx_cosq_gport_egress_multicast_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (((BCM_COSQ_GPORT_IS_CORE(gport) && BCM_COSQ_GPORT_CORE_GET(gport) == BCM_CORE_ALL) || (gport == 0)) &&
        (flags == BCM_COSQ_MULTICAST_UNSCHEDULED))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_core_multicast_config_get(unit, ingress_pri, ingress_dp, config));
    }
    else if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_MODPORT(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_multicast_config_get
                        (unit, gport, ingress_pri, ingress_dp, flags, config));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "invalid port parameter");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * 
 * \brief Function to validate if TC port priority is within the 
 *        valid range
 */
static int
dnx_cosq_port_priority_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    int nof_priorities;
    int port;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport))
    {
        /** Get logical port */
        port = BCM_COSQ_GPORT_E2E_PORT_TC_GET(gport);
        /** Find port nof prioriteis */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &nof_priorities));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parent_gport 0x%x provided", gport);
    }

    if (cosq >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cosq %d is out of range. Should be in range of 0-%d.", cosq, nof_priorities - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Function to convert a gport to another gport type if 
 * possible. 
 * 
 * @param unit  - unit
 * @param gport_type - requested gport type 
 * @param gport_info - gport information used for handling the 
 *                   request
 * 
 * @return shr_error_e 
 *  
 *  * \see
 *   * bcm_dnx_cosq_gport_handle_core_get 
 */
shr_error_e
bcm_dnx_cosq_gport_handle_get(
    int unit,
    bcm_cosq_gport_type_t gport_type,
    bcm_cosq_gport_info_t * gport_info)
{
    bcm_cosq_gport_info_core_t gport_info_core;
    int core = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_info, _SHR_E_PARAM, "gport_info");

    /** Clear structure */
    sal_memset(&gport_info_core, 0, sizeof(bcm_cosq_gport_info_core_t));

    /** Assign default values for the gport_core_get functions */
    gport_info_core.gport_type = gport_type;
    gport_info_core.in_gport = gport_info->in_gport;
    gport_info_core.flags = 0;
    gport_info_core.cosq = gport_info->cosq;
    core = BCM_CORE_ALL;

    switch (gport_type)
    {
        case bcmCosqGportTypeE2EPort:
        case bcmCosqGportTypeUnicastEgress:
        case bcmCosqGportTypeMulticastEgress:
        case bcmCosqGportTypeCompositeFlow2:
        case bcmCosqGportTypeSchedCIR:
        case bcmCosqGportTypeSchedPIR:
        case bcmCosqGportTypeSchedSharedShaper0:
        case bcmCosqGportTypeSchedSharedShaper1:
        case bcmCosqGportTypeSchedSharedShaper2:
        case bcmCosqGportTypeSchedSharedShaper3:
        case bcmCosqGportTypeSchedSharedShaper4:
        case bcmCosqGportTypeSchedSharedShaper5:
        case bcmCosqGportTypeSchedSharedShaper6:
        case bcmCosqGportTypeSchedSharedShaper7:
        case bcmCosqGportTypeLocalPort:
        case bcmCosqGportTypeLocalPortTC:
        case bcmCosqGportTypeLocalPortTCG:
        case bcmCosqGportTypeE2EPortTC:
        case bcmCosqGportTypeE2EPortTCG:
        case bcmCosqGportTypeSched:
            core = -1;
            break;
        case bcmCosqGportTypeCore:
        case bcmCosqGportTypeGlobalFmqClass1:
        case bcmCosqGportTypeGlobalFmqClass2:
        case bcmCosqGportTypeGlobalFmqClass3:
        case bcmCosqGportTypeGlobalFmqClass4:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Gport type (%d) is not supported with this API. Use bcm_dnx_cosq_gport_handle_core_get instead.\n",
                         gport_type);
            break;
        default:
            core = BCM_CORE_ALL;
            break;
    }

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_core_get(unit, core, &gport_info_core, &gport_info->out_gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Ingress Transmit Scheduler types
 */
static int
dnx_cosq_gport_handle_ingress_scheduler_verify(
    int unit,
    bcm_cosq_gport_type_t gport_type)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (gport_type)
    {
        case bcmCosqGportTypeGlobalFabricMeshCommonLocal0:
        case bcmCosqGportTypeGlobalFabricMeshCommonLocal1:
        case bcmCosqGportTypeGlobalFabricMeshCommonDev1:
        case bcmCosqGportTypeGlobalFabricMeshCommonDev2:
        case bcmCosqGportTypeGlobalFabricMeshCommonDev3:
        case bcmCosqGportTypeGlobalFabricMeshCommonMc:
        case bcmCosqGportTypeGlobalFabricClosCommonLocal0:
        case bcmCosqGportTypeGlobalFabricClosCommonLocal1:
        case bcmCosqGportTypeGlobalFabricClosCommonFabric:
        case bcmCosqGportTypeGlobalFabricClosCommonUnicastFabric:
        case bcmCosqGportTypeGlobalFabricClosCommonMulticastFabric:
        case bcmCosqGportTypeGlobalFabricMeshLocal0OcbHigh:
        case bcmCosqGportTypeGlobalFabricMeshLocal0OcbLow:
        case bcmCosqGportTypeGlobalFabricMeshLocal0MixHigh:
        case bcmCosqGportTypeGlobalFabricMeshLocal0MixLow:
        case bcmCosqGportTypeGlobalFabricMeshLocal1OcbHigh:
        case bcmCosqGportTypeGlobalFabricMeshLocal1OcbLow:
        case bcmCosqGportTypeGlobalFabricMeshLocal1MixHigh:
        case bcmCosqGportTypeGlobalFabricMeshLocal1MixLow:
        case bcmCosqGportTypeGlobalFabricMeshDev1OcbHigh:
        case bcmCosqGportTypeGlobalFabricMeshDev1OcbLow:
        case bcmCosqGportTypeGlobalFabricMeshDev1MixHigh:
        case bcmCosqGportTypeGlobalFabricMeshDev1MixLow:
        case bcmCosqGportTypeGlobalFabricMeshDev2OcbHigh:
        case bcmCosqGportTypeGlobalFabricMeshDev2OcbLow:
        case bcmCosqGportTypeGlobalFabricMeshDev2MixHigh:
        case bcmCosqGportTypeGlobalFabricMeshDev2MixLow:
        case bcmCosqGportTypeGlobalFabricMeshDev3OcbHigh:
        case bcmCosqGportTypeGlobalFabricMeshDev3OcbLow:
        case bcmCosqGportTypeGlobalFabricMeshDev3MixHigh:
        case bcmCosqGportTypeGlobalFabricMeshDev3MixLow:
        case bcmCosqGportTypeGlobalFabricMeshMcOcbHigh:
        case bcmCosqGportTypeGlobalFabricMeshMcOcbLow:
        case bcmCosqGportTypeGlobalFabricMeshMcMixHigh:
        case bcmCosqGportTypeGlobalFabricMeshMcMixLow:
        case bcmCosqGportTypeGlobalFabricMeshLocal0:
        case bcmCosqGportTypeGlobalFabricMeshLocal1:
        case bcmCosqGportTypeGlobalFabricMeshDev1:
        case bcmCosqGportTypeGlobalFabricMeshDev2:
        case bcmCosqGportTypeGlobalFabricMeshDev3:
        case bcmCosqGportTypeGlobalFabricMesh:
        case bcmCosqGportTypeGlobalFabricClosLocal0OcbHigh:
        case bcmCosqGportTypeGlobalFabricClosLocal0OcbLow:
        case bcmCosqGportTypeGlobalFabricClosLocal0MixHigh:
        case bcmCosqGportTypeGlobalFabricClosLocal0MixLow:
        case bcmCosqGportTypeGlobalFabricClosLocal1OcbHigh:
        case bcmCosqGportTypeGlobalFabricClosLocal1OcbLow:
        case bcmCosqGportTypeGlobalFabricClosLocal1MixHigh:
        case bcmCosqGportTypeGlobalFabricClosLocal1MixLow:
        case bcmCosqGportTypeGlobalFabricClosUnicastFabricOcbHigh:
        case bcmCosqGportTypeGlobalFabricClosUnicastFabricOcbLow:
        case bcmCosqGportTypeGlobalFabricClosUnicastFabricMixHigh:
        case bcmCosqGportTypeGlobalFabricClosUnicastFabricMixLow:
        case bcmCosqGportTypeGlobalFabricClosFmqGuaranteedOcb:
        case bcmCosqGportTypeGlobalFabricClosFmqBestEffortOcb:
        case bcmCosqGportTypeGlobalFabricClosFmqGuaranteedMix:
        case bcmCosqGportTypeGlobalFabricClosFmqBestEffortMix:
        case bcmCosqGportTypeGlobalFabricClosLocal0:
        case bcmCosqGportTypeGlobalFabricClosLocal1:
        case bcmCosqGportTypeGlobalFabricClosFabric:
        case bcmCosqGportTypeGlobalFabricClosLocal0High:
        case bcmCosqGportTypeGlobalFabricClosLocal0Low:
        case bcmCosqGportTypeGlobalFabricClosLocal1High:
        case bcmCosqGportTypeGlobalFabricClosLocal1Low:
        case bcmCosqGportTypeGlobalFabricClosUnicastFabricHigh:
        case bcmCosqGportTypeGlobalFabricClosUnicastFabricLow:
        case bcmCosqGportTypeGlobalFabricClosOcbHigh:
        case bcmCosqGportTypeGlobalFabricClosMixHigh:
        case bcmCosqGportTypeGlobalFabricClosOcbLow:
        case bcmCosqGportTypeGlobalFabricClosMixLow:
        case bcmCosqGportTypeGlobalFabricClos:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Ingress transmit scheduler is not supported on this device\n");
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Functin to validate the core input for the 
 *         bcm_dnx_cosq_gport_handle_core_get API
 */
static int
dnx_cosq_gport_handle_core_verify(
    int unit,
    int core,
    bcm_cosq_gport_info_core_t * gport_info,
    bcm_gport_t * out_gport)
{
    int gport_core = BCM_CORE_ALL;
    int flow_id;
    int nof_priorities;
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_info, _SHR_E_PARAM, "gport_info");
    SHR_NULL_CHECK(out_gport, _SHR_E_PARAM, "out_gport");

    /*
     * check if Ingress transmit scheduler type is specified
     * must be called before switching on the types
     */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_handle_ingress_scheduler_verify(unit, gport_info->gport_type));

    switch (gport_info->gport_type)
    {
        case bcmCosqGportTypeE2EPort:
        case bcmCosqGportTypeUnicastEgress:
        case bcmCosqGportTypeMulticastEgress:
        case bcmCosqGportTypeLocalPort:
        case bcmCosqGportTypeLocalPortTC:
        case bcmCosqGportTypeLocalPortTCG:
        case bcmCosqGportTypeE2EPortTC:
        case bcmCosqGportTypeE2EPortTCG:
            /** Check function input core parameter for expected value   */
            SHR_VAL_VERIFY(core, -1, _SHR_E_PARAM, "This gport type supports only -1 for core parameter");
        case bcmCosqGportTypeSched:
            /** Check function input core parameter for expected value   */
            SHR_VAL_VERIFY(core, -1, _SHR_E_PARAM, "This gport type supports only -1 for core parameter");

            if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport_info->in_gport))
            {
                /** validate priority */
                SHR_IF_ERR_EXIT(dnx_cosq_port_priority_validate(unit, gport_info->in_gport, gport_info->cosq));
            }
            else if (BCM_COSQ_GPORT_IS_PORT_TC(gport_info->in_gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Port TC (0x%x) cannot be used to create SCHED gport.\n E2E_PORT_TC gport schould be provided",
                             gport_info->in_gport);
            }
            break;
        case bcmCosqGportTypeSchedCIR:
        case bcmCosqGportTypeSchedPIR:
            /** Check function input core parameter for expected value   */
            SHR_VAL_VERIFY(core, -1, _SHR_E_PARAM, "This gport type supports only -1 for core parameter");

            if (!BCM_GPORT_IS_SCHEDULER(gport_info->in_gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) of incorrect type, Expect Scheduler gport\n",
                             gport_info->in_gport);
            }
            /** Validate core provided with the gport */
            gport_core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            DNXCMN_CORE_VALIDATE(unit, gport_core, FALSE);

            /** validate the flow has appropirate shared shaper mode */
            flow_id = BCM_GPORT_SCHEDULER_GET(gport_info->in_gport);
            SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, gport_core, flow_id, &shared_shaper_mode));

            if (shared_shaper_mode != DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "CIR/PIR gport can be created for dual shaper only (flow_id %d)\n", flow_id);
            }
            break;

        case bcmCosqGportTypeSchedSharedShaper0:
        case bcmCosqGportTypeSchedSharedShaper1:
        case bcmCosqGportTypeSchedSharedShaper2:
        case bcmCosqGportTypeSchedSharedShaper3:
        case bcmCosqGportTypeSchedSharedShaper4:
        case bcmCosqGportTypeSchedSharedShaper5:
        case bcmCosqGportTypeSchedSharedShaper6:
        case bcmCosqGportTypeSchedSharedShaper7:
            /** Check function input core parameter for expected value   */
            SHR_VAL_VERIFY(core, -1, _SHR_E_PARAM, "This gport type supports only -1 for core parameter");

            if (!BCM_GPORT_IS_SCHEDULER(gport_info->in_gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) of incorrect type, Expect Scheduler gport\n",
                             gport_info->in_gport);
            }
            /** Validate core provided with the gport */
            gport_core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            DNXCMN_CORE_VALIDATE(unit, gport_core, FALSE);

            /** validate the flow has appropirate shared shaper mode */
            flow_id = BCM_GPORT_SCHEDULER_GET(gport_info->in_gport);
            SHR_IF_ERR_EXIT(dnx_scheduler_element_shared_shaper_nof_priorities_get(unit, gport_core, flow_id,
                                                                                   &nof_priorities));

            if ((gport_info->gport_type - bcmCosqGportTypeSchedSharedShaper0) >= nof_priorities)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Index %d cannot be created for shared shaper of %d priorities (flow id %d)\n",
                             (gport_info->gport_type - bcmCosqGportTypeSchedSharedShaper0), nof_priorities, flow_id);
            }
            break;

        case bcmCosqGportTypeCompositeFlow2:
            /** Check function input core parameter for expected value   */
            SHR_VAL_VERIFY(core, -1, _SHR_E_PARAM, "This gport type supports only -1 for core parameter");
            if (BCM_GPORT_IS_SCHEDULER(gport_info->in_gport))
            {
                gport_core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            }
            else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport_info->in_gport))
            {
                gport_core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport_info->in_gport);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) of incorrect type, Expect Scheduler/VoQ Connector gport\n",
                             gport_info->in_gport);
            }
            /** Validate core provided with the gport */
            DNXCMN_CORE_VALIDATE(unit, gport_core, TRUE);
            break;

        case bcmCosqGportTypeGlobalFmqRoot:
        case bcmCosqGportTypeFabricRxLocalMulticast:
        case bcmCosqGportTypeFabricRxLocalUnicast:
        case bcmCosqGportTypeGlobalFmqGuaranteed:
        case bcmCosqGportTypeGlobalFmqBestEffortAggregate:
        case bcmCosqGportTypeGlobalFmqBestEffort0:
        case bcmCosqGportTypeGlobalFmqBestEffort1:
        case bcmCosqGportTypeGlobalFmqBestEffort2:
        case bcmCosqGportTypeGlobalFabricClosFmqGuaranteed:
        case bcmCosqGportTypeGlobalFabricClosFmqBestEffort:
        case bcmCosqGportTypeGlobalFabricMeshMc:
        case bcmCosqGportTypeFabricPipe:
        case bcmCosqGportTypeFabricPipeEgress:
        case bcmCosqGportTypeFabricPipeIngress:
            SHR_VAL_VERIFY(core, BCM_CORE_ALL, _SHR_E_PARAM, "This gport type supports only BCM_CORE_ALL");
            break;
        case bcmCosqGportTypeCore:
            DNXCMN_CORE_VALIDATE(unit, core, TRUE);
            break;
        case bcmCosqGportTypeGlobalFmqClass1:
        case bcmCosqGportTypeGlobalFmqClass2:
        case bcmCosqGportTypeGlobalFmqClass3:
        case bcmCosqGportTypeGlobalFmqClass4:
            /** BCM_CORE_ALL is not supported for FMQ Class */
            DNXCMN_CORE_VALIDATE(unit, core, FALSE);
            break;
        case bcmCosqGportTypeGlobalIsqRoot:
            SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ root gport is not supported on this device (unit %d)!", unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Handle type (%d) requested\n", gport_info->gport_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for creating gport of specific types.
 * ****************************************
 *  Control types:
 * * Control type: "Logical port based gports" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeLocalPort,
 *      bcmCosqGportTypeLocalPortTC,
 *      bcmCosqGportTypeLocalPortTCG
 *      bcmCosqGportTypeE2EPort, 
 *      bcmCosqGportTypeE2EPortTC,
 *      bcmCosqGportTypeE2EPortTCG
 *      bcmCosqGportTypeUnicastEgress, 
 *      bcmCosqGportTypeMulticastEgress
 *  
 * - gport_info->in_gport - Local port or any gport encoding 
 *   logical port.
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be -1 
 *  
 * ****************************************
 * * Control type: "Composite Flow" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeCompositeFlow2
 *  
 * - gport_info->in_gport - 
 *      BCM_GPORT_IS_SCHEDULER(gpor)
 *      BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be -1 
 * * ****************************************
 * * Control type: "Scheduler" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeSched
 *  
 * - gport_info->in_gport - SCHED gport, VOQ_CONNECTOR gport, 
 *   FMQ_CLASS gport, any gport encoding logical port except
 *   PORT_TC gports.
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be -1 
 *  
 * ****************************************
 * * Control type: "Scheduler CIR/PIR" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeSchedCIR,
 *      bcmCosqGportTypeSchedPIR
 *  
 * - gport_info->in_gport - 
 *      BCM_GPORT_IS_SCHEDULER(gport)
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be -1 
 *  
 * ****************************************
 * * Control type: "Shared shaper element"
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeSchedSharedShaper0
 *      bcmCosqGportTypeSchedSharedShaper1
 *      bcmCosqGportTypeSchedSharedShaper2
 *      bcmCosqGportTypeSchedSharedShaper3
 *      bcmCosqGportTypeSchedSharedShaper4
 *      bcmCosqGportTypeSchedSharedShaper5
 *      bcmCosqGportTypeSchedSharedShaper6
 *      bcmCosqGportTypeSchedSharedShaper7
 *  
 * - gport_info->in_gport - 
 *      BCM_GPORT_IS_SCHEDULER(gport)
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be -1 
 *  
 * ****************************************
 * * Control type: "FMQ credit generator" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeGlobalFmqRoot,
 *      bcmCosqGportTypeGlobalFmqGuaranteed
 *      bcmCosqGportTypeGlobalFmqBestEffortAggregate,
 *      bcmCosqGportTypeGlobalFmqBestEffort0,
 *      bcmCosqGportTypeGlobalFmqBestEffort1,
 *      bcmCosqGportTypeGlobalFmqBestEffort2
 * - gport_info->in_gport - unused.
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be BCM_CORE_ALL
 *  
 * ****************************************
 * * Control type: "FMQ Class" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeGlobalFmqClass1,
 *      bcmCosqGportTypeGlobalFmqClass2,
 *      bcmCosqGportTypeGlobalFmqClass3,
 *      bcmCosqGportTypeGlobalFmqClass4
 * - gport_info->in_gport - unused. must be -1 
 * - gport_info->cosq -     unused.
 * - core -                 core_id (BCM_CORE_ALL is not 
 *   supported)
 *  
 * ****************************************
 * * Control type: "FMQ Clos Best Effort and Guaranteed" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeGlobalFabricClosFmqGuaranteed,
 *      bcmCosqGportTypeGlobalFabricClosFmqBestEffort
 * - gport_info->in_gport - unused. must be -1 
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be BCM_CORE_ALL 
 *  
 * ****************************************
 * * Control type: "Fabric Mesh MC" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeGlobalFabricMeshMc
 * - gport_info->in_gport - unused. must be -1 
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be BCM_CORE_ALL 
 *  
 * ****************************************
 * * Control type: "Fabric Pipe" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeFabricPipe, 
 *      bcmCosqGportTypeFabricPipeEgress,
 *      bcmCosqGportTypeFabricPipeIngress
 * - gport_info->in_gport - unused. must be -1
 * - gport_info->cosq -     Pipe index.
 * - core -                 unused. must be BCM_CORE_ALL
 *  
 * ****************************************
 * * Control type: "Fabric RX local" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeFabricRxLocalUnicast,
 *      bcmCosqGportTypeFabricRxLocalMulticast
 * - gport_info->in_gport - unused. must be -1 
 * - gport_info->cosq -     unused.
 * - core -                 unused. must be -1 
 *  
 * ****************************************
 * * Control type: "Core" 
 * Parameters: 
 * - gport_info->gport_type - 
 *      bcmCosqGportTypeCore,
 *    
 * - gport_info->in_gport - unused. must be -1 
 * - gport_info->cosq -     unused. must be -1
 * - core -                 core_id
 *  
 * \param [in] unit -
 *   Unit-ID
 * \param [in] core -
 *   Core-ID of the required gport
 * \param [in] gport_info -
 *   Information about the required gport.
 * \param [out] out_gport -
 *   gport of the required type which contains the required information.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_handle_core_get(
    int unit,
    int core,
    bcm_cosq_gport_info_core_t * gport_info,
    bcm_gport_t * out_gport)
{

    int flow_id = 0, fmq_id = 1, base_hr, se_id;
    bcm_gport_t parent_gport = BCM_GPORT_INVALID;
    bcm_port_t local_port = 0;
    bcm_cos_t priority = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_handle_core_verify(unit, core, gport_info, out_gport));

    switch (gport_info->gport_type)
    {
            /*
             * Logical port based gports
             */
        case bcmCosqGportTypeLocalPort:
        case bcmCosqGportTypeLocalPortTC:
        case bcmCosqGportTypeLocalPortTCG:
        case bcmCosqGportTypeE2EPort:
        case bcmCosqGportTypeE2EPortTC:
        case bcmCosqGportTypeE2EPortTCG:
        case bcmCosqGportTypeUnicastEgress:
        case bcmCosqGportTypeMulticastEgress:
        {
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport_info->in_gport, &local_port));

            switch (gport_info->gport_type)
            {
                case bcmCosqGportTypeLocalPort:
                    BCM_GPORT_LOCAL_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeLocalPortTC:
                    BCM_COSQ_GPORT_PORT_TC_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeLocalPortTCG:
                    BCM_COSQ_GPORT_PORT_TCG_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeE2EPort:
                    BCM_COSQ_GPORT_E2E_PORT_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeE2EPortTC:
                    BCM_COSQ_GPORT_E2E_PORT_TC_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeE2EPortTCG:
                    BCM_COSQ_GPORT_E2E_PORT_TCG_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeUnicastEgress:
                    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(*out_gport, local_port);
                    break;
                case bcmCosqGportTypeMulticastEgress:
                    BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(*out_gport, local_port);
                    break;
                    /*
                     * must have default. Otherwise, compilation error 
                     */
                    /*
                     * coverity[dead_error_begin : FALSE] 
                     */
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected gport type(0x%x)\n", gport_info->in_gport);
                    break;
            }
            break;
        }
            /*
             * Composite flow 
             */
        case bcmCosqGportTypeCompositeFlow2:
        {
            if (BCM_GPORT_IS_SCHEDULER(gport_info->in_gport))
            {
                core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            }
            else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport_info->in_gport))
            {
                core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport_info->in_gport);
            }
            BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(*out_gport, gport_info->in_gport, core);
            break;
        }
            /*
             * Scheduler
             */
        case bcmCosqGportTypeSched:
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get
                            (unit, gport_info->in_gport, FALSE, &parent_gport));

            if (BCM_GPORT_IS_SCHEDULER(parent_gport))
            {
                /** If scheduler element parent is also a scheduler there is
                 *  nothing to do */
                *out_gport = parent_gport;
            }
            else
            {
                /** API is releavant for port TC or other Port physical gports */
                /** Get relevant for the scheduler local port */
                SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, parent_gport, &local_port));
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, local_port, &core));
                if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(parent_gport))
                {
                    priority = gport_info->cosq;
                }
                else
                {
                    priority = 0;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, local_port, &base_hr));
                se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);
                SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));
                BCM_GPORT_SCHEDULER_CORE_SET(*out_gport, flow_id, core);
            }
            break;
        }
            /*
             * Scheduler CIR/PIR
             */
        case bcmCosqGportTypeSchedCIR:
        {
            core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            BCM_COSQ_GPORT_SCHED_CIR_CORE_SET(*out_gport, gport_info->in_gport, core);
            break;
        }
        case bcmCosqGportTypeSchedPIR:
        {
            core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            BCM_COSQ_GPORT_SCHED_PIR_CORE_SET(*out_gport, gport_info->in_gport, core);
            break;
        }
            /*
             * Shared shaper element
             */
        case bcmCosqGportTypeSchedSharedShaper0:
        case bcmCosqGportTypeSchedSharedShaper1:
        case bcmCosqGportTypeSchedSharedShaper2:
        case bcmCosqGportTypeSchedSharedShaper3:
        case bcmCosqGportTypeSchedSharedShaper4:
        case bcmCosqGportTypeSchedSharedShaper5:
        case bcmCosqGportTypeSchedSharedShaper6:
        case bcmCosqGportTypeSchedSharedShaper7:
        {
            core = BCM_GPORT_SCHEDULER_CORE_GET(gport_info->in_gport);
            flow_id = BCM_GPORT_SCHEDULER_GET(gport_info->in_gport);
            flow_id += gport_info->gport_type - bcmCosqGportTypeSchedSharedShaper0;
            BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_SET(*out_gport, flow_id, core);
            break;
        }

            /*
             * FMQ hierarchy and FMQ credit generator
             */
        case bcmCosqGportTypeGlobalFmqRoot:
        {
            BCM_COSQ_GPORT_FMQ_ROOT_SET(*out_gport);
            break;
        }
        case bcmCosqGportTypeGlobalFmqGuaranteed:
        {
            BCM_COSQ_GPORT_FMQ_GUARANTEED_SET(*out_gport);
            break;
        }
            /*
             * FMQ Best Effort
             */
        case bcmCosqGportTypeGlobalFmqBestEffortAggregate:
        {
            BCM_COSQ_GPORT_FMQ_BESTEFFORT_AGR_SET(*out_gport);
            break;
        }
        case bcmCosqGportTypeGlobalFmqBestEffort0:
        {
            BCM_COSQ_GPORT_FMQ_BESTEFFORT0_SET(*out_gport);
            break;
        }
        case bcmCosqGportTypeGlobalFmqBestEffort1:
        {
            BCM_COSQ_GPORT_FMQ_BESTEFFORT1_SET(*out_gport);
            break;
        }
        case bcmCosqGportTypeGlobalFmqBestEffort2:
        {
            BCM_COSQ_GPORT_FMQ_BESTEFFORT2_SET(*out_gport);
            break;
        }
            /*
             * FMQ Class 
             */
        case bcmCosqGportTypeGlobalFmqClass1:
        case bcmCosqGportTypeGlobalFmqClass2:
        case bcmCosqGportTypeGlobalFmqClass3:
        case bcmCosqGportTypeGlobalFmqClass4:
        {
            fmq_id = gport_info->gport_type - bcmCosqGportTypeGlobalFmqClass1 + 1;
            /** The FMQ Ids are 1' based */
            BCM_COSQ_GPORT_FMQ_CLASS_CORE_SET(*out_gport, core, fmq_id);
            break;
        }
            /*
             * FMQ Clos Best Effort and Guaranteed
             */
        case bcmCosqGportTypeGlobalFabricClosFmqGuaranteed:
        {
            BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_SET(*out_gport);
            break;
        }
        case bcmCosqGportTypeGlobalFabricClosFmqBestEffort:
        {
            BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT_SET(*out_gport);
            break;
        }
            /*
             * Fabric Mesh MC
             */
        case bcmCosqGportTypeGlobalFabricMeshMc:
        {
            BCM_COSQ_GPORT_FABRIC_MESH_MC_SET(*out_gport);
            break;
        }
            /*
             * Fabric Pipe
             */
        case bcmCosqGportTypeFabricPipe:
        {
            BCM_COSQ_GPORT_FABRIC_PIPE_SET(*out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        }
        case bcmCosqGportTypeFabricPipeEgress:
        {
            BCM_COSQ_GPORT_FABRIC_PIPE_EGRESS_SET(*out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        }
        case bcmCosqGportTypeFabricPipeIngress:
        {
            BCM_COSQ_GPORT_FABRIC_PIPE_INGRESS_SET(*out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        }
            /*
             * Fabric RX Local
             */
        case bcmCosqGportTypeFabricRxLocalUnicast:
        {
            BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_SET(*out_gport, core);
            break;
        }
        case bcmCosqGportTypeFabricRxLocalMulticast:
        {
            BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_SET(*out_gport, core);
            break;
        }
            /*
             * Core gport
             */
        case bcmCosqGportTypeCore:
        {
            BCM_COSQ_GPORT_CORE_SET(*out_gport, core);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported gport type");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     verification for API bcm_cosq_discard_set
* \param [in] unit -unit id
* \param [in] flags -flags
* \return
*    shr_error_e
* \remark
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_discard_set_verify(
    int unit,
    int flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** FIME JIRA SDK-135033 - define instead number*/
    if (flags & ~(BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_ALL | BCM_COSQ_DISCARD_EGRESS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag. Supported flags are: BCM_COSQ_DISCARD_ENABLE, "
                     "BCM_COSQ_DISCARD_COLOR_BLACK, BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_YELLOW, "
                     "BCM_COSQ_DISCARD_COLOR_ALL, BCM_COSQ_DISCARD_EGRESS");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Configure global drop per DP - ingress or egress
* \param [in] unit        - unit id
* \param [in] flags - supported flags. include:
*           BCM_COSQ_DISCARD_EGRESS
*           BCM_COSQ_DISCARD_ENABLE
*           BCM_COSQ_DISCARD_COLOR_ALL
*           BCM_COSQ_DISCARD_COLOR_GREEN
*           BCM_COSQ_DISCARD_COLOR_YELLOW
*           BCM_COSQ_DISCARD_COLOR_RED
*           BCM_COSQ_DISCARD_COLOR_BLACK.
*           see specific supported flags in internal functions documentation
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
int
bcm_dnx_cosq_discard_set(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_discard_set_verify(unit, flags));

    if (flags & BCM_COSQ_DISCARD_EGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_stat_discard_egress_meter_set(unit, flags));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_set(unit, flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*    Get configuration of global drop per DP - ingress or egress
* \param [in] unit        - unit id
* \param [out] flags - most flags are out, except BCM_COSQ_DISCARD_EGRESS (in)
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
int
bcm_dnx_cosq_discard_get(
    int unit,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    if ((*flags) & BCM_COSQ_DISCARD_EGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_stat_discard_egress_meter_get(unit, flags));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_discard_get(unit, flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - setting egress resources thresholds according to type.
*   The following thresholds can be configured only if traffic is not enabled,
*   or if "allow modifications during traffic" soc property is enabled:
*       bcmCosqThresholdAvailableDataBuffers (DB reserved)
*       bcmCosqThresholdAvailablePacketDescriptors (PD reserved)
*   if traffic is enabled and the property is disabled - the function will fail.
*
*/
static shr_error_e
dnx_cosq_egress_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (threshold->type)
    {
        case (bcmCosqThresholdAvailableDataBuffers):
        case (bcmCosqThresholdAvailablePacketDescriptors):
        case (bcmCosqThresholdPacketDescriptors):
        case (bcmCosqThresholdPacketDescriptorsMin):
        case (bcmCosqThresholdPacketDescriptorsMax):
        case (bcmCosqThresholdPacketDescriptorsAlpha):
        case (bcmCosqThresholdDataBuffers):
        case (bcmCosqThresholdDataBuffersMin):
        case (bcmCosqThresholdDataBuffersMax):
        case (bcmCosqThresholdDataBuffersAlpha):
        case (bcmCosqThresholdDataBuffersAlpha1):
            /*
             * Egress CGM thresholds
             */
            SHR_IF_ERR_EXIT(dnx_ecgm_gport_threshold_set(unit, gport, cosq, threshold));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d, Unsupported egress threshold\n", threshold->type);
    }

exit:
    SHR_FUNC_EXIT;
}

/* get egress threshold */
static shr_error_e
dnx_cosq_egress_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (threshold->type)
    {
        case (bcmCosqThresholdPacketDescriptors):
        case (bcmCosqThresholdPacketDescriptorsMin):
        case (bcmCosqThresholdPacketDescriptorsMax):
        case (bcmCosqThresholdPacketDescriptorsAlpha):
        case (bcmCosqThresholdDataBuffers):
        case (bcmCosqThresholdDataBuffersMin):
        case (bcmCosqThresholdDataBuffersMax):
        case (bcmCosqThresholdDataBuffersAlpha):
        case (bcmCosqThresholdDataBuffersAlpha1):
        case (bcmCosqThresholdAvailableDataBuffers):
        case (bcmCosqThresholdAvailablePacketDescriptors):
            /*
             * Egress CGM thresholds
             */
            SHR_IF_ERR_EXIT(dnx_ecgm_gport_threshold_get(unit, gport, cosq, threshold));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Type %d, Unsupported egress threshold\n", threshold->type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_gport_threshold_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    uint32 supported_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /*
     * Verify one of the following flags is set.
     * (there can be more flags supported in inner functions)
     */
    supported_flags =
        (BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_ETH_PORT_PFC | BCM_COSQ_THRESHOLD_EGRESS |
         BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED);
    if ((threshold->flags & supported_flags) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag combination (0x%x) not supported", threshold->flags);
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_RCI)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Function is not supported. Please read User Manual and use bcm_fabric_cgm_control_set() instead.");
    }
    else if ((threshold->flags & (BCM_COSQ_THRESHOLD_PER_DP | BCM_COSQ_THRESHOLD_DROP))
             && !(threshold->flags & BCM_COSQ_THRESHOLD_EGRESS))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Function is not supported. Please read User Manual and use bcm_fabric_cgm_control_set() instead.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function for configuring Ingress / Egress Thresholds for Flow control and Admission Control
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport - GPORT ID for a queue group (unicast/multicast egress queue group)
 *   \param [in] cosq - CoS queue
 *   \param [in] threshold - structure that folds information for the global recourse and the threshold we want to set.
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_threshold_verify(unit, gport, cosq, threshold));

    if ((threshold->flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC) || (threshold->flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC))
    {
        /** NIF Almost full thresholds */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_eth_port_threshold_set_get(unit, gport, threshold, TRUE));
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_EGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egress_gport_threshold_set(unit, gport, cosq, threshold));
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_threshold_set(unit, gport, cosq, threshold));
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED)
    {
        SHR_IF_ERR_EXIT(dnx_system_red_free_res_threshold_set(unit, gport, cosq, threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function for retrieving Ingress / Egress Thresholds for Flow control and Admission Control
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport - GPORT ID for a queue group (unicast/multicast egress queue group)
 *   \param [in] cosq - CoS queue
 *   \param [out] threshold - structure that folds information for the global recourse and the threshold we want to set.
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_threshold_verify(unit, gport, cosq, threshold));

    if ((threshold->flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC) || (threshold->flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC))
    {
        /** NIF Almost full thresholds */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_eth_port_threshold_set_get(unit, gport, threshold, FALSE));
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_EGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egress_gport_threshold_get(unit, gport, cosq, threshold));
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_gport_threshold_get(unit, gport, cosq, threshold));
    }
    else if (threshold->flags & BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED)
    {
        SHR_IF_ERR_EXIT(dnx_system_red_free_res_threshold_get(unit, gport, cosq, threshold));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuring bandwidth of different objects
 *
 * Use cases:
 *  ** Use case: "FMQ root"
 *  Configure maximal bandwidth of FMQ root
 *  Parameters:
 *  - gport - FMQ_ROOT, FMQ_GUARANTEED or FMQ_BESTEFFORT_AGR
 *  - cosq - Not relevant (set to 0)
 *
 *  ** Use case: "E2E scheduler"
 *  Configure maximal bandwidth of scheduler flow
 *  Parameters:
 *  - gport - SCHEDULER, VOQ_CONNECTOR or COMPOSITE_SF2 gport
 *  - cosq - for voq connector - offset from base voq connector, for scheduler - 0
 *  
 *  ** Use case: "Ingress shapers"
 *  Configure maximal bandwidth of ingress schedulers
 *  Parameters:
 *  - gport - INGRESS_RECEIVE_CPU_GUARANTEED,
 *    INGRESS_RECEIVE_RCY_GUARANTEED or INGRESS_RECEIVE_NIF
 *    gport
 *  - cosq - not used. Must be set to 0
 *  
 *  ** Use case: "E2E port/interface"
 *  Configure maximal bandwidth of scheduler port/interface
 *  Parameters:
 *  - gport - E2E_PORT
 *  - cosq - Not relevant (set to 0)
 * 
 *  ** Use case: "E2E port TC"
 *  Configure maximal bandwidth of scheduler port TC
 *  Parameters:
 *  - gport - E2E_PORT_TC
 *  - cosq - priority (TC)
 *
 *  ** Use case: "E2E port TCG"
 *  Configure maximal bandwidth of scheduler port TCG
 *  Parameters:
 *  - gport - E2E_PORT_TCG
 *  - cosq - TCG id
 *
 *  ** Use case: "Recycle"
 *  Configure maximal bandwidth of recycle
 *  Parameters:
 *  - gport - recycle gport.
 *  - cosq - Not relevant (set to 0)
 *
 *  ** Use case: "Mirror"
 *  Configure maximal bandwidth of mirror
 *  Parameters:
 *  - gport - mirror gport.
 *  - cosq - Not relevant (set to 0)
 *
 *  ** Use case: "EGQ port/interface"
 *  Configure maximal bandwidth of EGQ port/interface
 *  Parameters:
 *  - gport - local port
 *  - cosq - Not relevant (set to 0)
 * 
 *  ** Use case: "EGQ port TC"
 *  Configure maximal bandwidth of EGQ port TC
 *  Parameters:
 *  - gport - PORT_TC
 *  - cosq - priority (TC)
 *
 *  ** Use case: "EGQ port TCG"
 *  Configure maximal bandwidth of EGQ port TCG
 *  Parameters:
 *  - gport - PORT_TCG
 *  - cosq - TCG id
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - description per use case.
 * \param [in] cosq - description per use case.
 * \param [in] kbits_sec_min - guaranteed bandwidth in kbits/sec (if applicable)
 * \param [in] kbits_sec_max - maximal bandwidth in kbits/sec 
 * \param [in] flags - flags
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    bcm_gport_t parent_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Implementation per use case (gport type) */

    /** determine if parent port refers to a pre-allocated resource */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get(unit, gport, 1, &parent_port));

    /*
     * Use case: "FMQ root"
     * (Legacy implementation)
     */
    if (BCM_COSQ_GPORT_IS_FMQ_ROOT(parent_port)
        || BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(parent_port) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(parent_port))
    {
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_gport_fmq_bandwidth_set(unit, parent_port, cosq,
                                                              kbits_sec_min, kbits_sec_max, flags));
    }
    else if (BCM_COSQ_GPORT_IS_ISQ_ROOT(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ gport is not supported on this device\n");
    }
    /** Ingress shapers */
    else if (BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED(gport) ||
             BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_bandwith_set(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    }
    /** Ingress scheduler */
    else if (BCM_GPORT_IS_FABRIC_CLOS(gport) || BCM_GPORT_IS_FABRIC_MESH(gport))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress transmit scheduler is not supported on this device\n");
    }
    /*
     * Use case: "E2E scheduler"
     */
    else if ((BCM_GPORT_IS_SCHEDULER(parent_port)) ||
             (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(parent_port)) ||
             (BCM_COSQ_GPORT_IS_SCHED_CIR(parent_port)) || (BCM_COSQ_GPORT_IS_SCHED_PIR(parent_port)) ||
             (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(parent_port)) || (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(parent_port)))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_flow_bandwidth_set(unit, parent_port, cosq,
                                                               kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "E2E port/interface"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(parent_port))
    {

        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_bandwidth_set(unit, parent_port, cosq,
                                                                   kbits_sec_min, kbits_sec_max, flags));

    }
    else if (BCM_COSQ_GPORT_IS_E2E_INTERFACE(parent_port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_interface_bandwidth_set(unit, parent_port, cosq,
                                                                        kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "E2E port TC"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(parent_port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_bandwidth_set(unit, parent_port, cosq,
                                                                      kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "E2E port TCG"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(parent_port))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_bandwidth_set(unit, parent_port, cosq,
                                                                       kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "Recycle"
     */
    else if (BCM_COSQ_GPORT_IS_RCY(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_bandwidth_set(unit, BCM_COSQ_GPORT_CORE_GET(gport), 0, kbits_sec_max));
    }
    /*
     * Use case: "Mirror"
     */
    else if (BCM_COSQ_GPORT_IS_RCY_MIRR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_bandwidth_set(unit, BCM_COSQ_GPORT_CORE_GET(gport), 1, kbits_sec_max));
    }
    /*
     * Use case: "EGQ port/interface"
     * (Legacy implementation)
     */
    else if (BCM_GPORT_IS_LOCAL(parent_port) || BCM_GPORT_IS_MODPORT(parent_port))
    {
        /** Egress scheduler port */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_scheduler_bandwidth_set(unit, parent_port, cosq,
                                                                           kbits_sec_min, kbits_sec_max, flags));

    }
    else if (BCM_GPORT_IS_LOCAL_INTERFACE(parent_port))
    {
        /** Egress scheduler interface */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_interface_scheduler_bandwidth_set(unit, parent_port, cosq,
                                                                                kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "EGQ port TC"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TC(parent_port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tc_bandwidth_set(unit, parent_port, cosq,
                                                                    kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "EGQ port TCG"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(parent_port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tcg_bandwidth_set(unit, parent_port, cosq,
                                                                     kbits_sec_min, kbits_sec_max, flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported gport (0x%08x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for getting bandwidth of different objects
 * For detailed description refer to \ref bcm_dnx_cosq_gport_bandwidth_set()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - description per use case.
 * \param [in] cosq - description per use case.
 * \param [in] kbits_sec_min - guaranteed bandwidth in kbits/sec (if applicable)
 * \param [in] kbits_sec_max - maximal bandwidth in kbits/sec 
 * \param [in] flags - flags

 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    bcm_gport_t res_gport;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if parent port refers to a pre-allocated resource */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get(unit, gport, 1, &res_gport));

    /*
     * Use case: "EGQ port/interface"
     * (Legacy implementation)
     */
    if (BCM_GPORT_IS_LOCAL(res_gport) || BCM_GPORT_IS_MODPORT(res_gport))
    {

        /** Egress scheduler port */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_scheduler_bandwidth_get(unit, gport, cosq,
                                                                           kbits_sec_min, kbits_sec_max, flags));

    }
    else if (BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        /** Egress scheduler interface */
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_interface_scheduler_bandwidth_get(unit, gport, cosq,
                                                                                kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "Recycle"
     */
    else if (BCM_COSQ_GPORT_IS_RCY(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_bandwidth_get(unit, BCM_COSQ_GPORT_CORE_GET(gport), 0, kbits_sec_max));
    }
    /*
     * Use case: "Mirror"
     */
    else if (BCM_COSQ_GPORT_IS_RCY_MIRR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_egr_recycle_bandwidth_get(unit, BCM_COSQ_GPORT_CORE_GET(gport), 1, kbits_sec_max));
    }
    /*
     * Use case: "EGQ port TC"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TC(res_gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tc_bandwidth_get(unit, res_gport, cosq,
                                                                    kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "EGQ port TCG"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(res_gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_gport_egress_port_tcg_bandwidth_get(unit, res_gport, cosq,
                                                                     kbits_sec_min, kbits_sec_max, flags));
    }

    /*
     * Use case: "FMQ root"
     * (Legacy implementation)
     */
    else if (BCM_COSQ_GPORT_IS_FMQ_ROOT(gport)
             || BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport))
    {
        SHR_IF_ERR_EXIT(_bcm_dnx_cosq_gport_fmq_bandwidth_get(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    }
    else if (BCM_COSQ_GPORT_IS_ISQ_ROOT(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ gport is not supported on this device\n");
    }

    /** Ingress shapers */
    else if (BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED(gport) ||
             BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_bandwith_get(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    }
    /** Ingress Scheduler */
    else if (BCM_GPORT_IS_FABRIC_CLOS(gport) || BCM_GPORT_IS_FABRIC_MESH(gport))
    {
        /*
         * Note, 
         * these gports are scheduler gports, 
         * so the check must be before checking IS_SCHEDULER
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress transmit scheduler is not supported on this device\n");
    }
    /*
     * Use case: "E2E scheduler"
     */
    else if ((BCM_GPORT_IS_SCHEDULER(res_gport)) ||
             (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(res_gport)) ||
             (BCM_COSQ_GPORT_IS_SCHED_CIR(res_gport)) ||
             (BCM_COSQ_GPORT_IS_SCHED_PIR(res_gport)) ||
             (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(res_gport)) || (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(res_gport)))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_flow_bandwidth_get(unit, res_gport, cosq,
                                                               kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "E2E port/interface"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(res_gport))
    {

        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_bandwidth_get(unit, res_gport, cosq,
                                                                   kbits_sec_min, kbits_sec_max, flags));

    }
    else if (BCM_COSQ_GPORT_IS_E2E_INTERFACE(res_gport))
    {

        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_interface_bandwidth_get(unit, res_gport, cosq,
                                                                        kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "E2E port TC"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(res_gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_bandwidth_get(unit, res_gport, cosq,
                                                                      kbits_sec_min, kbits_sec_max, flags));
    }
    /*
     * Use case: "E2E port TCG"
     */
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(res_gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_bandwidth_get(unit, res_gport, cosq,
                                                                       kbits_sec_min, kbits_sec_max, flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported gport (0x%08x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify parameters of gport_connection_set
 */
static int
bcm_dnx_cosq_gport_connection_set_verify(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    int is_ingress = FALSE, is_egress = FALSE;
    int my_modid;
    int ingress_core = 0;
    uint32 valid_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_connect, _SHR_E_PARAM, "gport_connect");

    valid_flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_EGRESS |
        BCM_COSQ_GPORT_CONNECTION_INVALID;

    if (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_MODID_OVERRIDE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_COSQ_GPORT_CONNECTION_MODID_OVERRIDE flag is not supported on this device\n");

    }

    SHR_MASK_VERIFY(gport_connect->flags, valid_flags, _SHR_E_PARAM, "provided unsupported flags\n");

    is_ingress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INGRESS) ? TRUE : FALSE;
    is_egress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_EGRESS) ? TRUE : FALSE;

    if ((is_ingress == FALSE) && (is_egress == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags parameter(0x%x) need to specify ingresss/egress\n", gport_connect->flags);
    }
    if ((is_ingress == TRUE) && (is_egress == TRUE))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_stk_my_modid_get(unit, &my_modid));

        if (gport_connect->remote_modid != my_modid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "modid specified (%d) needs to be the local modid when ingress & egress are specified\n",
                         gport_connect->remote_modid);
        }
    }

    if (BCM_COSQ_GPORT_IS_ISQ(gport_connect->voq))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ is not supported in this device\n");
    }
    if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_connect->voq) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport_connect->voq)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ucast/mcast gport 0x%x\n", gport_connect->voq);
    }

    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, 0, gport_connect->voq, &ingress_core));
    DNXCMN_CORE_VALIDATE(unit, ingress_core, TRUE);

    if (!BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport_connect->voq_connector))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid connector gport 0x%x\n", gport_connect->voq_connector);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Connect ingress VOQ  to egress VOQ connector.
 * Should be called twice - once on ingress side and once on egress side
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport_connect - VOQ to VOQ connector parameters

 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_connection_set(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    int is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(bcm_dnx_cosq_gport_connection_set_verify(unit, gport_connect));

    is_ingress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INGRESS) ? TRUE : FALSE;
    is_egress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_EGRESS) ? TRUE : FALSE;

    if (is_ingress == TRUE)
    {
        /** is_ingress */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_connection_set(unit, gport_connect));
    }

    if (is_egress == TRUE)
    {
        /** is_egress */
        SHR_IF_ERR_EXIT(dnx_scheduler_connection_set(unit, gport_connect));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   verify parameters of gport_connection_get
 */
static int
bcm_dnx_cosq_gport_connection_get_verify(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    uint32 valid_flags;
    int is_ingress;
    int core_id, base_cid;
    uint8 sw_state_num_cos;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_connect, _SHR_E_PARAM, "gport_connect");

    valid_flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_EGRESS;

    SHR_MASK_VERIFY(gport_connect->flags, valid_flags, _SHR_E_PARAM, "provided unsupported flags\n");

    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(gport_connect->flags, valid_flags, 1, 1, _SHR_E_PARAM,
                                     "only one flag should be set");

    is_ingress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INGRESS) ? TRUE : FALSE;

    if (is_ingress)
    {
        if (BCM_COSQ_GPORT_IS_ISQ(gport_connect->voq))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ gport is not supported on this device");
        }
        if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_connect->voq) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport_connect->voq)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ucast/mcast gport 0x%x", gport_connect->voq);
        }

        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, 0, gport_connect->voq, &core_id));
        DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    }
    else
    {
        /** egress */
        if (!BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport_connect->voq_connector))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid voq connector 0x%x\n", gport_connect->voq_connector);
        }
        core_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport_connect->voq_connector);
        base_cid = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect->voq_connector);

        DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

        SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.get(unit, core_id, base_cid, &sw_state_num_cos));
        if (sw_state_num_cos <= 0)
        {
            int num_cos = sw_state_num_cos;
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid voq connector(%d) - num_cos = %d", base_cid, num_cos);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get connection inforamation between ingress VOQ  to egress VOQ connector.
 * Can be called on ingress side to obtain VOQ connector connected to specified VOQ
 * or on egress side to obtain VOQ connected to specified VOQ connector
 *
 * \param [in] unit -  Unit-ID
 * \param [in, out] gport_connect - VOQ to VOQ connector connection parameters

 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_connection_get(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    int is_ingress;

    /*
     * Initialize, Just to have some value. Overwritten below.
     */
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(bcm_dnx_cosq_gport_connection_get_verify(unit, gport_connect));

    is_ingress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INGRESS) ? TRUE : FALSE;

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_gport_connection_get(unit, gport_connect));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connection_get(unit, gport_connect));
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * ---------------------------------------------------------------------------------------------------
 * -- Scheduler APIs
 *
 * ---------------------------------------------------------------------------------------------------
 */

static int
dnx_cosq_gport_add_verify(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport)
{
    int resource_type_flags;
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_COSQ_GPORT_ISQ)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ is not supported in this device");
    }

    /** basic consistency checks */
    resource_type_flags = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_VOQ_CONNECTOR |
                           BCM_COSQ_GPORT_MCAST_QUEUE_GROUP | BCM_COSQ_GPORT_UCAST_QUEUE_GROUP);

    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags, resource_type_flags, 1, 1,
                                     _SHR_E_PARAM, "Exactly one resource type must be specified\n");

    if (flags & BCM_COSQ_GPORT_VOQ_CONNECTOR)
    {
        if (!(flags & BCM_COSQ_GPORT_WITH_ID) && dnx_data_device.general.nof_cores_get(unit) > 1)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Can't determine core ID\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate new gport
 * following gport types are supported
 * * unicast queue
 * * multicast queue
 * * VOQ connector
 * * Scheduling element
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] port - gport. usually target port gport 
 *   \param [in] numq - number of elements in bundle
 *   \param [in] flags - flags
 *   \param [out] gport - allocated gport
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark - this is deprecated function and supported for backward compatibility only
 * new code should use
 * bcm_cosq_voq_connector_gport_add - for VOQ connectors
 * bcm_cosq_scheduler_gport_add - for scheduling elements
 * bcm_cosq_ingress_queue_bundle_gport_add - for unicast and multicast queues
 *   * 
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_add(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport)
{
    bcm_cosq_ingress_queue_bundle_gport_config_t config;
    int cosq;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_add_verify(unit, port, numq, flags, gport));

    /** check for scheduler resource allocation */
    if (flags & BCM_COSQ_GPORT_SCHEDULER)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_sched_gport_add(unit, port, numq, flags, gport));
    }

    /** check for connector resource allocation */
    else if (flags & BCM_COSQ_GPORT_VOQ_CONNECTOR)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_gport_add(unit, -1, -1, 1, port, numq, flags, gport));
    }
    /** check for multicast resource allocation */
    else if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)
    {
        config.flags = flags;
        config.port = port;
        config.local_core_id = BCM_CORE_ALL;
        config.numq = numq;
        for (cosq = 0; cosq < DNX_COSQ_NOF_TC; cosq++)
        {
            /** setting all of the rate classes zero*/
            config.queue_atrributes[cosq].delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
            /** setting all of the credit request profiles to zero*/
            config.queue_atrributes[cosq].rate_class = 0;
        }
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_mcast_gport_add(unit, &config, gport));
    }
    /** unicast resources */
    else if (flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP)
    {
        config.flags = flags;
        config.port = port;
        config.local_core_id = BCM_CORE_ALL;
        config.numq = numq;
        for (cosq = 0; cosq < DNX_COSQ_NOF_TC; cosq++)
        {
            /** setting all of the rate classes zero*/
            config.queue_atrributes[cosq].delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
            /** setting all of the credit request profiles to default 10 slow enabled mode*/
            config.queue_atrributes[cosq].rate_class = 0;
        }
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_ucast_gport_add(unit, &config, gport));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "no supported resource specified 0x%x\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete existing gport
 * following gport types are supported
 * * unicast queue
 * * multicast queue
 * * VOQ connector
 * * Scheduling element
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [out] gport -  gport to be deleted
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * 
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_ISQ(gport))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ISQ is not supported on this device\n");
    }

    /** consistency checks */
    if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_delete(unit, gport));
    }
    else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_voq_connector_gport_delete(unit, gport));
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_mcast_gport_delete(unit, gport));
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_ucast_gport_delete(unit, gport));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "delete is not supported for gport(0x%x)", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_dnx_cosq_gport_get parameters
 */
static int
dnx_cosq_gport_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    SHR_NULL_CHECK(num_cos_levels, _SHR_E_PARAM, "num_cos_levels");

    /*
     * Not all calls use physical_port,
     * but since that may change, better to 
     * expect it as a valid parameter
     */
    SHR_NULL_CHECK(physical_port, _SHR_E_PARAM, "physical_port");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get gport parameters
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport -  gport to retrieve the information for
 *   \param [out] physical_port - destination gport (if applicable)
 *   \param [out] num_cos_levels - number of objects in a bundle
 *   \param [out] flags - bmc_gport_add flags used to create this gport
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * 
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags)
{

    bcm_cosq_gport_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_get_verify(unit, gport, physical_port, num_cos_levels, flags));

    (*flags) = 0;
    (*num_cos_levels) = 0;
    (*physical_port) = 0;

    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_gport_get(unit, gport, physical_port, num_cos_levels, flags));
    }
    else if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        SHR_IF_ERR_EXIT(dnx_sched_gport_get(unit, gport, physical_port, num_cos_levels, flags));
    }
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(gport))
    {
        gport_info.in_gport = gport;
        gport_info.cosq = 0;

        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));

        SHR_IF_ERR_EXIT(dnx_sched_gport_get(unit, gport_info.out_gport, physical_port, num_cos_levels, flags));
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_ucast_gport_get(unit, gport, physical_port, num_cos_levels, flags));
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_mcast_gport_get(unit, gport, physical_port, num_cos_levels, flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "invalid gport 0x%x\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_profile_mapping_set_verify(
    int unit,
    bcm_gport_t gport_to_map,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_switch_profile_mapping_t * profile_mapping)
{
    uint32 is_vsq_allowed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile_mapping, _SHR_E_PARAM, "profile_mapping");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flags (=%d)", flags);
    }
    if (profile_mapping->profile_type == bcmCosqIngressQueueToRateClass)
    {
        is_vsq_allowed = FALSE;
        SHR_INVOKE_VERIFY_DNX(dnx_cosq_rate_class_gport_verify
                              (unit, profile_mapping->mapped_profile, 0, is_vsq_allowed));
    }
    if ((profile_mapping->profile_type == bcmCosqIngressQueuesToLatencyProfile) &&
        (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(profile_mapping->mapped_profile) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport (=%d) is not supported for profile_type=IngressQueuesToLatencyProfile",
                     profile_mapping->mapped_profile);
    }
    if ((profile_mapping->profile_type == bcmCosqPortToEndToEndLatencyProfile) &&
        (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(profile_mapping->mapped_profile) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport (=%d) is not supported for profile_type=bcmCosqPortToEndToEndLatencyProfile",
                     profile_mapping->mapped_profile);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_gport_to_voq_get_verify(
    int unit,
    bcm_core_t core,
    int base_queue,
    bcm_cos_queue_t cosq,
    int force_core_all)
{
    uint8 sw_state_num_cos;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.num_cos.get(unit, DNX_COSQ_QUEUE_QUAD_GET(base_queue), &sw_state_num_cos));
    if (sw_state_num_cos <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "voq(%d) invalid cos %d", base_queue, sw_state_num_cos);
    }
    if ((cosq < 0) || (cosq >= sw_state_num_cos))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cos levels specified %d", cosq);
    }
    if ((force_core_all == TRUE) && (core != BCM_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "core=%d. allowed only BCM_CORE_ALL", cosq);
    }
    if ((base_queue + cosq) >= dnx_data_ipq.queues.nof_queues_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "input queue (=%d) bigger than max queue (=%d).", (base_queue + cosq),
                     (dnx_data_ipq.queues.nof_queues_get(unit) - 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the queue id and core from gport type queue
 * \param [in] unit - unit
 * \param [in] gport_queue -  gport to retrieve the information for
 * \param [in] cosq - offset of the queue group
 * \param [in] force_core_all - indicate if to verify that the core is BCM_CORE_ALL
 * \param [out] core - core the queue belong
 * \param [out] queue_id - queue id
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e
dnx_cosq_gport_to_voq_get(
    int unit,
    bcm_gport_t gport_queue,
    bcm_cos_queue_t cosq,
    int force_core_all,
    bcm_core_t * core,
    int *queue_id)
{
    int base_queue;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_queue))
    {
        base_queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_queue);
        *core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport_queue);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport_queue))
    {
        base_queue = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport_queue);
        *core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport_queue);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The type of gport %d is not queue.", gport_queue);
    }

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_to_voq_get_verify(unit, *core, base_queue, cosq, force_core_all));
    *queue_id = base_queue + cosq;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - gport profile mapping API. general API that can get any gport and map it to some profile, depend on the type.
 * \param [in] unit - unit
 * \param [in] gport_to_map -  the gport to map to the profile
 * \param [in] cosq - cosq
 * \param [in] flags - NONE
 * \param [in] profile_mapping - {type and profile gport}
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
int
bcm_dnx_cosq_profile_mapping_set(
    int unit,
    bcm_gport_t gport_to_map,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_switch_profile_mapping_t * profile_mapping)
{
    int profile_id, qid;
    int core = BCM_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_profile_mapping_set_verify(unit, gport_to_map, cosq, flags, profile_mapping));

    switch (profile_mapping->profile_type)
    {
        case bcmCosqIngressQueueToRateClass:
            profile_id = BCM_GPORT_PROFILE_GET(profile_mapping->mapped_profile);
            SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, gport_to_map, cosq, FALSE, &core, &qid));
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_to_rate_class_profile_map_set(unit, qid, profile_id));
            break;
        case bcmCosqIngressQueuesToLatencyProfile:
            profile_id = BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(profile_mapping->mapped_profile);
            SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, gport_to_map, cosq, TRUE, &core, &qid));
            SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_map_set(unit, flags, qid, profile_id));
            break;
        case bcmCosqPortToEndToEndLatencyProfile:
            profile_id = BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(profile_mapping->mapped_profile);
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_port_map_set(unit, flags, gport_to_map, profile_id));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile type specified %d\n", profile_mapping->profile_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - gport profile mapping get API. general API that can get any gport and map return the profile it is map to, 
 *          depend on the type.
 * \param [in] unit - unit
 * \param [in] gport_to_map -  the gport to map to the profile
 * \param [in] cosq - cosq
 * \param [in] flags - NONE
 * \param [out] profile_mapping - in out structure. {type is in parameter,  profile gport is out parameter}
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
int
bcm_dnx_cosq_profile_mapping_get(
    int unit,
    bcm_gport_t gport_to_map,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_switch_profile_mapping_t * profile_mapping)
{
    int qid, profile_id;
    int core = BCM_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile_mapping, _SHR_E_PARAM, "profile_mapping");
    switch (profile_mapping->profile_type)
    {
        case bcmCosqIngressQueueToRateClass:
            SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, gport_to_map, cosq, FALSE, &core, &qid));
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_to_rate_class_profile_map_get(unit, qid, &profile_id));
            BCM_GPORT_PROFILE_SET(profile_mapping->mapped_profile, profile_id);
            break;
        case bcmCosqIngressQueuesToLatencyProfile:
            SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get(unit, gport_to_map, cosq, TRUE, &core, &qid));
            SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_map_get(unit, flags, qid, &profile_id));
            BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(profile_mapping->mapped_profile, profile_id);
            break;
        case bcmCosqPortToEndToEndLatencyProfile:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_port_map_get(unit, flags, gport_to_map, &profile_id));
            BCM_GPORT_END_TO_END_LATENCY_PROFILE_SET(profile_mapping->mapped_profile, profile_id);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid profile type specified %d", unit,
                         profile_mapping->profile_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for bcm_dnx_cosq_gport_fadt_threshold_set/get
 */
static shr_error_e
dnx_cosq_gport_fadt_threshold_verify(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{
    int is_vsq_allowed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fadt_info, _SHR_E_PARAM, "fadt_info");
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /*
     * No flags currently supported
     */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "Flags not supported");

    /*
     * Validate Gport is rate class profile
     */
    is_vsq_allowed = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_rate_class_gport_verify(unit, fadt_info->gport, fadt_info->cosq, is_vsq_allowed));

    /*
     * Verify threshold type
     */
    switch (fadt_info->thresh_type)
    {
        case bcmCosqFadtDramBound:
        case bcmCosqFadtDramBoundRecoveryFailure:
        case bcmCosqFadtCongestionNotification:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fadt threshold type %d", fadt_info->thresh_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
bcm_dnx_cosq_gport_fadt_threshold_set(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_fadt_threshold_verify(unit, flags, fadt_info, threshold));

    switch (fadt_info->thresh_type)
    {
        case bcmCosqFadtDramBound:
        case bcmCosqFadtDramBoundRecoveryFailure:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_bound_fadt_threshold_set(unit, fadt_info, threshold));
            break;
        case bcmCosqFadtCongestionNotification:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_set
                            (unit, flags, fadt_info, threshold));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
bcm_dnx_cosq_gport_fadt_threshold_get(
    int unit,
    uint32 flags,
    bcm_cosq_fadt_info_t * fadt_info,
    bcm_cosq_fadt_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_fadt_threshold_verify(unit, flags, fadt_info, threshold));

    switch (fadt_info->thresh_type)
    {
        case bcmCosqFadtDramBound:
        case bcmCosqFadtDramBoundRecoveryFailure:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_bound_fadt_threshold_get(unit, fadt_info, threshold));
            break;
        case bcmCosqFadtCongestionNotification:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_congestion_notification_fadt_threshold_get
                            (unit, flags, fadt_info, threshold));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - Verify functions for the API bcm_dnx_cosq_icgm_resource_stat_get
 */
static shr_error_e
dnx_cosq_icgm_resource_stat_get_verify(
    int unit,
    bcm_cosq_icgm_resource_stat_key_t * stat_key,
    uint64 *value)
{
    int core_id = -1;
    uint32 nof_cores;
    bcm_gport_t core_gport = stat_key->gport;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_key, _SHR_E_PARAM, "stat_key");
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    /*
     * Verify Gport
     */
    if (BCM_COSQ_GPORT_IS_CORE(core_gport))
    {
        core_id = BCM_COSQ_GPORT_CORE_GET(core_gport);
        if ((core_id < 0) || (core_id >= nof_cores))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid core id %d", core_id);
        }
    }
    else if (nof_cores == 1)
    {
        /** if we didn't get core Gport, its possible device only has one core and then core_gport should be 0. */
        if (core_gport != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid core %d.", core_gport);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport 0x%x.", core_gport);
    }

    /*
     * Verify flags
     */
    SHR_VAL_VERIFY(stat_key->flags, 0, _SHR_E_PARAM, "Invalid flags given");

    /*
     * Verify stat type
     */
    switch (stat_key->stat_type)
    {
        case bcmCosqICgmMinFreeBDB:
        case bcmCosqICgmMinFreeSramBuffers:
        case bcmCosqICgmMinFreeSramPDB:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown resource statistic type %d.", stat_key->stat_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
bcm_dnx_cosq_icgm_resource_stat_get(
    int unit,
    bcm_cosq_icgm_resource_stat_key_t * stat_key,
    uint64 *value)
{
    int core_id = -1;
    bcm_gport_t core_gport = 0;
    dnx_ingress_congestion_statistics_info_t stats;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_icgm_resource_stat_get_verify(unit, stat_key, value));

    core_gport = stat_key->gport;

    if (BCM_COSQ_GPORT_IS_CORE(core_gport))
    {
        core_id = BCM_COSQ_GPORT_CORE_GET(core_gport);
    }
    else
    {
        /*
         * device only has one core 
         */
        core_id = 0;
    }

    /** Get all occupancy statistics for the core */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_free_resource_counter_get(unit, core_id, &stats));

    switch (stat_key->stat_type)
    {
        case bcmCosqICgmMinFreeBDB:
            *value = stats.min_free_count[DNX_INGRESS_CONGESTION_STATISTICS_BDBS];
            break;
        case bcmCosqICgmMinFreeSramBuffers:
            *value = stats.min_free_count[DNX_INGRESS_CONGESTION_STATISTICS_SRAM_BUFFERS];
            break;
        case bcmCosqICgmMinFreeSramPDB:
            *value = stats.min_free_count[DNX_INGRESS_CONGESTION_STATISTICS_SRAM_PDS];
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_gport_rate_class_create_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_rate_class_create_info_t * create_info)
{
    int rate_class;
    uint32 supported_attrs;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_info, _SHR_E_PARAM, "create_info");
    /*
     * Verify Gport is rate class
     */
    if (!BCM_GPORT_IS_PROFILE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport 0x%x.", gport);
    }

    rate_class = BCM_GPORT_PROFILE_GET(gport);
    SHR_RANGE_VERIFY(rate_class, 0, dnx_data_ingr_congestion.voq.nof_rate_class_get(unit) - 1, _SHR_E_PARAM,
                     "rate class %d is out of range\n", rate_class);

    /*
     * Verify flags, currently no flags supported
     */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "Invalid flags given");

    /*
     * Verify rate
     */
    if (create_info->rate <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Rate expected to positive - got %d.\n", create_info->rate);
    }

    /*
     * Verify Rate Class Attributes
     */
    supported_attrs =
        BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED | BCM_COSQ_RATE_CLASS_CREATE_ATTR_OCB_ONLY |
        BCM_COSQ_RATE_CLASS_CREATE_ATTR_MULTICAST;
    SHR_MASK_VERIFY(create_info->attributes, supported_attrs, _SHR_E_PARAM, "Invalid attribute given");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
bcm_dnx_cosq_gport_rate_class_create(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_rate_class_create_info_t * create_info)
{
    int rate_class;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_rate_class_create_verify(unit, gport, flags, create_info));

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    /*
     * Mark this rate class as created in SW state
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.is_created.set(unit, rate_class, TRUE));

    /*
     * set default values for the rate class in HW
     */
    SHR_IF_ERR_EXIT(dnx_tune_ingress_congestion_default_voq_rate_class_set(unit, rate_class, create_info));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pkt_size_adjust_delta_map_set(
    int unit,
    int delta,
    int final_delta)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pkt_size_adjust_delta_map_get(
    int unit,
    int delta,
    int *final_delta)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_get() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}
