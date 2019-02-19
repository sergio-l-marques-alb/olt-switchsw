/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :  $Id: falcon_tsc_internal.h 754 2014-10-29 22:29:05Z kirand $ *
 *                                                                                *
 *  Description :  Internal API functions                                         *
 *                                                                                *
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

/** @file falcon_tsc_internal.h 
 * Internal functions
 */

#ifndef FALCON_TSC_API_INTERNAL_H
#define FALCON_TSC_API_INTERNAL_H

#define SDK_STATIC static


#include "falcon_tsc_functions.h"
#include "falcon_tsc_common.h"
#include "falcon_tsc_field_access.h"
#include "falcon_tsc_ipconfig.h"
#include "falcon_tsc_common_err_code.h"
#include "falcon_tsc_interface.h"
#include "falcon_tsc_debug_functions.h"

/*------------------------------*/
/** Serdes OSR Mode Structure   */
/*------------------------------*/
typedef struct {
    uint8_t tx;
    uint8_t rx;
    uint8_t tx_rx;
}falcon_tsc_osr_mode_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {       
       uint16_t ucv_config;
       int16_t rx_ppm;
       int16_t p1_lvl;       
       int16_t m1_lvl;
       uint16_t link_time;
       falcon_tsc_osr_mode_st osr_mode;
       uint8_t sig_det;
       uint8_t rx_lock;
       int8_t clk90;
       int8_t clkp1;
       int8_t pf_main, pf_hiz, pf_bst, pf_low,pf2_ctrl;
       int8_t vga;
       int8_t dc_offset;
       int8_t p1_lvl_ctrl;
       int8_t dfe1;
       int8_t dfe2, dfe3, dfe4, dfe5,dfe6;
       int8_t dfe1_dcd, dfe2_dcd;
       int8_t pe, ze, me, po, zo, mo;
       int16_t tx_ppm;
       int8_t txfir_pre, txfir_main, txfir_post1;
       int8_t txfir_post2,txfir_post3;
       int8_t tx_amp_ctrl;
       uint16_t heye_left, heye_right,veye_upper,veye_lower;
       uint8_t br_pd_en;
} falcon_tsc_lane_state_st;

/*------------------------------*/
/** Serdes Core State Structure */
/*------------------------------*/
typedef struct { 
       uint8_t  core_reset;
       uint8_t  pll_pwrdn;
       uint8_t  uc_active;
       uint16_t comclk_mhz;
       uint16_t ucode_version;
       uint8_t  ucode_minor_version;
       uint8_t  afe_hardware_version;
       uint8_t  temp_idx; 
       uint16_t avg_tmon;
       uint8_t  rescal;
       uint16_t vco_rate_mhz;
       uint8_t  analog_vco_range;
       uint8_t  pll_div;
       uint8_t  pll_lock;
} falcon_tsc_core_state_st;


/** Check if the micro's operations on that lane are stopped. 
 * @return err_code Error Code "ERR_CODE_UC_NOT_STOPPED" returned if micro NOT stopped
 */
SDK_STATIC err_code_t _check_uc_lane_stopped( const phymod_access_t *pa );

/** Calculate the mode_sel parameter for tx pattern generator.
 * @param *mode_sel Mode select to be used for generating required pattern
 * @param *zero_pad_len Length of zero padding to be used for generating required pattern 
 * @param patt_length Desired Pattern length
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */ 
SDK_STATIC err_code_t _calc_patt_gen_mode_sel( const phymod_access_t *pa, uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length);




/*-------------------*/
/*  Ladder controls  */
/*-------------------*/
/** Converts a ladder setting to mV, given the range.
 * @param ctrl is the threshold control (-31..31) maps to -RANGE to RANGE in non-uniform steps
 * @param range_250 determines the range 0 = +/-150mV, 1 = +/-250mV
 * @return ladder threshold voltage in mV
 */
SDK_STATIC int16_t _ladder_setting_to_mV(int8_t ctrl, uint8_t range_250);



/*-----------------------*/
/*  TX_PI and ULL Setup  */
/*-----------------------*/

/** Safe multiply - multiplies 2 numbers and checks for overflow.
 * @param a First input
 * @param b Second input
 * @param *of Pointer to overflow indicator
 * @return value of a * b 
 */ 
static uint32_t _mult_with_overflow_check(uint32_t a, uint32_t b, uint8_t *of);


/*-----------------------------------------*/
/*  APIs used in Config Shared TX Pattern  */
/*-----------------------------------------*/
/** Compute Binary string for a Hex value ['0' to 'F'].
 * @param var Hex value to be converted to Binary (eg: '6', 'A', ...)  
 * @param bin Binary string returned by API (eg: '0110', '1010', ...)
 * @return Error Code generated by invalid hex variable (returns ERR_CODE_NONE if no errors) 
 */ 
static err_code_t _compute_bin( const phymod_access_t *pa, char var, char bin[]);

/** Compute Hex value for a Binary string ['0000' to '1111'].
 * @param bin Binary string to be coverted (eg: '0110', '1010', ...)
 * @param *hex Hex value calculated from the input Binary string
 * @return Error Code generated by invalid Binary string (returns ERR_CODE_NONE if no errors) 
 */ 
static err_code_t _compute_hex( const phymod_access_t *pa, char bin[], uint8_t *hex);


/*-----------------------------------------*/
/*  APIs used in Read Event Logger         */
/*-----------------------------------------*/
/** Interpret CL72 coefficient status field.
 * @param val Coefficient status field value to be interpreted
 * @return Char string to be displayed in event log 
 */ 
static char* _status_val_2_str( const phymod_access_t *pa, uint8_t val);

/** Interpret CL72 coefficient update field.
 * @param val Coefficient update field value to be interpreted
 * @return Char string to be displayed in event log
 */ 
static char* _update_val_2_str( const phymod_access_t *pa, uint8_t val);

/** Interpret error event code.
 * @param val Event code error value to be interpreted
 * @return Char string to be displayed in event log
 */ 
static char* _error_val_2_str( const phymod_access_t *pa, uint8_t val);

/** Display event information.
 * @param event_id Event id to be displayed
 * @param entry_len Length of the event entry
 * @param prev_cursor CL72 prev cursor value 
 * @param curr_cursor CL72 curr cursor value
 * @param post_cursor CL72 post cursor value
 * @param *supp_info Supplement information
 */ 
static err_code_t _falcon_display_event( const phymod_access_t *pa, uint8_t event_id, uint8_t entry_len, uint8_t prev_cursor, uint8_t curr_cursor, uint8_t post_cursor, uint8_t *supp_info);


/** Convert float8 to usigned int32.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input Float8 number
 * @return Usigned 32bit number  
 */
static uint32_t _float8_to_int32(float8_t input);


#ifdef TO_FLOATS
/*-----------------------------------------*/
/*  APIs used in uC data conversion        */
/*-----------------------------------------*/

/** Convert usigned int32 to float8.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input Unsigned int
 * @return Float8 8 bit representations of 32bit number  
 */
static float8_t _int32_to_float8(uint32_t input);
#endif


/*-----------------------------*/
/*  Read / Display Core state  */
/*-----------------------------*/
/** Read current falcon_tsc core status.
 * @param *istate Current falcon_tsc core status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _falcon_tsc_read_core_state( const phymod_access_t *pa, falcon_tsc_core_state_st *istate);

/** Display current core state.
 * Reads and displays all important core state values.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_tsc_display_core_state_no_newline( const phymod_access_t *pa ); 

/*-----------------------------*/
/*  Read / Display Lane state  */
/*-----------------------------*/
/** Read current falcon_tsc lane status.
 * @param *istate Current falcon_tsc lane status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _falcon_tsc_read_lane_state( const phymod_access_t *pa, falcon_tsc_lane_state_st *istate);

/*-----------------------------*/
/*  Get OSR mode              */
/*-----------------------------*/
/** Read current falcon_tsc lane status.
 * @param *mode Returns with the osr mode structure
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _falcon_tsc_get_osr_mode( const phymod_access_t *pa, falcon_tsc_osr_mode_st *mode);

/** Display current lane state.
 * Reads and displays all important lane state values.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_tsc_display_lane_state_no_newline( const phymod_access_t *pa );



/** Convert eye margin to mV.
 * Takes in the ladder setting with 3 fractional bits and converts to mV.
 * @param var Ladder setting with 3 fractional bits
 * @param ladder_range Specified if ladder is configured for 150mV or 250mV range
 * @return Eye opening in mV
 */
SDK_STATIC uint16_t _eye_to_mV(uint8_t var, uint8_t ladder_range); 

/** Convert eye margin to mUI.
 * Takes in a horizontal margin in Phase Interpolator codes and converts it to mUI.
 * @param var Horizontal margin in Phase Interpolator codes with 3 fractional bits
 * @return Eye opening in mV
 */
SDK_STATIC uint16_t _eye_to_mUI(uint8_t var); 


/** Serdes Core ClockGate.
 * Along with falcon_tsc_core_clkgate(), all lanes should also be clock gated using falcon_tsc_lane_clkgate() to complete a Core Clockgate 
 * @param enable Enable clockgate (1 = Enable clokgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_tsc_core_clkgate( const phymod_access_t *pa, uint8_t enable);

/** Serdes Lane ClockGate.
 * @param enable Enable lane clockgate (1 = Enable clockgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_tsc_lane_clkgate( const phymod_access_t *pa, uint8_t enable);


/** Set function for PF.
 * @param val Signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_pf_main( const phymod_access_t *pa, uint8_t val);

/** Get function for PF
 * @param *val PF read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_pf_main( const phymod_access_t *pa, int8_t *val);

/** Set function for PF2.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_pf2( const phymod_access_t *pa, uint8_t val);

/** Get function for PF2.
 * @param *val PF2 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_pf2( const phymod_access_t *pa, int8_t *val);

/** Set function for VGA.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_vga( const phymod_access_t *pa, uint8_t val);

/** Get function for VGA.
 * @param *val VGA read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_vga( const phymod_access_t *pa, int8_t *val);


/** Set function for DFE Tap1.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _set_rx_dfe1( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap1.
 * @param *val DFE1 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _get_rx_dfe1( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap2.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _set_rx_dfe2( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap2.
 * @param *val DFE2 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _get_rx_dfe2( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap3.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_dfe3( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap3.
 * @param *val DFE3 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_dfe3( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap4.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_dfe4( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap4.
 * @param *val DFE4 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_dfe4( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap5.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_dfe5( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap5
 * @param *val DFE5 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_dfe5( const phymod_access_t *pa, int8_t *val);

/* Set/Get DFE tap values */
static err_code_t _set_rx_dfe6( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe6( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe7( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe7( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe8( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe8( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe9( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe9( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe10( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe10( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe11( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe11( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe12( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe12( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe13( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe13( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe14( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe14( const phymod_access_t *pa, int8_t *val);

/* Set/Get TX Tap values */
static err_code_t _set_tx_pre( const phymod_access_t *pa, uint8_t val);
static err_code_t _set_tx_amp( const phymod_access_t *pa, int8_t val);
static err_code_t _get_tx_amp( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_tx_main( const phymod_access_t *pa, uint8_t val);
static err_code_t _set_tx_post1( const phymod_access_t *pa, uint8_t val);
static err_code_t _set_tx_post2( const phymod_access_t *pa, int8_t val);
static err_code_t _set_tx_post3( const phymod_access_t *pa, int8_t val);
static err_code_t _get_tx_post3( const phymod_access_t *pa, int8_t *val);

/* Lane Config Struct */
static void _update_uc_lane_config_st( const phymod_access_t *pa, struct falcon_tsc_uc_lane_config_st *st);
static void _update_uc_lane_config_word( const phymod_access_t *pa, struct falcon_tsc_uc_lane_config_st *st);

/* Lane User Control Disable Startup Function Struct */
static void _update_usr_ctrl_disable_functions_st( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_functions_st *st);
static void _update_usr_ctrl_disable_functions_byte( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_functions_st *st);

/* Lane User Control Disable Startup DFE Function Struct */
static void _update_usr_ctrl_disable_dfe_functions_st( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_dfe_functions_st *st);
static void _update_usr_ctrl_disable_dfe_functions_byte( const phymod_access_t *pa, struct falcon_tsc_usr_ctrl_disable_dfe_functions_st *st);

/* Core Config Struct */
static void _update_uc_core_config_st( const phymod_access_t *pa, struct  falcon_tsc_uc_core_config_st *st);
static void _update_uc_core_config_word( const phymod_access_t *pa, struct  falcon_tsc_uc_core_config_st *st);

#endif
