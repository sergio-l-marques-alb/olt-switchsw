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
/******************************************************************************
*  @file    paehost.h
*
*  @brief   PAE IPsec header file 
*
*  @note
*
*****************************************************************************/

#ifndef __PAEHOST_H__
#define __PAEHOST_H__

#define KERN_LEVEL	KERN_CRIT


//=================== PAE Host Interface Prototypes =========================
int PaeIpsecInit(void);
int PaeIpsecShutdown(void);
int PaeHostIpsecAddSA(struct xfrm_state *x);
int PaeHostIpsecAddPolicy(unsigned dir, struct xfrm_policy *xp);
int PaeHostIpsecDeleteSA(struct xfrm_state *x);
int PaeIpsecSetPeerAddr (struct sk_buff *skb, struct xfrm_state *x, uint8_t inbound); 

void DataDump(char *msg, void *buf, int len);
void Dump_skb(struct sk_buff *s);
#endif /* __PAEHOST_H__*/
