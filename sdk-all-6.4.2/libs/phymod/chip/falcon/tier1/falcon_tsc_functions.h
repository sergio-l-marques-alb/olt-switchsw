/***********************************************************************************/
/***********************************************************************************/
/*  File Name     :  falcon_api_functions.h                                        */
/*  Created On    :  29/04/2013                                                    */
/*  Created By    :  Kiran Divakar                                                 */
/*  Description   :  Header file with API functions for Serdes IPs                 */
/*  Revision      :  $Id: falcon_api_functions.h 650 2014-08-04 21:16:18Z jgaither $ */
/*                                                                                 */
/*  All Rights Reserved                                                            */
/*  No portions of this material may be reproduced in any form without             */
/*  the written permission of:                                                     */
/*      Broadcom Corporation                                                       */
/*      5300 California Avenue                                                     */
/*      Irvine, CA  92617                                                          */
/*                                                                                 */
/*  All information contained in this document is Broadcom Corporation             */
/*  company private proprietary, and trade secret.                                 */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/
/*
* $Copyright: Copyright 2012 Broadcom Corporation.
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

/** @file falcon_api_functions.h
 * Protoypes of all API functions for engineering use 
 */

#ifndef SERDES_API_FUNCTIONS_H
#define SERDES_API_FUNCTIONS_H

/* include all .h files, even though some are redundant */

#include "falcon_tsc_usr_includes.h"

#include "falcon_tsc_ipconfig.h"
#include "falcon_tsc_dependencies.h"
#include "falcon_tsc_interface.h"
#include "falcon_tsc_debug_functions.h"
#include "falcon_tsc_common.h"
#ifdef FALCON
#include "falcon_api_uc_common.h"
#endif
#if defined(EAGLE)||defined(MERLIN)
#include "eagle_merlin_api_uc_common.h"
#endif
#include "falcon_tsc_common_uc_common.h"

#include "falcon_tsc_field_access.h"
#include "falcon_tsc_enum.h"
#include "falcon_tsc_common_err_code.h"
#include "falcon_tsc_field_access.h"
#include "falcon_tsc_internal.h"

#ifdef SERDES_EVAL
#if defined _MSC_VER
#define FUNCTION_NAME __FUNCTION__
#else
#define FUNCTION_NAME __func__
#endif

uint16_t _api_error_fun(uint16_t err_code, const char *fname, const char *fun, const int line);

#define _error(err_code) _api_error_fun(err_code,__FILE__,FUNCTION_NAME,__LINE__)

#else
#define _error(args) args
#endif


/* declare __ERR macro for register read and RAM variable read access macros */
#if defined(SERDES_EVAL) && (defined(EAGLE_MERLIN_CORE_H_) || defined(FALCON_CORE_H_) || defined(SERDES_ACCESS_H_))
extern err_code_t global_err_code;
#define __ERR &global_err_code
#else
#define __ERR &__err
#endif

/* RAM access */
#ifdef FALCON
#include "falcon_api_uc_vars_rdwr_defns.h"
#endif
#if defined(EAGLE)||defined(MERLIN)
#include "eagle_merlin_api_uc_vars_rdwr_defns.h"
#endif

/* Reg access */
#ifdef EAGLE
   #include "eagle_tsc_fields.h"
#endif 

#ifdef MERLIN
   #ifdef MERLIN_GALLARDO  
      #include "merlin_gallardo_fields.h"
   #endif
   #ifdef MERLIN_KOS  
      #include "merlin_kos_fields.h"
   #endif
   #ifdef MERLIN_KOI  
      #include "merlin_koi_fields.h"
   #endif
   #ifdef MERLIN_QUADRA28  
      #include "merlin_quadra28_fields.h"
   #endif
   #ifdef MERLIN_SESTO  
      #include "merlin_sesto_fields.h"
   #endif
   #ifdef MERLIN_COUNTACH  
      #include "merlin_countach_fields.h"
   #endif
#endif

#ifdef FALCON
  #ifdef FALCON_TSC
      #include "falcon_tsc_fields.h"
   #endif
  #ifdef FALCON_FURIA
      #include "falcon_furia_fields.h"
   #endif
#endif

#ifdef FALCON
#define DIAG_MAX_SAMPLES (64)
#else
#define DIAG_MAX_SAMPLES (32)
#endif

#ifdef SERDES_DEFAULT_MICRO
#ifdef FALCON
#define CORE_VAR_RAM_BASE (0x400)
#define LANE_VAR_RAM_BASE (0x420)
#define LANE_VAR_RAM_SIZE (0x130)
#else
#define CORE_VAR_RAM_BASE (0x50)
#define LANE_VAR_RAM_BASE (0x400)
#define LANE_VAR_RAM_SIZE (0x100)
#endif
#endif

/**********************************************/
/* macros to simplify checking for error_code */
/**********************************************/

/* call this to invoke a function with automatic return of error */
#define EFUN(fun) do {err_code_t __err = (fun); if (__err) return(__err); } while(0)

/* call this to execute a statement with automatic return of error */
#define ESTM(statement) do {err_code_t __err = ERR_CODE_NONE; statement; if (__err) return(__err); } while(0)

/* call this to invoke a function with automatic return of error pointer */
#define EPFUN(fun) do { *err_code_p |= (fun); if (*err_code_p) return(0); } while(0)

/* call this to execute a statement with automatic return of error pointer */
#define EPSTM(statement) do {err_code_t __err = ERR_CODE_NONE; statement; *err_code_p |= __err; if (*err_code_p) return(0); } while(0)

/*----------------------*/
/*  Misc Useful macros  */
/*----------------------*/
/** Macro to display signed integer variable */
#define DISP(x) ESTM(USR_PRINTF(("%s = %d\n",#x,x)))

/** Macro to display unsigned integer variable */
#define DISPU(x) ESTM(USR_PRINTF(("%s = %u\n",#x,x)))

/** Macro to display floating point variable */
#define DISPF(x) ESTM(USR_PRINTF(("%s = %f\n",#x,x)))

/** Macro to display integer variable in hex */
#define DISPX(x) ESTM(USR_PRINTF(("%s = %x\n",#x,x)))

/** Macro to read and display value of register field */
#define DISP_REG(x) ESTM(USR_PRINTF(("%s = %d\n",#x,rd_ ## x ## ())))
#define DISP_REGC(x) ESTM(USR_PRINTF(("%s = %x\n",#x,rdc_ ## x ## ())))

#define dist_cw(a,b) (((a)<=(b))?((b)-(a)):((uint16_t)256-(a)+(b)))
#define dist_ccw(a,b) (((a)>=(b))?((a)-(b)):((uint16_t)256+(a)-(b)))
#define DISP_LN_VARS(name,param,format) USR_PRINTF(("%-16s\t",name)); for(i=0;i<num_lanes;i++) {  USR_PRINTF((format,(lane_st[i].param))); } USR_PRINTF(("\n")); 
#define DISP_LNQ_VARS(name,param1,param2,format) USR_PRINTF(("%-16s\t",name));for(i=0;i<num_lanes;i++) {  USR_PRINTF((format,(lane_st[i].param1),(lane_st[i].param2)));} USR_PRINTF(("\n")); 


#define _min(a,b) (((a)>(b)) ? (b) : (a))
#define _max(a,b) (((a)>(b)) ? (a) : (b))
#define _abs(a) (((a)>0) ? (a) : (-a))

#endif
