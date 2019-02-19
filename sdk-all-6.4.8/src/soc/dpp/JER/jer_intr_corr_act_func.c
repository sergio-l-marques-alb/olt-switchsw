/*
 * $Id: jer_appl_intr_corr_act_func.c, v1 Broadcom SDK $
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
 * Purpose:    Implement Correction action functions for jericho interrupts.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <sal/core/dpc.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <soc/dpp/mbcm.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_dram.h> 
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dpp/JER/jer_intr_corr_act_func.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_oam.h>
#include <soc/dcmn/dcmn_ser_correction.h>


/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

#define JER_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE 16384
#define JER_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY 8


/*************
 * FUNCTIONS *
 *************/

int
jer_mem_decide_corrective_action(int unit,dcmn_memory_dc_t type,soc_mem_t mem,int copyno, jer_int_corr_act_type *action_type, char* special_msg)
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
    uint32 dynamic_mem = dpp_tbl_is_dynamic(unit, mem) ;
    int cache_enable;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;




    if (dynamic_mem) {
        switch (type) {

        case DCMN_ECC_ECC1B_DC:
        case DCMN_P_1_ECC_ECC1B_DC:
        case DCMN_P_2_ECC_ECC1B_DC:
        case DCMN_P_3_ECC_ECC1B_DC:
            *action_type = JER_INT_CORR_ACT_NONE;
            SOC_EXIT;
        default:
            if (severity == SOC_MEM_SEVERE_MISSING || severity == SOC_MEM_SEVERE_LOW) {
                *action_type = JER_INT_CORR_ACT_NONE;
                SOC_EXIT;
            }
            *action_type = JER_INT_CORR_ACT_SOFT_RESET;
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
        *action_type =  JER_INT_CORR_ACT_ECC_1B_FIX;
        SOC_EXIT;
    default:
        if (cache_enable) {
            sal_sprintf(special_msg, "mem %s is not cachble \n",
                        SOC_MEM_NAME(unit, mem));
        }
        switch (severity) {
        case SOC_MEM_SEVERE_HIGH:
            *action_type =  JER_INT_CORR_ACT_SOFT_RESET;
            SOC_EXIT;
        case SOC_MEM_SEVERE_MEDIUM:
            *action_type =  cache_enable ? JER_INT_CORR_ACT_SHADOW : JER_INT_CORR_ACT_SOFT_RESET;
            SOC_EXIT;
        case SOC_MEM_SEVERE_LOW:
        case SOC_MEM_SEVERE_MISSING:
            *action_type =  cache_enable ? JER_INT_CORR_ACT_SHADOW : JER_INT_CORR_ACT_NONE;
            SOC_EXIT;
        }
    }

    exit:
        SOCDNX_FUNC_RETURN;
}

int
jer_interrupt_data_collection_for_shadowing(
    int unit, 
    int block_instance, 
    jer_interrupt_type en_interrupt, 
    char* special_msg, 
    jer_int_corr_act_type* p_corrective_action,
    jer_interrupt_mem_err_info* shadow_correct_info)
{
    int rc;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg = INVALIDr;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char* memory_name; 
    uint32 block;
    dcmn_memory_dc_t type ;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(shadow_correct_info);
    *p_corrective_action = JER_INT_CORR_ACT_NONE;
    cnt_reg = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].cnt_reg;
    if (!SOC_REG_IS_VALID(unit, cnt_reg)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_interrupt));
    }
    type = get_cnt_reg_type(unit, cnt_reg); 

    if (type==DCMN_INVALID_DC) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_interrupt));
    }
    SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));

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
            rc = jer_mem_decide_corrective_action(unit, type, mem, block_instance,p_corrective_action, special_msg); 
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
jer_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    jer_interrupt_mem_err_info* ecc_1b_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry=0;
    int entry_dw;
    int current_index;
    int cache_enable;
    soc_reg_above_64_val_t value, orig_value;

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
    data_entry = sal_alloc((entry_dw * 4), "JER_INTERRUPT ecc 1 data entry allocation");

    if (NULL == data_entry) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Before correction of %s \n"),
                SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));

    /* disable ser for block because we need to read the errornous memory*/
    SOC_REG_ABOVE_64_CLEAR(value);
    SOCDNX_IF_ERR_EXIT(dcmn_disable_block_ecc_check(unit,interrupt_id, ecc_1b_correct_info_p->mem,value,orig_value));

    for (current_index = ecc_1b_correct_info_p->min_index ; current_index <= ecc_1b_correct_info_p->max_index ; current_index++) {
        

        rc = soc_mem_array_read(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->array_index, ecc_1b_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);

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
    SOCDNX_IF_ERR_EXIT(dcmn_disable_block_ecc_check(unit,interrupt_id,ecc_1b_correct_info_p->mem,orig_value,NULL));

exit:
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOCDNX_FUNC_RETURN;
}



int
jer_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  jer_interrupt_type interrupt_id,
  char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*empty function*/

    SOCDNX_FUNC_RETURN;
}


int jer_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
jer_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char* msg_print,
    char* msg)
{
    soc_interrupt_db_t* interrupt;
    char print_msg[JER_INTERRUPT_PRINT_MSG_SIZE];

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
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#else
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d | %s\n ", 
                interrupt->name, interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#endif

    LOG_INFO(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    
exit:
    SOCDNX_FUNC_RETURN;
}

int jer_interrupt_handles_corrective_action_handle_oamp_event_fifo(int unit, 
                                                                       int block_instance, 
                                                                       jer_interrupt_type en_arad_interrupt, 
                                                                       char* msg)
{

	sal_dpc(arad_pp_oam_dma_event_handler, INT_TO_PTR(unit), INT_TO_PTR(JER_PP_OAM_API_DMA_EVENT_TYPE_EVENT),0,0,0 );
    return SOC_E_NONE;
}
 
int jer_interrupt_handles_corrective_action_handle_oamp_stat_event_fifo(int unit,
                                                                            int block_instance,
                                                                            jer_interrupt_type en_arad_interrupt,
                                                                            char* msg)
{

	sal_dpc(arad_pp_oam_dma_event_handler, INT_TO_PTR(unit), INT_TO_PTR(JER_PP_OAM_API_DMA_EVENT_TYPE_STAT_EVENT),0,0,0 );
    return SOC_E_NONE;
}
int
jer_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char* msg)
{
   uint32 port = block_instance*4 + SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].bit_in_field;
    uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;
    

    rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_enable_set, (unit, port, 0,0)); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:


    SOCDNX_FUNC_RETURN;

}

int jer_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: Hard reset!\n")));
#endif

    rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


/* Corrective Action main function */
int
jer_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg,
    jer_int_corr_act_type corr_act,
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
        corr_act = JER_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
    case JER_INT_CORR_ACT_NONE:
        rc = jer_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case JER_INT_CORR_ACT_SOFT_RESET:
        rc = jer_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case JER_INT_CORR_ACT_HARD_RESET:
        rc = jer_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case JER_INT_CORR_ACT_PRINT:
        rc = jer_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_SHADOW:
        rc = jer_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(jer_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_ECC_1B_FIX:
        rc = jer_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id,(jer_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
        rc = jer_interrupt_handles_corrective_action_handle_oamp_event_fifo(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
        rc = jer_interrupt_handles_corrective_action_handle_oamp_stat_event_fifo(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
        rc = jer_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("UnKnown corrective action")));
  }
   
exit:
  SOCDNX_FUNC_RETURN;
}

int jer_interrupt_print_info(
      int unit,
    int block_instance,
    jer_interrupt_type en_jer_interrupt,
    int recurring_action,
    jer_int_corr_act_type corr_act,
    char *special_msg)
{
    int rc;
    uint32 flags;  
    soc_interrupt_db_t* interrupt;

    char cur_special_msg[JER_INTERRUPT_SPECIAL_MSG_SIZE];
    char cur_corr_act_msg[JER_INTERRUPT_COR_ACT_MSG_SIZE];
    char print_msg[JER_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_jer_interrupt]);

    if (special_msg == NULL) {
        sal_snprintf(cur_special_msg, sizeof(cur_special_msg), "None");
    } else {
        sal_strncpy(cur_special_msg, special_msg, sizeof(cur_special_msg) - 1);
    }

    rc = soc_interrupt_flags_get(unit, en_jer_interrupt, &flags);
    SOCDNX_IF_ERR_EXIT(rc);

    /* Corrective action will be performed only if BCM_AND_USR_CB flag set or corrective action override flag not overriden */
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = JER_INT_CORR_ACT_NONE;
    }
  
 

    switch(corr_act) {
        case JER_INT_CORR_ACT_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset");
            break;
        case JER_INT_CORR_ACT_NONE:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "None");
            break;
        case JER_INT_CORR_ACT_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Reset");
            break;
        case JER_INT_CORR_ACT_PRINT:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Print");
            break;
        case JER_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Event Fifo");
            break;
        case JER_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Statistics Event Fifo");
            break;
        case JER_INT_CORR_ACT_SHADOW:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow");
            break;
        case JER_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown link");
            break;
        default:
            sal_snprintf(cur_corr_act_msg, JER_INTERRUPT_COR_ACT_MSG_SIZE, "Unknown");
    }

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                en_jer_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#else
    sal_snprintf(print_msg, JER_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", 
                interrupt->name, en_jer_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#endif

    /* Print per interrupt mechanism */
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int 
jer_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    jer_interrupt_mem_err_info* shadow_correct_info_p,
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
 
    if (dpp_tbl_is_dynamic(unit, mem)) {
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
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_through_flag);
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOCDNX_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME

