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
RCSID("$OpenBSD: auth2.c,v 1.107 2004/07/28 09:40:29 markus Exp $");

#include "ssh2.h"
#include "xmalloc.h"
#include "packet.h"
#include "log.h"
#include "servconf.h"
#include "compat.h"
#include "auth.h"
#include "dispatch.h"
#include "pathnames.h"
#include "monitor_wrap.h"
#include "buffer.h"

#ifdef GSSAPI
#include "ssh-gss.h"
#endif

#ifdef L7_SSHD
#include "user_manager_exports.h"
#include "user_mgr_api.h"
#include "sshd_api.h"
#endif
/* import */
extern ServerOptions options;
#ifndef L7_SSHD
extern u_char *session_id2;
extern u_int session_id2_len;
extern Buffer loginmsg;
#else
extern u_char *session_id2[];
extern u_int session_id2_len[];
extern Buffer loginmsg[];
#endif /* L7_SSHD */

/* methods */

extern Authmethod method_none;
extern Authmethod method_pubkey;
extern Authmethod method_passwd;
extern Authmethod method_kbdint;
extern Authmethod method_hostbased;
#ifdef GSSAPI
extern Authmethod method_gssapi;
#endif

Authmethod *authmethods[] = {
    &method_none,
    &method_pubkey,
#ifdef GSSAPI
    &method_gssapi,
#endif
    &method_passwd,
    &method_kbdint,
    &method_hostbased,
    NULL
};

/* protocol */

#ifndef L7_SSHD
static void input_service_request(int, u_int32_t, void *);
static void input_userauth_request(int, u_int32_t, void *);
#else
static void input_service_request(int, int, u_int32_t, void *);
static void input_userauth_request(int, int, u_int32_t, void *);
#endif /* L7_SSHD */

/* helper */
static Authmethod *authmethod_lookup(const char *);
static char *authmethods_get(void);
#ifndef L7_SSHD
int user_key_allowed(struct passwd *, Key *);
#else
int user_key_allowed(int, struct passwd *, Key *);
#endif /* L7_SSHD */

/*
 * loop until authctxt->success == TRUE
 */

void
do_authentication2(Authctxt *authctxt)
{
    /* challenge-response is implemented via keyboard interactive */
    if (options.challenge_response_authentication)
        options.kbd_interactive_authentication = 1;

#ifndef L7_SSHD
    dispatch_init(&dispatch_protocol_error);
    dispatch_set(SSH2_MSG_SERVICE_REQUEST, &input_service_request);
    dispatch_run(DISPATCH_BLOCK, &authctxt->success, authctxt);
#else
    dispatch_init(authctxt->cn, &dispatch_protocol_error);
    dispatch_set(authctxt->cn, SSH2_MSG_SERVICE_REQUEST, &input_service_request);
    dispatch_run(authctxt->cn, DISPATCH_BLOCK, &authctxt->authdone, authctxt);
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
static void
input_service_request(int type, u_int32_t seq, void *ctxt)
#else
static void
input_service_request(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    u_int len;
    int acceptit = 0;
#ifndef L7_SSHD
    char *service = packet_get_string(&len);
    packet_check_eom();

    if (authctxt == NULL)
        fatal("input_service_request: no authctxt");
#else
    char *service = packet_get_string(cn, &len);
    packet_check_eom(cn);

    if (authctxt == NULL)
        fatal_cn(cn, "input_service_request: no authctxt");
#endif /* L7_SSHD */

    if (strcmp(service, "ssh-userauth") == 0) {
        if (!authctxt->success) {
            acceptit = 1;
            /* now we can handle user-auth requests */
#ifndef L7_SSHD
            dispatch_set(SSH2_MSG_USERAUTH_REQUEST, &input_userauth_request);
#else
            dispatch_set(cn, SSH2_MSG_USERAUTH_REQUEST, &input_userauth_request);
#endif /* L7_SSHD */
        }
    }
    /* XXX all other service requests are denied */

#ifndef L7_SSHD
    if (acceptit) {
        packet_start(SSH2_MSG_SERVICE_ACCEPT);
        packet_put_cstring(service);
        packet_send();
        packet_write_wait();
    } else {
        debug("bad service request %s", service);
        packet_disconnect("bad service request %s", service);
    }
#else
    if (acceptit) {
        packet_start(cn, SSH2_MSG_SERVICE_ACCEPT);
        packet_put_cstring(cn, service);
        packet_send(cn);
        packet_write_wait(cn);
    } else {
        debug("bad service request %s", service);
        packet_disconnect(cn, "bad service request %s", service);
    }
#endif /* L7_SSHD */
    xfree(service);
}

#ifndef L7_SSHD
static void
input_userauth_request(int type, u_int32_t seq, void *ctxt)
#else
static void
input_userauth_request(int cn, int type, u_int32_t seq, void *ctxt)
#endif /* L7_SSHD */
{
    Authctxt *authctxt = ctxt;
    Authmethod *m = NULL;
#ifndef L7_SSHD
    char *user, *service, *method, *style = NULL;
#else
    char *style = NULL;
#endif /* L7_SSHD */
    int authenticated = 0;

#ifndef L7_SSHD
    if (authctxt == NULL)
        fatal("input_userauth_request: no authctxt");

    user = packet_get_string(NULL);
    service = packet_get_string(NULL);
    method = packet_get_string(NULL);
    debug("userauth-request for user %s service %s method %s", user, service, method);
#else
    if (authctxt == NULL)
        fatal_cn(cn, "input_userauth_request: no authctxt");

    authctxt->user = packet_get_string(cn, NULL);
    authctxt->service = packet_get_string(cn, NULL);
    authctxt->method = packet_get_string(cn, NULL);
    debug("userauth-request for user %s service %s method %s", authctxt->user, authctxt->service, authctxt->method);
#endif /* L7_SSHD */
    debug("attempt %d failures %d", authctxt->attempt, authctxt->failures);

#ifndef L7_SSHD
    if ((style = strchr(user, ':')) != NULL)
        *style++ = 0;
#else
    if ((style = strchr(authctxt->user, ':')) != NULL)
        *style++ = 0;
    authctxt->style = style ? xstrdup(style) : NULL;
#endif /* L7_SSHD */

    if (authctxt->attempt++ == 0) {
        /* setup auth context */
#ifndef L7_SSHD
        authctxt->pw = PRIVSEP(getpwnamallow(user));
        authctxt->user = xstrdup(user);
        if (authctxt->pw && strcmp(service, "ssh-connection")==0) {
            authctxt->valid = 1;
            debug2("input_userauth_request: setting up authctxt for %s", user);
        } else {
            logit("input_userauth_request: invalid user %s", user);
            authctxt->pw = fakepw();
#ifdef SSH_AUDIT_EVENTS
            PRIVSEP(audit_event(SSH_INVALID_USER));
#endif
        }
#else
        authctxt->pw = PRIVSEP(getpwnamallow(cn, authctxt->user));
        if (authctxt->pw && strcmp(authctxt->service, "ssh-connection")==0) {
            authctxt->valid = 1;
            debug2("input_userauth_request: setting up authctxt for %s", authctxt->user);
        } else {
            logit("input_userauth_request: invalid user %s", authctxt->user);
            authctxt->pw = fakepw();
#ifdef SSH_AUDIT_EVENTS
            PRIVSEP(audit_event(SSH_INVALID_USER));
#endif
        }
#endif /* L7_SSHD */

#ifdef USE_PAM
        if (options.use_pam)
            PRIVSEP(start_pam(authctxt));
#endif

#ifndef L7_SSHD
        setproctitle("%s%s", authctxt->valid ? user : "unknown",
            use_privsep ? " [net]" : "");
        authctxt->service = xstrdup(service);
        authctxt->style = style ? xstrdup(style) : NULL;
        if (use_privsep)
            mm_inform_authserv(service, style);
    } else if (strcmp(user, authctxt->user) != 0 ||
        strcmp(service, authctxt->service) != 0) {
        packet_disconnect("Change of username or service not allowed: "
            "(%s,%s) -> (%s,%s)",
            authctxt->user, authctxt->service, user, service);
#endif /* L7_SSHD */
    }
    /* reset state */
    auth2_challenge_stop(authctxt);

#ifdef GSSAPI
    dispatch_set(SSH2_MSG_USERAUTH_GSSAPI_TOKEN, NULL);
    dispatch_set(SSH2_MSG_USERAUTH_GSSAPI_EXCHANGE_COMPLETE, NULL);
#endif

    authctxt->postponed = 0;

    /* try to authenticate user */
#ifndef L7_SSHD
    m = authmethod_lookup(method);
    if (m != NULL) {
        debug2("input_userauth_request: try method %s", method);
        authenticated = m->userauth(authctxt);
    }
    userauth_finish(authctxt, authenticated, method);

    xfree(service);
    xfree(user);
    xfree(method);
#else
    m = authmethod_lookup(authctxt->method);
    if (m != NULL) {
        debug2("input_userauth_request: try method %s", authctxt->method);
        authenticated = m->userauth(authctxt);
    }
    userauth_finish(authctxt, authenticated, authctxt->method);

    if (authctxt->service != NULL)
    {
        xfree(authctxt->service);
        authctxt->service = NULL;
    }

    if (authctxt->user != NULL)
    {
        xfree(authctxt->user);
        authctxt->user = NULL;
    }

    if (authctxt->method != NULL)
    {
        xfree(authctxt->method);
        authctxt->method = NULL;
    }
#endif /* L7_SSHD */
}

void
userauth_finish(Authctxt *authctxt, int authenticated, char *method)
{
    char *methods;
#ifdef L7_SSHD
    L7_BOOL  authSuccess = L7_FALSE;
#endif

    if (!authctxt->valid && authenticated)
#ifndef L7_SSHD
        fatal("INTERNAL ERROR: authenticated invalid user %s",
#else
        fatal_cn(authctxt->cn, "INTERNAL ERROR: authenticated invalid user %s",
#endif /* L7_SSHD */
            authctxt->user);

    /* Special handling for root */
#ifndef L7_SSHD
    if (authenticated && authctxt->pw->pw_uid == 0 &&
        !auth_root_allowed(method)) {
#else
    if (authenticated &&
        !auth_root_allowed(authctxt->cn, method)) {
#endif /* L7_SSHD */
        authenticated = 0;
#ifdef SSH_AUDIT_EVENTS
        PRIVSEP(audit_event(SSH_LOGIN_ROOT_DENIED));
#endif
    }

#ifdef USE_PAM
    if (options.use_pam && authenticated) {
        if (!PRIVSEP(do_pam_account())) {
            /* if PAM returned a message, send it to the user */
            if (buffer_len(&loginmsg) > 0) {
                buffer_append(&loginmsg, "\0", 1);
#ifndef L7_SSHD
                userauth_send_banner(buffer_ptr(&loginmsg));
                packet_write_wait();
#else
                userauth_send_banner(authctxt->cn, buffer_ptr(&loginmsg));
                packet_write_wait(authctxt->cn);
#endif /* L7_SSHD */
            }
#ifndef L7_SSHD
            fatal("Access denied for user %s by PAM account "
                "configuration", authctxt->user);
#else
            fatal_cn(authctxt->cn, "Access denied for user %s by PAM account "
                "configuration", authctxt->user);
#endif /* L7_SSHD */
        }
    }
#endif

#ifdef _UNICOS
    if (authenticated && cray_access_denied(authctxt->user)) {
        authenticated = 0;
        fatal("Access denied for user %s.",authctxt->user);
    }
#endif /* _UNICOS */

    /* Log before sending the reply */
    auth_log(authctxt, authenticated, method, " ssh2");

    if (authctxt->postponed)
        return;

    /* XXX todo: check if multiple auth methods are needed */
    if (authenticated == 1) {
        /* turn off userauth */
#ifndef L7_SSHD
        dispatch_set(SSH2_MSG_USERAUTH_REQUEST, &dispatch_protocol_ignore);
        packet_start(SSH2_MSG_USERAUTH_SUCCESS);
        packet_send();
        packet_write_wait();
        authctxt->success = 1;
#else

        if ( (strcmp (method, "publickey") == 0)  &&
             (userMgrIsAuthenticationRequired(ACCESS_LINE_SSH, ACCESS_LEVEL_LOGIN) == L7_TRUE))
        {
          debug("public key auth success; password auth to follow\n");
          packet_start(authctxt->cn, SSH2_MSG_USERAUTH_FAILURE);
          packet_put_cstring(authctxt->cn, "password");
          packet_put_char(authctxt->cn, 1); /* XXX partial success */
        }
        else
        {
          debug("public key auth success;\n");
          dispatch_set(authctxt->cn, SSH2_MSG_USERAUTH_REQUEST, &dispatch_protocol_ignore);
          packet_start(authctxt->cn, SSH2_MSG_USERAUTH_SUCCESS);
          authSuccess = L7_TRUE;
        }

        packet_send(authctxt->cn);
        packet_write_wait(authctxt->cn);
        if ( authSuccess == L7_TRUE )
        {
        /* now we can break out */
        authctxt->success = 1;
        authctxt->authdone = 1;
        }
#endif /* L7_SSHD */
    } else {
        if (authctxt->failures++ > options.max_authtries) {
#ifdef SSH_AUDIT_EVENTS
            PRIVSEP(audit_event(SSH_LOGIN_EXCEED_MAXTRIES));
#endif
#ifndef L7_SSHD
            packet_disconnect(AUTH_FAIL_MSG, authctxt->user);
#else
            packet_disconnect(authctxt->cn, AUTH_FAIL_MSG, authctxt->user);
#endif /* L7_SSHD */
        }
        methods = authmethods_get();
#ifndef L7_SSHD
        packet_start(SSH2_MSG_USERAUTH_FAILURE);
        packet_put_cstring(methods);
        packet_put_char(0); /* XXX partial success, unused */
        packet_send();
        packet_write_wait();
#else
      {
        L7_uint32 pkmode = L7_DISABLE;
        if ((sshdPubKeyAuthModeGet(&pkmode) == L7_SUCCESS) && (L7_ENABLE == pkmode) &&
            ((strcmp (method, "publickey") == 0)  &&
             (userMgrIsAuthenticationRequired(ACCESS_LINE_SSH, ACCESS_LEVEL_LOGIN) == L7_FALSE)))
        {
          packet_start(authctxt->cn, SSH2_MSG_USERAUTH_FAILURE);
          packet_send(authctxt->cn);
          packet_write_wait(authctxt->cn);
          packet_disconnect(authctxt->cn, AUTH_FAIL_MSG, authctxt->user);
        }
        else
        {
          packet_start(authctxt->cn, SSH2_MSG_USERAUTH_FAILURE);
          packet_put_cstring(authctxt->cn, methods);
          packet_put_char(authctxt->cn, 0); /* XXX partial success, unused */
          packet_send(authctxt->cn);
          packet_write_wait(authctxt->cn);
        }
      }
#endif /* L7_SSHD */
        xfree(methods);
    }
}

#define DELIM   ","

static char *
authmethods_get(void)
{
    Buffer b;
    char *list;
    int i;

    buffer_init(&b);
    for (i = 0; authmethods[i] != NULL; i++) {
        if (strcmp(authmethods[i]->name, "none") == 0)
            continue;
        if (authmethods[i]->enabled != NULL &&
            *(authmethods[i]->enabled) != 0) {
            if (buffer_len(&b) > 0)
                buffer_append(&b, ",", 1);
            buffer_append(&b, authmethods[i]->name,
                strlen(authmethods[i]->name));
        }
    }
    buffer_append(&b, "\0", 1);
    list = xstrdup(buffer_ptr(&b));
    buffer_free(&b);
    return list;
}

static Authmethod *
authmethod_lookup(const char *name)
{
    int i;

#ifndef L7_SSHD
    if (name != NULL)
        for (i = 0; authmethods[i] != NULL; i++)
            if (authmethods[i]->enabled != NULL &&
                *(authmethods[i]->enabled) != 0 &&
                strcmp(name, authmethods[i]->name) == 0)
                return authmethods[i];
    debug2("Unrecognized authentication method name: %s",
        name ? name : "NULL");
#else
    if (name != NULL)
    {
        for (i = 0; authmethods[i] != NULL; i++)
        {
            debugl7(SYSLOG_LEVEL_DEBUG3, "method[%d]: \"%s\", enabled %d", i, authmethods[i]->name,
                    (authmethods[i]->enabled != NULL ? *(authmethods[i]->enabled) : 0));
            if (authmethods[i]->enabled != NULL &&
                *(authmethods[i]->enabled) != 0 &&
                strcmp(name, authmethods[i]->name) == 0)
            {
                return authmethods[i];
            }
        }
    }
    debugl7(SYSLOG_LEVEL_DEBUG2, "Unrecognized authentication method name: \"%s\"",
        name ? name : "NULL");
#endif /* L7_SSHD */
    return NULL;
}
