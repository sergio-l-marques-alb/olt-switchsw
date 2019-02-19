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
#include <appl/diag/dcmn/cmdlist.h> 

/*
 * If PEM_DIAG is set non-zero then CLI menu includes PEM operations.
 * Set to '0' on final version.
 */
#define PEM_DIAG 1

cmd_t bcm_dpp_cmd_list[] = {
 
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && defined(BCM_88650_A0)
    {"arad",      cmd_dpp_arad_card,           cmd_dpp_arad_card_usage, "Arad card specific commands" },
    {"ardon",      cmd_dpp_ardon_card,           cmd_dpp_ardon_card_usage, "Ardon card specific commands" },
#endif
#ifdef BCM_DPP_SUPPORT
#endif /* BCM_DPP_SUPPORT */
#if defined(BCM_ARAD_SUPPORT) && defined(BROADCOM_DEBUG) && defined(INCLUDE_REGRESS_ASIC)
    {"ASIC_INIT",    asic_init_cfg, cmd_dpp_asic_init_usage    , 
    "loads cints that are pertinent to asic regression" },
#endif
    {"Attach",      sh_attach,                sh_attach_usage,
     "Attach SOC device(s)" },
    {"AVS",        cmd_avs,      cmd_avs_usage,        "AVS - get AVS (Adjustable Voltage Scaling) value"  },
    {"CLEAR",           cmd_dpp_clear,          cmd_dpp_clear_usage,
        "Clear a memory table or counters" },
#ifdef BCM_DPP_SUPPORT
    {"COSQ",       cmd_dpp_cosq,        cmd_dpp_cosq_usage,
     "Set/Get cosq Parameters" },
#endif 
    {"CounTeR",     cmd_dpp_counter,          cmd_dpp_counter_usage,
     "Enable/disable counter collection"},
#ifdef BCM_DPP_SUPPORT
    {"CounTeRProc", cmd_dpp_ctr_proc,    cmd_dpp_ctr_proc_usage,
     "Counter processor diagnostics"},
#endif /* BCM_DPP_SUPPORT */
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
    {"cpu_i2c",     cmd_dpp_cpu_i2c,          cmd_dpp_cpu_i2c_usage, "Read/Write I2C via cpu" },
    {"cpu_regs",    cmd_dpp_cpu_regs,         cmd_dpp_cpu_regs_usage, "Read/Write function to cpu regs" },
#endif
#ifdef BCM_ARAD_SUPPORT
#if defined(BCM_DDR3_SUPPORT)
    {"DDRPhyCDR",       cmd_arad_ddr_phy_cdr,        cmd_arad_ddr_phy_cdr_usage,   "Control Combo28 DDR CDR"},
    {"DDRPhyRegs",      cmd_arad_ddr_phy_regs,       cmd_arad_ddr_phy_regs_usage,  "Read/Write DDR40 phy registers"},
    {"DDRPhyTune",      cmd_arad_ddr_phy_tune,       cmd_arad_ddr_phy_tune_usage,  "Tune DDR40 phy registers"},
#endif
#endif
#if defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)
    {"DEInit",          sh_deinit,              sh_deinit_usage, "Deinit SW modules" },
#ifdef BROADCOM_ALLOCATION_STATUS
    {"MEMUse",          sh_mem_usage,              sh_dpp_mem_usage, "Allocated memory" },
#endif
#endif

#ifdef BCM_DNX_P3_SUPPORT
    {"DELete",       cmd_dpp_mem_delete,        cmd_dpp_mem_delete_usage,
    "lookup entry according to key in exact match" },
#endif /* BCM_DNX_P3_SUPPORT */

    {"DETach",          sh_detach,              sh_detach_usage,        
      "Detach SOC device(s)" },
#if defined(BCM_DPP_SUPPORT)
    {"DEviceReset",          sh_device_reset,              sh_device_reset_usage, "Perform different device reset modes/actions." },      
#endif
    {"DIAG",        cmd_dpp_diag,             cmd_dpp_diag_usage,
     "Display diagnostic information" },
    {"DMA",         cmd_dpp_dma,              cmd_dpp_dma_usage,
     "DMA Facilities Interface" },
 
#if defined(BCM_ARAD_SUPPORT)
    {"DRAMBuf", cmd_arad_dram_buf, cmd_arad_dram_buf_usage, "Manage and get information on dram buffers"},
    {"DramMmuIndAccess", cmd_arad_dram_mmu_ind_access, cmd_arad_dram_mmu_ind_access_usage, "Perform MMU indirect reading and writing"},
#endif /* BCM_ARAD_SUPPORT */
    {"Dump",        cmd_dpp_dump,             cmd_dpp_dump_usage,
     "Dump an address space or registers" },
#if defined(INCLUDE_DUNE_UI) && defined (BCM_PETRA_SUPPORT)
    {"DUNE_ui",     cmd_dpp_ui,               cmd_dpp_ui_usage,
     "Open DUNE UI shell for DPP commands"},
#endif /* INCLUDE_DUNE_UI && BCM_PETRA_SUPPORT */
#ifdef BCM_DPP_SUPPORT
    {"export",      cmd_dpp_export,           cmd_dpp_export_usage_str, "Data Export commands" },
#endif /* BCM_DPP_SUPPORT */
    {"Fabric",      diag_dnx_fabric_diag_pack,           diag_dnx_fabric_diag_pack_usage_str,
     "DNX fabric diagnostic pack" },
#ifdef BCM_DPP_SUPPORT
    {"Fc",          cmd_dpp_fc,               cmd_dpp_fc_usage,
     "Show Flow-control status"},
#endif /* BCM_DPP_SUPPORT */ 
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && defined(BCM_DFE_SUPPORT)
    {"fe1600",      cmd_dpp_fe1600_card,           cmd_dpp_fe1600_card_usage, "Fe1600 card specific commands" },
    {"fe3200",      cmd_dpp_fe3200_card,           cmd_dpp_fe3200_card_usage, "Fe3200 card specific commands" },
#endif
    {"Getreg",      cmd_dpp_reg_get,          cmd_dpp_reg_get_usage,
     "Get register" },
    {"GPort",       cmd_dpp_gport,            cmd_dpp_gport_usage,
     "Show the current queue gports set up in the system"},
#ifdef BCM_DPP_SUPPORT
    {"Gtimer",      cmd_dpp_gtimer,           cmd_dpp_gtimer_usage,
     "Manage gtimer"},
#endif /* BCM_DPP_SUPPORT */  
#ifdef INCLUDE_I2C
    {"I2C",             cmd_i2c,            cmd_i2c_usage,              "Inter-Integrated Circuit (I2C) Bus commands"},
#endif  
    {"INIT",        sh_init,                  sh_init_usage,
     "Initialize SOC and S/W"},
#if defined(BCM_ARAD_SUPPORT) || defined(BCM_DFE_SUPPORT)
    {"init_dnx",            cmd_init_dnx,                appl_dcmn_init_usage,          "Initialize/deinitialize DNX S/W"},
#endif
#ifdef BCM_DNX_P3_SUPPORT
    {"INSert",       cmd_dpp_mem_insert,        cmd_dpp_mem_insert_usage,
    "insert entry to Exact-Match or Tcam" },
#endif /* BCM_DNX_P3_SUPPORT */
{"intr",        cmd_dpp_intr,             cmd_dpp_intr_usage,
     "Interrupt Controling" },
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && defined(BCM_88675_A0)
    {"jericho",      cmd_dpp_jericho_card,           cmd_dpp_jericho_card_usage,
     "Jericho card specific commands" },
#endif
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    {"KBP", cmd_dpp_kbp, cmd_dpp_kbp_usage, "Perform Access to KBP data"},
#endif   
#ifdef BCM_DPP_SUPPORT
    {"L2",          cmd_dpp_l2,               cmd_dpp_l2_usage,
     "Manage L2 (MAC) addresses"},
    {"L3",          cmd_dpp_l3,               cmd_dpp_l3_usage,
     "Manage L3 "},
#endif /* BCM_DPP_SUPPORT */
    {"LINKscan",    if_esw_linkscan,          if_esw_linkscan_usage,
     "Configure/Display link scanning" },
    {"LISTmem",     cmd_dpp_mem_list,         cmd_dpp_mem_list_usage,
     "List the entry format for a given table" },
    {"Listreg",     cmd_dpp_reg_list,         cmd_dpp_reg_list_usage,
     "List register fields"},

#ifdef BCM_DNX_P3_SUPPORT
    {"LKup",       cmd_dpp_mem_lkup,        cmd_dpp_mem_lkup_usage,
    "lookup entry according to key in exact match" },
#endif /* BCM_DNX_P3_SUPPORT */

#if defined(__DUNE_GTO_BCM_CPU__) && defined(BCM_DPP_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    {"LLm",     cmd_dpp_llm,         cmd_dpp_llm_usage,
     "LLM Application"},
#endif /* defined(__DUNE_GTO_BCM_CPU__) && defined(BCM_DPP_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
    {"MODify",      cmd_dpp_mem_modify,       cmd_dpp_mem_modify_usage,
     "Modify table entry by field names" },
    {"Modreg",      cmd_dpp_reg_modify,       cmd_dpp_reg_modify_usage,
     "Register Read/Modify/Write" },
#ifdef BCM_DPP_SUPPORT
#if defined(__DUNE_GTO_BCM_CPU__)
    {"negev",      cmd_dcmn_negev_chassis,           cmd_dcmn_negev_chassis_usage, "Negev chassis specific commands" },
#endif
#endif
    {"PacketWatcher",   pw_command,             pw_usage,                   "Monitor ports for packets"},
    {"PBMP",        cmd_dpp_pbmp,             cmd_dpp_pbmp_usage,
     "Convert port bitmap string to hex"},
    {"PCIC",        cmd_pcic_access,      cmd_pcic_access_usage,        "Access to PCI configuration space"  },
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
    {"pcie",        cmd_dpp_pcie_reg,         cmd_dpp_pcie_reg_usage,
     "Read/Write form devices via pcie" },
#endif

#if PEM_DIAG
    {"PEM",      cmd_dpp_pem /* cmd_dpp_reg_get */ ,          cmd_dpp_pem_usage /* cmd_dpp_reg_get_usage */,
     "PEM operations" },
#endif

    {"PHY",         if_dpp_phy,             if_dpp_phy_usage,
            "Set/Display phy characteristics"},
    {"PORT",        if_dpp_port,              if_dpp_port_usage,      "Set/Display port characteristics"},
#ifdef PORTMOD_DIAG
    {"PortMod", cmd_portmod_diag, portmod_diag_usage, "portmod diagnostics"},
#endif
    {"PortStat",    if_dpp_port_stat,         if_dpp_port_stat_usage,
     "Display port status in table"},
    {"PROBE",       sh_probe,                 sh_probe_usage,
     "Probe for available SOC units"},
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && defined(BCM_88375_A0)
    {"qmx",      cmd_dpp_qmx_card,           cmd_dpp_qmx_card_usage,
     "Qmx card specific commands" },
#endif
#ifdef  BCM_ARAD_SUPPORT
     {"REINIT",        sh_reinit,              sh_init_usage,
     "ReInitialize SOC and S/W"},
#endif
 /* don't use shr_res or shr_mres directly here 
#ifdef BCM_ESW_SUPPORT 
    {"ResTest",     cmd_sh_restest,           cmd_sh_restest_usage,
     "Tests for resource manager"},
#endif  
 */ 
    {"Setreg",      cmd_dpp_reg_set,          cmd_dpp_reg_set_usage,
     "Set register" },
    {"SHOW",        cmd_dpp_show,             cmd_dpp_show_usage,
     "Show information on a subsystem" },
    {"SOC",         cmd_dpp_soc,              cmd_dpp_soc_usage,
     "Print internal driver control information"},
#ifdef BCM_DPP_SUPPORT
    {"STG",         if_dpp_stg,               if_dpp_stg_usage,      
     "Manage spanning tree groups"},
#endif
    {"STKMode",     cmd_stkmode,              cmd_stkmode_usage,
     "Hardware Stacking Mode Control"},
    {"SwitchControl", cmd_dpp_switch_control, cmd_dpp_switch_control_usage,
     "General switch control"},
#ifdef BCM_DPP_SUPPORT
    {"Tdm",         cmd_dpp_tdm,              cmd_dpp_tdm_usage,
     "Manage tdm"},
#endif /* BCM_DPP_SUPPORT */
#ifdef INCLUDE_TEST
    {"TestClear",   test_clear,               test_clear_usage,
     "Clear run statisistics for a test"},
    {"TestList",    test_print_list,          test_list_usage,
     "List loaded tests and status"},
    {"TestMode",    test_mode,                test_mode_usage,
     "Set global test run modes"},
    {"TestParameters", test_parameters,       test_parameters_usage,
     "Set test Parameters"},
    {"TestRun",     test_run,                 test_run_usage,
     "Run a specific or selected tests"},
    {"TestSelect",  test_select,              test_select_usage,
     "Select tests for running"},
#endif
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && defined(BCM_88650_A0)
    {"TimeMeasurement",          cmd_dpp_time_measure,               cmd_dpp_time_measure_usage, "Time Measurement"},
#endif
#ifdef BCM_DPP_SUPPORT
    {"Tx",          cmd_dpp_tx,               cmd_dpp_tx_usage,
     "Transmit packets"},
    {"Vlan",        cmd_dpp_vlan,             cmd_dpp_vlan_usage,
     "Manage VLANs"},
#endif /* BCM_DPP_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
    {"WARMBOOT",        sh_warmboot,            sh_warmboot_usage,
    "Optionally boot warm"},
#endif /*BCM_WARM_BOOT_SUPPORT*/
    {"Write",       cmd_dpp_mem_write,        cmd_dpp_mem_write_usage,
     "Write entry(s) into a table" },
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    {"XXReload",        cmd_xxreload,           cmd_xxreload_usage,     "\"Easy\" Reload control" },
#endif /*defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)*/
};

int bcm_dpp_cmd_cnt = COUNTOF(bcm_dpp_cmd_list);



