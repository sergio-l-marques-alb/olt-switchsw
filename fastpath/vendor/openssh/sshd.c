/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This program is the ssh daemon.  It listens for connections from clients,
 * and performs authentication, executes use commands or shell, and forwards
 * information to/from the application to the user client over an encrypted
 * connection.  This can also handle forwarding of X11, TCP/IP, and
 * authentication agent connections.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 implementation:
 * Privilege Separation:
 *
 * Copyright (c) 2000, 2001, 2002 Markus Friedl.  All rights reserved.
 * Copyright (c) 2002 Niels Provos.  All rights reserved.
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
RCSID("$OpenBSD: sshd.c,v 1.318 2005/12/24 02:27:41 djm Exp $");

#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#ifdef HAVE_SECUREWARE
#include <sys/security.h>
#include <prot.h>
#endif

#include "ssh.h"
#include "ssh1.h"
#include "ssh2.h"
#include "xmalloc.h"
#include "rsa.h"
#include "sshpty.h"
#include "packet.h"
#include "log.h"
#include "servconf.h"
#include "uidswap.h"
#include "compat.h"
#include "buffer.h"
#include "bufaux.h"
#include "cipher.h"
#include "kex.h"
#include "key.h"
#include "dh.h"
#include "myproposal.h"
#include "authfile.h"
#include "pathnames.h"
#include "atomicio.h"
#include "canohost.h"
#include "auth.h"
#include "misc.h"
#include "msg.h"
#include "dispatch.h"
#include "channels.h"
#include "session.h"
#include "monitor_mm.h"
#include "monitor.h"
#include "monitor_wrap.h"
#include "monitor_fdpass.h"

#ifdef L7_SSHD
#include "commdefs.h"
#include "osapi.h"
#include "sshd_cfg.h"
#include "../../src/l7public/common/log.h"
#endif /* L7_SSHD */

#ifdef LIBWRAP
#include <tcpd.h>
#include <syslog.h>
int allow_severity = LOG_INFO;
int deny_severity = LOG_WARNING;
#endif /* LIBWRAP */

#ifndef O_NOCTTY
#define O_NOCTTY    0
#endif

/* Re-exec fds */
#define REEXEC_DEVCRYPTO_RESERVED_FD    (STDERR_FILENO + 1)
#define REEXEC_STARTUP_PIPE_FD      (STDERR_FILENO + 2)
#define REEXEC_CONFIG_PASS_FD       (STDERR_FILENO + 3)
#define REEXEC_MIN_FREE_FD      (STDERR_FILENO + 4)

#ifndef L7_SSHD
extern char *__progname;
#else
extern int connection_in[], connection_out[], inout_v1[];
extern void         *sshdGlobalSema;
extern void         *sshdExitSema;
extern sshdGlobal_t sshdGlobal;
extern LogLevel log_level;
extern int sshd_exited;
extern u_int16_t *ssh_attack_detector;
#endif /* L7_SSHD */

/* Server configuration options. */
ServerOptions options;

/* Name of the server configuration file. */
char *config_file_name = _PATH_SERVER_CONFIG_FILE;

/*
 * Debug mode flag.  This can be set on the command line.  If debug
 * mode is enabled, extra debugging output will be sent to the system
 * log, the daemon will not go to background, and will exit after processing
 * the first connection.
 */
int debug_flag = 0;

/* Flag indicating that the daemon should only test the configuration and keys. */
int test_flag = 0;

/* Flag indicating that the daemon is being started from inetd. */
int inetd_flag = 0;

/* Flag indicating that sshd should not detach and become a daemon. */
int no_daemon_flag = 0;

/* debug goes to stderr unless inetd_flag is set */
int log_stderr = 0;

/* Saved arguments to main(). */
char **saved_argv;
int saved_argc;

/* re-exec */
int rexeced_flag = 0;
#ifndef L7_SSHD
int rexec_flag = 1;
#else
int rexec_flag = 0;
#endif /* L7_SSHD */
int rexec_argc = 0;
char **rexec_argv;

/*
 * The sockets that the server is listening; this is used in the SIGHUP
 * signal handler.
 */
#define MAX_LISTEN_SOCKS    16
int listen_socks[MAX_LISTEN_SOCKS] = {0};
int num_listen_socks = 0;

/*
 * the client's version string, passed by sshd2 in compat mode. if != NULL,
 * sshd will skip the version-number exchange
 */
#ifndef L7_SSHD
char *client_version_string = NULL;
char *server_version_string = NULL;
#else
char *sshd_client_version_string[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};
char *sshd_server_version_string[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};
int sshd_exit_flag = 0;
struct sockaddr_storage client_peers[L7_OPENSSH_MAX_CONNECTIONS];
int conn_taskid[L7_OPENSSH_MAX_CONNECTIONS] = {0};
char *ssh_myproposal[L7_OPENSSH_MAX_CONNECTIONS][PROPOSAL_MAX];

static void close_listen_socks(void);

void sshd_var_init(void)
{
    int i;

    /*
    Set data for SSH that is not 0 default

    sshdExitSema and sshdGlobalSema should already be taken when
    entering this function
    */

    for (i = 0; i < SSHD_MAX_CONNECTIONS; i++)
    {
        connection_in[i] = -1;
        connection_out[i] = -1;
        max_packet_size[i] = 32768;
        inout_v1[i] = -1;
    }

}

void sshd_dounconfig(void)
{
    int i;

    debugl7(SYSLOG_LEVEL_INFO, "sshd_dounconfig: shutting down connections ...");

    /*
    Note that sshdGlobalSema should already be taken before entering this
    function
    */

    for (i = 0; i < SSHD_MAX_CONNECTIONS; i++)
    {
        if ((connection_out[i] != -1) && (connection_out[i] != 0))
        {
            debugl7(SYSLOG_LEVEL_INFO, "sshd_dounconfig: shutting down connection_out %d, socket %d ...",
                    i, connection_out[i]);
            shutdown(connection_out[i], SHUT_RDWR);
        }

        if ((connection_in[i] != -1) && (connection_in[i] != 0) && (connection_in[i] != connection_out[i]))
        {
            debugl7(SYSLOG_LEVEL_INFO, "sshd_dounconfig: shutting down connection_in %d, socket %d ...",
                    i, connection_in[i]);
            shutdown(connection_in[i], SHUT_RDWR);
        }
    }
}

void sshd_doquit(int sig)
{
    sshd_exit_flag = 1;

    debugl7(SYSLOG_LEVEL_INFO, "sshd_doquit: Quitting sshd listen task ...");

    close_listen_socks();

    debugl7(SYSLOG_LEVEL_INFO, "sshd_doquit: sshd_exit_flag now %d ...", sshd_exit_flag);
}
void sshd_undoquit(void)
{
    sshd_exit_flag = 0;
    num_listen_socks = 0;

    debugl7(SYSLOG_LEVEL_INFO, "sshd_undoquit: sshd_exit_flag now %d ...", sshd_exit_flag);
}
#endif /* L7_SSHD */

/* for rekeying XXX fixme */
extern Kex *xxx_kex[];

/*
 * Any really sensitive data in the application is contained in this
 * structure. The idea is that this structure could be locked into memory so
 * that the pages do not get written into swap.  However, there are some
 * problems. The private key contains BIGNUMs, and we do not (in principle)
 * have access to the internals of them, and locking just the structure is
 * not very useful.  Currently, memory locking is not implemented.
 */
#ifndef L7_SSHD
struct {
    Key *server_key;        /* ephemeral server key */
    Key *ssh1_host_key;     /* ssh1 host key */
    Key **host_keys;        /* all private host keys */
    int have_ssh1_key;
    int have_ssh2_key;
    u_char  ssh1_cookie[SSH_SESSION_KEY_LENGTH];
} sensitive_data;
#else
typedef struct l7_sensitive_data
{
    Key *server_key;        /* ephemeral server key */
    Key *ssh1_host_key;     /* ssh1 host key */
    Key **host_keys;        /* all private host keys */
    int have_ssh1_key;
    int have_ssh2_key;
    u_char  ssh1_cookie[SSH_SESSION_KEY_LENGTH];
} sensitive_data_t;

sensitive_data_t sensitive_data;
#endif /* L7_SSHD */

/*
 * Flag indicating whether the RSA server key needs to be regenerated.
 * Is set in the SIGALRM handler and cleared when the key is regenerated.
 */
static volatile sig_atomic_t key_do_regen = 0;

/* This is set to true when a signal is received. */
static volatile sig_atomic_t received_sighup = 0;
static volatile sig_atomic_t received_sigterm = 0;

/* session identifier, used by RSA-auth */
#ifndef L7_SSHD
u_char session_id[16];

/* same for ssh2 */
u_char *session_id2 = NULL;
u_int session_id2_len = 0;
#else
extern u_char session_id[L7_OPENSSH_MAX_CONNECTIONS][16];

/* same for ssh2 */
u_char *session_id2[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};
u_int session_id2_len[L7_OPENSSH_MAX_CONNECTIONS] = {0};
#endif /* L7_SSHD */

/* record remote hostname or ip */
u_int utmp_len = MAXHOSTNAMELEN;

/* options.max_startup sized array of fd ints */
int *startup_pipes = NULL;
int startup_pipe;       /* in child */

/* variables used for privilege separation */
#ifndef L7_SSHD
int use_privsep;
struct monitor *pmonitor = NULL;
#else
int use_privsep = 0;
#endif /* L7_SSHD */

/* global authentication context */
#ifndef L7_SSHD
Authctxt *the_authctxt = NULL;
#else
Authctxt *the_authctxt[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};
#endif /* L7_SSHD */

/* message to be displayed after login */
Buffer loginmsg[L7_OPENSSH_MAX_CONNECTIONS];

#ifdef L7_SSHD
typedef struct child_context
{
    int                 sock_in;
    int                 sock_out;
    int                 cn;
    int                 selfTaskId;
    osapiTimerDescr_t   *pLoginTimerHolder;
    sensitive_data_t    child_sensitive_data;
} child_context_t;
child_context_t child_connect_context[L7_OPENSSH_MAX_CONNECTIONS];

osapiTimerDescr_t *pKeyRegenTimerHolder;
#endif /* L7_SSHD */

/* Prototypes for various functions defined later in this file. */
void destroy_sensitive_data(void);
void demote_sensitive_data(void);

#ifndef L7_SSHD
static void do_ssh1_kex(void);
static void do_ssh2_kex(void);
#else
static void do_ssh1_kex(int cn, sensitive_data_t sensitive_data);
static void do_ssh2_kex(int cn);
#endif /* L7_SSHD */

/*
 * Close all listening sockets
 */
static void
close_listen_socks(void)
{
    int i;

#ifdef L7_SSHD
    if (num_listen_socks == 0)
    {
        debugl7(SYSLOG_LEVEL_OPEN, "no listen sockets should be open");
    }
#endif /* L7_SSHD */

    for (i = 0; i < MAX_LISTEN_SOCKS; i++)
#ifndef L7_SSHD
        close(listen_socks[i]);
#else
    {
        if (listen_socks[i] != 0)
        {
            debugl7(SYSLOG_LEVEL_OPEN, "closing listen_socks[%d] %d", i, listen_socks[i]);
            close(listen_socks[i]);
        }
        listen_socks[i] = 0;
    }
#endif /* L7_SSHD */
    num_listen_socks = 0;
}

#ifndef L7_SSHD
static void
close_startup_pipes(void)
{
    int i;

    if (startup_pipes)
        for (i = 0; i < options.max_startups; i++)
            if (startup_pipes[i] != -1)
                close(startup_pipes[i]);
}

/*
 * Signal handler for SIGHUP.  Sshd execs itself when it receives SIGHUP;
 * the effect is to reread the configuration file (and to regenerate
 * the server key).
 */
static void
sighup_handler(int sig)
{
    int save_errno = errno;

    received_sighup = 1;
    signal(SIGHUP, sighup_handler);
    errno = save_errno;
}

/*
 * Called from the main program after receiving SIGHUP.
 * Restarts the server.
 */
static void
sighup_restart(void)
{
    logit("Received SIGHUP; restarting.");
    close_listen_socks();
    close_startup_pipes();
    execv(saved_argv[0], saved_argv);
    logit("RESTART FAILED: av[0]='%.100s', error: %.100s.", saved_argv[0],
        strerror(errno));
    exit(1);
}

/*
 * Generic signal handler for terminating signals in the master daemon.
 */
static void
sigterm_handler(int sig)
{
    received_sigterm = sig;
}

/*
 * SIGCHLD handler.  This is called whenever a child dies.  This will then
 * reap any zombies left by exited children.
 */
static void
main_sigchld_handler(int sig)
{
    int save_errno = errno;
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0 ||
        (pid < 0 && errno == EINTR))
        ;

    signal(SIGCHLD, main_sigchld_handler);
    errno = save_errno;
}

/*
 * Signal handler for the alarm after the login grace period has expired.
 */
static void
grace_alarm_handler(int sig)
{
    /* XXX no idea how fix this signal handler */

    if (use_privsep && pmonitor != NULL && pmonitor->m_pid > 0)
        kill(pmonitor->m_pid, SIGALRM);

    /* Log error and exit. */
    fatal("Timeout before authentication for %s", get_remote_ipaddr());
}
#endif /* L7_SSHD */

#ifdef L7_SSHD
void
grace_alarm_handler(int cn, int int_context)
{
    child_context_t *context = (child_context_t *) int_context;

    /* Log error and exit. */
    debugl7(SYSLOG_LEVEL_FATAL|SYSLOG_LEVEL_TIMER, "connection %d: Timeout before authentication for %s", cn, get_remote_ipaddr(cn));

    if (sshdLogLevelCheck(SYSLOG_LEVEL_ALL) && !sshdLogLevelCheck(SYSLOG_LEVEL_TIMER))
    {
        osapiSemaTake(sshdExitSema, L7_WAIT_FOREVER);
        if (context->pLoginTimerHolder != NULL)
        {
            context->pLoginTimerHolder = NULL;
        }
        osapiSemaGive(sshdExitSema);
        return;
    }

    osapiSemaTake(sshdExitSema, L7_WAIT_FOREVER);
    if (context->pLoginTimerHolder != NULL)
    {
        context->pLoginTimerHolder = NULL;
        osapiSemaGive(sshdExitSema);
        /*
        cleanup_exit_cn(cn, 255);
        */
        if ((connection_out[cn] != -1) && (connection_out[cn] != 0))
        {
            debugl7(SYSLOG_LEVEL_INFO, "grace_alarm_handler: shutting down connection_out %d, socket %d ...",
                    cn, connection_out[cn]);
            shutdown(connection_out[cn], SHUT_RDWR);
        }

        if ((connection_in[cn] != -1) && (connection_in[cn] != 0) && (connection_in[cn] != connection_out[cn]))
        {
            debugl7(SYSLOG_LEVEL_INFO, "sshd_dounconfig: shutting down connection_in %d, socket %d ...",
                    cn, connection_in[cn]);
            shutdown(connection_in[cn], SHUT_RDWR);
        }

        return;
    }

    osapiSemaGive(sshdExitSema);
}
#endif /* L7_SSHD */

/*
 * Signal handler for the key regeneration alarm.  Note that this
 * alarm only occurs in the daemon waiting for connections, and it does not
 * do anything with the private key or random state before forking.
 * Thus there should be no concurrency control/asynchronous execution
 * problems.
 */
static void
generate_ephemeral_server_key(void)
{
    u_int32_t rnd = 0;
    int i;

    verbose("Generating %s%d bit RSA key.",
        sensitive_data.server_key != NULL ? "new " : "replacement ", options.server_key_bits);
    if (sensitive_data.server_key != NULL)
    {
        verbose("Freeing old %d bit RSA key", options.server_key_bits);
        key_free(sensitive_data.server_key);
    }
    sensitive_data.server_key = key_generate(KEY_RSA1,
        options.server_key_bits);
    verbose("RSA key generation complete.");

    for (i = 0; i < SSH_SESSION_KEY_LENGTH; i++) {
        if (i % 4 == 0)
            rnd = arc4random();
        sensitive_data.ssh1_cookie[i] = rnd & 0xff;
        rnd >>= 8;
    }
#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_VERBOSE, "Re-initializing random number generator ...");
#endif /* L7_SSHD */
    arc4random_stir();
}

#ifndef L7_SSHD
static void
key_regeneration_alarm(int sig)
{
    int save_errno = errno;

    signal(SIGALRM, SIG_DFL);
    errno = save_errno;
    key_do_regen = 1;
}
#else
void
key_regeneration_alarm(void)
{
    pKeyRegenTimerHolder = NULL;
    key_do_regen = 1;
}
#endif /* L7_SSHD */

static void
sshd_exchange_identification(int cn, int sock_in, int sock_out)
{
    u_int i;
    int mismatch;
    int remote_major, remote_minor;
    int major, minor;
    char *s;
    char buf[256];          /* Must not be larger than remote_version. */
    char remote_version[256];   /* Must be at least as big as buf. */

    if ((options.protocol & SSH_PROTO_1) &&
        (options.protocol & SSH_PROTO_2)) {
        major = PROTOCOL_MAJOR_1;
        minor = 99;
    } else if (options.protocol & SSH_PROTO_2) {
        major = PROTOCOL_MAJOR_2;
        minor = PROTOCOL_MINOR_2;
    } else {
        major = PROTOCOL_MAJOR_1;
        minor = PROTOCOL_MINOR_1;
    }
#ifndef L7_SSHD
    snprintf(buf, sizeof buf, "SSH-%d.%d-%.100s\n", major, minor, SSH_VERSION);
    server_version_string = xstrdup(buf);
#else
    osapiSnprintf(buf, sizeof buf, "SSH-%d.%d-%.100s\n", major, minor, SSH_VERSION);
    sshd_server_version_string[cn] = xstrdup(buf);
#endif /* L7_SSHD */

    /* Send our protocol version identification. */
#ifndef L7_SSHD
    if (atomicio(vwrite, sock_out, server_version_string,
        strlen(server_version_string))
        != strlen(server_version_string)) {
        logit("Could not write ident string to %s", get_remote_ipaddr());
        cleanup_exit(255);
    }
#else
    if (atomicio(vwrite, sock_out, sshd_server_version_string[cn],
        strlen(sshd_server_version_string[cn]))
        != strlen(sshd_server_version_string[cn])) {
        logit("Could not write ident string to %s", get_remote_ipaddr(cn));
        cleanup_exit_cn(cn, 255);
    }
#endif /* L7_SSHD */

    /* Read other sides version identification. */
    memset(buf, 0, sizeof(buf));
    for (i = 0; i < sizeof(buf) - 1; i++) {
#ifndef L7_SSHD
        if (atomicio(read, sock_in, &buf[i], 1) != 1) {
            logit("Did not receive identification string from %s",
                get_remote_ipaddr());
            cleanup_exit(255);
        }
#else
        if (atomicio(vread, sock_in, &buf[i], 1) != 1) {
            logit("Did not receive identification string from %s",
                get_remote_ipaddr(cn));
            cleanup_exit_cn(cn, 255);
        }
#endif /* L7_SSHD */
        if (buf[i] == '\r') {
            buf[i] = 0;
            /* Kludge for F-Secure Macintosh < 1.0.2 */
            if (i == 12 &&
                strncmp(buf, "SSH-1.5-W1.0", 12) == 0)
                break;
            continue;
        }
        if (buf[i] == '\n') {
            buf[i] = 0;
            break;
        }
    }
    buf[sizeof(buf) - 1] = 0;
#ifndef L7_SSHD
    client_version_string = xstrdup(buf);
#else
    sshd_client_version_string[cn] = xstrdup(buf);
#endif /* L7_SSHD */

    /*
     * Check that the versions match.  In future this might accept
     * several versions and set appropriate flags to handle them.
     */
#ifndef L7_SSHD
    if (sscanf(client_version_string, "SSH-%d.%d-%[^\n]\n",
#else
    if (sscanf(sshd_client_version_string[cn], "SSH-%d.%d-%[^\n]\n",
#endif /* L7_SSHD */
        &remote_major, &remote_minor, remote_version) != 3) {
        s = "Protocol mismatch.\n";
        (void) atomicio(vwrite, sock_out, s, strlen(s));
#ifdef L7_SSHD
        debugl7(SYSLOG_LEVEL_OPEN, "closing sock_in %d, sock_out %d", sock_in, sock_out);
#endif /* L7_SSHD */
        close(sock_in);
#ifndef L7_SSHD
        close(sock_out);
#else
        if (sock_out != sock_in)
        {
            close(sock_out);
        }
#endif /* L7_SSHD */
        logit("Bad protocol version identification '%.100s' from %s",
#ifndef L7_SSHD
            client_version_string, get_remote_ipaddr());
        cleanup_exit(255);
#else
            sshd_client_version_string[cn], get_remote_ipaddr(cn));
        cleanup_exit_cn(cn, 255);
#endif /* L7_SSHD */
    }
    debug("Client protocol version %d.%d; client software version %.100s",
        remote_major, remote_minor, remote_version);

#ifndef L7_SSHD
    compat_datafellows(remote_version);
#else
    compat_datafellows(cn, remote_version);
#endif /* L7_SSHD */

#ifndef L7_SSHD
    if (datafellows & SSH_BUG_PROBE) {
        logit("probed from %s with %s.  Don't panic.",
            get_remote_ipaddr(), client_version_string);
        cleanup_exit(255);
    }

    if (datafellows & SSH_BUG_SCANNER) {
        logit("scanned from %s with %s.  Don't panic.",
            get_remote_ipaddr(), client_version_string);
        cleanup_exit(255);
    }
#else
    if (datafellows[cn] & SSH_BUG_PROBE) {
        logit("probed from %s with %s.  Don't panic.",
            get_remote_ipaddr(cn), sshd_client_version_string[cn]);
        cleanup_exit_cn(cn, 255);
    }

    if (datafellows[cn] & SSH_BUG_SCANNER) {
        logit("scanned from %s with %s.  Don't panic.",
            get_remote_ipaddr(cn), sshd_client_version_string[cn]);
        cleanup_exit_cn(cn, 255);
    }
#endif /* L7_SSHD */

    mismatch = 0;
    switch (remote_major) {
    case 1:
        if (remote_minor == 99) {
            if (options.protocol & SSH_PROTO_2)
#ifndef L7_SSHD
                enable_compat20();
#else
                enable_compat20(cn);
#endif /* L7_SSHD */
            else
                mismatch = 1;
            break;
        }
        if (!(options.protocol & SSH_PROTO_1)) {
            mismatch = 1;
            break;
        }
        if (remote_minor < 3) {
#ifndef L7_SSHD
            packet_disconnect("Your ssh version is too old and "
                "is no longer supported.  Please install a newer version.");
#else
            packet_disconnect(cn, "Your ssh version is too old and "
                "is no longer supported.  Please install a newer version.");
#endif /* L7_SSHD */
        } else if (remote_minor == 3) {
            /* note that this disables agent-forwarding */
#ifndef L7_SSHD
            enable_compat13();
#else
            enable_compat13(cn);
#endif /* L7_SSHD */
        }
        break;
    case 2:
        if (options.protocol & SSH_PROTO_2) {
#ifndef L7_SSHD
            enable_compat20();
#else
            enable_compat20(cn);
#endif /* L7_SSHD */
            break;
        }
        /* FALLTHROUGH */
    default:
        mismatch = 1;
        break;
    }
#ifndef L7_SSHD
    chop(server_version_string);
    debug("Local version string %.200s", server_version_string);
#else
    chop(sshd_server_version_string[cn]);
    debugl7(SYSLOG_LEVEL_INFO, "Local version string %.200s", sshd_server_version_string[cn]);
#endif /* L7_SSHD */

    if (mismatch) {
        s = "Protocol major versions differ.\n";
        (void) atomicio(vwrite, sock_out, s, strlen(s));
#ifdef L7_SSHD
        debugl7(SYSLOG_LEVEL_OPEN, "closing sock_in %d, sock_out %d", sock_in, sock_out);
#endif /* L7_SSHD */
        close(sock_in);
#ifndef L7_SSHD
        close(sock_out);
#else
        if (sock_out != sock_in)
        {
            close(sock_out);
        }
#endif /* L7_SSHD */
        logit("Protocol major versions differ for %s: %.200s vs. %.200s",
#ifndef L7_SSHD
              get_remote_ipaddr(),
            server_version_string, client_version_string);
        cleanup_exit(255);
#else
            get_remote_ipaddr(cn),
            sshd_server_version_string[cn], sshd_client_version_string[cn]);
        cleanup_exit_cn(cn, 255);
#endif /* L7_SSHD */
    }
}

/* Destroy the host and server keys.  They will no longer be needed. */
void
destroy_sensitive_data(void)
{
    int i;

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "destroying main sensitive data ...");
#endif /* L7_SSHD */
    if (sensitive_data.server_key) {
        key_free(sensitive_data.server_key);
        sensitive_data.server_key = NULL;
    }
    for (i = 0; i < options.num_host_key_files; i++) {
        if (sensitive_data.host_keys[i]) {
            key_free(sensitive_data.host_keys[i]);
            sensitive_data.host_keys[i] = NULL;
        }
    }
    sensitive_data.ssh1_host_key = NULL;
    memset(sensitive_data.ssh1_cookie, 0, SSH_SESSION_KEY_LENGTH);
}

#ifdef L7_SSHD
void
destroy_sensitive_data_cn(int cn)
{
    debugl7(SYSLOG_LEVEL_INFO, "connection %d: destroying sensitive data ...", cn);

    if (child_connect_context[cn].child_sensitive_data.server_key)
    {
        debugl7(SYSLOG_LEVEL_INFO, "connection %d: free child sensitive data server key ...", cn);
        key_free(child_connect_context[cn].child_sensitive_data.server_key);
        child_connect_context[cn].child_sensitive_data.server_key = NULL;
    }

    memset(&child_connect_context[cn].child_sensitive_data, 0, sizeof(sensitive_data_t));
}
#endif /* L7_SSHD */

/* Demote private to public keys for network child */
void
demote_sensitive_data(void)
{
    Key *tmp;
    int i;

    if (sensitive_data.server_key) {
        tmp = key_demote(sensitive_data.server_key);
        key_free(sensitive_data.server_key);
        sensitive_data.server_key = tmp;
    }

    for (i = 0; i < options.num_host_key_files; i++) {
        if (sensitive_data.host_keys[i]) {
            tmp = key_demote(sensitive_data.host_keys[i]);
            key_free(sensitive_data.host_keys[i]);
            sensitive_data.host_keys[i] = tmp;
            if (tmp->type == KEY_RSA1)
                sensitive_data.ssh1_host_key = tmp;
        }
    }

    /* We do not clear ssh1_host key and cookie.  XXX - Okay Niels? */
}

#ifndef L7_SSHD
static void
privsep_preauth_child(void)
{
    u_int32_t rnd[256];
    gid_t gidset[1];
    struct passwd *pw;
    int i;

    /* Enable challenge-response authentication for privilege separation */
    privsep_challenge_enable();

    for (i = 0; i < 256; i++)
        rnd[i] = arc4random();
    RAND_seed(rnd, sizeof(rnd));

    /* Demote the private keys to public keys. */
    demote_sensitive_data();

    if ((pw = getpwnam(SSH_PRIVSEP_USER)) == NULL)
        fatal("Privilege separation user %s does not exist",
            SSH_PRIVSEP_USER);
    memset(pw->pw_passwd, 0, strlen(pw->pw_passwd));
    endpwent();

    /* Change our root directory */
    if (chroot(_PATH_PRIVSEP_CHROOT_DIR) == -1)
        fatal("chroot(\"%s\"): %s", _PATH_PRIVSEP_CHROOT_DIR,
            strerror(errno));
    if (chdir("/") == -1)
        fatal("chdir(\"/\"): %s", strerror(errno));

    /* Drop our privileges */
    debug3("privsep user:group %u:%u", (u_int)pw->pw_uid,
        (u_int)pw->pw_gid);
#if 0
    /* XXX not ready, too heavy after chroot */
    do_setusercontext(pw);
#else
    gidset[0] = pw->pw_gid;
    if (setgroups(1, gidset) < 0)
        fatal("setgroups: %.100s", strerror(errno));
    permanently_set_uid(pw);
#endif
}

static int
privsep_preauth(Authctxt *authctxt)
{
    int status;
    pid_t pid;

    /* Set up unprivileged child process to deal with network data */
    pmonitor = monitor_init();
    /* Store a pointer to the kex for later rekeying */
    pmonitor->m_pkex = &xxx_kex;

    pid = fork();
    if (pid == -1) {
        fatal("fork of unprivileged child failed");
    } else if (pid != 0) {
        debug2("Network child is on pid %ld", (long)pid);

        close(pmonitor->m_recvfd);
        pmonitor->m_pid = pid;
        monitor_child_preauth(authctxt, pmonitor);
        close(pmonitor->m_sendfd);

        /* Sync memory */
        monitor_sync(pmonitor);

        /* Wait for the child's exit status */
        while (waitpid(pid, &status, 0) < 0)
            if (errno != EINTR)
                break;
        return (1);
    } else {
        /* child */

        close(pmonitor->m_sendfd);

        /* Demote the child */
        if (getuid() == 0 || geteuid() == 0)
            privsep_preauth_child();
        setproctitle("%s", "[net]");
    }
    return (0);
}

static void
privsep_postauth(Authctxt *authctxt)
{
#ifdef DISABLE_FD_PASSING
    if (1) {
#else
    if (authctxt->pw->pw_uid == 0 || options.use_login) {
#endif
        /* File descriptor passing is broken or root login */
        use_privsep = 0;
        goto skip;
    }

    /* New socket pair */
    monitor_reinit(pmonitor);

    pmonitor->m_pid = fork();
    if (pmonitor->m_pid == -1)
        fatal("fork of unprivileged child failed");
    else if (pmonitor->m_pid != 0) {
        debug2("User child is on pid %ld", (long)pmonitor->m_pid);
        close(pmonitor->m_recvfd);
        buffer_clear(&loginmsg);
        monitor_child_postauth(pmonitor);

        /* NEVERREACHED */
        exit(0);
    }

    close(pmonitor->m_sendfd);

    /* Demote the private keys to public keys. */
    demote_sensitive_data();

    /* Drop privileges */
    do_setusercontext(authctxt->pw);

 skip:
    /* It is safe now to apply the key state */
    monitor_apply_keystate(pmonitor);

    /*
     * Tell the packet layer that authentication was successful, since
     * this information is not part of the key state.
     */
    packet_set_authenticated();
}
#endif /* L7_SSHD */

static char *
list_hostkey_types(void)
{
    Buffer b;
    const char *p;
    char *ret;
    int i;

    buffer_init(&b);
    for (i = 0; i < options.num_host_key_files; i++) {
        Key *key = sensitive_data.host_keys[i];
        if (key == NULL)
            continue;
        switch (key->type) {
        case KEY_RSA:
        case KEY_DSA:
            if (buffer_len(&b) > 0)
                buffer_append(&b, ",", 1);
            p = key_ssh_name(key);
            buffer_append(&b, p, strlen(p));
            break;
        }
    }
    buffer_append(&b, "\0", 1);
    ret = xstrdup(buffer_ptr(&b));
    buffer_free(&b);
    debug("list_hostkey_types: %s", ret);
    return ret;
}

Key *
get_hostkey_by_type(int type)
{
    int i;

    for (i = 0; i < options.num_host_key_files; i++) {
        Key *key = sensitive_data.host_keys[i];
        if (key != NULL && key->type == type)
            return key;
    }
    return NULL;
}

Key *
get_hostkey_by_index(int ind)
{
    if (ind < 0 || ind >= options.num_host_key_files)
        return (NULL);
    return (sensitive_data.host_keys[ind]);
}

int
get_hostkey_index(Key *key)
{
    int i;

    for (i = 0; i < options.num_host_key_files; i++) {
        if (key == sensitive_data.host_keys[i])
            return (i);
    }
    return (-1);
}

/*
 * returns 1 if connection should be dropped, 0 otherwise.
 * dropping starts at connection #max_startups_begin with a probability
 * of (max_startups_rate/100). the probability increases linearly until
 * all connections are dropped for startups > max_startups
 */
static int
drop_connection(int startups)
{
    int p, r;

    if (startups < options.max_startups_begin)
        return 0;
    if (startups >= options.max_startups)
        return 1;
    if (options.max_startups_rate == 100)
        return 1;

    p  = 100 - options.max_startups_rate;
    p *= startups - options.max_startups_begin;
    p /= options.max_startups - options.max_startups_begin;
    p += options.max_startups_rate;
    r = arc4random() % 100;

    debug("drop_connection: p %d, r %d", p, r);
    return (r < p) ? 1 : 0;
}

#ifndef L7_SSHD
static void
usage(void)
{
    fprintf(stderr, "%s, %s\n",
        SSH_RELEASE, SSLeay_version(SSLEAY_VERSION));
    fprintf(stderr,
"usage: sshd [-46Ddeiqt] [-b bits] [-f config_file] [-g login_grace_time]\n"
"            [-h host_key_file] [-k key_gen_time] [-o option] [-p port] [-u len]\n"
    );
    exit(1);
}

static void
send_rexec_state(int fd, Buffer *conf)
{
    Buffer m;

    debug3("%s: entering fd = %d config len %d", __func__, fd,
        buffer_len(conf));

    /*
     * Protocol from reexec master to child:
     *  string  configuration
     *  u_int   ephemeral_key_follows
     *  bignum  e       (only if ephemeral_key_follows == 1)
     *  bignum  n           "
     *  bignum  d           "
     *  bignum  iqmp            "
     *  bignum  p           "
     *  bignum  q           "
     *  string rngseed      (only if OpenSSL is not self-seeded)
     */
    buffer_init(&m);
    buffer_put_cstring(&m, buffer_ptr(conf));

    if (sensitive_data.server_key != NULL &&
        sensitive_data.server_key->type == KEY_RSA1) {
        buffer_put_int(&m, 1);
        buffer_put_bignum(&m, sensitive_data.server_key->rsa->e);
        buffer_put_bignum(&m, sensitive_data.server_key->rsa->n);
        buffer_put_bignum(&m, sensitive_data.server_key->rsa->d);
        buffer_put_bignum(&m, sensitive_data.server_key->rsa->iqmp);
        buffer_put_bignum(&m, sensitive_data.server_key->rsa->p);
        buffer_put_bignum(&m, sensitive_data.server_key->rsa->q);
    } else
        buffer_put_int(&m, 0);

#ifndef OPENSSL_PRNG_ONLY
    rexec_send_rng_seed(&m);
#endif

    if (ssh_msg_send(fd, 0, &m) == -1)
        fatal("%s: ssh_msg_send failed", __func__);

    buffer_free(&m);

    debug3("%s: done", __func__);
}

static void
recv_rexec_state(int fd, Buffer *conf)
{
    Buffer m;
    char *cp;
    u_int len;

    debug3("%s: entering fd = %d", __func__, fd);

    buffer_init(&m);

    if (ssh_msg_recv(fd, &m) == -1)
        fatal("%s: ssh_msg_recv failed", __func__);
    if (buffer_get_char(&m) != 0)
        fatal("%s: rexec version mismatch", __func__);

    cp = buffer_get_string(&m, &len);
    if (conf != NULL)
        buffer_append(conf, cp, len + 1);
    xfree(cp);

    if (buffer_get_int(&m)) {
        if (sensitive_data.server_key != NULL)
            key_free(sensitive_data.server_key);
        sensitive_data.server_key = key_new_private(KEY_RSA1);
        buffer_get_bignum(&m, sensitive_data.server_key->rsa->e);
        buffer_get_bignum(&m, sensitive_data.server_key->rsa->n);
        buffer_get_bignum(&m, sensitive_data.server_key->rsa->d);
        buffer_get_bignum(&m, sensitive_data.server_key->rsa->iqmp);
        buffer_get_bignum(&m, sensitive_data.server_key->rsa->p);
        buffer_get_bignum(&m, sensitive_data.server_key->rsa->q);
        rsa_generate_additional_parameters(
            sensitive_data.server_key->rsa);
    }

#ifndef OPENSSL_PRNG_ONLY
    rexec_recv_rng_seed(&m);
#endif

    buffer_free(&m);

    debug3("%s: done", __func__);
}
#endif /* L7_SSHD */

#ifdef L7_SSHD
void sshd_connection_task(L7_uint32 numArgs, int *argv)
{
    int                 cn = argv[0];
    child_context_t     *context = &child_connect_context[cn];
    int                 sock_in = context->sock_in;
    int                 sock_out = context->sock_out;
    sensitive_data_t    child_sensitive_data = context->child_sensitive_data;

    int                 on = 1;
    const char          *remote_ip = NULL;
    int                 remote_port = -1;
    Authctxt            *authctxt = NULL;

    /* This is the child processing a new connection. */

    osapiSemaTake(sshdExitSema, L7_WAIT_FOREVER);
    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
    sshdGlobal.sshdNumSessions++;
    osapiSemaGive(sshdGlobalSema);

    conn_taskid[cn] = osapiTaskIdSelf();
    context->selfTaskId = osapiTaskIdSelf();
    osapiSemaGive(sshdExitSema);

    /*
     * Register our connection.  This turns encryption off because we do
     * not have a key.
     */

    debugl7(SYSLOG_LEVEL_VERBOSE, "connection %d: task 0x%08x", cn, conn_taskid[cn]);
    packet_set_connection(cn, sock_in, sock_out);
    packet_set_server(cn);

    /* Set SO_KEEPALIVE if requested. */
    if (options.tcp_keep_alive && packet_connection_is_on_socket(cn) &&
        setsockopt(sock_in, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on)) < 0)
        error("setsockopt SO_KEEPALIVE: %.100s", strerror(errno));

    if ((remote_port = get_remote_port(cn)) < 0) {
        debugl7(SYSLOG_LEVEL_ERROR, "get_remote_port failed");
        cleanup_exit_cn(cn, 255);
    }

    /*
     * We use get_canonical_hostname with usedns = 0 instead of
     * get_remote_ipaddr here so IP options will be checked.
     */
    remote_ip = get_canonical_hostname(cn, 0);

    /* Log the connection. */
    debugl7(SYSLOG_LEVEL_VERBOSE, "connection %d: Connection from %.500s port %d", cn, remote_ip, remote_port);

#ifdef COMMENTED_OUT
    if (remote_ip != NULL)
    {
        xfree(remote_ip);
        remote_ip = NULL;
    }
#endif /* COMMENTED_OUT */

    /*
     * We don't want to listen forever unless the other side
     * successfully authenticates itself.  So we set up an alarm which is
     * cleared after successful authentication.  A limit of zero
     * indicates no limit. Note that we don't set the alarm in debugging
     * mode; it is just annoying to have the server exit just when you
     * are about to discover the bug.
     */
    debugl7(SYSLOG_LEVEL_TIMER, "connection %d: Added timer for grace_alarm_handler, %d seconds",
            cn, options.login_grace_time);
    osapiTimerAdd((void*)grace_alarm_handler, cn, (int) context,
                  options.login_grace_time * 1000, &context->pLoginTimerHolder);

    debugl7(SYSLOG_LEVEL_INFO, "connection %d: Exchanging identification for socks %d and %d", cn, sock_in, sock_out);
    sshd_exchange_identification(cn, sock_in, sock_out);

    debugl7(SYSLOG_LEVEL_INFO, "connection %d: Setting connection to non-blocking ...", cn);
    packet_set_nonblocking(cn);

    /* allocate authentication context */
    authctxt = xmalloc(sizeof(*authctxt));
    memset(authctxt, 0, sizeof(*authctxt));

    authctxt->loginmsg = &loginmsg[cn];
    authctxt->cn = cn;

    /* XXX global for cleanup, access from other modules */
    the_authctxt[cn] = authctxt;

    /* prepare buffer to collect messages to display to user after login */
    buffer_init(&loginmsg[cn]);

    /* perform the key exchange */
    /* authenticate user and start session */
    if (compat20[cn]) {
        debugl7(SYSLOG_LEVEL_INFO, "connection %d: do ssh2 key exchange ...", cn);
        do_ssh2_kex(cn);
        debugl7(SYSLOG_LEVEL_INFO, "connection %d: do ssh2 authentication ...", cn);
        do_authentication2(authctxt);
    } else {
        debugl7(SYSLOG_LEVEL_INFO, "connection %d: do ssh1 key exchange ...", cn);
        do_ssh1_kex(cn, child_sensitive_data);
        debugl7(SYSLOG_LEVEL_INFO, "connection %d: do ssh1 authentication ...", cn);
        do_authentication(authctxt);
    }

    debugl7(SYSLOG_LEVEL_INFO, "connection %d: authentication complete...", cn);

    /*
    Cancel the alarm we set to limit the time taken for
    authentication.
    */
    if (context->pLoginTimerHolder != NULL)
    {
        debugl7(SYSLOG_LEVEL_TIMER, "Freeing timer for grace_alarm_handler");
        osapiTimerFree(context->pLoginTimerHolder);
        context->pLoginTimerHolder = NULL;
    }

    /* Start session. */
    debugl7(SYSLOG_LEVEL_INFO, "connection %d: start session ...", cn);
    do_authenticated(authctxt);

    /* The connection has been terminated. */
    debugl7(SYSLOG_LEVEL_VERBOSE, "connection %d: Closing", cn);


    cleanup_exit_cn(cn, 0);

    return;
}
#endif /* L7_SSHD */

/*
 * Main program for the daemon.
 */
int
#ifndef L7_SSHD
main(int ac, char **av)
#else
sshd_main(int ac, char **av)
#endif /* L7_SSHD */
{
#ifndef L7_SSHD
    extern char *optarg;
    extern int optind;
    int opt, j, i, fdsetsz, on = 1;
    int sock_in = -1, sock_out = -1, newsock = -1;
#else
    int i, on = 1;
    unsigned int j;
    int newsock = -1;
    char taskName[9];
    int sshd_taskid = osapiTaskIdSelf();
#endif /* L7_SSHD */
#ifndef L7_SSHD
    pid_t pid;
#else
    pid_t pid = -1;
#endif /* L7_SSHD */
    socklen_t fromlen;
#ifndef L7_SSHD
    fd_set *fdset;
#else
    fd_set fdset;
#endif /* L7_SSHD */
    struct sockaddr_storage from;
#ifndef L7_SSHD
    const char *remote_ip;
    int remote_port;
    FILE *f;
#else
    int f;
    L7_RC_t rc;
    char printbuf[128];
#endif /* L7_SSHD */
    struct addrinfo *ai;
    char ntop[NI_MAXHOST], strport[NI_MAXSERV];
#ifndef L7_SSHD
    char *line;
#endif /* L7_SSHD */
    int listen_sock, maxfd;
#ifndef L7_SSHD
    int startup_p[2] = { -1 , -1 }, config_s[2] = { -1 , -1 };
#endif /* L7_SSHD */
    int startups = 0;
    Key *key;
#ifndef L7_SSHD
    Authctxt *authctxt;
#endif /* L7_SSHD */
    int ret, key_used = 0;
#ifndef L7_SSHD
    Buffer cfg;
#endif /* L7_SSHD */
#ifdef L7_SSHD
    int found, cn = -1;
    int sshdTaskArgv[L7_OPENSSH_MAX_CONNECTIONS][1];
#endif /* L7_SSHD */

#ifndef L7_SSHD
#ifdef HAVE_SECUREWARE
    (void)set_auth_parameters(ac, av);
#endif
    __progname = ssh_get_progname(av[0]);
    init_rng();

    /* Save argv. Duplicate so setproctitle emulation doesn't clobber it */
    saved_argc = ac;
    rexec_argc = ac;
    saved_argv = xmalloc(sizeof(*saved_argv) * (ac + 1));
    for (i = 0; i < ac; i++)
        saved_argv[i] = xstrdup(av[i]);
    saved_argv[i] = NULL;

#ifndef HAVE_SETPROCTITLE
    /* Prepare for later setproctitle emulation */
    compat_init_setproctitle(ac, av);
    av = saved_argv;
#endif

    if (geteuid() == 0 && setgroups(0, NULL) == -1)
        debug("setgroups(): %.200s", strerror(errno));

    /* Ensure that fds 0, 1 and 2 are open or directed to /dev/null */
    sanitise_stdfd();
#endif /* L7_SSHD */

    /* Initialize configuration options to their default values. */
#ifdef L7_SSHD
    log_init("L7_SSHD", log_level, SYSLOG_FACILITY_NOT_SET, 1);
    debugl7(SYSLOG_LEVEL_INFO, "Initializing server options ...");
#endif /* L7_SSHD */
    ssh_attack_detector = NULL;
    initialize_server_options(&options);

#ifndef L7_SSHD
    /* Parse command-line arguments. */
    while ((opt = getopt(ac, av, "f:p:b:k:h:g:u:o:dDeiqrtQR46")) != -1) {
        switch (opt) {
        case '4':
            options.address_family = AF_INET;
            break;
        case '6':
            options.address_family = AF_INET6;
            break;
        case 'f':
            config_file_name = optarg;
            break;
        case 'd':
            if (debug_flag == 0) {
                debug_flag = 1;
                options.log_level = SYSLOG_LEVEL_DEBUG1;
            } else if (options.log_level < SYSLOG_LEVEL_DEBUG3)
                options.log_level++;
            break;
        case 'D':
            no_daemon_flag = 1;
            break;
        case 'e':
            log_stderr = 1;
            break;
        case 'i':
            inetd_flag = 1;
            break;
        case 'r':
            rexec_flag = 0;
            break;
        case 'R':
            rexeced_flag = 1;
            inetd_flag = 1;
            break;
        case 'Q':
            /* ignored */
            break;
        case 'q':
            options.log_level = SYSLOG_LEVEL_QUIET;
            break;
        case 'b':
            options.server_key_bits = atoi(optarg);
            break;
        case 'p':
            options.ports_from_cmdline = 1;
            if (options.num_ports >= MAX_PORTS) {
                fprintf(stderr, "too many ports.\n");
                exit(1);
            }
            options.ports[options.num_ports++] = a2port(optarg);
            if (options.ports[options.num_ports-1] == 0) {
                fprintf(stderr, "Bad port number.\n");
                exit(1);
            }
            break;
        case 'g':
            if ((options.login_grace_time = convtime(optarg)) == -1) {
                fprintf(stderr, "Invalid login grace time.\n");
                exit(1);
            }
            break;
        case 'k':
            if ((options.key_regeneration_time = convtime(optarg)) == -1) {
                fprintf(stderr, "Invalid key regeneration interval.\n");
                exit(1);
            }
            break;
        case 'h':
            if (options.num_host_key_files >= MAX_HOSTKEYS) {
                fprintf(stderr, "too many host keys.\n");
                exit(1);
            }
            options.host_key_files[options.num_host_key_files++] = optarg;
            break;
        case 't':
            test_flag = 1;
            break;
        case 'u':
            utmp_len = atoi(optarg);
            if (utmp_len > MAXHOSTNAMELEN) {
                fprintf(stderr, "Invalid utmp length.\n");
                exit(1);
            }
            break;
        case 'o':
            line = xstrdup(optarg);
            if (process_server_config_line(&options, line,
                "command-line", 0) != 0)
                exit(1);
            xfree(line);
            break;
        case '?':
        default:
            usage();
            break;
        }
    }
    if (rexeced_flag || inetd_flag)
        rexec_flag = 0;
    if (rexec_flag && (av[0] == NULL || *av[0] != '/'))
        fatal("sshd re-exec requires execution with an absolute path");
    if (rexeced_flag)
        closefrom(REEXEC_MIN_FREE_FD);
    else
        closefrom(REEXEC_DEVCRYPTO_RESERVED_FD);
#endif /* L7_SSHD */

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "SSL algorithms added during OpenSSL component init, not added here");
#else
    SSLeay_add_all_algorithms();
#endif /* L7_SSHD */

#ifndef L7_SSHD
    /*
     * Force logging to stderr until we have loaded the private host
     * key (unless started from inetd)
     */
    log_init(__progname,
        options.log_level == SYSLOG_LEVEL_NOT_SET ?
        SYSLOG_LEVEL_INFO : options.log_level,
        options.log_facility == SYSLOG_FACILITY_NOT_SET ?
        SYSLOG_FACILITY_AUTH : options.log_facility,
        log_stderr || !inetd_flag);

    /*
     * Unset KRB5CCNAME, otherwise the user's session may inherit it from
     * root's environment
     */
    if (getenv("KRB5CCNAME") != NULL)
        unsetenv("KRB5CCNAME");

#ifdef _UNICOS
    /* Cray can define user privs drop all privs now!
     * Not needed on PRIV_SU systems!
     */
    drop_cray_privs();
#endif
#endif /* L7_SSHD */

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "Initializing sensitive data ...");
#endif /* L7_SSHD */
    sensitive_data.server_key = NULL;
    sensitive_data.ssh1_host_key = NULL;
    sensitive_data.have_ssh1_key = 0;
    sensitive_data.have_ssh2_key = 0;

#ifndef L7_SSHD
    /* Fetch our configuration */
    buffer_init(&cfg);
    if (rexeced_flag)
        recv_rexec_state(REEXEC_CONFIG_PASS_FD, &cfg);
    else
        load_server_config(config_file_name, &cfg);

    parse_server_config(&options,
        rexeced_flag ? "rexec" : config_file_name, &cfg);

    if (!rexec_flag)
        buffer_free(&cfg);
#else
    /*
    Get config from SSH configuration
    */
#endif /* L7_SSHD */

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "Seed random number generator ...");
#endif /* L7_SSHD */
    seed_rng();

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "Filling in server options ...");
#endif /* L7_SSHD */
    /* Fill in default values for those options not explicitly set. */
    fill_default_server_options(&options);

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "Setting channel address family %d ...", options.address_family);
#endif /* L7_SSHD */
    /* set default channel AF */
    channel_set_af(options.address_family);

#ifndef L7_SSHD
    /* Check that there are no remaining arguments. */
    if (optind < ac) {
        fprintf(stderr, "Extra argument %s.\n", av[optind]);
        exit(1);
    }
#endif /* L7_SSHD */

    debug("sshd version %.100s", SSH_RELEASE);

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "Loading host keys ...");
#endif /* L7_SSHD */
    /* load private host keys */
    sensitive_data.host_keys = xmalloc(options.num_host_key_files *
        sizeof(Key *));
    for (i = 0; i < options.num_host_key_files; i++)
        sensitive_data.host_keys[i] = NULL;

    for (i = 0; i < options.num_host_key_files; i++) {
        key = key_load_private(options.host_key_files[i], "", NULL);
        sensitive_data.host_keys[i] = key;
        if (key == NULL) {
            error("Could not load host key: %s",
                options.host_key_files[i]);
            sensitive_data.host_keys[i] = NULL;
            continue;
        }
        switch (key->type) {
        case KEY_RSA1:
            sensitive_data.ssh1_host_key = key;
            sensitive_data.have_ssh1_key = 1;
            break;
        case KEY_RSA:
        case KEY_DSA:
            sensitive_data.have_ssh2_key = 1;
            break;
        }
        debug("private host key: #%d type %d %s", i, key->type,
            key_type(key));
    }
    if ((options.protocol & SSH_PROTO_1) && !sensitive_data.have_ssh1_key) {
        logit("Disabling protocol version 1. Could not load host key");
        options.protocol &= ~SSH_PROTO_1;
    }
    if ((options.protocol & SSH_PROTO_2) && !sensitive_data.have_ssh2_key) {
        logit("Disabling protocol version 2. Could not load host key");
        options.protocol &= ~SSH_PROTO_2;
    }
    if (!(options.protocol & (SSH_PROTO_1|SSH_PROTO_2))) {
        logit("sshd: no hostkeys available -- exiting.");
#ifndef L7_SSHD
        exit(1);
#else
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshd: no hostkeys available"
                          " -- exiting ... The SSHD main process could not find the host keys.");
        sshd_exit_flag = 1;
        sshd_exited = 1;
        osapiTaskDelete(sshd_taskid);
#endif /* L7_SSHD */
    }

    /* Check certain values for sanity. */
    if (options.protocol & SSH_PROTO_1) {
        if (options.server_key_bits < 512 ||
            options.server_key_bits > 32768) {
#ifndef L7_SSHD
            fprintf(stderr, "Bad server key size.\n");
            exit(1);
#else
            debugl7(SYSLOG_LEVEL_INFO, "Bad server key size, exiting with error code 1 ...");
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Bad server key size, exiting ..."
                                                               " The SSHD server key size is incorrect.");
            sshd_exit_flag = 1;
            sshd_exited = 1;
            osapiTaskDelete(sshd_taskid);
#endif /* L7_SSHD */
        }
        /*
         * Check that server and host key lengths differ sufficiently. This
         * is necessary to make double encryption work with rsaref. Oh, I
         * hate software patents. I dont know if this can go? Niels
         */
        if (options.server_key_bits >
            BN_num_bits(sensitive_data.ssh1_host_key->rsa->n) -
            SSH_KEY_BITS_RESERVED && options.server_key_bits <
            BN_num_bits(sensitive_data.ssh1_host_key->rsa->n) +
            SSH_KEY_BITS_RESERVED) {
            options.server_key_bits =
                BN_num_bits(sensitive_data.ssh1_host_key->rsa->n) +
                SSH_KEY_BITS_RESERVED;
            debug("Forcing server key to %d bits to make it differ from host key.",
                options.server_key_bits);
        }
    }

#ifndef L7_SSHD
    if (use_privsep) {
        struct passwd *pw;
        struct stat st;

        if ((pw = getpwnam(SSH_PRIVSEP_USER)) == NULL)
            fatal("Privilege separation user %s does not exist",
                SSH_PRIVSEP_USER);
        if ((stat(_PATH_PRIVSEP_CHROOT_DIR, &st) == -1) ||
            (S_ISDIR(st.st_mode) == 0))
            fatal("Missing privilege separation directory: %s",
                _PATH_PRIVSEP_CHROOT_DIR);

#ifdef HAVE_CYGWIN
        if (check_ntsec(_PATH_PRIVSEP_CHROOT_DIR) &&
            (st.st_uid != getuid () ||
            (st.st_mode & (S_IWGRP|S_IWOTH)) != 0))
#else
        if (st.st_uid != 0 || (st.st_mode & (S_IWGRP|S_IWOTH)) != 0)
#endif
            fatal("%s must be owned by root and not group or "
                "world-writable.", _PATH_PRIVSEP_CHROOT_DIR);
    }

    /* Configuration looks good, so exit if in test mode. */
    if (test_flag)
        exit(0);

    /*
     * Clear out any supplemental groups we may have inherited.  This
     * prevents inadvertent creation of files with bad modes (in the
     * portable version at least, it's certainly possible for PAM
     * to create a file, and we can't control the code in every
     * module which might be used).
     */
    if (setgroups(0, NULL) < 0)
        debug("setgroups() failed: %.200s", strerror(errno));

    if (rexec_flag) {
        rexec_argv = xmalloc(sizeof(char *) * (rexec_argc + 2));
        for (i = 0; i < rexec_argc; i++) {
            debug("rexec_argv[%d]='%s'", i, saved_argv[i]);
            rexec_argv[i] = saved_argv[i];
        }
        rexec_argv[rexec_argc] = "-R";
        rexec_argv[rexec_argc + 1] = NULL;
    }

    /* Initialize the log (it is reinitialized below in case we forked). */
    if (debug_flag && (!inetd_flag || rexeced_flag))
        log_stderr = 1;
    log_init(__progname, options.log_level, options.log_facility, log_stderr);

    /*
     * If not in debugging mode, and not started from inetd, disconnect
     * from the controlling terminal, and fork.  The original process
     * exits.
     */
    if (!(debug_flag || inetd_flag || no_daemon_flag)) {
#ifdef TIOCNOTTY
        int fd;
#endif /* TIOCNOTTY */
        if (daemon(0, 0) < 0)
            fatal("daemon() failed: %.200s", strerror(errno));

        /* Disconnect from the controlling tty. */
#ifdef TIOCNOTTY
        fd = open(_PATH_TTY, O_RDWR | O_NOCTTY);
        if (fd >= 0) {
            (void) ioctl(fd, TIOCNOTTY, NULL);
            close(fd);
        }
#endif /* TIOCNOTTY */
    }
    /* Reinitialize the log (because of the fork above). */
    log_init(__progname, options.log_level, options.log_facility, log_stderr);
#endif /* L7_SSHD */

#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "Initializing random number generator ...");
#endif /* L7_SSHD */
    /* Initialize the random number generator. */
    arc4random_stir();

#ifndef L7_SSHD
    /* Chdir to the root directory so that the current disk can be
       unmounted if desired. */
    chdir("/");
#endif /* L7_SSHD */

    /* ignore SIGPIPE */
    signal(SIGPIPE, SIG_IGN);

    /* Start listening for a socket, unless started from inetd. */
    if (inetd_flag) {
#ifndef L7_SSHD
        int fd;

        startup_pipe = -1;
        if (rexeced_flag) {
            close(REEXEC_CONFIG_PASS_FD);
            sock_in = sock_out = dup(STDIN_FILENO);
            if (!debug_flag) {
                startup_pipe = dup(REEXEC_STARTUP_PIPE_FD);
                close(REEXEC_STARTUP_PIPE_FD);
            }
        } else {
            sock_in = dup(STDIN_FILENO);
            sock_out = dup(STDOUT_FILENO);
        }
        /*
         * We intentionally do not close the descriptors 0, 1, and 2
         * as our code for setting the descriptors won't work if
         * ttyfd happens to be one of those.
         */
        if ((fd = open(_PATH_DEVNULL, O_RDWR, 0)) != -1) {
            dup2(fd, STDIN_FILENO);
            dup2(fd, STDOUT_FILENO);
            if (fd > STDOUT_FILENO)
                close(fd);
        }
        debug("inetd sockets after dupping: %d, %d", sock_in, sock_out);
        if ((options.protocol & SSH_PROTO_1) &&
            sensitive_data.server_key == NULL)
            generate_ephemeral_server_key();
#endif /* L7_SSHD */
    } else {
        for (ai = options.listen_addrs; ai; ai = ai->ai_next) {
            if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
                continue;
            if (num_listen_socks >= MAX_LISTEN_SOCKS)
                fatal("Too many listen sockets. "
                    "Enlarge MAX_LISTEN_SOCKS");
            if ((ret = getnameinfo(ai->ai_addr, ai->ai_addrlen,
                ntop, sizeof(ntop), strport, sizeof(strport),
                NI_NUMERICHOST|NI_NUMERICSERV)) != 0) {
                error("getnameinfo failed: %.100s",
                    (ret != EAI_SYSTEM) ? gai_strerror(ret) :
                    strerror(errno));
                continue;
            }
            /* Create socket for listening. */
            listen_sock = socket(ai->ai_family, ai->ai_socktype,
                ai->ai_protocol);
            if (listen_sock < 0) {
                /* kernel may not support ipv6 */
                verbose("socket: %.100s", strerror(errno));
                continue;
            }
#ifdef L7_SSHD
            debugl7(SYSLOG_LEVEL_OPEN, "socket %d created, family %d", listen_sock, ai->ai_family);
#endif /* L7_SSHD */
            if (set_nonblock(listen_sock) == -1) {
#ifdef L7_SSHD
                debugl7(SYSLOG_LEVEL_OPEN, "set_nonblock of listen_sock %d failed, closing", listen_sock);
#endif /* L7_SSHD */
                close(listen_sock);
                continue;
            }
            /*
             * Set socket options.
             * Allow local port reuse in TIME_WAIT.
             */
            if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
                (char*)&on, sizeof(on)) == -1)
                error("setsockopt SO_REUSEADDR: %s", strerror(errno));

#ifdef IPV6_V6ONLY
            if (ai->ai_family == AF_INET6)
            {
                if (setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY,
                    (char*)&on, sizeof(on)) == -1)
                    error("setsockopt IPV6_V6ONLY: %s", strerror(errno));
            }
#endif /* IPV6_V6ONLY */

            debug("Bind to port %s on %s.", strport, ntop);

            /* Bind the socket to the desired port. */
            if (bind(listen_sock, ai->ai_addr, ai->ai_addrlen) < 0) {
                if (!ai->ai_next)
                    error("Bind to port %s on %s failed: %.200s.",
                        strport, ntop, strerror(errno));
#ifdef L7_SSHD
                debugl7(SYSLOG_LEVEL_OPEN, "bind of listen_sock %d failed, closing", listen_sock);
#endif /* L7_SSHD */
                close(listen_sock);
                continue;
            }
            listen_socks[num_listen_socks] = listen_sock;
#ifdef L7_SSHD
            debugl7(SYSLOG_LEVEL_OPEN, "listen_sock[%d] %d bound, adding to array", num_listen_socks, listen_sock);
#endif /* L7_SSHD */
            num_listen_socks++;

            /* Start listening on the port. */
#ifndef L7_SSHD
            logit("Server listening on %s port %s.", ntop, strport);
#else
            logit("Call listen on %s port %s.", ntop, strport);
#endif /* L7_SSHD */
            if (listen(listen_sock, SSH_LISTEN_BACKLOG) < 0)
#ifndef L7_SSHD
                fatal("listen: %.100s", strerror(errno));
#else
            {
                close_listen_socks();
                fatal("listen: %.100s", strerror(errno));
            }
            logit("Server listening on %s port %s.", ntop, strport);
#endif /* L7_SSHD */

        }
        freeaddrinfo(options.listen_addrs);

        if (!num_listen_socks)
#ifndef L7_SSHD
            fatal("Cannot bind any address.");
#else
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "SSHD: Cannot bind any address. The "
                                 "SSHD main process could not bind the address used to accept connections.");
            fatal("Cannot bind any address.");
        }
#endif /* L7_SSHD */

#ifndef L7_SSHD
        if (options.protocol & SSH_PROTO_1)
            generate_ephemeral_server_key();
#else
        if (options.protocol & SSH_PROTO_1)
        {
            debugl7(SYSLOG_LEVEL_INFO, "Generating SSH1 ephemeral server key");

            generate_ephemeral_server_key();
        }
#endif /* L7_SSHD */

#ifndef L7_SSHD
        /*
         * Arrange to restart on SIGHUP.  The handler needs
         * listen_sock.
         */
        signal(SIGHUP, sighup_handler);

        signal(SIGTERM, sigterm_handler);
        signal(SIGQUIT, sigterm_handler);

        /* Arrange SIGCHLD to be caught. */
        signal(SIGCHLD, main_sigchld_handler);
#endif /* L7_SSHD */

        /* Write out the pid file after the sigterm handler is setup */
        if (!debug_flag) {
            /*
             * Record our pid in /var/run/sshd.pid to make it
             * easier to kill the correct sshd.  We don't want to
             * do this before the bind above because the bind will
             * fail if there already is a daemon, and this will
             * overwrite any old pid in the file.
             */
#ifndef L7_SSHD
            f = fopen(options.pid_file, "wb");
            if (f == NULL) {
                error("Couldn't create pid file \"%s\": %s",
                    options.pid_file, strerror(errno));
            } else {
                fprintf(f, "%ld\n", (long) getpid());
                fclose(f);
            }
#else
            rc = osapiFsFileCreate(options.pid_file, &f);
            if (rc != L7_SUCCESS) {
                error("Couldn't create pid file \"%s\": %s",
                    options.pid_file, strerror(errno));
            } else {
                osapiSnprintf(printbuf, sizeof(printbuf), "%ld\n", (long) sshd_taskid);
                osapiFsWriteNoClose(f, printbuf, strlen(printbuf));
                osapiFsClose(f);
            }
#endif /* L7_SSHD */
        }

        /* setup fd set for listen */
#ifndef L7_SSHD
        fdset = NULL;
#endif /* L7_SSHD */
        maxfd = 0;
        for (i = 0; i < num_listen_socks; i++)
            if (listen_socks[i] > maxfd)
                maxfd = listen_socks[i];
#ifdef L7_SSHD
            debugl7(SYSLOG_LEVEL_OPEN, "num_listen_socks %d, maxfd = %d", num_listen_socks, maxfd);
#endif /* L7_SSHD */
#ifndef L7_SSHD
        /* pipes connected to unauthenticated childs */
        startup_pipes = xmalloc(options.max_startups * sizeof(int));
        for (i = 0; i < options.max_startups; i++)
            startup_pipes[i] = -1;
#endif /* L7_SSHD */

        /*
         * Stay listening for connections until the system crashes or
         * the daemon is killed with a signal.
         */
#ifndef L7_SSHD
        for (;;) {
            if (received_sighup)
                sighup_restart();
            if (fdset != NULL)
                xfree(fdset);
            fdsetsz = howmany(maxfd+1, NFDBITS) * sizeof(fd_mask);
            fdset = (fd_set *)xmalloc(fdsetsz);
            memset(fdset, 0, fdsetsz);

            for (i = 0; i < num_listen_socks; i++)
                FD_SET(listen_socks[i], fdset);
            for (i = 0; i < options.max_startups; i++)
                if (startup_pipes[i] != -1)
                    FD_SET(startup_pipes[i], fdset);

                /* Wait in select until there is a connection. */
                ret = select(maxfd+1, fdset, NULL, NULL, NULL);
#else
        for (;sshd_exit_flag != 1;)
        {
            struct timeval tv;

            memset(&fdset, 0, sizeof(fd_set));
            memset(&tv, 0, sizeof(struct timeval));
            tv.tv_sec = 1;

            for (i = 0; i < num_listen_socks; i++)
            {
                debugl7(0x01000000, "adding listen_socks[%d], %d to fdset", i, listen_socks[i]);
                FD_SET(listen_socks[i], &fdset);
            }

            /* Wait in select until there is a connection. */
            ret = select(maxfd+1, &fdset, NULL, NULL, &tv);
            if (ret == 0)
            {
                debugl7(0x01000000, "select timed out, sshd_exit_flag %d", sshd_exit_flag);
                continue;
            }
#endif /* L7_SSHD */
            if (ret < 0 && errno != EINTR)
                error("select: %.100s", strerror(errno));
            if (received_sigterm) {
                logit("Received signal %d; terminating.",
                    (int) received_sigterm);
                close_listen_socks();
                unlink(options.pid_file);
#ifndef L7_SSHD
                exit(255);
#else
                debugl7(SYSLOG_LEVEL_INFO, "Received sigterm, exiting with error code 255 ...");
                sshd_exit_flag = 1;
                sshd_exited = 1;
                osapiTaskDelete(sshd_taskid);
#endif /* L7_SSHD */
            }
            if (key_used && key_do_regen) {
                generate_ephemeral_server_key();
                key_used = 0;
                key_do_regen = 0;
            }
            if (ret < 0)
                continue;

#ifndef L7_SSHD
            for (i = 0; i < options.max_startups; i++)
                if (startup_pipes[i] != -1 &&
                    FD_ISSET(startup_pipes[i], fdset)) {
                    /*
                     * the read end of the pipe is ready
                     * if the child has closed the pipe
                     * after successful authentication
                     * or if the child has died
                     */
                    close(startup_pipes[i]);
                    startup_pipes[i] = -1;
                    startups--;
                }
#endif /* L7_SSHD */
            for (i = 0; i < num_listen_socks; i++) {
#ifndef L7_SSHD
                if (!FD_ISSET(listen_socks[i], fdset))
                    continue;
#else
                if (!FD_ISSET(listen_socks[i], &fdset))
                    continue;
#endif /* L7_SSHD */
                fromlen = sizeof(from);
                newsock = accept(listen_socks[i], (struct sockaddr *)&from,
#ifndef L7_SSHD
                    &fromlen);
#else
                    (int *) &fromlen);
#endif /* L7_SSHD */
                if (newsock < 0) {
                    if (errno != EINTR && errno != EWOULDBLOCK)
                        error("accept: %.100s", strerror(errno));
                    continue;
                }
#ifdef L7_SSHD
                debugl7(SYSLOG_LEVEL_OPEN, "accept on listen_socks[%d] %d, newsock %d",
                        i, listen_socks[i], newsock);
#endif /* L7_SSHD */
                if (unset_nonblock(newsock) == -1) {
#ifdef L7_SSHD
                debugl7(SYSLOG_LEVEL_OPEN, "unset_nonblock on newsock %d failed, closing",
                        newsock);
#endif /* L7_SSHD */
                    close(newsock);
                    continue;
                }
                if (drop_connection(startups) == 1) {
                    debug("drop connection #%d", startups);
#ifndef L7_SSHD
                    debug("drop connection #%d", startups);
#else
                    debugl7(SYSLOG_LEVEL_OPEN, "drop connect #%d, closing newsock %d",
                            startups, newsock);
#endif /* L7_SSHD */
                    close(newsock);
                    continue;
                }
#ifndef L7_SSHD
                if (pipe(startup_p) == -1) {
                    close(newsock);
                    continue;
                }

                if (rexec_flag && socketpair(AF_UNIX,
                    SOCK_STREAM, 0, config_s) == -1) {
                    error("reexec socketpair: %s",
                        strerror(errno));
                    close(newsock);
                    close(startup_p[0]);
                    close(startup_p[1]);
                    continue;
                }

                for (j = 0; j < options.max_startups; j++)
                    if (startup_pipes[j] == -1) {
                        startup_pipes[j] = startup_p[0];
                        if (maxfd < startup_p[0])
                            maxfd = startup_p[0];
                        startups++;
                        break;
                    }

                /*
                 * Got connection.  Fork a child to handle it, unless
                 * we are in debugging mode.
                 */
                if (debug_flag) {
                    /*
                     * In debugging mode.  Close the listening
                     * socket, and start processing the
                     * connection without forking.
                     */
                    debug("Server will not fork when running in debugging mode.");
                    close_listen_socks();
                    sock_in = newsock;
                    sock_out = newsock;
                    close(startup_p[0]);
                    close(startup_p[1]);
                    startup_pipe = -1;
                    pid = getpid();
                    if (rexec_flag) {
                        send_rexec_state(config_s[0],
                            &cfg);
                        close(config_s[0]);
                    }
                    break;
                } else {
                    /*
                     * Normal production daemon.  Fork, and have
                     * the child process the connection. The
                     * parent continues listening.
                     */
                    if ((pid = fork()) == 0) {
                        /*
                         * Child.  Close the listening and max_startup
                         * sockets.  Start using the accepted socket.
                         * Reinitialize logging (since our pid has
                         * changed).  We break out of the loop to handle
                         * the connection.
                         */
                        startup_pipe = startup_p[1];
                        close_startup_pipes();
                        close_listen_socks();
                        sock_in = newsock;
                        sock_out = newsock;
                        log_init(__progname, options.log_level, options.log_facility, log_stderr);
                        if (rexec_flag)
                            close(config_s[0]);
                        break;
                    }
                }

                /* Parent.  Stay in the loop. */
                if (pid < 0)
                    error("fork: %.100s", strerror(errno));
                else
                    debug("Forked child %ld.", (long)pid);

                close(startup_p[1]);

#else
                found = -1;
                osapiSemaTake(sshdExitSema, L7_WAIT_FOREVER);
                osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
                for (j=0; j<sshdGlobal.sshdMaxSessions; j++)
                {
                    if (connection_in[j] == -1)
                    {
                        found = 1;
                        cn = j;
                        child_connect_context[j].cn = j;
                        connection_in[j] = 0;
                        break;
                    }
                }
                osapiSemaGive(sshdGlobalSema);
                osapiSemaGive(sshdExitSema);

                if (found == -1)
                {
                    debugl7(SYSLOG_LEVEL_INFO, "Unable to find free connection for child task\n");
                    close(newsock);
                    continue;
                }

                memset(&client_peers[cn], 0, sizeof(struct sockaddr_storage));
                client_peers[cn] = from;

                child_connect_context[cn].sock_in = newsock;
                child_connect_context[cn].sock_out = newsock;
                child_connect_context[cn].child_sensitive_data = sensitive_data;

                if ((sensitive_data.server_key != NULL) && (options.protocol & SSH_PROTO_1))
                {
                    /*
                    Key has been given to the child
                    */
                    debugl7(SYSLOG_LEVEL_INFO, "Gave server key to child\n");
                    sensitive_data.server_key = NULL;
                }

                osapiSnprintf(taskName, sizeof(taskName), "sshd[%d]", cn);
                sshdTaskArgv[cn][0] = cn;
                pid = (int) osapiTaskCreate(taskName, sshd_connection_task,
                                       1, &sshdTaskArgv[cn],
                                       L7_DEFAULT_STACK_SIZE*4,
                                       L7_DEFAULT_TASK_PRIORITY,
                                       L7_DEFAULT_TASK_SLICE);

                if (pid == L7_ERROR)
                {
                    debugl7(SYSLOG_LEVEL_OPEN, "osapiTaskCreate failed, closing newsock %d and cleaning up", newsock);
                    close(newsock);
                    if ((child_connect_context[cn].child_sensitive_data.server_key != NULL) &&
                        (options.protocol & SSH_PROTO_1))
                    {
                        debugl7(SYSLOG_LEVEL_INFO, "Returned server key to main thread\n");
                        sensitive_data.server_key = child_connect_context[cn].child_sensitive_data.server_key;
                    }
                    connection_in[cn] = -1;
                    memset(&client_peers[cn], 0, sizeof(struct sockaddr_storage));
                    memset(&child_connect_context[cn], 0, sizeof (child_context_t));
                    continue;
                }

                debugl7(SYSLOG_LEVEL_INFO, "Created child task %s, 0x%08lx", taskName, (long)pid);
#endif /* L7_SSHD */
#ifndef L7_SSHD
                if (rexec_flag) {
                    send_rexec_state(config_s[0], &cfg);
                    close(config_s[0]);
                    close(config_s[1]);
                }
#endif /* L7_SSHD */

                /* Mark that the key has been used (it was "given" to the child). */
                if ((options.protocol & SSH_PROTO_1) &&
                    key_used == 0) {
                    /* Schedule server key regeneration alarm. */
#ifdef L7_SSHD
#ifdef COMMENTED_OUT
                    debugl7(SYSLOG_LEVEL_INFO, "rescheduling the key generation");
                    debugl7(SYSLOG_LEVEL_TIMER, "Added timer for key_regeneration_alarm %d seconds", options.key_regeneration_time);
                    osapiTimerAdd((void*)key_regeneration_alarm, L7_NULL, L7_NULL,
                                  options.key_regeneration_time * 1000, &pKeyRegenTimerHolder);
#endif /* COMMENTED_OUT */
                    debugl7(SYSLOG_LEVEL_INFO, "rescheduling the key generation");
                    key_do_regen = 1;
#else
                    signal(SIGALRM, key_regeneration_alarm);
                    alarm(options.key_regeneration_time);
#endif /* L7_SSHD */
                    key_used = 1;
                }

#ifdef L7_SSHD
                debugl7(SYSLOG_LEVEL_INFO, "Re-initializing random number generator ...");
#endif /* L7_SSHD */
                arc4random_stir();

#ifndef L7_SSHD
                /* Close the new socket (the child is now taking care of it). */
                close(newsock);
#endif /* L7_SSHD */
            }
            /* child process check (or debug mode) */
            if (num_listen_socks <= 0)
#ifndef L7_SSHD
                break;
#else
            {
                debugl7(SYSLOG_LEVEL_INFO, "no listen sockets, exiting ...");
                break;
            }
#endif /* L7_SSHD */
        }
#ifndef L7_SSHD
        if (fdset != NULL)
            xfree(fdset);
#endif /* L7_SSHD */
    }
#ifdef L7_SSHD
    debugl7(SYSLOG_LEVEL_INFO, "sshd_main: loop finished, exiting ...");
    destroy_sensitive_data();
    debugl7(SYSLOG_LEVEL_INFO, "sshd_main: destroying host_keys pointers 0x%p", sensitive_data.host_keys);
    if (sensitive_data.host_keys != NULL)
    {
        xfree(sensitive_data.host_keys);
        sensitive_data.host_keys = NULL;
    }
    debugl7(SYSLOG_LEVEL_INFO, "sshd_main: destroying ssh_attack_detector 0x%08x", ssh_attack_detector);
    if (ssh_attack_detector != NULL)
    {
        xfree(ssh_attack_detector);
        ssh_attack_detector = NULL;
    }
    sshd_exited = 1;
    debugl7(SYSLOG_LEVEL_INFO, "sshd_main: exited = %d", sshd_exited);
    close_listen_socks();
#endif /* L7_SSHD */

#ifndef L7_SSHD
    /* This is the child processing a new connection. */
    setproctitle("%s", "[accepted]");

    /*
     * Create a new session and process group since the 4.4BSD
     * setlogin() affects the entire process group.  We don't
     * want the child to be able to affect the parent.
     */
#if !defined(SSHD_ACQUIRES_CTTY)
    /*
     * If setsid is called, on some platforms sshd will later acquire a
     * controlling terminal which will result in "could not set
     * controlling tty" errors.
     */
    if (!debug_flag && !inetd_flag && setsid() < 0)
        error("setsid: %.100s", strerror(errno));
#endif

    if (rexec_flag) {
        int fd;

        debug("rexec start in %d out %d newsock %d pipe %d sock %d",
            sock_in, sock_out, newsock, startup_pipe, config_s[0]);
        dup2(newsock, STDIN_FILENO);
        dup2(STDIN_FILENO, STDOUT_FILENO);
        if (startup_pipe == -1)
            close(REEXEC_STARTUP_PIPE_FD);
        else
            dup2(startup_pipe, REEXEC_STARTUP_PIPE_FD);

        dup2(config_s[1], REEXEC_CONFIG_PASS_FD);
        close(config_s[1]);
        if (startup_pipe != -1)
            close(startup_pipe);

        execv(rexec_argv[0], rexec_argv);

        /* Reexec has failed, fall back and continue */
        error("rexec of %s failed: %s", rexec_argv[0], strerror(errno));
        recv_rexec_state(REEXEC_CONFIG_PASS_FD, NULL);
        log_init(__progname, options.log_level,
            options.log_facility, log_stderr);

        /* Clean up fds */
        startup_pipe = REEXEC_STARTUP_PIPE_FD;
        close(config_s[1]);
        close(REEXEC_CONFIG_PASS_FD);
        newsock = sock_out = sock_in = dup(STDIN_FILENO);
        if ((fd = open(_PATH_DEVNULL, O_RDWR, 0)) != -1) {
            dup2(fd, STDIN_FILENO);
            dup2(fd, STDOUT_FILENO);
            if (fd > STDERR_FILENO)
                close(fd);
        }
        debug("rexec cleanup in %d out %d newsock %d pipe %d sock %d",
            sock_in, sock_out, newsock, startup_pipe, config_s[0]);
    }

    /*
     * Disable the key regeneration alarm.  We will not regenerate the
     * key since we are no longer in a position to give it to anyone. We
     * will not restart on SIGHUP since it no longer makes sense.
     */
    alarm(0);
    signal(SIGALRM, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    signal(SIGINT, SIG_DFL);

    /*
     * Register our connection.  This turns encryption off because we do
     * not have a key.
     */
    packet_set_connection(sock_in, sock_out);
    packet_set_server();

    /* Set SO_KEEPALIVE if requested. */
    if (options.tcp_keep_alive && packet_connection_is_on_socket() &&
        setsockopt(sock_in, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) < 0)
        error("setsockopt SO_KEEPALIVE: %.100s", strerror(errno));

    if ((remote_port = get_remote_port()) < 0) {
        debug("get_remote_port failed");
        cleanup_exit(255);
    }

    /*
     * We use get_canonical_hostname with usedns = 0 instead of
     * get_remote_ipaddr here so IP options will be checked.
     */
    remote_ip = get_canonical_hostname(0);

#ifdef SSH_AUDIT_EVENTS
    audit_connection_from(remote_ip, remote_port);
#endif
#ifdef LIBWRAP
    /* Check whether logins are denied from this host. */
    if (packet_connection_is_on_socket()) {
        struct request_info req;

        request_init(&req, RQ_DAEMON, __progname, RQ_FILE, sock_in, 0);
        fromhost(&req);

        if (!hosts_access(&req)) {
            debug("Connection refused by tcp wrapper");
            refuse(&req);
            /* NOTREACHED */
            fatal("libwrap refuse returns");
        }
    }
#endif /* LIBWRAP */

    /* Log the connection. */
    verbose("Connection from %.500s port %d", remote_ip, remote_port);

    /*
     * We don't want to listen forever unless the other side
     * successfully authenticates itself.  So we set up an alarm which is
     * cleared after successful authentication.  A limit of zero
     * indicates no limit. Note that we don't set the alarm in debugging
     * mode; it is just annoying to have the server exit just when you
     * are about to discover the bug.
     */
    signal(SIGALRM, grace_alarm_handler);
    if (!debug_flag)
        alarm(options.login_grace_time);

    sshd_exchange_identification(sock_in, sock_out);

    packet_set_nonblocking();

    /* allocate authentication context */
    authctxt = xmalloc(sizeof(*authctxt));
    memset(authctxt, 0, sizeof(*authctxt));

    authctxt->loginmsg = &loginmsg;

    /* XXX global for cleanup, access from other modules */
    the_authctxt = authctxt;

    /* prepare buffer to collect messages to display to user after login */
    buffer_init(&loginmsg);

    if (use_privsep)
        if (privsep_preauth(authctxt) == 1)
            goto authenticated;

    /* perform the key exchange */
    /* authenticate user and start session */
    if (compat20) {
        do_ssh2_kex();
        do_authentication2(authctxt);
    } else {
        do_ssh1_kex();
        do_authentication(authctxt);
    }
    /*
     * If we use privilege separation, the unprivileged child transfers
     * the current keystate and exits
     */
    if (use_privsep) {
        mm_send_keystate(pmonitor);
        exit(0);
    }

 authenticated:
    /*
     * Cancel the alarm we set to limit the time taken for
     * authentication.
     */
    alarm(0);
    signal(SIGALRM, SIG_DFL);
    if (startup_pipe != -1) {
        close(startup_pipe);
        startup_pipe = -1;
    }

#ifdef SSH_AUDIT_EVENTS
    audit_event(SSH_AUTH_SUCCESS);
#endif

    /*
     * In privilege separation, we fork another child and prepare
     * file descriptor passing.
     */
    if (use_privsep) {
        privsep_postauth(authctxt);
        /* the monitor process [priv] will not return */
        if (!compat20)
            destroy_sensitive_data();
    }

    /* Start session. */
    do_authenticated(authctxt);

    /* The connection has been terminated. */
    verbose("Closing connection to %.100s", remote_ip);

#ifdef USE_PAM
    if (options.use_pam)
        finish_pam();
#endif /* USE_PAM */

#ifdef SSH_AUDIT_EVENTS
    PRIVSEP(audit_event(SSH_CONNECTION_CLOSE));
#endif

    packet_close();

    if (use_privsep)
        mm_terminate();

    exit(0);
#else
    return (0);
#endif /* L7_SSHD */
}

/*
 * Decrypt session_key_int using our private server key and private host key
 * (key with larger modulus first).
 */
#ifndef L7_SSHD
int
ssh1_session_key(BIGNUM *session_key_int)
#else
int
ssh1_session_key(int cn, BIGNUM *session_key_int)
#endif /* L7_SSHD */
{
    int rsafail = 0;

#ifndef L7_SSHD
    if (BN_cmp(sensitive_data.server_key->rsa->n, child_connect_context[cn]->child_sensitive_data.ssh1_host_key->rsa->n) > 0) {
        /* Server key has bigger modulus. */
        if (BN_num_bits(sensitive_data.server_key->rsa->n) <
            BN_num_bits(sensitive_data.ssh1_host_key->rsa->n) + SSH_KEY_BITS_RESERVED) {
            fatal("do_connection: %s: server_key %d < host_key %d + SSH_KEY_BITS_RESERVED %d",
                get_remote_ipaddr(),
                  BN_num_bits(sensitive_data.server_key->rsa->n),
                  BN_num_bits(sensitive_data.ssh1_host_key->rsa->n),
                  SSH_KEY_BITS_RESERVED);
        }
        if (rsa_private_decrypt(session_key_int, session_key_int,
            sensitive_data.server_key->rsa) <= 0)
            rsafail++;
        if (rsa_private_decrypt(session_key_int, session_key_int,
            sensitive_data.ssh1_host_key->rsa) <= 0)
            rsafail++;
    } else {
        /* Host key has bigger modulus (or they are equal). */
        if (BN_num_bits(sensitive_data.ssh1_host_key->rsa->n) <
            BN_num_bits(sensitive_data.server_key->rsa->n) + SSH_KEY_BITS_RESERVED) {
            fatal("do_connection: %s: host_key %d < server_key %d + SSH_KEY_BITS_RESERVED %d",
                get_remote_ipaddr(),
                  BN_num_bits(sensitive_data.ssh1_host_key->rsa->n),
                  BN_num_bits(sensitive_data.server_key->rsa->n),
                  SSH_KEY_BITS_RESERVED);
        }
        if (rsa_private_decrypt(session_key_int, session_key_int,
            sensitive_data.ssh1_host_key->rsa) < 0)
            rsafail++;
        if (rsa_private_decrypt(session_key_int, session_key_int,
            sensitive_data.server_key->rsa) < 0)
            rsafail++;
    }
#else
    if (BN_cmp(child_connect_context[cn].child_sensitive_data.server_key->rsa->n,
               child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa->n) > 0) {
        /* Server key has bigger modulus. */
        if (BN_num_bits(child_connect_context[cn].child_sensitive_data.server_key->rsa->n) <
            BN_num_bits(child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa->n) + SSH_KEY_BITS_RESERVED) {
            debugl7(SYSLOG_LEVEL_FATAL, "do_connection: %s: server_key %d < host_key %d + SSH_KEY_BITS_RESERVED %d",
                get_remote_ipaddr(cn),
                    BN_num_bits(child_connect_context[cn].child_sensitive_data.server_key->rsa->n),
                    BN_num_bits(child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa->n),
                    SSH_KEY_BITS_RESERVED);
            cleanup_exit_cn(cn, 255);
        }
        if (rsa_private_decrypt(session_key_int, session_key_int,
            child_connect_context[cn].child_sensitive_data.server_key->rsa) <= 0)
            rsafail++;
        if (rsa_private_decrypt(session_key_int, session_key_int,
            child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa) <= 0)
            rsafail++;
    } else {
        /* Host key has bigger modulus (or they are equal). */
        if (BN_num_bits(child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa->n) <
            BN_num_bits(child_connect_context[cn].child_sensitive_data.server_key->rsa->n) + SSH_KEY_BITS_RESERVED) {
            debugl7(SYSLOG_LEVEL_FATAL, "do_connection: %s: host_key %d < server_key %d + SSH_KEY_BITS_RESERVED %d",
                get_remote_ipaddr(cn),
                    BN_num_bits(child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa->n),
                    BN_num_bits(child_connect_context[cn].child_sensitive_data.server_key->rsa->n),
                    SSH_KEY_BITS_RESERVED);
            cleanup_exit_cn(cn, 255);
        }
        if (rsa_private_decrypt(session_key_int, session_key_int,
            child_connect_context[cn].child_sensitive_data.ssh1_host_key->rsa) < 0)
            rsafail++;
        if (rsa_private_decrypt(session_key_int, session_key_int,
            child_connect_context[cn].child_sensitive_data.server_key->rsa) < 0)
            rsafail++;
    }
#endif /* L7_SSHD */
    return (rsafail);
}
/*
 * SSH1 key exchange
 */
#ifndef L7_SSHD
static void
do_ssh1_kex(void)
#else
void
do_ssh1_kex(int cn, sensitive_data_t sensitive_data)
#endif /* L7_SSHD */
{
    int i, len;
    int rsafail = 0;
    BIGNUM *session_key_int;
    u_char session_key[SSH_SESSION_KEY_LENGTH];
    u_char cookie[8];
    u_int cipher_type, auth_mask, protocol_flags;
    u_int32_t rnd = 0;

    /*
     * Generate check bytes that the client must send back in the user
     * packet in order for it to be accepted; this is used to defy ip
     * spoofing attacks.  Note that this only works against somebody
     * doing IP spoofing from a remote machine; any machine on the local
     * network can still see outgoing packets and catch the random
     * cookie.  This only affects rhosts authentication, and this is one
     * of the reasons why it is inherently insecure.
     */
    for (i = 0; i < 8; i++) {
        if (i % 4 == 0)
            rnd = arc4random();
        cookie[i] = rnd & 0xff;
        rnd >>= 8;
    }

    /*
     * Send our public key.  We include in the packet 64 bits of random
     * data that must be matched in the reply in order to prevent IP
     * spoofing.
     */
#ifndef L7_SSHD
    packet_start(SSH_SMSG_PUBLIC_KEY);
    for (i = 0; i < 8; i++)
        packet_put_char(cookie[i]);

    /* Store our public server RSA key. */
    packet_put_int(BN_num_bits(sensitive_data.server_key->rsa->n));
    packet_put_bignum(sensitive_data.server_key->rsa->e);
    packet_put_bignum(sensitive_data.server_key->rsa->n);

    /* Store our public host RSA key. */
    packet_put_int(BN_num_bits(sensitive_data.ssh1_host_key->rsa->n));
    packet_put_bignum(sensitive_data.ssh1_host_key->rsa->e);
    packet_put_bignum(sensitive_data.ssh1_host_key->rsa->n);

    /* Put protocol flags. */
    packet_put_int(SSH_PROTOFLAG_HOST_IN_FWD_OPEN);

    /* Declare which ciphers we support. */
    packet_put_int(cipher_mask_ssh1(0));
#else
    ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] = NULL;
    packet_start(cn, SSH_SMSG_PUBLIC_KEY);
    for (i = 0; i < 8; i++)
        packet_put_char(cn, cookie[i]);

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: store public server RSA key", cn);
    /* Store our public server RSA key. */
    packet_put_int(cn, BN_num_bits(sensitive_data.server_key->rsa->n));
    packet_put_bignum(cn, sensitive_data.server_key->rsa->e);
    packet_put_bignum(cn, sensitive_data.server_key->rsa->n);

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: store public host RSA key", cn);
    /* Store our public host RSA key. */
    packet_put_int(cn, BN_num_bits(sensitive_data.ssh1_host_key->rsa->n));
    packet_put_bignum(cn, sensitive_data.ssh1_host_key->rsa->e);
    packet_put_bignum(cn, sensitive_data.ssh1_host_key->rsa->n);

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: store protocol flags", cn);
    /* Put protocol flags. */
    packet_put_int(cn, SSH_PROTOFLAG_HOST_IN_FWD_OPEN);

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: store cipher mask", cn);
    /* Declare which ciphers we support. */
    packet_put_int(cn, cipher_mask_ssh1(0));
#endif /* L7_SSHD */

    /* Declare supported authentication types. */
    auth_mask = 0;
    if (options.rhosts_rsa_authentication)
        auth_mask |= 1 << SSH_AUTH_RHOSTS_RSA;
    if (options.rsa_authentication)
        auth_mask |= 1 << SSH_AUTH_RSA;
    if (options.challenge_response_authentication == 1)
        auth_mask |= 1 << SSH_AUTH_TIS;
    if (options.password_authentication)
        auth_mask |= 1 << SSH_AUTH_PASSWORD;
#ifndef L7_SSHD
    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: store authentication mask", cn);
    packet_put_int(auth_mask);

    /* Send the packet and wait for it to be sent. */
    packet_send();
    packet_write_wait();
#else
    packet_put_int(cn, auth_mask);

    /* Send the packet and wait for it to be sent. */
    packet_send(cn);
    packet_write_wait(cn);
#endif /* L7_SSHD */

    debug("Sent %d bit server key and %d bit host key.",
        BN_num_bits(sensitive_data.server_key->rsa->n),
        BN_num_bits(sensitive_data.ssh1_host_key->rsa->n));

    /* Read clients reply (cipher type and session key). */
#ifndef L7_SSHD
    packet_read_expect(SSH_CMSG_SESSION_KEY);

    /* Get cipher type and check whether we accept this. */
    cipher_type = packet_get_char();

    if (!(cipher_mask_ssh1(0) & (1 << cipher_type)))
        packet_disconnect("Warning: client selects unsupported cipher.");

    /* Get check bytes from the packet.  These must match those we
       sent earlier with the public key packet. */
    for (i = 0; i < 8; i++)
        if (cookie[i] != packet_get_char())
            packet_disconnect("IP Spoofing check bytes do not match.");
#else
    packet_read_expect(cn, SSH_CMSG_SESSION_KEY);

    /* Get cipher type and check whether we accept this. */
    cipher_type = packet_get_char(cn);

    if (!(cipher_mask_ssh1(0) & (1 << cipher_type)))
        packet_disconnect(cn, "Warning: client selects unsupported cipher.");

    /* Get check bytes from the packet.  These must match those we
       sent earlier with the public key packet. */
    for (i = 0; i < 8; i++)
        if (cookie[i] != packet_get_char(cn))
            packet_disconnect(cn, "IP Spoofing check bytes do not match.");
#endif /* L7_SSHD */

    debug("Encryption type: %.200s", cipher_name(cipher_type));

    /* Get the encrypted integer. */
    if ((session_key_int = BN_new()) == NULL)
        fatal("do_ssh1_kex: BN_new failed");
#ifndef L7_SSHD
    if ((session_key_int = BN_new()) == NULL)
        fatal("do_ssh1_kex: BN_new failed");
    packet_get_bignum(session_key_int);

    protocol_flags = packet_get_int();
    packet_set_protocol_flags(protocol_flags);
    packet_check_eom();
#else
    if ((session_key_int = BN_new()) == NULL)
    {
        debugl7(SYSLOG_LEVEL_FATAL, "do_ssh1_kex: BN_new failed");
        cleanup_exit_cn(cn, 255);
    }
    packet_get_bignum(cn, session_key_int);

    protocol_flags = packet_get_int(cn);
    packet_set_protocol_flags(cn, protocol_flags);
    packet_check_eom(cn);
#endif /* L7_SSHD */

    /* Decrypt session_key_int using host/server keys */
#ifndef L7_SSHD
    rsafail = PRIVSEP(ssh1_session_key(session_key_int));
#else
    rsafail = PRIVSEP(ssh1_session_key(cn, session_key_int));
#endif /* L7_SSHD */

    /*
     * Extract session key from the decrypted integer.  The key is in the
     * least significant 256 bits of the integer; the first byte of the
     * key is in the highest bits.
     */
    if (!rsafail) {
        BN_mask_bits(session_key_int, sizeof(session_key) * 8);
        len = BN_num_bytes(session_key_int);
        if (len < 0 || (u_int)len > sizeof(session_key)) {
            error("do_connection: bad session key len from %s: "
                "session_key_int %d > sizeof(session_key) %lu",
#ifndef L7_SSHD
                get_remote_ipaddr(), len, (u_long)sizeof(session_key));
#else
                get_remote_ipaddr(cn), len, (u_long)sizeof(session_key));
#endif /* L7_SSHD */
            rsafail++;
        } else {
            memset(session_key, 0, sizeof(session_key));
            BN_bn2bin(session_key_int,
                session_key + sizeof(session_key) - len);

            derive_ssh1_session_id(
                sensitive_data.ssh1_host_key->rsa->n,
                sensitive_data.server_key->rsa->n,
#ifndef L7_SSHD
                cookie, session_id);
#else
                cookie, session_id[cn]);
#endif /* L7_SSHD */
            /*
             * Xor the first 16 bytes of the session key with the
             * session id.
             */
            for (i = 0; i < 16; i++)
#ifndef L7_SSHD
                session_key[i] ^= session_id[i];
#else
                session_key[i] ^= session_id[cn][i];
#endif /* L7_SSHD */
        }
    }
    if (rsafail) {
        int bytes = BN_num_bytes(session_key_int);
        u_char *buf = xmalloc(bytes);
        MD5_CTX md;

        logit("do_connection: generating a fake encryption key");
        BN_bn2bin(session_key_int, buf);
        MD5_Init(&md);
        MD5_Update(&md, buf, bytes);
        MD5_Update(&md, sensitive_data.ssh1_cookie, SSH_SESSION_KEY_LENGTH);
        MD5_Final(session_key, &md);
        MD5_Init(&md);
        MD5_Update(&md, session_key, 16);
        MD5_Update(&md, buf, bytes);
        MD5_Update(&md, sensitive_data.ssh1_cookie, SSH_SESSION_KEY_LENGTH);
        MD5_Final(session_key + 16, &md);
        memset(buf, 0, bytes);
        xfree(buf);
        for (i = 0; i < 16; i++)
#ifndef L7_SSHD
            session_id[i] = session_key[i] ^ session_key[i + 16];
#else
            session_id[cn][i] = session_key[i] ^ session_key[i + 16];
#endif /* L7_SSHD */
    }
    /* Destroy the private and public keys. No longer. */
    destroy_sensitive_data_cn(cn);

#ifndef L7_SSHD
    if (use_privsep)
        mm_ssh1_session_id(session_id);
#endif /* L7_SSHD */

    /* Destroy the decrypted integer.  It is no longer needed. */
    BN_clear_free(session_key_int);

    /* Set the session key.  From this on all communications will be encrypted. */
#ifndef L7_SSHD
    packet_set_encryption_key(session_key, SSH_SESSION_KEY_LENGTH, cipher_type);
#else
    packet_set_encryption_key(cn, session_key, SSH_SESSION_KEY_LENGTH, cipher_type);
#endif /* L7_SSHD */

    /* Destroy our copy of the session key.  It is no longer needed. */
    memset(session_key, 0, sizeof(session_key));

    debug("Received session key; encryption turned on.");

    /* Send an acknowledgment packet.  Note that this packet is sent encrypted. */
#ifndef L7_SSHD
    packet_start(SSH_SMSG_SUCCESS);
    packet_send();
    packet_write_wait();
#else
    packet_start(cn, SSH_SMSG_SUCCESS);
    packet_send(cn);
    packet_write_wait(cn);
#endif /* L7_SSHD */
}

#ifdef L7_SSHD
void xxx_kex_free_cn(int cn)
{
    if (xxx_kex[cn] != NULL)
    {
        if (xxx_kex[cn]->session_id != NULL)
        {
            xfree(xxx_kex[cn]->session_id);
            xxx_kex[cn]->session_id = NULL;
        }

        if (xxx_kex[cn]->server_version_string != NULL)
        {
            xfree(xxx_kex[cn]->server_version_string);
            xxx_kex[cn]->server_version_string = NULL;
        }

        if (xxx_kex[cn]->client_version_string != NULL)
        {
            xfree(xxx_kex[cn]->client_version_string);
            xxx_kex[cn]->client_version_string = NULL;
        }

        buffer_free(&(xxx_kex[cn]->peer));
        buffer_free(&(xxx_kex[cn]->my));

        xfree(xxx_kex[cn]);
        xxx_kex[cn] = NULL;
    }
}
#endif /* L7_SSHD */

/*
 * SSH2 key exchange: diffie-hellman-group1-sha1
 */
#ifndef L7_SSHD
static void
do_ssh2_kex(void)
#else
void
do_ssh2_kex(int cn)
#endif /* L7_SSHD */
{
    Kex *kex;

#ifndef L7_SSHD
    if (options.ciphers != NULL) {
        myproposal[PROPOSAL_ENC_ALGS_CTOS] =
        myproposal[PROPOSAL_ENC_ALGS_STOC] = options.ciphers;
    }
    myproposal[PROPOSAL_ENC_ALGS_CTOS] =
        compat_cipher_proposal(myproposal[PROPOSAL_ENC_ALGS_CTOS]);
    myproposal[PROPOSAL_ENC_ALGS_STOC] =
        compat_cipher_proposal(myproposal[PROPOSAL_ENC_ALGS_STOC]);

    if (options.macs != NULL) {
        myproposal[PROPOSAL_MAC_ALGS_CTOS] =
        myproposal[PROPOSAL_MAC_ALGS_STOC] = options.macs;
    }
    if (options.compression == COMP_NONE) {
        myproposal[PROPOSAL_COMP_ALGS_CTOS] =
        myproposal[PROPOSAL_COMP_ALGS_STOC] = "none";
    } else if (options.compression == COMP_DELAYED) {
        myproposal[PROPOSAL_COMP_ALGS_CTOS] =
        myproposal[PROPOSAL_COMP_ALGS_STOC] = "none,zlib@openssh.com";
    }

    myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] = list_hostkey_types();

    /* start key exchange */
    kex = kex_setup(myproposal);
#else
    ssh_myproposal[cn][0] = KEX_DEFAULT_KEX;
    ssh_myproposal[cn][1] = KEX_DEFAULT_PK_ALG;
    ssh_myproposal[cn][2] = KEX_DEFAULT_ENCRYPT;
    ssh_myproposal[cn][3] = KEX_DEFAULT_ENCRYPT;
    ssh_myproposal[cn][4] = KEX_DEFAULT_MAC;
    ssh_myproposal[cn][5] = KEX_DEFAULT_MAC;
    ssh_myproposal[cn][6] = KEX_DEFAULT_COMP;
    ssh_myproposal[cn][7] = KEX_DEFAULT_COMP;
    ssh_myproposal[cn][8] = KEX_DEFAULT_LANG;
    ssh_myproposal[cn][9] = KEX_DEFAULT_LANG;

    if (options.ciphers != NULL) {
        ssh_myproposal[cn][PROPOSAL_ENC_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_ENC_ALGS_STOC] = options.ciphers;
    }
    ssh_myproposal[cn][PROPOSAL_ENC_ALGS_CTOS] =
        compat_cipher_proposal(cn, ssh_myproposal[cn][PROPOSAL_ENC_ALGS_CTOS]);
    ssh_myproposal[cn][PROPOSAL_ENC_ALGS_STOC] =
        compat_cipher_proposal(cn, ssh_myproposal[cn][PROPOSAL_ENC_ALGS_STOC]);

    if (options.macs != NULL) {
        ssh_myproposal[cn][PROPOSAL_MAC_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_MAC_ALGS_STOC] = options.macs;
    }
    if (options.compression == COMP_NONE) {
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_STOC] = "none";
    } else if (options.compression == COMP_DELAYED) {
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_STOC] = "none,zlib@openssh.com";
    }

    ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] = list_hostkey_types();

    /* start key exchange */
    kex = kex_setup(cn, ssh_myproposal[cn]);
#endif /* L7_SSHD */
    kex->kex[KEX_DH_GRP1_SHA1] = kexdh_server;
    kex->kex[KEX_DH_GRP14_SHA1] = kexdh_server;
    kex->kex[KEX_DH_GEX_SHA1] = kexgex_server;
    kex->server = 1;
#ifndef L7_SSHD
    kex->server_version_string=server_version_string;
    kex->client_version_string=client_version_string;
#else
    kex->server_version_string=sshd_server_version_string[cn];
    kex->client_version_string=sshd_client_version_string[cn];
    sshd_server_version_string[cn] = NULL;
    sshd_client_version_string[cn] = NULL;
#endif /* L7_SSHD */
    kex->load_host_key=&get_hostkey_by_type;
    kex->host_key_index=&get_hostkey_index;

#ifndef L7_SSHD
    xxx_kex = kex;

    dispatch_run(DISPATCH_BLOCK, &kex->done, kex);

    session_id2 = kex->session_id;
    session_id2_len = kex->session_id_len;
#else
    xxx_kex[cn] = kex;

    dispatch_run(cn, DISPATCH_BLOCK, &kex->done, kex);

    if (kex->flags & KEX_ERROR)
    {
      cleanup_exit_cn(cn, 255);
    }
    session_id2[cn] = kex->session_id;
    session_id2_len[cn] = kex->session_id_len;
#endif /* L7_SSHD */

#ifdef DEBUG_KEXDH
    /* send 1st encrypted/maced/compressed message */
#ifndef L7_SSHD
    packet_start(SSH2_MSG_IGNORE);
    packet_put_cstring("markus");
    packet_send();
    packet_write_wait();
#else
    packet_start(cn, SSH2_MSG_IGNORE);
    packet_put_cstring(cn, "markus");
    packet_send(cn);
    packet_write_wait(cn);
#endif /* L7_SSHD */
#endif
    if (ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] != NULL)
    {
        xfree(ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS]);
        ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] = NULL;
    }
    debug("KEX done");
}

/* server specific fatal cleanup */
#ifndef L7_SSHD
void
cleanup_exit(int i)
{
    if (the_authctxt)
        do_cleanup(the_authctxt);
#ifdef SSH_AUDIT_EVENTS
    /* done after do_cleanup so it can cancel the PAM auth 'thread' */
    if (!use_privsep || mm_is_monitor())
        audit_event(SSH_CONNECTION_ABANDON);
#endif
    _exit(i);
}
#else
void
sshd_cleanup_exit(int cn, int i)
{
    if (the_authctxt[cn])
    {
        do_cleanup(the_authctxt[cn]);
        the_authctxt[cn] = NULL;
    }

    /*
    This function is not currently being used, but
    sshdExitSema should be taken before calling
    this function
    */

    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
    connection_out[cn] = connection_in[cn] = -1;
    osapiSemaGive(sshdGlobalSema);

    debugl7(SYSLOG_LEVEL_INFO, "exiting with error code %d ...", i);
    osapiTaskDelete(osapiTaskIdSelf());
}

void
sshd_session_cleanup(int cn)
{
    /*
    Note that sshdExitSema should already be taken
    when entering this function
    */

    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
    if (cn < SSHD_MAX_CONNECTIONS)
    {
        sshdGlobal.sshdNumSessions--;
    }
    connection_out[cn] = connection_in[cn] = -1;
    osapiSemaGive(sshdGlobalSema);
}

void
child_connect_context_cleanup(int cn)
{
    /*
    Cancel the alarm we set to limit the time taken for
    authentication.
    */
    if (child_connect_context[cn].pLoginTimerHolder != NULL)
    {
        debugl7(SYSLOG_LEVEL_TIMER, "Freeing timer for grace_alarm_handler");
        osapiTimerFree(child_connect_context[cn].pLoginTimerHolder);
        child_connect_context[cn].pLoginTimerHolder = NULL;
    }

    destroy_sensitive_data_cn(cn);

    memset(&child_connect_context[cn], 0, sizeof (child_context_t));
}

int
child_connect_context_find(int taskId)
{
    int i;

    for (i = 0; i < L7_OPENSSH_MAX_CONNECTIONS; i++)
    {
        if (child_connect_context[i].selfTaskId == taskId)
        {
            return i;
        }
    }

    return -1;
}
#endif /* L7_SSHD */
