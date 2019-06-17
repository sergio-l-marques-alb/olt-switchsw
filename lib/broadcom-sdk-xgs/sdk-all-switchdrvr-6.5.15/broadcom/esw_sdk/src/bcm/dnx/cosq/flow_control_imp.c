/** \file src/bcm/dnx/cosq/flow_control_imp.c
 * $Id$
 *
 * Flow control callback functions for DNX.\n
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/dbal/dbal.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnx/swstate/auto_generated/access/flow_control_access.h>
#include <soc/portmod/portmod.h>
#include <bcm_int/dnx/cosq/flow_control.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnx/legacy/TMC/tmc_api_flow_control.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq.h>

#include "flow_control_imp.h"
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

/*
 * Defines
 * {
 */

/**
 * \brief - Number of STE VSQs per core. From VSQs B only categories 2 and 3 are used to generate Flow Control
 */
#define NOF_STE_VSQS_PER_CORE (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS)->nof + \
                              (dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY_CNCTN_CLS)->nof/2) + \
                               dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY)->nof)

/** 
 * \brief - Choose the approprriate calendar table according to the input Congestion Gport subtype
 */
#define GPORT_CAL_TABLE_GET(gport) (BCM_GPORT_IS_CONGESTION_COE(gport) ? DBAL_TABLE_FC_REC_COE_CAL_MAP : DBAL_TABLE_FC_REC_OOB_CAL_MAP);

/*
 * }
 */

/*
 * Functions
 * {
 */

/**
 * See .h file
 */
shr_error_e
dnx_fc_cfc_global_enable_set(
    int unit,
    uint8 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set CFC Global Enabler for flow-control */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CFC_ENABLERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CFC_EN, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_cfc_global_enable_get(
    int unit,
    uint8 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get CFC Global Enabler for flow-control */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CFC_ENABLERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_CFC_EN, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_pfc_refresh_set(
    int unit,
    bcm_port_t port,
    int value)
{
    portmod_pfc_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &control));
    if (value == 0)
    {                /** disable PFC refresh */
        control.refresh_timer = -1;
    }
    else
    {
        control.refresh_timer = value;
    }
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_set(unit, port, &control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_pfc_refresh_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    portmod_pfc_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &control));
    if (control.refresh_timer == -1)
    {                                 /** PFC refresh is disabled*/
        *value = 0;
    }
    else
    {
        *value = control.refresh_timer;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_inband_mode_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_fc_direction_type_t fc_direction,
    uint32 *llfc_enabled,
    uint32 *pfc_enabled)
{
    portmod_pfc_control_t pfc_control;
    portmod_pause_control_t pause_control;
    SHR_FUNC_INIT_VARS(unit);

        /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &pfc_control));
    SHR_IF_ERR_EXIT(portmod_port_pause_control_get(unit, port, &pause_control));
    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        *pfc_enabled = pfc_control.tx_enable;
        *llfc_enabled = pause_control.tx_enable;
    }
    else
    {
        *pfc_enabled = pfc_control.rx_enable;
        *llfc_enabled = pause_control.rx_enable;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_inband_mode_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_fc_direction_type_t fc_direction,
    uint32 llfc_enabled,
    uint32 pfc_enabled)
{
    portmod_pfc_control_t pfc_control;
    portmod_pause_control_t pause_control;
    SHR_FUNC_INIT_VARS(unit);

    if (pfc_enabled == 1 && llfc_enabled == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode can not be both PFC and LLFC");
    }

    /** Enable / Disable - LL */
    SHR_IF_ERR_EXIT(portmod_port_pause_control_get(unit, port, &pause_control));
    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        pause_control.tx_enable = llfc_enabled;
    }
    else
    {
        pause_control.rx_enable = llfc_enabled;
    }
    SHR_IF_ERR_EXIT(portmod_port_pause_control_set(unit, port, &pause_control));

    /** Enable / Disable - PFC*/
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &pfc_control));
    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        pfc_control.tx_enable = pfc_enabled;
    }
    else
    {
        pfc_control.rx_enable = pfc_enabled;
    }
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_set(unit, port, &pfc_control));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_port_mac_sa_set(
    int unit,
    int port,
    bcm_mac_t mac_sa)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_tx_mac_sa_set(unit, port, mac_sa));
    SHR_IF_ERR_EXIT(portmod_port_rx_mac_sa_set(unit, port, mac_sa));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_port_mac_sa_get(
    int unit,
    int port,
    bcm_mac_t mac_sa)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_rx_mac_sa_get(unit, port, mac_sa));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to set the PFC Generic Bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] core - core ID
 *   \param [in] is_delete - indicates if we want to add or delete path
 *   \param [in] cosq_min - start COSQ from which to start setting the bits in the generic bitmap
 *   \param [in] cosq_max - end COSQ up to which to set the bits in the generic bitmap
 *   \param [in] bitmap_index_old - index of one of the 32 PFC Generic Bitmaps that was previously used
 *   \param [in] bitmap_index_new - index of one of the 32 PFC Generic Bitmaps that will be used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_pfc_generic_bmp_hw_set(
    int unit,
    bcm_core_t core,
    int is_delete,
    int cosq_min,
    int cosq_max,
    int bitmap_index_old,
    int bitmap_index_new)
{
    uint32 entry_handle_id;
    uint32 qpair, enabled = 0;
    bcm_core_t core_iter;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** If a new bitmap index was allocated, iterate over the generic bitmap specified with
     *  bitmap_index_old to get the Qpairs with enabled FC then copy them to the bitmap specified with bitmap_index_new */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GENERIC_BITMAP, &entry_handle_id));
    if (bitmap_index_old != bitmap_index_new)
    {
        DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_iter)
        {
            for (qpair = 0; qpair < dnx_data_egr_queuing.params.nof_q_pairs_get(unit); qpair++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index_old);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (enabled)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index_new);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enabled);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
            }
        }
    }

    /** Set the appropriate bits for Q-pairs of the port in the generic bitmap */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GENERIC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index_new);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    for (qpair = cosq_min; qpair <= cosq_max; qpair++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GENERIC_BITMAP, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the targets from the PFC Generic Bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] bitmap_index - index of one of the 32 PFC Generic Bitmaps
 *   \param [in] core_in - core
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object array
 *   \param [out] target_count - updated size(valid entries) of target array
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_pfc_generic_bmp_targets_get(
    int unit,
    uint32 bitmap_index,
    bcm_core_t core_in,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 qpair, enabled;
    bcm_port_t port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Itterate over the generic bitmap specified with bitmap_index to get the Qpairs with enabled FC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GENERIC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index);
    DNXCMN_CORES_ITER(unit, core_in, core)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (qpair = 0; qpair < dnx_data_egr_queuing.params.nof_q_pairs_get(unit); qpair++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (enabled)
            {
                if (*target_count > target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                /** From the Qpairs with enabled FC get the associated Port+Cosq*/
                SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get
                                (unit, qpair, core, &port, &target[*target_count].cosq));
                BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                *target_count += 1;
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the cosq_nim, cosq_max and is_bitmap needed to set the PFC Generic Bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] target - holds information for the targeted reaction point
 *   \param [in] is_cal - indicate if the reception is from Calendar(OOB, COE) or PFC
 *   \param [out] cosq_min - start COSQ from which to start setting the bits in the generic bitmap
 *   \param [out] cosq_max - end COSQ up to which to set the bits in the generic bitmap
 *   \param [out] is_bitmap - indicates if bitmap should be used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_rec_is_bmp_get(
    int unit,
    bcm_cosq_fc_endpoint_t * target,
    uint8 is_cal,
    int *cosq_min,
    int *cosq_max,
    uint8 *is_bitmap)
{
    bcm_port_t port = 0;
    int base_q_pair;
    int num_priorities;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    port = BCM_GPORT_LOCAL_GET(target->port);

    /** Get the priorities and base Q-pair*/
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

    if (target->cosq >= num_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
    }

    /** When this flag is specified we always use PFC Generic Bitmap */
    if (target->flags & BCM_COSQ_FC_PORT_OVER_PFC)
    {
        if (target->cosq == -1)
        {
                        /**  Reaction point is Port */
            *cosq_min = base_q_pair;
            *cosq_max = base_q_pair + (num_priorities - 1);
        }
        else
        {
            /**  Reaction point is Port+COSQ using PFC Generic Bitmap */
            /** Inheritance is enabled */
            if (target->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
            {
                /** Determine which Q-pairs need to be stopped according to the Inheritance flag */
                if (target->flags & BCM_COSQ_FC_INHERIT_UP)
                {
                    *cosq_min = base_q_pair + target->cosq;
                    *cosq_max = base_q_pair + (num_priorities - 1);
                }
                else if (target->flags & BCM_COSQ_FC_INHERIT_DOWN)
                {
                    *cosq_min = base_q_pair;
                    *cosq_max = base_q_pair + target->cosq;
                }
            }
            else
            {
                /** No Inheritance */
                *cosq_min = base_q_pair + target->cosq;
                *cosq_max = base_q_pair + target->cosq;
            }
        }

        *is_bitmap = TRUE;

    }
    else
    {
        if (target->cosq == -1)
        {
            if (is_cal)
            {
                                /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
                *is_bitmap = FALSE;
            }
            else
            {
                if (num_priorities == 1)
                {
                                        /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
                    *is_bitmap = FALSE;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid Flags, for Port reaction point with port priorities different than 1 need to set BCM_COSQ_FC_PORT_OVER_PFC.");
                }
            }
        }
        else
        {
                        /**  Reaction point is Port+COSQ with no BCM_COSQ_FC_PORT_OVER_PFC*/
                        /** Inheritance is enabled */
            if (target->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
            {
                                /** Determine which Q-pairs need to be stopped according to the Inheritance flag */
                if (target->flags & BCM_COSQ_FC_INHERIT_UP)
                {
                    *cosq_min = base_q_pair + target->cosq;
                    *cosq_max = base_q_pair + (num_priorities - 1);
                }
                else if (target->flags & BCM_COSQ_FC_INHERIT_DOWN)
                {
                    *cosq_min = base_q_pair;
                    *cosq_max = base_q_pair + target->cosq;
                }

                *is_bitmap = TRUE;

            }
            else
            {
                                /** No Inheritance */
                *is_bitmap = FALSE;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to check if bitmap is already allocated and return its index.
 *  If it is not allocated, allocate a new one
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] source - holds information for the source of the FC
 *   \param [in] target - holds information for the target of the FC
 *   \param [in] is_cal - indicate if the reception is from Calendar(OOB, COE) or PFC
 *   \param [out] bitmap_index_old - index of one of the 32 PFC Generic Bitmaps that was previously used
 *   \param [out] bitmap_index_new - index of one of the 32 PFC Generic Bitmaps that will be used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_rec_bmp_index_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target,
    uint8 is_cal,
    int *bitmap_index_old,
    int *bitmap_index_new)
{
    dnx_pfc_generic_bmp_t new_data;
    dnx_pfc_generic_bmp_t old_data;
    int qpair;
    int base_q_pair;
    int num_priorities;
    int ref_count;
    uint32 entry_handle_id;
    bcm_port_t src_port = 0;
    bcm_port_t tar_port = 0;
    bcm_port_t port_i = 0;
    bcm_port_t master_port;
    int nif_port;
    int cal_if = 0;
    bcm_core_t core;
    uint32 map_sel;
    uint32 map_val;
    uint8 is_first, is_last;
    uint8 is_bitmap;
    int channelized;
    int cosq_min = 0;
    int cosq_max = 0;
    bcm_pbmp_t channels;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Prepare the bitmap variable */
    sal_memset(&new_data, 0, sizeof(dnx_pfc_generic_bmp_t));

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source port */
    if (BCM_GPORT_IS_LOCAL(source->port) || BCM_GPORT_IS_LOCAL_INTERFACE(source->port))
    {
        /** Return the local port and then find the NIF port mapped to it */
        src_port =
            BCM_GPORT_IS_LOCAL(source->
                               port) ? BCM_GPORT_LOCAL_GET(source->port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
        nif_port = nif_port - core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;
    }
    else
    {
        /** In case of calendar determine the calendar interface */
        cal_if = BCM_GPORT_CONGESTION_GET(source->port);
        /** Choose the approprriate calendar table according to the input Gport subtype */
        cal_table = GPORT_CAL_TABLE_GET(source->port);
        /** Core is necessary for the allocation of the resource manager. */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, tar_port, &core));
    }

    if (is_cal)
    {
        /** Check if bitmap is already selected for this port */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, &map_sel);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, cal_table, entry_handle_id));
    }
    else
    {
        /** Check if bitmap is already selected for this port in the PFC table*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_REC_PFC_MAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, source->cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_SEL, INST_SINGLE, &map_sel);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_PFC_MAP, entry_handle_id));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, tar_port, &channelized));
    /** Interface is channelized */
    if (channelized && is_cal)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, tar_port, 0, &channels));
        /** For each channel set the appropriate bits in the FC generic bitmap */
        _SHR_PBMP_ITER(channels, port_i)
        {
            /** Get the base Q-pair and priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &num_priorities));

            cosq_min = base_q_pair;
            cosq_max = base_q_pair + (num_priorities - 1);

            /** Create the PFC Generic Bitmap */
            for (qpair = cosq_min; qpair <= cosq_max; qpair++)
            {
                SHR_BITSET(&new_data.qpair_bits[core][0], qpair);
            }
        }
    }
    else
    {
        /** Get the relevant cosq_min and cosq_max */
        SHR_IF_ERR_EXIT(dnx_fc_rec_is_bmp_get(unit, target, is_cal, &cosq_min, &cosq_max, &is_bitmap));
        /** Create the PFC Generic Bitmap */
        for (qpair = cosq_min; qpair <= cosq_max; qpair++)
        {
            SHR_BITSET(&new_data.qpair_bits[core][0], qpair);
        }
    }

    if ((map_sel == DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP) || (map_sel == DNX_RX_CAL_DEST_PFC_GENERIC_BMP))
    {
        *bitmap_index_old = map_val;
        /** Get the old profile data */
        SHR_IF_ERR_EXIT(flow_control_db.
                        fc_generic_bmp.profile_data_get(unit, _SHR_CORE_ALL, *bitmap_index_old, &ref_count, &old_data));

        /** Combine the old profile data and the new data*/
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_or
                            (&new_data.qpair_bits[core][0], &old_data.qpair_bits[core][0],
                             (DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS / (sizeof(uint32) * 8))));
        }

        /** Exchange the existing profile with a new one*/
        SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.exchange(unit, _SHR_CORE_ALL, 0, &new_data, (int) map_val, NULL,
                                                                bitmap_index_new, &is_first, &is_last));
    }
    else
    {
        /** Exchange the default profile with a new one*/
        SHR_IF_ERR_EXIT(flow_control_db.
                        fc_generic_bmp.exchange(unit, _SHR_CORE_ALL, 0, &new_data, bitmap_index_default, NULL,
                                                bitmap_index_new, &is_first, &is_last));
        *bitmap_index_old = *bitmap_index_new;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the VSQ attributes, the source select and FC index in context of Calendar(OOB, COE), when provided a VSQ Gport.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] vsq_gport - VSQ gport, whose attributes we want to get
 *   \param [out] src_sel - source select for the Calendar(OOB, COE)
 *   \param [out] fc_index - flow control index for the Calendar(OOB, COE)
 *   \param [out] traffic_class - traffic class
 *   \param [out] connection_class - connection class
 *   \param [out] category - category
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_vsq_gport_to_fc_index(
    int unit,
    bcm_gport_t vsq_gport,
    uint8 *src_sel,
    uint32 *fc_index,
    int *traffic_class,
    int *connection_class,
    int *category)
{
    bcm_core_t core;
    uint32 vsq_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the VSQ type from the port */
    if (BCM_COSQ_GPORT_IS_VSQ_PP(vsq_gport))
    {
        *src_sel = DNX_OOB_TX_CAL_SRC_VSQ_D;
        *fc_index = BCM_COSQ_GPORT_VSQ_PP_GET(vsq_gport);
    }
    else
    {
        *src_sel = DNX_OOB_TX_CAL_SRC_VSQ_A_B_C;

        /** Determine the traffic class, connection class, core and category */
        *traffic_class = BCM_COSQ_GPORT_VSQ_TC_GET(vsq_gport);
        core = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(vsq_gport);
        *category = BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport);
        *connection_class = BCM_COSQ_GPORT_VSQ_CC_GET(vsq_gport);

        /** Calculate the VSQ index */
        if (*connection_class != -1)
        {
            /** VSQ C */
            if (((*category) != 2) && ((*category) != 3))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only Categories 2 and 3 of VSQs C can generate Flow Control");
            }
            vsq_index = (*category - 2) * (*connection_class) + *connection_class;
            *fc_index =
                NOF_STE_VSQS_PER_CORE * core + vsq_index + dnx_data_ingr_congestion.vsq.info_get(unit,
                                                                                                 SOC_DNX_ITM_VSQ_GROUP_CTGRY)->nof
                + dnx_data_ingr_congestion.vsq.info_get(unit, SOC_DNX_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS)->nof;
        }
        else if ((*traffic_class) != -1)
        {
            /** VSQ B */
            vsq_index = (*category) * (*traffic_class) + (*traffic_class);
            *fc_index =
                NOF_STE_VSQS_PER_CORE * core + vsq_index + dnx_data_ingr_congestion.vsq.info_get(unit,
                                                                                                 SOC_DNX_ITM_VSQ_GROUP_CTGRY)->nof;
        }
        else
        {
            /** VSQ A */
            vsq_index = *category;
            *fc_index = NOF_STE_VSQS_PER_CORE * core + vsq_index;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to set the CMIC to EGQ mapping, enable CFC to react to CMIC FC
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] port - CPU port which is the source of the FC
 *   \param [in] is_delete - indicates if we want to add or delete path
 *   \param [in] cosq_min - start COSQ from which to start setting the bits in the mask
 *   \param [in] cosq_max - end COSQ up to which to set the bits in the mask
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_cmic_egq_mask_set(
    int unit,
    bcm_port_t port,
    int is_delete,
    int cosq_min,
    int cosq_max)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    int cosq = 0;
    int channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

    /** Set the mask of generated CMIC vector */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MASK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    for (cosq = cosq_min; cosq <= cosq_max; cosq++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, channel + cosq);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !is_delete);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_llfc_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t tar_port;
    bcm_port_t src_port;
    int cosq_min = 0;
    int cosq_max = 0;
    int num_priorities;
    dnx_algo_port_type_e port_type = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source port */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** When configuring reception from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if (src_port != tar_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configured reception from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

    /** If port is CPU we need to set the CMIC to EGQ mapping, enable CFC to react to CMIC FC */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, src_port, &port_type));
    if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
    {
        /** Map all Q-pairs of the port to receive FC from CPU */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, tar_port, &num_priorities));
        cosq_min = 0;
        cosq_max = num_priorities - 1;

        /** Set the CMIC to EGQ mask */
        SHR_IF_ERR_EXIT(dnx_fc_cmic_egq_mask_set(unit, src_port, is_delete, cosq_min, cosq_max));
    }
    else
    {
        /** Check which mode is enabled */
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, tar_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
        if (pfc_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
        }
        llfc_enabled = (is_delete ? FALSE : TRUE);
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlReception, llfc_enabled, FALSE));

        /** Call IMB API to stop transmission on all lanes of the port according LLFC signal received from CFC */
        SHR_IF_ERR_EXIT(imb_port_stop_pm_from_cfc_llfc_enable_set(unit, tar_port, !(is_delete)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_pfc_port_cosq_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t tar_port = 0;
    bcm_port_t src_port = 0;
    bcm_core_t core;
    int base_q_pair;
    uint32 map_sel;
    uint32 map_val;
    int bitmap_index_new = 0;
    int bitmap_index_old = 0;
    int cosq_min = 0;
    int cosq_max = 0;
    int num_priorities;
    int nif_port;
    int master_port;
    int first_affected_nif_port;
    uint8 is_bitmap = 0;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    dnx_algo_port_type_e port_type = 0;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_first;
    dnx_pfc_generic_bmp_t default_data;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    tar_port = BCM_GPORT_LOCAL_GET(target->port);

    /** Determine the source port */
    /** Return the local port and then find the NIF port mapped to it */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** When configuring reception from PFC, port and cosq parameters must match or flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if ((src_port != tar_port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid Flags, need to specify BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT, or ports for source and destination need to be the same");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, tar_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, tar_port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));

    /** If port is CPU we need to set the CMIC to EGQ mapping, enable CFC to react to CMIC FC */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, src_port, &port_type));
    if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
    {
        if (source->cosq >= num_priorities)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, source port has only %d priorities.", num_priorities);
        }
        /** Target is Port+COSQ */
        /** Inheritance is enabled */
        if (target->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
        {
            /** Determine which Q-pairs need to be stopped according to the Inheritance flag */
            if (target->flags & BCM_COSQ_FC_INHERIT_UP)
            {
                cosq_min = source->cosq;
                cosq_max = num_priorities - 1;
            }
            else if (target->flags & BCM_COSQ_FC_INHERIT_DOWN)
            {
                cosq_min = 0;
                cosq_max = source->cosq;
            }
        }
        else
        {
            /** No Inheritance */
            cosq_min = cosq_max = source->cosq;
        }

        /** Set the CMIC to EGQ mask */
        SHR_IF_ERR_EXIT(dnx_fc_cmic_egq_mask_set(unit, src_port, is_delete, cosq_min, cosq_max));
    }
    else
    {
         /** Check which mode is enabled */
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, tar_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
        if (llfc_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
        }
        pfc_enabled = (is_delete ? FALSE : TRUE);
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlReception, FALSE, pfc_enabled));

        /** Check if Generic PFC Bitmap needs to be used and get the relevant cosq_min and cosq_max */
        SHR_IF_ERR_EXIT(dnx_fc_rec_is_bmp_get(unit, target, FALSE, &cosq_min, &cosq_max, &is_bitmap));

        if (is_bitmap)
        {
            /** Get the PFC Generic Bitmap index */
            SHR_IF_ERR_EXIT(dnx_fc_rec_bmp_index_get
                            (unit, source, target, FALSE, &bitmap_index_old, &bitmap_index_new));

                        /** If the entry is for delete, free the bitmap index in the resource manager */
            if (is_delete)
            {
                sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));
                SHR_IF_ERR_EXIT(flow_control_db.
                                fc_generic_bmp.exchange(unit, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                        &default_data, bitmap_index_new, NULL, &bitmap_index_default,
                                                        &tmp_is_first, &tmp_is_last));
            }

                        /** If called from Add API set the BMP, if called from Delete API only delete if it is the last reference in the template manager */
            if ((!is_delete) || (is_delete && tmp_is_last))
            {
                                /** Set the PFC Generic Bitmap  */
                SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_hw_set
                                (unit, core, is_delete, cosq_min, cosq_max, bitmap_index_old, bitmap_index_new));
            }

            /** Set the values to be written in the FC_REC_PFC_MAP */
            map_val = (is_delete ? 0 : bitmap_index_new);
            map_sel = (is_delete ? 0 : DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP);
        }
        else
        {
            if (target->cosq == -1)
            {
                /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
                if (num_priorities == 1)
                {
                    /** Set the values to be written in the FC_REC_PFC_MAP */
                    map_val = base_q_pair;
                    map_sel = (is_delete ? 0 : DNX_REC_PFC_MAP_SEL_PFC);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid Flags, for Port reaction point with port priorities different than 1 need to set BCM_COSQ_FC_PORT_OVER_PFC.");
                }
            }
            else
            {
                if (target->cosq < num_priorities)
                {
                    /**  Reaction point is Port+COSQ, no inheritance */
                    map_val = (is_delete ? 0 : (base_q_pair + target->cosq));
                    map_sel = (is_delete ? 0 : DNX_REC_PFC_MAP_SEL_PFC);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
                }
            }
        }

        /** Set FC_REC_PFC_MAP with the appropriate values */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
        /** Calculate the NIF port (per core) */
        nif_port = nif_port - core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;

        /** Allocate handle to the table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_REC_PFC_MAP, &entry_handle_id));

        if (dnx_data_fc.inband.pfc_rec_cdu_shift_get(unit))
        {
            /** The last two CDUs of each core have one bit shift left so the bitmap configuration is changed.
              * The implications here are that the mapping at CFC also needs to be shifted to correspond to the mapping of 
              * the CDU bitmaps set on init. To do that, for the last two CDUs of each core when we want to map priority 0 
              * we need to set it to the next nif_port. For example if we want to set prio 0 on NIF 36, we need to set the 
              * field corresponding to NIF 37. For the other priorities the mapping is standard */

            /** Calculate the first NIF affected by the bug */
            first_affected_nif_port =
                DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE - (2 * dnx_data_nif.eth.cdu_lanes_nof_get(unit));
            if ((nif_port >= first_affected_nif_port) && (source->cosq == 0))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT,
                                           ((nif_port ==
                                             (DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE -
                                              1)) ? first_affected_nif_port : (nif_port + 1)));
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
            }
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, source->cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_SEL, INST_SINGLE, map_sel);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, map_val);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VALID, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t port_i;
    bcm_core_t core;
    int base_q_pair;
    uint32 dest_sel;
    uint32 map_val;
    int bitmap_index_new;
    int bitmap_index_old;
    int cal_if;
    int cosq_min = 0;
    int cosq_max = 0;
    bcm_pbmp_t channels;
    int num_priorities;
    bcm_port_t master_port;
    int nif_port;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_first;
    dnx_pfc_generic_bmp_t default_data;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    dnx_algo_port_type_e port_type = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set the variable for the new bitmap to 0 */
    sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));

    /** Determine the source Calendar(OOB, COE) interface */
    cal_if = BCM_GPORT_CONGESTION_GET(source->port);

    /** Return the port associated with that interface */
    port = BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Get the core */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    /** Interface is not Ethernet port use Generic bitmap type */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE))
    {
        /** Get the PFC Generic Bitmap index */
        SHR_IF_ERR_EXIT(dnx_fc_rec_bmp_index_get(unit, source, target, TRUE, &bitmap_index_old, &bitmap_index_new));

        /** If the entry is for delete, free the allocated bitmap index from the resource manager */
        if (is_delete)
        {
            SHR_IF_ERR_EXIT(flow_control_db.
                            fc_generic_bmp.exchange(unit, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                    &default_data, bitmap_index_new, NULL, &bitmap_index_default,
                                                    &tmp_is_first, &tmp_is_last));
        }

        /** Set the values to be written in the Calendar(OOB, COE) */
        map_val = (is_delete ? 0 : bitmap_index_new);
        dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC_GENERIC_BMP);

        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &channels));
        /** For each channel set the appropriate bits in the FC generic bitmap */
        _SHR_PBMP_ITER(channels, port_i)
        {
            /** Get the base Q-pair and priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &num_priorities));

            cosq_min = base_q_pair;
            cosq_max = base_q_pair + (num_priorities - 1);

            /** If called from Add API set the BMP, if called from Delete API only delete if it is the last erference in the template manager */
            if ((!is_delete) || (is_delete && tmp_is_last))
            {
                /** Set the PFC Generic Bitmap  */
                SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_hw_set
                                (unit, core, is_delete, cosq_min, cosq_max, bitmap_index_old, bitmap_index_new));
            }
        }
    }
    else
    {
        /** Get the NIF interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

        /** Call IMB API to stop transmission on all lanes of the port according LLFC signal received from CFC */
        SHR_IF_ERR_EXIT(imb_port_stop_pm_from_cfc_llfc_enable_set(unit, master_port, !(is_delete)));

        /** Set the values to be written in the Calendar(OOB, COE) */
        map_val = (is_delete ? 0 : nif_port);
        dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_NIF);
    }

    /** Choose the approprriate calendar table according to the input Gport subtype */
    cal_table = GPORT_CAL_TABLE_GET(source->port);

    /** Set the Calendar(OOB, COE) calendar mapping with the appropriate destination and value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, map_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, dest_sel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_port_cosq_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_core_t core;
    int base_q_pair;
    uint32 dest_sel;
    uint32 map_val;
    int cal_if;
    int bitmap_index_new;
    int bitmap_index_old;
    int cosq_min = 0;
    int cosq_max = 0;
    int num_priorities;
    uint8 is_bitmap = 0;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_first;
    dnx_pfc_generic_bmp_t default_data;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source Calendar(OOB, COE) interface */
    cal_if = BCM_GPORT_CONGESTION_GET(source->port);

    /** Determine the target port */
    port = BCM_GPORT_LOCAL_GET(target->port);

    /** Get the base Q-pair, core and priorities */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

    /** Check if Generic PFC Bitmap needs to be used and get the relevant cosq_min and cosq_max */
    SHR_IF_ERR_EXIT(dnx_fc_rec_is_bmp_get(unit, target, TRUE, &cosq_min, &cosq_max, &is_bitmap));

    if (is_bitmap)
    {
        /** Get the PFC Generic Bitmap index */
        SHR_IF_ERR_EXIT(dnx_fc_rec_bmp_index_get(unit, source, target, TRUE, &bitmap_index_old, &bitmap_index_new));

        /** If the entry is for delete, free the bitmap index in the resource manager */
        if (is_delete)
        {
            sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));
            SHR_IF_ERR_EXIT(flow_control_db.
                            fc_generic_bmp.exchange(unit, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                    &default_data, bitmap_index_new, NULL, &bitmap_index_default,
                                                    &tmp_is_first, &tmp_is_last));
        }

        /** If called from Add API set the BMP, if called from Delete API only delete if it is the last erference in the template manager */
        if ((!is_delete) || (is_delete && tmp_is_last))
        {
            /** Set the PFC Generic Bitmap  */
            SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_hw_set
                            (unit, core, is_delete, cosq_min, cosq_max, bitmap_index_old, bitmap_index_new));
        }

        /** Set the values to be written in the Calendar(OOB, COE) */
        map_val = (is_delete ? 0 : bitmap_index_new);
        dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC_GENERIC_BMP);
    }
    else
    {
        if (target->cosq == -1)
        {
            /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
            /** Select destination according to the port priorities */
            switch (num_priorities)
            {
                case 1:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC);
                    break;
                case 2:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PORT_2_PRIO);
                    break;
                case 4:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PORT_4_PRIO);
                    break;
                case 8:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PORT_8_PRIO);
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid number of port priorities");
            }
            map_val = (is_delete ? 0 : (base_q_pair + dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * core));
        }
        else
        {
            if (target->cosq < num_priorities)
            {
                /**  Reaction point is Port+COSQ, no inheritance */
                /** Set the values to be written in the calendar DBAL table */
                map_val =
                    (is_delete ? 0
                     : (base_q_pair + dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * core + target->cosq));
                dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
            }
        }
    }

    /** Choose the approprriate calendar table according to the input Gport subtype */
    cal_table = GPORT_CAL_TABLE_GET(source->port);

    /** Set the Calendar(OOB, COE) calendar mapping with the appropriate source and value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, map_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, dest_sel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_h_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    bcm_core_t core;
    int base_q_pair;
    int if_id;
    int nif_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    dbal_enum_value_field_glb_rcs_e resourse;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));
    nif_port = nif_port - core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;

    /** Determine the Global resourse */
    if (source->flags & BCM_COSQ_FC_BDB)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM0;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM1;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM;
        }
    }
    else if (source->flags & BCM_COSQ_FC_IS_OCB_ONLY)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM0;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM1;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM;
        }
    }
    else if (source->flags & BCM_COSQ_FC_HEADROOM)
    {
        resourse = DBAL_ENUM_FVAL_GLB_RCS_HDRM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Flags, need to specify global resource");
    }

    /** Check which mode is enabled and set inband mode to LLFC*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
    }

    /** Set the CFC to enable the generation from global resource to LLFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_LLFC_EN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS, resourse);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_h_l_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    bcm_core_t core;
    int base_q_pair;
    int if_id;
    int nif_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 glb_res_prio;
    int num_priorities;
    dbal_enum_value_field_glb_rcs_e resourse;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    nif_port = nif_port - core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;

    /** Determine the Global resource */
    if (source->flags & BCM_COSQ_FC_BDB)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM0;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM1;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM;
        }
    }
    else if (source->flags & BCM_COSQ_FC_IS_OCB_ONLY)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM0;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM1;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM;
        }
    }
    else if (source->flags & BCM_COSQ_FC_HEADROOM)
    {
        resourse = DBAL_ENUM_FVAL_GLB_RCS_HDRM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Flags, need to specify global resource");
    }

    /** Check which mode is enabled and set inband mode to PFC*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }

    /** To keep backward compatibility, because in legacy devices 0 represented high priority and 1 represented low priority */
    glb_res_prio = ((source->priority == 0) || (source->priority == BCM_COSQ_HIGH_PRIORITY)) ? 1 : 0;
    if (resourse == DBAL_ENUM_FVAL_GLB_RCS_HDRM)
    {
        /** Only high priority is supported for HDRM */
        glb_res_prio = 1;
    }

    /** Set the HW to enable the generation from global resource to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_PFC_EN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS, resourse);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, glb_res_prio);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, target->cosq);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_h_l_oob_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    int oob_if = 0;
    int fc_index;
    uint32 glb_res_prio;
    dbal_enum_value_field_glb_rcs_e resourse;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    oob_if = BCM_GPORT_CONGESTION_GET(target->port);

    /** To keep backward compatibility, because in legacy devices 0 represented high priority and 1 represented low priority */
    glb_res_prio = ((source->priority == 0) || (source->priority == BCM_COSQ_HIGH_PRIORITY)) ? 1 : 0;

    /** Determine the Global resourse */
    if (source->flags & BCM_COSQ_FC_BDB)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM0;
            fc_index = glb_res_prio ? DNX_OOB_GLB_RCS_IDX_DRAM_POOL_0_HIGH : DNX_OOB_GLB_RCS_IDX_DRAM_POOL_0_LOW;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM1;
            fc_index = glb_res_prio ? DNX_OOB_GLB_RCS_IDX_DRAM_POOL_1_HIGH : DNX_OOB_GLB_RCS_IDX_DRAM_POOL_1_LOW;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM;
            fc_index = glb_res_prio ? DNX_OOB_GLB_RCS_IDX_DRAM_HIGH : DNX_OOB_GLB_RCS_IDX_DRAM_LOW;
        }
    }
    else if (source->flags & BCM_COSQ_FC_IS_OCB_ONLY)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM0;
            fc_index = glb_res_prio ? DNX_OOB_GLB_RCS_IDX_SRAM_POOL_0_HIGH : DNX_OOB_GLB_RCS_IDX_SRAM_POOL_0_LOW;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM1;
            fc_index = glb_res_prio ? DNX_OOB_GLB_RCS_IDX_SRAM_POOL_1_HIGH : DNX_OOB_GLB_RCS_IDX_SRAM_POOL_1_LOW;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM;
            fc_index = glb_res_prio ? DNX_OOB_GLB_RCS_IDX_SRAM_HIGH : DNX_OOB_GLB_RCS_IDX_SRAM_LOW;
        }
    }
    else if (source->flags & BCM_COSQ_FC_HEADROOM)
    {
        resourse = DBAL_ENUM_FVAL_GLB_RCS_HDRM;
        fc_index = DNX_OOB_GLB_RCS_IDX_HDRM;
        /** Only high priority is supported for HDRM */
        glb_res_prio = 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Flags, need to specify global resourse");
    }

    /** Set the HW to enable the generation from global resource to OOB calendar */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS, resourse);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, glb_res_prio);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, entry_handle_id));

    /** Set also mapping of the OOB calender to the appropriate source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, target->calender_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : fc_index));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_OOB_TX_CAL_SRC_CONST : DNX_OOB_TX_CAL_SRC_GLB_RSC));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t tar_port;
    bcm_port_t src_port;
    bcm_cosq_vsq_info_t vsq_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LLFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if (src_port != tar_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, tar_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
    }
    llfc_enabled = (is_delete ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlGeneration, llfc_enabled, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_oob_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 oob_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    oob_if = BCM_GPORT_CONGESTION_GET(target->port);

    /** Set mapping of the OOB calendar to the LLFC VSQ source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX,
                                 INST_SINGLE, (is_delete ? 0 : BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(source->port)));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_OOB_TX_CAL_SRC_CONST : DNX_OOB_TX_CAL_SRC_LLFC_VSQ));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t tar_port;
    bcm_port_t src_port;
    bcm_cosq_vsq_info_t vsq_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if ((src_port != tar_port) || (target->cosq != source->cosq))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port and cosq parameter must match, or IMPLICIT flag should be used");
    }

    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, tar_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }
    pfc_enabled = (is_delete ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlGeneration, FALSE, pfc_enabled));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_oob_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 oob_if;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    oob_if = BCM_GPORT_CONGESTION_GET(target->port);

    /** Set mapping of the OOB calendar to the PFC VSQ source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_OOB_TX_CAL_SRC_CONST : DNX_OOB_TX_CAL_SRC_PFC_VSQ));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX,
                                 INST_SINGLE,
                                 (is_delete ? 0
                                  : (BCM_COSQ_GPORT_VSQ_PG_GET(source->port) * BCM_COS_COUNT + source->cosq)));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_c2_tc_vsq_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    uint32 cat2_prio, cat2_prio_min = 0, cat2_prio_max = 0;
    bcm_core_t core;
    int base_q_pair;
    int if_id, num_priorities;
    int nif_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    nif_port = nif_port - core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;

    /** Check which mode is enabled and set inband mode to PFC*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }

    /** Determine the Category 2 priority */
    cat2_prio = BCM_COSQ_GPORT_VSQ_TC_GET(source->port);

    /** No Inheritance */
    cat2_prio_min = cat2_prio;
    cat2_prio_max = cat2_prio;

    /** Inheritance */
    /** Determine which Category 2 priorities need to be stopped according to the Inheritance flag */
    if (source->flags & BCM_COSQ_FC_INHERIT_UP)
    {
        cat2_prio_max = (num_priorities - 1);
    }
    else if (source->flags & BCM_COSQ_FC_INHERIT_DOWN)
    {
        cat2_prio_min = 0;
    }

    /** Enable generation of Flow control from Category 2 and Traffic class to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_PFC_EN, &entry_handle_id));
    for (cat2_prio = cat2_prio_min; cat2_prio <= cat2_prio_max; cat2_prio++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAT2_PRIO, cat2_prio);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, target->cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_vsq_a_d_oob_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 entry_handle_id_cat2_ena;
    int oob_if = 0;
    uint8 src_sel;
    int traffic_class;
    int connection_class;
    uint32 fc_index;
    int category;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    oob_if = BCM_GPORT_CONGESTION_GET(target->port);

    /** Determine the VSQ properties, FC index and source select from the VSQ Gport */
    SHR_IF_ERR_EXIT(dnx_fc_vsq_gport_to_fc_index
                    (unit, source->port, &src_sel, &fc_index, &traffic_class, &connection_class, &category));

    if ((category == 2) && (traffic_class != -1))
    {
        /** Special case for Category 2 + TC - Enable generation from Cat2 TC to OOB calendar */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_OOB_EN, &entry_handle_id_cat2_ena));
        dbal_entry_key_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_OOB_IF, oob_if);
        dbal_entry_key_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_CAL_IDX, target->calender_index);
        dbal_entry_key_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_CAT2_PRIO, traffic_class);
        dbal_entry_value_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_ENABLE, INST_SINGLE,
                                     (is_delete ? 0 : 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_cat2_ena, DBAL_COMMIT));
    }

    /** Set mapping of the OOB calendar to the VSQ (A to D) source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_OOB_TX_CAL_SRC_CONST : src_sel));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : fc_index));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t port, src_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target interface */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source interface */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        port = src_port;
    }
    else if (src_port != port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }
    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
    }

    /** Call IMB API to map port's RMCs to generate LLFC */
    SHR_IF_ERR_EXIT(imb_port_gen_llfc_from_rmc_enable_set(unit, port, !(is_delete)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t port, src_port;
    int num_priorities;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target interface */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source interface */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

        /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        port = src_port;
    }
    else if (src_port != port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

        /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }

    /** Call IMB API to set HW to enable all RMCs of the port to produce PFC for the specified priority */
    SHR_IF_ERR_EXIT(imb_port_gen_pfc_from_rmc_enable_set(unit, port, target->cosq, !(is_delete)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_oob_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    int oob_if;
    int if_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source interface */
    port = BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** Determine the target OOB interface */
    oob_if = BCM_GPORT_CONGESTION_GET(target->port);

    /** Get the interface ID and the core */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));

    /** Set mapping of the OOB calendar to the Almost full NIF source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_OOB_TX_CAL_SRC_CONST : DNX_OOB_TX_CAL_SRC_NIF));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : if_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_llfc_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source calender_index */
    if (source->calender_index != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_pfc_port_cosq_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source calender_index */
    if (source->calender_index != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
    }

    /** Verify target flags */
    if (target->cosq != BCM_COS_INVALID)
    {
        if (target->flags
            & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN
                | BCM_COSQ_FC_PORT_OVER_PFC | BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }
    else
    {
        if (target->flags & ~(BCM_COSQ_FC_PORT_OVER_PFC | BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify source calender_index */
    if ((source->calender_index < 0) || (source->calender_index > 511))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_port_cosq_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

        /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify source calender_index */
    if ((source->calender_index < 0) || (source->calender_index > 511))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index");
    }

    /** Verify target flags */
    if (target->cosq != BCM_COS_INVALID)
    {
        if (target->flags & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN | BCM_COSQ_FC_PORT_OVER_PFC))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }
    else
    {
        if (target->flags & ~BCM_COSQ_FC_PORT_OVER_PFC)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_h_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags & ~(BCM_COSQ_FC_BDB | BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_IS_OCB_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < -1) || (source->cosq > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_h_l_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags & ~(BCM_COSQ_FC_BDB | BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_IS_OCB_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < -1) || (source->cosq > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_h_l_oob_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags & ~(BCM_COSQ_FC_BDB | BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_IS_OCB_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < -1) || (source->cosq > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

    /** Verify target COSQ */
    if (target->cosq != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target COSQ, must be -1");
    }

    /** Verify target calender_index */
    if ((target->calender_index < 0) || (target->calender_index > 511))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target calender_index");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_to_oob_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

    /** Verify target COSQ */
    if (target->cosq != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target COSQ, must be -1");
    }

    /** Verify target calender_index */
    if ((target->calender_index < 0) || (target->calender_index > 511))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target calender_index");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < BCM_COS_MIN) || (source->cosq > BCM_COS_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify target flags */
    if (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_oob_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < BCM_COS_MIN) || (source->cosq > BCM_COS_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

    /** Verify target COSQ */
    if (target->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target COSQ, must be -1");
    }

    /** Verify target calender_index */
    if ((target->calender_index < 0) || (target->calender_index > 511))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target calender_index");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_c2_tc_vsq_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if ((source->flags != 0) && (source->flags & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_llfc_pfc_react_point_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    bcm_port_t src_port = 0;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    bcm_core_t core;
    int if_id = 0;
    int channel;
    int nif_port;
    int first_affected_nif_port;
    dnx_algo_port_type_e port_type = 0;
    uint32 map_sel, map_val;
    uint32 valid, llfc_enabled, pfc_enabled;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source port */

    /** Return the local port and then find the NIF port mapped to it */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, src_port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
    }

    /** Get information for the ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, src_port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &channel));
    nif_port = nif_port - core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;

    if (source->cosq == -1)
    {
        /** If port is CPU we need to get the CIMC to EGQ mapping */
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
        {
            /** Get the mask of generated CMIC vector */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MASK, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /** We only need to check the first channel of the port to confirm it is mapped */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, channel);
            /** Getting value fields */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &llfc_enabled);
            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        }
        else
        {
            /** Get inband mode */
            SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                            (unit, src_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
        }

        if (llfc_enabled)
        {
            /** Source is LLFC, target is interface */
            target[*target_count].port = source->port;
            *target_count += 1;
        }
    }
    else
    {
        /** Source is PFC */

        /** If port is CPU we need to get the CIMC to EGQ mapping */
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
        {
            /** Get the mask of generated CMIC vector */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MASK, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /** We only need to check the first channel of the port to confirm it is mapped */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, channel + source->cosq);
            /** Getting value fields */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &pfc_enabled);
            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (pfc_enabled)
            {
                if (*target_count > target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                target[*target_count].port = source->port;
                target[*target_count].cosq = source->cosq;
                *target_count += 1;
            }
        }
        else
        {
            /** Get FC_REC_PFC_MAP from the entry corresponding to the source */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_REC_PFC_MAP, &entry_handle_id));
            if (dnx_data_fc.inband.pfc_rec_cdu_shift_get(unit))
            {
                /** The last two CDUs of each core have one bit shift left so the bitmap configuration is changed.
                  * The implications here are that the mapping at CFC also needs to be shifted to correspond to the mapping of 
                  * the CDU bitmaps set on init. To do that, for the last two CDUs of each core when we want to map priority 0 
                  * we need to set it to the next nif_port. For example if we want to set prio 0 on NIF 36, we need to set the 
                  * field corresponding to NIF 37. For the other priorities the mapping is standard */

                /** Calculate the first NIF affected by the bug */
                first_affected_nif_port =
                    DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE - (2 * dnx_data_nif.eth.cdu_lanes_nof_get(unit));
                if ((nif_port >= first_affected_nif_port) && (source->cosq == 0))
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT,
                                               ((nif_port ==
                                                 (DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE -
                                                  1)) ? first_affected_nif_port : (nif_port + 1)));
                }
                else
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
                }
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, source->cosq);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_SEL, INST_SINGLE, &map_sel);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VALID, INST_SINGLE, &valid);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (valid)
            {
                if (map_sel == DNX_REC_PFC_MAP_SEL_PFC)
                {
                    /** Reaction point is Port+Cosq, no inheritance */
                    if (*target_count > target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get
                                    (unit, map_val, core, &port, &target[*target_count].cosq));
                    BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                    *target_count += 1;
                }
                else if (map_sel == DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP)
                {
                    /** Reaction point is using Generic PFC bitmap, which means it is Port or Port+Cosq with inheritance */
                    SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_targets_get
                                    (unit, map_val, core, target_max, target, target_count));
                }
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_react_point_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint32 dest_sel, map_val;
    int cal_if;
    bcm_port_t port;
    bcm_core_t core;
    int q_pair;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source Calendar(OOB, COE) interface */
    cal_if = BCM_GPORT_CONGESTION_GET(source->port);

    /** Choose the approprriate calendar table according to the input Gport subtype */
    cal_table = GPORT_CAL_TABLE_GET(source->port);

    /** Set the Calendar(OOB, COE) calendar mapping with the appropriate destination and value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, &dest_sel);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (dest_sel == DNX_RX_CAL_DEST_PFC_GENERIC_BMP)
    {
        /*
         * Reaction point is Interface, Port+Cosq using a Generic Bitmap
         */
        SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_targets_get
                        (unit, map_val, _SHR_CORE_ALL, target_max, target, target_count));
    }
    else if ((dest_sel == DNX_RX_CAL_DEST_PFC) || (dest_sel == DNX_RX_CAL_DEST_PORT_2_PRIO)
             || (dest_sel == DNX_RX_CAL_DEST_PORT_4_PRIO) || (dest_sel == DNX_RX_CAL_DEST_PORT_8_PRIO))
    {
        /*
         * Reaction point is Port
         */
        if (*target_count > target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }

        core = (map_val >= dnx_data_egr_queuing.params.nof_q_pairs_get(unit)) ? 1 : 0;
        q_pair = map_val - (dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * core);
        SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get(unit, q_pair, core, &port, &target[*target_count].cosq));
        BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
        if (dest_sel != DNX_RX_CAL_DEST_PFC)
        {
            target[*target_count].cosq = -1;
        }
        *target_count += 1;
    }
    else if (dest_sel == DNX_RX_CAL_DEST_NIF)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, map_val, 0, 0, &port));
        if (*target_count > target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }
        BCM_GPORT_LOCAL_INTERFACE_SET(target[*target_count].port, port);
        target[*target_count].cosq = -1;
        *target_count += 1;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_fc_gen_glb_res_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id_llfc;
    uint32 entry_handle_id_pfc;
    uint32 entry_handle_id_oob_en;
    uint32 entry_handle_id_oob_map;
    bcm_port_t port = 0;
    bcm_core_t core;
    int if_id = 0;
    int nif_port = 0;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 enabled_llfc, enabled_pfc, enabled_oob;
    uint32 tar_prio = 0, cal_index, oob_if_cur = 0, src_sel;
    uint32 glb_res_prio;
    dbal_enum_value_field_glb_rcs_e resourse;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

        /** Determine the Global recourse */
    if (source->flags & BCM_COSQ_FC_BDB)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM0;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM1;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_DRAM;
        }
    }
    else if (source->flags & BCM_COSQ_FC_IS_OCB_ONLY)
    {
        if (source->cosq == 0)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM0;
        }
        else if (source->cosq == 1)
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM1;
        }
        else
        {
            resourse = DBAL_ENUM_FVAL_GLB_RCS_SRAM;
        }
    }
    else if (source->flags & BCM_COSQ_FC_HEADROOM)
    {
        resourse = DBAL_ENUM_FVAL_GLB_RCS_HDRM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Flags, need to specify global resource");
    }

    /** To keep backward compatibility, because in legacy devices 0 represented high priority and 1 represented low priority */
    glb_res_prio = ((source->priority == 0) || (source->priority == BCM_COSQ_HIGH_PRIORITY)) ? 1 : 0;
    if (resourse == DBAL_ENUM_FVAL_GLB_RCS_HDRM)
    {
        /** Only high priority is supported for HDRM */
        glb_res_prio = 1;
    }

    /** Allocate handles for global resource to OOB */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, &entry_handle_id_oob_en));
    dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_GLB_RCS, resourse);
    dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_GLB_PRIO, glb_res_prio);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id_oob_map));

    /** Allocate handle for global resource to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_PFC_EN, &entry_handle_id_pfc));
    dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_GLB_RCS, resourse);
    dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_GLB_PRIO, glb_res_prio);

     /** Allocate handle for global resource to LLFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_LLFC_EN, &entry_handle_id_llfc));
    dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_GLB_RCS, resourse);
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core)
    {
        for (if_id = 0; if_id < DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE; if_id++)
        {
            /** Get targets from global resource to LLFC */
            dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_NIF_PORT, if_id);
            dbal_value_field32_request(unit, entry_handle_id_llfc, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_llfc);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_llfc, DBAL_COMMIT));
            if (enabled_llfc)
            {
                if (*target_count > target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                nif_port = if_id + core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                (unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));

                target[*target_count].cosq = -1;
                BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                *target_count += 1;
            }

            /** Get targets from global resource to PFC */
            dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_NIF_PORT, if_id);
            for (tar_prio = 0; tar_prio < BCM_COS_COUNT; tar_prio++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_NIF_PRIORITY, tar_prio);
                dbal_value_field32_request(unit, entry_handle_id_pfc, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_pfc);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_pfc, DBAL_COMMIT));
                if (enabled_pfc)
                {
                    nif_port = if_id + core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                    (unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
                    if (pfc_enabled)
                    {
                        if (*target_count > target_max)
                        {
                            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                         "The target count is more than the specified maximum number of targets.");
                        }
                        target[*target_count].cosq = tar_prio;
                        BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                        *target_count += 1;
                    }
                }
            }
        }
    }

    /** Get targets from global resource to OOB */
    for (oob_if_cur = 0; oob_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); oob_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_OOB_IF, oob_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, oob_if_cur)->tx; cal_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_CAL_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id_oob_en, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_oob);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_oob_en, DBAL_COMMIT));
            if (enabled_oob)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_map, DBAL_FIELD_OOB_IF, oob_if_cur);
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_map, DBAL_FIELD_OOB_IDX, cal_index);
                dbal_value_field32_request(unit, entry_handle_id_oob_map, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_oob_map, DBAL_COMMIT));

                if (src_sel == DNX_OOB_TX_CAL_SRC_GLB_RSC)
                {
                    if (*target_count > target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    target[*target_count].calender_index = cal_index;
                    BCM_GPORT_CONGESTION_SET(port, oob_if_cur);
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 src_sel;
    bcm_port_t src_port, port;
    uint32 src_port_info, fc_index;
    bcm_cosq_vsq_info_t vsq_info;
    int cal_index, oob_if_cur;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    src_port_info = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(source->port);

        /** Get targets from LLFC VSQ to OOB */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    for (oob_if_cur = 0; oob_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); oob_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, oob_if_cur)->tx; cal_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (src_sel == DNX_OOB_TX_CAL_SRC_LLFC_VSQ)
            {
                if (src_port_info == fc_index)
                {
                    if (*target_count > target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    target[*target_count].calender_index = cal_index;
                    BCM_GPORT_CONGESTION_SET(port, oob_if_cur);
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }

    /*
     * Source is LLFC, get the inband mode
     */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, src_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        /*
         * Target is LLFC
         */
        BCM_GPORT_LOCAL_SET(port, src_port);
        target[*target_count].port = port;
        target[*target_count].cosq = -1;
        *target_count += 1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint32 src_sel, fc_index;
    bcm_port_t port, src_port;
    int cal_index, oob_if_cur;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_cosq_vsq_info_t vsq_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** Get targets from PFC VSQ to OOB */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    for (oob_if_cur = 0; oob_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); oob_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, oob_if_cur)->tx; cal_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (src_sel == DNX_OOB_TX_CAL_SRC_PFC_VSQ)
            {
                if ((src_port * BCM_COS_COUNT + source->cosq) == fc_index)
                {
                    if (*target_count > target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    target[*target_count].calender_index = cal_index;
                    BCM_GPORT_CONGESTION_SET(port, oob_if_cur);
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }

    /*
     * Source is PFC VSQ, get the inband mode
     */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, src_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        /*
         * Target is PFC
         */

        if (*target_count > target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }
        BCM_GPORT_LOCAL_SET(port, src_port);
        target[*target_count].port = port;
        target[*target_count].cosq = source->cosq;
        *target_count += 1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_vsq_a_d_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint8 src_sel;
    uint32 src_sel_get;
    bcm_port_t port;
    int traffic_class;
    int category;
    int connection_class;
    uint32 fc_index, fc_index_get;
    int cal_index, oob_if_cur;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the VSQ properties, FC index and source select from the VSQ Gport */
    SHR_IF_ERR_EXIT(dnx_fc_vsq_gport_to_fc_index
                    (unit, source->port, &src_sel, &fc_index, &traffic_class, &connection_class, &category));

    /** Get targets from A,B,C,D VSQs to OOB */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id));
    for (oob_if_cur = 0; oob_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); oob_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, oob_if_cur)->tx; cal_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel_get);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index_get);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (src_sel == src_sel_get)
            {
                if (fc_index == fc_index_get)
                {
                    if (*target_count > target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    target[*target_count].calender_index = cal_index;
                    BCM_GPORT_CONGESTION_SET(port, oob_if_cur);
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_c2_tc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint32 entry_handle_id_oob_en;
    int traffic_class;
    bcm_core_t core;
    int if_id;
    int nif_port = 0;
    int category;
    int connection_class;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t port;
    uint32 tar_prio, enabled, cal_index;
    uint32 oob_if_cur;
    uint8 src_sel;
    uint32 src_sel_get;
    uint32 fc_index, fc_index_get;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the VSQ properties, FC index and source select from the VSQ Gport */
    SHR_IF_ERR_EXIT(dnx_fc_vsq_gport_to_fc_index
                    (unit, source->port, &src_sel, &fc_index, &traffic_class, &connection_class, &category));

    /** Get the PFC targets from Category 2 TC VSQ */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_PFC_EN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAT2_PRIO, traffic_class);
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (if_id = 0; if_id < DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE; if_id++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, if_id);
            for (tar_prio = 0; tar_prio < BCM_COS_COUNT; tar_prio++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, tar_prio);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (enabled)
                {
                    nif_port = if_id + core * DNX_DATA_MAX_NIF_PHYS_NOF_PHYS_PER_CORE;
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                    (unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
                    if (pfc_enabled)
                    {
                        if (*target_count > target_max)
                        {
                            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                         "The target count is more than the specified maximum number of targets.");
                        }
                        target[*target_count].cosq = tar_prio;
                        BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                        *target_count += 1;
                    }
                }
            }
        }
    }

    /** Get targets from from Category 2 TC VSQ to OOB */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_OOB_MAP, entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_OOB_EN, &entry_handle_id_oob_en));
    for (oob_if_cur = 0; oob_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); oob_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, oob_if_cur)->tx; cal_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel_get);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index_get);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (src_sel == src_sel_get)
            {
                if (fc_index == fc_index_get)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_OOB_IF, oob_if_cur);
                    dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_CAL_IDX, cal_index);
                    dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_CAT2_PRIO, traffic_class);
                    dbal_value_field32_request(unit, entry_handle_id_oob_en, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_oob_en, DBAL_COMMIT));
                    if (enabled == 1)
                    {
                        if (*target_count > target_max)
                        {
                            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                         "The target count is more than the specified maximum number of targets.");
                        }
                        target[*target_count].calender_index = cal_index;
                        BCM_GPORT_CONGESTION_SET(port, oob_if_cur);
                        target[*target_count].port = port;
                        target[*target_count].cosq = -1;
                        *target_count += 1;
                    }
                }
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id_oob;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 src_sel, fc_index, cal_index, enabled;
    bcm_port_t src_port, port;
    int if_id, oob_if_cur;
    int tar_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source interface */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /**Get the inband mode*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, src_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));

    if (llfc_enabled)
    {
        /** Get LLFC targets */
        if (*target_count > target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }
        target[*target_count].port = source->port;
        target[*target_count].cosq = -1;
        *target_count += 1;
    }

    if (pfc_enabled)
    {
        /** Get LLFC targets, check if thresholds are enabled to produce PFC */
        for (tar_prio = 0; tar_prio < BCM_COS_COUNT; tar_prio++)
        {
            /** Call IMB API to get the Enable field of the RMCs of the Port to check if they were enabled to produce PFC for the specified priority */
            SHR_IF_ERR_EXIT(imb_port_gen_pfc_from_rmc_enable_get(unit, src_port, tar_prio, &enabled));

            if (*target_count > target_max)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
            }
            if (enabled)
            {
                target[*target_count].port = source->port;
                target[*target_count].cosq = tar_prio;
                *target_count += 1;
            }
        }
    }

    /*
     * Get the source interface ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, src_port, &if_id));

    /** Get targets from Almost Fill NIF to OOB */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_MAP, &entry_handle_id_oob));
    for (oob_if_cur = 0; oob_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); oob_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id_oob, DBAL_FIELD_OOB_IF, oob_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, oob_if_cur)->tx; cal_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id_oob, DBAL_FIELD_OOB_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id_oob, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel);
            dbal_value_field32_request(unit, entry_handle_id_oob, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_oob, DBAL_COMMIT));
            if (src_sel == DNX_OOB_TX_CAL_SRC_NIF)
            {
                if (if_id == fc_index)
                {
                    if (*target_count > target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    target[*target_count].calender_index = cal_index;
                    BCM_GPORT_CONGESTION_SET(port, oob_if_cur);
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_cosq_gport_eth_port_threshold_set_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /*
     * Validate gport
     */
    if (!BCM_GPORT_IS_LOCAL(gport) && !BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unit %d, Unsupported gport(0x%x) for BCM_COSQ_THRESHOLD_ETH_PORT_LLFC/PFC",
                     unit, gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_cosq_gport_eth_port_threshold_set_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set)
{
    bcm_port_t port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_eth_port_threshold_set_get_verify(unit, gport, threshold, is_set));

    /** Get the logical port */
    port = BCM_GPORT_IS_LOCAL(gport) ? BCM_GPORT_LOCAL_GET(gport) : BCM_GPORT_LOCAL_INTERFACE_GET(gport);

    /** Call IMB APIs to set/get the threshold for all RMCs of the port */
    if (is_set)
    {
        SHR_IF_ERR_EXIT(imb_port_fc_rx_qmlf_threshold_set(unit, port, threshold->flags, (uint32) threshold->value));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_port_fc_rx_qmlf_threshold_get(unit, port, threshold->flags, (uint32 *) &threshold->value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function get the appropriate DBAL table and fields from  the COSQ resource format
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] cosq_resource - COSQ resource
 *   \param [out] table - DBAL table
 *   \param [out] set_thresh_field - DBAL field for SET threshold
 *   \param [out] clear_thresh_field - DBAL field for CLEAR threshold
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_vsq_gl_fc_resource_to_dbal_info(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    bcm_cosq_resource_t cosq_resource,
    dbal_tables_e * table,
    dbal_fields_e * set_thresh_field,
    dbal_fields_e * clear_thresh_field)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (cosq_resource)
    {
        case bcmResourceBytes:
            *table = DBAL_TABLE_FC_GLB_RCS_PER_POOL_THS;
            *set_thresh_field = DBAL_FIELD_DRAM_WORDS_PER_POOL_TH_SET;
            *clear_thresh_field = DBAL_FIELD_DRAM_WORDS_PER_POOL_TH_CLR;
            break;
        case bcmResourceDramBundleDescriptorsBuffers:
            *table = DBAL_TABLE_FC_GLB_RCS_TOTAL_THS;
            *set_thresh_field = DBAL_FIELD_DRAM_BDBS_TOTAL_TH_SET;
            *clear_thresh_field = DBAL_FIELD_DRAM_BDBS_TOTAL_TH_CLR;
            break;
        case bcmResourceOcbBuffers:
            if (index_type == DNX_COSQ_THRESHOLD_INDEX_PRIO)
            {
                *table = DBAL_TABLE_FC_GLB_RCS_TOTAL_THS;
                *set_thresh_field = DBAL_FIELD_SRAM_BUFFERS_TOTAL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_SRAM_BUFFERS_TOTAL_TH_CLR;
            }
            else if (index_type == DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO)
            {
                *table = DBAL_TABLE_FC_GLB_RCS_PER_POOL_THS;
                *set_thresh_field = DBAL_FIELD_SRAM_BUFFERS_PER_POOL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_SRAM_BUFFERS_PER_POOL_TH_CLR;
            }
            else
            {
                *table = DBAL_TABLE_FC_GLB_RCS_HDRM_THS;
                *set_thresh_field = DBAL_FIELD_HDRM_BUFFERS_TOTAL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_HDRM_BUFFERS_TOTAL_TH_CLR;
            }
            break;
        case bcmResourceOcbPacketDescriptors:
            if (index_type == DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO)
            {
                *table = DBAL_TABLE_FC_GLB_RCS_PER_POOL_THS;
                *set_thresh_field = DBAL_FIELD_SRAM_PDS_PER_POOL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_SRAM_PDS_PER_POOL_TH_CLR;
            }
            else
            {
                *table = DBAL_TABLE_FC_GLB_RCS_HDRM_THS;
                *set_thresh_field = DBAL_FIELD_HDRM_PDS_TOTAL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_HDRM_PDS_TOTAL_TH_CLR;
            }
            break;
        case bcmResourceOcbPacketDescriptorsBuffers:
            *table = DBAL_TABLE_FC_GLB_RCS_TOTAL_THS;
            *set_thresh_field = DBAL_FIELD_SRAM_PDBS_TOTAL_TH_SET;
            *clear_thresh_field = DBAL_FIELD_SRAM_PDBS_TOTAL_TH_CLR;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", cosq_resource);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to verify the arguments of dnx_cosq_vsq_gl_total_fc_threshold_set_get.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_vsq_gl_total_fc_threshold_verify(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    int threshold_set, threshold_clear;
    dbal_tables_e table;
    dbal_fields_e set_thresh_field;
    dbal_fields_e clear_thresh_field;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE /** allow_all */ );

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");
    }

    if ((index_type == DNX_COSQ_THRESHOLD_INDEX_PRIO) && ((priority != 0) && (priority != 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority value, must be Low:1 or High:0 %d", priority);
    }

    if ((index_type == DNX_COSQ_THRESHOLD_INDEX_INVALID)
        && ((thresh_info->resource_type != bcmResourceOcbPacketDescriptors)
            && (thresh_info->resource_type != bcmResourceOcbBuffers)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index value, must be Global resource priority");
    }

    switch (thresh_info->resource_type)
    {

        case bcmResourceDramBundleDescriptorsBuffers:
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
        case bcmResourceOcbPacketDescriptorsBuffers:
            /** expected resources */

            if (is_set)
            {
                /** set API only*/

                /** Get the appropriate DBAL table and fields */
                SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_fc_resource_to_dbal_info
                                (unit, index_type, thresh_info->resource_type, &table, &set_thresh_field,
                                 &clear_thresh_field));

                /** Check "set" value */
                SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                                (unit, table, set_thresh_field, FALSE, 0, 0, &threshold_set));
                if (threshold->thresh_set > threshold_set)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Threshold value to set Flow Control(%d) is bigger than the maximum allowed(%d).\n",
                                 threshold->thresh_set, threshold_set);
                }

                /** Check "clear" value */
                SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                                (unit, table, clear_thresh_field, FALSE, 0, 0, &threshold_clear));
                if (threshold->thresh_clear > threshold_clear)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Threshold value to clear Flow Control(%d) is bigger than the maximum allowed(%d).\n",
                                 threshold->thresh_clear, threshold_clear);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to set the HW for the FC thresholds of total global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] cosq_resource - COSQ resource
 *   \param [in,out] set_threshold - threshold value to start generating FC
 *   \param [in,out] clear_threshold - threshold value to stop generating FC
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_vsq_gl_total_fc_threshold_hw_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int priority,
    bcm_cosq_resource_t cosq_resource,
    uint32 *set_threshold,
    uint32 *clear_threshold,
    uint8 is_set)
{
    uint32 entry_handle_id;
    dbal_tables_e table;
    dbal_fields_e set_thresh_field;
    dbal_fields_e clear_thresh_field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the appropriate DBAL table and fields */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_fc_resource_to_dbal_info
                    (unit, index_type, cosq_resource, &table, &set_thresh_field, &clear_thresh_field));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (table != DBAL_TABLE_FC_GLB_RCS_HDRM_THS)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS_PRIO, priority);
    }

    if (is_set)
    {
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, set_thresh_field, INST_SINGLE, *set_threshold);
        dbal_entry_value_field32_set(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, *clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Getting value fields */
        dbal_value_field32_request(unit, entry_handle_id, set_thresh_field, INST_SINGLE, set_threshold);
        dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function set or get the thresholds for Flow control generation from total global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_vsq_gl_total_fc_threshold_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    bcm_cosq_static_threshold_t threshold_internal;
    uint32 granularity;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ranularity for the internal representation encoding of the thresholds */
    granularity =
        ((thresh_info->resource_type ==
          bcmResourceBytes) ? dnx_data_ingr_congestion.
         info.bytes_threshold_granularity_get(unit) : dnx_data_ingr_congestion.info.threshold_granularity_get(unit));

    /** Convert threshold to internal representation according to granularity */
    if (is_set)
    {
        threshold_internal.thresh_set = threshold->thresh_set / granularity;
        threshold_internal.thresh_clear = threshold->thresh_clear / granularity;
    }

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_vsq_gl_total_fc_threshold_verify(unit, core_id, index_type,
                                                                    priority, thresh_info, &threshold_internal,
                                                                    is_set));
    /** Set/get HW */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_total_fc_threshold_hw_set_get(unit, core_id, index_type, priority,
                                                                  thresh_info->resource_type,
                                                                  (uint32 *) &threshold_internal.thresh_set,
                                                                  (uint32 *) &threshold_internal.thresh_clear, is_set));

    /** Convert internal representation to threshold according to granularity */
    if (!is_set)
    {
        threshold->thresh_set = threshold_internal.thresh_set * granularity;
        threshold->thresh_clear = threshold_internal.thresh_clear * granularity;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to verify the arguments of dnx_cosq_vsq_gl_total_fc_threshold_set_get.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] pool_id - Pool ID
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_vsq_gl_per_pool_fc_threshold_verify(
    int unit,
    int core_id,
    int pool_id,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    dnx_ingress_congestion_resource_type_e resource_type;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE /** allow_all */ );

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");
    }

    if ((priority != 0) && (priority != 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority value, must be Low:1 or High:0 %d", priority);

    }

    if (pool_id < 0 || pool_id >= DNX_DEVICE_COSQ_VSQ_NOF_RSRC_POOLS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Pool ID value %d", pool_id);
    }

    switch (thresh_info->resource_type)
    {

        case bcmResourceBytes:
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
            /** expected resources */
            if (is_set)
            {
                /** set API */
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit,
                                                                                         thresh_info->resource_type,
                                                                                         &resource_type));

                if (threshold->thresh_set > dnx_data_ingr_congestion.info.resource_get(unit, resource_type)->max)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Set threshold(%d) higher than maximum allowed(%d)",
                                 threshold->thresh_set, dnx_data_ingr_congestion.info.resource_get(unit,
                                                                                                   resource_type)->max);

                }
                if (threshold->thresh_clear > dnx_data_ingr_congestion.info.resource_get(unit, resource_type)->max)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Clear threshold(%d) higher than maximum allowed(%d)",
                                 threshold->thresh_clear, dnx_data_ingr_congestion.info.resource_get(unit,
                                                                                                     resource_type)->max);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to set the HW for the FC thresholds of total global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] pool_id - Pool ID
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] cosq_resource - COSQ resource
 *   \param [in,out] set_threshold - threshold value to start generating FC
 *   \param [in,out] clear_threshold - threshold value to stop generating FC
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_vsq_gl_per_pool_fc_threshold_hw_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int pool_id,
    int priority,
    bcm_cosq_resource_t cosq_resource,
    uint32 *set_threshold,
    uint32 *clear_threshold,
    uint8 is_set)
{
    uint32 entry_handle_id;
    dbal_tables_e table;
    dbal_fields_e set_thresh_field;
    dbal_fields_e clear_thresh_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the appropriate DBAL table and fields */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_fc_resource_to_dbal_info
                    (unit, index_type, cosq_resource, &table, &set_thresh_field, &clear_thresh_field));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS_PRIO, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS_POOL, pool_id);

    if (is_set)
    {
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, set_thresh_field, INST_SINGLE, *set_threshold);
        dbal_entry_value_field32_set(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, *clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Getting value fields */
        dbal_value_field32_request(unit, entry_handle_id, set_thresh_field, INST_SINGLE, set_threshold);
        dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function set or get the thresholds for Flow control generation from per pool global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] pool_id - Pool ID
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_vsq_gl_per_pool_fc_threshold_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int pool_id,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    bcm_cosq_static_threshold_t threshold_internal;
    uint32 granularity;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ranularity for the internal representation encoding of the thresholds */
    granularity =
        ((thresh_info->resource_type ==
          bcmResourceBytes) ? dnx_data_ingr_congestion.
         info.bytes_threshold_granularity_get(unit) : dnx_data_ingr_congestion.info.threshold_granularity_get(unit));

    /** Convert threshold to internal representation according to granularity */
    if (is_set)
    {
        threshold_internal.thresh_set = threshold->thresh_set / granularity;
        threshold_internal.thresh_clear = threshold->thresh_clear / granularity;
    }

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_vsq_gl_per_pool_fc_threshold_verify(unit, core_id, pool_id,
                                                                       priority, thresh_info, &threshold_internal, is_set));

    /** Set/get HW */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_per_pool_fc_threshold_hw_set_get(unit, core_id, index_type,
                                                                     pool_id, priority,
                                                                     thresh_info->resource_type,
                                                                     (uint32 *) &threshold_internal.thresh_set,
                                                                     (uint32 *) &threshold_internal.thresh_clear,
                                                                     is_set));

    /** Convert internal representation to threshold according to granularity */
    if (!is_set)
    {
        threshold->thresh_set = threshold_internal.thresh_set * granularity;
        threshold->thresh_clear = threshold_internal.thresh_clear * granularity;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_glb_rsc_threshold_set(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_PRIO:
        case DNX_COSQ_THRESHOLD_INDEX_INVALID:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_total_fc_threshold_set_get(unit, core_id, index_type,
                                                                     index_info->priority,
                                                                     thresh_info, threshold, TRUE));
            break;
        case DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_per_pool_fc_threshold_set_get(unit, core_id, index_type,
                                                                        index_info->pool_id,
                                                                        index_info->priority,
                                                                        thresh_info, threshold, TRUE));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid index type %d", unit, index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_glb_rsc_threshold_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_PRIO:
        case DNX_COSQ_THRESHOLD_INDEX_INVALID:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_total_fc_threshold_set_get(unit, core_id,
                                                                     index_type,
                                                                     index_info->priority,
                                                                     thresh_info, threshold, FALSE));
            break;
        case DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_per_pool_fc_threshold_set_get(unit, core_id, index_type,
                                                                        index_info->pool_id,
                                                                        index_info->priority,
                                                                        thresh_info, threshold, FALSE));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid index type %d", unit, index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
