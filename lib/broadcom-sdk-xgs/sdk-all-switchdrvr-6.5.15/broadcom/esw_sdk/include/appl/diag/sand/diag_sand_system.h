/**
 * \file diag_sand_system.h
 *
 * Framework utilities, structures and definitions
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SAND_SYSTEM_H_INCLUDED
#define SAND_SYSTEM_H_INCLUDED

#include <appl/diag/sand/diag_sand_prt.h>

typedef struct
{
    prt_control_t *prt_ctr;
    int failed;
    int succeeded;
    int skipped;
} shell_flex_t;

#endif /* SAND_SYSTEM_H_INCLUDED */
