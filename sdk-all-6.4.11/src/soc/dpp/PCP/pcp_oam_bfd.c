/* $Id: pcp_oam_bfd.c,v 1.6 Broadcom SDK $
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

#include <soc/dpp/PCP/pcp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_oam_bfd.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_MAX                   (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_TC_MAX                                     (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_DP_MAX                                     (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_TNL_EXP_MAX                                (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_PWE_EXP_MAX                                (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_TNL_TTL_MAX                                (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_PWE_TTL_MAX                                (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_DETECT_MULT_MAX                            (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_MIN_RX_INTERVAL_MAX                        (PCP_OAM_NOF_BFD_TX_RATES-1)
#define PCP_OAM_BFD_MIN_TX_INTERVAL_MAX                        (PCP_OAM_NOF_BFD_TX_RATES-1)
#define PCP_OAM_BFD_EEP_MAX                                    (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_PWE_MAX                                    (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_COS_MAX                                    (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_SRC_IP_MAX                                 (SOC_SAND_UINT_MAX)
#define PCP_OAM_BFD_TX_RATE_MAX                                (PCP_OAM_NOF_BFD_TX_RATES-1)
#define PCP_OAM_BFD_DISCRIMINATOR_MAX                          (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_BFD_TYPE_MAX                               (PCP_OAM_NOF_BFD_TYPES-1)
#define PCP_OAM_BFD_LIFE_TIME_MAX                              (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_BASE_MAX                                   (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_START_MAX                                  (SOC_SAND_U32_MAX)
#define PCP_OAM_BFD_END_MAX                                    (SOC_SAND_U32_MAX)

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_oam_bfd[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TTL_MAPPING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_IP_MAPPING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_TX_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_RX_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_BFD_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_oam_bfd[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_OAM_BFD_COS_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_COS_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_BFD_COS_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_BFD_TTL_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_IP_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_IP_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_BFD_IP_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'my_discriminator_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_TC_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_TC_OUT_OF_RANGE_ERR",
    "The parameter 'tc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_DP_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_DP_OUT_OF_RANGE_ERR",
    "The parameter 'dp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_TNL_EXP_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_TNL_EXP_OUT_OF_RANGE_ERR",
    "The parameter 'tnl_exp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_PWE_EXP_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_PWE_EXP_OUT_OF_RANGE_ERR",
    "The parameter 'pwe_exp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_TNL_TTL_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_TNL_TTL_OUT_OF_RANGE_ERR",
    "The parameter 'tnl_ttl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_PWE_TTL_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_PWE_TTL_OUT_OF_RANGE_ERR",
    "The parameter 'pwe_ttl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_DETECT_MULT_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_DETECT_MULT_OUT_OF_RANGE_ERR",
    "The parameter 'detect_mult' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_MIN_RX_INTERVAL_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_MIN_RX_INTERVAL_OUT_OF_RANGE_ERR",
    "The parameter 'min_rx_interval' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_BFD_TX_RATES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_MIN_TX_INTERVAL_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_MIN_TX_INTERVAL_OUT_OF_RANGE_ERR",
    "The parameter 'min_tx_interval' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_BFD_TX_RATES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_EEP_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_PWE_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_PWE_OUT_OF_RANGE_ERR",
    "The parameter 'pwe' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_COS_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_COS_OUT_OF_RANGE_ERR",
    "The parameter 'cos' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_SRC_IP_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_SRC_IP_OUT_OF_RANGE_ERR",
    "The parameter 'src_ip' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_TX_RATE_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_TX_RATE_OUT_OF_RANGE_ERR",
    "The parameter 'tx_rate' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_BFD_TX_RATES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_DISCRIMINATOR_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_DISCRIMINATOR_OUT_OF_RANGE_ERR",
    "The parameter 'discriminator' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_BFD_TYPE_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_BFD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'bfd_type' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_BFD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_LIFE_TIME_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_LIFE_TIME_OUT_OF_RANGE_ERR",
    "The parameter 'life_time' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_BASE_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_BASE_OUT_OF_RANGE_ERR",
    "The parameter 'base' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_START_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_START_OUT_OF_RANGE_ERR",
    "The parameter 'start' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_BFD_END_OUT_OF_RANGE_ERR,
    "PCP_OAM_BFD_END_OUT_OF_RANGE_ERR",
    "The parameter 'end' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
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

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build BFD packets. Sets the TTL
 *     of the MPLS tunnel label, and in case PWE protection is
 *     applied, also sets the MPLS PWE label TTL.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TTL_MAPPING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ttl_mapping_info_set_unsafe()", ttl_ndx, 0);
}

uint32
  pcp_oam_bfd_ttl_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TTL_MAPPING_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_ndx, PCP_OAM_BFD_TTL_ID_MAX, PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_TTL_MAPPING_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ttl_mapping_info_set_verify()", ttl_ndx, 0);
}

uint32
  pcp_oam_bfd_ttl_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TTL_MAPPING_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_ndx, PCP_OAM_BFD_TTL_ID_MAX, PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ttl_mapping_info_get_verify()", ttl_ndx, 0);
}

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build BFD packets. Sets the TTL
 *     of the MPLS tunnel label, and in case PWE protection is
 *     applied, also sets the MPLS PWE label TTL.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TTL_MAPPING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_BFD_TTL_MAPPING_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ttl_mapping_info_get_unsafe()", ttl_ndx, 0);
}

/*********************************************************************
*     This function sets one of four IPv4 possible source
 *     addresses. These addresses are used by the OAMP to
 *     generate BFD messages of types MPLS, PWE with IP, IP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_IP_MAPPING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ip_mapping_info_set_unsafe()", ip_ndx, 0);
}

uint32
  pcp_oam_bfd_ip_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_IP_MAPPING_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ip_ndx, PCP_OAM_BFD_IP_ID_MAX, PCP_OAM_BFD_IP_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_IP_MAPPING_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ip_mapping_info_set_verify()", ip_ndx, 0);
}

uint32
  pcp_oam_bfd_ip_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_IP_MAPPING_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ip_ndx, PCP_OAM_BFD_IP_ID_MAX, PCP_OAM_BFD_IP_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ip_mapping_info_get_verify()", ip_ndx, 0);
}

/*********************************************************************
*     This function sets one of four IPv4 possible source
 *     addresses. These addresses are used by the OAMP to
 *     generate BFD messages of types MPLS, PWE with IP, IP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_IP_MAPPING_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_IP_MAPPING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_BFD_IP_MAPPING_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_ip_mapping_info_get_unsafe()", ip_ndx, 0);
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
  pcp_oam_bfd_my_discriminator_range_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(disc_range);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_my_discriminator_range_set_unsafe()", 0, 0);
}

uint32
  pcp_oam_bfd_my_discriminator_range_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_BFD_DISCRIMINATOR_RANGE, disc_range, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_my_discriminator_range_set_verify()", 0, 0);
}

uint32
  pcp_oam_bfd_my_discriminator_range_get_verify(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_my_discriminator_range_get_verify()", 0, 0);
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
  pcp_oam_bfd_my_discriminator_range_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(disc_range);

  PCP_OAM_BFD_DISCRIMINATOR_RANGE_clear(disc_range);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_my_discriminator_range_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This function configures/updates BFD message
 *     transmission information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TX_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_tx_info_set_unsafe()", my_discriminator_ndx, 0);
}

uint32
  pcp_oam_bfd_tx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TX_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(my_discriminator_ndx, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_MAX, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_TX_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_tx_info_set_verify()", my_discriminator_ndx, 0);
}

uint32
  pcp_oam_bfd_tx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TX_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(my_discriminator_ndx, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_MAX, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_tx_info_get_verify()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     This function configures/updates BFD message
 *     transmission information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_TX_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_BFD_TX_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_tx_info_get_unsafe()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     This function configures/updates BFD message reception
 *     and session monitoring information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_RX_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_rx_info_set_unsafe()", my_discriminator_ndx, 0);
}

uint32
  pcp_oam_bfd_rx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_RX_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(my_discriminator_ndx, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_MAX, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_RX_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_rx_info_set_verify()", my_discriminator_ndx, 0);
}

uint32
  pcp_oam_bfd_rx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_RX_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(my_discriminator_ndx, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_MAX, PCP_OAM_BFD_MY_DISCRIMINATOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_rx_info_get_verify()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     This function configures/updates BFD message reception
 *     and session monitoring information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_RX_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_BFD_RX_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_BFD_RX_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_bfd_rx_info_get_unsafe()", my_discriminator_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_bfd module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_bfd_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_oam_bfd;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_bfd module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_bfd_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_oam_bfd;
}
uint32
  PCP_OAM_BFD_COS_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, PCP_OAM_BFD_TC_MAX, PCP_OAM_BFD_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, PCP_OAM_BFD_DP_MAX, PCP_OAM_BFD_DP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tnl_exp, PCP_OAM_BFD_TNL_EXP_MAX, PCP_OAM_BFD_TNL_EXP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pwe_exp, PCP_OAM_BFD_PWE_EXP_MAX, PCP_OAM_BFD_PWE_EXP_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_COS_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_TTL_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tnl_ttl, PCP_OAM_BFD_TNL_TTL_MAX, PCP_OAM_BFD_TNL_TTL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pwe_ttl, PCP_OAM_BFD_PWE_TTL_MAX, PCP_OAM_BFD_PWE_TTL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_TTL_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_IP_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_IP_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_NEGOTIATION_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_NEGOTIATION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->detect_mult, PCP_OAM_BFD_DETECT_MULT_MAX, PCP_OAM_BFD_DETECT_MULT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->min_rx_interval, PCP_OAM_BFD_MIN_RX_INTERVAL_MAX, PCP_OAM_BFD_MIN_RX_INTERVAL_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->min_tx_interval, PCP_OAM_BFD_MIN_TX_INTERVAL_MAX, PCP_OAM_BFD_MIN_TX_INTERVAL_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_NEGOTIATION_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->eep, PCP_OAM_BFD_EEP_MAX, PCP_OAM_BFD_EEP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos, PCP_OAM_BFD_COS_ID_MAX, PCP_OAM_BFD_COS_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, PCP_OAM_BFD_TTL_ID_MAX, PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->eep, PCP_OAM_BFD_EEP_MAX, PCP_OAM_BFD_EEP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pwe, PCP_OAM_BFD_PWE_MAX, PCP_OAM_BFD_PWE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos, PCP_OAM_BFD_COS_ID_MAX, PCP_OAM_BFD_COS_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, PCP_OAM_BFD_TTL_ID_MAX, PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_FWD_MPLS_PWE_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_ip, PCP_OAM_BFD_IP_ID_MAX, PCP_OAM_BFD_IP_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->eep, PCP_OAM_BFD_EEP_MAX, PCP_OAM_BFD_EEP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pwe, PCP_OAM_BFD_PWE_MAX, PCP_OAM_BFD_PWE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos, PCP_OAM_BFD_COS_MAX, PCP_OAM_BFD_COS_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, PCP_OAM_BFD_TTL_ID_MAX, PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_FWD_IP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_ip, PCP_OAM_BFD_SRC_IP_MAX, PCP_OAM_BFD_SRC_IP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos, PCP_OAM_BFD_COS_MAX, PCP_OAM_BFD_COS_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, PCP_OAM_BFD_TTL_ID_MAX, PCP_OAM_BFD_TTL_ID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_FWD_IP_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_FWD_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO, &(info->mpls_tunnel_info), 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_FWD_MPLS_PWE_INFO, &(info->mpls_pwe_info), 11, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO, &(info->mpls_pwe_with_ip_info), 12, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_FWD_IP_INFO, &(info->ip_info), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_FWD_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_TX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_rate, PCP_OAM_BFD_TX_RATE_MAX, PCP_OAM_BFD_TX_RATE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->discriminator, PCP_OAM_BFD_DISCRIMINATOR_MAX, PCP_OAM_BFD_DISCRIMINATOR_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->bfd_type, PCP_OAM_BFD_BFD_TYPE_MAX, PCP_OAM_BFD_BFD_TYPE_OUT_OF_RANGE_ERR, 13, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_NEGOTIATION_INFO, &(info->ng_info), 14, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_BFD_FWD_INFO, &(info->fw_info), 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_TX_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_RX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->life_time, PCP_OAM_BFD_LIFE_TIME_MAX, PCP_OAM_BFD_LIFE_TIME_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_RX_INFO_verify()",0,0);
}

uint32
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_verify(
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->base, PCP_OAM_BFD_BASE_MAX, PCP_OAM_BFD_BASE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->start, PCP_OAM_BFD_START_MAX, PCP_OAM_BFD_START_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->end, PCP_OAM_BFD_END_MAX, PCP_OAM_BFD_END_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_BFD_DISCRIMINATOR_RANGE_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

