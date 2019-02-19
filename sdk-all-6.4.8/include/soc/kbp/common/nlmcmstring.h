/*
 * $Id: nlmcmstring.h,v 1.2.8.2 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


 
/*@@NlmCmString Module

   Summary
   Implementation for string and converion functions.
 
   Description
   All the string manipulation functions are implemented by
   this module. Users should not be using provided calls for
   string manipulations.

 */

#ifndef INCLUDED_NLMCMSTRING_H
#define INCLUDED_NLMCMSTRING_H

#ifndef NLMPLATFORM_BCM
#include <nlmcmbasic.h>
#include <nlmcmallocator.h>
#else
#include <soc/kbp/common/nlmcmbasic.h>
#include <soc/kbp/common/nlmcmallocator.h>
#endif

#ifndef __doxygen__

/**************************************************************************
 * declare in-house implementations
 **************************************************************************/
#ifndef NLMPLATFORM_BCM
#include <nlmcmexterncstart.h> 
#else
#include <soc/kbp/common/nlmcmexterncstart.h>
#endif

/* strlwr */
extern char*
NlmCm__strlwr(
    char* str) ;

/* strupr */
extern char*
NlmCm__strupr(
    char* str) ;

/* strcasecmp */
extern int
NlmCm__strcasecmp(
    const char* str1,
    const char* str2) ;

/* strncasecmp */
extern int
NlmCm__strncasecmp(
    const char* str1,
    const char* str2i,
    size_t n) ;

/* strdup */
extern char*
NlmCm__strdup(
    NlmCmAllocator* alloc,
    const char* str) ;

/* Hashes a string using PJW Hash (Aho, Sethi, and Ullman pp. 434-438) */
extern nlm_u32
NlmCm__strhash(
    const char *str) ;

/* 
 * Convert an ASCII string to an nlm_32
 *
 * Details
 * Behaves as atoi, except doesn't use errno, instead passes back a NlmBool.
 *
 * Return
 * NlmFalse if the number found within the string is outside of the range
 * of an nlm_32. NlmTrue otherwise, even if there was no number to convert.
 *
 */
extern NlmBool
NlmCm__atoi(const char* str,
         nlm_32*    o_num);



/*
 * Compare strings
 */


extern int
NlmCm__strcmp(const char* s1, 
    const char* s2) ;

/* Compare set of characters in string. 
 */

extern int
NlmCm__strncmp(const char* s1, 
    const char* s2, 
    size_t n) ;

/* Find a character in a string.
 */

extern char* 
NlmCm__strchr(const char* p,
    int ch) ;


/* Find substring-find in string s
 */

extern char* 
NlmCm__strstr(const char* s,
    const char* find) ;


/* Find string length 
 */
extern size_t 
NlmCm__strlen(const char* str) ;

/* String copy- copies "from" to "to" and returns "to". 
 */
extern char* 
NlmCm__strcpy(char* to, 
    const char* from) ;

/* String copy characters - copies at most n chars from src to dst,
 * returns dst
 */
extern char* 
NlmCm__strncpy(char* dst,
    const char* src,
    size_t n) ;

/* String concatenation: appends 'count' chars from "append" to "s"
 * and return s.
 */
char*
NlmCm__strncat(char *to,
               const char* append,
               size_t count) ;

/* String concatenation , appends "append" to "s"
 * and return s.
 */
extern char*
NlmCm__strcat(char* s,
        const char* append) ;

/**************************************************************************
 * unit test module 
 **************************************************************************/

extern void
NlmCmString__Verify(void);

#ifndef NLMPLATFORM_BCM
#include <nlmcmexterncend.h>
#else
#include <soc/kbp/common/nlmcmexterncend.h>
#endif

#endif /* __doxygen__ */

#endif /* INCLUDED_NLMCMSTRING_H */

