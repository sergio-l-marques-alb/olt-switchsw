/* $Id: ppc_api_eg_qos.c,v 1.15 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppc/src/soc_ppc_api_eg_qos.c
*
* MODULE PREFIX:  soc_ppc_eg
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_eg_qos.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 *  MACROS   *
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

void
  SOC_PPC_EG_QOS_MAP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_MAP_KEY));
  info->in_dscp_exp = 0;
  info->dp = 0;
  info->remark_profile = 0;  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_QOS_MAP_KEY));
  info->in_dscp_exp = 0;
  info->remark_profile = 0;  
  info->pkt_hdr_type = 0;
/*  info->ecn_capable = 0;*/
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_PARAMS));
  info->ipv4_tos = 0;
  info->ipv6_tc = 0;
  info->mpls_exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_QOS_PARAMS));
  info->ip_dscp = 0;
  info->mpls_exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_PHP_REMARK_KEY));
  info->exp_map_profile = 0;
  info->php_type = SOC_PPC_NOF_EG_QOS_UNIFORM_PHP_TYPES;
  info->exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_PORT_INFO));
  info->exp_map_profile = 0;
#ifdef BCM_88660
  info->marking_profile = 0;
#endif
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#ifdef BCM_88660
void
  SOC_PPC_EG_QOS_MARKING_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MARKING_KEY *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(*info));

    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_MARKING_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MARKING_PARAMS *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(*info));

    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_EG_QOS_GLOBAL_INFO_clear(SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
 
  sal_memset(info, 0x0, sizeof(*info));
 
  SOC_SAND_MAGIC_NUM_SET;
 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* BCM_88660 */

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4:
    str = "soc_ppc_eg_qos_uniform_php_pop_into_ipv4";
  break;
  case SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6:
    str = "soc_ppc_eg_qos_uniform_php_pop_into_ipv6";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_EG_QOS_MAP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_dscp_exp: %u\n\r"),info->in_dscp_exp));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "remark_profile: %u\n\r"), info->remark_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_dscp_exp: %u\n\r"),info->in_dscp_exp));
  LOG_CLI((BSL_META_U(unit,
                      "remark_profile: %u\n\r"), info->remark_profile));
  LOG_CLI((BSL_META_U(unit,
                      "pkt_header_type: %u\n\r"), info->pkt_hdr_type));
  if (info->pkt_hdr_type == SOC_PPC_PKT_HDR_TYPE_ETH) 
  {
    /*LOG_CLI((BSL_META_U(unit,
                          "ecn_capable: %u\n\r"),info->ecn_capable));*/
    LOG_CLI((BSL_META_U(unit,
                        "dp: %u\n\r"), info->dp));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PARAMS_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ipv4_tos: %u\n\r"),info->ipv4_tos));
  LOG_CLI((BSL_META_U(unit,
                      "ipv6_tc: %u\n\r"), info->ipv6_tc));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_exp: %u\n\r"), info->mpls_exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_PARAMS_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ip_dscp: %u\n\r"),info->ip_dscp));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_exp: %u\n\r"), info->mpls_exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "exp_map_profile: %u\n\r"),info->exp_map_profile));
  LOG_CLI((BSL_META_U(unit,
                      "php_type %s "), SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE_to_string(info->php_type)));
  LOG_CLI((BSL_META_U(unit,
                      "exp: %u\n\r"), info->exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "exp_map_profile: %u\n\r"),info->exp_map_profile));
#ifdef BCM_88660
  LOG_CLI((BSL_META_U(unit,
                      "marking_profile: %u\n\r"),info->marking_profile));
#endif
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#ifdef BCM_88660
void
  SOC_PPC_EG_QOS_MARKING_KEY_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
    
  LOG_CLI((BSL_META_U(unit,
                      "resolved_dp_ndx: %u\n\r"), info->resolved_dp_ndx ));
  LOG_CLI((BSL_META_U(unit,
                      "tc_ndx: %u\n\r"), info->tc_ndx ));
  LOG_CLI((BSL_META_U(unit,
                      "in_lif_profile: %u\n\r"), info->in_lif_profile ));
  LOG_CLI((BSL_META_U(unit,
                      "marking_profile: %u\n\r"), info->marking_profile ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_MARKING_PARAMS_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ip_dscp: %u\n\r"), info->ip_dscp ));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_exp: %u\n\r"), info->mpls_exp ));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_EG_QOS_GLOBAL_INFO_print(SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
 
  LOG_CLI((BSL_META_U(unit,
                      "inlif_profile_bitmap: 0x%x\n\r"), info->in_lif_profile_bitmap));
  LOG_CLI((BSL_META_U(unit,
                      "resolved_dp_bitmap: 0x%x\n\r"), info->resolved_dp_bitmap));
 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
 
}

#endif /* BCM_88660 */

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

