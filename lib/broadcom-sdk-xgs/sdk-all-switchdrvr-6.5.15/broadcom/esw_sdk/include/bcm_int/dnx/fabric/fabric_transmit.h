/** \file fabric_transmit.h
 *
 * Functions for handling Fabric transmit adapter.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __DNX_FABRIC_TRANSMIT_INCLUDED__
/* { */
#define __DNX_FABRIC_TRANSMIT_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/shrextend/shrextend_error.h>

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/**
 * \brief
 *   Initialize Fabric Transmit Adapter configurations,
 *   including DTQs, PDQs, etc.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_transmit_init(
    int unit);

/**
 * \brief
 *   Initialize DTQs contexts.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_dtqs_contexts_init(
    int unit);

/* } */

/* } __JER2_FABRIC_TRANSMIT_INCLUDED__*/
#endif

