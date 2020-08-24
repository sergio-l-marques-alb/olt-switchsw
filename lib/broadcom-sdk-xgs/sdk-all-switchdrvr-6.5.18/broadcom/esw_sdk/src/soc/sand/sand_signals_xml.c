/** \File: sand_signals.c
 * Signal Driver - init from XML DB, access and filtering
 */
/*
 * $Id: Exp $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#if !defined(__KERNEL__)
#include <stdlib.h>
#endif

#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_pparse.h>

#include <bcm/types.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/arr/arr.h>
#include <soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#endif

#include "sand_signals_internal.h"

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

/*
 * {
 */
/**
 * \brief Fetch pointer to the stage based either on PP or adapter name
 * \param [in] unit     - unit id
 * \param [in] pp_block - pointer to the block, stage should belong to
 * \param [in] stage_n  - array of commands under the root
 * \return
 *     \retval pointer to the stage if found one
 *     \retval NULL if stage was not found in the block
 */
static pp_stage_t *
sand_signal_stage_get(
    int unit,
    device_t *device,
    pp_block_t *pp_block,
    char *stage_n)
{
    pp_stage_t *pp_stage;
    int i_st;

#ifdef BCM_DNX_SUPPORT
    /*
     * Replace stage if it was KBR encoded, we are not interested in return status
     * It was either modified or not in both cases continue with what we have
     */
    if (SOC_IS_DNX(unit))
    {
        dnx_pp_stage_kbr_resolve(unit, stage_n, stage_n);
    }
#endif

    for(i_st = 0; i_st < pp_block->stage_num; i_st++)
    {
        pp_stage = &pp_block->stages[i_st];

        if(!sal_strcasecmp(stage_n, pp_stage->name))
            return pp_stage;
    }

    
/*    return utilex_rhlist_entry_get_by_name(device->phydb_list, stage_n); */
    return NULL;
}

/*
 * Read expansion information from XML for structure and fields
 * No verification is performed, will be done by sand_signal_expand_init
 */
static void
sand_signal_expand_read(
    xml_node cur,
    expansion_t * expansion)
{
    xml_node curOption;
    expansion_option_t *option;

    /*
     * Read expansion from XML and dynamic options if any, they will be verified later when all the structures will be
     * scanned from XML to memory
     */
    RHDATA_GET_STR_DEF_NULL(cur, "expansion", expansion->name);

    if (!sal_strcasecmp(expansion->name, "Dynamic"))
    {
        RHDATA_GET_NODE_NUM(expansion->option_num, cur, "option");
        option = expansion->options = utilex_alloc(expansion->option_num * sizeof(expansion_option_t));
        RHDATA_ITERATOR(curOption, cur, "option")
        {
            char trim_str[RHNAME_MAX_SIZE];
            RHDATA_GET_STR_CONT(curOption, "expansion", option->name);
            RHDATA_DELETE(curOption, "expansion");
            RHDATA_GET_STR_DEF_NULL(curOption, "from", option->from);
            RHDATA_DELETE(curOption, "from");
            RHDATA_GET_STR_DEF_NULL(curOption, "block", option->block);
            RHDATA_DELETE(curOption, "block");
            RHDATA_GET_STR_DEF_NULL(curOption, "trim", trim_str);
            RHDATA_DELETE(curOption, "trim");

            if(!sal_strcasecmp(trim_str, "ms"))
            {
                option->trim_side = TRIM_MS;
            }
            else if(!sal_strcasecmp(trim_str, "ls"))
            {
                option->trim_side = TRIM_LS;
            }
            else
            {
                option->trim_side = TRIM_NONE;
            }

            dbx_xml_property_get_all(curOption, option->param, DSIG_OPTION_PARAM_MAX_NUM);
            option++;
        }
    }
    return;
}

static int
sand_signal_param_read(
    rhlist_t *param_list,
    xml_node nodeTop)
{
    xml_node curSubTop, cur, curSub;
    int res = _SHR_E_NONE;

    /*
     * Read all available parameters with their possible values
     */
    if ((curSubTop = dbx_xml_child_get_first(nodeTop, "signal-params")) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_ITERATOR(cur, curSubTop, "signal")
    {
        char name[RHNAME_MAX_SIZE];
        rhhandle_t temp;
        sigparam_t* sigparam;
        sigparam_value_t* sigparam_value;
        int value;

        RHDATA_GET_STR_CONT(cur, "name", name);

        if (utilex_rhlist_entry_add_tail(param_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            break;

        sigparam = temp;

        RHDATA_GET_STR_DEF_NULL(cur, "block", sigparam->block_n);
        RHDATA_GET_STR_DEF_NULL(cur, "from", sigparam->from_n);
        RHDATA_GET_STR_DEF_NULL(cur, "to", sigparam->to_n);
        RHDATA_GET_STR_DEF_NULL(cur, "default", sigparam->default_str);
        RHDATA_GET_STR_DEF_NULL(cur, "dbal", sigparam->dbal_n);
        /*
         * If there is dbal property size if not must
         */
        if(ISEMPTY(sigparam->dbal_n))
        {
            RHDATA_GET_INT_CONT(cur, "size", sigparam->size);
        }
        else
        {
            RHDATA_GET_INT_DEF(cur, "size", sigparam->size, 0);
        }

        if ((sigparam->value_list = utilex_rhlist_create("values", sizeof(sigparam_value_t), 0)) == NULL)
            goto exit;

        RHDATA_ITERATOR(curSub, cur, "entry")
        {
            RHDATA_GET_STR_CONT(curSub, "name", name);
            RHDATA_GET_INT_CONT(curSub, "value", value);
            if (utilex_rhlist_entry_add_tail(sigparam->value_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                break;
            sigparam_value = temp;
            sigparam_value->value = value;
        }
    }

exit:
    return res;
}

static void
sand_signal_field_type_set(
    xml_node cur,
    sigstruct_field_t *sigstruct_field)
{
    char name[RHNAME_MAX_SIZE];
    RHDATA_GET_STR_DEF_NULL(cur, "type", name);
    /*
     * If no type was assigned, assume uint32 for fields fitting into 32 or array for longer one
     */
    if(ISEMPTY(name))
    {
        if(sigstruct_field->size <= 32)
        {
            sigstruct_field->type = SAL_FIELD_TYPE_UINT32;
        }
        else
        {
            sigstruct_field->type = SAL_FIELD_TYPE_ARRAY32;
        }
    }
    else
    {
        if((sigstruct_field->type = sal_field_type_by_name(name)) == SAL_FIELD_TYPE_MAX)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("field:%s type:%s is unsupported\n"),RHNAME(sigstruct_field),  name));
        }
    }
}

static shr_error_e
sand_signal_expand_verify(
        device_t * device,
        char *name,
        char *from_n,
        char *to_n,
        char *block_n,
        expansion_t * expansion,
        char *resolution)
{
    /*
     * First check if there is expansion at all
     */
    if (ISEMPTY(expansion->name))
    {
        return _SHR_E_EMPTY;
    }
    /*
     * If the expansion is dynamic check all options
     */
    else if(!sal_strcasecmp(expansion->name, "Dynamic"))
    {
        int i_opt;
        expansion_option_t *option = expansion->options;
        for (i_opt = 0; i_opt < expansion->option_num; i_opt++)
        {
            if(sand_signal_struct_get(device, option->name, from_n, to_n, block_n) == NULL)
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META("Expansion:%s does not exist\n"), option->name));
                return _SHR_E_NOT_FOUND;
            }
            option++;
        }
    }
    /*
     * Now we should check if specific expansion exists
     */
    else if(sand_signal_struct_get(device, expansion->name, from_n, to_n, block_n) == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No signal expansion:%s for %s\n"), expansion->name, name));
        SET_EMPTY(expansion->name);
        return _SHR_E_NOT_FOUND;
    }
    else if (!ISEMPTY(resolution) && (sand_signal_resolve_get(device, resolution, from_n, to_n, block_n) == NULL))
    {
        /*
         * If resolution is not empty look if it exists
         */
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No signal resolution:%s on %s\n"), resolution, name));
        SET_EMPTY(resolution);
        return _SHR_E_NOT_FOUND;
    }

    return _SHR_E_NONE;
}

static void
sand_signal_expand_init(
    device_t * device)
{
    sigstruct_t *sigstruct;
    /*
     * Go through all structures & their fields and fill expansion were available
     */
    RHITERATOR(sigstruct, device->struct_list)
    {
        sigstruct_field_t *sigstruct_field;
        /*
         * If plugin installed no need for further checks - responsibility of plugin
         */
        if(sigstruct->expansion_cb)
        {
            continue;
        }
        /*
         * First verify that structure expansion exists and is not dynamic,
         * if yes field are irrelevant even if they exist
         */
        if(sand_signal_expand_verify(device, RHNAME(sigstruct), sigstruct->from_n, sigstruct->to_n, sigstruct->block_n,
                                                         &sigstruct->expansion_m, sigstruct->resolution) == _SHR_E_NONE)
        {
            if((sigstruct->field_list != NULL) && (RHLNUM(sigstruct->field_list) != 0))
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("Structure:\"%s\" is expanded as \"%s\""
                                "Internal fields will be ignored\n"), RHNAME(sigstruct), sigstruct->expansion_m.name));
            }
            continue;
        }

        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            /*
             * Do the same verification on field, what is different that we need to check if field name is expansion
             */
            if(sand_signal_expand_verify(device, RHNAME(sigstruct_field), sigstruct->from_n, sigstruct->to_n,
                    sigstruct->block_n, &sigstruct_field->expansion_m, sigstruct_field->resolution) == _SHR_E_NONE)
            {
                /*
                 * Expansion found and it is valid, go to next field
                 */
                continue;
            }
            /*
             * When expansion is empty(invalid) first check may be it can be expanded by name
             */
            if (sand_signal_struct_get(device, RHNAME(sigstruct_field),
                                                        sigstruct->from_n, sigstruct->to_n, sigstruct->block_n) != NULL)
            {
                sal_strncpy(sigstruct_field->expansion_m.name, RHNAME(sigstruct_field), RHNAME_MAX_SIZE - 1);
            }
            else if (sand_signal_resolve_get(device, RHNAME(sigstruct_field),
                                                        sigstruct->from_n, sigstruct->to_n, sigstruct->block_n) != NULL)
            {
                /*
                 * If there is match - attribute serves as resolution
                 */
                sal_strncpy(sigstruct_field->resolution, RHNAME(sigstruct_field), RHNAME_MAX_SIZE - 1);
            }
        }
    }

    return;
}

static int
sand_signal_struct_read(
    int unit,
    rhlist_t *struct_list,
    xml_node nodeTop)
{
    xml_node curSubTop;
    xml_node cur;
    xml_node curSub;
    int res = _SHR_E_NONE;
    device_t * device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if ((curSubTop = dbx_xml_child_get_first(nodeTop, "signal-structures")) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    /*
     * loop through entries
     */
    RHDATA_ITERATOR(cur, curSubTop, "structure")
    {
        char name[RHNAME_MAX_SIZE];
        rhhandle_t temp;
        sigstruct_t *sigstruct;
        sigstruct_field_t *sigstruct_field;
        int size;

        sal_memset(name,0,sizeof(name));
        RHDATA_GET_STR_CONT(cur, "name", name);
        RHDATA_GET_INT_CONT(cur, "size", size);

        if(size > DSIG_MAX_SIZE_BITS)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Structure:%s size:%d is more than maximum:%d\n"),
                                                                                    name, size, DSIG_MAX_SIZE_BITS));
            continue;
        }

        if (utilex_rhlist_entry_add_tail(struct_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            break;

        sigstruct = temp;

        sigstruct->size = size;
        RHDATA_GET_STR_DEF_NULL(cur, "block", sigstruct->block_n);
        RHDATA_GET_STR_DEF_NULL(cur, "from", sigstruct->from_n);
        RHDATA_GET_STR_DEF_NULL(cur, "to", sigstruct->to_n);

        RHDATA_GET_STR_DEF_NULL(cur, "order", name);
        RHDATA_GET_STR_DEF_NULL(cur, "resolution", sigstruct->resolution);
        if(!sal_strcasecmp(name, "msb"))
            sigstruct->order = PRINT_BIG_ENDIAN;
        else
            sigstruct->order = PRINT_LITTLE_ENDIAN;

        RHDATA_GET_STR_DEF_NULL(cur, "plugin", name);
        if(!ISEMPTY(name))
        {
            /*
             * Record plugin name
             */
            sal_strncpy(sigstruct->plugin_n, name, RHKEYWORD_MAX_SIZE - 1);
            /*
             * We have read plugin name from XML, not we assume that callback name if <plugin>_decode
             */
            if(!sal_strcasecmp(name, "pparse"))
            {
                sigstruct->expansion_cb = pparse_decode;
            }
#ifdef BCM_DNX_SUPPORT
            /** dbal and dnx_arr are supported only for DNX */
            else  if (SOC_IS_DNX(unit))
            {
                if(!sal_strcasecmp(name, "dbal"))
                {
                    sigstruct->expansion_cb = dbal_decode;
                }
                else if(!sal_strcasecmp(name, "dnx_arr"))
                {
                    sigstruct->expansion_cb = dnx_arr_decode;
                }
                else if(!sal_strcasecmp(name, "nasid_context"))
                {
                    sigstruct->expansion_cb = nasid_context_decode;
                }
            }
#endif /* BCM_DNX_SUPPORT */
            if(sigstruct->expansion_cb == NULL)
            {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Plugin:%s for struct:%s is not supported\n"),
                                                                                            name, RHNAME(sigstruct)));
            }
            else
            {   /*
                 * We succeeded to load plugin callback, get expansion name
                 * If there is none signal name itself will be used
                 * In this case even if there are fields inside the structure they are not relevant,
                 * Plugin takes precedence, field may stay for the situation without dynamic loading
                 */
                RHDATA_GET_STR_DEF_NULL(cur, "expansion", sigstruct->expansion_m.name);
                continue;
            }
        }

        sand_signal_expand_read(cur, &sigstruct->expansion_m);

        if ((sigstruct->field_list = utilex_rhlist_create("fields", sizeof(sigstruct_field_t), 0)) == NULL)
            goto exit;

        /*
         * Check if the structure is array
         */
        {
            int element_size, i_el;
            RHDATA_GET_INT_DEF(cur, "array", sigstruct->array_count, 0);
            RHDATA_GET_INT_DEF(cur, "start", sigstruct->array_start, 1);
            if(sigstruct->array_count != 0)
            {
                if((size % sigstruct->array_count) != 0)
                {
                    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Array struct %s size %d is not divisible by %d\n"),
                                                                      RHNAME(sigstruct), size, sigstruct->array_count));
                }
                element_size = size/sigstruct->array_count;
                for(i_el = 0; i_el < sigstruct->array_count; i_el++)
                {
                    char el_name[4];
                    sal_snprintf(el_name, RHNAME_MAX_SIZE - 1, "%d", i_el + sigstruct->array_start);

                    if (utilex_rhlist_entry_add_tail(sigstruct->field_list, el_name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                        break;
                    sigstruct_field = temp;

                    if(sigstruct->order == PRINT_LITTLE_ENDIAN)
                    {
                        sigstruct_field->start_bit = element_size * i_el;
                    }
                    else
                    {
                        sigstruct_field->start_bit = size - element_size * (i_el + 1);
                    }
                    sigstruct_field->end_bit   = sigstruct_field->start_bit + element_size - 1;

                    sigstruct_field->size      = element_size;
                    sand_signal_field_type_set(cur, sigstruct_field);

                    sal_strncpy(sigstruct_field->expansion_m.name, sigstruct->expansion_m.name, RHNAME_MAX_SIZE - 1);
                    sigstruct_field->expansion_m.option_num = sigstruct->expansion_m.option_num;
                    /*
                     * For dynamic expansion if option is also indexed, add it to option name as it is for field name
                     */
                    if(sigstruct->expansion_m.options != NULL)
                    {
                        int i_opt;
                        expansion_option_t *option;
                        /*
                         * Expansion already extracted by parent, so just copy it
                         * Copy expansion name, number of options and set options after allocation the spaces for copy
                         */
                        option = sigstruct_field->expansion_m.options
                                  = utilex_alloc(sigstruct_field->expansion_m.option_num * sizeof(expansion_option_t));

                        sal_memcpy(sigstruct_field->expansion_m.options, sigstruct->expansion_m.options,
                                                 sigstruct_field->expansion_m.option_num * sizeof(expansion_option_t));

                        for(i_opt = 0; i_opt < sigstruct_field->expansion_m.option_num; i_opt++, option++)
                        {
                            int i_param;
                            attribute_param_t *attribute_param = option->param;
                            for(i_param = 0; i_param < DSIG_OPTION_PARAM_MAX_NUM; i_param++, attribute_param++)
                            {
                                sigstruct_t *param_sigstruct;
                                if(ISEMPTY(attribute_param->name))
                                    break;
                                /*
                                 * If attribute parameter is an arrayed structure name
                                 * assume that it should have the same index as field, otherwise it is plain parameter
                                 */
                                if(((param_sigstruct = sand_signal_struct_get(device, attribute_param->name,
                                                sigstruct->from_n, sigstruct->to_n, sigstruct->block_n)) == NULL) ||
                                                                                    (param_sigstruct->array_count == 0))
                                    continue;

                                sal_snprintf(attribute_param->name, RHNAME_MAX_SIZE - 1, "%s.%d", attribute_param->name,
                                                                                   i_el + param_sigstruct->array_start);
                            }
                        }
                    }
                    RHDATA_GET_STR_DEF_NULL(cur, "resolution", sigstruct_field->resolution);
                }
                /*
                 * Clean expansion from structure, for array only fields may be expanded
                 */
                if(sigstruct->expansion_m.options != NULL)
                {
                    sal_free(sigstruct->expansion_m.options);
                }
                sal_memset(&sigstruct->expansion_m, 0, sizeof(expansion_t));
            }
        }
        /*
         * loop through entries
         */
        RHDATA_ITERATOR(curSub, cur, "field")
        {
            char str_value[RHNAME_MAX_SIZE];

            RHDATA_GET_STR_CONT(curSub, "name", name);

            if (utilex_rhlist_entry_add_tail(sigstruct->field_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                break;

            sigstruct_field = temp;

            RHDATA_GET_STR_CONT(curSub, "bits", name);

            if (sand_signal_range_parse(name, &sigstruct_field->start_bit, &sigstruct_field->end_bit,
                                                                    sigstruct->order, sigstruct->size) !=  _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Field:%s.%s, has bad bits range:%s\n"),
                                                                    RHNAME(sigstruct), RHNAME(sigstruct_field), name));
                continue;
            }

            sigstruct_field->size = sigstruct_field->end_bit + 1 - sigstruct_field->start_bit;
            if(sigstruct_field->size > DSIG_FIELD_MAX_SIZE_BITES)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("field:%s.%s size(%d) is more than maximum :%d\n"),
                           RHNAME(sigstruct), RHNAME(sigstruct_field), sigstruct_field->size, DSIG_FIELD_MAX_SIZE_BITES));
            }
            RHDATA_GET_STR_DEF_NULL(curSub, "condition", sigstruct_field->cond_attribute);
            RHDATA_GET_STR_DEF_NULL(curSub, "resolution", sigstruct_field->resolution);
            /*
             * Fetch field type name and convert it to enum
             */
            sand_signal_field_type_set(curSub, sigstruct_field);

            RHDATA_GET_STR_DEF_NULL(curSub, "default", str_value);
            /*
             * If default was not defined assume null value
             * Nothing to do - just keep initial state
             */
            if(!ISEMPTY(str_value))
            {
                if(sand_signal_str_to_value(unit, sigstruct_field->type, str_value, sigstruct_field->size,
                                                                    sigstruct_field->default_value) != _SHR_E_NONE)
                {
                    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Error extracting value of %s.%s from %s\n"),
                                                                 RHNAME(sigstruct), RHNAME(sigstruct_field),  str_value));
                }
            }
            sand_signal_expand_read(curSub, &sigstruct_field->expansion_m);
        }
    }

exit:
    return res;
}

static shr_error_e
sand_signal_struct_init(
    int unit,
    device_t * device)
{
    xml_node curTop = (xml_node)0;
    xml_node curSubTop = (xml_node)0;
    xml_node cur = (xml_node)0;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(filename,0,sizeof(filename));
    if (device->struct_list != NULL)
    {
        /*
         * Already initialized
         */
        SHR_EXIT();
    }

    if ((device->param_list = utilex_rhlist_create("param_list", sizeof(sigparam_t), 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    if ((device->struct_list = utilex_rhlist_create("struct_list", sizeof(sigstruct_t), 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    /* All structure/param files will be listed in parsing-objects section of DNX-Devices */
    if ((curTop = dbx_file_get_xml_top(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if ((curSubTop = dbx_xml_child_get_first(curTop, "parsing-objects")) == NULL)
    {
        /*
         * No parsing objects - just leave
         */
        SHR_EXIT();
    }

    RHDATA_ITERATOR(cur, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(cur, "type", type);
        if(!sal_strcasecmp(type, "structs"))
        {
            xml_node curFile;
            int device_specific = 0;
            int flags = CONF_OPEN_ALTER_LOC | CONF_OPEN_NO_ERROR_REPORT;

            RHDATA_GET_INT_DEF(cur, "device_specific", device_specific, 0);
            if(device_specific == TRUE)
            {
                flags |= CONF_OPEN_PER_DEVICE;
            }

            RHDATA_GET_STR_CONT(cur, "file", filename);
            if ((curFile = dbx_file_get_xml_top(unit, filename, "top", flags)) == NULL)
                continue;

            sand_signal_param_read(device->param_list, curFile);
            sand_signal_struct_read(unit, device->struct_list, curFile);
            dbx_xml_top_close(curFile);
        }
    }

    dbx_xml_top_close(curTop);

    /*
     * Check if there is Unsupported DebugSignals, if not - not an error - continue as usual
     */
    if ((curTop = dbx_file_get_xml_top(unit, "UnsupportedDebugSignals.xml", "signals", CONF_OPEN_NO_ERROR_REPORT)) != NULL)
    {
        rhhandle_t temp = NULL;
        unsupported_t *unsupported_signal;
        int this_is_adapter;
        soc_control_t *soc;
        char *this_chip_n;

        soc = SOC_CONTROL(unit);
        this_chip_n = SOC_CHIP_NAME(soc->chip_driver->type);


#ifdef ADAPTER_SERVER_MODE
        this_is_adapter = 1;
#else
        this_is_adapter = 0;
#endif
        /*
         * Create list that will include all legitimate signals not supported for the device
         */
        if ((device->unsupported_list = utilex_rhlist_create("unsupported_list", sizeof(unsupported_t), 0)) == NULL)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
            SHR_EXIT();
        }
        /*
         * Read the list of unsupported signals. The difference will be in return status in signal get routines
         *   For not supported _SHR_E_UNAVAIL will be returned
         *   For not found     _SHR_E_NOT_FOUND
         */
        RHDATA_ITERATOR(cur, curTop, "signal")
        {
            xml_node curDevice;
            char chip_n[RHNAME_MAX_SIZE];
            int is_adapter;
            /*
             * Check if the unsupported device is a current one
             */
            RHDATA_ITERATOR(curDevice, cur, "device")
            {
                RHDATA_GET_STR_CONT(curDevice, "chip", chip_n);
                RHDATA_GET_INT_DEF(curDevice, "adapter", is_adapter, 0);
                if(!sal_strcasecmp(this_chip_n, chip_n) && (this_is_adapter == is_adapter))
                {
                    char signal_n[RHNAME_MAX_SIZE];
                    /*
                     * Add this element to the list
                     */
                    RHDATA_GET_STR_DEF_NULL(cur, "name", signal_n);
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(device->unsupported_list, signal_n,
                                                                                         RHID_TO_BE_GENERATED, &temp));
                    unsupported_signal = (unsupported_t *)temp;
                    RHDATA_GET_STR_DEF_NULL(cur, "block", unsupported_signal->block_n);
                    RHDATA_GET_STR_DEF_NULL(cur, "from", unsupported_signal->from_n);
                    break;
                }
            }
        }

        dbx_xml_top_close(curTop);
    }

    /*
     * Check if there is N/A restricted DebugSignals, if not - not an error - continue as usual
     */
    if ((curTop = dbx_file_get_xml_top(unit, "RestrictedDebugSignals.xml", "signals", CONF_OPEN_NO_ERROR_REPORT)) != NULL)
    {
        rhhandle_t temp = NULL;
        restricted_t *restricted_signal;
        int this_is_adapter;
        soc_control_t *soc;
        char *this_chip_n;

        soc = SOC_CONTROL(unit);
        this_chip_n = SOC_CHIP_NAME(soc->chip_driver->type);


#ifdef ADAPTER_SERVER_MODE
        this_is_adapter = 1;
#else
        this_is_adapter = 0;
#endif
        /*
         * Create list that will include all legitimate signals that are present but restricted for the device
         */
        if ((device->restricted_list = utilex_rhlist_create("restricted_list", sizeof(restricted_t), 0)) == NULL)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
            SHR_EXIT();
        }
        /*
         * Read the list of unsupported signals. The difference will be that these signals would return N/A for signal get routines
         */
        RHDATA_ITERATOR(cur, curTop, "signal")
        {
            xml_node curDevice;
            char chip_n[RHNAME_MAX_SIZE];
            int is_adapter;
            /*
             * Check if the unsupported device is a current one
             */
            RHDATA_ITERATOR(curDevice, cur, "device")
            {
                RHDATA_GET_STR_CONT(curDevice, "chip", chip_n);
                RHDATA_GET_INT_DEF(curDevice, "adapter", is_adapter, 0);
                if(!sal_strcasecmp(this_chip_n, chip_n) && (this_is_adapter == is_adapter))
                {
                    char signal_n[RHNAME_MAX_SIZE];
                    /*
                     * Add this element to the list
                     */
                    RHDATA_GET_STR_DEF_NULL(cur, "name", signal_n);
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(device->restricted_list, signal_n,
                                                                                         RHID_TO_BE_GENERATED, &temp));
                    restricted_signal = (restricted_t *)temp;
                    RHDATA_GET_STR_DEF_NULL(cur, "block", restricted_signal->block_n);
                    RHDATA_GET_STR_DEF_NULL(cur, "to", restricted_signal->to_n);
                    break;
                }
            }
        }

        dbx_xml_top_close(curTop);
    }

    sand_signal_expand_init(device);

exit:
    SHR_FUNC_EXIT;
}

static void
sand_signal_struct_deinit(
    device_t * device)
{
    sigstruct_t *sigstruct;
    sigparam_t* sigparam;

    RHITERATOR(sigstruct, device->struct_list)
    {
        sigstruct_field_t *sigstruct_field;
        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            utilex_free(sigstruct_field->expansion_m.options);
        }
        utilex_free(sigstruct->expansion_m.options);
        if(sigstruct->field_list)
            utilex_rhlist_free_all(sigstruct->field_list);
    }
    utilex_rhlist_free_all(device->struct_list);
    device->struct_list = NULL;

    RHITERATOR(sigparam, device->param_list)
    {
        utilex_rhlist_free_all(sigparam->value_list);
    }
    utilex_rhlist_free_all(device->param_list);
    device->param_list = NULL;

    if(device->unsupported_list)
        utilex_rhlist_free_all(device->unsupported_list);
    device->unsupported_list = NULL;

    if(device->restricted_list)
        utilex_rhlist_free_all(device->restricted_list);
    device->restricted_list = NULL;

    return;
}
shr_error_e
sand_qual_signal_init(
    int unit,
    char *db_name,
    char *qual_signals_db,
    pp_stage_t * pp_stage)
{
    void *curTop, *cur;
    internal_signal_t *cur_internal_signal;
    SHR_FUNC_INIT_VARS(NO_UNIT);
    if ((curTop = dbx_file_get_xml_top(unit, qual_signals_db, "SignalInfo", CONF_OPEN_PER_DEVICE)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "No section SignalInfo in:%s%s%s\n", db_name, EMPTY, EMPTY);
    }
    RHDATA_GET_INT_STOP(curTop, "size0", pp_stage->buffer0_size);
    RHDATA_GET_INT_DEF(curTop,  "size1", pp_stage->buffer1_size, 0);
    RHDATA_GET_NODE_NUM(pp_stage->number, curTop, "signal");
    if (pp_stage->number == 0)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "No pp stages in:%s%s%s\n", db_name, EMPTY, EMPTY);
    }
    cur_internal_signal = pp_stage->signals = utilex_alloc(sizeof(internal_signal_t) * pp_stage->number);
    RHDATA_ITERATOR(cur, curTop, "signal")
    {
        dbx_xml_child_get_content_str(cur, "name",    cur_internal_signal->name, RHNAME_MAX_SIZE);
        dbx_xml_child_get_content_int(cur, "offset",  &cur_internal_signal->offset);
        dbx_xml_child_get_content_int(cur, "size",    &cur_internal_signal->size);
        dbx_xml_child_get_content_int(cur, "buffer",  &cur_internal_signal->buffer);
        dbx_xml_child_get_content_str(cur, "verilog", cur_internal_signal->hw, RHSTRING_MAX_SIZE);
        cur_internal_signal++;
    }
    dbx_xml_top_close(curTop);
exit:
    SHR_FUNC_EXIT;
}

char *
sand_signal_metadata_description(
        device_t * device,
        char *attribute)
{
    metadata_t *metadata;

    if((metadata = utilex_rhlist_entry_get_by_name(device->metadata_list, attribute)) != NULL)
    {
        if(!ISEMPTY(metadata->description))
        {
            return metadata->description;
        }
    }

    return NULL;
}

/**
 * \brief Init list of metadata with descriptions per device
 * \param [in] unit     - unit id
 * \param [in] device   - pointer to device structure
 * \return
 *     \retval _SHR_E_NONE if all went smooth
 */
static shr_error_e
sand_signal_metadata_init(
        int unit,
        device_t * device)
{
    void *curTop = NULL, *curMeta;
    rhhandle_t rhhandle = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if(device->metadata_list != NULL)
    {
        /*
         * Already initialized
         */
        SHR_EXIT();
    }

    if ((curTop = dbx_file_get_xml_top(unit, "Metadata.xml", "MetadataList",
                                                             CONF_OPEN_NO_ERROR_REPORT | CONF_OPEN_ALTER_LOC)) == NULL)
    {
        /*
         * No metadata file is not considered an error
         */
        SHR_EXIT();
    }

    if ((device->metadata_list = utilex_rhlist_create("metadata_list", sizeof(metadata_t), 1)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for db-list on:%s\n", soc_dev_name(unit));
    }
    /*
     * loop through entries
     */
    RHDATA_ITERATOR(curMeta, curTop, "Metadata")
    {
        char metadata_n[RHNAME_MAX_SIZE];
        char description[SIGNAL_DESCRIPTION_MAX_SIZE];
        metadata_t *metadata;

        /*
         * Fetch Metadata Name
         */
        dbx_xml_child_get_content_str(curMeta, "Name", metadata_n, RHNAME_MAX_SIZE);
        /*
         * If no description or it is empty skip the metadata
         */
        dbx_xml_child_get_content_str(curMeta, "Description", description , SIGNAL_DESCRIPTION_MAX_SIZE);
        if(ISEMPTY(description) || ((description[0] == ' ') && (description[1] == 0)))
            continue;
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(device->metadata_list, metadata_n, RHID_TO_BE_GENERATED,
                                                                                                         &rhhandle));
        metadata = (metadata_t *)rhhandle;
        sal_strncpy(metadata->description, description, SIGNAL_DESCRIPTION_MAX_SIZE - 1);
    }

exit:
    if(curTop != NULL)
    {
        dbx_xml_top_close(curTop);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief Init list of memories(physical DB) per device
 * \param [in] unit     - unit id
 * \param [in] device   - pointer to device structure
 * \param [in] curTop   - handle to the top of PP.XML
 * \return
 *     \retval _SHR_E_NONE if all went smooth
 */
static shr_error_e
sand_signal_phydb_init(
        int unit,
        device_t * device,
        void *curTop)
{
    void *curSubTop, *curPhyDB;

    SHR_FUNC_INIT_VARS(unit);

    if ((curSubTop = dbx_xml_child_get_first(curTop, "phydb-list")) == NULL)
    {
        /*
         * No list - nothing happened
         */
        SHR_EXIT();
    }

    if ((device->phydb_list = utilex_rhlist_create("phydb_list", sizeof(pp_stage_t), 1)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for db-list on:%s\n", soc_dev_name(unit));
    }
    /*
     * loop through entries
     */
    RHDATA_ITERATOR(curPhyDB, curSubTop, "phydb")
    {
        rhhandle_t rhhandle;
        char db_name[RHNAME_MAX_SIZE];
        /*
         * If there is no name - stage is not relevant for chip signal processing
         * Number of stages recorded is ones relevant
         */
        RHDATA_GET_STR_CONT(curPhyDB, "name", db_name);
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(device->phydb_list, db_name, RHID_TO_BE_GENERATED, &rhhandle));
    }

exit:
    SHR_FUNC_EXIT;
}

static pp_stage_t *
sand_signal_block_stage_find(
    pp_block_t *pp_block,
    char *name)
{
    int i_st;
    pp_stage_t *pp_stage = NULL;

    for (i_st = 0; i_st < pp_block->stage_num; i_st++)
    {
        pp_stage = &pp_block->stages[i_st];
        if(!sal_strcasecmp(pp_stage->name, name))
        {
            break;
        }
    }
    return pp_stage;
}

/*
 *
 */
static shr_error_e
sand_signal_get_stages(
        int unit,
        pp_block_t *pp_block,
        void *curBlock)
{
    void *curStage;
    int db_stage_num;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_NODE_NUM(db_stage_num, curBlock, "stage")
    if(db_stage_num == 0)
    {
        /*
         * No stages - nothing to we can do - continue to the next block
         */
        SHR_EXIT();
    }

    if((pp_block->stages = utilex_alloc(sizeof(pp_stage_t) * db_stage_num)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for stages\n");
    }
    /*
     * loop through entries
     */
    pp_block->stage_num = 0;
    RHDATA_ITERATOR(curStage, curBlock, "stage")
    {
        char tmp[RHNAME_MAX_SIZE];
        pp_stage_t *pp_stage = &pp_block->stages[pp_block->stage_num];
        /*
         * MS_ID is legitimate number, so we need to initialize it to some illegal one
         */
        pp_stage->ms_id = DSIG_ILLEGAL_MS_ID;
        /*
         * If there is no name - stage is not relevant for chip signal processing
         * Number of stages recorded is ones relevant
         */
        RHDATA_GET_STR_DEF_NULL(curStage, "name", pp_stage->name);
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit)) {
            if(!ISEMPTY(pp_stage->name) && dnx_pp_stage_string_to_id(unit, pp_stage->name, &pp_stage->pp_id) != 0)
            {
                LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Stage:%s is not registered in dnx data\n"), pp_stage->name));
            }
            RHDATA_GET_STR_DEF_NULL(curStage, "adapter", pp_stage->adapter_name);
#ifdef ADAPTER_SERVER_MODE
            /*
             * Skip stages marked as adapter="skip"
             */
            if(!sal_strcasecmp(pp_stage->adapter_name, "skip"))
            {
                continue;
            }
#endif
            if(ISEMPTY(pp_stage->name))
            {
#ifdef ADAPTER_SERVER_MODE
                if(!ISEMPTY(pp_stage->adapter_name))
                {
                    sal_strncpy(pp_stage->name, pp_stage->adapter_name, RHNAME_MAX_SIZE - 1);
                }
                else
#endif
                {  
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Stage has no name, ignore\n")));
                    continue;
                }
            }

        }
#endif
        RHDATA_GET_STR_DEF_NULL(curStage, "status", tmp);
        if(!ISEMPTY(tmp))
        {
            if(!sal_strcasecmp(tmp, "global") || !sal_strcasecmp(tmp, "register"))
            {
                pp_stage->status |= SIGNAL_STAGE_PORT_VIS_IGNORE;
            }
            else if(!sal_strcasecmp(tmp, "previous"))
            {
                pp_stage->status |= SIGNAL_STAGE_PREVIOUS;
            }
        }
        pp_block->stage_num++;
    }
    /*
     * Now match substituted stages
     */
    RHDATA_ITERATOR(curStage, curBlock, "stage")
    {
        char stage_n[RHNAME_MAX_SIZE];
        pp_stage_t *pp_stage, *is_subst_pp_stage;

        RHDATA_GET_STR_DEF_NULL(curStage, "substitute", stage_n);
        if(ISEMPTY(stage_n))
            continue;
            /*
             * Substitute exists means that this stage is not represented explicitly in ADAPTER,
             * but rather its signals are provided through substitute stage
             * So look for substitute and save the pointer to this peer inside
             * It should be always in already learned stages above
             */

        if((is_subst_pp_stage = sand_signal_block_stage_find(pp_block, stage_n)) == NULL)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Non existing substitute stage:%s\n"), stage_n));
            continue;
        }

        RHDATA_GET_STR_DEF_NULL(curStage, "name", stage_n);
        if((pp_stage = sand_signal_block_stage_find(pp_block, stage_n)) == NULL)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Stage to be substituted by:%s has no name\n"),
                                                                                    is_subst_pp_stage->name));
            continue;
        }
        /** Fill adapter name with regular PP name of substitute stage */
        sal_strncpy(pp_stage->adapter_name, pp_stage->name, RHNAME_MAX_SIZE - 1);

        is_subst_pp_stage->status |= SIGNAL_STAGE_IS_SUBSTITUTE;
        is_subst_pp_stage->peer_stage = pp_stage;
        pp_stage->status |= SIGNAL_STAGE_HAS_SUBSTITUTE;
        pp_stage->peer_stage = is_subst_pp_stage;
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
/*
* Get number of all signals mapped to virtual wires under the given block 'sdk_block_id'
*/
static shr_error_e
sand_signal_dynamic_nof(
    int unit,
    dnx_pp_block_e sdk_block_id,
    int* nof_signals)
{
    VirtualWiresContainer* virtual_wires;
    int vw_idx, stage_idx;
    int pp_stage, pemla_block, pema_id;
    uint32 start_stage, end_stage;

    SHR_FUNC_INIT_VARS(unit);

    *nof_signals = 0;

    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);

    /* calculate total nof signals to be allocated for the given 'sdk_block_id' */
    for(vw_idx = 0; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
    {
        start_stage = virtual_wires->vw_info_arr[vw_idx].start_stage;
        end_stage = virtual_wires->vw_info_arr[vw_idx].end_stage;
        for(stage_idx = start_stage; stage_idx <= end_stage; stage_idx++)
        {
            SHR_IF_ERR_EXIT(pemladrv_vw_stages_pemla_to_sdk(unit, stage_idx, &pp_stage));
            pema_id = dnx_data_pp.stages.params_get(unit, pp_stage)->pema_id;
            /* validate that 'pp_stage' is valid and has PEM access */
            if((pp_stage != DNX_PP_STAGE_INVALID) && (pema_id != -1))
            {
                SHR_IF_ERR_EXIT(dnx_pp_get_block_id(unit, pp_stage, &pemla_block));
                if(sdk_block_id == pemla_block)
                {
                    *nof_signals += 2;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* Get all signals mapped to virtual wires under the given block 'sdk_block_id'
*/
static shr_error_e
sand_signal_dynamic_init(
    int unit,
    dnx_pp_block_e sdk_block_id,
    int* nof_inited_signals,
    debug_signal_t *debug_signal)
{
    VirtualWiresContainer* virtual_wires;
    int vw_idx, stage_idx, vw_mappping_idx;
    int pp_stage, pemla_block, pema_id;
    uint32 start_stage, end_stage;

    SHR_FUNC_INIT_VARS(unit);

    *nof_inited_signals = 0;
    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);

    /* fill the related signals information */
    for(vw_idx = 0; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
    {
        start_stage = virtual_wires->vw_info_arr[vw_idx].start_stage;
        end_stage = virtual_wires->vw_info_arr[vw_idx].end_stage;
        for(stage_idx = start_stage; stage_idx <= end_stage; stage_idx++)
        {
            SHR_IF_ERR_EXIT(pemladrv_vw_stages_pemla_to_sdk(unit, stage_idx, &pp_stage));
            pema_id = dnx_data_pp.stages.params_get(unit, pp_stage)->pema_id;
            /* validate that 'pp_stage' is valid and has PEM access */
            if((pp_stage != DNX_PP_STAGE_INVALID) && (pema_id != -1))
            {
                SHR_IF_ERR_EXIT(dnx_pp_get_block_id(unit, pp_stage, &pemla_block));
                if(sdk_block_id == pemla_block)
                {
                    int nof_addrs = sizeof(debug_signal->address) / sizeof(signal_address_t);
                    int phy_wire_found = 0;

                    if(nof_addrs < virtual_wires->vw_info_arr[vw_idx].nof_mappings)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof signal address(%d) < PEMLA virtual wire mapping(%d)", 
                            nof_addrs, virtual_wires->vw_info_arr[vw_idx].nof_mappings);
                    }

                    /* PEM -> stage */
                    phy_wire_found = 0;
                    for(vw_mappping_idx = 0; vw_mappping_idx < virtual_wires->vw_info_arr[vw_idx].nof_mappings; vw_mappping_idx++)
                    {
                        debug_signal_t *signal;
                        int wire_size = virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].virtual_wire_msb - virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].virtual_wire_lsb + 1;
                        if(sand_signal_handle_get(unit, SIGNALS_MATCH_PEM, dnx_pp_block_name(unit, sdk_block_id), "PEM", dnx_pp_stage_name(unit, pp_stage),
                            virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].physical_wire_name, &signal) != _SHR_E_NONE)
                        {
                            break;
                        }
                        phy_wire_found = 1;
                        debug_signal->address[vw_mappping_idx].lsb = signal->address[0].lsb + virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].physical_wire_lsb;
                        debug_signal->address[vw_mappping_idx].msb = signal->address[0].lsb + virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].physical_wire_lsb + wire_size;
                        debug_signal->address[vw_mappping_idx].low = pema_id;
                    }
                    if(phy_wire_found)
                    {
                        sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE, "%s", virtual_wires->vw_info_arr[vw_idx].virtual_wire_name);
                        sal_snprintf(debug_signal->from, RHNAME_MAX_SIZE, "PEM");
                        sal_snprintf(debug_signal->to, RHNAME_MAX_SIZE, "%s", dnx_pp_stage_name(unit, pp_stage));
                        debug_signal->block_id = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE;
                        debug_signal->size = virtual_wires->vw_info_arr[vw_idx].width_in_bits;
                        debug_signal->range_num = virtual_wires->vw_info_arr[vw_idx].nof_mappings;
                        debug_signal++;
                        (*nof_inited_signals)++;
                    }

                    /* stage -> PEM */
                    phy_wire_found = 0;
                    for(vw_mappping_idx = 0; vw_mappping_idx < virtual_wires->vw_info_arr[vw_idx].nof_mappings; vw_mappping_idx++)
                    {
                        debug_signal_t *signal;
                        int wire_size = virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].virtual_wire_msb - virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].virtual_wire_lsb + 1;
                        if(sand_signal_handle_get(unit, SIGNALS_MATCH_PEM, dnx_pp_block_name(unit, sdk_block_id), dnx_pp_stage_name(unit, pp_stage), "PEM",
                                  virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].physical_wire_name, &signal) != _SHR_E_NONE)
                        {
                            break;
                        }
                        phy_wire_found = 1;
                        debug_signal->address[vw_mappping_idx].lsb = signal->address[0].lsb + virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].physical_wire_lsb;
                        debug_signal->address[vw_mappping_idx].msb = signal->address[0].lsb + virtual_wires->vw_info_arr[vw_idx].vw_mappings_arr[vw_mappping_idx].physical_wire_lsb + wire_size;
                        debug_signal->address[vw_mappping_idx].low = pema_id;
                    }
                    if(phy_wire_found)
                    {
                        sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE, "%s", virtual_wires->vw_info_arr[vw_idx].virtual_wire_name);
                        sal_snprintf(debug_signal->from, RHNAME_MAX_SIZE, "%s", dnx_pp_stage_name(unit, pp_stage));
                        sal_snprintf(debug_signal->to, RHNAME_MAX_SIZE, "PEM");
                        debug_signal->block_id = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE;
                        debug_signal->size = virtual_wires->vw_info_arr[vw_idx].width_in_bits;
                        debug_signal->range_num = virtual_wires->vw_info_arr[vw_idx].nof_mappings;
                        debug_signal++;
                        (*nof_inited_signals)++;
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
sand_signal_init(
    int        unit,
    device_t * device)
{
    void *curTop, *curSubTop, *curBlock;
    debug_signal_t *debug_signal;
    int i, i_block = 0;

    pp_block_t *cur_pp_block;

    SHR_FUNC_INIT_VARS(unit);

    if (device == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No device for:%s(%d)\n", soc_dev_name(unit), unit);
    }

    if (device->pp_blocks != NULL)
    { /* Already initialized, success */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(sand_signal_struct_init(unit, device));

    if ((curTop = dbx_file_get_xml_top(unit, "PP.xml", "top", CONF_OPEN_ALTER_LOC | CONF_OPEN_PER_DEVICE)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No PP Scheme found for:%s\n", soc_dev_name(unit));
    }

    SHR_IF_ERR_EXIT(sand_signal_phydb_init(unit, device, curTop));

    SHR_IF_ERR_EXIT(sand_signal_metadata_init(unit, device));

    if ((curSubTop = dbx_xml_child_get_first(curTop, "block-list")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No block-list in PP Scheme for:%s\n", soc_dev_name(unit));
    }

    RHDATA_GET_NODE_NUM(device->block_num, curSubTop, "block");
    if(device->block_num == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No blocks in PP Scheme for:%s\n", soc_dev_name(unit));
    }

    if((device->pp_blocks = utilex_alloc(sizeof(pp_block_t) * device->block_num)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for %s Blocks Data\n", soc_dev_name(unit));
    }
    /*
     * loop through entries
     */
    RHDATA_ITERATOR(curBlock, curSubTop, "block")
    {
        /*
         * Verify that we are inside the limitations MAX_NUM
         */
        cur_pp_block = &device->pp_blocks[i_block++];
        RHDATA_GET_STR_CONT(curBlock, "name", cur_pp_block->name);
        RHDATA_GET_STR_DEF_NULL(curBlock, "debug-signals", cur_pp_block->debug_signals_n);

        /*
         * Mechanism of building stage list is different for adapter and chip
         * In adapter we take the list from server, check presence of adapter name in PP.XML and assign stages to
         * block accordingly. Signals will be assigned per stage
         * In chip we just read the list, signals will be assigned per block
         */
        SHR_IF_ERR_EXIT(sand_signal_get_stages(unit, cur_pp_block, curBlock));
        {
            void *curSignalTop, *curSignal, *cur;
            int size =0;
            char full_address[DSIG_ADDRESS_MAX_SIZE];
            char temp[RHNAME_MAX_SIZE];
#ifdef BCM_DNX_SUPPORT
            int dynamic_signal_num = 0;
            int nof_inited_signals = 0;
#endif
            if(cur_pp_block->stage_num == 0)
                continue;
            sal_memset(full_address,0,sizeof(full_address));
            if (ISEMPTY(cur_pp_block->debug_signals_n))     /* No debug signals for this block */
                continue;

            if ((curSignalTop = dbx_file_get_xml_top(unit, cur_pp_block->debug_signals_n, "SignalInfo",
                                                                  CONF_OPEN_ALTER_LOC | CONF_OPEN_PER_DEVICE)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No Signal Info found in:%s\n", cur_pp_block->debug_signals_n);
            }
            /*
             * Obtain number of static signals
             */
            RHDATA_GET_NODE_NUM(cur_pp_block->block_signal_num, curSignalTop, "Signal");
            /*
             * Obtain number of dynamic signals
             */
            #ifdef BCM_DNX_SUPPORT
            if(SOC_IS_DNX(unit))
            {
                dnx_pp_block_e pp_block_id;
                SHR_IF_ERR_EXIT(dnx_pp_block_string_to_id(unit, cur_pp_block->name, &pp_block_id));
                SHR_IF_ERR_EXIT(sand_signal_dynamic_nof(unit, pp_block_id, &dynamic_signal_num));
                cur_pp_block->block_signal_num += dynamic_signal_num;
            }
            #endif
            /*
             * Check existence of debug signals for this block
             */
            if (cur_pp_block->block_signal_num == 0)
                continue;

            if((debug_signal = utilex_alloc(sizeof(debug_signal_t) * cur_pp_block->block_signal_num)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for debug signals\n");
            }

            cur_pp_block->block_debug_signals = debug_signal;

            /*
             * loop through entries
             */
            RHDATA_ITERATOR(curSignal, curSignalTop, "Signal")
            {
                dbx_xml_child_get_content_str(curSignal, "Name", debug_signal->hw, RHSTRING_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "From", debug_signal->from, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "To", debug_signal->to, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_int(curSignal, "Size", &debug_signal->size);
                dbx_xml_child_get_content_int(curSignal, "BlockID", &debug_signal->block_id);
                dbx_xml_child_get_content_str(curSignal, "Attribute", debug_signal->attribute, RHNAME_MAX_SIZE);
                if(debug_signal->size > DSIG_MAX_SIZE_BITS)
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Signal:%s:%s:%s size:%d is more than maximum:%d\n"),
                                                    cur_pp_block->name, debug_signal->from, debug_signal->attribute,
                                                    debug_signal->size, DSIG_MAX_SIZE_BITS));
                    continue;
                }
                debug_signal->description = sand_signal_metadata_description(device, debug_signal->attribute);
#ifdef ADAPTER_SERVER_MODE
                dbx_xml_child_get_content_str(curSignal, "Adapter", debug_signal->adapter_name, RHNAME_MAX_SIZE);
#endif
                debug_signal->stage_from = sand_signal_stage_get(unit, device, cur_pp_block, debug_signal->from);
                if(ISEMPTY(debug_signal->from))
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - 'From Stage' is empty\n"),
                                                                        cur_pp_block->name, debug_signal->attribute));
                }
                else if(debug_signal->stage_from == NULL)
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - From Stage:%s is not in PP Scheme\n"),
                                                      cur_pp_block->name, debug_signal->attribute, debug_signal->from));
                }
                debug_signal->stage_to = sand_signal_stage_get(unit, device, cur_pp_block, debug_signal->to);
                if(ISEMPTY(debug_signal->to))
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - 'To Stage' is empty\n"),
                                                                        cur_pp_block->name, debug_signal->attribute));
                }
                else if(debug_signal->stage_to == NULL)
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - To Stage:%s is not in PP Scheme\n"),
                                                      cur_pp_block->name, debug_signal->attribute, debug_signal->to));
                }

                dbx_xml_child_get_content_int(curSignal, "Perm", &debug_signal->perm);
                cur = dbx_xml_child_get_content_str(curSignal, "Condition", debug_signal->cond_attribute, RHNAME_MAX_SIZE);
                if ((cur != NULL) && !ISEMPTY(debug_signal->cond_attribute))
                {
                    RHDATA_GET_INT_DEF(cur, "Value", debug_signal->cond_value, 1);
                }

                dbx_xml_child_get_content_str(curSignal, "Expansion", debug_signal->expansion, RHNAME_MAX_SIZE);
                /*
                 * No explicit expansion - look for implicit one
                 */
                if (!ISEMPTY(debug_signal->expansion))
                {
                    if ((sand_signal_struct_get(device, debug_signal->expansion,
                                                    debug_signal->from, debug_signal->to, cur_pp_block->name)) == NULL)
                    {
                        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("No signal expansion:%s from:%s block:%s\n"),
                                                  debug_signal->expansion, debug_signal->from, cur_pp_block->name));
                        SET_EMPTY(debug_signal->expansion);
                    }
                }
                else
                {
                    if ((sand_signal_struct_get(device, debug_signal->attribute,
                                                     debug_signal->from, debug_signal->to, cur_pp_block->name)) != NULL)
                        /*
                         * If there is match - attribute serves as expansion
                         */
                        sal_strncpy(debug_signal->expansion, debug_signal->attribute, RHNAME_MAX_SIZE - 1);
                }

                dbx_xml_child_get_content_str(curSignal, "Resolution", debug_signal->resolution, RHNAME_MAX_SIZE);
                if (!ISEMPTY(debug_signal->resolution))
                {
                    if ((sand_signal_resolve_get(device, debug_signal->resolution,
                                                    debug_signal->from, debug_signal->to, cur_pp_block->name)) == NULL)
                    {
                        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Signal resolution:%s does not exist\n"),  debug_signal->resolution));
                        SET_EMPTY(debug_signal->resolution);
                    }
                }
                else
                {
                    if ((sand_signal_resolve_get(device, debug_signal->attribute,
                                                    debug_signal->from, debug_signal->to, cur_pp_block->name)) != NULL)
                        /*
                         * If there is match - attribute serves as resolution param name
                         */
                        sal_strncpy(debug_signal->resolution, debug_signal->attribute, RHNAME_MAX_SIZE - 1);
                }

                cur = dbx_xml_child_get_content_str(curSignal, "Double", temp, RHNAME_MAX_SIZE);
                if ((cur != NULL) && !ISEMPTY(temp))
                    debug_signal->double_flag = 1;

                dbx_xml_child_get_content_str(curSignal, "Address", full_address, DSIG_ADDRESS_MAX_SIZE);
                if(SOC_IS_DPP(unit))
                {
                    debug_signal->range_num = sand_signal_address_parse(full_address, debug_signal->address);
                    /*
                     * Verify consistency between size and sum of all bits in range
                     */
                    size = 0;
                    for (i = 0; i < debug_signal->range_num; i++)
                        size += (debug_signal->address[i].msb + 1 - debug_signal->address[i].lsb);
                }
                else if(SOC_IS_DNX(unit))
                {
                    /*
                     * address[0] will serve for DNX devices, low will represent bus
                     */
                    sscanf(full_address, "{bus:%d} bits: [%d : %d]", &debug_signal->address[0].low,
                                                                      &debug_signal->address[0].msb,
                                                                      &debug_signal->address[0].lsb);
                    size = debug_signal->address[0].msb + 1 - debug_signal->address[0].lsb;
                    debug_signal->range_num = 1;
                }
                if (size != debug_signal->size)
                {
                    cli_out("Correcting size for:%s from:%d to %d\n", debug_signal->attribute, debug_signal->size, size);
                    debug_signal->size = size;
                }

                debug_signal++;
            }

            dbx_xml_top_close(curSignalTop);
            /*
             * Init Dynamic Signals - append to the static ones
             */
            #ifdef BCM_DNX_SUPPORT
            if(SOC_IS_DNX(unit))
            {
                dnx_pp_block_e pp_block_id;
                SHR_IF_ERR_EXIT(dnx_pp_block_string_to_id(unit, cur_pp_block->name, &pp_block_id));
                SHR_IF_ERR_EXIT(sand_signal_dynamic_init(unit, pp_block_id, &nof_inited_signals, debug_signal));
                /* update the total number of configured signals */
                cur_pp_block->block_signal_num += nof_inited_signals - dynamic_signal_num;
            }
            #endif
        } /** End of static signals init per block */
    }
#ifdef ADAPTER_SERVER_MODE
    /*
     * Update adapter stage ms_id
     */
    SHR_IF_ERR_EXIT(sand_adapter_update_stages(unit, device));
#else
    /*
     * Now we need to extract condition attribute - not in ADAPTER - there are no signals yet
     * We can consider to fetch them in run-time
     */
    for (i = 0; i < device->block_num; i++)
    {
        int j;
        cur_pp_block = &device->pp_blocks[i];
        debug_signal = cur_pp_block->block_debug_signals;
        for (j = 0; j < cur_pp_block->block_signal_num; j++)
        {
            if (!ISEMPTY(debug_signal->cond_attribute))
            {
#ifdef BCM_DNX_SUPPORT
                /*
                 * debug signal for hit bit for certain accesses on certain stages is not updated, so do not
                 * assign cond_signal for such situation
                 */
                if (SOC_IS_DNX(unit) && dnx_data_debug.hw_bug.feature_get(unit, dnx_data_debug_hw_bug_no_hit_bit_on_mdb_access))
                {
                    if(sal_strcasestr(debug_signal->cond_attribute, "hit") != NULL)
                    {
                        if(!sal_strcasecmp(debug_signal->to, "VTT1") || !sal_strcasecmp(debug_signal->to, "VTT2") ||
                           !sal_strcasecmp(debug_signal->to, "VTT4") || !sal_strcasecmp(debug_signal->to, "FWD1"))
                        {
                            debug_signal++;
                            continue;
                        }
                    }
                }
#endif
                if(sand_signal_handle_get(unit, 0,cur_pp_block->name,
                                             debug_signal->from, debug_signal->to,
                                             debug_signal->cond_attribute, &debug_signal->cond_signal) != _SHR_E_NONE)
                {
                    cli_out("Condition Attribute:%s does not exist for:%s:%s -> %s\n",
                            debug_signal->cond_attribute, cur_pp_block->name, debug_signal->from, debug_signal->to);
                }
            }
            debug_signal++;
        }
    }
#endif

    dbx_xml_top_close(curTop);
exit:
    SHR_FUNC_EXIT;
}

void
sand_signal_deinit(
    int        unit,
    device_t * device)
{
    int i_ind, i_st;

    if (device == NULL)
    {
        goto exit;
    }

    if (device->pp_blocks == NULL)
    { /* Not initialized, leave peacefully */
        goto exit;
    }

    sand_signal_struct_deinit(device);

    for(i_ind = 0; i_ind < device->block_num; i_ind++)
    {
        if(device->pp_blocks != NULL)
        {
            for(i_st = 0; i_st < device->pp_blocks[i_ind].stage_num; i_st++)
            {
                if(device->pp_blocks[i_ind].stages != NULL)
                {
                    utilex_free(device->pp_blocks[i_ind].stages[i_st].stage_debug_signals);
                }
            }
            utilex_free(device->pp_blocks[i_ind].stages);
            utilex_free(device->pp_blocks[i_ind].block_debug_signals);
        }
    }
    utilex_free(device->pp_blocks);
    device->block_num = 0;

    if(device->phydb_list != NULL)
    {
        utilex_rhlist_free_all(device->phydb_list);
    }
    device->phydb_list = NULL;

    if(device->metadata_list != NULL)
    {
        utilex_rhlist_free_all(device->metadata_list);
    }
    device->metadata_list = NULL;

    device->pp_blocks = NULL;

exit:
    return;
}

/*
 * }
 */

shr_error_e
sand_signal_reread(
    int unit)
{
    device_t *device;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "");
    }
    /* DEInitialize Signals Data */
    sand_signal_deinit(unit, device);

    /* Initialize Access Objects */
    SHR_IF_ERR_EXIT(sand_signal_init(unit, device));

exit:
    SHR_FUNC_EXIT;
}

