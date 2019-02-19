/*
 * $Id: wb_db_stack.h,v 1.6 Broadcom SDK $
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
 * File:        wb_db_stack.h
 * Purpose:     WarmBoot - Level 2 support (STACK Module)
 */

#ifndef _BCM_DPP_WB_DB_STACK_H_
#define _BCM_DPP_WB_DB_STACK_H_


#include <bcm_int/dpp/stack.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

typedef struct bcm_dpp_wb_stack_config_s {

    int _sysport_erp[_DPP_STACK_MAX_DEVICES];
    uint32 _modid_to_domain[_DPP_STACK_MAX_TM_DOMAIN][_DPP_STACK_MAX_DEVICES_UINT32_BITMAP];
    uint32 _domain_to_stk_trunk[_DPP_STACK_MAX_TM_DOMAIN][_DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP];

} bcm_dpp_wb_stack_config_t; 

extern bcm_dpp_wb_stack_config_t bcm_dpp_wb_stack_config[BCM_MAX_NUM_UNITS];

int
_bcm_dpp_wb_stack__sysport_erp_get(int unit, int *_sysport_erp, uint32 indx1);
int
_bcm_dpp_wb_stack__sysport_erp_set(int unit, int _sysport_erp, uint32 indx1);

uint32*
_bcm_dpp_wb_stack__modid_to_domain_get(int unit, uint32 domain);
int
_bcm_dpp_wb_stack__modid_to_domain_set(int unit, uint32 domain, uint32 modid, uint32 val);

uint32*
_bcm_dpp_wb_stack__stk_trunk_to_domains_get(int unit, uint32 stk_trunk);
int
_bcm_dpp_wb_stack__stk_trunk_to_domain_set(int unit, uint32 stk_trunk, uint32 domain, uint32 val);


#if defined(BCM_WARM_BOOT_SUPPORT)

/*
 * Functions
 */
extern int
_bcm_dpp_wb_stack_state_init(int unit);

extern int
_bcm_dpp_wb_stack_state_deinit(int unit);

extern int
_bcm_dpp_wb_stack_sync(int unit);

extern int
_bcm_dpp_wb_stack_update__sysport_erp_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_stack_update__modid_to_domain_state(int unit, uint32 arr_indx1, uint32 arr_indx2);

extern int
_bcm_dpp_wb_stack_update__domain_to_stk_trunk_state(int unit, uint32 arr_indx1, uint32 arr_indx2);


#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_STACK_H_ */
