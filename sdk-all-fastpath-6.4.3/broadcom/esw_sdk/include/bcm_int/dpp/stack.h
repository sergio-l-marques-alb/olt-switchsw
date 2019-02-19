/*
 * $Id: stack.h,v 1.9 Broadcom SDK $
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
 *
 * File:        stack.h
 * Purpose:     STACK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_STACK_H_
#define   _BCM_INT_DPP_STACK_H_

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/PPD/ppd_api_lag.h>

/* Save internal sysport erp for each unit,device */
#define _DPP_STACK_MAX_DEVICES (SOC_TMC_NOF_FAPS_IN_SYSTEM)
#define _DPP_STACK_MAX_DEVICES_UINT32_BITMAP ((_DPP_STACK_MAX_DEVICES / 32) + 1)
#define _DPP_STACK_MAX_TM_DOMAIN (SOC_TMC_NOF_TM_DOMAIN_IN_SYSTEM)
#define _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP ((_DPP_STACK_MAX_DEVICES / 32) + 1)

#define _DPP_STACK_INVALID_DOMAIN 0xffffffff

/* 
 *  Stacknig utility functions.
 */ 
int bcm_petra_stk_modid_to_domain_find(int unit, uint32 local_modid, uint32 sysport_modid, uint32 *is_same_domain);
int bcm_petra_stk_domain_modid_get(int unit, uint32 modid, int *domain);

extern int
_bcm_petra_stk_detach(int unit);

int 
_bcm_petra_stk_trunk_domian_get(
    int unit, 
    int domain,
    int stk_trunk_max,
    bcm_trunk_t *stk_trunk_array,
    int *stk_trunk_count);

int 
__bcm_petra_stk_trunk_domian_fec_map_update(
    int unit, 
    int domain);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_stk_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
      
#endif /* _BCM_INT_PETRA_STACK_H_ */
