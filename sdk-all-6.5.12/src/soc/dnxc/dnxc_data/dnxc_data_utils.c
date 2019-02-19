/** \file dnxc_data_utils.c
 * 
 * DEVICE DATA MGMT - 
 * 
 * Device Data
 * SW component that maintains per device data
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
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <sal/appl/sal.h>

/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */

shr_error_e
dnxc_data_utils_dump_verify(
    int unit,
    uint32 state_flags,
    uint32 data_flags,
    char **data_labels,
    uint32 dump_flags,
    char *dump_label,
    int *dump)
{
    uint32 data_types;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Assume dump 
     */
    *dump = 1;

    /*
     * Check if data is supported by device 
     */
    if ((data_flags & DNXC_DATA_F_SUPPORTED) == 0)
    {
        *dump = 0;
    }

    /*
     * Check if to filter unchanged 
     */
    if ((dump_flags & DNXC_DATA_F_CHANGED) && ((data_flags & DNXC_DATA_F_CHANGED) == 0))
    {
        *dump = 0;
    }

    /*
     * Check if to filter non-property 
     */
    if ((dump_flags & DNXC_DATA_F_PROPERTY) && ((data_flags & DNXC_DATA_F_PROPERTY) == 0))
    {
        *dump = 0;
    }

    /*
     * Check if to filter according to data type (define, feature, numeric and table) 
     */
    data_types = dump_flags & DNXC_DATA_FLAG_DATA_TYPES_MASK;
    if (data_types)
    {
        if ((data_flags & data_types) == 0)
        {
            *dump = 0;
        }
    }

    /*
     * check if filter according to labels
     */
    if (dump_label != NULL && (*dump_label) != 0)
    {
        int label_index, label_exist = 0;
        for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
        {
            if (data_labels[label_index] != NULL
                && !sal_strncmp(dump_label, data_labels[label_index], sal_strlen(data_labels[label_index])))
            {
                label_exist = 1;
                break;
            }
        }
        if (label_exist == 0)
        {
            *dump = 0;
        }
    }

    /*
     * exit:
     */
    SHR_FUNC_EXIT;
}

const uint32 *
dnxc_data_utils_generic_feature_data_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name)
{
    int feature_index;
    dnxc_data_feature_t *features;
    int nof_features;

    /*
     * Get feature array
     */
    features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].features;
    nof_features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_features;

    /*
     * Iterate over features 
     */
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {
        if (!sal_strncasecmp(data_name, features[feature_index].name, strlen(data_name)))
        {
            /*
             * If feature found - return pointer to feature
             */
            return (uint32 *) &features[feature_index].data;
        }
    }

    /*
     * If not found return null
     */
    return NULL;
}

const uint32 *
dnxc_data_utils_generic_define_data_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name)
{
    int define_index;
    dnxc_data_define_t *defines;
    int nof_defines;
    shr_error_e rv;

    /*
     * Get defines array
     */
    defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].defines;
    nof_defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_defines;

    /*
     * Iterate over defines 
     */
    for (define_index = 0; define_index < nof_defines; define_index++)
    {
        if (!sal_strncasecmp(data_name, defines[define_index].name, strlen(data_name)))
        {
            /*
             * If define found - make sure define is supported 
             */
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                              defines[define_index].set);
            if (rv != _SHR_E_NONE)
            {
                /*
                 * in case access denied 
                 */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"),
                           defines[define_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            /*
             * If supported - return pointer to data
             */
            return &defines[define_index].data;
        }
    }

    return NULL;
}

const uint32 *
dnxc_data_utils_generic_table_data_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data,
    char *member,
    int key1,
    int key2)
{
    int table_index, value_index;
    dnxc_data_table_t *tables;
    int nof_tables;
    char *table_data;

    /*
     * 1. Get list of tables
     * 2. Look for the requested table
     * 3. Look for the requested member (value) in table
     * 4. return pointer to data
     */
    tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].tables;
    nof_tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_tables;

    /*
     * Iterate over tables 
     */
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(data, tables[table_index].name, strlen(data)))
        {
            /*
             * If table found - make sure table is supported by device
             */
            table_data = dnxc_data_mgmt_table_data_get(unit, &tables[table_index], key1, key2);
            if (table_data == NULL)
            {
                /*
                 * table is not supported by the device - return null
                 */
                return NULL;
            }

            /*
             * iterate over values 
             */
            for (value_index = 0; value_index < tables[table_index].nof_values; value_index++)
            {
                if (!sal_strncasecmp(member, tables[table_index].values[value_index].name, strlen(member)))
                {
                    /*
                     * If value found - return pointer to data
                     */
                    return (const uint32 *) (table_data + tables[table_index].values[value_index].offset);
                }
            }

            return NULL;
        }
    }

    return NULL;
}

const uint32 *
dnxc_data_utils_generic_module_2d_data_get(
    int unit,
    int module_index,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2)
{
    int submodule_index;
    const uint32 *res;

    /*
     * Get list of submodules
     */
    dnxc_data_module_t *module = &_dnxc_data[unit].modules[module_index];
    dnxc_data_submodule_t *submodules = module->submodules;

    /*
     * Iterate over submodules 
     */
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        if (!sal_strncasecmp(submodule, submodules[submodule_index].name, strlen(submodule)))
        {

            /*
             * look for feature with that name 
             */
            res = dnxc_data_utils_generic_feature_data_get(unit, module_index, submodule_index, data);
            if (res != NULL)
            {
                return res;
            }

            /*
             * look for define with that name 
             */
            res = dnxc_data_utils_generic_define_data_get(unit, module_index, submodule_index, data);
            if (res != NULL)
            {
                return res;
            }

            /*
             * look for define with that name 
             */
            res = dnxc_data_utils_generic_table_data_get(unit, module_index, submodule_index, data, member, key1, key2);
            if (res != NULL)
            {
                return res;
            }

            /*
             * required data wasn't found  (data and member) throw an error 
             */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "DNX DATA - data wasn't found  %s, %s\n"), data, member != NULL ? member : ""));
            return NULL;
        }
    }

    /*
     * required data wasn't found (submodule) throwan error 
     */
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - submodule wasn't found  %s\n"), submodule));
    return NULL;
}

const uint32 *
dnxc_data_utils_generic_2d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2)
{
    int module_index;

    /*
     * get modules array
     */
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;

    /*
     * Iterate over modules 
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        if (!sal_strncasecmp(module, modules[module_index].name, strlen(module)))
        {
            /*
             * If module found - look for requested data in that module
             */
            return dnxc_data_utils_generic_module_2d_data_get(unit, module_index, submodule, data, member, key1, key2);
        }
    }

    /*
     * required data wasn't found (module) throw print an error 
     */
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - module wasn't found  %s\n"), module));
    return NULL;
}

const uint32 *
dnxc_data_utils_generic_1d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1)
{
    return dnxc_data_utils_generic_2d_data_get(unit, module, submodule, data, member, key1, 0);
}

const uint32 *
dnxc_data_utils_generic_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1)
{
    return dnxc_data_utils_generic_2d_data_get(unit, module, submodule, data, member, 0, 0);
}

char *
dnxc_data_utils_generic_feature_data_str_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name,
    char *buffer)
{
    int feature_index;
    dnxc_data_feature_t *features;
    int nof_features;
    int val;

    /*
     * Get features array
     */
    features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].features;
    nof_features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_features;

    /*
     * Iterate over features 
     */
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {
        if (!sal_strncasecmp(data_name, features[feature_index].name, strlen(data_name)))
        {
            /*
             * If found - convert value to string
             */
            val = features[feature_index].data;
            sal_sprintf(buffer, "%d", val);
            return buffer;

        }
    }

    return NULL;
}

char *
dnxc_data_utils_generic_define_data_str_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name,
    char *buffer)
{
    int define_index;
    dnxc_data_define_t *defines;
    int nof_defines;
    shr_error_e rv;

    /*
     * Get list of defines
     */
    defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].defines;
    nof_defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_defines;

    /*
     * Iterate over defines 
     */
    for (define_index = 0; define_index < nof_defines; define_index++)
    {
        if (!sal_strncasecmp(data_name, defines[define_index].name, strlen(data_name)))
        {
            /*
             * Make sure that data is supported
             */
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                              defines[define_index].set);
            if (rv != _SHR_E_NONE)
            {
                /*
                 * in case access denied - return string 'null'
                 */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"),
                           defines[define_index].name, _dnxc_data[unit].name));
                sal_sprintf(buffer, "%s", "null");
            }

            /*
             * Convert value to string
             */
            sal_sprintf(buffer, "%d", defines[define_index].data);
            return buffer;
        }
    }

    return NULL;
}

char *
dnxc_data_utils_generic_table_data_str_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data,
    char *member,
    int key1,
    int key2,
    char *buffer)
{
    int table_index, value_index;
    dnxc_data_table_t *tables;
    int nof_tables;
    shr_error_e rv;

    /*
     * 1. Get tables array
     * 2. Look for the requested table
     * 3. Look for the requested member
     * 4. get values as string
     */
    tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].tables;
    nof_tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_tables;

    /*
     * Iterate over tables 
     */
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(data, tables[table_index].name, strlen(data)))
        {
            if (tables[table_index].entry_get == NULL)
            {
                /*
                 * table is not supported by the device 
                 */
                return NULL;
            }

            /*
             * iterate over values 
             */
            for (value_index = 0; value_index < tables[table_index].nof_values; value_index++)
            {
                if (!sal_strncasecmp(member, tables[table_index].values[value_index].name, strlen(member)))
                {
                    /*
                     * If found - get values as string
                     */
                    rv = tables[table_index].entry_get(unit, buffer, key1, key2, value_index);
                    if (rv != _SHR_E_NONE)
                    {
                        return NULL;
                    }
                    return buffer;
                }
            }

            return NULL;
        }
    }

    return NULL;
}

char *
dnxc_data_utils_generic_module_str_get(
    int unit,
    int module_index,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2,
    char *buffer)
{
    int submodule_index;
    char *res;
    dnxc_data_module_t *module = &_dnxc_data[unit].modules[module_index];
    dnxc_data_submodule_t *submodules = module->submodules;

    /*
     * Iterate over submodules 
     */
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        if (!sal_strncasecmp(submodule, submodules[submodule_index].name, strlen(submodule)))
        {

            /*
             * look for feature with that name 
             */
            res = dnxc_data_utils_generic_feature_data_str_get(unit, module_index, submodule_index, data, buffer);
            if (res != NULL)
            {
                return res;
            }

            /*
             * look for define with that name 
             */
            res = dnxc_data_utils_generic_define_data_str_get(unit, module_index, submodule_index, data, buffer);
            if (res != NULL)
            {
                return res;
            }

            /*
             * look for define with that name 
             */
            res = dnxc_data_utils_generic_table_data_str_get(unit, module_index, submodule_index, data, member, key1,
                                                             key2, buffer);
            if (res != NULL)
            {
                return res;
            }

            /*
             * required data wasn't found (data and member) throwan error 
             */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "DNX DATA - data wasn't found  %s, %s\n"), data, member != NULL ? member : ""));
            return NULL;
        }
    }

    /*
     * required data wasn't found (submodule) throw an error 
     */
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - submodule wasn't found  %s\n"), submodule));
    return NULL;
}

char *
dnxc_data_utils_generic_data_str_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2,
    char *buffer,
    int buffer_size)
{
    int module_index;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;

    /*
     * Iterate over modules 
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        if (!sal_strncasecmp(module, modules[module_index].name, strlen(module)))
        {
            /*
             * If module found look for the data in that module
             */
            return dnxc_data_utils_generic_module_str_get(unit, module_index, submodule, data, member, key1, key2,
                                                          buffer);
        }
    }

    /*
     * required data wasn't found (module) throw print an error 
     */
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - module wasn't found  %s\n"), module));
    return NULL;
}

/**
 * \brief - Get module and submodule internal indexes
 *
 * \param [in] unit - The unit number.
 * \param [in] module_name - Module name
 * \param [in] submodule_name - Submodule name
 * \param [out] module_index - Returned module index
 * \param [out] submodule_index - Returned submodule index
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_data_utils_indices_get(
    int unit,
    char *module_name,
    char *submodule_name,
    int *module_index,
    int *submodule_index)
{
    dnxc_data_module_t *modules;

    SHR_FUNC_INIT_VARS(unit);

    /** get modules array */
    modules = _dnxc_data[unit].modules;

    /*
     * Iterate over modules
     */
    for (*module_index = 0; *module_index < _dnxc_data[unit].nof_modules; (*module_index)++)
    {
        if (!sal_strncasecmp(module_name, modules[*module_index].name, strlen(module_name)))
        {
            /** If module found - look for requested data in that module */
            dnxc_data_module_t *module;
            dnxc_data_submodule_t *submodules;

            /** Get list of submodules */
            module = &_dnxc_data[unit].modules[*module_index];
            submodules = module->submodules;

            /** Iterate over submodules */
            for (*submodule_index = 0; *submodule_index < module->nof_submodules; (*submodule_index)++)
            {
                if (!sal_strncasecmp(submodule_name, submodules[*submodule_index].name, strlen(submodule_name)))
                {
                    /** success! submodule found, finish the iteration */
                    SHR_EXIT();
                }
            }

            SHR_ERR_EXIT(_SHR_E_PARAM, "DNX DATA - submodule wasn't found  %s\n", submodule_name);
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "DNX DATA - module wasn't found  %s\n", module_name);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
dnxc_data_define_t *
dnxc_data_utils_define_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *define_name)
{
    int module_index, submodule_index, define_index;
    dnxc_data_module_t *modules;
    dnxc_data_define_t *defines;
    int nof_defines;
    shr_error_e rv;

    /** get modules array */
    modules = _dnxc_data[unit].modules;

    /** find the provided module and submodule */
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    /*
     * Get defines array
     */
    defines = modules[module_index].submodules[submodule_index].defines;
    nof_defines = modules[module_index].submodules[submodule_index].nof_defines;

    /*
     * Iterate over defines
     */
    for (define_index = 0; define_index < nof_defines; define_index++)
    {
        if (!sal_strncasecmp(define_name, defines[define_index].name, strlen(define_name)))
        {
            /*
             * If define found - make sure define is supported
             */
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                              defines[define_index].set);
            if (rv != _SHR_E_NONE)
            {
                /** in case access denied */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"),
                           defines[define_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            /** If supported - return pointer to data struct */
            return &defines[define_index];
        }
    }

    return NULL;
}

/*
 * See .h file
 */
dnxc_data_feature_t *
dnxc_data_utils_feature_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *feature_name)
{
    int module_index, submodule_index, feature_index;
    dnxc_data_module_t *modules;
    dnxc_data_feature_t *features;
    int nof_features;
    shr_error_e rv;

    /** get modules array */
    modules = _dnxc_data[unit].modules;

    /** find the provided module and submodule */
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    /*
     * Get features array
     */
    features = modules[module_index].submodules[submodule_index].features;
    nof_features = modules[module_index].submodules[submodule_index].nof_features;

    /*
     * Iterate over features
     */
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {
        if (!sal_strncasecmp(feature_name, features[feature_index].name, strlen(feature_name)))
        {
            /*
             * If feature found - make sure feature is supported
             */
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, features[feature_index].flags,
                                              features[feature_index].set);
            if (rv != _SHR_E_NONE)
            {
                /** in case access denied */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - feature %s access denied for device %s\n"),
                           features[feature_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            /** If supported - return pointer to data struct */
            return &features[feature_index];
        }
    }

    return NULL;
}

/*
 * See .h file
 */
dnxc_data_table_t *
dnxc_data_utils_table_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *table_name)
{
    int module_index, submodule_index, table_index;
    dnxc_data_module_t *modules;
    dnxc_data_table_t *tables;
    int nof_tables;
    shr_error_e rv;

    /** get modules array */
    modules = _dnxc_data[unit].modules;

    /** find the provided module and submodule */
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    /*
     * Get features array
     */
    tables = modules[module_index].submodules[submodule_index].tables;
    nof_tables = modules[module_index].submodules[submodule_index].nof_tables;

    /*
     * Iterate over tables
     */
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(table_name, tables[table_index].name, strlen(table_name)))
        {
            /*
             * If table found - make sure table is supported
             */
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, tables[table_index].flags,
                                              tables[table_index].set);
            if (rv != _SHR_E_NONE)
            {
                /** in case access denied */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - feature %s access denied for device %s\n"),
                           tables[table_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            /** If supported - return pointer to data struct */
            return &tables[table_index];
        }
    }

    return NULL;
}

/*
 * See .h file
 */
uint32 *
dnxc_data_utils_table_member_data_pointer_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *table_name,
    char *member_name,
    int key1,
    int key2)
{
    int module_index, submodule_index, table_index, value_index;
    dnxc_data_module_t *modules;
    dnxc_data_table_t *tables;
    int nof_tables;
    char *table_data;
    shr_error_e rv;

    /** get modules array */
    modules = _dnxc_data[unit].modules;

    /** find the provided module and submodule */
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    /*
     * Get features array
     */
    tables = modules[module_index].submodules[submodule_index].tables;
    nof_tables = modules[module_index].submodules[submodule_index].nof_tables;

    /*
     * Iterate over tables
     */
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(table_name, tables[table_index].name, strlen(table_name)))
        {
            /*
             * If table found - make sure table is supported by device
             */
            table_data = dnxc_data_mgmt_table_data_get(unit, &tables[table_index], key1, key2);
            if (table_data == NULL)
            {
                /*
                 * table is not supported by the device - return null
                 */
                return NULL;
            }

            /*
             * iterate over values
             */
            for (value_index = 0; value_index < tables[table_index].nof_values; value_index++)
            {
                if (!sal_strncasecmp(member_name, tables[table_index].values[value_index].name, strlen(member_name)))
                {
                    /*
                     * If value found - return pointer to data
                     */
                    return (uint32 *) (table_data + tables[table_index].values[value_index].offset);
                }
            }

            return NULL;
        }
    }

    return NULL;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_utils_label_set(
    int unit,
    char **labels,
    char *label)
{
    int label_index;

    SHR_FUNC_INIT_VARS(unit);

    /** iterate over the labels and find a free spot */
    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
    {
        /** check if label is free */
        if (labels[label_index] == NULL)
        {
            labels[label_index] = label;
            break;
        }
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/*
 * }
 */
