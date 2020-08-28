/*
 * $Id: bare_metal_dynamic_loading.h
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef BARE_METAL_DYNAMIC_LOADING_H_INCLUDED

#define BARE_METAL_DYNAMIC_LOADING_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

/*
 * \brief - Get file path for ARR dynamic dnx data file path to be loaded
 * \param [in] unit - Device Id
 * \param [out] arr_prefix_dynamic_file - ARR dynamic dnx data file path
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_arr_prefix_dynamic_file_get(
    int unit,
    char arr_prefix_dynamic_file[RHFILE_MAX_SIZE]);

/*
 * \brief - Get file path for AOD tables dynamic dnx data file path to be loaded
 * \param [in] unit - Device Id
 * \param [out] aod_dynamic_file - AOD dynamic dnx data file path
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_aod_tables_dynamic_file_get(
    int unit,
    char aod_dynamic_file[RHFILE_MAX_SIZE]);

/*
 * \brief - Get file path for MDB dynamic dnx data file path to be loaded
 * \param [in] unit - Device Id
 * \param [out] mdb_app_db_dynamic_file - MDB dynamic dnx data file path
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mdb_app_db_dynamic_file_get(
    int unit,
    char mdb_app_db_dynamic_file[RHFILE_MAX_SIZE]);

/*
 * \brief - Get file path for Pemla soc dynamic dnx data file path to be loaded
 * \param [in] unit - Device Id
 * \param [out] pemla_soc_file - Pemla soc dnx data file path
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pemla_soc_dynamic_file_get(
    int unit,
    char pemal_soc_file[RHFILE_MAX_SIZE]);

/*
 * \brief - Get file path for AOD sizes definitions dynamic dnx data file path to be loaded
 * \param [in] unit - Device Id
 * \param [out] aod_sizes_file -Aod dnx data file path
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_aod_sizes_dynamic_file_get(
    int unit,
    char aod_sizes_file[RHFILE_MAX_SIZE]);

#endif
