/*
 * $Id: wb_db_counter.h,v 1.1 Broadcom SDK $
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
 * Module: COUNTER warmboot
 *
 * Purpose:
 *     Counter Management Unit
 *     Warm boot support
 */

#ifndef __SOC_SBX_CALADAN3_WB_COUNTER_H__
#define __SOC_SBX_CALADAN3_WB_COUNTER_H__

#include <soc/error.h>
#include <soc/debug.h>
#include <soc/sbx/counter.h>
#include <soc/sbx/caladan3_counter.h>

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 *  Versioning...
 */
#define SOC_SBX_CALADAN3_WB_COUNTER_VERSION_1_0      SOC_SCACHE_VERSION(1, 0)
#define SOC_SBX_CALADAN3_WB_COUNTER_VERSION_CURR     SOC_SBX_CALADAN3_WB_COUNTER_VERSION_1_0


/*
 *  Overall descriptor for VLAN warmboot 
 */
typedef struct soc_sbx_caladan3_wb_counter_state_scache_info_s {
    int                            init_done;
    int                            is_dirty;
    uint16                         version;
    uint8                         *scache_ptr;
    unsigned int                   scache_len;
    uint32                         counter_hw_val_offs;
    uint32                         counter_sw_val_offs;
    uint32                         counter_delta_offs;
} soc_sbx_caladan3_wb_counter_state_scache_info_t;

/*
 *  Function
 *    soc_sbx_caladan3_wb_counter_state_sync
 *  Purpose
 *    Sync the warmboot state for the counter module
 *  Arguments
 *    IN unit = unit number
 *  Results
 *    soc_error_t (cast as int)
 *      SOC_E_NONE for success
 *      SOC_E_* otherwise as appropriate
 *  Notes
 *    Does not report error conditions for many things...
 *
 *    Use this call from anywhere outside of the VLAN module.
 */
int soc_sbx_caladan3_wb_counter_sync(int unit, int arg);

/*
 *  Function
 *    soc_sbx_caladan3_wb_counter_state_init
 *  Purpose
 *    Initialise the warm boot support for counter module.
 *  Arguments
 *    IN unit = unit number
 *  Results
 *    soc_error_t (cast as int)
 *      SOC_E_NONE if success
 *      SOC_E_* otherwise as appropriate
 *  Notes
 */
int soc_sbx_caladan3_wb_counter_state_init(int unit);

#endif /* def BCM_WARM_BOOT_SUPPORT */

#endif /* __SOC_SBX_CALADAN3_WB_DB_COUNTER_H__  */

