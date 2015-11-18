/*
* $Id: dpp_wb_engine.h,v 1.58 Broadcom SDK $
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
* 
*/ 
#ifndef _SOC_DPP_WB_ENGINE_H_
#define _SOC_DPP_WB_ENGINE_H_

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/wb_db_hash_tbl.h>
#endif /*BCM_ARAD_SUPPORT*/

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/wb_db_hash_tbl.h>
#include <soc/dpp/port_map.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/TMC/tmc_api_general.h>
#endif /*BCM_PETRA_SUPPORT*/

#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/dpp_wb_engine_defs.h>
#include <shared/bitop.h>
#include <bcm_int/dpp/ipmc.h>


#define SOC_DPP_WB_ENGINE_DECLARATIONS_BEGIN\
    switch (buffer_id) {\
        case SOC_DPP_OLD_BUFFERS_DECS:\
            /* old buffers are handled in deprecated code */\
            break;\
        case SOC_DPP_WB_ENGINE_NOF_BUFFERS:\
            return SOC_E_INTERNAL;

#define SOC_DPP_WB_ENGINE_DECLARATIONS_END\
            break;\
        default:\
            return SOC_E_INTERNAL;\
    }


/********************************/
/* dynamic vars addition macros */
/********************************/



/********************************/
/* static vars addition macros  */
/********************************/

#define SOC_DPP_WB_ENGINE_ADD_BUFF(_buff, _buff_string, _upgrade_func, _version, _is_added_after_release)\
        break;\
    case _buff:\
        SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, _version, 0x1 /*stored only in scache buffer*/, _is_added_after_release)\
        SOCDNX_IF_ERR_EXIT(rv);\
        SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET;

/* this is used only for data structure buffers that do hold data on memory outside of scache buffer */
#define SOC_DPP_WB_ENGINE_ADD_BUFF_WITH_MEMORY_DUPLICATIONS(_buff, _buff_string, _upgrade_func, _version, _is_added_after_release)\
        break;\
    case _buff:\
        SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PRIMARY, _buff, _buff_string, _upgrade_func, _version, 0x0 /*copy is stored in scache buffer*/, _is_added_after_release)\
        SOCDNX_IF_ERR_EXIT(rv);\
        SOC_DPP_WB_ENGINE_DYNAMIC_VAR_STATE_GET;

/* ---------most-usefull----------- */
#define SOC_DPP_WB_ENGINE_ADD_VAR(_var, _var_string, _buffer, _data_size, _version_added)\
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, NULL, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_ARR(_var, _var_string, _buffer, _data_size, _arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, NULL, _arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);

#define SOC_DPP_WB_ENGINE_ADD_2D_ARR(_var, _var_string, _buffer, _data_size, _outer_arr_length, _inner_arr_length, _version_added)\
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PRIMARY, _var, _var_string, _buffer, _data_size, NULL, _outer_arr_length, _inner_arr_length, _version_added)\
    SOCDNX_IF_ERR_EXIT(rv);
/* -------------------------------- */





/********************************************************************************************/
/* set/get macros to be used to set/get vars/array entries that are handled by wb_engine    */
/* retun SOC_E_ERRORS                                                                       */
/********************************************************************************************/

#define SOC_DPP_WB_ENGINE_SET_DBL_ARR(unit, _var_id, _data_ptr, _outer_idx, _inner_idx)\
    SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _outer_idx, _inner_idx)

#define SOC_DPP_WB_ENGINE_GET_DBL_ARR(unit, _var_id, _data_ptr, _outer_idx, _inner_idx)\
    SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _outer_idx, _inner_idx)

#define SOC_DPP_WB_ENGINE_SET_ARR(unit, _var_id, _data_ptr, _idx)\
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _idx)

#define SOC_DPP_WB_ENGINE_GET_ARR(unit, _var_id, _data_ptr, _idx)\
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr, _idx)

#define SOC_DPP_WB_ENGINE_SET_VAR(unit, _var_id, _data_ptr)\
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr)

#define SOC_DPP_WB_ENGINE_GET_VAR(unit, _var_id, _data_ptr)\
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PRIMARY, _var_id, _data_ptr)


/* set/get macros for changing multiple array entries at once */
#define SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, _var_id, _data_ptr, _inner_idx, _cpy_length) \
    soc_wb_engine_array_range_set_or_get(unit, SOC_WB_ENGINE_PRIMARY, (_var_id), (_inner_idx), (_cpy_length), 1, (uint8 *)(_data_ptr))

#define SOC_DPP_WB_ENGINE_MEMCPY_ARR_GET(unit, _var_id, _data_ptr, _inner_idx, _cpy_length) \
    soc_wb_engine_array_range_set_or_get(unit, SOC_WB_ENGINE_PRIMARY, (_var_id), (_inner_idx), (_cpy_length), 0, (uint8 *)(_data_ptr))

#define SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, _var_id, _val) \
    soc_wb_engine_array_set(unit, SOC_WB_ENGINE_PRIMARY, (_var_id), (_val))

#ifdef BCM_ARAD_SUPPORT

/* Table init functions */
int soc_dpp_wb_engine_state_init_port(int unit);

int soc_dpp_wb_engine_Arad_pp_sw_db_get(ARAD_PP_SW_DB *sw_db);

/* allocation mngr structs */
/* Egress encapsulation start definiations */
typedef struct bcm_dpp_am_egress_encap_s {
  SHR_BITDCL             *egress_encap_banks;  /* Bitmap of existing Egress Encapsulation banks */
  uint8                   egress_encap_count;   /* Number of existing Egress Encapsulation banks */
  uint8                   init; /* Indicate egress encap was initialized */
} bcm_dpp_am_egress_encap_t;

/* Ingress LIF start definitions */
typedef struct bcm_dpp_am_ingress_lif_s {
  SHR_BITDCL             *ingress_lif_banks;  /* Bitmap of existing Ingress LIF banks */
  uint8                   ingress_lif_count;   /* Number of existing Ingress LIF banks */
  uint8                   init; /* Indicate ingress lif was initialized */
} bcm_dpp_am_ingress_lif_t;

typedef struct bcm_dpp_am_sync_lif_s {
  SHR_BITDCL              *sync_lif_banks;  /* Bitmap of sync LIF banks */
  uint8                   init; /* Indicate sync lif was initialized */
} bcm_dpp_am_sync_lif_t;
#endif /*BCM_ARAD_SUPPORT*/

/* Other table structs */
/* Advanced VLAN editing action SW table structures */
typedef struct _bcm_dpp_vlan_translate_tag_action_s {
    bcm_vlan_action_t vid_action;
    bcm_vlan_action_t pcp_action;
    bcm_vlan_tpid_action_t tpid_action;
    uint16 tpid_val;
} _bcm_dpp_vlan_translate_tag_action_t;

typedef struct _bcm_dpp_vlan_translate_action_s {
    _bcm_dpp_vlan_translate_tag_action_t outer;
    _bcm_dpp_vlan_translate_tag_action_t inner;
} _bcm_dpp_vlan_translate_action_t;


int soc_dpp_wb_engine_enable_dynamic_var(int unit, int var_idx, soc_wb_engine_dynamic_var_info_t var, uint8 *data_orig);
int soc_dpp_wb_engine_disable_dynamic_var(int unit, int var_idx);
int soc_dpp_wb_engine_enable_dynamic_occ_bm(int unit, int var_idx, SOC_SAND_OCC_BM_PTR occ_bm);
int soc_dpp_wb_engine_disable_dynamic_occ_bm(int unit, int var_idx);
int soc_dpp_wb_engine_enable_dynamic_hash_tbl(int unit, int var_idx, SOC_SAND_HASH_TABLE_INFO hash_tbl);
int soc_dpp_wb_engine_disable_dynamic_hash_tbl(int unit, int var_idx);
int soc_dpp_wb_engine_enable_dynamic_sorted_list(int unit, int var_idx, SOC_SAND_SORTED_LIST_INFO sorted_list);
int soc_dpp_wb_engine_disable_dynamic_sorted_list(int unit, int var_idx);
int soc_dpp_wb_engine_init_buffer(int unit, int buffer_id);
int soc_dpp_wb_engine_deinit(int unit);
int soc_dpp_wb_engine_init(int unit);
int soc_dpp_wb_engine_sync(int unit);
int soc_dpp_wb_engine_update_var_info(int unit, int var_idx, uint32 data_size, uint32 outer_arr_length, uint32 inner_arr_length);
int soc_dpp_wb_engine_sand_bitstream_set_any_field(int unit,int var_ndx,uint32 outer_array_ndx,uint32 start_bit,uint32 nof_bits,uint32 *input_buffer);
int soc_dpp_wb_engine_sand_bitstream_get_any_field(int unit,int var_ndx,uint32 outer_array_ndx,uint32 start_bit,uint32 nof_bits,uint32 *output_buffer);
int soc_dpp_wb_engine_sand_bitstream_set(int unit,int var_ndx, uint32 place, uint32 bit_indicator);
int soc_dpp_wb_engine_sand_bitstream_test_bit(int unit, int var_ndx, uint32 place, uint8 *result);


/************************************************************************************************/
/*                                                                                              */
/*                              ADD BUFFERS and VARIABLES here                                  */
/*                      above this box - infrastructure implementation                          */
/*                        below this box - add your buffers\vars info                           */
/*                                                                                              */
/************************************************************************************************/

#define DPP_WB_ENGINE_NOF_LIFS (SOC_DPP_CONFIG(unit))->l2.nof_lifs

/* need to declare the buffers and variables in the two enums below, init them at dpp_wb_db.c */

/* original variables declaration -
   the variables\structs that hold the real data,
   pointed to by the wb_engine */
#ifdef BCM_ARAD_SUPPORT
extern bcm_dpp_am_egress_encap_t _dpp_am_egress_encap[SOC_MAX_NUM_DEVICES];
extern bcm_dpp_am_ingress_lif_t _dpp_am_ingress_lif[SOC_MAX_NUM_DEVICES];
extern bcm_dpp_am_sync_lif_t _dpp_am_sync_lif[SOC_MAX_NUM_DEVICES];
extern ARAD_SW_DB Arad_sw_db;
  
#ifdef BCM_WARM_BOOT_SUPPORT   
extern dpp_wb_hash_tbl_data_t wb_hash_tbl_data[SOC_SAND_MAX_DEVICE][SOC_DPP_WB_HASH_TBLS_NUM];                                                            
#endif /*BCM_WARM_BOOT_SUPPORT*/                                                           
#endif /*BCM_ARAD_SUPPORT*/                                                             
extern SOC_PPD_OAM_INIT_TRAP_INFO _bcm_dpp_oam_trap_info[SOC_MAX_NUM_DEVICES];
extern SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP _bcm_oam_cpu_trap_code_to_mirror_profile_map[SOC_MAX_NUM_DEVICES];
  
extern SOC_PPD_BFD_INIT_TRAP_INFO _bcm_dpp_bfd_trap_info[SOC_MAX_NUM_DEVICES];                                                           

#ifdef BCM_PETRA_SUPPORT
extern int _bcm_dpp_egr_thresh_map[BCM_MAX_NUM_UNITS][SOC_TMC_NOF_FAP_PORTS_MAX];
extern int _bcm_dpp_egr_interface_unicast_thresh_map[BCM_MAX_NUM_UNITS][SOC_TMC_IF_NOF_NIFS];
extern int _bcm_dpp_egr_interface_multicast_thresh_map[BCM_MAX_NUM_UNITS][SOC_TMC_IF_NOF_NIFS];
#endif /*BCM_PETRA_SUPPORT*/

/* 
 * buffers - 
 * unique buffer names, strictly numbered enum. numbers should be only added, never changed. 
 * these numbers are used as unique id for the buffer to be saved on external storage.
 */
typedef enum
{
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_MGMT_OPERATION_MODE             =  1,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_MIRROR                =  3,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_VID_ASSIGN            =  4,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_MIRROR                 =  6,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE   =  7, 
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO                 =  8,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_TRAP                  = 17,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_ENCAP                  = 18,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_COS                   = 19,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF_AC                 = 20,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF                    = 21,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB                         = 23,
    SOC_DPP_WB_ENGINE_BUFFER_ALLOC_MNGR                              = 24,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_RESTORATION_DATA        = 27,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_OCC_BMS_DYNAMIC         = 30,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_SORTED_LIST_DYNAMIC     = 32,
    SOC_DPP_WB_ENGINE_BUFFER_SWITCH                                  = 34,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS            = 35,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SRC_BIND                     = 36,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM                             = 37,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP            = 42,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_HASH_TBL                = 47,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_TCAM_DB_BANK_OCC_BMS_DYNAMIC = 48,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_TM_GUARANTEED_Q_RESOURCE           = 51,
    SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_REASSEMBLY_CONTEXT           = 65,
    SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM                                = 69,
	SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB                   = 74,
    SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_OUTLIF_SW_DB                  = 75,
    SOC_DPP_WB_ENGINE_BUFFER_JER_EEDB_BANKS_SW_DB                    = 76,
  /*NOTE THAT 252 IS THE HIGHEST BUFFER INDEX CURRENTLY ALLOWED!     = 255,*/

    SOC_DPP_WB_ENGINE_NOF_BUFFERS                                    

} SOC_DPP_WB_ENGINE_BUFFER;


/* 
 * variables, 
 * when adding a new var, always add it as the last var of his buffer variables!
 */
typedef enum
{
    /* ARAD_PP_SW_DB buffer */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_AUTHENTICATION_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_MPLS_TERMINATION_LABEL_INDEX_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_FAST_REROUTE_LABELS_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_MPLS_COUPLING_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MPLS_INFO_LOOKUP_INCLUDE_INRIF,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_SIMPLE_VLAN_TRANSLATION_ENABLE_SET,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_MIM_INITIALIZED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_OAM_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_BFD_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_MGMT_OPERATION_MODE_TRILL_ENABLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_VID_ASSIGN_VID_SA_BASED_ENABLE,

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO buffer */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_DEFAULT_FEC,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_NOF_LPM_ENTRIES_IN_LPM,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_NOF_VRFS,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_VRF_MODIFIED_BITMASK,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_CACHE_MODIFIED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_CACHE_MODE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_IPV4_INFO_LEM_ADD_FAIL,

    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_1),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_2),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_3),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_4),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_5),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_6),

    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_1),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_2),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_3),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_4),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_5),
    COMPLEX_DS_ARR_MEM_ALLOCATOR(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MEM_LL),

    COMPLEX_DS_PAT_TREE_AGREGATION(SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_LPMS),

    /* SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_MIRROR_LLP buffer */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_MIRROR_LLP_MIRROR_PORT_VLAN,
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_MIRROR_MIRROR_PROFILE_MULTI_SET),

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_TRAP buffer */
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_TRAP_L3_PROTOCOLS_MULTI_SET),


    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_MIRROR buffer */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_EG_MIRROR_EG_MIRROR_PORT_VLAN,
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_EG_MIRROR_MIRROR_PROFILE_MULTI_SET),

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_EG_ENCAP buffer */
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_EG_ENCAP_PROG_DATA),

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_LLP_COS buffer */
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_LLP_COS_ETHER_TYPE_MULTI_SET),

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE buffer */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE_CONFIG_METER_STATUS,
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE_ETH_METER_PROFILE_MULTI_SET),
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE_GLOBAL_METER_PROFILE_MULTI_SET),

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF_AC buffer */
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_L2_LIF_AC_AC_KEY_MAP_MULTI_SET),

    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_L2_LIF buffer */
    COMPLEX_DS_MULTI_SET(SOC_DPP_WB_ENGINE_VAR_ARAD_PP_SW_DB_L2_LIF_VLAN_COMPRESSION_RANGE_MULTI_SET),
    
    /* SOC_DPP_WB_ENGINE_BUFFER_ARAD_PORT_SW_DB bufer */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INITIALIZED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_PHY_PORTS,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_INTERFACE_TYPE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_SPEED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_MASTER_PORT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_CHANNELS_COUNT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LATCH_DOWN,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_VALID,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FIRST_PHY_PORT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_CHANNEL,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PROTOCOL_OFFSET,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FLAGS,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_CORE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_CHANNELIZED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_ENCAP_MODE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_CORE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_IS_SINGLE_CAL_MODE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_HIGH_PRIORITY_CAL,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_LOW_PRIORITY_CAL,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_PHY_PORT_RUNT_PAD,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_TM_PORT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_PORT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PRIORITY_MODE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_MC_OFFSET,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_EGR_INTERFACE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_OLIF_EXT_EN,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_SRC_EXT_EN,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_DST_EXT_EN,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FRST_HDR_SZ,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_HDR_T_OUT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_HDR_T_IN,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PP_FLAGS,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_FC_TYPE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_MIRROR_PROFILE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_SRC_SYST_PORT_EXT_PRESENT,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_PPH_PRESENT_ENABLED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_STAG_ENABLED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_SNOOP_ENABLED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_IS_TM_ING_SHAPING_ENABLED,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_SHAPER_MODE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_PEER_TM_DOMAIN,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PORT_SW_DB_LOGICAL_PORTS_BASE_Q_PAIR,

    /*alloc mngr vars*/
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_BANKS,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_COUNT,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_INIT,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_BANKS,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_COUNT,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_INIT,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_SYNC_LIF_BANKS,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_SYNC_LIF_INIT,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_COSQ_EGR_THRESH_MAPPING,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_COSQ_EGR_INTERFACE_UNICAST_THRESH_MAPPING,
    SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_COSQ_EGR_INTERFACE_MULTICAST_THRESH_MAPPING,

    /*arad_sw_db tcam*/
    SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_OCC_BM_RESTORE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_OCC_BM_RESTORE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_SORTED_LIST_RESTORE,

    /*arad oam */
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_ACC_ENDPOINT_REF_COUNTER,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_MIRROR_IDS,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_INIT_TRAP_INFO_TRAP_IDS,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_UDP_SPORT_REF_COUNTER,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_UDP_SPORT_REF_COUNTER,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_RECYCLE,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_SNOOP,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_DROP,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_TRAP_TO_CPU,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_CC_MPLS_TP,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_IPV4,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_MPLS,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_PWE,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_SNOOP_TO_CPU,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_RECYCLE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_LEVEL,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE,
	SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_ERROR_TYPE_TO_TRAP_ID_MAP,
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP,
    /*hash tables*/
    COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_MA_NAME),
    COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_MA_TO_MEP),
    COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_BFD_MEP_INFO),
    /*SOC_DPP_WB_ENGINE_VAR_OAM_BFD_MEP_TO_RMEP,*/
    COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_BFD_RMEP_INFO),
    COMPLEX_DS_BCM_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_OAM_MEP_TO_AIS_ID),

    ADD_MULTIPLE_OCC_BMS(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED, SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS),
    ADD_MULTIPLE_OCC_BMS(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_BANK_ENTRIES_USED_INVERSE, SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS),
    ADD_MULTIPLE_OCC_BMS(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TCAM_DB_BANK_ENTRIES_USED, (SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS * ARAD_TCAM_MAX_NOF_LISTS)),
    ADD_MULTIPLE_HASH_TBLS(SOC_DPP_WB_ENGINE_VAR_TCAM_DB_ENTRY_ID_TO_LOCATION, 1),
    ADD_MULTIPLE_SORTED_LISTS(SOC_DPP_WB_ENGINE_VAR_TCAM_DB_PRIORITY, ARAD_TCAM_MAX_NOF_LISTS),

    /*dpp/switch.c*/
    SOC_DPP_WB_ENGINE_VAR_SWITCH_WARMBOOT_AUTO_SYNC,

    /* SLB aging time. */
    SOC_DPP_WB_ENGINE_VAR_SWITCH_SLB_AGING_TIME,

    /*SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_SAND_DATA_STRUCTS*/
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_CHANIF2CHAN_ARB_OCC),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_EGR_PORTS_NONCHANIF2SCH_OFFSET_OCC),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_SRC_BINDS_SRCBIND_ARB_OCC),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_REASSEMBLY_CONTEXT_OCC),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_CHANNELIZED_CALS_OCC_CORE0),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_CHANNELIZED_CALS_OCC_CORE1),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_CHANNELIZED_CALS_OCC_CORE0),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_CHANNELIZED_CALS_OCC_CORE1),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_E2E_INTERFACES_OCC_CORE0),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_E2E_INTERFACES_OCC_CORE1),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_E2E_INTERFACES_OCC_CORE0),
    COMPLEX_DS_OCC_BM(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_ARAD_SW_DB_MODIFIED_E2E_INTERFACES_OCC_CORE1),
    COMPLEX_DS_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_VTT_ISEM_KEY_TO_ENTRY_ID),
    COMPLEX_DS_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_FRWRD_IP_ROUTE_KEY_TO_ENTRY_ID),
    COMPLEX_DS_HASH_TABLE(SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_SRC_BIND_IPV6_STATIC_SRC_BIND_IPV6_STATIC_KEY_TO_ENTRY_ID),

    /* RIF urpf mode */
    SOC_DPP_WB_ENGINE_VAR_ARAD_PP_RIF_URPF_MODE,
    SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE,


#ifdef BCM_ARAD_SUPPORT
    /* SOC_DPP_WB_ENGINE_BUFFER_CHIP_SIM */
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_READ_RESULT_ADDRESS,
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_OFFSET,
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_TABLE_SIZE,
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_KEY_SIZE,
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_DATA_NOF_BYTES,
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_START_ADDRESS,
    SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_END_ADDRESS,
    ADD_MULTIPLE_MULTI_SETS(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_MULTI_SET, 8),
    ADD_MULTIPLE_VARS(SOC_DPP_WB_ENGINE_VAR_CHIP_SIM_BASE, 8),
#endif /*BCM_ARAD_SUPPORT*/

    /*SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB*/
    SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_INIT,
    SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_DB,
    SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO,
    SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_APP_TYPE_INFO,

    /*SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_OUTLIF_SW_DB*/
    SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_INIT,
    SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_DB,

    /*SOC_DPP_WB_ENGINE_BUFFER_JER_EEDB_BANKS_SW_DB*/
    SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO,
    SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO,
    /*always last*/
    SOC_DPP_WB_ENGINE_VAR_NOF_VARS

} SOC_DPP_WB_ENGINE_VAR;

#endif /*_SOC_DPP_WB_ENGINE_H_*/
