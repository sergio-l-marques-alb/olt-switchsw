/* $Id: tmc_pmf_pgm_mgmt.c,v 1.6 Broadcom SDK $
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
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_pmf_pgm_mgmt.h>

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
  SOC_TMC_PMF_PGM_MGMT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_MGMT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_MGMT_INFO));
  info->is_to_set = 0;
  info->is_addition = 0;
  info->is_2nd_iter = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_MGMT_NDX_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_MGMT_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_MGMT_NDX));
  info->pp_port_ndx = 0;
  info->pfg_ndx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_FP_PKT_HDR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FP_PKT_HDR_TYPE_RAW:
    str = "raw";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_FTMH:
    str = "ftmh";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_TM:
    str = "tm";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_TM_IS:
    str = "tm_is";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_TM_PPH:
    str = "tm_pph";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_TM_IS_PPH:
    str = "tm_is_pph";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH:
    str = "eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH:
    str = "eth_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH:
    str = "ipv4_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH:
    str = "ipv6_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH:
    str = "mpls1_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH:
    str = "mpls2_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH:
    str = "mpls3_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH:
    str = "eth_mpls1_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH:
    str = "eth_mpls2_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH:
    str = "eth_mpls3_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH:
    str = "ipv4_mpls1_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH:
    str = "ipv4_mpls2_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH:
    str = "ipv4_mpls3_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH:
    str = "ipv6_mpls1_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH:
    str = "ipv6_mpls2_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH:
    str = "ipv6_mpls3_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH:
    str = "ipv4_ipv4_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH:
    str = "ipv6_ipv4_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH:
    str = "eth_trill_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_ETH_IPV4_ETH:
    str = "eth_ipv4_eth";
  break;
  case SOC_TMC_FP_PKT_HDR_TYPE_FC_ETH:
    str = "fc_eth";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


const char*
  SOC_TMC_PMF_PGM_MGMT_SOURCE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_MGMT_SOURCE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_PGM_MGMT_SOURCE_PP_PORT:
    str = "pp_port";
  break;
  case SOC_TMC_PMF_PGM_MGMT_SOURCE_FP:
    str = "fp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_PMF_PGM_MGMT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_MGMT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_to_set: %u\n\r"),info->is_to_set));
  LOG_CLI((BSL_META_U(unit,
                      "is_addition: %u\n\r"),info->is_addition));
  LOG_CLI((BSL_META_U(unit,
                      "is_2nd_iter: %u\n\r"),info->is_2nd_iter));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_MGMT_NDX_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_MGMT_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pp_port_ndx: %u\n\r"),info->pp_port_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "pfg_ndx: %u\n\r"),info->pfg_ndx));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

