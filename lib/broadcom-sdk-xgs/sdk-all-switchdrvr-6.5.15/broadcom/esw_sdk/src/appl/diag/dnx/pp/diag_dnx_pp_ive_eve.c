/** \file diag_dnx_pp_ive_eve.c
 *
 * Ingress VLAN editing information of the last received packet
 *
 */

/*************
 * INCLUDES  *
 *************/
#include "diag_dnx_pp.h"

/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/*************
 *  MACROS  *
 *************/

/** increases a counter if signal's value is above '0' in an signal element which points to an rhlist of interest  */
#define __COUNT_SIGNAL_EXISTANCE(__sig_element,__sig_name,__counter)                                                             \
if (utilex_rhlist_entry_get_by_name(__sig_element->field_list, __sig_name) == NULL)                                              \
{                                                                                                                                \
    /** in case for some reason the signal not found, due to broken test don't crash, just don't count  */                       \
    SHR_CLI_EXIT(_SHR_E_NONE, "Warning - Could not find %s signal !\n", __sig_name);                                             \
}                                                                                                                                \
                                                                                                                                 \
if (((signal_output_t *)utilex_rhlist_entry_get_by_name(__sig_element->field_list, __sig_name))->value[0] > 0)                   \
{                                                                                                                                \
    __counter++;                                                                                                                 \
}                                                                                                                                \

/** prints in table a name of signal and value where parsing is string */
#define __PRT_TABLE_ENTRY_PARSE_STR(__shifts, __entry_name, __entry_value, __parsing, __line_separation)                                   \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                                  \
    PRT_CELL_SET("%s (%d)", __parsing, __entry_value);                                                                            \

/** prints in table a name of signal and value where parsing is hex number */
#define __PRT_TABLE_ENTRY_PARSE_INT(__shifts, __entry_name, __entry_value, __parsing, __line_separation)                                   \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                                  \
    PRT_CELL_SET("0x%x (%d)", __parsing, __entry_value);                                                                          \

/** prints in table a name of signal and value */
#define __PRT_TABLE_ENTRY(__shifts, __entry_name, __entry_value, __line_separation)                                                        \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                                  \
    PRT_CELL_SET("%d",__entry_value);                                                                                            \

/** prints in table a name of signal and hex value */
#define __PRT_TABLE_ENTRY_HEX(__shifts, __entry_name, __entry_value, __line_separation)                                          \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                  \
    PRT_CELL_SET("0x%x",__entry_value);                                                                                          \


/** returns the tpid_value according to the tpid_idx from the DBAL table GLOBAL_TPID */
#define __GET_TPID_VALUE(__entry_handle_id, __tpid_idx, __tpid_value)                                                            \
if (__tpid_idx < 7)                                                                                                              \
{                                                                                                                                \
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GLOBAL_TPID, __entry_handle_id));                                         \
    dbal_entry_key_field32_set(unit, __entry_handle_id, DBAL_FIELD_TPID_INDEX, __tpid_idx);                                      \
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, __entry_handle_id, DBAL_GET_ALL_FIELDS));                                               \
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get                                                                          \
                    (unit, __entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, &__tpid_value));                               \
}                                                                                                                                \

/** get the vlan_edit_cmd signals values, taking vlan_edit_cmd from Igress and corresponding Igress Core_Id */
#define __GET_VLAN_EDIT_CMD_SIGNAL(__core_id, __signal_name, __signal_value, __action_type_str)                                  \
{                                                                                                                                \
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "vlan_edit_cmd", __signal_name, NULL };                        \
    rc = sig_get_to_value                                                                                                        \
                    (unit, __core_id, "IRPP", "IPMF3", "", sig_name_hieararchy, prt_value,                                       \
                     &tmp_sig_size, &tmp_sub_rhlist, &base_rhlist_to_free);                                                      \
    if (rc != _SHR_E_NONE)                                                                                                       \
    {                                                                                                                            \
        /** print the warning only if in full mode, otherwise just skip*/                                                        \
        if (!short_mode)                                                                                                         \
        {                                                                                                                        \
            cli_out("\n%s is not performed for Ingress_Core=%d\n", __action_type_str, core_id);                                  \
        }                                                                                                                        \
        continue;                                                                                                                \
    }                                                                                                                            \
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);                                                                      \
    SHR_IF_ERR_EXIT(utilex_str_xstoul(prt_value, &__signal_value));                                                               \
}                                                                                                                                \


/** check if AC LIF source and print value if yes */
#define __CHECK_IF_SRC_LIF_PRINT(__shifts, _parse_value, _match_string, __print_value)                                                     \
if (sal_strncmp(_parse_value,_match_string, DSIG_MAX_SIZE_STR-1) == 0)                                                           \
{                                                                                                                                \
    __PRT_TABLE_ENTRY_HEX(__shifts, "(LIF_Value)", __print_value, PRT_ROW_SEP_NONE);                                                     \
}                                                                                                                                \

/** check if AC LIF source and print value if yes */
#define __CHECK_IF_SRC_LIF_PRINT_NAME(__shifts, _parse_value, _match_string, __print_value, __print_name)                                  \
if (sal_strncmp(_parse_value,_match_string, DSIG_MAX_SIZE_STR-1) == 0)                                                           \
{                                                                                                                                \
    __PRT_TABLE_ENTRY_HEX(__shifts, __print_name, __print_value, PRT_ROW_SEP_NONE);                                    \
}                                                                                                                                \

/** check signal availability, and skip core_id loop if not available */
#define __CHECK_IF_SIG_NULL_SKIP_CORE_ID(__sig_element, __sig_name, __action_name)                                               \
if (__sig_element == NULL)                                                                                                       \
{                                                                                                                                \
    cli_out("\n%s is not performed for Core_Id=%d\n", __action_name, core_id);                        \
    continue;                                                                                                                    \
}                                                                                                                                \

/*************
 * TYPEDEFS  *
 *************/

/*************
 * GLOBALS   *
 *************/
typedef enum
{
    IVE = 0,
    EVE_1 = 1,
    EVE_2 = 2,
    NOF_VLAN_EDITING_STAGES
} vlan_edit_stages_t;
/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - get the existing number of tags in the last packet in forwarding layer if it's ETHERNET
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] core_id
 *   \param [out] num_of_tags - pointer to the result
 *
 * \par DIRECT OUTPUT:
 *   int
 */
static shr_error_e
dnx_pp_ive_get_num_of_existing_eth_tags(
    int unit,
    int core_id,
    int *num_of_tags)
{

    /** init initial number of tags to 0 */
    int counter = 0;

    rhlist_t *qualifiers_rhlist = NULL;
    signal_output_t *layer_qualifiers_sig;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * this function is called from IVE and EVE FWD, and if IVE or EVE FWD were performed,
     * the layer of interest would be ETH layer 0, and so the qualifiers would be taken for layer 0
     */

    /** get the layer qualifier */
    SHR_IF_ERR_EXIT(sig_get_to_rhlist
                    (unit, core_id, "ETPP", "PRP2", "Term", "epp_layer_qualifiers", &layer_qualifiers_sig,
                     &qualifiers_rhlist));

    layer_qualifiers_sig = utilex_rhlist_entry_get_first(layer_qualifiers_sig->field_list);

    /** check that we stopped not due to end of layer_protocol_sig list but got to the index */
    if (layer_qualifiers_sig == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Core_Id %d doesn't contain IVE/EVE FWD info !\n", core_id);
    }

    __COUNT_SIGNAL_EXISTANCE(layer_qualifiers_sig, "1st_tpid_exist", counter);
    __COUNT_SIGNAL_EXISTANCE(layer_qualifiers_sig, "2nd_tpid_exist", counter);

exit:

    *num_of_tags = counter;
    SHR_FUNC_EXIT;
}

/**
 * \brief - check how many tags are there to add
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] stage
 *   \param [in] key_field_value
 *   \param [out] tpids_to_add
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_pp_ive_get_num_of_tags_to_add(
    int unit,
    vlan_edit_stages_t stage,
    uint32 key_field_value,
    int *tpids_to_add)
{

    dbal_tables_e dbal_table_name;
    dbal_fields_e dbal_key_field_name;

    uint32 entry_handle_id;

    uint32 vid_src_out, vid_src_in;

    int vid_to_add_count = 0;

    /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (stage == IVE)
    {
        dbal_table_name = DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND;
        dbal_key_field_name = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    }
    else if (stage == EVE_1)
    {
        dbal_table_name = DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD;
        dbal_key_field_name = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    }
    else if (stage == EVE_2)
    {
        dbal_table_name = DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP;
        dbal_key_field_name = DBAL_FIELD_VLAN_EDIT_PROFILE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognizable DBAL VLAN EDIT Table !\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key_field_name, key_field_value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (stage == IVE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));

        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_OUTER_VID_SRC, vid_src_out, &vid_src_out);
        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_INNER_VID_SRC, vid_src_in, &vid_src_in);
    }
    else if ((stage == EVE_1) || (stage == EVE_2))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));

        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_VID_SRC, vid_src_out, &vid_src_out);
        dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_VID_SRC, vid_src_in, &vid_src_in);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognizable DBAL VLAN EDIT Table !\n");
    }

    /** we check that the VID source is not equal to 0 */
    if (vid_src_out)
    {
        vid_to_add_count++;
    }

    if (vid_src_in)
    {
        vid_to_add_count++;
    }

exit:

    *tpids_to_add = vid_to_add_count;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_pp_ive_eve_get_tags_to_remove(
    int unit,
    vlan_edit_stages_t stage,
    uint32 vlan_edit_cmd,
    uint32 *tags_to_remove)
{

    uint32 entry_handle_id;
    uint32 local_tags_to_remove = 0;
    dbal_tables_e dbal_table_name;

    /*
     * access the DBAL table and get all IVE information
     */
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (stage == IVE)
    {
        dbal_table_name = DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND;
    }
    else if (stage == EVE_1)
    {
        dbal_table_name = DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD;
    }
    else if (stage == EVE_2)
    {
        dbal_table_name = DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognizable DBAL VLAN EDIT Table !\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, vlan_edit_cmd);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE, &local_tags_to_remove));

exit:
    *tags_to_remove = local_tags_to_remove;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - main function of Ingress VLAN editing information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_ive_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int core_id, core_low, core_high;
    int core = -1;

    rhlist_t *base_rhlist_to_free = NULL;

    /** assume default value be FALSE */
    uint8 short_mode = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    PRT_INIT_VARS;

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    /*
     * Determine if the user passed a correct core value. If a specific correct core was asked, iterate on it only, otherwise on 0,1
     */
    set_core_low_high(unit, core, &core_low, &core_high);

    for (core_id = core_low; core_id <= core_high; core_id++)
    {

        int rc;
        uint32 entry_handle_id;

        uint8 core_is_valid;

        int ing_core_id;

        /** check if Egress core_id is valid, at which the IVE signals are read */
        rc = diag_pp_check_core_validity(unit, core_id, FALSE, &core_is_valid);
        if ((!core_is_valid) || (rc != _SHR_E_NONE))
        {
            /** in SHORT mode don't output any message, just skip*/
            if (!short_mode)
            {
                cli_out("IVE: Please run a packet through Egress_Core=%d.\n", core_id);
            }
            continue;
        }

        /*
         * - Getting the ingress_vlan_edit_command DBAL table info,
         *   which contains the outer/inner tpid,vid,pcp and tags to remove
         * - For each Egress Core_Id we iterate on available Ingress Core IDs,
         *   because only there we can take the VLAN Edit Command, which is key to IVE DBAL,
         *   and we present for each possible Ingress Core_Id the IVE info
         */

        for (ing_core_id = core_low; ing_core_id <= core_high; ing_core_id++)
        {

            /** dbal variables */
            uint32 tpid_idx_out, tpid_idx_in;
            uint32 vid_src_out, vid_src_in;
            uint32 pcp_dei_src_out, pcp_dei_src_in;
            uint32 tags_to_remove;
            uint32 vlan_edit_cmd = 0;
            uint32 ac_lif_vid_out = 0;
            uint32 ac_lif_vid_in = 0;
            uint32 ac_lif_dei_out = 0;
            uint32 ac_lif_dei_in = 0;
            uint32 ac_lif_pcp_out = 0;
            uint32 ac_lif_pcp_in = 0;
            uint32 ac_lif_dei_pcp_out = 0;
            uint32 ac_lif_dei_pcp_in = 0;

            uint32 tpid_value_out, tpid_value_in;
            char vid_parse_out[DSIG_MAX_SIZE_STR];
            char vid_parse_in[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_out[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_in[DSIG_MAX_SIZE_STR];

            /** rhlist variables */
            int tmp_sig_size;
            rhlist_t *tmp_sub_rhlist = NULL;
            char prt_value[DSIG_MAX_SIZE_STR];

            /** tpid count variables */
            int tpid_count = 0;
            int ive_num_of_tags_to_add = 0;

            /** check validity of Ingress Core_ID */
            rc = diag_pp_check_core_validity(unit, ing_core_id, TRUE, &core_is_valid);

            if ((!core_is_valid) || (rc != _SHR_E_NONE))
            {
                continue;
            }

            /** get the vlan_edit_cmd values */
            __GET_VLAN_EDIT_CMD_SIGNAL(ing_core_id, "Cmd", vlan_edit_cmd, "IVE");
            __GET_VLAN_EDIT_CMD_SIGNAL(ing_core_id, "VID_1", ac_lif_vid_out, "IVE");
            __GET_VLAN_EDIT_CMD_SIGNAL(ing_core_id, "VID_2", ac_lif_vid_in, "IVE");
            __GET_VLAN_EDIT_CMD_SIGNAL(ing_core_id, "DEI_PCP1", ac_lif_dei_pcp_out, "IVE");
            __GET_VLAN_EDIT_CMD_SIGNAL(ing_core_id, "DEI_PCP2", ac_lif_dei_pcp_in, "IVE");

            /** split DEI and PCP fields from the DEI[1]_PCP[3] bitstream */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_out,0,1,&ac_lif_dei_out));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_out,1,3,&ac_lif_pcp_out));

            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_in,0,1,&ac_lif_dei_in));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_in,1,3,&ac_lif_pcp_in));

            /*
             * access the DBAL table and get all IVE information
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, vlan_edit_cmd);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &tpid_idx_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &tpid_idx_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_INNER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE, &tags_to_remove));

            PRT_TITLE_SET("PP VISiblity IVE IRPP (Assuming Ingress_Core=%d), Egress_Core=%d", ing_core_id, core_id);

            /** Creating columns which support spaces with no shrinking for hierarchy showing */
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");

            /*
             * count the number of tags that are present to check that tags_to_remove doesn't exceed it
             */
            dnx_pp_ive_get_num_of_existing_eth_tags(unit, core_id, &tpid_count);

            if (tags_to_remove > tpid_count)
            {
                PRT_INFO_ADD("**Number of tags to remove is larger than existing tags.");
            }

            /*
             * Parsing the received values for more information to print
             */

            /** TPID index information is present in GLOBAL_TPID table */
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_out, tpid_value_out);
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_in, tpid_value_in);

            /*
             *  get the DBAL text parse of VID, PCP DEI source.
             *  casting 'dbal_field_to_string' because it returns (const char*) and
             *  dnx_dbal_fields_string_form_hw_value_get accepts (char *)
             */

            /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_OUTER_VID_SRC, vid_src_out, &vid_src_out);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit, DBAL_FIELD_IVE_OUTER_VID_SRC),
                                                     vid_src_out, vid_parse_out);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC, pcp_dei_src_out, &pcp_dei_src_out);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit,
                                                                                   DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC),
                                                     pcp_dei_src_out, pcp_dei_parse_out);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_INNER_VID_SRC, vid_src_in, &vid_src_in);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit, DBAL_FIELD_IVE_INNER_VID_SRC),
                                                     vid_src_in, vid_parse_in);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_INNER_PCP_DEI_SRC, pcp_dei_src_in, &pcp_dei_src_in);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit,
                                                                                   DBAL_FIELD_IVE_INNER_PCP_DEI_SRC),
                                                     pcp_dei_src_in, pcp_dei_parse_in);

            SHR_IF_ERR_EXIT(dnx_pp_ive_get_num_of_tags_to_add(unit, IVE, vlan_edit_cmd, &ive_num_of_tags_to_add));

            /** validate that inner tag isn't added before outer */
            if ((vid_src_in) && (!vid_src_out))
            {
                PRT_INFO_ADD("**Inner VID is marked to be added while Outer is not.");
            }

            /*
             * print all the IVE fields in the table
             */
            __PRT_TABLE_ENTRY_HEX(0, "Cmd_Index", vlan_edit_cmd, PRT_ROW_SEP_NONE);
            __PRT_TABLE_ENTRY_PARSE_INT(1, "Outer_TPID_index", tpid_idx_out, tpid_value_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_VID_source", vid_src_out, vid_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_out, "IVE_OUTER_VID_SRC_AC_OUTER_VID", ac_lif_vid_out);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_out, "IVE_OUTER_VID_SRC_AC_INNER_VID", ac_lif_vid_in);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_PCP_DEI_source", pcp_dei_src_out, pcp_dei_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_dei_out, "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_pcp_out, "PCP_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_dei_in,
                                          "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_pcp_in,
                                          "PCP_(LIF_Value)");

            __PRT_TABLE_ENTRY_PARSE_INT(1, "Inner_TPID_index", tpid_idx_in, tpid_value_in,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_VID_source", vid_src_in, vid_parse_in, PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_in, "IVE_INNER_VID_SRC_AC_OUTER_VID", ac_lif_vid_out);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_in, "IVE_INNER_VID_SRC_AC_INNER_VID", ac_lif_vid_in);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_PCP_DEI_source", pcp_dei_src_in, pcp_dei_parse_in,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_dei_out, "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_pcp_out, "PCP_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_dei_in,
                                          "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_pcp_in,
                                          "PCP_(LIF_Value)");

            __PRT_TABLE_ENTRY(1, "Tags_to_remove", tags_to_remove, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY(0, "Tags_to_add", ive_num_of_tags_to_add, PRT_ROW_SEP_EQUAL_BEFORE);
            __PRT_TABLE_ENTRY(0, "Original_packet_tags_number", tpid_count, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            PRT_COMMITX;

        } /** of ing_core_id structure */

    } /** of for of core_id */

exit:

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - main function of Egress VLAN editing information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_eve_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    rhlist_t *general_rhlist = NULL;
    rhlist_t *in_tag_s_rhlist = NULL;
    rhlist_t *vlan_edit_profile_rhlist = NULL;
    rhlist_t *etps_valid_rhlist = NULL;
    rhlist_t *base_rhlist_to_free = NULL;

    int core_id, core_low, core_high;
    int core = -1;

    /** assume initial value is FALSE*/
    uint8 short_mode = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    PRT_INIT_VARS;

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    /*
     * Determine if the user passed a correct core value. If a specific correct core was asked, iterate on it only, otherwise on 0,1
     */
    set_core_low_high(unit, core, &core_low, &core_high);

    for (core_id = core_low; core_id <= core_high; core_id++)
    {
        /*
         * signal value variables
         */
        signal_output_t *general_sig;
        signal_output_t *in_tag_s_sig;
        signal_output_t *vlan_edit_profile_sig;

        uint32 llvp_profile;
        uint32 in_tag_s;
        uint32 vlan_edit_profile;
        uint32 eve_vlan_edit_cmd;

        int rc;

        uint32 entry_handle_id;

        /** dbal variables */
        uint32 tpid_idx_out, tpid_idx_in;
        uint32 vid_src_out, vid_src_in;
        uint32 pcp_dei_src_out, pcp_dei_src_in;

        uint8 core_is_valid;

        int ing_core_id;

        /** check if core_id is valid */
        rc = diag_pp_check_core_validity(unit, core_id, FALSE, &core_is_valid);
        if ((!core_is_valid) || (rc != _SHR_E_NONE))
        {
            /** print the warning only if not in a short mode */
            if (!short_mode)
            {
                cli_out("EVE_FWD: Please run a packet through Egress_Core=%d.\n", core_id);
            }
            continue;
        }

        /*
         * Get the LLVP Profile and Tag Format which are keys to LLVP Table that outputs VLAN Edit Profile and Tag Structure
         */
        SHR_IF_ERR_EXIT(sig_get_to_rhlist
                        (unit, core_id, "ETPP", "PRP2", "Term", "per_port_attributes", &general_sig, &general_rhlist));
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(general_sig, "per_port_attributes", "EVE_FWD");

        /** get one hierarchy down for the incoming_tag_structure */
        general_sig = utilex_rhlist_entry_get_by_name(general_sig->field_list, "LLVP_Profile");
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(general_sig, "LLVP_Profile", "EVE_FWD");
        llvp_profile = general_sig->value[0];

        /*
         * To get the Egress "VLAN Edit Index Command" - need to look in DBAL table "EGRESS_VLAN_EDIT_COMMAND_MAPPING"
         * there use 2 keys - "Incoming Tag Structure" and "VLAN edit profile"
         */

        /** get the incoming tag structure */
        SHR_IF_ERR_EXIT(sig_get_to_rhlist
                        (unit, core_id, "ETPP", "Term", "FWD", "LLVP_Data", &in_tag_s_sig, &in_tag_s_rhlist));
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(in_tag_s_sig, "LLVP_Data", "EVE_FWD");

        /** get one hierarchy down for the incoming_tag_structure */
        in_tag_s_sig = utilex_rhlist_entry_get_by_name(in_tag_s_sig->field_list, "Incoming_Tag_Structure");
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(in_tag_s_sig, "Incoming_Tag_Structure", "EVE_FWD");
        in_tag_s = in_tag_s_sig->value[0];

        /** Get the VLAN edit profile */
        SHR_IF_ERR_EXIT(sig_get_to_rhlist
                        (unit, core_id, "ETPP", "Term", "FWD", "ETPS",
                         &vlan_edit_profile_sig, &vlan_edit_profile_rhlist));
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(vlan_edit_profile_sig, "ETPS", "EVE_FWD");

        /** get one hierarchy down and search for the VLAN edit profile field */
        vlan_edit_profile_sig = utilex_rhlist_entry_get_first(vlan_edit_profile_sig->field_list);
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(vlan_edit_profile_sig, "ETPS_1", "EVE_FWD");

        vlan_edit_profile_sig =
            utilex_rhlist_entry_get_by_name(vlan_edit_profile_sig->field_list, "EGRESS_VLAN_EDIT_PROFILE");
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(vlan_edit_profile_sig, "EGRESS_VLAN_EDIT_PROFILE", "EVE_FWD");

        vlan_edit_profile = vlan_edit_profile_sig->value[0];

        /** access the DBAL Table "EGRESS_VLAN_EDIT_COMMAND_MAPPING" to get the VLAN editing command */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, vlan_edit_profile);

        /** convert the in_tag_s HW value (from signal) to ENUM value of this field in DBAL */
        dbal_fields_enum_value_get(unit, DBAL_FIELD_INCOMING_TAG_STRUCTURE, in_tag_s, &in_tag_s);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, in_tag_s);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE, &eve_vlan_edit_cmd));

        /*
         * - Proceed with getting the egress_vlan_edit_command DBAL table info
         *   which contains the outer/inner tpid,vid,pcp and tags to remove
         * - For each Egress Core_Id we iterate on available Ingress Core IDs,
         *   because only there we can take the VLAN Edit Command, which is key to DBAL,
         *   for which to read number of tags_to_add and tags_to_remove
         */

        for (ing_core_id = core_low; ing_core_id <= core_high; ing_core_id++)
        {

            uint32 tpid_value_out, tpid_value_in;
            char vid_parse_out[DSIG_MAX_SIZE_STR];
            char vid_parse_in[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_out[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_in[DSIG_MAX_SIZE_STR];

            /** tags validation */
            uint32 ive_tags_to_remove;
            int ive_num_of_tags_to_add;
            int after_ive_num_of_tags = 0;
            uint32 eve_fwd_tags_to_remove;
            int eve_num_of_tags_to_add;

            uint32 ive_vlan_edit_cmd = 0;

            /** rhlist variables */
            int tmp_sig_size;
            rhlist_t *tmp_sub_rhlist = NULL;
            char prt_value[DSIG_MAX_SIZE_STR];

            /** tpid count variables */
            int tpid_count = 0;

            /** check validity of Ingress Core_ID */
            rc = diag_pp_check_core_validity(unit, ing_core_id, TRUE, &core_is_valid);

            if ((!core_is_valid) || (rc != _SHR_E_NONE))
            {
                continue;
            }

            /*
             * access the DBAL table and get all IVE information
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD, entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, eve_vlan_edit_cmd);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &tpid_idx_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &tpid_idx_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE, &eve_fwd_tags_to_remove));

            PRT_TITLE_SET("PP VISiblity EVE_FWD (Assuming Ingress_Core=%d), Egress_Core=%d", ing_core_id, core_id);

            /** Creating columns which support spaces with no shrinking for hierarchy showing */
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");

            /*
             * Check that in both in EVE FWD stage there was no violation of tags removing or adding
             */

            /** original number of TPIDs in the packet */
            SHR_IF_ERR_EXIT(dnx_pp_ive_get_num_of_existing_eth_tags(unit, core_id, &tpid_count));

            /** get the IVE vlan_edit_cmd values */
            __GET_VLAN_EDIT_CMD_SIGNAL(ing_core_id, "Cmd", ive_vlan_edit_cmd, "EVE_FWD");

            /** IVE tags to remove */
            SHR_IF_ERR_EXIT(dnx_pp_ive_eve_get_tags_to_remove(unit, IVE, ive_vlan_edit_cmd, &ive_tags_to_remove));

            /** IVE tags to add */
            SHR_IF_ERR_EXIT(dnx_pp_ive_get_num_of_tags_to_add(unit, IVE, ive_vlan_edit_cmd, &ive_num_of_tags_to_add));

            SHR_IF_ERR_EXIT(dnx_pp_ive_get_num_of_tags_to_add(unit, EVE_1, eve_vlan_edit_cmd, &eve_num_of_tags_to_add));

            /** check that after IVE stage, the total tag count is 0 <= x <= 2 */
            after_ive_num_of_tags = tpid_count - ive_tags_to_remove + ive_num_of_tags_to_add;

            if ((after_ive_num_of_tags < 0) || (after_ive_num_of_tags > 2))
            {
                PRT_INFO_ADD("**IVE total tag count is negative - Run PP VIS IVE - pkt tags:%d, removed:%d, added:%d.",
                             tpid_count, ive_tags_to_remove, ive_num_of_tags_to_add);
            }

            /** check that in EVE tags to remove isn't bigger of left tags after IVE */
            if (eve_fwd_tags_to_remove > after_ive_num_of_tags)
            {
                PRT_INFO_ADD("**EVE_FWD num of tags to remove:%d is larger than total tags after:%d.",
                             eve_fwd_tags_to_remove, after_ive_num_of_tags);
            }

            /*
             * Parsing the received values for more information to print
             */

            /** TPID index information is present in GLOBAL_TPID table */
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_out, tpid_value_out);
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_in, tpid_value_in);

            /*
             *  get the DBAL text parse of VID, PCP DEI source.
             *  casting 'dbal_field_to_string' because it returns (const char*) and
             *  dnx_dbal_fields_string_form_hw_value_get accepts (char *)
             */

            /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_VID_SRC, vid_src_out, &vid_src_out);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_OUTER_VID_SRC),
                                                     vid_src_out, vid_parse_out);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, pcp_dei_src_out, &pcp_dei_src_out);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit,
                                                                                   DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC),
                                                     pcp_dei_src_out, pcp_dei_parse_out);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_VID_SRC, vid_src_in, &vid_src_in);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_INNER_VID_SRC),
                                                     vid_src_in, vid_parse_in);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, pcp_dei_src_in, &pcp_dei_src_in);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit,
                                                                                   DBAL_FIELD_EVE_INNER_PCP_DEI_SRC),
                                                     pcp_dei_src_in, pcp_dei_parse_in);

            /**  validate that inner tag isn't added before outer */

            if ((vid_src_in) && (!vid_src_out))
            {
                PRT_INFO_ADD("**EVE_FWD inner VID is marked to be added while Outer is not.");
            }

            /*
             * print all the EVE FWD fields in the table
             */
            __PRT_TABLE_ENTRY_HEX(0, "VLAN_Edit_Profile", vlan_edit_profile, PRT_ROW_SEP_NONE);
            __PRT_TABLE_ENTRY_HEX(0, "LLVP_Profile", llvp_profile, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /** print the parsing of the IVE_Eth_Tag_Format */
            {
                char sig_result_str[DSIG_MAX_SIZE_STR];
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "ive_eth_tag_format", NULL };

                /** print the parsed names */
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "ETPP", "PRP2", "Term", sig_name_hieararchy, "Tag_Format", 0,
                                 sig_result_str, FALSE, TRUE, FALSE, 0));

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("%s", sig_result_str);

                /** print the parsed values */
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "ETPP", "PRP2", "Term", sig_name_hieararchy, "Tag_Format", 1,
                                 sig_result_str, FALSE, TRUE, FALSE, 0));
                PRT_CELL_SET("%s", sig_result_str);
            }
            __PRT_TABLE_ENTRY_HEX(1, "Tag_Structure", in_tag_s, PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __PRT_TABLE_ENTRY_HEX(2, "Cmd_Index", eve_vlan_edit_cmd, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /** tpid 14 and 15 are special cases that are taken from Packets TPID */
            if (tpid_idx_out == 14)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(3, "Outer_TPID_index", tpid_idx_out, "Packet_Inner_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else if (tpid_idx_out == 15)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(3, "Outer_TPID_index", tpid_idx_out, "Packet_Outer_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                __PRT_TABLE_ENTRY_PARSE_INT(3, "Outer_TPID_index", tpid_idx_out, tpid_value_out,
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }

            __PRT_TABLE_ENTRY_PARSE_STR(3, "Outer_VID_source", vid_src_out, vid_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(3, "Outer_PCP_DEI_source", pcp_dei_src_out, pcp_dei_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /** tpid 14 and 15 are special cases that are taken from Packets TPID */
            if (tpid_idx_in == 14)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(3, "Inner_TPID_index", tpid_idx_in, "Packet_Inner_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else if (tpid_idx_in == 15)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(3, "Inner_TPID_index", tpid_idx_in, "Packet_Outer_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                __PRT_TABLE_ENTRY_PARSE_INT(3, "Inner_TPID_index", tpid_idx_in, tpid_value_in,
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }

            __PRT_TABLE_ENTRY_PARSE_STR(3, "Inner_VID_source", vid_src_in, vid_parse_in, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(3, "Inner_PCP_DEI_source", pcp_dei_src_in, pcp_dei_parse_in,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY(3, "Tags_to_remove", eve_fwd_tags_to_remove, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY(0, "Packet_tags_num", tpid_count, PRT_ROW_SEP_EQUAL_BEFORE);
            __PRT_TABLE_ENTRY(0, "IVE_tags_to_remove", ive_tags_to_remove, PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __PRT_TABLE_ENTRY(0, "IVE_tags_to_add", ive_num_of_tags_to_add, PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __PRT_TABLE_ENTRY(0, "EVE_tags_to_add", eve_num_of_tags_to_add, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            PRT_COMMITX;

        } /** of general structure */

    } /** of for of core_id */

    /******************************
     * Now handle EVE ENCAP stage *
     ******************************/

    for (core_id = core_low; core_id <= core_high; core_id++)
    {
        /*
         * signal value variables
         */
        signal_output_t *vlan_edit_profile_sig = NULL;
        signal_output_t *prev_edit_profile_sig = NULL;

        uint32 vlan_edit_profile;

        uint32 entry_handle_id;

         /** dbal variables */
        uint32 tpid_idx_out, tpid_idx_in;
        uint32 vid_src_out, vid_src_in;
        uint32 pcp_dei_src_out, pcp_dei_src_in;

        int last_etps_valid_idx;
        signal_output_t *etps_valid_sig = NULL;

        uint8 core_is_valid;

        int rc;

         /** check if core_id is valid */
        rc = diag_pp_check_core_validity(unit, core_id, FALSE, &core_is_valid);
        if ((!core_is_valid) || (rc != _SHR_E_NONE))
        {
            /** print the warning message only if not in short_mode */
            if (!short_mode)
            {
                cli_out("EVE_ENCAP: Please run a packet through Egress_Core=%d.\n", core_id);
            }
            continue;
        }

        /*
         * Get the EVE Encap "VLAN Edit Profile - it's located as last in the ETPS stack"
         */

          /** Get the VLAN edit profile */
        SHR_IF_ERR_EXIT(sig_get_to_rhlist
                        (unit, core_id, "ETPP", "Term", "FWD", "ETPS",
                         &vlan_edit_profile_sig, &vlan_edit_profile_rhlist));
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(vlan_edit_profile_sig, "ETPS", "EVE_ENCAP");

          /** get one hierarchy down and search for the VLAN edit profile field */
        vlan_edit_profile_sig = utilex_rhlist_entry_get_first(vlan_edit_profile_sig->field_list);
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(vlan_edit_profile_sig, "ETPS_1", "EVE_ENCAP");

          /** get the ETPS valid signal */
        SHR_IF_ERR_EXIT(sig_get_to_rhlist
                        (unit, core_id, "ETPP", "Term", "FWD", "ETPS_Valid", &etps_valid_sig, &etps_valid_rhlist));
        __CHECK_IF_SIG_NULL_SKIP_CORE_ID(etps_valid_sig, "ETPS_Valid", "EVE_ENCAP");

          /** Iterate on the ETPS valid indexes to find last that's non zero */
        last_etps_valid_idx = 0;
        RHITERATOR(etps_valid_sig, etps_valid_sig->field_list)
        {
            if (etps_valid_sig->value[0] == 0)
            {
                break;
            }
            last_etps_valid_idx++;

            prev_edit_profile_sig = vlan_edit_profile_sig;
              /** also iterate on the ETPS_(x) fields to get to last ETPS last in stack */
            vlan_edit_profile_sig = utilex_rhlist_entry_get_next(vlan_edit_profile_sig);
        }

          /** get the previous pointer, which's valid was '1' */
        vlan_edit_profile_sig = prev_edit_profile_sig;

          /** if the last ETPS is 1 or 0, means that there's only EVE FWD and not EVE ENCAP - skip this Core_Id */
        if (last_etps_valid_idx <= 1)
        {
            /** print warning only if in full mode */
            if (!short_mode)
            {
                cli_out
                    ("EVE_ENCAP: In Core_Id=%d Ethernet layer is not built in ENCAP5, therefore EVE_ENCAP is not relevant\n",
                     core_id);
            }
            continue;
        }

        vlan_edit_profile = vlan_edit_profile_sig->value[0];

        /*
         * At this point we know that ENCAP5 is used to build ETH encapsulation
         */
        {

            uint32 tpid_value_out, tpid_value_in;
            char vid_parse_out[DSIG_MAX_SIZE_STR];
            char vid_parse_in[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_out[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_in[DSIG_MAX_SIZE_STR];

              /** tags validation */
            int eve_num_of_tags_to_add;

              /** get the EVE FWD - last stack ETPS vlan_edit_profile */
            vlan_edit_profile_sig =
                utilex_rhlist_entry_get_by_name(vlan_edit_profile_sig->field_list, "EGRESS_VLAN_EDIT_PROFILE");
            __CHECK_IF_SIG_NULL_SKIP_CORE_ID(vlan_edit_profile_sig, "EGRESS_VLAN_EDIT_PROFILE", "EVE_ENCAP");

            vlan_edit_profile = vlan_edit_profile_sig->value[0];

            /*
             * access the DBAL table and get all IVE information
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, vlan_edit_profile);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &tpid_idx_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &tpid_idx_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_in));

            PRT_TITLE_SET("PP VISiblity EVE_ENCAP Egress_Core=%d", core_id);

              /** Creating columns which support spaces with no shrinking for hierarchy showing */
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");

            SHR_IF_ERR_EXIT(dnx_pp_ive_get_num_of_tags_to_add(unit, EVE_2, vlan_edit_profile, &eve_num_of_tags_to_add));

            /*
             * Parsing the received values for more information to print
             */

              /** TPID index information is present in GLOBAL_TPID table */
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_out, tpid_value_out);
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_in, tpid_value_in);

            /*
             *  get the DBAL text parse of VID, PCP DEI source.
             *  casting 'dbal_field_to_string' because it returns (const char*) and
             *  dnx_dbal_fields_string_form_hw_value_get accepts (char *)
             */

              /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_VID_SRC, vid_src_out, &vid_src_out);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_OUTER_VID_SRC),
                                                     vid_src_out, vid_parse_out);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, pcp_dei_src_out, &pcp_dei_src_out);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit,
                                                                                   DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC),
                                                     pcp_dei_src_out, pcp_dei_parse_out);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_VID_SRC, vid_src_in, &vid_src_in);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_INNER_VID_SRC),
                                                     vid_src_in, vid_parse_in);

            dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, pcp_dei_src_in, &pcp_dei_src_in);
            dnx_dbal_fields_string_form_hw_value_get(unit,
                                                     (char *) dbal_field_to_string(unit,
                                                                                   DBAL_FIELD_EVE_INNER_PCP_DEI_SRC),
                                                     pcp_dei_src_in, pcp_dei_parse_in);

              /**  validate that inner tag isn't added before outer */

            if ((vid_src_in) && (!vid_src_out))
            {
                PRT_INFO_ADD("**EVE_ENCAP inner VID is marked to be added while Outer is not.");
            }

            /*
             * print all the EVE FWD fields in the table
             */

            __PRT_TABLE_ENTRY_HEX(0, "VLAN_Edit_Profile", vlan_edit_profile, PRT_ROW_SEP_NONE);

              /** tpid 14 and 15 are special cases that are taken from Packets TPID */
            if (tpid_idx_out == 14)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_TPID_index", tpid_idx_out, "Packet_Inner_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else if (tpid_idx_out == 15)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_TPID_index", tpid_idx_out, "Packet_Outer_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                __PRT_TABLE_ENTRY_PARSE_INT(1, "Outer_TPID_index", tpid_idx_out, tpid_value_out,
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_VID_source", vid_src_out, vid_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_PCP_DEI_source", pcp_dei_src_out, pcp_dei_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

              /** tpid 14 and 15 are special cases that are taken from Packets TPID */
            if (tpid_idx_in == 14)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_TPID_index", tpid_idx_in, "Packet_Inner_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else if (tpid_idx_in == 15)
            {
                __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_TPID_index", tpid_idx_in, "Packet_Outer_TPID",
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                __PRT_TABLE_ENTRY_PARSE_INT(1, "Inner_TPID_index", tpid_idx_in, tpid_value_in,
                                            PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_VID_source", vid_src_in, vid_parse_in, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_PCP_DEI_source", pcp_dei_src_in, pcp_dei_parse_in,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY(0, "Tags_to_add", eve_num_of_tags_to_add, PRT_ROW_SEP_EQUAL_BEFORE);

            PRT_COMMITX;

        }   /** of EVE ENCAP general structure */

    }  /** of EVE ENCAP for Core_Id*/

exit:

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(in_tag_s_rhlist);
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(vlan_edit_profile_rhlist);
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(etps_valid_rhlist);
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(general_rhlist);

    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_ive_man = {
    .brief = "PP VISibility IngressVlanEditing diagnostics.\n"
        "Shows the Ingress VLAN editing information of the last received packet.\n",
    .full = NULL,
    .synopsis = "[PP VISibility IngressVlanEditing]",
    .examples = "\n" "SHORT\n" "file=diag_pp_vis_ive.xml"
};

sh_sand_option_t dnx_pp_ive_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};

sh_sand_man_t dnx_pp_eve_man = {
    .brief = "PP VISibility EgressVlanEditing diagnostics.\n"
        "Shows the Egress VLAN editing information of the last received packet.\n",
    .full = NULL,
    .synopsis = "[PP VISibility EgressVlanEditing]",
    .examples = "\n" "SHORT\n" "file=diag_pp_vis_eve.xml"
};

sh_sand_option_t dnx_pp_eve_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};
