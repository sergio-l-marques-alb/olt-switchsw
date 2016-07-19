/* $Id: pb_pp_llp_parse.c,v 1.8 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_MAX                   (7)
#define SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX                   (7)
#define SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX                    (3)
#define SOC_PB_PP_LLP_PARSE_INDEX_MAX                              (3)
#define SOC_PB_PP_LLP_PARSE_TYPE_MAX                               (SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1)
#define SOC_PB_PP_LLP_PARSE_OUTER_TPID_MAX                         (SOC_PB_PP_NOF_LLP_PARSE_TPID_INDEXS-1)
#define SOC_PB_PP_LLP_PARSE_INNER_TPID_MAX                         (SOC_PB_PP_NOF_LLP_PARSE_TPID_INDEXS-1)
#define SOC_PB_PP_LLP_PARSE_TAG_FORMAT_MAX                         (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)

#define SOC_PB_PP_LLP_PARSE_TT_DECODE_ETH                          (0)
#define SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS1                        (1)
#define SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS2                        (2)
#define SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS3                        (3)
#define SOC_PB_PP_LLP_PARSE_TT_DECODE_IP                           (4)
#define SOC_PB_PP_LLP_PARSE_TT_DECODE_TRILL                        (5)
#define SOC_PB_PP_LLP_PARSE_TT_DECODE_DEFAULT                      (6)
#define SOC_PB_PP_LLP_PARSE_PRIORITY_TAG_TYPE_MAX                  (SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1)
#define SOC_PB_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_MAX               (63)
#define SOC_PB_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_MAX              (1)


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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_llp_parse[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_PARSE_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_llp_parse[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_id' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR",
    "The parameter 'outer_tpid' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LLP_PARSE_TPID_INDEXS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR",
    "The parameter 'inner_tpid' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LLP_PARSE_TPID_INDEXS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'priority_tag_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR",
    "The parameter 'dlft_edit_command_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'dflt_edit_pcp_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_LLP_PARSE_TYPE_ILLEGAL_ERR,
    "SOC_PB_PP_LLP_PARSE_TYPE_ILLEGAL_ERR",
    "User should not specify VLAN_TAG type for TPID profile. \n\r "
    "SOC_PB_PP_LLP_PARSE_TPID_INFO.type must be SOC_SAND_PP_VLAN_TAG_TYPE_ANY.\n\r "
    "this is the default value set by clear function.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP - .\n\r "
    "SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'priority_tag_type' is out of range. \n\r "
    "The range is: SOC_SAND_PP_VLAN_TAG_TYPE_CTAG or  .\n\r "
    "SOC_SAND_PP_VLAN_TAG_TYPE_STAG.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR,
    "SOC_PB_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR",
    "The parameter 'priority_tag_type' is illegal. \n\r "
    "The range is: when is_outer_prio is FALSE \n\r "
    "then has to be SOC_SAND_PP_VLAN_TAG_TYPE_NONE.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_llp_parse_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;
/*  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ihp_ll_mirror_profile_tbl_data;*/
  SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA
    ihp_llr_llvp_tbl_data;
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA
    ihp_llvp_prog_sel_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
/* $Id: pb_pp_llp_parse.c,v 1.8 Broadcom SDK $
  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_ll_mirror_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  ihp_ll_mirror_profile_tbl_data.ll_mirror_profile = 0x1;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_ll_mirror_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);*/

  res = soc_pb_pp_ihp_llr_llvp_tbl_get_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_llr_llvp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  ihp_llr_llvp_tbl_data.acceptable_frame_type_action = 0x1;
  /* Vlan compression range indexes are constant*/
  ihp_llr_llvp_tbl_data.outer_comp_index = SOC_PB_PP_LLP_PARSE_VLAN_COMPRESSION_OUTER_COMP_NDX;
  ihp_llr_llvp_tbl_data.inner_comp_index = SOC_PB_PP_LLP_PARSE_VLAN_COMPRESSION_INNER_COMP_NDX;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihp_llr_llvp_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_llr_llvp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  for(idx = 0; idx < 1024; ++idx)
  {
    if (SOC_SAND_GET_FLD_FROM_PLACE(idx, 8, SOC_SAND_BITS_MASK(9, 8)) == SOC_PB_PARSER_INJECTED_KEY_PROGRAM_PROFILE)
    {
      ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY;
    }
    else
    {
    if (SOC_SAND_GET_FLD_FROM_PLACE(idx, 8, SOC_SAND_BITS_MASK(9, 8)) == SOC_PB_PARSER_NON_ETH_KEY_PROGRAM_PROFILE) /* TM and raw set per PP-Port */
    {
      ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_NO_LOOKUP;
    }
	  else if ((SOC_SAND_GET_FLD_FROM_PLACE(idx, 6, SOC_SAND_BITS_MASK(7, 6))==3) && (idx & 1)) /* mybmac and itag --> no btag */
	  {
		  ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_ID_ISID_WO_BTAG;
	  }
	  else if ((SOC_SAND_GET_FLD_FROM_PLACE(idx, 3, SOC_SAND_BITS_MASK(4, 3))==3) && (idx & 1) 
		        &&(SOC_SAND_GET_FLD_FROM_PLACE(idx, 3, SOC_SAND_BITS_MASK(7, 6))!=0) ) /* mybmac, btag and itag */
	  {
		  ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_ID_ISID_W_BTAG;
	  }
	  else if (SOC_SAND_GET_FLD_FROM_PLACE(idx, 6, SOC_SAND_BITS_MASK(7, 6)))
	  {
		  ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID;
	  }
      else
      {
        switch(SOC_SAND_GET_FLD_FROM_PLACE(idx, 3, SOC_SAND_BITS_MASK(5, 3)))
        {
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_ETH:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID;
          break;
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_IP:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_DST_IP;
          break;
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_TRILL:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_TRILL_ING_NICK;
          break;
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS1:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS1;
          break;
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS2:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS2;
          break;
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS3:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS3;
          break;
        case SOC_PB_PP_LLP_PARSE_TT_DECODE_DEFAULT:
        default:
          ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID;
          break;
        }
      }
    }
    res = soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe(
            unit,
            idx,
            &ihp_llvp_prog_sel_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_init_unsafe()", 0, 0);
}

/************************************************************************/
/* internal Function                                                    */
/************************************************************************/

STATIC uint8
  soc_pb_pp_llp_parse_is_outer_s_tag(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO     *packet_format
  )
{
  /* if tag format indicates the outer is s-tag */
  if (
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG
     )
  {
    return TRUE;
  }
  /* if tag format indicates outer is Priority, and Priority considered as s-tag */
  if (
      (packet_format->priority_tag_type == SOC_SAND_PP_VLAN_TAG_TYPE_STAG) &&
       (packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG)
     )
  {
    return TRUE;
  }
  return FALSE;
}


STATIC uint8
  soc_pb_pp_llp_parse_is_outer_c_tag(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO     *packet_format
  )
{
  /* if tag format indicates the outer is s-tag */
  if (
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG
     )
  {
    return TRUE;
  }
  /* if tag format indicates outer is Priority, and Priority considered as s-tag */
  if (
      (packet_format->priority_tag_type == SOC_SAND_PP_VLAN_TAG_TYPE_CTAG) &&
       (packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG)
     )
  {
    return TRUE;
  }
  return FALSE;
}


STATIC uint8
  soc_pb_pp_llp_parse_is_inner_c_tag(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO     *packet_format
  )
{
  /* if tag format indicates the outer is s-tag */
  if (
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
     )
  {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************************
*     Sets the global information for link layer parsing,
 *     including TPID values. Used in ingress to identify VLAN
 *     tags on incoming packets, and used in egress to
 *     construct VLAN tags on outgoing packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_values_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_vals);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_SET(regs->eci.tpid_configuration_register_reg[0].tpid0, tpid_vals->tpid_vals[0], 10, exit);
  SOC_PB_PP_FLD_SET(regs->eci.tpid_configuration_register_reg[0].tpid1, tpid_vals->tpid_vals[1], 20, exit);
  SOC_PB_PP_FLD_SET(regs->eci.tpid_configuration_register_reg[1].tpid0, tpid_vals->tpid_vals[2], 30, exit);
  SOC_PB_PP_FLD_SET(regs->eci.tpid_configuration_register_reg[1].tpid1, tpid_vals->tpid_vals[3], 40, exit);
  SOC_PB_PP_FLD_SET(regs->eci.tpid_configuration_register_2_reg.itag_tpid, tpid_vals->tpid_vals[4], 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_values_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_parse_tpid_values_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_TPID_VALUES, tpid_vals, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_values_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_llp_parse_tpid_values_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_values_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets the global information for link layer parsing,
 *     including TPID values. Used in ingress to identify VLAN
 *     tags on incoming packets, and used in egress to
 *     construct VLAN tags on outgoing packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_values_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_vals);

  SOC_PB_PP_LLP_PARSE_TPID_VALUES_clear(tpid_vals);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->eci.tpid_configuration_register_reg[0].tpid0, tmp, 10, exit);
  tpid_vals->tpid_vals[0] = (SOC_SAND_PP_TPID)tmp;
  SOC_PB_PP_FLD_GET(regs->eci.tpid_configuration_register_reg[0].tpid1, tmp, 20, exit);
  tpid_vals->tpid_vals[1] = (SOC_SAND_PP_TPID)tmp;
  SOC_PB_PP_FLD_GET(regs->eci.tpid_configuration_register_reg[1].tpid0, tmp, 30, exit);
  tpid_vals->tpid_vals[2] = (SOC_SAND_PP_TPID)tmp;
  SOC_PB_PP_FLD_GET(regs->eci.tpid_configuration_register_reg[1].tpid1, tmp, 40, exit);
  tpid_vals->tpid_vals[3] = (SOC_SAND_PP_TPID)tmp;
  SOC_PB_PP_FLD_GET(regs->eci.tpid_configuration_register_2_reg.itag_tpid, tmp, 50, exit);
  tpid_vals->tpid_vals[4] = (SOC_SAND_PP_TPID)tmp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_values_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets the TPID profile selection of two TPIDs from the
 *     Global TPIDs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    tmp,
    lsb,
    msb;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_info);

  regs = soc_pb_pp_regs();
  /* if this tpid profile for both parsing: ingress and egress  */
  if (tpid_profile_ndx <= SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX)
  {
    lsb = tpid_profile_ndx*2;
    msb = tpid_profile_ndx*2 + 1;
    /* set relevant bits for the profile, 2 bits per profile */
    /* get field val for TPID1*/
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index0, tmp, 10, exit);
    res = soc_sand_set_field(&tmp, msb, lsb, tpid_profile_info->tpid1.index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    /* set field val*/
    SOC_PB_PP_FLD_SET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index0, tmp, 25, exit);

    /* get field val for TPID2*/
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index0, tmp, 30, exit);
    res = soc_sand_set_field(&tmp, msb, lsb, tpid_profile_info->tpid2.index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
    /* set field val*/
    SOC_PB_PP_FLD_SET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index0, tmp, 30, exit);
  }
  else
  {
    lsb = (tpid_profile_ndx-SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX-1)*2;
    msb = (tpid_profile_ndx-SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX-1)*2 + 1;

    /* else tpid-profile > 3 and this profile for egress parsing only */
    /* TPID-0*/
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index1, tmp, 50, exit);
    res = soc_sand_set_field(&tmp, msb, lsb, tpid_profile_info->tpid1.index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    SOC_PB_PP_FLD_SET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index1, tmp, 65, exit);
    /* TPID-1*/
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index1, tmp, 70, exit);
    res = soc_sand_set_field(&tmp, msb, lsb, tpid_profile_info->tpid2.index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    SOC_PB_PP_FLD_SET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index1, tmp, 85, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_profile_info_set_unsafe()", tpid_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_parse_tpid_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tpid_profile_ndx, SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_MAX, SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO, tpid_profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_profile_info_set_verify()", tpid_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_parse_tpid_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tpid_profile_ndx, SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_MAX, SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_profile_info_get_verify()", tpid_profile_ndx, 0);
}

/*********************************************************************
*     Sets the TPID profile selection of two TPIDs from the
 *     Global TPIDs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    tmp,
    lsb,
    msb;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_info);

  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_clear(tpid_profile_info);

  regs = soc_pb_pp_regs();
  /* if this tpid profile for both parsing: ingress and egress  */
  if (tpid_profile_ndx <= SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX)
  {
    lsb = tpid_profile_ndx*2;
    msb = tpid_profile_ndx*2 + 1;
    /* set relevant bits for the profile, 2 bits per profile */
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index0, tmp, 10, exit);
    tpid_profile_info->tpid1.index = (uint8)SOC_SAND_GET_BITS_RANGE(tmp, msb, lsb);
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index0, tmp, 30, exit);
    tpid_profile_info->tpid2.index = (uint8)SOC_SAND_GET_BITS_RANGE(tmp, msb, lsb);
  }
  else
  {
    lsb = (tpid_profile_ndx-SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX-1)*2;
    msb = (tpid_profile_ndx-SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX-1)*2 + 1;

    /* else tpid-profile > 3 and this profile for egress parsing only */
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index1, tmp, 50, exit);
    tpid_profile_info->tpid1.index = (uint8)SOC_SAND_GET_BITS_RANGE(tmp, msb, lsb);
    SOC_PB_PP_FLD_GET(regs->eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index1, tmp, 70, exit);
    tpid_profile_info->tpid2.index = (uint8)SOC_SAND_GET_BITS_RANGE(tmp, msb, lsb);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_tpid_profile_info_get_unsafe()", tpid_profile_ndx, 0);
}

/*********************************************************************
*     Maps from Port profile to TPID Profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe()", port_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tpid_profile_id, SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_MAX, SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_verify()", port_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_get_verify()", port_profile_ndx, 0);
}

/*********************************************************************
*     Maps from Port profile to TPID Profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT uint32                                  *tpid_profile_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_id);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_get_unsafe()", port_profile_ndx, 0);
}

/*********************************************************************
*     Given the port profile and the parsing information
 *     determine: - Whether this packet format accepted or
 *     denied. - The tag structure of the packet, i.e. what
 *     vlan tags exist on the packet (S-tag, S-C-tag, etc...).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_packet_format_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA
    llr_llvp_tbl;
  SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA
    vtt_llvp_tbl;
  SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA
    epni_llvp_tbl;
  uint32
    entry_offset,
    entry_indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);

  /* map parsing key into index in the LLVP table */
  entry_offset = SOC_PB_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(
                   port_profile_ndx,
                   parse_key->outer_tpid,
                   parse_key->is_outer_prio,
                   parse_key->inner_tpid
                 );

  res = soc_pb_pp_ihp_llr_llvp_tbl_get_unsafe(
          unit,
          entry_offset,
          &llr_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  llr_llvp_tbl.acceptable_frame_type_action = (format_info->action_trap_code - SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP);

  /* vid exist IFF tag format is not none */
  llr_llvp_tbl.incoming_tag_exist = (format_info->tag_format != SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE);
  /* vid exist IFF Tag exist and it's not priority */
  llr_llvp_tbl.incoming_vid_exist = (llr_llvp_tbl.incoming_tag_exist && !parse_key->is_outer_prio);
  /* S-tag exist IFF Tag exist and outer is stag */
  llr_llvp_tbl.incoming_stag_exist = (llr_llvp_tbl.incoming_tag_exist && soc_pb_pp_llp_parse_is_outer_s_tag(format_info));

  res = soc_pb_pp_ihp_llr_llvp_tbl_set_unsafe(
          unit,
          entry_offset,
          &llr_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);


  res = soc_pb_pp_ihp_vtt_llvp_tbl_get_unsafe(
          unit,
          entry_offset<<2,
          &vtt_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  vtt_llvp_tbl.incoming_tag_structure = format_info->tag_format;
  vtt_llvp_tbl.ivec = format_info->dlft_edit_command_id;
  vtt_llvp_tbl.pcp_dei_profile = format_info->dflt_edit_pcp_profile;

  /* set 4 entries, as 2 lsb for key-prog-profile */
  for (entry_indx = 0; entry_indx < 3; ++entry_indx)
  {
    res = soc_pb_pp_ihp_vtt_llvp_tbl_set_unsafe(
            unit,
            (entry_offset<<2) + entry_indx,
            &vtt_llvp_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }


  res = soc_pb_pp_epni_llvp_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  epni_llvp_tbl.llvp_incoming_tag_format = format_info->tag_format;
  
  if (soc_pb_pp_llp_parse_is_outer_c_tag(format_info))
  {
    epni_llvp_tbl.llvp_c_tag_offset = 0;
    /* is outer C-tag */
    epni_llvp_tbl.llvp_packet_has_c_tag = TRUE;
    epni_llvp_tbl.llvp_packet_has_pcp_dei = FALSE;
    epni_llvp_tbl.llvp_packet_has_up = TRUE;
  }
  else if (soc_pb_pp_llp_parse_is_inner_c_tag(format_info))
  {
    epni_llvp_tbl.llvp_c_tag_offset = 1;
    /* is outer C-tag */
    epni_llvp_tbl.llvp_packet_has_c_tag = TRUE;
  }
  else
  {
    epni_llvp_tbl.llvp_packet_has_c_tag = FALSE;
  }
  /* if outer is S-Tag then has PCP-DEI */
  if (soc_pb_pp_llp_parse_is_outer_s_tag(format_info))
  {
    epni_llvp_tbl.llvp_packet_has_pcp_dei = TRUE;
    epni_llvp_tbl.llvp_packet_has_up = FALSE;
  }

  res = soc_pb_pp_epni_llvp_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &epni_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  
  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_packet_format_info_set_unsafe()", port_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_parse_packet_format_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_INFO, parse_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO, format_info, 30, exit);
  
  if (parse_key->is_outer_prio)
  {
    /* Priority tag Type can be S or C*/
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      format_info->priority_tag_type,
      SOC_SAND_PP_VLAN_TAG_TYPE_CTAG,
      SOC_SAND_PP_VLAN_TAG_TYPE_STAG,
      SOC_PB_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
      40,
      exit
    );
  }
  else
  {
    /* Priority tag Type has to be none. for tag format with no priority tag*/
    if (format_info->priority_tag_type != SOC_SAND_PP_VLAN_TAG_TYPE_NONE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_packet_format_info_set_verify()", port_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_parse_packet_format_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_INFO, parse_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_packet_format_info_get_verify()", port_profile_ndx, 0);
}

/*********************************************************************
*     Given the port profile and the parsing information
 *     determine: - Whether this packet format accepted or
 *     denied. - The tag structure of the packet, i.e. what
 *     vlan tags exist on the packet (S-tag, S-C-tag, etc...).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_packet_format_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA
    llr_llvp_tbl;
  SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA
    vtt_llvp_tbl;
  uint32
    entry_offset;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);

  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_clear(format_info);

  /* map parsing key into index in the LLVP table */
  entry_offset = SOC_PB_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(
                   port_profile_ndx,
                   parse_key->outer_tpid,
                   parse_key->is_outer_prio,
                   parse_key->inner_tpid
                 );

  res = soc_pb_pp_ihp_llr_llvp_tbl_get_unsafe(
          unit,
          entry_offset,
          &llr_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  format_info->action_trap_code = (SOC_PB_PP_TRAP_CODE)(llr_llvp_tbl.acceptable_frame_type_action + SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP);

  res = soc_pb_pp_ihp_vtt_llvp_tbl_get_unsafe(
          unit,
          entry_offset<<2,
          &vtt_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  format_info->tag_format = vtt_llvp_tbl.incoming_tag_structure;
  format_info->dlft_edit_command_id = vtt_llvp_tbl.ivec;
  format_info->dflt_edit_pcp_profile = vtt_llvp_tbl.pcp_dei_profile;

  format_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_CTAG;

  /* if outer is not priority then priority type is none */
  if (!parse_key->is_outer_prio)
  {
    format_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_NONE;
  }
  else
  {
    /* check if format has S-tag as outer, the priority used as S-tag */
    if (soc_pb_pp_llp_parse_is_outer_s_tag(format_info))
    {
      format_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_STAG;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_parse_packet_format_info_get_unsafe()", port_profile_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_llp_parse module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_parse_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_llp_parse;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_llp_parse module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_parse_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_llp_parse;
}
uint32
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
  for (ind = 0; ind < SOC_PB_PP_LLP_PARSE_NOF_TPID_VALS; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_vals[ind], SOC_SAND_PP_TPID_MAX, SOC_SAND_PP_TPID_OUT_OF_RANGE_ERR, 10, exit);
  }
  */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_PARSE_TPID_VALUES_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_PARSE_TPID_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->index, SOC_PB_PP_LLP_PARSE_INDEX_MAX, SOC_PB_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_LLP_PARSE_TYPE_MAX, SOC_PB_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  if (info->type != SOC_SAND_PP_VLAN_TAG_TYPE_ANY)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_PARSE_TYPE_ILLEGAL_ERR, 12, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_PARSE_TPID_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_TPID_INFO, &(info->tpid1), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_TPID_INFO, &(info->tpid2), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_PARSE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->outer_tpid, SOC_PB_PP_LLP_PARSE_OUTER_TPID_MAX, SOC_PB_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->inner_tpid, SOC_PB_PP_LLP_PARSE_INNER_TPID_MAX, SOC_PB_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_PARSE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_format, SOC_PB_PP_LLP_PARSE_TAG_FORMAT_MAX, SOC_PB_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->priority_tag_type, SOC_PB_PP_LLP_PARSE_PRIORITY_TAG_TYPE_MAX, SOC_PB_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dlft_edit_command_id, SOC_PB_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_MAX, SOC_PB_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dflt_edit_pcp_profile, SOC_PB_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_MAX, SOC_PB_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR, 14, exit);
  /* trap code can be in the range drop-custom2 */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->action_trap_code,
    SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP,
    SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2,
    SOC_PB_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR,
    20,
    exit
  );

  /* Priority tag Type can be none, S or C*/
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->priority_tag_type,
    SOC_SAND_PP_VLAN_TAG_TYPE_NONE,
    SOC_SAND_PP_VLAN_TAG_TYPE_STAG,
    SOC_PB_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
    30,
    exit
  );

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

