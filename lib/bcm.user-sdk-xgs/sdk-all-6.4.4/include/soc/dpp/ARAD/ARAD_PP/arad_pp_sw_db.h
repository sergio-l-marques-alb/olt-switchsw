/* $Id: arad_pp_sw_db.h,v 1.48 Broadcom SDK $
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
 * $
*/

#ifndef __ARAD_PP_SW_DB_INCLUDED__
/* { */
#define __ARAD_PP_SW_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/swstate/sw_state.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mgmt.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lif_table.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_vid_assign.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_fec.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fec.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>

#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

#include <soc/dpp/dpp_config_defs.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_SW_DB_TYPE_BITS (4)

#define ARAD_PP_SW_IPV4_VRF_BITMAP_SIZE (128)

typedef enum
{
  /*
   * for assigning VID according to l3 protocol
   */
  ARAD_PP_SW_DB_MULTI_SET_L3_PROTOCOL = ARAD_NOF_SW_DB_MULTI_SETS, /* Synchronisation with SW DB TM profiles */
  /*
  *  for assigning VID according to l2 protocol
  */
  ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,

  /* for assigning VID to index of outband mirror*/
  ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,
  /*
   * for managing ether-type for protocol-based vlan classification and
   * protocol-based TC assignment
   */
   ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
  /* 
   * for assigning eth_policer to profiles
   */
   ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,

   ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE,

   ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,

 /*
  *  for AC-key map
  */
  ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE,

 /*
  *  DATA of egress programmable editor
  */
  ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY, 

 /* 
  * overlay ARP data in egress programmable editor
  */ 
  ARAD_PP_SW_DB_MULTI_SET_ENTRY_OVERLAY_ARP_PROG_DATA_ENTRY

} ARAD_PP_SW_DB_MULTI_SET;

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define ARAD_PP_SW_DB_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_GLOBAL_FIELD_SET(field_name, val)            \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_pp_sw_db.field_name),                                 \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
                                                                  \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_GLOBAL_FIELD_GET(field_name, val)            \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_pp_sw_db.field_name),                                 \
      sizeof(*val)                                                \
    );                                                            \
                                                                  \
  return soc_sand_ret;


#define ARAD_PP_SW_DB_IPV4_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL || Arad_pp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_IPV4_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL || Arad_pp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;
#define ARAD_PP_ALLOC_VAR(var, type, count)                                           \
  do                                                                                \
  {                                                                                 \
    if ((var = (type*)soc_sand_os_malloc(count * sizeof(type))) == NULL)                \
    {                                                                               \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 2, exit);                               \
    }                                                                               \
  } while(0)


/*
 * is l3 supported
 */
#define ARAD_PP_L3_ROUTING_ENABLED(_unit)  \
      arad_pp_sw_db_ipv4_is_routing_enabled(_unit)



/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_SW_DB_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_SW_DB_FIRST,
  ARAD_PP_SW_DB_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  ARAD_PP_SW_DB_INIT,
  ARAD_PP_SW_DB_DEVICE_INIT,
  ARAD_PP_SW_DB_DEVICE_CLOSE,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_SW_DB_PROCEDURE_DESC_LAST
} ARAD_PP_SW_DB_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_SW_DB_ERR_LAST
} ARAD_PP_SW_DB_ERR;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO l3_protocols_multi_set;
} ARAD_PP_SW_DB_LLP_TRAP;

typedef struct
{
  uint8 llp_mirror_port_vlan[SOC_DPP_DEFS_MAX(NOF_CORES)][ARAD_PORT_NOF_PP_PORTS]; /* bitmap storing whether port x vlan pairs were added to be mirrored */
  SOC_SAND_MULTI_SET_INFO mirror_profile_multi_set;
} ARAD_PP_SW_DB_LLP_MIRROR;

typedef struct
{
  uint8 eg_mirror_port_vlan[SOC_DPP_DEFS_MAX(NOF_CORES)][ARAD_PORT_NOF_PP_PORTS]; /* bitmap storing whether port x vlan pairs were added to be mirrored */
  SOC_SAND_MULTI_SET_INFO mirror_profile_multi_set;
} ARAD_PP_SW_DB_EG_MIRROR;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO prog_data;
} ARAD_PP_SW_DB_EG_ENCAP;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO ether_type_multi_set;
} ARAD_PP_SW_DB_LLP_COS;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO       eth_meter_profile_multi_set; 
  SOC_SAND_MULTI_SET_INFO       global_meter_profile_multi_set;
  uint32                        config_meter_status[ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BYTE];
} ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE;

typedef struct
{
  uint32 lif_use[ARAD_BIT_TO_U32(SOC_DPP_DEFS_MAX(NOF_LOCAL_LIFS) * ARAD_PP_SW_DB_TYPE_BITS)];

  /* for ac will till if it AC2AC, 2PWE, ...*/
  uint32 lif_sub_use[ARAD_BIT_TO_U32(SOC_DPP_DEFS_MAX(NOF_LOCAL_LIFS) * ARAD_PP_SW_DB_TYPE_BITS)];

} ARAD_PP_SW_DB_LIF_TABLE;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO       vlan_compression_range_multi_set;
} ARAD_PP_SW_DB_L2_LIF;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO       ac_key_map_multi_set;
} ARAD_PP_SW_DB_L2_LIF_AC;

typedef struct
{
  uint16 vrrp_mac_use_bit_map[ARAD_PP_VRRP_NOF_MAC_ADDRESSES/16];
} ARAD_PP_SW_DB_VRRP_INFO;

/*
 * Map a rif to a group of lifs. 
 * It is assumed that rif and lif are uint32 and no extra map is needed to map lif or rif to uint32 and back. 
 */
typedef struct 
{
  SOC_SAND_GROUP_MEM_LL_INFO group_info;

  /* URPF mode for RIFs */
  uint8 *rif_urpf_mode;
  
} ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP;

typedef struct
{
  /*
   *	Indicates if External-Key is enabled
   */
  uint8 ext_key_enabled;

} ARAD_PP_SW_DB_ISEM;

typedef struct {
  PARSER_HINT_ARR uint32 *cur_size;
  PARSER_HINT_ARR uint32 *max_size;
} ARAD_PP_SW_DB_ECMP;

/* Hw information need for LPM ALG */
typedef struct
{
  uint8 bank_id;
  uint32 address;
} ARAD_PP_IPV4_LPM_FREE_LIST_ITEM_INFO;


typedef struct
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;

  uint32
    default_fec;
  uint32
    nof_lpm_entries_in_lpm;
  uint32
    nof_vrfs;
  uint32
    *vrf_modified_bitmask;
  uint32
    *cm_buf_array;
  uint32
    cm_buf_entry_words;
  uint8
    cache_modified;
  uint32
    cache_mode;
  ARAD_PP_IPV4_LPM_FREE_LIST_ITEM_INFO
    *free_list;
  uint32
    free_list_size;
#ifdef SOC_SAND_DEBUG
  /* in debug mode, if TRUE lem_route_add will fail. used for denug purposes */
  uint8
    lem_add_fail;
#endif

}  ARAD_PP_SW_DB_IPV4_INFO;


typedef struct
{
  SOC_SAND_MULTI_SET_INFO prog_data;
} ARAD_PP_SW_OVERLAY_ARP;


typedef struct
{
  ARAD_PP_MGMT_OPERATION_MODE *oper_mode;

  ARAD_PP_SW_DB_LLP_TRAP *llp_trap;
  ARAD_PP_SW_DB_LLP_MIRROR *llp_mirror;
  ARAD_PP_SW_DB_EG_MIRROR *eg_mirror;
  ARAD_PP_SW_DB_EG_ENCAP *eg_encap;
  ARAD_PP_SW_DB_LLP_COS *llp_cos;

  ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE *eth_policer_mtr_profile;

  ARAD_PP_SW_DB_IPV4_INFO *ipv4_info;

  ARAD_PP_SW_DB_ISEM *isem;

  ARAD_PP_SW_DB_L2_LIF *l2_lif;

  ARAD_PP_SW_DB_L2_LIF_AC *l2_lif_ac;

/* ARAD+ data*/
  ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP *rif_to_lif_group_map;

  ARAD_PP_SW_OVERLAY_ARP *overlay_arp;

} ARAD_PP_SW_DB_DEVICE;

/* NEW SW STATE STRUCT */
typedef struct soc_arad_sw_state_pp_s{
    PARSER_HINT_PTR    ARAD_PP_SW_DB_ECMP                *ecmp_info;
    PARSER_HINT_PTR    ARAD_LLP_VID_ASSIGN_t             *llp_vid_assign;
    PARSER_HINT_PTR    ARAD_PP_LLP_FILTER_t              *llp_filter;
    PARSER_HINT_PTR    ARAD_PON_DOUBLE_LOOKUP            *pon_double_lookup;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_VRRP_INFO           *vrrp_info;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LIF_TABLE           *lif_table;
    PARSER_HINT_PTR    ARAD_PP_HEADER_DATA               *header_data;
    PARSER_HINT_PTR    ARAD_PP_FWD_MACT                  *fwd_mact;
    PARSER_HINT_PTR    ARAD_PP_LAG                       *lag;
    PARSER_HINT_PTR    ARAD_PP_ILM_INFO                  *ilm_info;
    PARSER_HINT_PTR    ARAD_PP_OAM_MY_MAC_LSB            *oam_my_mac_lsb;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_DIAG                *diag;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LIF_COS             *lif_cos;
    PARSER_HINT_PTR    ARAD_PP_MAC_LIMIT_PER_TUNNEL_INFO *mac_limit_info;
    PARSER_HINT_PTR    ARAD_PP_OCCUPATION                *occupation;
    PARSER_HINT_PTR    ARAD_PP_FEC                       *fec;
    PARSER_HINT_PTR    ARAD_PP_SLB_CONFIG                *slb_config;
} soc_arad_sw_state_pp_t;

typedef struct
{
  uint8
    init;

  ARAD_PP_SW_DB_DEVICE*
    device[SOC_SAND_MAX_DEVICE];

} ARAD_PP_SW_DB;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * SW DB manangement {
 */

uint32
  arad_pp_sw_db_init(void);

uint32
  arad_pp_sw_db_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  arad_pp_sw_db_device_close(
    SOC_SAND_IN int unit
  );

/*
 * SW DB manangement }
 */

/*
 * SW DB multiset
 */
SOC_SAND_MULTI_SET_INFO*
  arad_pp_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_PP_SW_DB_MULTI_SET     multiset
  );

/*
 *	Operation mode
 */
uint32
  arad_pp_sw_db_oper_mode_initialize(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  arad_pp_sw_db_oper_mode_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  ARAD_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  arad_pp_sw_db_oper_mode_terminate(
    SOC_SAND_IN  int unit
  );

/*
 * llp filter {
 */


uint32
  arad_pp_sw_db_llp_filter_initialize(
    SOC_SAND_IN  int unit
    );

uint32
  arad_pp_sw_db_llp_filter_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp filter }
*/

/*
* ipv4 {
*/

uint32
  arad_pp_sw_db_ipv4_initialize(
   SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  *max_nof_routes,
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  *mem_to_bank,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, /* must be uint32 */
    SOC_SAND_IN ARAD_PP_IPV4_LPM_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  );

uint32
  arad_pp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
  );

uint32
  arad_pp_sw_db_ipv4_cache_mode_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32                                     *cache_mode_bitmask
  );

uint32
  arad_pp_sw_db_ipv4_cache_mode_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     cache_mode_bitmask
  );


uint32
  arad_pp_sw_db_ipv4_cache_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint8	                          	     *is_modified
  );

uint32
  arad_pp_sw_db_ipv4_cache_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint8	                          	     modified
);

uint32
  arad_pp_sw_db_ipv4_cache_vrf_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                     vrf_ndx,
    SOC_SAND_OUT uint32                                     *is_modified
  );

uint32
  arad_pp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                    vrf_ndx,
    SOC_SAND_IN uint32                                     is_modified
  );

uint32
  arad_pp_sw_db_ipv4_cache_modified_bitmap_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32	                          	     *modified_bitmap
  );

uint32
  arad_pp_sw_db_ipv4_cache_modified_bitmap_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32	                          	     *modified_bitmap
  );

uint32
  arad_pp_sw_db_ipv4_default_fec_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    default_fec
  );

uint32
  arad_pp_sw_db_ipv4_default_fec_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *default_fec
  );

uint32
  arad_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_lpm_entries_in_lpm
  );

uint32
  arad_pp_sw_db_ipv4_lpm_dma_buff_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    **dma_buff
  );


uint32
  arad_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_lpm_entries_in_lpm
  );

uint32
  arad_pp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  );

uint32
  arad_pp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  );


uint32
  arad_pp_sw_db_ipv4_nof_vrfs_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_vrfs
  );

uint32
  arad_pp_sw_db_ipv4_nof_vrfs_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_vrfs
  );

uint8
  arad_pp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   vrf_ndx
  );

uint8
  arad_pp_sw_db_ipv4_is_routing_enabled(
    SOC_SAND_IN int                    unit
  );


uint32
  arad_pp_sw_db_ipv4_is_shared_lpm_memory(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   vrf_ndx,
    SOC_SAND_OUT uint8                  *shared
  );

uint32
  arad_pp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN ARAD_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr
  );

uint32
  arad_pp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  );

#ifdef SOC_SAND_DEBUG
uint32
  arad_pp_sw_db_ipv4_lem_add_fail_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    *ipv4_lem_add_fail
  );

uint32
  arad_pp_sw_db_ipv4_lem_add_fail_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *ipv4_lem_add_fail
  );
#endif


/*
 * ipv4 }
 */

/*
* llp trap {
*/

uint32
  arad_pp_sw_db_llp_trap_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_llp_trap_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp trap }
*/

/*
* llp mirror {
*/
uint32
  arad_pp_sw_db_llp_mirror_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_llp_mirror_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp mirror }
*/

/*
* llp vid_assign {
*/

uint32
  arad_pp_sw_db_llp_vid_assign_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_llp_vid_assign_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp vid_assign }
*/

#define ARAD_PP_PON_DOUBLE_LOOKUP_PORT_SET(_local_port_ndx, _enable)\
  \
  if(_enable) {\
    SHR_BITSET(sw_state[unit]->dpp.soc.arad.pp[0].pon_double_lookup[0].pon_double_lookup_enable, _local_port_ndx);\
  } else {\
    SHR_BITCLR(sw_state[unit]->dpp.soc.arad.pp[0].pon_double_lookup[0].pon_double_lookup_enable, _local_port_ndx);\
  }


#define ARAD_PP_PON_DOUBLE_LOOKUP_PORT_GET(_local_port_ndx)\
  \
  SHR_BITGET(sw_state[unit]->dpp.soc.arad.pp[0].pon_double_lookup[0].pon_double_lookup_enable, _local_port_ndx);

/* EG MIRROR { */

uint32
  arad_pp_sw_db_eg_mirror_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_eg_mirror_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_eg_encap_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_eg_encap_terminate(
    SOC_SAND_IN  int unit
  );


/* EG MIRROR } */

/*
* llp cos {
*/

uint32
  arad_pp_sw_db_llp_cos_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_llp_cos_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp cos }
*/

/*
* eth_policer {
*/

uint32
  arad_pp_sw_db_eth_policer_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_eth_policer_terminate(
    SOC_SAND_IN  int unit
  );

void
  arad_pp_sw_db_eth_policer_config_status_bit_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint32                                         bit_offset,
    SOC_SAND_IN  uint8                                        status
  );

uint32
  arad_pp_sw_db_eth_policer_config_status_bit_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint32                                         bit_offset,
    SOC_SAND_OUT  uint8                                       *status
  );

/*
* eth_policer }
*/

/*
 * Forward MAC Table
 */
uint32
  arad_pp_sw_db_mact_flush_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_index,
    SOC_SAND_IN uint32 flush_db_data[7]
  );

uint32
  arad_pp_sw_db_mact_flush_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_index,
    SOC_SAND_OUT uint32 flush_db_data[7]
  );

uint32
  arad_pp_sw_db_mact_traverse_flush_entry_use_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_offset,
    SOC_SAND_IN uint8  in_use
  );

uint32
  arad_pp_sw_db_mact_traverse_flush_entry_use_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_OUT uint8   *status
  );

/*
* llp cos }
*/

/*
 * lif table {
 */
uint32
  arad_pp_sw_db_lif_table_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID         lif_id,
    SOC_SAND_IN  ARAD_PP_LIF_ENTRY_TYPE   entry_type,
    SOC_SAND_IN    uint32                 sub_type
  );


uint32
  arad_pp_sw_db_lif_table_entry_use_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID         lif_id,
    SOC_SAND_OUT  ARAD_PP_LIF_ENTRY_TYPE   *entry_type,
    SOC_SAND_OUT  uint32                   *sub_type
  );

/*
* llp table }
*/


/*
 * FEC
 */
uint32
  arad_pp_sw_db_flp_prog_app_to_index_get(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8    app_id,
    SOC_SAND_OUT uint32 *prog_index
  );

/*
 * VVRP
 */
uint32
  arad_pp_sw_db_vrrp_mac_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            reg_ndx,
    SOC_SAND_IN  uint32            bit_ndx,
    SOC_SAND_IN  uint8             entry_in_use
  );

uint32
  arad_pp_sw_db_vrrp_mac_entry_use_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            reg_ndx,
    SOC_SAND_IN  uint32            bit_ndx,
    SOC_SAND_OUT uint8             *entry_in_use
  );

/*
 * MAC limit per tunnel
 */
uint32
  arad_pp_sw_db_mac_limit_per_tunnel_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint16            port,
    SOC_SAND_IN  uint16            tunnel,
    SOC_SAND_IN  int               mac_limit
  );

uint32
  arad_pp_sw_db_mac_limit_per_tunnel_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint16            port,
    SOC_SAND_IN  uint16            tunnel,
    SOC_SAND_OUT int              *mac_limit
  );

/* ARAD+ */

/*
 * RIF to LIF group map
 */
/**
 * Add a lif to a rif group.
 */
uint32 
  arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN uint32            rif,
     SOC_SAND_IN bcm_port_t        lif
     );

/**
 * Remove a lif to a rif group.
 */
uint32 
  arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN bcm_port_t        lif
     );

typedef SOC_SAND_GROUP_MEM_LL_ITER_FUN_POINTER_PARAM ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP_LIF_VISITOR;

/**
 * Traverse all lifs of a RIF. 
 * The callback visitor is called for each lif. 
 * The callback parameter "member" will be the lif id (bcm_port_t). 
 * param1 and param2 will be the parameters passed to 
 * this function. 
 * 
 * @param unit 
 * @param rif The lifs of this rif will be traversed.
 * @param visitor The callback to call for each lif.
 * @param opaque Will be passed untouched to the callback.
 * 
 * @return uint32 Status
 */
uint32
  arad_pp_sw_db_rif_to_lif_group_map_visit_lif_group(
     SOC_SAND_IN int unit,
     SOC_SAND_IN uint32 rif,
     SOC_SAND_IN ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP_LIF_VISITOR visitor,
     SOC_SAND_INOUT void *opaque
     );

/**
 * Get the urpf mode of a rif.
 *  
 * @param unit 
 * @param rif Get the urpf mode of this rif.
 * @param mode Filled with the urpf mode.
 * 
 * @return uint32 Status
 */
uint32
  arad_pp_sw_db_rif_to_lif_group_map_get_rif_urpf_mode(
     SOC_SAND_IN int unit,
     SOC_SAND_IN uint32 rif,
     SOC_SAND_OUT SOC_PPC_FRWRD_FEC_RPF_MODE *mode
     );
/* ARAD+ end */

/*
 *	Operation mode
 */

uint32
  arad_pp_sw_db_oam_enable_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8 oam_enable
  );

uint32
  arad_pp_sw_db_oam_enable_get(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint8 *enable
  );

uint32
  arad_pp_sw_db_mim_initialized_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  mim_initialized
  );

uint32
  arad_pp_sw_db_mim_initialized_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8 *mim_initialized
  );

uint32
  arad_pp_sw_db_bfd_enable_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8 bfd_enable
  );

uint32
  arad_pp_sw_db_bfd_enable_get(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint8 *enable
  );

uint32
  arad_pp_sw_db_trill_enable_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8 trill_enable
  );

uint32
  arad_pp_sw_db_trill_enable_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_OUT uint8 *trill_enable
  );

uint32
  arad_pp_sw_db_authentication_enable_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8 *authentication_enable
  );

uint8
  arad_pp_sw_db_split_horizon_enable_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_pp_sw_db_authentication_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 authentication_enable
  );

uint32
  arad_pp_sw_db_mpls_termination_label_index_enable_get(
    SOC_SAND_IN int                  unit,
    SOC_SAND_OUT uint8       *mpls_termination_label_index_enable
  );

uint32
  arad_pp_sw_db_mpls_fast_reroute_labels_enable_get(
    SOC_SAND_IN int                  unit,
    SOC_SAND_OUT uint8       *mpls_fast_reroute_labels_enable
  );

uint32
  arad_pp_sw_db_mpls_coupling_enable_get(
    SOC_SAND_IN int                  unit,
    SOC_SAND_OUT uint8       *mpls_coupling_enable
  );

uint32
  arad_pp_sw_db_mpls_lookup_include_inrif_enable_get(
    SOC_SAND_IN int                  unit,
    SOC_SAND_OUT uint8       *include_inrif_enable
  );

uint32
  arad_pp_sw_db_isem_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_isem_terminate(
    SOC_SAND_IN  int unit
  );

uint8
  arad_pp_sw_db_isem_ext_key_enabled_get(
    SOC_SAND_IN int unit
  );

void
  arad_pp_sw_db_isem_ext_key_enabled_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 ext_key_enabled
  );


uint32
  arad_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_PP_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_IN  uint8         is_exist
  );

uint32
  arad_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_PP_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_OUT uint8         *is_exist
  );

uint32
  arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_PP_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_IN  uint8         is_exist
  );

uint32
  arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_PP_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_OUT uint8         *is_exist
  );

uint32
  arad_pp_sw_db_ecmp_cur_size_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_IN  uint32       cur_size
  );

uint32 
  arad_pp_sw_db_ecmp_cur_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_OUT uint32       *size
  );

uint32
  arad_pp_sw_db_ecmp_max_size_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_IN  uint32       max_size
  );

uint32
  arad_pp_sw_db_ecmp_max_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_OUT uint32       *max_size
  );

uint32
arad_pp_sw_db_test_defragmet(SOC_SAND_IN  int unit);
/*********************************************************************
* NAME:
 *   arad_pp_sw_db_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_sw_db module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_sw_db_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_sw_db_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_sw_db module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_sw_db_get_errs_ptr(void);

/* } */




/*********************************************************************
* NAME:
 *   arad_pp_sw_db_eg_encap_overlay_arp_initialize
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Initalize multiset data for overlay arp entries in prge data.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
arad_pp_sw_db_overlay_arp_initialize(SOC_SAND_IN  int unit);


uint32
arad_pp_sw_db_overlay_arp_terminate(SOC_SAND_IN  int unit);


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_SW_DB_INCLUDED__*/
#endif

