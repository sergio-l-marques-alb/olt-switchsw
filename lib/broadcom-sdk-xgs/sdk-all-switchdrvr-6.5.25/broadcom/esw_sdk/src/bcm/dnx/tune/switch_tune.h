/** \file switch_tune.h
 * $Id$
 * 
 * Internal initializations for trap 
 * 
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef _SWITCH_TUNE_H_INCLUDED_
/** { */
#define _SWITCH_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm/switch.h>

/**
 * \brief - Initialization for Switch.
 * Initialize Split Horizon default filter - trap HUB-HUB.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  Asumes that HUB network group Id is 1.
 *
 * \see
 *  None.
 */
shr_error_e dnx_tune_switch_init(
    int unit);

#endif /*_SWITCH_TUNE_H_INCLUDED_*/
