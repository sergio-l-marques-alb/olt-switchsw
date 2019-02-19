  /* $Id: utils_line_gfa_bi.h,v 1.6 Broadcom SDK $
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


#ifndef __UTILITIES_UTILS_LINE_GFA_BI_H_INCLUDED__
/* { */
#define __UTILITIES_UTILS_LINE_GFA_BI_H_INCLUDED__

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
#include <appl/diag/dpp/utils_host_board.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_api_dram.h>
#endif /* !DUNE_BCM */
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define DEBUG_GFA_BI_BOARD                     0

#define GFA_BI_PETRA_BASE_ADDR                 (0x40000000)
#define GFA_BI_PCP_BASE_ADDR                   (0x40100000)
#define GFA_BI_IO_FPGA_BASE_ADDR               (0x40200000)

#define GFA_BI_PETRA_SIZE                      (0x00100000)
#define GFA_BI_PCP_SIZE                        (0x00100000)
#define GFA_BI_IO_FPGA_SIZE                    (0x00100000)

#define GFA_BI_BSP_I2C_BUFFER_SIZE             24
#define GFA_BI_BSP_I2C_INT_ADDR_LEN            4

#define GFA_BI_I2C_PS_AD_DEVICE_ADDR           0x35
#define GFA_BI_I2C_PCP_DEVICE_ADDR             0x40
#define GFA_BI_I2C_GPIO_1_DEVICE_ADDR          0x4d
#define GFA_BI_I2C_GPIO_2_DEVICE_ADDR          0x4c
#define GFA_BI_I2C_GPIO_3_DEVICE_ADDR          0x49
#define GFA_BI_I2C_GPIO_4_DEVICE_ADDR          0x4b

#define GFA_BI_I2C_GPIO_IOLATCH_INT_ADDR       0x70
#define GFA_BI_I2C_GPIO_DIRECTION_INT_ADDR     0x50
#define GFA_BI_I2C_GPIO_DATA_INT_ADDR          0x58


#define GFA_BI_DEVICE_RESET_PETRA_BIT          (0)
#define GFA_BI_DEVICE_RESET_PCP_FPGA_BIT       (1)
#define GFA_BI_DEVICE_RESET_IO_AGENT_FPGA_BIT  (2)
#define GFA_BI_DEVICE_RESET_HOT_SWAP_BIT       (7)


#define GFA_BI_IO_AGENT_FPGA_FILE_NAME         "gfa_bi_fpga_io_agent.rbf"
#define GFA_BI2_IO_AGENT_FPGA_FILE_NAME         "gfa_bi2_fpga_io_agent.rbf"
#define GFA_BI_PCP_FPGA_FILE_NAME              "gfa_bi_fpga_pcp.rbf"
       
/* } */

/*************
 * MACROS    *
 *************/
/* { */


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */


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

uint8
  gfa_bi_is_line_card_connected(void
  );

SOC_SAND_RET
  gfa_bi_fpga_io_regs_init(void
  );

SOC_SAND_RET
  gfa_bi_fpga_pcp_hw_reset(
    SOC_SAND_IN  uint8 in_reset_not_out_of_reset
  );

SOC_SAND_RET
  gfa_bi_init_i2c_devices(
	  SOC_BSP_CARDS_DEFINES          card_type
  );

SOC_SAND_RET
  gfa_bi_fpga_io_agent_start(
	  SOC_BSP_CARDS_DEFINES          card_type
  );

SOC_SAND_RET
  gfa_bi_init_host_board(
    SOC_SAND_IN  char                       board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN],
    SOC_SAND_IN  uint16                   gfa_board_serial_number,
    SOC_SAND_IN  SOC_BSP_CARDS_DEFINES          card_type,
    SOC_SAND_OUT uint32                  *chip_ver
  );

SOC_SAND_RET
  gfa_bi_utils_hot_swap_enable(
    SOC_SAND_IN  uint8                  silent,
    SOC_SAND_IN  uint32                   enable
  );

SOC_SAND_RET
  gfa_bi_ps_ad_read_single(
    SOC_SAND_IN  uint32                   channel,
    SOC_SAND_OUT uint8                    *buffer
  );

/* I2C Functions */
int
  gfa_bi_bsp_i2c_write_gen_inner(
    uint8 device_address,
    uint8 write_data[GFA_BI_BSP_I2C_BUFFER_SIZE],
    uint8 write_data_len
  );

int
  gfa_bi_bsp_i2c_write_gen(
    uint8 device_address,
    uint8 internal_address[GFA_BI_BSP_I2C_INT_ADDR_LEN],
    uint8 internal_address_len,
    uint8 write_data[GFA_BI_BSP_I2C_BUFFER_SIZE],
    uint8 write_data_len
  );

int
  gfa_bi_bsp_i2c_read_gen_inner(
    uint8  device_address,
    uint8  exp_read_len,
    uint8  read_data[GFA_BI_BSP_I2C_BUFFER_SIZE],
    unsigned short *nof_read_data_entries
  );

int
  gfa_bi_bsp_i2c_read_gen(
    uint8  device_address,
    uint8  internal_address[GFA_BI_BSP_I2C_INT_ADDR_LEN],
    uint8  internal_address_len,
    uint8  exp_read_len,
    uint32 *read_data,
    uint16 *read_data_len
  );

/* 
 * MDIO functions 
 */

SOC_SAND_RET
  gfa_bi_phy_single_aeluros_init(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  phy_ndx
  );

SOC_SAND_RET
  gfa_bi_phy_single_nlp1042_init(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  phy_ndx
  );

/* } */


#ifdef  __cplusplus
}
#endif

/* } __UTILITIES_UTILS_LINE_GFA_BI_H_INCLUDED__*/
#endif

