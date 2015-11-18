/* $Id: pb_api_stat_if.c,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_api_stat_if.c
*
* MODULE PREFIX:  soc_pb_stat
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

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_stat_if.h>
#include <soc/dpp/Petra/PB_TM/pb_stat_if.h>
#include <soc/dpp/Petra/petra_api_stat_if.h>

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
*     This function configures the format of the report sent
 *     through the statistics interface
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_report_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_REPORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_stat_if_report_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_stat_if_report_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_report_set()", 0, 0);
}

/*********************************************************************
*     This function configures the format of the report sent
 *     through the statistics interface
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_report_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_REPORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_stat_if_report_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_report_get()", 0, 0);
}

/*********************************************************************
*     Set the Statistic Interface under a BIST mode. Discard
 *     the statistic reports and allow the user to present test
 *     data on the statistic interface as a debug feature.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_bist_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_BIST_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_stat_if_bist_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_stat_if_bist_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_bist_set()", 0, 0);
}

/*********************************************************************
*     Set the Statistic Interface under a BIST mode. Discard
 *     the statistic reports and allow the user to present test
 *     data on the statistic interface as a debug feature.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_bist_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_BIST_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_stat_if_bist_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_bist_get()", 0, 0);
}

void
  SOC_PB_STAT_IF_BILLING_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_STAT_IF_BILLING_INFO));
  info->mode = SOC_PB_STAT_NOF_IF_BILLING_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_FAP20V_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_STAT_IF_FAP20V_INFO));
  info->mode = SOC_PB_STAT_NOF_IF_FAP20V_MODES;
  info->fabric_mc = SOC_PB_STAT_NOF_IF_MC_MODES;
  info->ing_rep_mc = SOC_PB_STAT_NOF_IF_MC_MODES;
  info->count_snoop = 0;
  info->is_original_pkt_size = 0;
  info->single_copy_reported = 0;
  info->cnm_report = SOC_PB_STAT_NOF_IF_CNM_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_MODE    report_mode,
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_STAT_IF_REPORT_MODE_INFO));
  switch(report_mode)
  {
  case SOC_PETRA_STAT_IF_REPORT_MODE_BILLING:
    SOC_PB_STAT_IF_BILLING_INFO_clear(&(info->billing));
    break;
  
  case SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V:
    SOC_PB_STAT_IF_FAP20V_INFO_clear(&(info->fap20v));
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_REPORT_MODE_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_STAT_IF_REPORT_INFO));
  info->mode = SOC_PETRA_STAT_NOF_IF_REPORT_MODES;
  SOC_PB_STAT_IF_REPORT_MODE_INFO_clear(SOC_PETRA_STAT_IF_REPORT_MODE_BILLING, &(info->format));
  SOC_PB_STAT_IF_REPORT_MODE_INFO_clear(SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V, &(info->format));
  info->sync_rate = 0;
  info->parity_mode = SOC_PB_STAT_NOF_IF_PARITY_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_BIST_PATTERN_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_STAT_IF_BIST_PATTERN));
  info->word1 = 0;
  info->word2 = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_BIST_INFO_clear(
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_STAT_IF_BIST_INFO));
  info->en_mode = SOC_PB_STAT_NOF_IF_BIST_EN_MODES;
  SOC_PB_STAT_IF_BIST_PATTERN_clear(&(info->pattern));
  info->nof_duplications = 0;
  info->nof_idle_clocks = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_STAT_IF_BILLING_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_BILLING_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_STAT_IF_BILLING_MODE_EGR_Q_NB:
    str = "egr_q_nb";
  break;
  case SOC_PB_STAT_IF_BILLING_MODE_CUD:
    str = "cud";
  break;
  case SOC_PB_STAT_IF_BILLING_MODE_VSI_VLAN:
    str = "vsi_vlan";
  break;
  case SOC_PB_STAT_IF_BILLING_MODE_BOTH_LIFS:
    str = "both_lifs";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_STAT_IF_FAP20V_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_FAP20V_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_STAT_IF_FAP20V_MODE_Q_SIZE:
    str = "q_size";
  break;
  case SOC_PB_STAT_IF_FAP20V_MODE_PKT_SIZE:
    str = "pkt_size";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_STAT_IF_MC_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_MC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_STAT_IF_MC_MODE_Q_NUM:
    str = "q_num";
  break;
  case SOC_PB_STAT_IF_MC_MODE_MC_ID:
    str = "mc_id";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_STAT_IF_CNM_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_CNM_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_STAT_IF_CNM_MODE_DIS:
    str = "disabled";
  break;
  case SOC_PB_STAT_IF_CNM_MODE_EN:
    str = "enabled";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_STAT_IF_PARITY_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_PARITY_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_STAT_IF_PARITY_MODE_DIS:
    str = "disabled";
  break;
  case SOC_PB_STAT_IF_PARITY_MODE_EN:
    str = "enabled";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PB_STAT_IF_BIST_EN_MODE_to_string(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_EN_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_STAT_IF_BIST_EN_MODE_DIS:
    str = "disabled";
  break;
  case SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN:
    str = "pattern";
  break;
  case SOC_PB_STAT_IF_BIST_EN_MODE_WALKING_ONE:
    str = "walking_one";
  break;
  case SOC_PB_STAT_IF_BIST_EN_MODE_PRBS:
    str = "prbs";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  SOC_PB_STAT_IF_BILLING_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", SOC_PB_STAT_IF_BILLING_MODE_to_string(info->mode));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_FAP20V_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", SOC_PB_STAT_IF_FAP20V_MODE_to_string(info->mode));
  soc_sand_os_printf("fabric_mc %s ", SOC_PB_STAT_IF_MC_MODE_to_string(info->fabric_mc));
  soc_sand_os_printf("ing_rep_mc %s ", SOC_PB_STAT_IF_MC_MODE_to_string(info->ing_rep_mc));
  soc_sand_os_printf("count_snoop: %u\n\r",info->count_snoop);
  soc_sand_os_printf("is_original_pkt_size: %u\n\r",info->is_original_pkt_size);
  soc_sand_os_printf("single_copy_reported: %u\n\r",info->single_copy_reported);
  soc_sand_os_printf("cnm_report %s ", SOC_PB_STAT_IF_CNM_MODE_to_string(info->cnm_report));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("billing:");
  SOC_PB_STAT_IF_BILLING_INFO_print(&(info->billing));
  soc_sand_os_printf("fap20v:");
  SOC_PB_STAT_IF_FAP20V_INFO_print(&(info->fap20v));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "mode:    %s \n\r",
    soc_petra_PETRA_STAT_IF_REPORT_MODE_to_string(info->mode)
    );
  soc_sand_os_printf("format:");
  SOC_PB_STAT_IF_REPORT_MODE_INFO_print(&(info->format));
  soc_sand_os_printf("sync_rate: %u[Nano-seconds]\n\r",info->sync_rate);
  soc_sand_os_printf("parity_mode %s ", SOC_PB_STAT_IF_PARITY_MODE_to_string(info->parity_mode));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_BIST_PATTERN_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("word1: %u\n\r",info->word1);
  soc_sand_os_printf("word2: %u\n\r",info->word2);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_STAT_IF_BIST_INFO_print(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("en_mode %s ", SOC_PB_STAT_IF_BIST_EN_MODE_to_string(info->en_mode));
  soc_sand_os_printf("pattern:");
  SOC_PB_STAT_IF_BIST_PATTERN_print(&(info->pattern));
  soc_sand_os_printf("nof_duplications: %u\n\r",info->nof_duplications);
  soc_sand_os_printf("nof_idle_clocks: %u\n\r",info->nof_idle_clocks);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

