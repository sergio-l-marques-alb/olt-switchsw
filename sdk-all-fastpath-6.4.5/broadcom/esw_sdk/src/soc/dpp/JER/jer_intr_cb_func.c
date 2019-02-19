/*
 * $Id: jer_appl_intr_cb_func.c, v1 Broadcom SDK $
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
 * Purpose:    Implement CallBacks function for JER interrupts.
 */
 
/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/mcm/allenum.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/dpp/mbcm.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>

#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dpp/JER/jer_intr_corr_act_func.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_em_ser.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*************
 * FUNCTIONS *
 *************/


/*
 * Generic None handles - for CB without specific handling
 */
int jer_intr_handle_generic_none(int unit, int block_instance, jer_interrupt_type en_jer_interrupt,char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = jer_interrupt_print_info(unit, block_instance, en_jer_interrupt, 0, JER_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = jer_interrupt_handles_corrective_action(unit, block_instance, en_jer_interrupt, msg, JER_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_intr_recurring_action_handle_generic_none(int unit, int block_instance, jer_interrupt_type en_jer_interrupt, char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = jer_interrupt_print_info(unit, block_instance, en_jer_interrupt, 1, JER_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = jer_interrupt_handles_corrective_action(unit, block_instance, en_jer_interrupt, msg, JER_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_interrupt_handle_oamppendingevent(int unit, int block_instance, jer_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
 

    /* print info */
    rc = jer_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = jer_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_interrupt_handle_oamppendingstatevent(int unit, int block_instance, jer_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = jer_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = jer_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, JER_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
 int
 jer_hard_reset(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    char info_msg[240];
    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_HARD_RESET, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = jer_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, JER_INT_CORR_ACT_HARD_RESET , NULL,NULL);
    rc = soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET, SOC_DCMN_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);


exit:


    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_hard_reset_w_fabric(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {

    int rc = 0;
    char info_msg[240];
    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = jer_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, JER_INT_CORR_ACT_SOFT_RESET , NULL,NULL);

    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_none(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    char info_msg[240];

    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_NONE, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:


    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_reprogram_resource(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     int rc = 0;
     char info_msg[240];

    SOCDNX_INIT_FUNC_DEFS;

    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_REPROGRAM_RESOURCE, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_scrub_shadow_write(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     int rc = 0;
     jer_int_corr_act_type p_corrective_action;
     jer_interrupt_mem_err_info shadow_correct_info;
     char info_msg[240];
     SOCDNX_INIT_FUNC_DEFS;
     rc = jer_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
     SOCDNX_IF_ERR_EXIT(rc);
     rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
     SOCDNX_IF_ERR_EXIT(rc);
     rc = jer_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,NULL);
     SOCDNX_IF_ERR_EXIT(rc);
exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_shutdown_link(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     char info_msg[240];
     int rc = 0;
     SOCDNX_INIT_FUNC_DEFS;
     rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, info_msg);
     SOCDNX_IF_ERR_EXIT(rc);
     rc = jer_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS , NULL,info_msg);
     SOCDNX_IF_ERR_EXIT(rc);

exit:


    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    jer_int_corr_act_type p_corrective_action;
    jer_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];
    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = jer_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SOCDNX_IF_ERR_EXIT(rc);
exit:


    SOCDNX_FUNC_RETURN;

 }

void jer_interrupt_cb_init(int unit)
{
    dcmn_intr_add_handler(unit,                                 JER_INT_OAMP_PENDING_EVENT,     0,     0,                                 jer_interrupt_handle_oamppendingevent, NULL);
    dcmn_intr_add_handler(unit,                                 JER_INT_OAMP_STAT_PENDING_EVENT,0,     0,                                 jer_interrupt_handle_oamppendingstatevent, NULL);




    dcmn_intr_add_handler(unit, JER_INT_IPT_CRC_ERR_PKT, 100,1,jer_special_handling,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_IPT_CRC_DELETED_BUFFERS_FIFO_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_CRC_DELETED_BUFFERS_FIFO_NOT_EMPTY, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_CNM_DRAM_PKT_TOO_SMALL_ENCP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_CNM_PACKET_DELETED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_0, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_1, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_LARGE_LATENCY_PKT_DROP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_OCB_ONLY_DATA_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_OCB_MIX_0_DATA_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_OCB_MIX_1_DATA_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPS_DQCQ_DEPTH_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_EMPTY_DQCQ_WRITE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_QUEUE_ENTERED_DEL, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_CREDIT_LOST, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_CREDIT_OVERFLOW, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_DEQ_COMMAND_TIMEOUT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_DQCQ_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_PUSH_QUEUE_ACTIVE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_ENQ_BLOCK_OVERFLOW, 1000,1,jer_none,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPS_CR_FLW_ID_ERR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPS_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPS_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_SCH_SMP_FULL_LEVEL_1, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_SCH_SMP_FULL_LEVEL_2, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_SCH_REBOUND_FIFO_CR_LOSS, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_SCH_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_SCH_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_SCH_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 1,1000000,jer_shutdown_link,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_LOS_INT_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_LOS_INT_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_LOS_INT_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_LOS_INT_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_OOF_INT_0, 1,100,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_OOF_INT_1, 1,100,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_OOF_INT_2, 1,100,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_OOF_INT_3, 1,100,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 1,1000000,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 1,1000000,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 1,1000000,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 1,1000000,jer_special_handling,jer_none);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FMAC_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FSRD_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_0_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_0_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_1_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_1_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_0_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_0_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_1_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_1_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_0_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_0_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_1_A_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_1_B_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_TM_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_TM_INVLD_AD_ACC, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_TM_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_TM_OFFSET_WAS_FILT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_MRPS_MCDA_WRAP, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_MRPS_MCDB_WRAP, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_TX_FIFO_OVERFLOW_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_RX_FIFO_OVERFLOW_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_WRONG_WORD_FROM_PM_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_ERR_BITS_FROM_EGQ_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_WRONG_EGQ_WORD_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_ILLEGAL_PORT_MAP_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_EGQ_0_TRAFFIC_TO_INACTIVE_PORT_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_EGQ_1_TRAFFIC_TO_INACTIVE_PORT_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_TX_MISSING_SOB_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_TX_DOUBLE_SOB_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_RX_ELK_0_OVF_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_RX_ELK_1_OVF_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_SYNC_ETH_0_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_SYNC_ETH_1_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_0, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_1, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_2, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_3, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_4, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_5, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_6, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_7, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_2_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_3_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_NBIH_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_TX_FIFO_OVERFLOW_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_RX_FIFO_OVERFLOW_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_WRONG_WORD_FROM_PM_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_ILLEGAL_INPUT_FROM_GPORT_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_TX_MISSING_SOB_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_TX_DOUBLE_SOB_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_RX_ELK_0_OVF_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_RX_ELK_1_OVF_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_0, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_1, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_2, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_3, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_4, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_5, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_6, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_7, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_24, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_25, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_26, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_27, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_28, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_29, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_30, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_31, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_NBIL_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_INT_UCFIFO_AF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_VLAN_EMPTY_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_DROPPED_UC_PD_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_DROPPED_UC_DB_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_UC_PKT_PORT_FF, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_DELETE_FIFO_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_INVALID_OTM_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EGQ_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EPNI_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EPNI_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EPNI_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_CAM_TABLE_FULL, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_TABLE_COHERENCY, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_EDB_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_OAMP_RX_STATS_DONE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OAMP_RFC_6374_PKT_DROPPED, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OAMP_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_OAMP_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_OAMP_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDR_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDR_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDR_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_PRM_FDRA_UN_EXP_CELL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_A_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_A_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_FDRA_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_FDRA_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_FDRA_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_PRM_FDRB_UN_EXP_CELL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_B_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_B_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_FDRB_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_FDRB_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_1_P_1_FDRB_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRA_UN_EXP_CELL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_A_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_A_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRA_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRA_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRA_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRB_UN_EXP_CELL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_B_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_B_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRB_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRB_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_2_P_2_FDRB_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRA_UN_EXP_CELL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_A_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_A_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRA_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRA_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRA_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRB_UN_EXP_CELL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_B_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_B_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRB_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRB_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_3_P_3_FDRB_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_P_1_ECC_1B_ERR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_P_1_ECC_2B_ERR_INT, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_P_2_ECC_1B_ERR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_P_2_ECC_2B_ERR_INT, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_P_3_ECC_1B_ERR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_P_3_ECC_2B_ERR_INT, 0,0,jer_hard_reset_w_fabric,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_IN_BAND_FIFO_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDR_FDR_4_FDR_UN_EXP_ERROR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_PRIO_0_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_PRIO_1_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_PRIO_2_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_0_FAB_0_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_0_FAB_1_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_0_FAB_2_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_1_FAB_0_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_1_FAB_1_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_1_FAB_2_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_0_MESHMC_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_1_MESHMC_OVF_DROP_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_0_TDM_OVF_DROP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_EGQ_1_TDM_OVF_DROP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDA_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDA_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDA_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FCR_SRC_DV_CNG_LINK_EV, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_CPU_CNT_CELL_FNE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_LOCAL_ROUT_FS_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_LOCAL_ROUTRC_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_REACH_FIFO_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_FLOW_FIFO_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_CREDIT_FIFO_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCR_MAC_PARITY_DROP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCT_UNRCH_DEST_EVENT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FCT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FCT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FCT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_RTP_LINK_MASK_CHANGE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_RTP_TABLE_CHANGE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_RTP_DISCONNECT_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_RTP_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_RTP_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_RTP_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDT_UNRCH_DEST_EVT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDT_TDM_CRC_DROP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDT_ILEGAL_IRE_PACKET_SIZE_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDT_IN_BAND_LAST_READ_CNT_ZERO_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDT_BURST_TOO_LARGE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_FDT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_FDT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPST_CR_ARB_DROP_IPS_0_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPST_CR_ARB_DROP_IPS_1_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPST_CR_MSG_ERR_QNUM_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IPST_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IPST_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_0_LOCK_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_1_LOCK_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_0_OUT_OF_FRM, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_1_OUT_OF_FRM, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_0_DIP_2_ALARM, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_1_DIP_2_ALARM, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_0_FRM_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_1_FRM_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_0_DIP_2_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_1_DIP_2_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_0_WD_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SPI_OOB_RX_1_WD_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SCH_OOB_RX_CRC_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_SCH_OOB_RX_WD_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_HCFC_OOB_RX_0_HEADER_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_HCFC_OOB_RX_1_HEADER_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_HCFC_OOB_RX_0_CRC_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_HCFC_OOB_RX_1_CRC_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_CFC_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_CFC_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_CFC_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IQM_FREE_BDB_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_FREE_BDB_UNF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_DEQ_COM_EMPTY_Q, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_FULL_USCNT_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_MINI_USCNT_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_UPDT_FIFO_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_TXDSCR_FIFO_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_TXDSCR_BURST_FIFO_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_PD_FIFO_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_ISP_DUP_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_RST_USCNT_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_PKT_ENQ_RSRC_ERR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_PKT_ENQ_MC_ERR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_FREE_BDB_PROT_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_PKT_ENQ_QNVALID_ERR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_QROLL_OVER, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_VSQ_ROLL_OVER, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_DRAM_DYN_SIZE_ROLL_OVER, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_CNM_PKT_RJCT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_DEQ_STOP_PDM_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_ENQ_QNUM_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_ING_MC_2_ISP_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_DOUBLE_CD_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_FR_FLMC_DB_ROLL_OVER, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_FR_MNMC_DB_ROLL_OVER, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQM_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IQM_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IQM_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_0_ST_ING_ENQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_0_ST_ING_DEQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_EPNI_0_ST_EGR_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_0_ST_ENQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_0_ST_DEQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_1_ST_ING_ENQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_1_ST_ING_DEQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_EPNI_1_ST_EGR_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_1_ST_ENQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_1_ST_DEQ_RPT_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_0_SCRB_RPT_FF_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_IQM_1_SCRB_RPT_FF_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IQMT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_MMU_DRAM_OPP_CRC_ERR_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACA_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACA_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACA_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACA_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACB_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACB_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACB_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACB_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACC_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACC_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACC_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACC_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACD_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACD_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACD_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACD_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACE_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACE_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACE_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACE_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACF_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACF_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACF_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACF_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACG_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACG_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACG_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACG_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACH_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_BACH_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset);
    dcmn_intr_add_handler(unit, JER_INT_MMU_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_MMU_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_OCB_CRC_ERR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OCB_WR_DBUFF_POINTER_NOT_IN_ANY_RANGE_INT, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OCB_WR_DBUFF_POINTER_MATCHED_MORE_THAN_ONE_RANGE_INT, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OCB_RD_DBUFF_POINTER_NOT_IN_ANY_RANGE_INT, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OCB_RD_DBUFF_POINTER_MATCHED_MORE_THAN_ONE_RANGE_INT, 0,0,jer_reprogram_resource,NULL);
    dcmn_intr_add_handler(unit, JER_INT_OCB_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_OCB_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_OCB_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IHB_NIF_PHYSICAL_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_SEQ_RX_BIGER_SEQ_EXP_AND_SMALLER_SEQ_TX, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_SEQ_RX_SMALLER_SEQ_EXP_OR_BIGGER_EQ_SEQ_TX, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_FLP_FIFO_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_FLP_LOOKUP_TIMEOUT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_NIF_RX_FIFO_OVF, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_LB_VECTOR_INDEX_ILLEGAL_CALC_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_LB_ECMP_LAG_USE_SAME_HASH_INT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_FEC_ENTRY_ACCESSED_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_ILLEGAL_BYTES_TO_REMOVE_VALUE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_INVALID_DESTINATION_VALID, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_CPU_LOOKUP_REPLY_OVF, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_BAD_CHANNEL_NUM, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_BAD_LKP_TYPE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_BAD_LRN_TYPE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_NIF_TX_FIFO_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_RX_BROKEN_RECORD, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_CPU_INFO_REPLY_VALID, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_CPU_INFO_REPLY_ERR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_CPU_LOOKUP_REPLY_VALID, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_CPU_LOOKUP_REPLY_ERR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_ISEM_INTERRUPT_ONE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_KAPS_DIRECT_LOOKUP_ERR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHB_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IHB_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IHB_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IHP_MAX_HEADER_STACK_EXCEEDED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_VT_LIF_BANK_CONTENTION, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_TT_LIF_BANK_CONTENTION, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_MPLS_LABEL_TERMINATION_ERROR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_OAM_BFD_MISCONFIG_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_KAPS_DB_LOOKUP_ERR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IHP_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IHP_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IHP_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_TCAM_PROTECTION_ERROR, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_TCAM_QUERY_FAILURE_VALID, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_CAM_TABLE_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_CAM_TABLE_FULL, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_A_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_B_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_B_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_B_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IRE_PIPE_0_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRE_PIPE_1_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_DATA_ARRIVED, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_PACKET_SIZE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRE_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IRE_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IRE_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_0_RECYCLE, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_0_RECYCLE, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_OCB_0_RECYCLE, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_1_RECYCLE, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_1_RECYCLE, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_OCB_1_RECYCLE, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_0_EXTERNAL_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_0_EXTERNAL_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_1_EXTERNAL_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_1_EXTERNAL_OVERFLOW, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_RECEIVED_PIPE, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_ETHERNET_METER_PTR, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_REASSEMBLY_CONTEXT, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_SENT_PIPE_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ERROR_SENT_PIPE_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_MMU_ECC_1B_ERR_INT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_MMU_ECC_2B_ERR_INT, 0,0,jer_hard_reset,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IDR_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_DP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_PCB, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_SOP_PCB, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_DP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_EOP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_IDLE, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_DRAM_REJECT, 0,0,jer_none,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_MAX_REPLICATION_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_MAX_REPLICATION_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_IS_MAX_REPLICATION_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_IS_MAX_REPLICATION_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_REPLICATION_EMPTY_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_REPLICATION_EMPTY_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_IS_REPLICATION_EMPTY_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_IS_REPLICATION_EMPTY_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_FLOW_ID_IS_OVER_96K_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_FLOW_ID_IS_OVER_96K_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_QUEUE_IS_OVER_96K_0, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ERR_QUEUE_IS_OVER_96K_1, 0,0,jer_special_handling,NULL);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling);
    dcmn_intr_add_handler(unit, JER_INT_IRR_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling,jer_special_handling);

}

#undef _ERR_MSG_MODULE_NAME
