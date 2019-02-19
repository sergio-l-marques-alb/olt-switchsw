/* $Id: pcp_oam_api_mpls.c,v 1.6 Broadcom SDK $
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
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PCP/pcp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_oam_api_mpls.h>
#include <soc/dpp/PCP/pcp_oam_mpls.h>

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

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_ttl_mapping_info_set_verify(
          unit,
          ttl_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_ttl_mapping_info_set_unsafe(
          unit,
          ttl_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ttl_mapping_info_set()", ttl_ndx, 0);
}

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_ttl_mapping_info_get_verify(
          unit,
          ttl_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_ttl_mapping_info_get_unsafe(
          unit,
          ttl_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ttl_mapping_info_get()", ttl_ndx, 0);
}

/*********************************************************************
*     This function maps 4 bit ip_ndx to IPv4/IPv6
 *     information. This mapping is used to determine the LSR
 *     identifier of the constructed MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_IN  uint8                       is_ipv6,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_IP_MAPPING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_ip_mapping_info_set_verify(
          unit,
          ip_ndx,
          is_ipv6,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_ip_mapping_info_set_unsafe(
          unit,
          ip_ndx,
          is_ipv6,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ip_mapping_info_set()", ip_ndx, 0);
}

/*********************************************************************
*     This function maps 4 bit ip_ndx to IPv4/IPv6
 *     information. This mapping is used to determine the LSR
 *     identifier of the constructed MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_OUT uint8                       *is_ipv6,
    SOC_SAND_OUT PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_IP_MAPPING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_ipv6);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_ip_mapping_info_get_verify(
          unit,
          ip_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_ip_mapping_info_get_unsafe(
          unit,
          ip_ndx,
          is_ipv6,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ip_mapping_info_get()", ip_ndx, 0);
}

/*********************************************************************
*     This function sets label ranges for MPLS OAM. An
 *     incoming MPLS OAM packet is accelerated if in_label (the
 *     first label after the MPLS OAM label, 14) fulfils one of
 *     the following conditions: 1. in_label is between
 *     tnl_range.start and tnl_range.end.2. in_label is between
 *     pwe_range.start and pwe_range.end. pwe_range.base and
 *     tnl_range.base set the physical ranges associated with
 *     pwe_range and tnl_range respectively. The physical range
 *     is [range.base, range.base + range.end - range.start].if
 *     the packet does not fulfill either conditions, it is
 *     forwarded to CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LABEL_RANGES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tnl_range);
  SOC_SAND_CHECK_NULL_INPUT(pwe_range);

  res = pcp_oam_mpls_label_ranges_set_verify(
          unit,
          tnl_range,
          pwe_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_label_ranges_set_unsafe(
          unit,
          tnl_range,
          pwe_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_label_ranges_set()", 0, 0);
}

/*********************************************************************
*     This function sets label ranges for MPLS OAM. An
 *     incoming MPLS OAM packet is accelerated if in_label (the
 *     first label after the MPLS OAM label, 14) fulfils one of
 *     the following conditions: 1. in_label is between
 *     tnl_range.start and tnl_range.end.2. in_label is between
 *     pwe_range.start and pwe_range.end. pwe_range.base and
 *     tnl_range.base set the physical ranges associated with
 *     pwe_range and tnl_range respectively. The physical range
 *     is [range.base, range.base + range.end - range.start].if
 *     the packet does not fulfill either conditions, it is
 *     forwarded to CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LABEL_RANGES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tnl_range);
  SOC_SAND_CHECK_NULL_INPUT(pwe_range);

  res = pcp_oam_mpls_label_ranges_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_label_ranges_get_unsafe(
          unit,
          tnl_range,
          pwe_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_label_ranges_get()", 0, 0);
}

/*********************************************************************
*     This function sets the configurable FFD rate. Rate is
 *     given by the interval (milliseconds) between two
 *     consecutive frames. Range: 10 - 1270 (denominations of
 *     10ms)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_IN  uint32                       interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = pcp_oam_mpls_configurable_ffd_rate_set_verify(
          unit,
          rate_ndx,
          interval
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_configurable_ffd_rate_set_unsafe(
          unit,
          rate_ndx,
          interval
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_configurable_ffd_rate_set()", 0, 0);
}

/*********************************************************************
*     This function sets the configurable FFD rate. Rate is
 *     given by the interval (milliseconds) between two
 *     consecutive frames. Range: 10 - 1270 (denominations of
 *     10ms)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_OUT uint32                       *interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(interval);

  res = pcp_oam_mpls_configurable_ffd_rate_get_verify(
          unit,
          rate_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_configurable_ffd_rate_get_unsafe(
          unit,
          rate_ndx,
          interval
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_configurable_ffd_rate_get()", 0, 0);
}

/*********************************************************************
*     This function configures LSP-source message generator
 *     (CV/FFD/BDI/FDI)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_TX_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_lsp_tx_info_set_verify(
          unit,
          mot_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_lsp_tx_info_set_unsafe(
          unit,
          mot_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_tx_info_set()", mot_ndx, 0);
}

/*********************************************************************
*     This function configures LSP-source message generator
 *     (CV/FFD/BDI/FDI)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_TX_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_lsp_tx_info_get_verify(
          unit,
          mot_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_lsp_tx_info_get_unsafe(
          unit,
          mot_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_tx_info_get()", mot_ndx, 0);
}

/*********************************************************************
*     This function configures LSP-sink
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_RX_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_lsp_rx_info_set_verify(
          unit,
          mor_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_lsp_rx_info_set_unsafe(
          unit,
          mor_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_rx_info_set()", mor_ndx, 0);
}

/*********************************************************************
*     This function configures LSP-sink
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_RX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_RX_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mpls_lsp_rx_info_get_verify(
          unit,
          mor_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mpls_lsp_rx_info_get_unsafe(
          unit,
          mor_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_rx_info_get()", mor_ndx, 0);
}

void
  PCP_OAM_MPLS_COS_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_COS_MAPPING_INFO));
  info->tc = 0;
  info->dp = 0;
  info->exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_TTL_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_TTL_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_TTL_MAPPING_INFO));
  info->ttl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_IPV4_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_IPV4_MAPPING_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_IPV6_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_IPV6_MAPPING_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_IP_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_IP_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_IP_MAPPING_INFO));
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_clear(&(info->ipv4));
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_clear(&(info->ipv6));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_PKT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_LSP_TX_PKT_INFO));
  info->system_port = 0;
  info->cos = 0;
  info->ttl = 0;
  info->eep = 0;
  info->lsr_id = 0;
  info->lsp_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_DEFECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_LSP_TX_DEFECT_INFO));
  info->defect_type = PCP_OAM_NOF_MPLS_LSP_TX_DEFECT_TYPES;
  info->defect_location = 0;
  info->bdi_enable = 0;
  info->fdi_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_TX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_LSP_TX_INFO));
  info->valid = 0;
  info->type = PCP_OAM_NOF_MPLS_CC_PKT_TYPES;
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_clear(&(info->packet_info));
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_clear(&(info->defect_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_RX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_RX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_LSP_RX_INFO));
  info->valid = 0;
  info->is_ffd = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LABEL_RANGE_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MPLS_LABEL_RANGE));
  info->base = 0;
  info->start = 0;
  info->end = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_MPLS_CC_PKT_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE:
    str = "ffd_configurable";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_10:
    str = "ffd_10";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_20:
    str = "ffd_20";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_50:
    str = "ffd_50";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_100:
    str = "ffd_100";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_200:
    str = "ffd_200";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_500:
    str = "ffd_500";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_FFD_1000:
    str = "ffd_1000";
  break;
  case PCP_OAM_MPLS_CC_PKT_TYPE_CV:
    str = "cv";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_SERVER:
    str = "server";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_PEER_ME:
    str = "peer_me";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_DLOCV:
    str = "dlocv";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMATCH:
    str = "ttsi_mismatch";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMERGE:
    str = "ttsi_mismerge";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_EXCESS:
    str = "excess";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_UNKNOWN:
    str = "unknown";
  break;
  case PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_NONE:
    str = "none";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_SERVER:
    str = "server";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_PEER_ME:
    str = "peer_me";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_DLOCV:
    str = "dlocv";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMATCH:
    str = "ttsi_mismatch";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMERGE:
    str = "ttsi_mismerge";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_EXCESS:
    str = "excess";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_UNKNOWN:
    str = "unknown";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_NONE:
    str = "none";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEFECT_SET:
    str = "pcp_oam_mpls_lsp_rx_defect_filter_disable_defect_set";
  break;
  case PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEFECT_CLEAR:
    str = "pcp_oam_mpls_lsp_rx_defect_filter_disable_defect_clear";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  PCP_OAM_MPLS_COS_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META("dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META("exp: %u\n\r"),info->exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_TTL_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("ttl: %u\n\r"),info->ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV4_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV6_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_IP_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("ipv4:")));
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_print(&(info->ipv4));
  LOG_CLI((BSL_META("ipv6:")));
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_print(&(info->ipv6));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_PKT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("system_port: %u\n\r"),info->system_port));
  LOG_CLI((BSL_META("cos: %u\n\r"),info->cos));
  LOG_CLI((BSL_META("ttl: %u\n\r"),info->ttl));
  LOG_CLI((BSL_META("eep: %u\n\r"),info->eep));
  LOG_CLI((BSL_META("lsr_id: %u\n\r"),info->lsr_id));
  LOG_CLI((BSL_META("lsp_id: %u\n\r"),info->lsp_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("defect_type %s "), PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_to_string(info->defect_type)));
  LOG_CLI((BSL_META("defect_location: %u\n\r"),info->defect_location));
  LOG_CLI((BSL_META("bdi_enable: %u\n\r"),info->bdi_enable));
  LOG_CLI((BSL_META("fdi_enable: %u\n\r"),info->fdi_enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_TX_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META("type %s "), PCP_OAM_MPLS_CC_PKT_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META("packet_info:")));
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_print(&(info->packet_info));
  LOG_CLI((BSL_META("defect_info:")));
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_print(&(info->defect_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LSP_RX_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META("is_ffd: %u\n\r"),info->is_ffd));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_MPLS_LABEL_RANGE_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("base: %u\n\r"),info->base));
  LOG_CLI((BSL_META("start: %u\n\r"),info->start));
  LOG_CLI((BSL_META("end: %u\n\r"),info->end));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

