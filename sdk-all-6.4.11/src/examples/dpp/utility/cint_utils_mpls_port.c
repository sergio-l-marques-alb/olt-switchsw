/* $Id: cint_utils_l2.c,v 1.10 Broadcom SDK $
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
 *
 * This file provides MPLS PORT basic functionality and defines MPLS_PORT global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */
/* Struct definitions */

struct mpls_port_utils_s
{
    uint32 vpn;
    bcm_gport_t mpls_port_id; 
    bcm_mpls_label_t egress_pwe_label;
    bcm_mpls_label_t ingress_pwe_label;
    bcm_if_t encap_id;
    bcm_mpls_port_match_t ingress_matching_criteria;
    bcm_if_t fec;
    bcm_if_t egress_mpls_tunnel_id;
    bcm_switch_network_group_t network_group;
    uint8 ttl;
    uint8 exp;
    bcm_gport_t port;

};

struct vswitch_vpls_shared_info_s {
    
    /* don't modify used by script*/
    int egress_intf;
    int egress_intf2;
    bcm_vlan_t vpn;

};

struct mpls_port__forward_group__info_s {
    
    int flags;
    int flags2;
    int encap_id; /* FEC will point to this value */
    int port;
    int second_heirarchy_fec; /* used in case the FEC is cascaded */
    int mpls_port_forward_group_id; /* out id parameter */

};

/* Globals */
mpls_port_utils_s g_mpls_port_utils = { 6202 /* vpn */,0,0,0,0,0,0,0,0,0,0};

vswitch_vpls_shared_info_s vswitch_vpls_shared_info_1;

/* *************************************************************************************************** */

/* 
 * In Jericho when using split_horizon_forwarding_groups_mode=1, if a MPLS port has
 * the flag BCM_MPLS_PORT_NETWORK, it needs to have a forwarding group which is not 0.
 */
int mpls_port__update_network_group_id(int unit, bcm_mpls_port_t* mpls_port) {
    int rv = BCM_E_NONE;

    mpls_port->network_group_id = 0;

    if(mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
        if (is_device_or_above(unit,JERICHO) && soc_property_get(unit, "split_horizon_forwarding_groups_mode", 1)) {
            mpls_port->network_group_id = 1;
        }
    }

    return rv;
}

int
mpls_port__init(int unit)
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


/*
 *  mpls_port__fec_only__create
 *  For protected gport that allocate only FEC.
 *  The indication is by setting the encap_id with BCM_ENCAP_REMOTE_SET().
 *  from SDK implementation side if given bcm_mpls_port_create called with BCM_MPLS_PORT_ENCAP_WITH_ID
 *  where the encap_id parameter has BCM_ENCAP_REMOTE_SET(): then this encap used as remote-LIF
 *  i.e. it's pointed by FEC but not allocated or configured.
 *  For Jericho egress_tunnel_if can be used in case of H-VPLS.
 *  Note:
 *  - this call can be done only for protected mpls_port i.e. failover_id valid.
 *  - when configure attribute of LIF the user need to supply the gport_id returned
 *  when calling bcm_mpls_port_create for the LIF. if FEC gport provided error is returned
 */
int
mpls_port__fec_only__create(
    int unit,
    bcm_port_t  port_id,
    bcm_gport_t failover_id, 
    bcm_gport_t failover_port_id, /* secondary, if this is primary*/
    bcm_if_t    encap_id, /* outLif ID */
    bcm_gport_t *gport,
    bcm_if_t egress_tunnel_if,
    bcm_mpls_port_t *mpls_port
    )
{
    int rv;
    
    bcm_mpls_port_t_init(mpls_port);
  
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_INVALID; /* not relevant for FEC */
    mpls_port->port = port_id; /* physical port */
    mpls_port->encap_id = encap_id;
    BCM_ENCAP_REMOTE_SET(&(mpls_port->encap_id));
    mpls_port->flags |= BCM_MPLS_PORT_ENCAP_WITH_ID; /* Must be used, specify LIF ID is remote */
    mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL; 
    mpls_port->failover_port_id = failover_port_id;
    mpls_port->failover_id = failover_id;

    if (*gport!=0) {
        mpls_port->flags |= BCM_MPLS_PORT_WITH_ID;
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id,*gport); /* this is the FEC id */
    }
    mpls_port->egress_tunnel_if = egress_tunnel_if;
   
    rv = bcm_mpls_port_add(unit, 0 /* No VPN required in FEC creation */, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n"); 
        return rv;
    }
    if(verbose >= 1) {
        printf("unit (%d): created mpls_port (FEC) :0x%08x point to remote LIF:0x%08x\n",unit,mpls_port->mpls_port_id,encap_id);
    }

    *gport = mpls_port->mpls_port_id;

    return BCM_E_NONE;
}



/*
 *  mpls_port__forward_group__create
 *  Allocating MPLS PORT non-protected FEC without allocating LIF.
 *  The indication is by setting the encap_id with BCM_ENCAP_REMOTE_SET() and flag BCM_MPLS_PORT_FORWARD_GROUP.
 */
int
mpls_port__forward_group__create(
    int unit,
    mpls_port__forward_group__info_s *forward_group_info
    )
{
    int rv;
    bcm_mpls_port_t mpls_port;

    bcm_mpls_port_t_init(&mpls_port);
  
    /* set port attribures */
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_FORWARD_GROUP|BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags |= forward_group_info->flags;
    mpls_port.flags2 |= forward_group_info->flags2;
    mpls_port.encap_id = forward_group_info->encap_id;
    BCM_ENCAP_REMOTE_SET(mpls_port.encap_id); /* in forwarding group remote bit should always be set */
    mpls_port.port = forward_group_info->port;
    if (forward_group_info->second_heirarchy_fec) {
        mpls_port.egress_tunnel_if = forward_group_info->second_heirarchy_fec;
    }

    rv = bcm_mpls_port_add(unit, 0, &mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }

    if(verbose){
        printf("MPLS PORT: FORWARD GROUP created: 0x%x.\n", mpls_port.mpls_port_id);
    }

    forward_group_info->mpls_port_forward_group_id = mpls_port.mpls_port_id;

    return BCM_E_NONE;
}

/* create VPN */
int
mpls_port__vswitch_vpls_vpn_create__set(int unit, bcm_vpn_t vpn){
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.vpn = vpn;
	int mc_group;
    int rv;

    printf("vswitch_vpls_vpn_create %d\n", vpn);
    
    /* destroy before create, just to be sure it will not fail */
    rv = bcm_mpls_vpn_id_destroy(unit,vpn);
    
    /* VPLS VPN with user supplied VPN ID */
    vpn_info.flags = BCM_MPLS_VPN_VPLS|BCM_MPLS_VPN_WITH_ID; 
    vpn_info.broadcast_group = vpn_info.vpn;
    vpn_info.unknown_multicast_group = vpn_info.vpn;
    vpn_info.unknown_unicast_group = vpn_info.vpn;
    rv = bcm_mpls_vpn_id_create(unit,&vpn_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_vpn_id_create\n");
        return rv;
    }
    if(verbose1){
        printf("created new VPN %d\n", vpn);
    }
    
    vswitch_vpls_shared_info_1.vpn = vpn;
    /* after it was created, set MC for MP */
    
    /* 2. create multicast: 
     *     - to be used for VPN flooding 
     *     - has to be same id as vpn 
     */
	mc_group = vpn;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_L2);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }
    if(verbose1){
        printf("created multicast   0x%08x\n\r",vpn);
    }
    
    return rv;
}

/*
 * When using VPLS application with forwarding two pointers (one for PWE and the second for MPLS): 
 * In ARAD egress tunnel if must be valid since multicast case include one pointer only. 
 * In Jericho next pointer must be null, in multicast case we use two pointers (one for PWE, second for MPLS).
 */
void
mpls_port__egress_tunnel_if_egress_only__set(int unit, int tunnel_id, int use_two_pointers, bcm_mpls_port_t *mpls_port) {

    if (use_two_pointers && is_device_or_above(unit, JERICHO)) {
        /* In Jericho next pointer is taken from the second pointer */
        mpls_port->egress_tunnel_if = 0;
    } else {
        mpls_port->egress_tunnel_if = tunnel_id;
    }
}

/* Create a standard multi point mpls port both in ingress and egress */
int mpls_port__mp_create_ingress_egress_mpls_port(int unit, mpls_port_utils_s *mpls_port_properties){

    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;

    bcm_mpls_port_t_init(&mpls_port);

    /* Set default flags */
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;

    /* Set (default or given) match criteria */
    if (!mpls_port_properties->ingress_matching_criteria) {
        mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    } else {
        mpls_port.criteria = mpls_port_properties->ingress_matching_criteria;
    }

    /* Set orientation /network group id*/
    if (is_device_or_above(unit,JERICHO) && soc_property_get(unit , "split_horizon_forwarding_groups_mode",1)) {
        if (mpls_port_properties->network_group) {
            mpls_port.network_group_id = mpls_port_properties->network_group;
            mpls_port.flags |= BCM_MPLS_PORT_NETWORK;
        }
    }
    else {
        mpls_port_1.flags |= BCM_MPLS_PORT_NETWORK;
    }


    /* Set the mpls tunnel to which the pwe entry points two.
       It can be taken from two optional sources:
       1) If we learn the fec, the pointer to the egress mpls tunnel is taken from the fec
       2) Else, the pointer is taken directly from the given egress mpls tunnel id.
    */
    if (mpls_port_properties->fec) {
        mpls_port.egress_tunnel_if = mpls_port_properties->fec;
    } else {
        mpls_port.egress_tunnel_if = mpls_port_properties->egress_mpls_tunnel_id;
    }

    /* Default ttl configuration */
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;

    /* Set (given or default) ttl */
    if (!mpls_port_properties->ttl) {
        mpls_port.egress_label.ttl = 20;
    } else {
        mpls_port.egress_label.ttl = mpls_port_properties->ttl;
    }

    /* Set exp */
    mpls_port.egress_label.exp = mpls_port_properties->exp;

    /* Set the egress pwe label */
    mpls_port.egress_label.label = mpls_port_properties->egress_pwe_label;

    /* Set the  port */
    mpls_port.port = mpls_port_properties->port;

    rv = bcm_mpls_port_add(unit,mpls_port_properties->vpn,&mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }

    mpls_port_properties->mpls_port_id = mpls_port.mpls_port_id;

    mpls_port_properties->encap_id = mpls_port.encap_id;

    return rv;
}


/* Create a standard multi point mpls port binded with a regular mpls label in one entry
   User is expected to pass the mpls label information in mpls_tunnel_properties*/
int mpls_port__mp_create_egress_mpls_port_binded_with_mpls(int unit, mpls_port_utils_s *mpls_port_properties, mpls__egress_tunnel_utils_s *mpls_tunnel_properties){

    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    
    bcm_mpls_egress_label_t label_array[1];
    int label_count;
    bcm_if_t pwe_encap_id;

    /* Create pwe */
    rv = mpls_port__mp_create_ingress_egress_mpls_port(unit,mpls_port_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__mp_create_ingress_egress_mpls_port\n");
        print rv;
        return rv;
    }

    /* Get the pwe */
    BCM_L3_ITF_SET(pwe_encap_id, BCM_L3_ITF_TYPE_LIF, mpls_port_properties->encap_id);
    rv = bcm_mpls_tunnel_initiator_get(unit, pwe_encap_id, 1, &label_array, &label_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_get\n");
        return rv;
    }

    /* Bind the pwe to the mpls label */
    mpls_tunnel_properties->label_in = label_array[0].label;
    mpls_tunnel_properties->with_exp = 1;
    mpls_tunnel_properties->exp = label_array[0].exp;
    mpls_tunnel_properties->ttl = label_array[0].ttl;
    mpls_tunnel_properties->tunnel_id = pwe_encap_id;
    mpls_tunnel_properties->egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
    mpls_tunnel_properties->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    rv = mpls__create_tunnel_initiator__set(unit,mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    return rv;

}


