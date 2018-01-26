/*
 * $Id: cmdlist.h,v 1.26 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    cmdlist.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */

#ifndef _INCLUDE_CTEST_DNXF_ROOT_H
#define _INCLUDE_CTEST_DNXF_ROOT_H

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (RAMON) family only!"
#endif

#include <appl/diag/shell.h>
#include <shared/shrextend/shrextend_error.h>

extern const char cmd_dnxf_ctest_usage[];

/**
 * \brief Routine called from DNX shell on ctest
 * Entry point of any ctest command
 */
cmd_result_t cmd_dnxf_ctest_invoke(
    int unit,
    args_t * args);

cmd_result_t cmd_dnxf_ctest_init(
    int unit);

cmd_result_t cmd_dnxf_ctest_deinit(
    int unit);

#endif /* _INCLUDE_CTEST_DNX_ROOT_H */
