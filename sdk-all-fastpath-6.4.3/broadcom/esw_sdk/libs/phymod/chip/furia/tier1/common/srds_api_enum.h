/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: srds_api_enum.h 661 2014-08-14 23:20:49Z kirand $ *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 *                                                                            *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                      *
 *  No portions of this material may be reproduced in any form without        *
 *  the written permission of:                                                *
 *      Broadcom Corporation                                                  *
 *      5300 California Avenue                                                *
 *      Irvine, CA  92617                                                     *
 *                                                                            *
 *  All information contained in this document is Broadcom Corporation        *
 *  company private proprietary, and trade secret.                            *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

/** @file srds_api_enum.h
 * Enum types used by Serdes API functions
   This file contains Enum's which are common to all serdes cores
 */

#ifndef SRDS_API_ENUM_H
#define SRDS_API_ENUM_H

/** PRBS Polynomial Enum */ 
enum srds_prbs_polynomial_enum {
  PRBS_7  = 0,
  PRBS_9  = 1,
  PRBS_11 = 2,
  PRBS_15 = 3,
  PRBS_23 = 4,
  PRBS_31 = 5,
  PRBS_58 = 6
};


/** PRBS Checker Mode Enum */
enum srds_prbs_checker_mode_enum {
  PRBS_SELF_SYNC_HYSTERESIS       = 0,
  PRBS_INITIAL_SEED_HYSTERESIS    = 1,
  PRBS_INITIAL_SEED_NO_HYSTERESIS = 2
};


/** TX_PI Fixed Frequency and Jitter Generation Modes Enum */
enum srds_tx_pi_freq_jit_gen_enum {
  TX_PI_FIXED_FREQ      = 0,
  TX_PI_SSC_LOW_FREQ    = 1,
  TX_PI_SSC_HIGH_FREQ   = 2,
  TX_PI_SJ              = 3
};


enum srds_rx_afe_settings_enum {
	RX_AFE_PF,
	RX_AFE_PF2,
	RX_AFE_VGA,
	RX_AFE_DFE1,
	RX_AFE_DFE2,
	RX_AFE_DFE3,
	RX_AFE_DFE4,
	RX_AFE_DFE5,
	RX_AFE_DFE6,
	RX_AFE_DFE7,
	RX_AFE_DFE8,
	RX_AFE_DFE9,
	RX_AFE_DFE10,
	RX_AFE_DFE11,
	RX_AFE_DFE12,
	RX_AFE_DFE13,
	RX_AFE_DFE14
};

enum srds_tx_afe_settings_enum {
	TX_AFE_PRE,
	TX_AFE_MAIN,
	TX_AFE_POST1,
	TX_AFE_POST2,
	TX_AFE_POST3,
	TX_AFE_AMP,
    TX_AFE_DRIVERMODE
};

enum srds_afe_drivermode_enum {
    DM_DEFAULT = 0,
    DM_NOT_SUPPORTED,
    DM_HALF_AMPLITUDE,
    DM_HALF_AMPLITUDE_HI_IMPED
};

enum srds_rptr_mode_enum {
    DATA_IN_SIDE,
    DIG_LPBK_SIDE
};

enum srds_core_pwrdn_mode_enum {
	PWR_ON,
	PWRDN,
	PWRDN_DEEP,
	PWRDN_TX,
	PWRDN_RX
};

enum srds_event_log_display_mode_enum {
	EVENT_LOG_HEX,
	EVENT_LOG_DECODED,
	EVENT_LOG_HEX_AND_DECODED,
	EVENT_LOG_MODE_MAX
};

enum srds_eye_scan_dir_enum {
      EYE_SCAN_HORIZ = 0,
      EYE_SCAN_VERTICAL = 1
};

#endif

