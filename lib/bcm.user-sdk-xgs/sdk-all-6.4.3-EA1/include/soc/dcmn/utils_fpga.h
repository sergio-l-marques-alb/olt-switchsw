
/* $Id: utils_fpga.h,v 1.2 Broadcom SDK $
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

#ifndef _DCMN_UTILS_FPGA_H_
#define _DCMN_UTILS_FPGA_H_

#define SOC_DPP_FPGA_BUFF_SIZE (80 * 1024 * 1024)

#ifdef __DUNE_WRX_BCM_CPU__
/* REGS ADDR */
#define SOC_DPP_GPOUTDR_PIN_REG_ADDR    0x34108 /* 0x000E0040 */
#define SOC_DPP_GPINDR_PIN_REG_ADDR     0x34110 /* 0x000E0050 */

/* GPIO pins used for FPGA */
#define GPIO_CONFDONE_PIN_READ 16 /* connector pin 49 */
#define GPIO_NCONFIG_PIN_WRITE 12 /* connector pin 54 */
#define GPIO_DATA0_PIN_WRITE   13 /* connector pin 50 */
#define GPIO_DCLK_PIN_WRITE    14 /* connector pin 48 */
#define GPIO_NSTATUS_PIN_READ  11 /* connector pin 58 */
#define GPIO_CONFDONE_MASK     (1 << GPIO_CONFDONE_PIN_READ)
#define GPIO_NCONFIG_MASK      (1 << GPIO_NCONFIG_PIN_WRITE)
#define GPIO_DATA0_MASK        (1 << GPIO_DATA0_PIN_WRITE)
#define GPIO_DCLK_MASK         (1 << GPIO_DCLK_PIN_WRITE)
#define GPIO_NSTATUS_MASK      (1 << GPIO_NSTATUS_PIN_READ)

/* GPOUT REGISTER BITS */
#define SOC_DPP_GPOUTDR_NCONFIG_BIT   GPIO_NCONFIG_MASK /*     0x00200000 */ /* gpout_1 = gpout[10] revert bits */
#define SOC_DPP_GPOUTDR_DATA_BIT      GPIO_DATA0_MASK /*   0x00100000 */ /* gpout_2 = gpout[11] revert bits */
#define SOC_DPP_GPOUTDR_DCLK_BIT      GPIO_DCLK_MASK /*   0x00080000 */ /* gpout_3 = gpout[12] revert bits */

/* GPIN REGISTER BITS */
#define SOC_DPP_GPINDR_CONF_DONE_BIT  GPIO_CONFDONE_MASK /*  0x00800000 */ /* gpin_5 = gpin[8] revert bits */
#define SOC_DPP_GPINDR_NSTATUS_BIT    GPIO_NSTATUS_MASK /*    0x00400000 */ /* gpin_0 = gpin[9] revert bits */

#else /* __DUNE_WRX_BCM_CPU__ */

/* REGS ADDR */
#define SOC_DPP_GPOUTDR_PIN_REG_ADDR	0x000E0040
#define SOC_DPP_GPINDR_PIN_REG_ADDR		0x000E0050

/* GPOUT REGISTER BITS */
#define SOC_DPP_GPOUTDR_NCONFIG_BIT		0x00200000 /* gpout_1 = gpout[10] revert bits */
#define SOC_DPP_GPOUTDR_DATA_BIT		0x00100000 /* gpout_2 = gpout[11] revert bits */
#define SOC_DPP_GPOUTDR_DCLK_BIT		0x00080000 /* gpout_3 = gpout[12] revert bits */

/* GPIN REGISTER BITS */
#define SOC_DPP_GPINDR_CONF_DONE_BIT	0x00800000 /* gpin_5 = gpin[8] revert bits */
#define SOC_DPP_GPINDR_NSTATUS_BIT		0x00400000 /* gpin_0 = gpin[9] revert bits */

#endif

typedef struct
{
  int prog_reg_addr; /* data */
  int status_reg_addr;
  char conf_done_bit_offset; /* offset of done bit in status register */  
} SocDppFpgaRegsMapping;

extern int soc_dpp_fpga_load(int unit, char *file_name);
#endif /* _DCMN_UTILS_FPGA_H_ */
