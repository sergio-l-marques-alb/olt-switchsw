/** \file bcm_int/dnx/pvt/pvt.h
 * 
 * Internal DNX PVT APIs 
 * 
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef PVT_H_INCLUDED
/* { */
#define PVT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/**
 * \brief
 *   Initialize PVT monitor module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pvt_mon_init(
    int unit);

/* } */
#endif /* PVT_H_INCLUDED */
