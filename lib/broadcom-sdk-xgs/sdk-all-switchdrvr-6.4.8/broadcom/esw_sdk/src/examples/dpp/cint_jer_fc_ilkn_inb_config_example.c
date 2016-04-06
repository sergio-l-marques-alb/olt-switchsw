/*
 * $Id: $
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
 * File: cint_jer_fc_ilkn_inb_config_example.c
 */
/*
 * Purpose: 
 *     Example of inband fc generation and reception on ILKN.
 *
 */

/*##############################################################*/
/*###################    INB GENERATION     ####################*/
/*##############################################################*/
/* set fc generation(global resource) and fc target(a tx slot on ILKN) */
int cint_jer_test_fc_ilkn_inb_gl_gen_set_example(int unit, int ilkn_port, int inb_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;

    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_set_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_inb_gl_gen_set_example\n", rv);
        return rv;
    }

    /* Configuring inb fc { */
    /* setting port */

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags = BCM_COSQ_FC_BDB;
            break;
        case global_vsq_src_mini_db: 
            flags = BCM_COSQ_FC_MINI_DB;
            break;
        case global_vsq_src_full_db: 
            flags = BCM_COSQ_FC_FULL_DB;
            break;
        case global_vsq_src_ocb_db: 
            flags = BCM_COSQ_FC_OCB_DB;
            break;
        default: 
            return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(fc_handler.port, ilkn_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("INB fc generation configuration failed(%d) in cint_jer_test_fc_inb_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring inb fc } */

    return rv;
}

int cint_jer_test_fc_ilkn_inb_gl_gen_unset_example(int unit, int ilkn_port, int inb_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger;
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    
    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_unset_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("Trigger global fc failed(%d) in cint_jer_test_fc_inb_gl_gen_unset_example\n", rv);
        return rv;
    }

    /* Configuring inb fc { */
    /* setting port */

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags = BCM_COSQ_FC_BDB;
            break;
        case global_vsq_src_mini_db: 
            flags = BCM_COSQ_FC_MINI_DB;
            break;
        case global_vsq_src_full_db: 
            flags = BCM_COSQ_FC_FULL_DB;
            break;
        case global_vsq_src_ocb_db: 
            flags = BCM_COSQ_FC_OCB_DB;
            break;
        default: 
            return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(fc_handler.port, ilkn_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags;
    fc_handler.flags = 0;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("INB fc generation configuration failed(%d) in cint_jer_test_fc_inb_gl_gen_unset_example\n", rv);
        return rv;
    }
    /* Configuring inb fc } */

    return rv;
}

/* set fc generation(global resource) and fc target(a tx slot on ILKN MUB) */
int cint_jer_test_fc_mub_gl_gen_set_example(int unit, int ilkn_port, int mub_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    
    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_set_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }
    
    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags = BCM_COSQ_FC_BDB;
            break;
        case global_vsq_src_mini_db: 
            flags = BCM_COSQ_FC_MINI_DB;
            break;
        case global_vsq_src_full_db: 
            flags = BCM_COSQ_FC_FULL_DB;
            break;
        case global_vsq_src_ocb_db: 
            flags = BCM_COSQ_FC_OCB_DB;
            break;
        default: 
            return BCM_E_PARAM;
            break;
    }

    /* Configuring mub fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_handler.port, ilkn_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags;
    fc_handler.flags = BCM_COSQ_FC_MUB;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = mub_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("MUB fc generation configuration failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring mub fc } */

    return rv;
}    

int cint_jer_test_fc_mub_gl_gen_unset_example(int unit, int ilkn_port, int mub_calendar_idx, int global_vsq_src, int priority, int shaped_port)
{
    int rv = BCM_E_NONE;
    int flags = 0;

    bcm_cosq_fc_endpoint_t fc_trigger; 
    bcm_cosq_fc_endpoint_t fc_handler;

    bcm_cosq_vsq_info_t vsq_inf;
    bcm_gport_t vsq_gport;
    
    /*Creating vsq*/
    vsq_inf.flags = BCM_COSQ_VSQ_GL;
    rv = bcm_cosq_gport_vsq_create(unit,&vsq_inf,&vsq_gport);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in bcm_cosq_gport_vsq_create\n", rv);
        return rv;
    }

    /* Trigger global fc : Setting FC threshold and Shaping local port*/
    rv = cint_jer_test_fc_gl_trigger_unset_example(unit, global_vsq_src, priority, shaped_port);
    if (rv != BCM_E_NONE) {
        printf("creating vsq failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }

    switch (global_vsq_src) {
        case global_vsq_src_bdb:
            flags = BCM_COSQ_FC_BDB;
            break;
        case global_vsq_src_mini_db: 
            flags = BCM_COSQ_FC_MINI_DB;
            break;
        case global_vsq_src_full_db: 
            flags = BCM_COSQ_FC_FULL_DB;
            break;
        case global_vsq_src_ocb_db: 
            flags = BCM_COSQ_FC_OCB_DB;
            break;
        default: 
            return BCM_E_PARAM;
            break;
    }

    /* Configuring mub fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_handler.port, ilkn_port);
    fc_trigger.port = vsq_gport;
    /* setting flags */    
    fc_trigger.flags = flags;
    fc_handler.flags = BCM_COSQ_FC_MUB;
    /* setting priority */    
    fc_trigger.priority = priority;
    /* setting cosq */ 
    fc_trigger.cosq = 0;
    fc_handler.cosq = -1; /* cosq can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_handler.calender_index = mub_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlGeneration, &fc_trigger, &fc_handler);
    if (rv != BCM_E_NONE) {
        printf("MUB fc generation configuration failed(%d) in cint_arad_test_fc_oob_vsq_gl_gen_set_example\n", rv);
        return rv;
    }
    /* Configuring mub fc } */

    return rv;
}    

/*##############################################################*/
/*##################     INB RECEPTION     #####################*/
/*##############################################################*/

/* set fc reception(an rx slot on ILKN) and fc target(a port traffic) */
int cint_jer_test_fc_ilkn_inb_port_rec_set_example(int unit, int target_port, int ilkn_port, int inb_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config inb fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("INB fc reception configuration failed(%d) in cint_jer_test_fc_inb_pfc_rec_set_example\n", rv);
    }
    /* Config inb fc } */

    return rv;
}

int cint_jer_test_fc_ilkn_inb_port_rec_unset_example(int unit, int target_port, int ilkn_port, int inb_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config inb fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("INB fc reception configuration failed(%d) in cint_jer_test_fc_inb_pfc_rec_unset_example\n", rv);
    }
    /* Config inb fc } */

    return rv;
}

/* set fc reception(an rx slot on ILKN MUB) and fc target(a port traffic) */
int cint_jer_test_fc_mub_port_rec_set_example(int unit, int target_port, int ilkn_port, int mub_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config mub fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_reception_port.flags = BCM_COSQ_FC_MUB;
    fc_target.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = mub_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("MUB fc reception configuration failed(%d) in cint_jer_test_fc_mub_port_rec_set_example\n", rv);
    }
    /* Config mub fc } */

    return rv;
}

int cint_jer_test_fc_mub_port_rec_unset_example(int unit,int target_port, int ilkn_port, int mub_calendar_idx)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config mub fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port,ilkn_port);
    /* setting flags */    
	  fc_reception_port.flags = BCM_COSQ_FC_MUB;
    fc_target.flags = 0;
    /* setting cosq */    
    fc_target.cosq = -1;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index=mub_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("MUB fc reception configuration failed in cint_jer_test_fc_mub_port_rec_unset_example\n");
    }
    /* Config mub fc } */
    
    return rv;
}

/* set fc reception(an rx slot on ILKN) and fc target(a port priority traffic) */
int cint_jer_test_fc_ilkn_inb_pfc_rec_set_example(int unit, int target_port, int ilkn_port, int inb_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config inb fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("INB fc reception configuration failed(%d) in cint_jer_test_fc_inb_pfc_rec_set_example\n", rv);
    }
    /* Config inb fc } */

    return rv;
}

int cint_jer_test_fc_ilkn_inb_pfc_rec_unset_example(int unit, int target_port, int ilkn_port, int inb_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config inb fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("INB fc reception configuration failed(%d) in cint_jer_test_fc_inb_pfc_rec_unset_example\n", rv);
    }
    /* Config inb fc } */

    return rv;
}

/* set fc reception(an rx slot on ILKN MUB) and fc target(a port priority traffic) */
int cint_jer_test_fc_mub_pfc_rec_set_example(int unit, int target_port, int ilkn_port, int mub_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config mub fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_reception_port.flags = BCM_COSQ_FC_MUB;
    fc_target.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = mub_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("MUB fc reception configuration failed(%d) in cint_jer_test_fc_mub_pfc_rec_set_example\n", rv);
    }
    /* Config mub fc } */

    return rv;
}

int cint_jer_test_fc_mub_pfc_rec_unset_example(int unit,int target_port, int ilkn_port, int mub_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config mub fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port,ilkn_port);
    /* setting flags */    
	  fc_reception_port.flags = BCM_COSQ_FC_MUB;
    fc_target.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index=mub_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("MUB fc reception configuration failed in cint_jer_test_fc_mub_pfc_rec_unset_example\n");
    }
    /* Config mub fc } */
    
    return rv;
}

/* set fc reception(an rx slot on ILKN) and fc target(traffic specified by PFC bitmap) */
int cint_jer_test_fc_ilkn_inb_pfc_bmp_rec_set_example(int unit, int target_port, int ilkn_port, int inb_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config inb fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_target.flags = BCM_COSQ_FC_PORT_OVER_PFC;
	  fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("INB fc reception configuration failed(%d) in cint_jer_test_fc_inb_pfc_rec_set_example\n", rv);
    }
    /* Config inb fc } */

    return rv;
}

int cint_jer_test_fc_ilkn_inb_pfc_bmp_rec_unset_example(int unit, int target_port, int ilkn_port, int inb_calendar_idx, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config inb fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    BCM_GPORT_LOCAL_SET(fc_reception_port.port, ilkn_port);
    /* setting flags */
	  fc_target.flags = BCM_COSQ_FC_PORT_OVER_PFC;
	  fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = inb_calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception_port, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("INB fc reception configuration failed(%d) in cint_jer_test_fc_inb_pfc_rec_unset_example\n", rv);
    }
    /* Config inb fc } */

    return rv;
}
