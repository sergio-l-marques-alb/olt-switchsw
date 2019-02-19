/*~~~~~~~~~~~~~~~~~~Multicast Options: Device-Level Scheduled~~~~~~~~~~~~~~~~~~~~*/
/*
 * $Id: cint_multicast_applications.c,v 1.6 Broadcom SDK $
 *
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
 * File: cint_multicast_application_device_lvl_example.c
 * Purpose: Example of device level multicast configuration.
 * 
 * The configuration needed is defined in the cint_multicast_applications.c file.
 * The configuration includes the following: 
 *  o   Ingress FAPs: Open MC group defined by a set of destination FAPs. 
 *  o   Egress FAPs: 
 *      For each MC group holding a member port on this FAP, open a group using one of two methods:
 *          o   LAN Membership - the MC group is defined by the set of destination (local) ports
 *          o   General  - the MC group is defined by the set of {destination local ports, CUD}
 */


cint_reset();
/* Verify script cint_multicast_applications is in the same folder */
#include <cint_multicast_applications.c>


/* 2 Examples of device level partially scheduled multicast 
 * In first example destination based on 2 local ports 
 * and cud is not important (different ports) thus 
 * we use egress multicast type vlan membership.
 * In second example destionation basded on 1 local port
 * but with two different cuds. Thus we use egress multicast
 * type linked list.
 */
  
/* 
 * The script Assumes Queues and Ports related are already defined. 
 * In the example we will assume diag_init application was executed. 
 * User can specify different parameters but need to make sure 
 * Queues and related physical ports are created. 
 * Also pay attention multicast group, use L2 reserved group (for vlan membership).
 */

/* 
 * Parameters defined 
 * Ports: Destination Port to transmit multicast replication packet 
 * CUD packet for destination ports
 * Specify mode: is tm: assumes packets being transmit by ITMH. Run ITMH with destinatino: multicast_id. 
 *               If not, creating mac address to send packets to related destinations.
 * Multicast ID: Multicast ID for example1
 */
int is_tm = 0;
uint8 incoming_mac_mc[6] = {0,0,0,0,0,0x3}; /* 00:00:00:00:00:03 */
int incoming_vlan_mc = 2;

bcm_port_t ports_example[2] = {1,4};
int cud = 0;
int multicast_id_1 = 12;

/* Example of system ports */
int run_device_lvl_multicast_example1(int unit) 
{
    bcm_error_t rv = BCM_E_NONE;        
    bcm_gport_t local_ports[2];
    int cuds[2];
  
    if (!is_tm) {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, incoming_mac_mc, incoming_vlan_mc);   
        l2_addr.flags = 0x00000020 | BCM_L2_MCAST;
        l2_addr.l2mc_group = multicast_id_1;

        rv = bcm_l2_addr_add(0,&l2_addr);
        if (rv != BCM_E_NONE) {
            printf("Error, in setup_mac_forwarding\n");
        }
    }
            
    /* Convert local port to gport */
    BCM_GPORT_LOCAL_SET(local_ports[0],ports_example[0]);
    BCM_GPORT_LOCAL_SET(local_ports[1],ports_example[1]);
    
    cuds[0] = cud;
    cuds[1] = cud;
    
    rv = device_level_scheduled_multicast(unit,multicast_id_1,local_ports,cuds,2);
    if (rv != BCM_E_NONE) {
        return rv;
    }       

    return rv;
}

/* Example of VOQs */
uint8 incoming_mac_mc_2[6] = {0,0,0,0,0,0x4}; /* 00:00:00:00:00:04 */
int incoming_vlan_mc_2 = 3;

bcm_port_t port = 1;
int cud_2[2] = {1,2};
/* Multicast id must be higher than 4K for linked list type */
/* duplicate the packtes for both cud 1 and 2 */
int multicast_id_2 = 5005;

int run_device_lvl_multicast_example2(int unit)
{
    bcm_error_t rv;
    bcm_gport_t local_ports[2];
    int cuds[2];
     
    if (!is_tm) {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, incoming_mac_mc_2, incoming_vlan_mc_2);   
        l2_addr.flags = 0x00000020 | BCM_L2_MCAST;
        l2_addr.l2mc_group = multicast_id_2;

        rv = bcm_l2_addr_add(0,&l2_addr);
        if (rv != BCM_E_NONE) {
            printf("Error, in setup_mac_forwarding\n");
        }
    }
            
    /* Convert local port to gport */
    /* destionation basded on 1 local port
     * but with two different cuds. Thus we use egress multicast
     * type linked list. 
     */ 
    BCM_GPORT_LOCAL_SET(local_ports[0],ports_example[0]);
    BCM_GPORT_LOCAL_SET(local_ports[1],ports_example[1]);
    
    cuds[0] = cud_2[0];
    cuds[1] = cud_2[1];
    
    rv = device_level_scheduled_multicast(unit,multicast_id_2,local_ports,cuds,2);
    if (rv != BCM_E_NONE) {return rv;}       

    return rv;
}

/*util function to set relevent param for arad and PetraB*/
int set_params_device_lvl_multicast (int is_arad) {
    return set_params_device_lvl_multicast_with_ports (1, (is_arad ? 14 : 4), 1);
}

/*util function to set relevent param for arad and PetraB*/
int set_params_device_lvl_multicast_with_ports (int port0, int port1, int is_arad) {
    ports_example[0] = port0;
    ports_example[1] = port1;

    cud  = is_arad ? BCM_IF_INVALID : 1;
    cud_2[0] = is_arad ? BCM_IF_INVALID : 1;
    cud_2[1] = is_arad ? BCM_IF_INVALID : 2;
    return BCM_E_NONE;
}

