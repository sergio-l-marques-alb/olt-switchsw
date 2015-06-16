/* $Id: cint_utils_l2.c,v 1.10 Broadcom SDK $
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
 * This file provides MPLS PORT basic functionality and defines MPLS_PORT global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */
/* Struct definitions */

struct mpls_port_utils_s
{
    uint32 vpn;
};

struct vswitch_vpls_shared_info_s {
    
    /* don't modify used by script*/
    int egress_intf;
    int egress_intf2;
    bcm_vlan_t vpn;

};

/* Globals */
mpls_port_utils_s g_mpls_port_utils = { 6202 /* vpn */ };

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
  
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_NONE; /* no VC key */
    mpls_port->port = port_id; /* physical port */
    mpls_port->encap_id = encap_id;
    BCM_ENCAP_REMOTE_SET(&(mpls_port->encap_id));
    mpls_port->flags |= BCM_MPLS_PORT_ENCAP_WITH_ID; /* Must be used, specify LIF ID is remote */
    mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL; 
    mpls_port->failover_port_id = failover_port_id;
    mpls_port->failover_id = failover_id;

    if (*gport!=0) {
        mpls_port->flags |= BCM_MPLS_PORT_WITH_ID;
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id,*gport); /* this is the LIF id */
    }
    mpls_port->egress_tunnel_if = egress_tunnel_if;
    rv = mpls_port__update_network_group_id(unit, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_vpls_forward_group_mpls_port_create\n");
        return rv;
    }
   
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
	egress_mc = 0;
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
