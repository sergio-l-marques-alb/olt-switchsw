/* $Id: dcmn_mbist.h,v  $
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

#ifndef _SOC_DCMN_MBIST_H
#define _SOC_DCMN_MBIST_H

#include <sal/types.h>
#include <soc/shared/sat.h>

#define DCMN_MBIST_COMMAND_WAIT    0
#define DCMN_MBIST_COMMAND_WRITE   0x40
#define DCMN_MBIST_COMMAND_READ    0x80
#define DCMN_MBIST_COMMAND_COMMENT 0xc0

#define DCMN_MBIST_COMMAND_MASK    0xc0
#define DCMN_MBIST_COMMAND_INVMASK 0x3f

#define DCMN_MBIST_TEST_LONG_WAIT_VALUE 0x3fffffff /* wait value representing to perform a long wait for testing purp_ses */
#define DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC 0x80000000 /* If this bit is 1, then the delay is specified in seconds, otherwise in us */

#define DCMN_MBIST_UINT32(value) (value)>>24, ((value)>>16) & 0xff, ((value)>>8) & 0xff, (value) & 0xff

#define DCMN_MBIST_READ(mask, expected_value, error_line_offset) DCMN_MBIST_COMMAND_READ | (((error_line_offset)>>8) & \
  DCMN_MBIST_COMMAND_INVMASK), (error_line_offset) & 0xff, DCMN_MBIST_UINT32(mask), DCMN_MBIST_UINT32(expected_value)
#define DCMN_MBIST_WRITE(value) DCMN_MBIST_COMMAND_WRITE, DCMN_MBIST_UINT32(value)
#define DCMN_MBIST_WAIT(time) DCMN_MBIST_COMMAND_WAIT | (((time)>>24) & DCMN_MBIST_COMMAND_INVMASK), ((time)>>16) & 0xff, ((time)>>8) & 0xff, (time) & 0xff
#define DCMN_MBIST_COMMENT DCMN_MBIST_COMMAND_COMMENT
#ifdef COMPILER_STRING_CONST_LIMIT
#define DCMN_MBIST_COMMENT_TEXT(comment) ""
#else
#define DCMN_MBIST_COMMENT_TEXT(comment) comment
#endif


typedef struct dcmn_mbist_script_s {
    const uint8       *commands;
    const char        **comments;
    uint32      nof_commands;
    uint32      nof_comments;
    char        *script_name;
    uint32      sleep_after_write;
} dcmn_mbist_script_t;


typedef struct dcmn_mbist_dynamic_s {
    uint32      nof_errors;
    uint8       skip_errors;
    sal_usecs_t measured_time;
    uint32      ser_test_delay; /* The length of a delay in sec/us */
} dcmn_mbist_dynamic_t;


typedef struct dcmn_mbist_device_s {
    uint32      sleep_divisor;
    soc_reg_t   reg_tap_command;
    soc_reg_t   reg_tap_data_in;
    soc_reg_t   reg_tap_data_out;
} dcmn_mbist_device_t;



uint32 soc_dcmn_mbist_init(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic);
/* function to de-initialize the MBIST mechanism */
uint32 soc_dcmn_mbist_deinit(
                             SOC_SAND_IN int unit, 
                             SOC_SAND_IN dcmn_mbist_device_t *mbist_device, 
                             SOC_SAND_IN dcmn_mbist_dynamic_t *dynamic
                             );

uint32 soc_dcmn_run_mbist_script(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN dcmn_mbist_script_t *script,  
    SOC_SAND_IN dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic);


typedef uint16 dcmn_mbist_line_offset; /* 14 bit offset of source script line from previous read */



/* 
 * test using a given MBIST script
 */
#define DCMN_MBIST_TEST_SCRIPT(func, mbist_device) \
    SOCDNX_IF_ERR_EXIT(soc_dcmn_run_mbist_script(unit, & func ## _script, &mbist_device, &dynamic));


#endif /*_SOC_DCMN_MBIST_H*/

