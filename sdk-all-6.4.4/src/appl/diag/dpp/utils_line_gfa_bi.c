/* $Id: utils_line_gfa_bi.c,v 1.11 Broadcom SDK $
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
#ifdef _MSC_VER                                                     
  /*                                                                
   * On MS-Windows platform this attribute is not defined.          
   */                                                               
  #define __ATTRIBUTE_PACKED__                                      
  #pragma pack(push)                                                
  #pragma pack(1)                                                   
                                                                    
#elif __GNUC__                                                      
  /*                                                              
   * GNUC packing attribute                                       
   */                                                             
  #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))          
#else                                                               
  #error  "Add your system support for packed attribute."       
#endif                                                              
                                                                    
/*************                                                      
 * INCLUDES  *                                                      
 *************/                                                     
/* { */
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#ifndef __DUNE_GTO_BCM_CPU__
#include <appl/diag/dpp/utils_host_board.h>
#include <appl/diag/dpp/utils_i2c_mem.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>
#endif

#include <soc/drv.h>
#include <soc/i2c.h>
#include <soc/dcmn/utils_fpga.h>

#include <Petra/petra_api_general.h>
#include <Petra/petra_api_mgmt.h>

#include <appl/diag/dpp/utils_error_defs.h>
#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_line_gfa_petra.h>
#include <appl/diag/dpp/utils_line_gfa_bi.h>
#include <appl/diag/dcmn/utils_board_general.h>
#include <appl/diag/dpp/dune_chips.h>

#ifndef __DUNE_GTO_BCM_CPU__
#include <appl/diag/dpp/utils_line_PTG.h>
#include <appl/diag/dpp/dffs_driver.h>
#endif

#ifndef DUNE_BCM

#ifdef __VXWORKS__

#include "vxWorks.h"
#include "intLib.h"
#include "arch/ppc/vxPpcLib.h"
#include "vxWorks.h" 
#include "../Config/MdpPro8xxT_01/config.h"
#include "../Config/MdpPro8xxT_01/flashDev.h"
#include "../Config/MdpPro8xxT_01/flash28.h"

#endif

#include "Pub/include/utils_defx.h"

#include "../../../../h/usrApp.h" 

#include "Utilities/include/utils_defi.h"
#include "Utilities/include/utils_dffs_cpu_mez.h"
#include "Utilities/include/utils_line_gfa_petra.h"

#include "Utilities/include/utils_error_defs.h"
#include "Utilities/include/utils_dune_fpga_download.h"
#include "Utilities/include/utils_serdes_defs.h"
#include "Utilities/include/utils_line_bringup.h"
#include "Utilities/include/utils_host_board.h"
#include "Utilities/include/utils_ip_mgmt.h"
#include "Utilities/include/utils_front_end_host_card.h"
#include "Utilities/include/utils_line_gfa_petra.h"
#include "Utilities/include/utils_line_PTG.h"
#include "Utilities/include/utils_i2c_mem.h"

#include "UserInterface/include/ui_pure_defi.h"  
#include "UserInterface/include/ui_cli_files.h"

#include "DuneDriver/SAND/SAND_FM/include/sand_chip_defines.h"

#include "DuneDriver/SAND/Utils/include/sand_os_interface.h"
#include "DuneDriver/SAND/Utils/include/sand_framework.h"

#include "DuneDriver/Petra/include/petra_api_general.h"
#include "DuneDriver/Petra/include/petra_api_nif.h"

#include "Bsp_drv/include/bsp_cards_consts.h"

#include "Background/include/bckg_defi.h"

#else

#if !(defined(LINUX) || defined(UNIX))

#include "vxWorks.h"
#include "intLib.h"
#include "arch/ppc/vxPpcLib.h"

#else
typedef volatile unsigned int VUINT32;
#endif

#endif /* !DUNE_BCM */
/* } */                                                             
                                                                    
/*************                                                      
 * DEFINES   *                                                      
 *************/                                                     
/* { */       

/* } */                                                             
                                                                    
/*************                                                      
 *  MACROS   *                                                      
 *************/                                                     
/* { */                                                             
    
/* } */                                                             
                                                                    
/*************                                                      
 * TYPE DEFS *                                                      
 *************/                                                     
/* { */        

typedef struct
{
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ version;                        /* 0x00 - same as gfa */ 
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ scratch_test_register;          /* 0x04 - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR qdr_clk_mux_sel_0;
    GFA_PETRA_FLD_ADDR com_clk_mux_sel_0;
    GFA_PETRA_FLD_ADDR com_clk_mux_sel_1;
    GFA_PETRA_FLD_ADDR nif_clk_mux_sel_0;
    GFA_PETRA_FLD_ADDR nif_clk_mux_sel_1;
    GFA_PETRA_FLD_ADDR port_1_phy_reset;
    GFA_PETRA_FLD_ADDR port_2_phy_reset;
    GFA_PETRA_FLD_ADDR port_3_phy_reset;
  } __ATTRIBUTE_PACKED__ misc_2;                         /* 0x08 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR qsfp_p1_reset_n;
    GFA_PETRA_FLD_ADDR qsfp_p2_reset_n;
    GFA_PETRA_FLD_ADDR qsfp_p3_reset_n;
    GFA_PETRA_FLD_ADDR qsfp_p4_reset_n;
    GFA_PETRA_FLD_ADDR qsfp_p5_reset_n;
    GFA_PETRA_FLD_ADDR qsfp_p6_reset_n;
    GFA_PETRA_FLD_ADDR i2c_read_mux;
  } __ATTRIBUTE_PACKED__ misc_3;                         /* 0x0c */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR fc_a_dir;
    GFA_PETRA_FLD_ADDR fc_b_dir;
    GFA_PETRA_FLD_ADDR fc_a_oe_n;
    GFA_PETRA_FLD_ADDR fc_b_oe_n;
  } __ATTRIBUTE_PACKED__ misc_4;                         /* 0x10 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR pcp_qdr_doff_n;
    GFA_PETRA_FLD_ADDR pcp_pll_reset;
    GFA_PETRA_FLD_ADDR pcp_hw_reset;
    GFA_PETRA_FLD_ADDR soc_petra_core_pll_bypass;
    GFA_PETRA_FLD_ADDR soc_petra_ddr_pll_bypass;
    GFA_PETRA_FLD_ADDR soc_petra_qdr_pll_bypass;
    GFA_PETRA_FLD_ADDR soc_petra_pll_reset;
    GFA_PETRA_FLD_ADDR soc_petra_hardware_reset;
  } __ATTRIBUTE_PACKED__ soc_petra_miscelanous_controls_1;   /* 0x14 - partial same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ gpio;                           /* 0x18 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ interrupt;                      /* 0x1c */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR controller_ready_1_8_v;
    GFA_PETRA_FLD_ADDR controller_ready_1_5_v;
    GFA_PETRA_FLD_ADDR controller_ready_1_0_v;
  } __ATTRIBUTE_PACKED__ power_supply_status;            /* 0x20 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR controller_start_1_8_v;
    GFA_PETRA_FLD_ADDR controller_start_1_5_v;
    GFA_PETRA_FLD_ADDR controller_start_1_0_v;
  } __ATTRIBUTE_PACKED__ power_supply_control;           /* 0x24 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR qsfp_i2c_mux_reset;
    GFA_PETRA_FLD_ADDR clockes_i2c_mux_reset;
    GFA_PETRA_FLD_ADDR power_i2c_mux_reset;
  } __ATTRIBUTE_PACKED__ i2c_mux_select;                 /* 0x28 - partial same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_mux_config;                 /* 0x2c - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_address_command;            /* 0x30 - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_data_tx_rx;                 /* 0x34 - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_status;                     /* 0x38 - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_triggers;                   /* 0x3c - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR led_0;
    GFA_PETRA_FLD_ADDR led_1;
    GFA_PETRA_FLD_ADDR led_2;
    GFA_PETRA_FLD_ADDR fe_led_0;
    GFA_PETRA_FLD_ADDR fe_led_1;
  } __ATTRIBUTE_PACKED__ debug_leds;                     /* 0x40 */
 
  struct
  {
  GFA_PETRA_FLD_ADDR addr;
  GFA_PETRA_FLD_ADDR combo_links_of_nif_a_path_select;
  GFA_PETRA_FLD_ADDR combo_links_of_nif_b_path_select;
  } __ATTRIBUTE_PACKED__ combo_switches_controls;        /* 0x44 - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ qsfp_module_present;            /* 0x48 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ core_synt_freq;                 /* 0x50 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ qdr_synt_freq;                  /* 0x54 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ ddr_synt_freq;                  /* 0x58 - same as gfa */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ fabric_synt_freq;               /* 0x5c */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ nif_synt_freq;                  /* 0x60 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ combo_synt_freq;                /* 0x64 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR power_supply_control;
    GFA_PETRA_FLD_ADDR soc_petra_efuse_and_test_mode;
    GFA_PETRA_FLD_ADDR broadcom_cpu_exist;
  } __ATTRIBUTE_PACKED__ protected_register_write_control; /* 0x68 */


  /* Gfa-Bi 2 registers */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ interrupt_1;                         /* 0x1c  - changed register */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ interrupt_2;                         /* 0x6c - new register */
                                                                                  /* 0x28 - changed register */
                                                                                    /* 0x44 - removed register */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR qphy1_port_reset_n0;
    GFA_PETRA_FLD_ADDR qphy1_port_reset_n1;
    GFA_PETRA_FLD_ADDR qphy1_port_reset_n2;
    GFA_PETRA_FLD_ADDR qphy1_port_reset_n3;
    GFA_PETRA_FLD_ADDR qphy2_port_reset_n0;
    GFA_PETRA_FLD_ADDR qphy2_port_reset_n1;
    GFA_PETRA_FLD_ADDR qphy2_port_reset_n2;
    GFA_PETRA_FLD_ADDR qphy2_port_reset_n3;
  } __ATTRIBUTE_PACKED__ qphy_port_reset_1;            /* 0x48 - changed register */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR qphy3_port_reset_n0;
    GFA_PETRA_FLD_ADDR qphy3_port_reset_n1;
    GFA_PETRA_FLD_ADDR qphy3_port_reset_n2;
    GFA_PETRA_FLD_ADDR qphy3_port_reset_n3;
  } __ATTRIBUTE_PACKED__ qphy_port_reset_2;            /* 0x70 - changed register */           

} __ATTRIBUTE_PACKED__ GFA_BI_FPGA_IO_REGS;                                                     
      

/* } */                                                             
                                                                    
/*************                                                      
 * GLOBALS   *                                                      
 *************/                                                     
/* { */       

static GFA_BI_FPGA_IO_REGS
  Gfa_bi_fpga_io_regs;

static uint8
  Gfa_bi_fpga_io_regs_init;                                                      
    
/* } */                                                             
                                                                    
/*************                                                      
 * FUNCTIONS *                                                      
 *************/                                                     
/* { */      

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(__DUNE_GTO_BCM_CPU__)
extern int _cpu_write(int d, uint32 addr, uint32 *buf);
extern int _cpu_read(int d, uint32 addr, uint32 *buf);
#endif

/* general functions */

uint8
  gfa_bi_is_line_card_connected(void
  )
{
  static uint8
     is_connected = FALSE;
  SOC_BSP_CARDS_DEFINES
    card_type;

  host_board_type_from_nv(&card_type);
#if DEBUG_GFA_BI_BOARD
  soc_sand_os_printf("gfa_bi_is_line_card_connected(). card_type=%d.\n",card_type);
#endif

  if ((card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) || (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2))
  {
    is_connected = TRUE;
  }

#if DEBUG_GFA_BI_BOARD 
  soc_sand_os_printf("gfa_bi_is_line_card_connected(). is_connected=%d.\n",is_connected);
#endif
  return is_connected;
  
}


/* init host board functions*/

SOC_SAND_RET
  gfa_bi_fpga_io_regs_init(void)
{
  if (!Gfa_bi_fpga_io_regs_init)
  {
    Gfa_bi_fpga_io_regs.version.addr.address = 0x00;                                              /* same as in gfa */
    Gfa_bi_fpga_io_regs.version.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.version.addr.msb = 7;

    Gfa_bi_fpga_io_regs.scratch_test_register.addr.address = 0x04;                                /* same as in gfa */
    Gfa_bi_fpga_io_regs.scratch_test_register.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.scratch_test_register.addr.msb = 7;

    Gfa_bi_fpga_io_regs.misc_2.addr.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.misc_2.addr.msb = 7;
    Gfa_bi_fpga_io_regs.misc_2.qdr_clk_mux_sel_0.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.qdr_clk_mux_sel_0.lsb = 0;
    Gfa_bi_fpga_io_regs.misc_2.qdr_clk_mux_sel_0.msb = 0;
    Gfa_bi_fpga_io_regs.misc_2.com_clk_mux_sel_0.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.com_clk_mux_sel_0.lsb = 1;
    Gfa_bi_fpga_io_regs.misc_2.com_clk_mux_sel_0.msb = 1;
    Gfa_bi_fpga_io_regs.misc_2.com_clk_mux_sel_1.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.com_clk_mux_sel_1.lsb = 2;
    Gfa_bi_fpga_io_regs.misc_2.com_clk_mux_sel_1.msb = 2;
    Gfa_bi_fpga_io_regs.misc_2.nif_clk_mux_sel_0.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.nif_clk_mux_sel_0.lsb = 3;
    Gfa_bi_fpga_io_regs.misc_2.nif_clk_mux_sel_0.msb = 3;
    Gfa_bi_fpga_io_regs.misc_2.nif_clk_mux_sel_1.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.nif_clk_mux_sel_1.lsb = 4;
    Gfa_bi_fpga_io_regs.misc_2.nif_clk_mux_sel_1.msb = 4;
    Gfa_bi_fpga_io_regs.misc_2.port_1_phy_reset.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.port_1_phy_reset.lsb = 5;
    Gfa_bi_fpga_io_regs.misc_2.port_1_phy_reset.msb = 5;
    Gfa_bi_fpga_io_regs.misc_2.port_2_phy_reset.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.port_2_phy_reset.lsb = 6;
    Gfa_bi_fpga_io_regs.misc_2.port_2_phy_reset.msb = 6;
    Gfa_bi_fpga_io_regs.misc_2.port_3_phy_reset.address = 0x08;
    Gfa_bi_fpga_io_regs.misc_2.port_3_phy_reset.lsb = 7;
    Gfa_bi_fpga_io_regs.misc_2.port_3_phy_reset.msb = 7;

    Gfa_bi_fpga_io_regs.misc_3.addr.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.misc_3.addr.msb = 7;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p1_reset_n.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p1_reset_n.lsb = 0;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p1_reset_n.msb = 0;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p2_reset_n.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p2_reset_n.lsb = 1;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p2_reset_n.msb = 1;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p3_reset_n.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p3_reset_n.lsb = 2;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p3_reset_n.msb = 2;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p4_reset_n.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p4_reset_n.lsb = 3;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p4_reset_n.msb = 3;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p5_reset_n.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p5_reset_n.lsb = 4;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p5_reset_n.msb = 4;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p6_reset_n.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p6_reset_n.lsb = 5;
    Gfa_bi_fpga_io_regs.misc_3.qsfp_p6_reset_n.msb = 5;
    Gfa_bi_fpga_io_regs.misc_3.i2c_read_mux.address = 0x0c;
    Gfa_bi_fpga_io_regs.misc_3.i2c_read_mux.lsb = 7;
    Gfa_bi_fpga_io_regs.misc_3.i2c_read_mux.msb = 7;

    Gfa_bi_fpga_io_regs.misc_4.addr.address = 0x10;
    Gfa_bi_fpga_io_regs.misc_4.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.misc_4.addr.msb = 7;
    Gfa_bi_fpga_io_regs.misc_4.fc_a_dir.address = 0x10;
    Gfa_bi_fpga_io_regs.misc_4.fc_a_dir.lsb = 0;
    Gfa_bi_fpga_io_regs.misc_4.fc_a_dir.msb = 0;
    Gfa_bi_fpga_io_regs.misc_4.fc_b_dir.address = 0x10;
    Gfa_bi_fpga_io_regs.misc_4.fc_b_dir.lsb = 1;
    Gfa_bi_fpga_io_regs.misc_4.fc_b_dir.msb = 1;
    Gfa_bi_fpga_io_regs.misc_4.fc_a_oe_n.address = 0x10;
    Gfa_bi_fpga_io_regs.misc_4.fc_a_oe_n.lsb = 2;
    Gfa_bi_fpga_io_regs.misc_4.fc_a_oe_n.msb = 2;
    Gfa_bi_fpga_io_regs.misc_4.fc_b_oe_n.address = 0x10;
    Gfa_bi_fpga_io_regs.misc_4.fc_b_oe_n.lsb = 3;
    Gfa_bi_fpga_io_regs.misc_4.fc_b_oe_n.msb = 3;

    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.addr.address = 0x14;                     /* partial same as gfa */
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.addr.msb = 7;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset.lsb = 0;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset.msb = 0;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset.lsb = 1;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset.msb = 1;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_pll_bypass.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_pll_bypass.lsb = 2;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_pll_bypass.msb = 2;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_ddr_pll_bypass.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_ddr_pll_bypass.lsb = 3;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_ddr_pll_bypass.msb = 3;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_core_pll_bypass.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_core_pll_bypass.lsb = 4;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_core_pll_bypass.msb = 4;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_hw_reset.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_hw_reset.lsb = 5;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_hw_reset.msb = 5;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_pll_reset.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_pll_reset.lsb = 6;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_pll_reset.msb = 6;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_qdr_doff_n.address = 0x14;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_qdr_doff_n.lsb = 7;
    Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_qdr_doff_n.msb = 7;

    Gfa_bi_fpga_io_regs.interrupt.addr.address = 0x1c;
    Gfa_bi_fpga_io_regs.interrupt.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.interrupt.addr.msb = 7;

    Gfa_bi_fpga_io_regs.power_supply_status.addr.address = 0x20;
    Gfa_bi_fpga_io_regs.power_supply_status.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.power_supply_status.addr.msb = 7;

    Gfa_bi_fpga_io_regs.power_supply_control.addr.address = 0x24;
    Gfa_bi_fpga_io_regs.power_supply_control.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.power_supply_control.addr.msb = 7;

    Gfa_bi_fpga_io_regs.i2c_mux_select.addr.address = 0x28;                                   /* partial same as gfa */
    Gfa_bi_fpga_io_regs.i2c_mux_select.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.i2c_mux_select.addr.msb = 7;
    Gfa_bi_fpga_io_regs.i2c_mux_select.qsfp_i2c_mux_reset.address = 0x28;
    Gfa_bi_fpga_io_regs.i2c_mux_select.qsfp_i2c_mux_reset.lsb = 3;
    Gfa_bi_fpga_io_regs.i2c_mux_select.qsfp_i2c_mux_reset.msb = 3;
    Gfa_bi_fpga_io_regs.i2c_mux_select.clockes_i2c_mux_reset.address = 0x28;
    Gfa_bi_fpga_io_regs.i2c_mux_select.clockes_i2c_mux_reset.lsb = 4;
    Gfa_bi_fpga_io_regs.i2c_mux_select.clockes_i2c_mux_reset.msb = 4;
    Gfa_bi_fpga_io_regs.i2c_mux_select.power_i2c_mux_reset.address = 0x28;
    Gfa_bi_fpga_io_regs.i2c_mux_select.power_i2c_mux_reset.lsb = 5;
    Gfa_bi_fpga_io_regs.i2c_mux_select.power_i2c_mux_reset.msb = 5;

    Gfa_bi_fpga_io_regs.i2c_mux_config.addr.address = 0x2c;                                   /* same as in gfa */
    Gfa_bi_fpga_io_regs.i2c_mux_config.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.i2c_mux_config.addr.msb = 7;

    Gfa_bi_fpga_io_regs.i2c_address_command.addr.address = 0x30;                              /* same as in gfa */
    Gfa_bi_fpga_io_regs.i2c_address_command.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.i2c_address_command.addr.msb = 7;

    Gfa_bi_fpga_io_regs.i2c_data_tx_rx.addr.address = 0x34;                                   /* same as in gfa */
    Gfa_bi_fpga_io_regs.i2c_data_tx_rx.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.i2c_data_tx_rx.addr.msb = 7;

    Gfa_bi_fpga_io_regs.i2c_status.addr.address = 0x38;                                       /* same as in gfa */
    Gfa_bi_fpga_io_regs.i2c_status.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.i2c_status.addr.msb = 7;

    Gfa_bi_fpga_io_regs.i2c_triggers.addr.address = 0x3c;                                         /* same as in gfa */
    Gfa_bi_fpga_io_regs.i2c_triggers.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.i2c_triggers.addr.msb = 7;

    Gfa_bi_fpga_io_regs.debug_leds.addr.address = 0x40;
    Gfa_bi_fpga_io_regs.debug_leds.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.debug_leds.addr.msb = 7;
    Gfa_bi_fpga_io_regs.debug_leds.led_0.address = 0x40;
    Gfa_bi_fpga_io_regs.debug_leds.led_0.lsb = 0;
    Gfa_bi_fpga_io_regs.debug_leds.led_0.msb = 0;
    Gfa_bi_fpga_io_regs.debug_leds.led_1.address = 0x40;
    Gfa_bi_fpga_io_regs.debug_leds.led_1.lsb = 1;
    Gfa_bi_fpga_io_regs.debug_leds.led_1.msb = 1;
    Gfa_bi_fpga_io_regs.debug_leds.led_2.address = 0x40;
    Gfa_bi_fpga_io_regs.debug_leds.led_2.lsb = 2;
    Gfa_bi_fpga_io_regs.debug_leds.led_2.msb = 2;
    Gfa_bi_fpga_io_regs.debug_leds.fe_led_0.address = 0x40;
    Gfa_bi_fpga_io_regs.debug_leds.fe_led_0.lsb = 3;
    Gfa_bi_fpga_io_regs.debug_leds.fe_led_0.msb = 3;
    Gfa_bi_fpga_io_regs.debug_leds.fe_led_1.address = 0x40;
    Gfa_bi_fpga_io_regs.debug_leds.fe_led_1.lsb = 4;
    Gfa_bi_fpga_io_regs.debug_leds.fe_led_1.msb = 4;    

    Gfa_bi_fpga_io_regs.combo_switches_controls.addr.address = 0x44;                               /* same as in gfa */
    Gfa_bi_fpga_io_regs.combo_switches_controls.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.combo_switches_controls.addr.msb = 7;
    Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select.address = 0x44;
    Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select.lsb = 0;
    Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select.msb = 0;
    Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select.address = 0x44;
    Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select.lsb = 1;
    Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select.msb = 1;

    Gfa_bi_fpga_io_regs.qsfp_module_present.addr.address = 0x48;
    Gfa_bi_fpga_io_regs.qsfp_module_present.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.qsfp_module_present.addr.msb = 7;

    Gfa_bi_fpga_io_regs.core_synt_freq.addr.address = 0x50;
    Gfa_bi_fpga_io_regs.core_synt_freq.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.core_synt_freq.addr.msb = 7;

    Gfa_bi_fpga_io_regs.qdr_synt_freq.addr.address = 0x54;
    Gfa_bi_fpga_io_regs.qdr_synt_freq.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.qdr_synt_freq.addr.msb = 7;

    Gfa_bi_fpga_io_regs.ddr_synt_freq.addr.address = 0x58;                                            /* same as in gfa */
    Gfa_bi_fpga_io_regs.ddr_synt_freq.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.ddr_synt_freq.addr.msb = 7;

    Gfa_bi_fpga_io_regs.fabric_synt_freq.addr.address = 0x5c;
    Gfa_bi_fpga_io_regs.fabric_synt_freq.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.fabric_synt_freq.addr.msb = 7;

    Gfa_bi_fpga_io_regs.nif_synt_freq.addr.address = 0x60;
    Gfa_bi_fpga_io_regs.nif_synt_freq.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.nif_synt_freq.addr.msb = 7;
    
    Gfa_bi_fpga_io_regs.combo_synt_freq.addr.address = 0x64;
    Gfa_bi_fpga_io_regs.combo_synt_freq.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.combo_synt_freq.addr.msb = 7;

    Gfa_bi_fpga_io_regs.protected_register_write_control.addr.address = 0x68;
    Gfa_bi_fpga_io_regs.protected_register_write_control.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.protected_register_write_control.addr.msb = 7;

    /* 
     * Gfa-Bi 2 registers
     */
    Gfa_bi_fpga_io_regs.interrupt_1.addr.address = 0x1c;
    Gfa_bi_fpga_io_regs.interrupt_1.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.interrupt_1.addr.msb = 7;

    Gfa_bi_fpga_io_regs.interrupt_1.addr.address = 0x6c;
    Gfa_bi_fpga_io_regs.interrupt_1.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.interrupt_1.addr.msb = 7;

    /* 0x28 - changed register */
    /* 0x44 - removed register */

    Gfa_bi_fpga_io_regs.qphy_port_reset_1.addr.address = 0x48; 
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.addr.msb = 7;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n0.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n0.lsb = 0;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n0.msb = 0;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n1.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n1.lsb = 1;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n1.msb = 1;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n2.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n2.lsb = 2;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n2.msb = 2;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n3.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n3.lsb = 3;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n3.msb = 3;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n0.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n0.lsb = 4;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n0.msb = 4;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n1.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n1.lsb = 5;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n1.msb = 5;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n2.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n2.lsb = 6;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n2.msb = 6;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n3.address = 0x48;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n3.lsb = 7;
    Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n3.msb = 7;

    Gfa_bi_fpga_io_regs.qphy_port_reset_2.addr.address = 0x70; 
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.addr.lsb = 0;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.addr.msb = 7;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n0.address = 0x70;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n0.lsb = 0;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n0.msb = 0;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n1.address = 0x70;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n1.lsb = 1;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n1.msb = 1;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n2.address = 0x70;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n2.lsb = 2;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n2.msb = 2;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n3.address = 0x70;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n3.lsb = 3;
    Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n3.msb = 3;

    Gfa_bi_fpga_io_regs_init = TRUE;
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_bi_fpga_io_agent_start(
      SOC_BSP_CARDS_DEFINES          card_type
  )
{
    uint8
        err = SOC_SAND_OK;
#ifndef __DUNE_GTO_BCM_CPU__
  uint8
    value = 0;
  uint32
    offset;
#else
  int i2c_chip_addr, i2c_data_addr, i2c_value;
  CPU_I2C_BUS_LEN i2c_alen;
#endif
    
  UTILS_INIT_ERR_DEFS("gfa_bi_fpga_io_agent_start");

#ifndef __DUNE_GTO_BCM_CPU__
  /* set io_agent reset, mux select from GPIO */
  value = 0x00;
  err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_4_DEVICE_ADDR, &value, 1, GFA_BI_I2C_GPIO_DATA_INT_ADDR, TRUE);
  UTILS_EXIT_IF_ERR(err, 10);

  soc_sand_os_task_delay_milisec(50);
  
  /* Fpga Io out of reset */
  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));

  value = read_epld_reg((uint32)offset);
  SOC_SAND_SET_BIT(value,0x1,GFA_BI_DEVICE_RESET_IO_AGENT_FPGA_BIT);
  write_epld_reg((uint8)value,(uint32)offset);
#else
   if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
        /* io_agent reset */
        i2c_chip_addr = 0x4b;
        i2c_alen      = CPU_I2C_ALEN_BYTE_DLEN_BYTE;
        i2c_data_addr = 0x58;
        i2c_value     = 0x2;
        err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
        UTILS_EXIT_IF_ERR(err, 20);
   }
#endif

  soc_sand_os_task_delay_milisec(50);

  if ((gfa_petra_fpga_io_fld_get(&Gfa_bi_fpga_io_regs.scratch_test_register.addr)) != 0x55)
  {
    soc_sand_os_printf("%s(): scratch_test_register != 0x55\n", FUNCTION_NAME());
    return SOC_SAND_ERR;
  }
  else
  {
      soc_sand_os_printf("%s(): io_agent version=0x%x\n", FUNCTION_NAME(), gfa_petra_fpga_io_fld_get(&Gfa_bi_fpga_io_regs.version.addr));
  }
  
  /* turn led on */
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.debug_leds.led_0, 0x0);
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.debug_leds.led_1, 0x1);
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.debug_leds.led_2, 0x1);
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.debug_leds.fe_led_0, 0x1);
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.debug_leds.fe_led_1, 0x1);

  if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
      soc_sand_os_printf("%s(): cofigure io_agent according  to LINE_CARD_GFA_PETRA_B_INTERLAKEN\n", FUNCTION_NAME());
    
      /* turn on OOB FC A/B to output from IO Agent */
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_4.fc_a_dir, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_4.fc_b_dir, 0x1);
    
      /* take port phy out of reset */
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_2.port_1_phy_reset, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_2.port_2_phy_reset, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_2.port_3_phy_reset, 0x1);
    
      /* take qsfp ports out of reset */
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_3.qsfp_p1_reset_n, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_3.qsfp_p2_reset_n, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_3.qsfp_p3_reset_n, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_3.qsfp_p4_reset_n, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_3.qsfp_p5_reset_n, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.misc_3.qsfp_p6_reset_n, 0x1);
    
      /* set combo mux */
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select, 0x1);
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select, 0x0);
  } else if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2) {
      soc_sand_os_printf("%s(): cofigure io_agent according  to LINE_CARD_GFA_PETRA_B_INTERLAKEN_2\n", FUNCTION_NAME());

      /* take qphy ports out of reset */
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n0, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n1, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n2, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy1_port_reset_n3, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n0, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n1, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n2, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_1.qphy2_port_reset_n3, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n0, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n1, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n2, 0x1);  
      gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.qphy_port_reset_2.qphy3_port_reset_n3, 0x1);  
  } else {
      soc_sand_os_printf("%s(): Error unknown card type=0x%x !!!\n", FUNCTION_NAME(), card_type);
       return SOC_SAND_ERR;
  }

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_bi_init_host_board_mezzanine_epld(void
  )
{
  uint32   
    offset;
  uint8
    value = 0x0;

  /* Mezzanine-epld init */
  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
#ifndef __DUNE_GTO_BCM_CPU__
  value = read_epld_reg((uint32)offset);
#endif
#if DEBUG_GFA_BI_BOARD
  soc_sand_os_printf("gfa_bi_init_host_board_mezzanine_epld. reset control read:  offset=0x%x, value=0x%x\n",offset, value);
#endif
  value = 0x0;
#if DEBUG_GFA_BI_BOARD
  soc_sand_os_printf("gfa_bi_init_host_board_mezzanine_epld. canceled!!!: reset control write: offset=0x%x, value=0x%x\n",offset,value);
#endif
  /* write_epld_reg((uint8)value,(uint32)offset); */

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_bi_utils_hot_swap_enable(
    SOC_SAND_IN  uint8                  silent,
    SOC_SAND_IN  uint32                   enable
  )
{
#ifndef __DUNE_GTO_BCM_CPU__
  uint8
    value = 0;
  uint32
    offset;
#else

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
  int unit = 0; 
#endif

  uint32 cpu_regs_addr, cpu_regs_value;
#endif

  UTILS_INIT_ERR_DEFS("gfa_bi_utils_hot_swap_reset");

#ifndef __DUNE_GTO_BCM_CPU__
  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset);
  SOC_SAND_SET_BIT(value,enable,GFA_BI_DEVICE_RESET_HOT_SWAP_BIT);
  write_epld_reg((uint8)value,(uint32)offset);
#else
    /* turn on/off HotSwap */
    cpu_regs_addr  = 0xe0040;
    if (enable == 0x1)
    {
        cpu_regs_value = 0x40003;
    }
    else
    {
        cpu_regs_value = 0x3;
    }
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(__DUNE_GTO_BCM_CPU__)
    _cpu_write(unit, cpu_regs_addr, &cpu_regs_value);
#endif

    /* increase i2c freq */
    cpu_regs_addr  = 0x3104;
    cpu_regs_value = 0x06060606;
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(__DUNE_GTO_BCM_CPU__)
    _cpu_write(unit, cpu_regs_addr, &cpu_regs_value);
#endif

#endif

  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_bi_utils_epld_device_reset_init(void
  )
{
  uint8
    value = 0;
  uint32
    offset;

  UTILS_INIT_ERR_DEFS("gfa_bi_utils_epld_device_reset_init");

  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
#ifndef __DUNE_GTO_BCM_CPU__
  value = read_epld_reg((uint32)offset);
#endif
  SOC_SAND_SET_BIT(value,0x1,GFA_BI_DEVICE_RESET_PETRA_BIT);
  SOC_SAND_SET_BIT(value,0x1,GFA_BI_DEVICE_RESET_PCP_FPGA_BIT);
#if DEBUG_GFA_BI_BOARD
  soc_sand_os_printf("gfa_bi_utils_epld_device_reset_init. reset control write: offset=0x%x, value=0x%x\n",offset,value);
#endif
#ifndef __DUNE_GTO_BCM_CPU__
  write_epld_reg((uint8)value,(uint32)offset);
#endif

  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_bi_init_i2c_devices(
      SOC_BSP_CARDS_DEFINES          card_type
  )
{
  uint32
    err = SOC_SAND_OK;
#ifndef __DUNE_GTO_BCM_CPU__
  uint8
    iolatch_value,
    direction_value;
#else
    int i2c_chip_addr, i2c_data_addr, i2c_value;
    CPU_I2C_BUS_LEN i2c_alen;
#endif

  UTILS_INIT_ERR_DEFS("gfa_bi_init_i2c_devices");

  /* set setup byte for PS A/D according to HW Specification */
  /*
  value = 0xa2;
  err = lb_bsp_i2c_write_gen_inner(
          GFA_BI_I2C_PS_AD_DEVICE_ADDR, 
          &value, 
          1, 
          0, 
          FALSE
        );
  UTILS_EXIT_IF_ERR(err, 10);
  */

#ifndef __DUNE_GTO_BCM_CPU__
  /* set gpio device input/output pin's */
  soc_sand_os_printf("gfa_bi_init_i2c_devices. configure gpio: input/output pin's\n");

  if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
      soc_sand_os_printf("%s(): cofigure i2c devices according  to LINE_CARD_GFA_PETRA_B_INTERLAKEN\n", FUNCTION_NAME());

      iolatch_value = 0x1;
      direction_value = 0x00;
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_1_DEVICE_ADDR, &iolatch_value, 1, GFA_BI_I2C_GPIO_IOLATCH_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 10);
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_1_DEVICE_ADDR, &direction_value, 1, GFA_BI_I2C_GPIO_DIRECTION_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 20);
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_2_DEVICE_ADDR, &iolatch_value, 1, GFA_BI_I2C_GPIO_IOLATCH_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 30);
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_2_DEVICE_ADDR, &direction_value, 1, GFA_BI_I2C_GPIO_DIRECTION_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 40);
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_3_DEVICE_ADDR, &iolatch_value, 1, GFA_BI_I2C_GPIO_IOLATCH_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 50);
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_3_DEVICE_ADDR, &direction_value, 1, GFA_BI_I2C_GPIO_DIRECTION_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 60);
      
      direction_value = 0x02;
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_4_DEVICE_ADDR, &iolatch_value, 1, GFA_BI_I2C_GPIO_IOLATCH_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 70);
      err = lb_bsp_i2c_write_gen_inner(GFA_BI_I2C_GPIO_4_DEVICE_ADDR, &direction_value, 1, GFA_BI_I2C_GPIO_DIRECTION_INT_ADDR, TRUE);
      UTILS_EXIT_IF_ERR(err, 80);

  } 
#else

    if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
      soc_sand_os_printf("%s(): cofigure i2c devices according  to LINE_CARD_GFA_PETRA_B_INTERLAKEN\n", FUNCTION_NAME());
        /* set I2C devices: gpio */
        i2c_chip_addr = 0x4b;
        i2c_alen      = CPU_I2C_ALEN_BYTE_DLEN_BYTE;
        
        i2c_data_addr = 0x70;
        i2c_value     = 0x1;
        err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
        UTILS_EXIT_IF_ERR(err, 90);
        sal_usleep(50000); 
        i2c_data_addr = 0x50;
        i2c_value     = 0x3;
        err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
        UTILS_EXIT_IF_ERR(err, 100);
        sal_usleep(50000);
        i2c_data_addr = 0x58;
        i2c_value     = 0x0;
        err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
        UTILS_EXIT_IF_ERR(err, 110);
    }
    
    /* Delay between mux set and burn start */
    sal_usleep(100000);
#endif

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET 
  gfa_bi_fpga_pcp_init(
      SOC_BSP_CARDS_DEFINES          card_type,
      SOC_SAND_IN  uint8        silent
  )
{
  const char
    *pcp_file_name = GFA_BI_PCP_FPGA_FILE_NAME;

#ifdef __DUNE_GTO_BCM_CPU__
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && (defined(LINUX) || defined(UNIX))
  int unit = 0; 
#endif
  int i2c_chip_addr, i2c_data_addr, i2c_value;
  CPU_I2C_BUS_LEN i2c_alen;
#endif

  if(pcp_file_name != NULL)
  {
#ifndef __DUNE_GTO_BCM_CPU__
    if (gfa_petra_fpga_download_and_burn(pcp_file_name, GFA_PETRA_STREAMING_FPGA_GPIO, silent))
    {
      return SOC_SAND_ERR;
    }
#else
    /* burn pcp */
    /* set fpga mux select - to burn pcp*/
    i2c_alen      = CPU_I2C_ALEN_BYTE_DLEN_BYTE;
    i2c_data_addr = 0x58;
    if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
        i2c_chip_addr = 0x4b;
        i2c_value     = 0x3; /* bit 0 - mux select, bit 1 - io_agent (keep 1) */
    } else if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2) {
        i2c_chip_addr = 0x4c;
        i2c_value     = 0x10; /* bit 0 - mux select, bit 1 - io_agent (keep 1) */
    } else {
        soc_sand_os_printf("%s(): Error unknown card type=0x%x !!!\n", FUNCTION_NAME(), card_type);
        return SOC_SAND_ERR;
    }

    cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
    
    /* Delay between mux set and burn start */
    sal_usleep(100000);
    
    /* unset pcp reset pins */
    gfa_bi_fpga_pcp_hw_reset(TRUE);
    
    /* actual burn */
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#if (defined(LINUX) || defined(UNIX))
    soc_sand_os_printf("utils_line_gfa_bi.c, gfa_bi_fpga_pcp_init(): burn pcp\n");
    soc_dpp_fpga_load(unit, GFA_BI_PCP_FPGA_FILE_NAME);
#else
    /* For VxWorks, skip pcp burn */
    soc_sand_os_printf("utils_line_gfa_bi.c, gfa_bi_fpga_pcp_init(): skipping burn pcp\n");
#endif /* LINUX */
#endif
#endif
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_bi_fpga_pcp_hw_reset(
    SOC_SAND_IN  uint8 in_reset_not_out_of_reset
  )
{
  uint8
    reset_val;

  reset_val = SOC_SAND_BOOL2NUM_INVERSE(in_reset_not_out_of_reset);

  /* reset: Pcp, Pcp pll  */
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_pll_reset, reset_val);
  soc_sand_os_task_delay_milisec(100);
  gfa_petra_fpga_io_fld_set(&Gfa_bi_fpga_io_regs.soc_petra_miscelanous_controls_1.pcp_hw_reset, reset_val);

  return SOC_SAND_OK;
}
  
SOC_SAND_RET
  gfa_bi_init_host_board(
    SOC_SAND_IN  char                       board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN],
    SOC_SAND_IN  uint16                   gfa_board_serial_number,
    SOC_SAND_IN  SOC_BSP_CARDS_DEFINES          card_type,
    SOC_SAND_OUT uint32                  *chip_ver
  )
{
#ifdef __DUNE_GTO_BCM_CPU__
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && (defined(LINUX) || defined(UNIX))
    int unit = 0; 
#endif
#endif

  if (gfa_petra_fpga_io_regs_init())
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_petra_fpga_io_regs_init(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }
  
  if (gfa_bi_fpga_io_regs_init())
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_fpga_io_regs_init(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }

  if (Gfa_petra_GFA_PETRA_BOARD_SPECIFICATIONS_clear(card_type))
  {
    soc_sand_os_printf("gfa_bi_init_host_board. Gfa_petra_GFA_PETRA_BOARD_SPECIFICATIONS_clear(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }

#ifndef __DUNE_GTO_BCM_CPU__
  soc_sand_os_printf("gfa_bi_init_host_board. gfa_petra_init_host_board_860()\n");
  if (gfa_petra_init_host_board_860())
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_petra_init_host_board_860(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }

  soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_init_host_board_mezzanine_epld()\n");
  if (gfa_bi_init_host_board_mezzanine_epld())
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_init_host_board_mezzanine_epld(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }
#endif

  if (gfa_bi_utils_hot_swap_enable(FALSE,1))
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_utils_hot_swap_enable(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }

#ifndef __DUNE_GTO_BCM_CPU__
  soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_utils_epld_device_reset_init()\n");
  if (gfa_bi_utils_epld_device_reset_init())
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_utils_epld_device_reset_init(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }
#endif

  if (gfa_bi_init_i2c_devices(card_type))
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_init_i2c_devices(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }

#ifndef __DUNE_GTO_BCM_CPU__
  soc_sand_os_printf("gfa_bi_init_host_board. gfa_petra_fpga_io_agent_init()\n");
  if (gfa_petra_fpga_io_agent_init(FALSE))
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_petra_fpga_io_agent_init(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }
#else
  /* burn io_agent */
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && (defined(LINUX) || defined(UNIX))
   /* For VxWorks, skip fpga load */
   if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) {
       soc_dpp_fpga_load(unit, GFA_BI_IO_AGENT_FPGA_FILE_NAME);
   } else if (card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2) {
       soc_dpp_fpga_load(unit, GFA_BI2_IO_AGENT_FPGA_FILE_NAME);
   }
#endif
#endif

  if (gfa_bi_fpga_io_agent_start(card_type)) 
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_fpga_io_agent_start(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }

#ifndef __DUNE_GTO_BCM_CPU__
#else
  if (gfa_bi_fpga_pcp_init(card_type, FALSE))
  {
    soc_sand_os_printf("gfa_bi_init_host_board. gfa_bi_fpga_pcp_init(). FAILED !!!\n");
    return SOC_SAND_ERR;
  }  
#endif

  /* remark: pcp start occur in pcp sweep */

  *chip_ver = SOC_PETRA_EXPECTED_CHIP_VER;

  return SOC_SAND_OK;
}


/* UI functions */
#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_bi_ps_ad_read_single(
    SOC_SAND_IN  uint32                   channel,
    SOC_SAND_OUT uint8                    *buffer
  )
{
  uint32
    err = SOC_SAND_OK;
  uint8
    value = 0;

  UTILS_INIT_ERR_DEFS("gfa_bi_ps_ad_read_single");

  UTILS_ERR_IF_NULL(buffer, 5);

  /* set setup byte for PS A/D according to HW Specification */
  value = 0x01;
  soc_sand_os_printf("gfa_bi_ps_ad_read_single. write value=0x%x\n",value);
  err = lb_bsp_i2c_write_gen_inner(
          GFA_BI_I2C_PS_AD_DEVICE_ADDR, 
          &value, 
          1, 
          0, 
          FALSE
        );
  UTILS_EXIT_IF_ERR(err, 10);

  err = lb_petra_i2c_read(
          GFA_BI_I2C_PS_AD_DEVICE_ADDR,
          0x0,
          2,
          buffer
        );
  UTILS_EXIT_IF_ERR(err, 20);
  soc_sand_os_printf("gfa_bi_ps_ad_read_single. read buffer[0]=0x%x, buffer[1]=0x%x\n",buffer[0],buffer[1]);
 
exit:
  UTILS_EXIT_AND_PRINT_ERR;
}                       
#endif 

#ifndef __DUNE_GTO_BCM_CPU__
/* I2C Functions */
int
  gfa_bi_bsp_i2c_write_gen_inner(
    uint8 device_address,
    uint8 write_data[GFA_BI_BSP_I2C_BUFFER_SIZE],
    uint8 write_data_len
  )
{
  int
    ret = 0;
#if !SAND_LOW_LEVEL_SIMULATION
  char
    *proc_name ;
  char
    err_msg[80*3] ;
  unsigned int
      err_flag ;
  uint8
    device_write_address;

  proc_name = "gfa_bsp_i2c_write_gen_inner" ;
  device_write_address = device_address << 1;

#if DEBUG_GFA_BI_BOARD
  {
    int byte_i;
    soc_sand_os_printf("gfa_bi_bsp_i2c_write_gen_inner. device_address=0x%x, write_data_len=%d\n", device_address,write_data_len);
    soc_sand_os_printf("write_data: ");
    for(byte_i=0;byte_i<write_data_len;byte_i++)
    {
      soc_sand_os_printf("0x%x ", write_data[byte_i]);
    }
    soc_sand_os_printf("\n\r");
  }
#endif


  /*
   * Write a byte.
   */
  ret = i2c_load_buffer(
          device_write_address,
          write_data_len,
          write_data,
          &err_flag,
          err_msg
        );
  gen_err(TRUE,FALSE,(int)err_flag,0,err_msg,proc_name,SVR_WRN,I2C_MEM_ERR_01,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,"I2C fail. #1 ",proc_name,SVR_ERR,I2C_MEM_ERR_02,FALSE,0,-1,FALSE) ;
  if (ret || err_flag)
  {
    ret = TRUE;
    goto exit ;
  }

  soc_sand_os_task_delay_milisec(50);

  /*
   * Set a kick to fulfill the request.
   */
  i2c_start() ;

  ret = i2c_poll_tx(
          &err_flag,
          err_msg
        );
  gen_err(TRUE,FALSE,(int)err_flag,0,err_msg,proc_name,SVR_WRN,I2C_MEM_ERR_03,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,"I2C fail. #2 ",proc_name,SVR_ERR,I2C_MEM_ERR_04,FALSE,0,-1,FALSE) ;
  if (ret)
  {
    ret = TRUE;
    goto exit ;
  }

exit:
#endif
  return ret;
}

int
  gfa_bi_bsp_i2c_write_gen( 
    uint8 device_address,
    uint8 internal_address[GFA_BI_BSP_I2C_INT_ADDR_LEN],
    uint8 internal_address_len,
    uint8 write_data[GFA_BI_BSP_I2C_BUFFER_SIZE],
    uint8 write_data_len
  )
{
  uint32
    err = SOC_SAND_OK;
  uint8
    inner_write_len = internal_address_len + write_data_len;
  uint8
    inner_write_data[GFA_BI_BSP_I2C_BUFFER_SIZE];

  UTILS_INIT_ERR_DEFS("gfa_bi_bsp_i2c_write_gen");

  UTILS_ERR_IF_NULL(write_data, 10);

#if DEBUG_GFA_BI_BOARD
  {
    int byte_i;
    soc_sand_os_printf("gfa_bi_bsp_i2c_write_gen. device_address=0x%x, internal_address_len=%d, write_data_len=%d\n", device_address,internal_address_len,write_data_len);
    soc_sand_os_printf("internal_address: ");
    for(byte_i=0;byte_i<internal_address_len;byte_i++)
    {
      soc_sand_os_printf("0x%x ", internal_address[byte_i]);
    }
    soc_sand_os_printf("\n\r");
    soc_sand_os_printf("write_data: ");
    for(byte_i=0;byte_i<write_data_len;byte_i++)
    {
      soc_sand_os_printf("0x%x ", write_data[byte_i]);
    }
    soc_sand_os_printf("\n\r");
  }
#endif

  soc_sand_os_memset(inner_write_data,0x0,sizeof(uint8)*GFA_BI_BSP_I2C_BUFFER_SIZE);
  
  if (internal_address_len != 0)
  {
    soc_sand_os_memcpy(inner_write_data, internal_address, internal_address_len);
  }

  soc_sand_os_memcpy(inner_write_data + internal_address_len, write_data, write_data_len); 

  err = gfa_bi_bsp_i2c_write_gen_inner(
          device_address,
          inner_write_data,
          inner_write_len
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

int
  gfa_bi_bsp_i2c_read_gen_inner(
    uint8  device_address,
    uint8  exp_read_len,
    uint8  read_data[GFA_BI_BSP_I2C_BUFFER_SIZE],
    unsigned short *nof_read_data_entries
  )
{
  int
    ret = 0;
#if !SAND_LOW_LEVEL_SIMULATION
  char
      *proc_name ;
  uint8
    read_dummy_data;
  char
    err_msg[80*3] ;
  unsigned int
      err_flag ;
  unsigned short
    i2c_data_len;
  uint8
    device_read_address;

  proc_name = "gfa_bi_bsp_i2c_read_gen_inner" ;

#if DEBUG_GFA_BI_BOARD
  {
    soc_sand_os_printf("gfa_bi_bsp_i2c_read_gen_inner. device_address=0x%x\n", device_address);
  }
#endif

  if (exp_read_len == 0)
  {
    soc_sand_os_printf("ERROR: gfa_bi_bsp_i2c_read_gen_inner. exp_read_len=%d\n", exp_read_len);
    ret = TRUE;
    goto exit ;   
  }

  device_read_address  = device_address << 1;
  device_read_address |= 0x1;

  /*
   * Set the address of the device to read from.
   */
  read_dummy_data = 0xff;
  ret = i2c_load_buffer(
          device_read_address,
          exp_read_len * sizeof(read_dummy_data),
          (uint8*)(&read_dummy_data),
          &err_flag,
          err_msg
        );
  gen_err(TRUE,FALSE,(int)err_flag,0,err_msg,proc_name,SVR_WRN,I2C_MEM_ERR_05,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,"I2C fail. #3 ",proc_name,SVR_ERR,I2C_MEM_ERR_06,FALSE,0,-1,FALSE) ;
  if (ret || err_flag)
  {
    soc_sand_os_printf("ERROR: i2c_load_buffer\n");
    ret = TRUE;
    goto exit ;
  }

  soc_sand_os_task_delay_milisec(50);

  /*
   * Set a kick to fulfill the request.
   */
  i2c_start() ;

  ret = i2c_poll_tx(&err_flag,err_msg) ;
  gen_err(TRUE,FALSE,(int)err_flag,0,err_msg,proc_name,SVR_WRN,I2C_MEM_ERR_07,FALSE,0,-1,FALSE);
  gen_err(TRUE,FALSE,(int)ret,0,"I2C fail. #4 ",proc_name,SVR_ERR,I2C_MEM_ERR_08,FALSE,0,-1,FALSE) ;
  if (ret || err_flag)
  {
    soc_sand_os_printf("ERROR: i2c_poll_tx\n");
    ret = TRUE;
    goto exit ;
  }

  /*
   * Wait 50 milisec to make sure received bytes are registered.
   */
  soc_sand_os_task_delay_milisec(50);

  ret = i2c_get_buffer(
          &i2c_data_len,
          read_data,
          &err_flag,
          err_msg
        );
  gen_err(TRUE,FALSE,(int)err_flag,0,err_msg,proc_name,SVR_WRN,I2C_MEM_ERR_09,FALSE,0,-1,FALSE);
  gen_err(TRUE,FALSE,(int)ret,0,"I2C fail. #5",proc_name,SVR_ERR,I2C_MEM_ERR_10,FALSE,0,-1,FALSE);
  if (ret || err_flag)
  {
    soc_sand_os_printf("ERROR: i2c_get_buffer\n");
    ret = TRUE;
    goto exit ;
  }

  soc_sand_os_task_delay_milisec(50);

  *nof_read_data_entries = i2c_data_len;

#if DEBUG_GFA_BI_BOARD
  {
    int byte_i;
    soc_sand_os_printf("gfa_bi_bsp_i2c_read_gen_inner. i2c_data_len=%d\n", i2c_data_len);
    soc_sand_os_printf("read_data: ");
    for(byte_i=0;byte_i<i2c_data_len;byte_i++)
    {
      soc_sand_os_printf("0x%x ", read_data[byte_i]);
    }
    soc_sand_os_printf("\n\r");
  }
#endif

exit:
#endif
  return ret;
}

int
  gfa_bi_bsp_i2c_read_gen(
    uint8  device_address,
    uint8  internal_address[GFA_BI_BSP_I2C_INT_ADDR_LEN],
    uint8  internal_address_len,
    uint8  exp_read_len,
    uint32 *read_data,
    uint16 *read_data_len
  )
{
  uint32
    err = SOC_SAND_OK;
  uint8
    inner_write_len = 0;
  uint8
    inner_write_data[GFA_BI_BSP_I2C_BUFFER_SIZE];

  UTILS_INIT_ERR_DEFS("gfa_bi_bsp_i2c_write_gen");

  UTILS_ERR_IF_NULL(read_data, 10);
  soc_sand_os_memset(read_data,0x0,sizeof(uint32));

#if DEBUG_GFA_BI_BOARD
  {
    int byte_i;
    soc_sand_os_printf("gfa_bi_bsp_i2c_read_gen. device_address=0x%x, internal_address_len=%d\n", device_address,internal_address_len);
    soc_sand_os_printf("internal_address: ");
    for(byte_i=0;byte_i<internal_address_len;byte_i++)
    {
      soc_sand_os_printf("0x%x ", internal_address[byte_i]);
    }
    soc_sand_os_printf("\n\r");
  }
#endif

  if (internal_address_len != 0)
  {
    soc_sand_os_memset(inner_write_data,0x0,sizeof(uint8)*GFA_BI_BSP_I2C_BUFFER_SIZE);
    inner_write_len = internal_address_len;
    soc_sand_os_memcpy(inner_write_data, internal_address, internal_address_len);

    err = gfa_bi_bsp_i2c_write_gen_inner(
            device_address,
            inner_write_data,
            inner_write_len
          );
     UTILS_EXIT_IF_ERR(err, 20);
  }

  err = gfa_bi_bsp_i2c_read_gen_inner(
          device_address,
          exp_read_len,
          (uint8 *)read_data,
          read_data_len
        );
  UTILS_EXIT_IF_ERR(err, 30);

#if DEBUG_GFA_BI_BOARD
  {
    int byte_i;
    soc_sand_os_printf("gfa_bi_bsp_i2c_read_gen. read_data_len=%d\n", *read_data_len);
    soc_sand_os_printf("read_data: ");
    for(byte_i=0;byte_i<*read_data_len;byte_i++)
    {
      soc_sand_os_printf("0x%x ", read_data[byte_i]);
    }
    soc_sand_os_printf("\n\r");
  }
#endif

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}
#endif /* __DUNE_GTO_BCM_CPU__ */

 /* 
  * MDIO Functions 
  */

#define GFA_BI_MDIO_SET(phy_ndx, addr, msb, lsb, curr_data)                                         \
                    do                                                                                        \
          {                                                                                         \
            err = soc_petra_nif_mdio45_read(                                                            \
                  unit,                                                                        \
                  phy_ndx,                                                                          \
                  SOC_SAND_GET_BITS_RANGE(addr, 19, 16),                                                \
                  SOC_SAND_GET_BITS_RANGE(addr, 15, 0),                                                 \
                  0x1,                                                                              \
                  &data                                                                             \
                );                                                                                  \
            UTILS_EXIT_IF_ERR(err, 10);                                                             \
            data &= SOC_SAND_ZERO_BITS_MASK(msb,lsb);                                                   \
            data |= SOC_SAND_SET_BITS_RANGE(curr_data,msb,lsb);                                                \
                      err = soc_petra_nif_mdio45_write(                                                           \
                  unit,                                                                        \
                  phy_ndx,                                                                          \
                  SOC_SAND_GET_BITS_RANGE(addr, 19, 16),                                                \
                  SOC_SAND_GET_BITS_RANGE(addr, 15, 0),                                                 \
                  0x1,                                                                              \
                  &data                                                                             \
                );                                                                                  \
            UTILS_EXIT_IF_ERR(err, 20);                                                             \
          } while(0)

#define GFA_BI_AELUROS_MICRO_CONTROLLER_START(phy_ndx)                                              \
          do                                                                                        \
          {                                                                                         \
            GFA_BI_MDIO_SET(phy_ndx, 0x1c04a, 15, 0, 0x5200);                                      \
            soc_sand_os_task_delay_milisec(56);

#define GFA_BI_AELUROS_MICRO_CONTROLLER_END(phy_ndx)                                                \
            GFA_BI_MDIO_SET(phy_ndx, 0x1ca00, 15, 0, 0x0080);                                      \
            GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);                                      \
          } while(0)

SOC_SAND_RET
gfa_bi_phy_aeluros_board_configuration_init(
  SOC_SAND_IN  int                 unit,
  SOC_SAND_IN  uint32                  phy_ndx 
  )
{
  uint32
    err = SOC_SAND_OK;
  uint16 
    data;  

  UTILS_INIT_ERR_DEFS("gfa_bi_phy_aeluros_board_configuration_init");

  GFA_BI_MDIO_SET(phy_ndx, 0x10000, 15, 0, 0xa040);
  /* wait for 100ms for device to get out of reset. */
  soc_sand_os_task_delay_milisec(112);;

  GFA_BI_MDIO_SET(phy_ndx, 0x1c214, 15, 0, 0x8288);
  GFA_BI_MDIO_SET(phy_ndx, 0x10800, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c017, 15, 0, 0xfeb0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c013, 15, 0, 0xf341);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c001, 15, 0, 0x0428);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c210, 15, 0, 0x8000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c210, 15, 0, 0x8100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c210, 15, 0, 0x8000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c210, 15, 0, 0x0000);
  /* wait for 50ms for data path reset to complete. (mandatory patch); */
  soc_sand_os_task_delay_milisec(56);

  GFA_BI_MDIO_SET(phy_ndx, 0x1c214, 15, 0, 0x1abb);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c55b, 15, 0, 0x1d02);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c003, 15, 0, 0x0181);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c010, 15, 0, 0x448a);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
gfa_bi_phy_aeluros_microcode_init(
                                  SOC_SAND_IN  int                 unit,
                                  SOC_SAND_IN  uint32                 phy_ndx
                                  )
{
  uint32
    err = SOC_SAND_OK;
  uint16 
    data;

  UTILS_INIT_ERR_DEFS("gfa_bi_phy_aeluros_microcode_init");

  GFA_BI_AELUROS_MICRO_CONTROLLER_START(phy_ndx);

  GFA_BI_MDIO_SET(phy_ndx, 0x1cc00, 15, 0, 0x2ff4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc01, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc02, 15, 0, 0x2015);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc03, 15, 0, 0x3105);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc04, 15, 0, 0x6524);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc05, 15, 0, 0x27ff);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc06, 15, 0, 0x300f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc07, 15, 0, 0x2c8b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc08, 15, 0, 0x300b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc09, 15, 0, 0x4009);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0a, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0b, 15, 0, 0x2f72);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0c, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0d, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 15, 0, 0x2172);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0f, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc10, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc11, 15, 0, 0x25d2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc12, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc13, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc14, 15, 0, 0xd01e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc15, 15, 0, 0x27d2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc16, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc17, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc18, 15, 0, 0x2004);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc19, 15, 0, 0x3c84);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc1a, 15, 0, 0x6436);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc1b, 15, 0, 0x2007);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc1c, 15, 0, 0x3f87);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc1d, 15, 0, 0x8676);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc1e, 15, 0, 0x40b7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc1f, 15, 0, 0xa746);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc20, 15, 0, 0x4047);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc21, 15, 0, 0x5673);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc22, 15, 0, 0x2982);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc23, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc24, 15, 0, 0x13d2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc25, 15, 0, 0x8bbd);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc26, 15, 0, 0x2862);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc27, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc28, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc29, 15, 0, 0x2092);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc2a, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc2b, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc2c, 15, 0, 0x5cc3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc2d, 15, 0, 0x0314);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc2e, 15, 0, 0x2942);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc2f, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc30, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc31, 15, 0, 0xd019);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc32, 15, 0, 0x2032);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc33, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc34, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc35, 15, 0, 0x2a04);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc36, 15, 0, 0x3c74);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc37, 15, 0, 0x6435);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc38, 15, 0, 0x2fa4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc39, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc3a, 15, 0, 0x6624);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc3b, 15, 0, 0x5563);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc3c, 15, 0, 0x2d42);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc3d, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc3e, 15, 0, 0x13d2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc3f, 15, 0, 0x464d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc40, 15, 0, 0x2862);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc41, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc42, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc43, 15, 0, 0x2032);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc44, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc45, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc46, 15, 0, 0x2fb4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc47, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc48, 15, 0, 0x6624);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc49, 15, 0, 0x5563);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc4a, 15, 0, 0x2d42);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc4b, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc4c, 15, 0, 0x13d2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc4d, 15, 0, 0x2ed2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc4e, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc4f, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc50, 15, 0, 0x2fd2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc51, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc52, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc53, 15, 0, 0x0004);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc54, 15, 0, 0x2942);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc55, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc56, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc57, 15, 0, 0x2092);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc58, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc59, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc5a, 15, 0, 0x5cc3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc5b, 15, 0, 0x0317);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc5c, 15, 0, 0x2f72);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc5d, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc5e, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc5f, 15, 0, 0x2942);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc60, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc61, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc62, 15, 0, 0x22cd);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc63, 15, 0, 0x301d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc64, 15, 0, 0x2862);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc65, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc66, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc67, 15, 0, 0x2ed2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc68, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc69, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc6a, 15, 0, 0x2d72);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc6b, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc6c, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc6d, 15, 0, 0x628f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc6e, 15, 0, 0x2112);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc6f, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc70, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc71, 15, 0, 0x5aa3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc72, 15, 0, 0x2dc2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc73, 15, 0, 0x3002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc74, 15, 0, 0x1312);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc75, 15, 0, 0x6f72);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc76, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc77, 15, 0, 0x2807);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc78, 15, 0, 0x31a7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc79, 15, 0, 0x20c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc7a, 15, 0, 0x3c24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc7b, 15, 0, 0x6724);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc7c, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc7d, 15, 0, 0x2807);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc7e, 15, 0, 0x3187);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc7f, 15, 0, 0x20c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc80, 15, 0, 0x3c24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc81, 15, 0, 0x6724);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc82, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc83, 15, 0, 0x2514);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc84, 15, 0, 0x3c64);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc85, 15, 0, 0x6436);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc86, 15, 0, 0xdff4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc87, 15, 0, 0x6436);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc88, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc89, 15, 0, 0x40a4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc8a, 15, 0, 0x643c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc8b, 15, 0, 0x4016);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc8c, 15, 0, 0x8c6c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc8d, 15, 0, 0x2b24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc8e, 15, 0, 0x3c24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc8f, 15, 0, 0x6435);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc90, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc91, 15, 0, 0x2b24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc92, 15, 0, 0x3c24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc93, 15, 0, 0x643a);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc94, 15, 0, 0x4025);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc95, 15, 0, 0x8a5a);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc96, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc97, 15, 0, 0x2731);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc98, 15, 0, 0x3011);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc99, 15, 0, 0x1001);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc9a, 15, 0, 0xc7a0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc9b, 15, 0, 0x0100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc9c, 15, 0, 0xc502);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc9d, 15, 0, 0x53ac);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc9e, 15, 0, 0xc503);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc9f, 15, 0, 0xd5d5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca0, 15, 0, 0xc600);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca1, 15, 0, 0x2a6d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca2, 15, 0, 0xc601);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca3, 15, 0, 0x2a4c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca4, 15, 0, 0xc602);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca5, 15, 0, 0x0111);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca6, 15, 0, 0xc60c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca7, 15, 0, 0x5900);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca8, 15, 0, 0xc710);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cca9, 15, 0, 0x0700);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccaa, 15, 0, 0xc718);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccab, 15, 0, 0x0700);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccac, 15, 0, 0xc720);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccad, 15, 0, 0x4700);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccae, 15, 0, 0xc801);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccaf, 15, 0, 0x7f50);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb0, 15, 0, 0xc802);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb1, 15, 0, 0x7760);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb2, 15, 0, 0xc803);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb3, 15, 0, 0x7fce);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb4, 15, 0, 0xc804);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb5, 15, 0, 0x5700);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb6, 15, 0, 0xc805);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb7, 15, 0, 0x5f11);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb8, 15, 0, 0xc806);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccb9, 15, 0, 0x4751);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccba, 15, 0, 0xc807);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccbb, 15, 0, 0x57e1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccbc, 15, 0, 0xc808);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccbd, 15, 0, 0x2700);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccbe, 15, 0, 0xc809);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccbf, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc0, 15, 0, 0xc821);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc1, 15, 0, 0x0002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc2, 15, 0, 0xc822);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc3, 15, 0, 0x0014);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc4, 15, 0, 0xc832);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc5, 15, 0, 0x1186);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc6, 15, 0, 0xc847);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc7, 15, 0, 0x1e02);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc8, 15, 0, 0xc013);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccc9, 15, 0, 0xf341);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccca, 15, 0, 0xc01a);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cccb, 15, 0, 0x0446);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cccc, 15, 0, 0xc024);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cccd, 15, 0, 0x1000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccce, 15, 0, 0xc025);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cccf, 15, 0, 0x0a00);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd0, 15, 0, 0xc026);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd1, 15, 0, 0x0c0c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd2, 15, 0, 0xc027);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd3, 15, 0, 0x0c0c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd4, 15, 0, 0xc029);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd5, 15, 0, 0x00a0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd6, 15, 0, 0xc030);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd7, 15, 0, 0x0a00);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd8, 15, 0, 0xc03c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccd9, 15, 0, 0x001c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccda, 15, 0, 0xc005);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccdb, 15, 0, 0x7a06);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccdc, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccdd, 15, 0, 0x2731);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccde, 15, 0, 0x3011);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccdf, 15, 0, 0x1001);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce0, 15, 0, 0xc620);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce1, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce2, 15, 0, 0xc621);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce3, 15, 0, 0x003f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce4, 15, 0, 0xc622);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce5, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce6, 15, 0, 0xc623);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce7, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce8, 15, 0, 0xc624);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cce9, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccea, 15, 0, 0xc625);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cceb, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccec, 15, 0, 0xc627);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cced, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccee, 15, 0, 0xc628);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccef, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf0, 15, 0, 0xc62c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf1, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf2, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf3, 15, 0, 0x2806);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf4, 15, 0, 0x3cb6);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf5, 15, 0, 0xc161);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf6, 15, 0, 0x6134);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf7, 15, 0, 0x6135);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf8, 15, 0, 0x5443);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccf9, 15, 0, 0x0303);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccfa, 15, 0, 0x6524);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccfb, 15, 0, 0x000b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccfc, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccfd, 15, 0, 0x2104);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccfe, 15, 0, 0x3c24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ccff, 15, 0, 0x2105);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd00, 15, 0, 0x3805);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd01, 15, 0, 0x6524);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd02, 15, 0, 0xdff4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd03, 15, 0, 0x4005);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd04, 15, 0, 0x6524);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd05, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd06, 15, 0, 0x5dd3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd07, 15, 0, 0x0306);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd08, 15, 0, 0x2ff7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd09, 15, 0, 0x38f7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd0a, 15, 0, 0x60b7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd0b, 15, 0, 0xdffd);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd0c, 15, 0, 0x000a);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd0d, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd0e, 15, 0, 0x0000);

  GFA_BI_MDIO_SET(phy_ndx, 0x1ca00, 15, 0, 0x0080);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 15, 0, 0x0000);

  GFA_BI_AELUROS_MICRO_CONTROLLER_END(phy_ndx);

  GFA_BI_MDIO_SET(phy_ndx, 0x1c012, 15, 0, 0x0000);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_bi_phy_nlp1042_microcode_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_ndx 
  )
{
  uint32
    err = SOC_SAND_OK;
  uint16 
    data;  

  UTILS_INIT_ERR_DEFS("gfa_bi_phy_nlp1042_microcode_init");
/* 
 *  NLP1042 RevC2 rxaui init combined disparity mode (code name soc_sand) w/o xbar init
 */
    
  /* PART 1: nlp_init */
  
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca42, 10, 8, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca44, 15, 12, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca44, 9, 5, 0x10);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca46, 8, 8, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca46, 11, 9, 0x4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0e, 13, 12, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca4c, 5, 0, 0x2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca12, 10, 8, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca14, 15, 12, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca14, 9, 5, 0x10);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca16, 8, 8, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca16, 11, 9, 0x4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb06, 13, 12, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca1c, 5, 0, 0x2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca42, 13, 11, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca43, 14, 12, 0x2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c246, 9, 7, 0x3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c242, 2, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c240, 7, 4, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c240, 13, 11, 0x7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c242, 15, 13, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c246, 0, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c246, 1, 1, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c243, 4, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c243, 10, 5, 0x3f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c243, 15, 11, 0x2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0b, 1, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0b, 5, 4, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0b, 9, 8, 0x3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0b, 11, 10, 0x3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0b, 13, 12, 0x3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb0b, 15, 14, 0x3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc00, 7, 0, 0xf6);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc02, 7, 7, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc02, 7, 7, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc07, 4, 0, 0x17);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc08, 5, 0, 0x1f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc08, 11, 6, 0x1f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc09, 5, 0, 0x1f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc09, 11, 6, 0x1f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc07, 8, 5, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 0, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 1, 1, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 2, 2, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 0, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 1, 1, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 2, 2, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc02, 5, 5, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc04, 0, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc04, 8, 1, 0x5a);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc11, 5, 3, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc11, 8, 6, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc11, 11, 9, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc11, 14, 12, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc11, 2, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb02, 7, 4, 0xf);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb02, 3, 0, 0xf);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb21, 15, 12, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca01, 0, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca01, 7, 1, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cb1b, 15, 12, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca51, 0, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca51, 7, 1, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2c3, 3, 0, 0xf);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2ca, 15, 11, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2d0, 15, 11, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2d6, 15, 11, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2dc, 15, 11, 0x8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2f1, 5, 1, 0xa);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2f1, 8, 6, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2f5, 5, 1, 0xa);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2f5, 8, 6, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2f9, 5, 1, 0xa);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2f9, 8, 6, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2fd, 5, 1, 0xa);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2fd, 8, 6, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e8, 8, 0, 0x100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e9, 8, 0, 0x100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2ea, 8, 0, 0x100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2eb, 8, 0, 0x100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e6, 11, 11, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e6, 11, 11, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c010, 14, 14, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ff2a, 15, 0, 0x4a);

  /*  PART 2: uc code  ---------------------------------- */

  /*  cmdUC processing binary and source file... */
  /*  cmdUC Writing binary into memory... */
  GFA_BI_MDIO_SET(phy_ndx, 0x1d008, 0, 0, 0x1);
  /*  config uC */
  soc_sand_os_task_delay_milisec(100); /*  100ms */
  GFA_BI_MDIO_SET(phy_ndx, 0x1d000, 15, 0, 0x5200);
  soc_sand_os_task_delay_milisec(100); /*  100ms */
  /*  writing binary into uC SRAM... */
  GFA_BI_MDIO_SET(phy_ndx, 0x1d800, 15, 0, 0x2fff);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d801, 15, 0, 0x300f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d802, 15, 0, 0x220e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d803, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d804, 15, 0, 0x2124);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d805, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d806, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d807, 15, 0, 0x23fe);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d808, 15, 0, 0x3c1e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d809, 15, 0, 0x2214);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d80a, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d80b, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d80c, 15, 0, 0x20a4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d80d, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d80e, 15, 0, 0x2dfe);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d80f, 15, 0, 0x307e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d810, 15, 0, 0x6e24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d811, 15, 0, 0x6e24);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d812, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d813, 15, 0, 0x20e4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d814, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d815, 15, 0, 0x402e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d816, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d817, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d818, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d819, 15, 0, 0x2014);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d81a, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d81b, 15, 0, 0x64de);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d81c, 15, 0, 0x6e8f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d81d, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d81e, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d81f, 15, 0, 0x2044);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d820, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d821, 15, 0, 0x64de);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d822, 15, 0, 0x6e8f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d823, 15, 0, 0x201e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d824, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d825, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d826, 15, 0, 0x20d4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d827, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d828, 15, 0, 0x64de);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d829, 15, 0, 0x6e8f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d82a, 15, 0, 0x21fe);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d82b, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d82c, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d82d, 15, 0, 0x20e4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d82e, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d82f, 15, 0, 0x404e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d830, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d831, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d832, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d833, 15, 0, 0x21f5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d834, 15, 0, 0x3005);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d835, 15, 0, 0xb805);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d836, 15, 0, 0x8556);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d837, 15, 0, 0x8557);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d838, 15, 0, 0x8558);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d839, 15, 0, 0x8559);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d83a, 15, 0, 0x855a);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d83b, 15, 0, 0x400d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d83c, 15, 0, 0x6d8f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d83d, 15, 0, 0x2dd2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d83e, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d83f, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d840, 15, 0, 0x2ed2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d841, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d842, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d843, 15, 0, 0x2f62);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d844, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d845, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d846, 15, 0, 0x20a2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d847, 15, 0, 0x3022);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d848, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d849, 15, 0, 0x2142);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d84a, 15, 0, 0x3022);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d84b, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d84c, 15, 0, 0x2262);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d84d, 15, 0, 0x3022);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d84e, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d84f, 15, 0, 0x2302);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d850, 15, 0, 0x3022);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d851, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d852, 15, 0, 0x6f7e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d853, 15, 0, 0x4004);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d854, 15, 0, 0xb814);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d855, 15, 0, 0x5e43);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d856, 15, 0, 0x3d7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d857, 15, 0, 0x2dd2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d858, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d859, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d85a, 15, 0, 0x200e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d85b, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d85c, 15, 0, 0x2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d85d, 15, 0, 0xd01e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d85e, 15, 0, 0x6e8f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d85f, 15, 0, 0x20fe);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d860, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d861, 15, 0, 0xb80e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d862, 15, 0, 0xd01d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d863, 15, 0, 0x5de3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d864, 15, 0, 0x240e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d865, 15, 0, 0x301e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d866, 15, 0, 0x135e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d867, 15, 0, 0x6f7e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d868, 15, 0, 0x6f7e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d869, 15, 0, 0x20d4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d86a, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d86b, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d86c, 15, 0, 0x20e4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d86d, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d86e, 15, 0, 0x404e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d86f, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d870, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d871, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d872, 15, 0, 0x6f7e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d873, 15, 0, 0x2044);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d874, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d875, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d876, 15, 0, 0x6f7e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d877, 15, 0, 0x2014);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d878, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d879, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d87a, 15, 0, 0x200e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d87b, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d87c, 15, 0, 0x2124);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d87d, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d87e, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d87f, 15, 0, 0x2504);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d880, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d881, 15, 0, 0x4015);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d882, 15, 0, 0x65c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d883, 15, 0, 0x2514);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d884, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d885, 15, 0, 0x64d5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d886, 15, 0, 0xb145);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d887, 15, 0, 0xb115);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d888, 15, 0, 0x65c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d889, 15, 0, 0x29f2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d88a, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d88b, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d88c, 15, 0, 0x678f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d88d, 15, 0, 0x2514);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d88e, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d88f, 15, 0, 0x64d5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d890, 15, 0, 0xb145);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d891, 15, 0, 0xb105);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d892, 15, 0, 0x65c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d893, 15, 0, 0x29f2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d894, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d895, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d896, 15, 0, 0x6f78);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d897, 15, 0, 0xe78e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d898, 15, 0, 0x2d02);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d899, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d89a, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d89b, 15, 0, 0x2832);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d89c, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d89d, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d89e, 15, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d89f, 15, 0, 0x628f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a0, 15, 0, 0x4007);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a1, 15, 0, 0x2524);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a2, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a3, 15, 0, 0x64d5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a4, 15, 0, 0x2005);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a5, 15, 0, 0x9575);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a6, 15, 0, 0x65c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a7, 15, 0, 0x678f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a8, 15, 0, 0x2bf2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8a9, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8aa, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ab, 15, 0, 0x6f77);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ac, 15, 0, 0x2514);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ad, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ae, 15, 0, 0x64d5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8af, 15, 0, 0xbd05);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b0, 15, 0, 0xbf45);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b1, 15, 0, 0x2bd2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b2, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b3, 15, 0, 0x5553);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b4, 15, 0, 0x1302);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b5, 15, 0, 0x2006);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b6, 15, 0, 0x3016);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b7, 15, 0, 0x5763);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b8, 15, 0, 0x13c2);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8b9, 15, 0, 0xd017);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ba, 15, 0, 0x2a12);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8bb, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8bc, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8bd, 15, 0, 0x6f72);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8be, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8bf, 15, 0, 0x628f);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c0, 15, 0, 0x2514);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c1, 15, 0, 0x3cd4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c2, 15, 0, 0x64d5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c3, 15, 0, 0x4026);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c4, 15, 0, 0x9655);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c5, 15, 0, 0x65c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c6, 15, 0, 0x401d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c7, 15, 0, 0x2d22);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c8, 15, 0, 0x3012);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8c9, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ca, 15, 0, 0x2fd6);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8cb, 15, 0, 0x3ff6);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8cc, 15, 0, 0x8655);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8cd, 15, 0, 0x65c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ce, 15, 0, 0x6f72);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8cf, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d0, 15, 0, 0x200d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d1, 15, 0, 0x302d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d2, 15, 0, 0x2408);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d3, 15, 0, 0x35d8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d4, 15, 0, 0x5dd3);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d5, 15, 0, 0x307);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d6, 15, 0, 0x8887);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d7, 15, 0, 0x63a7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d8, 15, 0, 0x8887);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8d9, 15, 0, 0x63a7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8da, 15, 0, 0xdffd);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8db, 15, 0, 0xf9);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8dc, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8dd, 15, 0, 0x2214);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8de, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8df, 15, 0, 0x64de);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e0, 15, 0, 0x2ef4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e1, 15, 0, 0x3ff4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e2, 15, 0, 0x8e4e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e3, 15, 0, 0x2214);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e4, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e5, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e6, 15, 0, 0x2104);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e7, 15, 0, 0x3004);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e8, 15, 0, 0x9e4e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8e9, 15, 0, 0x2214);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ea, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8eb, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ec, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ed, 15, 0, 0x2294);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ee, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ef, 15, 0, 0x64db);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f0, 15, 0, 0x8bbc);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f1, 15, 0, 0xb84b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f2, 15, 0, 0x300c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f3, 15, 0, 0xdf0b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f4, 15, 0, 0xdf0c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f5, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f6, 15, 0, 0xc5b5);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f7, 15, 0, 0xc6c6);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f8, 15, 0, 0x855e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8f9, 15, 0, 0xb84e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8fa, 15, 0, 0x866c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8fb, 15, 0, 0xb84c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8fc, 15, 0, 0xb60c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8fd, 15, 0, 0x9cee);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8fe, 15, 0, 0x20a4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d8ff, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d900, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d901, 15, 0, 0x20e4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d902, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d903, 15, 0, 0x202e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d904, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d905, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d906, 15, 0, 0x200e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d907, 15, 0, 0x300e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d908, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d909, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d90a, 15, 0, 0x22b4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d90b, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d90c, 15, 0, 0x64db);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d90d, 15, 0, 0x8bbc);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d90e, 15, 0, 0xb84b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d90f, 15, 0, 0xb80c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d910, 15, 0, 0xb84c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d911, 15, 0, 0xdf0b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d912, 15, 0, 0xdf0c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d913, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d914, 15, 0, 0xc7b7);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d915, 15, 0, 0xc8c8);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d916, 15, 0, 0x877e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d917, 15, 0, 0xb84e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d918, 15, 0, 0x888c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d919, 15, 0, 0xb84c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d91a, 15, 0, 0xb60c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d91b, 15, 0, 0x9cee);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d91c, 15, 0, 0x20b4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d91d, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d91e, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d91f, 15, 0, 0x20e4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d920, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d921, 15, 0, 0x402e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d922, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d923, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d924, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d925, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d926, 15, 0, 0x22a4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d927, 15, 0, 0x3ca4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d928, 15, 0, 0x64db);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d929, 15, 0, 0x8bbc);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d92a, 15, 0, 0xb84b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d92b, 15, 0, 0xb80c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d92c, 15, 0, 0xb84c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d92d, 15, 0, 0xdf0b);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d92e, 15, 0, 0xdf0c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d92f, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d930, 15, 0, 0xc9b9);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d931, 15, 0, 0xcaca);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d932, 15, 0, 0x899e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d933, 15, 0, 0xb84e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d934, 15, 0, 0x8aac);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d935, 15, 0, 0xb84c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d936, 15, 0, 0xb60c);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d937, 15, 0, 0x9cee);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d938, 15, 0, 0x20c4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d939, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d93a, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d93b, 15, 0, 0x20e4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d93c, 15, 0, 0x3cc4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d93d, 15, 0, 0x402e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d93e, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d93f, 15, 0, 0x400e);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d940, 15, 0, 0x6ec4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d941, 15, 0, 0x1002);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d942, 15, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d080, 15, 0, 0x100);
  GFA_BI_MDIO_SET(phy_ndx, 0x1d092, 15, 0, 0x0);
  /*  done with uC binary */
  /*  cmdUC verifying binary... */
  soc_sand_os_task_delay_milisec(200);  /*  wait for uC to settle offset calibration */

  GFA_BI_MDIO_SET(phy_ndx, 0x1cc08, 5, 0, 0x6);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc08, 11, 6, 0x23);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc09, 5, 0, 0x28);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc09, 11, 6, 0x2d);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 0, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cc0e, 0, 0, 0x0);
  /*  end of s_eepromLoad */

  /*  PART 3: speed mode -------------------------------- */

  /*  Evaluating: 'rxaui_init ' */
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e1, 3, 0, 0xf);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c088, 3, 0, 0x4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c290, 0, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca01, 7, 1, 0x20);
  GFA_BI_MDIO_SET(phy_ndx, 0x1ca51, 7, 1, 0x20);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e8, 8, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e9, 8, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2ea, 8, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2eb, 8, 0, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e6, 11, 11, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e6, 11, 11, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e0, 7, 0, 0xe4);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c2e1, 13, 12, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c20d, 9, 8, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c012, 3, 0, 0xa);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c012, 7, 4, 0xa);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c017, 13, 13, 0x0);
  GFA_BI_MDIO_SET(phy_ndx, 0x1cd40, 3, 0, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c010, 13, 13, 0x1);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c0f0, 15, 0, 0x0103);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c0f1, 15, 0, 0x0056);
  GFA_BI_MDIO_SET(phy_ndx, 0x1c20d, 1, 1, 0x1);
  soc_sand_os_task_delay_milisec(200);  /*  sleep 200ms */

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_bi_phy_single_aeluros_init(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  phy_ndx
  )
{
    uint32
        err = SOC_SAND_OK;
    uint16 
        data;  

  UTILS_INIT_ERR_DEFS("gfa_bi_phy_single_aeluros_init");

  if(gfa_bi_phy_aeluros_board_configuration_init(unit, phy_ndx))
  {
    return SOC_SAND_ERR;
  }

  if(gfa_bi_phy_aeluros_microcode_init(unit, phy_ndx))
  {
    return SOC_SAND_ERR;
  }

    /* Added configuration */
    /* Fixes polarity swaps that are NR in Gfa-Bi */
    GFA_BI_MDIO_SET(phy_ndx, 0x1c001, 15, 0, 0x0408);

  soc_sand_os_printf("    PHY %d (single AEL2005) - loaded\n", phy_ndx);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_bi_phy_single_nlp1042_init(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  phy_ndx
  )
{

  if(gfa_bi_phy_nlp1042_microcode_init(unit, phy_ndx))
  {
    return SOC_SAND_ERR;
  }

  soc_sand_os_printf("    PHY %d (quad NLP1042) - loaded (uC v1.3)\n", phy_ndx);

  return SOC_SAND_OK;
}
                        

/* } */                                                             

