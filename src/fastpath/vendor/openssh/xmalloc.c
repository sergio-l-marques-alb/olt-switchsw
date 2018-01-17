/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Versions of malloc and friends that check their results, and never return
 * failure (they call fatal if they encounter an error).
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: xmalloc.c,v 1.16 2001/07/23 18:21:46 stevesk Exp $");

#include "xmalloc.h"
#include "log.h"

#include "commdefs.h"
#include "osapi.h"
#include "l7utils_api.h"

void *
L7xmalloc(size_t size, char *file, int line)
{
    void *ptr;

    l7utilsFilenameStrip(&file);

    if (size == 0)
    {
        fatal("L7xmalloc: zero size requested, file %s, line %d", file, line);
    }

    debugl7(SYSLOG_LEVEL_MALLOC, "L7xmalloc: attempting malloc of size: %d, file %s, line %d", size, file, line);

    ptr = osapiMalloc_track(size, L7_FLEX_SSHD_COMPONENT_ID, file, line);

    if (ptr != NULL)
    {
        debugl7(SYSLOG_LEVEL_MALLOC, "L7xmalloc: location: %p, size: %d, file %s, line %d", ptr, size, file, line);
    }
    else
    {
        fatal("L7xmalloc: out of memory, location: %p, size: %d, file %s, line %d", ptr, size, file, line);
    }

    return ptr;
}

void *
L7xrealloc(void *ptr, size_t new_size, char *file, int line)
{
    void *new_ptr = NULL;

    l7utilsFilenameStrip(&file);

    if (new_size == 0)
    {
        fatal("L7xrealloc: zero size requested, file %s, line %d", file, line);
    }

    if (ptr == NULL)
    {
        new_ptr = osapiMalloc_track(new_size, L7_FLEX_SSHD_COMPONENT_ID, file, line);
    }
    else
    {
        debugl7(SYSLOG_LEVEL_MALLOC, "L7xrealloc: original location: %p, file %s, line %d", ptr, file, line);

        new_ptr = osapiRealloc_track(ptr, new_size, L7_FLEX_SSHD_COMPONENT_ID, file, line);
    }

    if (new_ptr != NULL)
    {
        debugl7(SYSLOG_LEVEL_MALLOC, "L7xrealloc: location: %p, size: %d, file %s, line %d", new_ptr, new_size, file, line);
    }
    else
    {
        fatal("L7xrealloc: out of memory, location: %p, size: %d, file %s, line %d", new_ptr, new_size, file, line);
    }

    return new_ptr;
}

void
L7xfree(void *ptr, char *file, int line)
{
    l7utilsFilenameStrip(&file);

    if (ptr == NULL)
    {
        fatal("L7xfree: freeing NULL, file %s, line %d ", file, line);
    }
    else
    {
        debugl7(SYSLOG_LEVEL_MALLOC, "L7xfree: location: %p, file %s, line %d", ptr, file, line);
        osapiFree(L7_FLEX_SSHD_COMPONENT_ID, ptr);
    }
}

char *
L7xstrdup(const char *str, char *file, int line)
{
    size_t len;
    char *cp;

    len = strlen(str) + 1;
    cp = L7xmalloc(len, file, line);
    strlcpy(cp, str, len);
    return cp;
}
