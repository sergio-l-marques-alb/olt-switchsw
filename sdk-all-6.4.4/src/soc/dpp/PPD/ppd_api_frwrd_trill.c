/* $Id: ppd_api_frwrd_trill.c,v 1.18 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_frwrd_trill.c
*
* MODULE PREFIX:  soc_ppd_frwrd
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
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_trill.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_frwrd_trill.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_trill.h>
#endif
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
    Ppd_procedure_desc_element_frwrd_trill[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_ADJ_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_ADJ_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_ADJ_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_ADJ_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_TRILL_GET_PROCS_PTR),
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
*     Set the key type of TRILL multicast routes lookup. The
 *     following fields are optional: Ing-Nick-key;
 *     Adjacent-EEP-key; FID-key
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_multicast_key_mask_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_multicast_key_mask_set,(unit, masked_fields));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_multicast_key_mask_set_print,(unit,masked_fields));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_key_mask_set()", 0, 0);
}

/*********************************************************************
*     Set the key type of TRILL multicast routes lookup. The
 *     following fields are optional: Ing-Nick-key;
 *     Adjacent-EEP-key; FID-key
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_multicast_key_mask_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PPD_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_multicast_key_mask_get,(unit, masked_fields));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_multicast_key_mask_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_key_mask_get()", 0, 0);
}

/*********************************************************************
*     Map nick-name to a FEC entry ID. Used for forwarding
 *     packets with the nick name as destination to the FEC,
 *     and to associate the FEC as learning information, upon
 *     receiving packets with the Nick-Name as the source
 *     address
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_unicast_route_add(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID              lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB) {

/*  SOC_PB_PP_DEVICE_CALL(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD,(unit, lif_index, nickname_key, learn_info, success));*/
  } else {
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_unicast_route_add,(unit, nickname_key, learn_info, success));
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_unicast_route_add_print,(unit,lif_index,nickname_key,trill_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_unicast_route_add()", nickname_key, 0);
}

/*********************************************************************
*     Get mapping of TRILL nickname to FEC ID and LIF index
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_unicast_route_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     nickname_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID            *lif_index,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT uint8                     *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(learn_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB) {
      
/*  SOC_PB_PP_DEVICE_CALL(frwrd_trill_unicast_route_get,(unit, nickname_key, lif_index, trill_info, is_found));*/
  } else {
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_unicast_route_get,(unit, nickname_key, learn_info, is_found));
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_unicast_route_get_print,(unit,nickname_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_unicast_route_get()", nickname_key, 0);
}

/*********************************************************************
*     Remove TRILL nick-name
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_unicast_route_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB) {
    SOC_PB_PP_DEVICE_CALL(frwrd_trill_unicast_route_remove,(unit, nickname_key, lif_index));
  } else {
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_unicast_route_remove,(unit, nickname_key));
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_unicast_route_remove_print,(unit,nickname_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_unicast_route_remove()", nickname_key, 0);
}


/*********************************************************************
*     Map nick-name to a FEC entry ID. Used for forwarding
 *     packets with the nick name as destination to the FEC,
 *     and to associate the FEC as learning information, upon
 *     receiving packets with the Nick-Name as the source
 *     address
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_ingress_lif_add(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID              lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_ingress_lif_add,(unit, lif_index, nickname_key, trill_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_ingress_lif_add_print,(unit,lif_index,nickname_key,trill_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_ingress_lif_add()", nickname_key, 0);
}

/*********************************************************************
*     Get mapping of TRILL nickname to FEC ID and LIF index
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_ingress_lif_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     nickname_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID            *lif_index,
    SOC_SAND_OUT SOC_PPD_L2_LIF_TRILL_INFO *trill_info,
    SOC_SAND_OUT uint8                     *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_ingress_lif_get,(unit, nickname_key, lif_index, trill_info, is_found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_ingress_lif_get_print,(unit,nickname_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_ingress_lif_get()", nickname_key, 0);
}

/*********************************************************************
*     Remove TRILL nick-name
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_ingress_lif_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_ingress_lif_remove,(unit, nickname_key, lif_index));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_ingress_lif_remove_print,(unit,nickname_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_ingress_lif_remove()", nickname_key, 0);
}

/*********************************************************************
*     Map a TRILL distribution tree to a FEC
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_multicast_route_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                    mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_multicast_route_add,(unit, trill_mc_key, mc_id, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_multicast_route_add_print,(unit,trill_mc_key,mc_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_route_add()", 0, 0);
}

/*********************************************************************
*     Get Mapping of TRILL distribution tree to a FEC
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_multicast_route_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                    *mc_id,
    SOC_SAND_OUT uint8                   *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_multicast_route_get,(unit, trill_mc_key, mc_id, is_found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_multicast_route_get_print,(unit,trill_mc_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_route_get()", 0, 0);
}

/*********************************************************************
*     Remove a TRILL distribution tree mapping
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_multicast_route_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_multicast_route_remove,(unit, trill_mc_key));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_multicast_route_remove_print,(unit,trill_mc_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_route_remove()", 0, 0);
}

/*********************************************************************
*     Map SA MAC adress to expected adjacent EEP and expected
 *     system port in SA-Based_adj db. Used for authenticating
 *     incoming trill packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_adj_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS         *mac_address_key,
    SOC_SAND_IN  SOC_PPD_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                   enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_ADJ_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_adj_info_set,(unit, mac_address_key, mac_auth_info, enable, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_adj_info_set_print,(unit,mac_address_key,mac_auth_info,enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_adj_info_set()", 0, 0);
}

/*********************************************************************
*     Map SA MAC adress to expected adjacent EEP and expected
 *     system port in SA-Based_adj db. Used for authenticating
 *     incoming trill packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_adj_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS         *mac_address_key,
    SOC_SAND_OUT SOC_PPD_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                   *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_ADJ_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_adj_info_get,(unit, mac_address_key, mac_auth_info, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_adj_info_get_print,(unit,mac_address_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_adj_info_get()", 0, 0);
}

/*********************************************************************
*     Set TRILL global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_global_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_global_info_set,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_global_info_set_print,(unit,glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_global_info_set()", 0, 0);
}

/*********************************************************************
*     Set TRILL global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_global_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_trill_global_info_get,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_global_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_global_info_get()", 0, 0);
}
/*********************************************************************
*    Sets the inner tpid - used for fine-grained trill
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_native_inner_tpid_add(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             tpid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_NATIVE_INNER_TPID_ADD);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_native_inner_tpid_add,(unit, tpid, success ));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_native_inner_tpid_add_print,(unit));
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_native_inner_tpid_add()", 0, 0);
    
}
/*********************************************************************
 *    Map high-vid, low-vid to vsi. Used for FGL forwarding
 *    for  VL only high-vid is used.
 *    Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_trill_vsi_entry_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_VSI_ENTRY_ADD);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_vsi_entry_add,(unit, vsi, flags, high_vid, low_vid, success ));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_vsi_entry_add_print,(unit));
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_vsi_entry_add()", 0, 0);

}
uint32
  soc_ppd_frwrd_trill_vsi_entry_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_TRILL_VSI_ENTRY_REMOVE);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_trill_vsi_entry_remove,(unit, vsi, flags, high_vid, low_vid ));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_trill_vsi_entry_remove_print,(unit));
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_vsi_entry_remove()", 0, 0);

}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_frwrd_trill module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_trill_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_frwrd_trill;
}
void
  SOC_PPD_TRILL_MC_MASKED_FIELDS_clear(
    SOC_SAND_OUT SOC_PPD_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRILL_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRILL_ADJ_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRILL_ADJ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_ADJ_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

void
  SOC_PPD_TRILL_MC_MASKED_FIELDS_print(
    SOC_SAND_IN  SOC_PPD_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_MASKED_FIELDS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRILL_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_ROUTE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRILL_ADJ_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRILL_ADJ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_ADJ_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_frwrd_trill_multicast_key_mask_set_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "masked_fields:")));
  SOC_PPD_TRILL_MC_MASKED_FIELDS_print((masked_fields));

  return;
}
void
  soc_ppd_frwrd_trill_multicast_key_mask_get_print(
    SOC_SAND_IN  int                   unit
  )
{

  return;
}
void
  soc_ppd_frwrd_trill_unicast_route_add_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID               lif_index,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO    *trill_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_index: %lu\n\r"),lif_index));

  LOG_CLI((BSL_META_U(unit,
                      "nickname_key: %lu\n\r"),nickname_key));

  LOG_CLI((BSL_META_U(unit,
                      "fec_id: %lu\n\r"),fec_id));
  LOG_CLI((BSL_META_U(unit,
                      "learn_enable: %lu\n\r"),trill_info->learn_enable));
  return;
}
void
  soc_ppd_frwrd_trill_unicast_route_get_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "nickname_key: %lu\n\r"),nickname_key));

  return;
}
void
  soc_ppd_frwrd_trill_unicast_route_remove_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "nickname_key: %lu\n\r"),nickname_key));

  return;
}
void
  soc_ppd_frwrd_trill_ingress_lif_add_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID               lif_index,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO    *trill_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_index: %lu\n\r"),lif_index));

  LOG_CLI((BSL_META_U(unit,
                      "nickname_key: %lu\n\r"),nickname_key));

  LOG_CLI((BSL_META_U(unit,
                      "fec_id: %lu\n\r"),fec_id));
  LOG_CLI((BSL_META_U(unit,
                      "learn_enable: %lu\n\r"),trill_info->learn_enable));
  return;
}
void
  soc_ppd_frwrd_trill_ingress_lif_get_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "nickname_key: %lu\n\r"),nickname_key));

  return;
}
void
  soc_ppd_frwrd_trill_ingress_lif_remove_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "nickname_key: %lu\n\r"),nickname_key));

  return;
}
void
  soc_ppd_frwrd_trill_multicast_route_add_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                    mc_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trill_mc_key:")));
  SOC_PPD_TRILL_MC_ROUTE_KEY_print((trill_mc_key));

  LOG_CLI((BSL_META_U(unit,
                      "mc_id: %lu\n\r"),mc_id));

  return;
}
void
  soc_ppd_frwrd_trill_multicast_route_get_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trill_mc_key:")));
  SOC_PPD_TRILL_MC_ROUTE_KEY_print((trill_mc_key));

  return;
}
void
  soc_ppd_frwrd_trill_multicast_route_remove_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trill_mc_key:")));
  SOC_PPD_TRILL_MC_ROUTE_KEY_print((trill_mc_key));

  return;
}
void
  soc_ppd_frwrd_trill_adj_info_set_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS         *mac_address_key,
    SOC_SAND_IN  SOC_PPD_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                   enable
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_address_key:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((mac_address_key));

  LOG_CLI((BSL_META_U(unit,
                      "mac_auth_info:")));
  SOC_PPD_TRILL_ADJ_INFO_print((mac_auth_info));

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),enable));

  return;
}
void
  soc_ppd_frwrd_trill_adj_info_get_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS         *mac_address_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_address_key:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((mac_address_key));

  return;
}
void
  soc_ppd_frwrd_trill_global_info_set_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_info:")));
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_print((glbl_info));

  return;
}
void
  soc_ppd_frwrd_trill_global_info_get_print(
    SOC_SAND_IN  int                   unit
  )
{

  return;
}
void
  soc_ppd_frwrd_trill_native_inner_tpid_add_print(
    SOC_SAND_IN  int                   unit
  )
{
  return;
}
void
  soc_ppd_frwrd_trill_vsi_entry_add_print(
    SOC_SAND_IN  int                   unit
  )
{
  return;
}


#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

