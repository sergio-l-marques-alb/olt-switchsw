/* $Id$
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
*/

/*  
 * test 1: 
 * run: 
 * cint cint_mpls_mc.c
 * cint
 * mpls_mc_test1(0, <port_1>, <port_2>); 
 *  
 * run packet: 
 *      ethernet header with DA 0x11 and any SA
 *      vlan tag with vlan tag id 100
 *      mpls header with label 5000
 * from <port_1> 
 *  
 * the packet will arrive at <port_2> with: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 8000 and label_2 6000
 * after swap of label 5000 with label 6000 and push of label 8000 
 *  
 * test2: 
 * run: 
 * cint cint_mpls_mc.c
 * cint 
 * mpls_mc_test2(0, <port_1>, <port_2>);  
 *  
 * run same packet (as for test 1)
 *  
 * the packet will arrive at <port_2> with: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 300 and label_2 6000
 * after swap of label 5000 with label 6000 and push of label 300 by tunnel 1
 *  
 * or: 
 *      ethernet header with DA 0x44 and SA 0x11
 *      vlan tag with vlan tag id 201
 *      mpls header with label_1 400, label_2 200 and lable_3 6000
 * after swap of label 5000 with label 6000 and push of labels 400,200 by tunnel 2 
 *  
 * run packet: 
 *      ethernet header with DA 0x33 and any SA
 *      vlan tag with vlan tag id 101
 *      mpls header with label 5001
 * from <port_1>
 * 
 * the packet will arrive at <port_2> with looking the same,
 * except for the last label, which will be 6001 instead of 6000 
 *  
 * test3: 
 * run: 
 * cint cint_mpls_mc.c
 * cint 
 * mpls_mc_test3_explicit_flags_example(0, <port_1>, <port_2>);  
 *  
 * run same packet (as for test 1). One change: above MPLS header add L3 (IPV4) header 
 * Test example the ability to use explicit flags to indicate ACTION on MPLS tunnel initator.
 * Also provides ability to indicate ILM to do NOP action. 
 *  
 * Expectation in test is that label manipulation will be done only at egress (SWAP, PHP, PUSH, SWAP & PUSH, PUSH & PUSH)
 * five packets will arrive at <port_2> with: 
 * MPLS SWAP: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      after swap of label 5000 with label 6000
 * MPLS PHP (to IPV4): 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      l3 (ipv4) header: TTL, TOS not changed
 * MPLS PUSH: 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 400, label_2 200 and label_3 5000 (no swap)
 * MPLS SWAP & PUSH
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 400, label_2 200 and label_3 6000 (swap)
 * MPLS PUSH & PUSH (4 tunnels) 
 *      ethernet header with DA 0x22 and SA 0x11
 *      vlan tag with vlan tag id 200
 *      mpls header with label_1 400, label_2 200 and label_3 800, label_4 600, label_5 5000 (no swap)
 */ 

struct mpls_mc_info_s {
   
    /* ingress interface attribures: */

    int in_port; /* phy port */

    /* outer vlan */
    int vid_1;
    int vid_2;

    bcm_mac_t my_mac_1;
    bcm_mac_t my_mac_2;

    /* incomming label */
    int in_label_1; 
    int in_label_2; 

    int swap_label_1;
    int swap_label_2;

    /* ingress objects, packets will be routed to */
    bcm_if_t l3_ingress_intf_1;
    bcm_if_t l3_ingress_intf_2; 
  
    /* egress attribures: */

    int eg_port; /* phy port */

    /* egress label*/
    int eg_label_1;
    int eg_label_2;

    int eg_vid_1;
    int eg_vid_2;

    bcm_mac_t next_hop_mac_1;
    bcm_mac_t next_hop_mac_2;

    bcm_if_t l3_egress_intf_1;
    bcm_if_t l3_egress_intf_2;

    bcm_if_t encap_id_1;
    bcm_if_t encap_id_2;

    int mc_group;
};

mpls_mc_info_s mpls_mc_info;

uint8 pipe_mode_exp_set = 0;

int exp = 0;

int mpls_pipe_mode_exp_set(int unit) {

    int rv = BCM_E_NONE;

    if (pipe_mode_exp_set) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, pipe_mode_exp_set);
    }
    
    return rv;                
}

int is_arad_plus(int unit, int *yesno)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    *yesno = (((info.device == 0x8660) || (info.device == 0x8675)) ? 1 : 0);

    return rv;
}

void
mpls_mc_init(
    int in_port, 
    int out_port, 
    int in_label_base, 
    int swap_label_base, 
    int out_label_base, 
    int in_vlan_base, 
    int out_vlan_base) {

    mpls_mc_info.in_port = in_port;
    mpls_mc_info.eg_port = out_port;

    uint8 mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    uint8 next_mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    sal_memcpy(mpls_mc_info.my_mac_1, mac_1, 6);
    sal_memcpy(mpls_mc_info.next_hop_mac_1, next_mac_1, 6);

    uint8 mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    uint8 next_mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    sal_memcpy(mpls_mc_info.my_mac_2, mac_2, 6);
    sal_memcpy(mpls_mc_info.next_hop_mac_2, next_mac_2, 6);

    mpls_mc_info.mc_group = 5000;

    /* incomming */ 
    mpls_mc_info.vid_1 = in_vlan_base++;
    mpls_mc_info.vid_2 = in_vlan_base++;

    mpls_mc_info.in_label_1 = in_label_base++;
    mpls_mc_info.in_label_2 = in_label_base++;

    /* eg atrributes */
    mpls_mc_info.eg_label_1 = out_label_base++;
    mpls_mc_info.eg_label_2 = out_label_base++;

    mpls_mc_info.eg_vid_1 = out_vlan_base++;
    mpls_mc_info.eg_vid_2 = out_vlan_base++;

    mpls_mc_info.swap_label_1 = swap_label_base++;
    mpls_mc_info.swap_label_2 = swap_label_base++;

    mpls_mc_info.encap_id_1 = 0;
    mpls_mc_info.encap_id_2 = 0;
}

/* add switch entry to perform swap
   swap in_label with eg_label
   and point to egress-object: egress_intf, returned by create_l3_egress
   In case eg_label = 0 sequence set action to be NOP (i.e. dont touch label)
*/
int
mpls_add_switch_entry(int unit, int in_label, int eg_label,  bcm_if_t egress_intf, int multicast)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = (eg_label == 0) ? BCM_MPLS_SWITCH_ACTION_NOP:BCM_MPLS_SWITCH_ACTION_SWAP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
       in general valid options: 
       both present (uniform) or none of them (Pipe)
    */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    
    /* incomming label */
    entry.label = in_label;

    /* egress attribures*/
    entry.egress_label.label = eg_label;
    entry.egress_if = egress_intf;

    if (multicast) {
        /* add multicast group to entry */
        entry.flags |= BCM_MPLS_SWITCH_P2MP;
        entry.mc_group = mpls_mc_info.mc_group;
        printf("entry.mc_group:%08x\n", entry.mc_group);
    }

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* create l3 interface - ingress */
int create_l3_intf(int unit, int flags, int port, int vlan, bcm_mac_t my_mac_addr, int *intf) {

  int rc, station_id;
  bcm_l3_intf_t l3if, l3if_ori;
  bcm_l2_station_t station;

  bcm_l3_intf_t_init(&l3if);
  bcm_l2_station_t_init(&station); 

  /* set my-Mac global MSB */
  station.flags = 0;
  sal_memcpy(station.dst_mac, my_mac_addr, 6);
  station.src_port_mask = 0; /* port is not valid */
  station.vlan_mask = 0; /* vsi is not valid */
  station.dst_mac_mask[0] = 0xff; /* dst_mac my-Mac MSB mask is -1 */
  station.dst_mac_mask[1] = 0xff;
  station.dst_mac_mask[2] = 0xff;
  station.dst_mac_mask[3] = 0xff;
  station.dst_mac_mask[4] = 0xff;
  station.dst_mac_mask[5] = 0xff;

  rc = bcm_l2_station_add(unit, &station_id, &station);
  if (rc != BCM_E_NONE) {
      return rc;
  }

  /* before creating L3 INTF, check whether L3 INTF already exists*/
  bcm_l3_intf_t_init(&l3if_ori);
  l3if_ori.l3a_intf_id = vlan;
  rc = bcm_l3_intf_get(unit, &l3if_ori);
  if (rc == BCM_E_NONE) {
      /* if L3 INTF already exists, replace it*/
      l3if.l3a_flags = flags | BCM_L3_REPLACE | BCM_L3_WITH_ID;
      l3if.l3a_intf_id = vlan;
  }
  else {
      l3if.l3a_flags = flags; 
      l3if.l3a_intf_id = *intf;
  }

  sal_memcpy(l3if.l3a_mac_addr, my_mac_addr, 6);
  l3if.l3a_vid = vlan;
  l3if.l3a_ttl = 31;
  l3if.l3a_mtu = 1524;

  rc = bcm_l3_intf_create(unit, l3if);
  if (rc != BCM_E_NONE) {
      print rc;
      printf("Error, in bcm_l3_intf_create\n");
      return rc;
  }
  printf("create ingress:%08x\n", l3if.l3a_intf_id);
  *intf = l3if.l3a_intf_id;

  return rc;
}

/* set egress action over this l3 interface, so packet forwarded to this interface will be tunneled/swapped/popped */
int 
create_tunnel_initiator(int unit, bcm_mpls_egress_action_t egress_action, bcm_mpls_label_t label, int* ingress_intf) {
    bcm_mpls_egress_label_t label_array[2];
    int num_labels;
    int rv;
    int is_arad_plus;

    rv = is_arad_plus(unit, &is_arad_plus);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].exp = exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID);  
    if (!is_arad_plus || pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }

    if (egress_action == BCM_MPLS_EGRESS_ACTION_PHP) {
      label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV4;
    }
    label_array[0].label = label;
    label_array[0].ttl = 20;
    label_array[0].action = egress_action;
    label_array[0].l3_intf_id = *ingress_intf;
    num_labels = 1;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *ingress_intf = label_array[0].tunnel_id;
    return rv;
}


/* create l3 interface - egress */
int create_l3_egress(int unit, uint32 flags, int port, int vlan, int ingress_intf, bcm_mac_t nh_mac_addr, int label, int *intf, int *encap_id, int push) {

  int rc;
  bcm_l3_egress_t l3eg;
  bcm_l3_egress_t_init(&l3eg);
  
  bcm_if_t l3egid;

  int mod = 0;
  int test_failover_id = 0;
  int test_failover_intf_id = 0;

  sal_memcpy(l3eg.mac_addr, nh_mac_addr, 6);
  l3eg.vlan   = vlan;
  l3eg.module = mod;
  l3eg.port   = port;
  l3eg.failover_id = test_failover_id;
  l3eg.failover_if_id = test_failover_intf_id;
  l3eg.encap_id = *encap_id;
  l3egid = *intf; 

  if (push) {
      rc = create_tunnel_initiator(unit, BCM_MPLS_EGRESS_ACTION_PUSH, label, &ingress_intf);
      if (rc != BCM_E_NONE) {
          print rc;
          printf(" error, in create_tunnel_initiator\n");
          return rc;
      }      
  }

  l3eg.intf = ingress_intf;

  rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
  if (rc != BCM_E_NONE) {
      print rc;
      printf(" error, in bcm_l3_egress_create\n");
      return rc;
  }
  printf("create egress:%08x\n", l3egid);
  printf("create encap_id:%08x\n", l3eg.encap_id);
  *encap_id = l3eg.encap_id;
  *intf = l3egid;

  return rc;
}

/* 
 * single_config:  if =1 then only 1 vlan, tunnel, ingress_intf, egress_obj and switch_entry will be done
 *                 if =0 then 2 of each will be done
 * l3_egress_push: if =1 then l3_egress_create will config push of eg_label 
 *                 if =0 then no mpls action will be done in l3_egress_create
 * multicast:      if =1 a multicast group is used when creating mpls_add_switch_entry 
 *                 if =0 no multicast group is used
 */
int 
mpls_mc_config(int unit, int in_port, int out_port, int single_config, int l3_egress_push, int multicast){

    int CINT_NO_FLAGS = 0;
    int rv;
    int tmp_itf;
    bcm_pbmp_t pbmp, ubmp;

    mpls_mc_init(in_port, out_port, 5000 /*in_label*/, 6000 /*swap_label*/, 8000 /*out_label*/, 100 /*vlan*/, 200 /*out_vlan*/);
  
    /* create 2 vlans - incomming,outgoing and add ports to them */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.in_port);

    rv = bcm_vlan_create(unit, mpls_mc_info.vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }
       
    if (!single_config) {
        rv = bcm_vlan_create(unit, mpls_mc_info.vid_2);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.vid_2);
            return rv;
        }
        rv = bcm_vlan_port_add(unit, mpls_mc_info.vid_2, pbmp, ubmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }  
    }
    
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.eg_port); 

    rv = bcm_vlan_create(unit,mpls_mc_info.eg_vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.eg_vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }  

    if (!single_config) {
        rv = bcm_vlan_create(unit,mpls_mc_info.eg_vid_2);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_2);
            return rv;
        }
        
        rv = bcm_vlan_port_add(unit, mpls_mc_info.eg_vid_2, pbmp, ubmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }  
    }

    /* set l2 termination for mpls routing for the in_vlan */    
    rv = create_l3_intf(unit, 
                           CINT_NO_FLAGS, 
                           mpls_mc_info.in_port, 
                           mpls_mc_info.vid_1, 
                           mpls_mc_info.my_mac_1, 
                           &mpls_mc_info.l3_ingress_intf_1);
    /*rv = bcm_l2_tunnel_add(unit, mpls_mc_info.my_mac_1, mpls_mc_info.vid_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_tunnel_add\n");
        return rv;
    }*/

    if (!single_config) {
        rv = create_l3_intf(unit, 
                           CINT_NO_FLAGS, 
                           mpls_mc_info.in_port, 
                           mpls_mc_info.vid_2, 
                           mpls_mc_info.my_mac_2, 
                           &mpls_mc_info.l3_ingress_intf_1);
        /*rv = bcm_l2_tunnel_add(unit, mpls_mc_info.my_mac_2, mpls_mc_info.vid_2);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_tunnel_add\n");
            return rv;
        }*/
    }

    /* create an ingress object, packets will be routed to */
    rv = create_l3_intf(unit, 
                           CINT_NO_FLAGS, 
                           mpls_mc_info.eg_port, 
                           mpls_mc_info.eg_vid_1, 
                           mpls_mc_info.my_mac_1, 
                           &mpls_mc_info.l3_ingress_intf_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_intf with vid = %d, intf = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }

  
    /* open MC-group */
    rv = mpls_open_ingress_mc_group(unit, mpls_mc_info.mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_open_ingress_mc_group\n");
        return rv;
    }

    /* create egress object */
    rv = create_l3_egress(unit, 
                          CINT_NO_FLAGS, 
                          mpls_mc_info.eg_port, 
                          mpls_mc_info.eg_vid_1, 
                          mpls_mc_info.l3_ingress_intf_1, 
                          mpls_mc_info.next_hop_mac_1, 
                          mpls_mc_info.eg_label_1, 
                          &mpls_mc_info.l3_egress_intf_1, 
                          &mpls_mc_info.encap_id_1,
                          l3_egress_push);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress with vid = %d, intf = %d\n", mpls_mc_info.eg_vid_1, ingress_intf_1);
        return rv;
    }  

    if (!single_config) {
        rv = create_l3_egress(unit, 
                              CINT_NO_FLAGS, 
                              mpls_mc_info.eg_port, 
                              mpls_mc_info.eg_vid_1, 
                              mpls_mc_info.l3_ingress_intf_1, /* same ingress_intf is used */
                              mpls_mc_info.next_hop_mac_2, 
                              mpls_mc_info.eg_label_2, 
                              &mpls_mc_info.l3_egress_intf_2, 
                              &mpls_mc_info.encap_id_2,
                              l3_egress_push);
        if (rv != BCM_E_NONE) {
            printf("Error, in create_l3_egress with vid = %d, intf = %d\n", mpls_mc_info.eg_vid_2, mpls_mc_info.l3_ingress_intf_2);
            return rv;
        }  
    }
  
    /* swap push-label with egress-label and point to egress object */
    rv = mpls_add_switch_entry(unit, mpls_mc_info.in_label_1, mpls_mc_info.swap_label_1 ,mpls_mc_info.l3_egress_intf_1, multicast);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }

    if (!single_config) {
        rv = mpls_add_switch_entry(unit, mpls_mc_info.in_label_2, mpls_mc_info.swap_label_2 ,mpls_mc_info.l3_egress_intf_2, multicast);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_switch_entry\n");
            return rv;
        }
    }

    return rv;
}

/* performs swap of in_label with swap_label and then push of push_label */
int 
mpls_mc_test1(int unit, int in_port, int out_port){

    int rv;

    rv = mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = mpls_mc_config(unit, in_port, out_port, 1, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_mc_config\n");
        return rv;
    }

    return rv;
}

/* performs swap of in_label with swap_label and then creates a tunnel that pushes 1/2 labels */
int 
mpls_mc_test2(int unit, int in_port, int out_port){

    int rv;
    int egress_id, egress_id_base = 0x40000000;
    bcm_mpls_egress_label_t label_array[2];
    int is_arad_plus;

    rv = mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = is_arad_plus(unit, &is_arad_plus);
    if (rv < 0) {
        return rv;
    }

    rv = mpls_mc_config(unit, in_port, out_port, 0, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_mc_config\n");
        return rv;
    }

    /* set 2 mpls tunnels, one for each egress_intf */

    /* tunnel 1 is for 1 label */
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].exp = exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_arad_plus || pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = 300;
    label_array[0].ttl = 30;
    label_array[0].l3_intf_id = mpls_mc_info.l3_egress_intf_1;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* Connect Egress MPLS tunnel to encap_id (LL) */
    rv = create_l3_egress(unit, 
                          BCM_L3_EGRESS_ONLY | BCM_L3_REPLACE, 
                          mpls_mc_info.eg_port, 
                          mpls_mc_info.eg_vid_1, 
                          label_array[0].tunnel_id,
                          mpls_mc_info.next_hop_mac_1, 
                          0, 
                          &mpls_mc_info.l3_egress_intf_1, 
                          &mpls_mc_info.encap_id_1,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  


    rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, label_array[0].tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  

    /* tunnel 2 is for 2 labels */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].exp = exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_arad_plus || pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = 200;
    label_array[0].ttl = 20;

    label_array[1].exp = exp; 
    label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_arad_plus || pipe_mode_exp_set) {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[1].label = 400;
    label_array[1].ttl = 40;
    label_array[1].l3_intf_id = mpls_mc_info.l3_egress_intf_2;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /* Connect Egress MPLS tunnel to encap_id (LL) */
    rv = create_l3_egress(unit, 
                          BCM_L3_EGRESS_ONLY | BCM_L3_REPLACE, 
                          mpls_mc_info.eg_port, 
                          mpls_mc_info.eg_vid_1, 
                          label_array[0].tunnel_id,
                          mpls_mc_info.next_hop_mac_1, 
                          0, 
                          &mpls_mc_info.l3_egress_intf_1, 
                          &mpls_mc_info.encap_id_1,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  

    rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, label_array[0].tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", label_array[0].tunnel_id);
        return rv;
    }  

    return rv;
}

int
mpls_mc_test3_explicit_flags_example_aux(int unit, int in_port, int out_port, int pipe_mode_exp, int expected){

   pipe_mode_exp_set = pipe_mode_exp;

   exp = expected;

   return mpls_mc_test3_explicit_flags_example(unit,in_port,out_port);
}


/* 
 * Test example the ability to use explicit flags to indicate ACTION on MPLS tunnel initator.
 * Also provides ability to indicate ILM to do NOP action                                   .
 * See comments at the start of the file for more information.                                                                                         .
 */  
int 
mpls_mc_test3_explicit_flags_example(int unit, int in_port, int out_port){

    int rv;
    int egress_id, egress_id_base = 0x40000000;
    bcm_mpls_egress_label_t label_array[2];
    bcm_if_t push_tunnel_intf;
    int CINT_NO_FLAGS = 0;    
    int tmp_itf;
    bcm_if_t ingress_intf;
    bcm_pbmp_t pbmp, ubmp;
    int is_arad_plus;

    rv = mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = is_arad_plus(unit, &is_arad_plus);
    if (rv < 0) {
        return rv;
    }

    mpls_mc_init(in_port, out_port, 5000 /*in_label*/, 6000 /*swap_label*/, 8000 /*out_label*/, 100 /*vlan*/, 200 /*out_vlan*/);
    mpls_mc_info.mc_group = 6293;
  
    /* create 2 vlans - incomming,outgoing and add ports to them */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.in_port);

    rv = bcm_vlan_create(unit, mpls_mc_info.vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }
       
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_mc_info.eg_port); 

    rv = bcm_vlan_create(unit,mpls_mc_info.eg_vid_1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_mc_info.eg_vid_1, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }  

    /* set l2 termination for mpls routing for the in_vlan */    
    rv = create_l3_intf(unit, 
                           CINT_NO_FLAGS, 
                           mpls_mc_info.in_port, 
                           mpls_mc_info.vid_1, 
                           mpls_mc_info.my_mac_1, 
                           &mpls_mc_info.l3_ingress_intf_1);
  
    /* create an ingress object, packets will be routed to */
    rv = create_l3_intf(unit, 
                           CINT_NO_FLAGS, 
                           mpls_mc_info.eg_port, 
                           mpls_mc_info.eg_vid_1, 
                           mpls_mc_info.my_mac_1, 
                           &mpls_mc_info.l3_ingress_intf_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_intf with vid = %d, intf = %d\n", mpls_mc_info.eg_vid_1);
        return rv;
    }

  
    /* open MC-group */
    rv = mpls_open_ingress_mc_group(unit, mpls_mc_info.mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_open_ingress_mc_group\n");
        return rv;
    }

    /* create egress object */
    rv = create_l3_egress(unit, 
                          CINT_NO_FLAGS, 
                          mpls_mc_info.eg_port, 
                          mpls_mc_info.eg_vid_1, 
                          mpls_mc_info.l3_ingress_intf_1, 
                          mpls_mc_info.next_hop_mac_1, 
                          0, 
                          &mpls_mc_info.l3_egress_intf_1, 
                          &mpls_mc_info.encap_id_1,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress with vid = %d, intf = %d\n", mpls_mc_info.eg_vid_1, ingress_intf_1);
        return rv;
    }  

    /* do not swap label (Action NOP) and point to egress object */
    rv = mpls_add_switch_entry(unit, mpls_mc_info.in_label_1, 0 /* NOP */ , 0 , 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }

    /* set 4 mpls egress tunnels, one for each action */

    /* tunnel 1 is for PHP action */
    ingress_intf = mpls_mc_info.l3_ingress_intf_1;
    rv = create_tunnel_initiator(unit, BCM_MPLS_EGRESS_ACTION_PHP, 0, &ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_tunnel_initiator\n");
        return rv;
    }

    /* Connect Egress MPLS PHP to encap_id (LL) */
    rv = create_l3_egress(unit, 
                          BCM_L3_EGRESS_ONLY | BCM_L3_REPLACE, 
                          mpls_mc_info.eg_port, 
                          mpls_mc_info.eg_vid_1, 
                          ingress_intf,
                          mpls_mc_info.next_hop_mac_1, 
                          0, 
                          &mpls_mc_info.l3_egress_intf_1, 
                          &mpls_mc_info.encap_id_1,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress intf = %d\n", ingress_intf);
        return rv;
    }  

    /* Connect multicast group to MPLS PHP */
    rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
        return rv;
    }  

   /* tunnel 2 is for SWAP action */
   ingress_intf = mpls_mc_info.l3_ingress_intf_1;
   rv = create_tunnel_initiator(unit, BCM_MPLS_EGRESS_ACTION_SWAP, mpls_mc_info.swap_label_1, &ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in create_tunnel_initiator\n");
       return rv;
   }

   /* Connect Egress MPLS SWAP to encap_id (LL) */
   rv = create_l3_egress(unit, 
                         BCM_L3_EGRESS_ONLY | BCM_L3_REPLACE, 
                         mpls_mc_info.eg_port, 
                         mpls_mc_info.eg_vid_1, 
                         ingress_intf,
                         mpls_mc_info.next_hop_mac_1, 
                         0, 
                         &mpls_mc_info.l3_egress_intf_1, 
                         &mpls_mc_info.encap_id_1,
                         0);
   if (rv != BCM_E_NONE) {
       printf("Error, in create_l3_egress intf = %d\n", ingress_intf);
       return rv;
   }  

   /* Connect multicast group to MPLS SWAP */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

   /* tunnel 3 is for PUSH action */
   ingress_intf = mpls_mc_info.l3_ingress_intf_1;

   bcm_mpls_egress_label_t_init(&label_array[0]);
   bcm_mpls_egress_label_t_init(&label_array[1]);

   label_array[0].exp = exp; 
   label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID);  
   if (!is_arad_plus || pipe_mode_exp_set) {
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
   } else {
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
   }
   label_array[0].label = 200;
   label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
   label_array[0].ttl = 20;

   label_array[1].exp = exp; 
   label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID);  
   if (!is_arad_plus || pipe_mode_exp_set) {
       label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
   } else {
       label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
   }
   label_array[1].label = 400;
   label_array[1].action = BCM_MPLS_EGRESS_ACTION_PUSH;
   label_array[1].ttl = 40;
   label_array[1].l3_intf_id = ingress_intf;

   rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_initiator_create\n");
       return rv;
   }
   ingress_intf = label_array[0].tunnel_id;
   push_tunnel_intf = label_array[0].tunnel_id; /* save that for later use */

   /* Connect Egress MPLS PUSH to encap_id (LL) */
   rv = create_l3_egress(unit, 
                         BCM_L3_EGRESS_ONLY | BCM_L3_REPLACE, 
                         mpls_mc_info.eg_port, 
                         mpls_mc_info.eg_vid_1, 
                         ingress_intf,
                         mpls_mc_info.next_hop_mac_1, 
                         0, 
                         &mpls_mc_info.l3_egress_intf_1, 
                         &mpls_mc_info.encap_id_1,
                         0);
   if (rv != BCM_E_NONE) {
       printf("Error, in create_l3_egress intf = %d\n", ingress_intf);
       return rv;
   }  

   /* Connect multicast group to MPLS PUSH */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

   /* tunnel 4 is for SWAP & PUSH actions */
   /* Use the same PUSH tunnels from tunnel 3 */
   /* On top of them operate a SWAP action */
   ingress_intf = label_array[0].tunnel_id;

   bcm_mpls_egress_label_t_init(&label_array[0]);
   bcm_mpls_egress_label_t_init(&label_array[1]);

   label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID);  
   label_array[0].label = mpls_mc_info.swap_label_1;
   label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;
   label_array[0].l3_intf_id = push_tunnel_intf; /* Connect SWAP action to MPLS tunnel push */

   rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_initiator_create\n");
       return rv;
   }
   ingress_intf = label_array[0].tunnel_id;

   /* Connect multicast group to MPLS SWAP */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

   /* tunnel 4 is for SWAP & PUSH actions */
   /* Use the same PUSH tunnels from tunnel 3 */
   /* On top of them operate a SWAP action */
   ingress_intf = label_array[0].tunnel_id;

   bcm_mpls_egress_label_t_init(&label_array[0]);
   bcm_mpls_egress_label_t_init(&label_array[1]);

   
   label_array[0].exp = exp; 
   label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID);  
   if (!is_arad_plus || pipe_mode_exp_set) {
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
   } else {
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
   }
   label_array[0].label = 600;
   label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
   label_array[0].ttl = 60;

   label_array[1].exp = exp; 
   label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_ACTION_VALID);  
   if (!is_arad_plus || pipe_mode_exp_set) {
       label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
   } else {
       label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
   }
   label_array[1].label = 800;
   label_array[1].action = BCM_MPLS_EGRESS_ACTION_PUSH;
   label_array[1].ttl = 40;
   label_array[1].l3_intf_id = push_tunnel_intf; /* Connect SWAP action to MPLS tunnel push */

   rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_initiator_create\n");
       return rv;
   }
   ingress_intf = label_array[0].tunnel_id;

   /* Connect multicast group to MPLS PUSH & PUSH */
   rv = multicast_mpls_encap_add(unit, mpls_mc_info.mc_group, mpls_mc_info.eg_port, ingress_intf);
   if (rv != BCM_E_NONE) {
       printf("Error, in multicast_mpls_encap_add intf = %d\n", ingress_intf);
       return rv;
   }  

    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int multicast_mpls_encap_add(int unit, int mc_group_id, int sys_port, bcm_if_t egress_itf){

  int encap_id;
  int rv;

  rv = bcm_multicast_egress_object_encap_get(unit, mc_group_id, egress_itf, &encap_id);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_multicast_egress_object_encap_get mc_group_id:  0x%08x   gport:  0x%08x \n", mc_group_id, egress_itf);
    return rv;
  }

  rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
    return rv;
  }

  printf("encap_id:%08x\n", encap_id);
  printf("mc_group_id:%08x\n", mc_group_id);
  
  return rv;
}

/* multicast utilities */

int mpls_open_ingress_mc_group(int unit, int mc_group_id){
  int rv = BCM_E_NONE;
  int flags;
  bcm_multicast_t mc_group = mc_group_id;

  /* destroy before open, to ensure it not exist */
  rv = bcm_multicast_destroy(unit, mc_group);

  /* create ingress MC */
  flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_EGRESS_OBJECT;
  rv = bcm_multicast_create(unit, flags, &mc_group);
  if (rv != BCM_E_NONE) {
    printf("Error, in mc create, flags $flags mc_group $mc_group \n");
    return rv;
  }

   return rv;
}
