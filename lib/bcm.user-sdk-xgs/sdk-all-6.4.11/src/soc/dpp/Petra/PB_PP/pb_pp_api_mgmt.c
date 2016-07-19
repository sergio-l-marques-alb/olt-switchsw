/* $Id: pb_pp_api_mgmt.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_mgmt.c
*
* MODULE PREFIX:  soc_pb_pp_mgmt
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

#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>

#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

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

uint32
  soc_pb_pp_mgmt_device_close(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_DEVICE_CLOSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_device_close_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_device_close()", 0, 0);
}

/*********************************************************************
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_OPERATION_MODE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_operation_mode_set_unsafe(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_operation_mode_set()", 0, 0);
}

/*********************************************************************
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT SOC_PB_PP_MGMT_OPERATION_MODE     *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_OPERATION_MODE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_operation_mode_get_unsafe(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_operation_mode_get()", 0, 0);
}

/*********************************************************************
*     Set the ELK interface mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_elk_mode_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ELK_MODE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);

  res = soc_pb_pp_mgmt_elk_mode_set_verify(
          unit,
          elk_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_elk_mode_set_unsafe(
          unit,
          elk_mode,
          ingress_pkt_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_elk_mode_set()", 0, 0);
}

/*********************************************************************
*     Set the ELK interface mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_elk_mode_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT SOC_PB_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ELK_MODE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(elk_mode);
  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);

  res = soc_pb_pp_mgmt_elk_mode_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_elk_mode_get_unsafe(
          unit,
          elk_mode,
          ingress_pkt_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_elk_mode_get()", 0, 0);
}

/*********************************************************************
*     Determine whether the specified lookup is externalized
 *     or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_use_elk_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_USE_ELK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_mgmt_use_elk_set_verify(
          unit,
          lkp_type,
          use_elk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_use_elk_set_unsafe(
          unit,
          lkp_type,
          use_elk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_use_elk_set()", 0, 0);
}

/*********************************************************************
*     Determine whether the specified lookup is externalized
 *     or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_use_elk_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN SOC_PB_PP_MGMT_LKP_TYPE  lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_USE_ELK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lkp_type);
  SOC_SAND_CHECK_NULL_INPUT(use_elk);

  res = soc_pb_pp_mgmt_use_elk_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_mgmt_use_elk_get_unsafe(
          unit,
          lkp_type,
          use_elk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_use_elk_get()", 0, 0);
}

void
  SOC_PB_PP_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT SOC_PB_PP_MGMT_OPERATION_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_MGMT_OPERATION_MODE));
  info->authentication_enable = FALSE;
  info->system_vsi_enable = FALSE;
  info->hairpin_enable = FALSE;
  info->split_horizon_filter_enable = FALSE;
  SOC_PB_PP_MGMT_P2P_INFO_clear(&info->p2p_info);
  SOC_PB_PP_MGMT_IPV4_INFO_clear(&info->ipv4_info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_MGMT_P2P_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MGMT_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_MGMT_P2P_INFO));
  info->mim_vsi = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_MGMT_IPV4_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MGMT_IPV4_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_MGMT_IPV4_INFO));
  info->ipv4_host_extend = 0;
  info->pvlan_enable = 0;
  info->nof_vrfs = 0;
  for (ind = 0; ind < SOC_DPP_NOF_VRFS_PETRAB; ++ind)
  {
    info->max_routes_in_vrf[ind] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

void
  SOC_PB_PP_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("authentication_enable: %u\n\r",info->authentication_enable);
  soc_sand_os_printf("system_vsi_enable: %u\n\r",info->system_vsi_enable);
  soc_sand_os_printf("hairpin_enable: %u\n\r",info->hairpin_enable);
  soc_sand_os_printf("split_horizon_filter_enable: %u\n\r",info->split_horizon_filter_enable);
  soc_sand_os_printf("p2p_info :\n\r");
  SOC_PB_PP_MGMT_P2P_INFO_print(&info->p2p_info);
  SOC_PB_PP_MGMT_IPV4_INFO_print(&info->ipv4_info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_MGMT_P2P_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mim_vsi: %u\n\r",info->mim_vsi);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_MGMT_IPV4_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_IPV4_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("ipv4_host_extend: %u\n\r",info->ipv4_host_extend);
  soc_sand_os_printf("pvlan_enable: %u\n\r",info->pvlan_enable);
  soc_sand_os_printf("nof_vrfs: %u\n\r",info->nof_vrfs);
  soc_sand_os_printf("ipv4_host_extend: %u\n\r",info->ipv4_host_extend);
  for (ind = 0; ind < info->nof_vrfs; ++ind)
  {
    soc_sand_os_printf("info->max_routes_in_vrf[ind]: %u\n\r",info->max_routes_in_vrf[ind]);
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_PB_PP_MGMT_ELK_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_PP_MGMT_ELK_MODE_NONE:
    str = "none";
  break;
  case SOC_PB_PP_MGMT_ELK_MODE_NORMAL:
    str = "normal";
  break;
  case SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT:
    str = "b0_short";
  break;
  case SOC_PB_PP_MGMT_ELK_MODE_B0_LONG:
    str = "b0_long";
  break;
  case SOC_PB_PP_MGMT_ELK_MODE_B0_BOTH:
    str = "b0_both";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_PP_MGMT_LKP_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_PP_MGMT_LKP_TYPE_P2P:
    str = "p2p";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_ETH:
    str = "eth";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_TRILL_UC:
    str = "trill_uc";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_TRILL_MC:
    str = "trill_mc";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_IPV4_UC:
    str = "ipv3_uc";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_PB_PP_MGMT_LKP_TYPE_LSR:
    str = "lsr";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

