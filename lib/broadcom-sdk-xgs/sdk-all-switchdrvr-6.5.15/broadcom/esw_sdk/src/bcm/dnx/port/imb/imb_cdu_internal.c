/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dbal/dbal.h>
#include "imb_cdu_internal.h"
#include <shared/utilex/utilex_bitstream.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

static int imb_cdu_scheduler_nof_ports_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    int *nof_ports);

static int imb_cdu_rmc_to_lane_map(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int first_cdu_lane,
    dnx_algo_port_rmc_info_t * rmc);

static int imb_cdu_lane_to_rmc_map(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int first_lane_in_cdu,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid);

/**
 * \brief - set TX start threshold. this threshold represent the 
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 * 
 *  see .h file
 */
int
imb_cdu_internal_tx_start_thr_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane,
    uint32 start_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

    if (lane == IMB_CDU_ALL_LANES)
    {
        /*
         * All lanes in CDU
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_THR, INST_SINGLE, start_thr);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set TX start threshold. this threshold represent the 
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 * 
 *  see .h file
 */
int
imb_cdu_internal_tx_start_thr_get(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane,
    uint32 *start_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_START_THR, INST_SINGLE, start_thr);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable Tx data to the PM
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] cdu_id - cdu id inside the core
 * \param [in] lane - lane index in the CDU
 * \param [in] enable - enable indocation
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
imb_cdu_tx_data_to_pm_enable_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

    if (lane == IMB_CDU_ALL_LANES)
    {
        /*
         * All lanes in CDU
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_DATA, INST_SINGLE, enable ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable Tx data to the PM. 
 *      see imb_cdu_tx_data_to_pm_enable_set
 */
int
imb_cdu_port_tx_data_to_pm_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * call direct function 
     */
    SHR_IF_ERR_EXIT(imb_cdu_tx_data_to_pm_enable_set
                    (unit, cdu_info.core, cdu_info.inner_cdu_id, cdu_info.first_lane_in_cdu, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all master logical ports on a CDU
 *  
 * see .h file 
 */
int
imb_cdu_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * cdu_ports)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    int first_phy, i, pm_i;
    bcm_port_t port_i;
    bcm_pbmp_t all_ports, cdu_phys, pm_phys;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*cdu_ports);
    /*
     * get port cdu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * get CDU lanes
     */
    BCM_PBMP_CLEAR(cdu_phys);
    for (i = 0; i < dnx_data_nif.eth.cdu_pms_nof_get(unit); i++)
    {
        pm_i = dnx_data_nif.eth.cdu_pm_map_get(unit, cdu_info.cdu_id)->pms[i];
        pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_i)->phys;
        BCM_PBMP_OR(cdu_phys, pm_phys);
    }
    /*
     * get all nif logical ports for our specific device core (0 or 1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, cdu_info.core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_ports));
    BCM_PBMP_ITER(all_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port_i, 0, &first_phy));
        /*
         * check if port first phy is in CDU range
         */
        if (BCM_PBMP_MEMBER(cdu_phys, first_phy))
        {
            BCM_PBMP_PORT_ADD(*cdu_ports, port_i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all ilkn master logical ports on a CDU
 *
 * see .h file
 */
int
imb_cdu_ilkn_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * cdu_ilkn_ports)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    int i, pm_i, nof_ilkn_lanes_on_cdu, is_over_fabric;
    bcm_port_t ilkn_port_i;
    bcm_pbmp_t all_ilkn_ports, cdu_phys, pm_phys, ilkn_lanes;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*cdu_ilkn_ports);
    /*
     * get port cdu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * get CDU lanes
     */
    BCM_PBMP_CLEAR(cdu_phys);
    for (i = 0; i < dnx_data_nif.eth.cdu_pms_nof_get(unit); i++)
    {
        pm_i = dnx_data_nif.eth.cdu_pm_map_get(unit, cdu_info.cdu_id)->pms[i];
        pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_i)->phys;
        BCM_PBMP_OR(cdu_phys, pm_phys);
    }
    /*
     * get all ilkn logical ports on our specific device core (0 or 1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, cdu_info.core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_ilkn_ports));
    BCM_PBMP_ITER(all_ilkn_ports, ilkn_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, ilkn_port_i, 0, &ilkn_lanes));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, ilkn_port_i, &is_over_fabric));
        /*
         * check if ilkn port is in CDU range.
         * Note that ilkn-over-fabric might overlap the same links as the CDU so need to confirm ilkn is not over fabric
         */
        BCM_PBMP_AND(ilkn_lanes, cdu_phys);
        BCM_PBMP_COUNT(ilkn_lanes, nof_ilkn_lanes_on_cdu);
        if ((nof_ilkn_lanes_on_cdu > 0) && (!is_over_fabric))
        {
            BCM_PBMP_PORT_ADD(*cdu_ilkn_ports, ilkn_port_i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dbal_sch_weight_get(
    int unit,
    int weight,
    uint32 *dbal_sch_weight)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (weight)
    {
        case 0:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0;
            break;
        case 1:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_1;
            break;
        case 2:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_2;
            break;
        case 3:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_3;
            break;
        case 4:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_4;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight %d\n", weight);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the RMC priority
 *  
 * see .h file 
 */
int
imb_cdu_port_rmc_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    bcm_port_nif_scheduler_t sch_prio)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 is_tdm_priority = 0, is_high_priority = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_tdm_priority = (sch_prio == bcmPortNifSchedulerTDM) ? 1 : 0;
    is_high_priority = (sch_prio == bcmPortNifSchedulerHigh) ? 1 : 0;

    /*
     * Get CDU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    /*
     * alloc DABL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, is_high_priority);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the CDU in the specified Scheduler
 *
 * see .h file
 */
int
imb_cdu_rmc_low_prio_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int weight)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id, dbal_sch_weight;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    /*
     * convert to DBAL weight from reguler int (0-4)
     */
    SHR_IF_ERR_EXIT(dbal_sch_weight_get(unit, weight, &dbal_sch_weight));
    /*
     * alloc DABL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_PRIO_WEIGHT, INST_SINGLE, dbal_sch_weight);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dbal_sch_prio_from_bcm_sch_prio_get(
    int unit,
    bcm_port_nif_scheduler_t bcm_sch_prio,
    uint32 *dbal_sch_prio)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_sch_prio)
    {
        case bcmPortNifSchedulerLow:
            *dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_LOW;
            break;
        case bcmPortNifSchedulerHigh:
            *dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_HIGH;
            break;
        case bcmPortNifSchedulerTDM:
            *dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_TDM;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "No type %d in bcm_port_nif_acheduler_t\n", bcm_sch_prio);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the CDU in the specified Scheduler
 *  
 * see .h file 
 */
int
imb_cdu_scheduler_config_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    bcm_port_nif_scheduler_t sch_prio,
    int weight)
{
    uint32 entry_handle_id, dbal_sch_weight;
    uint32 dbal_sch_prio = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * convert bcm sch_prio to DBAL sch_prio
     */
    SHR_IF_ERR_EXIT(dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_prio, &dbal_sch_prio));
    /*
     * convert to DBAL weight from reguler int (0-4)
     */
    SHR_IF_ERR_EXIT(dbal_sch_weight_get(unit, weight, &dbal_sch_weight));
    /*
     * alloc DABL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, dbal_sch_prio);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT, INST_SINGLE, dbal_sch_weight);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of ports from a CDU on the specified 
 *        scheduler
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_prio - Scheduler. see 
 *        bcm_port_nif_scheduler_t
 * \param [out] nof_ports - how many ports from the CDU use this 
 *        scheduler.
 *   
 * \return
 *   int - seee _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_cdu_scheduler_nof_ports_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    int *nof_ports)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    bcm_pbmp_t cdu_ports;
    dnx_algo_port_rmc_info_t rmc;
    bcm_port_t port_i;
    int ii, ports_counter, nof_priority_groups;
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    /*
     * Get CDU ports bitmap 
     */
    SHR_IF_ERR_EXIT(imb_cdu_master_ports_get(unit, port, &cdu_ports));

    ports_counter = 0;
    BCM_PBMP_ITER(cdu_ports, port_i)
    {
        for (ii = 0; ii < nof_priority_groups; ++ii)
        {
            /*
             * Get logical fifo information 
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port_i, 0, ii, &rmc));
            if ((rmc.rmc_id != IMB_CDU_RMC_INVALID) && (rmc.sch_priority == sch_prio))
            {
                ++ports_counter;
            }
        }
    }
    *nof_ports = ports_counter;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the port's CDU as active in the specified 
 *        scheduler
 * see .h file 
 */
int
imb_cdu_port_scheduler_active_set(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 is_active)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 curr_active;
    int nof_ports = 0, weight = 0, nof_channels = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    if (sch_prio == bcmPortNifSchedulerLow)
    {
        /*
         * Get the weight for the low priority scheduler 
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_cdu_scheduler_weight_get(unit, port, is_active, &weight));
        SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_set(unit, cdu_info.core, cdu_info.inner_cdu_id, sch_prio, weight));
    }
    else        /* High / TDM scheduler */
    {
        SHR_IF_ERR_EXIT(imb_cdu_port_scheduler_active_get(unit, port, sch_prio, &curr_active));
        if (is_active == curr_active)
        {
            /*
             * Nothing to do
             */
            SHR_EXIT();
        }
        else if (is_active)
        {
            /*
             * This is the first port in the CDU that needs this priority 
             */
            /*
             * for TDM / High priority - we always set the weight to the max BW - 2 bits per CDU. 
             */
            SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_set(unit, cdu_info.core, cdu_info.inner_cdu_id, sch_prio, 2));
        }
        else if (is_active == 0)
        {
            /*
             * before disabling this CDU in the High / TDM scheduler, we need to check if other 
             * ports on the CDU need this scheduler 
             */
            SHR_IF_ERR_EXIT(imb_cdu_scheduler_nof_ports_get(unit, port, sch_prio, &nof_ports));
            /*
             * Get nof channels on the port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
            if ((nof_ports == 1) && (nof_channels == 1))
            {
                SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_set(unit, cdu_info.core, cdu_info.inner_cdu_id, sch_prio, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get CDU configuration in specified scheduler
 * 
 * see .h file
 */
int
imb_cdu_scheduler_config_get(
    int unit,
    bcm_core_t core,
    int cdu_id,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DABL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_prio);

    /*
     * request value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT, INST_SINGLE, weight);

    /*
     * commit the request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port CDU active indication in the specified 
 *        scheduler.
 * see .h file 
 */
int
imb_cdu_port_scheduler_active_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *active)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 weight;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    SHR_IF_ERR_EXIT(imb_cdu_scheduler_config_get(unit, cdu_info.core, cdu_info.inner_cdu_id, sch_prio, &weight));

    *active = weight ? 1 : 0;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map RMC (logical FIFO) to a physical lane in the CDU
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] first_cdu_lane - port first lane index in the cdu
 * \param [in] rmc - logical fifo info.
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_cdu_rmc_to_lane_map(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int first_cdu_lane,
    dnx_algo_port_rmc_info_t * rmc)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc->rmc_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, INST_SINGLE, first_cdu_lane);
    /*
     * channel 0-1 => low/high? 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, INST_SINGLE,
                                 rmc->sch_priority == bcmPortNifSchedulerLow ? 0 : 1);
    /*
     * no TDM in JER2 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MLF_FIRST_ENTRY, INST_SINGLE, rmc->first_entry);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MLF_LAST_ENTRY, INST_SINGLE, rmc->last_entry);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map RMC to a specific lane in the cdu.
 * see imb_cdu_rmc_to_lane_map.
 */
int
imb_cdu_port_rmc_to_lane_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_rmc_to_lane_map
                    (unit, cdu_info.core, cdu_info.inner_cdu_id, is_map_valid ? cdu_info.first_lane_in_cdu : 0, rmc));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the CDU + PRD priority to a 
 *        specific RMC (logical FIFO)
 * 
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] first_lane_in_cdu - port first lane index in the CDU
 * \param [in] rmc - logical fifo info
 * \param [in] is_map_valid - map valid indication: TRUE => map, 
 *        FALSE=> unmap
 *   
 * \return
 *   int 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_cdu_lane_to_rmc_map(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int first_lane_in_cdu,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid)
{
    uint32 entry_handle_id;
    int rmc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * set this RMC in RMCs bitmap for the lane 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_LANE_RMC_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, first_lane_in_cdu);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc->rmc_id);

    /*
     * enable the RMC for the lane
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE, INST_SINGLE, is_map_valid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Clear the handle before re-use
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_CDU_RX_LANE_RMC_MAP, entry_handle_id));

    /*
     * if map is invalid, map all prioritties to RMC 0
     */
    rmc_id = is_map_valid ? rmc->rmc_id : 0;
    /*
     * map RMC to lane + PRD priority 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_PRIORITY_TO_RMC_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, first_lane_in_cdu);

    /*
     * RMC can be mapped to more than one PRD priority of hte lane (even to all priorities), so each priority is
     * checked independently
     */
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_0)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_2)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_3)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_TDM)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, DBAL_DEFINE_NIF_PRD_PRIORITY_TDM);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the CDU + PRD priority to a
 *        specific RMC (logical FIFO)
 *  see imb_cdu_lane_to_rmc_map
 */
int
imb_cdu_port_lane_to_rmc_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_lane_to_rmc_map(unit, cdu_info.core, cdu_info.inner_cdu_id, cdu_info.first_lane_in_cdu,
                                            rmc, is_map_valid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the logical FIFO threshold after overflow. 
 * after the fifo reaches overflow, it will not resume writing 
 * until fifo level will get below this value. 
 *  
 * see .h file 
 */
int
imb_cdu_port_rmc_thr_after_ovf_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int rmc_id,
    int thr_after_ovf)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_AFTER_OVF, INST_SINGLE, thr_after_ovf);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the logical FIFO threshold after overflow. 
 *   see imb_cdu_thr_after_ovf_set
 */
int
imb_cdu_port_thr_after_ovf_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_port_rmc_thr_after_ovf_set
                    (unit, cdu_info.core, cdu_info.inner_cdu_id, rmc->rmc_id, rmc->thr_after_ovf));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable the MAC in the CDU
 * see .h file 
 */
int
imb_cdu_port_mac_enable_set(
    int unit,
    bcm_port_t port,
    int mac_mode,
    int enable)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    /*
     * alooc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_CTRL, &entry_handle_id));

    /*
     * set key fields 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_ID, cdu_info.mac_id);

    /*
     * set value field 
     */
    if (enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE, INST_SINGLE, mac_mode);
    }
    else
    {
        /*
         * return to default value - ignore mac_mode argument
         */
        dbal_entry_value_field_predefine_value_set(unit,
                                                   entry_handle_id, DBAL_FIELD_MAC_MODE, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_DEFAULT_VALUE);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the Async unit in the CDU
 * 
 * see .h file 
 */
int
imb_cdu_port_async_unit_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_ID, cdu_info.mac_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_ASYNC_UNIT, INST_SINGLE,
                                 in_reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port in the NMG
 * see .h file
 */
int
imb_cdu_port_rx_nmg_reset(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset)
{
    dnx_algo_port_rmc_info_t rmc;
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id;
    int ii, nof_rmcs_to_reset, rmc_id_to_reset, nof_priority_groups;
    int rmc_cnt = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    nof_rmcs_to_reset = (rmc_id == IMB_CDU_ALL_RMCS) ? nof_priority_groups : 1;
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * go over all logical FIFOs of the port
     */
    for (ii = 0; ii < nof_rmcs_to_reset; ++ii)
    {
        /*
         * get logical fifo information
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        rmc_id_to_reset = (rmc_id == IMB_CDU_ALL_RMCS) ? rmc.rmc_id : rmc_id;
        if (rmc_id_to_reset == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }

        rmc_cnt++;

        /*
         * set logical fifo key
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id_to_reset);
        /*
         * set reset indication
         */
        dbal_entry_value_field32_set(unit,
                                     entry_handle_id,
                                     DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                     in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    }

    if (rmc_cnt > 0)
    {
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port PM.
 * see .h file
 */
int
imb_cdu_port_pm_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_CTRL, &entry_handle_id));
    /*
     * set key fields 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * set value fields 
     * Note: taking all lanes out-of-reset even if there is only one active lane on the CDU,
     * since the per lane pwrdown is not taking lane mpa into account.
     * e.g. if lane 0 on the chip is connected to rx of lane 1 and tx of lane 2, then when power down lane 0,
     * it can affect two different ports. 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_LANES_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_PM_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_PM_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port PM.
 * see .h file
 */
int
imb_cdu_internal_pm_reset_set(
    int unit,
    int imb_id,
    uint32 in_reset)
{
    uint32 entry_handle_id;
    int pm_index;
    bcm_pbmp_t phys;
    int first_phy_port, nof_cdus_per_core, nof_phys_per_cdu, nof_phys_per_core;
    int core, cdu_id, inner_cdu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    nof_phys_per_core = nof_cdus_per_core * nof_phys_per_cdu;

    pm_index = dnx_data_nif.eth.cdu_pm_map_get(unit, imb_id)->pms[0];
    phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;

    _SHR_PBMP_FIRST(phys, first_phy_port);

    core = first_phy_port / nof_phys_per_core;
    cdu_id = (first_phy_port / nof_phys_per_cdu);
    inner_cdu_id = cdu_id % nof_cdus_per_core;

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    /*
     * set value fields
     * Note: taking all lanes out-of-reset even if there is only one active lane on the CDU,
     * since the per lane pwrdown is not taking lane mpa into account.
     * e.g. if lane 0 on the chip is connected to rx of lane 1 and tx of lane 2, then when power down lane 0,
     * it can affect two different ports.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_LANES_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_PM_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_PM_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port PM.
 * see .h file
 */
int
imb_cdu_internal_pm_reset_get(
    int unit,
    int imb_id,
    uint32 *in_reset)
{
    uint32 entry_handle_id;
    uint32 in_reset_dbal;
    int pm_index;
    bcm_pbmp_t phys;
    int first_phy_port, nof_cdus_per_core, nof_phys_per_cdu, nof_phys_per_core;
    int core, cdu_id, inner_cdu_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    nof_phys_per_core = nof_cdus_per_core * nof_phys_per_cdu;

    pm_index = dnx_data_nif.eth.cdu_pm_map_get(unit, imb_id)->pms[0];
    phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;

    _SHR_PBMP_FIRST(phys, first_phy_port);

    core = first_phy_port / nof_phys_per_core;
    cdu_id = (first_phy_port / nof_phys_per_cdu);
    inner_cdu_id = cdu_id % nof_cdus_per_core;

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PM_RESET, INST_SINGLE, &in_reset_dbal);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *in_reset = (in_reset_dbal == DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET) ? 1 : 0;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_tx_egress_flush_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int first_lane,
    int flush_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    if (first_lane == IMB_CDU_ALL_LANES)
    {
        /*
         * All lanes in CDU
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, first_lane);
    }
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH, INST_SINGLE, flush_enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable egress flush. Egress flush means 
 *        continously geanting credits to the port in order to
 *        free occupied resources.
 *  
 * see .h file
 */
int
imb_cdu_port_tx_egress_flush_set(
    int unit,
    bcm_port_t port,
    int flush_enable)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_tx_egress_flush_set
                    (unit, cdu_info.core, cdu_info.inner_cdu_id, cdu_info.first_lane_in_cdu, flush_enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port in the CDU.
 *  
 * see .h file 
 */
int
imb_cdu_port_tx_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * Reset the port in TX 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - internal use only, Reset the port credit tx lane ctrl.
 *
 */
int
imb_cdu_port_tx_lane_ctrl_credit_reset(
    int unit,
    dnx_algo_port_cdu_access_info_t cdu_info,
    bcm_pbmp_t phys,
    int value)
{
    int phy, lane;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    BCM_PBMP_ITER(phys, phy)
    {
        lane = phy % dnx_data_nif.eth.cdu_lanes_nof_get(unit);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);
        /*
         * set value field 0
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RESET, INST_SINGLE, value);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port credit in the CDU.
 *
 * see .h file
 */
int
imb_cdu_port_credit_tx_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    bcm_pbmp_t phys;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    /*
     * Reset the port credit value in TX
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_VALUE, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset the port credit in TX - sequence 0 1 0
     */
    SHR_IF_ERR_EXIT(imb_cdu_port_tx_lane_ctrl_credit_reset(unit, cdu_info, phys, 0));
    SHR_IF_ERR_EXIT(imb_cdu_port_tx_lane_ctrl_credit_reset(unit, cdu_info, phys, 1));
    SHR_IF_ERR_EXIT(imb_cdu_port_tx_lane_ctrl_credit_reset(unit, cdu_info, phys, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port tx in the NMG
 * see .h file
 */
int
imb_cdu_port_tx_nmg_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * set lane we want to reset as key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set reset indication
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id,
                                 DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port in the CDU.
 *  
 * see .h file 
 */
int
imb_cdu_port_rx_reset(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset)
{
    dnx_algo_port_cdu_access_info_t cdu_info;
    dnx_algo_port_rmc_info_t rmc;
    uint32 entry_handle_id;
    int ii, nof_rmcs_to_reset, rmc_id_to_reset, nof_priority_groups;
    int rmc_cnt = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    nof_rmcs_to_reset = (rmc_id == IMB_CDU_ALL_RMCS) ? nof_priority_groups : 1;
    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * Reset the port in RX - port is represented by logical FIFOs 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);

    for (ii = 0; ii < nof_rmcs_to_reset; ++ii)
    {
        /*
         * get logical fifo info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        rmc_id_to_reset = (rmc_id == IMB_CDU_ALL_RMCS) ? rmc.rmc_id : rmc_id;
        if (rmc_id_to_reset == IMB_CDU_RMC_INVALID)
        {
            /*
             * RMC id not valid
             */
            continue;
        }

        rmc_cnt++;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id_to_reset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                     in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    }

    if (rmc_cnt > 0)
    {
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see .h file 
 */

int
imb_cdu_port_pm_boundary_get(
    int unit,
    bcm_port_t port,
    int *lower_bound,
    int *upper_bound)
{
    dnx_algo_port_cdu_access_info_t cdu_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));

    *lower_bound = cdu_info.cdu_first_phy;
    *upper_bound = *lower_bound + dnx_data_nif.eth.cdu_lanes_nof_get(unit) - 1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable EEE for the CDU 
 * see .h file 
 */
int
imb_cdu_eee_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEE_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    /*
     * SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
     */
exit:DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the RMC current FIFO level
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to check fifo level
 * \param [out] fifo_level - RMC FIFO fullnes level 
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
imb_cdu_rmc_level_get(
    int unit,
    bcm_port_t port,
    uint32 rmc_id,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser wether to ignore indication of IP 
 *        dscp even if packet is identified as IP.
 *  
 * see .h file
 */
int
imb_cdu_prd_ignore_ip_dscp_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_ip_dscp)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication wether it ignores IP 
 *        DSCP
 *  
 * see .h file
 */
int
imb_cdu_prd_ignore_ip_dscp_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_ip_dscp)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is 
 *        identified as MPLS
 * 
 * see .h file
 */
int
imb_cdu_prd_ignore_mpls_exp_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_mpls_exp)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication wether it ignores 
 *        MPLS EXP
 * 
 * see .h file
 */
int
imb_cdu_prd_ignore_mpls_exp_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_mpls_exp)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser wether to ignore inner tag PCP DEI 
 *        indication even if packet is identified as double
 *        tagged
 * 
 * see .h file
 */
int
imb_cdu_prd_ignore_inner_tag_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_inner_tag)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication wether it ignores 
 *        inner VLAN tag
 * 
 * see .h file 
 */
int
imb_cdu_prd_ignore_inner_tag_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_inner_tag)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser wether to ignore outer tag PCP DEI 
 *        indication even if packet is identified as VLAN tagged
 * 
 * see .h file
 */
int
imb_cdu_prd_ignore_outer_tag_set(
    int unit,
    bcm_port_t port,
    uint32 ignore_outer_tag)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication wether it ignores 
 *        outer VLAN tag
 * 
 * see .h file
 */
int
imb_cdu_prd_ignore_outer_tag_get(
    int unit,
    bcm_port_t port,
    uint32 *ignore_outer_tag)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set defualt priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overriden by the priority from the soft
 *        stage.
 * 
 * see .h file
 */
int
imb_cdu_prd_default_priority_set(
    int unit,
    bcm_port_t port,
    uint32 defualt_priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE, defualt_priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get default priority given in the PRD hard stage 
 *        parser.
 * 
 * see .h file
 */
int
imb_cdu_prd_default_priority_get(
    int unit,
    bcm_port_t port,
    uint32 *defualt_priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE, defualt_priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the TM priority map (PRD hard stage). add the 
 *        the map priorty value to be returned per TC + DP
 *        values
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_tm_tc_dp_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from TM priority map (PRD hard 
 *        stage). get the priority given for a conbination of
 *        TC+DP.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_tm_tc_dp_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the IP priority table (PRD hard stage). set 
 *        priority value for a specific DSCP.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_ip_dscp_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from IP priority table (PRD hard 
 *        stage). get the priorty returned for a specific DSCP
 * 
 * See .h file
 */
int
imb_cdu_internal_prd_map_ip_dscp_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set 
 *        priority value for a specific EXP value.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_mpls_exp_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the MPLS priority table. get 
 *        the priority returned for a specific EXP value
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_mpls_exp_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the ETH (VLAN) Prioroity table (PRD hard 
 *        stage). set a priority value to match a spcific
 *        PCP+DEI combination
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_eth_pcp_dei_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the ETH (VLAN) priority table 
 *        (PRD hard stage). get the priorty returned for a
 *        specific combination of PCP+DEI
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_map_eth_pcp_dei_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD thresholds per priority. based on the 
 *        prioroity given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 priority,
    uint32 threshold)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current threshold for a specific priority in 
 *        a specific RMC.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set one of the port's TPID values to be recognized 
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get one of the TPID values recognized by the PRD 
 *        Parser.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD drop counter value, per RMC
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint64 *count)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_DROP_COUNTER, INST_SINGLE, count);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set configurable ether type to a ether type code. 
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 * 
 * See .h file
 */
int
imb_cdu_internal_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type value mapped to a specific ether 
 *        type code, out of the configurable ether types (codes
 *        1-6). 
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the PRD soft stage TCAM table. there are 32 
 *        entries in the table. if there is a hit, the priority
 *        for the packet will be taken from the TCAM entry
 *        information 
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tcam_entry_set(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    imb_cdu_internal_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 entry_info->ether_code_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 entry_info->ether_code_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                                 entry_info->offset_array_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                                 entry_info->offset_array_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, entry_info->priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the PRD soft stage (TCAM) 
 *        table. get specific entry information
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tcam_entry_get(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    imb_cdu_internal_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &entry_info->ether_code_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &entry_info->ether_code_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                               &entry_info->offset_array_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                               &entry_info->offset_array_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &entry_info->priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key 
 *        per ether type (total of 16 keys). the key is build of
 *        4 offstes given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCMA entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        ----------------------------------------------------- 
 *        35              31       23       15       7        0
 *  
 * see .h file
 */
int
imb_cdu_internal_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information about the TCAM key for a specific 
 *        ether type. 
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the offset base for the TCAM key. it means the 
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet 
 * 1=>end of eth header 
 * 2=>end of header after eth header. 
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current offset base for the given ether 
 *        code.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ether type size (in bytes) for the given ether 
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 *  
 * see .h file 
 */
int
imb_cdu_internal_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type currently set for a specific 
 *        ether type code
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the higest priroity (3). each
 *        packet is compared against all control frame
 *        properties of the CDU
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * set value field
     */
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                                 control_frame_config->mac_da_val);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                                 control_frame_config->mac_da_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 control_frame_config->ether_type_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get Control Frame properties recognized by the 
 *        parser. 
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                               &control_frame_config->mac_da_val);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                               &control_frame_config->mac_da_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &control_frame_config->ether_type_code);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD hard stage per RMC. this effectively 
 *        enable the PRD feature.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for the PRD hard stage per RMC
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 *enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a 
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_soft_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable_eth,
    uint32 enable_tm)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for PRD soft stage
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_soft_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable_eth,
    uint32 *enable_tm)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_itmh_offsets_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ITMH TC and DP offstes in PRD
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_itmh_offsets_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ITMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_ftmh_offsets_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get FTMH TC + DP offsets in PRD
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_ftmh_offsets_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable checking for MPLS special label, if packet is 
 *        identified as MPLS
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_mpls_special_label_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for MPLS special label.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_mpls_special_label_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set MPLS special label properties. if one of the 
 *        MPLS labels is identified as special label and its
 *        value match one of the special label values, priority
 *        for the packet is taken from the special label
 *        properties.
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, label_config->label_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, label_config->priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get current configuration of MPLS special label 
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, &label_config->label_value);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &label_config->priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set outer tag size for the port. if port is port 
 *        extander, the outer tag size should be set to 8B,
 *        otherwise 4B
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_outer_tag_size_set(
    int unit,
    bcm_port_t port,
    uint32 outer_tag_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TAG_SIZE, INST_SINGLE, outer_tag_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get current outer tag size for the port
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_outer_tag_size_get(
    int unit,
    bcm_port_t port,
    uint32 *outer_tag_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUTER_TAG_SIZE, INST_SINGLE, outer_tag_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD port type, acocrding to the header type of 
 *        the port
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_port_type_set(
    int unit,
    bcm_port_t port,
    uint32 prd_port_type)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_type);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD port type for the port
 * 
 * see .h file
 */
int
imb_cdu_internal_prd_port_type_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_port_type)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_type);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get NIF RX fifo status
 *
 * see .h file
 */
int
imb_cdu_internal_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    int rmc_id,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_RX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_OCCUPANCY, INST_SINGLE, max_occupancy);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get NIF_TX fifo status
 *
 * see .h file
 */

int
imb_cdu_internal_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits)
{
    uint32 entry_handle_id;
    dnx_algo_port_cdu_access_info_t cdu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get CDU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_TX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, cdu_info.first_lane_in_cdu);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_OCCUPANCY, INST_SINGLE, max_occupancy);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PM_CREDITS, INST_SINGLE, pm_credits);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map NIF port to EGQ interface
 *
 * see .h file
 */
int
imb_cdu_internal_nif_egq_interface_map(
    int unit,
    bcm_core_t core,
    uint32 nif_id,
    uint32 egq_if)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_EGQ_INTERFACE_MAP, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ID, nif_id);

    /*
     * request for value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE, INST_SINGLE, egq_if);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map STIF onstance to NIF port
 *
 * see .h file
 */
int
imb_cdu_internal_port_to_stif_instance_map(
    int unit,
    int core,
    int sif_instance_id,
    int inner_cdu_id,
    int first_lane_in_cdu)
{

    uint32 entry_handle_id;
    uint32 bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_STIF_INSTANCE_MAP, &entry_handle_id));
    /** set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE_ID, sif_instance_id);
    /** request for value field */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&bitmap, inner_cdu_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_CDU_ID_BITMAP, INST_SINGLE, bitmap);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, INST_SINGLE, first_lane_in_cdu);
    /** commit value */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
int
imb_cdu_internal_prd_bypass_set(
    int unit,
    int core,
    int inner_cdu_id,
    uint32 is_bypass)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set Core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set CDU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    /*
     *  Set Bypass mode
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_BYPASS, INST_SINGLE, is_bypass);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * see .h file
 */
int
imb_cdu_internal_prd_bypass_get(
    int unit,
    int core,
    int inner_cdu_id,
    uint32 *is_bypass)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set Core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set CDU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    /*
     *  Get Bypass mode
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRD_BYPASS, INST_SINGLE, is_bypass);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

int
imb_cdu_internal_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 priority,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Use LLFC threshold to produce PFC  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_GEN_PFC_FROM_LLFC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_A_ENABLE, INST_SINGLE,
                                 enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_B_ENABLE, INST_SINGLE,
                                 enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_C_ENABLE, INST_SINGLE,
                                 enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_D_ENABLE, INST_SINGLE,
                                 enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Use PFC threshold to produce PFC (same configuration as LLFC) */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FC_CDU_GEN_PFC_FROM_RX_FIFOS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 priority,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    /** Check the table used to enable LLFC threshold to produce PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_GEN_PFC_FROM_LLFC, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    uint32 lane_in_cdu,
    int rmc,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to enable the RMC of the relevant lane to generate LLFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_GEN_LLFC_FROM_RX_FIFOS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_in_cdu);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    uint32 lane_in_cdu,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to stop transmission according LLFC signal received from CFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_REC_LLFC_STOP_PM_FROM_CFC, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_in_cdu);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 flags,
    uint32 threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the handle to the relevant table and set the key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_RX_QMLF_THRESHOLD, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC)
    {
        /*
         * Set RX PORT FIFO Link Level Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }

        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC)
    {
        /*
         * Set RX PORT FIFO Priority Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }
        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_core_t core,
    int inner_cdu_id,
    int rmc,
    uint32 flags,
    uint32 *threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the handle to the relevant table and set the key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_RX_QMLF_THRESHOLD, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, inner_cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC)
    {
        /*
         * Set RX PORT FIFO Link Level Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }

        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC)
    {
        /*
         * Set RX PORT FIFO Priority Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }
        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_fc_reset_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    uint32 in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to put in/out of reset all CDUs on both cores */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_RESET, INST_SINGLE, in_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_llfc_from_glb_rsc_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int lane,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to enable/disable the generation of LLFC based on a signal for Global resources for all CDUs */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_GEN_LLFC_FROM_GLB_RCS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADROOM_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_pfc_rec_priority_map(
    int unit,
    bcm_core_t core,
    int cdu_id,
    uint32 nif_priority,
    uint32 egq_priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to map the NIF priority to the EGQ priority for the specified CDU */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_REC_PFC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CFC_PRIO, egq_priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIO, nif_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_A_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_B_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_cdu_internal_pfc_rec_priority_unmap(
    int unit,
    bcm_core_t core,
    int cdu_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to unmap all priorities for the specified CDU */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_REC_PFC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CFC_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_A_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_B_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
