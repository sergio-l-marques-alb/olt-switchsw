/*
 * $Id: jer_appl_intr_cb_func.c, v1 Broadcom SDK $
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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <bcm_int/dpp/l2.h>

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

    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_NONE, msg);
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
int jer_event_ready_handler(int unit, 
                            int block_instance, 
                            uint32 en_interrupt,
                            char* msg)
{
    int rc;
    soc_reg_above_64_val_t pr_reg;
    uint32 fld_val;
    uint64 reg64_val;
    int check_for_more_events;

    SOCDNX_INIT_FUNC_DEFS;

    /* Read and handle all event-fifo contents */
    while(1)
    {
        check_for_more_events = 0;

        /* Verify an event is present in the Event FIFO via its interrupt */
        rc = soc_reg64_get(unit, PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr, (REG_PORT_ANY), 0, &reg64_val);
        SOCDNX_IF_ERR_EXIT(rc);

        /* Check and handle the master pipe and then handle the slave pipe */
        fld_val = soc_reg64_field32_get(unit, PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr, reg64_val, LARGE_EM_MASTER_EVENT_READYf);
        if (fld_val != 0x0)
        {
            /* Handle master pipe */
            rc = soc_reg_above_64_get(unit, PPDB_B_LARGE_EM_MASTER_EVENTr, block_instance, 0, pr_reg);
            SOCDNX_IF_ERR_EXIT(rc);

            /* Process event-FIFO record */
            arad_pp_frwrd_mact_handle_learn_event(unit, (uint32*)pr_reg);
            check_for_more_events = 1;
        }

        /* Handle slave pipe */
        fld_val = soc_reg64_field32_get(unit, PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr, reg64_val, LARGE_EM_SLAVE_EVENT_READYf);
        if (fld_val != 0x0)
        {
            /* Data collection */
            rc = soc_reg_above_64_get(unit, PPDB_B_LARGE_EM_SLAVE_EVENTr, block_instance, 0, pr_reg);
            SOCDNX_IF_ERR_EXIT(rc);

            /* Process event-FIFO record */
            arad_pp_frwrd_mact_handle_learn_event(unit, (uint32*)pr_reg);
            check_for_more_events = 1;
        }

        if (!check_for_more_events) {
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
} 




STATIC
 int
 jer_special_handling_normal_0(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:Check Configuration */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_1(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt according to the memory severity either init device or core fix line ignore or perform action that applies for the specific memory  */

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
STATIC
 int
 jer_special_handling_recur_2(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* recur:identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt according to the memory severity either init device or core fix line ignore or perform action that applies for the specific memory  */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_recur_3(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* recur:if memory was not cached ignore  */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_recur_4(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* recur:Check Configuration */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_5(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:Tune SerDes If not resolved shutdown link  */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_6(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:if pcs mode is 9 10 or pcs mode is 64 66 KR shutdown link else ignore  */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_7(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:If pcs mode is KR read int_info_csrs_prop1 registers set int_info_csrs_prp2 registers to count errors on the relevant lane Read error counter to get exact rate and shutdown link when threshold is reached  */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_8(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:Handling depands on PCS mode 8 10 Shutdown link else None */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_9(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:Driver should hold shadow of Link Active Mask register in order to understand what was changed If link goes up then this is not an error If link goes down If MAC link status is down then this is the problem else if local ACL is down then there is a problem with local Rx MAC else if remote ACL is down then there is an problem with remote Rx MAC else check if remote device was isolated else review local RTPWD and remote RMGR settings */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_10(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:fix configuration and reset to IPP Interrupt handler log a message  */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_normal_11(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* normal:Check the parity error address if it is in the range of the WDF memory perform ingress soft reset Otherwise if memory was cached perform scrub_shadow_write */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jer_special_handling_recur_12(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    /* recur:Check the parity error address if it is in the range of the WDF memory perform ingress soft reset Otherwise if memory was cached perform scrub_shadow_write */

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = jer_interrupt_print_info(unit, block_instance, en_interrupt, 0, JER_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }


void jer_interrupt_cb_init(int unit)
{

    /* this part of the code is autogenerated please do not edit, use the part after to add interrupt registration*/
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_CRC_ERR_PKT, 100,1,jer_special_handling_normal_0,jer_hard_reset,"the sw process should clear the FIFO and consider each buffer in it whether it may be returned to the free buffers pool.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_CRC_DELETED_BUFFERS_FIFO_FULL, 0,0,jer_special_handling_normal_0,NULL,"the sw process should clear the FIFO and consider each buffer in it whether it may be returned to the free buffers pool.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_CRC_DELETED_BUFFERS_FIFO_NOT_EMPTY, 0,0,jer_special_handling_normal_0,NULL,"the sw process should clear the FIFO and consider each buffer in it whether it may be returned to the free buffers pool.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_CNM_DRAM_PKT_TOO_SMALL_ENCP, 0,0,jer_none,NULL,"JER_INT_IPT_CNM_DRAM_PKT_TOO_SMALL_ENCP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_CNM_PACKET_DELETED, 0,0,jer_none,NULL,"JER_INT_IPT_CNM_PACKET_DELETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_0, 0,0,jer_none,NULL,"JER_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_1, 0,0,jer_none,NULL,"JER_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_LARGE_LATENCY_PKT_DROP, 0,0,jer_none,NULL,"JER_INT_IPT_LARGE_LATENCY_PKT_DROP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_OCB_ONLY_DATA_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"bad configuration, should re-tune the threshold to stop (prophet counter)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_OCB_MIX_0_DATA_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"bad configuration, should re-tune the threshold to stop (prophet counter)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_OCB_MIX_1_DATA_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"bad configuration, should re-tune the threshold to stop (prophet counter)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_DQCQ_DEPTH_OVF, 0,0,jer_special_handling_normal_0,NULL,"fix the configuration in the faulted dqcq depth registers and resume traffic.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_EMPTY_DQCQ_WRITE, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_QUEUE_ENTERED_DEL, 0,0,jer_none,NULL,"Monitor queue number (0x0149)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_CREDIT_LOST, 0,0,jer_none,NULL,"JER_INT_IPS_CREDIT_LOST");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_CREDIT_OVERFLOW, 0,0,jer_none,NULL,"JER_INT_IPS_CREDIT_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_DEQ_COMMAND_TIMEOUT, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_DQCQ_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_PUSH_QUEUE_ACTIVE, 0,0,jer_none,NULL,"JER_INT_IPS_PUSH_QUEUE_ACTIVE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_ENQ_BLOCK_OVERFLOW, 1000,1,jer_none,jer_special_handling_recur_4,"JER_INT_IPS_ENQ_BLOCK_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_CR_FLW_ID_ERR_INT, 0,0,jer_none,NULL,"JER_INT_IPS_CR_FLW_ID_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPS_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_SCH_SMP_FULL_LEVEL_1, 0,0,jer_reprogram_resource,NULL,"Fix the IPS configuration.");
    dcmn_intr_add_handler_ext(unit, JER_INT_SCH_SMP_FULL_LEVEL_2, 0,0,jer_reprogram_resource,NULL,"Fix the IPS configuration.");
    dcmn_intr_add_handler_ext(unit, JER_INT_SCH_REBOUND_FIFO_CR_LOSS, 0,0,jer_reprogram_resource,NULL,"JER_INT_SCH_REBOUND_FIFO_CR_LOSS");
    dcmn_intr_add_handler_ext(unit, JER_INT_SCH_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_SCH_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_SCH_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 1,1000000,jer_shutdown_link,jer_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 1,1000000,jer_shutdown_link,jer_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 1,1000000,jer_shutdown_link,jer_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 1,1000000,jer_shutdown_link,jer_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 1,1000000,jer_shutdown_link,jer_none,"JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 1,1000000,jer_shutdown_link,jer_none,"JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 1,1000000,jer_shutdown_link,jer_none,"JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 1,1000000,jer_shutdown_link,jer_none,"JER_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_LOS_INT_0, 0,0,jer_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_LOS_INT_1, 0,0,jer_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_LOS_INT_2, 0,0,jer_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_LOS_INT_3, 0,0,jer_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_OOF_INT_0, 1,100,jer_special_handling_normal_6,jer_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_OOF_INT_1, 1,100,jer_special_handling_normal_6,jer_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_OOF_INT_2, 1,100,jer_special_handling_normal_6,jer_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_OOF_INT_3, 1,100,jer_special_handling_normal_6,jer_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 1,1000000,jer_special_handling_normal_7,jer_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 1,1000000,jer_special_handling_normal_7,jer_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 1,1000000,jer_special_handling_normal_7,jer_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 1,1000000,jer_special_handling_normal_7,jer_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0, 0,0,jer_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1, 0,0,jer_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2, 0,0,jer_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3, 0,0,jer_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_0, 0,0,jer_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_1, 0,0,jer_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_2, 0,0,jer_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_3, 0,0,jer_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0, 0,0,jer_hard_reset,NULL,"JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1, 0,0,jer_hard_reset,NULL,"JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2, 0,0,jer_hard_reset,NULL,"JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3, 0,0,jer_hard_reset,NULL,"JER_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FMAC_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jer_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FSRD_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_0_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI0 configuration for command EPNI0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_0_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI0 configuration for command EPNI0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_1_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI1 configuration for command EPNI1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EPNI_1_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI1 configuration for command EPNI1-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ0 configuration for command EGQ0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ0 configuration for command EGQ0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ1 configuration for command EGQ1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ1 configuration for command EGQ1-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_0_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP0 configuration for command IRPP0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_0_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP0 configuration for command IRPP0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_1_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP1 configuration for command IRPP1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IRPP_1_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP1 configuration for command IRPP1-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_0_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IQM0 configuration for command IQM0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_0_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IQM0 configuration for command IQM0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_1_A_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IQM1 configuration for command IQM1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_IQM_1_B_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check IQM1 configuration for command IQM1-B. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_TM_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ0TM configuration for command EGQ0TM.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_TM_INVLD_AD_ACC, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ1TM configuration for command EGQ1TM. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI0  DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EPNI1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IRPP1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_0_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IQM0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_0_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IQM0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_1_A_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IQM1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_1_B_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check IQM1 DoNotCountOffset configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_TM_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ0TM DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_TM_OFFSET_WAS_FILT, 0,0,jer_special_handling_normal_0,NULL,"Check EGQ1TM DoNotCountOffset configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_MRPS_MCDA_WRAP, 0,0,jer_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, JER_INT_MRPS_MCDB_WRAP, 0,0,jer_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_TX_FIFO_OVERFLOW_INT, 0,0,jer_special_handling_normal_0,NULL,"This interrupt means that the EGQ configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF Check also HrfTx<n>NumCreditsToEgq.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_RX_FIFO_OVERFLOW_INT, 0,0,jer_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_WRONG_WORD_FROM_PM_INT, 0,0,jer_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_ERR_BITS_FROM_EGQ_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_ERR_BITS_FROM_EGQ_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_WRONG_EGQ_WORD_INT, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_NBIH_WRONG_EGQ_WORD_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_ILLEGAL_PORT_MAP_INT, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_NBIH_ILLEGAL_PORT_MAP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_EGQ_0_TRAFFIC_TO_INACTIVE_PORT_INT, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_NBIH_EGQ_0_TRAFFIC_TO_INACTIVE_PORT_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_EGQ_1_TRAFFIC_TO_INACTIVE_PORT_INT, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_NBIH_EGQ_1_TRAFFIC_TO_INACTIVE_PORT_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_TX_MISSING_SOB_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_TX_MISSING_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_TX_DOUBLE_SOB_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_TX_DOUBLE_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_RX_ELK_0_OVF_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_RX_ELK_0_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_RX_ELK_1_OVF_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_RX_ELK_1_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_SYNC_ETH_0_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_SYNC_ETH_0_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_SYNC_ETH_1_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_SYNC_ETH_1_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_0, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_1, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_2, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_2");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_3, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_3");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_4, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_4");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_5, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_5");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_6, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_6");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_7, 0,0,jer_none,NULL,"JER_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_7");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23, 0,0,jer_none,NULL,"JER_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_2_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_2_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_3_INT, 0,0,jer_none,NULL,"JER_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_3_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIH_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_TX_FIFO_OVERFLOW_INT, 0,0,jer_special_handling_normal_0,NULL,"This interrupt means that the EGQ configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF Check also HrfTx<n>NumCreditsToEgq.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_RX_FIFO_OVERFLOW_INT, 0,0,jer_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_WRONG_WORD_FROM_PM_INT, 0,0,jer_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_ILLEGAL_INPUT_FROM_GPORT_INT, 0,0,jer_none,NULL,"JER_INT_NBIL_ILLEGAL_INPUT_FROM_GPORT_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_TX_MISSING_SOB_INT, 0,0,jer_none,NULL,"JER_INT_NBIL_TX_MISSING_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_TX_DOUBLE_SOB_INT, 0,0,jer_none,NULL,"JER_INT_NBIL_TX_DOUBLE_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_RX_ELK_0_OVF_INT, 0,0,jer_none,NULL,"JER_INT_NBIL_RX_ELK_0_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_RX_ELK_1_OVF_INT, 0,0,jer_none,NULL,"JER_INT_NBIL_RX_ELK_1_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT, 0,0,jer_none,NULL,"JER_INT_NBIL_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_0, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_1, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_2, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_2");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_3, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_3");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_4, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_4");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_5, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_5");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_6, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_6");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_7, 0,0,jer_none,NULL,"JER_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_7");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_24, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_24");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_25, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_25");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_26, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_26");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_27, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_27");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_28, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_28");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_29, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_29");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_30, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_30");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_31, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_31");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_32, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_32");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_33, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_33");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_34, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_34");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_35, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_35");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_36, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_36");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_37, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_37");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_38, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_38");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_39, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_39");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_40, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_40");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_41, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_41");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_42, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_42");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_43, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_43");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_44, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_44");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_45, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_45");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_46, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_46");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_47, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_47");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_48, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_48");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_49, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_49");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_50, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_50");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_51, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_51");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_52, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_52");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_53, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_53");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_54, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_54");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_55, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_55");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_56, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_56");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_57, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_57");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_58, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_58");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_59, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_59");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_60, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_60");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_61, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_61");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_62, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_62");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_63, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_63");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_64, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_64");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_65, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_65");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_66, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_66");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_67, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_67");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_68, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_68");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_69, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_69");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_70, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_70");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_71, 0,0,jer_none,NULL,"JER_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_71");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_NBIL_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_INT_UCFIFO_AF, 0,0,jer_none,NULL,"JER_INT_EGQ_INT_UCFIFO_AF");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_VLAN_EMPTY_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_VLAN_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_DROPPED_UC_PD_INT, 0,0,jer_special_handling_normal_0,NULL,"Check tresholds of of packet descriptors per Port / TC / Queue / Interface or Total. The CGM registers Unicast Packet Descriptors Drop Counter, Congestion Tracking Unicast PDMax Value and Unicast Packet Descriptors Counter should be read to analaze the status.    Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_DROPPED_UC_DB_INT, 0,0,jer_special_handling_normal_0,NULL,"Check tresholds of of data buffers per Port / TC / QUE / Interface or Total.The CGM registers Unicast Data Buffers Drop Counter (RQP / PQP), Congestion Tracking Unicast DBMax Value and Unicast Data Buffers Counter should be read to analaze the status.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_UC_PKT_PORT_FF, 0,0,jer_reprogram_resource,NULL,"JER_INT_EGQ_UC_PKT_PORT_FF");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_DELETE_FIFO_FULL, 0,0,jer_special_handling_normal_0,NULL,"Check that the configuration of the EGQ does not result in too many discards. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_INVALID_OTM_INT, 0,0,jer_special_handling_normal_0,NULL,"Verify the ingress configuration of the device the packet was recived from.   Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT, 0,0,jer_none,NULL,"JER_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_PKT_SIZE_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_FRAG_NUM_ZERO_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_PCP_LOC_NOT_IN_CELL_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_NO_EOP_ON_EOC_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_FDR_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_NON_PCP_SRC_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_SINGLE_CELL_SRC_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_SOP_CELL_SIZE_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CDC_SOP_CELL_OVERSIZE_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CUP_MISSING_SOP_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CUP_MISSING_EOP_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CUP_MISSING_CELL_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CUP_TDM_MISMATCH_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CUP_PKT_SIZE_MISMATCH_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CUP_MOP_CELL_SIZE_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_CRC_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_RCM_FAP_ID_NOT_VALID, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_RCM_ALL_CONTEXTS_TAKEN_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_RCM_ALL_CONTEXTS_TAKEN_DISCARD_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_RCM_DYNAMIC_MISSING_SOP_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_PACKET_REASSEMBLY_RCM_STATIC_MISS_CONFIG_ERR, 1000,1,jer_none,jer_special_handling_recur_4,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EGQ_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EPNI_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EPNI_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EPNI_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_CAM_TABLE_FULL, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_TABLE_COHERENCY, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL,"JER_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL,"JER_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_EDB_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_PENDING_EVENT, 0,0,jer_special_handling_normal_0,NULL,"Pop message from event fifo by  reading  from register InterruptMessage (address 0x0140) till read data is 0xFFFFFF.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_STAT_PENDING_EVENT, 0,0,jer_special_handling_normal_0,NULL,"Pop message from stat fifo by  reading  from register InterruptMessage (address 0x0140) till read data is 0xFFFFFF.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_RX_STATS_DONE, 0,0,jer_special_handling_normal_0,NULL,"read SAT statistcics entry from the registers");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_RFC_6374_PKT_DROPPED, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_OAMP_RFC_6374_PKT_DROPPED");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling_normal_0,NULL,"Act according to the fail source specified in 0x0265, RmapemManagementUnitFailure");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL,"JER_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OAMP_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_PRM_FDRA_UN_EXP_CELL, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_A_0, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_A_1, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_FDRA_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_1_P_1_FDRA_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_FDRA_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_1_P_1_FDRA_FILRTER_DROPP_INT_A");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_FDRA_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_1_P_1_FDRA_FILRTER_DROPP_INT_B");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_PRM_FDRB_UN_EXP_CELL, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_B_0, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_B_1, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_FDRB_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_1_P_1_FDRB_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_FDRB_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_1_P_1_FDRB_FILRTER_DROPP_INT_A");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_1_P_1_FDRB_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_1_P_1_FDRB_FILRTER_DROPP_INT_B");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRA_UN_EXP_CELL, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_A_0, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_A_1, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRA_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_2_P_2_FDRA_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRA_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_2_P_2_FDRA_FILRTER_DROPP_INT_A");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRA_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_2_P_2_FDRA_FILRTER_DROPP_INT_B");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRB_UN_EXP_CELL, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_B_0, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_B_1, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRB_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_2_P_2_FDRB_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRB_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_2_P_2_FDRB_FILRTER_DROPP_INT_A");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_2_P_2_FDRB_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_2_P_2_FDRB_FILRTER_DROPP_INT_B");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRA_UN_EXP_CELL, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_A_0, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_A_1, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRA_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_3_P_3_FDRA_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRA_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_3_P_3_FDRA_FILRTER_DROPP_INT_A");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRA_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_3_P_3_FDRA_FILRTER_DROPP_INT_B");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRB_UN_EXP_CELL, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_B_0, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_B_1, 0,0,jer_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRB_OUT_OF_SYNC, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_3_P_3_FDRB_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRB_FILRTER_DROPP_INT_A, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_3_P_3_FDRB_FILRTER_DROPP_INT_A");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_3_P_3_FDRB_FILRTER_DROPP_INT_B, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_3_P_3_FDRB_FILRTER_DROPP_INT_B");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_P_1_ECC_1B_ERR_INT, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_4_P_1_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_P_1_ECC_2B_ERR_INT, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_4_P_1_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_P_2_ECC_1B_ERR_INT, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_4_P_2_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_P_2_ECC_2B_ERR_INT, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_4_P_2_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_P_3_ECC_1B_ERR_INT, 0,0,jer_none,NULL,"JER_INT_FDR_FDR_4_P_3_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_P_3_ECC_2B_ERR_INT, 0,0,jer_hard_reset_w_fabric,NULL,"JER_INT_FDR_FDR_4_P_3_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_IN_BAND_FIFO_FULL, 0,0,jer_special_handling_normal_0,NULL,"Reduce FDT inband shaper rate.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDR_FDR_4_FDR_UN_EXP_ERROR, 0,0,jer_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiguration in the system, need to collect dumps of all devices in system and analyze them.    Interrupt handler logs a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_PRIO_0_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_PRIO_0_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_PRIO_1_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_PRIO_1_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_PRIO_2_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_PRIO_2_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_0_FAB_0_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_0_FAB_0_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_0_FAB_1_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_0_FAB_1_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_0_FAB_2_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_0_FAB_2_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_1_FAB_0_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_1_FAB_0_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_1_FAB_1_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_1_FAB_1_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_1_FAB_2_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_1_FAB_2_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_0_MESHMC_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_0_MESHMC_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_1_MESHMC_OVF_DROP_INT, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_1_MESHMC_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_0_TDM_OVF_DROP, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_0_TDM_OVF_DROP");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_EGQ_1_TDM_OVF_DROP, 0,0,jer_none,NULL,"JER_INT_FDA_EGQ_1_TDM_OVF_DROP");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDA_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_SRC_DV_CNG_LINK_EV, 0,0,jer_none,NULL,"JER_INT_FCR_SRC_DV_CNG_LINK_EV");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_CPU_CNT_CELL_FNE, 0,0,jer_special_handling_normal_0,NULL,"Read captured cell(s) from register ControlCellFIFOBuffer");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_LOCAL_ROUT_FS_OVF, 0,0,jer_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_LOCAL_ROUTRC_OVF, 0,0,jer_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_REACH_FIFO_OVF, 0,0,jer_special_handling_normal_0,NULL,"Check rechability message Generation Rate (RMGR) configuration of link partner. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_FLOW_FIFO_OVF, 0,0,jer_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_CREDIT_FIFO_OVF, 0,0,jer_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCR_MAC_PARITY_DROP, 0,0,jer_none,NULL,"Nothing, this should not happen (unconnected in the design)");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCT_UNRCH_DEST_EVENT, 0,0,jer_none,NULL,"There might be a problem with the local or remote reachbility tables.  Remote device might be down.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FCT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_RTP_LINK_MASK_CHANGE, 0,0,jer_special_handling_normal_9,NULL,"Driver should hold shadow of Link Active Mask register in order to understand what was changed. If link goes up then this is not an error.  If link goes down:  If MAC link status is down then this is the problem else, if local ACL is down then there is a problem with local Rx MAC else, if remote ACL is down then there is an problem with remote Rx MAC else, check if remote device was isolated else, review local RTPWD and remote RMGR settings");
    dcmn_intr_add_handler_ext(unit, JER_INT_RTP_TABLE_CHANGE, 0,0,jer_special_handling_normal_0,NULL,"Check fabric rechability tables.");
    dcmn_intr_add_handler_ext(unit, JER_INT_RTP_DISCONNECT_INT, 0,0,jer_none,NULL,"JER_INT_RTP_DISCONNECT_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_RTP_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_RTP_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_RTP_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_UNRCH_DEST_EVT, 0,0,jer_none,NULL,"Problem in rtp topology table or multicast bitmap.  Link / destination device might be down.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_TDM_CRC_DROP, 0,0,jer_none,NULL,"JER_INT_FDT_TDM_CRC_DROP");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_ILEGAL_IRE_PACKET_SIZE_INT, 0,0,jer_special_handling_normal_0,NULL,"Reprogram IRE filter (cell size should be 65B-256B).  Interrupt handler logs a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_IN_BAND_LAST_READ_CNT_ZERO_INT, 0,0,jer_none,NULL,"JER_INT_FDT_IN_BAND_LAST_READ_CNT_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_BURST_TOO_LARGE, 0,0,jer_special_handling_normal_0,NULL,"Reprogram IQM burst size settings.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_FDT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPST_CR_ARB_DROP_IPS_0_INT, 0,0,jer_none,NULL,"JER_INT_IPST_CR_ARB_DROP_IPS_0_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPST_CR_ARB_DROP_IPS_1_INT, 0,0,jer_none,NULL,"JER_INT_IPST_CR_ARB_DROP_IPS_1_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPST_CR_MSG_ERR_QNUM_INT, 0,0,jer_none,NULL,"JER_INT_IPST_CR_MSG_ERR_QNUM_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPST_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IPST_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_0_LOCK_ERR, 0,0,jer_special_handling_normal_0,NULL,"the interface did not lock, should verify that calendar length and number of multiplications in a frame match on both devices. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_1_LOCK_ERR, 0,0,jer_special_handling_normal_0,NULL,"the interface did not lock, should verify that calendar length and number of multiplications in a frame match on both devices. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_0_OUT_OF_FRM, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_1_OUT_OF_FRM, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_0_DIP_2_ALARM, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_1_DIP_2_ALARM, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Interrupt handler  log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_0_FRM_ERR, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_1_FRM_ERR, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_0_DIP_2_ERR, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_1_DIP_2_ERR, 0,0,jer_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. . Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_0_WD_ERR, 0,0,jer_special_handling_normal_0,NULL,"the spi watch dog expired, and indicates that the interface is not active (down). Should verify the wd period config and also see if the board maybe noisy or defected. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SPI_OOB_RX_1_WD_ERR, 0,0,jer_special_handling_normal_0,NULL,"the spi watch dog expired, and indicates that the interface is not active (down). Should verify the wd period config and also see if the board maybe noisy or defected. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SCH_OOB_RX_CRC_ERR, 0,0,jer_special_handling_normal_0,NULL,"the sch oob interface received a crc error. Should check the other device and the interface itself. Not relevant if the SCH-Oob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_SCH_OOB_RX_WD_ERR, 0,0,jer_special_handling_normal_0,NULL,"the sch oob interface did not receive a correct crc for the duration of the watch dog period. Should check the other device and the interface. Not relevant if the SCH-Oob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_HCFC_OOB_RX_0_HEADER_ERR, 0,0,jer_special_handling_normal_0,NULL,"the hcfc oob interface received a msg type or fwd field that does not match the configuration. Should check the config and the other device config. Not relevant if the Hcfc-Oob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_HCFC_OOB_RX_1_HEADER_ERR, 0,0,jer_special_handling_normal_0,NULL,"the hcfc oob interface received a msg type or fwd field that does not match the configuration. Should check the config and the other device config. Not relevant if the Hcfc-Oob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_HCFC_OOB_RX_0_CRC_ERR, 0,0,jer_special_handling_normal_0,NULL,"the hcfc oob interface received a crc error in one of the msgs. Should check the other device and the interface itself. Not relevant if the Hcfc-Oob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_HCFC_OOB_RX_1_CRC_ERR, 0,0,jer_special_handling_normal_0,NULL,"the hcfc oob interface received a crc error in one of the msgs. Should check the other device and the interface itself. Not relevant if the Hcfc-Oob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_CFC_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_FREE_BDB_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_FREE_BDB_UNF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_DEQ_COM_EMPTY_Q, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_FULL_USCNT_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init , check mc table in irr");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_MINI_USCNT_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init , check other table in irr");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_UPDT_FIFO_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_TXDSCR_FIFO_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_TXDSCR_BURST_FIFO_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_PD_FIFO_OVF, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_ISP_DUP_ERR, 0,0,jer_special_handling_normal_0,NULL,"check configurations in ingress");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_RST_USCNT_ERR, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_PKT_ENQ_RSRC_ERR, 0,0,jer_none,NULL,"JER_INT_IQM_PKT_ENQ_RSRC_ERR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_PKT_ENQ_MC_ERR, 0,0,jer_none,NULL,"JER_INT_IQM_PKT_ENQ_MC_ERR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_FREE_BDB_PROT_ERR, 0,0,jer_special_handling_normal_0,NULL,"check counter (0x202) , chcek the free bdb discard threshold, ingress soft init");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_PKT_ENQ_QNVALID_ERR, 0,0,jer_none,NULL,"JER_INT_IQM_PKT_ENQ_QNVALID_ERR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_QROLL_OVER, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init check soft error indication on bdb ll and  pdm tables");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_VSQ_ROLL_OVER, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init check soft error indication on bdb ll and  pdm tables");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_DRAM_DYN_SIZE_ROLL_OVER, 0,0,jer_special_handling_normal_0,NULL,"check guarnteed bytes config check discard over dynamic size config");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_CNM_PKT_RJCT, 0,0,jer_special_handling_normal_0,NULL,"check CNM shaper and CNM thresholds");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_DEQ_STOP_PDM_ERR, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_ENQ_QNUM_OVF, 0,0,jer_special_handling_normal_0,NULL,"check other module condifuration (irr/ ihp)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_ING_MC_2_ISP_ERR, 0,0,jer_special_handling_normal_0,NULL,"check other module condifuration (irr mc table, isp queues )");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_DOUBLE_CD_ERR, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_FR_FLMC_DB_ROLL_OVER, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init, check buffer configurations, check soft errors in other modules (all the dbuff number path)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_FR_MNMC_DB_ROLL_OVER, 0,0,jer_special_handling_normal_0,NULL,"ingress soft init, check buffer configurations, check soft errors in other modules (all the dbuff number path)");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQM_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_0_ST_ING_ENQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_0_ST_ING_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_0_ST_ING_DEQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_0_ST_ING_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_EPNI_0_ST_EGR_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_EPNI_0_ST_EGR_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_0_ST_ENQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_0_ST_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_0_ST_DEQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_0_ST_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_1_ST_ING_ENQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_1_ST_ING_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_1_ST_ING_DEQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_1_ST_ING_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_EPNI_1_ST_EGR_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_EPNI_1_ST_EGR_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_1_ST_ENQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_1_ST_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_1_ST_DEQ_RPT_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_1_ST_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_0_SCRB_RPT_FF_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_0_SCRB_RPT_FF_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_IQM_1_SCRB_RPT_FF_OVF, 0,0,jer_none,NULL,"JER_INT_IQMT_IQM_1_SCRB_RPT_FF_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IQMT_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_DRAM_OPP_CRC_ERR_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_DRAM_OPP_CRC_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACA_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACA_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACA_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACA_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACA_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACA_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACA_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACA_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACB_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACB_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACB_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACB_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACB_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACB_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACB_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACB_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACC_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACC_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACC_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACC_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACC_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACC_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACC_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACC_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACD_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACD_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACD_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACD_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACD_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACD_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACD_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACD_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACE_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACE_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACE_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACE_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACE_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACE_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACE_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACE_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACF_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACF_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACF_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACF_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACF_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACF_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACF_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACF_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACG_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACG_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACG_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACG_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACG_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACG_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACG_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACG_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACH_BG_0_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACH_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACH_BG_1_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACH_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACH_BG_2_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACH_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACH_BG_3_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACH_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACA_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACA_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACB_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACB_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACC_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACC_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACD_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACD_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACE_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACE_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACF_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACF_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACG_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACG_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_BACH_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jer_none,jer_hard_reset,"JER_INT_MMU_BACH_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_MMU_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_CRC_ERR_INT, 0,0,jer_none,NULL,"JER_INT_OCB_CRC_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_WR_DBUFF_POINTER_NOT_IN_ANY_RANGE_INT, 0,0,jer_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_WR_DBUFF_POINTER_MATCHED_MORE_THAN_ONE_RANGE_INT, 0,0,jer_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_RD_DBUFF_POINTER_NOT_IN_ANY_RANGE_INT, 0,0,jer_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_RD_DBUFF_POINTER_MATCHED_MORE_THAN_ONE_RANGE_INT, 0,0,jer_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_OCB_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_NIF_PHYSICAL_ERR, 0,0,jer_special_handling_normal_0,NULL,"need to check H/W. might be issue with ELK connection.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_SEQ_RX_BIGER_SEQ_EXP_AND_SMALLER_SEQ_TX, 0,0,jer_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, means packet loss in ELK and should be treated in ELK.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_SEQ_RX_SMALLER_SEQ_EXP_OR_BIGGER_EQ_SEQ_TX, 0,0,jer_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, means packet loss in ELK and should be treated in ELK. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_FLP_FIFO_FULL, 0,0,jer_special_handling_normal_0,NULL,"this means that ELK link rate can not sustain FLP requests rate. Can be configuration error, or system setup error.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_FLP_LOOKUP_TIMEOUT, 0,0,jer_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, issue with ELK. Could be too much info replies generated by ELK Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_NIF_RX_FIFO_OVF, 0,0,jer_special_handling_normal_0,NULL,"worst case: ELK alignment is wrong, meaning NOP are inserted in packet start. Could be too much info replies generated by ELK. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_LB_VECTOR_INDEX_ILLEGAL_CALC_INT, 0,0,jer_special_handling_normal_0,NULL,"fix LB configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_LB_ECMP_LAG_USE_SAME_HASH_INT, 0,0,jer_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_FEC_ENTRY_ACCESSED_INT, 0,0,jer_none,NULL,"JER_INT_IHB_FEC_ENTRY_ACCESSED_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_ILLEGAL_BYTES_TO_REMOVE_VALUE, 0,0,jer_special_handling_normal_0,NULL,"fix configuration. ");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_INVALID_DESTINATION_VALID, 0,0,jer_none,NULL,"JER_INT_IHB_INVALID_DESTINATION_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_CPU_LOOKUP_REPLY_OVF, 0,0,jer_none,NULL,"JER_INT_IHB_CPU_LOOKUP_REPLY_OVF");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_BAD_CHANNEL_NUM, 0,0,jer_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_BAD_LKP_TYPE, 0,0,jer_special_handling_normal_10,NULL,"fix configuration, and reset to IPP. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_BAD_LRN_TYPE, 0,0,jer_special_handling_normal_10,NULL,"fix configuration, and reset to IPP.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_NIF_TX_FIFO_FULL, 0,0,jer_special_handling_normal_0,NULL,"this means that ELK link rate can not sustain FLP requests rate. Can be configuration error, or system setup error.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_RX_BROKEN_RECORD, 0,0,jer_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, means packet loss in ELK and should be treated in ELK.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_CPU_INFO_REPLY_VALID, 0,0,jer_none,NULL,"JER_INT_IHB_CPU_INFO_REPLY_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_CPU_INFO_REPLY_ERR, 0,0,jer_none,NULL,"JER_INT_IHB_CPU_INFO_REPLY_ERR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_CPU_LOOKUP_REPLY_VALID, 0,0,jer_none,NULL,"JER_INT_IHB_CPU_LOOKUP_REPLY_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_CPU_LOOKUP_REPLY_ERR, 0,0,jer_none,NULL,"JER_INT_IHB_CPU_LOOKUP_REPLY_ERR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_ISEM_INTERRUPT_ONE, 0,0,jer_none,NULL,"JER_INT_IHB_ISEM_INTERRUPT_ONE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_KAPS_DIRECT_LOOKUP_ERR, 0,0,jer_special_handling_normal_0,NULL,"Read Kaps Register Address: 0x0000001b/1a Bit0 = ECC Error Bit1 = Address range Error if Address range Error need to reconfigure PMF direct access address");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHB_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_MAX_HEADER_STACK_EXCEEDED, 0,0,jer_none,NULL,"JER_INT_IHP_MAX_HEADER_STACK_EXCEEDED");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED, 0,0,jer_none,NULL,"JER_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_VT_LIF_BANK_CONTENTION, 0,0,jer_none,NULL,"JER_INT_IHP_VT_LIF_BANK_CONTENTION");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_TT_LIF_BANK_CONTENTION, 0,0,jer_none,NULL,"JER_INT_IHP_TT_LIF_BANK_CONTENTION");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_MPLS_LABEL_TERMINATION_ERROR, 0,0,jer_none,NULL,"JER_INT_IHP_MPLS_LABEL_TERMINATION_ERROR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT, 0,0,jer_none,NULL,"JER_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_OAM_BFD_MISCONFIG_INT, 0,0,jer_none,NULL,"JER_INT_IHP_OAM_BFD_MISCONFIG_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_KAPS_DB_LOOKUP_ERR, 0,0,jer_none,NULL,"JER_INT_IHP_KAPS_DB_LOOKUP_ERR");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IHP_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_TCAM_PROTECTION_ERROR, 0,0,jer_none,NULL,"JER_INT_PPDB_A_TCAM_PROTECTION_ERROR");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_TCAM_QUERY_FAILURE_VALID, 0,0,jer_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_CAM_TABLE_FULL, 0,0,jer_special_handling_normal_0,NULL,"defrag should be initiated. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY, 0,0,jer_hard_reset,NULL,"JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling_normal_0,NULL,"Act according to specified in 0x0285, OemaManagementUnitFailure. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_CAM_TABLE_FULL, 0,0,jer_special_handling_normal_0,NULL,"defrag should be initiated.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY, 0,0,jer_hard_reset,NULL,"JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling_normal_0,NULL,"Act according to specified in 0x02c5, OembManagementUnitFailure.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL,"JER_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_A_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
#ifdef COMPILER_STRING_CONST_LIMIT
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling_normal_0,NULL,"Full documentation cannot be displayed with -pendantic compiler");
#else
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jer_special_handling_normal_0,NULL,"Act according to specified in 0x0265, MactManagementUnitFailure: Cam table full (0xA) -  Initiate Defrag operation to the table. Table coherency (0x9) - It will never happen, you should print an error to the screen and check your configuration. Delete unknown key (0x8) - You should print an error to the screen and check your configuration Reached max entry limit (0x7) - Nothing to do, the table is full. Inserted existing (0x6) - The new MAC was inserted, this is miss configuration and the SW should prevent such cases (delete the old MAC and insert again). Learn request over static (0x5) - You should print an error to the screen and check your configuration. Learn over existing (0x4) - Suggests DSP loss in the System. You should print an error to the screen and check your configuration. Change-fail non exist (0x3) - Suggests DSP loss in the System. You should print an error to the screen and check your configuration. Change request over static (0x2) - You should print an error to the screen and check your configuration Change non-exist from other (0x1) - Suggests DSP loss in the System. You should print an error to the screen and check your configuration. Change non-exist from self (0x0) - Suggests DSP loss in the System. You should print an error to the screen and check your configuration");
#endif
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_COMPLETED, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_STATIC_ALLOWED, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_STATIC_ALLOWED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_READY, 0,0,jer_special_handling_normal_0,NULL,"read mact_event if needed. This interrupt should be handled as part of the learning mechanism application (CPU based learning). Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FIFO_HIGH_THRESHOLD_REACHED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_REPLY_READY, 0,0,jer_special_handling_normal_0,NULL,"read mact_reply if needed. This interrupt should be handled as part of the learning mechanism application (CPU based learning).  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AGE_REACHED_END_INDEX, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AGE_REACHED_END_INDEX");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FLU_REACHED_END_INDEX, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FLU_REACHED_END_INDEX");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AMSG_DROP, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AMSG_DROP");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FMSG_DROP, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FMSG_DROP");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE, 0,0,jer_special_handling_normal_0,NULL,"fix configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_FCNT_COUNTER_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_MTM_CONSECUTIVE_OPS, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_MTM_CONSECUTIVE_OPS");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_LEL_ERR_DATA_VALID, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_LEL_ERR_DATA_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_CPU_ALLOWED, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_CPU_ALLOWED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FID_OR_LIF_EXCEED_LIMIT, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FID_OR_LIF_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_READY, 0,0,jer_special_handling_normal_0,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_READY");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FIFO_HIGH_THRESHOLD_REACHED");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FID_OR_LIF_EXCEED_LIMIT, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FID_OR_LIF_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_LEL_ERR_DATA_VALID, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_LEL_ERR_DATA_VALID");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_FCNT_COUNTER_OVERFLOW, 0,0,jer_none,NULL,"JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_FCNT_COUNTER_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_PPDB_B_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
#ifdef COMPILER_STRING_CONST_LIMIT
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_PIPE_0_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jer_special_handling_normal_0,NULL,"Full documentation cannot be displayed with -pendantic compiler");
#else
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_PIPE_0_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jer_special_handling_normal_0,NULL,"Check configuration of contexts in the device: This interrupt implies that an incoming reassembly context was equal to the value configured in the field BadReassemblyContext of the IRE Static Configuration register. One of the context mappings erroneously points to this reassembly context. The mappings are defined as follows:  For NIF packets, IRE table Nif Ctxt Map maps port plus channel to reassembly context. For packets through the recycle interface, the channel is mapped to a reassembly context through the IRE table Rcy Ctxt Map. For Pacskets through the CMIC interface, the CPU channel is mapped to a context through the IRE table CPU Ctxt Map. For the OAMP, OLP and Reg interfaces, the reassembly context is taken from the registers OAMP/OLP/Reg Fap Port Configuration, field OAMP/OLP/Reg Reassembly context (reg. address 0x120,0x121, 0x122). Interrupt handler log a message.");
#endif
#ifdef COMPILER_STRING_CONST_LIMIT
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_PIPE_1_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jer_special_handling_normal_0,NULL,"Full documentation cannot be displayed with -pendantic compiler");
#else
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_PIPE_1_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jer_special_handling_normal_0,NULL,"Check configuration of contexts in the device: This interrupt implies that an incoming reassembly context was equal to the value configured in the field BadReassemblyContext of the IRE Static Configuration register. One of the context mappings erroneously points to this reassembly context. The mappings are defined as follows:  For NIF packets, IRE table Nif Ctxt Map maps port plus channel to reassembly context. For packets through the recycle interface, the channel is mapped to a reassembly context through the IRE table Rcy Ctxt Map. For Pacskets through the CMIC interface, the CPU channel is mapped to a context through the IRE table CPU Ctxt Map. For the OAMP, OLP and Reg interfaces, the reassembly context is taken from the registers OAMP/OLP/Reg Fap Port Configuration, field OAMP/OLP/Reg Reassembly context (reg. address 0x120,0x121, 0x122). Interrupt handler log a message.");
#endif
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_DATA_ARRIVED, 0,0,jer_special_handling_normal_0,NULL,"Check how the code configued RegisterInterfacePacketControl: Check if the RegiPktErr field of that register was set to true.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_PACKET_SIZE, 0,0,jer_special_handling_normal_0,NULL,"Check how the code configured RegisterInterfacePacketControl: Check if the packet sent was smaller than 32 bytes.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_11,jer_special_handling_recur_12,"Check the parity error address, if it is in the range of the WDF memory, perform ingress soft reset. Otherwise, if memory was cached, perform scrub_shadow_write");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRE_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_0_RECYCLE, 0,0,jer_hard_reset,NULL,"should not happen by design.  It indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_0_RECYCLE, 0,0,jer_hard_reset,NULL,"should not happen by design.  It indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_OCB_0_RECYCLE, 0,0,jer_hard_reset,NULL,"Check configuratgion of OCB bank buffer ranges");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_1_RECYCLE, 0,0,jer_hard_reset,NULL,"should not happen by design.  It indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_1_RECYCLE, 0,0,jer_hard_reset,NULL,"should not happen by design.  It indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_OCB_1_RECYCLE, 0,0,jer_hard_reset,NULL,"Check configuratgion of OCB bank buffer ranges");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_0_EXTERNAL_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_0_EXTERNAL_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_MINI_MULTICAST_1_EXTERNAL_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_FULL_MULTICAST_1_EXTERNAL_OVERFLOW, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_RECEIVED_PIPE, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of contect pipe mapping");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_ETHERNET_METER_PTR, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of Ethernet Metering in PP block");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_REASSEMBLY_CONTEXT, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of reassembly contect mapping in IRE block");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_SENT_PIPE_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of contect pipe mapping");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ERROR_SENT_PIPE_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of contect pipe mapping");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_MMU_ECC_1B_ERR_INT, 0,0,jer_none,NULL,"JER_INT_IDR_MMU_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_MMU_ECC_2B_ERR_INT, 0,0,jer_hard_reset,NULL,"JER_INT_IDR_MMU_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_DP, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_DP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_PCB, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_PCB");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_SOP_PCB, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_SOP_PCB");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_DP, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_DP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_EOP, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_EOP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_IDLE, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_IDLE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_DRAM_REJECT, 0,0,jer_none,NULL,"JER_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_DRAM_REJECT");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_MAX_REPLICATION_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_MAX_REPLICATION_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_IS_MAX_REPLICATION_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_IS_MAX_REPLICATION_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_REPLICATION_EMPTY_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_REPLICATION_EMPTY_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_IS_REPLICATION_EMPTY_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_IS_REPLICATION_EMPTY_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_FLOW_ID_IS_OVER_96K_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_FLOW_ID_IS_OVER_96K_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_QUEUE_IS_OVER_96K_0, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ERR_QUEUE_IS_OVER_96K_1, 0,0,jer_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ECC_PARITY_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ECC_ECC_1B_ERR_INT, 1000,100,jer_scrub_shadow_write,jer_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JER_INT_IRR_ECC_ECC_2B_ERR_INT, 1000,100,jer_special_handling_normal_1,jer_special_handling_recur_2,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");


    /* end of  autogenerated  code*/

    dcmn_intr_add_handler(unit,                                 JER_INT_OAMP_PENDING_EVENT,     0,     0,                                 jer_interrupt_handle_oamppendingevent, NULL);
    dcmn_intr_add_handler(unit,                                 JER_INT_OAMP_STAT_PENDING_EVENT,0,     0,                                 jer_interrupt_handle_oamppendingstatevent, NULL);
	dcmn_intr_add_handler(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_READY, 0,0,jer_event_ready_handler,NULL);
    dcmn_intr_add_handler(unit, JER_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_READY, 0,0,jer_event_ready_handler,NULL);
}

#undef _ERR_MSG_MODULE_NAME
