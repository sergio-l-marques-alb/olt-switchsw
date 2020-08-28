/** \file ctest_dnx_oamp_basic.c
 * 
 * Tests for OAMP
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm/bfd.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include "ctest_dnx_oamp.h"

/** Value used as a not-so-random seed to generate values   */
#define INIT_GEN_VALUE 0x5D2C85

/**
 *  Value used to generate a sequence of values that will be the
 *  same every time the test is run
 */
#define GEN_VALUE_DELTA 0xE5

/** Generalized OAMP entry types */
#define ENTRY_TYPE_BFD 0
#define ENTRY_TYPE_CCM 1

/** Test flags */

/** Flag for additional LM entry */
#define OAM_WITH_LM_ENTRY SAL_BIT(0)

/** Flag for additional DM entry */
#define OAM_WITH_DM_ENTRY SAL_BIT(1)

/** Value of MEP ID - this bit is LSB */
#define MEP_ID_OFFSET 2

/** Bit shift from entry ID to "oam ID" */
#define FULL_ENTRY_ID_SHIFT 2

/** Bit that determines: ICC or short group name? */
#define DNX_OAMP_ICC_USED_BIT SAL_BIT(0)

/** ICC index: number of bits, and maximum value */
#define DNX_OAMP_NOF_ICC_INDEX_BITS 4
#define DNX_OAMP_MAX_ICC_INDEX SAL_UPTO_BIT(DNX_OAMP_NOF_ICC_INDEX_BITS)

/** Short MAID: number of bits, and maximum value */
#define DNX_OAMP_NOF_MAID_BITS 16
#define DNX_OAMP_MAX_MAID SAL_UPTO_BIT(DNX_OAMP_NOF_MAID_BITS)

/** Number of possible MEP types for CCM entry */
#define NOF_CCM_MEP_TYPES 3

/** CCM interval: number of bits, and maximum value */
#define DNX_OAMP_NOF_CCM_INT_BITS 3
#define DNX_OAMP_MAX_CCM_INT SAL_UPTO_BIT(DNX_OAMP_NOF_CCM_INT_BITS)

/** TC/DP profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_TC_DP_PROFILE_BITS 3
#define DNX_OAMP_MAX_TC_DP_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_TC_DP_PROFILE_BITS)

/** System port: number of bits, and maximum value */
#define DNX_OAMP_NOF_SYSTEM_PORT_BITS 16
#define DNX_OAMP_MAX_SYSTEM_PORT SAL_UPTO_BIT(DNX_OAMP_NOF_SYSTEM_PORT_BITS)

/** PP port index: number of bits, and maximum value */
#define DNX_OAMP_NOF_PORT_INDEX_BITS 9
#define DNX_OAMP_MAX_PORT_INDEX SAL_UPTO_BIT(DNX_OAMP_NOF_PORT_INDEX_BITS)

/** PP port index division: mask for first part, and offset of second part */
#define PORT_BITS SAL_UPTO_BIT(8)
#define CORE_BIT 8

/** MDL: number of bits, and maximum value */
#define DNX_OAMP_NOF_MDL_BITS 3
#define DNX_OAMP_MAX_MDL SAL_UPTO_BIT(DNX_OAMP_NOF_MDL_BITS)

/** MEP ID: number of bits, and maximum value */
#define DNX_OAMP_NOF_MEP_ID_BITS 13
#define DNX_OAMP_MAX_MEP_ID SAL_UPTO_BIT(DNX_OAMP_NOF_MEP_ID_BITS)

/** TLV code: number of bits, and maximum value */
#define DNX_OAMP_NOF_TLV_CODE_BITS 3
#define DNX_OAMP_MAX_TLV_CODE SAL_UPTO_BIT(DNX_OAMP_NOF_TLV_CODE_BITS)

/** MEP profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_MEP_PROFILE_BITS 7
#define DNX_OAMP_MAX_MEP_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_MEP_PROFILE_BITS)

/** Number of hardware counter interfaces */
#define DNX_OAMP_NOF_COUNTER_INTERFACES 3

/** TPID index: number of bits, and maximum value */
#define DNX_OAMP_NOF_TPID_INDEX_BITS 2
#define DNX_OAMP_MAX_TPID_INDEX SAL_UPTO_BIT(DNX_OAMP_NOF_TPID_INDEX_BITS)

/** VLAN ID: number of bits, and maximum value */
#define DNX_OAMP_NOF_VID_BITS 12
#define DNX_OAMP_MAX_VID SAL_UPTO_BIT(DNX_OAMP_NOF_VID_BITS)

/** PCP DEI: number of bits, and maximum value */
#define DNX_OAMP_NOF_DEI_PCP_BITS 4
#define DNX_OAMP_MAX_DEI_PCP SAL_UPTO_BIT(DNX_OAMP_NOF_DEI_PCP_BITS)

/** Possibilities for number of VLAN tags */
#define VLAN_TAGS_NOF_OPTIONS 3

/** Source MAC address LSB: number of bits, and maximum value */
#define DNX_OAMP_NOF_SA_GEN_LSB_BITS 15
#define DNX_OAMP_MAX_SA_GEN_LSB SAL_UPTO_BIT(DNX_OAMP_NOF_SA_GEN_LSB_BITS)

/** Source MAC address MSB PROFILE: number of bits, and maximum value */
#define DNX_OAMP_NOF_SA_GEN_MSB_PROFILE_BITS 1
#define DNX_OAMP_MAX_SA_GEN_MSB_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_SA_GEN_MSB_PROFILE_BITS)

/** Bit that determines if egress interface is FEC or out-LIF */
#define FEC_OR_OUT_LIF_BIT SAL_BIT(0)

/** MPLS/PWE label: number of bits, and maximum value */
#define DNX_OAMP_NOF_LABEL_BITS 20
#define DNX_OAMP_MAX_LABEL SAL_UPTO_BIT(DNX_OAMP_NOF_LABEL_BITS)

/** MPLS/PWE TTL/EXP profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_PUSH_PROFILE_BITS 4
#define DNX_OAMP_MAX_PUSH_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_PUSH_PROFILE_BITS)

/** LMM DA profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_LMM_DA_PROFILE_BITS 7
#define DNX_OAMP_MAX_LMM_DA_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_LMM_DA_PROFILE_BITS)

/** Generation of BFD MEP type */
#define MIN_BFD_MEP_TYPE DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP
#define NOF_BFD_MEP_TYPES (DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE - MIN_BFD_MEP_TYPE)

/** BFD Tx rate profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_TX_RATE_PROFILE_BITS 3
#define DNX_OAMP_MAX_TX_RATE_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_TX_RATE_PROFILE_BITS)

/** ACH selection: number of bits, and maximum value */
#define DNX_OAMP_NOF_ACH_SEL_BITS 2
#define DNX_OAMP_MAX_ACH_SEL SAL_UPTO_BIT(DNX_OAMP_NOF_ACH_SEL_BITS)

/** Source IP profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_SRC_IP_PROFILE_BITS 8
#define DNX_OAMP_MAX_SRC_IP_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_SRC_IP_PROFILE_BITS)

/** Detect multiplier: number of bits, and maximum value */
#define DNX_OAMP_NOF_DETECT_MULT_BITS 8
#define DNX_OAMP_MAX_DETECT_MULT SAL_UPTO_BIT(DNX_OAMP_NOF_DETECT_MULT_BITS)

/** Detect multiplier: number of bits, and maximum value */
#define DNX_OAMP_NOF_INTERVAL_PROFILE_BITS 3
#define DNX_OAMP_MAX_INTERVAL_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_INTERVAL_PROFILE_BITS)

/** Number of possible values of diag field */
#define NOF_DIAG_VALUES 10

/** Flags profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_FLAGS_PROFILE_BITS 4
#define DNX_OAMP_MAX_FLAGS_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_FLAGS_PROFILE_BITS)

/** BFD endpoint state: number of bits, and maximum value */
#define DNX_OAMP_NOF_STATE_BITS 2
#define DNX_OAMP_MAX_STATE SAL_UPTO_BIT(DNX_OAMP_NOF_STATE_BITS)

/** Subnet length: number of bits, and maximum value */
#define DNX_OAMP_NOF_SUBNET_LEN_BITS 5
#define DNX_OAMP_MAX_SUBNET_LEN SAL_UPTO_BIT(DNX_OAMP_NOF_SUBNET_LEN_BITS)

/** TTL/TOS profile: number of bits, and maximum value */
#define DNX_OAMP_NOF_TTL_TOS_PROFILE_BITS 4
#define DNX_OAMP_MAX_TTL_TOS_PROFILE SAL_UPTO_BIT(DNX_OAMP_NOF_TTL_TOS_PROFILE_BITS)

/** CRPS core select:number of bits, and maximum value */
#define DNX_OAMP_NOF_CRPS_CORE_BITS 1
#define DNX_OAMP_MAX_CRPS_CORE SAL_UPTO_BIT(DNX_OAMP_NOF_CRPS_CORE_BITS)

/** Structure to keep current values of parameters   */
typedef struct
{
        /** Value used to generate all field values */
    uint32 gen_value;

        /** Options selected for the current test */
    uint32 test_flags;

} diag_dnx_oamp_semantic_test_struct;

/**
 * \brief - Write values for all fields needed to create
 *          an OAMP CCM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] for_modification - if this value is true,
 *        overwrite only fields that may be modified after the
 *        entry was created.
 * \param [out] ccm_entry_data - structure to which values
 *        are written
 * \param [in,out] test - structure that holds "global"
 *        variables for test.
 * \param [out] entry_id - index of OAMP MEP DB entry,
 *        including least significant bits for short entries.
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
diag_dnx_oamp_test_generate_ccm_params(
    int unit,
    int for_modification,
    dnx_oam_oamp_ccm_endpoint_t * ccm_entry_data,
    diag_dnx_oamp_semantic_test_struct * test,
    uint32 *entry_id)
{
    int mep_db_threshold, avail_full_entries, total_full_entries;
    int nof_counter_base_bits, max_counter_base, nof_mep_pe_profile_bits, max_mep_pe_profile, nof_cores;
    uint32 temp, num_fec_ids, num_lif_ids;

    if (!for_modification)
    {
        mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
        total_full_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
        avail_full_entries = total_full_entries - (mep_db_threshold >> FULL_ENTRY_ID_SHIFT);
        if ((avail_full_entries == 0) ||        /* If threshold set to maximum, all entries are short */
            ((mep_db_threshold > 0) && (test->test_flags == 0)
             && _SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)))
        {
                        /** Short entry */
            ccm_entry_data->flags |= DNX_OAMP_OAM_CCM_MEP_Q_ENTRY;
            *entry_id = ((test->gen_value >> MEP_ID_OFFSET) % mep_db_threshold);
            test->gen_value += GEN_VALUE_DELTA;
        }
        else
        {
                        /** Entry above threshold */
            *entry_id = mep_db_threshold +
                (((test->gen_value >> MEP_ID_OFFSET) % avail_full_entries) << FULL_ENTRY_ID_SHIFT);
            test->gen_value += GEN_VALUE_DELTA;

            if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED))
            {
                                /** LM/DM offloaded entry */
                ccm_entry_data->flags |= DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED;
                test->gen_value += GEN_VALUE_DELTA;
            }
        }

        if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_ICC_USED_BIT))
        {
            ccm_entry_data->icc_index = (test->gen_value & DNX_OAMP_MAX_ICC_INDEX);
        }
        else
        {
            ccm_entry_data->maid = (test->gen_value & DNX_OAMP_MAX_MAID);
        }

        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->mep_type = (test->gen_value % NOF_CCM_MEP_TYPES);
        test->gen_value += GEN_VALUE_DELTA;

        if (ccm_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {
            if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
            {
                ccm_entry_data->flags |= DNX_OAMP_OAM_CCM_MEP_IS_UPMEP;
            }
        }
    }

    if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN))
    {
        ccm_entry_data->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;
        test->gen_value += GEN_VALUE_DELTA;
    }

    if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL))
    {
        ccm_entry_data->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL;
        test->gen_value += GEN_VALUE_DELTA;
    }

    ccm_entry_data->ccm_interval = (test->gen_value & DNX_OAMP_MAX_CCM_INT);
    test->gen_value += GEN_VALUE_DELTA;

    ccm_entry_data->itmh_tc_dp_profile = (test->gen_value & DNX_OAMP_MAX_TC_DP_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    if (ccm_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)
    {
        ccm_entry_data->unified_port_access.port_profile = 0;
        ccm_entry_data->dest_sys_port_agr = (test->gen_value & DNX_OAMP_MAX_SYSTEM_PORT);
        test->gen_value += GEN_VALUE_DELTA;
    }
    else
    {
        ccm_entry_data->unified_port_access.port_profile = (test->gen_value & DNX_OAMP_MAX_PORT_INDEX);
        test->gen_value += GEN_VALUE_DELTA;
    }

    ccm_entry_data->mdl = (test->gen_value & DNX_OAMP_MAX_MDL);
    test->gen_value += GEN_VALUE_DELTA;

    ccm_entry_data->mep_id = (test->gen_value & DNX_OAMP_MAX_MEP_ID);
    test->gen_value += GEN_VALUE_DELTA;

    ccm_entry_data->interface_status_tlv_code = (test->gen_value & DNX_OAMP_MAX_TLV_CODE);
    test->gen_value += GEN_VALUE_DELTA;

    ccm_entry_data->mep_profile = (test->gen_value & DNX_OAMP_MAX_MEP_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    nof_counter_base_bits = dnx_data_oam.general.oam_nof_bits_counter_base_get(unit);
    max_counter_base = SAL_UPTO_BIT(nof_counter_base_bits);
    ccm_entry_data->counter_ptr = (test->gen_value & max_counter_base);
    test->gen_value += GEN_VALUE_DELTA;

    ccm_entry_data->counter_interface = (test->gen_value % DNX_OAMP_NOF_COUNTER_INTERFACES);
    test->gen_value += GEN_VALUE_DELTA;

    nof_mep_pe_profile_bits = dnx_data_oam.oamp.mep_pe_profile_nof_bits_get(unit);
    max_mep_pe_profile = SAL_UPTO_BIT(nof_mep_pe_profile_bits);
    ccm_entry_data->mep_pe_profile = (test->gen_value & max_mep_pe_profile);
    test->gen_value += GEN_VALUE_DELTA;

    ccm_entry_data->crps_core_select = (test->gen_value & DNX_OAMP_MAX_CRPS_CORE);
    test->gen_value += GEN_VALUE_DELTA;

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (ccm_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        if (_SHR_IS_FLAG_SET(ccm_entry_data->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
        {
            temp = (test->gen_value & DNX_OAMP_MAX_PORT_INDEX);
            ccm_entry_data->unified_port_access.ccm_eth_up_mep_port.pp_port = (temp & PORT_BITS);
            ccm_entry_data->unified_port_access.ccm_eth_up_mep_port.port_core =
                UTILEX_GET_BIT(temp, CORE_BIT) % nof_cores;
            test->gen_value += GEN_VALUE_DELTA;
        }

        /** Ethernet OAM CCM packets are always multicast, unicast not supported */
        UTILEX_SET_BIT(ccm_entry_data->flags, DBAL_ENUM_FVAL_DA_UC_MC_TYPE_MC, OAMP_MEP_MC_UC_SEL);

        ccm_entry_data->inner_tpid_index = (test->gen_value & DNX_OAMP_MAX_TPID_INDEX);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->inner_vid = (test->gen_value & DNX_OAMP_MAX_VID);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->inner_pcp_dei = (test->gen_value & DNX_OAMP_MAX_DEI_PCP);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->outer_tpid_index = (test->gen_value & DNX_OAMP_MAX_TPID_INDEX);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->outer_vid = (test->gen_value & DNX_OAMP_MAX_VID);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->outer_pcp_dei = (test->gen_value & DNX_OAMP_MAX_DEI_PCP);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->nof_vlan_tags = (test->gen_value % VLAN_TAGS_NOF_OPTIONS);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->sa_gen_lsb = (test->gen_value & DNX_OAMP_MAX_SA_GEN_LSB);
        test->gen_value += GEN_VALUE_DELTA;
        ccm_entry_data->sa_gen_msb_profile = (test->gen_value & DNX_OAMP_MAX_SA_GEN_MSB_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;
    }
    else
    {
        if (_SHR_IS_FLAG_SET(test->gen_value, FEC_OR_OUT_LIF_BIT))
        {
            ccm_entry_data->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
            num_fec_ids = dnx_data_l3.fec.nof_fecs_get(unit);
            ccm_entry_data->fec_id_or_glob_out_lif.fec_id = (test->gen_value) % num_fec_ids;
        }
        else
        {
            num_lif_ids = dnx_data_lif.global_lif.nof_global_out_lifs_get(unit);
            ccm_entry_data->fec_id_or_glob_out_lif.glob_out_lif = (test->gen_value) % num_lif_ids;
        }
        test->gen_value += GEN_VALUE_DELTA;

        ccm_entry_data->label = (test->gen_value & DNX_OAMP_MAX_LABEL);
        test->gen_value += GEN_VALUE_DELTA;

        ccm_entry_data->push_profile = (test->gen_value & DNX_OAMP_MAX_PUSH_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;
    }
}

/**
 * \brief - This function compares two information structure,
 *        presumably one that contains data to create a or
 *        modify a CCM entry, and the other that was read
 *        after the creation/modification occurs, and verifies
 *        that the relevant values are identical.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ccm_entry_data - Structure that was used for
 *        creation or modification
 * \param [in] ccm_entry_data2 - Structure to which the data was
 *        later read.
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
diag_dnx_oamp_test_compare_ccm_info_structs(
    int unit,
    dnx_oam_oamp_ccm_endpoint_t * ccm_entry_data,
    dnx_oam_oamp_ccm_endpoint_t * ccm_entry_data2)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ccm_entry_data->flags != ccm_entry_data2->flags)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Flags read don't match flags written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->flags, ccm_entry_data2->flags);
    }

    if (ccm_entry_data->mep_type != ccm_entry_data2->mep_type)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP type read doesn't match MEP type written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->mep_type, ccm_entry_data2->mep_type);
    }

    if (ccm_entry_data->ccm_interval != ccm_entry_data2->ccm_interval)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  CCM interval read doesn't match CCM interval written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->ccm_interval, ccm_entry_data2->ccm_interval);
    }

    if (ccm_entry_data->itmh_tc_dp_profile != ccm_entry_data2->itmh_tc_dp_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  TC/DP profile read doesn't match TC/DP profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->itmh_tc_dp_profile, ccm_entry_data2->itmh_tc_dp_profile);
    }

    if (ccm_entry_data->mdl != ccm_entry_data2->mdl)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MD level read doesn't match MD level written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->mdl, ccm_entry_data2->mdl);
    }

    if (ccm_entry_data->mep_id != ccm_entry_data2->mep_id)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP ID read doesn't match MEP ID written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->mep_id, ccm_entry_data2->mep_id);
    }

    if (ccm_entry_data->interface_status_tlv_code != ccm_entry_data2->interface_status_tlv_code)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  TLV code read doesn't match TLV code written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->interface_status_tlv_code, ccm_entry_data2->interface_status_tlv_code);
    }

    if (ccm_entry_data->mep_profile != ccm_entry_data2->mep_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP profile read doesn't match MEP profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->mep_profile, ccm_entry_data2->mep_profile);
    }

    if (ccm_entry_data->counter_ptr != ccm_entry_data2->counter_ptr)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Counter base read doesn't match counter base written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->counter_ptr, ccm_entry_data2->counter_ptr);
    }

    if (ccm_entry_data->counter_interface != ccm_entry_data2->counter_interface)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Counter interface read doesn't match counter interface written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->counter_interface, ccm_entry_data2->counter_interface);
    }

    if (ccm_entry_data->mep_pe_profile != ccm_entry_data2->mep_pe_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP PE profile read doesn't match MEP PE profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->mep_pe_profile, ccm_entry_data2->mep_pe_profile);
    }

    if (ccm_entry_data->crps_core_select != ccm_entry_data2->crps_core_select)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  CRPS code select read doesn't match selection written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     ccm_entry_data->crps_core_select, ccm_entry_data2->crps_core_select);
    }

    if (ccm_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)
    {
        if (ccm_entry_data->dest_sys_port_agr != ccm_entry_data2->dest_sys_port_agr)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  System port read doesn't match system port written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->dest_sys_port_agr, ccm_entry_data2->dest_sys_port_agr);
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(ccm_entry_data->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP) &&
            (ccm_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM))
        {
            if ((ccm_entry_data->unified_port_access.ccm_eth_up_mep_port.pp_port !=
                 ccm_entry_data2->unified_port_access.ccm_eth_up_mep_port.pp_port) ||
                (ccm_entry_data->unified_port_access.ccm_eth_up_mep_port.port_core !=
                 ccm_entry_data2->unified_port_access.ccm_eth_up_mep_port.port_core))
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  PP port profile read doesn't match PP port profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                             ccm_entry_data->unified_port_access.ccm_eth_up_mep_port.pp_port |
                             (ccm_entry_data->unified_port_access.ccm_eth_up_mep_port.port_core << CORE_BIT),
                             ccm_entry_data2->unified_port_access.ccm_eth_up_mep_port.pp_port |
                             (ccm_entry_data2->unified_port_access.ccm_eth_up_mep_port.port_core << CORE_BIT));
            }
        }
        else
        {
            if (ccm_entry_data->unified_port_access.port_profile != ccm_entry_data2->unified_port_access.port_profile)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  PP port profile read doesn't match PP port profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                             ccm_entry_data->unified_port_access.port_profile,
                             ccm_entry_data2->unified_port_access.port_profile);
            }
        }
    }

    if (ccm_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        if (ccm_entry_data->inner_tpid_index != ccm_entry_data2->inner_tpid_index)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Inner TPID index read doesn't match inner TPID index written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->inner_tpid_index, ccm_entry_data2->inner_tpid_index);
        }

        if (ccm_entry_data->inner_vid != ccm_entry_data2->inner_vid)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Inner VID read doesn't match inner VID written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->inner_vid, ccm_entry_data2->inner_vid);
        }

        if (ccm_entry_data->inner_pcp_dei != ccm_entry_data2->inner_pcp_dei)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Inner PCP DEI read doesn't match inner PCP DEI written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->inner_pcp_dei, ccm_entry_data2->inner_pcp_dei);
        }

        if (ccm_entry_data->outer_tpid_index != ccm_entry_data2->outer_tpid_index)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Outer TPID index read doesn't match outer TPID index written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->outer_tpid_index, ccm_entry_data2->outer_tpid_index);
        }

        if (ccm_entry_data->outer_vid != ccm_entry_data2->outer_vid)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Outer VID read doesn't match outer VID written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->outer_vid, ccm_entry_data2->outer_vid);
        }

        if (ccm_entry_data->outer_pcp_dei != ccm_entry_data2->outer_pcp_dei)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Outer PCP DEI read doesn't match outer PCP DEI written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->outer_pcp_dei, ccm_entry_data2->outer_pcp_dei);
        }

        if (ccm_entry_data->nof_vlan_tags != ccm_entry_data2->nof_vlan_tags)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Number of VLAN tags read doesn't match number of VLAN tags written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->nof_vlan_tags, ccm_entry_data2->nof_vlan_tags);
        }

        if (ccm_entry_data->sa_gen_lsb != ccm_entry_data2->sa_gen_lsb)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Source MAC LSB read doesn't match Source MAC LSB written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->sa_gen_lsb, ccm_entry_data2->sa_gen_lsb);
        }

        if (ccm_entry_data->sa_gen_msb_profile != ccm_entry_data2->sa_gen_msb_profile)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Source MAC MSB profile read doesn't match Source MAC MSB profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->sa_gen_msb_profile, ccm_entry_data2->sa_gen_msb_profile);
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(ccm_entry_data->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
        {
            if (ccm_entry_data->fec_id_or_glob_out_lif.fec_id != ccm_entry_data2->fec_id_or_glob_out_lif.fec_id)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  FEC ID read doesn't match FEC ID written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                             ccm_entry_data->fec_id_or_glob_out_lif.fec_id,
                             ccm_entry_data2->fec_id_or_glob_out_lif.fec_id);
            }
        }
        else
        {
            if (ccm_entry_data->fec_id_or_glob_out_lif.glob_out_lif !=
                ccm_entry_data2->fec_id_or_glob_out_lif.glob_out_lif)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL,
                             "Test failed.  Global out-LIF read doesn't match global out-LIF written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                             ccm_entry_data->fec_id_or_glob_out_lif.glob_out_lif,
                             ccm_entry_data2->fec_id_or_glob_out_lif.glob_out_lif);
            }
        }

        if (ccm_entry_data->label != ccm_entry_data2->label)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  MPLS/PWE label read doesn't match label written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->label, ccm_entry_data2->label);
        }

        if (ccm_entry_data->push_profile != ccm_entry_data2->push_profile)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  MPLS/PWE TTL/EXP profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         ccm_entry_data->push_profile, ccm_entry_data2->push_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write values for all fields needed to create
 *          an OAMP LM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] lm_entry_data - structure to which values
 *        are written
 * \param [in,out] test - structure that holds "global"
 *        variables for test.
 * \param [in] is_self_contained - flag for type of related
 *        CCM entry: TRUE for self-contained, FALSE for
 *        LM/DM offloaded.
 * \param [in] is_ccm_ethernet_entry - flag for type of
 *        related CCM entry: TRUE for ethernet, FALSE for
 *        MPLS/PWE.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
diag_dnx_oamp_test_generate_lm_params(
    int unit,
    dnx_oam_oamp_endpoint_lm_params_t * lm_entry_data,
    diag_dnx_oamp_semantic_test_struct * test,
    uint8 is_self_contained,
    uint8 is_ccm_ethernet_entry)
{
    int mep_db_endpoint_entries, total_entries, avail_data_entries, entries_per_bank;
    lm_entry_data->flags = 0;

    lm_entry_data->flags |= DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY;
    test->gen_value += GEN_VALUE_DELTA;
    mep_db_endpoint_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
    total_entries = dnx_data_oam.oamp.nof_mep_db_entries_get(unit);
    entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    /**
     * LM entry ID will be above 64K.
     * Leave space for two entries in the subsequent banks
     */
    avail_data_entries = total_entries - mep_db_endpoint_entries - 2 * entries_per_bank;
    lm_entry_data->flex_lm_entry =
        (mep_db_endpoint_entries + (((test->gen_value >> MEP_ID_OFFSET) % avail_data_entries))) << FULL_ENTRY_ID_SHIFT;
    test->gen_value += GEN_VALUE_DELTA;

    if (is_ccm_ethernet_entry)
    {
        /** Relevant only to ethernet endpoints */
        lm_entry_data->lmm_dmm_da_profile = (test->gen_value & DNX_OAMP_MAX_LMM_DA_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;
    }
    else
    {
        lm_entry_data->lmm_dmm_da_profile = 0;
    }
}

/**
 * \brief - This function compares two information structure,
 *        presumably one that contains data to create a or
 *        modify an LM entry, and the other that was read
 *        after the creation/modification occurs, and verifies
 *        that the relevant values are identical.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] lm_entry_data - Structure that was used for
 *        creation or modification
 * \param [in] lm_entry_data2 - Structure to which the data was
 *        later read.
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
diag_dnx_oamp_test_compare_lm_info_structs(
    int unit,
    dnx_oam_oamp_endpoint_lm_params_t * lm_entry_data,
    dnx_oam_oamp_endpoint_lm_params_t * lm_entry_data2)
{
    SHR_FUNC_INIT_VARS(unit);

    /** No flags to check - both are write-only */

    if (lm_entry_data->flex_lm_entry != lm_entry_data2->flex_lm_entry)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  LM entry index read don't match index written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     lm_entry_data->flex_lm_entry, lm_entry_data2->flex_lm_entry);
    }

    if (lm_entry_data->lmm_dmm_da_profile != lm_entry_data2->lmm_dmm_da_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  LMM DA profile read doesn't match LMM DA profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     lm_entry_data->lmm_dmm_da_profile, lm_entry_data2->lmm_dmm_da_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write values for all fields needed to create
 *          an OAMP DM entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] dm_entry_data - structure to which values
 *        are written
 * \param [in,out] test - structure that holds "global"
 *        variables for test.
 * \param [in] is_self_contained - flag for type of related
 *        CCM entry: TRUE for self-contained, FALSE for
 *        LM/DM offloaded.
 * \param [in] is_ccm_ethernet_entry - flag for type of
 *        related CCM entry: TRUE for ethernet, FALSE for
 *        MPLS/PWE.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
diag_dnx_oamp_test_generate_dm_params(
    int unit,
    dnx_oam_oamp_endpoint_dm_params_t * dm_entry_data,
    diag_dnx_oamp_semantic_test_struct * test,
    uint8 is_self_contained,
    uint8 is_ccm_ethernet_entry)
{
    int mep_db_endpoint_entries, total_entries, avail_data_entries;
    if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY) && is_self_contained)
    {
        dm_entry_data->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY;
    }
    else
    {
        dm_entry_data->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    }
    test->gen_value += GEN_VALUE_DELTA;

    dm_entry_data->lmm_dmm_da_profile = 0;

    if (!_SHR_IS_FLAG_SET(test->test_flags, OAM_WITH_LM_ENTRY))
    {
        /** DM entry ID will be above 64K */
        mep_db_endpoint_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
        total_entries = dnx_data_oam.oamp.nof_mep_db_entries_get(unit);
        avail_data_entries = total_entries - mep_db_endpoint_entries;
        dm_entry_data->flex_dm_entry =
            (mep_db_endpoint_entries +
             (((test->gen_value >> MEP_ID_OFFSET) % avail_data_entries))) << FULL_ENTRY_ID_SHIFT;
        test->gen_value += GEN_VALUE_DELTA;
        if (is_ccm_ethernet_entry)
        {
            /** Relevant only to ethernet endpoints */
            dm_entry_data->lmm_dmm_da_profile = (test->gen_value & DNX_OAMP_MAX_LMM_DA_PROFILE);
            test->gen_value += GEN_VALUE_DELTA;
        }
    }
}

/**
 * \brief - This function compares two information structure,
 *        presumably one that contains data to create a or
 *        modify an DM entry, and the other that was read
 *        after the creation/modification occurs, and verifies
 *        that the relevant values are identical.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] dm_entry_data - Structure that was used for
 *        creation or modification
 * \param [in] dm_entry_data2 - Structure to which the data was
 *        later read.
 * \param [in] is_lm_configured - flag for type of test:
 *           DM only, or LM and DM together
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
diag_dnx_oamp_test_compare_dm_info_structs(
    int unit,
    dnx_oam_oamp_endpoint_dm_params_t * dm_entry_data,
    dnx_oam_oamp_endpoint_dm_params_t * dm_entry_data2,
    uint8 is_lm_configured)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dm_entry_data->flags != dm_entry_data2->flags)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Flags read don't match flags written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     dm_entry_data->flags, dm_entry_data2->flags);
    }

    if (!is_lm_configured && (dm_entry_data->flex_dm_entry != dm_entry_data2->flex_dm_entry))
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  DM entry index read don't match index written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     dm_entry_data->flex_dm_entry, dm_entry_data2->flex_dm_entry);
    }

    if (dm_entry_data->lmm_dmm_da_profile != dm_entry_data2->lmm_dmm_da_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  LMM DA profile read doesn't match LMM DA profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     dm_entry_data->lmm_dmm_da_profile, dm_entry_data2->lmm_dmm_da_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write values for all fields needed to create
 *          an OAMP BFD entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] for_modification - if this value is true,
 *        overwrite only fields that may be modified after the
 *        entry was created.
 * \param [out] bfd_entry_data - structure to which values
 *        are written
 * \param [in,out] test - structure that holds "global"
 *        variables for test.
 * \param [out] entry_id - index of OAMP MEP DB entry,
 *        including least significant bits for short entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
diag_dnx_oamp_test_generate_bfd_params(
    int unit,
    int for_modification,
    dnx_bfd_oamp_endpoint_t * bfd_entry_data,
    diag_dnx_oamp_semantic_test_struct * test,
    uint32 *entry_id)
{
    uint32 num_fec_ids, num_lif_ids;
    int mep_db_threshold, total_full_entries, avail_full_entries;
    int nof_mep_pe_profile_bits, max_mep_pe_profile;

    if (!for_modification)
    {
        mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
        total_full_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
        avail_full_entries = total_full_entries - (mep_db_threshold >> FULL_ENTRY_ID_SHIFT);
        if ((avail_full_entries == 0) ||
            ((mep_db_threshold > 0) && _SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_BFD_MEP_Q_ENTRY)))
        {
                        /** Short entry */
            bfd_entry_data->flags |= DNX_OAMP_BFD_MEP_Q_ENTRY;
            *entry_id = ((test->gen_value >> MEP_ID_OFFSET) % mep_db_threshold);
            test->gen_value += GEN_VALUE_DELTA;
        }
        else
        {
                        /** Full entry */
            *entry_id = mep_db_threshold +
                (((test->gen_value >> MEP_ID_OFFSET) % avail_full_entries) << FULL_ENTRY_ID_SHIFT);
            test->gen_value += GEN_VALUE_DELTA;
        }

        bfd_entry_data->mep_type = MIN_BFD_MEP_TYPE + (test->gen_value % NOF_BFD_MEP_TYPES);
        test->gen_value += GEN_VALUE_DELTA;
    }

    bfd_entry_data->tx_rate_profile = (test->gen_value & DNX_OAMP_MAX_TX_RATE_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->itmh_tc_dp_profile = (test->gen_value & DNX_OAMP_MAX_TC_DP_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    if (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE)
    {
        bfd_entry_data->dest_sys_port_agr = (test->gen_value & DNX_OAMP_MAX_SYSTEM_PORT);
        test->gen_value += GEN_VALUE_DELTA;

        if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_BFD_MEP_ACH))
        {
            bfd_entry_data->flags |= DNX_OAMP_BFD_MEP_ACH;
            test->gen_value += GEN_VALUE_DELTA;
        }

        if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_BFD_MEP_GAL))
        {
            bfd_entry_data->flags |= DNX_OAMP_BFD_MEP_GAL;
            test->gen_value += GEN_VALUE_DELTA;
        }

        if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_BFD_MEP_ROUTER_ALERT))
        {
            bfd_entry_data->flags |= DNX_OAMP_BFD_MEP_ROUTER_ALERT;
            test->gen_value += GEN_VALUE_DELTA;
        }

        bfd_entry_data->ach_sel = (test->gen_value & DNX_OAMP_MAX_ACH_SEL);
        test->gen_value += GEN_VALUE_DELTA;
    }
    else
    {
        bfd_entry_data->pp_port_profile = (test->gen_value & DNX_OAMP_MAX_PORT_INDEX);
        test->gen_value += GEN_VALUE_DELTA;

        bfd_entry_data->src_ip_profile = (test->gen_value & DNX_OAMP_MAX_SRC_IP_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;
    }

    bfd_entry_data->your_disc = test->gen_value;
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->detect_mult = (test->gen_value & DNX_OAMP_MAX_DETECT_MULT);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->min_tx_interval_profile = (test->gen_value & DNX_OAMP_MAX_INTERVAL_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->min_rx_interval_profile = (test->gen_value & DNX_OAMP_MAX_INTERVAL_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    if (_SHR_IS_FLAG_SET(test->gen_value, FEC_OR_OUT_LIF_BIT))
    {
        bfd_entry_data->flags |= DNX_OAMP_BFD_DESTINATION_IS_FEC;
        num_fec_ids = dnx_data_l3.fec.nof_fecs_get(unit);
        bfd_entry_data->fec_id_or_glob_out_lif.fec_id = (test->gen_value) % num_fec_ids;
    }
    else
    {
        num_lif_ids = dnx_data_lif.global_lif.nof_global_out_lifs_get(unit);
        bfd_entry_data->fec_id_or_glob_out_lif.glob_out_lif = (test->gen_value) % num_lif_ids;
    }
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->diag_profile = ((test->gen_value) % NOF_DIAG_VALUES);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->flags_profile = ((test->gen_value) & DNX_OAMP_MAX_FLAGS_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->sta = ((test->gen_value) & DNX_OAMP_MAX_STATE);
    test->gen_value += GEN_VALUE_DELTA;

    nof_mep_pe_profile_bits = dnx_data_oam.oamp.mep_pe_profile_nof_bits_get(unit);
    max_mep_pe_profile = SAL_UPTO_BIT(nof_mep_pe_profile_bits);
    bfd_entry_data->mep_pe_profile = (test->gen_value & max_mep_pe_profile);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->mep_profile = (test->gen_value & DNX_OAMP_MAX_MEP_PROFILE);
    test->gen_value += GEN_VALUE_DELTA;

    bfd_entry_data->crps_core_select = (test->gen_value & DNX_OAMP_MAX_CRPS_CORE);
    test->gen_value += GEN_VALUE_DELTA;

    if ((bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP) ||
        (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP))
    {
        bfd_entry_data->ipv4_dip = test->gen_value;
        test->gen_value += GEN_VALUE_DELTA;
    }
    else
    {
                /** MPLS/PWE */
        bfd_entry_data->label = ((test->gen_value) & DNX_OAMP_MAX_LABEL);
        test->gen_value += GEN_VALUE_DELTA;

        bfd_entry_data->push_profile = (test->gen_value & DNX_OAMP_MAX_PUSH_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;
    }

    if (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP)
    {
        bfd_entry_data->ip_subnet_len = ((test->gen_value) & DNX_OAMP_MAX_SUBNET_LEN);
        test->gen_value += GEN_VALUE_DELTA;

        if (_SHR_IS_FLAG_SET(test->gen_value, DNX_OAMP_BFD_MEP_MICRO_BFD))
        {
            bfd_entry_data->flags |= DNX_OAMP_BFD_MEP_MICRO_BFD;
            test->gen_value += GEN_VALUE_DELTA;
        }
    }

    if ((bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP) ||
        (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS))
    {
        bfd_entry_data->ip_ttl_tos_profile = ((test->gen_value) & DNX_OAMP_MAX_TTL_TOS_PROFILE);
        test->gen_value += GEN_VALUE_DELTA;
    }
}

static shr_error_e
diag_dnx_oamp_test_compare_bfd_info_structs(
    int unit,
    dnx_bfd_oamp_endpoint_t * bfd_entry_data,
    dnx_bfd_oamp_endpoint_t * bfd_entry_data2)
{
    SHR_FUNC_INIT_VARS(unit);

    if (bfd_entry_data->flags != bfd_entry_data2->flags)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Flags read don't match flags written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->flags, bfd_entry_data2->flags);
    }

    if (bfd_entry_data->mep_type != bfd_entry_data2->mep_type)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP type read doesn't match MEP type written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->mep_type, bfd_entry_data2->mep_type);
    }

    if (bfd_entry_data->tx_rate_profile != bfd_entry_data2->tx_rate_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  TX rate profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->tx_rate_profile, bfd_entry_data2->tx_rate_profile);
    }

    if (bfd_entry_data->itmh_tc_dp_profile != bfd_entry_data2->itmh_tc_dp_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  TC/DP profile read doesn't match TC/DP profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->itmh_tc_dp_profile, bfd_entry_data2->itmh_tc_dp_profile);
    }

    if (bfd_entry_data->crps_core_select != bfd_entry_data2->crps_core_select)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  CRPS code select read doesn't match selection written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->crps_core_select, bfd_entry_data2->crps_core_select);
    }

    if (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE)
    {
        if (bfd_entry_data->dest_sys_port_agr != bfd_entry_data2->dest_sys_port_agr)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  System port read doesn't match system port written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->dest_sys_port_agr, bfd_entry_data2->dest_sys_port_agr);
        }

        if (bfd_entry_data->ach_sel != bfd_entry_data2->ach_sel)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  ACH selection read doesn't match selection written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->ach_sel, bfd_entry_data2->ach_sel);
        }
    }
    else
    {
        if (bfd_entry_data->pp_port_profile != bfd_entry_data2->pp_port_profile)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  PP port profile read doesn't match PP port profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->pp_port_profile, bfd_entry_data2->pp_port_profile);
        }

        if (bfd_entry_data->src_ip_profile != bfd_entry_data2->src_ip_profile)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Source IP profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->src_ip_profile, bfd_entry_data2->src_ip_profile);
        }
    }

    if (bfd_entry_data->your_disc != bfd_entry_data2->your_disc)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Your discriminator read doesn't match discriminator written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->your_disc, bfd_entry_data2->your_disc);
    }

    if (bfd_entry_data->detect_mult != bfd_entry_data2->detect_mult)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Detect mult read doesn't match detect mult written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->detect_mult, bfd_entry_data2->detect_mult);
    }

    if (bfd_entry_data->min_tx_interval_profile != bfd_entry_data2->min_tx_interval_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Min Tx interval profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->min_tx_interval_profile, bfd_entry_data2->min_tx_interval_profile);
    }

    if (bfd_entry_data->min_rx_interval_profile != bfd_entry_data2->min_rx_interval_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Min Rx interval profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->min_rx_interval_profile, bfd_entry_data2->min_rx_interval_profile);
    }

    if (_SHR_IS_FLAG_SET(bfd_entry_data->flags, DNX_OAMP_BFD_DESTINATION_IS_FEC))
    {
        if (bfd_entry_data->fec_id_or_glob_out_lif.fec_id != bfd_entry_data2->fec_id_or_glob_out_lif.fec_id)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  FEC ID read doesn't match FEC ID written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->fec_id_or_glob_out_lif.fec_id, bfd_entry_data2->fec_id_or_glob_out_lif.fec_id);
        }
    }
    else
    {
        if (bfd_entry_data->fec_id_or_glob_out_lif.glob_out_lif != bfd_entry_data2->fec_id_or_glob_out_lif.glob_out_lif)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Global out-LIF read doesn't match global out-LIF written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->fec_id_or_glob_out_lif.glob_out_lif,
                         bfd_entry_data2->fec_id_or_glob_out_lif.glob_out_lif);
        }
    }

    if (bfd_entry_data->diag_profile != bfd_entry_data2->diag_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Diag profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->diag_profile, bfd_entry_data2->diag_profile);
    }

    if (bfd_entry_data->flags_profile != bfd_entry_data2->flags_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  Flags profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->flags_profile, bfd_entry_data2->flags_profile);
    }

    if (bfd_entry_data->sta != bfd_entry_data2->sta)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  BFD state read doesn't match state written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->sta, bfd_entry_data2->sta);
    }

    if (bfd_entry_data->mep_pe_profile != bfd_entry_data2->mep_pe_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP PE profile read doesn't match MEP PE profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->mep_pe_profile, bfd_entry_data2->mep_pe_profile);
    }

    if (bfd_entry_data->mep_profile != bfd_entry_data2->mep_profile)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL,
                     "Test failed.  MEP profile read doesn't match MEP profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                     bfd_entry_data->mep_profile, bfd_entry_data2->mep_profile);
    }

    if ((bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP) ||
        (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP))
    {
        if (bfd_entry_data->ipv4_dip != bfd_entry_data2->ipv4_dip)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Destination IP read doesn't match IP written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->ipv4_dip, bfd_entry_data2->ipv4_dip);
        }
    }
    else
    {
                /** MPLS/PWE */
        if (bfd_entry_data->label != bfd_entry_data2->label)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  MPLS/PWE label read doesn't match label written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->label, bfd_entry_data2->label);
        }

        if (bfd_entry_data->push_profile != bfd_entry_data2->push_profile)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  MPLS/PWE TTL/EXP profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->push_profile, bfd_entry_data2->push_profile);
        }
    }

    if (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP)
    {
        if (bfd_entry_data->ip_subnet_len != bfd_entry_data2->ip_subnet_len)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  Subnet length read doesn't match length written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->ip_subnet_len, bfd_entry_data2->ip_subnet_len);
        }
    }

    if ((bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP) ||
        (bfd_entry_data->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS))
    {
        if (bfd_entry_data->ip_ttl_tos_profile != bfd_entry_data2->ip_ttl_tos_profile)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed.  TTL/EXP profile read doesn't match profile written to entry.  Written: 0x%08X. Read: 0x%08X.\n",
                         bfd_entry_data->ip_ttl_tos_profile, bfd_entry_data2->ip_ttl_tos_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function verifies the passed 
 *         OAMP MEP scan phase counts are filled up
 *         in the mep profile data.
 *
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] exp_ccm_cnt - expected CCM_CNT value
 * \param [in] exp_lmm_cnt - expected LMM_CNT value
 * \param [in] exp_dmm_cnt - expected DMM_CNT value
 * \param [in] exp_op0_cnt - expected OPCODE_0_CNT value
 * \param [in] exp_op1_cnt - expected OPCODE_1_CNT value
 * \param [in] mep_profile_data - Profile data against which these values are verified
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
dnx_oam_oamp_compare_phase_counts(
    int unit,
    uint32 exp_ccm_cnt,
    uint32 exp_lmm_cnt,
    uint32 exp_dmm_cnt,
    uint32 exp_op0_cnt,
    uint32 exp_op1_cnt,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mep_profile_data->ccm_count != exp_ccm_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "CCM_CNT not matching %d %d", mep_profile_data->ccm_count, exp_ccm_cnt);
    }
    if (mep_profile_data->lmm_count != exp_lmm_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "LMM_CNT not matching %d %d", mep_profile_data->lmm_count, exp_lmm_cnt);
    }
    if (mep_profile_data->dmm_count != exp_dmm_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "DMM_CNT not matching %d %d", mep_profile_data->dmm_count, exp_dmm_cnt);
    }
    if (mep_profile_data->opcode_0_count != exp_op0_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "OPCODE_0_CNT not matching %d %d", mep_profile_data->opcode_0_count, exp_op0_cnt);
    }
    if (mep_profile_data->opcode_1_count != exp_op1_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "OPCODE_1_CNT not matching %d %d", mep_profile_data->opcode_1_count, exp_op1_cnt);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the OAMP MEP scan phase 
 *        counts calculation APIs in a very basic way: 
 *        CCM, LMM, DMM, opcode 0 rates are filled up
 *        and passed to the API to verify if it fills up
 *        phase counts correctly.
 *
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line 
 *        arguments were parsed (partially)
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
diag_dnx_oamp_scan_rate_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_oam_endpoint_t endpoint_id = 8;
    dbal_enum_value_field_ccm_interval_e ccm_rate = DBAL_ENUM_FVAL_CCM_INTERVAL_3MS;
    dnx_oam_oamp_tx_opcode opcode = OAMP_MEP_TX_OPCODE_CCM_BFD;
    dnx_oam_mep_profile_t mep_profile_data = { 0 };
    uint32 exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt;
    /*
     * Nothing much to check for short entry. So assuming self_contained/offloaded 
     */
    uint8 ccm_rx_without_tx = 0;
    uint8 is_short_entry = 0;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * TEST CASE 1 :Check for even endpoint id - CCM cnt = 0
     */
    /*
     * TEST CASE 1.1: Fill up ccm rate as 3.3ms and check if CCM_CNT is filled correctly 
     */
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 1.2: Fill up LMM rate as 3.3ms and check if LMM_CNT is filled correctly as 1
     */
    opcode = OAMP_MEP_TX_OPCODE_LMM;
    mep_profile_data.lmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 1;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 1.3: Now try to add DMM rate as 10ms and check if E_RESOURCE is returned, since already 2 3.3ms
     * opcodes are there. 
     */
    opcode = OAMP_MEP_TX_OPCODE_DMM;
    mep_profile_data.dmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 1;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    rv = dnx_oam_oamp_scan_count_calc(unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode,
                                      &mep_profile_data);
    if (rv != _SHR_E_RESOURCE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected E_RESOURCE but got %d", rv);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));
    /*
     * Change it back to 0 since dmm_rate did not get configured with appropriate phase cnt 
     */
    mep_profile_data.dmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;

    /*
     * TEST CASE 1.4: Modify LMM rate to 10ms and check if LMM_CNT and DMM_CNT is filled correctly 
     */
    opcode = OAMP_MEP_TX_OPCODE_LMM;
    mep_profile_data.lmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 1;
    exp_dmm_cnt = 3;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 1.5: Now try to add DMM rate as 10ms and check if DMM_CNT and LMM_CNT is filled correctly. 
     */
    opcode = OAMP_MEP_TX_OPCODE_DMM;
    mep_profile_data.dmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 1;
    exp_dmm_cnt = 3;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 1.6: Add Opcode 0 rate as 10ms and check it returns E_PARAM
     */
    opcode = OAMP_MEP_TX_OPCODE_OP0;
    mep_profile_data.opcode_0_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 1;
    exp_dmm_cnt = 3;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    rv = dnx_oam_oamp_scan_count_calc(unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode,
                                      &mep_profile_data);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected E_PARAM but got %d", rv);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));
    /*
     * Change it back to 0 since opcode_0_rate did not get configured with appropriate phase cnt 
     */
    mep_profile_data.opcode_0_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;

    /*
     * TEST CASE 1.7: Modify LMM rate to 100ms and check if LMM_CNT and DMM_CNT is filled correctly. 
     */
    opcode = OAMP_MEP_TX_OPCODE_LMM;
    mep_profile_data.lmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 3;
    exp_dmm_cnt = 1;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 1.8: Make opcode 0 rate as 1s and check if OPCODE_0_COUNT is filled correctly. 
     */
    opcode = OAMP_MEP_TX_OPCODE_OP0;
    mep_profile_data.opcode_0_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    exp_ccm_cnt = 0;
    exp_lmm_cnt = 3;
    exp_dmm_cnt = 1;
    exp_op0_cnt = 5;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 2 : Check for odd endpoint id - CCM_CNT = 1.
     */
    /*
     * Repeat the tests in TEST CASE 1 for odd endpoint id. 
     */
    endpoint_id = 12;
    ccm_rate = DBAL_ENUM_FVAL_CCM_INTERVAL_3MS;
    opcode = OAMP_MEP_TX_OPCODE_CCM_BFD;
    sal_memset(&mep_profile_data, 0, sizeof(mep_profile_data));

    /*
     * TEST CASE 2.1: Fill up ccm rate as 3.3ms and check if CCM_CNT is filled correctly 
     */
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 2.2: Fill up LMM rate as 3.3ms and check if LMM_CNT is filled correctly as 1
     */
    opcode = OAMP_MEP_TX_OPCODE_LMM;
    mep_profile_data.lmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 2.3: Now try to add DMM rate as 10ms and check if E_RESOURCE is returned, since already 2 3.3ms
     * opcodes are there. 
     */
    opcode = OAMP_MEP_TX_OPCODE_DMM;
    mep_profile_data.dmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    rv = dnx_oam_oamp_scan_count_calc(unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode,
                                      &mep_profile_data);
    if (rv != _SHR_E_RESOURCE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected E_RESOURCE but got %d", rv);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));
    /*
     * Change it back to 0 since dmm_rate did not get configured with appropriate phase cnt 
     */
    mep_profile_data.dmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;

    /*
     * TEST CASE 2.4: Modify LMM rate to 10ms and check if LMM_CNT and DMM_CNT is filled correctly 
     */
    opcode = OAMP_MEP_TX_OPCODE_LMM;
    mep_profile_data.lmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 2;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 2.5: Now try to add DMM rate as 10ms and check if DMM_CNT and LMM_CNT is filled correctly. 
     */
    opcode = OAMP_MEP_TX_OPCODE_DMM;
    mep_profile_data.dmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 2;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 2.6: Add Opcode 0 rate as 10ms and check it returns E_PARAM
     */
    opcode = OAMP_MEP_TX_OPCODE_OP0;
    mep_profile_data.opcode_0_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 0;
    exp_dmm_cnt = 2;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    rv = dnx_oam_oamp_scan_count_calc(unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode,
                                      &mep_profile_data);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected E_PARAM but got %d", rv);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));
    /*
     * Change it back to 0 since opcode_0_rate did not get configured with appropriate phase cnt 
     */
    mep_profile_data.opcode_0_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;

    /*
     * TEST CASE 2.7: Modify LMM rate to 100ms and check if LMM_CNT and DMM_CNT is filled correctly. 
     */
    opcode = OAMP_MEP_TX_OPCODE_LMM;
    mep_profile_data.lmm_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 2;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 0;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    /*
     * TEST CASE 2.8: Make opcode 0 rate as 1s and check if OPCODE_0_COUNT is filled correctly. 
     */
    opcode = OAMP_MEP_TX_OPCODE_OP0;
    mep_profile_data.opcode_0_rate = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    exp_ccm_cnt = 1;
    exp_lmm_cnt = 2;
    exp_dmm_cnt = 0;
    exp_op0_cnt = 4;
    exp_op1_cnt = 0;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc
                    (unit, endpoint_id, 0, is_short_entry, ccm_rate, ccm_rx_without_tx, opcode, &mep_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_compare_phase_counts
                    (unit, exp_ccm_cnt, exp_lmm_cnt, exp_dmm_cnt, exp_op0_cnt, exp_op1_cnt, &mep_profile_data));

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the OAMP MEP DB APIs  in a
 *        very basic way: a number of OAMP MEP DB entries
 *        are created and verified, modified and verified
 *        again, and finally deleted.  It is part of the
 *        ctest feature.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line 
 *        arguments were parsed (partially)
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
diag_dnx_oamp_semantic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 num_entries, flags, entry_index, entry_type = ENTRY_TYPE_CCM;
    uint32 *entry_id = NULL;
    dnx_bfd_oamp_endpoint_t *bfd_entry_data = NULL, *bfd_entry_data2 = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_entry_data = NULL, *ccm_entry_data2 = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_entry_data = NULL, *lm_entry_data2 = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_entry_data = NULL, *dm_entry_data2 = NULL;
    diag_dnx_oamp_semantic_test_struct *test = NULL;
    uint8 dm_entry_found = 0, lm_exists = 0, lm_stat_exists = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("Number", num_entries);
    SH_SAND_GET_ENUM("Type", entry_type);
    SH_SAND_GET_ENUM("FLaGs", flags);

    LOG_CLI((BSL_META("Test with %d entries\n"), num_entries));
    LOG_CLI((BSL_META("Test with entry type = %d\n"), entry_type));
    LOG_CLI((BSL_META("Test with flags = 0x%08X\n"), flags));

    if (entry_type == ENTRY_TYPE_CCM)
    {
        SHR_ALLOC_SET_ZERO(ccm_entry_data, sizeof(dnx_oam_oamp_ccm_endpoint_t), "Struct for creating OAMP CCM entries",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(ccm_entry_data2, sizeof(dnx_oam_oamp_ccm_endpoint_t), "Struct for reading OAMP CCM entries",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        if (_SHR_IS_FLAG_SET(flags, OAM_WITH_LM_ENTRY))
        {
            SHR_ALLOC_SET_ZERO(lm_entry_data, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                               "Struct for creating OAMP LM entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(lm_entry_data2, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                               "Struct for reading OAMP LM entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        }

        if (_SHR_IS_FLAG_SET(flags, OAM_WITH_DM_ENTRY))
        {
            SHR_ALLOC_SET_ZERO(dm_entry_data, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                               "Struct for creating OAMP DM entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(dm_entry_data2, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                               "Struct for reading OAMP DM entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        }
    }
    else
    {
        if (entry_type == ENTRY_TYPE_BFD)
        {
            SHR_ALLOC_SET_ZERO(bfd_entry_data, sizeof(dnx_bfd_oamp_endpoint_t), "Struct for creating OAMP BFD entries",
                               "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(bfd_entry_data2, sizeof(dnx_bfd_oamp_endpoint_t), "Struct for reading OAMP BFD entries",
                               "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_ALLOC_SET_ZERO(entry_id, num_entries * sizeof(uint32), "Array of entry IDs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(test, sizeof(diag_dnx_oamp_semantic_test_struct), "Struct for test variables", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

    test->gen_value = INIT_GEN_VALUE;
    test->test_flags = flags;

    for (entry_index = 0; entry_index < num_entries; entry_index++)
    {
        dm_entry_found = 0;
        if (entry_type == ENTRY_TYPE_CCM)
        {
            ccm_entry_data->flags = 0;
            diag_dnx_oamp_test_generate_ccm_params(unit, FALSE, ccm_entry_data, test, &entry_id[entry_index]);

            /** Create CCM entry */
            SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_ccm_endpoint_set(unit, entry_id[entry_index], ccm_entry_data),
                                "Test failed.  Could not create CCM entry\n");
            LOG_CLI((BSL_META("Added CCM entry #%d\n"), entry_index + 1));

            /** Read entry parameters and compare   */
            SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_ccm_endpoint_get(unit, entry_id[entry_index], 0, ccm_entry_data2),
                                "Test failed.  CCM entry could not be read.\n");

            LOG_CLI((BSL_META("Checking values for CCM entry #%d\n"), entry_index + 1));
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_compare_ccm_info_structs(unit, ccm_entry_data, ccm_entry_data2),
                                "Test failed.  Parameters read did not match parameters written.\n");
            LOG_CLI((BSL_META("Read CCM entry #%d\n"), entry_index + 1));

            /** Modify CCM entry   */
            ccm_entry_data->flags = DNX_OAMP_OAM_CCM_MEP_UPDATE |
                (ccm_entry_data->flags &
                 (DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED | DNX_OAMP_OAM_CCM_MEP_Q_ENTRY | DNX_OAMP_OAM_CCM_MEP_IS_UPMEP));
            diag_dnx_oamp_test_generate_ccm_params(unit, TRUE, ccm_entry_data, test, &entry_id[entry_index]);

            SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_ccm_endpoint_set(unit, entry_id[entry_index], ccm_entry_data),
                                "Test failed.  Could not modify CCM entry\n");
            LOG_CLI((BSL_META("Modify CCM entry #%d\n"), entry_index + 1));

            /** Read and compare again   */
            ccm_entry_data->flags &= ~DNX_OAMP_OAM_CCM_MEP_UPDATE;
            SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_ccm_endpoint_get(unit, entry_id[entry_index], 0, ccm_entry_data2),
                                "Test failed.  CCM entry could not be read.\n");
            LOG_CLI((BSL_META("Checking values for CCM entry #%d\n"), entry_index + 1));
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_compare_ccm_info_structs(unit, ccm_entry_data, ccm_entry_data2),
                                "Test failed.  Parameters read did not match modified parameters written.\n");
            LOG_CLI((BSL_META("Read modified CCM entry #%d\n"), entry_index + 1));

            if (_SHR_IS_FLAG_SET(flags, OAM_WITH_LM_ENTRY))
            {
                diag_dnx_oamp_test_generate_lm_params(unit, lm_entry_data, test,
                                                      !_SHR_IS_FLAG_SET(ccm_entry_data->flags,
                                                                        DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED),
                                                      (ccm_entry_data->mep_type ==
                                                       DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM));

                /** Create LM entry */
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_lm_endpoint_set(unit, entry_id[entry_index], lm_entry_data),
                                    "Test failed.  Could not create LM entry\n");
                LOG_CLI((BSL_META("Added LM entry #%d\n"), entry_index + 1));

                /** Read entry parameters and compare   */
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_lm_endpoint_get(unit, entry_id[entry_index], lm_entry_data2,
                                                                    &lm_exists, &lm_stat_exists),
                                    "Test failed.  LM entry could not be read.\n");
                if (!lm_exists)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  LM entry was not found.\n");
                }

                LOG_CLI((BSL_META("Checking values for LM entry #%d\n"), entry_index + 1));
                SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_compare_lm_info_structs(unit, lm_entry_data, lm_entry_data2),
                                    "Test failed.  Parameters read did not match parameters written.\n");
                LOG_CLI((BSL_META("Read LM entry #%d\n"), entry_index + 1));

            }

            if (_SHR_IS_FLAG_SET(flags, OAM_WITH_DM_ENTRY))
            {
                diag_dnx_oamp_test_generate_dm_params(unit, dm_entry_data, test,
                                                      !_SHR_IS_FLAG_SET(ccm_entry_data->flags,
                                                                        DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED),
                                                      (ccm_entry_data->mep_type ==
                                                       DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM));

                /** Create DM entry */
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_dm_endpoint_set(unit, entry_id[entry_index], dm_entry_data),
                                    "Test failed.  Could not create DM entry\n");
                LOG_CLI((BSL_META("Added DM entry #%d\n"), entry_index + 1));

                /** Read entry parameters and compare   */
                dm_entry_data2->lmm_dmm_da_profile = 0;
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_dm_endpoint_get
                                    (unit, entry_id[entry_index], dm_entry_data2, &dm_entry_found),
                                    "Test failed.  DM entry could not be read.\n");
                if (!dm_entry_found)
                {
                    SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed.  DM entry was not found.\n");
                }

                LOG_CLI((BSL_META("Checking values for DM entry #%d\n"), entry_index + 1));
                SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_compare_dm_info_structs
                                    (unit, dm_entry_data, dm_entry_data2, _SHR_IS_FLAG_SET(flags, OAM_WITH_LM_ENTRY)),
                                    "Test failed.  Parameters read did not match parameters written.\n");
                LOG_CLI((BSL_META("Read DM entry #%d\n"), entry_index + 1));

            }
        }
        else
        {
            if (entry_type == ENTRY_TYPE_BFD)
            {
                bcm_bfd_endpoint_info_t endpoint_info;
                diag_dnx_oamp_test_generate_bfd_params(unit, FALSE, bfd_entry_data, test, &entry_id[entry_index]);

                endpoint_info.id = entry_id[entry_index];
                /** Create BFD entry */
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_bfd_endpoint_set(unit, &endpoint_info, bfd_entry_data),
                                    "Test failed.  Could not create BFD entry\n");
                LOG_CLI((BSL_META("Added BFD entry #%d\n"), entry_index + 1));

                /** Read entry parameters and compare   */
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_bfd_endpoint_get(unit, entry_id[entry_index], bfd_entry_data2),
                                    "Test failed.  BFD entry could not be read.\n");

                LOG_CLI((BSL_META("Checking values for BFD entry #%d\n"), entry_index + 1));
                SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_compare_bfd_info_structs(unit, bfd_entry_data, bfd_entry_data2),
                                    "Test failed.  Parameters read did not match parameters written.\n");
                LOG_CLI((BSL_META("Read BFD entry #%d\n"), entry_index + 1));

                /** Modify BFD entry   */
                bfd_entry_data->flags |= DNX_OAMP_BFD_MEP_UPDATE;
                diag_dnx_oamp_test_generate_bfd_params(unit, TRUE, bfd_entry_data, test, &entry_id[entry_index]);

                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_bfd_endpoint_set(unit, &endpoint_info, bfd_entry_data),
                                    "Test failed.  Could not modify BFD entry\n");
                LOG_CLI((BSL_META("Modify BFD entry #%d\n"), entry_index + 1));

                /** Read and compare again   */
                bfd_entry_data->flags &= ~DNX_OAMP_BFD_MEP_UPDATE;
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_bfd_endpoint_get(unit, entry_id[entry_index], bfd_entry_data2),
                                    "Test failed.  BFD entry could not be read.\n");
                LOG_CLI((BSL_META("Checking values for BFD entry #%d\n"), entry_index + 1));
                SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_compare_bfd_info_structs(unit, bfd_entry_data, bfd_entry_data2),
                                    "Test failed.  Parameters read did not match modified parameters written.\n");
                LOG_CLI((BSL_META("Read modified BFD entry #%d\n"), entry_index + 1));
            }
        }
    }

    /** Delete all the entries  */
    for (entry_index = 0; entry_index < num_entries; entry_index++)
    {
        if (entry_type == ENTRY_TYPE_CCM)
        {
            if (_SHR_IS_FLAG_SET(flags, OAM_WITH_LM_ENTRY))
            {
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_lm_endpoint_clear(unit, entry_id[entry_index]),
                                    "Test failed.  Could not delete LM entry.\n");
                LOG_CLI((BSL_META("Deleted LM entry #%d\n"), entry_index + 1));
            }
            if (_SHR_IS_FLAG_SET(flags, OAM_WITH_DM_ENTRY))
            {
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_dm_endpoint_clear(unit, entry_id[entry_index]),
                                    "Test failed.  Could not delete DM entry.\n");
                LOG_CLI((BSL_META("Deleted DM entry #%d\n"), entry_index + 1));
            }

            SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_ccm_endpoint_clear(unit, entry_id[entry_index], 0),
                                "Test failed.  Could not delete CCM entry.\n");
            LOG_CLI((BSL_META("Deleted CCM entry #%d\n"), entry_index + 1));
        }
        else
        {
            if (entry_type == ENTRY_TYPE_BFD)
            {
                uint8 is_ipv6 = 0;
                SHR_CLI_EXIT_IF_ERR(dnx_oamp_mep_db_bfd_endpoint_clear(unit, entry_id[entry_index], is_ipv6),
                                    "Test failed.  Could not delete BFD entry.\n");
                LOG_CLI((BSL_META("Deleted BFD entry #%d\n"), entry_index + 1));
            }
        }
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FREE(bfd_entry_data);
    SHR_FREE(bfd_entry_data2);
    SHR_FREE(ccm_entry_data);
    SHR_FREE(ccm_entry_data2);
    SHR_FREE(lm_entry_data);
    SHR_FREE(lm_entry_data2);
    SHR_FREE(dm_entry_data);
    SHR_FREE(dm_entry_data2);
    SHR_FREE(entry_id);
    SHR_FREE(test);
    SHR_FUNC_EXIT;
}

/** Supported endpoint types   */
static sh_sand_enum_t diag_dnx_oamp_entry_all_type_enum_table[] = {
    {"BFD", ENTRY_TYPE_BFD},
    {"CCM", ENTRY_TYPE_CCM},
    {NULL}
};

/** Supported flag combinations   */
static sh_sand_enum_t diag_dnx_oam_entry_more_options_enum_table[] = {
    {"NONE", 0},
    {"LM", OAM_WITH_LM_ENTRY},
    {"DM", OAM_WITH_DM_ENTRY},
    {"LM_DM", (OAM_WITH_LM_ENTRY | OAM_WITH_DM_ENTRY)},
    {NULL}
};

/** Test arguments   */
static sh_sand_option_t diag_dnx_oamp_sem_test_options[] = {
    {"Number", SAL_FIELD_TYPE_INT32, "Number of entries", "4"},
    {"Type", SAL_FIELD_TYPE_ENUM, "Type of entry", "BFD", (void *) diag_dnx_oamp_entry_all_type_enum_table},
    {"FLaGs", SAL_FIELD_TYPE_ENUM, "OAMP flags", "NONE", (void *) diag_dnx_oam_entry_more_options_enum_table},
    {NULL}
};

/** Test manual   */
static sh_sand_man_t diag_dnx_oamp_sem_test_man = {
    /** Brief */
    "Semantic test of basic OAMP MEP APIs",
    /** Full */
    "Create OAMP MEP entry.  Get entry and compare."
        " Modify some entries.  Get entries and compare."
        " Delete half the entries individually." " Delete the rest of the entries.",
    /** Synopsis   */
    "ctest oamp semantic [Number=<value> Type=<value> FLaGs=<value>]",
    /** Example   */
    "ctest oamp semantic Number=3 Type=CCM FLaGs=DM",
};

/** Test manual   */
static sh_sand_man_t diag_dnx_oamp_scan_rate_test_man = {
    /** Brief */
    "Semantic test of basic OAMP MEP scan rate calculation APIs",
    /** Full */
    "Call scan rate calc API for various combinations of periods of CCM,LMM,DMM and opcode0"
        "Verify that the phase counts are correctly filled up.",
    /** Synopsis   */
    "ctest oamp scan_rate",
    /** Example   */
    "ctest oamp scan_rate",
};

/** Automatic test list (ctest oamp run)   */
static sh_sand_invoke_t diag_dnx_oamp_sem_tests[] = {
    {"eps_4_bfd", "Number=4 Type=BFD FLaGs=NONE"},
    {"eps_6_bfd", "Number=6 Type=BFD FLaGs=NONE"},
    {"eps_4_ccm", "Number=4 Type=CCM FLaGs=NONE"},
    {"eps_6_ccm", "Number=6 Type=CCM FLaGs=NONE"},
    {"eps_4_ccm_lm", "Number=4 Type=CCM FLaGs=LM"},
    {"eps_6_ccm_lm", "Number=6 Type=CCM FLaGs=LM"},
    {"eps_4_ccm_dm", "Number=4 Type=CCM FLaGs=DM"},
    {"eps_6_ccm_dm", "Number=6 Type=CCM FLaGs=DM"},
    {"eps_4_ccm_lm_dm", "Number=4 Type=CCM FLaGs=LM_DM"},
    {"eps_6_ccm_lm_dm", "Number=6 Type=CCM FLaGs=LM_DM"},
    {NULL}
};

/** List of OAMP tests   */
sh_sand_cmd_t dnx_oamp_test_cmds[] = {
    {"scan_rate", diag_dnx_oamp_scan_rate_test_cmd, NULL, NULL, &diag_dnx_oamp_scan_rate_test_man, NULL, NULL},
    {"semantic", diag_dnx_oamp_semantic_test_cmd, NULL, diag_dnx_oamp_sem_test_options, &diag_dnx_oamp_sem_test_man,
     NULL,
     diag_dnx_oamp_sem_tests},
    {"traffic", diag_dnx_oamp_traffic_test_cmd, NULL, diag_dnx_oamp_traffic_test_options,
     &diag_dnx_oamp_traffic_test_man,
     NULL,
     diag_dnx_oamp_traffic_tests},
    {"bfd", diag_dnx_oamp_bfd_test_cmd, NULL, diag_dnx_oamp_bfd_test_options,
     &diag_dnx_oamp_bfd_test_man,
     NULL,
     diag_dnx_oamp_bfd_tests},
    {NULL}
};
