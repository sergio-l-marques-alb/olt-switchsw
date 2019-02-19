/* 
* $Id: dpp_wb_engine_defs.h,v 1.0 Broadcom SDK $ 
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
#ifndef _SOC_DPP_WB_ENGINE_DEFS_H_
#define _SOC_DPP_WB_ENGINE_DEFS_H_

#include <soc/wb_engine.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

extern soc_port_unit_info_t        ports_unit_info[SOC_MAX_NUM_DEVICES];
extern soc_phy_port_sw_db_t        phy_ports_info[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
extern soc_logical_port_sw_db_t    logical_ports_info[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
extern soc_port_core_info_t        core_info[SOC_MAX_NUM_DEVICES][SOC_DPP_DEFS_MAX(NOF_CORES)];

#define SOC_DPP_OLD_BUFFERS_DECS\
         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_MIRROR:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_MIRROR:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_TRAP:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_ENCAP:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_COS:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF_AC:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB:\
    case SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_SORTED_LIST_DYNAMIC:\
    case SOC_DPP_WB_ENGINE_BUFFER_SWITCH:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_REASSEMBLY_CONTEXT:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TM:\
	case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_LOCATION_TBL:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_HASH_TBL:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_DB_BANK_OCC_BMS_DYNAMIC:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TBL:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_FRWRD_IP:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_TM_GUARANTEED_Q_RESOURCE:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_RESERVED_REASSEMBLY_CONTEXTS:\
    case SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND_SPOOF_ID_REF_CNT:\
    case SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM


#define ADD_MULTIPLE_VARS(_wb_engine_var, nof_vars_to_add) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (nof_vars_to_add - 1)

#define ADD_MULTIPLE_OCC_BMS(_wb_engine_var, nof_bms_to_add) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + ((nof_bms_to_add * WB_ENGINE_OCC_BM_INNER_VARS_NUM) - 1)

#define ADD_MULTIPLE_MULTI_SETS(_wb_engine_var, nof_msts_to_add) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + ((nof_msts_to_add * WB_ENGINE_MULTI_SET_INNER_VARS_NUM) - 1)

#define ADD_MULTIPLE_HASH_TBLS(_wb_engine_var, nof_hash_tbls_to_add) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + ((nof_hash_tbls_to_add * WB_ENGINE_HASH_TABLE_INNER_VARS_NUM) - 1)

#define ADD_MULTIPLE_SORTED_LISTS(_wb_engine_var, nof_sorted_lists_to_add) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + ((nof_sorted_lists_to_add * WB_ENGINE_SORTED_LIST_INNER_VARS_NUM) - 1)

#define COMPLEX_DS_ARR_MEM_ALLOCATOR(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_MEM_ALLOCATOR_INNER_VARS_NUM - 1)


#define COMPLEX_DS_OCC_BM(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_OCC_BM_INNER_VARS_NUM - 1)

#define COMPLEX_DS_HASH_TABLE(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_HASH_TABLE_INNER_VARS_NUM - 1)

#define COMPLEX_DS_BCM_HASH_TABLE(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_BCM_HASH_TABLE_INNER_VARS_NUM - 1)

#define COMPLEX_DS_MULTI_SET(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_MULTI_SET_INNER_VARS_NUM - 1)

#define COMPLEX_DS_PAT_TREE_AGREGATION(_wb_engine_var) \
    _wb_engine_var,                                                     \
    _wb_engine_var##_LAST = _wb_engine_var + (WB_ENGINE_PAT_TREE_AGREGATION_INNER_VARS_NUM - 1)



#define SOC_DPP_WB_ENGINE_VAR_NONE -1

#define SOC_DPP_WB_ENGINE (SOC_WB_ENGINE_PRIMARY)

typedef enum
{
    /* HASH_TABLE complex data structures, internal simple structures */

    /* arrays in SOC_SAND_HASH_TABLE_T */
    WB_ENGINE_BCM_HASH_TABLE_NOF_USED_VALUES, 
    WB_ENGINE_BCM_HASH_TABLE_KEYS,
    WB_ENGINE_BCM_HASH_TABLE_DATAS,
    
    WB_ENGINE_BCM_HASH_TABLE_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_BCM_HASH_TABLE_INNER_VAR;

typedef enum
{
    /* MEM_ALLOCATOR complex data structures, internal simple structures */

    /* simple variables in ARAD_PP_ARR_MEM_ALLOCATOR_T */
    WB_ENGINE_MEM_ALLOCATOR_FREE_LIST, 
    WB_ENGINE_MEM_ALLOCATOR_CACHE_ENABLED, 
    WB_ENGINE_MEM_ALLOCATOR_FREE_LIST_CACHE, 
    WB_ENGINE_MEM_ALLOCATOR_NOF_UPDATES, 

    /* arrays in ARAD_PP_ARR_MEM_ALLOCATOR_T */
    WB_ENGINE_MEM_ALLOCATOR_ARRAY,
    WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW,
    WB_ENGINE_MEM_ALLOCATOR_ARRAY_CACHE,
    WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE,
    WB_ENGINE_MEM_ALLOCATOR_UPDATE_INDEXES,

    WB_ENGINE_MEM_ALLOCATOR_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_MEM_ALLOCATOR_INNER_VAR;

typedef enum
{
    /* GROUP_MEM_LL complex data structures, internal simple structures */

    /* simple variables in SOC_SAND_GROUP_MEM_LL_T */
    WB_ENGINE_GROUP_MEM_LL_CACHE_ENABLED, 

    /* arrays in ARAD_PP_ARR_GROUP_MEM_LL_T */
    WB_ENGINE_GROUP_MEM_LL_GROUPS,
    WB_ENGINE_GROUP_MEM_LL_MEMBERS,
    WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE,
    WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE,

    WB_ENGINE_GROUP_MEM_LL_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_GROUP_MEM_LL_INNER_VAR;

typedef enum
{
    /* OCC_BM complex data structures, internal simple structures */

    /* simple variables in SOC_SAND_OCC_BM_T */
    WB_ENGINE_OCC_BM_CACHE_ENABLED, 

    /* arrays in SOC_SAND_OCC_BM_T */
    WB_ENGINE_OCC_BM_LEVELS_BUFFER, 
    WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, 

    WB_ENGINE_OCC_BM_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_OCC_BM_INNER_VAR;

typedef enum
{
    /* HASH_TABLE complex data structures, internal simple structures */

    /* arrays in SOC_SAND_HASH_TABLE_T */
    WB_ENGINE_HASH_TABLE_KEYS, 
    WB_ENGINE_HASH_TABLE_NEXT, 
    WB_ENGINE_HASH_TABLE_LISTS_HEAD, 

    /* complex DS in SOC_SAND_HASH_TABLE_T */
    COMPLEX_DS_OCC_BM(WB_ENGINE_HASH_TABLE_MEMORY_USE), 

    WB_ENGINE_HASH_TABLE_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_HASH_TABLE_INNER_VAR;

typedef enum
{
    /* MULTI_SET complex data structures, internal simple structures */

    /* arrays in SOC_SAND_MULTI_SET_T */
    WB_ENGINE_MULTI_SET_NEXT_REF_COUNTER, 
 
    /* complex DS in SOC_SAND_MULTI_SET_T */
    COMPLEX_DS_HASH_TABLE(WB_ENGINE_MULTI_SET_HASH_TABLE), 

    WB_ENGINE_MULTI_SET_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_MULTI_SET_INNER_VAR;

typedef enum
{
    /* PAT_TREE_AGREGATION complex data structures, internal simple structures */

    /* PAT_TREE_AGREGATION is a collection of pat_tree that some structures are allocated only once, */
    /* i.e all of the pat_trees use pat_tree index 0 allocations */

    /* simple variables allocated to each pat_tree (thus added as one array for all the pat_trees) */
    WB_ENGINE_PAT_TREE_AGREGATION_ROOT, 
    WB_ENGINE_PAT_TREE_AGREGATION_CACHE_ENABLED, 
    WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, 
    WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
    WB_ENGINE_PAT_TREE_AGREGATION_CURRENT_NODE_PLACE, 

    /* 1 common allocation for all pat_trees */
    WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY, 
    COMPLEX_DS_OCC_BM(WB_ENGINE_PAT_TREE_AGREGATION_MEMORY_USE), 
    WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, 

    WB_ENGINE_PAT_TREE_AGREGATION_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_PAT_TREE_AGREGATION_INNER_VAR;

typedef enum
{
    /* SORTED_LIST complex data structures, internal simple structures */

    /* arrays in SOC_SAND_SORTED_LIST_T */
    WB_ENGINE_SORTED_LIST_KEYS, 
    WB_ENGINE_SORTED_LIST_NEXT, 
    WB_ENGINE_SORTED_LIST_PREV, 
    WB_ENGINE_SORTED_LIST_DATA, 
 
    /* COMPLEX DS in SOC_SAND_SORTED_LIST_T */
    COMPLEX_DS_OCC_BM(WB_ENGINE_SORTED_LIST_MEMORY_USE), 

    WB_ENGINE_SORTED_LIST_INNER_VARS_NUM

} SOC_DPP_WB_ENGINE_COMPLEX_DS_SORTED_LIST_INNER_VAR;

/* !!!!!!!!!!!!!!!!!!!!!! */
/* deprecated definitions */
/* !!!!!!!!!!!!!!!!!!!!!! */
#define SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(_buff, _buff_string, _upgrade_func, _version)\
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, _version, 0x0 /*not only copy*/, SOC_WB_ENGINE_PRE_RELEASE)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_2D_ARR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

/* implicit arrays */
/* examle for an implicit array:
    x[3].y.z[4] is an implicit 2d array (as opposed to w[3][4] which is an explicit 2d array)
   these arrays require two more fields to instruct the engine how many bytes separate two consecutive array entries.
 */
#define SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _arr_jump, _version_added)\
    SOC_WB_ENGINE_ADD_IMPL_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _arr_jump, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_arr_jump, _inner_arr_jump, _version_added)\
    SOC_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _outer_arr_length, _inner_arr_length, _outer_arr_jump, _inner_arr_jump, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

/*******************************************/
/* deprecated dynamic vars addition macros */
/*******************************************/
#define SOC_DPP_WB_ENGINE_ADD_DYNAMIC_VAR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOC_WB_ENGINE_ADD_DYNAMIC_VAR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_DYNAMIC_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);


#define SOC_DPP_WB_ENGINE_ADD_DYNAMIC_BUFF_DEPRECATED(_buff, _buff_string, _upgrade_func, _version, _is_dynamic)\
    SOC_WB_ENGINE_ADD_DYNAMIC_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, _version, _is_dynamic, 0x0 /*not only copy*/, SOC_WB_ENGINE_PRE_RELEASE)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var, _var_string, _buffer, _data_size, _orig_data_ptr, _arr_length, _inner_jump, _version_added) \
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, _orig_data_ptr, 1, _arr_length, 0xffffffff, _inner_jump, _version_added, 0xff, NULL)

/* !!!!!!!!!!!!!!!!!!!!!! */




/************************/
/* complex data structs */
/************************/
#ifdef BCM_ARAD_SUPPORT
/* init these data structure only in warmboot compilation 
 * set/get are not functional for these variables only added to wb engine gain sync and restore
 * we save a shadow of the bcm_hash_table and not the hash table itself, the shadow doesnt exist
 * if not compiling with BCM_WARM_BOOT_SUPPORT. 
 */
#ifdef BCM_WARM_BOOT_SUPPORT   
#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(_var, _var_string, _buffer, _hash_tbl_idx, _version_added) \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_BCM_HASH_TABLE_NOF_USED_VALUES,       \
                              _var_string,                                    \
                              _buffer,                                        \
                              sizeof(int),                                    \
                              &(wb_hash_tbl_data[unit][_hash_tbl_idx].nof_used_values),\
                              _version_added);                                \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_BCM_HASH_TABLE_KEYS,           \
                              _var_string,                                    \
                              _buffer,                                        \
                              wb_hash_tbl_data[unit][_hash_tbl_idx].key_size, \
                              wb_hash_tbl_data[unit][_hash_tbl_idx].keys,     \
                              wb_hash_tbl_data[unit][_hash_tbl_idx].max_nof_values,\
                              _version_added);                                \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_BCM_HASH_TABLE_DATAS,          \
                              _var_string,                                    \
                              _buffer,                                        \
                              wb_hash_tbl_data[unit][_hash_tbl_idx].max_data_size,\
                              wb_hash_tbl_data[unit][_hash_tbl_idx].data,     \
                              wb_hash_tbl_data[unit][_hash_tbl_idx].max_nof_values,\
                              _version_added);
#else /*BCM_WARM_BOOT_SUPPORT*/  
#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(_var, _var_string, _buffer, _hash_tbl_idx, _version_added)
#endif /*BCM_WARM_BOOT_SUPPORT*/   
 

    

#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(_var, _var_string, _buffer, _mem_allocator, _entry_size, _nof_entries, _support_caching, _version_added) \
    if(_buffer == buffer_id && _support_caching == TRUE )               \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_FREE_LIST_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_PTR), \
                                  &(_mem_allocator.arr_mem_allocator_data.free_list_cache), \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_ARRAY_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY), \
                                  _mem_allocator.arr_mem_allocator_data.array_cache, \
                                  _nof_entries,                         \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(uint32),                       \
                                  _mem_allocator.arr_mem_allocator_data.mem_shadow_cache, \
                                  _entry_size * _nof_entries,           \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_UPDATE_INDEXES, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY), \
                                  _mem_allocator.arr_mem_allocator_data.update_indexes, \
                                  _nof_entries,                         \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_FREE_LIST, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_PTR),    \
                              &(_mem_allocator.arr_mem_allocator_data.free_list), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_CACHE_ENABLED, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_mem_allocator.arr_mem_allocator_data.cache_enabled), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_NOF_UPDATES, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint32),                           \
                              &(_mem_allocator.arr_mem_allocator_data.nof_updates), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_ARRAY,     \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY),  \
                              _mem_allocator.arr_mem_allocator_data.array, \
                              _nof_entries,                             \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint32),                           \
                              _mem_allocator.arr_mem_allocator_data.mem_shadow, \
                              _entry_size * _nof_entries,               \
                              _version_added)


#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(_var, _var_string, _buffer, _group_mem_ll, _nof_groups, _nof_elements, _support_caching, _version_added) \
    if(_buffer == buffer_id && _support_caching == TRUE )               \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY), \
                                  _group_mem_ll->group_members_data.groups_cache, \
                                  _nof_groups,                          \
                                  _version_added);                      \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY), \
                                  _group_mem_ll->group_members_data.members_cache, \
                                  _nof_elements,                        \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_CACHE_ENABLED, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_group_mem_ll->group_members_data.cache_enabled), \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_GROUPS,     \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY), \
                              _group_mem_ll->group_members_data.groups, \
                              _nof_groups,                              \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_GROUP_MEM_LL_MEMBERS,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY), \
                              _group_mem_ll->group_members_data.members, \
                              _nof_elements,                            \
                              _version_added)


#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(_var, _var_string, _buffer, _occ_bm, _array_size, _support_cache, _version_added) \
    if(_buffer == buffer_id && _support_cache == TRUE )                 \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(uint8),                        \
                                  _occ_bm->levels_cache_buffer,         \
                                  _array_size,                          \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(_var + WB_ENGINE_OCC_BM_CACHE_ENABLED,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_occ_bm->cache_enabled),                \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_OCC_BM_LEVELS_BUFFER,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              _occ_bm->levels_buffer,                   \
                              _array_size,                              \
                              _version_added);                          

/*dynamic occ bitmap */
#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM_DYNAMIC(_var, _var_string, _buffer, _occ_bm, _array_size, _support_cache, _version_added) \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_VAR_DEPRECATED(_var + WB_ENGINE_OCC_BM_CACHE_ENABLED,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              &(_occ_bm->cache_enabled),                \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var + WB_ENGINE_OCC_BM_LEVELS_BUFFER,    \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(uint8),                            \
                              _occ_bm->levels_buffer,                   \
                              _array_size,                              \
                              _version_added);                          \
    /* in dynamic case always add - if no need than dont enable*/                  \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, \
                              _var_string,                          \
                              _buffer,                              \
                              sizeof(uint8),                        \
                              _occ_bm->levels_cache_buffer,         \
                              _array_size,                          \
                              _version_added);
                                                                       

/* add dynamic hash table */

#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_HASH_TABLE(_var, _var_string, _buffer, _hash_table, _table_size, _key_size, _ptr_size, _table_width, \
                                                    _occ_bm_array_size, _occ_bm_support_cache, _version_added) \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_HASH_TABLE_KEYS,         \
                              _var_string,                              \
                              _buffer,                                  \
                              _key_size,                                \
                              _hash_table.hash_data.keys,               \
                              _table_size,                              \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_HASH_TABLE_NEXT,         \
                              _var_string,                              \
                              _buffer,                                  \
                              _ptr_size,                                \
                              _hash_table.hash_data.next,               \
                              _table_size,                              \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_HASH_TABLE_LISTS_HEAD,   \
                              _var_string,                              \
                              _buffer,                                  \
                              _ptr_size,                                \
                              _hash_table.hash_data.lists_head,         \
                              _table_width,                             \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(_var + WB_ENGINE_HASH_TABLE_MEMORY_USE, \
                                            _var_string,                \
                                            _buffer,                    \
                                            _hash_table.hash_data.memory_use, \
                                            _occ_bm_array_size,         \
                                            _occ_bm_support_cache,      \
                                            _version_added)

#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(_var, _var_string, _buffer, _multi_set, _nof_members, _counter_size, _max_duplications, _table_size, \
                                                   _key_size, _ptr_size, _table_width, _occ_bm_array_size, _occ_bm_support_cache, _version_added) \
                                                                        \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_HASH_TABLE(_var + WB_ENGINE_MULTI_SET_HASH_TABLE, \
                                                _var_string,        \
                                                _buffer,            \
                                                _multi_set.multiset_data.hash_table, \
                                                _table_size,        \
                                                _key_size,          \
                                                _ptr_size,          \
                                                _table_width,       \
                                                _occ_bm_array_size, \
                                                _occ_bm_support_cache, \
                                                _version_added);    \
    if(_buffer == buffer_id && _max_duplications > 1)                   \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_MULTI_SET_NEXT_REF_COUNTER, \
                                  _var_string,                              \
                                  _buffer,                                  \
                                  _counter_size,                            \
                                  _multi_set.multiset_data.ref_counter,     \
                                  _nof_members,                             \
                                  _version_added)                           \
     }




#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_PAT_TREE_AGREGATION(_var, _var_string, _buffer, _pat_tree_0, _nof_pat_trees, _pat_tree_size, \
                                                             _pat_tree_support_cache, _occ_bm_array_size, _occ_bm_support_cache, _version_added) \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_ROOT, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_PLACE), \
                                          &(_pat_tree_0.pat_tree_data.root), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_ENABLED, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(uint8),                \
                                          &(_pat_tree_0.pat_tree_data.cache_enabled), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_PLACE), \
                                          &(_pat_tree_0.pat_tree_data.root_cache), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_KEY), \
                                          &(_pat_tree_0.pat_tree_data.cache_change_head), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED_WITH_OFFSET(_var + WB_ENGINE_PAT_TREE_AGREGATION_CURRENT_NODE_PLACE, \
                                          _var_string,                  \
                                          _buffer,                      \
                                          sizeof(SOC_SAND_PAT_TREE_NODE_PLACE), \
                                          &(_pat_tree_0.pat_tree_data.current_node_place), \
                                          _nof_pat_trees,               \
                                          sizeof(SOC_SAND_PAT_TREE_INFO), \
                                          _version_added);              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY, \
                              _var_string,                              \
                              _buffer,                                  \
                              sizeof(SOC_SAND_PAT_TREE_NODE),           \
                              _pat_tree_0.pat_tree_data.tree_memory,    \
                              _pat_tree_size,                           \
                              _version_added);                          \
    if(_buffer == buffer_id && _pat_tree_support_cache == TRUE )        \
    {                                                                   \
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(_var + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, \
                                  _var_string,                          \
                                  _buffer,                              \
                                  sizeof(SOC_SAND_PAT_TREE_NODE),       \
                                  _pat_tree_0.pat_tree_data.tree_memory_cache, \
                                  _pat_tree_size,                       \
                                  _version_added);                      \
    }                                                                   \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(_var + WB_ENGINE_PAT_TREE_AGREGATION_MEMORY_USE, \
                                            _var_string,                \
                                            _buffer,                    \
                                            _pat_tree_0.pat_tree_data.memory_use, \
                                            _occ_bm_array_size,         \
                                            _occ_bm_support_cache,      \
                                            _version_added)                           


/* add dynamic sorted list */
#define SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_SORTED_LIST_DYNAMIC(_var, _var_string, _buffer, _sorted_list, _key_size, _list_size, _ptr_size, _data_size, \
                                                     _occ_bm_array_size, _occ_bm_support_cache, _version_added) \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var + WB_ENGINE_SORTED_LIST_KEYS,        \
                              _var_string,                              \
                              _buffer,                                  \
                              _key_size,                                \
                              _sorted_list.list_data.keys,                       \
                              _list_size,                               \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var + WB_ENGINE_SORTED_LIST_NEXT,        \
                              _var_string,                              \
                              _buffer,                                  \
                              _ptr_size,                                \
                              _sorted_list.list_data.next,                       \
                              (_list_size + 2),                         \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var + WB_ENGINE_SORTED_LIST_PREV,        \
                              _var_string,                              \
                              _buffer,                                  \
                              _ptr_size,                                \
                              _sorted_list.list_data.prev,                       \
                              (_list_size + 2),                         \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_ARR_DEPRECATED(_var + WB_ENGINE_SORTED_LIST_DATA,        \
                              _var_string,                              \
                              _buffer,                                  \
                              _data_size,                               \
                              _sorted_list.list_data.data,                       \
                              _list_size,                               \
                              _version_added);                          \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM_DYNAMIC(_var + WB_ENGINE_SORTED_LIST_MEMORY_USE, \
                                            _var_string,                \
                                            _buffer,                    \
                                            _sorted_list.list_data.memory_use,   \
                                            _occ_bm_array_size,         \
                                            _occ_bm_support_cache,      \
                                            _version_added) 
#endif /*BCM_ARAD_SUPPORT*/



/********************************************************************************/
/* depracated code, located in here to state that it generally shouldnt be used */
/* as reference implementation                                                  */
/********************************************************************************/

/*SOC_DPP_ADD_ALLOC_MNGR_BUFF_DEPRECATED_CODE*/
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_ARAD_SUPPORT))
    #define SOC_DPP_ADD_ALLOC_MNGR_BUFF_DEPRECATED_CODE \
        /* alloc mngr module */ \
        SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR, "alloc_mngr" , NULL, VERSION(1));
#else /* (defined(BCM_PETRA_SUPPORT) || defined(BCM_ARAD_SUPPORT)) */
    #define SOC_DPP_ADD_ALLOC_MNGR_BUFF_DEPRECATED_CODE
#endif /* (defined(BCM_PETRA_SUPPORT) || defined(BCM_ARAD_SUPPORT)) */




/*deprecated ARAD buffer declarations*/
#define SOC_DPP_ADD_ARAD_BUFFS_DEPRECATED_CODE \
    /*ARAD_PP_MGMT_OPERATION_MODE module*/                                                                                                                                     \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE, "operation_mode" , NULL, VERSION(2));                                          \
                                                                                                                                                                               \
    /*ARAD_PP_SW_DB_PON_DOUBLE_LOOKUP module*/                                                                                                                                 \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO, "ipv4_info" , NULL, VERSION(1));                                                   \
                                                                                                                                                                               \
    /* arad_sw_db module - pmf struct */                                                                                                                                       \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF, "arad_sw_db_pmf" , NULL, VERSION(1));                                                       \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam restoration data */                                                                                                                            \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA, "arad_sw_db_tcam - restoration data" , NULL, VERSION(1));                 \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct */                                                                                                                                      \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM, "arad_sw_db_tcam" , NULL, VERSION(1));                                                     \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam_mgmt struct */                                                                                                                                 \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT, "arad_sw_db_tcam_mgmt" , NULL, VERSION(1));                                           \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct - buffer contains all occ bitmaps */                                                                                                    \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC, "arad_sw_db_tcam_mgmt occ bitmaps" , NULL, VERSION(1), TRUE);                 \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct - buffer contains all occ bitmaps */                                                                                                    \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_DB_BANK_OCC_BMS_DYNAMIC, "arad_sw_db_tcam_mgmt db bank occ bitmaps" , NULL, VERSION(1), TRUE); \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct - buffer contains all hash tables */                                                                                                    \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_HASH_TBL, "arad_sw_db_tcam_db hash tbls" , NULL, VERSION(1));                               \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct - buffer contains all sorted lists */                                                                                                   \
    SOC_DPP_WB_ENGINE_ADD_DYNAMIC_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_SORTED_LIST_DYNAMIC, "arad_sw_db_tcam_db hash tbls" , NULL, VERSION(1), TRUE);                 \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct - buffer contains all location tables */                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_LOCATION_TBL, "arad_sw_db_tcam_db_location_tbls" , NULL, VERSION(1));                       \
                                                                                                                                                                               \
    /* arad_sw_db module - tcam struct - buffer contains all location tables */                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TBL, "arad_sw_db_tcam_db_global_location_tbls" , NULL, VERSION(1));         \
                                                                                                                                                                               \
    /* switch module */                                                                                                                                                        \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_SWITCH, "switch" , NULL, VERSION(1));                                                                       \
                                                                                                                                                                               \
    /* arad_sw_db module - sand data structs */                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS, "arad_sw_db - sand data structs" , NULL, VERSION(1));                         \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND, "arad_sw_db - src_bind data structs" , NULL, VERSION(1));                              \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_REASSEMBLY_CONTEXT, "arad_sw_db - reassembly_ctxt data structs" , NULL, VERSION(1));             \
                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT, "arad_port" , NULL, VERSION(1));                                                                 \
                                                                                                                                                                               \
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {                                                                             \
        SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP, "rif to lif group map" , NULL, VERSION(1));                               \
    }                                                                                                                                                                          \
                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_TM_GUARANTEED_Q_RESOURCE, "tm guaranteed q resource" , NULL, VERSION(1));                              \
                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TM, "tm" , NULL, VERSION(1));                                                                    \
                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_FRWRD_IP, "ip forwarding" , NULL, VERSION(1));                                                   \
                                                                                                                                                                               \
                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_RESERVED_REASSEMBLY_CONTEXTS, "reserved reassembly contexts" , NULL, VERSION(1));                      \
                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM, "Exact match table simulation", NULL, VERSION(1))                                                \
                                                                                                                                                                               \
                                                                                                                                                                               \
    /* IPMC module*/                                                                                                                                                           \
    propval = soc_property_get_str(unit, spn_L3_SOURCE_BIND_MODE);                                                                                                             \
    if (NULL != propval && 0 != sal_strcmp("DISABLE", propval)){                                                                                                               \
        SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND_SPOOF_ID_REF_CNT, "spoof_id_ref_count" , NULL, VERSION(1));                         \
    }                                                                                                                                                                          \

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET\
    /*special treatment for restoring dynamic vars*/\
    if (SOC_WARM_BOOT(unit) && buffer_is_dynamic) {\
        rv = soc_wb_engine_buffer_dynamic_vars_state_get(unit, SOC_DPP_WB_ENGINE, buffer_id, &buffer_header);\
        SOCDNX_IF_ERR_EXIT(rv);\
    }
#else /*BCM_WARM_BOOT_SUPPORT*/
#define SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET\
    /*special treatment for restoring dynamic vars*/\
    if (SOC_WARM_BOOT(unit) && buffer_is_dynamic) {\
        rv = SOC_E_NONE;\
        SOCDNX_IF_ERR_EXIT(rv);\
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/

#ifdef BCM_WARM_BOOT_SUPPORT
    #define SOC_DPP_ADD_SWITCH_VARS_DEPRECATED_CODE\
            SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_SWITCH_WARMBOOT_AUTO_SYNC,\
                                  "autosync flag",\
                                  SOC_DPP_WB_ENGINE_BUFFER_SWITCH,\
                                  sizeof(uint32),\
                                  &(SOC_CONTROL(unit)->autosync), /*only exist when compiled with BCM_WARM_BOOT_SUPPORT*/\
                                  VERSION(1));
#else
    #define SOC_DPP_ADD_SWITCH_VARS_DEPRECATED_CODE
#endif /*BCM_WARM_BOOT_SUPPORT*/


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    #define SOC_DPP_ADD_KBP_VARS_DEPRECATED_CODE \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_CACHE_MODE,\
                              "kbp_cache_mode",\
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_FRWRD_IP,\
                              sizeof(uint8),\
                              &(Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.kbp_cache_mode),\
                              VERSION(1));\
        SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_LOCATION_TABLE,\
                              "location_tbl",\
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_FRWRD_IP,\
                              sizeof(ARAD_SW_KBP_HANDLE),\
                              &(Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.location_tbl[0]),\
                              (ARAD_KBP_NL_88650_MAX_NOF_ENTRIES + SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit)),\
                              VERSION(1));
#else
    #define SOC_DPP_ADD_KBP_VARS_DEPRECATED_CODE
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */


#define SOC_DPP_ADD_ARAD_VARS_DEPRECATED_CODE \
    /*ARAD_PP_MGMT_OPERATION_MODE module*/                                                                                                                                                                                                                    \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_AUTHENTICATION_ENABLE,                                                                                                                                             \
                               "authentication_enable",                                                                                                                                                                                                       \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->authentication_enable),                                                                                                                                                 \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_MPLS_TERMINATION_LABEL_INDEX_ENABLE,                                                                                                                     \
                              "mpls_termination_label_index_enable",                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->mpls_info.mpls_termination_label_index_enable),                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_FAST_REROUTE_LABELS_ENABLE,                                                                                                                              \
                              "fast_reroute_labels_enable",                                                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->mpls_info.fast_reroute_labels_enable),                                                                                                                                  \
                              VERSION(1));	                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_MPLS_COUPLING_ENABLE,                                                                                                                                    \
                              "mpls_coupling_enable",                                                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->mpls_info.mpls_coupling_enable),                                                                                                                                        \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_LOOKUP_INCLUDE_INRIF,                                                                                                                                    \
                              "lookup_include_inrif",                                                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->mpls_info.lookup_include_inrif),                                                                                                                                        \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MIM_INITIALIZED,                                                                                                                                                   \
                              "mim_initialized",                                                                                                                                                                                                              \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->mim_initialized),                                                                                                                                                       \
                              VERSION(2));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_OAM_ENABLE,                                                                                                                                                        \
                              "oam_enable",                                                                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->oam_enable),                                                                                                                                                            \
                              VERSION(2));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_BFD_ENABLE,                                                                                                                                                        \
                              "bfd_enable",                                                                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->bfd_enable),                                                                                                                                                            \
                              VERSION(2));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_TRILL_ENABLE,                                                                                                                                                      \
                              "trill_enable",                                                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE,                                                                                                                                                                           \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(Dpp_wb_engine_Arad_pp_sw_db->device[unit]->oper_mode->trill_enable),                                                                                                                                                          \
                              VERSION(2));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_PON_DOUBLE_LOOKUP module*/                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_DEFAULT_FEC,                                                                                                                                                           \
                              "default_fec",                                                                                                                                                                                                                  \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(PP_SW_DB_DEVICE->ipv4_info->default_fec),                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_NOF_LPM_ENTRIES_IN_LPM,                                                                                                                                                \
                              "nof_lpm_entries_in_lpm",                                                                                                                                                                                                       \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(PP_SW_DB_DEVICE->ipv4_info->nof_lpm_entries_in_lpm),                                                                                                                                                                          \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_NOF_VRFS,                                                                                                                                                              \
                              "nof_vrfs",                                                                                                                                                                                                                     \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(PP_SW_DB_DEVICE->ipv4_info->nof_vrfs),                                                                                                                                                                                        \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*Note:consider pointer as an array with specific size*/                                                                                                                                                                                                  \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_VRF_MODIFIED_BITMASK,                                                                                                                                                  \
                              "vrf_modified_bitmask",                                                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              (PP_SW_DB_DEVICE->ipv4_info->vrf_modified_bitmask),                                                                                                                                                                             \
                              ARAD_PP_SW_IPV4_VRF_BITMAP_SIZE,                                                                                                                                                                                                \
                              VERSION(1));	                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_CACHE_MODIFIED,                                                                                                                                                        \
                              "cache_modified",                                                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(PP_SW_DB_DEVICE->ipv4_info->cache_modified),                                                                                                                                                                                  \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_CACHE_MODE,                                                                                                                                                            \
                              "cache_mode",                                                                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(PP_SW_DB_DEVICE->ipv4_info->cache_mode),                                                                                                                                                                                      \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_LEM_ADD_FAIL,                                                                                                                                                          \
                              "lem_add_fail",                                                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                                               \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &(PP_SW_DB_DEVICE->ipv4_info->lem_add_fail),                                                                                                                                                                                    \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB, "port_sw_db", NULL, VERSION(1));                                                                                                                                          \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INITIALIZED,                                                                                                                                                                \
                              "phy_port_initialized",                                                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].initialized),                                                                                                                                                                                         \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB encap mode*/                                                                                                                                                                                                                            \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_ENCAP_MODE,                                                                                                                                                                 \
                              "phy_port_encap",                                                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].encap_mode),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB core id */                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_CORE,                                                                                                                                                                       \
                              "phy_port_core",                                                                                                                                                                                                                \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].core),                                                                                                                                                                                                \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB calendar mode */                                                                                                                                                                                                                        \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_SINGLE_CAL_MODE,                                                                                                                                                         \
                              "is_single_calendar_mode",                                                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].is_single_cal_mode),                                                                                                                                                                                  \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB high priority cal id */                                                                                                                                                                                                                 \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_HIGH_PRIORITY_CAL,                                                                                                                                                          \
                              "high_pirority_cal",                                                                                                                                                                                                            \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(phy_ports_info[unit][0].high_pirority_cal),                                                                                                                                                                                   \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB low priority cal id */                                                                                                                                                                                                                  \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LOW_PRIORITY_CAL,                                                                                                                                                           \
                              "low_pirority_cal",                                                                                                                                                                                                             \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(phy_ports_info[unit][0].low_pirority_cal),                                                                                                                                                                                    \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB runt pad*/                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_RUNT_PAD,                                                                                                                                                                   \
                              "phy_port_runt_pad",                                                                                                                                                                                                            \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].runt_pad),                                                                                                                                                                                            \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_PHY_PORTS,                                                                                                                                                                  \
                              "phy_port_phy_ports",                                                                                                                                                                                                           \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(soc_pbmp_t),                                                                                                                                                                                                             \
                              &(phy_ports_info[unit][0].phy_ports),                                                                                                                                                                                           \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INTERFACE_TYPE,                                                                                                                                                             \
                              "phy_port_interface_type",                                                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(soc_port_if_t),                                                                                                                                                                                                          \
                              &(phy_ports_info[unit][0].interface_type),                                                                                                                                                                                      \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_SPEED,                                                                                                                                                                      \
                              "phy_port_speed",                                                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].speed),                                                                                                                                                                                               \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_MASTER_PORT,                                                                                                                                                                \
                              "phy_port_master_port",                                                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(soc_port_t),                                                                                                                                                                                                             \
                              &(phy_ports_info[unit][0].master_port),                                                                                                                                                                                         \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_CHANNELS_COUNT,                                                                                                                                                             \
                              "phy_port_channels_count",                                                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(phy_ports_info[unit][0].channels_count),                                                                                                                                                                                      \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LATCH_DOWN,                                                                                                                                                                 \
                              "phy_port_latch_down",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].latch_down),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_VALID,                                                                                                                                                                 \
                              "logical_ports_valid",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(logical_ports_info[unit][0].valid),                                                                                                                                                                                           \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FIRST_PHY_PORT,                                                                                                                                                        \
                              "logical_ports_first_phy_port",                                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(logical_ports_info[unit][0].first_phy_port),                                                                                                                                                                                  \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_CHANNEL,                                                                                                                                                               \
                              "logical_ports_channel",                                                                                                                                                                                                        \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(logical_ports_info[unit][0].channel),                                                                                                                                                                                         \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PROTOCOL_OFFSET,                                                                                                                                                               \
                              "logical_ports_protocol_offset",                                                                                                                                                                                                        \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(logical_ports_info[unit][0].protocol_offset),                                                                                                                                                                                         \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FLAGS,                                                                                                                                                                 \
                              "logical_ports_flag",                                                                                                                                                                                                           \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(logical_ports_info[unit][0].flags),                                                                                                                                                                                           \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_CORE,                                                                                                                                                        \
                              "logical_ports_core",                                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              &(logical_ports_info[unit][0].core),                                                                                                                                                                                  \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_CHANNELIZED,                                                                                                                                                        \
                              "phy_port_is_channelized",                                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(phy_ports_info[unit][0].is_channelized),                                                                                                                                                                                  \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                              \
                              sizeof(soc_phy_port_sw_db_t),                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_TM_PORT,                                                                                                                                                               \
                              "logical_ports_tm_port",                                                                                                                                                                                                        \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                       \
                              sizeof(int),                                                                                                                                                                                                                    \
                              &(logical_ports_info[unit][0].tm_port),                                                                                                                                                                                         \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
                                                                                                                                                                                                                 \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_PORT,                                                                                                                                                                \
                              "logical_ports_pp_port",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].pp_port),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PRIORITY_MODE,                                                                                                                                                                \
                              "logical_ports_port_prio_mode",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].priority_mode),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_MC_OFFSET,                                                                                                                                                                \
                              "logical_ports_mc_offset",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].multicast_offset),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_EGR_INTERFACE,                                                                                                                                                                \
                              "logical_ports_egr_interface",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(uint32),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].egr_interface),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                               \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_OLIF_EXT_EN,                                                                                                                                                                \
                              "logical_ports_olif_ext_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(SOC_TMC_PORTS_FTMH_EXT_OUTLIF),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].outlif_ext_en),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_SRC_EXT_EN,                                                                                                                                                                \
                              "logical_ports_src_ext_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].src_ext_en),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_DST_EXT_EN,                                                                                                                                                                \
                              "logical_ports_dst_ext_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].dst_ext_en),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FRST_HDR_SZ,                                                                                                                                                                \
                              "logical_ports_frst_hdr_sz",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].first_header_size),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_HDR_T_OUT,                                                                                                                                                                \
                              "logical_ports_hdr_t_out",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(ARAD_PORT_HEADER_TYPE),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].header_type_out),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_HDR_T_IN,                                                                                                                                                                \
                              "logical_ports_hdr_t_in",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(ARAD_PORT_HEADER_TYPE),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].header_type_in),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_FLAGS,                                                                                                                                                                \
                              "logical_ports_pp_flags",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(uint32),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].pp_flags),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FC_TYPE,                                                                                                                                                                \
                              "logical_ports_fc_type",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(SOC_TMC_PORTS_FC_TYPE),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].fc_type),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_MIRROR_PROFILE,                                                                                                                                                                \
                              "logical_ports_mirror_prof",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].mirror_profile),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_ING_SHAPING_ENABLED,                                                                                                                                                                \
                              "logical_ports_tm_ing_shap_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].is_tm_ing_shaping_enabled),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_SRC_SYST_PORT_EXT_PRESENT,                                                                                                                                                                \
                              "logical_ports_src_syst_port_ext_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].is_tm_src_syst_port_ext_present),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_PPH_PRESENT_ENABLED,                                                                                                                                                                \
                              "logical_ports_pph_present_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].is_tm_pph_present_enabled),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_STAG_ENABLED,                                                                                                                                                                \
                              "logical_ports_stag_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].is_stag_enabled),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_SNOOP_ENABLED,                                                                                                                                                                \
                              "logical_ports_snoop_en",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].is_snoop_enabled),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_SHAPER_MODE,                                                                                                                                                                \
                              "logical_ports_shaper_mode",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(ARAD_EGR_PORT_SHAPER_MODE),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].shaper_mode),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PEER_TM_DOMAIN,                                                                                                                                                                \
                              "logical_ports_peer_tm_domain",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].peer_tm_domain),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
    /*ARAD_PORT_SW_DB module*/                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_BASE_Q_PAIR,                                                                                                                                                                \
                              "logical_ports_base_q_pair",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB,                                                                                                                                                                                      \
                              sizeof(int),                                                                                                                                                                                                                \
                              &(logical_ports_info[unit][0].base_q_pair),                                                                                                                                                                                          \
                              SOC_MAX_NUM_PORTS,                                                                                                                                                                                                             \
                              sizeof(soc_logical_port_sw_db_t),                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_1,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_1",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_1].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_2,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_2",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_2].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_3,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_3",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_3].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_4,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_4",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_4].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_5,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_5",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_5].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_6,                                                                                                                                  \
                                                       "ipv4_info_arr_mem_allocator_6",                                                                                                                                                                       \
                                                       SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                      \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6],                                                                                                              \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6].entry_size,                                                                                                   \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6].nof_entries,                                                                                                  \
                                                       PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.mem_allocators[ARAD_PP_IPV4_LPM_MEMORY_6].support_caching,                                                                                              \
                                                       VERSION(1));                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /* checking buffer_id because when soc_dpp_wb_engine_init_tables is called with other buffers                                                                                                                                                             \
       ipv4_info may be not initialized yet */                                                                                                                                                                                                                \
    if (buffer_id == SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO &&                                                                                                                                                                                      \
        PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.flags & ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG)                                                                                                                                                               \
    {                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        /*there are 5 GROUP_MEM_LLs that are allocated in memories 2-6 (array entries 1-5)*/                                                                                                                                                                  \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_1,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_1",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_2]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_2,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_2",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_3]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_3,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_3",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_4]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_4,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_4",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_5]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_5,                                                                                                                                     \
                                                         "ipv4_info_group_mem_ll_5",                                                                                                                                                                          \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6],                                                                                                                  \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6]->nof_groups,                                                                                                      \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6]->nof_elements,                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.rev_ptrs[ARAD_PP_IPV4_LPM_MEMORY_6]->support_caching,                                                                                                 \
                                                         VERSION(1));                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_PAT_TREE_AGREGATION(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_LPMS,                                                                                                                                               \
                                                         "ipv4_info_lpms",                                                                                                                                                                                    \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO,                                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0],                                                                                                                                              \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.nof_lpms,                                                                                                                                             \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].tree_size,                                                                                                                                    \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].support_cache,                                                                                                                                \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].pat_tree_data.memory_use->buffer_size,                                                                                                        \
                                                         PP_SW_DB_DEVICE->ipv4_info->lpm_mngr.init_info.lpms[0].pat_tree_data.memory_use->support_cache,                                                                                                      \
                                                         VERSION(1));                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_LLP_MIRROR module*/                                                                                                                                                                                                                       \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_MIRROR, "llp_mirror" , NULL, VERSION(1));                                                                                                                                \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_MIRROR_LLP_MIRROR_PORT_VLAN,                                                                                                                                                 \
                              "llp_mirror_port_vlan",                                                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_MIRROR,                                                                                                                                                                              \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              (&PP_SW_DB_DEVICE->llp_mirror->llp_mirror_port_vlan[0][0]),                                                                                                                          \
                              ARAD_PORT_NOF_PP_PORTS,                                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_MIRROR_MIRROR_PROFILE_MULTI_SET,                                                                                                                                       \
                                               "llp_mirror_mirror_profile_multi_set",                                                                                                                                                                         \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_MIRROR,                                                                                                                                                             \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set,                                                                                                                                                         \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.init_info.nof_members,                                                                                                                                   \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.counter_size,                                                                                                                              \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.init_info.max_duplications,                                                                                                                              \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                                           \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                                             \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                                             \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                                          \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                              \
                                               PP_SW_DB_DEVICE->llp_mirror->mirror_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                            \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_LLP_TRAP module*/                                                                                                                                                                                                                         \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_TRAP, "llp_trap" , NULL, VERSION(1));                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_TRAP_L3_PROTOCOLS_MULTI_SET,                                                                                                                                           \
                                               "llp_trap_l3_protocols_multi_set",                                                                                                                                                                             \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_TRAP,                                                                                                                                                               \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set,                                                                                                                                                             \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.init_info.nof_members,                                                                                                                                       \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.counter_size,                                                                                                                                  \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.init_info.max_duplications,                                                                                                                                  \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                                               \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                                                 \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                                                 \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                                              \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                                  \
                                               PP_SW_DB_DEVICE->llp_trap->l3_protocols_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                                \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_EG_MIRROR module*/                                                                                                                                                                                                                        \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_MIRROR, "eg_mirror" , NULL, VERSION(1));                                                                                                                                  \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_EG_MIRROR_EG_MIRROR_PORT_VLAN,                                                                                                                                                   \
                              "eg_mirror_port_vlan",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_MIRROR,                                                                                                                                                                               \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              (&PP_SW_DB_DEVICE->eg_mirror->eg_mirror_port_vlan[0][0]),                                                                                                                                                                              \
                              ARAD_PORT_NOF_PP_PORTS,                                                                                                                                                                                                         \
                              VERSION(1));	                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_EG_MIRROR_MIRROR_PROFILE_MULTI_SET,                                                                                                                                        \
                                               "eg_mirror_mirror_profile_multi_set",                                                                                                                                                                          \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_MIRROR,                                                                                                                                                              \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set,                                                                                                                                                          \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.init_info.nof_members,                                                                                                                                    \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.counter_size,                                                                                                                               \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.init_info.max_duplications,                                                                                                                               \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                                            \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                                              \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                                              \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                                           \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                               \
                                               PP_SW_DB_DEVICE->eg_mirror->mirror_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                             \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_EG_ENCAP module*/                                                                                                                                                                                                                         \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_ENCAP, "eg_encap" , NULL, VERSION(1));                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_EG_ENCAP_PROG_DATA,                                                                                                                                                        \
                                               "eg_encap_prog_data",                                                                                                                                                                                          \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_ENCAP,                                                                                                                                                               \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data,                                                                                                                                                                          \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.init_info.nof_members,                                                                                                                                                    \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.counter_size,                                                                                                                                               \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.init_info.max_duplications,                                                                                                                                               \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.hash_table.init_info.table_size,                                                                                                                            \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.hash_table.init_info.key_size,                                                                                                                              \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.hash_table.hash_data.ptr_size,                                                                                                                              \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.hash_table.init_info.table_width,                                                                                                                           \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                                               \
                                               PP_SW_DB_DEVICE->eg_encap->prog_data.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                                             \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_LLP_COS module*/                                                                                                                                                                                                                          \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_COS, "llp_cos" , NULL, VERSION(1));                                                                                                                                      \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_COS_ETHER_TYPE_MULTI_SET,                                                                                                                                              \
                                               "llp_cos_ether_type_multi_set",                                                                                                                                                                                \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_COS,                                                                                                                                                                \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set,                                                                                                                                                                \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.init_info.nof_members,                                                                                                                                          \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.counter_size,                                                                                                                                     \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.init_info.max_duplications,                                                                                                                                     \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                                                  \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                                                    \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                                                    \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                                                 \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                                     \
                                               PP_SW_DB_DEVICE->llp_cos->ether_type_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                                   \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE module*/                                                                                                                                                                                                          \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE, "eth_policer_mtr_profile" , NULL, VERSION(1));                                                                                                      \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE_CONFIG_METER_STATUS,                                                                                                                                     \
                              "config_meter_status",                                                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE,                                                                                                                                                                 \
                              sizeof(uint32),                                                                                                                                                                                                                 \
                              (PP_SW_DB_DEVICE->eth_policer_mtr_profile->config_meter_status),                                                                                                                                                                \
                              ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BYTE,                                                                                                                                                       \
                              VERSION(1));	                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE_ETH_METER_PROFILE_MULTI_SET,                                                                                                                       \
                                               "eth_policer_mtr_profile_eth_meter_profile_multi_set",                                                                                                                                                         \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE,                                                                                                                                                \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set,                                                                                                                                         \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.init_info.nof_members,                                                                                                                   \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.counter_size,                                                                                                              \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.init_info.max_duplications,                                                                                                              \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                           \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                             \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                             \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                          \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                              \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->eth_meter_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                            \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE_GLOBAL_METER_PROFILE_MULTI_SET,                                                                                                                    \
                                               "eth_policer_mtr_profile_global_meter_profile_multi_set",                                                                                                                                                      \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE,                                                                                                                                                \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set,                                                                                                                                      \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.init_info.nof_members,                                                                                                                \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.counter_size,                                                                                                           \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.init_info.max_duplications,                                                                                                           \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                        \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                          \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                          \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                       \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                           \
                                               PP_SW_DB_DEVICE->eth_policer_mtr_profile->global_meter_profile_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                         \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_L2_LIF_AC module*/                                                                                                                                                                                                                        \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF_AC, "l2_lif_ac" , NULL, VERSION(1));                                                                                                                                  \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_L2_LIF_AC_AC_KEY_MAP_MULTI_SET,                                                                                                                                            \
                                               "l2_lif_ac_ac_key_map_multi_set",                                                                                                                                                                              \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF_AC,                                                                                                                                                              \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set,                                                                                                                                                              \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.init_info.nof_members,                                                                                                                                        \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.counter_size,                                                                                                                                   \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.init_info.max_duplications,                                                                                                                                   \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                                                \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                                                  \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                                                  \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                                               \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                                   \
                                               PP_SW_DB_DEVICE->l2_lif_ac->ac_key_map_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                                 \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    /*ARAD_PP_SW_DB_L2_LIF_AC module*/                                                                                                                                                                                                                        \
    SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF, "l2_lif" , NULL, VERSION(1));                                                                                                                                        \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_L2_LIF_VLAN_COMPRESSION_RANGE_MULTI_SET,                                                                                                                                   \
                                               "l2_lif_vlan_compression_range_multi_set",                                                                                                                                                                     \
                                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF,                                                                                                                                                                 \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set,                                                                                                                                                     \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.init_info.nof_members,                                                                                                                               \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.counter_size,                                                                                                                          \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.init_info.max_duplications,                                                                                                                          \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.hash_table.init_info.table_size,                                                                                                       \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.hash_table.init_info.key_size,                                                                                                         \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.hash_table.hash_data.ptr_size,                                                                                                         \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.hash_table.init_info.table_width,                                                                                                      \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                                                                                          \
                                               PP_SW_DB_DEVICE->l2_lif->vlan_compression_range_multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                                                                                        \
                                               VERSION(1));                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
    if ( SOC_IS_ARAD(unit) && SOC_DPP_PP_ENABLE(unit)) {                                                                                                                                                                                                      \
                                                                                                                                                                                                                                                              \
		/* OAM module*/                                                                                                                                                                                                                                           \
		SOC_DPP_WB_ENGINE_ADD_BUFF_DEPRECATED(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM, "oam" , NULL, VERSION(1));                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_ACC_ENDPOINT_REF_COUNTER,                                                                                                                                                          \
                                  "oam_acc_ref_counter",                                                                                                                                                                                                      \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(int),                                                                                                                                                                                                                \
                                  &(_dpp_oam_acc_ref_counter[unit]),                                                                                                                                                                                          \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER,                                                                                                                                                                 \
                                  "oam_traps_ref_counter",                                                                                                                                                                                                    \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(int),                                                                                                                                                                                                                \
                                  &(_dpp_oam_traps_ref_counter[unit]),                                                                                                                                                                                        \
                                  SOC_PPD_NOF_TRAP_CODES,                                                                                                                                                                                                     \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,                                                                                                                                                           \
                                  "oam_init_trap_info_trap_ids",                                                                                                                                                                                              \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  _bcm_dpp_oam_trap_info[unit].trap_ids,                                                                                                                                                                                      \
								  SOC_PPC_OAM_TRAP_ID_COUNT,                                                                                                                                                                                                                  \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS,                                                                                                                                                     \
                                  "oam_init_trap_info_upmep_trap_ids",                                                                                                                                                                                        \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  _bcm_dpp_oam_trap_info[unit].upmep_trap_ids,                                                                                                                                                                                \
								  SOC_PPC_OAM_UPMEP_TRAP_ID_COUNT,                                                                                                                                                                                                            \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_MIRROR_IDS,                                                                                                                                                         \
                                  "oam_init_trap_info_mirror_ids",                                                                                                                                                                                            \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  _bcm_dpp_oam_trap_info[unit].mirror_ids,                                                                                                                                                                                    \
								  SOC_PPD_OAM_MIRROR_ID_COUNT,                                                                                                                                                                                                                \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP,                                                                                                                                                   \
                                  "oam_init_trap_code_to_mirror_profile_map",                                                                                                                                                                                 \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit],                                                                                                                                                                         \
								  SOC_PPD_NOF_TRAP_CODES,                                                                                                                                                                                                                     \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP,                                                                                                                                               \
                                  "oam_y1731_opcode_to_intenal_opcode_map",                                                                                                                                                                                   \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  _dpp_oam_y1731_opcode_to_intenal_opcode_map[unit],                                                                                                                                                                          \
								  SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT,                                                                                                                                                                                                      \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        if (SOC_IS_ARADPLUS(unit)) {                                                                                                                                                                                                                          \
            SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_ERROR_TYPE_TO_TRAP_ID_MAP,                                                                                                                                                \
                                      "oamp_error_type_to_trap_id_map",                                                                                                                                                                                       \
                                      SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                   \
                                      sizeof(uint32),                                                                                                                                                                                                         \
                                      _bcm_oamp_error_type_to_trap_id_map[unit],                                                                                                                                                                              \
                                      SOC_PPD_OAM_OAMP_TRAP_TYPE_COUNT,                                                                                                                                                                                       \
                                      VERSION(1));                                                                                                                                                                                                            \
            SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP,                                                                                                                                                \
                                      "oamp_server_rx_trap",                                                                                                                                                                                       \
                                      SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                   \
                                      sizeof(uint32),                                                                                                                                                                                                         \
                                      _bcm_oamp_rx_trap_code_ref_count[unit],                                                                                                                                                                              \
                                      SOC_PPD_NOF_TRAP_CODES,                                                                                                                                                                                       \
                                      VERSION(1));                                                                                                                                                                                                            \
        }                                                                                                                                                                                                                                                     \
                                                                                                                                                                                                                                                              \
		/*BFD*/                                                                                                                                                                                                                                                   \
		SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_INIT_TRAP_INFO_TRAP_IDS,                                                                                                                                                           \
                                  "bfd_init_trap_info_trap_ids",                                                                                                                                                                                              \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  _bcm_dpp_bfd_trap_info[unit].trap_ids,                                                                                                                                                                                      \
								  SOC_PPC_BFD_TRAP_ID_COUNT,                                                                                                                                                                                                                  \
                                  VERSION(1));                                                                                                                                                                                                                \
		                                                                                                                                                                                                                                                          \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_UDP_SPORT_REF_COUNTER,                                                                                                                                                        \
                                  "bfd_mpls_udp_sport_ref_counter",                                                                                                                                                                                           \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_bfd_mpls_udp_sport_ref_counter[unit]),                                                                                                                                                                               \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_UDP_SPORT_REF_COUNTER,                                                                                                                                                        \
                                  "bfd_ipv4_udp_sport_ref_counter",                                                                                                                                                                                           \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_bfd_ipv4_udp_sport_ref_counter[unit]),                                                                                                                                                                               \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER,                                                                                                                                                    \
                                  "bfd_ipv4_udp_sport_ref_counter",                                                                                                                                                                                           \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_bfd_ipv4_multi_hop_acc_ref_counter[unit]),                                                                                                                                                                           \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER,                                                                                                                                                                   \
                                  "bfd_pdu_ref_counter",                                                                                                                                                                                                      \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_bfd_pdu_ref_counter[unit]),                                                                                                                                                                                          \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER,                                                                                                                                                        \
                                  "bfd_mpls_tp_cc_ref_counter",                                                                                                                                                                                               \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_bfd_mpls_tp_cc_ref_counter[unit]),                                                                                                                                                                                   \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        /* SOC */                                                                                                                                                                                                                                             \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_RECYCLE,                                                                                                                                                                 \
                                  "oam_trap_code_recycle",                                                                                                                                                                                                    \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_trap_code_recycle[unit]),                                                                                                                                                                                \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_SNOOP,                                                                                                                                                                   \
                                  "oam_trap_code_snoop",                                                                                                                                                                                                      \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_trap_code_snoop[unit]),                                                                                                                                                                                  \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD,                                                                                                                                                                   \
                                  "oam_trap_code_frwrd",                                                                                                                                                                                                      \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_trap_code_frwrd[unit]),                                                                                                                                                                                  \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_DROP,                                                                                                                                                                    \
                                  "oam_trap_code_drop",                                                                                                                                                                                                       \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_trap_code_drop[unit]),                                                                                                                                                                                   \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL,                                                                                                                                                       \
                                  "oam_trap_code_trap_to_cpu_level",                                                                                                                                                                                          \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_trap_code_trap_to_cpu_level[unit]),                                                                                                                                                                      \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE,                                                                                                                                                     \
                                  "oam_trap_code_trap_to_cpu_passive",                                                                                                                                                                                        \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_trap_code_trap_to_cpu_passive[unit]),                                                                                                                                                                    \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_TRAP_TO_CPU,                                                                                                                                                             \
                                  "bfd_trap_code_trap_to_cpu",                                                                                                                                                                                                \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_bfd_trap_code_trap_to_cpu[unit]),                                                                                                                                                                            \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_CC_MPLS_TP,                                                                                                                                                     \
                                  "bfd_trap_code_oamp_bfd_cc_mpls_tp",                                                                                                                                                                                        \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_bfd_trap_code_oamp_bfd_cc_mpls_tp[unit]),                                                                                                                                                                    \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
		SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_IPV4,                                                                                                                                                           \
                                  "bfd_trap_code_oamp_bfd_ipv4",                                                                                                                                                                                              \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_bfd_trap_code_oamp_bfd_ipv4[unit]),                                                                                                                                                                          \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_MPLS,                                                                                                                                                       \
                                  "bfd_trap_code_oamp_bfd_mpls",                                                                                                                                                                                              \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_bfd_trap_code_oamp_bfd_mpls[unit]),                                                                                                                                                                          \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_PWE,                                                                                                                                                        \
                                  "bfd_trap_code_oamp_bfd_pwe",                                                                                                                                                                                               \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_bfd_trap_code_oamp_bfd_pwe[unit]),                                                                                                                                                                           \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_SNOOP_TO_CPU,                                                                                                                                                   \
                                  "oam_mirror_profile_snoop_to_cpu",                                                                                                                                                                                          \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_mirror_profile_snoop_to_cpu[unit]),                                                                                                                                                                      \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_RECYCLE,                                                                                                                                                        \
                                  "oam_mirror_profile_recycle",                                                                                                                                                                                               \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_mirror_profile_recycle[unit]),                                                                                                                                                                           \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_LEVEL,                                                                                                                                                      \
                                  "oam_mirror_profile_err_level",                                                                                                                                                                                             \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_mirror_profile_err_level[unit]),                                                                                                                                                                         \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE,                                                                                                                                                    \
                                  "oam_mirror_profile_err_passive",                                                                                                                                                                                           \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_oam_mirror_profile_err_passive[unit]),                                                                                                                                                                       \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID,                                                                                                                                                            \
                                  "tcam_last_entry_id",                                                                                                                                                                                                       \
                                  SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                       \
                                  sizeof(uint32),                                                                                                                                                                                                             \
                                  &(_dpp_arad_pp_tcam_last_entry_id[unit]),                                                                                                                                                                                   \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        /* OAM hash */                                                                                                                                                                                                                                        \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_MA_NAME,                                                                                                                                                                    \
                     "soc_dpp_wb_engine_var_oam_ma_name_hash_table",                                                                                                                                                                                          \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                                    \
                     SOC_DPP_WB_HASH_TBLS_OAM_MA_NAME,                                                                                                                                                                                                        \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_MA_TO_MEP,                                                                                                                                                                  \
                     "soc_dpp_wb_engine_var_oam_ma_to_mep_hash_table",                                                                                                                                                                                        \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                                    \
                     SOC_DPP_WB_HASH_TBLS_OAM_MA_TO_MEP,                                                                                                                                                                                                      \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_BFD_MEP_INFO,                                                                                                                                                               \
                     "soc_dpp_wb_engine_var_oam_bfd_mep_info_hash_table",                                                                                                                                                                                     \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                                    \
                     SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_INFO,                                                                                                                                                                                                   \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
        /*SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_BFD_MEP_TO_RMEP,                                                                                                                                                          \
                     "soc_dpp_wb_engine_var_oam_bfd_mep_to_rmep_hash_table",                                                                                                                                                                                  \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                                    \
                     SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_TO_RMEP,                                                                                                                                                                                                \
                     VERSION(1));*/                                                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_BFD_RMEP_INFO,                                                                                                                                                              \
                     "soc_dpp_wb_engine_var_oam_bfd_rmep_info_hash_table",                                                                                                                                                                                    \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM,                                                                                                                                                                                                    \
                     SOC_DPP_WB_HASH_TBLS_OAM_BFD_RMEP_INFO,                                                                                                                                                                                                  \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
		/* OAM end */                                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_INIT,                                                                                                                                                              \
                                  "egress_encap_init",                                                                                                                                                                                                        \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  &(_dpp_am_egress_encap[unit].init),                                                                                                                                                                                         \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_COUNT,                                                                                                                                                             \
                                  "egress_encap_count",                                                                                                                                                                                                       \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  &(_dpp_am_egress_encap[unit].egress_encap_count),                                                                                                                                                                           \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_BANKS,                                                                                                                                                             \
                                  "egress_encap_banks",                                                                                                                                                                                                       \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  SHR_BITALLOCSIZE(SOC_PPD_EG_ENCAP_NOF_BANKS(unit)*3), /*imitates SHR_BITALLOCSIZE(_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS)*/                                                                                                            \
                                  _dpp_am_egress_encap[unit].egress_encap_banks,                                                                                                                                                                              \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_INIT,                                                                                                                                                               \
                                  "ingress_lif_init",                                                                                                                                                                                                         \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  &(_dpp_am_ingress_lif[unit].init),                                                                                                                                                                                          \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_COUNT,                                                                                                                                                              \
                                  "ingress_lif_count",                                                                                                                                                                                                        \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  &(_dpp_am_ingress_lif[unit].ingress_lif_count),                                                                                                                                                                             \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_BANKS,                                                                                                                                                              \
                                  "ingress_lif_banks",                                                                                                                                                                                                        \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  SHR_BITALLOCSIZE(SOC_PPD_EG_ENCAP_NOF_BANKS(unit)*5),                                                                                                                \
                                  _dpp_am_ingress_lif[unit].ingress_lif_banks,                                                                                                                                                                                \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_SYNC_LIF_INIT,                                                                                                                                                                  \
                                  "sync_lif_init",                                                                                                                                                                                                            \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  sizeof(uint8),                                                                                                                                                                                                              \
                                  &(_dpp_am_sync_lif[unit].init),                                                                                                                                                                                             \
                                  VERSION(1));                                                                                                                                                                                                                \
                                                                                                                                                                                                                                                              \
        SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_SYNC_LIF_BANKS,                                                                                                                                                                 \
                                  "sync_lif_banks",                                                                                                                                                                                                           \
                                  SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,                                                                                                                                                                                        \
                                  SHR_BITALLOCSIZE(SOC_PPD_EG_ENCAP_NOF_BANKS(unit)*2), /*imitates SHR_BITALLOCSIZE(_BCM_DPP_AM_SYNC_LIF_NOF_BITS)*/                                                                                                                \
                                  _dpp_am_sync_lif[unit].sync_lif_banks,                                                                                                                                                                                      \
                                  VERSION(1));                                                                                                                                                                                                                \
    }                                                                                                                                                                                                                                                         \
	                                                                                                                                                                                                                                                            \
    /*ARAD_SW_DB pmf*/                                                                                                                                                                                                                                        \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_DB_INFO,                                                                                                                                                                           \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_db_info",                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_PMF_DB_INFO),                                                                                                                                                                                                 \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].db_info[0]),                                                                                                                                                                       \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_DBS,                                                                                                                                                                                                         \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_DB_PMB,                                                                                                                                                                        \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pgm_db_pmb",                                                                                                                                                                              \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)),                                                                                                                                                                     \
                              Arad_sw_db.arad_device_sw_db[unit]->pmf[0].pgm_db_pmb[0],                                                                                                                                                                       \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_PROGS,                                                                                                                                                                                                       \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_PFG_DBS,                                                                                                                                                                       \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_psl_pfg_dbs",                                                                                                                                                                             \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)),                                                                                                                                                                     \
                              Arad_sw_db.arad_device_sw_db[unit]->pmf[0].psl_info.pfgs_db_pmb[0],                                                                                                                                                             \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_PMF_NOF_GROUPS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_DEFAULT_DBS,                                                                                                                                                                      \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_psl_default_dbs",                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)),                                                                                                                                                                     \
                              Arad_sw_db.arad_device_sw_db[unit]->pmf[0].psl_info.default_db_pmb,                                                                                                                                                             \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_PFG_TMS,                                                                                                                                                                          \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_psl_pfg_tms",                                                                                                                                                                             \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_BIT_TO_U32(ARAD_PMF_NOF_GROUPS)),                                                                                                                                                                        \
                              Arad_sw_db.arad_device_sw_db[unit]->pmf[0].psl_info.pfgs_tm_bmp,                                                                                                                                                                \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_LEVELS_INFO,                                                                                                                                                                   \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_psl_levels_info",                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_SW_DB_PMF_PSL_LEVEL_INFO) * ARAD_PMF_NOF_LEVELS_MAX_ALL_STAGES),                                                                                                                                                   \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].psl_info.levels_info[0][0]),                                                                                                                                                       \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              2,                                                                                                                                                                                                                              \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PSL_INIT_INFO,                                                                                                                                                                        \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_psl_init_info",                                                                                                                                                                           \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              sizeof(ARAD_PMF_SEL_INIT_INFO),                                                                                                                                                                                                 \
                              &Arad_sw_db.arad_device_sw_db[unit]->pmf[0].psl_info.init_info,                                                                                                                                                                 \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE,                                                                                                                                                                            \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pgm_ce",                                                                                                                                                                                  \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_SW_DB_PMF_CE) * ARAD_SW_DB_PMF_NOF_CYCLES * ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS),                                                                                                                     \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].pgm_ce[0][0][0]),                                                                                                                                                                  \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_PROGS,                                                                                                                                                                                                       \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_FES,                                                                                                                                                                           \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pgm_fes",                                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_SW_DB_PMF_FES) * ARAD_SW_DB_PMF_NOF_FES),                                                                                                                                                                          \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].pgm_fes[0][0]),                                                                                                                                                                    \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_PROGS,                                                                                                                                                                                                       \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FEM_ENTRY,                                                                                                                                                                         \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_fem_entry",                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_PP_FP_FEM_ENTRY)),                                                                                                                                                                                                 \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fem_entry[0]),                                                                                                                                                                     \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_PMF_LOW_LEVEL_NOF_FEMS,                                                                                                                                                                                                    \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_CE_RSRC,                                                                                                                                                                       \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pgm_ce_rsrc",                                                                                                                                                                             \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_SW_DB_PMF_NOF_CYCLES),                                                                                                                                                                                   \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].rsources.ce[0][0]),                                                                                                                                                                \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_PROGS,                                                                                                                                                                                                       \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_KEY_RSRC,                                                                                                                                                                      \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pgm_key_rsrc",                                                                                                                                                                            \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_SW_DB_PMF_NOF_CYCLES),                                                                                                                                                                                   \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].rsources.key[0][0]),                                                                                                                                                               \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_PROGS,                                                                                                                                                                                                       \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PGM_PROGRAM_RSRC,                                                                                                                                                                     \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pgm_program_rsrc",                                                                                                                                                                        \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32) * ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_PROGS)),                                                                                                                                                                   \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].rsources.progs[0]),                                                                                                                                                                \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_INFO,                                                                                                                                                                        \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_fp_db_info",                                                                                                                                                                              \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_PP_FP_DATABASE_INFO)),                                                                                                                                                                                             \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.db_info[0]),                                                                                                                                                               \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_PP_FP_NOF_DBS,                                                                                                                                                                                                             \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRIES,                                                                                                                                                                     \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_fp_db_entries",                                                                                                                                                                           \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_SW_DB_FP_ENTRY)),                                                                                                                                                                                                  \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.db_entries[0]),                                                                                                                                                            \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_SW_DB_PMF_NOF_DBS,                                                                                                                                                                                                         \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_ENTRY_BITMAP,                                                                                                                                                                \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_fp_db_entry_bitmap",                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint8) * SOC_DPP_DEFS_GET_TCAM_NOF_LINES_IN_BYTES(unit)),                                                                                                                                                                                 \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.entry_bitmap[0][0]),                                                                                                                                                       \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_DB_UDF,                                                                                                                                                                         \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_fp_db_udf",                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(ARAD_PMF_CE_QUAL_INFO)),                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.udf[0]),                                                                                                                                                                   \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_PP_FP_NOF_HDR_USER_DEFS,                                                                                                                                                                                                   \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_INNER_ETH_NOF_TAGS,                                                                                                                                                                \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_inner_eth_nof_tags",                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.inner_eth_nof_tags[0]),                                                                                                                                                    \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_PMF_NOF_GROUPS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_FP_KEY_CHANGE_SIZE,                                                                                                                                                                   \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_fp_key_change_size",                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.key_change_size),                                                                                                                                                          \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_PMF_PFG_INFO,                                                                                                                                                                          \
                              "soc_dpp_wb_engine_var_arad_sw_db_pmf_pfg_info",                                                                                                                                                                                \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_PMF,                                                                                                                                                                                        \
                              (sizeof(SOC_TMC_PMF_PFG_INFO)),                                                                                                                                                                                                 \
                              &(Arad_sw_db.arad_device_sw_db[unit]->pmf[0].fp_info.pfg_info[0]),                                                                                                                                                              \
                              ARAD_NOF_FP_DATABASE_STAGES,                                                                                                                                                                                                    \
                              ARAD_PMF_NOF_GROUPS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_PMF),                                                                                                                                                                                                         \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /* TCAM restoration data */                                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_2D_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_OCC_BM_RESTORE,                                                                                                                                                             \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_occ_bm_restore",                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA,                                                                                                                                                                      \
                              (sizeof(ARAD_TCAM_OCC_BM_RESTORE)),                                                                                                                                                                                             \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data.entries_used[0][0]),                                                                                                                                                \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              2,                                                                                                                                                                                                                              \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_2D_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_OCC_BM_RESTORE,                                                                                                                                                     \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_bank_occ_bm_restore",                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA,                                                                                                                                                                      \
                              (sizeof(ARAD_TCAM_OCC_BM_RESTORE)),                                                                                                                                                                                             \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data.db_entries_used[0][0]),                                                                                                                                             \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SORTED_LIST_RESTORE,                                                                                                                                                           \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_sorted_list_restore",                                                                                                                                                                    \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA,                                                                                                                                                                      \
                              sizeof(ARAD_TCAM_SORTED_LIST_RESTORE),                                                                                                                                                                                          \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.restoration_data.priorities[0]),                                                                                                                                                     \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /*ARAD_SW_DB_TM*/                                                                                                                                                                                                                                         \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TM_POWER_SAVE_INIT_CALLED,                                                                                                                                                          \
                              "soc_dpp_wb_engine_var_arad_sw_db_tm_is_power_saving_called",                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TM,                                                                                                                                                                                         \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tm.is_power_saving_called),                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /* TCAM */                                                                                                                                                                                                                                                \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_VALID,                                                                                                                                                                          \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_bank_valid",                                                                                                                                                                             \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[0].valid),                                                                                                                                                                      \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_BANK),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_HAS_DIRECT_TABLE,                                                                                                                                                               \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_bank_has_direct_table",                                                                                                                                                                  \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[0].has_direct_table),                                                                                                                                                           \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_BANK),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRY_SIZE,                                                                                                                                                                     \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_bank_entry_size",                                                                                                                                                                        \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(ARAD_TCAM_BANK_ENTRY_SIZE)),                                                                                                                                                                                            \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[0].entry_size),                                                                                                                                                                 \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_BANK),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_NOF_ENTRIES_FREE,                                                                                                                                                               \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_bank_nof_entries_free",                                                                                                                                                                  \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[0].nof_entries_free),                                                                                                                                                           \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_BANK),                                                                                                                                                                                                   \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_VALID,                                                                                                                                                                            \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_valid",                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].valid),                                                                                                                                                                   \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_IS_DIRECT,                                                                                                                                                                        \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_is_direct",                                                                                                                                                                           \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].is_direct),                                                                                                                                                               \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ENTRY_SIZE,                                                                                                                                                                       \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_entry_size",                                                                                                                                                                          \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(ARAD_TCAM_BANK_ENTRY_SIZE)),                                                                                                                                                                                            \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].entry_size),                                                                                                                                                              \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ACTION_BITMAP_NDX,                                                                                                                                                                \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_action_bitmap_ndx",                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(ARAD_TCAM_ACTION_SIZE)),                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].action_bitmap_ndx),                                                                                                                                                       \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_PREFIX_SIZE,                                                                                                                                                                      \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_prefix_size",                                                                                                                                                                         \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].prefix_size),                                                                                                                                                             \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_USED,                                                                                                                                                                     \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_bank_used",                                                                                                                                                                           \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].bank_used[0]),                                                                                                                                                            \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_USE_SMALL_BANKS,                                                                                                                                                                     \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_use_small_banks",                                                                                                                                                                        \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(ARAD_TCAM_SMALL_BANKS)),                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].use_small_banks),                                                                                                                                                         \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_NO_INSERTION_PRIORITY_ORDER,                                                                                                                                                         \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_no_insertion_priority_order",                                                                                                                                                            \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].no_insertion_priority_order),                                                                                                                                             \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SPARSE_PRIORITIES,                                                                                                                                                                   \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_sparse_priorities",                                                                                                                                                                      \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].sparse_priorities),                                                                                                                                                       \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_NOF_ENTRIES,                                                                                                                                                              \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_bank_nof_entries",                                                                                                                                                                    \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].bank_nof_entries[0]),                                                                                                                                                     \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_ACCESS_PROFILE_ID,                                                                                                                                                             \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_access_profile_id",                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].access_profile_id[0]),                                                                                                                                                    \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX,                                                                                                                                                                                             \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_FORBIDDEN_DBS,                                                                                                                                                                    \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_forbidden_dbs",                                                                                                                                                                       \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(uint32) * ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)),                                                                                                                                                                    \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].forbidden_dbs[0]),                                                                                                                                                        \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_PREFIX,                                                                                                                                                                           \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_prefix",                                                                                                                                                                              \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM,                                                                                                                                                                                       \
                              (sizeof(ARAD_TCAM_PREFIX)),                                                                                                                                                                                                     \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[0].prefix),                                                                                                                                                                  \
                              ARAD_TCAM_MAX_NOF_LISTS,                                                                                                                                                                                                        \
                              sizeof(ARAD_SW_DB_TCAM_DB),                                                                                                                                                                                                     \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /* TCAM_MGMT */                                                                                                                                                                                                                                           \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_VALID,                                                                                                                                                                \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_access_profile_valid",                                                                                                                                                                   \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(uint8)),                                                                                                                                                                                                                \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.profiles[0].valid),                                                                                                                                                             \
                              ARAD_TCAM_NOF_ACCESS_PROFILE_IDS,                                                                                                                                                                                               \
                              sizeof(ARAD_SW_DB_TCAM_ACCESS_PROFILE),                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_MIN_BANKS,                                                                                                                                                            \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_access_profile_min_banks",                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.profiles[0].min_banks),                                                                                                                                                         \
                              ARAD_TCAM_NOF_ACCESS_PROFILE_IDS,                                                                                                                                                                                               \
                              sizeof(ARAD_SW_DB_TCAM_ACCESS_PROFILE),                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_BANK_OWNER,                                                                                                                                                           \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_access_profile_bank_owner",                                                                                                                                                              \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(ARAD_TCAM_BANK_OWNER)),                                                                                                                                                                                                 \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.profiles[0].bank_owner),                                                                                                                                                        \
                              ARAD_TCAM_NOF_ACCESS_PROFILE_IDS,                                                                                                                                                                                               \
                              sizeof(ARAD_SW_DB_TCAM_ACCESS_PROFILE),                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_ACCESS_PROFILE_USER_DATA,                                                                                                                                                            \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_access_profile_user_data",                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.profiles[0].user_data),                                                                                                                                                         \
                              ARAD_TCAM_NOF_ACCESS_PROFILE_IDS,                                                                                                                                                                                               \
                              sizeof(ARAD_SW_DB_TCAM_ACCESS_PROFILE),                                                                                                                                                                                         \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_2D_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_PREFIX_DB,                                                                                                                                                           \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_managed_bank_prefix_db",                                                                                                                                                                 \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.banks[0].prefix_db[0]),                                                                                                                                                         \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              ARAD_TCAM_NOF_PREFIXES,                                                                                                                                                                                                         \
                              sizeof(ARAD_SW_DB_TCAM_MANAGED_BANK),                                                                                                                                                                                           \
                              _NO_JUMP_NEEDED_,                                                                                                                                                                                                               \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_NOF_DB,                                                                                                                                                                 \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_managed_bank_nof_db",                                                                                                                                                                    \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.banks[0].nof_dbs),                                                                                                                                                              \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_MANAGED_BANK),                                                                                                                                                                                           \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_MANAGED_BANK_NOF_DB,                                                                                                                                                                 \
                              "soc_dpp_wb_engine_var_arad_sw_db_tcam_managed_bank_nof_db",                                                                                                                                                                    \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_MGMT,                                                                                                                                                                                  \
                              (sizeof(uint32)),                                                                                                                                                                                                               \
                              &(Arad_sw_db.arad_device_sw_db[unit]->tcam_mgmt.banks[0].nof_dbs),                                                                                                                                                              \
                              SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS,                                                                                                                                                                                                            \
                              sizeof(ARAD_SW_DB_TCAM_MANAGED_BANK),                                                                                                                                                                                           \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    /* add all TCAM location tables*/                                                                                                                                                                                                                         \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_LOCATION_TABLE,                                                                                                                                                                \
                          "soc_dpp_wb_engine_var_arad_sw_db_tcam_location_table",                                                                                                                                                                             \
                          SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_LOCATION_TBL,                                                                                                                                                                              \
                          sizeof(ARAD_TCAM_LOCATION),                                                                                                                                                                                                         \
                          Arad_sw_db.arad_device_sw_db[unit]->tcam.db_location_tbl,                                                                                                                                                                           \
                          SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit),                                                                                                                                                                                                      \
                          VERSION(1));                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TABLE,                                                                                                                                                         \
                          "soc_dpp_wb_engine_var_arad_sw_db_tcam_global_location_table",                                                                                                                                                                      \
                          SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_GLOBAL_LOCATION_TBL,                                                                                                                                                                       \
                          sizeof(ARAD_TCAM_GLOBAL_LOCATION),                                                                                                                                                                                                  \
                          Arad_sw_db.arad_device_sw_db[unit]->tcam.global_location_tbl,                                                                                                                                                                       \
                          SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit),                                                                                                                                                                                                      \
                          VERSION(1));                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
    /* add tcam occ_bms */                                                                                                                                                                                                                                    \
    for (i = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED; i <= SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_LAST; i+=WB_ENGINE_OCC_BM_INNER_VARS_NUM ) {                                                                               \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM_DYNAMIC(i,                                                                                                                                                                                                    \
                             "soc_dpp_wb_engine_var_arad_sw_db_tcam_bank_entries_used",                                                                                                                                                                       \
                             SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC,                                                                                                                                                                        \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED)/WB_ENGINE_OCC_BM_INNER_VARS_NUM)].entries_used[0]),                                                                  \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED)/WB_ENGINE_OCC_BM_INNER_VARS_NUM)].entries_used[0]->buffer_size),                                                     \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED)/WB_ENGINE_OCC_BM_INNER_VARS_NUM)].entries_used[0]->support_cache),                                                   \
                             VERSION(1));                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    for (i = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_INVERSE; i <= SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_INVERSE_LAST; i+=WB_ENGINE_OCC_BM_INNER_VARS_NUM ) {                                                               \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM_DYNAMIC(i,                                                                                                                                                                                                    \
                             "soc_dpp_wb_engine_var_arad_sw_db_tcam_bank_entries_used1",                                                                                                                                                                      \
                             SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC,                                                                                                                                                                        \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_INVERSE)/WB_ENGINE_OCC_BM_INNER_VARS_NUM)].entries_used[1]),                                                          \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_INVERSE)/WB_ENGINE_OCC_BM_INNER_VARS_NUM)].entries_used[1]->buffer_size),                                             \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.bank[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_INVERSE)/WB_ENGINE_OCC_BM_INNER_VARS_NUM)].entries_used[1]->support_cache),                                           \
                             VERSION(1));                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    for (i = SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_ENTRIES_USED; i <= SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_ENTRIES_USED_LAST; i+= (WB_ENGINE_OCC_BM_INNER_VARS_NUM * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS)) {                                                 \
        for (j = 0; j < (SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit) * WB_ENGINE_OCC_BM_INNER_VARS_NUM); j+=WB_ENGINE_OCC_BM_INNER_VARS_NUM) {                                                                                                                                        \
            SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM_DYNAMIC(i + j,                                                                                                                                                                                            \
                 "soc_dpp_wb_engine_var_arad_sw_db_tcam_db_bank_entries_used",                                                                                                                                                                                \
                 SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_DB_BANK_OCC_BMS_DYNAMIC,                                                                                                                                                                            \
                 (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_ENTRIES_USED)/(WB_ENGINE_OCC_BM_INNER_VARS_NUM * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS))].entries_used[j/WB_ENGINE_OCC_BM_INNER_VARS_NUM]),                \
                 (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_ENTRIES_USED)/(WB_ENGINE_OCC_BM_INNER_VARS_NUM * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS))].entries_used[j/WB_ENGINE_OCC_BM_INNER_VARS_NUM]->buffer_size),   \
                 (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_ENTRIES_USED)/(WB_ENGINE_OCC_BM_INNER_VARS_NUM * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS))].entries_used[j/WB_ENGINE_OCC_BM_INNER_VARS_NUM]->support_cache), \
                 VERSION(1));                                                                                                                                                                                                                                 \
        }                                                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_TCAM_DB_ENTRY_ID_TO_LOCATION,                                                                                                                                                           \
                         "soc_dpp_wb_engine_var_tcam_db_entry_id_to_location",                                                                                                                                                                                \
                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_HASH_TBL,                                                                                                                                                                                   \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location),                                                                                                                                                                     \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location.init_info.table_size),                                                                                                                                                \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location.init_info.key_size),                                                                                                                                                  \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location.hash_data.ptr_size),                                                                                                                                                  \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location.init_info.table_width),                                                                                                                                               \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location.hash_data.memory_use->buffer_size),                                                                                                                                   \
                         (Arad_sw_db.arad_device_sw_db[unit]->tcam.entry_id_to_location.hash_data.memory_use->support_cache),                                                                                                                                 \
                         VERSION(1));                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    for (i = SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY; i <= SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY_LAST; i+=WB_ENGINE_SORTED_LIST_INNER_VARS_NUM ) {                                                                                                            \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_SORTED_LIST_DYNAMIC(i,                                                                                                                                                                                               \
                             "soc_dpp_wb_engine_var_tcam_db_priority",                                                                                                                                                                                        \
                             SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_SORTED_LIST_DYNAMIC,                                                                                                                                                                    \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities),                                                                                \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities.init_info.key_size),                                                             \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities.init_info.list_size),                                                            \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities.list_data.ptr_size),                                                             \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities.init_info.data_size),                                                            \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities.list_data.memory_use->buffer_size),                                              \
                             (Arad_sw_db.arad_device_sw_db[unit]->tcam.tcam_db[((i-SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY)/WB_ENGINE_SORTED_LIST_INNER_VARS_NUM)].priorities.list_data.memory_use->support_cache),                                            \
                             VERSION(1));                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    /* arad_sw_db module - sand data structs */                                                                                                                                                                                                               \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_CHANIF2CHAN_ARB_OCC,                                                                                                                                                   \
                     "soc_dpp_wb_engine_var_arad_sw_db_egr_ports_chanif2chan_arb_occ",                                                                                                                                                                        \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                                   \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ),                                                                                                                                                          \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ->buffer_size),                                                                                                                                             \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.chanif2chan_arb_occ->support_cache),                                                                                                                                           \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_NONCHANIF2SCH_OFFSET_OCC,                                                                                                                                              \
                     "soc_dpp_wb_engine_var_arad_sw_db_egr_ports_nonchanif2sch_offset_occ",                                                                                                                                                                   \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                                   \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ),                                                                                                                                                     \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ->buffer_size),                                                                                                                                        \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.nonchanif2sch_offset_occ->support_cache),                                                                                                                                      \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_SRC_BINDS_SRCBIND_ARB_OCC,                                                                                                                                            \
                     "soc_dpp_wb_engine_var_arad_sw_db_arad_sw_db_src_binds_srcbind_arb_occ",                                                                                                                                                                 \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND,                                                                                                                                                                                            \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ),                                                                                                                                                              \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ->buffer_size),                                                                                                                                                 \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.srcbind_arb_occ->support_cache),                                                                                                                                               \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_REASSEMBLY_CONTEXT_OCC,                                                                                                                                               \
                     "soc_dpp_wb_engine_var_arad_sw_db_arad_sw_db_src_reassembly_context_occ",                                                                                                                                                                \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_REASSEMBLY_CONTEXT,                                                                                                                                                                                  \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ),                                                                                                                                                    \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ->buffer_size),                                                                                                                                       \
                     (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_reassembly_ctxt.reassembly_ctxt_occ->support_cache),                                                                                                                                     \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                         \
        int32 var[] = {                                                                                                                                                                                                                                       \
            SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_CHANNELIZED_CALS_OCC_CORE0,                                                                                                                                                                           \
            SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_CHANNELIZED_CALS_OCC_CORE1,                                                                                                                                                                           \
        };                                                                                                                                                                                                                                                    \
        uint32 core_idx;                                                                                                                                                                                                                                      \
        for(core_idx = 0; core_idx < SOC_DPP_DEFS_GET(unit, nof_cores); ++core_idx) {                                                                                                                                                                         \
            SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(var[core_idx],                                                                                                                                                                                            \
                             "soc_dpp_wb_engine_var_arad_sw_db_arad_sw_db_channelized_cals_occ",                                                                                                                                                              \
                             SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                            \
                             (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core_idx]),                                                                                                                                       \
                             (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core_idx]->buffer_size),                                                                                                                          \
                             (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.channelized_cals_occ[core_idx]->support_cache),                                                                                                                        \
                             VERSION(1));                                                                                                                                                                                                                     \
        }                                                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                         \
        int32 var[] = {                                                                                                                                                                                                                                       \
            SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_CHANNELIZED_CALS_OCC_CORE0,                                                                                                                                                                  \
            SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_CHANNELIZED_CALS_OCC_CORE1,                                                                                                                                                                  \
        };                                                                                                                                                                                                                                                    \
        uint32 core_idx;                                                                                                                                                                                                                                      \
        for(core_idx = 0; core_idx < SOC_DPP_DEFS_GET(unit, nof_cores); ++core_idx) {                                                                                                                                                                         \
            SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(var[core_idx],                                                                                                                                                                                            \
                             "soc_dpp_wb_engine_var_arad_sw_db_arad_sw_db_modified_channelized_cals_occ",                                                                                                                                                     \
                             SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                   \
                             (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core_idx]),                                                                                                                              \
                             (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core_idx]->buffer_size),                                                                                                                 \
                             (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_channelized_cals_occ[core_idx]->support_cache),                                                                                                               \
                             VERSION(1));                                                                                                                                                                                                                     \
        }                                                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                         \
       int32 var[] = {                                                                                                                                                                                                                                        \
           SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_E2E_INTERFACES_OCC_CORE0,                                                                                                                                                                              \
           SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_E2E_INTERFACES_OCC_CORE1,                                                                                                                                                                              \
       };                                                                                                                                                                                                                                                     \
       uint32 core_idx;                                                                                                                                                                                                                                       \
       for(core_idx = 0; core_idx < SOC_DPP_DEFS_GET(unit, nof_cores); ++core_idx) {                                                                                                                                                                          \
           SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(var[core_idx],                                                                                                                                                                                             \
                            "soc_dpp_wb_engine_var_arad_sw_db_arad_sw_db_e2e_interfaces_occ",                                                                                                                                                                 \
                            SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                               \
                            (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core_idx]),                                                                                                                                          \
                            (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core_idx]->buffer_size),                                                                                                                             \
                            (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.e2e_interfaces_occ[core_idx]->support_cache),                                                                                                                           \
                            VERSION(1));                                                                                                                                                                                                                      \
       }                                                                                                                                                                                                                                                      \
   }                                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
   {                                                                                                                                                                                                                                                          \
      int32 var[] = {                                                                                                                                                                                                                                         \
          SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_E2E_INTERFACES_OCC_CORE0,                                                                                                                                                                      \
          SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_E2E_INTERFACES_OCC_CORE1,                                                                                                                                                                      \
      };                                                                                                                                                                                                                                                      \
      uint32 core_idx;                                                                                                                                                                                                                                        \
      for(core_idx = 0; core_idx < SOC_DPP_DEFS_GET(unit, nof_cores); ++core_idx) {                                                                                                                                                                           \
          SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_OCC_BM(var[core_idx],                                                                                                                                                                                              \
                           "soc_dpp_wb_engine_var_arad_sw_db_arad_sw_db_e2e_interfaces_occ",                                                                                                                                                                  \
                           SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                       \
                           (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core_idx]),                                                                                                                                  \
                           (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core_idx]->buffer_size),                                                                                                                     \
                           (Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.modified_e2e_interfaces_occ[core_idx]->support_cache),                                                                                                                   \
                           VERSION(1));                                                                                                                                                                                                                       \
      }                                                                                                                                                                                                                                                       \
  }                                                                                                                                                                                                                                                           \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_VTT_ISEM_KEY_TO_ENTRY_ID,                                                                                                                                                    \
                     "soc_dpp_wb_engine_var_arad_sw_db_vtt_isem_key_to_entry_id",                                                                                                                                                                             \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                                   \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id),                                                                                                                                                                          \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id.init_info.table_size),                                                                                                                                                     \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id.init_info.key_size),                                                                                                                                                       \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id.hash_data.ptr_size),                                                                                                                                                       \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id.init_info.table_width),                                                                                                                                                    \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id.hash_data.memory_use->buffer_size),                                                                                                                                        \
                     (Arad_sw_db.arad_device_sw_db[unit]->vtt.isem_key_to_entry_id.hash_data.memory_use->support_cache),                                                                                                                                      \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_FRWRD_IP_ROUTE_KEY_TO_ENTRY_ID,                                                                                                                                              \
                     "soc_dpp_wb_engine_var_arad_sw_db_frwrd_ip_route_key_to_entry_id",                                                                                                                                                                       \
                     SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS,                                                                                                                                                                                   \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id),                                                                                                                                                                    \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id.init_info.table_size),                                                                                                                                               \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id.init_info.key_size),                                                                                                                                                 \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id.hash_data.ptr_size),                                                                                                                                                 \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id.init_info.table_width),                                                                                                                                              \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id.hash_data.memory_use->buffer_size),                                                                                                                                  \
                     (Arad_sw_db.arad_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id.hash_data.memory_use->support_cache),                                                                                                                                \
                     VERSION(1));                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_PORT_NUM_TPIDS,                                                                                                                                                                                \
                              "port_num_tpids",                                                                                                                                                                                                               \
                              SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT,                                                                                                                                                                                             \
                              sizeof(uint8),                                                                                                                                                                                                                  \
                              &bcm_dpp_port_tpids_count[unit][0],                                                                                                                                                                                             \
                              SOC_TMC_NOF_FAP_PORTS_MAX,                                                                                                                                                                                                      \
                              VERSION(1));                                                                                                                                                                                                                    \
                                                                                                                                                                                                                                                              \
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {                                                                                                                                                            \
      SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_RIF_URPF_MODE,                                                                                                                                                                       \
                                "rif_urpf_mode",                                                                                                                                                                                                              \
                                SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP,                                                                                                                                                                        \
                                sizeof(*(PP_SW_DB_DEVICE->rif_to_lif_group_map->rif_urpf_mode)),                                                                                                                                                              \
                                PP_SW_DB_DEVICE->rif_to_lif_group_map->rif_urpf_mode,                                                                                                                                                                         \
                                SOC_DPP_CONFIG(unit)->l3.nof_rifs,                                                                                                                                                                                            \
                                VERSION(1));                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                              \
      if (buffer_id == SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP) {                                                                                                                                                                               \
        SOC_SAND_GROUP_MEM_LL_INFO *group_info_ptr = &(PP_SW_DB_DEVICE->rif_to_lif_group_map->group_info);                                                                                                                                                    \
        SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_DB_GROUP_MEM_LL(                                                                                                                                                                                                     \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MEM_LL,                                                                                                                                      \
                                                         "rif_to_lif_group_mem_ll",                                                                                                                                                                           \
                                                         SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP,                                                                                                                                               \
                                                         group_info_ptr,                                                                                                                                                                                      \
                                                         group_info_ptr->nof_groups,                                                                                                                                                                          \
                                                         group_info_ptr->nof_elements,                                                                                                                                                                        \
                                                         group_info_ptr->support_caching,                                                                                                                \
                                                         VERSION(1));                                                                                                                                                                                         \
      }                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                         \
                                                                                                                                                                                                                                                              \
     SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE,                                                                                                                                                                \
                               "tm_guaranteed_q_resource",                                                                                                                                                                                                    \
                               SOC_DPP_WB_ENGINE_BUFFER_ARAD_TM_GUARANTEED_Q_RESOURCE,                                                                                                                                                                        \
                               sizeof(soc_dpp_guaranteed_q_resource_t),                                                                                                                                                                                       \
                               &(SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource),                                                                                                                                                                             \
                               VERSION(1));                                                                                                                                                                                                                   \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
    SOC_DPP_WB_ENGINE_ADD_VAR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_RESERVED_REASSEMBLY_CONTEXTS,                                                                                                                                                             \
        "Arad reserved reassembly contexts",                                                                                                                                                                                                                  \
        SOC_DPP_WB_ENGINE_BUFFER_ARAD_RESERVED_REASSEMBLY_CONTEXTS,                                                                                                                                                                                           \
        sizeof(Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_reserved_reassembly_context),                                                                                                                                                    \
        Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_egr_ports.port_reserved_reassembly_context,                                                                                                                                                            \
        VERSION(1));                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
      propval = soc_property_get_str(unit, spn_L3_SOURCE_BIND_MODE);                                                                                                                                                                                          \
      if (NULL != propval && 0 != sal_strcmp("DISABLE", propval)) {                                                                                                                                                                                           \
      SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_SRC_BIND_SPOOF_ID_REF_CNT,                                                                                                                                                        \
          "Spoof_id_ref_count",                                                                                                                                                                                                                               \
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND_SPOOF_ID_REF_CNT,                                                                                                                                                                                      \
          sizeof(uint16),                                                                                                                                                                                                                                     \
          Arad_sw_db.arad_device_sw_db[unit]->arad_sw_db_src_binds.spoof_id_ref_count,                                                                                                                                                                        \
          ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID,                                                                                                                                                                                                                 \
          VERSION(1));                                                                                                                                                                                                                                        \
      }                                                                                                                                                                                                                                                       \
                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                              \
      /* chip sim vars */                                                                                                                                                                                                                                     \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_READ_RESULT_ADDRESS,                                                                                                                                                           \
          "chip_sim_read_result_address",                                                                                                                                                                                                                     \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].read_result_address),                                                                                                                                                                                                     \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_OFFSET,                                                                                                                                                                        \
          "chip_sim_offset",                                                                                                                                                                                                                                  \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].offset),                                                                                                                                                                                                                  \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_TABLE_SIZE,                                                                                                                                                                    \
          "chip_sim_table_size",                                                                                                                                                                                                                              \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].table_size),                                                                                                                                                                                                              \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_KEY_SIZE,                                                                                                                                                                      \
          "chip_sim_KEY_SIZE",                                                                                                                                                                                                                                \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].key_size),                                                                                                                                                                                                                \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_DATA_NOF_BYTES,                                                                                                                                                                \
          "chip_sim_DATA_NOF_BYTES",                                                                                                                                                                                                                          \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].data_nof_bytes),                                                                                                                                                                                                          \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_START_ADDRESS,                                                                                                                                                                 \
          "chip_sim_START_ADDRESS",                                                                                                                                                                                                                           \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].start_address),                                                                                                                                                                                                           \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      SOC_DPP_WB_ENGINE_ADD_IMPL_ARR(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_END_ADDRESS,                                                                                                                                                                   \
          "chip_sim_END_ADDRESS",                                                                                                                                                                                                                             \
          SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                                  \
          sizeof(UINT32),                                                                                                                                                                                                                                     \
          &(Arad_em_blocks[unit][0].end_address),                                                                                                                                                                                                             \
          (ARAD_EM_TABLE_ID_LAST + 1),                                                                                                                                                                                                                        \
          sizeof(CHIP_SIM_EM_BLOCK),                                                                                                                                                                                                                          \
          VERSION(1));                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                              \
      /* add multiset one by one (should be already initialized */                                                                                                                                                                                            \
      for (i = SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET; i <= (SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET_LAST-1); i+=WB_ENGINE_MULTI_SET_INNER_VARS_NUM ) {                                                                                                        \
         SOC_DPP_WB_ENGINE_ADD_COMPLEX_DS_MULTI_SET(i,                                                                                                                                                                                                       \
                                                     "soc_dpp_wb_engine_var_chip_sim_multi_set",                                                                                                                                                              \
                                                     SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                       \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set,                                                                                       \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.init_info.nof_members,                                                                 \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.counter_size,                                                            \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.init_info.max_duplications,                                                            \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.hash_table.init_info.table_size,                                         \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.hash_table.init_info.key_size,                                           \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.hash_table.hash_data.ptr_size,                                           \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.hash_table.init_info.table_width,                                        \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.hash_table.hash_data.memory_use->buffer_size,                            \
                                                     Arad_em_blocks[unit][((i-SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET)/WB_ENGINE_MULTI_SET_INNER_VARS_NUM)].multi_set.multiset_data.hash_table.hash_data.memory_use->support_cache,                          \
                                                     VERSION(1));                                                                                                                                                                                             \
      }                                                                                                                                                                                                                                                       \
                                                                                                                                                                                                                                                              \
      /* add buffers separately as they may have different length */                                                                                                                                                                                          \
      /* length should be already determined at this point */                                                                                                                                                                                                 \
      for (i = 0; i < ARAD_EM_TABLE_ID_LAST; i++) {                                                                                                                                                                                                          \
         SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_BASE + i,                                                                                                                                                                        \
             "soc_dpp_wb_engine_var_chip_sim_base",                                                                                                                                                                                                           \
             SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM,                                                                                                                                                                                                               \
             sizeof(uint8),                                                                                                                                                                                                                                   \
             &(Arad_em_blocks[unit][i].base[0]),                                                                                                                                                                                                              \
             ((Arad_em_blocks[unit][i].end_address - Arad_em_blocks[unit][i].start_address + 1) * Arad_em_blocks[unit][i].data_nof_bytes),                                                                                                                    \
             VERSION(1));                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                 


#ifdef BCM_ARAD_SUPPORT
#define SOC_DPP_ALL_ARAD_VARS_DEPRECATED_CODE\
    SOC_DPP_ADD_SWITCH_VARS_DEPRECATED_CODE /* deprecated code */\
    SOC_DPP_ADD_KBP_VARS_DEPRECATED_CODE /* deprecated code */\
    SOC_DPP_ADD_ARAD_VARS_DEPRECATED_CODE /* deprecated code */
#else /*BCM_ARAD_SUPPORT*/
#define SOC_DPP_ALL_ARAD_VARS_DEPRECATED_CODE
#endif /*BCM_ARAD_SUPPORT*/

#ifdef BCM_PETRA_SUPPORT
#define SOC_DPP_ADD_PETRA_VARS_DEPRECATED_CODE \
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_COSQ_EGR_THRESH_MAPPING,\
                              "cosq_egr_thresh_mapping",\
                              SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,\
                              sizeof(int),\
                              &(_bcm_dpp_egr_thresh_map[unit][0]),\
                              SOC_TMC_NOF_FAP_PORTS_MAX,\
                              VERSION(1));	\
\
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_COSQ_EGR_INTERFACE_UNICAST_THRESH_MAPPING,\
                              "cosq_egr_interface_unicast_thresh_mapping",\
                              SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,\
                              sizeof(int),\
                              &(_bcm_dpp_egr_interface_unicast_thresh_map[unit][0]),\
                              SOC_TMC_IF_NOF_NIFS,\
                              VERSION(1));	\
\
    SOC_DPP_WB_ENGINE_ADD_ARR_DEPRECATED(SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_COSQ_EGR_INTERFACE_MULTICAST_THRESH_MAPPING,\
                              "cosq_egr_interface_multicast_thresh_mapping",\
                              SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR,\
                              sizeof(int),\
                              &(_bcm_dpp_egr_interface_multicast_thresh_map[unit][0]),\
                              SOC_TMC_IF_NOF_NIFS,\
                              VERSION(1));	
#else
#define SOC_DPP_ADD_PETRA_VARS_DEPRECATED_CODE 
#endif /*BCM_PETRA_SUPPORT*/


#define SOC_DPP_WB_ENGINE_DEPRECATED_CODE\
    /* --------- buffers inside these macros were added to wb_engine using an old implementation ----------- */\
    /* -- that is now deprected and shouldn't be used as an example implementation for adding new buffers -- */\
    SOC_DPP_ADD_ALLOC_MNGR_BUFF_DEPRECATED_CODE; /* deprecated code */\
    SOC_DPP_ADD_ARAD_BUFFS_DEPRECATED_CODE;      /* deprecated code */\
    /* ------------------------------------------------------------------------------------------------------ */\
\
    SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET;\
\
    /* --------- variables inside these macros were added to wb_engine using an old implementation ----------- */\
    /* -- that is now deprected and shouldn't be used as an example implementation for adding new variables -- */\
    SOC_DPP_ALL_ARAD_VARS_DEPRECATED_CODE\
    SOC_DPP_ADD_PETRA_VARS_DEPRECATED_CODE /* deprecated code */\
    /* ------------------------------------------------------------------------------------------------------- */


#endif /*_SOC_DPP_WB_ENGINE_DEPRECATED_H_*/
