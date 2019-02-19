/* $Id: ppd_api_llp_parse.c,v 1.12 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_llp_parse.c
*
* MODULE PREFIX:  soc_ppd_llp
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
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_parse.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_llp_parse.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_parse.h>
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
    Ppd_procedure_desc_element_llp_parse[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_VALUES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_VALUES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_VALUES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_VALUES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_PARSE_GET_PROCS_PTR),
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
*     Sets the global information for link layer parsing,
 *     including TPID values. Used in ingress to identify VLAN
 *     tags on incoming packets, and used in egress to
 *     construct VLAN tags on outgoing packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_values_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_TPID_VALUES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tpid_vals);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_tpid_values_set,(unit, tpid_vals));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_tpid_values_set_print,(unit,tpid_vals));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_tpid_values_set()", 0, 0);
}

/*********************************************************************
*     Sets the global information for link layer parsing,
 *     including TPID values. Used in ingress to identify VLAN
 *     tags on incoming packets, and used in egress to
 *     construct VLAN tags on outgoing packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_values_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_TPID_VALUES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tpid_vals);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_tpid_values_get,(unit, tpid_vals));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_tpid_values_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_tpid_values_get()", 0, 0);
}

/*********************************************************************
*     Sets the TPID profile selection of two TPIDs from the
 *     Global TPIDs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_tpid_profile_info_set,(unit, tpid_profile_ndx, tpid_profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_tpid_profile_info_set_print,(unit,tpid_profile_ndx,tpid_profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_tpid_profile_info_set()", tpid_profile_ndx, 0);
}

/*********************************************************************
*     Sets the TPID profile selection of two TPIDs from the
 *     Global TPIDs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_tpid_profile_info_get,(unit, tpid_profile_ndx, tpid_profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_tpid_profile_info_get_print,(unit,tpid_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_tpid_profile_info_get()", tpid_profile_ndx, 0);
}

/*********************************************************************
*     Maps from Port profile to TPID Profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint32                                tpid_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_port_profile_to_tpid_profile_map_set,(unit, port_profile_ndx, tpid_profile_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set_print,(unit,port_profile_ndx,tpid_profile_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set()", port_profile_ndx, 0);
}

/*********************************************************************
*     Maps from Port profile to TPID Profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_OUT uint32                                *tpid_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_id);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_port_profile_to_tpid_profile_map_get,(unit, port_profile_ndx, tpid_profile_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get_print,(unit,port_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get()", port_profile_ndx, 0);
}

/*********************************************************************
*     Given the port profile and the parsing information
 *     determine: - Whether this packet format accepted or
 *     denied. - The tag structure of the packet, i.e. what
 *     vlan tags exist on the packet (S-tag, S-C-tag, etc...).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_packet_format_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
  case SOC_SAND_DEV_PB:
    SOC_PB_PP_DEVICE_CALL(llp_parse_packet_format_info_set,(unit, port_profile_ndx, parse_key, format_info));
    break;
  case SOC_SAND_DEV_ARAD:
    SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_packet_format_info_set,(unit, port_profile_ndx, parse_key, format_info));
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 10, exit);
    break;
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_packet_format_info_set_print,(unit,port_profile_ndx,parse_key,format_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_packet_format_info_set()", port_profile_ndx, 0);
}

/*********************************************************************
*     Given the port profile and the parsing information
 *     determine: - Whether this packet format accepted or
 *     denied. - The tag structure of the packet, i.e. what
 *     vlan tags exist on the packet (S-tag, S-C-tag, etc...).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_parse_packet_format_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
  case SOC_SAND_DEV_PB:
    SOC_PB_PP_DEVICE_CALL(llp_parse_packet_format_info_get,(unit, port_profile_ndx, parse_key, format_info));
    break;
  case SOC_SAND_DEV_ARAD:
    SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_parse_packet_format_info_get,(unit, port_profile_ndx, parse_key, format_info));
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 10, exit);
    break;
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_parse_packet_format_info_get_print,(unit,port_profile_ndx,parse_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_parse_packet_format_info_get()", port_profile_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_llp_parse module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_llp_parse_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_llp_parse;
}
void
  SOC_PPD_LLP_PARSE_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_TPID_VALUES_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_TPID_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_TPID_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LLP_PARSE_TPID_INDEX_to_string(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_INDEX enum_val
  )
{
  return SOC_PPC_LLP_PARSE_TPID_INDEX_to_string(enum_val);
}

void
  SOC_PPD_LLP_PARSE_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_TPID_VALUES_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_TPID_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_TPID_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_llp_parse_tpid_values_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tpid_vals:")));
  SOC_PPD_LLP_PARSE_TPID_VALUES_print((tpid_vals));

  return;
}
void
  soc_ppd_llp_parse_tpid_values_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_llp_parse_tpid_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile_ndx: %lu\n\r"),tpid_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile_info:")));
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_print((tpid_profile_info));

  return;
}
void
  soc_ppd_llp_parse_tpid_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile_ndx: %lu\n\r"),tpid_profile_ndx));

  return;
}
void
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint32                                tpid_profile_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile_id: %lu\n\r"),tpid_profile_id));

  return;
}
void
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  return;
}
void
  soc_ppd_llp_parse_packet_format_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "parse_key:")));
  SOC_PPD_LLP_PARSE_INFO_print((parse_key));

  LOG_CLI((BSL_META_U(unit,
                      "format_info:")));
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_print((format_info));

  return;
}
void
  soc_ppd_llp_parse_packet_format_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "parse_key:")));
  SOC_PPD_LLP_PARSE_INFO_print((parse_key));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

