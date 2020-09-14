/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

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

#define _FILE_OFFSET_BITS 64

#include "kaps_portable.h"
#include "sal/core/alloc.h"
#include "sal/core/libc.h"
#include "sal/core/sync.h"
#include "sal/appl/sal.h"
#include "sal/appl/io.h"
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <sys/mman.h>
#include <fcntl.h>

struct kaps_pthread_mutex
{
    sal_mutex_t mutex;
};

int
kaps_gettimeofday(
    struct kaps_timeval *tv)
{
    struct timeval t;
    int retval;

    retval = gettimeofday(&t, 0);

    tv->tv_sec = t.tv_sec;
    tv->tv_ns = t.tv_usec * 1000;

    return retval;
}

int
kaps_gettimerval(
    struct kaps_timeval *tv)
{
    struct timeval t;
    int retval;

    retval = gettimeofday(&t, 0);

    tv->tv_sec = t.tv_sec;
    tv->tv_ns = t.tv_usec * 1000;

    return retval;
}

kaps_time_t
kaps_time(
    kaps_time_t * time_in_sec)
{
    time_t rawtime = time(NULL);
    kaps_time_t ktime = (kaps_time_t) rawtime;

    if (time_in_sec)
        *time_in_sec = ktime;

    return ktime;
}

struct kaps_tm *
kaps_localtime(
    kaps_time_t * time_in_sec,
    struct kaps_tm *info)
{
    time_t rawtime = (time_t) (*time_in_sec);
    struct tm timeinfo;

    localtime_r(&rawtime, &timeinfo);
    kaps_memcpy(info, &timeinfo, sizeof(struct kaps_tm));

    return info;
}

void *
kaps_memset(
    void *str,
    int32_t val,
    uint32_t size)
{
    sal_memset(str, val, size);
    return str;
}

void *
kaps_memcpy(
    void *dest,
    const void *src,
    uint32_t size)
{
    sal_memcpy(dest, src, size);
    return dest;
}

int
kaps_memcmp(
    const void *s1,
    const void *s2,
    uint32_t size)
{
    int ret = 0;
    ret = sal_memcmp(s1, s2, size);
    return ret;
}

void *
kaps_sysmalloc(
    uint32_t size)
{
    void *ptr = NULL;
    ptr = sal_alloc((size), __FILE__);
    return ptr;
}

void *
kaps_syscalloc(
    uint32_t nmemblk,
    uint32_t size)
{
    void *ptr = NULL;
    ptr = sal_alloc((nmemblk * size), __FILE__);
    kaps_memset(ptr, 0, (nmemblk * size));
    return ptr;
}

void
kaps_sysfree(
    void *ptr)
{
    sal_free(ptr);
}

void
kaps_abort(
    )
{
    abort();
}

int
kaps_vsnprintf(
    char *buff,
    uint32_t buffsize,
    const char *fmt,
    va_list ap)
{
    return sal_vsnprintf(buff, buffsize, fmt, ap);
}

int
kaps_sprintf(
    char *buff,
    const char *fmt,
    ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsprintf(buff, fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_snprintf(
    char *buff,
    uint32_t buffsize,
    const char *fmt,
    ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsnprintf(buff, buffsize, fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_printf(
    const char *fmt,
    ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vprintf(fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_fputs(
    const char *str,
    FILE * fp)
{
    int r;

    r = sal_fprintf(fp, str);
    return r;
}

int
kaps_vprintf(
    const char *fmt,
    va_list ap)
{
    return sal_vprintf(fmt, ap);
}

int
kaps_vfprintf(
    FILE * fp,
    const char *fmt,
    va_list ap)
{
    if (!fp)
        return 0;

    return sal_vfprintf(fp, fmt, ap);
}

int
kaps_fprintf(
    FILE * fp,
    const char *fmt,
    ...)
{
    int r;
    va_list ap;

    if (!fp)
        return 0;

    va_start(ap, fmt);
    r = sal_vfprintf(fp, fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_fscanf(
    FILE * fp,
    const char *fmt,
    ...)
{
    va_list ap;
    void *arg;
    char *str = strdup(fmt);
    char *token;
    int r = 0, c = 0;

    if (!fp)
        goto error;

    va_start(ap, fmt);
    for (token = kaps_strsep(&str, " "); token; token = kaps_strsep(&str, " "))
    {
        if (strstr(token, "%"))
        {
            char *tkn = kaps_sysmalloc(strlen(token) + 1);
            strcpy(tkn, token);
            tkn[strlen(token)] = ' ';
            arg = va_arg(ap, void *);
            c = fscanf(fp, tkn, arg);
            kaps_sysfree(tkn);

            if (c == EOF)
            {
                r = EOF;
                va_end(ap);
                kaps_sysfree(str);
                return r;
            }
            r++;
        }
    }
    va_end(ap);
error:
    kaps_sysfree(str);
    return r;
}

void *
kaps_memmove(
    void *dst,
    const void *src,
    uint32_t size)
{
    uint8_t *a = dst;
    const uint8_t *b = src;

    if (a <= b || a >= (b + size))
    {
        while (size--)
            *a++ = *b++;
    }
    else
    {
        a = a + size - 1;
        b = b + size - 1;

        while (size--)
            *a-- = *b--;
    }

    return dst;
}

void
kaps_enable_error_box(
    int32_t enable)
{
    (void) enable;
}

void
kaps_srandom(
    uint32_t seed)
{
    srand(seed);
}

int
kaps_random_r(
    uint32_t * seedp)
{
    return rand_r(seedp);
}

char *
kaps_strsep(
    char **stringp,
    const char *delim)
{
    return strsep(stringp, delim);
}

int
kaps_fseek(
    FILE * fp,
    int32_t offset,
    int whence)
{
    return fseek(fp, offset, whence);
}

int
kaps_fclose(
    FILE * fp)
{
    return fclose(fp);
}

void
kaps_usleep(
    uint32_t usecs)
{
    sal_usleep(usecs);
}

void *
kaps_mmap(
    void *addr,
    uint32_t length,
    int prot,
    int flags,
    int fd,
    uint64_t offset)
{
    return mmap(addr, length, prot, flags, fd, offset);
}

int
kaps_munmap(
    void *addr,
    uint32_t length)
{
    return munmap(addr, length);
}

int
kaps_open(
    const char *pathname,
    int flags,
    uint32_t mode)
{
    return open(pathname, flags, mode);
}

int32_t
kaps_identity(
    const int32_t b)
{
    return b;
}

int32_t
kaps_assert_detail(
    const char *msg,
    const char *file,
    int32_t line)
{
    kaps_printf("ERROR %s:%d: %s\n", file, line, msg);
    kaps_abort();
    return 0;
}

int32_t
kaps_assert_detail_or_error(
    const char *msg,
    uint32_t return_error,
    uint32_t error_code,
    const char *file,
    int32_t line)
{
    kaps_printf("ERROR %s:%d: %s\n", file, line, msg);
    if (!return_error)
        kaps_abort();
    return error_code;
}

int
kaps_mutex_init(
    struct kaps_mutex **mutex)
{
    struct kaps_pthread_mutex *pmutex = NULL;
    int ret_val = 0;

    if (mutex == NULL)
        return EINVAL;

    pmutex = kaps_syscalloc(1, sizeof(struct kaps_pthread_mutex));
    if (pmutex == NULL)
        return ENOMEM;

    pmutex->mutex = sal_mutex_create("KBP Mutex");
    if (pmutex->mutex)
    {
        *mutex = (struct kaps_mutex *) pmutex;
    }
    else
    {
        /*
         * What to return? Only value returned by sal_mutex_create is NULL 
         */
        kaps_sysfree(pmutex);
        ret_val = ENOMEM;
    }

    return ret_val;
}

int
kaps_mutex_destroy(
    struct kaps_mutex *mutex)
{
    struct kaps_pthread_mutex *pmutex;
    int ret_val = 0;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_pthread_mutex *) mutex;

    /*
     * This function does not return any value 
     */
    sal_mutex_destroy(pmutex->mutex);
    kaps_sysfree(pmutex);

    return ret_val;
}

int
kaps_mutex_lock(
    struct kaps_mutex *mutex)
{
    struct kaps_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_pthread_mutex *) mutex;
    return sal_mutex_take(pmutex->mutex, sal_mutex_FOREVER);
}

int
kaps_mutex_trylock(
    struct kaps_mutex *mutex)
{
    struct kaps_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_pthread_mutex *) mutex;
    return sal_mutex_take(pmutex->mutex, sal_mutex_NOWAIT);
}

int
kaps_mutex_unlock(
    struct kaps_mutex *mutex)
{
    struct kaps_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_pthread_mutex *) mutex;
    return sal_mutex_give(pmutex->mutex);

}
 /**/
