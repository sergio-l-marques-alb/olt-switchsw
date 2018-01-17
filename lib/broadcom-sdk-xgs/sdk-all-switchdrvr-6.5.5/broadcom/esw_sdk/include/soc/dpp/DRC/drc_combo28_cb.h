/*
 * $Id: drc_combo28_cb.h,v 1.1.2.4 Broadcom SDK $
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
 *
 * This file contains DPP DRC CallBack structure and routine declarations for the Dram operation using PHY Combo28.
 *
 */
#ifndef _SOC_DPP_DRC_COMBO28_CB_H
#define _SOC_DPP_DRC_COMBO28_CB_H

/* SOC DPP DRC includes */ 
#include <soc/dpp/DRC/drc_combo28.h>

/* 
 * CallBack Functions implementetion
 */
int soc_dpp_drc_combo28_shmoo_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data);
int soc_dpp_drc_combo28_shmoo_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data);
int soc_dpp_drc_combo28_shmoo_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask);
int soc_dpp_drc_combo28_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo28_bist_info_t info);
int soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo28_bist_err_cnt_t *info);
int soc_dpp_drc_combo28_shmoo_dram_init(int unit, int drc_ndx, int phase);
int soc_dpp_drc_combo28_shmoo_drc_pll_set(int unit, int drc_ndx, CONST combo28_drc_pll_t *pll_info);
int soc_dpp_drc_combo28_shmoo_modify_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 data, uint32 mask);
int soc_dpp_drc_combo28_shmoo_drc_enable_adt(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_drc_enable_write_leveling(int unit, int drc_ndx, uint32 command_parity_lattency, int use_continious_gddr5_dqs, int enable);
int soc_dpp_drc_combo28_shmoo_drc_mpr_en(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_mpr_load(int unit, int drc_ndx, uint8 *mpr_pattern);
int soc_dpp_drc_combo28_shmoo_drc_enable_gddr5_training_protocol(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_vendor_info_get(int unit, int drc_ndx, combo28_vendor_info_t *info);
int soc_dpp_drc_combo28_shmoo_drc_dqs_pulse_gen(int unit, int drc_ndx, int use_continious_gddr5_dqs);
int soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo28_gddr5_bist_info_t info);
int soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo28_gddr5_bist_err_cnt_t *info);
int soc_dpp_drc_combo28_gddr5_shmoo_drc_dq_byte_pairs_swap_info_get(int unit, int drc_ndx, int* pairs_were_swapped);
int soc_dpp_drc_combo28_shmoo_enable_wr_crc(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_rd_crc(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_wr_dbi(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_rd_dbi(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_refresh(int unit, int drc_ndx , int enable, uint32 new_trefi, uint32 * curr_refi);
int soc_dpp_drc_combo28_shmoo_force_dqs(int unit, int drc_ndx , uint32 force_dqs_val, uint32 force_dqs_oeb);
int soc_dpp_drc_combo28_soft_reset_drc_without_dram(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_dram_info_access(int unit, combo28_shmoo_dram_info_t** shmoo_info);
int soc_dpp_drc_combo28_shmoo_vendor_info_access(int unit, combo28_vendor_info_t** vendor_info);

/*
 * Utility functions 
 */
int soc_dpp_drc_combo28_shmoo_drc_precharge_all(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_drc_active_gddr5_cmd(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_write_mpr(int unit, int drc_ndx, uint32 intial_calib_mpr_addr, uint32 mpr_mode, uint32 mpr_page,uint32 mrs_readout,int enable_mpr);
int soc_dpp_drc_combo28_shmoo_load_mpr_pattern(int unit, int drc_ndx, uint32 mpr_location, uint32 mpr_pattern);
int soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_wait_dram_init_done(int unit, int drc_ndx);
#endif /* !_SOC_DPP_DRC_COMBO28_CB_H  */
