/** \file dnxc_data_dyn_loader.c
 *
 * DEVICE DATA Dynamic loader -
 *
 * Device Data dynamic (after init) loading
 * For additional details about Device Data Dynamic Loader Component goto 'dnxc_data_dyn_loader.h'
 */

/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/pbmp.h>

#include <sal/appl/sal.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/dnxc/dnxc_data/dnxc_data_dyn_loader.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#if defined(BCM_DNX_SUPPORT)
#include <include/soc/dnx/utils/bare_metal_dynamic_loading.h>
#endif
#include <bcm/types.h>
/*
 * }
 */

/*
 * Function declarations:
 * {
 */

/**
 * brief - Return dynamic data file path for SKU revision (device ID + revision ID)
 */
static shr_error_e dnxc_data_dyn_loader_sku_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);

/**
 * brief - Return dynamic data file path for SKU
 */
static shr_error_e dnxc_data_dyn_loader_sku_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);
/*
 * }
 */

/*
 * DEFINES:
 * {
 */

/**
 * brief - Dynamic DNX data XML files path relative to the db directory
 */
#define DNXC_DATA_DYN_XML_DB_PATH "data/"

/**
 * brief - Array that holds dynamic data file pointers for DNX (path from db directory).
 */
#if defined(BCM_DNX_SUPPORT)
static dnxc_data_dyn_file_path_get_f dnx_data_dyn_files[] = {
    dnxc_data_dyn_loader_sku_file_path_get,
    dnxc_data_dyn_loader_sku_revision_file_path_get,
    /*
     * below are used for bare-metal mode 
     */
    dnx_arr_prefix_dynamic_file_get,
    dnx_aod_tables_dynamic_file_get,
    dnx_mdb_app_db_dynamic_file_get,
    dnx_pemla_soc_dynamic_file_get,
    dnx_aod_sizes_dynamic_file_get
};
#endif

/**
 * brief - Array that holds dynamic data file pointers for DNXF (path from db directory).
 */
#if defined(BCM_DNXF_SUPPORT)
static dnxc_data_dyn_file_path_get_f dnxf_data_dyn_files[] = {
    dnxc_data_dyn_loader_sku_file_path_get,
    dnxc_data_dyn_loader_sku_revision_file_path_get
};
#endif

/**
 * \brief - String element in a linked list.
 *          Used to support unknown number of dynamically loaded strings
 */
typedef struct dnxc_data_dyn_str_linked_list_s dnxc_data_dyn_str_linked_list_s;
typedef struct dnxc_data_dyn_str_linked_list_s
{
    dnxc_data_dyn_str_linked_list_s *next;
    char str[RHSTRING_MAX_SIZE];
} dnxc_data_dyn_str_linked_list_t;

/**
 * brief - Hold the handles to allocated strings to be freed in deinit,
 *         DNX DATA might hold pointers to strings loaded from the file that should be freed after DNXC DATA deinit.
 */
static dnxc_data_dyn_str_linked_list_t *dnxc_data_dyn_str_handles_head[SOC_MAX_NUM_DEVICES] = { NULL };
static dnxc_data_dyn_str_linked_list_t *dnxc_data_dyn_str_handles_tail[SOC_MAX_NUM_DEVICES] = { NULL };

typedef struct
{
    /*
     * Entry used to be queued on info_list
     */
    rhentry_t entry;

    /*
     * pointer to dynamic data file XML top
     */
    xml_node file_top;
} dnxc_data_dyn_file_top_info_t;

/**
 * \brief - list of XML file tops
 */
rhlist_t *dnxc_data_dyn_files_top_list[SOC_MAX_NUM_DEVICES] = { NULL };

/**
 * \brief
 *  Supported dynamic DNX data table data types
 */
typedef enum
{
  /**
   * Type - bcm_pbmp_t
   */
    dnxc_data_dyn_table_data_type_pbmp = 0,

  /**
   * Type - int
   */
    dnxc_data_dyn_table_data_type_int = 1,

  /**
   * Type - uint32
   */
    dnxc_data_dyn_table_data_type_uint32 = 2,

    /**
     * Type - int array
     */
    dnxc_data_dyn_table_data_type_int_array = 3,

    /**
     * Type - uint32 array
     */
    dnxc_data_dyn_table_data_type_uint32_array = 4,

    /**
     * Type - uint8
     */
    dnxc_data_dyn_table_data_type_uint8 = 5,
    /**
     * Type - string
     */
    dnxc_data_dyn_table_data_type_string = 6
} dnxc_data_dyn_table_data_type;

/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */

/**
 * \brief - Parse soc property attributes. For more information \see dnxc_data_property_t.
 */
static shr_error_e
dxc_data_dyn_loader_property_parse(
    int unit,
    void *node,
    dnxc_data_property_t * property_info)
{
    int map_id, range_min, range_max, map_value;
    void *sub_node;
    char map_name[RHSTRING_MAX_SIZE];
    char method[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /** In case method is provided, verify that the method is the same as the original method */
    RHDATA_GET_STR_DEF_NULL(node, "method", method);
    if (!sal_strcasecmp(method, ""))
    {
        if (sal_strncmp(method, property_info->method_str, RHSTRING_MAX_SIZE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - new method=%s is different than old method=%s\n",
                         method, property_info->method_str);
        }
    }

    /** handle range */
    if (property_info->method == dnxc_data_property_method_port_range_signed ||
        property_info->method == dnxc_data_property_method_port_range ||
        property_info->method == dnxc_data_property_method_range ||
        property_info->method == dnxc_data_property_method_suffix_range)
    {
        RHDATA_GET_INT_STOP(node, "range_min", range_min);
        RHDATA_GET_INT_STOP(node, "range_max", range_max);

        property_info->range_min = range_min;
        property_info->range_max = range_max;
    }

    /** handle direct mapping, some of the mappings might be disabled */
    if (property_info->method == dnxc_data_property_method_suffix_direct_map ||
        property_info->method == dnxc_data_property_method_port_direct_map ||
        property_info->method == dnxc_data_property_method_direct_map)
    {

        /** Init existing mappings to invalid */
        for (map_id = 0; map_id < property_info->nof_mapping; map_id++)
        {
            (property_info->mapping[map_id]).is_invalid = TRUE;
        }

        /** iterate over the mappings: parse its attributes and set valid indication for supported mappings */
        RHDATA_ITERATOR(sub_node, node, "map")
        {
            RHDATA_GET_STR_STOP(sub_node, "name", map_name);

            /** look for the mapping in the original data */
            for (map_id = 0; map_id < property_info->nof_mapping; map_id++)
            {
                if (!sal_strncmp(map_name, (property_info->mapping[map_id]).name, RHSTRING_MAX_SIZE))
                {
                    /** mapping found */
                    (property_info->mapping[map_id]).is_invalid = FALSE;

                    /** read value, use original value by default */
                    RHDATA_GET_INT_BREAK(sub_node, "value", map_value);

                    (property_info->mapping[map_id]).val = map_value;

                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert data type name (string) to enum. \see
 */
static shr_error_e
dnxc_data_dyn_table_data_type_name_to_enum_convert(
    int unit,
    char *type_name,
    dnxc_data_dyn_table_data_type * data_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /** handle bitmap type */
    if (!sal_strncasecmp(type_name, "bcm_pbmp_t", RHSTRING_MAX_SIZE))
    {
        *data_type = dnxc_data_dyn_table_data_type_pbmp;
    }
    /** handle int type */
    else if (!sal_strncasecmp(type_name, "int", RHSTRING_MAX_SIZE))
    {
        *data_type = dnxc_data_dyn_table_data_type_int;
    }
    /** handle uint32 type */
    else if (!sal_strncasecmp(type_name, "uint32", RHSTRING_MAX_SIZE))
    {
        *data_type = dnxc_data_dyn_table_data_type_uint32;
    }
    /** handle int array type */
    else if (!sal_strncasecmp(type_name, "int[", 7))
    {
        *data_type = dnxc_data_dyn_table_data_type_int_array;
    }
    /** handle uint32 array type */
    else if (!sal_strncasecmp(type_name, "uint32[", 7))
    {
        *data_type = dnxc_data_dyn_table_data_type_uint32_array;
    }
    /** handle uint8 type */
    else if (!sal_strncasecmp(type_name, "uint8", RHSTRING_MAX_SIZE))
    {
        *data_type = dnxc_data_dyn_table_data_type_uint8;
    }
    /** handle strings */
    else if (!sal_strncasecmp(type_name, "char *", RHSTRING_MAX_SIZE))
    {
        *data_type = dnxc_data_dyn_table_data_type_string;
    }
    /** unsupported table data type */
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Dynamic DNX DATA - dynamic tables doesn't support the member type (%s)\n",
                     type_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert DNX data bitmap string to bcm_pbmp_t type
 */
static shr_error_e
dxc_data_dyn_loader_string_to_bitmap_convert(
    int unit,
    char *pbmp_str,
    bcm_pbmp_t * pbmp)
{
    char **datatokens;
    uint32 nof_tokens;
    int word_index, word_int;
    char *str_pointer_end;

    SHR_FUNC_INIT_VARS(unit);

    datatokens = NULL;

    /** remove white spaces */
    utilex_str_white_spaces_remove(pbmp_str);

    /** split the provided string into words of hexa */
    datatokens = utilex_str_split(pbmp_str, ",", _SHR_PBMP_WORD_MAX, &nof_tokens);
    SHR_NULL_CHECK(datatokens, _SHR_E_FAIL, "Failed to split bitmap string into tokens");

    for (word_index = 0; word_index < nof_tokens; word_index++)
    {
        word_int = sal_ctoi(datatokens[word_index], &str_pointer_end);
        if (datatokens[word_index] == str_pointer_end)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Provided string (%s) is not a valid bitmap format", pbmp_str);
        }

        /** construct bitmap word by word */
        _SHR_PBMP_WORD_SET(*pbmp, word_index, word_int);
    }

exit:

    /** deallocate datatokens */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert DNX data array string to array
 */
static shr_error_e
dxc_data_dyn_loader_string_to_array_convert(
    int unit,
    char *array_str,
    uint32 *array)
{
    char **datatokens;
    uint32 nof_tokens;
    int word_index;
    uint32 word_int;
    char *str_pointer_end;

    SHR_FUNC_INIT_VARS(unit);

    datatokens = NULL;

    /** remove white spaces */
    utilex_str_white_spaces_remove(array_str);

    /** split the provided string into words of hexa */
    datatokens = utilex_str_split(array_str, ",", _SHR_PBMP_WORD_MAX, &nof_tokens);
    SHR_NULL_CHECK(datatokens, _SHR_E_FAIL, "Failed to split bitmap string into tokens");

    for (word_index = 0; word_index < nof_tokens; word_index++)
    {
        word_int = sal_ctoi(datatokens[word_index], &str_pointer_end);
        if (datatokens[word_index] == str_pointer_end)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Provided string (%s) is not a valid array format", array_str);
        }

        /** construct array word by word */
        array[word_index] = word_int;
    }

exit:

    /** deallocate datatokens */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Return array length given DNX data array string
 */
static shr_error_e
dxc_data_dyn_loader_string_to_array_length_get(
    int unit,
    char *array_str,
    int *length)
{
    char **datatokens;
    uint32 nof_tokens;

    SHR_FUNC_INIT_VARS(unit);

    datatokens = NULL;

    /** remove white spaces */
    utilex_str_white_spaces_remove(array_str);

    /** split the provided string into words of hexa */
    datatokens = utilex_str_split(array_str, ",", _SHR_PBMP_WORD_MAX, &nof_tokens);
    SHR_NULL_CHECK(datatokens, _SHR_E_FAIL, "Failed to split bitmap string into tokens");

    *length = nof_tokens;

exit:

    /** deallocate datatokens */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Modify property info if exists
 */
static shr_error_e
dnxc_data_dyn_loader_table_properties_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    int value_index;
    void *value_node, *property_node;
    char value_name[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    if (table_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table data wasn't found \n");
    }

    /** iterate over the table members */
    RHDATA_ITERATOR(value_node, table_info->dynamic_data_node, "value")
    {
        /** check if property exists */
        property_node = dbx_xml_child_get_first(value_node, "property");
        if (property_node != NULL)
        {
            /** get member name */
            RHDATA_GET_STR_STOP(value_node, "name", value_name);

            /** find the member index */
            for (value_index = 0; value_index < table_info->nof_values; value_index++)
            {
                if (!sal_strncasecmp(table_info->values[value_index].name, value_name, RHSTRING_MAX_SIZE))
                {
                    /** member found - break the loop */
                    break;
                }
            }

            /** if member wasn't found, return an error */
            if (value_index == table_info->nof_values)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) has no member (%s) \n",
                             table_info->name, value_name);
            }

            /** override property struct */
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_property_parse(unit, property_node,
                                                               &(table_info->values[value_index].property)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set all table entries to default values
 */
static shr_error_e
dnxc_data_dyn_loader_table_defaults_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    int key1_size, key2_size, key1, key2, value_index, index;
    void *value_node;
    char default_value_str[RHSTRING_MAX_SIZE], value_name[RHSTRING_MAX_SIZE];
    char *str_pointer_end;
    uint32 *data_value_ptr;
    dnxc_data_dyn_table_data_type data_type;
    int val_int = 0;
    bcm_pbmp_t val_pbmp;
    char *val_str = NULL;
    uint32 *val_array = NULL;
    int length = 0;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(val_pbmp);

    /** iterate over the table members */
    RHDATA_ITERATOR(value_node, table_info->dynamic_data_node, "value")
    {
        /** get member name */
        RHDATA_GET_STR_STOP(value_node, "name", value_name);

        /** get default value */
        RHDATA_GET_LSTR_STOP(value_node, "default", default_value_str);

        /** find the member index */
        for (value_index = 0; value_index < table_info->nof_values; value_index++)
        {
            if (!sal_strncasecmp(table_info->values[value_index].name, value_name, RHSTRING_MAX_SIZE))
            {
                /** member found - break the loop */
                break;
            }
        }

        /** if member wasn't found, return an error */
        if (value_index == table_info->nof_values)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) has no member (%s) \n", table_info->name,
                         value_name);
        }

        /** convert type name from string to enum */
        SHR_IF_ERR_EXIT(dnxc_data_dyn_table_data_type_name_to_enum_convert(unit,
                                                                           (table_info->values[value_index].type),
                                                                           &data_type));

        /** handle bitmap type */
        if (data_type == dnxc_data_dyn_table_data_type_pbmp)
        {
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_bitmap_convert(unit, default_value_str, &val_pbmp));
        }
        /** handle int or uint32 type */
        else if ((data_type == dnxc_data_dyn_table_data_type_int) ||
                 (data_type == dnxc_data_dyn_table_data_type_uint32))
        {
            val_int = sal_ctoi(default_value_str, &str_pointer_end);
            if (default_value_str == str_pointer_end)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                             table_info->name, value_name);
            }
        }
        /** handle uint8 type */
        else if (data_type == dnxc_data_dyn_table_data_type_uint8)
        {
            val_int = sal_ctoi(default_value_str, &str_pointer_end);
            if (default_value_str == str_pointer_end)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                             table_info->name, value_name);
            }
        }
        /** handle int or uint32 array type */
        else if ((data_type == dnxc_data_dyn_table_data_type_int_array) ||
                 (data_type == dnxc_data_dyn_table_data_type_uint32_array))
        {
            /** get default array length */
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_length_get(unit, default_value_str, &length));

            /** alloc */
            SHR_ALLOC_SET_ZERO(val_array, length * sizeof(uint32), "data array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            /** covert */
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_convert(unit, default_value_str, val_array));
        }
        else if (data_type == dnxc_data_dyn_table_data_type_string)
        {
            dnxc_data_dyn_str_linked_list_t *element = NULL;
            /*
             * allocate linked list element which will hold the string
             * The string stored in a linked list so we can free it upon deinit
             */
            SHR_ALLOC_SET_ZERO(element, sizeof(dnxc_data_dyn_str_linked_list_t), "dnxc_data_dyn_str",
                               "dnxc_data_dyn_str_handle for unit %d %s%s\r\n", unit, EMPTY, EMPTY);
            sal_snprintf(element->str, sizeof(element->str) - 1, "%s", default_value_str);
            if (dnxc_data_dyn_str_handles_head[unit] == NULL)
            {
                dnxc_data_dyn_str_handles_head[unit] = element;
                dnxc_data_dyn_str_handles_tail[unit] = element;
            }
            else
            {
                dnxc_data_dyn_str_handles_tail[unit]->next = element;
                dnxc_data_dyn_str_handles_tail[unit] = element;
            }

            val_str = element->str;
        }
        /** unsupported table data type */
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) member (%s) has an "
                         "unsupported data type (%s). Dynamic data supports only 'int, uint32 and bcm_pbmp_t' \n",
                         table_info->name, value_name, table_info->values[value_index].name);
        }

        /** iterate over all table entries (possible key combinations) and set the member to default value */
        key1_size = (table_info->keys[0]).size;
        key2_size = (table_info->keys[1]).size;

        /** if no key exists, iterate 1 time (set size to 1)*/
        if (key1_size == 0)
        {
            key1_size = 1;
        }
        if (key2_size == 0)
        {
            key2_size = 1;
        }

        for (key1 = 0; key1 < key1_size; key1++)
        {
            for (key2 = 0; key2 < key2_size; key2++)
            {
                /** get pointer to data offset */
                data_value_ptr = dnxc_data_utils_table_member_data_pointer_get(unit, table_info, value_name,
                                                                               key1, key2);
                if (data_value_ptr == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find data pointer for member (%s) in table (%s)",
                                 value_name, table_info->name);
                }

                /** handle bitmap type */
                if (data_type == dnxc_data_dyn_table_data_type_pbmp)
                {
                    _SHR_PBMP_ASSIGN(*(bcm_pbmp_t *) data_value_ptr, val_pbmp);
                }
                /** handle int or uint32 type */
                else if ((data_type == dnxc_data_dyn_table_data_type_int) ||
                         (data_type == dnxc_data_dyn_table_data_type_uint32))
                {
                    *(int *) data_value_ptr = val_int;
                }
                /** handle uint8 type */
                else if (data_type == dnxc_data_dyn_table_data_type_uint8)
                {
                    *(uint8 *) data_value_ptr = (uint8) val_int;
                }
                /** handle int or uint32 array type */
                else if ((data_type == dnxc_data_dyn_table_data_type_int_array) ||
                         (data_type == dnxc_data_dyn_table_data_type_uint32_array))
                {
                    /** set new defaults to array */
                    if (val_array != NULL)
                    {
                        for (index = 0; index < length; index++)
                        {
                            ((uint32 *) data_value_ptr)[index] = val_array[index];
                        }

                        SHR_FREE(val_array);
                    }
                }
                /** handle string type */
                else if (data_type == dnxc_data_dyn_table_data_type_string)
                {
                    *(char **) data_value_ptr = val_str;
                }
            }
        }
    }

exit:
    SHR_FREE(val_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set given table entries
 */
static shr_error_e
dnxc_data_dyn_loader_table_entries_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    int key1, key2, value_index, index;
    void *entries_node, *entry_node;
    char value_str[RHSTRING_MAX_SIZE];
    char *str_pointer_end;
    uint32 *data_value_ptr;
    dnxc_data_dyn_table_data_type data_type;
    int val_int = 0;
    bcm_pbmp_t val_pbmp;
    uint32 *val_array = NULL;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(val_pbmp);

    /** verify */
    if (table_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table data wasn't found \n");
    }

    /** check if entries exist */
    entries_node = dbx_xml_child_get_first(table_info->dynamic_data_node, "entries");
    if (entries_node != NULL)
    {
        /** parse entry by entry */
        RHDATA_ITERATOR(entry_node, entries_node, "entry")
        {
            /** construct keys */
            {
                key1 = 0;
                key2 = 0;

                /** find key1 and key2 */
                if (table_info->nof_keys > 0)
                {
                    /** get key1 value */
                    RHDATA_GET_INT_STOP(entry_node, (table_info->keys[0].name), key1);
                }
                if (table_info->nof_keys > 1)
                {
                    /** get key2 value */
                    RHDATA_GET_INT_STOP(entry_node, (table_info->keys[1].name), key2);
                }
                if (table_info->nof_keys > 2)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) has more than 2 keys = %d", table_info->name,
                                 table_info->nof_keys);
                }
            }

            /** Iterate over all values, if value exists modify it */
            for (value_index = 0; value_index < table_info->nof_values; value_index++)
            {
                /** get value */
                RHDATA_GET_LSTR_DEF(entry_node, (table_info->values[value_index].name), value_str, "");

                /** Check if value exists */
                if (sal_strncmp(value_str, "", RHSTRING_MAX_SIZE))
                {
                    /** convert type name from string to enum */
                    SHR_IF_ERR_EXIT(dnxc_data_dyn_table_data_type_name_to_enum_convert(unit,
                                                                                       (table_info->
                                                                                        values[value_index].type),
                                                                                       &data_type));

                    /** get pointer to data offset */
                    data_value_ptr = dnxc_data_utils_table_member_data_pointer_get(unit, table_info,
                                                                                   (table_info->
                                                                                    values[value_index].name), key1,
                                                                                   key2);
                    if (data_value_ptr == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find data pointer for member (%s) in table (%s)",
                                     (table_info->values[value_index].name), table_info->name);
                    }

                    /** handle bitmap type */
                    if (data_type == dnxc_data_dyn_table_data_type_pbmp)
                    {
                        SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_bitmap_convert(unit, value_str, &val_pbmp));

                        /** set bitmap */
                        _SHR_PBMP_ASSIGN(*(bcm_pbmp_t *) data_value_ptr, val_pbmp);
                    }
                    /** handle int or uint32 type */
                    else if ((data_type == dnxc_data_dyn_table_data_type_int) ||
                             (data_type == dnxc_data_dyn_table_data_type_uint32))
                    {
                        val_int = sal_ctoi(value_str, &str_pointer_end);
                        if (value_str == str_pointer_end)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                                         table_info->name, table_info->values[value_index].name);
                        }

                        /** set value */
                        *(int *) data_value_ptr = val_int;
                    }
                    /** handle uint8 type */
                    else if (data_type == dnxc_data_dyn_table_data_type_uint8)
                    {
                        val_int = sal_ctoi(value_str, &str_pointer_end);
                        if (value_str == str_pointer_end)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                                         table_info->name, table_info->values[value_index].name);
                        }

                        /** set value */
                        *(uint8 *) data_value_ptr = (uint8) val_int;
                    }
                    /** handle int or uint32 array type */
                    else if ((data_type == dnxc_data_dyn_table_data_type_int_array) ||
                             (data_type == dnxc_data_dyn_table_data_type_uint32_array))
                    {
                        int length;

                        /** get default array length */
                        SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_length_get(unit, value_str, &length));

                        SHR_ALLOC_SET_ZERO(val_array, length * sizeof(uint32),
                                           "data array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

                        SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_convert(unit, value_str, val_array));

                        /** set the new array values */
                        for (index = 0; index < length; index++)
                        {
                            ((uint32 *) data_value_ptr)[index] = val_array[index];
                        }

                        SHR_FREE(val_array);
                    }
                    /** handle strings */
                    else if (data_type == dnxc_data_dyn_table_data_type_string)
                    {
                        dnxc_data_dyn_str_linked_list_t *element = NULL;
                        /*
                         * allocate linked list element which will hold the string
                         * The string stored in a linked list so we can free it upon deinit
                         */
                        SHR_ALLOC_SET_ZERO(element, sizeof(dnxc_data_dyn_str_linked_list_t), "dnxc_data_dyn_str",
                                           "dnxc_data_dyn_str_handle for unit %d %s%s\r\n", unit, EMPTY, EMPTY);
                        sal_snprintf(element->str, sizeof(element->str) - 1, "%s", value_str);
                        if (dnxc_data_dyn_str_handles_head[unit] == NULL)
                        {
                            dnxc_data_dyn_str_handles_head[unit] = element;
                            dnxc_data_dyn_str_handles_tail[unit] = element;
                        }
                        else
                        {
                            dnxc_data_dyn_str_handles_tail[unit]->next = element;
                            dnxc_data_dyn_str_handles_tail[unit] = element;
                        }

                        *(char **) data_value_ptr = element->str;
                    }
                }
            }
        }
    }

exit:
    SHR_FREE(val_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Parse tables attributes (name, value, property, method, etc...) of a given module and submodule
 */
static shr_error_e
dnxc_data_dyn_loader_tables_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char table_name[RHSTRING_MAX_SIZE];
    xml_node table_node;
    dnxc_data_table_t *table_info;

    SHR_FUNC_INIT_VARS(unit);

    /** iterate over the tables and parse its attributes */
    RHDATA_ITERATOR(table_node, node, "table")
    {
        /** parse table name */
        RHDATA_GET_STR_STOP(table_node, "name", table_name);

        /** Get pointer to the table structure */
        table_info = dnxc_data_utils_table_info_get(unit, module_name, submodule_name, table_name);

        if (table_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) data wasn't found \n", table_name);
        }

        table_info->dynamic_data_node = table_node;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_table_value_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Override table values if dynamic data exists */
    if (table_info->dynamic_data_node != NULL)
    {
        /** step 1 - set table members to default values */
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_defaults_set(unit, table_info));

        /** step 2 - iterate over given entries and set the provided values */
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_entries_set(unit, table_info));

        /** step 3 - read and set soc property info if exists */
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_properties_set(unit, table_info));

        /** mark label as dynamic */
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, table_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Parse numerics attributes (name, value, property, method, etc...) of a given module and submodule
 */
static shr_error_e
dnxc_data_dyn_loader_numerics_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char numeric_name[RHSTRING_MAX_SIZE];
    dnxc_data_define_t *numeric_info;
    xml_node sub_node;

    SHR_FUNC_INIT_VARS(unit);

    /** iterate over the features and parse its attributes */
    RHDATA_ITERATOR(sub_node, node, "numeric")
    {
        RHDATA_GET_STR_STOP(sub_node, "name", numeric_name);

        /** Get pointer to the parsed numeric structure */
        numeric_info = dnxc_data_utils_define_info_get(unit, module_name, submodule_name, numeric_name);
        if (numeric_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - numeric (%s) data wasn't found \n", numeric_name);
        }

        numeric_info->dynamic_data_node = sub_node;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_numeric_value_set(
    int unit,
    dnxc_data_define_t * numeric_info)
{
    int numeric_default_value;
    xml_node property_node;

    SHR_FUNC_INIT_VARS(unit);

    /** Override define value if dynamic data exists */
    if (numeric_info->dynamic_data_node != NULL)
    {
        /** parse numeric new value */
        RHDATA_GET_INT_STOP(numeric_info->dynamic_data_node, "value", numeric_default_value);

        /** set the new value */
        numeric_info->data = numeric_default_value;

        /** get property info in case it exists */
        property_node = dbx_xml_child_get_first(numeric_info->dynamic_data_node, "property");
        if (property_node != NULL)
        {
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_property_parse(unit, property_node, &(numeric_info->property)));
        }

        /** mark label as dynamic */
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, numeric_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Parse features attributes (name and value) of a given module and submodule
 */
static shr_error_e
dnxc_data_dyn_loader_features_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char feature_name[RHSTRING_MAX_SIZE];
    dnxc_data_feature_t *feature_info;
    xml_node sub_node;

    SHR_FUNC_INIT_VARS(unit);

    /** iterate over the features and parse its attributes */
    RHDATA_ITERATOR(sub_node, node, "feature")
    {
        RHDATA_GET_STR_STOP(sub_node, "name", feature_name);

        /** Get pointer to the parsed feature structure */
        feature_info = dnxc_data_utils_feature_info_get(unit, module_name, submodule_name, feature_name);
        if (feature_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - feature (%s) data wasn't found \n", feature_name);
        }

        feature_info->dynamic_data_node = sub_node;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_feature_value_set(
    int unit,
    dnxc_data_feature_t * feature_info)
{
    int feature_new_value;

    SHR_FUNC_INIT_VARS(unit);

    /** Override feature value if dynamic data exists */
    if (feature_info->dynamic_data_node != NULL)
    {
        /** parse feature new value */
        RHDATA_GET_INT_DEF(feature_info->dynamic_data_node, "value", feature_new_value, 1);

        /** verify that provided value is either '0' or '1' */
        if ((feature_new_value != 0) && (feature_new_value != 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - feature (%s) value (%d) must be either '0' or '1'\n",
                         feature_info->name, feature_new_value);
        }

        /** set the new value */
        feature_info->data = feature_new_value;

        /** mark label as dynamic */
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, feature_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parsed define attributes
 */
static shr_error_e
dnxc_data_dyn_loader_define_value_set_verify(
    int unit,
    dnxc_data_define_t * define_info,
    int define_new_value)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify the type of the data is actually a define (not numeric) */
    if (_SHR_IS_FLAG_SET(define_info->flags, DNXC_DATA_F_DEFINE) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - failed parsing define."
                     " data (%s) type is not of type 'define'\n", define_info->name);
    }

    /*
     * Verify that the provided value is not bigger than the original value to keep consistency of the
     * define's original maximal value.
     *
     * Note - this limitation can be later removed if such a requirement will be needed.
     */
    if (define_new_value > define_info->data)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - define data (%s) new value (%d) is bigger than the original"
                     " value(%d)\n", define_info->name, define_new_value, define_info->data);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Parse defines attributes (name and value) of a given module and submodule
 */
static shr_error_e
dnxc_data_dyn_loader_defines_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char define_name[RHSTRING_MAX_SIZE];
    dnxc_data_define_t *define_info;
    xml_node sub_node;

    SHR_FUNC_INIT_VARS(unit);

    /** iterate over the defines and parse its attributes */
    RHDATA_ITERATOR(sub_node, node, "define")
    {
        /** parse the define value and name */
        RHDATA_GET_STR_STOP(sub_node, "name", define_name);

        /** Get pointer to the parsed defined structure */
        define_info = dnxc_data_utils_define_info_get(unit, module_name, submodule_name, define_name);
        if (define_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - define data wasn't found %s\n", define_name);
        }

        define_info->dynamic_data_node = sub_node;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_define_value_set(
    int unit,
    dnxc_data_define_t * define_info)
{
    int define_new_value;

    SHR_FUNC_INIT_VARS(unit);

    /** numerics are handled in a dedicated function */
    if (define_info->flags & DNXC_DATA_F_NUMERIC)
    {
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_numeric_value_set(unit, define_info));
        SHR_EXIT();
    }

    /** Override define value if dynamic data exists */
    if (define_info->dynamic_data_node != NULL)
    {
        /** parse define new value */
        RHDATA_GET_INT_STOP(define_info->dynamic_data_node, "value", define_new_value);

        /** verify define attributes */
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set_verify(unit, define_info, define_new_value));

        /** set the new value */
        define_info->data = define_new_value;

        /** mark label as dynamic */
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, define_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_dev_id_verify(
    int unit)
{

    uint16 dev_id;
    uint8 rev_id;
    uint32 data_dev_id;
    dnxc_data_define_t *numeric_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get chip device ID */
    SHR_IF_ERR_EXIT(soc_cm_get_id(unit, &dev_id, &rev_id));

    /** get device ID from DNXC data */
    numeric_info = dnxc_data_utils_define_info_get(unit, "device", "general", "device_id");
    SHR_NULL_CHECK(numeric_info, _SHR_E_FAIL, "numeric_info");

    data_dev_id = numeric_info->data;
    /** compare the two device IDs, return error if they don't match */
    if (dev_id != data_dev_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - Incorrect DNX Data XML was provided. Current device ID is %x, "
                     "but the provided data belongs to device ID %x \n", dev_id, data_dev_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - parse provided module
 */
static shr_error_e
dnxc_data_dyn_loader_module_parse(
    int unit,
    xml_node module,
    char module_name[RHNAME_MAX_SIZE])
{
    xml_node cur_sub_module, node;
    char sub_module_name[RHFILE_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /** iterate over sub modules */
    RHDATA_ITERATOR(cur_sub_module, module, "sub_module")
    {
        /** get sub module name */
        RHDATA_GET_STR_STOP(cur_sub_module, "name", sub_module_name);

        /** parse defines */
        node = dbx_xml_child_get_first(cur_sub_module, "defines");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_defines_parse(unit, node, module_name, sub_module_name));
        }

        /** parse features */
        node = dbx_xml_child_get_first(cur_sub_module, "features");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_features_parse(unit, node, module_name, sub_module_name));
        }

        /** parse numerics */
        node = dbx_xml_child_get_first(cur_sub_module, "numerics");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_numerics_parse(unit, node, module_name, sub_module_name));
        }

        /** parse tables */
        node = dbx_xml_child_get_first(cur_sub_module, "tables");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_tables_parse(unit, node, module_name, sub_module_name));
        }

    } /** Iterate over sub modules */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - parse provided file
 */
static shr_error_e
dnxc_data_dyn_loader_file_parse(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    xml_node cur_top = NULL, cur_module;
    char module_name[RHNAME_MAX_SIZE];
    rhhandle_t temp = NULL;
    dnxc_data_dyn_file_top_info_t *dnxc_data_dyn_file_top_info;

    SHR_FUNC_INIT_VARS(unit);

    /** do nothing if no path is provided */
    if (ISEMPTY(file_path))
    {
        SHR_EXIT();
    }

    /** get pointer to top of the XML file */
    cur_top = dbx_file_get_xml_top(unit, file_path, "device_data", CONF_OPEN_NO_ERROR_REPORT | CONF_OPEN_PER_DEVICE);

    /** If dynamic data XML found - read the data inside of it, otherwise do nothing. */
    if (cur_top == NULL)
    {
        SHR_EXIT();
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "Dynamic Data XML found: %s\n%s%s%s", file_path, EMPTY, EMPTY, EMPTY);

    /** add file top pointer to the list of file top pointers */
    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(dnxc_data_dyn_files_top_list[unit], "", RHID_TO_BE_GENERATED, &temp));
    dnxc_data_dyn_file_top_info = (dnxc_data_dyn_file_top_info_t *) temp;
    dnxc_data_dyn_file_top_info->file_top = cur_top;

    /** check for single module name at the same level as 'device_data' */
    RHDATA_GET_STR_DEF_NULL(cur_top, "module", module_name);
    if (sal_strcasecmp(module_name, ""))
    {
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_module_parse(unit, cur_top, module_name));
    }
    else
    {
        /** Multi module file - Iterate over modules */
        RHDATA_ITERATOR(cur_module, cur_top, "module")
        {
            /** get module name */
            RHDATA_GET_STR_STOP(cur_module, "name", module_name);
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_module_parse(unit, cur_module, module_name));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_parse_init(
    int unit)
{
    char file_path[RHFILE_MAX_SIZE];
    int file_i, nof_files = 0;
    dnxc_data_dyn_file_path_get_f *file_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * create a list that will hold the files top pointers - this list is maintained in order to free the file tops
     * when they are no longer needed
     */
    if ((dnxc_data_dyn_files_top_list[unit] =
         utilex_rhlist_create("dnxc_data_dyn_files_top_list", sizeof(dnxc_data_dyn_file_top_info_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to create dnxc_data_dyn_files_top_list\n");
    }

    /** get file list and its size */
#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX(unit))
    {
        nof_files = COUNTOF(dnx_data_dyn_files);
        file_list = dnx_data_dyn_files;
    }
#endif

#if defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_DNXF(unit))
    {
        nof_files = COUNTOF(dnxf_data_dyn_files);
        file_list = dnxf_data_dyn_files;
    }
#endif

    /** iterate over all dynamic data files and parse them */
    for (file_i = 0; file_i < nof_files; file_i++)
    {
        sal_memset(file_path, 0, sizeof(file_path));

        if (file_list != NULL)
        {
            /** get file path */
            SHR_IF_ERR_EXIT(file_list[file_i] (unit, file_path));

            /** parse the provided file */
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_file_parse(unit, file_path));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_dyn_loader_parse_deinit(
    int unit)
{
    dnxc_data_dyn_str_linked_list_t *cur_element, *next_element;
    rhhandle_t file_top_element;
    dnxc_data_dyn_file_top_info_t *dnxc_data_dyn_file_top_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * free all allocated strings 
     */
    cur_element = dnxc_data_dyn_str_handles_head[unit];
    while (cur_element != NULL)
    {
        next_element = cur_element->next;
        SHR_FREE(cur_element);
        cur_element = next_element;
    }

    /*
     * set linked list head and tail back to NULL 
     */
    dnxc_data_dyn_str_handles_head[unit] = NULL;
    dnxc_data_dyn_str_handles_tail[unit] = NULL;

    /** iterate over the list of file tops and close the files */
    if (dnxc_data_dyn_files_top_list[unit] != NULL)
    {
        file_top_element = utilex_rhlist_entry_get_first(dnxc_data_dyn_files_top_list[unit]);
        while (file_top_element != NULL)
        {
            dnxc_data_dyn_file_top_info = (dnxc_data_dyn_file_top_info_t *) file_top_element;

            /** close the file */
            if (dnxc_data_dyn_file_top_info->file_top != NULL)
            {
                dbx_xml_top_close(dnxc_data_dyn_file_top_info->file_top);
            }

            /** move to next element */
            file_top_element = utilex_rhlist_entry_get_next(file_top_element);
        }
    }

    /** free files top list */
    utilex_rhlist_free_all(dnxc_data_dyn_files_top_list[unit]);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_data_dyn_loader_sku_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE];
    uint16 dev_id;
    uint8 rev_id;
    char chip_name[RHFILE_MAX_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /** construct relative path to file from the db directory */

    /** Start from the DNX data dynamic directory - 'data' */
    sal_strncpy(file_path, DNXC_DATA_DYN_XML_DB_PATH, RHFILE_MAX_SIZE);

    /** construct file name using the device ID*/
    /** get chip device ID */
    SHR_IF_ERR_EXIT(soc_cm_get_id(unit, &dev_id, &rev_id));
    /** convert it to string */
    sal_snprintf(chip_name, RHFILE_MAX_SIZE, "BCM8%hx", dev_id);
    sal_strncpy(file_name, chip_name, RHFILE_MAX_SIZE - 12);

    /** change upper case letter do lower case */
    utilex_str_to_lower(file_name);

    /** add suffix '_data' to chip name */
    sal_strncat(file_name, "_data.xml", RHFILE_MAX_SIZE - 1);

    /** add file name to the relative path */
    sal_strncat(file_path, file_name, RHFILE_MAX_SIZE - 1);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - construct a file path relative to db directory of the current chip SKU revision (device ID + revision ID)
 */
static shr_error_e
dnxc_data_dyn_loader_sku_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE];
    const char *chip_name;

    /** construct relative path to file from the db directory */

    /** Start from the DNX data dynamic directory - 'data' */
    sal_strncpy(file_path, DNXC_DATA_DYN_XML_DB_PATH, RHFILE_MAX_SIZE);

    /** construct file name using the device ID and revision */
    chip_name = soc_dev_name(unit);
    sal_strncpy(file_name, chip_name, RHFILE_MAX_SIZE - 10);

    /** change upper case letter do lower case */
    utilex_str_to_lower(file_name);

    /** add suffix '_data' to chip name */
    sal_strncat(file_name, "_data.xml", RHFILE_MAX_SIZE - 1);

    /** allow to explicitly load file for regression*/

    /** add file name to the relative path */
    sal_strncat(file_path, file_name, RHFILE_MAX_SIZE - 1);

    return _SHR_E_NONE;
}

/*
 * }
 */
