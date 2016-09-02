/*! \file shrextend_error.h
 *
 * Shared error codes for JR2 system.
 * Note that they are not necessarily equal to the BCN API
 * error values and, therefore, need to be converted.
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

#ifndef SHREXTEND_ERROR_H_INCLUDED
/* { */
#define SHREXTEND_ERROR_H_INCLUDED
/*
 * Note:
 *   This file, shared/error.h, is added here because JR2 uses the _SHR error
 *   codes (which JR1 is using for both BCM and SOC).
 *   See _shr_error_t
 */
#include <shared/error.h>
#include <shared/bslenum.h>
/*!
 * \brief
 *   Creating type for procedure which is similar to bsldnx_get_prefix_range_min_max()
 *   This is used by bsldnx_mgmt_init() to load the address of bsldnx_get_prefix_range_min_max()
 *   into a static variable of DNX error/debug system (get_prefix_range_min_max)
 *
 *   This is required since the 'prefix' system for BSL is handled by DIAG. ('prefix is
 *   file, line, function name, ...)
 */
typedef int (
  *BSLDNX_GET_PREFIX_RANGE_MIN_MAX) (
  bsl_severity_t * min_severity,
  bsl_severity_t * max_severity);
/*!
 * \brief
 *   Creating type for procedure which is similar to bsldnx_set_prefix_range_min_max()
 *   This is used by bsldnx_mgmt_init() to load the address of bsldnx_set_prefix_range_min_max()
 *   into a static variable of DNX error/debug system (set_prefix_range_min_max)
 *
 *   This is required since the 'prefix' system for BSL is handled by DIAG. ('prefix is
 *   file, line, function name, ...)
 */
typedef int (
  *BSLDNX_SET_PREFIX_RANGE_MIN_MAX) (
  bsl_severity_t min_severity,
  bsl_severity_t max_severity);
/*!
 * \brief
 *   Creating type for procedure which is similar to bslenable_get()()
 *   This is used by bsldnx_mgmt_init() to load the address of bslenable_get()
 *   into a static variable of DNX error/debug system (dnx_bslenable_get)
 *
 *   This is required since the 'severity' system for BSL is handled by DIAG.
 */
typedef bsl_severity_t(
  *BSLENABLE_GET) (
  bsl_layer_t layer,
  bsl_source_t source);
/*!
 * \brief
 *   Creating type for procedure which is similar to bslenable_set()()
 *   This is used by bsldnx_mgmt_init() to load the address of bslenable_set()
 *   into a static variable of DNX error/debug system (dnx_bslenable_set)
 *
 *   This is required since the 'severity' system for BSL is handled by DIAG.
 */
typedef void (
  *BSLENABLE_SET) (
  bsl_layer_t layer,
  bsl_source_t source,
  bsl_severity_t severity);
/*!
 * \brief Standard SDK error codes : shr_error_e
 *
 * \remark
 * IMPORTANT: These error codes are as per _shr_error_t (which both
 * bcm_error_t and soc_error_t use) and may be converted to a string by
 * passing the code to _shr_errmsg[]
 * Note that the value '0' (BCM_E_NONE) stands for 'success' and negative
 * values stand for 'fail'
 */
typedef _shr_error_t shr_error_e ;

/*!
 * \brief Check for successful return value.
 * 
 * \par DIRECT INPUT:
 *   \param [in] _expr -
 *      Error code to be checked (as per 'shr_error_e').
 * \par DIRECT OUTPUT:
 *   int (boolean) - \n
 *     Non-zero if input indicates 'no error'. Otherwise, input \n
 *     indicates a true error.
 */
#define SHR_SUCCESS(_expr) ((_expr) >= 0)

/*!
 * \brief Check for error return value.
 *
 * \par DIRECT INPUT:
 *   \param [in] _expr -
 *     Error code to be checked (as per 'shr_error_e').
 * \par DIRECT OUTPUT:
 *   int (boolean) - \n
 *     Non-zero if input is a true error. Otherwise, input \n
 *     indicates 'no error'
 */
#define SHR_FAILURE(_expr) ((_expr) < 0)

/*! ****************************************************
 * \brief
 *   Given an error code (per shr_error_e) get the corresponding
 *   human readable text description.
 *
 * \par DIRECT INPUT:
 *   \param [in] rv -
 *     Error code (as per 'shr_error_e').
 * \par INDIRECT INPUT:
 *   See macro 'bcm_errmsg'
 * \par DIRECT OUTPUT:
 *   const char * -\n
 *     Pointer to NULL terminated string of human readable\n
 *     text: very short description of input error.
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   None
 * \see
 *   shr_error_e
 *   bcm_error_e
 *****************************************************/
const char *shrextend_errmsg_get(
  int rv);

/*! ****************************************************
 * \brief
 *   Given address of procedure to get severities corresponding
 *   to printing of 'prefix' on BSL (See bsldnx_get_prefix_range_min_max()),
 *   set it in a local static variable (to be used as bsldnx_get_prefix_range_min_max())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Address of procedure to load into local static - get_prefix_range_min_max
 * \par INDIRECT INPUT:
 *   Name of 'get_prefix_range_min_max'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded 'get_prefix_range_min_max'
 * \remark
 *   None
 * \see
 *   get_prefix_range_min_max
 *****************************************************/
void set_proc_get_prefix_range_min_max(
  BSLDNX_GET_PREFIX_RANGE_MIN_MAX proc);
/*! ****************************************************
 * \brief
 *   Given address of procedure to get severities corresponding
 *   to printing of 'prefix' on BSL (See bsldnx_set_prefix_range_min_max()),
 *   set it in a local static variable (to be used as bsldnx_set_prefix_range_min_max())
 *
 * \par DATE:
 *   24/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Address of procedure to load into local static - set_prefix_range_min_max
 * \par INDIRECT INPUT:
 *   Name of 'set_prefix_range_min_max'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded 'set_prefix_range_min_max'
 * \remark
 *   None
 * \see
 *   set_prefix_range_min_max
 *****************************************************/
void set_proc_set_prefix_range_min_max(
  BSLDNX_SET_PREFIX_RANGE_MIN_MAX proc);

/*! ****************************************************
 * \brief
 *   Given pointer to a variable to contain address of procedure to get
 *   severities corresponding to printing of 'prefix' on BSL (See
 *   bsldnx_get_prefix_range_min_max()), load it from local static
 *   variable (to be used as bsldnx_get_prefix_range_min_max())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Load pointed memory by Address of procedure - get_prefix_range_min_max
 * \par INDIRECT INPUT:
 *   Name of 'get_prefix_range_min_max'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded *proc from 'get_prefix_range_min_max'
 * \remark
 *   None
 * \see
 *   get_prefix_range_min_max
 *****************************************************/
void get_proc_get_prefix_range_min_max(
  BSLDNX_GET_PREFIX_RANGE_MIN_MAX * proc);
/*! ****************************************************
 * \brief
 *   Given pointer to a variable to contain address of procedure to set
 *   severities corresponding to printing of 'prefix' on BSL (See
 *   bsldnx_set_prefix_range_min_max()), load it from local static
 *   variable (to be used as bsldnx_set_prefix_range_min_max())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Load pointed memory by Address of procedure - set_prefix_range_min_max
 * \par INDIRECT INPUT:
 *   Name of 'set_prefix_range_min_max'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded *proc from 'set_prefix_range_min_max'
 * \remark
 *   None
 * \see
 *   set_prefix_range_min_max
 *****************************************************/
void get_proc_set_prefix_range_min_max(
  BSLDNX_SET_PREFIX_RANGE_MIN_MAX * proc);

/*! ****************************************************
 * \brief
 *   Given address of procedure to get severity corresponding
 *   to printing of log on BSL (See bslenable_get()),
 *   set it in a local static variable (to be used as bslenable_get())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Address of procedure to load into local static - dnx_bslenable_get
 * \par INDIRECT INPUT:
 *   Name of 'dnx_bslenable_get'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded 'dnx_bslenable_get'
 * \remark
 *   None
 * \see
 *   dnx_bslenable_get
 *****************************************************/
void set_proc_bslenable_get(
  BSLENABLE_GET proc);

/*! ****************************************************
 * \brief
 *   Given address of procedure to set severity corresponding
 *   to printing of log on BSL (See bslenable_set()),
 *   set it in a local static variable (to be used as bslenable_set())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Address of procedure to load into local static - dnx_bslenable_set
 * \par INDIRECT INPUT:
 *   Name of 'dnx_bslenable_set'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded 'dnx_bslenable_set'
 * \remark
 *   None
 * \see
 *   dnx_bslenable_set
 *****************************************************/
void set_proc_bslenable_set(
  BSLENABLE_SET proc);

/*! ****************************************************
 * \brief
 *   Given pointer to a variable to contain address of procedure to
 *   get severity corresponding to printing of log on BSL (See
 *   bslenable_get()), load it from local static variable
 *   (to be used as bslenable_get())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Load pointed memory by Address of procedure - dnx_bslenable_get
 * \par INDIRECT INPUT:
 *   Name of 'dnx_bslenable_get'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded *proc from 'dnx_bslenable_get'
 * \remark
 *   None
 * \see
 *   dnx_bslenable_get
 *****************************************************/
void get_proc_bslenable_get(
  BSLENABLE_GET * proc);

/*! ****************************************************
 * \brief
 *   Given pointer to a variable to contain address of procedure to
 *   set severity corresponding to printing of log on BSL (See
 *   bslenable_set()), load it from local static variable
 *   (to be used as bslenable_set())
 *
 * \par DIRECT INPUT:
 *   \param [in] proc -
 *     Load pointed memory by Address of procedure - dnx_bslenable_set
 * \par INDIRECT INPUT:
 *   Name of 'dnx_bslenable_set'
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   * Loaded *proc from 'dnx_bslenable_set'
 * \remark
 *   None
 * \see
 *   dnx_bslenable_set
 *****************************************************/
void get_proc_bslenable_set(
  BSLENABLE_SET * proc);
/* } */
#endif /* !SHREXTEND_ERROR_H_INCLUDED */
