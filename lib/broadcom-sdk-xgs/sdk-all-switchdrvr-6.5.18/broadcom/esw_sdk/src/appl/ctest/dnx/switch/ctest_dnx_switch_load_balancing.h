/**
 * \file ctest_dnx_load_balancing.h
 *
 * 'Load balancing' operations (set and get parameters) procedures for DNX.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_DNX_SWITCH_LOAD_BALANCING_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_ACTION_H_INCLUDED
/*
* Include files.
* {
*/
/*
 * appl
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*
 * bcm
 */
#include <bcm/switch.h>
#include <bcm/types.h>
#include <bcm/error.h>
/*
 * sal
 */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*
 * soc
 */
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/**
 * Describe the various LOAD_BALANCING tests.
 * Definitions used to fill enum for type of LOAD_BALANCING test within 'switch' tests
 * See 'ctest_dnx_switch_load_balancing.c'.
 */
typedef enum
{
    /**
     * Invalid test type
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TYPE_INVALID = -1,
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TYPE_FIRST = 0,
    /**
     * DNX level:
     * Carry out test of LOAD_BALANCING: Check utilities dnx_switch_load_balancing_tcam_info_get()
     * and dnx_switch_load_balancing_tcam_info_set()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TCAM_INFO_TYPE = CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TYPE_FIRST,
    /**
     * DNX level:
     * Carry out test of LOAD_BALANCING: Check utilities dnx_switch_lb_logical_client_to_physical()
     * and dnx_switch_lb_physical_client_to_logical()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CLIENT_CONVERT_TYPE,
    /**
     * DNX level:
     * Carry out test of MPLS_RESERVED labels: Check utilities
     * dnx_switch_load_balancing_mpls_reserved_label_set()
     * and dnx_switch_load_balancing_mpls_reserved_label_get()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_MPLS_RESERVED_TYPE,
    /**
     * DNX level:
     * Carry out test of MPLS_RESERVED labels: Check utilities
     * dnx_switch_load_balancing_mpls_identification_set()
     * and dnx_switch_load_balancing_mpls_identification_get()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_MPLS_IDENT_TYPE,
    /**
     * DNX level:
     * Carry out test of crc seed: Check utilities
     * dnx_switch_load_balancing_crc_seed_set()
     * and dnx_switch_load_balancing_crc_seed_get()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CRC_SEED_TYPE,
    /**
     * DNX level:
     * Carry out test of crc seed: Check utilities
     * dnx_switch_load_balancing_lsms_crc_select_set()
     * and dnx_switch_load_balancing_lsms_crc_select_get()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_BIT_SELECT_TYPE,
    /**
     * DNX level:
     * Carry out test of general crc seed: Check utilities
     * dnx_switch_load_balancing_general_seeds_set()
     * and dnx_switch_load_balancing_general_seeds_get()
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_GENERAL_SEED_TYPE,
    /**
     * BCM level:
     * Carry out test of invoking bcm_dnx_switch_control_indexed_set()
     * with general parameters, as per:
     *   bcm_switch_control_key_t (inpout)
     *     'Type' and 'index'
     *   bcm_switch_control_info_t (input)
     *     'value'
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CTRL_INDEX_SET_TYPE,
    /**
     * BCM level:
     * Carry out test of invoking bcm_dnx_switch_control_indexed_get()
     * with general parameters, as per:
     *   bcm_switch_control_key_t (input)
     *     'Type' and 'index'
     *   bcm_switch_control_info_t (output)
     *     'value'
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CTRL_INDEX_GET_TYPE,
    /**
     * Number of types in ctest_dnx_switch_load_balancing_test_type_e
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TYPE_NOF
} ctest_dnx_switch_load_balancing_test_type_e;

/**
 * Describe the various LOAD_BALANCING levels (bcm/dnx) for the various tests.
 * Definitions used to fill enum for level of LOAD_BALANCING test within 'switch' tests
 * See 'ctest_dnx_switch_load_balancing.c'.
 */
typedef enum
{
    /**
     * Invalid test type
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_LEVEL_INVALID = -1,
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_LEVEL_FIRST = 0,
    /**
     * Carry out test of LOAD_BALANCING for BCM level
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_LEVEL_BCM = CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_LEVEL_FIRST,
    /**
     * Carry out test of LOAD_BALANCING for DNX level
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_LEVEL_DNX,
    /**
     * Number of types in ctest_dnx_switch_load_balancing_test_level_e
     */
    CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_LEVEL_NOF
} ctest_dnx_switch_load_balancing_test_level_e;
/*
 * }
 */

extern sh_sand_man_t Sh_dnx_switch_load_balancing_bcm_man;
extern sh_sand_man_t Sh_dnx_switch_load_balancing_dnx_man;
/**
 * \brief
 *   Options list for 'load_balancing' DNX-tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_load_balancing_dnx_starter() in ctest_dnx_switch_load_balancing.c
 */
extern sh_sand_invoke_t Sh_dnx_switch_load_balancing_dnx_tests[];
/**
 * \brief
 *   Options list for 'load_balancing' BCM-tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_load_balancing_bcm_starter() in ctest_dnx_switch_load_balancing.c
 */
extern sh_sand_invoke_t Sh_dnx_switch_load_balancing_bcm_tests[];
/**
 * \brief
 *   Options list for 'load_balancing/dnx' shell command
 * \see
 *   appl_dnx_load_balancing_dnx_starter() in ctest_dnx_switch_load_balancing.c
 */
extern sh_sand_option_t Sh_dnx_switch_load_balancing_dnx_options[];
/**
 * \brief
 *   Options list for 'load_balancing/bcm' shell command
 * \see
 *   appl_dnx_load_balancing_bcm_starter() in ctest_dnx_switch_load_balancing.c
 */
extern sh_sand_option_t Sh_dnx_switch_load_balancing_bcm_options[];
/**
 * \brief
 * Enum for type of LOAD_BALANCING/DNX test within 'switch' tests
 * See 'ctest_dnx_switch_load_balancing.c'
 */
extern sh_sand_enum_t Switch_load_balancing_test_type_enum_dnx_table[];
/**
 * \brief
 * Enum for type of LOAD_BALANCING/BCM test within 'switch' tests
 * See 'ctest_dnx_switch_load_balancing.c'
 */
extern sh_sand_enum_t Switch_load_balancing_test_type_enum_bcm_table[];

/**
 * \brief - run 'load_balancing/bcm' sequence in diag shell
 */
shr_error_e sh_dnx_switch_load_balancing_bcm_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - run 'load_balancing/dnx' sequence in diag shell
 */
shr_error_e sh_dnx_switch_load_balancing_dnx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* CTEST_DNX_SWITCH_LOAD_BALANCING_H_INCLUDED */
