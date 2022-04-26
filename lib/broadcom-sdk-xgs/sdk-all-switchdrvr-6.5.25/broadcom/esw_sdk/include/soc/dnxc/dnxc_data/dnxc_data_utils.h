/** \file dnxc_data_utils.h
 * 
 * DEVICE DATA UTILS - Utilities function for DNX DATA
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNXC_DATA_UTILS_H_
/*{*/
#define _DNXC_DATA_UTILS_H_

/**
* \brief This file is only used by DNXC. Including it by
* software that is not specific to DNXC is an error.
*/
#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

/**
 * \brief - the max size of string buffer used by DNXC DATA
 */
#define DNXC_DATA_UTILS_STRING_MAX (1024)

/*
 * FUNCTIONS:
 * {
 */
/**
 * \brief Check weather to dump data according to input:
 * \par INDIRECT INPUT:
 *   \param [in] unit - Unit #
 *   \param [in] state_flags - component state  \n
 *     see DNX_DATA_STATE_F_* for details
 *   \param [in] data_flags - specific data flags \n
 *     see DNX_DATA_F_* for details
 *   \param [in] data_labels - list of string labels of the verified data  \n
 *   \param [in] dump_flags - flags to dump \n
 *   \param [in] dump_label - label to filter the data accordingly - set to NULL or "" in order not to filter \n
 *     see DNX_DATA_F_* for details
 *  
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   \param [out] dump - whether to dump or not \n
 *     see DNX_DATA_F_* for details
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_utils_dump_verify(
    int unit,
    uint32 state_flags,
    uint32 data_flags,
    char **data_labels,
    uint32 dump_flags,
    char *dump_label,
    int *dump);

/**
 * \brief Generic function to get dnxc data 
 *        (will be mostly used as xml pointer read)
 *        The function will look by name the data and will return const pointer to the required data.
 *        Typically will be used for define, numeric, feature or table with no keys.
 *
 * \param [in] unit - Unit #
 * \param [in] module - module name
 * \param [in] submodule - submodule name
 * \param [in] data - data namme
 * \param [in] member - member name - required only to get data from tables (otherwiae should be NULL) 
 *  
 * \return
 *   * pointer to the data - if the data is not uint32, use casting
 * \remark
 *   * None
 * \see
 *   * None
 */
const uint32 *dnxc_data_utils_generic_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member);
/**
 * \brief Generic function to get dnxc data 
 *        (will be mostly used as xml pointer read)
 *        The function will look by name the data and will return const pointer to the required data.
 *        Typically will be used for  table with single key. 
 *
 * \param [in] unit - Unit #
 * \param [in] module - module name
 * \param [in] submodule - submodule name
 * \param [in] data - data namme
 * \param [in] member - member name - required only to get data from tables (otherwiae should be NULL) 
 * \param [in] key1 - index of first key (if not exist - set to zero) 
 *  
 * \return
 *   * pointer to the data - if the data is not uint32, use casting
 * \remark
 *   * None
 * \see
 *   * None
 */
const uint32 *dnxc_data_utils_generic_1d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1);

/**
 * \brief Generic function to get dnxc data vs string representing dnx data
 *        The function will look by name the data and will return const pointer to the required data.
 *        Several forms are supported
 *               single data       - module.submodule.data,
 *               table with no key - module.submodule.data.member(),
 *               table with 1 key  - module.submodule.data.member(key)
 *               table with 2 keys - module.submodule.data.member(key1, key2)
 * \param [in] unit - Unit #
 * \param [in] dnx_data_str - String representing dnx data path in one of formats
 * \return
 *   * pointer to the data - if the data is not uint32, use casting, NULL if error
 * \remark
 *   * None
 * \see
 *   * None
 */
const uint32 *dnxc_data_utils_generic_value_get(
    int unit,
    char *dnx_data_str);

/**
 * \brief Generic function to get dnxc data 
 *        (will be mostly used as xml pointer read)
 *        The function will look by name the data and will return const pointer to the required data.
 *        Typically will be used for  table with two keys. 
 *
 * \param [in] unit - Unit #
 * \param [in] module - module name
 * \param [in] submodule - submodule name
 * \param [in] data - data name
 * \param [in] member - member name - required only to get data from tables (otherwise should be NULL)
 * \param [in] key1 - index of first key (if not exist - set to zero)
 * \param [in] key2 - index of second key (if not exist - set to zero)
 * \return
 *   * pointer to the data - if the data is not uint32, use casting
 * \remark
 *   * None
 * \see
 *   * None
 */
const uint32 *dnxc_data_utils_generic_2d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2);

/**
 * \brief - Retrieve define data structure
 *
 * \param [in] unit - Unit #
 * \param [in] module_name - module name
 * \param [in] submodule_name - submodule name
 * \param [in] define_name - name of the desired define
 * \return
 *   * pointer to the data
 * \remark
 *   * None
 * \see
 *   * None
 */
dnxc_data_define_t *dnxc_data_utils_define_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *define_name);

/**
 * \brief - Retrieve feature data structure
 *
 * \param [in] unit - Unit #
 * \param [in] module_name - module name
 * \param [in] submodule_name - submodule name
 * \param [in] feature_name - name of the desired feature
 * \return
 *   * pointer to the data
 * \remark
 *   * None
 * \see
 *   * None
 */
dnxc_data_feature_t *dnxc_data_utils_feature_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *feature_name);

/**
 * \brief - Retrieve table data structure
 *
 * \param [in] unit - Unit #
 * \param [in] module_name - module name
 * \param [in] submodule_name - submodule name
 * \param [in] table_name - name of the desired table
 * \return
 *   * pointer to the data
 * \remark
 *   * None
 * \see
 *   * None
 */
dnxc_data_table_t *dnxc_data_utils_table_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *table_name);

/**
 * \brief - Retrieve table  member data offset address
 *
 * \param [in] unit - Unit #
 * \param [in] table_info - pointer to table info
 * \param [in] member_name - name of the member
 * \param [in] key1 - table 1st key
 * \param [in] key2 - table 2nd key
 * \return
 *   * pointer to the data
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 *dnxc_data_utils_table_member_data_pointer_get(
    int unit,
    dnxc_data_table_t * table_info,
    char *member_name,
    int key1,
    int key2);

/**
 * \brief - Add a new label to an exsiting array of data labels
 *
 * \param [in] unit - Unit #
 * \param [in] labels - Array of labels that belong to a certain data (define, numeric, feature, table)
 * \param [in] label - new label to add
 * \return
 *   * shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_utils_label_set(
    int unit,
    char **labels,
    char *label);

/**
 * \brief - Get a string with info about the supported values for a soc property
 *
 * \param [in] unit - Unit #
 * \param [in] property - property structure
 * \param [in] buffer - buffer for the string
 * \param [in] buffer_size - max size of the buffer
 * \param [in] show_mapping - display mapping from property value to data value
 * \return
 *   * shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_utils_supported_values_get(
    int unit,
    dnxc_data_property_t * property,
    char *buffer,
    int buffer_size,
    int show_mapping);

/**
 * \brief - Convert DNXC DATA value to soc property value
 *
 * \param [in] unit - Unit #
 * \param [in] property - property structure used to load the DNXC DATA value
 * \param [in] val_str - string represent DNXC DATA value
 * \param [in] buffer_property_val_str - buffer for the string
 * \param [in] buffer_size - max size of the buffer
 * \return
 *   * shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_utils_property_val_get(
    int unit,
    dnxc_data_property_t * property,
    char *val_str,
    char *buffer_property_val_str,
    int buffer_size);

/**
 * \brief - Retrieve property value of given DNXC DATA (generic version)
 *
 * \param [in] unit - Unit #
 * \param [in] module - module name
 * \param [in] submodule - submodule name
 * \param [in] data - name of the desired data
 * \param [in] member - member name (relevant for tables only)
 * \param [out] property - const pointer to retrieved property
 * \return
 *   * shr_error_e
 * \remark
 *   * if not found property is NULL
 * \see
 *   * None
 */
shr_error_e dnxc_data_utils_generic_property_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    const dnxc_data_property_t ** property);

/**
 * \brief - Retrieve property value of given DNXC DATA (generic version for TCL)
 *
 * \param [in] unit - Unit #
 * \param [in] module - module name
 * \param [in] submodule - submodule name
 * \param [in] data - name of the desired data
 * \param [in] member - member name (relevant for tables only)
 * \param [in] buffer - buffer for the string
 * \param [in] buffer_size - max size of the buffer
 * \return
 *   * pointer to the data
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_utils_generic_property_val_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    char *buffer,
    int buffer_size);
/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_UTILS_H_*/
