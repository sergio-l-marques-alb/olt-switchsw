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

#ifndef __PAEMGMT_H__
#define __PAEMGMT_H__

#include "pae_ipsec.h"


#define MAX_SPUH_LEN       128
#define MAX_SPD_LEN         64

/* structure of the cached SA in the PAE */
typedef struct pae_cached_sa_t {
    PAE_IPSEC_ACTION *pae_action;/* SA action: includes SPU headers and packet prepend payload */
	uint16_t flags;              /* flags for the cached SA */
	uint8_t *sau_update;         /* pointer the 5862 style SAU structure */
	uint8_t *spd_update;         /* pointer to the 5862 styel SPD structure */
} PAE_CACHED_SA;

extern PAE_IPSEC_ACTION PaeIpsecOutboundSA[];
extern PAE_IPSEC_ACTION PaeIpsecInboundSA[];
extern PAE_CACHED_SA PaeCachedSA[];
extern int PaeIpsecCachedSACount;
extern int IpsecSAInboundCount;
extern int IpsecSAOutboundCount;
extern int PaeProcessMessage (uint8_t *msg);

#endif /* _PAEMGMT_H__ */
