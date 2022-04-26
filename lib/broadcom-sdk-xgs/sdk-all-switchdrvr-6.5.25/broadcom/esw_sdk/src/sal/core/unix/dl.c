/*! \file dl.c
 *
 * Dynamic loader API.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <dlfcn.h>
#include <stdio.h>

#include <sal/core/dl.h>

sal_dl_t
sal_dl_open(const char *libname)
{
    sal_dl_t rv;

    rv = dlopen(libname, RTLD_LAZY);
    if (rv == NULL) {
        printf("dllopen failed error = %s\n", dlerror());
    }
    return rv;
}

void *
sal_dl_sym(sal_dl_t dlh, const char *symname)
{
    return dlsym(dlh, symname);
}

int
sal_dl_close(sal_dl_t dlh)
{
    return dlclose(dlh);
}
