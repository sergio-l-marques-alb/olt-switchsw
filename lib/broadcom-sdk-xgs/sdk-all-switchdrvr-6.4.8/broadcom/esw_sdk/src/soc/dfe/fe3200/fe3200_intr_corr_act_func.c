/*
 * $Id: fe3200_intr_corr_act_func.c, v1 Broadcom SDK $
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
 * Purpose:    Implement Correction action functions for fe3200 interrupts.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_defs.h>

#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dfe/fe3200/fe3200_intr_cb_func.h>
#include <soc/dfe/fe3200/fe3200_intr_corr_act_func.h>
#include <soc/dcmn/dcmn_ser_correction.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

#define FE3200_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE 16384
#define FE3200_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY 8

/*************
 * FUNCTIONS *
 *************/


int
fe3200_mem_decide_corrective_action(int unit,dcmn_memory_dc_t type,soc_mem_t mem,int copyno, fe3200_int_corr_act_type *action_type, char* special_msg)
{

    /*
*    Each memory is marked at HW PLs by 1 of the following 3 options: low, medium, severe.
*SW will implement soft error correction for memories according the following guidelines.
*Severe (ecc_1b)-if memory static and accessible SW ecc_1b correction routine will take place. Otherwise, ignore.
*Severe (parity/ecc_2b)-soft reset.
*Medium (ecc_1b)-if memory static and accessible SW ecc_1b correction routine will take place. Otherwise, ignore.
*Medium (parity/ecc_2b) - if memory static and accessible and SW shadow available fix with shadow. Otherwise soft reset
*Low (ecc_1b)-if memory static and accessible SW ecc_1b correction routine will take place. Otherwise, ignore.
*Low (parity/ecc_2b) - - if memory static and accessible and SW shadow available fix with shadow. Otherwise ignore.


    */

    soc_mem_severe_t severity = SOC_MEM_SEVERITY(unit,mem);
    uint32 dynamic_mem = soc_dfe_tbl_is_dynamic(unit, mem);
    int cache_enable;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;




    if (dynamic_mem) {
        switch (type) {

        case DCMN_ECC_ECC1B_DC:
        case DCMN_P_1_ECC_ECC1B_DC:
        case DCMN_P_2_ECC_ECC1B_DC:
        case DCMN_P_3_ECC_ECC1B_DC:
            *action_type = FE3200_INT_CORR_ACT_NONE;
            SOC_EXIT;
        default:
            if (severity == SOC_MEM_SEVERE_MISSING || severity == SOC_MEM_SEVERE_LOW) {
                *action_type = FE3200_INT_CORR_ACT_NONE;
                SOC_EXIT;
            }
            *action_type = FE3200_INT_CORR_ACT_SOFT_RESET;
        }
        SOC_EXIT;
    }

    /* static mem*/
    rc = interrupt_memory_cached(unit, mem, copyno, &cache_enable);
    if(rc != SOC_E_NONE) {
        sal_sprintf(special_msg, "mem %s failed to check cachebaility \n",
                    SOC_MEM_NAME(unit, mem));
    }
    SOCDNX_IF_ERR_EXIT(rc);
    if (cache_enable) {
    }
    switch (type) {

    case DCMN_ECC_ECC1B_DC:
    case DCMN_P_1_ECC_ECC1B_DC:
    case DCMN_P_2_ECC_ECC1B_DC:
    case DCMN_P_3_ECC_ECC1B_DC:
        *action_type =  FE3200_INT_CORR_ACT_ECC_1B_FIX;
        SOC_EXIT;
    default:
        if (cache_enable) {
            sal_sprintf(special_msg, "mem %s is not cachble \n",
                        SOC_MEM_NAME(unit, mem));
        }
        switch (severity) {
        case SOC_MEM_SEVERE_HIGH:
            *action_type =  FE3200_INT_CORR_ACT_SOFT_RESET;
            SOC_EXIT;
        case SOC_MEM_SEVERE_MEDIUM:
            *action_type =  cache_enable ? FE3200_INT_CORR_ACT_SHADOW : FE3200_INT_CORR_ACT_SOFT_RESET;
            SOC_EXIT;
        case SOC_MEM_SEVERE_LOW:
        case SOC_MEM_SEVERE_MISSING:
            *action_type =  cache_enable ? FE3200_INT_CORR_ACT_SHADOW : FE3200_INT_CORR_ACT_NONE;
            SOC_EXIT;
        }
    }

    exit:
        SOCDNX_FUNC_RETURN;
}




int
fe3200_interrupt_data_collection_for_shadowing(
    int unit, 
    int block_instance, 
    fe3200_interrupt_type en_interrupt, 
    char* special_msg, 
    fe3200_int_corr_act_type* p_corrective_action,
    fe3200_interrupt_mem_err_info* shadow_correct_info)
{
    int rc;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg = INVALIDr;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char* memory_name; 
    uint32 block;
    dcmn_memory_dc_t type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(shadow_correct_info);
    *p_corrective_action = FE3200_INT_CORR_ACT_NONE;

    switch(en_interrupt) {

    case FE3200_INT_DCL_ECC_ECC_2B_ERR_INT:

                cnt_reg = DCL_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_CCS_ECC_ECC_2B_ERR_INT:

                cnt_reg = CCS_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_RTP_ECC_ECC_2B_ERR_INT:

                cnt_reg = RTP_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_DCH_ECC_ECC_2B_ERR_INT:

                cnt_reg = DCH_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_ECI_ECC_ECC_2B_ERR_INT:

                cnt_reg = ECI_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_DCM_ECC_ECC_2B_ERR_INT:

                cnt_reg = DCM_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT:

                cnt_reg = FSRD_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT:

                cnt_reg = FMAC_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC2B_DC;
                break;
    case FE3200_INT_DCL_ECC_ECC_1B_ERR_INT:

                cnt_reg = DCL_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_CCS_ECC_ECC_1B_ERR_INT:

                cnt_reg = CCS_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_RTP_ECC_ECC_1B_ERR_INT:

                cnt_reg = RTP_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_DCH_ECC_ECC_1B_ERR_INT:

                cnt_reg = DCH_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_ECI_ECC_ECC_1B_ERR_INT:

                cnt_reg = ECI_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT:

                cnt_reg = FMAC_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT:

                cnt_reg = FSRD_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_DCM_ECC_ECC_1B_ERR_INT:

                cnt_reg = DCM_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_ECC1B_DC;
                break;
    case FE3200_INT_CCS_CRP_PARITY_ERR_INT:

                cnt_reg = CCS_CRP_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_PARITY_DC;
                break;
    case FE3200_INT_FMAC_ECC_PARITY_ERR_INT:

                cnt_reg = FMAC_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  DCMN_ECC_PARITY_DC;
                break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_interrupt));
      }



    rc = dcmn_get_cnt_reg_values(unit, type, cnt_reg,&cntf, &cnt_overflowf, &addrf,&addr_validf);
    SOCDNX_IF_ERR_EXIT(rc);




    if (addr_validf != 0) {

        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);

        if(mem!= INVALIDm) {
            memory_name = SOC_MEM_NAME(unit, mem);
        } else {
            memory_name = NULL;
        }
         
        switch(mem) {
        case INVALIDm:
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory is not accessible",
                    cntf, cnt_overflowf, addrf);
            break;     
        default:


            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                        cntf, cnt_overflowf, addrf, memory_name, numels, index);
            /* set corrective action */
            rc = fe3200_mem_decide_corrective_action(unit, type, mem, block_instance,p_corrective_action, special_msg); 
            SOCDNX_IF_ERR_EXIT(rc);
            shadow_correct_info->array_index = numels;
            shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
            shadow_correct_info->min_index = index;
            shadow_correct_info->max_index = index;
            shadow_correct_info->mem = mem;


        }
    
     } else {
         sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid",
                     cntf, cnt_overflowf, addrf);
     }

exit:
    SOCDNX_FUNC_RETURN;
}


int
fe3200_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  fe3200_interrupt_type interrupt_id,
  char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*empty function*/

    SOCDNX_FUNC_RETURN;
}

int
fe3200_interrupt_handles_corrective_action_special_handling (
  int unit,
  int block_instance,
  fe3200_interrupt_type interrupt_id,
  char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*empty function*/

    SOCDNX_FUNC_RETURN;
}

int fe3200_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_device_reset(unit, SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DCMN_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe3200_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    char* msg_print,
    char* msg)
{
    soc_interrupt_db_t* interrupt;
    char print_msg[FE3200_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, FE3200_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#else
    sal_snprintf(print_msg, FE3200_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt->name, interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#endif

    LOG_INFO(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    
exit:
    SOCDNX_FUNC_RETURN;
}




int 
fe3200_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    fe3200_interrupt_mem_err_info* ecc_1b_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry=0;
    int entry_dw;
    int current_index;
    int cache_enable;
    soc_reg_t cnt_reg = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].cnt_reg;
    uint64 clear_buf;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ecc_1b_correct_info_p);

    rc = interrupt_memory_cached(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->copyno, &cache_enable);

    if(rc != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Couldnt decide cache state for %s \n"),
                    SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
        SOC_EXIT;
     }
                  
   
    if (soc_mem_is_readonly(unit, ecc_1b_correct_info_p->mem) || soc_mem_is_writeonly(unit, ecc_1b_correct_info_p->mem) || soc_mem_is_signal(unit, ecc_1b_correct_info_p->mem)) {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Interrupt will not be handled cause %s is dynamic\n"),
                    SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
        SOC_EXIT;
    }

    entry_dw = soc_mem_entry_words(unit, ecc_1b_correct_info_p->mem);
    data_entry = sal_alloc((entry_dw * 4), "FE3200_INTERRUPT ecc 1 data entry allocation");

    if (NULL == data_entry) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Before correction of %s \n"),
                SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
    
    for (current_index = ecc_1b_correct_info_p->min_index ; current_index <= ecc_1b_correct_info_p->max_index ; current_index++) {
        
        rc = soc_mem_array_read(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->array_index, ecc_1b_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);
        
        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &clear_buf));

        if (cache_enable&& SOC_MEM_FIELD_VALID(unit, ecc_1b_correct_info_p->mem, ECCf)) {

            LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"correction for  %s will be done by shadow\n"),
                        SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
        }
        else
        {
            LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"correction for  %s will be done by ecc 1 bit correction\n"),
                        SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
        }


        /* if mem not in cache and memory have ecc (what should be either we not supposed to be here)*/
        if (!cache_enable && SOC_MEM_FIELD_VALID(unit, ecc_1b_correct_info_p->mem, ECCf) ) {
            uint32 mem_row_bit_width = soc_mem_entry_bits(unit, ecc_1b_correct_info_p->mem) - soc_mem_field_length(unit, ecc_1b_correct_info_p->mem, ECCf);
            uint32 ecc_field[1]; 
            uint32 ecc_field_prev;

            *ecc_field = soc_mem_field32_get(unit, ecc_1b_correct_info_p->mem, data_entry, ECCf);
            ecc_field_prev = *ecc_field;
            if (1<<soc_mem_field_length(unit, ecc_1b_correct_info_p->mem, ECCf)< mem_row_bit_width) {
                LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Ecc len:%d for memory %s len%d \n"),
                           soc_mem_field_length(unit, ecc_1b_correct_info_p->mem, ECCf), SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem), current_index));
                SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
            }
            SOCDNX_IF_ERR_EXIT(ecc_correction(unit,ecc_1b_correct_info_p->mem, mem_row_bit_width,data_entry, ecc_field));
            if (ecc_field_prev != *ecc_field) {
                soc_mem_field32_set(unit,ecc_1b_correct_info_p->mem,data_entry, ECCf, *ecc_field);
            }
        }



        rc = soc_mem_array_write(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->array_index, ecc_1b_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);

    }

exit:
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOCDNX_FUNC_RETURN;
}



int 
fe3200_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    fe3200_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry=0;
    uint8 orig_read_through_flag;
    uint8 *vmap;
    int entry_dw;
    int current_index;
    soc_mem_t mem;
    int copyno;

    SOCDNX_INIT_FUNC_DEFS;

    mem = shadow_correct_info_p->mem;
    copyno = (shadow_correct_info_p->copyno == COPYNO_ALL) ? SOC_MEM_BLOCK_ANY(unit, mem) : shadow_correct_info_p->copyno;

    orig_read_through_flag = SOC_MEM_FORCE_READ_THROUGH(unit);
    SOCDNX_NULL_CHECK(shadow_correct_info_p);
 
    if (soc_dfe_tbl_is_dynamic(unit, mem)) {
        SOC_EXIT;
    }

    if (!soc_mem_cache_get(unit, mem, copyno)) {
         SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
 
    entry_dw = soc_mem_entry_words(unit, mem);
    data_entry = sal_alloc((entry_dw * 4), "ARAD_INTERRUPT Shadow data entry allocation");

    if (NULL == data_entry) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0); /* read from cache */
    for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {

        vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
        if (!CACHE_VMAP_TST(vmap, current_index) || SOC_MEM_TEST_SKIP_CACHE(unit)) {
            LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Cache memory %s at entry %u is not available"),
                       SOC_MEM_NAME(unit, mem), current_index));
        }

        rc = soc_mem_array_read(unit, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);

        rc = soc_mem_array_write(unit, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_through_flag);
    SOCDNX_FUNC_RETURN;
}




int
fe3200_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    char* msg)
{
    uint32 port = block_instance*4 + SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].bit_in_field;
    uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;


    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_enable_set, (unit, port, 0)); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}


/* Corrective Action main function */
int
fe3200_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    char *msg,
    fe3200_int_corr_act_type corr_act,
    void *param1, 
    void *param2)
{
    int rc;
    uint32 flags;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SOCDNX_IF_ERR_EXIT(rc);
    
    /* Corrective action will be performed only if BCM_AND_USR_CB flag set or corrective action override flag not overriden */
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = FE3200_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
    case FE3200_INT_CORR_ACT_NONE:
        rc = fe3200_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case FE3200_INT_CORR_ACT_SOFT_RESET:
        rc = fe3200_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case FE3200_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
        rc = fe3200_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case FE3200_INT_SPECIAL_HANDLING:
        rc = fe3200_interrupt_handles_corrective_action_special_handling(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case FE3200_INT_CORR_ACT_PRINT:
        rc = fe3200_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case FE3200_INT_CORR_ACT_SHADOW:
        rc = fe3200_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(fe3200_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case FE3200_INT_CORR_ACT_ECC_1B_FIX:
        rc = fe3200_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id,(fe3200_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("UnKnown corrective action")));
  }
   
exit:
  SOCDNX_FUNC_RETURN;
}

int fe3200_interrupt_print_info(
      int unit,
    int block_instance,
    fe3200_interrupt_type en_fe3200_interrupt,
    int recurring_action,
    fe3200_int_corr_act_type corr_act,
    char *special_msg)
{
    int rc;
    uint32 flags;  
    soc_interrupt_db_t* interrupt;

    char cur_special_msg[FE3200_INTERRUPT_SPECIAL_MSG_SIZE];
    char cur_corr_act_msg[FE3200_INTERRUPT_COR_ACT_MSG_SIZE];
    char print_msg[FE3200_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_fe3200_interrupt]);

    if (special_msg == NULL) {
        sal_snprintf(cur_special_msg, sizeof(cur_special_msg), "None");
    } else {
        if(sal_strlen(special_msg) >= FE3200_INTERRUPT_SPECIAL_MSG_SIZE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Too long special message"))); 
        }
        sal_strncpy(cur_special_msg, special_msg, sizeof(cur_special_msg) - 1);
    }

    rc = soc_interrupt_flags_get(unit, en_fe3200_interrupt, &flags);
    SOCDNX_IF_ERR_EXIT(rc);

    /* Corrective action will be performed only if BCM_AND_USR_CB flag set or corrective action override flag not overriden */
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = FE3200_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
        case FE3200_INT_CORR_ACT_NONE:
            sal_snprintf(cur_corr_act_msg, FE3200_INTERRUPT_COR_ACT_MSG_SIZE, "None");
            break;
        case FE3200_INT_CORR_ACT_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, FE3200_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Reset");
            break;
        case FE3200_INT_CORR_ACT_PRINT:
            sal_snprintf(cur_corr_act_msg, FE3200_INTERRUPT_COR_ACT_MSG_SIZE, "Print");
            break;

        default:
            sal_snprintf(cur_corr_act_msg, FE3200_INTERRUPT_COR_ACT_MSG_SIZE, "Unknown");
    }

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, FE3200_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                en_fe3200_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#else
    sal_snprintf(print_msg, FE3200_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                interrupt->name, en_fe3200_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#endif

    /* Print per interrupt mechanism */
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




#undef _ERR_MSG_MODULE_NAME

