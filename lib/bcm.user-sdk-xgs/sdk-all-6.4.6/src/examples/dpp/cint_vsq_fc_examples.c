

/*
 * $Id: cint_vsq_fc_examples.c,v 1.4 Broadcom SDK $
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
 * File: cint_vsq_fc_examples.c
 * VSQ FC (LLFC, PFC)
 * Purpose: Example of how to configure FC for specific VSQs (LLFC, PFC)
 * 
 * TM example script.
 * The focus of the CINT is to show how to enable a LLFC, PFC.
 * VSQ used here are from vsq group: LLFC, PFC.
 * 
 * Application Sequence LLFC:
 *  1.  Create a VSQ-LFFC.
 *  2.  Set FC threshold settings to be triggered by the VSQ.
 *  3.  Enable admission test of the LLFC category.
 *  4.  Enable FC generation
 * 
 * Application Sequence PFC:
 *  1.  Create a VSQ-PFC.
 *  2.  Set FC threshold settings to be triggered by the VSQ.
 *  3.  Enable admission test of the PFC category.
 *  4.  Enable FC generation
 * 
 *  The application assumes that queue/port mappings are configured before running the examples.
 *  Current parameters defined are according to diag_init application.
 *  
 */


/* 
 * PFC Example
 * This example sets PFC Vsq
 * SOC property PDM mode must be reduced (mandatory for LLFC-VSQ, PFC-VSQ, or ST-VSQ)
 * bcm886xx_pdm_mode.BCM88650_A0=1
 * 
 * Traffic class two parameters:
 *  traffic_class_packet : TC on the ITMH or the TC resolved from PP processing.
 *  traffic_class_pfc_vsq : TC for the PFC VSQ.
 * Expectation:
 * Send traffic from source-port and priority TC
 *
 * Results: 
 * FC generation is being triggered and Pause frames are being send.
 * Example run: run_vsq_pfc_example(0,13,0,1); 
 * 
 */
int run_vsq_pfc_example(int unit,int source_port, int traffic_class_packet, int traffic_class_pfc_vsq)
{
    bcm_error_t rv = BCM_E_NONE;  
    int max_queue_size;
    int dp;
    int index;
    bcm_switch_control_t type;
    bcm_gport_t vsq;    
    int admission_test; 
    bcm_cosq_pfc_config_t pfc_threshold; 
    bcm_cosq_vsq_info_t vsq_info;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlGeneration;
        
    /* Create VSQ gport */
    vsq_info.flags = BCM_COSQ_VSQ_PFC;
    vsq_info.src_port = source_port;
    vsq_info.traffic_class = traffic_class_pfc_vsq;    
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_info,&vsq);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_create\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set mapping traffic class to PFC VSQ */
    switch (traffic_class_packet) {
      case 0:
      type = bcmSwitchPFCClass0Queue;
      break;
      case 1:
      type = bcmSwitchPFCClass1Queue;
      break;
      case 2:
      type = bcmSwitchPFCClass2Queue;
      break;
      case 3:
      type = bcmSwitchPFCClass3Queue;
      break;
      case 4:
      type = bcmSwitchPFCClass4Queue;
      break;
      case 5:
      type = bcmSwitchPFCClass5Queue;
      break;
      case 6:
      type = bcmSwitchPFCClass6Queue;
      break;
      case 7:
      type = bcmSwitchPFCClass7Queue;
      break;
    default:
      printf("Error invalid traffic class\n");
      return BCM_E_PARAM;
    }
    bcm_switch_control_port_set(unit, source_port, type, traffic_class_pfc_vsq);
    
    /* Set FC settings. */
    /* An example of FC settings */   
    pfc_threshold.xoff_threshold = 0;
    pfc_threshold.xon_threshold = 0;
    pfc_threshold.xoff_threshold_bd = 100;
    pfc_threshold.xon_threshold_bd = 100;    
    rv = bcm_cosq_pfc_config_set(unit, vsq, 0, 0, &pfc_threshold);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    return rv;
}

/* 
 * LLFC Example
 * This example sets LLFC Vsq
 * SOC property PDM mode must be reduced (mandatory for LLFC-VSQ, PFC-VSQ, or ST-VSQ)
 * bcm886xx_pdm_mode.BCM88650_A0=1
 * 
 * Expectation:
 * Send traffic from source-port
 *
 * Results: 
 * FC generation is being triggered and Pause frames are being send.
 * Example run: run_vsq_llfc_example(0,13); 
 * 
 */
int run_vsq_llfc_example(int unit,int source_port)
{
    bcm_error_t rv = BCM_E_NONE;  
    int max_queue_size;
    int dp;
    int index;
    bcm_gport_t vsq;    
    int admission_test; 
    bcm_cosq_pfc_config_t pfc_threshold; 
    bcm_cosq_vsq_info_t vsq_info;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlGeneration;
        
    /* Create VSQ gport */
    vsq_info.flags = BCM_COSQ_VSQ_LLFC;
    vsq_info.src_port = source_port;    
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_info,&vsq);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_cosq_gport_vsq_create\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* Set FC settings. */
    /* An example of FC settings */   
    pfc_threshold.xoff_threshold = 10;
    pfc_threshold.xon_threshold = 20;
    pfc_threshold.xoff_threshold_bd = 100;
    pfc_threshold.xon_threshold_bd = 200;    
    rv = bcm_cosq_pfc_config_set(unit, vsq, 0, 0, &pfc_threshold);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* Set the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));
    source.cosq = -1;
    source.port = vsq;
    target.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}
