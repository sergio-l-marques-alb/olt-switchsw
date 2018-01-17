/* $Id: chip_sim_command.h,v 1.3 Broadcom SDK $
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
#ifndef _CHIP_SIM_COMMAND_H_
/* { */
#define _CHIP_SIM_COMMAND_H_

#include "chip_sim.h"


STATUS chip_sim_command_indirect_delay(SOC_SAND_IN UINT32 delay,
                                       SOC_SAND_OUT char  msg[]) ;
STATUS chip_sim_command_wake_up_task(SOC_SAND_OUT char  msg[]) ;
STATUS chip_sim_command_set_task_sleep_time(SOC_SAND_IN  UINT32 mili_sec,
                                            SOC_SAND_OUT char   msg[]) ;
STATUS chip_sim_command_interrupt_assert(SOC_SAND_OUT char  msg[]) ;
STATUS chip_sim_command_mask_all_int(SOC_SAND_IN  uint8  active,
                                     SOC_SAND_OUT char  msg[]
                                     ) ;
STATUS chip_sim_command_int(SOC_SAND_IN int    chip_ndx,
                            SOC_SAND_IN UINT32 int_chip_offset,
                            SOC_SAND_IN UINT32 int_bit,
                            SOC_SAND_IN uint8   is_random,
                            SOC_SAND_IN INT32  interval_milisec,
                            SOC_SAND_IN UINT32 count,
                            SOC_SAND_OUT char  msg[]
                            ) ;
STATUS
  chip_sim_command_cell_tx_delay(
    SOC_SAND_IN UINT32 delay,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_data_cell_rx(
    SOC_SAND_IN int           chip_ndx,
    SOC_SAND_IN int           block_ndx,
    SOC_SAND_IN unsigned char *data_buff,
    SOC_SAND_IN int           data_buff_size,
    SOC_SAND_IN uint8          is_random,
    SOC_SAND_IN INT32         interval_mili,
    SOC_SAND_IN UINT32        cells_count,
    SOC_SAND_OUT char         msg[]
    );

STATUS
  chip_sim_command_counter(
    SOC_SAND_IN  uint8  is_random,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_en_counter(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_en_int(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_get_time_monitor(
    SOC_SAND_OUT char   msg[]
    );

STATUS
  chip_sim_command_interrupt_assert(
    SOC_SAND_OUT char  msg[]
    );
/* } _CHIP_SIM_COMMAND_H_*/
#endif

