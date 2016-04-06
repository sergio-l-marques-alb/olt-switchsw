/* $Id: petra_general.c,v 1.9 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_interrupt_service.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_general.h>
#endif



/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_general.c,v 1.9 Broadcom SDK $
 * Numeric correction used in Fabric Multicast rate calculation
 */
#define SOC_PA_FMC_RATE_DELTA_CONST(is_for_ips)               (102) /* Both for IPS and FMC */

#ifdef LINK_PB_LIBRARIES
  /* The constant is 7 only for Soc_petra-B and for FMC, otherwise 102 */
  #define SOC_PB_FMC_RATE_DELTA_CONST(is_for_ips)             ((is_for_ips == TRUE)? SOC_PA_FMC_RATE_DELTA_CONST(is_for_ips) : 7)
#else
  #define SOC_PB_FMC_RATE_DELTA_CONST(is_for_ips)             SOC_PA_FMC_RATE_DELTA_CONST(is_for_ips)
#endif

#define SOC_PETRA_FMC_RATE_DELTA_CONST(is_for_ips)  \
  SOC_PETRA_CST_VALUE_DISTINCT(FMC_RATE_DELTA_CONST(is_for_ips), uint32)
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

typedef SOC_SAND_RET (*SOC_PETRA_SEND_MESSAGE_TO_QUEUE_FUNC)(SOC_SAND_IN  uint32 msg);

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/*
 * Polling is done here using SOC_SAND TCM. The user may implement his own
 * message queue monitoring thread and bind it instead of 'soc_sand_tcm_send_message_to_q_from_int'.
 * Suppose monitoring is done using TCM, one should enable it before using the interrupt mechanism.
 */
static
  SOC_PETRA_SEND_MESSAGE_TO_QUEUE_FUNC
    Soc_petra_send_message_to_queue = soc_sand_tcm_send_message_to_q_from_int;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*********************************************************************/
STATIC
 uint32
  soc_pa_interrupt_handler_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    tmp_bitstream[SIZE_OF_BITSTRAEM_IN_UINT32S] ;
  uint32
    *soc_sand_int = soc_sand_get_chip_descriptor_interrupt_bitstream(unit);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_INTERRUPT_HANDLER_UNSAFE);
  
  ret = SOC_SAND_OK; sal_memset(tmp_bitstream, 0x0, SIZE_OF_BITSTRAEM_IN_UINT32S * sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 3, exit);

  /*
   * mask interrupts
   * usually we would take a mutex before, but not in interrupt ...
   * anyway, it is not such a big deal - since we are in interrupt
   * context, "normal" tasks can not execute, and also not
   * ISR of the same level or below. So we're clear.
   */
  res = soc_petra_interrupt_mask_all_unsafe(
          unit,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit)

  /* read modify write all interrupt causes, into the chip descriptor's bit stream */
  res = soc_petra_interrupt_get_unsafe(
          unit,
          tmp_bitstream
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ret = soc_sand_bitstream_or(
          soc_sand_int,
          tmp_bitstream,
          SIZE_OF_BITSTRAEM_IN_UINT32S
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 33, exit);

  if (
      (soc_sand_bitstream_have_one_in_range(soc_sand_int, 0, MAX_INTERRUPT_CAUSE - 1)) &&
      (Soc_petra_send_message_to_queue != NULL)
     )
  {
    /* Send a message to the message queue. Callback engine will unmask the interrupts back */
    ret = Soc_petra_send_message_to_queue(
            unit
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 26, exit);
}
  else
  {
    res = soc_petra_interrupt_unmask_all_unsafe(
            unit,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 25, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pa_interrupt_handler_unsafe", 0, 0);
}

uint32
  soc_petra_interrupt_handler_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_HANDLER_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(interrupt_handler_unsafe,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_handler_unsafe()",0,0);
}

uint8
  soc_petra_is_multicast_id_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multicast_id
  )
{
  if (multicast_id < SOC_PETRA_MULT_NOF_MULTICAST_GROUPS)
  {
    return TRUE;
  }
  return FALSE;
}

uint8
  soc_petra_is_queue_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                queue
  )
{
/*
  uint32
    queue_max;
*/
  /*
  soc_petra_mgmt_nof_queues_get(unit, &queue_max);
  */
  if (queue < SOC_PETRA_NOF_QUEUES)
  {
    return TRUE;
  }
  return FALSE;
}

uint8
  soc_petra_is_flow_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                flow
  )
{
  uint8
    is_in_range;

  is_in_range = (flow < SOC_PETRA_NOF_FLOWS);

  return is_in_range;
}

uint32
  soc_petra_fbr2srd_lane_id(
    SOC_SAND_IN  uint32 fabric_link_id
  )
{
  uint32
    serdes_lane_id;

  if (fabric_link_id <= 27)
  {
    /* Fabric 0 - 27, SerDes 32-59 */
    serdes_lane_id = fabric_link_id + 32;
  }
  else if (fabric_link_id <= 31)
  {
    /* Fabric 28 - 31, SerDes 12-15*/
    serdes_lane_id = fabric_link_id - 16;
  }
  else if (fabric_link_id <= 35)
  {
    /* Fabric 32 - 35, SerDes 28-31 */
    serdes_lane_id = fabric_link_id - 4;
  }
  else
  {
    serdes_lane_id = SOC_PETRA_SRD_LANE_INVALID;
  }

  return serdes_lane_id;
}

uint32
  soc_petra_srd2fbr_link_id(
    SOC_SAND_IN  uint32 serdes_lane_id
  )
{
  uint32
    fabric_link_id;

  if (SOC_SAND_IS_VAL_IN_RANGE(serdes_lane_id, 32, 59))
  {
    /* Fabric 0 - 27, SerDes 32-59 */
    fabric_link_id = serdes_lane_id - 32;
  }
  else if (SOC_SAND_IS_VAL_IN_RANGE(serdes_lane_id, 12, 15))
  {
    /* Fabric 28 - 31, SerDes 12-15*/
    fabric_link_id = serdes_lane_id + 16;
  }
  else if (SOC_SAND_IS_VAL_IN_RANGE(serdes_lane_id, 28, 31))
  {
    /* Fabric 32 - 35, SerDes 28-31 */
    fabric_link_id = serdes_lane_id + 4;
  }
  else
  {
    fabric_link_id = SOC_PETRA_FBR_LINK_INVALID;
  }

  return fabric_link_id;
}


/*
 *  Internal Rate to clock conversion.
 *  Used for rate configuration, e.g. IPS (IssMaxCrRate),
 *  FMC (FmcMaxCrRate), Guaranteed/Best Effort FMC (GfmcMaxCrRate/BfmcMaxCrRate)
 */
uint32
  soc_petra_intern_rate2clock(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  rate_kbps,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *clk_interval
  )
{
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    cr_size = 0,
    ticks_per_sec = 0,
    interval = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERN_RATE2CLOCK);

  res = soc_petra_mgmt_credit_worth_get(
          unit,
          &cr_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  if (rate_kbps == 0)
  {
    *clk_interval = 0;
  }
  else
  {
    ret = soc_sand_kbits_per_sec_to_clocks(
            rate_kbps,
            cr_size,
            ticks_per_sec,
            &interval
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

    *clk_interval = interval * 8 - SOC_PETRA_FMC_RATE_DELTA_CONST(is_for_ips);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_intern_rate2clock()",0,0);
}

/*
 *  Internal Rate to clock conversion.
 *  Used for rate configuration, e.g. IPS (IssMaxCrRate),
 *  FMC (FmcMaxCrRate), Guaranteed/Best Effort FMC (GfmcMaxCrRate/BfmcMaxCrRate)
 */
uint32
  soc_petra_intern_clock2rate(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  clk_interval,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *rate_kbps
  )
{
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    cr_size = 0,
    ticks_per_sec = 0,
    interval = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERN_CLOCK2RATE);

  res = soc_petra_mgmt_credit_worth_get(
        unit,
        &cr_size
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  if (clk_interval == 0)
  {
    *rate_kbps = 0;
  }
  else
  {
    interval = (clk_interval + SOC_PETRA_FMC_RATE_DELTA_CONST(is_for_ips)) / 8;

    ret = soc_sand_clocks_to_kbits_per_sec(
            interval,
            cr_size,
            ticks_per_sec,
            rate_kbps
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_intern_clock2rate()",0,0);
}

#ifdef LINK_PB_LIBRARIES

void
  soc_petra_pb_api_not_implemented_err(
    SOC_SAND_IN  int              unit
  )
{
#if SOC_PETRA_DEBUG_IS_LVL2
  soc_sand_os_printf("This API is not implemented for Soc_petra-B yet\n\r");
#endif /* SOC_PETRA_DEBUG_IS_LVL2 */
}

#endif /* LINK_PB_LIBRARIES */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
