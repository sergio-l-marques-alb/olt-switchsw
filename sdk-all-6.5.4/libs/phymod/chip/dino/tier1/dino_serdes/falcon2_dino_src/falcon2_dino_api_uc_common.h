/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision      :  $Id: falcon_api_uc_common.h 1139 2015-09-17 23:39:05Z kirand $  *
 *                                                                                *
 *  Description   :  Defines and Enumerations required by Falcon ucode            *
 *                                                                                *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                          *
 *  No portions of this material may be reproduced in any form without            *
 *  the written permission of:                                                    *
 *      Broadcom Corporation                                                      *
 *      5300 California Avenue                                                    *
 *      Irvine, CA  92617                                                         *
 *                                                                                *
 *  All information contained in this document is Broadcom Corporation            *
 *  company private proprietary, and trade secret.                                *
 *                                                                                *
 **********************************************************************************
 **********************************************************************************/

/** @file falcon_api_uc_common.h
 * Defines and Enumerations shared by Falcon IP Specific API and Microcode
 */

#ifndef FALCON_API_UC_COMMON_H
#define FALCON_API_UC_COMMON_H

/* Add Falcon specific items below this */

/** Translate between a VCO frequency in MHz and the vco_rate that is **\
*** found in the Core Config Variable Structure using the formula:    ***
***                                                                   ***
***     vco_rate = (frequency_in_ghz * 16.0) - 224.0                  ***
***                                                                   ***
*** Both functions round to the nearest resulting value.  This        ***
*** provides the highest accuracy possible, and ensures that:         ***
***                                                                   ***
***     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))        ***
***                                                                   ***
*** In the microcode, this should only be called with a numeric       ***
*** literal parameter.                                                ***
\**                                                                   **/
#define FALCON2_DINO_MHZ_TO_VCO_RATE(mhz) ((uint8_t)(((((uint16_t)(mhz) * 2) + 62) / 125) - 224))
#define FALCON2_DINO_VCO_RATE_TO_MHZ(vco_rate) (((((uint16_t)(vco_rate) + 224) * 125) + 1) >> 1)

/* Please note that when adding entries here you should update the #defines in the uc shared/ */

/** OSR_MODES Enum */
enum falcon2_dino_osr_mode_enum {
  FALCON2_DINO_OSX1    = 0,
  FALCON2_DINO_OSX2    = 1,
  FALCON2_DINO_OSX4    = 2,
  FALCON2_DINO_OSX8    = 5,
  FALCON2_DINO_OSX16P5   = 8,
  FALCON2_DINO_OSX16     = 9,
  FALCON2_DINO_OSX20P625 = 12,
  FALCON2_DINO_OSX32     = 13
};

/** CDR mode Enum **/
enum falcon2_dino_cdr_mode_enum {
  FALCON2_DINO_CDR_MODE_OS_ALL_EDGES         = 0,
  FALCON2_DINO_CDR_MODE_OS_PATTERN           = 1,
  FALCON2_DINO_CDR_MODE_OS_PATTERN_ENHANCED  = 2,
  FALCON2_DINO_CDR_MODE_BR_PATTERN           = 3
};

/** Lane User Control Clause93/72 Force Value **/
enum falcon2_dino_cl93n72_frc_val_enum {
  FALCON2_DINO_CL93N72_FORCE_OS  = 0,
  FALCON2_DINO_CL93N72_FORCE_BR  = 1
};


#endif
