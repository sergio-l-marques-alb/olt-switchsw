/*
 * $Id: v 1.236 2014/02/05 08:26:50  Exp $
 *
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
 * Soc_petra-B VLAN Implementation
 */


/*
 * $Id: cint_multicast_application_port_lvl_example_multi_device.c,v 1.3 Broadcom SDK $
 *
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
 * File: cint_multicast_application_port_lvl_example_multi_device.c
 * Purpose: Example of port level multicast configuration for a two-device system.
 *
 * The configuration needed is defined in the cint_multicast_applications.c file.
 * The configuration includes the following:
 *   o  Ingress FAPs: Open MC group defined by a set of {Destination System Ports, Copy-Unique-Data (CUD)}.
 *   o  Egress FAPs: None.
 */


cint_reset();

int create_multicast_group(int unit,int is_ingress,int is_egress,bcm_multicast_t * multicast_group) {
    bcm_error_t rv;
    uint32 flags;
    flags = BCM_MULTICAST_WITH_ID;

    if (is_ingress) {
        flags |= BCM_MULTICAST_INGRESS_GROUP;
    }
    if (is_egress) {
        flags |= BCM_MULTICAST_EGRESS_GROUP;
    }

    
    rv = bcm_multicast_create(unit,flags,multicast_group);      
    return rv;
}


/* 
 * example of port level scheduled multicast on two devices 
 */

/* 
 * The script Assumes Queues and Ports related are already defined (also ERP). 
 * In the example we will assume diag_init application was executed. 
 * User can specify different parameters but need to make sure 
 * Queues and related physical ports are created. 
 */

/* 
 * in order to run the script you need to have a two device system where unit 0 has modid 0
 * and unit 1 has modid 1, run_port_lvl_multicast_example1() should be called from unit 0 (the master)
 * traffic should be sent to port 1 of unit 0 and with mac addr 00:00:00:00:00:01
 * and should go out on device 0 port 2, and on device 1 port 1.
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
uint8 incoming_mac_mc[6] = {0,0,0,0,0,0x1}; /* 00:00:00:00:00:01 */
int incoming_vlan_mc = 1;

bcm_port_t ports_example[2] = {2,1};
int cud = 1;
int multicast_id_1 = 10;

/* Example of system ports */
int run_port_lvl_multicast_example1() 
{
    bcm_multicast_t multicast_group = multicast_id_1;
    bcm_error_t rv = BCM_E_NONE;    
    bcm_gport_t system_ports[2];
    bcm_gport_t local_gport;
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
            
    /* Convert local port to system port */  
    BCM_GPORT_LOCAL_SET(local_gport,ports_example[0]);
    rv = bcm_stk_gport_sysport_get(/*unit*/ 0,local_gport,&system_ports[0]);
    if (rv != BCM_E_NONE) {
        printf("bcm_petra_stk_gport_sysport_get failed: (%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    BCM_GPORT_LOCAL_SET(local_gport,ports_example[1]);
    rv = bcm_stk_gport_sysport_get(/*unit*/ 1,local_gport,&system_ports[1]);
    if (rv != BCM_E_NONE) {
        printf("bcm_petra_stk_gport_sysport_get failed: (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    cuds[0] = cud;
    cuds[1] = cud;    
    
    
    rv = create_multicast_group(0,/*is_ingress*/ 1,/*is_egress*/ 0,&multicast_group);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }   

    rv = bcm_multicast_ingress_add(/*unit*/ 0,multicast_group,system_ports[0],cuds[0]);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    rv = bcm_multicast_ingress_add(/*unit*/ 0,multicast_group,system_ports[1],cuds[1]);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
 
    return rv;
}
