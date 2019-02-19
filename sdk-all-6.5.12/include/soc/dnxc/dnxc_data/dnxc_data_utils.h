/** \file dnxc_data_utils.h
 * 
 * DEVICE DATA UTILS - Utilities function for DNX DATA
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * $Copyright: (c) 2017 Broadcom.
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
 * \brief Generic function to get dnxc data 
 *        (will be mostly used as xml pointer read)
 *        The function will look by name the data and will return const pointer to the required data.
 *        Typically will be used for  table with two keys. 
 *
 * \param [in] unit - Unit #
 * \param [in] module - module name
 * \param [in] submodule - submodule name
 * \param [in] data - data name
 * \param [in] member - member name - required only to get data from tables (otherwiae should be NULL) 
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
 * \param [in] module_name - module name
 * \param [in] submodule_name - submodule name
 * \param [in] table_name - name of the desired table
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
    char *module_name,
    char *submodule_name,
    char *table_name,
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
/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_UTILS_H_*/
