
/*
 * $Id: interrupt_handler.c,v 1.18 Broadcom SDK $
 *
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
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR

/* 
 *  include  
 */ 
#include <shared/bsl.h>
#include <sal/core/time.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dpp/drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_port.h>

#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <appl/dpp/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler_cb_func.h>
#include <appl/dpp/interrupts/interrupt_handler_cb_func.h>
#include <appl/dfe/interrupts/interrupt_handler_corr_act_func.h>
#include <appl/dpp/interrupts/interrupt_handler_corr_act_func.h>

/*
 *  Interrupt Data Semaphor time out defines
 *      the time out of the deinit interrupts is long because some call-back.
 */
#define APPL_DCMN_INTR_DATA_SEM_TAKE_TO_DEINIT 20000000
#define APPL_INTR_DATA_SEM_TAKE_TO_CB  10000

interrupt_handle_data_base_t interrupt_data_base[SOC_MAX_NUM_DEVICES];
interrupt_common_params_t interrupt_common_params[SOC_MAX_NUM_DEVICES];

int
interrupt_add_interrupt_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    handle_interrupt_func inter_action,
    handle_interrupt_func inter_recurring_action)
{
    SOCDNX_INIT_FUNC_DEFS;

    interrupt_data_base[unit].recurring_action_cycle_counting[en_inter] = occurrences;
    interrupt_data_base[unit].recurring_action_cycle_time[en_inter] = timeCycle;
    interrupt_data_base[unit].func_arr[en_inter] = inter_action;
    interrupt_data_base[unit].func_arr_recurring_action[en_inter] = inter_recurring_action;

    SOCDNX_FUNC_RETURN;
}

int
interrupt_config_interrupt_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    handle_interrupt_func inter_action,
    handle_interrupt_func inter_recurring_action)
{
    SOCDNX_INIT_FUNC_DEFS;

    if(-1 != occurrences) {
        interrupt_data_base[unit].recurring_action_cycle_counting[en_inter] = occurrences;
    }

    if(-1 != timeCycle) {
        interrupt_data_base[unit].recurring_action_cycle_time[en_inter] = timeCycle;
    }

    if(NULL != inter_action) {
        interrupt_data_base[unit].func_arr[en_inter] = inter_action;
    }
    if(NULL != inter_recurring_action) {
        interrupt_data_base[unit].func_arr_recurring_action[en_inter] = inter_recurring_action;
    }

    SOCDNX_FUNC_RETURN;
}


int
handle_interrupt_recurring_detect(int unit, int block_instance, uint32 en_interrupt) 
{
    int currentTime;
    int cycleTime, *startCountingTime;
    int cycleCount, *counter;

    SOCDNX_INIT_FUNC_DEFS;

    currentTime = sal_time(); /*seconds*/

    cycleTime = (interrupt_data_base[unit].recurring_action_cycle_time[en_interrupt]); 
    startCountingTime = &(interrupt_data_base[unit].recurring_action_time[en_interrupt]);

    cycleCount = (interrupt_data_base[unit].recurring_action_cycle_counting[en_interrupt]);
    counter  = &(interrupt_data_base[unit].recurring_action_counters[en_interrupt]);

    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            "%s: en_interrupt=%d, currentTime=%d, cycleTime=%d, *startCountingTime=%d, cycleCount=%d, *counter=%d.\n"), FUNCTION_NAME(), en_interrupt, currentTime, cycleTime, *startCountingTime, cycleCount, *counter));

    if (((cycleTime) <= 0) || ((cycleCount) <=0)) {
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


void
diag_switch_event_cb(
    int unit, 
    bcm_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata)
{
    int interrupt_id       = arg1;
    int block_instance  = arg2;
    soc_interrupt_db_t* interrupt;

    uint32 flags;

    int is_storm_count_period, is_storm_nominal;
    bcm_switch_event_control_t event_bcm_switch_event_mask, event_bcm_switch_event_clear; /*will be cfg after verifying interrupt_id*/
    int rc;
    int inter_get;

    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(_rv);

    /*handle only interrupts*/
    if(BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event) {
        return;
    }
    

    /*
     * Take the interrupt Data semaphor
     */ 
    if(interrupt_data_base[unit].interrupts_data_sem != NULL) {
        if( sal_sem_take(interrupt_data_base[unit].interrupts_data_sem, APPL_INTR_DATA_SEM_TAKE_TO_CB) != 0) {
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

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }
    
    if(interrupt_id >= interrupt_common_params[unit].nof_interrupts || interrupt_id < 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);
    if(NULL == interrupt) {
        BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_BCM_MSG("No interrupts for device")));
    }

    /*set mask event*/
    event_bcm_switch_event_mask.action = bcmSwitchEventMask;
    event_bcm_switch_event_mask.event_id = interrupt_id;
    event_bcm_switch_event_mask.index = block_instance;
    /*mask*/
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_mask, 1); 
    SOCDNX_IF_ERR_EXIT(rc);

    /*storm detection*/
    rc = soc_interrupt_update_storm_detection(unit, block_instance, interrupt);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_interrupt_is_storm(unit, block_instance, interrupt, &is_storm_count_period, &is_storm_nominal);
    SOCDNX_IF_ERR_EXIT(rc);
    if (is_storm_count_period || is_storm_nominal) {
        /* Storm detected: keep the interrupt masked and return from CB */
        if (SOC_IS_FE1600(unit)) {
#ifdef BCM_DFE_SUPPORT
            rc = fe1600_interrupt_print_info(unit, block_instance, interrupt_id, 0, FE1600_INT_CORR_ACT_NONE, "Storm Detected");
            SOCDNX_IF_ERR_EXIT(rc);
#endif
        } else {
#ifdef BCM_ARAD_SUPPORT
            rc = arad_interrupt_print_info(unit, block_instance, interrupt_id, 0, ARAD_INT_CORR_ACT_NONE, "Storm Detected");
            SOCDNX_IF_ERR_EXIT(rc);
#endif
        }
    }

    /* update statistics */
    rc = soc_interrupt_stat_cnt_increase(unit, block_instance, interrupt_id);
    SOCDNX_IF_ERR_EXIT(rc);

    /*interrupt_action*/
    if (handle_interrupt_recurring_detect(unit, block_instance, interrupt_id)) {
        /*recurring action*/
        rc = interrupt_data_base[unit].func_arr_recurring_action[interrupt_id](unit, block_instance, interrupt_id, NULL);
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        rc = interrupt_data_base[unit].func_arr[interrupt_id](unit, block_instance, interrupt_id, NULL);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    /*check if should unmask interrupt after handled*/
    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SOCDNX_IF_ERR_EXIT(rc);
    if (flags & SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE) {
        SOC_EXIT;
    }

    /*set clear event*/
    event_bcm_switch_event_clear.event_id = interrupt_id;
    event_bcm_switch_event_clear.index = block_instance;

    event_bcm_switch_event_clear.action = bcmSwitchEventClear;
    rc = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_clear, 1);
    if(rc!= SOC_E_UNAVAIL) {
        SOCDNX_IF_ERR_EXIT(rc);
    }


    rc = soc_interrupt_get(unit, block_instance, interrupt, &inter_get);
    SOCDNX_IF_ERR_EXIT(rc);

    if (inter_get == 0x0 || (flags & SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK)) {
         if (!(is_storm_count_period || is_storm_nominal)) {
             rc = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_mask, 0); 
             SOCDNX_IF_ERR_EXIT(rc);
         }
    } else {
        if (SOC_IS_FE1600(unit)) {
#ifdef BCM_DFE_SUPPORT
            rc = fe1600_interrupt_print_info(unit, block_instance, interrupt_id, 0, FE1600_INT_CORR_ACT_NONE, "Interrupt clear Failed!!! Masked");
            SOCDNX_IF_ERR_EXIT(rc);
#endif
        } else {
#ifdef BCM_ARAD_SUPPORT
            rc = arad_interrupt_print_info(unit, block_instance, interrupt_id, 0, ARAD_INT_CORR_ACT_NONE, "Interrupt clear Failed!!! Masked");
            SOCDNX_IF_ERR_EXIT(rc);
#endif
        }
    }

exit:
    if(SOC_UNIT_VALID(unit)) {
        sal_sem_give(interrupt_data_base[unit].interrupts_data_sem);
    }
    return;
}

int
interrupt_handler_data_base_init(int unit)
{

    fe1600_interrupt_type index;
    int cur_time;
    handle_interrupt_func interrupt_handle_genenric_none, interrupt_recurring_action_handle_generic_none;
    uint32 nof_interrupts;

    SOCDNX_INIT_FUNC_DEFS;

    interrupt_data_base[unit].interrupts_data_sem = sal_sem_create("Interrupt appl data Sem", sal_sem_BINARY, 0);   
    if(interrupt_data_base[unit].interrupts_data_sem == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOC_MSG("Failed to create the Interrupt Data Semaphor")));     
    }
 
    if (SOC_IS_FE1600(unit)) {
        interrupt_handle_genenric_none = fe1600_interrupt_handle_generic_none;
        interrupt_recurring_action_handle_generic_none = fe1600_interrupt_recurring_action_handle_generic_none;
    } else if (SOC_IS_ARAD(unit)){
        interrupt_handle_genenric_none = arad_interrupt_handle_generic_none;
        interrupt_recurring_action_handle_generic_none = arad_interrupt_recurring_action_handle_generic_none;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    nof_interrupts = interrupt_common_params[unit].nof_interrupts;

    interrupt_data_base[unit].func_arr = sal_alloc( sizeof(handle_interrupt_func) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].func_arr_recurring_action =sal_alloc( sizeof(handle_interrupt_func) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_counters = sal_alloc( sizeof(int) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_time = sal_alloc( sizeof(int) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_cycle_time = sal_alloc( sizeof(int) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_cycle_counting = sal_alloc( sizeof(int) * nof_interrupts, "interrupt_handler" );

    cur_time = sal_time();
    
    /*init recurring_action_unit*/
    for (index = 0; index < nof_interrupts; index++) {
        interrupt_data_base[unit].func_arr[index] = interrupt_handle_genenric_none; /*do nothing by default*/
        interrupt_data_base[unit].func_arr_recurring_action[index] = interrupt_recurring_action_handle_generic_none; /*do nothing by default*/
        interrupt_data_base[unit].recurring_action_cycle_time[index] = -1; /*without recurring action by default*/
        interrupt_data_base[unit].recurring_action_cycle_counting[index] = -1;
        interrupt_data_base[unit].recurring_action_time[index] = cur_time;
        interrupt_data_base[unit].recurring_action_counters[index] = 0;
    }
   
    interrupt_common_params[unit].interrupt_add_interrupt_handler_init(unit);  
    
    if(sal_sem_give(interrupt_data_base[unit].interrupts_data_sem)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to give the Interrupts Data Semaphor")));;
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_db_init(int unit)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* Create the common parameters according to Device type */
    if (SOC_IS_FE1600(unit)) {
        rc = fe1600_interrupt_handler_init_cmn_param(unit, &interrupt_common_params[unit]);
        SOCDNX_IF_ERR_EXIT(rc);
    } else if(SOC_IS_ARAD(unit)) {
        rc = arad_interrupt_handler_init_cmn_param(unit, &interrupt_common_params[unit]);
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Interrupt Application not supported for this device"))); 
    }

    /* Init interrupt CB datad base */
    rc = interrupt_handler_data_base_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
interrupt_handler_appl_ref_cache_defaults_set(int unit)
{
     int rc;
     int memory_index, cache_enable;

    SOCDNX_INIT_FUNC_DEFS;

    /* attach cache memory */
    if (interrupt_common_params[unit].cached_mem != NULL) {
        for (memory_index=0; (interrupt_common_params[unit].cached_mem[memory_index]) != NUM_SOC_MEM; memory_index++) {

            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                if ((interrupt_common_params[unit].cached_mem[memory_index] == OAMP_LMM_DA_NIC_TABLEm) ||
                    (interrupt_common_params[unit].cached_mem[memory_index] == OAMP_PE_PROGRAMm) ||
                    (interrupt_common_params[unit].cached_mem[memory_index] == IHB_IPP_LAG_TO_LAG_RANGEm)) {
                    continue;
                }
            }
            if (SOC_IS_ARADPLUS(unit) && !SOC_IS_ARDON(unit)) {
                if (interrupt_common_params[unit].cached_mem[memory_index] == OAMP_MEP_DB_BFD_CC_ON_MPLSTPm) {
                    continue;
                }
            }

            rc = interrupt_memory_could_be_cached(unit, interrupt_common_params[unit].cached_mem[memory_index], &cache_enable); 
            SOCDNX_IF_ERR_EXIT(rc);
  
            if (TRUE == cache_enable) {
                rc = interrupt_attach_cache(unit, interrupt_common_params[unit].cached_mem[memory_index], COPYNO_ALL);
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_ref_log_defaults_set(int unit)
{
     int rv,i;
     bcm_switch_event_control_t event;

    SOCDNX_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))  
#endif
    {
       event.action = bcmSwitchEventLog;
       event.index = 0;
       event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    
       rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
       BCMDNX_IF_ERR_EXIT(rv);

       for (i=0 ; interrupt_common_params[unit].int_disable_print_on_init[i] != interrupt_common_params[unit].nof_interrupts; i++) {
            event.event_id = interrupt_common_params[unit].int_disable_print_on_init[i];  
            event.index = 0;         
            rv=bcm_switch_event_control_set(unit,  BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0);
            BCMDNX_IF_ERR_EXIT(rv);
       }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_ref_mask_defaults_set(int unit)
{
     int rv, i;
     soc_interrupt_db_t* interrupts;
     soc_block_types_t  block;
     soc_reg_t reg;
     int blk;
     bcm_switch_event_control_t event;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }
    
    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) 
#endif
    {
        for (i=0 ; interrupt_common_params[unit].int_disable_on_init[i] != interrupt_common_params[unit].nof_interrupts; i++) { 
            event.event_id = interrupt_common_params[unit].int_disable_on_init[i];
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,1);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        for (i=0 ; interrupt_common_params[unit].int_active_on_init[i] != interrupt_common_params[unit].nof_interrupts; i++) {
            /* ARAD_INT_IDR_MMUECC false alarm at arad_a0. should not be activated.*/
            if (((interrupt_common_params[unit].int_active_on_init[i] == ARAD_INT_IDR_MMUECC_1BERRINT) || 
                 (interrupt_common_params[unit].int_active_on_init[i] == ARAD_INT_IDR_MMUECC_2BERRINT)) && SOC_IS_ARAD_A0(unit)) {
                continue;
            }
            event.event_id = interrupt_common_params[unit].int_active_on_init[i]; 
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_ref_defaults_set(int unit)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    rc = interrupt_handler_appl_ref_cache_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_ref_log_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_ref_mask_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);



exit:
    SOCDNX_FUNC_RETURN;
}


int
interrupt_handler_appl_revent_register(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv =bcm_switch_event_register(unit, diag_switch_event_cb, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_init(int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) { 
        SOC_EXIT;
    }
 
    rc = interrupt_handler_appl_db_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_ref_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_revent_register(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    /* Enable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0); 
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int interrupt_handler_appl_deinit(int unit)
{
    int memory_index;
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SOCDNX_INIT_FUNC_DEFS;

    /* Update cached memories to detect ECC and parity errors */
    rc = bcm_switch_control_set(unit, bcmSwitchCacheTableUpdateAll, 0);
     SOCDNX_IF_ERR_CONT(rc); 
 
    /* Disable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x1); 
    SOCDNX_IF_ERR_CONT(rc); 
 
    if(interrupt_data_base[unit].interrupts_data_sem != NULL) {
        if( sal_sem_take(interrupt_data_base[unit].interrupts_data_sem, APPL_DCMN_INTR_DATA_SEM_TAKE_TO_DEINIT ) != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to take the interrupt data Semaphor")));
        }
    } 
 
    bcm_switch_event_unregister(unit, diag_switch_event_cb, NULL);

    if (interrupt_data_base[unit].func_arr != NULL) {
        sal_free(interrupt_data_base[unit].func_arr);
        interrupt_data_base[unit].func_arr = NULL;
    }
    if (interrupt_data_base[unit].func_arr_recurring_action != NULL) {
        sal_free(interrupt_data_base[unit].func_arr_recurring_action);
        interrupt_data_base[unit].func_arr_recurring_action = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_counters != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_counters);
        interrupt_data_base[unit].recurring_action_counters = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_time != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_time);
        interrupt_data_base[unit].recurring_action_time = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_cycle_time != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_cycle_time);
        interrupt_data_base[unit].recurring_action_cycle_time = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_cycle_counting != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_cycle_counting);
        interrupt_data_base[unit].recurring_action_cycle_counting = NULL;
    }

    /* detach cache */
    if(interrupt_common_params[unit].cached_mem!= NULL) {
        for(memory_index=0; interrupt_common_params[unit].cached_mem[memory_index] != NUM_SOC_MEM; memory_index++) {

            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                if ((interrupt_common_params[unit].cached_mem[memory_index] == OAMP_LMM_DA_NIC_TABLEm) ||
                    (interrupt_common_params[unit].cached_mem[memory_index] == OAMP_PE_PROGRAMm) ||
                    (interrupt_common_params[unit].cached_mem[memory_index] == IHB_IPP_LAG_TO_LAG_RANGEm)) {
                    continue;
                }
            }
            if (SOC_IS_ARADPLUS(unit) && !SOC_IS_ARDON(unit)) {
                if (interrupt_common_params[unit].cached_mem[memory_index] == OAMP_MEP_DB_BFD_CC_ON_MPLSTPm) {
                    continue;
                }
            }
            rc = interrupt_detach_cache(unit, interrupt_common_params[unit].cached_mem[memory_index], COPYNO_ALL);
            SOCDNX_IF_ERR_CONT(rc);
        }
    }

    if (interrupt_data_base[unit].interrupts_data_sem != NULL) {
        sal_sem_destroy(interrupt_data_base[unit].interrupts_data_sem);
        interrupt_data_base[unit].interrupts_data_sem = NULL; 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


int
interrupt_attach_cache(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;
   
    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {

       SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
       rc = soc_mem_cache_set(unit, mem, block_instance, 1);
       SOCDNX_IF_ERR_EXIT(rc);

    } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
   
        
exit:
    SOCDNX_FUNC_RETURN;

}

int
interrupt_detach_cache(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
  
    SOCDNX_INIT_FUNC_DEFS;
  
    if (soc_mem_is_valid(unit, mem) &&
            ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {
    rc = soc_mem_cache_set(unit, mem, block_instance, 0);
    SOCDNX_IF_ERR_EXIT(rc);

    SOC_MEM_INFO(unit, mem).flags &= (~SOC_MEM_FLAG_CACHABLE);
    }
    else {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_memory_cached(int unit, soc_reg_t mem, int block_instance, int* cached_flag)
{
    int memory_index;
    int cache_found_flag;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cached_flag);
 
    if (soc_mem_is_valid(unit, mem) &&
             ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {
    
        cache_found_flag = FALSE;

        if(interrupt_common_params[unit].cached_mem!= NULL) {
            for(memory_index=0; interrupt_common_params[unit].cached_mem[memory_index]!= NUM_SOC_MEM; memory_index++) {
                if(interrupt_common_params[unit].cached_mem[memory_index] == mem) {
                    cache_found_flag = TRUE;
                    break;
                }
            }
        }
                  
        if( TRUE == cache_found_flag) {
            *cached_flag = TRUE;
        }
        else {
            *cached_flag = FALSE;
        }
    }
    else {
          SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_memory_could_be_cached(int unit, soc_mem_t cached_mem, int* cache_enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cache_enable);

#ifdef BCM_ARAD_SUPPORT
    /* verify that cached memory is not dynamic */
    if (SOC_IS_ARAD(unit) && arad_tbl_is_dynamic(cached_mem)) {
        *cache_enable = FALSE;
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "Trying to cache dynamic memory: %s\n"), SOC_MEM_UFNAME(unit, cached_mem)));
                   SOC_EXIT;
    }
#endif
#ifdef BCM_DFE_SUPPORT
    /* verify that cached memory is not dynamic */
    if (SOC_IS_DFE(unit) && soc_dfe_tbl_is_dynamic(unit, cached_mem)) {
        *cache_enable = FALSE;
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "Trying to cache dynamic memory: %s\n"), SOC_MEM_UFNAME(unit, cached_mem)));
                   SOC_EXIT;
    }
#endif
    if (soc_mem_is_valid(unit, cached_mem) &&
        ((SOC_MEM_INFO(unit, cached_mem).blocks | SOC_MEM_INFO(unit, cached_mem).blocks_hi) != 0)) {
         
        switch(cached_mem) {
        case RTP_SLSCTm :
            if(soc_dfe_load_balancing_mode_destination_unreachable == SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) {
                *cache_enable = TRUE;
            } else {
                *cache_enable = FALSE;
            }
            break;
        default:
            *cache_enable = TRUE;
        }
  
    } else {
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "memory is not valid: %s\n"), SOC_MEM_UFNAME(unit, cached_mem)));
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME


