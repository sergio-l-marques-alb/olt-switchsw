#if !defined(_SB_WRAPPERS_H_)
#define _SB_WRAPPERS_H_
/**
 * @file sbWrappers.h
 *
 * <pre>
 * ==========================================================
 * == sbWrappers.h - Wrapper file for commonly used macros ==
 * ==========================================================
 *
 * WORKING REVISION: $Id: sbWrappers.h,v 1.6 Broadcom SDK $
 *
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
 *
 *
 * MODULE NAME:
 *
 *     sbWrappers.h
 *
 * ABSTRACT:
 *
 *     Commonly used macros, wrapped for checked build vs. production builds
 *
 * LANGUAGE:
 *
 *     C
 *
 * AUTHORS:
 *
 *     Travis B. Sawyer
 *
 * CREATION DATE:
 *
 *     12-July-2005
 *
 * </pre>
 */

#if defined(CHECK_BUILD)
#ifndef VXWORKS
#include <assert.h>
#ifndef __KERNEL__
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#endif /* ! KERNEL */
#else
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#endif /* !VXWORKS */

#include <soc/drv.h>
#include <soc/debug.h>

#if !defined(SB_ZF_INCLUDE_CONSOLE)
#define SB_ZF_INCLUDE_CONSOLE
#endif

#define SB_ASSERT(__X)  assert(__X)

/*
#define SB_MEMSET(dest, pattern, size) memset(dest, pattern, size)
#define SB_MEMCPY(dest, src, size)     memcpy(dest, src, size)
#define SB_MEMCMP(dest, src, size)     memcmp(dest, src, size)
#define SB_STRCPY(str1, str2)          strcpy(str1, str2)
#define SB_STRNCPY(str1, str2, maxLen) strncpy(str1, str2, maxLen)
#define SB_STRCAT(str, ct)             strcat(str, ct)
#define SB_STRNCAT(str, ct, num)       strncat(str, ct, num)
#define SB_STRCMP(str1, str2)          strcmp(str1, str2)
#define SB_STRNCMP(str1, str2, maxLen) strncmp(str1, str2, maxLen)
#define SB_STRLEN(str)                 strlen(str)
#define SB_STRERROR(num)               strerror(num)
#define SB_STRTOK(str, ct)             strtok(str, ct)
*/

#define SB_MEMSET(dest, pattern, size) sal_memset(dest, pattern, size)
#define SB_MEMCPY(dest, src, size)     sal_memcpy(dest, src, size)
#define SB_MEMCMP(dest, src, size)     sal_memcmp(dest, src, size)
#define SB_STRCPY(str1, str2)          sal_strcpy(str1, str2)
#define SB_STRNCPY(str1, str2, maxLen) sal_strncpy(str1, str2, maxLen)
#define SB_STRCAT(str, ct)             strcat(str, ct)
#define SB_STRNCAT(str, ct, num)       sal_strncat(str, ct, num)
#define SB_STRCMP(str1, str2)          sal_strcmp(str1, str2)
#define SB_STRNCMP(str1, str2, maxLen) sal_strncmp(str1, str2, maxLen)
#define SB_STRLEN(str)                 sal_strlen(str)
#define SB_STRERROR(num)               sal_strerror(num)
#define SB_STRTOK(str, ct)             sal_strtok(str, ct)
#define SB_SPRINTF                     sal_sprintf
#define SB_STRSTR                      strstr

#else /* !defined(CHECK_BUILD) */
#define SB_ASSERT(__X)
#define SB_MEMSET(dest, pattern, size) memset(dest, pattern, size)
#define SB_MEMCPY(dest, src, size)     memcpy(dest, src, size)
#define SB_MEMCMP(dest, src, size)     memcmp(dest, src, size)
#define SB_STRCPY(str1, str2)          strcpy(str1, str2)
#define SB_STRNCPY(str1, str2, maxLen) strncpy(str1, str2, maxLen)
#define SB_STRCAT(str, ct)             strcat(str, ct)
#define SB_STRNCAT(str, ct, num)       strncat(str, ct, num)
#define SB_STRCMP(str1, str2)          strcmp(str1, str2)
#define SB_STRNCMP(str1, str2, maxLen) strncmp(str1, str2, maxLen)
#define SB_STRLEN(str)                 strlen(str)
#define SB_STRERROR(num)               strerror(num)
#define SB_STRTOK(str, ct)             strtok(str, ct)

#endif /* defined(CHECK_BUILD) */
#endif /* _SB_WRAPPERS_H_ */
