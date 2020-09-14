/** \file ctest_dnx_l3.c
 * $Id$
 *
 * Contains all of the L3 ctest commands
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

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <soc/dnx/dnx_visibility.h>
#include <bcm/switch.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
/**
 * DEFINES
 * {
 */
/**
 * }
 */

extern sh_sand_cmd_t dnx_ecmp_test_cmds[];
extern sh_sand_cmd_t dnx_l3_fec_test_cmds[];
extern sh_sand_cmd_t dnx_l3_ing_intf_test_cmds[];
extern sh_sand_cmd_t dnx_l3_arp_test_cmds[];
extern sh_sand_cmd_t dnx_l3_performance_cmds[];
extern sh_sand_cmd_t dnx_l3_sllb_test_cmds[];
extern sh_sand_man_t sh_dnx_l3_performance_man;

/** L3 ecmp test details */
static sh_sand_man_t sh_dnx_l3_ecmp_man = {
    "Egress ECMP tests",
    "Tests related to bcm_l3_egress_ecmp_* APIs and actions"
};

/** L3 fec test details */
static sh_sand_man_t sh_dnx_l3_fec_man = {
    "Egress interface FEC tests",
    "Tests related to bcm_l3_egress_* APIs and actions with BCM_L3_INGRESS_ONLY flag"
};
/** L3 intf test details */
static sh_sand_man_t sh_dnx_l3_ing_intf_man = {
    "L3 ingress interface tests",
    "Tests related to bcm_l3_intf_* and bcm_l3_ingress_ APIs and actions"
};

/** L3 ARP test details */
static sh_sand_man_t sh_dnx_l3_arp_man = {
    "Egress interface ARP tests",
    "Tests related to bcm_l3_egress_* APIs and actions with BCM_L3_EGRESS_ONLY flag."
};
/** L3 route_basic test details */
static sh_sand_man_t sh_dnx_l3_ip_route_basic_man = {
    "IP Route basic configuration",
    "IP Route basic ctest"
};

/** SLLB */
static sh_sand_man_t sh_dnx_l3_sllb_test_man = {
    "SLLB tests",
    "Tests related to bcm_l3_egress_ecmp* for Virtual IP ECMP groups."
};


/**
 * \brief
 * Ip Route basic ctest
 */
static shr_error_e
sh_dnx_l3_ip_route_basic_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_switch_fec_property_config_t fec_config;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    bcm_port_mapping_info_t mapping_info_in_port;
    bcm_port_mapping_info_t mapping_info_out_port;
    bcm_port_interface_info_t interface_info;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t ingress_rif;
    bcm_port_t in_port = 200;
    bcm_port_t out_port = 202;
    bcm_l3_egress_t l3eg;
    bcm_l3_route_t l3rt;
    bcm_l3_host_t host;
    int kaps_result;
    int intf_in = 15;
    int intf_out = 100;
    int vrf = 1;
    int encap_id = 0x1384;
    int type_vsi;
    int type_vrf;
    int forward_code_ipv4;
    int pph_type;
    int lif_extension_type;
    int match_count;
    int eth_layer_protocol;
    int ipv4_layer_protocol;
    int intf_port = 0xc0000 + out_port;
    int system_headers_mode;
    int physical_port_type;

    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */

    uint32 route = 0x7fffff00;
    uint32 host_addr = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    uint32 in_ttl = 80;
    uint32 after_eth_offset = 0xe;
    uint32 dip_exp = 0x7fff02;
    uint32 fwd_destination;
    uint32 flags;
    uint32 out_intf = 0x64;
    uint32 masked_out_port = out_port & 0xff;
    uint32 signal_values[] = { 0x02, 0x10, 0x24, 0, 0, 0, 0, 1, 1,
        0x2, 0, 0, 0, 0, 0, 0x1, 0x9a, 0x14,
        0x1384, 0xc01384, 1, 1, 1, 0, 0x12,
        0
    };

    char return_value[DSIG_MAX_SIZE_STR];
    char *smac = "00:00:07:00:01:00";
    char *dmac = "00:0c:00:02:00:00";
    char *exp_smac = "00:12:34:56:78:9a";
    char *exp_dmac = "00:00:00:00:cd:1d";
    char dip[RHNAME_MAX_SIZE] = "127.255.255.2";
    char sip[RHNAME_MAX_SIZE] = "192.128.1.1";
    rhhandle_t packet_tx = NULL;
    rhhandle_t packet_rx = NULL;

    SHR_FUNC_INIT_VARS(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
    kaps_result = fec_config.start;
    fwd_destination = BCM_L3_ITF_VAL_GET(kaps_result);

    /** Set In-Port to In ETh-RIF*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = intf_in;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_add(unit, intf_in, in_port, 0), "Test failed, bcm_vlan_gport_add failed.\n");

    /** Set Out-Port default properties, in case of ARP+AC no need*/
    bcm_vlan_port_t_init(&vlan_port);
    bcm_port_match_info_t_init(&match_info);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = out_port;
    SHR_CLI_EXIT_IF_ERR(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info),
                        "Test failed, bcm_port_match_add failed.\n");

    /*
     * Create ETH-RIF and set its properties
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_in_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_in;
    l3if.l3a_vid = intf_in;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed, bcm_l3_intf_create failed.\n");

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_out_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_out;
    l3if.l3a_vid = intf_out;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed, bcm_l3_intf_create failed.\n");

    /** Set Incoming ETH-RIF properties*/
    bcm_l3_ingress_t_init(&ingress_rif);
    ingress_rif.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_rif.vrf = vrf;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_ingress_create(unit, &ingress_rif, &intf_in),
                        "Test failed, bcm_l3_ingress_create failed.\n");

    /** Create ARP and set its properties*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, arp_next_hop_mac, sizeof(bcm_mac_t));
    l3eg.encap_id = encap_id;
    l3eg.vlan = intf_out;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, NULL),
                        "Test failed, bcm_l3_egress_create failed.\n");
    encap_id = l3eg.encap_id;

    /*
     * Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = intf_out;
    l3eg.encap_id = encap_id;
    l3eg.port = out_port;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &kaps_result),
                        "Test failed, bcm_l3_egress_create failed.\n");

    /** Add Route entry*/
    bcm_l3_route_t_init(&l3rt);
    l3rt.l3a_subnet = route;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = kaps_result;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_route_add(unit, &l3rt), "Test failed, bcm_l3_route_add failed.\n");

    /**Add host entry*/
    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = host_addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = kaps_result;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_add(unit, &host), "Test failed, bcm_l3_host_add failed.\n");

    dnx_dbal_fields_enum_value_get(unit, "FODO_ENCODING", "VSI", &type_vsi);
    dnx_dbal_fields_enum_value_get(unit, "FODO_ENCODING", "VRF", &type_vrf);
    dnx_dbal_fields_enum_value_get(unit, "EGRESS_FWD_CODE", "IPV4_UC_R0", &forward_code_ipv4);
    dnx_dbal_fields_enum_value_get(unit, "PPH_TYPE", "PPH_BASE_PLUS_TS", &pph_type);
    dnx_dbal_fields_enum_value_get(unit, "LIF_EXTENSION_TYPE", "1xOUT_LIF", &lif_extension_type);
    dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "ETHERNET", &eth_layer_protocol);
    dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "IPV4", &ipv4_layer_protocol);
    dnx_dbal_fields_enum_value_get(unit, "DESTINATION_ENCODING", "PORT_ID", &physical_port_type);

    /** Allocate and init packet_tx.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info_in_port));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, out_port, &flags, &interface_info, &mapping_info_out_port));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                    (unit, packet_tx, "PTCH_2.PP_SSP", &mapping_info_in_port.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ETH0"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ETH0.DA", dmac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ETH0.SA", smac));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "IPv4.SIP", sip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "IPv4.DIP", dip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "IPv4.TTL", &in_ttl, 8));

    dnx_visibility_resume(unit, BCM_CORE_ALL,
                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);

    /** Send packet */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_tx, SAND_PACKET_RX));
    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));

    in_ttl--;
    /** Check the received packet.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_rx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.DA", exp_dmac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.SA", exp_smac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_rx, "ETH1.VLAN.VID", (uint32 *) &intf_out, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "IPv4.DIP", dip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "IPv4.SIP", sip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_rx, "IPv4.TTL", &in_ttl, 8));

    SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_rx, &match_count));
    if (match_count == 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
    }

    /**Validate signals*/
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Offsets.0", &signal_values[0], 1,
                         NULL, return_value, 0), "Test failed , layer_offsets.0 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Offsets.1", &signal_values[1], 1,
                         NULL, return_value, 0), "Test failed , layer_offsets.1 \n.");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Offsets.2", &signal_values[2], 1,
                         NULL, return_value, 0), "Test failed , layer_offsets.2 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Protocols.0",
                         (uint32 *) &eth_layer_protocol, 1, NULL, return_value, 0),
                        "Test failed , Layer_Protocols.0 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Protocols.1",
                         (uint32 *) &ipv4_layer_protocol, 1, NULL, return_value, 0),
                        "Test failed , Layer_Protocols.1 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Qualifiers.0", &signal_values[3],
                         1, NULL, return_value, 0), "Test failed , Layer_Qualifiers.0 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Layer_Qualifiers.1", &signal_values[4],
                         1, NULL, return_value, 0), "Test failed , Layer_Qualifiers.1 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT5", "", "In_Port", &mapping_info_in_port.pp_port,
                         1, NULL, return_value, 0), "Test failed , In_Port \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "LLR", "", "Incoming_Tag_Exist", &signal_values[5], 1,
                         NULL, return_value, 0), "Test failed , Incoming_Tag_Exist \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "LLR", "", "Vlan_Domain", (uint32 *) &in_port, 1,
                         NULL, return_value, 0), "Test failed , Vlan_Domain \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Fwd_Domain_ID.type",
                         (uint32 *) &type_vrf, 1, NULL, return_value, 0), "Test failed , Fwd_Domain_ID.type \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Fwd_Domain_ID.value", (uint32 *) &vrf, 1,
                         NULL, return_value, 0), "Test failed , Fwd_Domain_ID.value \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Incoming_Tag_Structure",
                         &signal_values[6], 1, NULL, return_value, 0), "Test failed , Incoming_Tag_Structure \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "VTT1", "", "Fwd_Layer_Index", &signal_values[7], 1,
                         NULL, return_value, 0), "Test failed , Fwd_Layer_Index \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "FWD1", "", "Fwd_Domain_ID.type",
                         (uint32 *) &type_vrf, 1, NULL, return_value, 0), "Test failed , Fwd_Domain_ID.type \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "FWD1", "", "Fwd_Domain_ID.value", (uint32 *) &vrf, 1,
                         NULL, return_value, 0), "Test failed , Fwd_Domain_ID.value \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "FWD1", "", "Fwd_Layer_Index", &signal_values[8], 1,
                         NULL, return_value, 0), "Test failed , Fwd_Layer_Index \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "LBP", "", "TM_Cmd.Fwd_Action_Dst",
                         (uint32 *) &intf_port, 1, NULL, return_value, 0), "Test failed , TM_Cmd.Fwd_Action_Dst \n");

    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ERPP", "Fabric", "",
                             "FTMH_Multicast_ID_or_MC_REP_IDX_or_OutLIF_0", &out_intf, 1, NULL, return_value, 0),
                            "Test failed , FTMH_Multicast_ID_or_MC_REP_IDX_or_OutLIF_0 \n");
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ERPP", "Fabric", "", "FTMH_PP_DSP", &masked_out_port, 1,
                             NULL, return_value, 0), "Test failed , FTMH_PP_DSP \n");
    }
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "FER", "", "All_Out_LIFs.0", &out_intf, 1, NULL,
                         return_value, 0), "Test failed , All_Out_LIFs.0 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "LBP", "", "Fwd_Action_Dst.type",
                         (uint32 *) &physical_port_type, 1, NULL, return_value, 0),
                        "Test failed , Fwd_Action_Dst.type \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "LBP", "", "Fwd_Action_Dst.value",
                         (uint32 *) &out_port, 1, NULL, return_value, 0), "Test failed , Fwd_Action_Dst.value \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "LBP", "", "Bytes_To_Remove", &signal_values[9], 1,
                         NULL, return_value, 0), "Test failed , Bytes_To_Remove \n");
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "", "ETParser",
                             "FTMH_Multicast_ID_or_MC_REP_IDX_or_OutLIF_0", &out_intf, 1, NULL, return_value, 0),
                            "Test failed , FTMH_Multicast_ID_or_MC_REP_IDX_or_OutLIF_0 \n");
    }
    else
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "ETParser", "", "CUD_Out_LIF_or_MCDB_Ptr",
                             &out_intf, 1, NULL, return_value, 0), "Test failed , CUD_Out_LIF_or_MCDB_Ptr \n");
    }
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "Fabric_or_Egress_MC",
                         &signal_values[10], 1, NULL, return_value, 0), "Test failed , Fabric_or_Egress_MC \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "fall2bridge_indication",
                         &signal_values[11], 1, NULL, return_value, 0), "Test failed , fall2bridge_indication \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "Fwd_Code",
                         (uint32 *) &forward_code_ipv4, 1, NULL, return_value, 0), "Test failed , Fwd_Code \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Qualifiers.0",
                         &signal_values[12], 1, NULL, return_value, 0), "Test failed , EPP_Layer_Qualifiers \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Qualifiers.1",
                         &signal_values[13], 1, NULL, return_value, 0), "Test failed , EPP_Layer_Qualifiers.1 \n");

    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Protocols.0",
                             (uint32 *) &eth_layer_protocol, 1, NULL, return_value, 0),
                            "Test failed , EPP_Layer_Protocols.0 \n");
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Protocols.1",
                             (uint32 *) &ipv4_layer_protocol, 1, NULL, return_value, 0),
                            "Test failed , EPP_Layer_Protocols.1 \n");
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Offsets.0",
                             &signal_values[14], 1, NULL, return_value, 0), "Test failed , EPP_Layer_Offsets.0 \n");
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Offsets.1",
                             (uint32 *) &after_eth_offset, 1, NULL, return_value, 0),
                            "Test failed , EPP_Layer_Offsets.1 \n");
    }
    else
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Protocols.0",
                             (uint32 *) &ipv4_layer_protocol, 1, NULL, return_value, 0),
                            "Test failed , EPP_Layer_Protocols.0 \n");
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "EPP_Layer_Offsets.0",
                             &after_eth_offset, 1, NULL, return_value, 0), "Test failed , EPP_Layer_Offsets.0 \n");
    }
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Term", "", "VSDS.My_Mac_Prefix_1",
                         &signal_values[15], 1, NULL, return_value, 0), "Test failed , My_Mac_Prefix_1 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Term", "", "VSDS.My_Mac_LSBs_1", &signal_values[16],
                         1, NULL, return_value, 0), "Test failed , VSDS.My_Mac_LSBs_1 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "FWD", "", "Dst_IP", &dip_exp, 1, NULL, return_value,
                         0), "Test failed , Dst_IP \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "FWD", "", "Current_Protocol_Type",
                         (uint32 *) &ipv4_layer_protocol, 1, NULL, return_value, 0),
                        "Test failed , Current_Protocol_Type \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "FWD", "", "Fwd_Bytes_to_Add", &signal_values[17], 1,
                         NULL, return_value, 0), "Test failed , Fwd_Bytes_to_Add \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "FWD2", "IPMF1", "Fwd_Action_Dst",
                         (uint32 *) &fwd_destination, 1, NULL, return_value, 0), "Test failed , Fwd_Action_Dst \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_in_port.core, "IRPP", "IPMF1", "FER", "Fwd_Action_Dst",
                         (uint32 *) &fwd_destination, 1, NULL, return_value, 0), "Test failed , Fwd_Action_Dst \n");
    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_in_port.core, "IRPP", "FER", "", "All_Out_LIFs.1", &signal_values[18],
                             1, NULL, return_value, 0), "Test failed , All_Out_LIFs.1 \n");
    }
    else
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                            (unit, mapping_info_in_port.core, "IRPP", "FER", "", "EEI", &signal_values[19], 1, NULL,
                             return_value, 0), "Test failed , EEI \n");
    }
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Term", "", "ETPS_Valid.1", &signal_values[20], 1,
                         NULL, return_value, 0), "Test failed , ETPS_Valid.1 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Term", "", "ETPS_Valid.2", &signal_values[21], 1,
                         NULL, return_value, 0), "Test failed , ETPS_Valid.2 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Term", "", "ETPS_Valid.3", &signal_values[22], 1,
                         NULL, return_value, 0), "Test failed , ETPS_Valid.3 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Term", "", "ETPS_Valid.4", &signal_values[23], 1,
                         NULL, return_value, 0), "Test failed , ETPS_Valid.4 \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Enc5", "", "Enc_Bytes_to_Add", &signal_values[24],
                         1, NULL, return_value, 0), "Test failed , Enc_Bytes_to_Add \n");
    SHR_CLI_EXIT_IF_ERR(sand_signal_verify
                        (unit, mapping_info_out_port.core, "ETPP", "Enc5", "", "Fwd_Action_Profile", &signal_values[25],
                         1, NULL, return_value, 0), "Test failed , Fwd_Action_Profile \n");

    SHR_CLI_EXIT_IF_ERR(bcm_l3_route_delete(unit, &l3rt), "Test failed");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_delete(unit, &host), "Test failed");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_delete_all(unit), "Test failed. Could not delete ETH-RIF second attempt\n");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_destroy(unit, kaps_result), "Test failed,destroy fec");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_destroy(unit, encap_id), "Test failed, destroy arp");
    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "Route basic test end.\nResult: PASS!\n")));
exit:
    diag_sand_packet_free(unit, packet_tx);
    diag_sand_packet_free(unit, packet_rx);
    SHR_FUNC_EXIT;

}

sh_sand_cmd_t dnx_l3_test_cmds[] = {
    {"Performance", NULL, dnx_l3_performance_cmds, NULL, &sh_dnx_l3_performance_man, NULL, NULL, CTEST_UM},
    {"fec", NULL, dnx_l3_fec_test_cmds, NULL, &sh_dnx_l3_fec_man, NULL, NULL, CTEST_UM},
    {"ecmp", NULL, dnx_ecmp_test_cmds, NULL, &sh_dnx_l3_ecmp_man, NULL, NULL, CTEST_UM},
    {"rif", NULL, dnx_l3_ing_intf_test_cmds, NULL, &sh_dnx_l3_ing_intf_man, NULL, NULL, CTEST_UM},
    {"arp", NULL, dnx_l3_arp_test_cmds, NULL, &sh_dnx_l3_arp_man, NULL, NULL, CTEST_UM},
    {"route_basic", sh_dnx_l3_ip_route_basic_cmd, NULL, NULL, &sh_dnx_l3_ip_route_basic_man, NULL, NULL},
    {"sllb", NULL, dnx_l3_sllb_test_cmds, NULL, &sh_dnx_l3_sllb_test_man, NULL, NULL},
    {NULL}
};
