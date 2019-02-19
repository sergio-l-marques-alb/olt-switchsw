/*******************************************************************************************
 *******************************************************************************************
 *  File Name     :  merlin16_tx_analog_functions.h                                        *
 *  Created On    :  04/20/2015                                                            *
 *  Created By    :  Kiran Divakar                                                         *
 *  Description   :  Functions to Configure Merlin16 TXFIR                                 *
 *  Revision      :   *
 *                                                                                         *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                                   *
 *  No portions of this material may be reproduced in any form without                     *
 *  the written permission of:                                                             *
 *      Broadcom Corporation                                                               *
 *      5300 California Avenue                                                             *
 *      Irvine, CA  92617                                                                  *
 *                                                                                         *
 *  All information contained in this document is Broadcom Corporation                     *
 *  company private proprietary, and trade secret.                                         *
 *                                                                                         *
 *******************************************************************************************
 *******************************************************************************************/

/** @file merlin16_tx_analog_functions.h
 * Functions to configure Merlin16 TX
 */

#ifndef MERLI16_TX_ANALOG_FUNCTIONS_H
#define MERLI16_TX_ANALOG_FUNCTIONS_H

#include "merlin16_tsc_dependencies.h"

/* Structs required for M16 TXFIR AFE slicer controls */

/** Post2 Slicer Register Field Struct */
struct p2sr_regfield_st {
  uint16_t post2_1x_0         ;
  uint16_t dc_level_post2_1x_0;
  uint16_t post2_1x_1         ;
  uint16_t dc_level_post2_1x_1;
  uint16_t post2_2x_0         ;
  uint16_t dc_level_post2_2x_0;
  uint16_t post2_2x_1         ;
  uint16_t dc_level_post2_2x_1;
  uint16_t reserved0          ;
};

/** Post1 Slicer Register Field Struct */
struct p1sr_regfield_st {
  uint16_t post1_1x_0            ;
  uint16_t dc_level_post1_1x_0   ;
  uint16_t post1_1x_1            ;
  uint16_t dc_level_post1_1x_1   ;
  uint16_t post1_2x_0            ;
  uint16_t dc_level_post1_2x_0   ;
  uint16_t post1_2x_1            ;
  uint16_t dc_level_post1_2x_1   ;
  uint16_t post1_2x_2            ;
  uint16_t dc_level_post1_2x_2   ;
  uint16_t reserved0             ;    
  uint16_t pre_post1pre_1x_0     ;
  uint16_t post1_post1pre_1x_0   ;
  uint16_t dc_level_post1pre_1x_0;
  uint16_t reserved1             ;
};

/** Post1Pre Slicer Register Field Struct */
struct presr_regfield_st {
  uint16_t pre_post1pre_1x_1     ;
  uint16_t post1_post1pre_1x_1   ;
  uint16_t dc_level_post1pre_1x_1;
  uint16_t pre_post1pre_2x_0     ;
  uint16_t post1_post1pre_2x_0   ;
  uint16_t dc_level_post1pre_2x_0;
  uint16_t pre_post1pre_2x_1     ;
  uint16_t post1_post1pre_2x_1   ;
  uint16_t dc_level_post1pre_2x_1;
  uint16_t pre_post1pre_2x_2     ;
  uint16_t post1_post1pre_2x_2   ;
  uint16_t dc_level_post1pre_2x_2;
  uint16_t pre_post1pre_2x_3     ;
  uint16_t post1_post1pre_2x_3   ;
  uint16_t dc_level_post1pre_2x_3;
  uint16_t reserved0             ;
};

/** Main Slicer Register Field Struct */
struct mssr_regfield_st {
  uint16_t dc_level_0p5x     ;
  uint16_t dc_level_main_1x  ;
  uint16_t dc_level_main_2x_0;
  uint16_t dc_level_main_2x_1;
  uint16_t reserved0         ;
};

/** Post2 Slicer Register Struct */
struct  p2sr_reg_st {
  struct p2sr_regfield_st field;
  uint16_t word;
};

/** Post1 Slicer Register Struct */
struct  p1sr_reg_st {
  struct p1sr_regfield_st field;
  uint16_t word;
};

/** Post1Pre Slicer Register Struct */
struct  presr_reg_st {
  struct presr_regfield_st field;
  uint16_t word;
};

/** Main Slicer Register Struct */
struct  mssr_reg_st {
  struct mssr_regfield_st field;
  uint16_t word;
};

/** Converts the input TXFIR tap values to the required analog tap slicer settings
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param pre      TXFIR pre   tap value (0..10)  in 1/60 Vpp
 * @param main     TXFIR main  tap value (1..60)  in 1/60 Vpp
 * @param post1    TXFIR post1 tap value (0..24)  in 1/60 Vpp
 * @param post2    TXFIR post2 tap value (0..6)   in 1/60 Vpp
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _txfir_tap_mapping(srds_access_t *sa__, int8_t pre, int8_t main, int8_t post1, int8_t post2);

/* Update Merlin16 TXFIR Slicer Structs */
static err_code_t _update_p1sr_reg_word(struct p1sr_reg_st *st);
static err_code_t _update_p2sr_reg_word(struct p2sr_reg_st *st);
static err_code_t _update_mssr_reg_word(struct mssr_reg_st *st);
static err_code_t _update_presr_reg_word(struct presr_reg_st *st);

/** Write to Analog Slicer Control Registers */ 
static err_code_t _write_txfir_slicer_controls(srds_access_t *sa__,
                                               struct p1sr_reg_st p1_st, struct p2sr_reg_st p2_st,
                                               struct mssr_reg_st ms_st, struct presr_reg_st pre_st);
#endif
