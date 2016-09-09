/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef paeipsecmsg_H_
#define paeipsecmsg_H_


int PaeIpsecAddSA(PAEIF_SAFLAGS *sa_flags, PAEIF_SAID *sa_id, PAEIF_CRYPTOOP *op);
int PaeIpsecAddPayload(PAEIF_SAID *sa_id, TLVWORD *tlv);
int PaeIpsecDeleteSA(unsigned idx);
int PaeIpsecGetSAStats(unsigned idx);
int PaeIpsecGetStats(PAE_IPSEC_STATS *stats);
int PaeIpsecListSA(PAEIF_SAID *sa_id, int *maxLen);
int PaeIpsecSetHashMask(uint8_t version, uint8_t *src, 
		uint8_t *dst, uint16_t sport, uint16_t dport, uint8_t proto);
int PaeIpsecCreateSnoopAction(uint8_t inbound, uint8_t version, uint8_t *src, 
		uint8_t *dst, uint16_t sport, uint16_t dport, uint8_t proto, uint8_t masked);
int PaeIpsecGetSAIVLen(uint32_t spi, int inbound);
int PaeSnoopActionUpdate(uint8_t inbound, uint8_t version, uint8_t *need0, 
		uint8_t *need1, uint8_t *have0, uint8_t *have1);

#endif /* paeipsecmsg_H_ */
