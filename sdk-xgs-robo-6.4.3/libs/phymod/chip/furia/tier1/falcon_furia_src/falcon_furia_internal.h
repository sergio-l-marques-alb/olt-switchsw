/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :  $Id: falcon_furia_internal.h 714 2014-09-18 19:54:56Z kirand $ *
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

/** @file falcon_furia_internal.h 
 * Internal functions
 */

#ifndef FALCON_FURIA_API_INTERNAL_H
#define FALCON_FURIA_API_INTERNAL_H

#define SDK_STATIC static


#include "falcon_furia_common.h"
#include "falcon_furia_field_access.h"
#include "../common/srds_api_err_code.h"
#include "falcon_furia_interface.h"
#include "falcon_furia_debug_functions.h"

/*------------------------------*/
/** Serdes OSR Mode Structure   */
/*------------------------------*/
typedef struct {
    uint8_t tx;
    uint8_t rx;
    uint8_t tx_rx;
}falcon_furia_osr_mode_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {       
       uint16_t ucv_config;
       int16_t rx_ppm;
       int16_t p1_lvl;       
       uint16_t link_time;
       falcon_furia_osr_mode_st osr_mode;
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
       int8_t tx_ppm;
       int8_t txfir_pre, txfir_main, txfir_post1;
       int8_t txfir_post2,txfir_post3;
       int8_t tx_amp_ctrl;
       uint16_t heye_left, heye_right,veye_upper,veye_lower;
       uint8_t br_pd_en;
} falcon_furia_lane_state_st;

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
       uint16_t vco_rate_mhz;
       uint8_t  analog_vco_range;
       uint8_t  pll_div;
       uint8_t  rescal;
} falcon_furia_core_state_st;


/** Check if the micro's operations on that lane are stopped 
 * @return err_code Error Code "ERR_CODE_UC_NOT_STOPPED"
  */
err_code_t _check_uc_lane_stopped(void);

/* calculate the mode_sel parameter for tx pattern generator */
err_code_t _calc_patt_gen_mode_sel(uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length);



/*-------------------*/
/*  Ladder controls  */
/*-------------------*/
/** Converts a ladder setting to mV, given the range
 * @param ctrl is the threshold control (-31..31) maps to -RANGE to RANGE in non-uniform steps
 * @param range_250 determines the range 0 = +/-150mV, 1 = +/-250mV
 * @return ladder threshold voltage in mV
 */
int16_t _ladder_setting_to_mV(int8_t ctrl, uint8_t range_250);




/*-----------------------*/
/*  TX_PI and ULL Setup  */
/*-----------------------*/

/** TX PI setup for Repeater Mode
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */ 
err_code_t _rptr_mode_timing(void);                


/** Setup the Ultra low latency clk and datapath
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */ 
err_code_t _ull_mode_setup(void); 
                  

/** (safe multiply) multiplies 2 numbers and checks for overflow
 * @param a input
 * @param b input
 * @param of pointer to overflow indicator
 * @return value of a * b 
 */ 
uint32_t _mult_with_overflow_check(uint32_t a, uint32_t b, uint8_t *of);

/*-----------------------------------------*/
/*  APIs used in Config Shared TX Pattern  */
/*-----------------------------------------*/
/** Compute Binary string for a Hex value ['0' to 'F']
 * @param var Hex value to be converted to Binary (eg: '6', 'A', ...)  
 * @param bin Binary string returned by API (eg: '0110', '1010', ...)
 * @return Error Code generated by invalid hex variable (returns ERR_CODE_NONE if no errors) 
 */ 
err_code_t _compute_bin(char var, char bin[]);


/** Compute Hex value for a Binary string ['0000' to '1111']
 * @param bin Binary string to be coverted (eg: '0110', '1010', ...)
 * @param hex Hex value calculated from the input Binary string
 * @return Error Code generated by invalid Binary string (returns ERR_CODE_NONE if no errors) 
 */ 
err_code_t _compute_hex(char bin[], uint8_t *hex);


/*-----------------------------------------*/
/*  APIs used in Read Event Logger         */
/*-----------------------------------------*/
/** Interpret CL72 coefficient status field
 * @param val coefficient status field value to be interpreted
 * @return char string to be displayed in event log 
 */ 
char* _status_val_2_str(uint8_t val);


/** Interpret CL72 coefficient update field
 * @param val coefficient update field value to be interpreted
 * @return char string to be displayed in event log
 */ 
char* _update_val_2_str(uint8_t val);

/** Interpret error event code
 * @param val event code error value to be interpreted
 * @return char string to be displayed in event log
 */ 
char* _error_val_2_str(uint8_t val);

/** Display event information
 * @param event_id event id to be displayed
 * @param entry_len length of the event entry
 * @param prev_cursor CL72 prev cursor value 
 * @param curr_cursor CL72 curr cursor value
 * @param post_cursor CL72 post cursor value
 * @param supp_info supplement information
 */ 
err_code_t _display_event(uint8_t event_id, uint8_t entry_len, uint8_t prev_cursor, uint8_t curr_cursor, uint8_t post_cursor, uint8_t *supp_info);


/** convert float8 to usigned int32
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input float8 number
 * @return usigned 32bit number  
 */
uint32_t _float8_to_int32(float8_t input);


#ifdef TO_FLOATS
/*-----------------------------------------*/
/*  APIs used in uC data conversion        */
/*-----------------------------------------*/

/** convert usigned int32 to float8
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input unsigned int
 * @return float8 8 bit representations of 32bit number  
 */
float8_t _int32_to_float8(uint32_t input);
#endif


/*-----------------------------*/
/*  Read / Display Core state  */
/*-----------------------------*/
/** Read current falcon_furia core status.
 * @param istate Current falcon_furia core status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _falcon_furia_read_core_state(falcon_furia_core_state_st *istate);

/** Display current core state.
 * Reads and displays all important core state values.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t _falcon_furia_display_core_state_no_newline(void); 

/*-----------------------------*/
/*  Read / Display Lane state  */
/*-----------------------------*/
/** Read current falcon_furia lane status.
 * @param istate Current falcon_furia lane status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _falcon_furia_read_lane_state(falcon_furia_lane_state_st *istate);

/*-----------------------------*/
/*  Get OSR mode              */
/*-----------------------------*/
/** Read current falcon_furia lane status.
 * @param mode Returns with the osr mode structure
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _falcon_furia_get_osr_mode(falcon_furia_osr_mode_st *mode);

/** Display current lane state.
 * Reads and displays all important lane state values.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t _falcon_furia_display_lane_state_no_newline(void);



/** Convert eye margin to mV
 * Takes in the ladder setting with 3 fractional bits and converts to mV
 * @param var ladder setting with 3 fractional bits
 * @param ladder_range specified if ladder is configured for 150mV or 250mV range
 * @return Eye opening in mV
 */
uint16_t _eye_to_mV(uint8_t var, uint8_t ladder_range); 

/** Convert eye margin to mUI
 * Takes in a horizontal margin in Phase Interpolator codes and converts it to mUI
 * @param var horizontal margin in Phase Interpolator codes with 3 fractional bits
 * @return Eye opening in mV
 */
uint16_t _eye_to_mUI(uint8_t var); 

/** Serdes Core ClockGate.
 * Along with falcon_furia_core_clkgate(), all lanes should also be clock gated using falcon_furia_lane_clkgate() to complete a Core Clockgate 
 * @param enable Enable clockgate (1 = Enable clokgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t _falcon_furia_core_clkgate(uint8_t enable);


/** Serdes Lane ClockGate.
 * @param enable Enable lane clockgate (1 = Enable clockgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t _falcon_furia_lane_clkgate(uint8_t enable);

/** Set function for PF
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_pf_main(uint8_t val);

/** Get function for PF
 * @return signed output value
 */
err_code_t _get_rx_pf_main(int8_t *val);

/** Set function for PF2
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_pf2(uint8_t val);

/** Get function for PF2
 * @return signed output value
 */
err_code_t _get_rx_pf2(int8_t *val);

/** Set function for VGA
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_vga(uint8_t val);

/** Get function for VGA
 * @return signed output value
 */
err_code_t _get_rx_vga(int8_t *val);


/** Set function for DFE1
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_dfe1(int8_t val);

/** Get function for DFE Tap
 * @return signed output value
 */
err_code_t _get_rx_dfe1(int8_t *val);

/** Set function for DFE2
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_dfe2(int8_t val);

/** Get function for DFE Tap
 * @return signed output value
 */
err_code_t _get_rx_dfe2(int8_t *val);

/** Set function for DFE3
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_dfe3(int8_t val);

/** Get function for DFE Tap
 * @return signed output value
 */
err_code_t _get_rx_dfe3(int8_t *val);

/** Set function for DFE4
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_dfe4(int8_t val);

/** Get function for DFE Tap
 * @return signed output value
 */
err_code_t _get_rx_dfe4(int8_t *val);

/** Set function for DFE5
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t _set_rx_dfe5(int8_t val);

/** Get function for DFE Tap
 * @return signed output value
 */
err_code_t _get_rx_dfe5(int8_t *val);

err_code_t _set_rx_dfe6(int8_t val);
err_code_t _get_rx_dfe6(int8_t *val);
err_code_t _set_rx_dfe7(int8_t val);
err_code_t _get_rx_dfe7(int8_t *val);
err_code_t _set_rx_dfe8(int8_t val);
err_code_t _get_rx_dfe8(int8_t *val);
err_code_t _set_rx_dfe9(int8_t val);
err_code_t _get_rx_dfe9(int8_t *val);
err_code_t _set_rx_dfe10(int8_t val);
err_code_t _get_rx_dfe10(int8_t *val);
err_code_t _set_rx_dfe11(int8_t val);
err_code_t _get_rx_dfe11(int8_t *val);
err_code_t _set_rx_dfe12(int8_t val);
err_code_t _get_rx_dfe12(int8_t *val);
err_code_t _set_rx_dfe13(int8_t val);
err_code_t _get_rx_dfe13(int8_t *val);
err_code_t _set_rx_dfe14(int8_t val);
err_code_t _get_rx_dfe14(int8_t *val);

err_code_t _set_tx_pre(uint8_t val);
err_code_t _set_tx_amp(int8_t val);
err_code_t _get_tx_amp(int8_t *val);
err_code_t _set_tx_main(uint8_t val);
err_code_t _set_tx_post1(uint8_t val);
err_code_t _set_tx_post2(int8_t val);
err_code_t _set_tx_post3(int8_t val);
err_code_t _get_tx_post3(int8_t *val);

/** Lane Config Struct */
void _update_uc_lane_config_st(struct falcon_furia_uc_lane_config_st *st);
void _update_uc_lane_config_word(struct falcon_furia_uc_lane_config_st *st);

/** Lane User Control Disable Startup Function Struct */
void _update_usr_ctrl_disable_functions_st(struct falcon_furia_usr_ctrl_disable_functions_st *st);
void _update_usr_ctrl_disable_functions_byte(struct falcon_furia_usr_ctrl_disable_functions_st *st);

/** Lane User Control Disable Startup DFE Function Struct */
void _update_usr_ctrl_disable_dfe_functions_st(struct falcon_furia_usr_ctrl_disable_dfe_functions_st *st);
void _update_usr_ctrl_disable_dfe_functions_byte(struct falcon_furia_usr_ctrl_disable_dfe_functions_st *st);

/** Core Config Struct */
void _update_uc_core_config_st(struct  falcon_furia_uc_core_config_st *st);
void _update_uc_core_config_word(struct  falcon_furia_uc_core_config_st *st);

#endif
