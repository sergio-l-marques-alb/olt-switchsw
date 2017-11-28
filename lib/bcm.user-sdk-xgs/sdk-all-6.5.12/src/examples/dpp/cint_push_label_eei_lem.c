/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* chenr 13/04/2017
 * 
 * Function to push label on EEI in LEM
 * Tested on Jericho, SDK 6.5.8
 * Code sequence:
 * cint_push_label_eei_lem.c
 * c
 * int unit = 0;
 * push_label_eei(unit);
 * exit;
 * debug bcm rx ver
 * tx 1 psrc=200 data=000c00020000778899001a228100006408004501002e000040004006b7c111111111226645013344556600a1a2a300b1b2b3500f01f57dac0000f5f5f5f5f5f5f5f5f5f5
 * 
 * Expected :
 * One packet should be recieved with label 30, EXP and TTL from push profile configuration (EXP = 3, TTL = 30)
 */

void push_label_eei(int unit)
{
    int rv = BCM_E_NONE;
    int vrf = 100;
    int fecId;
    bcm_l3_intf_t intf;
    bcm_mac_t my_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}; /* my-MAC */
    bcm_mac_t out_mac = {0x00, 0x45, 0x45, 0x45, 0x45, 0x00};
    bcm_l3_egress_t l3eg;
    bcm_l3_host_t l3host;
    uint32 IPaddr = 0x22664501;
    int out_port = 202;
    int flags = 0;
    int label = 30;
    int eei=0;
    int mpls_port_push_profile = 2;
    bcm_mpls_port_t mpls_port_push;
    int exp = 3;
    int ttl = 30;


    bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, 1); /* Allow Pipe mode - EXP_SET and TTL_SET */

    bcm_mpls_port_t_init(&mpls_port_push);
    mpls_port_push.flags = BCM_MPLS_PORT_WITH_ID;
    mpls_port_push.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_port_push.mpls_port_id = mpls_port_push_profile;
    mpls_port_push.egress_label.exp = exp;
    mpls_port_push.egress_label.ttl = ttl;
    mpls_port_push.flags2 = BCM_MPLS_PORT2_TUNNEL_PUSH_INFO;

    bcm_mpls_port_add(unit, 0, &mpls_port_push);

    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, my_mac_address, 6); /* My-MAC */
    intf.l3a_vid = vrf;                               /* VSI ID which is associated with the RIF */
    intf.l3a_vrf = vrf;                               /* VRF ID */
    intf.l3a_intf_id = vrf;                           /* RIF ID */
    rv = bcm_l3_intf_create(unit, intf);

    /* Create EEDB entry*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, out_mac, 6); /* Packet will get out_mac when leaving the device*/

    l3eg.vlan = 100;                       /* Out vlan*/
    l3eg.port = out_port;                  /* Out port */
    l3eg.flags = BCM_L3_EGRESS_ONLY;

    rv = bcm_l3_egress_create(unit, 0, &l3eg, &fecId);

    /* Add label details for Push label on EEI */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = IPaddr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fecId;
    l3host.l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(eei, label, mpls_port_push_profile);
    BCM_FORWARD_ENCAP_ID_VAL_SET(l3host.encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI, BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, eei);
    rv = bcm_l3_host_add(unit, &l3host);
}
