/**
 * \file diag_sand_system.h
 *
 * Framework utilities, structures and definitions
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SAND_SYSTEM_H_INCLUDED
#define SAND_SYSTEM_H_INCLUDED

#include <appl/diag/sand/diag_sand_prt.h>

typedef struct
{
    sh_sand_control_t *sand_control;
    prt_control_t *prt_ctr;
    int failed;
    int succeeded;
    int skipped;
} shell_flex_t;

#endif /* SAND_SYSTEM_H_INCLUDED */
