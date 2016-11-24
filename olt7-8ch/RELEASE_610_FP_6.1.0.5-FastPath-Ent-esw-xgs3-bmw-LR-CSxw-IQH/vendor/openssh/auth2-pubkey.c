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
RCSID("$OpenBSD: auth2-pubkey.c,v 1.9 2004/12/11 01:48:56 dtucker Exp $");

#include "ssh.h"
#include "ssh2.h"
#include "xmalloc.h"
#include "packet.h"
#include "buffer.h"
#include "log.h"
#include "servconf.h"
#include "compat.h"
#include "bufaux.h"
#include "auth.h"
#include "key.h"
#include "pathnames.h"
#include "uidswap.h"
#include "auth-options.h"
#include "canohost.h"
#include "monitor_wrap.h"
#include "misc.h"

/* import */
extern ServerOptions options;
#ifndef L7_SSHD
extern u_char *session_id2;
extern u_int session_id2_len;
#else
extern u_char *session_id2[];
extern u_int session_id2_len[];
/* callback to application layer to get public key for user if enabled */
extern L7_RC_t sshdUserPubKeyAuthCallback(L7_char8 *user, Key *key);
#endif /* L7_SSHD */

static int
userauth_pubkey(Authctxt *authctxt)
{
    Buffer b;
    Key *key = NULL;
    char *pkalg;
    u_char *pkblob, *sig;
    u_int alen, blen, slen;
    int have_sig, pktype;
    int authenticated = 0;

    if (!authctxt->valid) {
        debug2("userauth_pubkey: disabled because of invalid user");
        return 0;
    }
#ifndef L7_SSHD
    have_sig = packet_get_char();
    if (datafellows & SSH_BUG_PKAUTH) {
#else
    have_sig = packet_get_char(authctxt->cn);
    if (datafellows[authctxt->cn] & SSH_BUG_PKAUTH) {
#endif /* L7_SSHD */
        debug2("userauth_pubkey: SSH_BUG_PKAUTH");
        /* no explicit pkalg given */
#ifndef L7_SSHD
        pkblob = packet_get_string(&blen);
#else
        pkblob = packet_get_string(authctxt->cn, &blen);
#endif /* L7_SSHD */
        buffer_init(&b);
        buffer_append(&b, pkblob, blen);
        /* so we have to extract the pkalg from the pkblob */
        pkalg = buffer_get_string(&b, &alen);
        buffer_free(&b);
    } else {
#ifndef L7_SSHD
        pkalg = packet_get_string(&alen);
        pkblob = packet_get_string(&blen);
#else
        pkalg = packet_get_string(authctxt->cn, &alen);
        pkblob = packet_get_string(authctxt->cn, &blen);
#endif /* L7_SSHD */
    }
    pktype = key_type_from_name(pkalg);
    if (pktype == KEY_UNSPEC) {
        /* this is perfectly legal */
        logit("userauth_pubkey: unsupported public key algorithm: %s",
            pkalg);
        goto done;
    }
#ifndef L7_SSHD
    key = key_from_blob(pkblob, blen);
#else
    key = key_from_blob(authctxt->cn, pkblob, blen);
#endif /* L7_SSHD */
    if (key == NULL) {
        error("userauth_pubkey: cannot decode key: %s", pkalg);
        goto done;
    }
    if (key->type != pktype) {
        error("userauth_pubkey: type mismatch for decoded key "
            "(received %d, expected %d)", key->type, pktype);
        goto done;
    }
    if (have_sig) {
#ifndef L7_SSHD
        sig = packet_get_string(&slen);
        packet_check_eom();
#else
        sig = packet_get_string(authctxt->cn, &slen);
        packet_check_eom(authctxt->cn);
#endif /* L7_SSHD */
        buffer_init(&b);
#ifndef L7_SSHD
        if (datafellows & SSH_OLD_SESSIONID) {
            buffer_append(&b, session_id2, session_id2_len);
        } else {
            buffer_put_string(&b, session_id2, session_id2_len);
        }
#else
        if (datafellows[authctxt->cn] & SSH_OLD_SESSIONID) {
            buffer_append(&b, session_id2[authctxt->cn], session_id2_len[authctxt->cn]);
        } else {
            buffer_put_string(&b, session_id2[authctxt->cn], session_id2_len[authctxt->cn]);
        }
#endif /* L7_SSHD */
        /* reconstruct packet */
        buffer_put_char(&b, SSH2_MSG_USERAUTH_REQUEST);
        buffer_put_cstring(&b, authctxt->user);
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
            buffer_put_cstring(&b, "publickey");
            buffer_put_char(&b, have_sig);
            buffer_put_cstring(&b, pkalg);
        }
        buffer_put_string(&b, pkblob, blen);
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
        /* test for correct signature */
        authenticated = 0;
#ifndef L7_SSHD
        if (PRIVSEP(user_key_allowed(authctxt->pw, key)) &&
            PRIVSEP(key_verify(key, sig, slen, buffer_ptr(&b),
                               buffer_len(&b))) == 1)
#else
        if ((sshdUserPubKeyAuthCallback(authctxt->pw->username, key) == L7_SUCCESS) &&
            PRIVSEP(key_verify(authctxt->cn, key, sig, slen, buffer_ptr(&b),
                               buffer_len(&b))) == 1)
        {
#endif /* L7_SSHD */
            authenticated = 1;
#ifdef L7_SSHD
        }
        else
        {
          debug2("public auth failed for user %s\n",authctxt->pw->username);
        }
#endif
        buffer_free(&b);
        xfree(sig);
    } else {
        debug("test whether pkalg/pkblob are acceptable");
#ifndef L7_SSHD
        packet_check_eom();
#else
        packet_check_eom(authctxt->cn);
#endif /* L7_SSHD */

        /* XXX fake reply and always send PK_OK ? */
        /*
         * XXX this allows testing whether a user is allowed
         * to login: if you happen to have a valid pubkey this
         * message is sent. the message is NEVER sent at all
         * if a user is not allowed to login. is this an
         * issue? -markus
         */
#ifndef L7_SSHD
        if (PRIVSEP(user_key_allowed(authctxt->pw, key))) {
            packet_start(SSH2_MSG_USERAUTH_PK_OK);
            packet_put_string(pkalg, alen);
            packet_put_string(pkblob, blen);
            packet_send();
            packet_write_wait();
            authctxt->postponed = 1;
        }
#else
        if (sshdUserPubKeyAuthCallback(authctxt->pw->username, key) == L7_SUCCESS) {
            packet_start(authctxt->cn, SSH2_MSG_USERAUTH_PK_OK);
            packet_put_string(authctxt->cn, pkalg, alen);
            packet_put_string(authctxt->cn, pkblob, blen);
            packet_send(authctxt->cn);
            packet_write_wait(authctxt->cn);
            authctxt->postponed = 1;
        } else {
          debug2("public auth not allowed for user %s\n",authctxt->pw->username);
        }
              
#endif /* L7_SSHD */
    }
    if (authenticated != 1)
        auth_clear_options();
done:
    debug2("userauth_pubkey: authenticated %d pkalg %s", authenticated, pkalg);
    if (key != NULL)
        key_free(key);
    xfree(pkalg);
    xfree(pkblob);
#ifdef HAVE_CYGWIN
    if (check_nt_auth(0, authctxt->pw) == 0)
        authenticated = 0;
#endif
    return authenticated;
}

/* return 1 if user allows given key */
#ifndef L7_SSHD
static int
user_key_allowed2(struct passwd *pw, Key *key, char *file)
#else
static int
user_key_allowed2(int cn, struct passwd *pw, Key *key, char *file)
#endif /* L7_SSHD */
{
    char line[SSH_MAX_PUBKEY_BYTES];
    int found_key = 0;
    FILE *f;
    u_long linenum = 0;
    struct stat st;
    Key *found;
    char *fp;

    /* Temporarily use the user's uid. */
    temporarily_use_uid(pw);

    debug("trying public key file %s", file);

    /* Fail quietly if file does not exist */
    if (stat(file, &st) < 0) {
        /* Restore the privileged uid. */
        restore_uid();
        return 0;
    }
    /* Open the file containing the authorized keys. */
    f = fopen(file, "r");
    if (!f) {
        /* Restore the privileged uid. */
        restore_uid();
        return 0;
    }
    if (options.strict_modes &&
        secure_filename(f, file, pw, line, sizeof(line)) != 0) {
        fclose(f);
        logit("Authentication refused: %s", line);
        restore_uid();
        return 0;
    }

    found_key = 0;
#ifndef L7_SSHD
    found = key_new(key->type);
#else
    found = key_new(cn, key->type);
#endif /* L7_SSHD */

    while (read_keyfile_line(f, file, line, sizeof(line), &linenum) != -1) {
        char *cp, *key_options = NULL;

        /* Skip leading whitespace, empty and comment lines. */
        for (cp = line; *cp == ' ' || *cp == '\t'; cp++)
            ;
        if (!*cp || *cp == '\n' || *cp == '#')
            continue;

        if (key_read(found, &cp) != 1) {
            /* no key?  check if there are options for this key */
            int quoted = 0;
            debug2("user_key_allowed: check options: '%s'", cp);
            key_options = cp;
            for (; *cp && (quoted || (*cp != ' ' && *cp != '\t')); cp++) {
                if (*cp == '\\' && cp[1] == '"')
                    cp++;   /* Skip both */
                else if (*cp == '"')
                    quoted = !quoted;
            }
            /* Skip remaining whitespace. */
            for (; *cp == ' ' || *cp == '\t'; cp++)
                ;
            if (key_read(found, &cp) != 1) {
                debug2("user_key_allowed: advance: '%s'", cp);
                /* still no key?  advance to next line*/
                continue;
            }
        }
        if (key_equal(found, key) &&
#ifndef L7_SSHD
            auth_parse_options(pw, key_options, file, linenum) == 1) {
#else
            auth_parse_options(cn, pw, key_options, file, linenum) == 1) {
#endif /* L7_SSHD */
            found_key = 1;
            debug("matching key found: file %s, line %lu",
                file, linenum);
            fp = key_fingerprint(found, SSH_FP_MD5, SSH_FP_HEX);
            verbose("Found matching %s key: %s",
                key_type(found), fp);
            xfree(fp);
            break;
        }
    }
    restore_uid();
    fclose(f);
    key_free(found);
    if (!found_key)
        debug2("key not found");
    return found_key;
}

/* check whether given key is in .ssh/authorized_keys* */
#ifndef L7_SSHD
int
user_key_allowed(struct passwd *pw, Key *key)
#else
int
user_key_allowed(int cn, struct passwd *pw, Key *key)
#endif /* L7_SSHD */
{
    int success;
    char *file;

#ifndef L7_SSHD
    file = authorized_keys_file(pw);
    success = user_key_allowed2(pw, key, file);
#else
    file = authorized_keys_file(cn, pw);
    success = user_key_allowed2(cn, pw, key, file);
#endif /* L7_SSHD */
    xfree(file);
    if (success)
        return success;

    /* try suffix "2" for backward compat, too */
#ifndef L7_SSHD
    file = authorized_keys_file2(pw);
    success = user_key_allowed2(pw, key, file);
#else
    file = authorized_keys_file2(cn, pw);
    success = user_key_allowed2(cn, pw, key, file);
#endif /* L7_SSHD */
    xfree(file);
    return success;
}

Authmethod method_pubkey = {
    "publickey",
    userauth_pubkey,
    &options.pubkey_authentication
};
