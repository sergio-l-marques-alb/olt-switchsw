/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Functions used internally and available in debug shell only         *
 *                                                                                       *
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

/** @file merlin16_tsc_types.h
 * Common types used by Serdes API functions
 */

#ifndef MERLIN16_TSC_API_TYPES_H
#define MERLIN16_TSC_API_TYPES_H

/* 
 * Includes
 */
#include <phymod/phymod_types.h>
#include "merlin16_tsc_ipconfig.h"
#include "merlin16_tsc_usr_includes.h"

/*----------------------------------------*/
/*  Lane/Core structs (without bitfields) */
/*----------------------------------------*/

/** Lane Config Variable Structure in Microcode */
struct merlin16_tsc_uc_lane_config_field_st {
  uint8_t  lane_cfg_from_pcs ;
  uint8_t  an_enabled        ;
  uint8_t  dfe_on            ;
  uint8_t  force_brdfe_on    ;
  uint8_t  media_type        ;
  uint8_t  unreliable_los    ;
  uint8_t  scrambling_dis    ;
  uint8_t  cl72_auto_polarity_en ;
  uint8_t  cl72_restart_timeout_en;
  uint8_t  reserved          ;
};

/** Core Config Variable Structure in Microcode */
struct merlin16_tsc_uc_core_config_field_st {
  uint8_t  core_cfg_from_pcs ;
  uint8_t  vco_rate          ;
  uint8_t  reserved1         ;
  uint8_t  reserved2         ;
};

/** Lane Config Struct */
struct  merlin16_tsc_uc_lane_config_st {
  struct merlin16_tsc_uc_lane_config_field_st field;
  uint16_t word;
};

/** Core Config Struct */
struct  merlin16_tsc_uc_core_config_st {
  struct merlin16_tsc_uc_core_config_field_st field;
  uint16_t word;
  int vco_rate_in_Mhz; /* if >0 then will get converted and replace field.vco_rate when update is called */
};

/** Lane User Control Function Structure in Microcode */
struct merlin16_tsc_usr_ctrl_func_st {
  /** Peaking Filter Adaptation */
  uint8_t pf_adaptation            ;
  /** DC Offset Adaptation */
  uint8_t dc_adaptation            ;
  /** Variable Gain Amplifier adaptation */
  uint8_t vga_adaptation           ;
  /** Slicer Offset tuning */
  uint8_t slicer_offset_tuning     ;
  /** Clk90 Offset adaptation */
  uint8_t clk90_offset_adaptation  ;
  /** P1 Level tuning */
  uint8_t p1_level_tuning          ;
  /** Eye adaptation */
  uint8_t eye_adaptation           ;
  /** All adaptation */
  uint8_t all_adaptation           ;
};

/** Lane User DFE Control Function Structure in Microcode */
struct merlin16_tsc_usr_ctrl_dfe_func_st {
  /** DFE tap 1 adaptation */
  uint8_t dfe_tap1_adaptation      ;
  /** DFE tap 2 adaptation */
  uint8_t dfe_tap2_adaptation      ;
  /** DFE tap 3 adaptation */
  uint8_t dfe_tap3_adaptation      ;
  /** DFE tap 4 adaptation */
  uint8_t dfe_tap4_adaptation      ;
  /** DFE tap 5 adaptation */
  uint8_t dfe_tap5_adaptation      ;
  /** DFE tap 1 Duty Cycle Distortion */
  uint8_t dfe_tap1_dcd             ;
  /** DFE tap 2 Duty Cycle Distortion */
  uint8_t dfe_tap2_dcd             ;
};

/** Lane User Control Disable Function Struct */
struct merlin16_tsc_usr_ctrl_disable_functions_st {
  struct merlin16_tsc_usr_ctrl_func_st field;
  uint8_t  byte;
};

/** Lane User Control Disable DFE Function Struct */
struct merlin16_tsc_usr_ctrl_disable_dfe_functions_st {
  struct merlin16_tsc_usr_ctrl_dfe_func_st field;
  uint8_t  byte;
};


/************************************************************************//**
 * @name Diagnostic Sampling
 *
 *//*************************************************************************/
/**@{*/

#ifdef STANDALONE_EVENT
#define DIAG_MAX_SAMPLES (64)
#else
/**
 * Diagnostic sample set size, non-FALCON16 variant.
 * Applies to collections of BER measurements, eye margins, etc.
 */
#define DIAG_MAX_SAMPLES (32)
#endif

/**@}*/

#endif
