/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MS PWE NODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * $Id: cint_qos_mspw.c, 2014/09/02 huilizha Exp $
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
 
 * File: cint_qos_mspw.c
 * Purpose: An example of cos mapping for MS PWE Transit Node

 * Cos mapping 
 * 1. Ingress      
       - If Tunnel is uniform, map LSP.EXP to TC/DP
       - If Tunnel is pipe, map PWE.EXP to TC/DP(ignore PWE mode)
   2. Egress
       - PWE header
         - EXP: If uniform, map TC/DP to EXP; if pipe, use user configuration
         - TTL: If uniform, it's 255; if pipe, from MPLS CMD profile
       - LSP header
         - EXP: If uniform, copy from PWE.EXP; if pipe, use user configuration
         - TTL: If uniform, copy form PWE label; if pipe, from MPLS CMD profile
         
 *  Map ingress and egress as follows 
 *  Map EXP                                                  
 *  Ingress LSP.EXP(PWE.EXP)            INTERNAL TC/DP          Egress EXP
       0                                   1                      1
       1                                   2                      2                 
       2                                   3                      3    
       3                                   4                      4
       4                                   5                      5
       5                                   6                      6
       6                                   7                      7
       7                                   7                      7
       
 * Explanation:
   1. In this service we create two PWE ports. PWE1 and PWE2 refers to MPLS network ports.
   2. PWE: Virtual circuit attached to the service based on VC-label, and connectted to the MPLS network side.
   3. PWE-1: network port with incoming VC = 50,  egress-VC = 100 defined over tunnel 200.
   4. PWE-2: network port with incoming VC = 250, egress-VC = 350 defined over tunnel 400.
   
 * Calling up sequence:
 * Set up sequence:
   1. set the pipe tunnel inheritance mode, pipe or uniform
      - Call bcm_switch_control_set with property bcmSwitchMplsPipeTunnelLabelExpSet
   2. create in PWE LIF
      - call bcm_mpls_port_add with mpls_port.match_label 50
   3. set up MPLS tunnels. Refer to mpls_tunnels_config().
      - Set MPLS L2 termination (on ingress).
        - MPLS packets arriving with DA 00:00:00:00:00:11 and VID 100 causes L2 termination.  
      - Create MPLs tunnels over the created l3 interface.
        - Packet routed to above interface is tunneled into MPLS tunnels. 
      - Create egress object points to the above l3-interface.
        - Packet routed to this egress-object is forwarded to the above MPLS tunnels and with the L3-interface VID and MAC.
        - Calls bcm_l3_egress_create().
      - Add pop entries to MPLS switch.
        - MPLS packet arriving from the MPLS network side with labels 200 is popped (refer to mpls_add_pop_entry).
        - Call bcm_mpls_tunnel_switch_create().
   4. create out PWE LIF
       - call bcm_mpls_port_add with mpls_port.egress_label.label 350
   5. cross connect the 2 PWE LIFs
      - Call bcm_vswitch_cross_connect_add()
 * Clean up sequence:
   1. Delete the cross connected LIFs
      - Call bcm_vswitch_cross_connect_delete()
   2. Delete in  PWE LIF
      - Call bcm_mpls_port_delete()
   3. Delete out PWE LIF
      - Call bcm_mpls_port_delete()
   4. Delete FEC, ARP outLif
      - Call bcm_l3_egress_destroy()
   5. Delete tunnel outLIf
      - Call bcm_mpls_tunnel_initiator_clear()
   6. Delete tunnel inLif
      - Call bcm_mpls_tunnel_switch_delete()

 * To Activate Above Settings Run:
   BCM> cd ../../../../src/examples/dpp
   BCM> cint utility/cint_utils_global.c
   BCM> cint utility/cint_utils_mpls.c
   BCM> cint utility/cint_utils_l3.c
   BCM> cint cint_ip_route.c
   BCM> cint cint_qos_mspw.c
   BCM> cint
   cint> mpls_pipe_mode_exp_set=0(1);
   cint> qos_map_vpls(unit);
   cint> vswitch_vpls_run_with_defaults(unit,15,16,mpls_pipe_mode_exp_set);
 
 * Headers:
 *  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS   ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8100|    |100||Label:200||Lable:50  ||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1: Packets Transmitted from PWE1 |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA  |SA  ||TIPD1 |Prio|VID||  MPLS   ||   PWE    ||Eth||Data ||
 *   |  |0:22|0:11||0x8100|    |150||Label:400||Lable:350 ||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2: Packets Received on PWE2 |                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 
 * Traffic example:
   1. uniform model
   Send PWE to PWE:
   Send:
   Ethernet DA 00:00:00:00:00:11, SA, VLAN: TPID 0x8100, VID 100
   PWE LABEL 50 EXP 4  TTL 20
   MPLS LABEL 200 EXP 3  TTL 10
   Expect:
   Ethernet DA 00:00:00:00:00:22, SA 00:00:00:00:00:11, VLAN: TPID 0x8100, VID 150
   PWE LABEL 350 EXP 4  TTL 255
   MPLS LABEL 400 EXP 4   TTL 255
   
   2. pipe model
   Send PWE to PWE:
   Send:
   Ethernet DA 00:00:00:00:00:11, SA, VLAN: TPID 0x8100, VID 100
   PWE LABEL 50 EXP 4  TTL 20
   MPLS LABEL 200 EXP 3  TTL 10
   Expect:
   Ethernet DA 00:00:00:00:00:22, SA 00:00:00:00:00:11, VLAN: TPID 0x8100, VID 150
   PWE LABEL 350 EXP 1  TTL 15
   MPLS LABEL 400 EXP 2  TTL 20
 */

struct mpls_info_s {
    int l3_intf1;
    int l3_intf2;
    int tunnel_id;
    int fec;
    int encap_id;
    int with_id;
    int mpls_id;
};

struct cross_connect_info_s {
    bcm_port_t in_port; 
    bcm_port_t eg_port; 
    bcm_vswitch_cross_connect_t gports;
    bcm_vlan_t in_vid;
    bcm_vlan_t eg_vid;
    int in_vc_label; /* incomming VC label */
    int eg_vc_label; /* egress VC label */
    bcm_mac_t my_mac;
    bcm_mac_t nh_mac; /* next hop mac address */
};

static int vpls_in_map_id; /* QOS COS-profile for PWE-LIF */
static int vpls_eg_map_id; /* QOS Remark profile for PCP->EXP */
bcm_mac_t my_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
bcm_mac_t nh_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}; 
cross_connect_info_s cross_connect_info;
bcm_if_t tunnel_outLif;
int fec_id;

bcm_color_t qos_map_mpls_color[8] = {bcmColorGreen,
                          bcmColorYellow,
                          bcmColorGreen,
                          bcmColorYellow,
                          bcmColorGreen,
                          bcmColorRed,
                          bcmColorYellow,
                          bcmColorRed};

/* initialize the information of cross connect */
void cross_connect_info_init(int unit, int port1, int port2) 
{
    bcm_vswitch_cross_connect_t_init(&cross_connect_info.gports);

    cross_connect_info.in_port = port1; 
    cross_connect_info.eg_port = port2;
    cross_connect_info.in_vid = 100;
    cross_connect_info.eg_vid = 150;
    cross_connect_info.in_vc_label = 50;
    cross_connect_info.eg_vc_label = 350; 
    sal_memcpy(cross_connect_info.my_mac, my_mac, 6);    
    sal_memcpy(cross_connect_info.nh_mac, nh_mac, 6);    
}

/* Map Ingress COS-profile to do EXP-> TC/DP for PWE LIF. */
int qos_map_vpls_ingress_profile(int unit)
{
    bcm_qos_map_t vpls_in_map;
    int flags = 0;
    int exp;
    int rv = BCM_E_NONE;

    /* Clear structure */
    bcm_qos_map_t_init(&vpls_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &vpls_in_map_id);
    if (rv != BCM_E_NONE) {
       printf("error in ingress vpls bcm_qos_map_create() $rv\n");
       return rv;
    }

    /* EXP In-PWE */
    for (exp=0; exp<8; exp++) {
      flags = BCM_QOS_MAP_MPLS;
      
      /* Set ingress EXP */
      vpls_in_map.exp = exp; /* EXP Input */
      
      /* Set internal priority for this ingress EXP */
      vpls_in_map.int_pri = exp+1; /* TC Output */
      
      if (exp == 7) {
          vpls_in_map.int_pri = exp; /* TC Output */
      }

      /* Set color for this ingress EXP  */
      vpls_in_map.color = qos_map_mpls_color[exp]; /* DP Output */

      /* Set In-DSCP-EXP = EXP */
      vpls_in_map.remark_int_pri = exp+1; /* In-DSCP-EXP Output */
      
      if (exp == 7) {
          vpls_in_map.remark_int_pri = exp; /* In-DSCP-EXP Output */
      }

      rv = bcm_qos_map_add(unit, flags, &vpls_in_map, vpls_in_map_id);
      if (rv != BCM_E_NONE) {
        printf("Error in: bcm_qos_map_add $rv\n");
        return rv;
      }
    }
    
    return rv;
}

/* Map Egress Remark-Profile to do TC/DP->EXP */
int qos_map_vpls_egress_profile(int unit)
{
    bcm_qos_map_t vpls_eg_map;
    int flags = 0;
    int int_pri;
    int rv = BCM_E_NONE;
 
    /* Clear structure */
    bcm_qos_map_t_init(&vpls_eg_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &vpls_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in VPLS egress bcm_qos_map_create() $rv \n");
        return rv;
    }

    for (int_pri=0; int_pri<8; int_pri++) {        
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP;
        /* Set PCP -> EXP by mapping TC/DP -> EXP (assume PCP 1:1 TC) */
        if (int_pri == 7) {
            vpls_eg_map.remark_int_pri = int_pri; /* Out-DSCP-EXP equals TC Input */
        } else {
            vpls_eg_map.remark_int_pri = int_pri + 1; /* Out-DSCP-EXP equals TC Input */
        }

        vpls_eg_map.color = qos_map_mpls_color[int_pri]; /* DP */
        vpls_eg_map.exp = int_pri; /* PWE.EXP Output */        
        
        rv = bcm_qos_map_add(unit, flags, &vpls_eg_map, vpls_eg_map_id); 
        if (rv != BCM_E_NONE) {
           printf("error in VPLS egress bcm_qos_map_add() $rv \n");
           return rv;
        }
    }
    return rv;
}

/* Main function set all QOS profiles */
int qos_map_vpls(int unit)
{
    int rv = BCM_E_NONE;
    
    rv = qos_map_vpls_ingress_profile(unit);
    if (rv) {
        printf("error setting up ingress qos profile $rv\n");
        return rv;
    }

    rv = qos_map_vpls_egress_profile(unit);
    if (rv) {
        printf("error setting up egress qos profile $rv\n");
        return rv;
    }
    return rv;
}

/* Return the map id set up for egress  */
int qos_map_id_vpls_egress_get(int unit)
{
    return vpls_eg_map_id;
}

/* Return the map id set up for ingress  */
int qos_map_id_vpls_ingress_get(int unit)
{
    return vpls_in_map_id;
}

/* get the mpls tunnel pipe mode */
int mpls_pipe_mode_exp_get(int unit, int *is_pipe) 
{
    int rv = BCM_E_NONE;
    rv = bcm_switch_control_get(unit, bcmSwitchMplsPipeTunnelLabelExpSet, is_pipe);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_switch_control_get $rv\n");
        return rv;
    } 
    
    return rv;                
}

/* add switch entry to perform pop for ingress mpls tunnel*/
int mpls_add_pop_entry(int unit, mpls_info_s *info, int in_label)
{
    int rv = BCM_E_NONE;
    int is_pipe;
    
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);
    
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
     * in general valid options: 
     *    both present (uniform) or none of them (Pipe)
    */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    
    rv = mpls_pipe_mode_exp_get(unit, &is_pipe);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_get $rv \n");
        return rv;
    }
    
    if (!is_pipe) {
        /* if tunnel is uniform, then map lsp.exp to TC/DP */
        entry.qos_map_id = qos_map_id_vpls_ingress_get(unit);
    }
    
    /* incoming label */
    entry.label = in_label;
    entry.port = cross_connect_info.in_port;
    
    /* Add the switch entry */
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create $rv\n");
        return rv;
    }

    return rv;
}

/* initialize the tunnels for mpls routing */
int mpls_tunnels_config(int unit, mpls_info_s *info, int in_tunnel_label, int out_tunnel_label) 
{
    int CINT_NO_FLAGS = 0;
    int ingress_intf;
    bcm_mpls_egress_label_t label_array[1];
    int rv,num_labels;
    int is_pipe;
    int vrf = 0;
    
    bcm_mpls_egress_label_t_init(&label_array[0]);
    
    rv = mpls_pipe_mode_exp_get(unit, &is_pipe);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_get $rv \n");
        return rv;
    }
        
    printf("open vlan %d\n", cross_connect_info.in_vid);
    rv = bcm_vlan_destroy(unit, cross_connect_info.in_vid);
    rv = bcm_vlan_create(unit,cross_connect_info.in_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vid = %d and unit %d\n", cross_connect_info.in_vid, unit);
        return rv;
    }

    printf("open vlan %d\n", cross_connect_info.eg_vid);
    rv = bcm_vlan_destroy(unit, cross_connect_info.eg_vid);
    rv = bcm_vlan_create(unit,cross_connect_info.eg_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vid = %d and unit %d\n", cross_connect_info.eg_vid, unit);
        return rv;
    }
    
    /* Create ingress and egress vlan gports */
    rv = bcm_vlan_gport_add(unit, cross_connect_info.in_vid, cross_connect_info.in_port, BCM_VLAN_GPORT_ADD_UNTAGGED);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add, vlan=%d, \n", vlan);
        return rv;
    }
    
    rv = bcm_vlan_gport_add(unit, cross_connect_info.eg_vid, cross_connect_info.eg_port, BCM_VLAN_GPORT_ADD_UNTAGGED);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add, vlan=%d, \n", vlan);
        return rv;
    }
    
    /* create ingress router interface  */
    rv = vlan__open_vlan_per_mc(unit, cross_connect_info.in_vid, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", cross_connect_info.in_vid, unit);
    }
    rv = bcm_vlan_gport_add(unit, cross_connect_info.in_vid, cross_connect_info.in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", cross_connect_info.in_port, cross_connect_info.in_vid);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = cross_connect_info.in_vid;
    intf.my_global_mac = cross_connect_info.my_mac;
    intf.my_lsb_mac = cross_connect_info.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rv = l3__intf_rif__create(unit, &intf);
    info->l3_intf1 = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    /* create egress router interface */
    rv = vlan__open_vlan_per_mc(unit, cross_connect_info.eg_vid, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", cross_connect_info.eg_vid, unit);
    }
    rv = bcm_vlan_gport_add(unit, cross_connect_info.eg_vid, cross_connect_info.eg_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", cross_connect_info.eg_port, cross_connect_info.eg_vid);
      return rv;
    }
    
    intf.vsi = cross_connect_info.eg_vid;
    
    rv = l3__intf_rif__create(unit, &intf);
    info->l3_intf2 = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    label_array[0].exp = 2;
    /* if the tunnel is uniform, then lsp.exp is copied from pwe.exp; if the tunnel is pipe, then use user configuration */   
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
    if (!is_device_or_above(unit,ARAD_PLUS) || is_pipe) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_TTL_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
    }
    
    label_array[0].qos_map_id = qos_map_id_vpls_egress_get(unit);
    label_array[0].label = out_tunnel_label;
    label_array[0].ttl = 20;
    label_array[0].tunnel_id = info->tunnel_id;
    label_array[0].l3_intf_id = info->l3_intf2;
    num_labels=1;
    
    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create $rv\n");
        return rv;
    } 
    
    info->tunnel_id = label_array[0].tunnel_id;
    tunnel_outLif = label_array[0].tunnel_id;
    ingress_intf = info->tunnel_id;

    /* create egress object */
    rv = create_l3_egress(unit, CINT_NO_FLAGS, cross_connect_info.eg_port, cross_connect_info.eg_vid, ingress_intf, cross_connect_info.nh_mac, &(info->fec), &(info->encap_id));
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress\n");
        return rv;
    }
    
    fec_id = info->fec;
    
    /* add switch entry to pop MPLS label */
    mpls_add_pop_entry(unit, info, in_tunnel_label);
    
    return rv;
}

/* Add mpls port in ingress*/ 
int mpls_port_create_in(int unit, mpls_info_s *info) 
{
    int rv;
    int is_pipe;
    bcm_mpls_port_t mpls_port;
	int mpls_termination_label_index_enable;
    
    rv = mpls_pipe_mode_exp_get(unit, &is_pipe);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_get $rv \n");
        return rv;
    }

	/* set port attribures */
    mpls_port.mpls_port_id = info->mpls_id;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = cross_connect_info.in_vc_label;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.egress_label.label = 100;

	/* read mpls index soc property */
	mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	if (mpls_termination_label_index_enable) {
	    BCM_MPLS_INDEXED_LABEL_SET(&mpls_port.match_label,cross_connect_info.in_vc_label,1);
	}
    
    /* in ingress if tunnel is pipe, then map pwe.exp to TC/DP */
    if (is_pipe) {
        mpls_port.qos_map_id = qos_map_id_vpls_ingress_get(unit);
    }
    
    /* to create a mpls port */
    rv = bcm_mpls_port_add(unit, 0, &mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add $rv\n");
        return rv;
    }
    
    info->mpls_id = mpls_port.mpls_port_id;
    cross_connect_info.gports.port1 = mpls_port.mpls_port_id;
    
    return rv;
} 

/* Add mpls port in egress*/ 
int mpls_port_create_out(int unit, mpls_info_s *info, bcm_mpls_port_t *mpls_port, int in_tunnel_label, int out_tunnel_label) 
{
    int rv;
    int is_pipe;
	int mpls_termination_label_index_enable;
    
    rv = mpls_pipe_mode_exp_get(unit, &is_pipe);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_get $rv \n");
        return rv;
    }  
    
    rv = mpls_tunnels_config(unit, info, in_tunnel_label, out_tunnel_label);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnels_config $rv\n");
        return rv;
    }
    
    /* set port attribures */
    if (!is_device_or_above(unit,ARAD_PLUS) || is_pipe) {
        mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_TTL_SET;
    } else {
        mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
    }
    mpls_port->mpls_port_id = info->mpls_id;
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port->match_label = 250;
    mpls_port->egress_tunnel_if = info->fec;
    mpls_port->flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port->port = cross_connect_info.eg_port;
    mpls_port->egress_label.label = cross_connect_info.eg_vc_label;
    mpls_port->egress_label.ttl = 15;
    mpls_port->egress_label.exp = 1;

	/* read mpls index soc property */
	mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	if (mpls_termination_label_index_enable) {
	    BCM_MPLS_INDEXED_LABEL_SET(&mpls_port->match_label,250,1);
	}
    
    /* map TC/DP to pwe.exp */
    mpls_port->egress_label.qos_map_id = qos_map_id_vpls_egress_get(unit);   

    /* to create a p2p mpls port */
    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add $rv\n");
        return rv;
    }
    
    info->mpls_id = mpls_port->mpls_port_id;
    cross_connect_info.gports.port2 = mpls_port->mpls_port_id;
    
    return rv;
}

int vswitch_vpls_run_with_defaults(int unit, int inPort, int outPort, int pipe_mode_exp_set) 
{
    int rv;
    bcm_mpls_port_t mpls_port;
    mpls_info_s info;
    
    /* add port according to VC label */
    bcm_mpls_port_t_init(&mpls_port);
    
    /* initialize the information of cross connect */
    cross_connect_info_init(unit, inPort, outPort);
    
	mpls_pipe_mode_exp_set = pipe_mode_exp_set;
    /* set the mpls tunnel as pipe or uniform */
    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls__mpls_pipe_mode_exp_set $rv\n");
        return rv;
    }
    
    rv = mpls_port_create_in(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port_create_in $rv\n");
        return rv;
    }
    
    rv = mpls_port_create_out(unit, &info, &mpls_port, 200, 400);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port_create_out $rv\n");
        return rv;
    }
    
    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_info.gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add $rv\n");
        return rv;
    }
    
    return rv;
}

/* Delete PWE1 and PWE2 */
int delete_pwe_lifs(int unit) {
    int rv = BCM_E_NONE;

    rv = bcm_vswitch_cross_connect_delete(unit, cross_connect_info.gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete $rv\n");
        return rv;
    }
    
    rv = bcm_mpls_port_delete(unit, 0, cross_connect_info.gports.port1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_delete $rv\n");
        return rv;
    }
    
    rv = bcm_mpls_port_delete(unit, 0, cross_connect_info.gports.port2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_delete $rv\n");
        return rv;
    }

    return rv;
}

/* delete tunnel outLif */
int mpls_tunnel_initiator_clear(int unit, bcm_if_t intf)
{
    int rv;
    
    rv = bcm_mpls_tunnel_initiator_clear(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_clear $rv\n");
        return rv;
    }
    
    return rv;
}

/* delete FEC, ARP outLif */
int l3_egress_destroy(int unit)
{
    int rv;
    
    rv = bcm_l3_egress_destroy(unit, fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_destroy $rv\n");
        return rv;
    }
    
    return rv;
}

/* delete tunnel inLif */
int mpls_tunnel_switch_delete(int unit, int in_label)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.label = in_label;

    rv = bcm_mpls_tunnel_switch_delete(unit, &entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_switch_delete $rv\n");
        return rv;
    }
    
    return rv;
}
