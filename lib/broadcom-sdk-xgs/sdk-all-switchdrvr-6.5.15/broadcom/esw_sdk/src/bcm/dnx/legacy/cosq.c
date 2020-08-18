/*
 * $Id: cosq.c,v 1.433 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Soc_dnx-B COSQ
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <bcm/types.h>
#include <bcm/cosq.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dnxc/legacy/error.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <soc/dnx/legacy/mbcm.h>


#ifdef JR2_CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif
/* 
 * local defines
 */
#define DNX_COSQ_FMQ_MAX_BURST_LIMIT            (63)
#define DNX_COSQ_FMQ_MAX_BE_WEIGHT              (31)
#define DNX_COSQ_FMQ_NOF_BE                     (SOC_DNX_MULT_FABRIC_NOF_BE_CLASSES)

#define DNX_SAND_TRUE  1
#define DNX_SAND_FALSE 0

/* FC Hyst threshold set */
#define DNX_COSQ_THRESHOLD_FC_HYST_SET(unit,hyst,cosq,flags,value) \
{ \
    if (flags & BCM_COSQ_THRESHOLD_SET) { \
        if (cosq == BCM_COSQ_HIGH_PRIORITY) { \
            hyst.hp.set = value; \
        } else if (cosq == BCM_COSQ_LOW_PRIORITY) { \
            hyst.lp.set = value; \
        } else { \
            LOG_ERROR(BSL_LS_BCM_COSQ, \
                      (BSL_META_U(unit, \
                                  "unit %d, invalid cosq parameter %d "), unit, cosq)); \
            return _SHR_E_PARAM; \
        } \
    } else if (flags & BCM_COSQ_THRESHOLD_CLEAR) { \
        if (cosq == BCM_COSQ_HIGH_PRIORITY) { \
            hyst.hp.clear = value; \
        } else if (cosq == BCM_COSQ_LOW_PRIORITY) { \
            hyst.lp.clear = value; \
        } else { \
            LOG_ERROR(BSL_LS_BCM_COSQ, \
                      (BSL_META_U(unit, \
                                  "unit %d, invalid cosq parameter %d "), unit, cosq)); \
            return _SHR_E_PARAM; \
        } \
    } else { \
        LOG_ERROR(BSL_LS_BCM_COSQ, \
                  (BSL_META_U(unit, \
                              "unit %d, invalid flags 0x%x "), unit, flags)); \
        return _SHR_E_PARAM; \
    } \
}
/* FC Hyst threshold get */
#define DNX_COSQ_THRESHOLD_FC_HYST_GET(unit,value,cosq,flags,hyst) \
{ \
    if (flags & BCM_COSQ_THRESHOLD_SET) { \
        if (cosq == BCM_COSQ_HIGH_PRIORITY) { \
            value = hyst.hp.set; \
        } else if (cosq == BCM_COSQ_LOW_PRIORITY) { \
            value = hyst.lp.set; \
        } else { \
            LOG_ERROR(BSL_LS_BCM_COSQ, \
                      (BSL_META_U(unit, \
                                  "unit %d, invalid cosq parameter %d "), unit, cosq)); \
            return _SHR_E_PARAM; \
        } \
    } else if (flags & BCM_COSQ_THRESHOLD_CLEAR) { \
        if (cosq == BCM_COSQ_HIGH_PRIORITY) { \
            value = hyst.hp.clear; \
        } else if (cosq == BCM_COSQ_LOW_PRIORITY) { \
            value = hyst.lp.clear; \
        } else { \
            LOG_ERROR(BSL_LS_BCM_COSQ, \
                      (BSL_META_U(unit, \
                                  "unit %d, invalid cosq parameter %d "), unit, cosq)); \
            return _SHR_E_PARAM; \
        } \
    } else { \
        LOG_ERROR(BSL_LS_BCM_COSQ, \
                  (BSL_META_U(unit, \
                              "unit %d, invalid flags 0x%x "), unit, flags)); \
        return _SHR_E_PARAM; \
    } \
}

int
_bcm_dnx_cosq_vsq_gl_fc_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold)
{
    shr_error_e rc = _SHR_E_NONE;
    SOC_DNX_ITM_GLOB_RCS_FC_TH fc_info, exact_fc_info;
    uint32 relevant_flags, threshold_type_flags;
    int flag_count;

    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
    SOC_DNX_ITM_GLOB_RCS_FC_TH_clear(&fc_info);
    SOC_DNX_ITM_GLOB_RCS_FC_TH_clear(&exact_fc_info);

    /*
     * Validate flags 
     */
    relevant_flags = flags & (BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_MULTICAST);
    if (flags != relevant_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid flags parameter 0x%x", unit, flags);
    }

    /*
     * basic consistency checks 
     */
    threshold_type_flags = (flags & (BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_MULTICAST));

    flag_count = dnx_cosq_bit_count_get(threshold_type_flags);
    if (flag_count > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid flags 0x%x - only one flag can be set", unit,
                     threshold_type_flags);
    }

    /*
     * Validate params 
     */
    if (cosq != BCM_COSQ_HIGH_PRIORITY && cosq != BCM_COSQ_LOW_PRIORITY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "unit %d, Invalid cosq parameter %d, can be either BCM_COSQ_HIGH_PRIORITY, BCM_COSQ_LOW_PRIORITY",
                     unit, cosq);
    }

    /*
     * retrieve information 
     */
    rc = (MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_itm_glob_rcs_fc_get, (unit, &fc_info)));
    SHR_IF_ERR_EXIT(rc);

    if ((flags & BCM_COSQ_THRESHOLD_UNICAST))
    {
        /*
         * Set unicast dbuffs thresholds 
         */
        DNX_COSQ_THRESHOLD_FC_HYST_SET(unit, fc_info.unicast, cosq, BCM_COSQ_THRESHOLD_SET,
                                       pfc_threshold->xoff_threshold);
        DNX_COSQ_THRESHOLD_FC_HYST_SET(unit, fc_info.unicast, cosq, BCM_COSQ_THRESHOLD_CLEAR,
                                       pfc_threshold->xon_threshold);
    }

    if ((flags & BCM_COSQ_THRESHOLD_MULTICAST))
    {
        /*
         * Set full multicast dbuffs thresholds 
         */
        DNX_COSQ_THRESHOLD_FC_HYST_SET(unit, fc_info.full_mc, cosq, BCM_COSQ_THRESHOLD_SET,
                                       pfc_threshold->xoff_threshold);
        DNX_COSQ_THRESHOLD_FC_HYST_SET(unit, fc_info.full_mc, cosq, BCM_COSQ_THRESHOLD_CLEAR,
                                       pfc_threshold->xon_threshold);
    }

    /*
     * Set bdbs thresholds 
     */
    DNX_COSQ_THRESHOLD_FC_HYST_SET(unit, fc_info.bdbs, cosq, BCM_COSQ_THRESHOLD_SET, pfc_threshold->xoff_threshold_bd);
    DNX_COSQ_THRESHOLD_FC_HYST_SET(unit, fc_info.bdbs, cosq, BCM_COSQ_THRESHOLD_CLEAR, pfc_threshold->xon_threshold_bd);

    /*
     * Set information 
     */
    rc = (MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_itm_glob_rcs_fc_set, (unit, &fc_info, &exact_fc_info)));
    SHR_IF_ERR_EXIT(rc);

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

int
_bcm_dnx_cosq_vsq_gl_fc_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold)
{
    shr_error_e rc = _SHR_E_NONE;
    SOC_DNX_ITM_GLOB_RCS_FC_TH fc_info;
    uint32 relevant_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
    SHR_NULL_CHECK(pfc_threshold, _SHR_E_PARAM, "pfc_threshold");

    /*
     * Validate flags 
     */
    relevant_flags = flags & (BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_MULTICAST);

    if (flags != relevant_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid flags parameter 0x%x", unit, flags);
    }

    /*
     * Validate params 
     */
    if (cosq != BCM_COSQ_HIGH_PRIORITY && cosq != BCM_COSQ_LOW_PRIORITY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "unit %d, Invalid cosq parameter %d, can be either BCM_COSQ_HIGH_PRIORITY, BCM_COSQ_LOW_PRIORITY",
                     unit, cosq);
    }

    SOC_DNX_ITM_GLOB_RCS_FC_TH_clear(&fc_info);

    /*
     * retrieve information 
     */
    rc = (MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_itm_glob_rcs_fc_get, (unit, &fc_info)));
    SHR_IF_ERR_EXIT(rc);

    if ((flags & BCM_COSQ_THRESHOLD_UNICAST))
    {
        /*
         * Get unicast dbuffs thresholds 
         */
        DNX_COSQ_THRESHOLD_FC_HYST_GET(unit, pfc_threshold->xoff_threshold, cosq, BCM_COSQ_THRESHOLD_SET,
                                       fc_info.unicast);
        DNX_COSQ_THRESHOLD_FC_HYST_GET(unit, pfc_threshold->xon_threshold, cosq, BCM_COSQ_THRESHOLD_CLEAR,
                                       fc_info.unicast);
    }

    if ((flags & BCM_COSQ_THRESHOLD_MULTICAST))
    {
        /*
         * Get full multicast dbuffs thresholds 
         */
        DNX_COSQ_THRESHOLD_FC_HYST_GET(unit, pfc_threshold->xoff_threshold, cosq, BCM_COSQ_THRESHOLD_SET,
                                       fc_info.full_mc);
        DNX_COSQ_THRESHOLD_FC_HYST_GET(unit, pfc_threshold->xon_threshold, cosq, BCM_COSQ_THRESHOLD_CLEAR,
                                       fc_info.full_mc);
    }

    /*
     * Get bdbs thresholds 
     */
    DNX_COSQ_THRESHOLD_FC_HYST_GET(unit, pfc_threshold->xoff_threshold_bd, cosq, BCM_COSQ_THRESHOLD_SET, fc_info.bdbs);
    DNX_COSQ_THRESHOLD_FC_HYST_GET(unit, pfc_threshold->xon_threshold_bd, cosq, BCM_COSQ_THRESHOLD_CLEAR, fc_info.bdbs);

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose : Set weight, sp between FMQ class
 */
int
_bcm_dnx_cosq_gport_fmq_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    shr_error_e rc = _SHR_E_NONE;
    int index, core;
    dnx_mult_fabric_info_t info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate 
     */
    if (mode == -1 && (weight < 0 || weight > DNX_COSQ_FMQ_MAX_BE_WEIGHT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight given %d", weight);
    }
    /*
     * clear 
     */
    SOC_DNX_MULT_FABRIC_INFO_clear((SOC_DNX_MULT_FABRIC_INFO *) (&info));
    /*
     * Get cores 
     */
    core = BCM_GPORT_SCHEDULER_CORE_GET(gport);
    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "core must be BCM_CORE_ALL");
    }
    /*
     * Get configuration 
     */
    rc = (MBCM_DNX_DRIVER_CALL
          (unit, mbcm_dnx_mult_fabric_credit_source_get, (unit, core, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
    SHR_IF_ERR_EXIT(rc);

    if (mode != -1)
    {
        /*
         * Mode is not disable, Mode is SP only. i.e. MC2 > MC1 > MC0 
         */
        info.best_effort.wfq_enable = DNX_SAND_FALSE;
        for (index = 0; index < DNX_COSQ_FMQ_NOF_BE; index++)
        {
            info.best_effort.be_sch[index].weight = 0;
        }
    }
    else
    {
        /*
         * Set weight configuration 
         */
        info.best_effort.wfq_enable = DNX_SAND_TRUE;

        if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(gport))
        {
            info.best_effort.be_sch[0].weight = weight;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(gport))
        {
            info.best_effort.be_sch[1].weight = weight;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(gport))
        {
            info.best_effort.be_sch[2].weight = weight;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport given (0x%08x) ", gport);
        }
    }

    /*
     * Set configuration 
     */
    rc = (MBCM_DNX_DRIVER_CALL
          (unit, mbcm_dnx_mult_fabric_credit_source_set, (unit, core, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

/* 
 * Purpose : Retrieve weight, sp between FMQ class
 */
int
_bcm_dnx_cosq_gport_fmq_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    shr_error_e rc = _SHR_E_NONE;
    dnx_mult_fabric_info_t info;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core 
     */
    core = BCM_GPORT_SCHEDULER_CORE_GET(gport);

    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "core must be BCM_CORE_ALL");
    }

    /*
     * Get configuration 
     */
    rc = (MBCM_DNX_DRIVER_CALL
          (unit, mbcm_dnx_mult_fabric_credit_source_get, (unit, core, ((SOC_DNX_MULT_FABRIC_INFO *) (&info)))));
    SHR_IF_ERR_EXIT(rc);

    if (!(info.best_effort.wfq_enable))
    {
        /*
         * Mode is not disable, Mode is SP only. i.e. MC2 > MC1 > MC0 
         */
        *mode = 0;
        *weight = -1;
    }
    else
    {
        *mode = -1;

        /*
         * weight configuration 
         */
        if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(gport))
        {
            *weight = info.best_effort.be_sch[0].weight;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(gport))
        {
            *weight = info.best_effort.be_sch[1].weight;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(gport))
        {
            *weight = info.best_effort.be_sch[2].weight;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport given (0x%08x) ", gport);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
_bcm_dnx_cosq_gport_fmq_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    shr_error_e rc = _SHR_E_NONE;
    dnx_mult_fabric_info_t info;
    int core, core_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate 
     */
    if (!(BCM_COSQ_GPORT_IS_FMQ_ROOT(gport)
          || BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport)))
    {

        SHR_ERR_EXIT(_SHR_E_PORT, "unit(%d) invalid gport type (0x%08x)", unit, gport);
    }
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid cosq parameter %d\n", unit, cosq);
    }
    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid minimum kbits per second parameter %d\n", unit, kbits_sec_min);
    }
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid flags parameter %d\n", unit, flags);
    }

    /*
     * Get cores 
     */
    core = BCM_GPORT_SCHEDULER_CORE_GET(gport);
    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL");
    }

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        /*
         * Get existing configuration 
         */
        rc = (MBCM_DNX_DRIVER_CALL
              (unit, mbcm_dnx_mult_fabric_credit_source_get, (unit, core_index, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
        SHR_IF_ERR_EXIT(rc);

        if (BCM_COSQ_GPORT_IS_FMQ_ROOT(gport))
        {
            info.max_rate = kbits_sec_max;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport))
        {
            info.guaranteed.gr_shaper.rate = kbits_sec_max;
        }
        else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport))
        {
            info.best_effort.be_shaper.rate = kbits_sec_max;
        }

        /*
         * Set configuration 
         */
        rc = (MBCM_DNX_DRIVER_CALL
              (unit, mbcm_dnx_mult_fabric_credit_source_set, (unit, core_index, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}
int
_bcm_dnx_cosq_gport_fmq_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    shr_error_e rc = _SHR_E_NONE;
    dnx_mult_fabric_info_t info;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate 
     */
    if (!(BCM_COSQ_GPORT_IS_FMQ_ROOT(gport)
          || BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport)))
    {

        SHR_ERR_EXIT(_SHR_E_PORT, "unit(%d) invalid gport type (0x%08x)", unit, gport);
    }
    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid cosq parameter %d\n", unit, cosq);
    }

    /*
     * Get core 
     */
    core = BCM_GPORT_SCHEDULER_CORE_GET(gport);

    if (core != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL");
    }

    /*
     * Get existing configuration 
     */
    rc = (MBCM_DNX_DRIVER_CALL
          (unit, mbcm_dnx_mult_fabric_credit_source_get, (unit, core, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
    SHR_IF_ERR_EXIT(rc);

    *kbits_sec_min = 0;

    if (BCM_COSQ_GPORT_IS_FMQ_ROOT(gport))
    {
        *kbits_sec_max = info.max_rate;
    }
    else if (BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(gport))
    {
        *kbits_sec_max = info.guaranteed.gr_shaper.rate;
    }
    else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(gport))
    {
        *kbits_sec_max = info.best_effort.be_shaper.rate;
    }

exit:
    SHR_FUNC_EXIT;
}



int
_bcm_dnx_cosq_control_fmq_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    shr_error_e rc = _SHR_E_NONE;
    dnx_mult_fabric_info_t info;
    int core, core_index;

    SHR_FUNC_INIT_VARS(unit);
    core = BCM_CORE_ALL;
    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
        {
            /*
             * Set max burst 
             */
            /*
             * Validate 
             */
            if (!(BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(port)
                  || BCM_COSQ_GPORT_IS_FMQ_ROOT(port) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(port)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d for this gport(0x%08x) ", unit, type, port);
            }
            if (cosq != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid cosq parameter %d\n", unit, cosq);
            }
            if (arg > DNX_COSQ_FMQ_MAX_BURST_LIMIT || arg < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid max_burst %d \n", unit, arg);
            }
            /*
             * Get cores 
             */
            core = BCM_GPORT_SCHEDULER_CORE_GET(port);
            if (core != BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL");
            }
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
            {
                /*
                 * Get existing configuration 
                 */
                rc = (MBCM_DNX_DRIVER_CALL
                      (unit, mbcm_dnx_mult_fabric_credit_source_get,
                       (unit, core_index, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
                SHR_IF_ERR_EXIT(rc);

                if (BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(port))
                {
                    info.guaranteed.gr_shaper.max_burst = arg;
                }
                else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(port))
                {       /* Best effort */
                    info.best_effort.be_shaper.max_burst = arg;
                }
                else
                {
                    if (BCM_COSQ_GPORT_IS_FMQ_ROOT(port))
                    {   /* root */
                        info.max_burst = arg;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error in max burst. Not supposed to come here");
                    }
                }

                /*
                 * Set configuration 
                 */
                rc = (MBCM_DNX_DRIVER_CALL
                      (unit, mbcm_dnx_mult_fabric_credit_source_set,
                       (unit, core_index, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
                SHR_IF_ERR_EXIT(rc);
            }
        }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
_bcm_dnx_cosq_control_fmq_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    shr_error_e rc = _SHR_E_NONE;
    dnx_mult_fabric_info_t info;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
            /*
             * Set max burst 
             */
        {
            /*
             * Validate 
             */
            if (!(BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(port)
                  || BCM_COSQ_GPORT_IS_FMQ_ROOT(port) || BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(port)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid type %d for this gport(0x%08x) ", unit, type, port);
            }
            if (cosq != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid cosq parameter %d\n", unit, cosq);
            }

            /*
             * Get core 
             */
            core = BCM_GPORT_SCHEDULER_CORE_GET(port);

            if (core != BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL");
            }

            /*
             * Get existing configuration 
             */
            rc = (MBCM_DNX_DRIVER_CALL
                  (unit, mbcm_dnx_mult_fabric_credit_source_get, (unit, core, (SOC_DNX_MULT_FABRIC_INFO *) (&info))));
            SHR_IF_ERR_EXIT(rc);

            if (BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(port))
            {
                *arg = info.guaranteed.gr_shaper.max_burst;
            }
            else if (BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(port))
            {
                /*
                 * Best effort 
                 */
                *arg = info.best_effort.be_shaper.max_burst;
            }
            else
            {
                if (BCM_COSQ_GPORT_IS_FMQ_ROOT(port))
                {       /* root */
                    *arg = info.max_burst;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error in max burst. Not supposed to come here");
                }
            }
        }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}


#if (0)
/* { */
/*
 * This procedure is not called from anywhere in the code...
 */
/* 
 * Purpose:
 * Retrieve specific multicast configuration (out_dp) from port
 */
int
_bcm_dnx_cosq_gport_egress_multicast_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config)
{
    int core;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    SOC_DNX_EGR_Q_PRIO_MAPPING_TYPE map_type;
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
    map_type = SOC_DNX_EGR_MCAST_TO_UNSCHED;
    config->scheduled_dp = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].dp;
    config->priority = mapping_info.queue_mapping[map_type][ingress_pri][ingress_dp].tc;
exit:
    SHR_FUNC_EXIT;
}
/* } */
#endif

int
_bcm_dnx_cosq_priority_mode_to_nof_priorities(
    int unit,
    SOC_DNX_EGR_PORT_PRIORITY_MODE priority_mode,
    int *nof_priorities)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (priority_mode)
    {
        case SOC_DNX_EGR_PORT_ONE_PRIORITY:
            *nof_priorities = 1;
            break;
        case SOC_DNX_EGR_PORT_TWO_PRIORITIES:
            *nof_priorities = 2;
            break;
        case SOC_DNX_EGR_PORT_EIGHT_PRIORITIES:
            *nof_priorities = 8;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority type is invalid", unit);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
