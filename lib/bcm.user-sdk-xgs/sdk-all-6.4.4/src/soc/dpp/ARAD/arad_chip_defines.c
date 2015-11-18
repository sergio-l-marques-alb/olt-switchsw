#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_chip_defines.c,v 1.8 Broadcom SDK $
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
 * $
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_CHIP
/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

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
  uint8
    Arad_chip_defines_init = FALSE;

static
  ARAD_CHIP_DEFINITIONS
    Arad_chip_definitions;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  arad_chip_defines_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SW_DB_INIT);

  if (Arad_chip_defines_init)
  {
    goto exit;
  }

  Arad_chip_definitions.ticks_per_sec = ARAD_DFLT_TICKS_PER_SEC;

  Arad_chip_defines_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in Arad_chip_defines_initialize()",0,0);
}



/*
 *  {  Clock Parameters.
 */

/*********************************************************************
*  This procedure is used to convert from time values to machine
*  clocks value.
*********************************************************************/
uint32
  arad_chip_time_to_ticks(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        time_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_IN  uint8       is_round_up,
    SOC_SAND_OUT uint32        *result
  )
{
  uint32
    granularity,
    ticks,
    kilo_ticks_per_sec,
    reminder;
  SOC_SAND_U64
    val1,
    val2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CHIP_TIME_TO_TICKS);
  SOC_SAND_CHECK_NULL_INPUT(result);

  kilo_ticks_per_sec = arad_chip_kilo_ticks_per_sec_get(unit);
  /*
   *    We use kilo-ticks-per-sec and not mega- for better accuracy.
   *  The granularity is multiplied by 1000 here to compensate.
   */
  granularity = (is_nano)?(result_granularity*1000000):(result_granularity*1000);

  if (result_granularity == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DIV_BY_ZERO_ERR, 10, exit);
  }

  soc_sand_u64_multiply_longs(time_value, kilo_ticks_per_sec, &val1);
  reminder = soc_sand_u64_devide_u64_long(&val1, granularity, &val2);

  if (val2.arr[1] != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 12, exit);
  }

  ticks = val2.arr[0];

  if ((is_round_up) && (reminder != 0))
  {
    ticks += 1;
  }

  *result = ticks;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_chip_time_to_ticks()",0,0);
}

/*********************************************************************
*  This procedure is used to convert from machine to
*  time values.
*********************************************************************/
uint32
  arad_ticks_to_time(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        ticks_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_OUT uint32        *result
  )
{
  uint32
    mega_ticks_per_sec;
  uint32
    granularity;
  SOC_SAND_U64
    val1,
    val2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CHIP_TICKS_TO_TIME);
  SOC_SAND_CHECK_NULL_INPUT(result);

  mega_ticks_per_sec = arad_chip_mega_ticks_per_sec_get(unit);

  granularity = (is_nano)?(result_granularity * 1000):result_granularity;

  soc_sand_u64_multiply_longs(ticks_value, granularity, &val1);
  soc_sand_u64_devide_u64_long(&val1, mega_ticks_per_sec, &val2);

  if (val2.arr[1] != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 12, exit);
  }

  *result =  val2.arr[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ticks_to_time()",0,0);
}

uint32
  arad_chip_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
    return Arad_chip_definitions.ticks_per_sec;
}

void
  arad_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  clck_freq_khz
  )
{
    Arad_chip_definitions.ticks_per_sec = clck_freq_khz * 1000;
    Arad_chip_defines_init = TRUE;
}

uint32
  arad_chip_kilo_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    tps;

  tps = arad_chip_ticks_per_sec_get(unit);
  return SOC_SAND_DIV_ROUND_DOWN(tps, 1000);
}

uint32
  arad_chip_mega_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    tps;

  tps = arad_chip_ticks_per_sec_get(unit);
  return SOC_SAND_DIV_ROUND_DOWN(tps, (1000*1000));
}

 /*
  *  END Clock Parameters. }
  */

/* } */


void
  arad_ARAD_REG_INFO_clear(
    SOC_SAND_OUT ARAD_REG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_REG_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */
