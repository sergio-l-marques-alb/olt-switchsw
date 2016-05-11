/* $Id: chip_sim_mem.h,v 1.3 Broadcom SDK $
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

#ifndef _CHIP_SIM_MEM_H_
/* { */
#define _CHIP_SIM_MEM_H_

#include <appl/diag/dpp/dune_chips.h>

#include "chip_sim_counter.h"

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

void chip_sim_mem_init(void);
STATUS chip_sim_mem_malloc(
  SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type,
  SOC_SAND_IN unsigned int     chip_ver,
  SOC_SAND_IN DESC_FE_REG*     desc,
  SOC_SAND_IN UINT32           start_address[CHIP_SIM_NOF_CHIPS]);
STATUS chip_sim_mem_free(void);
STATUS chip_sim_mem_address_to_offset(SOC_SAND_IN  UINT32  address,
                                      SOC_SAND_OUT UINT32* chip_offset,
                                      SOC_SAND_OUT int*    chip_ndx);
STATUS chip_sim_mem_check_is_write_access(SOC_SAND_IN UINT32 chip_offset);
STATUS
  chip_sim_mem_reset(SOC_SAND_IN int chip_ndx);
void chip_sim_mem_init(void);


STATUS chip_sim_indirect_set_delay_time(SOC_SAND_IN UINT32 delay);

STATUS chip_sim_mem_write(SOC_SAND_IN int    chip_ndx,
                          SOC_SAND_IN UINT32 chip_offset,
                          SOC_SAND_IN UINT32 value);
STATUS chip_sim_mem_read( SOC_SAND_IN  int    chip_ndx,
                          SOC_SAND_IN  UINT32 chip_offset,
                          SOC_SAND_OUT UINT32 *value);
STATUS chip_sim_mem_write_field(SOC_SAND_IN int    chip_ndx,
                                SOC_SAND_IN UINT32 chip_offset,
                                SOC_SAND_IN UINT32 value,
                                SOC_SAND_IN UINT32 shift,
                                SOC_SAND_IN UINT32 mask);
STATUS chip_sim_mem_read_field(SOC_SAND_IN  int    chip_ndx,
                               SOC_SAND_IN  UINT32 chip_offset,
                               SOC_SAND_OUT UINT32 *value,
                               SOC_SAND_IN  UINT32 shift,
                               SOC_SAND_IN  UINT32 mask);
STATUS chip_sim_mem_check_is_indirect_access(SOC_SAND_IN UINT32 chip_offset);
STATUS chip_sim_mem_check_is_write_access(SOC_SAND_IN UINT32 chip_offset);
STATUS chip_sim_mem_check_is_read_access(SOC_SAND_IN UINT32 chip_offset);
STATUS chip_sim_mem_check_is_clear_access(SOC_SAND_IN UINT32 chip_offset);


/* } _CHIP_SIM_MEM_H_*/
#endif

