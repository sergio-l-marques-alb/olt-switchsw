/* $Id: pb_pp_sw_db.h,v 1.9 Broadcom SDK $
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

#ifndef __SOC_PB_PP_SW_DB_INCLUDED__
/* { */
#define __SOC_PB_PP_SW_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_llp_filter.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_mact_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_diag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_fec.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_key.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_fem.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_metering.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif_table.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_mirror.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>


#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_SW_DB_PP_PORTS_NOF_U32  (SOC_PB_PP_NOF_PORTS/32)

#define SOC_PB_PP_SW_DB_TYPE_BITS (3)

#define SOC_PB_PP_SW_IPV4_VRF_BITMAP_SIZE (8)


typedef enum
{
  /*
   *  for assigning VID according to l3 protocol
   */
  SOC_PB_PP_SW_DB_MULTI_SET_L3_PROTOCOL = SOC_PB_NOF_SW_DB_MULTI_SETS, /* Synchronisation with SW DB TM profiles */
  /*
  *  for assigning VID according to l2 protocol
  */
  SOC_PB_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,

  /* for assigning VID to index of outband mirror*/
  SOC_PB_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,

  /*
   *	for managing ether-type for protocol-based vlan classification and
   *  protocol-based TC assignment
   */
   SOC_PB_PP_SW_DB_MULTI_SET_ETHER_TYPE,

  /* for assigning eth_policer to profiles */
   SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
   SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE,
   SOC_PB_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE

} SOC_PB_PP_SW_DB_MULTI_SET;

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PB_PP_SW_DB_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Soc_pb_pp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Soc_pb_pp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define SOC_PB_PP_SW_DB_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Soc_pb_pp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Soc_pb_pp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;




#define SOC_PB_PP_SW_DB_IPV4_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Soc_pb_pp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define SOC_PB_PP_SW_DB_IPV4_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Soc_pb_pp_sw_db.device[unit] == NULL || Soc_pb_pp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Soc_pb_pp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;



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
  SOC_PB_PP_SW_DB_GET_PROCS_PTR = SOC_PB_PP_PROC_DESC_BASE_SW_DB_FIRST,
  SOC_PB_PP_SW_DB_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_SW_DB_INIT,
  SOC_PB_PP_SW_DB_DEVICE_INIT,
  SOC_PB_PP_SW_DB_DEVICE_CLOSE,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_SW_DB_PROCEDURE_DESC_LAST
} SOC_PB_PP_SW_DB_PROCEDURE_DESC;

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
  SOC_PB_PP_SW_DB_ERR_LAST
} SOC_PB_PP_SW_DB_ERR;

typedef enum {
  SOC_PB_PP_PXX_MODEL_UNKNOWN = 0,
  SOC_PB_PP_PXX_MODEL_P4X = 1,
  SOC_PB_PP_PXX_MODEL_P6X = 2,
  SOC_PB_PP_PXX_MODEL_P6N = 3,
  SOC_PB_PP_PXX_MODEL_P4X_COMPLETE = 4
} SOC_PB_PP_PXX_MODEL;

typedef struct
{
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT ref_count;
} SOC_PB_PP_SW_DB_LLP_FILTER;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO l3_protocols_multi_set;
} SOC_PB_PP_SW_DB_LLP_TRAP;

typedef struct
{
  uint8 llp_mirror_port_vlan[SOC_PB_PORT_NOF_PP_PORTS][SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES];

  SOC_SAND_MULTI_SET_INFO mirror_profile_multi_set;
} SOC_PB_PP_SW_DB_LLP_MIRROR;

typedef struct
{
  uint32 vid_sa_based_enable[SOC_PB_PP_SW_DB_PP_PORTS_NOF_U32];
} SOC_PB_PP_SW_DB_LLP_VID_ASSIGN;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO mirror_profile_multi_set;
} SOC_PB_PP_SW_DB_EG_MIRROR;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO ether_type_multi_set;
} SOC_PB_PP_SW_DB_LLP_COS;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO       eth_meter_profile_multi_set;
  SOC_SAND_MULTI_SET_INFO       global_meter_profile_multi_set;
  uint32                  config_meter_status[SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BYTE];
} SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE;

typedef struct
{
  SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE
    uc_host_table_resources;

  SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE
    mc_host_table_resources;

  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE learning_mode;
  uint8                      is_petra_a_compatible;
} SOC_PB_PP_SW_DB_FWD_MACT;

typedef struct
{
  uint8 map_from_tc_dp;
} SOC_PB_PP_SW_DB_LIF_COS;


typedef struct
{
  uint32 lif_use[(SOC_PB_PP_LIF_ID_MAX+1)*SOC_PB_PP_SW_DB_TYPE_BITS];
} SOC_PB_PP_SW_DB_LIF_TABLE;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO       vlan_compression_range_multi_set;
} SOC_PB_PP_SW_DB_L2_LIF;
typedef struct
{
  uint32 trap_dest[SOC_PB_PP_NOF_TRAP_CODES * 4];

  uint8 already_saved;

  SOC_PB_PP_DIAG_MODE_INFO mode_info;
} SOC_PB_PP_SW_DB_DIAG;

typedef struct
{
  /*
   *	Indicates if the number of entries already created
   */
  uint32 nof_db_entries;

} SOC_PB_PP_SW_DB_FP_ENTRY;

typedef struct
{
  /*
   *  Software handle of the entry
   */
  uint32 entry_ndx;
  /*
   *  Actual table index
   */
  uint32 direct_tbl_ndx;
} SOC_PB_PP_SW_DB_FP_DIRECT_TBL_ENTRY;

typedef struct
{
  /*
   *	Indicate the Database type (TCAM, Direct Table, Direct Extraction, or not exist)
   */
  SOC_PB_PP_FP_DATABASE_INFO db_info[SOC_PB_PP_FP_NOF_DBS];

  uint32 tcam_db_key_size[SOC_PB_PP_FP_NOF_DBS];

  uint8 db_has_key_format[SOC_PB_PP_FP_NOF_DBS];

  SOC_PB_PP_FP_KEY_DESC key_descs[SOC_PB_PP_FP_NOF_DBS];

  /*
   *	Composition of the Copy Engine Keys
   *  (assumption of a fixed format)
   */
  SOC_PB_PP_FP_QUAL_TYPE ce_instr[SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX+1][SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX];

  /*
   *	Composition of the FEMs - use of Action 2
   */
  SOC_PB_PP_FP_FEM_ENTRY fem_entry[SOC_PB_PMF_NOF_CYCLES][SOC_PB_PMF_LOW_LEVEL_NOF_FEMS];

  /*
   *  Database location in the CE keys encoded as an input
   */
  SOC_PB_PP_FP_KEY_LOCATION db_key_location[SOC_PB_PP_FP_NOF_DBS][SOC_PB_PP_FP_NOF_PFGS];

  /*
   *	Database entries
   */
  SOC_PB_PP_SW_DB_FP_ENTRY db_entries[SOC_PB_PP_FP_NOF_DBS];

  /*
   * Predefined key per Database
   */
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_acl_key[SOC_PB_PP_FP_NOF_DBS];

  /*
   *  Direct table SW handle to HW index mapping
   */
  SOC_PB_PP_SW_DB_FP_DIRECT_TBL_ENTRY direct_tbl_sw2hw[32];

  /*
   * User-Defined Fields
   */
  SOC_PB_PP_FP_KEY_QUAL_INFO  udf[SOC_PB_PP_FP_NOF_HDR_USER_DEFS];

  /*
   * Database-ID to know if Tag already configured
   * per PFG and Tag action types
   */
  uint32 is_tag_used[SOC_PB_PP_FP_NOF_PFGS][SOC_PB_PP_FP_NOF_TAG_ACTION_TYPES];

  /*
   * Direct table entries info
   * Assumption of a single Direct Table Database
   */
  SOC_PB_PP_FP_ENTRY_INFO dt_entry[SOC_PB_PP_FP_NOF_ENTRY_IDS];

  /*
   * Number of VLAN Tags in the inner Ethernet header per PFG
   */
  uint32 inner_eth_nof_tags[SOC_PB_PP_FP_NOF_PFGS];

  /*
   * Location of the Key changed per PFG
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION loc_key_changed[SOC_PB_PP_FP_NOF_PFGS];

  /*
   * Key change size in bits. Default: 12 bits
   */
  uint8 key_change_size;

  uint8 db_id_cycle[SOC_PB_PP_FP_NOF_DBS];

} SOC_PB_PP_SW_DB_FP;

typedef struct
{
  /*
   *	Indicates if External-Key is enabled
   */
  uint8 ext_key_enabled;

} SOC_PB_PP_SW_DB_ISEM;

typedef struct
{
  /*
  *	indicates whether LB key is symmetric
  */
  uint8 lb_key_is_symtrc;

  SOC_PB_PP_HASH_MASKS masks;

} SOC_PB_PP_SW_DB_LAG;


typedef struct
{
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO glbl_info;
  uint8 fec_entry_type[SOC_DPP_NOF_FECS_PETRAB];
} SOC_PB_PP_SW_DB_FEC;

typedef struct
{
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;

  uint32
    default_fec;
  uint32
    nof_lpm_entries_in_lpm;
  uint32
    nof_vrfs;
  uint32
    *vrf_modified_bitmask;
  uint8
    cache_modified;
  uint32
    cache_mode;
  SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE
    uc_host_table_resources;
  SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE
    mc_host_table_resources;
  uint16
    **free_list;
  uint32
    *free_list_size;
#ifdef SOC_SAND_DEBUG
  /* in debug mode, if TRUE lem_route_add will fail. used for denug purposes */
  uint8
    lem_add_fail;
#endif

}  SOC_PB_PP_SW_DB_IPV4_INFO;


typedef struct
{
  /*
  * whether to mask InRIF in ILM key
  */
  uint8 mask_inrif;

  /*
   * whether to mask port in ILM key
   */
  uint8 mask_port;

}  SOC_PB_PP_SW_DB_ILM_INFO;


typedef struct
{
  SOC_PB_PP_MGMT_OPERATION_MODE *oper_mode;

  SOC_PB_PP_SW_DB_LLP_FILTER *llp_filter;

  SOC_PB_PP_SW_DB_LLP_TRAP *llp_trap;

  SOC_PB_PP_SW_DB_LLP_MIRROR *llp_mirror;

  SOC_PB_PP_SW_DB_LLP_VID_ASSIGN *llp_vid_assign;

  SOC_PB_PP_SW_DB_EG_MIRROR *eg_mirror;

  SOC_PB_PP_SW_DB_LLP_COS *llp_cos;

  SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE *eth_policer_mtr_profile;
  
  SOC_PB_PP_SW_DB_IPV4_INFO *ipv4_info;

  SOC_PB_PP_SW_DB_ILM_INFO *ilm_info;

  SOC_PB_PP_SW_DB_FWD_MACT *fwd_mact;

  SOC_PB_PP_SW_DB_LIF_COS *lif_cos;

  SOC_PB_PP_SW_DB_LIF_TABLE *lif_table;

  SOC_PB_PP_SW_DB_FEC *fec;

  SOC_PB_PP_SW_DB_DIAG *diag;

  SOC_PB_PP_SW_DB_FP *fp;

  SOC_PB_PP_SW_DB_ISEM *isem;

  SOC_PB_PP_SW_DB_LAG *lag;

  SOC_PB_PP_SW_DB_L2_LIF *l2_lif;
} SOC_PB_PP_SW_DB_DEVICE_SSR_V01;

typedef struct
{
  SOC_PB_PP_MGMT_OPERATION_MODE *oper_mode;

  SOC_PB_PP_SW_DB_LLP_FILTER *llp_filter;

  SOC_PB_PP_SW_DB_LLP_TRAP *llp_trap;

  SOC_PB_PP_SW_DB_LLP_MIRROR *llp_mirror;

  SOC_PB_PP_SW_DB_LLP_VID_ASSIGN *llp_vid_assign;

  SOC_PB_PP_SW_DB_EG_MIRROR *eg_mirror;

  SOC_PB_PP_SW_DB_LLP_COS *llp_cos;

  SOC_PB_PPSW_DB_ETH_POLICER_MTR_PROFILE *eth_policer_mtr_profile;

  SOC_PB_PP_SW_DB_IPV4_INFO *ipv4_info;

  SOC_PB_PP_SW_DB_ILM_INFO *ilm_info;

  SOC_PB_PP_SW_DB_FWD_MACT *fwd_mact;

  SOC_PB_PP_SW_DB_LIF_COS *lif_cos;

  SOC_PB_PP_SW_DB_LIF_TABLE *lif_table;

  SOC_PB_PP_SW_DB_FEC *fec;

  SOC_PB_PP_SW_DB_DIAG *diag;

  SOC_PB_PP_SW_DB_FP *fp;

  SOC_PB_PP_SW_DB_ISEM *isem;

  SOC_PB_PP_SW_DB_LAG *lag;

  SOC_PB_PP_SW_DB_L2_LIF *l2_lif;
} SOC_PB_PP_SW_DB_DEVICE_SSR_V02;

typedef SOC_PB_PP_SW_DB_DEVICE_SSR_V01 SOC_PB_PP_SW_DB_DEVICE;

typedef struct
{
  uint8
    init;

  SOC_PB_PP_SW_DB_DEVICE*
    device[SOC_SAND_MAX_DEVICE];

} SOC_PB_PP_SW_DB;

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

/* $Id: pb_pp_sw_db.h,v 1.9 Broadcom SDK $
 * SW DB manangement {
 */

uint32
  soc_pb_pp_sw_db_init(void);

uint32
  soc_pb_pp_sw_db_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  soc_pb_pp_sw_db_device_close(
    SOC_SAND_IN int unit
  );

/*
 * SW DB manangement }
 */

/*
 * SW DB multiset
 */
SOC_SAND_MULTI_SET_INFO*
  soc_pb_pp_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PP_SW_DB_MULTI_SET     multiset
  );

/*
 *	Operation mode
 */
uint32
  soc_pb_pp_sw_db_oper_mode_initialize(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  soc_pb_pp_sw_db_oper_mode_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  SOC_PB_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  soc_pb_pp_sw_db_oper_mode_terminate(
    SOC_SAND_IN  int unit
  );

/*
 * llp filter {
 */

uint32
  soc_pb_pp_sw_db_llp_filter_initialize(
    SOC_SAND_IN  int unit
    );

uint32
  soc_pb_pp_sw_db_llp_filter_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_llp_filter_desig_vlan_table_ref_count_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT *ref_count
  );

uint32
  soc_pb_pp_sw_db_llp_filter_desig_vlan_table_ref_count_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT *ref_count
  );

/*
* llp filter }
*/

/*
* ipv4 {
*/

uint32
  soc_pb_pp_sw_db_ipv4_initialize(
   SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  max_nof_routes[SOC_DPP_NOF_VRFS_PETRAB],
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, /* must be uint32 */
    SOC_SAND_IN SOC_PB_PP_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  );

uint32
  soc_pb_pp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_ipv4_uc_table_resources_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE		       uc_host_table_resources
  );

uint32
  soc_pb_pp_sw_db_ipv4_uc_table_resources_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE		       *uc_host_table_resources
  );

uint32
  soc_pb_pp_sw_db_ipv4_mc_table_resources_set(
  SOC_SAND_IN int                                      unit,
  SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE		       mc_host_table_resources
 );

uint32
  soc_pb_pp_sw_db_ipv4_mc_table_resources_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE		     *mc_host_table_resources
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_mode_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32                                     *cache_mode_bitmask
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_mode_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     cache_mode_bitmask
  );


uint32
  soc_pb_pp_sw_db_ipv4_cache_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint8	                          	     *is_modified
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint8	                          	     modified
);

uint32
  soc_pb_pp_sw_db_ipv4_cache_vrf_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                     vrf_ndx,
    SOC_SAND_OUT uint32                                     *is_modified
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                    vrf_ndx,
    SOC_SAND_IN uint32                                     is_modified
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_modified_bitmap_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32	                          	     *modified_bitmap
  );

uint32
  soc_pb_pp_sw_db_ipv4_cache_modified_bitmap_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32	                          	     *modified_bitmap
  );

uint32
  soc_pb_pp_sw_db_ipv4_default_fec_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    default_fec
  );

uint32
  soc_pb_pp_sw_db_ipv4_default_fec_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *default_fec
  );

uint32
  soc_pb_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_lpm_entries_in_lpm
  );

uint32
  soc_pb_pp_sw_db_ipv4_nof_lpm_entries_in_lpm_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_lpm_entries_in_lpm
  );

uint32
  soc_pb_pp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  );

uint32
  soc_pb_pp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  );


uint32
  soc_pb_pp_sw_db_ipv4_nof_vrfs_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_vrfs
  );

uint32
  soc_pb_pp_sw_db_ipv4_nof_vrfs_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_vrfs
  );

uint8
  soc_pb_pp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   vrf_ndx
  );

uint32
  soc_pb_pp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN SOC_PB_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr
  );

uint32
  soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  SOC_PB_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  );

#ifdef SOC_SAND_DEBUG
uint32
  soc_pb_pp_sw_db_ipv4_lem_add_fail_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    *ipv4_lem_add_fail
  );

uint32
  soc_pb_pp_sw_db_ipv4_lem_add_fail_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *ipv4_lem_add_fail
  );
#endif


/*
 * ipv4 }
 */

/* ILM { */

uint32
  soc_pb_pp_sw_db_ilm_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_ilm_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_ilm_key_mask_port_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    mask_port
  );

uint32
  soc_pb_pp_sw_db_ilm_key_mask_port_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *mask_port
  );

uint32
  soc_pb_pp_sw_db_ilm_key_mask_inrif_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    mask_inrif
  );

uint32
  soc_pb_pp_sw_db_ilm_key_mask_inrif_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *mask_inrif
  );
/* } */
/*
* llp trap {
*/

uint32
  soc_pb_pp_sw_db_llp_trap_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_llp_trap_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp trap }
*/

/*
* llp mirror {
*/

uint32
  soc_pb_pp_sw_db_llp_mirror_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_llp_mirror_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp mirror }
*/

/*
* llp vid_assign {
*/

uint32
  soc_pb_pp_sw_db_llp_vid_assign_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_llp_vid_assign_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_llp_vid_assign_port_sa_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            local_port_ndx,
    SOC_SAND_IN  uint8             enable
  );

uint32
  soc_pb_pp_sw_db_llp_vid_assign_port_sa_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            local_port_ndx,
    SOC_SAND_OUT  uint8             *enable
  );

/*
* llp vid_assign }
*/


/* EG MIRROR { */

uint32
  soc_pb_pp_sw_db_eg_mirror_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_eg_mirror_terminate(
    SOC_SAND_IN  int unit
  );

/* EG MIRROR } */


/*
* llp cos {
*/

uint32
  soc_pb_pp_sw_db_llp_cos_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_llp_cos_terminate(
    SOC_SAND_IN  int unit
  );

/*
* llp cos }
*/

/*
* eth_policer {
*/

uint32
  soc_pb_pp_sw_db_eth_policer_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_eth_policer_terminate(
    SOC_SAND_IN  int unit
  );

void
  soc_pb_pp_sw_db_eth_policer_config_status_bit_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint32                                         bit_offset,
    SOC_SAND_IN  uint8                                        status
  );

void
  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint32                                         bit_offset,
    SOC_SAND_OUT  uint8                                       *status
  );

/*
* eth_policer }
*/


/*
 *	Forward MAC Table
 */
uint32
  soc_pb_pp_sw_db_fwd_mact_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_fwd_mact_terminate(
    SOC_SAND_IN  int unit
  );

void
  soc_pb_pp_sw_db_learning_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_MACT_LEARNING_MODE  learning_mode
  );

SOC_PB_PP_FRWRD_MACT_LEARNING_MODE
  soc_pb_pp_sw_db_learning_mode_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_is_petra_a_compatible_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  is_petra_a_compatible
  );

uint8
  soc_pb_pp_sw_db_is_petra_a_compatible_get(
    SOC_SAND_IN int unit
  );

/*
* lif cos {
*/

uint32
  soc_pb_pp_sw_db_lif_cos_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_lif_cos_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_set(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  uint8                                        map_from_tc_dp
  );

uint32
  soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_OUT uint8                                        *map_from_tc_dp
  );

/*
* llp cos }
*/

/*
 * lif table {
 */
uint32
  soc_pb_pp_sw_db_lif_table_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID         lif_id,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_TYPE   entry_type
  );

uint32
  soc_pb_pp_sw_db_lif_table_entry_use_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID         lif_id,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ENTRY_TYPE   *entry_type
  );

/*
* llp table }
*/


/*
 * FEC
 */
uint32
  soc_pb_pp_sw_db_fwd_fec_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_fwd_fec_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_fec_glbl_info_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_FEC_GLBL_INFO    *glbl_info
  );

uint32
  soc_pb_pp_sw_db_fec_glbl_info_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_GLBL_INFO    *glbl_info
  );

uint32
  soc_pb_pp_sw_db_fec_entry_type_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32          fec_ndx,
    SOC_SAND_IN SOC_PB_PP_FEC_TYPE    entry_type
  );

uint32
  soc_pb_pp_sw_db_fec_entry_type_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32          fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FEC_TYPE   *entry_type
  );
/*
 *	Diag
 */
uint32
  soc_pb_pp_sw_db_fwd_diag_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_fwd_diag_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_fp_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_fp_terminate(
    SOC_SAND_IN  int unit
  );


uint32
  soc_pb_pp_sw_db_diag_trap_save_status_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint8              saved
  );

uint32
  soc_pb_pp_sw_db_diag_trap_save_status_get(
    SOC_SAND_IN int              unit,
    SOC_SAND_OUT uint8              *saved
  );

uint32
  soc_pb_pp_sw_db_diag_trap_dest_set(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN uint32                               trap_code_ndx,
    SOC_SAND_IN SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  	 *trap_info
  );

uint32
  soc_pb_pp_sw_db_diag_trap_dest_get(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN uint32                               trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  	 *trap_info
  );

uint32
  soc_pb_pp_sw_db_diag_mode_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN SOC_PB_PP_DIAG_MODE_INFO  	 *mode_info
  );

uint32
  soc_pb_pp_sw_db_diag_mode_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_MODE_INFO  	 *mode_info
  );

/*
 *	Field Processor
 */
uint32
  soc_pb_pp_sw_db_fp_db_info_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *db_info
  );

uint32
  soc_pb_pp_sw_db_fp_db_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO *db_info
 );

uint32
  soc_pb_pp_sw_db_fp_db_entries_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  SOC_PB_PP_SW_DB_FP_ENTRY  *db_entries
  );

uint32
  soc_pb_pp_sw_db_fp_db_entries_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT SOC_PB_PP_SW_DB_FP_ENTRY   *db_entries
 );

void
  soc_pb_pp_sw_db_fp_db_predefined_acl_key_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   db_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_acl_key
  );

void
  soc_pb_pp_sw_db_fp_db_predefined_acl_key_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      db_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_PREDEFINED_ACL_KEY   *predefined_acl_key
 );

uint32
  soc_pb_pp_sw_db_fp_db_udf_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     udf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_QUAL_INFO        *qual_info
  );

uint32
  soc_pb_pp_sw_db_fp_db_udf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      udf_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_QUAL_INFO         *qual_info
 );

void
  soc_pb_pp_sw_db_fp_ce_instr_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE    ce_instr
  );

SOC_PB_PP_FP_QUAL_TYPE
  soc_pb_pp_sw_db_fp_ce_instr_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx
  );

uint32
  soc_pb_pp_sw_db_fp_fem_entry_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY   *fem_entry
  );

uint32
  soc_pb_pp_sw_db_fp_fem_entry_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_ENTRY   *fem_entry
  );

uint32
  soc_pb_pp_sw_db_fp_db_key_location_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             db_id_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_LOCATION *key_loc
  );

uint32
  soc_pb_pp_sw_db_fp_db_key_location_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             db_id_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_LOCATION *key_loc
  );

void
  soc_pb_pp_sw_db_fp_is_tag_used_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN uint32 tag_action_ndx,
    SOC_SAND_IN uint32 is_tag_used_db_id
  );

uint32
  soc_pb_pp_sw_db_fp_is_tag_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN uint32 tag_action_ndx
  );

void
  soc_pb_pp_sw_db_fp_inner_eth_nof_tags_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN uint32  inner_eth_nof_tags
  );

uint32
  soc_pb_pp_sw_db_fp_inner_eth_nof_tags_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx
  );

void
  soc_pb_pp_sw_db_fp_loc_key_changed_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx,
    SOC_SAND_IN SOC_PB_PP_FP_KEY_CHANGE_LOCATION  loc_key_changed
  );

SOC_PB_PP_FP_KEY_CHANGE_LOCATION
  soc_pb_pp_sw_db_fp_loc_key_changed_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 pfg_ndx
  );

void
  soc_pb_pp_sw_db_fp_key_change_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8   key_change_size
  );

uint8
  soc_pb_pp_sw_db_fp_key_change_size_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_fp_db_id_cycle_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx,
    SOC_SAND_IN uint8 db_id_cycle
  );

uint8
  soc_pb_pp_sw_db_fp_db_id_cycle_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx
  );

uint32
  soc_pb_pp_sw_db_fp_dt_entry_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO   *entry
  );

uint32
  soc_pb_pp_sw_db_fp_dt_entry_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO  *entry
  );

void
  soc_pb_pp_sw_db_fp_db_has_key_format_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx,
    SOC_SAND_IN uint8 has_key_format
  );

uint8
  soc_pb_pp_sw_db_fp_db_has_key_format_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx
  );

void
  soc_pb_pp_sw_db_fp_key_desc_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32         db_id_ndx,
    SOC_SAND_IN SOC_PB_PP_FP_KEY_DESC *key_desc
  );

void
  soc_pb_pp_sw_db_fp_key_desc_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32         db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC *key_desc
  );

void
  soc_pb_pp_sw_db_fp_tcam_db_key_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx,
    SOC_SAND_IN uint32 key_size
  );

uint32
  soc_pb_pp_sw_db_fp_tcam_db_key_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id_ndx
  );

/*
 *	Operation mode
 */
uint8
  soc_pb_pp_sw_db_authentication_enable_get(
    SOC_SAND_IN int unit
  );

uint8
  soc_pb_pp_sw_db_pvlan_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_split_horizon_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 split_horizon_enable
  );

uint8
  soc_pb_pp_sw_db_split_horizon_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_authentication_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 authentification_enable
  );

uint8
  soc_pb_pp_sw_db_system_vsi_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_system_vsi_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 system_vsi_enable
  );

uint8
  soc_pb_pp_sw_db_hairpin_enable_get(
    SOC_SAND_IN int unit
  );

uint8
  soc_pb_pp_sw_db_ipv4_host_extend_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_hairpin_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 hairpin_enable
  );

uint32
  soc_pb_pp_sw_db_mpls_ether_types_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_SAND_PP_ETHER_TYPE    mpls_ether_types[2]
  );

uint32
  soc_pb_pp_sw_db_mpls_ether_types_get(
    SOC_SAND_IN int                  unit,
    SOC_SAND_OUT SOC_SAND_PP_ETHER_TYPE        mpls_ether_types[2]
  );

uint32
  soc_pb_pp_sw_db_isem_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_isem_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_lag_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_pp_sw_db_lag_terminate(
    SOC_SAND_IN  int unit
  );

uint8
  soc_pb_pp_sw_db_isem_ext_key_enabled_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_isem_ext_key_enabled_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 ext_key_enabled
  );

uint8
  soc_pb_pp_sw_db_lag_lb_key_is_symtrc_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_lag_lb_key_is_symtrc_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 lb_key_is_symtrc
  );

SOC_PB_PP_HASH_MASKS
  soc_pb_pp_sw_db_lag_lb_key_mask_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_pp_sw_db_lag_lb_key_mask_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PB_PP_HASH_MASKS masks
  );

uint32
  soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_IN  uint8             is_exist
  );

uint32
  soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_PORT            pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_OUT uint8             *is_exist
  );

uint32
  soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_additional_buff_size_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT uint32                     *buff_byte_size
  );

uint32
  soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_save(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_PP_SW_DB_DEVICE         *ssr_data,
    SOC_SAND_OUT uint8                      *additional_data,
    SOC_SAND_IN  uint32                      additional_data_size
  );

uint32
  soc_pb_pp_ssr_PB_PP_SW_DB_SSR_V01_data_load(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                      *ssr_stream,
    SOC_SAND_IN  uint32                     size_of_pb_pp_ssr,
    SOC_SAND_IN  uint8                      *additional_data
  );

uint8
  soc_pb_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun(
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  );

uint8
  soc_pb_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun(
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info
  );

uint32
soc_pb_pp_sw_db_test_defragmet(SOC_SAND_IN  int unit);
/*********************************************************************
* NAME:
 *   soc_pb_pp_sw_db_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_sw_db module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_sw_db_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_sw_db_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_sw_db module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_sw_db_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_SW_DB_INCLUDED__*/
#endif
