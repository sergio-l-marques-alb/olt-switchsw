/* $Id: chip_sim_engine.c,v 1.3 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include "chip_sim.h"
#include "chip_sim_engine.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
#include "chip_sim_mem.h"
#include "chip_sim_cell.h"




/* { */
/* run over counters */
int
  Cntr_enable = FALSE;

uint8
 chip_sim_engine_get_cntr_enable(
   void
   )
{
  return Cntr_enable;
}

void
 chip_sim_engine_set_cntr_enable(
   SOC_SAND_IN uint8 i_cntr_enable
   )
{
  Cntr_enable = i_cntr_enable;
}
/* } */

/* { */
/* run over interrupts */
int
  Int_enable = FALSE;

uint8
 chip_sim_engine_get_int_enable(
   void
   )
{
  return Int_enable;
}

void
 chip_sim_engine_set_int_enable(
   SOC_SAND_IN uint8 i_int_enable
   )
{
  Int_enable = i_int_enable;
}
/* } */


/*****************************************************
*NAME
*   chip_sim_engine_run
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 18-Sep-02 16:44:08
*FUNCTION:
*  (1) disable interrupts
*  (2) go over all modules.
*  (3) enable interrupts.
*  (4) trigger interrupt - if needed.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) const int chip_ndx
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
STATUS
  chip_sim_engine_run(
    SOC_SAND_IN UINT32 time_diff
    )
{
  STATUS
    status = OK ;
  uint8
    gen_int = FALSE ;

  uint32
    interrupt_flag;

  /* 1 */
  soc_sand_os_stop_interrupts(&interrupt_flag);

  /* 2 */
  chip_sim_indirect_triger(time_diff);
  if (chip_sim_engine_get_cntr_enable())
  {
    chip_sim_counter_run(time_diff);
  }

  chip_sim_cell_tx(time_diff);
  chip_sim_cell_rx(time_diff);

  if (chip_sim_engine_get_int_enable())
  {
    chip_sim_interrupt_run(time_diff, &gen_int);
  }
  /* 3 */
  soc_sand_os_start_interrupts(interrupt_flag);

  /* 4 */
  if (gen_int)
  {
    chip_sim_interrupt_cpu_assert();
  }

  return status ;
}

