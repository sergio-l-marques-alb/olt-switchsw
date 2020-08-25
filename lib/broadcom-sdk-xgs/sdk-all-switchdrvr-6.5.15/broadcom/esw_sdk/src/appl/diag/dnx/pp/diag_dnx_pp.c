/** \file diag_dnx_pp.c
 *
 * Main file for PP Diagnostics commands calls.
 */

/*************
 * INCLUDES  *
 *************/

#include "diag_dnx_pp.h"
#include "diag_dnx_pp_occ.h"

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
/** sub layer names that are defined to be a sublayer and not a real layer - for printing sakes */
char *sh_dnx_pp_sub_layer_names[] = { "VLAN", "GRE", "UDP", "VXLAN", "" };

/** This array contains strings that are system layers. only substring is enough. last string should be "" */
char *dnx_pp_system_header_names[] = { "FTMH", "PTCH", "ITMH", "UDH", "TSH", "PPH", "LB_Key", "DSP", "Stacking", "OTSH", "FHEI", "" };

/*************
 * EXTERNAL  *
 * FUNCTIONS *
 *************/

/*
 * ppi command external function declaration
 */
extern sh_sand_option_t dnx_pp_ppi_options[];
extern sh_sand_man_t dnx_pp_ppi_man;
extern shr_error_e sh_dnx_pp_ppi_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * pkttm command external function declaration
 */
extern sh_sand_option_t dnx_pp_pkttm_options[];
extern sh_sand_man_t dnx_pp_pkttm_man;
extern shr_error_e sh_dnx_pp_pkttm_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * fdt command external function declaration
 */
extern sh_sand_option_t dnx_pp_fdt_options[];
extern sh_sand_man_t dnx_pp_fdt_man;
extern shr_error_e sh_dnx_pp_fdt_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * termi command external function declaration
 */
extern sh_sand_option_t dnx_pp_termi_options[];
extern sh_sand_man_t dnx_pp_termi_man;
extern shr_error_e sh_dnx_pp_termi_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * VISibility FEC (LAST) command external functions declaration
 */
extern sh_sand_man_t dnx_pp_vis_fec_man;
extern sh_sand_option_t dnx_pp_vis_fec_man_options[];
extern shr_error_e sh_dnx_pp_vis_fec_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * INFo FEC (ALL) command external functions declaration
 */
extern sh_sand_man_t dnx_pp_info_fec_man;

extern sh_sand_option_t dnx_pp_info_fec_man_options[];
extern shr_error_e sh_dnx_pp_info_fec_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * VISibility SystemHeaders command external functions declaration
 */
extern sh_sand_option_t dnx_pp_sh_options[];
extern sh_sand_man_t dnx_pp_sh_man;
extern shr_error_e sh_dnx_pp_sh_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * VISibility IVE and EVE command external functions declaration
 */
extern sh_sand_option_t dnx_pp_ive_options[];
extern sh_sand_man_t dnx_pp_ive_man;
extern shr_error_e sh_dnx_pp_ive_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t dnx_pp_eve_options[];
extern sh_sand_man_t dnx_pp_eve_man;
extern shr_error_e sh_dnx_pp_eve_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * VISibility LAST command external functions declaration
 */
extern sh_sand_man_t dnx_pp_last_man;
extern shr_error_e sh_dnx_pp_last_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * VISibility Context command external functions declaration
 */
extern sh_sand_option_t dnx_pp_context_options[];
extern sh_sand_man_t dnx_pp_context_man;
extern shr_error_e sh_dnx_pp_context_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*************
 * FUNCTIONS *
 *************/

shr_error_e
diag_pp_check_core_validity(
    int unit,
    int core_id,
    uint8 check_ingress,
    uint8 *core_is_valid)
{
    rhlist_t *sig_list;
    signal_output_t *signal_output;

    SHR_FUNC_INIT_VARS(unit);

    sig_list = NULL;
    if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
    }

    /** take the signal either from Ingress or Egress */
    if (check_ingress)
    {
        SHR_IF_ERR_EXIT_NO_MSG(sand_signal_find(unit, core_id, 0, "", "VTT1", "VTT2", "Context", &sig_list));
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(sand_signal_find(unit, core_id, 0, "", "PRP2", "Term", "Context", &sig_list));
    }

    signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
    if (sal_strcmp(signal_output->print_value, "N/A") == 0)
    {
        *core_is_valid = 0;
    }
    else
    {
        *core_is_valid = 1;
    }
    utilex_rhlist_free_all(sig_list);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_pp_find_ingress_valid_core(
    int unit,
    int *valid_core_id)
{
    rhlist_t *sig_list;
    signal_output_t *signal_output;
    int core_idx;

    SHR_FUNC_INIT_VARS(unit);

    /** set initial value that indicates that no core is available */
    *valid_core_id = -1;

    sig_list = NULL;
    if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
    }

    for (core_idx = 0; core_idx < dnx_data_device.general.nof_cores_get(unit); core_idx++)
    {
        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_idx, 0, "", "VTT1", "VTT2", "Context", &sig_list));

        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);

        if ((signal_output != NULL) && (sal_strcmp(signal_output->print_value, "N/A") != 0))
        {
            *valid_core_id = core_idx;
            break;
        }
    }

    utilex_rhlist_free_all(sig_list);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sig_get_to_rhlist(
    int unit,
    int core_id,
    char *block,
    char *from,
    char *to,
    char *name,
    signal_output_t ** list_element,
    rhlist_t ** rhlist)
{
    int rc;

    SHR_FUNC_INIT_VARS(unit);

    *rhlist = NULL;

    if ((*rhlist = utilex_rhlist_create(name, sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for %s info list creation !\n", name);
    }

    rc = sand_signal_find(unit, core_id, 0, block, from, to, name, rhlist);

    /** manage the Errors - if not found, don't output error message but return an empty list */
    if (rc == _SHR_E_NOT_FOUND)
    {
        *rhlist = NULL;
        *list_element = NULL;
        SHR_EXIT();
    }
    else if (rc != 0)
    {
        SHR_ERR_EXIT(rc, "Error in sand_signal_find with - block %s, from %s, signal %s  !\n", block, from, name);
    }
    else
    {
        *list_element = (signal_output_t *) utilex_rhlist_entry_get_first(*rhlist);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sig_get_to_value(
    int unit,
    int core_id,
    char *block,
    char *from,
    char *to,
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    char *result_str,
    int *size,
    rhlist_t ** signal_rhlist,
    rhlist_t ** base_rhlist_to_free)
{

        /** this list is not creating any elements, so it's not freed */
    signal_output_t *rhlist_element;

        /** last index of sub field in the array */
    int sig_name_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sig_name_hieararchy[0], _SHR_E_PARAM, "base signal name");

        /** get the first rhlist from base signal name */
    SHR_IF_ERR_EXIT(sig_get_to_rhlist
                    (unit, core_id, block, from, to, sig_name_hieararchy[0], &rhlist_element, base_rhlist_to_free));

        /** if the returned list is NULL, i.e signal not found - NULL the returned lists, value to '-', size to '0' and exit*/
    if (*base_rhlist_to_free == NULL)
    {
        sal_strncpy(result_str, "-", DSIG_MAX_SIZE_STR - 1);
        *size = 0;
        *signal_rhlist = NULL;
        SHR_EXIT();
    }

        /** the initial point is that both pointers set to basic command rhlist*/
    *signal_rhlist = rhlist_element->field_list;

        /** keep advancing in the hierarchies while the sub field is not NULL */
    for (sig_name_idx = 1; sig_name_idx < SH_DNX_PP_MAX_SIGNAL_HIERARCHY; sig_name_idx++)
    {
        if (sig_name_hieararchy[sig_name_idx] != NULL)
        {
            rhlist_element =
                (signal_output_t *) utilex_rhlist_entry_get_by_name(rhlist_element->field_list,
                                                                    sig_name_hieararchy[sig_name_idx]);

            if (rhlist_element == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
            }

                /** after locating the correct element from rhlist by name, we send its sub rhlist */
            *signal_rhlist = rhlist_element->field_list;

        }
        else
        {
            break;
        }

    }

    sal_strncpy(result_str, rhlist_element->print_value, DSIG_MAX_SIZE_STR - 1);
    *size = rhlist_element->size;

exit:
    SHR_FUNC_EXIT;
}

int
string_in_array(
    char *str,
    uint8 exact_match,
    char **str_arr)
{

    int index = 0;

    while (sal_strcmp(*str_arr, "") != -0)
    {
        if (exact_match)
        {
            /** exact match */
            if (sal_strcmp(str, *str_arr) == 0)
            {
                return index;
            }
        }
        else
        {
            /** str is contained in str_arr */
            if (sal_strstr(str, *str_arr))
            {
                return index;
            }
        }

        str_arr++;
        index++;
    }

    /** if came to this point, the str was not found in the array*/
    return -1;
}

shr_error_e
construct_packet_structure(
    int unit,
    rhlist_t * layer_list,
    char *packet_struct_str,
    char next_protocol_array[SH_DNX_PP_MAX_LAYERS_NUMBER][RHNAME_MAX_SIZE],
    char mpls_label_array[SH_DNX_PP_MAX_LAYERS_NUMBER][DSIG_MAX_SIZE_UINT32])
{
    int next_protocol_idx = 0, mpls_idx = 0;
    signal_output_t *layer_list_element;
    signal_output_t *mpls_list_element;

    SHR_FUNC_INIT_VARS(unit);

    /** zero the packet structure string */
    sal_memset(packet_struct_str, 0, sal_strlen(packet_struct_str));

    /*
     * iterate through the layers and built the string of the packet structure
     */
    RHITERATOR(layer_list_element, layer_list)
    {

        /** current layer name */
        char *layer_name = SH_DNX_PP_GET_SIG_EXP_NAME(layer_list_element);

        /** previous string without current layer*/
        char previous_str[DSIG_MAX_SIZE_STR];

        sal_strncpy(previous_str, packet_struct_str, DSIG_MAX_SIZE_STR - 1);

        /** append the current layer at the beginning of the previous layer stack */
        sal_strncpy(packet_struct_str, layer_name, DSIG_MAX_SIZE_STR - 1);
        sal_strncat(packet_struct_str, "o", DSIG_MAX_SIZE_STR - 1);
        sal_strncat(packet_struct_str, previous_str, DSIG_MAX_SIZE_STR - 1);

        /** if the layer is not sub layer add to linked list of layers */
        if (string_in_array(layer_name, FALSE, sh_dnx_pp_sub_layer_names) == -1)
        {
            /** push into next protocol array the layer_name and increase the next protocol idx */
            sal_strncpy(next_protocol_array[next_protocol_idx], layer_name, RHNAME_MAX_SIZE - 1);
            next_protocol_idx++;
            /** mark the next string as NULL to mark end of layers */
            next_protocol_array[next_protocol_idx][0] = '\0';

            if (mpls_label_array && (sal_strcmp(layer_name, "MPLS") == 0))
            {

                RHITERATOR(mpls_list_element, layer_list_element->field_list)
                {
                    char *mpls_attr_name = SH_DNX_PP_GET_SIG_EXP_NAME(mpls_list_element);
                    if (sal_strcmp(mpls_attr_name, "Label") == 0)
                    {
                        sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, (uint32 *) &mpls_list_element->value,
                                                 mpls_label_array[mpls_idx], 20, PRINT_BIG_ENDIAN);
                        mpls_idx++;
                    }
                }
            }
        }

    } /** of RHITERATOR */

    /** strip last characters which is an 'o'*/
    packet_struct_str[sal_strlen(packet_struct_str) - 1] = 0;

    SHR_FUNC_EXIT;
}

/**
 * \brief - function to drill down the hierarchy of the signal and accumulate in a
 *          result string the name/value or size - usually used to print a whole hierarchy
 *          in a single cell of a table
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] field_list - list of fields to print, and drill down each if it has down hierarchy
 *   \param [in] shifts - number of shifts (aka. tabs) to be used for hierarchial printing - relevant for only signal name case (nvs = 0)
 *   \param [in] nvs - Name/Value/Size/Offset - 0/1/2/3 respectively
 *   \param [in] print_result_accum - in case of a multicolumn table, return the appended recursive name string to print in specific column
 *   \param [in] skip_zero_value - if set, won't print values that are '0' - relevant only for value case (nvs = 1)
 *   \param [in] tie_name_to_value - in case of value, this will tie a name prefix if set
 *   \param [in] sig_offset - offset to which to add in case offset is of interest (nvs = 3)
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
recursive_sig_hierarchy_iterator(
    int unit,
    rhlist_t * field_list,
    int shifts,
    uint8 nvs,
    char *print_result_accum,
    uint8 skip_zero_value,
    uint8 tie_name_to_value,
    int sig_offset)
{

    signal_output_t *field_list_element;

    SHR_FUNC_INIT_VARS(unit);

     /** exit condition the field_list is empty (NULL) */
    if (field_list == NULL)
    {
        SHR_EXIT();
    }

    field_list_element = (signal_output_t *) utilex_rhlist_entry_get_first(field_list);

    RHITERATOR(field_list_element, field_list)
    {

        if (!ISEMPTY(print_result_accum))
        {
            sal_strncat(print_result_accum, "\n", DSIG_MAX_SIZE_STR - 1);
        }

         /** name printing should occur either when nvs == 0 or that nvs == 1 with tie_name_to_value */
        if ((nvs == 0) || tie_name_to_value)
        {
            char *name = SH_DNX_PP_GET_SIG_EXP_NAME(field_list_element);
            int tab_idx;

             /** adding spaces instead of tabs for hierarchy print in case of name accumulation */
            for (tab_idx = 0; tab_idx < shifts; tab_idx++)
            {
                sal_strncat(print_result_accum, " ", DSIG_MAX_SIZE_STR - 1);
            }

            sal_strncat(print_result_accum, name, DSIG_MAX_SIZE_STR - 1);

             /** add a splitter because the value should be also printed */
            if (tie_name_to_value)
            {
                sal_strncat(print_result_accum, ": ", DSIG_MAX_SIZE_STR - 1);
            }
        }

         /** don't use else if - because it could be nvs == 1 with tie_name_to_value so the 2 if should occur */

        if (nvs == 1)
        {

            uint32 signal_numeric_value = 0;
            uint8 value_is_number;

            value_is_number = utilex_str_is_number(field_list_element->print_value);

            if (value_is_number)
            {
                /** convert the received subfield value to numeric */
                SHR_IF_ERR_EXIT(utilex_str_xstoul(field_list_element->print_value, &signal_numeric_value));
            }

            /*
             * if value is numeric and equals 0 and the skip_zero_value is set, don't print this row of value 
             */
            if ( !value_is_number || (skip_zero_value == FALSE) || (value_is_number && signal_numeric_value != 0))
            {

                  /** differentiate between a numeric number or string */
                sal_strncat(print_result_accum, field_list_element->print_value, DSIG_MAX_SIZE_STR - 1);

            }
            else
            {
                  /** remove the line break that is performed automatically at each RHITERATION  since we skip this value line */
                print_result_accum[strlen(print_result_accum) - 1] = '\0';
            }   /** of if rc */

        }
        else if (nvs == 2)
        {
            /** size is of interest */
            char size_str[RHNAME_MAX_SIZE];

            sal_itoa(size_str, field_list_element->size, 10, 0, 0);
            sal_strncat(print_result_accum, size_str, DSIG_MAX_SIZE_STR - 1);
        }
        else if (nvs == 3)
        {
            /** offset is of interest */
            char size_str[RHNAME_MAX_SIZE];

            sal_itoa(size_str, sig_offset, 10, 0, 0);
            sal_strncat(print_result_accum, size_str, DSIG_MAX_SIZE_STR - 1);
        }
        else if (nvs != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NVS value is wrong - not [0..3] : %d !\n", nvs);
        }

        /*
         * Call recursive function to continue printing down the hierarchy of the layer record don't update the offset
         * for deeper hieararchy, only at the RHITERATOR level 
         */
        SHR_IF_ERR_EXIT(recursive_sig_hierarchy_iterator
                        (unit, field_list_element->field_list, shifts + 1, nvs, print_result_accum, skip_zero_value,
                         tie_name_to_value, sig_offset));

        /** update the sig_offset for next RHITERATOR */
        sig_offset = sig_offset + field_list_element->size;

    }  /** of RHITERATOR */

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
get_sig_hierarchy_str(
    int unit,
    int core_id,
    char *block,
    char *from,
    char *to,
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    char print_name[RHNAME_MAX_SIZE],
    int nvs,
    char result_hierarchial_str[DSIG_MAX_SIZE_STR],
    uint8 skip_zero_value,
    uint8 include_first_hierarchy,
    uint8 tie_name_to_value,
    int sig_offset)
{

    /** signal value variables */
    char print_value[DSIG_MAX_SIZE_STR];
    int size;
    /** the signal list is a sub list in base_rhlist_to_free, there for only the latter needs freeing in the end */
    rhlist_t *signal_sub_rhlist = NULL;
    rhlist_t *base_rhlist_to_free = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(print_name, _SHR_E_PARAM, "print name");

     /** Get the value and size of the signal from sig get */
    SHR_IF_ERR_EXIT(sig_get_to_value
                    (unit, core_id, block, from, "", sig_name_hieararchy, print_value, &size, &signal_sub_rhlist,
                     &base_rhlist_to_free));

     /** zero the result string initially */
    result_hierarchial_str[0] = '\0';

     /** only if the first hierarchy is to be included */
    if (include_first_hierarchy)
    {

        /*
         * if the signal wasn't found, i.e the rhlist is NULL, exit and return an '-' in the result string and exit 
         */

        if (base_rhlist_to_free == NULL)
        {
            sal_strncpy(result_hierarchial_str, "-", DSIG_MAX_SIZE_STR - 1);
            SHR_EXIT();
        }

         /** apply addition of name/value/size */
        if (nvs == 0)
        {
            sal_strncpy(result_hierarchial_str, print_name, DSIG_MAX_SIZE_STR - 1);
        }
        else if (nvs == 1)
        {
             /** this value is formatted to hex if it's a number */
            char print_value_formatted[DSIG_MAX_SIZE_STR] = "";

             /** differentiate between a numeric number or string */
            sal_strncat(print_value_formatted, print_value, DSIG_MAX_SIZE_STR - 1);

            sal_strncpy(result_hierarchial_str, print_value_formatted, DSIG_MAX_SIZE_STR - 1);
        }
        else if (nvs == 2)
        {
             /** case the size is of interest, convert size to string */
            sal_itoa(result_hierarchial_str, size, 10, 0, 0);
        }
        else if (nvs == 3)
        {
            /** case that offset is of interest, initial offset was the input offset */

            sal_itoa(result_hierarchial_str, sig_offset, 10, 0, 0);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NVS value is wrong - not [0..3] : %d !\n", nvs);
        }

    }  /** of if  skip_first_hierarchy */

     /** proceed hierarchical string accumulation of the signal, but don't add the header size to the offset */
    SHR_IF_ERR_EXIT(recursive_sig_hierarchy_iterator
                    (unit, signal_sub_rhlist, 1, nvs, result_hierarchial_str,
                     skip_zero_value, tie_name_to_value, sig_offset));

     /** there can be a possibility of empty pointer if signal not found - so check before release */
    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

exit:

    /** don't leave a string empty, if it is make it '-' */
    if (result_hierarchial_str[0] == '\0')
    {
        sal_strncpy(result_hierarchial_str, "-", DSIG_MAX_SIZE_STR - 1);
    }

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

    SHR_FUNC_EXIT;
}

shr_error_e
recursive_field_print_iterator(
    int unit,
    rhlist_t * field_list,
    prt_control_t * prt_ctr,
    int shifts,
    char *print_name)
{
    signal_output_t *field_list_element = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** exit condition the field_list is empty (NULL) */
    if (field_list == NULL)
    {
        SHR_EXIT();
    }

    RHITERATOR(field_list_element, field_list)
    {
        char *name = RHNAME(field_list_element);

        /** if suggested print name is not NULL, replace the name of the signal in the table */
        if (print_name != NULL)
        {
            name = print_name;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /** add number of tab shifts that fits the hierarchy */
        PRT_CELL_SET_SHIFT(shifts, "%s", name);

        /** Add value and size */
        PRT_CELL_SET("%s", field_list_element->print_value);
        PRT_CELL_SET("%3d", field_list_element->size);

        /** call recursive function to continue printing down the hierarchy of the layer record */
        SHR_IF_ERR_EXIT(recursive_field_print_iterator
                        (unit, field_list_element->field_list, prt_ctr, shifts + 1, NULL));

    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
print_sig_in_table(
    int unit,
    int core_id,
    char *block,
    char *from,
    char *to,
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    char *cond_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    uint32 condition_field_expected_value,
    char print_name[RHNAME_MAX_SIZE],
    prt_control_t * prt_ctr)
{

    /** signal value variables */
    char print_value[DSIG_MAX_SIZE_STR];
    /** this value is formatted to hex if it's a number */
    char print_value_formatted[DSIG_MAX_SIZE_STR] = "";
    int size;
    /** the signal list is a sub list in base_rhlist_to_free, there for only the latter needs freeing in the end */
    rhlist_t *signal_sub_rhlist = NULL;
    rhlist_t *base_rhlist_to_free = NULL;

    /** condition value variables */
    char condition_print_value[DSIG_MAX_SIZE_STR];
    uint32 condition_received_numeric_value = 0;
    uint8 cond_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(print_name, _SHR_E_PARAM, "print name");

    SHR_NULL_CHECK(sig_name_hieararchy[0], _SHR_E_PARAM, "base signal name");

    /*
     * Check if condition value matches it's passing value
     */

    /** condition is considered invalid either if its pointer is NULL, or its first string */
    if (cond_name_hieararchy != NULL && cond_name_hieararchy[0] != NULL)
    {
        /** get the condition value by applying the cond name hierarchy */
        SHR_IF_ERR_EXIT(sig_get_to_value
                        (unit, core_id, block, from, "", cond_name_hieararchy, condition_print_value, &size,
                         &signal_sub_rhlist, &base_rhlist_to_free));

        SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

        /** convert the received subfield value to numeric */

        if(utilex_str_is_number(condition_print_value) == FALSE)
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "The string:%s is non-numeric !\n", condition_print_value);
        }

        SHR_IF_ERR_EXIT(utilex_str_xstoul(condition_print_value, &condition_received_numeric_value));

        /** if condition field doesn't match the passing value skip to exit label */
        if (condition_received_numeric_value != condition_field_expected_value)
        {
            cond_valid = FALSE;
        }

    }

    /*
     * we print the element's value and later use its sub list to proceed printing
     * IN CASE the signal is NOT found the returned lists are NULL, print_value = '-' and size = '0'
     */

    /** if the condition to show the signal wasn't met, change it's value to N/A */
    if (cond_valid)
    {
        /** Get the value and size of the signal from sig get only if it was valid */
        SHR_IF_ERR_EXIT(sig_get_to_value
                        (unit, core_id, block, from, "", sig_name_hieararchy, print_value, &size, &signal_sub_rhlist,
                         &base_rhlist_to_free));

        /** differentiate between a numeric number or string */
        sal_strncat(print_value_formatted, print_value, DSIG_MAX_SIZE_STR - 1);
    }
    else
    {
        sal_strncpy(print_value_formatted, "N/A", DSIG_MAX_SIZE_STR - 1);
        size = 0;
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", print_name);
    PRT_CELL_SET("%s", print_value_formatted);
    PRT_CELL_SET("%3d", size);

    /** proceed hierarchical print of the signal with only if the condition for the signal was met */
    if (cond_valid)
    {
        SHR_IF_ERR_EXIT(recursive_field_print_iterator(unit, signal_sub_rhlist, prt_ctr, 1, NULL));
    }

    /** there can be a possibility of empty pointer if signal not found - so check before release */

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

exit:

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);

    SHR_FUNC_EXIT;

}

void
set_core_low_high(
    int unit,
    int core,
    int *core_low,
    int *core_high)
{
    if (core >= 0 && core <= dnx_data_device.general.nof_cores_get(unit) - 1)
    {
        *core_low = core;
        *core_high = core;
    }
    else
    {
        *core_low = 0;
        *core_high = dnx_data_device.general.nof_cores_get(unit) - 1;
    }

/** In cmodel force to use core_id = 0 */
#ifdef ADAPTER_SERVER_MODE
    *core_low = 0;
    *core_high = 0;
#endif

}

/**
 * \brief DNX PP diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for PP diagnostic commands
 */

sh_sand_man_t sh_dnx_pp_man = {
    .brief = "PP Diagnostic - shows relevant information after a packet traverse.\n",
    .full = NULL
};

static sh_sand_man_t sh_dnx_pp_vis_sub_man = {
    .brief = "Show last packet relevant PP information",
    .full = "Show last packet relevant PP information",
};

static sh_sand_man_t sh_dnx_pp_info_sub_man = {
    .brief = "Provide PP thorough information and configuration in the required aspect",
    .full = "Provide PP thorough information and configuration in the required aspect",
};

static sh_sand_cmd_t sh_dnx_pp_vis_sub_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
    ********************************************************************************************************************************/
    {"PacketParsingInfo", sh_dnx_pp_ppi_cmds, NULL, dnx_pp_ppi_options, &dnx_pp_ppi_man},
    {"PacKeTTM", sh_dnx_pp_pkttm_cmds, NULL, dnx_pp_pkttm_options, &dnx_pp_pkttm_man},
    {"ForwardDecisionTrace", sh_dnx_pp_fdt_cmds, NULL, dnx_pp_fdt_options, &dnx_pp_fdt_man},
    {"FEC", sh_dnx_pp_vis_fec_cmds, NULL, dnx_pp_vis_fec_man_options, &dnx_pp_vis_fec_man},
    {"SYS_Header", sh_dnx_pp_sh_cmds, NULL, dnx_pp_sh_options, &dnx_pp_sh_man},
    {"TERMination", sh_dnx_pp_termi_cmds, NULL, dnx_pp_termi_options, &dnx_pp_termi_man},
    {"IngressVlanEditing", sh_dnx_pp_ive_cmds, NULL, dnx_pp_ive_options, &dnx_pp_ive_man},
    {"EgressVlanEditing", sh_dnx_pp_eve_cmds, NULL, dnx_pp_eve_options, &dnx_pp_eve_man},
    {"LAST", sh_dnx_pp_last_cmds, NULL, NULL, &dnx_pp_last_man},
    {"CONTEXT", sh_dnx_pp_context_cmd, NULL, dnx_pp_context_options, &dnx_pp_context_man},
    {"OCCupation", NULL, sh_dnx_pp_occ_sub_cmd, NULL, &sh_dnx_pp_occ_sub_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_pp_res_sub_man = {
    .brief = "Provide resource information",
    .full = "Provide resource information",
};

static sh_sand_cmd_t sh_dnx_pp_info_sub_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
    ********************************************************************************************************************************/
    {"FEC", sh_dnx_pp_info_fec_cmds, NULL, dnx_pp_info_fec_man_options, &dnx_pp_info_fec_man},

    {NULL}
};

static sh_sand_cmd_t sh_dnx_pp_res_sub_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
    ********************************************************************************************************************************/
    {NULL}

};

sh_sand_cmd_t sh_dnx_pp_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
    ********************************************************************************************************************************/

    {"VISibility", NULL, sh_dnx_pp_vis_sub_cmds, NULL, &sh_dnx_pp_vis_sub_man},
    {"Info", NULL, sh_dnx_pp_info_sub_cmds, NULL, &sh_dnx_pp_info_sub_man},
    {"ReSouRCe", NULL, sh_dnx_pp_res_sub_cmds, NULL, &sh_dnx_pp_res_sub_man},
    {NULL}

};
