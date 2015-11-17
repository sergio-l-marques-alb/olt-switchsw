/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: oam.c,v 1.140 Broadcom SDK $
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
 * File: cint_oam_acceleration_advanced_features.c
 * Purpose: Example of using OAM APIs specific for arad +
 *
 * Usage:
 * 
   cd
   cd ../../../../src/examples/dpp
   cint cint_port_tpid.c
   cint cint_l2_mact.c
   cint cint_vswitch_metro_mp.c
   cint cint_oam_acceleration_advanced_features.c
   cint
   int unit=0;
   int port1=13,port2=14,port3=15;
   print bcm_oam_init(unit);
   print oam_lm_dm_run_with_defaults(unit,port1,port2,port3);
 * 
 * This cint uses cint_vswitch_metro_mp_single_vlan.c to build the following vswitch:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \                         o                      |
 *  |                      \  \<4>        <10,20>/  /                       |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |                <40>\  \  |    \  /     |\/ /  /                       |
 *  |                     \  \ |     \/      |  /  /<30>                    |
 *  |                       p3\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1                           |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2  -----------                                |
 *  |                <5>/  /  /                                             |
 *  |                  /  /  /<3>                                           |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
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
  bcm_mac_t mac_mep_1 = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
  bcm_mac_t mac_mep_2 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
  bcm_mac_t mac_mep_3 = {0x00, 0x00, 0x00, 0x01, 0x02, 0xff};
  bcm_mac_t mac_mip = {0x00, 0x00, 0x00, 0x01, 0x02, 0xfe};
  bcm_mac_t src_mac_mep_2 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  bcm_mac_t src_mac_mep_3 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x01};
  bcm_mac_t mac_mep_2_mc = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x35}; /* 01-80-c2-00-00-3 + md_level_2 */
  bcm_gport_t gport1, gport2, gport3; /* these are the ports created by the vswitch*/
  uint32 port_1 = 13; /* physical port (signal generator)*/
  uint32 port_2 = 14;
  uint32 port_3 = 15;
  bcm_oam_group_info_t group_info_long_ma;
  bcm_oam_group_info_t group_info_short_ma;
  bcm_oam_endpoint_info_t mep_acc_info;
  bcm_oam_endpoint_info_t rmep_info;
  /*1-const; 32-long format; 13-length; all the rest - MA name*/
  uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0xa, 0xb, 0xc, 0xd};
  /*1-const; 3-short format; 2-length; all the rest - MA name*/
  uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};

  int timeout_events_count = 0;
  bcm_vlan_t  vsi;
  int md_level_mip = 7;
  bcm_oam_endpoint_info_t mip_info;

  int counter =0;
  int mep_id = 4096;


int is_arad_plus(int unit, int *yesno)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  *yesno = (((info.device == 0x8660) || (info.device == 0x8670)) ? 1 : 0);

  return rv;
}


int is_jericho(int unit, int *yesno)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    *yesno = (info.device == 0x8670 ? 1 : 0);

    return rv;
}



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
        rv = init_vlan(unit,vswitch_metro_mp_info.p_vlans[index]);
        if (rv != BCM_E_NONE) {
            printf("Error, init_vlan\n");
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



int cb_oam_ais(int unit, uint32 flags, bcm_oam_event_type_t event_type, bcm_oam_group_t group,
               bcm_oam_endpoint_t endpoint, void *user_data) {
    bcm_oam_ais_t ais;
    int rv; 


    ais.id = endpoint & 0x3fff ; /* endpoint represents that of the RMEP. 
         14 LSBs of the endpoint represent the HW ID which is identical to that of the local MEP.*/
    ais.period =BCM_OAM_ENDPOINT_CCM_PERIOD_1S; /**/
    ais.flags |= BCM_OAM_AIS_MULTICAST;
    ais.level = 7;

    rv = bcm_oam_ais_add(unit,&ais);
    if (rv!=BCM_E_NONE ) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    return BCM_E_NONE;
}


/**
 * Create an accelerated MEP with ID 4096 and level 2 and a MEP
 * with ID 4100 and level 5. 
 * MEP 4096 will have a loss (with statistics extended), delay, 
 * and loopback-reply entry. 
 *  
 * MEP 4100 will have a piggy-back loss entry.
 * 
 * @author sinai (09/09/2013)
 * 
 * @param unit 
 * @param port1 physical ports to be used
 * @param port2 
 * @param port3 
 * 
 * @return int 
 */
int oam_lm_dm_run_with_defaults(int unit, int port1, int port2, int port3) {
	int arad_plus_device, jericho_device;
	bcm_error_t rv;
    bcm_oam_endpoint_info_t acc_endpoint; 
    bcm_oam_endpoint_info_t remote_endpoint;

    rv = is_arad_plus(unit, &arad_plus_device);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }
    if (!arad_plus_device) {
        printf("Test only works for Arad+.\n");
        return 21; 
    }

    rv = is_jericho(unit, &jericho_device);
    if (rv < 0) {
        printf("Error checking whether the device is Jericho.\n");
        print rv;
        return rv;
    }

    single_vlan_tag = 1;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    rv = initialize_vswitch_and_oam(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
  
    mep_id = 4096;
    counter = 1;
    rv =   create_acc_endpoint(0, &acc_endpoint, &remote_endpoint, 1, group_info_short_ma.id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    mep_id = 4100;
    counter = 4; /*This gives us a mep with the same LIF but a diffferent level.*/
    rv =   create_acc_endpoint(0, &acc_endpoint, &remote_endpoint, 0, group_info_short_ma.id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    /* In order to send packets to endpoint 4096, send the packets to port1.*/
    bcm_l2_addr_t addr;
    addr.mac[3] = addr.mac[4] = addr.mac[5] = 0xfe;
    addr.vid = 4096;
    BCM_GPORT_LOCAL_SET(addr.port, port1); 
    rv =  bcm_l2_addr_add(unit, &addr); 
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

/* Create LM/DM/LBR entries for endpoint 4096:*/


    bcm_oam_loss_t loss;
    bcm_oam_delay_t delay;
    bcm_oam_loopback_t lb;
    bcm_oam_loss_t_init(&loss);
    bcm_oam_delay_t_init(&delay);
    bcm_oam_loopback_t_init(&lb);
    lb.id = delay.id = loss.id = 4096;
    loss.period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    delay.period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    delay.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; /*The onlt supported format*/
    loss.peer_da_mac_address[3] = delay.peer_da_mac_address[3] = lb.peer_da_mac_address[3] = 0xef;
    loss.peer_da_mac_address[1] = delay.peer_da_mac_address[1] = lb.peer_da_mac_address[1] = 0xab;
    /* use extended statistics*/
    loss.flags = BCM_OAM_LOSS_STATISTICS_EXTENDED;
    loss.flags = BCM_OAM_LOSS_SINGLE_ENDED; /* LMM based loss management*/

    rv = bcm_oam_loss_add(unit,&loss);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_delay_add(unit,&delay);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_oam_loopback_add(unit,&lb);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* Now a CCM based entry for endpoint 4100.*/
    bcm_oam_loss_t_init(&loss);
    loss.id = 4100;
    loss.flags = 0;
    rv = bcm_oam_loss_add(unit,&loss);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if (jericho_device) {
        /* For Jericho Also send a one time "on demand" DMM.  */
        
        bcm_oam_delay_t_init(&delay);
        delay.id = 4100;
        delay.period = 0 ; /* 0 signifies one shot on demand packet.*/
        delay.peer_da_mac_address[0] = 0x12;
        delay.peer_da_mac_address[1] = 0x34;
        delay.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; /*The onlt supported format*/
        rv = bcm_oam_delay_add(unit, &delay);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }


        bcm_oam_delay_t_init(&delay);
        delay.id = 4100;
        /* Now call delay_delete() to free resources, redirect DMMs back to the CPU.*/
        rv = bcm_oam_delay_delete(unit, &delay);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /* Register AIS callback: transmit AISframes upon loss of continuity interrupt*/
        bcm_oam_event_types_t timeout_event, timein_event;
        BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
        rv = bcm_oam_event_register(0, timeout_event, cb_oam_ais, (void *)0  );
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

    }

    return 0;
}





/**
 * Initialize vswitch and mac entries, oam group. After calling 
 * this function create_acc_endpoint() may be called. 
 * bcm_oam_init() must be called seperately. 
 * 
 * @author sinai (23/12/2013)
 * 
 * @param unit 
 * 
 * @return int - success or fail.
 */
int initialize_vswitch_and_oam(int unit) {
    int is_arad_plus;
    bcm_error_t rv;



    rv = is_arad_plus(unit, &is_arad_plus);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }
    if (!is_arad_plus) {
        printf("Test only works for Arad+.\n");
        return 21; 
    }


    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return 0;

}



/**
 *  Function creates an accelerated endpoint with a matching 
 * remote endpoint. 
 * Function is limited to 6 meps ( 2 MEPs per LIF, each with 
 * different direction and level) 
 * 
 * @author sinai (09/09/2013)
 * 
 * @param unit 
 * @param local_ep 
 * @param remote_ep (optional - may be NULL)
 * @param is_up signifies whether an up-mep or down-mep is 
 *               requested.
 * @param group_id 
 * 
 * @return int 
 */
int create_acc_endpoint(int unit, bcm_oam_endpoint_info_t *local_ep, bcm_oam_endpoint_info_t *remote_ep, int is_up, int group_id) {
    int lm_counter_base_id_1  = 5;
    int lm_counter_base_id_2  = 6;
    bcm_error_t rv;
    bcm_gport_t gportss[3];
    bcm_mac_t src_mac = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    int local_ports[3] = { port_1, port_2, port_3 };

    gportss[0] = gport1;
    gportss[1] = gport2;
    gportss[2] = gport3;
    /*Luckily, the g.c.d. of 3 and 7 is 1, so for 21 iterations, level%7 * gport%3 produces a unique identifier*/

    bcm_oam_endpoint_info_t_init(local_ep);
    /*TX*/
    local_ep->type = bcmOAMEndpointTypeEthernet;
    local_ep->group = group_id;
    local_ep->level = counter % 7 + 1;
    local_ep->name = 123 + counter;
    local_ep->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    local_ep->id = mep_id;
   /* Time stamp format 1588:    otherwise delay_add() cannot be called for this endpoint.*/
    local_ep->timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 

    if (is_up) {
        local_ep->flags |= BCM_OAM_ENDPOINT_UP_FACING;
    } else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(local_ep->tx_gport, local_ports[counter % 3]);
    }

  local_ep->flags |= BCM_OAM_ENDPOINT_WITH_ID; 
   local_ep->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW; 
   local_ep->timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 


    local_ep->vlan = 5;
    local_ep->pkt_pri = local_ep->pkt_pri = 0 +(2<<1);  /*dei(1bit) + (pcp(3bit) << 1) */
    local_ep->outer_tpid = 0x8100;
    local_ep->inner_vlan = 0;
    local_ep->inner_pkt_pri = 0;
    local_ep->inner_tpid = 0;
    local_ep->int_pri = 3 +(1<<2);


    /* The MAC address that the CCM packets are sent with. */
    if (is_up) {
        /* For Up MEPs the LSB of the src MAC address must be equal to the local port*/
        src_mac[5] = local_ports[counter % 3];
    } else {
        /* For Down MEPs the limitation is that MEPs using different system ports (encoded in tx_gport) must have distinct LSBs in the src MAC address.*/
        src_mac[5] = local_ports[counter % 3];
    }
   sal_memcpy(local_ep->src_mac_address, src_mac, 6);
/*     RX*/
    local_ep->gport = gportss[counter%3];
    sal_memcpy(local_ep->dst_mac_address, mac_mep_2, 6);
    local_ep->lm_counter_base_id = lm_counter_base_id_2;

    printf("Creating accelerated EP.\n");
    rv = bcm_oam_endpoint_create(unit, local_ep);
    if (rv != BCM_E_NONE) {
        printf("Failed at the %d-th attempt: (%s) \n", counter, bcm_errmsg(rv));

        return rv;
    }
    printf("created MEP with id %d\n", local_ep->id);


    if (remote_ep) {
        /*
        * Adding Remote MEP
        */
        bcm_oam_endpoint_info_t_init(remote_ep);
        remote_ep->name = 0xff;
        remote_ep->local_id = local_ep->id;
        remote_ep->type = bcmOAMEndpointTypeEthernet;
        remote_ep->ccm_period = 0;
        remote_ep->flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
        remote_ep->loc_clear_threshold = 1;
        remote_ep->id = local_ep->id;

        printf("bcm_oam_endpoint_create RMEP\n");
        rv = bcm_oam_endpoint_create(unit, remote_ep);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        printf("created RMEP with id %d\n", remote_ep->id);

    }

    ++counter; 
    return 0; 
}



