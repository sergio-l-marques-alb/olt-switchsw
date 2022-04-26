/**
 * \file ctest_dnx_vlan_port.c
 *
 * Tests for vlan port:
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

#include <sal/core/boot.h>
#include <bcm_int/dnx/auto_generated/dnx_vlan_dispatch.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include "bcm/port.h"
#include "bcm/vlan.h"
#include "bcm/l2.h"
#include "bcm/failover.h"
#include "bcm/vswitch.h"

#ifdef BSL_LOG_MODULE
#undef BSL_LOG_MODULE
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

static bcm_error_t
vlan_port_prfm_ingress_ac_create(
    int unit,
    int vid,
    uint32 flags,
    uint32 group,
    int ingress_failover_id,
    int vlan_port_id)
{
    bcm_vlan_port_t entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&entry);
    entry.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    entry.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    entry.flags |= BCM_VLAN_PORT_WITH_ID;
    entry.flags |= flags;
    entry.match_vlan = vid;
    entry.vlan_port_id = vlan_port_id;
    entry.port = 200;
    entry.group = group;
    entry.ingress_failover_id = ingress_failover_id;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &entry));
exit:
    SHR_FUNC_EXIT;
}
static bcm_error_t
vlan_port_prfm_ac_create(
    int unit,
    int vid,
    uint32 flags,
    int vlan_port_id)
{
    bcm_vlan_port_t entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&entry);
    entry.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    entry.flags |= BCM_VLAN_PORT_WITH_ID;
    entry.flags |= flags;
    entry.match_vlan = vid;
    entry.vlan_port_id = vlan_port_id;
    entry.port = 200;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &entry));
exit:
    SHR_FUNC_EXIT;
}

static bcm_error_t
vlan_port_prfm_ingress_match_add(
    int unit,
    int gport,
    int svid,
    int cvid)
{
    bcm_port_match_info_t entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_port_match_info_t_init(&entry);
    entry.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;
    entry.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    entry.match_vlan = svid;
    entry.match_inner_vlan = cvid;
    SHR_IF_ERR_EXIT(bcm_port_match_add(unit, gport, &entry));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_ingress_perf(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int flags, ii, jj;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;

    int svlan = 2;
    int cvlan = 2;
    int vlan_port_id = 0x44901001;
    int nof_entries = 1000;
    int nof_match_per_lif = 5;

#ifndef ADAPTER_SERVER_MODE
    uint32 threshold_time_per_timer[8] = { 600, 50000, 100000, 100000, 1000, 2000, 60000, 600 };
    /*
     * Very loose threshold, to pass all CPUs and compilation
     */
    uint8 threshold_error = FALSE;
    char *step;
    uint32 threshold;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Performance testing is not supported in simulation\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("Vlan port performance test", &timers_group));
    utilex_ll_timer_clear_all(timers_group);

    timer_indx = 0;
    flags = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("Vlan port ingress create", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        for (jj = 0; jj < nof_match_per_lif; jj++)
        {
            SHR_IF_ERR_EXIT(utilex_ll_timer_set("Vlan port match add", timers_group, timer_indx));
            SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_match_add(unit, vlan_port_id + ii, svlan + ii, cvlan + jj));
            SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
        }
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_INGRESS_WIDE | BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to wide", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to default", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_STAT_INGRESS_ENABLE | BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to with stats", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to default 2nd", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("delete vlan match", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_port_match_delete_all(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("delete vlan port", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    nof_timers = timer_indx;
    SHR_IF_ERR_EXIT(utilex_ll_timers_group_name_get(timers_group, &timers_group_name));
    PRT_TITLE_SET("%s", timers_group_name);
    PRT_COLUMN_ADD("Timer index");
    PRT_COLUMN_ADD("Timer Name");
    PRT_COLUMN_ADD("nof_hits");
    PRT_COLUMN_ADD("total_time [us]");
    PRT_COLUMN_ADD("Avg. per hit [us]");

    for (timer_indx = 0; timer_indx < nof_timers; timer_indx++)
    {
        int is_active;
        uint32 total_time;
        uint32 nof_hits;
        char *name;
        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_indx, &is_active, &name, &nof_hits, &total_time));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", timer_indx);
        PRT_CELL_SET("%s", name);
        PRT_CELL_SET("%d", nof_hits);
        PRT_CELL_SET("%d", total_time);
        PRT_CELL_SET("%d", total_time / nof_hits);
#ifndef ADAPTER_SERVER_MODE
        if (total_time / nof_hits > threshold_time_per_timer[timer_indx])
        {
            threshold_error = TRUE;
            step = name;
            threshold = threshold_time_per_timer[timer_indx];
        }
#endif
    }
    PRT_COMMIT;
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_free(timers_group));
#ifndef ADAPTER_SERVER_MODE
    if (threshold_error)
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Performance of %s is slower than expected, TH=%d, ", step, threshold);
    }
#endif
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_ingress_protected_perf(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flags, ii, jj;
    int failover_id;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;

    int svlan = 2;
    int cvlan = 2;
    int vlan_port_id = 0x44901001;
    int nof_entries = 1000;
    int nof_match_per_lif = 5;

#ifndef ADAPTER_SERVER_MODE
    uint32 threshold_time_per_timer[8] = { 600, 50000, 600, 600, 600, 600, 60000, 600 };
                                                                                  /** Very loose threshold, to pass all CPUs and compilation */
    char *step;
    uint32 threshold;
    uint8 threshold_error = FALSE;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Performance testing is not supported in simulation\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(bcm_failover_create(unit, BCM_FAILOVER_INGRESS, &failover_id));

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("Vlan port performance test", &timers_group));
    utilex_ll_timer_clear_all(timers_group);

    timer_indx = 0;
    flags = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("Vlan port ingress create", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, failover_id, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        for (jj = 0; jj < nof_match_per_lif; jj++)
        {
            SHR_IF_ERR_EXIT(utilex_ll_timer_set("Vlan port match add", timers_group, timer_indx));
            SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_match_add(unit, vlan_port_id + ii, svlan + ii, cvlan + jj));
            SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
        }
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_INGRESS_WIDE | BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to wide", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, failover_id, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to default", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, failover_id, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_STAT_INGRESS_ENABLE | BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to with stats", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, failover_id, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to default 2nd", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, 0, failover_id, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("delete vlan match", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_port_match_delete_all(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("delete vlan port", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    nof_timers = timer_indx;
    SHR_IF_ERR_EXIT(utilex_ll_timers_group_name_get(timers_group, &timers_group_name));
    PRT_TITLE_SET("%s", timers_group_name);
    PRT_COLUMN_ADD("Timer index");
    PRT_COLUMN_ADD("Timer Name");
    PRT_COLUMN_ADD("nof_hits");
    PRT_COLUMN_ADD("total_time [us]");
    PRT_COLUMN_ADD("Avg. per hit [us]");

    for (timer_indx = 0; timer_indx < nof_timers; timer_indx++)
    {
        int is_active;
        uint32 total_time;
        uint32 nof_hits;
        char *name;
        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_indx, &is_active, &name, &nof_hits, &total_time));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", timer_indx);
        PRT_CELL_SET("%s", name);
        PRT_CELL_SET("%d", nof_hits);
        PRT_CELL_SET("%d", total_time);
        PRT_CELL_SET("%d", total_time / nof_hits);
#ifndef ADAPTER_SERVER_MODE
        if (total_time / nof_hits > threshold_time_per_timer[timer_indx])
        {
            threshold_error = TRUE;
            step = name;
            threshold = threshold_time_per_timer[timer_indx];

        }
#endif
    }
    PRT_COMMIT;
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_free(timers_group));
#ifndef ADAPTER_SERVER_MODE
    if (threshold_error)
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Performance of %s is slower than expected, TH=%d, ", step, threshold);

    }
#endif
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_ingress_ext_perf(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int flags, ii, jj;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;

    int svlan = 2;
    int cvlan = 2;
    int group = 2;
    int vlan_port_id = 0x44901001;
    int nof_entries = 1000;
    int nof_match_per_lif = 5;

#ifndef ADAPTER_SERVER_MODE
    uint32 threshold_time_per_timer[6] = { 800, 50000, 800, 800, 60000, 800 };
                                                                          /** Very loose threshold, to pass all CPUs and compilation */
    char *step;
    uint32 threshold;
    uint8 threshold_error = FALSE;
#endif
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Performance testing is not supported in simulation\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("Vlan port performance test", &timers_group));
    utilex_ll_timer_clear_all(timers_group);

    timer_indx = 0;
    flags = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("Vlan port ingress create", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, group, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        for (jj = 0; jj < nof_match_per_lif; jj++)
        {
            SHR_IF_ERR_EXIT(utilex_ll_timer_set("Vlan port match add", timers_group, timer_indx));
            SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_match_add(unit, vlan_port_id + ii, svlan + ii, cvlan + jj));
            SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
        }
    }
    timer_indx++;

    flags = BCM_VLAN_PORT_STAT_INGRESS_ENABLE | BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to with stats", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, group, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    flags = BCM_VLAN_PORT_REPLACE;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("vlan port replace to default", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ingress_ac_create(unit, svlan + ii, flags, group, 0, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("delete vlan match", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_port_match_delete_all(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("delete vlan port", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    nof_timers = timer_indx;
    SHR_IF_ERR_EXIT(utilex_ll_timers_group_name_get(timers_group, &timers_group_name));
    PRT_TITLE_SET("%s", timers_group_name);
    PRT_COLUMN_ADD("Timer index");
    PRT_COLUMN_ADD("Timer Name");
    PRT_COLUMN_ADD("nof_hits");
    PRT_COLUMN_ADD("total_time [us]");
    PRT_COLUMN_ADD("Avg. per hit [us]");

    for (timer_indx = 0; timer_indx < nof_timers; timer_indx++)
    {
        int is_active;
        uint32 total_time;
        uint32 nof_hits;
        char *name;
        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_indx, &is_active, &name, &nof_hits, &total_time));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", timer_indx);
        PRT_CELL_SET("%s", name);
        PRT_CELL_SET("%d", nof_hits);
        PRT_CELL_SET("%d", total_time);
        PRT_CELL_SET("%d", total_time / nof_hits);
#ifndef ADAPTER_SERVER_MODE
        if (total_time / nof_hits > threshold_time_per_timer[timer_indx])
        {
            threshold_error = TRUE;
            step = name;
            threshold = threshold_time_per_timer[timer_indx];

        }
#endif
    }
    PRT_COMMIT;
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_free(timers_group));
#ifndef ADAPTER_SERVER_MODE
    if (threshold_error)
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Performance of %s is slower than expected, TH=%d, ", step, threshold);

    }
#endif
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_p2p_perf(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int ii;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;

    int flags = 0;
    int svlan = 1000;
    int svlan2 = 3000;
    int vlan_port_id = 0x44801001;
    int peer_vlan_port_id = 0x44803001;
    int nof_entries = 1000;

#ifndef ADAPTER_SERVER_MODE
    uint32 threshold_time_per_timer[3] = { 10000, 300, 8000 };
                                                            /** Very loose threshold, to pass all CPUs and compilation */
    uint8 threshold_error = FALSE;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Performance testing is not supported in simulation\n")));
        SHR_EXIT();
    }

    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(vlan_port_prfm_ac_create(unit, svlan + ii, flags, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(vlan_port_prfm_ac_create(unit, svlan2 + ii, flags, peer_vlan_port_id + ii));

    }

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("Vlan port performance test", &timers_group));
    utilex_ll_timer_clear_all(timers_group);

    timer_indx = 0;

    for (ii = 0; ii < nof_entries; ii++)
    {
        bcm_vswitch_cross_connect_t cross_connect;
        bcm_vswitch_cross_connect_t_init(&cross_connect);
        cross_connect.port1 = vlan_port_id + ii;
        cross_connect.port2 = peer_vlan_port_id + ii;
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("Convert vlan port to P2P", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_add(unit, &cross_connect));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;

    for (ii = 0; ii < nof_entries; ii++)
    {
        bcm_vswitch_cross_connect_t cross_connect;
        bcm_vswitch_cross_connect_t_init(&cross_connect);
        cross_connect.port1 = vlan_port_id + ii;
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("Get vlan port", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_get(unit, &cross_connect));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));

    }
    timer_indx++;

    for (ii = 0; ii < nof_entries; ii++)
    {
        bcm_vswitch_cross_connect_t cross_connect;
        bcm_vswitch_cross_connect_t_init(&cross_connect);
        cross_connect.port1 = vlan_port_id + ii;
        cross_connect.port2 = peer_vlan_port_id + ii;
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("Convert vlan port to MP", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_delete(unit, &cross_connect));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;

    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, vlan_port_id + ii));
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, peer_vlan_port_id + ii));
    }

    nof_timers = timer_indx;
    SHR_IF_ERR_EXIT(utilex_ll_timers_group_name_get(timers_group, &timers_group_name));
    PRT_TITLE_SET("%s", timers_group_name);
    PRT_COLUMN_ADD("Timer index");
    PRT_COLUMN_ADD("Timer Name");
    PRT_COLUMN_ADD("nof_hits");
    PRT_COLUMN_ADD("total_time [us]");
    PRT_COLUMN_ADD("Avg. per hit [us]");

    for (timer_indx = 0; timer_indx < nof_timers; timer_indx++)
    {
        int is_active;
        uint32 total_time;
        uint32 nof_hits;
        char *name;
        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_indx, &is_active, &name, &nof_hits, &total_time));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", timer_indx);
        PRT_CELL_SET("%s", name);
        PRT_CELL_SET("%d", nof_hits);
        PRT_CELL_SET("%d", total_time);
        PRT_CELL_SET("%d", total_time / nof_hits);
#ifndef ADAPTER_SERVER_MODE
        if (total_time / nof_hits > threshold_time_per_timer[timer_indx])
        {
            threshold_error = TRUE;
        }
#endif
    }
    PRT_COMMIT;
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_free(timers_group));
#ifndef ADAPTER_SERVER_MODE
    if (threshold_error)
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Performance is slower than expected. TH: Create=%d, Delete=%d\n",
                     threshold_time_per_timer[0], threshold_time_per_timer[1]);
    }
#endif
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** Test manual   */
static sh_sand_man_t dnx_vlan_port_ingress_perf_test_man = {
    /** Brief */
    "Performance test of ingress AC with replacement",
    /** Full */
    "Create 1k IN-LIF with 15 matches per lif" "Replace to wide lif" "Restore to default IN-LiF"
        "Replace to with stats",
    /** Synopsis   */
    "ctest vlan port"
};

static sh_sand_man_t dnx_vlan_port_ingress_protected_perf_test_man = {
    /** Brief */
    "Performance test of ingress proteced AC with replacement",
    /** Full */
    "Create 1k IN-LIF with 15 matches per lif" "Replace to wide lif" "Restore to default IN-LiF"
        "Replace to with stats",
    /** Synopsis   */
    "ctest vlan port"
};

static sh_sand_man_t dnx_vlan_port_ingress_ext_perf_test_man = {
    /** Brief */
    "Performance test of ingress EXT AC with replacement",
    /** Full */
    "Create 1k IN-LIF with 15 matches per lif" "Replace to with stats" "Restore to default IN-LiF",
    /** Synopsis   */
    "ctest vlan port"
};

static sh_sand_man_t dnx_vlan_port_p2p_perf_test_man = {
    /** Brief */
    "Performance test of MP->P2P->MP",
    /** Full */
    "Create 2K symmetric MP LIFs" "Update to P2P LIFs " "Restore to MP LIFs",
    /** Synopsis   */
    "ctest vlan port"
};

/** List of vlan port performance tests   */
sh_sand_cmd_t dnx_vlan_port_test_cmds[] = {
    {"ingress_performance", dnx_vlan_port_ingress_perf, NULL, NULL, &dnx_vlan_port_ingress_perf_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"ingress_protected_performance", dnx_vlan_port_ingress_protected_perf, NULL, NULL,
     &dnx_vlan_port_ingress_protected_perf_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"ingress_ext_performance", dnx_vlan_port_ingress_ext_perf, NULL, NULL, &dnx_vlan_port_ingress_ext_perf_test_man,
     NULL, NULL,
     CTEST_POSTCOMMIT},
    {"p2p_performance", dnx_vlan_port_p2p_perf, NULL, NULL, &dnx_vlan_port_p2p_perf_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {NULL}
};

#undef BSL_LOG_MODULE

/* *INDENT-ON* */
