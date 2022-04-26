/* $Id: dcmn_mbist.h,v  $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef _SOC_DCMN_MBIST_H
#define _SOC_DCMN_MBIST_H

#include <soc/sand/sand_mbist.h>

#define DCMN_MBIST_COMMAND_WAIT    SAND_MBIST_COMMAND_WAIT
#define DCMN_MBIST_COMMAND_WRITE   SAND_MBIST_COMMAND_WRITE
#define DCMN_MBIST_COMMAND_READ    SAND_MBIST_COMMAND_READ
#define DCMN_MBIST_COMMAND_COMMENT SAND_MBIST_COMMAND_COMMENT

#define DCMN_MBIST_COMMAND_MASK    SAND_MBIST_COMMAND_MASK
#define DCMN_MBIST_COMMAND_INVMASK SAND_MBIST_COMMAND_INVMASK

#define DCMN_MBIST_TEST_LONG_WAIT_VALUE SAND_MBIST_TEST_LONG_WAIT_VALUE /* wait value representing to perform a long wait for testing purp_ses */
#define DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC /* If this bit is 1, then the delay is specified in seconds, otherwise in us */

#define DCMN_MBIST_UINT32(value) SAND_MBIST_UINT32(value)

#define DCMN_MBIST_READ(mask, expected_value, error_line_offset) SAND_MBIST_READ(mask, expected_value, error_line_offset)
#define DCMN_MBIST_WRITE(value) SAND_MBIST_WRITE(value)
#define DCMN_MBIST_WAIT(time) SAND_MBIST_WAIT(time)
#define DCMN_MBIST_COMMENT SAND_MBIST_COMMENT
#define DCMN_MBIST_COMMENT_TEXT(comment) SAND_MBIST_COMMENT_TEXT(comment)


typedef sand_cpu2tap_script_t dcmn_mbist_script_t;


typedef sand_cpu2tap_dynamic_t dcmn_mbist_dynamic_t;


typedef sand_cpu2tap_device_t dcmn_mbist_device_t;



uint32 soc_dcmn_mbist_init_legacy(
    const int unit, 
    const dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic);

/* function to de-initialize the MBIST mechanism */
uint32 soc_dcmn_mbist_deinit(
                             const int unit, 
                             const dcmn_mbist_device_t *mbist_device, 
                             const dcmn_mbist_dynamic_t *dynamic
                             );

uint32 soc_dcmn_run_mbist_script(
    const int unit, 
    const dcmn_mbist_script_t *script,  
    const dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic);



/* 
 * test using a given MBIST script
 */
#define DCMN_MBIST_TEST_SCRIPT(func, mbist_device) SAND_MBIST_TEST_SCRIPT(func, mbist_device)

/*
 * Used to run an MBIST script after exit: , without handling errors in the script.
 */
#define DCMN_MBIST_TEST_SCRIPT_DO_NOT_FAIL(func, mbist_device) \
    SAND_MBIST_TEST_SCRIPT_DO_NOT_FAIL(func, mbist_device)


#endif /*_SOC_DCMN_MBIST_H*/

