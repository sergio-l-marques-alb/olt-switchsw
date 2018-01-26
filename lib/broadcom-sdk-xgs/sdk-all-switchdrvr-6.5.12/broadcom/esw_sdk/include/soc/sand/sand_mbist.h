/* $Id: sand_mbist.h,v  $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef _SOC_SAND_MBIST_H
#define _SOC_SAND_MBIST_H

#include <sal/types.h>
#include <sal/core/time.h>
#include <soc/register.h>
#include <soc/shared/sat.h>

#define SAND_MBIST_COMMAND_WAIT    0
#define SAND_MBIST_COMMAND_WRITE   0x40
#define SAND_MBIST_COMMAND_READ    0x80
#define SAND_MBIST_COMMAND_COMMENT 0xc0

#define SAND_MBIST_COMMAND_MASK    0xc0
#define SAND_MBIST_COMMAND_INVMASK 0x3f

#define SAND_MBIST_TEST_LONG_WAIT_VALUE 0x3fffffff /* wait value representing to perform a long wait for testing purp_ses */
#define SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC 0x80000000 /* If this bit is 1, then the delay is specified in seconds, otherwise in us */

#define SAND_MBIST_UINT32(value) (value)>>24, ((value)>>16) & 0xff, ((value)>>8) & 0xff, (value) & 0xff

#define SAND_MBIST_READ(mask, expected_value, error_line_offset) SAND_MBIST_COMMAND_READ | (((error_line_offset)>>8) & \
  SAND_MBIST_COMMAND_INVMASK), (error_line_offset) & 0xff, SAND_MBIST_UINT32(mask), SAND_MBIST_UINT32(expected_value)
#define SAND_MBIST_WRITE(value) SAND_MBIST_COMMAND_WRITE, SAND_MBIST_UINT32(value)
#define SAND_MBIST_WAIT(time) SAND_MBIST_COMMAND_WAIT | (((time)>>24) & SAND_MBIST_COMMAND_INVMASK), ((time)>>16) & 0xff, ((time)>>8) & 0xff, (time) & 0xff
#define SAND_MBIST_COMMENT SAND_MBIST_COMMAND_COMMENT
#if defined(COMPILER_STRING_CONST_LIMIT) && !defined(INCLUDE_MBIST_COMMENTS)
#define SAND_MBIST_COMMENT_TEXT(comment) ""
#else
#define SAND_MBIST_COMMENT_TEXT(comment) comment
#endif


typedef struct sand_mbist_script_s {
    const uint8       *commands;
    const char        **comments;
    uint32      nof_commands;
    uint32      nof_comments;
    char        *script_name;
    uint32      sleep_after_write;
} sand_mbist_script_t;


typedef struct sand_mbist_dynamic_s {
    uint32      nof_errors;
    uint8       skip_errors;
    sal_usecs_t measured_time;
    uint32      ser_test_delay; /* The length of a delay in sec/us */
} sand_mbist_dynamic_t;


typedef struct sand_mbist_device_s {
    uint32      sleep_divisor;
    soc_reg_t   reg_tap_command;
    soc_reg_t   reg_tap_data_in;
    soc_reg_t   reg_tap_data_out;
} sand_mbist_device_t;



uint32 soc_sand_mbist_init_legacy(
    const int unit, 
    const sand_mbist_device_t *mbist_device, 
    sand_mbist_dynamic_t *dynamic);

uint32 soc_sand_mbist_init_new(
    const int unit, 
    const sand_mbist_device_t *mbist_device, 
    sand_mbist_dynamic_t *dynamic);

/* function to de-initialize the MBIST mechanism */
uint32 soc_sand_mbist_deinit(
                             const int unit, 
                             const sand_mbist_device_t *mbist_device, 
                             const sand_mbist_dynamic_t *dynamic
                             );

/* function to de-initialize the MBIST mechanism */
uint32 soc_sand_mbist_deinit_new(
                             const int unit, 
                             const sand_mbist_device_t *mbist_device, 
                             const sand_mbist_dynamic_t *dynamic
                             );

/* Legacy function to de-initialize the MBIST mechanism, writing a zero value */
uint32 soc_sand_mbist_deinit_legacy(
                             const int unit, 
                             const sand_mbist_device_t *mbist_device, 
                             const sand_mbist_dynamic_t *dynamic
                             );

uint32 soc_sand_run_mbist_script(
    const int unit, 
    const sand_mbist_script_t *script,  
    const sand_mbist_device_t *mbist_device, 
    sand_mbist_dynamic_t *dynamic);


typedef uint16 sand_mbist_line_offset; /* 14 bit offset of source script line from previous read */



/* 
 * test using a given MBIST script
 */
#define SAND_MBIST_TEST_SCRIPT(func, mbist_device) \
    SOCDNX_IF_ERR_EXIT(soc_sand_run_mbist_script(unit, & func ## _script, &mbist_device, &dynamic));

/*
 * Used to run an MBIST script after exit: , without handling errors in the script.
 */
#define SAND_MBIST_TEST_SCRIPT_DO_NOT_FAIL(func, mbist_device) \
    soc_sand_run_mbist_script(unit, & func ## _script, &mbist_device, &dynamic);


#endif /*_SOC_SAND_MBIST_H*/

