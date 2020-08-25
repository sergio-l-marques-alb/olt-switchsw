/*  $OpenBSD: packet.h,v 1.43 2005/07/25 11:59:40 markus Exp $  */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Interface for the packet protocol functions.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef PACKET_H
#define PACKET_H

#include <openssl/bn.h>

#include "defaultconfig.h"

#define  SSHD_MAX_CONNECTIONS    FD_SSHD_MAX_SESSIONS
#define  SSH_MAX_CONNECTIONS     FD_SSHD_MAX_SESSIONS
#define  L7_OPENSSH_MAX_CONNECTIONS   SSHD_MAX_CONNECTIONS + SSH_MAX_CONNECTIONS

void     packet_set_connection(int, int, int);
void     packet_set_nonblocking(int);
int      packet_get_connection_in(int);
int      packet_get_connection_out(int);
void     packet_close(int);
void     packet_set_encryption_key(int, const u_char *, u_int, int);
u_int    packet_get_encryption_key(int, u_char *);
void     packet_set_protocol_flags(int, u_int);
u_int    packet_get_protocol_flags(int);
void     packet_start_compression(int, int);
void     packet_set_interactive(int, int);
int      packet_is_interactive(int);
void     packet_set_server(int);
void     packet_set_authenticated(int);

void     packet_start(int, u_char);
void     packet_put_char(int, int);
void     packet_put_int(int, u_int value);
void     packet_put_bignum(int, BIGNUM * value);
void     packet_put_bignum2(int, BIGNUM * value);
void     packet_put_string(int, const void *buf, u_int len);
void     packet_put_cstring(int, const char *str);
void     packet_put_raw(int, const void *buf, u_int len);
void     packet_send(int);

int      packet_read(int);
void     packet_read_expect(int, int type);
int      packet_read_poll(int);
void     packet_process_incoming(int, const char *buf, u_int len);
int      packet_read_seqnr(int cn, u_int32_t *seqnr_p);
int      packet_read_poll_seqnr(int cn, u_int32_t *seqnr_p);

u_int    packet_get_char(int);
u_int    packet_get_int(int);
void     packet_get_bignum(int, BIGNUM * value);
void     packet_get_bignum2(int, BIGNUM * value);
void    *packet_get_raw(int, u_int *length_ptr);
void    *packet_get_string(int, u_int *length_ptr);
void     packet_exit(int);
void     packet_disconnect(int, const char *fmt,...) __attribute__((format(printf, 2, 3)));
void     packet_send_debug(int, const char *fmt,...) __attribute__((format(printf, 2, 3)));

void     set_newkeys(int cn, int mode);
int      packet_get_keyiv_len(int, int);
void     packet_get_keyiv(int, int, u_char *, u_int);
int      packet_get_keycontext(int, int, u_char *);
void     packet_set_keycontext(int, int, u_char *);
void     packet_get_state(int, int, u_int32_t *, u_int64_t *, u_int32_t *);
void     packet_set_state(int, int, u_int32_t, u_int64_t, u_int32_t);
int      packet_get_ssh1_cipher(int);
void     packet_set_iv(int, int, u_char *);

void     packet_write_poll(int cn);
void     packet_write_wait(int);
int      packet_have_data_to_write(int);
int      packet_not_very_much_data_to_write(int);

int      packet_connection_is_on_socket(int);
int      packet_connection_is_ipv4(int);
int      packet_remaining(int);
void     packet_send_ignore(int, int);
void     packet_add_padding(int, u_char);

extern u_int max_packet_size[];
int  packet_set_maxsize(int, u_int);
#define  packet_get_maxsize(cn) max_packet_size[cn]

/* don't allow remaining bytes after the end of the message */
#define packet_check_eom(cn) \
do { \
    int _len = packet_remaining(cn); \
    if (_len > 0) { \
        logit("Packet integrity error (%d bytes remaining) at %s:%d", \
            _len ,__FILE__, __LINE__); \
        packet_disconnect(cn, "Packet integrity error."); \
    } \
} while (0)

int  packet_need_rekeying(int);
void     packet_set_rekey_limit(int, u_int32_t);

#endif              /* PACKET_H */
