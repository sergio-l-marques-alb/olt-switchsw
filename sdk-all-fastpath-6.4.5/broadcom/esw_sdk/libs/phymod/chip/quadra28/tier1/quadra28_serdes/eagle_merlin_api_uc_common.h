/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :  $Id: eagle_merlin_api_uc_common.h 924 2015-02-24 18:08:11Z eroes $ *
 *                                                                                       *
 *  Description   :  Defines and Enumerations required by Eagle/Merlin APIs              *
 *                                                                                       *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                                 *
 *  No portions of this material may be reproduced in any form without                   *
 *  the written permission of:                                                           *
 *      Broadcom Corporation                                                             *
 *      5300 California Avenue                                                           *
 *      Irvine, CA  92617                                                                *
 *                                                                                       *
 *  All information contained in this document is Broadcom Corporation                   *
 *  company private proprietary, and trade secret.                                       *
 *                                                                                       *
 *****************************************************************************************
 *****************************************************************************************/

/** @file eagle_merlin_api_uc_common.h
 * Defines and Enumerations shared by Eagle & Merlin IP Specific API and Microcode
 */

#ifndef EAGLE_MERLIN_API_UC_COMMON_H
#define EAGLE_MERLIN_API_UC_COMMON_H

/* Add Eagle/Merlin specific items below this */ 

/* Please note that when adding entries here you should update the #defines in the merlin_quadra28_common.h */

/** OSR_MODES Enum */
enum merlin_quadra28_osr_mode_enum {
  MERLIN_QUADRA28_OSX1    = 0,
  MERLIN_QUADRA28_OSX2    = 1,
  MERLIN_QUADRA28_OSX3    = 2,
  MERLIN_QUADRA28_OSX3P3  = 3,
  MERLIN_QUADRA28_OSX4    = 4,
  MERLIN_QUADRA28_OSX5    = 5,
  MERLIN_QUADRA28_OSX7P5  = 6,
  MERLIN_QUADRA28_OSX8    = 7,
  MERLIN_QUADRA28_OSX8P25 = 8,
  MERLIN_QUADRA28_OSX10   = 9
};


/** VCO_RATE Enum */
enum merlin_quadra28_vco_rate_enum {
  MERLIN_QUADRA28_VCO_5P5G = 0,
  MERLIN_QUADRA28_VCO_5P75G,
  MERLIN_QUADRA28_VCO_6G,
  MERLIN_QUADRA28_VCO_6P25G,
  MERLIN_QUADRA28_VCO_6P5G,
  MERLIN_QUADRA28_VCO_6P75G,
  MERLIN_QUADRA28_VCO_7G,
  MERLIN_QUADRA28_VCO_7P25G,
  MERLIN_QUADRA28_VCO_7P5G,
  MERLIN_QUADRA28_VCO_7P75G,
  MERLIN_QUADRA28_VCO_8G,
  MERLIN_QUADRA28_VCO_8P25G,
  MERLIN_QUADRA28_VCO_8P5G,
  MERLIN_QUADRA28_VCO_8P75G,
  MERLIN_QUADRA28_VCO_9G,
  MERLIN_QUADRA28_VCO_9P25G,
  MERLIN_QUADRA28_VCO_9P5G,
  MERLIN_QUADRA28_VCO_9P75G,
  MERLIN_QUADRA28_VCO_10G,
  MERLIN_QUADRA28_VCO_10P25G,
  MERLIN_QUADRA28_VCO_10P5G,
  MERLIN_QUADRA28_VCO_10P75G,
  MERLIN_QUADRA28_VCO_11G,
  MERLIN_QUADRA28_VCO_11P25G,
  MERLIN_QUADRA28_VCO_11P5G,
  MERLIN_QUADRA28_VCO_11P75G,
  MERLIN_QUADRA28_VCO_12G,
  MERLIN_QUADRA28_VCO_12P25G,
  MERLIN_QUADRA28_VCO_12P5G,
  MERLIN_QUADRA28_VCO_12P75G,
  MERLIN_QUADRA28_VCO_13G,
  MERLIN_QUADRA28_VCO_13P25G
};

#endif   
