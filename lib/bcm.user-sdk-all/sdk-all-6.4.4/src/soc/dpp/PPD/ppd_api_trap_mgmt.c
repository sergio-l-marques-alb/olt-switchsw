/* $Id: ppd_api_trap_mgmt.c,v 1.18 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_trap_mgmt.c
*
* MODULE PREFIX:  soc_ppd_trap
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

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_port.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
  #include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_trap_mgmt.h>
  #include <soc/dpp/T20E/t20e_trap_mgmt.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_trap_mgmt.h>
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PPD_API_TRAP_MGMT_FTMH_SIZE_BYTE               (6)
#define SOC_PPD_API_TRAP_MGMT_FTMH_DEST_EXT_SIZE_BYTE      (2)
#define SOC_PPD_API_TRAP_MGMT_FTMH_LB_EXT_SIZE_BYTE        (2)
#define SOC_PPD_API_TRAP_MGMT_PPH_SIZE_BYTE                (8)
#define SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_SIZE_BYTE      (5)
#define SOC_PPD_API_TRAP_MGMT_PPH_EEI_EXT_SIZE_BYTE        (3)
#define SOC_PPD_API_TRAP_MGMT_PPH_EEP_EXT_SIZE_BYTE        (2)

#define SOC_PPD_API_TRAP_MGMT_FTMH_MSB \
  (SOC_PPD_API_TRAP_MGMT_FTMH_SIZE_BYTE * SOC_SAND_NOF_BITS_IN_BYTE - 1)
#define SOC_PPD_API_TRAP_MGMT_PPH_MSB \
  (SOC_PPD_API_TRAP_MGMT_PPH_SIZE_BYTE * SOC_SAND_NOF_BITS_IN_BYTE - 1)
#define SOC_PPD_API_TRAP_MGMT_LE_MSB \
  (SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_SIZE_BYTE * SOC_SAND_NOF_BITS_IN_BYTE - 1)

#define SOC_PPD_API_TRAP_MGMT_FTMH_PKT_SIZE_MSB        (SOC_PPD_API_TRAP_MGMT_FTMH_MSB - 45)
#define SOC_PPD_API_TRAP_MGMT_FTMH_PKT_SIZE_NOF_BITS   (14)
#define SOC_PPD_API_TRAP_MGMT_FTMH_SRC_SYS_PORT_MSB        (SOC_PPD_API_TRAP_MGMT_FTMH_MSB - 28)
#define SOC_PPD_API_TRAP_MGMT_FTMH_SRC_SYS_PORT_NOF_BITS   (13)
#define SOC_PPD_API_TRAP_MGMT_FTMH_ACTION_TYPE_MSB        (SOC_PPD_API_TRAP_MGMT_FTMH_MSB - 5)
#define SOC_PPD_API_TRAP_MGMT_FTMH_ACTION_TYPE_NOF_BITS   (2)
#define SOC_PPD_API_TRAP_MGMT_FTMH_EXT_MSB              (SOC_PPD_API_TRAP_MGMT_FTMH_MSB - 3)
#define SOC_PPD_API_TRAP_MGMT_FTMH_EXT_NOF_BITS         (1)
#define SOC_PPD_API_TRAP_MGMT_FTMH_SYS_MC_MSB              (SOC_PPD_API_TRAP_MGMT_FTMH_MSB - 0)
#define SOC_PPD_API_TRAP_MGMT_FTMH_SYS_MC_NOF_BITS         (1)

#define SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_MSB            (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 7)
#define SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_NOF_BITS       (8)
#define SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_QUAL_MSB       (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 21)
#define SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_QUAL_NOF_BITS  (14)
#define SOC_PPD_API_TRAP_MGMT_PPH_VSI_MSB       (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 43)
#define SOC_PPD_API_TRAP_MGMT_PPH_VSI_NOF_BITS  (16)
#define SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_MSB      (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 47)
#define SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_NOF_BITS (1)
#define SOC_PPD_API_TRAP_MGMT_PPH_EEI_EXT_PRSNT_MSB        (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 62)
#define SOC_PPD_API_TRAP_MGMT_PPH_EEI_EXT_PRSNT_NOF_BITS   (1)
#define SOC_PPD_API_TRAP_MGMT_PPH_SC_MSB        (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 25)
#define SOC_PPD_API_TRAP_MGMT_PPH_SC_NOF_BITS   (2)
#define SOC_PPD_API_TRAP_MGMT_PPH_FWD_MSB        (SOC_PPD_API_TRAP_MGMT_PPH_MSB - 61)
#define SOC_PPD_API_TRAP_MGMT_PPH_FWD_NOF_BITS   (4)
#define SOC_PPD_API_TRAP_MGMT_LE_AC_MSB     (SOC_PPD_API_TRAP_MGMT_LE_MSB - 39)
#define SOC_PPD_API_TRAP_MGMT_LE_AC_NOF_BITS     (24)

#define SOC_PPD_API_TRAP_MGMT_HDR_MAX_SIZE (40)

#define SOC_PPD_API_TRAP_MGMT_IS_TRAP_VAL (7)

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
    Ppd_procedure_desc_element_trap_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_FRWRD_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_FRWRD_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_FRWRD_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_FRWRD_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_SNOOP_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_SNOOP_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_SNOOP_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_SNOOP_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_TO_EG_ACTION_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_TO_EG_ACTION_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_TO_EG_ACTION_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_TO_EG_ACTION_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_EG_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_EG_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_EG_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_EG_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_MACT_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_MACT_EVENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_MACT_EVENT_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_MACT_EVENT_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_PACKET_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_PACKET_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_TRAP_MGMT_GET_PROCS_PTR),
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
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_frwrd_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_FRWRD_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_frwrd_profile_info_set,(unit, trap_code_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_frwrd_profile_info_set_print,(unit,trap_code_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_frwrd_profile_info_set()", 0, 0);
}

/*********************************************************************
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_frwrd_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_FRWRD_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_frwrd_profile_info_get,(unit, trap_code_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_frwrd_profile_info_get_print,(unit,trap_code_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_frwrd_profile_info_get()", 0, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_snoop_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_SNOOP_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_snoop_profile_info_set,(unit, trap_code_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_snoop_profile_info_set_print,(unit,trap_code_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_snoop_profile_info_set()", 0, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_snoop_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_SNOOP_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_snoop_profile_info_get,(unit, trap_code_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_snoop_profile_info_get_print,(unit,trap_code_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_snoop_profile_info_get()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_to_eg_action_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_TO_EG_ACTION_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_to_eg_action_map_set,(unit, trap_type_bitmap_ndx, eg_action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_to_eg_action_map_set_print,(unit,trap_type_bitmap_ndx,eg_action_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_to_eg_action_map_set()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_to_eg_action_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                *eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_TO_EG_ACTION_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_action_profile);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_to_eg_action_map_get,(unit, trap_type_bitmap_ndx, eg_action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_to_eg_action_map_get_print,(unit,trap_type_bitmap_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_to_eg_action_map_get()", 0, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_eg_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_EG_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_eg_profile_info_set,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_eg_profile_info_set_print,(unit,profile_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_eg_profile_info_set()", profile_ndx, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_ppd_trap_eg_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_EG_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_eg_profile_info_get,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_eg_profile_info_get_print,(unit,profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_eg_profile_info_get()", profile_ndx, 0);
}

/*********************************************************************
*     Read MACT event from the events FIFO into buffer.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_mact_event_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint32                                buff[SOC_PPD_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                *buff_len
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_MACT_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff_len);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_mact_event_get,(unit, buff, buff_len));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_mact_event_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_mact_event_get()", 0, 0);
}

/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_mact_event_parse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                buff[SOC_PPD_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                buff_len,
    SOC_SAND_OUT SOC_PPD_TRAP_MACT_EVENT_INFO                *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_MACT_EVENT_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_mact_event_parse,(unit, buff, buff_len, mact_event));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_mact_event_parse_print,(unit,buff,buff_len));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_mact_event_parse()", 0, 0);
}

/*********************************************************************
*     Parse a packet received as buffer, identifying the
 *     reason of trapping (if any), the source system port, and
 *     pointer to the packet payload and additional
 *     information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_trap_packet_parse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                 *buff,
    SOC_SAND_IN  uint32                                buff_len,
    SOC_SAND_OUT SOC_PPD_TRAP_PACKET_INFO                    *packet_info
  )
{
  uint32
    res = SOC_SAND_OK,
    header_ptr;
  SOC_PPD_TRAP_MGMT_PACKET_PARSE_INFO
    packet_parse_info;
  uint8
    hdr_buff[SOC_PPD_API_TRAP_MGMT_HDR_MAX_SIZE];
  uint32
    fld_val,
    ac_present=FALSE,
    fld_type,
    indx,
    hdr_size;
  SOC_PPD_TRAP_CODE
    trap_code;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_TRAP_PACKET_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff);
  SOC_SAND_CHECK_NULL_INPUT(packet_info);

  /* Taking device semaphore, since we call unsafe functions from here */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /* Using implicit switch instead of SOC_PPD_ARAD_TMP_DEVICE_CALL, since in case of error
     we'd like to jump to 'exit_semaphore' and not 'exit' */
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_PB:
	  SOC_PB_PP_DEVICE_CALL(trap_packet_parse_info_get_unsafe,(unit, &packet_parse_info));
      break;
  case SOC_SAND_DEV_ARAD:
      ARAD_PP_DEVICE_CALL(trap_packet_parse_info_get_unsafe,(unit, &packet_parse_info));
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit_semaphore);
      break;
  }

  SOC_PPD_TRAP_PACKET_INFO_clear(packet_info);
 
  if (buff_len == 0)
  {
    packet_info->size_in_bytes = 0;
    goto exit_semaphore;
  }

  /* if ARAD go to ARAD specific code, PPD code is only for Soc_petra-B due to many changes, and code actually is not common */
  if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD)
  {
/*
 * COVERITY
 *
 * The variable packet_parse_info is assigned because if device id is ARAD we will go into
 * ARAD section in the previous switch.
 */
/* coverity[uninit_use_in_call] */
      ARAD_PP_DEVICE_CALL(trap_packet_parse_unsafe,(unit, buff, buff_len,&packet_parse_info,packet_info));
      goto exit_semaphore;
  }

  /* header to examine */
  hdr_size = SOC_SAND_MIN(SOC_PPD_API_TRAP_MGMT_HDR_MAX_SIZE, buff_len);
  /* align header byte order, only for examined header */
  if (packet_parse_info.recv_mode == SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
  {
    for (indx = 0; indx < hdr_size; ++indx)
    {
      hdr_buff[indx] = buff[buff_len - indx - 1];
    }
  }
  else
  {
    sal_memcpy(hdr_buff,buff,hdr_size);
  }

  packet_info->frwrd_type = SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_NORMAL;

  header_ptr = 0;
  /*
   * FTMH
   */
  if (header_ptr + SOC_PPD_API_TRAP_MGMT_FTMH_SIZE_BYTE > buff_len)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 5, exit_semaphore);
  }

   /* Advance header ptr to end of FTMH. Note that during this functions, header_ptr
     always points to the beginning of the next header (FTMH/PPH), since it used as
     an offset in hdr_buff, when reading fields from the header, and packet_info->ll_header_ptr
     is used to add the different extensions, if exist */
  packet_info->ntwrk_header_ptr = SOC_PPD_API_TRAP_MGMT_FTMH_SIZE_BYTE;

  /* packet size */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          SOC_PPD_API_TRAP_MGMT_FTMH_PKT_SIZE_MSB,
          SOC_PPD_API_TRAP_MGMT_FTMH_PKT_SIZE_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit_semaphore);
  packet_info->size_in_bytes = fld_val;
  /* action type */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          SOC_PPD_API_TRAP_MGMT_FTMH_ACTION_TYPE_MSB,
          SOC_PPD_API_TRAP_MGMT_FTMH_ACTION_TYPE_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit_semaphore);

  switch(fld_val)
  {
  case 0:
      packet_info->frwrd_type = SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_NORMAL;
  break;
  case 1:
      packet_info->frwrd_type = SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP;
  break;
  case 2:
      packet_info->frwrd_type = SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_IN_MIRROR;
  break;
  }
  
  /* Source system port */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          SOC_PPD_API_TRAP_MGMT_FTMH_SRC_SYS_PORT_MSB,
          SOC_PPD_API_TRAP_MGMT_FTMH_SRC_SYS_PORT_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit_semaphore);

  packet_info->src_sys_port = fld_val;

  /* Advance header ptr to end of ftmh destination extension, if exists */
  if (packet_parse_info.dest_ext_mode == SOC_PPD_TRAP_MGMT_FTMH_DEST_EXT_MODE_ALWAYS)
  {
    packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_FTMH_DEST_EXT_SIZE_BYTE;
  }
  else if (packet_parse_info.dest_ext_mode == SOC_PPD_TRAP_MGMT_FTMH_DEST_EXT_MODE_MC)
  {
    /* Check if system multicast flag is high*/
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      &hdr_buff[header_ptr],
      SOC_PPD_API_TRAP_MGMT_FTMH_SYS_MC_MSB,
      SOC_PPD_API_TRAP_MGMT_FTMH_SYS_MC_NOF_BITS,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

    if (fld_val)
    {
      packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_FTMH_DEST_EXT_SIZE_BYTE;
    }
  }


  /* Advance header ptr to end of ftmh load balancing extension, if exists */
  if (packet_parse_info.lb_ext_en)
  {
    packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_FTMH_LB_EXT_SIZE_BYTE;
  }

  /* ftmh ext */
  fld_val = 0;
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    SOC_PPD_API_TRAP_MGMT_FTMH_EXT_MSB,
    SOC_PPD_API_TRAP_MGMT_FTMH_EXT_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  if (fld_val == 0)
  {
    goto exit_semaphore;
  }

  /* Both ptr's now points to the PPH */
  header_ptr = packet_info->ntwrk_header_ptr;

  /*
   * PPH
   */
  /* is trapped packet */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          SOC_PPD_API_TRAP_MGMT_PPH_FWD_MSB,
          SOC_PPD_API_TRAP_MGMT_PPH_FWD_NOF_BITS,
          &fld_val
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit_semaphore);
  if (packet_info->frwrd_type != SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP)
  {
    packet_info->is_trapped = (uint8)(fld_val == SOC_PPD_API_TRAP_MGMT_IS_TRAP_VAL);
    if(packet_info->is_trapped)
    {
      packet_info->frwrd_type = SOC_PPD_TRAP_MGMT_PKT_FRWRD_TYPE_TRAP;
    }
  }
  
  if (packet_info->is_trapped)
  {
    /* CPU trap code qualifier */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_QUAL_MSB,
            SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_QUAL_NOF_BITS,
            &fld_val
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit_semaphore);

    packet_info->cpu_trap_qualifier = fld_val;

    /* CPU trap code */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      &hdr_buff[header_ptr],
      SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_MSB,
      SOC_PPD_API_TRAP_MGMT_PPH_TRAP_CODE_NOF_BITS,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_semaphore);

    SOC_PPD_ARAD_TMP_DEVICE_CALL(trap_cpu_trap_code_from_internal_unsafe,(unit, fld_val, &trap_code));

    packet_info->cpu_trap_code = trap_code;
  }
    /* vsi/RIF  */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          SOC_PPD_API_TRAP_MGMT_PPH_VSI_MSB,
          SOC_PPD_API_TRAP_MGMT_PPH_VSI_NOF_BITS,
          &fld_val
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit_semaphore);

  packet_info->rif = fld_val;

  /* snoop extension */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    SOC_PPD_API_TRAP_MGMT_PPH_SC_MSB,
    SOC_PPD_API_TRAP_MGMT_PPH_SC_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit_semaphore);

  packet_info->snoop_cmnd = fld_val * 2;

  /* Learn extension */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_MSB,
    SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit_semaphore);
  ac_present = fld_val;

  /* EEI extension */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    SOC_PPD_API_TRAP_MGMT_PPH_EEI_EXT_PRSNT_MSB,
    SOC_PPD_API_TRAP_MGMT_PPH_EEI_EXT_PRSNT_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit_semaphore);

  /* Advance header ptr to end of pph eei extension, if exists */
  if (fld_val)
  {
    packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_PPH_EEI_EXT_SIZE_BYTE;
  }
  
  /* Advance header ptr to end of PPH */
  packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_PPH_SIZE_BYTE;

  
  /* Advance header ptr to end of pph EEP extension, if exists */
  if (packet_parse_info.pph_eep_ext)
  {
    packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_PPH_EEP_EXT_SIZE_BYTE;
  }

    /* Advance header ptr to end of pph learn extension, if exists */
  if (ac_present)
  {
    header_ptr = packet_info->ntwrk_header_ptr;
    /* get AC */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            SOC_PPD_API_TRAP_MGMT_LE_AC_MSB,
            SOC_PPD_API_TRAP_MGMT_LE_AC_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit_semaphore);

    fld_type = SOC_SAND_GET_BITS_RANGE(fld_val,22,14);
    if (fld_type == 0x32)
    {
      packet_info->in_ac = SOC_SAND_GET_BITS_RANGE(fld_val,13,0);
    }

    packet_info->ntwrk_header_ptr += SOC_PPD_API_TRAP_MGMT_PPH_LEARN_EXT_SIZE_BYTE;

  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_trap_packet_parse_print,(unit,hdr_buff,buff_len));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_packet_parse()", buff_len, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_trap_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_trap_mgmt_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_trap_mgmt;
}
void
  SOC_PPD_TRAP_ACTION_PROFILE_DEST_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_COUNT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_METER_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_POLICE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_COS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_MACT_EVENT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_TRAP_SNOOP_ACTION_SIZE_to_string(
    SOC_SAND_IN  SOC_PPD_TRAP_SNOOP_ACTION_SIZE enum_val
  )
{
  return SOC_PPC_TRAP_SNOOP_ACTION_SIZE_to_string(enum_val);
}

const char*
  SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_to_string(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE enum_val
  )
{
  return SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_to_string(enum_val);
}

const char*
  SOC_PPD_TRAP_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_TRAP_MACT_EVENT_TYPE enum_val
  )
{
  return SOC_PPC_TRAP_MACT_EVENT_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_TRAP_CODE_to_string(
    SOC_SAND_IN  SOC_PPD_TRAP_CODE enum_val
  )
{
  return SOC_PPC_TRAP_CODE_to_string(enum_val);
}

const char*
  SOC_PPD_TRAP_EG_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_TRAP_EG_TYPE enum_val
  )
{
  return SOC_PPC_TRAP_EG_TYPE_to_string(enum_val);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_DEST_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_COUNT_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_METER_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_POLICE_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_COS_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_ACTION_PROFILE_PROCESS_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_MACT_EVENT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TRAP_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_PACKET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_trap_frwrd_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_code_ndx %s "), SOC_PPD_TRAP_CODE_to_string(trap_code_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_TRAP_FRWRD_ACTION_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_trap_frwrd_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_code_ndx %s "), SOC_PPD_TRAP_CODE_to_string(trap_code_ndx)));

  return;
}
void
  soc_ppd_trap_snoop_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_code_ndx %s "), SOC_PPD_TRAP_CODE_to_string(trap_code_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_trap_snoop_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TRAP_CODE                           trap_code_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_code_ndx %s "), SOC_PPD_TRAP_CODE_to_string(trap_code_ndx)));

  return;
}
void
  soc_ppd_trap_to_eg_action_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                eg_action_profile
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_type_bitmap_ndx %d "), trap_type_bitmap_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "eg_action_profile: %lu\n\r"),eg_action_profile));

  return;
}
void
  soc_ppd_trap_to_eg_action_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                trap_type_bitmap_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_type_ndx %d "), trap_type_bitmap_ndx));

  return;
}
void
  soc_ppd_trap_eg_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_trap_eg_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  return;
}
void
  soc_ppd_trap_mact_event_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_trap_mact_event_parse_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                buff[SOC_PPD_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                buff_len
  )
{

  uint32
    ind;

  for (ind = 0; ind < SOC_PPD_TRAP_EVENT_BUFF_MAX_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "buff[%u]: %lu\n\r"),ind,buff[ind]));
  }

  LOG_CLI((BSL_META_U(unit,
                      "buff_len: %lu\n\r"),buff_len));

  return;
}
void
  soc_ppd_trap_packet_parse_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                 *buff,
    SOC_SAND_IN  uint32                                buff_len
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "buff: %u\n\r"), *buff));

  LOG_CLI((BSL_META_U(unit,
                      "buff_len: %lu\n\r"),buff_len));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

