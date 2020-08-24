/** \file dnxc_data_dyn_loader.h
 * 
 * DEVICE DATA Dynamic Loader - Function used for dynamic DATA parsing.
 * This method is mostly useful for SKUs handling. 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNXC_DATA_DYN_LOADER_H_
/*{*/
#define _DNXC_DATA_DYN_LOADER_H_

/**
* \brief This file is only used by DNXC. Including it by
* software that is not specific to DNXC is an error.
*/
#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <shared/utilex/utilex_str.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
/*
 * }
 */

/**
 * \brief - Parse dynamic DNX DATA. 
 * Usually DNX data is parsed statically (auto coder) but in order to support SKUs, some features 
 * require that their data will be loaded dynamically 
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Unlike static DNX Data which is divided into XML files per module, in Dynamic Data all the information will be
 *     gathered inside a single file per chip(SKU).
 *
 *   * Limitations and assumptions:
 *      * Dynamic Data which depends on soc property (numeric, table) doesn't change the method type from the
 *        original data.
 *      * In case of 'Direct Mapping' method, Dynamic Data doesn't support new mapping (meaning that only disabling
 *        some of the originally supported mappings is allowed.).
 *
 * \see
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_parse_init(
    int unit);

/**
 * \brief - Free resources allocated by dnxc_data_dyn_loader_parse_init()
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_parse_deinit(
    int unit);

/**
 * \brief Pointer to function to get dynamic data path relative to db directory
 *
 * \param [in] unit - Unit #
 * \param [in] file_path - path to dynamic data file relative to db directory
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
typedef shr_error_e(
    *dnxc_data_dyn_file_path_get_f) (
    int unit,
    char file_path[RHFILE_MAX_SIZE]);

/**
 * \brief Override define value with the provided dynamic value
 *
 * \param [in] unit - Unit #
 * \param [in] define_info - pointer to define info \see dnxc_data_define_t.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_define_value_set(
    int unit,
    dnxc_data_define_t * define_info);

/**
 * \brief Override feature value with the provided dynamic value
 *
 * \param [in] unit - Unit #
 * \param [in] feature_info - pointer to feature info \see dnxc_data_feature_t.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_feature_value_set(
    int unit,
    dnxc_data_feature_t * feature_info);

/**
 * \brief Override numeric value with the provided dynamic value
 *
 * \param [in] unit - Unit #
 * \param [in] numeric_info - pointer to numeric info \see dnxc_data_define_t.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_numeric_value_set(
    int unit,
    dnxc_data_define_t * numeric_info);

/**
 * \brief Override table values with the provided dynamic value
 *
 * \param [in] unit - Unit #
 * \param [in] table_info - pointer to table info \see dnxc_data_table_t.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_table_value_set(
    int unit,
    dnxc_data_table_t * table_info);

/**
 * \brief - Verify the provided DNX data belongs to the device.
 * If the device ID doesn't match the ID in DNX data XML - return an error.
 *
 * \param [in] unit - Unit #
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_dev_id_verify(
    int unit);

/*}*/
#endif /*_DNXC_DATA_DYN_LOADER_H_*/
