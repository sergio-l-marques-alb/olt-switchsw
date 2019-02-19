/*
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
 *  $Id$
*/
/* $Id: madura_sdk_interface.h 3438 2015-06-30 06:39:49Z binliu $
 *
 *  Broadcom Confidential/Proprietary.
 *  Copyright (c) 2015 Broadcom Corporation. All rights reserved.
 */

#ifndef MADURA_SDK_INTERFACE_H_
#define MADURA_SDK_INTERFACE_H_

/** @file madura_sdk_interface.h
 *  Declaration of data structure and constants shared by Madura firmware and Madura SDK.
 */

/** @enum E_MADURA_SYS_LINE_SEL
 *  Constants to select Madura system side or line side in some functions.
 */
/** @enum E_MADURA_MEDIA_TYPE
 *  Constants to select Serdes media type.
 */

/** @enum E_MADURA_DFE_OPTION
 *  Constants to select DFE type.
 */

/** @enum E_MADURA_TX_TIMING_MODE
 *  Constants to select Serdes Tx timing source.
 */

/** @typedef union madura_user_port_config_un madura_port_config_t
 *  Data structure to specify Madura user configuration at port level.
 */

enum E_MADURA_SYS_LINE_SEL
{
    MADURA_SYS  = 0,  /**< select Madura system side */
    MADURA_LINE = 1   /**< select Madura line side */
};

enum E_MADURA_MEDIA_TYPE
{
    SERDES_MEDIA_TYPE_BACK_PLANE             = 0,  /**< KR link: back plane media  */
    SERDES_MEDIA_TYPE_COPPER_CABLE           = 1,  /**< CR link: direct-attached copper cable  */
    SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS   = 2,  /**< SR/LR link: optical fiber with reliable LOS detection  */
    SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS = 3   /**< SR/LR link: optical fiber with unreliable LOS detection  */
};

enum E_MADURA_DFE_OPTION
{
    SERDES_DFE_OPTION_NO_DFE      = 0,  /**< no DFE is used  */
    SERDES_DFE_OPTION_DFE         = 1,  /**< DFE is enabled  */
    SERDES_DFE_OPTION_BRDEF       = 2,  /**< Force BRDFE mode, DFE will be on, too.  */
    SERDES_DFE_OPTION_DFE_LP_MODE = 3   /**< DFE is enabled in low-power mode  */
};

enum E_MADURA_TX_TIMING_MODE
{
    MADURA_TX_REPEATER      = 0, /**< Tx repeater mode, chip default  */
    MADURA_TX_REPEATER1     = 1, /**< Alternative value for Tx repeater mode  */
    MADURA_TX_RETIMER       = 2, /**< Tx retimer mode  */
    MADURA_TX_REPEATER_ULL  = 3  /**< Tx repeater mode with ULL data path  */
};

#ifdef  MADURA_UNTESTED_CODE
typedef union madura_port_user_config_un
{
    union {
        uint16_t word;
        struct {
            uint8_t sys_media_type       : 2; /**< [1:0] use the symbols in E_MADURA_MEDIA_TYPE  */
            uint8_t sys_DFE_option       : 2; /**< [3:2] use the symbols in E_MADURA_DFE_OPTION_TYPE  */
            uint8_t sys_Tx_training_en   : 1; /**< [4]   0/1: disable/enable forced Tx training at system side  */
            uint8_t sys_Tx_timing_mode   : 2; /**< [6:5] use the symbols in E_MADURA_TX_TIMING_MODE for LRx->STx direction  */
            uint8_t loopback_change      : 1; /**< [7]   0/1: loopback config at either side no change/changed.  */

            uint8_t line_media_type      : 2; /**< [9:8] use the symbols in E_MADURA_MEDIA_TYPE  */
            uint8_t line_DFE_option      : 2; /**< [11:10] use the symbols in E_MADURA_DFE_OPTION_TYPE  */
            uint8_t line_Tx_training_en  : 1; /**< [12]  0/1: disable/enable forced Tx training at line side  */
            uint8_t line_Tx_timing_mode  : 2; /**< [14:13] use the symbols in E_MADURA_TX_TIMING_MODE for SRx->LTx direction  */
            uint8_t AN_enable            : 1; /**< [15]  0/1: disable/enable auto-negotiation at line side  */
        } fields;
    } both_sides;

    union {
        uint8_t byte;
        struct {
            uint8_t media_type           : 2; /**< [1:0] use the symbols in E_MADURA_MEDIA_TYPE  */
            uint8_t DFE_option           : 2; /**< [3:2] use the symbols in E_MADURA_DFE_OPTION_TYPE  */
            uint8_t Tx_training_en       : 1; /**< [4]   0/1: disable/enable forced Tx training at system side  */
            uint8_t Tx_timing_mode       : 2; /**< [6:5] use the symbols in E_MADURA_TX_TIMING_MODE  */
            uint8_t chip_feature_en      : 1; /**< [7]   0/1: disable/enable chip level features such as loopback and auto-negotiation  */
        } fields;
    } at_side[2];
} madura_port_user_config_t;
#endif /* MADURA_UNTESTED_CODE */

#endif /* MADURA_SDK_INTERFACE_H_ */

