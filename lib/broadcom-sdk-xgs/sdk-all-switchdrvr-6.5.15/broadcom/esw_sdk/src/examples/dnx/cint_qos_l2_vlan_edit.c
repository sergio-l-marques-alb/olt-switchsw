/*
 * $Id: cint_mpls_tandem.c, Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_qos_l2_vlan_it.c Purpose: qos mapping for IVE&EVE (uniform & pipeNextNameSpace model). 
 */

/*
 * Configuration:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/sand/Cint_vswitch_metro_mp_vlan_port.c
 * cint ../../../../src/examples/sand/cint_qos_l3_remark.c
 * cint ../../../../src/examples/dnx/cint_qos_l2_vlan_edit.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int inPort = 200; 
 * int outPort = 201;
 * rv = l2_qos_ive_nop_eve_replace_add_main(unit,inPort,outPort);
 * print rv; 
 *
 */


int ing_qos_gport = -1;
int egr_qos_gport = -1;
int l2_qos_model=0;
uint8 egress_pkt_pri=0;
uint8 egress_pkt_cfi=0;

/*
 * Add the QoS map configurations.
 * For bridging packets, map L2 ETH.PCPDEI to NWK_QOS/IN_DSCP_EXP
 * The ingress mapping relastionship is: NWK_QOS = ETH.PCPDEI + 1
 * The egress mapping relastionship is:  NWK_QOS = NWK_QOS - 2
 */
int
qos_map_l2_pcpdei_config(int unit)
{
    return qos_map_phb_remark_set(unit,
                                  BCM_QOS_MAP_L2_OUTER_TAG,
                                  BCM_QOS_MAP_L2_OUTER_TAG,
                                  BCM_QOS_MAP_L2_OUTER_TAG,
                                  ing_qos_gport,
                                  egr_qos_gport,
                                  7, 6);
}

/*
 * Clear the QoS map configurations.
 */
int
qos_map_l2_pcpdei_clear(int unit)
{
    return qos_map_phb_remark_clear(unit,
                                    BCM_QOS_MAP_L2_OUTER_TAG,
                                    BCM_QOS_MAP_L2_OUTER_TAG,
                                    BCM_QOS_MAP_L2_OUTER_TAG,
                                    ing_qos_gport,
                                    egr_qos_gport,
                                    7, 6);
}


/*
 * Cross connect a inlif to outlif for verifying qos map for EVE.
 * No IVE action is added. In EVE stage, {replace, add} is performed.
 * The incoming packet should be single tagged, and outgoing packet will be double tagged.
 * For outgoing packet:
 *     outer-tag.pcpdei is mapped from nwk_qos.
 *     inner-tag.pcpdei is copied from incoming-tag.pcpei.
 */
int
vlan_translation_action_create_and_set(
    int unit,
    int is_ingress,
    int action_id,
    bcm_vlan_action_t outer_vlan_action,
    bcm_vlan_action_t inner_vlan_action,
    bcm_vlan_action_t outer_prio_action,
    bcm_vlan_action_t inner_prio_action,
    uint16 outer_tpid,
    uint16 inner_tpid)
{
    int rv;
    int flags;
    bcm_vlan_action_set_t action;

    /*
     * Create new Action IDs 
     */
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    flags |= BCM_VLAN_ACTION_SET_WITH_ID;
    rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Fill Action 
     */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_vlan_action;
    action.dt_outer_pkt_prio = outer_prio_action;
    action.dt_inner = inner_vlan_action;
    action.dt_inner_pkt_prio = inner_prio_action;

    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}

int
l2_qos_ive_nop_eve_replace_add_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int egress_outer_vid1 = 500;
    int egress_outer_vid2 = 600;
    int egress_inner_vid1 = 700;
    int egress_inner_vid2 = 800;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };      /* DA */

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize s_c_format */
    rv = port_tpid_class_add(unit, inPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }
    
    /** After IVE recognize s_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. IVE action settings - Nop 
     */
    rv = vlan_translation_action_create_and_set(unit, 1 /* Ingress */, ingress_action_id,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 4. EVE action settings - Replace and add 
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionReplace, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionOuterAdd, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 5. Map LLVP and VLAN Port profile to Action 
     */
    rv = vlan_translate_action_map_set(unit, 1 /* Ingress */ , ingress_action_id,
                                       tag_format_s, in_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }
    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s, out_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    /*
     * 6. VPN create 
     */
    rv = vswitch_create(unit, vsi_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create\n");
        return rv;
    }

    /*
     * 7. Create VLAN-Ports 
     */
    rv = vswitch_vlan_port_create(unit,
                                  in_vlan_port_id,
                                  inPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid1,
                                  match_inner_vid1,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN,
                                  in_vlan_port_edit_prfoile,
                                  egress_outer_vid1,
                                  egress_inner_vid1,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = vswitch_vlan_port_create(unit,
                                  out_vlan_port_id,
                                  outPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid2,
                                  match_inner_vid2,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN,
                                  in_vlan_port_edit_prfoile,
                                  egress_outer_vid2,
                                  egress_inner_vid2,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }

    /*
     * 8. Add MACT entry 
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}

/*
 * Cross connect a inlif to outlif for verifying qos map for IVE.
 * No EVE action is added. In IVE stage, {replace, replace} is performed.
 * The incoming packet should be double tagged, and outgoing packet is double tagged.
 * For outgoing packet:
 *     outer-tag.pcpdei is copied from incoming-tag.inner-pcpei.
 *     inner-tag.pcpdei is copied from incoming-tag.outer-pcpei.
 */
int
l2_qos_ive_replace_replace_eve_nop_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int tag_format_s_c = 16;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int ingress_outer_vid = 1000;
    int ingress_inner_vid = 2000;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };      /* DA */

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize s_c_format */
    rv = port_tpid_class_add(unit, inPort, s_tpid, c_tpid, tag_format_s_c);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /** After IVE recognize s_c_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, c_tpid, tag_format_s_c);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. IVE action settings - Replace and Replace, Exchange PCPDEI between outer and inner
     */
    rv = vlan_translation_action_create_and_set(unit, 1 /* Ingress */, ingress_action_id,
                                                bcmVlanActionReplace, bcmVlanActionReplace,
                                                bcmVlanActionInnerAdd, bcmVlanActionOuterAdd,
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 4. EVE action settings - Nop
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                bcmVlanActionNone, bcmVlanActionNone, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 5. Map LLVP and VLAN Port profile to Action 
     */
    rv = vlan_translate_action_map_set(unit, 1 /* Ingress */ , ingress_action_id,
                                       tag_format_s_c, in_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }
    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s_c, out_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    /*
     * 6. VPN create 
     */
    rv = vswitch_create(unit, vsi_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create\n");
        return rv;
    }

    /*
     * 7. Create VLAN-Ports 
     */
    rv = vswitch_vlan_port_create(unit,
                                  in_vlan_port_id,
                                  inPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid1,
                                  match_inner_vid1,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = vswitch_vlan_port_create(unit,
                                  out_vlan_port_id,
                                  outPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid2,
                                  match_inner_vid2,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }

    /*
     * 8. Add MACT entry 
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}

/*
 * Cross connect a inlif to outlif for verifying qos map for IVE.
 * No EVE action is added. In IVE stage, {Add, Add} is performed.
 * The incoming packet should be untagged, and outgoing packet is double tagged.
 * For outgoing packet:
 *     outer-tag.pcpdei is mapped from tc/dp.
 *     inner-tag.pcpdei is mapped from tc/dp.
 */
int
l2_qos_untag_ive_add_add_eve_nop_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int tag_format_s_c = 16;
    int tag_format_untag = 0;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int ingress_outer_vid = 1000;
    int ingress_inner_vid = 2000;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };      /* DA */

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize untagged */
    rv = port_tpid_class_add(unit, inPort, BCM_PORT_TPID_CLASS_TPID_INVALID, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_untag);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /** After IVE recognize s_c_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, c_tpid, tag_format_s_c);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. IVE action settings - Replace and Replace, Exchange PCPDEI between outer and inner
     */
    rv = vlan_translation_action_create_and_set(unit, 1 /* Ingress */, ingress_action_id,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 4. EVE action settings - Nop
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                bcmVlanActionNone, bcmVlanActionNone, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 5. Map LLVP and VLAN Port profile to Action 
     */
    rv = vlan_translate_action_map_set(unit, 1 /* Ingress */ , ingress_action_id,
                                       tag_format_untag, in_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }
    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s_c, out_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    /*
     * 6. VPN create 
     */
    rv = vswitch_create(unit, vsi_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create\n");
        return rv;
    }

    /*
     * 7. Create VLAN-Ports 
     */
    rv = vswitch_vlan_port_create(unit,
                                  in_vlan_port_id,
                                  inPort,
                                  vsi_id,
                                  BCM_VLAN_PORT_CREATE_INGRESS_ONLY,
                                  match_outer_vid1,
                                  match_inner_vid1,
                                  BCM_VLAN_PORT_MATCH_PORT,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = vswitch_vlan_port_create(unit,
                                  out_vlan_port_id,
                                  outPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid2,
                                  match_inner_vid2,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }

    /*
     * 8. Add vswitch cross connect 
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}


/** Set vlan translation actions for the given action-id.*/
int
vlan_translation_action_set(
    int unit,
    int is_ingress,
    int action_id,
    bcm_vlan_action_t outer_vlan_action,
    bcm_vlan_action_t inner_vlan_action,
    bcm_vlan_action_t outer_prio_action,
    bcm_vlan_action_t inner_prio_action,
    uint16 outer_tpid,
    uint16 inner_tpid)
{
    int rv;
    int flags;
    bcm_vlan_action_set_t action;

    /*
     * Fill Action 
     */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_vlan_action;
    action.dt_outer_pkt_prio = outer_prio_action;
    action.dt_inner = inner_vlan_action;
    action.dt_inner_pkt_prio = inner_prio_action;

    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /** Replace can be executed directly. This flag is not needed.*/
    /*flags |= BCM_VLAN_ACTION_SET_REPLACE;*/

    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}


/*
 * No EVE action. In IVE stage, {replace, replace} is performed.
 * The incoming packet should be double tagged, and outgoing packet is double tagged.
 * For Ingress PCP mapping(explicit mapping):
 *     {PCP, DEI=0} --> {PCP + 1, DEI=1}
 *     {PCP, DEI=1} --> {PCP - 1, DEI=0}
 * For outgoing packet:
 *     outer-tag.pcpdei is mapped from nwk_qos.
 *     inner-tag.pcpdei is copied from incoming-tag.pcpei.
 */
int
vlan_translation_ingress_pcpdei_setting_run(
    int unit)
{
    int rv;
    int ingress_action_id = 5;
    int l2_qos_map_id;
    bcm_qos_map_t l2_qos_map;
    uint32 flags;
    int qos_value;
    int in_vlan_port_id = 10000;

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);

    /** Set pcpdei action for IVE*/
    rv = vlan_translation_action_set(unit, TRUE/*Ingress*/, ingress_action_id,
                                     bcmVlanActionReplace, bcmVlanActionReplace,
                                     bcmVlanActionAdd, bcmVlanActionAdd,
                                     0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_action_replace\n");
        return rv;
    }

    /** Create L2 PCPDEI MAP-ID*/
    flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_INGRESS;
    rv = bcm_qos_map_create(unit, flags, &l2_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_qos_map_create\n");
        return rv;
    }

    if (verbose > 1) {
        printf("pcp dep profile for In-AC is: 0x%08X\n", l2_qos_map_id);
    }

    /** Add L2 PCPDEI MAP*/
    flags = BCM_QOS_MAP_L2_VLAN_PCP;
    for (qos_value = 0; qos_value < 8; qos_value++) {
        bcm_qos_map_t_init(&l2_qos_map);
        l2_qos_map.color = 0;
        l2_qos_map.int_pri = qos_value;

        l2_qos_map.pkt_cfi = 1;
        l2_qos_map.pkt_pri = (qos_value + 1) & 0x7;

        rv = bcm_qos_map_add(unit, flags, &l2_qos_map, l2_qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_qos_map_add\n");
            return rv;
        }

        l2_qos_map.color = 1;
        l2_qos_map.int_pri = qos_value;

        l2_qos_map.pkt_cfi = 0;
        l2_qos_map.pkt_pri = (qos_value - 1) & 0x7;

        rv = bcm_qos_map_add(unit, flags, &l2_qos_map, l2_qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_qos_map_add\n");
            return rv;
        }
    }

    /** Add the map-id to in-AC*/
    rv = bcm_qos_port_map_set(unit, in_vlan_port_id, l2_qos_map_id, -1);
    if (rv != BCM_E_NONE)
    {
            printf("Error, bcm_qos_port_map_set\n");
            return rv;
    }

    return rv;
}

/*create vlan port with qos pipe model (JR2 only pipeNextNameSpace) and egress output PCP/DEI*/
int
qos_vlan_port_create_with_qos_model(
    int unit,
    int vlan_port_id,
    int port,
    int match_vid, 
    uint8 pkt_pri,
    uint8 pkt_cfi,
    int qos_model)
{
    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port;
    vp.match_vlan = match_vid;
    vp.vlan_port_id = vlan_port_id;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    vp.flags |= BCM_VLAN_PORT_CROSS_CONNECT;
    vp.pkt_pri = pkt_pri;
    vp.pkt_cfi = pkt_cfi;
    vp.egress_qos_model.egress_qos = qos_model;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, match_vid, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

/*main function to test AC QOS pipe model at forward stage*/
int l2_qos_fwd_qos_model_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int egress_action_id = 6;
    int out_vlan_edit_prfoile = 3;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_vid1 = 100;
    int match_vid2 = 200;
    int egress_outer_vid = 500;
    int egress_inner_vid = 700;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; 
    bcm_vlan_port_translation_t port_trans;

    bcm_vlan_port_translation_t_init(&port_trans);

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize s_c_format */
    rv = port_tpid_class_add(unit, inPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }
    
    /** After IVE recognize s_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. Create VLAN-Ports 
     */
    rv = qos_vlan_port_create_with_qos_model(unit,in_vlan_port_id,inPort, match_vid1,0,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = qos_vlan_port_create_with_qos_model(unit,out_vlan_port_id,outPort,match_vid2,egress_pkt_pri,egress_pkt_cfi,l2_qos_model);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
 

    /*
     * 4. EVE action settings - Replace and add 
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionReplace, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionAdd, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s, out_vlan_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    port_trans.new_outer_vlan = egress_outer_vid;
    port_trans.new_inner_vlan = egress_inner_vid;
    port_trans.gport = out_vlan_port_id;
    port_trans.vlan_edit_class_id = out_vlan_edit_prfoile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     * 5. Cross connect
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}

