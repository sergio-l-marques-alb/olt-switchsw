#ifndef __DRV_INTERNAL_H__
#define __DRV_INTERNAL_H__
/* $Id: drv_internal.h,v 1.3 Broadcom SDK $
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
typedef struct {
  uint32 start;
  uint32 end;
}SOC_PB_MGMT_DBUFF_BOUNDARIES;

typedef struct {
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  mmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  uc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_fmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_mmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_uc;
} SOC_PB_INIT_DBUFFS_BDRY;


/*
 * Prototypes - internal functions from dune driver
 */
extern uint32
soc_pb_init_dram_nof_buffs_calc(uint32                   dram_size_total_mbyte,
                            SOC_PETRA_ITM_DBUFF_SIZE_BYTES dbuff_size,
                            SOC_PETRA_HW_QDR_PROTECT_TYPE  qdr_protection_mode,
                            SOC_PETRA_HW_QDR_SIZE_MBIT     qdr_total_size_mbit,
                            uint32                   *nof_dram_buffs);
extern uint32
soc_pb_init_dram_buff_boudaries_calc(uint32                     total_buffs,
                                 SOC_PETRA_ITM_DBUFF_SIZE_BYTES   dbuff_size,
                                 SOC_PB_INIT_DBUFFS_BDRY       *dbuffs);
extern uint32
soc_pb_mgmt_init_before_blocks_oor(int                    unit,
                               SOC_PETRA_ITM_DBUFF_SIZE_BYTES   dbuff_size,
                               SOC_PB_INIT_DBUFFS_BDRY         *dbuffs_bdries,
                               SOC_PB_HW_ADJUSTMENTS           *hw_adj);
extern uint32
soc_pb_mgmt_blocks_init_unsafe(int unit);

extern uint32
soc_pb_mgmt_init_after_blocks_oor(int                         unit, 
                              SOC_PETRA_ITM_DBUFF_SIZE_BYTES  dbuff_size, 
                              SOC_PB_INIT_DBUFFS_BDRY      *dbuffs_bdries);
extern uint32
soc_pb_mgmt_tbls_init(int unit, int silent);

extern uint32
soc_pb_pp_mgmt_tbls_init_unsafe(int unit, int silent);

extern uint32 
soc_pb_mgmt_hw_set_defaults(int unit);

extern uint32
soc_pb_mgmt_hw_adjust_ddr(int unit, SOC_PETRA_HW_ADJ_DDR *hw_adjust);

extern uint32
soc_pb_mgmt_functional_init(int unit, SOC_PB_HW_ADJUSTMENTS *hw_adjust, int sildent);

extern uint32
soc_pb_mgmt_hw_adjust_qdr(int unit, SOC_PB_HW_ADJ_QDR *hw_adjust);

extern uint32
soc_pb_mgmt_init_finalize(int unit);

#endif /* __DRV_INTERNAL_H__ */
