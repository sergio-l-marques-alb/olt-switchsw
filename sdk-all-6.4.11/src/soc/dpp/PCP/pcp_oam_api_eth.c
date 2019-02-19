/* $Id: pcp_oam_api_eth.c,v 1.8 Broadcom SDK $
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
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PCP/pcp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_oam_api_eth.h>
#include <soc/dpp/PCP/pcp_oam_eth.h>

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
*     This function sets general configuration of Ethernet
 *     OAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_general_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_GENERAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_general_info_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_general_info_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_general_info_set()", 0, 0);
}

/*********************************************************************
*     This function sets general configuration of Ethernet
 *     OAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_general_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_ETH_GENERAL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_GENERAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_general_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_general_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_general_info_get()", 0, 0);
}

/*********************************************************************
*     This function maps the cos profile and the user priority
 *     (both are MEP attributes) to TC and DP for the ITMH
 *     (relevant for down MEP injection)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_COS_MAPPING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_cos_mapping_info_set_verify(
          unit,
          cos_ndx,
          prio_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_cos_mapping_info_set_unsafe(
          unit,
          cos_ndx,
          prio_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_cos_mapping_info_set()", cos_ndx, prio_ndx);
}

/*********************************************************************
*     This function maps the cos profile and the user priority
 *     (both are MEP attributes) to TC and DP for the ITMH
 *     (relevant for down MEP injection)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_COS_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_COS_MAPPING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_cos_mapping_info_get_verify(
          unit,
          cos_ndx,
          prio_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_cos_mapping_info_get_unsafe(
          unit,
          cos_ndx,
          prio_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_cos_mapping_info_get()", cos_ndx, prio_ndx);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an Attachment-Circuit (incoming/outgoing), MD-level,
 *     and MP direction (up/down). If the MP is one of the 4K
 *     accelerated MEPs, the function configures the related
 *     OAMP databases and associates the AC, MD-Level and the
 *     MP type with a user-provided handle. This handle is
 *     later used by user to access OAMP database for this MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mp_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mp_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mp_info_set_verify(
          unit,
          mp_key,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mp_info_set_unsafe(
          unit,
          mp_key,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mp_info_set()", 0, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an Attachment-Circuit (incoming/outgoing), MD-level,
 *     and MP direction (up/down). If the MP is one of the 4K
 *     accelerated MEPs, the function configures the related
 *     OAMP databases and associates the AC, MD-Level and the
 *     MP type with a user-provided handle. This handle is
 *     later used by user to access OAMP database for this MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mp_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_OUT PCP_OAM_ETH_MP_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mp_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mp_info_get_verify(
          unit,
          mp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_mp_info_get_unsafe(
          unit,
          mp_key,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mp_info_get()", 0, 0);
}

/*********************************************************************
*     This function configures an accelerated MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_acc_mep_info_set_verify(
          unit,
          acc_mep_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_mep_info_set_unsafe(
          unit,
          acc_mep_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_info_set()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function configures an accelerated MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_acc_mep_info_get_verify(
          unit,
          acc_mep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_mep_info_get_unsafe(
          unit,
          acc_mep_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_info_get()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets user custom header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint32                        header
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = pcp_oam_eth_acc_mep_custom_header_set_verify(
          unit,
          acc_mep_ndx,
          header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_mep_custom_header_set_unsafe(
          unit,
          acc_mep_ndx,
          header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_custom_header_set()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets user custom header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint32                        *header
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(header);

  res = pcp_oam_eth_acc_mep_custom_header_get_verify(
          unit,
          acc_mep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_mep_custom_header_get_unsafe(
          unit,
          acc_mep_ndx,
          header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_custom_header_get()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets LMM info for the specified LMM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_LMM_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_acc_lmm_set_verify(
          unit,
          acc_lmm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_lmm_set_unsafe(
          unit,
          acc_lmm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_lmm_set()", acc_lmm_ndx, 0);
}

/*********************************************************************
*     This function sets LMM info for the specified LMM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_LMM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_LMM_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_acc_lmm_get_verify(
          unit,
          acc_lmm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_lmm_get_unsafe(
          unit,
          acc_lmm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_lmm_get()", acc_lmm_ndx, 0);
}

/*********************************************************************
*     This function sets DLM info for the specified DLM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_DLM_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_acc_dlm_set_verify(
          unit,
          acc_dlm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_dlm_set_unsafe(
          unit,
          acc_dlm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_dlm_set()", acc_dlm_ndx, 0);
}

/*********************************************************************
*     This function sets DLM info for the specified DLM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_DLM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_DLM_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_acc_dlm_get_verify(
          unit,
          acc_dlm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_dlm_get_unsafe(
          unit,
          acc_dlm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_dlm_get()", acc_dlm_ndx, 0);
}

/*********************************************************************
*     This function adds a remote MEP to list of remote MEPs
 *     associated with the accelerated MEP identified by
 *     acc_mep_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_add(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key,
    SOC_SAND_IN  uint32                        rmep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rmep_key);

  res = pcp_oam_eth_rmep_add_verify(
          unit,
          rmep_key,
          rmep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_rmep_add_unsafe(
          unit,
          rmep_key,
          rmep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_add()", 0, 0);
}

/*********************************************************************
*     This function removes a remote MEP to list of remote
 *     MEPs associated with the accelerated MEP identified by
 *     acc_mep_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_remove(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rmep_key);

  res = pcp_oam_eth_rmep_remove_verify(
          unit,
          rmep_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_rmep_remove_unsafe(
          unit,
          rmep_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_remove()", 0, 0);
}

/*********************************************************************
*     This API sets a remote MEP info.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rmep_info);

  res = pcp_oam_eth_rmep_info_set_verify(
          unit,
          rmep_ndx,
          rmep_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_rmep_info_set_unsafe(
          unit,
          rmep_ndx,
          rmep_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_info_set()", rmep_ndx, 0);
}

/*********************************************************************
*     This API sets a remote MEP info.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_INFO           *rmep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rmep_info);

  res = pcp_oam_eth_rmep_info_get_verify(
          unit,
          rmep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_rmep_info_get_unsafe(
          unit,
          rmep_ndx,
          rmep_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_info_get()", rmep_ndx, 0);
}

/*********************************************************************
*     This function sets/clears remote defect indication on
 *     outgoing CCM messages
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint8                       rdi
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = pcp_oam_eth_acc_mep_tx_rdi_set_verify(
          unit,
          acc_mep_ndx,
          rdi
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_mep_tx_rdi_set_unsafe(
          unit,
          acc_mep_ndx,
          rdi
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_tx_rdi_set()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets/clears remote defect indication on
 *     outgoing CCM messages
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint8                       *rdi
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rdi);

  res = pcp_oam_eth_acc_mep_tx_rdi_get_verify(
          unit,
          acc_mep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_acc_mep_tx_rdi_get_unsafe(
          unit,
          acc_mep_ndx,
          rdi
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_tx_rdi_get()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function returns the last measured delay. In case
 *     DM was not defined on the provided MEP, an error is
 *     thrown
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_mep_delay_measurement_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT uint32                        *dm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dm);

  res = pcp_oam_eth_mep_delay_measurement_get_verify(
          unit,
          acc_dlm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_mep_delay_measurement_get_unsafe(
          unit,
          acc_dlm_ndx,
          dm
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_mep_delay_measurement_get()", acc_dlm_ndx, 0);
}

/*********************************************************************
*     This function returns the loss measurement counters of
 *     the device (both local and remote). In case LM was not
 *     defined on the provided MEP, an error is thrown
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_mep_loss_measurement_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_eth_mep_loss_measurement_get_verify(
          unit,
          acc_lmm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_eth_mep_loss_measurement_get_unsafe(
          unit,
          acc_lmm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_mep_loss_measurement_get()", acc_lmm_ndx, 0);
}

void
  PCP_OAM_ETH_GENERAL_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_GENERAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_GENERAL_INFO));
  info->custom_header_size = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_COS_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_COS_MAPPING_INFO));
  info->tc = 0;
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_MP_KEY_clear(
    SOC_SAND_OUT PCP_OAM_ETH_MP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_MP_KEY));
  info->level = 0;
  info->direction = PCP_OAM_NOF_ETH_MEP_DIRECTIONS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_MP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_MP_INFO));
  info->valid = 0;
  info->acc_mep_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO));
  info->dn_vid = 0;
  info->ac = 0;
  info->cos_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO));
  info->up_vid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_FWD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_FWD_INFO));
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_clear(&(info->up_mep));
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_clear(&(info->down_mep));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_CCM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_CCM_INFO));
  info->ccm_interval = PCP_OAM_NOF_ETH_INTERVALS;
  info->ccm_priority = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LMM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_LMM_INFO));
  info->lmm_enable = 0;
  info->lmm_index = 0;
  info->is_dual = 0;
  info->lmr_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_DLM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_DLM_INFO));
  info->dlm_enable = 0;
  info->dlm_index = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO));
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_clear(&(info->ccm_info));
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_clear(&(info->lmm_info));
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_clear(&(info->dlm_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_INFO));
  info->ma_id = 0;
  info->mep_id = 0;
  info->md_level = 0;
  info->direction = PCP_OAM_NOF_ETH_MEP_DIRECTIONS;
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_clear(&(info->frwd_info));
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_clear(&(info->peformance_monitoring_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_RMEP_KEY_clear(
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_RMEP_KEY));
  info->acc_mep_id = 0;
  info->rmep_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_RMEP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_RMEP_INFO));
  info->interval = PCP_OAM_NOF_ETH_INTERVALS;
  info->rdi_received = 0;
  info->local_defect = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_DLM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_DLM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_DLM_INFO));
  info->interval = PCP_OAM_NOF_ETH_INTERVALS;
  info->priority = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_LMM_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_LMM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_LMM_INFO));
  info->interval = PCP_OAM_NOF_ETH_INTERVALS;
  info->priority = 0;
  info->rx_counter = 0;
  info->tx_counter = 0;  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO));
  info->tx_fcf_curr = 0;
  info->tx_fcf_prev = 0;
  info->rx_fcb_curr = 0;
  info->rx_fcb_prev = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_clear(
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO));
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_clear(&(info->local));
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_clear(&(info->peer));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_ETH_MEP_DIRECTION_to_string(
    SOC_SAND_IN  PCP_OAM_ETH_MEP_DIRECTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_ETH_MEP_DIRECTION_UP:
    str = "up";
  break;
  case PCP_OAM_ETH_MEP_DIRECTION_DOWN:
    str = "down";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  PCP_OAM_ETH_INTERVAL_to_string(
    SOC_SAND_IN  PCP_OAM_ETH_INTERVAL enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_ETH_INTERVAL_0:
    str = "interval_0";
  break;
  case PCP_OAM_ETH_INTERVAL_1:
    str = "interval_1";
  break;
  case PCP_OAM_ETH_INTERVAL_2:
    str = "interval_2";
  break;
  case PCP_OAM_ETH_INTERVAL_3:
    str = "interval_3";
  break;
  case PCP_OAM_ETH_INTERVAL_4:
    str = "interval_4";
  break;
  case PCP_OAM_ETH_INTERVAL_5:
    str = "interval_5";
  break;
  case PCP_OAM_ETH_INTERVAL_6:
    str = "interval_6";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  PCP_OAM_ETH_GENERAL_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("custom_header_size: %u\n\r"), info->custom_header_size));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&info->mep_sa_msb);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_COS_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META("dp: %u\n\r"),info->dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_MP_KEY_print(
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("level: %u\n\r"),info->level));
  LOG_CLI((BSL_META("direction %s\n\r"), PCP_OAM_ETH_MEP_DIRECTION_to_string(info->direction)));
  LOG_CLI((BSL_META("ac_ndx: %u\n\r"),info->ac_ndx));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_MP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META("acc_mep_id: %u\n\r"),info->acc_mep_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("dn_vid: %u\n\r"),info->dn_vid));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&info->dn_sa_lsb);
  LOG_CLI((BSL_META("\n")));
  LOG_CLI((BSL_META("ac: %u\n\r"),info->ac));
  LOG_CLI((BSL_META("cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META("destination:\n\r")));
  SOC_TMC_DEST_INFO_print(&(info->destination));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("up_vid: %u\n\r"),info->up_vid));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&info->up_sa_lsb);
  LOG_CLI((BSL_META("\n")));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("up_mep:\n")));
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_print(&(info->up_mep));
  LOG_CLI((BSL_META("down_mep:\n")));
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_print(&(info->down_mep));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_CCM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("ccm_interval %s "), PCP_OAM_ETH_INTERVAL_to_string(info->ccm_interval)));
  LOG_CLI((BSL_META("ccm_priority: %u\n\r"), info->ccm_priority));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LMM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("lmm_enable: %u\n\r"),info->lmm_enable));
  LOG_CLI((BSL_META("lmm_index: %u\n\r"),info->lmm_index));
  LOG_CLI((BSL_META("is_dual: %u\n\r"),info->is_dual));
  LOG_CLI((BSL_META("lmr_enable: %u\n\r"),info->lmr_enable));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_DLM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("dlm_enable: %u\n\r"),info->dlm_enable));
  LOG_CLI((BSL_META("dlm_index: %u\n\r"),info->dlm_index));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("ccm_info:\n")));
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_print(&(info->ccm_info));
  LOG_CLI((BSL_META("lmm_info:\n")));
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_print(&(info->lmm_info));
  LOG_CLI((BSL_META("dlm_info:\n")));
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_print(&(info->dlm_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("ma_id: %u\n\r"),info->ma_id));
  LOG_CLI((BSL_META("mep_id: %u\n\r"),info->mep_id));
  LOG_CLI((BSL_META("md_level: %u\n\r"),info->md_level));
  LOG_CLI((BSL_META("direction %s\n"), PCP_OAM_ETH_MEP_DIRECTION_to_string(info->direction)));
  LOG_CLI((BSL_META("frwd_info:\n")));
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_print(&(info->frwd_info));
  LOG_CLI((BSL_META("peformance_monitoring_info:\n")));
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_print(&(info->peformance_monitoring_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_RMEP_KEY_print(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("acc_mep_id: %u\n\r"),info->acc_mep_id));
  LOG_CLI((BSL_META("rmep_id: %u\n\r"),info->rmep_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_RMEP_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("interval %s "), PCP_OAM_ETH_INTERVAL_to_string(info->interval)));
  LOG_CLI((BSL_META("rdi_received: %u\n\r"),info->rdi_received));
  LOG_CLI((BSL_META("local_defect: %u\n\r"),info->local_defect));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_DLM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("interval %s\n"), PCP_OAM_ETH_INTERVAL_to_string(info->interval)));
  LOG_CLI((BSL_META("priority: %u\n\r"), info->priority));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&info->da);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_LMM_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("interval %s\n"), PCP_OAM_ETH_INTERVAL_to_string(info->interval)));
  LOG_CLI((BSL_META("priority: %u\n\r"), info->priority));
  LOG_CLI((BSL_META("rx_counter: %u\n\r"), info->rx_counter));
  LOG_CLI((BSL_META("tx_counter: %u\n\r"), info->tx_counter));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&info->da);
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("tx_fcf_curr: %u\n\r"),info->tx_fcf_curr));
  LOG_CLI((BSL_META("tx_fcf_prev: %u\n\r"),info->tx_fcf_prev));
  LOG_CLI((BSL_META("rx_fcb_curr: %u\n\r"),info->rx_fcb_curr));
  LOG_CLI((BSL_META("rx_fcb_prev: %u\n\r"),info->rx_fcb_prev));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_print(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("local:\n")));
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_print(&(info->local));
  LOG_CLI((BSL_META("peer:\n")));
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_print(&(info->peer));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

