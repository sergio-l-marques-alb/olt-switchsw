/* 
 * $Id: utils_fe1600_card.h,v 1.6 Broadcom SDK $
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
 *
 * File:        utils_fe1600_card.h
 * Purpose:     fe1600 card Utilities Header.
 *
 */

#ifndef __UTILITIES_UTILS_FE1600_CARD_H_INCLUDED__
/* { */
#define __UTILITIES_UTILS_FE1600_CARD_H_INCLUDED__

/*************                                                      
 * INCLUDES  *                                                      
 *************/                                                     
/* { */

#include <sal/types.h>
#include <appl/diag/dcmn/bsp_cards_consts.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* IO_AGENT_FPGA { */
#define FE1600_CARD_IO_AGENT_FPGA_FILE              "fe1600_card_io_agent.rbf"

/* I2C Adress */
#define FE1600_CARD_I2C_IO_AGENT_DEVICE_ADDR    0x40
#define FE1600_CARD_I2C_FE1600_DEVICE_ADDR          0x44
#define FE1600_CARD_I2C_GPIO_DEVICE_ADDR             0x20

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  FE1600_CARD_SYNT_SERDES_0 =   0,
  FE1600_CARD_SYNT_SERDES_1 =   1,
  FE1600_CARD_SYNT_SERDES_2 =   2,
  FE1600_CARD_SYNT_SERDES_3 =   3,
  FE1600_CARD_SYNT_CORE     =   4,
  FE1600_CARD_NOF_SYNT_TYPES
} FE1600_CARD_SYNT_TYPE;

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

/* 
 * Board functions 
 */
uint32
    fe1600_card_fpga_io_regs_init(void);

uint32
  fe1600_card_init_host_board(
    const  SOC_BSP_CARDS_DEFINES          card_type
  );

uint32 
  fe1600_BOARD_SPECIFICATIONS_clear( 
    const SOC_BSP_CARDS_DEFINES  card_type 
  );

/* 
 * Board Utility Functions 
 */
uint32
    fe1600_card_power_monitor(
        int eq_sel
    );

uint32
    fe3200_card_power_monitor(
        int eq_sel
    );

/* 
 * Synt Functions 
 */
uint32
  fe1600_card_board_synt_set(
   const FE1600_CARD_SYNT_TYPE targetsynt,
   const uint32               targetfreq,
   const unsigned char         silent
  );

uint32 
  fe1600_card_board_synt_nominal_freq_get( 
    const FE1600_CARD_SYNT_TYPE   targetsynt 
  );

uint32 fe3200_card_board_synt_set(uint32 targetfreq, uint32 nominalfreq);
/* } */

#endif /*  __UTILITIES_UTILS_LINE_GFA_PETRA_H_INCLUDED__ */
