/**
 * \file dnx_hw_overwrites.h
 *
 *  Internal DNX HW Managment APIs
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  All Rights Reserved.
 *
 */

#ifndef DNX_HW_OVERWRITES_H_INCLUDED
/*
 * {
 */
#define DNX_HW_OVERWRITES_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* */

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <shared/shrextend/shrextend_debug.h>

/**
 * }
 */

/*
 * Defines
 * {
 */

/**
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/*
 * }
 */

/**
 * \brief - Set OAM overwrites.
 *
 * \param [in]  unit - unit id
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 * \see
 *
 */
shr_error_e dnx_hw_overwrite_init(
    int unit);

#endif /* DNX_HW_OVERWRITES_H_INCLUDED */
