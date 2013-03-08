/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Code to connect to a remote host, and to perform the client side of the
 * login (authentication) dialog.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: sshconnect1.c,v 1.62 2005/10/30 08:52:18 djm Exp $");

#include <openssl/bn.h>
#include <openssl/md5.h>

#include "ssh.h"
#include "ssh1.h"
#include "xmalloc.h"
#include "rsa.h"
#include "buffer.h"
#include "packet.h"
#include "kex.h"
#include "uidswap.h"
#include "log.h"
#include "readconf.h"
#include "key.h"
#include "authfd.h"
#include "sshconnect.h"
#include "authfile.h"
#include "misc.h"
#include "cipher.h"
#include "canohost.h"
#include "auth.h"

/* Session id for the current session. */
#ifndef L7_SSHD
u_char session_id[16];
#else
u_char session_id[L7_OPENSSH_MAX_CONNECTIONS][16];
#endif /* L7_SSHD */
u_int supported_authentications = 0;

#ifndef L7_SSHD
extern Options options;
#else
extern Options client_options;
#endif /* L7_SSHD */
extern char *__progname;

/*
 * Checks if the user has an authentication agent, and if so, tries to
 * authenticate using the agent.
 */
#ifndef L7_SSHD
static int
try_agent_authentication(void)
#else
static int
try_agent_authentication(int cn)
#endif /* L7_SSHD */
{
    int type;
    char *comment;
    AuthenticationConnection *auth;
    u_char response[16];
    u_int i;
    Key *key;
    BIGNUM *challenge;

    /* Get connection to the agent. */
    auth = ssh_get_authentication_connection();
    if (!auth)
        return 0;

#ifndef L7_SSHD
    if ((challenge = BN_new()) == NULL)
        fatal("try_agent_authentication: BN_new failed");
        /* Loop through identities served by the agent. */
    for (key = ssh_get_first_identity(auth, &comment, 1);
        key != NULL;
        key = ssh_get_next_identity(auth, &comment, 1)) {
#else
    if ((challenge = BN_new()) == NULL)
        fatal_cn(cn, "try_agent_authentication: BN_new failed");
        /* Loop through identities served by the agent. */
    for (key = ssh_get_first_identity(cn, auth, &comment, 1);
        key != NULL;
        key = ssh_get_next_identity(cn, auth, &comment, 1)) {
#endif /* L7_SSHD */

        /* Try this identity. */
        debug("Trying RSA authentication via agent with '%.100s'", comment);
        xfree(comment);

        /* Tell the server that we are willing to authenticate using this key. */
#ifndef L7_SSHD
        packet_start(SSH_CMSG_AUTH_RSA);
        packet_put_bignum(key->rsa->n);
        packet_send();
        packet_write_wait();

        /* Wait for server's response. */
        type = packet_read();
#else
        packet_start(cn, SSH_CMSG_AUTH_RSA);
        packet_put_bignum(cn, key->rsa->n);
        packet_send(cn);
        packet_write_wait(cn);

        /* Wait for server's response. */
        type = packet_read(cn);
#endif /* L7_SSHD */

        /* The server sends failure if it doesn't like our key or
           does not support RSA authentication. */
        if (type == SSH_SMSG_FAILURE) {
            debug("Server refused our key.");
            key_free(key);
            continue;
        }
        /* Otherwise it should have sent a challenge. */
#ifndef L7_SSHD
        if (type != SSH_SMSG_AUTH_RSA_CHALLENGE)
            packet_disconnect("Protocol error during RSA authentication: %d",
                      type);
        packet_get_bignum(challenge);
        packet_check_eom();
#else
        if (type != SSH_SMSG_AUTH_RSA_CHALLENGE)
            packet_disconnect(cn, "Protocol error during RSA authentication: %d",
                      type);
        packet_get_bignum(cn, challenge);
        packet_check_eom(cn);
#endif /* L7_SSHD */

        debug("Received RSA challenge from server.");

        /* Ask the agent to decrypt the challenge. */
#ifndef L7_SSHD
        if (!ssh_decrypt_challenge(auth, key, challenge, session_id, 1, response)) {
#else
        if (!ssh_decrypt_challenge(auth, key, challenge, session_id[cn], 1, response)) {
#endif /* L7_SSHD */
            /*
             * The agent failed to authenticate this identifier
             * although it advertised it supports this.  Just
             * return a wrong value.
             */
            logit("Authentication agent failed to decrypt challenge.");
            memset(response, 0, sizeof(response));
        }
        key_free(key);
        debug("Sending response to RSA challenge.");

        /* Send the decrypted challenge back to the server. */
#ifndef L7_SSHD
        packet_start(SSH_CMSG_AUTH_RSA_RESPONSE);
        for (i = 0; i < 16; i++)
            packet_put_char(response[i]);
        packet_send();
        packet_write_wait();

        /* Wait for response from the server. */
        type = packet_read();
#else
        packet_start(cn, SSH_CMSG_AUTH_RSA_RESPONSE);
        for (i = 0; i < 16; i++)
            packet_put_char(cn, response[i]);
        packet_send(cn);
        packet_write_wait(cn);

        /* Wait for response from the server. */
        type = packet_read(cn);
#endif /* L7_SSHD */

        /* The server returns success if it accepted the authentication. */
        if (type == SSH_SMSG_SUCCESS) {
            ssh_close_authentication_connection(auth);
            BN_clear_free(challenge);
            debug("RSA authentication accepted by server.");
            return 1;
        }
        /* Otherwise it should return failure. */
        if (type != SSH_SMSG_FAILURE)
#ifndef L7_SSHD
            packet_disconnect("Protocol error waiting RSA auth response: %d",
                      type);
#else
            packet_disconnect(cn, "Protocol error waiting RSA auth response: %d",
                      type);
#endif /* L7_SSHD */
    }
    ssh_close_authentication_connection(auth);
    BN_clear_free(challenge);
    debug("RSA authentication using agent refused.");
    return 0;
}

/*
 * Computes the proper response to a RSA challenge, and sends the response to
 * the server.
 */
#ifndef L7_SSHD
static void
respond_to_rsa_challenge(BIGNUM * challenge, RSA * prv)
#else
static void
respond_to_rsa_challenge(int cn, BIGNUM * challenge, RSA * prv)
#endif /* L7_SSHD */
{
    u_char buf[32], response[16];
    MD5_CTX md;
    int i, len;

    /* Decrypt the challenge using the private key. */
    /* XXX think about Bleichenbacher, too */
    if (rsa_private_decrypt(challenge, challenge, prv) <= 0)
#ifndef L7_SSHD
        packet_disconnect(
            "respond_to_rsa_challenge: rsa_private_decrypt failed");
#else
        packet_disconnect(cn,
            "respond_to_rsa_challenge: rsa_private_decrypt failed");
#endif /* L7_SSHD */

    /* Compute the response. */
    /* The response is MD5 of decrypted challenge plus session id. */
    len = BN_num_bytes(challenge);
    if (len <= 0 || (u_int)len > sizeof(buf))
#ifndef L7_SSHD
        packet_disconnect(
            "respond_to_rsa_challenge: bad challenge length %d", len);
#else
        packet_disconnect(cn,
            "respond_to_rsa_challenge: bad challenge length %d", len);
#endif /* L7_SSHD */

    memset(buf, 0, sizeof(buf));
    BN_bn2bin(challenge, buf + sizeof(buf) - len);
    MD5_Init(&md);
    MD5_Update(&md, buf, 32);
#ifndef L7_SSHD
    MD5_Update(&md, session_id, 16);
#else
    MD5_Update(&md, session_id[cn], 16);
#endif /* L7_SSHD */
    MD5_Final(response, &md);

    debug("Sending response to host key RSA challenge.");

    /* Send the response back to the server. */
#ifndef L7_SSHD
    packet_start(SSH_CMSG_AUTH_RSA_RESPONSE);
    for (i = 0; i < 16; i++)
        packet_put_char(response[i]);
    packet_send();
    packet_write_wait();
#else
    packet_start(cn, SSH_CMSG_AUTH_RSA_RESPONSE);
    for (i = 0; i < 16; i++)
        packet_put_char(cn, response[i]);
    packet_send(cn);
    packet_write_wait(cn);
#endif /* L7_SSHD */

    memset(buf, 0, sizeof(buf));
    memset(response, 0, sizeof(response));
    memset(&md, 0, sizeof(md));
}

/*
 * Checks if the user has authentication file, and if so, tries to authenticate
 * the user using it.
 */
#ifndef L7_SSHD
static int
try_rsa_authentication(int idx)
#else
static int
try_rsa_authentication(int cn, int idx)
#endif /* L7_SSHD */
{
    BIGNUM *challenge;
    Key *public, *private;
    char buf[300], *passphrase, *comment, *authfile;
    int i, type, quit;

#ifndef L7_SSHD
    public = options.identity_keys[idx];
    authfile = options.identity_files[idx];
#else
    public = client_options.identity_keys[idx];
    authfile = client_options.identity_files[idx];
#endif /* L7_SSHD */
    comment = xstrdup(authfile);

    debug("Trying RSA authentication with key '%.100s'", comment);

    /* Tell the server that we are willing to authenticate using this key. */
#ifndef L7_SSHD
    packet_start(SSH_CMSG_AUTH_RSA);
    packet_put_bignum(public->rsa->n);
    packet_send();
    packet_write_wait();

    /* Wait for server's response. */
    type = packet_read();
#else
    packet_start(cn, SSH_CMSG_AUTH_RSA);
    packet_put_bignum(cn, public->rsa->n);
    packet_send(cn);
    packet_write_wait(cn);

    /* Wait for server's response. */
    type = packet_read(cn);
#endif /* L7_SSHD */

    /*
     * The server responds with failure if it doesn't like our key or
     * doesn't support RSA authentication.
     */
    if (type == SSH_SMSG_FAILURE) {
        debug("Server refused our key.");
        xfree(comment);
        return 0;
    }
    /* Otherwise, the server should respond with a challenge. */
    if (type != SSH_SMSG_AUTH_RSA_CHALLENGE)
#ifndef L7_SSHD
        packet_disconnect("Protocol error during RSA authentication: %d", type);
#else
        packet_disconnect(cn, "Protocol error during RSA authentication: %d", type);
#endif /* L7_SSHD */

    /* Get the challenge from the packet. */
#ifndef L7_SSHD
    if ((challenge = BN_new()) == NULL)
        fatal("try_rsa_authentication: BN_new failed");
    packet_get_bignum(challenge);
    packet_check_eom();
#else
    if ((challenge = BN_new()) == NULL)
        fatal_cn(cn, "try_rsa_authentication: BN_new failed");
    packet_get_bignum(cn, challenge);
    packet_check_eom(cn);
#endif /* L7_SSHD */

    debug("Received RSA challenge from server.");

    /*
     * If the key is not stored in external hardware, we have to
     * load the private key.  Try first with empty passphrase; if it
     * fails, ask for a passphrase.
     */
    if (public->flags & KEY_FLAG_EXT)
        private = public;
    else
        private = key_load_private_type(KEY_RSA1, authfile, "", NULL);
#ifndef L7_SSHD
    if (private == NULL && !options.batch_mode) {
        snprintf(buf, sizeof(buf),
                 "Enter passphrase for RSA key '%.100s': ", comment);
        for (i = 0; i < options.number_of_password_prompts; i++) {
#else
    if (private == NULL && !client_options.batch_mode) {
        osapiSnprintf(buf, sizeof(buf),
                      "Enter passphrase for RSA key '%.100s': ", comment);
        for (i = 0; i < client_options.number_of_password_prompts; i++) {
#endif /* L7_SSHD */
            passphrase = read_passphrase(buf, 0);
            if (strcmp(passphrase, "") != 0) {
                private = key_load_private_type(KEY_RSA1,
                    authfile, passphrase, NULL);
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
    /* We no longer need the comment. */
    xfree(comment);

#ifndef L7_SSHD
    if (private == NULL) {
        if (!options.batch_mode)
            error("Bad passphrase.");

        /* Send a dummy response packet to avoid protocol error. */
        packet_start(SSH_CMSG_AUTH_RSA_RESPONSE);
        for (i = 0; i < 16; i++)
            packet_put_char(0);
        packet_send();
        packet_write_wait();

        /* Expect the server to reject it... */
        packet_read_expect(SSH_SMSG_FAILURE);
#else
    if (private == NULL) {
        if (!client_options.batch_mode)
            error("Bad passphrase.");

        /* Send a dummy response packet to avoid protocol error. */
        packet_start(cn, SSH_CMSG_AUTH_RSA_RESPONSE);
        for (i = 0; i < 16; i++)
            packet_put_char(cn, 0);
        packet_send(cn);
        packet_write_wait(cn);

        /* Expect the server to reject it... */
        packet_read_expect(cn, SSH_SMSG_FAILURE);
#endif /* L7_SSHD */
        BN_clear_free(challenge);
        return 0;
    }

    /* Compute and send a response to the challenge. */
#ifndef L7_SSHD
    respond_to_rsa_challenge(challenge, private->rsa);
#else
    respond_to_rsa_challenge(cn, challenge, private->rsa);
#endif /* L7_SSHD */

    /* Destroy the private key unless it in external hardware. */
    if (!(private->flags & KEY_FLAG_EXT))
        key_free(private);

    /* We no longer need the challenge. */
    BN_clear_free(challenge);

    /* Wait for response from the server. */
#ifndef L7_SSHD
    type = packet_read();
#else
    type = packet_read(cn);
#endif /* L7_SSHD */
    if (type == SSH_SMSG_SUCCESS) {
        debug("RSA authentication accepted by server.");
        return 1;
    }
    if (type != SSH_SMSG_FAILURE)
#ifndef L7_SSHD
        packet_disconnect("Protocol error waiting RSA auth response: %d", type);
#else
        packet_disconnect(cn, "Protocol error waiting RSA auth response: %d", type);
#endif /* L7_SSHD */
    debug("RSA authentication refused.");
    return 0;
}

/*
 * Tries to authenticate the user using combined rhosts or /etc/hosts.equiv
 * authentication and RSA host authentication.
 */
#ifndef L7_SSHD
static int
try_rhosts_rsa_authentication(const char *local_user, Key * host_key)
#else
static int
try_rhosts_rsa_authentication(int cn, const char *local_user, Key * host_key)
#endif /* L7_SSHD */
{
    int type;
    BIGNUM *challenge;

    debug("Trying rhosts or /etc/hosts.equiv with RSA host authentication.");

    /* Tell the server that we are willing to authenticate using this key. */
#ifndef L7_SSHD
    packet_start(SSH_CMSG_AUTH_RHOSTS_RSA);
    packet_put_cstring(local_user);
    packet_put_int(BN_num_bits(host_key->rsa->n));
    packet_put_bignum(host_key->rsa->e);
    packet_put_bignum(host_key->rsa->n);
    packet_send();
    packet_write_wait();

    /* Wait for server's response. */
    type = packet_read();
#else
    packet_start(cn, SSH_CMSG_AUTH_RHOSTS_RSA);
    packet_put_cstring(cn, local_user);
    packet_put_int(cn, BN_num_bits(host_key->rsa->n));
    packet_put_bignum(cn, host_key->rsa->e);
    packet_put_bignum(cn, host_key->rsa->n);
    packet_send(cn);
    packet_write_wait(cn);

    /* Wait for server's response. */
    type = packet_read(cn);
#endif /* L7_SSHD */

    /* The server responds with failure if it doesn't admit our
       .rhosts authentication or doesn't know our host key. */
    if (type == SSH_SMSG_FAILURE) {
        debug("Server refused our rhosts authentication or host key.");
        return 0;
    }
    /* Otherwise, the server should respond with a challenge. */
    if (type != SSH_SMSG_AUTH_RSA_CHALLENGE)
#ifndef L7_SSHD
        packet_disconnect("Protocol error during RSA authentication: %d", type);
#else
        packet_disconnect(cn, "Protocol error during RSA authentication: %d", type);
#endif /* L7_SSHD */

    /* Get the challenge from the packet. */
#ifndef L7_SSHD
    if ((challenge = BN_new()) == NULL)
        fatal("try_rhosts_rsa_authentication: BN_new failed");
    packet_get_bignum(challenge);
    packet_check_eom();
#else
    if ((challenge = BN_new()) == NULL)
        fatal_cn(cn, "try_rhosts_rsa_authentication: BN_new failed");
    packet_get_bignum(cn, challenge);
    packet_check_eom(cn);
#endif /* L7_SSHD */

    debug("Received RSA challenge for host key from server.");

    /* Compute a response to the challenge. */
#ifndef L7_SSHD
    respond_to_rsa_challenge(challenge, host_key->rsa);
#else
    respond_to_rsa_challenge(cn, challenge, host_key->rsa);
#endif /* L7_SSHD */

    /* We no longer need the challenge. */
    BN_clear_free(challenge);

    /* Wait for response from the server. */
#ifndef L7_SSHD
    type = packet_read();
#else
    type = packet_read(cn);
#endif /* L7_SSHD */
    if (type == SSH_SMSG_SUCCESS) {
        debug("Rhosts or /etc/hosts.equiv with RSA host authentication accepted by server.");
        return 1;
    }
    if (type != SSH_SMSG_FAILURE)
#ifndef L7_SSHD
        packet_disconnect("Protocol error waiting RSA auth response: %d", type);
#else
        packet_disconnect(cn, "Protocol error waiting RSA auth response: %d", type);
#endif /* L7_SSHD */
    debug("Rhosts or /etc/hosts.equiv with RSA host authentication refused.");
    return 0;
}

/*
 * Tries to authenticate with any string-based challenge/response system.
 * Note that the client code is not tied to s/key or TIS.
 */
#ifndef L7_SSHD
static int
try_challenge_response_authentication(void)
#else
static int
try_challenge_response_authentication(int cn)
#endif /* L7_SSHD */
{
    int type, i;
    u_int clen;
    char prompt[1024];
    char *challenge, *response;

    debug("Doing challenge response authentication.");

#ifndef L7_SSHD
    for (i = 0; i < options.number_of_password_prompts; i++) {
        /* request a challenge */
        packet_start(SSH_CMSG_AUTH_TIS);
        packet_send();
        packet_write_wait();

        type = packet_read();
        if (type != SSH_SMSG_FAILURE &&
            type != SSH_SMSG_AUTH_TIS_CHALLENGE) {
            packet_disconnect("Protocol error: got %d in response "
                "to SSH_CMSG_AUTH_TIS", type);
        }
#else
    for (i = 0; i < client_options.number_of_password_prompts; i++) {
        /* request a challenge */
        packet_start(cn, SSH_CMSG_AUTH_TIS);
        packet_send(cn);
        packet_write_wait(cn);

        type = packet_read(cn);
        if (type != SSH_SMSG_FAILURE &&
            type != SSH_SMSG_AUTH_TIS_CHALLENGE) {
            packet_disconnect(cn, "Protocol error: got %d in response "
                "to SSH_CMSG_AUTH_TIS", type);
        }
#endif /* L7_SSHD */
        if (type != SSH_SMSG_AUTH_TIS_CHALLENGE) {
            debug("No challenge.");
            return 0;
        }
#ifndef L7_SSHD
        challenge = packet_get_string(&clen);
        packet_check_eom();
        snprintf(prompt, sizeof prompt, "%s%s", challenge,
#else
        challenge = packet_get_string(cn, &clen);
        packet_check_eom(cn);
        osapiSnprintf(prompt, sizeof prompt, "%s%s", challenge,
#endif /* L7_SSHD */
            strchr(challenge, '\n') ? "" : "\nResponse: ");
        xfree(challenge);
        if (i != 0)
            error("Permission denied, please try again.");
#ifndef L7_SSHD
        if (options.cipher == SSH_CIPHER_NONE)
#else
        if (client_options.cipher == SSH_CIPHER_NONE)
#endif /* L7_SSHD */
            logit("WARNING: Encryption is disabled! "
                "Response will be transmitted in clear text.");
        response = read_passphrase(prompt, 0);
        if (strcmp(response, "") == 0) {
            xfree(response);
            break;
        }
#ifndef L7_SSHD
        packet_start(SSH_CMSG_AUTH_TIS_RESPONSE);
        ssh_put_password(response);
#else
        packet_start(cn, SSH_CMSG_AUTH_TIS_RESPONSE);
        ssh_put_password(cn, response);
#endif /* L7_SSHD */
        memset(response, 0, strlen(response));
        xfree(response);
#ifndef L7_SSHD
        packet_send();
        packet_write_wait();
        type = packet_read();
#else
        packet_send(cn);
        packet_write_wait(cn);
        type = packet_read(cn);
#endif /* L7_SSHD */
        if (type == SSH_SMSG_SUCCESS)
            return 1;
        if (type != SSH_SMSG_FAILURE)
#ifndef L7_SSHD
            packet_disconnect("Protocol error: got %d in response "
                "to SSH_CMSG_AUTH_TIS_RESPONSE", type);
#else
            packet_disconnect(cn, "Protocol error: got %d in response "
                "to SSH_CMSG_AUTH_TIS_RESPONSE", type);
#endif /* L7_SSHD */
    }
    /* failure */
    return 0;
}

/*
 * Tries to authenticate with plain passwd authentication.
 */
#ifndef L7_SSHD
static int
try_password_authentication(char *prompt)
#else
static int
try_password_authentication(int cn, char *prompt, char *password)
#endif /* L7_SSHD */
{
#ifndef L7_SSHD
    int type, i;
    char *password;
#else
    int type;
#endif /* L7_SSHD */

    debug("Doing password authentication.");
#ifndef L7_SSHD
    if (options.cipher == SSH_CIPHER_NONE)
        logit("WARNING: Encryption is disabled! Password will be transmitted in clear text.");
    for (i = 0; i < options.number_of_password_prompts; i++) {
        if (i != 0)
            error("Permission denied, please try again.");
        password = read_passphrase(prompt, 0);
        packet_start(SSH_CMSG_AUTH_PASSWORD);
        ssh_put_password(password);
#else
        if (client_options.cipher == SSH_CIPHER_NONE)
            logit("WARNING: Encryption is disabled! Password will be transmitted in clear text.");
        packet_start(cn, SSH_CMSG_AUTH_PASSWORD);
        ssh_put_password(cn, password);
#endif /* L7_SSHD */
#ifndef L7_SSHD
        memset(password, 0, strlen(password));
        xfree(password);
        packet_send();
        packet_write_wait();

        type = packet_read();
#else
        packet_send(cn);
        packet_write_wait(cn);

        type = packet_read(cn);
#endif /* L7_SSHD */
        if (type == SSH_SMSG_SUCCESS)
            return 1;
        if (type != SSH_SMSG_FAILURE)
#ifndef L7_SSHD
            packet_disconnect("Protocol error: got %d in response to passwd auth", type);
    }
#else
            return -1;
#endif /* L7_SSHD */
    /* failure */
    return 0;
}

/*
 * SSH1 key exchange
 */
#ifndef L7_SSHD
void
ssh_kex(char *host, struct sockaddr *hostaddr)
#else
int
ssh_kex(int cn, char *host, struct sockaddr *hostaddr)
#endif /* L7_SSHD */
{
    int i;
    BIGNUM *key;
    Key *host_key, *server_key;
    int bits, rbits;
    int ssh_cipher_default = SSH_CIPHER_3DES;
    u_char session_key[SSH_SESSION_KEY_LENGTH];
    u_char cookie[8];
    u_int supported_ciphers;
    u_int server_flags, client_flags;
    u_int32_t rnd = 0;

    debug("Waiting for server public key.");

    /* Wait for a public key packet from the server. */
#ifndef L7_SSHD
    packet_read_expect(SSH_SMSG_PUBLIC_KEY);
#else
    packet_read_expect(cn, SSH_SMSG_PUBLIC_KEY);
#endif /* L7_SSHD */

    /* Get cookie from the packet. */
    for (i = 0; i < 8; i++)
#ifndef L7_SSHD
        cookie[i] = packet_get_char();
#else
        cookie[i] = packet_get_char(cn);
#endif /* L7_SSHD */

    /* Get the public key. */
#ifndef L7_SSHD
    server_key = key_new(KEY_RSA1);
    bits = packet_get_int();
    packet_get_bignum(server_key->rsa->e);
    packet_get_bignum(server_key->rsa->n);
#else
    server_key = key_new(cn, KEY_RSA1);
    bits = packet_get_int(cn);
    packet_get_bignum(cn, server_key->rsa->e);
    packet_get_bignum(cn, server_key->rsa->n);
#endif /* L7_SSHD */

    rbits = BN_num_bits(server_key->rsa->n);
    if (bits != rbits) {
        logit("Warning: Server lies about size of server public key: "
            "actual size is %d bits vs. announced %d.", rbits, bits);
        logit("Warning: This may be due to an old implementation of ssh.");
    }
#ifndef L7_SSHD
    /* Get the host key. */
    host_key = key_new(KEY_RSA1);
    bits = packet_get_int();
    packet_get_bignum(host_key->rsa->e);
    packet_get_bignum(host_key->rsa->n);
#else
    /* Get the host key. */
    host_key = key_new(cn, KEY_RSA1);
    bits = packet_get_int(cn);
    packet_get_bignum(cn, host_key->rsa->e);
    packet_get_bignum(cn, host_key->rsa->n);
#endif /* L7_SSHD */

    rbits = BN_num_bits(host_key->rsa->n);
    if (bits != rbits) {
        logit("Warning: Server lies about size of server host key: "
            "actual size is %d bits vs. announced %d.", rbits, bits);
        logit("Warning: This may be due to an old implementation of ssh.");
    }

    /* Get protocol flags. */
#ifndef L7_SSHD
    server_flags = packet_get_int();
    packet_set_protocol_flags(server_flags);

    supported_ciphers = packet_get_int();
    supported_authentications = packet_get_int();
    packet_check_eom();
#else
    server_flags = packet_get_int(cn);
    packet_set_protocol_flags(cn, server_flags);

    supported_ciphers = packet_get_int(cn);
    supported_authentications = packet_get_int(cn);
    packet_check_eom(cn);
#endif /* L7_SSHD */

    debug("Received server public key (%d bits) and host key (%d bits).",
        BN_num_bits(server_key->rsa->n), BN_num_bits(host_key->rsa->n));

    if (verify_host_key(host, hostaddr, host_key) == -1)
#ifndef L7_SSHD
        fatal("Host key verification failed.");
#else
    {
      error("Host key verification failed.");
      return -1;
    }
#endif /* L7_SSHD */

    client_flags = SSH_PROTOFLAG_SCREEN_NUMBER | SSH_PROTOFLAG_HOST_IN_FWD_OPEN;

#ifndef L7_SSHD
    derive_ssh1_session_id(host_key->rsa->n, server_key->rsa->n, cookie, session_id);
#else
    derive_ssh1_session_id(host_key->rsa->n, server_key->rsa->n, cookie, session_id[cn]);
#endif /* L7_SSHD */

    /* Generate a session key. */
    arc4random_stir();

    /*
     * Generate an encryption key for the session.   The key is a 256 bit
     * random number, interpreted as a 32-byte key, with the least
     * significant 8 bits being the first byte of the key.
     */
    for (i = 0; i < 32; i++) {
        if (i % 4 == 0)
            rnd = arc4random();
        session_key[i] = rnd & 0xff;
        rnd >>= 8;
    }

    /*
     * According to the protocol spec, the first byte of the session key
     * is the highest byte of the integer.  The session key is xored with
     * the first 16 bytes of the session id.
     */
    if ((key = BN_new()) == NULL)
#ifndef L7_SSHD
        fatal("respond_to_rsa_challenge: BN_new failed");
#else
    {
      error("respond_to_rsa_challenge: BN_new failed");
      return -1;
    }
#endif /* L7_SSHD */
    BN_set_word(key, 0);
    for (i = 0; i < SSH_SESSION_KEY_LENGTH; i++) {
        BN_lshift(key, key, 8);
        if (i < 16)
#ifndef L7_SSHD
            BN_add_word(key, session_key[i] ^ session_id[i]);
#else
            BN_add_word(key, session_key[i] ^ session_id[cn][i]);
#endif /* L7_SSHD */
        else
            BN_add_word(key, session_key[i]);
    }

    /*
     * Encrypt the integer using the public key and host key of the
     * server (key with smaller modulus first).
     */
    if (BN_cmp(server_key->rsa->n, host_key->rsa->n) < 0) {
        /* Public key has smaller modulus. */
        if (BN_num_bits(host_key->rsa->n) <
            BN_num_bits(server_key->rsa->n) + SSH_KEY_BITS_RESERVED) {
#ifndef L7_SSHD
            fatal("respond_to_rsa_challenge: host_key %d < server_key %d + "
                  "SSH_KEY_BITS_RESERVED %d",
                  BN_num_bits(host_key->rsa->n),
                  BN_num_bits(server_key->rsa->n),
                  SSH_KEY_BITS_RESERVED);
#else
            error("respond_to_rsa_challenge: host_key %d < server_key %d + "
                  "SSH_KEY_BITS_RESERVED %d",
                  BN_num_bits(host_key->rsa->n),
                  BN_num_bits(server_key->rsa->n),
                  SSH_KEY_BITS_RESERVED);
            return -1;
#endif /* L7_SSHD */
        }
        rsa_public_encrypt(key, key, server_key->rsa);
        rsa_public_encrypt(key, key, host_key->rsa);
    } else {
        /* Host key has smaller modulus (or they are equal). */
        if (BN_num_bits(server_key->rsa->n) <
            BN_num_bits(host_key->rsa->n) + SSH_KEY_BITS_RESERVED) {
#ifndef L7_SSHD
            fatal("respond_to_rsa_challenge: server_key %d < host_key %d + "
                  "SSH_KEY_BITS_RESERVED %d",
                  BN_num_bits(server_key->rsa->n),
                  BN_num_bits(host_key->rsa->n),
                  SSH_KEY_BITS_RESERVED);
#else
            error("respond_to_rsa_challenge: server_key %d < host_key %d + "
                  "SSH_KEY_BITS_RESERVED %d",
                  BN_num_bits(server_key->rsa->n),
                  BN_num_bits(host_key->rsa->n),
                  SSH_KEY_BITS_RESERVED);
            return -1;
#endif /* L7_SSHD */
        }
        rsa_public_encrypt(key, key, host_key->rsa);
        rsa_public_encrypt(key, key, server_key->rsa);
    }

    /* Destroy the public keys since we no longer need them. */
    key_free(server_key);
    key_free(host_key);

#ifndef L7_SSHD
    if (options.cipher == SSH_CIPHER_NOT_SET) {
        if (cipher_mask_ssh1(1) & supported_ciphers & (1 << ssh_cipher_default))
            options.cipher = ssh_cipher_default;
    } else if (options.cipher == SSH_CIPHER_INVALID ||
        !(cipher_mask_ssh1(1) & (1 << options.cipher))) {
        logit("No valid SSH1 cipher, using %.100s instead.",
            cipher_name(ssh_cipher_default));
        options.cipher = ssh_cipher_default;
    }
    /* Check that the selected cipher is supported. */
    if (!(supported_ciphers & (1 << options.cipher)))
        fatal("Selected cipher type %.100s not supported by server.",
            cipher_name(options.cipher));

    debug("Encryption type: %.100s", cipher_name(options.cipher));
#else
    if (client_options.cipher == SSH_CIPHER_NOT_SET) {
        if (cipher_mask_ssh1(1) & supported_ciphers & (1 << ssh_cipher_default))
            client_options.cipher = ssh_cipher_default;
    } else if (client_options.cipher == SSH_CIPHER_INVALID ||
        !(cipher_mask_ssh1(1) & (1 << client_options.cipher))) {
        logit("No valid SSH1 cipher, using %.100s instead.",
            cipher_name(ssh_cipher_default));
        client_options.cipher = ssh_cipher_default;
    }
    /* Check that the selected cipher is supported. */
    if (!(supported_ciphers & (1 << client_options.cipher)))
    {
      error("Selected cipher type %.100s not supported by server.",
          cipher_name(client_options.cipher));
      return -1;
    }

    debug("Encryption type: %.100s", cipher_name(client_options.cipher));
#endif /* L7_SSHD */

    /* Send the encrypted session key to the server. */
#ifndef L7_SSHD
    packet_start(SSH_CMSG_SESSION_KEY);
    packet_put_char(options.cipher);
#else
    packet_start(cn, SSH_CMSG_SESSION_KEY);
    packet_put_char(cn, client_options.cipher);
#endif /* L7_SSHD */

    /* Send the cookie back to the server. */
    for (i = 0; i < 8; i++)
#ifndef L7_SSHD
        packet_put_char(cookie[i]);
#else
        packet_put_char(cn, cookie[i]);
#endif /* L7_SSHD */

    /* Send and destroy the encrypted encryption key integer. */
#ifndef L7_SSHD
    packet_put_bignum(key);
    BN_clear_free(key);

    /* Send protocol flags. */
    packet_put_int(client_flags);

    /* Send the packet now. */
    packet_send();
    packet_write_wait();

    debug("Sent encrypted session key.");

    /* Set the encryption key. */
    packet_set_encryption_key(session_key, SSH_SESSION_KEY_LENGTH, options.cipher);
#else
    packet_put_bignum(cn, key);
    BN_clear_free(key);

    /* Send protocol flags. */
    packet_put_int(cn, client_flags);

    /* Send the packet now. */
    packet_send(cn);
    packet_write_wait(cn);

    debug("Sent encrypted session key.");

    /* Set the encryption key. */
    packet_set_encryption_key(cn, session_key, SSH_SESSION_KEY_LENGTH, client_options.cipher);
#endif /* L7_SSHD */

    /* We will no longer need the session key here.  Destroy any extra copies. */
    memset(session_key, 0, sizeof(session_key));

    /*
     * Expect a success message from the server.  Note that this message
     * will be received in encrypted form.
     */
#ifndef L7_SSHD
    packet_read_expect(SSH_SMSG_SUCCESS);
#else
    packet_read_expect(cn, SSH_SMSG_SUCCESS);
#endif /* L7_SSHD */

    debug("Received encrypted confirmation.");
    return 0;
}

/*
 * Authenticate user
 */
#ifndef L7_SSHD
void
ssh_userauth1(const char *local_user, const char *server_user, char *host,
    Sensitive *sensitive)
#else
int
ssh_userauth1(int cn, const char *local_user, const char *server_user, char *host,
    Sensitive *sensitive, char *password)
#endif /* L7_SSHD */
{
    int i, type;

#ifndef L7_SSHD
    if (supported_authentications == 0)
        fatal("ssh_userauth1: server supports no auth methods");

    /* Send the name of the user to log in as on the server. */
    packet_start(SSH_CMSG_USER);
    packet_put_cstring(server_user);
    packet_send();
    packet_write_wait();
#else
    if (supported_authentications == 0)
        fatal_cn(cn, "ssh_userauth1: server supports no auth methods");

    /* Send the name of the user to log in as on the server. */
    packet_start(cn, SSH_CMSG_USER);
    packet_put_cstring(cn, server_user);
    packet_send(cn);
    packet_write_wait(cn);
#endif /* L7_SSHD */

    /*
     * The server should respond with success if no authentication is
     * needed (the user has no password).  Otherwise the server responds
     * with failure.
     */
#ifndef L7_SSHD
    type = packet_read();
#else
    type = packet_read(cn);
#endif /* L7_SSHD */

    /* check whether the connection was accepted without authentication. */
    if (type == SSH_SMSG_SUCCESS)
        goto success;
    if (type != SSH_SMSG_FAILURE)
#ifndef L7_SSHD
        packet_disconnect("Protocol error: got %d in response to SSH_CMSG_USER", type);
#else
        return -1;
#endif /* L7_SSHD */

    /*
     * Try .rhosts or /etc/hosts.equiv authentication with RSA host
     * authentication.
     */
#ifndef L7_SSHD
    if ((supported_authentications & (1 << SSH_AUTH_RHOSTS_RSA)) &&
        options.rhosts_rsa_authentication) {
        for (i = 0; i < sensitive->nkeys; i++) {
            if (sensitive->keys[i] != NULL &&
                sensitive->keys[i]->type == KEY_RSA1 &&
                try_rhosts_rsa_authentication(local_user,
                sensitive->keys[i]))
                goto success;
        }
    }
#else
    if ((supported_authentications & (1 << SSH_AUTH_RHOSTS_RSA)) &&
        client_options.rhosts_rsa_authentication) {
        for (i = 0; i < sensitive->nkeys; i++) {
            if (sensitive->keys[i] != NULL &&
                sensitive->keys[i]->type == KEY_RSA1 &&
                try_rhosts_rsa_authentication(cn, local_user,
                sensitive->keys[i]))
                goto success;
        }
    }
#endif /* L7_SSHD */
    /* Try RSA authentication if the server supports it. */
#ifndef L7_SSHD
    if ((supported_authentications & (1 << SSH_AUTH_RSA)) &&
        options.rsa_authentication) {
        /*
         * Try RSA authentication using the authentication agent. The
         * agent is tried first because no passphrase is needed for
         * it, whereas identity files may require passphrases.
         */
        if (try_agent_authentication())
            goto success;
#else
    if ((supported_authentications & (1 << SSH_AUTH_RSA)) &&
        client_options.rsa_authentication) {
        /*
         * Try RSA authentication using the authentication agent. The
         * agent is tried first because no passphrase is needed for
         * it, whereas identity files may require passphrases.
         */
        if (try_agent_authentication(cn))
            goto success;
#endif /* L7_SSHD */

        /* Try RSA authentication for each identity. */
#ifndef L7_SSHD
        for (i = 0; i < options.num_identity_files; i++)
            if (options.identity_keys[i] != NULL &&
                options.identity_keys[i]->type == KEY_RSA1 &&
                try_rsa_authentication(i))
                goto success;
#else
        for (i = 0; i < client_options.num_identity_files; i++)
            if (client_options.identity_keys[i] != NULL &&
                client_options.identity_keys[i]->type == KEY_RSA1 &&
                try_rsa_authentication(cn, i))
                goto success;
#endif /* L7_SSHD */
    }
    /* Try challenge response authentication if the server supports it. */
#ifndef L7_SSHD
    if ((supported_authentications & (1 << SSH_AUTH_TIS)) &&
        options.challenge_response_authentication && !options.batch_mode) {
        if (try_challenge_response_authentication())
            goto success;
    }
#else
    if ((supported_authentications & (1 << SSH_AUTH_TIS)) &&
        client_options.challenge_response_authentication && !client_options.batch_mode) {
        if (try_challenge_response_authentication(cn))
            goto success;
    }
#endif /* L7_SSHD */
    /* Try password authentication if the server supports it. */
#ifndef L7_SSHD
    if ((supported_authentications & (1 << SSH_AUTH_PASSWORD)) &&
        options.password_authentication && !options.batch_mode) {
        char prompt[80];

        snprintf(prompt, sizeof(prompt), "%.30s@%.128s's password: ",
            server_user, host);
        if (try_password_authentication(prompt))
          goto success;
    }
#else
    if ((supported_authentications & (1 << SSH_AUTH_PASSWORD)) &&
        client_options.password_authentication && !client_options.batch_mode) {
        char prompt[80];

        osapiSnprintf(prompt, sizeof(prompt), "%.30s@%.128s's password: ",
            server_user, host);
        if (try_password_authentication(cn, prompt, password))
          goto success;
    }
#endif /* L7_SSHD */

    /* All authentication methods have failed.  Exit with an error message. */
#ifndef L7_SSHD
    fatal("Permission denied.");
#else
    return -1;
#endif /* L7_SSHD */
    /* NOTREACHED */

 success:
    return 1; /* need statement after label */
}
