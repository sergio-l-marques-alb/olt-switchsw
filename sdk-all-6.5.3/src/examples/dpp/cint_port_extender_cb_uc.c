/* $Id: cint_port_extender_cb_uc.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/

/*
 * Purpose: The CINT demonstrates a Controlling bridge configuration as part of a Port Extender (802.BR) application.
 *
 * 1. The basic example handles UC traffic with no protection.
 *    Packets can be sent from/to a cascaded port or an End station.
 * 2. A new match can be added to an existing extender port using port_extender_cb_uc_match_add.
 *    A new Extender-Channel registration entry, corresponding to the new match, may be added by setting add_e_channel_reg_entry = 1.
 * 3. The previously created new match can be removed using port_extender_cb_uc_match_delete
 *    An existing Extender-Channel registration entry, corresponding to the removed match, may be added by setting delete_e_channel_reg_entry = 1.
 *
 *                                                                         Network
 *                                                                       | Port
 *                                                                       |
 *                                                    ___________________|__________________
 *                 __________                        |                                      |                           __________
 *        LIF 1 __|          |                       |         Controlling Bridge           |                          |          |__  LIF 3
 *        LIF 2 __|   Port   | --------------------->|------------------------------------> |------------------------->|  Port    |__
 *              __|          |      In-Cascaded-Port |                    |    /\           | Out-Cascaded-Port        |          |__
 *              __| Extender | <---------------------|<-------------------|    |            |                          | Extender |__
 *                |__________|                       |                    |    |            |                          |__________|
 *                                                   |                    |    |            |
 *                             _____________         |                    |    |            |
 *                            |             |<------ |<-------------------     |            |
 *                  LIF4(AC)  | End Station |        |                         |            |
 *                            |_____________|------->|-------------------------             |
 *                                                   |                                      |
 *                                                   |______________________________________|
 *
 * SOC properties:
 * 1. Configure the device to be a Control Bridge: extender_control_bridge_enable. 1: Enable, 0: Disable.
 * 2. Set the number of bytes to prepend to the start of the packet: prepend_tag_bytes = 8B
 * 3. Set the offset in bytes from the DA/SA in the packet from which to prepend the tag: prepend_tag_offset = 0
 *
 * CINT files:
 * cd ../../../../src/examples/dpp
 * cint cint_utils_global.c
 * cint cint_utils_port.c
 * cint cint_utils_l2.c
 * cint cint_utils_vlan.c
 * cint cint_port_extender_cb_uc.c
 * cint
 *
 * port_extender_cb_uc__start_run(int unit, port_extender_cb_uc_s *param) - param = NULL for default params
 *
 * All default values could be re-written with initialization of the global structure 'g_port_extender_cb_uc', before calling the main function.
 * In order to re-write only part of the values, call 'port_extender_cb_uc_struct_get(port_extender_cb_uc_s)' function and re-write the values
 * prior to calling the main function.
 */



/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
int NOF_EXTENDER_PORTS = 3;
uint8 adv_xlate_mode = 0;
int saved_field_group_8021br_pmf = 1;

struct port_extender_cascaded_port_s {
    bcm_gport_t port;
    bcm_port_tag_format_class_t tag_format;     /* Tag format used for Tag classification */
};

struct port_extender_port_s {
    bcm_gport_t port;
    bcm_vlan_t tag_vid;                         /* Outer C-VID */
    uint16 extender_vid;                        /* Extended-Port E-CID */
    bcm_gport_t extender_gport_id;              /* The LIF ID */
};

struct port_extender_end_station_s {
    bcm_gport_t port;
    bcm_vlan_t tag_vid;
    bcm_vlan_t tag_format;                      /* Tag format used for Tag classification */
    bcm_gport_t vlan_port_id;                   /* The LIF ID */
};

/*  Main Struct  */
struct port_extender_cb_uc_s {
    port_extender_cascaded_port_s cascaded_port[NOF_EXTENDER_PORTS];
    port_extender_port_s extender_port[NOF_EXTENDER_PORTS];
    port_extender_end_station_s end_station;
    bcm_vlan_t vsi;
    uint32 extender_tpid;
    uint32 tag_tpid;
    uint32 ext_vlan_edit_profile;               /* A VLAN-Edit profile that will be used only by Extender Ports */
    bcm_vlan_t new_vid;
};

/* Initialization of global struct*/
port_extender_cb_uc_s g_port_extender_cb_uc = { /* Cascaded Ports configuration
                                                Phy Port    Tag Format                  */
                                                {{ 13,       5   },     /* In-Cascaded-Port  */
                                                 { 13,       5   },     /* In-Cascaded-Port (Out) */
                                                 { 14,       5   }},    /* Out-Cascaded-Port */

                                                /* Extender Ports configuration
                                                Phy Port    Tag VID     Extender VID    Extender gport ID */
                                                {{ 13,       100,       201,            0 },    /* LIF 1 */
                                                 { 13,       100,       202,            0 },    /* LIF 2 */
                                                 { 14,       100,       203,            0 }},   /* LIF 3 */

                                                /* End Station configuration
                                                Phy Port    Tag VID     Tag format      vlan_port_id*/
                                                 { 15,       100,       4,              0 },

                                                /* Additional parameters */
                                                12,         /* VSI */
                                                0x893F,     /* Extender TPID */
                                                0x8100,     /* Tag TPID */
                                                3};         /* VLAN-Edit Profile */


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Create a PMF rule to restore the ETAG of packets */
int port_extender_8021br_pmf_init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    rv = bcm_field_group_create_mode_id(unit, qset, 10, bcmFieldGroupModeAuto, saved_field_group_8021br_pmf);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR:: bcm_field_group_create_mode_id returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);

    rv = bcm_field_group_action_set(unit, saved_field_group_8021br_pmf, aset);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_action_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_install(unit, saved_field_group_8021br_pmf);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_install\n");
        print rv;
    }
    return rv;
}

/* Add entry per CB port */
int port_extender_8021br_pmf_entry_add(int unit, int in_port) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_entry_t ent;

    rv = bcm_field_entry_create(unit, saved_field_group_8021br_pmf, &ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent, in_port, 0xffffffff);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_qualify_InPort returned %s\n", bcm_errmsg(rv));
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionStartPacketStrip, bcmFieldStartToL2Strip, 0x38 /*-8 Bytes in two s complement*/);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_action_add returned %s\n", bcm_errmsg(rv));
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_install returned %s\n", bcm_errmsg(rv));
    }

    return rv;
}


/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_port_extender_cb_uc
 */
int port_extender_cb_uc_init(int unit, port_extender_cb_uc_s *params) {

    int rv, cb_enable, prepend_bytes, prepend_offset, avt_mode;

    if (params != NULL) {
       sal_memcpy(&g_port_extender_cb_uc, params, sizeof(g_port_extender_cb_uc));
    }

    /* Verify Per device SOC Properties */
    cb_enable = soc_property_get(unit ,"extender_control_bridge_enable", 0);
    if (!cb_enable) {
        printf("Error in port_extender_cb_uc_init, cb_enable - %d\n", cb_enable);
        return BCM_E_PARAM;
    }

    prepend_bytes = soc_property_get(unit ,"prepend_tag_bytes", 0);
    if (prepend_bytes != 8) {
        printf("Error in port_extender_cb_uc_init, prepend_bytes - %d\n", prepend_bytes);
        return BCM_E_PARAM;
    }

    prepend_offset = soc_property_get(unit ,"prepend_tag_offset", 0);
    if (prepend_offset) {
        printf("Error in port_extender_cb_uc_init, prepend_offset - %d\n", prepend_offset);
        return BCM_E_PARAM;
    }

    /* Save VLAN Edit mode (advanced/standard) */
    adv_xlate_mode = soc_property_get(unit ,"bcm886xx_vlan_translate_mode", 0);

    /* Define a VSI for the switch */
    rv = bcm_vswitch_create_with_id(unit, g_port_extender_cb_uc.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create failed for vsi - %d, rv - %d\n", g_port_extender_cb_uc.vsi, rv);
        return rv;
    }

    /* Set the global Etag ethertype */
    rv = bcm_switch_control_set(unit, bcmSwitchEtagEthertype, g_port_extender_cb_uc.extender_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set failed for bcmSwitchEtagEthertype, etag_ethertype: 0x%x\n", g_port_extender_cb_uc.etag_ethertype);
        return rv;
    }

    rv = port_extender_8021br_pmf_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_extender_8021br_pmf_init failed \n");
        return rv;
    }
    return BCM_E_NONE;
}


/* Configuration of a single cascaded port.
 * Various per physical port configuration that determine the prepend tag,
 * the Extender device type and filter configuration.
 * In addition, the function handles the port's TPIDs - The profile and the
 * tag format classification.
 *
 * INPUT:
 *   cascaded_port_info: Configuration info for a single cascaded port.
 */
int port_extender_cb_uc__set_cascaded_port(int unit, port_extender_cascaded_port_s *cascaded_port_info) {

    int rv;
    uint32 vlan_domain, enable, flags;

    /* Enable prepend operation for the port */
    rv = bcm_switch_control_port_set(unit, cascaded_port_info->port, bcmSwitchPrependTagEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_switch_control_set for bcmSwitchPrependTagEnable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Configure the Port Extender Port-type to be a Cascaded Port */
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlExtenderType, BCM_PORT_EXTENDER_TYPE_SWITCH);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlExtenderType to SWITCH, port - %d, rv - %d\n", cascaded_port_info->port, rv);
    }

    /* Attach the port to a VLAN-Domain - The same as the port in this case (but doesn't have to) */
    vlan_domain = cascaded_port_info->port;
    rv = bcm_port_class_set(unit, cascaded_port_info->port, bcmPortClassId, vlan_domain);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Disable VLAN-Membership filter */
    flags = 0;
    rv = bcm_port_vlan_member_set(unit, cascaded_port_info->port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set to disable for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Disable same-interface filter */
    enable = 0;
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlBridge, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlBridge to Disable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Drop the untagged channel */
    enable = 1;
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlNonEtagDrop, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlNonEtagDrop to Enable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
    }

    /* Configure the TPIDs for the port and their classification */
    rv = port__tpids__set(unit, cascaded_port_info->port, g_port_extender_cb_uc.extender_tpid, g_port_extender_cb_uc.tag_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, port__tpids__set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    if (adv_xlate_mode) {
        rv = port__tag_classification__set(unit, cascaded_port_info->port, cascaded_port_info->tag_format, g_port_extender_cb_uc.extender_tpid, 0xFFFFFFFF);
        if (rv != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
            return rv;
        }
    }

    /* Create a PMF rule to restore the ETAG of packets */
    rv = port_extender_8021br_pmf_entry_add(unit, cascaded_port_info->port);
    if (rv != BCM_E_NONE) {
        printf("Error, port_extender_8021br_pmf_entry_add, port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }
    return BCM_E_NONE;
}


/* Configuration of Egress VLAN Editing for a single Extender LIF
 *
 * All cascaded Ports: Add ETAG, VID no change
 * g_port_extender_cb_uc.cascaded_port[2]: Add ETAG + Modify VID
 *
 */
int
port_extender_cb_uc__eve_translation_set(int unit,
                    bcm_gport_t lif,
                    int outer_tpid,
                    int inner_tpid,
                    bcm_vlan_action_t outer_action,
                    bcm_vlan_action_t inner_action,
                    bcm_vlan_t new_outer_vid,
                    bcm_vlan_t new_inner_vid,
                    uint32 vlan_edit_profile,
                    port_extender_cascaded_port_s * cascaded_port_info)
{
    bcm_vlan_action_set_t action;
    int rv;

    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    if (adv_xlate_mode)
    {
        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_port_translation_t port_trans;
        int action_id_1;

        /* set edit profile for egress LIF */
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.new_outer_vlan = new_outer_vid;
        port_trans.new_inner_vlan = new_inner_vid;
        port_trans.gport = lif;
        port_trans.vlan_edit_class_id = vlan_edit_profile;
        port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        /* Create action ID*/
        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action */
        action.dt_outer = outer_action;
        action.dt_inner = inner_action;
        action.dt_inner_pkt_prio = bcmVlanActionCopy;
        rv = bcm_vlan_translate_action_id_set( unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               action_id_1,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }

        /* Set translation action class */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = cascaded_port_info->tag_format;
        action_class.vlan_translation_action_id	= action_id_1;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }
    else if (cascaded_port_info->port == g_port_extender_cb_uc.cascaded_port[2].port)
    {
        action.ot_outer = outer_action;
        action.ot_inner = inner_action;
        action.new_outer_vlan = new_outer_vid;
        action.new_inner_vlan = new_inner_vid;
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.ot_inner_pkt_prio = bcmVlanActionCopy;
        rv =  bcm_vlan_translate_egress_action_add(unit, lif, BCM_VLAN_NONE ,BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add for port - %d, rv - %d\n", cascaded_port_info->port, rv);
            return rv;
        }
    }
    return rv;
}


/* Configuration of a Port Extender single Extender LIF
 * Create the object(LIF), associate it with a VSI and define an EVE operation.
 *
 * INPUT:
 *   extender_port_info: Configuration info for a single Extender LIF.
 */
int port_extender_cb_uc__set_extender_lif(int unit, port_extender_port_s *extender_port_info, port_extender_cascaded_port_s *cascaded_port_info) {

    int rv;
    bcm_extender_port_t extender_port;

    /* Format the supplied LIF info to the Port-Extender struct */
    bcm_extender_port_t_init(&extender_port);
    extender_port.port = extender_port_info->port;
    extender_port.match_vlan = extender_port_info->tag_vid;
    extender_port.extended_port_vid = extender_port_info->extender_vid;

    printf("Extender port params. port: %d, VLAN: %d ECID: %d\n", extender_port_info->port, extender_port_info->tag_vid, extender_port_info->extender_vid);

    extender_port.pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;
    extender_port.flags = BCM_EXTENDER_PORT_INGRESS_WIDE;

    /* Call the API to create the Port Extender object */
    rv = bcm_extender_port_add(unit, &extender_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_extender_port_add, port - %d, vlan - %d, E-TAG - %d, rv - %d\n", extender_port.port, extender_port.match_vlan, extender_port.extended_port_vid, rv);
    }
    extender_port_info->extender_gport_id = extender_port.extender_port_id;

    /* Attach the Extender-Port to a specific VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, extender_port.extender_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, extender_port.extender_port_id, rv);
    }
    printf("Allocated extender_port_id - %x\n", extender_port.extender_port_id);


    /* Configure an EVE action that Adds the E-TAG of the destination VM (out cascaded port: Add ETAG + replace VID) */
    rv = port_extender_cb_uc__eve_translation_set(unit,
                                                  extender_port.extender_port_id,
                                                  g_port_extender_cb_uc.extender_tpid,
                                                  g_port_extender_cb_uc.tag_tpid,
                                                  bcmVlanActionReplace,
                                                  bcmVlanActionAdd,
                                                  extender_port_info->extender_vid,
                                                  ((cascaded_port_info->port == g_port_extender_cb_uc.cascaded_port[2].port) ? g_port_extender_cb_uc.new_vid : extender_port.match_vlan),
                                                  g_port_extender_cb_uc.ext_vlan_edit_profile,
                                                  cascaded_port_info);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc__eve_translation_set, extender_port_id - %d, rv - %d\n", extender_port.extender_port_id, rv);
    }

    return BCM_E_NONE;
}


/* Configuration of a Port Extender End station
 * Handles the port's TPIDs - The profile and the tag format classification,
 * creates the AC LIF and associates it with a VSI.
 * No EVE is defined as the C-VID in this example is the same across all the
 * interfaces.
 *
 * INPUT:
 *   end_station_info: Configuration info for a single End station
 */
int port_extender_cb_uc__set_end_station(int unit, port_extender_end_station_s *end_station_info) {

    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_translation_t vp_translation;

    /* Configure the TPIDs for the port and their classification */
    rv = port__tpids__set(unit, end_station_info->port, g_port_extender_cb_uc.tag_tpid, 0x0);
    if (rv != BCM_E_NONE) {
        printf("Error, port__tpids__set for port - %d, rv - %d\n", end_station_info->port, rv);
        return rv;
    }

    if (adv_xlate_mode) {
        rv = port__tag_classification__set(unit, end_station_info->port, end_station_info->tag_format, g_port_extender_cb_uc.tag_tpid, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", end_station_info->port, rv);
            return rv;
        }
    }

    /* Create an AC LIF */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = end_station_info->port;
    vlan_port.match_vlan = end_station_info->tag_vid;
    vlan_port.egress_vlan = end_station_info->tag_vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n", end_station_info->port, end_station_info->tag_vid, rv);
        return rv;
    }
    end_station_info->vlan_port_id = vlan_port.vlan_port_id;

    /* Attach the AC LIF to the Controller Bridge VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
        return rv;
    }

    /* No IVE/EVE is configured as it is assumed that the End Station provides packets with the same C-TAG value.
       In other cases, the IVE/EVE should reflect the C-TAG manipulation. */
    if (adv_xlate_mode) {

        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &action);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vlan_translate_action_id_set, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}


/*
 * Main function to run the Port Extender Control Bridge UC example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Set Cascaded Ports
 *    3. Set Port-Extender LIFs
 *    4. Set an End Station
 *
 * INPUT: unit  - unit
 *        param - NULL for default params, or new values.
 */
int port_extender_cb_uc__start_run(int unit,  port_extender_cb_uc_s *params) {

    int rv, idx;
    bcm_extender_forward_t fwd_entry;

    /* Initialize the test parameters */
    rv = port_extender_cb_uc_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc_init, rv - %d\n", rv);
        return rv;
    }

    /* Configure the cascaded ports */
    for (idx = 1; idx < NOF_EXTENDER_PORTS; idx++) {
        rv = port_extender_cb_uc__set_cascaded_port(unit, &g_port_extender_cb_uc.cascaded_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__set_cascaded_port for port #%d, rv - %d\n", idx, rv);
            return rv;
        }
    }

    rv = port_extender_registration_and_no_etag_filter__create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_extender_registration_and_no_etag_filter__create, rv - %d\n", rv);
        return rv;
    }


    for (idx = 0; idx < NOF_EXTENDER_PORTS; idx++) {

        /* Configure Port-Extender LIFs */
        rv = port_extender_cb_uc__set_extender_lif(unit, &g_port_extender_cb_uc.extender_port[idx], &g_port_extender_cb_uc.cascaded_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__set_extender_lif for LIF #%d, rv - %d\n", idx, rv);
            return rv;
        }

        /* Add E-Channel registration entry */
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = g_port_extender_cb_uc.cascaded_port[idx].port; /* vlan_doamin */
        fwd_entry.extended_port_vid = g_port_extender_cb_uc.extender_port[idx].extender_vid; /* ecid */
        rv = bcm_extender_forward_add(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", cascaded_port_info->port, g_port_extender_cb_uc.extender_port[idx].extender_vid, rv);
            return rv;
        }
    }


    /* Configure an End Station */
    rv = port_extender_cb_uc__set_end_station(unit, &g_port_extender_cb_uc.end_station);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc__set_end_station, rv - %d\n", rv);
        return rv;
    } 

    /* Configure a mac address for the endpoint. */
    l2__mact_properties_s endpoint_mact_entry = { g_port_extender_cb_uc.end_station.vlan_port_id,
                                        {0x00, 0x00, 0x12, 0x34, 0x56, 0x78},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &endpoint_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    } 

    /* Configure a mac address for the out cascade port. */
    l2__mact_properties_s out_cascade_mact_entry = { g_port_extender_cb_uc.extender_port[2].extender_gport_id,
                                        {0x00, 0x00, 0x9a, 0xbc, 0xde, 0xf0},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &out_cascade_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    /* Configure a mac address for the in cascade port. */
    l2__mact_properties_s in_cascade_mact_entry = { g_port_extender_cb_uc.extender_port[1].extender_gport_id,
                                        {0x00, 0x00, 0x00, 0x11, 0x22, 0x33},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &in_cascade_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    printf("Done configuring\n");

    return BCM_E_NONE;
}

/*
 * Configuration of the extender channel registration and no etag filter.
 *
 * E-channel registration is performed using tt lookup #1 and the bcmPortControlNonEtagDrop
 * configures the port to use an e-tpid tt lookup #2, and this field program will
 * filter any packets that fail their tt lookup.
 *
 * Setting the port property bcmPortControlNonEtagDrop and E-channel registration is not enough to create no e-tag filtering -
 * the filtering itself is done in the field proccessor.
 * Therefore, in order to configure the filter, the port property must be set and
 * this function should be called, per port.
 * When the filter is removed, this field program should be removed as well.
 *
 * INPUT: unit  - unit
 */
int port_extender_registration_and_no_etag_filter__create(int unit){
    int rv;
    bcm_field_group_config_t grp_config;
    bcm_field_entry_t ent, ent2;
    int idx, enabled;

    bcm_field_group_config_t_init(&grp_config);

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp_config.qset);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyTunnelTerminated);   /* TT Lookup 0 - E-Channel registration */
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyIpTunnelHit);        /* TT Lookup 1 - Untagged check */
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInPort);

    /* Define the ASET */
    BCM_FIELD_ASET_INIT(grp_config.aset);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionDrop);

    /*  Create the Field group */
    grp_config.priority = 1;
    grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET;
    rv = bcm_field_group_config_create(unit, &grp_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    for (idx = 0; idx < NUMBER_OF_PORTS ; idx++) {

        /* Entry 1: TT lookup 0 (E-Channel registration check) */

        rv = bcm_field_entry_create(unit, grp_config.group, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create\n");
            return rv;
        }
        rv = bcm_field_qualify_TunnelTerminated(unit, ent, 0x0, 0x1);
        if (rv != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_TunnelTerminated\n");
          return rv;
        }
        rv = bcm_field_qualify_InPort(unit, ent, g_port_extender_cb_uc.cascaded_port[idx].port, ~0);
        if (rv != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_InPort\n");
          return rv;
        }
        rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_action_add\n");
            return rv;
        }

        /* Entry 2: TT lookup 1 (untagged check)  - only if enabled */

        rv = bcm_port_control_get(unit, g_port_extender_cb_uc.cascaded_port[idx].port, bcmPortControlNonEtagDrop, &enabled);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_get for bcmPortControlNonEtagDrop, port - %d, rv - %d\n", g_port_extender_cb_uc.cascaded_port[idx].port, rv);
        }

        if (enabled) {
            rv = bcm_field_entry_create(unit, grp_config.group, &ent2);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_entry_create\n");
                return rv;
            }
            rv = bcm_field_qualify_IpTunnelHit(unit, ent2, 0x0, 0x1);
            if (rv != BCM_E_NONE) {
              printf("Error in bcm_field_qualify_IpTunnelHit\n");
              return rv;
            }
            rv = bcm_field_qualify_InPort(unit, ent2, g_port_extender_cb_uc.cascaded_port[idx].port, ~0);
            if (rv != BCM_E_NONE) {
              printf("Error in bcm_field_qualify_InPort\n");
              return rv;
            }
            rv = bcm_field_action_add(unit, ent2, bcmFieldActionDrop, 0, 0);
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_action_add\n");
                return rv;
            }
        }
    }

    /* Install all to the HW */
    rv = bcm_field_group_install(unit, grp_config.group);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    printf("Done creating field filter\n");

    return rv;
}

/*
 * You can use this function to change the ports the cint is using from the defaults (13, 14 and 15).
 *
 * INPUT: unit              - unit
 *        in_cascaded_port  - cascaded port holding lifs 1 and 2
 *        out_cascaded_port - cascaded port holding lif 3
 *        end_station_port  - ethernet port holding lif 4
 */
int port_extender_cb_uc__start_run_dvapi(int unit, int in_cascaded_port, int out_cascaded_port, int end_station_port, bcm_vlan_t new_vid){
    port_extender_cb_uc_s params;

    sal_memcpy(&params, &g_port_extender_cb_uc, sizeof (g_port_extender_cb_uc));

    params.cascaded_port[0].port = in_cascaded_port;
    params.cascaded_port[1].port = in_cascaded_port;
    params.cascaded_port[2].port = out_cascaded_port;

    params.end_station.port = end_station_port;

    params.extender_port[0].port = in_cascaded_port;
    params.extender_port[1].port = in_cascaded_port;
    params.extender_port[2].port = out_cascaded_port;

    params.new_vid = new_vid;

    return port_extender_cb_uc__start_run(unit, &params);
}

/*
 * add match to an existing Extender LIF
 * INPUT: unit              - unit
 *        extender_index    - index of the extender port LIF, choose 0 or 1
 *        extended_port_vid - ECID of the new match to add
 *        add_e_channel_reg_entry  - add E-Channel registration entry flag
 */
int port_extender_cb_uc_match_add(int unit, int extender_index, int extended_port_vid, int add_e_channel_reg_entry){
    int rv;
    bcm_extender_forward_t fwd_entry;

    bcm_port_match_info_t extender_port_match;
    bcm_port_match_info_t_init(&extender_port_match);
    extender_port_match.match = BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_VLAN;
    extender_port_match.port = 0;
    extender_port_match.extended_port_vid = extended_port_vid;
    extender_port_match.match_vlan = 100;
    extender_port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_add(unit, g_port_extender_cb_uc.extender_port[extender_index].extender_gport_id, &extender_port_match);
    if (rv != BCM_E_NONE) {
          printf("Error in bcm_port_match_add\n");
          return rv;
    }

    if (add_e_channel_reg_entry) {
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = g_port_extender_cb_uc.cascaded_port[extender_index].port;    /* vlan_domain */
        fwd_entry.extended_port_vid = extended_port_vid;                                    /* ecid */
        rv = bcm_extender_forward_add(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", cascaded_port_info->port, extended_port_vid, rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Remove match from an existing Extender LIF (that was previously created with bcm_port_match_add)
 * INPUT:   unit                        - unit
 *          extender_index              - index of the extender port LIF, choose 0 or 1
 *          extended_port_vid           - ECID of the match to delete
 *          delete_e_channel_reg_entry  - delete E-Channel registration entry flag
 */
int port_extender_cb_uc_match_delete(int unit, int extender_index, int extended_port_vid, int delete_e_channel_reg_entry){
    int rv;
    bcm_extender_forward_t fwd_entry;

    bcm_port_match_info_t extender_port_match;
    bcm_port_match_info_t_init(&extender_port_match);
    extender_port_match.match = BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_VLAN;
    extender_port_match.port = 0;
    extender_port_match.extended_port_vid = extended_port_vid;
    extender_port_match.match_vlan = 100;
    extender_port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_delete(unit, g_port_extender_cb_uc.extender_port[extender_index].extender_gport_id, &extender_port_match);
    if (rv != BCM_E_NONE) {
          printf("Error in bcm_port_match_delete\n");
          return rv;
    }

    if (delete_e_channel_reg_entry) {
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = g_port_extender_cb_uc.cascaded_port[extender_index].port;    /* vlan_domain */
        fwd_entry.extended_port_vid = extended_port_vid;                                    /* ecid */
        rv = bcm_extender_forward_delete(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", cascaded_port_info->port, extended_port_vid, rv);
            return rv;
        }
    }

    return rv;
}
