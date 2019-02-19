/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Rhosts or /etc/hosts.equiv authentication combined with RSA host
 * authentication.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: auth-rh-rsa.c,v 1.38 2005/07/17 07:17:54 djm Exp $");

#include "packet.h"
#include "uidswap.h"
#include "log.h"
#include "servconf.h"
#include "key.h"
#include "hostfile.h"
#include "pathnames.h"
#include "auth.h"
#include "canohost.h"

#include "monitor_wrap.h"

/* import */
extern ServerOptions options;

#ifndef L7_SSHD
int
auth_rhosts_rsa_key_allowed(struct passwd *pw, char *cuser, char *chost,
    Key *client_host_key)
#else
int
auth_rhosts_rsa_key_allowed(int cn, struct passwd *pw, char *cuser, char *chost,
    Key *client_host_key)
#endif /* L7_SSHD */
{
    HostStatus host_status;

    /* Check if we would accept it using rhosts authentication. */
#ifndef L7_SSHD
    if (!auth_rhosts(pw, cuser))
#else
    if (!auth_rhosts(cn, pw, cuser))
#endif /* L7_SSHD */
        return 0;

#ifndef L7_SSHD
    host_status = check_key_in_hostfiles(pw, client_host_key,
#else
    host_status = check_key_in_hostfiles(cn, pw, client_host_key,
#endif /* L7_SSHD */
        chost, _PATH_SSH_SYSTEM_HOSTFILE,
        options.ignore_user_known_hosts ? NULL : _PATH_SSH_USER_HOSTFILE);

    return (host_status == HOST_OK);
}

/*
 * Tries to authenticate the user using the .rhosts file and the host using
 * its host key.  Returns true if authentication succeeds.
 */
int
auth_rhosts_rsa(Authctxt *authctxt, char *cuser, Key *client_host_key)
{
    char *chost;
    struct passwd *pw = authctxt->pw;

    debug("Trying rhosts with RSA host authentication for client user %.100s",
        cuser);

    if (!authctxt->valid || client_host_key == NULL ||
        client_host_key->rsa == NULL)
        return 0;

#ifndef L7_SSHD
    chost = (char *)get_canonical_hostname(options.use_dns);
#else
    chost = (char *)get_canonical_hostname(authctxt->cn, options.use_dns);
#endif /* L7_SSHD */
    debug("Rhosts RSA authentication: canonical host %.900s", chost);

#ifndef L7_SSHD
    if (!PRIVSEP(auth_rhosts_rsa_key_allowed(pw, cuser, chost, client_host_key))) {
#else
    if (!PRIVSEP(auth_rhosts_rsa_key_allowed(authctxt->cn, pw, cuser, chost, client_host_key))) {
#endif /* L7_SSHD */
        debug("Rhosts with RSA host authentication denied: unknown or invalid host key");
#ifndef L7_SSHD
        packet_send_debug("Your host key cannot be verified: unknown or invalid host key.");
#else
        packet_send_debug(authctxt->cn, "Your host key cannot be verified: unknown or invalid host key.");
#endif /* L7_SSHD */
        return 0;
    }
    /* A matching host key was found and is known. */

    /* Perform the challenge-response dialog with the client for the host key. */
#ifndef L7_SSHD
    if (!auth_rsa_challenge_dialog(client_host_key)) {
#else
    if (!auth_rsa_challenge_dialog(authctxt->cn, client_host_key)) {
#endif /* L7_SSHD */
        logit("Client on %.800s failed to respond correctly to host authentication.",
            chost);
        return 0;
    }
    /*
     * We have authenticated the user using .rhosts or /etc/hosts.equiv,
     * and the host using RSA. We accept the authentication.
     */

#ifndef L7_SSHD
    verbose("Rhosts with RSA host authentication accepted for %.100s, %.100s on %.700s.",
        pw->pw_name, cuser, chost);
    packet_send_debug("Rhosts with RSA host authentication accepted.");
#else
    verbose("Rhosts with RSA host authentication accepted for %.100s, %.100s on %.700s.",
        pw->username, cuser, chost);
    packet_send_debug(authctxt->cn, "Rhosts with RSA host authentication accepted.");
#endif /* L7_SSHD */
    return 1;
}
