
/* $Id: arad_pp_frwrd_ip_tcam.h,v 1.19 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ARAD/ARAD_PP/include/arad_pp_frwrd_ip_tcam.h
*
* MODULE PREFIX:  arad_pp
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __ARAD_PP_FRWRD_IP_TCAM_INCLUDED__
/* { */
#define __ARAD_PP_FRWRD_IP_TCAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/ARAD/arad_sw_db_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_tcam.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_workarounds.h>
#endif
/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define ARAD_PP_FRWRD_IP_TCAM_IPV4_MC (1 << 0)
#define ARAD_PP_FRWRD_IP_TCAM_IPV6_UC (1 << 1)
#define ARAD_PP_FRWRD_IP_TCAM_IPV6_MC (1 << 2)

/*
* the length of key stream
*    1 : VRF
*    16 :  dest IP
*    1: mask len
*/
#define ARAD_PP_IPV6_UC_OR_VPN_KEY_LEN_BYTES 18

/*
* the length of key stream
*    2 : in-rif
*    2:  in_rif mask
*    15 : group
*/
#define ARAD_PP_IPV6_MC_KEY_LEN_BYTES 19

/*
* the length of key stream
*    1 : in-rif valid
*    2 : in_rif
*    4 : SIP
*    1 : SIP mask len
*    4 : group
*/
#define ARAD_PP_IPV4_MC_KEY_LEN_BYTES 12

/* length of the key stream that
* is big enough to contain either ipv6 uc / ipvc mc / ipv4 mc
* key stream 
* and also ELK key  
*/
#define ARAD_PP_IP_TCAM_KEY_LEN_BYTES 80
#define ARAD_PP_IP_TCAM_KEY_LEN_LONGS (ARAD_PP_IP_TCAM_KEY_LEN_BYTES / sizeof(uint32))
/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#define ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES (40) /* For 320b key size */
#define ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_LONGS (ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES / sizeof(uint32)) /* For 320b key size */
/* 
 * Key structure for ELK in the IP TCAM management 
 * common code for add/get/get_block/remove/table_clear code 
 */
typedef struct
{
  uint8 m_data[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
  uint8 m_mask[ARAD_PP_FRWRD_IP_ELK_FWD_MAX_KEY_LENGTH_IN_BYTES];
  uint32 priority;
} ARAD_PP_FRWRD_IP_ELK_FWD_KEY;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */



typedef union arad_pp_frwrd_ip_tcam_key_u
  {
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY ipv4_mc;
    SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY ipv6_uc;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY ipv6_mc;
    SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY ipv6_vpn;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    ARAD_PP_FRWRD_IP_ELK_FWD_KEY elk_fwd;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    ARAD_PP_SRC_BIND_IPV6_ENTRY ipv6_static_src_bind;
    ARAD_PP_TRILL_MC_ROUTE_KEY trill_mc;
    ARAD_PP_IPV6_COMPRESSED_ENTRY ip6_compression;
  } arad_pp_frwrd_ip_tcam_key_t;


/* 
 * General struct for all the possible TCAM keys: 
 * - the type differentiates between the possible formats 
 * - ipv4_mc / .. / ipv6_vpn corresponds to internal TCAM addition 
 * - elk for all the forwarding tables implemented in the ELK KBP 
 */
typedef struct
{
  ARAD_IP_TCAM_ENTRY_TYPE type;
  arad_pp_frwrd_ip_tcam_key_t key;
  uint32 vrf_ndx;
} ARAD_PP_IP_TCAM_ENTRY_KEY;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
typedef kbp_entry_t* kbp_entry_handle;
typedef struct
{
  kbp_entry_handle  db_entry;
} ARAD_SW_KBP_HANDLE;

typedef struct {
  /* 
   * In ACLs, a location table is needed 
   * to translate the SOC entry ID to the KBP entry handle. 
   */
  PARSER_HINT_ARR ARAD_SW_KBP_HANDLE *location_tbl;  /* for ACL location table */
  uint8 kbp_cache_mode;
} ARAD_KBP_FRWRD_IP;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

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
  arad_pp_frwrd_ip_tcam_init_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  required_uses
  );

uint32
  arad_pp_ip_tcam_entry_hw_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key,
    SOC_SAND_IN  uint8                       is_for_update,
    SOC_SAND_IN  ARAD_TCAM_ACTION           *action,
    SOC_SAND_IN  uint32                     data_indx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );

uint32
  arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(
     SOC_SAND_IN  uint8     *mask,
     SOC_SAND_IN  uint32    table_size_in_bytes,
     SOC_SAND_OUT uint32    *prefix_len
  );

uint32
    arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      frwrd_table_id,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_IN  uint8                       *data
    );

uint32
    arad_pp_frwrd_ip_tcam_kbp_route_remove(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint8                       frwrd_table_id,
       SOC_SAND_IN  uint32                      data_indx
    );

uint32
    arad_pp_frwrd_ip_tcam_kbp_lpm_route_get(
       SOC_SAND_IN  int         unit,
       SOC_SAND_IN  uint8       frwrd_table_id,
       SOC_SAND_IN  uint32      prefix_len,
       SOC_SAND_IN  uint8       *data,
       SOC_SAND_OUT uint8       *assoData,
       SOC_SAND_OUT uint8       *found
    );

uint32
    arad_pp_frwrd_ip_tcam_kbp_lpm_route_add(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      frwrd_table_id,
       SOC_SAND_IN  uint32                      data_indx,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_IN  uint8                       *data,
       SOC_SAND_IN  uint8                       *assoData,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
    arad_pp_frwrd_ip_tcam_kbp_route_add(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      frwrd_table_id,
       SOC_SAND_IN  uint32                      data_indx,
       SOC_SAND_IN  uint8                       is_for_update,
       SOC_SAND_IN  uint32                      priority,
       SOC_SAND_IN  uint8                       *m_data,
       SOC_SAND_IN  uint8                       *m_mask,
       SOC_SAND_IN  uint8                       *assoData,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    );

uint32
    arad_pp_frwrd_ip_tcam_ip_db_id_get(
      SOC_SAND_IN ARAD_IP_TCAM_ENTRY_TYPE entry_type
    );

uint32
  arad_pp_ip_tcam_entry_hw_general_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     tcam_db_id,
    SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE    tcam_key_type,
    SOC_SAND_IN  uint32                     entry_id,
    SOC_SAND_IN  uint8                      hit_bit_clear,
    SOC_SAND_OUT ARAD_TCAM_ENTRY            *entry,
    SOC_SAND_OUT uint32                     *priority,
    SOC_SAND_OUT ARAD_TCAM_ACTION           *action,
    SOC_SAND_OUT uint8                      *found,
    SOC_SAND_OUT uint8                      *hit_bit
  );

uint32
  arad_pp_frwrd_ip_tcam_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  ARAD_TCAM_ACTION                                *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  );

uint32
  arad_pp_frwrd_ip_tcam_route_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                 *route_key,
    SOC_SAND_IN  uint8                                     exact_match,
    SOC_SAND_OUT ARAD_TCAM_ACTION                           *action,
    SOC_SAND_OUT uint8                                     *found,
    SOC_SAND_OUT uint8                                     *hit_bit
  );

uint32
  arad_pp_frwrd_ip_tcam_route_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_INOUT  ARAD_PP_IP_ROUTING_TABLE_RANGE               *block_range_key,
    SOC_SAND_OUT  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_OUT ARAD_TCAM_ACTION                            *action,
    SOC_SAND_OUT uint32                                      *nof_entries
  );



uint32
  arad_pp_frwrd_ip_tcam_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key
  );



uint32
  arad_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint8                                    clear_all_vrf
  );

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
/* #define KBP_COMPARE_DEBUG 1 */
#ifdef KBP_COMPARE_DEBUG
/* Compare to see if insertion and deletion was correct */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_compare(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint8                      *m_data
  );
#endif /* KBP_COMPARE_DEBUG */

uint32
  arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     logical_entry_size_in_bytes,
    SOC_SAND_IN  uint32                     *buffer_data,
    SOC_SAND_IN  uint32                     *buffer_mask,
    SOC_SAND_OUT uint8                      *data,
    SOC_SAND_OUT uint8                      *mask
  );

/* Build the buffer for payload - then encode according to KBP expectations */
uint32
  arad_pp_frwrd_ip_tcam_route_to_kbp_payload_buffer_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     table_payload_in_bytes,
    SOC_SAND_IN  uint32                     *asso_data_buffer,
    SOC_SAND_OUT uint8                      *asso_data
  );
uint32
  arad_pp_frwrd_ip_tcam_route_from_kbp_key_decode(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint8                          *data,
    SOC_SAND_IN  uint8                          *mask,
    SOC_SAND_IN  int                            valid_bytes,
    SOC_SAND_OUT uint32                         *route_key_data,
    SOC_SAND_OUT uint32                         *route_key_mask
  );

uint32
  arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint8                      *asso_data,
    SOC_SAND_OUT uint32                     *asso_data_buffer
  );

uint32
    arad_pp_frwrd_ip_tcam_kbp_route_compare(
       SOC_SAND_IN  int              unit,
       SOC_SAND_IN  uint8               ltr_num,
       SOC_SAND_OUT uint8               *master_key,
       SOC_SAND_OUT uint8               *found
    );

uint32
    arad_pp_frwrd_ip_tcam_kbp_table_clear(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  uint32   frwrd_table_id
    );

/* 
 * Conversion functions from application-dependent 
 * structure (already transformed in a LEM access key) 
 * to the regular TCAM entry handling functions: 
 * add, remove, get and get-block 
 *  
 * In ARAD_PP_LEM_ACCESS_KEY, only nof_params and param[] 
 * are used. 
 */
typedef struct
{
  uint32 lsb_nof_bits[ARAD_PP_LEM_KEY_MAX_NOF_PARAMS][2]; /* 0 - lsb, 1 - nof-bits */
  uint32 logical_entry_size_in_bytes; /* The size of data to be searched in entry (without padding) */
} ARAD_PP_FRWRD_IP_TCAM_KBP_TABLE_ATTRIBUTES;


/* Encode the LEM key under 32 bits */
#define ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, param_ndx, fld_val, route_key)            \
        {                                                                                                         \
            res = arad_pp_frwrd_ip_tcam_lem_key_encode(unit, frwrd_table_id, param_ndx, fld_val, route_key); \
            SOC_SAND_CHECK_FUNC_RESULT(res, 1000 + param_ndx, exit);                                              \
        }
#define ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, param_ndx, fld_val, route_key_data, route_key_mask)             \
        SHR_BITCOPY_RANGE(fld_val, 0, route_key_data->param[param_ndx].value, 0, route_key_data->param[param_ndx].nof_bits);            \
        SHR_BITAND_RANGE(fld_val, route_key_mask->param[param_ndx].value, 0, route_key_mask->param[param_ndx].nof_bits, fld_val);       
                      


/* Build the mask according to the properties of data */
uint32
  arad_pp_frwrd_ip_tcam_route_kbp_route_key_mask_build(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask
  );

uint32
  arad_pp_frwrd_ip_tcam_lem_key_encode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint32                     param_ndx,
    SOC_SAND_IN  uint32                     fld_val, /* only for values with less than 32 bits */
    SOC_SAND_INOUT ARAD_PP_LEM_ACCESS_KEY     *route_key
  );

/* insert entry to the KBP data-mask */
uint32
  arad_pp_frwrd_ip_tcam_kbp_tcam_entry_add(
     int unit, 
     ARAD_KBP_FRWRD_IP_TBL_ID frwrd_table_id, 
     uint32 entry_id_ndx, 
     uint8 is_for_update, 
     uint32 priority, 
     uint32 data[ARAD_TCAM_ENTRY_MAX_LEN], 
     uint32 mask[ARAD_TCAM_ENTRY_MAX_LEN], 
     uint32 value[ARAD_TCAM_ACTION_MAX_LEN], 
     SOC_SAND_SUCCESS_FAILURE *success
  );

uint32
  arad_pp_frwrd_ip_tcam_kbp_tcam_entry_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  uint32                     entry_id,
    SOC_SAND_OUT uint32                     data[ARAD_TCAM_ENTRY_MAX_LEN],
    SOC_SAND_OUT uint32                     mask[ARAD_TCAM_ENTRY_MAX_LEN],
    SOC_SAND_OUT uint32                     value[ARAD_TCAM_ACTION_MAX_LEN],
    SOC_SAND_OUT uint32                     *priority,    
    SOC_SAND_OUT uint8                      *found,
    SOC_SAND_OUT uint8                      *hit_bit
  );

uint32
  arad_pp_frwrd_ip_tcam_route_kbp_add_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask,
    SOC_SAND_IN  uint32                     priority,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD *payload,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );

uint32
  arad_pp_frwrd_ip_tcam_route_kbp_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask,
    SOC_SAND_OUT uint32                     *priority,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD *payload,
    SOC_SAND_OUT uint8                      *found
  );


uint32
  arad_pp_frwrd_ip_tcam_route_kbp_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_data,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY     *route_key_mask
  );

uint32
  arad_pp_frwrd_ip_tcam_route_kbp_hw_get_block_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID                frwrd_table_id,
    SOC_SAND_INOUT  ARAD_PP_IP_ROUTING_TABLE_RANGE       *block_range_key,
    SOC_SAND_INOUT  ARAD_PP_IP_TCAM_ENTRY_KEY            *keys,
    SOC_SAND_INOUT  ARAD_TCAM_ACTION                     *actions,
    SOC_SAND_INOUT  uint32                  			 *nof_entries
  );

uint32
  arad_pp_frwrd_ip_tcam_route_kbp_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID                   frwrd_table_id,
    SOC_SAND_INOUT  ARAD_PP_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                    *route_key_data,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                    *route_key_mask,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                 *payload,
    SOC_SAND_OUT uint32                                     *nof_entries
  );

#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
/* } */

uint32
  arad_pp_frwrd_ip_tcam_rewrite_entry(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  uint8                      entry_exists,
     SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION  *global_location,
     SOC_SAND_IN  ARAD_TCAM_LOCATION         *location
  );

soc_error_t 
  arad_pp_frwrd_ip_tcam_ipmc_ssm_add(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success
   );

soc_error_t
  arad_pp_frwrd_ip_tcam_ipmc_ssm_delete(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key
   );

soc_error_t
  arad_pp_frwrd_ip_tcam_ipmc_ssm_get(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT ARAD_PP_FRWRD_IP_ROUTE_STATUS*    route_status,
     SOC_SAND_OUT uint8* found
   );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_FRWRD_IP_TCAM_INCLUDED__*/
#endif



