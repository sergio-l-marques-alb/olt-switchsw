/*
 * $Id: ctest_dnx_rx.c
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    ctest_dnx_rx.c
 * Purpose: Ctest infrastructure for RX module tests.
 */
 /*
  * Include files.
  * {
  */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/bsl.h>
#include <bcm_int/dnx/rx/rx_trap_map.h>
#include <bcm/tunnel.h>
#include "ctest_dnx_rx_trap_protocol.h"
#include "ctest_dnx_rx_trap_prog.h"
#include "ctest_dnx_rx_trap_ingress_semantic.h"
#include "ctest_dnx_rx_trap_mtu.h"
#include "ctest_dnx_rx_trap_erpp.h"
#include "ctest_dnx_rx_trap_etpp.h"
#include "ctest_dnx_rx_trap_lif.h"
#include "ctest_dnx_rx_trap_egress_exhaustive.h"
#include "ctest_dnx_rx_trap_svtag.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
/*
 * }
 */

/* *INDENT-OFF* */
/**
 * \brief DNX trap Tests
 * List of trap ctests modules.
 */
sh_sand_cmd_t dnx_rx_trap_test_cmds[] = {
    {"protocol", sh_dnx_rx_trap_protocol_cmd, NULL, sh_dnx_rx_trap_protocol_options, &sh_dnx_rx_trap_protocol_man, NULL, sh_dnx_rx_trap_protocol_tests},
    {"ingress_sem", sh_dnx_rx_trap_ingress_sem_cmd, NULL, sh_dnx_rx_trap_ingress_sem_options, &sh_dnx_rx_trap_ingress_sem_man,   NULL, sh_dnx_rx_trap_ingress_sem_tests},
    {"mtu", sh_dnx_rx_trap_mtu_cmd, NULL, sh_dnx_rx_trap_mtu_options, &sh_dnx_rx_trap_mtu_man, NULL, sh_dnx_rx_trap_mtu_tests},
    {"svtag", sh_dnx_rx_trap_svtag_cmd, NULL, sh_dnx_rx_trap_svtag_options, &sh_dnx_rx_trap_svtag_man, NULL, sh_dnx_rx_trap_svtag_tests},
    {"erpp", sh_dnx_rx_trap_erpp_cmd, NULL, sh_dnx_rx_trap_erpp_options, &sh_dnx_rx_trap_erpp_man, NULL, sh_dnx_rx_trap_erpp_tests},
    {"etpp", sh_dnx_rx_trap_etpp_cmd, NULL, sh_dnx_rx_trap_etpp_options, &sh_dnx_rx_trap_etpp_man, NULL, sh_dnx_rx_trap_etpp_tests},
    {"lif", sh_dnx_rx_trap_lif_cmd, NULL, sh_dnx_rx_trap_lif_options, &sh_dnx_rx_trap_lif_man, NULL, sh_dnx_rx_trap_lif_tests},
    {"egress", sh_dnx_rx_trap_egress_exhaustive_cmd, NULL, NULL, &sh_dnx_rx_trap_egress_exhaustive_man, NULL, sh_dnx_rx_trap_egress_exhaustive_tests},
    {"prog", sh_dnx_rx_trap_prog_cmd, NULL, sh_dnx_rx_trap_prog_options, &sh_dnx_rx_trap_prog_man, NULL, sh_dnx_rx_trap_prog_tests},
    {NULL}
};
/* *INDENT-ON* */

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_compare_values(
    int unit,
    void *set_value_p,
    void *get_value_p,
    uint32 size_of_value,
    char *value_name)
{
    int *set_p = (int *) set_value_p;
    int *get_p = (int *) get_value_p;
    SHR_FUNC_INIT_VARS(unit);

    if (sal_memcmp(set_value_p, get_value_p, size_of_value))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Set and Get %s values are not equal.\n Set:%d\n Get:%d\n", value_name,
                     *set_p, *get_p);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_create_and_set(
    int unit,
    bcm_rx_trap_t trap_type,
    int *trap_id_p,
    bcm_rx_trap_config_t * trap_config_p)
{
    dnx_rx_trap_map_type_t predefined_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&predefined_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, trap_type, trap_id_p));
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, *trap_id_p, trap_config_p));

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_map_info_get(unit, trap_type, &predefined_trap_info));
    LOG_INFO_EX(BSL_LOG_MODULE, "Created and set bcmRxTrap%s Trap. trap_id=%d \n %s%s",
                predefined_trap_info.trap_name, *trap_id_p, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_destroy(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));

    LOG_INFO_EX(BSL_LOG_MODULE, "Trap destroy. trap_id=%d \n %s%s%s", trap_id, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_profile_set(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_gport_t trap_gport)
{
    dnx_rx_trap_map_type_t appl_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_type, &appl_trap_info));
    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Set on Trap: bcmRxTrap%s \n %s%s%s",
                appl_trap_info.trap_name, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, trap_type, trap_gport));

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_profile_clear(
    int unit,
    bcm_rx_trap_t trap_type)
{
    dnx_rx_trap_map_type_t appl_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_clear(unit, trap_type));

    SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_type, &appl_trap_info));
    LOG_INFO_EX(BSL_LOG_MODULE, "Clearing Action profile for trap: bcmRxTrap%s \n %s%s%s",
                appl_trap_info.trap_name, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_inrif_create_example(
    int unit,
    bcm_if_t * rif_p)
{
    bcm_tunnel_terminator_t tunnel_term;
    bcm_vrf_t vrf = 1;
    bcm_l3_ingress_t ing_intf;
    bcm_if_t intf_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = 0xabcdef;
    tunnel_term.dip_mask = 0xffffff;
    tunnel_term.sip = 0xefcdab;
    tunnel_term.sip_mask = 0xffffff;
    tunnel_term.vrf = vrf;
    tunnel_term.type = bcmTunnelTypeGreAnyIn4;

    SHR_IF_ERR_EXIT(bcm_tunnel_terminator_create(unit, &tunnel_term));

    /*
     * In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API
     */
    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID;    /* must, as we update exist RIF */
    ing_intf.vrf = vrf;

    /*
     * Convert tunnel's GPort ID to intf ID
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &ing_intf, &intf_id));

    *rif_p = intf_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_eth_inrif_create_example(
    int unit,
    bcm_if_t * rif_p)
{
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_intf_t l3if;
    bcm_mac_t my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    /*
     * My-MAC
     */
    sal_memcpy(&l3if.l3a_mac_addr, &my_mac, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = l3if.l3a_vid = *rif_p;
    /*
     * set qos map id to 0 as default
     */
    l3if.dscp_qos.qos_map_id = 0;

    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = l3if.l3a_vid;
    l3_ing_if.qos_map_id = 0;

    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &l3_ing_if, &l3if.l3a_intf_id));

    *rif_p = l3if.l3a_intf_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_outrif_create_example(
    int unit,
    bcm_if_t * rif_p)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_l3_intf_t l3_intf;
    bcm_mac_t da = { 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a };
    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_t_init(&l3eg);

    sal_memcpy(l3eg.mac_addr, da, sizeof(bcm_mac_t));
    l3eg.encap_id = 0;
    l3eg.vlan = 100;
    l3eg.flags = 0;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null));

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = 0xabcdef;
    tunnel_init.sip = 0xefcdab;
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type = bcmTunnelTypeGreAnyIn4;
    tunnel_init.ttl = 64;
    tunnel_init.l3_intf_id = l3eg.encap_id;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel_init));

    *rif_p = l3_intf.l3a_intf_id;

exit:
    SHR_FUNC_EXIT;

}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_inlif_create_example(
    int unit,
    bcm_gport_t * lif_p)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    bcm_gport_t mpls_port_id_ingress = 9999 - *lif_p;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, mpls_port_id_ingress);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.encap_id = 8888 - *lif_p;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 5000 - *lif_p;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, 50000 - *lif_p);

    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, 0, &mpls_port));

    *lif_p = mpls_port.mpls_port_id;

exit:
    SHR_FUNC_EXIT;

}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_outlif_create_example(
    int unit,
    bcm_gport_t * lif_p)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, 18888 - *lif_p);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = 5000 - *lif_p;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, 0, &mpls_port));

    *lif_p = mpls_port.mpls_port_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_esem_default_example(
    int unit,
    bcm_gport_t * vlan_port_id_p)
{
    bcm_vlan_port_t vlan_port;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_DEFAULT;

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    *vlan_port_id_p = vlan_port.vlan_port_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_esem_match_example(
    int unit,
    bcm_gport_t * vlan_port_id_p)
{
    bcm_vlan_port_t vlan_port;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.port = 13;
    vlan_port.vsi = 1;

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    *vlan_port_id_p = vlan_port.vlan_port_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_rif_destroy_example(
    int unit,
    bcm_if_t * rif_p,
    bcm_rx_trap_lif_type_t rif_type)
{
    bcm_tunnel_terminator_t tunnel_term;
    bcm_l3_intf_t l3_intf;
    SHR_FUNC_INIT_VARS(unit);

    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = 0xabcdef;
    tunnel_term.dip_mask = 0xffffff;
    tunnel_term.sip = 0xefcdab;
    tunnel_term.sip_mask = 0xffffff;
    tunnel_term.vrf = 1;
    tunnel_term.type = bcmTunnelTypeGreAnyIn4;

    if (rif_type == bcmRxTrapLifTypeInRif)
    {
        SHR_IF_ERR_EXIT(bcm_tunnel_terminator_delete(unit, &tunnel_term));
    }
    else
    {
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = *rif_p;
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_clear(unit, &l3_intf));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_lif_destroy_example(
    int unit,
    bcm_gport_t * lif_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, 0, *lif_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx.h for info
 */
shr_error_e
ctest_dnx_rx_trap_esem_destroy_example(
    int unit,
    bcm_gport_t * vlan_port_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, *vlan_port_id_p));

exit:
    SHR_FUNC_EXIT;
}
