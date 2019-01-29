/*  $OpenBSD: xmalloc.h,v 1.9 2002/06/19 00:27:55 deraadt Exp $ */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Created: Mon Mar 20 22:09:17 1995 ylo
 *
 * Versions of malloc and friends that check their results, and never return
 * failure (they call fatal if they encounter an error).
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef XMALLOC_H
#define XMALLOC_H

#define xmalloc(size) L7xmalloc(size, __FILE__, __LINE__)
#define xrealloc(memory, size) L7xrealloc(memory, size, __FILE__, __LINE__)
#define xfree(memory) L7xfree(memory, __FILE__, __LINE__)
#define xstrdup(stringToDup) L7xstrdup(stringToDup, __FILE__, __LINE__)

void    *L7xmalloc(size_t, char *, int);
void    *L7xrealloc(void *, size_t, char *, int);
void     L7xfree(void *, char *, int);
char    *L7xstrdup(const char *, char *, int);

#endif              /* XMALLOC_H */
