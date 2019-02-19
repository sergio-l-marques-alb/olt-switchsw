/*! \file shrextend_error.h
 *
 * Shared error codes.
 *
 */
/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

#ifndef _SHREXTEND_ERROR_H_INCLUDED
/* { */
#define _SHREXTEND_ERROR_H_INCLUDED

/*!
 * \brief Standard SDK error codes.
 *
 * IMPORTANT: These error codes must match the corresponding text
 * messages in shrextend_error.c.
 */
typedef enum
{
    /*!
     * No errors
     */
  SHR_E_NONE                 =  0,
    /*!
     * Internal Error
     *   For internal use, not expose to the user
     */
  SHR_E_INTERNAL             = -1,
    /*!
     * Memory allocation error
     */
  SHR_E_MEMORY               = -2,
    /*!
     * Invalid unit number - Unit number is out of range
     */
  SHR_E_UNIT                 = -3,
    /*!
     * A function receives a wrong parameter
     *   None of the other errors fits to describe the problem
     *   (for example: A given value is out of range for the parameter
     */
  SHR_E_PARAM                = -4,
    /*!
     * Unable to delete a non existing entry
     *   To be used on 'delet' APIs or the like.
     */
  SHR_E_EMPTY                = -5,
    /*!
     * Data base is full
     *   Failed to add an entry to a full database
     */
  SHR_E_FULL                 = -6,
    /*!
     * An entry is not found in the database
     *   Use for 'get' APIs (or the like)
     */
  SHR_E_NOT_FOUND            = -7,
    /*!
     * Trying to add an entry which already exists
     */
  SHR_E_EXISTS               = -8,
    /*!
     * Exceeded allowed timeout
     *   For example: Waiting too long for HW access response.
     */
  SHR_E_TIMEOUT              = -9,
    /*!
     * The service is busy
     *   For example: Trying to delete an interface when it is still in use
     */
  SHR_E_BUSY                 = -10,
    /*!
     * A function fails
     *   Returned to the user, only if all other errors can't describe the error
     */
  SHR_E_FAIL                 = -11,
    /*!
     * Trying to configure disabled service
     *   For example:
     *     Trying to set up a service disabled by SOC property
     *     Trying to configure a disabled module
     */
  SHR_E_DISABLED             = -12,
    /*!
     * Wrong parameter (provided no better description exists)
     */
  SHR_E_BADID                = -13,
    /*!
     * Out of resource (not including memory)
     */
  SHR_E_RESOURCE             = -14,
    /*!
     * Calling a function with a legal but not supported (in current configuration) parameter
     */
  SHR_E_CONFIG               = -15,
    /*!
     * The service is supported on current device but is not available
     * on current setup
     */
  SHR_E_UNAVAIL              = -16,
    /*!
     * Initialization error on module level -
     *   A module is not initialized
     *   Initializing already initialized module
     */
  SHR_E_INIT                 = -17,
    /*!
     * Wrong port configuration
     */
  SHR_E_PORT                 = -18,
    /*!
     * The service is not supported on current device
     */
  SHR_E_NOT_SUPPORTED        = -19,
    /*!
     * Trying to access a feature which is not ready yet.
     *   For example, initialization of feature has started but has not yet been completed.
     */
  SHR_E_ACCESS               = -20,
    /*!
     * A handler for some object/operation was indicated but it does not exist.
     *   For example, identifier of transaction which does not exist.
     */
  SHR_E_NO_HANDLER           = -21,
    /*!
     * Last known code
     */
  SHR_E_LIMIT                = -22
} shr_error_e ;

/*!
 * Check for successful return value.
 */
#define SHR_SUCCESS(_expr) ((_expr) >= 0)

/*!
 * Check for error return value.
 */
#define SHR_FAILURE(_expr) ((_expr) < 0)

/*!
 * \brief Get standard error message
 *
 * Returns a text message corresponding to the error code passed in.
 *
 * \param [in] rv Error code
 *
 * \return Pointer to error message
 */
extern const char *shrextend_errmsg_get(int rv) ;
/* } */
#endif  /* !_SHREXTEND_ERROR_H_INCLUDED */
