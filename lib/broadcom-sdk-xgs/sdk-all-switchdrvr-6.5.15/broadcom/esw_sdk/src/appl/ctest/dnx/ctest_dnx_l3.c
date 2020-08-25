/** \file ctest_dnx_l3.c
 * $Id$
 *
 * Contains all of the L3 ctest commands
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_L3DNX
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
#include "ctest_dnx_l3.h"

/**
 * DEFINES
 * {
 */

extern sh_sand_cmd_t dnx_ecmp_test_cmds[];
extern sh_sand_cmd_t dnx_l3_host_test_cmds[];
extern sh_sand_cmd_t dnx_l3_route_test_cmds[];
extern sh_sand_cmd_t dnx_l3_fec_test_cmds[];

/** L3 host test details */
static sh_sand_man_t sh_dnx_l3_host_man = {
    "L3 Host tests",
    "Tests related to bcm_l3_host_* APIs and actions"
};

/** L3 route test details */
static sh_sand_man_t sh_dnx_l3_route_man = {
    "L3 Route tests",
    "Tests related to bcm_l3_route_* APIs and actions"
};

/** L3 ecmp test details */
static sh_sand_man_t sh_dnx_l3_ecmp_man = {
    "Egress ECMP tests",
    "Tests related to bcm_l3_egress_ecmp_* APIs and actions"
};

/** L3 fec test details */
static sh_sand_man_t sh_dnx_l3_fec_man = {
    "Egress interface FEC tests",
    "Tests related to bcm_l3_egress_* APIs and actions"
};

/*
 * }
 */

sh_sand_cmd_t dnx_l3_test_cmds[] = {
    {"host", NULL, dnx_l3_host_test_cmds, NULL, &sh_dnx_l3_host_man},
    {"route", NULL, dnx_l3_route_test_cmds, NULL, &sh_dnx_l3_route_man},
    {"fec", NULL, dnx_l3_fec_test_cmds, NULL, &sh_dnx_l3_fec_man},
    {"ecmp", NULL, dnx_ecmp_test_cmds, NULL, &sh_dnx_l3_ecmp_man},
    {NULL}
};
