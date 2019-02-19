/* $Id: pcp_oam_api_general.c,v 1.6 Broadcom SDK $
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
#include <soc/dpp/PCP/pcp_oam_general.h>
#include <soc/dpp/PCP/pcp_oam_api_general.h>

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
*     This procedure applies general configuration of the
 *     OAMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_general_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_GENERAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_general_info_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_general_info_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_general_info_set()", 0, 0);
}

/*********************************************************************
*     This procedure applies general configuration of the
 *     OAMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_general_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_GENERAL_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_GENERAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_general_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_general_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_general_info_get()", 0, 0);
}

/*********************************************************************
*     This procedure registers user callback that will be
 *     called as a response to a certain interrupt.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_callback_function_register(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct,
    SOC_SAND_OUT uint32                        *callback_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_CALLBACK_FUNCTION_REGISTER);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(user_callback_struct);
  SOC_SAND_CHECK_NULL_INPUT(callback_id);

  res = pcp_oam_callback_function_register_verify(
          unit,
          user_callback_struct
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_callback_function_register_unsafe(
          unit,
          user_callback_struct,
          callback_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_callback_function_register()", 0, 0);
}

/*********************************************************************
*     This procedure is called from the CPU ISR when interrupt
 *     handling for OAMP is required.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_interrupt_handler(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_INTERRUPT_HANDLER);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = pcp_oam_interrupt_handler_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_interrupt_handler_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_interrupt_handler()", 0, 0);
}

/*********************************************************************
*     This function reads and parses the head of the OAM
 *     message FIFO. If no message exists an indication is
 *     returned.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_msg_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MSG_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MSG_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_msg_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_oam_msg_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_msg_info_get()", 0, 0);
}

void
  PCP_OAM_MSG_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MSG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_MSG_INFO));
  info->exception = PCP_OAM_NOF_MSG_EXCEPTION_TYPES;
  info->info1 = 0;
  info->info2 = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_GENERAL_INFO_clear(
    SOC_SAND_OUT PCP_OAM_GENERAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_OAM_GENERAL_INFO));
  info->cpu_dst_sys_port = 0;
  info->cpu_tc = 0;
  info->cpu_dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_MSG_EXCEPTION_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MSG_EXCEPTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case PCP_OAM_MSG_NONE:
    str = "pcp_oam_msg_none";
  break;
  case PCP_OAM_MSG_CCM_TIMEOUT:
    str = "pcp_oam_msg_ccm_timeout";
  break;
  case PCP_OAM_MSG_CCM_EXIT_TIMEOUT:
    str = "pcp_oam_msg_ccm_exit_timeout";
  break;
  case PCP_OAM_MSG_BFD_TIMEOUT:
    str = "pcp_oam_msg_bfd_timeout";
  break;
  case PCP_OAM_MSG_MPLS_DLOCKV_SET:
    str = "pcp_oam_msg_mpls_dlockv_set";
  break;
  case PCP_OAM_MSG_MPLS_DLOCKV_CLEAR:
    str = "pcp_oam_msg_mpls_dlockv_clear";
  break;
  case PCP_OAM_MSG_MPLS_FDI_CLEAR:
    str = "pcp_oam_msg_mpls_fdi_clear";
  break;
  case PCP_OAM_MSG_MPLS_BDI_CLEAR:
    str = "pcp_oam_msg_mpls_bdi_clear";
  break;
  case PCP_OAM_MSG_MPLS_DEXCESS_SET:
    str = "pcp_oam_msg_mpls_dexcess_set";
  break;
  case PCP_OAM_MSG_MPLS_DEXCESS_CLEAR:
    str = "pcp_oam_msg_mpls_dexcess_clear";
  break;
  case PCP_OAM_MSG_UNKNOWN:
    str = "pcp_oam_msg_unknown";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  PCP_OAM_MSG_INFO_print(
    SOC_SAND_IN  PCP_OAM_MSG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("exception %s\n"), PCP_OAM_MSG_EXCEPTION_TYPE_to_string(info->exception)));
  LOG_CLI((BSL_META("info1: %u\n\r"),info->info1));
  LOG_CLI((BSL_META("info2: %u\n\r"),info->info2));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OAM_GENERAL_INFO_print(
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("cpu_dst_sys_port: %u\n\r"),info->cpu_dst_sys_port));
  LOG_CLI((BSL_META("cpu_tc: %u\n\r"),info->cpu_tc));
  LOG_CLI((BSL_META("cpu_dp: %u\n\r"),info->cpu_dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

