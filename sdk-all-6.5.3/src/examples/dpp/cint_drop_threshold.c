
/* $Id: cint_drop_threshold.c,v 1.4 Broadcom SDK $
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
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: Egress Transmit~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  
 * File:        cint_drop_threshold.c
 * Purpose:     Example of configuration for the drop thresholds of queues and ports.
 * 
 * It is assumed diag_init is executed.
 *
 * Settings include:
 *  o    Threshold Type - ThresholdBytes is the example's default.
 *  o    Threshold value.
 * 
 *  CINT usage:
 *  o    Run application: 
 *          drop_threshold_example(unit,local_port_id,threshold_value)
 *  o    The application will set the queue's and port's bytes threshold to the given value.
 *  o    Run traffic to the above port. You can see the drop rate change according to the threshold type/value.
 */


/*  Set Queue Drop Threshold
*   queue = BCM_COSQ_LOW_PRIORITY
*   BCM_COSQ_HIGH_PRIORITY
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
 int set_queue_drop_threshold(int unit, int local_port_id,int uc_mc, int queue,  int color, bcm_cosq_threshold_type_t threshold_type, int value){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    /* Set gport_type and threshold flag according to uc_mc argument */
    if(uc_mc == 0) {
      gport_type = bcmCosqGportTypeUnicastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      gport_type = bcmCosqGportTypeMulticastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;

      threshold.flags |= BCM_COSQ_THRESHOLD_PER_DP;
    }
    

    threshold.dp = color;

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
      printf("Error, in handle get, gport_type $gport_type \n");
      return rv;
    }

    out_gport = gport_info.out_gport;

    /* Fill the Threshold Struct
     *  In this example we only set the Drop Threshold
     */
    threshold.type = threshold_type;
    threshold.value = value;
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      BCM_COSQ_THRESHOLD_DROP;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, queue, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in queue threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}

/*  Set Port Drop Threshold
*   uc_mc = 0 for UC
*   1 for MC
*   threshold_type according to bcm_cosq_threshold_type_t types
*/ 
int set_port_drop_threshold(int unit, int local_port_id, int uc_mc, bcm_cosq_threshold_type_t threshold_type, int value){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    if(uc_mc == 0) {
      gport_type = bcmCosqGportTypeUnicastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      gport_type = bcmCosqGportTypeMulticastEgress; 
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    }

    rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
    if (rv != BCM_E_NONE) {
      printf("Error, in handle get, gport_type $gport_type \n");
      return rv;
    }

    out_gport = gport_info.out_gport;

    // Fill the Threshold Struct 
    threshold.type = threshold_type;
    threshold.value = value;
    if(uc_mc == 0) {
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else {
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    }
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      BCM_COSQ_THRESHOLD_DROP;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, -1, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in port threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}

int set_device_drop_threshold(int unit, int uc_mc, int cosq, bcm_cosq_threshold_type_t threshold_type, int value)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t out_gport;
    bcm_cosq_threshold_t threshold;

    /* Set GPORT to be global */
    out_gport = 0;

    if(uc_mc == 0) { /* UC */
      threshold.flags = BCM_COSQ_THRESHOLD_UNICAST;
    } else if (uc_mc == 1) { /* MC */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST;
    } else if (uc_mc == 2) { /* SP 0 */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP0;
    } else if (uc_mc == 3) { /* SP 1 */
      threshold.flags = BCM_COSQ_THRESHOLD_MULTICAST_SP1;
    }
    
    /* Fill the Threshold Struct */
    threshold.type = threshold_type;
    threshold.value = value;
    
    threshold.flags = threshold.flags |
      BCM_COSQ_THRESHOLD_EGRESS | 
      BCM_COSQ_THRESHOLD_DROP;

    rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, threshold);

    if (rv != BCM_E_NONE) {
      printf("Error, in port threshold set, out_gport $out_gport threshold_type $threshold_type value $value \n");
      return rv;
    }
    
    return rv;
}

/* 
*  Main function
*  Soc_petra-B functionality example
*
*/
int drop_threshold_example(int unit, int bcm_local_port_id, int threshold_bytes){
    bcm_error_t rv = BCM_E_NONE;

    /* Set UC Low Priority Queue bytes threshold */
    rv = set_queue_drop_threshold(unit, bcm_local_port_id, 0, 0, BCM_COSQ_LOW_PRIORITY, bcmCosqThresholdBytes, threshold_bytes);
    if (rv != BCM_E_NONE) return rv;

    /* Set UC High Priority Queue bytes threshold */
    rv = set_queue_drop_threshold(unit, bcm_local_port_id, 0, 0, BCM_COSQ_HIGH_PRIORITY, bcmCosqThresholdBytes, threshold_bytes);
    if (rv != BCM_E_NONE) return rv;

    /* Set Port MC bytes threshold */
    rv = set_port_drop_threshold(unit, bcm_local_port_id, 1, bcmCosqThresholdBytes, threshold_bytes);
    if (rv != BCM_E_NONE) return rv;

    return rv;
}

/*
 * Main function 
 * ARAD functionality example 
 */
int drop_treshold_arad_example(int unit, int bcm_local_port_id) {
    bcm_error_t rv = BCM_E_NONE;
    int total, total_pd, total_db, sp, cosq, val;    

    /* Queue settings example */
    /* QDCT table QueueUcPdDisTh */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 0, 1, 0, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* QQST table QueueUcDbDisTh */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 0, 1, 0, bcmCosqThresholdDataBuffers, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* QDCT table QUEUE_MC_PD_TH_DP_0 */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 1, 1, 1 /* DP */, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    total_pd = 0;
    /* QDCT table QueueMcPdRsvdTh */
    rv = set_queue_drop_threshold(unit , bcm_local_port_id, 1, 1, 1 /* DP */, bcmCosqThresholdAvailablePacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;
    /* By default all queues are mapped to SP 0 */
    total_pd += 1000; 

    /* Port settings example */
    /* PDCT table PortUcPdDisTh */
    rv = set_port_drop_threshold(unit , bcm_local_port_id, 0, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* PDCT table PortMcPdSheardTh */
    rv = set_port_drop_threshold(unit , bcm_local_port_id, 1, bcmCosqThresholdPacketDescriptors, 1000); 
    if (rv != BCM_E_NONE) return rv;

    /* General settings */
    /* CgmGeneralPdTh register Uc_Pd_Th */
    rv = set_device_drop_threshold(unit, 0, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register MC_Pd_Th */
    rv = set_device_drop_threshold(unit, 1, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register Total_Pd_Th */
    rv = set_device_drop_threshold(unit, -1, -1, bcmCosqThresholdPacketDescriptors, 2000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register MC_PD_SP_0_TH */
    rv = set_device_drop_threshold(unit, 2, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralPdTh register MC_PD_SP_1_TH */
    rv = set_device_drop_threshold(unit, 3, -1, bcmCosqThresholdPacketDescriptors, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register UC_DB_Th */
    rv = set_device_drop_threshold(unit, 0, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register MC_DB_Th */
    rv = set_device_drop_threshold(unit, 1, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register Total_DB_Th */
    rv = set_device_drop_threshold(unit, -1, -1, bcmCosqThresholdDataBuffers, 2000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register MC_DB_SP_0_TH */
    rv = set_device_drop_threshold(unit, 2, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* CgmGeneralDbTh register MC_DB_SP_1_TH */
    rv = set_device_drop_threshold(unit, 3, -1, bcmCosqThresholdDataBuffers, 1000);
    if (rv != BCM_E_NONE) return rv;

    /* Per TC */    
    for (sp = 0; sp < 2; sp++) {       
        total_db = 0;

        for (cosq = 0; cosq < 8; cosq++) {   
            /* CgmMcPdSpTcTh register CgmMcPdSpTcTh0-CgmMcPdSpTcTh15 */         
            rv = set_device_drop_threshold(unit, 2+sp, cosq, bcmCosqThresholdPacketDescriptors, 1000);
            if (rv != BCM_E_NONE) return rv;
            

            /* CgmMcDbSpTcTh register CgmMcDbSpTcTh0-CgmMcDbSpTcTh15 */
            rv = set_device_drop_threshold(unit, 2+sp, cosq, bcmCosqThresholdDataBuffers, 10000);
            if (rv != BCM_E_NONE) return rv;

            /* CgmMcRsvdDbSpTh register CgmMcRsvdDbSpTh0-CgmMcRsvdDbSpTh15 */
            rv = set_device_drop_threshold(unit, 2+sp, cosq, bcmCosqThresholdAvailableDataBuffers, 10000);
            if (rv != BCM_E_NONE) return rv;
            total_db += 10000;                       
        }

        /* CgmMcRsvdMaxVal register CgmMcRsvdPdSp0MaxVal, CgmMcRsvdPdSp1MaxVal */
        /* Max threshold of all queues */
        val = (sp == 0) ? total_pd:0;
        rv = set_device_drop_threshold(unit, 2+sp, -1, bcmCosqThresholdAvailablePacketDescriptors, val);
        if (rv != BCM_E_NONE) return rv;
        

        /* CgmMcRsvdMaxVal register CgmMcRsvdDbSp0MaxVal CgmMcRsvdDbSp1MaxVal */
        /* Max threshold of all TCs */
        rv = set_device_drop_threshold(unit, 2+sp, -1, bcmCosqThresholdAvailableDataBuffers, total_db);
        if (rv != BCM_E_NONE) return rv;
    }
          

    return rv;
}
