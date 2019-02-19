/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision      :  $Id: falcon_api_uc_common.h 375 2015-03-06 20:51:45Z kirand $  *
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

/* Please note that when adding entries here you should update the #defines in the falcon2_madura_common.h */

/** OSR_MODES Enum */
enum falcon2_madura_osr_mode_enum {
  FALCON2_MADURA_OSX1    = 0,
  FALCON2_MADURA_OSX2    = 1,
  FALCON2_MADURA_OSX4    = 2,
  FALCON2_MADURA_OSX16P5 = 8,
  FALCON2_MADURA_OSX20P625 = 12
};

/** CDR mode Enum **/
enum falcon2_madura_cdr_mode_enum {
  FALCON2_MADURA_CDR_MODE_OS_ALL_EDGES         = 0,
  FALCON2_MADURA_CDR_MODE_OS_PATTERN           = 1,
  FALCON2_MADURA_CDR_MODE_OS_PATTERN_ENHANCED  = 2,
  FALCON2_MADURA_CDR_MODE_BR_PATTERN           = 3
};

/** Lane User Control Clause93/72 Force Value **/
enum falcon2_madura_cl93n72_frc_val_enum {
  FALCON2_MADURA_CL93N72_FORCE_OS  = 0,
  FALCON2_MADURA_CL93N72_FORCE_BR  = 1
};


#endif
