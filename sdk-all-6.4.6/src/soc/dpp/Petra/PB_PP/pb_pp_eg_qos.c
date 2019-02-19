/* $Id: pb_pp_eg_qos.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_eg_qos.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_qos.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_EG_QOS_DSCP_EXP_MAX                              (255)
#define SOC_PB_PP_EG_QOS_IN_DSCP_EXP_MAX                           (255)
#define SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_MAX                       (3)
#define SOC_PB_PP_EG_QOS_PHP_TYPE_MAX                              (SOC_PB_PP_NOF_EG_QOS_UNIFORM_PHP_TYPES-1)
#define SOC_PB_PP_EG_QOS_DP_MAX                                    (1)
#define SOC_PB_PP_EG_QOS_IPV4_OUT_EXP_NOF_BITS                     (8)
#define SOC_PB_PP_EG_QOS_IPV6_OUT_EXP_NOF_BITS                     (8)
#define SOC_PB_PP_EG_QOS_MPLS_OUT_EXP_NOF_BITS                     (3)

#define SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_KEY_ENTRY(tmp,dp) \
  ((tmp << 1) + dp)

#define PP_PB_EPNI_IPV4_EXP_TO_TOS_MAP_REGS_KEY_ENTRY(profile,out_exp) \
  ((profile << 3) + out_exp)

#define PP_PB_EPNI_IPV6_EXP_TO_TC_MAP_REGS_KEY_ENTRY(profile,out_exp) \
  ((profile << 3) + out_exp)


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
  Soc_pb_pp_procedure_desc_element_eg_qos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_QOS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_eg_qos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_EG_QOS_DSCP_EXP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_QOS_DSCP_EXP_OUT_OF_RANGE_ERR",
    "The parameter 'dscp_exp' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_QOS_IN_DSCP_EXP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_QOS_IN_DSCP_EXP_OUT_OF_RANGE_ERR",
    "The parameter 'in_dscp_exp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'exp_map_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_QOS_PHP_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_QOS_PHP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'php_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_QOS_UNIFORM_PHP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

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
  soc_pb_pp_eg_qos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    in_dscp;
  SOC_SAND_PP_DP
    dp;
  SOC_PB_PP_EG_QOS_MAP_KEY
    rmrk_key;
  SOC_PB_PP_EG_QOS_PARAMS
    eg_qos_vals;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  SOC_PB_PP_EG_QOS_MAP_KEY_clear(&rmrk_key);
  SOC_PB_PP_EG_QOS_PARAMS_clear(&eg_qos_vals);

  /* init remark mapping to be 1-1 mapping */
  for (in_dscp = 0; in_dscp <= SOC_PB_PP_EG_QOS_IN_DSCP_EXP_MAX; ++in_dscp)
  {
    rmrk_key.in_dscp_exp = in_dscp;
    eg_qos_vals.ipv4_tos = (SOC_SAND_PP_IPV4_TOS)in_dscp;
    eg_qos_vals.ipv6_tc = (SOC_SAND_PP_IPV6_TC)in_dscp;
    eg_qos_vals.mpls_exp = (SOC_SAND_PP_MPLS_EXP)in_dscp%8;

    for (dp = 0; dp <= SOC_PB_PP_EG_QOS_DP_MAX; ++dp)
    {
      rmrk_key.dp = dp;
      res = soc_pb_pp_eg_qos_params_remark_set_unsafe(
              unit,
              &rmrk_key,
              &eg_qos_vals
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets port information for egress QoS setting, including
 *     profiles used for QoS remarking.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_qos_info);
  
  soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, local_port_ndx, &tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  tbl_data.exp_map_profile  = port_qos_info->exp_map_profile;

  soc_pb_pp_epni_pp_pct_tbl_set_unsafe(unit, local_port_ndx, &tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_qos_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_QOS_PORT_INFO, port_qos_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_qos_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for egress QoS setting, including
 *     profiles used for QoS remarking.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_QOS_PORT_INFO                    *port_qos_info
  )
{
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_qos_info);

  SOC_PB_PP_EG_QOS_PORT_INFO_clear(port_qos_info);

  soc_pb_pp_epni_pp_pct_tbl_get_unsafe(unit, local_port_ndx, &tbl_data);

  port_qos_info->exp_map_profile = tbl_data.exp_map_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_port_info_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets how to remark QoS parameters upon PHP operation.
 *     When uniform pop performed the dscp_exp value is
 *     remarked.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_php_remark_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                  dscp_exp
  )
{
  uint32
    index,
    key,
    buffer[SOC_SAND_MAX(PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS,PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS)],
    tmp,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
   * soc_pb_pp_reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(php_key);

  soc_pb_pp_reg = soc_pb_pp_regs();

  
  switch ( php_key->php_type )
  {
  case SOC_PB_PP_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4:
    key = PP_PB_EPNI_IPV4_EXP_TO_TOS_MAP_REGS_KEY_ENTRY(php_key->exp_map_profile,php_key->exp);
    index = key * SOC_PB_PP_EG_QOS_IPV4_OUT_EXP_NOF_BITS;
    SOC_PB_PP_REG_BUFFER_GET(soc_pb_pp_reg->epni.ipv4_exp_to_tos_map_reg, PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS, buffer ,20, exit);
    
    tmp = dscp_exp;
    res = soc_sand_bitstream_set_any_field(&tmp, index, (SOC_PB_PP_EG_QOS_IPV4_OUT_EXP_NOF_BITS), buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    SOC_PB_PP_REG_BUFFER_SET(soc_pb_pp_reg->epni.ipv4_exp_to_tos_map_reg, PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS, buffer, 40, exit);

    break;
  case SOC_PB_PP_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6:
    key = PP_PB_EPNI_IPV6_EXP_TO_TC_MAP_REGS_KEY_ENTRY(php_key->exp_map_profile,php_key->exp);
    index = key * SOC_PB_PP_EG_QOS_IPV6_OUT_EXP_NOF_BITS;
    SOC_PB_PP_REG_BUFFER_GET(soc_pb_pp_reg->epni.ipv6_exp_to_tc_map_reg, PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS, buffer ,50, exit);

    tmp = dscp_exp;
    res = soc_sand_bitstream_set_any_field(&tmp, index, (SOC_PB_PP_EG_QOS_IPV6_OUT_EXP_NOF_BITS), buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    SOC_PB_PP_REG_BUFFER_SET(soc_pb_pp_reg->epni.ipv6_exp_to_tc_map_reg, PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS, buffer, 70, exit);
    break;
  default:
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_php_remark_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_qos_params_php_remark_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                  dscp_exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_QOS_PHP_REMARK_KEY, php_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dscp_exp, SOC_PB_PP_EG_QOS_DSCP_EXP_MAX, SOC_PB_PP_EG_QOS_DSCP_EXP_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_php_remark_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_qos_params_php_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_QOS_PHP_REMARK_KEY, php_key, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_php_remark_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets how to remark QoS parameters upon PHP operation.
 *     When uniform pop performed the dscp_exp value is
 *     remarked.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_php_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_OUT uint32                                  *dscp_exp
  )
{
  uint32
    index,
    key,
    buffer[SOC_SAND_MAX(PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS,PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS)],
    tmp = 0,
    res = SOC_SAND_OK;

  SOC_PB_PP_REGS
    * soc_pb_pp_reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_PHP_REMARK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(php_key);
  SOC_SAND_CHECK_NULL_INPUT(dscp_exp);

  soc_pb_pp_reg = soc_pb_pp_regs();
  
  switch (php_key->php_type)
  {
  case SOC_PB_PP_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4:
    key = PP_PB_EPNI_IPV4_EXP_TO_TOS_MAP_REGS_KEY_ENTRY(php_key->exp_map_profile,php_key->exp);
    index = key * SOC_PB_PP_EG_QOS_IPV4_OUT_EXP_NOF_BITS;

    SOC_PB_PP_REG_BUFFER_GET(soc_pb_pp_reg->epni.ipv4_exp_to_tos_map_reg, PP_PB_NOF_IPV4_EXP_TO_TOS_MAP_REGS, buffer ,20, exit);
    
    res = soc_sand_bitstream_get_any_field(buffer, index, (SOC_PB_PP_EG_QOS_IPV4_OUT_EXP_NOF_BITS), &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    break;

  case SOC_PB_PP_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6:
    key = PP_PB_EPNI_IPV6_EXP_TO_TC_MAP_REGS_KEY_ENTRY(php_key->exp_map_profile,php_key->exp);
    index = key * SOC_PB_PP_EG_QOS_IPV6_OUT_EXP_NOF_BITS;
    
    SOC_PB_PP_REG_BUFFER_GET(soc_pb_pp_reg->epni.ipv6_exp_to_tc_map_reg, PP_PB_NOF_IPV6_EXP_TO_TC_MAP_REGS, buffer ,50, exit);

    res = soc_sand_bitstream_get_any_field(buffer, index, (SOC_PB_PP_EG_QOS_IPV6_OUT_EXP_NOF_BITS), &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
  default:
    break;
  }

  *dscp_exp = tmp;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_php_remark_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Remark QoS parameters, i.e. map in-dscp/exp and DP to
 *     out-dscp/exp in order to be set in outgoing packet
 *     headers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_remark_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params
  )
{
  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA
    epni_ip_exp_map_tbl;
  uint32
    entry_offset,
    key,
    tmp,
    index,
    buffer[SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS];
  SOC_PB_PP_REGS
    * soc_pb_pp_reg;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_qos_params);

  soc_pb_pp_reg = soc_pb_pp_regs();

  /* ipv4 */
  entry_offset = SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_KEY_ENTRY_OFFSET(TRUE,in_qos_key->in_dscp_exp,in_qos_key->dp);
  res = soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_ip_exp_map_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  epni_ip_exp_map_tbl.ip_exp_map = out_qos_params->ipv4_tos;

  res = soc_pb_pp_epni_ip_exp_map_tbl_set_unsafe(
          unit,
          entry_offset,
          &epni_ip_exp_map_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* ipv6 */
  entry_offset = SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_KEY_ENTRY_OFFSET(FALSE,in_qos_key->in_dscp_exp,in_qos_key->dp);
  res = soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_ip_exp_map_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  epni_ip_exp_map_tbl.ip_exp_map = out_qos_params->ipv6_tc;

  res = soc_pb_pp_epni_ip_exp_map_tbl_set_unsafe(
          unit,
          entry_offset,
          &epni_ip_exp_map_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* mpls */
  tmp = in_qos_key->in_dscp_exp % 8;
  key = SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_KEY_ENTRY(tmp,in_qos_key->dp);
  index = key * SOC_PB_PP_EG_QOS_MPLS_OUT_EXP_NOF_BITS;

  SOC_PB_PP_REG_BUFFER_GET(soc_pb_pp_reg->epni.mpls_exp_map_table_reg, SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS, buffer ,50, exit);
  tmp = out_qos_params->mpls_exp;

  res = soc_sand_bitstream_set_any_field(&tmp, index, (SOC_PB_PP_EG_QOS_MPLS_OUT_EXP_NOF_BITS), buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  SOC_PB_PP_REG_BUFFER_SET(soc_pb_pp_reg->epni.mpls_exp_map_table_reg, SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS, buffer, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_remark_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_qos_params_remark_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_REMARK_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_QOS_MAP_KEY, in_qos_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_QOS_PARAMS, out_qos_params, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_remark_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_qos_params_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_QOS_MAP_KEY, in_qos_key, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_remark_get_verify()", 0, 0);
}

/*********************************************************************
*     Remark QoS parameters, i.e. map in-dscp/exp and DP to
 *     out-dscp/exp in order to be set in outgoing packet
 *     headers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_qos_params_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_OUT SOC_PB_PP_EG_QOS_PARAMS                       *out_qos_params
  )
{
  SOC_PB_PP_REGS
    *soc_pb_pp_reg;
  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA
    epni_ip_exp_map_tbl;
  uint32
    entry_offset,
    key,
    tmp,
    index,
    buffer[SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS];
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_QOS_PARAMS_REMARK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_qos_params);

  soc_pb_pp_reg = soc_pb_pp_regs();

  SOC_PB_PP_EG_QOS_PARAMS_clear(out_qos_params);

  /* IPV4 */
  entry_offset = SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_KEY_ENTRY_OFFSET(TRUE,in_qos_key->in_dscp_exp,in_qos_key->dp);

  res = soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_ip_exp_map_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  out_qos_params->ipv4_tos = (SOC_SAND_PP_IPV4_TOS)epni_ip_exp_map_tbl.ip_exp_map;

  /* IPV6 */
  entry_offset = SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_KEY_ENTRY_OFFSET(FALSE,in_qos_key->in_dscp_exp,in_qos_key->dp);

  res = soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_ip_exp_map_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  out_qos_params->ipv6_tc = (SOC_SAND_PP_IPV6_TC)epni_ip_exp_map_tbl.ip_exp_map;

  /* mpls */
  tmp = in_qos_key->in_dscp_exp % 8;
  key = SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_KEY_ENTRY(tmp,in_qos_key->dp);
  index = key * SOC_PB_PP_EG_QOS_MPLS_OUT_EXP_NOF_BITS;

  SOC_PB_PP_REG_BUFFER_GET(soc_pb_pp_reg->epni.mpls_exp_map_table_reg, SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS, buffer ,50, exit);

  res = soc_sand_bitstream_get_any_field(buffer, index, (SOC_PB_PP_EG_QOS_MPLS_OUT_EXP_NOF_BITS), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  out_qos_params->mpls_exp = (SOC_SAND_PP_MPLS_EXP)tmp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_qos_params_remark_get_unsafe()", 0, 0);
}

/********************************************************************* 
* Invalid for Soc_petraB. 
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_qos_params_remark_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_qos_params_remark_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_qos_params_remark_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_qos_params_remark_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_encap_qos_params_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_qos_params_remark_get_verify()", 0, 0);
}

/********************************************************************* 
* Invalid for Soc_petraB. 
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_qos_params_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_QOS_PARAMS_REMARK_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_qos_params_remark_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_eg_qos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_qos_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_eg_qos;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_eg_qos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_qos_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_eg_qos;
}

uint32
  SOC_PB_PP_EG_QOS_MAP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_dscp_exp, SOC_PB_PP_EG_QOS_IN_DSCP_EXP_MAX, SOC_PB_PP_EG_QOS_IN_DSCP_EXP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_PB_PP_EG_QOS_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_QOS_MAP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_QOS_PARAMS_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->ipv4_tos, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 10, exit); */
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->ipv6_tc, SOC_SAND_PP_IPV6_TC_MAX, SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR, 11, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mpls_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_QOS_PARAMS_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_map_profile, SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_MAX, SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->php_type, SOC_PB_PP_EG_QOS_PHP_TYPE_MAX, SOC_PB_PP_EG_QOS_PHP_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_QOS_PHP_REMARK_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_QOS_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_map_profile, SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_MAX, SOC_PB_PP_EG_QOS_EXP_MAP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_QOS_PORT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

