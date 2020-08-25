/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Server main loop for handling the interactive session.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 support by Markus Friedl.
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

#include "includes.h"
RCSID("$OpenBSD: serverloop.c,v 1.124 2005/12/13 15:03:02 reyk Exp $");

#include "xmalloc.h"
#include "packet.h"
#include "buffer.h"
#include "log.h"
#include "servconf.h"
#include "canohost.h"
#include "sshpty.h"
#include "channels.h"
#include "compat.h"
#include "ssh1.h"
#include "ssh2.h"
#include "auth.h"
#include "session.h"
#include "dispatch.h"
#include "auth-options.h"
#include "serverloop.h"
#include "misc.h"
#include "kex.h"

extern ServerOptions options;

/* XXX */
extern Kex *xxx_kex[];
extern Authctxt *the_authctxt[];
extern int use_privsep;

Buffer stdin_buffer[L7_OPENSSH_MAX_CONNECTIONS]; /* Buffer for stdin data. */
Buffer stdout_buffer[L7_OPENSSH_MAX_CONNECTIONS];    /* Buffer for stdout data. */
Buffer stderr_buffer[L7_OPENSSH_MAX_CONNECTIONS];    /* Buffer for stderr data. */
static int fdin[L7_OPENSSH_MAX_CONNECTIONS];        /* Descriptor for stdin (for writing) */
static int fdout[L7_OPENSSH_MAX_CONNECTIONS];       /* Descriptor for stdout (for reading);
                   May be same number as fdin. */
static int fderr[L7_OPENSSH_MAX_CONNECTIONS];       /* Descriptor for stderr.  May be -1. */
static long stdin_bytes[L7_OPENSSH_MAX_CONNECTIONS] = {0};    /* Number of bytes written to stdin. */
static long stdout_bytes[L7_OPENSSH_MAX_CONNECTIONS] = {0};   /* Number of stdout bytes sent to client. */
static long stderr_bytes[L7_OPENSSH_MAX_CONNECTIONS] = {0};   /* Number of stderr bytes sent to client. */
static long fdout_bytes[L7_OPENSSH_MAX_CONNECTIONS] = {0};    /* Number of stdout bytes read from program. */
static int stdin_eof[L7_OPENSSH_MAX_CONNECTIONS] = {0};   /* EOF message received from client. */
static int fdout_eof[L7_OPENSSH_MAX_CONNECTIONS] = {0};   /* EOF encountered reading from fdout. */
static int fderr_eof[L7_OPENSSH_MAX_CONNECTIONS] = {0};   /* EOF encountered readung from fderr. */
static int fdin_is_tty[L7_OPENSSH_MAX_CONNECTIONS] = {0}; /* fdin points to a tty. */
static int connection_closed[L7_OPENSSH_MAX_CONNECTIONS] = {0};   /* Connection to client closed. */
static u_int buffer_high[L7_OPENSSH_MAX_CONNECTIONS];   /* "Soft" max buffer size. */
static int client_alive_timeouts[L7_OPENSSH_MAX_CONNECTIONS] = {0};

extern int connection_in[];
extern int connection_out[];

/*
 * This SIGCHLD kludge is used to detect when the child exits.  The server
 * will exit after that, as soon as forwarded connections have terminated.
 */

static volatile sig_atomic_t child_terminated[L7_OPENSSH_MAX_CONNECTIONS] = {0};  /* The child has terminated. */

/* Cleanup on signals (!use_privsep case only) */
static volatile sig_atomic_t received_sigterm = 0;

/* prototypes */
static void server_init_dispatch(int);

#ifndef L7_SSHD
/*
 * we write to this pipe if a SIGCHLD is caught in order to avoid
 * the race between select() and child_terminated
 */
static int notify_pipe[2];
static void
notify_setup(void)
{
    if (pipe(notify_pipe) < 0) {
        error("pipe(notify_pipe) failed %s", strerror(errno));
    } else if ((fcntl(notify_pipe[0], F_SETFD, 1) == -1) ||
        (fcntl(notify_pipe[1], F_SETFD, 1) == -1)) {
        error("fcntl(notify_pipe, F_SETFD) failed %s", strerror(errno));
        close(notify_pipe[0]);
        close(notify_pipe[1]);
    } else {
        set_nonblock(notify_pipe[0]);
        set_nonblock(notify_pipe[1]);
        return;
    }
    notify_pipe[0] = -1;    /* read end */
    notify_pipe[1] = -1;    /* write end */
}
static void
notify_parent(void)
{
    if (notify_pipe[1] != -1)
        write(notify_pipe[1], "", 1);
}
static void
notify_prepare(fd_set *readset)
{
    if (notify_pipe[0] != -1)
        FD_SET(notify_pipe[0], readset);
}
static void
notify_done(fd_set *readset)
{
    char c;

    if (notify_pipe[0] != -1 && FD_ISSET(notify_pipe[0], readset))
        while (read(notify_pipe[0], &c, 1) != -1)
            debug2("notify_done: reading");
}

static void
sigchld_handler(int sig)
{
    int save_errno = errno;
    debug("Received SIGCHLD.");
    child_terminated = 1;
#ifndef _UNICOS
    mysignal(SIGCHLD, sigchld_handler);
#endif
    notify_parent();
    errno = save_errno;
}
#endif /* L7_SSHD */

static void
sigterm_handler(int sig)
{
    received_sigterm = sig;
}

/*
 * Make packets from buffered stderr data, and buffer it for sending
 * to the client.
 */
void
make_packets_from_stderr_data(int cn)
{
    u_int len;

    /* Send buffered stderr data to the client. */
    while (buffer_len(&stderr_buffer[cn]) > 0 &&
        packet_not_very_much_data_to_write(cn)) {
        len = buffer_len(&stderr_buffer[cn]);
        if (packet_is_interactive(cn)) {
            if (len > 512)
                len = 512;
        } else {
            /* Keep the packets at reasonable size. */
            if (len > packet_get_maxsize(cn))
                len = packet_get_maxsize(cn);
        }
        packet_start(cn, SSH_SMSG_STDERR_DATA);
        packet_put_string(cn, buffer_ptr(&stderr_buffer[cn]), len);
        packet_send(cn);
        buffer_consume(&stderr_buffer[cn], len);
        stderr_bytes[cn] += len;
    }
}

/*
 * Make packets from buffered stdout data, and buffer it for sending to the
 * client.
 */
void
make_packets_from_stdout_data(int cn)
{
    u_int len;

    /* Send buffered stdout data to the client. */
    while (buffer_len(&stdout_buffer[cn]) > 0 &&
        packet_not_very_much_data_to_write(cn)) {
        len = buffer_len(&stdout_buffer[cn]);
        if (packet_is_interactive(cn)) {
            if (len > 512)
                len = 512;
        } else {
            /* Keep the packets at reasonable size. */
            if (len > packet_get_maxsize(cn))
                len = packet_get_maxsize(cn);
        }
        packet_start(cn, SSH_SMSG_STDOUT_DATA);
        packet_put_string(cn, buffer_ptr(&stdout_buffer[cn]), len);
        packet_send(cn);
        buffer_consume(&stdout_buffer[cn], len);
        stdout_bytes[cn] += len;
    }
}

void
client_alive_check(int cn)
{
    int channel_id;

    /* timeout, check to see how many we have had */
    if (++client_alive_timeouts[cn] > options.client_alive_count_max)
        packet_disconnect(cn, "Timeout, your session not responding.");

    /*
     * send a bogus global/channel request with "wantreply",
     * we should get back a failure
     */
    if ((channel_id = channel_find_open(cn)) == -1) {
        packet_start(cn, SSH2_MSG_GLOBAL_REQUEST);
        packet_put_cstring(cn, "keepalive@openssh.com");
        packet_put_char(cn, 1); /* boolean: want reply */
    } else {
        channel_request_start(cn, channel_id, "keepalive@openssh.com", 1);
    }
    packet_send(cn);
}

/*
 * Sleep in select() until we can do something.  This will initialize the
 * select masks.  Upon return, the masks will indicate which descriptors
 * have data or can accept data.  Optionally, a maximum time can be specified
 * for the duration of the wait (0 = infinite).
 */
void
wait_until_can_do_something(int cn, fd_set *readsetp, fd_set *writesetp, int *maxfdp,
    u_int *nallocp, u_int max_time_milliseconds)
{
    struct timeval tv, *tvp;
    int ret;
    int client_alive_scheduled = 0;

    /*
     * if using client_alive, set the max timeout accordingly,
     * and indicate that this particular timeout was for client
     * alive by setting the client_alive_scheduled flag.
     *
     * this could be randomized somewhat to make traffic
     * analysis more difficult, but we're not doing it yet.
     */
    if (compat20[cn] &&
        max_time_milliseconds == 0 && options.client_alive_interval) {
        client_alive_scheduled = 1;
        max_time_milliseconds = options.client_alive_interval * 1000;
    }

    /* Allocate and update select() masks for channel descriptors. */
    channel_prepare_select(cn, readsetp, writesetp, maxfdp, nallocp, 0);

    if (compat20[cn]) {
#if 0
        /* wrong: bad condition XXX */
        if (channel_not_very_much_buffered_data(cn))
#endif
        FD_SET(connection_in[cn], readsetp);
    } else {
        /*
         * Read packets from the client unless we have too much
         * buffered stdin or channel data.
         */
        if (buffer_len(&stdin_buffer[cn]) < buffer_high[cn] &&
            channel_not_very_much_buffered_data(cn))
            FD_SET(connection_in[cn], readsetp);
        /*
         * If there is not too much data already buffered going to
         * the client, try to get some more data from the program.
         */
        if (packet_not_very_much_data_to_write(cn)) {
            if (!fdout_eof[cn])
                FD_SET(fdout[cn], readsetp);
            if (!fderr_eof[cn])
                FD_SET(fderr[cn], readsetp);
        }
        /*
         * If we have buffered data, try to write some of that data
         * to the program.
         */
        if (fdin[cn] != -1 && buffer_len(&stdin_buffer[cn]) > 0)
            FD_SET(fdin[cn], writesetp);
    }
#ifndef L7_SSHD
    notify_prepare(*readsetp);
#endif /* L7_SSHD */

    /*
     * If we have buffered packet data going to the client, mark that
     * descriptor.
     */
    if (packet_have_data_to_write(cn))
        FD_SET(connection_out[cn], writesetp);

    /*
     * If child has terminated and there is enough buffer space to read
     * from it, then read as much as is available and exit.
     */
    if (child_terminated[cn] && packet_not_very_much_data_to_write(cn))
        if (max_time_milliseconds == 0 || client_alive_scheduled)
            max_time_milliseconds = 100;

    if (max_time_milliseconds == 0)
        tvp = NULL;
    else {
        tv.tv_sec = max_time_milliseconds / 1000;
        tv.tv_usec = 1000 * (max_time_milliseconds % 1000);
        tvp = &tv;
    }

    /* Wait for something to happen, or the timeout to expire. */
    ret = select((*maxfdp)+1, readsetp, writesetp, NULL, tvp);

    if (ret == -1) {
        memset(readsetp, 0, sizeof(fd_set));
        memset(writesetp, 0, sizeof(fd_set));
        if (errno != EINTR)
            error("select: %.100s", strerror(errno));
    } else if (ret == 0 && client_alive_scheduled)
        client_alive_check(cn);

#ifndef L7_SSHD
    notify_done(*readsetp);
#endif /* L7_SSHD */
}

/*
 * Processes input from the client and the program.  Input data is stored
 * in buffers and processed later.
 */
void
process_input(int cn, fd_set * readset)
{
    int len;
    char buf[16384];

    /* Read and buffer any input data from the client. */
    if (FD_ISSET(connection_in[cn], readset)) {
        len = read(connection_in[cn], buf, sizeof(buf));
        if (len == 0) {
            verbose("Connection closed by %.100s",
                get_remote_ipaddr(cn));
            connection_closed[cn] = 1;
            if (compat20[cn])
                return;
            cleanup_exit_cn(cn, 255);
        } else if (len < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                verbose("Read error from remote host "
                    "%.100s: %.100s",
                    get_remote_ipaddr(cn), strerror(errno));
                cleanup_exit_cn(cn, 255);
            }
        } else {
            /* Buffer any received data. */
            packet_process_incoming(cn, buf, len);
        }
    }
    if (compat20[cn])
        return;

    /* Read and buffer any available stdout data from the program. */
    if (!fdout_eof[cn] && FD_ISSET(fdout[cn], readset)) {
        len = read(fdout[cn], buf, sizeof(buf));
        if (len < 0 && (errno == EINTR || errno == EAGAIN)) {
            /* do nothing */
        } else if (len <= 0) {
            fdout_eof[cn] = 1;
        } else {
            buffer_append(&stdout_buffer[cn], buf, len);
            fdout_bytes[cn] += len;
        }
    }
    /* Read and buffer any available stderr data from the program. */
    if (!fderr_eof[cn] && FD_ISSET(fderr[cn], readset)) {
        len = read(fderr[cn], buf, sizeof(buf));
        if (len < 0 && (errno == EINTR || errno == EAGAIN)) {
            /* do nothing */
        } else if (len <= 0) {
            fderr_eof[cn] = 1;
        } else {
            buffer_append(&stderr_buffer[cn], buf, len);
        }
    }
}

/*
 * Sends data from internal buffers to client program stdin.
 */
void
process_output(int cn, fd_set * writeset)
{
#ifndef L7_SSHD
    struct termios tio;
#endif /* L7_SSHD */
    u_char *data;
    u_int dlen;
    int len;

    /* Write buffered data to program stdin. */
    if (!compat20[cn] && fdin[cn] != -1 && FD_ISSET(fdin[cn], writeset)) {
        data = buffer_ptr(&stdin_buffer[cn]);
        dlen = buffer_len(&stdin_buffer[cn]);
        len = write(fdin[cn], data, dlen);
        if (len < 0 && (errno == EINTR || errno == EAGAIN)) {
            /* do nothing */
        } else if (len <= 0) {
            if (fdin[cn] != fdout[cn])
                close(fdin[cn]);
            else
                shutdown(fdin[cn], SHUT_WR); /* We will no longer send. */
            fdin[cn] = -1;
        } else {
            /* Successful write. */
#ifndef L7_SSHD
  /* PTin removed: serial port */
  #if (L7_SERIAL_COM_ATTR)
            if (fdin_is_tty[cn] && dlen >= 1 && data[0] != '\r' &&
                tcgetattr(fdin[cn], &tio) == 0 &&
                !(tio.c_lflag & ECHO) && (tio.c_lflag & ICANON)) {
                /*
                 * Simulate echo to reduce the impact of
                 * traffic analysis
                 */
                packet_send_ignore(cn, len);
                packet_send(cn);
            }
  #endif
#endif /* L7_SSHD */
            /* Consume the data from the buffer. */
            buffer_consume(&stdin_buffer[cn], len);
            /* Update the count of bytes written to the program. */
            stdin_bytes[cn] += len;
        }
    }
    /* Send any buffered packet data to the client. */
    if (FD_ISSET(connection_out[cn], writeset))
        packet_write_poll(cn);
}

/*
 * Wait until all buffered output has been sent to the client.
 * This is used when the program terminates.
 */
void
drain_output(int cn)
{
    /* Send any buffered stdout data to the client. */
    if (buffer_len(&stdout_buffer[cn]) > 0) {
        packet_start(cn, SSH_SMSG_STDOUT_DATA);
        packet_put_string(cn, buffer_ptr(&stdout_buffer[cn]),
                  buffer_len(&stdout_buffer[cn]));
        packet_send(cn);
        /* Update the count of sent bytes. */
        stdout_bytes[cn] += buffer_len(&stdout_buffer[cn]);
    }
    /* Send any buffered stderr data to the client. */
    if (buffer_len(&stderr_buffer[cn]) > 0) {
        packet_start(cn, SSH_SMSG_STDERR_DATA);
        packet_put_string(cn, buffer_ptr(&stderr_buffer[cn]),
                  buffer_len(&stderr_buffer[cn]));
        packet_send(cn);
        /* Update the count of sent bytes. */
        stderr_bytes[cn] += buffer_len(&stderr_buffer[cn]);
    }
    /* Wait until all buffered data has been written to the client. */
    packet_write_wait(cn);
}

void
process_buffered_input_packets(int cn)
{
    dispatch_run(cn, DISPATCH_NONBLOCK, NULL, compat20[cn] ? xxx_kex[cn] : NULL);
}

/*
 * Performs the interactive session.  This handles data transmission between
 * the client and the program.  Note that the notion of stdin, stdout, and
 * stderr in this function is sort of reversed: this function writes to
 * stdin (of the child program), and reads from stdout and stderr (of the
 * child program).
 */
void
server_loop(int cn, pid_t pid, int fdin_arg, int fdout_arg, int fderr_arg)
{
    fd_set readset, writeset;
    int max_fd = 0;
    u_int nalloc = 0;
#ifndef L7_SSHD
    int wait_status;    /* Status returned by wait(). */
#endif /* L7_SSHD */
#ifndef L7_SSHD
    pid_t wait_pid;     /* pid returned by wait(). */
#endif /* L7_SSHD */
    int waiting_termination = 0;    /* Have displayed waiting close message. */
    u_int max_time_milliseconds;
    u_int previous_stdout_buffer_bytes;
    u_int stdout_buffer_bytes;
    int type;

    debug("Entering interactive session, fdin %d, fdout %d, fderr %d", fdin_arg, fdout_arg, fderr_arg);

    /* Initialize the SIGCHLD kludge. */
    child_terminated[cn] = 0;
#ifndef L7_SSHD
    mysignal(SIGCHLD, sigchld_handler);
#endif /* L7_SSHD */

    if (!use_privsep) {
        signal(SIGTERM, sigterm_handler);
        signal(SIGINT, sigterm_handler);
        signal(SIGQUIT, sigterm_handler);
    }

    /* Initialize our global variables. */
    fdin[cn] = fdin_arg;
    fdout[cn] = fdout_arg;
    fderr[cn] = fderr_arg;

    /* nonblocking IO */
    set_nonblock(fdin[cn]);
    set_nonblock(fdout[cn]);
    /* we don't have stderr for interactive terminal sessions, see below */
    if (fderr[cn] != -1)
        set_nonblock(fderr[cn]);

    if (!(datafellows[cn] & SSH_BUG_IGNOREMSG) && isatty(fdin[cn]))
        fdin_is_tty[cn] = 1;

#ifndef L7_SSHD
    connection_in = packet_get_connection_in();
    connection_out = packet_get_connection_out();

    notify_setup();
#endif /* L7_SSHD */

    previous_stdout_buffer_bytes = 0;

    /* Set approximate I/O buffer size. */
    if (packet_is_interactive(cn))
        buffer_high[cn] = 4096;
    else
        buffer_high[cn] = 64 * 1024;

#if 0
    /* Initialize max_fd to the maximum of the known file descriptors. */
    max_fd = MAX(connection_in[cn], connection_out[cn]);
    max_fd = MAX(max_fd, fdin[cn]);
    max_fd = MAX(max_fd, fdout[cn]);
    if (fderr[cn] != -1)
        max_fd = MAX(max_fd, fderr[cn]);
#endif

    /* Initialize Initialize buffers. */
    buffer_init(&stdin_buffer[cn]);
    buffer_init(&stdout_buffer[cn]);
    buffer_init(&stderr_buffer[cn]);

    /*
     * If we have no separate fderr (which is the case when we have a pty
     * - there we cannot make difference between data sent to stdout and
     * stderr), indicate that we have seen an EOF from stderr.  This way
     * we don't need to check the descriptor everywhere.
     */
    if (fderr[cn] == -1)
        fderr_eof[cn] = 1;

    server_init_dispatch(cn);

    /* Main loop of the server for the interactive session mode. */
    for (;;) {

        /* Process buffered packets from the client. */
        process_buffered_input_packets(cn);

        /*
         * If we have received eof, and there is no more pending
         * input data, cause a real eof by closing fdin.
         */
        if (stdin_eof[cn] && fdin[cn] != -1 && buffer_len(&stdin_buffer[cn]) == 0) {
            if (fdin[cn] != fdout[cn])
                close(fdin[cn]);
            else
                shutdown(fdin[cn], SHUT_WR); /* We will no longer send. */
            fdin[cn] = -1;
        }
        /* Make packets from buffered stderr data to send to the client. */
        make_packets_from_stderr_data(cn);

        /*
         * Make packets from buffered stdout data to send to the
         * client. If there is very little to send, this arranges to
         * not send them now, but to wait a short while to see if we
         * are getting more data. This is necessary, as some systems
         * wake up readers from a pty after each separate character.
         */
        max_time_milliseconds = 0;
        stdout_buffer_bytes = buffer_len(&stdout_buffer[cn]);
        if (stdout_buffer_bytes != 0 && stdout_buffer_bytes < 256 &&
            stdout_buffer_bytes != previous_stdout_buffer_bytes) {
            /* try again after a while */
            max_time_milliseconds = 10;
        } else {
            /* Send it now. */
            make_packets_from_stdout_data(cn);
        }
        previous_stdout_buffer_bytes = buffer_len(&stdout_buffer[cn]);

        /* Send channel data to the client. */
        if (packet_not_very_much_data_to_write(cn))
            channel_output_poll(cn);

        /*
         * Bail out of the loop if the program has closed its output
         * descriptors, and we have no more data to send to the
         * client, and there is no pending buffered data.
         */
        if (fdout_eof[cn] && fderr_eof[cn] && !packet_have_data_to_write(cn) &&
            buffer_len(&stdout_buffer[cn]) == 0 && buffer_len(&stderr_buffer[cn]) == 0) {
            if (!channel_still_open(cn))
                break;
            if (!waiting_termination) {
                const char *s = "Waiting for forwarded connections to terminate...\r\n";
                char *cp;
                waiting_termination = 1;
                buffer_append(&stderr_buffer[cn], s, strlen(s));

                /* Display list of open channels. */
                cp = channel_open_message(cn);
                buffer_append(&stderr_buffer[cn], cp, strlen(cp));
                xfree(cp);
            }
        }
        max_fd = MAX(connection_in[cn], connection_out[cn]);
        max_fd = MAX(max_fd, fdin[cn]);
        max_fd = MAX(max_fd, fdout[cn]);
        max_fd = MAX(max_fd, fderr[cn]);
#ifndef L7_SSHD
        max_fd = MAX(max_fd, notify_pipe[0]);
#endif /* L7_SSHD */

        /* Sleep in select() until we can do something. */
        wait_until_can_do_something(cn, &readset, &writeset, &max_fd,
            &nalloc, max_time_milliseconds);

        if (received_sigterm) {
            logit("Exiting on signal %d", received_sigterm);
            /* Clean up sessions, utmp, etc. */
            cleanup_exit_cn(cn, 255);
        }

        /* Process any channel events. */
        channel_after_select(cn, &readset, &writeset);

        /* Process input from the client and from program stdout/stderr. */
        process_input(cn, &readset);

        /* Process output to the client and to program stdin. */
        process_output(cn, &writeset);
    }
#ifndef L7_SSHD
    if (readset)
        xfree(readset);
    if (writeset)
        xfree(writeset);
#else
    connection_closed[cn] = 0;
#endif /* L7_SSHD */

    /* Cleanup and termination code. */

    /* Wait until all output has been sent to the client. */
    drain_output(cn);

    debug("End of interactive session; stdin %ld, stdout (read %ld, sent %ld), stderr %ld bytes.",
        stdin_bytes[cn], fdout_bytes[cn], stdout_bytes[cn], stderr_bytes[cn]);

    /* Free and clear the buffers. */
    buffer_free(&stdin_buffer[cn]);
    buffer_free(&stdout_buffer[cn]);
    buffer_free(&stderr_buffer[cn]);

    /* Close the file descriptors. */
    if (fdout[cn] != -1)
        close(fdout[cn]);
    fdout[cn] = -1;
    fdout_eof[cn] = 0;
    if (fderr[cn] != -1)
        close(fderr[cn]);
    fderr[cn] = -1;
    fderr_eof[cn] = 0;
    if (fdin[cn] != -1)
        close(fdin[cn]);
    fdin[cn] = -1;
    stdin_eof[cn] = 0;

#ifndef L7_SSHD
    channel_free_all();

    /* We no longer want our SIGCHLD handler to be called. */
    mysignal(SIGCHLD, SIG_DFL);

    while ((wait_pid = waitpid(-1, &wait_status, 0)) < 0)
        if (errno != EINTR)
            packet_disconnect(cn, "wait: %.100s", strerror(errno));
    if (wait_pid != pid)
        error("Strange, wait returned pid %ld, expected %ld",
            (long)wait_pid, (long)pid);

    /* Check if it exited normally. */
    if (WIFEXITED(wait_status)) {
        /* Yes, normal exit.  Get exit status and send it to the client. */
        debug("Command exited with status %d.", WEXITSTATUS(wait_status));
        packet_start(cn, SSH_SMSG_EXITSTATUS);
        packet_put_int(cn, WEXITSTATUS(wait_status));
        packet_send(cn);
        packet_write_wait(cn);

        /*
         * Wait for exit confirmation.  Note that there might be
         * other packets coming before it; however, the program has
         * already died so we just ignore them.  The client is
         * supposed to respond with the confirmation when it receives
         * the exit status.
         */
        do {
            type = packet_read(cn);
        }
        while (type != SSH_CMSG_EXIT_CONFIRMATION);

        debug("Received exit confirmation.");
        return;
    }
    /* Check if the program terminated due to a signal. */
    if (WIFSIGNALED(wait_status))
        packet_disconnect(cn, "Command terminated on signal %d.",
                  WTERMSIG(wait_status));
#else
    channel_free_cn(cn);
#endif /* L7_SSHD */

#ifdef COMMENTED_OUT
    /* Some weird exit cause.  Just exit. */
    packet_disconnect(cn, "logout");
#else
    packet_start(cn, SSH_SMSG_EXITSTATUS);
    packet_put_int(cn, 0);
    packet_send(cn);
    packet_write_wait(cn);
    /*
     * Wait for exit confirmation.  Note that there might be
     * other packets coming before it; however, the program has
     * already died so we just ignore them.  The client is
     * supposed to respond with the confirmation when it receives
     * the exit status.
     */
    do {
        type = packet_read(cn);
    }
    while ((type != SSH_CMSG_EXIT_CONFIRMATION) && (type != SSH_MSG_DISCONNECT));
    if (type == SSH_CMSG_EXIT_CONFIRMATION)
    {
        debug("Received exit confirmation.");
    }
    else
    {
        debug("Received SSH_MSG_DISCONNECT");
    }
    channel_close_cn(cn);
    cleanup_exit_cn(cn, 255);
#endif /* COMMENTED_OUT */
    /* NOTREACHED */
}

#ifndef L7_SSHD
static void
collect_children(void)
{
    pid_t pid;
    sigset_t oset, nset;
    int status;

    /* block SIGCHLD while we check for dead children */
    sigemptyset(&nset);
    sigaddset(&nset, SIGCHLD);
    sigprocmask(SIG_BLOCK, &nset, &oset);
    if (child_terminated) {
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0 ||
            (pid < 0 && errno == EINTR))
            if (pid > 0)
                session_close_by_pid(pid, status);
        child_terminated = 0;
    }
    sigprocmask(SIG_SETMASK, &oset, NULL);
}
#endif /* L7_SSHD */

void
server_loop2(Authctxt *authctxt)
{
    fd_set readset, writeset;
    int rekeying = 0, max_fd, nalloc = 0;

    debug("connection %d: entering interactive session for SSH2", authctxt->cn);

#ifndef L7_SSHD
    mysignal(SIGCHLD, sigchld_handler);
#endif /* L7_SSHD */
    child_terminated[authctxt->cn] = 0;
#ifndef L7_SSHD
    connection_in = packet_get_connection_in();
    connection_out = packet_get_connection_out();
#endif /* L7_SSHD */

    if (!use_privsep) {
        signal(SIGTERM, sigterm_handler);
        signal(SIGINT, sigterm_handler);
        signal(SIGQUIT, sigterm_handler);
    }

#ifndef L7_SSHD
    notify_setup();
#endif /* L7_SSHD */

    max_fd = MAX(connection_in[authctxt->cn], connection_out[authctxt->cn]);
    debugl7(SYSLOG_LEVEL_VERBOSE, "max_fd %d, connection_in[%d] %d, connection_out[%d] %d",
            max_fd, authctxt->cn, connection_in[authctxt->cn], authctxt->cn, connection_out[authctxt->cn]);

#ifndef L7_SSHD
    max_fd = MAX(max_fd, notify_pipe[0]);
#endif /* L7_SSHD */

    server_init_dispatch(authctxt->cn);

    for (;;) {
        process_buffered_input_packets(authctxt->cn);

        rekeying = (xxx_kex[authctxt->cn] != NULL && !xxx_kex[authctxt->cn]->done);

        if (!rekeying && packet_not_very_much_data_to_write(authctxt->cn))
            channel_output_poll(authctxt->cn);
        wait_until_can_do_something(authctxt->cn, &readset, &writeset, &max_fd,
            &nalloc, 0);

        if (received_sigterm) {
            logit("Exiting on signal %d", received_sigterm);
            /* Clean up sessions, utmp, etc. */
            cleanup_exit_cn(authctxt->cn, 255);
        }

#ifndef L7_SSHD
        collect_children();
#endif /* L7_SSHD */
        if (!rekeying) {
            channel_after_select(authctxt->cn, &readset, &writeset);
            if (packet_need_rekeying(authctxt->cn)) {
                debug("need rekeying");
                xxx_kex[authctxt->cn]->done = 0;
                kex_send_kexinit(authctxt->cn, xxx_kex[authctxt->cn]);
            }
        }
        process_input(authctxt->cn, &readset);
        if (connection_closed[authctxt->cn])
            break;
        process_output(authctxt->cn, &writeset);
    }
#ifndef L7_SSHD
    collect_children();

    if (readset)
        xfree(readset);
    if (writeset)
        xfree(writeset);
#else
    connection_closed[authctxt->cn] = 0;
#endif /* L7_SSHD */

#ifndef L7_SSHD
    /* free all channels, no more reads and writes */
    channel_free_cn(authctxt->cn);

    /* free remaining sessions, e.g. remove wtmp entries */
    session_destroy_cn(authctxt->cn, NULL);
#else
    cleanup_exit_cn(authctxt->cn, 255);
#endif /* L7_SSHD */
}

void
server_input_keep_alive(int cn, int type, u_int32_t seq, void *ctxt)
{
    debug("Got %d/%u for keepalive", type, (unsigned int) seq);
    /*
     * reset timeout, since we got a sane answer from the client.
     * even if this was generated by something other than
     * the bogus CHANNEL_REQUEST we send for keepalives.
     */
    client_alive_timeouts[cn] = 0;
}

#ifdef L7_SSHD
void
server_input_disconnect(int cn, int type, u_int32_t seq, void *ctxt)
{
  cleanup_exit_cn(cn, 255);
}
#endif /* L7_SSHD */

void
server_input_stdin_data(int cn, int type, u_int32_t seq, void *ctxt)
{
    char *data;
    u_int data_len;

    /* Stdin data from the client.  Append it to the buffer. */
    /* Ignore any data if the client has closed stdin. */
    if (fdin[cn] == -1)
        return;
    data = packet_get_string(cn, &data_len);
    packet_check_eom(cn);
    buffer_append(&stdin_buffer[cn], data, data_len);
    memset(data, 0, data_len);
    xfree(data);
}

void
server_input_eof(int cn, int type, u_int32_t seq, void *ctxt)
{
    /*
     * Eof from the client.  The stdin descriptor to the
     * program will be closed when all buffered data has
     * drained.
     */
    debug("EOF received for stdin.");
    packet_check_eom(cn);
    stdin_eof[cn] = 1;
}

void
server_input_window_size(int cn, int type, u_int32_t seq, void *ctxt)
{
    int row = packet_get_int(cn);
    int col = packet_get_int(cn);
    int xpixel = packet_get_int(cn);
    int ypixel = packet_get_int(cn);

    debug("Window change received.");
    packet_check_eom(cn);
#ifndef L7_SSHD
    if (fdin[cn] != -1)
        pty_change_window_size(fdin[cn], row, col, xpixel, ypixel);
#else
    debugl7(SYSLOG_LEVEL_DEBUG1, "row %d, col %d, xpixel %d, ypixel %d\n", row, col, xpixel, ypixel);
    /*
    L7: Send appropriate information to emWeb
    */
#endif /* L7_SSHD */
}

Channel *
server_request_direct_tcpip(int cn)
{
    Channel *c;
    int sock;
    char *target, *originator;
    int target_port, originator_port;

    target = packet_get_string(cn, NULL);
    target_port = packet_get_int(cn);
    originator = packet_get_string(cn, NULL);
    originator_port = packet_get_int(cn);
    packet_check_eom(cn);

    debug("server_request_direct_tcpip: originator %s port %d, target %s port %d",
        originator, originator_port, target, target_port);

    /* XXX check permission */
    sock = channel_connect_to(target, target_port);
    xfree(target);
    xfree(originator);
    if (sock < 0)
        return NULL;
    c = channel_new(cn, "direct-tcpip", SSH_CHANNEL_CONNECTING,
        sock, sock, -1, CHAN_TCP_WINDOW_DEFAULT,
        CHAN_TCP_PACKET_DEFAULT, 0, "direct-tcpip", 1);
    return c;
}

Channel *
server_request_tun(int cn)
{
    Channel *c = NULL;
    int mode, tun;
    int sock;

    mode = packet_get_int(cn);
    switch (mode) {
    case SSH_TUNMODE_POINTOPOINT:
    case SSH_TUNMODE_ETHERNET:
        break;
    default:
        packet_send_debug(cn, "Unsupported tunnel device mode.");
        return NULL;
    }
    if ((options.permit_tun & mode) == 0) {
        packet_send_debug(cn, "Server has rejected tunnel device "
            "forwarding");
        return NULL;
    }

    tun = packet_get_int(cn);
    if (forced_tun_device != -1) {
        if (tun != SSH_TUNID_ANY && forced_tun_device != tun)
            goto done;
        tun = forced_tun_device;
    }
    sock = tun_open(tun, mode);
    if (sock < 0)
        goto done;
    c = channel_new(cn, "tun", SSH_CHANNEL_OPEN, sock, sock, -1,
        CHAN_TCP_WINDOW_DEFAULT, CHAN_TCP_PACKET_DEFAULT, 0, "tun", 1);
    c->datagram = 1;
#if defined(SSH_TUN_FILTER)
    if (mode == SSH_TUNMODE_POINTOPOINT)
        channel_register_filter(cn, c->self, sys_tun_infilter,
            sys_tun_outfilter);
#endif

 done:
    if (c == NULL)
        packet_send_debug(cn, "Failed to open the tunnel device.");
    return c;
}

Channel *
server_request_session(int cn)
{
    Channel *c;

    debug("input_session_request");
    packet_check_eom(cn);
    /*
     * A server session has no fd to read or write until a
     * CHANNEL_REQUEST for a shell is made, so we set the type to
     * SSH_CHANNEL_LARVAL.  Additionally, a callback for handling all
     * CHANNEL_REQUEST messages is registered.
     */
    c = channel_new(cn, "session", SSH_CHANNEL_LARVAL,
        -1, -1, -1, /*window size*/0, CHAN_SES_PACKET_DEFAULT,
        0, "server-session", 1);
    if (session_open(the_authctxt[cn], c->self) != 1) {
        debug("session open failed, free channel %d", c->self);
        channel_free(c);
        return NULL;
    }
    channel_register_cleanup(cn, c->self, session_close_by_channel, 0);
    return c;
}

void
server_input_channel_open(int cn, int type, u_int32_t seq, void *ctxt)
{
    Channel *c = NULL;
    char *ctype;
    int rchan;
    u_int rmaxpack, rwindow, len;

    ctype = packet_get_string(cn, &len);
    rchan = packet_get_int(cn);
    rwindow = packet_get_int(cn);
    rmaxpack = packet_get_int(cn);

    debug("server_input_channel_open: ctype %s rchan %d win %d max %d",
        ctype, rchan, rwindow, rmaxpack);

    if (strcmp(ctype, "session") == 0) {
        c = server_request_session(cn);
    } else if (strcmp(ctype, "direct-tcpip") == 0) {
        c = server_request_direct_tcpip(cn);
    } else if (strcmp(ctype, "tun@openssh.com") == 0) {
        c = server_request_tun(cn);
    }
    if (c != NULL) {
        debug("server_input_channel_open: confirm %s", ctype);
        c->remote_id = rchan;
        c->remote_window = rwindow;
        c->remote_maxpacket = rmaxpack;
        if (c->type != SSH_CHANNEL_CONNECTING) {
            packet_start(cn, SSH2_MSG_CHANNEL_OPEN_CONFIRMATION);
            packet_put_int(cn, c->remote_id);
            packet_put_int(cn, c->self);
            packet_put_int(cn, c->local_window);
            packet_put_int(cn, c->local_maxpacket);
            packet_send(cn);
        }
    } else {
        debug("server_input_channel_open: failure %s", ctype);
        packet_start(cn, SSH2_MSG_CHANNEL_OPEN_FAILURE);
        packet_put_int(cn, rchan);
        packet_put_int(cn, SSH2_OPEN_ADMINISTRATIVELY_PROHIBITED);
        if (!(datafellows[cn] & SSH_BUG_OPENFAILURE)) {
            packet_put_cstring(cn, "open failed");
            packet_put_cstring(cn, "");
        }
        packet_send(cn);
    }
    xfree(ctype);
}

void
server_input_global_request(int cn, int type, u_int32_t seq, void *ctxt)
{
    char *rtype;
    int want_reply;
    int success = 0;

    rtype = packet_get_string(cn, NULL);
    want_reply = packet_get_char(cn);
    debug("server_input_global_request: rtype %s want_reply %d", rtype, want_reply);

    /* -R style forwarding */
    if (strcmp(rtype, "tcpip-forward") == 0) {
        struct passwd *pw;
        char *listen_address;
        u_short listen_port;

        pw = the_authctxt[cn]->pw;
        if (pw == NULL || !the_authctxt[cn]->valid)
            fatal_cn(cn, "server_input_global_request: no/invalid user");
        listen_address = packet_get_string(cn, NULL);
        listen_port = (u_short)packet_get_int(cn);
        debug("server_input_global_request: tcpip-forward listen %s port %d",
            listen_address, listen_port);

        /* check permissions */
        if (!options.allow_tcp_forwarding ||
            no_port_forwarding_flag
#ifndef NO_IPPORT_RESERVED_CONCEPT
#ifndef L7_SSHD
            || (listen_port < IPPORT_RESERVED && pw->pw_uid != 0)
#else
            || (listen_port < IPPORT_RESERVED)
#endif /* L7_SSHD */
#endif
            ) {
            success = 0;
            packet_send_debug(cn, "Server has disabled port forwarding.");
        } else {
            /* Start listening on the port */
            success = channel_setup_remote_fwd_listener(cn,
                listen_address, listen_port, options.gateway_ports);
        }
        xfree(listen_address);
    } else if (strcmp(rtype, "cancel-tcpip-forward") == 0) {
        char *cancel_address;
        u_short cancel_port;

        cancel_address = packet_get_string(cn, NULL);
        cancel_port = (u_short)packet_get_int(cn);
        debug("%s: cancel-tcpip-forward addr %s port %d", __func__,
            cancel_address, cancel_port);

        success = channel_cancel_rport_listener(cn, cancel_address,
            cancel_port);
    }
    if (want_reply) {
        packet_start(cn, success ?
            SSH2_MSG_REQUEST_SUCCESS : SSH2_MSG_REQUEST_FAILURE);
        packet_send(cn);
        packet_write_wait(cn);
    }
    xfree(rtype);
}
void
server_input_channel_req(int cn, int type, u_int32_t seq, void *ctxt)
{
    Channel *c;
    int id, reply, success = 0;
    char *rtype;

    id = packet_get_int(cn);
    rtype = packet_get_string(cn, NULL);
    reply = packet_get_char(cn);

    debug("server_input_channel_req: channel %d request %s reply %d",
        id, rtype, reply);

    if ((c = channel_lookup(cn, id)) == NULL)
        packet_disconnect(cn, "server_input_channel_req: "
            "unknown channel %d", id);
    if (c->type == SSH_CHANNEL_LARVAL || c->type == SSH_CHANNEL_OPEN)
        success = session_input_channel_req(cn, c, rtype);
    if (reply) {
        packet_start(cn, success ?
            SSH2_MSG_CHANNEL_SUCCESS : SSH2_MSG_CHANNEL_FAILURE);
        packet_put_int(cn, c->remote_id);
        packet_send(cn);
    }
    xfree(rtype);
}

void
server_init_dispatch_20(int cn)
{
    debug("connection %d: server_init_dispatch_20", cn);
    dispatch_init(cn, &dispatch_protocol_error);
    dispatch_set(cn, SSH2_MSG_CHANNEL_CLOSE, &channel_input_oclose);
    dispatch_set(cn, SSH2_MSG_CHANNEL_DATA, &channel_input_data);
    dispatch_set(cn, SSH2_MSG_CHANNEL_EOF, &channel_input_ieof);
    dispatch_set(cn, SSH2_MSG_CHANNEL_EXTENDED_DATA, &channel_input_extended_data);
    dispatch_set(cn, SSH2_MSG_CHANNEL_OPEN, &server_input_channel_open);
    dispatch_set(cn, SSH2_MSG_CHANNEL_OPEN_CONFIRMATION, &channel_input_open_confirmation);
    dispatch_set(cn, SSH2_MSG_CHANNEL_OPEN_FAILURE, &channel_input_open_failure);
    dispatch_set(cn, SSH2_MSG_CHANNEL_REQUEST, &server_input_channel_req);
    dispatch_set(cn, SSH2_MSG_CHANNEL_WINDOW_ADJUST, &channel_input_window_adjust);
    dispatch_set(cn, SSH2_MSG_GLOBAL_REQUEST, &server_input_global_request);
    /* client_alive */
    dispatch_set(cn, SSH2_MSG_CHANNEL_FAILURE, &server_input_keep_alive);
    dispatch_set(cn, SSH2_MSG_REQUEST_SUCCESS, &server_input_keep_alive);
    dispatch_set(cn, SSH2_MSG_REQUEST_FAILURE, &server_input_keep_alive);
    /* rekeying */
    dispatch_set(cn, SSH2_MSG_KEXINIT, &kex_input_kexinit);
#ifdef L7_SSHD
    dispatch_set(cn, SSH2_MSG_DISCONNECT, &server_input_disconnect);
#endif /* L7_SSHD */
}
void
server_init_dispatch_13(int cn)
{
    debug("connection %d: server_init_dispatch_13", cn);
    dispatch_init(cn, NULL);
    dispatch_set(cn, SSH_CMSG_EOF, &server_input_eof);
    dispatch_set(cn, SSH_CMSG_STDIN_DATA, &server_input_stdin_data);
    dispatch_set(cn, SSH_CMSG_WINDOW_SIZE, &server_input_window_size);
    dispatch_set(cn, SSH_MSG_CHANNEL_CLOSE, &channel_input_close);
    dispatch_set(cn, SSH_MSG_CHANNEL_CLOSE_CONFIRMATION, &channel_input_close_confirmation);
    dispatch_set(cn, SSH_MSG_CHANNEL_DATA, &channel_input_data);
    dispatch_set(cn, SSH_MSG_CHANNEL_OPEN_CONFIRMATION, &channel_input_open_confirmation);
    dispatch_set(cn, SSH_MSG_CHANNEL_OPEN_FAILURE, &channel_input_open_failure);
    dispatch_set(cn, SSH_MSG_PORT_OPEN, &channel_input_port_open);
#ifdef L7_SSHD
    dispatch_set(cn, SSH_MSG_DISCONNECT, &server_input_disconnect);
#endif /* L7_SSHD */
}
void
server_init_dispatch_15(int cn)
{
    server_init_dispatch_13(cn);
    debug("connection %d: server_init_dispatch_15", cn);
    dispatch_set(cn, SSH_MSG_CHANNEL_CLOSE, &channel_input_ieof);
    dispatch_set(cn, SSH_MSG_CHANNEL_CLOSE_CONFIRMATION, &channel_input_oclose);
}
void
server_init_dispatch(int cn)
{
    if (compat20[cn])
        server_init_dispatch_20(cn);
    else if (compat13[cn])
        server_init_dispatch_13(cn);
    else
        server_init_dispatch_15(cn);
}
