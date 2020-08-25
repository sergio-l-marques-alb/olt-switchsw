/** \file diag_dnxc_data_man.c
 * 
 * DEVICE DATA DIAG - diagnostic pack for module dnx_data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
/*shared*/
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */
/* *INDENT-OFF* */
sh_sand_option_t dnxc_data_man_options[] = {
    {NULL}
};
/* *INDENT-ON* */

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
sh_sand_man_t dnxc_data_man_man = {
    .brief    = "export xml format manual of all soc properties supported by device",
    .full     = "export xml format manual of all soc properties supported by device",
    .synopsis = "[file=<filename.xml>]",
    .examples = ""
};
/* *INDENT-ON* */

/**
 * \brief - print property info according to the input structure 
 */
static shr_error_e
diag_dnxc_data_man_property_add(
    int unit,
    void *submodule_node,
    int depth,
    dnxc_data_property_t * property,
    char *key_name,
    char *default_val_str)
{

    void *property_node, *description_node, *synopsis_node, *default_val_node;
    char property_name[SOC_PROPERTY_NAME_MAX];
    char synopsis_str[SOC_PROPERTY_NAME_MAX * 2], val_str[SOC_PROPERTY_NAME_MAX],
        default_val_str_actual[SOC_PROPERTY_NAME_MAX];
    int is_suffix, len, offset, map_index;
    SHR_FUNC_INIT_VARS(unit);

    /** property node */
    if ((property_node = dbx_xml_child_add(submodule_node, "property", depth++)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "property");
    }

    /** get property name */
    is_suffix = property->suffix != NULL && property->suffix[0] != 0 ? 1 : 0;
    len = sal_snprintf(property_name, SOC_PROPERTY_NAME_MAX,
                       "%s%s%s", property->name, is_suffix ? "_" : "", is_suffix ? property->suffix : "");
    if (len >= SOC_PROPERTY_NAME_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc property length unsupported\n");
    }
    RHDATA_SET_STR(property_node, "name", property_name);

    /** description */
    if ((description_node =
         dbx_xml_child_set_content_str(property_node, "description", property->doc, depth++)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "description");
    }
    dbx_xml_node_end(description_node, --depth);        /* inline end */

    /** synopsis - not defined in a case of custom method*/
    if (property->method != dnxc_data_property_method_custom)
    {
        /** initialize buffers strings*/
        val_str[0] = 0;
        default_val_str_actual[0] = 0;
        switch (property->method)
        {
            case dnxc_data_property_method_enable:
            case dnxc_data_property_method_port_enable:
            case dnxc_data_property_method_suffix_enable:
            case dnxc_data_property_method_port_suffix_enable:
                sal_snprintf(val_str, SOC_PROPERTY_NAME_MAX, "<0 | 1>");
                sal_snprintf(default_val_str_actual, SOC_PROPERTY_NAME_MAX, "%s", default_val_str);
                break;
            case dnxc_data_property_method_range:
            case dnxc_data_property_method_port_range:
            case dnxc_data_property_method_suffix_range:
            case dnxc_data_property_method_port_suffix_range:
                sal_snprintf(val_str, SOC_PROPERTY_NAME_MAX, "<[%u - %u]>", property->range_min, property->range_max);
                sal_snprintf(default_val_str_actual, SOC_PROPERTY_NAME_MAX, "%s", default_val_str);
                break;
            case dnxc_data_property_method_range_signed:
            case dnxc_data_property_method_port_range_signed:
            case dnxc_data_property_method_suffix_range_signed:
            case dnxc_data_property_method_port_suffix_range_signed:
                sal_snprintf(val_str, SOC_PROPERTY_NAME_MAX, "<[%d - %d]>", property->range_min, property->range_max);
                sal_snprintf(default_val_str_actual, SOC_PROPERTY_NAME_MAX, "%s", default_val_str);
                break;
            case dnxc_data_property_method_direct_map:
            case dnxc_data_property_method_port_direct_map:
            case dnxc_data_property_method_suffix_direct_map:
            case dnxc_data_property_method_port_suffix_direct_map:
                offset = sal_snprintf(val_str, SOC_PROPERTY_NAME_MAX, "<");
                for (map_index = 0; map_index < property->nof_mapping; map_index++)
                {
                    if (property->mapping[map_index].is_invalid != TRUE)
                    {
                        offset +=
                            sal_snprintf(val_str + offset, SOC_PROPERTY_NAME_MAX - offset, "%s | ",
                                         property->mapping[map_index].name);

                        /** check if it the default value */
                        if (property->mapping[map_index].is_default)
                        {
                            sal_snprintf(default_val_str_actual, SOC_PROPERTY_NAME_MAX, "%s",
                                         property->mapping[map_index].name);
                        }
                    }
                }
                sal_snprintf(val_str + offset - 3, SOC_PROPERTY_NAME_MAX - offset, ">");
                break;
            case dnxc_data_property_method_pbmp:
            case dnxc_data_property_method_port_pbmp:
            case dnxc_data_property_method_suffix_pbmp:
                sal_snprintf(val_str, SOC_PROPERTY_NAME_MAX, "<port bitmap>");
                sal_snprintf(default_val_str_actual, SOC_PROPERTY_NAME_MAX, "%s", default_val_str);
                break;
            case dnxc_data_property_method_str:
            case dnxc_data_property_method_port_str:
            case dnxc_data_property_method_suffix_str:
            case dnxc_data_property_method_port_suffix_str:
                sal_snprintf(val_str, SOC_PROPERTY_NAME_MAX, "<string>");
                sal_snprintf(default_val_str_actual, SOC_PROPERTY_NAME_MAX, "%s", default_val_str);
                break;
            default:
                /*
                 * do nothing
                 */
                break;
        }

        if (is_suffix)
        {
            if (key_name == NULL)
            {
                sal_sprintf(synopsis_str, "%s[_%s]=%s", property->name, property->suffix, val_str);
            }
            else
            {
                sal_sprintf(synopsis_str, "%s[_%s<%s>]=%s", property->name, property->suffix, key_name, val_str);
            }
        }
        else
        {
            sal_sprintf(synopsis_str, "%s=%s", property->name, val_str);
        }
        if ((synopsis_node = dbx_xml_child_set_content_str(property_node, "synopsis", synopsis_str, depth++)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "synopsis");
        }
        dbx_xml_node_end(synopsis_node, 0);     /* inline end */
        depth--;
        if ((default_val_node =
             dbx_xml_child_set_content_str(property_node, "default", default_val_str_actual, depth++)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "default");
        }
        dbx_xml_node_end(default_val_node, 0);  /* inline end */
        depth--;
    }

    dbx_xml_node_end(property_node, --depth);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_submodule_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *parent_node,
    dnxc_data_module_t * module,
    int submodule_index,
    int silent,
    int *should_print)
{
    dnxc_data_submodule_t *submodule;
    dnxc_data_table_t *table;
    dnxc_data_feature_t *feature;
    dnxc_data_define_t *define;
    void *submodule_node = NULL;
    int dump;
    int depth = 2;
    int table_index, feature_index, define_index, value_index;
    char default_val_str[SOC_PROPERTY_NAME_MAX];
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    submodule = &module->submodules[submodule_index];

    if (!silent)
    {
        submodule_node = dbx_xml_child_add(parent_node, "submodule", depth++);
        if (submodule_node == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "submodule");
        }
        RHDATA_SET_STR(submodule_node, "name", submodule->name);
        RHDATA_SET_STR(submodule_node, "description", submodule->doc);
    }

    /*
     * iterate of tables
     */
    for (table_index = 0; table_index < submodule->nof_tables; table_index++)
    {
        table = &submodule->tables[table_index];
        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, table->flags, table->labels, 0, NULL, &dump);
        SHR_IF_ERR_EXIT(rv);
        /** filter internal properties */
        if (table->flags & DNXC_DATA_F_PROPERTY_INTERNAL)
        {
            continue;
        }

        if (dump)
        {
            /*
             * check if data loaded by SoC property and soc property is documented
             */
            if (table->property.name != NULL && table->property.doc != NULL)
            {
                if (silent)
                {
                    *should_print = 1;
                    SHR_EXIT();
                }

                SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                (unit, submodule_node, depth, &table->property, table->keys[0].name,
                                 table->values[0].default_val));
            }

            for (value_index = 0; value_index < table->nof_values; value_index++)
            {
                /*
                 * check if data loaded be SoC property and documented
                 */
                if (table->values[value_index].property.name != NULL && table->values[value_index].property.doc != NULL)
                {
                    if (silent)
                    {
                        *should_print = 1;
                        SHR_EXIT();
                    }

                    SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                    (unit, submodule_node, depth, &table->values[value_index].property,
                                     table->keys[0].name, table->values[value_index].default_val));
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
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, feature->flags, feature->labels, 0, NULL, &dump);
        SHR_IF_ERR_EXIT(rv);
        /** filter internal properties */
        if (feature->flags & DNXC_DATA_F_PROPERTY_INTERNAL)
        {
            continue;
        }

        if (dump)
        {
            /*
             * check if data loaded be SoC property and documented
             */
            if (feature->property.name != NULL && feature->property.doc != NULL)
            {
                if (silent)
                {
                    *should_print = 1;
                    SHR_EXIT();
                }

                SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                (unit, submodule_node, depth, &feature->property, NULL,
                                 feature->default_data ? "1" : "0"));
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
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, define->flags, define->labels, 0, NULL, &dump);
        SHR_IF_ERR_EXIT(rv);
        /** filter internal properties */
        if (define->flags & DNXC_DATA_F_PROPERTY_INTERNAL)
        {
            continue;
        }

        if (dump)
        {

            /*
             * check if data loaded be SoC property and documented
             */
            if (define->property.name != NULL && define->property.doc != NULL)
            {
                if (silent)
                {
                    *should_print = 1;
                    SHR_EXIT();
                }

                sal_snprintf(default_val_str, SOC_PROPERTY_NAME_MAX, "%d", define->default_data);
                SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                (unit, submodule_node, depth, &define->property, NULL, default_val_str));
            }
        }       /* Dump numeric */
    }   /* numeric iterator */
    if (!silent)
    {
        dbx_xml_node_end(submodule_node, --depth);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_module_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *parent_node,
    int module_index,
    int silent,
    int *should_print)
{
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    dnxc_data_module_t *module;
    void *module_node = NULL;
    int submodule_index;
    SHR_FUNC_INIT_VARS(unit);

    module = &modules[module_index];

    /** skip module with no soc properties */
    *should_print = 0;

    if (!silent)
    {
        if ((module_node = dbx_xml_child_add(parent_node, "module", 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "module");
        }
        RHDATA_SET_STR(module_node, "name", module->name);
    }
    /*
     * iterate of submodules
     */
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        *should_print = 0;

        SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_man
                        (unit, args, sand_control, module_node, module, submodule_index, 1, should_print));

        if (*should_print)
        {
            if (silent)
            {
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_man
                                (unit, args, sand_control, module_node, module, submodule_index, silent, should_print));
            }
        }
    }   /* submodule iterator */

    if (!silent)
    {
        dbx_xml_node_end(module_node, 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
shr_error_e
cmd_dnxc_data_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;
    void *top_node;
    void *device_node;
    int should_print;
    int module_index;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file", filename);
    /*
     * Validate input
     */
    if (ISEMPTY(filename))
    {
        filename = "manual.xml";
    }
    else
    {
        if (dbx_file_get_type(filename) != DBX_FILE_XML)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "File must have .xml suffix. File name was:\"%s\"\n", filename);
        }
    }
    /*
     * Create XML Document
     */
    if ((top_node = dbx_file_get_xml_top(unit, filename, "top",
                                        CONF_OPEN_CREATE | CONF_OPEN_OVERWRITE | CONF_OPEN_CURRENT_LOC)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Failed to create:\"%s\"\n", filename);
    }

    if ((device_node = dbx_xml_child_add(top_node, "device", 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "module");
    }
    RHDATA_SET_STR(device_node, "name", (char *) soc_dev_name(unit));

    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        should_print = 0;
        /** first check if should print */
        SHR_IF_ERR_EXIT(diag_dnxc_data_module_man
                        (unit, args, sand_control, device_node, module_index, 1, &should_print));

        /** print if required */
        if (should_print)
        {
            SHR_IF_ERR_EXIT(diag_dnxc_data_module_man
                            (unit, args, sand_control, device_node, module_index, 0, &should_print));
        }
    }   /* module iterator */

    dbx_xml_node_end(device_node, 0);
    dbx_xml_top_save(top_node, filename);
    dbx_xml_top_close(top_node);

exit:
    SHR_FUNC_EXIT;
}
