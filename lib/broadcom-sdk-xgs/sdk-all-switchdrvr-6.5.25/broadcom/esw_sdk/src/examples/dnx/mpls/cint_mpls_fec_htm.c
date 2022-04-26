/*
 * $Id: cint_mpls_fec_htm.c, Exp $
 $Copyright: (c) 2021 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * 
 * Configuration:
 * 
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c  
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c   
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c                                       
 * cint ../../../../src/examples/dnx/mpls/cint_mpls_fec_htm.c                                       
 * cint                                                                   
 * int unit = 0; 
 * int rv = 0; 
 * int port1 = 200; 
 * int port2 = 201;
 * int port3 = 202;
 * int port4 = 203;
 * int mode = 0;
 * rv = mpls_fec_htm_main(unit,port1,port2,port3,port4,mode);
 * print rv; 
 * 
 * 
 * This cint configures Heirarchical TM-FLOW(HTM) field in FEC entry where DESTINATION field is of type FEC.
 * 3 FEC entries configured, one for each FEC level.
 * Generate various ETHoMPLSoIPv4 packet to perform MPLS forwarding based on matching different FEC levels.
 *
 *                                                           +--------------------------------------------------------------+
 *                                                           |                                +---------------------------+ |
 *      ILM(labels)               FEC level 1                |      FEC level 2               |      FEC level 3          | |      EEDB (labels)
 *  +-------------+            +---------------------+       |   +---------------------+      |   +--------------------+  | |   +--------------+
 *  |   0x1111    +----------> +GLOBAL OUTLIF        +-------+   |GLOBAL OUTLIF        +------+   |GLOBAL OUTLIF       +-+| +--->   0x2221     |
 *  |   0x1112    +-------v    |  DESTINATION(port2) +----------->  DESTINATION(port3) +---------->  DESTINATION(port4)| |+----->   0x2222     |
 *  |   0x1113    +----v  |    |     HTM             |           |    HTM              |          |                    | +------>   0x2223     |
 *  +-------------+    |  |    +---------------------+           +------^--------------+          +--------^-----------+        +--------------+
 *                     |  +---------------------------------------------+                                  |
 *                     +-----------------------------------------------------------------------------------+
 *
 * Scenarios configured in this cint:
 * Configure desired HTM configuration on FEC entries.
 * First encountered HTM overrides following ones and the DESTINATION field, thus changing the outgoing port in current scenarios.
 * Once FEC configuration is done generate 3 packet:
 *
 *  Match FEC level 1:
 *  ##############################################################################################
 *  Sending packet from port == port1 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+
 *   |    |      DA         | SA              ||   MPLS     ||  SIP          ||  DIP          ||
 *   |    |00:0C:00:02:00:01|00:0C:00:02:00:00||Label:0x1111||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on    <------------
 *		if 'mode' == MPLS_FEC_HTM_LVL1_HTM_LVL2_HTM or MPLS_FEC_HTM_LVL1_HTM_LVL2_NO_HTM
 *			port == port2
 *		else if 'mode' == MPLS_FEC_HTM_LVL1_NO_HTM_LVL2_HTM
 *			port == port3
 *		else
 *			port == port4
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         | SA              ||   MPLS     ||   MPLS     ||   MPLS     ||  SIP          ||  DIP          ||
 *   |    |00:0c:00:02:00:03|00:0c:00:02:00:02||Label:0x2221||Label:0x2222||Label:0x2223||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  Match FEC level 2:
 *  ##############################################################################################
 *  Sending packet from port == port1 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS     ||  SIP          ||  DIP          ||
 *   |    |00:0C:00:02:00:01|00:0C:00:02:00:00||Label:0x1112||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on    <------------
 *		if 'mode' == MPLS_FEC_HTM_LVL1_NO_HTM_LVL2_HTM or MPLS_FEC_HTM_LVL1_HTM_LVL2_HTM
 *			port == port3
 *		else
 *			port == port4
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         | SA              ||   MPLS     ||   MPLS     ||  SIP          ||  DIP          ||
 *   |    |00:0c:00:02:00:03|00:0c:00:02:00:02||Label:0x2222||Label:0x2223||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 * 
 *  Match FEC level 3:
 *  ##############################################################################################
 *  Sending packet from port == port1 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS     ||  SIP          ||  DIP          ||
 *   |    |00:0C:00:02:00:01|00:0C:00:02:00:00||Label:0x1113||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == port4:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+
 *   |    |      DA         | SA              ||   MPLS     ||  SIP          ||  DIP          ||
 *   |    |00:0c:00:02:00:03|00:0c:00:02:00:02||Label:0x2223||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 */

bcm_mac_t mpls_fec_htm_mac_in_port = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
bcm_mac_t mpls_fec_htm_exp_smac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x02 };
bcm_mac_t mpls_fec_htm_exp_dmac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x03 };


int MPLS_FEC_HTM_MIN_MODE = 0;
int MPLS_FEC_HTM_MAX_MODE = 3;
int MPLS_FEC_HTM_NO_HTM = MPLS_FEC_HTM_MIN_MODE;
int MPLS_FEC_HTM_LVL1_HTM_LVL2_HTM = 1;
int MPLS_FEC_HTM_LVL1_HTM_LVL2_NO_HTM = 2;
int MPLS_FEC_HTM_LVL1_NO_HTM_LVL2_HTM = MPLS_FEC_HTM_MAX_MODE;


int
init_mpls_fec_htm_params(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mode)
{
    int rv = BCM_E_NONE;
    int htm1, htm2;

    mpls_util_s_init(mpls_util_entity, MPLS_UTIL_MAX_NOF_ENTITIES);

    rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, out_port1, &htm1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in appl_dnx_e2e_scheme_logical_port_to_base_voq_get for htm1\n", rv);
        return rv;
    }

    rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, out_port2, &htm2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in appl_dnx_e2e_scheme_logical_port_to_base_voq_get for htm2\n", rv);
        return rv;
    }

    printf("received TM_FLOWs(VOQ):)\n");
    printf("    port %d -> VOQ %d\n", out_port1, htm1);
    printf("    port %d -> VOQ %d\n", out_port2, htm2);

    /*
     * l2, mac, arp, fec parameters
     */
    /******************ETH termination*********************/
    mpls_util_entity[0].ports[0].port = in_port;
    mpls_util_entity[0].ports[0].eth_rif_id = &mpls_util_entity[0].rifs[0].intf;

    mpls_util_entity[0].rifs[0].intf = 30;
    mpls_util_entity[0].rifs[0].mac_addr = mpls_fec_htm_mac_in_port;
    mpls_util_entity[0].rifs[0].vrf = 1;

    /*******************FEC level 3 flow configuration - mpls forwarding, fec {dest(port), outlif}, mpls encapsulation********************/
    mpls_util_entity[0].mpls_switch_tunnel[0].label = 0x1113;
    mpls_util_entity[0].mpls_switch_tunnel[0].egress_if = &mpls_util_entity[0].fecs[0].fec_id;
    mpls_util_entity[0].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_util_entity[0].mpls_switch_tunnel[0].flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    mpls_util_entity[0].fecs[0].fec_id = 0;
    mpls_util_entity[0].fecs[0].flags|= BCM_L3_3RD_HIERARCHY;
    mpls_util_entity[0].fecs[0].port = &mpls_util_entity[1].ports[0].port;
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    mpls_util_entity[0].fecs[0].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;

    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = 0x2223;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[1].arps[0].arp;
    mpls_util_entity[0].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel1;
    mpls_util_entity[0].mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    mpls_util_entity[1].ports[0].port = out_port3;
    mpls_util_entity[1].ports[0].eth_rif_id = &mpls_util_entity[1].rifs[0].intf;

    mpls_util_entity[1].rifs[0].intf = 33;
    mpls_util_entity[1].rifs[0].mac_addr = mpls_fec_htm_exp_smac;
    mpls_util_entity[1].rifs[0].vrf = 2;

    mpls_util_entity[1].arps[0].arp = 0;
    mpls_util_entity[1].arps[0].next_hop = mpls_fec_htm_exp_dmac;
    mpls_util_entity[1].arps[0].intf = &mpls_util_entity[1].rifs[0].intf;

    /*******************FEC level 2 flow configuration - mpls forwarding, fec {dest(port/fec), outlif, HTM(optional))}, mpls encapsulation********************/
    mpls_util_entity[0].mpls_switch_tunnel[1].label = 0x1112;
    mpls_util_entity[0].mpls_switch_tunnel[1].egress_if = &mpls_util_entity[0].fecs[1].fec_id;
    mpls_util_entity[0].mpls_switch_tunnel[1].action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_util_entity[0].mpls_switch_tunnel[1].flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    mpls_util_entity[0].fecs[1].fec_id = 0;
    mpls_util_entity[0].fecs[1].flags|= BCM_L3_2ND_HIERARCHY;
    mpls_util_entity[0].fecs[1].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[1].tunnel_id;
    mpls_util_entity[0].fecs[1].fec = &mpls_util_entity[0].fecs[0].fec_id;
    if ((mode == MPLS_FEC_HTM_LVL1_HTM_LVL2_HTM) ||(mode == MPLS_FEC_HTM_LVL1_NO_HTM_LVL2_HTM))
    {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(mpls_util_entity[0].fecs[1].hierarchical_gport, BCM_CORE_ALL, htm2);
    }

    mpls_util_entity[0].mpls_encap_tunnel[1].label[0] = 0x2222;
    mpls_util_entity[0].mpls_encap_tunnel[1].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel2;
    mpls_util_entity[0].mpls_encap_tunnel[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[1].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    /*******************FEC level 1 flow configuration - mpls forwarding, fec {dest(port/fec), outlif, HTM(optional))}, mpls encapsulation********************/
    mpls_util_entity[0].mpls_switch_tunnel[2].label = 0x1111;
    mpls_util_entity[0].mpls_switch_tunnel[2].egress_if = &mpls_util_entity[0].fecs[2].fec_id;
    mpls_util_entity[0].mpls_switch_tunnel[2].action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_util_entity[0].mpls_switch_tunnel[2].flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    mpls_util_entity[0].fecs[2].fec_id = 0;
    mpls_util_entity[0].fecs[2].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[2].tunnel_id;
    mpls_util_entity[0].fecs[2].fec = &mpls_util_entity[0].fecs[1].fec_id;
    if ((mode == MPLS_FEC_HTM_LVL1_HTM_LVL2_HTM) ||(mode == MPLS_FEC_HTM_LVL1_HTM_LVL2_NO_HTM))
    {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(mpls_util_entity[0].fecs[2].hierarchical_gport, BCM_CORE_ALL, htm1);
    }

    mpls_util_entity[0].mpls_encap_tunnel[2].label[0] = 0x2221;
    mpls_util_entity[0].mpls_encap_tunnel[2].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[2].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[2].encap_access = bcmEncapAccessTunnel3;
    mpls_util_entity[0].mpls_encap_tunnel[2].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    mpls_util_entity[0].mpls_encap_tunnel[2].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;

    /***************************************/

    mpls_params_set = 1;

    return rv;
}

int
mpls_fec_htm_main(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mode)
{
    int rv = BCM_E_NONE;

    if ((mode < MPLS_FEC_HTM_MIN_MODE) || (mode > MPLS_FEC_HTM_MAX_MODE))
    {
        printf("Error, provided unsupported mode(%d), valid is %d-%d\n", mode, MPLS_FEC_HTM_MIN_MODE, MPLS_FEC_HTM_MAX_MODE);
        return BCM_E_PARAM;
    }

    rv = init_mpls_fec_htm_params(unit, in_port, out_port1, out_port2, out_port3, mode);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in init_mpls_fec_htm_params\n", rv);
        return rv;
    }

    rv = mpls_util_main(unit, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    return rv;
}
