/**
 * \file bcm_int/dnx/init/edk_init.h
 *
 * Internal DNX EDK Init APIs
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef EDK_H_INCLUDED
/*
 * {
 */
#define EDK_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/fabric.h>

/*
 * }
 */

/*
  * Internal functions
  * {
  */

/**
 * \brief - Initialize all EDK related resources
 *
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_edk_state_init(
    int unit);

/**
 * \brief - De initialize all EDK related resources
 *
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_edk_state_deinit(
    int unit);

/* } */
/* } */
#endif /** EDK_H_INCLUDED */
