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
 */

#ifndef _SHR_SW_STATE_UTILS_H
#define _SHR_SW_STATE_UTILS_H

#include <soc/types.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/layout/sw_state_layout.h> 

/* scache operations */
typedef enum shr_sw_state_scache_oper_e {
    socSwStateScacheRetreive,
    socSwStateScacheCreate,
    socSwStateScacheRealloc
} shr_sw_state_scache_oper_t;

typedef enum
{
    SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK,
    SHR_SW_STATE_SCACHE_HANDLE_JOURNAL
} SHR_SW_STATE_SCACHE_HANDLES;

#define SW_STATE_NODE_ID_CHECK(unit, node_id) \
    do{ \
        if (node_id >= SW_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES) {\
            _SOC_EXIT_WITH_ERR(BCM_E_FULL, \
                       (BSL_META_U(unit, \
                          "Unit:%d sw state ERROR: the ds_layout_nodes array is full.\n"), unit)); \
        }\
    } while(0)

/*
 *           ------ 
 *          | node |
 *          -------\
 *            |     \-----------------------------|
 *            v                                   v
 *          -------     -------     ------- 
 *          | node |--->| node |--->| node |--> ...
 *          -------     -------     ------- 
 * 
 * ex:
 *          -------
 *          | dpp |
 *          -------
 *            |    \-------------------|
 *            v                        v
 *          -------     -------     --------- 
 *          | bcm  |--->| soc  |--->| shared |
 *          -------     -------     --------- 

 * Note: layout is saved in sw state header (shr_sw_state_data_block_header)
 *       offsets are relative to shr_sw_state_data_block_header address
 *       layout node: an element of a struct
 *       children of a node: they are the element of parent node's struct 
 *       root: *sw_state 
 */


int shr_sw_state_ds_layout_node_t_clear(shr_sw_state_ds_layout_node_t *node); 

int shr_sw_state_ds_layout_node_set(int unit, 
                                    int node_id, 
                                    char* name, 
                                    int size, 
                                    int nof_pointer,
                                    char* type, 
                                    int array_size_0, 
                                    int array_size_1); 
int shr_sw_state_ds_layout_add_child(int unit, int parent_node_id, int child_node_id); 
int shr_sw_state_ds_layout_add_brother(int unit, int older_brother_node_id, int younger_brother_node_id); 
int shr_sw_state_ds_layout_update_offset(int unit, int node_id, int offset); 

int shr_sw_state_utils_init(int unit);
int shr_sw_state_utils_deinit(int unit);

int shr_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, shr_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists);

int shr_sw_state_scache_sync(int unit, soc_scache_handle_t handle, int offset, int size);




#endif  /* _SHR_SW_STATE_UTILS_H */
