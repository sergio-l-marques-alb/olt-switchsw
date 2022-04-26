/** \file ctest_dnx_mpls_root.c
 *
 * Tests junction for MPLS CTests
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

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>

#include "ctest_dnx_mpls_esi.h"
#include "ctest_dnx_mpls_evpn.h"
#include "ctest_dnx_mpls_port.h"
#include "ctest_dnx_mpls_tunnel.h"
#include "ctest_dnx_mpls_bier.h"
#include "ctest_dnx_mpls_performance.h"

/** Test manual   */
sh_sand_man_t dnx_mpls_tunnel_test_man = {
    /** Brief */
    "Semantic test of MPLS TUNNEL related APIs",
    /** Full */
    "Add, delete, get, traverse and verify input validation with proper errors " "for TUNNEL management APIs.",
    /** Synopsis   */
    "ctest mpls tunnel"
};

/** List of mpls tests   */
sh_sand_cmd_t dnx_mpls_test_cmds[] = {
    {"esi", NULL, dnx_mpls_esi_tests}
    ,
    {"evpn", NULL, dnx_mpls_evpn_tests}
    ,
    {"port", NULL, dnx_mpls_port_tests}
    ,
    {"tunnel", NULL, dnx_mpls_tunnel_tests, NULL, &dnx_mpls_tunnel_test_man}
    ,
    {"bier", NULL, dnx_mpls_bier_tests}
    ,
    {"performance", NULL, dnx_mpls_performance_tests}
    ,
    {NULL}
};
