/* $Id: petra_chip_defines.c,v 1.10 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_api_general.h>
  #include <soc/dpp/Petra/PB_TM/pb_general.h>
#endif
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_chip_defines.c,v 1.10 Broadcom SDK $
 * Number of ticks the SOC_PETRA device clock ticks per second
 * (about a tick every 4.00 nano-seconds). This value should
 * be changed if different on the specific board.
 */
#define SOC_PETRA_DFLT_TICKS_PER_SEC               (250000000)

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
static
  uint8
    Soc_petra_chip_defines_init = FALSE;

static
  SOC_PETRA_CHIP_DEFINITIONS
    Soc_petra_chip_definitions;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_petra_chip_defines_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  if (Soc_petra_chip_defines_init)
  {
    goto exit;
  }

  Soc_petra_chip_definitions.ticks_per_sec = SOC_PETRA_DFLT_TICKS_PER_SEC;

  Soc_petra_chip_defines_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in Soc_petra_chip_defines_initialize()",0,0);
}

STATIC uint32
  soc_petra_revision_fld_get_lcl(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32 *fld_val
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(fld_val);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->eci.revision_reg.revision, *fld_val, 0, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_revision_fld_get_lcl()",0,0);
}

uint32
  soc_petra_revision_fld_get(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res,
    fld_val = 0;

  res = soc_petra_revision_fld_get_lcl(
          unit,
          &fld_val
        );
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) {
    return 0xffffffff;
  }
  return fld_val;
}

/*
 *  {  Clock Parameters.
 */

/*********************************************************************
*  This procedure is used to convert from time values to machine
*  clocks value.
*********************************************************************/
uint32
  soc_petra_chip_time_to_ticks(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CHIP_TIME_TO_TICKS);
  SOC_SAND_CHECK_NULL_INPUT(result);

  kilo_ticks_per_sec = soc_petra_chip_kilo_ticks_per_sec_get(unit);
  /*
   *	We use kilo-ticks-per-sec and not mega- for better accuracy.
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_chip_time_to_ticks()",0,0);
}

/*********************************************************************
*  This procedure is used to convert from machine to
*  time values.
*********************************************************************/
uint32
  soc_petra_ticks_to_time(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CHIP_TICKS_TO_TIME);
  SOC_SAND_CHECK_NULL_INPUT(result);

  mega_ticks_per_sec = soc_petra_chip_mega_ticks_per_sec_get(unit);

  granularity = (is_nano)?(result_granularity * 1000):result_granularity;

  soc_sand_u64_multiply_longs(ticks_value, granularity, &val1);
  soc_sand_u64_devide_u64_long(&val1, mega_ticks_per_sec, &val2);

  if (val2.arr[1] != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 12, exit);
  }

  *result =  val2.arr[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ticks_to_time()",0,0);
}

STATIC uint32
  soc_pa_chip_ticks_per_sec_get(void)
{
  return Soc_petra_chip_definitions.ticks_per_sec;
}

uint32
  soc_petra_chip_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  switch (soc_petra_sw_db_ver_get(unit))
  {
  case SOC_PETRA_DEV_VER_A:
    return soc_pa_chip_ticks_per_sec_get();
    break;
#ifdef LINK_PB_LIBRARIES
  case SOC_PETRA_DEV_VER_B:
    return soc_pb_chip_ticks_per_sec_get(unit);
    break;
#endif
  default:
    return soc_pa_chip_ticks_per_sec_get();
    break;
  }
}

STATIC void
  soc_pa_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN uint32 clck_freq_khz
  )
{
   Soc_petra_chip_definitions.ticks_per_sec = clck_freq_khz * 1000;
   Soc_petra_chip_defines_init = TRUE;
}

void
  soc_petra_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  clck_freq_khz
  )
{
  switch (soc_petra_sw_db_ver_get(unit))
  {
  case SOC_PETRA_DEV_VER_A:
    soc_pa_chip_kilo_ticks_per_sec_set(clck_freq_khz);
    break;
#ifdef LINK_PB_LIBRARIES
  case SOC_PETRA_DEV_VER_B:
    soc_pb_chip_kilo_ticks_per_sec_set(unit, clck_freq_khz);
    break;
#endif
  default:
    soc_pa_chip_kilo_ticks_per_sec_set(clck_freq_khz);
    break;
  }
}

uint32
  soc_petra_chip_kilo_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    tps;

  tps = soc_petra_chip_ticks_per_sec_get(unit);
  return SOC_SAND_DIV_ROUND_DOWN(tps, 1000);
}

uint32
  soc_petra_chip_mega_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    tps;

  tps = soc_petra_chip_ticks_per_sec_get(unit);
  return SOC_SAND_DIV_ROUND_DOWN(tps, (1000*1000));
}

 /*
  *  END Clock Parameters. }
  */

/* } */


void
  soc_petra_PETRA_REG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_REG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_REG_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*
 *  Get Soc_petra device revision
 */
SOC_PETRA_REV
  soc_pa_revision_get(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    rev_val;
  SOC_PETRA_REV
    rev;

  rev_val = soc_petra_revision_fld_get(unit);

  switch(rev_val) {
  case SOC_PETRA_REVISION_FLD_VAL_A0:
    rev = SOC_PETRA_REV_A0;
    break;
  case SOC_PETRA_REVISION_FLD_VAL_A1:
    rev = SOC_PETRA_REV_A1;
    break;
  case SOC_PETRA_REVISION_FLD_VAL_A2:
    rev = SOC_PETRA_REV_A2;
    break;
  case SOC_PETRA_REVISION_FLD_VAL_A3:
    rev = SOC_PETRA_REV_A3;
    break;
  case SOC_PETRA_REVISION_FLD_VAL_A4:
    rev = SOC_PETRA_REV_A4;
    break;
  default:
    rev = SOC_PETRA_NOF_REVS;
  }

  return rev;
}

SOC_PETRA_REV
  soc_petra_revision_get(
    SOC_SAND_IN  int  unit
  )
{
  SOC_PETRA_REV
    rev;

  switch (soc_petra_sw_db_ver_get(unit))
  {
  case SOC_PETRA_DEV_VER_A:
    rev = soc_pa_revision_get(unit);
    break;
  case SOC_PETRA_DEV_VER_B:
#ifdef LINK_PB_LIBRARIES
    rev = soc_pb_revision_get(unit);
#else
    rev = SOC_SAND_INTERN_VAL_INVALID_32;
#endif
    break;
  default:
    rev = soc_pa_revision_get(unit);
    break;
  }

  return rev;
}

const char*
  soc_petra_PETRA_REV_to_string(
    SOC_SAND_IN SOC_PETRA_REV enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_REV_A0:
    str = "A0";
  break;

  case SOC_PETRA_REV_A1:
    str = "A1";
  break;

  case SOC_PETRA_REV_A2:
    str = "A2";
  break;

  case SOC_PETRA_REV_A3:
    str = "A3";
  break;

  case SOC_PETRA_REV_A4:
    str = "A4";
  break;

  case SOC_PB_REV_A0:
    str = "B-A0";
    break;

  case SOC_PB_REV_A1:
    str = "B-A1";
    break;

  case SOC_PB_REV_B0:
    str = "B-B0";
    break;

  case SOC_PETRA_NOF_REVS:
    str = "Unknown";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


#if SOC_PETRA_DEBUG
void
  soc_petra_PETRA_REG_INFO_print(
    SOC_SAND_IN SOC_PETRA_REG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Address base:          0x%.8x\n\r",info->addr.base);
  soc_sand_os_printf(" Address step:          0x%.4x\n\r",info->addr.step);
  soc_sand_os_printf(" Value:                 0x%.8x\n\r",(uint32)info->val);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>
