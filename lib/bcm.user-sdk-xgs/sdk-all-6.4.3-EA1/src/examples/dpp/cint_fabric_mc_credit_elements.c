/*~~~~~~~~~~~~~~~~~~Multicast Options: Fabric Multicast Credit Elements~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_fabric_mc_credit_elements.c,v 1.2 Broadcom SDK $
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
 * File:        cint_fabric_mc_credit_elements.c
 * Purpose:     Example of configuration for Ingress Multicast schedulers' credits 
 *              and TC mapping in the Fabric MC hierarchy. 
 *  
 * In the Fabric MC, configuration results in unscheduled MC scheme.
 * This configuration provides no end-to-end scheduling by the Credit Scheduler.
 * The Ingress Scheduling is based on the ingress queue.
 * The ingress queues are typically defined by the MC traffic class (0 - 3).
 * 
 * Settings include:
 *   1.        Set shapers: root 5 Gbps, guaranteed 3 Gbps, BestEffortAgg, 2 Gbps.
 *   2.        Set max burst of the BestEffortAgg and guaranteed.
 *   3.        Configure best-effort priorities SP or WFQ: SP means BE2 > BE1 > BE0.
 *          WFQ means custom weights can be set to the different BE queues. 
 *   4.        Map the incoming TC to a specific queue in the Fabric MC.
 *          High Priority traffic (TC 6-7) to the guaranteed FMQ, and send the
 *          Low Priority traffic (TC 0-5) to the best effort FMQs.
 *  
 * You can specify different parameters for this example,
 * including shaper's bandwidth, BE weights, TC mapping, and so on.
 * You can also use the individual functions to configure each of the different elements. 
 *  
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                                           |
 *  |                                                                                           |
 *  |         +-----------+           Gauranteed - 3G         +-------+                         |
 *  | TC 6-7 -|Class1  || |----------------SPR----------------|H      |                         |
 *  |         +-----------+                                   |       | Root - 5G +----------+  |
 *  |         +-----------+                                   |  SP   |<---SPR----|credit gen|  |
 *  | TC 0-1 -|Class2  || |       +---------+ Best Effor - 2G |       |           +----------+  |
 *  |         +-----------+-------|1/6      |-------SPR-------|L      |                         |
 *  |         +-----------+       |         |                 +-------+                         |
 *  | TC 2-3 -|Class3  || |-------|1/3  WFQ |                                                   |                              
 *  |         +-----------+       |         |                                                   |
 *  |         +-----------+-------|1/2      |                                                   |
 *  | TC 4-5 -|Class4  || |       +---------+                          +----------------+       |
 *  |         +-----------+                                            |      KEY       |       |                
 *  |                                                                  +----------------+       |
 *  |                                                                  |SPR- Rate Shaper|       |
 *  |                                                                  |                |       |
 *  |                                                                  +----------------+       |
 *  |                                                                                           |
 *  |                     +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                          |
 *  |                     |           Figure 1: Multicast            |                          |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 */


enum credit_element{
    root,
    guaranteed,
    bestEffortAggregate,
    bestEffort0,
    bestEffort1,
    bestEffort2
};

/* Set Best Effort Bandwidth (max KB per sec)
 * Element should be: root, guaranteed, bestEffortAggregate
 */
int set_bandwidth(int unit, int local_port_id, credit_element element, int max_kbits_sec){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    int min_kbits_rate = 0;
    int flags = 0;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport, local_port_id); 
    
    /* Set the gport_type according to the designated element */
    if(element == root) 
        gport_type = bcmCosqGportTypeGlobalFmqRoot; 
    if(element == bestEffortAggregate) 
        gport_type = bcmCosqGportTypeGlobalFmqBestEffortAggregate; 
    if(element == guaranteed) 
        gport_type = bcmCosqGportTypeGlobalFmqGuaranteed; 

    /* Get the handle to the gport we want to config */
    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    out_gport = gport_info.out_gport;

    /* Set the Bandwidth */
    rv = bcm_cosq_gport_bandwidth_set(unit, out_gport, 0, min_kbits_rate, max_kbits_sec, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bandwidth set, out_gport $out_gport max_kbits_sec $max_kbits_sec \n");
        return rv;
    }
    
    return rv;
}

/* Set OFP Burst (max size)
 * Element should be: guaranteed, bestEffortAggregate
 */
int set_burst(int unit, int local_port_id,credit_element element,  int max_burst_kbits){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport, local_port_id); 

    /* Set the gport_type according to the designated element */
    if(element == bestEffortAggregate) 
        gport_type = bcmCosqGportTypeGlobalFmqBestEffortAggregate; 
    if(element == guaranteed) 
        gport_type = bcmCosqGportTypeGlobalFmqGuaranteed; 
    if(element == root) 
        gport_type = bcmCosqGportTypeGlobalFmqRoot; 

    /* Get the handle to the gport we want to config */
    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    out_gport = gport_info.out_gport;

    /* Set the Burst */
    rv = bcm_cosq_control_set(unit, out_gport, 0, bcmCosqControlBandwidthBurstMax, max_burst_kbits);
    if (rv != BCM_E_NONE) {
        printf("Error, in burst set, out_gport $out_gport max_burst_kbits $max_burst_kbits \n");
        return rv;
    }
    
    return rv;
}

/* Set Strict Priority Configuration
 * BE2 over BE1 over BE0
 */
int set_be_sp(int unit, int local_port_id){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    
    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    /* Set the gport_type according to the designated element */
    gport_type = bcmCosqGportTypeGlobalFmqBestEffort0; 
        
    /* Get the handle to the gport we want to config */
    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    out_gport = gport_info.out_gport;

    /* Set the Strict priority */
    rv = bcm_cosq_gport_sched_set(unit, out_gport, 0, BCM_COSQ_SP, -1);
    
    if (rv != BCM_E_NONE) {
        printf("Error, in SP set, out_gport $out_gport fmq_num  $fmq_num \n");
        return rv;
    }
    
    return rv;
}

/* Generic set weight function 
 * Element should be: bestEffort0, bestEffort1, bestEffort2 
 */ 
int set_be_weight(int unit, int local_port_id, credit_element element, int weight){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    /* Set the gport_type according to the designated element */
    if(element == bestEffort0) 
        gport_type = bcmCosqGportTypeGlobalFmqBestEffort0; 
    if(element == bestEffort1) 
        gport_type = bcmCosqGportTypeGlobalFmqBestEffort1; 
    if(element == bestEffort2) 
        gport_type = bcmCosqGportTypeGlobalFmqBestEffort2; 

    /* Get the handle to the gport we want to config */
    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    out_gport = gport_info.out_gport;

    /* Set the weight of the designated queue
    *  mode = -1 becuase we are setting weight and not strict priority
    */
    rv = bcm_cosq_gport_sched_set(unit, out_gport, 0, -1, weight);
    if (rv != BCM_E_NONE) {
        printf("Error, in weight set, out_gport $out_gport  weight $weight \n");
        return rv;
    }
    
    return rv;
}

/* Set TC Mapping
 * incoming tc = 0-7
 * mc_queue = 0-3
 */
int set_tc_mapping(int unit, int local_port_id, int incoming_tc, int mc_queue){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    /* The type we need in order to set the TC mapping is ROOT */
    gport_type = bcmCosqGportTypeGlobalFmqRoot; 

    /* Get the handle to the gport we want to config */
    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    out_gport = gport_info.out_gport;

    rv = bcm_cosq_port_mapping_set(unit, out_gport, incoming_tc, mc_queue);
    if (rv != BCM_E_NONE) {
        printf("Error, in tc mapping, out_gport $out_gport  weight $weight \n");
        return rv;
    }

    return rv;
}

/*  Create Egress Multicast Group 
 *  with given mc_group_id and add destination port
 *  matching dest_local_port_id.
 *  cud = Encap ID.
 */
int open_egress_mc_group(int unit, int mc_group_id, int dest_local_port_id, int cud){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t dest_gport;
    bcm_cosq_gport_type_t gport_type;
    int flags;
    bcm_multicast_t mc_group = mc_group_id;
    
    gport_type = bcmCosqGportTypeLocalPort; 

    /* create Multicast Egress Group with given mc_group_id */
    flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID ;
    rv = bcm_multicast_create(unit, flags, mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc create, flags $flags mc_group $mc_group \n");
        return rv;
    }

    /* find info of gport matching des_local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,dest_local_port_id); 
    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }
    
    /* add matching destination port to MC group created */
    dest_gport  = gport_info.out_gport;
    rv = bcm_multicast_egress_add(unit, mc_group, dest_gport, cud);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc egress add, mc_group $mc_group dest_gport $dest_gport \n");
        return rv;
    }

    return rv;
}

/*  Setup MAC forwading
 *  dest_type = 0 for Local Port
 *              1 for MC Group
 */
int setup_mac_forwarding(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_id){
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t dest_gport;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);   
    /* Create MC or PORT address forwarding */
    if(dest_type == 0) {
        l2_addr.flags = BCM_L2_STATIC;
        BCM_GPORT_LOCAL_SET(dest_gport, dest_id);
        l2_addr.port = dest_gport;
    } else {
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
        l2_addr.l2mc_group = dest_id;
    }
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in setup_mac_forwarding, dest_type $dest_type dest_id $dest_id \n");
        return rv;
    }

    return rv;
}

/* Main function
 * is_tm = 1 for TM port, 0 for PP port.
 */
int fabric_mc_credit_example(int unit, int bcm_local_port_id, int is_tm){
    bcm_error_t rv = BCM_E_NONE;
    /* Run Traffic with multicast destination, multicast ID 5005 */
    int mc_group_id = 5005;

    /* Init
     * Here we open an Egress MC group and forward traffic to it.
     * This is done in order to actualy have traffic go through our configuration.
     */
     
    /* Open Egress MC Group */
    rv = open_egress_mc_group(unit, mc_group_id, bcm_local_port_id, 1);
    if (rv != BCM_E_NONE) return rv;
    /* Setup MAC forwarding */
    if(is_tm == 0) {
        bcm_mac_t incoming_mac;
        incoming_mac[5] = 0x1;
        int incoming_vlan = 1;
        rv = setup_mac_forwarding(unit, incoming_mac, incoming_vlan, 1, mc_group_id);
        if (rv != BCM_E_NONE) return rv;
    }

    /* Set Shapers:
     *   We will set the root shaper to 5G
     *   We will set the GR shaper to 3G
     *   We will set the BE shaper to 2G
     * 
     *   We must also set the burst for the GR and BE shapers,
     *   otherwise we wont see any traffic at all.
     */
    int root_bw = 5000000;
    int gr_bw = 3000000;
    int be_bw = 2000000;
    rv = set_bandwidth(unit, bcm_local_port_id, root, root_bw);
    if (rv != BCM_E_NONE) return rv;
    rv = set_bandwidth(unit, bcm_local_port_id, guaranteed, gr_bw);
    if (rv != BCM_E_NONE) return rv;
    rv = set_bandwidth(unit, bcm_local_port_id, bestEffortAggregate, be_bw);
    if (rv != BCM_E_NONE) return rv;
    rv = set_burst(unit, bcm_local_port_id, guaranteed, 10);
    if (rv != BCM_E_NONE) return rv;
    rv= set_burst(unit, bcm_local_port_id, bestEffortAggregate, 10);
    if (rv != BCM_E_NONE) return rv;

    /* Set Strict Priority
     *   Will set the following: BE2 over BE1 over BE0
     * 
     *   Setting weights will cancel this configuration
     */
    rv = set_be_sp(unit, bcm_local_port_id);
    if (rv != BCM_E_NONE) return rv;

    /* Set Best Effort Weight 
     *   We will set the weight for BE0 to 1
     *   We will set the weight for BE1 to 2
     *   We will set the weight for BE2 to 3
     * 
     *   This will cancel the SP setting from above
     */
    int be0_weight = 1;
    int be1_weight = 2;
    int be2_weight = 3;
    rv = set_be_weight(unit, bcm_local_port_id, bestEffort0, be0_weight);
    if (rv != BCM_E_NONE) return rv;
    rv = set_be_weight(unit, bcm_local_port_id, bestEffort1, be1_weight);
    if (rv != BCM_E_NONE) return rv;
    rv = set_be_weight(unit, bcm_local_port_id, bestEffort2, be2_weight);
    if (rv != BCM_E_NONE) return rv;

    /* Set TC mapping
     *   TC        MC_QUEUE
     *   0-1        0
     *   2-3        1
     *   4-5        2
     *   6-7        3
     */
    rv = set_tc_mapping(unit, bcm_local_port_id, 0, 0);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 1, 0);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 2, 1);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 3, 1);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 4, 2);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 5, 2);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 6, 3);
    if (rv != BCM_E_NONE) return rv;
    rv = set_tc_mapping(unit, bcm_local_port_id, 7, 3);
    if (rv != BCM_E_NONE) return rv;

    return rv;
}



