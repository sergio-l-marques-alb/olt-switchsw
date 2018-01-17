/*
 * $Id: $
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
 * File: cint_jer_fc_trigger_config_example.c
 */
/* 
 * Purpose: 
 *   
 *   Example of trigger FC for jericho device.
 *
 */

enum global_vsq_src_e {
    global_vsq_src_bdb,
    global_vsq_src_mini_db,
    global_vsq_src_full_db,
    global_vsq_src_ocb_db
};

int cint_jer_test_fc_gl_trigger_set_example(int unit, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_threshold_t fc_threshold;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /*Setting GL vsq threshold*/
    bcm_cosq_threshold_t_init(&fc_threshold);   
    switch (priority) {
        case 0:
            fc_threshold.priority = BCM_COSQ_HIGH_PRIORITY;
            break;
        case 1:
            fc_threshold.priority = BCM_COSQ_LOW_PRIORITY;
            break;
        defalut:
            if (priority != BCM_COSQ_HIGH_PRIORITY && priority != BCM_COSQ_LOW_PRIORITY)
            return BCM_E_PARAM;        
    }
    fc_threshold.flags = BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL|BCM_COSQ_THRESHOLD_SET;
    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            fc_threshold.type = bcmCosqThresholdBufferDescriptorBuffers;
            break;
        case global_vsq_src_mini_db: 
            fc_threshold.type = bcmCosqThresholdMiniDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_MULTICAST;
            break;
        case global_vsq_src_full_db: 
            fc_threshold.type = bcmCosqThresholdFullDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_MULTICAST;
            break;
        case global_vsq_src_ocb_db: 
            fc_threshold.type = bcmCosqThresholdDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_OCB;
            break;
        default: 
            break;
    }
    fc_threshold.value = 0x7f0000; 
    rv = bcm_cosq_gport_threshold_set(unit, vsq_gport, 0, &fc_threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc set threshold in cint_jer_test_fc_gl_trigger_set_example\n", rv);
        return rv;
    }

    fc_threshold.flags &= ~(BCM_COSQ_THRESHOLD_SET);
    fc_threshold.flags |= BCM_COSQ_THRESHOLD_CLEAR;
    fc_threshold.value = 0x7f0000; 
    rv = bcm_cosq_gport_threshold_set(unit, vsq_gport, 0, &fc_threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc clear threshold in cint_jer_test_fc_gl_trigger_set_example\n", rv);
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to shape in bcm_cosq_gport_bandwidth_set. port %d\n", rv, local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}  

int cint_jer_test_fc_gl_trigger_unset_example(int unit, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_threshold_t fc_threshold;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /*Setting GL vsq threshold*/
    bcm_cosq_threshold_t_init(&fc_threshold);   
    switch (priority) {
        case 0:
            fc_threshold.priority = BCM_COSQ_HIGH_PRIORITY;
            break;
        case 1:
            fc_threshold.priority = BCM_COSQ_LOW_PRIORITY;
            break;
        defalut:
            if (priority != BCM_COSQ_HIGH_PRIORITY && priority != BCM_COSQ_LOW_PRIORITY)
            return BCM_E_PARAM;        
    }
    fc_threshold.flags = BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL|BCM_COSQ_THRESHOLD_SET;
    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            fc_threshold.type = bcmCosqThresholdBufferDescriptorBuffers;
            break;
        case global_vsq_src_mini_db: 
            fc_threshold.type = bcmCosqThresholdMiniDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_MULTICAST;
            break;
        case global_vsq_src_full_db: 
            fc_threshold.type = bcmCosqThresholdFullDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_MULTICAST;
            break;
        case global_vsq_src_ocb_db: 
            fc_threshold.type = bcmCosqThresholdDbuffs;
            fc_threshold.flags |= BCM_COSQ_THRESHOLD_OCB;
            break;
        default: 
            break;
    }
    fc_threshold.value = 0; 
    rv = bcm_cosq_gport_threshold_set(unit, vsq_gport, 0, &fc_threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc set threshold in cint_jer_test_fc_gl_trigger_set_example\n", rv);
        return rv;
    }

    fc_threshold.flags &= ~(BCM_COSQ_THRESHOLD_SET);
    fc_threshold.flags |= BCM_COSQ_THRESHOLD_CLEAR;
    fc_threshold.value = 0; 
    rv = bcm_cosq_gport_threshold_set(unit, vsq_gport, 0, &fc_threshold);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to set fc set threshold in cint_jer_test_fc_gl_trigger_set_example\n", rv);
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) to shape in cint_arad_fc_oob_llfc_gen_config_test_example. port %d\n", rv, local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}