/* $Id: ppd_api_oam.c,v 1.35 Broadcom SDK $
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


#undef LINK_PB_LIBRARIES

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_oam.h>
#endif

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_oam.h>

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_oam[] =
{
  /*
  * Auto generated. Do not edit following section {
  */
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_ING_AC_KEY_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_ING_AC_KEY_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_EGR_PORT_VSI_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_EGR_PORT_VSI_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_EGR_PORT_CVID_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_EGR_PORT_CVID_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_EGR_OUT_AC_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_EGR_OUT_AC_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_TRAP_CODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ETH_TRAP_CODE_GET),
  /*ARAD OAM APIS*/
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_DEINIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_ICC_MAP_REGISTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_CLASSIFIER_MEP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_CLASSIFIER_MEP_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_RMEP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_RMEP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_RMEP_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_RMEP_INDEX_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_MEP_DB_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_MEP_DB_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAMP_MEP_DB_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_CLASSIFIER_OEM_MEP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_OEM1_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_OEM1_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_OEM1_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_OEM2_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_OEM2_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_CLASSIFIER_OEM2_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_COUNTER_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_COUNTER_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_IPV4_TOS_TTL_SELECT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_IPV4_TOS_TTL_SELECT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_TX_RATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_TX_RATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_REQ_INTERVAL_POINTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_REQ_INTERVAL_POINTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_MPLS_PWE_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_MPLS_PWE_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_MPLS_UDP_SPORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_MPLS_UDP_SPORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_IPV4_UDP_SPORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_IPV4_UDP_SPORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_PDU_STATIC_REGISTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_PDU_STATIC_REGISTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_MY_BFD_DIP_IPV4_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_MY_BFD_DIP_IPV4_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_TX_IPV4_MULTI_HOP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_TX_IPV4_MULTI_HOP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFD_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFDCC_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFDCC_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFDCV_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_BFDCV_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_OAMP_TX_PRIORITY_REGISTERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_OAMP_TX_PRIORITY_REGISTERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_EVENT_FIFO_READ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_PP_PCT_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_OAM_PP_PCT_PROFILE_GET),
	
  /*
  * } Auto generated. Do not edit previous section.
  */
  /*
  * Last element. Do no touch.
  */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an incoming Attachment-Circuit (port x VLAN x VLAN)
 *     and MD-level, and to determine the action to perform
 *     (see Table 8: Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the
 *     MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_ing_ac_key_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_L2_LIF_AC_KEY           *ac_key,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_ING_AC_KEY_MP_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_ing_ac_key_mp_info_set(unit, ac_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_ing_ac_key_mp_info_set(unit, ac_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_ing_ac_key_mp_info_set()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an incoming Attachment-Circuit (port x VLAN x VLAN)
 *     and MD-level, and to determine the action to perform
 *     (see Table 8: Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the
 *     MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_ing_ac_key_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_L2_LIF_AC_KEY           *ac_key,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_ING_AC_KEY_MP_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_ing_ac_key_mp_info_get(unit, ac_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_ing_ac_key_mp_info_get(unit, ac_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_ing_ac_key_mp_info_get()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x VSI) and
 *     MD-level, and to determine the action to perform (see
 *     Table 8: Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the MP
 *     is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_egr_port_vsi_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY           *port_vsi_key,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_EGR_PORT_VSI_MP_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_vsi_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_egr_port_vsi_mp_info_set(unit, port_vsi_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_egr_port_vsi_mp_info_set(unit, port_vsi_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_egr_port_vsi_mp_info_set()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x VSI) and
 *     MD-level, and to determine the action to perform (see
 *     Table 8: Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the MP
 *     is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_egr_port_vsi_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY           *port_vsi_key,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_EGR_PORT_VSI_MP_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_vsi_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_egr_port_vsi_mp_info_get(unit, port_vsi_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_egr_port_vsi_mp_info_get(unit, port_vsi_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_egr_port_vsi_mp_info_get()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x CVID) and
 *     MD-level, and to determine the action to perform (see
 *     Table 8: Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the MP
 *     is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_egr_port_cvid_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY      *port_cep_key,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_EGR_PORT_CVID_MP_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_cep_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_egr_port_cvid_mp_info_set(unit, port_cep_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_egr_port_cvid_mp_info_set(unit, port_cep_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_egr_port_cvid_mp_info_set()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x CVID) and
 *     MD-level, and to determine the action to perform (see
 *     Table 8: Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the MP
 *     is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_egr_port_cvid_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY      *port_cep_key,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_EGR_PORT_CVID_MP_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_cep_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_egr_port_cvid_mp_info_get(unit, port_cep_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_egr_port_cvid_mp_info_get(unit, port_cep_key, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_egr_port_cvid_mp_info_get()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (Out-AC) and MD-level,
 *     and to determine the action to perform (see Table 8:
 *     Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the MP is one of
 *     the 4K accelerated MEPs, the function configures the
 *     related OAMP databases and associates the AC and
 *     MD-Level with a user-provided handle. This handle is
 *     later used by user to access OAMP database for this MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_egr_out_ac_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_EGR_OUT_AC_MP_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_egr_out_ac_mp_info_set(unit, out_ac_ndx, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_egr_out_ac_mp_info_set(unit, out_ac_ndx, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_egr_out_ac_mp_info_set()",0,0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (Out-AC) and MD-level,
 *     and to determine the action to perform (see Table 8:
 *     Enumerator - SOC_PPC_OAM_ETH_ACC_FUNC_TYPE). If the MP is one of
 *     the 4K accelerated MEPs, the function configures the
 *     related OAMP databases and associates the AC and
 *     MD-Level with a user-provided handle. This handle is
 *     later used by user to access OAMP database for this MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_egr_out_ac_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPD_OAM_ETH_MP_INFO         *info
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_EGR_OUT_AC_MP_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_egr_out_ac_mp_info_get(unit, out_ac_ndx, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_egr_out_ac_mp_info_get(unit, out_ac_ndx, level_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_egr_out_ac_mp_info_get()",0,0);
}

/*********************************************************************
*     This function maps the CFM opcode to local opcode. The
 *     local opcode is used to construct a key to determine the
 *     packet trap code
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_trap_code_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_ACC_FUNC_TYPE   func_type_ndx,
    SOC_SAND_IN  uint32                   opcode_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE          *action_profile
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_TRAP_CODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_trap_code_set(unit, func_type_ndx, opcode_ndx, action_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_trap_code_set(unit, func_type_ndx, opcode_ndx, action_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_trap_code_set()",0,0);
}

/*********************************************************************
*     This function maps the CFM opcode to local opcode. The
 *     local opcode is used to construct a key to determine the
 *     packet trap code
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oam_eth_trap_code_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_OAM_ETH_ACC_FUNC_TYPE   func_type_ndx,
    SOC_SAND_IN  uint32                   opcode_ndx,
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE          *action_profile
  )
{
#if (defined(LINK_PB_LIBRARIES) || defined(LINK_T20E_LIBRARIES))
  uint32
    res = SOC_SAND_OK;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ETH_TRAP_CODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
#ifdef LINK_PB_LIBRARIES
  case SOC_SAND_DEV_PB:
    res = soc_petra_b_oam_eth_trap_code_get(unit, func_type_ndx, opcode_ndx, action_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
    break;
#endif
#ifdef LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    res = t20e_oam_eth_trap_code_get(unit, func_type_ndx, opcode_ndx, action_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_oam_eth_trap_code_get()",0,0);
}

void
  SOC_PPD_OAM_ETH_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OAM_ETH_MP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
*     soc_ppd_api_lif_table module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_oam_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_oam;
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_OAM_ETH_ACC_FUNC_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_OAM_ETH_ACC_FUNC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_0:
    str = "func type 0";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_1:
    str = "func type 1";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_2:
    str = "func type 2";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_3:
    str = "func type 3";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_4:
    str = "func type 4";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_5:
    str = "func type 5";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  SOC_PPD_OAM_ETH_MP_INFO_print(
    SOC_SAND_IN  SOC_PPD_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OAM_ETH_MP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */
/* } */


/*
* ARAD OAM APIs
*/

void
  SOC_PPD_OAM_INIT_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_OAM_INIT_TRAP_INFO *init_trap_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(init_trap_info);

  SOC_PPC_OAM_INIT_TRAP_INFO_clear(init_trap_info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_MEP_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(profile_data);

  SOC_PPC_OAM_MEP_PROFILE_DATA_clear(profile_data);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_LIF_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPD_OAM_LIF_PROFILE_DATA *profile_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(profile_data);

  SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_RMEP_INFO_DATA_clear(
    SOC_SAND_OUT SOC_PPD_OAM_RMEP_INFO_DATA *rmep_info_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(rmep_info_data);

  SOC_PPC_OAM_RMEP_INFO_DATA_clear(rmep_info_data);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);

  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(rmep_db_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_print(
    SOC_SAND_IN SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);

  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_print(rmep_db_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(mep_db_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_print(
    SOC_SAND_IN SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_print(mep_db_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(classifier_entry);

  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(classifier_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_SAT_CTF_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_SAT_CTF_ENTRY *ctf_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(ctf_entry);

  SOC_PPC_OAM_SAT_CTF_ENTRY_clear(ctf_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_SAT_GTF_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_SAT_GTF_ENTRY *gtf_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(gtf_entry);

  SOC_PPC_OAM_SAT_GTF_ENTRY_clear(gtf_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(mep_db_entry); /*A walk across the oam files... Useless!*/
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_DM_INFO_GET_clear(
    SOC_SAND_OUT SOC_PPD_OAM_OAMP_DM_INFO_GET *mep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  SOC_PPC_OAM_OAMP_DM_INFO_GET_clear(mep_db_entry); /*A walk across the oam files... Useless!*/
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_LM_INFO_GET_clear(
    SOC_SAND_OUT SOC_PPD_OAM_OAMP_LM_INFO_GET *mep_db_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  SOC_PPC_OAM_OAMP_LM_INFO_GET_clear(mep_db_entry); /*A walk across the oam files... Useless!*/
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY *entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(entry);

  SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(entry); /*A walk across the oam files... Useless!*/
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_print(
    SOC_SAND_IN SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(classifier_entry);

  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_print(classifier_entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD *oem1_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(oem1_payload);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_print(
    SOC_SAND_IN SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD *oem1_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_print(oem1_payload);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD *oem2_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);

  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(oem2_payload);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_print(
    SOC_SAND_IN SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD *oem2_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);

  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_print(oem2_payload);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY *oem1_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(oem1_key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY *oem2_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);

  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(oem2_key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *oam_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);

  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(oam_payload);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_print(
    SOC_SAND_IN SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *oam_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);

  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_print(oam_payload);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY *oam1_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam1_key);

  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(oam1_key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_CLASSIFIER_OAM2_ENTRY_KEY *oam2_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam2_key);

  SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(oam2_key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_TCAM_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_OAM_TCAM_ENTRY_KEY *oam_tcam_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam_tcam_key);

  SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(oam_tcam_key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_TCAM_ENTRY_ACTION_clear(
    SOC_SAND_OUT SOC_PPD_OAM_TCAM_ENTRY_ACTION *oam_tcam_action
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam_tcam_action);

  SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(oam_tcam_action);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*bfd*/
void
  SOC_PPD_BFD_INIT_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_BFD_INIT_TRAP_INFO *init_trap_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(init_trap_info);

  SOC_PPC_BFD_INIT_TRAP_INFO_clear(init_trap_info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BFD_PDU_STATIC_REGISTER_clear(
    SOC_SAND_OUT SOC_PPD_BFD_PDU_STATIC_REGISTER *bfd_pdu
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);

  SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(bfd_pdu);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER_clear(
     SOC_SAND_OUT SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER *bfd_cc_packet
     )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(bfd_cc_packet);
    
    SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_clear(bfd_cc_packet);
    SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(&(bfd_cc_packet->bfd_static_reg_fields)); 

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA *mpls_pwe_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mpls_pwe_profile);

  SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(mpls_pwe_profile);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(
    SOC_SAND_OUT SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA *tos_ttl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);

  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(tos_ttl_data);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(
    SOC_SAND_OUT SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *itmh_attr
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(itmh_attr);

  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(itmh_attr);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_EVENT_DATA_clear(
    SOC_SAND_OUT SOC_PPD_OAM_EVENT_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OAM_EVENT_DATA_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OAM_RX_REPORT_EVENT_DATA_clear(
    SOC_SAND_OUT SOC_PPD_OAM_RX_REPORT_EVENT_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OAM_RX_REPORT_EVENT_DATA_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*********************************************************************
*     Initialize oam registers and tables
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_init(
    SOC_SAND_IN  int                       unit,
	SOC_SAND_IN  uint8                     is_bfd
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_INIT);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_init,(unit, is_bfd));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_init_print,(unit, is_bfd));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_init()", 0, 0);
}

/*********************************************************************
*     De-Initialize oam registers and tables
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_deinit(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                     is_bfd,
    SOC_SAND_IN  uint8                    tcam_db_destroy
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_DEINIT);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_deinit,(unit, is_bfd, tcam_db_destroy));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_deinit_print,(unit, is_bfd, tcam_db_destroy));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_deinit()", 0, 0);
}

/*********************************************************************
*     Set Icc Map Register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_icc_map_register_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                        icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA   * data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_ICC_MAP_REGISTER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_icc_map_register_set,(unit, icc_ndx, data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_icc_map_register_set,(unit, icc_ndx, data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_icc_map_register_set()", icc_ndx, 0);
}

/*********************************************************************
*     Set OAM1 table default profile entries
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oam1_entries_insert_default_profile(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_data);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oam1_entries_insert_default_profile,
								(unit, profile_data, classifier_mep_entry, is_bfd));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_oam1_entries_insert_default_profile,
					 (unit, profile_data, classifier_mep_entry, is_bfd));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oam1_entries_insert_default_profile()", 0, 0);
}

/*********************************************************************
*     Replace rellevant entries in OEM1 and OEM2
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem_mep_profile_replace(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem_mep_profile_replace,(unit, classifier_mep_entry, update_mp_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_oem_mep_profile_replace,(unit, classifier_mep_entry));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem_mep_profile_replace()", 0, 0);
}

/*********************************************************************
*      Set Icc Map Register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem_mep_add(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry,
	SOC_SAND_IN  uint8                    update
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_CLASSIFIER_OEM_MEP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem_mep_add,(unit, mep_index, classifier_mep_entry, update));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_oem_mep_add,(unit, mep_index, classifier_mep_entry, update));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem_mep_add()", mep_index, 0);
}

/*********************************************************************
*      Delete MEP from classifier
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_mep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_CLASSIFIER_MEP_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_mep_delete,(unit, mep_index, classifier_mep_entry));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_mep_delete,(unit, mep_index, classifier_mep_entry));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_mep_delete()", mep_index, 0);
}

/*********************************************************************
*      Set RMEP
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_rmep_set(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
    SOC_SAND_IN  uint8                    update
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_RMEP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_rmep_set,(unit, rmep_index, rmep_id, mep_index, mep_type, rmep_db_entry, update));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_rmep_set,(unit, rmep_index, rmep_id, mep_index, mep_type, rmep_db_entry, update));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_set()", rmep_index, 0);
}

/*********************************************************************
*      Get RMEP
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_rmep_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_RMEP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_rmep_get,(unit, rmep_index, rmep_db_entry));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_rmep_get,(unit, rmep_index));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_get()", rmep_index, 0);
}

/*********************************************************************
*      Delete RMEP
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_rmep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_RMEP_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_rmep_delete,(unit, rmep_index, rmep_id, mep_index, mep_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_rmep_delete,(unit, rmep_index, rmep_id, mep_index, mep_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_delete()", rmep_index, 0);
}

/*********************************************************************
*     Get RMEP index from Exact match
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_rmep_index_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
	SOC_SAND_OUT uint32                   *rmep_index,
	SOC_SAND_OUT  uint8                   *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_RMEP_INDEX_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_rmep_index_get,(unit, rmep_id, mep_index, mep_type, rmep_index, is_found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_rmep_index_get,(unit, rmep_id, mep_index, mep_type, rmep_index, is_found));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_index_get()", rmep_id, 0);
}

/*********************************************************************
*     Set MEP db entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_mep_db_entry_set(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
	SOC_SAND_IN  uint8                   allocate_icc_ndx,
	SOC_SAND_IN  SOC_PPD_OAM_MA_NAME     name
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_MEP_DB_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);
  SOC_SAND_CHECK_NULL_INPUT(name);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_mep_db_entry_set,(unit, mep_index, mep_db_entry, allocate_icc_ndx, name));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_oamp_mep_db_entry_set,(unit, mep_index, mep_db_entry, allocate_icc_ndx, name));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_mep_db_entry_set()", mep_index, 0);
}

/*********************************************************************
*     Get MEP db entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_mep_db_entry_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_MEP_DB_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_mep_db_entry_get,(unit, mep_index, mep_db_entry));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_oamp_mep_db_entry_set,(unit, mep_index, mep_db_entry));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_mep_db_entry_get()", mep_index, 0);
}

/*********************************************************************
*     Delete MEP db entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_mep_db_entry_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
	SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	SOC_SAND_IN  uint8                    is_last_mep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAMP_MEP_DB_ENTRY_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_mep_db_entry_delete,(unit, mep_index, mep_db_entry, deallocate_icc_ndx, is_last_mep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_oamp_mep_db_entry_delete,(unit, mep_index, deallocate_icc_ndx, is_last_mep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_mep_db_entry_delete()", mep_index, 0);
}

/*********************************************************************
*     Delete MEP db entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
	SOC_SAND_IN  uint8                    md_level,
	SOC_SAND_IN  uint8                    is_upmep,
	SOC_SAND_OUT uint8                    *found_mep,
	SOC_SAND_OUT uint32                   *profile,
	SOC_SAND_OUT uint8                    *found_profile,
	SOC_SAND_OUT uint8                    *is_mp_type_flexible,
    SOC_SAND_OUT uint8                    *is_mip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_find_mep_and_profile_by_lif_and_mdlevel,(unit, lif, md_level, is_upmep, found_mep, profile, found_profile, is_mp_type_flexible, is_mip));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_classifier_find_mep_and_profile_by_lif_and_mdlevel,(unit, lif, md_level, is_upmep, found_mep, profile, found_profile, is_mp_type_flexible, is_mip));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel()", lif, md_level);
}

/*********************************************************************
*     Set OEM1 classifier entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem1_entry_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_OEM1_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem1_entry_set,(unit, oem1_key, oem1_payload));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_oamp_mep_db_entry_set,(unit, oem1_key, oem1_payload));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem1_entry_set()", 0, 0);
}

/*********************************************************************
*     Get OEM1 classifier entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem1_entry_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_OEM1_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem1_entry_get,(unit, oem1_key, oem1_payload, is_found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_classifier_oem1_entry_get,(unit, oem1_key, oem1_payload, is_found));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem1_entry_get()", 0, 0);
}

/*********************************************************************
*     Delete OEM1 classifier entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem1_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_OEM1_ENTRY_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oem1_key);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem1_entry_delete,(unit, oem1_key));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_oem1_entry_delete,(unit, oem1_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem1_entry_delete()", 0, 0);
}

/*********************************************************************
*     Set OEM2 classifier entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem2_entry_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   *oem2_payload
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_OEM2_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem2_entry_set,(unit, oem2_key, oem2_payload));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_oamp_mep_db_entry_set,(unit, oem2_key, oem2_payload));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem2_entry_set()", 0, 0);
}

/*********************************************************************
*     Get OEM2 classifier entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem2_entry_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_OEM2_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem2_entry_get,(unit, oem2_key, oem2_payload, is_found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(oam_classifier_oem1_entry_get,(unit, oem2_key, oem2_payload, is_found));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem2_entry_get()", 0, 0);
}

/*********************************************************************
*     Delete OEM2 classifier entry
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_classifier_oem2_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_CLASSIFIER_OEM2_ENTRY_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oem2_key);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_oem2_entry_delete,(unit, oem2_key));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_oem2_entry_delete,(unit, oem2_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem2_entry_delete()", 0, 0);
}

/*********************************************************************
*     Set OAM counter range
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_counter_range_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_COUNTER_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_counter_range_set,(unit, counter_range_min, counter_range_max));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_counter_range_set,(unit, counter_range_min, counter_range_max));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_counter_range_set()", 0, 0);
}

/*********************************************************************
*     Get OAM counter range
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_counter_range_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_OUT  uint32                                     *counter_range_min,
	SOC_SAND_OUT  uint32                                     *counter_range_max
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_COUNTER_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_range_min);
  SOC_SAND_CHECK_NULL_INPUT(counter_range_max);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_counter_range_get,(unit, counter_range_min, counter_range_max));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_counter_range_get,(unit, counter_range_min, counter_range_max));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_counter_range_get()", 0, 0);
}

/*********************************************************************
*     Configure mapping of network opcode to internal opcode.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_eth_oam_opcode_map_set(
    SOC_SAND_IN   int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_eth_oam_opcode_map_set,(unit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_eth_oam_opcode_map_set,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_eth_oam_opcode_map_set()", 0, 0);
}

/* 
 * BFD APIs
 */
/*********************************************************************
*     Set BFD IPV4 TOS TTL Select register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_IPV4_TOS_TTL_SELECT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_ipv4_tos_ttl_select_set,(unit, ipv4_tos_ttl_select_index, tos_ttl_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_ipv4_tos_ttl_select_set,(unit, ipv4_tos_ttl_select_index, tos_ttl_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_tos_ttl_select_set()", ipv4_tos_ttl_select_index, 0);
}

/*********************************************************************
*     Get BFD IPV4 TOS TTL Select register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_ipv4_tos_ttl_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_OUT SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_IPV4_TOS_TTL_SELECT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_ipv4_tos_ttl_select_get,(unit, ipv4_tos_ttl_select_index, tos_ttl_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_ipv4_tos_ttl_select_get,(unit, ipv4_tos_ttl_select_index, tos_ttl_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_tos_ttl_select_get()", ipv4_tos_ttl_select_index, 0);
}

/*********************************************************************
*     Set BFD IPV4 src address Select register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_ipv4_src_addr_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_ipv4_src_addr_select_set,(unit, ipv4_src_addr_select_index, src_addr));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_ipv4_src_addr_select_set,(unit, ipv4_tos_ttl_select_index, tos_ttl_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_src_addr_select_set()", ipv4_src_addr_select_index, 0);
}

/*********************************************************************
*     Get BFD IPV4 src address Select register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_ipv4_src_addr_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_OUT uint32                                       *src_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_addr);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_ipv4_src_addr_select_get,(unit, ipv4_src_addr_select_index, src_addr));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_ipv4_src_addr_select_set,(unit, ipv4_tos_ttl_select_index, tos_ttl_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_src_addr_select_get()", ipv4_src_addr_select_index, 0);
}


/*********************************************************************
*     Set BFD Tx Rate register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_tx_rate_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_TX_RATE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_tx_rate_set,(unit, bfd_tx_rate_index, tx_rate));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_tx_rate_set,(unit, bfd_tx_rate_index, tx_rate));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_rate_set()", bfd_tx_rate_index, 0);
}

/*********************************************************************
*     Get BFD Tx Rate register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_tx_rate_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_OUT uint32                                       *tx_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_TX_RATE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tx_rate);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_tx_rate_get,(unit, bfd_tx_rate_index, tx_rate));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_tx_rate_get,(unit, bfd_tx_rate_index, tx_rate));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_rate_get()", bfd_tx_rate_index, 0);
}


/*********************************************************************
*     Set BFD Req Interval Pointer register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_req_interval_pointer_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_REQ_INTERVAL_POINTER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_req_interval_pointer_set,(unit, req_interval_pointer, req_interval));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_req_interval_pointer_set,(unit, req_interval_pointer, req_interval));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_req_interval_pointer_set()", req_interval_pointer, 0);
}

/*********************************************************************
*     Get BFD Req Interval Pointer register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_req_interval_pointer_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_OUT uint32                                       *req_interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_REQ_INTERVAL_POINTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(req_interval);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_req_interval_pointer_get,(unit, req_interval_pointer, req_interval));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_req_interval_pointer_get,(unit, req_interval_pointer, req_interval));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_req_interval_pointer_get()", req_interval_pointer, 0);
}

/*********************************************************************
*     Set BFD MPLS PWE Profile register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_mpls_pwe_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_MPLS_PWE_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(push_data);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_mpls_pwe_profile_set,(unit, push_profile, push_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_mpls_pwe_profile_set,(unit, push_profile, push_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mpls_pwe_profile_set()", push_profile, 0);
}


/*********************************************************************
*     Get BFD MPLS PWE Profile register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_mpls_pwe_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_OUT SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_MPLS_PWE_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(push_data);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_mpls_pwe_profile_get,(unit, push_profile, push_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_mpls_pwe_profile_get,(unit, push_profile, push_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mpls_pwe_profile_get()", push_profile, 0);
}


/*********************************************************************
*     Set BFD MPLS Udp Sport register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_mpls_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_MPLS_UDP_SPORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_mpls_udp_sport_set,(unit, udp_sport));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_mpls_pwe_profile_set,(unit, udp_sport));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mpls_pwe_profile_set()", 0, 0);
}

/*********************************************************************
*     Get BFD MPLS Udp Sport register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_mpls_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT uint16                                       *udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_MPLS_UDP_SPORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(udp_sport);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_mpls_udp_sport_get,(unit, udp_sport));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_mpls_pwe_profile_get,(unit, udp_sport));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mpls_pwe_profile_get()", 0, 0);
}

/*********************************************************************
*     Set BFD IPV4 Udp Sport register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_ipv4_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_IPV4_UDP_SPORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_ipv4_udp_sport_set,(unit, udp_sport));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_ipv4_udp_sport_set,(unit, udp_sport));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_udp_sport_set()", 0, 0);
}

/*********************************************************************
*     Get BFD IPV4 Udp Sport register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_ipv4_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint16                                      *udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_IPV4_UDP_SPORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(udp_sport);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_ipv4_udp_sport_get,(unit, udp_sport));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_ipv4_udp_sport_get,(unit, udp_sport));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_udp_sport_get()", 0, 0);
}

/*********************************************************************
*     Set BFD Pdu static register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_pdu_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_PDU_STATIC_REGISTER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_pdu_static_register_set,(unit, bfd_pdu));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_pdu_static_register_set,(unit, bfd_pdu));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_pdu_static_register_set()", 0, 0);
}

/*********************************************************************
*     Get BFD Pdu static register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_pdu_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_PDU_STATIC_REGISTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_pdu_static_register_get,(unit, bfd_pdu));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_pdu_static_register_get,(unit, bfd_pdu));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_pdu_static_register_get()", 0, 0);
}

/*********************************************************************
*     Set BFD  CC packet static register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_cc_packet_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bfd_cc_packet);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_cc_packet_static_register_set,(unit, bfd_cc_packet));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_pdu_static_register_set,(unit, bfd_cc_packet));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_cc_packet_static_register_set()", 0, 0);
}

/*********************************************************************
*     Get BFD  CC packet static register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_cc_packet_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bfd_cc_packet);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_cc_packet_static_register_get,(unit, bfd_cc_packet));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_pdu_static_register_get,(unit, bfd_cc_packet));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_cc_packet_static_register_get()", 0, 0);
}

/*********************************************************************
*     Set BFD discriminator range registers
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_discriminator_range_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_discriminator_range_registers_set,(unit, range));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_discriminator_range_registers_set,(unit, bfd_pdu));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_discriminator_range_registers_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_discriminator_range_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint32  						              *range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_discriminator_range_registers_get,(unit, range));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_discriminator_range_registers_get,(unit, bfd_pdu));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_discriminator_range_registers_get()", 0, 0);
}

/*********************************************************************
*     Set My Bfd Dip table with IPv4 values register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_my_bfd_dip_ip_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS					*dip							
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_MY_BFD_DIP_IPV4_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_my_bfd_dip_ip_set,(unit, dip_index, dip));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_my_bfd_dip_ip_set,(unit, dip_index, dip));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_my_bfd_dip_ipv4_set()", dip_index, 0);
}

uint32
  soc_ppd_oam_bfd_my_bfd_dip_ip_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS					*dip							
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_MY_BFD_DIP_IPV4_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_my_bfd_dip_ip_get,(unit, dip_index, dip));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_my_bfd_dip_ip_get,(unit, dip_index, dip));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_my_bfd_dip_ipv4_get()", dip_index, 0);
}

/*********************************************************************
*     Set Bfd Tx Ipv4 Multi Hop register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_TX_IPV4_MULTI_HOP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_tx_ipv4_multi_hop_set,(unit, tx_ipv4_multi_hop_att));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_tx_ipv4_multi_hop_set,(unit, tx_ipv4_multi_hop_att));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_ipv4_multi_hop_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES               *tx_ipv4_multi_hop_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_BFD_TX_IPV4_MULTI_HOP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_tx_ipv4_multi_hop_get,(unit, tx_ipv4_multi_hop_att));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_tx_ipv4_multi_hop_get,(unit, tx_ipv4_multi_hop_att));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_ipv4_multi_hop_get()", 0, 0);
}

 


/*********************************************************************
*     Set OAMP priority TC and DP registers
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_tx_priority_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_OAMP_TX_PRIORITY_REGISTERS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_tx_priority_registers_set,(unit, priority, tx_oam_att));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_tx_priority_registers_set,(unit, priority, tx_oam_att));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_tx_priority_registers_set()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_tx_priority_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_OUT  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_OAMP_TX_PRIORITY_REGISTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_tx_priority_registers_get,(unit, priority, tx_oam_att));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_tx_priority_registers_get,(unit, priority, tx_oam_att));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_tx_priority_registers_get()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_enable_interrupt_message_event_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_enable_interrupt_message_event_set,(unit, interrupt_message_event_bmp));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_enable_interrupt_message_event_set,(unit, interrupt_message_event_bmp));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_enable_interrupt_message_event_set()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_enable_interrupt_message_event_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint8                                       *interrupt_message_event_bmp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_enable_interrupt_message_event_get,(unit, interrupt_message_event_bmp));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_enable_interrupt_message_event_get,(unit, interrupt_message_event_bmp));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_enable_interrupt_message_event_get()", 0, 0);
}

/*********************************************************************
*     Read oam event fifo
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_event_fifo_read(
    SOC_SAND_IN   int                                           unit, 
    SOC_SAND_IN   int                                           event_type,
    SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
    SOC_SAND_OUT  uint32                                       *event_id,
    SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *event_data,
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA               *interrupt_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_EVENT_FIFO_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_event_fifo_read,(unit, event_type, rmeb_db_ndx, event_id, valid, event_data, interrupt_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_event_fifo_read,(unit, rmeb_db_ndx, event_id, valid, event_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_event_fifo_read()", 0, 0);
}

/*********************************************************************
*     Set PP_PCT oam port profile field
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_pp_pct_profile_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_PP_PCT_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_pp_pct_profile_set,(unit, core_id, local_port_ndx, oam_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_pp_pct_profile_set,(unit, core_id, local_port_ndx, oam_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_pp_pct_profile_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Get PP_PCT oam port profile field
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_pp_pct_profile_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_OAM_PP_PCT_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_pp_pct_profile_get,(unit, core_id, local_port_ndx, oam_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_pp_pct_profile_get,(unit, local_port_ndx, oam_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_pp_pct_profile_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Set bfd diag profile
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_diag_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_diag_profile_set,(unit, profile_ndx, diag_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_diag_profile_set,(unit, profile_ndx, diag_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_diag_profile_set()", profile_ndx, 0);
}

uint32
  soc_ppd_oam_bfd_diag_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                      *diag_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_diag_profile_get,(unit, profile_ndx, diag_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_diag_profile_get,(unit, profile_ndx, diag_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_diag_profile_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set bfd flags profile
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_bfd_flags_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_flags_profile_set,(unit, profile_ndx, flags_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_flags_profile_set,(unit, profile_ndx, flags_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_diag_profile_set()", profile_ndx, 0);
}

uint32
  soc_ppd_oam_bfd_flags_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                      *flags_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_bfd_flags_profile_get,(unit, profile_ndx, flags_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_bfd_flags_profile_get,(unit, profile_ndx, flags_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_flags_profile_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set oam_mep_passive_active_enable register - MP_type of each profile
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_mep_passive_active_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_mep_passive_active_enable_set,(unit, profile_ndx, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_mep_passive_active_enable_set,(unit, profile_ndx, enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mep_passive_active_enable_set()", profile_ndx, 0);
}

uint32
  soc_ppd_oam_mep_passive_active_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_mep_passive_active_enable_get,(unit, profile_ndx, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_mep_passive_active_enable_get,(unit, profile_ndx, enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mep_passive_active_enable_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set oamp_punt_event_hendling register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_punt_event_hendling_profile_set,(unit, profile_ndx, punt_profile_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_punt_event_hendling_profile_set,(unit, profile_ndx, punt_profile_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_punt_event_hendling_profile_set()", profile_ndx, 0);
}

uint32
  soc_ppd_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_punt_event_hendling_profile_get,(unit, profile_ndx, punt_profile_data));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_punt_event_hendling_profile_get,(unit, profile_ndx, punt_profile_data));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_punt_event_hendling_profile_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set PORT2CPU OAMP register fields trap id and system port with the given
*       values according to the error type.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_error_trap_id_and_destination_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                         dest_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_error_trap_id_and_destination_set,(unit, trap_type, trap_id, dest_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_error_trap_id_and_destination_set,(unit, trap_type, trap_id, dest_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_error_trap_id_and_destination_set()", trap_type, 0);
}

uint32
  soc_ppd_oam_oamp_error_trap_id_and_destination_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  SOC_TMC_DEST_INFO                         *dest_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_error_trap_id_and_destination_get,(unit, trap_type, trap_id, dest_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_oamp_error_trap_id_and_destination_get,(unit, trap_type, trap_id, dest_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_error_trap_id_and_destination_get()", trap_type, 0);
}

/*********************************************************************
*     Manage oamp LM, DM sessions.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_lm_dm_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_lm_dm_set,(unit, mep_db_entry));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_oamp_lm_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_LM_INFO_GET     *lm_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_lm_get,(unit, lm_info));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_oamp_dm_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_dm_get,(unit, dm_info, is_1dm));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
*     Index get 
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_ppd_oam_oamp_next_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_next_index_get,(unit, endpoint_id,next_index, has_dm ));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
*     Profiles get, LM,DM find.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_eth1731_and_oui_profiles_get,(unit, endpoint_id,eth1731_prof,da_oui_prof));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
    soc_ppd_oam_oamp_nic_profile_get(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_nic_profile_get,(unit, endpoint_id,da_nic_prof));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
    soc_ppd_oam_oamp_search_for_lm_dm(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *found_bitmap
       )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_search_for_lm_dm,(unit, endpoint_id,found_bitmap));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



/*********************************************************************
*     eth1731 profile. 
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_create_new_eth1731_profile(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_create_new_eth1731_profile,(unit, endpoint_id,profile_indx,was_previously_alloced, eth1731_profile));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
*     eth1731 profile. 
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_oamp_set_oui_nic_registers(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_set_oui_nic_registers,(unit, endpoint_id,msb_to_oui,lsb_to_nic, profile_indx_oui, profile_indx_nic));


    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
*     Remove LM/DM entries. 
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_ppd_oam_oamp_lm_dm_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN uint8                           exists_or_removing_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_lm_dm_remove,(unit, endpoint_id,is_lm,exists_or_removing_piggy_back_down, num_removed , removed_index));


    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}

/*********************************************************************
* NAME:
*   soc_ppd_oam_classifier_counter_disable_map_set,
*   Set classifier counter disable map
*   More details in header file. 
*********************************************************************/
uint32
  soc_ppd_oam_classifier_counter_disable_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_counter_disable_map_set,(unit, packet_is_oam,profile,counter_enable));
    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_counter_disable_map_set,(unit, packet_is_oam,profile,counter_enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_counter_disable_map_set()", 0, 0);
}

uint32
  soc_ppd_oam_classifier_counter_disable_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                  *counter_enable
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_classifier_counter_disable_map_get,(unit, packet_is_oam,profile,counter_enable));


    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_oam_classifier_counter_disable_map_get,(unit, packet_is_oam,profile,counter_enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_counter_disable_map_get()", 0, 0);
}

/*********************************************************************
*     Remove loopback
*     Details: in the H file.
**********************************************************************/


uint32 
    soc_ppd_oam_oamp_loopback_remove(
       SOC_SAND_IN  int                                                  unit
       )
{
        uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_loopback_remove,(unit));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32 
    soc_ppd_oam_dma_reset(
       SOC_SAND_IN  int                                                  unit
       )
{
        uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_dma_reset,(unit));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    soc_ppd_oam_dma_clear(
       SOC_SAND_IN  int                                                  unit
       )
{
        uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_dma_clear,(unit));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



/*********************************************************************
*     Configure OAMP supported trap codes per endpoint type
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 
    soc_ppd_oam_oamp_rx_trap_codes_set(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       )
{
        uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_rx_trap_codes_set,(unit, mep_type, trap_code));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    soc_ppd_oam_oamp_rx_trap_codes_delete(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       )
{
        uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(oam_oamp_rx_trap_codes_delete,(unit, mep_type, trap_code));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/*********************************************************************
*     Get crps counter memory
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_oam_get_crps_counter(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                           crps_counter_number,
    SOC_SAND_IN  uint32                                  reg_number,
    SOC_SAND_OUT uint32*                               value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);



  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(get_crps_counter,(unit, crps_counter_number, reg_number, value));


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_get_crps_counter,(unit, crps_counter_number, reg_number, value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_get_crps_counter()", 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

