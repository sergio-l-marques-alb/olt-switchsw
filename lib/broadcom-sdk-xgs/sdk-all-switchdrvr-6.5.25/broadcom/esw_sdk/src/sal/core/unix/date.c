/*! \file date.c
 *
 * Date API.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <sal/core/date.h>

char *
sal_date(sal_date_t *date)
{
    char *ptr, *nl;
    time_t now = time(NULL);

    if (!date || sizeof(date->buf) < 26) {
        return SAL_DATE_NULL;
    }
    ptr = ctime_r(&now, date->buf);
    if (!ptr) {
        return SAL_DATE_NULL;
    }
    /* Remove newline if present */
    if ((nl = strchr(ptr, '\n')) != NULL) {
        *nl = '\0';
    }
    return ptr;
}
