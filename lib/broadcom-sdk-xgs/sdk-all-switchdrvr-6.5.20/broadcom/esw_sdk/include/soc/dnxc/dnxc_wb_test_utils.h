/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _TEST_DNXC_WB_H_
#define _TEST_DNXC_WB_H_

/**********************************************************/
/*                  Defines                               */
/**********************************************************/

#define DNXC_WARM_BOOT_API_TEST_MODE_NONE                          (0)  /* don't perform any test */
#define DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API               (1)  /* perform wb test at end of every api call */

#define DNXC_WARMBOOT_API_FUNCTION_NAME_MAX_LEN 100

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#define BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)\
do {\
    dnxc_wb_test_mode_skip_wb_sequence(unit);\
    }while (0)
#else /* BCM_WARM_BOOT_SUPPORT && BCM_WARM_BOOT_API_TEST */
#define BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)
#endif /* BCM_WARM_BOOT_SUPPORT && BCM_WARM_BOOT_API_TEST */

typedef int (
    *dnxc_wb_test_callback_func_t) (
    int unit,
    void *userdata);

typedef struct
{
    dnxc_wb_test_callback_func_t dnxc_wb_test_callback_func;
    void *dnxc_wb_test_callback_data;
} dnxc_wb_test_callback_t;

/**********************************************************/
/*                  Functions                             */
/**********************************************************/

#ifdef BCM_WARM_BOOT_API_TEST

void dnxc_wb_api_test_filter_set(
    int unit,
    uint8 low_high,
    uint32 value);

void dnxc_wb_test_mode_set(
    int unit,
    int mode);

void dnxc_wb_test_mode_get(
    int unit,
    int *mode);

/* wb_flag == 1  - temporary disable WB test mode (OVERRIDE) */
/* wb_flag == 0  - re-enable WB test mode (RETRACT) */
/* for example - can be used by BCM APIs that internally call other BCM APIs after which 
   WB sequence should not be called 
   NOTE: !!! I'ts the caller responsability to call RETRACT for every OVERIDE call!!!
         !!! there is an internal counter here, nof retracts should equal nof overides !!! */

void dnxc_wb_no_wb_test_set(
    int unit,
    int wb_flag);

/*
 * Get WB test mode override counter value.
 */
void dnxc_wb_no_wb_test_get(
    int unit,
    int *override_counter);

/* wb_flag == TRUE  - Mark deinit-init during WB test. */
/* wb_flag == FALSE  - Restore the flag to its initial value. */
/* NOTE: !!! I'ts the caller responsability to call RESTORE for every MARK call!!!*/
void dnxc_wb_deinit_init_during_wb_test_set(
    int unit,
    int wb_flag);

/*
 * Gets if the deinit-init was called during WB test.
 */
int dnxc_wb_deinit_init_during_wb_test_get(
    int unit);

/* WB_TEST_MODE_SKIP_WB_SEQUENCE flag disable the WB test mode for current BCM API call.
 * i.e - if test mode is on (i.e - perform warm rebooot at the end of APIs), when turning this
 * flag on the driver will NOT perform WB sequence when the BCM API finish to run. 
 * for example - to be used by APIs that create a mismatch between SW state and HW state
 * (like in the field instructions that are saved to SW but not yet commited to HW) 
 * also used to exclude some not interesting _get APIs from wb regression */
int dnxc_wb_test_mode_skip_wb_sequence(
    int unit);

int dnxc_wb_api_test_counter_increase(
    int unit);
int dnxc_wb_api_test_counter_decrease(
    int unit);

int dnxc_warm_boot_api_test_reset(
    int unit);

void dnxc_warmboot_test_tr_141_command_set(
    int unit,
    int (*function_ptr) (int unit));

/** callback registration - give ability to add outside functionality to the WB procedure */
void dnxc_wb_test_callback_register(
    int unit,
    uint32 flags,
    dnxc_wb_test_callback_func_t callback,
    void *userdata);

void dnxc_wb_test_callback_unregister(
    int unit);

int dnxc_wb_all_reset_test_utils_preconditions(
    int unit);
#endif
/*************************************************************************************************/

#endif /* !_TEST_DNXC_WB_H_ */
