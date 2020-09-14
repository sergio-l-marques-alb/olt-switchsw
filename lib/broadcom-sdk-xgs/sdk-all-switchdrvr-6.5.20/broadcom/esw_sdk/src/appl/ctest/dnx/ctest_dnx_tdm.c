/**
 **  \file dnx\ctest_dnx_tdm.c
 *
 * File:        ctest_dnx_packet.c
 * Purpose:     Utility to send packet and verify signals as a result
 * Requires:
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTTDM

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

#include <bcm/types.h>
#include <bcm/tdm.h>
#include <bcm/field.h>
#include <bcm/switch.h>
#include <bcm/fabric.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/algo/algo_tdm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tdm_access.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_ile.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx_dispatch.h>

#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/gport.h>
#include <shared/pbmp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/counter.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/sand/sand_pparse.h>

/*
 * Maximal number of soc properties on 'ctest_soc_property' array as passed
 * to 'ctest_dnxc_set_soc_properties'
 */
#define MAX_NUM_SOC_PROPERTIES 100
/*
 * Number of bytes occupied by OTCH at the beginning of a test message (to tx).
 */
#define BYTES_IN_PTCH                       2
/*
 * Number of bytes occupied by CRC at the end of a test message (to tx to external LAN).
 */
#define BYTES_EXTERNAL_CRC                  4
/*
 * Number of bytes occupied by CRC at the end of a packet as it moves inside the chip
 * from block to block.
 */
#define BYTES_INTERNAL_CRC                  2
/*
 * Number of bytes occupied in optimized FTMH header (used in bypass mode).
 * See dnx_tdm_bypass_test_cmd()
 */
#define OPT_FTMH_HEADER_SIZE    dnx_data_headers.optimized_ftmh.base_header_size_get(unit)
/*
 * Number of bytes occupied in standard FTMH header, for Jr2 (used in bypass mode).
 * See dnx_tdm_bypass_test_cmd()
 */
#define STD_FTMH_HEADER_SIZE    dnx_data_headers.ftmh.base_header_size_get(unit)
/*
 * Number of bytes occupied in standard FTMH header, for Jr2 (used in bypass mode).
 * See dnx_tdm_bypass_test_cmd()
 */
#define LEGACY_FTMH_HEADER_SIZE dnx_data_headers.ftmh_legacy.base_header_size_get(unit)
/*
 * This is the default value, as it should be loaded into DBAL table TDM_CONTEXT_MAP.
 * This context is assigned, then, to all strams that have not been assigned
 * on this table.
 */
#define DEFAULT_CONTEXT_PER_STREAM  0
/*
 * Maximal number of ports to add to multicast group (if required) in addition
 * to the standard 'destination port'.
 */
#define MAX_NUM_MCAST_PORTS         2

/*
 * generated MC ID when working in stamping mode is stream_id prefixed with a given prefix
 */
#define TDM_SID_TO_MCID(unit, sid, prefix) (sid + (prefix << dnx_data_tdm.params.sid_size_get(unit)))

/*
 * Header To Add Signal size (in Bytes)
 */
#define HEADER_TO_ADD_MAX_SIZE      160

/*
 *  OTMH 4-Byte Outgoing TDM Header
 *  31                    16 15                     0
 * |-------------------------------------------------|
 * |    MC_CUD (16 bits)    |     MC_ID (16 bits)    |
 * |-------------------------------------------------|
 */

/*
 * OTMH 4-Byte Outgoing TDM Header Size (in Bytes)
 */
#define OTMH_4BYTE_OUTGOING_TDM_HEADER_SIZE                  4

/*
 * OTMH 4-Byte Outgoing TDM Header: MC-ID offset in Bytes from LSB
 */
#define OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_ID_OFFSET          0

/*
 * OTMH 4-Byte Outgoing TDM Header: MC-CUD offset in Bytes from LSB
 */
#define OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_CUD_OFFSET         2

/*
 * OTMH 4-Byte Outgoing TDM Header: MC-CUD Field size (in bits)
 */
#define OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_CUD_BIT_SIZE       16

/*
 * OTMH 4-Byte Outgoing TDM Header: MC-ID Field size (in bits)
 */
#define OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_ID_BIT_SIZE        16


/* *INDENT-OFF* */
static sh_sand_enum_t sh_enum_editing_types[] = {
    {"nochange", bcmTdmIngressEditingNoChange, "Incoming FTMH Header is assumed."},
    {"prepend", bcmTdmIngressEditingPrepend, "FTMH Header is prepended at ingress"},
    {NULL}
};

static sh_sand_option_t dnx_tdm_context_test_arguments[] = {
    {"type", SAL_FIELD_TYPE_ENUM, "Context Editing Type", "Prepend", sh_enum_editing_types},
    {"destination", SAL_FIELD_TYPE_PORT, "Destination Port for Unicast Context", "eth13"},
    {"fabric", SAL_FIELD_TYPE_PORT, "Fabric ports for direct routing", "none"},
    {"multicast", SAL_FIELD_TYPE_UINT32, "Multicast ID for Multicast Context", "1025"},
    {"user_data", SAL_FIELD_TYPE_ARRAY32, "User Data", "0"},
    {"user_data_count", SAL_FIELD_TYPE_INT32, "User Data Bit Count", "0"},
    {"stamp", SAL_FIELD_TYPE_BOOL, "Stamp Stream ID in place of Multicast ID for Multicast Context", "No"},
    {"clean", SAL_FIELD_TYPE_BOOL, "Clean after test", "Yes"},
    {"mode", SAL_FIELD_TYPE_STR, "TDM Mode - TDM_OPTIMIZED/TDM_STANDARD/TDM_PACKET", "TDM_OPTIMIZED"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_context_test_man = {
    "TDM Context Semantic Tests"
};

static sh_sand_invoke_t dnx_tdm_context_tests[] = {
    {"mc_prepend", ""},
    {"mc_nochange", "type=nochange multicast=2048"},
    {"mc_nochange_fabric", "type=nochange multicast=2048 fabric=sfi"},
    {"mc_nochange_stamp", "type=nochange stamp multicast=0"},
    {"mc_prepende_stamp", "stamp multicast=0"},
    {"dst_prepend", "type=prepend dst=cpu200 mc=0"},
    {"dst_nochange", "dst=1 type=nochange mc=0"},
    {NULL}
};
/* *INDENT-ON* */

static shr_error_e
dnx_tdm_context_compare(
    int unit,
    int silent,
    int ingress_context_id,
    bcm_tdm_ingress_context_t * set_ingress_context,
    bcm_tdm_ingress_context_t * get_ingress_context,
    int *is_equal_p,
    sh_sand_control_t * sand_control)
{
    int is_equal = TRUE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("TDM Context Create/Get Comparison ");
    PRT_INFO_ADD("Context:%d", ingress_context_id);
    PRT_COLUMN_ADD("Parameters");
    PRT_COLUMN_ADD("Set Context");
    PRT_COLUMN_ADD("Get Context");
    PRT_COLUMN_ADD("Result");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Destination Port");
    PRT_CELL_SET("0x%08x", set_ingress_context->destination_port);
    PRT_CELL_SET("0x%08x", get_ingress_context->destination_port);
    if (set_ingress_context->destination_port != get_ingress_context->destination_port)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Multicast ID");
    PRT_CELL_SET("0x%08x", set_ingress_context->multicast_id);
    PRT_CELL_SET("0x%08x", get_ingress_context->multicast_id);
    if (set_ingress_context->multicast_id != get_ingress_context->multicast_id)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Stamp MCID with SID");
    PRT_CELL_SET("%d", set_ingress_context->stamp_mcid_with_sid);
    PRT_CELL_SET("%d", get_ingress_context->stamp_mcid_with_sid);
    if (set_ingress_context->stamp_mcid_with_sid != get_ingress_context->stamp_mcid_with_sid)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("is Multicast");
    PRT_CELL_SET("%d", set_ingress_context->is_mc);
    PRT_CELL_SET("%d", get_ingress_context->is_mc);
    if (set_ingress_context->is_mc != get_ingress_context->is_mc)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Fabric Ports Count");
    PRT_CELL_SET("%d", set_ingress_context->fabric_ports_count);
    PRT_CELL_SET("%d", get_ingress_context->fabric_ports_count);
    if (set_ingress_context->fabric_ports_count != get_ingress_context->fabric_ports_count)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        if (get_ingress_context->fabric_ports_count != 0)
        {
            int i_fabric;
            bcm_pbmp_t get_fabric_ports, set_fabric_ports;
            char get_fabric_ports_str[_SHR_PBMP_FMT_LEN] = { 0 }, set_fabric_ports_str[_SHR_PBMP_FMT_LEN] =
            {
            0};
            PRT_CELL_SET("OK");
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("Fabric Ports List");
            BCM_PBMP_CLEAR(set_fabric_ports);
            BCM_PBMP_CLEAR(get_fabric_ports);
            for (i_fabric = 0; i_fabric < get_ingress_context->fabric_ports_count; i_fabric++)
            {
                BCM_PBMP_PORT_ADD(set_fabric_ports, set_ingress_context->fabric_ports[i_fabric]);
                BCM_PBMP_PORT_ADD(get_fabric_ports, get_ingress_context->fabric_ports[i_fabric]);
            }
            _SHR_PBMP_FMT(get_fabric_ports, get_fabric_ports_str);
            _SHR_PBMP_FMT(set_fabric_ports, set_fabric_ports_str);
            PRT_CELL_SET("%s", set_fabric_ports_str);
            PRT_CELL_SET("%s", get_fabric_ports_str);
            if (sal_strcmp(set_fabric_ports_str, set_fabric_ports_str))
            {
                is_equal = FALSE;
                PRT_CELL_SET("Failed");
            }
            else
            {
                PRT_CELL_SET("OK");
            }
        }
        else
        {
            PRT_CELL_SET("OK");
        }
    }
    if (silent == FALSE)
    {
        PRT_COMMITX;
    }
    if (is_equal_p != NULL)
    {
        *is_equal_p = is_equal;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric priority mapping so that it has
 *   no 'is_tdm' and no 'TDM fabric priority'.
 *   This way, the setup of the test () may be properly checked.
 */
static shr_error_e
dnx_tdm_fabric_priority_map_no_tdm_init(
    int unit,
    int restore)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int dp = 0;
    int tc = 0;
    int is_ocb_only = 0;
    int min_hp_mc = dnx_data_fabric.pipes.map_get(unit)->min_hp_mc;
    uint32 fabric_priority = 0;
    uint32 is_tdm = 0;
    soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;
    /*
     * Local storage. Keep here all changed values, in DBAL_TABLE_FABRIC_CELL_PRIORITY, and
     * put them back, in place, if 'restore' (= clean) is required.
     * Dimensions are: is_hp, is_mc, dp, tc, is_ocb_only
     * Last dimension: '0' - stands for 'fabric_priority', '1' - stands for 'is_tdm'.
     */
    static int image_of_fabric_cell_priority[2][2][DNX_COSQ_NOF_DP][DNX_COSQ_NOF_TC][2][2] = { {{{{{0, 0}}}}} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));
        /*
         * Loop over all table indices
         */
        for (is_hp = 0; is_hp <= 1; ++is_hp)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);
            for (is_mc = 0; is_mc <= 1; ++is_mc)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
                for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
                    for (tc = 0; tc < DNX_COSQ_NOF_TC; ++tc)
                    {
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
                        for (is_ocb_only = 0; is_ocb_only <= 1; ++is_ocb_only)
                        {
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);
                            if ((dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET) && is_ocb_only)
                            {
                                /*
                                 * Packet-TDM: Clear away TDM and high fabric priority
                                 */
                                fabric_priority = 0;
                                is_tdm = 0;
                            }
                            else if (map_type == soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc)
                            {
                                /*
                                 * fabric_priority must be fully aligned with hp_bit since dtq uses hp_bit to decide queue id while FDR
                                 * uses fabric_priority to decide pipe id.  Therefore, in "uc / hp mc / lp mc" configuration => LP bit ==>
                                 * priority 0 (= lowest priority of lp mc) HP bit ==> lowest priority of hp mc
                                 */
                                fabric_priority = is_hp ? min_hp_mc : 0;
                                is_tdm = 0;
                            }
                            else
                            {
                                /*
                                 * tc=0,1,2 ==> prio=0
                                 * tc=3,4,5 ==> prio=1
                                 * tc=6,7   ==> prio=2
                                 */
                                fabric_priority = tc / 3;
                                is_tdm = 0;
                            }
                            if (restore)
                            {
                                /*
                                 * Read original values from local storage and, below, put them back into DBAL.
                                 */
                                fabric_priority = image_of_fabric_cell_priority[is_hp][is_mc][dp][tc][is_ocb_only][0];
                                is_tdm = image_of_fabric_cell_priority[is_hp][is_mc][dp][tc][is_ocb_only][1];
                            }
                            else
                            {
                                /*
                                 * Read original values from BAL and store in local storage.
                                 */
                                uint32 loc_fabric_priority;
                                uint32 loc_is_tdm;

                                loc_fabric_priority = 0;
                                loc_is_tdm = 0;
                                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY,
                                                           INST_SINGLE, &loc_fabric_priority);
                                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE,
                                                           &loc_is_tdm);
                                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                                image_of_fabric_cell_priority[is_hp][is_mc][dp][tc][is_ocb_only][0] =
                                    loc_fabric_priority;
                                image_of_fabric_cell_priority[is_hp][is_mc][dp][tc][is_ocb_only][1] = loc_is_tdm;
                            }
                            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE,
                                                         fabric_priority);
                            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                        }
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
 * \brief
 *   Given a bit-map of ports, get the first (and only!) port.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] logical_ports -
 *   Bit-map of ports. BIT0 represents port0, bit1 represents port1, etc.
 * \param [out] dst_port_p -
 *    Pointer to bcm_port_t. This procedure loads pointed memory by the
 *    single port that has been found.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
static shr_error_e
dnx_tdm_get_first_and_only_port(
    int unit,
    bcm_pbmp_t logical_ports,
    bcm_port_t * dst_port_p)
{
    int count;
    bcm_port_t dst_port;

    SHR_FUNC_INIT_VARS(unit);
    BCM_PBMP_COUNT(logical_ports, count);
    if (count != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There should be exactly one logical port in input bitmap\r\n");
    }
    _SHR_PBMP_FIRST(logical_ports, dst_port);
    *dst_port_p = dst_port;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given port speed (as per the configuration file of this specific chip),
 *   get the string representation of the port type. Also, if port type is 'interLaken',
 *   indicate that on a dedicated output variable.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] speed -
 *   Int. This is the speed, in bits per second, which identifies port type.
 * \param [out] is_ilkn_p -
 *    Pointer to int. This procedure loads pointed memory by TRUE if
 *    port type is 'interLaken'
 * \param [out] ucode_port_type_p -
 *    Pointer to char pointer. This procedure loads pointed memory by the pointer
 *    to a string which represents indicated port type.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
static shr_error_e
dnx_tdm_convert_speed_to_port_type_str(
    int unit,
    int speed,
    int *is_ilkn_p,
    char **ucode_port_type_p)
{
    char *ucode_port_type;

    SHR_FUNC_INIT_VARS(unit);
    *is_ilkn_p = FALSE;
    switch (speed)
    {
        case 400000:
        {
            ucode_port_type = "CDGE";
            break;
        }
        case 100000:
        {
            ucode_port_type = "CGE";
            break;
        }
        case 10000:
        {
            ucode_port_type = "XE";
            break;
        }
        case 25781:
        {
            ucode_port_type = "ILKN";
            *is_ilkn_p = TRUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed: %d\n", speed);
        }
    }
    *ucode_port_type_p = ucode_port_type;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given array of soc property names and parameters, fill array with
 *   input parameters.
 * \param [in] unit -
 *   int. HW identifier of unit
 * \param [out] ctest_soc_property_p -
 *   Pointer to ctest_soc_property_t. This procedure loads pointed array by required soc
 *   properties as specified plus some that are constant.
 * \param [in] soc_name -
 *   Pointer to array or string. This is the soc name to load into the soc property corresponding
 * \param [in] soc_value -
 *   Pointer to array or string. This is the soc value to load into the soc property corresponding
 */
static shr_error_e
dnx_ctest_soc_add(
    int unit,
    ctest_soc_property_t * ctest_soc_property_p,
    char *soc_name,
    char *soc_value)
{
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(soc_name, _SHR_E_PARAM, "soc_name");
    /*
     * Find the end of ctest_soc_property_p
     */
    while (ii < MAX_NUM_SOC_PROPERTIES && ctest_soc_property_p[ii].property != NULL)
    {
        ii++;
    }
    ctest_soc_property_p[ii].property = soc_name;
    ctest_soc_property_p[ii].value = soc_value;
    ii++;

    if (ii >= MAX_NUM_SOC_PROPERTIES)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY,
                     "Number of soc properties to change (%d) is larger than num elements on container (%d). Quit\r\n",
                     ii, MAX_NUM_SOC_PROPERTIES);
    }

    ctest_soc_property_p[ii].property = NULL;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given array of basic soc property names and parameters, fill array with
 *   the parameters tdm required.
 * \param [in] unit -
 *   int. HW identifier of unit
 * \param [out] ctest_soc_property_p -
 *   Pointer to ctest_soc_property_t. This procedure loads pointed array by required soc
 *   properties that are constant.
 */
static shr_error_e
dnx_tdm_base_soc_properties(
    int unit,
    ctest_soc_property_t * ctest_soc_property_p)
{
    /*
     * Make this array static since it is used by the caller. (The caller uses the
     * array which is pointed by entries of 'ctest_soc_property_p' and they point to
     * lane_to_serdes.
     */
    static char lane_to_serdes_name_all[8][RHNAME_MAX_SIZE];
    static char lane_to_serdes_name[8][RHNAME_MAX_SIZE];
    static char lane_to_serdes_value[8][RHNAME_MAX_SIZE];
    static char flexe_mode[RHNAME_MAX_SIZE];
    static char appl_enable_field_str[RHNAME_MAX_SIZE];

    /*
     * For Interlaken, need to change lane setup to enable internal loopback.
     * See "bcm88480_board.bcm", "bcm88800_board.bcm".
     */
    int lane_ids[8] = { 0, 1, 2, 3, 45, 46, 49, 50 };
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * For each soc property:
     * Erase "lane_to_serdes_map_nif_lane?"
     * Load "lane_to_serdes_map_nif_lane?.unit" with "rx?:tx?"
     *
     * rx0
     * rx1
     * rx2
     * rx3
     * rx45
     * rx46
     * rx49
     * rx50
     */
    for (ii = 0; ii < COUNTOF(lane_ids); ii++)
    {
        sal_snprintf(lane_to_serdes_name_all[ii], RHNAME_MAX_SIZE, "%s%d.*", "lane_to_serdes_map_nif_lane",
                     lane_ids[ii]);
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property_p, lane_to_serdes_name_all[ii], NULL));
        sal_snprintf(lane_to_serdes_name[ii], RHNAME_MAX_SIZE, "%s%d", "lane_to_serdes_map_nif_lane", lane_ids[ii]);
        sal_snprintf(lane_to_serdes_value[ii], RHNAME_MAX_SIZE, "rx%d:tx%d", lane_ids[ii], lane_ids[ii]);
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add
                        (unit, ctest_soc_property_p, lane_to_serdes_name[ii], lane_to_serdes_value[ii]));
    }

    /*
     * This is to let the test use 'ucode_port_6': On the standard
     * initialization of 88480, ucode_port_5 is on the same interface as ucode_port_6.
     */
    SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property_p, "ucode_port_5.*", NULL));
    /*
     * This is to free the test from 'flexe' ports. 'ucode_port_20' is defined as 'flexe' on the standard
     * initialization of 88480.
     */
    SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property_p, "ucode_port_20.*", NULL));
    /*
     * This is required only for 'packet' mode.
     * It makes sure that all extra extensions, to FTMH header, are NOT
     * applied.
     */
    sal_snprintf(appl_enable_field_str, RHNAME_MAX_SIZE, "%s.%d", "appl_enable_field_app", unit);
    SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property_p, appl_enable_field_str, "0"));

    sal_snprintf(flexe_mode, RHNAME_MAX_SIZE, "%s.%d", "flexe_device_mode", unit);
    SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property_p, flexe_mode, "DISABLED"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given array of soc property descriptors and parameters, fill array with
 *   input parameters.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] value_of_fabric_connect_mode -
 *   Pointer to string. This is the value to load into 'fabric_connect_mode' soc property.
 *   If this pointer is NULL then leave this soc property alone.
 * \param [in] value_of_tdm_mode -
 *   Pointer to string. This is the value to load into 'tdm_mode' soc property.
 * \param [in] value_of_flexe_mode -
 *   Pointer to string. This is the value to load into 'flexe_device_mode' soc property.
 * \param [in] name_of_all_src -
 *   Pointer to string. This is the wildcard name representing all soc properties corresponding
 *   to 'src' port.
 * \param [in] name_of_src -
 *   Pointer to string. This is the name of the soc property corresponding
 *   to 'src' port.
 * \param [in] value_of_src -
 *   Pointer to string. This is the value to load into the soc property corresponding
 *   to 'src' port.
 * \param [in] name_of_dst -
 *   Pointer to string. This is the name of the soc property corresponding
 *   to 'dst' port.
 * \param [in] value_of_dst -
 *   Pointer to string. This is the value to load into the soc property corresponding
 *   to 'dst' port.
 * \param [in] name_of_all_dst -
 *   Pointer to string. This is the wildcard name representing all soc properties corresponding
 *   to 'dst' port.
 * \param [in] num_of_mcast_ports -
 *   Int. Number of multicast ports described in the next three input parameters.
 * \param [in] name_of_mcast -
 *   Pointer to array of strings (two dimensional: char [][]). First index is identifier of
 *   the multicast port. Second index is the name of the soc property corresponding to 'mcast'
 *   port (added to multicast group, if any). If this parameter is NULL then 'num_of_mcast_ports'
 *   is assumed to be '0'.
 * \param [in] value_of_mcast -
 *   Pointer to array of strings (two dimensional: char [][]). First index is identifier of
 *   the multicast port. Second index is the value to load into the soc property corresponding
 *   to 'mcast' port.
 * \param [in] name_of_all_mcast -
 *   Pointer to array of strings (two dimensional: char [][]).First index is identifier of
 *   the multicast port. Second index is the wildcard name representing all soc properties corresponding
 *   to 'mcast' port.
 * \param [in] stream_multicast_prefix -
 *    Int. multicast ID prefix
 * \param [in] num_elements_on_ctest_soc_property -
 *    Int. Number of elements reserved, by the caller, on array pointed by 'ctest_soc_property_p'.
 * \param [out] ctest_soc_property_p -
 *    Pointer to ctest_soc_property_t. This procedure loads pointed array by all required soc
 *    properties as specified above plus some that are constant and do not depend on
 *    direct input.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    dnx_tdm_bypass_test_cmd
 */
static shr_error_e
dnx_tdm_load_soc_properties(
    int unit,
    char *value_of_fabric_connect_mode,
    char *value_of_tdm_mode,
    char *value_of_flexe_mode,
    char *name_of_all_src,
    char *name_of_src,
    char *value_of_src,
    char *name_of_dst,
    char *value_of_dst,
    char *name_of_all_dst,
    int num_of_mcast_ports,
    char name_of_mcast[][RHNAME_MAX_SIZE],
    char value_of_mcast[][RHNAME_MAX_SIZE],
    char name_of_all_mcast[][RHNAME_MAX_SIZE],
    int stream_multicast_prefix,
    int num_elements_on_ctest_soc_property,
    ctest_soc_property_t * ctest_soc_property_p)
{
    int ii;
    int mcast_index;
    /*
     * Make this array static since it is used by the caller. (The caller uses the
     * array which is pointed by entries of 'ctest_soc_property_p' and they point to
     * lane_to_serdes.
     */
    static char lane_to_serdes[8 * 3][RHNAME_MAX_SIZE];
    static char flexe_mode[RHNAME_MAX_SIZE];
    static char nof_egress_mc_groups[RHNAME_MAX_SIZE];
    static char appl_enable_field_str[RHNAME_MAX_SIZE];
    static char fabric_connect_mode_str[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    ii = 0;
    {
        /*
         * For Interlaken, need to change lane setup to enable internal loopback.
         * See "bcm88480_board.bcm", "bcm88800_board.bcm".
         */
        int lane_number;
        int lane_to_serdes_index;
        char *lane_str;
        int rx_number;
        int num_entries;

        lane_str = "lane_to_serdes_map_nif_lane";
        lane_to_serdes_index = 0;
        /*
         * For each soc property:
         * Erase "lane_to_serdes_map_nif_lane?"
         * Load "lane_to_serdes_map_nif_lane?.unit" with "rx?:tx?"
         *
         * rx0
         * rx1
         * rx2
         * rx3
         * rx45
         * rx46
         * rx49
         * rx50
         */
        lane_number = 0;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 0;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 1;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 1;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 2;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 2;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 3;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 3;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 45;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 45;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 46;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 46;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 49;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 49;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        lane_number = 50;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d", lane_str, lane_number);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        ctest_soc_property_p[ii++].value = NULL;
        lane_to_serdes_index++;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "%s%d.%d", lane_str, lane_number, unit);
        ctest_soc_property_p[ii].property = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;
        rx_number = 50;
        sal_snprintf(lane_to_serdes[lane_to_serdes_index], RHNAME_MAX_SIZE, "rx%d:tx%d", rx_number, rx_number);
        ctest_soc_property_p[ii++].value = lane_to_serdes[lane_to_serdes_index];
        lane_to_serdes_index++;

        num_entries = sizeof(lane_to_serdes) / sizeof(lane_to_serdes[0]);
        if (lane_to_serdes_index > num_entries)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY,
                         "Number of elements on lane_to_serdes (%d) is smaller than required (%d). Quit\r\n",
                         num_entries, lane_to_serdes_index);
        }
        /*
         * This is to let the test use 'ucode_port_6': On the standard
         * initialization of 88480, ucode_port_5 is on the same interface as ucode_port_6.
         */
        ctest_soc_property_p[ii].property = "ucode_port_5.*";
        ctest_soc_property_p[ii++].value = NULL;
        /*
         * This is to free the test from 'flexe' ports. 'ucode_port_20' is defined as 'flexe' on the standard
         * initialization of 88480.
         */
        ctest_soc_property_p[ii].property = "ucode_port_20.*";
        ctest_soc_property_p[ii++].value = NULL;
    }
    /*
     * This is required only for 'packet' mode.
     * It makes sure that all extra extensions, to FTMH header, are NOT
     * applied.
     */
    sal_snprintf(appl_enable_field_str, RHNAME_MAX_SIZE, "%s.%d", "appl_enable_field_app", unit);
    ctest_soc_property_p[ii].property = appl_enable_field_str;
    ctest_soc_property_p[ii++].value = "0";

    if (value_of_fabric_connect_mode != NULL)
    {
        sal_snprintf(fabric_connect_mode_str, RHNAME_MAX_SIZE, "%s.%d", "fabric_connect_mode", unit);
        ctest_soc_property_p[ii].property = fabric_connect_mode_str;
        ctest_soc_property_p[ii++].value = value_of_fabric_connect_mode;
    }

    ctest_soc_property_p[ii].property = "tdm_mode";
    ctest_soc_property_p[ii++].value = value_of_tdm_mode;
    sal_snprintf(flexe_mode, RHNAME_MAX_SIZE, "%s.%d", "flexe_device_mode", unit);
    ctest_soc_property_p[ii].property = flexe_mode;
    ctest_soc_property_p[ii++].value = value_of_flexe_mode;
    if (name_of_all_src != NULL)
    {
        ctest_soc_property_p[ii].property = name_of_all_src;
        ctest_soc_property_p[ii++].value = NULL;
    }
    if (name_of_src != NULL)
    {
        ctest_soc_property_p[ii].property = name_of_src;
        ctest_soc_property_p[ii++].value = value_of_src;
    }
    {
        /*
         * We always expect to have a destination port even if the destination is multicast.
         * In the latter case, the destination port is added to the destination multicast group.
         * So we need to update the standard setup (from config-q2a.bcm).
         */
        ctest_soc_property_p[ii].property = name_of_all_dst;
        ctest_soc_property_p[ii++].value = NULL;
        ctest_soc_property_p[ii].property = name_of_dst;
        ctest_soc_property_p[ii++].value = value_of_dst;
    }
    if (name_of_mcast != NULL)
    {
        /*
         * These are the ports which are added to the multicast destination group (if any)
         */
        for (mcast_index = 0; mcast_index < num_of_mcast_ports; mcast_index++)
        {
            ctest_soc_property_p[ii].property = name_of_all_mcast[mcast_index];
            ctest_soc_property_p[ii++].value = NULL;
            ctest_soc_property_p[ii].property = name_of_mcast[mcast_index];
            ctest_soc_property_p[ii++].value = value_of_mcast[mcast_index];
        }
    }

    /** set nof egress multicast groups */
    {
        /** set number of egress multicast groups as a multiple of multicast prefix (and at least 16k) */
        sal_snprintf(nof_egress_mc_groups, RHNAME_MAX_SIZE, "%d",
                     TDM_SID_TO_MCID(unit, dnx_data_tdm.params.nof_stream_ids_get(unit), stream_multicast_prefix));
        ctest_soc_property_p[ii].property = "multicast_egress_group_id_range_max";
        ctest_soc_property_p[ii++].value = NULL;
        ctest_soc_property_p[ii].property = "multicast_egress_group_id_range_max";
        ctest_soc_property_p[ii++].value = nof_egress_mc_groups;
    }
    if (ii >= num_elements_on_ctest_soc_property)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY,
                     "Number of soc properties to change (%d) is larger than num elements on container (%d). Quit\r\n",
                     ii, num_elements_on_ctest_soc_property);
    }
    /*
     * Fill in 'end of list' indication.
     */
    ctest_soc_property_p[ii].property = NULL;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given source port and destination port, get counters for each:
 *   No. of received bytes and number of receive errors for 'source' port.
 *   No. of transmitted bytes and number of transmit errors for 'destination' port.
 *   .
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] this_is_precall -
 *   int. This is a binary flag. If non-zero then this invocation is made before transmission
 *   just to get the base line of the counters. Otherwise, this is invocation after
 *   transmission and counter is extracted by reading and, then, reducing the base
 *   line. Note that, for ILKN, we add, artificially, the size of ETHERNET CRC since
 *   we may compare ILKN and ETHERNET and ETHERNET counters, internally, include CRC
 * \param [in] src_port -
 *   bcm_port_t. Source port for which to get counters. Data is expected to be received
 *   on this port and, then, transmitted to the destination port.
 *   If negative then 'octets_rx_from_src_port_p' and 'err_pkt_rx_from_src_port_p' are NOT
 *   loaded.
 * \param [in] dst_port -
 *   bcm_port_t. Destination port for which to get counters. Data is expected to be received
 *   on source port and, then, transmitted to this port.
 *   If negative then 'octets_tx_from_dst_port_p' and 'err_pkt_tx_from_dst_port_p' are NOT
 *   loaded.
 * \param [in,out] octets_rx_from_src_port_p -
 *   Pointer to uint32. This procedure reads pointed memory and subtracts it from the
 *   current counter value of 'received octets' on source port. The result is loaded
 *   into pointed memory.
 * \param [in,out] octets_tx_from_dst_port_p -
 *   Pointer to uint32. This procedure reads pointed memory and subtracts it from the
 *   current counter value of 'tranmitted octets' on destination port. The result is loaded
 *   into pointed memory.
 * \param [out] err_pkt_rx_from_src_port_p -
 *   Pointer to uint32. This procedure reads pointed memory and subtracts it from the
 *   current counter value of 'received errors' on source port. The result is loaded
 *   into pointed memory.
 * \param [out] err_pkt_tx_from_dst_port_p -
 *   Pointer to uint32. This procedure reads pointed memory and subtracts it from the
 *   current counter value of 'tranmitted errors' on destination port. The result is loaded
 *   into pointed memory.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
static shr_error_e
dnx_tdm_load_src_dst_counters(
    int unit,
    int this_is_precall,
    bcm_port_t src_port,
    bcm_port_t dst_port,
    uint32 *octets_rx_from_src_port_p,
    uint32 *octets_tx_from_dst_port_p,
    uint32 *err_pkt_rx_from_src_port_p,
    uint32 *err_pkt_tx_from_dst_port_p)
{
    bcm_stat_val_t stat_type;
    uint64 uint64_value_tx;
    uint64 uint64_value_rx;
    uint32 uint32_work_reg;
    dnx_algo_port_info_s src_port_info, dst_port_info;

    SHR_FUNC_INIT_VARS(unit);
    if (src_port > 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &src_port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, src_port_info, FALSE, TRUE))
        {
            stat_type = snmpIfInOctets;
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, src_port, stat_type, &uint64_value_rx));
            *octets_rx_from_src_port_p = COMPILER_64_LO(uint64_value_rx) - *octets_rx_from_src_port_p;
            stat_type = snmpIfInErrors;
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, src_port, stat_type, &uint64_value_rx));
            *err_pkt_rx_from_src_port_p = COMPILER_64_LO(uint64_value_rx) - *err_pkt_rx_from_src_port_p;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, src_port_info, FALSE, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_mib_ilkn_counter_get
                            (unit, dnx_mib_counter_ilkn_rx_bytes, src_port, &uint64_value_rx, NULL));
            uint32_work_reg = COMPILER_64_LO(uint64_value_rx) - (*octets_rx_from_src_port_p);
            if (!this_is_precall)
            {
                /*
                 * On 'post transmission call', we add size of CRC so we can compare with ETHERNET.
                 */
                *octets_rx_from_src_port_p = uint32_work_reg + BYTES_EXTERNAL_CRC;
            }
            else
            {
                *octets_rx_from_src_port_p = uint32_work_reg;
            }
            SHR_IF_ERR_EXIT(dnx_mib_ilkn_counter_get
                            (unit, dnx_mib_counter_ilkn_rx_err_pkt, src_port, &uint64_value_rx, NULL));
            *err_pkt_rx_from_src_port_p = COMPILER_64_LO(uint64_value_rx) - *err_pkt_rx_from_src_port_p;
        }
    }
    if (dst_port > 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dst_port, &dst_port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, dst_port_info, FALSE, TRUE))
        {
            stat_type = snmpIfOutOctets;
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, dst_port, stat_type, &uint64_value_tx));
            *octets_tx_from_dst_port_p = COMPILER_64_LO(uint64_value_tx) - *octets_tx_from_dst_port_p;
            stat_type = snmpIfOutErrors;
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, dst_port, stat_type, &uint64_value_tx));
            *err_pkt_tx_from_dst_port_p = COMPILER_64_LO(uint64_value_tx) - *err_pkt_tx_from_dst_port_p;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, dst_port_info, FALSE, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_mib_ilkn_counter_get
                            (unit, dnx_mib_counter_ilkn_tx_bytes, dst_port, &uint64_value_tx, NULL));
            uint32_work_reg = COMPILER_64_LO(uint64_value_tx) - (*octets_tx_from_dst_port_p);
            if (!this_is_precall)
            {
                /*
                 * On 'post transmission call', we add size of CRC so we can compare with ETHERNET.
                 */
                *octets_tx_from_dst_port_p = uint32_work_reg + BYTES_EXTERNAL_CRC;
            }
            else
            {
                *octets_tx_from_dst_port_p = uint32_work_reg;
            }
            SHR_IF_ERR_EXIT(dnx_mib_ilkn_counter_get
                            (unit, dnx_mib_counter_ilkn_tx_err_pkt, dst_port, &uint64_value_tx, NULL));
            *err_pkt_tx_from_dst_port_p = COMPILER_64_LO(uint64_value_tx) - *err_pkt_tx_from_dst_port_p;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_context_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_tdm_ingress_context_t ingress_context, get_ingress_context;
    int ingress_context_id;
    int to_clean = FALSE, dst_present;
    uint32 flags = 0;
    int is_equal = TRUE;
    char *tdm_mode_str;
    char *flexe_mode_str;
    uint32 *user_data;
    rhhandle_t ctest_soc_set_h = NULL;
    char *test_name;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("clean", to_clean);
    test_name = "dnx_tdm_context_test_cmd";
    bcm_tdm_ingress_editing_context_t_init(&ingress_context);

    SH_SAND_GET_STR("mode", tdm_mode_str);
    flexe_mode_str = "DISABLED";
    if (!ISEMPTY(tdm_mode_str))
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"tdm_mode", tdm_mode_str}
            ,
            {"flexe_device_mode.BCM8848X", flexe_mode_str}
            ,
            {"flexe_device_mode.BCM8880X", flexe_mode_str}
            ,
            {"ucode_port*", NULL}
            ,
            {NULL}
        };

        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    SH_SAND_GET_ENUM("type", ingress_context.type);

    SH_SAND_GET_BOOL("stamp", ingress_context.stamp_mcid_with_sid);

    SH_SAND_GET_INT32("user_data_count", ingress_context.user_data_count);
    SH_SAND_GET_ARRAY32("user_data", user_data);

    utilex_U32_to_U8(user_data, BCM_TDM_USER_DATA_MAX_SIZE, ingress_context.user_data);

    {
        bcm_pbmp_t fabric_ports;
        bcm_port_t fabric_port;
        SH_SAND_GET_PORT("fabric", fabric_ports);
        BCM_PBMP_ITER(fabric_ports, fabric_port)
        {
            ingress_context.fabric_ports[ingress_context.fabric_ports_count++] = fabric_port;
        }
    }

    SH_SAND_IS_PRESENT("destination", dst_present);
    if (dst_present == TRUE)
    {
        bcm_pbmp_t logical_ports;
        bcm_port_t port = 0;
        int count;
        SH_SAND_GET_PORT("destination", logical_ports);
        BCM_PBMP_COUNT(logical_ports, count);
        if (count != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "There should be exactly one logical port in destination\n");
        }
        BCM_PBMP_ITER(logical_ports, port)
        {
            BCM_GPORT_MODPORT_SET(ingress_context.destination_port, unit, port);
            ingress_context.is_mc = FALSE;
            break;
        }
    }
    else
    {
        int mc_id;
        ingress_context.is_mc = TRUE;
        SH_SAND_GET_UINT32("multicast", mc_id);
        _BCM_MULTICAST_GROUP_SET(ingress_context.multicast_id, 0, mc_id);
    }

    /*
     * Set TDM editing context configuration
     */
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_create(unit, flags, &ingress_context_id, &ingress_context));
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_get(unit, 0, ingress_context_id, &get_ingress_context));
    SHR_IF_ERR_EXIT(dnx_tdm_context_compare(unit, FALSE, ingress_context_id,
                                            &ingress_context, &get_ingress_context, &is_equal, sand_control));
    if (to_clean == TRUE)
    {
        SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_destroy(unit, flags, ingress_context_id));
    }
    if (is_equal == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
exit:
    if (to_clean == TRUE)
    {
        shr_error_e rv;
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_context_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        SH_SAND_GET_ENUM("type", ingress_context.type);
        SH_SAND_GET_BOOL("stamp", ingress_context.stamp_mcid_with_sid);
        SH_SAND_GET_INT32("user_data_count", ingress_context.user_data_count);
        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "type %d, stamp %d user_data_count %d\r\n",
                   test_name, ingress_context.type, ingress_context.stamp_mcid_with_sid,
                   ingress_context.user_data_count);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_tdm_failover_test_arguments[] = {
    {"multicast", SAL_FIELD_TYPE_UINT32, "Multicast ID for Multicast Context", "1025"},
    {"clean", SAL_FIELD_TYPE_BOOL, "Clean after test", "Yes"},
    {"count", SAL_FIELD_TYPE_INT32, "Number of contexts to work with", "1"},
    {"step", SAL_FIELD_TYPE_INT32, "Step in context ID value", "1"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_failover_test_man = {
    "TDM Context Failover Semantic Tests"
};

static sh_sand_invoke_t dnx_tdm_failover_tests[] = {
    {"count_300_step_3", "count=300 step=3"},
    {"count_700_step_1", "multicast=2048 count=700"},
    {NULL}
};

static shr_error_e
dnx_tdm_failover_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_tdm_ingress_context_t ingress_context, get_ingress_context;
    int to_clean = FALSE;
    uint32 flags = 0;
    int is_equal = TRUE;
    int mc_id;
    int context_count, context_step, i_context, context_id;
    rhhandle_t ctest_soc_set_h = NULL;
    int context_ids[DNX_DATA_MAX_TDM_PARAMS_NOF_CONTEXTS];
    int set_context_ens[DNX_DATA_MAX_TDM_PARAMS_NOF_CONTEXTS];
    int get_context_ens[DNX_DATA_MAX_TDM_PARAMS_NOF_CONTEXTS];
    char *flexe_mode_str;
    char *test_name;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("clean", to_clean);
    test_name = "dnx_tdm_failover_test_cmd";
    flexe_mode_str = "DISABLED";
    bcm_tdm_ingress_editing_context_t_init(&ingress_context);

    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"tdm_mode", "TDM_OPTIMIZED"}
            ,
            {"flexe_device_mode.BCM8848X", flexe_mode_str}
            ,
            {"flexe_device_mode.BCM8880X", flexe_mode_str}
            ,
            {"ucode_port*", NULL},
            {NULL}
        };

        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    /*
     * Generate some contexts with predefined data - with only difference being mc_id
     */
    ingress_context.type = bcmTdmIngressEditingPrepend;
    ingress_context.is_mc = TRUE;
    flags |= BCM_TDM_INGRESS_CONTEXT_WITH_ID;
    SH_SAND_GET_UINT32("multicast", mc_id);
    SH_SAND_GET_INT32("count", context_count);
    SH_SAND_GET_INT32("step", context_step);
    for (i_context = 0; i_context < context_count; i_context++)
    {
        _BCM_MULTICAST_GROUP_SET(ingress_context.multicast_id, 0, mc_id + i_context);
        /*
         * Set TDM editing context configuration
         */
        context_id = i_context * context_step;
        SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_create(unit, flags, &context_id, &ingress_context));
        SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_get(unit, 0, context_id, &get_ingress_context));
        SHR_IF_ERR_EXIT(dnx_tdm_context_compare(unit, TRUE, context_id,
                                                &ingress_context, &get_ingress_context, &is_equal, sand_control));
        context_ids[i_context] = context_id;
        set_context_ens[i_context] = TRUE;
    }
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_set(unit, 0, context_count, context_ids, set_context_ens));
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_get(unit, 0, context_count, context_ids, get_context_ens));
    /*
     * Check set contexts for failover enabled
     */
    for (i_context = 0; i_context < context_count; i_context++)
    {
        if (set_context_ens[i_context] != get_context_ens[i_context])
        {
            is_equal = FALSE;
            LOG_CLI(("Failover state for context:%d is:%d instead of:%d\n", context_ids[i_context],
                     get_context_ens[i_context], set_context_ens[i_context]));
            break;
        }
    }
    /*
     * Now set all these context to disable failover
     */
    for (i_context = 0; i_context < context_count; i_context++)
    {
        set_context_ens[i_context] = FALSE;
    }
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_set(unit, 0, context_count, context_ids, set_context_ens));
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_get(unit, 0, context_count, context_ids, get_context_ens));
    /*
     * Check set contexts for failover disabled
     */
    for (i_context = 0; i_context < context_count; i_context++)
    {
        if (set_context_ens[i_context] != get_context_ens[i_context])
        {
            is_equal = FALSE;
            LOG_CLI(("Failover state for context:%d is:%d instead of:%d\n", context_ids[i_context],
                     get_context_ens[i_context], set_context_ens[i_context]));
            break;
        }
    }
    if (to_clean == TRUE)
    {
        for (i_context = 0; i_context < context_count; i_context++)
        {
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_destroy(unit, 0, context_ids[i_context]));
        }
    }
    if (is_equal == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
exit:
    if (to_clean == TRUE)
    {
        shr_error_e rv;
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_failover_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        SH_SAND_GET_UINT32("multicast", mc_id);
        SH_SAND_GET_INT32("count", context_count);
        SH_SAND_GET_INT32("step", context_step);
        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "multicast %d, count %d step %d\r\n", test_name, mc_id, context_count, context_step);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_interface_compare(
    int unit,
    int silent,
    bcm_gport_t gport,
    bcm_tdm_interface_config_t * set_interface_config,
    bcm_tdm_interface_config_t * get_interface_config,
    int *is_equal_p,
    sh_sand_control_t * sand_control)
{
    int is_equal = TRUE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("TDM Interface Config Create/Get Comparison ");
    PRT_INFO_ADD("Port:0x08%x", gport);
    PRT_COLUMN_ADD("Parameters");
    PRT_COLUMN_ADD("Set Context");
    PRT_COLUMN_ADD("Get Context");
    PRT_COLUMN_ADD("Result");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Stream Base");
    PRT_CELL_SET("%d", set_interface_config->stream_id_base);
    PRT_CELL_SET("%d", get_interface_config->stream_id_base);
    if (set_interface_config->stream_id_base != get_interface_config->stream_id_base)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Stream Enable");
    PRT_CELL_SET("%d", set_interface_config->stream_id_enable);
    PRT_CELL_SET("%d", get_interface_config->stream_id_enable);
    if (set_interface_config->stream_id_enable != get_interface_config->stream_id_enable)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Key Size");
    PRT_CELL_SET("%d", set_interface_config->stream_id_key_size);
    PRT_CELL_SET("%d", get_interface_config->stream_id_key_size);
    if (set_interface_config->stream_id_key_size != get_interface_config->stream_id_key_size)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Key Offset");
    PRT_CELL_SET("%d", set_interface_config->stream_id_key_offset);
    PRT_CELL_SET("%d", get_interface_config->stream_id_key_offset);
    if (set_interface_config->stream_id_key_offset != get_interface_config->stream_id_key_offset)
    {
        is_equal = FALSE;
        PRT_CELL_SET("Failed");
    }
    else
    {
        PRT_CELL_SET("OK");
    }

    if (silent == FALSE)
    {
        PRT_COMMITX;
    }
    if (is_equal_p != NULL)
    {
        *is_equal_p = is_equal;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/* *INDENT-OFF* */
static sh_sand_enum_t dnx_tdm_interface_channel_id_enum_table[] = {
    {"MIN", 0, "Minimal value for 'channel' to add to 'base' to get 'global SID' for ILKN"},
    {"MAX", 0, "Maximal value for 'channel' to add to 'base' to get 'global SID' for ILKN", "DNX_DATA.port.general.max_nof_channels-1"},
    {NULL}
};
/**
 * Enum for options of 'cell_size' field (Number of bytes allowed pe cell. If received packet is too
 * large, the 'packet mode' is assumed and cells are cut into pieces and, then, reconstructed.)
 */
static sh_sand_enum_t dnx_tdm_cell_size_enum_table[] = {
    {"MIN", 0, "Minimal value for 'cell size' under TDM.", "DNX_DATA.tdm.params.pkt_size_lower_limit"},
    {"MAX", 0, "Maximal value for 'cell size' under TDM.", "DNX_DATA.tdm.params.pkt_size_upper_limit"},
    {NULL}
};
/**
 * Enum for option of 'base' field (base added to SID to get 'global SID' to
 * use as index into 'TDM flow to context' table)
 */
static sh_sand_enum_t dnx_tdm_interface_base_enum_table[] = {
    {"MIN", 0, "Minimal value for SID 'base'"},
    {"MAX", 0, "Maximal value for SID 'base'", "DNX_DATA.tdm.params.context_map_base_nof-1"},
    {NULL}
};
/**
 * Enum for option of 'bits_in_key' field
 */
static sh_sand_enum_t dnx_tdm_interface_bits_in_key_enum_table[] = {
    {"MIN", 1, "Minimal value for number of bits in key", "DNX_DATA.tdm.params.sid_min_size"},
    {"MAX", 0, "Maximal value for number of bits in key", "DNX_DATA.tdm.params.sid_size"},
    {NULL}
};
/**
 * Enum for option of 'offset' field
 */
static sh_sand_enum_t dnx_tdm_interface_offset_enum_table[] = {
    {"MIN", 0, "Minimal value for 'offset' field (in bits)"},
    {"MAX", 0, "Maximal value for 'offset' field (in bits)", "DNX_DATA.tdm.params.sid_offset_nof_in_bits-1"},
    {NULL}
};

static sh_sand_option_t dnx_tdm_interface_test_arguments[] = {
    {"clean",         SAL_FIELD_TYPE_BOOL, "Clean after test", "Yes"},
    {"base",          SAL_FIELD_TYPE_INT32, "Offset, in mapping table, for first stream", "MIN", (void *) dnx_tdm_interface_base_enum_table, "MIN-MAX"},
    {"bits_in_key",   SAL_FIELD_TYPE_INT32, "Number of bits in key", "MIN", (void *) dnx_tdm_interface_bits_in_key_enum_table, "MIN-MAX"},
    {"offset",        SAL_FIELD_TYPE_INT32, "Key offset in bits", "MIN", (void *) dnx_tdm_interface_offset_enum_table, "MIN-MAX"},
    {"stream_enable", SAL_FIELD_TYPE_BOOL, "Stream enabled", "Yes"},
    {"mode",          SAL_FIELD_TYPE_STR, "TDM Mode - TDM_OPTIMIZED/TDM_STANDARD/TDM_PACKET", "TDM_OPTIMIZED"},
    {"port",          SAL_FIELD_TYPE_PORT, "Destination Port for Unicast Context", "eth13"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_interface_test_man = {
    "TDM Interface Configuration Semantic Tests"
};

static sh_sand_invoke_t dnx_tdm_interface_tests[] = {
    {"base_0_stream_first_4", "port=1 bits_in_key=4"},
    {"base_0_no_stream",      "stream_enable=no port=14"},
    {"base_1024_vlan_key",    "base=1024 bits_in_key=12 offset=116"},
    {NULL}
};
/* *INDENT-OFF* */

static shr_error_e
dnx_tdm_interface_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int to_clean = FALSE;
    int is_equal = TRUE;
    rhhandle_t ctest_soc_set_h = NULL;
    bcm_tdm_interface_config_t set_interface_config, get_interface_config;
    bcm_gport_t gport;
    bcm_pbmp_t logical_ports;
    bcm_port_t port = 0;
    int core_id = 0;
    int nif_interface_id = 0;
    uint32 tm_port;
    char ucode_port_name[RHNAME_MAX_SIZE];
    char ucode_port_value[RHNAME_MAX_SIZE];
    char *test_name = "dnx_tdm_interface_test_cmd";
    char *flexe_mode_str;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("clean", to_clean);
    flexe_mode_str = "DISABLED";
    sal_memset(&set_interface_config, 0, sizeof(set_interface_config));
    sal_memset(&get_interface_config, 0, sizeof(get_interface_config));
    SH_SAND_GET_PORT("port", logical_ports);
    _SHR_PBMP_FIRST(logical_ports, port);
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core_id, &nif_interface_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core_id, &tm_port));
    sal_snprintf(ucode_port_name, RHNAME_MAX_SIZE, "ucode_port_%d", port);
    sal_snprintf(ucode_port_value, RHNAME_MAX_SIZE, "XE%d:core_%d.%d:if_tdm_hybrid", nif_interface_id, core_id,
                 tm_port);
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"tdm_mode", "TDM_OPTIMIZED"}
            ,
            {"flexe_device_mode.BCM8848X", flexe_mode_str}
            ,
            {"flexe_device_mode.BCM8880X", flexe_mode_str}
            ,
            {"ucode_port*", NULL}
            ,
            {ucode_port_name, ucode_port_value}
            ,
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }
    BCM_GPORT_LOCAL_SET(gport, port);

    SH_SAND_GET_INT32("base", set_interface_config.stream_id_base);
    SH_SAND_GET_INT32("bits_in_key", set_interface_config.stream_id_key_size);
    SH_SAND_GET_INT32("offset", set_interface_config.stream_id_key_offset);
    SH_SAND_GET_BOOL("stream_enable", set_interface_config.stream_id_enable);

    SHR_IF_ERR_EXIT(bcm_tdm_interface_config_set(unit, 0, gport, &set_interface_config));
    SHR_IF_ERR_EXIT(bcm_tdm_interface_config_get(unit, 0, gport, &get_interface_config));
    SHR_IF_ERR_EXIT(dnx_tdm_interface_compare(unit, FALSE, gport,
                                              &set_interface_config, &get_interface_config, &is_equal, sand_control));

    if (to_clean == TRUE)
    {
        sal_memset(&set_interface_config, 0, sizeof(set_interface_config));
        set_interface_config.stream_id_key_size = dnx_data_tdm.params.sid_min_size_get(unit);
        SHR_IF_ERR_EXIT(bcm_tdm_interface_config_set(unit, 0, gport, &set_interface_config));
        SHR_IF_ERR_EXIT(bcm_tdm_interface_config_get(unit, 0, gport, &get_interface_config));
        SHR_IF_ERR_EXIT(dnx_tdm_interface_compare(unit, FALSE, gport,
                                                  &set_interface_config, &get_interface_config, &is_equal,
                                                  sand_control));
    }
    if (is_equal == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
exit:
    if (to_clean == TRUE)
    {
        shr_error_e rv;
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_interface_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        SH_SAND_GET_INT32("base", set_interface_config.stream_id_base);
        SH_SAND_GET_INT32("bits_in_key", set_interface_config.stream_id_key_size);
        SH_SAND_GET_INT32("offset", set_interface_config.stream_id_key_offset);
        SH_SAND_GET_BOOL("stream_enable", set_interface_config.stream_id_enable);
        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "port %d, nif_interface_id %d stream_id_base %d\r\n",
                   test_name, port, nif_interface_id, set_interface_config.stream_id_base);
        LOG_CLI_EX("stream_id_key_size %d, stream_id_key_offset %d stream_id_enable %d %s\r\n",
                   set_interface_config.stream_id_key_size,
                   set_interface_config.stream_id_key_offset, set_interface_config.stream_id_enable, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_tdm_stream_test_arguments[] = {
    {"multicast", SAL_FIELD_TYPE_UINT32, "Multicast ID for Multicast Context", "1025"},
    {"clean", SAL_FIELD_TYPE_BOOL, "Clean after test", "Yes"},
    {"stream", SAL_FIELD_TYPE_INT32, "Stream ID", "0"},
    {"count", SAL_FIELD_TYPE_INT32, "Number of streams to set", "1"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_stream_test_man = {
    "TDM Stream to Context Configuration Semantic Tests"
};

static sh_sand_invoke_t dnx_tdm_stream_tests[] = {
    {"stream_0", ""},
    {"stream_last", "stream=0x3FFF"},
    {"stream_1024_count_100", "stream=1024 count=100"},
    {NULL}
};

static shr_error_e
dnx_tdm_stream_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int to_clean = FALSE;
    int is_equal = TRUE;
    int count = 0;
    int i_stream, get_stream_context, set_stream_context;
    int mc_id = 0;
    int first_stream_id = 0;
    bcm_tdm_ingress_context_t ingress_context;
    rhhandle_t ctest_soc_set_h = NULL;
    bcm_gport_t gport;
    char *test_name = "dnx_tdm_stream_test_cmd";
    char *flexe_mode_str;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("clean", to_clean);
    flexe_mode_str = "DISABLED";
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"tdm_mode", "TDM_OPTIMIZED"},
            {"flexe_device_mode.BCM8848X", flexe_mode_str},
            {"flexe_device_mode.BCM8880X", flexe_mode_str},
            {"ucode_port*", NULL},
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    bcm_tdm_ingress_editing_context_t_init(&ingress_context);

    SH_SAND_GET_UINT32("multicast", mc_id);
    SH_SAND_GET_INT32("count", count);
    SH_SAND_GET_INT32("stream", first_stream_id);
    for (i_stream = 0; i_stream < count; i_stream++)
    {
        _BCM_MULTICAST_GROUP_SET(ingress_context.multicast_id, 0, mc_id + i_stream);
        ingress_context.is_mc = TRUE;
        /*
         * Set TDM editing context configuration
         */
        SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_create(unit, 0, &set_stream_context, &ingress_context));
        BCM_GPORT_TDM_STREAM_SET(gport, first_stream_id + i_stream);
        SHR_IF_ERR_EXIT(bcm_tdm_stream_ingress_context_set(unit, 0, gport, set_stream_context));
        SHR_IF_ERR_EXIT(bcm_tdm_stream_ingress_context_get(unit, 0, gport, &get_stream_context));
        if (set_stream_context != get_stream_context)
        {
            is_equal = FALSE;
            LOG_CLI(("For stream:%d Get stream context:%d is different from set one:%d\n", first_stream_id + i_stream,
                     get_stream_context, set_stream_context));

        }
    }

    if (to_clean == TRUE)
    {
        for (i_stream = 0; i_stream < count; i_stream++)
        {
            /*
             * Set TDM editing context configuration
             */
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_destroy(unit, 0, i_stream));
        }
    }
    if (is_equal == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
exit:
    if (to_clean == TRUE)
    {
        shr_error_e rv;
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_stream_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        SH_SAND_GET_UINT32("multicast", mc_id);
        SH_SAND_GET_INT32("count", count);
        SH_SAND_GET_INT32("stream", first_stream_id);
        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "multicast %d, count %d first_stream_id %d\r\n", test_name, mc_id, count, first_stream_id);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

typedef enum
{
    IF_TDM_ONLY,
    IF_TDM_HYBRID
} sh_sand_tdm_if_type_t;

/* *INDENT-OFF* */

/*
 * CLI parameters for "egress_edit_type" (one of the options of 'dnx_tdm_bypass_test_cmd()').
 * See bcm_tdm_egress_editing_type_e
 */
static sh_sand_enum_t sh_enum_egress_editing_type_types[] = {
    {"remove_header", bcmTdmEgressEditingNoHeader , "No Header is prepended for outgoing TDM packet. Header is removed."},
    {"leave_header", bcmTdmEgressEditingNoStamp, "No stamping performed by egress, TDM packet header left as it arrived from ingress."},
    {"stamp_cud", bcmTdmEgressEditingStampMulticastEncapId, "Stamp CUD (MC identifier) inside the outgoing TDM header(either OTMH or FTMH)."},
    {"stamp_port_attributes", bcmTdmEgressEditingStampMulticastUserDefined, "Stamp Port Attribute in Outgoing TDM Header(either OTMH or FTMH)."},
    {NULL}
};
static sh_sand_enum_t sh_enum_if_types[] = {
    {"if_tdm_only", IF_TDM_ONLY, "Interface will be either for 'TDM BYpass' or 'TDM packet' but not for non-TDM traffic."},
    {"if_tdm_hybrid", IF_TDM_HYBRID, "Interface will have TDM/NON-TDM traffic according to NIF Parser Configuration"},
    {NULL}
};

static sh_sand_enum_t sh_enum_header_types[] = {
    {"ETH",           BCM_SWITCH_PORT_HEADER_TYPE_ETH, ""},
    {"RAW",           BCM_SWITCH_PORT_HEADER_TYPE_RAW, ""},
    {"INJECTED_2_PP", BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP, ""},
    {"INJECTED_2_PP_JR1_MODE", BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE, ""},
    {"INJECTED_2",             BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2, ""},
    {"INJECTED_2_JR1_MODE",    BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE, ""},
    {"MPLS_RAW",               BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW, ""},
    {"TDM",                    BCM_SWITCH_PORT_HEADER_TYPE_TDM, ""},
    {"TDM_RAW",                BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW, ""},
    {"TDM_ETH",                BCM_SWITCH_PORT_HEADER_TYPE_TDM_ETH, ""},
    {NULL}
};


/**
 *  CLI parameters for 'dnx_tdm_fabric_test_cmd()'.
 */
static sh_sand_option_t dnx_tdm_fabric_test_arguments[] = {
    {"clean",         SAL_FIELD_TYPE_BOOL,  "Clean after test", "Yes"},
    {"base",          SAL_FIELD_TYPE_INT32, "Offset in mapping table for first stream", "1000", (void *) dnx_tdm_interface_base_enum_table, "MIN-MAX"},
    {"bits_in_key",   SAL_FIELD_TYPE_INT32, "Number of bits in key", "MIN", (void *) dnx_tdm_interface_bits_in_key_enum_table, "MIN-MAX"},
    {"offset",        SAL_FIELD_TYPE_INT32, "Key offset in bits", "MIN", (void *) dnx_tdm_interface_offset_enum_table, "MIN-MAX"},
    {"stream_enable", SAL_FIELD_TYPE_BOOL,  "Stream enabled", "No"},
    {"src_port",      SAL_FIELD_TYPE_PORT,  "Source Port for TDM Bypass Traffic", "eth1"},
    {"dst_port",      SAL_FIELD_TYPE_PORT,  "Destination Port for Unicast Context or one of multicast group", "eth13"},
    {"cpu_port",      SAL_FIELD_TYPE_PORT,  "CPU Port to be used in order to send traffic to TDM loopback port", "201"},
    {"fabric_ports",  SAL_FIELD_TYPE_UINT32,"Fabric links forced to be used by TDM packets going through fabric. '0' stands for 'default mask'.", "0-2"},
    {"if_type",       SAL_FIELD_TYPE_ENUM,  "TDM type for incoming and outgoing interface", "if_tdm_only", sh_enum_if_types},
    {"egress_edit_type",
                      SAL_FIELD_TYPE_ENUM,  "Type of egress editing for TDM", "remove_header", sh_enum_egress_editing_type_types},
    {"packet",        SAL_FIELD_TYPE_STR,   "Packet name from XML (TestCase.xml)", "tdm_01"},
    {"set",           SAL_FIELD_TYPE_STR,   "Signal set name from XML", "default"},
    {"context_type",  SAL_FIELD_TYPE_ENUM,  "Context Editing Type", "Prepend", sh_enum_editing_types},
    {"channel_id",    SAL_FIELD_TYPE_UINT32,"Channel id to apply in the ILKN case", "MIN", (void *) dnx_tdm_interface_channel_id_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_fabric_test_man = {
    "Test fabric links setup on Single Packet TDM Traffic"
};
/**
 * Regression tests to run for 'fabric' mode. See 'dnx_tdm_fabric_test_cmd()'
 */
static sh_sand_invoke_t dnx_tdm_fabric_tests[] = {
    {"eth_fabric_with_stream",  "src_port=eth1 stream_enable=yes dst_port=eth13"},
    {"eth_default_fabric_no_stream",
                                "fabric_ports=0"},
    {NULL}
};
/**
 *  CLI parameters for 'dnx_tdm_bypass_test_cmd()'.
 */
static sh_sand_option_t dnx_tdm_bypass_test_arguments[] = {
    {"min_cell_size",
                  SAL_FIELD_TYPE_UINT32,"Minimal number of bytes allowed per TDM packet in 'bypass'mode",
                  "MIN", (void *) dnx_tdm_cell_size_enum_table, "MIN-MAX"},
    {"max_cell_size",
                  SAL_FIELD_TYPE_UINT32,"Maximal number of bytes allowed per TDM packet in 'bypass'mode",
                  "MAX", (void *) dnx_tdm_cell_size_enum_table, "MIN-MAX"},
    {"multicast_id",
                  SAL_FIELD_TYPE_UINT32,"Multicast ID for Multicast Context", "1025"},
    {"multicast_cud",
                  SAL_FIELD_TYPE_UINT32,"Multicast CUD which is stamped into FTMH if 'egress_edit_type' is 'stamp_cud'", "2748"},
    {"clean",     SAL_FIELD_TYPE_BOOL,  "Clean after test", "Yes"},
    {"base",      SAL_FIELD_TYPE_INT32, "Offset in mapping table for first stream", "1000", (void *) dnx_tdm_interface_base_enum_table, "MIN-MAX"},
    {"bits_in_key",   SAL_FIELD_TYPE_INT32, "Number of bits in key", "MIN", (void *) dnx_tdm_interface_bits_in_key_enum_table, "MIN-MAX"},
    {"offset",        SAL_FIELD_TYPE_INT32, "Key offset in bits", "MIN", (void *) dnx_tdm_interface_offset_enum_table, "MIN-MAX"},
    {"stream_enable", SAL_FIELD_TYPE_BOOL,  "Stream enabled", "No"},
    {"vid_stream",    SAL_FIELD_TYPE_BOOL,  "Stream id is based on vid field", "No"},
    {"src_port",      SAL_FIELD_TYPE_PORT,  "Source Port for TDM Bypass Traffic", "eth1"},
    {"dst_port",      SAL_FIELD_TYPE_PORT,  "Destination Port for Unicast Context or one of multicast group", "eth13"},
    {"mcast_port_1",  SAL_FIELD_TYPE_PORT,  "Destination Port (1 of 2) to add to multicast group", "eth14"},
    {"mcast_port_2",  SAL_FIELD_TYPE_PORT,  "Destination Port (2 of 2) to add to multicast group", "eth15"},
    {"num_mcast_ports",
                      SAL_FIELD_TYPE_UINT32,"Number of ports to assign on multicast group (src_port/mcast_port_1/mcast_port_2)", "2", NULL, "1-3"},
    {"cpu_port",      SAL_FIELD_TYPE_PORT,  "CPU Port to be used in order to send traffic to TDM loopback port", "201"},
    {"if_type",       SAL_FIELD_TYPE_ENUM,  "TDM type for incoming and outgoing interface", "if_tdm_only", sh_enum_if_types},
    {"egress_edit_type",
                      SAL_FIELD_TYPE_ENUM,  "Type of egress editing for TDM", "remove_header", sh_enum_egress_editing_type_types},
    {"header_type",   SAL_FIELD_TYPE_ENUM,  "In Header Type for the src_port", "TDM", sh_enum_header_types},
    {"packet",        SAL_FIELD_TYPE_STR,   "Packet name from XML (TestCase.xml)", "tdm_01"},
    {"set",           SAL_FIELD_TYPE_STR,   "Signal set name from XML", "default"},
    {"context_type",  SAL_FIELD_TYPE_ENUM,  "Context Editing Type", "Prepend", sh_enum_editing_types},
    {"stamp_mcid_with_sid",
                      SAL_FIELD_TYPE_BOOL,  "Stamp Stream ID in place of Multicast ID for Multicast Context", "No"},
    {"stream_multicast_prefix",
                      SAL_FIELD_TYPE_UINT32, "5 bits mcid prefix when working in 'stamp_mcid_with_sid' mode", "0", NULL, "0-31"},
    {"channel_id",    SAL_FIELD_TYPE_UINT32,"Channel id to apply in the ILKN case", "MIN", (void *) dnx_tdm_interface_channel_id_enum_table, "MIN-MAX"},
    {"otmh_4byte_out_header", SAL_FIELD_TYPE_BOOL, "Outgoing TDM Header has OTMH 4-Byte format", "No"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_bypass_test_man = {
    "Test Ingress Bypass Single Packet TDM Traffic"
};
/**
 * Regression tests to run for 'bypass' mode. See 'dnx_tdm_bypass_test_cmd()'
 */
static sh_sand_invoke_t dnx_tdm_bypass_tests[] = {
    {"no_stream_mcast_with_strip",          "src_port=eth1"                                                                  },
    {"no_stream_with_strip",                "src_port=eth1 dst_port=eth13"                                                   },
    {"no_stream_no_strip",                  "src_port=eth1 dst_port=eth13 egress_edit_type=leave_header"                     },
    {"no_stream_bad_cellsize",              "src_port=eth1 dst_port=eth13 max_cell_size=190"                                 },
    {"stream_eth",                          "stream_enable=yes dst_port=eth13"                                               },
    {"stream_1024_keybits_6",               "stream_enable=yes base=1024 bits_in_key=6 dst_port=eth13"                       },
    {"stream_mcast_stamp_sid",              "stream_enable=yes base=512 bits_in_key=2 multicast_id=0 stamp_mcid_with_sid=yes"},
    {"stream_mcast_stamp_sid_with_prefix",  "stream_enable=yes base=512 bits_in_key=2 multicast_id=0 stamp_mcid_with_sid=yes stream_multicast_prefix=1"},
    {"stream_mcast_stamp_sid_with_vid",     "stream_enable=yes base=0 multicast_id=0 bits_in_key=12 offset=116 stamp_mcid_with_sid=yes vid_stream=yes"},
    {"src_ilkn_6",                          "src_port=il6 dst_port=eth13 base=100 channel_id=8"                              },
    {"src_n_dst_ilkn",                      "src_port=il6 dst_port=il7 base=200 channel_id=4"                                },
    {"src_ilkn_dst_mcast",                  "src_port=il6 base=400 channel_id=10"                                            },
    {"src_ilkn_mcast_cud",                  "src_port=il6 base=400 channel_id=10 egress_edit_type=stamp_cud"                 },
    {"mcast_with_otmh_4byte_out_header",    "egress_edit_type=stamp_cud otmh_4byte_out_header=yes"                           },
    {NULL}
};

/**
 *  CLI parameters for 'dnx_tdm_bypass_test_cmd()'.
 */
static sh_sand_option_t dnx_tdm_failover_drop_test_arguments[] = {
    {"clean",     SAL_FIELD_TYPE_BOOL,  "Clean after test", "Yes"},
    {"src_port_1",    SAL_FIELD_TYPE_PORT,  "Source Port 1 for TDM Bypass Traffic", "eth1"},
    {"src_port_2",    SAL_FIELD_TYPE_PORT,  "Source Port 2 for TDM Bypass Traffic", "eth13"},
    {"dst_port_1",    SAL_FIELD_TYPE_PORT,  "Destination Port for Unicast Context or one of multicast group", "eth14"},
    {"dst_port_2",    SAL_FIELD_TYPE_PORT,  "Destination Port for Unicast Context or one of multicast group", "eth15"},
    {"mcast_port_1",  SAL_FIELD_TYPE_PORT,  "Destination Port (1 of 2) to add to multicast group", "eth16"},
    {"mcast_port_2",  SAL_FIELD_TYPE_PORT,  "Destination Port (2 of 2) to add to multicast group", "eth17"},
    {"if_type",       SAL_FIELD_TYPE_ENUM,  "TDM type for incoming and outgoing interface", "if_tdm_only", sh_enum_if_types},
    {NULL}
};

static sh_sand_man_t dnx_tdm_failover_drop_test_man = {
    "Test TDM Ingress Context Failover Drop"
};
/**
 * Regression tests to run for 'failover_drop'. See 'dnx_tdm_failover_drop_test_cmd()'
 */
static sh_sand_invoke_t dnx_tdm_failover_drop_tests[] = {
    {"src_ilkn_mcast",            "src_port_1=il7"                 },
    {"src_eth_dst_ilkn",          "dst_port_1=il7"                 },
    {"src_ilkn",                  "src_port_1=il7 dst_port_1=eth14"},
    {NULL}
};
/**
 *  CLI parameters for 'dnx_tdm_packet_test_cmd()'.
 */
static sh_sand_option_t dnx_tdm_packet_test_arguments[] = {
    {"min_cell_size",
                  SAL_FIELD_TYPE_UINT32,"Minimal number of bytes allowed per TDM packet in 'bypass'mode",
                  "MIN", (void *) dnx_tdm_cell_size_enum_table, "MIN-MAX"},
    {"max_cell_size",
                  SAL_FIELD_TYPE_UINT32,"Maximal number of bytes allowed per TDM packet in 'bypass'mode",
                  "MAX", (void *) dnx_tdm_cell_size_enum_table, "MIN-MAX"},
    {"clean",         SAL_FIELD_TYPE_BOOL,  "Clean after test", "Yes"},
    {"dst_port",      SAL_FIELD_TYPE_PORT,  "Destination Port for Unicast Context", "eth14"},
    {"cpu_port",      SAL_FIELD_TYPE_PORT,  "CPU Port to be used in order to send traffic to TDM destination port", "201"},
    {"if_type",       SAL_FIELD_TYPE_ENUM,  "TDM type for outgoing interface", "if_tdm_only", sh_enum_if_types},
    {"packet",        SAL_FIELD_TYPE_STR,   "Packet name from XML (TestCase.xml)", "tdm_03"},
    {"channel_id",    SAL_FIELD_TYPE_UINT32,"Channel id to apply in the ILKN case", "MIN", (void *) dnx_tdm_interface_channel_id_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_packet_test_man = {
    "Test Ingress Bypass Single Packet TDM Traffic"
};
/**
 * regression tests to run for 'packet' mode ('dnx_tdm_packet_test_cmd()').
 */
static sh_sand_invoke_t dnx_tdm_packet_tests[] = {
    {"tdm_packet_tc_dp",              "dst_port=eth13" },
    {"tdm_packet_tc_dp_hybrid",       "dst_port=eth13 if_type=if_tdm_hybrid" },
    {"tdm_packet_below_min",          "dst_port=eth13 min_cell_size=206" },
    {NULL}
};

/**
 *  CLI parameters for 'dnx_tdm_hybrid_test_cmd()'.
 */
static sh_sand_option_t dnx_tdm_hybrid_test_arguments[] = {
    {"src_port",      SAL_FIELD_TYPE_PORT,  "Source Port for TDM Bypass Traffic", "eth13"},
    {"dst_port_tdm",  SAL_FIELD_TYPE_PORT,  "Destination Port for TDM traffic", "eth14"},
    {"dst_port_data", SAL_FIELD_TYPE_PORT,  "Destination Port for DATA traffic", "eth15"},
    {"clean",         SAL_FIELD_TYPE_BOOL,  "Clean after test", "Yes"},
    {"cpu_port",      SAL_FIELD_TYPE_PORT,  "CPU Port to be used in order to send traffic to TDM destination port", "201"},
    {NULL}
};

/**
 *  CLI parameters for 'dnx_tdm_external_ftmh_test_cmd()'.
 */
static sh_sand_option_t dnx_tdm_external_ftmh_test_arguments[] = {
    {"src_port",      SAL_FIELD_TYPE_PORT,  "Source Port for TDM Bypass Traffic", "eth13"},
    {"dst_port",      SAL_FIELD_TYPE_PORT,  "Destination Port for TDM traffic", "eth14"},
    {"clean",         SAL_FIELD_TYPE_BOOL,  "Clean after test", "Yes"},
    {"cpu_port",      SAL_FIELD_TYPE_PORT,  "CPU Port to be used in order to send traffic to TDM destination port", "201"},
    {NULL}
};

static sh_sand_man_t dnx_tdm_hybrid_test_man = {
    "Test TDM hybrid mode - single port that supports both TDM (set by PRD) and DATA traffic"
};

static sh_sand_man_t dnx_tdm_external_ftmh_test_man = {
    "Test TDM packets with external FTMH header - packet do not append FTMH header at ingress but, rather, get it from incoming packet"
};

/**
 * Regression tests to run for 'hybrid bypass' mode. See 'dnx_tdm_hybrid_test_cmd()'
 */
static sh_sand_invoke_t dnx_tdm_hybrid_tests[] = {
    {"", ""},
    {NULL}
};

/**
 * Regression tests to run for external ftmh option . See 'dnx_tdm_external_ftmh_test_cmd()'
 */
static sh_sand_invoke_t dnx_tdm_external_ftmh_tests[] = {
    {"", ""},
    {NULL}
};

/* *INDENT-ON* */

/**
 * \brief
 *   Given TDM stream id, indicate whether it has already been assigned
 *   to a context or not. Also, if it has been assigned, return the context id.
 *
 * \param [in] unit -
 *   int. HW identifier of unit
 * \param [in] stream_id -
 *   int. Identifier of stream.
 * \param [out] is_allocated_p -
 *   Pointer to int. This procedure loads pointed memory by boolean indication
 *   on whether 'stream_id' has been assigned to a context or not. (If not,
 *   then it may be written without overriding previous write)
 * \param [out] context_id_p -
 *   Pointer to uint32. If *is_allocated_p is loaded by a non-zero value then
 *   this procedure loads pointed memory by the context that has been assigned
 *   to this 'stream_id'
 * \return
 *   int
 *
 * \remark
 *   * Note that if the assigned context is found to be DEFAULT_CONTEXT_PER_STREAM
 *     then we have no way of knowing whether stream has actually been assigned
 *     and this procedure will indicate that it has not been assigned.
 * \see
 *   * None
 */
static shr_error_e
dnx_tdm_ingress_is_stream_allocated(
    int unit,
    int stream_id,
    int *is_allocated_p,
    uint32 *context_id_p)
{

    uint32 context_id;
    uint8 is_context_allocated;

    SHR_FUNC_INIT_VARS(unit);
    *is_allocated_p = FALSE;
    *context_id_p = DEFAULT_CONTEXT_PER_STREAM;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_CONTEXT_MAP,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_STREAM_ID, stream_id,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_CONTEXT_ID, INST_SINGLE,
                                     (uint32 *) (&context_id), GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Check whether context is already allocated.
     */
    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, context_id, &is_context_allocated));
    if (is_context_allocated == TRUE)
    {
        if (context_id != DEFAULT_CONTEXT_PER_STREAM)
        {
            *is_allocated_p = TRUE;
            *context_id_p = context_id;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 *
 * \param [in] unit -
 *   int. HW identifier of unit
 * \param [in] do_not_override_context -
 *   int. if 'do_not_override_context' is set then make sure specified stream(s) have not already
 *   allocated another context. In other words, make sure it/they is/are not already in use.
 * \param [in] src_port -
 *   bcm_port_t. source port to configure IRE for
 * \param [in] channel_id -
 *   uint32. This is relevant only for ILKN, for ETH should always use 0.
 *   offset to be taken from base address in case of set_interface_config_p.stream_id_enable == 0
 * \param [in] is_not_mc -
 *   flag to indicate if the destination is MC (0) or UC (1)
 * \param [in] dst_port -
 *   bcm_port_t.
 *   not relevant when type == bcmTdmIngressEditingNoChange.
 *   destinaiton port to send TDM traffic to.
 * \param [in] type -
 *   bcm_tdm_ingress_editing_type_t. ingress editing type - indication if to create a new FTMH header or use packet headers as is.
 * \param [in] stamp_mcid_with_sid -
 *   not relevant when type == bcmTdmIngressEditingNoChange.
 *   not relevant when is_not_mc == 1,
 *   indicates if MC_ID within FTMH header should be replaced by stream ID (factors of stream ID can be found in set_interface_config_p).
 * \param [in] mc_id -
 *   int.
 *   not relevant when is_not_mc == 1,
 *   not relevant when type == bcmTdmIngressEditingNoChange.
 *   MC_ID to send traffic to
 * \param [in] set_interface_config_p -
 *   bcm_tdm_interface_config_t.
 *   parameters for interface configuration see bcm_tdm_interface_config_t for details.
 * \param [in] fabric_ports_p -
 *   relevant only when fabric is available
 *   array of fabric links that are assigned to TDM context
 * \param [in] fabric_ports_count -
 *   nof entries in fabric_ports_p. If set to '-1' then both input 'fabric_ports_p'
 *   and 'fabric_ports_count' are ignored and, within bcm_tdm_ingress_context_create(),
 *   all available fabric links are set for TDM.
 * \param [out] ingress_context_id_p -
 *   pointer.
 *   system selected context.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_tdm_ire_configure(
    int unit,
    int do_not_override_context,
    bcm_port_t src_port,
    uint32 channel_id,
    int is_not_mc,
    bcm_port_t dst_port,
    bcm_tdm_ingress_editing_type_t type,
    int stamp_mcid_with_sid,
    int mc_id,
    bcm_tdm_interface_config_t * set_interface_config_p,
    bcm_port_t * fabric_ports_p,
    int fabric_ports_count,
    int *ingress_context_id_p)
{
    int i_stream, set_stream_context;
    int i_fabric_link;
    bcm_tdm_ingress_context_t ingress_context;
    bcm_gport_t gport;
    int stream_id;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure interface - how to generate streams?
     */
    BCM_GPORT_LOCAL_SET(gport, src_port);

    SHR_IF_ERR_EXIT(bcm_tdm_interface_config_set(unit, 0, gport, set_interface_config_p));
    /*
     * Create Ingress Context
     */
    bcm_tdm_ingress_editing_context_t_init(&ingress_context);
    if (is_not_mc != FALSE)
    {
        BCM_GPORT_MODPORT_SET(ingress_context.destination_port, unit, dst_port);
        ingress_context.is_mc = FALSE;
    }
    else
    {
        ingress_context.is_mc = TRUE;
        _BCM_MULTICAST_GROUP_SET(ingress_context.multicast_id, 0, mc_id);
    }

    ingress_context.type = type;
    ingress_context.stamp_mcid_with_sid = stamp_mcid_with_sid;
    if (fabric_ports_count >= 0)
    {
        if (fabric_ports_count >= COUNTOF(ingress_context.fabric_ports))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "Number of specified fabric links (%d) is larger than container size (%d). Quit.\r\n",
                         fabric_ports_count, COUNTOF(ingress_context.fabric_ports));
        }
        if (fabric_ports_count != 0)
        {
            for (i_fabric_link = 0; i_fabric_link < fabric_ports_count; i_fabric_link++)
            {
                ingress_context.fabric_ports[i_fabric_link] = fabric_ports_p[i_fabric_link];
                ingress_context.fabric_ports_count++;
            }
        }
        flags = BCM_TDM_INGRESS_CONTEXT_SPECIFIC_FABRIC_LINKS;
    }
    else
    {
        flags = 0;
    }
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_create(unit, flags, &set_stream_context, &ingress_context));
    *ingress_context_id_p = set_stream_context;
    /*
     * Fill all stream of the interface with the same context
     */
    /*
     * If 'do_not_override_context' is set then make sure specified stream(s) have not already
     * allocated another context. In other words, make sure it/they is/are not already in use.
     */
    if (set_interface_config_p->stream_id_enable == TRUE)
    {
        for (i_stream = 0; i_stream < SAL_BIT(set_interface_config_p->stream_id_key_size); i_stream++)
        {
            stream_id = set_interface_config_p->stream_id_base + i_stream;
            if (do_not_override_context != FALSE)
            {
                uint32 context_id;
                int is_allocated;

                SHR_IF_ERR_EXIT(dnx_tdm_ingress_is_stream_allocated(unit, stream_id, &is_allocated, &context_id));
                if (is_allocated == TRUE)
                {
                    if (set_stream_context != context_id)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "\r\n"
                                     "Stream ID (%d) has already allocated for Context ID %d. Newly allocated context is %d. Caller requested 'no override'. Quit.\r\n",
                                     stream_id, context_id, set_stream_context);
                    }
                }
            }
            BCM_GPORT_TDM_STREAM_SET(gport, stream_id);
            SHR_IF_ERR_EXIT(bcm_tdm_stream_ingress_context_set(unit, 0, gport, set_stream_context));
        }
    }
    else
    {
        stream_id = set_interface_config_p->stream_id_base + channel_id;
        if (do_not_override_context != FALSE)
        {
            uint32 context_id;
            int is_allocated;

            SHR_IF_ERR_EXIT(dnx_tdm_ingress_is_stream_allocated(unit, stream_id, &is_allocated, &context_id));
            if (is_allocated == TRUE)
            {
                if (set_stream_context != context_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "\r\n"
                                 "Stream ID (%d) has already allocated Context ID %d. Newly allocated context is %d. Caller requested 'no override'. Quit.\r\n",
                                 stream_id, context_id, set_stream_context);
                }
            }
        }
        BCM_GPORT_TDM_STREAM_SET(gport, stream_id);
        SHR_IF_ERR_EXIT(bcm_tdm_stream_ingress_context_set(unit, 0, gport, set_stream_context));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create multicast group(s) for fabric (if relevant) and for egress (in the
 *   future, also for ingress) as per TDM parameters (specifically, stamping
 *   of SID changes the multicast group id).
 *   Then, add members to multicast group(s).
 *   This procedure is only relevant for 'bypass' mode.
 *
 * \param [in] unit -
 *   int. HW identifier of unit
 * \param [in] stamp_mcid_with_sid -
 *   int. Flag. If set then Stream ID replaces multicast-id on optimized FTMH header
 *   which is created at ingress. (TDM always works with optimized FTMH)
 *   Note that if this flag is set then it is assumed that 'stream_id_enable' has also been
 *   set.
 * \param [in] stream_id_base -
 *   int. Value to add to 'offset' (which is either channel of stream_id) to create index for
 *   entering 'flow-to-context' table.
 * \param [in] stream_id_key_size -
 *   int. If 'stamp_mcid_with_sid' then this is the number of the bits on the 'key' which
 *   is extracted from input packet.
 * \param [in] stream_multicast_prefix -
 *   int. If 'stamp_mcid_with_sid' then this value represents a 5 bits prefix (msb) to the generated mcid (sid).
 * \param [in] multicast_cud -
 *   int. This is the single CUD to set on the created multicast group(s).
 * \param [in] multicast_id -
 *   int. This is the ID of the multicast group. Used in case 'stamp_mcid_with_sid' is NOT set.
 * \param [in] num_members_in_multicast_group -
 *   int. Number of the members on the newly created multicast group. This is the number
 *   of replications.
 * \param [in] mcast_port -
 *   Array of bcm_port_t. Ports to add to multicast group(s). Numver of elemnts on this
 *   array is 'num_members_in_multicast_group'.
 * \param [in] dst_port -
 *   bcm_port_t. This is a single port which is, also, added to the newly created multicast group.
 *   the number of members is, then, at least one (this port).
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None.
 * \see
 *   * None
 */
static shr_error_e
dnx_tdm_set_mcast_group(
    int unit,
    int stamp_mcid_with_sid,
    int stream_multicast_prefix,
    int stream_id_base,
    int stream_id_key_size,
    int multicast_cud,
    bcm_multicast_t multicast_id,
    int num_members_in_multicast_group,
    bcm_port_t mcast_port[MAX_NUM_MCAST_PORTS],
    bcm_port_t dst_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** set MC group prefix */
    SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, bcmTdmStreamMulticastPrefix, stream_multicast_prefix));

    /*
     * If 'stamp_mcid_with_sid' is set then relate to all corresponding
     * 'stream_id's as 'multicast_id's.
     * Note that if 'stamp_mcid_with_sid' is set then 'stream_id_enable' is
     * also set. This is verified by the caller.
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /*
         * If there is fabric and this is multicast then configure this multicast group
         * to ignore fabric and return to the same chip.
         */
        int nof_cores, core_index;
        bcm_module_t dest_id_array[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
        int dest_id_count;
        uint32 flags;

        nof_cores = dnx_data_device.general.nof_cores_get(unit);
        for (core_index = 0; core_index < nof_cores; core_index++)
        {
            dest_id_array[core_index] = core_index;
        }
        flags = 0;
        dest_id_count = nof_cores;
        if (stamp_mcid_with_sid == TRUE)
        {
            int i_stream;
            bcm_multicast_t mc_stream_id;
            for (i_stream = 0; i_stream < SAL_BIT(stream_id_key_size); i_stream++)
            {
                mc_stream_id = TDM_SID_TO_MCID(unit, stream_id_base + i_stream, stream_multicast_prefix);
                SHR_IF_ERR_EXIT(bcm_fabric_multicast_set(unit, mc_stream_id, flags, dest_id_count, dest_id_array));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_fabric_multicast_set(unit, multicast_id, flags, dest_id_count, dest_id_array));
        }
    }
    else
    {
        /*
         * If there is no fabric then ingress multicast group needs to be created, as well, to make
         * sure packets are distributed to the various nits (if any).
         */
        int ing_mc_group_id;
        int erp_port;
        int count_erp;
        int sysport;
        bcm_multicast_replication_t ing_reps;

        ing_mc_group_id = multicast_id;
        SHR_IF_ERR_EXIT(bcm_multicast_create
                        (unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &ing_mc_group_id));
        SHR_IF_ERR_EXIT(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 1, &erp_port, &count_erp));
        SHR_IF_ERR_EXIT(bcm_stk_gport_sysport_get(unit, erp_port, &sysport));
        ing_reps.port = sysport;
        ing_reps.encap1 = multicast_id;
        SHR_IF_ERR_EXIT(bcm_multicast_set(unit, ing_mc_group_id, BCM_MULTICAST_INGRESS_GROUP, 1, &ing_reps));
    }
    {
        uint32 flags;
        bcm_multicast_replication_t bcm_multicast_replication[MAX_NUM_MCAST_PORTS + 1];
        int nof_replications;
        bcm_gport_t gport;
        bcm_if_t encap1;
        bcm_cosq_gport_type_t gport_type;
        bcm_cosq_gport_info_t gport_info;
        int ii;

        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;
        if (stamp_mcid_with_sid == TRUE)
        {
            int i_stream;
            bcm_multicast_t mc_stream_id;
            for (i_stream = 0; i_stream < SAL_BIT(stream_id_key_size); i_stream++)
            {
                mc_stream_id = TDM_SID_TO_MCID(unit, stream_id_base + i_stream, stream_multicast_prefix);

                SHR_IF_ERR_EXIT(bcm_multicast_create(unit, flags, &mc_stream_id));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_multicast_create(unit, flags, &multicast_id));
        }
        nof_replications = MAX_NUM_MCAST_PORTS + 1;
        if (num_members_in_multicast_group > 0)
        {
            nof_replications = num_members_in_multicast_group;
        }
        flags = BCM_MULTICAST_EGRESS_GROUP;
        encap1 = multicast_cud;
        /*
         * At this point, 'dst_port' contains the default value (as specified on 'dnx_tdm_bypass_test_arguments[]')
         */
        bcm_multicast_replication_t_init(&bcm_multicast_replication[0]);
        BCM_GPORT_LOCAL_SET(gport, dst_port);
        gport_type = bcmCosqGportTypeLocalPort;
        gport_info.in_gport = gport;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, gport_type, &gport_info));
        bcm_multicast_replication[0].port = gport_info.out_gport;
        bcm_multicast_replication[0].encap1 = encap1;
        /*
         * Add 'mcast_port's to multicast group. Either default value (as specified on 'dnx_tdm_bypass_test_arguments[]')
         * or as specified on the command line.
         */
        for (ii = 0; ii < MAX_NUM_MCAST_PORTS; ii++)
        {
            bcm_multicast_replication_t_init(&bcm_multicast_replication[1 + ii]);
            BCM_GPORT_LOCAL_SET(gport, mcast_port[ii]);
            gport_type = bcmCosqGportTypeLocalPort;
            gport_info.in_gport = gport;
            SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, gport_type, &gport_info));
            bcm_multicast_replication[1 + ii].port = gport_info.out_gport;
            bcm_multicast_replication[1 + ii].encap1 = encap1;
        }
        if (stamp_mcid_with_sid == TRUE)
        {
            int i_stream;
            bcm_multicast_t mc_stream_id;
            for (i_stream = 0; i_stream < SAL_BIT(stream_id_key_size); i_stream++)
            {
                mc_stream_id = TDM_SID_TO_MCID(unit, stream_id_base + i_stream, stream_multicast_prefix);
                SHR_IF_ERR_EXIT(bcm_multicast_add
                                (unit, mc_stream_id, flags, nof_replications, &bcm_multicast_replication[0]));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_multicast_add
                            (unit, multicast_id, flags, nof_replications, &bcm_multicast_replication[0]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * This test verifies the usage of specific fabric links for TDM traffic.
 * It is only active for devices which have fabric (J2C,...)
 */
static shr_error_e
dnx_tdm_fabric_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_tdm_fabric_test_cmd";
    int to_clean = FALSE;
    rhhandle_t ctest_soc_set_h = NULL;
    rhhandle_t packet_h = NULL;
    bcm_port_t src_port, cpu_port;
    bcm_port_t dst_port;
    bcm_port_t mcast_port[MAX_NUM_MCAST_PORTS];
    bcm_pbmp_t logical_ports;
    bcm_pbmp_t fabric_logical_ports;
    int core_id, nif_interface_id;
    int core_id_dst;
    uint32 channel_id_src;
    uint32 tm_port;
    char ucode_port_name_src[RHNAME_MAX_SIZE];
    char ucode_port_name_all_src[RHNAME_MAX_SIZE];
    char ucode_port_value_src[RHNAME_MAX_SIZE];
    char ucode_port_name_dst[RHNAME_MAX_SIZE];
    char ucode_port_value_dst[RHNAME_MAX_SIZE];
    char ucode_port_name_all_dst[RHNAME_MAX_SIZE];
    char ucode_port_name_mcast[MAX_NUM_MCAST_PORTS][RHNAME_MAX_SIZE];
    char ucode_port_value_mcast[MAX_NUM_MCAST_PORTS][RHNAME_MAX_SIZE];
    char ucode_port_name_all_mcast[MAX_NUM_MCAST_PORTS][RHNAME_MAX_SIZE];
    char gen_string[RHNAME_MAX_SIZE];
    char *ucode_port_type;
    int if_type;
    int ingress_context_id;
    char *packet_n;
    int speed;
    int add_channel_src;
    char *flexe_mode_str;
    char *tdm_mode_str;
    char *fabric_connect_mode_str;
    ctest_soc_property_t ctest_soc_property[MAX_NUM_SOC_PROPERTIES];
    uint32 octets_rx_from_src_port, octets_tx_from_dst_port;
    uint32 err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port;
    int is_ilkn_src;
    int is_ilkn_dst;
    int channel_id_dst;

    int expect_fail_on_is_tdm;
    int egress_edit_type;
    int ftmh_header_size;
    uint32 use_optimized_ftmh;
    uint8 ftmh_image[DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE];
    int stamp_mcid_with_sid;
    int stream_id_enable;
    int stream_id_base, stream_id_key_size;
    uint64 tdm_packet_counter;
    uint32 tdm_dropped_packet_counter;
    uint32 err_tdm_packet_counter;
    uint64 fdt_egq_tdm_packet_counter;
    uint32 fabric_port_base, fabric_port_top, fabric_nof_links;
    uint32 test_fabric_port_base, test_fabric_port_top, test_fabric_nof_links;
    bcm_port_t fabric_port_index;
    bcm_port_t fabric_ports[BCM_TDM_MAX_FABRIC_PORTS];
    int fabric_ports_count;
    uint32 num_in_range_tdm_tx_packets;
    int select_fabric_ports_as_default;
    uint32 ftmh_opt_enabled;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just make sure 'ingress_context_id' is initialized to an illegal value.
     */
    ingress_context_id = -1;
    use_optimized_ftmh = 0;
    /*
     * Just make sure 'dst_port'/'src_port'/'cpu_port'/'mcast_port' are initialized to an illegal value.
     */
    /*
     * Filling an array with '-1' results in setting all elements to '-1'.
     */
    sal_memset(mcast_port, -1, sizeof(mcast_port));
    dst_port = -1;
    src_port = -1;
    cpu_port = -1;
    sal_memset(ftmh_image, 0, sizeof(ftmh_image));
    sal_memset(ucode_port_name_mcast[0], 0, sizeof(ucode_port_name_mcast[0]));
    flexe_mode_str = "DISABLED";
    tdm_mode_str = "TDM_OPTIMIZED";
    fabric_connect_mode_str = "FE";
    /*
     * Core_id of destination port.
     */
    core_id_dst = 0;
    /*
     * If this flag ('expect_fail_on_is_tdm') is TRUE then we expect this test to fail on 'is_tdm' check.
     */
    expect_fail_on_is_tdm = FALSE;
    /*
     * ID of the first fabric port within list of all port.
     */
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
    /*
     * Number if fabric links for this chip (equal to number of fabric ports on this chip).
     */
    fabric_nof_links = dnx_data_fabric.links.nof_links_get(unit);
    /*
     * ID of ('last fabric port' plus 1) within list of all port.
     * To be used in 'for'  loops.
     */
    fabric_port_top = fabric_port_base + fabric_nof_links;
    if (fabric_nof_links == 0)
    {
        /*
         * If chip has no fabric links then there is nothing to test. Just quit
         * indicating 'success'
         */
        SHR_EXIT();
    }
    SH_SAND_GET_ENUM("egress_edit_type", egress_edit_type);
    SH_SAND_GET_STR("packet", packet_n);
    stamp_mcid_with_sid = FALSE;
    SH_SAND_GET_BOOL("stream_enable", stream_id_enable);
    SH_SAND_GET_INT32("base", stream_id_base);
    SH_SAND_GET_INT32("bits_in_key", stream_id_key_size);
    SH_SAND_GET_UINT32_RANGE("fabric_ports", test_fabric_port_base, test_fabric_port_top);
    test_fabric_nof_links = test_fabric_port_top + 1 - test_fabric_port_base;
    if (test_fabric_nof_links > fabric_nof_links)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                     "Number of links specified (%d) is larger than number of fabric links on this chip (%d). Quit.\r\n",
                     test_fabric_nof_links, fabric_nof_links);
    }
    if ((test_fabric_port_base + fabric_port_base) > fabric_port_top)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                     "First link specified (%d), when added to base of fabric links on this chip (%d),\r\n"
                     "==> is larger than top of fabric links on this chip (%d). Quit.\r\n",
                     test_fabric_port_base, fabric_port_base, fabric_port_top);
    }
    if ((test_fabric_port_top + fabric_port_base) > fabric_port_top)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                     "Last link specified (%d), when added to base of fabric links on this chip (%d),\r\n"
                     "==> is larger than top of fabric links on this chip (%d). Quit.\r\n",
                     test_fabric_port_top, fabric_port_base, fabric_port_top);
    }
    /*
     * Port '0' is assumed here as indicator that default should be selected.
     * This means that 'dnx_tdm_ire_configure()' is informed that the number of selected
     * fabric links is 'fabric_nof_links' and, consequently,
     * bcm_tdm_ingress_context_create->dnx_tdm_link_mask_update is informed that 'fabric_ports_count'
     * is 'fabric_nof_links'.
     */
    BCM_PBMP_CLEAR(fabric_logical_ports);
    if ((test_fabric_port_base == 0) && (test_fabric_port_top == 0))
    {
        select_fabric_ports_as_default = TRUE;
    }
    else
    {
        for (fabric_port_index = test_fabric_port_base; fabric_port_index <= test_fabric_port_top; fabric_port_index++)
        {
            BCM_PBMP_PORT_ADD(fabric_logical_ports, fabric_port_index + fabric_port_base);
        }
        select_fabric_ports_as_default = FALSE;
    }
    {
        /*
         * 'destination' must be there, either on on the command line or by default
         */
        /*
         * Also, no 'MC-CUD stamping' is relevant so it should not be specified for egress editing.
         */
        if (egress_edit_type == bcmTdmEgressEditingStampMulticastEncapId)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "Since 'destination' is present on the command line then this is not a multicast setup\r\n"
                         "==> so 'egress_edit_type' may not be 'bcmTdmEgressEditingStampMulticastEncapId'.\r\n"
                         "==> egress_edit_type %d. Quit.\r\n", egress_edit_type);
        }
        if (stamp_mcid_with_sid == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "'stamp_mcid_with_sid' is set but this is not a multicast setup (destination is present on the line). Quit.\r\n");
        }
        if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "This unit has no fabric. This test is for units with fabric only. Quit.\r\n");
        }
    }
    /*
     * Obtain incoming port and its properties
     */
    SH_SAND_GET_ENUM("if_type", if_type);
    SH_SAND_GET_PORT("src_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &src_port));
    {
        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_gport_t gport;
            bcm_port_resource_t resource;

            SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
            LOG_CLI_EX("\r\n"
                       "Source port     : interface_info.interface_id %d. mapping_info.channel %d. \r\n"
                       "mapping_info.core %d. mapping_info.tm_port %d. \r\n\n",
                       interface_info.interface_id, mapping_info.channel, mapping_info.core, mapping_info.tm_port);
            nif_interface_id = interface_info.interface_id;
            tm_port = mapping_info.tm_port;
            core_id = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, src_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));
            speed = resource.speed;
        }
        SH_SAND_GET_UINT32("channel_id", channel_id_src);
        add_channel_src = FALSE;
        SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, speed, &is_ilkn_src, &ucode_port_type));
        if (is_ilkn_src)
        {
            add_channel_src = TRUE;
        }
        else
        {
            if (channel_id_src != 0)
            {
                /*
                 * For Ethernet ports, 'channel_id' must be zero. There is no other option.
                 */
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "Caller specified 'channel_id' to be %d.\r\n"
                             "==> For Ethernet ports, 'channel_id' must be zero. There is no other option.\r\n",
                             channel_id_src);
            }
        }
        sal_snprintf(ucode_port_name_src, RHNAME_MAX_SIZE, "ucode_port_%d", src_port);
        sal_snprintf(ucode_port_name_all_src, RHNAME_MAX_SIZE, "%s.*", ucode_port_name_src);
        if (add_channel_src)
        {
            sal_snprintf(ucode_port_value_src, RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm", ucode_port_type,
                         nif_interface_id, channel_id_src, core_id, tm_port, sh_sand_enum_value_text(sand_control,
                                                                                                     "if_type",
                                                                                                     if_type));
        }
        else
        {
            sal_snprintf(ucode_port_value_src, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm", ucode_port_type,
                         nif_interface_id, core_id, tm_port, sh_sand_enum_value_text(sand_control, "if_type", if_type));
        }
    }
    /*
     * Get 'dst_port' whether it is or is not present on the command line.
     * If it is not present, then get its defualt value.
     */
    SH_SAND_GET_PORT("dst_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port));
    {
        /*
         * Enter whether 'dst_port' is specified on the command line or not.
         * For this test, 'NOT multicast' is implied.
         */
        char *ucode_port_type_dst;
        int nif_interface_id_dst;
        uint32 tm_port_dst;
        int add_channel_dst;

        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_gport_t gport;
            bcm_port_resource_t resource;

            SHR_IF_ERR_EXIT(bcm_port_get(unit, dst_port, &flags, &interface_info, &mapping_info));
            LOG_CLI_EX("\r\n"
                       "Destination port: interface_info.interface_id %d. mapping_info.channel %d. \r\n"
                       "mapping_info.core %d. mapping_info.tm_port %d. \r\n\n",
                       interface_info.interface_id, mapping_info.channel, mapping_info.core, mapping_info.tm_port);
            nif_interface_id_dst = interface_info.interface_id;
            channel_id_dst = mapping_info.channel;
            tm_port_dst = mapping_info.tm_port;
            core_id_dst = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, dst_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));
            speed = resource.speed;
        }
        add_channel_dst = FALSE;
        SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, speed, &is_ilkn_dst, &ucode_port_type_dst));
        if (is_ilkn_dst)
        {
            add_channel_dst = TRUE;
        }
        else
        {
            /*
             * For Ethernet ports, 'channel_id' must be zero. There is no other option.
             */
            channel_id_dst = 0;
        }
        sal_snprintf(ucode_port_name_dst, RHNAME_MAX_SIZE, "ucode_port_%d", dst_port);
        sal_snprintf(ucode_port_name_all_dst, RHNAME_MAX_SIZE, "%s.*", ucode_port_name_dst);
        if (add_channel_dst)
        {
            sal_snprintf(ucode_port_value_dst, RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm", ucode_port_type_dst,
                         nif_interface_id_dst, channel_id_dst, core_id_dst, tm_port_dst,
                         sh_sand_enum_value_text(sand_control, "if_type", if_type));
        }
        else
        {
            sal_snprintf(ucode_port_value_dst, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm", ucode_port_type_dst,
                         nif_interface_id_dst, core_id_dst, tm_port_dst, sh_sand_enum_value_text(sand_control,
                                                                                                 "if_type", if_type));
        }
    }
    /*
     * Configure SOC Properties
     *          Set BYPASS Optimized Mode
     *          Assign interface mode
     *          Assign port property tdm
     */
    SHR_IF_ERR_EXIT(dnx_tdm_load_soc_properties
                    (unit, fabric_connect_mode_str, tdm_mode_str, flexe_mode_str, ucode_port_name_all_src,
                     ucode_port_name_src, ucode_port_value_src, ucode_port_name_dst, ucode_port_value_dst,
                     ucode_port_name_all_dst, COUNTOF(ucode_port_name_mcast),
                     ((ucode_port_name_mcast[0] == 0) ? NULL : ucode_port_name_mcast), ucode_port_value_mcast,
                     ucode_port_name_all_mcast, 0, COUNTOF(ctest_soc_property), &ctest_soc_property[0]));
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    {
        /*
         * Force fabric to be used. Force TDM traffic to go through fabric.
         * It is not clear, yet, whether this is required or whether it is already done at
         * init. See dnx_fabric_features_init->dnx_fabric_force_tdm_init
         */
        dnx_fabric_force_e force;
        bcm_module_t egress_modid;
        bcm_gport_t destination_gport;

        BCM_GPORT_MODPORT_SET(destination_gport, unit, dst_port);
        egress_modid = BCM_GPORT_MODPORT_MODID_GET(destination_gport);
        /*
         * Traffic must be disabled before forcing fabric usage.
         */
        SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, egress_modid, 0, FALSE));
        /*
         * Disable fabric control cells
         */
        SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricControlCellsEnable, 0));
        /*
         * Force fabric usage.
         */
        force = DNX_FABRIC_FORCE_FABRIC;
        SHR_IF_ERR_EXIT(dnx_fabric_force_set(unit, force));
        /*
         * Enable fabric control cells
         */
        SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricControlCellsEnable, 1));
        SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, egress_modid, 0, TRUE));
    }
    {
        /*
         * Set loopback on fabric links .
         * In theory, the same could be achievd by calling
         *   bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC);
         * for all fabric ports.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "port loopback fabric mode=MAC"));
    }
    /*
     * Configure fabric ports array to be used by dnx_tdm_ire_configure()
     */
    if (select_fabric_ports_as_default == FALSE)
    {
        sal_memset(fabric_ports, 0, sizeof(fabric_ports));
        fabric_ports_count = 0;
        LOG_CLI_EX("\r\n"
                   "FABRIC links, as specified by the user on CLI: \r\n"
                   "============================================== %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);

        BCM_PBMP_ITER(fabric_logical_ports, fabric_port_index)
        {
            if ((fabric_port_index >= (fabric_port_base + COUNTOF(fabric_ports))) ||
                (fabric_port_index >= (fabric_port_base + fabric_nof_links)) || (fabric_port_index < fabric_port_base))
            {
                /*
                 * Just making sure...
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "One of the links, specified by the user, (%d) on CLI is out of bounds (Either %d,%d or %d,%d). Quit.\r\n",
                             fabric_port_index, fabric_port_base, (fabric_port_base + COUNTOF(fabric_ports)),
                             fabric_port_base, (fabric_port_base + fabric_nof_links));
            }
            fabric_ports[fabric_port_index - fabric_port_base] = fabric_port_index;
            fabric_ports_count++;
            LOG_CLI_EX("  Fabric link %d: Port %d %s%s\r\n",
                       fabric_port_index - fabric_port_base, fabric_port_index, EMPTY, EMPTY);
        }
    }
    else
    {
        /*
         * Use 'fabric_ports_count' to indicate that 'select_fabric_ports_as_default' is
         * TRUE and, therefore, the setup of fabric links will be determined within
         * 'bcm_tdm_ingress_context_create' API.
         */
        fabric_ports_count = -1;
    }
    /*
     * Configure IRE Part
     */
    {
        bcm_tdm_interface_config_t set_interface_config;
        bcm_tdm_ingress_editing_type_t type;
        int do_not_override_context;
        int dst_present, multicast_id;

        set_interface_config.stream_id_base = stream_id_base;
        set_interface_config.stream_id_key_size = stream_id_key_size;
        SH_SAND_GET_INT32("offset", set_interface_config.stream_id_key_offset);
        set_interface_config.stream_id_enable = stream_id_enable;
        SH_SAND_GET_ENUM("context_type", type);
        do_not_override_context = TRUE;
        dst_present = TRUE;
        /*
         * 'multicast_id' will not be referenced by dnx_tdm_ire_configure() when 'dst_present'
         * is set so select any value.
         */
        multicast_id = 0;
        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, src_port, channel_id_src, dst_present, dst_port, type,
                         stamp_mcid_with_sid, multicast_id, &set_interface_config, fabric_ports, fabric_ports_count,
                         &ingress_context_id));
    }
    {
        /*
         * Calculate the size of FTMH header for the current setup.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_get(unit, &use_optimized_ftmh));
        if (use_optimized_ftmh)
        {
            ftmh_header_size = OPT_FTMH_HEADER_SIZE;
        }
        else
        {
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                ftmh_header_size = STD_FTMH_HEADER_SIZE;
            }
            else
            {
                ftmh_header_size = LEGACY_FTMH_HEADER_SIZE;
            }
        }
        LOG_CLI_EX("\r\n" "FTMH header size, for this test is %d %s%s%s\r\n\n", ftmh_header_size, EMPTY, EMPTY, EMPTY);
    }
    {
        /*
         * Set system parameters.
         * Use bcm_dnx_tdm_control_set()
         */
        bcm_tdm_control_t type;
        int arg;
        int min_packet_size, max_packet_size;
        int tdm_out_header_type, tdm_stream_multicast_prefix;
        uint32 octets_rx_from_cpu_port;

        /*
         * SET
         */
        type = bcmTdmOutHeaderType;
        arg = (int) BCM_TDM_CONTROL_OUT_HEADER_FTMH;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));
        /*
         * GET
         */
        type = bcmTdmOutHeaderType;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_out_header_type = arg;
        type = bcmTdmStreamMulticastPrefix;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_stream_multicast_prefix = arg;
        type = bcmTdmBypassMinPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        min_packet_size = arg;
        type = bcmTdmBypassMaxPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        max_packet_size = arg;
        /*
         * Print extracted values.
         */
        LOG_CLI_EX("\r\n"
                   "bcmTdmBypassMinPacketSize %d. bcmTdmBypassMaxPacketSize %d. \r\n"
                   "bcmTdmOutHeaderType %d. bcmTdmStreamMulticastPrefix %d. \r\n\n",
                   min_packet_size, max_packet_size, tdm_out_header_type, tdm_stream_multicast_prefix);
        /*
         * Get counters info on transmitted packet.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, NULL, NULL, &octets_rx_from_cpu_port, NULL));
        /*
         * Print packet size, as specified on XML.
         */
        LOG_CLI_EX("\r\n"
                   "octets_rx_from_cpu_port (packt size as originated on CPU) %d. %s%s%s \r\n\n",
                   octets_rx_from_cpu_port, EMPTY, EMPTY, EMPTY);
        /*
         * If number of bytes on received packet is larger than the system-wide limit for
         * bypass-TDM packets then it will be forwarded to the 'packet' path and, therefore,
         * this test is expected to fail since 'is_tdm' will not be set.
         * Packet size is calculated by adding 'FTMH header size' to original size.
         * Maximal size, allowed to be sent by CPU, including PTCH, is 252 for Q2A
         * and 507 for J2C.
         *
         * See, also, Register ECI_FAP_GLOBAL_GENERAL_CFG_3, field PACKET_CRC_EN
         */
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) > max_packet_size)
        {
            expect_fail_on_is_tdm = TRUE;
        }
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) < min_packet_size)
        {
            expect_fail_on_is_tdm = TRUE;
        }
    }
    {
        /*
         * For debug/visibility: display the general TDM setup tables
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=PEMLA_TDM"));
    }
    {
        /*
         * For debug/visibility: display the general TDM IRE (ingress)setup tables
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_STREAM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONTEXT_CONFIG"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONTEXT_MAP"));
    }
    SH_SAND_GET_PORT("cpu_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &cpu_port));
    /*
     * Force traffic from CPU to go to TDM source port
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port, src_port, 1));
    {
        /*
         * We have loopback on source port. However, we do not want to see signals on
         * egress of source port since this is not part of the test.
         * So, disable visibility for egress of src_port to allow for tdm packet to be recorded
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          src_port, FALSE), "SRC Port Force set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           src_port, FALSE), "SRC Port Enable set failed\n");
    }
    if (is_ilkn_dst)
    {
        /*
         * Remember: This is NOT multicast case.
         *
         * Set 'destination port' to pass TDM packets to 'cpu port'. This is done because
         * we need a loopback on this port (to get a 'link up' signal for ILKN).
         */
        bcm_tdm_interface_config_t set_interface_config;
        bcm_tdm_ingress_editing_type_t type;
        int cpu_present;
        int multicast_id;
        int do_not_override_context;
        int loc_stamp_mcid_with_sid;
        /*
         * Set 'base' to 'zero'. Whatever selected values will be OK.
         */
        set_interface_config.stream_id_base = 0;
        set_interface_config.stream_id_key_size = 1;
        set_interface_config.stream_id_key_offset = 0;
        set_interface_config.stream_id_enable = 0;
        type = bcmTdmIngressEditingPrepend;
        loc_stamp_mcid_with_sid = 0;
        /*
         * 'multicast_id' will not be used here so select whatever value.
         */
        multicast_id = 0;
        /*
         * The channel id of the destination port was assigned above: channel_id_dst
         */
        /*
         * We assume 'cpu_port' was set, for this test.
         */
        cpu_present = TRUE;
        /*
         * Make sure to not use already used streams.
         */
        do_not_override_context = TRUE;

        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, dst_port, channel_id_dst, cpu_present, cpu_port, type,
                         loc_stamp_mcid_with_sid, multicast_id, &set_interface_config, fabric_ports, fabric_ports_count,
                         &ingress_context_id));
        /*
         * We have loopback on destination port. This is set only to get 'link up' on ILKN destination
         * port.
         * However, we do not want to see signals on ingress of destination port since this is not
         * part of the test.
         * So, disable visibility for ingress of dst_port and disable visibility for egress of
         * cpu_port to allow for tdm packet to be recorded,
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                          dst_port, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                           dst_port, FALSE), "DST Port Enable set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          cpu_port, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           cpu_port, FALSE), "DST Port Enable set failed\n");
        /*
         * Close loopback on dst_port, so that 'link up' is set for it. TDM traffic injected into dst_port
         * is directed to CPU.
         */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, dst_port, BCM_PORT_LOOPBACK_MAC));
    }
    {
        /*
         * Close loopback on src_port, so that traffic injected by CPU to this port will enter as tdm
         */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, src_port, BCM_PORT_LOOPBACK_MAC));
    }
    {
        /*
         * Set egress editing to:
         * Prepended no header to outgoing TDM packet.
         */
        bcm_gport_t gport;
        bcm_tdm_egress_editing_t editing;
        uint32 flags;

        BCM_GPORT_LOCAL_SET(gport, dst_port);
        sal_memset(&editing, 0, sizeof(editing));
        flags = 0;
        editing.type = egress_edit_type;
        SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));
    }
    {
        /*
         * Make sure to take counters' values before starting the test.
         */
        octets_rx_from_src_port = 0;
        octets_tx_from_dst_port = 0;
        err_pkt_rx_from_src_port = 0;
        err_pkt_tx_from_dst_port = 0;

        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, TRUE, src_port, dst_port, &octets_rx_from_src_port,
                                                      &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                      &err_pkt_tx_from_dst_port));
    }
    {
        /*
         * Disable OLP packets so we can follow only TDM packets. This is not compulsory. Just nice-to-have level.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "mod olp_dsp_event_route 0 15 dsp_event_route_line=0"));
    }
    {
        /*
         * Debug info. See link status. For ILKN, if link is 'down', traffic will not be sent out
         * of that link (port).
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "nif st"));
    }
    {
        /*
         * Debug info. Regarding multicast. See port cnfiguration. See bcm_dnx_tdm_egress_editing_set().
         * Note TRAP_CONTEXT_PORT_PROFILE regarding CUD.
         */
        SHR_IF_ERR_EXIT(sh_process_command
                        (unit, "dbal table dump table=EGRESS_PP_PORT clmn=TRAP_CONTEXT_PORT_PROFILE,PP_PORT "));
    }
    {
        /*
         * Do not remove:
         * Clear TDM counters. They are 'read-clear' counters
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_PACKET_COUNTER"));
    }
    {
        /*
         * Do not remove:
         * Clear FABRIC counters. They are 'read-clear' counters
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "show counter NonZero port=fabric"));
    }
    {
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_DIRECT_LINKS"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=LINK_BITMAP_CONFIGURATION"));
    }
    {
        /*
         * Send Traffic to the src_port
         */
        LOG_CLI_EX("\r\n"
                   "Now, send traffic to specified SRC port. This will be looped back as TDM traffic. %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_load(unit, packet_n, packet_h, NULL), "");
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port, packet_h, SAND_PACKET_RESUME));
        sal_msleep(8000);
    }
    {
        bcm_port_t link_port;
        uint64 link_counter_value;
        uint32 num_nz_link_tx_counters;
        uint32 num_nz_link_rx_counters;
        uint32 num_link_tx_packets;
        uint32 num_link_rx_packets;
        uint32 ii;

        num_nz_link_tx_counters = 0;
        num_link_tx_packets = 0;
        /*
         * This is a counter of the number of cells detected within the range assigned to TDM
         * by the caller. See "fabric_ports" (fabric_logical_ports)
         */
        num_in_range_tdm_tx_packets = 0;
        LOG_CLI_EX("\r\n"
                   "TX: Counters for fabric data cells, per link id\r\n"
                   "=============================================== %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        for (fabric_port_index = fabric_port_base; fabric_port_index < fabric_port_top; fabric_port_index++)
        {
            link_port = fabric_port_index;
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, link_port, snmpBcmTxDataCells, &link_counter_value));
            if (COMPILER_64_IS_ZERO(link_counter_value))
            {
                continue;
            }
            LOG_CLI_EX("  Link id %d, counter value (hi,low) is (%d,%d) %s\r\n",
                       link_port, COMPILER_64_HI(link_counter_value), COMPILER_64_LO(link_counter_value), EMPTY);
            num_nz_link_tx_counters++;
            num_link_tx_packets += COMPILER_64_LO(link_counter_value);
            /*
             * If user has selected to use the default mask then it means all links
             * are assigned to TDM and, therefore, 'num_in_range_tdm_tx_packets' is the
             * sum of all counters.
             */
            if (select_fabric_ports_as_default)
            {
                num_in_range_tdm_tx_packets += COMPILER_64_LO(link_counter_value);
            }
            else
            {
                for (ii = 0; ii < fabric_ports_count; ii++)
                {
                    if (fabric_ports[ii] == fabric_port_index)
                    {
                        num_in_range_tdm_tx_packets += COMPILER_64_LO(link_counter_value);
                        break;
                    }
                }
            }
        }
        if (num_nz_link_tx_counters == 0)
        {
            LOG_CLI_EX("  No non-zero counter found on TX. Nothing to display. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            LOG_CLI_EX("=============================================== \r\n"
                       "Number of non-zero counters found on TX is %d.\r\n"
                       "Number of packets found on TX is %d. %s%s\r\n\n",
                       num_nz_link_tx_counters, num_link_tx_packets, EMPTY, EMPTY);
        }
        num_nz_link_rx_counters = 0;
        num_link_rx_packets = 0;
        LOG_CLI_EX("\r\n"
                   "RX: Counters for fabric data cells, per link id\r\n"
                   "=============================================== %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        for (fabric_port_index = fabric_port_base; fabric_port_index < fabric_port_top; fabric_port_index++)
        {
            link_port = fabric_port_index;
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, link_port, snmpBcmRxDataCells, &link_counter_value));
            if (COMPILER_64_IS_ZERO(link_counter_value))
            {
                continue;
            }
            LOG_CLI_EX("  Link id %d, counter value (hi,low) is (%d,%d) %s\r\n",
                       link_port, COMPILER_64_HI(link_counter_value), COMPILER_64_LO(link_counter_value), EMPTY);
            num_nz_link_rx_counters++;
            num_link_rx_packets += COMPILER_64_LO(link_counter_value);
        }
        if (num_nz_link_rx_counters == 0)
        {
            LOG_CLI_EX("  No non-zero counter found on RX. Nothing to display. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            LOG_CLI_EX("=============================================== \r\n"
                       "Number of non-zero counters found on RX is %d. \r\n"
                       "Number of packets found on RX is %d. %s%s\r\n\n",
                       num_nz_link_rx_counters, num_link_rx_packets, EMPTY, EMPTY);
        }
    }
    {
        /*
         * For debug. Display fabric links counters.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "show counter NonZero port=fabric"));
    }
    {
        /*
         * For debug. Display diagnostics counters.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "diag counter graphical coredisplay"));
    }
    {
        /*
         * For debug. Display 'is_tdm' signal. For success, they should all be '1'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=is_tdm"));
    }
    /*
     * Expected core is dependent on dst_port or MC ID
     * Do not go to exit on failing to detect expected signal, so as to allow for investigating
     * debug info.
     */
    {
        uint32 expected_value = 1;

        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ERPP", "ERparser", NULL, "is_TDM", &expected_value, 1, NULL, NULL, 0));
    }
    if (SHR_FUNC_ERR())
    {
        if (expect_fail_on_is_tdm == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n" "Signal 'is_TDM': sand_signal_verify() returned with error %s\r\n",
                         shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "Testing 'is_tdm' signal has failed, as expected. Test is, so far, successfull %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
        }
    }
    else
    {
        if (expect_fail_on_is_tdm == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Signal 'is_TDM': sand_signal_verify() succeeded while, actually, failure was expected.\r\n");
        }
    }
    {
        /*
         * Check TDM counters. They are assumed to have been cleared just before the
         * 'send' operation.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_PACKET_COUNTER,
                                         0, 4,
                                         GEN_DBAL_FIELD64, DBAL_FIELD_TDM_PACKET_COUNTER, INST_SINGLE,
                                         &tdm_packet_counter, GEN_DBAL_FIELD32, DBAL_FIELD_TDM_DROPPED_PACKET_COUNTER,
                                         INST_SINGLE, &tdm_dropped_packet_counter, GEN_DBAL_FIELD32,
                                         DBAL_FIELD_ERR_TDM_PACKET_COUNTER, INST_SINGLE, &err_tdm_packet_counter,
                                         GEN_DBAL_FIELD64, DBAL_FIELD_FDT_EGQ_TDM_PACKET_COUNTER, INST_SINGLE,
                                         &fdt_egq_tdm_packet_counter, GEN_DBAL_FIELD_LAST_MARK));
        if (tdm_dropped_packet_counter != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Some (%d) TDM packets were dropped due to blocked context. We expect no drops. Quit.\r\n",
                         tdm_dropped_packet_counter);
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "No TDM packets were dropped due to blocked contexts. This is as expected. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        if (err_tdm_packet_counter != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n" "Some (%d) TDM packets were dropped due to errors. We expect no drops. Quit.\r\n",
                         err_tdm_packet_counter);
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "No TDM packets were dropped due to errors. This is as expected. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        LOG_CLI_EX("\r\n"
                   "Number of TDM packets at entry of ingress is (hi,lo) (%d,%d).\r\n"
                   "==> Number of TDM packets sent to FDT/EGQ is (hi,lo) (%d,%d)\r\n\n",
                   COMPILER_64_HI(tdm_packet_counter), COMPILER_64_LO(tdm_packet_counter),
                   COMPILER_64_HI(fdt_egq_tdm_packet_counter), COMPILER_64_LO(fdt_egq_tdm_packet_counter));
        if (num_in_range_tdm_tx_packets < COMPILER_64_LO(tdm_packet_counter))
        {
            /*
             * The number of packets/cells in fabric links, which were assigned to TDM, must be
             * larger-than/equal-to the number of TDM packets originally ejected from source
             * port. If not, this is an error.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Number of TDM packets at entry of ingress is %d.\r\n"
                         "==> Number of packets detected on fabric on links assigned to TDM (and general traffic) is %d\r\n"
                         "==> The latter must be larger than or equal to the former and it is not. Quit with error.\r\n\n",
                         COMPILER_64_LO(tdm_packet_counter), num_in_range_tdm_tx_packets);
        }
        else
        {
            /*
             * The number of packets/cells in fabric links, which were assigned to TDM, is
             * larger-than/equal-to the number of TDM packets originally ejected from source
             * port. Test is successful.
             */
            LOG_CLI_EX("\r\n"
                       "Number of TDM packets at entry of ingress is %d.\r\n"
                       "==> Number of packets detected on fabric on links assigned to TDM (and general traffic) is %d\r\n"
                       "==> The latter is larger than or equal to the former. Test is successful. %s%s\r\n\n",
                       COMPILER_64_LO(tdm_packet_counter), num_in_range_tdm_tx_packets, EMPTY, EMPTY);
        }
    }
    {
        /*
         * Temporary. For debug. Display 'bytes_to_strip' signal. For 'optimized ftmh', this should be '4'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=bytes_to_strip"));
    }
    {
        uint32 expected_value;
        uint32 actual_value;
        char result_str[DSIG_MAX_SIZE_STR];

        expected_value = ftmh_header_size;
        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ETPP", "Trap", NULL, "Bytes_to_Strip", &expected_value, 1, NULL,
                             result_str, 0));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_FAIL)
        {
            actual_value = (uint32) sal_strtoul(result_str, NULL, 16);
            if (actual_value != expected_value)
            {
                /*
                 * For 'optimized FTMH',
                 *   At this point, we do not fail the test if the number of bytes to strip is not '4',
                 *   as it should be for 'optimized FTMH'
                 * For 'standard FTMH',
                 *   At this point, we do not fail the test if the number of bytes to strip is not '10',
                 *   as it should be for 'standard FTMH'
                 * For 'legacy FTMH',
                 *   At this point, we do not fail the test if the number of bytes to strip is not '9',
                 *   as it should be for 'standard FTMH'
                 */
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "Number of bytes to strip is %d. We expect this value to be %d. Test has failed here.\r\n\n",
                             actual_value, expected_value);
            }
        }
        else if (SHR_FUNC_ERR())
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n" "Signal 'Bytes_to_Strip': sand_signal_verify() returned with error %s\r\n",
                         shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
        }
    }
    {
        /*
         * Since the 'size', used by 'sand_signal_verify', is in 'longs', we make sure that both
         * 'expected_value' and 'result' are fully contained within an array of 'longs'.
         * Also, make sure to reserve space for ending NULL.
         */
        char expected_value[(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) +
                            sizeof(uint32) + 1];
        uint8 result[(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) +
                     sizeof(uint32) + 1];
        int size;
        /*
         * 'nof_bits_stamped_value' is the number of bits on the value to extract from 'FTMH header' signal.
         */
        uint32 nof_bits_stamped_value;
        /*
         * 'ms_stamped_offset' is the offset, starting from the MS bit of the 'FTMH header' signal,
         * of the value to extract.
         */
        uint32 ms_stamped_offset;
        uint32 nof_bits_ftmh_container, ls_offset_ftmh_header;

        sal_memset(expected_value, 0, sizeof(expected_value));
        sal_memset(result, 0, sizeof(result));
        size = (sizeof(result) - 1) / (SAL_UINT32_NOF_BITS / SAL_UINT8_NOF_BITS);
        SHR_IF_ERR_EXIT(packet_ftmh_opt(unit, core_id_dst, NULL, &ftmh_opt_enabled));
        if (ftmh_opt_enabled)
        {
            SHR_SET_CURRENT_ERR(sand_signal_verify
                                (unit, core_id_dst, "ETPP", NULL, "ETparser", "FTMH_OPT", (uint32 *) expected_value,
                                 size, NULL, (char *) result, 0));
        }
        else
        {
            SHR_SET_CURRENT_ERR(sand_signal_verify
                                (unit, core_id_dst, "ETPP", NULL, "ETparser", "FTMH_Base", (uint32 *) expected_value,
                                 size, NULL, (char *) result, 0));
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        LOG_CLI_EX("\r\n" "Result       : %s %s%s%s\r\n", result, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(utilex_hex_char_array_to_num
                        (unit, result, (sizeof(ftmh_image) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + 1,
                         ftmh_image));
        LOG_CLI_EX("\r\n" "FTMH image   : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", ftmh_image[0], ftmh_image[1], ftmh_image[2],
                   ftmh_image[3]);
        LOG_CLI_EX("==>          : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", ftmh_image[4], ftmh_image[5], ftmh_image[6],
                   ftmh_image[7]);
        LOG_CLI_EX("==>          : 0x%02X 0x%02X %s %s\r\n\n", ftmh_image[8], ftmh_image[9], EMPTY, EMPTY);
        /*
         * 'nof_bits_ftmh_container' is the maximal number of bits on FTMH. This is the size
         * of the container array. On 'optimized', only the MS bytes are used.
         */
        nof_bits_ftmh_container = DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE * SAL_UINT8_NOF_BITS;
        /*
         * 'ls_offset_ftmh_header' is the ordinal number of the bit, starting from the LS bit,
         * where the actual FTMH header is placed. For 'optimized FTMH', this value is '80 - 8*4 = 48'
         */
        ls_offset_ftmh_header = nof_bits_ftmh_container - (ftmh_header_size * SAL_UINT8_NOF_BITS);
        if (use_optimized_ftmh)
        {
            {
                /*
                 * This is not a multicast setup. This is just 'destination'
                 */
                {
                    uint32 pp_dsp, fap_id;

                    if (ftmh_opt_enabled)
                    {
                        signal_output_t *field_sig_7_0 = NULL;
                        signal_output_t *field_sig_18_8 = NULL;
                        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id_dst, 0,
                                                                "ETPP", NULL, "ETparser",
                                                                "Pkt_Header.FTMH_OPT.FTMH_OPT_PP_DSP_or_Multicast_ID_7_0",
                                                                &field_sig_7_0));
                        pp_dsp = field_sig_7_0->value[0];

                        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id_dst, 0,
                                                                "ETPP", NULL, "ETparser",
                                                                "Pkt_Header.FTMH_OPT.FTMH_OPT_Destination_FAP_ID_or_Multicast_ID_18_8",
                                                                &field_sig_18_8));
                        fap_id = field_sig_18_8->value[0];
                    }
                    else
                    {
                        nof_bits_stamped_value = dnx_data_headers.optimized_ftmh.fap_id_offset_get(unit);
                        ms_stamped_offset =
                            nof_bits_ftmh_container - (ls_offset_ftmh_header +
                                                       dnx_data_headers.
                                                       optimized_ftmh.pp_dsp_or_mc_id_offset_get(unit) +
                                                       nof_bits_stamped_value);
                        SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                        (unit, ftmh_image, ms_stamped_offset, nof_bits_stamped_value, &pp_dsp));
                        nof_bits_stamped_value =
                            dnx_data_headers.optimized_ftmh.user_defined_offset_get(unit) -
                            dnx_data_headers.optimized_ftmh.fap_id_offset_get(unit);
                        ms_stamped_offset =
                            nof_bits_ftmh_container - (ls_offset_ftmh_header +
                                                       dnx_data_headers.optimized_ftmh.fap_id_offset_get(unit) +
                                                       nof_bits_stamped_value);
                        SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                        (unit, ftmh_image, ms_stamped_offset, nof_bits_stamped_value, &fap_id));
                    }
                    LOG_CLI_EX("\r\n" "FAP ID,PP DSP\r\n" "Value: %d %d %s %s\r\n", fap_id, pp_dsp, EMPTY, EMPTY);
                    if (pp_dsp != dst_port)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n" "pp_dsp value (%d) is not as expected (%d). Quit.\r\n\n", pp_dsp, dst_port);
                    }
                    if (fap_id != unit)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n" "fap_id value (%d) is not as expected (%d). Quit.\r\n\n", fap_id, unit);
                    }
                }
            }
        }
    }
    /*
     * Display counters after transmit process.
     */
    LOG_CLI_EX("\r\n" "Source Port %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", src_port);
    SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
    LOG_CLI_EX("\r\n" "Destination Port %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", dst_port);
    SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
    {
        /*
         * Get counters info on received/transmitted packets.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, FALSE, src_port, dst_port, &octets_rx_from_src_port,
                                                      &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                      &err_pkt_tx_from_dst_port));
    }
    /*
     * At this point, the following holds:
     *   octets_rx_from_src_port is the number of bytes received from source port.
     *   octets_tx_from_dst_port is the number of bytes transmitted through destination port
     *     They should be equal.
     *   err_pkt_rx_from_src_port is the number of error packets detected on reception from source port.
     *   err_pkt_tx_from_dst_port is the number of error packets detected on transmission through destination port
     *     They should be zero.
     */
    if ((err_pkt_rx_from_src_port != 0) || (err_pkt_tx_from_dst_port != 0))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "\r\n" "Erros reported by HW: err_pkt_rx_from_src_port %d err_pkt_tx_from_dst_port %d\r\n\n",
                     err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port);
    }
    if ((octets_rx_from_src_port == 0) || (octets_tx_from_dst_port == 0))
    {
        /*
         * At this point, both number of received bytes and number of transmitted bytes need to be non-zero.
         */
        if ((expect_fail_on_is_tdm == TRUE) && (octets_tx_from_dst_port == 0))
        {
            /*
             * If checking 'is_tdm' is expected to fail then we also expect no packet to go to
             * destination port.
             */
            LOG_CLI_EX("\r\n"
                       "No packet was sent to 'destination port. This is as expected (negative test). So far, test is successfull. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "Either number of bytes received from source port (%d) or number of bytes transmitted through destination port (%d) is zero. Quit.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, octets_tx_from_dst_port);
        }
    }
    if (egress_edit_type == bcmTdmEgressEditingNoHeader)
    {
        /*
         * If header is stripped on egress then we expect received packet to be of the same size as the
         * one transmitted on egress.
         */
        if (octets_rx_from_src_port != octets_tx_from_dst_port)
        {
            /*
             * At this point, we do not fail the test if they are not equal. We only report.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "Number of bytes received from source port (%d) is not equal to number of bytes transmitted through destination port (%d).\r\n"
                         "Test has failed.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, octets_tx_from_dst_port);
        }
    }
    else
    {
        /*
         * If header is not stripped on egress then we expect received packet to be smaller than
         * one transmitted, on egress, by the size of the header.
         */
        if ((octets_rx_from_src_port + ftmh_header_size) != octets_tx_from_dst_port)
        {
            /*
             * At this point, we do not fail the test if they are not equal. We only report.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "==> Number of bytes received from source port (%d) plus header size (%d) is not equal to number\r\n"
                         "==> of bytes transmitted through destination port (%d).\r\n"
                         "==> Test has  failed.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, ftmh_header_size, octets_tx_from_dst_port);
        }
    }
exit:
    diag_sand_packet_free(unit, packet_h);
    SH_SAND_GET_BOOL("clean", to_clean);
    if (to_clean == TRUE)
    {
        /*
         * Clean calls does not modify test return status
         */
        shr_error_e rv;
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_bypass_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        bcm_tdm_interface_config_t set_interface_config;
        bcm_port_t dst_port;
        bcm_pbmp_t logical_ports;
        bcm_tdm_ingress_context_t ingress_context;

        SH_SAND_GET_ENUM("if_type", if_type);
        SH_SAND_GET_STR("packet", packet_n);
        SH_SAND_GET_BOOL("clean", to_clean);
        SH_SAND_GET_PORT("cpu_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, cpu_port);
        SH_SAND_GET_PORT("src_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, src_port);

        SH_SAND_GET_INT32("base", set_interface_config.stream_id_base);
        SH_SAND_GET_INT32("bits_in_key", set_interface_config.stream_id_key_size);
        SH_SAND_GET_INT32("offset", set_interface_config.stream_id_key_offset);
        SH_SAND_GET_BOOL("stream_enable", set_interface_config.stream_id_enable);

        SH_SAND_GET_PORT("dst_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, dst_port);
        SH_SAND_GET_ENUM("context_type", ingress_context.type);

        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "if_type %d, %s src_port %d\r\n", test_name, if_type, EMPTY, src_port);
        LOG_CLI_EX("==> cpu_port %d, Packet_name %s, clean %d %s\r\n", cpu_port, packet_n, to_clean, EMPTY);
        LOG_CLI_EX("==> base %d, bits_in_key %d, offset %d stream_enable %d\r\n",
                   set_interface_config.stream_id_base, set_interface_config.stream_id_key_size,
                   set_interface_config.stream_id_key_offset, set_interface_config.stream_id_enable);
        LOG_CLI_EX("==> %s dst_port %d, %s context_type %d\r\n", EMPTY, dst_port, EMPTY, ingress_context.type);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_bypass_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_tdm_bypass_test_cmd";
    int to_clean = FALSE;
    /*
     * is_otmh_4byte_outgoing_header indicates whether the Outgoing TDM header has "OTMH 4-Byte" format
     */
    int is_otmh_4byte_outgoing_header = FALSE;
    rhhandle_t ctest_soc_set_h = NULL;
    rhhandle_t packet_h = NULL;
    bcm_port_t src_port, cpu_port;
    bcm_port_t dst_port;
    bcm_port_t mcast_port[MAX_NUM_MCAST_PORTS];
    int dst_present;
    int mcast_present[MAX_NUM_MCAST_PORTS];
    bcm_pbmp_t logical_ports;
    int core_id, nif_interface_id;
    int core_id_dst;
    uint32 channel_id_src;
    uint32 tm_port;
    char ucode_port_name_src[RHNAME_MAX_SIZE];
    char ucode_port_name_all_src[RHNAME_MAX_SIZE];
    char ucode_port_value_src[RHNAME_MAX_SIZE];
    char ucode_port_name_dst[RHNAME_MAX_SIZE];
    char ucode_port_value_dst[RHNAME_MAX_SIZE];
    char ucode_port_name_all_dst[RHNAME_MAX_SIZE];
    char ucode_port_name_mcast[MAX_NUM_MCAST_PORTS][RHNAME_MAX_SIZE];
    char ucode_port_value_mcast[MAX_NUM_MCAST_PORTS][RHNAME_MAX_SIZE];
    char ucode_port_name_all_mcast[MAX_NUM_MCAST_PORTS][RHNAME_MAX_SIZE];
    char gen_string[RHNAME_MAX_SIZE];
    char *ucode_port_type;
    int if_type, header_type;
    int ingress_context_id;
    char *packet_n;
    int speed;
    int add_channel_src;
    char *tdm_mode_str;
    ctest_soc_property_t ctest_soc_property[MAX_NUM_SOC_PROPERTIES];
    uint32 octets_rx_from_src_port, octets_tx_from_dst_port;
    uint32 err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port;
    int is_ilkn_src;
    int is_ilkn_dst;
    int is_ilkn_mcast[MAX_NUM_MCAST_PORTS];
    int channel_id_dst;
    bcm_multicast_t multicast_id;
    int multicast_cud;
    int multicast_id_present, multicast_cud_present;
    uint32 vid = 0;
    int expect_fail_on_is_tdm;
    uint32 max_cell_size, min_cell_size;
    int egress_edit_type;
    int ftmh_header_size;
    uint32 use_optimized_ftmh;
    uint8 ftmh_image[DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE];
    int stamp_mcid_with_sid;
    uint32 stream_multicast_prefix;
    int stream_id_enable, vid_stream_enable;
    int stream_id_base, stream_id_key_size;
    int num_members_in_multicast_group;
    uint64 tdm_packet_counter;
    uint32 tdm_dropped_packet_counter;
    uint32 err_tdm_packet_counter;
    uint64 fdt_egq_tdm_packet_counter;
    static char nof_egress_mc_groups[RHNAME_MAX_SIZE];
    uint32 ftmh_opt_enabled;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just make sure 'ingress_context_id' is initialized to an illegal value.
     */
    ingress_context_id = -1;
    use_optimized_ftmh = 0;
    /*
     * Just make sure 'dst_port'/'src_port'/'cpu_port'/'mcast_port' are initialized to an illegal value.
     */
    /*
     * Filling an array with '-1' results in setting all elements to '-1'.
     */
    sal_memset(mcast_port, -1, sizeof(mcast_port));
    dst_port = -1;
    src_port = -1;
    cpu_port = -1;
    sal_memset(ftmh_image, 0, sizeof(ftmh_image));
    sal_memset(ucode_port_name_mcast[0], 0, sizeof(ucode_port_name_mcast[0]));
    tdm_mode_str = "TDM_OPTIMIZED";
    /*
     * Core_id of destination port.
     */
    core_id_dst = 0;
    /*
     * If this flag ('expect_fail_on_is_tdm') is TRUE then we expect this test to fail on 'is_tdm' check.
     */
    expect_fail_on_is_tdm = FALSE;
    num_members_in_multicast_group = 0;
    SH_SAND_GET_UINT32("num_mcast_ports", num_members_in_multicast_group);
    SH_SAND_GET_ENUM("egress_edit_type", egress_edit_type);
    SH_SAND_GET_STR("packet", packet_n);
    SH_SAND_GET_BOOL("stamp_mcid_with_sid", stamp_mcid_with_sid);
    SH_SAND_GET_UINT32("stream_multicast_prefix", stream_multicast_prefix);
    SH_SAND_GET_BOOL("stream_enable", stream_id_enable);
    SH_SAND_GET_INT32("base", stream_id_base);
    SH_SAND_GET_INT32("bits_in_key", stream_id_key_size);
    SH_SAND_GET_BOOL("otmh_4byte_out_header", is_otmh_4byte_outgoing_header);

    SH_SAND_GET_UINT32("multicast_id", multicast_id);
    SH_SAND_GET_UINT32("multicast_cud", multicast_cud);
    SH_SAND_IS_PRESENT("dst_port", dst_present);
    SH_SAND_IS_PRESENT("mcast_port_1", mcast_present[0]);
    SH_SAND_IS_PRESENT("mcast_port_2", mcast_present[1]);
    sal_memset(ctest_soc_property, 0, sizeof(ctest_soc_property));
    SHR_IF_ERR_EXIT(dnx_tdm_base_soc_properties(unit, ctest_soc_property));
    SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, "tdm_mode", tdm_mode_str));
    /*
     * Get 'mcast_port_1/_2' whether it is or is not present on the command line.
     * If it is not present, then get its defualt value.
     */
    SH_SAND_GET_PORT("mcast_port_1", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &mcast_port[0]));
    SH_SAND_GET_PORT("mcast_port_2", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &mcast_port[1]));
    if (dst_present != FALSE)
    {
        /*
         * If 'destination' is specified on the command line then this is NOT a multicast test and
         * "multicast_id", "mcast_port_1", "mcast_port_2" and "multicast_cud" must NOT be present.
         */
        SH_SAND_IS_PRESENT("multicast_id", multicast_id_present);
        SH_SAND_IS_PRESENT("multicast_cud", multicast_cud_present);
        if ((multicast_id_present == TRUE) || (multicast_cud_present == TRUE) || (mcast_present[0] == TRUE)
            || (mcast_present[1] == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "Since 'destination' is present on the command line then neither 'multicast_id' nor\r\n"
                         "==> 'multicast_cud' nor 'mcast_port_1/_2' may be present.\r\n"
                         "==> dst_present %d, multicast_id_present %d, multicast_cud_present %d, mcast_port_1 %d, mcast_port_2 %d. Quit.\r\n",
                         dst_present, multicast_id_present, multicast_cud_present, mcast_port[0], mcast_port[1]);
        }
        /*
         * Also, no 'MC-CUD stamping' is relevant so it should not be specified for egress editing.
         */
        if (egress_edit_type == bcmTdmEgressEditingStampMulticastEncapId)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "Since 'destination' is present on the command line then this is not a multucast setup\r\n"
                         "==> so 'egress_edit_type' may not be 'bcmTdmEgressEditingStampMulticastEncapId'.\r\n"
                         "==> egress_edit_type %d. Quit.\r\n", egress_edit_type);
        }
        if (stamp_mcid_with_sid == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "'stamp_mcid_with_sid' is set but this is not a multicast setup (destination is present on the line). Quit.\r\n");
        }
    }
    else
    {
        if (stamp_mcid_with_sid == TRUE)
        {
            if (stream_id_enable == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "'stamp_mcid_with_sid' is set but 'stream_id_enable' is NOT set.\r\n"
                             "==> This is an illegal combination since this stamping is only active\r\n"
                             "==> on multicast setup with 'stream_enabled'. Quit.\r\n");
            }
        }
        /*
         * Also, 'MC-CUD stamping' collides with 'stamp_mcid_with_sid' so only one of them can
         * be set.
         */
        if ((egress_edit_type == bcmTdmEgressEditingStampMulticastEncapId) && (stamp_mcid_with_sid == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "Both 'stamp_mcid_with_sid' is set and 'bcmTdmEgressEditingStampMulticastEncapId' is specified.\r\n"
                         "==> This is illegal since they both change the same field on FTMH-header. Quit.\r\n\n");
        }
    }
    SH_SAND_GET_UINT32("max_cell_size", max_cell_size);
    SH_SAND_GET_UINT32("min_cell_size", min_cell_size);
    if (min_cell_size > max_cell_size)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                     "'min_cell_size' (%d) is smaller tha 'max_cell_size' (%d).\r\n"
                     "==> Illegal combination. Quit.\r\n", min_cell_size, max_cell_size);
    }
    /*
     * Stream is based on VID
     * In this mode most of the arguments are set manually
     */
    SH_SAND_GET_BOOL("vid_stream", vid_stream_enable);
    if ((stamp_mcid_with_sid == FALSE) && (vid_stream_enable != FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "If VID stamping mode is enabled then 'stamp_mcid_with_sid' must be set as well.\r\n");
    }
    /*
     * Obtain incoming port and its properties
     */
    SH_SAND_GET_ENUM("if_type", if_type);
    SH_SAND_GET_ENUM("header_type", header_type);
    SH_SAND_GET_PORT("src_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &src_port));
    {
        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_gport_t gport;
            bcm_port_resource_t resource;

            SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
            LOG_CLI_EX("\r\n"
                       "Source port     : interface_info.interface_id %d. mapping_info.channel %d. \r\n"
                       "mapping_info.core %d. mapping_info.tm_port %d. \r\n\n",
                       interface_info.interface_id, mapping_info.channel, mapping_info.core, mapping_info.tm_port);
            nif_interface_id = interface_info.interface_id;
            tm_port = mapping_info.tm_port;
            core_id = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, src_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));
            speed = resource.speed;
        }
        SH_SAND_GET_UINT32("channel_id", channel_id_src);
        add_channel_src = FALSE;
        SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, speed, &is_ilkn_src, &ucode_port_type));
        if (is_ilkn_src)
        {
            add_channel_src = TRUE;
        }
        else
        {
            if (channel_id_src != 0)
            {
                /*
                 * For Ethernet ports, 'channel_id' must be zero. There is no other option.
                 */
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "Caller specified 'channel_id' to be %d.\r\n"
                             "==> For Ethernet ports, 'channel_id' must be zero. There is no other option.\r\n",
                             channel_id_src);
            }
        }
        sal_snprintf(ucode_port_name_src, RHNAME_MAX_SIZE, "ucode_port_%d", src_port);
        sal_snprintf(ucode_port_name_all_src, RHNAME_MAX_SIZE, "%s.*", ucode_port_name_src);
        if (add_channel_src)
        {
            sal_snprintf(ucode_port_value_src, RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm", ucode_port_type,
                         nif_interface_id, channel_id_src, core_id, tm_port, sh_sand_enum_value_text(sand_control,
                                                                                                     "if_type",
                                                                                                     if_type));
        }
        else
        {
            sal_snprintf(ucode_port_value_src, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm", ucode_port_type,
                         nif_interface_id, core_id, tm_port, sh_sand_enum_value_text(sand_control, "if_type", if_type));
        }
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, ucode_port_name_all_src, NULL));
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, ucode_port_name_src, ucode_port_value_src));
    }
    /*
     * Get 'dst_port' whether it is or is not present on the command line.
     * If it is not present, then get its defualt value.
     */
    SH_SAND_GET_PORT("dst_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port));
    {
        /*
         * Enter whether 'dst_port' is specified on the command line or not.
         * If it is not, then 'multicast' is implied.
         * In that case, we put 'dst_port' and 'mcast_port_1/_2' into the destination multicast group
         */
        char *ucode_port_type_dst;
        int nif_interface_id_dst;
        uint32 tm_port_dst;
        int add_channel_dst;

        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_gport_t gport;
            bcm_port_resource_t resource;

            SHR_IF_ERR_EXIT(bcm_port_get(unit, dst_port, &flags, &interface_info, &mapping_info));
            LOG_CLI_EX("\r\n"
                       "Destination port: interface_info.interface_id %d. mapping_info.channel %d. \r\n"
                       "mapping_info.core %d. mapping_info.tm_port %d. \r\n\n",
                       interface_info.interface_id, mapping_info.channel, mapping_info.core, mapping_info.tm_port);
            nif_interface_id_dst = interface_info.interface_id;
            channel_id_dst = mapping_info.channel;
            tm_port_dst = mapping_info.tm_port;
            core_id_dst = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, dst_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));
            speed = resource.speed;
        }
        add_channel_dst = FALSE;
        SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, speed, &is_ilkn_dst, &ucode_port_type_dst));
        if (is_ilkn_dst)
        {
            add_channel_dst = TRUE;
        }
        else
        {
            /*
             * For Ethernet ports, 'channel_id' must be zero. There is no other option.
             */
            channel_id_dst = 0;
        }
        sal_snprintf(ucode_port_name_dst, RHNAME_MAX_SIZE, "ucode_port_%d", dst_port);
        sal_snprintf(ucode_port_name_all_dst, RHNAME_MAX_SIZE, "%s.*", ucode_port_name_dst);
        if (add_channel_dst)
        {
            sal_snprintf(ucode_port_value_dst, RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm", ucode_port_type_dst,
                         nif_interface_id_dst, channel_id_dst, core_id_dst, tm_port_dst,
                         sh_sand_enum_value_text(sand_control, "if_type", if_type));
        }
        else
        {
            sal_snprintf(ucode_port_value_dst, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm", ucode_port_type_dst,
                         nif_interface_id_dst, core_id_dst, tm_port_dst, sh_sand_enum_value_text(sand_control,
                                                                                                 "if_type", if_type));
        }
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, ucode_port_name_all_dst, NULL));
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, ucode_port_name_dst, ucode_port_value_dst));
        /*
         * For 'multicast' case, set all destinations of multicast group to be 'tdm' (see above and below)
         */
        if (dst_present == FALSE)
        {
            char *ucode_port_type_mcast[MAX_NUM_MCAST_PORTS];
            int nif_interface_id_mcast[MAX_NUM_MCAST_PORTS];
            uint32 tm_port_mcast[MAX_NUM_MCAST_PORTS];
            int add_channel_mcast[MAX_NUM_MCAST_PORTS];
            int speed_mcast[MAX_NUM_MCAST_PORTS];
            int mcast_index;
            int channel_id_mcast[MAX_NUM_MCAST_PORTS];
            int core_id_mcast[MAX_NUM_MCAST_PORTS];
            /*
             * Core_id of multicast group port (in addition to destination port.
             */
            sal_memset(core_id_mcast, 0, sizeof(core_id_mcast));
            for (mcast_index = 0; mcast_index < MAX_NUM_MCAST_PORTS; mcast_index++)
            {
                {
                    uint32 flags;
                    bcm_port_interface_info_t interface_info;
                    bcm_port_mapping_info_t mapping_info;
                    bcm_gport_t gport;
                    bcm_port_resource_t resource;

                    SHR_IF_ERR_EXIT(bcm_port_get
                                    (unit, mcast_port[mcast_index], &flags, &interface_info, &mapping_info));
                    LOG_CLI_EX("\r\n" "Multicast port  : interface_info.interface_id %d. mapping_info.channel %d. \r\n"
                               "mapping_info.core %d. mapping_info.tm_port %d. \r\n\n", interface_info.interface_id,
                               mapping_info.channel, mapping_info.core, mapping_info.tm_port);
                    nif_interface_id_mcast[mcast_index] = interface_info.interface_id;
                    channel_id_mcast[mcast_index] = mapping_info.channel;
                    tm_port_mcast[mcast_index] = mapping_info.tm_port;
                    core_id_mcast[mcast_index] = mapping_info.core;
                    BCM_GPORT_LOCAL_SET(gport, mcast_port[mcast_index]);
                    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));
                    speed_mcast[mcast_index] = resource.speed;
                }
                add_channel_mcast[mcast_index] = FALSE;
                SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str
                                (unit, speed_mcast[mcast_index], &is_ilkn_mcast[mcast_index],
                                 &ucode_port_type_mcast[mcast_index]));
                if (is_ilkn_mcast[mcast_index])
                {
                    add_channel_mcast[mcast_index] = TRUE;
                }
                else
                {
                    /*
                     * For Ethernet ports, 'channel_id' must be zero. There is no other option.
                     */
                    channel_id_mcast[mcast_index] = 0;
                }
                sal_snprintf(ucode_port_name_mcast[mcast_index], RHNAME_MAX_SIZE, "ucode_port_%d",
                             mcast_port[mcast_index]);
                sal_snprintf(ucode_port_name_all_mcast[mcast_index], RHNAME_MAX_SIZE, "%s.*",
                             ucode_port_name_mcast[mcast_index]);
                if (add_channel_mcast[mcast_index])
                {
                    sal_snprintf(ucode_port_value_mcast[mcast_index], RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm",
                                 ucode_port_type_mcast[mcast_index], nif_interface_id_mcast[mcast_index],
                                 channel_id_mcast[mcast_index], core_id_mcast[mcast_index], tm_port_mcast[mcast_index],
                                 sh_sand_enum_value_text(sand_control, "if_type", if_type));
                }
                else
                {
                    sal_snprintf(ucode_port_value_mcast[mcast_index], RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm",
                                 ucode_port_type_mcast[mcast_index], nif_interface_id_mcast[mcast_index],
                                 core_id_mcast[mcast_index], tm_port_mcast[mcast_index],
                                 sh_sand_enum_value_text(sand_control, "if_type", if_type));
                }
                SHR_IF_ERR_EXIT(dnx_ctest_soc_add
                                (unit, ctest_soc_property, ucode_port_name_all_mcast[mcast_index], NULL));
                SHR_IF_ERR_EXIT(dnx_ctest_soc_add
                                (unit, ctest_soc_property, ucode_port_name_mcast[mcast_index],
                                 ucode_port_value_mcast[mcast_index]));
            }
        }
    }
    /*
     * Configure SOC Properties
     *          Set BYPASS Optimized Mode
     *          Assign interface mode
     *          Assign port property tdm
     */
    sal_snprintf(nof_egress_mc_groups, RHNAME_MAX_SIZE, "%d",
                 TDM_SID_TO_MCID(unit, dnx_data_tdm.params.nof_stream_ids_get(unit), stream_multicast_prefix));

    SHR_IF_ERR_EXIT(dnx_ctest_soc_add
                    (unit, ctest_soc_property, "multicast_egress_group_id_range_max", nof_egress_mc_groups));
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    if (dst_present == FALSE)
    {
        /*
         * If no destination is specified then assume destination is a multicast group.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_set_mcast_group
                        (unit, stamp_mcid_with_sid, stream_multicast_prefix, stream_id_base, stream_id_key_size,
                         multicast_cud, multicast_id, num_members_in_multicast_group, mcast_port, dst_port));
    }
    /*
     * Configure IRE Part
     */
    {
        bcm_tdm_interface_config_t set_interface_config;
        bcm_tdm_ingress_editing_type_t type;
        int do_not_override_context;
        bcm_port_t *fabric_ports_p;
        int fabric_ports_count;

        fabric_ports_p = NULL;
        /*
         * Indicate all available fabric links are to be assigned to TDM (as well as to
         * other sources).
         */
        fabric_ports_count = -1;
        set_interface_config.stream_id_base = stream_id_base;
        set_interface_config.stream_id_key_size = stream_id_key_size;
        SH_SAND_GET_INT32("offset", set_interface_config.stream_id_key_offset);
        set_interface_config.stream_id_enable = stream_id_enable;
        SH_SAND_GET_ENUM("context_type", type);
        do_not_override_context = TRUE;

        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, src_port, channel_id_src, dst_present, dst_port, type,
                         stamp_mcid_with_sid, multicast_id, &set_interface_config, fabric_ports_p, fabric_ports_count,
                         &ingress_context_id));
    }
    {
        /*
         * Calculate the size of FTMH header for the current setup.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_get(unit, &use_optimized_ftmh));
        if (use_optimized_ftmh)
        {
            ftmh_header_size = OPT_FTMH_HEADER_SIZE;
        }
        else
        {
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                ftmh_header_size = STD_FTMH_HEADER_SIZE;
            }
            else
            {
                ftmh_header_size = LEGACY_FTMH_HEADER_SIZE;
            }
        }
        LOG_CLI_EX("\r\n" "FTMH header size, for this test is %d %s%s%s\r\n\n", ftmh_header_size, EMPTY, EMPTY, EMPTY);
    }
    {
        /*
         * Set system parameters.
         * Use bcm_dnx_tdm_control_set()
         */
        bcm_tdm_control_t type;
        int arg;
        int min_packet_size, max_packet_size;
        int tdm_out_header_type, tdm_stream_multicast_prefix;
        uint32 octets_rx_from_cpu_port;

        /*
         * SET
         */
        type = bcmTdmBypassMinPacketSize;
        arg = (int) min_cell_size;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));
        type = bcmTdmBypassMaxPacketSize;
        arg = (int) max_cell_size;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));

        type = bcmTdmOutHeaderType;
        arg = (int) BCM_TDM_CONTROL_OUT_HEADER_FTMH;
        if (is_otmh_4byte_outgoing_header)
        {
            /*
             * Note: This configuration is valid only when tdm_mode = TDM_OPTIMIZED
             */
            arg = (int) BCM_TDM_CONTROL_OUT_HEADER_OTMH_4B_WITH_CUD;
        }
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));

        /*
         * GET
         */
        type = bcmTdmOutHeaderType;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_out_header_type = arg;
        type = bcmTdmStreamMulticastPrefix;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_stream_multicast_prefix = arg;
        type = bcmTdmBypassMinPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        min_packet_size = arg;
        type = bcmTdmBypassMaxPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        max_packet_size = arg;
        /*
         * Print extracted values.
         */
        LOG_CLI_EX("\r\n"
                   "bcmTdmBypassMinPacketSize %d. bcmTdmBypassMaxPacketSize %d. \r\n"
                   "bcmTdmOutHeaderType %d. bcmTdmStreamMulticastPrefix %d. \r\n\n",
                   min_packet_size, max_packet_size, tdm_out_header_type, tdm_stream_multicast_prefix);
        /*
         * Get counters info on transmitted packet.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, NULL, NULL, &octets_rx_from_cpu_port, NULL));
        /*
         * Print packet size, as specified on XML.
         */
        LOG_CLI_EX("\r\n"
                   "octets_rx_from_cpu_port (packt size as originated on CPU) %d. %s%s%s \r\n\n",
                   octets_rx_from_cpu_port, EMPTY, EMPTY, EMPTY);
        /*
         * If number of bytes on received packet is larger than the system-wide limit for
         * bypass-TDM packets then it will be forwarded to the 'packet' path and, therefore,
         * this test is expected to fail since 'is_tdm' will not be set.
         * Packet size is calculated by adding 'FTMH header size' to original size.
         * Maximal size, allowed to be sent by CPU, including PTCH, is 252 for Q2A
         * and 507 for J2C.
         *
         * See, also, Register ECI_FAP_GLOBAL_GENERAL_CFG_3, field PACKET_CRC_EN
         */
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) > max_packet_size)
        {
            expect_fail_on_is_tdm = TRUE;
        }
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) < min_packet_size)
        {
            expect_fail_on_is_tdm = TRUE;
        }
    }
    {
        /*
         * For debug/visibility: display the general TDM setup table and the relevant PEMLA Tables
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=PEMLA_TDM"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=PEMLA_OTMHFORMATS"));
    }
    {
        /*
         * For debug/visibility: display the general TDM IRE (ingress) setup tables
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_STREAM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONTEXT_CONFIG"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONTEXT_MAP"));
    }
    SH_SAND_GET_PORT("cpu_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &cpu_port));
    /*
     * Force traffic from CPU to go to TDM source port
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port, src_port, 1));
    {
        /*
         * We have loopback on source port. However, we do not want to see signals on
         * egress of source port since this is not part of the test.
         * So, disable visibility for egress of src_port to allow for tdm packet to be recorded
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          src_port, FALSE), "SRC Port Force set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           src_port, FALSE), "SRC Port Enable set failed\n");
    }
    if (dst_present != FALSE)
    {
        /*
         * Enter here only if this is the 'destination is NOT multicast case'.
         *
         * This clause is NOT carried out on the 'destination is multicast' case
         * because the frames ejected from the destination port are multicast and some of them
         * return to the ejecting port and create an endless loop.
         *
         * Set 'destination port' to pass TDM packets to 'cpu port'. This is done because
         * we need a loopback on this port (to get a 'link up' signal for ILKN).
         */
        bcm_tdm_interface_config_t set_interface_config;
        bcm_tdm_ingress_editing_type_t type;
        int cpu_present;
        int do_not_override_context;
        int loc_stamp_mcid_with_sid;
        bcm_port_t *fabric_ports_p;
        int fabric_ports_count;

        fabric_ports_p = NULL;
        fabric_ports_count = -1;
        /*
         * Set 'base' to 'zero'. Whatever selected values will be OK.
         */
        set_interface_config.stream_id_base = 0;
        set_interface_config.stream_id_key_size = 1;
        set_interface_config.stream_id_key_offset = 0;
        set_interface_config.stream_id_enable = 0;
        type = bcmTdmIngressEditingPrepend;
        loc_stamp_mcid_with_sid = 0;
        /*
         * 'multicast_id' will not be used here so select whatever value.
         */
        /*
         * The channel id of the destination port was assigned above: channel_id_dst
         */
        /*
         * We assume 'cpu_port' was set, for this test.
         */
        cpu_present = TRUE;
        /*
         * Make sure to not use already used streams.
         */
        do_not_override_context = TRUE;

        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, dst_port, channel_id_dst, cpu_present, cpu_port, type,
                         loc_stamp_mcid_with_sid, multicast_id, &set_interface_config, fabric_ports_p,
                         fabric_ports_count, &ingress_context_id));
        /*
         * We have loopback on destination port. This is set only to get 'link up' on ILKN destination
         * port.
         * However, we do not want to see signals on ingress of destination port since this is not
         * part of the test.
         * So, disable visibility for ingress of dst_port and disable visibility for egress of
         * cpu_port to allow for tdm packet to be recorded,
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                          dst_port, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                           dst_port, FALSE), "DST Port Enable set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          cpu_port, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           cpu_port, FALSE), "DST Port Enable set failed\n");
        /*
         * Close loopback on dst_port, so that 'link up' is set for it. TDM traffic injected into dst_port
         * is directed to CPU.
         */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, dst_port, BCM_PORT_LOOPBACK_MAC));
    }
    {
        /*
         * Close loopback on src_port, so that traffic injected by CPU to this port will enter as tdm
         */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, src_port, BCM_PORT_LOOPBACK_MAC));
    }
    {
        /*
         * Set egress editing to dst_port, mcast_port[0] and mcast_port[1]
         * We are setting all the potential destination ports.
         * This setting is applicable only to ports participating in the test.
         */
        bcm_gport_t gport;
        bcm_tdm_egress_editing_t editing;
        uint32 flags;

        sal_memset(&editing, 0, sizeof(editing));
        flags = 0;
        editing.type = egress_edit_type;

        BCM_GPORT_LOCAL_SET(gport, dst_port);
        SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));

        /*
         * The multicast ports are configured (with ucode_port SOC property) only when dst_present==FALSE
         * Therefore, we configure the multicast ports with egress editing only for "multicast flow" tests
         */
        if (dst_present == FALSE)
        {
            BCM_GPORT_LOCAL_SET(gport, mcast_port[0]);
            SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));

            BCM_GPORT_LOCAL_SET(gport, mcast_port[1]);
            SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));
        }
    }
    {
        /*
         * Make sure to take counters' values before starting the test.
         */
        octets_rx_from_src_port = 0;
        octets_tx_from_dst_port = 0;
        err_pkt_rx_from_src_port = 0;
        err_pkt_tx_from_dst_port = 0;

        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, TRUE, src_port, dst_port, &octets_rx_from_src_port,
                                                      &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                      &err_pkt_tx_from_dst_port));
    }
    {
        /*
         * Disable OLP packets so we can follow only TDM packets. This is not compulsory. Just nice-to-have level.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "mod olp_dsp_event_route 0 15 dsp_event_route_line=0"));
    }
    {
        /*
         * Debug info. See link status. For ILKN, if link is 'down', traffic will not be sent out
         * of that link (port).
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "nif st"));
    }
    {
        /*
         * Debug info. Regarding multicast. See port cnfiguration. See bcm_dnx_tdm_egress_editing_set().
         * Note TRAP_CONTEXT_PORT_PROFILE regarding CUD.
         */
        SHR_IF_ERR_EXIT(sh_process_command
                        (unit, "dbal table dump table=EGRESS_PP_PORT clmn=TRAP_CONTEXT_PORT_PROFILE,PP_PORT "));
    }
    {
        /*
         * Do not remove:
         * Clear TDM counters. They are 'read-clear' counters
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_PACKET_COUNTER"));
    }
    {
        /*
         * Send Traffic to the src_port
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_load(unit, packet_n, packet_h, NULL), "");
        /**
         * get vid value when working in vid_stream_enable mode, this will be later used
         * as the expected stream id
         */
        if (vid_stream_enable)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                            (unit, packet_n, "ETH2", "Outer_VLAN.VID", NULL, &vid));
        }

        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port, packet_h, SAND_PACKET_RESUME));
        sal_msleep(1000);
    }
    {
        /*
         * Temporary. For debug. Display diagnostics counters.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "diag counter graphical coredisplay"));
    }
    {
        /*
         * Temporary. For debug. Display 'is_tdm' signal. For success, they should all be '1'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=is_tdm"));
    }
    if (dst_present == FALSE)
    {
        /*
         * Temporary. For debug. Display 'multicast groups' table when multicast is tsted.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=MCDB"));
    }
    /*
     * Expected core is dependent on dst_port or MC ID
     * Do not go to exit on failing to detect expected signal, so as to allow for investigating
     * debug info.
     */
    {
        uint32 expected_value = 1;

        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ERPP", "ERParser", NULL, "is_TDM", &expected_value, 1, NULL, NULL, 0));
    }
    if (SHR_FUNC_ERR())
    {
        if (expect_fail_on_is_tdm == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n" "Signal 'is_TDM': sand_signal_verify() returned with error %s\r\n",
                         shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "Testing 'is_tdm' signal has failed, as expected. Test is, so far, successfull %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
        }
    }
    else
    {
        if (expect_fail_on_is_tdm == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Signal 'is_TDM': sand_signal_verify() succeeded while, actually, failure was expected.\r\n");
        }
    }
    {
        /*
         * Check TDM counters. They are assumed to have been cleared just before the
         * 'send' operation.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_PACKET_COUNTER,
                                         0, 4,
                                         GEN_DBAL_FIELD64, DBAL_FIELD_TDM_PACKET_COUNTER, INST_SINGLE,
                                         &tdm_packet_counter, GEN_DBAL_FIELD32, DBAL_FIELD_TDM_DROPPED_PACKET_COUNTER,
                                         INST_SINGLE, &tdm_dropped_packet_counter, GEN_DBAL_FIELD32,
                                         DBAL_FIELD_ERR_TDM_PACKET_COUNTER, INST_SINGLE, &err_tdm_packet_counter,
                                         GEN_DBAL_FIELD64, DBAL_FIELD_FDT_EGQ_TDM_PACKET_COUNTER, INST_SINGLE,
                                         &fdt_egq_tdm_packet_counter, GEN_DBAL_FIELD_LAST_MARK));
        if (tdm_dropped_packet_counter != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Some (%d) TDM packets were dropped due to blocked context. We expect no drops. Quit.\r\n",
                         tdm_dropped_packet_counter);
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "No TDM packets were dropped due to blocked contexts. This is as expected. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        if (err_tdm_packet_counter != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n" "Some (%d) TDM packets were dropped due to errors. We expect no drops. Quit.\r\n",
                         err_tdm_packet_counter);
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "No TDM packets were dropped due to errors. This is as expected. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        LOG_CLI_EX("\r\n"
                   "Number of TDM packets at entry of ingress is (hi,lo) (%d,%d).\r\n"
                   "==> Number of TDM packets sent to FDT/EGQ is (hi,lo) (%d,%d)\r\n\n",
                   COMPILER_64_HI(tdm_packet_counter), COMPILER_64_LO(tdm_packet_counter),
                   COMPILER_64_HI(fdt_egq_tdm_packet_counter), COMPILER_64_LO(fdt_egq_tdm_packet_counter));
    }
    {
        /*
         * For debug.
         * Display 'ftmh_base' signal.
         * Display 'bytes_to_strip' signal. For 'optimized ftmh', this should be '4'
         * Display 'bytes_to_add' and 'header_to_add' signals. Their values correspond with the generated outgoing TDM Header
         */
        SHR_IF_ERR_EXIT(packet_ftmh_opt(unit, core_id_dst, NULL, &ftmh_opt_enabled));
        if (ftmh_opt_enabled)
        {
            SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get to=ETParser name=ftmh_opt"));
        }
        else
        {
            SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get to=ETParser name=ftmh_base"));
        }
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=bytes_to_strip"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get from=BTC name=bytes_to_add"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get from=BTC name=header_to_add"));
    }
    {
        uint32 expected_value;
        uint32 actual_value;
        char result_str[DSIG_MAX_SIZE_STR];

        expected_value = ftmh_header_size;
        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ETPP", "Trap", NULL, "Bytes_to_Strip", &expected_value, 1, NULL,
                             result_str, 0));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_FAIL)
        {
            actual_value = (uint32) sal_strtoul(result_str, NULL, 16);
            if (actual_value != expected_value)
            {
                /*
                 * For 'optimized FTMH',
                 *   At this point, we do not fail the test if the number of bytes to strip is not '4',
                 *   as it should be for 'optimized FTMH'
                 * For 'standard FTMH',
                 *   At this point, we do not fail the test if the number of bytes to strip is not '10',
                 *   as it should be for 'standard FTMH'
                 * For 'legacy FTMH',
                 *   At this point, we do not fail the test if the number of bytes to strip is not '9',
                 *   as it should be for 'standard FTMH'
                 */
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "Number of bytes to strip is %d. We expect this value to be %d. Test has failed here.\r\n\n",
                             actual_value, expected_value);
            }
        }
        else if (SHR_FUNC_ERR())
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n" "Signal 'Bytes_to_Strip': sand_signal_verify() returned with error %s\r\n",
                         shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
        }
    }
    {
        /*
         * Since the 'size', used by 'sand_signal_verify', is in 'longs', we make sure that both
         * 'expected_value' and 'result' are fully contained within an array of 'longs'.
         * Also, make sure to reserve space for ending NULL.
         */
        char expected_value[(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) +
                            sizeof(uint32) + 1];
        uint8 result[(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) +
                     sizeof(uint32) + 1];
        int size;
        uint32 ftmh_stamped_value;
        /*
         * 'nof_bits_ftmh_stamped_value' is the number of bits on the value to extract from 'FTMH header' signal.
         */
        uint32 nof_bits_ftmh_stamped_value;
        /*
         * 'ms_ftmh_stamped_offset' is the offset, starting from the MS bit of the 'FTMH header' signal,
         * of the value to extract.
         */
        uint32 ms_ftmh_stamped_offset;
        uint32 nof_bits_ftmh_container, ls_offset_ftmh_header;

        /*
         * Since the 'size', used by 'sand_signal_verify', is in 'longs', we make sure that both
         * 'header_to_add_expected_value' and 'header_to_add_result' are fully contained within an array of 'longs'.
         * Also, make sure to reserve space for ending NULL.
         */
        char header_to_add_expected_value[(HEADER_TO_ADD_MAX_SIZE * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) +
                                          sizeof(uint32) + 1];

        uint8 header_to_add_result[(HEADER_TO_ADD_MAX_SIZE * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) +
                                   sizeof(uint32) + 1];

        int header_to_add_size;
        uint8 header_to_add_image[HEADER_TO_ADD_MAX_SIZE];

        sal_memset(expected_value, 0, sizeof(expected_value));
        sal_memset(result, 0, sizeof(result));
        size = (sizeof(result) - 1) / (SAL_UINT32_NOF_BITS / SAL_UINT8_NOF_BITS);
        if (ftmh_opt_enabled)
        {
            SHR_SET_CURRENT_ERR(sand_signal_verify
                                (unit, core_id_dst, "ETPP", NULL, "ETparser", "FTMH_OPT", (uint32 *) expected_value,
                                 size, NULL, (char *) result, 0));
        }
        else
        {
            SHR_SET_CURRENT_ERR(sand_signal_verify
                                (unit, core_id_dst, "ETPP", NULL, "ETparser", "FTMH_Base", (uint32 *) expected_value,
                                 size, NULL, (char *) result, 0));
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        LOG_CLI_EX("\r\n" "FTMH Header  : %s %s%s%s\r\n", result, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(utilex_hex_char_array_to_num
                        (unit, result, (sizeof(ftmh_image) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + 1,
                         ftmh_image));
        LOG_CLI_EX("\r\n" "FTMH image   : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", ftmh_image[0], ftmh_image[1], ftmh_image[2],
                   ftmh_image[3]);
        LOG_CLI_EX("==>          : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", ftmh_image[4], ftmh_image[5], ftmh_image[6],
                   ftmh_image[7]);
        LOG_CLI_EX("==>          : 0x%02X 0x%02X %s %s\r\n\n", ftmh_image[8], ftmh_image[9], EMPTY, EMPTY);

        /*
         *   Read "Header_to_Add" signal
         */
        sal_memset(header_to_add_expected_value, 0, sizeof(header_to_add_expected_value));
        sal_memset(header_to_add_result, 0, sizeof(header_to_add_result));
        header_to_add_size = (sizeof(header_to_add_result) - 1) / (SAL_UINT32_NOF_BITS / SAL_UINT8_NOF_BITS);

        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ETPP", "BTC", "Aligner", "Header_to_Add",
                             (uint32 *) header_to_add_expected_value, header_to_add_size, NULL,
                             (char *) header_to_add_result, 0));
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        LOG_CLI_EX("\r\n" "Header To Add: %s %s%s%s\r\n", header_to_add_result, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(utilex_hex_char_array_to_num
                        (unit, header_to_add_result,
                         (sizeof(header_to_add_image) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + 1,
                         header_to_add_image));

        LOG_CLI_EX("\r\n" "Header To Add image (10 LS Bytes): 0x%02X 0x%02X 0x%02X 0x%02X\r\n",
                   header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 10], header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 9],
                   header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 8], header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 7]);
        LOG_CLI_EX("==>                              : 0x%02X 0x%02X 0x%02X 0x%02X\r\n",
                   header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 6], header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 5],
                   header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 4], header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 3]);
        LOG_CLI_EX("==>                              : 0x%02X 0x%02X %s %s\r\n\n",
                   header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 2], header_to_add_image[HEADER_TO_ADD_MAX_SIZE - 1],
                   EMPTY, EMPTY);

        /*
         * 'nof_bits_ftmh_container' is the maximal number of bits on FTMH. This is the size
         * of the container array. On 'optimized', only the MS bytes are used.
         */
        nof_bits_ftmh_container = DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE * SAL_UINT8_NOF_BITS;
        /*
         * 'ls_offset_ftmh_header' is the ordinal number of the bit, starting from the LS bit,
         * where the actual FTMH header is placed. For 'optimized FTMH', this value is '80 - 8*4 = 48'
         */
        ls_offset_ftmh_header = nof_bits_ftmh_container - (ftmh_header_size * SAL_UINT8_NOF_BITS);
        if (use_optimized_ftmh)
        {
            if (dst_present != FALSE)
            {
                /*
                 * Not a multicast setup. This is just 'destination'
                 */
                {
                    uint32 pp_dsp, fap_id;

                    if (ftmh_opt_enabled)
                    {
                        signal_output_t *field_sig_7_0 = NULL;
                        signal_output_t *field_sig_18_8 = NULL;
                        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id_dst, 0,
                                                                "ETPP", NULL, "ETparser",
                                                                "Pkt_Header.FTMH_OPT.FTMH_OPT_PP_DSP_or_Multicast_ID_7_0",
                                                                &field_sig_7_0));
                        pp_dsp = field_sig_7_0->value[0];

                        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id_dst, 0,
                                                                "ETPP", NULL, "ETparser",
                                                                "Pkt_Header.FTMH_OPT.FTMH_OPT_Destination_FAP_ID_or_Multicast_ID_18_8",
                                                                &field_sig_18_8));
                        fap_id = field_sig_18_8->value[0];
                    }
                    else
                    {
                        nof_bits_ftmh_stamped_value = dnx_data_headers.optimized_ftmh.fap_id_offset_get(unit);
                        ms_ftmh_stamped_offset =
                            nof_bits_ftmh_container - (ls_offset_ftmh_header +
                                                       dnx_data_headers.
                                                       optimized_ftmh.pp_dsp_or_mc_id_offset_get(unit) +
                                                       nof_bits_ftmh_stamped_value);
                        SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                        (unit, ftmh_image, ms_ftmh_stamped_offset, nof_bits_ftmh_stamped_value,
                                         &pp_dsp));
                        nof_bits_ftmh_stamped_value =
                            dnx_data_headers.optimized_ftmh.user_defined_offset_get(unit) -
                            dnx_data_headers.optimized_ftmh.fap_id_offset_get(unit);
                        ms_ftmh_stamped_offset =
                            nof_bits_ftmh_container - (ls_offset_ftmh_header +
                                                       dnx_data_headers.optimized_ftmh.fap_id_offset_get(unit) +
                                                       nof_bits_ftmh_stamped_value);
                        SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                        (unit, ftmh_image, ms_ftmh_stamped_offset, nof_bits_ftmh_stamped_value,
                                         &fap_id));
                    }
                    LOG_CLI_EX("\r\n" "FAP ID,PP DSP\r\n" "Value: %d %d %s %s\r\n", fap_id, pp_dsp, EMPTY, EMPTY);
                    if (pp_dsp != dst_port)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n" "pp_dsp value (%d) is not as expected (%d). Quit.\r\n\n", pp_dsp, dst_port);
                    }
                    if (fap_id != unit)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n" "fap_id value (%d) is not as expected (%d). Quit.\r\n\n", fap_id, unit);
                    }
                }
            }
            else
            {
                /*
                 * This is a multicast setup
                 */
                if (ftmh_opt_enabled)
                {
                    signal_output_t *field_sig_7_0 = NULL;
                    signal_output_t *field_sig_18_8 = NULL;
                    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id_dst, 0,
                                                            "ETPP", NULL, "ETparser",
                                                            "Pkt_Header.FTMH_OPT.FTMH_OPT_PP_DSP_or_Multicast_ID_7_0",
                                                            &field_sig_7_0));
                    ftmh_stamped_value = field_sig_7_0->value[0];

                    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id_dst, 0,
                                                            "ETPP", NULL, "ETparser",
                                                            "Pkt_Header.FTMH_OPT.FTMH_OPT_Destination_FAP_ID_or_Multicast_ID_18_8",
                                                            &field_sig_18_8));
                    ftmh_stamped_value |= field_sig_18_8->value[0] << field_sig_7_0->size;
                }
                else
                {
                    nof_bits_ftmh_stamped_value = dnx_data_headers.optimized_ftmh.user_defined_offset_get(unit);
                    ms_ftmh_stamped_offset =
                        nof_bits_ftmh_container - (ls_offset_ftmh_header +
                                                   dnx_data_headers.optimized_ftmh.pp_dsp_or_mc_id_offset_get(unit) +
                                                   nof_bits_ftmh_stamped_value);
                    SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                    (unit, ftmh_image, ms_ftmh_stamped_offset, nof_bits_ftmh_stamped_value,
                                     &ftmh_stamped_value));
                }
                if (egress_edit_type == bcmTdmEgressEditingStampMulticastEncapId)
                {
                    /*
                     * This is a multicast setup with stamping of MC-CUD
                     */
                    char expected_cud[(sizeof(uint32) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + sizeof(uint32) +
                                      1];
                    uint8 result_cud[(sizeof(uint32) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + sizeof(uint32) +
                                     1];
                    uint8 cud_image[sizeof(uint32)];
                    uint32 cud_value;
                    int cud_size;

                    uint32 stamped_cud_value;
                    /*
                     * 'nof_bits_stamped_cud_value' is the number of bits on the value to extract from 'Header_to_Add' signal.
                     */
                    uint32 nof_bits_stamped_cud_value;
                    /*
                     * 'ms_stamped_cud_offset' is the offset, starting from the MS bit of the 'Header_to_Add' signal of the value to extract.
                     */
                    uint32 ms_stamped_cud_offset;

                    sal_memset(expected_cud, 0, sizeof(expected_cud));
                    sal_memset(result_cud, 0, sizeof(result_cud));
                    cud_size = (sizeof(result_cud) - 1) / (SAL_UINT32_NOF_BITS / SAL_UINT8_NOF_BITS);
                    SHR_SET_CURRENT_ERR(sand_signal_verify
                                        (unit, core_id_dst, "ETPP", "ETparser", NULL,
                                         "CUD_Out_LIF_or_MCDB_Ptr", (uint32 *) expected_cud, cud_size, NULL,
                                         (char *) result_cud, 0));
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                    SHR_IF_ERR_EXIT(utilex_hex_char_array_to_num
                                    (unit, result_cud,
                                     (sizeof(cud_image) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + 1, cud_image));
                    SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                    (unit, cud_image, 0, SAL_UINT32_NOF_BITS, &cud_value));
                    if (cud_value != multicast_cud)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "CUD value, from ETPP->ETparser signal (0x%08X), is not as expected (0x%08X). Quit.\r\n\n",
                                     cud_value, multicast_cud);
                    }
                    else
                    {
                        LOG_CLI_EX("\r\n"
                                   "CUD value, from ETPP->ETparser signal, (0x%08X) is as expected (0x%08X).%s%s\r\n\n",
                                   cud_value, multicast_cud, EMPTY, EMPTY);
                    }

                    /*
                     ***********************************************
                     * Check the format of the Outgoing TDM header
                     ************************************************
                     */

                    nof_bits_stamped_cud_value = dnx_data_headers.optimized_ftmh.user_defined_offset_get(unit);
                    /*
                     * ms_stamped_cud_offset is pointing to the MSB of CUD in Header_to_Add
                     */
                    ms_stamped_cud_offset =
                        (HEADER_TO_ADD_MAX_SIZE * SAL_UINT8_NOF_BITS) -
                        (dnx_data_headers.optimized_ftmh.pp_dsp_or_mc_id_offset_get(unit) + nof_bits_stamped_cud_value);
                    if (is_otmh_4byte_outgoing_header)
                    {
                        nof_bits_stamped_cud_value = OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_CUD_BIT_SIZE;
                        /*
                         * ms_stamped_cud_offset is pointing to the MSB of CUD in Header_to_Add
                         */
                        ms_stamped_cud_offset =
                            ((HEADER_TO_ADD_MAX_SIZE - OTMH_4BYTE_OUTGOING_TDM_HEADER_SIZE) * SAL_UINT8_NOF_BITS);
                    }

                    SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                    (unit, header_to_add_image, ms_stamped_cud_offset, nof_bits_stamped_cud_value,
                                     &stamped_cud_value));

                    LOG_CLI_EX("\r\n"
                               "Checking the Outgoing TDM Header\r\n"
                               "================================ %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);

                    LOG_CLI_EX("\r\n"
                               "MC-CUD stamping\r\n"
                               "Stamped MC-CUD value: 0x%08X %s %s %s\r\n", stamped_cud_value, EMPTY, EMPTY, EMPTY);

                    if (stamped_cud_value != multicast_cud)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "Stamped MC-CUD value (%d) is not as expected (%d). Quit.\r\n",
                                     stamped_cud_value, multicast_cud);
                    }

                    if (is_otmh_4byte_outgoing_header)
                    {
                        /*
                         * Check also the MC_ID field in the outgoing TDM Header
                         */

                        uint32 stamped_mcid_value;
                        /*
                         * 'nof_bits_stamped_mcid_value' is the number of bits on the value to extract from 'Header_to_Add' signal.
                         */
                        uint32 nof_bits_stamped_mcid_value;
                        /*
                         * 'ms_stamped_mcid_offset' is the offset, starting from the MS bit of the 'Header_to_Add' signal of the value to extract.
                         */
                        uint32 ms_stamped_mcid_offset;

                        nof_bits_stamped_mcid_value = OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_ID_BIT_SIZE;
                        /*
                         * ms_stamped_mcid_offset is pointing to the MSB of MC-ID in Header_to_Add
                         */
                        ms_stamped_mcid_offset =
                            ((HEADER_TO_ADD_MAX_SIZE -
                              OTMH_4BYTE_OUTGOING_TDM_HEADER_MC_CUD_OFFSET) * SAL_UINT8_NOF_BITS);

                        SHR_IF_ERR_EXIT(utilex_bitstream_u8_ms_byte_first_get_field
                                        (unit, header_to_add_image, ms_stamped_mcid_offset, nof_bits_stamped_mcid_value,
                                         &stamped_mcid_value));

                        LOG_CLI_EX("\r\n"
                                   "MC-ID stamping\r\n"
                                   "Stamped MC-ID value: 0x%08X %s %s %s\r\n", stamped_mcid_value, EMPTY, EMPTY, EMPTY);

                        if (stamped_mcid_value != multicast_id)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "Stamped MC-ID value (%d) is not as expected (%d). Quit.\r\n",
                                         stamped_mcid_value, multicast_id);
                        }
                    }
                }
                else if (stamp_mcid_with_sid == TRUE)
                {
                    /*
                     * This is a multicast setup with stamping of STREAM-ID
                     */
                    LOG_CLI_EX("\r\n"
                               "STREAM-ID stamping\r\n"
                               "Stamped value: 0x%08X %s %s %s\r\n", ftmh_stamped_value, EMPTY, EMPTY, EMPTY);
                    /*
                     * Since we fill in all stream_id's corresponding to specified 'key size', we
                     * expect one of them to 'hit'. (In other words, whatever the contents of the packet, we
                     * expect a 'hit' here.).
                     * Note that, if prefix is specified (5 MS bits added to the 14 bits of 'stream id'),
                     * then it also goes into the stamped value calculation.
                     */
                    /** if 'vid_stream_enable' expect vid stamping */
                    if (vid_stream_enable == TRUE)
                    {
                        if (ftmh_stamped_value != stream_id_base + vid)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "Stream id stamped value (%d) is not as expected (%d) Quit.\r\n\n",
                                         ftmh_stamped_value, stream_id_base + vid);
                        }
                    }
                    else
                    {
                        if ((ftmh_stamped_value < TDM_SID_TO_MCID(unit, stream_id_base, stream_multicast_prefix))
                            || (ftmh_stamped_value >=
                                TDM_SID_TO_MCID(unit, (stream_id_base + SAL_BIT(stream_id_key_size)),
                                                stream_multicast_prefix)))
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "Stream id stamped value (%d) is not as expected (between %d and %d). Quit.\r\n\n",
                                         ftmh_stamped_value, stream_id_base,
                                         (stream_id_base + SAL_BIT(stream_id_key_size)));
                        }
                    }
                }
                else
                {
                    /*
                     * This is a multicast setup with no stamping
                     */
                    uint32 hdr_multicast_id;

                    hdr_multicast_id = ftmh_stamped_value;
                    LOG_CLI_EX("\r\n"
                               "MC-ID\r\n"
                               "Multicast id value: 0x%08X %s %s %s\r\n", hdr_multicast_id, EMPTY, EMPTY, EMPTY);
                    if (hdr_multicast_id != multicast_id)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "Mc_id value (%d) is not as expected (%d). Quit.\r\n\n",
                                     hdr_multicast_id, multicast_id);
                    }
                }
            }
        }
    }
    /*
     * Display counters after transmit process.
     */
    LOG_CLI_EX("\r\n" "Source Port %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", src_port);
    SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
    LOG_CLI_EX("\r\n" "Destination Port %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", dst_port);
    SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
    if (dst_present == FALSE)
    {
        int ii;
        for (ii = 0; ii < MAX_NUM_MCAST_PORTS; ii++)
        {
            LOG_CLI_EX("\r\n" "Multicast Port no. %d: Port %d %s%s\r\n", ii, mcast_port[ii], EMPTY, EMPTY);
            sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", mcast_port[ii]);
            SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
        }
    }
    {
        /*
         * Get counters info on received/transmitted packets.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, FALSE, src_port, dst_port, &octets_rx_from_src_port,
                                                      &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                      &err_pkt_tx_from_dst_port));
    }
    /*
     * At this point, the following holds:
     *   octets_rx_from_src_port is the number of bytes received from source port.
     *   octets_tx_from_dst_port is the number of bytes transmitted through destination port
     *     They should be equal.
     *   err_pkt_rx_from_src_port is the number of error packets detected on reception from source port.
     *   err_pkt_tx_from_dst_port is the number of error packets detected on transmission through destination port
     *     They should be zero.
     */
    if ((err_pkt_rx_from_src_port != 0) || (err_pkt_tx_from_dst_port != 0))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "\r\n" "Erros reported by HW: err_pkt_rx_from_src_port %d err_pkt_tx_from_dst_port %d\r\n\n",
                     err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port);
    }
    if ((octets_rx_from_src_port == 0) || (octets_tx_from_dst_port == 0))
    {
        /*
         * At this point, both number of received bytes and number of transmitted bytes need to be non-zero.
         */
        if ((expect_fail_on_is_tdm == TRUE) && (octets_tx_from_dst_port == 0))
        {
            /*
             * If checking 'is_tdm' is expected to fail then we also expect no packet to go to
             * destination port.
             */
            LOG_CLI_EX("\r\n"
                       "No packet was sent to 'destination port. This is as expected (negative test). So far, test is successfull. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "Either number of bytes received from source port (%d) or number of bytes transmitted through destination port (%d) is zero. Quit.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, octets_tx_from_dst_port);
        }
    }
    if (egress_edit_type == bcmTdmEgressEditingNoHeader)
    {
        /*
         * If header is stripped on egress then we expect received packet to be of the same size as the
         * one transmitted on egress.
         */
        if (octets_rx_from_src_port != octets_tx_from_dst_port)
        {
            /*
             * At this point, we fail the test if they are not equal.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "Number of bytes received from source port (%d) is not equal to number of bytes transmitted through destination port (%d).\r\n"
                         "Test has failed.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, octets_tx_from_dst_port);
        }
    }
    else
    {
        /*
         * If header is not stripped on egress then we expect received packet to be smaller than
         * one transmitted, on egress, by the size of the header.
         */
        if ((octets_rx_from_src_port + ftmh_header_size) != octets_tx_from_dst_port)
        {
            /*
             * At this point, we fail the test if they are not equal.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "==> Number of bytes received from source port (%d) plus header size (%d) is not equal to number\r\n"
                         "==> of bytes transmitted through destination port (%d).\r\n"
                         "==> Test has  failed.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, ftmh_header_size, octets_tx_from_dst_port);
        }
    }
exit:
    diag_sand_packet_free(unit, packet_h);
    SH_SAND_GET_BOOL("clean", to_clean);
    if (to_clean == TRUE)
    {
        /*
         * Clean calls does not modify test return status
         */
        shr_error_e rv;

        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_bypass_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        bcm_tdm_interface_config_t set_interface_config;
        int dst_present;
        bcm_port_t dst_port;
        bcm_pbmp_t logical_ports;
        bcm_tdm_ingress_context_t ingress_context;
        int multicast_id;
        uint32 max_cell_size, min_cell_size;

        SH_SAND_GET_ENUM("if_type", if_type);
        SH_SAND_GET_ENUM("header_type", header_type);
        SH_SAND_GET_STR("packet", packet_n);
        SH_SAND_GET_BOOL("clean", to_clean);
        SH_SAND_GET_PORT("cpu_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, cpu_port);
        SH_SAND_GET_PORT("src_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, src_port);

        SH_SAND_GET_INT32("base", set_interface_config.stream_id_base);
        SH_SAND_GET_INT32("bits_in_key", set_interface_config.stream_id_key_size);
        SH_SAND_GET_INT32("offset", set_interface_config.stream_id_key_offset);
        SH_SAND_GET_BOOL("stream_enable", set_interface_config.stream_id_enable);

        SH_SAND_IS_PRESENT("dst_port", dst_present);
        SH_SAND_GET_PORT("dst_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, dst_port);
        SH_SAND_GET_UINT32("multicast_id", multicast_id);
        SH_SAND_GET_ENUM("context_type", ingress_context.type);
        SH_SAND_GET_BOOL("stamp_mcid_with_sid", ingress_context.stamp_mcid_with_sid);
        SH_SAND_GET_UINT32("max_cell_size", max_cell_size);
        SH_SAND_GET_UINT32("min_cell_size", min_cell_size);

        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "if_type %d, header_type %d src_port %d\r\n", test_name, if_type, header_type, src_port);
        LOG_CLI_EX("==> cpu_port %d, Packet_name %s, clean %d %s\r\n", cpu_port, packet_n, to_clean, EMPTY);
        LOG_CLI_EX("==> base %d, bits_in_key %d, offset %d stream_enable %d\r\n",
                   set_interface_config.stream_id_base, set_interface_config.stream_id_key_size,
                   set_interface_config.stream_id_key_offset, set_interface_config.stream_id_enable);
        LOG_CLI_EX("==> dst_present %d, dst_port %d, multicast %d context_type %d\r\n",
                   dst_present, dst_port, multicast_id, ingress_context.type);
        LOG_CLI_EX("==> stamp %d min_cell_size %d max_cell_size %d %s\r\n", ingress_context.stamp_mcid_with_sid,
                   min_cell_size, max_cell_size, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set base voq which points to 'port' to SRAM only mode.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Destination logical port
 */
static shr_error_e
dnx_tdm_voq_sram_only_set(
    int unit,
    bcm_port_t port)
{
    bcm_cosq_delay_tolerance_t delay_tolerance;
    bcm_switch_profile_mapping_t profile_mapping;
    int base_voq, profile, weight;
    bcm_gport_t voq_gport, rate_class_gport;
    bcm_gport_t destination_gport;
    bcm_module_t egress_modid;

    SHR_FUNC_INIT_VARS(unit);
    BCM_GPORT_MODPORT_SET(destination_gport, unit, port);
    egress_modid = BCM_GPORT_MODPORT_MODID_GET(destination_gport);
    /** get VOQ ID - assuming DNX reference application */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, egress_modid, port, &base_voq));
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);
    /*
     * Set  delay tolerace profile to SRAM only
     */
    /*
     * get delay tolerace profile
     */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, voq_gport, 0, &profile, &weight));
    /*
     * Modify delay tolerance profile to SRAM only mode
     */
    SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_get(unit, profile, &delay_tolerance));
    delay_tolerance.flags |= BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;
    SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_set(unit, profile, &delay_tolerance));
    /**
     * Set rate class to SRAM only
     */
    /*
     * get rate class
     */
    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
    SHR_IF_ERR_EXIT(bcm_cosq_profile_mapping_get(unit, voq_gport, 0, 0, &profile_mapping));
    /*
     * set rate class to SRAM only
     */
    rate_class_gport = profile_mapping.mapped_profile;
    SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, rate_class_gport, 0, bcmCosqControlOCBOnly, 1));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_packet_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_tdm_packet_test_cmd";
    int to_clean = FALSE;
    rhhandle_t ctest_soc_set_h = NULL;
    rhhandle_t packet_h = NULL;
    bcm_port_t cpu_port;
    bcm_port_t dst_port;
    bcm_pbmp_t logical_ports;
    int core_id_dst;
    char ucode_port_name_dst[RHNAME_MAX_SIZE];
    char ucode_port_value_dst[RHNAME_MAX_SIZE];
    char ucode_port_name_all_dst[RHNAME_MAX_SIZE];
    char gen_string[RHNAME_MAX_SIZE];
    int if_type;
    char *packet_n;
    int speed;
    int ftmh_header_size;
    char *flexe_mode_str;
    ctest_soc_property_t ctest_soc_property[MAX_NUM_SOC_PROPERTIES];
    uint32 max_cell_size, min_cell_size;
    uint32 octets_tx_from_dst_port, octets_rx_from_cpu_port;
    uint32 err_pkt_tx_from_dst_port;
    int egress_edit_type;
    uint32 use_optimized_ftmh;
    int32 expected_tc;
    uint32 expected_dp;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just make sure 'dst_port'/'cpu_port' are initialized to an illegal values.
     */
    dst_port = -1;
    cpu_port = -1;
    flexe_mode_str = "DISABLED";
    /*
     * Core_id of destination port.
     */
    core_id_dst = 0;
    SH_SAND_GET_UINT32("max_cell_size", max_cell_size);
    SH_SAND_GET_UINT32("min_cell_size", min_cell_size);
    if (min_cell_size > max_cell_size)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                     "'min_cell_size' (%d) is smaller tha 'max_cell_size' (%d).\r\n"
                     "==> Illegal combination. Quit.\r\n", min_cell_size, max_cell_size);
    }
    SH_SAND_GET_ENUM("if_type", if_type);
    SH_SAND_GET_STR("packet", packet_n);
    SH_SAND_GET_PORT("dst_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port));
    {
        /*
         * Enter whether 'dst_port' is specified on the command line or not.
         * If it is not, then 'multicast' is implied. In that case, we put 'dst_port' into
         * the destination multicast group
         */
        char *ucode_port_type_dst;
        int nif_interface_id_dst, channel_id_dst;
        uint32 tm_port_dst;
        int add_channel_dst;
        int is_ilkn_dst;

        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, dst_port, &nif_interface_id_dst));
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, dst_port, &channel_id_dst));
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, dst_port, &core_id_dst, &tm_port_dst));
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, dst_port, 0, &speed));
        add_channel_dst = FALSE;
        SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, speed, &is_ilkn_dst, &ucode_port_type_dst));
        if (is_ilkn_dst)
        {
            add_channel_dst = TRUE;
        }
        sal_snprintf(ucode_port_name_dst, RHNAME_MAX_SIZE, "ucode_port_%d", dst_port);
        sal_snprintf(ucode_port_name_all_dst, RHNAME_MAX_SIZE, "%s.*", ucode_port_name_dst);
        if (add_channel_dst)
        {
            sal_snprintf(ucode_port_value_dst, RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm", ucode_port_type_dst,
                         nif_interface_id_dst, channel_id_dst, core_id_dst, tm_port_dst,
                         sh_sand_enum_value_text(sand_control, "if_type", if_type));
        }
        else
        {
            sal_snprintf(ucode_port_value_dst, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm", ucode_port_type_dst,
                         nif_interface_id_dst, core_id_dst, tm_port_dst, sh_sand_enum_value_text(sand_control,
                                                                                                 "if_type", if_type));
        }
    }
    /*
     * Configure SOC Properties
     *          Set BYPASS Optimized Mode
     *          Assign interface mode
     *          Assign port property tdm
     */
    SHR_IF_ERR_EXIT(dnx_tdm_load_soc_properties
                    (unit, NULL, "TDM_PACKET", flexe_mode_str, NULL, NULL,
                     NULL, ucode_port_name_dst, ucode_port_value_dst, ucode_port_name_all_dst, 0,
                     NULL, NULL, NULL, 0, COUNTOF(ctest_soc_property), &ctest_soc_property[0]));
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    {
        /*
         * Make sure packets destined to 'dst_port' are marked as 'OCB only' since
         * this is required for 'packet mode'
         */
        SHR_IF_ERR_EXIT(dnx_tdm_voq_sram_only_set(unit, dst_port));
    }
    {
        /*
         * Calculate the size of FTMH header for the current setup.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_get(unit, &use_optimized_ftmh));
        if (use_optimized_ftmh)
        {
            ftmh_header_size = OPT_FTMH_HEADER_SIZE;
        }
        else
        {
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                ftmh_header_size = STD_FTMH_HEADER_SIZE;
            }
            else
            {
                ftmh_header_size = LEGACY_FTMH_HEADER_SIZE;
            }
        }
        LOG_CLI_EX("\r\n" "FTMH header size, for this test is %d %s%s%s\r\n\n", ftmh_header_size, EMPTY, EMPTY, EMPTY);
    }
    {
        /*
         * Set/Get system parameters.
         * Use bcm_dnx_tdm_control_set()
         */
        bcm_tdm_control_t type;
        int arg;
        int min_packet_size, max_packet_size;
        int tdm_out_header_type, tdm_stream_multicast_prefix;
        uint32 octets_rx_from_cpu_port;
        /*
         * SET system parameters
         */
        type = bcmTdmBypassMinPacketSize;
        arg = (int) min_cell_size;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));
        type = bcmTdmBypassMaxPacketSize;
        arg = (int) max_cell_size;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));
        type = bcmTdmOutHeaderType;
        arg = (int) BCM_TDM_CONTROL_OUT_HEADER_FTMH;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));
        /*
         * GET system parameters
         */
        type = bcmTdmOutHeaderType;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_out_header_type = arg;
        type = bcmTdmStreamMulticastPrefix;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_stream_multicast_prefix = arg;
        type = bcmTdmBypassMinPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        min_packet_size = arg;
        type = bcmTdmBypassMaxPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        max_packet_size = arg;
        /*
         * Print extracted values.
         */
        LOG_CLI_EX("\r\n"
                   "bcmTdmBypassMinPacketSize %d. bcmTdmBypassMaxPacketSize %d. \r\n"
                   "bcmTdmOutHeaderType %d. bcmTdmStreamMulticastPrefix %d. \r\n\n",
                   min_packet_size, max_packet_size, tdm_out_header_type, tdm_stream_multicast_prefix);
        /*
         * Get counters info on transmitted packet.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, NULL, NULL, &octets_rx_from_cpu_port, NULL));
        /*
         * If number of bytes on received packet is larger than the system-wide limit for
         * bypass-TDM packets then it will be forwarded to the 'packet' path so,
         * this test is expected to still succeed.
         * Packet size is calculated by adding 'FTMH header size' to original size.
         * Maximal size, allowed to be sent by CPU, including PTCH, is 252 for Q2A
         * and 507 for J2C.
         *
         * See, also, Register ECI_FAP_GLOBAL_GENERAL_CFG_3, field PACKET_CRC_EN
         */
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) > max_packet_size)
        {
            LOG_CLI_EX("\r\n"
                       "Injected packet size (%d) plus headers (%d) is larger than maximal cell size (%d). We still expect test to succeed. %s\r\n\n",
                       octets_rx_from_cpu_port, ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC, max_packet_size,
                       EMPTY);
        }
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) < min_packet_size)
        {
            LOG_CLI_EX("\r\n"
                       "Injected packet size (%d) plus headers %d) is smaller than minimal cell size (%d). We still expect test to succeed. %s\r\n\n",
                       octets_rx_from_cpu_port, ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC, min_packet_size,
                       EMPTY);
        }
    }
    {
        /*
         * For debug/visibility: display the general TDM setup tables
         * See
         * dnx_tdm_init(),
         *   dnx_tdm_global_config()
         *   dnx_tdm_system_ftmh_type_set()
         *   dnx_tdm_min_pkt_set()
         *   dnx_tdm_max_pkt_set()
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=PEMLA_TDM"));
    }
    SH_SAND_GET_PORT("cpu_port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &cpu_port));
    /*
     * Force traffic from CPU to go to destination port
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port, dst_port, 1));
    {
        /*
         * Set default-context to not add any headers. We assume that the
         * packet we send, below, is assigned to that context.
         */
        bcm_field_context_param_info_t context_params;
        uint32 flags;
        bcm_field_context_t context_id;

        bcm_field_context_param_info_t_init(&context_params);
        context_params.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        context_params.param_val = bcmFieldSystemHeaderProfileFtmh;
        flags = 0;
        context_id = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
        SHR_IF_ERR_EXIT(bcm_field_context_param_set
                        (unit, flags, bcmFieldStageIngressPMF1, context_id, &context_params));
    }
    {
        /*
         * Set egress editing to:
         * Prepended no header to outgoing TDM packet.
         */
        bcm_gport_t gport;
        bcm_tdm_egress_editing_t editing;
        uint32 flags;

        BCM_GPORT_LOCAL_SET(gport, dst_port);
        sal_memset(&editing, 0, sizeof(editing));
        flags = 0;
        egress_edit_type = bcmTdmEgressEditingNoHeader;
        editing.type = egress_edit_type;
        SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));
    }
    {
        /*
         * Debug info. Regarding multicast. See port cnfiguration. See bcm_dnx_tdm_egress_editing_set().
         * Note TRAP_CONTEXT_PORT_PROFILE regarding CUD.
         */
        SHR_IF_ERR_EXIT(sh_process_command
                        (unit, "dbal table dump table=EGRESS_PP_PORT clmn=TRAP_CONTEXT_PORT_PROFILE,PP_PORT "));
    }
    {
        /*
         * Set the table, at end of ingress, which determines, based on various parameters,
         * such as TC and DP, whether to mark a packet, which goes to the Egress, as 'tdm'.
         */
        uint32 flags;
        bcm_cos_t ingress_pri;
        bcm_color_t color;
        int fabric_priority;
        bcm_module_t egress_modid;
        bcm_gport_t destination_gport;
        /*
         * Make sure 'fabric priority table has not 'is_tdm' entries. This is required
         * for 'fabric' devices since, by default, all 'ocb_only' entries are loaded for TDM.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_fabric_priority_map_no_tdm_init(unit, FALSE));
        fabric_priority = dnx_data_fabric.tdm.priority_get(unit);
        /*
         * Get TC and DP from packet to be injected.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, "ETH2", "Outer_VLAN.PCP", NULL, (uint32 *) &expected_tc));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, "ETH2", "Outer_VLAN.DEI", NULL, (uint32 *) &expected_dp));
        /*
         * This is TC.
         */
        ingress_pri = expected_tc;
        /*
         * This is DP.
         */
        color = expected_dp;
        flags = BCM_FABRIC_PRIORITY_TDM | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY | BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY;
        BCM_GPORT_MODPORT_SET(destination_gport, unit, dst_port);
        egress_modid = BCM_GPORT_MODPORT_MODID_GET(destination_gport);
        /*
         * Traffic must be disabled before setting 'fabric_priority'.
         */
        SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, egress_modid, 0, FALSE));
        SHR_IF_ERR_EXIT(bcm_fabric_priority_set(unit, flags, ingress_pri, color, fabric_priority));
        SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, egress_modid, 0, TRUE));
    }
    {
        /*
         * For debug/visibility: display the table that determines whether the following packets will
         * go through the TDM path on Egress.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=FABRIC_CELL_PRIORITY"));
        /*
         * For debug/visibility: display the table that determines what kind of FTMH headr we are
         * expecting here. Look at field SYSTEM_HEADERS_MODE:
         *   <EnumVal Name="JERICHO_MODE" Description="0x0 - Jericho/QMX/QAX mode" />
         *   <EnumVal Name="JERICHO2_MODE" Description="0x1 - Jericho 2 mode" />
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=SYSTEM_HEADER_GLOBAL_CFG"));
    }
    {
        /*
         * Make sure to take counters' values before starting the test.
         */
        octets_rx_from_cpu_port = 0;
        octets_tx_from_dst_port = 0;
        err_pkt_tx_from_dst_port = 0;
        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, TRUE, -1, dst_port, NULL,
                                                      &octets_tx_from_dst_port, NULL, &err_pkt_tx_from_dst_port));
    }
    {
        /*
         * Disable OLP packets so that only data packets hit the 'fabric_priority' table.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "mod olp_dsp_event_route 0 15 dsp_event_route_line=0"));
    }
    {
        /*
         * Send Traffic to the dst_port
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_load(unit, packet_n, packet_h, NULL), "");
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port, packet_h, SAND_PACKET_RESUME));
        sal_msleep(1000);
    }
    {
        /*
         * Temporary. For debug. Display 'is_tdm' signal. For success, they should all be '1'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=is_tdm"));
    }
    /*
     * Expected core is dependent on dst_port
     * Do not go to exit on failing to detect expected signal, so as to allow for investigating
     * debug info.
     */
    {
        uint32 expected_value = 1;

        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ERPP", "ERparser", NULL, "is_TDM", &expected_value, 1, NULL, NULL, 0));
    }
    if (SHR_FUNC_ERR())
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n" "sand_signal_verify() returned with error %s\r\n",
                     shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
    }
    {
        /*
         * Temporary. For debug. Display 'dp'/'tc' signal. For 'optimized ftmh', this should be '4'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=dp show=exact"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=tc show=exact"));
    }
    /*
     * Display counters after transmit process.
     */
    LOG_CLI_EX("\r\n" "Destination Port %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", dst_port);
    SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
    if (egress_edit_type == bcmTdmEgressEditingNoHeader)
    {
        /*
         * If header is stripped on egress then we expect received packet to be of the same size as the
         * one transmitted on egress.
         */
        /*
         * Get counters info on received/transmitted packets.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, NULL, NULL, &octets_rx_from_cpu_port, NULL));
        /*
         * Take out the PTCH byte count and add CRC byte count.
         */
        octets_rx_from_cpu_port -= BYTES_IN_PTCH;
        octets_rx_from_cpu_port += BYTES_EXTERNAL_CRC;
        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, FALSE, -1, dst_port, NULL,
                                                      &octets_tx_from_dst_port, NULL, &err_pkt_tx_from_dst_port));
    }
    else
    {
        /*
         * If header is not stripped on egress then we expect received packet to be smaller than
         * one transmitted, on egress, by the size of the header.
         */
        /*
         * Get counters info on received/transmitted packets.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, NULL, NULL, &octets_rx_from_cpu_port, NULL));
        /*
         * Take out the PTCH byte count and add CRC byte count.
         */
        octets_rx_from_cpu_port -= BYTES_IN_PTCH;
        octets_rx_from_cpu_port += (ftmh_header_size + BYTES_EXTERNAL_CRC);
        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, FALSE, -1, dst_port, NULL,
                                                      &octets_tx_from_dst_port, NULL, &err_pkt_tx_from_dst_port));
    }
    {
        /*
         * Temporary. For debug. Display 'bytes_to_strip' signal. For 'optimized ftmh', this should be '4'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=bytes_to_strip"));
    }
    /*
     * At this point, the following holds:
     *   octets_rx_from_cpu_port is the number of bytes received from cpu port.
     *   octets_tx_from_dst_port is the number of bytes transmitted through destination port
     *     They should be equal.
     *   err_pkt_tx_from_dst_port is the number of error packets detected on transmission through destination port
     *     It should be zero.
     */
    if (err_pkt_tx_from_dst_port != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "\r\n" "Erros reported by HW: err_pkt_tx_from_dst_port %d\r\n\n", err_pkt_tx_from_dst_port);
    }
    if ((octets_rx_from_cpu_port == 0) || (octets_tx_from_dst_port == 0))
    {
        /*
         * At this point, both number of received bytes and number of transmitted bytes need to be non-zero.
         */
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "\r\n"
                     "Source port is %d. Destination port is %d\r\n"
                     "Either number of bytes received from cpu port (%d) or number of bytes transmitted through destination port (%d) is zero. Quit.\r\n\n",
                     cpu_port, dst_port, octets_rx_from_cpu_port, octets_tx_from_dst_port);

    }
    if ((octets_rx_from_cpu_port) != octets_tx_from_dst_port)
    {
        /*
         * Fail the test if they are not equal.
         */
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "\r\n"
                     "Source port is %d. Destination port is %d\r\n"
                     "Number of bytes received from cpu port (%d) is not equal to number of bytes transmitted through destination port (%d).\r\n\n",
                     cpu_port, dst_port, octets_rx_from_cpu_port, octets_tx_from_dst_port);
    }
exit:
    diag_sand_packet_free(unit, packet_h);
    SH_SAND_GET_BOOL("clean", to_clean);
    if (to_clean == TRUE)
    {
        /*
         * Clean calls may not modify test return status
         */
        shr_error_e rv;
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_packet_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        int dst_present;
        bcm_port_t dst_port;
        bcm_pbmp_t logical_ports;
        uint32 max_cell_size, min_cell_size;

        SH_SAND_GET_ENUM("if_type", if_type);
        SH_SAND_GET_STR("packet", packet_n);
        SH_SAND_GET_BOOL("clean", to_clean);
        SH_SAND_GET_PORT("cpu_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, cpu_port);

        SH_SAND_IS_PRESENT("dst_port", dst_present);
        SH_SAND_GET_PORT("dst_port", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, dst_port);
        SH_SAND_GET_UINT32("max_cell_size", max_cell_size);
        SH_SAND_GET_UINT32("min_cell_size", min_cell_size);

        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "%s %s %s\r\n", test_name, EMPTY, EMPTY, EMPTY);
        LOG_CLI_EX("==> cpu_port %d, Packet_name %s, clean %d if_type %d\r\n", cpu_port, packet_n, to_clean, if_type);
        LOG_CLI_EX("==> dst_present %d, dst_port %d, min_cell_size %d, max_cell_size %d.\r\n", dst_present, dst_port,
                   min_cell_size, max_cell_size);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Procedure to indicate whether to carry out 'tdm bypass' test.
 *   Currently, this test is carried out for device only (not on adapter)
 * \param [in] unit             - unit id
 * \param [in] invoke_list      - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 */
static shr_error_e
ctest_dnx_tdm_bypass_support_check(
    int unit,
    rhlist_t * invoke_list)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_SET_CURRENT_ERR(sh_cmd_is_device(unit, invoke_list));
    /*
     * Note that SHR_IF_ERR_EXIT() is not used here simply because it prints a
     * message, in case of error.
     */
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
    {
        /*
         * Reach here if unit is a real 'device'. In that case, return
         * value is _SHR_E_NONE
         */
        SHR_EXIT();
    }
    else
    {
        /*
         * Reach here if this is 'adapter'. In that case, return
         * value is _SHR_E_NOT_FOUND
         */
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Procedure to indicate whether to carry out 'tdm fabric' test.
 *   Currently, this test is carried out for device only (not on adapter)
 *   and provided it is J2C (has fabric).
 * \param [in] unit             - unit id
 * \param [in] invoke_list      - unused here (keep callback type)
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_NOT_FOUND - Otherwise
 */
static shr_error_e
ctest_dnx_tdm_fabric_support_check(
    int unit,
    rhlist_t * invoke_list)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_SET_CURRENT_ERR(sh_cmd_is_device(unit, invoke_list));
    /*
     * Note that SHR_IF_ERR_EXIT() is not used here simply because it prints a
     * message, in case of error.
     */
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
    {
        /*
         * Reach here if unit is a real 'device'. If it also has fabric then
         * return _SHR_E_NONE. Note, however, that, in some cases, a chip may have
         * fabric but still may not have any active links.
         */
        if (!soc_is(unit, J2C_TYPE))
        {
            /*
             * Reach here if device does not have fabric. In that case, return
             * value is _SHR_E_NOT_FOUND
             */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
        SHR_EXIT();
    }
    else
    {
        /*
         * Reach here if this is 'adapter'. In that case, return
         * value is _SHR_E_NOT_FOUND
         */
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_external_ftmh_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int to_clean = FALSE;
    bcm_port_t cpu_port, dst_port, src_port;
    bcm_pbmp_t logical_ports;
    char *test_name;
    rhhandle_t ctest_soc_set_h = NULL;
    int destination_core_id = 0;
    rhhandle_t packet_tx = NULL;
    SHR_FUNC_INIT_VARS(unit);
    test_name = "dnx_tdm_external_ftmh_test_cmd";

    /** parse arguments */
    {
        SH_SAND_GET_PORT("cpu_port", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &cpu_port));

        SH_SAND_GET_PORT("src_port", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &src_port));

        SH_SAND_GET_PORT("dst_port", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port));

        SH_SAND_GET_BOOL("clean", to_clean);
    }

    /** modify soc properties - set ports (src_port, dst_port) to TDM */
    {
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;
        bcm_gport_t gport;
        bcm_port_resource_t resource;
        int source_core_id = 0;
        int nif_interface_id = 0;
        uint32 tm_port;
        uint32 flags = 0;
        char *ucode_port_type;
        char ucode_port_src_key[RHNAME_MAX_SIZE], ucode_port_src_value[RHNAME_MAX_SIZE],
            ucode_port_dst_key[RHNAME_MAX_SIZE], ucode_port_dst_value[RHNAME_MAX_SIZE];
        int is_ilkn_src;

        /** set src_port as tdm */
        {
            SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
            nif_interface_id = interface_info.interface_id;
            tm_port = mapping_info.tm_port;
            source_core_id = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, src_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));

            SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str
                            (unit, resource.speed, &is_ilkn_src, &ucode_port_type));

            sal_snprintf(ucode_port_src_key, RHNAME_MAX_SIZE, "ucode_port_%d.%d", src_port, unit);
            sal_snprintf(ucode_port_src_value, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:if_tdm_only:tdm", ucode_port_type,
                         nif_interface_id, source_core_id, tm_port);
        }

        /** set dst_port as tdm */
        {
            SHR_IF_ERR_EXIT(bcm_port_get(unit, dst_port, &flags, &interface_info, &mapping_info));
            nif_interface_id = interface_info.interface_id;
            tm_port = mapping_info.tm_port;
            destination_core_id = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, dst_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));

            SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str
                            (unit, resource.speed, &is_ilkn_src, &ucode_port_type));

            sal_snprintf(ucode_port_dst_key, RHNAME_MAX_SIZE, "ucode_port_%d.%d", dst_port, unit);
            sal_snprintf(ucode_port_dst_value, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:if_tdm_only:tdm", ucode_port_type,
                         nif_interface_id, destination_core_id, tm_port);
        }

        /** init soc properties */
        {
            ctest_soc_property_t ctest_soc_property[] = { {ucode_port_src_key, ucode_port_src_value},
            {ucode_port_dst_key, ucode_port_dst_value},
            {"tdm_mode", "TDM_OPTIMIZED"},
            {"ucode_port_20.*", NULL},
            {"flexe_device_mode*", NULL},
            {NULL}
            };

            SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
        }
    }

    /** Configure IRE to no change */
    {
        int ingress_context_id;
        bcm_tdm_interface_config_t set_interface_config;
        bcm_port_t *fabric_ports_p;
        int fabric_ports_count;

        fabric_ports_p = NULL;
        fabric_ports_count = -1;
        set_interface_config.stream_id_base = 0;
        set_interface_config.stream_id_enable = 0;
        set_interface_config.stream_id_key_size = 1;
        set_interface_config.stream_id_key_offset = 0;
        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure(unit, TRUE, src_port, 0, TRUE, dst_port, bcmTdmIngressEditingNoChange,
                                              FALSE, -1, &set_interface_config, fabric_ports_p, fabric_ports_count,
                                              &ingress_context_id));
    }

    /**
     * Configure Egress editing, this is not interesting for the purpose of this test,
     * but it is added to provide a fuller picture.
     */
    {
        /*
         * Set egress editing to:
         * Prepended no header to outgoing TDM packet.
         */
        bcm_gport_t gport;
        bcm_tdm_egress_editing_t editing;
        uint32 flags;

        BCM_GPORT_LOCAL_SET(gport, dst_port);
        sal_memset(&editing, 0, sizeof(editing));
        flags = 0;
        editing.type = bcmTdmEgressEditingNoStamp;
        SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));
    }

    /** setup forwarding */
    {
        /**  forward cpu packet to src_port */
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port, src_port, 1));

        /** set loopback on src_port */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, src_port, BCM_PORT_LOOPBACK_PHY));

        /*
         * We have loopback on source port. However, we do not want to see signals on
         * egress of source port since this is not part of the test.
         * So, disable visibility for egress of src_port to allow for tdm packet to be recorded
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set
                            (unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, src_port, FALSE),
                            "SRC Port Force set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set
                            (unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, src_port, FALSE),
                            "SRC Port Enable set failed\n");

        /**
         * TDM forwarding is done by received FTMH, no need to do anything regarding this
         * forwarding besides setting the correct FTMH on the CPU packet
         */
    }

    /** Allocate and init packet_tx.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));

    if (dnx_data_headers.optimized_ftmh.feature_get(unit, dnx_data_headers_optimized_ftmh_ftmh_opt_parse))
    {
        /** send traffic */
        uint64 recieved_packets;

        /** Add headers to packet */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "FTMH_OPT"));

        /** clear stats on destination port */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, dst_port));

        /** send packets and verify result*/
        {
            uint32 pp_dsp = dst_port;
            uint32 destination_fap_id = unit;
            /** set pattern to user defined bits */
            uint32 user_defined_bits = 0x7;
            uint32 expected_value;
            uint32 ftmh_opt_enabled;

            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                            (unit, packet_tx, "FTMH_OPT.FTMH_OPT_PP_DSP_or_Multicast_ID_7_0", &pp_dsp, 8));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                            (unit, packet_tx, "FTMH_OPT.FTMH_OPT_Destination_FAP_ID_or_Multicast_ID_18_8",
                             &destination_fap_id, 11));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                            (unit, packet_tx, "FTMH_OPT.FTMH_OPT_User_Defined", &user_defined_bits, 3));

            /** Send packet.*/
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port, packet_tx, 0));

            /** verify packet was sent to correct port (dst_port) */
            sal_sleep(2);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));

            /** Check if packet arrived */
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, dst_port, snmpEtherStatsTXNoErrors, &recieved_packets));
            if (COMPILER_64_LO(recieved_packets) != 1)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected %d TDM packets on port %d but got %d.\n", 1, dst_port,
                             COMPILER_64_LO(recieved_packets));
            }

            /** Get is_tdm signal and make sure it is true */
            expected_value = 1;
            SHR_IF_ERR_EXIT(sand_signal_verify
                            (unit, destination_core_id, "ERPP", "ERparser", NULL, "is_TDM", &expected_value, 1, NULL,
                             NULL, 0));

            /** get bytes_to_strip signal and make sure it is 4 (corresponding to tdm optimaized) */
            expected_value = 4;
            SHR_IF_ERR_EXIT(sand_signal_verify
                            (unit, destination_core_id, "ETPP", "Trap", NULL, "Bytes_to_Strip", &expected_value, 1,
                             NULL, NULL, 0));

            SHR_IF_ERR_EXIT(packet_ftmh_opt(unit, destination_core_id, NULL, &ftmh_opt_enabled));
            if (ftmh_opt_enabled)
            {
                SHR_IF_ERR_EXIT(sand_signal_verify
                                (unit, destination_core_id, "ERPP", NULL, "ERparser", "FTMH_OPT.FTMH_OPT_User_Defined",
                                 &user_defined_bits, 1, NULL, NULL, 0));
            }
            else
            {
                uint8
                    result_str[(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE *
                                (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + sizeof(uint32) + 1];
                char dummy_expected_value[(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE *
                                           (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + sizeof(uint32) + 1] = { 0 };
                int size;
                uint8 ftmh_image[DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE] = { 0 };

                size = (sizeof(result_str) - 1) / (SAL_UINT32_NOF_BITS / SAL_UINT8_NOF_BITS);
                SHR_SET_CURRENT_ERR(sand_signal_verify
                                    (unit, destination_core_id, "ERPP", "Fabric", "ERParser", "FTMH_Base",
                                     (uint32 *) dummy_expected_value, size, NULL, (char *) result_str, 0));
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);

                LOG_CLI_EX("\r\n" "Result       : %s %s%s%s\r\n", result_str, EMPTY, EMPTY, EMPTY);
                SHR_IF_ERR_EXIT(utilex_hex_char_array_to_num
                                (unit, (uint8 *) result_str,
                                 (sizeof(ftmh_image) * (SAL_UINT8_NOF_BITS / SAL_NIBBLE_NOF_BITS)) + 1, ftmh_image));
                LOG_CLI_EX("\r\n" "FTMH image   : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", ftmh_image[0], ftmh_image[1],
                           ftmh_image[2], ftmh_image[3]);
                LOG_CLI_EX("==>          : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", ftmh_image[4], ftmh_image[5],
                           ftmh_image[6], ftmh_image[7]);
                LOG_CLI_EX("==>          : 0x%02X 0x%02X %s %s\r\n\n", ftmh_image[8], ftmh_image[9], EMPTY, EMPTY);

                /** Check that FTMH bits containing user defined bits have the same value that was put there */
                if (((ftmh_image[1] >> 3) & 0x7) != user_defined_bits)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected user defined bits to be 0x%x \n", user_defined_bits);
                }
            }
        }
    }

exit:
    /** free packet_tx */
    diag_sand_packet_free(unit, packet_tx);
    /** print counters */
    SHR_IF_ERR_CONT(sh_process_command(unit, "show counter"));

    if (to_clean)
    {
        int rv;
        /** reset soc properties */
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_external_ftmh_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }

    if (!SHR_FUNC_ERR())
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' Failed! %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - test hybrid TDM mode:
 * 1. Configure PRD block to indicate TDM for a certain stream
 * 2. Send 1 packet which matches the configured TDM rule in PRD - verify TDM processing.
 * 3. Send 1 packet which doesn't match the PRD rule - verify non TDM data processing.
 *
 * \note - This test only test small part of TDM options since they are already tested in other
 * test.
 */
static shr_error_e
dnx_tdm_hybrid_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int to_clean = FALSE, rv;
    bcm_port_t cpu_port, dst_port_tdm, dst_port_data, src_port;
    bcm_pbmp_t logical_ports;
    uint32 flags = 0;
    rhhandle_t ctest_soc_set_h = NULL;
    char *test_name;
    uint32 pcp = 3, dei = 0;
    ctest_cint_argument_t cint_arguments[3];
    rhhandle_t packet_tx = NULL;

    SHR_FUNC_INIT_VARS(unit);
    test_name = "dnx_tdm_hybrid_test_cmd";

    /** parse arguments */
    {
        SH_SAND_GET_PORT("cpu_port", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &cpu_port));

        SH_SAND_GET_PORT("src_port", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &src_port));

        SH_SAND_GET_PORT("dst_port_tdm", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port_tdm));

        SH_SAND_GET_PORT("dst_port_data", logical_ports);
        SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port_data));

        SH_SAND_GET_BOOL("clean", to_clean);
    }

    /** modify soc properties - set ports (src_port, dst_port_tdm)  to hybrid mode*/
    {
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;
        bcm_gport_t gport;
        bcm_port_resource_t resource;
        int core_id = 0;
        int nif_interface_id = 0;
        uint32 tm_port;
        char *ucode_port_type;
        char ucode_port_src_key[RHNAME_MAX_SIZE], ucode_port_src_value[RHNAME_MAX_SIZE],
            ucode_port_dst_key[RHNAME_MAX_SIZE], ucode_port_dst_value[RHNAME_MAX_SIZE];
        int is_ilkn_src;

        /** set src_port as tdm hybrid */
        {
            SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
            nif_interface_id = interface_info.interface_id;
            tm_port = mapping_info.tm_port;
            core_id = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, src_port);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));

            SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, resource.speed, &is_ilkn_src,
                                                                   &ucode_port_type));
            if (is_ilkn_src)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n" "src_port=%d is ILKN port. Hybrid test doesn't support ILKN port\r\n\n", src_port);
            }

            sal_snprintf(ucode_port_src_key, RHNAME_MAX_SIZE, "ucode_port_%d.%d", src_port, unit);
            sal_snprintf(ucode_port_src_value, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:if_tdm_hybrid", ucode_port_type,
                         nif_interface_id, core_id, tm_port);
        }

        /** set dst_port_tdm as tdm hybrid */
        {
            SHR_IF_ERR_EXIT(bcm_port_get(unit, dst_port_tdm, &flags, &interface_info, &mapping_info));
            nif_interface_id = interface_info.interface_id;
            tm_port = mapping_info.tm_port;
            core_id = mapping_info.core;
            BCM_GPORT_LOCAL_SET(gport, dst_port_tdm);
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));

            SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, resource.speed, &is_ilkn_src,
                                                                   &ucode_port_type));
            if (is_ilkn_src)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n" "dst_port_tdm=%d is ILKN port. Hybrid test doesn't support ILKN port\r\n\n",
                             dst_port_tdm);
            }

            sal_snprintf(ucode_port_dst_key, RHNAME_MAX_SIZE, "ucode_port_%d.%d", dst_port_tdm, unit);
            sal_snprintf(ucode_port_dst_value, RHNAME_MAX_SIZE, "%s%d:core_%d.%d:if_tdm_hybrid", ucode_port_type,
                         nif_interface_id, core_id, tm_port);
        }

        /** init soc properties */
        {
            ctest_soc_property_t ctest_soc_property[] = {
                {ucode_port_src_key, ucode_port_src_value}
                ,
                {ucode_port_dst_key, ucode_port_dst_value}
                ,
                {"ucode_port_20.*", NULL}
                ,
                {"tdm_mode", "TDM_OPTIMIZED"}
                ,
                {"flexe_device_mode*", NULL}
                ,
                {NULL}
            };

            SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
        }
    }

    /** load cints */
    {

        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_port_misc_configuration.c"),
                            "cint_port_misc_configuration.c Load Failed\n");
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "sand/utility/cint_sand_utils_global.c"),
                            "cint_sand_utils_global.c Load Failed\n");
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "sand/utility/cint_sand_utils_port.c"),
                            "cint_sand_utils_port.c Load Failed\n");
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/cint_nif_phy_data.c"),
                            "cint_nif_phy_data.c Load Failed\n");
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/cint_nif_test.c"),
                            "cint_nif_test.c Load Failed\n");
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/tdm/cint_tdm_bypass_mode.c"),
                            "cint_tdm_bypass_mode.c Load Failed\n");
    }

    /** setup prd on src_port - set priority 3 to indicate TDM */
    {
        /** map port to prd profile (if required) */
        cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[0].value.value_int32 = src_port;

        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "set_clu_prd_port_profile", cint_arguments, 1),
                            "set_clu_prd_port_profile Run Failed\n");

        /** set TDM for pcp=3 dei=0 */
        cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[0].value.value_int32 = src_port;
        cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[1].value.value_int32 = pcp;
        cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[2].value.value_int32 = dei;
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_nif_priority_l2_tdm_set", cint_arguments, 3),
                            "cint_nif_priority_l2_tdm_set Run Failed\n");
    }

    /** setup forwarding */
    {

        /**  forward cpu packet to src_port */
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port, src_port, 1));

        /** set loopback on src_port */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, src_port, BCM_PORT_LOOPBACK_PHY));

        /** forward src_port to dst_port_data for data traffic */
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, src_port, dst_port_data, 1));

        /** forward src_port to dst_port_tdm for tdm traffic */
        cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[0].value.value_int32 = src_port;
        cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[1].value.value_int32 = dst_port_tdm;
        cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[2].value.value_int32 = 0;
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "tdm_bypass_mode_example", cint_arguments, 3),
                            "tdm_bypass_mode_example Run Failed\n");
    }

    /*
     * send traffic:
     * 1. tdm packet (pcp=3, dei=0) - should be received on dst_port_tdm
     * 2. data packet (pcp!=3) - should be received on dst_port_data
     *
     */
    {
        uint64 recieved_packets;

        /** Allocate and init packet_tx.*/
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));

        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ETH1"));

        /** clear stats on destination ports */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, dst_port_tdm));
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, dst_port_data));

        /** send tdm packet - pcp=3, dei=0 */
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "ETH1.VLAN.PCP", &pcp, 3));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "ETH1.VLAN.DEI", &dei, 1));
            /** Send packet.*/
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port, packet_tx, 0));

            /** verify packet was sent to correct port (dst_port_tdm) */
            sal_sleep(2);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));

            /** Check if packet arrived */
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, dst_port_tdm, snmpEtherStatsTXNoErrors, &recieved_packets));
            if (COMPILER_64_LO(recieved_packets) != 1)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected %d TDM packets on port %d but got %d.\n",
                             1, dst_port_tdm, COMPILER_64_LO(recieved_packets));
            }
        }

        /** send data packet - pcp!=3 */
        {
            pcp = 0;
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "ETH1.VLAN.PCP", &pcp, 3));

            /** Send packet.*/
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port, packet_tx, 0));

            /** verify packet was sent to correct port (dst_port_data) */
            sal_sleep(2);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));

            /** Check if packet arrived */
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, dst_port_data, snmpEtherStatsTXNoErrors, &recieved_packets));
            if (COMPILER_64_LO(recieved_packets) != 1)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected %d DATA packets on port %d but got %d.\n",
                             1, dst_port_data, COMPILER_64_LO(recieved_packets));
            }
        }
    }

exit:
    /** print counters */
    sh_process_command(unit, "show counter");

    /** free packet */
    diag_sand_packet_free(unit, packet_tx);

    if (to_clean)
    {
        /** reset cint */
        rv = ctest_dnxc_cint_reset(unit);
        if (rv != _SHR_E_NONE)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
        /** reset soc properties */
        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_hybrid_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }

    if (!SHR_FUNC_ERR())
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' Failed! %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_port_ucode_soc_set(
    int unit,
    bcm_pbmp_t port_pbmp,
    char *tdm_if_type,
    ctest_soc_property_t * ctest_soc_property)
{
    int count;
    char *ucode_port_type;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t port;
    bcm_gport_t gport;
    bcm_port_resource_t resource;
    int is_ilkn_src;
    int add_channel;
    int speed;
    int core_id, nif_interface_id;
    uint32 channel_id;
    uint32 tm_port;

    /*
     * Assum at most 8 ports are supported
     */
    static char soc_name[8][RHNAME_MAX_SIZE];
    static char soc_name_all[8][RHNAME_MAX_SIZE];
    static char soc_value[8][RHNAME_MAX_SIZE];

    int i = 0;

    SHR_FUNC_INIT_VARS(unit);
    BCM_PBMP_COUNT(port_pbmp, count);
    if (!count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There should be at least one logical port in input bitmap\r\n");
    }
    BCM_PBMP_ITER(port_pbmp, port)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
        LOG_CLI_EX("\r\n"
                   "Port     : interface_info.interface_id %d. mapping_info.channel %d. \r\n"
                   "mapping_info.core %d. mapping_info.tm_port %d. \r\n\n",
                   interface_info.interface_id, mapping_info.channel, mapping_info.core, mapping_info.tm_port);
        nif_interface_id = interface_info.interface_id;
        channel_id = mapping_info.channel;
        tm_port = mapping_info.tm_port;
        core_id = mapping_info.core;
        BCM_GPORT_LOCAL_SET(gport, port);
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, gport, &resource));
        speed = resource.speed;

        add_channel = FALSE;
        SHR_IF_ERR_EXIT(dnx_tdm_convert_speed_to_port_type_str(unit, speed, &is_ilkn_src, &ucode_port_type));
        if (is_ilkn_src)
        {
            add_channel = TRUE;
        }
        else
        {
            if (channel_id != 0)
            {
                /*
                 * For Ethernet ports, 'channel_id' must be zero. There is no other option.
                 */
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "Caller specified 'channel_id' to be %d.\r\n"
                             "==> For Ethernet ports, 'channel_id' must be zero. There is no other option.\r\n",
                             channel_id);
            }
        }
        sal_snprintf(soc_name_all[i], RHNAME_MAX_SIZE, "ucode_port_%d.*", port);
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, soc_name_all[i], NULL));
        sal_snprintf(soc_name[i], RHNAME_MAX_SIZE, "ucode_port_%d", port);
        if (add_channel)
        {
            sal_snprintf(soc_value[i], RHNAME_MAX_SIZE, "%s%d.%d:core_%d.%d:%s:tdm", ucode_port_type,
                         nif_interface_id, channel_id, core_id, tm_port, tdm_if_type);
        }
        else
        {
            sal_snprintf(soc_value[i], RHNAME_MAX_SIZE, "%s%d:core_%d.%d:%s:tdm", ucode_port_type,
                         nif_interface_id, core_id, tm_port, tdm_if_type);
        }
        SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, soc_name[i], soc_value[i]));
        i++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_packet_drop_validate(
    int unit,
    int expect_count,
    int expect_drop_count)
{

    uint64 tdm_packet_counter;
    uint32 tdm_dropped_packet_counter;
    uint32 err_tdm_packet_counter;
    uint64 fdt_egq_tdm_packet_counter;
    SHR_FUNC_INIT_VARS(unit);
    {
        /*
         * Check TDM counters. They are assumed to have been cleared just before the
         * 'send' operation.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_PACKET_COUNTER,
                                         0, 4,
                                         GEN_DBAL_FIELD64, DBAL_FIELD_TDM_PACKET_COUNTER, INST_SINGLE,
                                         &tdm_packet_counter, GEN_DBAL_FIELD32, DBAL_FIELD_TDM_DROPPED_PACKET_COUNTER,
                                         INST_SINGLE, &tdm_dropped_packet_counter, GEN_DBAL_FIELD32,
                                         DBAL_FIELD_ERR_TDM_PACKET_COUNTER, INST_SINGLE, &err_tdm_packet_counter,
                                         GEN_DBAL_FIELD64, DBAL_FIELD_FDT_EGQ_TDM_PACKET_COUNTER, INST_SINGLE,
                                         &fdt_egq_tdm_packet_counter, GEN_DBAL_FIELD_LAST_MARK));
        if (expect_drop_count == 0)
        {
            if (tdm_dropped_packet_counter != 0)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n"
                             "Some (%d) TDM packets were dropped due to blocked context. We expect no drops. Quit.\r\n",
                             tdm_dropped_packet_counter);
            }
            else
            {
                LOG_CLI_EX("\r\n"
                           "TDM packets were not dropped due to blocked contexts. This is as expected. %s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
            }
            if (err_tdm_packet_counter != 0)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n" "Some (%d) TDM packets were dropped due to errors. We expect no drops. Quit.\r\n",
                             err_tdm_packet_counter);
            }
            else
            {
                LOG_CLI_EX("\r\n"
                           "TDM packets were not dropped due to errors. This is as expected. %s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
            }

        }
        else
        {
            if (tdm_dropped_packet_counter == 0)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n"
                             "Some (%d) TDM packets were not dropped due to blocked context. We expect drops. Quit.\r\n",
                             tdm_dropped_packet_counter);
            }
            else if (tdm_dropped_packet_counter == expect_drop_count)
            {
                LOG_CLI_EX("\r\n"
                           "TDM packets were dropped due to blocked contexts. This is as expected. %s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
            }
            else
            {
                LOG_CLI_EX("\r\n"
                           "TDM packets drop counter was not same as the expected. Drop count %d, Expect drop %d%s%s\r\n\n",
                           tdm_dropped_packet_counter, expect_drop_count, EMPTY, EMPTY);

            }
            if (err_tdm_packet_counter == 0)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n" "Some (%d) TDM packets were not dropped due to errors. We expect drops. Quit.\r\n",
                             err_tdm_packet_counter);
            }
            else if (err_tdm_packet_counter == expect_drop_count)
            {
                LOG_CLI_EX("\r\n"
                           "TDM packets were dropped due to errors. This is as expected. %s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
            }
            else
            {
                LOG_CLI_EX("\r\n"
                           "TDM error packets drop counter was not same as the expected. Error drop count %d, Expect drop %d%s%s\r\n\n",
                           err_tdm_packet_counter, expect_drop_count, EMPTY, EMPTY);
            }

        }
        LOG_CLI_EX("\r\n"
                   "Number of TDM packets at entry of ingress is (hi,lo) (%d,%d).\r\n"
                   "==> Number of TDM packets sent to FDT/EGQ is (hi,lo) (%d,%d)\r\n\n",
                   COMPILER_64_HI(tdm_packet_counter), COMPILER_64_LO(tdm_packet_counter),
                   COMPILER_64_HI(fdt_egq_tdm_packet_counter), COMPILER_64_LO(fdt_egq_tdm_packet_counter));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_port_counter_validate(
    int unit,
    bcm_port_t src_port,
    bcm_port_t dst_port,
    int expect_fail_on_is_tdm,
    int egress_edit_type,
    int ftmh_header_size)
{
    uint32 octets_rx_from_src_port, octets_tx_from_dst_port;
    uint32 err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port;
    SHR_FUNC_INIT_VARS(unit);

    octets_rx_from_src_port = 0;
    octets_tx_from_dst_port = 0;
    err_pkt_rx_from_src_port = 0;
    err_pkt_tx_from_dst_port = 0;
    /*
     * Get counters info on received/transmitted packets.
     */
    SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, FALSE, src_port, dst_port, &octets_rx_from_src_port,
                                                  &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                  &err_pkt_tx_from_dst_port));
    /*
     * At this point, the following holds:
     *   octets_rx_from_src_port is the number of bytes received from source port.
     *   octets_tx_from_dst_port is the number of bytes transmitted through destination port
     *     They should be equal.
     *   err_pkt_rx_from_src_port is the number of error packets detected on reception from source port.
     *   err_pkt_tx_from_dst_port is the number of error packets detected on transmission through destination port
     *     They should be zero.
     */
    if ((err_pkt_rx_from_src_port != 0) || (err_pkt_tx_from_dst_port != 0))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "\r\n" "Erros reported by HW: err_pkt_rx_from_src_port %d err_pkt_tx_from_dst_port %d\r\n\n",
                     err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port);
    }
    if ((octets_rx_from_src_port == 0) || (octets_tx_from_dst_port == 0))
    {
        /*
         * At this point, both number of received bytes and number of transmitted bytes need to be non-zero.
         */
        if ((expect_fail_on_is_tdm == TRUE) && (octets_tx_from_dst_port == 0))
        {
            /*
             * If checking 'is_tdm' is expected to fail then we also expect no packet to go to
             * destination port.
             */
            LOG_CLI_EX("\r\n"
                       "No packet was sent to 'destination port. This is as expected (negative test). So far, test is successfull. %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "Either number of bytes received from source port (%d) or number of bytes transmitted through destination port (%d) is zero. Quit.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, octets_tx_from_dst_port);
        }
    }

    if (egress_edit_type == bcmTdmEgressEditingNoHeader)
    {
        /*
         * If header is stripped on egress then we expect received packet to be of the same size as the
         * one transmitted on egress.
         */
        if (octets_rx_from_src_port != octets_tx_from_dst_port)
        {
            /*
             * At this point, we fail the test if they are not equal.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "Number of bytes received from source port (%d) is not equal to number of bytes transmitted through destination port (%d).\r\n"
                         "Test has failed.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, octets_tx_from_dst_port);
        }
    }
    else
    {
        /*
         * If header is not stripped on egress then we expect received packet to be smaller than
         * one transmitted, on egress, by the size of the header.
         */
        if ((octets_rx_from_src_port + ftmh_header_size) != octets_tx_from_dst_port)
        {
            /*
             * At this point, we fail the test if they are not equal.
             */
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Source port is %d. Destination port is %d\r\n"
                         "==> Number of bytes received from source port (%d) plus header size (%d) is not equal to number\r\n"
                         "==> of bytes transmitted through destination port (%d).\r\n"
                         "==> Test has  failed.\r\n\n",
                         src_port, dst_port, octets_rx_from_src_port, ftmh_header_size, octets_tx_from_dst_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Use bcm_tdm_ingress_context_failover_set() and see that some contexts are blocked and some are not
 *
 * \param [in] unit - Unit ID
 * \param [in] port1 - Source logical port1
 * \param [in] port2 - Source logical port2
 */
static shr_error_e
dnx_tdm_failover_drop_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_tdm_failover_drop_test_cmd";
    int to_clean = FALSE;
    rhhandle_t ctest_soc_set_h = NULL;
    rhhandle_t packet_h = NULL;
    bcm_port_t src_port_1, src_port_2, cpu_port_1, cpu_port_2;
    bcm_port_t dst_port_1, dst_port_2;
    bcm_port_t mcast_port[MAX_NUM_MCAST_PORTS];
    int dst_present;
    bcm_pbmp_t logical_ports;
    int core_id_dst;
    uint32 channel_id_src;
    char gen_string[RHNAME_MAX_SIZE];
    char *tdm_if_type;
    int if_type;
    int ingress_context_id_1;
    int ingress_context_id_2;
    char *packet_n;
    ctest_soc_property_t ctest_soc_property[MAX_NUM_SOC_PROPERTIES];
    uint32 octets_rx_from_src_port, octets_tx_from_dst_port;
    uint32 err_pkt_rx_from_src_port, err_pkt_tx_from_dst_port;
    int channel_id_dst;
    bcm_multicast_t multicast_id_1, multicast_id_2;
    int multicast_cud;
    uint32 vid = 0;
    int expect_fail_on_is_tdm;
    uint32 max_cell_size, min_cell_size;
    int egress_edit_type;
    int ftmh_header_size;
    uint32 use_optimized_ftmh;
    uint8 ftmh_image[DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE];
    int stamp_mcid_with_sid;
    uint32 stream_multicast_prefix;
    int stream_id_enable, vid_stream_enable;
    int stream_id_base, stream_id_key_size, stream_id_key_offset;
    bcm_tdm_ingress_editing_type_t context_type;
    int num_members_in_multicast_group;
    bcm_pbmp_t port_pbmp;
    int mcast_index;
    static char nof_egress_mc_groups[RHNAME_MAX_SIZE];
    uint32 ftmh_opt_enabled;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just make sure 'ingress_context_id' is initialized to an illegal value.
     */
    ingress_context_id_1 = -1;
    ingress_context_id_2 = -1;
    use_optimized_ftmh = 0;
    /*
     * Just make sure 'dst_port'/'src_port'/'cpu_port'/'mcast_port' are initialized to an illegal value.
     */
    /*
     * Filling an array with '-1' results in setting all elements to '-1'.
     */
    sal_memset(mcast_port, -1, sizeof(mcast_port));
    dst_port_1 = -1;
    dst_port_2 = -1;
    src_port_1 = -1;
    src_port_2 = -1;
    cpu_port_1 = 201;
    cpu_port_2 = 202;
    packet_n = "tdm_01";
    multicast_id_1 = 1025;
    multicast_id_2 = 1026;
    multicast_cud = 2748;
    stream_id_base = 1000;
    stream_id_key_size = 1;
    stream_id_key_offset = 0;
    stream_multicast_prefix = 1;
    egress_edit_type = bcmTdmEgressEditingNoHeader;
    context_type = bcmTdmIngressEditingPrepend;
    num_members_in_multicast_group = 2;
    stream_id_enable = FALSE;
    vid_stream_enable = FALSE;
    max_cell_size = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);
    min_cell_size = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    sal_memset(ftmh_image, 0, sizeof(ftmh_image));
    sal_memset(ctest_soc_property, 0, sizeof(ctest_soc_property));
    SHR_IF_ERR_EXIT(dnx_ctest_soc_add(unit, ctest_soc_property, "tdm_mode", "TDM_OPTIMIZED"));
    /*
     * Core_id of destination port.
     */
    core_id_dst = 0;
    /*
     * If this flag ('expect_fail_on_is_tdm') is TRUE then we expect this test to fail on 'is_tdm' check.
     */
    expect_fail_on_is_tdm = FALSE;
    stamp_mcid_with_sid = FALSE;

    SH_SAND_IS_PRESENT("dst_port_1", dst_present);
    if (dst_present != FALSE)
    {
        /*
         * Also, no 'MC-CUD stamping' is relevant so it should not be specified for egress editing.
         */
        if (egress_edit_type == bcmTdmEgressEditingStampMulticastEncapId)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "Since 'destination' is present on the command line then this is not a multucast setup\r\n"
                         "==> so 'egress_edit_type' may not be 'bcmTdmEgressEditingStampMulticastEncapId'.\r\n"
                         "==> egress_edit_type %d. Quit.\r\n", egress_edit_type);
        }
        if (stamp_mcid_with_sid == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "'stamp_mcid_with_sid' is set but this is not a multicast setup (destination is present on the line). Quit.\r\n");
        }
    }
    else
    {
        if (stamp_mcid_with_sid == TRUE)
        {
            if (stream_id_enable == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                             "'stamp_mcid_with_sid' is set but 'stream_id_enable' is NOT set.\r\n"
                             "==> This is an illegal combination since this stamping is only active\r\n"
                             "==> on multicast setup with 'stream_enabled'. Quit.\r\n");
            }
        }
        /*
         * Also, 'MC-CUD stamping' collides with 'stamp_mcid_with_sid' so only one of them can
         * be set.
         */
        if ((egress_edit_type == bcmTdmEgressEditingStampMulticastEncapId) && (stamp_mcid_with_sid == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                         "Both 'stamp_mcid_with_sid' is set and 'bcmTdmEgressEditingStampMulticastEncapId' is specified.\r\n"
                         "==> This is illegal since they both change the same field on FTMH-header. Quit.\r\n\n");
        }
    }
    if (min_cell_size > max_cell_size)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n"
                     "'min_cell_size' (%d) is smaller tha 'max_cell_size' (%d).\r\n"
                     "==> Illegal combination. Quit.\r\n", min_cell_size, max_cell_size);
    }
    /*
     * Stream is based on VID
     * In this mode most of the arguments are set manually
     */
    if ((stamp_mcid_with_sid == FALSE) && (vid_stream_enable != FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "If VID stamping mode is enabled then 'stamp_mcid_with_sid' must be set as well.\r\n");
    }
    /*
     * Obtain incoming port and its properties
     */
    SH_SAND_GET_ENUM("if_type", if_type);
    /*
     * Get port whether it is or is not present on the command line.
     * If it is not present, then get its defualt value.
     */
    SH_SAND_GET_PORT("src_port_1", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &src_port_1));
    SH_SAND_GET_PORT("src_port_2", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &src_port_2));
    SH_SAND_GET_PORT("dst_port_1", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port_1));
    SH_SAND_GET_PORT("dst_port_2", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &dst_port_2));
    /*
     * Get 'mcast_port_1/_2' whether it is or is not present on the command line.
     * If it is not present, then get its defualt value.
     */
    SH_SAND_GET_PORT("mcast_port_1", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &mcast_port[0]));
    SH_SAND_GET_PORT("mcast_port_2", logical_ports);
    SHR_IF_ERR_EXIT(dnx_tdm_get_first_and_only_port(unit, logical_ports, &mcast_port[1]));

    SHR_IF_ERR_EXIT(dnx_tdm_base_soc_properties(unit, ctest_soc_property));
    tdm_if_type = sh_sand_enum_value_text(sand_control, "if_type", if_type);
    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_PORT_ADD(port_pbmp, src_port_1);
    BCM_PBMP_PORT_ADD(port_pbmp, src_port_2);
    BCM_PBMP_PORT_ADD(port_pbmp, dst_port_1);
    BCM_PBMP_PORT_ADD(port_pbmp, dst_port_2);

    /*
     * For 'multicast' case, set all destinations of multicast group to be 'tdm' (see above and below)
     */
    if (dst_present == FALSE)
    {
        for (mcast_index = 0; mcast_index < MAX_NUM_MCAST_PORTS; mcast_index++)
        {
            BCM_PBMP_PORT_ADD(port_pbmp, mcast_port[mcast_index]);
        }
    }
    SHR_IF_ERR_EXIT(dnx_tdm_port_ucode_soc_set(unit, port_pbmp, tdm_if_type, ctest_soc_property));
    /*
     * Configure SOC Properties
     *          Set BYPASS Optimized Mode
     *          Assign interface mode
     *          Assign port property tdm
     */
    sal_snprintf(nof_egress_mc_groups, RHNAME_MAX_SIZE, "%d",
                 TDM_SID_TO_MCID(unit, dnx_data_tdm.params.nof_stream_ids_get(unit), stream_multicast_prefix));

    SHR_IF_ERR_EXIT(dnx_ctest_soc_add
                    (unit, ctest_soc_property, "multicast_egress_group_id_range_max", nof_egress_mc_groups));

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    if (dst_present == FALSE)
    {
        /*
         * If no destination is specified then assume destination is a multicast group.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_set_mcast_group
                        (unit, stamp_mcid_with_sid, stream_multicast_prefix, stream_id_base, stream_id_key_size,
                         multicast_cud, multicast_id_1, num_members_in_multicast_group, &mcast_port[0], dst_port_1));
        SHR_IF_ERR_EXIT(dnx_tdm_set_mcast_group
                        (unit, stamp_mcid_with_sid, stream_multicast_prefix, (stream_id_base + 1), stream_id_key_size,
                         multicast_cud, multicast_id_2, num_members_in_multicast_group, &mcast_port[1], dst_port_2));
    }
    /*
     * Configure IRE Part
     */
    {
        bcm_tdm_interface_config_t set_interface_config;
        int do_not_override_context;
        bcm_port_t *fabric_ports_p;
        int fabric_ports_count;

        fabric_ports_p = NULL;
        /*
         * Indicate all available fabric links are to be assigned to TDM (as well as to
         * other sources).
         */
        fabric_ports_count = -1;
        set_interface_config.stream_id_base = stream_id_base;
        set_interface_config.stream_id_key_size = stream_id_key_size;
        set_interface_config.stream_id_key_offset = stream_id_key_offset;
        set_interface_config.stream_id_enable = stream_id_enable;
        do_not_override_context = TRUE;
        channel_id_src = 0;

        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, src_port_1, channel_id_src, dst_present, dst_port_1,
                         context_type, stamp_mcid_with_sid, multicast_id_1, &set_interface_config, fabric_ports_p,
                         fabric_ports_count, &ingress_context_id_1));

        set_interface_config.stream_id_base = stream_id_base + 1;

        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, src_port_2, channel_id_src, dst_present, dst_port_2,
                         context_type, stamp_mcid_with_sid, multicast_id_2, &set_interface_config, fabric_ports_p,
                         fabric_ports_count, &ingress_context_id_2));
    }
    {
        /*
         * Calculate the size of FTMH header for the current setup.
         */
        SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_get(unit, &use_optimized_ftmh));
        if (use_optimized_ftmh)
        {
            ftmh_header_size = OPT_FTMH_HEADER_SIZE;
        }
        else
        {
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
                DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                ftmh_header_size = STD_FTMH_HEADER_SIZE;
            }
            else
            {
                ftmh_header_size = LEGACY_FTMH_HEADER_SIZE;
            }
        }
        LOG_CLI_EX("\r\n" "FTMH header size, for this test is %d %s%s%s\r\n\n", ftmh_header_size, EMPTY, EMPTY, EMPTY);
    }
    {
        /*
         * Set system parameters.
         * Use bcm_dnx_tdm_control_set()
         */
        bcm_tdm_control_t type;
        int arg;
        int min_packet_size, max_packet_size;
        int tdm_out_header_type, tdm_stream_multicast_prefix;
        uint32 octets_rx_from_cpu_port;

        /*
         * SET
         */
        type = bcmTdmBypassMinPacketSize;
        arg = (int) min_cell_size;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));
        type = bcmTdmBypassMaxPacketSize;
        arg = (int) max_cell_size;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));

        type = bcmTdmOutHeaderType;
        arg = (int) BCM_TDM_CONTROL_OUT_HEADER_FTMH;
        SHR_IF_ERR_EXIT(bcm_tdm_control_set(unit, type, arg));

        /*
         * GET
         */
        type = bcmTdmOutHeaderType;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_out_header_type = arg;
        type = bcmTdmStreamMulticastPrefix;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        tdm_stream_multicast_prefix = arg;
        type = bcmTdmBypassMinPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        min_packet_size = arg;
        type = bcmTdmBypassMaxPacketSize;
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, type, &arg));
        max_packet_size = arg;
        /*
         * Print extracted values.
         */
        LOG_CLI_EX("\r\n"
                   "bcmTdmBypassMinPacketSize %d. bcmTdmBypassMaxPacketSize %d. \r\n"
                   "bcmTdmOutHeaderType %d. bcmTdmStreamMulticastPrefix %d. \r\n\n",
                   min_packet_size, max_packet_size, tdm_out_header_type, tdm_stream_multicast_prefix);
        /*
         * Get counters info on transmitted packet.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                        (unit, packet_n, NULL, NULL, &octets_rx_from_cpu_port, NULL));
        /*
         * Print packet size, as specified on XML.
         */
        LOG_CLI_EX("\r\n"
                   "octets_rx_from_cpu_port (packt size as originated on CPU) %d. %s%s%s \r\n\n",
                   octets_rx_from_cpu_port, EMPTY, EMPTY, EMPTY);
        /*
         * If number of bytes on received packet is larger than the system-wide limit for
         * bypass-TDM packets then it will be forwarded to the 'packet' path and, therefore,
         * this test is expected to fail since 'is_tdm' will not be set.
         * Packet size is calculated by adding 'FTMH header size' to original size.
         * Maximal size, allowed to be sent by CPU, including PTCH, is 252 for Q2A
         * and 507 for J2C.
         *
         * See, also, Register ECI_FAP_GLOBAL_GENERAL_CFG_3, field PACKET_CRC_EN
         */
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) > max_packet_size)
        {
            expect_fail_on_is_tdm = TRUE;
        }
        if ((octets_rx_from_cpu_port + (ftmh_header_size - BYTES_IN_PTCH + BYTES_INTERNAL_CRC)) < min_packet_size)
        {
            expect_fail_on_is_tdm = TRUE;
        }
    }
    {
        /*
         * For debug/visibility: display the general TDM setup table and the relevant PEMLA Tables
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=PEMLA_TDM"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=PEMLA_OTMHFORMATS"));
    }
    {
        /*
         * For debug/visibility: display the general TDM IRE (ingress) setup tables
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_STREAM_CONFIGURATION"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONTEXT_CONFIG"));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_CONTEXT_MAP"));
    }
    /*
     * Force traffic from CPU to go to TDM source port
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port_1, src_port_1, 1));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, cpu_port_2, src_port_2, 1));
    {
        /*
         * We have loopback on source port. However, we do not want to see signals on
         * egress of source port since this is not part of the test.
         * So, disable visibility for egress of src_port to allow for tdm packet to be recorded
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          src_port_1, FALSE), "SRC Port Force set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          src_port_2, FALSE), "SRC Port Force set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           src_port_1, FALSE), "SRC Port Enable set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           src_port_2, FALSE), "SRC Port Enable set failed\n");
    }
    if (dst_present != FALSE)
    {
        /*
         * Enter here only if this is the 'destination is NOT multicast case'.
         *
         * This clause is NOT carried out on the 'destination is multicast' case
         * because the frames ejected from the destination port are multicast and some of them
         * return to the ejecting port and create an endless loop.
         *
         * Set 'destination port' to pass TDM packets to 'cpu port'. This is done because
         * we need a loopback on this port (to get a 'link up' signal for ILKN).
         */
        bcm_tdm_interface_config_t set_interface_config;
        bcm_tdm_ingress_editing_type_t type;
        int cpu_present;
        int do_not_override_context;
        int loc_stamp_mcid_with_sid;
        bcm_port_t *fabric_ports_p;
        int fabric_ports_count;

        fabric_ports_p = NULL;
        fabric_ports_count = -1;
        /*
         * Set 'base' to 'zero'. Whatever selected values will be OK.
         */
        set_interface_config.stream_id_base = 0;
        set_interface_config.stream_id_key_size = 1;
        set_interface_config.stream_id_key_offset = 0;
        set_interface_config.stream_id_enable = 0;
        type = bcmTdmIngressEditingPrepend;
        loc_stamp_mcid_with_sid = 0;
        /*
         * 'multicast_id' will not be used here so select whatever value.
         */
        /*
         * The channel id of the destination port was assigned above: channel_id_dst
         */
        /*
         * We assume 'cpu_port' was set, for this test.
         */
        cpu_present = TRUE;
        /*
         * Make sure to not use already used streams.
         */
        do_not_override_context = TRUE;
        channel_id_dst = 0;
        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, dst_port_1, channel_id_dst, cpu_present, cpu_port_1, type,
                         loc_stamp_mcid_with_sid, multicast_id_1, &set_interface_config, fabric_ports_p,
                         fabric_ports_count, &ingress_context_id_1));
        set_interface_config.stream_id_base = 1;
        SHR_IF_ERR_EXIT(dnx_tdm_ire_configure
                        (unit, do_not_override_context, dst_port_2, channel_id_dst, cpu_present, cpu_port_2, type,
                         loc_stamp_mcid_with_sid, multicast_id_2, &set_interface_config, fabric_ports_p,
                         fabric_ports_count, &ingress_context_id_2));
        /*
         * We have loopback on destination port. This is set only to get 'link up' on ILKN destination
         * port.
         * However, we do not want to see signals on ingress of destination port since this is not
         * part of the test.
         * So, disable visibility for ingress of dst_port and disable visibility for egress of
         * cpu_port to allow for tdm packet to be recorded,
         */
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                          dst_port_1, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                          dst_port_2, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                           dst_port_1, FALSE), "DST Port Enable set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                           dst_port_2, FALSE), "DST Port Enable set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          cpu_port_1, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                          cpu_port_2, FALSE), "DST Port Force visibility set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           cpu_port_1, FALSE), "DST Port Enable set failed\n");
        SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                           cpu_port_2, FALSE), "DST Port Enable set failed\n");
        /*
         * Close loopback on dst_port, so that 'link up' is set for it. TDM traffic injected into dst_port
         * is directed to CPU.
         */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, dst_port_1, BCM_PORT_LOOPBACK_MAC));
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, dst_port_2, BCM_PORT_LOOPBACK_MAC));
    }
    {
        /*
         * Close loopback on src_port, so that traffic injected by CPU to this port will enter as tdm
         */
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, src_port_1, BCM_PORT_LOOPBACK_MAC));
        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, src_port_2, BCM_PORT_LOOPBACK_MAC));
    }
    {
        /*
         * Set egress editing to dst_port, mcast_port[0] and mcast_port[1]
         * We are setting all the potential destination ports.
         * This setting is applicable only to ports participating in the test.
         */
        bcm_gport_t gport;
        bcm_tdm_egress_editing_t editing;
        uint32 flags;

        sal_memset(&editing, 0, sizeof(editing));
        flags = 0;
        editing.type = egress_edit_type;

        BCM_GPORT_LOCAL_SET(gport, dst_port_1);
        SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));
        BCM_GPORT_LOCAL_SET(gport, dst_port_2);
        SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));

        /*
         * The multicast ports are configured (with ucode_port SOC property) only when dst_present==FALSE
         * Therefore, we configure the multicast ports with egress editing only for "multicast flow" tests
         */
        if (dst_present == FALSE)
        {
            BCM_GPORT_LOCAL_SET(gport, mcast_port[0]);
            SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));

            BCM_GPORT_LOCAL_SET(gport, mcast_port[1]);
            SHR_IF_ERR_EXIT(bcm_tdm_egress_editing_set(unit, flags, gport, &editing));
        }
    }
    {
        /*
         * Make sure to take counters' values before starting the test.
         */
        octets_rx_from_src_port = 0;
        octets_tx_from_dst_port = 0;
        err_pkt_rx_from_src_port = 0;
        err_pkt_tx_from_dst_port = 0;

        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, TRUE, src_port_1, dst_port_1, &octets_rx_from_src_port,
                                                      &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                      &err_pkt_tx_from_dst_port));
        SHR_IF_ERR_EXIT(dnx_tdm_load_src_dst_counters(unit, TRUE, src_port_2, dst_port_2, &octets_rx_from_src_port,
                                                      &octets_tx_from_dst_port, &err_pkt_rx_from_src_port,
                                                      &err_pkt_tx_from_dst_port));
    }
    {
        /*
         * Disable OLP packets so we can follow only TDM packets. This is not compulsory. Just nice-to-have level.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "mod olp_dsp_event_route 0 15 dsp_event_route_line=0"));
    }
    {
        /*
         * Debug info. See link status. For ILKN, if link is 'down', traffic will not be sent out
         * of that link (port).
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "nif st"));
    }
    {
        /*
         * Debug info. Regarding multicast. See port cnfiguration. See bcm_dnx_tdm_egress_editing_set().
         * Note TRAP_CONTEXT_PORT_PROFILE regarding CUD.
         */
        SHR_IF_ERR_EXIT(sh_process_command
                        (unit, "dbal table dump table=EGRESS_PP_PORT clmn=TRAP_CONTEXT_PORT_PROFILE,PP_PORT "));
    }
    {
        /*
         * Do not remove:
         * Clear TDM counters. They are 'read-clear' counters
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=TDM_PACKET_COUNTER"));
    }
    {
        /*
         * Send Traffic to the src_port
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_load(unit, packet_n, packet_h, NULL), "");
        /**
         * get vid value when working in vid_stream_enable mode, this will be later used
         * as the expected stream id
         */
        if (vid_stream_enable)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_get_uint_field
                            (unit, packet_n, "ETH2", "Outer_VLAN.VID", NULL, &vid));
        }

        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_1, packet_h, SAND_PACKET_RESUME));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_2, packet_h, SAND_PACKET_RESUME));
        sal_msleep(1000);
    }
    {
        /*
         * Temporary. For debug. Display diagnostics counters.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "diag counter graphical coredisplay"));
    }
    {
        /*
         * Temporary. For debug. Display 'is_tdm' signal. For success, they should all be '1'
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=is_tdm"));
    }
    if (dst_present == FALSE)
    {
        /*
         * Temporary. For debug. Display 'multicast groups' table when multicast is tsted.
         */
        SHR_IF_ERR_EXIT(sh_process_command(unit, "dbal table dump table=MCDB"));
    }
    /*
     * Expected core is dependent on dst_port or MC ID
     * Do not go to exit on failing to detect expected signal, so as to allow for investigating
     * debug info.
     */
    {
        uint32 expected_value = 1;

        SHR_SET_CURRENT_ERR(sand_signal_verify
                            (unit, core_id_dst, "ERPP", "ERParser", NULL, "is_TDM", &expected_value, 1, NULL, NULL, 0));
    }
    if (SHR_FUNC_ERR())
    {
        if (expect_fail_on_is_tdm == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\r\n" "Signal 'is_TDM': sand_signal_verify() returned with error %s\r\n",
                         shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "Testing 'is_tdm' signal has failed, as expected. Test is, so far, successfull %s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
        }
    }
    else
    {
        if (expect_fail_on_is_tdm == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Signal 'is_TDM': sand_signal_verify() succeeded while, actually, failure was expected.\r\n");
        }
    }

    {
        /*
         * Verify the context count
         */
        int expect_count = 2;
        int expect_drop_count = 0;
        int context_count;
        int set_context_ens;

        SHR_IF_ERR_EXIT(dnx_tdm_packet_drop_validate(unit, expect_count, expect_drop_count));
        {
            /*
             * For debug.
             * Display 'ftmh_base' signal.
             * Display 'bytes_to_strip' signal. For 'optimized ftmh', this should be '4'
             * Display 'bytes_to_add' and 'header_to_add' signals. Their values correspond with the generated outgoing TDM Header
             */
            SHR_IF_ERR_EXIT(packet_ftmh_opt(unit, core_id_dst, NULL, &ftmh_opt_enabled));
            if (ftmh_opt_enabled)
            {
                SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get to=ETParser name=ftmh_opt"));
            }
            else
            {
                SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get to=ETParser name=ftmh_base"));
            }
            SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get name=bytes_to_strip"));
            SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get from=BTC name=bytes_to_add"));
            SHR_IF_ERR_EXIT(sh_process_command(unit, "sig get from=BTC name=header_to_add"));
        }
        {
            /*
             * Since the Bytes_to_Strip and headtype has be verified on dnx_tdm_bypass_test, the Bytes_to_Strip and header
             * check is ignored here
             */
        }
        /*
         * Display counters after transmit process.
         */
        LOG_CLI_EX("\r\n" "Source Port 1 %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", src_port_1);
        SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
        LOG_CLI_EX("\r\n" "Destination Port 1 %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", dst_port_1);
        SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));

        LOG_CLI_EX("\r\n" "Source Port 2 %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", src_port_2);
        SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
        LOG_CLI_EX("\r\n" "Destination Port 2 %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", dst_port_2);
        SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
        if (dst_present == FALSE)
        {
            int ii;
            for (ii = 0; ii < MAX_NUM_MCAST_PORTS; ii++)
            {
                LOG_CLI_EX("\r\n" "Multicast Port no. %d: Port %d %s%s\r\n", ii, mcast_port[ii], EMPTY, EMPTY);
                sal_snprintf(gen_string, RHNAME_MAX_SIZE, "show counter full port=%d", mcast_port[ii]);
                SHR_IF_ERR_EXIT(sh_process_command(unit, gen_string));
            }
        }

        SHR_IF_ERR_EXIT(dnx_tdm_port_counter_validate
                        (unit, src_port_1, dst_port_1, expect_fail_on_is_tdm, egress_edit_type, ftmh_header_size));
        SHR_IF_ERR_EXIT(dnx_tdm_port_counter_validate
                        (unit, src_port_2, dst_port_2, expect_fail_on_is_tdm, egress_edit_type, ftmh_header_size));
        context_count = 1;
        set_context_ens = 1;
        {
            /*
             * Enable failover on ingress_context_1, send packet to check the drop
             */
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_set
                            (unit, 0, context_count, &ingress_context_id_1, &set_context_ens));

            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_1, packet_h, SAND_PACKET_RESUME));
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_2, packet_h, SAND_PACKET_RESUME));
            sal_msleep(1000);
        }
        expect_count = 2;
        expect_drop_count = 1;

        SHR_IF_ERR_EXIT(dnx_tdm_packet_drop_validate(unit, expect_count, expect_drop_count));
        SHR_IF_ERR_EXIT(sh_process_command(unit, "clear cou"));
        {
            /*
             * Enable failover on ingress_context_2, send packet to check the drop
             */
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_set
                            (unit, 0, context_count, &ingress_context_id_2, &set_context_ens));

            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_1, packet_h, SAND_PACKET_RESUME));
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_2, packet_h, SAND_PACKET_RESUME));
            sal_msleep(1000);
        }
        expect_drop_count = 2;
        SHR_IF_ERR_EXIT(dnx_tdm_packet_drop_validate(unit, expect_count, expect_drop_count));
        {
            set_context_ens = 0;
            /*
             * Enable failover on ingress_context_2, send packet to check the drop
             */
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_set
                            (unit, 0, context_count, &ingress_context_id_1, &set_context_ens));
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_failover_set
                            (unit, 0, context_count, &ingress_context_id_2, &set_context_ens));

            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_1, packet_h, SAND_PACKET_RESUME));
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, cpu_port_2, packet_h, SAND_PACKET_RESUME));
            sal_msleep(1000);
        }
        expect_drop_count = 0;
        SHR_IF_ERR_EXIT(dnx_tdm_packet_drop_validate(unit, expect_count, expect_drop_count));
    }
exit:
    diag_sand_packet_free(unit, packet_h);
    SH_SAND_GET_BOOL("clean", to_clean);
    if (to_clean == TRUE)
    {
        /*
         * Clean calls does not modify test return status
         */
        shr_error_e rv;

        rv = ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI_EX("\r\n"
                       "dnx_tdm_bypass_test_cmd(): ctest_dnxc_restore_soc_properties() has failed on 'clean' process with error %d (%s) %s%s\r\n\n",
                       rv, _SHR_ERRMSG(rv), EMPTY, EMPTY);
        }
    }
    if (SHR_FUNC_ERR())
    {
        bcm_pbmp_t logical_ports;

        SH_SAND_GET_ENUM("if_type", if_type);
        SH_SAND_GET_BOOL("clean", to_clean);
        SH_SAND_GET_PORT("src_port_1", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, src_port_1);
        SH_SAND_GET_PORT("src_port_2", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, src_port_2);

        LOG_CLI_EX("\r\n"
                   "Test '%s' has FAILED with the following parameters:\r\n"
                   "if_type %s%s%s\r\n", test_name, if_type ? "IF_TDM_HYBRID" : "IF_TDM_ONLY", EMPTY, EMPTY);

        LOG_CLI_EX("==> cpu_port_1 %d, cpu_port_2 %d, Packet_name %s, clean %d\r\n", cpu_port_1, cpu_port_2, packet_n,
                   to_clean);
        LOG_CLI_EX("==> base %d, bits_in_key %d, offset %d stream_enable %d\r\n", stream_id_base, stream_id_key_size,
                   stream_id_key_offset, stream_id_enable);
        SH_SAND_GET_PORT("dst_port_1", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, dst_port_1);
        LOG_CLI_EX("==> src_port_1 %d, dst_port_1 %d, multicast %d context_type %d\r\n",
                   src_port_1, dst_port_1, multicast_id_1, context_type);

        SH_SAND_GET_PORT("dst_port_2", logical_ports);
        _SHR_PBMP_FIRST(logical_ports, dst_port_2);
        LOG_CLI_EX("==> src_port_2 %d, dst_port_2 %d, multicast %d context_type %d\r\n",
                   src_port_2, dst_port_2, multicast_id_2, context_type);
        LOG_CLI_EX("==> stamp %d min_cell_size %d max_cell_size %d %s\r\n", stamp_mcid_with_sid,
                   min_cell_size, max_cell_size, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;

}
/** List of vlan tests   */
/* *INDENT-OFF* */
sh_sand_cmd_t dnx_tdm_test_cmds[] = {
    {"context",     dnx_tdm_context_test_cmd, NULL, dnx_tdm_context_test_arguments, &dnx_tdm_context_test_man, NULL,
                    dnx_tdm_context_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"failover",    dnx_tdm_failover_test_cmd, NULL, dnx_tdm_failover_test_arguments, &dnx_tdm_failover_test_man, NULL,
                    dnx_tdm_failover_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"failover_drop",    dnx_tdm_failover_drop_test_cmd, NULL, dnx_tdm_failover_drop_test_arguments, &dnx_tdm_failover_drop_test_man, NULL,
                    dnx_tdm_failover_drop_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"interface",   dnx_tdm_interface_test_cmd, NULL, dnx_tdm_interface_test_arguments, &dnx_tdm_interface_test_man, NULL,
                    dnx_tdm_interface_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"stream",      dnx_tdm_stream_test_cmd, NULL, dnx_tdm_stream_test_arguments, &dnx_tdm_stream_test_man, NULL,
                    dnx_tdm_stream_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"bypass_mode", dnx_tdm_bypass_test_cmd, NULL, dnx_tdm_bypass_test_arguments, &dnx_tdm_bypass_test_man, NULL,
                    dnx_tdm_bypass_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"packet_mode", dnx_tdm_packet_test_cmd, NULL, dnx_tdm_packet_test_arguments, &dnx_tdm_packet_test_man, NULL,
                    dnx_tdm_packet_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"fabric",      dnx_tdm_fabric_test_cmd, NULL, dnx_tdm_fabric_test_arguments, &dnx_tdm_fabric_test_man, NULL,
                    dnx_tdm_fabric_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_fabric_support_check},
    {"hybrid",      dnx_tdm_hybrid_test_cmd, NULL, dnx_tdm_hybrid_test_arguments, &dnx_tdm_hybrid_test_man, NULL,
                    dnx_tdm_hybrid_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {"external_ftmh",   dnx_tdm_external_ftmh_test_cmd, NULL, dnx_tdm_external_ftmh_test_arguments, &dnx_tdm_external_ftmh_test_man, NULL,
                    dnx_tdm_external_ftmh_tests, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_tdm_bypass_support_check},
    {NULL}
};

