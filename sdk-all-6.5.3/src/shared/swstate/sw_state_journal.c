/*
 * $Id: $
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
 * INFO: this module implement a Roll-Back Journal for the SW State infrastructure layer,
 *       the roll-back is needed in order to implement Crash Recovery (and generic Error Recovery)
 *
 */

#include <soc/types.h>
#include <soc/error.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <shared/swstate/sw_state_utils.h>
#include <shared/swstate/sw_state_defs.h>

#define SW_STATE_JOURNAL_SAFETY_MARGIN (32)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

/* 
Header:
---------------------------------------------
| size | size left | offset_to_nodeN (last) | 
---------------------------------------------
Journal: 
------------------------------------------------------ 
| data1 | node1 | .... | dataN | nodeN | 00000000.... |
------------------------------------------------------ 
 
nodeN-1 location = nodeN location - nodeN.length   
*/ 
typedef struct sw_state_journal_node_s {
    uint32 offset;
    uint32 length;
    /* followed by data */
} sw_state_journal_node_t;

typedef struct sw_state_journal_s {
    uint32 max_size;
    uint32 size_left;
    uint32 last_node_offset;
} sw_state_journal_t;

sw_state_journal_t *sw_state_journal[BCM_MAX_NUM_UNITS];

int sw_state_journal_init(int unit, uint32 size){
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists;
    uint8                             *scache_ptr = NULL;
    uint32                             total_buffer_size = 0;
    sw_state_journal_node_t           *tmp_node;
    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_JOURNAL);

    total_buffer_size = size; 

    /* allocate (or reallocate) new scache buffer */
    rc = shr_sw_state_scache_ptr_get(unit, 
                                     wb_handle_orig, 
                                     (SOC_WARM_BOOT(unit) ? socSwStateScacheRetreive : socSwStateScacheCreate), 
                                     scache_flags,
                                     &total_buffer_size, &scache_ptr, &already_exists);
    _SOC_IF_ERR_EXIT(rc);
    SOC_NULL_CHECK(scache_ptr);

    if (!SOC_WARM_BOOT(unit)) {
        /*init the journal to be all zeros*/
        sal_memset(scache_ptr, 0x0, total_buffer_size);

        /* init the Journal,
           the first node (bottom of stack) get 0 values and no data attached to it */
        sw_state_journal[unit] = (sw_state_journal_t *) scache_ptr;
        tmp_node = (sw_state_journal_node_t *) ((uint8 *) sw_state_journal[unit] + sizeof(sw_state_journal_t));
        tmp_node->offset = 0;
        tmp_node->length = 0;
        sw_state_journal[unit]->last_node_offset = sizeof(sw_state_journal_t);
        sw_state_journal[unit]->max_size = total_buffer_size - sizeof(sw_state_journal_t) - sizeof(sw_state_journal_node_t);
        sw_state_journal[unit]->size_left = sw_state_journal[unit]->max_size - SW_STATE_JOURNAL_SAFETY_MARGIN;
    }

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_insert(int unit, uint32 offset, uint32 length, uint8 *data){
    soc_scache_handle_t                wb_handle_orig = 0;
    sw_state_journal_node_t           *tmp_node;
    int                                buff_offset;

    SOC_INIT_FUNC_DEFS;

    /* no journalling during warm reboot */
    if (SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    /* verify there is enough space left in the journal */
    if (sw_state_journal[unit]->size_left < length + sizeof(sw_state_journal_node_t)) {
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,
                           (BSL_META_U(unit,
                              "unit:%d SW State Journal is full.\n"), unit));
    }

    /* prepare the new node's location in the journal */
    tmp_node = (sw_state_journal_node_t * ) 
                ((uint8 *) sw_state_journal[unit] + 
                 sw_state_journal[unit]->last_node_offset + 
                 sizeof(sw_state_journal_node_t) + length);

    /* copy the given data into the journal */
    sal_memcpy(((uint8 *) tmp_node) - length, data, length);

    /* save the needed information for restoring the data */
    tmp_node->length = length;
    tmp_node->offset = offset;

    /* update the journal's header */
    sw_state_journal[unit]->last_node_offset += length + sizeof(sw_state_journal_node_t);
    sw_state_journal[unit]->size_left -= length + sizeof(sw_state_journal_node_t);

    /* autosync journal only if autosync is enabled */
    /* autosync must be enabled for journaling (until working over shared memory is implemented) */
    if (SOC_AUTOSYNC_IS_ENABLE(unit)) {
        /* get the wb buffer handle */
        SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_JOURNAL);

        /* get buff_offset */
        buff_offset = (sw_state_journal[unit]->last_node_offset - length); 

        /* perform auto sync on the specific var that has just been set */
        _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, buff_offset, length + sizeof(sw_state_journal_node_t)));
        
        /* perform auto sync on the header */
        _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, 0, sizeof(sw_state_journal_node_t))); 
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_roll_back(int unit, uint8 *block)
{
    sw_state_journal_node_t           *tmp_node;

    SOC_INIT_FUNC_DEFS;

    /* start with last node in the journal */
    tmp_node = (sw_state_journal_node_t *) 
                ((uint8 *) sw_state_journal[unit] + 
                 sw_state_journal[unit]->last_node_offset);

    /* roll back the entire journal */
    while (tmp_node->length != 0 && tmp_node->offset != 0) {
        /* copy the Node's data into it's offset in the block */
        sal_memcpy(block + tmp_node->offset, ((uint8 *) tmp_node) - tmp_node->length, tmp_node->length);
        tmp_node = (sw_state_journal_node_t *) (((uint8 *) tmp_node) - tmp_node->length - sizeof(sw_state_journal_node_t));
    }

    /* it is the caller responsibility to clear the log */

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_clear(int unit)
{
    SOC_INIT_FUNC_DEFS;

    sal_memset(((uint8 *) (sw_state_journal[unit])) + sizeof(sw_state_journal_t) + sizeof(sw_state_journal_node_t),
              0x0,
              sw_state_journal[unit]->max_size - sw_state_journal[unit]->size_left);

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

#endif
