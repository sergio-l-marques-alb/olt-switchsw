/* $Id: petra_api_interrupt_service.c,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       $file_name$
*
* MODULE PREFIX:  $prefix$
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
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_interrupt_service.h>
#include <soc/dpp/Petra/petra_api_interrupt_service.h>

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

uint32
  soc_petra_interrupt_all_interrupts_and_indications_clear(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_CLEAR_ALL);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_interrupt_all_interrupts_and_indications_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_all_interrupts_and_indications_clear()",0,0);
}

uint32
  soc_petra_interrupt_mask_all(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint8        is_isr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_MASK_ALL);
  
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_interrupt_mask_all_unsafe(
          unit,
          is_isr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_mask_all()",0,0);
}

uint32
  soc_petra_interrupt_unmask_all(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8      is_tcm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_UNMASK_ALL);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_interrupt_unmask_all_unsafe(
          unit,
          is_tcm
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_unmask_all()",0,0);
}

uint32
  soc_petra_interrupt_mask_clear(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INTERRUPT_MASK_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_interrupt_mask_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_interrupt_mask_clear()",0,0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

void
  soc_petra_interrupt_all_interrupts_print(
    SOC_SAND_IN  int                        unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_interrupt_all_interrupts_print_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
exit:
  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "soc_petra_interrupt_all_interrupts_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
