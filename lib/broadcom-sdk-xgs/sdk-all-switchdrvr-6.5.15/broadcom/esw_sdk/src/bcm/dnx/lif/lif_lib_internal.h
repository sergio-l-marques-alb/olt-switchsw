/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_lib_internal.h
 * $Id$ 
 * 
 * This file contains the APIs required to perform general lif HW writes, as well as lif allocation.
 */

#ifndef  INCLUDE_LIF_LIB_INTERNAL_H
#define  INCLUDE_LIF_LIB_INTERNAL_H

/*************
 * INCLUDES  *
 *************/
/*
 * { 
 */

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
/**
 * \brief
 * If LIF_LIB_GLEM_ACCESS_IS_OPTIONAL is non-zero then the code accepts failure to
 * read an entry from GLEM provided it is 'not found'.
 * This is mainly for 'remove', in which case, this passes silently. 
 * For 'read', it is up to the caller to decide.
 * Most probably this flag and option will be removed on the final version.
 */
#define LIF_LIB_GLEM_ACCESS_IS_OPTIONAL              1
/** 
 * }
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
 * \brief
 *   Allocate local inlif and save its dbal table and result in sw state.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in,out] inlif_info -
 *   Pointer to memory for local inlif allocation input/output. \n
 *   \b As \b input - \n
 *     All elements in inlif info are required as input, except for local_inlif which will be ignored.
 *     In the case of duplicated per core lif table, the field 'core_id' will specify which core is used
 *        for allocation, or it can be set to _SHR_CORE_ALL to allocate one lif per core.
 *     See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *   \b As \b output - \n
 *     This procedure loads the field inlif_info->local_inlifs with the local in lifs allocated, in the manner
 *     detailed in the struct's description.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info);

/**
 * \brief
 *   Deallocate the given local inlif.
 *
 * \param [in] unit -
 *     Identifier of the device to access.
 * \param [in] inlif_info -
 *     Pointer to memory for local inlif deallocation. \n
 *     \b As \b input - \n
 *       The local inlif[s] in the local_inlifs array will be freed. \n
 *       All elements in inlif info are required as input, except for dbal_result_type, which should be 0.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info);

/**
 * \brief
 *   Allocate local outlif, save its dbal table and result in sw state and configure HW.
 *
 *   This API performs several functions:
 *   1. Allocate local outlif and save its dbal table+result.
 *   2. Sometimes, configured the HW to map this lif to its logical phase and match it with Linked list entry.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in,out] outlif_info -
 *   Pointer to memory for local outlif allocation input/output. \n
 *   \b As \b input - \n
 *     All elements in outlif info are inputs, except for local_outlif.
 *   \b As \b output - \n
 *     This procedure loads the field outlif_info->local_outlif with the local out lif allocated.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_outlif_allocate(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info);

/**
 * \brief
 *   Deallocate the given local outlif.
 *
 *   All the steps described in lif_lib_allocate_outlif will be reverted.
 *
 * \param [in] unit -
 *     Identifier of the device to access.
 * \param [in] local_outlif -
 *     Local outlif to be freed.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_outlif_free(
    int unit,
    int local_outlif);

/*
 * } 
 */

#endif /* INCLUDE_LIF_LIB_INTERNAL_H */
