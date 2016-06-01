/*
 * $Id: jer_appl_intr_corr_act_func.h, v1 Broadcom SDK $
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
 * Purpose:    Implement header correction action functions for jericho interrupts.
 */

#ifndef _DCMN_INTR_CORR_ACT_FUNC_H_
#define _DCMN_INTR_CORR_ACT_FUNC_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/dcmn/dcmn_intr_handler.h>

/*************
 * DEFINES   *
 *************/
#define DCMN_INTERRUPT_PRINT_MSG_SIZE               512
#define DCMN_INTERRUPT_COR_ACT_MSG_SIZE             36
#define DCMN_INTERRUPT_SPECIAL_MSG_SIZE             1000

/*************
 * ENUMERATIONS *
 *************/
typedef enum {
    DCMN_INT_CORR_ACT_CLEAR_CHECK,
    DCMN_INT_CORR_ACT_CONFIG_DRAM,
    DCMN_INT_CORR_ACT_ECC_1B_FIX,
    DCMN_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_FORCE,
    DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO,
    DCMN_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO,
    DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO,
    DCMN_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO,
    DCMN_INT_CORR_ACT_HARD_RESET,
    DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC,
    DCMN_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_INGRESS_HARD_RESET,
    DCMN_INT_CORR_ACT_IPS_QDESC,
    DCMN_INT_CORR_ACT_NONE,
    DCMN_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_PRINT,
    DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE,
    DCMN_INT_CORR_ACT_RTP_LINK_MASK_CHANGE,
    DCMN_INT_CORR_ACT_RX_LOS_HANDLE,
    DCMN_INT_CORR_ACT_SHADOW,
    DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET,
    DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS,
    DCMN_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION,
    DCMN_INT_CORR_ACT_SOFT_RESET,
    DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB,
    DCMN_INT_CORR_ACT_RTP_SLSCT,
    DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,
    DCMN_INT_CORR_ACT_MC_RTP_CORRECT,
    DCMN_INT_CORR_ACT_UC_RTP_CORRECT,
    DCMN_INT_CORR_ACT_ALL_REACHABLE_FIX,
    DCMN_INT_CORR_ACT_EVENT_READY,
    DCMN_INT_CORR_ACT_MAX
} dcmn_int_corr_act_type;

/*************
 * STRUCTURES *
 *************/
typedef struct {
    soc_mem_t mem;
    unsigned int array_index;
    int copyno;
    int min_index;
    int max_index;
} dcmn_interrupt_mem_err_info;

typedef struct dcmn_intr_action_s
{
    soc_handle_interrupt_func func_arr;
    dcmn_int_corr_act_type corr_action;
} dcmn_intr_action_t;

/*************
 * FUNCTIONS *
 *************/

void dcmn_intr_action_info_set(int unit, dcmn_intr_action_t *dcmn_intr_action_info_set);
dcmn_intr_action_t *dcmn_intr_action_info_get(int unit);
    
int dcmn_mem_decide_corrective_action(int unit,dcmn_memory_dc_t type,soc_mem_t mem,int copyno, dcmn_int_corr_act_type *action_type, char* special_msg);
    

int 
dcmn_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg);
    
int 
dcmn_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* ecc_1b_correct_info_p,
    char* msg);
int
dcmn_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  uint32 interrupt_id,
  char *msg);
    
int dcmn_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);

int dcmn_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);        

int
dcmn_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    dcmn_int_corr_act_type corr_act,
    char *special_msg);

int
dcmn_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char* msg_print,
    char* msg);

#ifdef BCM_DFE_SUPPORT
int
dcmn_interrupt_handles_corrective_action_for_rtp_slsct(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg);
#endif

#endif /* _DCMN_INTR_CORR_ACT_FUNC_H_ */
