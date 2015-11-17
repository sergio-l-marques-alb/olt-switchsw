/* $Id: arad_pp_eg_encap_access.h,v 1.14 Broadcom SDK $
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

#ifndef __ARAD_PP_EG_ENCAP_ACCESS_INCLUDED__
/* { */
#define __ARAD_PP_EG_ENCAP_ACCESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* Entry size */
#define ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE                          ( 3)

/* Start location entry when it is 2 entries in one table entry */
#define ARAD_PP_EG_ENCAP_ACCESS_1ST_ENTRY_START_LOCATION (0)
#define ARAD_PP_EG_ENCAP_ACCESS_2ND_ENTRY_START_LOCATION (37)

/* Mutual Prefix Entry - other fields might overlab key prefix */
#define ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit)                     (SOC_DPP_DEFS_GET(unit, eg_encap_access_prefix_lsb))
#define ARAD_PP_EG_ENCAP_ACCESS_PREFIX_MSB(unit)                     (SOC_DPP_DEFS_GET(unit, eg_encap_access_prefix_msb))
#define ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit)                SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_MSB(unit))


/* Mutual Prefix Entry - other fields might overlab key prefix */
#define ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_LSB                     (0)
#define ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_MSB                     (1)
#define ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_NOF_BITS                SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_LSB, ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_MSB)


/* MPLS format - Relavant for all MPLS formats */
#define ARAD_PP_EG_ENCAP_ACCESS_MPLS_ENTRY_PREFIX_MASK         (0x8) /* 4'b1000*/

/* IP tunnel format */
#define ARAD_PP_EG_ENCAP_ACCESS_IP_ENTRY_PREFIX_MASK         (0xf) /* 4'b1111*/

/* Link Layer format */
#define ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK          (0xe) /* 4'b1110 */

/* Data format */
#define ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_PREFIX_MASK          (0xe) /* 4'b1110 */

/* OUT-RIF format */
#define ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_2_ENT_PREFIX_MASK     (0x3) /* 2'b11 */

/* Out-AC format */
#define ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_2_ENT_PREFIX_MASK     (0x3) /* 2'b11 */

#define ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK       (0xf) /* 4'b1111 */

/* Trill format - currently not in use. */
#define ARAD_PP_EG_ENCAP_ACCESS_TRILL_2_ENT_PREFIX_MASK       (0x4) /* 3'b100 */
/* } */
/*************
 * MACROS    *
 *************/
/* { */
/* Convert from outlif to entry offset */
#define ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(outlif) \
  (outlif >> 1)

/* Convert from entry offset to bank id (0-15) */
#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset) \
  (entry_offset >> SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs))

/* Convert from entry offset to internal offset inside a bank (0-2K) */
#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset) \
  (entry_offset - (ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset) << SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)))


#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_ENTRY_OFFSET_IS_LEGAL(unit, entry_offset)  \
      (entry_offset < SOC_DPP_DEFS_GET(unit, nof_eedb_lines))

#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_ID(entry_offset) \
      ( ( entry_offset - ARAD_PP_EG_ENCAP_ACCESS_NOF_EEDB_ENTRIES ) / ARAD_PP_EG_ENCAP_ACCESS_NOF_TOP_BANK_ENTRIES)

#define ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_INTERNAL_OFFSET(entry_offset) \
      ( ( entry_offset - ARAD_PP_EG_ENCAP_ACCESS_NOF_EEDB_ENTRIES ) % ARAD_PP_EG_ENCAP_ACCESS_NOF_TOP_BANK_ENTRIES)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum 
{
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS                   = 0x8, /* 4'b1xxx */
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA                   = 0x6, /* 4'b011x */
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER             = 0x4, /* 4'b010x */
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP                     = 0x3, /* 4'b0011 */
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_ISID                   = 0x2, /* 4'b0010 */
  /* Other entries that can be divided into two in one entry */
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER                  = 0x0, /* 4'b0000 */ 
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE                   = 0x2, /* 4'b0010 */ 
  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_TRILL                  = 0x1  /* 4'b0001 */ 

} ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE;


typedef enum 
{
  ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_TRILL            = 0x4, /* 3'b1xx */
  ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_OUT_RIF          = 0x1, /* 2'b01 */
  ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_OUT_AC           = 0x0 /* 2'b00 */
} ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE;


typedef enum
{
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE             = 0,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS             = 1,    
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA             = 2,  
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER       = 3,      
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP               = 4,   
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ISID             = 5,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL            = 6,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF          = 7,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC           = 8,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL      = 9, /* push*/
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP        = 10,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP         = 11,
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6        = 17 , 
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_ARP_OVERLAY = 18, 
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER   = 19


} ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE;

typedef enum{
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val = 0,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val = 7,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val  = 8,
  ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val = 9
} ARAD_PP_EG_ENCAP_MPLS1_COMMAND;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_EG_ENCAP_ACCESS_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_EG_ENCAP_ACCESS_FIRST,
  ARAD_PP_EG_ENCAP_ACCESS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_ACCESS_LOCAL_OUTLIF_ADDITIONAL_DATA_LOCATION_GET,
  ARAD_PP_EG_ENACP_ACCESS_ADDITIONAL_DATA_ENTRY_GET,
  ARAD_PP_EG_ENACP_ACCESS_ADDITIONAL_DATA_ENTRY_SET,
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_COMMAND_GET_UNSAFE,
  /*
   * Last element. Do no touch.
   */
  ARAD_PP_EG_ENCAP_ACCESS_PROCEDURE_DESC_LAST
} ARAD_PP_EG_ENCAP_ACCESS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */
  ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR = ARAD_PP_ERR_DESC_BASE_EG_ENCAP_ACCESS_FIRST,
  ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR,
  
  /*
   * Last element. Do no touch.
   */
  ARAD_PP_EG_ENCAP_ACCESS_ERR_LAST
} ARAD_PP_EG_ENCAP_ACCESS_ERR;

typedef struct
{  
  uint32 next_outlif;
  uint32 next_vsi_lsb;
  /* Two fields below are valid if MPLS1-Command is push-0 through push-7 */
  uint32 mpls2_label;
  uint32 mpls2_command;
  /* Else, three fields below are valid if MPLS1-Command is pop */
  uint32 has_cw;
  uint32 tpid_profile;
  uint32 model_is_pipe;
  uint32 upper_layer_protocol;
  /* Else, upper fields above are not valid */
  uint32 mpls1_label;
  uint32 mpls1_command;  
  uint32 oam_lif_set;
  uint32 drop;  
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT;

typedef struct
{
  uint32 next_outlif;  
  uint32 next_vsi_lsb;
  uint32 encapsulation_mode;
  uint32 ipv4_tos_index;
  uint32 ipv4_ttl_index;
  uint32 ipv4_src_index;
  uint32 ipv4_dst;
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT;

typedef struct
{    
  uint32 next_outlif_lsb;
  uint32 next_outlif_valid;
  uint32 vid;
  uint32 vid_valid;  
  uint32 dest_mac[2];
  uint32 remark_profile;
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT;


typedef struct
{
  uint32 next_outlif;
  uint32 next_outlif_valid;
  uint32 data[ARAD_PP_EG_ENCAP_DATA_INFO_MAX_SIZE];
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT;

typedef struct
{
  uint32 next_outlif;
  uint32 next_outlif_valid;
  uint32 next_vsi_lsb;
  uint32 remark_profile;
  uint32 oam_lif_set;
  uint32 drop; 
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT;

typedef struct
{
  uint32 pcp_dei_profile;
  uint32 vid[2];
  uint32 vlan_edit_profile;
  uint32 oam_lif_set;
  uint32 drop;
#ifdef BCM_88660_A0
  uint32 use_as_data_entry;
  uint32 data[ARAD_PP_EG_ENCAP_DATA_INFO_MAX_SIZE];
#endif
  uint32 outlif_profile;
} ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT;

typedef struct
{
  uint32 next_outlif;
  uint32 next_outlif_valid;
  uint32 m;
  uint32 nick;
  uint32 oam_lif_set;
  uint32 drop; 
  uint32 outlif_profile;
  uint32 my_nickname_index; 
} ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT;


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
  arad_pp_eg_encap_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Inits devision of the Egress Encapsulation Table entry
 *     This configuration only take effect the entry type is not ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER and 
 *     ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_encap_access_entry_init_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif
  );

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE *key_prefix
  );

uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
        SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  );

uint32
  arad_pp_eg_encap_access_local_outlif_additional_data_location_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int outlif, 
    SOC_SAND_OUT int *bank_id, 
    SOC_SAND_OUT int *entry_id 
  );

uint32
arad_pp_eg_enacp_access_additional_data_entry_get(    
    SOC_SAND_IN int unit, 
    SOC_SAND_IN  uint32 outlif,
    SOC_SAND_OUT uint32 *data
  );

uint32
arad_pp_eg_enacp_access_additional_data_entry_set(    
    SOC_SAND_IN int unit, 
    SOC_SAND_IN  uint32 outlif,
    SOC_SAND_OUT uint32 *data
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_encap_access_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_esem module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_eg_encap_access_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_esem module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_EG_ENCAP_ACCESS_INCLUDED__*/
#endif


