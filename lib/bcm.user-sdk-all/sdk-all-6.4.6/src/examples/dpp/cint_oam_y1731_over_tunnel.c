/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: cint_oam_y1731.c,v 1.8 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File: cint_oam_y13731.c
 * Purpose: Example of using Y.1731 OAM over MPLS-TP/PWE. 
 *
 * Usage:
 * 
 MPLS comes in two version, one in which the in/out LIFs are presumed to be symetric:
 cint utility/cint_utils_l3.c
 cint cint_ip_route.c
 cint cint_oam_y1731_over_tunnel.c
 cint
 print bcm_oam_init(0);
 print oam_run_with_defaults_mpls_tp(0,13,14,15,0);
 
 "New" MPLS calling sequence, using assymetric LIFs (for LMM counting purposes). MPLS LIFs created with mpls_lsr_tunnel_example()
 cint cint_qos.c
 cint cint_multi_device_utils.c
 cint utility/cint_utils_l3.c
 cint cint_mpls_lsr.c
 cint  cint_oam_y1731_over_tunnel.c
 cint cint_system_vswitch_encoding.c
 cint
 print bcm_oam_init(0);
 print oam_run_with_defaults_mpls_tp(0,13,14,15,1);
 
 cint utility/cint_utils_global.c
 cint utility/cint_utils_mpls.c
 cint cint_port_tpid.c
 cint cint_qos.c cint_vswitch_vpls.c
 cint cint_oam_y1731_over_tunnel.c
 cint utility/cint_utils_l3.c
 cint cint_mpls_lsr.c
 cint cint_advanced_vlan_translation_mode.c
 cint
  print bcm_oam_init(0);
  print oam_run_with_defaults_pwe(0,13,14,15);
 
 * 
 * This cint uses cint_vswitch_metro_mp_single_vlan.c to build the following vswitch:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \vlan        vlan         o                      |
 *  |                    \  \<4>        <10>/  /                            |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |             \  \  |    \  /     |\/ /  /                              |
 *  |                     \  \ |     \/      |  /  /                        |
 *  |                    p3=15\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1 = 13                      |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2=14----------                                |
 *  |         vlan <5>/  /  /                                               |
 *  |                  /  /  /                                              |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * The following MAs:
 *         1) MA with id 1 & short name format:    1-const; 3-short format; 2-length; all the rest - MA name
 *        short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xcd, 0xab}
 * 
 * The following MEPs:
 *         1) MEP with id 4096: accellerated, mdlevel 7,
 *      3) RMEP with id 0
 * 
 * In addition, get & delete APIs are used for testing.
 * 
 * Event callback registration example is provided
 * 
 * comments:
 * 1) In order to prevent from OAMP send packets do: BCM.0> m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0
 * 
 */
 
/*
 * Creating vswitch and adding mac table entries
 */

/* Globals - MAC addresses , ports & gports*/
  bcm_multicast_t mc_group = 1234;
  bcm_gport_t gport1, gport2, gport3; /* these are the ports created by the vswitch*/
  uint32 port_1 = 13; /* physical port (signal generator)*/
  uint32 port_2 = 14;
  uint32 port_3 = 15;
  bcm_oam_group_info_t group_info_short_ma;
  bcm_oam_endpoint_info_t mep_acc_info;
  bcm_oam_endpoint_info_t rmep_info;

  int mpls_label = 100;
  int next_hop_mac;

  int timeout_events_count = 0;


int read_timeout_event_count(int expected_event_count) {
    printf("timeout_events_count=%d\n",timeout_events_count);
    if (timeout_events_count==expected_event_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

/* 
* Add GAL label in order to avoit trap unknown_label
* Point to egress-object: egress_intf, returned by create_l3_egress
*/
int
mpls_add_gal_entry(int unit)
{
    int rv;
	int mpls_termination_label_index_enable;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    
    /* GAL label */
    entry.label = 13;

	/* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	if (mpls_termination_label_index_enable) {
		BCM_MPLS_INDEXED_LABEL_SET(&entry.label,13,2);
	}
    
    /* egress attribures*/   
    entry.egress_if = 0;    
    
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}


/* 
 * Add term entry to perform pop
 */
int
mpls_add_term_entry_ex(int unit, int term_label, uint32 next_prtcl, bcm_mpls_tunnel_switch_t *entry)
{
    int rv;
	int mpls_termination_label_index_enable;
    
    bcm_mpls_tunnel_switch_t_init(entry);
    entry->action = BCM_MPLS_SWITCH_ACTION_POP;

    /* 
     * Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry->flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    /* 
     * Next protocol indication: 
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6, or      
     * BCM_MPLS_SWITCH_NEXT_HEADER_L2, or 
     * 0 - unset(MPLS) 
     */
    entry->flags |= next_prtcl;
    
    /* incomming label */
    entry->label = term_label;

	/* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	if (mpls_termination_label_index_enable) {
		BCM_MPLS_INDEXED_LABEL_SET(&entry->label,term_label,1);
	}
    
    entry->qos_map_id = 0; /* qos not rellevant for BFD */
    
    rv = bcm_mpls_tunnel_switch_create(unit,entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* creating l3 interface */


int l3_interface_init(int unit, int in_sysport, int out_sysport, int * _next_hop_mac_encap_id, int * encap_id_l2, int is_mpls){
    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int flags1;
    int in_vlan = 1; 
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0};
    int route;
    int mask;
    int l3_eg_int;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

	mpls__egress_tunnel_utils_s mpls_tunnel_properties;
	if (device_type == device_type_jericho) {
		
		encap_id[0] = 0x1000;
		encap_id[1] = 0x4000;
	}

    /*** create ingress router interface ***/
	rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
	if (rv != BCM_E_NONE) {
		printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
	}
	rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
	if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
	  return rv;
	}

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress router interface ***/
	rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
	if (rv != BCM_E_NONE) {
		printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
	}
	rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
	if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
	  return rv;
	}

    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress object 1 ***/
    if (is_mpls) {
        /*** Create tunnel ***/
		mpls_tunnel_properties.label_in = 200;
		mpls_tunnel_properties.label_out = 0;
		mpls_tunnel_properties.next_pointer_intf = ing_intf_out;
	
		printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
		if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }

        l3_eg_int = mpls_tunnel_properties.tunnel_id;
    }
    else {
        l3_eg_int = ing_intf_out;
    }

    /*** Create egress object1 ***/
    flags1 = 0;
    rv = create_l3_egress(unit,flags1,out_sysport,out_vlan,l3_eg_int,next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
    }
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
    }

    /* Add another label for swapping.*/
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    /* incomming label */
    entry.label = 120; /*mpls labels 120 will get swapped (100 will get terminated)*/
    entry.egress_label.label =mpls_label +2;
    entry.egress_if = encap_id[0];
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
    }

    /*** create egress object 2***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */    
    flags1 = 0;
    rv = create_l3_egress(unit,flags1,out_sysport,out_vlan,l3_eg_int,next_hop_mac2, &fec[1], &encap_id[1]); 
    *_next_hop_mac_encap_id = encap_id[1];
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
    }
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d\n", fec[1], unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
    }
    *encap_id_l2 = encap_id[0];

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    rv = add_route(unit, route, mask , vrf, fec[1]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_sysport=%d in unit %d, \n", in_sysport, unit);
    }
    return rv;
}

/*
 * Creating vpls tunnel and termination
 */
int pwe_init(int unit) {
  bcm_error_t rv;
  bcm_mpls_tunnel_switch_t tunnel_switch;

  mpls_lsr_init(port_1,port_2, 0, next_hop_mac, mpls_label, mpls_label, 0, 0 ,0);
  rv = vswitch_vpls_run_with_defaults_dvapi(0,port_1,port_2);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = mpls_add_term_entry_ex(0, mpls_label, 0, &tunnel_switch);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }
  return rv;
}



/*
 * Creating mpls tunnel and termination
 */
int mpls_init(int unit, bcm_mpls_tunnel_switch_t *tunnel_switch, int *encap_id) {
  bcm_error_t rv;

  rv = l3_interface_init(0, port_1, port_2, &next_hop_mac, encap_id, 1);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }


  rv = mpls_add_term_entry_ex(0, mpls_label, 0, tunnel_switch);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = mpls_add_gal_entry(unit);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }
  return rv;
}

/* Send a packet from string input */
int tx(int unit, char *data, int *ptch) {
    char tmp, data_iter;
    int data_len, i, pkt_len, ind, data_base;
    bcm_pkt_t *pkt_info;
    bcm_error_t rv;
    
    /* If string data starts with 0x or 0X, skip it */
    if ((data[0] == '0') && ((data[1] == 'x') || (data[1] == 'X'))) {
        data_base = 2;
    } else {
        data_base = 0;
    }
    
    /* figure out packet data_length (no strdata_len in cint) */
    data_len=0;
    i=0;
    while (data[data_base+data_len] != 0) ++data_len;
    
    pkt_len = 64+2; /* two ptch bytes */
    pkt_len = (pkt_len < data_len+1 ? (data_len+2) : pkt_len);
    
        rv = bcm_pkt_alloc(unit, pkt_len, BCM_TX_CRC_ALLOC, &pkt_info);
        BCM_IF_ERROR_RETURN(rv);
        
        sal_memset(pkt_info->_pkt_data.data, 0, pkt_len);
    
        pkt_info->pkt_data = &pkt_info->_pkt_data;

    /* PTCH - 2B */
    pkt_info->_pkt_data.data[0] = ptch[0];
    pkt_info->_pkt_data.data[1] = ptch[1];

    /* Convert char to value */
    i = 0;
    while (i < data_len) {
        data_iter=data[data_base+i];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else {
            printf("Unexpected char: %c\n", data_iter);
            return BCM_E_PARAM;
        }       
        
        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */        
        pkt_info->_pkt_data.data[2+i/2] |= tmp << (((i+1)%2)*4);
        ++i;
    }
    
    rv = bcm_tx(unit, pkt_info, NULL);
    
    return rv;
}

/**
 * Create accelerated y.1731 OAM endpoint of type MPLS-TP or 
 * PWE. 
 * 
 * 
 * @param unit 
 * @param type May be bcmOAMEndpointTypeBHHMPLS or 
 *             bcmOAMEndpointTypeBHHPwe
 * @param use_mpls_out_gport: used for TX counting 
 * 
 * @return int 
 */
int oam_example_over_tunnel(int unit,  bcm_oam_endpoint_type_t type, int use_mpls_out_gport) {
  bcm_error_t rv;
  bcm_oam_endpoint_info_t mep_acc_test_info;
  bcm_oam_endpoint_info_t rmep_test_info;
  bcm_mpls_tunnel_switch_t tunnel_switch;
  bcm_oam_endpoint_info_t default_info;
  int encap_id;
  int gport;
  int outlif , label;
  bcm_gport_t out_gport = use_mpls_out_gport? 0 : BCM_GPORT_INVALID;


  int lm_counter_base_id_1 ;
  /*1-const; 3-short format; 2-length; all the rest - MA name*/
  uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};

  if (type==bcmOAMEndpointTypeBHHMPLS) {
      if (out_gport==BCM_GPORT_INVALID) {
          /* In this case use the "standard" mpls_init function.*/
          printf("mpls_init"); 
          rv = mpls_init(unit, &tunnel_switch, &encap_id);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n",bcm_errmsg(rv));
              return rv;
          }
          gport =  tunnel_switch.tunnel_id;
          outlif = encap_id;
          label = BCM_MPLS_INDEXED_LABEL_VALUE_GET(tunnel_switch.label);/* Represents label on transmitted frames from the OAMP*/
      } else {
          /* Use the mpls_lsr example*/
          rv = mpls_lsr_tunnel_example(&unit, 1,port_1, port_2);
          if (rv != BCM_E_NONE) {
              printf("MPLS LSR example (%s) \n", bcm_errmsg(rv));
              return rv;
          }
          gport = ingress_tunnel_id ;

          out_gport = mpls_lsr_info_1.ingress_intf;
          BCM_GPORT_TUNNEL_ID_SET(out_gport,mpls_lsr_info_1.ingress_intf );
          outlif = mpls_lsr_info_1.encap_id; 
          rv = mpls_tunnel_id_to_mpls_tunnel_gport(unit,&out_gport);
          BCM_IF_ERROR_RETURN(rv); 

           label = 0x1234;/* Represents label on transmitted frames from the OAMP*/

          rv = mpls_add_gal_entry(unit);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n", bcm_errmsg(rv));
              return rv;
          }
      }
       
  } else if (type==bcmOAMEndpointTypeBHHPwe) {
      printf("pwe_init\n");
      pwe_cw=1; 
      rv = pwe_init(unit);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
      gport = mpls_lsr_info_1.mpls_port_id; /*in lif: mpls_port->mpls_port_id */ 
      outlif = mpls_lsr_info_1.encap_id; /* out lif: mpls_port->encap_id */
      label = 100; /* Represents label on transmitted frames from the OAMP*/
  } else {
      printf("Only PWE and MPLS types supported."); 
      return 43;
  }

  rv = set_counter_source_and_engines(unit, &lm_counter_base_id_1,port_1);
  BCM_IF_ERROR_RETURN(rv); 


  printf("Creating group short name format\n");
  bcm_oam_group_info_t_init(&group_info_short_ma);
  sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_short_ma);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  if (!is_device_or_above(unit,ARAD_PLUS)) { 
	  /*
	  * Adding a default MEP
	  */
	  printf("Add default mep\n"); 
	  bcm_oam_endpoint_info_t_init(&default_info);
	  default_info.id = -1;
	  default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
	  rv = bcm_oam_endpoint_create(unit, &default_info);
	  if (rv != BCM_E_NONE) {
		  printf("(%s) \n",bcm_errmsg(rv));
		  return rv;
	  }
  }

  /*
  * Adding acc MEP
  */

  bcm_oam_endpoint_info_t_init(&mep_acc_info);
  /*RX*/
  mep_acc_info.type = type;
  mep_acc_info.group = group_info_short_ma.id;
  mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  mep_acc_info.level = 7; /*Y1731 level*/
  mep_acc_info.gport =gport; /* in lif */   
  mep_acc_info.mpls_out_gport =out_gport; /* out lif */   
  mep_acc_info.lm_counter_base_id  = lm_counter_base_id_1;

  /*TX*/
  BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_2); /* port that the traffic will be transmitted on */
  mep_acc_info.name = 123;     
  mep_acc_info.ccm_period = 1;
  mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

  mep_acc_info.intf_id = outlif;
  mep_acc_info.egress_label.label = label;
  mep_acc_info.egress_label.ttl = 0xa;
  mep_acc_info.egress_label.exp = 1;
  mep_acc_info.timestamp_format = soc_property_get(unit,"oam_dm_ntp_enable",1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;
       
  printf("bcm_oam_endpoint_create mep_acc_info\n"); 
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  printf("created MEP with id %d\n", mep_acc_info.id);

  /*
  * Adding Remote MEP
  */
  bcm_oam_endpoint_info_t_init(&rmep_info);
  rmep_info.name = 0xff;
  rmep_info.local_id = mep_acc_info.id;
  rmep_info.type = type;
  rmep_info.ccm_period = 0;
  rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info.loc_clear_threshold = 1;

  printf("bcm_oam_endpoint_create RMEP\n"); 
  rv = bcm_oam_endpoint_create(unit, &rmep_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }
  printf("created RMEP with id %d\n", rmep_info.id);

  bcm_oam_endpoint_info_t_init(&rmep_test_info);
  printf("bcm_oam_endpoint_get rmep_test_info\n"); 
  rv = bcm_oam_endpoint_get(unit, mep_acc_info.id, &rmep_test_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  /* Here rmep_info and rmep_test_info should be compared */

  return rv;
}


/**
 * OAM_endpoint_action_set calling sequence example.
 * 
 * @param unit 
 * @param port - Must be BCM_GPORT_INVALID for actions requiring
 *             invalid gport.
 * @param endpoint_id 
 * @param action_type 
 * @param opcode - OAM opcode upon which action will be applied
 * 
 * @return int 
 */
int oam_action_set(int unit, int port, int endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
  bcm_error_t rv;
  bcm_oam_endpoint_action_t action;
  int rx_trap;

  BCM_OAM_ACTION_CLEAR_ALL(action);
  BCM_OAM_OPCODE_CLEAR_ALL(action);
  action.flags = 0;
  BCM_GPORT_LOCAL_SET(action.destination, port); 

  BCM_OAM_OPCODE_SET(action, opcode); 
  BCM_OAM_ACTION_SET(action, action_type );


  rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
  rx_trap = BCM_GPORT_TRAP_GET_ID(action.rx_trap);
  printf( "rx_trap = %d\n",rx_trap);
  return rv;
}

/* OAM-TS header:
 *
 * OAM-TS header is the following:
 * bits                       meaning
 * ===================================
 * 47:46                      type of OAM-TS extension: 0-OAM 
 * 45:43                      OAM-Sub-Type: 0-CCM; 1-LM; 2-DM (1588 ToD) 3-DM (NTP ToD)
 * 42                         Up-MEP ('1') or Down-MEP ('0')
 * 41:8                       OAM-TS-Data: Interpretation of OAM-TS-Data depends on the OAM-TS type, sub type, and location in the system.
 *                                        Location in the system may be:
 *                                        IRPP - following ITMH on an injected packet
 *                                        ETPP or Out I/F - following FTMH
 *                                        OAM-Data is a function of (Location, Sub-Type, MEP-Type, Inject/Trap):
 *                                        "    (IRPP, LM, Up, Inj) - NA
 *                                        "    (IRPP, LM, Dn, Inj) - Counter-Pointer // bypass
 *                                        "    (IRPP, DM, Up, Inj) - NA
 *                                        "    (IRPP, DM, Dn, Inj) - Null // bypass
 *                                        "    (IRPP, LM/DM, Up/Dn, Trp) - NA
 *                                        "    (ETPP, LM, Up, Inj) - Counter-Value // Stamp into packet
 *                                        "    (ETPP, LM, Dn, Inj) - Counter-Pointer // Read counter and stamp into packet
 *                                        "    (ETPP, DM, Up, Inj) - ToD // Stamp into packet
 *                                        "    (ETPP, DM, Dn, Inj) -Null //Read ToD and stamp into packet
 *
 *                                        "    (ETPP, LM, Up, Trp) - NA // ETPP build OAM-TS Header w/ Counter-Value
 *                                        "    (ETPP, LM, Dn, Trp) - Counter-Value // bypass to Out I/F
 *                                        "    (ETPP, DM, Up, Trp) - NA // ETPP build OAM-TS Header w/ ToD
 *                                          "    (ETPP, DM, Dn, Trp) - ToD// bypass to Out I/F
 * 7:0                         Offset from end of System Headers to where to stamp/update.
 * 
 * parsing pkt_dm_down packet:
 *     ITMH      OAM-TS                ETH.               VLAN    MPLS LSP   GAL     G-ACH           PDU  
 * 0x800c000d|180000000022|00000000cd1d000c000200008100|00648847|0006 420a|0000D140|10008202|e02f002000000000000003e70000 
 * ITMH - send packet to port 13
 * OAM-TS - packet type DM, stamping offset - 0x22
 * 
 * parsing pkt_lm_down packet:
 * 0x800c000d|080000000522|00000000cd1d000c000200008100|00648847|0006 420a|0000D140|10008202|e02b000c000000000000000000000000
 *    ITMH       OAM-TS            ETH.                  VLAN     MPLS LSP   GAL     G-ACH           PDU  
 * ITMH - send packet to port 13
 * OAM-TS - packet type LM, stamping offset - 16, counter id - 5
 */
int inject_dm_and_lm_packets(int unit) {
  bcm_error_t rv;
  char *pkt_dm_down, *pkt_lm_down;
  int ptch_down[2];

  pkt_dm_down = "800c000d18000000002200000000cd1d000c000200008100006488470006420a0000D14010008202e02f002000000000000003e70000"; /*DM down*/
  pkt_lm_down = "800c000d08000000052200000000cd1d000c000200008100006488470006420a0000D14010008202e02b000c00000000000000000000"; /*LM down*/
  ptch_down[0] = 0; /* The next header is ITMH */
  ptch_down[1] = 0; /* in port is port_1 */

  rv = tx(unit, pkt_lm_down, ptch_down);
  rv = tx(unit, pkt_dm_down, ptch_down);
  return rv;
}

/* This is an example of using bcm_oam_event_register api.
 * A simple callback is created for CCM timeout event. 
 * After a mep and rmep are created, the callback is called 
 * whenever CCMTimeout event is generated. 
 */
int cb(
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
    {
        print unit;
        print flags;
        print event_type;
        print group;
        print endpoint;

        timeout_events_count++;

        return BCM_E_NONE;
    }

int register_events(int unit) {
  bcm_error_t rv;
  bcm_oam_event_types_t timeout_event, timein_event;

  BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);

  rv = bcm_oam_event_register(0, timeout_event, cb, (void*)1);

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);

  rv = bcm_oam_event_register(0, timein_event, cb, (void*)2);
  return rv;
}

/**
 * 
 * 
 * 
 * @param unit 
 * @param port1 Port on which the MEP will exist.
 * @param port2 
 * @param port3 
 * @param use_mpls_out_gport - If set, MPLS outLIF on will be 
 *                           associated with given value
 * 
 * @return int 
 */
int oam_run_with_defaults_mpls_tp (int unit, int port1, int port2, int port3, int use_mpls_out_gport) {
  bcm_error_t rv;
  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  rv = mpls__mpls_pipe_mode_exp_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
      return rv;
  }

  rv = get_device_type(unit, &device_type);
  if (rv < 0) {
	  printf("Error checking whether the device is arad+.\n");
	  print rv;
	  return rv;
  }

  rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeBHHMPLS, use_mpls_out_gport);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = register_events(unit);
  return rv;
}



int oam_run_with_defaults_pwe (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  rv = mpls__mpls_pipe_mode_exp_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
      return rv;
  }
  


  rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeBHHPwe, 0);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = register_events(unit);
  return rv;
}
