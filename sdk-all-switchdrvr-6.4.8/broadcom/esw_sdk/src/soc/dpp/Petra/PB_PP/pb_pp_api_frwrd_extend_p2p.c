/* $Id: pb_pp_api_frwrd_extend_p2p.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_frwrd_extend_p2p.c
*
* MODULE PREFIX:  soc_pb_pp
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_extend_p2p.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_extend_p2p.h>

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
*     Setting global information of the Extended P2P services,
 *     including enabling this mode and the profiles to assign
 *     for these services
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_glbl_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_frwrd_extend_p2p_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Setting global information of the Extended P2P services,
 *     including enabling this mode and the profiles to assign
 *     for these services
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_glbl_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_frwrd_extend_p2p_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Add P2P service for packets entering from the access
 *     side with AC key = (vlan-domain x VID x VID). Thus,
 *     these packets will have the same processing and
 *     forwarding regardless of the MAC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_ac_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO             *ac_p2p_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_AC_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

 

  res = soc_pb_pp_frwrd_extend_p2p_ac_add_verify(
          unit,
          ac_key,
          ac_p2p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_ac_add_unsafe(
          unit,
          ac_key,
          ac_p2p_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_ac_add()", 0, 0);
}

/*********************************************************************
*     Gets the P2P service for packets entering from the
 *     access side with AC key = (vlan-domain x VID x VID).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_ac_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                      *ac_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *ac_p2p_info,
    SOC_SAND_OUT uint8                                *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_AC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_frwrd_extend_p2p_ac_get_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_ac_get_unsafe(
          unit,
          ac_key,
          ac_p2p_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_ac_get()", 0, 0);
}

/*********************************************************************
*     Remove P2P service for packets entering from the access
 *     side with AC key = (vlan-domain x VID x VID).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_ac_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                      *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_AC_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);

  res = soc_pb_pp_frwrd_extend_p2p_ac_remove_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_ac_remove_unsafe(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_ac_remove()", 0, 0);
}

/*********************************************************************
*     Add P2P service for packets entering with PWE key =
 *     (in-RIF x VC label). Thus, these packets will have the
 *     same processing and forwarding regardless of the MAC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_pwe_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_PWE_KEY                    *pwe_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO             *pwe_p2p_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_PWE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_key);
  SOC_SAND_CHECK_NULL_INPUT(pwe_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_extend_p2p_pwe_add_verify(
          unit,
          pwe_key,
          pwe_p2p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_pwe_add_unsafe(
          unit,
          pwe_key,
          pwe_p2p_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_pwe_add()", 0, 0);
}

/*********************************************************************
*     Gets the P2P service for packets entering with PWE key =
 *     (in-RIF x VC label).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_pwe_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *pwe_p2p_info,
    SOC_SAND_OUT uint8                                *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_PWE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_key);
  SOC_SAND_CHECK_NULL_INPUT(pwe_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_frwrd_extend_p2p_pwe_get_verify(
          unit,
          pwe_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_pwe_get_unsafe(
          unit,
          pwe_key,
          pwe_p2p_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_pwe_get()", 0, 0);
}

/*********************************************************************
*     Removes P2P service for packets entering with PWE key =
 *     (in-RIF x VC label).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_pwe_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_PWE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_key);

  res = soc_pb_pp_frwrd_extend_p2p_pwe_remove_verify(
          unit,
          pwe_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_pwe_remove_unsafe(
          unit,
          pwe_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_pwe_remove()", 0, 0);
}

/*********************************************************************
*     Add P2P service for packets entering with MIM
 *     (MAC-in_MAC) key = (ISID x ISID-domain) from PBP port.
 *     Thus, these packets will have the same processing and
 *     forwarding, regardless of the Customer MAC address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_mim_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *mim_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO             *mim_p2p_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_MIM_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mim_key);
  SOC_SAND_CHECK_NULL_INPUT(mim_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_extend_p2p_mim_add_verify(
          unit,
          mim_key,
          mim_p2p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_mim_add_unsafe(
          unit,
          mim_key,
          mim_p2p_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_mim_add()", 0, 0);
}

/*********************************************************************
*     Gets the P2P service for packets entering with MIM
 *     (MAC-in_MAC) key = (ISID x ISID-domain) from PBP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_mim_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                      *mim_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *mim_p2p_info,
    SOC_SAND_OUT uint8                                *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_MIM_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mim_key);
  SOC_SAND_CHECK_NULL_INPUT(mim_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_frwrd_extend_p2p_mim_get_verify(
          unit,
          mim_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_mim_get_unsafe(
          unit,
          mim_key,
          mim_p2p_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_mim_get()", 0, 0);
}

/*********************************************************************
*     Remove P2P service for packets entering with MIM
 *     (MAC-in_MAC) key = (ISID x ISID-domain) from PBP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_extend_p2p_mim_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                      *mim_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_EXTEND_P2P_MIM_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mim_key);

  res = soc_pb_pp_frwrd_extend_p2p_mim_remove_verify(
          unit,
          mim_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_extend_p2p_mim_remove_unsafe(
          unit,
          mim_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_extend_p2p_mim_remove()", 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_PWE_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_EXTEND_PWE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_PWE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

void
  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_P2P_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_EXTEND_PWE_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_EXTEND_PWE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_EXTEND_PWE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

