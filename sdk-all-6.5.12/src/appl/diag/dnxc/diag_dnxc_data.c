/** \file diag_dnxc_data.c
 * 
 * DEVICE DATA DIAG - diagnostic pack for module dnx_data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*soc*/
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL FUNCTIONs:
 * {
 */
/**
 * \brief - Dump dnxc data 
 *          If specific data - will dump the requested data
 *          if "*" - will dump the all branch
 *          if NULL - will display list of supported data,
 * \par DIRECT INPUT:
 *   \param [in] unit -  Unit #
 *   \param [in] flags - data flags to get see DNXC_DATA_DATA_F_* for details
 *   \param [in] label - filter the according to label (use NULL or "" to skip label filtering)
 *   \param [in] module - module to get or "*" for all modules
 *   \param [in] submodule - submodule to get or "*" for all submodules
 *   \param [in] data - data to get or "*" for all data    
 *   
 * \par INDIRECT INPUT:
 *   * module global data - _dnxc_data[unit]
 * \par DIRECT OUTPUT:
 *   see shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Output to screen - see brief
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_data_dump(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data);

/**
 * \brief - Display info about dnxc data
 *          If specific data - will display info the requested data
 *          if "*" - will display info the all branch
 *          if NULL - will display list of supported data,
 * \par DIRECT INPUT:
 *   \param [in] unit -  Unit #
 *   \param [in] flags - data flags to get see DNXC_DATA_DATA_F_* for details
 *   \param [in] label - filter the according to label (use NULL or "" to skip label filtering)
 *   \param [in] module - module to get or "*" for all modules
 *   \param [in] submodule - submodule to get or "*" for all submodules
 *   \param [in] data - data to get or "*" for all data    
 *   
 * \par INDIRECT INPUT:
 *   * module global data - _dnxc_data[unit]
 * \par DIRECT OUTPUT:
 *   see shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Output to screen - see brief
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e diag_dnxc_data_info(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data);

/**
 * \brief - Print property info
 *          If specific soc property - will display soc property info
 *          Otherwise will display list of soc property includes the requested soc property 
 * \par DIRECT INPUT:
 *   \param [in] unit -  Unit #
 *   \param [in] label - filter the according to label (use NULL or "" to skip label filtering)
 *   \param [in] property - property name or substring of property name
 *
 * \par INDIRECT INPUT:
 *   * module global data - _dnxc_data[unit]
 * \par DIRECT OUTPUT:
 *   see shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Output to screen - see brief
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e diag_dnxc_data_property(
    int unit,
    char *label,
    char *property);

/**
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */
/* *INDENT-OFF* */
static sh_sand_option_t dnxc_data_dump_options[] = {
    {"changed",     SAL_FIELD_TYPE_BOOL,  "dump changed only properties",                       "no"},
    {"define",      SAL_FIELD_TYPE_BOOL,  "dump defines only",                                  "no"},
    {"numeric",     SAL_FIELD_TYPE_BOOL,  "dump numerics only",                                 "no"},
    {"feature",     SAL_FIELD_TYPE_BOOL,  "dump features only",                                 "no"},
    {"table",       SAL_FIELD_TYPE_BOOL,  "dump table only",                                    "no"},
    {"property",    SAL_FIELD_TYPE_BOOL,  "dump properties only",                               "no"},
    {"label",       SAL_FIELD_TYPE_STR,   "dump data marked with specified label only",          ""},
    {"variable",    SAL_FIELD_TYPE_STR,   "[<module_name>[.<submodule_name>[.<data_name>]]]",    ""},
    {NULL}
};

static sh_sand_invoke_t dnxc_data_dump_options_invokes[] = {
    {"dump_test", "module_testing.data_pointers.table_pointers", 0},
    {NULL}
};
/* *INDENT-ON* */

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
static sh_sand_man_t dnxc_data_dump_man = {
    "Dump data or get list of modules/submodules/data",
    "Please fill here full description for data dump",
    "data dump [changed] [property] [define/numeric/feature/table] [label=<label>] [<module_name>[.<submodule_name>[.<data_name>]]]'",
    "'data dump fabric.pipes.map'  - dump table 'map' in module 'fabric' and submodule 'pipes'\n"
    "'data dump fabric.pipes.*'    - dump all data in module 'fabric' and submodule 'pipes'\n"
    "'data dump fabric.pipes.'     - display list of all data in module 'fabric' and submodule 'pipes'\n"
    "'data dump fabric.'           - display list of all submodules in module 'fabric'\n"
    "'data dump chg *'             - will dump all data that changed by soc properties\n"
    "'data dump label=tm *'        - will dump all data marked with label tm"
};
/* *INDENT-ON* */

/**
 * \brief - parse args and call to diag_dnxc_data_dump() with the requested data params
 */
static shr_error_e
cmd_dnxc_data_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *module = NULL;
    char *submodule = NULL;
    char *data = NULL;
    uint32 flags = 0;
    uint32 nof_tokens = 0;
    char **datatokens = NULL;
    char *var, *label;
    int changed_flag, define_flag, feature_flag, numeric_flag, table_flag, property_flag;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get params
     */
    SH_SAND_GET_BOOL("changed", changed_flag);
    SH_SAND_GET_BOOL("define", define_flag);
    SH_SAND_GET_BOOL("numeric", numeric_flag);
    SH_SAND_GET_BOOL("feature", feature_flag);
    SH_SAND_GET_BOOL("table", table_flag);
    SH_SAND_GET_BOOL("property", property_flag);
    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_STR("variable", var);

    if (changed_flag)
    {
        /*
         * Only property data can be marked as changed
         */
        flags |= DNXC_DATA_F_CHANGED;
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
        flags |= DNXC_DATA_F_DEFINE;
    }
    if (property_flag)
    {
        /*
         * Only numeric, feature and table support property
         */
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
    }
    if (feature_flag)
    {
        flags |= DNXC_DATA_F_FEATURE;
    }

    if (define_flag)
    {
        flags |= DNXC_DATA_F_DEFINE;
    }
    if (table_flag)
    {
        flags |= DNXC_DATA_F_TABLE;
    }
    if (numeric_flag)
    {
        flags |= DNXC_DATA_F_NUMERIC;
    }

    /*
     * Expected format - <module>.<submodule>.<data>
     */
    datatokens = utilex_str_split(var, ".", 3, &nof_tokens);
    if (datatokens != NULL)
    {
        if (nof_tokens > 0)
        {
            module = datatokens[0];
        }
        if (nof_tokens > 1)
        {
            submodule = datatokens[1];
        }
        if (nof_tokens > 2)
        {
            data = datatokens[2];
        }
        if (nof_tokens > 3)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, ": data dump - data format not supported - %s\n", var);
        }
    }
    /*
     * set empty token to null
     */
    if (module != NULL && module[0] == 0)
    {
        module = NULL;
    }
    if (submodule != NULL && submodule[0] == 0)
    {
        submodule = NULL;
    }
    if (data != NULL && data[0] == 0)
    {
        data = NULL;
    }
    /*
     * Set '*' along the chain
     */
    if (module != NULL && !sal_strncasecmp(module, "*", strlen("*")))
    {
        submodule = "*";
        data = "*";
    }
    if (submodule != NULL && !sal_strncasecmp(submodule, "*", strlen("*")))
    {
        data = "*";
    }

    /*
     * Call to dnxc data dump with the relevant params
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_dump(unit, flags, label, module, submodule, data));
exit:
/*
 * Free resources
 */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
static sh_sand_option_t dnxc_data_info_options[] = {
    {"changed",     SAL_FIELD_TYPE_BOOL,  "dump changed only properties",                       "no"},
    {"define",      SAL_FIELD_TYPE_BOOL,  "dump defines only",                                  "no"},
    {"numeric",     SAL_FIELD_TYPE_BOOL,  "dump numerics only",                                 "no"},
    {"feature",     SAL_FIELD_TYPE_BOOL,  "dump features only",                                 "no"},
    {"table",       SAL_FIELD_TYPE_BOOL,  "dump table only",                                    "no"},
    {"property",    SAL_FIELD_TYPE_BOOL,  "dump properties only",                               "no"},
    {"label",       SAL_FIELD_TYPE_STR,   "dump data marked with specified label only",          ""},
    {"variable",    SAL_FIELD_TYPE_STR,   "[<module_name>[.<submodule_name>[.<data_name>]]]",   ""},
    {NULL}
};
/* *INDENT-ON* */

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
static sh_sand_man_t dnxc_data_info_man = {
    "Get list of modules/submodules/data or full description id data specified",
    "if '*' is specified will display info about the all branch",
    "data info [changed] [property] [define/numeric/feature/table] [label=<label>] [<module_name>[.<submodule_name>[.<data_name>]]]",
    "'data info fabric.pipes.map'   - will display info about table 'map' in module 'fabric' and submodule 'pipes'\n"
    "'data info fabric.pipes.*'     - will display info about all data in module 'fabric' and submodule 'pipes'\n"
    "'data info fabric.'            - will display list of all submodules in module 'fabric'\n"
    "'data info chg *'              - will display info about all data that changed by soc properties"
    "'data dump label=tm *'         - will display info about  all data marked with label tm"
};
/* *INDENT-ON* */

/**
 * \brief - parse args and call to diag_dnxc_data_info() with the requested data params
 */
static shr_error_e
cmd_dnxc_data_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *module = NULL;
    char *submodule = NULL;
    char *data = NULL;
    uint32 flags = 0;
    uint32 nof_tokens = 0;
    char **datatokens = NULL;
    char *var, *label;
    int changed_flag, define_flag, feature_flag, numeric_flag, table_flag, property_flag;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get params
     */
    SH_SAND_GET_BOOL("changed", changed_flag);
    SH_SAND_GET_BOOL("define", define_flag);
    SH_SAND_GET_BOOL("numeric", numeric_flag);
    SH_SAND_GET_BOOL("feature", feature_flag);
    SH_SAND_GET_BOOL("table", table_flag);
    SH_SAND_GET_BOOL("property", property_flag);
    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_STR("variable", var);

    if (changed_flag)
    {
        /*
         * Only property data can be marked as changed
         */
        flags |= DNXC_DATA_F_CHANGED;
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
    }
    if (property_flag)
    {
        /*
         * Only numeric, feature and table support property
         */
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
    }
    if (feature_flag)
    {
        flags |= DNXC_DATA_F_FEATURE;
    }

    if (define_flag)
    {
        flags |= DNXC_DATA_F_DEFINE;
    }
    if (table_flag)
    {
        flags |= DNXC_DATA_F_TABLE;
    }
    if (numeric_flag)
    {
        flags |= DNXC_DATA_F_NUMERIC;
    }

    /*
     * Expected format - <module>.<submodule>.<data>
     */
    datatokens = utilex_str_split(var, ".", 3, &nof_tokens);
    if (datatokens != NULL)
    {
        if (nof_tokens > 0)
        {
            module = datatokens[0];
        }
        if (nof_tokens > 1)
        {
            submodule = datatokens[1];
        }
        if (nof_tokens > 2)
        {
            data = datatokens[2];
        }
        if (nof_tokens > 3)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, ": data dump - data format not supported - %s\n", var);
        }
    }
    /*
     * set empty token to null
     */
    if (module != NULL && module[0] == 0)
    {
        module = NULL;
    }
    if (submodule != NULL && submodule[0] == 0)
    {
        submodule = NULL;
    }
    if (data != NULL && data[0] == 0)
    {
        data = NULL;
    }
    /*
     * Set '*' along the chain
     */
    if (module != NULL && !sal_strncasecmp(module, "*", strlen("*")))
    {
        submodule = "*";
        data = "*";
    }
    if (submodule != NULL && !sal_strncasecmp(submodule, "*", strlen("*")))
    {
        data = "*";
    }

    SHR_IF_ERR_EXIT(diag_dnxc_data_info(unit, flags, label, module, submodule, data));
exit:
/*
 * Free resources
 */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }
    SHR_FUNC_EXIT;
}
/* *INDENT-OFF* */
static sh_sand_invoke_t dnxc_data_property_options_invokes[] = {
    {"fabric_pcp_enable", "fabric_pcp_enable", 0},
    {NULL}
};

static sh_sand_option_t dnxc_data_property_options[] = {
    {"label",     SAL_FIELD_TYPE_STR,  "Filter soc properties accorfiny to data label",          ""},
    {"variable",  SAL_FIELD_TYPE_STR,  "Filter soc properties by this substring",                NULL},
    {NULL}
};

static sh_sand_man_t dnxc_data_property_man = {
    "Get list of soc properties contains substring <property> or info about soc property named <property>",
    "Please fill full description here",
    "data property [name]",
    "Please provide at least one example"
};
/* *INDENT-ON* */

/**
 * \brief - parse args and call to diag_dnxc_data_property() with the requested data params
 */
static shr_error_e
cmd_dnxc_data_property(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *property;
    char *label;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_STR("variable", property);

    /*
     * Call to dnxc data dump with the relevant params
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_property(unit, label, property));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print property info according to the input structure 
 */
static shr_error_e
diag_dnxc_data_property_info(
    int unit,
    char *module,
    char *submodule,
    char *data_type,
    char *data,
    char *member,
    dnxc_data_property_t * property,
    char *prefix)
{
    int map_index;
    SHR_FUNC_INIT_VARS(unit);

    if (module != NULL)
        cli_out("--------------\n");
    cli_out("%sPROPERTY:\n", prefix);
    cli_out("%s\tNAME: '%s'\n", prefix, property->name);
    if (module != NULL)
    {
        cli_out("\t%s: '%s.%s.%s", data_type, module, submodule, data);
        if (member != NULL)
        {
            cli_out(" (member %s )", member);
        }
        cli_out("'\n");
    }
    cli_out("%s\tMETHOD: '%s'\n", prefix, property->method_str);

    switch (property->method)
    {
        case dnxc_data_property_method_range:
        case dnxc_data_property_method_port_range:
        case dnxc_data_property_method_suffix_range:
            cli_out("%s\tRANGE: [%u - %u]\n", prefix, property->range_min, property->range_max);
            break;
        case dnxc_data_property_method_range_signed:
        case dnxc_data_property_method_port_range_signed:
        case dnxc_data_property_method_suffix_range_signed:
            cli_out("%s\tRANGE: [%d - %d]\n", prefix, property->range_min, property->range_max);
            break;
        case dnxc_data_property_method_direct_map:
        case dnxc_data_property_method_port_direct_map:
        case dnxc_data_property_method_suffix_direct_map:
            cli_out("%s\tMAPPING: \n", prefix);
            for (map_index = 0; map_index < property->nof_mapping; map_index++)
            {
                if (property->mapping[map_index].is_invalid == TRUE)
                {
                    cli_out("%s\t\t\t '%s' -> '%d' (Disabled) \n", prefix, property->mapping[map_index].name,
                            property->mapping[map_index].val);
                }
                else
                {
                    cli_out("%s\t\t\t '%s' -> '%d' \n", prefix, property->mapping[map_index].name,
                            property->mapping[map_index].val);
                }
            }
            break;
        default:
            /*
             * do nothing
             */
            break;
    }
    cli_out("%s\tDOC: \n'\n%s\n'\n", prefix, property->doc);
    if (module != NULL)
        cli_out("--------------\n");

    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of defines according to flags
 */
static shr_error_e
diag_dnxc_data_define_list(
    int unit,
    dnxc_data_define_t * defines,
    int nof_defines,
    uint32 flags,
    char *label)
{
    int define_index;
    int dump, count;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Dump define / numeric info
     */
    if (flags & DNXC_DATA_F_NUMERIC)
    {
        cli_out("LIST OF NUMERICS:\n");
    }
    else
    {
        cli_out("LIST OF DEFINES:\n");
    }

    cli_out("-\n");
    /*
     * Iterate over defines
     */
    count = 0;
    for (define_index = 0; define_index < nof_defines; define_index++)
    {

        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                         defines[define_index].labels, flags, label, &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)
        {
            cli_out("%-30s", defines[define_index].name);
            count++;
            if (count % 3 == 0)
                cli_out("\n");
        }
    }
    cli_out("\n");
    cli_out("-\n");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - print info about the define named 'data' 
 *          if 'data' == '*" print info of all defines according to flags. 
 */
static shr_error_e
diag_dnxc_data_define_info(
    int unit,
    dnxc_data_define_t * defines,
    int nof_defines,
    uint32 flags,
    char *label,
    char *data)
{
    int define_index;
    int dump, label_index;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    if (data == NULL)   /* list of defnies mode */
    {
        SHR_IF_ERR_EXIT(diag_dnxc_data_define_list(unit, defines, nof_defines, flags, label));
    }
    else
    {
        for (define_index = 0; define_index < nof_defines; define_index++)
        {
            if (!sal_strncasecmp(data, defines[define_index].name, strlen(data)) ||
                !sal_strncasecmp(data, "*", strlen("*")))
            {
                /*
                 * Check if data is supported/match flags
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                                 defines[define_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)       /* if supported/match flags */
                {
                    cli_out("-\n");
                    if (defines[define_index].flags & DNXC_DATA_F_NUMERIC)
                    {
                        cli_out("NUMERIC: '%s'\n", defines[define_index].name);
                    }
                    else
                    {
                        cli_out("DEFINE: '%s'\n", defines[define_index].name);
                    }
                    cli_out("\tDOC: '%s'\n", defines[define_index].doc);
                    if (defines[define_index].flags & DNXC_DATA_F_INIT_ONLY)
                    {
                        cli_out("\tINIT_ONLY\n");
                    }
                    cli_out("\tLABELS: ");
                    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
                    {
                        if (defines[define_index].labels[label_index] != NULL)
                        {
                            cli_out("%s ", defines[define_index].labels[label_index]);
                        }
                    }
                    cli_out("\n");
                    if (defines[define_index].property.name != NULL)
                    {
                        SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, NULL, NULL,
                                                                     NULL, NULL, NULL, &defines[define_index].property,
                                                                     "\t"));
                    }
                    cli_out("-\n");
                }
            }
        }
    }

    cli_out("\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of features according to flags
 */
static shr_error_e
diag_dnxc_data_feature_list(
    int unit,
    dnxc_data_feature_t * features,
    int nof_features,
    uint32 flags,
    char *label)
{
    int feature_index;
    int dump, count;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * title 
     */
    cli_out("LIST OF FEATURES:\n");
    cli_out("-\n");
    /*
     * Iterate over features
     */
    count = 0;
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {

        /*
         * Check if data is supported / match flags
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, features[feature_index].flags,
                                         features[feature_index].labels, flags, label, &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)       /* if supported / requested flags */
        {
            cli_out("%-30s", features[feature_index].name);
            count++;
            if (count % 3 == 0)
                cli_out("\n");
        }
    }
    cli_out("\n");
    cli_out("-\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print info about the feature named 'data' 
 *          if 'data' == '*" print info of all features according to flags. 
 */
static shr_error_e
diag_dnxc_data_feature_info(
    int unit,
    dnxc_data_feature_t * features,
    int nof_features,
    uint32 flags,
    char *label,
    char *data)
{
    int feature_index;
    int dump, label_index;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    if (data == NULL)   /* list of features mode */
    {
        SHR_IF_ERR_EXIT(diag_dnxc_data_feature_list(unit, features, nof_features, flags, label));
    }
    else
    {
        for (feature_index = 0; feature_index < nof_features; feature_index++)
        {
            if (!sal_strncasecmp(data, features[feature_index].name, strlen(data)) ||
                !sal_strncasecmp(data, "*", strlen("*")))
            {
                /*
                 * Check if data is supported / match flags
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, features[feature_index].flags,
                                                 features[feature_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)       /* if supported / match flags */
                {
                    cli_out("-\n");
                    cli_out("FEATURE: '%s'\n", features[feature_index].name);
                    cli_out("\tDOC: '%s'\n", features[feature_index].doc);
                    if (features[feature_index].flags & DNXC_DATA_F_INIT_ONLY)
                    {
                        cli_out("\tINIT_ONLY\n");
                    }
                    cli_out("\tLABELS: ");
                    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
                    {
                        if (features[feature_index].labels[label_index] != NULL)
                        {
                            cli_out("%s ", features[feature_index].labels[label_index]);
                        }
                    }
                    cli_out("\n");
                    if (features[feature_index].property.name != NULL)
                    {
                        SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, NULL, NULL,
                                                                     NULL, NULL, NULL,
                                                                     &features[feature_index].property, "\t"));
                    }
                    cli_out("-\n");
                }
            }
        }
    }

    cli_out("\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - display table info or dump data according to info_mode      
 */
static shr_error_e
diag_dnxc_data_table_dump(
    int unit,
    dnxc_data_table_t * table,
    uint32 dump_flags,
    int info_mode)
{
    int key1_index, key2_index, value_index, key_index;
    int key1_size, key2_size;
    char *buffer = NULL;
    int changed, label_index;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** alloc buffer */
    SHR_ALLOC(buffer, sizeof(char) * DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "buffer for dnxc data table dump", "%s%s%s",
              EMPTY, EMPTY, EMPTY);
    cli_out("-\n");
    if (info_mode)
    {
        /*
         * title
         */
        cli_out("TABLE: '%s'\n", table->name);

        /*
         * Display table info
         */
        cli_out("\tDOC: '%s'\n", table->doc);
        if (table->flags & DNXC_DATA_F_INIT_ONLY)
        {
            cli_out("\tINIT_ONLY\n");
        }
        cli_out("\tLABELS: ");
        for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
        {
            if (table->labels[label_index] != NULL)
            {
                cli_out("%s ", table->labels[label_index]);
            }
        }
        cli_out("\n");
        /*
         * Dump keys info
         */
        if (table->nof_keys != 0)
        {
            cli_out("\tKEYS:\n");

            for (key_index = 0; key_index < table->nof_keys; key_index++)
            {
                cli_out("\t\t-\n");
                cli_out("\t\tNAME: '%s'\n", table->keys[key_index].name);
                cli_out("\t\tDOC: '%s'\n", table->keys[key_index].doc);
                cli_out("\t\tSIZE: '%d'\n", table->keys[key_index].size);
                cli_out("\t\t-\n");
            }
        }

        /*
         * Dump values info
         */
        cli_out("\tVALUES:\n");
        for (value_index = 0; value_index < table->nof_values; value_index++)
        {
            cli_out("\t\t-\n");
            cli_out("\t\tNAME: '%s'\n", table->values[value_index].name);
            cli_out("\t\tTYPE: '%s'\n", table->values[value_index].type);
            cli_out("\t\tDOC: '%s'\n", table->values[value_index].doc);
            cli_out("\t\tDEFAULT: '%s'\n", table->values[value_index].default_val);
            if (table->values[value_index].property.name != NULL)
            {
                SHR_IF_ERR_EXIT(diag_dnxc_data_property_info
                                (unit, NULL, NULL, NULL, NULL, NULL, &table->values[value_index].property, "\t\t"));
            }

            cli_out("\t\t-\n");
        }

        /*
         * Dump property info
         */
        if (table->property.name != NULL)
        {
            SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, NULL, NULL, NULL, NULL, NULL, &table->property, "\t"));
        }
        cli_out("-\n");

    }
    else
    {
        /*
         * Dump table
         */
        /*
         * Get keys size
         */
        key1_size = 0;
        key2_size = 0;
        if (table->nof_keys > 0)
        {
            key1_size = table->keys[0].size;
            if (table->nof_keys > 1)
            {
                key2_size = table->keys[1].size;
            }
        }

        /*
         * define rows for table
         */
        PRT_TITLE_SET("TABLE: '%s'", table->name);
        for (key_index = 0; key_index < table->nof_keys; key_index++)
        {
            PRT_COLUMN_ADD("%s", table->keys[key_index].name);
        }
        PRT_COLUMN_ADD("#");
        for (value_index = 0; value_index < table->nof_values; value_index++)
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", table->values[value_index].name);
        }
        /*
         * Iterate entries and print keys and values
         */
        for (key1_index = 0; key1_index < key1_size || (key1_index == 0); key1_index++)
        {
            for (key2_index = 0; (key2_index < key2_size) || (key2_index == 0); key2_index++)
            {
                /*
                 * Filter changed entries according to dump flags
                 */
                changed = 1;
                if (dump_flags & DNXC_DATA_F_CHANGED)
                {
                    SHR_IF_ERR_EXIT(dnxc_data_mgmt_table_entry_changed_get
                                    (unit, table, key1_index, key2_index, &changed));
                }
                if (!changed)
                {
                    continue;
                }

                /*
                 *  Print entry
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /*
                 * Entry keys
                 */
                if (table->nof_keys == 1)
                {
                    PRT_CELL_SET("%d", key1_index);
                }
                else if (table->nof_keys == 2)
                {
                    PRT_CELL_SET("%d", key1_index);
                    PRT_CELL_SET("%d", key2_index);
                }
                PRT_CELL_SET("#");
                /*
                 * Entry values
                 */
                for (value_index = 0; value_index < table->nof_values; value_index++)
                {
                    /*
                     * Get value as string
                     */
                    table->entry_get(unit, buffer, key1_index, key2_index, value_index);
                    PRT_CELL_SET("%s", buffer);
                }

            }
        }
        PRT_COMMIT;
    }

exit:
    PRT_FREE;
    SHR_FREE(buffer);
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of tables according to flags
 */
static shr_error_e
diag_dnxc_data_table_list(
    int unit,
    dnxc_data_table_t * tables,
    int nof_tables,
    uint32 flags,
    char *label)
{
    int table_index;
    int dump, count;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Display table list
     */
    cli_out("LIST OF TABLES:\n");
    cli_out("-\n");
    /*
     * Iterate over tables
     */
    count = 0;
    for (table_index = 0; table_index < nof_tables; table_index++)
    {

        /*
         * Check if data is supported / match flags
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, tables[table_index].flags,
                                         tables[table_index].labels, flags, label, &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)       /* if supported / match flags */
        {
            /*
             * Print name
             */
            cli_out("%-30s", tables[table_index].name);
            count++;
            if (count % 3 == 0)
                cli_out("\n");
        }
    }
    cli_out("\n");
    cli_out("-\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print info about the table named 'data' 
 *          if 'data' == '*" print info of all tables according to flags. 
 */
static shr_error_e
diag_dnxc_data_table_info(
    int unit,
    dnxc_data_table_t * tables,
    int nof_tables,
    uint32 flags,
    char *label,
    char *data)
{
    int table_index;
    int dump;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Dump table info
     */
    if (data == NULL)   /* list of tables mode */
    {
        /*
         *  Display list of tables
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_table_list(unit, tables, nof_tables, flags, label));
    }
    else
    {
        for (table_index = 0; table_index < nof_tables; table_index++)
        {
            if (!sal_strncasecmp(data, tables[table_index].name, strlen(data)) ||
                !sal_strncasecmp(data, "*", strlen("*")))
            {
                /*
                 * Check if data is supported / match flags
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, tables[table_index].flags,
                                                 tables[table_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * Display table info
                     */
                    SHR_IF_ERR_EXIT(diag_dnxc_data_table_dump(unit, &tables[table_index], flags, 1));
                }       /* Dump table */
            }   /* if selected table */
        }
    }

    cli_out("\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of submodules
 */
static shr_error_e
diag_dnxc_data_submodule_list(
    int unit,
    dnxc_data_module_t * module)
{
    int count = 0;
    int submodule_index;
    dnxc_data_submodule_t *submodules = module->submodules;

    SHR_FUNC_INIT_VARS(unit);

    cli_out("LIST OF SUBMODULES:\n");
    cli_out("---\n");
    /*
     * Iterate over submodules
     */
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        /*
         * Print name
         */
        cli_out("%-30s", submodules[submodule_index].name);
        count++;
        if (count % 3 == 0)
            cli_out("\n");
    }
    cli_out("\n");

    SHR_FUNC_EXIT;
}

/**
 * \brief - print info about the submodule named 'submodule' 
 *          if 'submodule' == "*" print info of all submodules according to flags. 
 */
static shr_error_e
diag_dnxc_data_module_info(
    int unit,
    dnxc_data_module_t * module,
    uint32 flags,
    char *label,
    char *submodule,
    char *data)
{
    int submodule_index;
    dnxc_data_submodule_t *submodules = module->submodules;

    SHR_FUNC_INIT_VARS(unit);

    if (submodule == NULL)      /* list of submodules mode */
    {
        SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_list(unit, module));
    }
    else
    {
        /*
         * Iterate over submodules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            if (!sal_strncasecmp(submodule, submodules[submodule_index].name, strlen(submodule)) ||
                !sal_strncasecmp(submodule, "*", strlen("*")))
            {
                cli_out("SUBMODULE: %s \n", submodules[submodule_index].name);
                cli_out("---:\n");
                cli_out("%s \n\n", submodules[submodule_index].doc);

                /*
                 * Defines info
                 */
                if ((flags & DNXC_DATA_F_DEFINE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_define_info(unit, submodules[submodule_index].defines,
                                                               submodules[submodule_index].nof_defines,
                                                               flags | DNXC_DATA_F_DEFINE, label, data));
                }

                /*
                 * Numeric info
                 */
                if ((flags & DNXC_DATA_F_DEFINE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_define_info(unit, submodules[submodule_index].defines,
                                                               submodules[submodule_index].nof_defines,
                                                               flags | DNXC_DATA_F_NUMERIC, label, data));
                }

                /*
                 * Feature info
                 */
                if ((flags & DNXC_DATA_F_FEATURE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_feature_info(unit, submodules[submodule_index].features,
                                                                submodules[submodule_index].nof_features, flags, label,
                                                                data));
                }

                /*
                 * Table info
                 */
                if ((flags & DNXC_DATA_F_TABLE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_table_info(unit, submodules[submodule_index].tables,
                                                              submodules[submodule_index].nof_tables, flags, label,
                                                              data));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of modules
 */
static shr_error_e
diag_dnxc_data_list(
    int unit)
{
    int module_index;
    int count = 0;
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("LIST OF MODULES:\n");
    cli_out("----------------\n");
    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        /*
         * Print name
         */
        cli_out("%-30s", modules[module_index].name);
        count++;
        if (count % 3 == 0)
            cli_out("\n");
    }
    cli_out("\n");

    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_info(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data)
{
    int module_index;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("DNXC DATA INFO:\n");
    cli_out("==============\n");

    if (module == NULL) /* list of modules mode */
    {
        /*
         * Display module list
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_list(unit));
    }
    else
    {
        /*
         * Iterate over modules
         */
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            if (!sal_strncasecmp(module, modules[module_index].name, strlen(module)) ||
                !sal_strncasecmp(module, "*", strlen("*")))
            {
                cli_out("MODULE %s \n", modules[module_index].name);
                cli_out("---------------:\n");
                SHR_IF_ERR_EXIT(diag_dnxc_data_module_info
                                (unit, &modules[module_index], flags, label, submodule, data));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print data of define named 'data' 
 *          if 'data' == '*" print data of all defines according to flags. 
 */
static shr_error_e
diag_dnxc_data_defines_dump(
    int unit,
    dnxc_data_define_t * defines,
    int nof_defines,
    uint32 flags,
    char *label,
    char *data)
{
    int define_index;
    int dump;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    if (data == NULL)   /* defines list mode */
    {
        /*
         * Display numeric / define list
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_define_list(unit, defines, nof_defines, flags, label));
    }
    else
    {
        /*
         * Iterate over defines
         */
        for (define_index = 0; define_index < nof_defines; define_index++)
        {
            if (!sal_strncasecmp(data, "*", strlen("*")) ||
                !sal_strncasecmp(data, defines[define_index].name, strlen(data)))
            {
                /*
                 * Check if data is supported / match flags
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                                 defines[define_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)       /* if supported / match flags */
                {
                    /*
                     * Dump define / numeric 
                     */
                    cli_out("---\n");
                    if (flags & DNXC_DATA_F_NUMERIC)
                    {
                        cli_out("NUMERIC: '%s'        VALUE: '%d'\n", defines[define_index].name,
                                defines[define_index].data);
                    }
                    else
                    {
                        cli_out("DEFINE: '%s'        VALUE: '%d'\n", defines[define_index].name,
                                defines[define_index].data);
                    }
                    cli_out("---\n");
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print data of feature named 'data' 
 *          if 'data' == '*" print data of all features according to flags. 
 */
static shr_error_e
diag_dnxc_data_features_dump(
    int unit,
    dnxc_data_feature_t * features,
    int nof_features,
    uint32 flags,
    char *label,
    char *data)
{
    int feature_index;
    int dump;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    if (data == NULL)   /* features list mode */
    {
        /*
         * Display list of features
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_feature_list(unit, features, nof_features, flags, label));
    }
    else
    {
        /*
         * Iterate over features
         */
        for (feature_index = 0; feature_index < nof_features; feature_index++)
        {
            if (!sal_strncasecmp(data, "*", strlen("*")) ||
                !sal_strncasecmp(data, features[feature_index].name, strlen(data)))
            {
                /*
                 * Check if data is supported / match flags
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, features[feature_index].flags,
                                                 features[feature_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)       /* if supported / match flags */
                {
                    /*
                     * Dump data
                     */
                    cli_out("---\n");
                    cli_out("FEATURE: '%s' %s\n", features[feature_index].name,
                            features[feature_index].data == 0 ? " - disabled" : "");
                    cli_out("---\n");
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print data of table named 'data' 
 *          if 'data' == '*" print data of all tables according to flags. 
 */
static shr_error_e
diag_dnxc_data_tables_dump(
    int unit,
    dnxc_data_table_t * tables,
    int nof_tables,
    uint32 flags,
    char *label,
    char *data)
{
    int table_index;
    int dump;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    if (data == NULL)   /* tables list mode */
    {
        /*
         * Display list of tables
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_table_list(unit, tables, nof_tables, flags, label));
    }
    else
    {
        /*
         * Iterate over tables
         */
        for (table_index = 0; table_index < nof_tables; table_index++)
        {
            if (!sal_strncasecmp(data, "*", strlen("*")) ||
                !sal_strncasecmp(data, tables[table_index].name, strlen(data)))
            {
                /*
                 * Check if data is supported / match flags
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, tables[table_index].flags,
                                                 tables[table_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)       /* if supported / match flags */
                {
                    /*
                     * Dump data
                     */
                    SHR_IF_ERR_EXIT(diag_dnxc_data_table_dump(unit, &tables[table_index], flags, 0));
                }       /* Dump table */
            }   /* if selected table */
        }       /* tables iterate */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print data of submodule named 'submodule' 
 *          if 'submodule' == '*" print data of all submodules according to flags. 
 */
static shr_error_e
diag_dnxc_data_module_dump(
    int unit,
    dnxc_data_module_t * module,
    uint32 flags,
    char *label,
    char *submodule,
    char *data)
{
    int submodule_index;
    dnxc_data_submodule_t *submodules = module->submodules;
    SHR_FUNC_INIT_VARS(unit);

    if (submodule == NULL)      /* submodule list mode */
    {
        /*
         * Display list of submodules
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_list(unit, module));
    }
    else
    {
        /*
         * Iterate over submodules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            if (!sal_strncasecmp(submodule, "*", strlen("*")) ||
                !sal_strncasecmp(submodule, submodules[submodule_index].name, strlen(submodule)))
            {
                cli_out("SUBMODULE: %s \n", submodules[submodule_index].name);
                cli_out("----------------\n");

                /*
                 * Defines
                 */
                if ((flags & DNXC_DATA_F_DEFINE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_defines_dump(unit, submodules[submodule_index].defines,
                                                                submodules[submodule_index].nof_defines,
                                                                flags | DNXC_DATA_F_DEFINE, label, data));
                }

                /*
                 * Numerics
                 */
                if ((flags & DNXC_DATA_F_NUMERIC) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_defines_dump(unit, submodules[submodule_index].defines,
                                                                submodules[submodule_index].nof_defines,
                                                                flags | DNXC_DATA_F_NUMERIC, label, data));
                }

                /*
                 * Features
                 */
                if ((flags & DNXC_DATA_F_FEATURE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_features_dump(unit, submodules[submodule_index].features,
                                                                 submodules[submodule_index].nof_features, flags, label,
                                                                 data));
                }

                /*
                 * Tables
                 */
                if ((flags & DNXC_DATA_F_TABLE) || flags == 0)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_tables_dump(unit, submodules[submodule_index].tables,
                                                               submodules[submodule_index].nof_tables, flags, label,
                                                               data));
                }

                cli_out("----------------\n\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - see definition on local function section (top of this file)
 */
shr_error_e
diag_dnxc_data_dump(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data)
{
    int module_index;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("DNXC DATA DUMP:\n");
    cli_out("==============\n");
    cli_out("==============\n\n");

    if (module == NULL) /* module list mode */
    {
        /*
         * Display list of modules
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_list(unit));
    }
    else
    {
        /*
         * Iterate over modules
         */
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            if (!sal_strncasecmp(module, "*", strlen("*")) ||
                !sal_strncasecmp(module, modules[module_index].name, strlen(module)))
            {
                /*
                 * Dump module
                 */
                cli_out("MODULE: %s \n", modules[module_index].name);
                cli_out("================\n");
                SHR_IF_ERR_EXIT(diag_dnxc_data_module_dump
                                (unit, &modules[module_index], flags, label, submodule, data));
                cli_out("================\n\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_property(
    int unit,
    char *label,
    char *property)
{
    int module_index, submodule_index, table_index, feature_index, define_index;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    dnxc_data_module_t *module;
    dnxc_data_submodule_t *submodule;
    dnxc_data_table_t *table;
    dnxc_data_feature_t *feature;
    dnxc_data_define_t *define;
    int dump;
    shr_error_e rv;
    int value_index;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("DNXC DATA PROPERTY:\n");
    cli_out("==============\n");
    cli_out("==============\n\n");

    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        module = &modules[module_index];

        /*
         * iterate of submodules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            submodule = &module->submodules[submodule_index];

            /*
             * iterate of tables
             */
            for (table_index = 0; table_index < submodule->nof_tables; table_index++)
            {
                table = &submodule->tables[table_index];
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, table->flags, table->labels, 0, label,
                                                 &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * check if data loaded be SoC property
                     */
                    if (table->property.name != NULL)
                    {
                        /*
                         *check if identical
                         */
                        if (!sal_strncasecmp(table->property.name, property, strlen(table->property.name)))
                        {
                            SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, module->name, submodule->name,
                                                                         "TABLE", table->name, NULL, &table->property,
                                                                         ""));
                        }
                        else if (strcaseindex(table->property.name, property) != NULL)  /* check if substring */
                        {
                            cli_out("PROPERTY: '%s'\n", table->property.name);
                        }
                    }

                    for (value_index = 0; value_index < table->nof_values; value_index++)
                    {

                        /*
                         * check if data loaded be SoC property
                         */
                        if (table->values[value_index].property.name != NULL)
                        {
                            /*
                             *check if identical
                             */
                            if (!sal_strncasecmp
                                (table->values[value_index].property.name, property,
                                 strlen(table->values[value_index].property.name)))
                            {
                                SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, module->name, submodule->name,
                                                                             "TABLE", table->name,
                                                                             table->values[value_index].name,
                                                                             &table->values[value_index].property, ""));
                            }
                            else if (strcaseindex(table->values[value_index].property.name, property) != NULL)  /* check 
                                                                                                                 * if
                                                                                                                 * substring 
                                                                                                                 */
                            {
                                cli_out("PROPERTY: '%s'\n", table->values[value_index].property.name);
                            }
                        }
                    }
                }       /* Dump table */
            }   /* table iterator */

            /*
             * iterate of features
             */
            for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
            {
                feature = &submodule->features[feature_index];
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, feature->flags, feature->labels, 0,
                                                 label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * check if data loaded be SoC property
                     */
                    if (feature->property.name != NULL)
                    {
                        /*
                         * check if identical
                         */
                        if (!sal_strncasecmp(feature->property.name, property, strlen(feature->property.name)))
                        {
                            SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, module->name, submodule->name,
                                                                         "FEATURE", feature->name, NULL,
                                                                         &feature->property, ""));
                        }
                        /*
                         * check if substring
                         */
                        else if (strcaseindex(feature->property.name, property) != NULL)
                        {
                            cli_out("PROPERTY: '%s'\n", feature->property.name);
                        }
                    }
                }       /* Dump feature */
            }   /* feature iterator */

            /*
             * iterate of numerics
             */
            for (define_index = 0; define_index < submodule->nof_defines; define_index++)
            {
                define = &submodule->defines[define_index];
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, define->flags, define->labels, 0, label,
                                                 &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * check if data loaded be SoC property
                     */
                    if (define->property.name != NULL)
                    {
                        /*
                         * check if identical
                         */
                        if (!sal_strncasecmp(define->property.name, property, strlen(define->property.name)))
                        {
                            SHR_IF_ERR_EXIT(diag_dnxc_data_property_info(unit, module->name, submodule->name,
                                                                         "NUMERIC", define->name, NULL,
                                                                         &define->property, ""));
                        }
                        /*
                         * check if substring
                         */
                        else if (strcaseindex(define->property.name, property) != NULL)
                        {
                            cli_out("PROPERTY: '%s'\n", define->property.name);
                        }
                    }
                }       /* Dump numeric */
            }   /* numeric iterator */

        }       /* submodule iterator */
    }   /* module iterator */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Export submodule's features to xml
 */
static shr_error_e
diag_dnxc_data_submodule_features_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (submodule->nof_features > 0)
    {
        dnxc_data_feature_t *features = submodule->features;
        int feature_index, feature_col_id;
        uint8 table_has_content = FALSE;

        PRT_TITLE_SET("%s.%s.features", module->name, submodule->name);

        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &feature_col_id, "Feature");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, feature_col_id, NULL, "State");

        for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
        {
            int do_export = 0;

            /*
             * Check if data is supported / match flags
             */
            SHR_IF_ERR_EXIT(dnxc_data_utils_dump_verify
                            (unit, _dnxc_data[unit].state, features[feature_index].flags,
                             features[feature_index].labels, flags, NULL, &do_export));
            if (do_export)
            {
                const char *feature_state = (features[feature_index].data == 0 ? "disabled" : "enabled");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", features[feature_index].name);
                PRT_CELL_SET("%s", feature_state);
                table_has_content = TRUE;
            }
        }

        if (TRUE == table_has_content)
        {
            PRT_COMMITX;
        }

    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   export submodule's defines to xml
 */
static shr_error_e
diag_dnxc_data_submodule_defines_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (submodule->nof_defines > 0)
    {
        dnxc_data_define_t *defines = submodule->defines;
        int define_index, define_col_id;
        uint8 table_has_content = FALSE;

        PRT_TITLE_SET("%s.%s.defines", module->name, submodule->name);

        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &define_col_id, "Define");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, define_col_id, NULL, "Value");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, define_col_id, NULL, "Type");

        for (define_index = 0; define_index < submodule->nof_defines; define_index++)
        {
            int do_export = 0;

            /*
             * Check if data is supported / match flags
             */
            SHR_IF_ERR_EXIT(dnxc_data_utils_dump_verify
                            (unit, _dnxc_data[unit].state, defines[define_index].flags, defines[define_index].labels,
                             flags, NULL, &do_export));
            if (do_export)
            {
                const char *define_type = (defines[define_index].flags & DNXC_DATA_F_NUMERIC) ? "Numeric" : "Define";
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", defines[define_index].name);
                PRT_CELL_SET("%d", defines[define_index].data);
                PRT_CELL_SET("%s", define_type);
                table_has_content = TRUE;
            }
        }

        if (TRUE == table_has_content)
        {
            PRT_COMMITX;
        }

    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Export submodule's specific table to xml
 */
static shr_error_e
diag_dnxc_data_table_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    dnxc_data_table_t * table,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    /*
     * Table column ID holders
     */
    int entry_col_id, key_col_id, result_col_id;
    /*
     * 'for' counters
     */
    int key1_index, key2_index, entry_row, entry_counter = 0;
    /*
     * If each entry in the table requires more than a single line, use a separator to distinguish between them
     * (print only).
     */
    int entry_row_separator = ((table->nof_keys > 1) || (table->nof_values > 1))
        ? PRT_ROW_SEP_UNDERSCORE_BEFORE : PRT_ROW_SEP_NONE;
    /*
     * Indicates if there is actual content in the table to export.
     * Used to prevent exporting empty tables or, if the 'changed' flag is used,
     * not to export tables with no changed entries.
     */
    uint8 table_has_content = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * define title and headline for table
     */
    PRT_TITLE_SET("%s.%s.tables::%s", module->name, submodule->name, table->name);
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &entry_col_id, "entry");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &key_col_id, "key_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, key_col_id, NULL, "key_value");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &result_col_id, "result_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, result_col_id, NULL, "result_value");

    /*
     * Iterate entries and print keys and values
     */
    for (key1_index = 0; (key1_index < table->keys[0].size) || (key1_index == 0); key1_index++)
    {
        for (key2_index = 0; (key2_index < table->keys[1].size) || (key2_index == 0); key2_index++)
        {
            /*
             * Filter changed entries according to dump flags
             */
            int changed = 1;
            if (flags & DNXC_DATA_F_CHANGED)
            {
                SHR_IF_ERR_EXIT(dnxc_data_mgmt_table_entry_changed_get(unit, table, key1_index, key2_index, &changed));
            }
            if (changed)
            {
                /*
                 *  Entry changed - export it.
                 */
                PRT_ROW_ADD(entry_row_separator);
                /*
                 * entry_counter will give a root node to the entry nodes.
                 */
                PRT_CELL_SET("%d", entry_counter);
                /*
                 * Entry keys
                 */
                for (entry_row = 0; (entry_row < table->nof_keys) || (entry_row < table->nof_values); entry_row++)
                {
                    if (entry_row > 0)
                    {
                        /*
                         * 2nd and above lines of the same entry - create a line and skip the entry cell
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(1);
                    }
                    if (entry_row < table->nof_keys)
                    {
                        /*
                         * Key field
                         */
                        PRT_CELL_SET("%s", table->keys[entry_row].name);
                        PRT_CELL_SET("%d", entry_row == 0 ? key1_index : key2_index);
                    }
                    else
                    {
                        PRT_CELL_SKIP(2);
                    }
                    if (entry_row < table->nof_values)
                    {
                        char buffer[DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH];
                        /*
                         * Value field
                         */
                        PRT_CELL_SET("%s", table->values[entry_row].name);
                        /*
                         * Get value as string
                         */
                        SHR_IF_ERR_EXIT(table->entry_get(unit, buffer, key1_index, key2_index, entry_row));
                        PRT_CELL_SET("%s", buffer);
                    }
                    else
                    {
                        PRT_CELL_SKIP(2);
                    }
                }
                /*
                 * Mark that there is a reason to export the table (at least one entry)
                 */
                table_has_content = TRUE;
            }
            /*
             * count even if not exporting
             */
            entry_counter++;
        }
    }
    if (TRUE == table_has_content)
    {
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Iterate over a submodule's tables list and export the valid ones to xml.
 */
static shr_error_e
diag_dnxc_data_submodule_tables_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    int table_index;

    SHR_FUNC_INIT_VARS(unit);

    for (table_index = 0; table_index < submodule->nof_tables; table_index++)
    {
        int do_export;
        dnxc_data_table_t *table = &submodule->tables[table_index];

        /*
         * Check if data is supported / match flags
         */
        SHR_IF_ERR_EXIT(dnxc_data_utils_dump_verify
                        (unit, _dnxc_data[unit].state, table->flags, table->labels, flags, NULL, &do_export));
        if (do_export)
        {
            /*
             * Supported and requires exporting
             */
            SHR_IF_ERR_EXIT(diag_dnxc_data_table_export(unit, module, submodule, table, flags, sand_control));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Export submodule data to xml
 */
static shr_error_e
diag_dnxc_data_submodule_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Defines/Numerics
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_defines_export(unit, module, submodule, flags, sand_control));
    /*
     * Fetures
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_features_export(unit, module, submodule, flags, sand_control));
    /*
     * Tables
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_tables_export(unit, module, submodule, flags, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  data exprot command implementation
 * \see
 *  Man for 'data export'
 */
static shr_error_e
cmd_dnxc_data_export(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_index, submodule_index;
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    uint32 dump_flags = 0;
    int changed_only = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Parse options and set flags accordingly
     */
    SH_SAND_GET_BOOL("changed", changed_only);
    if (changed_only)
    {
        dump_flags |= DNXC_DATA_F_CHANGED;
        dump_flags |= DNXC_DATA_F_PROPERTY;
    }

    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        dnxc_data_module_t *module = &modules[module_index];
        /*
         * iterate over sub modules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            dnxc_data_submodule_t *submodule = &module->submodules[submodule_index];
            /*
             * Export submodule data
             */
            SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_export(unit, module, submodule, dump_flags, sand_control));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */
/**
 * \brief
 *   Vectors for DNX-Data export command
 */
static sh_sand_option_t dnxc_data_export_options[] = {
    {"changed", SAL_FIELD_TYPE_BOOL, "Export only changed properties", "yes"},
    {NULL}
};
static sh_sand_man_t dnxc_data_export_man = {
    "Export changed data to an xml file (can also print all changed data to the consol)",
    "Prints all the changed data in a format that is friendly to exporting to xml files. To perform the actual export,"
    "the 'file' option should be used with a .xml suffix to the file name.",
    "data export [file=<filename.xml>]",
    "'data export changed file=dnx_data.xml' - export all changed data to the file 'dnx_data.xml' in the current"
    " directory."
};
/* *INDENT-ON* */

/**
 * \brief DNXC DATA diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_data_cmds[] = {
    /*keyword,   action,                   command, options,                    man                   */
    {"dump",     cmd_dnxc_data_dump,       NULL,    dnxc_data_dump_options,     &dnxc_data_dump_man, NULL, dnxc_data_dump_options_invokes},
    {"export",   cmd_dnxc_data_export,     NULL,    dnxc_data_export_options,   &dnxc_data_export_man},
    {"info",     cmd_dnxc_data_info,       NULL,    dnxc_data_info_options,     &dnxc_data_info_man},
    {"property", cmd_dnxc_data_property,   NULL,    dnxc_data_property_options, &dnxc_data_property_man, NULL, dnxc_data_property_options_invokes},
    {NULL}
};

sh_sand_man_t sh_dnxc_data_man = {
    "Misc facilities for displaying dnxc data information",
    NULL,
    NULL,
    NULL,
};

const char cmd_dnxc_data_usage[] = "Display dnxc data information";
/* *INDENT-ON* */

/*
 * }
 */
