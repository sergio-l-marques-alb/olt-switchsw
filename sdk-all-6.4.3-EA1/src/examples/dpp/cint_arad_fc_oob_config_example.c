/*
/* $Copyright: Copyright 2012 Broadcom Corporation.
/* This program is the proprietary software of Broadcom Corporation
/* and/or its licensors, and may only be used, duplicated, modified
/* or distributed pursuant to the terms and conditions of a separate,
/* written license agreement executed between you and Broadcom
/* (an "Authorized License").  Except as set forth in an Authorized
/* License, Broadcom grants no license (express or implied), right
/* to use, or waiver of any kind with respect to the Software, and
/* Broadcom expressly reserves all rights in and to the Software
/* and all intellectual property rights therein.  IF YOU HAVE
/* NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
/* IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
/* ALL USE OF THE SOFTWARE.  
/*  
/* Except as expressly set forth in the Authorized License,
/*  
/* 1.     This program, including its structure, sequence and organization,
/* constitutes the valuable trade secrets of Broadcom, and you shall use
/* all reasonable efforts to protect the confidentiality thereof,
/* and to use this information only in connection with your use of
/* Broadcom integrated circuit products.
/*  
/* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
/* PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
/* REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
/* OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
/* DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
/* NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
/* ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
/* CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
/* OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
/* 
/* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
/* BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
/* INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
/* ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
/* TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
/* POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
/* THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
/* WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
/* ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$ */
/* $Id$ */

/*##############################################################*/
/*###################    OOB GENERATION     ####################*/
/*##############################################################*/

bcm_gport_t cttc_vsq_gport;

/*Sets a cttc_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | cttc |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_cttc_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port, int base_voq, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t voq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating a vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CTTC /*or BCM_COSQ_VSQ_CTCC...*/;
    vsq_inf.ct_id = 2; /*All queues*/
    vsq_inf.traffic_class = cosq;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }
    cttc_vsq_gport = vsq_gport;

    /*Associating the vsq with a voq(s)*/
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport,base_voq);
    rv = bcm_cosq_gport_vsq_add(unit, vsq_gport, voq_gport, cosq);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFF;
    threshold.xoff_threshold = 0xFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_cttc_gen_set_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}

int cint_arad_test_fc_oob_vsq_cttc_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port, int base_voq, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t voq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Getting vsq*/
    vsq_gport = cttc_vsq_gport;

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFFFFFFF;
    threshold.xoff_threshold = 0xFFFFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_cttc_gen_unset_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_unset_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_cttc_gen_unset_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}



int cint_arad_test_fc_oob_nif_mlf_gen_set_example(int unit,int local_traffic_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Config oob fc*/
    BCM_GPORT_LOCAL_SET(fc_trigger.port,local_traffic_port);
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_llfc_gen_set_example\n");
        return rv;
    }

    return rv;
}



/*Sets a gl_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | gl   |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only
 
 *                                                                                         *  
 *requires a calendar configuration of a minimum length of three slots (via soc properties)*
 *Make sure that the oob_calendar_index parameter < cal_len-2                              */
int cint_arad_test_fc_oob_gl_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0x7FFFFF;
    threshold.xoff_threshold = 0x7FFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,BCM_COSQ_HIGH_PRIORITY,BCM_COSQ_THRESHOLD_UNICAST,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n");
        return rv;
    }

    /*Configuring oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_fc_oob_llfc_gen_config_test_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}    

int cint_arad_test_fc_oob_gl_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed in cint_arad_test_fc_oob_vsq_gl_gen_unset_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0x0;
    threshold.xoff_threshold = 0x0;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,BCM_COSQ_HIGH_PRIORITY,BCM_COSQ_THRESHOLD_UNICAST,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_gl_gen_unset_example\n");
        return rv;
    }

    /*Deleting oob configuration*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_gl_gen_unset_example\n");
        return rv;
    }

    /*UNShaping local port {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_gl_gen_unset_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}    


/*Sets a ct_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | ct   |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_ct_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CT;
    vsq_inf.ct_id = 2; /*All queues*/
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFF;
    threshold.xoff_threshold = 0xFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_ct_gen_set_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_set_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_fc_oob_llfc_gen_config_test_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}

int cint_arad_test_fc_oob_vsq_ct_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CT;
    vsq_inf.ct_id = 2; /*All queues*/
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);

    /*UNSetting vsq threshold*/
    threshold.xon_threshold = 0xFFFFFFF;
    threshold.xoff_threshold = 0xFFFFFFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_ct_gen_unset_example\n");
        return rv;
    }

    /*Deleting oob configuration*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_unset_example\n");
        return rv;
    }

    /*UNshaping local port {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,10000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_llfc_gen_unset_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}




bcm_gport_t llfc_vsq_gport;
bcm_gport_t pfc_vsq_gport;

/*Sets a llfc_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | llfc |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_llfc_gen_set_example(int unit, int oob_port, int oob_calendar_idx,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Creating vsq*/
    vsq_inf.flags=BCM_COSQ_VSQ_LLFC;
    BCM_GPORT_LOCAL_SET(vsq_inf.src_port,local_traffic_port);
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    llfc_vsq_gport = vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0;
    threshold.xon_threshold = 0;
    threshold.xoff_threshold_bd = 0;
    threshold.xon_threshold_bd = 0; 
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_llfc_gen_set_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_set_example\n");
        return rv;
    }

    return rv;
}


int cint_arad_test_fc_oob_vsq_llfc_gen_unset_example(int unit, int oob_port, int oob_calendar_idx,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Getting vsq*/
    vsq_gport = llfc_vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0xFFFFF;
    threshold.xon_threshold = 0xFFFFF;
    threshold.xoff_threshold_bd = 0xFFFFF;
    threshold.xon_threshold_bd = 0xFFFFF; 
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_llfc_gen_unset_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_llfc_gen_unset_example\n");
        return rv;
    }

    return rv;
}


/*Sets a cttc_vsq as a fc trigger and a corresponding oob calendar slot as a fc handler 
  | pfc  |                ___________________________________________
  | vsq  | ------------> | calendar|                                 |
  |      |               |_slot_n__|_________________________________|  
  |      | 
  ||||||||  
  the shaped port parameter is for test puposes only*/
int cint_arad_test_fc_oob_vsq_pfc_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int cos,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_PFC;
    vsq_inf.traffic_class = cos;
    BCM_GPORT_LOCAL_SET(vsq_inf.src_port,local_traffic_port);
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    pfc_vsq_gport = vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0;
    threshold.xon_threshold = 0;
    threshold.xoff_threshold_bd = 0;
    threshold.xon_threshold_bd = 0;   
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_pfc_gen_set_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_pfc_gen_set_example\n");
        return rv;
    }

    return rv;
}

int cint_arad_test_fc_oob_vsq_pfc_gen_unset_example(int unit, int oob_port, int oob_calendar_idx, int cos,int local_traffic_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;


    /*Getting vsq*/
    vsq_gport = pfc_vsq_gport;

    /*Setting vsq threshold*/
    threshold.xoff_threshold = 0xFFFFF;
    threshold.xon_threshold = 0xFFFFF;
    threshold.xoff_threshold_bd = 0xFFFFF;
    threshold.xon_threshold_bd = 0xFFFFF;   
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_pfc_gen_unset_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = oob_calendar_idx;
    fc_trigger.flags = fc_handler.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration cint_arad_test_fc_oob_vsq_pfc_gen_unset_example\n");
        return rv;
    }

    return rv;
}

int cint_arad_test_hcfc_fc_oob_vsq_cttc_gen_set_example(int unit, int oob_port, int oob_calendar_idx, int shaped_port, int base_voq, int cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t voq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    /*Creating a vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_CTTC /*or BCM_COSQ_VSQ_CTCC...*/;
    vsq_inf.ct_id = 2; /*All queues*/
    vsq_inf.traffic_class = cosq;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }
    cttc_vsq_gport = vsq_gport;

    /*Associating the vsq with a voq(s)*/
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport,base_voq);
    rv = bcm_cosq_gport_vsq_add(unit, vsq_gport, voq_gport, cosq);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Setting vsq threshold*/
    threshold.xon_threshold = 0xFF;
    threshold.xoff_threshold = 0xFF;
    rv = bcm_cosq_pfc_config_set(unit,vsq_gport,0,0,&threshold);
    if (rv != BCM_E_NONE) {
        printf("setting vsq fc threshold failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Config oob fc*/
    BCM_GPORT_CONGESTION_SET(fc_handler.port,oob_port);
    fc_handler.calender_index = 0;
    fc_handler.cosq = cosq;
    fc_handler.priority = oob_calendar_idx;
    fc_handler.flags = BCM_COSQ_FC_HCFC_BITMAP;
    fc_trigger.flags = 0;
    fc_trigger.cosq = 0;
    fc_trigger.port = vsq_gport;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("OOB fc generation configuration failed in cint_arad_test_fc_oob_vsq_cttc_gen_set_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_oob_vsq_cttc_gen_set_example. port %d\n",local_traffic_port);
        return rv;
    }
    /*}*/
    return rv;
}


/*##############################################################*/
/*##################     OOB RECEPTION     #####################*/
/*##############################################################*/

/*Sets an oob rx slot as a fc reception object and a port as the fc target*/
int cint_arad_test_fc_oob_llfc_rec_set_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /*Config oob fc*/
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port);
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    fc_target.cosq=-1;
    fc_reception_port.calender_index=oob_calendar_idx;
    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration cint_arad_test_fc_oob_llfc_rec_set_example\n");
    }
    return rv;
}


int cint_arad_test_fc_oob_llfc_rec_unset_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /*Config oob fc*/
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port);
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    fc_target.cosq=-1;
    fc_reception_port.calender_index=oob_calendar_idx;
    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration cint_arad_test_fc_oob_llfc_rec_unset_example\n");
    }
    return rv;
}

/*Sets an oob rx slot as a fc reception object and a port priority traffic as the fc target*/
int cint_arad_test_fc_oob_pfc_rec_set_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /*Config oob fc*/
    BCM_COSQ_GPORT_E2E_PORT_SET(fc_target.port,target_port) /*Can also be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    fc_target.cosq=0;
    fc_reception_port.calender_index=oob_calendar_idx;
    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed in cint_arad_test_fc_oob_pfc_rec_set_example\n");
    }
    return rv;
}

int cint_arad_test_fc_oob_pfc_rec_unset_example(int unit,int target_port, int oob_port, int oob_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /*Config oob fc*/
    BCM_COSQ_GPORT_E2E_PORT_SET(fc_target.port,target_port) /*Can also be local, or modport etc..*/;
    BCM_GPORT_CONGESTION_SET(fc_reception_port.port,oob_port);
    fc_target.cosq=0;
    fc_reception_port.calender_index=oob_calendar_idx;
    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("OOB fc reception configuration failed in cint_arad_test_fc_oob_pfc_rec_unset_example\n");
    }
    return rv;
}

/*Traffic Setting func. for tests purposes*/
int cint_arad_test_fc_device_traffic_set(int ix_unit, int local_ix_port, int local_d2d_unit1_port_1, int local_d2d_unit1_port_2, int unit_2, int local_d2d_unit2_port_1 , int local_d2d_unit2_port_2)
{
    int rv = BCM_E_NONE;

    rv = bcm_port_force_forward_set(ix_unit,local_ix_port,local_d2d_unit1_port_1,1);
    if (rv != BCM_E_NONE) {
        printf("cint_arad_test_fc_rec_device_traffic_set failed to set traffic from ixia port to d2d port\n");
        return rv;
    }
    rv = bcm_port_force_forward_set(ix_unit,local_d2d_unit1_port_2,local_ix_port,1);
    if (rv != BCM_E_NONE) {
        printf("cint_arad_test_fc_rec_device_traffic_set failed to set traffic from d2d port to ixia port\n");
        return rv;
    }
    rv = bcm_port_force_forward_set(unit_2,local_d2d_unit2_port_1,local_d2d_unit2_port_2,1);
    if (rv != BCM_E_NONE) {
        printf("cint_arad_test_fc_rec_device_traffic_set failed to set traffic from d2d port to itself\n");
        return rv;
    }
    return rv;
}

