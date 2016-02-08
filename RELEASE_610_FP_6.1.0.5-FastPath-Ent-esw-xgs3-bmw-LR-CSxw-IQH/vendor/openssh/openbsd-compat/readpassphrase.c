/*  $OpenBSD: readpassphrase.c,v 1.18 2005/08/08 08:05:34 espie Exp $   */

/*
 * Copyright (c) 2000-2002 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

/* OPENBSD ORIGINAL: lib/libc/gen/readpassphrase.c */

#include "includes.h"

#ifndef HAVE_READPASSPHRASE

#ifndef L7_SSHD
#include <termios.h>
#include <readpassphrase.h>
#else
#include "readpassphrase.h"
#endif /* L7_SSHD */

#ifdef TCSASOFT
# define _T_FLUSH   (TCSAFLUSH|TCSASOFT)
#else
# define _T_FLUSH   (TCSAFLUSH)
#endif

/* SunOS 4.x which lacks _POSIX_VDISABLE, but has VDISABLE */
#if !defined(_POSIX_VDISABLE) && defined(VDISABLE)
#  define _POSIX_VDISABLE       VDISABLE
#endif

static volatile sig_atomic_t signo;

static void handler(int);

char *
readpassphrase(const char *prompt, char *buf, size_t bufsiz, int flags)
{
    ssize_t nr;
    int input, output, save_errno;
    char ch, *p, *end;
#ifndef L7_SSHD
    //struct termios term, oterm;
#endif /* L7_SSHD */
    struct sigaction sa, savealrm, saveint, savehup, savequit, saveterm;
    struct sigaction savetstp, savettin, savettou, savepipe;

    /* I suppose we could alloc on demand in this case (XXX). */
    if (bufsiz == 0) {
        errno = EINVAL;
        return(NULL);
    }

#ifndef L7_SSHD
restart:
#endif /* L7_SSHD */
    signo = 0;
    /*
     * Read and write to /dev/tty if available.  If not, read from
     * stdin and write to stderr unless a tty is required.
     */
    if ((flags & RPP_STDIN) ||
#ifndef L7_SSHD
        (input = output = open(_PATH_TTY, O_RDWR)) == -1) {
#else
        (input = output = osapiFsOpen(_PATH_TTY)) == -1) {
#endif /* L7_SSHD */
        if (flags & RPP_REQUIRE_TTY) {
            errno = ENOTTY;
            return(NULL);
        }
        input = STDIN_FILENO;
        output = STDERR_FILENO;
    }

    /*
     * Catch signals that would otherwise cause the user to end
     * up with echo turned off in the shell.  Don't worry about
     * things like SIGXCPU and SIGVTALRM for now.
     */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;        /* don't restart system calls */
    sa.sa_handler = handler;
    (void)sigaction(SIGALRM, &sa, &savealrm);
    (void)sigaction(SIGHUP, &sa, &savehup);
    (void)sigaction(SIGINT, &sa, &saveint);
    (void)sigaction(SIGPIPE, &sa, &savepipe);
    (void)sigaction(SIGQUIT, &sa, &savequit);
    (void)sigaction(SIGTERM, &sa, &saveterm);
    (void)sigaction(SIGTSTP, &sa, &savetstp);
    (void)sigaction(SIGTTIN, &sa, &savettin);
    (void)sigaction(SIGTTOU, &sa, &savettou);

#if (L7_SERIAL_COM_ATTR)
  #ifndef L7_SSHD
      /* Turn off echo if possible. */
      if (input != STDIN_FILENO && tcgetattr(input, &oterm) == 0) {
          memcpy(&term, &oterm, sizeof(term));
          if (!(flags & RPP_ECHO_ON))
              term.c_lflag &= ~(ECHO | ECHONL);
  #ifdef VSTATUS
          if (term.c_cc[VSTATUS] != _POSIX_VDISABLE)
              term.c_cc[VSTATUS] = _POSIX_VDISABLE;
  #endif
          (void)tcsetattr(input, _T_FLUSH, &term);
      } else {
          memset(&term, 0, sizeof(term));
          term.c_lflag |= ECHO;
          memset(&oterm, 0, sizeof(oterm));
          oterm.c_lflag |= ECHO;
      }
  #endif /* L7_SSHD */
#endif

    if (!(flags & RPP_STDIN))
#ifndef L7_SSHD
        (void)write(output, prompt, strlen(prompt));
#else
        (void)write(output, (char *) prompt, strlen(prompt));
#endif /* L7_SSHD */
    end = buf + bufsiz - 1;
    for (p = buf; (nr = read(input, &ch, 1)) == 1 && ch != '\n' && ch != '\r';) {
        if (p < end) {
            if ((flags & RPP_SEVENBIT))
                ch &= 0x7f;
            if (isalpha((unsigned char) ch)) {
                if ((flags & RPP_FORCELOWER))
                    ch = tolower(ch);
                if ((flags & RPP_FORCEUPPER))
                    ch = toupper(ch);
            }
            *p++ = ch;
        }
    }
    *p = '\0';
    save_errno = errno;

#if (L7_SERIAL_COM_ATTR)
    #ifndef L7_SSHD
    if (!(term.c_lflag & ECHO))
    #endif /* L7_SSHD */
        (void)write(output, "\n", 1);

    #ifndef L7_SSHD
    /* Restore old terminal settings and signals. */
    if (memcmp(&term, &oterm, sizeof(term)) != 0) {
        while (tcsetattr(input, _T_FLUSH, &oterm) == -1 &&
            errno == EINTR)
            continue;
    }
    #endif /* L7_SSHD */
#endif
    (void)sigaction(SIGALRM, &savealrm, NULL);
    (void)sigaction(SIGHUP, &savehup, NULL);
    (void)sigaction(SIGINT, &saveint, NULL);
    (void)sigaction(SIGQUIT, &savequit, NULL);
    (void)sigaction(SIGPIPE, &savepipe, NULL);
    (void)sigaction(SIGTERM, &saveterm, NULL);
    (void)sigaction(SIGTSTP, &savetstp, NULL);
    (void)sigaction(SIGTTIN, &savettin, NULL);
    if (input != STDIN_FILENO)
        (void)close(input);

    /*
     * If we were interrupted by a signal, resend it to ourselves
     * now that we have restored the signal handlers.
     */
#ifndef L7_SSHD
    if (signo) {
        kill(getpid(), signo);
        switch (signo) {
        case SIGTSTP:
        case SIGTTIN:
        case SIGTTOU:
            goto restart;
        }
    }
#endif /* L7_SSHD */

    errno = save_errno;
    return(nr == -1 ? NULL : buf);
}

#if 0
char *
getpass(const char *prompt)
{
    static char buf[_PASSWORD_LEN + 1];

    return(readpassphrase(prompt, buf, sizeof(buf), RPP_ECHO_OFF));
}
#endif

static void handler(int s)
{

    signo = s;
}
#endif /* HAVE_READPASSPHRASE */
