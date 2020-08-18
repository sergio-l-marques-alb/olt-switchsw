/** \file diag_dnx_pp_sh.c
 *
 * System Header PP Diagnostics File
 * Resolves only the system headers that's passed from Ingress to Egress
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

/*************
 * TYPEDEFS  *
 *************/

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief - main function of showing the system headers command
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
sh_dnx_pp_sh_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core_low, core_high;
    int core = -1;

    rhlist_t *rhlist = NULL;
    rhlist_t *base_rhlist_to_free = NULL;

    /** assume default value be FALSE */
    uint8 short_mode = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    /*
     * Determine if the user passed a correct core value. If a specific correct core was asked, iterate on it only, otherwise on 0,1
     */
    set_core_low_high(unit, core, &core_low, &core_high);

    for (core_id = core_low; core_id <= core_high; core_id++)
    {

        signal_output_t *list_element;
        signal_output_t *sub_list_element;

        char sig_from[RHNAME_MAX_SIZE] = "fabric";
        char sig_root_name[RHNAME_MAX_SIZE] = "pkt_header";

        int sig_offset = 0;

        int rc;
        uint8 core_is_valid;

        /** check if core_id is valid on the Ingress */
        rc = diag_pp_check_core_validity(unit, core_id, FALSE, &core_is_valid);
        if ((!core_is_valid) || (rc != _SHR_E_NONE))
        {
           /** in SHORT mode don't output any message, just skip*/
            if (!short_mode)
            {
                cli_out("Please run a packet through Core_Id=%d.\n", core_id);
            }
            continue;
        }

        /*
         * - In c-model take the signal from applet, since from=fabric returns all zeros signal
         *   this means that when a packet isn't received on Egress, applet doesn't exist and
         *   SYSH diagnostics will not show
         */
#ifdef ADAPTER_SERVER_MODE
        sal_strncpy(sig_from, "applet", RHNAME_MAX_SIZE - 1);
#endif

        /** get the signal list */
        SHR_IF_ERR_EXIT(sig_get_to_rhlist(unit, core_id, "", sig_from, "", sig_root_name, &list_element, &rhlist));

        if (list_element == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "no signal record for %s signal, please run a packet !\n", sig_root_name);
        }

        /*
         * Open a table for all
         */
        PRT_TITLE_SET("PP VISiblity SYSH Core=%d Results", core_id);

        /** Creating columns which support spaces with no shrinking for hierarchy showing */
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Size (bits)");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Offset (bits)");

        /** iterate through all headers */
        RHITERATOR(sub_list_element, list_element->field_list)
        {
            /** current layer name - careful not to take the expansion name */
            char *name = RHNAME(sub_list_element);

            /** the location of header in the systems header array that matches, -1 if it's not a system header */
            int sub_layer_idx = 0;

            /*
             *  check that the header is system header, if not break
             */

            sub_layer_idx = string_in_array(name, FALSE, dnx_pp_system_header_names);

            if (sub_layer_idx == -1)
            {
                break;
            }
            else
            {
                /*
                 * Collect the recursive Name, Value, Size and Offset per Each signal and print
                 * Interate on NVS values (Name, Value, Size, Offset)
                 */

                char print_value[DSIG_MAX_SIZE_STR];
                rhlist_t *signal_sub_rhlist;
                int sig_size;
                int nvs_idx;

                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { sig_root_name, name, NULL };

                /** Get the size of the signal for updating the offset */
                SHR_IF_ERR_EXIT(sig_get_to_value
                                (unit, core_id, "", sig_from, "", sig_name_hieararchy,
                                 print_value, &sig_size, &signal_sub_rhlist, &base_rhlist_to_free));

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);

                for (nvs_idx = 0; nvs_idx < 4; nvs_idx++)
                {
                    char sig_result_str[DSIG_MAX_SIZE_STR] = { 0 };

                    /** get the hierarchical name/value/size/offset */
                    SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                    (unit, core_id, "", sig_from, "", sig_name_hieararchy, name, nvs_idx,
                                     sig_result_str, FALSE, TRUE, FALSE, sig_offset));

                    /** print the hierarchical name/value/size/offset of this system header */
                    PRT_CELL_SET("%s", sig_result_str);

                } /** of for NVS */

                /** update the offset for next RHITERATION with current offset + current overall system header size */
                sig_offset = sig_offset + sig_size;

            } /** of else of sub_layer_idx */

        } /** of RHITERATOR */

        /** close the table per Core_Id */
        PRT_COMMITX;

    } /** of for of core_id */

exit:

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(rhlist);
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_sh_man = {
    .brief = "PP VISibility SYS_Header diagnostics.\n"
        "Shows the System Headers information that is passed from Ingress to Egress.\n",
    .full = NULL,
    .synopsis = "[PP VISibility SYS_Header]",
    .examples = "\n" "SHORT\n" "file=diag_pp_vis_sh.xml"
};

sh_sand_option_t dnx_pp_sh_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};
