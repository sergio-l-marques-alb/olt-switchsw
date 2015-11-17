/* $Id: soc_pb_stack.c,v 1.6 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_stack.h>

#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_STACK_LOCAL_STACK_PORT_NDX_MAX                      (SOC_SAND_UINT_MAX)

#define SOC_PB_STACK_MAX_NOF_TM_DOMAINS_MAX                        (SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16)
#define SOC_PB_STACK_MY_TM_DOMAIN_MAX                              (15)
#define SOC_PB_STACK_PEER_TM_DOMAIN_MAX                            (15)

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 *  When a DSS contains more than one TM domain the FTMH extension must be present, as the
 *  destination system port is required. The FTMH extension is built with the destination system
 *  port at the ingress and remains constant throughout the packet's travel in the DSS, but the
 *  FTMH's OTM-Port field changes as the packet crosses the fabrics of various TM domains.
 */

/*********************************************************************
*     set the global information of the stacking module,
 *     including whether stacking is supported, maximum number
 *     of supported TM-domains in the system and the TM-domain
 *     of this device
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stack_global_info_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_STACK_GLBL_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK,
    ftmh_ext,
    ftmh_lb_key_ext_enable,
    ftmh_lb_key_ext_mode;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_GLOBAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /*
   *  Set the device's TM domain
   */
  SOC_PB_FLD_SET(regs->ihb.tm_domain_reg.tm_domain, info->my_tm_domain, 10, exit);

  SOC_PB_FLD_GET(regs->eci.system_headers_configuration_0_reg.ftmh_ext, ftmh_ext, 20, exit);

  /*
   *  Enable the LB extension according to the number of TM domains
   */
  switch (info->max_nof_tm_domains)
  {
  case SOC_PB_STACK_MAX_NOF_TM_DOMAINS_8:
    ftmh_ext               = 0x2;
    ftmh_lb_key_ext_enable = 0x1;
    ftmh_lb_key_ext_mode   = 0x1;
    break;

  case SOC_PB_STACK_MAX_NOF_TM_DOMAINS_16:
    ftmh_ext               = 0x2;
    ftmh_lb_key_ext_enable = 0x1;
    ftmh_lb_key_ext_mode   = 0x0;
    break;

  case SOC_PB_STACK_MAX_NOF_TM_DOMAINS_1:
  default:
    ftmh_lb_key_ext_enable = 0x0;
    ftmh_lb_key_ext_mode   = 0x0;
    break;
  }

  SOC_PB_FLD_SET(regs->eci.system_headers_configuration_0_reg.ftmh_ext,               ftmh_ext,               30, exit);
  SOC_PB_FLD_SET(regs->eci.system_headers_configuration_0_reg.ftmh_lb_key_ext_enable, ftmh_lb_key_ext_enable, 40, exit);
  SOC_PB_FLD_SET(regs->eci.system_headers_configuration_0_reg.ftmh_lb_key_ext_mode,   ftmh_lb_key_ext_mode,   50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_global_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_stack_global_info_set_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_STACK_GLBL_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_GLOBAL_INFO_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_STACK_GLBL_INFO, info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_global_info_set_verify()", 0, 0);
}

uint32
  soc_pb_stack_global_info_get_verify(
    SOC_SAND_IN  int                unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_GLOBAL_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_global_info_get_verify()", 0, 0);
}

/*********************************************************************
*     set the global information of the stacking module,
 *     including whether stacking is supported, maximum number
 *     of supported TM-domains in the system and the TM-domain
 *     of this device
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stack_global_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_STACK_GLBL_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK,
    tm_domain,
    ftmh_lb_key_ext_enable,
    ftmh_lb_key_ext_mode;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STACK_GLBL_INFO_clear(info);

  regs = soc_petra_regs();

  /*
   *  Get the device's TM domain
   */
  SOC_PB_FLD_GET(regs->ihb.tm_domain_reg.tm_domain, tm_domain, 10, exit);
  info->my_tm_domain = tm_domain;

  /*
   *  Get the number of TM domains according to the LB extension
   */
  SOC_PB_FLD_GET(regs->eci.system_headers_configuration_0_reg.ftmh_lb_key_ext_enable, ftmh_lb_key_ext_enable, 20, exit);
  SOC_PB_FLD_GET(regs->eci.system_headers_configuration_0_reg.ftmh_lb_key_ext_mode,   ftmh_lb_key_ext_mode,   30, exit);

  if (ftmh_lb_key_ext_enable == 0x1)
  {
    if (ftmh_lb_key_ext_mode == 0x1)
    {
      info->max_nof_tm_domains = SOC_PB_STACK_MAX_NOF_TM_DOMAINS_8;
    }
    else
    {
      info->max_nof_tm_domains = SOC_PB_STACK_MAX_NOF_TM_DOMAINS_16;
    }
  }
  else
  {
    info->max_nof_tm_domains = SOC_PB_STACK_MAX_NOF_TM_DOMAINS_1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_global_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set how to distribute and prune packets sent through
 *     this stacking port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stack_port_distribution_info_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                local_stack_port_ndx,
    SOC_SAND_IN  SOC_PB_STACK_PORT_DISTR_INFO *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
    first_appear;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    tbl_data;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_PORT_DISTRIBUTION_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);

  /*
   *  Set the pruning bitmap
   */
  res = soc_pb_sw_db_multiset_add(
          unit,
          SOC_PB_SW_DB_MULTI_SET_STK_PRUNE_BMP,
          distribution_info->prun_bmp[0],
          &data_indx,
          &first_appear,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (success != SOC_SAND_SUCCESS)
  {
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  profile_ndx.port_ndx = local_stack_port_ndx;
  profile_params.eg_profile = data_indx == 0 ? SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1
                                             : SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK2;
  res = soc_pb_profile_add(
          unit,
          SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
          &profile_ndx,
          &profile_params,
          FALSE,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (first_appear)
  {
    res = soc_pb_egr_prog_editor_stack_prune_bitmap_set(
            unit,
            data_indx + 1,
            distribution_info->prun_bmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /*
   *  Set the peer TM domain
   */
  res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
          unit,
          local_stack_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  tbl_data.peer_tm_domain_id = distribution_info->peer_tm_domain;

  res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
          unit,
          local_stack_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_port_distribution_info_set_unsafe()", local_stack_port_ndx, 0);
}

uint32
  soc_pb_stack_port_distribution_info_set_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                local_stack_port_ndx,
    SOC_SAND_IN  SOC_PB_STACK_PORT_DISTR_INFO *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_PORT_DISTRIBUTION_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_stack_port_ndx, SOC_PB_STACK_LOCAL_STACK_PORT_NDX_MAX, SOC_PB_STACK_LOCAL_STACK_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_STACK_PORT_DISTR_INFO, distribution_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_port_distribution_info_set_verify()", local_stack_port_ndx, 0);
}

uint32
  soc_pb_stack_port_distribution_info_get_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                local_stack_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_PORT_DISTRIBUTION_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_stack_port_ndx, SOC_PB_STACK_LOCAL_STACK_PORT_NDX_MAX, SOC_PB_STACK_LOCAL_STACK_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_port_distribution_info_get_verify()", local_stack_port_ndx, 0);
}

/*********************************************************************
*     Set how to distribute and prune packets sent through
 *     this stacking port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stack_port_distribution_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                local_stack_port_ndx,
    SOC_SAND_OUT SOC_PB_STACK_PORT_DISTR_INFO *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_hw_ndx;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    tbl_data;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STACK_PORT_DISTRIBUTION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_PB_STACK_PORT_DISTR_INFO_clear(distribution_info);
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);

  /*
   *  Get the port's peer TM domain
   */
  res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
          unit,
          local_stack_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  distribution_info->peer_tm_domain = tbl_data.peer_tm_domain_id;

  /*
   *  Get the pruning bitmap
   */
  profile_ndx.port_ndx = local_stack_port_ndx;
  res = soc_pb_profile_get(
          unit,
          SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
          &profile_ndx,
          &profile_params,
          &internal_hw_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_egr_prog_editor_stack_prune_bitmap_get(
          unit,
          (profile_params.eg_profile == SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1) ? 1 : 2,
          distribution_info->prun_bmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stack_port_distribution_info_get_unsafe()", local_stack_port_ndx, 0);
}

uint32
  SOC_PB_STACK_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_STACK_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->max_nof_tm_domains, SOC_PB_STACK_MAX_NOF_TM_DOMAINS_MAX, SOC_PB_STACK_MAX_NOF_TM_DOMAINS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->my_tm_domain, SOC_PB_STACK_MY_TM_DOMAIN_MAX, SOC_PB_STACK_MY_TM_DOMAIN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STACK_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_STACK_PORT_DISTR_INFO_verify(
    SOC_SAND_IN  SOC_PB_STACK_PORT_DISTR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->peer_tm_domain, SOC_PB_STACK_PEER_TM_DOMAIN_MAX, SOC_PB_STACK_PEER_TM_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PB_STACK_PRUN_BMP_LEN; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STACK_PORT_DISTR_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

