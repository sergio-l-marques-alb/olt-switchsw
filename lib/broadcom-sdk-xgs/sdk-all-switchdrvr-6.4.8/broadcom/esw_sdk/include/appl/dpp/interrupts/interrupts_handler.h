/*
 * $Id: interrupts_handler.h,v 1.5 Broadcom SDK $
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
 */

#ifndef _DPP_INTERRUPTS_HANDLER_H_
#define _DPP_INTERRUPTS_HANDLER_H_

#include <soc/dpp/ARAD/arad_interrupts.h>

typedef int (* arad_handle_interrupt_func)(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg);

/*
 * static data strucure which holds the info about: 
 *             the function suppose to run for each interrupt
 *              info about storm threshold
 */
struct arad_interrupt_handle_data_base {
    arad_handle_interrupt_func *func_arr;/*interrupt action, always run it*/

    /* recurring action function/params */
    arad_handle_interrupt_func *func_arr_recurring_action;/*interrupt action, run it only in a storm*/
    int *recurring_action_counters;
    int *recurring_action_time; 
    int *recurring_action_cycle_time;  /*time in seconds of each cycle, -1 is without recurring_action*/
    int *recurring_action_cycle_counting;  /*threshold in each cycle, -1 is without recurring_action*/
};
typedef struct arad_interrupt_handle_data_base arad_interrupt_handle_data_base_t;

/* 
 * Main CallBack Function
 */
void diag_arad_switch_event_cb(int unit, bcm_switch_event_t event, uint32 arg1, uint32 arg2, uint32 arg3, void *userdata);

/*
 *  init the  data base the first time using it.  
 */
int arad_interrupt_handler_data_base_init(int unit);

/*
 * add_interrupt_handler (without storm threshold)
 * input: 
 * enInter - the interrupt.
 * interruptHandler - how to handle the interrupt.
 */
int arad_interrupt_add_interrupt_handler(int unit, arad_interrupt_type en_arad_inter, int occurrences, int timeCycle, arad_handle_interrupt_func inter_action, arad_handle_interrupt_func inter_recurring_action);

int arad_handle_interrupt_recurring_detect(int unit, int block_instance, arad_interrupt_type en_arad_interrupt);

int arad_interrupt_handler_appl_init(int unit);

int arad_interrupt_handler_appl_deinit(int unit);


#endif /*__DPP_INTERRUPTS_HANDLER_H_ */
