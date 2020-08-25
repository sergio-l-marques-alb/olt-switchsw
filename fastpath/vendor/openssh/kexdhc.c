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
RCSID("$OpenBSD: kexdhc.c,v 1.3 2005/11/04 05:15:59 djm Exp $");

#include "xmalloc.h"
#include "key.h"
#include "kex.h"
#include "log.h"
#include "packet.h"
#include "dh.h"
#include "ssh2.h"

#ifndef L7_SSHD
void
kexdh_client(Kex *kex)
#else

void kexdh_free_newkeys(int cn, Kex *kex)
{
  int mode;
  Enc *enc;
  Mac *mac;
  Comp *comp;

  for (mode = 0; mode < MODE_MAX; mode++)
  {
    if (kex->newkeys[mode] != NULL)
    {
      enc  = &kex->newkeys[mode]->enc;
      mac  = &kex->newkeys[mode]->mac;
      comp = &kex->newkeys[mode]->comp;

      if (enc->name != NULL)
        xfree(enc->name);
      if (enc->iv != NULL)
        xfree(enc->iv);
      if (enc->key != NULL)
        xfree(enc->key);
      if (mac->name != NULL)
        xfree(mac->name);
      if (mac->key != NULL)
        xfree(mac->key);
      if (comp->name != NULL)
        xfree(comp->name);

      xfree(kex->newkeys[mode]);
      kex->newkeys[mode] = NULL;
    }
  }

}
void
kexdh_client(int cn, Kex *kex)
#endif /* L7_SSHD */
{
    BIGNUM *dh_server_pub = NULL, *shared_secret = NULL;
#ifndef L7_SSHD
    DH *dh;
#else
    DH *dh = NULL;
#endif /* L7_SSHD */
    Key *server_host_key;
    u_char *server_host_key_blob = NULL, *signature = NULL;
    u_char *kbuf, *hash;
    u_int klen, kout, slen, sbloblen, hashlen;

    /* generate and send 'e', client DH public key */
    switch (kex->kex_type) {
    case KEX_DH_GRP1_SHA1:
        dh = dh_new_group1();
        break;
    case KEX_DH_GRP14_SHA1:
        dh = dh_new_group14();
        break;
    default:
#ifndef L7_SSHD
        fatal("%s: Unexpected KEX type %d", __func__, kex->kex_type);
#else
        error("%s: Unexpected KEX type %d", __func__, kex->kex_type);
        DH_free(dh);
        return;
#endif /* L7_SSHD */
    }
    dh_gen_key(dh, kex->we_need * 8);
#ifndef L7_SSHD
    packet_start(SSH2_MSG_KEXDH_INIT);
    packet_put_bignum2(dh->pub_key);
    packet_send();
#else
    packet_start(cn, SSH2_MSG_KEXDH_INIT);
    packet_put_bignum2(cn, dh->pub_key);
    packet_send(cn);
#endif /* L7_SSHD */

    debug("sending SSH2_MSG_KEXDH_INIT");
#ifndef L7_SSHD
#ifdef DEBUG_KEXDH
    DHparams_print_fp(stderr, dh);
    fprintf(stderr, "pub= ");
    BN_print_fp(stderr, dh->pub_key);
    fprintf(stderr, "\n");
#endif
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_CRYPTO))
    {
        DHparams_print_fp(stdout, dh);
        fprintf(stdout, "pub= ");
        BN_print_fp(stdout, dh->pub_key);
        fprintf(stdout, "\n");
    }
#endif /* L7_SSHD */

    debug("expecting SSH2_MSG_KEXDH_REPLY");
#ifndef L7_SSHD
    packet_read_expect(SSH2_MSG_KEXDH_REPLY);
#else
    packet_read_expect(cn, SSH2_MSG_KEXDH_REPLY);
#endif /* L7_SSHD */

    /* key, cert */
#ifndef L7_SSHD
    server_host_key_blob = packet_get_string(&sbloblen);
    server_host_key = key_from_blob(server_host_key_blob, sbloblen);
    if (server_host_key == NULL)
        fatal("cannot decode server_host_key_blob");
    if (server_host_key->type != kex->hostkey_type)
        fatal("type mismatch for decoded server_host_key_blob");
    if (kex->verify_host_key == NULL)
        fatal("cannot verify server_host_key");
    if (kex->verify_host_key(server_host_key) == -1)
        fatal("server_host_key verification failed");

    /* DH paramter f, server public DH key */
    if ((dh_server_pub = BN_new()) == NULL)
        fatal("dh_server_pub == NULL");
    packet_get_bignum2(dh_server_pub);
#else
    server_host_key_blob = packet_get_string(cn, &sbloblen);
    if (server_host_key_blob == NULL)
    {
      error("cannot get server_host_key_blob");
      DH_free(dh);
      kexdh_free_newkeys(cn, kex);
      return;
    }
    server_host_key = key_from_blob(cn, server_host_key_blob, sbloblen);
    if (server_host_key == NULL)
    {
        error("cannot decode server_host_key_blob");
        xfree(server_host_key_blob);
        DH_free(dh);
        kexdh_free_newkeys(cn, kex);
        return;
    }
    if (server_host_key->type != kex->hostkey_type)
    {
        error("type mismatch for decoded server_host_key_blob");
        key_free(server_host_key);
        xfree(server_host_key_blob);
        DH_free(dh);
        kexdh_free_newkeys(cn, kex);
        return;
    }
    if (kex->verify_host_key == NULL)
    {
        error("cannot verify server_host_key");
        key_free(server_host_key);
        xfree(server_host_key_blob);
        DH_free(dh);
        kexdh_free_newkeys(cn, kex);
        return;
    }
    if (kex->verify_host_key(server_host_key) == -1)
    {
        error("server_host_key verification failed");
        key_free(server_host_key);
        xfree(server_host_key_blob);
        DH_free(dh);
        kexdh_free_newkeys(cn, kex);
        return;
    }

    /* DH paramter f, server public DH key */
    if ((dh_server_pub = BN_new()) == NULL)
    {
        error("dh_server_pub == NULL");
        key_free(server_host_key);
        xfree(server_host_key_blob);
        DH_free(dh);
        kexdh_free_newkeys(cn, kex);
        return;
    }
    packet_get_bignum2(cn, dh_server_pub);
#endif /* L7_SSHD */

#ifndef L7_SSHD
#ifdef DEBUG_KEXDH
    fprintf(stderr, "dh_server_pub= ");
    BN_print_fp(stderr, dh_server_pub);
    fprintf(stderr, "\n");
    debug("bits %d", BN_num_bits(dh_server_pub));
#endif
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        fprintf(stdout, "dh_server_pub= ");
        BN_print_fp(stdout, dh_server_pub);
        fprintf(stdout, "\n");
        debug("bits %d", BN_num_bits(dh_server_pub));
    }
#endif /* L7_SSHD */

    /* signed H */
#ifndef L7_SSHD
    signature = packet_get_string(&slen);
    packet_check_eom();

    if (!dh_pub_is_valid(dh, dh_server_pub))
        packet_disconnect("bad server public DH value");
#else
    signature = packet_get_string(cn, &slen);
    packet_check_eom(cn);

    if (!dh_pub_is_valid(dh, dh_server_pub))
        packet_disconnect(cn, "bad server public DH value");
#endif /* L7_SSHD */

    klen = DH_size(dh);
    kbuf = xmalloc(klen);
    kout = DH_compute_key(kbuf, dh_server_pub, dh);
#ifndef L7_SSHD
#ifdef DEBUG_KEXDH
    dump_digest("shared secret", kbuf, kout);
#endif
    if ((shared_secret = BN_new()) == NULL)
        fatal("kexdh_client: BN_new failed");
#else
    if (sshdLogLevelCheck(SYSLOG_LEVEL_PACKET))
    {
        dump_digest("shared secret", kbuf, kout);
    }
    if ((shared_secret = BN_new()) == NULL)
    {
        fatal_cn(cn, "kexdh_client: BN_new failed");
    }
#endif /* L7_SSHD */
    BN_bin2bn(kbuf, kout, shared_secret);
    memset(kbuf, 0, klen);
    xfree(kbuf);

    /* calc and verify H */
    kex_dh_hash(
        kex->client_version_string,
        kex->server_version_string,
        buffer_ptr(&kex->my), buffer_len(&kex->my),
        buffer_ptr(&kex->peer), buffer_len(&kex->peer),
        server_host_key_blob, sbloblen,
        dh->pub_key,
        dh_server_pub,
        shared_secret,
        &hash, &hashlen
    );
    xfree(server_host_key_blob);
    BN_clear_free(dh_server_pub);
    DH_free(dh);

#ifndef L7_SSHD
    if (key_verify(server_host_key, signature, slen, hash, hashlen) != 1)
        fatal("key_verify failed for server_host_key");
#else
    if (key_verify(cn, server_host_key, signature, slen, hash, hashlen) != 1)
    {
        fatal_cn(cn, "key_verify failed for server_host_key");
    }
#endif /* L7_SSHD */
    key_free(server_host_key);
    xfree(signature);

    /* save session id */
    if (kex->session_id == NULL) {
        kex->session_id_len = hashlen;
        kex->session_id = xmalloc(kex->session_id_len);
        memcpy(kex->session_id, hash, kex->session_id_len);
    }

#ifndef L7_SSHD
    kex_derive_keys(kex, hash, hashlen, shared_secret);
#else
    kex_derive_keys(cn, kex, hash, hashlen, shared_secret);
#endif /* L7_SSHD */
    BN_clear_free(shared_secret);
#ifndef L7_SSHD
    kex_finish(kex);
#else
    kex_finish(cn, kex);
#endif /* L7_SSHD */
}
