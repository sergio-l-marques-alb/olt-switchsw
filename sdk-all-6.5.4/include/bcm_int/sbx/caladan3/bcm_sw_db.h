/*
 * $Id: wb_db_init.h,v 1.1 Broadcom SDK $
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
 * Module: INIT APIs
 *
 * Purpose:
 *     INIT API for Dune Packet Processor devices
 *     Warm boot support
 */

#ifndef __BCM_INT_SBX_CALADAN3_BCM_SW_DB_H__
#define __BCM_INT_SBX_CALADAN3_BCM_SW_DB_H__

#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm_int/common/debug.h>

#ifdef BCM_WARM_BOOT_SUPPORT

/* 
   modules that does not appear in: include/bcm/module.h 
   NOTE:
*/
enum {
    BCM_MODULE_INIT = BCM_MODULE__COUNT,
    BCM_MODULE_ALLOCATOR,
    BCM_MODULE_L2_CACHE,

    /* Make sure you update the BCM_CALADAN3_MODULE_NAMES_INITIALIZER */
    BCM_CALADAN3_MODULE__COUNT
};

#define BCM_CALADAN3_MODULE_NAMES_INITIALIZER               \
{                                                           \
    "init",                                                 \
    "allocator",                                            \
    "l2cache"                                               \
}
/*
 *  Function
 *    bcm_caladan3_module_name
 *  Purpose
 *   Wrapper function to call the bcm_module_name 
 *  Arguments
 *    IN unit = unit number
 *    IN module_num = MODULE 
 *    OUT char *= module name
 */
char *
bcm_caladan3_module_name(int unit, int module_num);

/*
 *  Function
 *    bcm_caladan3_scache_ptr_get
 *  Purpose
 *   Wrapper function to call the soc_caladan3_scache_ptr_get 
 *  Arguments
 *    IN unit = unit number
 *    IN handle = MODULE and SEQUENCE
 *    IN oper = create, retrieve etc..
 *    IN flags = Type of warm boot capability
 *    IN/OUT size = bytes of memory plus header
 *    OUT scache_ptr = pointer to the allocated/retrived memory    
 *    IN/OUT recovered_ver = version being created or retrieved
 *    OUT already_exists = 
 *  Results
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
int bcm_caladan3_scache_ptr_get(int unit, soc_scache_handle_t handle, soc_caladan3_scache_oper_t oper,
                                 int flags, uint32 *size, uint8 **scache_ptr,
                                 uint16 version, uint16 *recovered_ver, int *already_exists);

#endif /* def BCM_WARM_BOOT_SUPPORT */

#endif /* __BCM_INT_SBX_CALADAN3_BCM_SW_DB_H__  */

