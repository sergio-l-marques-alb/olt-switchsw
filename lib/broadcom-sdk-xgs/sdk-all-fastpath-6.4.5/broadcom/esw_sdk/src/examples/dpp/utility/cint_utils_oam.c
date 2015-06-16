/*
 * $Id: oam.c,v 1.148 2013/09/17 10:45:12  Exp $
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
 * File:    cint_utils_oam.c
 * Purpose: Utility functions and variables to be used by all OAM/BFD cints
 */



  /* Enum signifiyng the device type. Should be ordered oldest device to newest*/
 enum device_type_t { 
      device_type_arad_a0=0,
      device_type_arad_plus=1,
      device_type_jericho=2
  } ;

device_type_t device_type;

  /* Enum signifying the eth format header */
 enum eth_tag_type_t { 
      untagged=0,
      single_tag=1,
      double_tag=2
  } ;

eth_tag_type_t eth_tag_type;


int get_device_type(int unit, device_type_t *device_type)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  
  *device_type = (info.device == 0x8650 && info.revision == 0) ? device_type_arad_a0 :
                            (info.device == 0x8660)? device_type_arad_plus :
                            ((info.device == 0x8375) || (info.device == 0x8675)) ? device_type_jericho:
                            -1;
  return rv;
}




/****************************************************************/
/*                    VSWITCH INITIALIZATION FUNCTIONS                                               */
/****************************************************************/



/**
 * Part of vswitch initialization process.
 * 
 * 
 * @param unit 
 * @param known_mac_lsb 
 * @param known_vlan 
 * 
 * @return int 
 */
int
vswitch_metro_run(int unit, int known_mac_lsb, int known_vlan){
    int rv;
    /*bcm_vlan_t  vsi*/;
    bcm_mac_t kmac;
    int index;
    bcm_vlan_t kvlan;
    int flags, i;

    kmac[5] = known_mac_lsb;
    kvlan = known_vlan;
  
    /* set ports to identify double tags packets */
    port_tpid_init(vswitch_metro_mp_info.sysports[0],1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
       printf("Error, port_tpid_set, in unit %d\n", unit);
       print rv;
       return rv;
    }

    port_tpid_init(vswitch_metro_mp_info.sysports[1],1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }

    port_tpid_init(vswitch_metro_mp_info.sysports[2],1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }
    /* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    for (index = 0; index < 12; index++) {
        if (single_vlan_tag && (((index % 2) != 0))) {
            continue;
        }
        rv = vlan__init_vlan(unit,vswitch_metro_mp_info.p_vlans[index]);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__init_vlan\n");
            print rv;
            return rv;
         }
    }

	/* 1. create vswitch + create Multicast for flooding */
	rv = vswitch_create(unit, &vsi, 0);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_create\n");
		print rv;
		return rv;
	}
    printf("Created vswitch vsi=0x%x\n", vsi);

	/* 2. create first vlan-port */

	flags = 0;
	gport1 = 0;
	rv = vswitch_metro_add_port(unit, 0, &gport1, flags);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_metro_add_port_1\n");
		print rv;
		return rv;
	}
	if(verbose){
		printf("created vlan-port   0x%08x in unit %d\n",gport1, unit);
	}

	/* 3. add vlan-port to the vswitch and multicast */
	rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[0], gport1);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_add_port\n");
		return rv;
	}
	/* add another port to the vswitch */

	/* 4. create second vlan-port */
	flags = 0;
	gport2 = 0;
	rv = vswitch_metro_add_port(unit, 1, &gport2, flags);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_metro_add_port_2\n");
		return rv;
	}
	if(verbose){
		printf("created vlan-port   0x%08x\n\r",gport2);
	}

	/* 5. add vlan-port to the vswitch and multicast */

	/* Local unit for sysport2 is already first */
	rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[1], gport2);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_add_port\n");
		return rv;
	}

	/* add a third port to the vswitch */
 
	/* 6. create third vlan-port */
	flags = 0;
	gport3 = 0;
	rv = vswitch_metro_add_port(unit, 2, &gport3, flags);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_metro_add_port_2\n");
		return rv;
	}
	if(verbose){
		printf("created vlan=port   0x%08x in unit %d\n",gport3, unit);
	}

	/* 7. add vlan-port to the vswitch and multicast */ 
	rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[2], gport3);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_add_port\n");
		return rv;
	}

	rv = vswitch_add_l2_addr_to_gport(unit, gport3, kmac, kvlan);
	if (rv != BCM_E_NONE) {
		printf("Error, vswitch_add_l2_addr_to_gport\n");
		return rv;
	}
    return rv;
}


/**
 * Initialize vswitch for OAM example usage.
 * 
 * 
 * @param unit 
 * 
 * @return int 
 */
int create_vswitch_and_mac_entries(int unit) {
  bcm_error_t rv;
  int flags;
   
  printf("Creating vswitch\n");
  bcm_port_class_set(unit, port_1, bcmPortClassId, port_1); 
  bcm_port_class_set(unit, port_2, bcmPortClassId, port_2); 
  bcm_port_class_set(unit, port_3, bcmPortClassId, port_3);

  vswitch_metro_mp_info_init(port_1, port_2, port_3);

  rv = vswitch_metro_run(unit, 0xce, 3);
  if (rv != BCM_E_NONE){
	  printf("Error, in vswitch_metro_run\n");
	  return rv;
  }
  
  printf("Adding mep MAC addresses to MAC table\n");
  /* int l2_entry_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id){*/
  rv = l2_entry_add(unit, mac_mep_1, 4096, 0, gport1, 0);                                       
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = l2_entry_add(unit, mac_mep_2, 4096, 0, gport2, 0);                                       
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("Creating multicast group\n");
  flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
  rv = bcm_multicast_create(unit, flags, mc_group);                                    
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = bcm_multicast_ingress_add(unit, mc_group, port_1, 0);                                    
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  /* Adding MC mac address of mep 2 to the multicast group */
  rv = l2_entry_add(unit, mac_mep_2_mc, 4096, 1, 0, mc_group);                                       
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}

int NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE = 0x4000; 
 int current_lm_counter_base=0;
 int calls_to_counter_config_set=0; /* Assuming no other application is calling counter_config_set()*/

/** 
 * Whenever creating an endpoint on a new LIF this function 
 *  should be called. The value returned in counter_base should
 *  go into the field lm_counter_base_id in
 *  bcm_oam_endpoint_create().
 *  
 *  Allocate counters. In Arad counters have been statically
 *  allocated via soc property, simply maintain a global
 *  variable.
 *  
 *  In Jericho Counter engines must be allocated on the fly.
 *  Each call to bcm_stat_counter_config_set() allocates 8 K
 *  counter IDs. Maintain a global counter and whenever counters
 *  ids have run out call counter_config_set() (i.e. on the
 *  first call, 8K-th call, etc.).
 *  Each call allocates 8 counter ids to account for PCP based
 *  LM.
 *  
 * 
 * @author sinai (28/12/2014)
 * 
 * @param unit 
 * @param counter_base - return value
 * 
 * @return int 
 */
int set_counter_source_and_engines(int unit, int * counter_base) {
	bcm_stat_counter_engine_t counter_engine ; 
	bcm_stat_counter_config_t counter_config ;
	int ingress;
	bcm_color_t colors[] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
	uint8 drop_fwd[] = { 0,1 }; 
	int index1, index2;
	int rv, egress_engine_id, ingress_engine_id;


	/* This should be initialized by now anyways, but check again*/
    rv = get_device_type(unit, &device_type);
	BCM_IF_ERROR_RETURN(rv); 


	if (device_type<device_type_jericho ) {
		/* In Arad the counters have been statically allocated via soc properties. Furthurmore LM-PCP is not available.
		   simply increment the counter and return.*/
		*counter_base = ++current_lm_counter_base;
		return 0;
	}

	if ( current_lm_counter_base % NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE == 0  ) {
		/* Ran out of available counters. Allocate another bunch.*/
		if (calls_to_counter_config_set >= 16) {
			print "Out of counter engines.";
			return 444;
		}

		counter_config.format.format_type = bcmStatCounterFormatPackets; /* This gives us 16K counter ids per engine, but only 29 bits for the counter.*/
		counter_config.format.counter_set_mapping.counter_set_size = 1;/* Since we are indifferent to color/droped-forwarded we only need one counter set.*/
		counter_config.format.counter_set_mapping.num_of_mapping_entries = 8; /* Using 8 entries for 8 PCP values.*/
		for (index1 = 0; index1 < bcmColorPreserve; index1++) {
			for (index2 = 0; index2 < 2; index2++) {
				/* Counter configuration is independent on the color, drop precedence.*/
				counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0; /* Must be zero since counter_set_size is 1 */
				counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
				counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
			}
		}
		counter_config.source.core_id = 0; /* for now*/
		counter_config.source.pointer_range.start =(current_lm_counter_base / NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE)  *NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE ;
		counter_config.source.pointer_range.end = ((current_lm_counter_base / NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE) +1)  *NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE -1;

		counter_config.source.engine_source = bcmStatCounterSourceIngressOam;
		counter_engine.engine_id=  calls_to_counter_config_set;
		rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
		BCM_IF_ERROR_RETURN(rv); 

		/* Two calls with the same paramters: one for the ingress and one for the egress
		counter_config.source.engine_source = bcmStatCounterSourceEgressOam;
		counter_engine.engine_id=  calls_to_counter_config_set +1;
		rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
		BCM_IF_ERROR_RETURN(rv); 
*/
		calls_to_counter_config_set +=1;
	}

	/* At this stage the counter source has been allocated. Return the counter base and get out.*/
	current_lm_counter_base += 16; /* Increment by 16 to allow for PCP based LM.*/
                /* If PCP lm is used 16 counter sources are needed per MEP: 8 for the ingress and 8 for the egress.*/
	*counter_base = current_lm_counter_base; /* ID 0 cannot be used - the value 0 is reserved to signify disabling LM counters.*/
	return 0; 
}




/**
 *  Function creates an accelerated endpoint.
 *  group_info_short_ma  must be initialized before this
 *  function is called.
 *  Likewise the function create_vswitch_and_mac_entries() must
 *  also  be called before this function.
 *  
 * @author liat 
 * 
 * @param unit 
 * @param is_up - direction of the endpoint.
 * @param eth_tag_type - vlan tag format.
 * 
 * @return int 
 */
int create_oam_accelerated_endpoint(int unit , int is_up, eth_tag_type_t eth_tag_type) {
   bcm_oam_endpoint_info_t  acc_endpoint;
   bcm_oam_endpoint_info_t_init(&acc_endpoint);

  /*TX*/
  acc_endpoint.type = bcmOAMEndpointTypeEthernet;
  acc_endpoint.group = group_info_short_ma.id;
  acc_endpoint.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  acc_endpoint.timestamp_format = get_oam_timestamp_format(unit);;

  switch (eth_tag_type) {
  case untagged:
          acc_endpoint.level = 4;
		  acc_endpoint.vlan = 0; 
		  acc_endpoint.pkt_pri = 0;
		  acc_endpoint.outer_tpid = 0;
		  break;
  case single_tag:
        acc_endpoint.level = 2;
		  acc_endpoint.vlan = 5; 
		  acc_endpoint.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
		  acc_endpoint.outer_tpid = 0x8100;
		  break;
	  case double_tag:
		  acc_endpoint.vlan = 10;
		  acc_endpoint.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
		  acc_endpoint.outer_tpid = 0x8100;
		  acc_endpoint.inner_vlan = 5;
		  acc_endpoint.inner_pkt_pri = 3;
		  acc_endpoint.inner_tpid = 0x8100;
          acc_endpoint.level = 3;
		  break;
	   default:
		   printf("Error, non valid eth_tag_type\n");
	  }

  if (is_up) {
        /*TX*/
      acc_endpoint.level = 5;
      acc_endpoint.name = 123;     
      acc_endpoint.flags |= BCM_OAM_ENDPOINT_UP_FACING;
      acc_endpoint.int_pri = 3 + (1<<2);
      /* The MAC address that the CCM packets are sent with*/
      src_mac_mep_2[5] = port_2;
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_2, 6);
      /*RX*/
      acc_endpoint.gport = gport2;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_2, 6);
      acc_endpoint.lm_counter_base_id = 6;
      acc_endpoint.tx_gport = BCM_GPORT_INVALID;
  } else { /** Down*/
      
      BCM_GPORT_SYSTEM_PORT_ID_SET(acc_endpoint.tx_gport, port_1);
      acc_endpoint.name = 456;     
      acc_endpoint.ccm_period = 100;
      acc_endpoint.int_pri = 1 + (3<<2);
      /* The MAC address that the CCM packets are sent with*/
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_3, 6);

      /*RX*/
      acc_endpoint.gport = gport1;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_3, 6);
      acc_endpoint.lm_counter_base_id = 5;
  }

  return  bcm_oam_endpoint_create(unit, &acc_endpoint);
}

