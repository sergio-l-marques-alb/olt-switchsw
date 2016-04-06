/*
 * $Id: appl_dcmn_intr.c, v1 16/06/2014 09:55:39 azarrin $
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
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_APPL_INTR

/*************
 * TYPE DEFS *
 *************/
static intr_common_params_t interrupt_common_params[SOC_MAX_NUM_DEVICES];

/*************
 * FUNCTIONS *
 *************/
int
interrupt_appl_ref_log_defaults_set(int unit)
{
     int rv,i;
     soc_interrupt_db_t* interrupts;
     soc_block_types_t  block;
     soc_reg_t reg;
     int blk;
     bcm_switch_event_control_t event;

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))  
#endif
    {
       event.action = bcmSwitchEventLog;
       event.index = 0;
       event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    
       rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
       BCMDNX_IF_ERR_EXIT(rv);

       interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
       for (i=0 ; interrupt_common_params[unit].int_disable_print_on_init[i] != INVALIDr; i++) {
            event.event_id = interrupt_common_params[unit].int_disable_print_on_init[i];  
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
    BCMDNX_FUNC_RETURN;
}

int
interrupt_appl_ref_mask_defaults_set(int unit)
{
     int rv, i;
     soc_interrupt_db_t* interrupts;
     soc_block_types_t  block;
     soc_reg_t reg;
     int blk;
     bcm_switch_event_control_t event;

    BCMDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {

        LOG_ERROR(BSL_LS_APPL_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }
    
    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) 
#endif
    {
        for (i=0 ; interrupt_common_params[unit].int_disable_on_init[i] != INVALIDr; i++) { 
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

        for (i=0 ; interrupt_common_params[unit].int_active_on_init[i] != INVALIDr; i++) {
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
    BCMDNX_FUNC_RETURN;
}

void interrupt_appl_cb(
    int unit, 
    bcm_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata)
{
    if(BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event) {
        return;
    }

    LOG_INFO(BSL_LS_APPL_INTR, (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u\n"), arg1, arg2));
    return;
}

int
interrupt_appl_init(int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    BCMDNX_INIT_FUNC_DEFS;
 
#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)){
        rc = jer_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_FE3200(unit)){
        rc = fe3200_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif
    {
        LOG_ERROR(BSL_LS_APPL_INTR, (BSL_META_U(unit, "ERROR: Interrupt not supported\n")));
        return SOC_E_UNAVAIL;
    }

    rc = bcm_switch_event_register(unit, interrupt_appl_cb, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = interrupt_appl_ref_log_defaults_set(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = interrupt_appl_ref_mask_defaults_set(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Enable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0); 
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}

int interrupt_appl_deinit(int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_switch_event_unregister(unit, interrupt_appl_cb, NULL);
    BCMDNX_IF_ERR_CONT(rc);

    /* Update cached memories to detect ECC and parity errors */
    rc = bcm_switch_control_set(unit, bcmSwitchCacheTableUpdateAll, 0);
    BCMDNX_IF_ERR_CONT(rc); 
 
    /* Disable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x1); 
    BCMDNX_IF_ERR_CONT(rc); 
 
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

