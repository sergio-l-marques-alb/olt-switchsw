/*
 * ! \file diag_swstate_tests.h Purpose: shell registers commands for sw state tests 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXF_FABRIC_H_INCLUDED
#define DIAG_DNXF_FABRIC_H_INCLUDED

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

/* 
* { Local Structures
*/

/* 
* } Local Structures
*/

/* 
* { Local Functions
*/
shr_error_e diag_dnxf_queues_diag_print(
    int unit,
    int queue_stage_selected,
    sh_sand_control_t * sand_control);
shr_error_e diag_dnxf_thresholds_diag_print(
    int unit,
    int th_group_selected,
    int th_stage_selected,
    int pipe_selected,
    int priority_selected,
    int link_selected,
    sh_sand_control_t * sand_control);
shr_error_e diag_dnxf_counters_graphical_print(
    int unit,
    sh_sand_control_t * sand_control);

/* 
* } Local Functions
*/
#endif /* DIAG_DNXF_FABRIC_H_INCLUDED */
