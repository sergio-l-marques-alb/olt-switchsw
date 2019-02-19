/* $Id: utils_line_gfa_petra.c,v 1.14 Broadcom SDK $
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

#include <sal/types.h>
#include <sal/appl/i2c.h>
#include <soc/i2c.h>
#include <sal/appl/io.h>
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
#include "Utilities/include/utils_line_gfa_bi.h"
#include "Utilities/include/utils_line_PTG.h"
#include "Utilities/include/utils_i2c_mem.h"

#ifdef LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
  #include "Utilities/include/utils_line_TEVB.h"
#endif  

#include "UserInterface/include/ui_pure_defi.h"  
#include "UserInterface/include/ui_cli_files.h"

#include "Pub/include/utils_defx.h"

#include "DuneDriver/SAND/SAND_FM/include/sand_chip_defines.h"
#include "DuneDriver/SAND/Utils/include/sand_os_interface.h"
#include "DuneDriver/SAND/Utils/include/sand_framework.h"
#include "DuneDriver/Petra/include/petra_api_general.h"

#include "Bsp_drv/include/bsp_cards_consts.h"

#include "Background/include/bckg_defi.h"

#include "../../../../h/usrApp.h"      

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

#ifndef DUNE_BCM
#else
sal_mutex_t  I2c_bus_semaphore = NULL;
#define test_d_printf sal_printf
#define soc_sand_os_printf sal_printf
#ifndef WAIT_FOREVER
#define WAIT_FOREVER -1
#endif
#endif

/* CPU Mezzanine { */
#define GFA_PETRA_BASE_REGISTER_04                  (0x40007001)
#define GFA_PETRA_OPTION_REGISTER_04                (0xFF8007FA)
#define GFA_PETRA_BASE_REGISTER_05                  (0x50007001)
#define GFA_PETRA_OPTION_REGISTER_05                (0xFF0007FA)
#define GFA_PETRA_OR_RELAXED_TIMING                 (0x00000004)

#ifdef GFA_PETRA_BACK_END_IS_PETRA_GFA
  #define GFA_PETRA_BASE_REGISTER_08_PORT_SIZE      (0x00000400)
  #define GFA_PETRA_BASE_REGISTER_16_PORT_SIZE      (0x00000800)
  #define GFA_PETRA_BASE_REGISTER_32_PORT_SIZE      (0x00000000)
#else
  #define GFA_PETRA_BASE_REGISTER_08_PORT_SIZE      (0x00000000)
  #define GFA_PETRA_BASE_REGISTER_16_PORT_SIZE      (0x00000000)
  #define GFA_PETRA_BASE_REGISTER_32_PORT_SIZE      (0x00000000)
#endif

/* } CPU Mezzanine */

#define GFA_PETRA_FPGA_IO_BASE_OFFSET             ((sizeof(value) - sizeof(uint8)) * SOC_SAND_NOF_BITS_IN_BYTE)

/* SYNTHESIZER Defs*/
#define SYNTHESIZER_ID        0x60
#define SYNTHESIZER_REG7TO12  0x07
#define SYNTHESIZER_REG135    0x87
#define SYNTHESIZER_REG137    0x89

#define GFA_PETRA_SCREENING_FLAVOR                (SOC_D_USR_APP_FLAVOR_MINIMAL)
#define GFA_PETRA_DRAM_SCREENING_FREQ             (533000000)


#define GFA_PETRA_DFFS_INIT                       (FALSE)


#ifndef __DUNE_GTO_BCM_CPU__
#define GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT 2500
#else
#define GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT 1500
#endif

/* } */                                                             
                                                                    
/*************                                                      
 *  MACROS   *                                                      
 *************/                                                     
/* { */                                                             
    
#define GFA_PETRA_FPGA_IO_FLD_MASK(msb, lsb)            \
          ((2 * SOC_SAND_BIT(msb)) - (SOC_SAND_BIT(lsb)))

#define GFA_PETRA_FPGA_IO_FLD_SHFT(msb, lsb)            \
          (lsb)

#define GFA_PETRA_FPGA_IO_FLD_ZERO_MASK(msb, lsb)       \
          (~GFA_PETRA_FPGA_IO_FLD_MASK(msb, lsb))

#define GFA_PETRA_FPGA_IO_FLD_GET(val, msb, lsb)        \
          (((val) & (GFA_PETRA_FPGA_IO_FLD_MASK(msb, lsb))) >> (GFA_PETRA_FPGA_IO_FLD_SHFT(msb, lsb)))

#define GFA_PETRA_FPGA_IO_FLD_SET(val, msb, lsb)        \
          (((val) << (GFA_PETRA_FPGA_IO_FLD_SHFT(msb, lsb))) & (GFA_PETRA_FPGA_IO_FLD_MASK(msb, lsb)))

#define GFA_PETRA_FPGA_I2C_WAIT                         \
          do                                            \
          {                                             \
          } while(!gfa_petra_board_fpga_i2c_ready())

#define GFA_PETRA_IS_PB_BOARD ((Gfa_petra_board_info.card_type == LOAD_BOARD_PB) ||\
       (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_DDR3) || \
       (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_DDR3_STREAMING) ||\
       (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3) ||\
       (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING)\
       )

/* } */                                                             
                                                                    
/*************                                                      
 * TYPE DEFS *                                                      
 *************/                                                     
/* { */                                                             
      
typedef struct  
{
  uint32 br4;
  uint32 or4;
  uint32 br5;
  uint32 or5;
}__ATTRIBUTE_PACKED__ GFA_PETRA_CPU_PORT_SIZE;

typedef struct
{

  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ version;                        /* 0x00 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ scratch_test_register;          /* 0x04 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ puc_0;                         /* 0x08 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR core_pll_n;
    GFA_PETRA_FLD_ADDR core_pll_p;
    GFA_PETRA_FLD_ADDR i2c_read_mux;
  } __ATTRIBUTE_PACKED__ puc_1;                         /* 0x0c */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR core_pll_m;
  } __ATTRIBUTE_PACKED__ puc_2;                         /* 0x10 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR streaming_mode_enable;
    GFA_PETRA_FLD_ADDR flash_address_a;
    GFA_PETRA_FLD_ADDR soc_petra_qdr_ref_clock_select;
    GFA_PETRA_FLD_ADDR soc_petra_core_pll_bypass;
    GFA_PETRA_FLD_ADDR soc_petra_ddr_pll_bypass;
    GFA_PETRA_FLD_ADDR soc_petra_qdr_pll_bypass;
    GFA_PETRA_FLD_ADDR soc_petra_pll_reset;
    GFA_PETRA_FLD_ADDR soc_petra_hardware_reset;
  } __ATTRIBUTE_PACKED__ soc_petra_miscelanous_controls_1;   /* 0x14 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_mux_select;                 /* 0x28 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_mux_config;                 /* 0x2c */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_address_command;            /* 0x30 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_data_tx_rx;                 /* 0x34 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_status;                     /* 0x38 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ i2c_triggers;                   /* 0x3c */
  struct
  {
  GFA_PETRA_FLD_ADDR addr;
  GFA_PETRA_FLD_ADDR combo_links_of_nif_a_path_select;
  GFA_PETRA_FLD_ADDR combo_links_of_nif_b_path_select;
  } __ATTRIBUTE_PACKED__ combo_switches_controls;        /* 0x44 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
  } __ATTRIBUTE_PACKED__ ddr_synt_freq;                  /* 0x58 */
  struct
  {
    GFA_PETRA_FLD_ADDR addr;
    GFA_PETRA_FLD_ADDR pll_rst;
  } __ATTRIBUTE_PACKED__ synce_pll_ctrl;                 /* 0x6c */



} __ATTRIBUTE_PACKED__ GFA_PETRA_FPGA_IO_REGS;

typedef uint8 (*GFA_PETRA_FPGA_IO_FIELD_GET) (SOC_SAND_IN GFA_PETRA_FLD_ADDR *);
typedef void (*GFA_PETRA_FPGA_IO_FIELD_SET) (SOC_SAND_IN GFA_PETRA_FLD_ADDR *, uint8);

typedef struct  
{
  SOC_BSP_CARDS_DEFINES           card_type;
  GFA_PETRA_CPU_PORT_SIZE     cpu_port_size;
  char                        *agent_file;
  char                        *streaming_file;
  GFA_PETRA_FPGA_IO_FIELD_GET field_get_func;
  GFA_PETRA_FPGA_IO_FIELD_SET field_set_func;
  SOC_PETRA_DRAM_TYPE          dram_type;

  int 
    (*i2c_syntmuxset) (
      SOC_SAND_IN GFA_TG_SYNT_TYPE     target
    );

  int  
    (*i2c_read) (
      uint8            unit,
      uint8            internal_address,
      uint8            buffer_size,
      uint8           *buffer
    );
  int 
    (*i2c_write) (
      uint8            unit,
      uint8            internal_address,
      uint8            buffer_size,
      uint8           *buffer
    );

}__ATTRIBUTE_PACKED__ GFA_PETRA_BOARD_INFO;

/* } */                                                             
                                                                    
/*************                                                      
 * GLOBALS   *                                                      
 *************/                                                     
/* { */                                                             

#ifndef __DUNE_GTO_BCM_CPU__
static DFFS_LOW_FLASH_INFO_TYPE 
  Gfa_petra_flash_info;

static DFFS_DESCRIPTOR_TYPE
  Gfa_petra_descriptor;

static uint8
  Gfa_petra_is_initialized = FALSE;
#endif

uint32 
  Gfa_petra_device_ver = 0;

static GFA_PETRA_BOARD_INFO
  Gfa_petra_board_info;
  
static GFA_PETRA_FPGA_IO_REGS
  Gfa_petra_fpga_io_regs;

static uint8
  Gfa_petra_fpga_io_regs_init;

static uint32 Gfa_petra_synt_values[GFA_NOF_SYNT_TYPES] = {0};

/* } */                                                             
                                                                    
/*************                                                      
 * FUNCTIONS *                                                      
 *************/                                                     
/* { */                                                             
  
 
static
  int 
    lb_petra_i2c_syntmuxset (
      SOC_SAND_IN GFA_TG_SYNT_TYPE     target
    );


int
  lb_petra_i2c_write(
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8           *buffer
  );

 
int 
  gfa_petra_board_fpga_i2c_syntmuxset (
    SOC_SAND_IN GFA_TG_SYNT_TYPE     target
  );

SOC_SAND_RET
  gfa_petra_fpga_io_regs_init(void)
{
  if (!Gfa_petra_fpga_io_regs_init)
  {
    Gfa_petra_fpga_io_regs.version.addr.address = 0x00;
    Gfa_petra_fpga_io_regs.version.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.version.addr.msb = 7;

    Gfa_petra_fpga_io_regs.scratch_test_register.addr.address = 0x04;
    Gfa_petra_fpga_io_regs.scratch_test_register.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.scratch_test_register.addr.msb = 7;

    Gfa_petra_fpga_io_regs.puc_0.addr.address = 0x08;
    Gfa_petra_fpga_io_regs.puc_0.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.puc_0.addr.msb = 7;

    Gfa_petra_fpga_io_regs.puc_1.addr.address = 0x0c;
    Gfa_petra_fpga_io_regs.puc_1.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.puc_1.addr.msb = 7;

    Gfa_petra_fpga_io_regs.puc_1.core_pll_n.address = 0x0c;
    Gfa_petra_fpga_io_regs.puc_1.core_pll_n.lsb = 0;
    Gfa_petra_fpga_io_regs.puc_1.core_pll_n.msb = 1; /* Also one bit from puc_2, currently ignored */

    Gfa_petra_fpga_io_regs.puc_1.core_pll_p.address = 0x0c;
    Gfa_petra_fpga_io_regs.puc_1.core_pll_p.lsb = 2;
    Gfa_petra_fpga_io_regs.puc_1.core_pll_p.msb = 4;
    Gfa_petra_fpga_io_regs.puc_1.i2c_read_mux.address = 0x0c;
    Gfa_petra_fpga_io_regs.puc_1.i2c_read_mux.lsb = 7;
    Gfa_petra_fpga_io_regs.puc_1.i2c_read_mux.msb = 7;

    Gfa_petra_fpga_io_regs.puc_2.core_pll_m.address = 0x10;
    Gfa_petra_fpga_io_regs.puc_2.core_pll_m.lsb = 0;
    Gfa_petra_fpga_io_regs.puc_2.core_pll_m.msb = 3; /* Also one bit from puc_2, currently ignored */

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.addr.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.addr.msb = 7;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.streaming_mode_enable.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.streaming_mode_enable.lsb = 7;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.streaming_mode_enable.msb = 7;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.flash_address_a.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.flash_address_a.lsb = 6;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.flash_address_a.msb = 6;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_ref_clock_select.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_ref_clock_select.lsb = 5;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_ref_clock_select.msb = 5;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_core_pll_bypass.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_core_pll_bypass.lsb = 4;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_core_pll_bypass.msb = 4;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_ddr_pll_bypass.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_ddr_pll_bypass.lsb = 3;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_ddr_pll_bypass.msb = 3;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_pll_bypass.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_pll_bypass.lsb = 2;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_qdr_pll_bypass.msb = 2;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset.lsb = 1;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset.msb = 1;

    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset.address = 0x14;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset.lsb = 0;
    Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset.msb = 0;

    Gfa_petra_fpga_io_regs.i2c_mux_select.addr.address = 0x28;
    Gfa_petra_fpga_io_regs.i2c_mux_select.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.i2c_mux_select.addr.msb = 7;

    Gfa_petra_fpga_io_regs.i2c_mux_config.addr.address = 0x2c;
    Gfa_petra_fpga_io_regs.i2c_mux_config.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.i2c_mux_config.addr.msb = 7;

    Gfa_petra_fpga_io_regs.i2c_address_command.addr.address = 0x30;
    Gfa_petra_fpga_io_regs.i2c_address_command.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.i2c_address_command.addr.msb = 7;

    Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr.address = 0x34;
    Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr.msb = 7;

    Gfa_petra_fpga_io_regs.i2c_status.addr.address = 0x38;
    Gfa_petra_fpga_io_regs.i2c_status.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.i2c_status.addr.msb = 7;

    Gfa_petra_fpga_io_regs.i2c_triggers.addr.address = 0x3c;
    Gfa_petra_fpga_io_regs.i2c_triggers.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.i2c_triggers.addr.msb = 7;

    Gfa_petra_fpga_io_regs.combo_switches_controls.addr.address = 0x44;
    Gfa_petra_fpga_io_regs.combo_switches_controls.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.combo_switches_controls.addr.msb = 7;

    Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select.address = 0x44;
    Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select.lsb = 0;
    Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select.msb = 0;

    Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select.address = 0x44;
    Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select.lsb = 1;
    Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select.msb = 1;

    Gfa_petra_fpga_io_regs.ddr_synt_freq.addr.address = 0x58;
    Gfa_petra_fpga_io_regs.ddr_synt_freq.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.ddr_synt_freq.addr.msb = 7;

    Gfa_petra_fpga_io_regs.synce_pll_ctrl.addr.address = 0x6c;
    Gfa_petra_fpga_io_regs.synce_pll_ctrl.addr.lsb = 0;
    Gfa_petra_fpga_io_regs.synce_pll_ctrl.addr.msb = 7;

    Gfa_petra_fpga_io_regs.synce_pll_ctrl.pll_rst.address = 0x6c;
    Gfa_petra_fpga_io_regs.synce_pll_ctrl.pll_rst.lsb = 0;
    Gfa_petra_fpga_io_regs.synce_pll_ctrl.pll_rst.msb = 0;

    Gfa_petra_fpga_io_regs_init = TRUE;
  }

  return SOC_SAND_OK;
}

#ifdef __DUNE_GTO_BCM_CPU__
void
  gfa_bi_fpga_io_32_fld_set(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  )
{
#ifndef __KERNEL__
  int
    value = 0;
  uint8
    val = 0;

  /* soc_sand_os_printf("gfa_bi_fpga_io_32_fld_set(). fld->address=0x%x, fld->msb=%d, ,fld->lsb=%d, data=0x%x\n",fld->address,fld->msb,fld->lsb,data); */

  cpu_i2c_read(0x41, fld->address, CPU_I2C_ALEN_LONG_DLEN_LONG, &value);

  val = (uint8)(value & 0xff);

  /* soc_sand_os_printf("gfa_bi_fpga_io_32_fld_set(). before: value=0x%x, val=0x%x;\n",value,val); */
      
  val &= SOC_SAND_ZERO_BITS_MASK(fld->msb,fld->lsb);
  val |= SOC_SAND_SET_BITS_RANGE(data,fld->msb,fld->lsb);
 
  /* soc_sand_os_printf("gfa_bi_fpga_io_32_fld_set(). fld->address=0x%x, fld->msb=%d, ,fld->lsb=%d, data=0x%x, val=0x%x\n",fld->address,fld->msb,fld->lsb,data,val);  */

  cpu_i2c_write(0x41, fld->address, CPU_I2C_ALEN_LONG_DLEN_LONG, val);
#endif /* __KERNEL__ */
  soc_sand_os_printf("This function is unavailable in Kernel mode\n");
}

uint8
  gfa_bi_fpga_io_32_fld_get(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld
  )
{
#ifndef __KERNEL__
  int
    value = 0;
  uint8
    val = 0, ret_val = 0;  

  cpu_i2c_read(0x41, fld->address, CPU_I2C_ALEN_LONG_DLEN_LONG, &value);

  val = (uint8)(value & 0xff);

  /* soc_sand_os_printf("gfa_bi_fpga_io_32_fld_get(). before: value=0x%x, val=0x%x\n",value,val); */

  ret_val = SOC_SAND_GET_BITS_RANGE(val,fld->msb,fld->lsb);

  /* soc_sand_os_printf("gfa_bi_fpga_io_32_fld_get(). fld->address=0x%x, fld->msb=%d, ,fld->lsb=%d, value=0x%x\n",fld->address,fld->msb,fld->lsb,ret_val); */

  return ret_val;
#endif /* __KERNEL__ */
  soc_sand_os_printf("This function is unavailable in Kernel mode\n");
  return 0;
}
#endif 


void
  gfa_petra_fpga_io_32_fld_set(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  )
{
  uint32
    value = 0;
  uint32
    data2set = data;
#ifndef WIN32
  value = *((volatile uint32 *)(GFA_PETRA_IO_FPGA_BASE_ADDR | fld->address));  
#endif
  value = value & SOC_SAND_ZERO_BITS_MASK(fld->msb + GFA_PETRA_FPGA_IO_BASE_OFFSET, fld->lsb + GFA_PETRA_FPGA_IO_BASE_OFFSET);
  value = value | GFA_PETRA_FPGA_IO_FLD_SET(data2set, fld->msb + GFA_PETRA_FPGA_IO_BASE_OFFSET, fld->lsb + GFA_PETRA_FPGA_IO_BASE_OFFSET);
#ifndef WIN32
  *((volatile uint32 *)(GFA_PETRA_IO_FPGA_BASE_ADDR | fld->address)) = value;  
#endif
}

uint8
  gfa_petra_fpga_io_32_fld_get(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld
  )
{
  uint32
    value = 0;
#ifndef WIN32
  value = *((volatile uint32 *)(GFA_PETRA_IO_FPGA_BASE_ADDR | fld->address));  
#endif
  value = GFA_PETRA_FPGA_IO_FLD_GET(value, fld->msb + GFA_PETRA_FPGA_IO_BASE_OFFSET, fld->lsb + GFA_PETRA_FPGA_IO_BASE_OFFSET);
  return ((uint8)value);
}

void
  gfa_petra_fpga_io_08_fld_set(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  )
{
  uint8
    value = 0;
  uint8
    data2set = data;
#ifndef WIN32
  value = *((volatile uint8 *)(GFA_PETRA_IO_FPGA_BASE_ADDR | fld->address));  
#endif
  value = value & GFA_PETRA_FPGA_IO_FLD_ZERO_MASK(fld->msb + GFA_PETRA_FPGA_IO_BASE_OFFSET, fld->lsb + GFA_PETRA_FPGA_IO_BASE_OFFSET);
  value = value | GFA_PETRA_FPGA_IO_FLD_SET(data2set, fld->msb + GFA_PETRA_FPGA_IO_BASE_OFFSET, fld->lsb + GFA_PETRA_FPGA_IO_BASE_OFFSET);
#ifndef WIN32
  *((volatile uint8 *)(GFA_PETRA_IO_FPGA_BASE_ADDR | fld->address)) = value;  
#endif
}

uint8
  gfa_petra_fpga_io_08_fld_get(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld
  )
{
  uint8
    value = 0;
#ifndef WIN32
  value = *((volatile uint8 *)(GFA_PETRA_IO_FPGA_BASE_ADDR | fld->address));  
#endif
  value = GFA_PETRA_FPGA_IO_FLD_GET(value, fld->msb + GFA_PETRA_FPGA_IO_BASE_OFFSET, fld->lsb + GFA_PETRA_FPGA_IO_BASE_OFFSET);
  return ((uint8)value);
}

uint8
  gfa_petra_fpga_io_fld_get(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld
  )
{
  return Gfa_petra_board_info.field_get_func(fld);
}

void
  gfa_petra_fpga_io_fld_set(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  )
{
  Gfa_petra_board_info.field_set_func(fld, data);
}

SOC_SAND_RET
  Gfa_petra_GFA_PETRA_BOARD_SPECIFICATIONS_clear(
    SOC_SAND_IN SOC_BSP_CARDS_DEFINES  card_type
  )
{
  Gfa_petra_board_info.card_type = card_type;

  switch(card_type)
  {
  case LINE_CARD_GFA_PETRA_DDR2:
    soc_sand_os_printf("+. GFA board flavor is ddr2\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_08_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_IO_AGENT_FPGA_FILE;
    Gfa_petra_board_info.streaming_file = GFA_PETRA_STREAMING_FPGA_FILE;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_08_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_08_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR2;
    break;
  case LINE_CARD_GFA_PETRA_DDR3:
    soc_sand_os_printf("+. GFA board flavor is ddr3\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_08_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_IO_AGENT_FPGA_FILE;
    Gfa_petra_board_info.streaming_file = GFA_PETRA_STREAMING_FPGA_FILE;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_08_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_08_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
    break;
  case LINE_CARD_GFA_PETRA_DDR3_STREAMING:
    soc_sand_os_printf("+. GFA board flavor is ddr3 streaming\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_IO_AGENT_FPGA_FILE_STREAMING;
    Gfa_petra_board_info.streaming_file = GFA_PETRA_STREAMING_FPGA_FILE_STREAMING;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_32_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_32_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
    break;
  case LINE_CARD_GFA_PETRA_B_DDR3:
    soc_sand_os_printf("+. GFA Soc_petra B board flavor is ddr3\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_08_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_B_IO_AGENT_FPGA_FILE;
    Gfa_petra_board_info.streaming_file = NULL;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_08_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_08_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
    break;
  case LINE_CARD_GFA_PETRA_B_DDR3_STREAMING:
    soc_sand_os_printf("+. GFA Soc_petra B board flavor is ddr3 streaming\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_B_IO_AGENT_FPGA_FILE_STREAMING;
    Gfa_petra_board_info.streaming_file = GFA_PETRA_B_STREAMING_FPGA_FILE_STREAMING;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_32_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_32_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
    break;
  case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
  case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
  #ifdef __DUNE_GTO_BCM_CPU__
  	soc_sand_os_printf("\tGFA Soc_petra B board flavor interlaken\n");
	Gfa_petra_board_info.field_get_func = gfa_bi_fpga_io_32_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_bi_fpga_io_32_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
  #else
    soc_sand_os_printf("+. GFA Soc_petra B board flavor interlaken\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_08_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_BI_IO_AGENT_FPGA_FILE_NAME;
    Gfa_petra_board_info.streaming_file = NULL;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_08_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_08_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
  #endif
    break;
  case LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3:
    soc_sand_os_printf("+. GFA Soc_petra B board flavor is ddr3 with Soc_petra A device\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_08_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_B_WITH_PETRA_A_IO_AGENT_FPGA_FILE;
    Gfa_petra_board_info.streaming_file = NULL;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_08_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_08_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
    break;
  case LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING:
    soc_sand_os_printf("+. GFA Soc_petra B board flavor is ddr3 streaming with Soc_petra A device \n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = GFA_PETRA_B_WITH_PETRA_A_IO_AGENT_FPGA_FILE_STREAMING;
    Gfa_petra_board_info.streaming_file = GFA_PETRA_STREAMING_FPGA_FILE_STREAMING;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_32_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_32_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR3;
    break;
  case LOAD_BOARD_PB:
    soc_sand_os_printf("+. Load Board soc_petra\n");
    Gfa_petra_board_info.cpu_port_size.br4 = GFA_PETRA_BASE_REGISTER_04 | GFA_PETRA_BASE_REGISTER_08_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or4 = GFA_PETRA_OPTION_REGISTER_04 | GFA_PETRA_OR_RELAXED_TIMING;
    Gfa_petra_board_info.cpu_port_size.br5 = GFA_PETRA_BASE_REGISTER_05 | GFA_PETRA_BASE_REGISTER_32_PORT_SIZE;
    Gfa_petra_board_info.cpu_port_size.or5 = GFA_PETRA_OPTION_REGISTER_05;
    Gfa_petra_board_info.agent_file = NULL;
    Gfa_petra_board_info.streaming_file = NULL;
    Gfa_petra_board_info.field_get_func = gfa_petra_fpga_io_08_fld_get;
    Gfa_petra_board_info.field_set_func = gfa_petra_fpga_io_08_fld_set;
    Gfa_petra_board_info.dram_type = SOC_PETRA_DRAM_TYPE_DDR2;

    break;
  default:
	soc_sand_os_printf("%s(): ERROR: Unknown/Not supported card type=%d\n", FUNCTION_NAME(), card_type);
    return SOC_SAND_ERR;
  }

  if(card_type == LOAD_BOARD_PB)
  {
    Gfa_petra_board_info.i2c_read = lb_petra_i2c_read;
    Gfa_petra_board_info.i2c_write = lb_petra_i2c_write;
    Gfa_petra_board_info.i2c_syntmuxset = lb_petra_i2c_syntmuxset;
  }
  else
  {
    Gfa_petra_board_info.i2c_read = gfa_petra_fpga_i2c_read;
    Gfa_petra_board_info.i2c_write = gfa_petra_fpga_i2c_write;
    Gfa_petra_board_info.i2c_syntmuxset = gfa_petra_board_fpga_i2c_syntmuxset;
  }
  return SOC_SAND_OK;
}

SOC_SAND_RET 
  gfa_petra_dram_type_get(
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE *dram_type
  )
{
  *dram_type = Gfa_petra_board_info.dram_type;
  return SOC_SAND_OK;
}

uint8
  gfa_petra_is_look_aside_get(void
  )
{
  return FALSE;
}

#ifdef NEGEV
SOC_SAND_RET 
  gfa_petra_init_host_board_860(void
  )
{
#ifdef __VXWORKS__
  uint32   
    reg_base,
    reg_vals[4],
    size[4], 
    offset[4],
    ii;
 
  reg_base = vxImmrGet();
  soc_sand_os_printf("   -. Initializing PPC860 to support GFA peripherals:\n");
  *BR4(reg_base)  = Gfa_petra_board_info.cpu_port_size.br4;
  soc_sand_os_printf("   -. Initialized BR04 0x%08lx        .....     pass\n", Gfa_petra_board_info.cpu_port_size.br4);
  *OR4(reg_base)  = Gfa_petra_board_info.cpu_port_size.or4;
  soc_sand_os_printf("   -. Initialized OR04 0x%08lx        .....     pass\n", Gfa_petra_board_info.cpu_port_size.or4);
  *BR5(reg_base)  = Gfa_petra_board_info.cpu_port_size.br5;
  soc_sand_os_printf("   -. Initialized BR05 0x%08lx        .....     pass\n", Gfa_petra_board_info.cpu_port_size.br5);
  *OR5(reg_base)  = Gfa_petra_board_info.cpu_port_size.or5;
  soc_sand_os_printf("   -. Initialized OR05 0x%08lx        .....     pass\n", Gfa_petra_board_info.cpu_port_size.or5);

  reg_vals[0] = Gfa_petra_board_info.cpu_port_size.br4;
  reg_vals[1] = Gfa_petra_board_info.cpu_port_size.or4;
  reg_vals[2] = Gfa_petra_board_info.cpu_port_size.br5;
  reg_vals[3] = Gfa_petra_board_info.cpu_port_size.or5;

  offset[0] = (uint32)((char *)&(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.base_register_04));
  size[0]   = sizeof(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.base_register_04);
  offset[1] = (uint32)((char *)&(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.option_register_04));
  size[1]   = sizeof(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.option_register_04);
  offset[2] = (uint32)((char *)&(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.base_register_05));
  size[2]   = sizeof(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.base_register_05);
  offset[3] = (uint32)((char *)&(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.option_register_05));
  size[3]   = sizeof(((EE_AREA *)0)->ee_block_02.un_ee_block_02.block_02.option_register_05);

  /* lets write all 4 registers at once */
  for (ii = 0; ii < 4; ++ii)
  {
#ifndef DUNE_BCM
    set_run_vals((char *)&(reg_vals[ii]), size[ii], offset[ii]);
#endif
  }
#endif

  return SOC_SAND_OK;
}
#endif /* NEGEV */

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_utils_dffs_get_flash_info(
    SOC_SAND_OUT DFFS_LOW_FLASH_INFO_TYPE   *flash_info
  )
{

  uint32
    dev_code = 0,
    fs_version = 0;
  uint32
    base_addr = 0,
    top_addr = 0,
    sector_size = 0,
    app_size = 0,
    fs_size = 0,
    fs_addr = 0,
    data_addr = 0;
    
  UTILS_INIT_ERR_DEFS("gfa_petra_utils_dffs_get_flash_info");

  if (flash_info == NULL)
  {
    UTILS_SET_ERR_AND_EXIT(5);
  }

  if (boot_get_fs_version==NULL)
  {
    UTILS_SET_ERR_AND_EXIT(20);
  }

  dev_code = GFA_PETRA_FILE_SYSTEM_FLASH_TYPE;
  base_addr = GFA_PETRA_FILE_SYSTEM_FLASH_ADDR;
  sector_size = GFA_PETRA_FILE_SYSTEM_FLASH_SECTOR_SIZE;

  fs_version = boot_get_fs_version();
  if (fs_version == BT_FS_VERSION_0)
  {
    app_size = B_DEFAULT_APP_FLASH_SIZE_8MEG;
  }
  else if (fs_version == BT_FS_VERSION_1)
  {
    app_size = B_DEFAULT_APP_FLASH_SIZE_8MEG_VER_C;
  }
  else
  {
    UTILS_SET_ERR_AND_EXIT(40);
  }

  app_size = 0;
  fs_addr = base_addr + app_size;

  /* make sure fs_addr is a multiple of sector_size */
  fs_addr = SOC_SAND_DIV_ROUND_UP(fs_addr, sector_size);
  fs_addr *= sector_size;
  
  /* 
   *fs descriptor takes one sector - the next sector if the base 
   *sector of the fs data 
   */
  data_addr = fs_addr + sector_size;

  top_addr = GFA_PETRA_FILE_SYSTEM_FLASH_MAX_ADDR;
  /* make sure top_addr is a multiple of sector_size */
  top_addr = SOC_SAND_DIV_ROUND_DOWN(top_addr, sector_size);
  top_addr *= sector_size;
  top_addr -= sector_size;
  
  fs_size = top_addr - data_addr + sector_size;

  if (fs_size <= sector_size)
  {
    UTILS_SET_ERR_AND_EXIT(50);
  }
  
  flash_info->descriptor_sector = (uint8)((fs_addr - base_addr)   / sector_size); 
  flash_info->first_data_sector = (uint8)((data_addr - base_addr) / sector_size);
  flash_info->last_data_sector  = (uint8)((top_addr - base_addr)  / sector_size);
  flash_info->dev_code          = dev_code;
  flash_info->sector_size       = sector_size;
  flash_info->use_bsp_driver    = 1;

  flash_info->bsp_flash_def_inf.f_type          = dev_code;
  flash_info->bsp_flash_def_inf.f_adrs          = base_addr;
  flash_info->bsp_flash_def_inf.f_adrs_jump     = GFA_PETRA_FILE_SYSTEM_FLASH_ADRS_JUMP;      
  flash_info->bsp_flash_def_inf.f_sector_size   = sector_size;
  flash_info->bsp_flash_def_inf.struct_version  = FLASH_DEF_INF_V1;
  flash_info->bsp_flash_def_inf.flash_width     = GFA_PETRA_FILE_SYSTEM_FLASH_WIDTH;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_petra_utils_dffs_init(
    SOC_SAND_IN  uint8                  silent
  )
{
  uint32
    err = SOC_SAND_OK;
  uint8
    signature_valid = FALSE,
    crc_valid = FALSE;
  uint8
    value = 0;
  uint32
    offset;

  UTILS_INIT_ERR_DEFS("gfa_petra_utils_dffs_init");

  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)|GFA_PETRA_DEVICE_RESET_FILE_SYSTEM_FLASH;
  write_epld_reg((uint8)value,(uint32)offset);

  err = gfa_petra_utils_dffs_get_flash_info(&(Gfa_petra_flash_info));
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 10, "No flash info");
 
  err = dffs_low_is_flash_invalid(
          (&Gfa_petra_flash_info)
        );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 20, "Invalid flash version");

  err = dffs_low_read_descriptor(
          (&Gfa_petra_flash_info),
          (char*)(&(Gfa_petra_descriptor)),
          TRUE
        );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 10, "Cant read descriptor");

  signature_valid = dffs_signature_valid(&(Gfa_petra_descriptor));

  if (signature_valid)
  {
    crc_valid = dffs_drv_desc_crc_is_valid(&(Gfa_petra_descriptor),TRUE);
  }

  if (!(signature_valid) || !(crc_valid))
  {
    if (!signature_valid)
    {
      UTILS_PRINT_MSG(silent,"DFFS not found on flash");
    }
    else if (!crc_valid)
    {
      UTILS_GEN_SVR_ERR(
        "DFFS CRC invalid - all DFFS data will be lost!!!",
        "gfa_petra_utils_dffs_init",
         DFFS_DESCRIPTOR_CRC_ERR,
         0
      );
    }
    
    UTILS_PRINT_MSG(silent,"Creating empty DFFS...");

    err = dffs_drv_set_defaults(
            &(Gfa_petra_flash_info),
            &(Gfa_petra_descriptor),
            TRUE
          );
    UTILS_EXIT_IF_ERR(err, 50);

    err = dffs_low_write_descriptor(
            &(Gfa_petra_flash_info),
            (char*)(&(Gfa_petra_descriptor)),
            TRUE
          );
    UTILS_EXIT_IF_ERR(err, 60);
    UTILS_PRINT_MSG(silent, "DFFS create - success");
  }
  else
  {
    UTILS_PRINT_MSG(silent,"DFFS found on flash");
  }
  Gfa_petra_is_initialized = TRUE;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}
#endif /* __DUNE_GTO_BCM_CPU__ */

SOC_SAND_RET 
  gfa_petra_burn_fpga_low(
    SOC_SAND_IN  uint8                    *mem_base,
    SOC_SAND_IN  uint32                   nof_bytes,
    SOC_SAND_IN  char                       *fpga_name,
    SOC_SAND_IN  uint32                  silent
  )
{ 
  char
    image_name[80];

  sal_sprintf(image_name, "   -. Download FPGA %s\n", fpga_name);

#ifndef SAND_LOW_LEVEL_SIMULATION
  if (soc_sand_os_mutex_take(I2c_bus_semaphore, WAIT_FOREVER))
  {
    return SOC_SAND_ERR;
  }

#endif
#ifndef __DUNE_GTO_BCM_CPU__
  if (utils_dune_AlteraDownload(mem_base, image_name, nof_bytes, UTILS_DUNE_FPGA_DOWNLOAD_V_2, TRUE))
  {
    return SOC_SAND_ERR;
  }
#endif
  
#ifndef SAND_LOW_LEVEL_SIMULATION
  soc_sand_os_mutex_give(I2c_bus_semaphore);
#endif

  return SOC_SAND_OK;
}

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET 
  gfa_petra_burn_fpga(
    SOC_SAND_IN uint8                     *mem_base,
    SOC_SAND_IN uint32                    gpio_value,
    SOC_SAND_IN uint32                    nof_bytes,
    SOC_SAND_IN  char                       *fpga_name,
    SOC_SAND_IN uint8                   silent
  )
{
  uint32
    nof_bytes_to_burn = nof_bytes;
  int 
    offset;

  offset = (uint32)(&(((EPLD_REGS *)0)->gp_io_conf));
  write_epld_reg((uint8)0xc3, (uint32)offset);

  offset = (uint32)(&(((EPLD_REGS *)0)->gp_bus_data));
  write_epld_reg((uint8)gpio_value, (uint32)offset);

  /* Burn FPGA */
  if (gfa_petra_burn_fpga_low(mem_base, nof_bytes_to_burn, fpga_name, TRUE))
  {
    return SOC_SAND_ERR;
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_petra_fpga_download_and_burn(
    SOC_SAND_IN  char                       *file_name,
    SOC_SAND_IN  uint8                    gpio_value,
    SOC_SAND_IN  uint8                  silent
  )
{
  int32
    file_handle;
  uint32
    delete_cli_file = FALSE;
  char
    err_msg[8*80] = "";
  uint32
    host_ip,
    file_size,
    max_file_size;
  uint8
    *file_mem_base;
  
  host_ip = 0; /* !DUNE_BCM utils_ip_get_dld_host_ip(); */

  max_file_size = GFA_PETRA_FILE_SYSTEM_FLASH_SIZE_TO_USE;

  if( download_cli_file(file_name, err_msg, DEFAULT_DOWNLOAD_IP) )
  {
    return SOC_SAND_ERR;
  }

  delete_cli_file = TRUE;

  if (get_cli_file_name_handle(file_name, err_msg, &file_handle) )
  {
    return SOC_SAND_ERR;
  }

  if (get_cli_file_size(file_handle, (void *)&file_size))
  {
    return SOC_SAND_ERR;
  }

  if (get_cli_file_mem_base(file_handle, (void *)&file_mem_base))
  {
    return SOC_SAND_ERR;
  }


  if(gfa_petra_burn_fpga(file_mem_base, gpio_value, file_size, file_name, TRUE))
  {
    return SOC_SAND_ERR;
  }
 
  if (delete_cli_file)
  {
    erase_cli_file(file_name,err_msg);
  }

  return SOC_SAND_OK;  
}

SOC_SAND_RET 
  gfa_petra_utils_dffs_get_file_size(
             char                       *file_name,
    SOC_SAND_OUT uint32                   *file_size,
    SOC_SAND_IN  uint8                  silent
  )
{
  uint32
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("gfa_petra_utils_dffs_get_file_size");

  err = dffs_drv_get_file_size(
          &(Gfa_petra_flash_info),
          &(Gfa_petra_descriptor),
          file_name,
          file_size,
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_petra_utils_dffs_file_exists(
             char                       *file_name,
    SOC_SAND_OUT uint32                  *does_file_exist,
    SOC_SAND_IN  uint8                  silent
  )
{
 uint32
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("gfa_petra_utils_dffs_get_file_size");

  err = dffs_drv_file_exists(
          &(Gfa_petra_descriptor),
          file_name,
          does_file_exist,
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET 
  gfa_petra_utils_dffs_file_from_flash(
             char*                      file_name,
    SOC_SAND_IN  uint32                   file_size,
    SOC_SAND_OUT char*                      file_buff,
    SOC_SAND_IN  uint8                  silent
  )
{
  uint32
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("gfa_petra_utils_dffs_file_from_flash");

  err = dffs_drv_file_from_flash(
          &(Gfa_petra_flash_info),
          &(Gfa_petra_descriptor),
          file_name,
          file_size,
          file_buff,
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET 
  gfa_petra_fpga_flash_burn(
             char                       *file_name,
    SOC_SAND_IN  uint8                    gpio_value,
    SOC_SAND_IN  uint8                  silent
  )
{
  uint32
    file_exists;
  char 
    *flash_data = NULL;
  uint32
    file_size = 0;
  uint32
    max_file_size = GFA_PETRA_FILE_SYSTEM_FLASH_SIZE;

  if (gfa_petra_utils_dffs_file_exists(file_name, &file_exists, silent))
  {
    return SOC_SAND_ERR;  
  }

  if (!file_exists)
  {
    return SOC_SAND_ERR;  
  }

  if(gfa_petra_utils_dffs_get_file_size(file_name, &file_size, silent))
  {
    return SOC_SAND_ERR;  
  }

  if (file_size>max_file_size)
  {
    return SOC_SAND_ERR;  
  }

  if (NULL == (flash_data = (char*)soc_sand_os_malloc_any_size(file_size)))
  {
    return SOC_SAND_ERR;  
  }  
  
  if(gfa_petra_utils_dffs_file_from_flash(file_name, file_size, flash_data, silent))
  {
    return SOC_SAND_ERR;  
  }

  if(gfa_petra_burn_fpga((uint8 *)flash_data, gpio_value, file_size, file_name, !silent))
  {
    return SOC_SAND_ERR;  
  }

  if (flash_data)
  {
    soc_sand_os_free_any_size(flash_data);
    flash_data = NULL;
  }
  return SOC_SAND_OK;
}
#endif /* __DUNE_GTO_BCM_CPU__ */

SOC_SAND_RET 
  gfa_petra_board_core_frequency_get(
    SOC_SAND_OUT uint32                   *dram_frequency
  )
{
  return SOC_SAND_OK;
}

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET 
  gfa_petra_streaming_agent_init(
    SOC_SAND_IN  uint8                  silent
  )
{
  const char
    *fpga_streaming_file_name = Gfa_petra_board_info.streaming_file;

  /* some boards do not have fpga - do not burn if fpga file name is not set */
  if(fpga_streaming_file_name != NULL)
  {
    if (Gfa_petra_board_info.card_type != LINE_CARD_GFA_PETRA_DDR2)
    {
      if (gfa_petra_fpga_download_and_burn(fpga_streaming_file_name, GFA_PETRA_STREAMING_FPGA_GPIO, silent))
      {
        return SOC_SAND_ERR;
      }
    }
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_petra_streaming_agent_start(void)
{
  uint8
    value = 0;
  uint32
    offset;

  if ( Gfa_petra_board_info.streaming_file != NULL)
  {
    offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
    value = read_epld_reg((uint32)offset)|GFA_PETRA_DEVICE_RESET_STREAMING_FPGA;
    write_epld_reg((uint8)value,(uint32)offset);
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET 
  gfa_petra_fpga_io_agent_init(
    SOC_SAND_IN  uint8                  silent
  )
{
  char
    *fpga_io_agent_file_name = Gfa_petra_board_info.agent_file;

  /* some boards do not have fpga - do not burn if fpga file name is not set */
  if(fpga_io_agent_file_name != NULL)
  {
    if (gfa_petra_fpga_download_and_burn(fpga_io_agent_file_name, GFA_PETRA_IO_AGENT_FPGA_GPIO, silent))
    {
      if (gfa_petra_fpga_flash_burn(fpga_io_agent_file_name, GFA_PETRA_IO_AGENT_FPGA_GPIO, silent))
      {
        return SOC_SAND_ERR;
      }
    }
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_petra_fpga_io_agent_start(
  )
{
  uint8
    value = 0;
  uint32
    offset;

  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)|GFA_PETRA_DEVICE_RESET_IO_AGENT_FPGA;
  write_epld_reg((uint8)value,(uint32)offset);

  sal_usleep(1000*16);

  if ((gfa_petra_fpga_io_fld_get(&Gfa_petra_fpga_io_regs.scratch_test_register.addr)) != 0x55)
  {
    return SOC_SAND_ERR;
  }

  gfa_petra_fpga_io_fld_set(&Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_a_path_select, 0x0);
  gfa_petra_fpga_io_fld_set(&Gfa_petra_fpga_io_regs.combo_switches_controls.combo_links_of_nif_b_path_select, 0x0);

  return SOC_SAND_OK;
}
#endif /* __DUNE_GTO_BCM_CPU__ */

uint8
  gfa_petra_is_line_card_connected(void
  )
{
  static uint8
    have_answer = FALSE,
    answer = FALSE;
  SOC_BSP_CARDS_DEFINES
    card_type;

  if (have_answer)
  {
    return answer;
  }

  host_board_type_from_nv(&card_type);
#ifndef __DUNE_GTO_BCM_CPU__
  answer |= (bsp_card_is_same_family(card_type, LINE_CARD_GFA_PETRA_X) ? TRUE : FALSE);
#endif /* __DUNE_GTO_BCM_CPU__ */
  have_answer = TRUE;
  return answer;
}

SOC_SAND_RET
  gfa_petra_ddr_clock_init(
    void
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint8
    val;

  /* Soc_petra Reset ->0 */
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset), 0);
  /* Core Reset ->0 */
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset), 0);

  sal_usleep(1000*10);
  /* Core Reset ->1 */
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset), 1);
  sal_usleep(1000*1);

  /* Soc_petra Reset ->1 */
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset), 1);

  val = gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.ddr_synt_freq.addr));
  val = gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr));
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_select.addr), 0x0);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_config.addr), 0x10);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x10);
  val = gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr));
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x87);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x80);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
  val = gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr));
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x7);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC1);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x4);
  val = gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr));
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xC);
  val = gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr));
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x7);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0xff);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0xff);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.ddr_synt_freq.addr), 0x7d);

  return ret;
}

SOC_SAND_RET
  gfa_petra_hw_reset(
    SOC_SAND_IN  uint8  in_reset_not_out_of_reset
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
#ifndef SAND_LOW_LEVEL_SIMULATION
  uint8
    reset_val;

  SOC_SAND_INTERRUPT_INIT_DEFS;

#ifndef DUNE_BCM
  /* Prevent task switching, because read-modify-write. */
  if (taskLock() == ERROR)
  {
    send_string_to_screen("gfa_petra_reset_device - failed on taskLock()", TRUE);
    ret = SOC_SAND_ERR ;
    goto exit_no_lock;
  }
#endif

  reset_val = SOC_SAND_BOOL2NUM_INVERSE(in_reset_not_out_of_reset);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset), reset_val);

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
#ifndef DUNE_BCM
  taskUnlock();
exit_no_lock:
#endif

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
#endif
  return ret;
}

SOC_SAND_RET
  gfa_petra_core_pll_reset(
    SOC_SAND_IN  uint8  in_reset_not_out_of_reset
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
#ifndef SAND_LOW_LEVEL_SIMULATION
  uint8
    reset_val;

  SOC_SAND_INTERRUPT_INIT_DEFS;

#ifndef DUNE_BCM
  /* Prevent task switching, because read-modify-write. */
  if (taskLock() == ERROR)
  {
    send_string_to_screen("gfa_petra_reset_device - failed on taskLock()", TRUE);
    ret = SOC_SAND_ERR ;
    goto exit_no_lock;
  }
#endif

  reset_val = SOC_SAND_BOOL2NUM_INVERSE(in_reset_not_out_of_reset);
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset), reset_val);

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
#ifndef DUNE_BCM
  taskUnlock();
exit_no_lock:
#endif

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
#endif
  return ret;
}

SOC_SAND_RET
  gfa_petra_reset_device(
    SOC_SAND_IN  uint32                   delay_to_be_down_mili_sec,
    SOC_SAND_IN  uint32                  stay_down
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
#ifndef SAND_LOW_LEVEL_SIMULATION

  SOC_SAND_INTERRUPT_INIT_DEFS;

#ifndef DUNE_BCM
  /* Prevent task switching, because read-modify-write. */
  if (taskLock() == ERROR)
  {
    send_string_to_screen("gfa_petra_reset_device - failed on taskLock()", TRUE);
    ret = SOC_SAND_ERR ;
    goto exit_no_lock;
  }
#endif

  /* Soc_petra Reset ->0 */
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset), 0);
  /* Core Reset ->0 */
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset), 0);

  /* Wait for asked time. */
  sal_usleep(1000*delay_to_be_down_mili_sec);

  if (!stay_down)
  { 
    /* User wishes to get the device out of reset. */
    sal_usleep(100);

    /* Core Reset ->1 */
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_pll_reset), 1);
    sal_usleep(1000*100);

    /* Soc_petra Reset ->1 */
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.soc_petra_miscelanous_controls_1.soc_petra_hardware_reset), 1);

    sal_usleep(1000*10);
  }

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;

#ifndef DUNE_BCM
  taskUnlock();
exit_no_lock:
#endif

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;

#endif
  return ret;
}

SOC_SAND_RET
  gfa_petra_reset(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  remain_active
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;


  ret = gfa_petra_reset_device(100, !remain_active);

  return ret;
}

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_init_host_board_mezzanine_epld(
  )
{
  uint32   
    offset;
  uint8
    value = 0x0;

  /* Mezzanine-epld init - enabling host board memory banks through mezzanine epld. */
  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)&(0xf0);
  write_epld_reg((uint8)value,(uint32)offset);
  soc_sand_os_printf("   -. All out of reset                   .....     pass\n");

  return SOC_SAND_OK;
}
#endif /* __DUNE_GTO_BCM_CPU__ */

uint32 
  gfa_petra_device_ver_set(void
  )
{
#ifndef SAND_LOW_LEVEL_SIMULATION
  uint32
    version_reg = 0;
#endif
  uint32
    reset_fail;

  reset_fail = gfa_petra_reset(0, TRUE);

  if(reset_fail)
  {
    soc_sand_os_printf(
      "                                        .....     fail\n"
    );
    goto exit;
  }

#ifndef SAND_LOW_LEVEL_SIMULATION

  version_reg = *((uint32 *)(GFA_PETRA_PETRA_BASE_ADDR | DUNE_VERSION_REGISTERS_ADDRESS));
  Gfa_petra_device_ver = SOC_SAND_GET_FLD_FROM_PLACE(version_reg, SOC_PETRA_MGMT_CHIP_VER_FLD_LSB, SOC_SAND_BITS_MASK(SOC_PETRA_MGMT_CHIP_VER_FLD_MSB, SOC_PETRA_MGMT_CHIP_VER_FLD_LSB));

#else

  Gfa_petra_device_ver = 1;

#endif

  reset_fail = gfa_petra_reset(0, FALSE);

  soc_sand_os_printf(
    "                                        .....     %s\n", reset_fail ? "FAIL" : "PASS"
  );

exit:
  return Gfa_petra_device_ver;
}

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_init_host_board(
    SOC_SAND_IN  char                       board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN],
    SOC_SAND_IN  uint16                   gfa_board_serial_number,
    SOC_SAND_IN SOC_BSP_CARDS_DEFINES             card_type,
    SOC_SAND_OUT uint32                  *chip_ver
  )
{
  SOC_D_USR_APP_FLAVOR
    usr_app_flavor;

  d_usr_app_flavor_get(&usr_app_flavor);

  test_d_printf("call: gfa_petra_fpga_io_regs_init\n\r");

  if (gfa_petra_fpga_io_regs_init())
  {
    return SOC_SAND_ERR;
  }

  if (Gfa_petra_GFA_PETRA_BOARD_SPECIFICATIONS_clear(card_type))
  {
    return SOC_SAND_ERR;
  }

  /* Open address space */
#ifdef NEGEV
  soc_sand_os_printf("+ .Initializing CPU mezzanine            .....     pass\n");
  if (gfa_petra_init_host_board_860())
  {
    return SOC_SAND_ERR;
  }
#else
  soc_sand_os_printf("+ .Initializing CPU mezzanine            .....     missing GTO initialization\n");
#endif

#ifdef GFA_PETRA_BACK_END_IS_PETRA_GFA
  /* Reset all devices */
  if (gfa_petra_init_host_board_mezzanine_epld())
  {
    soc_sand_os_printf("+ .Getting all GFA devices out of reset  .....     fail\n");
    return SOC_SAND_ERR;
  }
  soc_sand_os_printf("+ .Getting all GFA devices out of reset  .....     pass\n");
#ifdef GFA_PETRA_DFFS_INIT
  if(Gfa_petra_board_info.card_type != LOAD_BOARD_PB)
  {
    /* Initialize system flash and release it from reset */
    test_d_printf("call: gfa_petra_utils_dffs_init\n\r");
    if (gfa_petra_utils_dffs_init(FALSE))
    {
      return SOC_SAND_ERR;
    }
  }
#endif
  
  if ( Gfa_petra_board_info.card_type != LOAD_BOARD_PB)
  {
    /* Initialize FPGA IO agent and release it from reset */
    test_d_printf("call: gfa_petra_fpga_io_agent_init\n\r");
    if (gfa_petra_fpga_io_agent_init(FALSE))
    {
      soc_sand_os_printf("\n+ .Loading GFA IO agent FPGA             .....     fail\n");
      return SOC_SAND_ERR;
    }
    soc_sand_os_printf("\n+ .Loading GFA IO agent FPGA             .....     pass\n");

    /* Initialize FPGA IO agent and release it from reset */
    if (gfa_petra_streaming_agent_init(FALSE))
    {
      soc_sand_os_printf("\n+ .Loading GFA streaming FPGA            .....     fail\n");
      return SOC_SAND_ERR;
    }
    soc_sand_os_printf("\n+ .Loading GFA streaming FPGA            .....     pass\n");

    if (gfa_petra_fpga_io_agent_start())
    {
      soc_sand_os_printf("+ .Starting GFA IO agent FPGA            .....     fail\n");
      return SOC_SAND_ERR;
    }
    soc_sand_os_printf("+ .Starting GFA IO agent FPGA            .....     pass\n");
  }
#endif /* GFA_PETRA_BACK_END_IS_PETRA_GFA == FALSE */ 

#ifdef LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
  if (tevb_is_timna_connected())
  {
    soc_sand_os_printf("+ .Setting QDR Synthesizer to 187.5 MHz\n\r");
    gfa_petra_board_synt_set(GFA_PETRA_SYNT_TYPE_QDR, 187500000, TRUE);
    soc_sand_os_printf("+ .Setting NIF Synthesizer to  125 MHz\n\r");
    gfa_petra_board_synt_set(GFA_PETRA_SYNT_TYPE_NIF, 125000000, TRUE);
  }
#endif

  if (usr_app_flavor == GFA_PETRA_SCREENING_FLAVOR)
  {
    if (gfa_petra_board_synt_set(GFA_PETRA_SYNT_TYPE_DDR, GFA_PETRA_DRAM_SCREENING_FREQ, TRUE))
    {
      return SOC_SAND_ERR;
    }
  }

  if(Gfa_petra_board_info.card_type == LOAD_BOARD_PB)
  {
  }
  *chip_ver = SOC_PETRA_EXPECTED_CHIP_VER;

  return SOC_SAND_OK;
}

SOC_SAND_RET
  gfa_petra_tg_type_get(
    SOC_SAND_OUT GFA_TG_TYPE *tg_type,
    SOC_SAND_IN  uint8   silent
  )
{
  uint8
    is_phy,
    is_ptg;

  UTILS_INIT_ERR_DEFS("gfa_petra_tg_type_get");
  UTILS_ERR_IF_NULL(tg_type, 5);

  is_phy = utils_ptg_is_line_card_phy_connected();
  is_ptg = utils_ptg_is_line_card_ptg_connected();

  if ((is_phy == TRUE) && (is_ptg == TRUE))
  {
    UTILS_PRINT_ERR_MSG(silent, "Simultanious PTG and external phy front end - not supported");
    UTILS_SET_ERR_AND_EXIT(20);
  }
  else if (is_phy)
  {
    *tg_type = GFA_TG_TYPE_EXTERNAL;
  } 
  else if (is_ptg)
  {
    *tg_type = GFA_TG_TYPE_PTG;
  }
  else
  {
   *tg_type = GFA_TG_TYPE_NONE;
  }

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}
#endif /* __DUNE_GTO_BCM_CPU__ */
SOC_SAND_RET
  gfa_petra_synt_assert(void
  )
{
  int
    wait_i;
    
  volatile uint8 *addr = (uint8*)0xff000868;
  uint8 val = 0xa;

  soc_sand_os_printf("BAUD rate reg 0xff000868 read val 0x%x\n\r", (int)*addr);
  soc_sand_os_printf("BAUD rate reg 0xff000868 writing val 0x%x\n\r", (int)val);
  *addr = val;

  if(Gfa_petra_board_info.card_type == LOAD_BOARD_PB)
  {
    uint32 dev, reg;
    int ret = 0;
    uint8 dummy;

    soc_sand_os_printf("i2c probe start\n\r");
    /*lb_petra_i2c_syntmuxset(GFA_PETRA_SYNT_TYPE_DDR);*/

    for(dev = 0; dev < 128; dev++)
    {
      /*for(reg = 0; reg < 256; reg++)*/
      reg = 0x0;
      {
        
        ret = lb_petra_i2c_read((uint8)dev, (uint8)reg, 1, &dummy);
        if(ret)
        {
          /* soc_sand_os_printf("dev %lx reg %lx FAIL\n\r", dev, reg); */
        }
        else
        {
          soc_sand_os_printf("dev %x reg %x PASS\n\r", dev, reg);
        }
      }
    }
    soc_sand_os_printf("i2c probe end\n\r");

    /* soc_sand_os_printf("gfa_petra_synt_assert not supported in LoadBoard\n"); */
    return SOC_SAND_OK;
  }
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_select.addr), 0x0);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_config.addr), 0x10);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x10);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
  
    sal_usleep(1000*100);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x89);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x18);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x7);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x61);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x43);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0xd);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x82);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x45);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x26);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);
    
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x89);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x8);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), 0xC0);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x1);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x87);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x2);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), 0x40);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0xA);
    while (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) != 3);
    GFA_PETRA_BUZY_WAIT(2500);

  return SOC_SAND_OK;
}

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_front_interface_type_get(
    SOC_SAND_OUT GFA_PETRA_FRONT_INTERFACE_TYPE *nif_type,
    SOC_SAND_IN  uint8                      silent
  )
{
  uint8
    is_phy,
#ifdef LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
    is_tim,
#endif  
    is_ptg;
  GFA_PETRA_FRONT_INTERFACE_TYPE
    interface_type;

  UTILS_INIT_ERR_DEFS("gfa_petra_front_interface_type_get");
  
  UTILS_ERR_IF_NULL(nif_type, 10);

  is_phy = utils_ptg_is_line_card_phy_connected();
  is_ptg = utils_ptg_is_line_card_ptg_connected();
#ifdef LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
  is_tim = tevb_is_timna_connected();
#endif  

  if ((is_phy) && (is_ptg))
  {
    UTILS_PRINT_ERR_MSG(silent, "Simultanious PTG and external phy front end - not supported");
    UTILS_SET_ERR_AND_EXIT(20);
  }
  else if(is_ptg)
  {
    interface_type = GFA_PETRA_FRONT_INTERFACE_PTG_SPAUI;
  }
  else if(is_phy)
  {
    interface_type = GFA_PETRA_FRONT_INTERFACE_PHY_XAUI;
  }
#ifdef LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
  else if(is_tim)
  {
    interface_type = GFA_PETRA_FRONT_INTERFACE_TMN_SPAUI;
  }
#endif  
  else
  {
    interface_type = GFA_PETRA_FRONT_INTERFACE_PHY_XAUI;
  }

  *nif_type = interface_type;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_petra_board_dram_frequency_get(
    SOC_SAND_OUT uint32                   *dram_frequency
  )
{
  uint32
    dram_f;
  SOC_D_USR_APP_FLAVOR
    usr_app_flavor;

  UTILS_INIT_ERR_DEFS("gfa_petra_board_dram_frequency_get");
  
  UTILS_ERR_IF_NULL(dram_frequency, 10);

  d_usr_app_flavor_get(&usr_app_flavor);

  if (GFA_PETRA_IS_PB_BOARD)
  {
    dram_f = 500;
  }
  else
  {
  switch (usr_app_flavor)
    {
    case SOC_D_USR_APP_FLAVOR_MINIMAL:
    case SOC_D_USR_APP_FLAVOR_MINIMAL_NO_APP:
      dram_f = 533;
      break;

    default:
      dram_f = 533;
      break;
    }
  }

  *dram_frequency = dram_f;
exit:
  UTILS_EXIT_AND_PRINT_ERR;
}
#endif /* __DUNE_GTO_BCM_CPU__ */

SOC_SAND_RET
  gfa_petra_board_dram_ref_clock_get(
    SOC_SAND_OUT uint32                   *ref_clock
  )
{
  uint32
    rfclk;
  UTILS_INIT_ERR_DEFS("gfa_petra_board_dram_ref_clock_get");
  
  UTILS_ERR_IF_NULL(ref_clock, 10);

  rfclk = Gfa_petra_synt_values[GFA_PETRA_SYNT_TYPE_DDR]/1000000;

  *ref_clock = (rfclk == 0)?125:rfclk;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/************************************************************************/
/*  Synthesizer Functions                                               */
/************************************************************************/

static char 
  gfa_petra_board_fpga_i2c_ready (void
  )
{
 if (((gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_status.addr)) & 0x3)) == 3)
 {
   return 1;
 }
#ifdef __DUNE_GTO_BCM_CPU__
 soc_sand_os_printf("gfa_petra_board_fpga_i2c_ready(). gfa_petra_board_fpga_i2c_ready() didnt return 0x3\n");
#endif
 return 0;
} 

int 
  gfa_petra_board_fpga_i2c_syntmuxset (
    SOC_SAND_IN GFA_TG_SYNT_TYPE     target
  )
{
    uint8 regval;

    switch (target) 
    {
      case GFA_PETRA_SYNT_TYPE_FABRIC : 
      case GFA_PETRA_SYNT_TYPE_PCP_CORE :      /*  relevant for gfa-bi board  */
        regval = 0x01;
        break;
      case GFA_PETRA_SYNT_TYPE_COMBO :
        regval = 0x02;
        break;
      case GFA_PETRA_SYNT_TYPE_NIF : 
        regval = 0x04;
        break;
      case GFA_PETRA_SYNT_TYPE_CORE : 
        regval = 0x08;
        break;
      case GFA_PETRA_SYNT_TYPE_DDR : 
        regval = 0x10;
        break;
      case GFA_PETRA_SYNT_TYPE_QDR : 
        regval = 0x20;
        break;     
      case GFA_PETRA_SYNT_TYPE_SYNCE : 
        regval = 0x40;
        break;
      case GFA_PETRA_SYNT_TYPE_PHY :          /*  relevant for gfa-bi board  */
        regval = 0x40;
        break;
      case GFA_PETRA_SYNT_TYPE_PCP_ELK :      /*  relevant for gfa-bi board  */
        regval = 0x80;
        break; 
      default : 
        regval = 0x00;
        break;               
    }

   
    /* first , disable the Power supply I2C Mux ports */ 
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_select.addr), 0x01);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_config.addr), 0x00);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x10);
    sal_usleep(1000*100);
    GFA_PETRA_FPGA_I2C_WAIT;
   
    /* Now , config the Clock I2C mux port to the selected target synthesizer */
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_select.addr), 0x00);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_mux_config.addr), regval);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), 0x10);
    sal_usleep(1000*100);   
    GFA_PETRA_FPGA_I2C_WAIT;
 
    return SOC_SAND_OK;
}

int
  gfa_petra_board_fpga_synce_muxset(void)
{

  if ((Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) || (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2))
  {
    soc_sand_os_printf("gfa_petra_board_fpga_synce_muxset. no Gfa_petra_fpga_io_regs.synce_pll_ctrl in LINE_CARD_GFA_PETRA_B_INTERLAKEN\n");
    return SOC_SAND_OK;
  }
  gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.synce_pll_ctrl.addr), 0x01);
  gfa_petra_board_fpga_i2c_syntmuxset(GFA_PETRA_SYNT_TYPE_SYNCE);
  return SOC_SAND_OK;
}

  int  
    gfa_petra_fpga_i2c_write (
      uint8            unit,
      uint8            internal_address,
      uint8            buffer_size,
      uint8           *buffer
    ) 
{
     uint32 
      wait_i;
    uint8 
      trig_sendcommand = 0x01;
    uint8 
      trig_sendbyte = 0x02;
    uint8 
      trig_lastByte = 0x08;
    uint8 
      command_byte;
    uint8 
      current_trigger;
    uint8 
      idx;

    GFA_PETRA_FPGA_I2C_WAIT;
     
     /* create I2C address/command byte value */
     command_byte = unit << 1;
     command_byte = command_byte & 0xFE;
     
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), command_byte);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendcommand);
    GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);
    GFA_PETRA_FPGA_I2C_WAIT;

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), internal_address);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendbyte);
    GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);    
    GFA_PETRA_FPGA_I2C_WAIT;

    for (idx = 0; idx<buffer_size; ++idx) 
    {
      if (idx == (buffer_size - 1)) 
      {
        current_trigger = trig_sendbyte | trig_lastByte;
      } 
      else 
      {
        current_trigger = trig_sendbyte;
      }
      gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), buffer[idx]);
      gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), current_trigger);
      GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);
      GFA_PETRA_FPGA_I2C_WAIT;       
    }
    return SOC_SAND_OK;
}

#ifdef __DUNE_GTO_BCM_CPU__
int  
    gfa_petra_fpga_i2c_write_quick (
      uint8            unit,
      uint8            internal_address,
      uint8            buffer_size,
      uint8           *buffer
    ) 
{
    /* uint32 
      wait_i; */
    uint8 
      trig_sendcommand = 0x01;
    uint8 
      trig_sendbyte = 0x02;
    uint8 
      trig_lastByte = 0x08;
    uint8 
      command_byte;
    uint8 
      current_trigger;
    uint8 
      idx;
    uint32
      old_flags = 0x0;

    sal_i2c_config_get(0, &old_flags);
    sal_i2c_config_set(0, (old_flags | SAL_I2C_FAST_ACCESS));

    /* GFA_PETRA_FPGA_I2C_WAIT; */

    /* create I2C address/command byte value */
     command_byte = unit << 1;
     command_byte = command_byte & 0xFE;
     
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), command_byte);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendcommand);
    /* GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);
    GFA_PETRA_FPGA_I2C_WAIT; */

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), internal_address);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendbyte);
    /* GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);    
    GFA_PETRA_FPGA_I2C_WAIT; */

    for (idx = 0; idx<buffer_size; ++idx) 
    {
      if (idx == (buffer_size - 1)) 
      {
        current_trigger = trig_sendbyte | trig_lastByte;
      } 
      else 
      {
        current_trigger = trig_sendbyte;
      }
      gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), buffer[idx]);
      gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), current_trigger);
      /* GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);
      GFA_PETRA_FPGA_I2C_WAIT;        */
    }
    
    sal_i2c_config_set(0, old_flags);
    return SOC_SAND_OK;
}

int  
    gfa_petra_fpga_i2c_write_quick2 (
      uint8            unit,
      uint8            internal_address,
      uint8            buffer_size,
      uint8           *buffer
    ) 
{
    /* uint32 
      wait_i; */
    uint8 
      trig_sendcommand = 0x01;
    uint8 
      trig_sendbyte = 0x02;
    uint8 
      trig_lastByte = 0x08;
    uint8 
      command_byte;
    uint8 
      current_trigger;
    uint8 
      idx;
    uint32
      old_flags = 0x0;

    sal_i2c_config_get(0, &old_flags);
    sal_i2c_config_set(0, (old_flags | SAL_I2C_FAST_ACCESS));

    /* GFA_PETRA_FPGA_I2C_WAIT; */
    
     /* create I2C address/command byte value */
     command_byte = unit << 1;
     command_byte = command_byte & 0xFE;
     
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendcommand);
    /* GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);
    GFA_PETRA_FPGA_I2C_WAIT; */

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), internal_address);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendbyte);
    /* GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);    
    GFA_PETRA_FPGA_I2C_WAIT; */

    for (idx = 0; idx<buffer_size; ++idx) 
    {
      if (idx == (buffer_size - 1)) 
      {
        current_trigger = trig_sendbyte | trig_lastByte;
      } 
      else 
      {
        current_trigger = trig_sendbyte;
      }
      gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), buffer[idx]);
      gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), current_trigger);
      /* GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);
      GFA_PETRA_FPGA_I2C_WAIT;        */
    }
    
    sal_i2c_config_set(0, old_flags);
    return SOC_SAND_OK;
}
#endif

int  
  gfa_petra_fpga_i2c_read (
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8           *buffer
 )   
{
    int32
      wait_i;
    uint8 
      trig_sendcommand = 0x01;
    uint8 
      trig_sendbyte = 0x02;
    uint8 
      trig_receivebyte = 0x4;
    uint8 
      trig_lastByte = 0x08;
    uint8 
      command_byte;
    uint8 
      current_trigger;
    uint8 
      idx;

    GFA_PETRA_FPGA_I2C_WAIT;     
     
     /* create I2C address/command byte value */
     command_byte = unit << 1;
     command_byte = command_byte & 0xFE;
     
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), command_byte);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendcommand);
    GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);            
    GFA_PETRA_FPGA_I2C_WAIT;

    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr), internal_address);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendbyte | trig_lastByte);
    GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);          
    GFA_PETRA_FPGA_I2C_WAIT;

      command_byte = command_byte | 0x01;
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_address_command.addr), command_byte);
    gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), trig_sendcommand);
    GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);           
    GFA_PETRA_FPGA_I2C_WAIT;

     for (idx = 0;idx<buffer_size; ++idx) 
     {
       if (idx == (buffer_size - 1)) 
       {
         current_trigger = trig_receivebyte | trig_lastByte;
       } 
       else
       {
         current_trigger = trig_receivebyte;
       }
        gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.i2c_triggers.addr), current_trigger);
        GFA_PETRA_BUZY_WAIT(GFA_PETRA_FPGA_I2C_BUAY_WAIT_INT);         
        GFA_PETRA_FPGA_I2C_WAIT;     
        buffer[idx] = (char)(gfa_petra_fpga_io_fld_get(&(Gfa_petra_fpga_io_regs.i2c_data_tx_rx.addr)) & 0xFF);     
     }
     return SOC_SAND_OK;
}

int
  lb_bsp_i2c_write_gen_inner(
    uint8  unit,
    uint8  write_data[LB_BSP_I2C_BUFFER_SIZE],
    unsigned short write_data_arr_entries,
    uint8  internal_address,
    SOC_SAND_IN uint8 internal_address_valid
  )
{
  int
    ret = 0;
#if !defined(SAND_LOW_LEVEL_SIMULATION) && !defined(__DUNE_GTO_BCM_CPU__)
  char
    *proc_name ;
  char
    err_msg[80*3] ;
  unsigned int
      err_flag ;
  uint8
    device_write_address,
    actual_write_data_arr_entries ;
  uint8  
    actual_write_data[LB_BSP_I2C_BUFFER_SIZE + 1];
  /*
   */
  proc_name = "lb_bsp_i2c_write_gen" ;
  device_write_address = unit << 1;
  ret = 0;

  if (write_data_arr_entries > 1)
  {
    /* May read one byte at a time via i2c. */
    
    gen_err(TRUE,FALSE,(int)TRUE,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_01,FALSE,0,-1,FALSE) ;

    ret = TRUE;
    goto exit ;

  }


  
  if(internal_address_valid)
  {
    actual_write_data[0] = internal_address;
    
    sal_memcpy(actual_write_data + 1, write_data, write_data_arr_entries);
    actual_write_data_arr_entries = write_data_arr_entries + 1;
    /*
    actual_write_data[1] = actual_write_data[2] = write_data[0];
    actual_write_data_arr_entries = 3; */
  }
  else
  {
    sal_memcpy(actual_write_data, write_data, write_data_arr_entries);
    actual_write_data_arr_entries = write_data_arr_entries;
  }

  /*
   * Write a byte.
   */
  ret =
    i2c_load_buffer(
      device_write_address,
      actual_write_data_arr_entries,
      actual_write_data,
      &err_flag,err_msg
    ) ;

  /* soc_sand_os_printf("i2c_load_buffer ret = %d\n\r", (int)ret); */
  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_02,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
      "I2C fail. #1 ",proc_name,
      SVR_ERR,I2C_MEM_ERR_03,FALSE,0,-1,FALSE) ; 
  if (ret || err_flag)
  {
    ret = TRUE;
    goto exit ;
  }

  /*
   * Set a kick to fulfill the request.
   */
  i2c_start() ;

  ret = i2c_poll_tx(&err_flag,err_msg) ;

  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_04,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
    "I2C fail. #2 ",proc_name,
    SVR_ERR,I2C_MEM_ERR_05,FALSE,0,-1,FALSE) ;
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
  lb_petra_i2c_syntmuxset (
    SOC_SAND_IN GFA_TG_SYNT_TYPE     target
  )
{
    uint8
      write_data[20];
    int
      ret;

    switch (target) 
    {
    case GFA_PETRA_SYNT_TYPE_FABRIC : 
      write_data[0] = 0x40;
      break;
    case GFA_PETRA_SYNT_TYPE_COMBO :
      write_data[0] = 0x20;
      break;
    case GFA_PETRA_SYNT_TYPE_NIF : 
      write_data[0] = 0x80;
      break;
    case GFA_PETRA_SYNT_TYPE_CORE : 
      write_data[0] = 0x10;
      break;
    case GFA_PETRA_SYNT_TYPE_DDR : 
      write_data[0] = 0x08;
      break;
    case GFA_PETRA_SYNT_TYPE_QDR : 
      write_data[0] = 0x04;
      break;     
    default : 
      write_data[0] = 0x00;
      break;               
    }

    /* Write to the MUX */

    ret = lb_bsp_i2c_write_gen_inner(0x70, write_data, 1, 0, FALSE);
    if(ret)
    {
      soc_sand_os_printf("lb_bsp_i2c_write_gen to MUX returned with %d\r\n", ret);
    }

    return ret;
}

int  
  lb_petra_i2c_read (
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8           *buffer
  )   
{
    int ret
      = 0;
#if !defined(SAND_LOW_LEVEL_SIMULATION) && !defined(__DUNE_GTO_BCM_CPU__)

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
      device_read_address,
      device_write_address,
      i;
    

    /*
    */
    proc_name = "lb_petra_i2c_read" ;
    device_read_address  = unit << 1;
    device_read_address |= 0x1;
    device_write_address = unit << 1;

    for (i = 0; i < buffer_size; ++i, ++internal_address)
    {
      ret = 0;

      /* sal_usleep(1000*50); */

      lb_bsp_i2c_write_gen_inner(unit, &read_dummy_data, 0, internal_address, TRUE);

      
      /*soc_sand_os_printf("lb_petra_i2c_read load_buffer device_read_address 0x%x \r\n", (int)device_read_address);*/

      /*
      * Set the address of the device to read from.
      */
      read_dummy_data = 0xFF;
      ret =
        i2c_load_buffer(
          device_read_address,
          sizeof(read_dummy_data),
          (uint8*)(&read_dummy_data),
          &err_flag,err_msg
        ) ;
       gen_err(TRUE,FALSE,(int)err_flag,0,
        err_msg,proc_name,
        SVR_WRN,I2C_MEM_ERR_05,FALSE,0,-1,FALSE) ;
      gen_err(TRUE,FALSE,(int)ret,0,
        "I2C fail. #3 ",proc_name,
        SVR_ERR,I2C_MEM_ERR_06,FALSE,0,-1,FALSE) ;
      if (ret || err_flag)
      {
        ret = TRUE;
        goto exit ;
        
      }

      /*
      * Set a kick to fulfill the request.
      */
      i2c_start() ;

      ret = i2c_poll_tx(&err_flag,err_msg) ;

      gen_err(TRUE,FALSE,(int)err_flag,0,
        err_msg,proc_name,
        SVR_WRN,I2C_MEM_ERR_07,FALSE,0,-1,FALSE) ;
      gen_err(TRUE,FALSE,(int)ret,0,
        "I2C fail. #4 ",proc_name,
        SVR_ERR,I2C_MEM_ERR_08,FALSE,0,-1,FALSE) ;

      /*
      * Wait 50 milisec to make sure received bytes are registered.
      */
      /* sal_usleep(1000*50); */
   
      ret =
        i2c_get_buffer(
        &i2c_data_len,
        &buffer[i],
        &err_flag,err_msg
        ) ;

      gen_err(TRUE,FALSE,(int)err_flag,0,
        err_msg,proc_name,
        SVR_WRN,I2C_MEM_ERR_09,FALSE,0,-1,FALSE) ;
      gen_err(TRUE,FALSE,(int)ret,0,
        "I2C fail. #5",proc_name,
        SVR_ERR,I2C_MEM_ERR_10,FALSE,0,-1,FALSE) ;
      if (ret || err_flag)
      {
        ret = TRUE;
        goto exit ; 
      }

      if(i2c_data_len != 1)
      {
        gen_err(TRUE,FALSE,(int)ret,0,
          "I2C read size != 1",proc_name,
          SVR_ERR,I2C_MEM_ERR_08,FALSE,0,-1,FALSE) ;
        
      }
    }


  exit:
#endif

    return ret;
  }

int  
  lb_petra_i2c_write (
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8            *buffer
  ) 
{
  int
    ret = 0;

  if(buffer_size > LB_BSP_I2C_BUFFER_SIZE)
  {
    soc_sand_os_printf("gfa_petra_fpga_i2c_write: too large buffer");
  }
  ret = lb_bsp_i2c_write_gen_inner(
          unit,
          buffer,
          buffer_size,
          internal_address,
          TRUE
      );
  if (ret)
  {
    ret = TRUE;
    goto exit ;
  }

  /* sal_usleep(1000*50); */

exit:
    return ret;
}

/*  
 *  Important note : the internal VCO of the synthesizer runs at a frequency in the range of 5GHz.
 *  performing calculations in this range requires 33 bits of precision for representing frequencies in single Hz units.
 *  for our purposes this precision is not required.
 *  Given our CPU limitation of 32bit unsigned integers and no floating point support from standard library,
 *  all frequencies used in this function are represented as 1/10 of the real freq.
 *  Any way the output frequency will be achieved with better than 1ppm precision.
 *  The nominal wake up frequency of 125MHz is going to be represented here as 12.5MHz
 *  example : a target frequency of 156,250,000Hz would be calculated as if it was requested to be virtually 15,625,000Hz
 *  based on the virtual 12,500,000Hz nominal frequency.
 *             
 */

STATIC SOC_SAND_RET
  gfa_petra_board_synt_set_imp(
   SOC_SAND_IN GFA_TG_SYNT_TYPE   targetsynt,
   SOC_SAND_IN uint32           targetfreq,
   SOC_SAND_IN uint8          silent,
   SOC_SAND_IN uint8            synt_id,
   SOC_SAND_IN uint32           nominalfreq
  )
{
  uint8 
    reg7,
    reg8,
    reg9,
    reg10,
    reg11,
    reg12,
    reg135,
    reg137,
    rawHS_DIV,
    rawN1,
    Ary_HS_DIV[12],
    Ary_N1[129],
    fFoundValues;
  uint8 
    write_sucess;
  uint32 
     HS_DIV,
     N1,
     RFREQ,
     newRFREQ,
     nominalfreqdiv10,
     targetfreqdiv10,
     fosc,
     fosc64l,
     fosc64h,
     fxtal,
     LowerFosc,
     UpperFosc,
     lowerRFREQ,
     UpperRFREQ;       
  uint8 
    databuffer[6],
    buffersize = 6;
  SOC_SAND_U64 
    fosc64;
  SOC_SAND_U64 
    result64;        
  uint32 
    idx,
    jdx;
  uint32
    max_loop_cnt = 100,
    loop_cnt = 0;

  UTILS_INIT_ERR_DEFS("gfa_petra_board_synt_set_imp");

  nominalfreqdiv10 = nominalfreq/10;         /* This is the real wake up frequency divided by 10 */
  targetfreqdiv10 = targetfreq / 10;   /* This is the real target frequency divided by 10 */

  do 
  {
    Gfa_petra_board_info.i2c_read(synt_id,SYNTHESIZER_REG135,1,&reg135);  

    /* Reset synt, in order for fout to be nominal rate. */
    databuffer[0] = (reg135 | 0x1);
    buffersize = 1;

    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG135,buffersize,databuffer);
    sal_usleep(1000*100);

    Gfa_petra_board_info.i2c_read(synt_id,SYNTHESIZER_REG137,1,&reg137);      

    Gfa_petra_board_info.i2c_read(synt_id,SYNTHESIZER_REG7TO12,6,databuffer);      

    reg7 = databuffer[0];
    reg8 = databuffer[1];
    reg9 = databuffer[2];
    reg10 = databuffer[3];
    reg11 = databuffer[4];
    reg12 = databuffer[5];                    
	
	if (!silent) soc_sand_os_printf("reg7=0x%x, reg8=0x%x, reg9=0x%x, reg10=0x%x, reg11=0x%x, reg12=0x%x, \n", reg7, reg8, reg9, reg10, reg11, reg12);
   
    /* extract the stored raw binary value of HS_DIV */
    rawHS_DIV = reg7 >> 5;
    rawHS_DIV = rawHS_DIV & 0x07;

    /* convert HS_DIV from raw binary to integer factor */
    switch (rawHS_DIV) 
    {
     case 0 : 
       HS_DIV = 4;
       break;
     case 1 : 
       HS_DIV = 5;
       break;
     case 2 : 
       HS_DIV = 6;
       break;
     case 3 : 
       HS_DIV = 7;
       break;
     case 5 : 
       HS_DIV = 9;
       break;
     case 7 : 
       HS_DIV = 11;
       break; 
     default:
       return SOC_SAND_ERR; 
    }

    /* 
     * Extract the stored binary value of N1 
     */
    rawN1 = reg7 & 0x1F;
    rawN1 = rawN1 << 2;
    rawN1 = rawN1 | ((reg8 >> 6) & 0x03);

    /* 
     * Convert N1 from synt binary to integer factor 
     */
    N1 = (long)rawN1 + 1;

    /*  Extracting the 30 most significant bits of RFREQ from synthesizer registers
     *  the 8 LSB bits of RFREQ (reg12) are not used here. 
     *  RFREQ is NOT going to be used as a real ("nn.nnnn") number here. 
     *  RFREQ will be used as a 30 bit binary divider.
     */
    RFREQ =  (((uint32)reg8 & 0x3F) << 24);
    RFREQ = (RFREQ | ((uint32)reg9 << 16));
    RFREQ = (RFREQ | ((uint32)reg10 << 8));   
    RFREQ = (RFREQ | (uint32)reg11);   

    /*   
     * RFREQ is now actualy a times 2^20 representation of the real REFREQ (the decimal point is between bit 19 and 20 
     * Calculate virtual internal crystal frequency (F_XTAL) from known nominal frequency (F_OUT)
     * From device datasheet -->  Fxtal = (F_OUT * HS_DIV * N1) / RFREQ
     */
    fosc = nominalfreqdiv10 * HS_DIV * N1;
    /* create a 64bit integer by pushing fosc value 20 bits to the left */
    /* fosc must be shifted left the same number of bits that RFREQ is shifted left !! */   
    fosc64l = fosc << 20;    
    fosc64h = fosc >> 12;
    fosc64.arr[0] = fosc64l;     
    fosc64.arr[1] = fosc64h;             
    soc_sand_u64_devide_u64_long(&fosc64,RFREQ,&result64);
    fxtal = result64.arr[0];
    if (!silent) soc_sand_os_printf("Reading synthesizer registers.\n");
    if (!silent) soc_sand_os_printf("Calculated internal crystal frequency: %u \n", fxtal * 10);

    loop_cnt++;
    if(loop_cnt > max_loop_cnt)
    {
      if (!silent) soc_sand_os_printf("Synt set: Fxtal not in required interval after %d loops \n", max_loop_cnt);
      m_ret = SOC_SAND_ERR;
      goto exit;
    }
  } while(fxtal < 11400000 || fxtal > 11460000);

  /* 
   * Set permitted values and ranges 
   */
  for (idx = 0; idx < 12; ++idx) 
  {
    Ary_HS_DIV[idx] = 0xFF;  /* 0xFF will indicate unused entries in the array */
  }

  Ary_HS_DIV[4]  = 0x00;
  Ary_HS_DIV[5]  = 0x01;
  Ary_HS_DIV[6]  = 0x02;
  Ary_HS_DIV[7]  = 0x03;
  Ary_HS_DIV[9]  = 0x05;
  Ary_HS_DIV[11] = 0x07;
  LowerFosc = 485000000;     /* thats 1/10 of the actual value */
  UpperFosc = 567000000;     /* thats 1/10 of the actual value */
  lowerRFREQ = 0x00000001;   
  UpperRFREQ = 0x3FFFFFFF;
  for (idx=0;idx<129; ++idx) 
  {
    Ary_N1[idx] = 0xFF;     /* 0xFF will indicate unused entries in the array */
  }

  Ary_N1[1] = 0x00;
  for (idx=2;idx <= 128; ++idx) 
  {
    if ((idx % 2) == 0) 
    {
      Ary_N1[idx] = (idx - 1);
    }
  }

  /* Calculate new values for requested frequency
    From device datasheet  --> Fosc = Fout * HS_DIV * N1
                           --> Fosc = Fxtal * RFREQ
                           --> RFREQ = Fosc / Fxtal
    Should find a combination of highest HS_DIV value with lowest N1 value
    Fosc must be in the range of 4.85GHz to 5.67GHz
    Actual (real) RFREQ must be greater than 1/(2^28) and less than 1024 (which is between 1 to 0x3FFFFFFF in the way it is used here).
  */

  fFoundValues = 0;
  newRFREQ = RFREQ;

  for (idx=12;idx>0;idx--) if (Ary_HS_DIV[idx-1] != 0xFF) 
  {
    HS_DIV = idx-1;
    for (jdx=0;jdx<=128;jdx++) if (Ary_N1[jdx] != 0xFF) 
    {
      N1 = jdx;
      fosc = targetfreqdiv10 * HS_DIV * N1;
      if (fosc > UpperFosc)
      {
        break;
      }

      if ((fosc > LowerFosc) && (fosc < UpperFosc)) {
        /* create a 64bit integer by pushing fosc value 20 bits to the left */
        /* fosc must be shifted left the same number of bits that RFREQ is shifted left !! */        
        fosc64l = fosc << 20;    
        fosc64h = fosc >> 12;
        fosc64.arr[0] = fosc64l;     
        fosc64.arr[1] = fosc64h;             
        soc_sand_u64_devide_u64_long(&fosc64,fxtal,&result64);
        newRFREQ = result64.arr[0];        
        if ((newRFREQ >= lowerRFREQ) && (newRFREQ <= UpperRFREQ)) 
        {
          fFoundValues = 1;
          break;
        }
      }
    }
    if (fFoundValues) 
    {
      break;
    }
  }

  if (!silent) soc_sand_os_printf("RFREQ X 2^20: %u \n", newRFREQ);
  if (!silent) soc_sand_os_printf("Internal Oscillator / 10: %u \n", fosc);
  if (!silent) soc_sand_os_printf("HS_DIV: %u \n", HS_DIV);
  if (!silent) soc_sand_os_printf("N1: %u \n", N1);

  /* Build target registers with new values */
  reg7 = (Ary_HS_DIV[HS_DIV] << 5) | ((Ary_N1[N1] >> 2) & 0x1F);
  reg8 = ((Ary_N1[N1] << 6) & 0xC0) | (uint8)((newRFREQ >> 24) & 0x3F);
  reg9 = (uint8)((newRFREQ >> 16) & 0xFF);
  reg10 = (uint8)((newRFREQ >> 8) & 0xFF); 
  reg11 = (uint8)(newRFREQ & 0xFF); 
  reg12 = 0x00;

  loop_cnt = 0;
  do 
  {
    /* Freeze the DCO before modification of parameters */

    reg137 = reg137 | 0x10;
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG137,1,&reg137);

    /* modify synthesizer values to generate new frequency */
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+0,1,&reg7);
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+1,1,&reg8);
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+2,1,&reg9);
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+3,1,&reg10);
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+4,1,&reg11);
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+5,1,&reg12);

    /* Unfreeze the DCO and assert the NewFreq bit */
    /* Data sheet requires maximum 10ms from unfreezing the DCO to asserting the NewFreq bit */

#ifndef __DUNE_GTO_BCM_CPU__
    reg137 = reg137 & 0xEF;
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG137,1,&reg137);

    reg135 = reg135 | 0x40; 
    Gfa_petra_board_info.i2c_write(synt_id,SYNTHESIZER_REG135,1,&reg135);
#else
	reg137 = reg137 & 0xEF;
    gfa_petra_fpga_i2c_write_quick(synt_id,SYNTHESIZER_REG137,1,&reg137);

    reg135 = reg135 | 0x40; 
    gfa_petra_fpga_i2c_write_quick2(synt_id,SYNTHESIZER_REG135,1,&reg135);
#endif

    /* Synthesizer should run new frequency now. */
    Gfa_petra_board_info.i2c_read(synt_id,SYNTHESIZER_REG7TO12,6,databuffer);      
    
    write_sucess = ((reg7  == databuffer[0]) &&
                    (reg8  == databuffer[1]) &&
                    (reg9  == databuffer[2]) &&
                    (reg10 == databuffer[3]) &&
                    (reg11 == databuffer[4]) &&
                    (reg12 == databuffer[5]));

    loop_cnt++;
    if(loop_cnt > max_loop_cnt)
    {
      soc_sand_os_printf("Synt set: write still did not succeeded after %d loops \n", max_loop_cnt);
      m_ret = SOC_SAND_ERR;
      goto exit;
    }
  }while(!write_sucess);
exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  gfa_petra_puc_pll_set(
    SOC_SAND_IN uint8 pll_m,
    SOC_SAND_IN uint8 pll_n,
    SOC_SAND_IN uint8 pll_p
  )
{

  if ((Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) || (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2))
  {
    soc_sand_os_printf("gfa_petra_puc_pll_set. no Gfa_petra_fpga_io_regs.puc in LINE_CARD_GFA_PETRA_B_INTERLAKEN\n");
    return SOC_SAND_OK;
  }
  gfa_petra_fpga_io_fld_set(&Gfa_petra_fpga_io_regs.puc_2.core_pll_m, pll_m);
  gfa_petra_fpga_io_fld_set(&Gfa_petra_fpga_io_regs.puc_1.core_pll_n, pll_n);
  gfa_petra_fpga_io_fld_set(&Gfa_petra_fpga_io_regs.puc_1.core_pll_p, pll_p);

  return SOC_SAND_OK;
}

uint32
  gfa_petra_board_synt_nominal_freq_get(
    SOC_SAND_IN GFA_TG_SYNT_TYPE   targetsynt
    )
{
  uint32 nominalfreq = 0;

  if(GFA_PETRA_IS_PB_BOARD)
  {
    switch(targetsynt)
    {
    case GFA_PETRA_SYNT_TYPE_QDR:
    case GFA_PETRA_SYNT_TYPE_CORE:
      nominalfreq = 100000000;
      break;
    case GFA_PETRA_SYNT_TYPE_DDR:
    case GFA_PETRA_SYNT_TYPE_COMBO:       
    case GFA_PETRA_SYNT_TYPE_NIF:
      nominalfreq = 125000000;
      break;
    case GFA_PETRA_SYNT_TYPE_FABRIC:
      nominalfreq = 312500000;
      break;

    default:
      break;
    }
  }
  else if ((Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) || (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2))
  {
    switch(targetsynt)
    {
    case GFA_PETRA_SYNT_TYPE_QDR:
    case GFA_PETRA_SYNT_TYPE_CORE:
    case GFA_PETRA_SYNT_TYPE_DDR:
      nominalfreq = 100000000;
      break;
    case GFA_PETRA_SYNT_TYPE_PCP_CORE:
    case GFA_PETRA_SYNT_TYPE_PCP_ELK:
    case GFA_PETRA_SYNT_TYPE_PHY:
      nominalfreq = 125000000;
      break;
    case GFA_PETRA_SYNT_TYPE_FABRIC:
    case GFA_PETRA_SYNT_TYPE_COMBO:       
    case GFA_PETRA_SYNT_TYPE_NIF:
      nominalfreq = 312500000;
      break;
    default:
      break;
    }
#if DEBUG_GFA_BI_BOARD
    soc_sand_os_printf("gfa_petra_board_synt_nominal_freq_get. gfa-bi. targetsynt=%d, nominalfreq=%d\n",targetsynt,nominalfreq);
#endif
  }
  else 
  {
    /*
    *	Soc_petra-A
    */
    nominalfreq = 125000000;
  }
  
  return nominalfreq;
}

SOC_SAND_RET
  gfa_petra_board_synt_set(
   SOC_SAND_IN GFA_TG_SYNT_TYPE   targetsynt,
   SOC_SAND_IN uint32           targetfreq,
   SOC_SAND_IN uint8          silent
  )
{
   uint8 synt_id = 0;
   uint32 nominalfreq = 0;
   SOC_SAND_RET ret;


   if (((Gfa_petra_board_info.card_type != LINE_CARD_GFA_PETRA_B_INTERLAKEN) && (Gfa_petra_board_info.card_type != LINE_CARD_GFA_PETRA_B_INTERLAKEN_2)) && 
      ((targetsynt == GFA_PETRA_SYNT_TYPE_PCP_CORE) || (targetsynt == GFA_PETRA_SYNT_TYPE_PCP_ELK) || (targetsynt == GFA_PETRA_SYNT_TYPE_PHY)))
   {
      /* this targetsynt are only on gfa-bi */
	  soc_sand_os_printf("gfa_petra_board_synt_set(). this targetsynt are only on gfa-bi. targetsynt=%d \n",targetsynt);
      return SOC_SAND_OK;
   }

#ifdef __DUNE_GTO_BCM_CPU__
   gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.puc_1.i2c_read_mux), 0x1);
#endif

   /*
    *	Save current configuration
    */
   if (targetsynt < GFA_NOF_SYNT_TYPES)
   {
      Gfa_petra_synt_values[targetsynt] = targetfreq;
   }

   nominalfreq = gfa_petra_board_synt_nominal_freq_get(targetsynt);

   if ((Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN) || (Gfa_petra_board_info.card_type == LINE_CARD_GFA_PETRA_B_INTERLAKEN_2))
   {
     switch(targetsynt)
     {
     case GFA_PETRA_SYNT_TYPE_QDR:
     case GFA_PETRA_SYNT_TYPE_DDR:
     case GFA_PETRA_SYNT_TYPE_CORE:
     case GFA_PETRA_SYNT_TYPE_PCP_CORE:
     case GFA_PETRA_SYNT_TYPE_PCP_ELK:
     case GFA_PETRA_SYNT_TYPE_PHY:
       synt_id = 0x60;
       break;
     case GFA_PETRA_SYNT_TYPE_COMBO:
     case GFA_PETRA_SYNT_TYPE_FABRIC:
     case GFA_PETRA_SYNT_TYPE_NIF:
       synt_id = 0x55;
       break;
     default:
       break;
     }      
   }
   else 
   {
     /* all other boards: gfa b, load board*/
     synt_id = SYNTHESIZER_ID;
   }

   Gfa_petra_board_info.i2c_syntmuxset(targetsynt);
#if DEBUG_GFA_BI_BOARD
   soc_sand_os_printf("gfa_petra_board_synt_set(). targetsynt=%d, targetfreq=%d, synt_id=0x%x, nominalfreq=%d\n",targetsynt,targetfreq,synt_id,nominalfreq);
#endif
   ret = gfa_petra_board_synt_set_imp(targetsynt, targetfreq, TRUE, synt_id, nominalfreq);
#ifdef __DUNE_GTO_BCM_CPU__
   gfa_petra_fpga_io_fld_set(&(Gfa_petra_fpga_io_regs.puc_1.i2c_read_mux), 0x0);
#endif

   return ret;
}

const char*
  soc_petra_GFA_PETRA_FRONT_INTERFACE_TYPE_to_string(
    SOC_SAND_IN GFA_PETRA_FRONT_INTERFACE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case GFA_PETRA_FRONT_INTERFACE_PHY_XAUI:
    str = "XAUI-PHY";
  break;

  case GFA_PETRA_FRONT_INTERFACE_PHY_SGMII:
    str = "SGMII";
  break;

  case GFA_PETRA_FRONT_INTERFACE_PTG_XAUI:
    str = "XAUI-PTG";
  break;

  case GFA_PETRA_FRONT_INTERFACE_PTG_SPAUI:
    str = "SPAUI";
  break;

  case GFA_PETRA_FRONT_INTERFACE_PTG_FAT_PIPE:
    str = "SPAUI";
  break;
  default:
    str = " Unknown type";
  }
  return str;
}

#ifndef __DUNE_GTO_BCM_CPU__
int 
  lb_bsp_gpio_op(
    SOC_SAND_IN uint8 io_val,
    SOC_SAND_IN uint8 in_data_val,
    SOC_SAND_OUT uint8 *out_data_val)
{
  uint32
    offset;

  offset = (uint32)(&(((EPLD_REGS *)0)->gp_bus_data));
  write_epld_reg((uint8)in_data_val,(uint32)offset);

  offset = (uint32)(&(((EPLD_REGS *)0)->gp_io_conf));
  write_epld_reg((uint8)io_val,(uint32)offset);
  
  offset = (uint32)(&(((EPLD_REGS *)0)->gp_bus_data));
  *out_data_val = read_epld_reg((uint32)offset);

  return SOC_SAND_OK;
}
#endif /* __DUNE_GTO_BCM_CPU__ */
/* } */                                                             

