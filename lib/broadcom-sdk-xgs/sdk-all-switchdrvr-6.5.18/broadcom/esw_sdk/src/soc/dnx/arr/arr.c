/** \file arr.c
 * $Id$
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/sand/sand_signals.h>

#include <soc/dnx/arr/arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

/*
 * Defines and macros
 * {
 */
/** \brief EES/ETPS data buffer type field size */
#  define ARR_EG_TYPE_FIELD_SIZE_BITS               (dnx_data_pp.ETPP.etps_type_size_bits_get(unit))

/** \brief EES/ETPS data buffer type field offset */
#  define ARR_EG_TYPE_FIELD_OFFSET_MSB              0
/*
 * }
 * Defines and macros
 */

/*
 * Utilities
 * {
 */
/**
 * \brief
 * path to tables xml validation file(s) (relative to DB path
 * folder)
 */
#  define ARR_FILE_PATH          "arr/arr_logical_formats"

/**
 * \brief
 * ARR field parse
 */
static shr_error_e
arr_field_parse(
    int unit,
    void *node,
    arr_field_info_t * field_info)
{
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_STOP(node, "Name", field_info->name);
    RHDATA_GET_INT_STOP(node, "Size", field_info->size);
    RHDATA_GET_INT_STOP(node, "Offset", field_info->offset);
    RHDATA_GET_INT_STOP(node, "Prefix", field_info->prefix);
    RHDATA_GET_INT_STOP(node, "Prefix_Value", field_info->prefix_value);
    RHDATA_GET_INT_STOP(node, "Prefix_Size", field_info->prefix_size);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * ARR format parse, moving all unused fields to the end of the format, order the fields in the format by the offset.
 */
static shr_error_e
arr_format_parse(
    int unit,
    void *node,
    arr_format_info_t * arr_format,
    int sort_fields)
{
    int field_counter = 0;
    void *curSub;
    arr_format_info_t local_arr_format;
    int ii, jj;
    int last_pos;
    int next_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_STOP(node, "Name", arr_format->name);
    RHDATA_GET_INT_STOP(node, "ArrIndex", arr_format->arr_index);

    RHDATA_ITERATOR(curSub, node, "Field")
    {
        if (field_counter >= ARR_MAX_NUMBER_OF_FIELDS_IN_FORMATS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "nember of fields ecceded limit \n");
        }
        SHR_IF_ERR_EXIT(arr_field_parse(unit, curSub, &local_arr_format.arr_field[field_counter]));
        field_counter++;
    }

    arr_format->nof_fields = field_counter;

    /** fields with size "0" will be added to end */

    last_pos = field_counter - 1;
    for (ii = 0; ii < field_counter; ii++)
    {
        if (local_arr_format.arr_field[ii].size == 0)
        {
            sal_strcpy(arr_format->arr_field[last_pos].name, local_arr_format.arr_field[ii].name);
            arr_format->arr_field[last_pos].size = local_arr_format.arr_field[ii].size;
            arr_format->arr_field[last_pos].offset = local_arr_format.arr_field[ii].offset;
            arr_format->arr_field[last_pos].prefix = local_arr_format.arr_field[ii].prefix;
            arr_format->arr_field[last_pos].prefix_value = local_arr_format.arr_field[ii].prefix_value;
            arr_format->arr_field[last_pos].prefix_size = local_arr_format.arr_field[ii].prefix_size;
            last_pos--;
        }
    }

    arr_format->nof_fields = last_pos + 1;
    arr_format->arr_nof_fields = field_counter;

    /** sort the fields in the format if requested */
    if (sort_fields == 1)
    {
        /** sorting the rest of the fields */
        for (ii = 0; ii < arr_format->nof_fields; ii++)
        {
            for (jj = 0; jj < field_counter; jj++)
            {
                if ((local_arr_format.arr_field[jj].size != 0)
                    && (next_offset == local_arr_format.arr_field[jj].offset))
                {
                    sal_strcpy(arr_format->arr_field[ii].name, local_arr_format.arr_field[jj].name);
                    arr_format->arr_field[ii].size = local_arr_format.arr_field[jj].size;
                    arr_format->arr_field[ii].offset = local_arr_format.arr_field[jj].offset;
                    arr_format->arr_field[ii].prefix = local_arr_format.arr_field[jj].prefix;
                    arr_format->arr_field[ii].prefix_value = local_arr_format.arr_field[jj].prefix_value;
                    arr_format->arr_field[ii].prefix_size = local_arr_format.arr_field[jj].prefix_size;
                    next_offset += local_arr_format.arr_field[jj].size;
                    break;
                }
            }
        }
    }
    else
    {
        for (ii = 0; ii < arr_format->nof_fields; ii++)
        {
            sal_strcpy(arr_format->arr_field[ii].name, local_arr_format.arr_field[ii].name);
            arr_format->arr_field[ii].size = local_arr_format.arr_field[ii].size;
            arr_format->arr_field[ii].offset = local_arr_format.arr_field[ii].offset;
            arr_format->arr_field[ii].prefix = local_arr_format.arr_field[ii].prefix;
            arr_format->arr_field[ii].prefix_value = local_arr_format.arr_field[ii].prefix_value;
            arr_format->arr_field[ii].prefix_size = local_arr_format.arr_field[ii].prefix_size;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * ARR set parse
 */
static shr_error_e
arr_set_parse(
    int unit,
    void *node,
    arr_set_info_t * arr_set,
    int sort_fields)
{
    int format_counter = 0;
    void *curSub;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_STOP(node, "Name", arr_set->name);
    RHDATA_GET_INT_STOP(node, "Instances", arr_set->nof_instances);
    RHDATA_GET_INT_STOP(node, "Size", arr_set->size);

    RHDATA_ITERATOR(curSub, node, "Format")
    {
        if (format_counter >= ARR_MAX_NUMBER_OF_FORMATS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "nember of fields ecceded limit \n");
        }
        SHR_IF_ERR_EXIT(arr_format_parse(unit, curSub, &arr_set->arr_format[format_counter], sort_fields));

        if (arr_set->max_nof_fields < arr_set->arr_format[format_counter].arr_nof_fields)
        {
            arr_set->max_nof_fields = arr_set->arr_format[format_counter].arr_nof_fields;
        }
        format_counter++;
    }

    arr_set->nof_formats = format_counter;
exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief ARR value extraction logic
 */
static shr_error_e
dnx_arr_decode_field_value_get(
    int unit,
    uint32 *data,
    uint32 data_size,
    int field_offset_msb,
    int field_size,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Value
     */
    if (field_offset_msb + field_size < data_size)
    {
        SHR_BITCOPY_RANGE(val, 0, data, data_size - field_offset_msb - field_size, field_size);
    }
    else
    {
        SHR_BITCOPY_RANGE(val, 0, data, 0, field_size);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief Check if the given descriptors list contain the set name
 */
static shr_error_e
dnx_arr_decode_set_desc_analyze(
    int unit,
    char **desc_lst,
    uint32 desc_nof,
    const char *set_name,
    uint8 *used_set)
{
    uint32 desc_i;

    SHR_FUNC_INIT_VARS(unit);

    *used_set = FALSE;
    for (desc_i = 0; desc_i < desc_nof; desc_i++)
    {
        if (!sal_strncasecmp(set_name, desc_lst[desc_i], ARR_MAX_STRING_LENGTH))
        {
            *used_set = TRUE;
            break;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief Copy a single field details to the field signal object
 */
static shr_error_e
dnx_arr_decode_field_parse(
    int unit,
    uint32 *data,
    uint32 data_size,
    arr_field_info_t * field,
    signal_output_t * field_sig)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Populate signal_output_t fields:
     * value
     * print_value
     * size
     */
    SHR_IF_ERR_EXIT(dnx_arr_decode_field_value_get(unit, data, data_size, field->offset, field->size,
                                                   field_sig->value));
    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, field_sig->value, field_sig->print_value,
                             field->size, PRINT_BIG_ENDIAN);
    field_sig->size = field->size;

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief Copies a signal_output_t structure's relevant fields to a newly allocated entry in 'list'
 */
static shr_error_e
dnx_arr_decode_field_signal_copy(
    int unit,
    signal_output_t * sig,
    const char *sig_status,
    rhlist_t ** list)
{
    rhhandle_t rhhandle;
    signal_output_t *new_sig;
    char full_name[RHNAME_MAX_SIZE];
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Generate name with prefix
     */
    sal_snprintf(full_name, RHNAME_MAX_SIZE, "%s", RHNAME(sig));

    if (NULL == *list)
    {
        if ((*list = utilex_rhlist_create("field information", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
        }
    }

    /*
     * Allocate a new entry in list
     */
    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(*list, full_name, RHID_TO_BE_GENERATED, &rhhandle));
    new_sig = rhhandle;
    /*
     * Copy relevant fields (name has already been copied)
     */
    new_sig->size = sig->size;
    SHR_BITCOPY_RANGE(new_sig->value, 0, sig->value, 0, sig->size);
    sal_snprintf(new_sig->print_value, DSIG_MAX_SIZE_STR - 1, "%s", sig->print_value);
    sal_strncpy(new_sig->status, sig_status, RHNAME_MAX_SIZE - 1);
    new_sig->status[RHNAME_MAX_SIZE - 1] = '\0';

    /*
     * Copy sub field if present (used for expected_prefix)
     */
    if (sig->field_list)
    {
        signal_output_t *new_sub_sig = utilex_rhlist_entry_get_last(sig->field_list);
        SHR_IF_ERR_EXIT(dnx_arr_decode_field_signal_copy(unit, new_sub_sig, "", &new_sig->field_list));
    }
    sal_snprintf(full_name, RHNAME_MAX_SIZE, "%s", RHNAME(sig));

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief add all the fields from an ARR format to the field list of signal sig.
 */
static shr_error_e
dnx_arr_decode_fields_add(
    int unit,
    arr_format_info_t * fmt,
    uint32 *data,
    uint32 size,
    rhlist_t * field_list)
{
    int field_i;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Look for missing fields and add them.
     */
    for (field_i = 0; field_i < fmt->nof_fields; field_i++)
    {
        arr_field_info_t *field = &(fmt->arr_field[field_i]);
        if (NULL == utilex_rhlist_entry_get_by_name(field_list, field->name))
        {
            rhhandle_t fld_handle;
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(field_list, field->name, RHID_TO_BE_GENERATED, &fld_handle));
            SHR_IF_ERR_EXIT(dnx_arr_decode_field_parse(unit, data, size, field, fld_handle));

            if (field->prefix_value)
            {
                rhhandle_t fld_handle1;
                signal_output_t *field_sig = fld_handle;

                if (NULL == field_sig->field_list)
                {
                    if ((field_sig->field_list =
                         utilex_rhlist_create("field information", sizeof(signal_output_t), 0)) == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "Could not allocate parsed information list to parse the required buffer.\n");
                    }
                }

                SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                                (field_sig->field_list, "expected_prefix", RHID_TO_BE_GENERATED, &fld_handle1));

                field_sig = fld_handle1;
                sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, (uint32 *) &field->prefix_value,
                                         field_sig->print_value, field->prefix_size, PRINT_BIG_ENDIAN);
                field_sig->size = field->prefix_size;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * Utilities
 */

/*
 * Internal APIs
 * {
 */
/**
 * \see
 *   arr.h
 */
shr_error_e
dnx_arr_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE])
{
    int set_i, fmt_i;
    uint32 data_type = 0;
    signal_output_t *sig_format = NULL;
    signal_output_t *sig = NULL;
    char **desc_lst = NULL;
    uint32 desc_nof = 0;
    root_arr_info_t *arr_info = 0;
    uint8 type_only = FALSE;

    rhlist_t *used_fields = NULL;
    rhlist_t *other_fields = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Input validation
     */
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_NULL_CHECK(parsed_info, _SHR_E_PARAM, "parsed_info");
    SHR_RANGE_VERIFY(data_size_bits, ARR_EG_TYPE_FIELD_SIZE_BITS, DSIG_FIELD_MAX_SIZE_BITES, _SHR_E_PARAM,
                     "Data size must be at least %d bits long to parse the data type, and at most %d bits long to fit"
                     " in the output structure.", ARR_EG_TYPE_FIELD_SIZE_BITS, DSIG_FIELD_MAX_SIZE_BITES);
    /*
     * Extract data type from data;
     */
    SHR_IF_ERR_EXIT(dnx_arr_decode_data_type_get(unit, data, data_size_bits, &data_type));
    /*
     * There is a "NULL" ETPS format defined for data_type==0, but it will just clutter the
     * output since all the unused entries will have it.
     * So for now, skipping those entries.
     */
    if (0 == data_type)
    {
        SHR_EXIT();
    }
    /*
     * Initialize arr_info
     */
    arr_info = sal_alloc(sizeof(root_arr_info_t), "root_arr_info");
    SHR_NULL_CHECK(arr_info, _SHR_E_MEMORY, "arr_info");
    SHR_IF_ERR_EXIT(arr_parse_file(unit, arr_info));
    /*
     * Make sure parsed_info is allocated.
     */
    if (NULL == *parsed_info)
    {
        if ((*parsed_info = utilex_rhlist_create("Arr information", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
        }
    }
    /*
     * Allocate temporary lists for found fields.
     */
    if ((used_fields = utilex_rhlist_create("Used Fields", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
    }
    if ((other_fields = utilex_rhlist_create("Other Fields", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
    }
    /*
     * descriptors may be combined in a comma separated list (desc1,desc2,desc3...).
     * If they are supplied at all, create a convenient list to go over them.
     */
    if (!ISEMPTY(desc))
    {
        desc_lst = utilex_str_split(desc, ",", ARR_MAX_NUMBER_OF_FORMATS, &desc_nof);
        /*
         * Fields are only added if the desc is not TYPE_ONLY. This is a keyword that signifies that
         * only the type of the entry is required.
         */
        type_only = (sal_strncasecmp(desc, "TYPE_ONLY", RHSTRING_MAX_SIZE) == 0);
    }
    /*
     * Find all the ARR formats with the relevant ArrIndex and add their fields.
     */
    for (set_i = 0; set_i < arr_info->nof_egress_arr_sets; set_i++)
    {
        uint8 used_set;
        rhlist_t *current_list = other_fields;
        arr_set_info_t *set = &(arr_info->egress_arr[set_i]);
        /*
         * Check if the set is compatible with the given 'desc' and
         */
        SHR_IF_ERR_EXIT(dnx_arr_decode_set_desc_analyze(unit, desc_lst, desc_nof, set->name, &used_set));
        if (used_set)
        {
            current_list = used_fields;
        }
        /*
         * Go over all the formats in this set to find relevant formats and add their fields.
         */
        for (fmt_i = 0; fmt_i < set->nof_formats; fmt_i++)
        {
            arr_format_info_t *fmt = &(set->arr_format[fmt_i]);
            if (fmt->arr_index == data_type)
            {
                /*
                 * This ARR format addresses the required type.
                 */
                if (NULL == sig_format)
                {
                    /*
                     * Create an entry (signal) in the parsed data list for the data type.
                     * This field gets a special treatment: no name prefix and an indication that it is the data type
                     */
                    rhhandle_t sig_handle;
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(used_fields, "ARR_TYPE", RHID_TO_BE_GENERATED,
                                                                 &sig_handle));
                    sig_format = sig_handle;
                    sig_format->size = ARR_EG_TYPE_FIELD_SIZE_BITS;
                    sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s(0x%02x)", fmt->name, data_type);
                }
                if (!type_only)
                {
                    SHR_IF_ERR_EXIT(dnx_arr_decode_fields_add(unit, fmt, data, data_size_bits, current_list));
                }
            }
        }
    }
    /*
     * At this point the relevant fields are in 2 lists: used_fields and other_fields.
     * The field may appear in both lists so need to be sure to add only once.
     */
    RHITERATOR(sig, used_fields)
    {
        /*
         * These fields can be added without fear of duplicates
         */
        SHR_IF_ERR_EXIT(dnx_arr_decode_field_signal_copy(unit, sig, "used", parsed_info));
    }
    RHITERATOR(sig, other_fields)
    {
        /*
         * These fields may already been added, so make sure they are not found in used_fields before adding them.
         */
        if (NULL == utilex_rhlist_entry_get_by_name(used_fields, RHNAME(sig)))
        {
            /*
             * Safe to add
             */
            SHR_IF_ERR_EXIT(dnx_arr_decode_field_signal_copy(unit, sig, "", parsed_info));
        }
    }

exit:
    if (arr_info)
    {
        sal_free(arr_info);
    }
    if (desc_lst)
    {
        utilex_str_split_free(desc_lst, desc_nof);
    }
    if (used_fields)
    {
        sand_signal_list_free(used_fields);
    }
    if (other_fields)
    {
        sand_signal_list_free(other_fields);
    }
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   arr.h
 */
shr_error_e
dnx_arr_decode_data_type_get(
    int unit,
    uint32 *data,
    uint32 size,
    uint32 *data_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_NULL_CHECK(data_type, _SHR_E_PARAM, "data_type");
    SHR_RANGE_VERIFY(size, ARR_EG_TYPE_FIELD_SIZE_BITS, DSIG_FIELD_MAX_SIZE_BITES, _SHR_E_PARAM,
                     "Data size must be at least %d bits long to parse the data type, and at most %d bits long to fit"
                     " in the output structure.", ARR_EG_TYPE_FIELD_SIZE_BITS, DSIG_FIELD_MAX_SIZE_BITES);
    *data_type = 0;
    SHR_IF_ERR_EXIT(dnx_arr_decode_field_value_get(unit, data, size, ARR_EG_TYPE_FIELD_OFFSET_MSB,
                                                   ARR_EG_TYPE_FIELD_SIZE_BITS, data_type));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * parse the XML file of the ARR info. store it in a global parameter.
 */
shr_error_e
arr_parse_file(
    int unit,
    root_arr_info_t * arr_info)
{
    void *curTop, *node, *curSub;
    int set_counter = 0;
    const dnx_data_arr_formats_files_t *table;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init the arr_info buffer
     */
    curTop = NULL;
    sal_memset(arr_info, 0, sizeof(root_arr_info_t));

    table = dnx_data_arr.formats.files_get(unit);

    curTop = dbx_file_get_xml_top(unit, table->arr_formats_file, "Root", CONF_OPEN_PER_DEVICE);
    if (curTop == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find arr xml file in: %s or cannot find tag in file\n",
                     table->arr_formats_file);
    }

    /** ingress ARR  */
    node = dbx_xml_child_get_first(curTop, "Ingress_arr");
    if (node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find Ingress_arr in file \n");
    }

    RHDATA_ITERATOR(curSub, node, "Arr")
    {
        SHR_IF_ERR_EXIT(arr_set_parse(unit, curSub, &arr_info->ingress_arr[set_counter], 1));
        set_counter++;
    }
    arr_info->nof_ingress_arr_sets = set_counter;

    /** egress ARR  */

    set_counter = 0;
    node = dbx_xml_child_get_first(curTop, "Egress_arr");
    if (node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find Ingress_arr in file \n");
    }

    RHDATA_ITERATOR(curSub, node, "Arr")
    {
        SHR_IF_ERR_EXIT(arr_set_parse(unit, curSub, &arr_info->egress_arr[set_counter], 0));
        set_counter++;
    }
    arr_info->nof_egress_arr_sets = set_counter;

    arr_info->is_initilized = 1;

exit:
    if (curTop != NULL)
    {
        dbx_xml_top_close(curTop);
    }
    SHR_FUNC_EXIT;
}

/*
 * }
 * Internal APIs
 */
