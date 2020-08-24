/**
 * \file ctest_dnx_mpls_bier.c
 *
 * Tests for MPLS Bier:
 *   * Transit
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLSTESTSDNX

#include <bcm/mpls.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>

/*
* Configure MPLS switch tunnels
* This function must be called separately to handle BCM_MPLS_SWITCH_ACTION_POP and other actions
* 'action_pop' must be set for BCM_MPLS_SWITCH_ACTION_POP
*/
static shr_error_e
mpls_bier_create_switch_tunnel(
    int unit,
    bcm_mpls_label_t label,
    int port,
    int domain,
    bcm_multicast_t mc_id,
    bcm_gport_t * tunnel_id_p)
{
    bcm_mpls_tunnel_switch_t entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = label;
    entry.second_label = BCM_MPLS_LABEL_INVALID;
    entry.flags = BCM_MPLS_SWITCH_ENCAP_SET;
    BCM_GPORT_FORWARD_PORT_SET(entry.port, port);
    entry.egress_label.label = BCM_MPLS_LABEL_INVALID;
    entry.vpn = domain;
    entry.mc_group = mc_id;

    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &entry));

    *tunnel_id_p = entry.tunnel_id;

exit:
    SHR_FUNC_EXIT;
}

/* Configure RIF entry */
static shr_error_e
mpls_bier_create_l3_interface(
    int unit,
    bcm_mac_t my_mac,
    int intf,
    int vrf)
{
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;

    /*
     * My-MAC
     */
    sal_memcpy(l3if.l3a_mac_addr, my_mac, 6);
    l3if.l3a_intf_id = intf;

    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;

    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &l3_ing_if, &l3if.l3a_intf_id));

exit:
    SHR_FUNC_EXIT;
}

/* Configure default egress vlan */
static shr_error_e
mpls_bier_create_default_vlan(
    int unit,
    bcm_gport_t * vlan_default)
{
    bcm_vlan_port_t vlan_port;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    *vlan_default = vlan_port.vlan_port_id;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mpls_bier_configure_port_properties(
    int unit,
    int port,
    int interface,
    bcm_gport_t vlan_default)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Ingress 
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = port;
    vlan_port.vsi = interface;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    bcm_port_match_info_t_init(&match_info);

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = port;
    SHR_IF_ERR_EXIT(bcm_port_match_add(unit, vlan_default, &match_info));

    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, interface, port, 0));

exit:
    SHR_FUNC_EXIT;
}

/*
 * 'BIER Transit' tests
 * {
 */
/**
 * \brief
 *   This function configures BIER MPLS tunnel, send the packet and verifies correct routing
 *
 * \param [in] unit - HW identifier of unit.
 * \param [in] args - Classic Shell Command arguments structure
 * \param [in] sand_control - Control information related to each of the 'options' in framework format
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
dnx_mpls_bier_transit_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mpls_label_t label;
    int port_0, port_1;
    bcm_vlan_t vlan;
    bcm_multicast_t mc_id;
    bcm_gport_t tunnel_id;
    int domain, interface_0, interface_1;
    bcm_mac_t my_mac;
    bcm_gport_t vlan_default = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("MC_Group", mc_id);
    SH_SAND_GET_UINT32("label", label);
    SH_SAND_GET_UINT32("port_0", port_0);
    SH_SAND_GET_UINT32("port_1", port_1);
    SH_SAND_GET_UINT32("vid", vlan);
    SH_SAND_GET_UINT32("domain", domain);
    SH_SAND_GET_UINT32("interface_0", interface_0);
    SH_SAND_GET_UINT32("interface_1", interface_1);
    SH_SAND_GET_MAC("my_mac", my_mac);

    SHR_IF_ERR_EXIT(mpls_bier_create_switch_tunnel(unit, label, port_0, domain, mc_id, &tunnel_id));
    SHR_IF_ERR_EXIT(mpls_bier_create_l3_interface(unit, my_mac, interface_0, vlan));
    SHR_IF_ERR_EXIT(mpls_bier_create_default_vlan(unit, &vlan_default));
    SHR_IF_ERR_EXIT(mpls_bier_configure_port_properties(unit, port_0, interface_0, vlan_default));
    SHR_IF_ERR_EXIT(mpls_bier_configure_port_properties(unit, port_1, interface_1, vlan_default));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'frr add' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
static sh_sand_option_t sh_dnx_mpls_bier_transit_argumenst[] = {
    /* Name */   /* Type */             /* Description */                                          /* Default */
    {"label",    SAL_FIELD_TYPE_UINT32, "Value of label for BIER MPLS Tag",                        "0x3333"},
    {"port_0",   SAL_FIELD_TYPE_UINT32, "Port",                                                    "201"},
    {"port_1",   SAL_FIELD_TYPE_UINT32, "Port",                                                    "203"},
    {"MC_Group", SAL_FIELD_TYPE_UINT32, "Multicast Group ID used by TM to perform BIER multicast", "1024"},
    {"VID",      SAL_FIELD_TYPE_UINT32, "VLAN ID",                                                 "100"},
    {"domain",   SAL_FIELD_TYPE_UINT32, "Bier Domain ID",                                          "101"},
    {"interface_0",SAL_FIELD_TYPE_UINT32, "LE Interface ID",                                       "30"},
    {"interface_1",SAL_FIELD_TYPE_UINT32, "LE Interface ID",                                       "40"},
    {"my_mac",   SAL_FIELD_TYPE_MAC,    "Router MY MAC address",                                   "00:00:00:00:CD:1D"},
    /*
     * End of options list - must be last. 
     */
    {NULL}
};

/**
 * \brief
 *   List of tests for 'action' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_mpls_bier_transit_tests[] = {
    {NULL}
};
/*
 * }
 */

/** Test manual   */
static sh_sand_man_t dnx_mpls_bier_transit_man = {
    .brief = "Verify BIER properties assignment and transit routing"
};

/** List of mpls bier tests   */
sh_sand_cmd_t dnx_mpls_bier_tests[] = {
    {"transit", dnx_mpls_bier_transit_cmd, NULL, sh_dnx_mpls_bier_transit_argumenst, &dnx_mpls_bier_transit_man, NULL, sh_mpls_bier_transit_tests, SH_CMD_SKIP_EXEC},
    {NULL}
};

/* *INDENT-ON* */
