 /* $Id: soc_dpp_i2c.h,v 1.2 Broadcom SDK $
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
#ifndef SOC_DPP_I2C_H
#define SOC_DPP_I2C_H

#ifdef  __cplusplus
extern "C" {
#endif

/* #include "utils/include/d_ssf_defs.h" */
  

#define SOC_DPP_I2C_DEVICE_FILE_NAME_PREFIX "/dev/i2c-" /* i2c device is SOC_DPP_I2C_DEVICE_FILE_NAME_PREFIX+device number*/

#define SOC_DPP_I2C_MEM_ACCESS_FD_INVALID (-1)
  
typedef enum 
{
  SOC_DPP_I2C_ALEN_CHAR = 0x1,
  SOC_DPP_I2C_ALEN_WORD = 0x2,
  SOC_DPP_I2C_ALEN_LONG = 0x4,
  SOC_DPP_I2C_ALEN_CHAR_DLEN_WORD = 0x12, 
  SOC_DPP_I2C_ALEN_CHAR_DLEN_LONG = 0x14, /*not implemented*/
  SOC_DPP_I2C_ALEN_WORD_DLEN_LONG = 0x24,
  SOC_DPP_I2C_ALEN_LONG_DLEN_WORD = 0x42, /*not implemented*/
  SOC_DPP_I2C_ALEN_LONG_DLEN_LONG = 0x44, /*not implemented*/
  SOC_DPP_I2C_ALEN_WORD_DLEN_CHAR = 0x21, /*not implemented*/
  SOC_DPP_I2C_ALEN_LONG_DLEN_CHAR = 0x41 /*not implemented*/
} SOC_DPP_I2C_BUS_LEN;

typedef struct  
{
  int file_desc;
  char chip; /* 7 bit address of the device */
  int addr;
  int alen; /* SOC_DPP_I2C_ALEN_* */
  int data;
  int no_addr; /* no address is needed - only data (then the addr field is taken as data) - such as MUX devices */
} SocDppI2CAccess;

int soc_dpp_i2c_main(int argc, char* argv[]);

/*
 * soc_dpp_i2c_init
 *    p_fd - If init returned successfully this hold the file desc. to the I2C device
 *    dev_num - The requested I2C device to open
 *    mem_access_fd - file descriptor to mem access, in order to enable bus speedup.
 *                    Use SOC_DPP_I2C_MEM_ACCESS_FD_INVALID if not used. 
 */
int soc_dpp_i2c_init(int* p_fd, char dev_num);

/*
 * soc_dpp_i2c_deinit
 *    fd - Close the given I2C file desc.
 */
int soc_dpp_i2c_deinit(int fd);

/*
 * soc_dpp_i2c_lock
 *    Should be called before every SHORT I2C r/w sequence
 */
int soc_dpp_i2c_lock(void);

/*
 * soc_dpp_i2c_unlock
 *    After the r/w sequence is done release the lock - this MUST be done
 */
int soc_dpp_i2c_unlock(void);

/*
 * soc_dpp_i2c_read
 *    p_i2c_access - See I2C access struct above
 */
int soc_dpp_i2c_read(SocDppI2CAccess* p_i2c_access);

/*
 * soc_dpp_i2c_write
 *    p_i2c_access - See I2C access struct above
 */
int soc_dpp_i2c_write(SocDppI2CAccess* p_i2c_access);

/* supported card types */
typedef enum 
{
  SOC_DPP_I2C_CARD_LINE = 0,
  SOC_DPP_I2C_CARD_FABRIC,
  SOC_DPP_I2C_CARD_MESH,
  SOC_DPP_I2C_CARD_MGMT,
  SOC_DPP_I2C_CARD_CHASSIS,
  SOC_DPP_I2C_CARD_NUM_CARDS
} SOC_DPP_I2C_CARD;

/* supported chips for each card */
typedef enum 
{
  /* NOTE: do not change order! 
   *   SOC_DPP_I2C_CHIP_LINE_xxx must start with 0
   *   In S_a_line_mux_vals array, entry S_a_line_mux_vals[SOC_DPP_I2C_CHIP_LINE_xxx]
   *   corresponds to chip SOC_DPP_I2C_CHIP_LINE_xxx
   */
  SOC_DPP_I2C_CHIP_LINE_EEPROM = 0,
  SOC_DPP_I2C_CHIP_LINE_FPGA,
  SOC_DPP_I2C_CHIP_LINE_TEMP_0,
  SOC_DPP_I2C_CHIP_LINE_TEMP_1,
  SOC_DPP_I2C_CHIP_LINE_TEMP_2,
  SOC_DPP_I2C_CHIP_LINE_AD_0,
  SOC_DPP_I2C_CHIP_LINE_AD_1,
  SOC_DPP_I2C_CHIP_LINE_AD_2,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_0,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_1,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_2,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_3,
  SOC_DPP_I2C_CHIP_LINE_NUM_CHIPS,
  
  /* NOTE: do not change order! 
   *   SOC_DPP_I2C_CHIP_FABRIC_xxx must start with 0
   *   In S_a_fabric_mux_vals array, entry S_a_fabric_mux_vals[SOC_DPP_I2C_CHIP_FABRIC_xxx]
   *   corresponds to chip SOC_DPP_I2C_CHIP_FABRIC_xxx
   */
  SOC_DPP_I2C_CHIP_FABRIC_AD_0  = 0, 
  SOC_DPP_I2C_CHIP_FABRIC_GPIO_0, 
  SOC_DPP_I2C_CHIP_FABRIC_GPIO_1, 
  SOC_DPP_I2C_CHIP_FABRIC_TEMP_0, 
  SOC_DPP_I2C_CHIP_FABRIC_EEPROM, 
  SOC_DPP_I2C_CHIP_FABRIC_FAN_0, 
  SOC_DPP_I2C_CHIP_FABRIC_FE_0, 
  SOC_DPP_I2C_CHIP_FABRIC_FPGA,
  SOC_DPP_I2C_CHIP_FABRIC_NUM_CHIPS,

  /* NOTE: do not change order! 
   *   SOC_DPP_I2C_CHIP_MESH_xxx must start with 0
   *   In S_a_mesh_mux_vals array, entry S_a_mesh_mux_vals[SOC_DPP_I2C_CHIP_MESH_xxx]
   *   corresponds to chip SOC_DPP_I2C_CHIP_MESH_xxx
   */
  SOC_DPP_I2C_CHIP_MESH_EEPROM  = 0, 
  SOC_DPP_I2C_CHIP_MESH_FAN_0,
  SOC_DPP_I2C_CHIP_MESH_FAN_1,
  SOC_DPP_I2C_CHIP_MESH_TEMP_0,
  SOC_DPP_I2C_CHIP_MESH_NUM_CHIPS,

  /* NOTE: do not change order! 
   *   SOC_DPP_I2C_CHIP_MGMT_xxx must start with 0
   *   In S_a_mgmt_mux_vals array, entry S_a_mgmt_mux_vals[SOC_DPP_I2C_CHIP_MGMT_xxx]
   *   corresponds to chip SOC_DPP_I2C_CHIP_MGMT_xxx
   */
  SOC_DPP_I2C_CHIP_MGMT_EEPROM  = 0, 
  SOC_DPP_I2C_CHIP_MGMT_TEMP_0 , 
  SOC_DPP_I2C_CHIP_MGMT_TEMP_1 , 
  SOC_DPP_I2C_CHIP_MGMT_NUM_CHIPS,
  
  /* NOTE: do not change order! 
   *   SOC_DPP_I2C_CHIP_CHASSIS_xxx must start with 0
   *   In S_a_chassis_mux_vals array, entry S_a_chassis_mux_vals[SOC_DPP_I2C_CHIP_CHASSIS_xxx]
   *   corresponds to chip SOC_DPP_I2C_CHIP_CHASSIS_xxx
   */
  SOC_DPP_I2C_CHIP_CHASSIS_EEPROM  = 0, 
  SOC_DPP_I2C_CHIP_CHASSIS_NUM_CHIPS
} SOC_DPP_I2C_CHIP;


/* write value to the given address in the given chip */
int soc_dpp_i2c_write_external(int chip, int no_addr, int addr, SOC_DPP_I2C_BUS_LEN alen, int val);

/* read value from the given address in the given chip */
int soc_dpp_i2c_read_external(int chip, int no_addr, int addr, SOC_DPP_I2C_BUS_LEN alen, int* p_val);

#ifdef  __cplusplus
}
#endif

#endif /* SOC_DPP_I2C_H */
