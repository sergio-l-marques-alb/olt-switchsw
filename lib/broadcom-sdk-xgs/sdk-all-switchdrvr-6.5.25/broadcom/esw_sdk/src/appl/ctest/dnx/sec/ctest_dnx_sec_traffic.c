/** \file ctest_dnx_sec_traffic.c
 *
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /**
  * Include files.
  * {
  */
/** sal */
#include <sal/appl/config.h>
#include <sal/compiler.h>

#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_SEC_TRAFFIC_OPTION_DIRECTION        "direction"

/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_SEC_TRAFFIC_OPTION_CLEAN              "clean"


#define CTEST_DNX_SEC_TRAFFIC_DIRECTION_EGRESS                      0
#define CTEST_DNX_SEC_TRAFFIC_DIRECTION_INGRESS                     1

/**
 * Enum for direction requiring
 */
sh_sand_enum_t ctest_dnx_sec_traffic_direction_enum_table[] = {
    {"Egress", CTEST_DNX_SEC_TRAFFIC_DIRECTION_EGRESS, "Egress Direction"},
    {"Ingress", CTEST_DNX_SEC_TRAFFIC_DIRECTION_INGRESS, "Ingress Direction"},
    {NULL}
};

/**
 * \brief
 *   Options list for 'traffic' shell command
 * \remark
 */
sh_sand_option_t sh_ctest_dnx_sec_traffic_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {CTEST_DNX_SEC_TRAFFIC_OPTION_DIRECTION,   SAL_FIELD_TYPE_UINT32,   "Direction of test (ingress or egress)",        "Egress",     (void *) ctest_dnx_sec_traffic_direction_enum_table, "Egress-Ingress"},
    {CTEST_DNX_SEC_TRAFFIC_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",     "Yes"},
    {NULL}      /* End of options list - must be last. */
};



/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t sh_dnx_sec_traffic_man = {
    "MACSec/IPSec Traffic tests",
    "Simple case of passing traffic through MACSEc block\n",
    "ctest sec traffic direction=<egress | ingress>"
};

static shr_error_e
ctest_dnx_sec_traffic_send_packet_stream(
    int unit,
    bcm_port_t from_port,
    uint32 vlan,
    sal_mac_addr_t dst_mac_addr,
    sal_mac_addr_t src_mac_addr,
    int dst_count)
{
    rhhandle_t packet_h = NULL;
    char mac_str[RHNAME_MAX_SIZE];
    int i_pkt;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port, &flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));

    for (i_pkt = 0; i_pkt < dst_count; i_pkt++)
    {
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                     dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4],
                     dst_mac_addr[5] + i_pkt);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", mac_str));
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                     src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4],
                     src_mac_addr[5] + i_pkt);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port, packet_h, SAND_PACKET_RX));
    }

exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;

}

static shr_error_e
ctest_dnx_sec_traffic_receive_packet_and_compare(
    int unit,
    uint32 vlan,
    sal_mac_addr_t dst_mac_addr,
    sal_mac_addr_t src_mac_addr)
{
    int match_count;
    rhhandle_t packet_rx = NULL;
    char mac_str[RHNAME_MAX_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /** Check the received packet.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_rx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "ETH1"));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4], dst_mac_addr[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.DA", mac_str));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_rx, "ETH1.VLAN.VID", &vlan, 12));

    SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_rx, &match_count));
    if (match_count == 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
    }

exit:
    diag_sand_packet_free(unit, packet_rx);
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * Traffic test basic scenario
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_sec_traffic_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    /*
     * unsigned int test_clean;
     */
    unsigned int test_dir;
    int instance_id = 0;
    int sci = 16;
    bcm_port_t in_port = 201;
    bcm_port_t out_port = 13;
    bcm_port_t port3 = 202;
    sal_mac_addr_t dmac = { 0x94, 0xfd, 0x43, 0x54, 0x31, 0x76 };
    sal_mac_addr_t smac = { 0x94, 0xfd, 0x43, 0x54, 0x31, 0x77 };
    uint32 vlan = 0xfef;
    ctest_cint_argument_t cint_arguments[4];

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Load Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "../../libs/xflow_macsec/src/example/dnx/sec/cint_macsec_simple.c"),
                        "Loading cint_macsec_simple.c failed!\n");

    test_dir = 0;
    /*
     * SH_SAND_GET_BOOL(CTEST_DNX_SEC_TRAFFIC_OPTION_CLEAN, test_clean);
     */
    SH_SAND_GET_UINT32(CTEST_DNX_SEC_TRAFFIC_OPTION_DIRECTION, test_dir);

    if (test_dir == CTEST_DNX_SEC_TRAFFIC_DIRECTION_INGRESS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Direction not supported for this test yet");
    }
    else if (test_dir == CTEST_DNX_SEC_TRAFFIC_DIRECTION_EGRESS)
    {
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, in_port, out_port, 1));

        SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, out_port, BCM_PORT_LOOPBACK_MAC));

        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, out_port, port3, 1));

        /**
         * Set Cint arguments
         */
        sal_memset(cint_arguments, 0, sizeof(cint_arguments));
        cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[0].value.value_int32 = instance_id;
        cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[1].value.value_int32 = out_port;
        cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[2].value.value_int32 = sci;

        /**
         * Run the Cint function to create one Ingress Meter Database with expansion enabled
         * and configuiration based on the user input.
         */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run
                            (unit, "cint_xflow_macsec_encrypt_control_setup_simple", cint_arguments, 3),
                            "Running cint_xflow_macsec_encrypt_control_setup_simple failed!\n");

        /**
         * Set Cint arguments
         */
        sal_memset(cint_arguments, 0, sizeof(cint_arguments));
        cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[0].value.value_int32 = out_port;
        cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[1].value.value_int32 = sci;

        /**
         * Run the Cint function to create one Ingress Meter Database with expansion enabled
         * and configuiration based on the user input.
         */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_xflow_macsec_encrypt_flow_simple", cint_arguments, 2),
                            "Running cint_xflow_macsec_encrypt_flow_simple failed!\n");
        /**
         * Enable decrypt errors forwarding so packet won't be dropped after loopback
         * Since decrypt path is not configured we need to bypass it somehow
         */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run
                            (unit, "cint_xflow_macsec_decrypt_forward_svtag_error", cint_arguments, 1),
                            "Running cint_xflow_macsec_decrypt_forward_svtag_error failed!\n");

        SHR_IF_ERR_EXIT(ctest_dnx_sec_traffic_send_packet_stream(unit, in_port, vlan, dmac, smac, 1));

        SHR_IF_ERR_EXIT(ctest_dnx_sec_traffic_receive_packet_and_compare(unit, vlan, dmac, smac));

        /**
         * Reset Cints
         */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the SEC Traffic ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field udh"
 */
shr_error_e
sh_dnx_sec_traffic_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_sec_traffic_run(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t dnx_sec_test_cmds[] = {
    {"traffic", sh_dnx_sec_traffic_cmd, NULL, sh_ctest_dnx_sec_traffic_options, &sh_dnx_sec_traffic_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};
