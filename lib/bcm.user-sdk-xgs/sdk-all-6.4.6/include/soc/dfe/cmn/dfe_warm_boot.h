/*
 * $Id: dfe_port.h,v 1.5 Broadcom SDK $
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
 * DFE WARM BOOT H
 */

#ifndef _SOC_DFE_WARM_BOOT_H
#define _SOC_DFE_WARM_BOOT_H

/**********************************************************/
/*                     Includes                           */
/**********************************************************/

#include <soc/error.h>
#include <soc/wb_engine.h>

/**********************************************************/
/*                     Defines                            */
/**********************************************************/

#define SOC_DFE_WARM_BOOT_ENGINE            (SOC_WB_ENGINE_PRIMARY)

#define SOC_DFE_WB_ANY                      (0)

/*Single VAR get*/
#define SOC_DFE_WARM_BOOT_VAR_SET(_unit_, _var_, _value_) \
    soc_wb_engine_var_set(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, SOC_DFE_WB_ANY, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_VAR_GET(_unit_, _var_, _value_) \
    soc_wb_engine_var_get(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, SOC_DFE_WB_ANY, (uint8 *) _value_)

/*ARR entry get*/
#define SOC_DFE_WARM_BOOT_ARR_VAR_SET(_unit_, _var_, _inner_index_ , _value_) \
    soc_wb_engine_var_set(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, _inner_index_, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_ARR_VAR_GET(_unit_, _var_, _inner_index_ , _value_) \
    soc_wb_engine_var_get(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, _inner_index_, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_ARR_MEMSET(_unit_, _var_, _value_) \
            SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , _value_);

/*2ARR entry get*/
#define SOC_DFE_WARM_BOOT_2ARR_VAR_SET(_unit_, _var_, _outer_index_, _inner_index_ , _value_) \
    soc_wb_engine_var_set(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , _outer_index_, _inner_index_, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_2ARR_VAR_GET(_unit_, _var_, _outer_index_, _inner_index_ , _value_) \
    soc_wb_engine_var_get(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , _outer_index_, _inner_index_, (uint8 *) _value_)
        
/**********************************************************/
/*                     Enums                              */
/**********************************************************/

/*Buffers*/
/*On a stable stream values should be add at the end*/
typedef enum soc_dfe_warm_boot_buffer_e
{
    SOC_DFE_WARM_BOOT_BUFFER_MODID = 0,
    SOC_DFE_WARM_BOOT_BUFFER_MC = 1,
    SOC_DFE_WARM_BOOT_BUFFER_FIFO = 2,
    SOC_DFE_WARM_BOOT_BUFFER_PORT = 3,
    SOC_DFE_WARM_BOOT_BUFFER_INTR = 4,
    SOC_DFE_WARM_BOOT_BUFFER_ISOLATE = 5,
    SOC_DFE_WARM_BOOT_BUFFER_NOF
} soc_dfe_warm_boot_buffer_t;

/*Vars*/
/*On a stable stream values should be add at the end*/
typedef enum soc_dfe_warm_boot_var_e
{
    SOC_DFE_WARM_BOOT_VAR_MODID_LOCAL_MAP = 0,
    SOC_DFE_WARM_BOOT_VAR_MODID_GROUP_MAP = 1,
    SOC_DFE_WARM_BOOT_VAR_MODID_MODID_TO_GROUP_MAP = 2,

    SOC_DFE_WARM_BOOT_VAR_MC_MODE = 3,
    SOC_DFE_WARM_BOOT_VAR_MC_ID_MAP = 4,

    SOC_DFE_WARM_BOOT_VAR_FIFO_HANDLERS = 5,
    
    SOC_DFE_WARM_BOOT_VAR_PORT_COMMA_BURST_CONF = 6,
    SOC_DFE_WARM_BOOT_VAR_PORT_CTRL_BURST_CONF = 7,
    SOC_DFE_WARM_BOOT_VAR_PORT_CL72_CONF = 8,
    
    SOC_DFE_WARM_BOOT_VAR_INTR_STORM_TIMED_COUNT = 9,
    SOC_DFE_WARM_BOOT_VAR_INTR_STORM_TIMED_PERIOD = 10,
    SOC_DFE_WARM_BOOT_VAR_INTR_STORM_NOMINAL = 11,
    SOC_DFE_WARM_BOOT_VAR_INTR_FLAGS = 12,

    SOC_DFE_WARM_BOOT_VAR_ISOLATE_UNISOLATED_LINKS = 13,
    SOC_DFE_WARM_BOOT_VAR_ISOLATE_ACTIVE_LINKS = 14,
    SOC_DFE_WARM_BOOT_VAR_ISOLATE_ISOLATE_DEVICE = 15,
    SOC_DFE_WARM_BOOT_VAR_ISOLATE_TYPE = 16,

    /*Must be the last one*/
    SOC_DFE_WARM_BOOT_VAR_NOF
} soc_dfe_warm_boot_var_t;


/**********************************************************/
/*                     Functions                          */
/**********************************************************/

soc_error_t soc_dfe_warm_boot_sync(int unit);
soc_error_t soc_dfe_warm_boot_init(int unit);
soc_error_t soc_dfe_warm_boot_deinit(int unit);
soc_error_t soc_dfe_warm_boot_engine_init_buffer_struct(int unit, int buffer_id);
soc_error_t soc_dfe_warm_boot_buffer_id_create(int unit, int buffer_id);

/**********************************************************/
/*                     Constants                          */
/**********************************************************/

#endif /*!_SOC_DFE_WARM_BOOT_H*/
