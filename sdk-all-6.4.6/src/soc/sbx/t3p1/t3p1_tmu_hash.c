/*
 * $Id: t3p1_tmu_hash.c,v 1.1 Broadcom SDK $
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
 * t3p1_tmu.c: Guadalupe2k V1.3 TMU table manager & wrappers
 *
 */

#include <soc/types.h>
#include <soc/drv.h>
#include <sal/core/boot.h>

#if defined(BCM_CALADAN3_SUPPORT) && defined(BCM_CALADAN3_T3P1_SUPPORT)
#include <soc/sbx/t3p1/t3p1_int.h>
#include <soc/sbx/t3p1/t3p1_tmu.h>
#include <soc/sbx/t3p1/t3p1_defs.h>


int soc_sbx_t3p1_hash_add_ext(int unit, soc_sbx_tmu_hash_handle_t handle, uint32 *key, uint32 *value)
{
    return soc_sbx_caladan3_tmu_hash_entry_add(unit, handle, key, value);
}

int soc_sbx_t3p1_hash_update_ext(int unit, soc_sbx_tmu_hash_handle_t handle, uint32 *key, uint32 *value)
{
    return soc_sbx_caladan3_tmu_hash_entry_update(unit, handle, key, value);
}

int soc_sbx_t3p1_hash_delete_ext(int unit, soc_sbx_tmu_hash_handle_t handle, uint32 *key)
{
    return soc_sbx_caladan3_tmu_hash_entry_delete(unit, handle, key);
}

int soc_sbx_t3p1_hash_get_ext(int unit, soc_sbx_tmu_hash_handle_t handle, uint32 *key, uint32 *value)
{
    return soc_sbx_caladan3_tmu_hash_entry_get(unit, handle, key, value);
}

int soc_sbx_t3p1_hash_hw_get_ext(int unit, soc_sbx_tmu_hash_handle_t handle, uint32 *key, uint32 *value)
{
    return soc_sbx_caladan3_tmu_hash_entry_hw_get(unit, handle, key, value);
}

#endif /* BCM_CALADAN3_SUPPORT */
