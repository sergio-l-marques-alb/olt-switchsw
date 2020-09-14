/** \file ctest_dnx_l3_sllb.c
 * $Id$
 *
 * Tests for L3 ECMP SLLB functionality
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <bcm_int/dnx/l3/l3_ecmp_vip.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_visibility.h>

/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#undef BSL_LOG_MODULE
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#define CTEST_SLLB_BASIC_NOF_SERVERS            (4)
#define CTEST_SLLB_BASIC_PKT_COUNT              (16)
#define CTEST_NUM_PCC_TABLES                    (2)
#define CTEST_NUM_MATCH_FLAGS_COMBO             (8)

#define CTEST_SLLB_MIN_VIP_ECMP_TABLE_SIZE      (0)

/*
 * \brief The following enumeration represent different test cases of the VIP ECMP forwarding program selection based on L4 type.
 * The SW behaves different in each one of the following cases.
 */
typedef enum
{
    /* Test case where packet is UDPoIPvX */
    CTEST_SLLB_L4_TYPE_UDP, 
    /* Test case where packet is TCPoIPvX */
    CTEST_SLLB_L4_TYPE_TCP,
    /* Test case where packet is UDPoIPvX */
    CTEST_SLLB_L4_TYPE_NONE,
    /* NOF test cases */
    CTEST_SLLB_NOF_L4_TYPES
} ctest_sllb_test_l4_type_e;

/*
 * \brief The following enumeration represent different test cases of the VIP ECMP forwarding program selection based on L3 type.
 * The SW behaves different in each one of the following cases.
 */
typedef enum
{
    /* Test case where packet is AnyoIpv4 */
    CTEST_SLLB_L3_TYPE_IPV4,
    /* Test case where packet is AnyoIpv6 */
    CTEST_SLLB_L3_TYPE_IPV6,
    /* NOF test cases */
    CTEST_SLLB_NOF_L3_TYPES
} ctest_sllb_test_l3_type_e;


/*
 * }
 */

/*
 * Enumeration
 * {
 */
/*
 * }
 */

/*
 * Structures
 * {
 */
/*
 * }
 */

/*
 * Externs
 * {
 */
/*
 * }
 */
/*
 * Globals.
 * {
 */

/** SLLB test details */
static sh_sand_man_t sh_dnx_l3_sllb_basic_man = {
    "Basic functional test."
};

static sh_sand_option_t sh_dnx_l3_sllb_basic_options[] = {
    {NULL}
};

static sh_sand_man_t sh_dnx_l3_sllb_sem_man = {
    "Semantic test."
};

static sh_sand_option_t sh_dnx_l3_sllb_sem_options[] = {
    {NULL}
};

/*
 * }
 */

/**
 * \brief
 *   Verify the UDH is stamped with expected VIP_ID, server FEC_ID, and SIGNATURE
 *   UDH2 = {server FEC_ID(20), VIP_ID(12)}
 *   UDH3 = SIGNATURE(32) = {ECMP-Lb-Key[2](16), Nwk-Lb-Key(16)}
 * \param [in] unit - The unit number.
 * \param [in] core - core from which to retrieve signals
 * \param [in] vip_id - The expected VIP_ID
 * \param [in] result_fec_id - The expected server FEC_ID
 * \param [in] result_ecmp_lb_key - The expected SIGNATURE 16 msb
 * \param [in] result_nwk_lb_key - The expected SIGNATURE 16 lsb
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_basic_udh_verify(
    int unit,
    int core,
    uint32 vip_id,
    uint32 result_fec_id,
    uint32 result_ecmp_lb_key,
    uint32 result_nwk_lb_key)
{
    uint32 udh_base;
    uint32 udh2;
    uint32 udh3;
    uint32 udh_vip_id;
    uint32 udh_server_fec_id;
    uint32 udh_ecmp3_lb_key;
    uint32 udh_nwk_lb_key;
    char return_value_str[DSIG_MAX_SIZE_STR];

    SHR_FUNC_INIT_VARS(unit);

    /* In the cint example, UDH2 and UDH3 are enabled for PMF data. UDH0 and UD1 are not enabled */
    udh_base = 0x5;
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
        (unit, core, "IRPP", "LBP", "ITMR", "UDH_Base", (uint32 *) &udh_base, 1, NULL, return_value_str, 0),
        "Test failed , UDH_Base from LBP core %d expecting %d return value %s\n", core, udh_base, return_value_str);

    /* Retrieve UHD_2 */
    udh2 = 0;
    sand_signal_verify(unit, core, "IRPP", "LBP", "ITMR", "UDH_DATA_2", (uint32 *) &udh2, 4, NULL, return_value_str, 0);
    sscanf(return_value_str, "%x", &udh2);

    /* Retrieve UHD_3 */
    udh3 = 0;
    sand_signal_verify(unit, core, "IRPP", "LBP", "ITMR", "UDH_DATA_3", (uint32 *) &udh3, 4, NULL, return_value_str, 0);
    sscanf(return_value_str, "%x", &udh3);

    /* Verify the VIP_ID */
    udh_vip_id = udh2 & 0xfff;
    if (udh_vip_id != vip_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , VIP_ID stamped on UDH2 (%d) does not match expected VIP_ID (%d)\n", udh_vip_id, vip_id);
    }

    /* Verify the server FEC_ID */
    udh_server_fec_id = udh2 >> 12;
    if (udh_server_fec_id != result_fec_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , Server FEC_ID stamped on UDH2 (%d) does not match expected Server FEC_ID (%d)\n", udh_server_fec_id, result_fec_id);
    }

    /* Verify the SIGNATURE */
    udh_ecmp3_lb_key = udh3 >> 16;
    if (udh_ecmp3_lb_key != result_ecmp_lb_key)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , ECMP3-Lb-Key stamped on UDH2 (%d) does not match expected ECMP3-Lb-Key (%d)\n", udh_ecmp3_lb_key, result_ecmp_lb_key);
    }

    udh_nwk_lb_key = udh3 & 0xffff;
    if (udh_nwk_lb_key != result_nwk_lb_key)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , NWK-LB-Key stamped on UDH2 (%d) does not match expected NWK-LB-Key (%d)\n", udh_nwk_lb_key, result_nwk_lb_key);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify that packets perform basic routing when result is a non-VIP FEC_ID.
 *   This test verfies that the packet destination after FWD2 is the basic route FEC_ID.
 * \param [in] unit - The unit number.
 * \param [in] core - Ingress core
 * \param [in] pp_port - IN_PP_PORT
 * \param [in] tm_port - IN_TM_PORT
 * \param [in] basic_route_fec_id - The expected fec_id result from FWD2
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_basic_direct_route_test(
        int unit,
        int core,
        uint32 pp_port,
        uint32 tm_port,
        uint32 basic_route_fec_id)
{
    rhhandle_t packet_h = NULL;
    char *dmac_str="00:11:22:33:44:55";
    char *smac_str="00:66:77:88:99:aa";
    uint32 vlan = 100;
    char dip[RHNAME_MAX_SIZE] = "127.255.255.33";
    char sip[RHNAME_MAX_SIZE] = "10.10.10.0";
    char dip6[RHNAME_MAX_SIZE] = "4444:0:0:0:0:0:0:0";
    char sip6[RHNAME_MAX_SIZE] = "1111:0:0:0:0:0:0:0";
    uint32 in_ttl = 80;
    uint32 proto = 17;
    uint32 src_port = 0x1000;
    uint32 dst_port = 0x2000;
    ctest_sllb_test_l3_type_e l3_test_type;
    ctest_sllb_test_l4_type_e l4_test_type;
    char return_value_str[DSIG_MAX_SIZE_STR];
    uint32 result_fec_id;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META(" \n=> Testing direct routing \n")));
    /* 
     * 2 non-VIP routes are installed in KAPS 
     *  IPV4: 127:255:255:32 VRF 999
     *  IPV6: 68:68:0:0:0:0:0:0 VRF 999
     * Test cases:
     * -- Packet format is UDPoIPv4, FWD1 context is SLLB___IPV4_ROUTE, packet destination is non-VIP FEC_ID
     * -- Packet format is TCPoIPv4, FWD1 context is SLLB___IPV4_ROUTE, packet destination is non-VIP FEC_ID
     * -- Packet format is ICMPoIPv4, FWD1 context IPV4___PRIVATE_UC, FWD1 result is a miss - packet destination is drop destination, FWD2 context is IPV4___PRIVATE_UC, FWD2 result is non-VIP FEC_ID
     * -- Packet format is UDPoIPv6, FWD1 context is SLLB___IPV6_ROUTE, packet destination is non-VIP FEC_ID
     * -- Packet format is TCPoIPv6, FWD1 context is SLLB___IPV6_ROUTE, packet destination is non-VIP FEC_ID
     * -- Packet format is ICMPoIPv6, FWD1 context IPV6___PRIVATE_UC, FWD1 result is a miss - packet destination is drop destination, FWD2 context is IPV6___PRIVATE_UC, FWD2 result is non-VIP FEC_ID
     */
    for (l3_test_type = CTEST_SLLB_L3_TYPE_IPV4; l3_test_type <= CTEST_SLLB_L3_TYPE_IPV6; l3_test_type++)
    {
        /* Build packet */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
        if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", &in_ttl, 8));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.DIP", dip));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.SIP", sip));
        }
        else
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv6"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.HOP_LIMIT", &in_ttl, 8));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv6.DIP", dip6));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv6.SIP", sip6));
        }

        for (l4_test_type = CTEST_SLLB_L4_TYPE_UDP; l4_test_type <= CTEST_SLLB_L4_TYPE_NONE; l4_test_type++)
        {
            if (l4_test_type == CTEST_SLLB_L4_TYPE_UDP)
            {
                proto = 17;
                if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
                {
                    LOG_CLI((BSL_META(" ===> Testing UDPoIPv4 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.PROTOCOL", &proto, 8));
                }
                else
                {
                    LOG_CLI((BSL_META(" ===> Testing UDPoIPv6 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.NEXT_HEADER", &proto, 8));
                }
                SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "UDP"));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "UDP.SRC_PORT", &src_port, 16));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "UDP.DST_PORT", &dst_port, 16));
            }
            else if (l4_test_type == CTEST_SLLB_L4_TYPE_TCP)
            {
                proto = 6;
                if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
                {
                    LOG_CLI((BSL_META(" ===> Testing TCPoIPv4 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.PROTOCOL", &proto, 8));
                }
                else
                {
                    LOG_CLI((BSL_META(" ===> Testing TCPoIPv6 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.NEXT_HEADER", &proto, 8));
                }
                SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "TCP"));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "TCP.SRC_PORT", &src_port, 16));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "TCP.DST_PORT", &dst_port, 16));
            }
            else /* CTEST_SLLB_L4_TYPE_NONE */
            {
                proto = 1;  /* ICMP */
                if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
                {
                    LOG_CLI((BSL_META(" ===> Testing IPv4.ICMP \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.PROTOCOL", &proto, 8));
                }
                else
                {
                    LOG_CLI((BSL_META(" ===> Testing IPv6.ICMP \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.NEXT_HEADER", &proto, 8));
                }
            }
            /*
             * Send packet
             */
            dnx_visibility_resume(unit, BCM_CORE_ALL,
                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

            /* Verify the FWD1 result */
            if (l4_test_type == CTEST_SLLB_L4_TYPE_NONE)
            {
                result_fec_id = 0x1fffff;   /* default destination */
            }
            else
            {
                result_fec_id = basic_route_fec_id;
            }
            SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                    (unit, core, "IRPP", "FWD1", "FWD2", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 2, NULL, return_value_str, 0),
                    "Test failed , Fwd_Action_Dst from FWD1 core %d expecting %d return value %s\n", core, result_fec_id, return_value_str);

            /* Verify the FWD2 result */
            result_fec_id = basic_route_fec_id;
            SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                    (unit, core, "IRPP", "FWD2", "IPMF1", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 2, NULL, return_value_str, 0),
                    "Test failed , Fwd_Action_Dst from FWD2 core %d expecting %d return value %s\n", core, result_fec_id, return_value_str);
        }
        /* Free test packet buffer */
        diag_sand_packet_free(unit, packet_h);
        packet_h = NULL;
    }

exit:
    /* Free test packet buffer */
    diag_sand_packet_free(unit, packet_h);

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify that packets which are  L4oIPvX perform SLLB VIP routing
 * \param [in] unit - The unit number.
 * \param [in] core - Ingress core
 * \param [in] pp_port - IN_PP_PORT
 * \param [in] tm_port - IN_TM_PORT
 * \param [in] server_start_fec_id - The starting server FEC_ID in the group of servers configured in the VIP ECMP table.
 * \param [in] sllb_table_size - The number of entries in the VIP ECPM consistent hash table
 * \param [in] kbp_pcc_enabled - Indicates if external KBP is enabled for SLLB
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_basic_vip_route_test(
        int unit,
        int core,
        uint32 pp_port,
        uint32 tm_port,
        uint32 vip_id,
        uint32 server_start_fec_id,
        int sllb_table_size,
        int kbp_pcc_enabled)
{
    ctest_cint_argument_t cint_arguments[12];
    rhhandle_t packet_h = NULL;
    char *dmac_str="00:11:22:33:44:55";
    char *smac_str="00:66:77:88:99:aa";
    uint32 vlan = 100;
    char dip[RHNAME_MAX_SIZE] = "127.255.255.17";
    char sip[RHNAME_MAX_SIZE] = "10.10.10.0";
    char dip6[RHNAME_MAX_SIZE] = "3333:0:0:0:0:0:0:0";
    char sip6[RHNAME_MAX_SIZE] = "1111:0:0:0:0:0:0:0";
    uint32 in_ttl = 80;
    uint32 proto = 17;
    uint32 src_port = 0x1000;
    uint32 dst_port = 0x2000;
    int i_pkt;
    uint32 result_fec_id;
    uint32 result_ecmp_lb_key;
    uint32 result_nwk_lb_key;
    uint32 result_lag_lb_key;
    uint32 result_snoop_qual;
    char return_value_str[DSIG_MAX_SIZE_STR];
    uint32 server_fec_id[CTEST_SLLB_BASIC_NOF_SERVERS];
    int server_fec_hit_count[CTEST_SLLB_BASIC_NOF_SERVERS] = {0, 0, 0, 0};
    uint32 result_server_fec_id;
    int i_server;
    int found_server;
    bcm_l3_egress_ecmp_resilient_entry_t entry;
    uint32 pcc_fec_id;
    uint32 kbp_pcc_fec_id;
    uint32 signature = 0;
    int tcam_entry_priority = 10;
    uint32 member_reference;
    ctest_sllb_test_l3_type_e l3_test_type;
    ctest_sllb_test_l4_type_e l4_test_type;
    int vip_ecmp_fec_id_min;
    int vip_ecmp_fec_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META(" => Testing VIP routing \n")));
    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchVIPL3EgressIDMin, &vip_ecmp_fec_id_min));

    /* 
     * Save the VIP ECPM FEC Id configured by sllb_basic_example
     */
    vip_ecmp_fec_id = vip_ecmp_fec_id_min + vip_id;

    /*
     * Save the list of servers in the ECPM group for validation later
     */
    for (i_server = 0; i_server < CTEST_SLLB_BASIC_NOF_SERVERS; i_server++)
    {
        server_fec_id[i_server] = server_start_fec_id + i_server;
    }
    /* 
     * 2 non-VIP routes are installed in KAPS 
     *  IPV4: 127:255:255:32 VRF 999
     *  IPV6: 68:68:0:0:0:0:0:0 VRF 999
     * Test cases:
     * -- Packet format is UDPoIPv4
     * -- Packet format is TCPoIPv4
     * -- Packet format is UDPoIPv6, FWD1 context is SLLB___IPV6_ROUTE, packet destination is non-VIP FEC_ID
     * -- Packet format is TCPoIPv6, FWD1 context is SLLB___IPV6_ROUTE, packet destination is non-VIP FEC_ID
     * For each test case verify
     * 1. FWD1 result is the VIP FEC_ID
     * 2. ECMP resolution - packets distributed to multiple servers in the VIP ECMP group
     * 3. Verify the packet is snooped for VIP_ID and server FEC_ID accessed. i
     * 4. Verify the UDH contains correct VIP_ID, server FEC_ID, and SIGNATURE.
     * 5. Verify the packet is snooped for VIP_ID and server FEC_ID accessed. i
     * 6. Verify a hit in the LEM PCC table takes priority over the KBP PCC table (if enabled) and the VIP ECMP result.
     * 7. If the LEM PCC entry does not exist, verify the KBP PCC table (if enabled) takes priority over the VIP ECMP result.
     */
    for (l3_test_type = CTEST_SLLB_L3_TYPE_IPV4; l3_test_type <= CTEST_SLLB_L3_TYPE_IPV6; l3_test_type++)
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
        if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", &in_ttl, 8));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.DIP", dip));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv4.SIP", sip));
        }
        else
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv6"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.HOP_LIMIT", &in_ttl, 8));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv6.DIP", dip6));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "IPv6.SIP", sip6));
        }

        for (l4_test_type = CTEST_SLLB_L4_TYPE_UDP; l4_test_type <= CTEST_SLLB_L4_TYPE_TCP; l4_test_type++)
        {
            if (l4_test_type == CTEST_SLLB_L4_TYPE_UDP)
            {
                proto = 17;
                if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
                {
                    LOG_CLI((BSL_META(" \n===> Testing UDPoIPv4 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.PROTOCOL", &proto, 8));
                }
                else
                {
                    LOG_CLI((BSL_META(" \n===> Testing UDPoIPv6 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.NEXT_HEADER", &proto, 8));
                }
                SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "UDP"));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "UDP.SRC_PORT", &src_port, 16));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "UDP.DST_PORT", &dst_port, 16));
            }
            else
            {
                proto = 6;
                if (l3_test_type == CTEST_SLLB_L3_TYPE_IPV4)
                {
                    LOG_CLI((BSL_META(" \n===> Testing TCPoIPv4 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.PROTOCOL", &proto, 8));
                }
                else
                {
                    LOG_CLI((BSL_META(" \n===> Testing TCPoIPv6 \n")));
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv6.NEXT_HEADER", &proto, 8));
                }
                SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "TCP"));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "TCP.SRC_PORT", &src_port, 16));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "TCP.DST_PORT", &dst_port, 16));
            }
            /*
             * Verify the VIP FEC ID is the FWD1 result
             */

            dnx_visibility_resume(unit, BCM_CORE_ALL,
                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

            /* Verify the FWD1 result is the VIP ECMP FEC-Id */
            result_fec_id = vip_ecmp_fec_id;
            SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                    (unit, core, "IRPP", "FWD1", "FWD2", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 2, NULL, return_value_str, 0), 
                    "Test failed , Fwd_Action_Dst from FWD1 core %d expecting %d return value %s\n", core, result_fec_id, return_value_str);

            /*
             * Verify the packets are being distributed to muliple servers 
             */
            for (i_server = 0; i_server < CTEST_SLLB_BASIC_NOF_SERVERS; i_server++)
            {
                /* Reset the hit count between test iterations */
                server_fec_hit_count[i_server] = 0;
            }
            for (i_pkt = 0; i_pkt < CTEST_SLLB_BASIC_PKT_COUNT; i_pkt++, dst_port++)
            {
                if (l4_test_type == CTEST_SLLB_L4_TYPE_UDP)
                {
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "UDP.DST_PORT", &dst_port, 16));
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "TCP.DST_PORT", &dst_port, 16));
                }

                dnx_visibility_resume(unit, BCM_CORE_ALL,
                        BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
                SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

                /* Verify the FWD2 result is one of the server FEC IDs */
                result_fec_id = 0; /* Dummy -- we don't know which server was accessed */
                sand_signal_verify(unit, core, "IRPP", "FWD2", "IPMF1", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 4, NULL, return_value_str, 0);
                sscanf(return_value_str, "%x", &result_server_fec_id);

                for (found_server = 0, i_server = 0; i_server < CTEST_SLLB_BASIC_NOF_SERVERS; i_server++)
                {
                    if (result_server_fec_id == server_fec_id[i_server])
                    {
                        found_server++;
                        server_fec_hit_count[i_server]++;
                    }
                }
                if (found_server == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , FWD2 result was not a matching server FEC ID\n");
                }
            }

            
            if (!CTEST_SLLB_L4_TYPE_TCP)
            {
                /* Verify that more than one of the server FEC IDs was accessed */
                for (found_server = 0, i_server = 0; i_server < CTEST_SLLB_BASIC_NOF_SERVERS; i_server++)
                {
                    if (server_fec_hit_count[i_server] > 0)
                    {
                        found_server++;
                    }
                }
                if (found_server < 2)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , Packets were not distributed\n");
                }
            }

            /* Send one more packet and find the ECMP member server fec to which it resolved */
            dnx_visibility_resume(unit, BCM_CORE_ALL,
                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

            result_fec_id = 0; /* Dummy -- we don't know which server was accessed */
            sand_signal_verify(unit, core, "IRPP", "FWD2", "IPMF1", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 4, NULL, return_value_str, 0);
            sscanf(return_value_str, "%x", &result_server_fec_id);

            for (found_server = 0, i_server = 0; i_server < CTEST_SLLB_BASIC_NOF_SERVERS; i_server++)
            {
                if (result_server_fec_id == server_fec_id[i_server])
                {
                    found_server = 1;
                    pcc_fec_id = server_fec_id[(i_server+1)%4];
                    kbp_pcc_fec_id = server_fec_id[(i_server+2)%4];
                    break;
                }
            }
            if (found_server == 0)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , FWD2 result was not a matching server FEC ID\n");
            }

            /* Construct the signature from the LB signals */
            result_ecmp_lb_key = 0; /* Dummy value  - we don't know the expected value */
            sand_signal_verify(unit, core, "IRPP", "FWD1", "FWD2", "ECMP_LB_Keys.Hierarchy_3", (uint32 *) &result_ecmp_lb_key, 2, NULL, return_value_str, 0);
            sscanf(return_value_str, "%x", &result_ecmp_lb_key);
            result_nwk_lb_key = 0; /* Dummy value  - we don't know the expected value */
            sand_signal_verify(unit, core, "IRPP", "FWD1", "FWD2", "Nwk_LB_Key", (uint32 *) &result_nwk_lb_key, 2, NULL, return_value_str, 0);
            sscanf(return_value_str, "%x", &result_nwk_lb_key);
            signature = (result_ecmp_lb_key << 16) | result_nwk_lb_key;

            /* Calculate the member_reference */
            result_lag_lb_key = 0; /* Dummy value  - we don't know the expected value */
            sand_signal_verify(unit, core, "IRPP", "FWD1", "FWD2", "LAG_LB_Key", (uint32 *) &result_lag_lb_key, 2, NULL, return_value_str, 0);
            sscanf(return_value_str, "%x", &result_lag_lb_key);
            member_reference = (result_lag_lb_key * sllb_table_size) >> 16;

            /*
             * Verify snooping based on SLLB Qualifiers:
             * Snoop packet if VIP_VALID = 1, VIP_ID = vip_ecmp_fec_id, PCC_HIT = 0, Fwd_Action_Dst = result_server_fec_id
             */

            cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
            cint_arguments[0].value.value_uint32 = tcam_entry_priority++;
            cint_arguments[1].type = SAL_FIELD_TYPE_UINT32;
            cint_arguments[1].value.value_uint32 = vip_id;
            cint_arguments[2].type = SAL_FIELD_TYPE_UINT32;
            cint_arguments[2].value.value_uint32 = result_server_fec_id;
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "sllb_server_fec_tcam_entry_add", cint_arguments, 3),
                    "sllb_server_fec_tcam_entry_add Run Failed\n");

            /* Send the same packet */
            dnx_visibility_resume(unit, BCM_CORE_ALL,
                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

            /* Verify the snoop qualifier */
            result_snoop_qual = 0x44;
            SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                    (unit, core, "IRPP", "IPMF1", "FER", "Snoop_Qualifier", (uint32 *) &result_snoop_qual, 1, NULL, return_value_str, 0),
                    "Test failed , Snoop_Qualifier from IPMF1 core %d expecting %d return value %s\n", core, result_snoop_qual, return_value_str);

            /* 
             * Verify UDH:
             * UDH2: Server FEC ID = Fwd_Action_Dst[19:0], VIP_ID(12)
             * UDH3: SIGNATURE(32) = Nwk-Lb-Key(16), ECMP-Lb-Key2(16)
             */
            SHR_IF_ERR_EXIT(dnx_l3_sllb_basic_udh_verify(unit, core, vip_id, result_server_fec_id, result_ecmp_lb_key, result_nwk_lb_key));

            /*
             * Remove FG TCAM entry
             */
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "sllb_server_fec_tcam_entry_remove", cint_arguments, 0),
                    "sllb_server_fec_tcam_entry_remove Run Failed\n");

            /*
             * Verify snooping based on SLLB Qualifiers:
             * Snoop packet if VIP_VALID = 1, VIP_ID = vip_ecmp_fec_id, PCC_HIT = 0, member_reference = ECMP member index
             */
            cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
            cint_arguments[0].value.value_uint32 = tcam_entry_priority++;
            cint_arguments[1].type = SAL_FIELD_TYPE_UINT32;
            cint_arguments[1].value.value_uint32 = vip_id;
            cint_arguments[2].type = SAL_FIELD_TYPE_UINT32;
            cint_arguments[2].value.value_uint32 = member_reference;
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "sllb_member_reference_tcam_entry_add", cint_arguments, 3),
                    "sllb_member_reference_tcam_entry_add Run Failed\n");

            /* Send the same packet */
            dnx_visibility_resume(unit, BCM_CORE_ALL,
                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

            /* Verify the snoop qualifier */
            result_snoop_qual = 0x33;
            SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                    (unit, core, "IRPP", "IPMF1", "FER", "Snoop_Qualifier", (uint32 *) &result_snoop_qual, 1, NULL, return_value_str, 0),
                    "Test failed , Snoop_Qualifier from IPMF1 core %d expecting %d return value %s\n", core, result_snoop_qual, return_value_str);

            /*
             * Remove FG TCAM entry
             */
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "sllb_member_reference_tcam_entry_remove", cint_arguments, 0),
                    "sllb_member_reference_tcam_entry_remove Run Failed\n");

            /*
             * Verify that the LEM PCC result takes priority over the ECMP result
             */

            /* Add a PCC entry in LEM to redirect the flow to a different server */
            sal_memset(&entry, 0, sizeof(entry));
            BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
            COMPILER_64_ZERO(entry.hash_key);
            COMPILER_64_SET(entry.hash_key, 0x0, signature);
            BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id);
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_add(unit, BCM_L3_ECMP_RESILIENT_PCC, &entry));

            /* Send the same packet. Verify that is was forwarded it to the server fec in the PCC entry */
            dnx_visibility_resume(unit, BCM_CORE_ALL,
                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            /* Send the same packet */
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

            result_fec_id = 0; /* Dummy - we don't know the expected server fec id */
            sand_signal_verify(unit, core, "IRPP", "FWD2", "IPMF1", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 4, NULL, return_value_str, 0);
            sscanf(return_value_str, "%x", &result_server_fec_id);

            if (result_server_fec_id != pcc_fec_id)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , FWD2 result 0x%08x did not match the PCC State table entry FEC ID 0x%08x\n", result_server_fec_id, pcc_fec_id);
            }

            /*
             * Verify snooping based on SLLB Qualifiers:
             * Snoop packet if VIP_VALID = 1, PCC_HIT = 1, L4=TCP, TCP FLAGS.FIN=1
             */

            
            if (0 /* l4_test_type == CTEST_SLLB_L4_TYPE_TCP*/)
            {
                /* Verify the snoop qualifier */
                result_snoop_qual = 0x11;
                SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, core, "IRPP", "IPMF1", "FER", "Snoop_Qualifier", (uint32 *) &result_snoop_qual, 1, NULL, return_value_str, 0),
                        "Test failed , Snoop_Qualifier from IPMF1 core %d expecting %d return value %s\n", core, result_snoop_qual, return_value_str);
            }

            /* 
             * Verify that the KBP PCC result takes priority over the LEM PCC result 
             */
            if (kbp_pcc_enabled)
            {
                /* Add a PCC entry in KBP to redirect the flow to a different server */
                sal_memset(&entry, 0, sizeof(entry));
                BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
                COMPILER_64_ZERO(entry.hash_key);
                COMPILER_64_SET(entry.hash_key, 0x0, signature);
                BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, kbp_pcc_fec_id);
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_add(unit, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL, &entry));

                /* Send the same packet. Verify that is was forwarded it to the server fec in the PCC entry */
                dnx_visibility_resume(unit, BCM_CORE_ALL,
                        BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
                SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, tm_port, packet_h, 0));

                result_fec_id = 0; /* Dummy */
                sand_signal_verify(unit, core, "IRPP", "FWD2", "IPMF1", "Fwd_Action_Dst", (uint32 *) &result_fec_id, 4, NULL, return_value_str, 0);
                sscanf(return_value_str, "%x", &result_server_fec_id);

                if (result_server_fec_id != kbp_pcc_fec_id)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed , FWD2 result 0x%08x did not match the KPB PCC State table entry FEC ID 0x%08x\n", result_server_fec_id, pcc_fec_id);
                }
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_delete(unit, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL, &entry));
            }

            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_delete(unit, BCM_L3_ECMP_RESILIENT_PCC, &entry));

        }
        /* Free test packet buffer */
        diag_sand_packet_free(unit, packet_h);
        packet_h = NULL;
    }
exit:
    sal_memset(&entry, 0, sizeof(entry));
    BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
    COMPILER_64_ZERO(entry.hash_key);
    COMPILER_64_SET(entry.hash_key, 0x0, signature);
    bcm_l3_egress_ecmp_resilient_delete(unit, BCM_L3_ECMP_RESILIENT_PCC, &entry);

    if (kbp_pcc_enabled)
    {
        bcm_l3_egress_ecmp_resilient_delete(unit, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL, &entry);
    }

    /* Remove transient resources */
    ctest_dnxc_cint_run(unit, "sllb_server_fec_tcam_entry_remove", cint_arguments, 0);
    ctest_dnxc_cint_run(unit, "sllb_member_reference_tcam_entry_remove", cint_arguments, 0);

    /* Free test packet buffer */
    diag_sand_packet_free(unit, packet_h);

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify correct routing for VIP and non-VIP routes
 * \param [in] unit - The unit number.
 * \param [in] args - unused
 * \param [in] sand_control - unused
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_basic_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_l3_sllb_basic";
    ctest_cint_argument_t cint_arguments[12];
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    int sllb_table_size = 32;
    int in_port = 200;
    int out_port = 201;
    bcm_switch_fec_property_config_t fec_config;
    int vip_id = 4095;
    int kbp_pcc_enabled = 0;

    SHR_FUNC_INIT_VARS(unit);

    /* Check that the KBP_PCC_STATE_TABLE is enabled */
    if (dnx_kbp_device_enabled(unit))
    {
        /* Sync the external TCAM */
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 1));
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_sllb))
        {
            kbp_pcc_enabled = 1;

        }
    }

    /*
     * Run SLLB cint basic example
     */
    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/cint_sllb_basic.c"), "cint_sllb_basic.c Load Failed\n");
    cint_arguments[0].type = SAL_FIELD_TYPE_UINT32;
    cint_arguments[0].value.value_uint32 = sllb_table_size;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = in_port;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = out_port;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = CTEST_SLLB_BASIC_NOF_SERVERS;
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = fec_config.start;
    cint_arguments[5].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[5].value.value_int32 = vip_id;
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "sllb_basic_example", cint_arguments, 6),
            "sllb_basic_example Run Failed\n");

    /*
     * Run SLLB cint field example
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/cint_sllb_field.c"), "cint_sllb_field.c Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "sllb_field_example", NULL, 0),
            "sllb_field_example Run Failed\n");

    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(dnx_l3_sllb_basic_vip_route_test(unit, mapping_info.core, mapping_info.pp_port, mapping_info.tm_port, vip_id, fec_config.start, sllb_table_size, kbp_pcc_enabled));

    SHR_IF_ERR_EXIT(dnx_l3_sllb_basic_direct_route_test(unit, mapping_info.core, mapping_info.pp_port, mapping_info.tm_port, fec_config.start));

exit:

    /* Cleanup and allocated resources */
    ctest_dnxc_cint_run(unit, "sllb_basic_example_cleanup", NULL, 0);

    /* Cleanup and allocated resources */
    ctest_dnxc_cint_run(unit, "sllb_field_example_cleanup", NULL, 0);

    /* Reset cint variables */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "ctest cint_reset failed\n");

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
 * \brief
 *   Semantic tests for SLLB bcmSwitch controls. Returned parameters are used by API semantic tests.
 * \param [in] unit - The unit number.
 * \param [in] max_vip_ecmp_table_size - maximum supported SLLB table size
 * \param [in] nof_fecs - number of fecs supported by the device.
 * \param [in] nof_ecmp - number of ecmp fecs supported by the device.
 * \param [in] nof_vip_ecmp - number of vip ecmp fecs supported by the device.
 * \param [out] sllb_table_size - configured table size
 * \param [out] vip_ecmp_fec_min_id - configured min VIP ECMP FEC_ID
 * \param [out] vip_ecmp_fec_max_id - configured max VIP ECMP FEC_ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_switch_control_sem_tests(
        int unit,
        int max_vip_ecmp_table_size,
        int nof_fecs,
        int nof_ecmp,
        int nof_vip_ecmp,
        int *sllb_table_size,
        int *vip_ecmp_fec_min_id,
        int *vip_ecmp_fec_max_id)
{
    int rv = BCM_E_NONE;
    int arg_value_set;
    int arg_value_get;

    SHR_FUNC_INIT_VARS (unit);

    /*
     * 1. bcmSwitchVIPEcmpTableSize semantics tests
     * Expect _PARAM error unless (0 <= bcmSwitchVIPEcmpTableSize <= max_vip_ecmp_table_size)
     */

    /* 1.a test arg = CTEST_SLLB_MIN_VIP_ECMP_TABLE_SIZE - 1 */
    arg_value_set = CTEST_SLLB_MIN_VIP_ECMP_TABLE_SIZE - 1;
    rv = bcm_switch_control_set (unit, bcmSwitchVIPEcmpTableSize, arg_value_set);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM bcmSwitchVIPEcmpTableSize with arg (%d) < %d, rv (%d).\n",
                arg_value_set, CTEST_SLLB_MIN_VIP_ECMP_TABLE_SIZE, rv);
    }

    /* 1.b test arg = max_vip_ecmp_table_size + 1 */
    arg_value_set = max_vip_ecmp_table_size + 1;
    rv =
        bcm_switch_control_set (unit, bcmSwitchVIPEcmpTableSize, arg_value_set);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM bcmSwitchVIPEcmpTableSize with arg > maximum supported size (%d), rv (%d).\n",
                max_vip_ecmp_table_size, rv);
    }

    /* 1.c test arg = CTEST_SLLB_MIN_VIP_ECMP_TABLE_SIZE */
    arg_value_set = CTEST_SLLB_MIN_VIP_ECMP_TABLE_SIZE;
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchVIPEcmpTableSize, arg_value_set));

    /*
     * Get bcmSwitchVIPEcmpTableSize and compare to set value
     */
    SHR_IF_ERR_EXIT (bcm_switch_control_get
            (unit, bcmSwitchVIPEcmpTableSize, &arg_value_get));
    if (arg_value_get != arg_value_set)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. bcmSwitchVIPEcmpTableSize from bcm_switch_control_get (%d) does not match set value (%d).\n",
                arg_value_get, arg_value_set);
    }

    /* 1.d test arg = max_vip_ecmp_table_size */
    arg_value_set = max_vip_ecmp_table_size;
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchVIPEcmpTableSize, arg_value_set));

    /*
     * Get bcmSwitchVIPEcmpTableSize and compare to set value
     */
    SHR_IF_ERR_EXIT (bcm_switch_control_get
            (unit, bcmSwitchVIPEcmpTableSize, &arg_value_get));
    if (arg_value_get != arg_value_set)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. bcmSwitchVIPEcmpTableSize from bcm_switch_control_get (%d) does not match set value (%d).\n",
                arg_value_get, arg_value_set);
    }
    *sllb_table_size = arg_value_get;


    /*
     * 2. bcmSwitchVIPL3EgressIDMin semantics tests
     * Expect _PARAM error unless (nof_ecmp - 1 < bcmSwitchVIPL3EgressIDMin <= (nof_fecs - nof_vip_ecmp)
     */

    /* 2.a test arg = nof_ecmp - 1 */
    arg_value_set = nof_ecmp - 1;
    rv =
        bcm_switch_control_set (unit, bcmSwitchVIPL3EgressIDMin, arg_value_set);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for bcmSwitchVIPL3EgressIDMin (%d) in device ECMP range (%d), rv (%d).\n",
                arg_value_set, (nof_ecmp - 1), rv);
    }

    /* 2.b test arg = nof_fecs - nof_vip_ecmp + 1 */
    arg_value_set = nof_fecs - nof_vip_ecmp + 1;
    rv =
        bcm_switch_control_set (unit, bcmSwitchVIPL3EgressIDMin, arg_value_set);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for VIP ECMP FEC range "
                "bcmSwitchVIPL3EgressIDMin (%d) to bcmSwitchVIPL3EgressIDMax (%d) which exceeds in device nof_fecs (%d), rv (%d).\n",
                arg_value_set, (arg_value_set + nof_vip_ecmp - 1),
                (nof_fecs - nof_vip_ecmp + 1), rv);
    }

    /* 2.c test arg = nof_fecs - nof_vip_ecmp */
    SHR_IF_ERR_EXIT (bcm_switch_control_get
            (unit, bcmSwitchVIPL3EgressIDMin, &arg_value_get));

    if (arg_value_get == 0)
    {
        arg_value_set = nof_fecs - nof_vip_ecmp;
        SHR_IF_ERR_EXIT (bcm_switch_control_set
                (unit, bcmSwitchVIPL3EgressIDMin, arg_value_set));

        /*  Get bcmSwitchVIPL3EgressIDMin and compare to set value */
        SHR_IF_ERR_EXIT (bcm_switch_control_get
                (unit, bcmSwitchVIPL3EgressIDMin, &arg_value_get));
        if (arg_value_get != arg_value_set)
        {
            SHR_ERR_EXIT (_SHR_E_FAIL,
                    "Test failed. bcmSwitchVIPL3EgressIDMin from bcm_switch_control_get (%d) does not match set value (%d).\n",
                    arg_value_get, arg_value_set);
        }
    }
    *vip_ecmp_fec_min_id = arg_value_get;

    /* Get bcmSwitchVIPL3EgressIDMax and compare to set value */
    arg_value_set = *vip_ecmp_fec_min_id + nof_vip_ecmp - 1;
    SHR_IF_ERR_EXIT (bcm_switch_control_get
            (unit, bcmSwitchVIPL3EgressIDMax, &arg_value_get));
    if (arg_value_get != arg_value_set)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. bcmSwitchVIPL3EgressIDMax from bcm_switch_control_get (%d) does not match set value (%d).\n",
                arg_value_get, arg_value_set);
    }
    *vip_ecmp_fec_max_id = arg_value_get;


    /*
     * 3. bcmSwitchSLLBDestinationNotFound semantic tests
     * Supported GPORT types:
     *    BCM_GPORT_SYSTEM_PORT
     *    BCM_GPORT_LOCAL
     *    BCM_GPORT_LOCAL_CPU
     *    BCM_GPORT_TRUNK
     *    BCM_GPORT_MCAST
     *    BCM_GPORT_UCAST_QUEUE_GROUP
     *    BCM_GPORT_MCAST_QUEUE_GROUP
     *    BCM_GPORT_TRAP
     *    BCM_GPORT_FORWARD_PORT
     *    BCM_GPORT_BLACK_HOLE
     *
     */
    arg_value_set = 999;
    rv = bcm_switch_control_set(unit, bcmSwitchSLLBDestinationNotFound, arg_value_set);
    if (rv != BCM_E_PORT)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw BCM_E_PORT for Invalid bcmSwitchSLLBDestinationNotFound, rv (%d).\n",
                arg_value_set);
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    BCM_GPORT_LOCAL_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound,
             BCM_GPORT_LOCAL_CPU));

    BCM_GPORT_TRUNK_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    BCM_GPORT_MCAST_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    BCM_GPORT_UCAST_QUEUE_GROUP_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    BCM_GPORT_MCAST_QUEUE_GROUP_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    BCM_GPORT_TRAP_SET (arg_value_set, 0, 0, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    BCM_GPORT_FORWARD_PORT_SET (arg_value_set, 0);
    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound, arg_value_set));

    SHR_IF_ERR_EXIT (bcm_switch_control_set
            (unit, bcmSwitchSLLBDestinationNotFound,
             BCM_GPORT_BLACK_HOLE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Callback for traverse function. Counts the number of matching entries.
 * \param [in] unit - The unit number.
 * \param [in] ecmp - vip ecmp group information
 * \param [in] intf_count - the number of FECs entries in the group
 * \param [in] intf_array - holds the FEC IDs
 * \param [out] counmter - incremented per callback access
 * \return
 *   \retval Zero
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static int
dnx_l3_sllb_ecmp_trav_cb(
        int unit,
        bcm_l3_egress_ecmp_t *ecmp,
        int intf_count,
        bcm_if_t *intf_array,
        void *counter)
{
    int count = *(int *) counter;
    count++;
    *(int *) counter = count;
    return _SHR_E_NONE;
}

/**
 * \brief
 *   Semantic tests for bcm_l3_egress_ecmp* VIP functions
 * \param [in] unit - The unit number.
 * \param [in] sllb_table_size - configured table size
 * \param [in] nof_ecmp - number of ecmp fecs supported by the device.
 * \param [in] vip_ecmp_fec_min_id - configured min VIP ECMP FEC_ID
 * \param [in] vip_ecmp_fec_max_id - configured max VIP ECMP FEC_ID
 * \return
 *   \retval Zero if no error.
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_ecmp_sem_tests(
        int unit,
        int sllb_table_size,
        int nof_ecmp,
        int vip_ecmp_fec_min_id,
        int vip_ecmp_fec_max_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_if_t *intf_array = NULL;
    bcm_if_t *update_intf_array = NULL;
    bcm_if_t *get_intf_array = NULL;
    bcm_if_t ecmp_intf = 0;
    int intf_count;
    int get_intf_count;
    int vip_ecmp_fec_id;
    int scale_sllb_table_size;

    SHR_FUNC_INIT_VARS (unit);

    SHR_ALLOC_SET_ZERO(intf_array, (sizeof (bcm_if_t) * sllb_table_size),
            "VIP ECMP intf_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(get_intf_array, (sizeof (bcm_if_t) * sllb_table_size),
            "VIP ECMP intf_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(update_intf_array, (sizeof (bcm_if_t) * sllb_table_size),
            "VIP ECMP update_intf_array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /* 
     * bcm_l3_egress_ecmp_create semantic tests
     *
     * 1. Test NULL pointers 
     */
    rv = bcm_l3_egress_ecmp_create (unit, NULL, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for ecmp NULL pointer.\n");
    }

    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, NULL);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for intf_array NULL pointer.\n");
    }

    /*
     * 2. Test invalid intf_count
     */
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size - 1, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for sllb_table_size != configured bcmSwitchVIPEcmpTableSize");
    }


    /* 
     * 2. Test unsupported flags 
     */
    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.flags = BCM_L3_2ND_HIERARCHY;
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for unsupported flags (BCM_L3_2ND_HIERARCHY).\n");
    }

    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.flags = BCM_L3_ECMP_LARGE_TABLE;
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for unsupported flags (BCM_L3_ECMP_LARGE_TABLE).\n");
    }

    /*
     * 3. Test invalid dynamic_mode
     */
    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP|BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for invalid dynamic mode.\n");
    }


    /* 
     * 4. Test invalid interface without REPLACE 
     */
    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    intf_array[0] = BCM_GPORT_INVALID;
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM if any member of intf_array is BCM_GPORT_INVALID when BCM_L3_REPLACE flag is not set.\n");
    }


    /* 
     * 5. Test invalid intf_count 
     */
    intf_count = sllb_table_size - 1;
    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, intf_count, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM if intf_count does not match configured bcmSwitchVIPEcmpTableSize.\n");
    }

    /* 
     * 6.  test ecmp_intf out of VIP ECMP FEC range 
     */
    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_min_id-1);
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM if ecmp.ecmp_intf (%d) is not in the VIP ECMP range.\n", ecmp.ecmp_intf);
    }

    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id+1);
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp, sllb_table_size, intf_array);
    if (rv != BCM_E_PARAM)
    {
        SHR_ERR_EXIT (_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM if ecmp.ecmp_intf (%d) is not in the VIP ECMP range.\n", ecmp.ecmp_intf);
    }

    /* 
     * 7. Create ecmp 
     */
    for (intf_count = 0; intf_count < sllb_table_size; intf_count++)
    {
        /* Use dummy FEC-ID outside of ECMP range */
        BCM_L3_ITF_SET(intf_array[intf_count], BCM_L3_ITF_TYPE_FEC, (nof_ecmp+intf_count));
    }

    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, sllb_table_size, intf_array));
    ecmp_intf = ecmp.ecmp_intf;

    /* Get the ecmp group and compare that the interface array matches the configured array */
    sal_memset(get_intf_array, 0, (sizeof(bcm_if_t) * sllb_table_size));
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = ecmp_intf;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp, sllb_table_size, get_intf_array, &get_intf_count));
    if (get_intf_count != sllb_table_size)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. intf_count from bcm_l3_egress_ecmp_get (%d) does not match intf_count set in bcm_l3_egress_ecmp_create (%d).\n",
                intf_count, sllb_table_size);

    }

    for (intf_count = 0; intf_count < sllb_table_size; intf_count++)
    {
        if (intf_array[intf_count] != get_intf_array[intf_count])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. intf_array[%d] (%d) from bcm_l3_egress_ecmp_get does not match intf set in bcm_l3_egress_ecmp_create (%d).\n",
                    intf_count, get_intf_array[intf_count], intf_array[intf_count]);
        }
    }

    /* 
     * 8. Test replace
     */

    /* Replace one member in the intf_array  - update full intf_array in HW */
    BCM_L3_ITF_SET(intf_array[sllb_table_size-1], BCM_L3_ITF_TYPE_FEC, nof_ecmp);

    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.flags |= BCM_L3_WITH_ID;
    ecmp.flags |= BCM_L3_REPLACE;
    ecmp.ecmp_intf = ecmp_intf;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, sllb_table_size, intf_array));

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = ecmp_intf;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp, sllb_table_size, get_intf_array, &get_intf_count));
    if (get_intf_count != sllb_table_size)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. After replace intf_count from bcm_l3_egress_ecmp_get (%d) does not match intf_count set in bcm_l3_egress_ecmp_create (%d).\n",
                intf_count, sllb_table_size);

    }
    for (intf_count = 0; intf_count < sllb_table_size; intf_count++)
    {
        if (get_intf_array[intf_count] != intf_array[intf_count])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. After replace intf_array[%d] from bcm_l3_egress_ecmp_get intf (%d) does not match intf set in bcm_l3_egress_ecmp_create (%d).\n",
                    get_intf_count, get_intf_array[intf_count], intf_array[intf_count]);
        }
    }


    /* Replace one member - update only the modified member in HW */
    for (intf_count = 0; intf_count < sllb_table_size-1; intf_count++)
    {
        /* members set to BCM_GPORT_INVALID will skip HW update */
        update_intf_array[intf_count] = BCM_GPORT_INVALID;
    }
    BCM_L3_ITF_SET(intf_array[sllb_table_size-1], BCM_L3_ITF_TYPE_FEC, nof_ecmp+1);
    update_intf_array[sllb_table_size-1] = intf_array[sllb_table_size-1];

    bcm_l3_egress_ecmp_t_init (&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.flags |= BCM_L3_WITH_ID;
    ecmp.flags |= BCM_L3_REPLACE;
    ecmp.ecmp_intf = ecmp_intf;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, sllb_table_size, update_intf_array));

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = ecmp_intf;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp, sllb_table_size, get_intf_array, &intf_count));
    if (intf_count != sllb_table_size)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. After (BCM_GPORT_INVALID) replace intf_count from bcm_l3_egress_ecmp_get (%d) does not match intf_count set in bcm_l3_egress_ecmp_create (%d).\n",
                intf_count, sllb_table_size);

    }
    for (intf_count = 0; intf_count < sllb_table_size; intf_count++)
    {
        if (get_intf_array[intf_count] != intf_array[intf_count])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. After (BCM_GPORT_INVALID) replace intf_array[%d] from bcm_l3_egress_ecmp_get does not match intf set in bcm_l3_egress_ecmp_create (%d).\n",
                    intf_count, get_intf_array[intf_count]);
        }
    }

    /* 
     * bcm_l3_egress_ecmp_get semantic tets 
     */

    /* 
     * 1. Test ecmp group not in VIP ECMP range 
     */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = nof_ecmp-1;
    rv =(bcm_l3_egress_ecmp_get(unit, &ecmp, sllb_table_size, get_intf_array, &intf_count));
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM if ecmp.ecmp_intf (%d) is not in VIP ECMP range.\n", ecmp.ecmp_intf);
    }

    /* 
     * 2. Test unallocated VIP ECP group 
     */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = ecmp_intf+1;
    rv =(bcm_l3_egress_ecmp_get(unit, &ecmp, sllb_table_size, get_intf_array, &intf_count));
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. Should throw _SHR_E_NOT_FOUND if ecmp.ecmp_intf (%d) is not allocated.\n", ecmp.ecmp_intf);
    }

    /* 
     * bcm_l3_egress_ecmp_destroy semantic tets 
     */

    /* 
     * 1. Test ecmp group not in VIP ECMP range 
     */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = nof_ecmp-1;
    rv =(bcm_l3_egress_ecmp_destroy(unit, &ecmp));
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM if ecmp.ecmp_intf (%d) is not in VIP ECMP range.\n", ecmp.ecmp_intf);
    }

    /* 
     * 2. Test unallocated VIP ECP group 
     */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = ecmp_intf+1;
    rv =(bcm_l3_egress_ecmp_destroy(unit, &ecmp));
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. Should throw _SHR_E_NOT_FOUND if ecmp.ecmp_intf (%d) is not allocated.\n", ecmp.ecmp_intf);
    }

    /*
     * 3. Destroy created ecmp group
     */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.ecmp_intf = ecmp_intf;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));

    /*
     * bcm_l3_egress_ecmp allocation tests
     */
    scale_sllb_table_size = 1;
    SHR_IF_ERR_EXIT(bcm_switch_control_set (unit, bcmSwitchVIPEcmpTableSize, scale_sllb_table_size));
    for (intf_count = 0; intf_count < scale_sllb_table_size; intf_count++)
    {
        /* Use dummy FEC-ID outside of ECMP range */
        BCM_L3_ITF_SET(intf_array[intf_count], BCM_L3_ITF_TYPE_FEC, nof_ecmp);
    }

    for (vip_ecmp_fec_id = vip_ecmp_fec_min_id; vip_ecmp_fec_id <= vip_ecmp_fec_max_id; vip_ecmp_fec_id++)
    {
        bcm_l3_egress_ecmp_t_init (&ecmp);
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, scale_sllb_table_size, intf_array));
    }

    for (vip_ecmp_fec_id = vip_ecmp_fec_min_id; vip_ecmp_fec_id <= vip_ecmp_fec_max_id; vip_ecmp_fec_id++)
    {
        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
        BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));
    }

    for (vip_ecmp_fec_id = vip_ecmp_fec_max_id; vip_ecmp_fec_id >= vip_ecmp_fec_min_id; vip_ecmp_fec_id--)
    {
        bcm_l3_egress_ecmp_t_init (&ecmp);
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
        ecmp.flags = BCM_L3_WITH_ID;
        BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, scale_sllb_table_size, intf_array));
    }

    /*
     * 4. bcm_l3_egress_ecmp_traverse semantic tests
     */
    {
        int count = 0;
        int expected_count = vip_ecmp_fec_max_id - vip_ecmp_fec_min_id + 1;
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_traverse(unit, dnx_l3_sllb_ecmp_trav_cb, &count));
        if (count != expected_count)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. bcm_l3_egress_ecmp_traverse count (%d) != expected count (%d).\n", count, expected_count);
        }
    }

exit:
    /* cleanup */
    for (vip_ecmp_fec_id = vip_ecmp_fec_min_id; vip_ecmp_fec_id <= vip_ecmp_fec_max_id; vip_ecmp_fec_id++)
    {
        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
        BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
        bcm_l3_egress_ecmp_destroy(unit, &ecmp);
    }

    SHR_FREE(intf_array);
    SHR_FREE(update_intf_array);
    SHR_FREE(get_intf_array);

    SHR_FUNC_EXIT;
}

static int
dnx_l3_sllb_resilient_trav_cb(
        int unit,
        bcm_l3_egress_ecmp_resilient_entry_t *entry,
        void *counter)
{
    int count = *(int *) counter;
    count++;
    *(int *) counter = count;
    return _SHR_E_NONE;
}

/**
 * \brief
 *   Semantic tests for bcm_l3_egress_resilient* VIP functions
 * \param [in] unit - The unit number.
 * \param [in] vip_ecmp_fec_min_id - configured min VIP ECMP FEC_ID
 * \param [in] vip_ecmp_fec_max_id - configured max VIP ECMP FEC_ID
 * \param [in] pcc_fec_id - server FEC_ID to be added to PCC table
 * \return
 *   \retval Zero if no error.
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_resilient_sem_tests(
        int unit,
        int vip_ecmp_fec_min_id,
        int vip_ecmp_fec_max_id,
        int pcc_fec_id)
{
    int rv = BCM_E_NONE;
    int kbp_pcc_enabled = 0;
    bcm_l3_egress_ecmp_resilient_entry_t entry;
    bcm_l3_egress_ecmp_resilient_entry_t match_entry;
    bcm_l3_egress_ecmp_resilient_entry_t replace_entry;
    int num_entries;
    int table_index;
    int flags_index;
    uint32 flags;
    uint32 table_flags[CTEST_NUM_PCC_TABLES] = {
        BCM_L3_ECMP_RESILIENT_PCC, 
        BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL};
    uint32 match_flags[CTEST_NUM_MATCH_FLAGS_COMBO] = {
        0,
        BCM_L3_ECMP_RESILIENT_MATCH_ECMP, 
        BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY, 
        BCM_L3_ECMP_RESILIENT_MATCH_INTF,
        (BCM_L3_ECMP_RESILIENT_MATCH_ECMP|BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY),
        (BCM_L3_ECMP_RESILIENT_MATCH_ECMP|BCM_L3_ECMP_RESILIENT_MATCH_INTF),
        (BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY|BCM_L3_ECMP_RESILIENT_MATCH_INTF),
        (BCM_L3_ECMP_RESILIENT_MATCH_ECMP|BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY|BCM_L3_ECMP_RESILIENT_MATCH_INTF)};

    SHR_FUNC_INIT_VARS (unit);

    /* Check that the KBP_PCC_STATE_TABLE is enabled */
    if (dnx_kbp_device_enabled(unit))
    {
        /* Sync the external TCAM */
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 1));
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_sllb))
        {
            kbp_pcc_enabled = 1;

        }
    }

    /*
     * 1. Test NULL pointer
     */
    rv = bcm_l3_egress_ecmp_resilient_add(unit, BCM_L3_ECMP_RESILIENT_PCC, NULL);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for intf_array NULL pointer.\n");
    }

    rv = bcm_l3_egress_ecmp_resilient_add(unit, 0, &entry);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                "Test failed. Should throw E_PARAM for missing flags.\n");
    }

    /*
     * bcm_l3_egress_ecmp_resilient_replace semantic tests
     *
     * Test all combinations of match criteria:
     * - BCM_L3_ECMP_RESILIENT_MATCH_ECMP
     * - BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY
     * - BCM_L3_ECMP_RESILIENT_MATCH_INTF
     */
    for (table_index = 0; table_index < CTEST_NUM_PCC_TABLES; table_index++)
    {
        flags = table_flags[table_index];
        if (flags == BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL && !kbp_pcc_enabled)
        {
            /* If KBP_PCC_STATE table is not enabled, skip semantic tests on this table */
            continue;
        }

        /* Test all combinations of match criteria -- passing cases */
        for (flags_index = 0; flags_index < CTEST_NUM_MATCH_FLAGS_COMBO; flags_index++)
        {
            /* Add an entry */
            sal_memset(&entry, 0, sizeof(entry));
            BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
            COMPILER_64_ZERO(entry.hash_key);
            COMPILER_64_SET(entry.hash_key, 0x0, 0x12345678);
            BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id);
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_add(unit, flags, &entry));

            sal_memset(&match_entry, 0, sizeof(match_entry));
            flags |= BCM_L3_ECMP_RESILIENT_REPLACE;
            flags |= match_flags[flags_index];
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_ECMP)
            {
                BCM_L3_ITF_SET(match_entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
            }
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY)
            {
                COMPILER_64_ZERO(match_entry.hash_key);
                COMPILER_64_SET(match_entry.hash_key, 0x0, 0x12345678);
            }
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_INTF)
            {
                BCM_L3_ITF_SET(match_entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id);
            }
            sal_memset(&replace_entry, 0, sizeof(replace_entry));
            BCM_L3_ITF_SET(replace_entry.intf, BCM_L3_ITF_TYPE_FEC, (pcc_fec_id+1));
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_replace(unit, flags, &match_entry, &num_entries, &replace_entry));

            flags &= ~BCM_L3_ECMP_RESILIENT_REPLACE;
            flags |= BCM_L3_ECMP_RESILIENT_DELETE;
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_INTF)
            {
                /* Use updated interface, from replace above */
                BCM_L3_ITF_SET(match_entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id+1);
            }
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_replace(unit, flags, &match_entry, &num_entries, &replace_entry));
        }

        /* Add an entry */
        sal_memset(&entry, 0, sizeof(entry));
        BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
        COMPILER_64_ZERO(entry.hash_key);
        COMPILER_64_SET(entry.hash_key, 0x0, 0x12345678);
        BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id);
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_add(unit, flags, &entry));

        /* Test all combinations of match criteria  -- failing cases */
        for (flags_index = 0; flags_index < CTEST_NUM_MATCH_FLAGS_COMBO; flags_index++)
        {
            sal_memset(&match_entry, 0, sizeof(match_entry));
            BCM_L3_ITF_SET(match_entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
            COMPILER_64_ZERO(match_entry.hash_key);
            COMPILER_64_SET(match_entry.hash_key, 0x0, 0x12345678);
            BCM_L3_ITF_SET(match_entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id);

            flags |= BCM_L3_ECMP_RESILIENT_REPLACE;
            flags |= match_flags[flags_index];
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_ECMP)
            {
                BCM_L3_ITF_SET(match_entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_min_id);
            }
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY)
            {
                COMPILER_64_ZERO(match_entry.hash_key);
            }
            if (flags & BCM_L3_ECMP_RESILIENT_MATCH_INTF)
            {
                BCM_L3_ITF_SET(match_entry.intf, BCM_L3_ITF_TYPE_FEC, 0);
            }

            sal_memset(&replace_entry, 0, sizeof(replace_entry));
            BCM_L3_ITF_SET(replace_entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id+1);
            rv = bcm_l3_egress_ecmp_resilient_replace(unit, flags, &match_entry, &num_entries, &replace_entry);
            if (rv != BCM_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                        "Test failed. Should throw E_NOT_FOUND for unmatched key field ecmp_intf");
            }
        }
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_delete(unit, flags, &entry));
    }

    /*
     * bcm_l3_egress_ecmp_resilient_delete semantic tests
     */
    for (table_index = 0; table_index < CTEST_NUM_PCC_TABLES; table_index++)
    {
        flags = table_flags[table_index];
        if (flags == BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL && !kbp_pcc_enabled)
        {
            /* If KBP_PCC_STATE table is not enabled, skip semantic tests on this table */
            continue;
        }

        /* Add entry */
        sal_memset(&entry, 0, sizeof(entry));
        BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
        COMPILER_64_ZERO(entry.hash_key);
        COMPILER_64_SET(entry.hash_key, 0x0, 0x12345678);
        BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, pcc_fec_id);
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_add(unit, flags, &entry));

        /* Test invalid ecmp_intf and invalid hash_key*/
        sal_memset(&entry, 0, sizeof(entry));
        BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_min_id);
        rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, &entry);
        if (rv != BCM_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. Should throw E_NOT_FOUND for unmatched key fields");
        }

        /* Test invalid ecmp_intf */
        sal_memset(&entry, 0, sizeof(entry));
        BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_min_id);
        COMPILER_64_ZERO(entry.hash_key);
        COMPILER_64_SET(entry.hash_key, 0x0, 0x12345678);
        rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, &entry);
        if (rv != BCM_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. Should throw E_NOT_FOUND for unmatched key field ecmp_intf");
        }

        /* Test invalid hash_key */
        sal_memset(&entry, 0, sizeof(entry));
        BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
        rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, &entry);
        if (rv != BCM_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                    "Test failed. Should throw E_NOT_FOUND for unmatched key field hash_key");
        }

        /* Delete entry */
        sal_memset(&entry, 0, sizeof(entry));
        BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_max_id);
        COMPILER_64_ZERO(entry.hash_key);
        COMPILER_64_SET(entry.hash_key, 0x0, 0x12345678);
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_delete(unit, flags, &entry));

    }

    /* bcm_l3_egress_ecmp_resilient_traverse semantic tests */
    {
        for (table_index = 0; table_index < CTEST_NUM_PCC_TABLES; table_index++)
        {
            int vip_ecmp_fec_id;
            int start_vip_ecmp_fec_id = vip_ecmp_fec_min_id+1;  /* Use vip_id 0 for mismatch */
            int num_vip_ecmp_fec_id = 2;
            int signature;
            int start_signature = 0x12345670;
            int num_signature = 3;
            int fec_id;
            int start_fec_id = pcc_fec_id;
            int count;
            int expected_count;

            flags = table_flags[table_index];
            if (flags == BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL && !kbp_pcc_enabled)
            {
                /* If KBP_PCC_STATE table is not enabled, skip semantic tests on this table */
                continue;
            }

            fec_id = start_fec_id;
            for (vip_ecmp_fec_id = start_vip_ecmp_fec_id; vip_ecmp_fec_id < (start_vip_ecmp_fec_id + num_vip_ecmp_fec_id); vip_ecmp_fec_id++)
            {
                for (signature = start_signature; signature < (start_signature + num_signature); signature++)
                {
                    /* Add entry */
                    sal_memset(&entry, 0, sizeof(entry));
                    BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
                    COMPILER_64_ZERO(entry.hash_key);
                    COMPILER_64_SET(entry.hash_key, 0x0, signature);
                    BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, fec_id++);
                    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_add(unit, flags, &entry));
                }
            }
            for (flags_index = 0; flags_index < CTEST_NUM_MATCH_FLAGS_COMBO; flags_index++)
            {
                flags = table_flags[table_index];
                flags |= match_flags[flags_index];
                count = 0;
                expected_count = (num_vip_ecmp_fec_id * num_signature);
                sal_memset(&match_entry, 0, sizeof(match_entry));

                if (flags & BCM_L3_ECMP_RESILIENT_MATCH_ECMP)
                {   
                    BCM_L3_ITF_SET(match_entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, start_vip_ecmp_fec_id);
                    expected_count = num_signature;
                }
                if (flags & BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY)
                {
                    COMPILER_64_ZERO(match_entry.hash_key);
                    COMPILER_64_SET(match_entry.hash_key, 0x0, start_signature);
                    if (flags & BCM_L3_ECMP_RESILIENT_MATCH_ECMP)
                    {
                        expected_count = 1;
                    }
                    else
                    {
                        expected_count = num_vip_ecmp_fec_id;
                    }
                }
                if (flags & BCM_L3_ECMP_RESILIENT_MATCH_INTF)
                {   
                    BCM_L3_ITF_SET(match_entry.intf, BCM_L3_ITF_TYPE_FEC, start_fec_id);
                    expected_count = 1;
                }
                SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_pcc_resilient_traverse(unit, flags, &match_entry, dnx_l3_sllb_resilient_trav_cb, &count));
                if (count != expected_count)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                            "Test failed. bcm_l3_egress_ecmp_resilient_traverse count (%d) != expected count (%d).\n", count, expected_count);
                }
            }
            fec_id = start_fec_id;
            for (vip_ecmp_fec_id = start_vip_ecmp_fec_id; vip_ecmp_fec_id < (start_vip_ecmp_fec_id + num_vip_ecmp_fec_id); vip_ecmp_fec_id++)
            {
                for (signature = start_signature; signature < (start_signature + num_signature); signature++)
                {
                    /* Add entry */
                    sal_memset(&entry, 0, sizeof(entry));
                    BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
                    COMPILER_64_ZERO(entry.hash_key);
                    COMPILER_64_SET(entry.hash_key, 0x0, signature);
                    BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, fec_id++);
                    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_resilient_delete(unit, flags, &entry));
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Entry point for SLLB semantic tests
 * \param [in] unit - The unit number.
 * \param [in] args - unused
 * \param [in] sand_control - unused
 * \return
 *   \retval Zero if no error.
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_sllb_sem_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_l3_sllb_sem";
    int max_vip_ecmp_table_size;
    int nof_fecs;
    int nof_ecmp;
    int nof_vip_ecmp;
    int sllb_table_size;
    int vip_ecmp_fec_min_id;
    int vip_ecmp_fec_max_id;

    SHR_FUNC_INIT_VARS (unit);

    max_vip_ecmp_table_size =
        dnx_data_l3.vip_ecmp.max_vip_ecmp_table_size_get (unit);
    nof_fecs = dnx_data_l3.fec.nof_fecs_get (unit);
    nof_ecmp = dnx_data_l3.ecmp.nof_ecmp_get (unit);
    nof_vip_ecmp = dnx_data_l3.vip_ecmp.nof_vip_ecmp_get (unit);

    /* bcmSwitchControl* semantic tests */
    SHR_IF_ERR_EXIT(dnx_l3_sllb_switch_control_sem_tests(unit, 
                max_vip_ecmp_table_size, 
                nof_fecs, 
                nof_ecmp, 
                nof_vip_ecmp, 
                &sllb_table_size,
                &vip_ecmp_fec_min_id,
                &vip_ecmp_fec_max_id));

    /* bcm_l3_egress_ecmp_* semantic tests */
    SHR_IF_ERR_EXIT(dnx_l3_sllb_ecmp_sem_tests(unit, 
                sllb_table_size,
                nof_ecmp,
                vip_ecmp_fec_min_id,
                vip_ecmp_fec_max_id));

    /* bcm_l3_egress_ecmp_resilient* semantic tests */
    SHR_IF_ERR_EXIT(dnx_l3_sllb_resilient_sem_tests(unit,
                vip_ecmp_fec_min_id,
                vip_ecmp_fec_max_id,
                nof_ecmp));

exit:
    if (!SHR_FUNC_ERR())
    {
        LOG_CLI_EX("\r\n" "Test '%s' completed SUCCESSFULLY. %s%s%s\r\n",
                test_name, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' Failed! %s%s%s\r\n", test_name, EMPTY,
                EMPTY, EMPTY);
    }

    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 intf Tests
 * List of intf ctests modules.
 */
sh_sand_cmd_t dnx_l3_sllb_test_cmds[] = {
    {"sem",   dnx_l3_sllb_sem_cmd,   NULL, sh_dnx_l3_sllb_sem_options,   &sh_dnx_l3_sllb_sem_man,   NULL, NULL, CTEST_POSTCOMMIT},
    {"basic", dnx_l3_sllb_basic_cmd, NULL, sh_dnx_l3_sllb_basic_options, &sh_dnx_l3_sllb_basic_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
