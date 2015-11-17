/* $Id: pcp_oam_bfd.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_BFD_INCLUDED__
/* { */
#define __SOC_PCP_OAM_BFD_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_oam_api_bfd.h>
#include <soc/dpp/PCP/pcp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define PCP_OAM_BFD_COS_ID_MAX                                 (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_TTL_ID_MAX                                 (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_IP_ID_MAX                                  (SOC_SAND_U32_MAX)

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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_OAM_BFD_TTL_MAPPING_INFO_SET = PCP_PROC_DESC_BASE_OAM_BFD_FIRST,
  PCP_OAM_BFD_TTL_MAPPING_INFO_SET_PRINT,
  PCP_OAM_BFD_TTL_MAPPING_INFO_SET_UNSAFE,
  PCP_OAM_BFD_TTL_MAPPING_INFO_SET_VERIFY,
  PCP_OAM_BFD_TTL_MAPPING_INFO_GET,
  PCP_OAM_BFD_TTL_MAPPING_INFO_GET_PRINT,
  PCP_OAM_BFD_TTL_MAPPING_INFO_GET_VERIFY,
  PCP_OAM_BFD_TTL_MAPPING_INFO_GET_UNSAFE,
  PCP_OAM_BFD_IP_MAPPING_INFO_SET,
  PCP_OAM_BFD_IP_MAPPING_INFO_SET_PRINT,
  PCP_OAM_BFD_IP_MAPPING_INFO_SET_UNSAFE,
  PCP_OAM_BFD_IP_MAPPING_INFO_SET_VERIFY,
  PCP_OAM_BFD_IP_MAPPING_INFO_GET,
  PCP_OAM_BFD_IP_MAPPING_INFO_GET_PRINT,
  PCP_OAM_BFD_IP_MAPPING_INFO_GET_VERIFY,
  PCP_OAM_BFD_IP_MAPPING_INFO_GET_UNSAFE,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_PRINT,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_UNSAFE,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_VERIFY,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_PRINT,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_VERIFY,
  PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_UNSAFE,
  PCP_OAM_BFD_TX_INFO_SET,
  PCP_OAM_BFD_TX_INFO_SET_PRINT,
  PCP_OAM_BFD_TX_INFO_SET_UNSAFE,
  PCP_OAM_BFD_TX_INFO_SET_VERIFY,
  PCP_OAM_BFD_TX_INFO_GET,
  PCP_OAM_BFD_TX_INFO_GET_PRINT,
  PCP_OAM_BFD_TX_INFO_GET_VERIFY,
  PCP_OAM_BFD_TX_INFO_GET_UNSAFE,
  PCP_OAM_BFD_RX_INFO_SET,
  PCP_OAM_BFD_RX_INFO_SET_PRINT,
  PCP_OAM_BFD_RX_INFO_SET_UNSAFE,
  PCP_OAM_BFD_RX_INFO_SET_VERIFY,
  PCP_OAM_BFD_RX_INFO_GET,
  PCP_OAM_BFD_RX_INFO_GET_PRINT,
  PCP_OAM_BFD_RX_INFO_GET_VERIFY,
  PCP_OAM_BFD_RX_INFO_GET_UNSAFE,
  PCP_OAM_BFD_GET_PROCS_PTR,
  PCP_OAM_BFD_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_OAM_BFD_PROCEDURE_DESC_LAST
} PCP_OAM_BFD_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_OAM_BFD_COS_ID_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_OAM_BFD_FIRST,
  PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_IP_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_TC_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_DP_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_TNL_EXP_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_PWE_EXP_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_TNL_TTL_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_PWE_TTL_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_DETECT_MULT_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_MIN_RX_INTERVAL_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_MIN_TX_INTERVAL_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_EEP_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_PWE_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_COS_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_SRC_IP_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_TX_RATE_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_DISCRIMINATOR_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_BFD_TYPE_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_LIFE_TIME_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_BASE_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_START_OUT_OF_RANGE_ERR,
  PCP_OAM_BFD_END_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_OAM_BFD_ERR_LAST
} PCP_OAM_BFD_ERR;

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
* NAME:
 *   pcp_oam_bfd_ttl_mapping_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function maps 2 bit ttl_ndx to TTL information.
 *   This mapping is used to build BFD packets. Sets the TTL
 *   of the MPLS tunnel label, and in case PWE protection is
 *   applied, also sets the MPLS PWE label TTL.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx -
 *     TTL profile identifier. Range: 0-3
 *   SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info -
 *     TTL (time to live) configuration information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  );

uint32
  pcp_oam_bfd_ttl_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  );

uint32
  pcp_oam_bfd_ttl_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_ttl_mapping_info_set_unsafe" API.
 *     Refer to "pcp_oam_bfd_ttl_mapping_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_ip_mapping_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets one of four IPv4 possible source
 *   addresses. These addresses are used by the OAMP to
 *   generate BFD messages of types MPLS, PWE with IP, IP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx -
 *     IP profile identifier. Range 0-3
 *   SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info -
 *     IPv4/IPv6 address to configure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info
  );

uint32
  pcp_oam_bfd_ip_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info
  );

uint32
  pcp_oam_bfd_ip_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_ip_mapping_info_set_unsafe" API.
 *     Refer to "pcp_oam_bfd_ip_mapping_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_IP_MAPPING_INFO     *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_my_discriminator_range_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets the range of the local accelerated
 *   discriminators. The value of disc_range.start is
 *   subtracted from the your_discriminator field of the
 *   incoming BFD message, to determine the entry offset to
 *   access from disc_range.base. The physical range is
 *   [disc_range.base, disc_range.base + disc_range.end -
 *   disc_range.start]. If the packet does not match the
 *   range, it is forwarded to CPU.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range -
 *     The range of accelerated discriminators.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_my_discriminator_range_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  );

uint32
  pcp_oam_bfd_my_discriminator_range_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  );

uint32
  pcp_oam_bfd_my_discriminator_range_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_my_discriminator_range_set_unsafe" API.
 *     Refer to "pcp_oam_bfd_my_discriminator_range_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_oam_bfd_my_discriminator_range_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_tx_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures/updates BFD message
 *   transmission information.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        my_discriminator_ndx -
 *     Discriminator index. Must be in accelerated
 *     discriminator range (see
 *     pcp_oam_bfd_my_discriminator_range_set for more
 *     information)
 *   SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info -
 *     BFD message transmission information for the specified
 *     discriminator index
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info
  );

uint32
  pcp_oam_bfd_tx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info
  );

uint32
  pcp_oam_bfd_tx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_tx_info_set_unsafe" API.
 *     Refer to "pcp_oam_bfd_tx_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TX_INFO             *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_rx_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures/updates BFD message reception
 *   and session monitoring information.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        my_discriminator_ndx -
 *     Discriminator index. Must be in accelerated
 *     discriminator range (see
 *     pcp_oam_bfd_my_discriminator_range_set for more
 *     information)
 *   SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info -
 *     BFD message reception and session monitoring information
 *     for the specified discriminator index
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info
  );

uint32
  pcp_oam_bfd_rx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info
  );

uint32
  pcp_oam_bfd_rx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_rx_info_set_unsafe" API.
 *     Refer to "pcp_oam_bfd_rx_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_RX_INFO             *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_oam_api_bfd module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_bfd_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_oam_api_bfd module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_bfd_get_errs_ptr(void);

uint32
  PCP_OAM_BFD_COS_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_COS_MAPPING_INFO *info
  );

uint32
  PCP_OAM_BFD_TTL_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO *info
  );

uint32
  PCP_OAM_BFD_IP_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO *info
  );

uint32
  PCP_OAM_BFD_NEGOTIATION_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_NEGOTIATION_INFO *info
  );

uint32
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO *info
  );

uint32
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_INFO *info
  );

uint32
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO *info
  );

uint32
  PCP_OAM_BFD_FWD_IP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_IP_INFO *info
  );

uint32
  PCP_OAM_BFD_FWD_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_INFO *info
  );

uint32
  PCP_OAM_BFD_TX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO *info
  );

uint32
  PCP_OAM_BFD_RX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO *info
  );

uint32
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_verify(
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_BFD_INCLUDED__*/
#endif

