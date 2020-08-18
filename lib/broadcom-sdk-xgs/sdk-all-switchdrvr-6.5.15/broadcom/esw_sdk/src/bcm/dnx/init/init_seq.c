/** \file bcm/dnx/init/init_seq.c
 * DNX init seq functions.
 *
 * The DNX init seq should be constructed of a series of well defined steps.
 * each step should have a separate init and deinit functions, where the deinit function
 * should not access the HW but just free allocated resources.
 *
 *
 * each step should have a built in mechanism to test for freeing all allocated resources.
 * each step should have a built in time stamping mechanism to provide an option to test for deviation from time frames.
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/control.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/srv6/srv6.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/switch/switch_load_balancing.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/kbp/kbp_xpt.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/oam/oam_init.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/flow_control.h>
#include <bcm_int/dnx/cosq/cosq_latency.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <bcm_int/dnx/bier/bier.h>
#include <bcm_int/dnx/spb/spb.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <bcm_int/dnx/trunk/trunk_init.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/port/port_ingress.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/sat/sat.h>
#include <bcm_int/dnx/algo/sat/algo_sat.h>
#include <bcm_int/dnx/stg/stg.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <bcm_int/dnx/pvt/pvt.h>
#include <bcm_int/dnx/ptp/ptp.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/switch/switch_reflector.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/access.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/legacy/mbcm.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>

#include <soc/dnxc/drv.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_mbist.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/mdb.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnxc/dnxc_mem_cache.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/dnx_sw_state.h>
#include <soc/dnx/arr/auto_generated/arr_output.h>
#include <appl/diag/dnx/ikleap/auto_generated/diag_dnx_ikleap_layers_info.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm_int/dnx/algo/mpls/algo_mpls.h>
#include <bcm_int/dnx/algo/vlan/algo_vlan.h>
#include <bcm_int/dnx/algo/stg/algo_stg.h>
#include <bcm_int/dnx/algo/instru/algo_instru.h>
#include <bcm_int/dnx/algo/l2/algo_l2.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_init.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/ppmc/algo_ppmc.h>
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <bcm_int/dnx/algo/stat_pp/algo_stat_pp.h>
#include <bcm_int/dnx/algo/ptp/algo_ptp.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/synce/algo_synce.h>
#include <src/bcm/dnx/port/port_utils.h>
#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/synce/synce.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>

#include "init_cb_wrappers.h"
#include "init_pp.h"
#include "init_pemla.h"
#include <bcm_int/dnx/intr/intr_init.h>
#include <bcm_int/dnx/link/link.h>

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif /* BCM_WARM_BOOT_SUPPORT && BCM_WARM_BOOT_API_TEST */

/*
 * }
 */

/*
 * Defines
 * {
 */
/**
 * \brief step should be skipped in warm reboot
 */
#define WB_SKIP     DNX_INIT_STEP_F_WB_SKIP
#define DISABLE_WB_CHECK    DNX_INIT_STEP_F_WB_DISABLE_CHECKS
/**
 * \brief step required for access only initialization (allow light initialization with access only)
 */
#define ACCESS      DNX_INIT_STEP_F_REQUIRED_FOR_ACCESS_ONLY
/**
 * \brief step required for access only (including per port access) initialization (allow light with access only)
 */
#define ACCESS_PORT DNX_INIT_STEP_F_REQUIRED_FOR_PORT_ACCESS_ONLY
/**
 * \brief can be run as standalone
 */
#define STANDALONE_EN DNX_INIT_STEP_F_STANDALONE_EN
/*
 * }
 */

/** callback function which skips J2C */
static shr_error_e
j2c_skip(
    int unit,
    int *dynamic_flags)
{
    if (SOC_IS_J2C(unit))
    {
        *dynamic_flags = DNX_INIT_STEP_F_SKIP;
    }
    else
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

/** callback function which skips BIER */
static shr_error_e
bier_skip(
    int unit,
    int *dynamic_flags)
{
    if (dnx_data_bier.params.nof_bfr_entries_get(unit))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = DNX_INIT_STEP_F_SKIP;
    }
    return _SHR_E_NONE;
}

/** callback function which skips the step if we are in WB test mode */
static shr_error_e
wb_test_mode_skip(
    int unit,
    int *dynamic_flags)
{
    int wb_test_mode = 0;
    /*
     * Gets warmboot test mode.
     */
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    dnxc_wb_test_mode_get(unit, &wb_test_mode);
#endif /* BCM_WARM_BOOT_SUPPORT && BCM_WARM_BOOT_API_TEST */

    if (wb_test_mode == 1)
    {
        *dynamic_flags = DNX_INIT_STEP_F_SKIP;
    }
    else
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

/*
 * Init Deinit step lists, Indent needs to ignore the format here
 * {
 */
/**
 * So how does this mechanism work?
 *    The init process start at the begining of the high level seq list and performs
 *    each step according to order. a step can expand to a list of sub steps. the deinint
 *    process is done in the reverse order of the init process and is used to free all allocated
 *    resources from the init.
 *
 * General guidelines regarding how to add a new Step:
 *  1.  Implement the Init, Deinit and flag CB. good practice will be to implement both init and deinit steps even if one of them is an empty function
 *      for future profing.
 *  2.  if your module already has an Init/Deinit functions, you can only use a wrapper to match the needed call for the CB.
 *  3.  in init.h add a step id to the enum to match your step, all step IDs must be unique and match only a single step.
 *  4.  add an entry to the relevant list below with your CBs, step ID and other needed info.
 *
 * Specific guidelines regarding how to add a new sub-step list:
 *  1.  it is possible to add as many sub-steps list as one desires and in as many cascaded level as needed,
 *      common sense however dictates that no more than 5 cascaded levels are really needed (and that also is an over-kill).
 *  2.  to add a new sub-step list create in the origin list a new step.
 *      use pointer to the sub-step list for the SUB_STEP_LIST_PTR.
 *      you may add a flag CB function with logic when to skip this list (not mandatory).
 *  3.  you can find an example for this with either one of the existing sub-step lists ( TM, PP etc..)
 */
/* *INDENT-OFF* */
/**
 * \brief Init-Deinit common modules seq.
 *
 * For specific info on steps look for each function description
 * See dnx_field_init(), dnx_field_init_deinit()
 */
static const  dnx_init_step_t dnx_init_deinit_field_seq[] = {
  /*STEP_ID,                      STEP_NAME,     STEP_INIT,                      STEP_DEINIT,                    FLAG_CB           STEP_FLAGS   TIME_THRESH   SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_FLD_MAPPING,     "FP Mapping",  dnx_field_map_init,             dnx_field_map_deinit,           NULL,             0,           0,            NULL},
  {DNX_INIT_STEP_FLD_SWSTATE,     "FP SwState",  dnx_field_init_sw_state,        dnx_field_deinit_sw_state,      NULL,             WB_SKIP,     1800000,      NULL},
  {DNX_INIT_STEP_FLD_TABLES,      "FP Tables",   dnx_field_init_tables,          NULL,                           NULL,             WB_SKIP,     1600000,      NULL},
  {DNX_INIT_STEP_FLD_TCAMFW,      "FP TCAMFW",   dnx_field_init_tcam_fwd_tables, NULL,                           NULL,             WB_SKIP,     0,            NULL},
  {DNX_INIT_STEP_FLD_CONTEXT,     "FP Context",  dnx_field_context_init,         dnx_field_context_deinit,       NULL,             WB_SKIP,     0,            NULL},
  {DNX_INIT_STEP_FLD_PRESEL,      "FP Presel",   dnx_field_presel_init,          dnx_field_presel_deinit,        NULL,             WB_SKIP,     0,            NULL},
  {DNX_INIT_STEP_FLD_UDH,         "FP UDH",      dnx_field_udh_init,             NULL,                           NULL,             WB_SKIP,     0,            NULL},
  {DNX_INIT_STEP_FLD_SYS_HDRS,    "FP SysHdrs",  dnx_field_system_headers_init,  NULL,                           NULL,             WB_SKIP,     0,            NULL},
  {DNX_INIT_STEP_LAST_STEP,       "Last Step",   NULL,                           NULL,                           NULL,             0,           0,            NULL}
};
static const  dnx_init_step_t dnx_init_deinit_common_seq[] = {
  /*STEP_ID,                      STEP_NAME,         STEP_INIT,                    STEP_DEINIT,                    FLAG_CB           STEP_FLAGS         TIME_THRESH   SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_AUX_ACCESS,      "Signals&Aux",     shr_access_device_init,       shr_access_device_deinit,       NULL,             DISABLE_WB_CHECK,  2800000,      NULL},
  {DNX_INIT_STEP_RX,              "RX",              dnx_init_rx_init,             dnx_init_rx_deinit,             NULL,             0,                 0,            NULL},
  {DNX_INIT_STEP_TX,              "TX",              dnx_init_tx_init,             dnx_init_tx_deinit,             NULL,             0,                 0,            NULL},
  {DNX_INIT_STEP_RCPU,            "RCPU",            soc_dnx_rcpu_init,            NULL,                           NULL,             WB_SKIP,           0,            NULL},
  {DNX_INIT_STEP_PVT_MON,         "PVT mon",         dnx_pvt_mon_init,             NULL,                           NULL,             WB_SKIP,           0,            NULL},
  {DNX_INIT_STEP_PEMLA,           "PEMLA",           dnx_init_pemla_init,          dnx_init_pemla_deinit,          NULL,             DISABLE_WB_CHECK,  9300000,      NULL},
  {DNX_INIT_STEP_KBP_MNGR,        "KBP manager",     kbp_mngr_init,                kbp_mngr_deinit,                NULL,             0,                 0,            NULL},
  {DNX_INIT_STEP_LAST_STEP,       "Last Step",       NULL,                         NULL,                           NULL,             0,                 0,            NULL}
};
/**
 * \brief Init-Deinit TM seq.
 *
 * For specific info on steps look for each function description
 */
static const  dnx_init_step_t dnx_init_deinit_tm_seq[] = {
  /*STEP_ID,                                    STEP_NAME,              STEP_INIT,                      STEP_DEINIT,                        FLAG_CB     STEP_FLAGS         TIME_THRESH    SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_IPQ_MODULE,                    "Ipq",                  dnx_cosq_ipq_init,              dnx_cosq_ipq_deinit,                j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_IQS_MODULE,                    "Iqs",                  dnx_iqs_init,                   dnx_iqs_deinit,                     j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_HBMC,                          "HBMC",                 dnx_hbmc_init,                  dnx_hbmc_deinit,                    j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_DRAM_BUFFERS,                  "Dram Buffers",         dnx_dram_buffers_init,          dnx_dram_buffers_deinit,            j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_SPB,                           "Spb",                  dnx_spb_init,                   dnx_spb_deinit,                     j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_MULTICAST_MODULE,              "Multicast",            dnx_multicast_init,             dnx_multicast_deinit,               NULL,       WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_BIER_MODULE,                   "Bier",                 dnx_bier_init,                  dnx_bier_deinit,                    bier_skip,  WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_PORT_INGRESS_MODULE,           "Ingress Port",         dnx_port_ingress_init,          dnx_port_ingress_deinit,            NULL,       WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_MIRROR_MODULE,                 "Mirror",               dnx_mirror_init,                dnx_mirror_deinit,                  j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_EGRESS_CONGESTION_MODULE,      "Ecgm",                 dnx_ecgm_init,                  dnx_ecgm_deinit,                    j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_EGR_QUEUING_MODULE,            "EGR queuing",          dnx_egr_queuing_init,           dnx_egr_queuing_deinit,             j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_OFP_RATES_MODULE,              "OFP rates",            dnx_ofp_rates_init,             dnx_ofp_rates_deinit,               j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_CRPS_MODULE,                   "Crps",                 dnx_crps_init,                  dnx_crps_deinit,                    j2c_skip,   0,                 0,             NULL},
  {DNX_INIT_STEP_POLICER_MODULE,                "Policer",              dnx_policer_init,               dnx_policer_deinit,                 j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_STIF_MODULE,                   "Stif",                 dnx_stif_init,                  dnx_stif_deinit,                    j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_TRUNK_MODULE,                  "Trunk",                dnx_trunk_init,                 dnx_trunk_deinit,                   NULL,       WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_SAT_MODULE,                    "SAT",                  dnx_sat_init,                   dnx_sat_deinit,                     j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_PORT_MODULE,                   "PORT",                 dnx_port_init,                  dnx_port_deinit,                    j2c_skip,   DISABLE_WB_CHECK,  0,             NULL},
  {DNX_INIT_STEP_COSQ_INGRESS_MODULE,           "COSQ Ingress",         dnx_cosq_ingress_init,          dnx_cosq_ingress_deinit,            j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_COSQ_LATENCY_MODULE,           "Latency",              dnx_cosq_latency_init,          dnx_cosq_latency_deinit,            j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_FLOW_CONTROL_MODULE,           "Flow Control",         bcm_dnx_fc_init,                bcm_dnx_fc_deinit,                  j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_SCH_MODULE,                    "E2E scheduler",        dnx_sch_init,                   dnx_sch_deinit,                     j2c_skip,   WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_FABRIC_MODULE,                 "Fabric",               dnx_fabric_init,                dnx_fabric_deinit,                  j2c_skip,   WB_SKIP,           3500000,       NULL},
  {DNX_INIT_STEP_MIB_MODULE,                    "MIB",                  dnx_mib_init,                   dnx_mib_deinit,                     j2c_skip,   0,                 0,             NULL},
  {DNX_INIT_STEP_SER,                           "SER",                  dnx_intr_init,                  dnx_intr_deinit,                    j2c_skip,   DISABLE_WB_CHECK,  1550000,       NULL},
  {DNX_INIT_STEP_LINKSCAN,                      "LINKSCAN",             dnx_linkscan_init,              dnx_linkscan_deinit,                j2c_skip,   0,                 0,             NULL},
  {DNX_INIT_STEP_LAST_STEP,                     "Last Step",            NULL,                           NULL,                               NULL,       0,                 0,             NULL}
};

/**
 * \brief Init-Deinit PP seq.
 *
 * For specific info on steps look for each function decription
 */
static const dnx_init_step_t dnx_init_deinit_pp_seq[] = {
  /*STEP_ID,                            STEP_NAME,                STEP_INIT,                                STEP_DEINIT,                          FLAG_CB          FLAGS      TIME_THRESH    SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_ARR,                   "ARR",                    dnx_arr_output_init,                      NULL,                                 NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_PP_GENERAL,            "General",                dnx_general_pp_init,                      dnx_general_pp_deinit,                NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_HW_OVERWRITE,          "HW Overwrites",          dnx_init_hw_overwrite_init,               dnx_init_hw_overwrite_deinit,         NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_LIF,                   "Lif",                    dnx_lif_lib_init,                         dnx_lif_lib_deinit,                   NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_FAILOVER,              "Failover",               dnx_failover_init,                        dnx_failover_deinit,                  NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_IN_LIF_PROFILE_MODULE, "InLIF Profile",          dnx_in_lif_profile_module_init,           dnx_in_lif_profile_module_deinit,     NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_OUT_LIF_PROFILE_MODULE,"OutLIF Profile",         dnx_out_lif_profile_module_init,          dnx_out_lif_profile_module_deinit,    NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_L2_MODULE,             "L2",                     dnx_l2_module_init,                       dnx_l2_module_deinit,                 NULL,            0,         0,             NULL},
  {DNX_INIT_STEP_L3_MODULE,             "L3",                     dnx_l3_module_init,                       dnx_l3_module_deinit,                 NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_LOAD_BALANCING_MODULE, "Load_Balancing",         dnx_switch_load_balancing_module_init,    dnx_switch_load_balancing_module_deinit, NULL,         WB_SKIP,   0,             NULL},
  {DNX_INIT_MPLS,                       "Mpls",                   dnx_mpls_module_init,                     dnx_mpls_module_deinit,               NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_PP_PORT,               "Port",                   dnx_pp_port_init,                         dnx_pp_port_deinit,                   NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_STG,                   "Stg",                    dnx_stg_init,                             dnx_stg_deinit,                       NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_SRV6,                  "SRv6",                   dnx_srv6_init,                            dnx_srv6_deinit,                      NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_VLAN,                  "Vlan",                   dnx_vlan_init,                            dnx_vlan_deinit,                      NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_QOS,                   "Qos",                    dnx_qos_init,                             dnx_qos_deinit,                       NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_PTP,                   "Ptp",                    dnx_ptp_init,                             dnx_ptp_deinit,                       NULL,            0,         0,             NULL},
  {DNX_INIT_STEP_OAM,                   "OAM",                    dnx_oam_init,                             dnx_oam_deinit,                       NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_BFD,                   "BFD",                    dnx_bfd_init,                             dnx_bfd_deinit,                       NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_STAT_PP,               "Stat pp",                dnx_stat_pp_init,                         NULL,                                 NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_INSTRU,                "Instrumentation",        dnx_instru_init,                          NULL,                                 NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_REFLECTOR,             "Reflector",              dnx_switch_reflector_init,                dnx_switch_reflector_deinit,          NULL,            WB_SKIP,   0,             NULL},
  {DNX_INIT_STEP_LAST_STEP,             "Last Step",              NULL,                                     NULL,                                 NULL,            0,         0,             NULL}
};

/**
 * \brief Init-Deinit legacy soc seq.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_legacy_soc[] = {
  /*STEP_ID,                      STEP_NAME,                    STEP_INIT,                                  STEP_DEINIT,                            FLAG_CB           STEP_FLAGS     TIME_THRESH    SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_INFO_CONFIG,     "Info Config",                soc_dnx_info_config,                        soc_dnx_info_config_deinit,             NULL,             ACCESS,        0,             NULL},
  {DNX_INIT_STEP_MBCM_INIT,       "MBCM",                       mbcm_dnx_init,                              mbcm_dnx_deinit,                        NULL,             ACCESS,        0,             NULL},
  {DNX_INIT_STEP_LAST_STEP,       "Last Step",                  NULL,                                       NULL,                                   NULL,             0,             0,             NULL}
};

static const  dnx_init_step_t dnx_init_deinit_sbus_seq[] = {
  /*STEP_ID,                      STEP_NAME,                    STEP_INIT,                                  STEP_DEINIT,                            FLAG_CB           STEP_FLAGS                TIME_THRESH    SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_SCHAN,           "Schan",                      soc_dnx_schan_config_and_init,              soc_schan_deinit,                       NULL,             ACCESS,                   0,             NULL},
  {DNX_INIT_STEP_ENDIANNESS_CONFIG,"Endianness configuration",  soc_dnx_endian_config,                      NULL,                                   NULL,             ACCESS,                   0,             NULL},
  {DNX_INIT_STEP_POLLING,         "Polling",                    soc_dnx_polling_init,                       NULL,                                   NULL,             ACCESS|WB_SKIP,           0,             NULL},
  {DNX_INIT_STEP_RING_CONFIG,     "S-bus ring Config",          soc_dnx_ring_config,                        NULL,                                   NULL,             ACCESS|STANDALONE_EN,     0,             NULL},
  {DNX_INIT_STEP_SBUS_TIMEOUT,    "Sbus Timeout",               soc_dnx_init_reset_cmic_regs,               NULL,                                   NULL,             ACCESS,                   0,             NULL},
  {DNX_INIT_STEP_LAST_STEP,       "Last Step",                  NULL,                                       NULL,                                   NULL,             0,                        0,             NULL}
};

/**
 * \brief Init-Deinit access seq.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_access_seq[] = {
  /*STEP_ID,                      STEP_NAME,                    STEP_INIT,                              STEP_DEINIT,                            FLAG_CB             STEP_FLAGS                          TIME_THRESH      SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_IPROC,           "IRPOC/PAXB",                 soc_dnx_iproc_config,                   NULL,                                   NULL,               ACCESS,                             0,               NULL},
  {DNX_INIT_STEP_HARD_RESET,      "Hard reset",                 soc_dnx_hard_reset,                     NULL,                                   NULL,               ACCESS | WB_SKIP | STANDALONE_EN,   0,               NULL},
  {DNX_INIT_STEP_INTERRUPT,       "Interrupt",                  soc_dnxc_intr_init,                     soc_dnxc_intr_deinit,                   NULL,               ACCESS | STANDALONE_EN,             0,               NULL},
  {DNX_INIT_STEP_SBUS,            "SBUS",                       NULL,                                   NULL,                                   NULL,               ACCESS | STANDALONE_EN,             0,               dnx_init_deinit_sbus_seq},
#ifdef ADAPTER_SERVER_MODE
  {DNX_INIT_STEP_ADAPTER_REG_ACC,  "Adapter",                   dnx_init_adapter_reg_access_init,       dnx_init_adapter_reg_access_deinit,     NULL,               ACCESS | DISABLE_WB_CHECK,          0,               NULL},
#endif
  {DNX_INIT_STEP_SOFT_RESET,      "Soft Reset",                 soc_dnx_soft_reset,                     NULL,                                   NULL,               ACCESS | STANDALONE_EN | WB_SKIP,   0,               NULL},
  {DNX_INIT_STEP_MEM_CACHEABLE,   "Cacheable",                  soc_dnxc_mem_mark_cachable,             NULL,                                   NULL,               ACCESS,                             0,               NULL},
  {DNX_INIT_STEP_DMA,             "DMA",                        soc_dnx_dma_init,                       soc_dnx_dma_deinit,                     NULL,               ACCESS | STANDALONE_EN,             0,               NULL},
  {DNX_INIT_STEP_LAST_STEP,       "Last Step",                  NULL,                                   NULL,                                   NULL,               0,                                  0,               NULL}
};

/**
 * \brief Init-Deinit Algo seq.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_algo_seq[] = {
  /*STEP_ID,                            STEP_NAME,                          STEP_INIT,                                  STEP_DEINIT,                                    FLAG_CB             STEP_FLAGS              TIME_THRESH  SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_ALGO_CHM,              "consistent hashing manager algo",  dnx_algo_consistent_hashing_manager_init,   dnx_algo_consistent_hashing_manager_deinit,     NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_LANE_MAP,         "Algo Lane Map",                    dnx_algo_lane_map_init,                     dnx_algo_lane_map_deinit,                       NULL,               ACCESS_PORT|WB_SKIP,    0,           NULL},
  {DNX_INIT_STEP_ALGO_STAT_PP,          "Algo Stat PP",                     dnx_algo_stat_pp_init,                      dnx_algo_stat_pp_deinit,                        NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_PTP,              "Algo Ptp",                         dnx_algo_ptp_init,                          dnx_algo_ptp_deinit,                            NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_PORT,             "Algo Port",                        dnx_algo_port_init,                         dnx_algo_port_deinit,                           NULL,               ACCESS_PORT,            0,           NULL},
  {DNX_INIT_STEP_ALGO_PORT_PP,          "Algo Port PP",                     dnx_algo_port_pp_init,                      dnx_algo_port_pp_deinit,                        NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_LIF,              "Algo Lif",                         dnx_algo_lif_mngr_init,                     dnx_algo_lif_mngr_deinit,                       NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_VLAN_ALGO_RES,         "Algo Vlan Res",                    dnx_algo_vlan_res_init,                     dnx_algo_vlan_res_deinit,                       NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_STG,              "Algo Stg",                         dnx_algo_stg_init,                          dnx_algo_stg_deinit,                            NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_IN_LIF_PROFILE,   "Algo IN_LIF_PROFILE",              dnx_algo_in_lif_profile_init,               dnx_algo_in_lif_profile_deinit,                 NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_OUT_LIF_PROFILE,  "Algo OUT_LIF_PROFILE",             dnx_algo_out_lif_profile_init,              dnx_algo_out_lif_profile_deinit,                NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_L2,               "Algo L2",                          dnx_algo_l2_init,                           dnx_algo_l2_deinit,                             NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_L3,               "Algo L3",                          dnx_algo_l3_init,                           dnx_algo_l3_deinit,                             NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_MPLS,             "Algo MPLS",                        dnx_algo_mpls_init,                         dnx_algo_mpls_deinit,                           NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_VLAN,             "Algo VLAN",                        dnx_algo_vlan_init,                         dnx_algo_vlan_deinit,                           NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_RX,               "Algo Rx",                          dnx_algo_rx_init,                           dnx_algo_rx_deinit,                             NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_QOS,              "Algo QOS",                         dnx_algo_qos_init,                          dnx_algo_qos_deinit,                            NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_PPMC,             "Algo PPMC",                        dnx_algo_ppmc_init,                         dnx_algo_ppmc_deinit,                           NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_FIELD,            "Algo Field",                       dnx_algo_field_init,                        dnx_algo_field_deinit,                          NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_SAT,              "Algo SAT",                         dnx_algo_sat_init,                          dnx_algo_sat_deinit,                            NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_BFD,              "Algo BFD",                         dnx_algo_bfd_init,                          dnx_algo_bfd_deinit,                            NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_OAM,              "Algo OAM",                         dnx_algo_oam_init,                          dnx_algo_oam_deinit,                            NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_OAMP,             "Algo OAMP",                        dnx_algo_oamp_init,                         dnx_algo_oamp_deinit,                           NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_FAILOVER,         "Algo Failover",                    dnx_algo_failover_init,                     dnx_algo_failover_deinit,                       NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_TUNNEL,           "Algo Tunnel",                      dnx_algo_tunnel_init,                       dnx_algo_tunnel_deinit,                         NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_INSTRU,           "Algo Instru",                      dnx_algo_instru_init,                       dnx_algo_instru_deinit,                         NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_ALGO_SYNCE,            "Algo SyncE",                       dnx_algo_synce_init,                        dnx_algo_synce_deinit,                          NULL,               WB_SKIP,                0,           NULL},
  {DNX_INIT_STEP_LAST_STEP,             "Last Step",                        NULL,                                       NULL,                                           NULL,               WB_SKIP,                0,           NULL}
};

/**
 * \brief Init-Deinit Traffic Disable seq.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_traffic_disable_seq[] = {
  /*STEP_ID,                            STEP_NAME,                          STEP_INIT,                                  STEP_DEINIT,                     FLAG_CB             STEP_FLAGS      TIME_THRESH  SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_DISABLE_DATA_TRAFFIC,  "Disable data traffic",             dnx_stk_sys_traffic_disable,                NULL,                            NULL,               0,              0,           NULL},
  {DNX_INIT_STEP_DISABLE_CONTROL_CELLS, "Disable control cells",            dnx_fabric_control_cells_disable,           NULL,                            NULL,               0,              0,           NULL},
  {DNX_INIT_STEP_LAST_STEP,             "Last Step",                        NULL,                                       NULL,                            NULL,               0,              0,           NULL}
};

/**
 * \brief Init-Deinit Dbal seq.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_dbal_seq[] = {
  /*STEP_ID,                         STEP_NAME,     STEP_INIT,                    STEP_DEINIT,                       FLAG_CB             STEP_FLAGS     TIME_THRESH   SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_DBAL_GENERAL,       "General",      dbal_init,                  dbal_deinit,                        NULL,               ACCESS,        0,            NULL},
  {DNX_INIT_STEP_DBAL_FIELDS,        "Fields",       dbal_field_types_init,      dbal_field_types_deinit,            NULL,               ACCESS,        0,            NULL},
  {DNX_INIT_STEP_DBAL_TABLES,        "Tables",       dbal_tables_init,           dbal_tables_deinit,                 NULL,               ACCESS,        6500000,      NULL},
  {DNX_INIT_STEP_DBAL_ACCESS_MDB,    "MDB Access",   dbal_mdb_init,              dbal_mdb_deinit,                    NULL,               0,             0,            NULL},
  {DNX_INIT_STEP_DBAL_ACCESS_SW,     "SW Access",    dbal_sw_access_init,        dbal_sw_access_deinit,              NULL,               WB_SKIP,       0,            NULL},
  {DNX_INIT_STEP_DBAL_ACCESS_PEMLA,  "PEMLA Access", dbal_pemla_init,            dbal_pemla_deinit,                  NULL,               0,             0,            NULL},
  {DNX_INIT_STEP_LAST_STEP,          "Last Step",    NULL,                       NULL,                               NULL,               0,             0,            NULL}
};

/**
 * \brief Init-Deinit Utilities seq.
 *
 * For specific info on steps look for each function decription
 */
static const dnx_init_step_t dnx_init_deinit_utilities_seq[] = {
  /*STEP_ID,                          STEP_NAME,      STEP_INIT,                        STEP_DEINIT,                        FLAG_CB                     STEP_FLAGS  TIME_THRESH     SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_FEATURE,             "Feature",      dnx_init_feature_init,            dnx_init_feature_deinit,            NULL,                       ACCESS,     0,              NULL},
  {DNX_INIT_STEP_FAMILY,              "Family",       dnx_init_family_init,             dnx_init_family_deinit,             NULL,                       ACCESS,     0,              NULL},
  {DNX_INIT_STEP_MUTEXES,             "Mutexes",      soc_dnx_mutexes_init,             soc_dnx_mutexes_deinit,             NULL,                       ACCESS,     0,              NULL},
  {DNX_INIT_STEP_TIME_ANALYZER,       "Time Analyzer",dnx_init_time_analyzer_init,      dnx_init_time_analyzer_deinit,      dnx_time_analyzer_flag_cb,  0,          0,              NULL},
  {DNX_INIT_KLEAP_INFO,               "iKleap info"  ,dnx_kleap_layers_info_init,       NULL,                               NULL,                       0,          0,              NULL},
  {DNX_INIT_STEP_LAST_STEP,           "Last Step",    NULL,                             NULL,                               NULL,                       0,          0,              NULL}
};

/**
 * \brief Appl properties seq.
 *
 * Used for dynamic soc properties (i.e. properties that can be set by both API and soc property)
 * In order to avoid from code duplication, those properties should be set as last step of init seq.
 * The expected implementation will be to get the relevant values from dnx data, and to set those values
 * through BCM APIs.
 */
static const dnx_init_step_t dnx_init_deinit_appl_properties_seq[] = {
  /*STEP_ID,                           STEP_NAME,               STEP_INIT,                              STEP_DEINIT,                            FLAG_CB     STEP_FLAGS              TIME_THRESH     SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_DYN_PORT_LANE_MAP_SET,  "Setting lane map",        dnx_port_init_port_lane_map_set,    dnx_port_init_port_lane_map_deinit,     NULL,       WB_SKIP,                0,              NULL},
  {DNX_INIT_STEP_DYN_PORT_ADD,           "Adding ports",            dnx_port_init_port_add,             dnx_port_init_port_add_deinit,          NULL,       ACCESS_PORT,            1200000,        NULL},
  {DNX_INIT_STEP_DYN_PORT_PROP_SET,      "Setting port properties", dnx_port_init_port_properties_set,  dnx_port_init_port_properties_deinit,   NULL,       WB_SKIP,                3020000,        NULL},
  {DNX_INIT_STEP_VISIBILITY,             "Configure visibility",    dnx_visibility_init,                dnx_visibility_deinit,                  NULL,       0,                      0,              NULL},
  {DNX_INIT_STEP_SYNCE,                  "Configure SyncE",         dnx_synce_init,                     dnx_synce_deinit,                       NULL,       WB_SKIP,                0,              NULL},
  {DNX_INIT_STEP_LAST_STEP,              "Last Step",               NULL,                               NULL,                                   NULL,       0,                      0,              NULL}
};

/**
 * \brief Init-Deinit Utilities seq.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_pre_soft_init_seq[] = {
  /*STEP_ID,                          STEP_NAME,            STEP_INIT,                            STEP_DEINIT,                        FLAG_CB           STEP_FLAGS                          TIME_THRESH     SUB_STEP_LIST_PTR */
  {DNX_INIT_STEP_PLL,                 "PLL",                soc_dnx_pll_init,                     soc_dnx_pll_deinit,                 j2c_skip,         ACCESS | STANDALONE_EN | WB_SKIP,   0,              NULL},
  {DNX_INIT_STEP_SBUS_INTERRUPTS,     "Polled Interrupt",   soc_dnx_sbus_polled_interrupts,       NULL,                               NULL,             ACCESS | STANDALONE_EN | WB_SKIP,   0,              NULL},
  {DNX_INIT_STEP_BIST_TESTS,          "BIST tests",         soc_dnx_perform_bist_tests,           sand_deinit_cpu2tap,                NULL,             0,                                  0,              NULL},
  {DNX_INIT_STEP_DISABLE_TRAFFIC,     "Traffic Disable",    NULL,                                 NULL,                               NULL,             WB_SKIP,                            0,              dnx_init_deinit_traffic_disable_seq},
  {DNX_INIT_STEP_MEM_MASKS,           "Memories Config",    soc_dnx_unmask_mem_writes,            NULL,                               NULL,             ACCESS,                             0,              NULL},
  {DNX_INIT_STEP_BRDC_CONFIG,         "Broadcast config",   soc_dnx_drv_sbus_broadcast_config,    NULL,                               NULL,             ACCESS | WB_SKIP,                   0,              NULL},
  {DNX_INIT_STEP_SHADOW_MEMORY,       "Shadow memory",      soc_dnxc_mem_cache_init,              soc_dnxc_mem_cache_deinit,          NULL,             ACCESS,                             0,              NULL},
  {DNX_INIT_STEP_MEM_DEFAULTS,        "Memories",           dnx_init_mem_default_init,            NULL,                               NULL,             WB_SKIP,                            11000000,       NULL},
  {DNX_INIT_STEP_PEM_SEQ_INIT,        "PEM sequence",       dnx_init_pem_sequence_flow,           NULL,                               NULL,             WB_SKIP,                            0,              NULL},
  {DNX_INIT_STEP_DTQS_CONTEXTS_INIT,  "DTQs",               dnx_fabric_dtqs_contexts_init,        NULL,                               j2c_skip,         WB_SKIP,                            0,              NULL},
  {DNX_INIT_STEP_LAST_STEP,           "Last Step",          NULL,                                 NULL,                               NULL,             0,                                  0,              NULL}
};

/**
 * \brief Init-Deinit additional Threads.
 *
 * For specific info on steps look for each function decription
 */
static const  dnx_init_step_t dnx_init_deinit_thread_seq[] = {
    /*STEP_ID,                          STEP_NAME,                      STEP_INIT,                            STEP_DEINIT,                          FLAG_CB         STEP_FLAGS      TIME_THRESH     SUB_STEP_LIST_PTR */
    {DNX_INIT_STEP_TEMPERATURE_MONITOR, "Dram Temperature Monitor",     dnx_hbmc_temp_monitor_init,           dnx_hbmc_temp_monitor_deinit,         j2c_skip,       0,              0,              NULL},
    {DNX_INIT_STEP_LAST_STEP,           "Last Step",                    NULL,                                 NULL,                                 NULL,           0,              0,              NULL}
};

/**
 * \brief Init-Deinit High level seq.
 *
 * For specific info on steps look for each function description
 */
const dnx_init_step_t dnx_init_deinit_seq[] = {
  /*STEP_ID,                          STEP_NAME,              STEP_INIT,                    STEP_DEINIT,             FLAG_CB           STEP_FLAGS            TIME_THRESH   SUB_STEP_LIST_PTR */
  /*                                                              |                             ^                                                                                            */
  /*                                                             \ /                           / \                                                                                           */
  /*                                                              V                             |                                                                                            */
  {DNX_INIT_STEP_MARK_NOT_INITED,     "Soc Flags",            soc_dnx_mark_not_inited,      soc_dnx_mark_not_inited, NULL,             ACCESS,                0,            NULL},
  {DNX_INIT_STEP_LEGACY_SOC,          "Legacy Soc",           NULL,                         NULL,                    NULL,             ACCESS,                0,            dnx_init_deinit_legacy_soc},
  {DNX_INIT_STEP_DNX_DATA,            "DNX Data",             dnx_data_init,                dnx_data_deinit,         wb_test_mode_skip,ACCESS,                0,            NULL},
  {DNX_INIT_STEP_SW_STATE,            "SW State",             dnx_sw_state_init,            dnx_sw_state_deinit,     NULL,             ACCESS,                0,            NULL},
  {DNX_INIT_STEP_UTILITIES,           "Utilities",            NULL,                         NULL,                    NULL,             ACCESS,                0,            dnx_init_deinit_utilities_seq},
  {DNX_INIT_STEP_DBAL,                "DBAL",                 NULL,                         NULL,                    NULL,             ACCESS,                6800000,      dnx_init_deinit_dbal_seq},
  {DNX_INIT_STEP_ACCESS,              "Access",               NULL,                         NULL,                    NULL,             ACCESS,                0,            dnx_init_deinit_access_seq},
  {DNX_INIT_STEP_PRE_SOFT_INIT,       "Pre-Soft Init",        NULL,                         NULL,                    NULL,             ACCESS,                11000000,     dnx_init_deinit_pre_soft_init_seq},
  {DNX_INIT_STEP_SOFT_INIT,           "Soft Init",            soc_dnx_soft_init,            NULL,                    NULL,      ACCESS|WB_SKIP|STANDALONE_EN, 0,            NULL},
  {DNX_INIT_MDB,                      "MDB",                  dnx_mdb_init,                 dnx_mdb_deinit,          NULL,             0,                     1200000,      NULL},
  {DNX_INIT_STEP_ALGO,                "Algo",                 NULL,                         NULL,                    NULL,             ACCESS_PORT,           0,            dnx_init_deinit_algo_seq},
  {DNX_INIT_STEP_COMMON_MODULES,      "Common Modules",       NULL,                         NULL,                    NULL,             0,                     12020000,     dnx_init_deinit_common_seq},
  {DNX_INIT_STEP_FIELD,               "FP modules",           NULL,                         NULL,                    NULL,             0,                     3300000,      dnx_init_deinit_field_seq},
  {DNX_INIT_STEP_TM_MODULES,          "TM Modules",           NULL,                         NULL,                    NULL,             0,                     6500000,      dnx_init_deinit_tm_seq},
  {DNX_INIT_STEP_PP_MODULES,          "PP Modules",           NULL,                         NULL,                    NULL,             0,                     0,            dnx_init_deinit_pp_seq},
  {DNX_INIT_STEP_APPL_PROPERTIES,     "Appl Properties",      NULL,                         NULL,                    NULL,             ACCESS_PORT,           4200000,      dnx_init_deinit_appl_properties_seq},
  {DNX_INIT_STEP_TUNE,                "Tune",                 dnx_tune_init,                dnx_tune_deinit,         NULL,             WB_SKIP,               0,            NULL},
  {DNX_INIT_STEP_INIT_DONE,           "Init Done",            dnx_init_done_init,           dnx_init_done_deinit,    NULL,             0,                     0,            NULL},
  {DNX_INIT_STEP_THREADS,             "Threads",              NULL,                         NULL,                    NULL,             0,                     0,            dnx_init_deinit_thread_seq},
  {DNX_INIT_STEP_LAST_STEP,           "Last Step",            NULL,                         NULL,                    NULL,             0,                     0,            NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
