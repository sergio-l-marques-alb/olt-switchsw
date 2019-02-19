#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_llp_mirror.c,v 1.6 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP

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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_mirror.h>

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
*     Set a mirroring for port and VLAN, so all incoming
 *     packets enter from the given port and identified with
 *     the given VID will be associated with Mirror command
 *     (Enables mirroring (copying) the packets to additional
 *     destination.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_vlan_add(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                      mirror_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_llp_mirror_port_vlan_add_verify(
          unit,
          local_port_ndx,
          vid_ndx,
          mirror_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_mirror_port_vlan_add_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          vid_ndx,
          mirror_profile,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_add()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     Remove a mirroring for port and VLAN
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_vlan_remove(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_llp_mirror_port_vlan_remove_verify(
          unit,
          local_port_ndx,
          vid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_mirror_port_vlan_remove_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          vid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_remove()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     Get the assigned mirroring profile for port and VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_vlan_get(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                      *mirror_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mirror_profile);

  res = arad_pp_llp_mirror_port_vlan_get_verify(
          unit,
          local_port_ndx,
          vid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_mirror_port_vlan_get_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          vid_ndx,
          mirror_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_get()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     Set default mirroring profiles for port
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_dflt_set(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);

  res = arad_pp_llp_mirror_port_dflt_set_verify(
          unit,
          local_port_ndx,
          dflt_mirroring_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_mirror_port_dflt_set_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          dflt_mirroring_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_dflt_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Set default mirroring profiles for port
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_dflt_get(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);

  res = arad_pp_llp_mirror_port_dflt_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_mirror_port_dflt_get_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          dflt_mirroring_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_dflt_get()", local_port_ndx, 0);
}

void
  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

void
  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

