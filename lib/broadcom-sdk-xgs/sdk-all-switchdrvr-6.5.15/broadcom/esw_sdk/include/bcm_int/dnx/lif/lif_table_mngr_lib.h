/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_table_mngr_lib.h
 * $Id$
 *
 * This file contains the public APIs required for lif table allocations HW configuration.
 *
 */

#ifndef  INCLUDE_LIF_TABLE_MNGR_LIB_H_INCLUDED
#define  INCLUDE_LIF_TABLE_MNGR_LIB_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * \brief - Manage local lif creation and lif table management:
 *          1. Decide on result type according to set of fields required
 *          2. Allocate local LIF
 *          3. Fill HW LIF table
 *          4. Write to GLEM if required
 *          5. Add global to local SW mapping
 *
 *      For replace:
 *          1. Find the new result type if changed
 *          2. Allocate new local lif
 *          3. Create new lif entry in HW Lif table and copy old content into the new entry
 *          4. Change all EEDB pointers to point on the new lif
 *          5. Update GLEM if required
 *          6. Write the new content to the new entry
 *          7. Free old local lif entry
 *          8. Update global to local SW mapping
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] local_lif_id - local outlif allocated
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_allocate_local_outlif_and_set_hw(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info);

/**
 * \brief - Manage local lif creation and lif table management:
 *      For new LIF:
 *          1. Decide on result type according to set of fields required
 *          2. Allocate local LIF
 *          3. Fill HW LIF table
 *          4. Add global to local SW mapping
 *
 *      For replace:
 *          1. Find the new result type if changed
 *          2. Allocate new local lif
 *          3. For AC: Create new lif entry in HW Lif table and copy old content into the new entry
 *          4. For AC: change all ISEM pointers to point on the new lif
 *          5. Write the new content to the new entry
 *          6. Free old local lif entry
 *          7. Update global to local SW mapping
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] local_lif_id - local inlif allocated
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_allocate_local_inlif_and_set_hw(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info);

/**
 * \brief - Get all fields
 *
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - local outlif to get
 *   \param [in] entry_handle_id - DBAL entry handle to fill.
 *                                Filled handle is of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] lif_info - struct containing lif information
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_get_outlif_info(
    int unit,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * lif_info);

/**
 * \brief - Get all fields
 *
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - local inlif to get
 *   \param [in] entry_handle_id - DBAL entry handle to fill.
 *                                Filled handle is of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] lif_info - struct containing lif information
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_get_inlif_info(
    int unit,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info);

/**
 * \brief - Clear local LIF SW and HW allocation, and GLEM
 *
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - local outlif to clear
 *   \param [in] flags - Flags LIF_TABLE_MNGR_LIF_INFO_XXX
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_outlif_info_clear(
    int unit,
    int local_lif_id,
    uint32 flags);

/**
 * \brief - Clear local LIF SW and HW allocation
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] local_lif_id - local inlif to clear
 *   \param [in] dbal_table_id - dbal table id to clear from
 *   \param [in] flags - Flags LIF_TABLE_MNGR_LIF_INFO_XXX
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_inlif_info_clear(
    int unit,
    int core_id,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 flags);

/**
 * \brief - Check if a given field exists on a given dabl handle.
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - dbal handle
 *   \param [in] field_id - required field
 *   \param [out] is_field_on_handle - indication if field is available
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *
 */
shr_error_e dnx_lif_table_mngr_is_field_exist_on_dbal_handle(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *is_field_on_handle);

/**
 * \brief - Check if a given field is valid for in/out lif entry.
 *          This is according to user specification on entry allocation.
 *          Field can be present in the result type but not enabled by the user.
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for inlif)
 *   \param [in] local_lif_id - local inlif or outlif
 *   \param [in] is_ingress - indication if ingress
 *   \param [in] physical_table_id - relevant only for ingress
 *   \param [in] field_id - the field that will be checked
 *   \param [out] is_valid - indication if field is available
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *
 */
shr_error_e dnx_lif_table_mngr_is_valid_field(
    int unit,
    int core_id,
    int local_lif_id,
    uint8 is_ingress,
    dbal_physical_tables_e physical_table_id,
    dbal_fields_e field_id,
    uint8 *is_valid);

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_LIB_H_INCLUDED */
