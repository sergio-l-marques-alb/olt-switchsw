/* $Id: petra_api_stat_if.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_stat_if.h>
#include <soc/dpp/Petra/petra_stat_if.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_general.h>

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
*     This function configures the working mode of the
*     statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_stat_if_info_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_if_info_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_info_set()",0,0);
}

/*********************************************************************
*     This function configures the working mode of the
*     statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_if_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_info_get()",0,0);
}

/*********************************************************************
*     This function configures the format of the report sent
*     through the statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_report_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_REPORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_stat_if_report_info_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_if_report_info_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_report_info_set()",0,0);
}

/*********************************************************************
*     This function configures the format of the report sent
*     through the statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_report_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_REPORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_if_report_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_report_info_get()",0,0);
}

void
  soc_petra_PETRA_STAT_IF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BILLING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_FAP20V_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_BIST_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_PATTERN_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_BIST_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_STAT_IF_PHASE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_PHASE enum_val
  )
{
  return SOC_TMC_STAT_IF_PHASE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_REPORT_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_REPORT_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_BILLING_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_BILLING_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_BILLING_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_FAP20V_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_FAP20V_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_FAP20V_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_MC_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_MC_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_MC_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_CNM_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_CNM_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_CNM_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_PARITY_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_PARITY_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_PARITY_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_STAT_IF_BIST_EN_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_BIST_EN_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_BIST_EN_MODE_to_string(enum_val);
}

void
  soc_petra_PETRA_STAT_IF_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_MODE_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BILLING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO_print(
    SOC_SAND_IN SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_FAP20V_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_MODE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_BIST_PATTERN_print(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_PATTERN_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_STAT_IF_BIST_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

