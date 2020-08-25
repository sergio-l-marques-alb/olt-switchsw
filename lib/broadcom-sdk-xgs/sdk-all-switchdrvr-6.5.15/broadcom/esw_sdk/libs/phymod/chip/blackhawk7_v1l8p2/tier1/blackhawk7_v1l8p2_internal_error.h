/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :   *
 *                                                                                *
 *  Description :  Internal API error functions                                   *
 *                                                                                *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                          *
 *  No portions of this material may be reproduced in any form without            *
 *  the written permission of:                                                    *
 *      Broadcom Corporation                                                      *
 *      5300 California Avenue                                                    *
 *      Irvine, CA  92617                                                         *
 *                                                                                *
 *  All information contained in this document is Broadcom Corporation            *
 *  company private proprietary, and trade secret.                                *
 *                                                                                *
 **********************************************************************************
 **********************************************************************************/

/** @file blackhawk7_v1l8p2_internal_error.h
 * Internal API error functions
 */

#ifndef BLACKHAWK7_V1L8P2_API_INTERNAL_ERROR_H
#define BLACKHAWK7_V1L8P2_API_INTERNAL_ERROR_H

#include "common/srds_api_err_code.h"
#include "common/srds_api_types.h"


/**
 * Error-trapping macro.
 *
 * In other then SerDes-team post-silicon evaluation builds, simply yields
 * the error code supplied as an argument, without further action.
 */
#define _error(err_code) blackhawk7_v1l8p2_INTERNAL_print_err_msg(sa__, (err_code))
/**@}*/

/** Print Error messages to screen before returning.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param err_code Error Code input which is returned as well
 * @return Error Code
 */
err_code_t blackhawk7_v1l8p2_INTERNAL_print_err_msg(srds_access_t *sa__, uint16_t err_code);

/** Print Convert Error code to String.
 * @param err_code Error Code input which is converted to string
 * @return String containing Error code information.
 */
const char* blackhawk7_v1l8p2_INTERNAL_e2s_err_code(err_code_t err_code);

#endif
