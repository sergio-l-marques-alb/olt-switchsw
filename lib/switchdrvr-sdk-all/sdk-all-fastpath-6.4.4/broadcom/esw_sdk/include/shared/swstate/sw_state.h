/*
 * $Id: $
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
#ifndef _SOC_SW_STATE_H
#define _SOC_SW_STATE_H

#include <soc/types.h>

#define SOC_MODULE_SW_STATE 100 

#define PARSER_HINT_ARR 
#define PARSER_HINT_PTR 
#define PARSER_HINT_ARR_ARR 
#define PARSER_HINT_ARR_PTR 
#define PARSER_HINT_PTR_ARR 
#define PARSER_HINT_PTR_PTR 

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

typedef enum
{
    SOC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK,
    SOC_SW_STATE_SCACHE_HANDLE_COMMIT_BLOCK
} SOC_SW_STATE_SCACHE_HANDLES;

#define SOC_SW_STATE_IS_SMART_SYNC(_unit)   (0x0)
#define SOC_SW_STATE_MAX_DATA_SIZE_IN_BYTES (100000000)
#define SOC_SW_STATE_MEMBER_NAME_MAX_LENGTH (128)
#define SOC_SW_STATE_SCACHE_CONTROL_SIZE    (0x0) /* currently not using any control */
#define SOC_SW_STATE_MAX_NOF_ALLOCATINS     (10000) /*capacity*/
#define SW_STATE_MAX_NOF_HASH_TABLES        (100)  /* in the future derive from some property ?*/

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_SW_STATE_SCACHE_HANDLE_SET(_wb_handle, _unit, _id)\
    SOC_SCACHE_HANDLE_SET(_wb_handle, _unit, SOC_MODULE_SW_STATE, _id)
#else
#define SOC_SW_STATE_SCACHE_HANDLE_SET(_wb_handle, _unit, _id)
#endif /*BCM_WARM_BOOT_SUPPORT*/

#define SOC_SW_STATE_ALIGN_SIZE(size) \
        (size) = ((size) + 3) & (~3)

/* scache operations */
typedef enum soc_sw_state_scache_oper_e {
    socSwStateScacheRetreive,
    socSwStateScacheCreate,
    socSwStateScacheRealloc
} soc_sw_state_scache_oper_t;


typedef enum soc_sw_state_data_block_init_mode_e {
    socSwStateDataBlockRegularInit,
    socSwStateDataBlockRestoreAndOveride
} soc_sw_state_init_mode_t;

int soc_sw_state_init(int unit, int flags, soc_sw_state_init_mode_t init_mode, uint32 size);
int soc_sw_state_deinit(int unit);
int soc_sw_state_alloc(int unit, uint8** ptr, uint32 size);
int soc_sw_state_free(int unit, uint8 *ptr);
int soc_sw_state_sync(int unit);
int soc_sw_state_data_block_free_restored_data(int unit);
int soc_sw_state_data_block_set(int unit, int start, int end, uint8 *data);
int soc_sw_state_data_block_get(int unit, int start, int end, uint8 *data);
int soc_sw_state_data_block_restored_get(int unit, int start, int end, uint8 *data);
int soc_sw_state_data_block_sync(int unit, int start, int end);

#if AGGRESSIVE_ALLOC_DEBUG_TESTING
int soc_sw_state_nof_allocations_get(void);
#endif /* AGGRESSIVE_ALLOC_DEBUG_TESTING */

#endif /* _SOC_SW_STATE_H */
