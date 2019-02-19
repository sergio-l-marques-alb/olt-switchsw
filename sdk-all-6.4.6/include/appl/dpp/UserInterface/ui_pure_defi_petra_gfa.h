/* $Id: ui_pure_defi_petra_gfa.h,v 1.2 Broadcom SDK $
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


#ifndef __UI_PURE_DEFI_PETRA_GFA_INCLUDED__
/* { */
#define __UI_PURE_DEFI_PETRA_GFA_INCLUDED__


#include <soc/dpp/SAND/Utils/sand_header.h>

#define PARAM_GFA_PETRA_SYNT_ASSERT_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 1)
#define PARAM_GFA_PETRA_SYNT_SET_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 2)
#define PARAM_GFA_PETRA_SYNT_SET_SYN_TYPE_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 3)
#define PARAM_GFA_PETRA_SYNT_SET_FREQ_VAL_ID  \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 4)
#define PARAM_GFA_PETRA_SYNT_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 5)
#define PARAM_GFA_PETRA_START_CSR_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 6)
#define PARAM_GFA_PETRA_GPIO_OP_IO_VAL_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 8)
#define PARAM_GFA_PETRA_GPIO_OP_DATA_VAL_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 9)
#define PARAM_GFA_PETRA_I2C_WRITE_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 10)
#define PARAM_GFA_PETRA_I2C_READ_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 11)
#define PARAM_GFA_PETRA_I2C_WRITE_DEV_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 12)
#define PARAM_GFA_PETRA_I2C_WRITE_REG_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 13)
#define PARAM_GFA_PETRA_I2C_WRITE_VAL_ID \
                       (PARAM_PETRA_GFA_START_RANGE_ID + 14)
#define PARAM_GFA_PETRA_I2C_READ_DEV_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 15)
#define PARAM_GFA_PETRA_I2C_READ_REG_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 16)
#define PARAM_GFA_PETRA_BI_HOT_SWAP_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 17)
#define PARAM_GFA_PETRA_BI_PS_AD_READ_ALL_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 18)
#define PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 19)
#define PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_DEV_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 20)
#define PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 21)
#define PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_LEN_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 22)
#define PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 23)
#define PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_LEN_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 24)
#define PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 25)
#define PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_DEV_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 26)
#define PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 27)
#define PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_LEN_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 28)
#define PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_EXP_READ_LEN_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 31)
#define PARAM_GFA_PETRA_BI_BSP_NLP1024_PHY_INIT_ID \
                      (PARAM_PETRA_GFA_START_RANGE_ID + 32)

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_PURE_DEFI_PETRA_GFA_INCLUDED__*/
#endif
