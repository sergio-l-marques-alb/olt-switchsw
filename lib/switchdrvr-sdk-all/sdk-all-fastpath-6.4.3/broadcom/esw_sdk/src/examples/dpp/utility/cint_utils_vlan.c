/* $Id: cint_utils_vlan.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * This file provides vlan editing basic functionality and defines vlan editing global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */

/* Used for action definition*/
struct vlan_action_utils_s{
    int action_id;
    uint16 o_tpid;
    bcm_vlan_action_t o_action;
    uint16 i_tpid;
    bcm_vlan_action_t i_action;
};

/* Entry for vlan edit profile table */
struct vlan_edit_utils_s{
    int edit_profile;
    bcm_port_tag_format_class_t tag_format;
    int action_id;    
};


struct vlan_eve_edit_utils_s{
    int edit_profile;
    int action;
};

vlan_eve_edit_utils_s g_eve_edit_utils = {2, /* edit profile */
                                          7 /* action       */ };


 /* ************************************************************************************************** */





int vlan__eve_default__set(int unit, bcm_gport_t  lif, int outer_vlan, int inner_vlan , bcm_vlan_action_t outer_action, bcm_vlan_action_t inner_action){

    int rv;

    rv = vlan_port_translation_set(unit, outer_vlan, inner_vlan, lif, g_eve_edit_utils.edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set editing actions*/
    rv = vlan_translate_action_set(unit, g_eve_edit_utils.action, 0, 0, outer_action, inner_action);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_set\n");
        return rv;
    }

    /* set action class */
    rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_class_set\n");
        return rv;
    }

    return BCM_E_NONE;

}


/* set vlan port translation, determine edit profile for LIF */
int vlan__port_translation__set(int unit, bcm_vlan_t new_vid, bcm_vlan_t new_inner_vid, bcm_gport_t vlan_port, uint32 edit_class_id, uint8 is_ingress) {
    bcm_vlan_port_translation_t port_trans;
    int rv;
    
    /* Set port translation */
    bcm_vlan_port_translation_t_init(&port_trans);	
    port_trans.new_outer_vlan = new_vid;		
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = vlan_port;
    port_trans.vlan_edit_class_id = edit_class_id;
    port_trans.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    return rv;
}


/* Define actions */
int
vlan__translate_action_with_id__set(int unit, int action_id,
                                              uint16 outer_tpid, bcm_vlan_action_t outer_action,
                                              uint16 inner_tpid, bcm_vlan_action_t inner_action,  
                                              uint8 is_ingress) {
    bcm_vlan_action_set_t action;
    uint32 flags;
    int rv;

    flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /* Create action ID*/
    rv = bcm_vlan_translate_action_id_create( unit, flags | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }
    
    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    rv = bcm_vlan_translate_action_id_set( unit, 
                                           flags,
                                           action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    }  
    
    return rv; 
}


/* Set translation action - connect tag format and edit profile with action*/
int vlan__translate_action_class__set(int unit, int action_id, 
                                                uint32 edit_profile, 
                                                bcm_port_tag_format_class_t tag_format,
                                                uint8 is_ingress ) {
    bcm_vlan_translate_action_class_t action_class;
    int rv;

    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id	= action_id;
    action_class.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}
