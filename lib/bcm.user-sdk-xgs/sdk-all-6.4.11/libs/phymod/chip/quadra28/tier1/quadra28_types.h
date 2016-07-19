/*
 *
 * $Id: quadra28_types.h 2014/04/02 aman Exp $
 *
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
 *
 *
 */
#ifndef __QUADRA28_TYPES_H__
#define __QUADRA28_TYPES_H__

/*
 * Includes
 */
#include <phymod/phymod_types.h>
/*
 *  Defines
 */
#define QUADRA28_MAX_LANE   4 

#define Q28_SPEED_1G                   1000
#define Q28_SPEED_10G                  10000
#define Q28_SPEED_HG11                 11000
#define Q28_SPEED_11P5G                11500
#define Q28_SPEED_12P5G                12500
#define Q28_SPEED_40G                  40000
#define SPEED_1G_OS8                   1000
#define SPEED_1G_OS8P25                0
#define Q28_SPEED_20G                  20000
#define Q28_SPEED_42G                  42000

typedef enum {
    Q28_SINGLE_PMD = 1,
    Q28_MULTI_PMD
} Q28_PMD_MODE;

/*Input Command Messages.*/
#define Q28_WR_CPU_CTRL_REGS    0x11    /* Write CPU/SPI Control Regs, followed by Count And CPU/SPI Controller Reg add/data pairs.*/
#define Q28_RD_CPU_CTRL_REGS    0xEE    /* Read CPU/SPI Control Regs, followed by Count and CPU/SPI Controller Register Add.*/
#define Q28_WR_CPU_CTRL_FIFO    0x66    /* Write CPU/SPI Control Regs Continously, followed by Count and CPU/SPI Controller Reg addr and data's.*/
#define Q28_SPI_CTRL_1_L        0xC000
#define Q28_SPI_CTRL_1_H        0xC002
#define Q28_SPI_CTRL_2_L        0xC400
#define Q28_SPI_CTRL_2_H        0xC402
#define Q28_SPI_TXFIFO          0xD000
#define Q28_SPI_RXFIFO          0xD400
/* SPI Controller Commands (known As messages).*/
#define Q28_MSGTYPE_HWR         0x40
#define Q28_MSGTYPE_HRD         0x80
#define Q28_WRSR_OPCODE         0x01
#define Q28_WR_OPCODE           0x02
#define Q28_RD_OPCODE           0x03
#define Q28_WRDI_OPCODE         0x04
#define Q28_RDSR_OPCODE         0x05
#define Q28_WREN_OPCODE         0x06
#define Q28_WR_BLOCK_SIZE       0x40    /* Maximum 64 Bytes Writes.*/
#define Q28_TOTAL_WR_BYTE       0xF000  /* Code Size is 60k bytes.*/
#define Q28_RETRY_CNT           5

#endif
