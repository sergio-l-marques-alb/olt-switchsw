/*
 * $Id: $
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
 */
#ifndef _SHR_SW_STATE_H
#define _SHR_SW_STATE_H

#include <soc/types.h>
#include <shared/swstate/layout/sw_state_defs_layout.h>

#define SOC_MODULE_SW_STATE 100 

#define PARSER_HINT_ARR 
#define PARSER_HINT_PTR 
#define PARSER_HINT_ARR_ARR 
#define PARSER_HINT_ARR_PTR 
#define PARSER_HINT_PTR_ARR 
#define PARSER_HINT_PTR_PTR 
#define PARSER_HINT_ALLOW_WB_ACCESS

typedef enum
{
    SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK,
    SHR_SW_STATE_SCACHE_HANDLE_COMMIT_BLOCK
} SHR_SW_STATE_SCACHE_HANDLES;

typedef uint8 SW_STATE_BUFF;

#define SHR_SW_STATE_IS_SMART_SYNC(_unit)        (0x0)


#define SHR_SW_STATE_MAX_DATA_SIZE_IN_BYTES      (200000000)
#define SHR_SW_STATE_SCACHE_RESERVE_SIZE         (SOC_IS_JERICHO(unit) ? 5000000 : 20000000)    /* reserve some scache space for legacy wb modules */

#define SHR_SW_STATE_MEMBER_NAME_MAX_LENGTH      (128)
#define SHR_SW_STATE_SCACHE_CONTROL_SIZE         (0x0) /* currently not using any control */
#define SW_STATE_MAX_NOF_HASH_TABLES             (100)  /* in the future derive from some property ?*/

#ifdef BCM_WARM_BOOT_SUPPORT
#define SHR_SW_STATE_SCACHE_HANDLE_SET(_wb_handle, _unit, _id)\
    SOC_SCACHE_HANDLE_SET(_wb_handle, _unit, SOC_MODULE_SW_STATE, _id)
#else
#define SHR_SW_STATE_SCACHE_HANDLE_SET(_wb_handle, _unit, _id)
#endif /*BCM_WARM_BOOT_SUPPORT*/

#define SHR_SW_STATE_ALIGN_SIZE(size) \
        (size) = ((size) + 3) & (~3)

#define INIT_SW_STATE_MEM
#define DISPLAY_SW_STATE_MEM
#define DISPLAY_SW_STATE_MEM_PRINTF(x)
#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#ifdef SW_STATE_MEM_MONITOR
/* { */
void
    sw_state_alloc_resource_usage_get(unsigned int *alloc_curr, unsigned int *alloc_max);
void 
    sw_state_alloc_resource_usage_init(void) ;
#undef DISPLAY_SW_STATE_MEM
/*
 * Display total memory allocated by shr_sw_state_alloc() so far and maximum
 * allocated so far.
 * Note that collection is initialized to zero in shr_sw_state_init() (See INIT_SW_STATE_MEM).
 */
#define DISPLAY_SW_STATE_MEM \
    { \
        uint32 alloc_curr ; \
        uint32 alloc_max ; \
        sw_state_alloc_resource_usage_get(&alloc_curr, &alloc_max) ; \
        sal_printf( \
            "\r\n" \
            "%s(),%d: Sw state memory consumers - curr/max %lu/%lu bytes\r\n\n", \
                            __FUNCTION__,__LINE__,(unsigned long)alloc_curr,(unsigned long)alloc_max) ; \
    }
#undef INIT_SW_STATE_MEM
#define INIT_SW_STATE_MEM sw_state_alloc_resource_usage_init() ;
#undef DISPLAY_SW_STATE_MEM_PRINTF
#define DISPLAY_SW_STATE_MEM_PRINTF(x) sal_printf x ;
/* } */
#endif
/* } */
#endif
/* } */
#endif

/* scache operations */
typedef enum shr_sw_state_scache_oper_e {
    socSwStateScacheRetreive,
    socSwStateScacheCreate,
    socSwStateScacheRealloc
} shr_sw_state_scache_oper_t;


typedef enum shr_sw_state_data_block_init_mode_e {
    socSwStateDataBlockRegularInit,
    socSwStateDataBlockRestoreAndOveride
} shr_sw_state_init_mode_t;



typedef struct shr_sw_state_alloc_element_s {
    uint32 ptr_offset;
    uint8 *ptr_value;
} shr_sw_state_alloc_element_t;


/* a layout node represent an element of the struct.
 * Therefore, it has a size: size of the element's type 
              it has an offset: position of the element in memory, relative to the struct memory position*/
typedef struct shr_sw_state_ds_layout_node_s {
    char   name[50]; 
    int    size; /* size of the struct */
    uint32 offset; /* offset of the element in the struct */
    int    nof_pointer; 
    int    array_sizes[2]; 
    int    first_child_node_index; 
    int    last_child_node_index; 
    int    next_brother_node_index; /* node index to the next brother. */
} shr_sw_state_ds_layout_node_t; 


typedef struct shr_sw_state_data_block_header_s {
    uint32 total_buffer_size;
    shr_sw_state_alloc_element_t *ptr_offsets_sp; /* stack pointer */
    shr_sw_state_alloc_element_t *ptr_offsets_stack_base;
    uint8 *data_ptr;
    uint32 data_size;
    uint8 *next_free_data_slot;
    uint32 size_left;
    uint32 nof_ds_layout_nodes; 
    uint32 nof_max_ds_layout_nodes; /* contains SHR_SW_STATE_DEFS_LAYOUT_NOF_LAYOUT_NODES 
                                       nof_ds_layout_nodes contains the nof layout nodes for the current compilation. 
                                       nof_max_ds_layout_nodes (or SHR_SW_STATE_DEFS_LAYOUT_NOF_LAYOUT_NODES) is nof layout nodes, not depending on compilation flags, 
                                       calculated by autocoder. */
    shr_sw_state_ds_layout_node_t *ds_layout_nodes; /* data structure layout node array */
} shr_sw_state_data_block_header_t;


extern shr_sw_state_data_block_header_t           *shr_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

void _sw_state_access_debug_hook(int id);
int shr_sw_state_init(int unit, int flags, shr_sw_state_init_mode_t init_mode);
int shr_sw_state_deinit(int unit);
int shr_sw_state_alloc(int unit, uint8** ptr, uint32 size);
int shr_sw_state_free(int unit, uint8 *ptr);
int shr_sw_state_data_block_free_restored_data(int unit);
int shr_sw_state_data_block_set(int unit, int start, int end, uint8 *data);
int shr_sw_state_data_block_get(int unit, int start, int end, uint8 *data);
int shr_sw_state_data_block_restored_get(int unit, int start, int end, uint8 *data);
int shr_sw_state_data_block_sync(int unit, int start, int end);
void shr_sw_state_block_dump(int unit, char *file_name, char *mode);
int shr_sw_state_auto_sync(int unit, uint8* current_pointer_in_sw_state, int size);
int shr_sw_state_sizes_get(int unit, uint32* in_use, uint32* left);
int shr_sw_state_defrag(int unit);
int soc_sw_state_nof_allocations_get(void);
int shr_sw_state_allocated_size_get(int unit, uint8* ptr, uint32* allocated_size);
#endif /* _SHR_SW_STATE_H */
