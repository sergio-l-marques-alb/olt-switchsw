/*
 * $Id: soc_dcmn_intr_handler.h, v1 16/06/2014 09:55:39 azarrin $
 *
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
 *
 * Purpose:    Implement header for soc interrupt handler.
 */

#ifndef _DCMN_INTR_HANDLER_H_
#define _DCMN_INTR_HANDLER_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/defs.h>
#include <soc/dcmn/error.h>

/*************
 * FUNCTIONS *
 *************/

typedef enum dcmn_memory_dc_e
{
    DCMN_INVALID_DC=-1,
    DCMN_ECC_ECC2B_DC,
    DCMN_ECC_ECC1B_DC,
    DCMN_P_1_ECC_ECC2B_DC,
    DCMN_P_1_ECC_ECC1B_DC,
    DCMN_P_2_ECC_ECC2B_DC,
    DCMN_P_2_ECC_ECC1B_DC,
    DCMN_P_3_ECC_ECC2B_DC,
    DCMN_P_3_ECC_ECC1B_DC,
    DCMN_ECC_PARITY_DC

} dcmn_memory_dc_t;




typedef struct dcmn_block_control_info_s
{
    int ecc1_int;
    int ecc2_int;
    int parity_int;
    soc_reg_t gmo_reg;   /*  global mem option reg*/
    soc_reg_t ecc1_monitor_mem_reg;   /*  global mem option reg*/

} dcmn_block_control_info_t;

typedef struct dcmn_block_control_info_main_s
{
    dcmn_block_control_info_t *map;
    dcmn_block_control_info_t *config;

} dcmn_block_control_info_main_t;


void dcmn_collect_blocks_control_info(int unit,dcmn_block_control_info_t *map, dcmn_block_control_info_t *config);
 
int dcmn_disable_block_ecc_check(int unit, int interrupt_id, soc_mem_t mem, int copyno, soc_reg_above_64_val_t value, soc_reg_above_64_val_t orig_value);

int dcmn_num_of_ints(int unit);

void dcmn_intr_switch_event_cb(
    int unit, 
    soc_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata);
int dcmn_intr_add_handler(int unit, int en_inter, int occurrences, int timeCycle, soc_handle_interrupt_func inter_action, soc_handle_interrupt_func inter_recurring_action);
int dcmn_intr_add_handler_ext(int unit, int en_inter, int occurrences, int timeCycle, soc_handle_interrupt_func inter_action, soc_handle_interrupt_func inter_recurring_action,char *msg);
int dcmn_intr_handler_deinit(int unit);
int dcmn_intr_handler_short_init(int unit);
int dcmn_intr_handler_init(int unit);
int dcmn_get_cnt_reg_values(int unit,dcmn_memory_dc_t type,soc_reg_t cnt_reg,int copyno, uint32 *cntf,uint32 *cnt_overflowf, uint32 *addrf,uint32 *addr_validf);
dcmn_memory_dc_t get_cnt_reg_type(int unit,soc_reg_t cnt_reg);
int dcmn_blktype_to_index(soc_block_t blktype);

int dcmn_get_ser_entry_from_cache(int unit,  soc_mem_t mem, int copyno, int array_index, int index, uint32 *data_entr);


#endif /*_DCMN_INTR_HANDLER_H_ */
