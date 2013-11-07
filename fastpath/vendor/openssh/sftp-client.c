/*
 * Copyright (c) 2001-2004 Damien Miller <djm@openbsd.org>
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
 */

/* XXX: memleaks */
/* XXX: signed vs unsigned */
/* XXX: remove all logging, only return status codes */
/* XXX: copy between two remote sites */

#include "includes.h"
RCSID("$OpenBSD: sftp-client.c,v 1.58 2006/01/02 01:20:31 djm Exp $");

#include "openbsd-compat/sys-queue.h"

#include "buffer.h"
#include "bufaux.h"
#include "getput.h"
#include "xmalloc.h"
#include "log.h"
#include "atomicio.h"
#include "progressmeter.h"

#include "sftp.h"
#include "sftp-common.h"
#include "sftp-client.h"

#ifdef L7_SSHD
#include "simapi.h"
#endif /* L7_SSHD */

extern volatile sig_atomic_t interrupted;
extern int showprogress;

/* Minimum amount of data to read at at time */
#define MIN_READ_SIZE   512

struct sftp_conn {
    int fd_in;
    int fd_out;
    u_int transfer_buflen;
    u_int num_requests;
    u_int version;
    u_int msg_id;
};

#ifndef L7_SSHD
static void
#else
static int
#endif /* L7_SSHD */
send_msg(int fd, Buffer *m)
{
    u_char mlen[4];

    if (buffer_len(m) > SFTP_MAX_MSG_LENGTH)
#ifndef L7_SSHD
        fatal("Outbound message too long %u", buffer_len(m));
#else
    {
        error("Outbound message too long %u", buffer_len(m));
        return -1;
    }
#endif /* L7_SSHD */

    /* Send length first */
    PUT_32BIT(mlen, buffer_len(m));
    if (atomicio(vwrite, fd, mlen, sizeof(mlen)) != sizeof(mlen))
#ifndef L7_SSHD
        fatal("Couldn't send packet: %s", strerror(errno));
#else
    {
        error("Couldn't send packet: %s", strerror(errno));
        return -1;
    }
#endif /* L7_SSHD */

    if (atomicio(vwrite, fd, buffer_ptr(m), buffer_len(m)) != buffer_len(m))
#ifndef L7_SSHD
        fatal("Couldn't send packet: %s", strerror(errno));
#else
    {
        error("Couldn't send packet: %s", strerror(errno));
        return -1;
    }
#endif /* L7_SSHD */

    buffer_clear(m);
    return 0;
}

#ifndef L7_SSHD
static void
#else
static int
#endif /* L7_SSHD */
get_msg(int fd, Buffer *m)
{
    u_int msg_len;

    buffer_append_space(m, 4);
#ifndef L7_SSHD
    if (atomicio(read, fd, buffer_ptr(m), 4) != 4) {
#else
    if (atomicio(vread, fd, buffer_ptr(m), 4) != 4) {
#endif /* L7_SSHD */
#ifndef L7_SSHD
        if (errno == EPIPE)
            fatal("Connection closed");
        else
            fatal("Couldn't read packet: %s", strerror(errno));
#else
        if (errno == EPIPE)
        {
          error("Connection closed");
          return -1;
        }
        else
        {
          error("Couldn't read packet: %s", strerror(errno));
          return -1;
        }
#endif /* L7_SSHD */
    }

    msg_len = buffer_get_int(m);
    if (msg_len > SFTP_MAX_MSG_LENGTH)
#ifndef L7_SSHD
        fatal("Received message too long %u", msg_len);
#else
    {
      error("Received message too long %u", msg_len);
      return -1;
    }
#endif /* L7_SSHD */

    buffer_append_space(m, msg_len);
#ifndef L7_SSHD
    if (atomicio(read, fd, buffer_ptr(m), msg_len) != msg_len) {
#else
    if (atomicio(vread, fd, buffer_ptr(m), msg_len) != msg_len) {
#endif /* L7_SSHD */
#ifndef L7_SSHD
        if (errno == EPIPE)
            fatal("Connection closed");
        else
            fatal("Read packet: %s", strerror(errno));
#else
        if (errno == EPIPE)
        {
          error("Connection closed");
          return -1;
        }
        else
        {
          error("Read packet: %s", strerror(errno));
          return -1;
        }
#endif /* L7_SSHD */
    }
    return 0;
}

static void
send_string_request(int fd, u_int id, u_int code, char *s,
    u_int len)
{
    Buffer msg;

    buffer_init(&msg);
    buffer_put_char(&msg, code);
    buffer_put_int(&msg, id);
    buffer_put_string(&msg, s, len);
    send_msg(fd, &msg);
    debug3("Sent message fd %d T:%u I:%u", fd, code, id);
    buffer_free(&msg);
}

static void
send_string_attrs_request(int fd, u_int id, u_int code, char *s,
    u_int len, Attrib *a)
{
    Buffer msg;

    buffer_init(&msg);
    buffer_put_char(&msg, code);
    buffer_put_int(&msg, id);
    buffer_put_string(&msg, s, len);
    encode_attrib(&msg, a);
    send_msg(fd, &msg);
    debug3("Sent message fd %d T:%u I:%u", fd, code, id);
    buffer_free(&msg);
}

static u_int
get_status(int fd, u_int expected_id)
{
    Buffer msg;
    u_int type, id, status;

    buffer_init(&msg);
#ifndef L7_SSHD
    get_msg(fd, &msg);
#else
    if (get_msg(fd, &msg) != 0)
    {
      error("SSH2_FX_FAILURE");
      buffer_free(&msg);
      return SSH2_FX_FAILURE;
    }
#endif /* L7_SSHD */
    type = buffer_get_char(&msg);
    id = buffer_get_int(&msg);

#ifndef L7_SSHD
    if (id != expected_id)
        fatal("ID mismatch (%u != %u)", id, expected_id);
    if (type != SSH2_FXP_STATUS)
        fatal("Expected SSH2_FXP_STATUS(%u) packet, got %u",
            SSH2_FXP_STATUS, type);
#else
    if (id != expected_id)
        error("ID mismatch (%u != %u)", id, expected_id);
    if (type != SSH2_FXP_STATUS)
        error("Expected SSH2_FXP_STATUS(%u) packet, got %u",
            SSH2_FXP_STATUS, type);
#endif /* L7_SSHD */

    status = buffer_get_int(&msg);
    buffer_free(&msg);

    debug3("SSH2_FXP_STATUS %u", status);

    return(status);
}

static char *
get_handle(int fd, u_int expected_id, u_int *len)
{
    Buffer msg;
    u_int type, id;
    char *handle;

    buffer_init(&msg);
#ifndef L7_SSHD
    get_msg(fd, &msg);
#else
    if (get_msg(fd, &msg) != 0)
    {
      error("get_msg failure");
      buffer_free(&msg);
      return NULL;
    }
#endif /* L7_SSHD */
    type = buffer_get_char(&msg);
    id = buffer_get_int(&msg);

    if (id != expected_id)
#ifndef L7_SSHD
        fatal("ID mismatch (%u != %u)", id, expected_id);
#else
    {
      error("ID mismatch (%u != %u)", id, expected_id);
      buffer_free(&msg);
      return(NULL);
    }
#endif /* L7_SSHD */
    if (type == SSH2_FXP_STATUS) {
        int status = buffer_get_int(&msg);

        error("Couldn't get handle: %s", fx2txt(status));
        buffer_free(&msg);
        return(NULL);
    } else if (type != SSH2_FXP_HANDLE)
#ifndef L7_SSHD
        fatal("Expected SSH2_FXP_HANDLE(%u) packet, got %u",
            SSH2_FXP_HANDLE, type);
#else
    {
      error("Expected SSH2_FXP_HANDLE(%u) packet, got %u",
          SSH2_FXP_HANDLE, type);
      buffer_free(&msg);
      return(NULL);
    }
#endif /* L7_SSHD */

    handle = buffer_get_string(&msg, len);
    buffer_free(&msg);

    return(handle);
}

static Attrib *
get_decode_stat(int fd, u_int expected_id, int quiet)
{
    Buffer msg;
    u_int type, id;
    Attrib *a;

    buffer_init(&msg);
#ifndef L7_SSHD
    get_msg(fd, &msg);
#else
    if (get_msg(fd, &msg) != 0)
    {
      error("get_msg failure");
      buffer_free(&msg);
      return NULL;
    }
#endif /* L7_SSHD */

    type = buffer_get_char(&msg);
    id = buffer_get_int(&msg);

    debug3("Received stat reply T:%u I:%u", type, id);
    if (id != expected_id)
#ifndef L7_SSHD
        fatal("ID mismatch (%u != %u)", id, expected_id);
#else
    {
      error("ID mismatch (%u != %u)", id, expected_id);
      buffer_free(&msg);
      return NULL;
    }
#endif /* L7_SSHD */
    if (type == SSH2_FXP_STATUS) {
        int status = buffer_get_int(&msg);

        if (quiet)
            debug("Couldn't stat remote file: %s", fx2txt(status));
        else
            error("Couldn't stat remote file: %s", fx2txt(status));
        buffer_free(&msg);
        return(NULL);
    } else if (type != SSH2_FXP_ATTRS) {
#ifndef L7_SSHD
        fatal("Expected SSH2_FXP_ATTRS(%u) packet, got %u",
            SSH2_FXP_ATTRS, type);
#else
        error("Expected SSH2_FXP_ATTRS(%u) packet, got %u",
            SSH2_FXP_ATTRS, type);
        buffer_free(&msg);
        return NULL;
#endif /* L7_SSHD */
    }
    a = decode_attrib(&msg);
    buffer_free(&msg);

    return(a);
}

struct sftp_conn *
do_init(int fd_in, int fd_out, u_int transfer_buflen, u_int num_requests)
{
    u_int type;
    int version;
    Buffer msg;
    struct sftp_conn *ret;

    buffer_init(&msg);
    buffer_put_char(&msg, SSH2_FXP_INIT);
    buffer_put_int(&msg, SSH2_FILEXFER_VERSION);
#ifndef L7_SSHD
    send_msg(fd_out, &msg);
#else
    if (send_msg(fd_out, &msg) != 0)
    {
      error("send_msg failure");
      buffer_free(&msg);
      return NULL;
    }
#endif /* L7_SSHD */

    buffer_clear(&msg);

#ifndef L7_SSHD
    get_msg(fd_in, &msg);
#else
    if (get_msg(fd_in, &msg) != 0)
    {
      error("get_msg failure");
      buffer_free(&msg);
      return NULL;
    }
#endif /* L7_SSHD */

    /* Expecting a VERSION reply */
    if ((type = buffer_get_char(&msg)) != SSH2_FXP_VERSION) {
        error("Invalid packet back from SSH2_FXP_INIT (type %u)",
            type);
        buffer_free(&msg);
        return(NULL);
    }
    version = buffer_get_int(&msg);

    debug2("Remote version: %d", version);

    /* Check for extensions */
    while (buffer_len(&msg) > 0) {
        char *name = buffer_get_string(&msg, NULL);
        char *value = buffer_get_string(&msg, NULL);

        debug2("Init extension: \"%s\"", name);
        xfree(name);
        xfree(value);
    }

    buffer_free(&msg);

    ret = xmalloc(sizeof(*ret));
    ret->fd_in = fd_in;
    ret->fd_out = fd_out;
    ret->transfer_buflen = transfer_buflen;
    ret->num_requests = num_requests;
    ret->version = version;
    ret->msg_id = 1;

    /* Some filexfer v.0 servers don't support large packets */
    if (version == 0)
        ret->transfer_buflen = MIN(ret->transfer_buflen, 20480);

    return(ret);
}

u_int
sftp_proto_version(struct sftp_conn *conn)
{
    return(conn->version);
}

int
do_close(struct sftp_conn *conn, char *handle, u_int handle_len)
{
    u_int id, status;
    Buffer msg;

    buffer_init(&msg);

    id = conn->msg_id++;
    buffer_put_char(&msg, SSH2_FXP_CLOSE);
    buffer_put_int(&msg, id);
    buffer_put_string(&msg, handle, handle_len);
#ifndef L7_SSHD
    send_msg(conn->fd_out, &msg);
#else
    if (send_msg(conn->fd_out, &msg) != 0)
    {
      error("send_msg failure");
      buffer_free(&msg);
      return -1;
    }
#endif /* L7_SSHD */
    debug3("Sent message SSH2_FXP_CLOSE I:%u", id);

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't close file: %s", fx2txt(status));

    buffer_free(&msg);

    return(status);
}


static int
do_lsreaddir(struct sftp_conn *conn, char *path, int printflag,
    SFTP_DIRENT ***dir)
{
    Buffer msg;
    u_int count, type, id, handle_len, i, expected_id, ents = 0;
    char *handle;

    id = conn->msg_id++;

    buffer_init(&msg);
    buffer_put_char(&msg, SSH2_FXP_OPENDIR);
    buffer_put_int(&msg, id);
    buffer_put_cstring(&msg, path);
    send_msg(conn->fd_out, &msg);

    buffer_clear(&msg);

    handle = get_handle(conn->fd_in, id, &handle_len);
    if (handle == NULL)
        return(-1);

    if (dir) {
        ents = 0;
        *dir = xmalloc(sizeof(**dir));
        (*dir)[0] = NULL;
    }

    for (; !interrupted;) {
        id = expected_id = conn->msg_id++;

        debug3("Sending SSH2_FXP_READDIR I:%u", id);

        buffer_clear(&msg);
        buffer_put_char(&msg, SSH2_FXP_READDIR);
        buffer_put_int(&msg, id);
        buffer_put_string(&msg, handle, handle_len);
        send_msg(conn->fd_out, &msg);

        buffer_clear(&msg);

#ifndef L7_SSHD
        get_msg(conn->fd_in, &msg);
#else
        if (get_msg(conn->fd_in, &msg) != 0)
        {
           error("get_msg failure");
           buffer_free(&msg);
           return -1;
        }
#endif /* L7_SSHD */

        type = buffer_get_char(&msg);
        id = buffer_get_int(&msg);

        debug3("Received reply T:%u I:%u", type, id);

        if (id != expected_id)
#ifndef L7_SSHD
            fatal("ID mismatch (%u != %u)", id, expected_id);
#else
        {
           error("ID mismatch (%u != %u)", id, expected_id);
           buffer_free(&msg);
           return -1;
        }
#endif /* L7_SSHD */

        if (type == SSH2_FXP_STATUS) {
            int status = buffer_get_int(&msg);

            debug3("Received SSH2_FXP_STATUS %d", status);

            if (status == SSH2_FX_EOF) {
                break;
            } else {
                error("Couldn't read directory: %s",
                    fx2txt(status));
                do_close(conn, handle, handle_len);
                xfree(handle);
                return(status);
            }
        } else if (type != SSH2_FXP_NAME)
#ifndef L7_SSHD
            fatal("Expected SSH2_FXP_NAME(%u) packet, got %u",
                SSH2_FXP_NAME, type);
#else
        {
           error("Expected SSH2_FXP_NAME(%u) packet, got %u",
              SSH2_FXP_NAME, type);
           buffer_free(&msg);
           return -1;
        }
#endif /* L7_SSHD */

        count = buffer_get_int(&msg);
        if (count == 0)
            break;
        debug3("Received %d SSH2_FXP_NAME responses", count);
        for (i = 0; i < count; i++) {
            char *filename, *longname;
            Attrib *a;

            filename = buffer_get_string(&msg, NULL);
            longname = buffer_get_string(&msg, NULL);
            a = decode_attrib(&msg);

            if (printflag)
                printf("%s\n", longname);

            if (dir) {
                *dir = xrealloc(*dir, sizeof(**dir) *
                    (ents + 2));
                (*dir)[ents] = xmalloc(sizeof(***dir));
                (*dir)[ents]->filename = xstrdup(filename);
                (*dir)[ents]->longname = xstrdup(longname);
                memcpy(&(*dir)[ents]->a, a, sizeof(*a));
                (*dir)[++ents] = NULL;
            }

            xfree(filename);
            xfree(longname);
        }
    }

    buffer_free(&msg);
    do_close(conn, handle, handle_len);
    xfree(handle);

    /* Don't return partial matches on interrupt */
    if (interrupted && dir != NULL && *dir != NULL) {
        free_sftp_dirents(*dir);
        *dir = xmalloc(sizeof(**dir));
        **dir = NULL;
    }

    return(0);
}

int
do_readdir(struct sftp_conn *conn, char *path, SFTP_DIRENT ***dir)
{
    return(do_lsreaddir(conn, path, 0, dir));
}

void free_sftp_dirents(SFTP_DIRENT **s)
{
    int i;

    for (i = 0; s[i]; i++) {
        xfree(s[i]->filename);
        xfree(s[i]->longname);
        xfree(s[i]);
    }
    xfree(s);
}

int
do_rm(struct sftp_conn *conn, char *path)
{
    u_int status, id;

    debug2("Sending SSH2_FXP_REMOVE \"%s\"", path);

    id = conn->msg_id++;
    send_string_request(conn->fd_out, id, SSH2_FXP_REMOVE, path,
        strlen(path));
    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't delete file: %s", fx2txt(status));
    return(status);
}

int
do_mkdir(struct sftp_conn *conn, char *path, Attrib *a)
{
    u_int status, id;

    id = conn->msg_id++;
    send_string_attrs_request(conn->fd_out, id, SSH2_FXP_MKDIR, path,
        strlen(path), a);

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't create directory: %s", fx2txt(status));

    return(status);
}

int
do_rmdir(struct sftp_conn *conn, char *path)
{
    u_int status, id;

    id = conn->msg_id++;
    send_string_request(conn->fd_out, id, SSH2_FXP_RMDIR, path,
        strlen(path));

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't remove directory: %s", fx2txt(status));

    return(status);
}

Attrib *
do_stat(struct sftp_conn *conn, char *path, int quiet)
{
    u_int id;

    id = conn->msg_id++;

    send_string_request(conn->fd_out, id,
        conn->version == 0 ? SSH2_FXP_STAT_VERSION_0 : SSH2_FXP_STAT,
        path, strlen(path));

    return(get_decode_stat(conn->fd_in, id, quiet));
}

Attrib *
do_lstat(struct sftp_conn *conn, char *path, int quiet)
{
    u_int id;

    if (conn->version == 0) {
        if (quiet)
            debug("Server version does not support lstat operation");
        else
            logit("Server version does not support lstat operation");
        return(do_stat(conn, path, quiet));
    }

    id = conn->msg_id++;
    send_string_request(conn->fd_out, id, SSH2_FXP_LSTAT, path,
        strlen(path));

    return(get_decode_stat(conn->fd_in, id, quiet));
}

Attrib *
do_fstat(struct sftp_conn *conn, char *handle, u_int handle_len, int quiet)
{
    u_int id;

    id = conn->msg_id++;
    send_string_request(conn->fd_out, id, SSH2_FXP_FSTAT, handle,
        handle_len);

    return(get_decode_stat(conn->fd_in, id, quiet));
}

int
do_setstat(struct sftp_conn *conn, char *path, Attrib *a)
{
    u_int status, id;

    id = conn->msg_id++;
    send_string_attrs_request(conn->fd_out, id, SSH2_FXP_SETSTAT, path,
        strlen(path), a);

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't setstat on \"%s\": %s", path,
            fx2txt(status));

    return(status);
}

int
do_fsetstat(struct sftp_conn *conn, char *handle, u_int handle_len,
    Attrib *a)
{
    u_int status, id;

    id = conn->msg_id++;
    send_string_attrs_request(conn->fd_out, id, SSH2_FXP_FSETSTAT, handle,
        handle_len, a);

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't fsetstat: %s", fx2txt(status));

    return(status);
}

char *
do_realpath(struct sftp_conn *conn, char *path)
{
    Buffer msg;
    u_int type, expected_id, count, id;
    char *filename, *longname;
    Attrib *a;

    expected_id = id = conn->msg_id++;
    send_string_request(conn->fd_out, id, SSH2_FXP_REALPATH, path,
        strlen(path));

    buffer_init(&msg);

#ifndef L7_SSHD
        get_msg(conn->fd_in, &msg);
#else
        if (get_msg(conn->fd_in, &msg) != 0)
        {
           error("get_msg failure");
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */
    type = buffer_get_char(&msg);
    id = buffer_get_int(&msg);

    if (id != expected_id)
#ifndef L7_SSHD
        fatal("ID mismatch (%u != %u)", id, expected_id);
#else
        {
           error("ID mismatch (%u != %u)", id, expected_id);
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */

    if (type == SSH2_FXP_STATUS) {
        u_int status = buffer_get_int(&msg);

        error("Couldn't canonicalise: %s", fx2txt(status));
        return(NULL);
    } else if (type != SSH2_FXP_NAME)
#ifndef L7_SSHD
        fatal("Expected SSH2_FXP_NAME(%u) packet, got %u",
            SSH2_FXP_NAME, type);
#else
        {
           error("Expected SSH2_FXP_NAME(%u) packet, got %u",
              SSH2_FXP_NAME, type);
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */

    count = buffer_get_int(&msg);
    if (count != 1)
#ifndef L7_SSHD
        fatal("Got multiple names (%d) from SSH_FXP_REALPATH", count);
#else
        {
           error("Got multiple names (%d) from SSH_FXP_REALPATH", count);
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */

    filename = buffer_get_string(&msg, NULL);
    longname = buffer_get_string(&msg, NULL);
    a = decode_attrib(&msg);

    debug3("SSH_FXP_REALPATH %s -> %s", path, filename);

    xfree(longname);

    buffer_free(&msg);

    return(filename);
}

int
do_rename(struct sftp_conn *conn, char *oldpath, char *newpath)
{
    Buffer msg;
    u_int status, id;

    buffer_init(&msg);

    /* Send rename request */
    id = conn->msg_id++;
    buffer_put_char(&msg, SSH2_FXP_RENAME);
    buffer_put_int(&msg, id);
    buffer_put_cstring(&msg, oldpath);
    buffer_put_cstring(&msg, newpath);
    send_msg(conn->fd_out, &msg);
    debug3("Sent message SSH2_FXP_RENAME \"%s\" -> \"%s\"", oldpath,
        newpath);
    buffer_free(&msg);

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't rename file \"%s\" to \"%s\": %s", oldpath,
            newpath, fx2txt(status));

    return(status);
}

int
do_symlink(struct sftp_conn *conn, char *oldpath, char *newpath)
{
    Buffer msg;
    u_int status, id;

    if (conn->version < 3) {
        error("This server does not support the symlink operation");
        return(SSH2_FX_OP_UNSUPPORTED);
    }

    buffer_init(&msg);

    /* Send symlink request */
    id = conn->msg_id++;
    buffer_put_char(&msg, SSH2_FXP_SYMLINK);
    buffer_put_int(&msg, id);
    buffer_put_cstring(&msg, oldpath);
    buffer_put_cstring(&msg, newpath);
    send_msg(conn->fd_out, &msg);
    debug3("Sent message SSH2_FXP_SYMLINK \"%s\" -> \"%s\"", oldpath,
        newpath);
    buffer_free(&msg);

    status = get_status(conn->fd_in, id);
    if (status != SSH2_FX_OK)
        error("Couldn't symlink file \"%s\" to \"%s\": %s", oldpath,
            newpath, fx2txt(status));

    return(status);
}

char *
do_readlink(struct sftp_conn *conn, char *path)
{
    Buffer msg;
    u_int type, expected_id, count, id;
    char *filename, *longname;
    Attrib *a;

    expected_id = id = conn->msg_id++;
    send_string_request(conn->fd_out, id, SSH2_FXP_READLINK, path,
        strlen(path));

    buffer_init(&msg);

#ifndef L7_SSHD
        get_msg(conn->fd_in, &msg);
#else
        if (get_msg(conn->fd_in, &msg) != 0)
        {
           error("get_msg failure");
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */
    type = buffer_get_char(&msg);
    id = buffer_get_int(&msg);

    if (id != expected_id)
#ifndef L7_SSHD
        fatal("ID mismatch (%u != %u)", id, expected_id);
#else
        {
           error("ID mismatch (%u != %u)", id, expected_id);
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */

    if (type == SSH2_FXP_STATUS) {
        u_int status = buffer_get_int(&msg);

        error("Couldn't readlink: %s", fx2txt(status));
        return(NULL);
    } else if (type != SSH2_FXP_NAME)
#ifndef L7_SSHD
        fatal("Expected SSH2_FXP_NAME(%u) packet, got %u",
            SSH2_FXP_NAME, type);
#else
        {
           error("Expected SSH2_FXP_NAME(%u) packet, got %u",
              SSH2_FXP_NAME, type);
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */

    count = buffer_get_int(&msg);
    if (count != 1)
#ifndef L7_SSHD
        fatal("Got multiple names (%d) from SSH_FXP_READLINK", count);
#else
        {
           error("Got multiple names (%d) from SSH_FXP_READLINK", count);
           buffer_free(&msg);
           return NULL;
        }
#endif /* L7_SSHD */

    filename = buffer_get_string(&msg, NULL);
    longname = buffer_get_string(&msg, NULL);
    a = decode_attrib(&msg);

    debug3("SSH_FXP_READLINK %s -> %s", path, filename);

    xfree(longname);

    buffer_free(&msg);

    return(filename);
}

static void
send_read_request(int fd_out, u_int id, u_int64_t offset, u_int len,
    char *handle, u_int handle_len)
{
    Buffer msg;

    buffer_init(&msg);
    buffer_clear(&msg);
    buffer_put_char(&msg, SSH2_FXP_READ);
    buffer_put_int(&msg, id);
    buffer_put_string(&msg, handle, handle_len);
    buffer_put_int64(&msg, offset);
    buffer_put_int(&msg, len);
    send_msg(fd_out, &msg);
    buffer_free(&msg);
}

int
do_download(struct sftp_conn *conn, char *remote_path, char *local_path,
    int pflag)
{
    Attrib junk, *a;
    Buffer msg;
    char *handle;
    int local_fd, status = 0, write_error;
    int read_error, write_errno;
    u_int64_t offset, size;
    u_int handle_len, mode, type, id, buflen, num_req, max_req;
    off_t progress_counter;
    struct request {
        u_int id;
        u_int len;
        u_int64_t offset;
        TAILQ_ENTRY(request) tq;
    };
    TAILQ_HEAD(reqhead, request) requests;
    struct request *req;

    TAILQ_INIT(&requests);

    a = do_stat(conn, remote_path, 0);
    if (a == NULL)
        return(-1);

    /* XXX: should we preserve set[ug]id? */
    if (a->flags & SSH2_FILEXFER_ATTR_PERMISSIONS)
        mode = a->perm & 0777;
    else
        mode = 0666;

    if ((a->flags & SSH2_FILEXFER_ATTR_PERMISSIONS) &&
        (!S_ISREG(a->perm))) {
        error("Cannot download non-regular file: %s", remote_path);
        return(-1);
    }

    if (a->flags & SSH2_FILEXFER_ATTR_SIZE)
        size = a->size;
    else
        size = 0;

    buflen = conn->transfer_buflen;
    buffer_init(&msg);

    /* Send open request */
    id = conn->msg_id++;
    buffer_put_char(&msg, SSH2_FXP_OPEN);
    buffer_put_int(&msg, id);
    buffer_put_cstring(&msg, remote_path);
    buffer_put_int(&msg, SSH2_FXF_READ);
    attrib_clear(&junk); /* Send empty attributes */
    encode_attrib(&msg, &junk);
#ifndef L7_SSHD
    send_msg(conn->fd_out, &msg);
#else
    if (send_msg(conn->fd_out, &msg) != 0)
    {
      error("send_msg failure");
      buffer_free(&msg);
      return -1;
    }
#endif /* L7_SSHD */
    debug3("Sent message SSH2_FXP_OPEN I:%u P:%s", id, remote_path);

    handle = get_handle(conn->fd_in, id, &handle_len);
    if (handle == NULL) {
        buffer_free(&msg);
        return(-1);
    }

#ifndef L7_SSHD
    local_fd = open(local_path, O_WRONLY | O_CREAT | O_TRUNC,
        mode | S_IWRITE);
#else
    if (osapiFsOpen(local_path, &local_fd) == L7_ERROR)
    {
      if (osapiFsCreateFile(local_path) != L7_SUCCESS)
      {
        local_fd = -1;
      }
      else
      {
        if (osapiFsOpen(local_path, &local_fd) == L7_ERROR)
        {
          local_fd = -1;
        }
      }
    }
#endif /* L7_SSHD */
    if (local_fd == -1) {
        error("Couldn't open local file \"%s\" for writing: %s",
            local_path, strerror(errno));
        buffer_free(&msg);
        xfree(handle);
        return(-1);
    }

    /* Read from remote and write to local */
    write_error = read_error = write_errno = num_req = offset = 0;
    max_req = 1;
    progress_counter = 0;

    if (showprogress && size != 0)
        start_progress_meter(remote_path, size, &progress_counter);
#ifdef L7_SSHD
    simTransferBytesCompletedSet(0);
#endif /* L7_SSHD */

    while (num_req > 0 || max_req > 0) {
        char *data;
        u_int len;

        /*
         * Simulate EOF on interrupt: stop sending new requests and
         * allow outstanding requests to drain gracefully
         */
        if (interrupted) {
            if (num_req == 0) /* If we haven't started yet... */
                break;
            max_req = 0;
        }

        /* Send some more requests */
        while (num_req < max_req) {
            debug3("Request range %llu -> %llu (%d/%d)",
                (unsigned long long)offset,
                (unsigned long long)offset + buflen - 1,
                num_req, max_req);
            req = xmalloc(sizeof(*req));
            req->id = conn->msg_id++;
            req->len = buflen;
            req->offset = offset;
            offset += buflen;
            num_req++;
            TAILQ_INSERT_TAIL(&requests, req, tq);
            send_read_request(conn->fd_out, req->id, req->offset,
                req->len, handle, handle_len);
        }

        buffer_clear(&msg);
#ifndef L7_SSHD
        get_msg(conn->fd_in, &msg);
#else
        if (get_msg(conn->fd_in, &msg) != 0)
        {
           error("get_msg failure");
           interrupted = 1;
           read_error = 1;
           status = -1;
           for (req = TAILQ_FIRST(&requests);
               req != NULL;
               req = TAILQ_NEXT(req, tq))
           {
             TAILQ_REMOVE(&requests, req, tq);
             xfree(req);
             num_req--;
           }
           break;
        }
#endif /* L7_SSHD */
        type = buffer_get_char(&msg);
        id = buffer_get_int(&msg);
        debug3("Received reply T:%u I:%u R:%d", type, id, max_req);

        /* Find the request in our queue */
        for (req = TAILQ_FIRST(&requests);
            req != NULL && req->id != id;
            req = TAILQ_NEXT(req, tq))
            ;
        if (req == NULL)
#ifndef L7_SSHD
            fatal("Unexpected reply %u", id);
#else
        {
          error("Unexpected reply %u", id);
          interrupted = 1;
          read_error = 1;
          status = -1;
          break;
        }
#endif /* L7_SSHD */

        switch (type) {
        case SSH2_FXP_STATUS:
            status = buffer_get_int(&msg);
            if (status != SSH2_FX_EOF)
                read_error = 1;
            max_req = 0;
            TAILQ_REMOVE(&requests, req, tq);
            xfree(req);
            num_req--;
            break;
        case SSH2_FXP_DATA:
            data = buffer_get_string(&msg, &len);
            debug3("Received data %llu -> %llu",
                (unsigned long long)req->offset,
                (unsigned long long)req->offset + len - 1);
            if (len > req->len)
#ifndef L7_SSHD
                fatal("Received more data than asked for "
                    "%u > %u", len, req->len);
#else
            {
              error("Received more data than asked for "
                  "%u > %u", len, req->len);
              xfree(data);
              interrupted = 1;
              read_error = 1;
              status = -1;
              break;
            }
#endif /* L7_SSHD */
            if ((lseek(local_fd, req->offset, SEEK_SET) == -1 ||
                atomicio(vwrite, local_fd, data, len) != len) &&
                !write_error) {
                write_errno = errno;
                write_error = 1;
                max_req = 0;
            }
            progress_counter += len;
#ifdef L7_SSHD
            simTransferBytesCompletedSet(progress_counter);
#endif /* L7_SSHD */
            xfree(data);

            if (len == req->len) {
                TAILQ_REMOVE(&requests, req, tq);
                xfree(req);
                num_req--;
            } else {
                /* Resend the request for the missing data */
                debug3("Short data block, re-requesting "
                    "%llu -> %llu (%2d)",
                    (unsigned long long)req->offset + len,
                    (unsigned long long)req->offset +
                    req->len - 1, num_req);
                req->id = conn->msg_id++;
                req->len -= len;
                req->offset += len;
                send_read_request(conn->fd_out, req->id,
                    req->offset, req->len, handle, handle_len);
                /* Reduce the request size */
                if (len < buflen)
                    buflen = MAX(MIN_READ_SIZE, len);
            }
            if (max_req > 0) { /* max_req = 0 iff EOF received */
                if (size > 0 && offset > size) {
                    /* Only one request at a time
                     * after the expected EOF */
                    debug3("Finish at %llu (%2d)",
                        (unsigned long long)offset,
                        num_req);
                    max_req = 1;
                } else if (max_req <= conn->num_requests) {
                    ++max_req;
                }
            }
            break;
        default:
#ifndef L7_SSHD
            fatal("Expected SSH2_FXP_DATA(%u) packet, got %u",
                SSH2_FXP_DATA, type);
#else
            error("Expected SSH2_FXP_DATA(%u) packet, got %u",
                SSH2_FXP_DATA, type);
            interrupted = 1;
            read_error = 1;
            status = -1;
            break;
#endif /* L7_SSHD */
        }
    }

    if (showprogress && size)
        stop_progress_meter();

    /* Sanity check */
    if (TAILQ_FIRST(&requests) != NULL)
#ifndef L7_SSHD
        fatal("Transfer complete, but requests still in queue");
#else
        error("Transfer complete, but requests still in queue");
#endif /* L7_SSHD */

    if (read_error) {
        error("Couldn't read from remote file \"%s\" : %s",
            remote_path, fx2txt(status));
        do_close(conn, handle, handle_len);
    } else if (write_error) {
        error("Couldn't write to \"%s\": %s", local_path,
            strerror(write_errno));
        status = -1;
        do_close(conn, handle, handle_len);
    } else {
        status = do_close(conn, handle, handle_len);
#ifndef L7_SSHD
        /* Override umask and utimes if asked */
#ifdef HAVE_FCHMOD
        if (pflag && fchmod(local_fd, mode) == -1)
#else
        if (pflag && chmod(local_path, mode) == -1)
#endif /* HAVE_FCHMOD */
            error("Couldn't set mode on \"%s\": %s", local_path,
                strerror(errno));
        if (pflag && (a->flags & SSH2_FILEXFER_ATTR_ACMODTIME)) {
            struct timeval tv[2];
            tv[0].tv_sec = a->atime;
            tv[1].tv_sec = a->mtime;
            tv[0].tv_usec = tv[1].tv_usec = 0;
            if (utimes(local_path, tv) == -1)
                error("Can't set times on \"%s\": %s",
                    local_path, strerror(errno));
        }
#endif /* L7_SSHD */
    }
#ifndef L7_SSHD
    close(local_fd);
#else
    osapiFsClose(local_fd);
#endif /* L7_SSHD */
    buffer_free(&msg);
    xfree(handle);

    return(status);
}

int
do_upload(struct sftp_conn *conn, char *local_path, char *remote_path,
    int pflag)
{
    int local_fd, status;
    u_int handle_len, id, type;
    u_int64_t offset;
    char *handle, *data;
    Buffer msg;
    struct stat sb;
    Attrib a;
    u_int32_t startid;
    u_int32_t ackid;
    struct outstanding_ack {
        u_int id;
        u_int len;
        u_int64_t offset;
        TAILQ_ENTRY(outstanding_ack) tq;
    };
    TAILQ_HEAD(ackhead, outstanding_ack) acks;
    struct outstanding_ack *ack = NULL;

    TAILQ_INIT(&acks);

#ifndef L7_SSHD
    if ((local_fd = open(local_path, O_RDONLY, 0)) == -1) {
#else
    if (osapiFsOpen(local_path, &local_fd) == L7_ERROR) {
#endif /* L7_SSHD */
        error("Couldn't open local file \"%s\" for reading: %s",
            local_path, strerror(errno));
        return(-1);
    }
    if (fstat(local_fd, &sb) == -1) {
        error("Couldn't fstat local file \"%s\": %s",
            local_path, strerror(errno));
#ifndef L7_SSHD
        close(local_fd);
#else
        osapiFsClose(local_fd);
#endif /* L7_SSHD */
        return(-1);
    }
    if (!S_ISREG(sb.st_mode)) {
        error("%s is not a regular file", local_path);
#ifndef L7_SSHD
        close(local_fd);
#else
        osapiFsClose(local_fd);
#endif /* L7_SSHD */
        return(-1);
    }
    stat_to_attrib(&sb, &a);

    a.flags &= ~SSH2_FILEXFER_ATTR_SIZE;
    a.flags &= ~SSH2_FILEXFER_ATTR_UIDGID;
    a.perm &= 0777;
    if (!pflag)
        a.flags &= ~SSH2_FILEXFER_ATTR_ACMODTIME;

    buffer_init(&msg);

    /* Send open request */
    id = conn->msg_id++;
    buffer_put_char(&msg, SSH2_FXP_OPEN);
    buffer_put_int(&msg, id);
    buffer_put_cstring(&msg, remote_path);
    buffer_put_int(&msg, SSH2_FXF_WRITE|SSH2_FXF_CREAT|SSH2_FXF_TRUNC);
    encode_attrib(&msg, &a);
#ifndef L7_SSHD
    send_msg(conn->fd_out, &msg);
#else
    if (send_msg(conn->fd_out, &msg) != 0)
    {
      error("send_msg failure");
      osapiFsClose(local_fd);
      buffer_free(&msg);
      return -1;
    }
#endif /* L7_SSHD */
    debug3("Sent message SSH2_FXP_OPEN I:%u P:%s", id, remote_path);

    buffer_clear(&msg);

    handle = get_handle(conn->fd_in, id, &handle_len);
    if (handle == NULL) {
#ifndef L7_SSHD
        close(local_fd);
#else
        osapiFsClose(local_fd);
#endif /* L7_SSHD */
        buffer_free(&msg);
        return(-1);
    }

    startid = ackid = id + 1;
    data = xmalloc(conn->transfer_buflen);

    /* Read from local and write to remote */
    offset = 0;
    if (showprogress)
#ifndef L7_SSHD
        start_progress_meter(local_path, sb.st_size, &offset);
#else
        start_progress_meter(local_path, sb.st_size, (off_t *)&offset);
#endif /* L7_SSHD */

#ifdef L7_SSHD
    simTransferBytesCompletedSet(0);
#endif /* L7_SSHD */
    for (;;) {
        int len;

        /*
         * Can't use atomicio here because it returns 0 on EOF,
         * thus losing the last block of the file.
         * Simulate an EOF on interrupt, allowing ACKs from the
         * server to drain.
         */
        if (interrupted)
            len = 0;
        else do
            len = read(local_fd, data, conn->transfer_buflen);
        while ((len == -1) && (errno == EINTR || errno == EAGAIN));

        if (len == -1)
#ifndef L7_SSHD
            fatal("Couldn't read from \"%s\": %s", local_path,
                  strerror(errno));
#else
        {
          error("Couldn't read from \"%s\": %s", local_path,
                strerror(errno));
          interrupted = 1;
          status = -1;
          break;
        }
#endif /* L7_SSHD */

        if (len != 0) {
            ack = xmalloc(sizeof(*ack));
            ack->id = ++id;
            ack->offset = offset;
            ack->len = len;
            TAILQ_INSERT_TAIL(&acks, ack, tq);

            buffer_clear(&msg);
            buffer_put_char(&msg, SSH2_FXP_WRITE);
            buffer_put_int(&msg, ack->id);
            buffer_put_string(&msg, handle, handle_len);
            buffer_put_int64(&msg, offset);
            buffer_put_string(&msg, data, len);
#ifndef L7_SSHD
            send_msg(conn->fd_out, &msg);
#else
            if (send_msg(conn->fd_out, &msg) != 0)
            {
              error("send_msg failure");
              interrupted = 1;
              status = -1;
              break;
            }
#endif /* L7_SSHD */
            debug3("Sent message SSH2_FXP_WRITE I:%u O:%llu S:%u",
                id, (unsigned long long)offset, len);
        } else if (TAILQ_FIRST(&acks) == NULL)
            break;

        if (ack == NULL)
#ifndef L7_SSHD
            fatal("Unexpected ACK %u", id);
#else
        {
          error("Unexpected ACK %u", id);
          interrupted = 1;
          status = -1;
          break;
        }
#endif /* L7_SSHD */

        if (id == startid || len == 0 ||
            id - ackid >= conn->num_requests) {
            u_int r_id;

            buffer_clear(&msg);
#ifndef L7_SSHD
            get_msg(conn->fd_in, &msg);
#else
            if (get_msg(conn->fd_in, &msg) != 0)
            {
              error("get_msg failed");
              interrupted = 1;
              status = -1;
              osapiFsClose(local_fd);
              close(local_fd);
              xfree(data);
              for (ack = TAILQ_FIRST(&acks);
                  ack != NULL;
                  ack = TAILQ_NEXT(ack, tq))
              {
                TAILQ_REMOVE(&acks, ack, tq);
                xfree(ack);
              }
              goto done;
            }
#endif /* L7_SSHD */
            type = buffer_get_char(&msg);
            r_id = buffer_get_int(&msg);

            if (type != SSH2_FXP_STATUS)
#ifndef L7_SSHD
                fatal("Expected SSH2_FXP_STATUS(%d) packet, "
                    "got %d", SSH2_FXP_STATUS, type);
#else
            {
              error("Expected SSH2_FXP_STATUS(%d) packet, "
                  "got %d", SSH2_FXP_STATUS, type);
              interrupted = 1;
              status = -1;
              osapiFsClose(local_fd);
              close(local_fd);
              xfree(data);
              for (ack = TAILQ_FIRST(&acks);
                  ack != NULL;
                  ack = TAILQ_NEXT(ack, tq))
              {
                TAILQ_REMOVE(&acks, ack, tq);
                xfree(ack);
              }
              goto done;
            }
#endif /* L7_SSHD */

            status = buffer_get_int(&msg);
            debug3("SSH2_FXP_STATUS %d", status);

            /* Find the request in our queue */
            for (ack = TAILQ_FIRST(&acks);
                ack != NULL && ack->id != r_id;
                ack = TAILQ_NEXT(ack, tq))
                ;
            if (ack == NULL)
#ifndef L7_SSHD
                fatal("Can't find request for ID %u", r_id);
#else
            {
              error("Can't find request for ID %u", r_id);
              interrupted = 1;
              status = -1;
              break;
            }
#endif /* L7_SSHD */
            TAILQ_REMOVE(&acks, ack, tq);

            if (status != SSH2_FX_OK) {
                error("Couldn't write to remote file \"%s\": %s",
                    remote_path, fx2txt(status));
                do_close(conn, handle, handle_len);
#ifndef L7_SSHD
                close(local_fd);
#else
                osapiFsClose(local_fd);
#endif /* L7_SSHD */
                close(local_fd);
                xfree(data);
                xfree(ack);
                goto done;
            }
            debug3("In write loop, ack for %u %u bytes at %llu",
                ack->id, ack->len, (unsigned long long)ack->offset);
            ++ackid;
            xfree(ack);
        }
        offset += len;
#ifdef L7_SSHD
        simTransferBytesCompletedSet(offset);
#endif /* L7_SSHD */
    }
    if (showprogress)
        stop_progress_meter();
    xfree(data);

#ifndef L7_SSHD
    if (close(local_fd) == -1) {
#else
    if (osapiFsClose(local_fd) == L7_ERROR) {
#endif /* L7_SSHD */
        error("Couldn't close local file \"%s\": %s", local_path,
            strerror(errno));
        do_close(conn, handle, handle_len);
        status = -1;
        goto done;
    }

    /* Override umask and utimes if asked */
    if (pflag)
        do_fsetstat(conn, handle, handle_len, &a);

    status = do_close(conn, handle, handle_len);

done:
    xfree(handle);
    buffer_free(&msg);
    return(status);
}
