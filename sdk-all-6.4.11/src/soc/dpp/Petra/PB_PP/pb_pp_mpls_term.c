/* $Id: pb_pp_mpls_term.c,v 1.12 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_mpls_term.c
*
* MODULE PREFIX:  soc_pb_pp
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
#include <soc/dpp/Petra/PB_PP/pb_pp_mpls_term.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX                          (2)
#define SOC_PB_PP_MPLS_TERM_KEY_TYPE_MAX                           (SOC_PB_PP_NOF_MPLS_TERM_KEY_TYPES-1)
#define SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_MAX                    (SOC_PB_PP_NOF_MPLS_TERM_MODEL_TYPES-1)
#define SOC_PB_PP_MPLS_TERM_COS_PROFILE_MAX                        (15)
#define SOC_PB_PP_MPLS_TERM_RANGES_NOF_BITS                        (16*1024)
#define SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_MAX                         (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_MPLS_TERM_MODEL_MAX                              (SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1)

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

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_mpls_term[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_COS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MPLS_TERM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_UPDATE_BASES),
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_mpls_term[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'range_ndx' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_IS_TERMINATED_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_IS_TERMINATED_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'is_terminated_label' is out of range. \n\r "
    "The range is: 0 - No max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'key_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MPLS_TERM_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'processing_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MPLS_TERM_MODEL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_COS_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR",
    "The parameter 'next_prtcl' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L3_NEXT_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR",
    "The parameter 'model' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' or. \n\r "
    "'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - 0 set using soc_ppd_trap_mgmt apis\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' for reserved labels\n\r "
    "BOS/non-BOS is out of range \n\r"
    "The range is: SOC_PPD_TRAP_CODE_MPLS_LABEL_VALUE_0(60) - SOC_PPD_TRAP_CODE_MPLS_LABEL_VALUE_3(63)",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR",
    " calling soc_pb_pp_mpls_term_range_terminated_label_set/get \n\r"
    " The parameter 'label_ndx' doesn't match any range\n\r "
    "use soc_pb_pp_mpls_term_label_range_get to see valid range",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_SIMPLE_RANGE_TERM_NEXT_NOT_MPLS_ERR,
    "SOC_PB_PP_MPLS_TERM_SIMPLE_RANGE_TERM_NEXT_NOT_MPLS_ERR",
    " calling soc_pb_pp_mpls_term_label_range_set \n\r"
    " where next protocol is not MPLS label \n\r "
    " ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR,
    "SOC_PB_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR",
    " calling soc_pb_pp_mpls_term_label_range_set \n\r"
    " where exceed nof entries in range\n\r "
    " total size of ranges is 16K",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR,
    "SOC_PB_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR",
    " calling soc_pb_pp_mpls_term_label_range_set \n\r"
    " where last label is smaller than first label \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR,
    "SOC_PB_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR",
    " calling soc_pb_pp_mpls_term_label_range_set \n\r"
    " while not keeping order, ranges in lower index \n\r "
    " has to include lower values\n\r ",
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
  soc_pb_pp_mpls_term_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA
    ihp_mpls_label_range_encountered_tbl_data;
  SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA
    ihp_ip_over_mpls_exp_mapping_tbl_data;
  uint32
    indx;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ihp_mpls_label_range_encountered_tbl_data.mpls_label_range_encountered = 0;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihp_mpls_label_range_encountered_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihp_mpls_label_range_encountered_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (indx = 0 ; indx < 8; ++indx)
  {
    ihp_ip_over_mpls_exp_mapping_tbl_data.ip_over_mpls_exp_mapping = indx;
    res = soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_set_unsafe(
            unit,
            indx,
            &ihp_ip_over_mpls_exp_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the range of MPLS labels that may be used as
 *     tunnels, and enable terminating those tables
 *     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_pb_pp_ihp_mpls_tunnel_termination_update_bases(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                                 modified_range,
    SOC_SAND_IN  int32                                 modified_val,
    SOC_SAND_IN  int32                                 old_low_val
  )
{
  uint32
    rng_indx;
  int32
    base=0;
  uint32
    size=0,
    high,
    low;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_UPDATE_BASES);

  regs = soc_pb_pp_regs();

  base = 0;

  for (rng_indx = 0; rng_indx <= SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],low,10,exit);
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],high,20,exit);
    if (rng_indx >= modified_range)
    {
      if (low >= size)
      {
        base = low - size;
        SOC_PB_PP_REG_SET(regs->ihp.mpls_label_range_base_reg[rng_indx],base,40,exit);
      }
    }
    size += (high - low);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_mpls_tunnel_termination_update_bases()", modified_range, 0);
}

/*********************************************************************
*     Set the lookup to perfrom for MPLS tunnel termination
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_lkup_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  uint8
    update;
  uint32
    idx,
    base;
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA
    ihp_llvp_prog_sel_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  if (lkup_info->key_type == SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL)
  {
    base = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS1;
  }
  else
  {
    base = SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS1;
  }
  
  for(idx = 0; idx < 1024; ++idx)
  {
    update = TRUE;
    if (SOC_SAND_GET_FLD_FROM_PLACE(idx, 6, SOC_SAND_BITS_MASK(7, 6)))
    {
      update = FALSE;
    }
    else
    {
      switch(SOC_SAND_GET_FLD_FROM_PLACE(idx, 3, SOC_SAND_BITS_MASK(5, 3)))
      {
      case 1/*SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS1*/:
        ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = base + 0;
        break;
      case 2/*SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS2*/:
        ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = base + 1;
        break;
      case 3/*SOC_PB_PP_LLP_PARSE_TT_DECODE_MPLS3*/:
        ihp_llvp_prog_sel_tbl_data.llvp_prog_sel = base + 2;
        break;
      default:
        update = FALSE;
        break;
      }
    }
    if (update)
    {
      res = soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe(
              unit,
              idx,
              &ihp_llvp_prog_sel_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_lkup_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_lkup_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LKUP_INFO, lkup_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_lkup_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_lkup_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the lookup to perfrom for MPLS tunnel termination
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA
    ihp_llvp_prog_sel_tbl_data;
  uint32
    entry_ndx = 1 << 3;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  SOC_PB_PP_MPLS_TERM_LKUP_INFO_clear(lkup_info);

  res = soc_pb_pp_ihp_llvp_prog_sel_tbl_get_unsafe(
          unit,
          entry_ndx,
          &ihp_llvp_prog_sel_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  if (ihp_llvp_prog_sel_tbl_data.llvp_prog_sel >= SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS1
      && ihp_llvp_prog_sel_tbl_data.llvp_prog_sel <= SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS3)
  {
    lkup_info->key_type = SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL;
  }
  else
  {
    lkup_info->key_type = SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL_RIF;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_lkup_info_get_unsafe()", 0, 0);
}
/*********************************************************************
*     Set the range of MPLS labels that may be used as
 *     tunnels, and enable terminating those tables
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  )
{
  uint32
    reg_val,
    old_low,
    rif_val,
    temp;
  int32
    diff = 0,
    old_diff = 0 ;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(label_range_info);

  regs = soc_pb_pp_regs();

  diff = (label_range_info->range.last_label - label_range_info->range.first_label);

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[range_ndx],temp,5,exit);
  old_diff = temp;
  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[range_ndx],old_low,3,exit);
  old_diff -= old_low;
  diff -= old_diff;
  
  SOC_PB_PP_REG_SET(regs->ihp.mpls_label_range_low_reg[range_ndx],label_range_info->range.first_label,10,exit);
  SOC_PB_PP_REG_SET(regs->ihp.mpls_label_range_high_reg[range_ndx],label_range_info->range.last_label,20,exit);

  SOC_PB_PP_REG_GET(regs->ihp.mpls_range_configs_reg[range_ndx],reg_val, 30,exit);

  rif_val = (label_range_info->term_info.rif == SOC_PB_PP_RIF_NULL)?SOC_PB_PP_RIF_NULL_VAL:label_range_info->term_info.rif;
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_in_rif,rif_val,reg_val,40,exit);
  temp = (label_range_info->term_info.processing_type == SOC_PB_PP_MPLS_TERM_MODEL_PIPE);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_is_pipe,temp,reg_val,50,exit);

  /* ranges are used only for simple purpose, i.e. info is taken from range configuration and not SEM */
  temp = TRUE;
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_is_simple,temp,reg_val,60,exit);

  temp = label_range_info->term_info.cos_profile;
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_cos_profile,temp,reg_val,70,exit);

  temp = (label_range_info->term_info.rif != SOC_PB_PP_RIF_NULL);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_in_rif_valid,temp,reg_val,80,exit);

  SOC_PB_PP_REG_SET(regs->ihp.mpls_range_configs_reg[range_ndx],reg_val, 90,exit);

  res = soc_pb_pp_ihp_mpls_tunnel_termination_update_bases(
          unit,
          range_ndx,
          diff,
          old_low
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_range_set_unsafe()", range_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_label_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  )
{
  uint32
    rng_indx;
  uint32
    temp,
    temp2;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    nof_bits = 0,
    total_bits = 0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX, SOC_PB_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO, label_range_info, 20, exit);

 /*
  * check if there is place for this range i.e. bitmap can include this range
  */
  regs = soc_pb_pp_regs();

  for (rng_indx = 0; rng_indx <= SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
    if (rng_indx == range_ndx)
    {
      nof_bits = label_range_info->range.last_label - label_range_info->range.first_label + 1;
    }
    else
    {
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],temp,40,exit);
      nof_bits = temp + 1;
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],temp,50,exit);
      nof_bits -= temp;
    }
    total_bits += nof_bits;
   /*
    * check order of mpls ranges
    */

    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],temp,40,exit);
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],temp2,50,exit);

    /* self range, or empty range then no check */
    if (rng_indx == range_ndx || temp == temp2 || label_range_info->range.first_label == label_range_info->range.last_label)
    {
      continue;
    }
    else if (rng_indx < range_ndx)
    {
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],temp,40,exit);
      if (label_range_info->range.first_label < temp)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR, 45, exit);
      }
    }
    else if (rng_indx > range_ndx)
    {
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],temp,50,exit);
      if (label_range_info->range.last_label > temp)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR, 55, exit);
      }
    }
  }

  if (total_bits > SOC_PB_PP_MPLS_TERM_RANGES_NOF_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_range_set_verify()", range_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_label_range_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX, SOC_PB_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_range_get_verify()", range_ndx, 0);
}

/*********************************************************************
*     Set the range of MPLS labels that may be used as
 *     tunnels, and enable terminating those tables
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  )
{
  uint32
    reg_val;
  uint32
    temp;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(label_range_info);

  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO_clear(label_range_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[range_ndx],label_range_info->range.first_label,10,exit);
  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[range_ndx],label_range_info->range.last_label,20,exit);

  SOC_PB_PP_REG_GET(regs->ihp.mpls_range_configs_reg[range_ndx],reg_val, 30,exit);

  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_in_rif,label_range_info->term_info.rif,reg_val,40,exit);
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_is_pipe,temp,reg_val,50,exit);
  label_range_info->term_info.processing_type = (temp)?SOC_PB_PP_MPLS_TERM_MODEL_PIPE:SOC_PB_PP_MPLS_TERM_MODEL_UNIFORM;
  
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_cos_profile,temp,reg_val,70,exit);
  label_range_info->term_info.cos_profile = temp;

  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_range_configs_reg[range_ndx].range_in_rif_valid,temp,reg_val,80,exit);
  /* if rif not valid then set it to SOC_PB_PP_RIF_NULL */
  if (temp == 0)
  {
    label_range_info->term_info.rif= SOC_PB_PP_RIF_NULL;
  }

  label_range_info->term_info.next_prtcl = SOC_PB_PP_L3_NEXT_PRTCL_TYPE_MPLS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_range_get_unsafe()", range_ndx, 0);
}

STATIC
uint32
  soc_pb_pp_mpls_term_range_label_to_bit_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT  uint32                                 *bit_index,
    SOC_SAND_OUT  uint8                                *found
  )
{
  uint32
    rng_low=0,
    rng_high,
    rng_base,
    rng_indx;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_pb_pp_regs();

  for (rng_indx = 0; rng_indx <= SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],rng_low,10,exit);
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],rng_high,20,exit);
    if (SOC_SAND_IS_VAL_IN_RANGE(label_ndx,rng_low,rng_high))
    {
      *found = TRUE;
      break;
    }
    if (label_ndx < rng_low)
    {
      *found = FALSE;
      break;
    }
  }

  if (rng_indx > SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX)
  {
    *found = FALSE;
    goto exit;
  }

  if (*found)
  {
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_base_reg[rng_indx],rng_base,30,exit);
    /* skip low of the range */
    *bit_index = label_ndx - rng_base;
  }
  else
  {
    res = soc_pb_pp_mpls_term_range_label_to_bit_index_get(
            unit,
            rng_low,
            bit_index,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_range_label_to_bit_index_get()", label_ndx, *bit_index);
}
/*********************************************************************
*     Enable / Disable termination of each label in the MPLS
 *     tunnels range
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_range_terminated_label_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  )
{
  uint32
    temp,
    rng_low=0,
    rng_high,
    rng_base,
    rng_indx,
    nof_bits=0;
  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA
    mpls_tunnel_termination_valid_tbl;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  for (rng_indx = 0; rng_indx <= SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],rng_low,10,exit);
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],rng_high,20,exit);
    if (SOC_SAND_IS_VAL_IN_RANGE(label_ndx,rng_low,rng_high))
    {
      break;
    }
  }
  /* if no match in 0-2 ranges then error*/
  SOC_SAND_ERR_IF_ABOVE_MAX(rng_indx, SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX, SOC_PB_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR, 30, exit);

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_base_reg[rng_indx],rng_base,30,exit);

  /* skip low of the range */
  nof_bits = label_ndx - rng_base;

  res = soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_get_unsafe(
          unit,
          nof_bits/16,
          &mpls_tunnel_termination_valid_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  temp = SOC_SAND_NUM2BOOL(is_terminated_label);

  /* set the bit to one/zero according to param */
  res = soc_sand_set_field(
          &(mpls_tunnel_termination_valid_tbl.mpls_tunnel_termination_valid),
          nof_bits%16,
          nof_bits%16,
          temp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_set_unsafe(
          unit,
          nof_bits/16,
          &mpls_tunnel_termination_valid_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_range_terminated_label_set_unsafe()", label_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_range_terminated_label_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_range_terminated_label_set_verify()", label_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_range_terminated_label_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_range_terminated_label_get_verify()", label_ndx, 0);
}

/*********************************************************************
*     Enable / Disable termination of each label in the MPLS
 *     tunnels range
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_range_terminated_label_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT uint8                                 *is_terminated_label
  )
{
  uint32
    temp,
    rng_low=0,
    rng_high,
    rng_base,
    rng_indx,
    nof_bits=0;
  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA
    mpls_tunnel_termination_valid_tbl;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_terminated_label);

  regs = soc_pb_pp_regs();

  for (rng_indx = 0; rng_indx <= SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],rng_low,10,exit);
    SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],rng_high,20,exit);
    if (SOC_SAND_IS_VAL_IN_RANGE(label_ndx,rng_low,rng_high))
    {
      break;
    }
  }
  /* if no match in 0-2 ranges then error*/
  SOC_SAND_ERR_IF_ABOVE_MAX(rng_indx, SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX, SOC_PB_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR, 30, exit);

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_base_reg[rng_indx],rng_base,30,exit);

  /* skip low of the range */
  nof_bits = label_ndx - rng_base;

  res = soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_get_unsafe(
          unit,
          nof_bits/16,
          &mpls_tunnel_termination_valid_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  temp = SOC_SAND_GET_BIT(mpls_tunnel_termination_valid_tbl.mpls_tunnel_termination_valid, nof_bits%16);

  *is_terminated_label = SOC_SAND_NUM2BOOL(temp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_range_terminated_label_get_unsafe()", label_ndx, 0);
}

/*********************************************************************
*     Processing information for the MPLS reserved labels.
 *     MPLS Reserved labels are from 0 to 15.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_labels_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_labels_global_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO, reserved_labels_info, 10, exit);
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_labels_global_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_labels_global_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Processing information for the MPLS reserved labels.
 *     MPLS Reserved labels are from 0 to 15.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_labels_info);

  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(reserved_labels_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_labels_global_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the per-reserved label processing information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_label_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp,
    rif_val,
    reg_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(label_info);
  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_value_configs_reg[label_ndx],reg_val,10,exit);

  rif_val = (label_info->term_info.rif == SOC_PB_PP_RIF_NULL)?SOC_PB_PP_RIF_NULL_VAL:label_info->term_info.rif;
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_in_rif,rif_val,reg_val,20,exit);
  temp = (label_info->term_info.processing_type == SOC_PB_PP_MPLS_TERM_MODEL_PIPE);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_model_is_pipe,temp,reg_val,30,exit);

  temp = label_info->term_info.cos_profile;
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_cos_profile,temp,reg_val,40,exit);

  temp = (label_info->term_info.rif != SOC_PB_PP_RIF_NULL);
  SOC_PB_PP_FLD_TO_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_in_rif_valid,temp,reg_val,50,exit);

  SOC_PB_PP_REG_SET(regs->ihp.mpls_label_value_configs_reg[label_ndx],reg_val, 60,exit);

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_value_bos_action_index_reg,reg_val,70,exit);
  temp = label_info->bos_action.trap_code - SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0;
  res = soc_sand_bitstream_set_any_field(&temp,label_ndx*2,2,&reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
  SOC_PB_PP_REG_SET(regs->ihp.mpls_label_value_bos_action_index_reg,reg_val,77,exit);
  
  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_value_no_bos_action_index_reg,reg_val,80,exit);
  temp = label_info->non_bos_action.trap_code - SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0;
  res = soc_sand_bitstream_set_any_field(&temp,label_ndx*2,2,&reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  SOC_PB_PP_REG_SET(regs->ihp.mpls_label_value_no_bos_action_index_reg,reg_val,95,exit);

  /* set if has ip next to label */
  if (label_info->term_info.next_prtcl == SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV4 || label_info->term_info.next_prtcl == SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV6)
  {
    temp = 1;
  }
  else
  {
    temp = 0;
  }
  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_has_ip_reg,reg_val,100,exit);
  res = soc_sand_bitstream_set_any_field(&(temp),label_ndx,1,&reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  SOC_PB_PP_REG_SET(regs->ihp.mpls_label_has_ip_reg,reg_val,110,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_label_info_set_unsafe()", label_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_reserved_label_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO, label_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_label_info_set_verify()", label_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_reserved_label_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_label_info_get_verify()", label_ndx, 0);
}

/*********************************************************************
*     Set the per-reserved label processing information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_label_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp,
    reg_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(label_info);

  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_clear(label_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_value_configs_reg[label_ndx],reg_val,10,exit);

  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_in_rif,label_info->term_info.rif,reg_val,20,exit);
  
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_model_is_pipe,temp,reg_val,30,exit);
  label_info->term_info.processing_type = (temp)?SOC_PB_PP_MPLS_TERM_MODEL_PIPE:SOC_PB_PP_MPLS_TERM_MODEL_UNIFORM;

  
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_cos_profile,temp,reg_val,40,exit);
  label_info->term_info.cos_profile = temp;

  temp = (label_info->term_info.rif != SOC_PB_PP_RIF_NULL);
  SOC_PB_PP_FLD_FROM_REG(regs->ihp.mpls_label_value_configs_reg[label_ndx].mpls_value_in_rif_valid,temp,reg_val,50,exit);
  /* if rif not valid then set it to SOC_PB_PP_RIF_NULL */
  if (temp == 0)
  {
    label_info->term_info.rif= SOC_PB_PP_RIF_NULL;
  }
  
  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_value_bos_action_index_reg,reg_val,70,exit);
  res = soc_sand_bitstream_get_any_field(&reg_val,label_ndx*2,2,&temp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
  label_info->bos_action.trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0 + temp;

  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_value_no_bos_action_index_reg,reg_val,80,exit);
  res = soc_sand_bitstream_get_any_field(&reg_val,label_ndx*2,2,&temp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  label_info->non_bos_action.trap_code = SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0 + temp;

  /* get if has ip next to label */
  temp = 0;
  SOC_PB_PP_REG_GET(regs->ihp.mpls_label_has_ip_reg,reg_val,100,exit);
  res = soc_sand_bitstream_get_any_field(&reg_val,label_ndx,1,&(temp));
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  if (temp == 1)
  {
    label_info->term_info.next_prtcl = SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV4;
  }
  else
  {
    label_info->term_info.next_prtcl = SOC_PB_PP_L3_NEXT_PRTCL_TYPE_MPLS;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_reserved_label_info_get_unsafe()", label_ndx, 0);
}

/*********************************************************************
*     Diagnostic tool: Indicates the terminated MPLS label
 *     Ids.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_encountered_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                *nof_encountered_labels,
    SOC_SAND_OUT uint32                                  *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id
  )
{
  uint32
    rng_low=0,
    rng_high,
    rng_base=0,
    rng_indx,
    bit_index=0xffffffff, /* Assigning all 1s, in case we accidently access it without assigning real value */
    prev_bit,
    enc_lbls=0;
  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA
    mpls_label_range_encountered_tbl;
  uint8
    found,
    encountered;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nof_encountered_labels);
  SOC_SAND_CHECK_NULL_INPUT(encountered_labels);
  SOC_SAND_CHECK_NULL_INPUT(next_label_id);

  regs = soc_pb_pp_regs();

  res = soc_pb_pp_mpls_term_range_label_to_bit_index_get(
          unit,
          first_label_ndx,
          &bit_index,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mpls_label_range_encountered_tbl.mpls_label_range_encountered = 0;
  if (!found)
  {
    SOC_SAND_TBL_ITER_SET_END(next_label_id);
    goto exit;
  }

  prev_bit = 0xFFFFFFFF;
  for (;bit_index < SOC_PB_PP_MPLS_TERM_RANGES_NOF_BITS; ++bit_index)
  {
    if (prev_bit / 16 != bit_index/16)
    {
      res = soc_pb_pp_ihp_mpls_label_range_encountered_tbl_get_unsafe(
              unit,
              bit_index/16,
              &mpls_label_range_encountered_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    prev_bit = bit_index;

    encountered = SOC_SAND_GET_BIT(mpls_label_range_encountered_tbl.mpls_label_range_encountered,bit_index%16);
    if (!encountered)
    {
      continue;
    }

    /* what label is this*/
    for (rng_indx = 0; rng_indx <= SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
    {
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_base_reg[rng_indx],rng_base,10,exit);
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_low_reg[rng_indx],rng_low,20,exit);
      SOC_PB_PP_REG_GET(regs->ihp.mpls_label_range_high_reg[rng_indx],rng_high,30,exit);
      if (rng_low <= bit_index + rng_base && bit_index + rng_base <= rng_high)
      {
        break;
      }
    }
    if (rng_indx > SOC_PB_PP_MPLS_TERM_RANGE_NDX_MAX)
    {
      continue;
    }
    encountered_labels[enc_lbls++] = rng_base + bit_index;
  }

  SOC_SAND_TBL_ITER_SET_END(next_label_id);

  *nof_encountered_labels = enc_lbls;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_encountered_entries_get_block_unsafe()", first_label_ndx, 0);
}

uint32
  soc_pb_pp_mpls_term_encountered_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(first_label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_PB_PP_STRUCT_VERIFY(uint32, nof_encountered_labels, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_encountered_entries_get_block_verify()", first_label_ndx, 0);
}

/*********************************************************************
*     Set information of resolving COS parameters whenever
 *     MPLS label is terminated.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_COS_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_cos_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_cos_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_COS_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_COS_INFO, term_cos_info, 10, exit);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_cos_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_COS_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_cos_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set information of resolving COS parameters whenever
 *     MPLS label is terminated.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_COS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_cos_info);

  SOC_PB_PP_MPLS_TERM_COS_INFO_clear(term_cos_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_cos_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set mapping from terminated label fields (EXP) to COS
 *     parameters TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_to_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cos_key);
  SOC_SAND_CHECK_NULL_INPUT(cos_val);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_to_cos_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_label_to_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY, cos_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL, cos_val, 20, exit);


  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_to_cos_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mpls_term_label_to_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY, cos_key, 10, exit);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_to_cos_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set mapping from terminated label fields (EXP) to COS
 *     parameters TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_to_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cos_key);
  SOC_SAND_CHECK_NULL_INPUT(cos_val);

  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL_clear(cos_val);


  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mpls_term_label_to_cos_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_mpls_term module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_mpls_term_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_mpls_term;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_mpls_term module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_mpls_term_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_mpls_term;
}

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  if (info->last_label < info->first_label)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR, 60, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_LABEL_RANGE_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_type, SOC_PB_PP_MPLS_TERM_KEY_TYPE_MAX, SOC_PB_PP_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_LKUP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->processing_type, SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_MAX, SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(info->rif, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_NULL, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, SOC_PB_PP_MPLS_TERM_COS_PROFILE_MAX, SOC_PB_PP_MPLS_TERM_COS_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  /* petraB code. Almost not in use. Ignore coveruty defects */
  /* coverity[result_independent_of_operands] */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->next_prtcl, SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_MAX, SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR, 13, exit);
  if (info->next_prtcl != SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV4 &&
      info->next_prtcl != SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV6 &&
      info->next_prtcl != SOC_PB_PP_L3_NEXT_PRTCL_TYPE_MPLS
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR, 15, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_LABEL_RANGE, &(info->range), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_INFO, &(info->term_info), 11, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, &(info->bos_action), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, &(info->non_bos_action), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_TERM_INFO, &(info->term_info), 12, exit);
  if (info->bos_action.frwrd_action_strength != 0 || info->non_bos_action.frwrd_action_strength != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 15, exit);
  }

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->bos_action.trap_code,
    SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0,
    SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_3,
    SOC_PB_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR,30,exit)


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->processing_type, SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_MAX, SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_rif, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_COS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_COS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->model, SOC_PB_PP_MPLS_TERM_MODEL_MAX, SOC_PB_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

