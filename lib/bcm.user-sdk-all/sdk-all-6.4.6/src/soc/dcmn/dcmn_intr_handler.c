/*
 * $Id: soc_dcmn_intr_handler.c, v1 16/06/2014 09:55:39 azarrin $
 *
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
 * Purpose:    Implement soc interrupt handler.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>

#include <soc/intr.h>
#include <soc/ipoll.h>

#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dpp/JER/jer_intr_corr_act_func.h>
#include <soc/dpp/JER/jer_intr.h>
#endif
#ifdef BCM_88950
#include <soc/dfe/fe3200/fe3200_intr_cb_func.h>
#include <soc/dfe/fe3200/fe3200_intr_corr_act_func.h>
#include <soc/dfe/fe3200/fe3200_intr.h>
#endif

#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_intr_handler.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*
 *  Interrupt Data Semaphor time out defines
 *      the time out of the deinit interrupts is long because some call-back.
 */
#define SOC_DCMN_INTR_DATA_SEM_TAKE_TO_CB  10000
#define SOC_DCMN_INTR_DATA_SEM_TAKE_TO_DEINIT 20000000
#define REG_FIRST_BLK_TYPE(regblklist) regblklist[0]

/*************
 * TYPE DEFS *
 *************/


/*************
 * FUNCTIONS *
 *************/

int
dcmn_get_cnt_reg_values(
                            int unit,
                            dcmn_memory_dc_t type,
                            soc_reg_t cnt_reg,
                            uint32 *cntf, 
                            uint32 *cnt_overflowf, 
                            uint32 *addrf, 
                            uint32 *addr_validf
                             )
{
    uint64 counter;
    int rc;
    soc_field_t         
        error_addr_valid_field=INVALIDf, 
        error_addr_field=INVALIDf,error_cnt_overflow_field=INVALIDf, 
        error_cnt_field=INVALIDf;
    SOCDNX_INIT_FUNC_DEFS;

    switch (type) {
    case DCMN_ECC_PARITY_DC:
        error_addr_valid_field = PARITY_ERR_ADDR_VALIDf;
        error_addr_field = PARITY_ERR_ADDRf;
        error_cnt_overflow_field = PARITY_ERR_CNT_OVERFLOWf;
        error_cnt_field = PARITY_ERR_CNTf;
        break;

    case DCMN_ECC_ECC2B_DC:
        error_addr_valid_field = ECC_2B_ERR_ADDR_VALIDf;
        error_addr_field = ECC_2B_ERR_ADDRf;
        error_cnt_overflow_field = ECC_2B_ERR_CNT_OVERFLOWf;
        error_cnt_field = ECC_2B_ERR_CNTf;
        break;
    case DCMN_ECC_ECC1B_DC:
        error_addr_valid_field = ECC_1B_ERR_ADDR_VALIDf;
        error_addr_field = ECC_1B_ERR_ADDRf;
        error_cnt_overflow_field = ECC_1B_ERR_CNT_OVERFLOWf;
        error_cnt_field = ECC_1B_ERR_CNTf;
        break;

    case DCMN_P_1_ECC_ECC1B_DC:
        error_addr_valid_field = P_1_ECC_1B_ERR_ADDR_VALIDf;
        error_addr_field = P_1_ECC_1B_ERR_ADDRf;
        error_cnt_overflow_field = P_1_ECC_1B_ERR_CNT_OVERFLOWf;
        error_cnt_field = P_1_ECC_1B_ERR_CNTf;
        break;
    case DCMN_P_1_ECC_ECC2B_DC:
        error_addr_valid_field = P_1_ECC_2B_ERR_ADDR_VALIDf;
        error_addr_field = P_1_ECC_2B_ERR_ADDRf;
        error_cnt_overflow_field = P_1_ECC_2B_ERR_CNT_OVERFLOWf;
        error_cnt_field = P_1_ECC_2B_ERR_CNTf;
        break;

    case DCMN_P_2_ECC_ECC1B_DC:
        error_addr_valid_field = P_2_ECC_1B_ERR_ADDR_VALIDf;
        error_addr_field = P_2_ECC_1B_ERR_ADDRf;
        error_cnt_overflow_field = P_2_ECC_1B_ERR_CNT_OVERFLOWf;
        error_cnt_field = P_2_ECC_1B_ERR_CNTf;
        break;
    case DCMN_P_2_ECC_ECC2B_DC:
        error_addr_valid_field = P_2_ECC_2B_ERR_ADDR_VALIDf;
        error_addr_field = P_2_ECC_2B_ERR_ADDRf;
        error_cnt_overflow_field = P_2_ECC_2B_ERR_CNT_OVERFLOWf;
        error_cnt_field = P_2_ECC_2B_ERR_CNTf;
        break;


    case DCMN_P_3_ECC_ECC1B_DC:
        error_addr_valid_field = P_3_ECC_1B_ERR_ADDR_VALIDf;
        error_addr_field = P_3_ECC_1B_ERR_ADDRf;
        error_cnt_overflow_field = P_3_ECC_1B_ERR_CNT_OVERFLOWf;
        error_cnt_field = P_3_ECC_1B_ERR_CNTf;
        break;
    case DCMN_P_3_ECC_ECC2B_DC:
        error_addr_valid_field = P_3_ECC_2B_ERR_ADDR_VALIDf;
        error_addr_field = P_3_ECC_2B_ERR_ADDRf;
        error_cnt_overflow_field = P_3_ECC_2B_ERR_CNT_OVERFLOWf;
        error_cnt_field = P_3_ECC_2B_ERR_CNTf;
        break;

    default:
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "Unknown type %d"),type));
        rc = SOC_E_NOT_FOUND;
        SOC_EXIT;


    }

    rc = soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &counter);
    SOCDNX_IF_ERR_EXIT(rc);

   /* get address validity bit */
   *addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, error_addr_valid_field);

   /* get memory address bit */
   *addrf = soc_reg64_field32_get(unit, cnt_reg, counter, error_addr_field);

    /* get counter overflow indication  */
   *cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, error_cnt_overflow_field);

   /* get counter value of 2 bit error */
   *cntf = soc_reg64_field32_get(unit, cnt_reg, counter, error_cnt_field);
exit:
    SOCDNX_FUNC_RETURN;


}


int
interrupt_memory_cached(int unit, soc_reg_t mem, int block_instance, int* cached_flag)
{
    int copyno;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cached_flag);

    copyno = (block_instance == SOC_BLOCK_ALL) ? SOC_BLOCK_ALL : block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
    *cached_flag = soc_mem_cache_get(unit, mem, copyno);

exit:
    SOCDNX_FUNC_RETURN;
}



int
dcmn_intr_add_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    soc_handle_interrupt_func inter_action,
    soc_handle_interrupt_func inter_recurring_action)
{
    SOCDNX_INIT_FUNC_DEFS;
    dcmn_intr_add_handler_ext(unit,en_inter,occurrences,timeCycle,inter_action,inter_recurring_action,NULL);

    SOCDNX_FUNC_RETURN;
}


int
dcmn_intr_add_handler_ext(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    soc_handle_interrupt_func inter_action,
    soc_handle_interrupt_func inter_recurring_action,
    char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].recurring_action_cycle_counting = occurrences;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].recurring_action_cycle_time = timeCycle;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].func_arr = inter_action;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].func_arr_recurring_action = inter_recurring_action;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].msg = msg;

    SOCDNX_FUNC_RETURN;
}

STATIC int
dcmn_intr_interrupt_recurring_detect(int unit, int block_instance, uint32 en_interrupt) 
{
    int currentTime;
    int cycleTime, *startCountingTime;
    int cycleCount, *counter;
    soc_handle_interrupt_func func_arr_recurring_action;
    SOCDNX_INIT_FUNC_DEFS;

    currentTime = sal_time(); /*seconds*/

    cycleTime = (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_cycle_time); 
    startCountingTime = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_time);
    func_arr_recurring_action = (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].func_arr_recurring_action);

    cycleCount = (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_cycle_counting);
    counter  = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_counters);

    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            "%s: en_interrupt=%d, currentTime=%d, cycleTime=%d, *startCountingTime=%d, cycleCount=%d, *counter=%d.\n"), FUNCTION_NAME(), en_interrupt, currentTime, cycleTime, *startCountingTime, cycleCount, *counter));

    if (((cycleTime) <= 0) || ((cycleCount) <=0) || (func_arr_recurring_action==NULL)) {
        return 0;
    }

    if (1 == cycleCount) {
        return 1;
    }

    if (currentTime - *startCountingTime > cycleTime) {
        /*init recurring counting*/
        *startCountingTime = currentTime;
        *counter = 1;
        return 0;
    }

    (*counter)++;
    if (*counter >= cycleCount) {
        (*counter) = 0;
        return 1;
    }

    SOCDNX_FUNC_RETURN;
}

void dcmn_intr_switch_event_cb(
    int unit, 
    soc_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata)
{
    int interrupt_id       = arg1;
    int block_instance  = arg2;
    soc_interrupt_db_t* interrupt;
    int nof_interrupts = 0;
    uint32 flags;
    int is_storm_count_period, is_storm_nominal;
    int rc;
    int inter_get;

    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(_rv);

    /*handle only interrupts*/
    if(SOC_SWITCH_EVENT_DEVICE_INTERRUPT != event) {
        return;
    }
    
    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /*
     * Take the interrupt Data semaphor
     */ 
    if(SOC_CONTROL(unit)->interrupts_info->interrupt_sem != NULL) {
        if( sal_sem_take(SOC_CONTROL(unit)->interrupts_info->interrupt_sem, SOC_DCMN_INTR_DATA_SEM_TAKE_TO_CB) != 0) {
            LOG_ERROR(BSL_LS_BCM_INTR,
                      (BSL_META_U(unit,
                                  "Failed to take the interrupt data Semaphor")));
                       return;
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "Interrupt data is uninitialiezed")));
                   return;
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts); 
    SOCDNX_IF_ERR_EXIT(rc);

    if(interrupt_id >= nof_interrupts || interrupt_id < 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);

    /*mask*/
    rc = soc_interrupt_disable(unit, block_instance, interrupt);
    SOCDNX_IF_ERR_EXIT(rc);

    /*storm detection*/
    rc = soc_interrupt_update_storm_detection(unit, block_instance, interrupt);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_interrupt_is_storm(unit, block_instance, interrupt, &is_storm_count_period, &is_storm_nominal);
    SOCDNX_IF_ERR_EXIT(rc);
    if (is_storm_count_period || is_storm_nominal) {
        /* Storm detected: keep the interrupt masked and return from CB */
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            rc = jer_interrupt_print_info(unit, block_instance, interrupt_id, 0, JER_INT_CORR_ACT_NONE, "Storm Detected");
            SOCDNX_IF_ERR_EXIT(rc);
        }
#endif
#ifdef BCM_88950
        if (SOC_IS_FE3200(unit)) {
            rc = fe3200_interrupt_print_info(unit, block_instance, interrupt_id, 0, FE3200_INT_CORR_ACT_NONE, "Storm Detected");
            SOCDNX_IF_ERR_EXIT(rc);
        }
#endif
    }

    /* update statistics */
    rc = soc_interrupt_stat_cnt_increase(unit, block_instance, interrupt_id);
    SOCDNX_IF_ERR_EXIT(rc);

    /*interrupt_action*/
    if (dcmn_intr_interrupt_recurring_detect(unit, block_instance, interrupt_id)) {
        /*recurring action*/
        char *info = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].msg;
        rc = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].func_arr_recurring_action(unit, block_instance, interrupt_id, info);
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        if (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].func_arr!=NULL) {
            char *info = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].msg;
            rc = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].func_arr(unit, block_instance, interrupt_id, info); 
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }

    /*check if should unmask interrupt after handled*/
    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SOCDNX_IF_ERR_EXIT(rc);
    if (flags & SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE) {
        SOC_EXIT;
    }

    /* check if there is an clear function for the interrupt */
    if (NULL == interrupt->interrupt_clear) {
        if (interrupt->vector_id == 0) {
            LOG_WARN(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "Warning: Interrupt not cleared, NULL pointer of interrupt_clear, no vector_id\n")));
        } else {
            LOG_WARN(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "Warning: call to interrupt clear for vector pointer, nothing done\n")));
        }
    } else {
        rc = interrupt->interrupt_clear(unit, block_instance, interrupt_id);
        if(rc!= SOC_E_UNAVAIL) {
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }

    rc = soc_interrupt_get(unit, block_instance, interrupt, &inter_get);
    SOCDNX_IF_ERR_EXIT(rc);

    if (inter_get == 0x0 || (flags & SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK)) {
         if (!(is_storm_count_period || is_storm_nominal)) {
             rc = soc_interrupt_enable(unit, block_instance, interrupt);
             SOCDNX_IF_ERR_EXIT(rc);
         }
    } else {
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            rc = jer_interrupt_print_info(unit, block_instance, interrupt_id, 0, JER_INT_CORR_ACT_NONE, "Interrupt clear Failed!!! Masked");
            SOCDNX_IF_ERR_EXIT(rc);
        }
#endif
#ifdef BCM_88950
        if (SOC_IS_FE3200(unit)) {
            rc = fe3200_interrupt_print_info(unit, block_instance, interrupt_id, 0, FE3200_INT_CORR_ACT_NONE, "Interrupt clear Failed!!! Masked");
            SOCDNX_IF_ERR_EXIT(rc);
        }
#endif
    }

exit:
    if(SOC_UNIT_VALID(unit)) {
        sal_sem_give(SOC_CONTROL(unit)->interrupts_info->interrupt_sem);
    }
    return;
}

int
dcmn_intr_handler_deinit(int unit)
{
    sal_sem_t intr_sem;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_CONTROL(unit)->interrupts_info == NULL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("interrupts wasn't initilized")));
    }

    intr_sem = SOC_CONTROL(unit)->interrupts_info->interrupt_sem;
    if(intr_sem != NULL) {
        if( sal_sem_take(intr_sem, SOC_DCMN_INTR_DATA_SEM_TAKE_TO_DEINIT ) != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to take the interrupt data Semaphor")));
        }
    } 

#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        jer_interrupts_array_deinit(unit); 
    }
#endif
#ifdef BCM_88950
    if (SOC_IS_FE3200(unit)) {
        fe3200_interrupts_array_deinit(unit); 
    }
#endif

    if (intr_sem != NULL) {
        sal_sem_destroy(intr_sem);
    }

exit:     
    SOCDNX_FUNC_RETURN;
}

int
dcmn_intr_handler_init(int unit)
{
    sal_sem_t intr_sem;
    SOCDNX_INIT_FUNC_DEFS;

    intr_sem = sal_sem_create("Interrupt appl data Sem", sal_sem_BINARY, 0);
    if(intr_sem == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOC_MSG("Failed to create the Interrupt Data Semaphor")));     
    }

#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(jer_interrupts_array_init(unit));

        /* Init interrupt CB datad base */
        jer_interrupt_cb_init(unit);
    }
#endif
#ifdef BCM_88950
    if (SOC_IS_FE3200(unit)) {
        SOCDNX_IF_ERR_EXIT(fe3200_interrupts_array_init(unit)); 

        /* Init interrupt CB datad base */
        fe3200_interrupt_cb_init(unit);
    }
#endif

    SOC_CONTROL(unit)->interrupts_info->interrupt_sem = intr_sem;
    if(sal_sem_give(SOC_CONTROL(unit)->interrupts_info->interrupt_sem)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to give the Interrupts Data Semaphor")));;
    }

exit:
    if (SOC_FAILURE(_rv)) {
        sal_sem_destroy(intr_sem);
        SOC_CONTROL(unit)->interrupts_info->interrupt_sem = NULL; 
    }
    /* In case of failure _rv can't be SOC_E_NONE, and hence, after the check of SOC_FAILURE(_rv) the allocation will be freed. */
    /* coverity[leaked_storage:FALSE] */
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME



