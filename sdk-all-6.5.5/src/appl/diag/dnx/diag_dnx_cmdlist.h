/*
 * $Id: cmdlist.h,v 1.26 Broadcom SDK $
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
 * File:    cmdlist.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */

#ifndef DIAG_DNX_CMDLIST_H_INCLUDED
#define DIAG_DNX_CMDLIST_H_INCLUDED

#include <appl/diag/diag.h>

#define DCL_CMD(_f,_u)  \
    extern cmd_result_t _f(int, args_t *); \
    extern char     _u[];

extern int   bcm_dnx_cmd_cnt;
extern cmd_t bcm_dnx_cmd_list[];

DCL_CMD(cmd_init_dnx,      appl_dcmn_init_usage)

DCL_CMD(cmd_avs,      cmd_avs_usage)

DCL_CMD(cmd_dpp_clear, cmd_dpp_clear_usage)

#if defined(__DUNE_WRX_BCM_CPU__)
DCL_CMD(cmd_dpp_cpu_i2c, cmd_dpp_cpu_i2c_usage)
#endif

DCL_CMD(cmd_dpp_counter, cmd_dpp_counter_usage)

#if defined(__DUNE_WRX_BCM_CPU__)
DCL_CMD(cmd_dpp_cpu_regs, cmd_dpp_cpu_regs_usage)
#endif

DCL_CMD(cmd_dpp_ctr_proc, cmd_dpp_ctr_proc_usage)

DCL_CMD(cmd_dpp_dump, cmd_dpp_dump_usage)

DCL_CMD(cmd_dpp_ui, cmd_dpp_ui_usage)

DCL_CMD(cmd_dpp_gport, cmd_dpp_gport_usage)

DCL_CMD(cmd_dpp_reg_get, cmd_dpp_reg_get_usage)

DCL_CMD(cmd_dpp_gfa_bi, cmd_dpp_gfa_bi_usage)

DCL_CMD(cmd_dpp_l2, cmd_dpp_l2_usage)

DCL_CMD(cmd_dpp_l3, cmd_dpp_l3_usage)

DCL_CMD(cmd_dnx_mem_list, cmd_dnx_mem_list_usage)

DCL_CMD(cmd_dnx_reg_list, cmd_dnx_reg_list_usage)

DCL_CMD(cmd_dnx_mem_modify, cmd_dnx_mem_modify_usage)

DCL_CMD(cmd_dnx_reg_modify, cmd_dnx_reg_modify_usage)

DCL_CMD(cmd_dpp_pbmp, cmd_dpp_pbmp_usage)

#if defined(__DUNE_WRX_BCM_CPU__)
DCL_CMD(cmd_dpp_pcie_reg, cmd_dpp_pcie_reg_usage)
#endif

DCL_CMD(cmd_pcic_access,      cmd_pcic_access_usage)

DCL_CMD(if_dpp_phy, if_dpp_phy_usage)

DCL_CMD(if_dpp_port_stat, if_dpp_port_stat_usage)

DCL_CMD(if_dpp_port, if_dpp_port_usage)

DCL_CMD(cmd_dnx_reg_set, cmd_dnx_reg_set_usage)

DCL_CMD(cmd_dpp_show, cmd_dpp_show_usage)

DCL_CMD(cmd_dpp_intr, cmd_dpp_intr_usage)

DCL_CMD(cmd_dpp_soc, cmd_dpp_soc_usage)

DCL_CMD(cmd_dpp_switch_control, cmd_dpp_switch_control_usage)

#if defined(__DUNE_WRX_BCM_CPU__)
DCL_CMD(cmd_dpp_time_measure, cmd_dpp_time_measure_usage)
#endif

DCL_CMD(cmd_dpp_tx, cmd_dpp_tx_usage)

DCL_CMD(cmd_dpp_vlan, cmd_dpp_vlan_usage)

DCL_CMD(cmd_dnx_mem_write, cmd_dnx_mem_write_usage)

DCL_CMD(cmd_dpp_diag, cmd_dpp_diag_usage)

DCL_CMD(cmd_arad_dram_buf, cmd_arad_dram_buf_usage)
DCL_CMD(cmd_arad_dram_mmu_ind_access, cmd_arad_dram_mmu_ind_access_usage)
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
DCL_CMD(cmd_dpp_kbp, cmd_dpp_kbp_usage);
#endif 

DCL_CMD(cmd_dnx_dbal_list, cmd_dnx_dbal_usage);


DCL_CMD(cmd_dpp_dma, cmd_dpp_dma_usage)

#ifdef BCM_DDR3_SUPPORT
DCL_CMD(cmd_arad_ddr_phy_regs, cmd_arad_ddr_phy_regs_usage)
DCL_CMD(cmd_arad_ddr_phy_tune, cmd_arad_ddr_phy_tune_usage)
DCL_CMD(cmd_arad_ddr_phy_cdr, cmd_arad_ddr_phy_cdr_usage)
#endif

#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
DCL_CMD(cmd_xxreload,   cmd_xxreload_usage)
#endif /*defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)*/

DCL_CMD(cmd_dpp_tdm, cmd_dpp_tdm_usage)

DCL_CMD(cmd_dpp_gtimer, cmd_dpp_gtimer_usage)

DCL_CMD(cmd_dpp_cosq, cmd_dpp_cosq_usage)
DCL_CMD(if_dpp_stg, if_dpp_stg_usage)

DCL_CMD(cmd_dpp_fc, cmd_dpp_fc_usage)

DCL_CMD(diag_dnx_fabric_diag_pack, diag_dnx_fabric_diag_pack_usage_str)

DCL_CMD(cmd_dpp_export, cmd_dpp_export_usage_str)

#undef  DCL_CMD

#endif  /*  DIAG_DNX_CMDLIST_H_INCLUDED */
