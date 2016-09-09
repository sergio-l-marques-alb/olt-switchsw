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
#include <linux/err.h>
#include <linux/module.h>
#include <net/ip.h>
#include <net/xfrm.h>
#include <net/esp.h>
#include <net/ah.h>
#include <linux/kernel.h>
#include <linux/pfkeyv2.h>
#include <linux/in6.h>
#include <linux/ipsec.h>
#include <net/icmp.h>
#include <net/protocol.h>
#include <net/udp.h>

#include "pae_ipsec.h"
#include "paemsg.h"
#include "paehost.h"
#include "paeipsecmsg.h"
#include "ipsec_notify.h"

#define BUFF_SIZE        1024

#define IPV4_TUNNEL_PRESET_LEN    (12)

uint8_t ipv4_outbound_l3_header [IPV4_TUNNEL_PRESET_LEN] = 
{   
	0x45, IPV4_TOS_DEFAULT, 0x00, 0x00, // Start of IPv4 Header
	0x00, 0x00, 0x00, 0x00, IPV4_TTL_DEFAULT, 
	0x00, 0x00, 0x00                    // src and dst IPv4 addr to follow
};

#define FLABEL0       (IPV6_FLABEL_DEFAULT & 0xFF)
#define FLABEL1       (IPV6_FLABEL_DEFAULT & 0xFF00 >> 8)
#define FLABEL2       (IPV6_FLABEL_DEFAULT & 0xFF0000 >> 16)

#define IPV6_TUNNEL_PRESET_LEN   (8) 
uint8_t ipv6_outbound_l3_header [IPV6_TUNNEL_PRESET_LEN] = 
{	
	0x6 , FLABEL2, FLABEL1, FLABEL0, 0x00, // Start of IPv6 Header
	IPV6_HOP_DEFAULT, 0x00, 0x00        // src and dst IPv6 addr to follow
};

uint8_t ipv4_outbound_l2_header [L2_HEADER_LEN] =
{
    0x00, 0x22, 0x68, 0x1c, 0x09, 0x88,  // dst MAC addr
    0xd4, 0xae, 0x52, 0xbc, 0xe5, 0xf8,  // src MAC addr
	0x81, 0x00, 0x00, 0x02,              // VLAN TAG with VID 2 is WAN
    0x08, 0x00,              // IPv4 payload
};

uint8_t ipv6_outbound_l2_header [L2_HEADER_LEN] =
{
    0x00, 0x22, 0x68, 0x1c, 0x09, 0x88,  // dst MAC addr
    0xd4, 0xae, 0x52, 0xbc, 0xe5, 0xf8,  // src MAC addr
	0x81, 0x00, 0x00, 0x02,              // VLAN TAG with VID 2 is WAN
	0x86, 0xdd,              			// IPv6 payload
};

uint8_t ipv4_inbound_l2_header [L2_HEADER_LEN] =
{
    0x80, 0x3f, 0xd5, 0x08, 0x33, 0xf4,  // dst MAC addr
    0xd4, 0xae, 0x52, 0xbc, 0xe5, 0x08,   // src MAC addr,
	0x81, 0x00, 0x00, 0x01,              // VLAN TAG with VID 1 is LAN
    0x08, 0x00,              // IPv4 payload
};

uint8_t ipv6_inbound_l2_header [L2_HEADER_LEN] =
{
    0x80, 0x3f, 0xd5, 0x08, 0x33, 0xf4,  // dst MAC addr
    0xd4, 0xae, 0x52, 0xbc, 0xe5, 0x08,   // src MAC addr,
	0x81, 0x00, 0x00, 0x01,              // VLAN TAG with VID 1 is LAN
	0x86, 0xdd,              			// IPv6 payload
};

#ifndef HTONL
#define HTONL(x)        ((x)=htonl(x))
#endif

//=================== Linux IPsec Stack PAE Interface =====================
//#ifndef __KERNEL__
/*---------------------------------------------------------------
 * Name    : DataDump
 * Purpose : Hardware Crypto Encapsulation Callback
 * Input   : 
 * Return  : Error status
 * Remarks : 
 *--------------------------------------------------------------*/
void
DataDump(char *msg, void *buf, int len)
{
    int n;
    uint8_t *p = (uint8_t *) buf;

    printk("%s\n", msg);
    for (n = 0; n < len && n < 256; n++) {
        printk("%02x", *p++);
        if (!((n + 1) % 8))
            printk(" ");
        if (!((n + 1) % 32))
            printk("\n");
    }
    printk("\n");
}

/*---------------------------------------------------------------
 * Name    : Dump_skb
 * Purpose : Hardware Crypto Encapsulation Callback
 * Input   : 
 * Return  : Error status
 * Remarks : 
 *--------------------------------------------------------------*/
void
Dump_skb(struct sk_buff *s)
{
    // Looking at it as skb
    printk("skb %p: data %p tail %p len %d:%d", (void *)s, (void *)s->data, (void *)s->tail, s->len,
           s->tail - s->data);
    //printk ("head %p end %p", s, s->head, s->end);
    DataDump("skb data", s->data, 16);
}
//#endif /* __KERNEL__ */

int PaeHostIpsecDeleteSA (struct xfrm_state *x)
{
	PAEIF_SAID said;
	int idx;

	// Fill the said
	said.spi = x->id.spi;
	//said.flags.proto = x->id.proto;
	idx = GetSAIndex(&said);
	if (idx == UNKNOWN_SA_IDX)
	    return PAEIF_HOST_ERROR_SA_NOT_FOUND;
	return PaeIpsecDeleteSA(idx);
}

/*----------------------------------------------------------------------
 * Name    : CalcIPv4PartialChecksum
 * Purpose : Calculates the IP checksum for IPv4 header
 * Input   : pointer to the start of the IP packet
 * Output  : 
 * Returns : 
 *---------------------------------------------------------------------*/
inline void IPv4PartialChecksum (uint8_t *src, int len)
{
    uint8_t *s;
    uint32_t cksum = 0;
    int i;

	*(uint16_t*)(src + 10) = 0;
    s = (uint8_t *)src;

    for (i = 0; i < len; i += 2, s +=2) {
		cksum += ntohs(*(uint16_t*)s);
        if(cksum & 0x80000000) {  /* if high order bit set, fold */
		    cksum = (cksum & 0xFFFF) + (cksum >> 16);
		}
	}

    while (cksum >> 16)
	   cksum = (cksum & 0xFFFF) + (cksum >> 16);

	// This is a partial checksum so will skip the last complementation
    // cksum = ~cksum;

	/* copy back and swap for endian-ness */
	*(uint16_t*)(src + 10) = ntohs((uint16_t)cksum);
}

#ifdef USE_FHMAC_MODE
#define MAX_HMAC_CONTEXT             64
#define HMAC_INNER_CONTEXT           1
#define HMAC_OUTER_CONTEXT           2 

static int
create_hmac_context(struct xfrm_state *x, uint8_t *dst, int context)
{
    uint8_t ipad = 0x36;
    uint8_t opad = 0x5c;
    struct xfrm_algo_desc *aalgd = NULL;
    int i;

    uint8_t pad[MAX_HMAC_CONTEXT];

    if (!x->aalg) {
        printk(KERN_LEVEL "No auth alg\n");
        return -1;
    }

    aalgd = xfrm_aalg_get_byname(x->aalg->alg_name, 0),
    memcpy (pad, x->aalg->alg_key, x->aalg->alg_key_len / 8 );
    for (i = 0; i < MAX_HMAC_CONTEXT; i++) {
        if (context == HMAC_INNER_CONTEXT) {
            ctx[i] ^= ipad;
        }
        else if (context == HMAC_OUTER_CONTEXT) {
            ctx[i] ^= opad;
        }
        else {
            printk(KERN_LEVEL "Unknown HMAC context type %d\n", context);
            return -1;
        }
    }
    switch (aalgd->desc.sadb_alg_id) {
    case SADB_AALG_MD5HMAC:
        break;
    case SADB_AALG_SHA1HMAC:
        break;
    default:
        printk(KERN_LEVEL "HMAC algorithm %d not supported for FHMAC mode\n", 
                aalgd->desc.sadb_alg_id);
        return -1;
    }
    memcpy (dst, ctx, x->aalg->alg_key_len / 8);
    return 0;
}
#endif

int PaeIpsecSetPeerAddr (struct sk_buff *skb, struct xfrm_state *x, uint8_t inbound) 
{
	struct iphdr *iph;
	struct iphdr *pkt_iph;
	struct local_if *local_if;
	int status = PAE_STATUS_OK;
	uint8_t ip_ver;
	int ivLen;

	ip_ver = 4;   // IPv6 is TBD
	iph = ip_hdr(skb);

	if (inbound) {
		local_if = PaeHostMatchLocalAddr((uint8_t*)&iph->daddr, ip_ver);
		if (!local_if) {
			printk("no local addr match for inbound traffic\n");
			status = PAEIF_HOST_ERROR_LOCAL_IF_MATCH;
			goto error;
		}
		ivLen = PaeIpsecGetSAIVLen(x->id.spi, inbound);
		if (ivLen < 0) {
			status = PAEIF_HOST_ERROR_SA_NOT_FOUND;
			printk("Unable to get ivLen\n");
			goto error;
		}
		// pkt_iph points to  the decapsulated inbound packet it starts with the SPI
		pkt_iph = (struct iphdr*)(skb->data + 4 + 4 + ivLen);
		// Put a watch on pkt_iph->daddr  and iph->saddr
		//printk("Inbound: snoop on %08x as src and %08x as dst\n", *(uint32_t*)&pkt_iph->daddr, *(uint32_t*)&iph->saddr);
		//printk("local addresses %08x %08x\n", *(uint32_t*)&pkt_iph->saddr, *(uint32_t*)&iph->daddr);
		status = PaeSnoopActionUpdate(inbound, ip_ver, (uint8_t*)&pkt_iph->daddr, (uint8_t*)&iph->saddr,
								(uint8_t*)&pkt_iph->saddr, (uint8_t*)&iph->daddr);
	} else {
		local_if = PaeHostMatchLocalAddr((uint8_t*)&iph->saddr, 4);
		if (!local_if) {
			printk("no local addr match for outbound traffic\n");
			status = PAEIF_HOST_ERROR_LOCAL_IF_MATCH;
			goto error;
		}
		// pkt_iph points to the original IP packet it starts with the header
		pkt_iph = (struct iphdr*)(skb->data);
		// Put a watch on pkt_iph->saddr  and iph->daddr
		//printk("Outbound: snoop on %08x as dst and %08x as src\n", *(uint32_t*)&pkt_iph->saddr, *(uint32_t*)&iph->daddr);
		//printk("local addresses %08x %08x\n", *(uint32_t*)&pkt_iph->daddr, *(uint32_t*)&iph->saddr);
		status = PaeSnoopActionUpdate(inbound, ip_ver, (uint8_t*)&iph->daddr, (uint8_t*)&pkt_iph->saddr,
									(uint8_t*)&pkt_iph->daddr, (uint8_t*)&iph->saddr);
	}
error:
	return status;
}

/*-------------------------------------------------------------------------
 * Name    : PaeHostIpsecAddSA
 * Purpose : Create the PAE Context for the SA and submit to the PAE 
 * Input   : 
 * Return  : Error status
 * Remarks : 
 *------------------------------------------------------------------------*/
int PaeHostIpsecAddSA(struct xfrm_state *x)
{
    int status = PAEIF_STATUS_OK;
    uint8_t *buff = 0;
	uint32_t flags = 0;
    PAEIF_SAID sa_id;
    PAEIF_SAID *said = &sa_id;
    PAEIF_CRYPTOOP cryptoop;
    PAEIF_CRYPTOOP *op = &cryptoop;
	PAEIF_SAFLAGS *sa_flags = (PAEIF_SAFLAGS*)&flags;
	struct local_if *local_if;
    TLVWORD *tlv;
	uint8_t *ptr;


    if (x->props.mode == XFRM_MODE_TRANSPORT) {
		// Transport mode not currently supported
        printk(KERN_LEVEL "Transport mode not currently supported in PAE");
		status = PAEIF_HOST_ERROR_MODE_NOT_SUPPORTED;
        goto error;
	}
    //printk(KERN_LEVEL "SA SPI %08x proto %d\n", ntohl(x->id.spi), x->id.proto);

    buff = kmalloc (BUFF_SIZE, GFP_KERNEL);
    if (!buff) {
        printk(KERN_LEVEL "Cannot allocate memory");
        status = PAEIF_HOST_ERROR_ALLOC;
        goto error;
    }
    memset((void *)buff, 0, BUFF_SIZE);
    memset((void *)said, 0, sizeof(PAEIF_SAID));
    memset((void *)op, 0, sizeof(PAEIF_CRYPTOOP));

    said->spi = x->id.spi;
    /* set the SA ID  */
    switch (x->id.proto) {
    case IPPROTO_ESP:
        sa_flags->esp = 1;
        break;
    case IPPROTO_AH:
        sa_flags->ah = 1;
        break;
    default:
        printk(KERN_LEVEL "Unknown IP protocol %d\n", x->id.proto);
        status = PAEIF_HOST_ERROR_INPUT;
        goto error;
    }

    /* Specify IPv4 context */
    if (x->sel.family == AF_INET) {
        sa_flags->ipv4 = 1;
        memcpy((void *)said->v4dstAddr, (void *)&x->id.daddr.a4, SIZE_IPV4_ADDR);
		local_if = PaeHostMatchLocalAddr(said->v4dstAddr, 4);
		if (local_if) {
			/* This is the WAN interface this is an inbound direction */
			local_if->wan = 1;
            sa_flags->inbound = 1;
			/* Now get the LAN interface */
			local_if = PaeHostGetLanInterface();
			if (!local_if) {
				printk("Unable to identify the LAN interface\n");
				status = PAEIF_HOST_ERROR_LAN_NOT_FOUND;
				goto error;
			}
			memcpy(&ipv4_inbound_l2_header[6], local_if->mac_addr, 6);
			*(uint16_t*)&ipv4_inbound_l2_header[14] = htons(local_if->vlan_id);
            printk(KERN_LEVEL "direction is inbound\n");
			//printk("lan vlanid %d remote src %x local dst %x\n", local_if->vlan_id, 
			//		*(uint32_t*)&x->props.saddr.a4, *(uint32_t*)said->v4dstAddr);
		} else if (x->props.saddr.a4) {
			local_if = PaeHostMatchLocalAddr((uint8_t*)&x->props.saddr.a4, 4);
			if (!local_if) {
				printk("Unable to identify the WAN interface\n");
				status = PAEIF_HOST_ERROR_WAN_NOT_FOUND;
				goto error;
			}
			/* src is local means it is the WAN interface */
			local_if->wan = 1;
			memcpy(&ipv4_outbound_l2_header[6], local_if->mac_addr, 6);
			*(uint16_t*)&ipv4_outbound_l2_header[14] = htons(local_if->vlan_id);
			printk(KERN_LEVEL "direction is outbound\n");
			//printk("wan vlanid %d local src %x remote dst %x\n", local_if->vlan_id, 
			//		*(uint32_t*)&x->props.saddr.a4, *(uint32_t*)said->v4dstAddr);
		}
    } else if (x->sel.family == AF_INET6) {
        sa_flags->ipv6 = 1;
        memcpy((void *)said->v6dstAddr, (void *)x->id.daddr.a6, SIZE_IPV6_ADDR);
		local_if = PaeHostMatchLocalAddr(said->v6dstAddr, 6);
		if (local_if) {
            sa_flags->inbound = 1;
            printk(KERN_LEVEL "direction is inbound\n");
		} else if (x->props.saddr.a6) {
				local_if = PaeHostMatchLocalAddr((uint8_t*)&x->props.saddr.a6, 6);
				if (local_if) {
					/* src is local means it is the WAN interface */
					local_if->wan = 1;
					printk(KERN_LEVEL "direction is outbound\n");
				}
		}
    }

    /* check if extended sequence number is required */
    if (x->props.flags & XFRM_STATE_ESN) {
        sa_flags->esn = 1;
    }

    /* Define the crypto operations for this SA */
    /* Specify the authentication protocol if any */
    if (x->aalg) {
        struct xfrm_algo_desc *aalgd;
        aalgd = xfrm_aalg_get_byname(x->aalg->alg_name, 0);
        if (!aalgd) {
            printk(KERN_LEVEL "Auth algorithm error\n");
            status = PAEIF_HOST_ERROR_INPUT;
            goto error;
        }
        switch (aalgd->desc.sadb_alg_id) {
        case SADB_AALG_NONE:
        case SADB_X_AALG_NULL:
            op->authAlg = PAEIF_AUTH_ALG_NULL;
            break;
        case SADB_AALG_MD5HMAC:
            op->authAlg = PAEIF_AUTH_ALG_MD5;
            op->authMode = PAEIF_AUTH_MODE_HMAC;
            break;
        case SADB_AALG_SHA1HMAC:
            op->authAlg = PAEIF_AUTH_ALG_SHA1;
            op->authMode = PAEIF_AUTH_MODE_HMAC;
            break;
        case SADB_X_AALG_SHA2_256HMAC:
            op->authAlg = PAEIF_AUTH_ALG_SHA256;
            op->authMode = PAEIF_AUTH_MODE_HMAC;
            break;
        case SADB_X_AALG_AES_XCBC_MAC:
            op->authAlg = PAEIF_AUTH_ALG_AES;
            op->authMode = PAEIF_AUTH_MODE_XCBC;
            break;
        default:
            printk(KERN_LEVEL "Unsuported HMAC algorithm %d\n",
                   aalgd->desc.sadb_alg_id);
            status = PAEIF_HOST_ERROR_AUTH_ALG;
            goto error;
        }
        /* copy the keys to the CRYPTOOP structure */
        sa_flags->icvTruncLen = x->aalg->alg_trunc_len/8;
        op->icvTruncBits = x->aalg->alg_trunc_len;
        if (x->aalg->alg_key_len > MAX_AUTH_KEY_SIZE) {
            status = PAEIF_HOST_ERROR_KEY_LENGTH;
            goto error;
        }
        if (op->authAlg != PAEIF_AUTH_ALG_NULL) {
			memcpy (op->authKey, x->aalg->alg_key, (x->aalg->alg_key_len + 7)/8); 
			op->authKeyBits = x->aalg->alg_key_len;
            sa_flags->auth = 1;
		}
    }    

    /* Specify the authenticed encryption protocol if any */
    if (x->aead) {
        struct xfrm_algo_desc *aealgd;
        int icvBits;
        for (icvBits = 8; icvBits < 17; icvBits += 4) {
            aealgd = xfrm_aead_get_byname(x->aead->alg_name, icvBits, 0);
            if (!aealgd) {
                continue;
            }
            switch (aealgd->desc.sadb_alg_id) {
            case SADB_X_EALG_AES_GCM_ICV8:
            case SADB_X_EALG_AES_GCM_ICV12:
            case SADB_X_EALG_AES_GCM_ICV16:
                /* Encryption AND authentication algorithm rfc4106 */
                op->authAlg = PAEIF_AUTH_ALG_AES;
                op->authMode = PAEIF_AUTH_MODE_GCM;
				op->encrAlg = PAEIF_ENCR_ALG_AES;
				op->encrMode = PAEIF_ENCR_MODE_GCM;
                op->icvTruncBits = icvBits;
                break;
            default:
                printk(KERN_LEVEL "Unsuported authenticated encryption algorithm %d\n",
                       aealgd->desc.sadb_alg_id);
                status = PAEIF_HOST_ERROR_ENCR_ALG;
                goto error;
                break;
            }
            if (!aealgd) {
                printk(KERN_LEVEL "Authicated encryption algorithm error\n");
                status = PAEIF_HOST_ERROR_INPUT;
                goto error;
            }
        }
        /* copy the keys to the CRYPTOOP structure */
        if (x->aead->alg_key_len > MAX_AUTH_KEY_SIZE) {
            status = PAEIF_HOST_ERROR_KEY_LENGTH;
            goto error;
        }
        memcpy (op->authKey, x->aead->alg_key, (x->aead->alg_key_len + 7)/8); 
		op->authKeyBits = x->aead->alg_key_len;
    }

    /* Set the encryption protocol */
    if (x->id.proto == IPPROTO_ESP) {
        struct xfrm_algo_desc *ealgd;
        ealgd = xfrm_ealg_get_byname(x->ealg->alg_name, 0);
        if (!ealgd) {
            printk(KERN_LEVEL "ESP but no encryption algorithm\n");
            status = PAEIF_HOST_ERROR_ENCR_ALG;
            goto error;
        }
        switch (ealgd->desc.sadb_alg_id) {
        case SADB_EALG_NULL:
            op->encrAlg = PAEIF_ENCR_ALG_NULL;
            op->encrMode = PAEIF_ENCR_MODE_CBC;
           break;
        case SADB_EALG_DESCBC:
            op->encrAlg = PAEIF_ENCR_ALG_DES;
            op->encrMode = PAEIF_ENCR_MODE_CBC;
            sa_flags->ivLen = 8;
        break;
        case SADB_EALG_3DESCBC:
            op->encrAlg = PAEIF_ENCR_ALG_3DES;
            op->encrMode = PAEIF_ENCR_MODE_CBC;
            sa_flags->ivLen = 8;
            break;
        case SADB_X_EALG_AESCBC:
            op->encrAlg = PAEIF_ENCR_ALG_AES;
            op->encrMode = PAEIF_ENCR_MODE_CBC;
            switch (x->ealg->alg_key_len) {
            case 128:
            case 192:
            case 256:
				sa_flags->ivLen = 16;
                break;
            default:
                printk(KERN_LEVEL "AES CBC key length %d not supported\n", 
                        x->ealg->alg_key_len);
                status = PAEIF_HOST_ERROR_KEY_LENGTH;
                goto error;
                break;
            }
            break;
        case SADB_X_EALG_AESCTR:
            op->encrAlg = PAEIF_ENCR_ALG_AES;
            op->encrMode = PAEIF_ENCR_MODE_CTR;
            switch (x->ealg->alg_key_len) {
            case 128:
            case 192:
            case 256:
                break;
            default:
                printk(KERN_LEVEL "AES CTR key length %d not supported\n", 
                        x->ealg->alg_key_len);
                status = PAEIF_HOST_ERROR_KEY_LENGTH;
                goto error;
                break;
            }
            break;
        case SADB_X_EALG_NULL_AES_GMAC:
            /* Authentication only algorithm rfc4543 */
            op->authAlg = PAEIF_AUTH_ALG_AES;
            op->authMode = PAEIF_AUTH_MODE_GMAC;
            switch (x->ealg->alg_key_len) {
            case 128:
            case 192:
            case 256:
                break;
            default:
                printk(KERN_LEVEL "AES GMAC key length %d not supported\n", 
                        x->ealg->alg_key_len);
                status = PAEIF_HOST_ERROR_KEY_LENGTH;
                goto error;
                break;
            }
            break;
        default:
            printk(KERN_LEVEL "Unsuported encryption algorithm %d\n",
                   ealgd->desc.sadb_alg_id);
            status = PAEIF_HOST_ERROR_ENCR_ALG;
            goto error;
            break;
        }
        /* copy the keys to the CRYPTOOP structure */
        if (x->ealg->alg_key_len > MAX_ENC_KEY_SIZE) {
            status = PAEIF_HOST_ERROR_KEY_LENGTH;
            goto error;
        }
        if (op->encrAlg != PAEIF_ENCR_ALG_NULL) {
			memcpy (op->encrKey, x->ealg->alg_key, (x->ealg->alg_key_len + 7)/8); 
			op->encrKeyBits = x->ealg->alg_key_len;
            sa_flags->encr = 1;
		}
    }

	/* Add expected payload flags */
    if (x->props.mode == XFRM_MODE_TUNNEL) {
        sa_flags->tunnel = 1;
	}
    /* Determine which payloads will be added to the SA */

#ifdef USE_IPSEC_NAT_TRAVERSAL
    if ((sa_flags->ipv4) && (x->encap)) {
        sa_flags->nat = 1;
    }
#endif

	//DataDump ("SAID", said, sizeof(PAEIF_SAID));
	//DataDump ("OP", op, sizeof(PAEIF_CRYPTOOP));
    status = PaeIpsecAddSA(sa_flags, said, op);
    if (status) {
        printk(KERN_LEVEL "PaeIpsecAddSA returned %d\n", status);
        goto error;
    }

    tlv = (TLVWORD *)buff;
	memset (buff, 0, BUFF_SIZE); 

	// Add statically defined L2 header payloads
	// Temporary until user mode utility can provide this data
    if (x->props.mode == XFRM_MODE_TUNNEL) {
        tlv->type = TLV_L2HEADER_PAYLOAD;
        tlv->length = L2_HEADER_LEN - 2 + BRCM_HEADER_LEN;
		/* Omit the eth_type in order to avoid word misalignment */
		ptr = (uint8_t *)(buff + sizeof(TLVWORD) + BRCM_HEADER_LEN);
		if (!(sa_flags->inbound) && (sa_flags->ipv4)) {
			memcpy(ptr, ipv4_outbound_l2_header, L2_HEADER_LEN - 2); 
		} 
		if ((sa_flags->inbound) && (sa_flags->ipv4)) {
			memcpy(ptr, ipv4_inbound_l2_header, L2_HEADER_LEN - 2); 
		}
		if (!(sa_flags->inbound) && (sa_flags->ipv6)) {
			memcpy(ptr, ipv6_outbound_l2_header, L2_HEADER_LEN - 2); 
		} 
		if ((sa_flags->inbound) && (sa_flags->ipv6)) {
			memcpy(ptr, ipv6_inbound_l2_header, L2_HEADER_LEN - 2); 
		}
		ptr = (uint8_t *)tlv + sizeof(TLVWORD);
        status =  PaeIpsecAddPayload(said, tlv);
        if (status) {
            printk(KERN_LEVEL "PaeIpsecAddPayload returned %d\n", status);
            goto error;
		}
        /* Reset the buffer and tlv */
        memset((void *)buff, 0, BUFF_SIZE);
        tlv = (TLVWORD *)buff;
	}

    /* Check for tunnel and add TLV for tunnel payload */
    if (!(sa_flags->inbound)
                && (x->props.mode == XFRM_MODE_TUNNEL)) {
        ptr =  (uint8_t *)buff + sizeof(TLVWORD);
        if (sa_flags->ipv4) {
            if (x->props.saddr.a4 || x->id.daddr.a4) {
                /* Create IPV4 tunnel payload */
				memcpy(ptr, ipv4_outbound_l3_header, IPV4_TUNNEL_PRESET_LEN);  
				*(ptr + 9) = x->id.proto;
				ptr += IPV4_TUNNEL_PRESET_LEN;
                memcpy((void*)ptr, &x->props.saddr.a4, SIZE_IPV4_ADDR);
                memcpy(ptr + SIZE_IPV4_ADDR, &x->id.daddr.a4, SIZE_IPV4_ADDR);
				ptr =  (uint8_t *)buff + sizeof(TLVWORD);
				IPv4PartialChecksum(ptr, SIZE_IPV4_HEADER);
				tlv->length = SIZE_IPV4_HEADER;
            } 
        } else if (sa_flags->ipv6) {
            if (x->props.saddr.a6 || x->id.daddr.a6) {
                /* Create IPV6 tunnel payload */
				memcpy(ptr, ipv6_outbound_l3_header, IPV6_TUNNEL_PRESET_LEN);  
				*(ptr + 7) = x->id.proto;
				ptr += IPV6_TUNNEL_PRESET_LEN;
                memcpy(ptr, (void *)x->props.saddr.a6, SIZE_IPV6_ADDR);
                memcpy(ptr + SIZE_IPV6_ADDR, (void *)x->id.daddr.a6, SIZE_IPV6_ADDR);
                tlv->length = SIZE_IPV6_HEADER;
            } 
        }
        tlv->type = TLV_TUNNEL_PAYLOAD;
        /* Any updates to send after the SA */
		
		ptr = (uint8_t *)tlv + sizeof(TLVWORD);
        status =  PaeIpsecAddPayload(said, tlv);
        if (status) {
            printk(KERN_LEVEL "PaeIpsecAddPayload returned %d\n", status);
            goto error;
        }
        /* Reset the buffer and tlv */
        memset((void *)buff, 0, BUFF_SIZE);
        tlv = (TLVWORD *)buff;
    }

	{
		/* Update the SA with the src address */
		ptr = (uint8_t *)tlv + sizeof(TLVWORD);
        tlv->type = TLV_SRC_ADDR_PAYLOAD;
		if (sa_flags->ipv4) {
			tlv->length = SIZE_IPV4_ADDR;
			memcpy(ptr, &x->props.saddr.a4, SIZE_IPV4_ADDR);
		}
		if (sa_flags->ipv6) {
			tlv->length = SIZE_IPV6_ADDR;
            memcpy(ptr, (void *)x->props.saddr.a6, SIZE_IPV6_ADDR);
		}
        status =  PaeIpsecAddPayload(said, tlv);
        if (status) {
            printk(KERN_LEVEL "PaeIpsecAddPayload returned %d\n", status);
            goto error;
		}

        /* Reset the buffer and tlv */
        memset((void *)buff, 0, BUFF_SIZE);
        tlv = (TLVWORD *)buff;
	}

    /* Check for UDP encapsulation for IPv4 only */
    if ((sa_flags->ipv4) && (x->encap)) {
#ifdef USE_IPSEC_NAT_TRAVERSAL
        struct udphdr *uh = NULL;
        printk (KERN_LEVEL "found esp encaptype %d\n", x->encap->encap_type);
        switch (x->encap->encap_type) {
        case UDP_ENCAP_ESPINUDP_NON_IKE:
        case UDP_ENCAP_ESPINUDP:
            uh = (struct udphdr *)(buff + sizeof(TLVWORD));
            uh->source = x->encap->encap_sport;
            uh->dest = x->encap->encap_dport;
            tlv->type = TLV_NAT_UDP_PAYLOAD;
            tlv->length = sizeof(struct udphdr);

            /* Any updates to send after the SA */
			//DataDump ("NAT", tlv + 4, tlv->length);
            status =  PaeIpsecAddPayload(said, tlv);
            if (status) {
                printk(KERN_LEVEL "PaeIpsecAddPayload returned %d\n", status);
                goto error;
            }
            /* Reset the buffer and tlv */
            memset((void *)buff, 0, BUFF_SIZE);
            tlv = (TLVWORD *)buff;
            break;
        }
#endif /* USE_IPSEC_NAT_TRAVERSAL */
    }

error:
    if (status) {
	    printk(KERN_LEVEL "%s returned %d\n", __func__, status);
	}
    if (buff)
        kfree (buff);
    return status;
}

int PaeHostIpsecAddPolicy(unsigned dir, struct xfrm_policy *xp)
{
	int status = 0;
	uint8_t inbound = 0;
	uint8_t version = 0;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	uint16_t sport;
	uint16_t dport;
	uint16_t sport_mask;
	uint16_t dport_mask;
	uint8_t proto;
	int masked = 0;
	int bits;
	uint32_t *ptr32;
	int i;

	src = kmalloc(SIZE_IPV6_ADDR, GFP_KERNEL);
	if (!src) {
		status = -ENOMEM;
		goto done;
	}
	dst = kmalloc(SIZE_IPV6_ADDR, GFP_KERNEL);
	if (!dst) {
		status = -ENOMEM;
		goto done;
	}

	version = xp->selector.family ==  AF_INET  ? 4 : 6;
	inbound = (dir == IPSEC_DIR_INBOUND);
	memset(src, 0xff, SIZE_IPV6_ADDR);
	memset(dst, 0xff, SIZE_IPV6_ADDR);
	sport = xp->selector.sport;
	dport = xp->selector.dport;
	sport_mask = xp->selector.sport_mask;
	dport_mask = xp->selector.dport_mask;
	proto = xp->selector.proto;

	if (inbound) {
		// inbound directions do not need a mask
		goto skip_mask;
	}
	// If any wildcards then set the hash mask if not already set
	if (!sport || !dport || !proto) {
		masked = 1;
	}
	if ((sport_mask != 0xffff) || (dport_mask != 0xffff)) {
		masked = 1;
	}
	if ((version == 4) && (xp->selector.prefixlen_s < 32)) {
		// use src as the source mask
		*(uint32_t*)src = 0xffffffff << (32- xp->selector.prefixlen_s);
		masked = 1;
	}
	if ((version == 4) && (xp->selector.prefixlen_d < 32)) {
		// use dst as the destination mask
		*(uint32_t*)dst = 0xffffffff << (32- xp->selector.prefixlen_d);
		masked = 1;
	}
	if ((version == 6) && (xp->selector.prefixlen_s < 128)) {
		// use src as the source mask
		bits = 128 - xp->selector.prefixlen_s;
		ptr32= (uint32_t *)src;
		for (i = 0; i < 4; i++, ptr32++) {
			if ((bits > 0) && (bits < 32)) {
				*ptr32 = 0xffffffff << bits;
			}
			if (bits <= 0) {
				*ptr32 = 0;
			}
			bits -= 32;
		}
		masked = 1;
	}
	if ((version == 6) && (xp->selector.prefixlen_d < 128)) {
		// use dst as the destination mask
		bits = 128 - xp->selector.prefixlen_d;
		ptr32= (uint32_t *)dst;
		for (i = 0; i < 4; i++, ptr32++) {
			if ((bits > 0) && (bits < 32)) {
				*ptr32 = htonl(0xffffffff << bits);
			}
			if (bits <= 0) {
				*ptr32 = 0;
			}
			bits -= 32;
		}
		masked = 1;
	}
skip_mask:
	// Need to set one mask for all
	sport &= sport_mask;
	dport &= dport_mask;
	if (masked) {
		status = PaeIpsecSetHashMask(version, src, dst, sport, dport, proto);
		if (status) {
			goto done;
		}
	}
	// Now set the rules using the configured hash mask
	memset(src, 0, SIZE_IPV6_ADDR);
	memset(dst, 0, SIZE_IPV6_ADDR);
	if (version == 4) {
		memcpy(src, &xp->selector.saddr.a4, SIZE_IPV4_ADDR);
		memcpy(dst, &xp->selector.daddr.a4, SIZE_IPV4_ADDR);
	}
	if (version == 6) {
		memcpy(src, xp->selector.saddr.a6, SIZE_IPV6_ADDR);
		memcpy(dst, xp->selector.daddr.a6, SIZE_IPV6_ADDR);
	}
	if (dir == IPSEC_DIR_INBOUND) {
		sport = 0;
		dport = 0;
	}
	PaeIpsecCreateSnoopAction(inbound, version, src, dst, htons(sport), htons(dport), proto, masked);
done:
	if (src) {
		kfree(src);
	}
	if (dst) {
		kfree(dst);
	}
	return status;
}

EXPORT_SYMBOL(DataDump);
EXPORT_SYMBOL(Dump_skb);
EXPORT_SYMBOL(PaeHostIpsecAddSA);
EXPORT_SYMBOL(PaeHostIpsecAddPolicy);
EXPORT_SYMBOL(PaeHostIpsecDeleteSA);
EXPORT_SYMBOL(PaeIpsecSetPeerAddr);
