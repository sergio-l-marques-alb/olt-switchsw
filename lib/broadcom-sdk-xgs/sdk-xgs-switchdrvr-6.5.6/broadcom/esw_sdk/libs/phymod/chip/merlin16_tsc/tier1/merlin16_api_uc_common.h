/***************************************************************************************
 ***************************************************************************************
 *                                                                                     *
 *  Revision      :   *
 *                                                                                     *
 *  Description   :  Defines and Enumerations required by Merlin16 APIs                *
 *                                                                                     *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                               *
 *  No portions of this material may be reproduced in any form without                 *
 *  the written permission of:                                                         *
 *      Broadcom Corporation                                                           *
 *      5300 California Avenue                                                         *
 *      Irvine, CA  92617                                                              *
 *                                                                                     *
 *  All information contained in this document is Broadcom Corporation                 *
 *  company private proprietary, and trade secret.                                     *
 *                                                                                     *
 ***************************************************************************************
 ***************************************************************************************/

/** @file merlin16_api_uc_common.h
 * Defines and Enumerations shared by Merlin16 IP Specific API and Microcode
 */

#ifndef MERLIN16_API_UC_COMMON_H
#define MERLIN16_API_UC_COMMON_H

/* Add Merlin16 specific items below this */ 

/* Please note that when adding entries here you should update the #defines in the merlin16_tsc_common.h */

/** OSR_MODES Enum */
enum merlin16_tsc_osr_mode_enum {
  MERLIN16_TSC_OSX1    = 0,
  MERLIN16_TSC_OSX2    = 1,
  MERLIN16_TSC_OSX3    = 2,
  MERLIN16_TSC_OSX3P3  = 3,
  MERLIN16_TSC_OSX4    = 4,
  MERLIN16_TSC_OSX5    = 5,
  MERLIN16_TSC_OSX7P5  = 6,
  MERLIN16_TSC_OSX8    = 7,
  MERLIN16_TSC_OSX8P25 = 8,
  MERLIN16_TSC_OSX10   = 9
};

/* The following functions translate between a VCO frequency in MHz and the
 * vco_rate that is found in the Core Config Variable Structure using the
 * formula:
 *
 *     vco_rate = (frequency_in_ghz * 4.0) - 22.0
 *
 * Both functions round to the nearest resulting value.  This
 * provides the highest accuracy possible, and ensures that:
 *
 *     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))
 *
 * In the microcode, these functions should only be called with a numeric
 * literal parameter.
 */
#define MHZ_TO_VCO_RATE(mhz) ((uint8_t)((((uint16_t)(mhz) + 125) / 250) - 22))
#define VCO_RATE_TO_MHZ(vco_rate) (((uint16_t)(vco_rate) + 22) * 250)


/* BOUNDARIES FOR FIR TAP VALUES
 *
 * Sources:  M16_TXFIR document (Section 1.1.2)
 *
 *   The following constraints are enforced by CL72 SW    
 *     pre + main + post1 + post2   <= TXFIR_SUM_LIMIT
 *     main - (pre + post1 + post2) >= TXFIR_V2_LIMIT
 *     pre + post                   <= TXFIR_PRE_POST_SUM_LIMIT
 *     main                         >= TXFIR_MAIN_CL72_MIN
 */
#define TXFIR_PRE_MIN   ( 0)
#define TXFIR_PRE_MAX   (10)
#define TXFIR_MAIN_MIN  ( 0)
#define TXFIR_MAIN_MAX  (60)
#define TXFIR_POST1_MIN ( 0)
#define TXFIR_POST1_MAX (24)
#define TXFIR_POST2_MIN ( 0)
#define TXFIR_POST2_MAX ( 6)

#define TXFIR_SUM_LIMIT (60)
#define TXFIR_V2_LIMIT  ( 6)
#define TXFIR_PRE_POST_SUM_LIMIT (24)
#define TXFIR_MAIN_CL72_MIN      (33)

/** INIT values for FIR taps */
#define TXFIR_PRE_INIT   ( 4)
#define TXFIR_MAIN_INIT  (40)
#define TXFIR_POST1_INIT (16)

/** Minimum and maximum values for rx_vga_ctrl_val */
/*  These are hardware limits for VGA. The tuning limits are defined separately in ucode */
#define RX_VGA_CTRL_VAL_MIN (0)
#define RX_VGA_CTRL_VAL_MAX (31)

/* BEGIN_GENERATED_TEMPERATURE_CODE */
/*
 * The formula for PVTMON is:
 *
 *     T = 434.10000 - 0.54619 * reg10bit
 */
#define _bin_to_degC_double(bin_) (434.10000 - (0.54619 * (USR_DOUBLE)(bin_)))


/* Identify the temperature from the PVTMON output. */
#define _bin_to_degC(bin_) (((((int32_t)(  910373683L) +           \
                               ((int32_t)(bin_) * (   -1145443L))) \
                              >> 20) + 1) >> 1)

/* Identify the PVTMON output corresponding to the temperature. */
#define _degC_to_bin(degc_) (((((int32_t)(  416692764L) +           \
                                ((int32_t)(degc_) * (    -959900L))) \
                               >> 18) + 1) >> 1)
/* END_GENERATED_TEMPERATURE_CODE */

#endif   
