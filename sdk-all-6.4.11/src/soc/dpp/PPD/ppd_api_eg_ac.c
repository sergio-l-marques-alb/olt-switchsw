/* $Id: ppd_api_eg_ac.c,v 1.13 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_eg_ac.c
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
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_ac.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_ac.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_eg_ac.h>
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
    Ppd_procedure_desc_element_eg_ac[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_MP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_MP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_INFO_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_INFO_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_INFO_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_INFO_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_CVID_MAP_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_PORT_VSI_MAP_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_AC_GET_PROCS_PTR),
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
*     Sets the editing information for packets
 *     associated with AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_info_set,(unit, out_ac_ndx, ac_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_info_set_print,(unit,out_ac_ndx,ac_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_info_set()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the editing information for packets
 *     associated with AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_PPD_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_info_get,(unit, out_ac_ndx, ac_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_info_get_print,(unit,out_ac_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_info_get()", out_ac_ndx, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x VSI) and
 *     MD-level, and to determine the action to perform. If the
 *     MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                          out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_MP_INFO                  *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_MP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_mp_info_set,(unit, out_ac_ndx, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_mp_info_set_print,(unit,out_ac_ndx,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_mp_info_set()", out_ac_ndx, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x VSI) and
 *     MD-level, and to determine the action to perform. If the
 *     MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                          out_ac_ndx,
    SOC_SAND_OUT SOC_PPD_EG_AC_MP_INFO                  *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_MP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_mp_info_get,(unit, out_ac_ndx, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_mp_info_get_print,(unit,out_ac_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_mp_info_get()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the editing information for packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_vsi_info_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_VSI_INFO_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_port_vsi_info_add,(unit, out_ac_ndx, vbp_key, ac_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_vsi_info_add_print,(unit,out_ac_ndx,vbp_key,ac_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_vsi_info_add()", out_ac_ndx, 0);
}

/*********************************************************************
*     Removes editing information of packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_vsi_info_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPD_AC_ID                               *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_port_vsi_info_remove,(unit, vbp_key, out_ac));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_vsi_info_remove_print,(unit,vbp_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_vsi_info_remove()", 0, 0);
}

/*********************************************************************
*     Gets the editing information for packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPD_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPD_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_VSI_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_port_vsi_info_get,(unit, vbp_key, out_ac, ac_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_vsi_info_get_print,(unit,vbp_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_vsi_info_get()", 0, 0);
}

/*********************************************************************
*     Sets the editing information for packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_cvid_info_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_CVID_INFO_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_port_cvid_info_add,(unit, out_ac_ndx, cep_key, ac_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_cvid_info_add_print,(unit,out_ac_ndx,cep_key,ac_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_cvid_info_add()", out_ac_ndx, 0);
}

/*********************************************************************
*     Removes editing information of packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_cvid_info_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPD_AC_ID                               *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_port_cvid_info_remove,(unit, cep_key, out_ac));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_cvid_info_remove_print,(unit,cep_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_cvid_info_remove()", 0, 0);
}

/*********************************************************************
*     Gets the editing information for packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_cvid_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPD_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPD_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_CVID_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_ac_port_cvid_info_get,(unit, cep_key, out_ac, ac_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_cvid_info_get_print,(unit,cep_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_cvid_info_get()", 0, 0);
}

/*********************************************************************
*     Maps CEP key (port X CVID) to out-AC-ID
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_cvid_map(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT uint32                                *out_ac_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_CVID_MAP);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac_id);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_T20E:
      SOC_T20E_DEVICE_CALL(eg_ac_port_cvid_map,(unit, cep_key, out_ac_id));
      break;
    case SOC_SAND_DEV_PB:
      /*
       *	Not supported for Soc_petra-B.
       */
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 12, exit);
      break;
  }

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_cvid_map_print,(unit,cep_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_cvid_map()", 0, 0);
}

/*********************************************************************
*     Maps CEP key (port X CVID) to out-AC-ID
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_ac_port_vsi_map(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPD_AC_ID                               *out_ac_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_AC_PORT_VSI_MAP);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac_id);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_T20E:
      SOC_T20E_DEVICE_CALL(eg_ac_port_vsi_map,(unit, vbp_key, out_ac_id));
      break;
    case SOC_SAND_DEV_PB:
      /*
       *	Not supported for Soc_petra-B.
       */
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 12, exit);
      break;
  }

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_ac_port_vsi_map_print,(unit,vbp_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_ac_port_vsi_map()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_eg_ac module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_eg_ac_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_eg_ac;
}
void
  SOC_PPD_EG_AC_VBP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VBP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_CEP_PORT_KEY_clear(
    SOC_SAND_OUT SOC_PPD_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_VLAN_EDIT_CEP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_VLAN_EDIT_VLAN_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_AC_VLAN_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VLAN_EDIT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_MP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

void
  SOC_PPD_EG_AC_VBP_KEY_print(
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VBP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_CEP_PORT_KEY_print(
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_CEP_PORT_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_VLAN_EDIT_CEP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_VLAN_EDIT_VLAN_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_VLAN_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_AC_VLAN_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VLAN_EDIT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_MP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_eg_ac_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO                          *ac_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ac_info:")));
  SOC_PPD_EG_AC_INFO_print((ac_info));

  return;
}
void
  soc_ppd_eg_ac_info_get_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                          out_ac_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  return;
}
void
  soc_ppd_eg_ac_mp_info_set_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                          out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_MP_INFO                  *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_EG_AC_MP_INFO_print((info));

  return;
}
void
  soc_ppd_eg_ac_mp_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  return;
}
void
  soc_ppd_eg_ac_port_vsi_info_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO                          *ac_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vbp_key:")));
  SOC_PPD_EG_AC_VBP_KEY_print((vbp_key));

  LOG_CLI((BSL_META_U(unit,
                      "ac_info:")));
  SOC_PPD_EG_AC_INFO_print((ac_info));

  return;
}
void
  soc_ppd_eg_ac_port_vsi_info_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vbp_key:")));
  SOC_PPD_EG_AC_VBP_KEY_print((vbp_key));

  return;
}
void
  soc_ppd_eg_ac_port_vsi_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vbp_key:")));
  SOC_PPD_EG_AC_VBP_KEY_print((vbp_key));

  return;
}
void
  soc_ppd_eg_ac_port_cvid_info_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPD_EG_AC_INFO                          *ac_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_ndx: %lu\n\r"),out_ac_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "cep_key:")));
  SOC_PPD_EG_AC_CEP_PORT_KEY_print((cep_key));

  LOG_CLI((BSL_META_U(unit,
                      "ac_info:")));
  SOC_PPD_EG_AC_INFO_print((ac_info));

  return;
}
void
  soc_ppd_eg_ac_port_cvid_info_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cep_key:")));
  SOC_PPD_EG_AC_CEP_PORT_KEY_print((cep_key));

  return;
}
void
  soc_ppd_eg_ac_port_cvid_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cep_key:")));
  SOC_PPD_EG_AC_CEP_PORT_KEY_print((cep_key));

  return;
}
void
  soc_ppd_eg_ac_port_cvid_map_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cep_key:")));
  SOC_PPD_EG_AC_CEP_PORT_KEY_print((cep_key));

  return;
}
void
  soc_ppd_eg_ac_port_vsi_map_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_AC_VBP_KEY                       *vbp_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vbp_key:")));
  SOC_PPD_EG_AC_VBP_KEY_print((vbp_key));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

