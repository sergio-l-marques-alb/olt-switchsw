/* $Id: chip_sim_cell.h,v 1.3 Broadcom SDK $
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
#ifndef _CHIP_SIM_CELL_H_
/* { */
#define _CHIP_SIM_CELL_H_

#include "chip_sim.h"

#include <soc/dpp/SAND/SAND_FM/sand_cell.h>

/*
 * Check if has send to cell (data or control).
 * Check if can send the cell (the relevant bit is down).
 * Check if has cell from the list of cell.
 *   case can, copy the cell to the registers and set the trap bit on.
 *    call to the interrupt service to enable the interrupt.
 */
STATUS chip_sim_cell_op(SOC_SAND_IN int chip_ndx);
STATUS chip_sim_cell_tx(SOC_SAND_IN UINT32 time_diff);
STATUS chip_sim_cell_rx(SOC_SAND_IN UINT32 time_diff);
STATUS chip_sim_cell_set_delay_op(SOC_SAND_IN int chip_ndx);
UINT32 chip_sim_cell_tx_get_delay(void);
void   chip_sim_cell_tx_set_delay(SOC_SAND_IN UINT32 delay_mili);
uint8 chip_sim_cell_is_trig_offset(SOC_SAND_IN UINT32 chip_offset);
SOC_SAND_RET chip_sim_cell_init_chip_specifics(SOC_SAND_IN int    i_nof_cell_blocks,
                                           SOC_SAND_IN UINT32 cell_access_trig_address);
SOC_SAND_RET chip_sim_cell_rx_init_chip_specifics(SOC_SAND_IN UINT32 chip_start_data_cell_offset_a,
                                              SOC_SAND_IN UINT32 chip_start_data_cell_offset_b,
                                              SOC_SAND_IN UINT32 data_int_address_a,
                                              SOC_SAND_IN UINT32 data_int_address_b,
                                              SOC_SAND_IN int    data_int_bit_a,
                                              SOC_SAND_IN int    data_int_bit_b,
                                              SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_a,
                                              SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_b,
                                              SOC_SAND_IN UINT32 ctrl_int_address_a,
                                              SOC_SAND_IN UINT32 ctrl_int_address_b,
                                              SOC_SAND_IN int    ctrl_int_bit_a,
                                              SOC_SAND_IN int    ctrl_int_bit_b
                                              );
STATUS chip_sim_cell_init(void);
STATUS chip_sim_cell_free();
STATUS chip_sim_cell_malloc();


STATUS chip_sim_cell_add_rx_data(
                                  SOC_SAND_IN int           chip_ndx,
                                  SOC_SAND_IN int           block_ndx,
                                  SOC_SAND_IN unsigned char *data_buff,
                                  SOC_SAND_IN int           data_buff_size,
                                  SOC_SAND_IN uint8          is_random,
                                  SOC_SAND_IN INT32         interval_mili,
                                  SOC_SAND_IN UINT32        cells_count
                                  );



/*
 * Check the necessary bit, if was written.
 *  Case yes: if was configured to loop-back, copy the cell to
 *    list of cells, case no loop-back, reset the bit (now, or remember to reset it later in time).
 * should be called on every write.
 */
STATUS
  chip_sim_read_cell();



/* } _CHIP_SIM_CELL_H_*/
#endif

