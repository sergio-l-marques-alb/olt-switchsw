/* $Id: pcp_sw_db.h,v 1.9 Broadcom SDK $
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

#ifndef __SOC_PCP_SW_DB_INCLUDED__
/* { */
#define __SOC_PCP_SW_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_group_member_list.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/PCP/pcp_frwrd_ipv4.h>
#include <soc/dpp/PCP/pcp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_exact_match.h>

#include <soc/dpp/PCP/pcp_api_mgmt.h>
#include <soc/dpp/PCP/pcp_api_frwrd_mact_mgmt.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_SW_DB_DEBUG                                           (0)

#define PCP_SW_IPV4_VRF_BITMAP_SIZE (8)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define PCP_SW_DB_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Pcp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Pcp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define PCP_SW_DB_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Pcp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Pcp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;




#define PCP_SW_DB_IPV4_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Pcp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define PCP_SW_DB_IPV4_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Pcp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 *  Global Operation mode for PCP
 */
typedef PCP_MGMT_OP_MODE  PCP_SW_DB_OP_MODE;

typedef struct
{
  PCP_FRWRD_MACT_LEARNING_MODE   learning_mode;
  uint8                      is_petra_a_compatible;

} PCP_SW_DB_FWD_MACT;

/*
 *  Operation mode for Local Modules
*/
typedef struct
{
	/*
	 *  Operation mode for statistics interface
	 */
	PCP_SW_DB_FWD_MACT 		fwd_mact;

}  PCP_SW_DB_ELK_OP_MODE;

typedef struct
{
#if defined(PCP_SW_DB_DEBUG)
  SOC_SAND_MAGIC_NUM_VAR
#endif
 /*
  *  The information for mapping key(rmep,mep) to rmep index
  */
  SOC_SAND_EXACT_MATCH_INFO rmep_map_info;

} PCP_SW_DB_DEVICE_RMEP_INFO;



/* Hw information need for LPM ALG */
typedef struct
{
  uint8 elk_enable;
  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT dram_size ;
  PCP_MGMT_ELK_LKP_MODE   lkup_mode; 
  uint32   bank_size;  /* physical bank size */
  uint32   mems_per_bank;  /* number of memory/blocks per bank*/
} PCP_SW_DB_LPM_INFO;

/* Hw information need for LPM ALG */
typedef struct
{
  uint8 bank_id;
  uint32 address;
} PCP_IPV4_LPM_FREE_LIST_ITEM_INFO;


typedef struct
{
  PCP_IPV4_LPM_MNGR_INFO
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
  PCP_IPV4_LPM_FREE_LIST_ITEM_INFO
    *free_list;
  uint32
    free_list_size;

  PCP_SW_DB_LPM_INFO
    lpm_hw_info;
#ifdef SOC_SAND_DEBUG
  /* in debug mode, if TRUE lem_route_add will fail. used for denug purposes */
  uint8
    lem_add_fail;
#endif

}  PCP_SW_DB_IPV4_INFO;
typedef struct
{
#if PCP_SW_DB_DEBUG
  SOC_SAND_MAGIC_NUM_VAR
#endif

  PCP_SW_DB_OP_MODE op_mode;
  PCP_SW_DB_ELK_OP_MODE elk;
  
  PCP_SW_DB_IPV4_INFO *ipv4_info;
  PCP_SW_DB_DEVICE_RMEP_INFO *rmep_info;

} PCP_SW_DB_DEVICE;

typedef struct
{
#if PCP_SW_DB_DEBUG
  SOC_SAND_MAGIC_NUM_VAR
#endif
  uint8
    init;

  PCP_SW_DB_DEVICE*
    device[SOC_SAND_MAX_DEVICE];
} PCP_SW_DB;

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

uint32
  pcp_sw_db_rmep_exact_match_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_INFO       **exact_match
  );

void
  soc_petra_sw_db_op_mode_elk_enable_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint8  enable
  );

uint8
  soc_petra_sw_db_op_mode_elk_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint32  ilm_key_mask_bitmap
  );

uint32
  soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_op_mode_oam_enable_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint8  enable
  );

uint8
  soc_petra_sw_db_op_mode_oam_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_op_mode_sts_conf_word_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint32  conf_word
  );

uint32
  soc_petra_sw_db_op_mode_sts_conf_word_get(
    SOC_SAND_IN int unit
  );

void
  pcp_sw_db_learning_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN PCP_FRWRD_MACT_LEARNING_MODE  learning_mode
  );

PCP_FRWRD_MACT_LEARNING_MODE
  pcp_sw_db_learning_mode_get(
    SOC_SAND_IN int unit
  );

void
  pcp_sw_db_is_petra_a_compatible_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  is_petra_a_compatible
  );

uint8
  pcp_sw_db_is_petra_a_compatible_get(
    SOC_SAND_IN int unit
  );

uint32
  pcp_sw_db_initialize(void);

uint32
  pcp_sw_db_device_initialize(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE    *oper_mode
  );

/*
* ipv4 {
*/

uint32
  pcp_sw_db_ipv4_initialize(
   SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  max_nof_routes[PCP_NOF_VRFS],
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  *mem_to_bank,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, /* must be uint32 */
    SOC_SAND_IN PCP_IPV4_LPM_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  );

uint32
  pcp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  );


uint32
  pcp_sw_db_ipv4_lpm_hw_info_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT PCP_SW_DB_LPM_INFO                          *lpm_info
  );

uint32
  pcp_sw_db_ipv4_lpm_hw_info_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN PCP_SW_DB_LPM_INFO                           *lpm_info
  );


uint32
  pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
  );

uint32
  pcp_sw_db_ipv4_cache_mode_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32                                     *cache_mode_bitmask
  );

uint32
  pcp_sw_db_ipv4_cache_mode_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     cache_mode_bitmask
  );


uint32
  pcp_sw_db_ipv4_cache_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint8	                          	     *is_modified
  );

uint32
  pcp_sw_db_ipv4_cache_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint8	                          	     modified
);

uint32
  pcp_sw_db_ipv4_cache_vrf_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                     vrf_ndx,
    SOC_SAND_OUT uint32                                     *is_modified
  );

uint32
  pcp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                    vrf_ndx,
    SOC_SAND_IN uint32                                     is_modified
  );

uint32
  pcp_sw_db_ipv4_cache_modified_bitmap_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32	                          	     *modified_bitmap
  );

uint32
  pcp_sw_db_ipv4_cache_modified_bitmap_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32	                          	     *modified_bitmap
  );

uint32
  pcp_sw_db_ipv4_default_fec_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    default_fec
  );

uint32
  pcp_sw_db_ipv4_default_fec_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *default_fec
  );

uint32
  pcp_sw_db_ipv4_nof_lpm_entries_in_lpm_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_lpm_entries_in_lpm
  );

uint32
  pcp_sw_db_ipv4_nof_lpm_entries_in_lpm_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_lpm_entries_in_lpm
  );

uint32
  pcp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  );

uint32
  pcp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  );


uint32
  pcp_sw_db_ipv4_nof_vrfs_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_vrfs
  );

uint32
  pcp_sw_db_ipv4_nof_vrfs_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_vrfs
  );

uint8
  pcp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   vrf_ndx
  );

uint32
  pcp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN PCP_IPV4_LPM_MNGR_INFO    *lpm_mngr
  );

uint32
  pcp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  PCP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  );

#ifdef SOC_SAND_DEBUG
uint32
  pcp_sw_db_ipv4_lem_add_fail_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    *ipv4_lem_add_fail
  );

uint32
  pcp_sw_db_ipv4_lem_add_fail_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *ipv4_lem_add_fail
  );
#endif


/*
 * ipv4 }
 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_SW_DB_INCLUDED__*/
#endif
