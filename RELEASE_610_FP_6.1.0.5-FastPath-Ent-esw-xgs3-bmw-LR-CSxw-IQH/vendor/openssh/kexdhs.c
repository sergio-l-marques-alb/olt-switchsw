/*
 * Copyright (c) 2001 Markus Friedl.  All rights reserved.
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
RCSID("$OpenBSD: kexdhs.c,v 1.3 2005/11/04 05:15:59 djm Exp $");

#include "xmalloc.h"
#include "key.h"
#include "kex.h"
#include "log.h"
#include "packet.h"
#include "dh.h"
#include "ssh2.h"
#include "monitor_wrap.h"

#ifdef L7_SSHD
extern DH *sshd_kex_dh[];
extern BIGNUM *sshd_dh_client_pub[];
#endif /* L7_SSHD */

#ifndef L7_SSHD
void
kexdh_server(Kex *kex)
#else
void
kexdh_server(int cn, Kex *kex)
#endif /* L7_SSHD */
{
#ifndef L7_SSHD
    BIGNUM *shared_secret = NULL, *dh_client_pub = NULL;
    DH *dh;
#else
    BIGNUM *shared_secret = NULL;
#endif /* L7_SSHD */
    Key *server_host_key;
    u_char *kbuf, *hash, *signature = NULL, *server_host_key_blob = NULL;
    u_int sbloblen, klen, kout, hashlen;
    u_int slen;

    /* generate server DH public key */
    switch (kex->kex_type) {
    case KEX_DH_GRP1_SHA1:
#ifndef L7_SSHD
        dh = dh_new_group1();
#else
        sshd_kex_dh[cn] = dh_new_group1();
#endif /* L7_SSHD */
        break;
    case KEX_DH_GRP14_SHA1:
#ifndef L7_SSHD
        dh = dh_new_group14();
#else
        sshd_kex_dh[cn] = dh_new_group14();
#endif /* L7_SSHD */
        break;
    default:
#ifndef L7_SSHD
        fatal("%s: Unexpected KEX type %d", __func__, kex->kex_type);
#else
        fatal_cn(cn, "%s: Unexpected KEX type %d", __func__, kex->kex_type);
#endif /* L7_SSHD */
    }
#ifndef L7_SSHD
    dh_gen_key(dh, kex->we_need * 8);
#else
    dh_gen_key(sshd_kex_dh[cn], kex->we_need * 8);
#endif /* L7_SSHD */

    debug("expecting SSH2_MSG_KEXDH_INIT");
#ifndef L7_SSHD
    packet_read_expect(SSH2_MSG_KEXDH_INIT);
#else
    packet_read_expect(cn, SSH2_MSG_KEXDH_INIT);
#endif /* L7_SSHD */

    if (kex->load_host_key == NULL)
#ifndef L7_SSHD
        fatal("Cannot load hostkey");
#else
    {
        fatal_cn(cn, "Cannot load hostkey");
    }
#endif /* L7_SSHD */
    server_host_key = kex->load_host_key(kex->hostkey_type);
    if (server_host_key == NULL)
#ifndef L7_SSHD
        fatal("Unsupported hostkey type %d", kex->hostkey_type);
#else
    {
        fatal_cn(cn, "Unsupported hostkey type %d", kex->hostkey_type);
    }
#endif /* L7_SSHD */

    /* key, cert */
#ifndef L7_SSHD
    if ((dh_client_pub = BN_new()) == NULL)
        fatal("dh_client_pub == NULL");
    packet_get_bignum2(dh_client_pub);
    packet_check_eom();
#else
    if ((sshd_dh_client_pub[cn] = BN_new()) == NULL)
    {
        fatal_cn(cn, "sshd_dh_client_pub[%d] == NULL", cn);
    }
    packet_get_bignum2(cn, sshd_dh_client_pub[cn]);
    packet_check_eom(cn);
#endif /* L7_SSHD */

#ifndef L7_SSHD
#ifdef DEBUG_KEXDH
    fprintf(stderr, "dh_client_pub= ");
    BN_print_fp(stderr, dh_client_pub);
    fprintf(stderr, "\n");
    debug("bits %d", BN_num_bits(dh_client_pub));
#endif

#ifdef DEBUG_KEXDH
    DHparams_print_fp(stderr, dh);
    fprintf(stderr, "pub= ");
    BN_print_fp(stderr, dh->pub_key);
    fprintf(stderr, "\n");
#endif
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_CRYPTO))
    {
        fprintf(stdout, "sshd_dh_client_pub[%d] = ", cn);
        BN_print_fp(stdout, sshd_dh_client_pub[cn]);
        fprintf(stdout, "\n");
        debug("bits %d", BN_num_bits(sshd_dh_client_pub[cn]));

        DHparams_print_fp(stdout, sshd_kex_dh[cn]);
        fprintf(stdout, "pub= ");
        BN_print_fp(stdout, sshd_kex_dh[cn]->pub_key);
        fprintf(stdout, "\n");
    }
#endif /* L7_SSHD */

#ifndef L7_SSHD
    if (!dh_pub_is_valid(dh, dh_client_pub))
        packet_disconnect("bad client public DH value");

    klen = DH_size(dh);
    kbuf = xmalloc(klen);
    kout = DH_compute_key(kbuf, dh_client_pub, dh);
#else
    if (!dh_pub_is_valid(sshd_kex_dh[cn], sshd_dh_client_pub[cn]))
        packet_disconnect(cn, "bad client public DH value");

    klen = DH_size(sshd_kex_dh[cn]);
    kbuf = xmalloc(klen);
    kout = DH_compute_key(kbuf, sshd_dh_client_pub[cn], sshd_kex_dh[cn]);
#endif /* L7_SSHD */

#ifndef L7_SSHD
#ifdef DEBUG_KEXDH
    dump_digest("shared secret", kbuf, kout);
#endif
    if ((shared_secret = BN_new()) == NULL)
        fatal("kexdh_server: BN_new failed");
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_CRYPTO))
    {
        dump_digest("shared secret", kbuf, kout);
    }
    if ((shared_secret = BN_new()) == NULL)
    {
        fatal_cn(cn, "kexdh_server: BN_new failed");
    }
#endif /* L7_SSHD */
    BN_bin2bn(kbuf, kout, shared_secret);
    memset(kbuf, 0, klen);
    xfree(kbuf);

    key_to_blob(server_host_key, &server_host_key_blob, &sbloblen);

    /* calc H */
    kex_dh_hash(
        kex->client_version_string,
        kex->server_version_string,
        buffer_ptr(&kex->peer), buffer_len(&kex->peer),
        buffer_ptr(&kex->my), buffer_len(&kex->my),
        server_host_key_blob, sbloblen,
#ifndef L7_SSHD
        dh_client_pub,
        dh->pub_key,
#else
        sshd_dh_client_pub[cn],
        sshd_kex_dh[cn]->pub_key,
#endif /* L7_SSHD */
        shared_secret,
        &hash, &hashlen
    );
#ifndef L7_SSHD
    BN_clear_free(dh_client_pub);
#else
    BN_clear_free(sshd_dh_client_pub[cn]);
    sshd_dh_client_pub[cn] = NULL;
#endif /* L7_SSHD */

    /* save session id := H */
    if (kex->session_id == NULL) {
        kex->session_id_len = hashlen;
        kex->session_id = xmalloc(kex->session_id_len);
        memcpy(kex->session_id, hash, kex->session_id_len);
    }

    /* sign H */
#ifndef L7_SSHD
    PRIVSEP(key_sign(server_host_key, &signature, &slen, hash, hashlen));
#else
    PRIVSEP(key_sign(cn, server_host_key, &signature, &slen, hash, hashlen));
#endif /* L7_SSHD */

    /* destroy_sensitive_data(); */

    /* send server hostkey, DH pubkey 'f' and singed H */
#ifndef L7_SSHD
    packet_start(SSH2_MSG_KEXDH_REPLY);
    packet_put_string(server_host_key_blob, sbloblen);
    packet_put_bignum2(dh->pub_key);    /* f */
    packet_put_string(signature, slen);
    packet_send();
#else
    packet_start(cn, SSH2_MSG_KEXDH_REPLY);
    packet_put_string(cn, server_host_key_blob, sbloblen);
    packet_put_bignum2(cn, sshd_kex_dh[cn]->pub_key);    /* f */
    packet_put_string(cn, signature, slen);
    packet_send(cn);
#endif /* L7_SSHD */

    xfree(signature);
    xfree(server_host_key_blob);
    /* have keys, free DH */
#ifndef L7_SSHD
    DH_free(dh);

    kex_derive_keys(kex, hash, hashlen, shared_secret);
#else
    DH_free(sshd_kex_dh[cn]);
    sshd_kex_dh[cn] = NULL;

    kex_derive_keys(cn, kex, hash, hashlen, shared_secret);
#endif /* L7_SSHD */
    BN_clear_free(shared_secret);
#ifndef L7_SSHD
    kex_finish(kex);
#else
    kex_finish(cn, kex);
#endif /* L7_SSHD */
}
