/*
 *         
 * $Id: furia_micro_seq.h 2014/04/02 palanivk Exp $
 * 
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
 *     
 *
 */
#ifndef __FURIA_MICRO_SEQ_H__
#define __FURIA_MICRO_SEQ_H__
/*
 * Includes
 */
#include <phymod/phymod.h>

/*
 *  Defines
 */
#define MICRO_CHECKSUM_ZERO_CODE (0x600D)

/* 
 *  Types
 */
typedef enum FURIA_MSGOUT
{
    MSGOUT_DONTCARE = 0x0000,
    MSGOUT_GET_CNT = 0x8888,
    MSGOUT_GET_LSB = 0xABCD,
    MSGOUT_GET_MSB = 0x4321,
    MSGOUT_GET_2B = 0xEEEE,
    MSGOUT_GET_B = 0xF00D,
    MSGOUT_ERR = 0x0BAD,
    MSGOUT_NEXT = 0x2222, 
    MSGOUT_NOT_DWNLD = 0x0101, 
    MSGOUT_DWNLD_ALREADY = 0x0202,
    MSGOUT_DWNLD_DONE = 0x0303,
    MSGOUT_PRGRM_DONE = 0x1414
} FURIA_MSGOUT_E;

/*
 *  Macros
 */

/*
 *  Global Variables
 */
  
/*
 *  Functions
 */

/**   Wait master message out 
 *    This function is to ensure whether master has sent the previous message
 *    out successfully 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param exp_message        Expected message specified by user  
 *    @param poll_time          Poll interval 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_wait_mst_msgout(const phymod_access_t *pa,
                            FURIA_MSGOUT_E exp_message,
                            int poll_time);


/**   Download and Fuse firmware 
 *    This function is used to download the firmware through I2C/MDIO
 *    and fuse it to SPI EEPROM if prg_eeprom flag is set 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param new_fw             Pointer to firmware array 
 *    @param fw_length          Length of the firmware array 
 *    @param prg_eeprom               Flag used to program EEPROM
 *
 *    @return num_bytes         number of bytes successfully downloaded
 */
int furia_download_prog_eeprom(const phymod_access_t *pa,
                                 uint8_t *new_fw,
                                 uint32_t fw_length,
                                 uint8_t prg_eeprom);

int furia_firmware_info_get(const phymod_access_t *pa,
                            phymod_core_firmware_info_t *fw_info);
#endif
