/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_table_mngr.h
 * $Id$
 *
 * This file contains the public APIs required for lif table algorithms.
 *
 */

#ifndef  INCLUDE_LIF_TABLE_MNGR_H_INCLUDED
#define  INCLUDE_LIF_TABLE_MNGR_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
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
/**
 * \brief Specific rules per table.
 *
 * /see
 * dnx_lif_mngr_result_type_deduce
 */
typedef struct
{
    /**
     * Array of forbidden fields that cannot appear in the result type.
     */
    dbal_fields_e forbidden_fields_dbal_ids[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
    /**
     *  Number of forbidden fields.
     */
    int forbidden_fields_nof;
    /**
     * Flags that are used for tables where advanced rules should be applied
     */
    uint32 allowed_result_types_bmp;
} lif_table_mngr_table_specific_rules_t;

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
 * \brief - Set specific rules for result type filtering per table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - table id relevant to the current lif
 *   \param [in] result_type - Result type of the current lif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_table_specific_flags_fill(
    int unit,
    dbal_tables_e dbal_table_id,
    int result_type,
    uint32 *table_specific_flags);

/**
 * \brief - Find best result type for given set of fields.
 * For each result type X (starting from smaller and increasing)
 *  For each input field Y
 *     check if field Y exists in result type X
 *     if no - continue to next result type
 *     If yes - check the size of the field
 *        If the size is big enough continue to the next field
 *        Else continue to next result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - table id of the lif
 *   \param [in] table_specific_flags - lif table specific information
 *   \param [in] entry_handle_id - DBAL handle id containing all the fields
 *   \param [out] result_type_decision - selected result type of the current lif
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_algo_lif_table_mngr_decide_result_type(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 table_specific_flags,
    uint32 entry_handle_id,
    uint32 *result_type_decision);

/**
 * \brief - Sort all lif tables result types according to size.
 * Should be done once on device init.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *
 */
shr_error_e dnx_algo_lif_table_mngr_sorted_result_types_table_init(
    int unit);
/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_H_INCLUDED */
