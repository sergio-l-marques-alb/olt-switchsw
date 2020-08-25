/** \file oam.c
 * $Id$
 *
 * OAM procedures for DNX.
 *
 * The procedured include procedures to add,delete,get the 
 * following oam_group_* , oam_endpoint_*, 
 * oam_endpoint_action_*, oam_loss_* oam_delay_*, 
 * oam_loopback_*, oam_action_*, oam_ais_*, oam_tst_*, 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include "oam_internal.h"
#include "oam_counter.h"
#include <soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Index of LSB in MAC */
#define MAC_LSB_INDEX (UTILEX_PP_MAC_ADDRESS_NOF_U8 - 1)

/*
 * }
 */

/*
 * MACROs
 * {
 */
/** Check if a MAC address is unicast or multicast */
#define OAM_CLASSIFIER_CHECK_MAC_IS_UNICAST(mac_address) ((mac_address[0] & 0x1) != 1)
/** Set correct G_ACH_CHANNEL_TYPE_VALUE field according to the index */
#define DBAL_FIELD_G_ACH_CHANNEL_TYPE_VALUE_INDEX(index) (DBAL_FIELD_CFG_G_ACH_CHANNEL_TYPE_VALUE_1 + index)
/** Set correct G_ACH_LAYER_PROTOCOL_VALUE field according to the index */
#define DBAL_FIELD_G_ACH_LAYER_PROTOCOL_VALUE_INDEX(index) (DBAL_FIELD_CFG_G_ACH_LAYER_PROTOCOL_VALUE_1 + index)
/*
 * }
 */

/** Valid range of indexes for BFD PWE CW registers are 0 to 3 */
#define _BCM_OAM_DISSECT_BFD_PWE_CW_MEM_INDEX_VALID(index) (index >=0 && index <=3)

/* Verify OAM event type */
#define DNX_OAM_EVENT_TYPE_VERIFY(event_types) \
    ((SHR_BITGET(event_types.w, bcmOAMEventEndpointCCMTimeout))         ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointCCMTimein))          ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointRemote))             ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointRemoteUp))           ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointPortDown))           ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointPortUp))             ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceDown))      ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceUp))        ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceTesting))   ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceUnkonwn))   ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceDormant))   ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceNotPresent))||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointInterfaceLLDown))    ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointSdSet))              ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointSdClear))            ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointSfSet))              ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointSfClear))            ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointDExcessSet))         ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointDMissmatch))         ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointDMissmerge))         ||\
     (SHR_BITGET(event_types.w, bcmOAMEventEndpointDAllClear)))

 /*
  * Global and Static
  */

/*
 * }
 */

/**
* \brief
*  Add an OAM Alarm Indication Signal entry. This function is
*  not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] ais_ptr  -
*   Pointer to the structure that holds information for the
*   Alarm Indication Signal etry.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_ais_delete
*   * \ref bcm_dnx_oam_ais_get
*/
shr_error_e
bcm_dnx_oam_ais_add(
    int unit,
    bcm_oam_ais_t * ais_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_ais_add not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete an OAM Alarm Indication Signal entry. This function is
*  not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] ais_ptr  -
*   Pointer to the structure that holds information for the
*   Alarm Indication Signal etry.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_ais_add
*   * \ref bcm_dnx_oam_ais_get
*/
shr_error_e
bcm_dnx_oam_ais_delete(
    int unit,
    bcm_oam_ais_t * ais_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_ais_delete not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an OAM Alarm Indication Signal entry. This function is
*  not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [out] ais_ptr  -
*   Pointer to the structure that holds information for the
*   Alarm Indication Signal etry. The information from the Get
*   procedure will be written in it.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_ais_delete
*   * \ref bcm_dnx_oam_ais_get
*/
shr_error_e
bcm_dnx_oam_ais_get(
    int unit,
    bcm_oam_ais_t * ais_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_ais_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get RX/TX enable per opcode index.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] is_rx  -
*   Select which side to be updated, RX or TX.
*   \param [in] index  -
*   Index of register array. Indexing per Opcode-For-Count-Index.
*   \param [out] arg  -
*   Return enable or disable value.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - Return relevant error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref dnx_oam_opcode_for_count_set
*/
shr_error_e
dnx_oam_opcode_for_count_get(
    int unit,
    uint8 is_rx,
    int index,
    uint32 *arg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OPCODE_FOR_COUNT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_INDEX, index);

    if (DNX_OAM_DISSECT_IS_RX_DIRECTION(is_rx))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_RX_ENABLE, INST_SINGLE, arg);
    }
    else
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_TX_ENABLE, INST_SINGLE, arg);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get TX/RX prefixes and shifts
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] shift  -
*   Represent what should be the shift value in the register
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - Return relevant error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref dnx_oam_opcode_for_count_set
*/
static shr_error_e
dnx_oam_tx_rx_shift_counter_cfg_get(
    int unit,
    uint32 *shift)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));

    /** Since the RX and TX shifts are with same value, get and return only one of them. */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_COUNTER_CFG_SHIFT, INST_SINGLE, shift);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Map the actual OpCode value to OpCode for count index according to the table
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] opcode  -
*   Represent the actual packet opcode in range 0-255.
*   \param [in] opcode_for_count_index  -
*   Represent the index for opcode for count in range 0-5.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - Return relevant error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref dnx_oam_opcode_for_count_set
*/
static shr_error_e
dnx_oam_opcode_to_opcode_for_count_index(
    int unit,
    int opcode,
    int *opcode_for_count_index)
{

    SHR_FUNC_INIT_VARS(unit);

    if (opcode < 0 || opcode > 255)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported Opcode, available range 0-255, actual %d", opcode);
    }

    switch (opcode)
    {
        /** Currently only CCM and BFD opcode are supported per opcode counting */
        /** Opcode "0" represent the BFD opcode*/
        case 0:
        case bcmOamOpcodeCCM:
            *opcode_for_count_index = 0;
            break;
        default:
            *opcode_for_count_index = 5;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an OAM Control entry.
* \param [in] unit  -
*   Relevant unit.
* \param [in] key  -
*   key.type - Type of Control being used
*   key.index - index from which to get control information.
* \param [out] arg -
*   Argument, in which to write the information from the get
*   procedure.
* \retval
*   shr_error_param - Wrong type was selected.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_control_index_set
*/
shr_error_e
bcm_dnx_oam_control_indexed_get(
    int unit,
    bcm_oam_control_key_t key,
    uint64 *arg)
{
    int opcode;
    uint32 enable;
    SHR_FUNC_INIT_VARS(unit);

    switch (key.type)
    {
        /** Get control value (enable/disable) per opcode for TX statistics*/
        case bcmOamControlOampStatsTxOpcode:
        {
            /** Map the actual OpCode value to OpCode index */
            SHR_IF_ERR_EXIT(dnx_oam_opcode_to_opcode_for_count_index(unit,key.index, &opcode));
            SHR_IF_ERR_EXIT(dnx_oam_opcode_for_count_get(unit, bcmOamControlOampStatsTxOpcode , opcode, &enable));
            COMPILER_64_SET(*arg, 0, enable);
        }
            break;
        /** Get control value (enable/disable) per opcode for RX statistics*/
        case bcmOamControlOampStatsRxOpcode:
        {
            SHR_IF_ERR_EXIT(dnx_oam_opcode_to_opcode_for_count_index(unit,key.index, &opcode));
            /** Enable RX per opcode counting */
            SHR_IF_ERR_EXIT(dnx_oam_opcode_for_count_get(unit, bcmOamControlOampStatsRxOpcode , opcode, &enable));
            COMPILER_64_SET(*arg, 0, enable);
        }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported bcm oam control index type");
    }

exit:
    SHR_FUNC_EXIT;
}

/** Get CPU port DP and TC values from OAMP_CPUPORT register */
shr_error_e
dnx_oam_cpu_port_dp_tc_get(
    int unit,
    uint32 *dp,
    uint32 *tc)
{

    uint32 entry_handle_id;
    int table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table = DBAL_TABLE_OAMP_INIT_CPU_PORT_CFG;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

    /** getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_TC, INST_SINGLE, tc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_DP, INST_SINGLE, dp);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an OAM Control entry. This function is not supported.
* \param [in] unit  -
*   Relevant unit.
* \param [in] type  -
*   Type of Control being used
* \param [out] arg -
*   Argument, in which to write the information from the get
*   procedure.
* \retval
*   shr_error_unavail - Return unavailable.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_control_set
*/
shr_error_e
bcm_dnx_oam_control_get(
    int unit,
    bcm_oam_control_type_t type,
    uint64 *arg)
{
    uint32 counter_range_min;
    uint32 counter_range_max;
    uint32 shift_index;
    uint32 dp, tc, dp_tc;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
            /*
             * Get lower boundary of counter range for non PCP counters
             */
        case bcmOamControlLmPcpCounterRangeMin:
        {
            /*
             * Get current counter range values Ingress and Egress will always have the same range, so we read the
             * ingress register only. 
             */
            rv = dnx_oam_counter_global_range_get(unit, DNX_OAM_CLASSIFIER_INGRESS, &counter_range_min,
                                                  &counter_range_max);
            SHR_IF_ERR_EXIT(rv);

            COMPILER_64_SET(*arg, 0, counter_range_min);
        }
            break;
            /*
             * Get upper boundary of counter range for non PCP counters
             */
        case bcmOamControlLmPcpCounterRangeMax:
        {
            /*
             * Get current counter range values Ingress and Egress will always have the same range, so we read the
             * ingress register only. 
             */
            rv = dnx_oam_counter_global_range_get(unit, DNX_OAM_CLASSIFIER_INGRESS, &counter_range_min,
                                                  &counter_range_max);
            SHR_IF_ERR_EXIT(rv);

            COMPILER_64_SET(*arg, 0, counter_range_max);
        }
            break;
        case bcmOamControlOampPuntPacketIntPri:
        {
            /*
             * Read the register, extract the fields, combine them and write to the output parameter
             */
            rv = dnx_oam_cpu_port_dp_tc_get(unit, &dp, &tc);
            SHR_IF_ERR_EXIT(rv);

            dp_tc = dp | (tc << 2);
            COMPILER_64_SET(*arg, 0, dp_tc);
        }
            break;
        /** Get TX/RX Shift value*/
        case bcmOamControlOampStatsShift:
        {
            SHR_IF_ERR_EXIT(dnx_oam_tx_rx_shift_counter_cfg_get(unit, &shift_index));
            COMPILER_64_SET(*arg, 0, shift_index);
        }
            break;
        case bcmOamControlFlexibleDropPort:
        case bcmOamControlFlexibleDropCVlan:
        case bcmOamControlFlexibleDropSVlan:
        case bcmOamControlFlexibleDropCPlusSVlan:
        case bcmOamControlFlexibleDropVP:
        case bcmOamControlFlexibleDropEgressPort:
        case bcmOamControlFlexibleDropEgressCVlan:
        case bcmOamControlFlexibleDropEgressSVlan:
        case bcmOamControlFlexibleDropEgressCPlusSVlan:
        case bcmOamControlFlexibleDropEgressVP:
        case bcmOamControlFlexibleDropPasssiveSap:
        case bcmOamControlFlexibleDropEgressPasssiveSap:
        case bcmOamControlReportMode:
        case bcmOamControlUnknownAchCount:
        case bcmOamControlNtpToD:
        case bcmOamControl1588ToD:
        case bcmOamControlSrcMacLsbReserve:
        case bcmOamControlSrcMacLsbUnReserve:
        case bcmOamControlOampCCMWeight:
        case bcmOamControlOampSATWeight:
        case bcmOamControlOampResponseWeight:
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported bcm oam control type");
    }

exit:
    SHR_FUNC_EXIT;
}

/** Set TX/RX prefixes and shifts */
static shr_error_e
dnx_oam_tx_rx_shift_counter_cfg_set(
    int unit,
    int shift)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_COUNTER_CFG_PREFIX, INST_SINGLE,
                                 0 /** TX prefix*/ );
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_COUNTER_CFG_SHIFT, INST_SINGLE, shift);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_COUNTER_CFG_PREFIX, INST_SINGLE,
                                 1 /** RX prefix*/ );
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_COUNTER_CFG_SHIFT, INST_SINGLE, shift);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set RX/TX enable per opcode index.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] is_rx  -
*   Select which side to be updated, RX or TX.
*   \param [in] index  -
*   Index of register array. Indexing per Opcode-For-Count-Index
*   \param [in] arg  -
*   Set enable or disable value.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - Return relevant error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref dnx_oam_opcode_for_count_get
*/
shr_error_e
dnx_oam_opcode_for_count_set(
    int unit,
    uint8 is_rx,
    int index,
    uint8 arg)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OPCODE_FOR_COUNT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_INDEX, index);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT, INST_SINGLE, index);
    if (DNX_OAM_DISSECT_IS_RX_DIRECTION(is_rx))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_RX_ENABLE, INST_SINGLE, arg);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_TX_ENABLE, INST_SINGLE, arg);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set an OAM Control entry.
* \param [in] unit  -
*   Relevant unit.
* \param [in] key  - Type of Control being used:
*   key.type - Type of Control being used
*       bcmOamControlOampStatsTxOpcode - Enable per opcode counting for TX side.
*       bcmOamControlOampStatsRxOpcode - Enable per opcode counting for TX side.
*   key.index - index to which will write.
* \param [in] arg -
*   Argument, containing information about the Control entry.
* \retval
*   shr_error_param - Wrong type was selected.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_control_indexed_get
*/
shr_error_e
bcm_dnx_oam_control_indexed_set(
    int unit,
    bcm_oam_control_key_t key,
    uint64 arg)
{
    shr_error_e rv;
    int opcode;
    int is_enabled=1;
    SHR_FUNC_INIT_VARS(unit);

    switch (key.type)
    {
        /** Enable/Disable statistics per opcode counting for TX side */
        case bcmOamControlOampStatsTxOpcode:
        {
            /** Map the actual OpCode value to OpCode index */
            SHR_IF_ERR_EXIT(dnx_oam_opcode_to_opcode_for_count_index(unit,key.index, &opcode));
            if (COMPILER_64_EQ(arg, is_enabled))
            {
                /** Enable TX per opcode counting */
                rv = dnx_oam_opcode_for_count_set(unit, bcmOamControlOampStatsTxOpcode , opcode, TRUE);
            }
            else
            {
                /** Disable TX per opcode counting */
                rv = dnx_oam_opcode_for_count_set(unit, bcmOamControlOampStatsTxOpcode , opcode, FALSE);
            }

            SHR_IF_ERR_EXIT(rv);
        }
            break;
        /** Enable/Disable statistics per opcode counting for RX side */
        case bcmOamControlOampStatsRxOpcode:
        {
            SHR_IF_ERR_EXIT(dnx_oam_opcode_to_opcode_for_count_index(unit,key.index, &opcode));
            if (COMPILER_64_EQ(arg, is_enabled))
            {
                /** Enable RX per opcode counting */
                rv = dnx_oam_opcode_for_count_set(unit, bcmOamControlOampStatsRxOpcode , opcode, TRUE);
            }
            else
            {
                /** Disable RX per opcode counting */
                rv = dnx_oam_opcode_for_count_set(unit, bcmOamControlOampStatsRxOpcode , opcode, FALSE);
            }

            SHR_IF_ERR_EXIT(rv);
        }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported bcm oam control index type");
    }

exit:
    SHR_FUNC_EXIT;
}

/** Set CPU port DP and TC values to OAMP_CPUPORT register */
shr_error_e
dnx_oam_cpu_port_dp_tc_set(
    int unit,
    uint32 dp,
    uint32 tc)
{

    uint32 entry_handle_id;
    int table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table = DBAL_TABLE_OAMP_INIT_CPU_PORT_CFG;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_TC, INST_SINGLE, tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_DP, INST_SINGLE, dp);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set an OAM Control entry. This function is not supported.
* \param [in] unit  -
*   Relevant unit.
* \param [in] type  -
*   Type of Control being used
* \param [in] arg -
*   Argument, containing information about the Control entry.
* \retval
*   shr_error_unavail - Return unavailable.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_control_get
*/
shr_error_e
bcm_dnx_oam_control_set(
    int unit,
    bcm_oam_control_type_t type,
    uint64 arg)
{
    uint32 counter_range_min;
    uint32 counter_range_max;
    uint64 MAX_RANGE, counter_range_min_u64, counter_range_max_u64;
    uint32 DP_MASK, TC_MASK;
    uint32 dp, tc;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
            /*
             * Set lower boundary of counter range for non PCP counters
             */
        case bcmOamControlLmPcpCounterRangeMin:
        {
            /*
             * Get current counter range values Ingress and Egress will always have the same range, so we check for
             * validity for ingress only. 
             */
            rv = dnx_oam_counter_global_range_get(unit, DNX_OAM_CLASSIFIER_INGRESS, &counter_range_min,
                                                  &counter_range_max);
            SHR_IF_ERR_EXIT(rv);

          /** Validity check for ingress */
            COMPILER_64_SET(counter_range_max_u64, 0, counter_range_max);
            if (COMPILER_64_GT(arg, counter_range_max_u64))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "llegal LM counter range min value. Must be below counter range max.\n");
            }

          /** Set the new ingress counter range value */
            rv = dnx_oam_counter_global_range_set(unit, DNX_OAM_CLASSIFIER_INGRESS, COMPILER_64_LO(arg),
                                                  counter_range_max);
            SHR_IF_ERR_EXIT(rv);

          /** Set the new egress counter range value */
            rv = dnx_oam_counter_global_range_set(unit, DNX_OAM_CLASSIFIER_EGRESS, COMPILER_64_LO(arg),
                                                  counter_range_max);
            SHR_IF_ERR_EXIT(rv);

        }
            break;
            /*
             * Set upper boundary of counter range for non PCP counters
             */
        case bcmOamControlLmPcpCounterRangeMax:
        {
            /*
             * Validity check - counter index must not be above DNX_MAX_COUNTER_RANGE 
             */
            COMPILER_64_SET(MAX_RANGE, 0, DNX_MAX_COUNTER_RANGE);
            if (COMPILER_64_GT(arg, MAX_RANGE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "llegal LM counter range max value\n");
            }

            /*
             * Get current counter range values Ingress and Egress will always have the same range, so we check for
             * validity for ingress only. 
             */
            rv = dnx_oam_counter_global_range_get(unit, DNX_OAM_CLASSIFIER_INGRESS, &counter_range_min,
                                                  &counter_range_max);
            SHR_IF_ERR_EXIT(rv);

            COMPILER_64_SET(counter_range_min_u64, 0, counter_range_min);
            if (COMPILER_64_LT(arg, counter_range_min_u64))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "llegal LM counter range max value. Must be larger counter range min.\n");
            }

            rv = dnx_oam_counter_global_range_set(unit, DNX_OAM_CLASSIFIER_INGRESS, counter_range_min,
                                                  COMPILER_64_LO(arg));
            SHR_IF_ERR_EXIT(rv);

            rv = dnx_oam_counter_global_range_set(unit, DNX_OAM_CLASSIFIER_EGRESS, counter_range_min,
                                                  COMPILER_64_LO(arg));
            SHR_IF_ERR_EXIT(rv);
        }
            break;
        case bcmOamControlOampPuntPacketIntPri:
        {
            /*
             * Bits [1:0] of given value determine the color (DP), bits [4:2] determine the Traffic Class 
             */
            COMPILER_64_SET(MAX_RANGE, 0, 0x001F);
            if (COMPILER_64_GT(arg, MAX_RANGE))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Illegal DP and TC values\n");
            }
            DP_MASK = 0x0003;
            TC_MASK = 0x001C;
            dp = COMPILER_64_LO(arg) & DP_MASK; /* prepare dp value: (arg &0x3) */
            tc = (COMPILER_64_LO(arg) & TC_MASK) >> 2;  /* prepare tc value: (arg >> 2)&0x7 */
            rv = dnx_oam_cpu_port_dp_tc_set(unit, dp, tc);
            SHR_IF_ERR_EXIT(rv);
        }
            break;
        /**
        * Set TX/RX shift, which will be used in the formula
        * to calculate statistics counter id 
        */
        case bcmOamControlOampStatsShift:
        {
            SHR_IF_ERR_EXIT(dnx_oam_tx_rx_shift_counter_cfg_set(unit, arg));
        }
            break;
        case bcmOamControlFlexibleDropPort:
        case bcmOamControlFlexibleDropCVlan:
        case bcmOamControlFlexibleDropSVlan:
        case bcmOamControlFlexibleDropCPlusSVlan:
        case bcmOamControlFlexibleDropVP:
        case bcmOamControlFlexibleDropEgressPort:
        case bcmOamControlFlexibleDropEgressCVlan:
        case bcmOamControlFlexibleDropEgressSVlan:
        case bcmOamControlFlexibleDropEgressCPlusSVlan:
        case bcmOamControlFlexibleDropEgressVP:
        case bcmOamControlFlexibleDropPasssiveSap:
        case bcmOamControlFlexibleDropEgressPasssiveSap:
        case bcmOamControlReportMode:
        case bcmOamControlUnknownAchCount:
        case bcmOamControlNtpToD:
        case bcmOamControl1588ToD:
        case bcmOamControlSrcMacLsbReserve:
        case bcmOamControlSrcMacLsbUnReserve:
        case bcmOamControlOampCCMWeight:
        case bcmOamControlOampSATWeight:
        case bcmOamControlOampResponseWeight:
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported bcm oam control type");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set an OAM Action entry. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] id -
*   Endpoint id, in which to set the action
*   \param [in] action -
*   Pointer to the structure, containing information about the
*   Action entry.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
bcm_dnx_oam_endpoint_action_set(
    int unit,
    bcm_oam_endpoint_t id,
    bcm_oam_endpoint_action_t * action)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_endpoint_action_set not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Enable registration of OAM events.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] event_types -
*   Enable registration of these event_types
*   \param [in] cb -
*   Call back for the oam events.
*   \param [in] user_data -
*   Pointer to user data which will be returned when
*   callback is invoked
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - invalid event_type
*   shr_error_exists - event already registered
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * Warmboot is not supported. Application has to
*   * re-register events after warmboot.
* \see
*   * \ref bcm_dnx_oam_event_register
*/
shr_error_e
bcm_dnx_oam_event_register(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_event_cb cb,
    void *user_data)
{
    bcm_oam_event_type_t event_i;

    SHR_FUNC_INIT_VARS(unit);

    if (!(DNX_OAM_EVENT_TYPE_VERIFY(event_types)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Event type not supported");
    }

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++)
    {
        if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i))
        {
            if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb))
            {
                /*
                 * A different calblack is already registered for this event. Return error
                 */
                SHR_ERR_EXIT(_SHR_E_EXISTS, "EVENT %d already has a registered callback ", event_i);
            }
            _g_oam_event_cb[unit][event_i] = cb;
            _g_oam_event_ud[unit][event_i] = user_data;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Disable registration of OAM events.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] event_types -
*   Disable registration of these event_types
*   \param [in] cb -
*   Call back for the oam events.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_event_unregister
*/
shr_error_e
bcm_dnx_oam_event_unregister(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_event_cb cb)
{
    bcm_oam_event_type_t event_i;

    SHR_FUNC_INIT_VARS(unit);

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++)
    {
        if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i))
        {
            if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb))
            {
                /*
                 * A different calblack is registered for this event. Return error
                 */
                SHR_ERR_EXIT(_SHR_E_EXISTS, "A different callback is registered for %d ", event_i);
            }
            _g_oam_event_cb[unit][event_i] = NULL;
            _g_oam_event_ud[unit][event_i] = NULL;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize OAM. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
bcm_dnx_oam_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_init not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add an OAM Loopback entry. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] loopback_ptr  -
*   Pointer to the structure, containing information about the
*   Looback entry
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_loopback_delete
*   * \ref bcm_dnx_oam_loopback_get
*/
shr_error_e
bcm_dnx_oam_loopback_add(
    int unit,
    bcm_oam_loopback_t * loopback_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_loopback_add not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete an OAM Loopback entry. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] loopback_ptr  -
*   Pointer to the structure, containing information about the
*   Looback entry
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_loopback_add
*   * \ref bcm_dnx_oam_loopback_get
*/
shr_error_e
bcm_dnx_oam_loopback_delete(
    int unit,
    bcm_oam_loopback_t * loopback_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_loopback_delete not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an OAM Loopback entry. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [out] loopback_ptr  -
*   Pointer to the structure, containing information about the
*   Looback entry. The information from the Get procedure will
*   be written in it.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_loopback_add
*   * \ref bcm_dnx_oam_loopback_delete
*/
shr_error_e
bcm_dnx_oam_loopback_get(
    int unit,
    bcm_oam_loopback_t * loopback_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_loopback_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *   get the field from channel type table
 *  \param [in] unit  -
 *    Number of hardware unit used..
 *  \param [in] index  -
 *    Index in channel type table. range according to channel
 *    type table size
 *  \param [in] field_type  -
 *    field to retrieve from table. acceptablr values:
 *    DBAL_FIELD_CHANNEL_TYPE, DBAL_FIELD_FORMAT_TYPE &
 *    DBAL_FIELD_VALID
 *  \param [out] field_val-
 *    The field value we get from entry in channel type table.
 *  \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 *  \remark
 *    None
 *  \see 
 *
 */
static shr_error_e
dnx_oam_channel_type_rx_get_field(
    int unit,
    int index,
    dbal_fields_e field_type,
    uint32 *field_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(field_val, _SHR_E_PARAM, "field val");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_CHANNEL_TYPE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, field_type, INST_SINGLE, field_val);

    /** Preforming the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *   get the field from channel type table
 *  \param [in] unit  -
 *    Number of hardware unit used..
 *  \param [in] index  -
 *    Index in channel type table. range according to channel
 *    type table size
 *  \param [out] channel_type-
 *    The channel_type value we get from entry in channel type
 *    table.
 *  \param [out] format_type-
 *    The format_type value we get from entry in channel type
 *    table.
 *  \param [out] packet_is_oam -
 *  The packet-is-oam value we get
 *    from entry in channel type table.
 *  \param [out] valid-
 *    The valid value we get from entry in channel type table.
 *  \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 *  \remark
 *    None
 *  \see 
 *
 */
static shr_error_e
dnx_oam_channel_type_rx_get_all_fields(
    int unit,
    int index,
    uint32 *channel_type,
    uint32 *format_type,
    uint32 *packet_is_oam,
    uint32 *valid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(channel_type, _SHR_E_PARAM, "channel type");
    SHR_NULL_CHECK(format_type, _SHR_E_PARAM, "format type");
    SHR_NULL_CHECK(valid, _SHR_E_PARAM, "valid");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_CHANNEL_TYPE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);

    /** Setting pointers value to receive the fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Preforming the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CHANNEL_TYPE, INST_SINGLE, channel_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FORMAT_TYPE, INST_SINGLE, format_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CHANNEL_TYPE_PACKET_IS_OAM, INST_SINGLE, packet_is_oam));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, valid));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get the number of entries in oam channel type and pemla oam
 *   tables, compare their sizes and use the smaller one.
 *  \param [in] unit  -
 *    Number of hardware unit used.
 *  \param [out] number_of_entries  -
 *    number of entries of oam_channel_type_table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see 
 *
 */
static shr_error_e
dnx_oam_channel_type_table_get_number_of_entries(
    int unit,
    int *number_of_entries)
{
    int table_oam_channel_type, table_roam_cfg;
    CONST dbal_logical_table_t *table;
    CONST dbal_logical_table_t *table2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(number_of_entries, _SHR_E_PARAM, "number of entries");

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, DBAL_TABLE_OAM_CHANNEL_TYPE, &table));
    table_oam_channel_type = table->max_capacity;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, DBAL_TABLE_PEMLA_OAM, &table2));
    /** 6 field channel type value and 6 for layer protocol value*/
    table_roam_cfg = table2->max_nof_result_fields / 2;

    /** Compare and use the the smaller size */
    if (table_roam_cfg <= table_oam_channel_type)
    {
        *number_of_entries = table_roam_cfg;
    }
    else
    {
        *number_of_entries = table_oam_channel_type;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *   get a list of custom channel types in  channel_type for
 *   a given OAM Format type. This function gets the RX side.
 * \param [in] unit  - 
 *   Number of hardware unit used..
 * \param [in] expected_format_type  - 
 *   OAM format type configured for all values.
 * \param [in] num_values -
 *    Number of values present in list_of_values_array.
 * \param [out] list_of_values -
 *    List of custom valus that will contain the channel type
 *    returned from function
 * \param [out] value_count -
 *    Pointer to the number of existing values. meaning the
 *    actaul number of values we have in channel_type table
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see 
 *
 */
static shr_error_e
dnx_oam_channel_type_rx_get_table(
    int unit,
    dbal_enum_value_field_oam_format_type_e expected_format_type,
    int num_values,
    int *list_of_values,
    int *value_count)
{
    uint32 channel_type_table_index, occupied_entry, entry_format_type;
    int val_index = 0, number_of_entries = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(list_of_values, _SHR_E_PARAM, "list of values");
    SHR_NULL_CHECK(value_count, _SHR_E_PARAM, "value count");

    SHR_IF_ERR_EXIT(dnx_oam_channel_type_table_get_number_of_entries(unit, &number_of_entries));

    for (channel_type_table_index = 0; channel_type_table_index < number_of_entries; channel_type_table_index++)
    {
        occupied_entry = 0;
        /** Break if we have exhausted the list of values */
        if (val_index >= num_values)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_get_field
                        (unit, channel_type_table_index, DBAL_FIELD_VALID, &occupied_entry));
        /*
         * This means the entry is occupid and we need to check if this is a format for retrieving the channel type 
         */
        if (occupied_entry == 1)
        {
            SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_get_field
                            (unit, channel_type_table_index, DBAL_FIELD_FORMAT_TYPE, &entry_format_type));
            /** format type required for retrieving the channel type */
            if (entry_format_type == expected_format_type)
            {
                SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_get_field
                                (unit, channel_type_table_index, DBAL_FIELD_CHANNEL_TYPE,
                                 (uint32 *) (&(list_of_values[val_index]))));
                val_index++;
            }
        }
    }
    *value_count = val_index;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get the relavent format type for the mpls tp channel type
 *  input.
 * \param [in] unit  -
 *    Relevant unit.
 * \param [in] channel_type  -
 *    The type of mpls tp channel.
 *    values can be: bcmOamMplsTpChannelPweBfd,
 *    bcmOamMplsTpChannelPweonoam or bcmOamMplsTpChannelY1731
 * \param [out] format_type -
 *    OAM format type that will be used in channel type table .
 *    values can be: DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_Y1731 or
 *    DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_RAW
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark 
 *     bcmOamMplsTpChannelPweonoam and 
 *     bcmOamMplsTpChannelY1731 map to same format type
 * \see
 */
static shr_error_e
dnx_oam_mpls_tp_channel_type_to_format_type(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    dbal_enum_value_field_oam_format_type_e * format_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(format_type, _SHR_E_PARAM, "format type");

    switch (channel_type)
    {
        case bcmOamMplsTpChannelPweonoam:
        case bcmOamMplsTpChannelY1731:
        {
            *format_type = DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_Y1731;
            break;
        }
        case bcmOamMplsTpChannelPweBfd:
        {

            *format_type = DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_RAW;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Unknown channel type, supported channel types  are:"
                         " bcmOamMplsTpChannelPweonoam, bcmOamMplsTpChannelY1731 " " and bcmOamMplsTpChannelPweBfd \n");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get a list of custom channel types in  channel_type for a
 *   given OAM Format type. This function sets the RX side.
 * \param [in] unit  -
 *    Relevant unit.
 * \param [in] channel_type  - 
 *   Type of the mpls tp channel.
 * \param [in] num_values - 
 *   Number of values to get with the Get procedure.
 * \param [out] list_of_values - 
 *   Pointer to a list in which to
 *   write the information from the Get procedure
 *  \param [out] value_count -
 *    Pointer to the number of existing values
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *     bcmOamMplsTpChannelPweonoam and bcmOamMplsTpChannelY1731
 *     map to same format type and thereofre the get function
 *     will return custom channel types for both
 *     bcm_oam_mpls_tp_channel_type fields
 * \see 
 *  *  bcm_dnx_oam_mpls_tp_channel_type_rx_set
 *  *  bcm_dnx_oam_mpls_tp_channel_type_tx_get
 *  *  bcm_dnx_oam_mpls_tp_channel_type_tx_set 
 */
shr_error_e
bcm_dnx_oam_mpls_tp_channel_type_rx_get(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    int num_values,
    int *list_of_values,
    int *value_count)
{
    dbal_enum_value_field_oam_format_type_e format_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Get format type accrding to channel type */
    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_channel_type_to_format_type(unit, channel_type, &format_type));
    /** Get custom channel types values for given  format type */
    SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_get_table(unit, format_type, num_values, list_of_values, value_count));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set an entry in tables with OAM channel type and OAM format
 * \param [in] unit  - 
 *   Number of hardware unit used..
 * \param [in] channel_type_table_index  - 
 *   index in channel type table.
 * \param [in] format_type - 
 *   OAM format type value  that should be added to entry.
 * \param [in] channel_type - 
 *   the custom channel type value that should be added to entry
 * \param [in] packet_is_oam - 
 *   the packet-is-oam value that should be added to entry
 * \param [in] valid - 
 *   valid field value that should be added to entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 *   * None
 * \remark
 *   None
 * \see 
 *
 */
static shr_error_e
dnx_oam_channel_type_rx_set_entry(
    int unit,
    int channel_type_table_index,
    dbal_enum_value_field_oam_format_type_e format_type,
    int channel_type,
    int packet_is_oam,
    int valid)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_layer_types_e layer_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle of  OAM CHANNEL TYPE table  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_CHANNEL_TYPE, &entry_handle_id));

    /** Setting key field according to index */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, channel_type_table_index);

    /** Setting value field  channel_type  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL_TYPE, INST_SINGLE, channel_type);

    /** Setting value field  format_type  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORMAT_TYPE, INST_SINGLE, format_type);

    /** Setting value field  packet_is_oam */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL_TYPE_PACKET_IS_OAM, INST_SINGLE,
                                 packet_is_oam);

    /** Setting value field  to be VALID  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, valid);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Taking a handle of PEMLA_OAM table  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));

    /** Used to map oam type to channel_type*/
    if (format_type == DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_Y1731)
    {
        layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731;
    }
    else
    {
        layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_G_ACH_CHANNEL_TYPE_VALUE_INDEX(channel_type_table_index), INST_SINGLE,
                                 channel_type);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_G_ACH_LAYER_PROTOCOL_VALUE_INDEX(channel_type_table_index), INST_SINGLE,
                                 layer_type);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set a list of custom channel types in  channel_type for a
 *   given OAM Format type. This function sets the RX side.
 *   function goes through the  entries in table, finds a empty 
 *   entry and sets a value from the user in the empty entry.
 *   The user can give a list of values, so function stops going
 *   through the table when all values in the list from user was
 *   set in empty entries or if we went through all the entries
 *   in tables. If there are not enough empty entries in table,
 *   the function will return an error.
 * \param [in] unit  -
 *   Number of hardware unit used..
 * \param [in] format_type  -
 *   OAM format type configured for all values.
 * \param [in] num_values -
 *   Number of values present in list_of_values_array.
 * \param [in] list_of_values -
 *   List of custom valus that will be
 *   configured for channel type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   in order not to reduce performance there is no check if
 *   entry already exists in HW, Meaning if user tries to set the
 *   same entry twice, it will occupy  2 entries
 * \see 
 *
 */
static shr_error_e
dnx_oam_channel_type_rx_set_table(
    int unit,
    dbal_enum_value_field_oam_format_type_e format_type,
    int num_values,
    int *list_of_values)
{
    uint32 channel_type_table_index, occupied_entry;
    int val_index = 0, number_of_entries = 0;
    uint8 packet_is_oam = 1;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(list_of_values, _SHR_E_PARAM, "list of values");

    SHR_IF_ERR_EXIT(dnx_oam_channel_type_table_get_number_of_entries(unit, &number_of_entries));

    for (channel_type_table_index = 0; channel_type_table_index < number_of_entries; channel_type_table_index++)
    {
        occupied_entry = 0;
        /*
         * Break if we have exhausted the list of values, meaning we set all input values 
         */
        if (val_index >= num_values)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_get_field
                        (unit, channel_type_table_index, DBAL_FIELD_VALID, &occupied_entry));
        /*
         * This means the entry is free and we can set a new entry, there is no check if entry already exists in HW,
         * Meaning if user tries to set the same entry twice, it will occupy 2 entries 
         */
        if (occupied_entry == 0)
        {
            SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_set_entry
                            (unit, channel_type_table_index, format_type, list_of_values[val_index], packet_is_oam, 1));
            val_index++;
        }
    }
    /** Did not write the all the values to table  */
    if (val_index < num_values - 1)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Not enough free entries. Number of values written, %d. Number of entries that should have been written: %d",
                     val_index + 1, num_values);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set a list of custom channel types for MPLS-TP channel types
 *   This function sets the RX side.
 * \param [in] unit  -  
 *   Number of hardware unit used..
 * \param [in] channel_type  - 
 *   Type of the mpls tp channel.
 * \param [in] num_values - 
 *   Number of values present in list_of_values_array.
 * \param [in] list_of_values - 
 *   List of custom valus that will
 *   be configured for channel type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_mpls_tp_channel_type_rx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_set
 */
shr_error_e
bcm_dnx_oam_mpls_tp_channel_type_rx_set(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    int num_values,
    int *list_of_values)
{
    dbal_enum_value_field_oam_format_type_e format_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Get format type according to channel type */
    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_channel_type_to_format_type(unit, channel_type, &format_type));
    /** set custom channel types and format with values in list */
    SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_set_table(unit, format_type, num_values, list_of_values));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Clear entry from oam channel type and twofield from pemla
 *   oam tables, meaning set all fields in table to be
 *   zero, entry number to delete accoridng to
 *   channel_type_table_index input. function will return Zero
 *   if no error was detected, Negative if error was detected.
 */
static shr_error_e
dnx_oam_mpls_tp_channel_type_rx_clear_entry(
    int unit,
    int channel_type_table_index)
{
    uint32 entry_handle_id, channel_type = 0, format_type = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_CHANNEL_TYPE, &entry_handle_id));

    /** Setting key fields according to index */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, channel_type_table_index);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Taking a handle of PEMLA_OAM table  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));

    /** Set Channel_type and layer_protocol values to zero*/
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_G_ACH_CHANNEL_TYPE_VALUE_INDEX(channel_type_table_index), INST_SINGLE,
                                 channel_type);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_G_ACH_LAYER_PROTOCOL_VALUE_INDEX(channel_type_table_index), INST_SINGLE,
                                 format_type);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   delete a list of custom channel types in  channel_type for
 *   a given OAM Format type. This function sets the RX side.
 * \param [in] unit  -
 *   Number of hardware unit used..
 * \param [in] format_type  -
 *   OAM format type configured for all values.
 * \param [in] num_values -
 *   Number of values present in list_of_values_array.
 * \param [in] list_of_values -
 *   List of custom valus that will be
 *   configured for channel type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see 
 */
static shr_error_e
dnx_oam_channel_type_rx_delete_table(
    int unit,
    dbal_enum_value_field_oam_format_type_e format_type,
    int num_values,
    int *list_of_values)
{
    uint32 channel_type_table_index, occupied_entry, entry_format_type, entry_channel_type, packet_is_oam;
    int val_index, match = 0, number_of_entries = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(list_of_values, _SHR_E_PARAM, "list of values");

    SHR_IF_ERR_EXIT(dnx_oam_channel_type_table_get_number_of_entries(unit, &number_of_entries));

    /*
     * nested loop because For each value in list, need to find a value in table first loop going through the list 
     */
    for (val_index = 0; val_index < num_values; val_index++)
    {
        match = 0;
        /** second loop going through the table */
        for (channel_type_table_index = 0; channel_type_table_index < number_of_entries; channel_type_table_index++)
        {
            occupied_entry = 0;
            SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_get_all_fields
                            (unit, channel_type_table_index, &entry_channel_type, &entry_format_type, &packet_is_oam,
                             &occupied_entry));
            /*
             * This means the entry is occupied and we need to check if entry should be deleted 
             */
            if (occupied_entry == 1)
            {
                /** format type & channel type are equal to types  to delete */
                if (entry_format_type == format_type && entry_channel_type == list_of_values[val_index])
                {
                    /** delete entry, meaning set all fields to be zero */
                    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_channel_type_rx_clear_entry(unit, channel_type_table_index));
                    match = 1;
                    break;
                }
            }
        }
        /*
         * In case one of the list values is not found the API will remove all entires till the error of no match 
         */
        if (match == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "OAM channel type = %d to delete does not exist in HW , number of values deleted %d",
                         list_of_values[val_index], val_index);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete mpls tp channel types according to vlaues in list - rx
 *  side.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] channel_type  - 
 *   Type of the mpls tp channel.
 * \param [in] num_values - 
 *   Number of values to delete.
 * \param [in] list_of_values -
 *    Pointer to a list with valus to delete 
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_mpls_tp_channel_type_rx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_set
*/
shr_error_e
bcm_dnx_oam_mpls_tp_channel_type_rx_delete(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    int num_values,
    int *list_of_values)
{
    dbal_enum_value_field_oam_format_type_e format_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Get format type accrding to channel type */
    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_channel_type_to_format_type(unit, channel_type, &format_type));
    /** Delete entries according to channel types in list */
    SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_delete_table(unit, format_type, num_values, list_of_values));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM opcode map. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] opcode  -
*    Opcode to read.
*   \param [out] profile -
*    Profile in which to write the information from the Get
*    procedure.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_opcode_map_set
*/
shr_error_e
bcm_dnx_oam_opcode_map_get(
    int unit,
    int opcode,
    int *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_opcode_map_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set OAM opcode map. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] opcode  -
*    Opcode to read.
*   \param [in] profile -
*    Profile containing the information to set.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_opcode_map_get
*/
shr_error_e
bcm_dnx_oam_opcode_map_set(
    int unit,
    int opcode,
    int profile)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_opcode_map_set not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Enable registration of OAM performance events. This function
*  is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] event_types  -
*    Event types to be enabled.
*   \param [in] cb -
*    Call-back for the oam performance events.
*   \param [in] user_data -
*    Pointer to user data, containing the information from the
*    Get procedure.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_performance_event_unregister
*/
shr_error_e
bcm_dnx_oam_performance_event_register(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_performance_event_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_performance_event_register not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Disable registration of OAM performance events. This function
*  is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] event_types  -
*    Event types to be disabled.
*   \param [in] cb -
*    Call-back for the oam performance events.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_performance_event_register
*/
shr_error_e
bcm_dnx_oam_performance_event_unregister(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_performance_event_cb cb)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_performance_event_unregister not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add OAM sd sf alarm detection. This function is not
*  supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] sd_sf_ptr  -
*    Pointer to the sd_sf structure containing the information
*    to be added.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_sd_sf_detection_delete
*   * \ref bcm_dnx_oam_sd_sf_detection_get
*/
shr_error_e
bcm_dnx_oam_sd_sf_detection_add(
    int unit,
    bcm_oam_sd_sf_detection_t * sd_sf_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_sd_sf_detection_add not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete OAM sd sf alarm detection. This function is not
*  supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] sd_sf_ptr  -
*    Pointer to the sd_sf structure containing the information
*    to be deleted.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_sd_sf_detection_add
*   * \ref bcm_dnx_oam_sd_sf_detection_get
*/
shr_error_e
bcm_dnx_oam_sd_sf_detection_delete(
    int unit,
    bcm_oam_sd_sf_detection_t * sd_sf_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_sd_sf_detection_delete not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM sd sf alarm detection. This function is not
*  supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [out] sd_sf_ptr  -
*    Pointer to the sd_sf structure containing the information
*    to be gotten. The information from the Get procedure will
*    be written in this structure
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_sd_sf_detection_add
*   * \ref bcm_dnx_oam_sd_sf_detection_delete
*/
shr_error_e
bcm_dnx_oam_sd_sf_detection_get(
    int unit,
    bcm_oam_sd_sf_detection_t * sd_sf_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_sd_sf_detection_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add OAM tst rx entry . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] tst_ptr  -
*    Pointer to the tst structure containing the information.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_tst_rx_delete
*   * \ref bcm_dnx_oam_tst_rx_get
*   * \ref bcm_dnx_oam_tst_tx_add
*   * \ref bcm_dnx_oam_tst_tx_delete
*   * \ref bcm_dnx_oam_tst_tx_get
*/
shr_error_e
bcm_dnx_oam_tst_rx_add(
    int unit,
    bcm_oam_tst_rx_t * tst_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_tst_rx_add not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete OAM tst rx entry . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] tst_ptr  -
*    Pointer to the tst structure containing the information.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_tst_rx_add
*   * \ref bcm_dnx_oam_tst_rx_get
*   * \ref bcm_dnx_oam_tst_tx_add
*   * \ref bcm_dnx_oam_tst_tx_delete
*   * \ref bcm_dnx_oam_tst_tx_get
*/
shr_error_e
bcm_dnx_oam_tst_rx_delete(
    int unit,
    bcm_oam_tst_rx_t * tst_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_tst_rx_delete not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM tst rx entry . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [out] tst_ptr  -
*    Pointer to the tst structure containing the information.
*    The information from the get procedure will be written
*    here.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_tst_rx_add
*   * \ref bcm_dnx_oam_tst_rx_delete
*   * \ref bcm_dnx_oam_tst_tx_add
*   * \ref bcm_dnx_oam_tst_tx_delete
*   * \ref bcm_dnx_oam_tst_tx_get
*/
shr_error_e
bcm_dnx_oam_tst_rx_get(
    int unit,
    bcm_oam_tst_rx_t * tst_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_tst_rx_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add OAM tst tx entry . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] tst_ptr  -
*    Pointer to the tst structure containing the information
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_tst_rx_add
*   * \ref bcm_dnx_oam_tst_rx_delete
*   * \ref bcm_dnx_oam_tst_rx_get
*   * \ref bcm_dnx_oam_tst_tx_delete
*   * \ref bcm_dnx_oam_tst_tx_get
*/
shr_error_e
bcm_dnx_oam_tst_tx_add(
    int unit,
    bcm_oam_tst_tx_t * tst_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_tst_tx_add not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete OAM tst tx entry . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] tst_ptr  -
*    Pointer to the tst structure containing the information
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_tst_rx_add
*   * \ref bcm_dnx_oam_tst_rx_delete
*   * \ref bcm_dnx_oam_tst_rx_get
*   * \ref bcm_dnx_oam_tst_tx_add
*   * \ref bcm_dnx_oam_tst_tx_get
*/
shr_error_e
bcm_dnx_oam_tst_tx_delete(
    int unit,
    bcm_oam_tst_tx_t * tst_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_tst_tx_delete not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM tst tx entry . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [out] tst_ptr  -
*    Pointer to the tst structure containing the information
*    The information from the Get procedure will be written
*    here.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_tst_rx_add
*   * \ref bcm_dnx_oam_tst_rx_delete
*   * \ref bcm_dnx_oam_tst_rx_get
*   * \ref bcm_dnx_oam_tst_tx_add
*   * \ref bcm_dnx_oam_tst_tx_delete
*/
shr_error_e
bcm_dnx_oam_tst_tx_get(
    int unit,
    bcm_oam_tst_tx_t * tst_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_tst_tx_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add OAM Y1711 alarm . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] alarm_ptr  -
*    Pointer to the alarm structure containing the information
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_y_1711_alarm_delete
*   * \ref bcm_dnx_oam_y_1711_alarm_get
*/
shr_error_e
bcm_dnx_oam_y_1711_alarm_add(
    int unit,
    bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_y_1711_alarm_add not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete OAM Y1711 alarm . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] alarm_ptr  -
*    Pointer to the alarm structure containing the information
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_y_1711_alarm_add
*   * \ref bcm_dnx_oam_y_1711_alarm_get
*/
shr_error_e
bcm_dnx_oam_y_1711_alarm_delete(
    int unit,
    bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_y_1711_alarm_delete not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM Y1711 alarm . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [out] alarm_ptr  -
*    Pointer to the alarm structure containing the information
*    The information from the Get procedure will be written
*    here.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_y_1711_alarm_add
*   * \ref bcm_dnx_oam_y_1711_alarm_delete
*/
shr_error_e
bcm_dnx_oam_y_1711_alarm_get(
    int unit,
    bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_y_1711_alarm_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification that is the same for dnx_oam_my_cfm_mac_get and dnx_oam_my_cfm_mac_add
 */
static shr_error_e
dnx_my_cfm_mac_verify(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac,
    int32 check_mac_unicast)
{
    uint32 legal_flags, num_of_cores, number_of_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(my_cfm_mac, _SHR_E_PARAM, "my_cfm_mac");

    /** Currenly, only one flag is defined */
    legal_flags = DNX_OAM_CLASSIFIER_INGRESS;

    if (flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: flags=0x%08X, only DNX_OAM_CLASSIFIER_INGRESS flag is allowed\n", flags);
    }

    /** Get number of cores for this device */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);

    if (((core_id >= num_of_cores) || (core_id < 0)) && (core_id != _SHR_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for core_id %d, total number of cores %d\n", core_id, num_of_cores);
    }

    /** How many ports does this device have? */
    number_of_ports = dnx_data_port.general.nof_pp_ports_get(unit);

    if (port_id >= number_of_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for port_id 0x%08X, must be between 0 and 0x%08X\n",
                     port_id, number_of_ports);
    }

    /** Only unicast mac addresses are relevant */
    if ((check_mac_unicast == TRUE) && !OAM_CLASSIFIER_CHECK_MAC_IS_UNICAST(my_cfm_mac))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MAC %02X:%02X:%02X:%02X:%02X:%02X; must be unicast\n",
                     SAL_MAC_ADDR_LIST(my_cfm_mac));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: for a given core and port, find the
 *  first LSB marked as "valid."
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 * \param [in] core_id - ID of core to which accessed table 
 *        belongs.  Current possible values: 0, 1, or
 *        _SHR_CORE_ALL (to access both tables.)
 * \param [in] port_id - number of configured port
 * \param [out] first_lsb - pointer to which output value is 
 *        written.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_my_cfm_mac_find_valid_lsb(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    uint32 *first_lsb)
{
    uint32 entry_handle_id;
    bcm_pbmp_t lsb_bmp;
    dbal_tables_e table_id;
    int bit_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** 
     *  No need to verify input parameters - already done by
     *  calling function
     */

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_PP_PORT;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_PP_PORT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set fixed parts of the key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get bitmap */
    BCM_PBMP_CLEAR(lsb_bmp);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_LSB_MAP, INST_SINGLE, lsb_bmp.pbits);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Iterate over all possible values   */
    BCM_PBMP_ITER(lsb_bmp, bit_index)
    {
        /** Non-zero bit found - use index */
        break;
    }

    /** Return value */
    *first_lsb = bit_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: read the MSB from a my_mac_cfm entry
 *  to the input array.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 * \param [in] core_id - ID of core to which accessed table 
 *        belongs.  Current possible values: 0, 1, or
 *        _SHR_CORE_ALL (to access both tables.)
 * \param [in] port_id - number of configured port
 * \param [out] input_msb - pointer to which result is written.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_my_cfm_mac_read_msb(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t input_msb)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    uint8 mac_msb[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** 
     *  No need to verify input parameters - already done by
     *  calling function
     */

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_PP_PORT;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_PP_PORT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get value */
    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_MSB, INST_SINGLE, mac_msb);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Copy to provided pointer */
    sal_memcpy(input_msb, mac_msb, sizeof(bcm_mac_t) - 1);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: compare the MSB from a my_mac_cfm entry
 *  to the input array.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 * \param [in] core_id - ID of core to which accessed table 
 *        belongs.  Current possible values: 0, 1, or
 *        _SHR_CORE_ALL (to access both tables.)
 * \param [in] port_id - number of configured port
 * \param [in] input_msb - Value to which configured MSB is 
 *        compared.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_my_cfm_mac_compare_msb(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t input_msb)
{
    bcm_mac_t read_msb;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dnx_my_cfm_mac_read_msb(unit, flags, core_id, port_id, read_msb);
    if (sal_memcmp(read_msb, input_msb, UTILEX_PP_MAC_ADDRESS_NOF_U8 - 1) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal MAC %02X:%02X:%02X:%02X:%02X:%02X; no match for existing MSB "
                     "%02X:%02X:%02X:%02X:%02X\n", SAL_MAC_ADDR_LIST(input_msb), read_msb[0],
                     read_msb[1], read_msb[2], read_msb[3], read_msb[4]);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_internal.h for function description
 */
shr_error_e
dnx_oam_my_cfm_mac_get(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac)
{
    uint32 lsb;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_my_cfm_mac_verify(unit, flags, core_id, port_id, my_cfm_mac, FALSE));

    /** First get MSB - one value per port */
    SHR_IF_ERR_EXIT(dnx_my_cfm_mac_read_msb(unit, flags, core_id, port_id, my_cfm_mac));

    /** Can you find a valid LSB?  If so, return the first valid
     *  LSB found */
    SHR_IF_ERR_EXIT(dnx_my_cfm_mac_find_valid_lsb(unit, flags, core_id, port_id, &lsb));
    if (lsb > UTILEX_U8_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: no legal my_cfm_mac found in table\n");
    }
    my_cfm_mac[MAC_LSB_INDEX] = lsb;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_internal.h for function description
 */
shr_error_e
dnx_oam_my_cfm_mac_add(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac)
{
    uint32 entry_handle_id, first_lsb, valid_lsb;
    dbal_tables_e table_id;
    bcm_pbmp_t lsb_bmp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_my_cfm_mac_verify(unit, flags, core_id, port_id, my_cfm_mac, TRUE));

    /** Get tables */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_PP_PORT;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_PP_PORT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Key for table */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get the LSB map; we'll need for reading and writing   */
    BCM_PBMP_CLEAR(lsb_bmp);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_LSB_MAP, INST_SINGLE, lsb_bmp.pbits);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Is there a valid my_mac_cfm? */
    SHR_IF_ERR_EXIT(dnx_my_cfm_mac_find_valid_lsb(unit, flags, core_id, port_id, &first_lsb));
    if (first_lsb <= UTILEX_U8_MAX)
    {
        /** 
         *  Yes there is; MSB specified must match the one already in
         *  the table
         */
        SHR_IF_ERR_EXIT(dnx_my_cfm_mac_compare_msb(unit, flags, core_id, port_id, my_cfm_mac));

        /** 
         *  There is/are (a) valid LSB(s) and the MSB matches.  Is
         *  the specified LSB already valid?
         */
        valid_lsb = BCM_PBMP_MEMBER(lsb_bmp, my_cfm_mac[MAC_LSB_INDEX]);
        if (valid_lsb == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MAC address %02X:%02X:%02X:%02X:%02X:%02X already in table\n",
                         SAL_MAC_ADDR_LIST(my_cfm_mac));
        }
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        /** No there isn't; write the MSB to the table  */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_MSB, INST_SINGLE, my_cfm_mac);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Set specified LSB value as valid */
    BCM_PBMP_PORT_ADD(lsb_bmp, my_cfm_mac[MAC_LSB_INDEX]);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_LSB_MAP, INST_SINGLE, lsb_bmp.pbits);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_internal.h for function description
 */
shr_error_e
dnx_oam_my_cfm_mac_delete(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac)
{
    uint32 entry_handle_id, first_lsb, valid_lsb;
    dbal_tables_e table_id;
    bcm_pbmp_t lsb_bmp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_my_cfm_mac_verify(unit, flags, core_id, port_id, my_cfm_mac, TRUE));

    /** Get tables */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_PP_PORT;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_PP_PORT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Keys for entry   */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Is there a valid my_mac_cfm? */
    SHR_IF_ERR_EXIT(dnx_my_cfm_mac_find_valid_lsb(unit, flags, core_id, port_id, &first_lsb));
    if (first_lsb > UTILEX_U8_MAX)
    {
        /** No there isn't - operation illegal */
        SHR_ERR_EXIT(_SHR_E_PARAM, "This port has no valid my_mac_cfm\n");
    }

    /** Is there an LSB to delete? */
    BCM_PBMP_CLEAR(lsb_bmp);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_LSB_MAP, INST_SINGLE, lsb_bmp.pbits);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    valid_lsb = BCM_PBMP_MEMBER(lsb_bmp, my_cfm_mac[MAC_LSB_INDEX]);
    if (valid_lsb == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MAC address %02X:%02X:%02X:%02X:%02X:%02X not found in table\n",
                     SAL_MAC_ADDR_LIST(my_cfm_mac));
    }

    /** LSB found; does the MSB match? */
    SHR_IF_ERR_EXIT(dnx_my_cfm_mac_compare_msb(unit, flags, core_id, port_id, my_cfm_mac));

    /** Set specified LSB value as invalid */
    BCM_PBMP_PORT_REMOVE(lsb_bmp, my_cfm_mac[MAC_LSB_INDEX]);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC_LSB_MAP, INST_SINGLE, lsb_bmp.pbits);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: Form the GACH or CW given the
 *                            channel value
 * 
 * \param [in] unit - Relevant unit
 * \param [in] channel - Channel value
 * \param [out] ach_header - CW/ACH header value
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_mpls_tp_bfd_ach_header_form(
    int unit,
    uint32 *channel,
    uint32 *ach_header)
{
    /*
     * reserved, ach_ver, gach_indication are hard_coded as per RFC 
     */
    uint32 reserved = 0;
    uint32 ach_ver = 0;
    uint32 gach_indication = 1;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_ACH_HEADER, DBAL_FIELD_CHANNEL_TYPE, channel, ach_header));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_ACH_HEADER, DBAL_FIELD_RESERVED, &reserved, ach_header));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_ACH_HEADER, DBAL_FIELD_ACH_VER, &ach_ver, ach_header));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_ACH_HEADER, DBAL_FIELD_GACH_INDICATION, &gach_indication, ach_header));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: Get the channel value given the
 *                            ACH or CW
 * 
 * \param [in] unit - Relevant unit
 * \param [in] ach_header - CW/ACH header value
 * \param [out] channel - Channel value
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_mpls_tp_bfd_channel_value_get_from_ach_header(
    int unit,
    uint32 *ach_header,
    uint32 *channel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_ACH_HEADER, DBAL_FIELD_CHANNEL_TYPE, channel, ach_header));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: Verify the BFD PWE DBAL table entry
 *                            set parameters
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] index - Index to be set.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_bfd_pwe_channel_value_set_verify(
    int unit,
    uint32 index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!_BCM_OAM_DISSECT_BFD_PWE_CW_MEM_INDEX_VALID(index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid mem index %u\n", index);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: Verify the BFD PWE DBAL table entry
 *                            get parameters
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] index - Index of the DBAL table.
 * \param [in] value - Out value memory.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_bfd_pwe_channel_value_get_verify(
    int unit,
    uint32 index,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!_BCM_OAM_DISSECT_BFD_PWE_CW_MEM_INDEX_VALID(index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid mem index %u\n", index);
    }

    if (value == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Null pointer value \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_internal.h for function description
 */
shr_error_e
dnx_oam_bfd_pwe_channel_value_set(
    int unit,
    uint32 index,
    uint32 value)
{
    uint32 entry_handle_id;
    uint32 channel, ach_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_bfd_pwe_channel_value_set_verify(unit, index));

    channel = value;
    /*
     * Form the ach header 
     */
    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_bfd_ach_header_form(unit, &channel, &ach_header));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_BFD_CFG, &entry_handle_id));

    /** Keys and result for entry   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_G_ACH_HEADER, index, ach_header);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * The function gets the channel value defined at a specified index
 * for BFD PWE
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] index -
 *   Index of the BFD PWE DBAL table
 * \param [out] value -
 *   Channel value programmed at that index.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
 */
shr_error_e
dnx_oam_bfd_pwe_channel_value_get(
    int unit,
    uint32 index,
    uint32 *value)
{
    uint32 entry_handle_id;
    uint32 ach_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_bfd_pwe_channel_value_get_verify(unit, index, value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_BFD_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_G_ACH_HEADER, index, &ach_header);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_bfd_channel_value_get_from_ach_header(unit, &ach_header, value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: Verify the Y1731 MPLS TP GACH DBAL table entry
 *                            get parameters
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] value - Out value memory.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_y1731_mpls_tp_channel_value_get_verify(
    int unit,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (value == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Null pointer value \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_internal.h for function description
 */
shr_error_e
dnx_oam_y1731_mpls_tp_channel_value_set(
    int unit,
    uint32 value)
{
    uint32 entry_handle_id;
    uint32 channel, ach_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    channel = value;
    /*
     * Form the ach header 
     */
    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_bfd_ach_header_form(unit, &channel, &ach_header));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    /** Keys and result for entry   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GACH, INST_SINGLE, ach_header);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * The function gets the channel value defined
 * for Y1731 MPLS TP
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] value -
 *   GACH programmed.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
 */
shr_error_e
dnx_oam_y1731_mpls_tp_channel_value_get(
    int unit,
    uint32 *value)
{
    uint32 entry_handle_id;
    uint32 ach_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_y1731_mpls_tp_channel_value_get_verify(unit, value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GACH, INST_SINGLE, &ach_header);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_bfd_channel_value_get_from_ach_header(unit, &ach_header, value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function: Verify the Y1731 PWE GACH DBAL table entry
 *                            get parameters
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] value - GACH value out memory
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_y1731_pwe_channel_value_get_verify(
    int unit,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (value == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Null pointer value \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_internal.h for function description
 */
shr_error_e
dnx_oam_y1731_pwe_channel_value_set(
    int unit,
    uint32 value)
{
    uint32 entry_handle_id;
    uint32 channel, ach_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    channel = value;
    /*
     * Form the ach header 
     */
    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_bfd_ach_header_form(unit, &channel, &ach_header));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    /** Keys and result for entry   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Y1731_PWE_GACH, INST_SINGLE, ach_header);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * The function gets the channel value defined
 * for Y1731 PWE
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] value -
 *   Channel value programmed.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
 */
shr_error_e
dnx_oam_y1731_pwe_channel_value_get(
    int unit,
    uint32 *value)
{
    uint32 entry_handle_id;
    uint32 ach_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_y1731_pwe_channel_value_get_verify(unit, value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_Y1731_PWE_GACH, INST_SINGLE, &ach_header);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_bfd_channel_value_get_from_ach_header(unit, &ach_header, value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get a channel type for MPLS-TP CW/ACH
 *   This function gets the TX side.
 * \param [in] unit  -  
 *   Number of hardware unit used..
 * \param [in] channel_type  - 
 *   Type of the mpls tp channel.
 * \param [out] value - 
 *   value configured for channel type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_mpls_tp_channel_type_rx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_set
 */
shr_error_e
bcm_dnx_oam_mpls_tp_channel_type_tx_get(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (channel_type)
    {
        case bcmOamMplsTpChannelPweBfd:
            /*
             * We always use index 0. We do not write into other indexes. 
             */
            SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_get(unit, 0, (uint32 *) value));
            break;
        case bcmOamMplsTpChannelPweonoam:
            SHR_IF_ERR_EXIT(dnx_oam_y1731_mpls_tp_channel_value_get(unit, (uint32 *) value));
            break;
        case bcmOamMplsTpChannelY1731:
            SHR_IF_ERR_EXIT(dnx_oam_y1731_pwe_channel_value_get(unit, (uint32 *) value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid channel_type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set a channel type for MPLS-TP CW/ACH
 *   This function sets the TX side.
 * \param [in] unit  -  
 *   Number of hardware unit used..
 * \param [in] channel_type  - 
 *   Type of the mpls tp channel.
 * \param [in] value - 
 *   value to be configured for channel type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_mpls_tp_channel_type_rx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_set
 */
shr_error_e
bcm_dnx_oam_mpls_tp_channel_type_tx_set(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (channel_type)
    {
        case bcmOamMplsTpChannelPweBfd:
            /*
             * We always use index 0. We do not write into other indexes. 
             */
            SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 0, value));
            break;
        case bcmOamMplsTpChannelPweonoam:
            SHR_IF_ERR_EXIT(dnx_oam_y1731_mpls_tp_channel_value_set(unit, value));
            break;
        case bcmOamMplsTpChannelY1731:
            SHR_IF_ERR_EXIT(dnx_oam_y1731_pwe_channel_value_set(unit, value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid channel_type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete mpls tp channel types according to vlaues in list - tx
 *  side.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] channel_type  - 
 *   Type of the mpls tp channel.
 * \param [in] value -
 *    value to delete
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_mpls_tp_channel_type_rx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_get
 *   * bcm_dnx_oam_mpls_tp_channel_type_tx_set
*/
shr_error_e
bcm_dnx_oam_mpls_tp_channel_type_tx_delete(
    int unit,
    bcm_oam_mpls_tp_channel_type_t channel_type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (channel_type)
    {
        case bcmOamMplsTpChannelPweBfd:
            /*
             * We always use index 0. We do not write into other indexes. 
             */
            SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 0, 0));
            break;
        case bcmOamMplsTpChannelPweonoam:
            SHR_IF_ERR_EXIT(dnx_oam_y1731_mpls_tp_channel_value_set(unit, 0));
            break;
        case bcmOamMplsTpChannelY1731:
            SHR_IF_ERR_EXIT(dnx_oam_y1731_pwe_channel_value_set(unit, 0));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid channel_type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
