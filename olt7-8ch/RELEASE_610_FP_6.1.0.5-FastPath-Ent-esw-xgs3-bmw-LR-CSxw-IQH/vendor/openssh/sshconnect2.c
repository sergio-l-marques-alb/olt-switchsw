/*
 * Copyright (c) 2000 Markus Friedl.  All rights reserved.
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
RCSID("$OpenBSD: sshconnect2.c,v 1.143 2005/10/14 02:17:59 stevesk Exp $");

#include "openbsd-compat/sys-queue.h"

#include "ssh.h"
#include "ssh2.h"
#include "xmalloc.h"
#include "buffer.h"
#include "packet.h"
#include "compat.h"
#include "bufaux.h"
#include "cipher.h"
#include "kex.h"
#include "myproposal.h"
#include "sshconnect.h"
#include "authfile.h"
#include "dh.h"
#include "authfd.h"
#include "log.h"
#include "readconf.h"
#include "misc.h"
#include "match.h"
#include "dispatch.h"
#include "canohost.h"
#include "msg.h"
#include "pathnames.h"

#ifdef GSSAPI
#include "ssh-gss.h"
#endif

/* import */
#ifndef L7_SSHD
extern char *server_version_string;
extern char *client_version_string;
extern Options options;
#else
extern char *sshc_server_version_string[];
extern char *sshc_client_version_string[];
extern Options client_options;
extern char *ssh_myproposal[L7_OPENSSH_MAX_CONNECTIONS][PROPOSAL_MAX];
#endif /* L7_SSHD */

/*
 * SSH2 key exchange
 */

#ifndef L7_SSHD
u_char *session_id2 = NULL;
u_int session_id2_len = 0;
#else
u_char *sshc_session_id2[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};
u_int sshc_session_id2_len[L7_OPENSSH_MAX_CONNECTIONS] = {0};
#endif /* L7_SSHD */

char *xxx_host;
struct sockaddr *xxx_hostaddr;

#ifndef L7_SSHD
Kex *xxx_kex = NULL;
#else
Kex *xxx_kex[L7_OPENSSH_MAX_CONNECTIONS] = {NULL};
#endif /* L7_SSHD */

static int
verify_host_key_callback(Key *hostkey)
{
    if (verify_host_key(xxx_host, xxx_hostaddr, hostkey) == -1)
        fatal("Host key verification failed.");
    return 0;
}

#ifndef L7_SSHD
void
ssh_kex2(char *host, struct sockaddr *hostaddr)
#else
int
ssh_kex2(int cn, char *host, struct sockaddr *hostaddr)
#endif /* L7_SSHD */
{
    Kex *kex;

    xxx_host = host;
    xxx_hostaddr = hostaddr;

#ifndef L7_SSHD
    if (options.ciphers == (char *)-1) {
        logit("No valid ciphers for protocol version 2 given, using defaults.");
        options.ciphers = NULL;
    }
    if (options.ciphers != NULL) {
        myproposal[PROPOSAL_ENC_ALGS_CTOS] =
        myproposal[PROPOSAL_ENC_ALGS_STOC] = options.ciphers;
    }
    myproposal[PROPOSAL_ENC_ALGS_CTOS] =
        compat_cipher_proposal(myproposal[PROPOSAL_ENC_ALGS_CTOS]);
    myproposal[PROPOSAL_ENC_ALGS_STOC] =
        compat_cipher_proposal(myproposal[PROPOSAL_ENC_ALGS_STOC]);
    if (options.compression) {
        myproposal[PROPOSAL_COMP_ALGS_CTOS] =
        myproposal[PROPOSAL_COMP_ALGS_STOC] = "zlib@openssh.com,zlib,none";
    } else {
        myproposal[PROPOSAL_COMP_ALGS_CTOS] =
        myproposal[PROPOSAL_COMP_ALGS_STOC] = "none,zlib@openssh.com,zlib";
    }
    if (options.macs != NULL) {
        myproposal[PROPOSAL_MAC_ALGS_CTOS] =
        myproposal[PROPOSAL_MAC_ALGS_STOC] = options.macs;
    }
    if (options.hostkeyalgorithms != NULL)
        myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] =
            options.hostkeyalgorithms;

    if (options.rekey_limit)
        packet_set_rekey_limit(options.rekey_limit);

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

    if (client_options.ciphers == (char *)-1) {
        logit("No valid ciphers for protocol version 2 given, using defaults.");
        client_options.ciphers = NULL;
    }
    if (client_options.ciphers != NULL) {
        ssh_myproposal[cn][PROPOSAL_ENC_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_ENC_ALGS_STOC] = client_options.ciphers;
    }
    ssh_myproposal[cn][PROPOSAL_ENC_ALGS_CTOS] =
        compat_cipher_proposal(cn, ssh_myproposal[cn][PROPOSAL_ENC_ALGS_CTOS]);
    ssh_myproposal[cn][PROPOSAL_ENC_ALGS_STOC] =
        compat_cipher_proposal(cn, ssh_myproposal[cn][PROPOSAL_ENC_ALGS_STOC]);
    if (client_options.compression) {
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_STOC] = "zlib@openssh.com,zlib,none";
    } else {
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_COMP_ALGS_STOC] = "none,zlib@openssh.com,zlib";
    }
    if (client_options.macs != NULL) {
        ssh_myproposal[cn][PROPOSAL_MAC_ALGS_CTOS] =
        ssh_myproposal[cn][PROPOSAL_MAC_ALGS_STOC] = client_options.macs;
    }
    if (client_options.hostkeyalgorithms != NULL)
        ssh_myproposal[cn][PROPOSAL_SERVER_HOST_KEY_ALGS] =
            client_options.hostkeyalgorithms;

    if (client_options.rekey_limit)
        packet_set_rekey_limit(cn, client_options.rekey_limit);

    /* start key exchange */
    kex = kex_setup(cn, ssh_myproposal[cn]);
#endif /* L7_SSHD */
    kex->kex[KEX_DH_GRP1_SHA1] = kexdh_client;
    kex->kex[KEX_DH_GRP14_SHA1] = kexdh_client;
    kex->kex[KEX_DH_GEX_SHA1] = kexgex_client;
#ifndef L7_SSHD
    kex->server_version_string=server_version_string;
    kex->client_version_string=client_version_string;
#else
    kex->server_version_string=sshc_server_version_string[cn];
    kex->client_version_string=sshc_client_version_string[cn];

    sshc_server_version_string[cn] = NULL;
    sshc_client_version_string[cn] = NULL;
#endif /* L7_SSHD */
    kex->verify_host_key=&verify_host_key_callback;

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
      return -1;
    }

    sshc_session_id2[cn] = kex->session_id;
    sshc_session_id2_len[cn] = kex->session_id_len;
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

#ifdef L7_SSHD
    return 0;
#endif /* L7_SSHD */
}

/*
 * Authenticate user
 */

typedef struct Authctxt Authctxt;
typedef struct Authmethod Authmethod;
typedef struct identity Identity;
typedef struct idlist Idlist;

struct identity {
    TAILQ_ENTRY(identity) next;
    AuthenticationConnection *ac;   /* set if agent supports key */
    Key *key;           /* public/private key */
    char    *filename;      /* comment for agent-only keys */
    int tried;
    int isprivate;      /* key points to the private key */
};
TAILQ_HEAD(idlist, identity);

struct Authctxt {
    const char *server_user;
    const char *local_user;
    const char *host;
    const char *service;
    Authmethod *method;
    int success;
    char *authlist;
    /* pubkey */
    Idlist keys;
    AuthenticationConnection *agent;
    /* hostbased */
    Sensitive *sensitive;
    /* kbd-interactive */
    int info_req_seen;
    /* generic */
    void *methoddata;
#ifdef L7_SSHD
    int cn;
    char *password;
    int authdone;
#endif /* L7_SSHD */
};
struct Authmethod {
    char    *name;      /* string to compare against server's list */
    int (*userauth)(Authctxt *authctxt);
    int *enabled;   /* flag in option struct that enables method */
    int *batch_flag;    /* flag in option struct that disables method */
};

#ifndef L7_SSHD
void    input_userauth_success(int, u_int32_t, void *);
void    input_userauth_failure(int, u_int32_t, void *);
void    input_userauth_banner(int, u_int32_t, void *);
void    input_userauth_error(int, u_int32_t, void *);
void    input_userauth_info_req(int, u_int32_t, void *);
void    input_userauth_pk_ok(int, u_int32_t, void *);
void    input_userauth_passwd_changereq(int, u_int32_t, void *);
#else
void    input_userauth_success(int, int, u_int32_t, void *);
void    input_userauth_failure(int, int, u_int32_t, void *);
void    input_userauth_banner(int, int, u_int32_t, void *);
void    input_userauth_error(int, int, u_int32_t, void *);
void    input_userauth_info_req(int, int, u_int32_t, void *);
void    input_userauth_pk_ok(int, int, u_int32_t, void *);
void    input_userauth_passwd_changereq(int, int, u_int32_t, void *);
#endif /* L7_SSHD */

int userauth_none(Authctxt *);
int userauth_pubkey(Authctxt *);
int userauth_passwd(Authctxt *);
int userauth_kbdint(Authctxt *);
int userauth_hostbased(Authctxt *);
int userauth_kerberos(Authctxt *);

#ifdef GSSAPI
int userauth_gssapi(Authctxt *authctxt);
void    input_gssapi_response(int type, u_int32_t, void *);
void    input_gssapi_token(int type, u_int32_t, void *);
void    input_gssapi_hash(int type, u_int32_t, void *);
void    input_gssapi_error(int, u_int32_t, void *);
void    input_gssapi_errtok(int, u_int32_t, void *);
#endif

void    userauth(Authctxt *, char *);

static int sign_and_send_pubkey(Authctxt *, Identity *);
static void pubkey_prepare(Authctxt *);
static void pubkey_cleanup(Authctxt *);
static Key *load_identity_file(char *);

static Authmethod *authmethod_get(char *authlist);
static Authmethod *authmethod_lookup(const char *name);
static char *authmethods_get(void);

#ifdef L7_SSHD /* RELOCATED TO TOP OF FILE */
/* XXX internal state */
static Authmethod *current = NULL;
static char *supported = NULL;
static char *preferred = NULL;
#endif /* L7_SSHD */

#ifndef L7_SSHD
Authmethod authmethods[] = {
#ifdef GSSAPI
    {"gssapi-with-mic",
        userauth_gssapi,
        &options.gss_authentication,
        NULL},
#endif
    {"hostbased",
        userauth_hostbased,
        &options.hostbased_authentication,
        NULL},
    {"publickey",
        userauth_pubkey,
        &options.pubkey_authentication,
        NULL},
    {"keyboard-interactive",
        userauth_kbdint,
        &options.kbd_interactive_authentication,
        &options.batch_mode},
    {"password",
        userauth_passwd,
        &options.password_authentication,
        &options.batch_mode},
    {"none",
        userauth_none,
        NULL,
        NULL},
    {NULL, NULL, NULL, NULL}
};
#else
Authmethod sshc_authmethods[] = {
#ifdef GSSAPI
    {"gssapi-with-mic",
        userauth_gssapi,
        &client_options.gss_authentication,
        NULL},
#endif
    {"hostbased",
        userauth_hostbased,
        &client_options.hostbased_authentication,
        NULL},
    {"publickey",
        userauth_pubkey,
        &client_options.pubkey_authentication,
        NULL},
    {"keyboard-interactive",
        userauth_kbdint,
        &client_options.kbd_interactive_authentication,
        &client_options.batch_mode},
    {"password",
        userauth_passwd,
        &client_options.password_authentication,
        &client_options.batch_mode},
    {"none",
        userauth_none,
        NULL,
        NULL},
    {NULL, NULL, NULL, NULL}
};
#endif /* L7_SSHD */

#ifndef L7_SSHD
void
ssh_userauth2(const char *local_user, const char *server_user, char *host,
    Sensitive *sensitive)
#else
int
ssh_userauth2(int cn, const char *local_user, const char *server_user, char *host,
    Sensitive *sensitive, char *password)
#endif /* L7_SSHD */
{
    Authctxt authctxt;
    int type;

#ifndef L7_SSHD
    if (options.challenge_response_authentication)
        options.kbd_interactive_authentication = 1;

    packet_start(SSH2_MSG_SERVICE_REQUEST);
    packet_put_cstring("ssh-userauth");
    packet_send();
    debug("SSH2_MSG_SERVICE_REQUEST sent");
    packet_write_wait();
    type = packet_read();
    if (type != SSH2_MSG_SERVICE_ACCEPT)
        fatal("Server denied authentication request: %d", type);
    if (packet_remaining() > 0) {
        char *reply = packet_get_string(NULL);
#else
    if (client_options.challenge_response_authentication)
        client_options.kbd_interactive_authentication = 1;

    packet_start(cn, SSH2_MSG_SERVICE_REQUEST);
    packet_put_cstring(cn, "ssh-userauth");
    packet_send(cn);
    debug("SSH2_MSG_SERVICE_REQUEST sent");
    packet_write_wait(cn);
    type = packet_read(cn);
    if (type != SSH2_MSG_SERVICE_ACCEPT)
    {
      error("Server denied authentication request: %d", type);
      return -1;
    }
    if (packet_remaining(cn) > 0) {
        char *reply = packet_get_string(cn, NULL);
#endif /* L7_SSHD */
        debug2("service_accept: %s", reply);
        xfree(reply);
    } else {
        debug2("buggy server: service_accept w/o service");
    }
#ifndef L7_SSHD
    packet_check_eom();
    debug("SSH2_MSG_SERVICE_ACCEPT received");

    if (options.preferred_authentications == NULL)
        options.preferred_authentications = authmethods_get();
#else
    packet_check_eom(cn);
    debug("SSH2_MSG_SERVICE_ACCEPT received");

    if (client_options.preferred_authentications == NULL)
        client_options.preferred_authentications = authmethods_get();
#endif /* L7_SSHD */

    /* setup authentication context */
    memset(&authctxt, 0, sizeof(authctxt));
    pubkey_prepare(&authctxt);
    authctxt.server_user = server_user;
    authctxt.local_user = local_user;
    authctxt.host = host;
    authctxt.service = "ssh-connection";        /* service name */
    authctxt.success = 0;
    authctxt.method = authmethod_lookup("none");
    authctxt.authlist = NULL;
    authctxt.methoddata = NULL;
    authctxt.sensitive = sensitive;
    authctxt.info_req_seen = 0;
#ifdef L7_SSHD
    authctxt.authdone = 0;
    authctxt.cn = cn;
#endif /* L7_SSHD */
#ifndef L7_SSHD
    if (authctxt.method == NULL)
        fatal("ssh_userauth2: internal error: cannot send userauth none request");
#else
    if (authctxt.method == NULL)
    {
      error("ssh_userauth2: internal error: cannot send userauth none request");
      if (supported != NULL)
      {
        xfree(supported);
        supported = NULL;
      }
      return -1;
    }
#endif /* L7_SSHD */

#ifdef L7_SSHD
    authctxt.password = password;
#endif
    /* initial userauth request */
    userauth_none(&authctxt);

#ifndef L7_SSHD
    dispatch_init(&input_userauth_error);
    dispatch_set(SSH2_MSG_USERAUTH_SUCCESS, &input_userauth_success);
    dispatch_set(SSH2_MSG_USERAUTH_FAILURE, &input_userauth_failure);
    dispatch_set(SSH2_MSG_USERAUTH_BANNER, &input_userauth_banner);
    dispatch_run(DISPATCH_BLOCK, &authctxt.success, &authctxt); /* loop until success */
#else
    dispatch_init(cn, &input_userauth_error);
    dispatch_set(cn, SSH2_MSG_USERAUTH_SUCCESS, &input_userauth_success);
    dispatch_set(cn, SSH2_MSG_USERAUTH_FAILURE, &input_userauth_failure);
    dispatch_set(cn, SSH2_MSG_USERAUTH_BANNER, &input_userauth_banner);
    dispatch_run(cn, DISPATCH_BLOCK, &authctxt.authdone, &authctxt); /* loop until success */
#endif /* L7_SSHD */

    pubkey_cleanup(&authctxt);
#ifndef L7_SSHD
    dispatch_range(SSH2_MSG_USERAUTH_MIN, SSH2_MSG_USERAUTH_MAX, NULL);

    debug("Authentication succeeded (%s).", authctxt.method->name);
#else
    dispatch_range(cn, SSH2_MSG_USERAUTH_MIN, SSH2_MSG_USERAUTH_MAX, NULL);

    if (authctxt.success == 1)
    {
      debug("Authentication succeeded (%s).", authctxt.method->name);
    }
    else
    {
      debug("Authentication failed (%s).", authctxt.method->name);
    }
    if (client_options.preferred_authentications != NULL)
    {
      xfree(client_options.preferred_authentications);
      client_options.preferred_authentications = NULL;
    }
    if (supported != NULL)
    {
      xfree(supported);
      supported = NULL;
    }
    return authctxt.success;
#endif /* L7_SSHD */
}

void
userauth(Authctxt *authctxt, char *authlist)
{
    if (authctxt->methoddata) {
        xfree(authctxt->methoddata);
        authctxt->methoddata = NULL;
    }
    if (authlist == NULL) {
        authlist = authctxt->authlist;
    } else {
        if (authctxt->authlist)
            xfree(authctxt->authlist);
        authctxt->authlist = authlist;
    }
    for (;;) {
        Authmethod *method = authmethod_get(authlist);
        if (method == NULL)
            fatal("Permission denied (%s).", authlist);
        authctxt->method = method;

        /* reset the per method handler */
#ifndef L7_SSHD
        dispatch_range(SSH2_MSG_USERAUTH_PER_METHOD_MIN,
            SSH2_MSG_USERAUTH_PER_METHOD_MAX, NULL);
#else
        dispatch_range(authctxt->cn, SSH2_MSG_USERAUTH_PER_METHOD_MIN,
            SSH2_MSG_USERAUTH_PER_METHOD_MAX, NULL);
#endif /* L7_SSHD */

        /* and try new method */
        if (method->userauth(authctxt) != 0) {
            debug2("we sent a %s packet, wait for reply", method->name);
            break;
        } else {
            debug2("we did not send a packet, disable method");
            method->enabled = NULL;
        }
    }
}

#ifndef L7_SSHD
void
input_userauth_error(int type, u_int32_t seq, void *ctxt)
#else
void
input_userauth_error(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
#ifndef L7_SSHD
    fatal("input_userauth_error: bad message during authentication: "
        "type %d", type);
#else
    Authctxt *authctxt = ctxt;
    error("input_userauth_error: bad message during authentication: "
        "type %d", type);
    if (authctxt != NULL)
    {
      authctxt->authdone = 1;          /* break out */
    }
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
void
input_userauth_banner(int type, u_int32_t seq, void *ctxt)
#else
void
input_userauth_banner(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    char *msg, *lang;

    debug3("input_userauth_banner");
#ifndef L7_SSHD
    msg = packet_get_string(NULL);
    lang = packet_get_string(NULL);
    if (options.log_level > SYSLOG_LEVEL_QUIET)
        fprintf(stderr, "%s", msg);
#else
    msg = packet_get_string(cn, NULL);
    lang = packet_get_string(cn, NULL);
    if (client_options.log_level > SYSLOG_LEVEL_QUIET)
        fprintf(stderr, "%s", msg);
#endif /* L7_SSHD */
    xfree(msg);
    xfree(lang);
}

#ifndef L7_SSHD
void
input_userauth_success(int type, u_int32_t seq, void *ctxt)
#else
void
input_userauth_success(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    if (authctxt == NULL)
#ifndef L7_SSHD
        fatal("input_userauth_success: no authentication context");
#else
        fatal_cn(cn, "input_userauth_success: no authentication context");
#endif /* L7_SSHD */
    if (authctxt->authlist) {
        xfree(authctxt->authlist);
        authctxt->authlist = NULL;
    }
    if (authctxt->methoddata) {
        xfree(authctxt->methoddata);
        authctxt->methoddata = NULL;
    }
    authctxt->success = 1;          /* break out */
#ifdef L7_SSHD
    authctxt->authdone = 1;          /* break out */
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
void
input_userauth_failure(int type, u_int32_t seq, void *ctxt)
#else
void
input_userauth_failure(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    char *authlist = NULL;
    int partial;

#ifndef L7_SSHD
    if (authctxt == NULL)
        fatal("input_userauth_failure: no authentication context");

    authlist = packet_get_string(NULL);
    partial = packet_get_char();
    packet_check_eom();
#else
    if (authctxt == NULL)
        fatal_cn(cn, "input_userauth_failure: no authentication context");

    authlist = packet_get_string(cn, NULL);
    partial = packet_get_char(cn);
    packet_check_eom(cn);
#endif /* L7_SSHD */

    if (partial != 0)
        logit("Authenticated with partial success.");
    debug("Authentications that can continue: %s", authlist);

#ifdef L7_SSHD
    if (strcmp(authctxt->method->name, "password") == 0)
    {
      if (authctxt->authlist) {
          xfree(authctxt->authlist);
          authctxt->authlist = NULL;
      }
      if (authctxt->methoddata) {
          xfree(authctxt->methoddata);
          authctxt->methoddata = NULL;
      }
      if (authlist)
      {
        xfree(authlist);
      }
      authctxt->authdone = 1;
    }
    else
    {
      userauth(authctxt, authlist);
    }
#else /* L7_SSHD */
    userauth(authctxt, authlist);
#endif /* L7_SSHD */
}
#ifndef L7_SSHD
void
input_userauth_pk_ok(int type, u_int32_t seq, void *ctxt)
#else
void
input_userauth_pk_ok(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    Key *key = NULL;
    Identity *id = NULL;
    Buffer b;
    int pktype, sent = 0;
    u_int alen, blen;
    char *pkalg, *fp;
    u_char *pkblob;

#ifndef L7_SSHD
    if (authctxt == NULL)
        fatal("input_userauth_pk_ok: no authentication context");
    if (datafellows & SSH_BUG_PKOK) {
#else
    if (authctxt == NULL)
        fatal_cn(cn, "input_userauth_pk_ok: no authentication context");
    if (datafellows[cn] & SSH_BUG_PKOK) {
#endif /* L7_SSHD */
        /* this is similar to SSH_BUG_PKAUTH */
        debug2("input_userauth_pk_ok: SSH_BUG_PKOK");
#ifndef L7_SSHD
        pkblob = packet_get_string(&blen);
#else
        pkblob = packet_get_string(cn, &blen);
#endif /* L7_SSHD */
        buffer_init(&b);
        buffer_append(&b, pkblob, blen);
        pkalg = buffer_get_string(&b, &alen);
        buffer_free(&b);
    } else {
#ifndef L7_SSHD
        pkalg = packet_get_string(&alen);
        pkblob = packet_get_string(&blen);
#else
        pkalg = packet_get_string(cn, &alen);
        pkblob = packet_get_string(cn, &blen);
#endif /* L7_SSHD */
    }
#ifndef L7_SSHD
    packet_check_eom();
#else
    packet_check_eom(cn);
#endif /* L7_SSHD */

    debug("Server accepts key: pkalg %s blen %u", pkalg, blen);

    if ((pktype = key_type_from_name(pkalg)) == KEY_UNSPEC) {
        debug("unknown pkalg %s", pkalg);
        goto done;
    }
#ifndef L7_SSHD
    if ((key = key_from_blob(pkblob, blen)) == NULL) {
#else
    if ((key = key_from_blob(cn, pkblob, blen)) == NULL) {
#endif /* L7_SSHD */
        debug("no key from blob. pkalg %s", pkalg);
        goto done;
    }
    if (key->type != pktype) {
        error("input_userauth_pk_ok: type mismatch "
            "for decoded key (received %d, expected %d)",
            key->type, pktype);
        goto done;
    }
    fp = key_fingerprint(key, SSH_FP_MD5, SSH_FP_HEX);
    debug2("input_userauth_pk_ok: fp %s", fp);
    xfree(fp);

    /*
     * search keys in the reverse order, because last candidate has been
     * moved to the end of the queue.  this also avoids confusion by
     * duplicate keys
     */
    TAILQ_FOREACH_REVERSE(id, &authctxt->keys, idlist, next) {
        if (key_equal(key, id->key)) {
            sent = sign_and_send_pubkey(authctxt, id);
            break;
        }
    }
done:
    if (key != NULL)
        key_free(key);
    xfree(pkalg);
    xfree(pkblob);

    /* try another method if we did not send a packet */
    if (sent == 0)
        userauth(authctxt, NULL);
}

#ifdef GSSAPI
int
userauth_gssapi(Authctxt *authctxt)
{
    Gssctxt *gssctxt = NULL;
    static gss_OID_set gss_supported = NULL;
    static u_int mech = 0;
    OM_uint32 min;
    int ok = 0;

    /* Try one GSSAPI method at a time, rather than sending them all at
     * once. */

    if (gss_supported == NULL)
        gss_indicate_mechs(&min, &gss_supported);

    /* Check to see if the mechanism is usable before we offer it */
    while (mech < gss_supported->count && !ok) {
        if (gssctxt)
            ssh_gssapi_delete_ctx(&gssctxt);
        ssh_gssapi_build_ctx(&gssctxt);
        ssh_gssapi_set_oid(gssctxt, &gss_supported->elements[mech]);

        /* My DER encoding requires length<128 */
        if (gss_supported->elements[mech].length < 128 &&
            !GSS_ERROR(ssh_gssapi_import_name(gssctxt,
            authctxt->host))) {
            ok = 1; /* Mechanism works */
        } else {
            mech++;
        }
    }

    if (!ok)
        return 0;

    authctxt->methoddata=(void *)gssctxt;

    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);

    packet_put_int(1);

    packet_put_int((gss_supported->elements[mech].length) + 2);
    packet_put_char(SSH_GSS_OIDTYPE);
    packet_put_char(gss_supported->elements[mech].length);
    packet_put_raw(gss_supported->elements[mech].elements,
        gss_supported->elements[mech].length);

    packet_send();

    dispatch_set(SSH2_MSG_USERAUTH_GSSAPI_RESPONSE, &input_gssapi_response);
    dispatch_set(SSH2_MSG_USERAUTH_GSSAPI_TOKEN, &input_gssapi_token);
    dispatch_set(SSH2_MSG_USERAUTH_GSSAPI_ERROR, &input_gssapi_error);
    dispatch_set(SSH2_MSG_USERAUTH_GSSAPI_ERRTOK, &input_gssapi_errtok);

    mech++; /* Move along to next candidate */

    return 1;
}

static OM_uint32
process_gssapi_token(void *ctxt, gss_buffer_t recv_tok)
{
    Authctxt *authctxt = ctxt;
    Gssctxt *gssctxt = authctxt->methoddata;
    gss_buffer_desc send_tok = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc mic = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc gssbuf;
    OM_uint32 status, ms, flags;
    Buffer b;

    status = ssh_gssapi_init_ctx(gssctxt, options.gss_deleg_creds,
        recv_tok, &send_tok, &flags);

    if (send_tok.length > 0) {
        if (GSS_ERROR(status))
            packet_start(SSH2_MSG_USERAUTH_GSSAPI_ERRTOK);
        else
            packet_start(SSH2_MSG_USERAUTH_GSSAPI_TOKEN);

        packet_put_string(send_tok.value, send_tok.length);
        packet_send();
        gss_release_buffer(&ms, &send_tok);
    }

    if (status == GSS_S_COMPLETE) {
        /* send either complete or MIC, depending on mechanism */
        if (!(flags & GSS_C_INTEG_FLAG)) {
            packet_start(SSH2_MSG_USERAUTH_GSSAPI_EXCHANGE_COMPLETE);
            packet_send();
        } else {
            ssh_gssapi_buildmic(&b, authctxt->server_user,
                authctxt->service, "gssapi-with-mic");

            gssbuf.value = buffer_ptr(&b);
            gssbuf.length = buffer_len(&b);

            status = ssh_gssapi_sign(gssctxt, &gssbuf, &mic);

            if (!GSS_ERROR(status)) {
                packet_start(SSH2_MSG_USERAUTH_GSSAPI_MIC);
                packet_put_string(mic.value, mic.length);

                packet_send();
            }

            buffer_free(&b);
            gss_release_buffer(&ms, &mic);
        }
    }

    return status;
}

void
input_gssapi_response(int type, u_int32_t plen, void *ctxt)
{
    Authctxt *authctxt = ctxt;
    Gssctxt *gssctxt;
    int oidlen;
    char *oidv;

    if (authctxt == NULL)
        fatal("input_gssapi_response: no authentication context");
    gssctxt = authctxt->methoddata;

    /* Setup our OID */
    oidv = packet_get_string(&oidlen);

    if (oidlen <= 2 ||
        oidv[0] != SSH_GSS_OIDTYPE ||
        oidv[1] != oidlen - 2) {
        xfree(oidv);
        debug("Badly encoded mechanism OID received");
        userauth(authctxt, NULL);
        return;
    }

    if (!ssh_gssapi_check_oid(gssctxt, oidv + 2, oidlen - 2))
        fatal("Server returned different OID than expected");

    packet_check_eom();

    xfree(oidv);

    if (GSS_ERROR(process_gssapi_token(ctxt, GSS_C_NO_BUFFER))) {
        /* Start again with next method on list */
        debug("Trying to start again");
        userauth(authctxt, NULL);
        return;
    }
}

void
input_gssapi_token(int type, u_int32_t plen, void *ctxt)
{
    Authctxt *authctxt = ctxt;
    gss_buffer_desc recv_tok;
    OM_uint32 status;
    u_int slen;

    if (authctxt == NULL)
        fatal("input_gssapi_response: no authentication context");

    recv_tok.value = packet_get_string(&slen);
    recv_tok.length = slen; /* safe typecast */

    packet_check_eom();

    status = process_gssapi_token(ctxt, &recv_tok);

    xfree(recv_tok.value);

    if (GSS_ERROR(status)) {
        /* Start again with the next method in the list */
        userauth(authctxt, NULL);
        return;
    }
}

void
input_gssapi_errtok(int type, u_int32_t plen, void *ctxt)
{
    Authctxt *authctxt = ctxt;
    Gssctxt *gssctxt;
    gss_buffer_desc send_tok = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc recv_tok;
    OM_uint32 status, ms;
    u_int len;

    if (authctxt == NULL)
        fatal("input_gssapi_response: no authentication context");
    gssctxt = authctxt->methoddata;

    recv_tok.value = packet_get_string(&len);
    recv_tok.length = len;

    packet_check_eom();

    /* Stick it into GSSAPI and see what it says */
    status = ssh_gssapi_init_ctx(gssctxt, options.gss_deleg_creds,
        &recv_tok, &send_tok, NULL);

    xfree(recv_tok.value);
    gss_release_buffer(&ms, &send_tok);

    /* Server will be returning a failed packet after this one */
}

void
input_gssapi_error(int type, u_int32_t plen, void *ctxt)
{
    OM_uint32 maj, min;
    char *msg;
    char *lang;

    maj=packet_get_int();
    min=packet_get_int();
    msg=packet_get_string(NULL);
    lang=packet_get_string(NULL);

    packet_check_eom();

    debug("Server GSSAPI Error:\n%s", msg);
    xfree(msg);
    xfree(lang);
}
#endif /* GSSAPI */

int
userauth_none(Authctxt *authctxt)
{
    /* initial userauth request */
#ifndef L7_SSHD
    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);
    packet_send();
#else
    packet_start(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->cn, authctxt->server_user);
    packet_put_cstring(authctxt->cn, authctxt->service);
    packet_put_cstring(authctxt->cn, authctxt->method->name);
    packet_send(authctxt->cn);
#endif /* L7_SSHD */
    return 1;
}

int
userauth_passwd(Authctxt *authctxt)
{
#ifndef L7_SSHD
    static int attempt = 0;
    char prompt[150];
    char *password;

    if (attempt++ >= options.number_of_password_prompts)
        return 0;

    if (attempt != 1)
        error("Permission denied, please try again.");

    snprintf(prompt, sizeof(prompt), "%.30s@%.128s's password: ",
             authctxt->server_user, authctxt->host);

    password = read_passphrase(prompt, 0);
#else
    char *password = authctxt->password;
#endif /* L7_SSHD */
#ifndef L7_SSHD
    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);
    packet_put_char(0);
    packet_put_cstring(password);
    memset(password, 0, strlen(password));
    xfree(password);
    packet_add_padding(64);
    packet_send();

    dispatch_set(SSH2_MSG_USERAUTH_PASSWD_CHANGEREQ,
        &input_userauth_passwd_changereq);
#else
    packet_start(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->cn, authctxt->server_user);
    packet_put_cstring(authctxt->cn, authctxt->service);
    packet_put_cstring(authctxt->cn, authctxt->method->name);
    packet_put_char(authctxt->cn, 0);
    packet_put_cstring(authctxt->cn, password);
    packet_add_padding(authctxt->cn, 64);
    packet_send(authctxt->cn);

    dispatch_set(authctxt->cn, SSH2_MSG_USERAUTH_PASSWD_CHANGEREQ,
        &input_userauth_passwd_changereq);
#endif /* L7_SSHD */

    return 1;
}
/*
 * parse PASSWD_CHANGEREQ, prompt user and send SSH2_MSG_USERAUTH_REQUEST
 */
#ifndef L7_SSHD
void
input_userauth_passwd_changereq(int type, u_int32_t seqnr, void *ctxt)
#else
void
input_userauth_passwd_changereq(int cn, int type, u_int32_t seqnr, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    char *info, *lang, *password = NULL, *retype = NULL;
    char prompt[150];

    debug2("input_userauth_passwd_changereq");

#ifndef L7_SSHD
    if (authctxt == NULL)
        fatal("input_userauth_passwd_changereq: "
            "no authentication context");

    info = packet_get_string(NULL);
    lang = packet_get_string(NULL);
#else
    if (authctxt == NULL)
        fatal_cn(cn, "input_userauth_passwd_changereq: "
            "no authentication context");

    info = packet_get_string(cn, NULL);
    lang = packet_get_string(cn, NULL);
#endif /* L7_SSHD */
    if (strlen(info) > 0)
        logit("%s", info);
    xfree(info);
    xfree(lang);
#ifndef L7_SSHD
    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);
    packet_put_char(1);         /* additional info */
    snprintf(prompt, sizeof(prompt),
#else
    packet_start(cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(cn, authctxt->server_user);
    packet_put_cstring(cn, authctxt->service);
    packet_put_cstring(cn, authctxt->method->name);
    packet_put_char(cn, 1);         /* additional info */
    osapiSnprintf(prompt, sizeof(prompt),
#endif /* L7_SSHD */
        "Enter %.30s@%.128s's old password: ",
        authctxt->server_user, authctxt->host);
    password = read_passphrase(prompt, 0);
#ifndef L7_SSHD
    packet_put_cstring(password);
#else
    packet_put_cstring(cn, password);
#endif /* L7_SSHD */
    memset(password, 0, strlen(password));
    xfree(password);
    password = NULL;
    while (password == NULL) {
#ifndef L7_SSHD
        snprintf(prompt, sizeof(prompt),
#else
        osapiSnprintf(prompt, sizeof(prompt),
#endif /* L7_SSHD */
            "Enter %.30s@%.128s's new password: ",
            authctxt->server_user, authctxt->host);
        password = read_passphrase(prompt, RP_ALLOW_EOF);
        if (password == NULL) {
            /* bail out */
            return;
        }
#ifndef L7_SSHD
        snprintf(prompt, sizeof(prompt),
#else
        osapiSnprintf(prompt, sizeof(prompt),
#endif /* L7_SSHD */
            "Retype %.30s@%.128s's new password: ",
            authctxt->server_user, authctxt->host);
        retype = read_passphrase(prompt, 0);
        if (strcmp(password, retype) != 0) {
            memset(password, 0, strlen(password));
            xfree(password);
            logit("Mismatch; try again, EOF to quit.");
            password = NULL;
        }
        memset(retype, 0, strlen(retype));
        xfree(retype);
    }
#ifndef L7_SSHD
    packet_put_cstring(password);
    memset(password, 0, strlen(password));
    xfree(password);
    packet_add_padding(64);
    packet_send();

    dispatch_set(SSH2_MSG_USERAUTH_PASSWD_CHANGEREQ,
        &input_userauth_passwd_changereq);
#else
    packet_put_cstring(cn, password);
    memset(password, 0, strlen(password));
    xfree(password);
    packet_add_padding(cn, 64);
    packet_send(cn);

    dispatch_set(cn, SSH2_MSG_USERAUTH_PASSWD_CHANGEREQ,
        &input_userauth_passwd_changereq);
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
static int
identity_sign(Identity *id, u_char **sigp, u_int *lenp,
    u_char *data, u_int datalen)
#else
static int
identity_sign(int cn, Identity *id, u_char **sigp, u_int *lenp,
    u_char *data, u_int datalen)
#endif /* L7_SSHD */
{
    Key *prv;
    int ret;

    /* the agent supports this key */
    if (id->ac)
#ifndef L7_SSHD
        return (ssh_agent_sign(id->ac, id->key, sigp, lenp,
#else
        return (ssh_agent_sign(cn, id->ac, id->key, sigp, lenp,
#endif /* L7_SSHD */
            data, datalen));
    /*
     * we have already loaded the private key or
     * the private key is stored in external hardware
     */
    if (id->isprivate || (id->key->flags & KEY_FLAG_EXT))
#ifndef L7_SSHD
        return (key_sign(id->key, sigp, lenp, data, datalen));
#else
        return (key_sign(cn, id->key, sigp, lenp, data, datalen));
#endif /* L7_SSHD */
    /* load the private key from the file */
    if ((prv = load_identity_file(id->filename)) == NULL)
        return (-1);
#ifndef L7_SSHD
    ret = key_sign(prv, sigp, lenp, data, datalen);
#else
    ret = key_sign(cn, prv, sigp, lenp, data, datalen);
#endif /* L7_SSHD */
    key_free(prv);
    return (ret);
}

static int
sign_and_send_pubkey(Authctxt *authctxt, Identity *id)
{
    Buffer b;
    u_char *blob, *signature;
    u_int bloblen, slen;
    u_int skip = 0;
    int ret = -1;
    int have_sig = 1;

    debug3("sign_and_send_pubkey");

    if (key_to_blob(id->key, &blob, &bloblen) == 0) {
        /* we cannot handle this key */
        debug3("sign_and_send_pubkey: cannot handle key");
        return 0;
    }
    /* data to be signed */
    buffer_init(&b);
#ifndef L7_SSHD
    if (datafellows & SSH_OLD_SESSIONID) {
        buffer_append(&b, session_id2, session_id2_len);
        skip = session_id2_len;
#else
    if (datafellows[authctxt->cn] & SSH_OLD_SESSIONID) {
        buffer_append(&b, sshc_session_id2[authctxt->cn], sshc_session_id2_len[authctxt->cn]);
        skip = sshc_session_id2_len[authctxt->cn];
#endif /* L7_SSHD */
    } else {
#ifndef L7_SSHD
        buffer_put_string(&b, session_id2, session_id2_len);
#else
        buffer_put_string(&b, sshc_session_id2[authctxt->cn], sshc_session_id2_len[authctxt->cn]);
#endif /* L7_SSHD */
        skip = buffer_len(&b);
    }
    buffer_put_char(&b, SSH2_MSG_USERAUTH_REQUEST);
    buffer_put_cstring(&b, authctxt->server_user);
    buffer_put_cstring(&b,
#ifndef L7_SSHD
        datafellows & SSH_BUG_PKSERVICE ?
#else
        datafellows[authctxt->cn] & SSH_BUG_PKSERVICE ?
#endif /* L7_SSHD */
        "ssh-userauth" :
        authctxt->service);
#ifndef L7_SSHD
    if (datafellows & SSH_BUG_PKAUTH) {
#else
    if (datafellows[authctxt->cn] & SSH_BUG_PKAUTH) {
#endif /* L7_SSHD */
        buffer_put_char(&b, have_sig);
    } else {
        buffer_put_cstring(&b, authctxt->method->name);
        buffer_put_char(&b, have_sig);
        buffer_put_cstring(&b, key_ssh_name(id->key));
    }
    buffer_put_string(&b, blob, bloblen);

    /* generate signature */
#ifndef L7_SSHD
    ret = identity_sign(id, &signature, &slen,
#else
    ret = identity_sign(authctxt->cn, id, &signature, &slen,
#endif /* L7_SSHD */
        buffer_ptr(&b), buffer_len(&b));
    if (ret == -1) {
        xfree(blob);
        buffer_free(&b);
        return 0;
    }
#ifndef L7_SSHD
#ifdef DEBUG_PK
    buffer_dump(&b);
#endif
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&b);
    }
#endif /* L7_SSHD */
#ifndef L7_SSHD
    if (datafellows & SSH_BUG_PKSERVICE) {
#else
    if (datafellows[authctxt->cn] & SSH_BUG_PKSERVICE) {
#endif /* L7_SSHD */
        buffer_clear(&b);
#ifndef L7_SSHD
        buffer_append(&b, session_id2, session_id2_len);
        skip = session_id2_len;
#else
        buffer_append(&b, sshc_session_id2[authctxt->cn], sshc_session_id2_len[authctxt->cn]);
        skip = sshc_session_id2_len[authctxt->cn];
#endif /* L7_SSHD */
        buffer_put_char(&b, SSH2_MSG_USERAUTH_REQUEST);
        buffer_put_cstring(&b, authctxt->server_user);
        buffer_put_cstring(&b, authctxt->service);
        buffer_put_cstring(&b, authctxt->method->name);
        buffer_put_char(&b, have_sig);
#ifndef L7_SSHD
        if (!(datafellows & SSH_BUG_PKAUTH))
#else
        if (!(datafellows[authctxt->cn] & SSH_BUG_PKAUTH))
#endif /* L7_SSHD */
            buffer_put_cstring(&b, key_ssh_name(id->key));
        buffer_put_string(&b, blob, bloblen);
    }
    xfree(blob);

    /* append signature */
    buffer_put_string(&b, signature, slen);
    xfree(signature);

    /* skip session id and packet type */
    if (buffer_len(&b) < skip + 1)
#ifndef L7_SSHD
        fatal("userauth_pubkey: internal error");
#else
        fatal_cn(authctxt->cn, "userauth_pubkey: internal error");
#endif /* L7_SSHD */
    buffer_consume(&b, skip + 1);

    /* put remaining data from buffer into packet */
#ifndef L7_SSHD
    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_raw(buffer_ptr(&b), buffer_len(&b));
    buffer_free(&b);
    packet_send();
#else
    packet_start(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_raw(authctxt->cn, buffer_ptr(&b), buffer_len(&b));
    buffer_free(&b);
    packet_send(authctxt->cn);
#endif /* L7_SSHD */

    return 1;
}

static int
send_pubkey_test(Authctxt *authctxt, Identity *id)
{
    u_char *blob;
    u_int bloblen, have_sig = 0;

    debug3("send_pubkey_test");

    if (key_to_blob(id->key, &blob, &bloblen) == 0) {
        /* we cannot handle this key */
        debug3("send_pubkey_test: cannot handle key");
        return 0;
    }
#ifndef L7_SSHD
    /* register callback for USERAUTH_PK_OK message */
    dispatch_set(SSH2_MSG_USERAUTH_PK_OK, &input_userauth_pk_ok);

    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);
    packet_put_char(have_sig);
    if (!(datafellows & SSH_BUG_PKAUTH))
        packet_put_cstring(key_ssh_name(id->key));
    packet_put_string(blob, bloblen);
    xfree(blob);
    packet_send();
#else
    /* register callback for USERAUTH_PK_OK message */
    dispatch_set(authctxt->cn, SSH2_MSG_USERAUTH_PK_OK, &input_userauth_pk_ok);

    packet_start(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->cn, authctxt->server_user);
    packet_put_cstring(authctxt->cn, authctxt->service);
    packet_put_cstring(authctxt->cn, authctxt->method->name);
    packet_put_char(authctxt->cn, have_sig);
    if (!(datafellows[authctxt->cn] & SSH_BUG_PKAUTH))
        packet_put_cstring(authctxt->cn, key_ssh_name(id->key));
    packet_put_string(authctxt->cn, blob, bloblen);
    xfree(blob);
    packet_send(authctxt->cn);
#endif /* L7_SSHD */
    return 1;
}

static Key *
load_identity_file(char *filename)
{
    Key *private;
    char prompt[300], *passphrase;
    int quit, i;
    struct stat st;

    if (stat(filename, &st) < 0) {
        debug3("no such identity: %s", filename);
        return NULL;
    }
    private = key_load_private_type(KEY_UNSPEC, filename, "", NULL);
    if (private == NULL) {
#ifndef L7_SSHD
        if (options.batch_mode)
            return NULL;
        snprintf(prompt, sizeof prompt,
#else
        if (client_options.batch_mode)
            return NULL;
        osapiSnprintf(prompt, sizeof prompt,
#endif /* L7_SSHD */
            "Enter passphrase for key '%.100s': ", filename);
#ifndef L7_SSHD
        for (i = 0; i < options.number_of_password_prompts; i++) {
#else
        for (i = 0; i < client_options.number_of_password_prompts; i++) {
#endif /* L7_SSHD */
            passphrase = read_passphrase(prompt, 0);
            if (strcmp(passphrase, "") != 0) {
                private = key_load_private_type(KEY_UNSPEC, filename,
                    passphrase, NULL);
                quit = 0;
            } else {
                debug2("no passphrase given, try next key");
                quit = 1;
            }
            memset(passphrase, 0, strlen(passphrase));
            xfree(passphrase);
            if (private != NULL || quit)
                break;
            debug2("bad passphrase given, try again...");
        }
    }
    return private;
}

/*
 * try keys in the following order:
 *  1. agent keys that are found in the config file
 *  2. other agent keys
 *  3. keys that are only listed in the config file
 */
static void
pubkey_prepare(Authctxt *authctxt)
{
    Identity *id;
    Idlist agent, files, *preferred;
    Key *key;
    AuthenticationConnection *ac;
    char *comment;
    int i, found;

    TAILQ_INIT(&agent); /* keys from the agent */
    TAILQ_INIT(&files); /* keys from the config file */
    preferred = &authctxt->keys;
    TAILQ_INIT(preferred);  /* preferred order of keys */

    /* list of keys stored in the filesystem */
#ifndef L7_SSHD
    for (i = 0; i < options.num_identity_files; i++) {
        key = options.identity_keys[i];
        if (key && key->type == KEY_RSA1)
            continue;
        options.identity_keys[i] = NULL;
        id = xmalloc(sizeof(*id));
        memset(id, 0, sizeof(*id));
        id->key = key;
        id->filename = xstrdup(options.identity_files[i]);
        TAILQ_INSERT_TAIL(&files, id, next);
    }
#else
    for (i = 0; i < client_options.num_identity_files; i++) {
        key = client_options.identity_keys[i];
        if (key && key->type == KEY_RSA1)
            continue;
        client_options.identity_keys[i] = NULL;
        id = xmalloc(sizeof(*id));
        memset(id, 0, sizeof(*id));
        id->key = key;
        id->filename = xstrdup(client_options.identity_files[i]);
        TAILQ_INSERT_TAIL(&files, id, next);
    }
#endif /* L7_SSHD */
    /* list of keys supported by the agent */
    if ((ac = ssh_get_authentication_connection())) {
#ifndef L7_SSHD
        for (key = ssh_get_first_identity(ac, &comment, 2);
            key != NULL;
            key = ssh_get_next_identity(ac, &comment, 2)) {
#else
        for (key = ssh_get_first_identity(authctxt->cn, ac, &comment, 2);
            key != NULL;
            key = ssh_get_next_identity(authctxt->cn, ac, &comment, 2)) {
#endif /* L7_SSHD */
            found = 0;
            TAILQ_FOREACH(id, &files, next) {
                /* agent keys from the config file are preferred */
                if (key_equal(key, id->key)) {
                    key_free(key);
                    xfree(comment);
                    TAILQ_REMOVE(&files, id, next);
                    TAILQ_INSERT_TAIL(preferred, id, next);
                    id->ac = ac;
                    found = 1;
                    break;
                }
            }
#ifndef L7_SSHD
            if (!found && !options.identities_only) {
#else
            if (!found && !client_options.identities_only) {
#endif /* L7_SSHD */
                id = xmalloc(sizeof(*id));
                memset(id, 0, sizeof(*id));
                id->key = key;
                id->filename = comment;
                id->ac = ac;
                TAILQ_INSERT_TAIL(&agent, id, next);
            }
        }
        /* append remaining agent keys */
        for (id = TAILQ_FIRST(&agent); id; id = TAILQ_FIRST(&agent)) {
            TAILQ_REMOVE(&agent, id, next);
            TAILQ_INSERT_TAIL(preferred, id, next);
        }
        authctxt->agent = ac;
    }
    /* append remaining keys from the config file */
    for (id = TAILQ_FIRST(&files); id; id = TAILQ_FIRST(&files)) {
        TAILQ_REMOVE(&files, id, next);
        TAILQ_INSERT_TAIL(preferred, id, next);
    }
    TAILQ_FOREACH(id, preferred, next) {
        debug2("key: %s (%p)", id->filename, (void *)id->key);
    }
}

static void
pubkey_cleanup(Authctxt *authctxt)
{
    Identity *id;

    if (authctxt->agent != NULL)
        ssh_close_authentication_connection(authctxt->agent);
    for (id = TAILQ_FIRST(&authctxt->keys); id;
        id = TAILQ_FIRST(&authctxt->keys)) {
        TAILQ_REMOVE(&authctxt->keys, id, next);
        if (id->key)
            key_free(id->key);
        if (id->filename)
            xfree(id->filename);
        xfree(id);
    }
}

int
userauth_pubkey(Authctxt *authctxt)
{
    Identity *id;
    int sent = 0;

    while ((id = TAILQ_FIRST(&authctxt->keys))) {
        if (id->tried++)
            return (0);
        /* move key to the end of the queue */
        TAILQ_REMOVE(&authctxt->keys, id, next);
        TAILQ_INSERT_TAIL(&authctxt->keys, id, next);
        /*
         * send a test message if we have the public key. for
         * encrypted keys we cannot do this and have to load the
         * private key instead
         */
        if (id->key && id->key->type != KEY_RSA1) {
            debug("Offering public key: %s", id->filename);
            sent = send_pubkey_test(authctxt, id);
        } else if (id->key == NULL) {
            debug("Trying private key: %s", id->filename);
            id->key = load_identity_file(id->filename);
            if (id->key != NULL) {
                id->isprivate = 1;
                sent = sign_and_send_pubkey(authctxt, id);
                key_free(id->key);
                id->key = NULL;
            }
        }
        if (sent)
            return (sent);
    }
    return (0);
}

/*
 * Send userauth request message specifying keyboard-interactive method.
 */
int
userauth_kbdint(Authctxt *authctxt)
{
    static int attempt = 0;

#ifndef L7_SSHD
    if (attempt++ >= options.number_of_password_prompts)
        return 0;
#else
    if (attempt++ >= client_options.number_of_password_prompts)
        return 0;
#endif /* L7_SSHD */
    /* disable if no SSH2_MSG_USERAUTH_INFO_REQUEST has been seen */
    if (attempt > 1 && !authctxt->info_req_seen) {
        debug3("userauth_kbdint: disable: no info_req_seen");
#ifndef L7_SSHD
        dispatch_set(SSH2_MSG_USERAUTH_INFO_REQUEST, NULL);
#else
        dispatch_set(authctxt->cn, SSH2_MSG_USERAUTH_INFO_REQUEST, NULL);
#endif /* L7_SSHD */
        return 0;
    }

    debug2("userauth_kbdint");
#ifndef L7_SSHD
    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);
    packet_put_cstring("");                 /* lang */
    packet_put_cstring(options.kbd_interactive_devices ?
        options.kbd_interactive_devices : "");
    packet_send();

    dispatch_set(SSH2_MSG_USERAUTH_INFO_REQUEST, &input_userauth_info_req);
#else
    packet_start(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->cn, authctxt->server_user);
    packet_put_cstring(authctxt->cn, authctxt->service);
    packet_put_cstring(authctxt->cn, authctxt->method->name);
    packet_put_cstring(authctxt->cn, "");                 /* lang */
    packet_put_cstring(authctxt->cn, client_options.kbd_interactive_devices ?
        client_options.kbd_interactive_devices : "");
    packet_send(authctxt->cn);

    dispatch_set(authctxt->cn, SSH2_MSG_USERAUTH_INFO_REQUEST, &input_userauth_info_req);
#endif /* L7_SSHD */
    return 1;
}

/*
 * parse INFO_REQUEST, prompt user and send INFO_RESPONSE
 */
#ifndef L7_SSHD
void
input_userauth_info_req(int type, u_int32_t seq, void *ctxt)
#else
void
input_userauth_info_req(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    char *name, *inst, *lang, *prompt, *response;
    u_int num_prompts, i;
    int echo = 0;

    debug2("input_userauth_info_req");

#ifndef L7_SSHD
    if (authctxt == NULL)
        fatal("input_userauth_info_req: no authentication context");
#else
    if (authctxt == NULL)
        fatal_cn(cn, "input_userauth_info_req: no authentication context");
#endif /* L7_SSHD */

    authctxt->info_req_seen = 1;

#ifndef L7_SSHD
    name = packet_get_string(NULL);
    inst = packet_get_string(NULL);
    lang = packet_get_string(NULL);
#else
    name = packet_get_string(cn, NULL);
    inst = packet_get_string(cn, NULL);
    lang = packet_get_string(cn, NULL);
#endif /* L7_SSHD */
    if (strlen(name) > 0)
        logit("%s", name);
    if (strlen(inst) > 0)
        logit("%s", inst);
    xfree(name);
    xfree(inst);
    xfree(lang);

#ifndef L7_SSHD
    num_prompts = packet_get_int();
    /*
     * Begin to build info response packet based on prompts requested.
     * We commit to providing the correct number of responses, so if
     * further on we run into a problem that prevents this, we have to
     * be sure and clean this up and send a correct error response.
     */
    packet_start(SSH2_MSG_USERAUTH_INFO_RESPONSE);
    packet_put_int(num_prompts);
#else
    num_prompts = packet_get_int(cn);
    /*
     * Begin to build info response packet based on prompts requested.
     * We commit to providing the correct number of responses, so if
     * further on we run into a problem that prevents this, we have to
     * be sure and clean this up and send a correct error response.
     */
    packet_start(cn, SSH2_MSG_USERAUTH_INFO_RESPONSE);
    packet_put_int(cn, num_prompts);
#endif /* L7_SSHD */

    debug2("input_userauth_info_req: num_prompts %d", num_prompts);
    for (i = 0; i < num_prompts; i++) {
#ifndef L7_SSHD
        prompt = packet_get_string(NULL);
        echo = packet_get_char();

        response = read_passphrase(prompt, echo ? RP_ECHO : 0);

        packet_put_cstring(response);
#else
        prompt = packet_get_string(cn, NULL);
        echo = packet_get_char(cn);

        response = read_passphrase(prompt, echo ? RP_ECHO : 0);

        packet_put_cstring(cn, response);
#endif /* L7_SSHD */
        memset(response, 0, strlen(response));
        xfree(response);
        xfree(prompt);
    }
#ifndef L7_SSHD
    packet_check_eom(); /* done with parsing incoming message. */

    packet_add_padding(64);
    packet_send();
#else
    packet_check_eom(cn); /* done with parsing incoming message. */

    packet_add_padding(cn, 64);
    packet_send(cn);
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
static int
ssh_keysign(Key *key, u_char **sigp, u_int *lenp,
    u_char *data, u_int datalen)
{
    Buffer b;
    struct stat st;
    pid_t pid;
    int to[2], from[2], status, version = 2;

    debug2("ssh_keysign called");

    if (stat(_PATH_SSH_KEY_SIGN, &st) < 0) {
        error("ssh_keysign: no installed: %s", strerror(errno));
        return -1;
    }
    if (fflush(stdout) != 0)
        error("ssh_keysign: fflush: %s", strerror(errno));
    if (pipe(to) < 0) {
        error("ssh_keysign: pipe: %s", strerror(errno));
        return -1;
    }
    if (pipe(from) < 0) {
        error("ssh_keysign: pipe: %s", strerror(errno));
        return -1;
    }
    if ((pid = fork()) < 0) {
        error("ssh_keysign: fork: %s", strerror(errno));
        return -1;
    }
    if (pid == 0) {
        seteuid(getuid());
        setuid(getuid());
        close(from[0]);
        if (dup2(from[1], STDOUT_FILENO) < 0)
            fatal("ssh_keysign: dup2: %s", strerror(errno));
        close(to[1]);
        if (dup2(to[0], STDIN_FILENO) < 0)
            fatal("ssh_keysign: dup2: %s", strerror(errno));
        close(from[1]);
        close(to[0]);
        execl(_PATH_SSH_KEY_SIGN, _PATH_SSH_KEY_SIGN, (char *) 0);
        fatal("ssh_keysign: exec(%s): %s", _PATH_SSH_KEY_SIGN,
            strerror(errno));
    }
    close(from[1]);
    close(to[0]);

    buffer_init(&b);
    buffer_put_int(&b, packet_get_connection_in()); /* send # of socket */
    buffer_put_string(&b, data, datalen);
    if (ssh_msg_send(to[1], version, &b) == -1)
        fatal("ssh_keysign: couldn't send request");

    if (ssh_msg_recv(from[0], &b) < 0) {
        error("ssh_keysign: no reply");
        buffer_free(&b);
        return -1;
    }
    close(from[0]);
    close(to[1]);

    while (waitpid(pid, &status, 0) < 0)
        if (errno != EINTR)
            break;

    if (buffer_get_char(&b) != version) {
        error("ssh_keysign: bad version");
        buffer_free(&b);
        return -1;
    }
    *sigp = buffer_get_string(&b, lenp);
    buffer_free(&b);

    return 0;
}
#endif /* L7_SSHD */

int
userauth_hostbased(Authctxt *authctxt)
{
    Key *private = NULL;
    Sensitive *sensitive = authctxt->sensitive;
    Buffer b;
    u_char *signature, *blob;
    char *chost, *pkalg, *p;
    const char *service;
    u_int blen, slen;
    int ok, i, len, found = 0;

    /* check for a useful key */
    for (i = 0; i < sensitive->nkeys; i++) {
        private = sensitive->keys[i];
        if (private && private->type != KEY_RSA1) {
            found = 1;
            /* we take and free the key */
            sensitive->keys[i] = NULL;
            break;
        }
    }
    if (!found) {
        debug("No more client hostkeys for hostbased authentication.");
        return 0;
    }
    if (key_to_blob(private, &blob, &blen) == 0) {
        key_free(private);
        return 0;
    }
    /* figure out a name for the client host */
#ifndef L7_SSHD
    p = get_local_name(packet_get_connection_in());
#else
    p = get_local_name(packet_get_connection_in(authctxt->cn));
#endif /* L7_SSHD */
    if (p == NULL) {
        error("userauth_hostbased: cannot get local ipaddr/name");
        key_free(private);
        return 0;
    }
    len = strlen(p) + 2;
    chost = xmalloc(len);
    strlcpy(chost, p, len);
    strlcat(chost, ".", len);
    debug2("userauth_hostbased: chost %s", chost);
    xfree(p);

#ifndef L7_SSHD
    service = datafellows & SSH_BUG_HBSERVICE ? "ssh-userauth" :
#else
    service = datafellows[authctxt->cn] & SSH_BUG_HBSERVICE ? "ssh-userauth" :
#endif /* L7_SSHD */
        authctxt->service;
    pkalg = xstrdup(key_ssh_name(private));
    buffer_init(&b);
    /* construct data */
#ifndef L7_SSHD
    buffer_put_string(&b, session_id2, session_id2_len);
#else
    buffer_put_string(&b, sshc_session_id2[authctxt->cn], sshc_session_id2_len[authctxt->cn]);
#endif /* L7_SSHD */
    buffer_put_char(&b, SSH2_MSG_USERAUTH_REQUEST);
    buffer_put_cstring(&b, authctxt->server_user);
    buffer_put_cstring(&b, service);
    buffer_put_cstring(&b, authctxt->method->name);
    buffer_put_cstring(&b, pkalg);
    buffer_put_string(&b, blob, blen);
    buffer_put_cstring(&b, chost);
    buffer_put_cstring(&b, authctxt->local_user);
#ifndef L7_SSHD
#ifdef DEBUG_PK
    buffer_dump(&b);
#endif
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&b);
    }
#endif /* L7_SSHD */
    if (sensitive->external_keysign)
#ifndef L7_SSHD
        ok = ssh_keysign(private, &signature, &slen,
            buffer_ptr(&b), buffer_len(&b));
#else
    {
        ok = key_sign(authctxt->cn, private, &signature, &slen,
            buffer_ptr(&b), buffer_len(&b));
    }
#endif /* L7_SSHD */
    else
#ifndef L7_SSHD
        ok = key_sign(private, &signature, &slen,
            buffer_ptr(&b), buffer_len(&b));
#else
    {
        ok = key_sign(authctxt->cn, private, &signature, &slen,
            buffer_ptr(&b), buffer_len(&b));
    }
#endif /* L7_SSHD */
    key_free(private);
    buffer_free(&b);
    if (ok != 0) {
        error("key_sign failed");
        xfree(chost);
        xfree(pkalg);
        return 0;
    }
#ifndef L7_SSHD
    packet_start(SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->server_user);
    packet_put_cstring(authctxt->service);
    packet_put_cstring(authctxt->method->name);
    packet_put_cstring(pkalg);
    packet_put_string(blob, blen);
    packet_put_cstring(chost);
    packet_put_cstring(authctxt->local_user);
    packet_put_string(signature, slen);
#else
    packet_start(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST);
    packet_put_cstring(authctxt->cn, authctxt->server_user);
    packet_put_cstring(authctxt->cn, authctxt->service);
    packet_put_cstring(authctxt->cn, authctxt->method->name);
    packet_put_cstring(authctxt->cn, pkalg);
    packet_put_string(authctxt->cn, blob, blen);
    packet_put_cstring(authctxt->cn, chost);
    packet_put_cstring(authctxt->cn, authctxt->local_user);
    packet_put_string(authctxt->cn, signature, slen);
#endif /* L7_SSHD */
    memset(signature, 's', slen);
    xfree(signature);
    xfree(chost);
    xfree(pkalg);

#ifndef L7_SSHD
    packet_send();
#else
    packet_send(authctxt->cn);
#endif /* L7_SSHD */
    return 1;
}

/* find auth method */

/*
 * given auth method name, if configurable options permit this method fill
 * in auth_ident field and return true, otherwise return false.
 */
static int
authmethod_is_enabled(Authmethod *method)
{
    if (method == NULL)
        return 0;
    /* return false if options indicate this method is disabled */
    if  (method->enabled == NULL || *method->enabled == 0)
        return 0;
    /* return false if batch mode is enabled but method needs interactive mode */
    if  (method->batch_flag != NULL && *method->batch_flag != 0)
        return 0;
    return 1;
}

static Authmethod *
authmethod_lookup(const char *name)
{
    Authmethod *method = NULL;
    if (name != NULL)
#ifndef L7_SSHD
        for (method = authmethods; method->name != NULL; method++)
#else
        for (method = sshc_authmethods; method->name != NULL; method++)
#endif /* L7_SSHD */
            if (strcmp(name, method->name) == 0)
                return method;
    debug2("Unrecognized authentication method name: %s", name ? name : "NULL");
    return NULL;
}
#ifndef L7_SSHD /* RELOCATED TO TOP OF FILE */
/* XXX internal state */
static Authmethod *current = NULL;
static char *supported = NULL;
static char *preferred = NULL;
#endif /* L7_SSHD */
/*
 * Given the authentication method list sent by the server, return the
 * next method we should try.  If the server initially sends a nil list,
 * use a built-in default list.
 */
static Authmethod *
authmethod_get(char *authlist)
{
    char *name = NULL;
    u_int next;

    /* Use a suitable default if we're passed a nil list.  */
    if (authlist == NULL || strlen(authlist) == 0)
#ifndef L7_SSHD
        authlist = options.preferred_authentications;
#else
        authlist = client_options.preferred_authentications;
#endif /* L7_SSHD */

    if (supported == NULL || strcmp(authlist, supported) != 0) {
        debug3("start over, passed a different list %s", authlist);
        if (supported != NULL)
            xfree(supported);
        supported = xstrdup(authlist);
#ifndef L7_SSHD
        preferred = options.preferred_authentications;
#else
        preferred = client_options.preferred_authentications;
#endif /* L7_SSHD */
        debug3("preferred %s", preferred);
        current = NULL;
    } else if (current != NULL && authmethod_is_enabled(current))
        return current;

    for (;;) {
        if ((name = match_list(preferred, supported, &next)) == NULL) {
            debug("No more authentication methods to try.");
            current = NULL;
            return NULL;
        }
        preferred += next;
        debug3("authmethod_lookup %s", name);
        debug3("remaining preferred: %s", preferred);
        if ((current = authmethod_lookup(name)) != NULL &&
            authmethod_is_enabled(current)) {
            debug3("authmethod_is_enabled %s", name);
            debug("Next authentication method: %s", name);
#ifdef L7_SSHD
            if (name != NULL)
            {
              xfree(name);
              name = NULL;
            }
#endif /* L7_SSHD */
            return current;
        }
    }
}

static char *
authmethods_get(void)
{
    Authmethod *method = NULL;
    Buffer b;
    char *list;

    buffer_init(&b);
#ifndef L7_SSHD
    for (method = authmethods; method->name != NULL; method++) {
#else
    for (method = sshc_authmethods; method->name != NULL; method++) {
#endif /* L7_SSHD */
        if (authmethod_is_enabled(method)) {
            if (buffer_len(&b) > 0)
                buffer_append(&b, ",", 1);
            buffer_append(&b, method->name, strlen(method->name));
        }
    }
    buffer_append(&b, "\0", 1);
    list = xstrdup(buffer_ptr(&b));
    buffer_free(&b);
    return list;
}
