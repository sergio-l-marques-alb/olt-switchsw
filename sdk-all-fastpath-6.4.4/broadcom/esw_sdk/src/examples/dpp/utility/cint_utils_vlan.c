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


vlan_edit_utils_s g_eve_edit_utils = {2,    /* edit profile */
                                      5,    /* tag format */
                                      7};   /* action ID */


/* ************************************************************************************************** */

/* Define a basic VLAN Translation Action.
 * The VIDs source actions and the TPID values are supplied, but with no PCP-DEI configuration. 
 * Applicable only in AVT mode. 
 *  
 * INPUT: 
 *   is_ingress: Applies the API on the Ingress ot Egress VLAN translate.
 *   outer_tpid: Outer TPID value
 *   inner_tpid: Inner TPID value
 *   outer_action: Source for the Outer VID value
 *   inner_action: Source for the Inner VID value
 *   action_id: Returned Action ID as allocated by the function.
 */
int vlan__avt_vid_action__set(int unit,
                              uint32 is_ingress,
                              uint16 outer_tpid,
                              uint16 inner_tpid,
                              bcm_vlan_action_t outer_action,
                              bcm_vlan_action_t inner_action,
                              int *action_id)
{
    int rv;
    uint32 flags;
    bcm_vlan_action_set_t action;
    
    /* The configuration is either for Ingress or Egress */
    flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /* Create an Action ID */
    rv = bcm_vlan_translate_action_id_create(unit, flags, action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create failed, rv - %d\n", rv);
        return rv;
    }

    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, flags, *action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set for action_id - %d, rv - %d\n", *action_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configure a LIF with VLAN translation information.
 * The VID values for new Outer/Inner tag and a VLAN-Edit Profile. No PCP-DEI configuration. 
 * Applicable only in AVT mode. 
 *  
 * INPUT: 
 *   is_ingress: Applies the API on the Ingress ot Egress VLAN translate.
 *   gport_id: The LIF for which the configuration is performed.
 *   new_outer_vid: VID value for a new Outer tag
 *   new_inner_vid: VID value for a new Inner tag
 *   vlan_edit_profile: VLAN-Edit profile value
 */
int vlan__avt_lif_info__set(int unit,
                            uint32 is_ingress,
                            bcm_gport_t gport_id,
                            bcm_vlan_t new_outer_vid,
                            bcm_vlan_t new_inner_vid,
                            uint32 vlan_edit_profile)
{
    bcm_vlan_port_translation_t lif_translation_info;
    int rv;

    bcm_vlan_port_translation_t_init(&lif_translation_info);
    
    /* The configuration is either for Ingress or Egress */
    lif_translation_info.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /* Set port translation */
    lif_translation_info.new_outer_vlan = new_outer_vid;		
    lif_translation_info.new_inner_vlan = new_inner_vid;
    lif_translation_info.gport = gport_id;
    lif_translation_info.vlan_edit_class_id = vlan_edit_profile;
    rv = bcm_vlan_port_translation_set(unit, &lif_translation_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set failed for gport_id - %d, rv - %d\n", lif_translation_info.gport, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Perform a basic EVE configuration.
 * The LIF, TPID values, VID values and VLAN-Edit profile are supplied.
 * The Action is defined, the LIF VLAN-Edit information is configured as well 
 * as mapping between the VLAN-Edit Profile and q fixed tag format.
 *  
 * INPUT: 
 *   lif: Out-LIF to set
 *   gport_id: The LIF for which the configuration is performed.
 *   new_outer_vid: VID value for a new Outer tag
 *   new_inner_vid: VID value for a new Inner tag
 *   vlan_edit_profile: VLAN-Edit profile value
 */
int vlan__avt_eve_vids__set(int unit,
                            bcm_gport_t lif,
                            int outer_tpid,
                            int inner_tpid,
                            bcm_vlan_action_t outer_action,
                            bcm_vlan_action_t inner_action,
                            bcm_vlan_t new_outer_vid,
                            bcm_vlan_t new_inner_vid,
                            uint32 vlan_edit_profile)
{
    int rv, action_id;
    uint32 is_ingress = 0;

    rv = vlan__avt_vid_action__set(unit, is_ingress, outer_tpid, inner_tpid, outer_action, inner_action, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__avt_vid_action__set failed, rv - %d\n", rv);
        return rv;
    }

    rv = vlan__avt_lif_info__set(unit, is_ingress, lif, new_outer_vid, new_inner_vid, vlan_edit_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__avt_lif_info__set failed for lif - %d, rv - %d\n", lif, rv);
        return rv;
    }

    rv = vlan__translate_action_class__set(unit, action_id, vlan_edit_profile, g_eve_edit_utils.tag_format, is_ingress);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__translate_action_class__set failed for action_id - %d, rv - %d\n", action_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}








/* set translation actions (replace) */


/* set translation action classes for different tag formats */


int vlan__eve_default__set(int unit, bcm_gport_t  lif, int outer_vlan, int inner_vlan , bcm_vlan_action_t outer_action, bcm_vlan_action_t inner_action){

    int rv;

    rv = vlan_port_translation_set(unit, outer_vlan, inner_vlan, lif, g_eve_edit_utils.edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    /* set editing actions*/
    rv = vlan_translate_action_set(unit, g_eve_edit_utils.action_id, 0, 0, outer_action, inner_action);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_set\n");
        return rv;
    }

    /* set action class */
    rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
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
int vlan__translate_action_with_id__set(int unit, int action_id,
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


int vlan__init_vlan(int unit, int vlan) {
    bcm_error_t rv;
    bcm_port_config_t c;
    bcm_pbmp_t p,u;

    rv = bcm_port_config_get(unit, &c);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get\n");
        return rv;
    }
    BCM_PBMP_ASSIGN(p, c.e);
    
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vlan=%d, \n", vlan);
        return rv;
    }
    
    rv = bcm_vlan_port_add(unit, vlan, p, u);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        return rv;
    }
    
    return rv;
}
