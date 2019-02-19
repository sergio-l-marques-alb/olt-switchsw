/*
 * $Id: fe3200_intr_corr_act_func.h, v1 Broadcom SDK $
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
 * Purpose:    Implement header correction action functions for fe3200icho interrupts.
 */

#ifndef _FE3200_INTR_CORR_ACT_FUNC_H_
#define _FE3200_INTR_CORR_ACT_FUNC_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/dfe/fe3200/fe3200_intr.h>

/*************
 * DEFINES   *
 *************/
#define FE3200_INTERRUPT_PRINT_MSG_SIZE               512
#define FE3200_INTERRUPT_COR_ACT_MSG_SIZE             36
#define FE3200_INTERRUPT_SPECIAL_MSG_SIZE             256

/*************
 * ENUMERATIONS *
 *************/
typedef enum {
    FE3200_INT_CORR_ACT_NONE = 0x0,
    FE3200_INT_CORR_ACT_SOFT_RESET = 0x1,
    FE3200_INT_CORR_ACT_PRINT = 0x2
} fe3200_int_corr_act_type;

/*************
 * STRUCTURES *
 *************/
typedef struct {
    soc_mem_t mem;
    unsigned array_index;
    int copyno;
    int min_index;
    int max_index;
} fe3200_interrupt_mem_err_info;

/*************
 * FUNCTIONS *
 *************/

/* Corrective Action main function */
int fe3200_interrupt_handles_corrective_action(int unit, int block_instance, fe3200_interrupt_type interrupt_id, char *msg, fe3200_int_corr_act_type corr_act, void *param1, void *param2);

/*
 *  Corrective Action functions    
 */
int fe3200_interrupt_handles_corrective_action_do_nothing (int unit, int block_instance, fe3200_interrupt_type interrupt_id, char *msg);
int fe3200_interrupt_handles_corrective_action_hard_reset(int unit,int block_instance,fe3200_interrupt_type interrupt_id,char *msg);
int fe3200_interrupt_handles_corrective_action_print(int unit, int block_instance, fe3200_interrupt_type interrupt_id, char* msg_print, char* msg);

/* print function */
int fe3200_interrupt_print_info(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt, int recurring_action, fe3200_int_corr_act_type corr_act, char *general_msg);

#endif /* _FE3200_INTR_CORR_ACT_FUNC_H_ */
