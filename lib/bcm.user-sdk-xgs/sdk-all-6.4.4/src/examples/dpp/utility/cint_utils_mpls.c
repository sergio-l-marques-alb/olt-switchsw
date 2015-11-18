/*~~~~~~~~~~~~~~~~~~~~~~~Auxilliary mpls related functions~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_utils_mpls.c,v 1.22 Broadcom SDK $
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
 *
 * File: cint_utils_mpls.c
 * Purpose: Provide mpls utility functions functions
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */
/* Struct definitions */

struct mpls__egress_tunnel_utils_s
{
    int flags;
    bcm_mpls_egress_action_t egress_action;
    bcm_mpls_label_t label_in;
    bcm_mpls_label_t label_out; /* 0 in case only one label is created */
    int next_pointer_intf;
    int tunnel_id; /* out parameter, created tunnel id */
};

struct mpls__ingress_tunnel_utils_s
{
    int flags;
    bcm_mpls_label_t label; /* Incoming label value. */
    bcm_mpls_label_t second_label;  /* Incoming second label. */
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_if_t tunnel_if; /* hierarchical interface, relevant for
                                           when action is
                                           BCM_MPLS_SWITCH_ACTION_POP. */
    bcm_vpn_t vpn; /* set this value only if action is pop */
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
};



/* Globals */
uint8 mpls_pipe_mode_exp_set = 0;

/* global exp value for a label*/
int mpls_exp = 0;
int ext_mpls_exp = 4;
int mpls_ttl = 20;
int ext_mpls_ttl = 60;

/* ****************************************************************************************************/

/*
 * Internal functions
 */

/* Init function for this file*/
int
mpls__init(int unit)
{
    int rv;
    bcm_info_t info;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    return rv;
}

/* Sets mpls pipe mode exp*/
int mpls__mpls_pipe_mode_exp_set(int unit) {
    int rv = BCM_E_NONE;
    if (mpls_pipe_mode_exp_set) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, mpls_pipe_mode_exp_set);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_set\n");
            print rv;
            return rv;
        }
    }

    return rv;                
}


/*
 * Functions calling BCM apis
 */

/* set egress action over this l3 interface, so packet forwarded to this interface will be tunneled/swapped/popped */
int 
mpls__create_tunnel_initiator__set(int unit, mpls__egress_tunnel_utils_s *mpls_tunnel_properties) {
    bcm_mpls_egress_label_t label_array[2];
    int num_labels;
    int rv;

    rv = mpls__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__init\n");
        return rv;
    }

    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].exp = mpls_exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    if (mpls_tunnel_properties->egress_action==BCM_MPLS_EGRESS_ACTION_PHP) {
      label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV4;
    }
	if (mpls_tunnel_properties->flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        label_array[0].action = mpls_tunnel_properties->egress_action;
	}
	label_array[0].label = mpls_tunnel_properties->label_in; 
    label_array[0].ttl = mpls_ttl;
    label_array[0].l3_intf_id = mpls_tunnel_properties->next_pointer_intf;
    label_array[0].tunnel_id = mpls_tunnel_properties->tunnel_id;
    num_labels = 1;
               
    if (mpls_tunnel_properties->label_out>0) {
        bcm_mpls_egress_label_t_init(&label_array[1]);
        label_array[1].exp = ext_mpls_exp; 
        label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
        label_array[1].label = mpls_tunnel_properties->label_out;
        label_array[1].ttl = ext_mpls_ttl;
        label_array[1].l3_intf_id = mpls_tunnel_properties->next_pointer_intf;
        num_labels = 2;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    mpls_tunnel_properties->tunnel_id = label_array[0].tunnel_id;

    return rv;
}


/* Add switch entry 
 */
int
mpls__add_switch_entry(int unit, mpls__ingress_tunnel_utils_s *mpls_tunnel_properties)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = mpls_tunnel_properties->action;
    /* TTL decrement has to be present 
     * Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags = mpls_tunnel_properties->flags | BCM_MPLS_SWITCH_TTL_DECREMENT|BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;

    /* incoming label */
    entry.label = mpls_tunnel_properties->label;
    entry.second_label = mpls_tunnel_properties->second_label;

    /* populate vpn information */
    entry.vpn = mpls_tunnel_properties->vpn;
    
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

