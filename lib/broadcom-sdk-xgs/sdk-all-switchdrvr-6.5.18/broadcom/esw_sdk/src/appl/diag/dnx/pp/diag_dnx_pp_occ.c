/** \file diag_dnx_pp_occ.c
 *
 * Occupation Management Diagnostic Command - Retrieves all allocated in and outlif profile resources.
 */

/*************
 * INCLUDES  *
 *************/
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/lif/in_lif_profile.h>
/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/**
 * \brief
 *    A diagnostics function that prints the MDB cluster allocation per hierarchy
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_pp_occ_etpp_profile_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, out_lif_profile, out_lif_orientation, split_horizon_enable, oam_priority_map_profile,
        outlif_profile_width;
    int is_end;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_end = 0;
    out_lif_profile = 0;
    out_lif_orientation = 0;
    split_horizon_enable = 0;
    oam_priority_map_profile = 0;

    PRT_TITLE_SET("ETPP Outlif Profile allocation");
    outlif_profile_width = utilex_log2_round_up(dnx_data_lif.out_lif.nof_out_lif_profiles_get(unit));
    PRT_INFO_ADD("Outlif profile width = %d", outlif_profile_width);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Outlif Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Outlif Orientation");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Split Horizon");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "OAM Priority Map Profile");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, &out_lif_profile));
        PRT_CELL_SET("%d", out_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, INST_SINGLE, &out_lif_orientation));
        PRT_CELL_SET("%d", out_lif_orientation);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, &split_horizon_enable));
        PRT_CELL_SET("%d", split_horizon_enable);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, INST_SINGLE,
                         &oam_priority_map_profile));
        PRT_CELL_SET("%d", oam_priority_map_profile);
        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief The function fills 3 arays of acl, ingress and egress bits with the 'x' character,
 * used as helper to empasize on the relevant bits for the diagnostic.
 */
static void
sh_dnx_pp_occ_ingress_profile_masked_bits_print(
    int unit,
    char *acl_bits,
    char *egress_bits,
    char *ingress_bits)
{
    int bit;
    int nof_ingress_profile_bits;
    int nof_egress_profile_bits;
    int system_headers_mode;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /** Print egress bits information from inlif profile*/
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        nof_ingress_profile_bits = NOF_IN_LIF_PROFILE_INGRESS_BITS;
        nof_egress_profile_bits = IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS;
    }
    else
    {
        nof_ingress_profile_bits =
            (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS);
        nof_egress_profile_bits = IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
    }
    for (bit = 0; bit < ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS + 1; bit++)
    {
        if (bit < ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS)
        {
            acl_bits[bit] = 'x';
        }
        else
        {
            acl_bits[bit] = '\0';
        }
    }
    for (bit = 0; bit < nof_ingress_profile_bits + 1; bit++)
    {
        if (bit < nof_ingress_profile_bits)
        {
            ingress_bits[bit] = 'x';
        }
        else
        {
            ingress_bits[bit] = '\0';
        }
    }
    for (bit = 0; bit < nof_egress_profile_bits + 1; bit++)
    {
        if (bit < nof_egress_profile_bits)
        {
            egress_bits[bit] = 'x';
        }
        else
        {
            egress_bits[bit] = '\0';
        }
    }
}

/**
 * \brief The function prints the egress bits from the inlif profile
 */
static shr_error_e
sh_dnx_pp_occ_ingress_profile_egress_bits_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, in_lif_profile, in_lif_orientation, inner_dp_profile,
        outer_dp_profile, in_lif_same_interface;
    int is_end, egress_msb_bit, nof_ingress_bits;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    char *acl_bits_p = NULL;
    char *ingress_bits_p = NULL;
    char *egress_bits_p = NULL;
    char prefix_value_binary_buffer[6];
    int saved_index, default_entry;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_ALLOC(acl_bits_p, sizeof(*acl_bits_p) * (ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS + 1),
              "acl_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(ingress_bits_p, sizeof(*ingress_bits_p) * (NOF_IN_LIF_PROFILE_INGRESS_BITS + 1),
              "ingress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egress_bits_p, sizeof(*egress_bits_p) * 2, "egress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    is_end = 0;
    in_lif_profile = 0;
    in_lif_orientation = 0;
    inner_dp_profile = 0;
    outer_dp_profile = 0;
    in_lif_same_interface = 0;
    egress_msb_bit = IN_LIF_PROFILE_PMF_RESERVED_START_BIT;
    nof_ingress_bits = NOF_IN_LIF_PROFILE_INGRESS_BITS;
    saved_index = 0;
    default_entry = 0;

    sh_dnx_pp_occ_ingress_profile_masked_bits_print(unit, acl_bits_p, egress_bits_p, ingress_bits_p);
    PRT_TITLE_SET("InLIF Ingress-PP profile");
    PRT_INFO_ADD
        ("Profile bits separation: ACL profile bits = [%d : %d], Egress lif profile bits = [%d : %d], number of Ingress lif/vsi profile bits = [%d : %d]",
         NOF_IN_LIF_PROFILE_BITS, IN_LIF_PROFILE_PMF_RESERVED_START_BIT, IN_LIF_PROFILE_PMF_RESERVED_START_BIT - 1,
         IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT, IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT - 1, 0);
    PRT_COMMITX;
    PRT_TITLE_SET("Egress bits");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "InLif Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Orientation");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Inner DP");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Outer DP");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Same interface");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &in_lif_profile));
        in_lif_profile = in_lif_profile & ((1 << egress_msb_bit) - 1);
        in_lif_profile = in_lif_profile >> nof_ingress_bits;
        if ((saved_index < in_lif_profile) || ((default_entry == 0) && (in_lif_profile == 0)))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            sal_itoa(prefix_value_binary_buffer, in_lif_profile, 2, 0, 0);
            PRT_CELL_SET("%s%s%s", acl_bits_p, prefix_value_binary_buffer, ingress_bits_p);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF_ORIENTATION, INST_SINGLE, &in_lif_orientation));
            PRT_CELL_SET("%d", in_lif_orientation);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_INNER_DP_PROFILE, INST_SINGLE, &inner_dp_profile));
            PRT_CELL_SET("%d", inner_dp_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUTER_DP_PROFILE, INST_SINGLE, &outer_dp_profile));
            PRT_CELL_SET("%d", outer_dp_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE, INST_SINGLE,
                             &in_lif_same_interface));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                            (unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, DBAL_FIELD_IN_LIF_SAME_INTERFACE,
                             &in_lif_same_interface, NULL, 0, FALSE, buffer));
            PRT_CELL_SET("%s", buffer);
            /** Receive next entry in table.*/
            saved_index = in_lif_profile;
            if (in_lif_profile == 0)
            {
                default_entry = 1;
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    PRT_COMMITX;

exit:
    SHR_FREE(acl_bits_p);
    SHR_FREE(ingress_bits_p);
    SHR_FREE(egress_bits_p);
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief The function prints the egress bits from the inlif profile
 */
static shr_error_e
sh_dnx_pp_occ_ingress_profile_jr_mode_egress_bits_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, in_lif_profile, egress_reserved_start_bit;
    int is_end, egress_msb_bit, entries_to_print, nof_ingress_bits, in_lif_profile_jr_mode,
        in_lif_profile_exclude_orientation, same_interface, orientation, in_lif_orientation;
    char *acl_bits_p = NULL;
    char *ingress_bits_p = NULL;
    char *egress_bits_p = NULL;
    int saved_index, default_entry;
    char prefix_value_binary_buffer[6];
    in_lif_profile_info_egress_t egress_fields_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_ALLOC(acl_bits_p, sizeof(*acl_bits_p) * (ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS + 1),
              "acl_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(ingress_bits_p, sizeof(*ingress_bits_p) * (NOF_IN_LIF_PROFILE_INGRESS_BITS + 1),
              "ingress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egress_bits_p, sizeof(*egress_bits_p) * IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE,
              "egress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    is_end = 0;
    in_lif_profile = 0;
    in_lif_orientation = 0;
    egress_msb_bit = IN_LIF_PROFILE_PMF_RESERVED_START_BIT;
    nof_ingress_bits =
        (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS);
    saved_index = 0;
    default_entry = 0;
    entries_to_print = 0;
    in_lif_profile_jr_mode = 0;
    in_lif_profile_exclude_orientation = 0;
    egress_reserved_start_bit = IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
    same_interface = dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    orientation = dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get(unit);
    sal_memset(&egress_fields_info, 0, sizeof(in_lif_profile_info_egress_t));

    sh_dnx_pp_occ_ingress_profile_masked_bits_print(unit, acl_bits_p, egress_bits_p, ingress_bits_p);
    PRT_TITLE_SET("InLIF Ingress-PP profile");
    PRT_INFO_ADD
        ("Profile bits separation: ACL profile bits = [%d : %d], Egress lif profile bits = [%d : %d], number of Ingress lif/vsi profile bits = [%d : %d]",
         NOF_IN_LIF_PROFILE_BITS, IN_LIF_PROFILE_PMF_RESERVED_START_BIT, IN_LIF_PROFILE_PMF_RESERVED_START_BIT - 1,
         IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE,
         IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE - 1, 0);
    PRT_COMMITX;
    PRT_TITLE_SET("Egress bits");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "InLif Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Orientation");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "System Scope same interface");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Device Scope same interface");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &in_lif_profile));
        in_lif_profile = in_lif_profile & ((1 << egress_msb_bit) - 1);
        in_lif_profile = in_lif_profile >> nof_ingress_bits;
        if ((saved_index < in_lif_profile) || ((default_entry == 0) && (in_lif_profile == 0)))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            sal_itoa(prefix_value_binary_buffer, in_lif_profile, 2, 0, 0);
            PRT_CELL_SET("%s%s%s", acl_bits_p, prefix_value_binary_buffer, ingress_bits_p);

            SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_extract_egress_profile
                            (unit, in_lif_profile << egress_reserved_start_bit, &egress_fields_info,
                             &in_lif_profile_jr_mode, &in_lif_profile_exclude_orientation, &in_lif_orientation, LIF));
            if ((orientation > 0) && (same_interface == 0))
            {
                PRT_CELL_SET("%d", egress_fields_info.in_lif_orientation);
                PRT_CELL_SET("%d", 0);
                PRT_CELL_SET("%d", 0);
            }
            else if (same_interface == JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
            {
                PRT_CELL_SET("%d", egress_fields_info.in_lif_orientation);
                PRT_CELL_SET("%d", egress_fields_info.lif_same_interface_mode);
                PRT_CELL_SET("%d", 0);
                if ((orientation == 0) && (entries_to_print == 1))
                {
                    break;
                }
                entries_to_print = entries_to_print + 1;
            }
            else if (same_interface == JR_COMP_MODE_ONLY_DEV_SCOPE_ENABLED_BY_SOC_PROPERTIES)
            {
                PRT_CELL_SET("%d", egress_fields_info.in_lif_orientation);
                PRT_CELL_SET("%d", 0);
                PRT_CELL_SET("%d", egress_fields_info.lif_same_interface_mode);
                if ((orientation == 0) && (entries_to_print == 1))
                {
                    break;
                }
                entries_to_print = entries_to_print + 1;
            }
            else if (same_interface == JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES)
            {
                PRT_CELL_SET("%d", 0);
                PRT_CELL_SET("%d", ((egress_fields_info.lif_same_interface_mode >> 1) & 1));
                PRT_CELL_SET("%d", (egress_fields_info.lif_same_interface_mode & 1));
            }
            /** Receive next entry in table.*/
            saved_index = in_lif_profile;
            if (in_lif_profile == 0)
            {
                default_entry = 1;
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    PRT_COMMITX;

exit:
    SHR_FREE(acl_bits_p);
    SHR_FREE(ingress_bits_p);
    SHR_FREE(egress_bits_p);
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief The function prints the ingress bits from the inlif profile
 */
static shr_error_e
sh_dnx_pp_occ_ingress_profile_ingress_bits_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, rpf_mode, in_lif_profile, icmpv4_trap_handling_profile,
        icmpv6_trap_handling_profile, arp_trap_handling_profile, igmp_trap_handling_profile,
        non_auth_8021x_trap_handling_profile, cs_in_lif_vtt_profile, cs_in_lif_fwd_profile,
        da_not_found_destination_profile, dhcp_trap_handling_profile;
    int is_end, nof_ingress_bits, system_headers_mode;
    char prefix_value_binary_buffer[6];
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    char *acl_bits_p = NULL;
    char *ingress_bits_p = NULL;
    char *egress_bits_p = NULL;
    int saved_index, default_entry;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_ALLOC(acl_bits_p, sizeof(*acl_bits_p) * (ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS + 1),
              "acl_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(ingress_bits_p, sizeof(*ingress_bits_p) * (NOF_IN_LIF_PROFILE_INGRESS_BITS + 1),
              "ingress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /** Print egress bits information from inlif profile*/
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        SHR_ALLOC(egress_bits_p, sizeof(*egress_bits_p) * (IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS + 1),
                  "egress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        nof_ingress_bits = NOF_IN_LIF_PROFILE_INGRESS_BITS;
    }
    else
    {
        SHR_ALLOC(egress_bits_p, sizeof(*egress_bits_p) * IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE,
                  "egress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        nof_ingress_bits =
            (NOF_IN_LIF_PROFILE_BITS - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE - IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS);
    }
    is_end = 0;
    rpf_mode = 0;
    in_lif_profile = 0;
    icmpv4_trap_handling_profile = 0;
    icmpv6_trap_handling_profile = 0;
    arp_trap_handling_profile = 0;
    igmp_trap_handling_profile = 0;
    non_auth_8021x_trap_handling_profile = 0;
    cs_in_lif_vtt_profile = 0;
    cs_in_lif_fwd_profile = 0;
    da_not_found_destination_profile = 0;
    saved_index = 0;

    sh_dnx_pp_occ_ingress_profile_masked_bits_print(unit, acl_bits_p, egress_bits_p, ingress_bits_p);
    PRT_TITLE_SET("Ingress bits");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "InLif Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "RPF mode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ICMPV4 profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ICMPV6 profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ARP Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "IGMP Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DHCP Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Non auth 802tx Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CS InLif VTT Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CS InLif FWD Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DA not found destination Profile");
    saved_index = 0;
    default_entry = 0;

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &in_lif_profile));
        in_lif_profile = in_lif_profile & ((1 << nof_ingress_bits) - 1);

        if ((saved_index < in_lif_profile) || ((default_entry == 0) && (in_lif_profile == 0)))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            sal_itoa(prefix_value_binary_buffer, in_lif_profile, 2, 0, 0);
            PRT_CELL_SET("%s%s%s", acl_bits_p, egress_bits_p, prefix_value_binary_buffer);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_RPF_MODE, INST_SINGLE, &rpf_mode));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                            (unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, DBAL_FIELD_RPF_MODE, &rpf_mode, NULL, 0,
                             FALSE, buffer));
            PRT_CELL_SET("%s", buffer);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ICMPV4_TRAP_HANDLING_PROFILE, INST_SINGLE,
                             &icmpv4_trap_handling_profile));
            PRT_CELL_SET("%d", icmpv4_trap_handling_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ICMPV6_TRAP_HANDLING_PROFILE, INST_SINGLE,
                             &icmpv6_trap_handling_profile));
            PRT_CELL_SET("%d", icmpv6_trap_handling_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ARP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                             &arp_trap_handling_profile));
            PRT_CELL_SET("%d", arp_trap_handling_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IGMP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                             &igmp_trap_handling_profile));
            PRT_CELL_SET("%d", igmp_trap_handling_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_DHCP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                             &dhcp_trap_handling_profile));
            PRT_CELL_SET("%d", dhcp_trap_handling_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NON_AUTH_8021X_TRAP_HANDLING_PROFILE, INST_SINGLE,
                             &non_auth_8021x_trap_handling_profile));
            PRT_CELL_SET("%d", non_auth_8021x_trap_handling_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CS_IN_LIF_VTT_PROFILE, INST_SINGLE,
                             &cs_in_lif_vtt_profile));
            PRT_CELL_SET("%d", cs_in_lif_vtt_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CS_IN_LIF_FWD_PROFILE, INST_SINGLE,
                             &cs_in_lif_fwd_profile));
            PRT_CELL_SET("%d", cs_in_lif_fwd_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE, INST_SINGLE,
                             &da_not_found_destination_profile));
            PRT_CELL_SET("%d", da_not_found_destination_profile);
            /** Receive next entry in table.*/
            saved_index = in_lif_profile;
            if (in_lif_profile == 0)
            {
                default_entry = 1;
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    PRT_COMMITX;
exit:
    SHR_FREE(acl_bits_p);
    SHR_FREE(ingress_bits_p);
    SHR_FREE(egress_bits_p);
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    A diagnostics function that prints the inlif profile allocated resources
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_pp_occ_inlif_profile_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /** Print egress bits information from inlif profile*/
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_occ_ingress_profile_egress_bits_print(unit, args, sand_control));
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_occ_ingress_profile_jr_mode_egress_bits_print(unit, args, sand_control));
    }
    /** Print ingress bits information from inlif profile*/
    SHR_IF_ERR_EXIT(sh_dnx_pp_occ_ingress_profile_ingress_bits_print(unit, args, sand_control));

exit:

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints the vsi profile allocated resources
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_pp_occ_vsi_profile_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, routing_enablers_profile, vsi_profile, ivl_svl_mode, fwd_type, nof_ingress_bits;
    int is_end, saved_profile, default_entry;
    char prefix_value_binary_buffer[6];

    char *acl_bits_p = NULL;
    char *ingress_bits_p = NULL;
    char *egress_bits_p = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_ALLOC(acl_bits_p, sizeof(*acl_bits_p) * (ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS + 1),
              "acl_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(ingress_bits_p, sizeof(*ingress_bits_p) * (NOF_IN_LIF_PROFILE_INGRESS_BITS + 1),
              "ingress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(egress_bits_p, sizeof(*egress_bits_p) * (IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS + 1),
              "egress_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    is_end = 0;
    routing_enablers_profile = 0;
    vsi_profile = 0;
    ivl_svl_mode = 0;
    fwd_type = 0;
    saved_profile = 0;
    nof_ingress_bits = NOF_IN_LIF_PROFILE_INGRESS_BITS;
    default_entry = 0;

    sh_dnx_pp_occ_ingress_profile_masked_bits_print(unit, acl_bits_p, egress_bits_p, ingress_bits_p);
    PRT_TITLE_SET("Ingress-PP VSI profile table");
    PRT_INFO_ADD
        ("Profile bits separation: ACL profile bits = [%d : %d], Egress lif profile bits = [%d : %d], number of Ingress lif/vsi profile bits = [%d : %d]",
         NOF_IN_LIF_PROFILE_BITS, IN_LIF_PROFILE_PMF_RESERVED_START_BIT, IN_LIF_PROFILE_PMF_RESERVED_START_BIT - 1,
         IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT, IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT - 1, 0);
    PRT_COMMITX;
    PRT_TITLE_SET("VSI profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "VSI Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Routing Enablers Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "IVL/SVL Lookup Mode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MCv4 forwarding type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MCv6 forwarding type");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_PROFILE_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, &vsi_profile));
        vsi_profile = vsi_profile & ((1 << nof_ingress_bits) - 1);
        if ((saved_profile < vsi_profile) || ((default_entry == 0) && (vsi_profile == 0)))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            sal_itoa(prefix_value_binary_buffer, vsi_profile, 2, 0, 0);
            PRT_CELL_SET("%s%s%s", acl_bits_p, egress_bits_p, prefix_value_binary_buffer);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, INST_SINGLE,
                             &routing_enablers_profile));
            PRT_CELL_SET("%d", routing_enablers_profile);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_IVL_SVL, INST_SINGLE, &ivl_svl_mode));
            PRT_CELL_SET("%d", ivl_svl_mode);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_L2_V4_MC_FWD_TYPE, INST_SINGLE, &fwd_type));
            PRT_CELL_SET("%d", fwd_type);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_L2_V6_MC_FWD_TYPE, INST_SINGLE, &fwd_type));
            PRT_CELL_SET("%d", fwd_type);
            saved_profile = vsi_profile;
            if (vsi_profile == 0)
            {
                default_entry = 1;
            }
        }
        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    PRT_COMMITX;
exit:
    SHR_FREE(acl_bits_p);
    SHR_FREE(ingress_bits_p);
    SHR_FREE(egress_bits_p);
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints the inlif profile allocated resources
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_pp_occ_erpp_profile_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, out_lif_profile, ttl_threshold, ttl_threshold_index, entry_handle_tth_threshold_id;
    int is_end, outlif_profile_width, max_number_of_pp_profiles;
    char prefix_value_binary_buffer[4];
    char *acl_bits_p = NULL;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_end = 0;
    out_lif_profile = 0;
    ttl_threshold = 0;
    ttl_threshold_index = 0;
    entry_handle_tth_threshold_id = 0;
    outlif_profile_width = utilex_log2_round_up(dnx_data_lif.out_lif.nof_erpp_out_lif_profiles_get(unit));
    max_number_of_pp_profiles = (1 << (outlif_profile_width)) - 1;
    SHR_ALLOC(acl_bits_p, sizeof(*acl_bits_p) * 1, "acl_bits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    acl_bits_p[0] = 'x';
    acl_bits_p[1] = 0;

    PRT_TITLE_SET("ERPP outlif profile");
    PRT_INFO_ADD
        ("Profile bits separation: ACL profile bits = [%d : %d], ERPP RIF Profile bits = [%d : %d]",
         outlif_profile_width, outlif_profile_width, outlif_profile_width - 1, 0);
    PRT_COMMITX;

    PRT_TITLE_SET("ERPP PP RIF Profile allocation");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ERPP OutRif Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "TTL Threshold");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_TTL_SCOPE_TABLE, &entry_handle_tth_threshold_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, &out_lif_profile));
        sal_itoa(prefix_value_binary_buffer, out_lif_profile, 2, 0, 0);
        PRT_CELL_SET("%s%s", acl_bits_p, prefix_value_binary_buffer);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, INST_SINGLE, &ttl_threshold_index));
        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        dbal_entry_key_field32_set(unit, entry_handle_tth_threshold_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX,
                                   ttl_threshold_index);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_tth_threshold_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_tth_threshold_id, DBAL_FIELD_TTL_SCOPE, INST_SINGLE, &ttl_threshold));
        PRT_CELL_SET("%d", ttl_threshold);
        if (out_lif_profile == max_number_of_pp_profiles)
        {
            break;
        }
    }
    PRT_COMMITX;
exit:
    SHR_FREE(acl_bits_p);
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_pp_occ_sub_man = {
    .brief = "Displays inlif and  outlif profile allocated resources",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};

static sh_sand_man_t sh_dnx_pp_occ_outlif_profile_sub_man = {
    .brief = "Displays outlif profile allocated resources",
};

static sh_sand_man_t sh_dnx_pp_occ_inlif_profile_sub_man = {
    .brief = "Displays inlif profile allocated resources",
};

static sh_sand_man_t sh_dnx_pp_occ_etpp_profile_sub_man = {
    .brief = "Displays ETPP profile allocated resources",
    .full =
        "Displays ETPP profile allocated resources, such as orientation, OAM priority map profile and split horizon",
};
static sh_sand_man_t sh_dnx_pp_occ_erpp_profile_sub_man = {
    .brief = "Displays ERPP profile allocated resources",
    .full = "Displays ERPP profile allocated resources, such as ERPP filters enablers per outlif profile",
};
static sh_sand_man_t sh_dnx_pp_occ_eth_rif_profile_sub_man = {
    .brief = "Displays ETH RIF profile allocated resources",
    .full = "Displays ETH RIF profile allocated resources. The resources are shared for ACL bits\
            inlif profile deticated for egress usage and ingress profile bits ",
};
static sh_sand_man_t sh_dnx_pp_occ_vsi_profile_sub_man = {
    .brief = "Displays VSI profile allocated resources",
    .full = "Displays VSI profile allocated resources, such as routing enablers profile, \
            IVL/SVL and forwarding modes",
};

sh_sand_cmd_t sh_dnx_pp_occ_outlif_profile_sub_cmd[] = {

    /*
     * keyword, action, command, options, man
     */
    {
     "Etpp", sh_dnx_pp_occ_etpp_profile_print_cmd, NULL, NULL, &sh_dnx_pp_occ_etpp_profile_sub_man},
    {
     "Erpp", sh_dnx_pp_occ_erpp_profile_print_cmd, NULL, NULL, &sh_dnx_pp_occ_erpp_profile_sub_man},
    {
     NULL}
};

sh_sand_cmd_t sh_dnx_pp_occ_inlif_profile_sub_cmd[] = {

    /*
     * keyword, action, command, options, man
     */
    {
     "InLif", sh_dnx_pp_occ_inlif_profile_print_cmd, NULL, NULL, &sh_dnx_pp_occ_inlif_profile_sub_man},
    {
     "EthRif", sh_dnx_pp_occ_inlif_profile_print_cmd, NULL, NULL, &sh_dnx_pp_occ_eth_rif_profile_sub_man},
    {
     "Vsi", sh_dnx_pp_occ_vsi_profile_print_cmd, NULL, NULL, &sh_dnx_pp_occ_vsi_profile_sub_man},
    {
     NULL}
};
sh_sand_cmd_t sh_dnx_pp_occ_sub_cmd[] = {

    /*
     * keyword, action, command, options, man
     */
    {
     "Ingress", NULL, sh_dnx_pp_occ_inlif_profile_sub_cmd, NULL, &sh_dnx_pp_occ_inlif_profile_sub_man},
    {
     "Egress", NULL, sh_dnx_pp_occ_outlif_profile_sub_cmd, NULL, &sh_dnx_pp_occ_outlif_profile_sub_man},
    {
     NULL}
};
