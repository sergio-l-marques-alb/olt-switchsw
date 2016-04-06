#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_lag.c,v 1.35 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/*
 *	If set, when the LAG information is retrieved, the API verifies that
 *  incoming and outgoing directions are configured identically
 */
#define ARAD_PP_LAG_SYMMETRIC_VERIFY_EN 0

#define ARAD_PP_LAG_LAG_NDX_MAX(__unit)                     (arad_ports_lag_nof_lag_groups_get_unsafe(__unit))
#define ARAD_PP_LAG_SYS_PORT_MAX                                 (SOC_TMC_NOF_SYS_PHYS_PORTS_ARAD)
#define ARAD_PP_LAG_SEED_MAX                                     (SOC_SAND_U16_MAX)
#define ARAD_PP_LAG_HASH_FUNC_ID_MIN                             (0x3)
#define ARAD_PP_LAG_HASH_FUNC_ID_MAX                             (SOC_IS_ARADPLUS(unit)? SOC_PPC_LAG_LB_CRC_0x9019: SOC_PPC_LAG_LB_CRC_0x1105D)
#define ARAD_PP_LAG_KEY_SHIFT_MAX                                (15)
#define ARAD_PP_LAG_NOF_HEADERS_MAX                              (3)
#define ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_MAX                    (ARAD_PP_NOF_LAG_HASH_FRST_HDRS-1)
#define ARAD_PP_LAG_MEMBER_ID_MAX                                (255)
#define ARAD_PP_LAG_NOF_ENTRIES_MAX(__unit)                  (arad_ports_lag_nof_lag_entries_get_unsafe(__unit))
#define ARAD_PP_LAG_LB_TYPE_MAX                                  (ARAD_PP_NOF_LAG_LB_TYPES-1)

/*
 * Defines for hash vectors
 */

#define ARAD_PP_LB_VECTOR_INDEX_NONE             0x0
#define ARAD_PP_LB_VECTOR_INDEX_ETHERNET         0x1
#define ARAD_PP_LB_VECTOR_INDEX_FC               0x2
#define ARAD_PP_LB_VECTOR_INDEX_IPV4             0x3
#define ARAD_PP_LB_VECTOR_INDEX_IPV6             0x4
#define ARAD_PP_LB_VECTOR_INDEX_MPLSX1           0x5
#define ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1    0x6
#define ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL2    0x7
#define ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1    0x8
#define ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL2    0x9
#define ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL3    0xa
#define ARAD_PP_LB_VECTOR_INDEX_TRILL            0xb
#define ARAD_PP_LB_VECTOR_INDEX_L4               0xc
#define ARAD_PP_LB_VECTOR_INDEX_FC_VFT           0xd
#define ARAD_PP_LB_VECTOR_INDEX_CALC             0xf
#define ARAD_PP_LB_NOF_VECTOR_INDEXES            0xf


#define SOC_DPP_LB_VECTOR_INDEX_NONE             (SOC_IS_JERICHO(unit) ? 0x0  : 0x0)
#define SOC_DPP_LB_VECTOR_INDEX_ETHERNET         (SOC_IS_JERICHO(unit) ? 0x1  : 0x1)
#define SOC_DPP_LB_VECTOR_INDEX_ETHERNET_1       (SOC_IS_JERICHO(unit) ? 0x2  : 0x1)
#define SOC_DPP_LB_VECTOR_INDEX_ETHERNET_2       (SOC_IS_JERICHO(unit) ? 0x3  : 0x1)
#define SOC_DPP_LB_VECTOR_INDEX_ETHERNET_3       (SOC_IS_JERICHO(unit) ? 0x4  : 0x1)
#define SOC_DPP_LB_VECTOR_INDEX_FC               (SOC_IS_JERICHO(unit) ? 0x5  : 0x2)
#define SOC_DPP_LB_VECTOR_INDEX_IPV4             (SOC_IS_JERICHO(unit) ? 0x6  : 0x3)
#define SOC_DPP_LB_VECTOR_INDEX_IPV6             (SOC_IS_JERICHO(unit) ? 0x7  : 0x4)
#define SOC_DPP_LB_VECTOR_INDEX_MPLSX1           (SOC_IS_JERICHO(unit) ? 0x8  : 0x5)
#define SOC_DPP_LB_VECTOR_INDEX_MPLSX2_LABEL1    (SOC_IS_JERICHO(unit) ? 0x9  : 0x6)
#define SOC_DPP_LB_VECTOR_INDEX_MPLSX2_LABEL2    (SOC_IS_JERICHO(unit) ? 0xa  : 0x7)
#define SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL1    (SOC_IS_JERICHO(unit) ? 0xb  : 0x8)
#define SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL2    (SOC_IS_JERICHO(unit) ? 0xc  : 0x9)
#define SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL3    (SOC_IS_JERICHO(unit) ? 0xd  : 0xa)
#define SOC_DPP_LB_VECTOR_INDEX_TRILL            (SOC_IS_JERICHO(unit) ? 0xe  : 0xb)
#define SOC_DPP_LB_VECTOR_INDEX_L4               (SOC_IS_JERICHO(unit) ? 0xf  : 0xc)
#define SOC_DPP_LB_VECTOR_INDEX_FC_VFT           (SOC_IS_JERICHO(unit) ? 0x10 : 0xd)
#define SOC_DPP_LB_VECTOR_INDEX_CALC             (SOC_IS_JERICHO(unit) ? 0x1f : 0xf)
#define SOC_DPP_LB_NOF_VECTOR_INDEXES            (SOC_IS_JERICHO(unit) ? 0x1f : 0xf)


#define ARAD_PP_LB_VECTOR_NOF_BITS 48

            
/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define ARAD_PP_LB_FIELD_TO_OFFSET(field) (soc_sand_log2_round_down(field))

/*
 * given chunk offset return the start place (lsb to msb) of the 2bits to control this chunk
 */
#define ARAD_PP_LB_FIELD_TO_NIBLE(chunk_ofst, chunk_size)   (ARAD_PP_LB_VECTOR_NOF_BITS - chunk_ofst*2 - 2)

#define ARAD_PP_LB_MPLS_LBL1_VLD(key_indx) \
  ((key_indx== SOC_DPP_LB_VECTOR_INDEX_MPLSX1)||(key_indx== SOC_DPP_LB_VECTOR_INDEX_MPLSX2_LABEL1)||(key_indx==SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL1))

#define ARAD_PP_LB_MPLS_LBL2_VLD(key_indx) \
  ((key_indx >= SOC_DPP_LB_VECTOR_INDEX_MPLSX2_LABEL1) && (key_indx <= SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL2))

#define ARAD_PP_LB_MPLS_LBL3_VLD(key_indx) \
  ((key_indx >= SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL1) && (key_indx <= SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL3))

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
typedef struct
{
	uint32 key_index;
	uint32 nof_nibles;
	uint32 nbls[8];
}ARAD_PP_LB_HASH_FIELD_INFO;

typedef struct
{
	uint32 offset;
	uint32 index5;
	uint32 index4;
	uint32 index3;
	uint32 index2;
	uint32 index1;
}ARAD_PP_LB_HASH_MAP;

typedef struct
{
    uint32 key_index;
    uint8 chunk_size;
}ARAD_PP_LB_HASH_CHUNK_SIZE;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_lag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_MEMBER_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_HASHING_MASK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LAG_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_lag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'lag_ndx' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'sys_port' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_MASKS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_MASKS_OUT_OF_RANGE_ERR",
    "The parameter 'masks' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_HASH_MASKS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_SEED_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_SEED_OUT_OF_RANGE_ERR",
    "The parameter 'seed' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR",
    "The parameter 'hash_func_id' is out of range. \n\r "
    "The range is: 3 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR",
    "The parameter 'key_shift' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_headers' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR",
    "The parameter 'first_header_to_parse' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_LAG_HASH_FRST_HDRS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'mask' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_HASH_MASKSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  {
    ARAD_PP_LAG_MEMBER_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_MEMBER_ID_OUT_OF_RANGE_ERR",
    "The parameter 'member_id' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - 16.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LAG_LB_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LAG_LB_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'lb_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_LAG_LB_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
   {
    ARAD_PP_LAG_ASYMETRIC_ERR,
    "ARAD_PP_LAG_ASYMETRIC_ERR",
    "When retrieving the LAG configuration, \n\r "
    "The incoming and the outgoing direction\n\r "
    "are configured differently",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   {
    ARAD_PP_LAG_DOUPLICATE_MEMBER_ERR,
    "ARAD_PP_LAG_DOUPLICATE_MEMBER_ERR",
    "define lag with duplicated member",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  /*
   * Last element. Do no touch.
   */
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* key = {In-Port.Lb-Profile, Parser-Leaf-Context.LB-Profile, Packet-Format-Code (6b)}
   assuming In-Port.Lb-Profile = 0 */
#define ARAD_PP_LB_PFC_OFFSET(_pfc_, _plc_) (_plc_ << 6 | _pfc_)

#define ARAD_PP_PLC_FC_VFT 1
#define ARAD_PP_PLC_FC_NO_VFT 0


#define ARAD_PP_PFC_FC_ENCAP        ARAD_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH
#define ARAD_PP_PFC_FC_NO_ENCAP     ARAD_PARSER_PKT_HDR_STK_TYPE_FC_STD_ETH



CONST STATIC ARAD_PP_LB_HASH_MAP
  Arad_pp_lb_hash_map_table[] =
{
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_CALC,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_ETHERNET,      ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_CALC,     ARAD_PP_LB_VECTOR_INDEX_MPLSX1,        ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_MPLSX1,        ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_CALC,     ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_CALC,     ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_ETHERNET, ARAD_PP_LB_VECTOR_INDEX_MPLSX1,        ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_ETHERNET, ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_ETHERNET, ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},

  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX1,        ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},

  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX1,        ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},

  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV6,     ARAD_PP_LB_VECTOR_INDEX_MPLSX1,        ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV6,     ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV4,     ARAD_PP_LB_VECTOR_INDEX_IPV4,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,   ARAD_PP_LB_VECTOR_INDEX_IPV6,     ARAD_PP_LB_VECTOR_INDEX_IPV4,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,   ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_IPV4,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,   ARAD_PP_LB_VECTOR_INDEX_L4,     ARAD_PP_LB_VECTOR_INDEX_IPV4,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,       ARAD_PP_LB_VECTOR_INDEX_IPV6,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_L4,       ARAD_PP_LB_VECTOR_INDEX_IPV6,          ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_ETHERNET, ARAD_PP_LB_VECTOR_INDEX_TRILL,         ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH,0), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_ETHERNET, ARAD_PP_LB_VECTOR_INDEX_IPV4,         ARAD_PP_LB_VECTOR_INDEX_ETHERNET},
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH,1), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_IPV4, ARAD_PP_LB_VECTOR_INDEX_ETHERNET, ARAD_PP_LB_VECTOR_INDEX_IPV4,         ARAD_PP_LB_VECTOR_INDEX_ETHERNET},

  /* FCOE */
  /* Standard Header: removed as conflicted with Ethernet Vector */
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PP_PFC_FC_NO_ENCAP,ARAD_PP_PLC_FC_NO_VFT), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_NONE,          ARAD_PP_LB_VECTOR_INDEX_FC, ARAD_PP_LB_VECTOR_INDEX_NONE},
  /* VFT,Standard Header*/
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PP_PFC_FC_NO_ENCAP,ARAD_PP_PLC_FC_VFT), ARAD_PP_LB_VECTOR_INDEX_NONE,  ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_FC,          ARAD_PP_LB_VECTOR_INDEX_FC_VFT, ARAD_PP_LB_VECTOR_INDEX_NONE},
  /* Encapsulation,VFT,Standard Header */
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PP_PFC_FC_ENCAP,ARAD_PP_PLC_FC_VFT),  ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_FC,          ARAD_PP_LB_VECTOR_INDEX_FC_VFT, ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE},
  /* Encapsulation, Standard Header*/
  { ARAD_PP_LB_PFC_OFFSET(ARAD_PP_PFC_FC_ENCAP,ARAD_PP_PLC_FC_NO_VFT), ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE,     ARAD_PP_LB_VECTOR_INDEX_FC,         ARAD_PP_LB_VECTOR_INDEX_NONE, ARAD_PP_LB_VECTOR_INDEX_NONE},
};


CONST STATIC ARAD_PP_LB_HASH_FIELD_INFO Arad_pp_lag_hash_field_info []=
{
	/* Ethernet */
	/* MAC_SA*/
	{ARAD_PP_LB_VECTOR_INDEX_ETHERNET, 6 , {5,6,9,10,13,14}},
	/* MAC_DA*/
	{ARAD_PP_LB_VECTOR_INDEX_ETHERNET, 6 , {3,4,7,8,11,12}},
	/* VSI*/
	{ARAD_PP_LB_VECTOR_INDEX_ETHERNET, 2 , {0,1,0,0,0,0}},
	/* ETH_TYPE_CODE*/
	{ARAD_PP_LB_VECTOR_INDEX_ETHERNET, 1 , {2,0,0,0,0,0}},
	/* MPLS_LABEL_1*/
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX1, 5 , {0,1,2,3,4,0}},
	/* MPLS_LABEL_2*/
	{ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, 5 , {8,9,10,11,12,0}},
	/* MPLS_LABEL_3*/
	{ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, 5 , {16,17,18,19,20,0}},
	/* IPV4_SIP*/
	{ARAD_PP_LB_VECTOR_INDEX_IPV4, 4 , {12,13,14,15,0,0}},
	/* IPV4_DIP*/
	{ARAD_PP_LB_VECTOR_INDEX_IPV4, 4 , {16,17,18,19,0,0}},
	/* IPV4_PROTOCOL*/
	{ARAD_PP_LB_VECTOR_INDEX_IPV4, 1 , {9,0,0,0,0,0}},
	/* IPV6_SIP*/
	{ARAD_PP_LB_VECTOR_INDEX_IPV6, 6 , {0,1,4,5,8,9}},
	/* IPV6_DIP*/
	{ARAD_PP_LB_VECTOR_INDEX_IPV6, 6 , {2,3,6,7,10,11}},
	/* IPV6_PROTOCOL*/
	{ARAD_PP_LB_VECTOR_INDEX_IPV6, 1 , {18,0,0,0,0,0}},
	/* L4_SRC_PORT*/
	{ARAD_PP_LB_VECTOR_INDEX_L4, 2 , {0,1,0,0,0,0}},
	/* L4_DEST_PORT*/
	{ARAD_PP_LB_VECTOR_INDEX_L4, 2 , {2,3,0,0,0,0}},
    /* FC_DEST_ID*/
    {ARAD_PP_LB_VECTOR_INDEX_FC, 3 , {1,2,3,0,0,0}},
    /* FC_SRC_ID*/
    {ARAD_PP_LB_VECTOR_INDEX_FC, 3 , {5,6,7,0,0,0}},
    /* FC_SEQ_ID*/
    {ARAD_PP_LB_VECTOR_INDEX_FC, 1 , {12,0,0,0,0,0}},
    /* FC_ORG_EX_ID*/
    {ARAD_PP_LB_VECTOR_INDEX_FC, 2 , {16,17,0,0,0,0}},
    /* FC_RES_EX_ID*/
    {ARAD_PP_LB_VECTOR_INDEX_FC, 2 , {18,19,0,0,0,0}},
    /* VFI */
    {ARAD_PP_LB_VECTOR_INDEX_FC_VFT, 3 , {5,6,7,0,0,0}},
    /* TRILL_EG_NICK*/
    {ARAD_PP_LB_VECTOR_INDEX_TRILL, 2 , {1,2,0,0,0,0}},
};

CONST STATIC ARAD_PP_LB_HASH_CHUNK_SIZE Arad_pp_lb_key_chunk_size[] = 
{   
    {ARAD_PP_LB_VECTOR_INDEX_NONE         , 0},
    {ARAD_PP_LB_VECTOR_INDEX_ETHERNET     , 1},
    {ARAD_PP_LB_VECTOR_INDEX_FC           , 1},
    {ARAD_PP_LB_VECTOR_INDEX_IPV4         , 1},
    {ARAD_PP_LB_VECTOR_INDEX_IPV6         , 1},
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX1       , 0},
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, 0},
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL2, 0},
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, 0},
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL2, 0},
    {ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL3, 0},
    {ARAD_PP_LB_VECTOR_INDEX_TRILL        , 0},
    {ARAD_PP_LB_VECTOR_INDEX_L4           , 1},
    {ARAD_PP_LB_VECTOR_INDEX_FC_VFT       , 0},
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  arad_pp_lag_lb_vector_index_map(int unit, uint32 lb_vector_index_id)
{
    uint32 dev_lb_vector_index;

    switch (lb_vector_index_id) {
    case ARAD_PP_LB_VECTOR_INDEX_ETHERNET:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_ETHERNET;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_FC:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_FC;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_IPV4:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_IPV4;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_IPV6:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_IPV6;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_MPLSX1:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_MPLSX1;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_MPLSX2_LABEL1;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_MPLSX2_LABEL2:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_MPLSX2_LABEL2;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL1;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL2:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL2;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_MPLSX3_LABEL3:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_MPLSX3_LABEL3;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_TRILL:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_TRILL;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_L4:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_L4;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_FC_VFT:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_FC_VFT;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_CALC:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_CALC;
        break;
    case ARAD_PP_LB_VECTOR_INDEX_NONE:
    default:
        dev_lb_vector_index = SOC_DPP_LB_VECTOR_INDEX_NONE;
        break;
    }

    return dev_lb_vector_index;
}

uint32
  arad_pp_lag_hash_func_to_hw_val(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8         hash_func_id,
    SOC_SAND_OUT  uint32        *hw_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (hash_func_id)
  {
    case ARAD_PP_LAG_LB_KEY:
     *hw_val = 10;
    break;
    case ARAD_PP_LAG_LB_ROUND_ROBIN:
     *hw_val = 11;
    break;
    case ARAD_PP_LAG_LB_2_CLOCK:
     *hw_val = 12;
    break;
    case ARAD_PP_LAG_LB_CRC_0x10861:
    case SOC_PPC_LAG_LB_CRC_0x8003: /* Arad+ */
     *hw_val = 0;
    break;
    case ARAD_PP_LAG_LB_CRC_0x10285:
    case SOC_PPC_LAG_LB_CRC_0x8011: /* Arad+ */
     *hw_val = 1;
    break;
    case ARAD_PP_LAG_LB_CRC_0x101A1:
    case SOC_PPC_LAG_LB_CRC_0x8423: /* Arad+ */
     *hw_val = 2;
    break;
    case ARAD_PP_LAG_LB_CRC_0x12499:
    case SOC_PPC_LAG_LB_CRC_0x8101: /* Arad+ */
     *hw_val = 3;
    break;
    case ARAD_PP_LAG_LB_CRC_0x1F801:
    case SOC_PPC_LAG_LB_CRC_0x84a1: /* Arad+ */
     *hw_val = 4;
    break;
    case ARAD_PP_LAG_LB_CRC_0x172E1:
    case SOC_PPC_LAG_LB_CRC_0x9019: /* Arad+ */
     *hw_val = 5;
    break;
    case ARAD_PP_LAG_LB_CRC_0x1EB21:
     *hw_val = 6;
    break;
    case ARAD_PP_LAG_LB_CRC_0x13965:
     *hw_val = 7;
    break;
    case ARAD_PP_LAG_LB_CRC_0x1698D:
     *hw_val = 8;
    break;
    case ARAD_PP_LAG_LB_CRC_0x1105D:
     *hw_val = 9;
    break;

    case ARAD_PP_LAG_LB_CRC_0x14D:
    case ARAD_PP_LAG_LB_CRC_0x1C3:
    case ARAD_PP_LAG_LB_CRC_0x1CF:
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hash_func_to_hw_val()", 0, 0);
}

uint32
  arad_pp_lag_hash_func_from_hw_val(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        hw_val,
    SOC_SAND_OUT  uint8        *hash_func_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (hw_val)
  {
    case 10:
     *hash_func_id = ARAD_PP_LAG_LB_KEY;
    break;
    case 11:
     *hash_func_id = ARAD_PP_LAG_LB_ROUND_ROBIN;
    break;
    case 12:
     *hash_func_id = ARAD_PP_LAG_LB_2_CLOCK;
    break;
    case 0:
#ifdef BCM_88660_A0
        /* Supported polynom after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x8003;
        }
        else
#endif /* BCM_88660_A0 */
        {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x10861;
        }
    break;
    case 1:
#ifdef BCM_88660_A0
        /* Supported polynom after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x8011;
        }
        else
#endif /* BCM_88660_A0 */
        {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x10285;
        }
    break;
    case 2:
#ifdef BCM_88660_A0
        /* Supported polynom after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x8423;
        }
        else
#endif /* BCM_88660_A0 */
        {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x101A1;
        }
    break;
    case 3:
#ifdef BCM_88660_A0
        /* Supported polynom after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x8101;
        }
        else
#endif /* BCM_88660_A0 */
        {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x12499;
        }
    break;
    case 4:
#ifdef BCM_88660_A0
        /* Supported polynom after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x84a1;
        }
        else
#endif /* BCM_88660_A0 */
        {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x1F801;
        }
    break;
    case 5:
#ifdef BCM_88660_A0
        /* Supported polynom after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x9019;
        }
        else
#endif /* BCM_88660_A0 */
        {
            *hash_func_id = ARAD_PP_LAG_LB_CRC_0x172E1;
        }
    break;
    case 6:
     *hash_func_id = ARAD_PP_LAG_LB_CRC_0x1EB21;
    break;
    case 7:
     *hash_func_id = ARAD_PP_LAG_LB_CRC_0x13965;
    break;
    case 8:
     *hash_func_id = ARAD_PP_LAG_LB_CRC_0x1698D;
    break;
    case 9:
     *hash_func_id = ARAD_PP_LAG_LB_CRC_0x1105D;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hash_func_to_hw_val()", 0, 0);
}

uint32
  arad_pp_lag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    idx;
  ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA
    ihb_lb_pfc_profile_tbl;
  ARAD_PP_HASH_MASK_INFO
    hash_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_PCID_LITE_SKIP(unit);


  for (idx = 0; idx < sizeof(Arad_pp_lb_hash_map_table) / sizeof(ARAD_PP_LB_HASH_MAP); ++idx)
  {
      ihb_lb_pfc_profile_tbl.lb_vector_index_1   = arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lb_hash_map_table[idx].index1);
      ihb_lb_pfc_profile_tbl.lb_vector_index_2   = arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lb_hash_map_table[idx].index2);
      ihb_lb_pfc_profile_tbl.lb_vector_index_3   = arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lb_hash_map_table[idx].index3);
      ihb_lb_pfc_profile_tbl.lb_vector_index_4   = arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lb_hash_map_table[idx].index4);
      ihb_lb_pfc_profile_tbl.lb_vector_index_5   = arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lb_hash_map_table[idx].index5);

      res = arad_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
                unit,
                Arad_pp_lb_hash_map_table[idx].offset,
                &ihb_lb_pfc_profile_tbl
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
  }

  ARAD_PP_HASH_MASK_INFO_clear(&hash_info);
  hash_info.mask = 0;

  res = arad_pp_lag_hashing_mask_set_unsafe(
          unit,
          &hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
   
  /* set Hashing = no-hashing for stacking ports (profile 1) */
#ifdef BCM_88675_A0
  if(SOC_IS_JERICHO(unit)) {
    uint64 reg_64;
    uint64 field64_val;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR, READ_IHB_LAG_LB_KEY_CFGr(unit, SOC_CORE_ALL, &reg_64));
    COMPILER_64_SET(field64_val, 0x0, 0xa);
    ARAD_FLD_TO_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_HASH_INDEX_1f, field64_val , reg_64, 72, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  77,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_LAG_LB_KEY_CFGr(unit, SOC_CORE_ALL, reg_64));
  } else
#endif /* BCM_88675_A0 */
  if(SOC_IS_ARAD_B0_AND_ABOVE(unit)) {

    uint32 reg_val;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_REG_0090r(unit, &reg_val));

    ARAD_FLD_TO_REG(IHB_REG_0090r, ITEM_0_7f, 0xf0 , reg_val, 62, exit);
    ARAD_FLD_TO_REG(IHB_REG_0090r, ITEM_8_11f, 0xa, reg_val, 64, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  66,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_REG_0090r(unit, reg_val));
  }
  /* 1. ARAD_PMF_PGM_SEL_PARSER_PROFILE_FCOE_VFT : (0x20 - bit 5) - for FC plc 4 => VFT exist
     2. ARAD_PMF_PGM_SEL_PARSER_PROFILE_GAL_GaCH_BFD : (0x2000 - bit 13)
     3. ARAD_PMF_PGM_SEL_PARSER_PROFILE_PP_L4 : (0x4000 - bit 14)
   */
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHB_LB_KEY_PARSER_LEAF_CONTEXT_PROFILEr(unit, REG_PORT_ANY, 0x6020));
   
   /* ARAD_PMF_PGM_SEL_PARSER_PROFILE_BFD_SINGLE_HOP : (0x40 -bit 6) - bfd ipv4 single hop */
   if (((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) || (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable == SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM)) ){
  		SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHB_LB_KEY_PARSER_LEAF_CONTEXT_PROFILEr(unit, REG_PORT_ANY, 0x6460));
   }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_init_unsafe()", 0, 0);
}

STATIC void
  arad_pp_lag_members_ppd2tm(
    SOC_SAND_IN   ARAD_PP_LAG_INFO        *soc_ppd_lag,
    SOC_SAND_OUT  ARAD_PORTS_LAG_INFO  *tm_lag
  )
{
  uint32
    entry_idx;

  tm_lag->soc_sand_magic_num = SOC_SAND_MAGIC_NUM_VAL;
  tm_lag->nof_entries = soc_ppd_lag->nof_entries;

  for (entry_idx = 0; entry_idx < soc_ppd_lag->nof_entries; entry_idx++)
  {
    tm_lag->members[entry_idx].member_id = soc_ppd_lag->members[entry_idx].member_id;
    tm_lag->members[entry_idx].sys_port  = soc_ppd_lag->members[entry_idx].sys_port;
    tm_lag->members[entry_idx].flags     = soc_ppd_lag->members[entry_idx].flags;
  }
}

STATIC void
  arad_pp_lag_members_tm2ppd(
    SOC_SAND_IN  ARAD_PORTS_LAG_INFO  *tm_lag,
    SOC_SAND_OUT ARAD_PP_LAG_INFO        *soc_ppd_lag
  )
{
  uint32
    entry_idx;

  soc_ppd_lag->nof_entries = tm_lag->nof_entries;

  for (entry_idx = 0; entry_idx < tm_lag->nof_entries; entry_idx++)
  {
    soc_ppd_lag->members[entry_idx].member_id = tm_lag->members[entry_idx].member_id;
    soc_ppd_lag->members[entry_idx].sys_port = tm_lag->members[entry_idx].sys_port;
  }
}


 /*********************************************************************
 *     Configure a LAG. A LAG is defined by a group of System
 *     Physical Ports that compose it. This configuration
 *     affects 1. LAG resolution: when the destination of
 *     packet is LAG 2. Learning: when packet source port
 *     belongs to LAG, then the LAG is learnt.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  arad_pp_lag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                            *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    lag2lag_rng_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  /*
   * Set Mode (Hash / RR / Smooth Division)
   */
  res = arad_irr_lag_to_lag_range_tbl_get_unsafe(unit, lag_ndx, &lag2lag_rng_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* 
   * 0x0 - Multiplication mode.
   * 0x1 - Round-Robin mode.
   * 0x2 - Smooth division mode: Jericho only. 
   */
  switch (lag_info->lb_type) {
  case ARAD_PP_LAG_LB_TYPE_HASH:
      lag2lag_rng_tbl.mode = 0x0;
      break;
  case ARAD_PP_LAG_LB_TYPE_ROUND_ROBIN:
      lag2lag_rng_tbl.mode = 0x1;
      break;
  case ARAD_PP_LAG_LB_TYPE_SMOOTH_DIVISION:
      lag2lag_rng_tbl.mode = 0x2;
      break;
  default:
      lag2lag_rng_tbl.mode = 0x3; /* invalid */
      break;
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    lag2lag_rng_tbl.is_stateful = lag_info->is_stateful;
  }
#endif /* BCM_88660_A0 */

  res = arad_irr_lag_to_lag_range_tbl_set_unsafe(unit, lag_ndx, &lag2lag_rng_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /************************************************************************/
  /* Set Members                                                          */
  /************************************************************************/
  res = arad_ports_lag_set_unsafe(unit, lag_ndx, lag_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_set_unsafe()", lag_ndx, 0);
}

uint32
  arad_pp_lag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                          *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, ARAD_PP_LAG_LAG_NDX_MAX(unit), ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  res = ARAD_PP_LAG_INFO_verify(unit,lag_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_set_verify()", lag_ndx, 0);
}

uint32
  arad_pp_lag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, ARAD_PP_LAG_LAG_NDX_MAX(unit), ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_get_verify()", lag_ndx, 0);
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
 *     Physical Ports that compose it. This configuration
 *     affects 1. LAG resolution: when the destination of
 *     packet is LAG 2. Learning: when packet source port
 *     belongs to LAG, then the LAG is learnt.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint8
  arad_pp_lag_is_symetric(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      lag_ndx,
    SOC_SAND_IN ARAD_PORTS_LAG_INFO           *in_lag_info,
    SOC_SAND_IN ARAD_PORTS_LAG_INFO           *out_lag_info
  )
{
  uint32
    indx,
    nof_entries;
  ARAD_PORT_LAG_SYS_PORT_INFO
    port_lag_info;

  if (out_lag_info->nof_entries != in_lag_info->nof_entries)
  {
    return FALSE;
  }

  nof_entries = out_lag_info->nof_entries;
  for (indx = 0; indx < nof_entries; ++indx)
  {
    arad_ports_lag_sys_port_info_get_unsafe(
      unit,
      out_lag_info->members[indx].sys_port,
      &port_lag_info
    );

    if (port_lag_info.in_lag != lag_ndx || port_lag_info.in_member != out_lag_info->members[indx].member_id)
    {
      return FALSE;
    }
  }

  return TRUE;
}


uint32
  arad_pp_lag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_OUT ARAD_PP_LAG_INFO                            *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PORTS_LAG_INFO
    *tm_lag_info_in = NULL,
    *tm_lag_info_out = NULL;
  uint8
    is_same_in_out = TRUE;
  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    lag2lag_rng_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  ARAD_PP_LAG_INFO_clear(lag_info);

  /*
   * Get Mode (Hash/RR/Smooth Division)
   */
  res = arad_irr_lag_to_lag_range_tbl_get_unsafe(
          unit,
          lag_ndx,
          &lag2lag_rng_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  switch (lag2lag_rng_tbl.mode) {
  case 0:
      lag_info->lb_type = ARAD_PP_LAG_LB_TYPE_HASH;
      break;
  case 1:
      lag_info->lb_type = ARAD_PP_LAG_LB_TYPE_ROUND_ROBIN;
      break;
  case 2:
      lag_info->lb_type = ARAD_PP_LAG_LB_TYPE_SMOOTH_DIVISION;
      break;
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    lag_info->is_stateful = lag2lag_rng_tbl.is_stateful;
  }
#endif /* BCM_88660_A0 */

  /************************************************************************/
  /* Get Members                                                          */
  /************************************************************************/

  ARAD_ALLOC(tm_lag_info_in, ARAD_PORTS_LAG_INFO, 1, "arad_pp_lag_get_unsafe.tm_lag_info_in");
  ARAD_ALLOC(tm_lag_info_out, ARAD_PORTS_LAG_INFO, 1, "arad_pp_lag_get_unsafe.tm_lag_info_out");

  res = arad_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          tm_lag_info_in,
          tm_lag_info_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#if ARAD_PP_LAG_SYMMETRIC_VERIFY_EN
  is_same_in_out = arad_pp_lag_is_symetric(unit, lag_ndx, tm_lag_info_in, tm_lag_info_out);
#endif

  if (!is_same_in_out)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LAG_ASYMETRIC_ERR, 12, exit);
  }

  arad_pp_lag_members_tm2ppd(tm_lag_info_out, lag_info);
  
exit:
  ARAD_FREE(tm_lag_info_in);
  ARAD_FREE(tm_lag_info_out);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_get_unsafe()", lag_ndx, 0);
}

/*********************************************************************
*     Add a system port as a member in LAG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_member_add_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                             lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                 *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PORTS_LAG_MEMBER
    lag_member;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_MEMBER_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(member);
  SOC_SAND_CHECK_NULL_INPUT(success);

  arad_ARAD_PORTS_LAG_MEMBER_clear(&lag_member);
  lag_member.member_id = member->member_id;
  lag_member.sys_port = member->sys_port;
  lag_member.flags    = member->flags;

  res = arad_ports_lag_member_add_unsafe( unit, lag_ndx, &lag_member );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *success = SOC_SAND_SUCCESS;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_member_add_unsafe()", lag_ndx, 0);
}

uint32
  arad_pp_lag_member_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                          *member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_MEMBER_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, ARAD_PP_LAG_LAG_NDX_MAX(unit), ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_LAG_MEMBER, member, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_member_add_verify()", lag_ndx, 0);
}

/*********************************************************************
*     Remove a system port from a LAG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_member_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                     *member
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PORTS_LAG_MEMBER
    lag_member;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_MEMBER_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(member);

  arad_ARAD_PORTS_LAG_MEMBER_clear(&lag_member);
  lag_member.member_id = member->member_id;
  lag_member.sys_port = member->sys_port;
  lag_member.flags    = member->flags;

  res = arad_ports_lag_sys_port_remove_unsafe( unit, lag_ndx, &lag_member );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_member_remove_unsafe()", lag_ndx, 0);
}

uint32
  arad_pp_lag_member_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                     *member
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_MEMBER_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, ARAD_PP_LAG_LAG_NDX_MAX(unit), ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(member->sys_port, ARAD_PP_LAG_SYS_PORT_MAX, ARAD_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_LAG_MEMBER, member, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_member_remove_verify()", lag_ndx, 0);
}

/*********************************************************************
*     Set the LAG hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  )
{
  uint64
    reg_val;
  uint64
      fld64_val;
  uint32
    fld_val;
  uint32
      res = SOC_SAND_OK;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_LAG_LB_KEY_CFGr(unit, SOC_CORE_ALL, &reg_val));

  COMPILER_64_SET(fld64_val, 0, glbl_hash_info->seed);  
  ARAD_FLD_TO_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_KEY_SEEDf, fld64_val, reg_val, 10, exit);

  COMPILER_64_SET(fld64_val, 0, SOC_SAND_BOOL2NUM(glbl_hash_info->use_port_id));  
  ARAD_FLD_TO_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_KEY_USE_IN_PORTf, fld64_val, reg_val, 14, exit);

  res = arad_pp_lag_hash_func_to_hw_val(unit,glbl_hash_info->hash_func_id,&fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  COMPILER_64_SET(fld64_val, 0, fld_val);  
  ARAD_FLD_TO_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_HASH_INDEXf, fld64_val, reg_val, 16, exit);

  COMPILER_64_SET(fld64_val, 0, glbl_hash_info->key_shift);  
  ARAD_FLD_TO_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_KEY_SHIFTf, fld64_val, reg_val, 18, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_LAG_LB_KEY_CFGr(unit, SOC_CORE_ALL,  reg_val));

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_global_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_lag_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_VERIFY);

  res = ARAD_PP_LAG_HASH_GLOBAL_INFO_verify(unit, glbl_hash_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_global_info_set_verify()", 0, 0);
}

uint32
  arad_pp_lag_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_global_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the LAG hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  )
{
  uint64
    reg_val;
  uint64
    fld_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);
  ARAD_PP_LAG_HASH_GLOBAL_INFO_clear(glbl_hash_info);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_LAG_LB_KEY_CFGr(unit, SOC_CORE_ALL, &reg_val));

  ARAD_FLD_FROM_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_KEY_SEEDf, fld_val, reg_val, 10, exit);
  glbl_hash_info->seed = (uint8)COMPILER_64_LO(fld_val);

  ARAD_FLD_FROM_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_KEY_USE_IN_PORTf, fld_val, reg_val, 14, exit);
  glbl_hash_info->use_port_id = SOC_SAND_NUM2BOOL(COMPILER_64_LO(fld_val));

  ARAD_FLD_FROM_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_HASH_INDEXf, fld_val, reg_val, 16, exit);

  res = arad_pp_lag_hash_func_from_hw_val(unit,COMPILER_64_LO(fld_val), &glbl_hash_info->hash_func_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
   

  ARAD_FLD_FROM_REG64(IHB_LAG_LB_KEY_CFGr, LAG_LB_KEY_SHIFTf, fld_val, reg_val, 18, exit);
  glbl_hash_info->key_shift = (uint8)COMPILER_64_LO(fld_val);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_global_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the LAG hashing per-lag attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_PINFO_FER_TBL_DATA
    pinfo_fer_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_hash_info);

  res = arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &pinfo_fer_tbl
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  pinfo_fer_tbl.lag_lb_key_start = (lag_hash_info->first_header_to_parse == ARAD_PP_LAG_HASH_FRST_HDR_FARWARDING)?0x1:0x0;
  pinfo_fer_tbl.lag_lb_key_count = lag_hash_info->nof_headers;
  pinfo_fer_tbl.lb_bos_search = SOC_SAND_BOOL2NUM(lag_hash_info->start_from_bos);
  pinfo_fer_tbl.lb_include_bos_hdr = SOC_SAND_BOOL2NUM(lag_hash_info->include_bos);

  res = arad_pp_ihb_pinfo_fer_tbl_set_unsafe(
          unit,
		  core_id,
          port_ndx,
          &pinfo_fer_tbl
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_port_info_set_unsafe()", port_ndx, 0);
}

uint32
  arad_pp_lag_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_LAG_HASH_PORT_INFO, lag_hash_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_port_info_set_verify()", port_ndx, 0);
}

uint32
  arad_pp_lag_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_port_info_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set the LAG hashing per-lag attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_OUT ARAD_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_PINFO_FER_TBL_DATA
    pinfo_fer_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_hash_info);

  ARAD_PP_LAG_HASH_PORT_INFO_clear(lag_hash_info);

  res = arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &pinfo_fer_tbl
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  lag_hash_info->first_header_to_parse = (pinfo_fer_tbl.lag_lb_key_start == 0x1)?ARAD_PP_LAG_HASH_FRST_HDR_FARWARDING:ARAD_PP_LAG_HASH_FRST_HDR_LAST_TERMINATED;
  lag_hash_info->nof_headers = (uint8)pinfo_fer_tbl.lag_lb_key_count;
  lag_hash_info->start_from_bos = SOC_SAND_NUM2BOOL(pinfo_fer_tbl.lb_bos_search);
  lag_hash_info->include_bos = SOC_SAND_NUM2BOOL(pinfo_fer_tbl.lb_include_bos_hdr);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_port_info_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Set the port lb_profile.
*********************************************************************/

int
  arad_pp_lag_hashing_port_lb_profile_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         pp_port,
    SOC_SAND_IN  uint32                         lb_profile
  )
{
    ARAD_PP_IHB_PINFO_FER_TBL_DATA pinfo_fer_tbl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
		  core,
          pp_port,
          &pinfo_fer_tbl
        ));

    pinfo_fer_tbl.lb_profile = lb_profile;

    SOCDNX_IF_ERR_EXIT(arad_pp_ihb_pinfo_fer_tbl_set_unsafe(
          unit,
          core,
          pp_port,
          &pinfo_fer_tbl
        ));

exit:
    SOCDNX_FUNC_RETURN;
}



/*********************************************************************
*     Mask / unmask fields from the packet header. Masked
 *     fields are ignored by the hashing function
 *     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC ARAD_PP_HASH_MASKS
  arad_pp_lag_hash_sym_peer(
    SOC_SAND_IN  ARAD_PP_HASH_MASKS            field
  )
{
	switch (field)
	{
  /* first part of symmetric, XOR + 2*/
	case ARAD_PP_HASH_MASKS_MAC_DA:
    return ARAD_PP_HASH_MASKS_MAC_SA;
	case ARAD_PP_HASH_MASKS_IPV6_SIP:
    return ARAD_PP_HASH_MASKS_IPV6_DIP;
  case ARAD_PP_HASH_MASKS_FC_ORG_EX_ID:
		return ARAD_PP_HASH_MASKS_FC_RES_EX_ID;
  /* first part of symmetric, XOR + 4*/
	case ARAD_PP_HASH_MASKS_IPV4_SIP:
    return ARAD_PP_HASH_MASKS_IPV4_DIP;
	case ARAD_PP_HASH_MASKS_FC_DEST_ID:
		return ARAD_PP_HASH_MASKS_FC_SRC_ID;
  case ARAD_PP_HASH_MASKS_L4_SRC_PORT:
    return ARAD_PP_HASH_MASKS_L4_DEST_PORT;
  /* first part of symmetric, XOR + 2*/
	case ARAD_PP_HASH_MASKS_MAC_SA:
    return ARAD_PP_HASH_MASKS_MAC_DA;
	case ARAD_PP_HASH_MASKS_IPV6_DIP:
    return ARAD_PP_HASH_MASKS_IPV6_SIP;
  case ARAD_PP_HASH_MASKS_FC_RES_EX_ID:
		return ARAD_PP_HASH_MASKS_FC_ORG_EX_ID;
  /* first part of symmetric, XOR + 4*/
	case ARAD_PP_HASH_MASKS_IPV4_DIP:
    return ARAD_PP_HASH_MASKS_IPV4_SIP;
	case ARAD_PP_HASH_MASKS_FC_SRC_ID:
		return ARAD_PP_HASH_MASKS_FC_DEST_ID;
  case ARAD_PP_HASH_MASKS_L4_DEST_PORT:
    return ARAD_PP_HASH_MASKS_L4_SRC_PORT;
  /* not part of symmetric: just take as is*/
	case ARAD_PP_HASH_MASKS_ETH_TYPE_CODE:
	case ARAD_PP_HASH_MASKS_VSI:
	case ARAD_PP_HASH_MASKS_MPLS_LABEL_1:
	case ARAD_PP_HASH_MASKS_MPLS_LABEL_2:
	case ARAD_PP_HASH_MASKS_MPLS_LABEL_3:
	case ARAD_PP_HASH_MASKS_IPV4_PROTOCOL:
	case ARAD_PP_HASH_MASKS_FC_SEQ_ID:
	default:
		return field;
	}

}


/*********************************************************************
*     Mask / unmask fields from the packet header. Masked
 *     fields are ignored by the hashing function
 *     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  arad_pp_lag_calc_nible_val(
    SOC_SAND_IN  ARAD_PP_HASH_MASKS            field,
    SOC_SAND_IN  uint8                   enable,
    SOC_SAND_IN  uint8                   peer_enable, /* second part for symmertic */
    SOC_SAND_IN  uint8                   is_symmetric
  )
{
	/* ignored */
  if (!enable)
	{
		return 0;
	}
  /* enabled and not symmetric */
	if (!is_symmetric)
	{
		return 1;
	}
  /* symmetric & enabled but peer is not enable */
  if (!peer_enable)
  {
    return 1; /* just enabled */
  }
	switch (field)/* set symmetric configuration */
	{
  /* first part of symmetric, XOR + 2*/
	case ARAD_PP_HASH_MASKS_MAC_DA:
	case ARAD_PP_HASH_MASKS_IPV6_SIP:
	case ARAD_PP_HASH_MASKS_L4_SRC_PORT:
	case ARAD_PP_HASH_MASKS_FC_ORG_EX_ID:
		return 2;
  /* first part of symmetric, XOR + 4*/
	case ARAD_PP_HASH_MASKS_IPV4_SIP:
	case ARAD_PP_HASH_MASKS_FC_DEST_ID:
		return 3;
  /* second part of symmetric, ignore*/
  case ARAD_PP_HASH_MASKS_MAC_SA:
  case ARAD_PP_HASH_MASKS_IPV4_DIP:
  case ARAD_PP_HASH_MASKS_IPV6_DIP:
  case ARAD_PP_HASH_MASKS_L4_DEST_PORT:
  case ARAD_PP_HASH_MASKS_FC_SRC_ID:
  case ARAD_PP_HASH_MASKS_FC_RES_EX_ID:
    return 0;
  /* not part of symmetric: just take as is*/
	case ARAD_PP_HASH_MASKS_ETH_TYPE_CODE:
	case ARAD_PP_HASH_MASKS_VSI:
	case ARAD_PP_HASH_MASKS_MPLS_LABEL_1:
	case ARAD_PP_HASH_MASKS_MPLS_LABEL_2:
	case ARAD_PP_HASH_MASKS_MPLS_LABEL_3:
	case ARAD_PP_HASH_MASKS_IPV4_PROTOCOL:
	case ARAD_PP_HASH_MASKS_FC_SEQ_ID:
	default:
		return 1;
	}

}


uint32
  arad_pp_lag_hashing_mask_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO  *mask_info
  )
{
  uint32
   mask_idx,
   map_index,
   vc_indx,
   fld_val,
   field_nbl_indx,
   field_nof_nbls,
   field_info_offset,
   is_equal,
   nible_val;
  ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA
    lb_vector_program_map_tbl_data,
    old_val;
  uint8
    is_symmetric = FALSE,
    skip;
  ARAD_PP_HASH_MASKS
    masks,
    fld,
    peer_fld;
  uint32
    chunk_bitmap[2] = {0};
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_MASK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mask_info);

  masks = mask_info->mask;

  is_symmetric = mask_info->is_symmetric_key;

  for (vc_indx = 0; vc_indx < sizeof(Arad_pp_lb_key_chunk_size) / sizeof(ARAD_PP_LB_HASH_CHUNK_SIZE); ++vc_indx)
  {
    map_index = arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lb_key_chunk_size[vc_indx].key_index);

    /*
     * update Ethernet key
     */
    res = arad_pp_ihb_lb_vector_program_map_tbl_get_unsafe(
            unit,
            map_index,
            &lb_vector_program_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

    ARAD_COPY(&old_val,&lb_vector_program_map_tbl_data,ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA,1);

    chunk_bitmap[0] = 0;
    chunk_bitmap[1] = 0;

    lb_vector_program_map_tbl_data.chunk_size = Arad_pp_lb_key_chunk_size[vc_indx].chunk_size;

    for (mask_idx = 0; mask_idx < ARAD_PP_NOF_HASH_MASKS; ++mask_idx)
    {
      skip = TRUE;
      field_info_offset = mask_idx; /*ARAD_PP_LB_FIELD_TO_OFFSET(SOC_SAND_BIT(mask_idx));*/

      if(arad_pp_lag_lb_vector_index_map(unit, Arad_pp_lag_hash_field_info[field_info_offset].key_index) == map_index)
      {
        skip = FALSE;
      }
      if ((SOC_SAND_BIT(mask_idx) == ARAD_PP_HASH_MASKS_MPLS_LABEL_1) && (ARAD_PP_LB_MPLS_LBL1_VLD(map_index)))
      {
        skip = FALSE;
      }
      else if ((SOC_SAND_BIT(mask_idx) == ARAD_PP_HASH_MASKS_MPLS_LABEL_2) && (ARAD_PP_LB_MPLS_LBL2_VLD(map_index)))
      {
        skip = FALSE;
      }
      else if ((SOC_SAND_BIT(mask_idx) == ARAD_PP_HASH_MASKS_MPLS_LABEL_3) && (ARAD_PP_LB_MPLS_LBL3_VLD(map_index)))
      {
        skip = FALSE;
      }

      if (skip)
      {
        continue;
      }

      fld = SOC_SAND_BIT(mask_idx);
      peer_fld = arad_pp_lag_hash_sym_peer(fld);
      /* if (masks & fld) -> enable = 0 */
	  nible_val = arad_pp_lag_calc_nible_val(fld, (uint8)((masks & fld)== 0), (uint8)((masks & peer_fld)== 0), is_symmetric);
	  field_nof_nbls = Arad_pp_lag_hash_field_info[field_info_offset].nof_nibles;

	  for (field_nbl_indx = 0 ; field_nbl_indx < field_nof_nbls; ++field_nbl_indx)
	  {
	      res = soc_sand_bitstream_set_any_field(
	              &nible_val,
	   	          ARAD_PP_LB_FIELD_TO_NIBLE(Arad_pp_lag_hash_field_info[field_info_offset].nbls[field_nbl_indx],lb_vector_program_map_tbl_data.chunk_size),
		            2/*(lb_vector_program_map_tbl_data.chunk_size)?8:4*/,
		            chunk_bitmap
    	        );
	      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
      }
    }

    COMPILER_64_SET(lb_vector_program_map_tbl_data.chunk_bitmap, chunk_bitmap[1], chunk_bitmap[0]);
    ARAD_COMP(&old_val,&lb_vector_program_map_tbl_data,ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA,1,is_equal);

    if (!is_equal)
    {
      res = arad_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
                unit,
                map_index,
                &lb_vector_program_map_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
    }
  }
 /*
  * set CW
  */
  fld_val = SOC_SAND_BOOL2NUM(mask_info->expect_cw);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  112,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LAG_LB_KEY_CFGr, SOC_CORE_ALL, 0, LB_MPLS_CONTROL_WORDf,  fld_val));

  /* store key in SW DB */
  res = sw_state_access[unit].dpp.soc.arad.pp.lag.masks.set(unit, masks);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);
  res = sw_state_access[unit].dpp.soc.arad.pp.lag.lb_key_is_symtrc.set(unit, is_symmetric);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_mask_set_unsafe()", 0, 0);
}

uint32
  arad_pp_lag_hashing_mask_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO       *mask_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_MASK_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_HASH_MASK_INFO, mask_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_mask_set_verify()", 0, 0);
}

uint32
  arad_pp_lag_hashing_mask_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_MASK_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_mask_get_verify()", 0, 0);
}

/*********************************************************************
*     Mask / unmask fields from the packet header. Masked
 *     fields are ignored by the hashing function
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_HASH_MASK_INFO       *mask_info
  )
{
  uint32
    res,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_MASK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mask_info);

  ARAD_PP_HASH_MASK_INFO_clear(mask_info);
  
  res = sw_state_access[unit].dpp.soc.arad.pp.lag.masks.get(unit, &(mask_info->mask));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.lag.lb_key_is_symtrc.get(unit, &(mask_info->is_symmetric_key));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * get CW
  */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  112,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_LAG_LB_KEY_CFGr, SOC_CORE_ALL, 0, LB_MPLS_CONTROL_WORDf, &fld_val));
  mask_info->expect_cw = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_mask_get_unsafe()", 0, 0);
}

uint32
  arad_pp_lag_lb_key_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                            *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PORTS_LAG_INFO
    tm_lag_info;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  /************************************************************************/
  /* Set Members                                                          */
  /************************************************************************/
  arad_pp_lag_members_ppd2tm(lag_info, &tm_lag_info);

  res = arad_ports_lag_lb_key_range_set_unsafe(
          unit,
          &tm_lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_lb_key_range_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_lag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lag_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_lag;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_lag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lag_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_lag;
}

uint32
  ARAD_PP_LAG_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_ERR_IF_OUT_OF_RANGE(info->hash_func_id, ARAD_PP_LAG_HASH_FUNC_ID_MIN, ARAD_PP_LAG_HASH_FUNC_ID_MAX, ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 14, exit);
   SOC_SAND_ERR_IF_ABOVE_MAX(info->key_shift, ARAD_PP_LAG_KEY_SHIFT_MAX, ARAD_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR, 15, exit);
   SOC_SAND_ERR_IF_ABOVE_MAX(info->seed, ARAD_PP_LAG_SEED_MAX, ARAD_PP_LAG_SEED_OUT_OF_RANGE_ERR, 16, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LAG_HASH_GLOBAL_INFO_verify()",0,0);
}

uint32
  ARAD_PP_LAG_HASH_PORT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_headers, ARAD_PP_LAG_NOF_HEADERS_MAX, ARAD_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR, 10, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_header_to_parse, ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_MAX, ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LAG_HASH_PORT_INFO_verify()",0,0);
}

uint32
  ARAD_PP_HASH_MASK_INFO_verify(
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO *info
  )
{
  uint32
    invalid_mask;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  invalid_mask = SOC_SAND_BITS_MASK(31,ARAD_PP_NOF_HASH_MASKS+1);
  
  if ((invalid_mask & info->mask) != 0)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LAG_MASK_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_HASH_MASK_INFO_verify()",info->mask,0);
}

uint32
  ARAD_PP_LAG_MEMBER_verify(
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_port, ARAD_PP_LAG_SYS_PORT_MAX, ARAD_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->member_id, ARAD_PP_LAG_MEMBER_ID_MAX, ARAD_PP_LAG_MEMBER_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LAG_MEMBER_verify()",0,0);
}

uint32
  ARAD_PP_LAG_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  ARAD_PP_LAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind/*,ind2 */;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, ARAD_PP_LAG_NOF_ENTRIES_MAX(unit), ARAD_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < ARAD_PP_LAG_MEMBERS_MAX; ++ind)
  {
    ARAD_PP_STRUCT_VERIFY(ARAD_PP_LAG_MEMBER, &(info->members[ind]), 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lb_type, ARAD_PP_LAG_LB_TYPE_MAX, ARAD_PP_LAG_LB_TYPE_OUT_OF_RANGE_ERR, 12, exit);
/*
  for (ind = 0; ind < info->nof_entries; ++ind)
  {
    for (ind2 = 0; ind2 < info->nof_entries; ++ind2)
    {
      if (info->members[ind].sys_port == info->members[ind2].sys_port && (ind != ind2))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LAG_DOUPLICATE_MEMBER_ERR, 12, exit);
      }
    }
  }
*/
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LAG_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */


