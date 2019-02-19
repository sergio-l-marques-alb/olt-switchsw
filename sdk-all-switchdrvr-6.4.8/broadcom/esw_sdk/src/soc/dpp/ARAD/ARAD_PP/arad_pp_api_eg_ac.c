
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_eg_ac.c,v 1.8 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_ac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_ac.h>

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
*     Sets the editing information for packets
 *     associated with AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO                              *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_info);

  res = arad_pp_eg_ac_info_set_verify(
          unit,
          out_ac_ndx,
          ac_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_info_set_unsafe(
          unit,
          out_ac_ndx,
          ac_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_info_set()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the editing information for packets
 *     associated with AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO                              *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_info);

  res = arad_pp_eg_ac_info_get_verify(
          unit,
          out_ac_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_info_get_unsafe(
          unit,
          out_ac_ndx,
          ac_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_info_get()", out_ac_ndx, 0);
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
  arad_pp_eg_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_MP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_MP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_pp_eg_ac_mp_info_set_verify(
          unit,
          out_ac_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_mp_info_set_unsafe(
          unit,
          out_ac_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_mp_info_set()", out_ac_ndx, 0);
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
  arad_pp_eg_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT ARAD_PP_EG_AC_MP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_MP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_pp_eg_ac_mp_info_get_verify(
          unit,
          out_ac_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_mp_info_get_unsafe(
          unit,
          out_ac_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_mp_info_get()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the editing information for packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_port_vsi_info_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO                              *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_eg_ac_port_vsi_info_add_verify(
          unit,
          out_ac_ndx,
          vbp_key,
          ac_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_port_vsi_info_add_unsafe(
          unit,
          out_ac_ndx,
          vbp_key,
          ac_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_add()", out_ac_ndx, 0);
}

/*********************************************************************
*     Removes editing information of packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_port_vsi_info_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);

  res = arad_pp_eg_ac_port_vsi_info_remove_verify(
          unit,
          vbp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_port_vsi_info_remove_unsafe(
          unit,
          vbp_key,
          out_ac
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_remove()", 0, 0);
}

/*********************************************************************
*     Gets the editing information for packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_port_vsi_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac,
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_eg_ac_port_vsi_info_get_verify(
          unit,
          vbp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_port_vsi_info_get_unsafe(
          unit,
          vbp_key,
          out_ac,
          ac_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_get()", 0, 0);
}

/*********************************************************************
*     Sets the editing information for packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_port_cvid_info_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_eg_ac_port_cvid_info_add_verify(
          unit,
          out_ac_ndx,
          cep_key,
          ac_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_port_cvid_info_add_unsafe(
          unit,
          out_ac_ndx,
          cep_key,
          ac_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_add()", out_ac_ndx, 0);
}

/*********************************************************************
*     Removes editing information of packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_port_cvid_info_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);

  res = arad_pp_eg_ac_port_cvid_info_remove_verify(
          unit,
          cep_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_port_cvid_info_remove_unsafe(
          unit,
          cep_key,
          out_ac
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_remove()", 0, 0);
}

/*********************************************************************
*     Gets the editing information for packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_eg_ac_port_cvid_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT ARAD_PP_AC_ID                               *out_ac,
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_eg_ac_port_cvid_info_get_verify(
          unit,
          cep_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_ac_port_cvid_info_get_unsafe(
          unit,
          cep_key,
          out_ac,
          ac_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_get()", 0, 0);
}

void
  ARAD_PP_EG_AC_VBP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VBP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_CEP_PORT_KEY_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_VLAN_EDIT_CEP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_VLAN_EDIT_VLAN_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_VLAN_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VLAN_EDIT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_MP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_MP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

void
  ARAD_PP_EG_AC_VBP_KEY_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VBP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_CEP_PORT_KEY_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_CEP_PORT_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_VLAN_EDIT_CEP_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_VLAN_EDIT_VLAN_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_VLAN_EDIT_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_VLAN_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_VLAN_EDIT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EG_AC_MP_INFO_print(
    SOC_SAND_IN  ARAD_PP_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_AC_MP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

