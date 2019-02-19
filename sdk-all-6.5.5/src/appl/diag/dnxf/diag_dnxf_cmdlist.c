/*
 * $Id: cmdlist.c,v 1.30 Broadcom SDK $
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
 * File:        cmdlist.c
 * Purpose:     List of commands available in DPP mode
 * Requires:
 */

#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include "diag_dnxf_cmdlist.h"

cmd_t bcm_dnxf_cmd_list[] = {

  {"Attach", sh_attach, sh_attach_usage, "Attach SOC device(s)"},
#if defined(BCM_PETRA_SUPPORT)
  {"AVS", cmd_avs, cmd_avs_usage, "AVS - get AVS (Adjustable Voltage Scaling) value"},
  {"CLEAR", cmd_dpp_clear, cmd_dpp_clear_usage, "Clear a memory table or counters"},
  {"CounTeR", cmd_dpp_counter, cmd_dpp_counter_usage, "Enable/disable counter collection"},
#if defined(__DUNE_WRX_BCM_CPU__)
  {"cpu_i2c", cmd_dpp_cpu_i2c, cmd_dpp_cpu_i2c_usage, "Read/Write I2C via cpu"},
  {"cpu_regs", cmd_dpp_cpu_regs, cmd_dpp_cpu_regs_usage, "Read/Write function to cpu regs"},
#endif
  {"DEInit", sh_deinit, sh_deinit_usage, "Deinit SW modules"},
#ifdef BROADCOM_ALLOCATION_STATUS
  {"MEMUse", sh_mem_usage, sh_dpp_mem_usage, "Allocated memory"},
#endif
  {"DETach", sh_detach, sh_detach_usage, "Detach SOC device(s)"},
  {"DIAG", cmd_dpp_diag, cmd_dpp_diag_usage, "Display diagnostic information"},
  {"DMA",  cmd_dpp_dma, cmd_dpp_dma_usage, "DMA Facilities Interface"},
  {"Dump", cmd_dpp_dump, cmd_dpp_dump_usage, "Dump an address space or registers"},
  {"Fabric", diag_dnx_fabric_diag_pack, diag_dnx_fabric_diag_pack_usage_str, "DNX fabric diagnostic pack"},
  {"Getreg", cmd_dpp_reg_get, cmd_dpp_reg_get_usage, "Get register"},
  {"GPort", cmd_dpp_gport, cmd_dpp_gport_usage, "Show the current queue gports set up in the system"},
#ifdef INCLUDE_I2C
  {"I2C", cmd_i2c, cmd_i2c_usage, "Inter-Integrated Circuit (I2C) Bus commands"},
#endif
  {"INIT", sh_init, sh_init_usage, "Initialize SOC and S/W"},
  {"init_dnx", cmd_init_dnx, appl_dcmn_init_usage, "Initialize/deinitialize DNX S/W"},
  {"intr", cmd_dpp_intr, cmd_dpp_intr_usage, "Interrupt Controling"},
  {"LINKscan", if_esw_linkscan, if_esw_linkscan_usage, "Configure/Display link scanning"},
  {"LISTmem", cmd_dpp_mem_list, cmd_dpp_mem_list_usage, "List the entry format for a given table"},
  {"Listreg", cmd_dpp_reg_list, cmd_dpp_reg_list_usage, "List register fields"},
  {"MODify", cmd_dpp_mem_modify, cmd_dpp_mem_modify_usage, "Modify table entry by field names"},
  {"Modreg", cmd_dpp_reg_modify, cmd_dpp_reg_modify_usage, "Register Read/Modify/Write"},
  {"PacketWatcher", pw_command, pw_usage, "Monitor ports for packets"},
  {"PBMP", cmd_dpp_pbmp, cmd_dpp_pbmp_usage, "Convert port bitmap string to hex"},
  {"PCIC", cmd_pcic_access, cmd_pcic_access_usage, "Access to PCI configuration space"},
#if defined(__DUNE_WRX_BCM_CPU__)
  {"pcie", cmd_dpp_pcie_reg, cmd_dpp_pcie_reg_usage, "Read/Write form devices via pcie"},
#endif
  {"PHY", if_dpp_phy, if_dpp_phy_usage, "Set/Display phy characteristics"},
  {"PORT", if_dpp_port, if_dpp_port_usage, "Set/Display port characteristics"},
#ifdef PORTMOD_DIAG
  {"PortMod", cmd_portmod_diag, portmod_diag_usage, "portmod diagnostics"},
#endif
  {"PortStat", if_dpp_port_stat, if_dpp_port_stat_usage, "Display port status in table"},
  {"PROBE", sh_probe, sh_probe_usage, "Probe for available SOC units"},
  {"Setreg", cmd_dpp_reg_set, cmd_dpp_reg_set_usage, "Set register"},
  {"SHOW", cmd_dpp_show, cmd_dpp_show_usage, "Show information on a subsystem"},
  {"SOC", cmd_dpp_soc, cmd_dpp_soc_usage, "Print internal driver control information"},
  {"STKMode", cmd_stkmode, cmd_stkmode_usage, "Hardware Stacking Mode Control"},
  {"SwitchControl", cmd_dpp_switch_control, cmd_dpp_switch_control_usage, "General switch control"},
#ifdef INCLUDE_TEST
  {"TestClear", test_clear, test_clear_usage,   "Clear run statisistics for a test"},
  {"TestList", test_print_list, test_list_usage, "List loaded tests and status"},
  {"TestMode", test_mode, test_mode_usage, "Set global test run modes"},
  {"TestParameters", test_parameters, test_parameters_usage, "Set test Parameters"},
  {"TestRun", test_run, test_run_usage, "Run a specific or selected tests"},
  {"TestSelect", test_select, test_select_usage, "Select tests for running"},
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
  {"WARMBOOT", sh_warmboot, sh_warmboot_usage, "Optionally boot warm"}
  ,
#endif /*BCM_WARM_BOOT_SUPPORT */
  {"Write", cmd_dpp_mem_write, cmd_dpp_mem_write_usage, "Write entry(s) into a table"},
#endif /* BCM_PETRA_SUPPORT */
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
  {"XXReload", cmd_xxreload, cmd_xxreload_usage, "\"Easy\" Reload control"}
  ,
#endif /*defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT) */
};

int bcm_dnxf_cmd_cnt = COUNTOF(bcm_dnxf_cmd_list);
