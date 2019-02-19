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
/*##################   INBND FC GENERATION  ####################*/
/*##############################################################*/

int cint_arad_test_fc_inbnd_gl_gen_set_example(int unit, int target_port, int shaped_port, bcm_port_control_t fc_control /*LLFC or PFC*/)
{
    int rv = BCM_E_NONE;
    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    bcm_gport_t local_gport;

    bcm_cosq_pfc_config_t threshold;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    if ((fc_control != bcmPortControlLLFCTransmit) && (fc_control != bcmPortControlPFCTransmit)) {
        printf("fc_control to cint_arad_test_fc_inbnd_gl_gen_set_example can be either bcmPortControlPFCTransmit or bcmPortControlLLFCTransmit\n");
        return rv;
    }
    /*Setting target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, target_port, fc_control, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_arad_test_fc_inbnd_gl_gen_set_example\n");
        return rv;
    }
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
        printf("setting vsq fc threshold failed in cint_arad_test_fc_inbnd_gl_gen_set_example\n");
        return rv;
    }

    /*Configuring inbnd fc*/
    BCM_GPORT_LOCAL_SET(fc_handler.port,target_port);
    fc_trigger.port = vsq_gport;
    fc_handler.flags = BCM_COSQ_FC_ENDPOINT_PORT_ALL;
    fc_trigger.flags = 0;
    fc_trigger.cosq = fc_handler.cosq = 0;
    fc_trigger.calender_index = fc_handler.calender_index = 0;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlGeneration,&fc_trigger,&fc_handler);
    if (rv != BCM_E_NONE) {
        printf("Inbnd fc generation configuration failed in cint_arad_test_fc_inbnd_gl_gen_set_example\n");
        return rv;
    }

    /*Shaping local port - for testing purposes {*/
    BCM_GPORT_LOCAL_SET(local_gport,shaped_port);
    rv = bcm_cosq_gport_bandwidth_set(unit,local_gport,0,0,1000000,0);
    if (rv != BCM_E_NONE) {
        printf("Failed to shape in cint_arad_test_fc_inbnd_gl_gen_set_example. port %d\n",shaped_port);
        return rv;
    }
    /*}*/
    return rv;
}    

