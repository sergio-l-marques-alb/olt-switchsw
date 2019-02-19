/* $Id: ui_consts.h,v 1.2 Broadcom SDK $
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
*/


#ifndef __UI_CONSTS_H_INCLUDED__
/* { */
#define __UI_CONSTS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


/*
 * Special editing characters: ASCII codes.
 */
/*
 * CTRL-A
 */
#define DEBUG_INFO             0x01
/*
 * CTRL-B
 */
/*
 * CTRL-C
 */
#define CTRL_C                 0x03
#define FLUSH_STD_OUTPUT       CTRL_C
/*
 * Ctrl-e. Enter 'no echo' mode
 */
#define CTRL_E                 0x05
#define ENTER_ECHO_MODE_OFF    CTRL_E
/*
 * Ctrl-F. Enter 'echo' mode
 */
#define CTRL_F                 0x06
#define ENTER_ECHO_MODE_ON     CTRL_F
/*
 * CTRL-H
 */
#define BACK_SPACE             0x08
/*
 * CTRL-I
 */
#define TAB                    0x09
/*
 * CTRL-J
 */
#define LINE_FEED              0x0A
/*
 * CTRL-K
 */
#define CLEAR_LINE             0x0B
/*
 * CTRL-M
 */
#define CARRIAGE_RETURN        0x0D
/*
 * CTRL-T
 */
#define CLEAR_TYPE_AHEAD       0x14
/*
 * Ctrl-U. Equivalent of 'up-arrow'
 */
#define CTRL_U                 0x15
#define CTRL_H                 0x48

/*
 * Ctrl-d. Equivalent of 'down-arrow'
 */
#define CTRL_D                 0x04
#define CTRL_P                 0x50

#ifdef __VXWORKS__
  #define DOWN_ARROW             CTRL_D
  #define UP_ARROW               CTRL_U
#else
  #define DOWN_ARROW             CTRL_H
  #define UP_ARROW               CTRL_P
#endif

#define CTRL_Q                 0x11
#define RESTART_TERMINAL       CTRL_Q
#define EXIT_TERMINAL          '`'

/*
 * Escape key
 */
#define ESC                    0x1B
/*
 * Space
 */
#define BLANK_SPACE            0x20
/*
 * Quotation mark (for text with spaces)
 */
#define QUOTATION_MARK         0x22
/*
 * Dollar sign (special character for marking space and
 * for marking specila fields)
 */
#define SPECIAL_DOLLAR         0x24
#define SPECIAL_DOLLAR_STRING  "$"
/*
 * Single quotation mark (for text within text)
 */
/*
 * Question mark (for help)
 */
#define QUESTION_MARK          0x3F
/*
 * DEL key (on PC)
 */
#define DEL_KEY                0x7F
/*
 * Maximal number of characters in any symbol in this
 * system (including ending null). Must be larger
 * than maximal number of characters in any field
 * on current line.
 */
#define MAX_SIZE_OF_SYMBOL      50
/*
 * BELL character on ansi chracter table.
 */
#define BELL_CHAR               ((char)(0x7))
/*
 * Maximal number of characters in text input variable
 * (including ending null).
 */
#define MAX_SIZE_OF_TEXT_VAR           140
/*
 * Maximal number of parameter strings on one line.
 */
#define MAX_PARAM_NAMES_ON_LINE 50
/*
 * Maximal number of parameters per one subject.
 * This system will only accept that many parameters
 * from input rom arrays.
 */
#define MAX_PARAMS_PER_SUBJECT         2800
/*
 * Maximal number of values per one parameter.
 * This system will only accept that many values
 * from input rom arrays.
 */
#define MAX_VALUES_PER_PARAM           400
/*
 * Maximal number of characters per help one value.
 */
#define MAX_CHAR_PER_HELP_PER_VALUE    (80*3)
/*
 * Maximal number of subjects in this system.
 * This system will only accept that much subjects
 * from input rom arrays.
 */
#define MAX_SUBJECTS_IN_UI             100
/*
 * Maximal number of parameter strings and corresponding
 * values on one line.
 */
#define MAX_PARAMS_VAL_PAIRS_ON_LINE   300
/*
 * Maximal number of times a value can be repeated
 * for one parameter (e.g. 'data' on 'mem write' command).
 */
#define MAX_REPEATED_PARAM_VAL         300
/*
 * The size of screen (terminal) line.
 */
#define MAX_CHARS_ON_SCREEN_LINE       79
/*
 * Maximal number of characters acceptable on one line.
 * One line is 4 screen lines
 */
#define MAX_CHARS_ON_LINE    MAX_CHARS_ON_SCREEN_LINE*15
/*
 * Maximal number of characters on a null terminated string
 * representing IP address. Add 2 bytes as spare.
 */
#define MAX_IP_STRING (4*3+3+1+2)
#define BROADCAST_IP 0xFFFFFFFF
#define INVALID_UNICAST_IP  (BROADCAST_IP)
#define DEFAULT_DOWNLOAD_IP (INVALID_UNICAST_IP)


#ifdef  __cplusplus
}
#endif


/* } __UI_CONSTS_H_INCLUDED__*/
#endif
