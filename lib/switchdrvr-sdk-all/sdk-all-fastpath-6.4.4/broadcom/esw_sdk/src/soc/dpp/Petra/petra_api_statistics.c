/* $Id: petra_api_statistics.c,v 1.9 Broadcom SDK $
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
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/Petra/petra_statistics.h>
#include <soc/dpp/Petra/petra_api_statistics.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_framework.h>

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
extern
 SOC_PETRA_COUNTER_INFO
  soc_petra_counters_info[SOC_PETRA_NOF_COUNTER_TYPES];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Selects the Incoming FAP Ports for which the number of
*     words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*     SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IFP_CNT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_ifp_cnt_select_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_stat_ifp_cnt_select_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_ifp_cnt_select_set()",0,0);
}

/*********************************************************************
*     Selects the Incoming FAP Ports for which the number of
*     words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*     SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IFP_CNT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_stat_ifp_cnt_select_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_ifp_cnt_select_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_ifp_cnt_select_get()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VOQ_CNT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_stat_voq_cnt_select_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_voq_cnt_select_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_voq_cnt_select_set()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VOQ_CNT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_stat_voq_cnt_select_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_voq_cnt_select_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_voq_cnt_select_get()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VSQ_CNT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_stat_vsq_cnt_select_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_vsq_cnt_select_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_vsq_cnt_select_set()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VSQ_CNT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_stat_vsq_cnt_select_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_vsq_cnt_select_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_vsq_cnt_select_get()",0,0);
}

/*****************************************************
* see remarks & definitions in the soc_petra_api_statistics.h
*****************************************************/
uint32
  soc_petra_stat_pkt_counters_collect(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_PKT_COUNTERS_COLLECT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_pkt_counters_collect_unsafe(
          unit,
          SOC_PETRA_NOF_COUNTER_TYPES
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_pkt_counters_collect()",0,0);
}

/*****************************************************
* see remarks & definitions in the soc_petra_api_statistics.h
*****************************************************/
uint32
  soc_petra_stat_counter_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE  counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                *result_ptr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_COUNTER_GET);

  SOC_SAND_CHECK_NULL_INPUT(result_ptr);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  if(counter_type >= SOC_PETRA_NOF_COUNTER_TYPES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_NO_SUCH_COUNTER_ERR,  10, exit) ;
  }

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_counter_get_unsafe(
          unit,
          counter_type,
          result_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_counter_get()",0,0);
}

/*****************************************************
* see remarks & definitions in the soc_petra_api_statistics.h
*****************************************************/
uint32
  soc_petra_stat_all_counters_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PETRA_STAT_PRINT_LEVEL               print_level,
    SOC_SAND_IN  uint32                             sampling_period,
    SOC_SAND_OUT SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS    *all_statistic_counters
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_ALL_COUNTERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(all_statistic_counters);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_stat_all_counters_get_unsafe(
          unit,
          print_level,
          sampling_period,
          all_statistic_counters
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_all_counters_get()",0,0);
}

void
  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS));
  for (ind=0; ind< SOC_PETRA_NOF_COUNTER_TYPES; ++ind)
  {
    info->counters[ind].overflowed = 0;
    info->counters[ind].u64.arr[0] = 0;
    info->counters[ind].u64.arr[1] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_STAT_IFP_SELECT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_STAT_IFP_SELECT_INFO));
  for (ind = 0; ind < SOC_PETRA_STAT_PER_IFP_NOF_CNTS; ++ind)
  {
    info->ifp_id[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_STAT_VOQ_SELECT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_STAT_VOQ_SELECT_INFO));
  info->voq_id = 0;
  info->enable_also_mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_STAT_VSQ_SELECT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_STAT_VSQ_SELECT_INFO));
  info->vsq_grp_id = 0;
  info->vsq_in_grp_id = 0;
  info->enable_also_mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

void
  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS  *all_counters,
    SOC_SAND_IN SOC_PETRA_STAT_PRINT_LEVEL              print_level,
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS                  print_flavor
  )
{
  uint32
    counter_i;
  uint32
    short_format    = print_flavor & SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT),
    dont_print_zero = print_flavor & SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS);
  SOC_PETRA_COUNTER_INFO
    *soc_petra_counters_info;
  uint8
    is_cnt_relevant;

  soc_petra_counters_info = soc_petra_stat_counter_info_ptr_get();

  if ((NULL == all_counters) || (NULL == soc_petra_counters_info))
  {
    soc_sand_os_printf("soc_petra_print_PETRA_ALL_STATISTIC_COUNTERS received NULL ptr\n\r");
    goto exit;
  }

  for(counter_i = 0; counter_i < SOC_PETRA_NOF_COUNTER_TYPES; ++counter_i)
  {
    soc_petra_stat_is_relevant_counter(
            unit,
            counter_i,
            &is_cnt_relevant
          );

    if(is_cnt_relevant == FALSE)
    {
      continue;
    }


    if(
       (soc_sand_u64_is_zero(&(all_counters->counters[counter_i].u64))) &&
       (dont_print_zero)
      )
    {
      continue;
    }
    if ( SOC_PETRA_STAT_PARAMS_HAS_THIS_FLAVOR(soc_petra_counters_info[counter_i].params, print_level) == 0)
    {
      continue;
    }

    if(short_format)
    {
      soc_sand_os_printf("  %-20s:", soc_petra_PETRA_STAT_COUNTER_TYPE_to_string(counter_i, TRUE));
      if (
          (all_counters->counters[counter_i].u64.arr[0] == SOC_PETRA_STAT_COUNTER_TIMER_INVALID)
         && (all_counters->counters[counter_i].u64.arr[1] == SOC_PETRA_STAT_COUNTER_TIMER_INVALID)
         )
      {
        soc_sand_os_printf(" Sampling timer error");
      }
      else
      {
        soc_sand_64cnt_print(&(all_counters->counters[counter_i]), short_format);
      }
      soc_sand_os_printf("\n\r");
    }
    else
    {
      soc_sand_os_printf("  %-20s:", soc_petra_PETRA_STAT_COUNTER_TYPE_to_string(counter_i, TRUE));
      soc_sand_os_printf("  %-60s:", soc_petra_PETRA_STAT_COUNTER_TYPE_to_string(counter_i, FALSE));
      if (
          (all_counters->counters[counter_i].u64.arr[0] == SOC_PETRA_STAT_COUNTER_TIMER_INVALID)
          && (all_counters->counters[counter_i].u64.arr[1] == SOC_PETRA_STAT_COUNTER_TIMER_INVALID)
        )
      {
        soc_sand_os_printf(" Sampling timer error");
      }
      else
      {
        soc_sand_64cnt_print(&(all_counters->counters[counter_i]), short_format);
      }
      soc_sand_os_printf("\n\r");
    }
  }

exit:
  return;
}

uint32
  soc_petra_stat_all_counters_get_and_print(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            sampling_period
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS
    all_statistic_counters;
  uint8
    print_format;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_ALL_COUNTERS_GET);

  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_clear(&all_statistic_counters);

  res = soc_petra_stat_all_counters_get(
          unit,
          SOC_PETRA_STAT_PRINT_LEVEL_ALL,
          sampling_period,
          &all_statistic_counters
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  print_format = 0;
  print_format |= SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT);
  print_format |= SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS);

  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_print(unit, &all_statistic_counters, SOC_PETRA_STAT_PRINT_LEVEL_ALL, print_format);

  sal_msleep(50);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_all_counters_get_and_print()",0,0);
}

void
  SOC_PETRA_STAT_IFP_SELECT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PETRA_STAT_PER_IFP_NOF_CNTS; ++ind)
  {
    soc_sand_os_printf("IFP-id[%u]: %u\n\r",ind,info->ifp_id[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_STAT_VOQ_SELECT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("VOQ_id: %u\n\r",info->voq_id);
  if (info->enable_also_mask != 0)
  {
    soc_sand_os_printf("enable_also_mask: 0x%x\n\r",info->enable_also_mask);
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_STAT_VSQ_SELECT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("VSQ_grp_id: %s, in_grp_id: %u\n\r",soc_petra_PETRA_ITM_VSQ_GROUP_to_string(info->vsq_grp_id), info->vsq_in_grp_id);
  if (info->enable_also_mask != 0)
  {
    soc_sand_os_printf("enable_also_mask: 0x%x\n\r",info->enable_also_mask);
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
