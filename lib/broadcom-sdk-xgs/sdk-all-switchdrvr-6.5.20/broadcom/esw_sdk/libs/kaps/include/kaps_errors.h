/*******************************************************************************
 *
 * Copyright 2011-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __KAPS_ERRORS_H
#define __KAPS_ERRORS_H

#include "kaps_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @file errors.h
 *
 * Errors and fault reporting functions
 *
 */

/**
 * @addtogroup ERRORS_API
 * @{
 */

/**
 * @brief Status and error codes returned by all API functions.
 */

typedef enum kaps_status { /** \def KAPS_INC_SEL(name,string)
                              Select between enumeration and error string */
#define KAPS_INC_SEL(name,string) name,
#include "kaps_error_tbl.def"
#undef KAPS_INC_SEL
    KAPS_STATUS_LAST_UNUSED
} kaps_status;

/**
 * Function to convert a status code to a string. The returned
 * string should not be overwritten or freed by the caller.
 *
 * @param status Code to be converted.
 *
 * @retval string Verbose description of status code if valid.
 * @retval NULL if invalid status code is passed in.
 *
 * @warning The returned string should not be modified or freed.
 */

const char *kaps_get_status_string(kaps_status status);

/**
 * @brief Convenience macro to wrap function calls and capture error codes and their descriptions.
 *
 * A verbose error is printed when an error occurs.
 */

#define KAPS_TRY(A)                                                      \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            kaps_printf(#A" failed: %s\n", kaps_get_status_string(__tmp_status)); \
            return __tmp_status;                                        \
        }                                                               \
    }                                                                   \
    while(0)

/**
 * @brief Convenience macro to wrap function calls and capture error codes.
 *
 * This macro is silent; it does not output any verbose messages.
 */

#define KAPS_STRY(A)                                                     \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)

/**
 * @brief Convenience macro to wrap function calls and capture error codes.
 *
 * This API is expected error code; if error code other than KAPS_OK or Expected, return error.
                                    and it does not output any verbose messages.
 */

#define KAPS_ETRY(A,E)                                                   \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        kaps_status __exp_status = E;                                    \
        if (__tmp_status != KAPS_OK && __tmp_status != __exp_status)     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)


/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif                          /* __ERRORS_H */
