/** \file trap_tune.h
 * $Id$
 * 
 * Internal initializations for trap 
 * 
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef _TRAP_TUNE_H_INCLUDED_
/** { */
#define _TRAP_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Initialization for trap.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
shr_error_e dnx_tune_trap_init(
    int unit);

#endif /*_TRAP_TUNE_H_INCLUDED_*/
