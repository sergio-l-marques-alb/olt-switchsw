/* $Id: pb_pp_eg_filter.c,v 1.10 Broadcom SDK $
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
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_mgmt.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_EG_FILTER_IS_MEMBER_MAX                          (4095)
#define SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_MAX                    (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define SOC_PB_PP_EG_FILTER_ACCEPT_MAX                             (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_MAX                    (SOC_PB_PP_NOF_EG_FILTER_PVLAN_PORT_TYPES-1)
#define SOC_PB_PP_EG_FILTER_ORIENTATION_MAX                        (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define SOC_PB_PP_EG_FILTER_FILTER_MASK_MAX                        (SOC_PB_PP_EG_FILTER_PORT_ENABLE_ALL)
#define SOC_PB_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_MAX          (3)

/* } */
/*************
 * MACROS    *
 *************/
/* { */
/* $Id: pb_pp_eg_filter.c,v 1.10 Broadcom SDK $
 * MACRO to check if pvlan feature is enabled
 * will be called in beginning of each API
 */
#define SOC_PB_PP_LLP_PVLAN_FEATURE_CHECK_IF_ENABLED(unit,fld_val)  \
  if(fld_val != 0)      \
  {                                                              \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_PVLAN_FEATURE_DISABLED_ERR, 150, exit);   \
  }

#define SOC_PB_PP_LLP_SPLIT_HORIZON_FEATURE_CHECK_IF_ENABLED(unit,fld_val)  \
  if(fld_val != 1)      \
  {                                                              \
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR, 150, exit);   \
  }


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
    Soc_pb_pp_procedure_desc_element_eg_filter[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_FILTER_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_eg_filter[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_EG_FILTER_IS_MEMBER_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_IS_MEMBER_OUT_OF_RANGE_ERR",
    "The parameter 'is_member' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_acc_frames_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_format_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR",
    "The parameter 'accept' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'pvlan_port_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_FILTER_PVLAN_PORT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_SPLIT_HORIZON_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_FILTER_MASK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_FILTER_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'filter_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'acceptable_frames_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_EG_PVLAN_FEATURE_DISABLED_ERR,
    "SOC_PB_PP_EG_PVLAN_FEATURE_DISABLED_ERR",
    "The private VLAN feature is disabled by operation mode. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR,
    "SOC_PB_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR",
    "The split-horizon filter feature is disabled by operation mode.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR,
    "SOC_PB_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR",
    "The split-horizon filter cannot be disable per port.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_FLTER_PORT_MTU_DISABLE_ERR,
    "SOC_PB_PP_EG_FLTER_PORT_MTU_DISABLE_ERR",
    "The MTU filter cannot be disable per port.\n\r ",
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

STATIC uint32
  soc_pb_pp_eg_filter_pvlan_type_to_hw_val(
    SOC_SAND_IN SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE pvlan_type
  )
{
  switch(pvlan_type)
  {
  case SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED:
    return 1;
  case SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY:
    return 3;
  default:
  case SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS:
    return 0;
  }
}

uint32
  soc_pb_pp_eg_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    reg_val=0,
    in_val,
    out_val;
  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE
    in_port_type,
    out_port_type,
    bit_indx;

  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  regs = soc_pb_pp_regs();


  for(in_port_type = SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS; in_port_type < SOC_PB_PP_NOF_EG_FILTER_PVLAN_PORT_TYPES; ++in_port_type)
  {
    for(out_port_type = SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS; out_port_type < SOC_PB_PP_NOF_EG_FILTER_PVLAN_PORT_TYPES; ++out_port_type)
    {

      if(
          (in_port_type == SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED && out_port_type != SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS) ||
          (out_port_type == SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED && in_port_type != SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS)
        )
      {

        in_val = soc_pb_pp_eg_filter_pvlan_type_to_hw_val(in_port_type);
        out_val = soc_pb_pp_eg_filter_pvlan_type_to_hw_val(out_port_type);

        bit_indx = (in_val * 4) + out_val;
        SOC_SAND_SET_BIT(reg_val,1,bit_indx);
      }
    }
  }
  SOC_PB_PP_REG_SET(regs->egq.private_vlan_filter_reg, reg_val, 10, exit);


  /* set SPLI horizon filtering */
  /* default filter hub to hub */
  reg_val = 8;

  SOC_PB_PP_FLD_SET(regs->egq.ehpgeneral_settings_reg.split_horizon_filter, reg_val, 20, exit);

  /* disable acceptable frame type filtering */
  reg_val = 0xFFFFFFFF;
  SOC_PB_PP_REG_SET(regs->egq.acceptable_frame_type_reg[0], reg_val, 10, exit);
  SOC_PB_PP_REG_SET(regs->egq.acceptable_frame_type_reg[1], reg_val, 20, exit);
  SOC_PB_PP_REG_SET(regs->egq.acceptable_frame_type_reg[2], reg_val, 20, exit);
  SOC_PB_PP_REG_SET(regs->egq.acceptable_frame_type_reg[3], reg_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_init_unsafe()", 0, 0);
}



STATIC uint32
  soc_pb_pp_eg_filter_pvlan_type_from_hw_val(
    SOC_SAND_IN uint32 pvlan_val
  )
{
  switch(pvlan_val)
  {
  case 1:
    return SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED;
  case 3:
    return SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY;
  default:
  case 0:
    return SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS;
  }
}

/*********************************************************************
*     Sets out-port filtering information, including which
 *     filtering to perform on this specific out-port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    filters;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(port_info);

  filters = port_info->filter_mask;

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(unit, out_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	Acceptable Frames Profile
   */
  egq_pp_ppct_tbl.acceptable_frame_type_profile = port_info->acceptable_frames_profile;
  epni_pp_pct_tbl.acceptable_frame_type_profile = egq_pp_ppct_tbl.acceptable_frame_type_profile;
  /*
   *	Filter Enablers Mask
   */
  egq_pp_ppct_tbl.disable_filtering = (filters == SOC_PB_PP_EG_FILTER_PORT_ENABLE_NONE)?TRUE:FALSE;
  epni_pp_pct_tbl.disable_filter = egq_pp_ppct_tbl.disable_filtering;

  /* don't update as filter in EPNI only, no filter at egq
  egq_pp_ppct_tbl.egress_vsi_filtering_enable
  */

  epni_pp_pct_tbl.egress_vsi_filter_enable =
    SOC_PB_PP_MASK_IS_ON(filters, SOC_PB_PP_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP);

  egq_pp_ppct_tbl.enable_src_equal_dst_filter =
    SOC_PB_PP_MASK_IS_ON(filters, SOC_PB_PP_EG_FILTER_PORT_ENABLE_SAME_INTERFACE);

  egq_pp_ppct_tbl.unknown_uc_da_action_filter =
    SOC_PB_PP_MASK_IS_ON(filters, SOC_PB_PP_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA);

  egq_pp_ppct_tbl.unknown_mc_da_action_filter =
    SOC_PB_PP_MASK_IS_ON(filters, SOC_PB_PP_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA);

  egq_pp_ppct_tbl.unknown_bc_da_action_filter =
    SOC_PB_PP_MASK_IS_ON(filters, SOC_PB_PP_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA);

  epni_pp_pct_tbl.egress_stp_filter_enable =
    SOC_PB_PP_MASK_IS_ON(filters, SOC_PB_PP_EG_FILTER_PORT_ENABLE_STP);

  res = soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(unit, out_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_epni_pp_pct_tbl_set_unsafe(unit, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_info_set_unsafe()", out_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_port_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_FILTER_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_info_set_verify()", out_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_port_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_info_get_verify()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets out-port filtering information, including which
 *     filtering to perform on this specific out-port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    filters;
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PB_PP_EG_FILTER_PORT_INFO_clear(port_info);

  filters = 0;

  res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(unit, out_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /*
   *	Acceptable Frames Profile
   */
  port_info->acceptable_frames_profile = egq_pp_ppct_tbl.acceptable_frame_type_profile;
  /*
   *	Filter Enablers Mask
   */
  if (egq_pp_ppct_tbl.disable_filtering)
  {
    port_info->filter_mask = SOC_PB_PP_EG_FILTER_PORT_ENABLE_NONE;
    goto exit;
  }
  if (epni_pp_pct_tbl.egress_vsi_filter_enable)
  {
    filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP;
  }

  if (egq_pp_ppct_tbl.enable_src_equal_dst_filter)
  {
    filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_SAME_INTERFACE;
  }

  if (egq_pp_ppct_tbl.unknown_uc_da_action_filter)
  {
    filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA;
  }

  if (egq_pp_ppct_tbl.unknown_mc_da_action_filter)
  {
    filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA;
  }

  if (egq_pp_ppct_tbl.unknown_bc_da_action_filter)
  {
    filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA;
  }
  if (epni_pp_pct_tbl.egress_stp_filter_enable)
  {
    filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_STP;
  }
  if (epni_pp_pct_tbl.egress_stp_filter_enable)
  {
    filters |= SOC_PPD_EG_FILTER_PORT_ENABLE_STP;
  }
  /* as it always enabled */
  filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON;
  filters |= SOC_PB_PP_EG_FILTER_PORT_ENABLE_MTU;


  port_info->filter_mask = filters;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_info_get_unsafe()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Set whether outgoing local
 *     port belongs to the VSI. Packets transmitted out through
 *     a port that is not member of the packet's VSI are
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_port_membership_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{
  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
    egq_vsi_membership_tbl;
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_UNSAFE);

  res = soc_pb_pp_egq_vsi_membership_tbl_get_unsafe(unit, vsid_ndx, &egq_vsi_membership_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_BOOL2NUM(is_member);
  res = soc_sand_bitstream_set_any_field(&tmp,out_port_ndx,1,egq_vsi_membership_tbl.vsi_membership);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_egq_vsi_membership_tbl_set_unsafe(unit, vsid_ndx, &egq_vsi_membership_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_port_membership_set_unsafe()", vsid_ndx, out_port_ndx);
}

uint32
  soc_pb_pp_eg_filter_vsi_port_membership_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, SOC_PB_PP_RIF_VSI_ID_MAX, SOC_PB_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_port_membership_set_verify()", vsid_ndx, out_port_ndx);
}

uint32
  soc_pb_pp_eg_filter_vsi_port_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_port_membership_get_verify()", vsid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Set whether outgoing local
 *     port belongs to the VSI. Packets transmitted out through
 *     a port that is not member of the packet's VSI are
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_port_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
    egq_vsi_membership_tbl;
  uint32
    tmp=0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  res = soc_pb_pp_egq_vsi_membership_tbl_get_unsafe(unit, vsid_ndx, &egq_vsi_membership_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(egq_vsi_membership_tbl.vsi_membership,out_port_ndx,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *is_member = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_port_membership_get_unsafe()", vsid_ndx, out_port_ndx);
}


uint32
  soc_pb_pp_eg_filter_vsi_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx    
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_membership_get_verify()", vsid_ndx, 0);
}

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_vsi_membership_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_eg_filter_vsi_membership_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
    egq_vsi_membership_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET_UNSAFE);

  res = soc_pb_pp_egq_vsi_membership_tbl_get_unsafe(unit, vsid_ndx, &egq_vsi_membership_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  sal_memset(ports, 0, SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE * sizeof(uint32));
  sal_memcpy(ports, &egq_vsi_membership_tbl, sizeof(egq_vsi_membership_tbl));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_membership_get_unsafe()", vsid_ndx, 0);
}

/*********************************************************************
*     Sets egress CVID membership (relevant for CEP ports).
 *     Sets whether outgoing local port belongs to the CVID.
 *     Packets transmitted out through a port that is not
 *     member of the packet's CVID are filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_cvid_port_membership_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_cvid_port_membership_set_unsafe()", cvid_ndx, out_port_ndx);
}

uint32
  soc_pb_pp_eg_filter_cvid_port_membership_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cvid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(is_member, SOC_PB_PP_EG_FILTER_IS_MEMBER_MAX, SOC_PB_PP_EG_FILTER_IS_MEMBER_OUT_OF_RANGE_ERR, 30, exit); */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_cvid_port_membership_set_verify()", cvid_ndx, out_port_ndx);
}

uint32
  soc_pb_pp_eg_filter_cvid_port_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cvid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_cvid_port_membership_get_verify()", cvid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets egress CVID membership (relevant for CEP ports).
 *     Sets whether outgoing local port belongs to the CVID.
 *     Packets transmitted out through a port that is not
 *     member of the packet's CVID are filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_cvid_port_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_cvid_port_membership_get_unsafe()", cvid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets acceptable frame type on outgoing port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO               *eg_prsr_out_key,
    SOC_SAND_IN  uint8                          accept
  )
{
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    entry_offset,
    reg_val;
  SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA
    epni_llvp_tbl;
  uint32
    port_acc_frames_profile_ndx;
  SOC_PB_PP_EG_FILTER_PORT_INFO
    port_info;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);

  regs = soc_pb_pp_regs();

  res = soc_pb_pp_eg_filter_port_info_get_unsafe(unit, out_port_ndx,&port_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_acc_frames_profile_ndx = port_info.acceptable_frames_profile;

  /*egq*/
  entry_offset = SOC_PB_PP_TBL_EGQ_ACC_FRM_ENTRY_OFFSET(
                   port_acc_frames_profile_ndx,
                   eg_prsr_out_key->outer_tpid,
                   eg_prsr_out_key->is_outer_prio,
                   eg_prsr_out_key->inner_tpid
                 );

  SOC_PB_PP_REG_GET(regs->egq.acceptable_frame_type_reg[entry_offset/32], reg_val, 10, exit);
  SOC_SAND_SET_BIT(reg_val,accept,entry_offset%32);
  SOC_PB_PP_REG_SET(regs->egq.acceptable_frame_type_reg[entry_offset/32], reg_val, 20, exit);

  /* EPNI, get llvp-profile */
  res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  entry_offset = SOC_PB_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(
                   epni_pp_pct_tbl.llvp_profile,
                   eg_prsr_out_key->outer_tpid,
                   eg_prsr_out_key->is_outer_prio,
                   eg_prsr_out_key->inner_tpid
                 );

  res = soc_pb_pp_epni_llvp_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  entry_offset = (epni_llvp_tbl.llvp_incoming_tag_format << 2) + port_acc_frames_profile_ndx;

  SOC_PB_PP_REG_GET(regs->epni.acceptable_frame_type_reg[entry_offset/32], reg_val, 10, exit);
  SOC_SAND_SET_BIT(reg_val,accept,entry_offset%32);
  /* omitted as EPNI and EGQ make same check 
  SOC_PB_PP_REG_SET(regs->epni.acceptable_frame_type_reg[entry_offset/32], reg_val, 20, exit);*/


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_acceptable_frames_set_unsafe()", out_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_INFO, eg_prsr_out_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_acceptable_frames_set_verify()", out_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *eg_prsr_out_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_INFO, eg_prsr_out_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_acceptable_frames_get_verify()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on outgoing port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                          *accept
  )
{
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    entry_offset,
    tmp,
    port_acc_frames_profile_ndx,
    reg_val;
  SOC_PB_PP_EG_FILTER_PORT_INFO
    port_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);


  res = soc_pb_pp_eg_filter_port_info_get_unsafe(unit, out_port_ndx,&port_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_acc_frames_profile_ndx = port_info.acceptable_frames_profile;


  regs = soc_pb_pp_regs();
  /*egq*/
  entry_offset = SOC_PB_PP_TBL_EGQ_ACC_FRM_ENTRY_OFFSET(
                   port_acc_frames_profile_ndx,
                   eg_prsr_out_key->outer_tpid,
                   eg_prsr_out_key->is_outer_prio,
                   eg_prsr_out_key->inner_tpid
                 );

  SOC_PB_PP_REG_GET(regs->egq.acceptable_frame_type_reg[entry_offset/32], reg_val, 10, exit);
  tmp = SOC_SAND_GET_BIT(reg_val,entry_offset%32);
  *accept = SOC_SAND_NUM2BOOL(tmp);
  /* epni filtering, is disabled as for bridged EGQ is
     same functionality, for routed packets not needed*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_acceptable_frames_get_unsafe()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pep_acceptable_frames_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_PEP_KEY, pep_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_MAX, SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(accept, SOC_PB_PP_EG_FILTER_ACCEPT_MAX, SOC_PB_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pep_acceptable_frames_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_PEP_KEY, pep_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_MAX, SOC_PB_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pep_acceptable_frames_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets acceptable frame type for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_OUT uint8                          *accept
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pep_acceptable_frames_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Private VLAN (PVLAN) port type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pvlan_port_type_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type
  )
{
  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  uint32
    fld_val,
    entry_offset,
    phy_sys_port_ndx,
    tmp,
    entry_index;
  uint32
    fap_id,
    local_port,
    sys_fap_id_self;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET_UNSAFE);
  regs = soc_pb_pp_regs();

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_PB_PP_FLD_GET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode, fld_val, 5, exit);

  SOC_PB_PP_LLP_PVLAN_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  phy_sys_port_ndx = SOC_PB_PP_SAND_SYS_PORT_ENCODE(src_sys_port_ndx);

  entry_index = phy_sys_port_ndx >> 2;
  entry_offset = SOC_SAND_GET_BITS_RANGE(phy_sys_port_ndx,1,0);

  res = soc_pb_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = soc_pb_pp_eg_filter_pvlan_type_to_hw_val(pvlan_port_type);

  res = soc_sand_set_field(
          &aux_table_tbl_data.auxtable,
          2*entry_offset+1,
          2*entry_offset,
          tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_egq_aux_table_tbl_set_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 /*
  * configure local port, if relevant
  */
  if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT)
  {
    res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
            unit,
            src_sys_port_ndx->sys_id,
            &fap_id,
            &local_port
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_mgmt_system_fap_id_get_unsafe(
            unit,
            &sys_fap_id_self
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    if (sys_fap_id_self != fap_id)
    {
      goto exit;
    }

    res = soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(unit, local_port, &egq_pp_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    egq_pp_ppct_tbl.pvlan_port_type = tmp;

    res = soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(unit, local_port, &egq_pp_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pvlan_port_type_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_filter_pvlan_port_type_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET_VERIFY);

  res = soc_pb_pp_SAND_PP_SYS_PORT_ID_verify(src_sys_port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(pvlan_port_type, SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_MAX, SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pvlan_port_type_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_filter_pvlan_port_type_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET_VERIFY);

  res = soc_pb_pp_SAND_PP_SYS_PORT_ID_verify(src_sys_port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pvlan_port_type_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the Private VLAN (PVLAN) port type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pvlan_port_type_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE    *pvlan_port_type
  )
{
  uint32
    fld_val,
    entry_offset,
    tmp,
    phy_sys_port_ndx,
    entry_index;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pvlan_port_type);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode, fld_val, 5, exit);

  SOC_PB_PP_LLP_PVLAN_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  phy_sys_port_ndx = SOC_PB_PP_SAND_SYS_PORT_ENCODE(src_sys_port_ndx);

  entry_index = phy_sys_port_ndx >> 2;
  entry_offset = SOC_SAND_GET_BITS_RANGE(phy_sys_port_ndx,1,0);

  res = soc_pb_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BITS_RANGE(aux_table_tbl_data.auxtable,2*entry_offset+1,2*entry_offset);

  *pvlan_port_type = soc_pb_pp_eg_filter_pvlan_type_from_hw_val(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pvlan_port_type_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets the orientation of out-AC, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    fld_val,
    entry_offset,
    tmp,
    entry_index;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode, fld_val, 5, exit);

  SOC_PB_PP_LLP_SPLIT_HORIZON_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  entry_index = out_ac_ndx >> 3;
  entry_offset = SOC_SAND_GET_BITS_RANGE(out_ac_ndx,2,0);

  res = soc_pb_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = (orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)?1:0;

  res = soc_sand_set_field(
          &aux_table_tbl_data.auxtable,
          entry_offset+1,
          entry_offset,
          tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_egq_aux_table_tbl_set_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(orientation, SOC_PB_PP_EG_FILTER_ORIENTATION_MAX, SOC_PB_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set_verify()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get_verify()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-AC, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{
  uint32
    fld_val,
    entry_offset,
    tmp,
    entry_index;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.general_pp_config_reg.auxiliary_data_table_mode, fld_val, 5, exit);

  SOC_PB_PP_LLP_SPLIT_HORIZON_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  entry_index = out_ac_ndx >> 3;
  entry_offset = SOC_SAND_GET_BITS_RANGE(out_ac_ndx,2,0);

  res = soc_pb_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BITS_RANGE(aux_table_tbl_data.auxtable,entry_offset+1,entry_offset);

  *orientation = (tmp == 1)?SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
 * Not supported for Soc_petraB.
 * see soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_set_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
 * Not supported for Soc_petraB.
 * see soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                        lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE);

    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe()", lif_eep_ndx, 0);
}

uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                        lif_eep_ndx
  )
{     
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY);

    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_get_verify()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_eg_filter module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_filter_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_eg_filter;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_eg_filter module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_filter_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_eg_filter;
}
uint32
  SOC_PB_PP_EG_FILTER_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->filter_mask != SOC_PB_PP_EG_FILTER_PORT_ENABLE_ALL)
  {
    /* Petra b code. Almost not in use. Ignore coverity defects */
    /* coverity[result_independent_of_operands] */
    SOC_SAND_ERR_IF_ABOVE_MAX(info->filter_mask, SOC_PB_PP_EG_FILTER_FILTER_MASK_MAX, SOC_PB_PP_EG_FILTER_FILTER_MASK_OUT_OF_RANGE_ERR, 10, exit);

    if ((info->filter_mask & SOC_PB_PP_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON) == 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR, 20, exit);
    }
    if ((info->filter_mask & SOC_PB_PP_EG_FILTER_PORT_ENABLE_MTU) == 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_FLTER_PORT_MTU_DISABLE_ERR, 30, exit);
    }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->acceptable_frames_profile, SOC_PB_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_MAX, SOC_PB_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_FILTER_PORT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

