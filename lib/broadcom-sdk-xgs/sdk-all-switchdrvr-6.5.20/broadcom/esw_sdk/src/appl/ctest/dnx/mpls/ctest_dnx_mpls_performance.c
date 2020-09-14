/**
 * \file ctest_dnx_mpls_performance.c
 *
 * Tests for MPLS Performance:
 *   * Transit
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLSTESTSDNX

#include <bcm/mpls.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include "bcm/vlan.h"
#include "bcm/l2.h"
#include "bcm/l3.h"
#include "bcm/mpls.h"
#include "bcm/vswitch.h"

static shr_error_e
mpls_prfm_tst_arp_create(
    int unit,
    int vsi,
    int arp_encap_id)
{
    int flags = 0;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_mac_t arp_mac_add;
    SHR_FUNC_INIT_VARS(unit);
    arp_mac_add[0] = 0x00;
    arp_mac_add[1] = 0x00;
    arp_mac_add[2] = 0x00;
    arp_mac_add[3] = 0x00;
    arp_mac_add[4] = 0xcd;
    arp_mac_add[5] = 0x1d;
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, arp_mac_add, 6);
    l3eg.intf = 0;
    l3eg.encap_id = 0x40000000 | (0xfffff & arp_encap_id);
    l3eg.vlan = vsi;
    l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    flags = (BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY);
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_arp_delete(
    unsigned int unit,
    unsigned int arp_encap_id)
{
    int encap_id = 0x40000000 | (0xfffff & arp_encap_id);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, encap_id));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_add_txlsp(
    int unit,
    int txlsp_label,
    int mpls_tunnel_id,
    int arp_id)
{
    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].label = txlsp_label;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID;
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel_id);
    label_array[0].l3_intf_id = arp_id;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_add_txlsp_delete(
    int unit,
    int mpls_tunnel_id)
{
    int tunnel_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    BCM_L3_ITF_SET(tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel_id);
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_initiator_clear(unit, tunnel_id));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_add_rxlsp(
    int unit,
    int rxlsp_match_label,
    int rx_hw_id)
{
    bcm_mpls_tunnel_switch_t entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.flags = BCM_MPLS_SWITCH_WITH_ID;
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    entry.egress_if = 0;
    entry.tunnel_id = rx_hw_id;
    entry.vpn = 0;
    entry.label = rxlsp_match_label;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_create(unit, &entry));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_mpls_rxlsp_delete(
    int unit,
    int rxlsp_match_label,
    int rx_hw_id)
{
    bcm_mpls_tunnel_switch_t entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.flags = BCM_MPLS_SWITCH_WITH_ID;
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    entry.egress_if = 0;
    entry.tunnel_id = rx_hw_id;
    entry.vpn = 0;
    entry.label = rxlsp_match_label;
    SHR_IF_ERR_EXIT(bcm_mpls_tunnel_switch_delete(unit, &entry));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_mpls_vc_egress_create(
    int unit,
    int encap_id,
    int encap_label,
    int vpn_id)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.encap_id = encap_id;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = encap_label;
    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port.port = 201;
    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, vpn_id, &mpls_port));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_l2_egress_mpls_port_delete(
    unsigned int unit,
    unsigned int outlif_id,
    unsigned int vpn_id)
{
    bcm_gport_t gport_id;
    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, outlif_id);
    BCM_GPORT_MPLS_PORT_ID_SET(gport_id, gport_id);
    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, vpn_id, gport_id));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_mpls_vc_term_crearte(
    int unit,
    int ingress_encap_id,
    int egress_lable,
    int vpn_id)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, ingress_encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = egress_lable;
    mpls_port.port = 0;
    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, vpn_id, &mpls_port));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mpls_prfm_tst_l2_ingress_mpls_port_delete(
    unsigned int unit,
    unsigned int inlif_id,
    int vpn_id)
{
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_SUB_TYPE_LIF_SET(gport, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, inlif_id);
    BCM_GPORT_MPLS_PORT_ID_SET(gport, gport);
    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, vpn_id, gport));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_mpls_ingress_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rxlsp_match_label = 0x3333;
    int rx_hw_id = 0x4c002116;
    int ii;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;
    int nof_entries = 1000;
#ifndef ADAPTER_SERVER_MODE
    uint32 threshold_time_per_timer[2] = { 600, 400 };/** Very loose threshold, to pass all CPUs and compilation */
    uint8 threshold_error = FALSE;
#endif
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("MPLS Ingress tunnel performance test", &timers_group));
    SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(timers_group));
    timer_indx = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS In Lif tunnel add", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_add_rxlsp(unit, rxlsp_match_label + ii, rx_hw_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS In Lif tunnel delete", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_mpls_rxlsp_delete(unit, rxlsp_match_label + ii, rx_hw_id + ii));
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

static shr_error_e
dnx_mpls_egress_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int vsi_id = 123;
    int arp_encap_id = 0x3002;
    int txlsp_label = 0x3333;
    int mpls_tunnel_encap_id = 0x2002;
    int arp_id = 0;
    int ii;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;
    int nof_entries = 1000;
#ifndef ADAPTER_SERVER_MODE
    uint32 threshold_time_per_timer[2] = { 600, 400 };/** Very loose threshold, to pass all CPUs and compilation */
    uint8 threshold_error = FALSE;
#endif
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * ARP create
     */
    SHR_IF_ERR_EXIT(mpls_prfm_tst_arp_create(unit, vsi_id, arp_encap_id));
    arp_id = 0x40000000 + arp_encap_id;
    /*
     * First allocation
     */
    SHR_IF_ERR_EXIT(mpls_prfm_tst_add_txlsp(unit, txlsp_label - 1, mpls_tunnel_encap_id - 1, arp_id));

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("MPLS Egress tunnel performance test", &timers_group));
    SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(timers_group));
    timer_indx = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS Out-Lif tunnel add", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_add_txlsp(unit, txlsp_label + ii, mpls_tunnel_encap_id + ii, arp_id));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS Out-Lif tunnel delete", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_add_txlsp_delete(unit, mpls_tunnel_encap_id + ii));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;

    SHR_IF_ERR_EXIT(mpls_prfm_tst_add_txlsp_delete(unit, mpls_tunnel_encap_id - 1));
    SHR_IF_ERR_EXIT(mpls_prfm_tst_arp_delete(unit, arp_encap_id));

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

static shr_error_e
dnx_mpls_ingress_port_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int vpn_id = 0;
    int egress_label = 0x3eb;
    int ingress_encap_id = 0x3300;
    int ii;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;
    int nof_entries = 1000;
#ifndef ADAPTER_SERVER_MODE
    uint8 threshold_error = FALSE;
    uint32 threshold_time_per_timer[2] = { 600, 400 };/** Very loose threshold, to pass all CPUs and compilation */
#endif

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("MPLS port Ingress tunnel performance test", &timers_group));
    SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(timers_group));
    timer_indx = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS port In Lif tunnel add", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_mpls_vc_term_crearte(unit, ingress_encap_id + ii, egress_label, vpn_id));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS port In Lif tunnel delete", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_l2_ingress_mpls_port_delete(unit, ingress_encap_id + ii, vpn_id));
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

static shr_error_e
dnx_mpls_egress_port_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int encap_label = 0x3eb;
    int egress_encap_id = 0x3300;
    int vpn_id = 0;
    uint32 timers_group, nof_timers, timer_indx;
    char *timers_group_name;
    int nof_entries = 1000;
    int ii;
#ifndef ADAPTER_SERVER_MODE
    uint8 threshold_error = FALSE;
    uint32 threshold_time_per_timer[2] = { 600, 400 };/** Very loose threshold, to pass all CPUs and compilation */
#endif

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * First allocation
     */
    SHR_IF_ERR_EXIT(mpls_prfm_tst_mpls_vc_egress_create(unit, egress_encap_id - 1, encap_label - 1, vpn_id));

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("MPLS port Egress tunnel performance test", &timers_group));
    SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(timers_group));
    timer_indx = 0;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS port Out-Lif tunnel add", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_mpls_vc_egress_create(unit, egress_encap_id + ii, encap_label + ii, vpn_id));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;
    for (ii = 0; ii < nof_entries; ii++)
    {
        SHR_IF_ERR_EXIT(utilex_ll_timer_set("MPLS port Out-Lif tunnel delete", timers_group, timer_indx));
        SHR_IF_ERR_EXIT(mpls_prfm_tst_l2_egress_mpls_port_delete(unit, egress_encap_id + ii, vpn_id));
        SHR_IF_ERR_EXIT(utilex_ll_timer_stop(timers_group, timer_indx));
    }
    timer_indx++;

    SHR_IF_ERR_EXIT(mpls_prfm_tst_l2_egress_mpls_port_delete(unit, egress_encap_id - 1, vpn_id));

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
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Performance is slower than expected. TH: create=%d, remove=%d\n",
                     threshold_time_per_timer[0], threshold_time_per_timer[1]);
    }
#endif
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** List of mpls performance tests   */
sh_sand_cmd_t dnx_mpls_performance_tests[] = {
    {"INGressTeST", dnx_mpls_ingress_performance, NULL}
    ,
    {"EGRessTeST", dnx_mpls_egress_performance, NULL}
    ,
    {"INGressPortTeST", dnx_mpls_ingress_port_performance, NULL}
    ,
    {"EGRessPortTeST", dnx_mpls_egress_port_performance, NULL}
    ,
    {NULL}
};

/* *INDENT-ON* */
