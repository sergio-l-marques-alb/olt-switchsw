/** \file algo_port_imb.c
 *  $Id$ Port IMB algorithms for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Includes.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>

/*
 * }
 */
/*
 * Macros
 * {
 */
/*
 * }
 */

/*
 * Local functions (documantation in function implementation)
 * {
 */
/*
 * }
 */
/**
 * \brief - calculate the thershold after overflow for the fifo according to the FIFO size. 
 * After FIFO reches overflow, writing to the fifo will resume only after fifo level 
 * goes below this value.  
 * 
 * see .h file
 */
shr_error_e
dnx_algo_port_imb_threshold_after_ovf_get(
    int unit,
    bcm_port_t port,
    uint32 fifo_size,
    uint32 *thr_after_ovf)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * ASIC recommended 80% of fifo size*  
     */
    *thr_after_ovf = fifo_size * 8 / 10;

    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate the weight to be set for the CDU in the 
 * Low priority scheduler. the weight is set per CDU - the 
 * bigger the weight, the CDU will get more BW in the RR. 
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_cdu_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *weight)
{
    int bandwidth;
    int nof_bits;
    int nof_cdu_schedulder_bits;
    SHR_FUNC_INIT_VARS(unit);

    nof_cdu_schedulder_bits = dnx_data_nif.schedulder.nof_cdu_bits_get(unit);
    SHR_IF_ERR_EXIT(imb_bandwidth_get(unit, port, is_port_include, &bandwidth));

    if (bandwidth == 0)
    {
        nof_bits = 0;
    }
    else
    {
        /*
         * SW decision to add one bit in the scheduler for every 100G traffic on the CDU
         */
        nof_bits = 1 + (bandwidth - 1) / dnx_data_nif.schedulder.rate_per_cdu_bit_get(unit);
    }

    /*
     * Number of bits should not exceed the number of CDU scheduler bits
     */
    nof_bits = ((nof_bits > nof_cdu_schedulder_bits) ? nof_cdu_schedulder_bits : nof_bits);

    *weight = nof_bits;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate the weight to be set for the RMC in the 
 * Low priority scheduler. the weight is set per RMC in the CDU 
 * - the bigger the weight, the RMC will get more BW in the RR. 
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_rmc_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int *weight)
{
    int speed;
    int nof_bits;
    int nof_rmc_schedulder_bits;
    SHR_FUNC_INIT_VARS(unit);

    nof_rmc_schedulder_bits = dnx_data_nif.schedulder.nof_rmc_bits_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &speed));

    /*
     * SW decision to add one bit in the scheduler for every 50G of the port speed
     */
    nof_bits = 1 + (speed - 1) / dnx_data_nif.schedulder.rate_per_rmc_bit_get(unit);

    /*
     * Number of bits should not exceed the number of RMC scheduler bits
     */
    nof_bits = ((nof_bits > nof_rmc_schedulder_bits) ? nof_rmc_schedulder_bits : nof_bits);

    *weight = nof_bits;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - algorithm to get the RMC id according to the lane 
 * index in the CDU and scheduler priority. 
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_cdu_port_rmc_id_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t prio,
    int *rmc_id)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    switch (prio)
    {
        case bcmPortNifSchedulerLow:
            *rmc_id = cdu_info.first_lane_in_cdu * 2;
            break;
        case bcmPortNifSchedulerHigh:
            *rmc_id = (cdu_info.first_lane_in_cdu * 2) + 1;
            break;
        case bcmPortNifSchedulerTDM:
            /*
             * no TDM in JER2
             */
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid scheduler priority");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - algorithm to calculate the first and last entry of a
 *        specific logical fifo in the CDU memory.
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_rmc_entry_range_get(
    int unit,
    bcm_port_t port,
    int nof_entries,
    int prev_last_entry,
    int nof_prio_groups,
    uint32 *first_entry,
    uint32 *last_entry)
{
    int nof_phys, cdu_nof_lanes, cdu_mlf_nof_entries;
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 total_first_entry, total_last_entry, rmc_fifo_total_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get port CDU info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * get port nof phys
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));

    cdu_nof_lanes = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    cdu_mlf_nof_entries = dnx_data_nif.eth.cdu_memory_entries_nof_get(unit);
    /*
     * claculate first and last entry bounderies for all port's RMCs depending on first lane and nof lanes
     */
    total_first_entry = (cdu_info.first_lane_in_cdu * cdu_mlf_nof_entries) / cdu_nof_lanes;
    total_last_entry = total_first_entry + ((nof_phys * cdu_mlf_nof_entries) / cdu_nof_lanes) - 1;

    *first_entry = (prev_last_entry == -1) ? total_first_entry : prev_last_entry + 1;
    if (nof_entries == -1)
    {
        /*
         * number of entries should be divided equally between all RMCs
         */
        rmc_fifo_total_size = total_last_entry - total_first_entry;
        *last_entry = *first_entry + (rmc_fifo_total_size / nof_prio_groups);
    }
    else
    {
        /*
         * number of entries for the RMC is given specifically
         */
        *last_entry = *first_entry + nof_entries - 1;
    }
    if (*last_entry > total_last_entry)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority Group number of entries exceeds port %d FIFO limit", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - segment is a resource of ILKN, ehich has to be 
 *        shared between the two ports of the same ILKN core.
 *        the division can be as follows:
 * 1. ILKN0 has 4 segments (ILKN1 is not active) 
 * 2. ILKN0 and ILKN1 get two segments each. 
 * the number of segment a port should get is determined by the 
 * number of lanes, lane rate, burst short value and core clock 
 * of the device. if there is not enough segments to support the 
 * port, an error should be returned. 
 */
shr_error_e
dnx_algo_port_imb_nof_segments_get(
    int unit,
    bcm_port_t port,
    int *nof_segments)
{
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    /** Verify port type is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_supported_verify(unit, port_type));

    /*
     * later on we should add a calculation here based on SLE excel file. 
     * but in Jer2, there is only one port in every ILKN core, so we can always return 4 segments. 
     * we can generally say that whenever there can be only one port per ILKN core,                                                     .
     * we can set nof segment to 4 and skip the calculation.
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1 /* include elk */ ))
    {
        if (dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit) == 1)
        {
            *nof_segments = 4;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of segments should be calculated for port %d", port);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of segment is a supported on ILKN ports only. port %d is not ILKN", port);
    }
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
