/** \file policer_tune.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef _POLICER_TUNE_H_INCLUDED_
/** { */
#define _POLICER_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief
 *      policer configurations in tune stage
 * \param [in] unit -unit id
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_tune_init(
    int unit);

#endif /*_POLICER_TUNE_H_INCLUDED_*/
