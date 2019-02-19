/*
 *
 * $Id: cint_bfd_over_lag.c,v $
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
 * File: cint_bfd.c
 * Purpose: Example of setting client/server BFD endpoint over LAG. 
 *
 * Usage:
 * 
 
To run server example: 
 
   cd
   cd ../../../../src/examples/dpp
   cint utility/cint_utils_l3.c
   cint cint_ip_route.c
   cint cint_bfd.c 
   cint cint_bfd_over_lag.c 
   cint cint_multi_device_utils.c
   cint 
   int client_unit=0, server_unit=2;
   int port1=13,port2=14,port3=15; 
   print bfd_endpoint_over_lag__init(port1,port2,client_unit,server_unit,15);
   print bfd_server_example(client_unit, server_unit);
 
 
 *
 */


const int MAX_NOF_PORTS_OVER_LAG    = 4;

/* 
 * example varaibles. Varibables marked as IN can be changed before running the test to change configuration. Variables marked as OUT
 * will be populated throughout the tests's run.
 */
struct bfd_endpoint_over_lag_s {
    bcm_port_t          trunk_ports[MAX_NOF_PORTS_OVER_LAG];        /* (IN) Ports to be configured over the lag. */
    int          trunk_ports_units[MAX_NOF_PORTS_OVER_LAG];        /* (IN) unit associated with ports in bcm_port_t. */
    bcm_port_t          eg_port;                                    /* (IN) Port to which the packets will be sent before step 6. */
    int                 nof_ports;                                  /* (IN) Number of ports  on the lag. */
    bcm_port_t          system_ports[MAX_NOF_PORTS_OVER_LAG];       /* (OUT)System ports matching port-unit */
    bcm_gport_t         trunk_vlan_gport;                           /* (OUT)Ethernet port id over which the endpoint will be created. */
    bcm_gport_t         second_vlan_gport;                          /* (OUT)Ethernet that will be cross connected for testing. */
    bcm_trunk_t         trunk_id;                                   /* (OUT)Created trunk id. */
    bcm_gport_t         trunk_gport;                                /* (OUT)Trunk gport over which the vlan port will be created. */
    int                 station_ids[MAX_NOF_PORTS_OVER_LAG - 1];    /* (OUT)Station ids containing the ports-mymac assignment. */
};

bfd_endpoint_over_lag_s            bfd_endpoint_over_lag_1;

/* 
 * bfd_endpoint_over_lag__init 
 *  
 * Inits the test variables to default values. 
 *  
 * Parameteres: 
 *  bcm_port_t  trunk_port_1-3  - (IN) 3 ports to be configured as one LAG (trunk).
 *  bcm_port_t  eg_port         - (IN) A port to be cross connected to the lag for testing. If you wish to skip the test and just set
 *                                      the configuration, set to -1.
 *  
 * Retruns: 
 *  BCM_E_NONE : In any case. 
 *  
 */
int bfd_endpoint_over_lag__init(bcm_port_t trunk_port1, bcm_port_t trunk_port2, int trunk_port1_unit,int trunk_port_2_unit , bcm_port_t eg_port){
    int i;

    /* First clear the struct. */
    sal_memset(&bfd_endpoint_over_lag_1, 0, sizeof(bfd_endpoint_over_lag_1));
    
    bfd_endpoint_over_lag_1.trunk_ports[0]  = trunk_port1;
    bfd_endpoint_over_lag_1.trunk_ports[1]  = trunk_port2;
    bfd_endpoint_over_lag_1.trunk_ports_units[0]  = trunk_port1_unit;
    bfd_endpoint_over_lag_1.trunk_ports_units[1]  = trunk_port_2_unit;
    bfd_endpoint_over_lag_1.eg_port         = eg_port;    
    bfd_endpoint_over_lag_1.nof_ports   = 2;
    
    return BCM_E_NONE;
}



/*
 * bfd_over_lag_trunk_create
 *  
 * Create a trunk, then set it to be round robin. 
 *  
 * Parameters: 
 *  
 * int              unit                                - (IN) Device to be configured. 
 * bcm_trunk_t      bfd_endpoint_over_lag_1.trunk_id    - (OUT)Created lag. 
 *  
 * Returns: 
 * BCM_E_NONE:  If the trunk was created successfully.
 * BCM_E_*:     If something went wrong. 
 */
int bfd_over_lag_trunk_create(int unit){
    int     rv;
    uint32  trunk_flags = 0;


    rv = bcm_trunk_create(unit, trunk_flags, &bfd_endpoint_over_lag_1.trunk_id);
    if (rv != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rv);
      return rv;
    }

    rv = bcm_trunk_psc_set(unit, bfd_endpoint_over_lag_1.trunk_id, BCM_TRUNK_PSC_ROUND_ROBIN);
    if (rv != BCM_E_NONE) {
      printf ("Error, in bcm_trunk_psc_set\n");
      return rv;
    }

    return rv;
}


/*
 * bfd_over_lag_add_members 
 *  
 * Add ports to the preconfigured lag. 
 *  
 * Parameters: 
 *  
 * int          unit                                    - (IN) Device to be configured. 
 * int          bfd_endpoint_over_lag_1.nof_ports       - (IN) Used to iterate over the ports. 
 * bcm_port_t   bfd_endpoint_over_lag_1.trunk_ports[0-x]- (IN) Ports to be configured. 
 * bcm_trunk_t  bfd_endpoint_over_lag_1.trunk_id        - (IN) The precofigured trunk (lag) id.
 *  
 * Returns: 
 * BCM_E_NONE:  If all ports were added successfully. 
 * BCM_E_*:     If something went wrong. 
 */
int
bfd_over_lag_add_members(int unit){
    int i, rv = BCM_E_NONE;
    bcm_trunk_member_t  member;
    bcm_port_t  port;

    bcm_trunk_member_t_init(&member);
    for (i = 0; i < bfd_endpoint_over_lag_1.nof_ports; i++) {
        port = bfd_endpoint_over_lag_1.trunk_ports[i];

        /* bcm_trunk_member_add only accepts modports or system ports. Translate it. */
        rv = port_to_system_port(bfd_endpoint_over_lag_1.trunk_ports_units[i], port, &member.gport);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_to_system_port\n");
            return rv;
        }

        rv = bcm_trunk_member_add(unit, bfd_endpoint_over_lag_1.trunk_id, &member);
        if (rv != BCM_E_NONE) {
          printf ("bcm_trunk_member_add with port %d failed: %d \n", port, rv);
          return rv;
        }
        bfd_endpoint_over_lag_1.system_ports[i] = member.gport;
    }

    return rv;
}



/**
 * 
 * 
 * @author sinai (26/10/2014)
 * 
 * @param server_unit 
 * @param client_unit 
 * @param port1 
 * 
 * @return int 
 */
int bfd_server_example(int server_unit, int client_unit) {
    int rv;
    bcm_bfd_endpoint_info_t client_ep, server_ep;
    int server_oamp_port= 232; /* proper apllication must be used so that this will actually be configured as the server OAMP port*/
    bcm_gport_t remote_gport;

    /** Step 0: Create LAG.  */

    /*  0.1 Create the LAG over the server unit.*/
    rv = bfd_over_lag_trunk_create(server_unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bfd_over_lag_trunk_create\n");
        return rv;
    }

    /*  0.2 Add ports to this lag. */
    rv = bfd_over_lag_add_members(server_unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bfd_over_lag_add_members\n");
        return rv;
    }

    /** Step 1: init BFD and set up IP routing on LAG port
     *  configured above */
    rv = l3_interface_init(client_unit, bfd_endpoint_over_lag_1.system_ports[0], bfd_endpoint_over_lag_1.eg_port, &next_hop_mac, &tunnel_id, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* EEDB must be configured on both devices (a LAG member exists on both devices).*/
    rv = l3_interface_init(server_unit, bfd_endpoint_over_lag_1.system_ports[0], bfd_endpoint_over_lag_1.eg_port, &next_hop_mac, &tunnel_id, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    printf("bcm_bfd_init\n");
    rv = bcm_bfd_init(server_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


        printf("bcm_bfd_init\n");
    rv = bcm_bfd_init(client_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    /** Step 2: set a trap with the destination set to the server
     *  side OAMP.*/
    int trap_code;
    bcm_rx_trap_config_t trap_remote_oamp;
    rv =  bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_remote_oamp);
    trap_remote_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    /* Set the destination*/
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


    /** Step 3: set up the server side endpoint */
    bcm_bfd_endpoint_info_t_init(&server_ep);
    server_ep.type = bcmBFDTunnelTypeUdp;
    server_ep.flags =    BCM_BFD_ENDPOINT_MULTIHOP;
    server_ep.flags |= BCM_BFD_ENDPOINT_IN_HW;
    server_ep.src_ip_addr = 0x30F0701;
    server_ep.dst_ip_addr = 0x40506001;
    server_ep.ip_ttl = 255;
    server_ep.ip_tos = 255;
    server_ep.udp_src_port = 0xC001;
    server_ep.egress_if = next_hop_mac;
    server_ep.int_pri = 1;
    server_ep.bfd_period = 100;
    server_ep.local_min_tx = 2;
    server_ep.local_min_rx = 3;
    server_ep.local_state = 3;
    server_ep.local_flags = 2;
    server_ep.local_detect_mult = 208;
    server_ep.remote_detect_mult = 30;
    server_ep.remote_discr = 0x10002;
    server_ep.flags |= BCM_BFD_ENDPOINT_HW_ACCELERATION_SET; 
    server_ep.local_discr =  0x30004;

    BCM_GPORT_TRUNK_SET(server_ep.tx_gport, bfd_endpoint_over_lag_1.trunk_id); /* port that the traffic will be transmitted on: LAG port */

    BCM_GPORT_TRAP_SET(remote_gport, trap_code, 0, 0);
    server_ep.remote_gport = remote_gport;

    printf("bcm_bfd_endpoint_create on server side\n"); 
    rv = bcm_bfd_endpoint_create(server_unit, &server_ep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created endpoint with id %d\n", server_ep.id); 

    rv = register_events(server_unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /** Step 4: Set up the client side  */
    bcm_bfd_endpoint_info_t_init(&client_ep);
    client_ep.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_MULTIHOP;
    client_ep.id = server_ep.id;
    client_ep.remote_gport = remote_gport;
    client_ep.type = bcmBFDTunnelTypeUdp;
    client_ep.local_discr =  0x30004;
    client_ep.src_ip_addr = 0x30F0701;

    rv = bcm_bfd_endpoint_create(client_unit, &client_ep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }


}

