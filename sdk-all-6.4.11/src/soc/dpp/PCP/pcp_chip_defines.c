/* $Id: pcp_chip_defines.c,v 1.3 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/PCP/pcp_api_framework.h>
#include <soc/dpp/PCP/pcp_chip_defines.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_reg_access.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Number of ticks the PCP device clock ticks per second
 * (about a tick every 4.00 nano-seconds).
 */
#define PCP_DFLT_TICKS_PER_SEC               (250000000)

#define PCP_CHIP_DEFINITIONS_TICKS_PER_SEC (PCP_DFLT_TICKS_PER_SEC)

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
  pcp_revision_fld_get(
    SOC_SAND_IN  int unit
  )
{
  return 1;
}

/*
 *  {  Clock Parameters.
 */

/*********************************************************************
*  This procedure is used to convert from time values to machine
*  clocks value.
*********************************************************************/
uint32
  pcp_chip_time_to_ticks(
    SOC_SAND_IN  uint32        time_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_OUT uint32        *result
  )
{
  uint32
    mega_ticks_per_sec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_CHIP_TIME_TO_TICKS);
  SOC_SAND_CHECK_NULL_INPUT(result);

  mega_ticks_per_sec = pcp_chip_mega_ticks_per_sec_get();

  if (result_granularity == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DIV_BY_ZERO_ERR, 10, exit);
  }

  *result = ((time_value * mega_ticks_per_sec) / result_granularity);

  if (is_nano)
  {
    /*
      The time is given in nano seconds - hence divide by 1000.
     */
    *result /= 1000;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_chip_time_to_ticks()",0,0);
}

uint32
  pcp_ticks_to_time(
    SOC_SAND_IN  uint32        ticks_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_OUT uint32        *result
  )
{
  uint32
    mega_ticks_per_sec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_CHIP_TICKS_TO_TIME);
  SOC_SAND_CHECK_NULL_INPUT(result);

  mega_ticks_per_sec = pcp_chip_mega_ticks_per_sec_get();

  *result = (ticks_value * result_granularity) / mega_ticks_per_sec;

  if (is_nano)
  {
    /*
      The result is to be returned in nano seconds - hence multiply by 1000.
     */
    *result *= 1000;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_ticks_to_time()",0,0);
}

uint32
  pcp_chip_ticks_per_sec_get(
    void
  )
{
  return PCP_CHIP_DEFINITIONS_TICKS_PER_SEC;
}

uint32
  pcp_chip_kilo_ticks_per_sec_get(
    void
  )
{
  return SOC_SAND_DIV_ROUND_DOWN(PCP_CHIP_DEFINITIONS_TICKS_PER_SEC, 1000);
}

uint32
  pcp_chip_mega_ticks_per_sec_get(
    void
  )
{
  return SOC_SAND_DIV_ROUND_DOWN(PCP_CHIP_DEFINITIONS_TICKS_PER_SEC, 1000000);
}

 /*
  *  END Clock Parameters. }
  */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
