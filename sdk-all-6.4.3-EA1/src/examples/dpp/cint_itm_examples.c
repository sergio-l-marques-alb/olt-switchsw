/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: Ingress Queue Manager~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $Id: cint_itm_examples.c,v 1.2 Broadcom SDK $
 *
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
 * File: cint_itm_examples.c
 * Purpose: Example of how to configure WRED, a tail drop (max queue size), and an incoming TC mapping.
 *  
 * There are 2 main examples:
 *      1. WRED - configuring WRED for a specefic queue.
 *      2. Tail Drop - configuring Max Queue size for 2 queues
 *
 * It is assumed diag_init is executed and queue/port mapping are done before running the examples.
 * Current parameters defined are according to diag_init application.
 * 
 * Settings include:
 *  o    Enabling WRED with max/min threshold values.
 *  o    Max queue size (Tail Drop) per DP for 2 different queues.
 *  o    TC mapping from 8 to 4: 0,1 ' 0    ,   2,3 ' 1, and so on.
 *  o    Running traffic with TC 0/2 and see the difference in latency (when using the WRED example)
 *      and difference in dropped packets (when using the tail drop example).
 *  o    Traffic configuration is defined according parameter base_queue.
 *      Destination traffic is the system port that is mapped by base_queue.
 * 
 * You can also use the individual functions that are included in the example file.
 * 
 */

/* 
 * Create VOQ queue
 */
int create_voq(int unit,int base_queue, int num_cos_level,int out_port)
{
    bcm_error_t rv;
    bcm_gport_t gport, gport_in;

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport,base_queue);
    BCM_GPORT_LOCAL_SET(gport_in,out_port);

    rv = bcm_cosq_gport_add(unit,gport_in,num_cos_level,BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,&gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in creating voq, voq: %d, num_cos: %d \n",base_queue,num_cos_level);
    }
    
    return rv;
}

/*
 * Set WRED configuration
 */
int set_wred_settings(int unit, int base_queue, int cosq, int is_enable, int drop_precedence, int max_thresh, int min_thresh, int max_prob, int gain)
{
   bcm_error_t rv = BCM_E_NONE;
   
   printf("Set WRED Settings unit %d, qid %d, is_enable %d, drop index %d, max thresh %d, min thresh %d, max prob %d gain %d \n",
          unit,base_queue+cosq,is_enable,drop_precedence,max_thresh,min_thresh,max_prob,gain);

   bcm_cosq_gport_discard_t discard;
   bcm_cosq_gport_discard_t_init(&discard);
   
   bcm_gport_t gport;
   BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport,base_queue);
   
   /* Fill the bcm_cosq_gport_discard_t struct with the requierd data */

   discard.flags = BCM_COSQ_DISCARD_BYTES;
   if (is_enable == 1)
       discard.flags |= BCM_COSQ_DISCARD_ENABLE;       
   
   switch (drop_precedence) {
   case 0:
       discard.flags |= BCM_COSQ_DISCARD_COLOR_GREEN;   
       break;
   case 1:
       discard.flags |= BCM_COSQ_DISCARD_COLOR_YELLOW;  
       break;
   case 2:
       discard.flags |= BCM_COSQ_DISCARD_COLOR_RED;
       break;
   case 3:
       discard.flags |= BCM_COSQ_DISCARD_COLOR_BLACK;
       break;
   default:
       discard.flags |= BCM_COSQ_DISCARD_COLOR_ALL;
   }
   
   discard.min_thresh = min_thresh;
   discard.max_thresh = max_thresh;
   discard.drop_probability = max_prob;
   discard.gain = gain;
    
   rv = bcm_cosq_gport_discard_set(0,gport,cosq,&discard);
   if (rv != BCM_E_NONE) {
       printf("bcm_cosq_gport_discard_set() failed $rv\n");
       return rv;
   }

   return rv;
}

/*
 * Set Tail Drop setting for a certain queue. 
 * If DP == -1 , set tail drop over all DPs 
 */
int set_voq_tail_drop_settings(int unit,int base_queue,int cosq,int dp,int max_size)
{
   bcm_error_t rv = BCM_E_NONE;
   bcm_gport_t gport;
   uint32 flags;
   bcm_cosq_gport_size_t gport_size;
   
   printf("Set Tail drop Settings unit %d, qid %d, max_size %d \n",
          unit,base_queue+cosq,max_size);

   BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport,base_queue);

   flags = BCM_COSQ_GPORT_SIZE_BYTES;

   if (dp == -1) {
       flags |= BCM_COSQ_GPORT_SIZE_COLOR_BLIND;
       dp = 0;
   }

   gport_size.size_min = 0;
   gport_size.size_max = max_size;

   rv = bcm_cosq_gport_color_size_set(unit,gport,cosq,dp,flags,&gport_size);
   if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_color_size_set() failed $rv\n");
        printf("gport: (0x%08x), cosq: %d, dp: %d, flags 0x%08x\n",
               gport,cosq,dp,flags);
        print gport_size;
      return rv;
   }

   return rv;
}

/* Map incoming TC
 * Use this to map the packet TC to a new TC 
 */ 
int set_global_tc_mapping(int unit,int packet_tc,int new_tc)
{
    bcm_error_t rv = BCM_E_NONE;
    
    printf("Set Global tc mapping unit %d, packet_tc %d, new_tc %d \n",
          unit,packet_tc,new_tc);
    
    rv = bcm_cosq_mapping_set(unit,packet_tc,new_tc);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_mapping_set() failed $rv\n");
        return rv;
    }

    return rv;
}

/* Set OFP Bandwidth (max KB per sec)
 *  Configure the Port Shaper's max rate
 */
int set_ofp_bandwidth(int unit, int local_port_id, int max_kbits_sec){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;

    int min_kbits_rate = 0;
    int flags = 0;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport, local_port_id); 

    gport_type = bcmCosqGportTypeLocalPort; 

    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_type $gport_type \n");
        return rv;
    }

    out_gport = gport_info.out_gport;
    rv = bcm_cosq_gport_bandwidth_set(unit, out_gport, 0, min_kbits_rate, max_kbits_sec, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bandwidth set, out_gport $out_gport max_kbits_sec $max_kbits_sec \n");
        return rv;
    }

    return rv;
}

/* WRED Example
 *  This example sets the port's shaper (to create oversubscription)
 *  and enable WRED.
 *  Example:
 *  run_wred_example(0,2,8);
 */
int run_wred_example(int unit,int local_port_id,int base_queue)
{
    bcm_error_t rv;

    /* Set the port Bandwidth (to create oversubscription, so the WRED will be meaningful) */
    int bandwidth = 3000000;
    rv = set_ofp_bandwidth(unit,local_port_id,bandwidth);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* WRED parameters */
    int is_enable = 1;
    int drop_precedence = -1;
    int max_thresh = 100;
    int min_thresh = 50;
    int max_prob = 100;
    int gain = 1;
      int cosq = 0;

    rv = set_wred_settings(unit,base_queue,cosq,is_enable,drop_precedence,max_thresh,min_thresh,max_prob,gain);    
    if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
    }

    return rv;
}

/* Tail Drop Example
 * This example sets Tail Drop per queue, and configure the TC mapping.
 * Example: 
 * run_tail_drop_example(0,2,8) 
 */
int run_tail_drop_example(int unit,int local_port_id,int base_queue)

{
    bcm_error_t rv = BCM_E_NONE;
    int max_queue_size;
    int dp;
    int index;

    /* An example of tail drop settings */
    /* queue base_queue with (TC 0/1) will have more packets dropped than with (TC 2/3) */
    for (dp = 0; dp < 4; dp++) {
        max_queue_size = 500;
        rv = set_voq_tail_drop_settings(unit,base_queue,0,dp,max_queue_size);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        max_queue_size = max_queue_size * 2;
        rv = set_voq_tail_drop_settings(unit,base_queue,1,dp,max_queue_size);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

    }

    /* An example of global mapping */
    /* 8 to 4. i.e. tc 0-1 -> 0 2-3 -> 1,...*/
    for (index = 0; index < 8; index++) {
        rv = set_global_tc_mapping(unit,index,index/2);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}
