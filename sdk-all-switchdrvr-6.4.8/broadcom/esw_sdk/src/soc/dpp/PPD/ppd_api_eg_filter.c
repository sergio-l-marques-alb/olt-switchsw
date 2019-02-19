/* $Id: ppd_api_eg_filter.c,v 1.16 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_eg_filter.c
*
* MODULE PREFIX:  soc_ppd_eg
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
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_llp_filter.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_filter.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_filter.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_eg_filter.h>
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
    Ppd_procedure_desc_element_eg_filter[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_FILTER_GET_PROCS_PTR),
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
*     Sets out-port filtering information, including which
 *     filtering to perform on this specific out-port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PORT_INFO                 *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_port_info_set,(unit, core_id, out_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_port_info_set_print,(unit,out_port_ndx,port_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_info_set()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets out-port filtering information, including which
 *     filtering to perform on this specific out-port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_port_info_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_OUT SOC_PPD_EG_FILTER_PORT_INFO                 *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_port_info_get,(unit, core_id, out_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_port_info_get_print,(unit,out_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_info_get()", out_port_ndx, 0);
}


#ifdef BCM_88660_A0
uint32
  soc_ppd_eg_filter_global_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO          *global_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(global_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_filter_global_info_set,(unit, global_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPC_EG_FILTER_GLOBAL_INFO_print(global_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_global_info_set()", 0, 0);
}

uint32
  soc_ppd_eg_filter_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO        *global_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(global_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_filter_global_info_get,(unit, global_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_global_info_get()", 0, 0);
}
#endif /* BCM_88660_A0 */

/*********************************************************************
*     Sets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Set whether outgoing local
 *     port belongs to the VSI. Packets transmitted out through
 *     a port that is not member of the packet's VSI are
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_vsi_port_membership_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_IN  uint8                               is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_vsi_port_membership_set,(unit, core_id, vsid_ndx, out_port_ndx, is_member));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_vsi_port_membership_set_print,(unit,vsid_ndx,out_port_ndx,is_member));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_vsi_port_membership_set()", vsid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Set whether outgoing local
 *     port belongs to the VSI. Packets transmitted out through
 *     a port that is not member of the packet's VSI are
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_vsi_port_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_OUT uint8                               *is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_vsi_port_membership_get,(unit, core_id, vsid_ndx, out_port_ndx, is_member));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_vsi_port_membership_get_print,(unit,vsid_ndx,out_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_vsi_port_membership_get()", vsid_ndx, out_port_ndx);
}

/*********************************************************************
*     Gets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Get all member outgoing local
 *     ports belongs to the VSI. 
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_vsi_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_VSI_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_vsi_membership_get,(unit, core_id, vsid_ndx, ports));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_vsi_membership_get_print,(unit,vsid_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_vsi_membership_get()", vsid_ndx, 0);
}

/*********************************************************************
*     Sets egress CVID membership (relevant for CEP ports).
 *     Sets whether outgoing local port belongs to the CVID.
 *     Packets transmitted out through a port that is not
 *     member of the packet's CVID are filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_cvid_port_membership_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         cvid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_IN  uint8                               is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_cvid_port_membership_set,(unit, cvid_ndx, out_port_ndx, is_member));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_cvid_port_membership_set_print,(unit,cvid_ndx,out_port_ndx,is_member));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_cvid_port_membership_set()", cvid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets egress CVID membership (relevant for CEP ports).
 *     Sets whether outgoing local port belongs to the CVID.
 *     Packets transmitted out through a port that is not
 *     member of the packet's CVID are filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_cvid_port_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         cvid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_OUT uint8                               *is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_cvid_port_membership_get,(unit, cvid_ndx, out_port_ndx, is_member));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_cvid_port_membership_get_print,(unit,cvid_ndx,out_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_cvid_port_membership_get()", cvid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets acceptable frame type on outgoing port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_port_acceptable_frames_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                               llvp_port_profile,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO               *eg_prsr_out_key,
    SOC_SAND_IN  uint8                                accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_port_acceptable_frames_set,(unit, eg_acceptable_frames_port_profile, llvp_port_profile, eg_prsr_out_key, accept));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_port_acceptable_frames_set_print,(unit,eg_acceptable_frames_port_profile,llvp_port_profile,eg_prsr_out_key,accept));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_acceptable_frames_set()", eg_acceptable_frames_port_profile, 0);
}

/*********************************************************************
*     Sets acceptable frame type on outgoing port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_port_acceptable_frames_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                out_port_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                               *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_port_acceptable_frames_get,(unit, out_port_ndx, eg_prsr_out_key, accept));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_port_acceptable_frames_get_print,(unit,out_port_ndx,eg_prsr_out_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_acceptable_frames_get()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_pep_acceptable_frames_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_IN  uint8                               accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_pep_acceptable_frames_set,(unit, pep_key, vlan_format_ndx, accept));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_pep_acceptable_frames_set_print,(unit,pep_key,vlan_format_ndx,accept));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pep_acceptable_frames_set()", 0, 0);
}

/*********************************************************************
*     Sets acceptable frame type for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_pep_acceptable_frames_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_OUT uint8                               *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_pep_acceptable_frames_get,(unit, pep_key, vlan_format_ndx, accept));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_pep_acceptable_frames_get_print,(unit,pep_key,vlan_format_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pep_acceptable_frames_get()", 0, 0);
}

/*********************************************************************
*     Set the Private VLAN (PVLAN) port type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_pvlan_port_type_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE           pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_pvlan_port_type_set,(unit, src_sys_port_ndx, pvlan_port_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_pvlan_port_type_set_print,(unit,src_sys_port_ndx,pvlan_port_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pvlan_port_type_set()", 0, 0);
}

/*********************************************************************
*     Set the Private VLAN (PVLAN) port type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_pvlan_port_type_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE           *pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(pvlan_port_type);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_pvlan_port_type_get,(unit, src_sys_port_ndx, pvlan_port_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_pvlan_port_type_get_print,(unit,src_sys_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pvlan_port_type_get()", 0, 0);
}

/*********************************************************************
*     Sets the orientation of out-AC, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION           orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_split_horizon_out_ac_orientation_set,(unit, out_ac_ndx, orientation));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_split_horizon_out_ac_orientation_set_print,(unit,out_ac_ndx,orientation));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_ac_orientation_set()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-AC, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION           *orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_split_horizon_out_ac_orientation_get,(unit, out_ac_ndx, orientation));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_split_horizon_out_ac_orientation_get_print,(unit,out_ac_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_ac_orientation_get()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION           orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_split_horizon_out_lif_orientation_set,(unit, lif_eep_ndx, orientation));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_split_horizon_out_lif_orientation_set_print,(unit,lif_eep_ndx,orientation));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_lif_orientation_set()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_filter_split_horizon_out_lif_orientation_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION           *orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_filter_split_horizon_out_lif_orientation_get,(unit, lif_eep_ndx, orientation)); 

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_filter_split_horizon_out_lif_orientation_get_print,(unit,lif_eep_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_lif_orientation_get()", lif_eep_ndx, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_eg_filter module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_eg_filter_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_eg_filter;
}
void
  SOC_PPD_EG_FILTER_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_FILTER_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE enum_val
  )
{
  return SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_EG_FILTER_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PORT_ENABLE enum_val
  )
{
  return SOC_PPC_EG_FILTER_PORT_ENABLE_to_string(enum_val);
}

void
  SOC_PPD_EG_FILTER_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_FILTER_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_eg_filter_port_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PORT_INFO                 *port_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "port_info:")));
  SOC_PPD_EG_FILTER_PORT_INFO_print((port_info));

  return;
}
void
  soc_ppd_eg_filter_port_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  return;
}
void
  soc_ppd_eg_filter_vsi_port_membership_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_IN  uint8                               is_member
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsid_ndx: %lu\n\r"),vsid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "is_member: %u\n\r"),is_member));

  return;
}
void
  soc_ppd_eg_filter_vsi_port_membership_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsid_ndx: %lu\n\r"),vsid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  return;
}

void
  soc_ppd_eg_filter_vsi_membership_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx    
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsid_ndx: %lu\n\r"),vsid_ndx));

  return;
}
void
  soc_ppd_eg_filter_cvid_port_membership_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         cvid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx,
    SOC_SAND_IN  uint8                               is_member
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cvid_ndx: %lu\n\r"),cvid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "is_member: %u\n\r"),is_member));

  return;
}
void
  soc_ppd_eg_filter_cvid_port_membership_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         cvid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                out_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cvid_ndx: %lu\n\r"),cvid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  return;
}
void
  soc_ppd_eg_filter_port_acceptable_frames_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               eg_acceptable_frames_port_profile,
    SOC_SAND_IN  unit32                               llvp_port_profile,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO               *eg_prsr_out_key,
    SOC_SAND_IN  uint8                                accept
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "eg_acceptable_frames_port_profile: %lu\n\r"), eg_acceptable_frames_port_profile));
  LOG_CLI((BSL_META_U(unit,
                      "llvp_port_profile: %lu\n\r"), llvp_port_profile));

  LOG_CLI((BSL_META_U(unit,
                      "eg_prsr_out_key:")));
  SOC_PPD_LLP_PARSE_INFO_print((eg_prsr_out_key));

  LOG_CLI((BSL_META_U(unit,
                      "accept: %u\n\r"),accept));

  return;
}
void
  soc_ppd_eg_filter_port_acceptable_frames_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                out_port_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *eg_prsr_out_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_port_ndx: %lu\n\r"),out_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "eg_prsr_out_key:")));
  SOC_PPD_LLP_PARSE_INFO_print((eg_prsr_out_key));

  return;
}
void
  soc_ppd_eg_filter_pep_acceptable_frames_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_IN  uint8                               accept
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pep_key:")));
  SOC_PPD_PEP_KEY_print((pep_key));

  LOG_CLI((BSL_META_U(unit,
                      "vlan_format_ndx %s "), soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(vlan_format_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "accept: %u\n\r"),accept));

  return;
}
void
  soc_ppd_eg_filter_pep_acceptable_frames_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pep_key:")));
  SOC_PPD_PEP_KEY_print((pep_key));

  LOG_CLI((BSL_META_U(unit,
                      "vlan_format_ndx %s "), soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(vlan_format_ndx)));

  return;
}
void
  soc_ppd_eg_filter_pvlan_port_type_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE           pvlan_port_type
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port_ndx:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print((src_sys_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "pvlan_port_type %s "), SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_to_string(pvlan_port_type)));

  return;
}
void
  soc_ppd_eg_filter_pvlan_port_type_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port_ndx:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print((src_sys_port_ndx));

  return;
}
void
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION           orientation
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "orientation %s "), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(orientation)));

  return;
}
void
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

