/*  $OpenBSD: session.h,v 1.25 2005/07/17 06:49:04 djm Exp $    */

/*
 * Copyright (c) 2000, 2001 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SESSION_H
#define SESSION_H

#define TTYSZ 64
typedef struct Session Session;
struct Session {
    int used;
    int self;
    struct passwd *pw;
    Authctxt *authctxt;
    pid_t   pid;
    /* tty */
    char    *term;
    int ptyfd, ttyfd, ptymaster;
    u_int   row, col, xpixel, ypixel;
    char    tty[TTYSZ];
    /* X11 */
    u_int   display_number;
    char    *display;
    u_int   screen;
    char    *auth_display;
    char    *auth_proto;
    char    *auth_data;
    int single_connection;
    /* proto 2 */
    int chanid;
    int *x11_chanids;
    int is_subsystem;
    u_int   num_env;
    struct {
        char    *name;
        char    *val;
    }   *env;
#ifdef L7_SSHD
    int cn;
#endif /* L7_SSHD */
};

void     do_authenticated(Authctxt *);
void     do_cleanup(Authctxt *);

int  session_open(Authctxt *, int);
#ifndef L7_SSHD
int  session_input_channel_req(Channel *, const char *);
#else
int      session_input_channel_req(int, Channel *, const char *);
#endif /* L7_SSHD */
void     session_close_by_pid(pid_t, int);
#ifndef L7_SSHD
void     session_close_by_channel(int, void *);
#else
void     session_close_by_channel(int, int, void *);
void     session_destroy_cn(int, void (*)(Session *));
#endif /* L7_SSHD */
void     session_destroy_all(void (*)(Session *));
void     session_pty_cleanup2(Session *);

Session *session_new(void);
Session *session_by_tty(char *);
void     session_close(Session *);
#ifndef L7_SSHD
void     do_setusercontext(struct passwd *);
void     child_set_env(char ***envp, u_int *envsizep, const char *name,
               const char *value);
#endif /* L7_SSHD */

#endif
