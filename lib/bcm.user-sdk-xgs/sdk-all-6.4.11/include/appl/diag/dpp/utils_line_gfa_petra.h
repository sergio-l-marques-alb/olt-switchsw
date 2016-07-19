/* $Id: utils_line_gfa_petra.h,v 1.9 Broadcom SDK $
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


#ifndef __UTILITIES_UTILS_LINE_GFA_PETRA_H_INCLUDED__
/* { */
#define __UTILITIES_UTILS_LINE_GFA_PETRA_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#if !DUNE_BCM

#include "Pub/include/ref_sys.h"

#ifdef __VXWORKS__

  #include "vxWorks.h"
  #include "stdio.h"
  #include "string.h"
  #include "ctype.h"
  #include "stdlib.h"
  #include "taskLib.h"
  #include "errnoLib.h"
  #include "sysLib.h"
  #include "usrLib.h"
  #include "tickLib.h"
  #include "ioLib.h"
  #include "iosLib.h"
  #include "logLib.h"
  #include "pipeDrv.h"
  #include "timers.h"
  #include "sigLib.h"
  #include "private\timerLibP.h"
  #include "logLib.h"
  #include "taskHookLib.h"
  #include "shellLib.h"
  #include "dbgLib.h"

#endif

#include "Pub/include/dune_chips.h"
#include "Pub/include/utils_defx.h"

#include "Utilities/include/utils_host_board.h"
#include "Utilities/include/utils_host_epld_io_agent.h"
#include "Utilities/include/utils_dune_fpga_download.h"

#include "DuneDriver/SAND/Utils/include/sand_framework.h"
#include "DuneDriver/Petra/include/petra_api_mgmt.h"

#else
#ifndef B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN
#define B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN 7
#endif

#ifndef __DUNE_GTO_BCM_CPU__
#include <appl/diag/dpp/utils_host_board.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_api_dram.h>
#endif 
#endif /* !DUNE_BCM */

#include "appl/diag/dcmn/bsp_cards_consts.h"

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* GFA Soc_petra board definitions { */
/* GFA Soc_petra addresses spaces { */

#define GFA_PETRA_PETRA_BASE_ADDR                 (0x40000000)
#define GFA_PETRA_STREAMING_BASE_ADDR             (0x40100000)
#define GFA_PETRA_IO_FPGA_BASE_ADDR               (0x40200000)
#define GFA_PETRA_FILE_SYSTEM_FLASH_BASE_ADDR     (0x50800000)

#define GFA_PETRA_PETRA_SIZE                      (0x00100000)
#define GFA_PETRA_STREAMING_SIZE                  (0x00100000)
#define GFA_PETRA_IO_FPGA_SIZE                    (0x00100000)
#define GFA_PETRA_FILE_SYSTEM_FLASH_SIZE          (0x00100000)

/* } GFA Soc_petra addresses spaces */
/* GFA Soc_petra reset lines { */
#define GFA_PETRA_DEVICE_RESET_STREAMING_FPGA     (SOC_SAND_BIT(1))
#define GFA_PETRA_DEVICE_RESET_IO_AGENT_FPGA      (SOC_SAND_BIT(2))
#define GFA_PETRA_DEVICE_RESET_FILE_SYSTEM_FLASH  (SOC_SAND_BIT(3))

/* } GFA Soc_petra reset lines */
/* } GFA Soc_petra board definitions */

/* IO_AGENT_FPGA { */
#define GFA_PETRA_IO_AGENT_FPGA_FILE              "FPG0003A.rbf"
#define GFA_PETRA_IO_AGENT_FPGA_FILE_STREAMING    GFA_PETRA_IO_AGENT_FPGA_FILE /* In case the streaming board's IO FPGA will change in the future please change this define to the desired file name */
#define GFA_PETRA_B_IO_AGENT_FPGA_FILE            "FPG0003C.rbf"
#define GFA_PETRA_B_IO_AGENT_FPGA_FILE_STREAMING  GFA_PETRA_B_IO_AGENT_FPGA_FILE /* In case the streaming board's IO FPGA will change in the future please change this define to the desired file name */
#define GFA_PETRA_B_WITH_PETRA_A_IO_AGENT_FPGA_FILE            "FPG0003B.rbf"
#define GFA_PETRA_B_WITH_PETRA_A_IO_AGENT_FPGA_FILE_STREAMING  GFA_PETRA_B_WITH_PETRA_A_IO_AGENT_FPGA_FILE /* In case the streaming board's IO FPGA will change in the future please change this define to the desired file name */
  
#define GFA_PETRA_IO_AGENT_FPGA_GPIO              (SOC_SAND_SET_FLD_IN_PLACE(0x1, 7, SOC_SAND_BIT(7))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 6, SOC_SAND_BIT(6))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 1, SOC_SAND_BIT(1))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 0, SOC_SAND_BIT(0)))
/* } IO_AGENT_FPGA */

  /* STREAMING_FPGA { */
#define GFA_PETRA_STREAMING_FPGA_FILE             "FPG0004A.rbf"
#define GFA_PETRA_STREAMING_FPGA_FILE_STREAMING   "FPG0008A.rbf"
#define GFA_PETRA_B_STREAMING_FPGA_FILE_STREAMING "FPG0008B.rbf"
#define GFA_PETRA_STREAMING_FPGA_GPIO             (SOC_SAND_SET_FLD_IN_PLACE(0x1, 7, SOC_SAND_BIT(7))|SOC_SAND_SET_FLD_IN_PLACE(0x1, 6, SOC_SAND_BIT(6))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 1, SOC_SAND_BIT(1))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 0, SOC_SAND_BIT(0)))
  /* } STREAMING_FPGA */

/* FILE_SYSTEM_FLASH { */
#define GFA_PETRA_FILE_SYSTEM_FLASH_ADDR          (GFA_PETRA_FILE_SYSTEM_FLASH_BASE_ADDR)
#define GFA_PETRA_FILE_SYSTEM_FLASH_MAX_ADDR      (GFA_PETRA_FILE_SYSTEM_FLASH_ADDR + GFA_PETRA_FILE_SYSTEM_FLASH_SIZE - 1)
#define GFA_PETRA_FILE_SYSTEM_FLASH_SIZE_TO_USE   (GFA_PETRA_FILE_SYSTEM_FLASH_SIZE - 12)
#define GFA_PETRA_FILE_SYSTEM_FLASH_TYPE          (FLASH_28F640)
#define GFA_PETRA_FILE_SYSTEM_FLASH_SECTOR_SIZE   (FLASH_28F320_SECTOR) /* two flashes*/
#define GFA_PETRA_FILE_SYSTEM_FLASH_ADRS_JUMP     (1) /* 4 bytes alignment */
#define GFA_PETRA_FILE_SYSTEM_FLASH_WIDTH         (4) /* two flashes-> 4 bytes alignment */
/* } FILE_SYSTEM_FLASH */

#define LB_BSP_I2C_BUFFER_SIZE  24

/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define GFA_PETRA_BUZY_WAIT(nof_loops) {for(wait_i=0;wait_i<nof_loops;wait_i++);}

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint8 address;
  uint8 msb;
  uint8 lsb;
}__ATTRIBUTE_PACKED__ GFA_PETRA_FLD_ADDR;

/*
 *  Traffic Generator types supported by Soc_petra sweep
 */
typedef enum
{
  GFA_TG_TYPE_NONE,
  GFA_TG_TYPE_EXTERNAL,
  GFA_TG_TYPE_PTG,
  GFA_NOF_TG_TYPES
}GFA_TG_TYPE;

typedef enum
{
  GFA_PETRA_DDR2_MICRON_K4T51163QE_ZC_LF7,
  GFA_PETRA_DDR3_SAMSUNG_K4B1G1646E_HCK0,
  GFA_PETRA_DDR3_MICRON_MT41J64M16_15E,
  GFA_PETRA_GDD3_SAMSUNG_K4J52324QE
}GFA_PETRA_DRAM_TYPE;
      
typedef enum {
  GFA_PETRA_FRONT_INTERFACE_PHY_XAUI     = 0,
  GFA_PETRA_FRONT_INTERFACE_PHY_SGMII    = 1,
  GFA_PETRA_FRONT_INTERFACE_PTG_XAUI     = 2,
  GFA_PETRA_FRONT_INTERFACE_PTG_SPAUI    = 3,
  GFA_PETRA_FRONT_INTERFACE_PTG_FAT_PIPE = 4,
  GFA_PETRA_FRONT_INTERFACE_TMN_SPAUI    = 5
} GFA_PETRA_FRONT_INTERFACE_TYPE;

typedef enum
{
  GFA_PETRA_SYNT_TYPE_FABRIC =   1,
  GFA_PETRA_SYNT_TYPE_COMBO  =   2,
  GFA_PETRA_SYNT_TYPE_NIF    =   3,
  GFA_PETRA_SYNT_TYPE_CORE   =   4,
  GFA_PETRA_SYNT_TYPE_DDR    =   5,
  GFA_PETRA_SYNT_TYPE_QDR    =   6,
  GFA_PETRA_SYNT_TYPE_SYNCE  =   7,
  GFA_PETRA_SYNT_TYPE_PCP_CORE = 8,
  GFA_PETRA_SYNT_TYPE_PCP_ELK  = 9,
  GFA_PETRA_SYNT_TYPE_PHY      = 10,
  GFA_NOF_SYNT_TYPES
}GFA_TG_SYNT_TYPE;

typedef struct  {
  uint32 fabric_freq;
  uint32 combo_freq;
  uint32 nif_freq;
  uint32 core_freq;
  uint32 ddr_freq;
  uint32 qdr_freq;
}GFA_PETRA_B_BSP_INTER_FREQ;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
int
  gfa_petra_board_fpga_synce_muxset(void);

int  
  gfa_petra_fpga_i2c_write (
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8           *buffer
  );

int  
  gfa_petra_fpga_i2c_read (
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8            *buffer
 );

uint8
  gfa_petra_is_look_aside_get(void
  );

SOC_SAND_RET
  gfa_petra_hw_reset(
    SOC_SAND_IN  uint8  in_reset_not_out_of_reset
  );

SOC_SAND_RET
  gfa_petra_core_pll_reset(
    SOC_SAND_IN  uint8  in_reset_not_out_of_reset
  );

SOC_SAND_RET
  gfa_petra_reset_device(
    SOC_SAND_IN  uint32                   delay_to_be_down_mili_sec,
    SOC_SAND_IN  uint32                  stay_down
  );

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_init_host_board(
    SOC_SAND_IN  char                       board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN],
    SOC_SAND_IN  uint16                   gfa_board_serial_number,
    SOC_SAND_IN SOC_BSP_CARDS_DEFINES           card_type,
    SOC_SAND_OUT uint32                  *chip_ver
  );
#endif

SOC_SAND_RET
  gfa_petra_front_interface_type_get(
    SOC_SAND_OUT GFA_PETRA_FRONT_INTERFACE_TYPE *nif_type,
    SOC_SAND_IN  uint8                      silent
  );

SOC_SAND_RET
  gfa_petra_reset(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  remain_active
  );

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_drc_conf_load(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_DDR           *drc_conf,
    SOC_SAND_IN  uint32                  freq,
    SOC_SAND_IN  uint8                  silent
  );
#endif

SOC_SAND_RET
  gfa_petra_board_dram_frequency_get(
    SOC_SAND_OUT uint32                   *dram_frequency
  );

SOC_SAND_RET
  gfa_petra_board_dram_ref_clock_get(
    SOC_SAND_OUT uint32                   *ref_clock
  );

SOC_SAND_RET
  gfa_petra_board_core_frequency_get(
    SOC_SAND_OUT uint32                   *core_frequency
  );

SOC_SAND_RET
  gfa_petra_connected_links_get(
    SOC_SAND_OUT uint32                  **connected_links
  );

uint8
  gfa_petra_is_line_card_connected(void
  );

SOC_SAND_RET
  gfa_petra_synt_assert(void
  );

#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET
  gfa_petra_dram_type_get(
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE *dram_type
  );
#endif

SOC_SAND_RET
  gfa_petra_puc_pll_set(
    SOC_SAND_IN uint8 pll_m,
    SOC_SAND_IN uint8 pll_n,
    SOC_SAND_IN uint8 pll_p
  );

SOC_SAND_RET
  gfa_petra_board_synt_set(
   SOC_SAND_IN GFA_TG_SYNT_TYPE   syntsizer,
   SOC_SAND_IN uint32           targetfreq,
   SOC_SAND_IN uint8          silent
  );

uint32
  gfa_petra_board_synt_nominal_freq_get(
    SOC_SAND_IN GFA_TG_SYNT_TYPE   targetsynt
  );

const char*
  soc_petra_GFA_PETRA_FRONT_INTERFACE_TYPE_to_string(
    SOC_SAND_IN GFA_PETRA_FRONT_INTERFACE_TYPE enum_val
  );

SOC_SAND_RET
  gfa_petra_streaming_agent_start(void);

int
  lb_bsp_i2c_write_gen_inner(
    uint8  unit,
    uint8  write_data[LB_BSP_I2C_BUFFER_SIZE],
    unsigned short write_data_arr_entries,
    uint8  internal_address,
    SOC_SAND_IN uint8 internal_address_valid
  );

int  
  lb_petra_i2c_read (
    uint8            unit,
    uint8            internal_address,
    uint8            buffer_size,
    uint8           *buffer
  );

SOC_SAND_RET
  gfa_petra_fpga_io_regs_init(void
  );

SOC_SAND_RET
  Gfa_petra_GFA_PETRA_BOARD_SPECIFICATIONS_clear(
    SOC_SAND_IN SOC_BSP_CARDS_DEFINES  card_type
  );
  
#ifndef __DUNE_GTO_BCM_CPU__
SOC_SAND_RET 
  gfa_petra_init_host_board_860(void
  );

SOC_SAND_RET
  gfa_petra_fpga_download_and_burn(
    SOC_SAND_IN  char                       *file_name,
    SOC_SAND_IN  uint8                    gpio_value,
    SOC_SAND_IN  uint8                  silent
  );

SOC_SAND_RET 
  gfa_petra_fpga_io_agent_init(
    SOC_SAND_IN  uint8                  silent
  );
#endif

uint8
  gfa_petra_fpga_io_fld_get(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld
  );

void
  gfa_petra_fpga_io_fld_set(
    SOC_SAND_IN  GFA_PETRA_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  );


/* } */


#ifdef  __cplusplus
}
#endif

/* } __UTILITIES_UTILS_LINE_GFA_PETRA_H_INCLUDED__*/
#endif

