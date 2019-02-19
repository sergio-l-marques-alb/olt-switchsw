/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$  
 */
#ifndef _SW_STATE_DIAGNOSTICS_H
#define _SW_STATE_DIAGNOSTICS_H

#include <soc/dnxc/swstate/dnx_sw_state_utils.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum
{
    DNX_SW_STATE_DIAG_ALLOC_BITMAP,
    DNX_SW_STATE_DIAG_ALLOC,
    DNX_SW_STATE_DIAG_FREE,
    DNX_SW_STATE_DIAG_PTR_ALLOC,
    DNX_SW_STATE_DIAG_PTR_FREE,
    DNX_SW_STATE_DIAG_MODIFY,
    DNX_SW_STATE_DIAG_READ,
    DNX_SW_STATE_DIAG_BITMAP,
    DNX_SW_STATE_DIAG_PBMP,
    DNX_SW_STATE_DIAG_MUTEX,
    DNX_SW_STATE_DIAG_OCC_BM,
    DNX_SW_STATE_DIAG_HTB,
    DNX_SW_STATE_DIAG_LL,
    DNX_SW_STATE_DIAG_CB_DB,
    DNX_SW_STATE_DIAG_MULTI_SET,
    DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK
} dnx_sw_state_func_type_diag_info;

typedef struct dnx_sw_state_diagnostic_info_s
{
    uint32 size;
    uint32 modify_operations_counter;
    uint32 read_operations_counter;
    uint32 bitmap_operations_counter;
    uint32 pbmp_operations_counter;
    uint32 mutex_operations_counter;
    uint32 occupation_bitmap_operations_counter;
    uint32 hash_table_operations_counter;
    uint32 linked_list_operations_counter;
    uint32 cb_db_operations_counter;
    uint32 multi_set_operations_counter;
    uint32 defragmented_chunk_operations_counter;
} dnx_sw_state_diagnostic_info_t;

#define DNX_SW_STATE_DIAG_INFO_UPDATE(unit, module_id, info_struct, node_idx, function, alloc_size, ptr) \
    do { \
        if (function == DNX_SW_STATE_DIAG_ALLOC || function == DNX_SW_STATE_DIAG_PTR_ALLOC) { \
            info_struct[unit][node_idx].size += alloc_size; \
        } else if (function == DNX_SW_STATE_DIAG_FREE || function == DNX_SW_STATE_DIAG_PTR_FREE) { \
            uint32 allocated_size = 0; \
            dnx_sw_state_oob_dynamic_allocated_size_get(unit, module_id, (uint8*)ptr, &allocated_size); \
            info_struct[unit][node_idx].size -= allocated_size; \
        } else if (function == DNX_SW_STATE_DIAG_READ) { \
            info_struct[unit][node_idx].read_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_MODIFY) { \
            info_struct[unit][node_idx].modify_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_BITMAP) { \
            info_struct[unit][node_idx].bitmap_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_PBMP) { \
            info_struct[unit][node_idx].pbmp_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_MUTEX) { \
            info_struct[unit][node_idx].mutex_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_OCC_BM) { \
            info_struct[unit][node_idx].occupation_bitmap_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_HTB) { \
            info_struct[unit][node_idx].hash_table_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_LL) { \
            info_struct[unit][node_idx].linked_list_operations_counter++; \
        } else if (function == DNX_SW_STATE_DIAG_CB_DB) { \
            info_struct[unit][node_idx].cb_db_operations_counter++; \
        }  else if (function == DNX_SW_STATE_DIAG_MULTI_SET) { \
            info_struct[unit][node_idx].multi_set_operations_counter++; \
        }  else if (function == DNX_SW_STATE_DIAG_DEFRAGMENTED_CHUNK) { \
            info_struct[unit][node_idx].defragmented_chunk_operations_counter++; \
        } \
    } while(0)

#else /* defined(DNX_SW_STATE_DIAGNOSTIC) */

#define DNX_SW_STATE_DIAG_INFO_UPDATE(unit, module_id, info_struct, node_idx, function, alloc_size, ptr)

#endif /* defined(DNX_SW_STATE_DIAGNOSTIC) */

#endif /* _SHR_SW_STATE_DIAGNOSTICS_H */
