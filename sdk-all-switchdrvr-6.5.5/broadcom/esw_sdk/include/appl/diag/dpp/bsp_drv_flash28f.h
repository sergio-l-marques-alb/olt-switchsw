/* $Id: bsp_drv_flash28f.h,v 1.2 Broadcom SDK $
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

#ifndef __BSP_DRV_FLASH28F_INCLUDED
#define __BSP_DRV_FLASH28F_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* defines */


#define  FLASH28_CMD_READ_ID        (FLASH_28F_DEF) 0x90909090
#define  FLASH28_CMD_ERASE_SETUP    (FLASH_28F_DEF) 0x20202020
#define  FLASH28_CMD_PROG_SETUP     (FLASH_28F_DEF) 0x40404040
#define  FLASH28_CMD_RESET           (FLASH_28F_DEF) 0xffffffff
#define  FLASH28F008_CMD_READ_STATUS  (FLASH_28F_DEF) 0x70707070
#define  FLASH28F008_CMD_CLEAR_STATUS (FLASH_28F_DEF) 0x50505050
#define  FLASH28F008_CMD_ERASE        (FLASH_28F_DEF) 0xD0D0D0D0

#define  FLASH28F320_BSP_CMD_LOCK_PREFIX  (FLASH_28F_DEF) 0x60606060
#define  FLASH28F320_BSP_CMD_CLEAR_LOCKS  (FLASH_28F_DEF) 0xD0D0D0D0
#define  FLASH28F320_BSP_CMD_RESUME       (FLASH_28F_DEF) 0xD0D0D0D0

#define  FLASH28F008_BSP_STAT_WSMS    (FLASH_28F_DEF) 0x00800080

#define  FLASH28F008_STAT_BWS     (FLASH_28F_DEF) 0x10101010
#define  FLASH28F008_STAT_EWS     (FLASH_28F_DEF) 0x20202020

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRV_FLASH28F_INCLUDED */
