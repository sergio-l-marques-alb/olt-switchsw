/* $Id: chip_sim_pkt.c,v 1.6 Broadcom SDK $
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


#include "chip_sim.h"
#include "chip_sim_pkt.h"
#include "chip_sim_mem.h"
#include "chip_sim_log.h"


/*
 * accessing as apears in the spec. doc.
 */
UINT32 access_trig_address;

INT32 delay_milis ;
uint8  has_delayed_task[CHIP_SIM_NOF_CHIPS];
INT32 delayed_task_time_milis[CHIP_SIM_NOF_CHIPS];


void
  chip_sim_pkt_init(
    void
    )
{
  int
    i;
  /*
   * chip_mem.pkt
   */
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    has_delayed_task[i] = FALSE ;
    delayed_task_time_milis[i] = 0 ;
  }
  delay_milis = 0 ;
}

STATUS
  chip_sim_pkt_init_chip_specifics(
    SOC_SAND_IN UINT32 i_access_trig_address
    )
{
  access_trig_address = i_access_trig_address;
  return OK;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*     chip_sim_send_pkt_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:47
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
STATUS
    chip_sim_send_pkt_op(
      SOC_SAND_IN int chip_ndx
      )
{
  STATUS
    status = OK;
  UINT32
    access_triger = 0;

  /* 1 */
  if (chip_sim_mem_read(chip_ndx, access_trig_address, &access_triger))
  {
    char msg[160];
    sal_sprintf(msg,"chip_sim_send_pkt_op(): ERROR \r\n"
                "    chip_sim_mem_read(chip_ndx(%d), access_trig_address(0x%X)) failed.\r\n",
                 chip_ndx, access_trig_address);
    chip_sim_log_run(msg);
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT ;
  }

  /* 2 */
  if (access_triger != 0x1)
  {
    /* simulator error*/

    chip_sim_log_run("chip_sim_send_pkt_op(): ERROR \r\n"
                     "    'access_triger != 0x1' simulator error\r\n");
    GOTO_FUNC_EXIT_POINT ;
  }


  /* 3 */
  /*
   *
   */



FUNC_EXIT_POINT:
/* 4 */
  if (chip_sim_mem_write(chip_ndx, access_trig_address, 0x0)){
      chip_sim_log_run("chip_sim_send_pkt_op()- error: \r\n"
 	              "chip_sim_mem_write() failed.\r\n");
       status = ERROR;
  }

  return status ;
}
/*****************************************************
*NAME
*  chip_sim_mem_is_pkt_trig
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:15
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint8
  chip_sim_mem_is_pkt_trig(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  uint8
    answer = FALSE;

  if (chip_offset == access_trig_address)
  {
    answer = TRUE;
  }

  return answer ;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_mem_get_pkt_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:21
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
UINT32
  chip_sim_mem_get_pkt_delay(
    void
    )
{
  return delay_milis ;
}

/* Under Interrupt Halting No OS Calls */
/*set the delay task .*/
/*****************************************************
*NAME
*  chip_sim_mem_set_pkt_delay_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:25
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN chip_ndx

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
STATUS
  chip_sim_mem_set_pkt_delay_op(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK ;

  if (has_delayed_task[chip_ndx] == TRUE)
  {
    /* an error by the driver */
    status = ERROR ;
    goto chip_sim_mem_set_pkt_delay_op_exit ;
  }

  has_delayed_task[chip_ndx] = TRUE ;
  delayed_task_time_milis[chip_ndx] = delay_milis ;

chip_sim_mem_set_pkt_delay_op_exit:
  return status ;
}


