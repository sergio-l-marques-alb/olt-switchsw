
/**
 * \file ctest_dnx_switch.c
 *
 * Purpose:    Routines for handling debug and internal 'switch' procedures
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

 /*
  * Include files.
  * {
  */
#include "ctest_dnx_switch_load_balancing.h"
/*
 * }
 */
/**
 * Enum for type of LOAD_BALANCING/DNX test within 'switch' tests
 * See 'ctest_dnx_switch_load_balancing.c'
 */
sh_sand_enum_t Switch_load_balancing_test_type_enum_dnx_table[] = {
    {"LB_TCAM_INFO", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TCAM_INFO_TYPE, "Test 'crc select' tcam: set and get."}
    ,
    {"LB_CLIENT_CONVERT", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CLIENT_CONVERT_TYPE,
     "Test 'physical' to 'logical client conversion (and back)."}
    ,
    {"LB_RESERVED_MPLS", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_MPLS_RESERVED_TYPE,
     "Test 'mpls reserved labels': set and get."}
    ,
    {"LB_IDENT_MPLS", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_MPLS_IDENT_TYPE,
     "Test 'mpls 5-bit identification': set and get."}
    ,
    {"LB_CRC_SEED", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CRC_SEED_TYPE,
     "Test 'crc seed': set and get."}
    ,
    {"LB_BIT_SELECT", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_BIT_SELECT_TYPE,
     "Test 'crc seed': set and get."}
    ,
    {"LB_GENERAL_SEED", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_GENERAL_SEED_TYPE,
     "Test 'general crc seed': set and get."}
    ,

    {NULL}
};
/**
 * Enum for type of LOAD_BALANCING/BCM test within 'switch' tests
 * See 'ctest_dnx_switch_load_balancing.c'
 */
sh_sand_enum_t Switch_load_balancing_test_type_enum_bcm_table[] = {
    {"LB_SET", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CTRL_INDEX_SET_TYPE,
     "Test bcm_dnx_switch_control_indexed_set(): set operation."}
    ,
    {"LB_GET", CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CTRL_INDEX_GET_TYPE,
     "Test bcm_dnx_switch_control_indexed_get(): get operation."}
    ,

    {NULL}
};

/* *INDENT-OFF* */

sh_sand_cmd_t Dnx_switch_test_lb_cmds[] = {
   /* Name                 | Leaf Action              | Junction Array Pointer | Option list for leaf                   | Man page                              | Dynamic Option CB   */
    {"bcm",      sh_dnx_switch_load_balancing_bcm_cmd, NULL,                   Sh_dnx_switch_load_balancing_bcm_options,  &Sh_dnx_switch_load_balancing_bcm_man, NULL, Sh_dnx_switch_load_balancing_bcm_tests},
    {"dnx",      sh_dnx_switch_load_balancing_dnx_cmd, NULL,                   Sh_dnx_switch_load_balancing_dnx_options,  &Sh_dnx_switch_load_balancing_dnx_man, NULL, Sh_dnx_switch_load_balancing_dnx_tests},
    {NULL}
};
static sh_sand_man_t Dnx_switch_test_lb_man = {
    "'Switch/load-balancing'-related test utilities",
    "Test load balancing for either BCM level or DNX level",
    "",
    "",
};
sh_sand_cmd_t Dnx_switch_test_cmds[] = {
   /* Name            | Leaf Action | Junction Array Pointer   | Option list for leaf | Man page         | Dynamic Option CB   */
    {"load_balancing",    NULL,       Dnx_switch_test_lb_cmds,   NULL,                  &Dnx_switch_test_lb_man},
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
