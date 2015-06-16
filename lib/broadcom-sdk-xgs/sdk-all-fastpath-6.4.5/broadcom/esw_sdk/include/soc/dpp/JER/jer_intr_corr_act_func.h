/*
 * $Id: jer_appl_intr_corr_act_func.h, v1 Broadcom SDK $
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
 * Purpose:    Implement header correction action functions for jericho interrupts.
 */

#ifndef _JER_INTR_CORR_ACT_FUNC_H_
#define _JER_INTR_CORR_ACT_FUNC_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/dpp/JER/jer_intr.h>

/*************
 * DEFINES   *
 *************/
#define JER_INTERRUPT_PRINT_MSG_SIZE               512
#define JER_INTERRUPT_COR_ACT_MSG_SIZE             36
#define JER_INTERRUPT_SPECIAL_MSG_SIZE             256

/*************
 * ENUMERATIONS *
 *************/
typedef enum {
    JER_INT_CORR_ACT_NONE = 0x0,
    JER_INT_CORR_ACT_SOFT_RESET = 0x1,
    JER_INT_CORR_ACT_PRINT = 0x2,
    JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO=0x3,
    JER_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO=0x4,
    JER_INT_CORR_ACT_SHADOW = 0x5,
    JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS = 0x6,
    JER_INT_CORR_ACT_HARD_RESET = 0x7,
    JER_INT_CORR_ACT_REPROGRAM_RESOURCE = 0x8
} jer_int_corr_act_type;

/*************
 * STRUCTURES *
 *************/
typedef struct {
    soc_mem_t mem;
    unsigned int array_index;
    int copyno;
    int min_index;
    int max_index;
} jer_interrupt_mem_err_info;

/*************
 * FUNCTIONS *
 *************/

/* Corrective Action main function */
int jer_interrupt_handles_corrective_action(int unit, int block_instance, jer_interrupt_type interrupt_id, char *msg, jer_int_corr_act_type corr_act, void *param1, void *param2);

/*
 *  Corrective Action functions    
 */
int jer_interrupt_handles_corrective_action_do_nothing (int unit, int block_instance, jer_interrupt_type interrupt_id, char *msg);
int jer_interrupt_handles_corrective_action_hard_reset(int unit,int block_instance,jer_interrupt_type interrupt_id,char *msg);
int jer_interrupt_handles_corrective_action_print(int unit, int block_instance, jer_interrupt_type interrupt_id, char* msg_print, char* msg);
int jer_interrupt_handles_corrective_action_soft_reset(int unit,int block_instance,jer_interrupt_type interrupt_id,char *msg);
int jer_interrupt_handles_corrective_action_shadow(int unit,int block_instance,jer_interrupt_type interrupt_id,
                                               jer_interrupt_mem_err_info* shadow_correct_info_p,char* msg);  
int jer_interrupt_data_collection_for_shadowing(int unit,int block_instance,jer_interrupt_type en_interrupt,char* special_msg,
                                            jer_int_corr_act_type* p_corrective_action,jer_interrupt_mem_err_info* shadow_correct_info);

int jer_interrupt_handles_corrective_action_shutdown_fbr_link(int unit,int block_instance,jer_interrupt_type interrupt_id,char* msg);
    
/* print function */
int jer_interrupt_print_info(int unit, int block_instance, jer_interrupt_type en_jer_interrupt, int recurring_action, jer_int_corr_act_type corr_act, char *general_msg);

#endif /* _JER_INTR_CORR_ACT_FUNC_H_ */
