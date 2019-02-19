/* $Id: soc_pb_general.c,v 1.6 Broadcom SDK $
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
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>
#include <soc/dpp/Petra/PB_TM/pb_api_interrupt_service.h>
#include <soc/dpp/Petra/PB_TM/pb_api_callback.h>
#include <soc/dpp/Petra/PB_TM/pb_general.h>


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

typedef SOC_SAND_RET (*SOC_PB_SEND_MESSAGE_TO_QUEUE_FUNC)(SOC_SAND_IN  uint32 msg);

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
  SOC_PB_SEND_MESSAGE_TO_QUEUE_FUNC
    Soc_pb_send_message_to_queue = soc_sand_tcm_send_message_to_q_from_int;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_interrupt_handler_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERRUPT_HANDLER_UNSAFE);
     
  ret = SOC_SAND_OK; sal_memset(tmp_bitstream, 0x0, SIZE_OF_BITSTRAEM_IN_UINT32S * sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 3, exit);

  /*
   * mask interrupts
   * usually we would take a mutex before, but not in interrupt ...
   * anyway, it is not such a big deal - since we are in interrupt
   * context, "normal" tasks can not execute, and also not
   * ISR of the same level or below. So we're clear.
   */
  res = soc_pb_interrupt_mask_all_unsafe(
          unit,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit)
  
  /* read modify write all interrupt causes, into the chip descriptor's bit stream */
  res = soc_pb_interrupt_get_unsafe(
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
      (Soc_pb_send_message_to_queue != NULL)
     )
  {
    /* Send a message to the message queue. Callback engine will unmask the interrupts back */
    ret = Soc_pb_send_message_to_queue(
            unit
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 26, exit);
  }
  else
  {
    res = soc_pb_interrupt_unmask_all_unsafe(
            unit,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 25, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_interrupt_handler_unsafe", 0, 0);
}

uint32
  soc_pb_chip_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    ticks_per_sec = soc_pb_sw_db_core_freq_self_freq_get(unit) * 1000;

  return ticks_per_sec;
}

void
  soc_pb_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  clck_freq_khz
  )
{
   soc_pb_sw_db_core_freq_self_freq_set(unit, clck_freq_khz);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
