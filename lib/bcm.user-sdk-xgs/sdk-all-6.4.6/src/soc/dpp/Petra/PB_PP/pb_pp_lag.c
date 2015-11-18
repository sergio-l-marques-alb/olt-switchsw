/* $Id: pb_pp_lag.c,v 1.11 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>


#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>

#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* $Id: pb_pp_lag.c,v 1.11 Broadcom SDK $
 *	If set, when the LAG information is retrieved, the API verifies that
 *  incoming and outgoing directions are configured identically
 */
#define SOC_PB_PP_LAG_SYMMETRIC_VERIFY_EN 0

#define SOC_PB_PP_LAG_LAG_NDX_MAX                                  (255)
#define SOC_PB_PP_LAG_SYS_PORT_MAX                                 (4095)
#define SOC_PB_PP_LAG_SEED_MAX                                     (SOC_SAND_U8_MAX)
#define SOC_PB_PP_LAG_HASH_FUNC_ID_MAX                             (5)
#define SOC_PB_PP_LAG_KEY_SHIFT_MAX                                (7)
#define SOC_PB_PP_LAG_NOF_HEADERS_MAX                              (SOC_SAND_U8_MAX)
#define SOC_PB_PP_LAG_FIRST_HEADER_TO_PARSE_MAX                    (SOC_PB_PP_NOF_LAG_HASH_FRST_HDRS-1)
#define SOC_PB_PP_LAG_MEMBER_ID_MAX                                (15)
#define SOC_PB_PP_LAG_NOF_ENTRIES_MAX                              (16)
#define SOC_PB_PP_LAG_LB_TYPE_MAX                                  (SOC_PB_PP_NOF_LAG_LB_TYPES-1)

/*
 * Defines for hash vectors
 */

#define SOC_PB_PP_LB_VECTOR_INDEX_NONE             0x0
#define SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET         0x1
#define SOC_PB_PP_LB_VECTOR_INDEX_FC               0x2
#define SOC_PB_PP_LB_VECTOR_INDEX_IPV4             0x3
#define SOC_PB_PP_LB_VECTOR_INDEX_IPV6             0x4
#define SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1           0x5
#define SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1    0x6
#define SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1    0x8
#define SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL2    0x9
#define SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL3    0xa
#define SOC_PB_PP_LB_VECTOR_INDEX_TRILL            0xb
#define SOC_PB_PP_LB_VECTOR_INDEX_L4               0xc
#define SOC_PB_PP_LB_VECTOR_INDEX_CALC             0xf
#define SOC_PB_PP_LB_NOF_VECTOR_INDEXES            0xf



#define SOC_PB_PP_LB_VECTOR_NOF_BITS 48

            
/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define SOC_PB_PP_LB_FIELD_TO_OFFSET(field) (soc_sand_log2_round_down(field))

/*
 * given chunk offset return the start place (lsb to msb) of the 2bits to control this chunk
 */
#define SOC_PB_PP_LB_FIELD_TO_NIBLE(chunk_ofst, chunk_size)   (SOC_PB_PP_LB_VECTOR_NOF_BITS - chunk_ofst*2 - 2)
#define SOC_PB_PP_LB_MPLS_LBL1_VLD(vc_indx) \
  ((vc_indx== SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1)||(vc_indx== SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1)||(vc_indx==SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1))

#define SOC_PB_PP_LB_MPLS_LBL2_VLD(vc_indx) \
  ((vc_indx >= SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1) && (vc_indx <= SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL2))

#define SOC_PB_PP_LB_MPLS_LBL3_VLD(vc_indx) \
  ((vc_indx >= SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1) && (vc_indx <= SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL3))

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
}SOC_PB_PP_LB_HASH_FIELD_INFO;

typedef struct
{
	uint32 offset;
	uint32 index5;
	uint32 index4;
	uint32 index3;
	uint32 index2;
	uint32 index1;
}SOC_PB_PP_LB_HASH_MAP;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_lag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_MEMBER_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_HASHING_MASK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LAG_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_lag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'lag_ndx' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'sys_port' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_MASKS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_MASKS_OUT_OF_RANGE_ERR",
    "The parameter 'masks' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_HASH_MASKS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_SEED_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_SEED_OUT_OF_RANGE_ERR",
    "The parameter 'seed' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR",
    "The parameter 'hash_func_id' is out of range. \n\r "
    "The range is: 0 - 5.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR",
    "The parameter 'key_shift' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_headers' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR",
    "The parameter 'first_header_to_parse' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LAG_HASH_FRST_HDRS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_MASK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'mask' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_HASH_MASKSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  {
    SOC_PB_PP_LAG_MEMBER_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_MEMBER_ID_OUT_OF_RANGE_ERR",
    "The parameter 'member_id' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - 16.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LAG_LB_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LAG_LB_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'lb_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LAG_LB_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
   {
    SOC_PB_PP_LAG_ASYMETRIC_ERR,
    "SOC_PB_PP_LAG_ASYMETRIC_ERR",
    "When retrieving the LAG configuration, \n\r "
    "The incoming and the outgoing direction\n\r "
    "are configured differently",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   {
    SOC_PB_PP_LAG_DOUPLICATE_MEMBER_ERR,
    "SOC_PB_PP_LAG_DOUPLICATE_MEMBER_ERR",
    "define lag with duplicated member",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static SOC_PB_PP_LB_HASH_MAP
  Soc_pb_pp_lb_hash_map_table[] =
{
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE,     SOC_PB_PP_LB_VECTOR_INDEX_CALC,          SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE,     SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET,      SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS1_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_CALC,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1,        SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS2_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_CALC,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS3_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_CALC,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1,        SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV4,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1,        SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV4,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV4,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV6,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1,        SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV6,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV4,     SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV4,     SOC_PB_PP_LB_VECTOR_INDEX_IPV4,          SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,   SOC_PB_PP_LB_VECTOR_INDEX_IPV6,     SOC_PB_PP_LB_VECTOR_INDEX_IPV4,          SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE,   SOC_PB_PP_LB_VECTOR_INDEX_L4,     SOC_PB_PP_LB_VECTOR_INDEX_IPV4,          SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_L4,       SOC_PB_PP_LB_VECTOR_INDEX_IPV6,          SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
  { SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_TRILL_ETH, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_NONE, SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, SOC_PB_PP_LB_VECTOR_INDEX_TRILL,         SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET},
};


SOC_PB_PP_LB_HASH_FIELD_INFO Soc_pb_pp_lag_hash_field_info []=
{
	/* Ethernet */
	/* MAC_SA*/
	{SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, 6 , {5,6,9,10,13,14}},
	/* MAC_DA*/
	{SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, 6 , {3,4,7,8,11,12}},
	/* VSI*/
	{SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, 2 , {0,1,0,0,0,0}},
	/* ETH_TYPE_CODE*/
	{SOC_PB_PP_LB_VECTOR_INDEX_ETHERNET, 1 , {2,0,0,0,0,0}},
	/* MPLS_LABEL_1*/
    {SOC_PB_PP_LB_VECTOR_INDEX_MPLSX1, 5 , {0,1,2,3,4,0}},
	/* MPLS_LABEL_2*/
	{SOC_PB_PP_LB_VECTOR_INDEX_MPLSX2_LABEL1, 5 , {8,9,10,11,12,0}},
	/* MPLS_LABEL_3*/
	{SOC_PB_PP_LB_VECTOR_INDEX_MPLSX3_LABEL1, 5 , {16,17,18,19,20,0}},
	/* IPV4_SIP*/
	{SOC_PB_PP_LB_VECTOR_INDEX_IPV4, 4 , {12,13,14,15,0,0}},
	/* IPV4_DIP*/
	{SOC_PB_PP_LB_VECTOR_INDEX_IPV4, 4 , {16,17,18,19,0,0}},
	/* IPV4_PROTOCOL*/
	{SOC_PB_PP_LB_VECTOR_INDEX_IPV4, 1 , {9,0,0,0,0,0}},
	/* IPV6_SIP*/
	{SOC_PB_PP_LB_VECTOR_INDEX_IPV6, 6 , {0,1,4,5,8,9}},
	/* IPV6_DIP*/
	{SOC_PB_PP_LB_VECTOR_INDEX_IPV6, 6 , {2,3,6,7,10,11}},
	/* IPV6_PROTOCOL*/
	{SOC_PB_PP_LB_VECTOR_INDEX_IPV6, 1 , {18,0,0,0,0,0}},
	/* L4_SRC_PORT*/
	{SOC_PB_PP_LB_VECTOR_INDEX_L4, 2 , {0,1,0,0,0,0}},
	/* L4_DEST_PORT*/
	{SOC_PB_PP_LB_VECTOR_INDEX_L4, 2 , {2,3,0,0,0,0}},
	/* FC_DEST_ID*/
	{SOC_PB_PP_LB_VECTOR_INDEX_FC, 2 , {5,6,0,0,0,0}},
	/* FC_SRC_ID*/
	{SOC_PB_PP_LB_VECTOR_INDEX_FC, 2 , {8,9,0,0,0,0}},
	/* FC_SEQ_ID*/
	{SOC_PB_PP_LB_VECTOR_INDEX_FC, 1 , {16,0,0,0,0,0}},
	/* FC_ORG_EX_ID*/
	{SOC_PB_PP_LB_VECTOR_INDEX_FC, 2 , {20,21,0,0,0,0}},
	/* FC_RES_EX_ID*/
	{SOC_PB_PP_LB_VECTOR_INDEX_FC, 2 , {22,23,0,0,0,0}},
    /* TRILL_EG_NICK*/
    {SOC_PB_PP_LB_VECTOR_INDEX_TRILL, 2 , {1,2,0,0,0,0}},

};

uint8 Soc_pb_pp_lb_key_chunk_size[SOC_PB_PP_LB_NOF_VECTOR_INDEXES] =
{0,1,1,1,1,0,0,0,0,0,0,0,1,0,0};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


uint32
  soc_pb_pp_lag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    idx;
  SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA
    ihb_lb_pfc_profile_tbl;
  SOC_PB_PP_HASH_MASK_INFO
    hash_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (idx = 0; idx < sizeof(Soc_pb_pp_lb_hash_map_table) / sizeof(SOC_PB_PP_LB_HASH_MAP); ++idx)
  {
	  ihb_lb_pfc_profile_tbl.lb_vector_index1   = Soc_pb_pp_lb_hash_map_table[idx].index1;
	  ihb_lb_pfc_profile_tbl.lb_vector_index2   = Soc_pb_pp_lb_hash_map_table[idx].index2;
	  ihb_lb_pfc_profile_tbl.lb_vector_index3   = Soc_pb_pp_lb_hash_map_table[idx].index3;
	  ihb_lb_pfc_profile_tbl.lb_vector_index4   = Soc_pb_pp_lb_hash_map_table[idx].index4;
	  ihb_lb_pfc_profile_tbl.lb_vector_index5   = Soc_pb_pp_lb_hash_map_table[idx].index5;

	  res = soc_pb_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
	   	      unit,
		      Soc_pb_pp_lb_hash_map_table[idx].offset,
		      &ihb_lb_pfc_profile_tbl
		    );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
  }

  SOC_PB_PP_HASH_MASK_INFO_clear(&hash_info);
  hash_info.mask = 0;

  res = soc_pb_pp_lag_hashing_mask_set_unsafe(
          unit,
          &hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_init_unsafe()", 0, 0);
}

STATIC void
  soc_pb_pp_lag_members_ppd2tm(
    SOC_SAND_IN   SOC_PB_PP_LAG_INFO        *soc_ppd_lag,
    SOC_SAND_OUT  SOC_PETRA_PORTS_LAG_INFO  *tm_lag
  )
{
  uint32
    entry_idx;

  tm_lag->soc_sand_magic_num = SOC_SAND_MAGIC_NUM_VAL;
  tm_lag->nof_entries = soc_ppd_lag->nof_entries;

  for (entry_idx = 0; entry_idx < soc_ppd_lag->nof_entries; entry_idx++)
  {
    tm_lag->lag_member_sys_ports[entry_idx].member_id = soc_ppd_lag->members[entry_idx].member_id;
    tm_lag->lag_member_sys_ports[entry_idx].sys_port  = soc_ppd_lag->members[entry_idx].sys_port;
  }
}

STATIC void
  soc_pb_pp_lag_members_tm2ppd(
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO  *tm_lag,
    SOC_SAND_OUT SOC_PB_PP_LAG_INFO        *soc_ppd_lag
  )
{
  uint32
    entry_idx;

  soc_ppd_lag->nof_entries = tm_lag->nof_entries;

  for (entry_idx = 0; entry_idx < tm_lag->nof_entries; entry_idx++)
  {
    soc_ppd_lag->members[entry_idx].member_id = tm_lag->lag_member_sys_ports[entry_idx].member_id;
    soc_ppd_lag->members[entry_idx].sys_port = tm_lag->lag_member_sys_ports[entry_idx].sys_port;
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
  soc_pb_pp_lag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_INFO                            *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_PORTS_LAG_INFO
    tm_lag_info;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    lag2lag_rng_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  /************************************************************************/
  /* Set Mode (Hush/RR)                                                   */
  /************************************************************************/
  res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
          unit,
          lag_ndx,
          &lag2lag_rng_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  lag2lag_rng_tbl.mode = (lag_info->lb_type == SOC_PB_PP_LAG_LB_TYPE_HASH)?0x0:0x1;

  res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
          unit,
          lag_ndx,
          &lag2lag_rng_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /************************************************************************/
  /* Set Members                                                          */
  /************************************************************************/
  soc_pb_pp_lag_members_ppd2tm(lag_info, &tm_lag_info);

  res = soc_petra_ports_lag_set_unsafe(
          unit,
          SOC_PETRA_PORT_DIRECTION_BOTH,
          lag_ndx,
          &tm_lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_set_unsafe()", lag_ndx, 0);
}

uint32
  soc_pb_pp_lag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_INFO                            *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, SOC_PB_PP_LAG_LAG_NDX_MAX, SOC_PB_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LAG_INFO, lag_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_set_verify()", lag_ndx, 0);
}

uint32
  soc_pb_pp_lag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, SOC_PB_PP_LAG_LAG_NDX_MAX, SOC_PB_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_get_verify()", lag_ndx, 0);
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
  soc_pb_pp_lag_is_symetric(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      lag_ndx,
    SOC_SAND_IN SOC_PETRA_PORTS_LAG_INFO           *in_lag_info,
    SOC_SAND_IN SOC_PETRA_PORTS_LAG_INFO           *out_lag_info
  )
{
  uint32
    indx,
    nof_entries;
  SOC_PETRA_PORT_LAG_SYS_PORT_INFO
    port_lag_info;

  if (out_lag_info->nof_entries != in_lag_info->nof_entries)
  {
    return FALSE;
  }

  nof_entries = out_lag_info->nof_entries;
  for (indx = 0; indx < nof_entries; ++indx)
  {
    soc_petra_ports_lag_sys_port_info_get_unsafe(
      unit,
      out_lag_info->lag_member_sys_ports[indx].sys_port,
      &port_lag_info
    );

    if (port_lag_info.in_lag != lag_ndx || port_lag_info.in_member != out_lag_info->lag_member_sys_ports[indx].member_id)
    {
      return FALSE;
    }
  }

  return TRUE;
}


uint32
  soc_pb_pp_lag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_OUT SOC_PB_PP_LAG_INFO                            *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_PORTS_LAG_INFO
    tm_lag_info_in,
    tm_lag_info_out;
  uint8
    is_same_in_out = TRUE;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    lag2lag_rng_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  SOC_PB_PP_LAG_INFO_clear(lag_info);

  /************************************************************************/
  /* Get Mode (Hush/RR)                                                   */
  /************************************************************************/
  res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
          unit,
          lag_ndx,
          &lag2lag_rng_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  lag_info->lb_type = (lag2lag_rng_tbl.mode == 0x0)?SOC_PB_PP_LAG_LB_TYPE_HASH:SOC_PB_PP_LAG_LB_TYPE_ROUND_ROBIN;

  /************************************************************************/
  /* Get Members                                                          */
  /************************************************************************/

  res = soc_petra_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          &tm_lag_info_in,
          &tm_lag_info_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#if SOC_PB_PP_LAG_SYMMETRIC_VERIFY_EN
  is_same_in_out = soc_pb_pp_lag_is_symetric(unit, lag_ndx, &tm_lag_info_in, &tm_lag_info_out);
#endif

  if (!is_same_in_out)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LAG_ASYMETRIC_ERR, 12, exit);
  }

  soc_pb_pp_lag_members_tm2ppd(&tm_lag_info_out, lag_info);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_get_unsafe()", lag_ndx, 0);
}

/*********************************************************************
*     Add a system port as a member in LAG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_MEMBER                          *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_PORTS_LAG_MEMBER
    lag_member;
  uint8
    is_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_MEMBER_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(member);
  SOC_SAND_CHECK_NULL_INPUT(success);

  soc_petra_PETRA_PORTS_LAG_MEMBER_clear(&lag_member);
  lag_member.member_id = member->member_id;
  lag_member.sys_port = member->sys_port;  

  res = soc_petra_ports_lag_member_add_unsafe(
          unit,
          SOC_PETRA_PORT_DIRECTION_BOTH,
          lag_ndx,
          &lag_member,
          &is_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *success = (is_success)?SOC_SAND_SUCCESS:SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_member_add_unsafe()", lag_ndx, 0);
}

uint32
  soc_pb_pp_lag_member_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_MEMBER                          *member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_MEMBER_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, SOC_PB_PP_LAG_LAG_NDX_MAX, SOC_PB_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LAG_MEMBER, member, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_member_add_verify()", lag_ndx, 0);
}

/*********************************************************************
*     Remove a system port from a LAG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_member_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_MEMBER                          *member
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_PORTS_LAG_MEMBER
    lag_member;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_MEMBER_REMOVE_UNSAFE);

  soc_petra_PETRA_PORTS_LAG_MEMBER_clear(&lag_member);
  lag_member.member_id = member->member_id;
  lag_member.sys_port = member->sys_port; 

  res = soc_petra_ports_lag_sys_port_remove_unsafe(
          unit,
          SOC_PETRA_PORT_DIRECTION_BOTH,
          lag_ndx,
          &lag_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_member_remove_unsafe()", lag_ndx, 0);
}

uint32
  soc_pb_pp_lag_member_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_MEMBER                          *member
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_MEMBER_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LAG_MEMBER, member, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(lag_ndx, SOC_PB_PP_LAG_LAG_NDX_MAX, SOC_PB_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(member->sys_port, SOC_PB_PP_LAG_SYS_PORT_MAX, SOC_PB_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_member_remove_verify()", lag_ndx, 0);
}

/*********************************************************************
*     Set the LAG hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->ihb.lag_lb_key_cfg_reg, reg_val, 8, exit);

  fld_val = glbl_hash_info->seed;
  SOC_PB_FLD_TO_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_key_seed, fld_val, reg_val, 10, exit);

  fld_val = SOC_SAND_BOOL2NUM(glbl_hash_info->use_port_id);
  SOC_PB_FLD_TO_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_key_use_in_port, fld_val, reg_val, 14, exit);

  fld_val = glbl_hash_info->hash_func_id;
  SOC_PB_FLD_TO_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_hash_index, fld_val, reg_val, 16, exit);

  fld_val = glbl_hash_info->key_shift;
  SOC_PB_FLD_TO_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_key_shift, fld_val, reg_val, 18, exit);

  SOC_PB_REG_SET(regs->ihb.lag_lb_key_cfg_reg, reg_val, 20, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_global_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_lag_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LAG_HASH_GLOBAL_INFO, glbl_hash_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_global_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_lag_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_global_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the LAG hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_GLOBAL_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);
  SOC_PB_PP_LAG_HASH_GLOBAL_INFO_clear(glbl_hash_info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->ihb.lag_lb_key_cfg_reg, reg_val, 8, exit);

  SOC_PB_FLD_FROM_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_key_seed, fld_val, reg_val, 10, exit);
  glbl_hash_info->seed = fld_val;

  SOC_PB_FLD_FROM_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_key_use_in_port, fld_val, reg_val, 14, exit);
  glbl_hash_info->use_port_id = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_hash_index, fld_val, reg_val, 16, exit);
  glbl_hash_info->hash_func_id = (uint8)fld_val;

  SOC_PB_FLD_FROM_REG(regs->ihb.lag_lb_key_cfg_reg.lag_lb_key_shift, fld_val, reg_val, 18, exit);
  glbl_hash_info->key_shift = (uint8)fld_val;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_global_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the LAG hashing per-lag attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PINFO_FER_TBL_DATA
    pinfo_fer_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_hash_info);

  res = soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          port_ndx,
          &pinfo_fer_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  pinfo_fer_tbl.lag_lb_key_start = (lag_hash_info->first_header_to_parse == SOC_PB_PP_LAG_HASH_FRST_HDR_FARWARDING)?0x1:0x0;
  pinfo_fer_tbl.lag_lb_key_count = lag_hash_info->nof_headers;

  res = soc_pb_pp_ihb_pinfo_fer_tbl_set_unsafe(
          unit,
          port_ndx,
          &pinfo_fer_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_port_info_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_pp_lag_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LAG_HASH_PORT_INFO, lag_hash_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_port_info_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_pp_lag_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_port_info_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set the LAG hashing per-lag attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LAG_HASH_PORT_INFO                  *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PINFO_FER_TBL_DATA
    pinfo_fer_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_hash_info);

  SOC_PB_PP_LAG_HASH_PORT_INFO_clear(lag_hash_info);

  res = soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          port_ndx,
          &pinfo_fer_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  lag_hash_info->first_header_to_parse = (pinfo_fer_tbl.lag_lb_key_start == 0x1)?SOC_PB_PP_LAG_HASH_FRST_HDR_FARWARDING:SOC_PB_PP_LAG_HASH_FRST_HDR_LAST_TERMINATED;
  lag_hash_info->nof_headers = (uint8)pinfo_fer_tbl.lag_lb_key_count;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_port_info_get_unsafe()", port_ndx, 0);
}


/*********************************************************************
*     Mask / unmask fields from the packet header. Masked
 *     fields are ignored by the hashing function
 *     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC SOC_PB_PP_HASH_MASKS
  soc_pb_pp_lag_hash_sym_peer(
    SOC_SAND_IN  SOC_PB_PP_HASH_MASKS            field
  )
{
	switch (field)
	{
  /* first part of symmetric, XOR + 2*/
	case SOC_PB_PP_HASH_MASKS_MAC_DA:
    return SOC_PB_PP_HASH_MASKS_MAC_SA;
	case SOC_PB_PP_HASH_MASKS_IPV6_SIP:
    return SOC_PB_PP_HASH_MASKS_IPV6_DIP;
  case SOC_PB_PP_HASH_MASKS_FC_ORG_EX_ID:
		return SOC_PB_PP_HASH_MASKS_FC_RES_EX_ID;
  /* first part of symmetric, XOR + 4*/
	case SOC_PB_PP_HASH_MASKS_IPV4_SIP:
    return SOC_PB_PP_HASH_MASKS_IPV4_DIP;
	case SOC_PB_PP_HASH_MASKS_FC_DEST_ID:
		return SOC_PB_PP_HASH_MASKS_FC_SRC_ID;
  case SOC_PB_PP_HASH_MASKS_L4_SRC_PORT:
    return SOC_PB_PP_HASH_MASKS_L4_DEST_PORT;
  /* first part of symmetric, XOR + 2*/
	case SOC_PB_PP_HASH_MASKS_MAC_SA:
    return SOC_PB_PP_HASH_MASKS_MAC_DA;
	case SOC_PB_PP_HASH_MASKS_IPV6_DIP:
    return SOC_PB_PP_HASH_MASKS_IPV6_SIP;
  case SOC_PB_PP_HASH_MASKS_FC_RES_EX_ID:
		return SOC_PB_PP_HASH_MASKS_FC_ORG_EX_ID;
  /* first part of symmetric, XOR + 4*/
	case SOC_PB_PP_HASH_MASKS_IPV4_DIP:
    return SOC_PB_PP_HASH_MASKS_IPV4_SIP;
	case SOC_PB_PP_HASH_MASKS_FC_SRC_ID:
		return SOC_PB_PP_HASH_MASKS_FC_DEST_ID;
  case SOC_PB_PP_HASH_MASKS_L4_DEST_PORT:
    return SOC_PB_PP_HASH_MASKS_L4_SRC_PORT;
  /* not part of symmetric: just take as is*/
	case SOC_PB_PP_HASH_MASKS_ETH_TYPE_CODE:
	case SOC_PB_PP_HASH_MASKS_VSI:
	case SOC_PB_PP_HASH_MASKS_MPLS_LABEL_1:
	case SOC_PB_PP_HASH_MASKS_MPLS_LABEL_2:
	case SOC_PB_PP_HASH_MASKS_MPLS_LABEL_3:
	case SOC_PB_PP_HASH_MASKS_IPV4_PROTOCOL:
	case SOC_PB_PP_HASH_MASKS_FC_SEQ_ID:
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
  soc_pb_pp_lag_calc_nible_val(
    SOC_SAND_IN  SOC_PB_PP_HASH_MASKS            field,
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
	case SOC_PB_PP_HASH_MASKS_MAC_DA:
	case SOC_PB_PP_HASH_MASKS_IPV6_SIP:
	case SOC_PB_PP_HASH_MASKS_L4_SRC_PORT:
  case SOC_PB_PP_HASH_MASKS_FC_ORG_EX_ID:
		return 2;
  /* first part of symmetric, XOR + 4*/
	case SOC_PB_PP_HASH_MASKS_IPV4_SIP:
	case SOC_PB_PP_HASH_MASKS_FC_DEST_ID:
		return 3;
  /* second part of symmetric, ignore*/
  case SOC_PB_PP_HASH_MASKS_MAC_SA:
  case SOC_PB_PP_HASH_MASKS_IPV4_DIP:
  case SOC_PB_PP_HASH_MASKS_IPV6_DIP:
  case SOC_PB_PP_HASH_MASKS_L4_DEST_PORT:
  case SOC_PB_PP_HASH_MASKS_FC_SRC_ID:
  case SOC_PB_PP_HASH_MASKS_FC_RES_EX_ID:
    return 0;
  /* not part of symmetric: just take as is*/
	case SOC_PB_PP_HASH_MASKS_ETH_TYPE_CODE:
	case SOC_PB_PP_HASH_MASKS_VSI:
	case SOC_PB_PP_HASH_MASKS_MPLS_LABEL_1:
	case SOC_PB_PP_HASH_MASKS_MPLS_LABEL_2:
	case SOC_PB_PP_HASH_MASKS_MPLS_LABEL_3:
	case SOC_PB_PP_HASH_MASKS_IPV4_PROTOCOL:
	case SOC_PB_PP_HASH_MASKS_FC_SEQ_ID:
	default:
		return 1;
	}

}


uint32
  soc_pb_pp_lag_hashing_mask_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_HASH_MASK_INFO                      *mask_info
  )
{
  uint32
   mask_idx,
   vc_indx,
   fld_val,
   field_nbl_indx,
   field_nof_nbls,
   field_info_offset,
   is_equal,
   nible_val;
  SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA
    lb_vector_program_map_tbl_data,
    old_val;
  uint8
    is_symmetric = FALSE,
    skip;
  SOC_PB_PP_HASH_MASKS
    masks,
    fld,
    peer_fld;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_MASK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mask_info);

  regs = soc_petra_regs();

  masks = mask_info->mask;

  is_symmetric = mask_info->is_symmetric_key;
  for (vc_indx = SOC_PB_PP_LB_VECTOR_INDEX_NONE; vc_indx < SOC_PB_PP_LB_NOF_VECTOR_INDEXES; ++vc_indx)
  {
    /*
     * update Ethernet key
     */
    res = soc_pb_pp_ihb_lb_vector_program_map_tbl_get_unsafe(
     	      unit,
            vc_indx,
		        &lb_vector_program_map_tbl_data
	        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

    SOC_PETRA_COPY(&old_val,&lb_vector_program_map_tbl_data,SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA,1);

    lb_vector_program_map_tbl_data.chunk_size = Soc_pb_pp_lb_key_chunk_size[vc_indx];

    for (mask_idx = 0; mask_idx < SOC_PB_PP_NOF_HASH_MASKS; ++mask_idx)
    {
      skip = TRUE;
	    field_info_offset = mask_idx; /*SOC_PB_PP_LB_FIELD_TO_OFFSET(SOC_SAND_BIT(mask_idx));*/

      if(Soc_pb_pp_lag_hash_field_info[field_info_offset].key_index == vc_indx)
      {
        skip = FALSE;
      }
      if ((SOC_SAND_BIT(mask_idx) == SOC_PB_PP_HASH_MASKS_MPLS_LABEL_1) && (SOC_PB_PP_LB_MPLS_LBL1_VLD(vc_indx)))
      {
        skip = FALSE;
      }
      else if ((SOC_SAND_BIT(mask_idx) == SOC_PB_PP_HASH_MASKS_MPLS_LABEL_2) && (SOC_PB_PP_LB_MPLS_LBL2_VLD(vc_indx)))
      {
        skip = FALSE;
      }
      else if ((SOC_SAND_BIT(mask_idx) == SOC_PB_PP_HASH_MASKS_MPLS_LABEL_3) && (SOC_PB_PP_LB_MPLS_LBL3_VLD(vc_indx)))
      {
        skip = FALSE;
      }

      if (skip)
      {
        continue;
      }
      fld = SOC_SAND_BIT(mask_idx);
      peer_fld = soc_pb_pp_lag_hash_sym_peer(fld);
	    nible_val = soc_pb_pp_lag_calc_nible_val(fld, (uint8)((masks & fld)== 0), (uint8)((masks & peer_fld)== 0), is_symmetric);
	    field_nof_nbls = Soc_pb_pp_lag_hash_field_info[field_info_offset].nof_nibles;

	    for (field_nbl_indx = 0 ; field_nbl_indx < field_nof_nbls; ++field_nbl_indx)
	    {
	      res = soc_sand_bitstream_set_any_field(
	              &nible_val,
	   	          SOC_PB_PP_LB_FIELD_TO_NIBLE(Soc_pb_pp_lag_hash_field_info[field_info_offset].nbls[field_nbl_indx],lb_vector_program_map_tbl_data.chunk_size),
		            2/*(lb_vector_program_map_tbl_data.chunk_size)?8:4*/,
		            lb_vector_program_map_tbl_data.chunk_bitmap
    	        );
	      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
      }
    }

    SOC_PETRA_COMP(&old_val,&lb_vector_program_map_tbl_data,SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA,1,is_equal);

    if (!is_equal)
    {
      res = soc_pb_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
     	        unit,
              vc_indx,
		          &lb_vector_program_map_tbl_data
	          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
    }
  }
 /*
  * set CW
  */
  fld_val = SOC_SAND_BOOL2NUM(mask_info->expect_cw);
  SOC_PB_FLD_SET(regs->ihb.lag_lb_key_cfg_reg.lb_mpls_control_word, fld_val, 112, exit)

  /* store key in SW DB */
  soc_pb_pp_sw_db_lag_lb_key_mask_set(unit, masks);
  soc_pb_pp_sw_db_lag_lb_key_is_symtrc_set(unit, is_symmetric);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_mask_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_lag_hashing_mask_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PP_HASH_MASK_INFO       *mask_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_MASK_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_HASH_MASK_INFO, mask_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_mask_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_lag_hashing_mask_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_MASK_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_mask_get_verify()", 0, 0);
}

/*********************************************************************
*     Mask / unmask fields from the packet header. Masked
 *     fields are ignored by the hashing function
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lag_hashing_mask_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_HASH_MASK_INFO       *mask_info
  )
{
  uint32
    fld_val;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LAG_HASHING_MASK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mask_info);

  SOC_PB_PP_HASH_MASK_INFO_clear(mask_info);
  
  regs = soc_petra_regs();

  mask_info->mask = soc_pb_pp_sw_db_lag_lb_key_mask_get(unit);
  mask_info->is_symmetric_key = soc_pb_pp_sw_db_lag_lb_key_is_symtrc_get(unit);
 /*
  * get CW
  */
  SOC_PB_FLD_GET(regs->ihb.lag_lb_key_cfg_reg.lb_mpls_control_word, fld_val, 112, exit)
  mask_info->expect_cw = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lag_hashing_mask_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_lag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lag_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_lag;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_lag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lag_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_lag;
}

uint32
  SOC_PB_PP_LAG_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_ERR_IF_ABOVE_MAX(info->hash_func_id, SOC_PB_PP_LAG_HASH_FUNC_ID_MAX, SOC_PB_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 14, exit);
   SOC_SAND_ERR_IF_ABOVE_MAX(info->key_shift, SOC_PB_PP_LAG_KEY_SHIFT_MAX, SOC_PB_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR, 15, exit);
   SOC_SAND_ERR_IF_ABOVE_MAX(info->seed, SOC_PB_PP_LAG_SEED_MAX, SOC_PB_PP_LAG_SEED_OUT_OF_RANGE_ERR, 16, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LAG_HASH_GLOBAL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LAG_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_headers, SOC_PB_PP_LAG_NOF_HEADERS_MAX, SOC_PB_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR, 10, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_header_to_parse, SOC_PB_PP_LAG_FIRST_HEADER_TO_PARSE_MAX, SOC_PB_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LAG_HASH_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_HASH_MASK_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_HASH_MASK_INFO *info
  )
{
  uint32
    invalid_mask;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  invalid_mask = SOC_SAND_BITS_MASK(31,SOC_PB_PP_NOF_HASH_MASKS+1);
  
  if ((invalid_mask & info->mask) != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LAG_MASK_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_HASH_MASK_INFO_verify()",info->mask,0);
}

uint32
  SOC_PB_PP_LAG_MEMBER_verify(
    SOC_SAND_IN  SOC_PB_PP_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_port, SOC_PB_PP_LAG_SYS_PORT_MAX, SOC_PB_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->member_id, SOC_PB_PP_LAG_MEMBER_ID_MAX, SOC_PB_PP_LAG_MEMBER_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LAG_MEMBER_verify()",0,0);
}

uint32
  SOC_PB_PP_LAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind,ind2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, SOC_PB_PP_LAG_NOF_ENTRIES_MAX, SOC_PB_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PB_PP_LAG_MEMBERS_MAX; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LAG_MEMBER, &(info->members[ind]), 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lb_type, SOC_PB_PP_LAG_LB_TYPE_MAX, SOC_PB_PP_LAG_LB_TYPE_OUT_OF_RANGE_ERR, 12, exit);

  for (ind = 0; ind < info->nof_entries; ++ind)
  {
    for (ind2 = 0; ind2 < info->nof_entries; ++ind2)
    {
      if (info->members[ind].sys_port == info->members[ind2].sys_port && (ind != ind2))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LAG_DOUPLICATE_MEMBER_ERR, 12, exit);
      }
    }
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LAG_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

