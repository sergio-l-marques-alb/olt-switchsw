/** \file appl_ref_init_deinit.c
 * init and deinit functions to be used by the INIT_DNX command.
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/init.h>
#include <bcm/cosq.h>
#include <bcm/stack.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/drv.h>

#include <appl/reference/dnxc/appl_ref_init_deinit.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <appl/reference/dnx/appl_ref_init_deinit.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_sniff_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/reference/dnx/appl_ref_dynamic_port.h>
#include "appl_ref_mtu_compressed_layer_type.h"
#include "appl_ref_oam_init.h"
#include "appl_ref_l2_init.h"
#include "appl_ref_vlan_init.h"
#include "appl_ref_qos_init.h"
#include "appl_ref_rx_trap_init.h"
#include "appl_ref_dram_init.h"
#include "appl_ref_compensation_init.h"
#include "appl_ref_sys_device.h"
#include "appl_ref_kbp_init.h"
#include "appl_ref_linkscan_init.h"
#include <appl/reference/dnx/appl_ref_itmh_init.h>
#include "appl_ref_field_j1_itmh_init.h"
#include "appl_ref_field_j1_itmh_pph_init.h"
#include "appl_ref_field_itmh_pph_init.h"
#include "appl_ref_field_stacking_init.h"
#include "appl_ref_srv6_field_init_deinit.h"
#include "appl_ref_field_wa_init.h"
#include <appl/reference/dnxc/appl_ref_intr.h>

/*
 * }
 */
/** callback function which skips J2C */
shr_error_e
j2c_skip(
    int unit,
    int *dynamic_flags)
{
    if (SOC_IS_J2C(unit))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    else
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

static shr_error_e
j2b0_j2c_skip(
    int unit,
    int *dynamic_flags)
{
    if (SOC_IS_J2C(unit) || SOC_IS_JERICHO_2_B0(unit))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    else
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

shr_error_e
j2b0_skip(
    int unit,
    int *dynamic_flags)
{
    if (SOC_IS_JERICHO_2_B0(unit))
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }
    else
    {
        *dynamic_flags = 0;
    }
    return _SHR_E_NONE;
}

/*
 * Typedefs.
 * {  
 */

/*
 * }
 */

/*
 * Globals.
 * {  
 */

/*
 * }
 */


/* *INDENT-OFF* */

/**
 * \brief DNX reference applications sequence 
 *        * Each reference application in the init sequence should be added here.
 *        * Each application can be enabled or disabled using a dedicated.
 *          soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'.
 *        * Additional information can be passed to the applications by using a dedicated
 *          soc property: 'appl_param_<suffix>=val'.
 *        * The last step must be all NULLs./
 */
static const appl_dnxc_init_step_t appl_dnx_ref_apps_steps[] = {
    /*STEP_ID                      NAME                      SUFFIX             INIT_FUNC                      DEINIT_FUNC                FLAG_CB         STEP_FLAGS                TIME_THRESH   SUB_LIST*/
    {APPL_DNX_STEP_SYS_PARAMS,    "System Params",          "sys_params",       appl_dnx_sys_params_parse,     NULL,                      NULL,           STANDALONE,               0,            NULL},
    {APPL_DNX_STEP_SYS_DEVICE,    "System Device",          "sys_device",       appl_dnx_sys_device_init,      NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_SYS_PORTS,     "System Ports",           "sys_ports" ,       appl_dnx_sys_ports_init,       NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_SNIFF_PORTS,   "Sniff Ports",            "sniff_ports",      appl_dnx_sniff_ports_init,     NULL,                      j2c_skip,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_PP_ETH_PROCESSING,"Port initial settings","dyn_ports" ,      appl_dnx_dyn_ports_packet_processing_eth_init,  NULL,     NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_LINKSCAN,      "Linkscan",               "linkscan" ,        appl_dnx_linkscan_init,        NULL,                      j2c_skip,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
#if defined(INCLUDE_KBP)
    {APPL_DNX_STEP_KBP,           "KBP",                    "kbp",              appl_dnx_kbp_init,             appl_dnx_kbp_deinit,       j2c_skip,       DEF_PROP_EN,              0,            NULL},
#endif
    {APPL_DNX_STEP_E2E_SCHEME,    "E2E Scheme",             "e2e_scheme",       appl_dnx_e2e_scheme_init,      NULL,                      j2c_skip,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_L2,            "L2",                     "l2",               appl_dnx_l2_init,              NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_VLAN,          "Vlan",                   "vlan",             appl_dnx_vlan_init,            NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_COMPRESSED_LAYER_TYPE,"Compressed layer type", "clt",        appl_dnx_mtu_compressed_layer_type_init, NULL,            j2c_skip,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_OAM,           "Oam",                    "oam",              appl_dnx_oam_init,             NULL,                      j2c_skip,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_QOS,           "QOS",                    "qos",              appl_dnx_qos_init,             NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_RX_TRAP,       "RX Trap",                "rx_trap",          appl_dnx_rx_trap_init,         NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},    
    {APPL_DNX_STEP_ITMH,          "Field ITMH",             "field_itmh",       appl_dnx_field_itmh_init,      NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_ITMH_PPH,      "Field ITMH_PPH",         "field_itmh_pph",   appl_dnx_field_itmh_pph_init,  NULL,                      NULL,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_J1_ITMH,       "Field J1 ITMH",          "field_j1_itmh",    appl_dnx_field_j1_itmh_init,   NULL,                      appl_dnx_field_j1_itmh_cb, DEF_PROP_EN | WB_SKIP,    0, NULL},
    {APPL_DNX_STEP_J1_ITMH_PPH,   "Field J1 ITMH_PPH",      "field_j1_itmh_pph",appl_dnx_field_j1_itmh_pph_init,  NULL,                   appl_dnx_field_j1_itmh_pph_cb,           DEF_PROP_EN | WB_SKIP,    0,            NULL},
    
    {APPL_DNX_STEP_STACKING,      "Field Stacking",         "field_stacking",   appl_dnx_field_stacking_init,  NULL,                      j2b0_j2c_skip,  DEF_PROP_DIS | WB_SKIP,   0,            NULL},
    {APPL_DNX_STEP_SRV6,          "Field SRV6",             "field_srv6",       appl_dnx_field_srv6_init,      NULL,                      j2c_skip,       DEF_PROP_DIS | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_DRAM,          "Dram cb registration",   "dram_cb_reg",      appl_dnx_dram_cb_register,     NULL,                      j2c_skip,       DEF_PROP_EN,              0,            NULL}, /* this step should run after WB */
    {APPL_DNX_STEP_VISIBILITY,    "Visibility Config",      "visibility",       appl_dnx_sys_vis_enable,       NULL,                      NULL,           DEF_PROP_EN,              0,            NULL},
    {APPL_DNX_STEP_COMPENSATION,  "Compensation",           "compensation",     appl_dnx_compensation_init,    NULL,                      j2c_skip,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_TRAFFIC_EN,    "Traffic Enable",         "traffic",          appl_dnx_sys_traffic_enable,   NULL,                      NULL,           DEF_PROP_EN,              0,            NULL}, /* this step must be last */
    {APPL_DNX_STEP_INTERRUPT,     "Interrupt Init",         "intr_init",        appl_dnxc_interrupt_init,      appl_dnxc_interrupt_deinit,j2c_skip,       DEF_PROP_EN,              0,            NULL}, /* this step must be after FabricControlCellsEnable */
    {APPL_DNX_STEP_LAST_STEP,     NULL,                     NULL,               NULL,                          NULL,                      NULL,           0,                        0,            NULL} /* Last step must be all NULLs*/

};




/**
 * \brief DNX reference Field WA applications sequence
 *        * Each refernce application in the init sequence should be added here.
 *        * Each application can be enabled or disabled using a dedicated.
 *          soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'.
 *        * Additional information can be passed to the applications by using a dedicated
 *          soc property: 'appl_param_<suffix>=val'.
 *        * The last step must be all NULLs.
 */
static const appl_dnxc_init_step_t appl_dnx_ref_field_wa_apps_steps[] = {
    /*STEP_ID                      NAME                      SUFFIX                 INIT_FUNC                           DEINIT_FUNC         FLAG_CB    STEP_FLAGS                         TIME_THRESH   SUB_LIST*/
    {APPL_DNX_STEP_FP_WA_FEC_DEST,      "Field FEC Dest WA",      "fec_dest_wa_init", appl_dnx_field_wa_fec_dest_init,    NULL,              appl_dnx_field_wa_fec_dest_cb,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_FP_WA_OAM_INDEX,     "Field OAM FWD Index WA", "oam_fwd_idx_wa_init", appl_dnx_field_wa_oam_layer_index_init, NULL,          appl_dnx_field_wa_oam_layer_index_cb,DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_FP_WA_TRAP_L4,       "Field L4 Trap WA",       "L4_trap_wa_init",  appl_dnx_field_wa_trap_l4_init,     NULL,              appl_dnx_field_wa_trap_l4_cb,        DEF_PROP_DIS| WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_FP_WA_OAM_STAT,      "Field OAM Stat WA",      "oam_stat_wa_init", appl_dnx_field_wa_oam_stat_init,    NULL,              appl_dnx_field_wa_oam_stat_cb,       DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_FP_WA_FLOW_ID,       "Field Flow Id WA",       "flow_id_wa_init",  appl_dnx_field_wa_flow_id_init,     NULL,              appl_dnx_field_wa_flow_id_cb,        DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_FP_WA_ROO,           "Field ROO WA",           "roo_wa_init",      appl_dnx_field_wa_roo_init,         NULL,              appl_dnx_field_wa_roo_cb,            DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_FP_WA_J1_SAME_PORT,  "Field J1 Same port WA",  "j1_same_p_init",   appl_dnx_field_wa_j1_same_port_init,NULL,              appl_dnx_field_wa_j1_same_port_cb,   DEF_PROP_EN | WB_SKIP,    0,            NULL},
	{APPL_DNX_STEP_FP_WA_J1_LEARNING,   "Field J1 Lear WA",       "j1_learn_init",     appl_dnx_field_j1_learning_wa_init, NULL,              appl_dnx_field_wa_j1_learning_cb,    DEF_PROP_EN | WB_SKIP,    0,            NULL},
    {APPL_DNX_STEP_LAST_STEP,               NULL,                     NULL,               NULL,                               NULL,                    NULL,                           0,                       0,           NULL} /* Last step must be all NULLs*/

};


/**
 * \brief DNX init sequence 
 *        * Each step in the init sequence should be added here
 *        * The init sequence has 2 main phases:
 *          1. Initialize SDK (create unit and init bcm)
 *          2. Run default reference applications - each application can be enabled or disabled using a dedicated
 *             soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'
 *        * The last step must be all NULLs
 */
static const appl_dnxc_init_step_t appl_dnx_init_steps[] = {
    /*STEP_ID                   NAME                     SUFFIX          INIT_FUNC                DEINIT_FUNC                 FLAGS_CB   STEP_FLAGS             TIME_THRESH   SUB_LIST*/
    {APPL_DNX_STEP_SDK,         "SDK",                   "",             appl_dnxc_sdk_init,      appl_dnxc_sdk_deinit,       NULL,      STANDALONE | ACCESS,   0,            NULL},
    {APPL_DNX_STEP_APPS,        "Applications",          "ref",          NULL,                    NULL,                       NULL,      DEF_PROP_EN,           0,            appl_dnx_ref_apps_steps},
    {APPL_DNX_STEP_FP_WA_APPS,  "Field WA Applications", "field_wa",     NULL,                    NULL,                       j2c_skip,  DEF_PROP_EN,           0,            appl_dnx_ref_field_wa_apps_steps},
    {APPL_DNX_STEP_INIT_DONE,   "Init Done",             "init_done",    appl_dnx_init_done_init, appl_dnx_init_done_deinit,  j2c_skip,  DEF_PROP_EN,           0,            NULL}, /* this step must be last */
    {APPL_DNX_STEP_LAST_STEP,    NULL,                    NULL,          NULL,                    NULL,                       NULL,      0,                     0,            NULL}  /* Last step must be all NULLs*/
};

/* *INDENT-ON* */

/*
 * See .h file
 */
shr_error_e
appl_dnx_step_list_get(
    int unit,
    const appl_dnxc_init_step_t ** appl_steps)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * return pointer to the list of DNX application init steps 
     */
    *appl_steps = appl_dnx_init_steps;

    SHR_FUNC_EXIT;
}
