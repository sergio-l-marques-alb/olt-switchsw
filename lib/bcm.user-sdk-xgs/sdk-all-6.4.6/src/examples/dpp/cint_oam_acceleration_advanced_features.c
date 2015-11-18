/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: oam.c,v 1.140 Broadcom SDK $
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
   cint utility/cint_utils_oam.c
   cint cint_oam_acceleration_advanced_features.c
   cint
   int unit=0;
   int port1=13,port2=14,port3=15;
   print bcm_oam_init(unit);
   print oam_lm_dm_run_with_defaults(unit,port1,port2,port3);
 
 
 For the OAMP server example:
   cd
   cd ../../../../src/examples/dpp
   cint cint_port_tpid.c
   cint cint_l2_mact.c
   cint cint_vswitch_metro_mp.c
   cint cint_multi_device_utils.c
   cint cint_oam_acceleration_advanced_features.c
   cint
   int client_unit=0, server_unit=2;
   int port1=13,port2=14,port3=15, recycle_port=41; 
   print bcm_oam_init(client_unit);
   print bcm_oam_init(server_unit);
   print oamp_server_example( server_unit,  client_unit,  port1,  port2,  port3, recycle_port) ;
 
 
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
  bcm_mac_t src_mac = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

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

  int OpCode_LMM = 43;
  int OpCode_DMM = 47;
  int OpCode_LBM = 3;
  int OpCode_LTM = 5;

  /* Jericho only */
  int oam_lm_dm_reports_enabled = 0; /* enable LM/DM reports */

  /* Jericho only - sticky statistics buffers */
  uint32 delay_report_count = 0;
  uint32 last_delta_FCB = -1;
  uint32 last_delta_FCf = -1;

  int counter =0;
  int mep_id = 4096;

  int arad_plus_device, jericho_device; /* Should be initialized in "initialize_vswitch_and_oam"*/


/**
 * Triggered upon CCM timeout event: Transmit AIS frames to 
 * indicate LOC. 
 * 
 * @author sinai (17/09/2014)
 * 
 * @param unit 
 * @param flags 
 * @param event_type 
 * @param group 
 * @param endpoint 
 * @param user_data 
 * 
 * @return int 
 */
int cb_oam_ais(int unit, uint32 flags, bcm_oam_event_type_t event_type, bcm_oam_group_t group,
               bcm_oam_endpoint_t endpoint, void *user_data) {
    bcm_oam_ais_t ais;
    int rv; 

    print event_type;
    printf("Adding AIS to MEP: 0x%08x\n", endpoint);

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
 * Triggered upon LM/DM report rx.
 */
int cb_stat(int unit,
            bcm_oam_event_type_t event_type,
            bcm_oam_group_t group,
            bcm_oam_endpoint_t endpoint,
            bcm_oam_performance_event_data_t *event_data,
            void *user_data) {

    print unit;
    print event_type;
    print group;
    print endpoint;
    print *event_data;

    /* Update globals */
    switch (event_type) {
    case bcmOAMEventEndpointDmStatistics:
        delay_report_count++;
        break;
    case bcmOAMEventEndpointLmStatistics:
        last_delta_FCB = event_data->delta_FCB;
        last_delta_FCf = event_data->delta_FCf;
        break;
    }
}


/**
 * The OAMP may reply to LMMs, DMMs, LBMs, LTMs and EXMs with 
 * their respective reply packets. 
 * This is an example of how to set the reply mechanism. 
 *  
 * The reply packet's DA will be taken from the incoming 
 * packet's SA. 
 * The reply packets SA will be as configured in the 
 * endpoint_create() API. 
 *  
 * Note that LMRs, DMRs will be transmitted with the 
 * counters/ToD stamped on the packet. For other types of reply 
 * packets, only the OpCode will change.
 * 
 * @author sinai (12/03/2015)
 * 
 * @param unit 
 * @param mep_id 
 * @param opcode 
 * @param endpoint_type_is_ethernet - set to 1 for Ethernet 
 *                                  endpoints, 0 for MPLS/PWE.
 *  
 * @return int 
 */
int oam_configure_oamp_reply(int unit, int mep_id, int opcode,int endpoint_type_is_ethernet) {
    bcm_gport_t oamp_gports[2];
    int count_oamp_port;
    int rv;
    bcm_oam_endpoint_action_t action;

    if (opcode != OpCode_LTM && opcode != OpCode_DMM && opcode != OpCode_LMM && opcode != OpCode_LBM ) {
        printf("Given opcode must be valid reply message. Given opcode: %d\n",opcode);
        return 44;
    }

    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP,2 /*One per core */, &oamp_gports, &action.destination);
    if (rv != BCM_E_NONE) {
        printf("Unit %d: Error, in bcm_port_internal_get\n", unit);
        return rv;
    }

    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action); 

    /* Find the local OAMP port*/
    rv =  port_to_system_port(unit,oamp_gports[0], &action.destination);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    BCM_OAM_OPCODE_SET(action, opcode); 
    if (endpoint_type_is_ethernet) {
        /* All actions of type UC are only supported for Ethernet OAM.*/
        BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd); 
    } else {
        /* The OAMP supports MC as well. For the sake of this cint only update unicast packets for Ethernet OAM. */
        BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd); 
    }

    rv =  bcm_oam_endpoint_action_set(unit, mep_id, &action);

    if (rv != BCM_E_NONE) {
       printf("bcm_oam_endpoint_action_set failed (%s) \n",bcm_errmsg(rv));
       return rv;
   }

    return  0;
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
	bcm_error_t rv;
    bcm_oam_endpoint_info_t acc_endpoint; 
    bcm_oam_endpoint_info_t remote_endpoint;


    single_vlan_tag = 1;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    rv = initialize_vswitch_and_oam(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
  
    if (device_type<device_type_arad_plus) {
        printf("Cint only works for Arad+.\n");
        return 21; 
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
    loss.flags = BCM_OAM_LOSS_SINGLE_ENDED; /* LMM based loss management*/

    if (oam_lm_dm_reports_enabled) {
        bcm_oam_event_types_t loss_event, delay_event;

        printf("Setting report mode for Loss/Delay measurements\nand increasing period to reduce CPU load\n");
        loss.flags |= BCM_OAM_LOSS_REPORT_MODE; /* LMR report mode */
        loss.period = BCM_OAM_ENDPOINT_CCM_PERIOD_10M;
        delay.flags |= BCM_OAM_DELAY_REPORT_MODE; /* DMR report mode */
        delay.period = BCM_OAM_ENDPOINT_CCM_PERIOD_10M;

        printf("Registering callbacks for statistics events\n");

        BCM_OAM_EVENT_TYPE_SET(loss_event, bcmOAMEventEndpointLmStatistics);
        rv = bcm_oam_performance_event_register(unit, loss_event, cb_stat, (void*)1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        BCM_OAM_EVENT_TYPE_SET(delay_event, bcmOAMEventEndpointDmStatistics);
        rv = bcm_oam_performance_event_register(unit, delay_event, cb_stat, (void*)2);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

    }
    else {
        loss.flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
    }

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

    if (device_type == device_type_arad_plus) { 
        rv = bcm_oam_loopback_add(unit, &lb);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Now a CCM based entry for endpoint 4100.*/
    bcm_oam_loss_t_init(&loss);
    loss.id = 4100;
    loss.flags = oam_lm_dm_reports_enabled ? BCM_OAM_LOSS_REPORT_MODE : 0;
    rv = bcm_oam_loss_add(unit,&loss);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if (device_type>=device_type_jericho) {
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

        if (!oam_lm_dm_reports_enabled) {
            printf("Register AIS callback: transmit AISframes upon loss of continuity interrupt\n");
            bcm_oam_event_types_t timeout_event, timein_event;
            BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
            rv = bcm_oam_event_register(unit, timeout_event, cb_oam_ais, (void *)0  );
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        }

    }

    return 0;
}


/**
 * An example of setting up the OAMP as a server. 
 * Create an up MEP and a down MEP, each first on the server 
 * side (transmitting and recieving CCMs), then on the client 
 * side (trapping OAM PDUs) 
 * 
 * @author sinai (01/09/2014)
 * 
 * @param server_unit 
 * @param client_unit 
 * @param port1 - Down MEP will reside on a LIF created at this
 *              port (presumably on the client device)
 * @param port2 - Up MEP will reside on a LIF created at this 
 *              port
 * @param port3 - CCMs from the up MEP will be injected from 
 *              this port
 * @param recycle_port - Client device recycle port. Note that 
 *                     this port must be defined as
 *                     INJECTED_2_PP via soc property. 
 * 
 * @return int 
 */
int oamp_server_example(int server_unit, int client_unit, int port1, int port2, int port3,int recycle_port) {
    int rv;
    bcm_oam_endpoint_info_t ep_server_down, ep_server_up, ep_client_down, ep_client_up, rmep;
    bcm_oam_group_info_t group_info_short_ma_client,group_info_short_ma_server ;
    int server_oamp_port= 232; /* proper apllication must be used so that this will actually be configured as the server OAMP port*/
    int counter_base_id_up_mep, counter_base_id_down_mep ;

    int down_mdl = 4;
    int up_mdl = 5;

    single_vlan_tag = 1;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    rv = get_device_type(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }


	rv = set_counter_source_and_engines(unit, &counter_base_id_up_mep,port1);
	BCM_IF_ERROR_RETURN(rv); 
	rv = set_counter_source_and_engines(unit, &counter_base_id_down_mep,port1);
	BCM_IF_ERROR_RETURN(rv); 

    /* A vswitch is defined on the client unit.*/
    rv = create_vswitch_and_mac_entries(client_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Groups must be created on server unit and client unit.*/
    bcm_oam_group_info_t_init(&group_info_short_ma_client);
    bcm_oam_group_info_t_init(&group_info_short_ma_server);
    sal_memcpy(group_info_short_ma_client.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(group_info_short_ma_server.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);

    /* The server side group is used to determine the MAID on CCMs, both those transmitted and the expected MAID on recieved CCMs.
       This is conveyed through the name field.*/
    rv = bcm_oam_group_create(server_unit, &group_info_short_ma_server);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* The client side is only used for logically clustering endpoint together. Recieved/Transmitted packets are not affected by this*/
    rv = bcm_oam_group_create(client_unit, &group_info_short_ma_client);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }


    /* Next, set up a trap with the destination being the remote OAMP. This is used by the down MEPs.*/
    int trap_code;
    bcm_rx_trap_config_t trap_remote_oamp;
    rv =  bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_remote_oamp);
    trap_remote_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    rv =  port_to_system_port(server_unit,server_oamp_port, &trap_remote_oamp.dest_port);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_rx_trap_set(client_unit, trap_code, trap_remote_oamp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code=%d \n", trap_code);

    /* Now set the Server endpoints*/

    /* Start with the down-MEP*/
    bcm_oam_endpoint_info_t_init(ep_server_down);

    BCM_GPORT_TRAP_SET(ep_server_down.remote_gport, trap_code, 7, 0); /*Taken from default values*/
    ep_server_down.type = bcmOAMEndpointTypeEthernet;
    ep_server_down.group = group_info_short_ma_server.id;
    ep_server_down.level = down_mdl;
    ep_server_down.name = 123;
    ep_server_down.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    ep_server_down.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 

    BCM_GPORT_SYSTEM_PORT_ID_SET(ep_server_down.tx_gport, port1); /* CCMs will be transmitted on this port.*/

    ep_server_down.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW; 
    ep_server_down.vlan = 5;
    ep_server_down.pkt_pri = 0 + (2 << 1);  /*dei(1bit) + (pcp(3bit) << 1) */
    ep_server_down.outer_tpid = 0x8100;
    ep_server_down.int_pri = 3 + (1 << 2); 
    ep_server_down.gport = BCM_GPORT_INVALID;
    

    sal_memcpy(ep_server_down.src_mac_address, src_mac, 6);

    printf("bcm_oam_endpoint_create down MEP, server, on unit %d\n", server_unit);
    rv = bcm_oam_endpoint_create(server_unit, &ep_server_down);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created down MEP with id %d\n", ep_server_down.id); 

    /* Set the RMEP entry associated with the above endpoint.*/
    bcm_oam_endpoint_info_t_init(rmep);
    rmep.name = 0x34;
    rmep.local_id = ep_server_down.id;
    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep.loc_clear_threshold = 1;
    rmep.id = ep_server_down.id;

    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(server_unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created RMEP with id %d\n", rmep.id); 

 /***************/

    /* Now the up MEP on the server device */
    bcm_oam_endpoint_info_t_init(ep_server_up);



    rv =  port_to_system_port(client_unit, recycle_port, &ep_server_up.remote_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    ep_server_up.type = bcmOAMEndpointTypeEthernet;
    ep_server_up.group = group_info_short_ma_server.id;
    ep_server_up.level = up_mdl;
    ep_server_up.name = 456;
    ep_server_up.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    ep_server_up.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 
    ep_server_up.flags = BCM_OAM_ENDPOINT_UP_FACING;


/* Source port from which CCMs will be injected*/
    rv =  port_to_system_port(client_unit, port3, &ep_server_up.tx_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    ep_server_up.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW; 
    ep_server_up.vlan = 10;
    ep_server_up.pkt_pri = 0 + (2 << 1);  /*dei(1bit) + (pcp(3bit) << 1) */
    ep_server_up.outer_tpid = 0x8100;
    ep_server_up.int_pri = 3 + (1 << 2); 
    ep_server_up.gport = BCM_GPORT_INVALID;

    /* For up-MEPs the counter base ID must be set for both the server and client MEPs*/
    ep_server_up.lm_counter_base_id = counter_base_id_up_mep; 

    sal_memcpy(ep_server_up.src_mac_address, src_mac, 6);


    printf("bcm_oam_endpoint_create up MEP, server, on unit %d\n", server_unit);
   rv = bcm_oam_endpoint_create(server_unit, &ep_server_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created up MEP with id %d\n", ep_server_up.id); 

    /* Set the RMEP for this endpoint as well*/
    bcm_oam_endpoint_info_t_init(rmep);
    rmep.name = 0x45;
    rmep.local_id = ep_server_up.id;
    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep.loc_clear_threshold = 1;
    rmep.id = ep_server_up.id;

    printf("bcm_oam_endpoint_create RMEP\n");
   rv = bcm_oam_endpoint_create(server_unit, &rmep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created RMEP with id %d\n", rmep.id); 


    /*********************/
    /*********************/

    /* Finally, set up the endpoints on the client device*/
    /* Note that it is the users responsibility to verify that the fields of the server and client device match.*/

    /*First the down MEP*/
    bcm_oam_endpoint_info_t_init(ep_client_down);

    /* The client endpoint must be set up with the ID of the server endpoint.*/
    ep_client_down.id = ep_server_down.id; 
    ep_client_down.flags = BCM_OAM_ENDPOINT_WITH_ID;
    ep_client_down.type = bcmOAMEndpointTypeEthernet;
    ep_client_down.group = group_info_short_ma_client.id;
    ep_client_down.level = down_mdl;
    ep_client_down.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 
    ep_client_down.lm_counter_base_id = counter_base_id_down_mep;
    ep_client_down.remote_gport = trap_code;
    sal_memcpy(ep_client_down.dst_mac_address, mac_mep_1, 6);

    ep_client_down.gport = gport1; /* represents the LIF on which the MEP resides*/
    BCM_GPORT_TRAP_SET(ep_client_down.remote_gport, trap_code, 7, 0); /*Taken from default values*/
    printf("bcm_oam_endpoint_create, down MEP on device %d\n",client_unit);
    rv = bcm_oam_endpoint_create(client_unit, &ep_client_down);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created client endpoint with id 0x%x\n", ep_client_down.id); 


    /* Now the up MEP*/
    bcm_oam_endpoint_info_t_init(ep_client_up);

    /* The client endpoint must be set up with the ID of the server endpoint.*/
    ep_client_up.id = ep_server_up.id; 
    ep_client_up.flags = BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
    ep_client_up.type = bcmOAMEndpointTypeEthernet;
    ep_client_up.group = group_info_short_ma_client.id;
    ep_client_up.level = up_mdl;
    ep_client_up.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 
    ep_client_up.lm_counter_base_id = counter_base_id_up_mep;
    sal_memcpy(ep_client_up.dst_mac_address, mac_mep_2, 6);

    ep_client_up.gport = gport3; /* represents the LIF on which the MEP resides*/

    rv =  port_to_system_port(server_unit, server_oamp_port, &ep_client_up.remote_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_oam_endpoint_create, up MEP on device %d\n",client_unit);
   rv = bcm_oam_endpoint_create(client_unit, &ep_client_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created client endpoint with id 0x%x\n", ep_client_up.id); 



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
    bcm_error_t rv;


    rv = get_device_type(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
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

int lm_counter_base_ids[3];
int first_call=1;

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
    bcm_error_t rv;
    bcm_gport_t gportss[3];
    int local_ports[3] = { port_1, port_2, port_3 };
    static int src_mac_lsb =0;
    ++src_mac_lsb;

    gportss[0] = gport1;
    gportss[1] = gport2;
    gportss[2] = gport3;

	if (first_call) {
          /*Endpoints will be created on 3 different LIFs. Allocate 3 different counters */
		rv = set_counter_source_and_engines(unit, &lm_counter_base_ids[0],port_1);
		BCM_IF_ERROR_RETURN(rv);
		rv = set_counter_source_and_engines(unit, &lm_counter_base_ids[1],port_1);
		BCM_IF_ERROR_RETURN(rv); 
        rv = set_counter_source_and_engines(unit, &lm_counter_base_ids[2],port_1);
        BCM_IF_ERROR_RETURN(rv);
        first_call=0; 
    }

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
    /* src_mac_address constraints: Only apply to Arad+*/
    if (device_type<device_type_jericho) {
        if (is_up) {
            /* For Up MEPs the LSB of the src MAC address must be equal to the local port*/
            src_mac[5] =src_mac_lsb;
        } else {
            /* For Down MEPs the limitation is that MEPs using different system ports (encoded in tx_gport) must have distinct LSBs in the src MAC address.*/
            src_mac[5] = local_ports[counter % 3];
        }
    }
   sal_memcpy(local_ep->src_mac_address, src_mac, 6);
/*     RX*/
    local_ep->gport = gportss[counter%3];
    sal_memcpy(local_ep->dst_mac_address, mac_mep_2, 6);
    local_ep->lm_counter_base_id = lm_counter_base_ids[counter%3]; /* Endpoints on the same LIF will use the same counter base id.*/

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

/**
 * Example of using PCP based loss measurement endpoint. At the 
 * egress the mapping must be configured manually, by 
 * outlif profile and the int_pri. At the ingress the PCP can 
 * either be taken from the VLAN PCP or the int_pri as well. 
 * 
 * @author sinai (12/10/2014)
 * 
 * @param unit 
 * @param port1 
 * @param port2 
 * @param port3 
 * 
 * @return int 
 */
int oam_pcp_example(int unit, int port1, int port2, int port3) {
    int rv;
    bcm_oam_endpoint_info_t ep;
    int oam_pcp_qos_map_id;
    int egress_tc;
    bcm_qos_map_t oam_pcp_qos_map;
    int up_mep_lm_counter_base , down_mep_lm_counter_base ; 


    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    bcm_oam_endpoint_info_t_init(&ep);

/* When using PCP the 3 LSBs of the counter must be 0. The following cints return an appropriate counter base.*/
    rv = set_counter_source_and_engines(unit, &up_mep_lm_counter_base,port1);
    BCM_IF_ERROR_RETURN(rv); 
    rv = set_counter_source_and_engines(unit, &down_mep_lm_counter_base,port1);
    BCM_IF_ERROR_RETURN(rv); 


    rv = initialize_vswitch_and_oam(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    bcm_qos_map_t_init(&oam_pcp_qos_map);

    oam_pcp_qos_map_id = 1; /* Use OAM=outlif-profile 1. (0-4 are available)*/

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OAM_PCP, &oam_pcp_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    for (egress_tc = 0; egress_tc < 8; egress_tc++) {

        oam_pcp_qos_map.int_pri = egress_tc;
        oam_pcp_qos_map.pkt_pri = egress_tc ; /* OAM-PCP taken from this value. Use a 1:1 mapping in this case from the TC to the OAM-PCP.*/

        rv = bcm_qos_map_add(unit, 0, &oam_pcp_qos_map, oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Set the OAM-outlif-profile on the up MEP's gport.*/
    rv = bcm_qos_port_map_set(unit, gport2, -1 /* ingress map */, oam_pcp_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Create up MEP.*/
    bcm_oam_endpoint_info_t_init(&ep);
    ep.gport = gport2;
    ep.type = bcmOAMEndpointTypeEthernet;
    ep.level = 3;
    ep.group = group_info_short_ma.id;
    sal_memcpy(ep.dst_mac_address, mac_mep_2, 6);
    ep.lm_counter_base_id = up_mep_lm_counter_base;

    ep.lm_flags = BCM_OAM_LM_PCP;
    ep.flags = BCM_OAM_ENDPOINT_UP_FACING;

    rv = bcm_oam_endpoint_create(unit, &ep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    if (!soc_property_get(unit, "oam_pcp_value_extract_from_packet", 1)) {
        /* In this case the PCP is taken from the COS-profile*/
        int packet_pcp;
        int packet_dei;
        bcm_qos_map_t_init(&oam_pcp_qos_map);


        oam_pcp_qos_map_id = 1; /* use QoS map profile 1 in this LIF*/
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        for (packet_dei = 0; packet_dei < 2  /*1 bit value */; ++packet_dei) {
            for (packet_pcp = 0; packet_pcp < 0x8; ++packet_pcp) {
                /* Iterate through all possible PCP, DEI values on the vlan*/
                oam_pcp_qos_map.pkt_pri = packet_pcp;
                oam_pcp_qos_map.int_pri = packet_pcp  | packet_dei; /* OAM-PCP taken from this value. Any mapping may be set here.*/
                oam_pcp_qos_map.pkt_cfi = packet_dei;

                rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2, &oam_pcp_qos_map, oam_pcp_qos_map_id);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
        /* Associate this Qos map profile with the  down MEP's gport.*/
        rv = bcm_qos_port_map_set(unit, gport1, oam_pcp_qos_map_id, -1 /* egress map*/);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Create down Mep*/
    bcm_oam_endpoint_info_t_init(&ep);
    ep.type = bcmOAMEndpointTypeEthernet;
    ep.level = 2;
    ep.gport = gport1;
    sal_memcpy(ep.dst_mac_address, mac_mep_1, 6);
    ep.lm_counter_base_id = down_mep_lm_counter_base;
    ep.lm_flags = BCM_OAM_LM_PCP;


    rv = bcm_oam_endpoint_create(unit, &ep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

}
