/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains functions for generic socket connection forwarding.
 * There is also code for initiating connection forwarding for X11 connections,
 * arbitrary tcp/ip connections, and the authentication agent connection.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 support added by Markus Friedl.
 * Copyright (c) 1999, 2000, 2001, 2002 Markus Friedl.  All rights reserved.
 * Copyright (c) 1999 Dug Song.  All rights reserved.
 * Copyright (c) 1999 Theo de Raadt.  All rights reserved.
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
RCSID("$OpenBSD: channels.c,v 1.232 2006/01/30 12:22:22 reyk Exp $");

#include "ssh.h"
#include "ssh1.h"
#include "ssh2.h"
#include "packet.h"
#include "xmalloc.h"
#include "log.h"
#include "misc.h"
#include "channels.h"
#include "compat.h"
#include "canohost.h"
#include "key.h"
#include "authfd.h"
#include "pathnames.h"
#include "bufaux.h"
#include "atomicio.h"

#define SSHD_MAX_CHANNELS   100

extern void         *sshdChannelSema;

/* -- channel core */

/*
 * Pointer to an array containing all allocated channels.  The array is
 * dynamically extended as needed.
 */
static Channel **channels[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};

/*
 * Size of the channel array.  All slots of the array must always be
 * initialized (at least the type field); unused slots set to NULL
 */
static u_int channels_alloc[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/*
 * Maximum file descriptor value used in any of the channels.  This is
 * updated in channel_new.
 */
static int channel_max_fd[L7_OPENSSH_MAX_CONNECTIONS];


/* -- tcp forwarding */

/*
 * Data structure for storing which hosts are permitted for forward requests.
 * The local sides of any remote forwards are stored in this array to prevent
 * a corrupt remote server from accessing arbitrary TCP/IP ports on our local
 * network (which might be behind a firewall).
 */
typedef struct {
    char *host_to_connect;      /* Connect to 'host'. */
    u_short port_to_connect;    /* Connect to 'port'. */
    u_short listen_port;        /* Remote side should listen port number. */
} ForwardPermission;

/* List of all permitted host/port pairs to connect. */
static ForwardPermission permitted_opens[SSH_MAX_FORWARDS_PER_DIRECTION];

/* Number of permitted host/port pairs in the array. */
static int num_permitted_opens = 0;
/*
 * If this is true, all opens are permitted.  This is the case on the server
 * on which we have to trust the client anyway, and the user could do
 * anything after logging in anyway.
 */
static int all_opens_permitted = 0;


/* -- X11 forwarding */

/* Maximum number of fake X11 displays to try. */
#define MAX_DISPLAYS  1000

/* Saved X11 local (client) display. */
static char *x11_saved_display = NULL;

/* Saved X11 authentication protocol name. */
static char *x11_saved_proto = NULL;

/* Saved X11 authentication data.  This is the real data. */
static char *x11_saved_data = NULL;
static u_int x11_saved_data_len = 0;

/*
 * Fake X11 authentication data.  This is what the server will be sending us;
 * we should replace any occurrences of this by the real data.
 */
static char *x11_fake_data = NULL;
static u_int x11_fake_data_len;


/* -- agent forwarding */

#define NUM_SOCKS   10

/* AF_UNSPEC or AF_INET or AF_INET6 */
static int IPv4or6 = AF_UNSPEC;

/* helper */
static void port_open_helper(int cn, Channel *c, char *rtype);

/* -- channel core */

Channel *
channel_by_id(int cn, int id)
{
    Channel *c;

    if (id < 0 || (u_int)id >= channels_alloc[cn]) {
        logit("channel_by_id: %d: bad id", id);
        return NULL;
    }
    c = channels[cn][id];
    if (c == NULL) {
        logit("channel_by_id: %d: bad id: channel free", id);
        return NULL;
    }
    return c;
}

/*
 * Returns the channel if it is allowed to receive protocol messages.
 * Private channels, like listening sockets, may not receive messages.
 */
Channel *
channel_lookup(int cn, int id)
{
    Channel *c;

    if ((c = channel_by_id(cn, id)) == NULL)
        return (NULL);

    switch(c->type) {
    case SSH_CHANNEL_X11_OPEN:
    case SSH_CHANNEL_LARVAL:
    case SSH_CHANNEL_CONNECTING:
    case SSH_CHANNEL_DYNAMIC:
    case SSH_CHANNEL_OPENING:
    case SSH_CHANNEL_OPEN:
    case SSH_CHANNEL_INPUT_DRAINING:
    case SSH_CHANNEL_OUTPUT_DRAINING:
        return (c);
        break;
    }
    logit("Non-public channel %d, type %d.", id, c->type);
    return (NULL);
}

/*
 * Register filedescriptors for a channel, used when allocating a channel or
 * when the channel consumer/producer is ready, e.g. shell exec'd
 */

void
channel_register_fds(int cn, Channel *c, int rfd, int wfd, int efd,
    int extusage, int nonblock)
{
    /* Update the maximum file descriptor value. */
    channel_max_fd[cn] = MAX(channel_max_fd[cn], rfd);
    channel_max_fd[cn] = MAX(channel_max_fd[cn], wfd);
    channel_max_fd[cn] = MAX(channel_max_fd[cn], efd);

    /* XXX set close-on-exec -markus */

    c->rfd = rfd;
    c->wfd = wfd;
    c->sock = (rfd == wfd) ? rfd : -1;
    c->ctl_fd = -1; /* XXX: set elsewhere */
    c->efd = efd;
    c->extended_usage = extusage;

    /* XXX ugly hack: nonblock is only set by the server */
    if (nonblock && isatty(c->rfd)) {
        debug2("channel %d: rfd %d isatty", c->self, c->rfd);
        c->isatty = 1;
        if (!isatty(c->wfd)) {
            error("channel %d: wfd %d is not a tty?",
                c->self, c->wfd);
        }
    } else {
        c->isatty = 0;
    }
    c->wfd_isatty = isatty(c->wfd);

    /* enable nonblocking mode */
    if (nonblock) {
        if (rfd != -1)
            set_nonblock(rfd);
        if (wfd != -1)
            set_nonblock(wfd);
        if (efd != -1)
            set_nonblock(efd);
    }

    debug("channel %d: registered [%s], cn %d, rfd %d, wfd %d, efd %d, sock %d, ctl_fd %d",
          c->self, c->remote_name, c->cn, c->rfd, c->wfd, c->efd, c->sock, c->ctl_fd);
}

/*
 * Allocate a new channel object and set its type and socket. This will cause
 * remote_name to be freed.
 */

Channel *
channel_new(int cn, char *ctype, int type, int rfd, int wfd, int efd,
    u_int window, u_int maxpack, int extusage, char *remote_name, int nonblock)
{
    int found;
    u_int i;
    Channel *c;

    /* Do initial allocation if this is the first call. */
    if (channels_alloc[cn] == 0) {
        debug2("channels_alloc[%d]: %d, doing alloc", cn, channels_alloc[cn]);
#ifndef L7_SSHD
        channels_alloc[cn] = 10;
#else
        channels_alloc[cn] = 1;
#endif /* L7_SSHD */
        channels[cn] = xmalloc(channels_alloc[cn] * sizeof(Channel *));
        for (i = 0; i < channels_alloc[cn]; i++)
            channels[cn][i] = NULL;
    }
    /* Try to find a free slot where to put the new channel. */
    for (found = -1, i = 0; i < channels_alloc[cn]; i++)
        if (channels[cn][i] == NULL) {
            /* Found a free slot. */
            debug2("channels_alloc[%d]: %d, found free slot %d", cn, channels_alloc[cn], i);
            found = (int)i;
            break;
        }
    if (found < 0) {
        /* There are no free slots.  Take last+1 slot and expand the array.  */
        found = channels_alloc[cn];
#ifndef L7_SSHD
        if (channels_alloc[cn] > 10000)
            fatal("channel_new: internal error: channels_alloc[%d] %d "
                "too big.", cn, channels_alloc[cn]);
        channels[cn] = xrealloc(channels[cn],
            (channels_alloc[cn] + 10) * sizeof(Channel *));
        channels_alloc[cn] += 10;
        debug2("channels_alloc[%d]: expanding %d", cn, channels_alloc[cn]);
        for (i = found; i < channels_alloc[cn]; i++)
            channels[cn][i] = NULL;
#else
        if (channels_alloc[cn] > SSHD_MAX_CHANNELS)
        {
            fatal_cn(cn, "channel_new: internal error: channels_alloc[%d] %d "
                "too big.", cn, channels_alloc[cn]);
        }
        channels[cn] = xrealloc(channels[cn],
            (channels_alloc[cn] + 10) * sizeof(Channel *));
        channels_alloc[cn] += 10;
        debug2("channels_alloc[%d]: expanding %d", cn, channels_alloc[cn]);
        for (i = found; i < channels_alloc[cn]; i++)
            channels[cn][i] = NULL;
#endif /* L7_SSHD */
    }
    /* Initialize and return new channel. */
    c = channels[cn][found] = xmalloc(sizeof(Channel));
    memset(c, 0, sizeof(Channel));
    buffer_init(&c->input);
    buffer_init(&c->output);
    buffer_init(&c->extended);
    c->ostate = CHAN_OUTPUT_OPEN;
    c->istate = CHAN_INPUT_OPEN;
    c->flags = 0;
    c->remote_name = xstrdup(remote_name);
    c->cn = cn;
    channel_register_fds(cn, c, rfd, wfd, efd, extusage, nonblock);
    c->self = found;
    c->type = type;
    c->ctype = ctype;
    c->local_window = window;
    c->local_window_max = window;
    c->local_consumed = 0;
    c->local_maxpacket = maxpack;
    c->remote_id = -1;
    c->remote_window = 0;
    c->remote_maxpacket = 0;
    c->force_drain = 0;
    c->single_connection = 0;
    c->detach_user = NULL;
    c->detach_close = 0;
    c->confirm = NULL;
    c->confirm_ctx = NULL;
    c->input_filter = NULL;
    c->output_filter = NULL;
    debug("channel %d: new [%s], cn %d, rfd %d, wfd %d, efd %d, sock %d, ctl_fd %d",
          found, remote_name, c->cn, c->rfd, c->wfd, c->efd, c->sock, c->ctl_fd);
    return c;
}

int
channel_find_maxfd(int cn)
{
    u_int i;
    int max = 0;
    Channel *c;

    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c != NULL) {
            max = MAX(max, c->rfd);
            max = MAX(max, c->wfd);
            max = MAX(max, c->efd);
        }
    }
    return max;
}

int
channel_close_fd(Channel *c, int *fdp)
{
    int ret = 0, fd = *fdp;

    if (fd != -1) {
        debugl7(SYSLOG_LEVEL_OPEN, "closing fd %d", fd);
        ret = close(fd);
        *fdp = -1;
        if (fd == channel_max_fd[c->cn])
            channel_max_fd[c->cn] = channel_find_maxfd(c->cn);
    }
    return ret;
}

/* Close all channel fd/socket. */

void
channel_close_fds(Channel *c)
{
    int rfd = c->rfd;
    int wfd = c->wfd;
    int efd = c->efd;
    int sock = c->sock;
    int ctl_fd = c->ctl_fd;

    debug3("channel %d: close_fds sock %d r %d w %d e %d c %d",
        c->self, c->sock, c->rfd, c->wfd, c->efd, c->ctl_fd);

    channel_close_fd(c, &c->sock);

    if ((ctl_fd != sock) && (ctl_fd != -1))
    {
        channel_close_fd(c, &c->ctl_fd);
    }
    else
    {
        c->ctl_fd = -1;
    }

    if ((rfd != ctl_fd) && (rfd != sock) && (rfd != -1))
    {
        channel_close_fd(c, &c->rfd);
    }
    else
    {
        c->rfd = -1;
        channel_max_fd[c->cn] = channel_find_maxfd(c->cn);
    }

    if ((wfd != rfd) && (wfd != sock) && (wfd != ctl_fd) && (wfd != -1))
    {
        channel_close_fd(c, &c->wfd);
    }
    else
    {
        c->wfd = -1;
        channel_max_fd[c->cn] = channel_find_maxfd(c->cn);
    }

    if ((efd != rfd) && (efd != wfd) && (efd != sock) && (efd != ctl_fd) && (efd != -1))
    {
        channel_close_fd(c, &c->efd);
    }
    else
    {
        c->wfd = -1;
        channel_max_fd[c->cn] = channel_find_maxfd(c->cn);
    }
}

/* Free the channel and close its fd/socket. */

void
channel_free(Channel *c)
{
    char *s;
    u_int i, n, cn = c->cn;
#ifdef COMMENTED_OUT
    int self = c->self;
#endif /* COMMENTED_OUT */

    for (n = 0, i = 0; i < channels_alloc[cn]; i++)
        if (channels[cn][i])
            n++;
    debug("channel %d: free: %s, nchannels %u", c->self,
        c->remote_name ? c->remote_name : "???", n);

    s = channel_open_message(cn);
    debug3("channel %d: status: %s", c->self, s);
    xfree(s);

    if (c->sock != -1)
        shutdown(c->sock, SHUT_RDWR);
    if (c->ctl_fd != -1)
        shutdown(c->ctl_fd, SHUT_RDWR);
    channel_close_fds(c);
    buffer_free(&c->input);
    buffer_free(&c->output);
    buffer_free(&c->extended);
    if (c->remote_name) {
        xfree(c->remote_name);
        c->remote_name = NULL;
    }
    channels[cn][c->self] = NULL;
    xfree(c);
#ifndef L7_SSHD
    if (self != (channels_alloc[cn] - 1))
    {
        debugl7(SYSLOG_LEVEL_DEBUG1, "connection %d: moving channel %d to %d",
                channels_alloc[cn] - 1, self);
        channels[cn][self] = channels[cn][channels_alloc[cn]-1];
        channels[cn][channels_alloc[cn]-1] = NULL;
        channels[cn][self]->self = self;
    }
    if (channels_alloc[cn] > 0)
    {
      channels_alloc[cn]--;
      if (channels_alloc[cn] == 0)
      {
        xfree(channels[cn]);
      }
      else
      {
        channels[cn] = xrealloc(channels[cn], channels_alloc[cn] * sizeof(Channel *));
      }
    }
#endif /* L7_SSHD */
}

void
channel_free_all(void)
{
    u_int i, j;

    osapiSemaTake(sshdChannelSema, L7_WAIT_FOREVER);
    for (i = 0; i < L7_OPENSSH_MAX_CONNECTIONS; i++)
    {
        for (j = 0; j < channels_alloc[i]; j++)
        {
            if (channels[i][j] != NULL)
            {
                channel_free(channels[i][j]);
            }
        }

        if ((channels_alloc[i]) != 0 && (channels[i] != NULL))
        {
            xfree(channels[i]);
            channels[i] = NULL;
        }
    }
    osapiSemaGive(sshdChannelSema);
}

void
channel_free_cn(int cn)
{
    u_int i;

    for (i = 0; i < channels_alloc[cn]; i++)
    {
        if (channels[cn][i] != NULL)
        {
            channel_free(channels[cn][i]);
        }
    }

    if (channels[cn] != NULL)
    {
        xfree(channels[cn]);
        channels[cn] = NULL;
    }

    channels_alloc[cn] = 0;
}

/*
 * Closes the sockets/fds of all channels.  This is used to close extra file
 * descriptors after a fork.
 */

void
channel_close_all(void)
{
    u_int i, j;

    osapiSemaTake(sshdChannelSema, L7_WAIT_FOREVER);
    for (i = 0; i < L7_OPENSSH_MAX_CONNECTIONS; i++)
    {
        for (j = 0; j < channels_alloc[i]; j++)
        {
            if (channels[i][j] != NULL)
            {
                channel_close_fds(channels[i][j]);
            }
        }
    }
    osapiSemaTake(sshdChannelSema, L7_WAIT_FOREVER);
}

void
channel_close_cn(int cn)
{
    u_int i;

    for (i = 0; i < channels_alloc[cn]; i++)
    {
        if (channels[cn][i] != NULL)
        {
            channel_close_fds(channels[cn][i]);
        }
    }
}

int channel_find_remote_id(int cn)
{
    int id;
    Channel *c;

    if ((id = channel_find_open(cn)) != -1)
    {
        c = channels[cn][id];
        return c->remote_id;
    }
    return -1;
}

/*
 * Stop listening to channels.
 */

void
channel_stop_listening(void)
{
    u_int i, j;
    Channel *c;

    osapiSemaTake(sshdChannelSema, L7_WAIT_FOREVER);
    for (i = 0; i < L7_OPENSSH_MAX_CONNECTIONS; i++)
    {
        for (j = 0; j < channels_alloc[i]; j++)
        {
            c = channels[i][j];
            if (c != NULL) {
                switch (c->type) {
                case SSH_CHANNEL_AUTH_SOCKET:
                case SSH_CHANNEL_PORT_LISTENER:
                case SSH_CHANNEL_RPORT_LISTENER:
                case SSH_CHANNEL_X11_LISTENER:
                    channel_close_fd(c, &c->sock);
                    channel_free(c);
                    break;
                }
            }
        }
    }
    osapiSemaGive(sshdChannelSema);
}

void
channel_stop_listening_cn(int cn)
{
    u_int i;
    Channel *c;

    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c != NULL) {
            switch (c->type) {
            case SSH_CHANNEL_AUTH_SOCKET:
            case SSH_CHANNEL_PORT_LISTENER:
            case SSH_CHANNEL_RPORT_LISTENER:
            case SSH_CHANNEL_X11_LISTENER:
                channel_close_fd(c, &c->sock);
                channel_free(c);
                break;
            }
        }
    }
}

/*
 * Returns true if no channel has too much buffered data, and false if one or
 * more channel is overfull.
 */

int
channel_not_very_much_buffered_data(int cn)
{
    u_int i;
    Channel *c;

    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c != NULL && c->type == SSH_CHANNEL_OPEN) {
#if 0
            if (!compat20[cn] &&
                buffer_len(&c->input) > packet_get_maxsize(cn)) {
                debug2("channel %d: big input buffer %d",
                    c->self, buffer_len(&c->input));
                return 0;
            }
#endif
            if (buffer_len(&c->output) > packet_get_maxsize(cn)) {
                debug2("channel %d: big output buffer %u > %u",
                    c->self, buffer_len(&c->output),
                    packet_get_maxsize(cn));
                return 0;
            }
        }
    }
    return 1;
}

/* Returns true if any channel is still open. */

int
channel_still_open(int cn)
{
    u_int i;
    Channel *c;

    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c == NULL)
            continue;
        switch (c->type) {
        case SSH_CHANNEL_X11_LISTENER:
        case SSH_CHANNEL_PORT_LISTENER:
        case SSH_CHANNEL_RPORT_LISTENER:
        case SSH_CHANNEL_CLOSED:
        case SSH_CHANNEL_AUTH_SOCKET:
        case SSH_CHANNEL_DYNAMIC:
        case SSH_CHANNEL_CONNECTING:
        case SSH_CHANNEL_ZOMBIE:
            continue;
        case SSH_CHANNEL_LARVAL:
            if (!compat20[cn])
            {
                fatal_cn(cn, "cannot happen: SSH_CHANNEL_LARVAL");
            }
            continue;
        case SSH_CHANNEL_OPENING:
        case SSH_CHANNEL_OPEN:
        case SSH_CHANNEL_X11_OPEN:
            return 1;
        case SSH_CHANNEL_INPUT_DRAINING:
        case SSH_CHANNEL_OUTPUT_DRAINING:
            if (!compat13[cn])
            {
                fatal_cn(cn, "cannot happen: OUT_DRAIN");
            }
            return 1;
        default:
            fatal_cn(cn, "channel_still_open: bad channel type %d", c->type);
            /* NOTREACHED */
        }
    }
    return 0;
}

/* Returns the id of an open channel suitable for keepaliving */

int
channel_find_open(int cn)
{
    u_int i;
    Channel *c;

    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c == NULL || c->remote_id < 0)
            continue;
        switch (c->type) {
        case SSH_CHANNEL_CLOSED:
        case SSH_CHANNEL_DYNAMIC:
        case SSH_CHANNEL_X11_LISTENER:
        case SSH_CHANNEL_PORT_LISTENER:
        case SSH_CHANNEL_RPORT_LISTENER:
        case SSH_CHANNEL_OPENING:
        case SSH_CHANNEL_CONNECTING:
        case SSH_CHANNEL_ZOMBIE:
            continue;
        case SSH_CHANNEL_LARVAL:
        case SSH_CHANNEL_AUTH_SOCKET:
        case SSH_CHANNEL_OPEN:
        case SSH_CHANNEL_X11_OPEN:
            return i;
        case SSH_CHANNEL_INPUT_DRAINING:
        case SSH_CHANNEL_OUTPUT_DRAINING:
            if (!compat13[cn])
                fatal_cn(cn, "cannot happen: OUT_DRAIN");
            return i;
        default:
            fatal_cn(cn, "channel_find_open: bad channel type %d", c->type);
            /* NOTREACHED */
        }
    }
    return -1;
}


/*
 * Returns a message describing the currently open forwarded connections,
 * suitable for sending to the client.  The message contains crlf pairs for
 * newlines.
 */

char *
channel_open_message(int cn)
{
    Buffer buffer;
    Channel *c;
    char buf[1024], *cp;
    u_int i;

    buffer_init(&buffer);
    osapiSnprintf(buf, sizeof buf, "The following connections are open:\r\n");
    buffer_append(&buffer, buf, strlen(buf));
    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c == NULL)
            continue;
        switch (c->type) {
        case SSH_CHANNEL_X11_LISTENER:
        case SSH_CHANNEL_PORT_LISTENER:
        case SSH_CHANNEL_RPORT_LISTENER:
        case SSH_CHANNEL_CLOSED:
        case SSH_CHANNEL_AUTH_SOCKET:
        case SSH_CHANNEL_ZOMBIE:
            continue;
        case SSH_CHANNEL_LARVAL:
        case SSH_CHANNEL_OPENING:
        case SSH_CHANNEL_CONNECTING:
        case SSH_CHANNEL_DYNAMIC:
        case SSH_CHANNEL_OPEN:
        case SSH_CHANNEL_X11_OPEN:
        case SSH_CHANNEL_INPUT_DRAINING:
        case SSH_CHANNEL_OUTPUT_DRAINING:
            osapiSnprintf(buf, sizeof buf,
                "  #%d %.300s (t%d r%d i%d/%d o%d/%d fd %d/%d cfd %d)\r\n",
                c->self, c->remote_name,
                c->type, c->remote_id,
                c->istate, buffer_len(&c->input),
                c->ostate, buffer_len(&c->output),
                c->rfd, c->wfd, c->ctl_fd);
            buffer_append(&buffer, buf, strlen(buf));
            continue;
        default:
#ifndef L7_SSHD
            fatal("channel_open_message: bad channel type %d", c->type);
#else
            fatal_cn(cn, "connection %d: channel_open_message: bad channel type %d", cn, c->type);
#endif /* L7_SSHD */
            /* NOTREACHED */
        }
    }
    buffer_append(&buffer, "\0", 1);
    cp = xstrdup(buffer_ptr(&buffer));
    buffer_free(&buffer);
    return cp;
}

void
channel_send_open(int cn, int id)
{
    Channel *c = channel_lookup(cn, id);

    if (c == NULL) {
        logit("channel_send_open: %d: bad id", id);
        return;
    }
    debug2("channel %d: send open", id);
    packet_start(cn, SSH2_MSG_CHANNEL_OPEN);
    packet_put_cstring(cn, c->ctype);
    packet_put_int(cn, c->self);
    packet_put_int(cn, c->local_window);
    packet_put_int(cn, c->local_maxpacket);
    packet_send(cn);
}

void
channel_request_start(int cn, int id, char *service, int wantconfirm)
{
    Channel *c = channel_lookup(cn, id);

    if (c == NULL) {
        logit("channel_request_start: %d: unknown channel id", id);
        return;
    }
    debug2("channel %d: request %s confirm %d", id, service, wantconfirm);
    packet_start(cn, SSH2_MSG_CHANNEL_REQUEST);
    packet_put_int(cn, c->remote_id);
    packet_put_cstring(cn, service);
    packet_put_char(cn, wantconfirm);
}
void
channel_register_confirm(int cn, int id, channel_callback_fn *fn, void *ctx)
{
    Channel *c = channel_lookup(cn, id);

    if (c == NULL) {
        logit("channel_register_comfirm: %d: bad id", id);
        return;
    }
    c->confirm = fn;
    c->confirm_ctx = ctx;
}
void
channel_register_cleanup(int cn, int id, channel_callback_fn *fn, int do_close)
{
    Channel *c = channel_by_id(cn, id);

    if (c == NULL) {
        logit("channel_register_cleanup: %d: bad id", id);
        return;
    }
    c->detach_user = fn;
    c->detach_close = do_close;
}
void
channel_cancel_cleanup(int cn, int id)
{
    Channel *c = channel_by_id(cn, id);

    if (c == NULL) {
        logit("channel_cancel_cleanup: %d: bad id", id);
        return;
    }
    c->detach_user = NULL;
    c->detach_close = 0;
}
void
channel_register_filter(int cn, int id, channel_infilter_fn *ifn,
    channel_outfilter_fn *ofn)
{
    Channel *c = channel_lookup(cn, id);

    if (c == NULL) {
        logit("channel_register_filter: %d: bad id", id);
        return;
    }
    c->input_filter = ifn;
    c->output_filter = ofn;
}

void
channel_set_fds(int cn, int id, int rfd, int wfd, int efd,
    int extusage, int nonblock, u_int window_max)
{
    Channel *c = channel_lookup(cn, id);

    if (c == NULL || c->type != SSH_CHANNEL_LARVAL)
        fatal_cn(cn, "channel_activate for non-larval channel %d.", id);
    channel_register_fds(cn, c, rfd, wfd, efd, extusage, nonblock);
    c->type = SSH_CHANNEL_OPEN;
    c->local_window = c->local_window_max = window_max;
    packet_start(cn, SSH2_MSG_CHANNEL_WINDOW_ADJUST);
    packet_put_int(cn, c->remote_id);
    packet_put_int(cn, c->local_window);
    packet_send(cn);
}

/*
 * 'channel_pre*' are called just before select() to add any bits relevant to
 * channels in the select bitmasks.
 */
/*
 * 'channel_post*': perform any appropriate operations for channels which
 * have events pending.
 */
typedef void chan_fn(int cn, Channel *c, fd_set * readset, fd_set * writeset);
chan_fn *channel_pre[SSH_CHANNEL_MAX_TYPE];
chan_fn *channel_post[SSH_CHANNEL_MAX_TYPE];

void
channel_pre_listener(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    FD_SET(c->sock, readset);
}

void
channel_pre_connecting(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    debug3("channel %d: waiting for connection", c->self);
    FD_SET(c->sock, writeset);
}

void
channel_pre_open_13(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    if (buffer_len(&c->input) < packet_get_maxsize(cn))
        FD_SET(c->sock, readset);
    if (buffer_len(&c->output) > 0)
        FD_SET(c->sock, writeset);
}

void
channel_pre_open(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    u_int limit = compat20[cn] ? c->remote_window : packet_get_maxsize(cn);

    /* check buffer limits */
    limit = MIN(limit, (BUFFER_MAX_LEN - BUFFER_MAX_CHUNK - CHAN_RBUF));

    if (c->istate == CHAN_INPUT_OPEN &&
        limit > 0 &&
        buffer_len(&c->input) < limit)
        FD_SET(c->rfd, readset);
    if (c->ostate == CHAN_OUTPUT_OPEN ||
        c->ostate == CHAN_OUTPUT_WAIT_DRAIN) {
        if (buffer_len(&c->output) > 0) {
            FD_SET(c->wfd, writeset);
        } else if (c->ostate == CHAN_OUTPUT_WAIT_DRAIN) {
            if (CHANNEL_EFD_OUTPUT_ACTIVE(c, cn))
                debug2("channel %d: obuf_empty delayed efd %d/(%d)",
                    c->self, c->efd, buffer_len(&c->extended));
            else
                chan_obuf_empty(cn, c);
        }
    }
    /** XXX check close conditions, too */
    if (compat20[cn] && c->efd != -1) {
        if (c->extended_usage == CHAN_EXTENDED_WRITE &&
            buffer_len(&c->extended) > 0)
            FD_SET(c->efd, writeset);
        else if (!(c->flags & CHAN_EOF_SENT) &&
            c->extended_usage == CHAN_EXTENDED_READ &&
            buffer_len(&c->extended) < c->remote_window)
            FD_SET(c->efd, readset);
    }
    /* XXX: What about efd? races? */
    if (compat20[cn] && c->ctl_fd != -1 &&
        c->istate == CHAN_INPUT_OPEN && c->ostate == CHAN_OUTPUT_OPEN)
        FD_SET(c->ctl_fd, readset);
}

void
channel_pre_input_draining(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    if (buffer_len(&c->input) == 0) {
        packet_start(cn, SSH_MSG_CHANNEL_CLOSE);
        packet_put_int(cn, c->remote_id);
        packet_send(cn);
        c->type = SSH_CHANNEL_CLOSED;
        debug2("channel %d: closing after input drain.", c->self);
    }
}

void
channel_pre_output_draining(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    if (buffer_len(&c->output) == 0)
        chan_mark_dead(c);
    else
        FD_SET(c->sock, writeset);
}

/*
 * This is a special state for X11 authentication spoofing.  An opened X11
 * connection (when authentication spoofing is being done) remains in this
 * state until the first packet has been completely read.  The authentication
 * data in that packet is then substituted by the real data if it matches the
 * fake data, and the channel is put into normal mode.
 * XXX All this happens at the client side.
 * Returns: 0 = need more data, -1 = wrong cookie, 1 = ok
 */
int
x11_open_helper(Buffer *b)
{
    u_char *ucp;
    u_int proto_len, data_len;

    /* Check if the fixed size part of the packet is in buffer. */
    if (buffer_len(b) < 12)
        return 0;

    /* Parse the lengths of variable-length fields. */
    ucp = buffer_ptr(b);
    if (ucp[0] == 0x42) {   /* Byte order MSB first. */
        proto_len = 256 * ucp[6] + ucp[7];
        data_len = 256 * ucp[8] + ucp[9];
    } else if (ucp[0] == 0x6c) {    /* Byte order LSB first. */
        proto_len = ucp[6] + 256 * ucp[7];
        data_len = ucp[8] + 256 * ucp[9];
    } else {
        debug2("Initial X11 packet contains bad byte order byte: 0x%x",
            ucp[0]);
        return -1;
    }

    /* Check if the whole packet is in buffer. */
    if (buffer_len(b) <
        12 + ((proto_len + 3) & ~3) + ((data_len + 3) & ~3))
        return 0;

    /* Check if authentication protocol matches. */
    if (proto_len != strlen(x11_saved_proto) ||
        memcmp(ucp + 12, x11_saved_proto, proto_len) != 0) {
        debug2("X11 connection uses different authentication protocol.");
        return -1;
    }
    /* Check if authentication data matches our fake data. */
    if (data_len != x11_fake_data_len ||
        memcmp(ucp + 12 + ((proto_len + 3) & ~3),
        x11_fake_data, x11_fake_data_len) != 0) {
        debug2("X11 auth data does not match fake data.");
        return -1;
    }
    /* Check fake data length */
    if (x11_fake_data_len != x11_saved_data_len) {
        error("X11 fake_data_len %d != saved_data_len %d",
            x11_fake_data_len, x11_saved_data_len);
        return -1;
    }
    /*
     * Received authentication protocol and data match
     * our fake data. Substitute the fake data with real
     * data.
     */
    memcpy(ucp + 12 + ((proto_len + 3) & ~3),
        x11_saved_data, x11_saved_data_len);
    return 1;
}

void
channel_pre_x11_open_13(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    int ret = x11_open_helper(&c->output);

    if (ret == 1) {
        /* Start normal processing for the channel. */
        c->type = SSH_CHANNEL_OPEN;
        channel_pre_open_13(cn, c, readset, writeset);
    } else if (ret == -1) {
        /*
         * We have received an X11 connection that has bad
         * authentication information.
         */
        logit("X11 connection rejected because of wrong authentication.");
        buffer_clear(&c->input);
        buffer_clear(&c->output);
        channel_close_fd(c, &c->sock);
        c->sock = -1;
        c->type = SSH_CHANNEL_CLOSED;
        packet_start(cn, SSH_MSG_CHANNEL_CLOSE);
        packet_put_int(cn, c->remote_id);
        packet_send(cn);
    }
}

void
channel_pre_x11_open(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    int ret = x11_open_helper(&c->output);

    /* c->force_drain = 1; */

    if (ret == 1) {
        c->type = SSH_CHANNEL_OPEN;
        channel_pre_open(cn, c, readset, writeset);
    } else if (ret == -1) {
        logit("X11 connection rejected because of wrong authentication.");
        debug2("X11 rejected %d i%d/o%d", c->self, c->istate, c->ostate);
        chan_read_failed(c);
        buffer_clear(&c->input);
        chan_ibuf_empty(cn, c);
        buffer_clear(&c->output);
        /* for proto v1, the peer will send an IEOF */
        if (compat20[cn])
            chan_write_failed(cn, c);
        else
            c->type = SSH_CHANNEL_OPEN;
        debug2("X11 closed %d i%d/o%d", c->self, c->istate, c->ostate);
    }
}

/* try to decode a socks4 header */
int
channel_decode_socks4(Channel *c, fd_set * readset, fd_set * writeset)
{
    char *p, *host;
    u_int len, have, i, found;
    char username[256];
    struct {
        u_int8_t version;
        u_int8_t command;
        u_int16_t dest_port;
        struct in_addr dest_addr;
    } s4_req, s4_rsp;

    debug2("channel %d: decode socks4", c->self);

    have = buffer_len(&c->input);
    len = sizeof(s4_req);
    if (have < len)
        return 0;
    p = buffer_ptr(&c->input);
    for (found = 0, i = len; i < have; i++) {
        if (p[i] == '\0') {
            found = 1;
            break;
        }
        if (i > 1024) {
            /* the peer is probably sending garbage */
            debug("channel %d: decode socks4: too long",
                c->self);
            return -1;
        }
    }
    if (!found)
        return 0;
    buffer_get(&c->input, (char *)&s4_req.version, 1);
    buffer_get(&c->input, (char *)&s4_req.command, 1);
    buffer_get(&c->input, (char *)&s4_req.dest_port, 2);
    buffer_get(&c->input, (char *)&s4_req.dest_addr, 4);
    have = buffer_len(&c->input);
    p = buffer_ptr(&c->input);
    len = strlen(p);
    debug2("channel %d: decode socks4: user %s/%d", c->self, p, len);
    if (len > have)
        fatal("channel %d: decode socks4: len %d > have %d",
            c->self, len, have);
    strlcpy(username, p, sizeof(username));
    buffer_consume(&c->input, len);
    buffer_consume(&c->input, 1);       /* trailing '\0' */

    host = inet_ntoa(s4_req.dest_addr);
    strlcpy(c->path, host, sizeof(c->path));
    c->host_port = ntohs(s4_req.dest_port);

    debug2("channel %d: dynamic request: socks4 host %s port %u command %u",
        c->self, host, c->host_port, s4_req.command);

    if (s4_req.command != 1) {
        debug("channel %d: cannot handle: socks4 cn %d",
            c->self, s4_req.command);
        return -1;
    }
    s4_rsp.version = 0;         /* vn: 0 for reply */
    s4_rsp.command = 90;            /* cd: req granted */
    s4_rsp.dest_port = 0;           /* ignored */
    s4_rsp.dest_addr.s_addr = INADDR_ANY;   /* ignored */
    buffer_append(&c->output, (char *)&s4_rsp, sizeof(s4_rsp));
    return 1;
}

/* try to decode a socks5 header */
#define SSH_SOCKS5_AUTHDONE 0x1000
#define SSH_SOCKS5_NOAUTH   0x00
#define SSH_SOCKS5_IPV4     0x01
#define SSH_SOCKS5_DOMAIN   0x03
#define SSH_SOCKS5_IPV6     0x04
#define SSH_SOCKS5_CONNECT  0x01
#define SSH_SOCKS5_SUCCESS  0x00

int
channel_decode_socks5(Channel *c, fd_set * readset, fd_set * writeset)
{
    struct {
        u_int8_t version;
        u_int8_t command;
        u_int8_t reserved;
        u_int8_t atyp;
    } s5_req, s5_rsp;
    u_int16_t dest_port;
    u_char *p, dest_addr[255+1];
    u_int have, i, found, nmethods, addrlen, af;

    debug2("channel %d: decode socks5", c->self);
    p = buffer_ptr(&c->input);
    if (p[0] != 0x05)
        return -1;
    have = buffer_len(&c->input);
    if (!(c->flags & SSH_SOCKS5_AUTHDONE)) {
        /* format: ver | nmethods | methods */
        if (have < 2)
            return 0;
        nmethods = p[1];
        if (have < nmethods + 2)
            return 0;
        /* look for method: "NO AUTHENTICATION REQUIRED" */
        for (found = 0, i = 2 ; i < nmethods + 2; i++) {
            if (p[i] == SSH_SOCKS5_NOAUTH ) {
                found = 1;
                break;
            }
        }
        if (!found) {
            debug("channel %d: method SSH_SOCKS5_NOAUTH not found",
                c->self);
            return -1;
        }
        buffer_consume(&c->input, nmethods + 2);
        buffer_put_char(&c->output, 0x05);      /* version */
        buffer_put_char(&c->output, SSH_SOCKS5_NOAUTH); /* method */
        FD_SET(c->sock, writeset);
        c->flags |= SSH_SOCKS5_AUTHDONE;
        debug2("channel %d: socks5 auth done", c->self);
        return 0;               /* need more */
    }
    debug2("channel %d: socks5 post auth", c->self);
    if (have < sizeof(s5_req)+1)
        return 0;           /* need more */
    memcpy((char *)&s5_req, p, sizeof(s5_req));
    if (s5_req.version != 0x05 ||
        s5_req.command != SSH_SOCKS5_CONNECT ||
        s5_req.reserved != 0x00) {
        debug2("channel %d: only socks5 connect supported", c->self);
        return -1;
    }
    switch (s5_req.atyp){
    case SSH_SOCKS5_IPV4:
        addrlen = 4;
        af = AF_INET;
        break;
    case SSH_SOCKS5_DOMAIN:
        addrlen = p[sizeof(s5_req)];
        af = -1;
        break;
    case SSH_SOCKS5_IPV6:
        addrlen = 16;
        af = AF_INET6;
        break;
    default:
        debug2("channel %d: bad socks5 atyp %d", c->self, s5_req.atyp);
        return -1;
    }
    if (have < 4 + addrlen + 2)
        return 0;
    buffer_consume(&c->input, sizeof(s5_req));
    if (s5_req.atyp == SSH_SOCKS5_DOMAIN)
        buffer_consume(&c->input, 1);    /* host string length */
    buffer_get(&c->input, (char *)&dest_addr, addrlen);
    buffer_get(&c->input, (char *)&dest_port, 2);
    dest_addr[addrlen] = '\0';
    if (s5_req.atyp == SSH_SOCKS5_DOMAIN)
        strlcpy(c->path, (char *)dest_addr, sizeof(c->path));
    else if (inet_ntop(af, dest_addr, c->path, sizeof(c->path)) == NULL)
        return -1;
    c->host_port = ntohs(dest_port);

    debug2("channel %d: dynamic request: socks5 host %s port %u command %u",
        c->self, c->path, c->host_port, s5_req.command);

    s5_rsp.version = 0x05;
    s5_rsp.command = SSH_SOCKS5_SUCCESS;
    s5_rsp.reserved = 0;            /* ignored */
    s5_rsp.atyp = SSH_SOCKS5_IPV4;
    ((struct in_addr *)&dest_addr)->s_addr = INADDR_ANY;
    dest_port = 0;              /* ignored */

    buffer_append(&c->output, (char *)&s5_rsp, sizeof(s5_rsp));
    buffer_append(&c->output, (char *)&dest_addr, sizeof(struct in_addr));
    buffer_append(&c->output, (char *)&dest_port, sizeof(dest_port));
    return 1;
}

/* dynamic port forwarding */
void
channel_pre_dynamic(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    u_char *p;
    u_int have;
    int ret;

    have = buffer_len(&c->input);
    c->delayed = 0;
    debug2("channel %d: pre_dynamic: have %d", c->self, have);
    /* buffer_dump(&c->input); */
    /* check if the fixed size part of the packet is in buffer. */
    if (have < 3) {
        /* need more */
        FD_SET(c->sock, readset);
        return;
    }
    /* try to guess the protocol */
    p = buffer_ptr(&c->input);
    switch (p[0]) {
    case 0x04:
        ret = channel_decode_socks4(c, readset, writeset);
        break;
    case 0x05:
        ret = channel_decode_socks5(c, readset, writeset);
        break;
    default:
        ret = -1;
        break;
    }
    if (ret < 0) {
        chan_mark_dead(c);
    } else if (ret == 0) {
        debug2("channel %d: pre_dynamic: need more", c->self);
        /* need more */
        FD_SET(c->sock, readset);
    } else {
        /* switch to the next state */
        c->type = SSH_CHANNEL_OPENING;
        port_open_helper(cn, c, "direct-tcpip");
    }
}

/* This is our fake X11 server socket. */
void
channel_post_x11_listener(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    Channel *nc;
    struct sockaddr addr;
    int newsock;
    socklen_t addrlen;
    char buf[16384], *remote_ipaddr;
    int remote_port;

    if (FD_ISSET(c->sock, readset)) {
        debug("X11 connection requested.");
        addrlen = sizeof(addr);
        newsock = accept(c->sock, &addr, (int *)&addrlen);
        if (c->single_connection) {
            debug2("single_connection: closing X11 listener.");
            channel_close_fd(c, &c->sock);
            chan_mark_dead(c);
        }
        if (newsock < 0) {
            error("accept: %.100s", strerror(errno));
            return;
        }
        set_nodelay(newsock);
        remote_ipaddr = get_peer_ipaddr(newsock);
        remote_port = get_peer_port(newsock);
        osapiSnprintf(buf, sizeof buf, "X11 connection from %.200s port %d",
            remote_ipaddr, remote_port);

        nc = channel_new(cn, "accepted x11 socket",
            SSH_CHANNEL_OPENING, newsock, newsock, -1,
            c->local_window_max, c->local_maxpacket, 0, buf, 1);
        if (compat20[cn]) {
            packet_start(cn, SSH2_MSG_CHANNEL_OPEN);
            packet_put_cstring(cn, "x11");
            packet_put_int(cn, nc->self);
            packet_put_int(cn, nc->local_window_max);
            packet_put_int(cn, nc->local_maxpacket);
            /* originator ipaddr and port */
            packet_put_cstring(cn, remote_ipaddr);
            if (datafellows[cn] & SSH_BUG_X11FWD) {
                debug2("ssh2 x11 bug compat mode");
            } else {
                packet_put_int(cn, remote_port);
            }
            packet_send(cn);
        } else {
            packet_start(cn, SSH_SMSG_X11_OPEN);
            packet_put_int(cn, nc->self);
            if (packet_get_protocol_flags(cn) &
                SSH_PROTOFLAG_HOST_IN_FWD_OPEN)
                packet_put_cstring(cn, buf);
            packet_send(cn);
        }
        xfree(remote_ipaddr);
    }
}

void
port_open_helper(int cn, Channel *c, char *rtype)
{
    int direct;
    char buf[1024];
    char *remote_ipaddr = get_peer_ipaddr(c->sock);
    int remote_port = get_peer_port(c->sock);

    direct = (strcmp(rtype, "direct-tcpip") == 0);

    osapiSnprintf(buf, sizeof buf,
        "%s: listening port %d for %.100s port %d, "
        "connect from %.200s port %d",
        rtype, c->listening_port, c->path, c->host_port,
        remote_ipaddr, remote_port);

    xfree(c->remote_name);
    c->remote_name = xstrdup(buf);

    if (compat20[cn]) {
        packet_start(cn, SSH2_MSG_CHANNEL_OPEN);
        packet_put_cstring(cn, rtype);
        packet_put_int(cn, c->self);
        packet_put_int(cn, c->local_window_max);
        packet_put_int(cn, c->local_maxpacket);
        if (direct) {
            /* target host, port */
            packet_put_cstring(cn, c->path);
            packet_put_int(cn, c->host_port);
        } else {
            /* listen address, port */
            packet_put_cstring(cn, c->path);
            packet_put_int(cn, c->listening_port);
        }
        /* originator host and port */
        packet_put_cstring(cn, remote_ipaddr);
        packet_put_int(cn, (u_int)remote_port);
        packet_send(cn);
    } else {
        packet_start(cn, SSH_MSG_PORT_OPEN);
        packet_put_int(cn, c->self);
        packet_put_cstring(cn, c->path);
        packet_put_int(cn, c->host_port);
        if (packet_get_protocol_flags(cn) &
            SSH_PROTOFLAG_HOST_IN_FWD_OPEN)
            packet_put_cstring(cn, c->remote_name);
        packet_send(cn);
    }
    xfree(remote_ipaddr);
}

void
channel_set_reuseaddr(int fd)
{
    int on = 1;

    /*
     * Set socket options.
     * Allow local port reuse in TIME_WAIT.
     */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on)) == -1)
        error("setsockopt SO_REUSEADDR fd %d: %s", fd, strerror(errno));
}

/*
 * This socket is listening for connections to a forwarded TCP/IP port.
 */
void
channel_post_port_listener(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    Channel *nc;
    struct sockaddr addr;
    int newsock, nextstate;
    socklen_t addrlen;
    char *rtype;

    if (FD_ISSET(c->sock, readset)) {
        debug("Connection to port %d forwarding "
            "to %.100s port %d requested.",
            c->listening_port, c->path, c->host_port);

        if (c->type == SSH_CHANNEL_RPORT_LISTENER) {
            nextstate = SSH_CHANNEL_OPENING;
            rtype = "forwarded-tcpip";
        } else {
            if (c->host_port == 0) {
                nextstate = SSH_CHANNEL_DYNAMIC;
                rtype = "dynamic-tcpip";
            } else {
                nextstate = SSH_CHANNEL_OPENING;
                rtype = "direct-tcpip";
            }
        }

        addrlen = sizeof(addr);
        newsock = accept(c->sock, &addr, (int *) &addrlen);
        if (newsock < 0) {
            error("accept: %.100s", strerror(errno));
            return;
        }
        set_nodelay(newsock);
        nc = channel_new(cn, rtype, nextstate, newsock, newsock, -1,
            c->local_window_max, c->local_maxpacket, 0, rtype, 1);
        nc->listening_port = c->listening_port;
        nc->host_port = c->host_port;
        strlcpy(nc->path, c->path, sizeof(nc->path));

        if (nextstate == SSH_CHANNEL_DYNAMIC) {
            /*
             * do not call the channel_post handler until
             * this flag has been reset by a pre-handler.
             * otherwise the FD_ISSET calls might overflow
             */
            nc->delayed = 1;
        } else {
            port_open_helper(cn, nc, rtype);
        }
    }
}

/*
 * This is the authentication agent socket listening for connections from
 * clients.
 */
void
channel_post_auth_listener(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    Channel *nc;
    int newsock;
    struct sockaddr addr;
    socklen_t addrlen;

    if (FD_ISSET(c->sock, readset)) {
        addrlen = sizeof(addr);
        newsock = accept(c->sock, &addr, (int *) &addrlen);
        if (newsock < 0) {
            error("accept from auth socket: %.100s", strerror(errno));
            return;
        }
        nc = channel_new(cn, "accepted auth socket",
            SSH_CHANNEL_OPENING, newsock, newsock, -1,
            c->local_window_max, c->local_maxpacket,
            0, "accepted auth socket", 1);
        if (compat20[cn]) {
            packet_start(cn, SSH2_MSG_CHANNEL_OPEN);
            packet_put_cstring(cn, "auth-agent@openssh.com");
            packet_put_int(cn, nc->self);
            packet_put_int(cn, c->local_window_max);
            packet_put_int(cn, c->local_maxpacket);
        } else {
            packet_start(cn, SSH_SMSG_AGENT_OPEN);
            packet_put_int(cn, nc->self);
        }
        packet_send(cn);
    }
}

void
channel_post_connecting(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    int err = 0;
    socklen_t sz = sizeof(err);

    if (FD_ISSET(c->sock, writeset)) {
        if (getsockopt(c->sock, SOL_SOCKET, SO_ERROR, (char*)&err, (int *) &sz) < 0) {
            err = errno;
            error("getsockopt SO_ERROR failed");
        }
        if (err == 0) {
            debug("channel %d: connected", c->self);
            c->type = SSH_CHANNEL_OPEN;
            if (compat20[cn]) {
                packet_start(cn, SSH2_MSG_CHANNEL_OPEN_CONFIRMATION);
                packet_put_int(cn, c->remote_id);
                packet_put_int(cn, c->self);
                packet_put_int(cn, c->local_window);
                packet_put_int(cn, c->local_maxpacket);
            } else {
                packet_start(cn, SSH_MSG_CHANNEL_OPEN_CONFIRMATION);
                packet_put_int(cn, c->remote_id);
                packet_put_int(cn, c->self);
            }
        } else {
            debug("channel %d: not connected: %s",
                c->self, strerror(err));
            if (compat20[cn]) {
                packet_start(cn, SSH2_MSG_CHANNEL_OPEN_FAILURE);
                packet_put_int(cn, c->remote_id);
                packet_put_int(cn, SSH2_OPEN_CONNECT_FAILED);
                if (!(datafellows[cn] & SSH_BUG_OPENFAILURE)) {
                    packet_put_cstring(cn, strerror(err));
                    packet_put_cstring(cn, "");
                }
            } else {
                packet_start(cn, SSH_MSG_CHANNEL_OPEN_FAILURE);
                packet_put_int(cn, c->remote_id);
            }
            chan_mark_dead(c);
        }
        packet_send(cn);
    }
}

int
channel_handle_rfd(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    char buf[CHAN_RBUF];
    int len;

    if (c->rfd != -1 &&
        FD_ISSET(c->rfd, readset)) {
        len = read(c->rfd, buf, sizeof(buf));
        debugl7(SYSLOG_LEVEL_READ, "handle_rfd read c->rfd %d, %d bytes", c->rfd, len);
        if (len < 0 && (errno == EINTR || errno == EAGAIN))
            return 1;
        if (len <= 0) {
            debug2("channel %d: read<=0 rfd %d len %d",
                c->self, c->rfd, len);
            if (c->type != SSH_CHANNEL_OPEN) {
                debug2("channel %d: not open", c->self);
                chan_mark_dead(c);
                return -1;
            } else if (compat13[cn]) {
                buffer_clear(&c->output);
                c->type = SSH_CHANNEL_INPUT_DRAINING;
                debug2("channel %d: input draining.", c->self);
            } else {
                chan_read_failed(c);
            }
            return -1;
        }
        if (c->input_filter != NULL) {
            if (c->input_filter(cn, c, buf, len) == -1) {
                debug2("channel %d: filter stops", c->self);
                chan_read_failed(c);
            }
        } else if (c->datagram) {
            buffer_put_string(&c->input, buf, len);
        } else {
            buffer_append(&c->input, buf, len);
        }
    }
    return 1;
}
int
channel_handle_wfd(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
#ifndef L7_SSHD
    struct termios tio;
#endif /* L7_SSHD */
    u_char *data = NULL, *buf;
    u_int dlen;
    int len;

    /* Send buffered output data to the socket. */
    if (c->wfd != -1 &&
        FD_ISSET(c->wfd, writeset) &&
        buffer_len(&c->output) > 0) {
        if (c->output_filter != NULL) {
            if ((buf = c->output_filter(cn, c, &data, &dlen)) == NULL) {
                debug2("channel %d: filter stops", c->self);
                if (c->type != SSH_CHANNEL_OPEN)
                    chan_mark_dead(c);
                else
                    chan_write_failed(cn, c);
                return -1;
            }
        } else if (c->datagram) {
            buf = data = buffer_get_string(&c->output, &dlen);
        } else {
            buf = data = buffer_ptr(&c->output);
            dlen = buffer_len(&c->output);
        }

        if (c->datagram) {
            /* ignore truncated writes, datagrams might get lost */
            c->local_consumed += dlen + 4;
            len = write(c->wfd, buf, dlen);
            debugl7(SYSLOG_LEVEL_OPEN, "Wrote datagram to c->wfd %d, %d of %d bytes", c->wfd, len, dlen);
            xfree(data);
            if (len < 0 && (errno == EINTR || errno == EAGAIN))
                return 1;
            if (len <= 0) {
                if (c->type != SSH_CHANNEL_OPEN)
                    chan_mark_dead(c);
                else
                    chan_write_failed(cn, c);
                return -1;
            }
            return 1;
        }
#ifdef _AIX
        /* XXX: Later AIX versions can't push as much data to tty */
        if (compat20[cn] && c->wfd_isatty)
            dlen = MIN(dlen, 8*1024);
#endif

#ifdef COMMENTED_OUT
        len = write(c->wfd, buf, dlen);
#else
        len = atomicio(vwrite, c->wfd, buf, dlen);
#endif /* COMMENTED_OUT */
        debugl7(SYSLOG_LEVEL_WRITE, "Wrote data to c->wfd %d, %d of %d bytes", c->wfd, len, dlen);
        if (len < 0 && (errno == EINTR || errno == EAGAIN))
            return 1;
        if (len <= 0) {
            if (c->type != SSH_CHANNEL_OPEN) {
                debug2("channel %d: not open", c->self);
                chan_mark_dead(c);
                return -1;
            } else if (compat13[cn]) {
                buffer_clear(&c->output);
                debug2("channel %d: input draining.", c->self);
                c->type = SSH_CHANNEL_INPUT_DRAINING;
            } else {
                chan_write_failed(cn, c);
            }
            return -1;
        }
#if (L7_SERIAL_COM_ATTR)
#ifndef L7_SSHD
        if (compat20[cn] && c->isatty && dlen >= 1 && buf[0] != '\r') {
            if (tcgetattr(c->wfd, &tio) == 0 &&
                !(tio.c_lflag & ECHO) && (tio.c_lflag & ICANON)) {
                /*
                 * Simulate echo to reduce the impact of
                 * traffic analysis. We need to match the
                 * size of a SSH2_MSG_CHANNEL_DATA message
                 * (4 byte channel id + buf)
                 */
                packet_send_ignore(cn, 4 + len);
                packet_send(cn);
            }
        }
#endif /* L7_SSHD */
#endif
        buffer_consume(&c->output, len);
        if (compat20[cn] && len > 0) {
            c->local_consumed += len;
        }
    }
    return 1;
}
int
channel_handle_efd(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    char buf[CHAN_RBUF];
    int len;

/** XXX handle drain efd, too */
    if (c->efd != -1) {
        if (c->extended_usage == CHAN_EXTENDED_WRITE &&
            FD_ISSET(c->efd, writeset) &&
            buffer_len(&c->extended) > 0) {
            len = write(c->efd, buffer_ptr(&c->extended),
                buffer_len(&c->extended));
            debug2("channel %d: written %d to efd %d",
                c->self, len, c->efd);
            if (len < 0 && (errno == EINTR || errno == EAGAIN))
                return 1;
            if (len <= 0) {
                debug2("channel %d: closing write-efd %d",
                    c->self, c->efd);
                channel_close_fd(c, &c->efd);
            } else {
                buffer_consume(&c->extended, len);
                c->local_consumed += len;
            }
        } else if (c->extended_usage == CHAN_EXTENDED_READ &&
            FD_ISSET(c->efd, readset)) {
            len = read(c->efd, buf, sizeof(buf));
            debug2("channel %d: read %d from efd %d",
                c->self, len, c->efd);
            if (len < 0 && (errno == EINTR || errno == EAGAIN))
                return 1;
            if (len <= 0) {
                debug2("channel %d: closing read-efd %d",
                    c->self, c->efd);
                channel_close_fd(c, &c->efd);
            } else {
                buffer_append(&c->extended, buf, len);
            }
        }
    }
    return 1;
}
int
channel_handle_ctl(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    char buf[16];
    int len;

    /* Monitor control fd to detect if the slave client exits */
    if (c->ctl_fd != -1 && FD_ISSET(c->ctl_fd, readset)) {
        len = read(c->ctl_fd, buf, sizeof(buf));
        debugl7(SYSLOG_LEVEL_READ, "handle_ctl read c->ctl_fd %d, %d bytes", c->ctl_fd, len);
        if (len < 0 && (errno == EINTR || errno == EAGAIN))
            return 1;
        if (len <= 0) {
            debug2("channel %d: ctl read<=0", c->self);
            if (c->type != SSH_CHANNEL_OPEN) {
                debug2("channel %d: not open", c->self);
                chan_mark_dead(c);
                return -1;
            } else {
                chan_read_failed(c);
                chan_write_failed(cn, c);
            }
            return -1;
        } else
            fatal_cn(cn, "%s: unexpected data on ctl fd", __func__);
    }
    return 1;
}
int
channel_check_window(int cn, Channel *c)
{
    if (c->type == SSH_CHANNEL_OPEN &&
        !(c->flags & (CHAN_CLOSE_SENT|CHAN_CLOSE_RCVD)) &&
        c->local_window < c->local_window_max/2 &&
        c->local_consumed > 0) {
        packet_start(cn, SSH2_MSG_CHANNEL_WINDOW_ADJUST);
        packet_put_int(cn, c->remote_id);
        packet_put_int(cn, c->local_consumed);
        packet_send(cn);
        debug2("channel %d: window %d sent adjust %d",
            c->self, c->local_window,
            c->local_consumed);
        c->local_window += c->local_consumed;
        c->local_consumed = 0;
    }
    return 1;
}

void
channel_post_open(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    int rc;

    if (c->delayed)
        return;
    rc = channel_handle_rfd(cn, c, readset, writeset);
    if (rc == -1)
    {
        debugl7(SYSLOG_LEVEL_READ, "connection[%d] %d read failed, closing ...", cn, c->rfd);
        packet_exit(cn);
        /*
        cleanup_exit_cn(cn, 255);
        */
    }
    rc = channel_handle_wfd(cn, c, readset, writeset);
    if (rc == -1)
    {
        debugl7(SYSLOG_LEVEL_READ, "connection[%d] %d write failed, closing ...", cn, c->wfd);
        packet_exit(cn);
        /*
        cleanup_exit_cn(cn, 255);
        */
    }
    if (!compat20[cn])
        return;
    channel_handle_efd(cn, c, readset, writeset);
    rc = channel_handle_ctl(cn, c, readset, writeset);
    if (rc == -1)
    {
        debugl7(SYSLOG_LEVEL_READ, "connection[%d] %d read failed, closing ...", cn, c->ctl_fd);
        packet_exit(cn);
        /*
        cleanup_exit_cn(cn, 255);
        */
    }
    channel_check_window(cn, c);
}

void
channel_post_output_drain_13(int cn, Channel *c, fd_set * readset, fd_set * writeset)
{
    int len;

    /* Send buffered output data to the socket. */
    if (FD_ISSET(c->sock, writeset) && buffer_len(&c->output) > 0) {
        len = write(c->sock, buffer_ptr(&c->output),
                buffer_len(&c->output));
        if (len <= 0)
            buffer_clear(&c->output);
        else
            buffer_consume(&c->output, len);
    }
}

void
channel_handler_init_20(void)
{
    channel_pre[SSH_CHANNEL_OPEN] =         &channel_pre_open;
    channel_pre[SSH_CHANNEL_X11_OPEN] =     &channel_pre_x11_open;
    channel_pre[SSH_CHANNEL_PORT_LISTENER] =    &channel_pre_listener;
    channel_pre[SSH_CHANNEL_RPORT_LISTENER] =   &channel_pre_listener;
    channel_pre[SSH_CHANNEL_X11_LISTENER] =     &channel_pre_listener;
    channel_pre[SSH_CHANNEL_AUTH_SOCKET] =      &channel_pre_listener;
    channel_pre[SSH_CHANNEL_CONNECTING] =       &channel_pre_connecting;
    channel_pre[SSH_CHANNEL_DYNAMIC] =      &channel_pre_dynamic;

    channel_post[SSH_CHANNEL_OPEN] =        &channel_post_open;
    channel_post[SSH_CHANNEL_PORT_LISTENER] =   &channel_post_port_listener;
    channel_post[SSH_CHANNEL_RPORT_LISTENER] =  &channel_post_port_listener;
    channel_post[SSH_CHANNEL_X11_LISTENER] =    &channel_post_x11_listener;
    channel_post[SSH_CHANNEL_AUTH_SOCKET] =     &channel_post_auth_listener;
    channel_post[SSH_CHANNEL_CONNECTING] =      &channel_post_connecting;
    channel_post[SSH_CHANNEL_DYNAMIC] =     &channel_post_open;
}

void
channel_handler_init_13(void)
{
    channel_pre[SSH_CHANNEL_OPEN] =         &channel_pre_open_13;
    channel_pre[SSH_CHANNEL_X11_OPEN] =     &channel_pre_x11_open_13;
    channel_pre[SSH_CHANNEL_X11_LISTENER] =     &channel_pre_listener;
    channel_pre[SSH_CHANNEL_PORT_LISTENER] =    &channel_pre_listener;
    channel_pre[SSH_CHANNEL_AUTH_SOCKET] =      &channel_pre_listener;
    channel_pre[SSH_CHANNEL_INPUT_DRAINING] =   &channel_pre_input_draining;
    channel_pre[SSH_CHANNEL_OUTPUT_DRAINING] =  &channel_pre_output_draining;
    channel_pre[SSH_CHANNEL_CONNECTING] =       &channel_pre_connecting;
    channel_pre[SSH_CHANNEL_DYNAMIC] =      &channel_pre_dynamic;

    channel_post[SSH_CHANNEL_OPEN] =        &channel_post_open;
    channel_post[SSH_CHANNEL_X11_LISTENER] =    &channel_post_x11_listener;
    channel_post[SSH_CHANNEL_PORT_LISTENER] =   &channel_post_port_listener;
    channel_post[SSH_CHANNEL_AUTH_SOCKET] =     &channel_post_auth_listener;
    channel_post[SSH_CHANNEL_OUTPUT_DRAINING] = &channel_post_output_drain_13;
    channel_post[SSH_CHANNEL_CONNECTING] =      &channel_post_connecting;
    channel_post[SSH_CHANNEL_DYNAMIC] =     &channel_post_open;
}

void
channel_handler_init_15(void)
{
    channel_pre[SSH_CHANNEL_OPEN] =         &channel_pre_open;
    channel_pre[SSH_CHANNEL_X11_OPEN] =     &channel_pre_x11_open;
    channel_pre[SSH_CHANNEL_X11_LISTENER] =     &channel_pre_listener;
    channel_pre[SSH_CHANNEL_PORT_LISTENER] =    &channel_pre_listener;
    channel_pre[SSH_CHANNEL_AUTH_SOCKET] =      &channel_pre_listener;
    channel_pre[SSH_CHANNEL_CONNECTING] =       &channel_pre_connecting;
    channel_pre[SSH_CHANNEL_DYNAMIC] =      &channel_pre_dynamic;

    channel_post[SSH_CHANNEL_X11_LISTENER] =    &channel_post_x11_listener;
    channel_post[SSH_CHANNEL_PORT_LISTENER] =   &channel_post_port_listener;
    channel_post[SSH_CHANNEL_AUTH_SOCKET] =     &channel_post_auth_listener;
    channel_post[SSH_CHANNEL_OPEN] =        &channel_post_open;
    channel_post[SSH_CHANNEL_CONNECTING] =      &channel_post_connecting;
    channel_post[SSH_CHANNEL_DYNAMIC] =     &channel_post_open;
}

void
channel_handler_init(int cn)
{
    int i;

    for (i = 0; i < SSH_CHANNEL_MAX_TYPE; i++) {
        channel_pre[i] = NULL;
        channel_post[i] = NULL;
    }
    if (compat20[cn])
        channel_handler_init_20();
    else if (compat13[cn])
        channel_handler_init_13();
    else
        channel_handler_init_15();
}

/* gc dead channels */
void
channel_garbage_collect(int cn, Channel *c)
{
    if (c == NULL)
        return;
    if (c->detach_user != NULL) {
        if (!chan_is_dead(cn, c, c->detach_close))
            return;
        debug2("channel %d: gc: notify user", c->self);
        c->detach_user(cn, c->self, NULL);
        /* if we still have a callback */
        if (c->detach_user != NULL)
            return;
        debug2("channel %d: gc: user detached", c->self);
    }
    if (!chan_is_dead(cn, c, 1))
        return;
    debug2("channel %d: garbage collecting", c->self);
    channel_free(c);
}

void
channel_handler(int cn, chan_fn *ftab[], fd_set * readset, fd_set * writeset)
{
    static int did_init = 0;
    u_int i;
    Channel *c;

    if (!did_init) {
        channel_handler_init(cn);
        did_init = 1;
    }
    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c == NULL)
            continue;
        if (c->cn != cn)
        {
            debugl7(SYSLOG_LEVEL_ERROR, "wrong channel number %d on channel %d!", c->cn, cn);
            continue;
        }
        if (ftab[c->type] != NULL)
            (*ftab[c->type])(cn, c, readset, writeset);
        channel_garbage_collect(cn, c);
    }
}

/*
 * Allocate/update select bitmasks and add any bits relevant to channels in
 * select bitmasks.
 */
void
channel_prepare_select(int cn, fd_set *readsetp, fd_set *writesetp, int *maxfdp,
    u_int *nallocp, int rekeying)
{
#ifdef COMMENTED_OUT
    u_int n, sz;

    n = MAX(*maxfdp, channel_max_fd[cn]);

    sz = howmany(n+1, NFDBITS) * sizeof(fd_mask);
    /* perhaps check sz < nalloc/2 and shrink? */
    if (*readsetp == NULL || sz > *nallocp) {
        *readsetp = xrealloc(*readsetp, sz);
        *writesetp = xrealloc(*writesetp, sz);
        *nallocp = sz;
    }
    *maxfdp = n;
    memset(*readsetp, 0, sz);
    memset(*writesetp, 0, sz);
#endif /* COMMENTED_OUT */
    u_int n = MAX(*maxfdp, channel_max_fd[cn]);

    *maxfdp = n;
    memset(readsetp, 0, sizeof(fd_set));
    memset(writesetp, 0, sizeof(fd_set));

    if (!rekeying)
        channel_handler(cn, channel_pre, readsetp, writesetp);
}

/*
 * After select, perform any appropriate operations for channels which have
 * events pending.
 */
void
channel_after_select(int cn, fd_set *readset, fd_set *writeset)
{
    channel_handler(cn, channel_post, readset, writeset);
}


/* If there is data to send to the connection, enqueue some of it now. */

void
channel_output_poll(int cn)
{
    Channel *c;
    u_int i, len;

    for (i = 0; i < channels_alloc[cn]; i++) {
        c = channels[cn][i];
        if (c == NULL)
            continue;

        /*
         * We are only interested in channels that can have buffered
         * incoming data.
         */
        if (compat13[cn]) {
            if (c->type != SSH_CHANNEL_OPEN &&
                c->type != SSH_CHANNEL_INPUT_DRAINING)
                continue;
        } else {
            if (c->type != SSH_CHANNEL_OPEN)
                continue;
        }
        if (compat20[cn] &&
            (c->flags & (CHAN_CLOSE_SENT|CHAN_CLOSE_RCVD))) {
            /* XXX is this true? */
            debug3("channel %d: will not send data after close", c->self);
            continue;
        }

        /* Get the amount of buffered data for this channel. */
        if ((c->istate == CHAN_INPUT_OPEN ||
            c->istate == CHAN_INPUT_WAIT_DRAIN) &&
            (len = buffer_len(&c->input)) > 0) {
            if (c->datagram) {
                if (len > 0) {
                    u_char *data;
                    u_int dlen;

                    data = buffer_get_string(&c->input,
                        &dlen);
                    packet_start(cn, SSH2_MSG_CHANNEL_DATA);
                    packet_put_int(cn, c->remote_id);
                    packet_put_string(cn, data, dlen);
                    packet_send(cn);
                    c->remote_window -= dlen + 4;
                    xfree(data);
                }
                continue;
            }
            /*
             * Send some data for the other side over the secure
             * connection.
             */
            if (compat20[cn]) {
                if (len > c->remote_window)
                    len = c->remote_window;
                if (len > c->remote_maxpacket)
                    len = c->remote_maxpacket;
            } else {
                if (packet_is_interactive(cn)) {
                    if (len > 1024)
                        len = 512;
                } else {
                    /* Keep the packets at reasonable size. */
                    if (len > packet_get_maxsize(cn)/2)
                        len = packet_get_maxsize(cn)/2;
                }
            }
            if (len > 0) {
                packet_start(cn, compat20[cn] ?
                    SSH2_MSG_CHANNEL_DATA : SSH_MSG_CHANNEL_DATA);
                packet_put_int(cn, c->remote_id);
                packet_put_string(cn, buffer_ptr(&c->input), len);
                packet_send(cn);
                buffer_consume(&c->input, len);
                c->remote_window -= len;
            }
        } else if (c->istate == CHAN_INPUT_WAIT_DRAIN) {
            if (compat13[cn])
                fatal_cn(cn, "cannot happen: istate == INPUT_WAIT_DRAIN for proto 1.3");
            /*
             * input-buffer is empty and read-socket shutdown:
             * tell peer, that we will not send more data: send IEOF.
             * hack for extended data: delay EOF if EFD still in use.
             */
            if (CHANNEL_EFD_INPUT_ACTIVE(c, cn))
                debug2("channel %d: ibuf_empty delayed efd %d/(%d)",
                    c->self, c->efd, buffer_len(&c->extended));
            else
                chan_ibuf_empty(cn, c);
        }
        /* Send extended data, i.e. stderr */
        if (compat20[cn] &&
            !(c->flags & CHAN_EOF_SENT) &&
            c->remote_window > 0 &&
            (len = buffer_len(&c->extended)) > 0 &&
            c->extended_usage == CHAN_EXTENDED_READ) {
            debug2("channel %d: rwin %u elen %u euse %d",
                c->self, c->remote_window, buffer_len(&c->extended),
                c->extended_usage);
            if (len > c->remote_window)
                len = c->remote_window;
            if (len > c->remote_maxpacket)
                len = c->remote_maxpacket;
            packet_start(cn, SSH2_MSG_CHANNEL_EXTENDED_DATA);
            packet_put_int(cn, c->remote_id);
            packet_put_int(cn, SSH2_EXTENDED_DATA_STDERR);
            packet_put_string(cn, buffer_ptr(&c->extended), len);
            packet_send(cn);
            buffer_consume(&c->extended, len);
            c->remote_window -= len;
            debug2("channel %d: sent ext data %d", c->self, len);
        }
    }
}


/* -- protocol input */

void
channel_input_data(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id;
    char *data;
    u_int data_len;
    Channel *c;

    /* Get the channel number and verify it. */
    id = packet_get_int(cn);
    c = channel_lookup(cn, id);
    if (c == NULL)
        packet_disconnect(cn, "Received data for nonexistent channel %d.", id);

    /* Ignore any data for non-open channels (might happen on close) */
    if (c->type != SSH_CHANNEL_OPEN &&
        c->type != SSH_CHANNEL_X11_OPEN)
        return;

    /* Get the data. */
    data = packet_get_string(cn, &data_len);

    /*
     * Ignore data for protocol > 1.3 if output end is no longer open.
     * For protocol 2 the sending side is reducing its window as it sends
     * data, so we must 'fake' consumption of the data in order to ensure
     * that window updates are sent back.  Otherwise the connection might
     * deadlock.
     */
    if (!compat13[cn] && c->ostate != CHAN_OUTPUT_OPEN) {
        if (compat20[cn]) {
            c->local_window -= data_len;
            c->local_consumed += data_len;
        }
        xfree(data);
        return;
    }

    if (compat20[cn]) {
        if (data_len > c->local_maxpacket) {
            logit("channel %d: rcvd big packet %d, maxpack %d",
                c->self, data_len, c->local_maxpacket);
        }
        if (data_len > c->local_window) {
            logit("channel %d: rcvd too much data %d, win %d",
                c->self, data_len, c->local_window);
            xfree(data);
            return;
        }
        c->local_window -= data_len;
    }
    packet_check_eom(cn);
    if (c->datagram)
        buffer_put_string(&c->output, data, data_len);
    else
        buffer_append(&c->output, data, data_len);
    xfree(data);
}

void
channel_input_extended_data(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id;
    char *data;
    u_int data_len, tcode;
    Channel *c;

    /* Get the channel number and verify it. */
    id = packet_get_int(cn);
    c = channel_lookup(cn, id);

    if (c == NULL)
        packet_disconnect(cn, "Received extended_data for bad channel %d.", id);
    if (c->type != SSH_CHANNEL_OPEN) {
        logit("channel %d: ext data for non open", id);
        return;
    }
    if (c->flags & CHAN_EOF_RCVD) {
        if (datafellows[cn] & SSH_BUG_EXTEOF)
            debug("channel %d: accepting ext data after eof", id);
        else
            packet_disconnect(cn, "Received extended_data after EOF "
                "on channel %d.", id);
    }
    tcode = packet_get_int(cn);
    if (c->efd == -1 ||
        c->extended_usage != CHAN_EXTENDED_WRITE ||
        tcode != SSH2_EXTENDED_DATA_STDERR) {
        logit("channel %d: bad ext data", c->self);
        return;
    }
    data = packet_get_string(cn, &data_len);
    packet_check_eom(cn);
    if (data_len > c->local_window) {
        logit("channel %d: rcvd too much extended_data %d, win %d",
            c->self, data_len, c->local_window);
        xfree(data);
        return;
    }
    debug2("channel %d: rcvd ext data %d", c->self, data_len);
    c->local_window -= data_len;
    buffer_append(&c->extended, data, data_len);
    xfree(data);
}

void
channel_input_ieof(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id;
    Channel *c;

    id = packet_get_int(cn);
    packet_check_eom(cn);
    c = channel_lookup(cn, id);
    if (c == NULL)
        packet_disconnect(cn, "Received ieof for nonexistent channel %d.", id);
    chan_rcvd_ieof(cn, c);

    /* XXX force input close */
    if (c->force_drain && c->istate == CHAN_INPUT_OPEN) {
        debug("channel %d: FORCE input drain", c->self);
        c->istate = CHAN_INPUT_WAIT_DRAIN;
        if (buffer_len(&c->input) == 0)
            chan_ibuf_empty(cn, c);
    }

}

void
channel_input_close(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id;
    Channel *c;

    id = packet_get_int(cn);
    packet_check_eom(cn);
    c = channel_lookup(cn, id);
    if (c == NULL)
        packet_disconnect(cn, "Received close for nonexistent channel %d.", id);

    /*
     * Send a confirmation that we have closed the channel and no more
     * data is coming for it.
     */
    packet_start(cn, SSH_MSG_CHANNEL_CLOSE_CONFIRMATION);
    packet_put_int(cn, c->remote_id);
    packet_send(cn);

    /*
     * If the channel is in closed state, we have sent a close request,
     * and the other side will eventually respond with a confirmation.
     * Thus, we cannot free the channel here, because then there would be
     * no-one to receive the confirmation.  The channel gets freed when
     * the confirmation arrives.
     */
    if (c->type != SSH_CHANNEL_CLOSED) {
        /*
         * Not a closed channel - mark it as draining, which will
         * cause it to be freed later.
         */
        buffer_clear(&c->input);
        c->type = SSH_CHANNEL_OUTPUT_DRAINING;
    }
}

/* proto version 1.5 overloads CLOSE_CONFIRMATION with OCLOSE */
void
channel_input_oclose(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id = packet_get_int(cn);
    Channel *c = channel_lookup(cn, id);

    packet_check_eom(cn);
    if (c == NULL)
        packet_disconnect(cn, "Received oclose for nonexistent channel %d.", id);
    chan_rcvd_oclose(cn, c);
}

void
channel_input_close_confirmation(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id = packet_get_int(cn);
    Channel *c = channel_lookup(cn, id);

    packet_check_eom(cn);
    if (c == NULL)
        packet_disconnect(cn, "Received close confirmation for "
            "out-of-range channel %d.", id);
    if (c->type != SSH_CHANNEL_CLOSED)
        packet_disconnect(cn, "Received close confirmation for "
            "non-closed channel %d (type %d).", id, c->type);
    channel_free(c);
}

void
channel_input_open_confirmation(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id, remote_id;
    Channel *c;

    id = packet_get_int(cn);
    c = channel_lookup(cn, id);

    if (c==NULL || c->type != SSH_CHANNEL_OPENING)
        packet_disconnect(cn, "Received open confirmation for "
            "non-opening channel %d.", id);
    remote_id = packet_get_int(cn);
    /* Record the remote channel number and mark that the channel is now open. */
    c->remote_id = remote_id;
    c->type = SSH_CHANNEL_OPEN;

    if (compat20[cn]) {
        c->remote_window = packet_get_int(cn);
        c->remote_maxpacket = packet_get_int(cn);
        if (c->confirm) {
            debug2("callback start");
            c->confirm(cn, c->self, c->confirm_ctx);
            debug2("callback done");
        }
        debug2("channel %d: open confirm rwindow %u rmax %u", c->self,
            c->remote_window, c->remote_maxpacket);
    }
    packet_check_eom(cn);
}

char *
reason2txt(int reason)
{
    switch (reason) {
    case SSH2_OPEN_ADMINISTRATIVELY_PROHIBITED:
        return "administratively prohibited";
    case SSH2_OPEN_CONNECT_FAILED:
        return "connect failed";
    case SSH2_OPEN_UNKNOWN_CHANNEL_TYPE:
        return "unknown channel type";
    case SSH2_OPEN_RESOURCE_SHORTAGE:
        return "resource shortage";
    }
    return "unknown reason";
}

void
channel_input_open_failure(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id, reason;
    char *msg = NULL, *lang = NULL;
    Channel *c;

    id = packet_get_int(cn);
    c = channel_lookup(cn, id);

    if (c==NULL || c->type != SSH_CHANNEL_OPENING)
        packet_disconnect(cn, "Received open failure for "
            "non-opening channel %d.", id);
    if (compat20[cn]) {
        reason = packet_get_int(cn);
        if (!(datafellows[cn] & SSH_BUG_OPENFAILURE)) {
            msg  = packet_get_string(cn, NULL);
            lang = packet_get_string(cn, NULL);
        }
        logit("channel %d: open failed: %s%s%s", id,
            reason2txt(reason), msg ? ": ": "", msg ? msg : "");
        if (msg != NULL)
            xfree(msg);
        if (lang != NULL)
            xfree(lang);
    }
    packet_check_eom(cn);
    /* Free the channel.  This will also close the socket. */
    channel_free(c);
}

void
channel_input_window_adjust(int cn, int type, u_int32_t seq, void *ctxt)
{
    Channel *c;
    int id;
    u_int adjust;

    if (!compat20[cn])
        return;

    /* Get the channel number and verify it. */
    id = packet_get_int(cn);
    c = channel_lookup(cn, id);

    if (c == NULL) {
        logit("Received window adjust for non-open channel %d.", id);
        return;
    }
    adjust = packet_get_int(cn);
    packet_check_eom(cn);
    debug2("channel %d: rcvd adjust %u", id, adjust);
    c->remote_window += adjust;
}

void
channel_input_port_open(int cn, int type, u_int32_t seq, void *ctxt)
{
    Channel *c = NULL;
    u_short host_port;
    char *host, *originator_string;
    int remote_id, sock = -1;

    remote_id = packet_get_int(cn);
    host = packet_get_string(cn, NULL);
    host_port = packet_get_int(cn);

    if (packet_get_protocol_flags(cn) & SSH_PROTOFLAG_HOST_IN_FWD_OPEN) {
        originator_string = packet_get_string(cn, NULL);
    } else {
        originator_string = xstrdup("unknown (remote did not supply name)");
    }
    packet_check_eom(cn);
    sock = channel_connect_to(host, host_port);
    if (sock != -1) {
        c = channel_new(cn, "connected socket",
            SSH_CHANNEL_CONNECTING, sock, sock, -1, 0, 0, 0,
            originator_string, 1);
        c->remote_id = remote_id;
    }
    xfree(originator_string);
    if (c == NULL) {
        packet_start(cn, SSH_MSG_CHANNEL_OPEN_FAILURE);
        packet_put_int(cn, remote_id);
        packet_send(cn);
    }
    xfree(host);
}


/* -- tcp forwarding */

void
channel_set_af(int af)
{
    IPv4or6 = af;
}

int
channel_setup_fwd_listener(int cn, int type, const char *listen_addr, u_short listen_port,
    const char *host_to_connect, u_short port_to_connect, int gateway_ports)
{
    Channel *c;
    int sock, r, success = 0, wildcard = 0, is_client;
    struct addrinfo hints, *ai, *aitop;
    const char *host, *addr;
    char ntop[NI_MAXHOST], strport[NI_MAXSERV];

    host = (type == SSH_CHANNEL_RPORT_LISTENER) ?
        listen_addr : host_to_connect;
    is_client = (type == SSH_CHANNEL_PORT_LISTENER);

    if (host == NULL) {
        error("No forward host name.");
        return 0;
    }
    if (strlen(host) > SSH_CHANNEL_PATH_LEN - 1) {
        error("Forward host name too long.");
        return 0;
    }

    /*
     * Determine whether or not a port forward listens to loopback,
     * specified address or wildcard. On the client, a specified bind
     * address will always override gateway_ports. On the server, a
     * gateway_ports of 1 (``yes'') will override the client's
     * specification and force a wildcard bind, whereas a value of 2
     * (``clientspecified'') will bind to whatever address the client
     * asked for.
     *
     * Special-case listen_addrs are:
     *
     * "0.0.0.0"               -> wildcard v4/v6 if SSH_OLD_FORWARD_ADDR
     * "" (empty string), "*"  -> wildcard v4/v6
     * "localhost"             -> loopback v4/v6
     */
    addr = NULL;
    if (listen_addr == NULL) {
        /* No address specified: default to gateway_ports setting */
        if (gateway_ports)
            wildcard = 1;
    } else if (gateway_ports || is_client) {
        if (((datafellows[cn] & SSH_OLD_FORWARD_ADDR) &&
            strcmp(listen_addr, "0.0.0.0") == 0) ||
            *listen_addr == '\0' || strcmp(listen_addr, "*") == 0 ||
            (!is_client && gateway_ports == 1))
            wildcard = 1;
        else if (strcmp(listen_addr, "localhost") != 0)
            addr = listen_addr;
    }

    debug3("channel_setup_fwd_listener: type %d wildcard %d addr %s",
        type, wildcard, (addr == NULL) ? "NULL" : addr);

    /*
     * getaddrinfo returns a loopback address if the hostname is
     * set to NULL and hints.ai_flags is not AI_PASSIVE
     */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = IPv4or6;
    hints.ai_flags = wildcard ? AI_PASSIVE : 0;
    hints.ai_socktype = SOCK_STREAM;
    osapiSnprintf(strport, sizeof strport, "%d", listen_port);
    if ((r = getaddrinfo(addr, strport, &hints, &aitop)) != 0) {
        if (addr == NULL) {
            /* This really shouldn't happen */
            packet_disconnect(cn, "getaddrinfo: fatal error: %s",
                gai_strerror(r));
        } else {
            error("channel_setup_fwd_listener: "
                "getaddrinfo(%.64s): %s", addr, gai_strerror(r));
        }
        return 0;
    }

    for (ai = aitop; ai; ai = ai->ai_next) {
        if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
            continue;
        if (getnameinfo(ai->ai_addr, ai->ai_addrlen, ntop, sizeof(ntop),
            strport, sizeof(strport), NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
            error("channel_setup_fwd_listener: getnameinfo failed");
            continue;
        }
        /* Create a port to listen for the host. */
        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sock < 0) {
            /* this is no error since kernel may not support ipv6 */
            verbose("socket: %.100s", strerror(errno));
            continue;
        }

        channel_set_reuseaddr(sock);

        debug("Local forwarding listening on %s port %s.", ntop, strport);

        /* Bind the socket to the address. */
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            /* address can be in use ipv6 address is already bound */
            if (!ai->ai_next)
                error("bind: %.100s", strerror(errno));
            else
                verbose("bind: %.100s", strerror(errno));

            close(sock);
            continue;
        }
        /* Start listening for connections on the socket. */
        if (listen(sock, SSH_LISTEN_BACKLOG) < 0) {
            error("listen: %.100s", strerror(errno));
            close(sock);
            continue;
        }
        /* Allocate a channel number for the socket. */
        c = channel_new(cn, "port listener", type, sock, sock, -1,
            CHAN_TCP_WINDOW_DEFAULT, CHAN_TCP_PACKET_DEFAULT,
            0, "port listener", 1);
        strlcpy(c->path, host, sizeof(c->path));
        c->host_port = port_to_connect;
        c->listening_port = listen_port;
        success = 1;
    }
    if (success == 0)
        error("channel_setup_fwd_listener: cannot listen to port: %d",
            listen_port);
    freeaddrinfo(aitop);
    return success;
}

int
channel_cancel_rport_listener(int cn, const char *host, u_short port)
{
    u_int i;
    int found = 0;

    for (i = 0; i < channels_alloc[cn]; i++) {
        Channel *c = channels[cn][i];

        if (c != NULL && c->type == SSH_CHANNEL_RPORT_LISTENER &&
            strncmp(c->path, host, sizeof(c->path)) == 0 &&
            c->listening_port == port) {
            debug2("%s: close channel %d", __func__, i);
            channel_free(c);
            found = 1;
        }
    }

    return (found);
}

/* protocol local port fwd, used by ssh (and sshd in v1) */
int
channel_setup_local_fwd_listener(int cn, const char *listen_host, u_short listen_port,
    const char *host_to_connect, u_short port_to_connect, int gateway_ports)
{
    return channel_setup_fwd_listener(cn, SSH_CHANNEL_PORT_LISTENER,
        listen_host, listen_port, host_to_connect, port_to_connect,
        gateway_ports);
}

/* protocol v2 remote port fwd, used by sshd */
int
channel_setup_remote_fwd_listener(int cn, const char *listen_address,
    u_short listen_port, int gateway_ports)
{
    return channel_setup_fwd_listener(cn, SSH_CHANNEL_RPORT_LISTENER,
        listen_address, listen_port, NULL, 0, gateway_ports);
}

/*
 * Initiate forwarding of connections to port "port" on remote host through
 * the secure channel to host:port from local side.
 */

void
channel_request_remote_forwarding(int cn, const char *listen_host, u_short listen_port,
    const char *host_to_connect, u_short port_to_connect)
{
    int type, success = 0;

    /* Record locally that connection to this host/port is permitted. */
    if (num_permitted_opens >= SSH_MAX_FORWARDS_PER_DIRECTION)
        fatal_cn(cn, "channel_request_remote_forwarding: too many forwards");

    /* Send the forward request to the remote side. */
    if (compat20[cn]) {
        const char *address_to_bind;
        if (listen_host == NULL)
            address_to_bind = "localhost";
        else if (*listen_host == '\0' || strcmp(listen_host, "*") == 0)
            address_to_bind = "";
        else
            address_to_bind = listen_host;

        packet_start(cn, SSH2_MSG_GLOBAL_REQUEST);
        packet_put_cstring(cn, "tcpip-forward");
        packet_put_char(cn, 1);         /* boolean: want reply */
        packet_put_cstring(cn, address_to_bind);
        packet_put_int(cn, listen_port);
        packet_send(cn);
        packet_write_wait(cn);
        /* Assume that server accepts the request */
        success = 1;
    } else {
        packet_start(cn, SSH_CMSG_PORT_FORWARD_REQUEST);
        packet_put_int(cn, listen_port);
        packet_put_cstring(cn, host_to_connect);
        packet_put_int(cn, port_to_connect);
        packet_send(cn);
        packet_write_wait(cn);

        /* Wait for response from the remote side. */
        type = packet_read(cn);
        switch (type) {
        case SSH_SMSG_SUCCESS:
            success = 1;
            break;
        case SSH_SMSG_FAILURE:
            logit("Warning: Server denied remote port forwarding.");
            break;
        default:
            /* Unknown packet */
            packet_disconnect(cn, "Protocol error for port forward request:"
                "received packet type %d.", type);
        }
    }
    if (success) {
        permitted_opens[num_permitted_opens].host_to_connect = xstrdup(host_to_connect);
        permitted_opens[num_permitted_opens].port_to_connect = port_to_connect;
        permitted_opens[num_permitted_opens].listen_port = listen_port;
        num_permitted_opens++;
    }
}

/*
 * Request cancellation of remote forwarding of connection host:port from
 * local side.
 */
void
channel_request_rforward_cancel(int cn, const char *host, u_short port)
{
    int i;

    if (!compat20[cn])
        return;

    for (i = 0; i < num_permitted_opens; i++) {
        if (permitted_opens[i].host_to_connect != NULL &&
            permitted_opens[i].listen_port == port)
            break;
    }
    if (i >= num_permitted_opens) {
        debug("%s: requested forward not found", __func__);
        return;
    }
    packet_start(cn, SSH2_MSG_GLOBAL_REQUEST);
    packet_put_cstring(cn, "cancel-tcpip-forward");
    packet_put_char(cn, 0);
    packet_put_cstring(cn, host == NULL ? "" : host);
    packet_put_int(cn, port);
    packet_send(cn);

    permitted_opens[i].listen_port = 0;
    permitted_opens[i].port_to_connect = 0;
    xfree(permitted_opens[i].host_to_connect);
    permitted_opens[i].host_to_connect = NULL;
}

/*
 * This is called after receiving CHANNEL_FORWARDING_REQUEST.  This initates
 * listening for the port, and sends back a success reply (or disconnect
 * message if there was an error).  This never returns if there was an error.
 */

void
channel_input_port_forward_request(int cn, int is_root, int gateway_ports)
{
    u_short port, host_port;
    char *hostname;

    /* Get arguments from the packet. */
    port = packet_get_int(cn);
    hostname = packet_get_string(cn, NULL);
    host_port = packet_get_int(cn);

#ifndef HAVE_CYGWIN
    /*
     * Check that an unprivileged user is not trying to forward a
     * privileged port.
     */
    if (port < IPPORT_RESERVED && !is_root)
        packet_disconnect(cn,
            "Requested forwarding of port %d but user is not root.",
            port);
    if (host_port == 0)
        packet_disconnect(cn, "Dynamic forwarding denied.");
#endif

    /* Initiate forwarding */
    channel_setup_local_fwd_listener(cn, NULL, port, hostname,
        host_port, gateway_ports);

    /* Free the argument string. */
    xfree(hostname);
}

/*
 * Permits opening to any host/port if permitted_opens[] is empty.  This is
 * usually called by the server, because the user could connect to any port
 * anyway, and the server has no way to know but to trust the client anyway.
 */
void
channel_permit_all_opens(void)
{
    if (num_permitted_opens == 0)
        all_opens_permitted = 1;
}

void
channel_add_permitted_opens(char *host, int port)
{
    if (num_permitted_opens >= SSH_MAX_FORWARDS_PER_DIRECTION)
        fatal("channel_request_remote_forwarding: too many forwards");
    debug("allow port forwarding to host %s port %d", host, port);

    permitted_opens[num_permitted_opens].host_to_connect = xstrdup(host);
    permitted_opens[num_permitted_opens].port_to_connect = port;
    num_permitted_opens++;

    all_opens_permitted = 0;
}

void
channel_clear_permitted_opens(void)
{
    int i;

    for (i = 0; i < num_permitted_opens; i++)
        if (permitted_opens[i].host_to_connect != NULL)
            xfree(permitted_opens[i].host_to_connect);
    num_permitted_opens = 0;

}


/* return socket to remote host, port */
int
connect_to(const char *host, u_short port)
{
    struct addrinfo hints, *ai, *aitop;
    char ntop[NI_MAXHOST], strport[NI_MAXSERV];
    int gaierr;
    int sock = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = IPv4or6;
    hints.ai_socktype = SOCK_STREAM;
    osapiSnprintf(strport, sizeof strport, "%d", port);
    if ((gaierr = getaddrinfo(host, strport, &hints, &aitop)) != 0) {
        error("connect_to %.100s: unknown host (%s)", host,
            gai_strerror(gaierr));
        return -1;
    }
    for (ai = aitop; ai; ai = ai->ai_next) {
        if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
            continue;
        if (getnameinfo(ai->ai_addr, ai->ai_addrlen, ntop, sizeof(ntop),
            strport, sizeof(strport), NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
            error("connect_to: getnameinfo failed");
            continue;
        }
        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sock < 0) {
            if (ai->ai_next == NULL)
                error("socket: %.100s", strerror(errno));
            else
                verbose("socket: %.100s", strerror(errno));
            continue;
        }
        if (set_nonblock(sock) == -1)
            fatal("%s: set_nonblock(%d)", __func__, sock);
        if (connect(sock, ai->ai_addr, ai->ai_addrlen) < 0 &&
            errno != EINPROGRESS) {
            error("connect_to %.100s port %s: %.100s", ntop, strport,
                strerror(errno));
            close(sock);
            continue;   /* fail -- try next */
        }
        break; /* success */

    }
    freeaddrinfo(aitop);
    if (!ai) {
        error("connect_to %.100s port %d: failed.", host, port);
        return -1;
    }
    /* success */
    set_nodelay(sock);
    return sock;
}

int
channel_connect_by_listen_address(u_short listen_port)
{
    int i;

    for (i = 0; i < num_permitted_opens; i++)
        if (permitted_opens[i].host_to_connect != NULL &&
            permitted_opens[i].listen_port == listen_port)
            return connect_to(
                permitted_opens[i].host_to_connect,
                permitted_opens[i].port_to_connect);
    error("WARNING: Server requests forwarding for unknown listen_port %d",
        listen_port);
    return -1;
}

/* Check if connecting to that port is permitted and connect. */
int
channel_connect_to(const char *host, u_short port)
{
    int i, permit;

    permit = all_opens_permitted;
    if (!permit) {
        for (i = 0; i < num_permitted_opens; i++)
            if (permitted_opens[i].host_to_connect != NULL &&
                permitted_opens[i].port_to_connect == port &&
                strcmp(permitted_opens[i].host_to_connect, host) == 0)
                permit = 1;

    }
    if (!permit) {
        logit("Received request to connect to host %.100s port %d, "
            "but the request was denied.", host, port);
        return -1;
    }
    return connect_to(host, port);
}

#ifndef L7_SSHD
void
channel_send_window_changes(int cn)
{
    u_int i;
    struct winsize ws;

    for (i = 0; i < channels_alloc[cn]; i++) {
        if (channels[cn][i] == NULL || !channels[cn][i]->client_tty ||
            channels[cn][i]->type != SSH_CHANNEL_OPEN)
            continue;
        if (ioctl(channels[cn][i]->rfd, TIOCGWINSZ, &ws) < 0)
            continue;
        channel_request_start(cn, i, "window-change", 0);
        packet_put_int(cn, ws.ws_col);
        packet_put_int(cn, ws.ws_row);
        packet_put_int(cn, ws.ws_xpixel);
        packet_put_int(cn, ws.ws_ypixel);
        packet_send(cn);
    }
}

/* -- X11 forwarding */

/*
 * Creates an internet domain socket for listening for X11 connections.
 * Returns 0 and a suitable display number for the DISPLAY variable
 * stored in display_numberp , or -1 if an error occurs.
 */
int
x11_create_display_inet(int x11_display_offset, int x11_use_localhost,
    int single_connection, u_int *display_numberp, int **chanids)
{
    Channel *nc = NULL;
    int display_number, sock;
    u_short port;
    struct addrinfo hints, *ai, *aitop;
    char strport[NI_MAXSERV];
    int gaierr, n, num_socks = 0, socks[NUM_SOCKS];

    if (chanids == NULL)
        return -1;

    for (display_number = x11_display_offset;
        display_number < MAX_DISPLAYS;
        display_number++) {
        port = 6000 + display_number;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = IPv4or6;
        hints.ai_flags = x11_use_localhost ? 0: AI_PASSIVE;
        hints.ai_socktype = SOCK_STREAM;
        osapiSnprintf(strport, sizeof strport, "%d", port);
        if ((gaierr = getaddrinfo(NULL, strport, &hints, &aitop)) != 0) {
            error("getaddrinfo: %.100s", gai_strerror(gaierr));
            return -1;
        }
        for (ai = aitop; ai; ai = ai->ai_next) {
            if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
                continue;
            sock = socket(ai->ai_family, ai->ai_socktype,
                ai->ai_protocol);
            if (sock < 0) {
                if ((errno != EINVAL) && (errno != EAFNOSUPPORT)) {
                    error("socket: %.100s", strerror(errno));
                    freeaddrinfo(aitop);
                    return -1;
                } else {
                    debug("x11_create_display_inet: Socket family %d not supported",
                         ai->ai_family);
                    continue;
                }
            }
#ifdef IPV6_V6ONLY
            if (ai->ai_family == AF_INET6) {
                int on = 1;
                if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0)
                    error("setsockopt IPV6_V6ONLY: %.100s", strerror(errno));
            }
#endif
            channel_set_reuseaddr(sock);
            if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
                debug2("bind port %d: %.100s", port, strerror(errno));
                close(sock);

                if (ai->ai_next)
                    continue;

                for (n = 0; n < num_socks; n++) {
                    close(socks[n]);
                }
                num_socks = 0;
                break;
            }
            socks[num_socks++] = sock;
#ifndef DONT_TRY_OTHER_AF
            if (num_socks == NUM_SOCKS)
                break;
#else
            if (x11_use_localhost) {
                if (num_socks == NUM_SOCKS)
                    break;
            } else {
                break;
            }
#endif
        }
        freeaddrinfo(aitop);
        if (num_socks > 0)
            break;
    }
    if (display_number >= MAX_DISPLAYS) {
        error("Failed to allocate internet-domain X11 display socket.");
        return -1;
    }
    /* Start listening for connections on the socket. */
    for (n = 0; n < num_socks; n++) {
        sock = socks[n];
        if (listen(sock, SSH_LISTEN_BACKLOG) < 0) {
            error("listen: %.100s", strerror(errno));
            close(sock);
            return -1;
        }
    }

    /* Allocate a channel for each socket. */
    *chanids = xmalloc(sizeof(**chanids) * (num_socks + 1));
    for (n = 0; n < num_socks; n++) {
        sock = socks[n];
        nc = channel_new("x11 listener",
            SSH_CHANNEL_X11_LISTENER, sock, sock, -1,
            CHAN_X11_WINDOW_DEFAULT, CHAN_X11_PACKET_DEFAULT,
            0, "X11 inet listener", 1);
        nc->single_connection = single_connection;
        (*chanids)[n] = nc->self;
    }
    (*chanids)[n] = -1;

    /* Return the display number for the DISPLAY environment variable. */
    *display_numberp = display_number;
    return (0);
}
#endif /* L7_SSHD */

int
connect_local_xsocket(u_int dnr)
{
    int sock;
    struct sockaddr_un addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        error("socket: %.100s", strerror(errno));
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_INET;
    osapiSnprintf(addr.sun_path, sizeof addr.sun_path, _PATH_UNIX_X, dnr);
    if (connect(sock, (struct sockaddr *) & addr, sizeof(addr)) == 0)
        return sock;
    close(sock);
    error("connect %.100s: %.100s", addr.sun_path, strerror(errno));
    return -1;
}

int
x11_connect_display(void)
{
    int display_number, sock = 0;
    const char *display;
    char buf[1024], *cp;
    struct addrinfo hints, *ai, *aitop;
    char strport[NI_MAXSERV];
    int gaierr;

    /* Try to open a socket for the local X server. */
    display = getenv("DISPLAY");
    if (!display) {
        error("DISPLAY not set.");
        return -1;
    }
    /*
     * Now we decode the value of the DISPLAY variable and make a
     * connection to the real X server.
     */

    /*
     * Check if it is a unix domain socket.  Unix domain displays are in
     * one of the following formats: unix:d[.s], :d[.s], ::d[.s]
     */
    if (strncmp(display, "unix:", 5) == 0 ||
        display[0] == ':') {
        /* Connect to the unix domain socket. */
        if (sscanf(strrchr(display, ':') + 1, "%d", &display_number) != 1) {
            error("Could not parse display number from DISPLAY: %.100s",
                display);
            return -1;
        }
        /* Create a socket. */
        sock = connect_local_xsocket(display_number);
        if (sock < 0)
            return -1;

        /* OK, we now have a connection to the display. */
        return sock;
    }
    /*
     * Connect to an inet socket.  The DISPLAY value is supposedly
     * hostname:d[.s], where hostname may also be numeric IP address.
     */
    strlcpy(buf, display, sizeof(buf));
    cp = strchr(buf, ':');
    if (!cp) {
        error("Could not find ':' in DISPLAY: %.100s", display);
        return -1;
    }
    *cp = 0;
    /* buf now contains the host name.  But first we parse the display number. */
    if (sscanf(cp + 1, "%d", &display_number) != 1) {
        error("Could not parse display number from DISPLAY: %.100s",
            display);
        return -1;
    }

    /* Look up the host address */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = IPv4or6;
    hints.ai_socktype = SOCK_STREAM;
    osapiSnprintf(strport, sizeof strport, "%d", 6000 + display_number);
    if ((gaierr = getaddrinfo(buf, strport, &hints, &aitop)) != 0) {
        error("%.100s: unknown host. (%s)", buf, gai_strerror(gaierr));
        return -1;
    }
    for (ai = aitop; ai; ai = ai->ai_next) {
        /* Create a socket. */
        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sock < 0) {
            debug2("socket: %.100s", strerror(errno));
            continue;
        }
        /* Connect it to the display. */
        if (connect(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            debug2("connect %.100s port %d: %.100s", buf,
                6000 + display_number, strerror(errno));
            close(sock);
            continue;
        }
        /* Success */
        break;
    }
    freeaddrinfo(aitop);
    if (!ai) {
        error("connect %.100s port %d: %.100s", buf, 6000 + display_number,
            strerror(errno));
        return -1;
    }
    set_nodelay(sock);
    return sock;
}

/*
 * This is called when SSH_SMSG_X11_OPEN is received.  The packet contains
 * the remote channel number.  We should do whatever we want, and respond
 * with either SSH_MSG_OPEN_CONFIRMATION or SSH_MSG_OPEN_FAILURE.
 */

void
x11_input_open(int cn, int type, u_int32_t seq, void *ctxt)
{
    Channel *c = NULL;
    int remote_id, sock = 0;
    char *remote_host;

    debug("Received X11 open request.");

    remote_id = packet_get_int(cn);

    if (packet_get_protocol_flags(cn) & SSH_PROTOFLAG_HOST_IN_FWD_OPEN) {
        remote_host = packet_get_string(cn, NULL);
    } else {
        remote_host = xstrdup("unknown (remote did not supply name)");
    }
    packet_check_eom(cn);

    /* Obtain a connection to the real X display. */
    sock = x11_connect_display();
    if (sock != -1) {
        /* Allocate a channel for this connection. */
        c = channel_new(cn, "connected x11 socket",
            SSH_CHANNEL_X11_OPEN, sock, sock, -1, 0, 0, 0,
            remote_host, 1);
        c->remote_id = remote_id;
        c->force_drain = 1;
    }
    xfree(remote_host);
    if (c == NULL) {
        /* Send refusal to the remote host. */
        packet_start(cn, SSH_MSG_CHANNEL_OPEN_FAILURE);
        packet_put_int(cn, remote_id);
    } else {
        /* Send a confirmation to the remote host. */
        packet_start(cn, SSH_MSG_CHANNEL_OPEN_CONFIRMATION);
        packet_put_int(cn, remote_id);
        packet_put_int(cn, c->self);
    }
    packet_send(cn);
}

/* dummy protocol handler that denies SSH-1 requests (agent/x11) */
void
deny_input_open(int cn, int type, u_int32_t seq, void *ctxt)
{
    int rchan = packet_get_int(cn);

    switch (type) {
    case SSH_SMSG_AGENT_OPEN:
        error("Warning: ssh server tried agent forwarding.");
        break;
    case SSH_SMSG_X11_OPEN:
        error("Warning: ssh server tried X11 forwarding.");
        break;
    default:
        error("deny_input_open: type %d", type);
        break;
    }
    error("Warning: this is probably a break-in attempt by a malicious server.");
    packet_start(cn, SSH_MSG_CHANNEL_OPEN_FAILURE);
    packet_put_int(cn, rchan);
    packet_send(cn);
}

/*
 * Requests forwarding of X11 connections, generates fake authentication
 * data, and enables authentication spoofing.
 * This should be called in the client only.
 */
void
x11_request_forwarding_with_spoofing(int cn, int client_session_id, const char *disp,
    const char *proto, const char *data)
{
    u_int data_len = (u_int) strlen(data) / 2;
    u_int i, value;
    char *new_data;
    int screen_number;
    const char *cp;
    u_int32_t rnd = 0;

    if (x11_saved_display == NULL)
        x11_saved_display = xstrdup(disp);
    else if (strcmp(disp, x11_saved_display) != 0) {
        error("x11_request_forwarding_with_spoofing: different "
            "$DISPLAY already forwarded");
        return;
    }

    cp = disp;
    if (disp)
        cp = strchr(disp, ':');
    if (cp)
        cp = strchr(cp, '.');
    if (cp)
        screen_number = atoi(cp + 1);
    else
        screen_number = 0;

    if (x11_saved_proto == NULL) {
        /* Save protocol name. */
        x11_saved_proto = xstrdup(proto);
        /*
         * Extract real authentication data and generate fake data
         * of the same length.
         */
        x11_saved_data = xmalloc(data_len);
        x11_fake_data = xmalloc(data_len);
        for (i = 0; i < data_len; i++) {
            if (sscanf(data + 2 * i, "%2x", &value) != 1)
                fatal_cn(cn, "x11_request_forwarding: bad "
                    "authentication data: %.100s", data);
            if (i % 4 == 0)
                rnd = arc4random();
            x11_saved_data[i] = value;
            x11_fake_data[i] = rnd & 0xff;
            rnd >>= 8;
        }
        x11_saved_data_len = data_len;
        x11_fake_data_len = data_len;
    }

    /* Convert the fake data into hex. */
    new_data = tohex(x11_fake_data, data_len);

    /* Send the request packet. */
    if (compat20[cn]) {
        channel_request_start(cn, client_session_id, "x11-req", 0);
        packet_put_char(cn, 0); /* XXX bool single connection */
    } else {
        packet_start(cn, SSH_CMSG_X11_REQUEST_FORWARDING);
    }
    packet_put_cstring(cn, proto);
    packet_put_cstring(cn, new_data);
    packet_put_int(cn, screen_number);
    packet_send(cn);
    packet_write_wait(cn);
    xfree(new_data);
}


/* -- agent forwarding */

/* Sends a message to the server to request authentication fd forwarding. */

void
auth_request_forwarding(int cn)
{
    packet_start(cn, SSH_CMSG_AGENT_REQUEST_FORWARDING);
    packet_send(cn);
    packet_write_wait(cn);
}
