/*
 * Copyright (c) 2003 Markus Friedl <markus@openbsd.org>
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
#include "includes.h"
RCSID("$OpenBSD: cleanup.c,v 1.1 2003/09/23 20:17:11 markus Exp $");

#include "log.h"

#ifdef L7_SSHD
#include "channels.h"
#include "auth.h"
#include "kex.h"
#include "session.h"
#include "xmalloc.h"
#include "packet.h"
#include "commdefs.h"
#include "osapi.h"

extern int connection_in[], connection_out[], compat13[], compat20[];
extern Authctxt *the_authctxt[];
extern Buffer loginmsg[];
extern Buffer stdin_buffer[];
extern Buffer stdout_buffer[];
extern Buffer stderr_buffer[];
extern int conn_taskid[];
extern void *sshdGlobalSema;
extern void *sshdExitSema;
extern char *sshd_server_version_string[], *sshd_client_version_string[];
extern char *canonical_host_ip[];
extern char *canonical_host_name[];
extern char *remote_ip[];
extern char *sshd_packet_buf[];
extern char *ssh_myproposal[L7_OPENSSH_MAX_CONNECTIONS][PROPOSAL_MAX];
extern int  sshd_exited;
extern void destroy_sensitive_data_cn(int);
extern void xxx_kex_free_cn(int);
extern void sshc_session_cleanup(int);
extern void sshd_session_cleanup(int);
extern void child_connect_context_cleanup(int cn);
extern int  child_connect_context_find(int taskId);
#endif /* L7_SSHD */

/* default implementation */
void
cleanup_exit(int i)
{
    int selfTaskId = osapiTaskIdSelf();
    int cn = -1;
    char taskName[128];
    memset(taskName, 0, sizeof(taskName));

#ifndef L7_SSHD
    _exit(i);
#else
    debugl7(SYSLOG_LEVEL_DEBUG3, "exiting with error code %d ...", i);

    if ((cn = child_connect_context_find(selfTaskId)) != -1)
    {
        cleanup_exit_cn(cn, i);
    }
    else
    {
      if ((osapiTaskNameGet(selfTaskId, taskName) == L7_SUCCESS) &&
         (strstr(taskName, "Transfer") == NULL))
      {
        debugl7(SYSLOG_LEVEL_DEBUG3, "cleanup_exit deleting task 0x%x...", selfTaskId);
        sshd_exited = 1;
        osapiTaskDelete(osapiTaskIdSelf());
      }
    }
#endif /* L7_SSHD */
}

#ifdef L7_SSHD
void
cleanup_exit_cn(int cn, int i)
{
    int taskId;
    int selfTaskId = osapiTaskIdSelf();

    osapiSemaTake(sshdExitSema, L7_WAIT_FOREVER);

    taskId = conn_taskid[cn];

    if (conn_taskid[cn] == 0)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: clean up already in progress ...", cn);
        osapiSemaGive(sshdExitSema);
        return;
    }
    conn_taskid[cn] = 0;
    child_connect_context_cleanup(cn);

    debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: cleaning up, task 0x%08x, self 0x%08x ...",
            cn, taskId, selfTaskId);
    if (selfTaskId != taskId)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: deleting conn task 0x%08x, error code %d ...", cn, taskId, i);
        osapiTaskDelete(taskId);
    }
    packet_close(cn);

    debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing channels ...", cn);
    channel_free_cn(cn);

    if (the_authctxt[cn])
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing sessions ...", cn);
        session_destroy_cn(cn, NULL);
        if (the_authctxt[cn]->user != NULL)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing the_authctxt[%d]->user ...", cn, cn);
            xfree(the_authctxt[cn]->user);
        }
        if (the_authctxt[cn]->service != NULL)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing the_authctxt[%d]->service ...", cn, cn);
            xfree(the_authctxt[cn]->service);
        }
        if (the_authctxt[cn]->method != NULL)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing the_authctxt[%d]->method ...", cn, cn);
            xfree(the_authctxt[cn]->method);
        }
        if (the_authctxt[cn]->style != NULL)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing the_authctxt[%d]->style ...", cn, cn);
            xfree(the_authctxt[cn]->style);
        }
        if (the_authctxt[cn]->pw != NULL)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing the_authctxt[%d]->pw ...", cn, cn);
            xfree(the_authctxt[cn]->pw);
        }
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing the_authctxt[%d] ...", cn, cn);
        xfree(the_authctxt[cn]);
        the_authctxt[cn] = NULL;
    }

    compat20[cn] = 0;
    compat13[cn] = 0;

    if (sshd_server_version_string[cn] != NULL)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing sshd_server_version_string[%d] ...", cn, cn);
        xfree(sshd_server_version_string[cn]);
    }
    sshd_server_version_string[cn] = NULL;
    if (sshd_client_version_string[cn] != NULL)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing sshd_client_version_string[%d] ...", cn, cn);
        xfree(sshd_client_version_string[cn]);
    }
    sshd_client_version_string[cn] = NULL;

    if (canonical_host_ip[cn] != NULL)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing canonical_host_ip[%d] ...", cn, cn);
        xfree(canonical_host_ip[cn]);
    }
    canonical_host_ip[cn] = NULL;
    if (remote_ip[cn] != NULL)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing remote_ip[%d] ...", cn, cn);
        xfree(remote_ip[cn]);
    }
    remote_ip[cn] = NULL;
    if (canonical_host_name[cn] != NULL)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing canonical_host_name[%d] ...", cn, cn);
        xfree(canonical_host_name[cn]);
    }
    canonical_host_name[cn] = NULL;
    if (sshd_packet_buf[cn] != NULL)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing sshd_packet_buf[%d] ...", cn, cn);
        xfree(sshd_packet_buf[cn]);
    }
    sshd_packet_buf[cn] = NULL;

    if (cn < SSHD_MAX_CONNECTIONS)
    {
        /*
        Only free this field for server connections
        */

        if (ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] != NULL)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: freeing ssh_myproposal[%d] ...", cn, cn);
            xfree(ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS]);
            ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] = NULL;
        }
    }

    /*
    xxx_kex_free_cn(cn);
    */
    buffer_free(&loginmsg[cn]);
    buffer_free(&stdin_buffer[cn]);
    buffer_free(&stdout_buffer[cn]);
    buffer_free(&stderr_buffer[cn]);
    sshd_session_cleanup(cn);
    if (selfTaskId == taskId)
    {
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: deleting self task 0x%08x, error code %d ...", cn, selfTaskId, i);
        osapiSemaGive(sshdExitSema);
        osapiTaskDelete(taskId);
        return;
    }
    osapiSemaGive(sshdExitSema);
}
#endif /* L7_SSHD */
