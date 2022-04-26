/** \file dnx_sw_state.h
 * This is the main dnx sw state module.
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_SW_STATE_H
/* { */
#define _DNX_SW_STATE_H

/*
 * Functions
 * {
 */

/**
 * \brief - perform sw state module init
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_init(
    int unit);

/**
 * \brief - perform sw state module deinit
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_deinit(
    int unit);

/* } */
#endif /* _DNX_SW_STATE_H */
