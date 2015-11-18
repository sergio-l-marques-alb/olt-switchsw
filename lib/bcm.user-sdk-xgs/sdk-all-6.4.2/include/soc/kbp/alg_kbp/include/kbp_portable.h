/*
 **************************************************************************************
 Copyright 2009-2014 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#ifndef __KBP_PORTABLE_H
#define __KBP_PORTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @file kbp_portable.h
 *
 * Portability-related APIs. These standard implementations for Linux-based and
 * Windows-based systems are available as part of the respective packages. Customers
 * are expected to implement the functions below on any other propriety
 * Operating Systems
 *
 * @addtogroup PORTABILITY_API
 * @{
 */

/**
 * Portability structure for timers
 */
struct kbp_timeval {
    long tv_sec;                /**< Seconds */
    long tv_ns;                 /**< Nanoseconds */
};

#define NLM_CAST_PTR_TO_NLM_U32(x) ((uint32_t)(uintptr_t)x)
#define NLM_CAST_NLM_U32_TO_PTR(x) (void*)((uintptr_t)x)

#ifdef WIN32
#define kbp_concat_identity(a)  a
#define kbp_concat_2(a,b)  kbp_concat_identity(a)kbp_concat_identity(b)
#define kbp_concat_3(a,b,c)    kbp_concat_identity(a)kbp_concat_identity(b)kbp_concat_identity(c)
#else
#define kbp_concat_2(a,b)      a##b
#define kbp_concat_3(a,b,c)    a##b##c
#endif

#define kbp_stdout   stdout
#define kbp_stderr   stderr
#define kbp_fopen    fopen
#define kbp_eof      (EOF)

/**
 * Implementation of memset.
 */

void *kbp_memset(void *str, int val, uint32_t size);

/**
 * Implementation of memcpy.
 */
void *kbp_memcpy(void *dest, const void *src, uint32_t size);

/**
 * Implementation of memcmp.
 */
int kbp_memcmp(const void *s1, const void *s2, uint32_t size);

/**
 * Implementation of memmove.
 */
void *kbp_memmove(void *dstp, const void *srcp, uint32_t numOfBytes);

/**
 * Implementation of malloc.
 */
void *kbp_sysmalloc(uint32_t size);

/**
 * Implementation of calloc.
 */
void *kbp_syscalloc(uint32_t nmemblk, uint32_t size);

/**
 * Implementation of free.
 */
void kbp_sysfree(void *ptr);

/**
 * Implementation of abort.
 */
void kbp_pvt_abort(void);

/**
 * Implementation of sprintf.
 */
int kbp_sprintf(char *buff, const char *fmt,...);

/**
 * Implementation of snprintf.
 */
int kbp_snprintf(char *buff, uint32_t buffsize, const char *fmt,...);

/**
 * Implementation of printf.
 */
int kbp_printf(const char *fmt,...);

/**
 * Implementation of fputs
 */
int kbp_fputs(const char *str, FILE *fp);

/**
 * Implementation of vprintf.
 */
int kbp_vprintf(const char *fmt, va_list arg);

/**
 * Implementation of fprintf.
 */
int kbp_fprintf(FILE * fp, const char *fmt, ...);

/**
 * Implementation of fscanf.
 */
int kbp_fscanf(FILE * fp,const char *fmt, ...);

/**
 * Implementation of fseek.
 */
int kbp_fseek(FILE * fp, uint32_t offset ,int whence);

/**
 * Windows implementation of error box.
 */

void kbp_enable_error_box(int32_t enable);

/**
 * Implementation of gettimeofday.
 */
int kbp_gettimeofday(struct kbp_timeval *tv);

/**
 * Implementation of gettimerval.
 */
int kbp_gettimerval(struct kbp_timeval * tv);

/**
 * Implementation of getenv.
 */
char *kbp_getenv(const char *name);

/**
 * Implementation of srandom.
 */
void kbp_srandom(uint32_t seed);

/**
 * Implementation of random.
 */
int kbp_random(void);

/**
 * Implementation of random_r.
 */
int kbp_random_r(uint32_t *seedp);


/**
 * Returns an array of files that end with the provided
 * file extension (suffix). If suffix is NULL, all files irrespective
 * of suffix are returned. Its the responsibility
 * of the caller to free up the returned array.
 *
 * @param location File or directory to start searching from.
 * @param suffix The file extension or NULL for all files.
 * @param num_found Number of elements in the returned array.
 *
 * @retval Array of file names with full paths, or NULL.
 */

char **kbp_get_files_recursive(const char *location, const char *suffix, uint32_t *num_found);

/**
 * Creates a directory structure if needed, opens the
 * specified file, and returns a file pointer. For example,
 * if /foo/bar/a.txt is passed in, and directories foo/bar
 * do not exist, it will create them.
 */

FILE *kbp_open_file(const char *fname);

/**
 * Portable Unix implementation of strsep() function.
 */

char *kbp_strsep(char **stringp, const char *delim);

/**
 * Implementation of fclose
 */

int kbp_fclose(FILE * fp);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif
#endif                          /* __KBP_PORTABLE_H */
