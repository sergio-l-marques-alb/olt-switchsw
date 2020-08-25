/** \file diag_dnx_pp_fec.c
 *
 * FEC PP Diagnostic Command - Retrieves either all FECs in the system,
 * or FECs that were hit in last packet traverse.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/l3/l3_fec.h>

/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

dbal_tables_e fec_hierarchy_tables[] =
    { DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY, DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY };

/** this array corresponds to the SUPER_FEC result type and describes which gerenral FEC fields is used */
dbal_fields_e fec_general_entry_dbal_fields[] =
    { DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_NO_STATS, DBAL_FIELD_FEC_ENTRY_W_PROTECTION_W_1_STATS,
    DBAL_FIELD_FEC_ENTRY_W_PROTECTION_NO_STATS, DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_W_2_STATS
};

/*************
 *  MACROS  *
 *************/

#define L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(sup_fec_id, instance) ((sup_fec_id << 1) | instance)

/*************
 * TYPEDEFS  *
 *************/

/** define the table which represents fec's inner struct values */
typedef struct table_fec_structs
{
    dbal_fields_e struct_id;
    uint32 struct_value;
    uint8 struct_present;
} table_fec_structs_t;

/** \brief List of Table types per FEC hierarchy */
typedef enum
{
    NO_PROTECTION_TABLE,
    W_PROTECTION_TABLE,

    NOF_FEC_TABLE_TYPES
} table_ids_t;

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - Function prints the columns of 2 tables - FECs with Protection and FECs with no Protection
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] prt_ctr
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
print_fec_coulmns(
    int unit,
    prt_control_t * prt_ctr[])
{

    table_ids_t table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
    {
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "FEC_Id");

        /** print these columns only in the protection FEC table */
        if (table_id == W_PROTECTION_TABLE)
        {
            PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Inst");
            PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Protect");
        }

        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Stat_Id");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Stat_Cmd");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Dest");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "MC_RPF_Mode");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "EEI");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Hierarch_TM_Flow");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Glob_Out_LIF");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Glob_Out_LIF_2nd");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function fills the fec tables with its values per a SUPER_FEC - for
 *          the 2 FEC instances in there, in parallel to 2 tables - FECs with Protections and FECs without
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] super_fec_id
 *   \param [in] fec_hierarchy
 *   \param [in] prt_ctr
 *   \param [in] first_row_separator - represents which separator to use before the first row
 *   \param [in] instance_id - if '-1' print both, otherwise print only one
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
fill_fec_table(
    int unit,
    uint32 super_fec_id,
    uint32 fec_hierarchy,
    prt_control_t * prt_ctr[],
    PRT_ROW_SEP_MODE first_row_separator,
    int instance_id)
{

#define PRIMARY_FEC_INST 0

    int fec_instance;

    uint32 entry_handle_id;

    /** array of pointers to Boolean for Primary and Secondary FECs */
    uint8 is_allocated_arr[2];
    uint8 is_allocated;
    uint8 prime_protected = FALSE;

    uint32 protection_pointer = 0;

    int low_instance, high_instance;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** gather information on allocated and protected (only prime has protection indication) */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated
                    (unit, L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, 0), &is_allocated));
    is_allocated_arr[0] = is_allocated;
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated
                    (unit, L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, 1), &is_allocated));
    is_allocated_arr[1] = is_allocated;
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get
                    (unit, L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, 0), &prime_protected));

    /** if both of the FEC instances aren't allocated there's nothing to do here (as in case where a Core isn't used) */
    if ((!is_allocated_arr[0]) && (!is_allocated_arr[1]))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fec_hierarchy_tables[fec_hierarchy], &entry_handle_id));

    /** use SUPER_FEC of the first 19bits from FEC_ID as key to access the table */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, super_fec_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (instance_id >= 0 && instance_id <= 1)
    {
        low_instance = instance_id;
        high_instance = instance_id;
    }
    else if (instance_id == -1)
    {
        low_instance = 0;
        high_instance = 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC instance: %d is not within 0-1 !\n", instance_id);
    }

    /** iterate print for both FEC instances in the SUPER_FEC - 1 is the Primary, 0 is secondary  */
    for (fec_instance = low_instance; fec_instance <= high_instance; fec_instance++)
    {

        uint32 fec_general_entry_result[3] = { 0 };
        uint32 fec_entry_value[3];

        dbal_fields_e fec_entry_type;

        /** this array corresponds to fec_general_entry_dbal_fields and describes for each in SUPER_FEC how many satistics supported */
        int statistics_support_arr[] = { 0, 1, 0, 2 };

        uint32 result_type;

        int low_level_field_idx;

        /** this is actually a simple uint32, but due to coverity, this variable is used as array in dbal function
         *  so defining as an array
         */
        uint32 struct_field_value[1];

        dbal_field_types_basic_info_t *low_level_fec_struct;

        /** define the table which represents fec's inner struct values */
        table_fec_structs_t table_fec_values[] = {
            {DBAL_FIELD_DESTINATION, 0, FALSE},
            {DBAL_FIELD_MC_RPF_MODE, 0, FALSE},
            {DBAL_FIELD_EEI, 0, FALSE},
            {DBAL_FIELD_HIERARCHICAL_TM_FLOW, 0, FALSE},
            {DBAL_FIELD_GLOB_OUT_LIF, 0, FALSE},
            {DBAL_FIELD_GLOB_OUT_LIF_2ND, 0, FALSE},
        };

        int table_struct_size = sizeof(table_fec_values) / sizeof(table_fec_values[0]);

        int table_struct_idx;

        int fec_id = L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, fec_instance);

        /** decide which table id, default is no protection table */
        table_ids_t table_id = NO_PROTECTION_TABLE;

        /** if the instance hasn't been allocated skip it */
        if (is_allocated_arr[fec_instance] == FALSE)
        {
            continue;
        }

        /*
         *  Direct the table in which to print according to allocation and protection flags
         *  at this point we know the FEC is allocated
         */

        if (prime_protected)
        {
            /** no matter if we're at primary or secondary - if the primary protected both be printed in protection table */
            table_id = W_PROTECTION_TABLE;
        }

        /** decide which separator to use before the first line - allow a line only in a protected table */
        if ((fec_instance == 0) && (prime_protected))
        {
            PRT_ROW_ADD_PL(table_id, first_row_separator);
        }
        else
        {
            PRT_ROW_ADD_PL(table_id, PRT_ROW_SEP_NONE);
        }

         /** retrive the result type */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

        /** print the FEC_ID - this is the SUPER_FEC + fec_instance bit */
        PRT_CELL_SET_PL(table_id, "%d", fec_id);

        if (prime_protected)
        {

            /** print the Instance - Primary/Secondary */
            PRT_CELL_SET_PL(table_id, "%s", (fec_instance == PRIMARY_FEC_INST) ? "Primary" : "Secondary");

            /** only the Primary contains the protection */
            if (fec_instance == PRIMARY_FEC_INST)
            {
                /** get the FEC Protection entry result */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, fec_instance,
                                 &protection_pointer));
            }

            PRT_CELL_SET_PL(table_id, "%d", protection_pointer);
        }

        /** print the statistics according to support of the result type - 0/1/2 statistics availability */
        if ((statistics_support_arr[result_type] == 2) ||
            (statistics_support_arr[result_type] == 1 && fec_instance == PRIMARY_FEC_INST))
        {
            uint32 stat_id, stat_cmd;

           /** get the FEC Statistics entry result */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, fec_instance, &stat_id));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, fec_instance, &stat_cmd));

            PRT_CELL_SET_PL(table_id, "%d", stat_id);
            PRT_CELL_SET_PL(table_id, "%d", stat_cmd);
        }
        else
        {
            PRT_CELL_SET_PL(table_id, "-");
            PRT_CELL_SET_PL(table_id, "-");
        }

        /*
         **************** PRINT THE FEC INNER STRUCTURES ***********
         */

        /*
         * according to each result_type, choose the correct field where fec_id is present
         * each result contains x2 FECs, so the LSB bit 0 will decide which FEC instance to read
         */

        /** get the fec general entry result - which can contain multiple options */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, fec_general_entry_dbal_fields[result_type], fec_instance,
                         fec_general_entry_result));

        /** understand the specific sub fec entry type from the general fec entry */
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                        (unit, fec_general_entry_dbal_fields[result_type], fec_general_entry_result, &fec_entry_type,
                         fec_entry_value));

        /** get the inner low level fields of fecs by telling its type (given in array form) */
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, fec_entry_type, &low_level_fec_struct));

        /**  iterate through the inner structure array */
        for (low_level_field_idx = 0; low_level_field_idx < low_level_fec_struct->nof_struct_fields;
             low_level_field_idx++)
        {

            /** get the value of the lower level struct */
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type,
                                                            low_level_fec_struct->struct_field_info
                                                            [low_level_field_idx].struct_field_id, struct_field_value,
                                                            fec_entry_value));
            /*
             * build the array that represents the table
             */

            /** iterate through the possible struct field types and compare to above type */
            for (table_struct_idx = 0; table_struct_idx < table_struct_size; table_struct_idx++)
            {
                if (table_fec_values[table_struct_idx].struct_id ==
                    low_level_fec_struct->struct_field_info[low_level_field_idx].struct_field_id)
                {
                    table_fec_values[table_struct_idx].struct_value = struct_field_value[0];
                    table_fec_values[table_struct_idx].struct_present = TRUE;
                    break;
                }
            } /** of for on table values index */

        } /** of for on inner structure of the fec */

        /** iterate throught the table struct values and print out */
        for (table_struct_idx = 0; table_struct_idx < table_struct_size; table_struct_idx++)
        {
            if (table_fec_values[table_struct_idx].struct_present)
            {
                /** destination field requires a parsing by DBAL */
                if (table_fec_values[table_struct_idx].struct_id == DBAL_FIELD_DESTINATION)
                {
                    char parsed_dest_print[DSIG_MAX_SIZE_STR];
                    /** setting a pointer to get a const str from the function, to be used in dbal from hw value get */
                    char dest_field_str[RHNAME_MAX_SIZE];

                    sal_strncpy(dest_field_str,
                                dbal_field_to_string(unit, table_fec_values[table_struct_idx].struct_id),
                                RHNAME_MAX_SIZE - 1);

                    dnx_dbal_fields_string_form_hw_value_get(unit, dest_field_str,
                                                             table_fec_values[table_struct_idx].struct_value,
                                                             parsed_dest_print);

                    PRT_CELL_SET_PL(table_id, "%s", parsed_dest_print);
                }
                else
                {
                    PRT_CELL_SET_PL(table_id, "%d", table_fec_values[table_struct_idx].struct_value);
                }
            }
            else
            {
                PRT_CELL_SET_PL(table_id, "-");
            } /** of if struct is present */

        } /** of for on table values index */

    } /** of for fec_instance */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Provides fwd_action_dst value per requested stage
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id
 *   \param [in] short_mode - flag to indicate that SHORT flag appeared in the BCM shell
 *   \param [in] stage_name - the stage name from which to take the fwd_action_dst signal value
 *   \param [out] dest_value - the fwd_action_dst signal value
 *   \param [out] dest_is_numeric - signal if the value is numeric or not, non numeric may indicate that packet wasn't run
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
diag_dnx_pp_fec_get_fwd_action_dest(
    int unit,
    int core_id,
    uint8 short_mode,
    char *stage_name,
    uint32 *dest_value,
    uint8 *dest_is_numeric)
{

    rhlist_t *base_rhlist_to_free = NULL;

    /** signal value variables */
    char dest_print[DSIG_MAX_SIZE_STR] = "";
    int size;
    /** the signal list is a sub list in base_rhlist_to_free, therefore only the latter needs freeing in the end */
    rhlist_t *signal_sub_rhlist = NULL;

    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "fwd_action_dst", NULL };

    int rc;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve the forward action destination */
    SHR_IF_ERR_EXIT(sig_get_to_value
                    (unit, core_id, "", stage_name, "", sig_name_hieararchy, dest_print, &size, &signal_sub_rhlist,
                     &base_rhlist_to_free));

    if (base_rhlist_to_free != NULL)
    {
        utilex_rhlist_free_all(base_rhlist_to_free);
        base_rhlist_to_free = NULL;
    }
    else
    {
        /** exit with print message only in case on non-SHORT mode */
        if (!short_mode)
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "couldn't retrieve the forward action destination signal from %s stage !\n",
                         stage_name);
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "");
        }

    }

    /*
     *  Convert the received subfield value to numeric
     *  if the signal is not valid - in case a Core_Id didn't run a packet - skip the loop for it
     */
    *dest_value = 0;
    *dest_is_numeric = TRUE;
    rc = (utilex_str_xstoul(dest_print, dest_value));

    if (rc != 0)
    {
        *dest_is_numeric = FALSE;
    }

exit:

    SH_DNX_PP_LIST_RELEASE_IF_NOTNULL(base_rhlist_to_free);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Provides the FEC diagnostics
 *          Supports two modes - dumps ALL FEC entries or the LAST FEC hits in the system
 ***************************************************************************************************************
 *  This function supports 2 different PP shell commands: PP Info FEC (all), PP VIS FEC (last)
 *  The (all) and (last) arguments are assumed to be given in each cased,
 *  so that this function can separate the cases
 ***************************************************************************************************************
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core_id id
 *   \param [in] all_print - flag to indicate that ALL FECs DB was asked by user
 *   \param [in] usr_hrc_idx - relevant only to ALL flag, the FECs Hierarchy of interest
 *   \param [in] last_print - flag to indicate that LAST packet FEC hits was asked by user
 *   \param [in] short_mode - flag to indicate that SHORT flag appeared in the BCM shell
 *   \param [in] sand_control
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_fec_cmds(
    int unit,
    int core,
    uint8 all_print,
    int usr_hrc_idx,
    uint8 last_print,
    uint8 short_mode,
    sh_sand_control_t * sand_control)
{
    int core_id, core_low, core_high;

    uint32 entry_handle_id;

    /** for ALL case there's no need to print twice for each core, it's same information */
    uint8 all_printed = FALSE;

    uint8 freed_tables[NOF_FEC_TABLE_TYPES] = { TRUE, TRUE };

    /** Allocate x2 Protection and Non Protection Table per each Hierarchy*/
    PRT_INIT_VARS_PL(NOF_FEC_TABLE_TYPES);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** determine if the user passed a correct core value. If a specific correct core was asked, iterate on it only, otherwise on 0,1 */
    set_core_low_high(unit, core, &core_low, &core_high);

    /** initial allocation for entry_handle_id so that re-use can be done on this handle with DBAL_HANDLE_CLEAR */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, &entry_handle_id));

    /** print per each core or the core that user has set */
    for (core_id = core_low; core_id <= core_high; core_id++)
    {
        table_ids_t table_id;

        uint8 core_is_valid;
        int rc;

        /** check if core_id is valid on the Ingress */
        rc = diag_pp_check_core_validity(unit, core_id, TRUE, &core_is_valid);
        if ((!core_is_valid) || (rc != _SHR_E_NONE))
        {
           /** in SHORT mode don't output any message, just skip*/
            if (!short_mode)
            {
                cli_out("Please run a packet through Core_Id=%d.\n", core_id);
            }
            continue;
        }

        if (all_print)
        {

            int hrc_low, hrc_high, loop_hrc_idx;

            /** for ALL case the information is same for each Core_ID, no need to do on both in the ALL FECs option */
            if (all_printed)
            {
                continue;
            }

            all_printed = TRUE;

            /** check validity of user input for FEC hierarchy to present for ALL FECs print */
            if ((usr_hrc_idx >= 0) && (usr_hrc_idx <= 2))
            {
                hrc_low = usr_hrc_idx;
                hrc_high = usr_hrc_idx;
            }
            else
            {
                hrc_low = 0;
                hrc_high = 2;
            }

            /** if user doesn't insert the usr_hrc_idx, it's value is 4, so loop will iterate on all 1-3 FEC hierarchies */
            for (loop_hrc_idx = hrc_low; loop_hrc_idx <= hrc_high; loop_hrc_idx++)
            {
                int is_end = 0;
                int loop_count = 0;

                /** start printing the table per each Hiararchy - with and without protection */
                PRT_TITLE_SET_PL(NO_PROTECTION_TABLE, "No Protection FECs Hierarchy %d, (Both Cores)", loop_hrc_idx);
                freed_tables[NO_PROTECTION_TABLE] = FALSE;

                PRT_TITLE_SET_PL(W_PROTECTION_TABLE, "Protection FECs Hierarchy %d, (Both Cores)", loop_hrc_idx);
                freed_tables[W_PROTECTION_TABLE] = FALSE;

                SHR_IF_ERR_EXIT(print_fec_coulmns(unit, prt_ctr));

                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, fec_hierarchy_tables[loop_hrc_idx], entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

                /** iterate on all valid entries of the table of this Hierarchy */
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                while (!is_end)
                {

                    uint32 super_fec_id;

                    /** get the current key of active entry - super_fec_id */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, &super_fec_id));

                    /** fill fec table - for first FEC entry print use no line, for next use a line before */
                    SHR_IF_ERR_EXIT(fill_fec_table(unit, super_fec_id, loop_hrc_idx, prt_ctr,
                                                   (loop_count ==
                                                    0) ? PRT_ROW_SEP_NONE : PRT_ROW_SEP_UNDERSCORE_BEFORE, -1));

                    /** move to the next valid entry in FEC */
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

                    loop_count++;

                } /** of while is_end */

                /** close the table per FEC only which is not empty */
                for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
                {
                    if (PRT_ROW_NUM_PL(table_id) > 0)
                    {
                        PRT_COMMITX_PL(table_id);
                    }
                    else
                    {
                        PRT_FREE_PL(table_id);
                        /** mark this table as freed */
                        freed_tables[table_id] = TRUE;
                    }
                }

            } /** of for on hierarchy index */

        }
        else if (last_print)
        {
            /** 'fwd_action_dst' signal value */
            uint32 dest_value = 0;
            uint8 dest_is_numeric;

            uint32 fec_id_value;

            dbal_fields_e dbal_dest_type;

            int loop_count = 0;

            /** to check that FECs point only to higher level hierarchies */
            int max_current_hrc = -1;

            /** to check if the FEC value is allocated (usually for cases where for Core no FECs defined) */
            uint8 fec_is_allocated;

            /** get fwd_action_dst value from FWD2 */
            SHR_IF_ERR_EXIT(diag_dnx_pp_fec_get_fwd_action_dest
                            (unit, core_id, short_mode, "FWD2", &dest_value, &dest_is_numeric));

            /** if the conversion of the FWD2 fwd_action_dst to number didn't succeed, this means the packet for this core wasn't run */
            if (!dest_is_numeric)
            {
                continue;
            }

            /*
             * Check if destination from FWD2 is FEC, by checking the destination DBAL encoding
             */
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                            (unit, DBAL_FIELD_DESTINATION, dest_value, &dbal_dest_type, &fec_id_value));

            /** check if FEC is allocated - mainly for cases where the destination value is '0' */
            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_id_value, &fec_is_allocated));

            /** now loop through the FEC's pointers until the next destination remains FEC */
            while ((dbal_dest_type == DBAL_FIELD_FEC) && (loop_count < 3) && (fec_is_allocated))
            {

                uint32 fec_hrc_result;
                uint32 result_type;

                uint32 fec_general_entry_result[3] = { 0 };

                /** 6bits from fec_id that determine the FEC hierarchy */
                uint32 sub_resource_id;

                /*
                 * Receive the ID of the sub-resource to which the FEC belongs, later to be used as 6bits for FEC hieararchy calculation
                 * divide the physical FEC to the number of physical FECs per sub-resource.
                 */
                sub_resource_id =
                    DNX_ALGO_L3_PHYSICAL_FEC_GET(unit,
                                                 fec_id_value) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);

                if (sub_resource_id > 29)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "the fec_id sub_resource_id is out of 0-29 range: %d!\n",
                                 sub_resource_id);
                }

                /*
                 * retrieve the FEC hierarchy by taking the initial 6bits of the FEC_ID and looking in dbal table
                 * the 6bits value in the table represents an instance number, which's value is the FEC hierarchy 1-3
                 */

                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FEC_BANK_TO_STAGE_MAP, entry_handle_id));

                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, sub_resource_id, &fec_hrc_result));

                /*
                 * start printing the table per each SUPER_FEC, only the FEC instance that was pointer
                 * starting both tables of protection and non protection - the empty will be deleted
                 */
                PRT_TITLE_SET_PL(NO_PROTECTION_TABLE, "No Protection FECs Hierarchy %d, Core Id=%d", fec_hrc_result,
                                 core_id);
                freed_tables[NO_PROTECTION_TABLE] = FALSE;

                PRT_TITLE_SET_PL(W_PROTECTION_TABLE, "Protection FECs Hierarchy %d, Core Id=%d", fec_hrc_result,
                                 core_id);
                freed_tables[W_PROTECTION_TABLE] = FALSE;

                SHR_IF_ERR_EXIT(print_fec_coulmns(unit, prt_ctr));

                /** casting uint32 to int so that -1 won't be interperted as ffff */
                if (max_current_hrc >= (int) fec_hrc_result)
                {
                    PRT_INFO_ADD_PL(NO_PROTECTION_TABLE,
                                    "This FEC_ID %d hierarchy %d was pointed from a higher hierarchy %d !",
                                    fec_id_value, fec_hrc_result, max_current_hrc);
                    PRT_INFO_ADD_PL(W_PROTECTION_TABLE,
                                    "This FEC_ID %d hierarchy %d was pointed from a higher hierarchy %d !",
                                    fec_id_value, fec_hrc_result, max_current_hrc);
                }
                else
                {
                    max_current_hrc = (int) fec_hrc_result;
                }

                /*
                 * at this point we know that destination was FEC
                 * now we know in which FEC hierarchy table to look
                 * we will now access to this FEC, read the new fec_id_value and repeat until finishing.
                 */

                /** fec_hrc_result valid values are 0-2, value - 3 means no hierarchy; no more than 3 times can this loop happen */
                if ((fec_hrc_result != 3))
                {

                    dbal_fields_e fec_entry_type;
                    uint32 fec_entry_value[3];

                    /** print the table for this FEC - both for Protection and Non-Protected FECs Tables */
                    SHR_IF_ERR_EXIT(fill_fec_table
                                    (unit, DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_id_value), fec_hrc_result, prt_ctr,
                                     (loop_count == 0) ? PRT_ROW_SEP_NONE : PRT_ROW_SEP_UNDERSCORE_BEFORE,
                                     L3_FEC_ID_TO_FEC_INSTANCE(fec_id_value)));

                    /*
                     *  GET THE RESULT TYPE, DEST_VALUE, AND UNDERSTAND NEXT DESTINATION IF FEC
                     */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, fec_hierarchy_tables[fec_hrc_result], entry_handle_id));

                    /** use SUPER_FEC of the first 19bits from FEC_ID as key to access the table */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID,
                                               DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_id_value));

                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

                    /** read the result type */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

                    /** get the fec general entry result - which can contain multiple options */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, entry_handle_id, fec_general_entry_dbal_fields[result_type],
                                     L3_FEC_ID_TO_FEC_INSTANCE(fec_id_value), fec_general_entry_result));

                    /** understand the specific sub fec entry type from the general fec entry */
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                                    (unit, fec_general_entry_dbal_fields[result_type], fec_general_entry_result,
                                     &fec_entry_type, fec_entry_value));

                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, fec_entry_type, DBAL_FIELD_DESTINATION, &dest_value, fec_entry_value));

                    /** check that the next destination in also FEC for next while iteration */
                    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                                    (unit, DBAL_FIELD_DESTINATION, dest_value, &dbal_dest_type, &fec_id_value));

                    /** check if FEC is allocated  */
                    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_id_value, &fec_is_allocated));

                }
                else
                {
                    /** we encounter a no_hierarchy  of a supposedly valid fec_id, we need to exit the FEC loop */
                    PRT_INFO_ADD_PL(NO_PROTECTION_TABLE, "FEC_ID %d hierarchy not set !", fec_id_value);
                    PRT_INFO_ADD_PL(W_PROTECTION_TABLE, "FEC_ID %d hierarchy not set !", fec_id_value);

                    /** set the dbal destination type to other than DBAL_FIELD_FEC to exit the while loop */
                    dbal_dest_type = DBAL_FIELD_EMPTY;

                } /** of if fec_hrc_result */

                /** close the table per FEC only which is not empty */
                for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
                {
                    if (PRT_ROW_NUM_PL(table_id) > 0)
                    {
                        PRT_COMMITX_PL(table_id);
                    }
                    else
                    {
                        PRT_FREE_PL(table_id);
                        /** mark this table as freed */
                        freed_tables[table_id] = TRUE;
                    }
                }

                loop_count++;

            } /** of while FEC's next destination is FEC*/

            /** if finished the loop and still next destination is FEC, means we exited because of max loop count */
            if ((dbal_dest_type == DBAL_FIELD_FEC) && (loop_count > 2))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "after 3 FEC iterations, destination is still FEC_ID %d!\n", fec_id_value);
            }

            /*
             * if at the end of FEC loop the FEC destination value is different
             * from FER forward action destination print error
             */
            {
                uint32 fer_dest_value = 0;

                /** get fwd_action_dst value from FER */
                SHR_IF_ERR_EXIT(diag_dnx_pp_fec_get_fwd_action_dest
                                (unit, core_id, short_mode, "FER", &fer_dest_value, &dest_is_numeric));

                /** if the conversion of the FER fwd_action_dst to number didn't succeed, it means something is wrong at this point, because from FWD2 it succeeded */
                if (!dest_is_numeric)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Core_Id=%d: couldn't convert fwd_action_dst from FER to a numeric value !\n",
                                 core_id);
                }

                if (fer_dest_value != dest_value)
                {
                    LOG_CLI(("Core_Id=%d: last FEC destination: %d, doesn't equal the FER destination: %d!\n", core_id,
                             dest_value, fer_dest_value));
                }
            }

        } /** of if all_print */

    }   /** of core id for loop*/

exit:

    /** free only non free tables */
    {
        table_ids_t table_id;

        for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
        {
            if (!freed_tables[table_id])
            {
                PRT_FREE_PL(table_id);
            }
        }

    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - an Adaptor for sh_dnx_pp_fec_cmds function
 *          In case of Visibility FEC command, which supports LAST flag
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
sh_dnx_pp_vis_fec_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core = -1;
    uint8 short_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("SHORT", short_mode);

    SHR_IF_ERR_EXIT(sh_dnx_pp_fec_cmds(unit, core, FALSE, 0, TRUE, short_mode, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - an Adaptor for sh_dnx_pp_fec_cmds function
 *          In case of Info FEC command, which supports ALL and HRC flag
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
sh_dnx_pp_info_fec_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int usr_hrc_idx = -1;

    int core = -1;
    uint8 short_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("SHORT", short_mode);

    /** the hrc is set to accept only values from 0-2 from user */
    SH_SAND_GET_INT32("HieRarChy", usr_hrc_idx);

    SHR_IF_ERR_EXIT(sh_dnx_pp_fec_cmds(unit, core, TRUE, usr_hrc_idx, FALSE, short_mode, sand_control));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_info_fec_man = {
    .brief = "PP INFo FEC Option.\n" "Shows ALL the FEC entries that are configured in the device.\n",
    .full = NULL,
    .synopsis = "[PP INFo FEC (HRC=<0/1/2>)]",
    /** ALL options is optional it's assumed to be always on */
    .examples = "\n" "ALL\n" "ALL HRC=0\n" "ALL SHORT\n" "ALL file=diag_pp_fec_info_all.xml"
};

sh_sand_man_t dnx_pp_vis_fec_man = {
    .brief = "PP VISibility FEC Option.\n" "Shows only the FECs that were hit by last packet traverse.\n",
    .full = NULL,
    .synopsis = "[PP VISibility FEC]",
    /** LAST options is optional it's assumed to be always on */
    .examples = "\n" "LAST\n" "LAST SHORT\n" "LAST file=diag_pp_fec_vis_last.xml"
};

/*
 *  The FEC command support 2 different PP shell commands: PP Info FEC (all), PP VIS FEC (last)
 *  The (all) and (last) options are assumed to be given, so that the fec main function can separate the cases
 */

sh_sand_option_t dnx_pp_vis_fec_man_options[] = {
    {"LAST", SAL_FIELD_TYPE_BOOL, "Shows last FEC hit in the system", "Yes"},
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};

sh_sand_option_t dnx_pp_info_fec_man_options[] = {
    {"ALL", SAL_FIELD_TYPE_BOOL, "Dumps all existing FEC entries", "Yes"},
    {"HieRarChy", SAL_FIELD_TYPE_INT32, "With ALL flag - restrains the dump for specific FEC hierarchy", "-1", NULL,
     "0-2"},
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};
