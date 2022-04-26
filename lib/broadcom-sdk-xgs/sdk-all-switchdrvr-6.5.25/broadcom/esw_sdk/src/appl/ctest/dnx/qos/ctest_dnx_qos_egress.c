/** \file ctest_dnx_qos_egress.c
 *
 * Tests for QOS egress APIs
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <bcm/l3.h>
#include <bcm/mpls.h>
#include <bcm/vlan.h>
#include <bcm/qos.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <soc/dnx/dnx_visibility.h>

/**
 * \brief Test MPLS PHP qos vis command.
 */
static shr_error_e
dnx_qos_egress_vis_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 in_vid = 100;
    uint32 out_vid = 101;
    uint32 vsi = 10;
    uint32 label = 1000;
    uint32 flags;
    bcm_mac_t my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
    bcm_mac_t next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x64 };
    bcm_port_t in_port = 200;
    bcm_port_t out_port = 202;
    bcm_l3_intf_t l3_intf_in;
    bcm_l3_ingress_t l3_ing_intf;
    bcm_l3_intf_t l3_intf_out;
    bcm_l3_egress_t egr_arp;
    bcm_l3_egress_t egr_fec;
    bcm_if_t intf_id;
    bcm_vlan_port_t vlan_port;
    bcm_mpls_tunnel_switch_t entry;
    int qos_php_map_id = 1;
    int qos_php_map_opcode = 2;
    bcm_qos_map_t qos_php_map;
    int dscp = 0;
    bcm_port_mapping_info_t mapping_info_in_port;
    bcm_port_mapping_info_t mapping_info_out_port;
    bcm_port_interface_info_t interface_info;
    char dip[RHNAME_MAX_SIZE] = "127.255.100.1";
    char sip[RHNAME_MAX_SIZE] = "120.255.255.03";
    uint32 in_ttl = 64;
    rhhandle_t packet_tx = NULL;
    rhhandle_t packet_rx = NULL;
    int match_count;
    char return_value[DSIG_MAX_SIZE_STR];
    char my_mac_str[RHNAME_MAX_SIZE] = "00:0c:00:02:00:01";
    char dest_mac_str[RHNAME_MAX_SIZE] = "00:00:00:00:00:64";
    uint32 nwk_qos = 20;
    uint32 mpls_exp = 3;
    uint32 DP = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, in_port, bcmPortClassId, in_port));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, out_port, bcmPortClassId, out_port));

    /**Create VSI*/
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vsi));

    /** Set vlan port membership*/
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, in_vid, in_port, 0));

    /**Create InLif*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = in_vid;
    vlan_port.port = in_port;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    /**in RIF*/
    bcm_l3_intf_t_init(&l3_intf_in);
    l3_intf_in.l3a_vid = in_vid;
    sal_memcpy(&l3_intf_in.l3a_mac_addr, my_mac, 6);
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3_intf_in));
    bcm_l3_ingress_t_init(&l3_ing_intf);
    SHR_IF_ERR_EXIT(bcm_l3_ingress_get(unit, l3_intf_in.l3a_intf_id, &l3_ing_intf));
    l3_ing_intf.flags |= BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE;
    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &l3_ing_intf, &l3_intf_in.l3a_intf_id));

    /**out RIF*/
    bcm_l3_intf_t_init(&l3_intf_out);
    l3_intf_out.l3a_vid = out_vid;
    sal_memcpy(&l3_intf_out.l3a_mac_addr, my_mac, 6);
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3_intf_out));

    /**ARP*/
    bcm_l3_egress_t_init(&egr_arp);
    egr_arp.port = out_port;
    egr_arp.vlan = out_vid;
    sal_memcpy(&egr_arp.mac_addr, &next_hop_mac, 6);
    egr_arp.flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    flags = BCM_L3_EGRESS_ONLY;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_arp, &intf_id));

    /**FEC*/
    bcm_l3_egress_t_init(&egr_fec);
    egr_fec.intf = egr_arp.encap_id;
    egr_fec.port = out_port;
    egr_fec.qos_map_id = 0;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &egr_fec, &intf_id));

    /**qos mapping*/
    bcm_qos_map_t_init(&qos_php_map);
    SHR_IF_ERR_EXIT(bcm_qos_map_create
                    (unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_WITH_ID, &qos_php_map_id));
    SHR_IF_ERR_EXIT(bcm_qos_map_create
                    (unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                     &qos_php_map_opcode));
    flags = BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_OPCODE;
    qos_php_map.opcode = qos_php_map_opcode;
    SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &qos_php_map, qos_php_map_id));
    for (dscp = 0; dscp < 256; dscp++)
    {
        bcm_qos_map_t_init(&qos_php_map);
        flags = BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_IPV4;
        qos_php_map.int_pri = dscp;
        qos_php_map.remark_int_pri = (dscp + 1) % 255;
        SHR_IF_ERR_EXIT(bcm_qos_map_add(unit, flags, &qos_php_map, qos_php_map_opcode));
    }

    /**ILM PHP entry*/
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    entry.label = 1000;
    entry.port = in_port;
    entry.egress_if = intf_id;
    entry.qos_map_id = qos_php_map_id;
    entry.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    entry.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &entry));

    dnx_visibility_resume(unit, BCM_CORE_ALL,
                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);

    /** Allocate and init packet_tx.*/
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info_in_port));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, out_port, &flags, &interface_info, &mapping_info_out_port));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                    (unit, packet_tx, "PTCH_2.PP_SSP", &mapping_info_in_port.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ETH1.DA", my_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "ETH1.VLAN.VID", &in_vid, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "MPLS"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "MPLS.EXP", &mpls_exp, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "MPLS.LABEL", &label, 20));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "MPLS.TTL", &in_ttl, 8));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "IPv4.SIP", sip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "IPv4.DIP", dip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "IPv4.TTL", &in_ttl, 8));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "IPv4.TOS", &nwk_qos, 8));
    /** Send packet.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_tx, SAND_PACKET_RX));
    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));
    /** Check the received packet.*/
    /*
     * nwk_qos mapping plus 1
     */
    nwk_qos++;
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_rx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.DA", dest_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.SA", my_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_rx, "ETH1.VLAN.VID", &out_vid, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "IPv4.TTL", &in_ttl, 8));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "IPv4.TOS", &nwk_qos, 8));

    SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_rx, &match_count));
    if (match_count == 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
    }

    /**Validate signals*/
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info_out_port.core, "ETPP", "PRP2", "", "Nwk_QOS", (uint32 *) &mpls_exp, 1,
                     NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info_out_port.core, "ETPP", "ETParser", "PRP2", "FTMH_DP", (uint32 *) &DP, 1,
                     NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info_out_port.core, "ETPP", "", "FWD", "Nwk_QOS", (uint32 *) &nwk_qos, 1,
                     NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info_out_port.core, "ETPP", "", "FWD", "FTMH_DP", (uint32 *) &DP, 1,
                     NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info_out_port.core, "ETPP", "", "enc1", "Updated_QOS_and_DP", (uint32 *) &nwk_qos, 1,
                     NULL, return_value, 0));
    /*
     * Cleanup
     */
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_destroy_all(unit), "Test failed. Could not delete vlan_port\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_delete(unit, in_vid, in_port),
                        "Test failed. Could not delete an L2 address entry\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_destroy(unit, vlan_port.vlan_port_id),
                        "Test failed. Could not destroy vlan port\n");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_delete_all(unit), "Test failed. Could not delete L3 interface\n");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_destroy(unit, egr_arp.encap_id), "Test failed. Could not destroy arp\n");
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_destroy(unit, intf_id), "Test failed. Could not destroy fec\n");
    SHR_CLI_EXIT_IF_ERR(bcm_qos_map_destroy(unit, qos_php_map_id), "Test failed. Could not destroy qos map id\n");
    SHR_CLI_EXIT_IF_ERR(bcm_qos_map_destroy(unit, qos_php_map_opcode),
                        "Test failed. Could not destroy qos opcode id\n");
    SHR_CLI_EXIT_IF_ERR(bcm_mpls_tunnel_switch_delete_all(unit), "Test failed. Could not delete tunnel\n");

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "MPLS PHP QOS TEST END.\nResult: PASS!\n")));

exit:
    diag_sand_packet_free(unit, packet_tx);
    diag_sand_packet_free(unit, packet_rx);

    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META("\n\n#@!  Test FAIL  !@#\n\n\n")));
    }
    SHR_FUNC_EXIT;
}

/** Test manual   */
sh_sand_man_t dnx_qos_egress_vis_test_man = {
    .brief = "test qos egress VIS command",
    .full = "create ILM PHP service, and then send traffic" " check egress qos signals",
    .synopsis = "ctest qos egress vis",
};

sh_sand_cmd_t dnx_qos_egress_tests[] = {
    {
     .keyword = "vis",
     .action = dnx_qos_egress_vis_test,
     .options = NULL,
     .man = &dnx_qos_egress_vis_test_man,
     .flags = CTEST_POSTCOMMIT}
    ,
    {NULL}
};
