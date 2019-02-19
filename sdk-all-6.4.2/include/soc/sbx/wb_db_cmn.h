/*
 * $Id: wb_db_cmn.h,v 1.13 Broadcom SDK $
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
 * File:        wb_db_cmn.h
 * Purpose:     WarmBoot - Level 2 support common file
 */

#ifndef _SOC_SBX_WB_DB_CMN_H_
#define _SOC_SBX_WB_DB_CMN_H_


#include <soc/drv.h>


#if defined(BCM_WARM_BOOT_SUPPORT)




/* RESTORE */
#define SBX_WB_DB_RESTORE_VARIABLE(type, count, var) do {                  \
        (var) = *(type *)SBX_SCACHE_INFO_PTR(unit)->scache_ptr;            \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += sizeof(type);             \
   } while(0)                                            

#define SBX_WB_DB_RESTORE_VARIABLE_OFFSET(type, count, offset, var) do {      \
        (var) = *(type *)(SBX_SCACHE_INFO_PTR(unit)->scache_ptr + (offset));  \
   } while(0)                                            

#define SBX_WB_DB_RESTORE_ARRAY(type, count, var) do {                     \
        for (array_idx=0; array_idx < (count); array_idx++) {                         \
        (var) = *(type *)SBX_SCACHE_INFO_PTR(unit)->scache_ptr;                                   \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += sizeof(type);                                    \
        }                                                                           \
   } while(0)                                            

#define SBX_WB_DB_RESTORE_MEMORY(type, count, var) do {                    \
        sal_memcpy((var), SBX_SCACHE_INFO_PTR(unit)->scache_ptr, ((count)*sizeof(type)));                            \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += ((count)*sizeof(type));                                        \
   } while(0)                                            

#define SBX_WB_DB_RESTORE_MEMORY_OFFSET(type, count, offset, var) do {          \
        sal_memcpy((var), (SBX_SCACHE_INFO_PTR(unit)->scache_ptr + (offset)), ((count)*sizeof(type))); \
   } while(0) 


/* SYNC */
#define SBX_WB_DB_SYNC_VARIABLE(type, count, var) do {                     \
        *(type *)SBX_SCACHE_INFO_PTR(unit)->scache_ptr = (var);                                   \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += sizeof(type);                                    \
   } while(0)                                            

#define SBX_WB_DB_SYNC_VARIABLE_OFFSET(type, count, offset, var) do {                     \
        *(type *)(SBX_SCACHE_INFO_PTR(unit)->scache_ptr + (offset))= (var);                                   \
   } while(0)                                            

#define SBX_WB_DB_SYNC_ARRAY(type, count, var) do {                        \
        for (array_idx=0; array_idx < (count); array_idx++) {                         \
        *(type *)SBX_SCACHE_INFO_PTR(unit)->scache_ptr = (var);                                   \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += sizeof(type);                                    \
        }                                                                           \
   } while(0)                                            

#define SBX_WB_DB_SYNC_MEMORY(type, count, var) do {                       \
        sal_memcpy(SBX_SCACHE_INFO_PTR(unit)->scache_ptr, (var), ((count)*sizeof(type)));                            \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += ((count)*sizeof(type));                                           \
   } while(0)                                            


#define SBX_WB_DB_SYNC_MEMORY_OFFSET(type, count, offset, var) do {      \
        sal_memcpy((SBX_SCACHE_INFO_PTR(unit)->scache_ptr + (offset)), var, ((count)*sizeof(type))); \
   } while(0)                                            


#define SBX_WB_DB_GET_SCACHE_PTR(var) do {                       \
        var = SBX_SCACHE_INFO_PTR(unit)->scache_ptr;             \
    } while(0)

#define SBX_WB_DB_MOVE_SCACHE_PTR(type, count) do {                       \
        SBX_SCACHE_INFO_PTR(unit)->scache_ptr += ((count)*sizeof(type));                                           \
    } while(0)

#define SBX_WB_DB_SYNC_VARIABLE_WITH_PTR(type, count, var, ptr) do {                       \
        *(type *)ptr = (var);                                   \
    } while(0)


/* INIT */
#define SBX_WB_DB_LAYOUT_INIT(type, count, var) do {                       \
        *scache_len += sizeof(type)*(count);                                           \
   } while(0)                                            

#define SBX_WB_DB_LAYOUT_INIT_NV(type, count) do {                       \
        *scache_len += sizeof(type)*(count);                                           \
   } while(0)                                            




#define SBX_WB_DEV_DIRTY_BIT_IS_SET(unit)                                    \
                        (SOC_CONTROL(unit)->scache_dirty == 1)                        \

#define SBX_WB_DEV_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        SOC_CONTROL_UNLOCK(unit);

#define SBX_WB_DEV_DIRTY_BIT_CLEAR(unit)                                     \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 0;                          \
                        SOC_CONTROL_UNLOCK(unit);




#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _SOC_SBX_WB_DB_CMN_H_ */
