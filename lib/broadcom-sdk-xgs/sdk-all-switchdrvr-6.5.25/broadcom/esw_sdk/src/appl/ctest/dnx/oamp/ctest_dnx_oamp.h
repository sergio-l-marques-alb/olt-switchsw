/**
 * \file ctest_dnx_oamp.h
 *
 * 'Load balancing' operations (set and get parameters) procedures for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_OAMP_H_INCLUDED
#define CTEST_OAMP_H_INCLUDED

/** Test types */
#define TEST_TYPE_TRAFFIC 0
#define TEST_TYPE_SEMANTIC 1

/** LoopBack defines for on and off of the traffic */
#define LOOP_BACK_ENABLE BCM_PORT_LOOPBACK_MAC
#define LOOP_BACK_DISABLE BCM_PORT_LOOPBACK_NONE

/** Tables defined in ctest_dnx_oamp_traffic.c */
extern sh_sand_option_t diag_dnx_oamp_traffic_test_options[];
extern sh_sand_man_t diag_dnx_oamp_traffic_test_man;
extern sh_sand_invoke_t diag_dnx_oamp_traffic_tests[];

/** Tables defined in ctest_dnx_oamp_bfd.c */
extern sh_sand_option_t diag_dnx_oamp_bfd_test_options[];
extern sh_sand_man_t diag_dnx_oamp_bfd_test_man;
extern sh_sand_invoke_t diag_dnx_oamp_bfd_tests[];

typedef struct diag_dnx_oamp_traffic_test_time_calc_s
{
    uint32 start_time;
    uint32 end_time;
    /** Actual create time.*/
    uint32 actual_create_time;
    /** How many MEPs are included in time calculation */
    uint32 interaction;
    /** Expected time limit */
    uint32 expected_max_create_time;

} diag_dnx_oamp_traffic_test_time_calc_t;

/** Test function defined in ctest_dnx_oamp_traffic.c */
shr_error_e diag_dnx_oamp_traffic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/** Test function defined in ctest_dnx_oamp_bfd.c */
shr_error_e diag_dnx_oamp_bfd_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/** Test function defined in ctest_dnx_oamp_traffic.c */
shr_error_e diag_dnx_oamp_bfd_test_option_cb(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p);

/** */
void diag_dnx_oamp_traffic_test_time_calculation(
    diag_dnx_oamp_traffic_test_time_calc_t * time_param);

/** */
void diag_dnx_oamp_traffic_test_time_calc_t_init(
    diag_dnx_oamp_traffic_test_time_calc_t * time_param);

shr_error_e diag_dnx_oamp_test_loopback_on_off(
    int unit,
    bcm_port_t port_1,
    bcm_port_t port_2,
    int action);

typedef struct diag_dnx_oamp_test_bfd_sw_db_lock_s
{
    /** access locking */
    sal_mutex_t dnx_bfd_lock[BCM_LOCAL_UNITS_MAX];
    int is_init;
} diag_dnx_oamp_test_bfd_lock_t;

#endif /* CTEST_OAMP_H_INCLUDED */
