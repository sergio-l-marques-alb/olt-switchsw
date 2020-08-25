/** \file diag_dnx_trap_last_pkt.c
 * $Id$
 *
 * file for last packet trap diagnostics
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include "include/soc/sand/sand_signals.h"
#include "include/bcm_int/dnx/rx/rx_trap.h"
#include "include/bcm_int/dnx/rx/rx_trap_map.h"
#include "diag_dnx_trap.h"

/*
 * }
 */

/**
 * \brief
 *    Init callback function for "trap last info" diag commands
 *    1) Create and set two competeing ingress predefined trap
 *    2) Send packet that trigger the traps
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
diag_dnx_trap_last_pkt_init_cb(
    int unit)
{
    bcm_mirror_destination_t snoop_dest;
    bcm_gport_t snoop_dest_port;
    bcm_rx_trap_config_t config = { 0 };
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Configure snoop command */
    bcm_mirror_destination_t_init(&snoop_dest);
    BCM_GPORT_SYSTEM_PORT_ID_SET(snoop_dest_port ,14);
    BCM_GPORT_LOCAL_SET(snoop_dest.gport, snoop_dest_port);
    snoop_dest.flags |= BCM_MIRROR_DEST_IS_SNOOP;
    SHR_IF_ERR_EXIT(bcm_mirror_destination_create(unit,&snoop_dest));

    /** Configure trap action */
    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_LOCAL_CPU;
    config.trap_strength = 15;
    config.snoop_cmnd = BCM_GPORT_MIRROR_GET(snoop_dest.mirror_dest_id);
    config.snoop_strength = 7;
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapLinkLayerSaEqualsDa, &trap_id));
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    SHR_IF_ERR_EXIT(diag_sand_packet_send_by_case(unit, "sa_equals_da", NULL));    

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_trap_last_pkt_info_man = {
    .brief = "Show last packet trap info\n",
    .full = "User-defined trap info includes action index(HW id)\n",
    .synopsis = "[block=<IRPP | ERPP | ETPP> core=0/1/all]",
    .examples = "block=IRPP core=1",
    .init_cb = diag_dnx_trap_last_pkt_init_cb    
};

static sh_sand_enum_t core_enum_table[] = {
    {"max", 0, "max core ID", "DNX_DATA.device.general.nof_cores-1"},
    {"all", _SHR_CORE_ALL, "all cores"},
    {NULL}
};

static sh_sand_option_t dnx_trap_last_pkt_info_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {"core", SAL_FIELD_TYPE_INT32, "Core ID for multi-core devices", "all", (void *) core_enum_table},
    {NULL}
};

#define SH_DNX_TRAP_MAX_NOF_HIT_SIGNALS (13)

typedef struct
{
    bcm_rx_trap_t trap_type;
    int res_priority;
    uint32 nof_hit_signals;
    char *hit_signals_names[SH_DNX_TRAP_MAX_NOF_HIT_SIGNALS];
} sh_dnx_trap_erpp_hit_indication_t;

/* *INDENT-OFF* */

const sh_dnx_trap_erpp_hit_indication_t erpp_hit_sig_list[DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES] = {
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TDM_DISCARD] =             {bcmRxTrapEgTdmDiscard, 1, 1, {"TDM_Wrong_Port_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_INVALID_OTM] =             {bcmRxTrapEgInvalidDestPort, 2, 1, {"Invalid_OTM_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_EXCLUDE_SRC] =             {bcmRxTrapEgExcludeSrc, 5, 1, {"Exclude_Src_Int"}},
        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_DSS_STACKING] =            {bcmRxTrapDssStacking, 3, 1, {"Dss_Stacking_Int"}},
        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_LAG_MULTICAST] =           {bcmRxTrapLagMulticast, 4, 1, {"LAG_MC_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_UNACCEPTABLE_FRAME_TYPE] = {bcmRxTrapEgDiscardFrameTypeFilter, 7, 1, {"Unacceptable_Frame_Type_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_SRC_EQUAL_DEST] =          {bcmRxTrapEgHairPinFilter, 6, 1, {"Src_Eq_Dst_Int"}},
        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_UNKNOWN_DA] =              {bcmRxTrapEgUnknownDa, 17, 1, {"Unknown_DA_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_SPLIT_HORIZON] =           {bcmRxTrapEgSplitHorizonFilter, 8, 1, {"Split_Horizon_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_SCOPE] =               {bcmRxTrapEgIpmcTtlErr, 11, 1, {"TTL_Scope_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_GLEM_PP_TRAP] =            {bcmRxTrapEgGlemPpTrap, 9, 1, {"GLEM_PP_Trap_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_GLEM_NON_PP_TRAP] =        {bcmRxTrapEgGlemNonePpTrap, 10, 1, {"GLEM_NON_PP_Trap_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV4_FILTERS_ERROR] =      {bcmRxTrapEgIpv4Error, 14, 8, {"IPv4_Version_Err_Int", 
                                                                                       "IPv4_Header_Length_Err_Int", 
                                                                                       "IPv4_Total_Length_Err_Int", 
                                                                                       "IPv4_Options_Int", 
                                                                                       "IPv4_SIP_Eq_DIP_Int", 
                                                                                       "IPv4_DIP_Eq_Zero_Int", 
                                                                                       "IPv4_SIP_is_MC_Int",
                                                                                       "IPv4_Checksum_Err_Int"}},
                                                                                       
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_IPV6_FILTERS_ERROR] =      {bcmRxTrapEgIpv6Error, 15, 13, {"IPv6_Version_Err_Int", 
                                                                                        "IPv6_Unspecified_Src_Int", 
                                                                                        "IPv6_Unspecified_Dst_Int", 
                                                                                        "IPv6_SIP_is_MC_Int", 
                                                                                        "IPv6_Loopback_Int", 
                                                                                        "IPv6_Hop_by_Hop_Int", 
                                                                                        "IPv6_DIP_is_MC_Int",
                                                                                        "IPv6_Link_Local_Dst_Int", 
                                                                                        "IPv6_Link_Local_Src_Int", 
                                                                                        "IPv6_Site_Local_Dst_Int",
                                                                                        "IPv6_Site_Local_Src_Int", 
                                                                                        "IPv6_IPv4_Compatible_Dst_Int", 
                                                                                        "IPv6_IPv4_Mapped_Dst_Int"}},
                                                                                        
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_LAYER_4_FILTERS_ERROR] =   {bcmRxTrapEgL4Error, 16, 7, {"TCP_Seq_Num_and_Flags_are_Zero_Int", 
                                                                                     "TCP_Fragment_with_Incomplete_TCP_Header_Int",
                                                                                     "TCP_Seq_Num_is_Zero_and_FIN_or_URG_or_PSH_is_Set_Int", 
                                                                                     "TCP_SYN_and_FIN_are_Set_Int",
                                                                                     "TCP_Src_Port_Eq_Dst_Port_Int", 
                                                                                     "TCP_Fragment_with_Offset_Less_than_8_Int",
                                                                                     "UDP_Src_Port_Eq_Dst_Port_Int"}},
                                                                                     
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ZERO] =         {bcmRxTrapEgIpv4Ttl0, 12, 1, {"TTL_Eq_Zero_Int"}},
    
    [DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE] =          {bcmRxTrapEgIpv4Ttl1, 13, 1, {"TTL_Eq_One_Int"}},
};

/* *INDENT-ON* */

/**
 * \brief
 *    Retrives ilist of signal which match given input
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] match_m - match info of signal
 * \param [out] dsig_list - list of signals answering match criteria
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_signal_get(
    int unit,
    int core,
    match_t * match_m,
    rhlist_t * dsig_list)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(dsig_list, 0, sizeof(rhlist_t));
    dsig_list->entry_size = sizeof(signal_output_t);

    SHR_IF_ERR_EXIT(sand_signal_list_get(unit, core, match_m, dsig_list));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Retrives info of ingress resolved trap from signals
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [out] is_trap_resolved - is trap triggered in IRPP 
 * \param [out] trap_id_p - trap id (HW code)
 * \param [out] snp_code_p - snoop code
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_ingress_resolved_trap_info_get(
    int unit,
    int core,
    uint8 *is_trap_resolved,
    uint32 *trap_id_p,
    uint32 *snp_code_p)
{
    match_t trap_id_match, snp_code_match, strength_match;
    rhlist_t trap_id_dsig_list, snp_code_dsig_list, strength_dsig_list;
    signal_output_t *trap_id_signal_output = NULL, *snp_code_signal_output = NULL, *strength_signal_output = NULL;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&trap_id_match, 0, sizeof(match_t));
    trap_id_match.flags = SIGNALS_MATCH_EXPAND;
    trap_id_match.name = "trap_code";
    trap_id_match.block = "irpp";
    trap_id_match.to = "lbp";

    sal_memset(&snp_code_match, 0, sizeof(match_t));
    snp_code_match.flags = SIGNALS_MATCH_EXPAND;
    snp_code_match.name = "snoop_code";
    snp_code_match.block = "irpp";
    snp_code_match.to = "lbp";

    sal_memset(&strength_match, 0, sizeof(match_t));
    strength_match.flags = SIGNALS_MATCH_EXPAND;
    strength_match.name = "fwd_strength";
    strength_match.block = "irpp";
    strength_match.to = "lbp";

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &strength_match, &strength_dsig_list));
    strength_signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&strength_dsig_list);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &trap_id_match, &trap_id_dsig_list));
    trap_id_signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&trap_id_dsig_list);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &snp_code_match, &snp_code_dsig_list));
    snp_code_signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&snp_code_dsig_list);

    if (trap_id_signal_output == NULL)
    {
        *is_trap_resolved = FALSE;
    }
    else
    {
        *trap_id_p = trap_id_signal_output->value[0];
        *snp_code_p = snp_code_signal_output->value[0];

        if (((*trap_id_p == 0) && (strength_signal_output->value[0] == 0)) ||
            ((*trap_id_p == *snp_code_p) && (*trap_id_p == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE)))
        {
            *is_trap_resolved = FALSE;
        }
        else
        {
            *is_trap_resolved = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints info of resolved trap for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] trap_id - trap id (HW code)
 * \param [in] snp_code - snoop code (HW code) 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_ingress_resolved_trap_info_print(
    int unit,
    int core,
    uint32 trap_id,
    uint32 snp_code,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_config_t trap_fwd_config, trap_snp_config;
    bcm_rx_trap_t trap_type = bcmRxTrapCount, snoop_type = bcmRxTrapCount;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_fwd_config);
    bcm_rx_trap_config_t_init(&trap_snp_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, trap_id, &trap_type));
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_fwd_config));
    SHR_IF_ERR_EXIT(dnx_trap_action_info_irpp_print(unit, "Resolved IRPP Forward Action", trap_type, trap_id, 
                                                    &trap_fwd_config, sand_control));

    if (snp_code < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, snp_code, &snoop_type));
        SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, snp_code, &trap_snp_config));
    }

    SHR_IF_ERR_EXIT(dnx_trap_snp_action_info_irpp_print(unit, "Resolved IRPP Snoop Action", snoop_type, snp_code, 
                                                        &trap_snp_config, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] is_snoop - indication of forward/snoop action type
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */

static shr_error_e
sh_dnx_trap_last_pkt_ingress_considered_traps_info_print(
    int unit,
    int core,
    uint8 is_snoop,
    sh_sand_control_t * sand_control)
{
    uint32 cur_trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    rhlist_t code_dsig_list, strength_dsig_list;
    debug_signal_t *debug_signal;
    signal_output_t *code_signal_output, *strength_signal_output;
    match_t code_match, strength_match;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    bcm_rx_trap_t trap_type;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&code_match, 0, sizeof(match_t));
    code_match.flags = SIGNALS_MATCH_EXPAND;
    code_match.name = ((is_snoop) ? "snoop_code" : "fwd_action_cpu_trap_code");
    code_match.block = "irpp";

    sal_memset(&strength_match, 0, sizeof(match_t));
    strength_match.flags = SIGNALS_MATCH_EXPAND;
    strength_match.name = ((is_snoop) ? "snoop_strength" : "fwd_action_strength");
    strength_match.block = "irpp";

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &code_match, &code_dsig_list));

    if (is_snoop)
    {
        PRT_TITLE_SET("IRPP Considered traps for packet - Snoop");
        PRT_COLUMN_ADD("Trap type");
        PRT_COLUMN_ADD("Snoop Code");
        PRT_COLUMN_ADD("Snoop strength");
    }
    else
    {
        PRT_TITLE_SET("IRPP Considered traps for packet - Forward");
        PRT_COLUMN_ADD("Trap type");
        PRT_COLUMN_ADD("Trap id (HW code)");
        PRT_COLUMN_ADD("Forward strength");
    }

    {
        RHSAFE_ITERATOR(code_signal_output, &code_dsig_list)
        {
            debug_signal = code_signal_output->debug_signal;
            if ((code_signal_output->value[0] != cur_trap_code) &&
                (code_signal_output->value[0] < DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_ITMH_SNIF_CODE))
            {
                cur_trap_code = code_signal_output->value[0];
                SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, cur_trap_code, &trap_type));
                SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("bcmRxTrap%s", trap_name);
                PRT_CELL_SET("0x%x", cur_trap_code);

                strength_match.from = debug_signal->from;
                strength_match.to = debug_signal->to;
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &strength_match, &strength_dsig_list));
                strength_signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&strength_dsig_list);
                PRT_CELL_SET("%d", strength_signal_output->value[0]);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that shows last packet trap info.
 * \param [in] unit - unit ID
 * \param [in] core - core id 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_ingress_info_cmd(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 trap_id = 0, snp_code = 0;
    uint8 is_trap_resolved = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_resolved_trap_info_get(unit, core, &is_trap_resolved, &trap_id,
                                                                        &snp_code));

    if (is_trap_resolved)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_resolved_trap_info_print(unit, core, trap_id, snp_code,
                                                                              sand_control));

        if (trap_id != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE)
        {
            SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_considered_traps_info_print(unit, core, 0, sand_control));
        }

        if (snp_code != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE)
        {
            SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_considered_traps_info_print(unit, core, 1, sand_control));
        }
    }
    else
    {
        sal_printf("No IRPP trap was resolved\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Map hit signal to trap type
 * \param [in] unit - unit ID
 * \param [in] hit_sig_name - name of hit signal
 * \param [out] trap_type_p - trap type
 * \param [out] trap_res_priority_p - trap resolution priority 
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_hit_sig_map(
    int unit,
    char *hit_sig_name,
    bcm_rx_trap_t * trap_type_p,
    int *trap_res_priority_p)
{
    int trap_ind, trap_sig_ind;
    SHR_FUNC_INIT_VARS(unit);

    for (trap_ind = 0; trap_ind < DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES; trap_ind++)
    {
        for (trap_sig_ind = 0; trap_sig_ind < erpp_hit_sig_list[trap_ind].nof_hit_signals; trap_sig_ind++)
        {
            if (sal_strcmp(hit_sig_name, erpp_hit_sig_list[trap_ind].hit_signals_names[trap_sig_ind]) == 0)
            {
                *trap_type_p = erpp_hit_sig_list[trap_ind].trap_type;
                *trap_res_priority_p = erpp_hit_sig_list[trap_ind].res_priority;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ERPP resolved trap (forward) for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core ID 
 * \param [in] resolved_fwd_trap_type - resolved trap for forward action 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_resolved_fwd_trap_info_print(
    int unit,
    int core,
    bcm_rx_trap_t resolved_fwd_trap_type,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t trap_gport_get;
    int trap_id;
    match_t fwd_act_profile_match;
    rhlist_t dsig_list;
    signal_output_t *signal_output;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    if (resolved_fwd_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, resolved_fwd_trap_type, &trap_gport_get));
        trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport_get);

        sal_memset(&fwd_act_profile_match, 0, sizeof(match_t));
        fwd_act_profile_match.flags = SIGNALS_MATCH_EXPAND;
        fwd_act_profile_match.name = "fwd_action_profile";
        fwd_act_profile_match.block = "erpp";
        fwd_act_profile_match.to = "etmr";

        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &fwd_act_profile_match, &dsig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);

        if (signal_output->value[0] != DNX_RX_TRAP_ID_TYPE_GET(trap_id))
        {
            sal_printf("No ERPP trap forward action was resolved\n");
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

        SHR_IF_ERR_EXIT(dnx_trap_action_info_erpp_print(unit, "Resolved ", resolved_fwd_trap_type, trap_id,
                                                        &trap_config, sand_control));
    }
    else
    {
        sal_printf("No ERPP trap forward action was resolved\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ERPP resolved trap (snoop) for last packet.
 * \param [in] unit - unit ID
 * \param [in] core - core ID 
 * \param [in] resolved_snp_trap_type - resolved trap for snoop action 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_resolved_snp_trap_info_print(
    int unit,
    int core,
    bcm_rx_trap_t resolved_snp_trap_type,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t trap_gport_get;
    int snp_act_profile;
    match_t snp_act_profile_match;
    rhlist_t dsig_list;
    signal_output_t *signal_output;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (resolved_snp_trap_type != bcmRxTrapCount)
    {
        PRT_TITLE_SET("Resolved ERPP Trap Snoop Action");
        PRT_COLUMN_ADD("Attribute");
        PRT_COLUMN_ADD("Value");

        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, resolved_snp_trap_type, &trap_gport_get));
        snp_act_profile = BCM_GPORT_TRAP_GET_ID(trap_gport_get);

        sal_memset(&snp_act_profile_match, 0, sizeof(match_t));
        snp_act_profile_match.flags = SIGNALS_MATCH_EXPAND;
        snp_act_profile_match.name = "snoop_action_profile";
        snp_act_profile_match.block = "erpp";
        snp_act_profile_match.to = "etmr";

        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &snp_act_profile_match, &dsig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);

        if (signal_output->value[0] != snp_act_profile)
        {
            sal_printf("No ERPP trap snoop action was resolved\n");
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, resolved_snp_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Trap type");
        PRT_CELL_SET("bcmRxTrap%s", trap_name);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop action profile");
        PRT_CELL_SET("%d", snp_act_profile);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Snoop strength");
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport_get));

        PRT_COMMITX;
    }
    else
    {
        sal_printf("No ERPP trap snoop action was resolved\n");
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet.
 * \param [in] unit - unit ID
 * \param [in] fwd_considered_traps_list - list of considered traps for forward action 
 * \param [in] nof_fwd_considered_traps - number of considered traps for forward action  
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_considered_fwd_traps_info_print(
    int unit,
    bcm_rx_trap_t * fwd_considered_traps_list,
    int nof_fwd_considered_traps,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_t cur_trap_type;
    int trap_ind, trap_id;
    bcm_gport_t trap_gport_get;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ERPP Considered traps for packet - Forward");
    PRT_COLUMN_ADD("Trap type");
    PRT_COLUMN_ADD("Trap id");
    PRT_COLUMN_ADD("Forward action profile");
    PRT_COLUMN_ADD("Forward strength");

    for (trap_ind = 0; trap_ind < nof_fwd_considered_traps; trap_ind++)
    {
        cur_trap_type = fwd_considered_traps_list[trap_ind];

        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, cur_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("bcmRxTrap%s", trap_name);

        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, cur_trap_type, &trap_gport_get));
        trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport_get);

        PRT_CELL_SET("0x%x", trap_id);
        PRT_CELL_SET("%d", DNX_RX_TRAP_ID_TYPE_GET(trap_id));
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(trap_gport_get));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of considered traps for last packet.
 * \param [in] unit - unit ID
 * \param [in] snp_considered_traps_list - list of considered traps for snoop action 
 * \param [in] nof_snp_considered_traps - number of considered traps for snoop action  
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_considered_snp_traps_info_print(
    int unit,
    bcm_rx_trap_t * snp_considered_traps_list,
    int nof_snp_considered_traps,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_t cur_trap_type = snp_considered_traps_list[0];
    int trap_ind;
    bcm_gport_t trap_gport_get;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ERPP Considered traps for packet - Snoop");
    PRT_COLUMN_ADD("Trap type");
    PRT_COLUMN_ADD("Snoop action profile");
    PRT_COLUMN_ADD("Snoop strength");

    for (trap_ind = 0; trap_ind < nof_snp_considered_traps; trap_ind++)
    {
        cur_trap_type = snp_considered_traps_list[trap_ind];
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, cur_trap_type, &trap_gport_get));

        SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, cur_trap_type, trap_name));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("bcmRxTrap%s", trap_name);
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_ID(trap_gport_get));
        PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport_get));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ERPP last info diag
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_erpp_info_cmd(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    match_t erpp_hit_match;
    rhlist_t dsig_list;
    signal_output_t *signal_output;
    bcm_rx_trap_t cur_trap_type = bcmRxTrapCount;
    bcm_gport_t gport_get = BCM_GPORT_INVALID;
    bcm_rx_trap_t fwd_considered_traps_list[DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES] = { 0 };
    bcm_rx_trap_t snp_considered_traps_list[DBAL_NOF_ENUM_ERPP_TRAP_ID_VALUES] = { 0 };
    int nof_fwd_considered_traps = 0, nof_snp_considered_traps = 0;
    bcm_rx_trap_t resolved_fwd_trap_type = bcmRxTrapCount, resolved_snp_trap_type = bcmRxTrapCount;
    int cur_fwd_strength = 0, resolved_fwd_strength = 0;
    int cur_snp_strength = 0, resolved_snp_strength = 0;
    int cur_trap_res_priority = 0, resolved_fwd_priority = 0, resolved_snp_priority = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&erpp_hit_match, 0, sizeof(match_t));
    erpp_hit_match.flags = SIGNALS_MATCH_EXPAND;
    erpp_hit_match.name = "int";
    erpp_hit_match.block = "erpp";

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &erpp_hit_match, &dsig_list));

    {
        RHSAFE_ITERATOR(signal_output, &dsig_list)
        {
            /** check if hit indication is TRUE */
            if (signal_output->value[0] == TRUE)
            {
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_hit_sig_map(unit, signal_output->debug_signal->attribute,
                                                                      &cur_trap_type, &cur_trap_res_priority));

                SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(0, cur_trap_type, &gport_get));
                cur_fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(gport_get);
                cur_snp_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(gport_get);

                if (cur_fwd_strength != 0)
                {
                    fwd_considered_traps_list[nof_fwd_considered_traps] = cur_trap_type;
                    nof_fwd_considered_traps++;

                    if ((cur_fwd_strength > resolved_fwd_strength) ||
                        ((cur_fwd_strength > resolved_fwd_strength) && (cur_trap_res_priority > resolved_fwd_priority)))
                    {
                        resolved_fwd_trap_type = cur_trap_type;
                        resolved_fwd_strength = cur_fwd_strength;
                        resolved_fwd_priority = cur_trap_res_priority;
                    }
                }

                if (cur_snp_strength != 0)
                {
                    snp_considered_traps_list[nof_snp_considered_traps] = cur_trap_type;
                    nof_snp_considered_traps++;

                    if ((cur_snp_strength > resolved_snp_strength) ||
                        ((cur_snp_strength > resolved_snp_strength) && (cur_trap_res_priority > resolved_snp_priority)))
                    {
                        resolved_snp_trap_type = cur_trap_type;
                        resolved_snp_strength = cur_snp_strength;
                        resolved_snp_priority = cur_trap_res_priority;
                    }
                }

                /** If one of trap signals wat TRUE, move to next trap type */
                continue;
            }
        }
    }

    if ((resolved_fwd_trap_type == bcmRxTrapCount) && (resolved_snp_trap_type == bcmRxTrapCount))
    {
        sal_printf("No ERPP trap was resolved\n");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_resolved_fwd_trap_info_print(unit, core, resolved_fwd_trap_type,
                                                                           sand_control));

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_resolved_snp_trap_info_print(unit, core, resolved_snp_trap_type,
                                                                           sand_control));

    if (resolved_fwd_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_considered_fwd_traps_info_print
                        (unit, fwd_considered_traps_list, nof_fwd_considered_traps, sand_control));
    }

    if (resolved_fwd_trap_type != bcmRxTrapCount)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_considered_snp_traps_info_print
                        (unit, snp_considered_traps_list, nof_snp_considered_traps, sand_control));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Map oam trap type and id from forward action profile and oam sub-type.
 * \param [in] unit - unit ID
 * \param [in] fwd_act_profile - forward action profile 
 * \param [in] oam_sub_type - OAM sub type  
 * \param [out] oam_trap_id_p - OAM trap id encoded
 * \param [out] trap_type_p - trap type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static void
sh_dnx_trap_last_pkt_etpp_oam_trap_info_get(
    int unit,
    uint32 fwd_act_profile,
    uint8 oam_sub_type,
    int *oam_trap_id_p,
    bcm_rx_trap_t * trap_type_p)
{
    dnx_rx_trap_etpp_oam_types_e oam_trap_type;

    switch (oam_sub_type)
    {
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT:
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588:
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP:
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM:
            switch (fwd_act_profile)
            {
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_2:
                    oam_trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP;
                    *trap_type_p = bcmRxTrapEgTxOamUpMEPOamp;
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3:
                    oam_trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1;
                    *trap_type_p = bcmRxTrapEgTxOamUpMEPDest1;
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4:
                    oam_trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2;
                    *trap_type_p = bcmRxTrapEgTxOamUpMEPDest2;
                    break;
                default:
                    return;
            }
            break;
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_REFLECTOR;
            *trap_type_p = bcmRxTrapEgTxOamReflector;
            break;
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR;
            *trap_type_p = bcmRxTrapEgTxOamLevel;
            break;
        case DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR:
            oam_trap_type = DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR;
            *trap_type_p = bcmRxTrapEgTxOamPassive;
            break;
        default:
            return;
    }

    *oam_trap_id_p = DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(fwd_act_profile, oam_trap_type);
}

/**
 * \brief
 *    A diagnostics function that prints ETPP resolved trap (forward) info for last packet.
 * \param [in] unit - unit ID
 * \param [in] fwd_act_profile - forward action profile 
 * \param [in] fwd_strength - forward strength
 * \param [in] oam_sub_type - OAM sub type  
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_resolved_fwd_trap_info_print(
    int unit,
    uint32 fwd_act_profile,
    uint32 fwd_strength,
    uint8 oam_sub_type,
    sh_sand_control_t * sand_control)
{
    int oam_trap_id = -1, trap_id = -1;
    bcm_rx_trap_t trap_type = bcmRxTrapCount;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    if (fwd_act_profile == 0)
    {
        sal_printf("No ETPP trap forward action was resolved\n");
        SHR_EXIT();
    }

    if (oam_sub_type != DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL)
    {
        sh_dnx_trap_last_pkt_etpp_oam_trap_info_get(unit, fwd_act_profile, oam_sub_type, &oam_trap_id, &trap_type);
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, oam_trap_id);
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED, fwd_act_profile);
    }

    bcm_rx_trap_config_t_init(&trap_config);
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));
    trap_config.trap_strength = fwd_strength;

    SHR_IF_ERR_EXIT(dnx_trap_action_info_etpp_print(unit, "Resolved ", trap_type, trap_id, &trap_config, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ETPP resolved trap (snoop) info for last packet.
 * \param [in] unit - unit ID
 * \param [in] snp_act_profile - snoop action profile 
 * \param [in] snp_strength - snoop strength
 * \param [in] oam_sub_type - OAM sub type  
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_resolved_snp_trap_info_print(
    int unit,
    uint32 snp_act_profile,
    uint32 snp_strength,
    uint8 oam_sub_type,
    sh_sand_control_t * sand_control)
{
    char trap_name[DBAL_MAX_STRING_LENGTH];
    int oam_trap_id = -1, trap_id = -1;
    bcm_rx_trap_t trap_type = bcmRxTrapCount;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (snp_act_profile == 0)
    {
        sal_printf("No ETPP trap snoop action was resolved\n");
        SHR_EXIT();
    }

    if (oam_sub_type != DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL)
    {
        sh_dnx_trap_last_pkt_etpp_oam_trap_info_get(unit, snp_act_profile, oam_sub_type, &oam_trap_id, &trap_type);
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, oam_trap_id);
    }
    else
    {
        trap_type = bcmRxTrapEgTxUserDefine;
        trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_USER_DEFINED, snp_act_profile);
    }

    PRT_TITLE_SET("Resolved ETPP Trap Snoop Action");
    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Value");

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_type, trap_name));
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap type");
    PRT_CELL_SET("bcmRxTrap%s", trap_name);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Trap id");
    PRT_CELL_SET("0x%x", trap_id);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Snoop strength");
    PRT_CELL_SET("%d", snp_act_profile);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Snoop action profile");
    PRT_CELL_SET("%d", snp_strength);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Retrives info of ETPP resolved trap from signals
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [out] fwd_act_profile_p - forward action profile
 * \param [out] fwd_strengh_p - forward strength 
 * \param [out] snp_act_profile_p - snoop action profile
 * \param [out] snp_strengh_p - snoop strength
 * \param [out] oam_sub_type_p - OAM sub type
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_resolved_trap_info_get(
    int unit,
    int core,
    uint32 *fwd_act_profile_p,
    uint32 *fwd_strengh_p,
    uint32 *snp_act_profile_p,
    uint32 *snp_strengh_p,
    uint8 *oam_sub_type_p)
{
    match_t sig_match;
    rhlist_t dsig_list;
    signal_output_t *signal_output;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sig_match, 0, sizeof(match_t));
    sig_match.flags = SIGNALS_MATCH_EXPAND;
    sig_match.block = "etpp";
    sig_match.to = "trap";

    sig_match.name = "fwd_action_profile";
    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &sig_match, &dsig_list));
    signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);

    if (signal_output == NULL)
    {
        *fwd_act_profile_p = 0;
        *fwd_strengh_p = 0;
        *snp_act_profile_p = 0;
        *snp_strengh_p = 0;
        *oam_sub_type_p = 0;
    }
    else
    {
        *fwd_act_profile_p = signal_output->value[0];

        sig_match.name = "fwd_action_strength";
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &sig_match, &dsig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);
        *fwd_strengh_p = signal_output->value[0];

        sig_match.name = "snoop_action_profile";
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &sig_match, &dsig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);
        *snp_act_profile_p = signal_output->value[0];

        sig_match.name = "snoop_action_strength";
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &sig_match, &dsig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);
        *snp_strengh_p = signal_output->value[0];

        sig_match.name = "oam_sub_type";
        sig_match.to = "fwd";
        SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_signal_get(unit, core, &sig_match, &dsig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_first(&dsig_list);
        *oam_sub_type_p = signal_output->value[0];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints ETPP last info diag
 * \param [in] unit - unit ID
 * \param [in] core - core id
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_etpp_info_cmd(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 fwd_act_profile = 0, fwd_strength = 0, snp_act_profile = 0, snp_strength = 0;
    uint8 oam_sub_type = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_resolved_trap_info_get(unit, core, &fwd_act_profile, &fwd_strength,
                                                                     &snp_act_profile, &snp_strength, &oam_sub_type));

    if ((fwd_act_profile == 0) && (snp_act_profile == 0))
    {
        sal_printf("No ETPP trap was resolved\n");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_resolved_fwd_trap_info_print(unit, fwd_act_profile, fwd_strength,
                                                                           oam_sub_type, sand_control));

    SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_resolved_snp_trap_info_print(unit, snp_act_profile, snp_strength,
                                                                           oam_sub_type, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that shows user-defined trap info.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_last_pkt_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_block_e trap_block;
    int core, start_core, nof_cores;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("block", trap_block);
    SH_SAND_GET_INT32("core", core);

    if (core == BCM_CORE_ALL)
    {
        start_core = 0;
        nof_cores = dnx_data_device.general.nof_cores_get(unit);
    }
    else
    {
        start_core = core;
        nof_cores = 1;
    }

    for (core = start_core; core < (start_core + nof_cores); core++)
    {
        sal_printf("\nCore %d:", core);
        sal_printf("\n-------\n");

        switch (trap_block)
        {
            case DNX_RX_TRAP_BLOCK_INGRESS:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_info_cmd(unit, core, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_ERPP:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_info_cmd(unit, core, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_ETPP:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_info_cmd(unit, core, sand_control));
                break;
            case DNX_RX_TRAP_BLOCK_NUM_OF:
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_ingress_info_cmd(unit, core, sand_control));
                sal_printf("\n");
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_erpp_info_cmd(unit, core, sand_control));
                sal_printf("\n");
                SHR_IF_ERR_EXIT(sh_dnx_trap_last_pkt_etpp_info_cmd(unit, core, sand_control));
                break;
            default:
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }

        sal_printf("\n");
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_last_pkt_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"INFo", sh_dnx_trap_last_pkt_info_cmd, NULL, dnx_trap_last_pkt_info_options, &dnx_trap_last_pkt_info_man}
    ,
    {NULL}
};
