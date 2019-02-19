/* $Id: utils_i2c_mem.h,v 1.2 Broadcom SDK $
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


#ifndef __UTILS_I2C_MEM_H_INCLUDED__
/* { */
#define __UTILS_I2C_MEM_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


#define I2C_MEM_MAX_IC2_BUFF_SIZE  32


#define I2C_MEM_ERR_BASE 6660

#define I2C_MEM_ERR_01 (I2C_MEM_ERR_BASE + 1)
#define I2C_MEM_ERR_02 (I2C_MEM_ERR_BASE + 2)
#define I2C_MEM_ERR_03 (I2C_MEM_ERR_BASE + 3)
#define I2C_MEM_ERR_04 (I2C_MEM_ERR_BASE + 4)
#define I2C_MEM_ERR_05 (I2C_MEM_ERR_BASE + 5)
#define I2C_MEM_ERR_06 (I2C_MEM_ERR_BASE + 6)
#define I2C_MEM_ERR_07 (I2C_MEM_ERR_BASE + 7)
#define I2C_MEM_ERR_08 (I2C_MEM_ERR_BASE + 8)
#define I2C_MEM_ERR_09 (I2C_MEM_ERR_BASE + 9)
#define I2C_MEM_ERR_10 (I2C_MEM_ERR_BASE + 10)

/*
 * }
 */


int
  utils_i2c_mem_read(
    unsigned char device_address,/*7 bits*/
    unsigned char i2c_data[I2C_MEM_MAX_IC2_BUFF_SIZE],
    unsigned short  i2c_data_len,
    unsigned int  silent
  );


int
  utils_i2c_mem_write(
    unsigned char device_address,/*7 bits*/
    unsigned char i2c_data[I2C_MEM_MAX_IC2_BUFF_SIZE],
    unsigned short  i2c_data_len,
    unsigned int  silent
  );


#ifdef  __cplusplus
}
#endif


/* } __UTILS_I2C_MEM_H_INCLUDED__*/
#endif



