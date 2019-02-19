/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains code implementing the packet protocol and communication
 * with the other side.  This same code is used both on client and server side.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 *
 * SSH2 packet format added by Markus Friedl.
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
RCSID("$OpenBSD: packet.c,v 1.120 2005/10/30 08:52:17 djm Exp $");

#include "openbsd-compat/sys-queue.h"

#include "xmalloc.h"
#include "buffer.h"
#include "packet.h"
#include "bufaux.h"
#include "crc32.h"
#include "getput.h"

#include "compress.h"
#include "deattack.h"
#include "channels.h"

#include "compat.h"
#include "ssh1.h"
#include "ssh2.h"

#include "cipher.h"
#include "kex.h"
#include "dh.h"
#include "mac.h"
#include "log.h"
#include "canohost.h"
#include "misc.h"
#include "ssh.h"
#include "atomicio.h"

#define DBG(x) x
#define PACKET_BUFFER_SIZE 8192

extern Kex *xxx_kex[];

extern char *sshc_server_version_string[];
extern char *sshc_client_version_string[];
extern int inout_v1[];    /* SSHv1 input/output socket */
extern Newkeys *current_keys[L7_OPENSSH_MAX_CONNECTIONS][MODE_MAX];

extern void kex_prop_free(char **proposal);

/*
 * This variable contains the file descriptors used for communicating with
 * the other side.  connection_in is used for reading; connection_out for
 * writing.  These can be the same descriptor, in which case it is assumed to
 * be a socket.
 */

/*
Note that the -1 initializer will not extend for array sizes
that are larger than 1 -- any array elements after the first
will be initialized to 0, but this will be taken care during
FastPath initialization
*/
int connection_in[L7_OPENSSH_MAX_CONNECTIONS] = {-1};
int connection_out[L7_OPENSSH_MAX_CONNECTIONS] = {-1};

/* Protocol flags for the remote side. */
static u_int remote_protocol_flags[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Encryption context for receiving data.  This is only used for decryption. */
static CipherContext receive_context[L7_OPENSSH_MAX_CONNECTIONS];

/* Encryption context for sending data.  This is only used for encryption. */
static CipherContext send_context[L7_OPENSSH_MAX_CONNECTIONS];

/* Buffer for raw input data from the socket. */
Buffer ssh_input[L7_OPENSSH_MAX_CONNECTIONS];

/* Buffer for raw output data going to the socket. */
Buffer ssh_output[L7_OPENSSH_MAX_CONNECTIONS];

/* Buffer for the partial outgoing packet being constructed. */
Buffer outgoing_packet[L7_OPENSSH_MAX_CONNECTIONS];

/* Buffer for the incoming packet currently being processed. */
Buffer incoming_packet[L7_OPENSSH_MAX_CONNECTIONS];

/* Pointers to the key exchange proposals */
char **sshd_my[L7_OPENSSH_MAX_CONNECTIONS] = {0};
char **sshd_peer[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* DH structs for kexdh and kexgex */
DH     *sshd_kex_dh[L7_OPENSSH_MAX_CONNECTIONS] = {0};
DH     *sshd_kexgex_dh[L7_OPENSSH_MAX_CONNECTIONS] = {0};
BIGNUM *sshd_dh_client_pub[L7_OPENSSH_MAX_CONNECTIONS] = {0};
BIGNUM *sshd_kexgex_dh_client_pub[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Scratch buffer for packet compression/decompression. */
static Buffer compression_buffer[L7_OPENSSH_MAX_CONNECTIONS];
static int compression_buffer_ready[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Flag indicating whether packet compression/decompression is enabled. */
static int packet_compression[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* default maximum packet size */
/*
Note that the -1 initializer will not extend for array sizes
that are larger than 1 -- any array elements after the first
will be initialized to 0, but this will be taken care during
FastPath initialization
*/
u_int max_packet_size[L7_OPENSSH_MAX_CONNECTIONS] = {32768};

/* Flag indicating whether this module has been initialized. */
static int initialized[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Set to true if the connection is interactive. */
static int interactive_mode[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Set to true if we are the server side. */
static int server_side[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Set to true if we are authenticated. */
static int after_authentication[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/* Session key information for Encryption and MAC */
Newkeys *newkeys[L7_OPENSSH_MAX_CONNECTIONS][MODE_MAX];

static struct packet_state {
    u_int32_t seqnr;
    u_int32_t packets;
    u_int64_t blocks;
} p_read[L7_OPENSSH_MAX_CONNECTIONS], p_send[L7_OPENSSH_MAX_CONNECTIONS];

static u_int64_t max_blocks_in[L7_OPENSSH_MAX_CONNECTIONS], max_blocks_out[L7_OPENSSH_MAX_CONNECTIONS];
static u_int32_t rekey_limit[L7_OPENSSH_MAX_CONNECTIONS];

/* Session key for protocol v1 */
static u_char ssh1_key[L7_OPENSSH_MAX_CONNECTIONS][SSH_SESSION_KEY_LENGTH];
static u_int ssh1_keylen[L7_OPENSSH_MAX_CONNECTIONS];

/* roundup current message to extra_pad bytes */
static u_char extra_pad[L7_OPENSSH_MAX_CONNECTIONS] = {0};

static u_int poll2_packet_length[L7_OPENSSH_MAX_CONNECTIONS] = {0};

struct packet {
    TAILQ_ENTRY(packet) next;
    u_char type;
    Buffer payload;
};
TAILQ_HEAD(, packet) outgoing[L7_OPENSSH_MAX_CONNECTIONS];

char *sshd_packet_buf[L7_OPENSSH_MAX_CONNECTIONS] = {0};

/*
 * Sets the descriptors used for communication.  Disables encryption until
 * packet_set_encryption_key is called.
 */
void
packet_set_connection(int cn, int fd_in, int fd_out)
{
    Cipher *none = cipher_by_name("none");

    debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: initializing connection", cn);
    if (none == NULL)
        fatal_cn(cn, "connection %d: packet_set_connection: cannot load cipher 'none'", cn);
    connection_in[cn] = fd_in;
    connection_out[cn] = fd_out;
    cipher_init(&send_context[cn], none, (const u_char *)"",
        0, NULL, 0, CIPHER_ENCRYPT);
    cipher_init(&receive_context[cn], none, (const u_char *)"",
        0, NULL, 0, CIPHER_DECRYPT);
    newkeys[cn][MODE_IN] = newkeys[cn][MODE_OUT] = NULL;
    if (!initialized[cn]) {
        initialized[cn] = 1;
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: initializing ssh_input", cn);
        buffer_init(&ssh_input[cn]);
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: initializing ssh_output", cn);
        buffer_init(&ssh_output[cn]);
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: initializing outgoing_packet", cn);
        buffer_init(&outgoing_packet[cn]);
        debugl7(SYSLOG_LEVEL_DEBUG3, "connection %d: initializing incoming_packet", cn);
        buffer_init(&incoming_packet[cn]);
        TAILQ_INIT(&outgoing[cn]);
    }
}

/* Returns 1 if remote host is connected via socket, 0 if not. */

int
packet_connection_is_on_socket(int cn)
{
    struct sockaddr_storage from, to;
    socklen_t fromlen, tolen;

    /* filedescriptors in and out are the same, so it's a socket */
    if (connection_in[cn] == connection_out[cn])
        return 1;
    fromlen = sizeof(from);
    memset(&from, 0, sizeof(from));
    if (getpeername(connection_in[cn], (struct sockaddr *)&from, (int *) &fromlen) < 0)
        return 0;
    tolen = sizeof(to);
    memset(&to, 0, sizeof(to));
    if (getpeername(connection_out[cn], (struct sockaddr *)&to, (int *) &tolen) < 0)
        return 0;
    if (fromlen != tolen || memcmp(&from, &to, fromlen) != 0)
        return 0;
    if (from.ss_family != AF_INET && from.ss_family != AF_INET6)
        return 0;
    return 1;
}

/*
 * Exports an IV from the CipherContext required to export the key
 * state back from the unprivileged child to the privileged parent
 * process.
 */

void
packet_get_keyiv(int cn, int mode, u_char *iv, u_int len)
{
    CipherContext *cc;

    if (mode == MODE_OUT)
        cc = &send_context[cn];
    else
        cc = &receive_context[cn];

    cipher_get_keyiv(cc, iv, len);
}

int
packet_get_keycontext(int cn, int mode, u_char *dat)
{
    CipherContext *cc;

    if (mode == MODE_OUT)
        cc = &send_context[cn];
    else
        cc = &receive_context[cn];

    return (cipher_get_keycontext(cc, dat));
}

void
packet_set_keycontext(int cn, int mode, u_char *dat)
{
    CipherContext *cc;

    if (mode == MODE_OUT)
        cc = &send_context[cn];
    else
        cc = &receive_context[cn];

    cipher_set_keycontext(cc, dat);
}

int
packet_get_keyiv_len(int cn, int mode)
{
    CipherContext *cc;

    if (mode == MODE_OUT)
        cc = &send_context[cn];
    else
        cc = &receive_context[cn];

    return (cipher_get_keyiv_len(cc));
}
void
packet_set_iv(int cn, int mode, u_char *dat)
{
    CipherContext *cc;

    if (mode == MODE_OUT)
        cc = &send_context[cn];
    else
        cc = &receive_context[cn];

    cipher_set_keyiv(cc, dat);
}
int
packet_get_ssh1_cipher(int cn)
{
    return (cipher_get_number(receive_context[cn].cipher));
}

void
packet_get_state(int cn, int mode, u_int32_t *seqnr, u_int64_t *blocks, u_int32_t *packets)
{
    struct packet_state *state;

    state = (mode == MODE_IN) ? &p_read[cn] : &p_send[cn];
    *seqnr = state->seqnr;
    *blocks = state->blocks;
    *packets = state->packets;
}

void
packet_set_state(int cn, int mode, u_int32_t seqnr, u_int64_t blocks, u_int32_t packets)
{
    struct packet_state *state;

    state = (mode == MODE_IN) ? &p_read[cn] : &p_send[cn];
    state->seqnr = seqnr;
    state->blocks = blocks;
    state->packets = packets;
}

/* returns 1 if connection is via ipv4 */

int
packet_connection_is_ipv4(int cn)
{
    struct sockaddr_storage to;
    socklen_t tolen = sizeof(to);

    memset(&to, 0, sizeof(to));
    if (getsockname(connection_out[cn], (struct sockaddr *)&to, (int *) &tolen) < 0)
        return 0;
    if (to.ss_family == AF_INET)
        return 1;
#ifdef IPV4_IN_IPV6
    if (to.ss_family == AF_INET6 &&
        IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6 *)&to)->sin6_addr))
        return 1;
#endif
    return 0;
}

/* Sets the connection into non-blocking mode. */

void
packet_set_nonblocking(int cn)
{
    /* Set the socket into non-blocking mode. */
    set_nonblock(connection_in[cn]);

    if (connection_out[cn] != connection_in[cn])
        set_nonblock(connection_out[cn]);
}

/* Returns the socket used for reading. */

int
packet_get_connection_in(int cn)
{
    return connection_in[cn];
}

/* Returns the descriptor used for writing. */

int
packet_get_connection_out(int cn)
{
    return connection_out[cn];
}

/* Closes the connection and clears and frees internal data structures. */

void
packet_close(int cn)
{
    int             mode;
    Enc             *enc;
    Mac             *mac;
    Comp            *comp;
    struct packet   *p;
    Buffer          b;

    if (!initialized[cn])
        return;
    initialized[cn] = 0;
    if (inout_v1[cn] > -1)
    {
      debugl7(SYSLOG_LEVEL_OPEN, "shutdown of inout_v1[%d] %d, type %d",
              cn, inout_v1[cn], SHUT_RDWR);
      shutdown(inout_v1[cn], SHUT_RDWR);
      debugl7(SYSLOG_LEVEL_OPEN, "close of inout_v1[%d] %d",
              cn, inout_v1[cn]);
      close(inout_v1[cn]);
      inout_v1[cn] = -1;
    }
    if (connection_in[cn] == connection_out[cn]  && connection_in[cn] != -1) {
        debugl7(SYSLOG_LEVEL_OPEN, "shutdown of connection_in[%d]/connection_out[%d] %d, type %d",
                cn, cn, connection_out[cn], SHUT_RDWR);
        shutdown(connection_out[cn], SHUT_RDWR);
        debugl7(SYSLOG_LEVEL_OPEN, "close of connection_in[%d]/connection_out[%d] %d",
                cn, cn, connection_out[cn]);
        close(connection_out[cn]);
    } else {
        debugl7(SYSLOG_LEVEL_OPEN, "close of connection_in[%d] %d, connection_out[%d] %d",
                cn, connection_in[cn], cn, connection_out[cn]);
        close(connection_in[cn]);
        close(connection_out[cn]);
    }
    buffer_free(&ssh_input[cn]);
    buffer_free(&ssh_output[cn]);
    buffer_free(&outgoing_packet[cn]);
    buffer_free(&incoming_packet[cn]);

    while ((p = TAILQ_FIRST(&outgoing[cn])))
    {
        memcpy(&b, &p->payload, sizeof(Buffer));
        buffer_free(&b);
        TAILQ_REMOVE(&outgoing[cn], p, next);
        xfree(p);
    }

    if (compression_buffer_ready[cn]) {
        buffer_free(&compression_buffer[cn]);
        compression_buffer_ready[cn] = 0;
        buffer_compress_uninit(cn);
    }
    cipher_cleanup(&send_context[cn]);
    cipher_cleanup(&receive_context[cn]);
    memset(&p_read[cn], 0, sizeof(struct packet_state));
    memset(&p_send[cn], 0, sizeof(struct packet_state));
    packet_compression[cn] = 0;
    interactive_mode[cn] = 0;
    server_side[cn] = 0;
    after_authentication[cn] = 0;
    remote_protocol_flags[cn] = 0;
    poll2_packet_length[cn] = 0;

    for (mode = 0; mode < MODE_MAX; mode++)
    {
      if (newkeys[cn][mode] != NULL)
      {
        enc  = &newkeys[cn][mode]->enc;
        mac  = &newkeys[cn][mode]->mac;
        comp = &newkeys[cn][mode]->comp;

        if (enc->name != NULL)
        {
            xfree(enc->name);
        }
        if (enc->iv != NULL)
        {
            xfree(enc->iv);
        }
        if (enc->key != NULL)
        {
            xfree(enc->key);
        }
        if (mac->name != NULL)
        {
            xfree(mac->name);
        }
        if (mac->key != NULL)
        {
            xfree(mac->key);
        }
        if (comp->name != NULL)
        {
            xfree(comp->name);
        }

        xfree(newkeys[cn][mode]);
        newkeys[cn][mode] = NULL;
      }

      if (current_keys[cn][mode] != NULL)
      {
          enc  = &current_keys[cn][mode]->enc;
          mac  = &current_keys[cn][mode]->mac;
          comp = &current_keys[cn][mode]->comp;

          if (enc->name != NULL)
          {
              xfree(enc->name);
          }
          if (enc->iv != NULL)
          {
              xfree(enc->iv);
          }
          if (enc->key != NULL)
          {
              xfree(enc->key);
          }
          if (mac->name != NULL)
          {
              xfree(mac->name);
          }
          if (mac->key != NULL)
          {
              xfree(mac->key);
          }
          if (comp->name != NULL)
          {
              xfree(comp->name);
          }

          xfree(current_keys[cn][mode]);
          current_keys[cn][mode] = NULL;
      }
    }

    if (xxx_kex[cn] != NULL)
    {
      for (mode = 0; mode < MODE_MAX; mode++)
      {
          if (xxx_kex[cn]->newkeys[mode] != NULL)
          {
            enc  = &xxx_kex[cn]->newkeys[mode]->enc;
            mac  = &xxx_kex[cn]->newkeys[mode]->mac;
            comp = &xxx_kex[cn]->newkeys[mode]->comp;

            if (enc->name != NULL)
            {
                xfree(enc->name);
            }
            if (enc->iv != NULL)
            {
                xfree(enc->iv);
            }
            if (enc->key != NULL)
            {
                xfree(enc->key);
            }
            if (mac->name != NULL)
            {
                xfree(mac->name);
            }
            if (mac->key != NULL)
            {
                xfree(mac->key);
            }
            if (comp->name != NULL)
            {
                xfree(comp->name);
            }

            xfree(xxx_kex[cn]->newkeys[mode]);
            xxx_kex[cn]->newkeys[mode] = NULL;
          }
      }

      if (xxx_kex[cn]->name != NULL)
      {
        xfree(xxx_kex[cn]->name);
        xxx_kex[cn]->name = NULL;
      }

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

    if (sshc_server_version_string[cn] != NULL)
    {
      xfree(sshc_server_version_string[cn]);
      sshc_server_version_string[cn] = NULL;
    }
    if (sshc_client_version_string[cn] != NULL)
    {
      xfree(sshc_client_version_string[cn]);
      sshc_client_version_string[cn] = NULL;
    }

    if (sshd_my[cn] != NULL)
    {
        kex_prop_free(sshd_my[cn]);
        sshd_my[cn] = NULL;
    }

    if (sshd_peer[cn] != NULL)
    {
        kex_prop_free(sshd_peer[cn]);
        sshd_peer[cn] = NULL;
    }

    if (sshd_kex_dh[cn] != NULL)
    {
        DH_free(sshd_kex_dh[cn]);
        sshd_kex_dh[cn] = NULL;
    }

    if (sshd_kexgex_dh[cn] != NULL)
    {
        DH_free(sshd_kexgex_dh[cn]);
        sshd_kexgex_dh[cn] = NULL;
    }

    if (sshd_dh_client_pub[cn] != NULL)
    {
        BN_clear_free(sshd_dh_client_pub[cn]);
        sshd_dh_client_pub[cn] = NULL;
    }

    if (sshd_kexgex_dh_client_pub[cn] != NULL)
    {
        BN_clear_free(sshd_kexgex_dh_client_pub[cn]);
        sshd_kexgex_dh_client_pub[cn] = NULL;
    }

    compat20[cn] = 0;
    compat13[cn] = 0;
}

/* Sets remote side protocol flags. */

void
packet_set_protocol_flags(int cn, u_int protocol_flags)
{
    remote_protocol_flags[cn] = protocol_flags;
}

/* Returns the remote protocol flags set earlier by the above function. */

u_int
packet_get_protocol_flags(int cn)
{
    return remote_protocol_flags[cn];
}

/*
 * Starts packet compression from the next packet on in both directions.
 * Level is compression level 1 (fastest) - 9 (slow, best) as in gzip.
 */

void
packet_init_compression(int cn)
{
    if (compression_buffer_ready[cn] == 1)
        return;
    compression_buffer_ready[cn] = 1;
    buffer_init(&compression_buffer[cn]);
}

void
packet_start_compression(int cn, int level)
{
    if (packet_compression[cn] && !compat20[cn])
        fatal_cn(cn, "connection %d: Compression already enabled.", cn);
    packet_compression[cn] = 1;
    packet_init_compression(cn);
    buffer_compress_init_send(cn, level);
    buffer_compress_init_recv(cn);
}

/*
 * Causes any further packets to be encrypted using the given key.  The same
 * key is used for both sending and reception.  However, both directions are
 * encrypted independently of each other.
 */

void
packet_set_encryption_key(int cn, const u_char *key, u_int keylen,
    int number)
{
    Cipher *cipher = cipher_by_number(number);

    if (cipher == NULL)
        fatal_cn(cn, "connection %d: packet_set_encryption_key: unknown cipher number %d", cn, number);
    if (keylen < 20)
        fatal_cn(cn, "connection %d: packet_set_encryption_key: keylen too small: %d", cn, keylen);
    if (keylen > SSH_SESSION_KEY_LENGTH)
        fatal_cn(cn, "connection %d: packet_set_encryption_key: keylen too big: %d", cn, keylen);
    memcpy(ssh1_key[cn], key, keylen);
    ssh1_keylen[cn] = keylen;
    cipher_init(&send_context[cn], cipher, key, keylen, NULL, 0, CIPHER_ENCRYPT);
    cipher_init(&receive_context[cn], cipher, key, keylen, NULL, 0, CIPHER_DECRYPT);
}

u_int
packet_get_encryption_key(int cn, u_char *key)
{
    if (key == NULL)
        return (ssh1_keylen[cn]);
    memcpy(key, ssh1_key[cn], ssh1_keylen[cn]);
    return (ssh1_keylen[cn]);
}

/* Start constructing a packet to send. */
void
packet_start(int cn, u_char type)
{
    u_char buf[9];
    int len;

    len = compat20[cn] ? 6 : 9;
    memset(buf, 0, len - 1);
    buf[len - 1] = type;
    DBG(debug("connection %d: packet_start[%d], len %d", cn, type, len));
    buffer_clear(&outgoing_packet[cn]);
    buffer_append(&outgoing_packet[cn], buf, len);
}

/* Append payload. */
void
packet_put_char(int cn, int value)
{
    char ch = value;

    buffer_append(&outgoing_packet[cn], &ch, 1);
}
void
packet_put_int(int cn, u_int value)
{
    buffer_put_int(&outgoing_packet[cn], value);
}
void
packet_put_string(int cn, const void *buf, u_int len)
{
    buffer_put_string(&outgoing_packet[cn], buf, len);
}
void
packet_put_cstring(int cn, const char *str)
{
    buffer_put_cstring(&outgoing_packet[cn], str);
}
void
packet_put_raw(int cn, const void *buf, u_int len)
{
    buffer_append(&outgoing_packet[cn], buf, len);
}
void
packet_put_bignum(int cn, BIGNUM * value)
{
    buffer_put_bignum(&outgoing_packet[cn], value);
}
void
packet_put_bignum2(int cn, BIGNUM * value)
{
    buffer_put_bignum2(&outgoing_packet[cn], value);
}

/*
 * Finalizes and sends the packet.  If the encryption key has been set,
 * encrypts the packet before sending.
 */

void
packet_send1(int cn)
{
    u_char buf[8], *cp;
    int i, padding, len;
    u_int checksum;
    u_int32_t rnd = 0;

    /*
     * If using packet compression, compress the payload of the outgoing
     * packet.
     */
    if (packet_compression[cn]) {
        debugl7(SYSLOG_LEVEL_PACKET, "connection %d: compression on", cn);
        buffer_clear(&compression_buffer[cn]);
        /* Skip padding. */
        buffer_consume(&outgoing_packet[cn], 8);
        /* padding */
        buffer_append(&compression_buffer[cn], "\0\0\0\0\0\0\0\0", 8);
        buffer_compress(cn, &outgoing_packet[cn], &compression_buffer[cn]);
        buffer_clear(&outgoing_packet[cn]);
        buffer_append(&outgoing_packet[cn], buffer_ptr(&compression_buffer[cn]),
            buffer_len(&compression_buffer[cn]));
    }
    /* Compute packet length without padding (add checksum, remove padding). */
    len = buffer_len(&outgoing_packet[cn]) + 4 - 8;
    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: packet len w/o padding %d", cn, len);

    /* Insert padding. Initialized to zero in packet_start1() */
    padding = 8 - len % 8;
    if (!send_context[cn].plaintext) {
        cp = buffer_ptr(&outgoing_packet[cn]);
        for (i = 0; i < padding; i++) {
            if (i % 4 == 0)
                rnd = arc4random();
            cp[7 - i] = rnd & 0xff;
            rnd >>= 8;
        }
    }
    buffer_consume(&outgoing_packet[cn], 8 - padding);

    /* Add check bytes. */
    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: adding check bytes to packet", cn);
    checksum = ssh_crc32(buffer_ptr(&outgoing_packet[cn]),
        buffer_len(&outgoing_packet[cn]));
    PUT_32BIT(buf, checksum);
    buffer_append(&outgoing_packet[cn], buf, 4);

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: packet_send plain: ", cn);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&outgoing_packet[cn]);
    }

    /* Append to ssh_output. */
    PUT_32BIT(buf, len);
    buffer_append(&ssh_output[cn], buf, 4);
    cp = buffer_append_space(&ssh_output[cn], buffer_len(&outgoing_packet[cn]));
    cipher_crypt(&send_context[cn], cp, buffer_ptr(&outgoing_packet[cn]),
        buffer_len(&outgoing_packet[cn]));

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: encrypted: ", cn);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&ssh_output[cn]);
    }

    buffer_clear(&outgoing_packet[cn]);

    /*
     * Note that the packet is now only buffered in ssh_output.  It won't be
     * actually sent until packet_write_wait or packet_write_poll is
     * called.
     */
}

void
set_newkeys(int cn, int mode)
{
    Enc *enc;
    Mac *mac;
    Comp *comp;
    CipherContext *cc;
    u_int64_t *max_blocks;
    int crypt_type;

    debug2("connection %d: set_newkeys: mode %d", cn, mode);

    if (mode == MODE_OUT) {
        cc = &send_context[cn];
        crypt_type = CIPHER_ENCRYPT;
        p_send[cn].packets = p_send[cn].blocks = 0;
        max_blocks = &max_blocks_out[cn];
    } else {
        cc = &receive_context[cn];
        crypt_type = CIPHER_DECRYPT;
        p_read[cn].packets = p_read[cn].blocks = 0;
        max_blocks = &max_blocks_in[cn];
    }
    if (newkeys[cn][mode] != NULL) {
        debug("connection %d: set_newkeys: rekeying", cn);
        cipher_cleanup(cc);
        enc  = &newkeys[cn][mode]->enc;
        mac  = &newkeys[cn][mode]->mac;
        comp = &newkeys[cn][mode]->comp;
        memset(mac->key, 0, mac->key_len);
        xfree(enc->name);
        xfree(enc->iv);
        xfree(enc->key);
        xfree(mac->name);
        xfree(mac->key);
        xfree(comp->name);
        xfree(newkeys[cn][mode]);
    }
    newkeys[cn][mode] = kex_get_newkeys(cn, mode);
    if (newkeys[cn][mode] == NULL)
        fatal_cn(cn, "connection %d: newkeys: no keys for mode %d", cn, mode);
    enc  = &newkeys[cn][mode]->enc;
    mac  = &newkeys[cn][mode]->mac;
    comp = &newkeys[cn][mode]->comp;
    if (mac->md != NULL)
        mac->enabled = 1;
    DBG(debug("connection %d: cipher_init_context: %d", cn, mode));
    cipher_init(cc, enc->cipher, enc->key, enc->key_len,
        enc->iv, enc->block_size, crypt_type);
    /* Deleting the keys does not gain extra security */
    /* memset(enc->iv,  0, enc->block_size);
       memset(enc->key, 0, enc->key_len); */
    if ((comp->type == COMP_ZLIB ||
        (comp->type == COMP_DELAYED && after_authentication[cn])) &&
        comp->enabled == 0) {
        packet_init_compression(cn);
        if (mode == MODE_OUT)
            buffer_compress_init_send(cn, 6);
        else
            buffer_compress_init_recv(cn);
        comp->enabled = 1;
    }
    /*
     * The 2^(blocksize*2) limit is too expensive for 3DES,
     * blowfish, etc, so enforce a 1GB limit for small blocksizes.
     */
    if (enc->block_size >= 16)
        *max_blocks = (u_int64_t)1 << (enc->block_size*2);
    else
        *max_blocks = ((u_int64_t)1 << 30) / enc->block_size;
    if (rekey_limit[cn])
        *max_blocks = MIN(*max_blocks, rekey_limit[cn] / enc->block_size);
}

/*
 * Delayed compression for SSH2 is enabled after authentication:
 * This happans on the server side after a SSH2_MSG_USERAUTH_SUCCESS is sent,
 * and on the client side after a SSH2_MSG_USERAUTH_SUCCESS is received.
 */
void
packet_enable_delayed_compress(int cn)
{
    Comp *comp = NULL;
    int mode;

    /*
     * Remember that we are past the authentication step, so rekeying
     * with COMP_DELAYED will turn on compression immediately.
     */
    after_authentication[cn] = 1;
    for (mode = 0; mode < MODE_MAX; mode++) {
        comp = &newkeys[cn][mode]->comp;
        if (comp && !comp->enabled && comp->type == COMP_DELAYED) {
            packet_init_compression(cn);
            if (mode == MODE_OUT)
                buffer_compress_init_send(cn, 6);
            else
                buffer_compress_init_recv(cn);
            comp->enabled = 1;
        }
    }
}

/*
 * Finalize packet in SSH2 format (compress, mac, encrypt, enqueue)
 */
void
packet_send2_wrapped(int cn)
{
    u_char type, *cp, *macbuf = NULL;
    u_char padlen, pad;
    u_int packet_length = 0;
    u_int i, len;
    u_int32_t rnd = 0;
    Enc *enc   = NULL;
    Mac *mac   = NULL;
    Comp *comp = NULL;
    int block_size;

    if (newkeys[cn][MODE_OUT] != NULL) {
        enc  = &newkeys[cn][MODE_OUT]->enc;
        mac  = &newkeys[cn][MODE_OUT]->mac;
        comp = &newkeys[cn][MODE_OUT]->comp;
    }
    block_size = enc ? enc->block_size : 8;

    cp = buffer_ptr(&outgoing_packet[cn]);
    type = cp[5];

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: plain:     ", cn);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&outgoing_packet[cn]);
    }

    if (comp && comp->enabled) {
        len = buffer_len(&outgoing_packet[cn]);
        /* skip header, compress only payload */
        buffer_consume(&outgoing_packet[cn], 5);
        buffer_clear(&compression_buffer[cn]);
        buffer_compress(cn, &outgoing_packet[cn], &compression_buffer[cn]);
        buffer_clear(&outgoing_packet[cn]);
        buffer_append(&outgoing_packet[cn], "\0\0\0\0\0", 5);
        buffer_append(&outgoing_packet[cn], buffer_ptr(&compression_buffer[cn]),
            buffer_len(&compression_buffer[cn]));
        DBG(debug("connection %d: compression: raw %d compressed %d", cn, len,
            buffer_len(&outgoing_packet[cn])));
    }

    /* sizeof (packet_len + pad_len + payload) */
    len = buffer_len(&outgoing_packet[cn]);

    /*
     * calc size of padding, alloc space, get random data,
     * minimum padding is 4 bytes
     */
    padlen = block_size - (len % block_size);
    if (padlen < 4)
        padlen += block_size;
    if (extra_pad[cn]) {
        /* will wrap if extra_pad+padlen > 255 */
        extra_pad[cn]  = roundup(extra_pad[cn], block_size);
        pad = extra_pad[cn] - ((len + padlen) % extra_pad[cn]);
        debug3("connection %d: packet_send2: adding %d (len %d padlen %d extra_pad %d)",
            cn, pad, len, padlen, extra_pad[cn]);
        padlen += pad;
        extra_pad[cn] = 0;
    }
    cp = buffer_append_space(&outgoing_packet[cn], padlen);
    if (enc && !send_context[cn].plaintext) {
        /* random padding */
        for (i = 0; i < padlen; i++) {
            if (i % 4 == 0)
                rnd = arc4random();
            cp[i] = rnd & 0xff;
            rnd >>= 8;
        }
    } else {
        /* clear padding */
        memset(cp, 0, padlen);
    }
    /* packet_length includes payload, padding and padding length field */
    packet_length = buffer_len(&outgoing_packet[cn]) - 4;
    cp = buffer_ptr(&outgoing_packet[cn]);
    PUT_32BIT(cp, packet_length);
    cp[4] = padlen;
    DBG(debug("connection %d: send: len %d (includes padlen %d)", cn, packet_length+4, padlen));

    /* compute MAC over seqnr and packet(length fields, payload, padding) */
    if (mac && mac->enabled) {
        macbuf = mac_compute(mac, p_send[cn].seqnr,
            buffer_ptr(&outgoing_packet[cn]),
            buffer_len(&outgoing_packet[cn]));
        DBG(debug("connection %d: done calc MAC out #%lx", cn, (unsigned long) p_send[cn].seqnr));
    }
    /* encrypt packet and append to ssh_output buffer. */
    cp = buffer_append_space(&ssh_output[cn], buffer_len(&outgoing_packet[cn]));
    cipher_crypt(&send_context[cn], cp, buffer_ptr(&outgoing_packet[cn]),
        buffer_len(&outgoing_packet[cn]));
    /* append unencrypted MAC */
    if (mac && mac->enabled)
        buffer_append(&ssh_output[cn], (char *)macbuf, mac->mac_len);
    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: encrypted: ", cn);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&ssh_output[cn]);
    }
    /* increment sequence number for outgoing packets */
    if (++p_send[cn].seqnr == 0)
        logit("connection %d: outgoing seqnr wraps around", cn);
    if (++p_send[cn].packets == 0)
        if (!(datafellows[cn] & SSH_BUG_NOREKEY))
            fatal_cn(cn, "connection %d: XXX too many packets with same key", cn);
    p_send[cn].blocks += (packet_length + 4) / block_size;
    buffer_clear(&outgoing_packet[cn]);

    if (type == SSH2_MSG_NEWKEYS)
        set_newkeys(cn, MODE_OUT);
    else if (type == SSH2_MSG_USERAUTH_SUCCESS && server_side[cn])
        packet_enable_delayed_compress(cn);
}

void
packet_send2(int cn)
{
    static int rekeying[L7_OPENSSH_MAX_CONNECTIONS] = {0};
    struct packet *p;
    u_char type, *cp;

    cp = buffer_ptr(&outgoing_packet[cn]);
    type = cp[5];

    /* during rekeying we can only send key exchange messages */
    if (rekeying[cn]) {
        if (!((type >= SSH2_MSG_TRANSPORT_MIN) &&
            (type <= SSH2_MSG_TRANSPORT_MAX))) {
            debug("connection %d: enqueue packet: %u", cn, type);
            p = xmalloc(sizeof(*p));
            p->type = type;
            memcpy(&p->payload, &outgoing_packet[cn], sizeof(Buffer));
            buffer_init(&outgoing_packet[cn]);
            TAILQ_INSERT_TAIL(&outgoing[cn], p, next);
            return;
        }
    }

    /* rekeying starts with sending KEXINIT */
    if (type == SSH2_MSG_KEXINIT)
        rekeying[cn] = 1;

    packet_send2_wrapped(cn);

    /* after a NEWKEYS message we can send the complete queue */
    if (type == SSH2_MSG_NEWKEYS) {
        rekeying[cn] = 0;
        while ((p = TAILQ_FIRST(&outgoing[cn]))) {
            type = p->type;
            debug("connection %d: dequeue packet: %u", cn, type);
            buffer_free(&outgoing_packet[cn]);
            memcpy(&outgoing_packet[cn], &p->payload,
                sizeof(Buffer));
            TAILQ_REMOVE(&outgoing[cn], p, next);
            xfree(p);
            packet_send2_wrapped(cn);
        }
    }
}

void
packet_send(int cn)
{
    if (compat20[cn])
    {
        debugl7(SYSLOG_LEVEL_PACKET, "connection %d: packet_send2", cn);
        packet_send2(cn);
    }
    else
    {
        debugl7(SYSLOG_LEVEL_PACKET, "connection %d: packet_send1", cn);
        packet_send1(cn);
    }
    debugl7(SYSLOG_LEVEL_DEBUG1, "connection %d: packet_send done", cn);
}

/*
 * Waits until a packet has been received, and returns its type.  Note that
 * no other data is processed until this returns, so this function should not
 * be used during the interactive session.
 */

int
packet_read_seqnr(int cn, u_int32_t *seqnr_p)
{
    int type, len;
    fd_set fdset;
    char *buf;

    sshd_packet_buf[cn] = buf = xmalloc(PACKET_BUFFER_SIZE); /* removed from stack */
    DBG(debug("connection %d: packet_read", cn));

    /* Since we are blocking, ensure that all written packets have been sent. */
    packet_write_wait(cn);

    /* Stay in the loop until we have received a complete packet. */
    for (;;)
    {
        struct timeval tv;
        int select_rc;

        memset(&tv, 0, sizeof(struct timeval));
        tv.tv_sec = 1;

        /* Try to read a packet from the buffer. */
        type = packet_read_poll_seqnr(cn, seqnr_p);
        if (!compat20[cn] && (
            type == SSH_SMSG_SUCCESS
            || type == SSH_SMSG_FAILURE
            || type == SSH_CMSG_EOF
            || type == SSH_CMSG_EXIT_CONFIRMATION))
            packet_check_eom(cn);
        /* If we got a packet, return it. */
        if (type != SSH_MSG_NONE) {
            xfree(buf);
            sshd_packet_buf[cn] = NULL;
            return type;
        }
        /*
         * Otherwise, wait for some data to arrive, add it to the
         * buffer, and try again.
         */
        memset(&fdset, 0, sizeof(fd_set));
        FD_SET(connection_in[cn], &fdset);

        /* Wait for some data to arrive. */
        while ((select_rc = select(connection_in[cn] + 1, &fdset, NULL, NULL, &tv)) <= 0)
        {
            memset(&tv, 0, sizeof(struct timeval));
            tv.tv_sec = 1;

            if (connection_in[cn] == -1)
            {
                logit("connection %d socket %d closed", cn, connection_in[cn]);
                xfree(buf);
                sshd_packet_buf[cn] = NULL;
                return SSH_MSG_DISCONNECT;
            }

            memset(&fdset, 0, sizeof(fd_set));
            FD_SET(connection_in[cn], &fdset);

            if ((select_rc == -1) && (errno == EAGAIN || errno == EINTR))
            {
                continue;
            }

            if (select_rc == 0)
            {
                continue;
            }

            logit("connection %d socket %d select error %d", cn, connection_in[cn], errno);
            xfree(buf);
            sshd_packet_buf[cn] = NULL;
            return SSH_MSG_DISCONNECT;
        }

        /* Read data from the socket. */
        len = read(connection_in[cn], buf, PACKET_BUFFER_SIZE);
        debugl7(SYSLOG_LEVEL_READ, "read data on connection_in[%d] %d, %d bytes", cn,
                connection_in[cn], len);
        if (len == 0) {
            logit("connection %d socket %d closed", cn, connection_in[cn]);
            xfree(buf);
            sshd_packet_buf[cn] = NULL;
            return SSH_MSG_DISCONNECT;
        }
        if (len < 0)
        {
          logit("connection %d: Read from socket %d failed: %.100s", cn, connection_in[cn], strerror(errno));
          xfree(buf);
          sshd_packet_buf[cn] = NULL;
          return SSH_MSG_DISCONNECT;
        }
        /* Append it to the buffer. */
        packet_process_incoming(cn, buf, len);
    }
    /* NOTREACHED */
}

int
packet_read(int cn)
{
    return packet_read_seqnr(cn, NULL);
}

/*
 * Waits until a packet has been received, verifies that its type matches
 * that given, and gives a fatal error and exits if there is a mismatch.
 */

void
packet_read_expect(int cn, int expected_type)
{
    int type;

    type = packet_read(cn);
    if (type != expected_type)
        packet_disconnect(cn, "connection %d: Protocol error: expected packet type %d, got %d",
            cn, expected_type, type);
}

/* Checks if a full packet is available in the data received so far via
 * packet_process_incoming.  If so, reads the packet; otherwise returns
 * SSH_MSG_NONE.  This does not wait for data from the connection.
 *
 * SSH_MSG_DISCONNECT is handled specially here.  Also,
 * SSH_MSG_IGNORE messages are skipped by this function and are never returned
 * to higher levels.
 */

int
packet_read_poll1(int cn)
{
    u_int len, padded_len;
    u_char *cp, type;
    u_int checksum, stored_checksum;

    /* Check if ssh_input size is less than minimum packet size. */
    if (buffer_len(&ssh_input[cn]) < 4 + 8)
        return SSH_MSG_NONE;
    /* Get length of incoming packet. */
    cp = buffer_ptr(&ssh_input[cn]);
    len = GET_32BIT(cp);
    if (len < 1 + 2 + 2 || len > 256 * 1024)
        packet_disconnect(cn, "connection %d: Bad packet length %u.", cn, len);
    padded_len = (len + 8) & ~7;

    /* Check if the packet has been entirely received. */
    if (buffer_len(&ssh_input[cn]) < 4 + padded_len)
        return SSH_MSG_NONE;

    /* The entire packet is in buffer. */

    /* Consume packet length. */
    buffer_consume(&ssh_input[cn], 4);

    /*
     * Cryptographic attack detector for ssh
     * (C)1998 CORE-SDI, Buenos Aires Argentina
     * Ariel Futoransky(futo@core-sdi.com)
     */
    if (!receive_context[cn].plaintext &&
        detect_attack(buffer_ptr(&ssh_input[cn]), padded_len, NULL) == DEATTACK_DETECTED)
        packet_disconnect(cn, "connection %d: crc32 compensation attack: network attack detected", cn);

    /* Decrypt data to incoming_packet. */
    buffer_clear(&incoming_packet[cn]);
    cp = buffer_append_space(&incoming_packet[cn], padded_len);
    cipher_crypt(&receive_context[cn], cp, buffer_ptr(&ssh_input[cn]), padded_len);

    buffer_consume(&ssh_input[cn], padded_len);

    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: read_poll plain: ", cn);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&incoming_packet[cn]);
    }

    /* Compute packet checksum. */
    checksum = ssh_crc32(buffer_ptr(&incoming_packet[cn]),
        buffer_len(&incoming_packet[cn]) - 4);

    /* Skip padding. */
    buffer_consume(&incoming_packet[cn], 8 - len % 8);

    /* Test check bytes. */
    if (len != buffer_len(&incoming_packet[cn]))
        packet_disconnect(cn, "connection %d: packet_read_poll1: len %d != buffer_len %d.",
            cn, len, buffer_len(&incoming_packet[cn]));

    cp = (u_char *)buffer_ptr(&incoming_packet[cn]) + len - 4;
    stored_checksum = GET_32BIT(cp);
    if (checksum != stored_checksum)
        packet_disconnect(cn, "connection %d: Corrupted check bytes on input.", cn);
    buffer_consume_end(&incoming_packet[cn], 4);

    if (packet_compression[cn]) {
        buffer_clear(&compression_buffer[cn]);
        buffer_uncompress(cn, &incoming_packet[cn], &compression_buffer[cn]);
        buffer_clear(&incoming_packet[cn]);
        buffer_append(&incoming_packet[cn], buffer_ptr(&compression_buffer[cn]),
            buffer_len(&compression_buffer[cn]));
    }
    type = buffer_get_char(&incoming_packet[cn]);
    if (type < SSH_MSG_MIN || type > SSH_MSG_MAX)
        packet_disconnect(cn, "connection %d: Invalid ssh1 packet type: %d", cn, type);
    return type;
}

int
packet_read_poll2(int cn, u_int32_t *seqnr_p)
{
    u_int padlen, need;
    u_char *macbuf, *cp, type;
    u_int maclen, block_size;
    Enc *enc   = NULL;
    Mac *mac   = NULL;
    Comp *comp = NULL;

    if (newkeys[cn][MODE_IN] != NULL) {
        enc  = &newkeys[cn][MODE_IN]->enc;
        mac  = &newkeys[cn][MODE_IN]->mac;
        comp = &newkeys[cn][MODE_IN]->comp;
    }
    maclen = mac && mac->enabled ? mac->mac_len : 0;
    block_size = enc ? enc->block_size : 8;

    if (poll2_packet_length[cn] == 0) {
        /*
         * check if ssh_input size is less than the cipher block size,
         * decrypt first block and extract length of incoming packet
         */
        if (buffer_len(&ssh_input[cn]) < block_size)
            return SSH_MSG_NONE;
        buffer_clear(&incoming_packet[cn]);
        cp = buffer_append_space(&incoming_packet[cn], block_size);
        cipher_crypt(&receive_context[cn], cp, buffer_ptr(&ssh_input[cn]),
            block_size);
        cp = buffer_ptr(&incoming_packet[cn]);
        poll2_packet_length[cn] = GET_32BIT(cp);
        if (poll2_packet_length[cn] < 1 + 4 || poll2_packet_length[cn] > 256 * 1024) {
            if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
            {
                buffer_dump(&incoming_packet[cn]);
            }
            packet_disconnect(cn, "connection %d: Bad packet length %u.", cn, poll2_packet_length[cn]);
        }
        DBG(debug("connection %d: ssh_input: packet len %u", cn, poll2_packet_length[cn]+4));
        buffer_consume(&ssh_input[cn], block_size);
    }
    /* we have a partial packet of block_size bytes */
    need = 4 + poll2_packet_length[cn] - block_size;
    DBG(debug("connection %d: partial packet %d, need %d, maclen %d", cn, block_size,
        need, maclen));
    if (need % block_size != 0)
        fatal_cn(cn, "connection %d: padding error: need %d block %d mod %d",
            cn, need, block_size, need % block_size);
    /*
     * check if the entire packet has been received and
     * decrypt into incoming_packet
     */
    if (buffer_len(&ssh_input[cn]) < need + maclen)
        return SSH_MSG_NONE;
    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: read_poll enc/full: ", cn);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&ssh_input[cn]);
    }
    cp = buffer_append_space(&incoming_packet[cn], need);
    cipher_crypt(&receive_context[cn], cp, buffer_ptr(&ssh_input[cn]), need);
    buffer_consume(&ssh_input[cn], need);
    /*
     * compute MAC over seqnr and packet,
     * increment sequence number for incoming packet
     */
    if (mac && mac->enabled) {
        macbuf = mac_compute(mac, p_read[cn].seqnr,
            buffer_ptr(&incoming_packet[cn]),
            buffer_len(&incoming_packet[cn]));
        if (memcmp(macbuf, buffer_ptr(&ssh_input[cn]), mac->mac_len) != 0)
            packet_disconnect(cn, "connection %d: Corrupted MAC on input.", cn);
        DBG(debug("connection %d: MAC #0x%lx ok", cn, (unsigned long)p_read[cn].seqnr));
        buffer_consume(&ssh_input[cn], mac->mac_len);
    }
    if (seqnr_p != NULL)
        *seqnr_p = p_read[cn].seqnr;
    if (++p_read[cn].seqnr == 0)
        logit("connection %d: incoming seqnr wraps around", cn);
    if (++p_read[cn].packets == 0)
        if (!(datafellows[cn] & SSH_BUG_NOREKEY))
            fatal_cn(cn, "connection %d: XXX too many packets with same key", cn);
    p_read[cn].blocks += (poll2_packet_length[cn] + 4) / block_size;

    /* get padlen */
    cp = buffer_ptr(&incoming_packet[cn]);
    padlen = cp[4];
    DBG(debug("connection %d: input: padlen %d", cn, padlen));
    if (padlen < 4)
        packet_disconnect(cn, "connection %d: Corrupted padlen %d on input.", cn, padlen);

    /* skip packet size + padlen, discard padding */
    buffer_consume(&incoming_packet[cn], 4 + 1);
    buffer_consume_end(&incoming_packet[cn], padlen);

    DBG(debug("connection %d: input: len before de-compress %d", cn, buffer_len(&incoming_packet[cn])));
    if (comp && comp->enabled) {
        buffer_clear(&compression_buffer[cn]);
        buffer_uncompress(cn, &incoming_packet[cn], &compression_buffer[cn]);
        buffer_clear(&incoming_packet[cn]);
        buffer_append(&incoming_packet[cn], buffer_ptr(&compression_buffer[cn]),
            buffer_len(&compression_buffer[cn]));
        DBG(debug("connection %d: input: len after de-compress %d",
            cn, buffer_len(&incoming_packet[cn])));
    }
    /*
     * get packet type, implies consume.
     * return length of payload (without type field)
     */
    type = buffer_get_char(&incoming_packet[cn]);
    if (type < SSH2_MSG_MIN || type >= SSH2_MSG_LOCAL_MIN)
        packet_disconnect(cn, "connection %d: Invalid ssh2 packet type: %d", cn, type);
    if (type == SSH2_MSG_NEWKEYS)
        set_newkeys(cn, MODE_IN);
    else if (type == SSH2_MSG_USERAUTH_SUCCESS && !server_side[cn])
        packet_enable_delayed_compress(cn);
    debugl7(SYSLOG_LEVEL_PACKET, "connection %d: read/plain[%d]:\r\n", cn, type);
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        buffer_dump(&incoming_packet[cn]);
    }
    /* reset for next packet */
    poll2_packet_length[cn] = 0;
    return type;
}

int
packet_read_poll_seqnr(int cn, u_int32_t *seqnr_p)
{
    u_int reason, seqnr;
    u_char type;
    char *msg;

    for (;;) {
        if (compat20[cn]) {
            type = packet_read_poll2(cn, seqnr_p);
            if (type)
                DBG(debug("connection %d: received packet type %d", cn, type));
            switch (type) {
            case SSH2_MSG_IGNORE:
                break;
            case SSH2_MSG_DEBUG:
                packet_get_char(cn);
                msg = packet_get_string(cn, NULL);
                debug("Remote: %.900s", msg);
#ifdef L7_SSHD
                if (msg != NULL)
#endif /* L7_SSHD */
                xfree(msg);
                msg = packet_get_string(cn, NULL);
#ifdef L7_SSHD
                if (msg != NULL)
#endif /* L7_SSHD */
                xfree(msg);
                break;
            case SSH2_MSG_DISCONNECT:
                reason = packet_get_int(cn);
                msg = packet_get_string(cn, NULL);
#ifndef L7_SSHD
                logit("Received disconnect from %s: %u: %.400s",
                    get_remote_ipaddr(cn), reason, msg);
                xfree(msg);
                cleanup_exit_cn(cn, 255);
#else
                logit("Received disconnect: %u: %.400s",
                       reason, msg);
#ifdef L7_SSHD
                if (msg != NULL)
#endif /* L7_SSHD */
                xfree(msg);
                return type;
#endif /* L7_SSHD */
                break;
            case SSH2_MSG_UNIMPLEMENTED:
                seqnr = packet_get_int(cn);
                debug("Received SSH2_MSG_UNIMPLEMENTED for %u",
                    seqnr);
                break;
            default:
                return type;
                break;
            }
        } else {
            type = packet_read_poll1(cn);
            switch (type) {
            case SSH_MSG_IGNORE:
                break;
            case SSH_MSG_DEBUG:
                msg = packet_get_string(cn, NULL);
                debug("Remote: %.900s", msg);
#ifdef L7_SSHD
                if (msg != NULL)
#endif /* L7_SSHD */
                xfree(msg);
                break;
            case SSH_MSG_DISCONNECT:
                msg = packet_get_string(cn, NULL);
#ifndef L7_SSHD
                logit("Received disconnect from %s: %.400s",
                    get_remote_ipaddr(cn), msg);
                xfree(msg);
                cleanup_exit_cn(cn, 255);
#else
                logit("Received disconnect: %.400s",
                      msg);
#ifdef L7_SSHD
                if (msg != NULL)
#endif /* L7_SSHD */
                xfree(msg);
                return type;
#endif /* L7_SSHD */
                break;
            default:
                if (type)
                    DBG(debug("received packet type %d", type));
                return type;
                break;
            }
        }
    }
}

int
packet_read_poll(int cn)
{
    return packet_read_poll_seqnr(cn, NULL);
}

/*
 * Buffers the given amount of input characters.  This is intended to be used
 * together with packet_read_poll.
 */

void
packet_process_incoming(int cn, const char *buf, u_int len)
{
    buffer_append(&ssh_input[cn], buf, len);
}

/* Returns a character from the packet. */

u_int
packet_get_char(int cn)
{
    char ch;

    buffer_get(&incoming_packet[cn], &ch, 1);
    return (u_char) ch;
}

/* Returns an integer from the packet data. */

u_int
packet_get_int(int cn)
{
    return buffer_get_int(&incoming_packet[cn]);
}

/*
 * Returns an arbitrary precision integer from the packet data.  The integer
 * must have been initialized before this call.
 */

void
packet_get_bignum(int cn, BIGNUM * value)
{
    buffer_get_bignum(&incoming_packet[cn], value);
}

void
packet_get_bignum2(int cn, BIGNUM * value)
{
    buffer_get_bignum2(&incoming_packet[cn], value);
}

void *
packet_get_raw(int cn, u_int *length_ptr)
{
    u_int bytes = buffer_len(&incoming_packet[cn]);

    if (length_ptr != NULL)
        *length_ptr = bytes;
    return buffer_ptr(&incoming_packet[cn]);
}

int
packet_remaining(int cn)
{
    return buffer_len(&incoming_packet[cn]);
}

/*
 * Returns a string from the packet data.  The string is allocated using
 * xmalloc; it is the responsibility of the calling program to free it when
 * no longer needed.  The length_ptr argument may be NULL, or point to an
 * integer into which the length of the string is stored.
 */

void *
packet_get_string(int cn, u_int *length_ptr)
{
    return buffer_get_string(&incoming_packet[cn], length_ptr);
}

/*
 * Sends a diagnostic message from the server to the client.  This message
 * can be sent at any time (but not while constructing another message). The
 * message is printed immediately, but only if the client is being executed
 * in verbose mode.  These messages are primarily intended to ease debugging
 * authentication problems.   The length of the formatted message must not
 * exceed 1024 bytes.  This will automatically call packet_write_wait.
 */

void
packet_send_debug(int cn, const char *fmt,...)
{
    char buf[1024];
    va_list args;

    if (compat20[cn] && (datafellows[cn] & SSH_BUG_DEBUG))
        return;

    va_start(args, fmt);
    osapiVsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (compat20[cn]) {
        packet_start(cn, SSH2_MSG_DEBUG);
        packet_put_char(cn, 0); /* bool: always display */
        packet_put_cstring(cn, buf);
        packet_put_cstring(cn, "");
    } else {
        packet_start(cn, SSH_MSG_DEBUG);
        packet_put_cstring(cn, buf);
    }
    packet_send(cn);
    packet_write_wait(cn);
}

void
packet_exit(int cn)
{
    int remote_id, type;

    /* Send the exit message to the other side, and wait for it to get sent. */
    if (compat20[cn]) {
        if ((remote_id = channel_find_remote_id(cn)) != -1)
        {
            packet_start(cn, SSH2_MSG_CHANNEL_CLOSE);
            packet_put_int(cn, remote_id);
            packet_send(cn);
            packet_write_wait(cn);
        }
    } else {
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
    }

    channel_close_cn(cn);

    /* Close the connection. */
    cleanup_exit_cn(cn, 255);
}

/*
 * Logs the error plus constructs and sends a disconnect packet, closes the
 * connection, and exits.  This function never returns. The error message
 * should not contain a newline.  The length of the formatted message must
 * not exceed 1024 bytes.
 */

void
packet_disconnect(int cn, const char *fmt,...)
{
    char buf[256];
    va_list args;
    static int disconnecting[L7_OPENSSH_MAX_CONNECTIONS] = {0};

    if (disconnecting[cn])  /* Guard against recursive invocations. */
        fatal_cn(cn, "connection %d: packet_disconnect called recursively.", cn);
    disconnecting[cn] = 1;

    /*
     * Format the message.  Note that the caller must make sure the
     * message is of limited size.
     */
    va_start(args, fmt);
    osapiVsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    /* Display the error locally */
    logit("connection %d: Disconnecting: %.100s", cn, buf);

    /* Send the disconnect message to the other side, and wait for it to get sent. */
    if (compat20[cn]) {
        packet_start(cn, SSH2_MSG_DISCONNECT);
        packet_put_int(cn, SSH2_DISCONNECT_PROTOCOL_ERROR);
        packet_put_cstring(cn, buf);
        packet_put_cstring(cn, "");
    } else {
        packet_start(cn, SSH_MSG_DISCONNECT);
        packet_put_cstring(cn, buf);
    }
    packet_send(cn);
    packet_write_wait(cn);

    /* Stop listening for connections. */
    channel_close_cn(cn);

    /* Close the connection. */
    disconnecting[cn] = 0;
    cleanup_exit_cn(cn, 255);
}

/* Checks if there is any buffered ssh_output, and tries to write some of the output. */

void
packet_write_poll(int cn)
{
    int len = buffer_len(&ssh_output[cn]);
    int tmplen;

    if (len > 0) {
        tmplen = len;

        len = atomicio(vwrite, connection_out[cn], buffer_ptr(&ssh_output[cn]), len);
        debugl7(SYSLOG_LEVEL_WRITE, "wrote data on connection_out[%d] %d, %d of %d bytes", cn,
                connection_out[cn], len, tmplen);
        if (len <= 0) {
            if (errno == EAGAIN)
                return;
            else
                fatal_cn(cn, "connection %d: Write failed: %.100s", cn, strerror(errno));
        }
        buffer_consume(&ssh_output[cn], len);
    }
}

/*
 * Calls packet_write_poll repeatedly until all pending output data has been
 * written.
 */

void
packet_write_wait(int cn)
{
    fd_set fdset;

    packet_write_poll(cn);
    while (packet_have_data_to_write(cn)) {
        memset(&fdset, 0, sizeof(fd_set));
        FD_SET(connection_out[cn], &fdset);
        while (select(connection_out[cn] + 1, NULL, &fdset, NULL, NULL) == -1 &&
            (errno == EAGAIN || errno == EINTR))
            ;
        packet_write_poll(cn);
    }
}

/* Returns true if there is buffered data to write to the connection. */

int
packet_have_data_to_write(int cn)
{
    return buffer_len(&ssh_output[cn]) != 0;
}

/* Returns true if there is not too much data to write to the connection. */

int
packet_not_very_much_data_to_write(int cn)
{
    if (interactive_mode[cn])
        return buffer_len(&ssh_output[cn]) < 16384;
    else
        return buffer_len(&ssh_output[cn]) < 128 * 1024;
}


void
packet_set_tos(int cn, int interactive)
{
#if defined(IP_TOS) && !defined(IP_TOS_IS_BROKEN)
    int tos = interactive ? IPTOS_LOWDELAY : IPTOS_THROUGHPUT;

    if (!packet_connection_is_on_socket(cn) ||
        !packet_connection_is_ipv4(cn))
        return;
#if !defined(L7_SSHD) || !defined(_L7_OS_VXWORKS_) || (VX_VERSION==55)
    if (setsockopt(connection_in[cn], IPPROTO_IP, IP_TOS, &tos,
#else
    if (setsockopt(connection_in[cn], IPPROTO_IP, IP_TOS, (char*) &tos,
#endif
        sizeof(tos)) < 0)
        error("connection %dL setsockopt IP_TOS %d: %.100s:",
            cn, tos, strerror(errno));
#endif
}

/* Informs that the current session is interactive.  Sets IP flags for that. */

void
packet_set_interactive(int cn, int interactive)
{
    static int called = 0;

    if (called)
        return;
    called = 1;

    /* Record that we are in interactive mode. */
    interactive_mode[cn] = interactive;

    /* Only set socket options if using a socket.  */
    if (!packet_connection_is_on_socket(cn))
        return;
    if (interactive)
        set_nodelay(connection_in[cn]);
    packet_set_tos(cn, interactive);
}

/* Returns true if the current connection is interactive. */

int
packet_is_interactive(int cn)
{
    return interactive_mode[cn];
}

int
packet_set_maxsize(int cn, u_int s)
{
    static int called[L7_OPENSSH_MAX_CONNECTIONS] = {0};

    if (called[cn]) {
        logit("packet_set_maxsize: called twice: old %d new %d",
            max_packet_size[cn], s);
        return -1;
    }
    if (s < 4 * 1024 || s > 1024 * 1024) {
        logit("packet_set_maxsize: bad size %d", s);
        return -1;
    }
    called[cn] = 1;
    debug("connection %d: packet_set_maxsize: setting to %d", cn, s);
    max_packet_size[cn] = s;
    return s;
}

/* roundup current message to pad bytes */
void
packet_add_padding(int cn, u_char pad)
{
    extra_pad[cn] = pad;
}

/*
 * 9.2.  Ignored Data Message
 *
 *   byte      SSH_MSG_IGNORE
 *   string    data
 *
 * All implementations MUST understand (and ignore) this message at any
 * time (after receiving the protocol version). No implementation is
 * required to send them. This message can be used as an additional
 * protection measure against advanced traffic analysis techniques.
 */
void
packet_send_ignore(int cn, int nbytes)
{
    u_int32_t rnd = 0;
    int i;

    packet_start(cn, compat20[cn] ? SSH2_MSG_IGNORE : SSH_MSG_IGNORE);
    packet_put_int(cn, nbytes);
    for (i = 0; i < nbytes; i++) {
        if (i % 4 == 0)
            rnd = arc4random();
        packet_put_char(cn, rnd & 0xff);
        rnd >>= 8;
    }
}

#define MAX_PACKETS (1U<<31)
int
packet_need_rekeying(int cn)
{
    if (datafellows[cn] & SSH_BUG_NOREKEY)
        return 0;
    return
        (p_send[cn].packets > MAX_PACKETS) ||
        (p_read[cn].packets > MAX_PACKETS) ||
        (max_blocks_out[cn] && (p_send[cn].blocks > max_blocks_out[cn])) ||
        (max_blocks_in[cn]  && (p_read[cn].blocks > max_blocks_in[cn]));
}

void
packet_set_rekey_limit(int cn, u_int32_t bytes)
{
    rekey_limit[cn] = bytes;
}

void
packet_set_server(int cn)
{
    server_side[cn] = 1;
}

void
packet_set_authenticated(int cn)
{
    after_authentication[cn] = 1;
}
