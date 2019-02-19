/*
 * $Id: jer_appl_intr_corr_act_func.c, v1 Broadcom SDK $
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
jer_interrupt_data_collection_for_shadowing(
    int unit, 
    int block_instance, 
    jer_interrupt_type en_interrupt, 
    char* special_msg, 
    dcmn_int_corr_act_type* p_corrective_action,
    dcmn_interrupt_mem_err_info* shadow_correct_info)
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
    *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
    cnt_reg = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].cnt_reg;
    if (!SOC_REG_IS_VALID(unit, cnt_reg)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_interrupt));
    }
    type = get_cnt_reg_type(unit, cnt_reg); 

    if (type==DCMN_INVALID_DC) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_interrupt));
    }
    SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));

    rc = dcmn_get_cnt_reg_values(unit, type, cnt_reg,block_instance,&cntf, &cnt_overflowf, &addrf,&addr_validf);
    SOCDNX_IF_ERR_EXIT(rc);

 



    if (addr_validf != 0) {

        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);
        if(mem!= INVALIDm) {
            SOC_MEM_ALIAS_TO_ORIG(unit,mem);
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
            SOC_MEM_ALIAS_TO_ORIG(unit,mem);
            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                        cntf, cnt_overflowf, addrf, memory_name, numels, index);
            /* set corrective action */
            rc = dcmn_mem_decide_corrective_action(unit, type, mem, block_instance,p_corrective_action, special_msg); 
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

/* Corrective Action main function */
int
jer_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    jer_interrupt_type interrupt_id,
    char *msg,
    dcmn_int_corr_act_type corr_act,
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
        corr_act = DCMN_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
    case DCMN_INT_CORR_ACT_NONE:
        rc = dcmn_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_SOFT_RESET:
        rc = jer_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_HARD_RESET:
        rc = dcmn_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_PRINT:
        rc = dcmn_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_SHADOW:
        rc = dcmn_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET:
        rc = dcmn_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        rc = jer_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_ECC_1B_FIX:
        rc = dcmn_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
        rc = jer_interrupt_handles_corrective_action_handle_oamp_event_fifo(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
        rc = jer_interrupt_handles_corrective_action_handle_oamp_stat_event_fifo(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
        rc = jer_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("UnKnown corrective action")));
  }
   
exit:
  SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

