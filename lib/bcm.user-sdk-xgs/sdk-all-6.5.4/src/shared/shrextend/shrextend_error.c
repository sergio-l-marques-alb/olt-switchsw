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

/*!
 * \brief Standard SDK error message.
 *
 * IMPORTANT: These error messages MUST match the corresponding error
 * codes in shrextend_error.h.
 */
static char *errmsg[] =
{
  "Ok",                               /* E_NONE */
  "Internal error",                   /* E_INTERNAL */
  "Out of memory",                    /* E_MEMORY */
  "Invalid unit",                     /* E_UNIT */
  "Invalid parameter",                /* E_PARAM */
  "Table empty",                      /* E_EMPTY */
  "Table full",                       /* E_FULL */
  "Entry not found",                  /* E_NOT_FOUND */
  "Entry exists",                     /* E_EXISTS */
  "Operation timed out",              /* E_TIMEOUT */
  "Operation still running",          /* E_BUSY */
  "Operation failed",                 /* E_FAIL */
  "Operation disabled",               /* E_DISABLED */
  "Invalid identifier",               /* E_BADID */
  "No resources for operation",       /* E_RESOURCE */
  "Invalid configuration",            /* E_CONFIG */
  "Feature unavailable",              /* E_UNAVAIL */
  "Feature not initialized",          /* E_INIT */
  "Invalid port",                     /* E_PORT */
  "Not supported",                    /* E_NOT_SUPPORTED */ 
  "Access error",                     /* E_ACCESS */
  "No handler",                       /* E_NO_HANDLER */

  "Unknown error"                     /* E_LIMIT */
} ;

const char
  *shrextend_errmsg_get(int rv)
{
  int idx ;

  idx = -SHR_E_LIMIT ;
  if ((rv <= 0) && (rv > SHR_E_LIMIT))
  {
      idx = -rv ;
  }
  return errmsg[idx] ;
}
