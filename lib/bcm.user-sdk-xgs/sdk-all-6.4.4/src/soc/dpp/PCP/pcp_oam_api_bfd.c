/* $Id: pcp_oam_api_bfd.c,v 1.6 Broadcom SDK $
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
#include <soc/dpp/PCP/pcp_oam_api_bfd.h>
#include <soc/dpp/PCP/pcp_oam_bfd.h>

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
 *     This mapping is used to build BFD packets. Sets the TTL
 *     of the MPLS tunnel label, and in case PWE protection is
 *     applied, also sets the MPLS PWE label TTL.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TTL_MAPPING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_ttl_mapping_info_set_verify(
          unit,
          ttl_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_ttl_mapping_info_set_unsafe(
          unit,
          ttl_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ttl_mapping_info_set()", ttl_ndx, 0);
}

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build BFD packets. Sets the TTL
 *     of the MPLS tunnel label, and in case PWE protection is
 *     applied, also sets the MPLS PWE label TTL.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TTL_MAPPING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_ttl_mapping_info_get_verify(
          unit,
          ttl_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_ttl_mapping_info_get_unsafe(
          unit,
          ttl_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ttl_mapping_info_get()", ttl_ndx, 0);
}

/*********************************************************************
*     This function sets one of four IPv4 possible source
 *     addresses. These addresses are used by the OAMP to
 *     generate BFD messages of types MPLS, PWE with IP, IP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_IP_MAPPING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_ip_mapping_info_set_verify(
          unit,
          ip_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_ip_mapping_info_set_unsafe(
          unit,
          ip_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ip_mapping_info_set()", ip_ndx, 0);
}

/*********************************************************************
*     This function sets one of four IPv4 possible source
 *     addresses. These addresses are used by the OAMP to
 *     generate BFD messages of types MPLS, PWE with IP, IP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_IP_MAPPING_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_IP_MAPPING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_ip_mapping_info_get_verify(
          unit,
          ip_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_ip_mapping_info_get_unsafe(
          unit,
          ip_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ip_mapping_info_get()", ip_ndx, 0);
}

/*********************************************************************
*     This function sets the range of the local accelerated
 *     discriminators. The value of disc_range.start is
 *     subtracted from the your_discriminator field of the
 *     incoming BFD message, to determine the entry offset to
 *     access from disc_range.base. The physical range is
 *     [disc_range.base, disc_range.base + disc_range.end -
 *     disc_range.start]. If the packet does not match the
 *     range, it is forwarded to CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_my_discriminator_range_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(disc_range);

  res = pcp_oam_bfd_my_discriminator_range_set_verify(
          unit,
          disc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_my_discriminator_range_set_unsafe(
          unit,
          disc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_my_discriminator_range_set()", 0, 0);
}

/*********************************************************************
*     This function sets the range of the local accelerated
 *     discriminators. The value of disc_range.start is
 *     subtracted from the your_discriminator field of the
 *     incoming BFD message, to determine the entry offset to
 *     access from disc_range.base. The physical range is
 *     [disc_range.base, disc_range.base + disc_range.end -
 *     disc_range.start]. If the packet does not match the
 *     range, it is forwarded to CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_my_discriminator_range_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(disc_range);

  res = pcp_oam_bfd_my_discriminator_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_my_discriminator_range_get_unsafe(
          unit,
          disc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_my_discriminator_range_get()", 0, 0);
}

/*********************************************************************
*     This function configures/updates BFD message
 *     transmission information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TX_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_tx_info_set_verify(
          unit,
          my_discriminator_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_tx_info_set_unsafe(
          unit,
          my_discriminator_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_tx_info_set()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     This function configures/updates BFD message
 *     transmission information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TX_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_tx_info_get_verify(
          unit,
          my_discriminator_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_tx_info_get_unsafe(
          unit,
          my_discriminator_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_tx_info_get()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     This function configures/updates BFD message reception
 *     and session monitoring information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_RX_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_rx_info_set_verify(
          unit,
          my_discriminator_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_rx_info_set_unsafe(
          unit,
          my_discriminator_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_rx_info_set()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     This function configures/updates BFD message reception
 *     and session monitoring information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_RX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_RX_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_bfd_rx_info_get_verify(
          unit,
          my_discriminator_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_bfd_rx_info_get_unsafe(
          unit,
          my_discriminator_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_rx_info_get()", my_discriminator_ndx, 0);
}

void
  PCP_OAM_BFD_COS_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_COS_MAPPING_INFO));
  info->tc = 0;
  info->dp = 0;
  info->tnl_exp = 0;
  info->pwe_exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_TTL_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_TTL_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_TTL_MAPPING_INFO));
  info->tnl_ttl = 0;
  info->pwe_ttl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_IP_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_IP_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_IP_MAPPING_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_NEGOTIATION_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_NEGOTIATION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_NEGOTIATION_INFO));
  info->detect_mult = 0;
  info->min_rx_interval = PCP_OAM_NOF_BFD_TX_RATES;
  info->min_tx_interval = PCP_OAM_NOF_BFD_TX_RATES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO));
  info->eep = 0;
  info->cos = 0;
  info->ttl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_MPLS_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_FWD_MPLS_PWE_INFO));
  info->eep = 0;
  info->pwe = 0;
  info->cos = 0;
  info->ttl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO));
  info->src_ip = 0;
  info->eep = 0;
  info->pwe = 0;
  info->cos = 0;
  info->ttl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_IP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_FWD_IP_INFO));
  info->src_ip = 0;
  info->cos = 0;
  info->ttl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_FWD_INFO));
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_clear(&(info->mpls_tunnel_info));
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_clear(&(info->mpls_pwe_info));
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_clear(&(info->mpls_pwe_with_ip_info));
  PCP_OAM_BFD_FWD_IP_INFO_clear(&(info->ip_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_TX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_TX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_TX_INFO));
  info->valid = 0;
  info->tx_rate = PCP_OAM_NOF_BFD_TX_RATES;
  info->discriminator = 0;
  info->bfd_type = PCP_OAM_NOF_BFD_TYPES;
  PCP_OAM_BFD_NEGOTIATION_INFO_clear(&(info->ng_info));
  PCP_OAM_BFD_FWD_INFO_clear(&(info->fw_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_RX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_RX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_RX_INFO));
  info->life_time = 0;
  info->defect = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_clear(
    SOC_SAND_OUT PCP_OAM_BFD_DISCRIMINATOR_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_BFD_DISCRIMINATOR_RANGE));
  info->base = 0;
  info->start = 0;
  info->end = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_BFD_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_BFD_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_BFD_TYPE_MPLS:
    str = "mpls";
  break;
  case PCP_OAM_BFD_TYPE_MPLS_PWE:
    str = "mpls_pwe";
  break;
  case PCP_OAM_BFD_TYPE_MPLS_PWE_WITH_IP:
    str = "mpls_pwe_with_ip";
  break;
  case PCP_OAM_BFD_TYPE_IP:
    str = "ip";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  PCP_OAM_BFD_TX_RATE_to_string(
    SOC_SAND_IN  PCP_OAM_BFD_TX_RATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_BFD_TX_RATE_1:
    str = "rate_1";
  break;
  case PCP_OAM_BFD_TX_RATE_2:
    str = "rate_2";
  break;
  case PCP_OAM_BFD_TX_RATE_5:
    str = "rate_5";
  break;
  case PCP_OAM_BFD_TX_RATE_10:
    str = "rate_10";
  break;
  case PCP_OAM_BFD_TX_RATE_20:
    str = "rate_20";
  break;
  case PCP_OAM_BFD_TX_RATE_50:
    str = "rate_50";
  break;
  case PCP_OAM_BFD_TX_RATE_100:
    str = "rate_100";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  PCP_OAM_BFD_COS_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META("dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META("tnl_exp: %u\n\r"),info->tnl_exp));
  LOG_CLI((BSL_META("pwe_exp: %u\n\r"),info->pwe_exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_TTL_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("tnl_ttl: %u\n\r"),info->tnl_ttl));
  LOG_CLI((BSL_META("pwe_ttl: %u\n\r"),info->pwe_ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_IP_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_NEGOTIATION_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_NEGOTIATION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("detect_mult: %u\n\r"),info->detect_mult));
  LOG_CLI((BSL_META("min_rx_interval %s "), PCP_OAM_BFD_TX_RATE_to_string(info->min_rx_interval)));
  LOG_CLI((BSL_META("min_tx_interval %s "), PCP_OAM_BFD_TX_RATE_to_string(info->min_tx_interval)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("eep: %u\n\r"),info->eep));
  LOG_CLI((BSL_META("cos: %u\n\r"),info->cos));
  LOG_CLI((BSL_META("ttl: %u\n\r"),info->ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("eep: %u\n\r"),info->eep));
  LOG_CLI((BSL_META("pwe: %u\n\r"),info->pwe));
  LOG_CLI((BSL_META("cos: %u\n\r"),info->cos));
  LOG_CLI((BSL_META("ttl: %u\n\r"),info->ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("src_ip: %u\n\r"),info->src_ip));
  LOG_CLI((BSL_META("eep: %u\n\r"),info->eep));
  LOG_CLI((BSL_META("pwe: %u\n\r"),info->pwe));
  LOG_CLI((BSL_META("cos: %u\n\r"),info->cos));
  LOG_CLI((BSL_META("ttl: %u\n\r"),info->ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_IP_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("src_ip: %u\n\r"),info->src_ip));
  LOG_CLI((BSL_META("cos: %u\n\r"),info->cos));
  LOG_CLI((BSL_META("ttl: %u\n\r"),info->ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_FWD_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("mpls_tunnel_info:")));
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_print(&(info->mpls_tunnel_info));
  LOG_CLI((BSL_META("mpls_pwe_info:")));
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_print(&(info->mpls_pwe_info));
  LOG_CLI((BSL_META("mpls_pwe_with_ip_info:")));
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_print(&(info->mpls_pwe_with_ip_info));
  LOG_CLI((BSL_META("ip_info:")));
  PCP_OAM_BFD_FWD_IP_INFO_print(&(info->ip_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_TX_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META("tx_rate %s "), PCP_OAM_BFD_TX_RATE_to_string(info->tx_rate)));
  LOG_CLI((BSL_META("discriminator: %u\n\r"),info->discriminator));
  LOG_CLI((BSL_META("bfd_type %s "), PCP_OAM_BFD_TYPE_to_string(info->bfd_type)));
  LOG_CLI((BSL_META("ng_info:")));
  PCP_OAM_BFD_NEGOTIATION_INFO_print(&(info->ng_info));
  LOG_CLI((BSL_META("fw_info:")));
  PCP_OAM_BFD_FWD_INFO_print(&(info->fw_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_RX_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("life_time: %u\n\r"),info->life_time));
  LOG_CLI((BSL_META("defect: %u\n\r"),info->defect));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_print(
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *info
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

