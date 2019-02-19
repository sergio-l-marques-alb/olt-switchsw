/*! \file shrextend_error.c
 *
 * Error message support.
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

#include <shared/shrextend/shrextend_error.h>
/*
 * Note:
 *   This file, bcm/error.h, is added here because JR2 uses the BCM API system
 *   of JR1 and the same error codes.
 */
#include <bcm/error.h>
/*
 * Pointers to procedures which are of the type of bsldnx_set_prefix_range_min_max()/
 * bsldnx_get_prefix_range_min_max().
 * These pointers are loaded at init and enable the DNX error-handling system to control
 * printing of prefix (file, line, function name, ...) on BSL log printout.
 * This is done using DIAG services. See bsldnx_mgmt_init().
 */
static BSLDNX_GET_PREFIX_RANGE_MIN_MAX get_prefix_range_min_max =
  (BSLDNX_GET_PREFIX_RANGE_MIN_MAX) 0;
static BSLDNX_SET_PREFIX_RANGE_MIN_MAX set_prefix_range_min_max =
  (BSLDNX_SET_PREFIX_RANGE_MIN_MAX) 0;
/*
 * Pointers to procedures which are of the type of bslenable_set()/bslenable_get().
 * These pointers are loaded at init and enable the DNX error-handling system to control
 * severity on BSL log printout.
 * This is done using DIAG services. See bsldnx_mgmt_init().
 */
static BSLENABLE_GET dnx_bslenable_get = (BSLENABLE_GET) 0;
static BSLENABLE_SET dnx_bslenable_set = (BSLENABLE_SET) 0;

void
set_proc_get_prefix_range_min_max(
  BSLDNX_GET_PREFIX_RANGE_MIN_MAX proc)
{
  get_prefix_range_min_max = proc;
  return;
}

void
set_proc_set_prefix_range_min_max(
  BSLDNX_SET_PREFIX_RANGE_MIN_MAX proc)
{
  set_prefix_range_min_max = proc;
  return;
}

void
get_proc_get_prefix_range_min_max(
  BSLDNX_GET_PREFIX_RANGE_MIN_MAX * proc)
{
  *proc = get_prefix_range_min_max;
  return;
}

void
get_proc_set_prefix_range_min_max(
  BSLDNX_SET_PREFIX_RANGE_MIN_MAX * proc)
{
  *proc = set_prefix_range_min_max;
  return;
}

void
set_proc_bslenable_get(
  BSLENABLE_GET proc)
{
  dnx_bslenable_get = proc;
  return;
}

void
set_proc_bslenable_set(
  BSLENABLE_SET proc)
{
  dnx_bslenable_set = proc;
  return;
}

void
get_proc_bslenable_get(
  BSLENABLE_GET * proc)
{
  *proc = dnx_bslenable_get;
  return;
}

void
get_proc_bslenable_set(
  BSLENABLE_SET * proc)
{
  *proc = dnx_bslenable_set;
  return;
}

const char *
shrextend_errmsg_get(
  shr_error_e rv)
{
  char *ret ;

  ret = _SHR_ERRMSG(rv) ;
  return (ret) ;
}

