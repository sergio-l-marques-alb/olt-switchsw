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
#include <appl/dcmn/interrupts/interrupt_handler.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/mbcm.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_dram.h> 
#include <soc/dpp/ARAD/arad_tbl_access.h>
#endif

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#include <appl/dcmn/rx_los/rx_los.h>
#endif

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>


#include <soc/dcmn/dcmn_intr_corr_act_func.h>
#include <soc/dcmn/dcmn_ser_correction.h>
#include <soc/dcmn/dcmn_mem.h>


/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

#define ARAD_MBMP_IS_DYNAMIC(_mem)       _SHR_PBMP_MEMBER(arad_mem_is_dynamic_bmp[_mem/32], _mem%32)

soc_mem_t jer_em_mem[] = {
    PPDB_B_LARGE_EM_KEYT_PLDT_Hm,
    PPDB_A_OEMA_KEYT_PLDT_Hm,
    PPDB_A_OEMB_KEYT_PLDT_Hm,
    IHB_ISEM_KEYT_PLDT_Hm,
    OAMP_REMOTE_MEP_EXACT_MATCH_KEYT_PLDT_Hm,
    EDB_GLEM_KEYT_PLDT_Hm,
    EDB_ESEM_KEYT_PLDT_Hm
};

/*************
 * FUNCTIONS *
 *************/

STATIC
int
dcmn_mem_is_em(int unit, soc_mem_t mem)
{
    int index;

    if (!SOC_IS_JERICHO(unit)) {
        return 0;
    }

    for (index = 0; jer_em_mem[index] != NUM_SOC_MEM; index++) {
        if (jer_em_mem[index] == mem) {
            return 1;
        }
    }

    return 0;
}

STATIC
soc_mem_severe_t
get_mem_severity(int unit, soc_mem_t mem)
{
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        return SOC_MEM_FIELD_VALID(unit,mem,ECCf) ? SOC_MEM_SEVERE_HIGH : SOC_MEM_SEVERE_LOW;
    }

    return SOC_MEM_SEVERITY(unit,mem);
}

int
dcmn_mem_decide_corrective_action(int unit,dcmn_memory_dc_t type,soc_mem_t mem,int copyno, dcmn_int_corr_act_type *action_type, char* special_msg)
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
    this little bit outdated for more accurate scheme look in 
    confluence.broadcom.com/display/DNXSW/SER 


    */
    soc_mem_severe_t severity = get_mem_severity(unit,mem);
    uint32 dynamic_mem ;
    uint32 mem_is_accessible ;
    int cache_enable;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;
    dynamic_mem =  dcmn_tbl_is_dynamic(unit,mem);

    mem_is_accessible = !(soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) );



    if (dynamic_mem) {
        switch (type) {

        case DCMN_ECC_ECC1B_DC:
        case DCMN_P_1_ECC_ECC1B_DC:
        case DCMN_P_2_ECC_ECC1B_DC:
        case DCMN_P_3_ECC_ECC1B_DC:
            if (SOC_IS_DFE(unit) || dcmn_mem_is_em(unit, mem)) {
                *action_type = DCMN_INT_CORR_ACT_ECC_1B_FIX;
            } else {
                *action_type = DCMN_INT_CORR_ACT_NONE;
            }
            SOC_EXIT;
        default:
            if (severity == SOC_MEM_SEVERE_HIGH) {
                *action_type = DCMN_INT_CORR_ACT_SOFT_RESET;
                SOC_EXIT;
            }
        }
        *action_type = DCMN_INT_CORR_ACT_NONE;
        SOC_EXIT;
    }

    /* static mem*/
    rc = interrupt_memory_cached(unit, mem, copyno, &cache_enable);
    if(rc != SOC_E_NONE) {
        sal_sprintf(special_msg, "mem %s failed to check cachebaility \n",
                    SOC_MEM_NAME(unit, mem));
    }
    SOCDNX_IF_ERR_EXIT(rc);
    if (mem_is_accessible) {
        switch (type) {

        case DCMN_ECC_ECC1B_DC:
        case DCMN_P_1_ECC_ECC1B_DC:
        case DCMN_P_2_ECC_ECC1B_DC:
        case DCMN_P_3_ECC_ECC1B_DC:
            if (cache_enable && SOC_MEM_FIELD_VALID(unit, mem, ECCf)) {
                *action_type =  DCMN_INT_CORR_ACT_SHADOW;
            } else {
                *action_type =  DCMN_INT_CORR_ACT_ECC_1B_FIX;
            }

            SOC_EXIT;
        default:
            if (!cache_enable) {
                sal_sprintf(special_msg, "mem %s is not cached \n",
                            SOC_MEM_NAME(unit, mem));
                *action_type =  DCMN_INT_CORR_ACT_HARD_RESET;
                SOC_EXIT;
            }
            switch (severity) {
            case SOC_MEM_SEVERE_HIGH:
                *action_type =  DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET;
                SOC_EXIT;
            default:
                *action_type = DCMN_INT_CORR_ACT_SHADOW ;
                SOC_EXIT;
            }
    } } else {

            switch (type) {

            case DCMN_ECC_ECC1B_DC:
            case DCMN_P_1_ECC_ECC1B_DC:
            case DCMN_P_2_ECC_ECC1B_DC:
            case DCMN_P_3_ECC_ECC1B_DC:

                *action_type =  DCMN_INT_CORR_ACT_NONE;
                SOC_EXIT;
            default:
                *action_type =  DCMN_INT_CORR_ACT_HARD_RESET;
            }



    }

    exit:
        SOCDNX_FUNC_RETURN;
}

int 
dcmn_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry=0;
    int entry_dw;
    int current_index;
    soc_mem_t mem;
    int copyno;

    SOCDNX_INIT_FUNC_DEFS;

    mem = shadow_correct_info_p->mem;

    copyno = (shadow_correct_info_p->copyno == COPYNO_ALL) ? SOC_MEM_BLOCK_ANY(unit, mem) : shadow_correct_info_p->copyno;

    SOCDNX_NULL_CHECK(shadow_correct_info_p);
 
    if (dcmn_tbl_is_dynamic(unit, mem)) {
        SOC_EXIT;
    }

    if (!soc_mem_cache_get(unit, mem, copyno)   && mem != IRR_MCDBm) {
         SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
 
    entry_dw = soc_mem_entry_words(unit, mem);
    data_entry = sal_alloc((entry_dw * 4), "ARAD_INTERRUPT Shadow data entry allocation");

    if (NULL == data_entry) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    
    for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {

        rc = dcmn_get_ser_entry_from_cache(unit,   mem, copyno, 
                                           shadow_correct_info_p->array_index, current_index, data_entry);
       SOCDNX_IF_ERR_EXIT(rc);

        rc = soc_mem_array_write(unit, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, data_entry);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOCDNX_FUNC_RETURN;
}


int 
dcmn_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* ecc_1b_correct_info_p,
    char* msg)
{
    int rc;
    uint32* data_entry=0;
    int entry_dw;
    int current_index;
    int cache_enable;
    soc_reg_above_64_val_t value, orig_value;
    int copyno;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ecc_1b_correct_info_p);
    copyno = ecc_1b_correct_info_p->copyno - SOC_MEM_BLOCK_MIN(unit, ecc_1b_correct_info_p->mem);
    rc = interrupt_memory_cached(unit, ecc_1b_correct_info_p->mem, block_instance, &cache_enable);

    if(rc != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit,"Couldnt decide cache state for %s \n"),
                    SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
        SOC_EXIT;
     }
                  
   

    if (SOC_IS_DPP(unit) && (dcmn_tbl_is_dynamic(unit, ecc_1b_correct_info_p->mem) && !dcmn_mem_is_em(unit, ecc_1b_correct_info_p->mem))) {
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
    SOCDNX_IF_ERR_EXIT(dcmn_disable_block_ecc_check(unit,interrupt_id, ecc_1b_correct_info_p->mem,copyno,value,orig_value));

    for (current_index = ecc_1b_correct_info_p->min_index ; current_index <= ecc_1b_correct_info_p->max_index ; current_index++) {
        

        if (cache_enable) {
            rc = dcmn_get_ser_entry_from_cache(unit,   ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->copyno, 
                                               ecc_1b_correct_info_p->array_index, current_index, data_entry);
        } else {
            rc = soc_mem_array_read(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->array_index, ecc_1b_correct_info_p->copyno, current_index, data_entry);
        }
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
    SOCDNX_IF_ERR_EXIT(dcmn_disable_block_ecc_check(unit,interrupt_id,ecc_1b_correct_info_p->mem,copyno,orig_value,NULL));

exit:
    if(data_entry != NULL) {
        sal_free(data_entry);
    }
    SOCDNX_FUNC_RETURN;
}

int dcmn_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_device_reset(unit, SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DCMN_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int dcmn_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg)
{
    int rc = 0;
#if defined(BCM_DFE_SUPPORT) && defined(INCLUDE_INTR)
    bcm_pbmp_t rx_los_pbmp;
    int rx_los_enable;
#endif

    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: Hard reset!\n")));
#endif

#ifdef BCM_DPP_SUPPORT
    if (SOC_IS_DPP(unit)) {
        rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
        SOCDNX_IF_ERR_EXIT(rc);
    }
#endif

#if defined(BCM_DFE_SUPPORT) && defined(INCLUDE_INTR)
    if (SOC_IS_DFE(unit)) {
        if (SOC_IS_FE1600(unit)) {
            /*RX LOS App disable - required in order to run soft reset without traffic loss*/
            rc = rx_los_enable_get(unit, &rx_los_pbmp, &rx_los_enable);
            SOCDNX_IF_ERR_EXIT(rc);

            if (rx_los_enable) {
                rc = rx_los_unit_detach(unit);
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }

        rc = soc_init(unit);
        SOCDNX_IF_ERR_EXIT(rc);

        if (SOC_IS_FE1600(unit)) {
            /*RX LOS App enable - without resetting RX LOS - required in order to run soft reset without traffic loss*/
            if (rx_los_enable) {
                rc = rx_los_unit_attach(unit, rx_los_pbmp, 1);
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }
    }
#endif

exit:
    SOCDNX_FUNC_RETURN;
}

int
dcmn_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  uint32 interrupt_id,
  char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*empty function*/

    SOCDNX_FUNC_RETURN;
}

int
dcmn_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    dcmn_int_corr_act_type corr_act,
    char *special_msg)
{
    int rc;
    uint32 flags;
    soc_interrupt_db_t* interrupt;

    char cur_special_msg[DCMN_INTERRUPT_SPECIAL_MSG_SIZE];
    char cur_corr_act_msg[DCMN_INTERRUPT_COR_ACT_MSG_SIZE];
    char print_msg[DCMN_INTERRUPT_PRINT_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device")));
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt]);

    if (special_msg == NULL) {
        sal_snprintf(cur_special_msg, sizeof(cur_special_msg), "None");
    } else {
        sal_strncpy(cur_special_msg, special_msg, sizeof(cur_special_msg) - 1);
    }

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
    SOCDNX_IF_ERR_EXIT(rc);

    /* Corrective action will be performed only if BCM_AND_USR_CB flag set or corrective action override flag not overriden */
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = DCMN_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
        case DCMN_INT_CORR_ACT_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset");
            break;
        case DCMN_INT_CORR_ACT_NONE:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "None");
            break;
        case DCMN_INT_CORR_ACT_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Reset");
            break;
        case DCMN_INT_CORR_ACT_PRINT:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Print");
            break;
        case DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Event Fifo");
            break;
        case DCMN_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Statistics Event Fifo");
            break;
        case DCMN_INT_CORR_ACT_SHADOW:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow");
            break;
        case DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown link");
            break;
        case DCMN_INT_CORR_ACT_CLEAR_CHECK:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Clear Check");
            break;
        case DCMN_INT_CORR_ACT_CONFIG_DRAM:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Config DRAM");
            break;
        case DCMN_INT_CORR_ACT_ECC_1B_FIX:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "ECC 1b Correct");
            break;
        case DCMN_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY:
        case DCMN_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY:
        case DCMN_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY:
        case DCMN_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "EM Soft Recovery");
            break;
        case DCMN_INT_CORR_ACT_FORCE:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Force");
            break;
        case DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Handle CRC Delete Buffer FIFO");
            break;
        case DCMN_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Handle MACT Event FIFO");
            break;
        case DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset without Fabric");
            break;
        case DCMN_INT_CORR_ACT_INGRESS_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Ingress Hard Reset");
            break;
        case DCMN_INT_CORR_ACT_IPS_QDESC:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "IPS QDESC Clear Unused");
            break;
        case DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Reprogram Resource");
            break;
        case DCMN_INT_CORR_ACT_RTP_LINK_MASK_CHANGE:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "RTP Link Mask Change");
            break;
        case DCMN_INT_CORR_ACT_RX_LOS_HANDLE:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "RX LOS Handle");
            break;
        case DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow and Soft Reset");
            break;
        case DCMN_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown Unreachable Destination");
            break;
        case DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "TCAM Shadow from SW DB");
            break;
        case DCMN_INT_CORR_ACT_RTP_SLSCT:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "RTP SLSCT");
            break;
        case DCMN_INT_CORR_ACT_SHUTDOWN_LINKS:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown links");
            break;
        case DCMN_INT_CORR_ACT_MC_RTP_CORRECT:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "MC RTP Correct");
            break;
        case DCMN_INT_CORR_ACT_UC_RTP_CORRECT:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "UC RTP Correct");
            break;
        case DCMN_INT_CORR_ACT_ALL_REACHABLE_FIX:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "All Rechable fix");
            break;
        default:
            sal_snprintf(cur_corr_act_msg, DCMN_INTERRUPT_COR_ACT_MSG_SIZE, "Unknown");
    }

    /* prepare string for print */
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, DCMN_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ",
                en_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#else
    sal_snprintf(print_msg, DCMN_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ",
                interrupt->name, en_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#endif

    /* Print per interrupt mechanism */
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
dcmn_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char* msg_print,
    char* msg)
{
    soc_interrupt_db_t* interrupt;
    char print_msg[DCMN_INTERRUPT_PRINT_MSG_SIZE];

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
    sal_snprintf(print_msg, DCMN_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d | %s\n ",
                interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#else
    sal_snprintf(print_msg, DCMN_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d | %s\n ",
                interrupt->name, interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#endif

    LOG_ERROR(BSL_LS_BCM_INTR,
              (BSL_META_U(unit,
                          "%s"),
               print_msg));

exit:
    SOCDNX_FUNC_RETURN;
}

#ifdef BCM_DFE_SUPPORT
int
dcmn_interrupt_handles_corrective_action_for_rtp_slsct(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg)
{

    uint32 slsct_val = 0, score_slsct;
    soc_field_t scrub_en;
    uint64 reg_val64;
    int current_index;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(shadow_correct_info_p);

    if(SOC_DFE_CONFIG(unit).fabric_load_balancing_mode == soc_dfe_load_balancing_mode_normal)
    {
        /* Table is unused */
        /* Write 0 to the entry */
        for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {
            rc = soc_mem_array_write(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, &slsct_val);
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }
    else if(SOC_DFE_CONFIG(unit).fabric_load_balancing_mode == soc_dfe_load_balancing_mode_balanced_input)
    {
        /* Table is used */
        /* reset the scrubber */
        SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, &reg_val64));
        scrub_en = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, SCT_SCRUB_ENABLEf);
        soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val64));

        SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, scrub_en);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val64));

    }
    else if(SOC_DFE_CONFIG(unit).fabric_load_balancing_mode == soc_dfe_load_balancing_mode_destination_unreachable)
    {
        /* Table is used */
        /* write max value to the entry */
        score_slsct = SOC_DFE_DRV_MULTIPLIER_MAX_LINK_SCORE;
        soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, LINK_NUMf, &score_slsct);

        for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {
            rc = soc_mem_array_write(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, &slsct_val);
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }
    else
    {
        /* Unexpected load balancing mode */
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#undef _ERR_MSG_MODULE_NAME

