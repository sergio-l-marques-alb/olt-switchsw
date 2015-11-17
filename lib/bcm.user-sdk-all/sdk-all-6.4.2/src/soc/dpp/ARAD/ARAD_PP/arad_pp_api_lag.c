#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_lag.c,v 1.9 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRUNK

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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>

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
*     Configure a LAG. A LAG is defined by a group of System
 *     Physical Ports that compose it. This configuration
 *     affects 1. LAG resolution: when the destination of
 *     packet is LAG 2. Learning: when packet source port
 *     belongs to LAG, then the LAG is learnt.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                                *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  res = arad_pp_lag_set_verify(
          unit,
          lag_ndx,
          lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_set_unsafe(
          unit,
          lag_ndx,
          lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_set()", lag_ndx, 0);
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
 *     Physical Ports that compose it. This configuration
 *     affects 1. LAG resolution: when the destination of
 *     packet is LAG 2. Learning: when packet source port
 *     belongs to LAG, then the LAG is learnt.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_OUT ARAD_PP_LAG_INFO                                *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  res = arad_pp_lag_get_verify(
          unit,
          lag_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_get_unsafe(
          unit,
          lag_ndx,
          lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_get()", lag_ndx, 0);
}

/*********************************************************************
*     Add a system port as a member in LAG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_member_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                              *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_MEMBER_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(member);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_lag_member_add_verify(
          unit,
          lag_ndx,
          member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_member_add_unsafe(
          unit,
          lag_ndx,
          member,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_member_add()", lag_ndx, 0);
}

/*********************************************************************
*     Remove a system port from a LAG.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_member_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                     lag_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER                         *member      
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_MEMBER_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_lag_member_remove_verify(
          unit,
          lag_ndx,
          member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_member_remove_unsafe(
          unit,
          lag_ndx,
          member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_member_remove()", lag_ndx, 0);
}

/*********************************************************************
*     Set the LAG hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO                    *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  res = arad_pp_lag_hashing_global_info_set_verify(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_hashing_global_info_set_unsafe(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_global_info_set()", 0, 0);
}

/*********************************************************************
*     Set the LAG hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT ARAD_PP_LAG_HASH_GLOBAL_INFO                    *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  res = arad_pp_lag_hashing_global_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_hashing_global_info_get_unsafe(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_global_info_get()", 0, 0);
}

/*********************************************************************
*     Set the LAG hashing per-lag attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_port_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO                      *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_hash_info);

  res = arad_pp_lag_hashing_port_info_set_verify(
          unit,
          port_ndx,
          lag_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_hashing_port_info_set_unsafe(
          unit,
          port_ndx,
          lag_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_port_info_set()", port_ndx, 0);
}

/*********************************************************************
*     Set the LAG hashing per-lag attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_port_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_OUT ARAD_PP_LAG_HASH_PORT_INFO                      *lag_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_hash_info);

  res = arad_pp_lag_hashing_port_info_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_hashing_port_info_get_unsafe(
          unit,
          port_ndx,
          lag_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_port_info_get()", port_ndx, 0);
}

/*********************************************************************
*     Sets the key used by hash functions for LAG/ECMP load
 *     balancing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO       *mask_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_MASK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mask_info);

  res = arad_pp_lag_hashing_mask_set_verify(
          unit,
          mask_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_hashing_mask_set_unsafe(
          unit,
          mask_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_mask_set()", 0, 0);
}

/*********************************************************************
*     Sets the key used by hash functions for LAG/ECMP load
 *     balancing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT ARAD_PP_HASH_MASK_INFO       *mask_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LAG_HASHING_MASK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mask_info);

  res = arad_pp_lag_hashing_mask_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_hashing_mask_get_unsafe(
          unit,
          mask_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_hashing_mask_get()", 0, 0);
}

uint32
  arad_pp_lag_lb_key_range_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LAG_INFO                                *lag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lag_lb_key_range_set_unsafe(
          unit,
          lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lag_lb_key_range_set()", 0, 0);
}


void
  ARAD_PP_LAG_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LAG_HASH_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_HASH_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_HASH_MASK_INFO_clear(
    SOC_SAND_OUT ARAD_PP_HASH_MASK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_HASH_MASK_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LAG_MEMBER_clear(
    SOC_SAND_OUT ARAD_PP_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_MEMBER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LAG_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LAG_HASH_FRST_HDR_to_string(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_FRST_HDR enum_val
  )
{
  return SOC_PPC_LAG_HASH_FRST_HDR_to_string(enum_val);
}

const char*
  ARAD_PP_LAG_LB_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LAG_LB_TYPE enum_val
  )
{
  return SOC_PPC_LAG_LB_TYPE_to_string(enum_val);
}

void
  ARAD_PP_LAG_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_HASH_GLOBAL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LAG_HASH_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_HASH_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_HASH_MASK_INFO_print(
    SOC_SAND_IN  ARAD_PP_HASH_MASK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_HASH_MASK_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LAG_MEMBER_print(
    SOC_SAND_IN  ARAD_PP_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_MEMBER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LAG_INFO_print(
    SOC_SAND_IN  ARAD_PP_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

