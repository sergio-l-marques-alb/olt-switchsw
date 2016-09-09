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
#include <linux/kernel.h>
#include <net/ip.h>
//#include <linux/pfkeyv2.h>
#include <linux/random.h>

#include "pae_spu.h"
#include "pae_ipsec.h"
#include "paemsg.h"
#include "paehost.h"
#include "paeipsecmsg.h"
#include "paemsghost.h"
#include "ipsec_notify.h"
#include "pae_cmds.h"
#include "pae_shared.h"


#define SA_PENDING_SPD_PAYLOAD       0x01
#define SA_PENDING_TUNNEL_PAYLOAD    0x02
#define SA_PENDING_NAT_PAYLOAD       0x04
#define SA_PENDING_LOCAL_MAC_PAYLOAD 0x08
#define SA_PENDING_SRC_ADDR_PAYLOAD  0x10

#define SNOOP_ACTION_UPDATED		 0xFF

typedef struct pae_sa_node_t PAESA_NODE; 

struct pae_sa_node_t {
	PAE_IPSEC_ACTION sa_action;
    uint16_t action_length;
	uint8_t  action_id;
	uint8_t  snoop_id;
    uint32_t payload_flags;
	uint8_t  peerAddr[SIZE_IPV6_ADDR];
	PAE_SNOOP_ACTION *snoop_action;
    PAESA_NODE *next;
};

static PAESA_NODE *pae_sa_pending = NULL;
static PAESA_NODE *pae_sa_configured = NULL;
static int action_idx = 1;

static struct ipv4_mask_t {
    uint32_t saddr;
	uint32_t daddr;
	uint16_t sport; 
	uint16_t dport; 
	uint8_t  proto;
	uint8_t ref_count;
} ipv4_mask = {0, 0, 0, 0, 0, 0};

static struct ipv6_mask_t {
    uint32_t saddr[4]; 
	uint32_t daddr[4]; 
	uint16_t sport; 
	uint16_t dport; 
	uint8_t  proto;
	uint32_t ref_count;
} ipv6_mask = {{0}, {0}, 0, 0, 0, 0};

static PAESA_NODE * AddSANode(PAEIF_SAFLAGS *sa_flags, PAEIF_SAID *said, PAESA_NODE *root_node) 
{
    PAESA_NODE *new_node = NULL;
    PAESA_NODE *sa_node = root_node;

    new_node = PAE_ALLOC (sizeof(PAESA_NODE));
    memset ((void *)new_node, 0, sizeof(PAESA_NODE));
    if (!new_node) {
        printk(KERN_LEVEL "Cannot allocate memory");
        return NULL;
    }
    memcpy(&new_node->sa_action.said, said, sizeof(PAEIF_SAID));
    memcpy(&new_node->sa_action.sa_flags, sa_flags, sizeof(PAEIF_SAFLAGS));
    new_node->action_length = sizeof(PAEIF_SAID) + sizeof(PAEIF_SAFLAGS);
    new_node->next = NULL;
    new_node->payload_flags = 0;
    if ((sa_flags->tunnel) && (!sa_flags->inbound)) {
        new_node->payload_flags |= SA_PENDING_TUNNEL_PAYLOAD;
	}
    if (sa_flags->tunnel) {
        new_node->payload_flags |= SA_PENDING_SRC_ADDR_PAYLOAD;
	}

    /* insert it in the linked list of pending SA  */
    if (pae_sa_pending == NULL) {
        /* insert at the front */
        pae_sa_pending = new_node;    
    } else {
        while (sa_node->next) {
            sa_node = sa_node->next;
        }
        sa_node->next = new_node;
    }
    return new_node;
}

static int AddSAConfigured(PAESA_NODE *sa_add)
{
    PAESA_NODE *sa_node = pae_sa_configured;

    /* insert it in the linked list of pending SA  */
    if (pae_sa_configured == NULL) {
        /* insert at the front */
        pae_sa_configured = sa_add;    
    } else {
        while (sa_node->next) {
            sa_node = sa_node->next;
        }
        sa_node->next = sa_add;
    }
	//printk("Add SA configured node %08x, SPI %08x\n", (unsigned)sa_add, sa_add->sa_action.said.spi);
	return 0;
}

static int RemoveConfiguredSA(PAESA_NODE *sa_delete)
{
    PAESA_NODE *sa_node = pae_sa_configured;

    if (pae_sa_configured == sa_delete) {
        pae_sa_configured = NULL;
        goto done;
    } else {
        while (sa_node->next) {
            if (sa_node->next == sa_delete) {
                sa_node->next = sa_delete->next;
                goto done;
            }
            sa_node = sa_node->next;
        }
    }
    return PAEIF_HOST_ERROR_SA_NOT_FOUND;
done:
    PAE_FREE(sa_delete);
    return 0;
}

static PAESA_NODE * AddSAPending(PAEIF_SAFLAGS *sa_flags, PAEIF_SAID *said) {
    return AddSANode (sa_flags, said, pae_sa_pending);
}

static int CheckSAIDMatch(PAEIF_SAID *sa_id, PAE_IPSEC_ACTION *sa_action)
{
    if ((sa_id->spi == sa_action->said.spi) 
            && (!memcmp(sa_id->v6dstAddr,  sa_action->said.v6dstAddr, SIZE_IPV6_ADDR))) {
		// This is a match
        return 0;
    }
    return 1;
}

int GetSAIndex (PAEIF_SAID *sa_id)
{
	int idx = UNKNOWN_SA_IDX;
    PAESA_NODE *sa_node = pae_sa_configured;

	while (sa_node) {
		if (!CheckSAIDMatch(sa_id, &sa_node->sa_action)) {
			idx = sa_node->action_id; 
			break;
		}
		sa_node = sa_node->next;	
	}
	return (idx);
}


static PAESA_NODE * GetSAPending(PAEIF_SAID *sa_id)
{
    PAESA_NODE *sa_node = pae_sa_pending;

    while (sa_node) {
        if (!CheckSAIDMatch(sa_id, &sa_node->sa_action)) {
            /* This is the one we want */
            return sa_node;
        }
        sa_node = sa_node->next;
    }
    return NULL;
}

static int SubmitIpsecConfigured(PAESA_NODE *sa_pending)
{
    PAESA_NODE *sa_node = pae_sa_pending;

	/* remove it from the pending list */
    if (pae_sa_pending == sa_pending) {
        pae_sa_pending = NULL;
        goto done;
    } else {
        while (sa_node->next) {
            if (sa_node->next == sa_pending) {
                sa_node->next = sa_pending->next;
                goto done;
            }
            sa_node = sa_node->next;
        }
    }
    return PAEIF_HOST_ERROR_SA_NOT_FOUND;
done:
    AddSAConfigured(sa_pending);
    return 0;
}

PAESA_NODE *PaeFindTwinNode(PAESA_NODE *node)
{
	PAESA_NODE *twin_node = pae_sa_configured;

	if (node->sa_action.twin_sa) {
		// we already found the twin_sa
		while (twin_node) {
			if (twin_node != node) {
				if (node->sa_action.twin_sa == (void*)&twin_node->sa_action) {
					break;
				}
			}
			twin_node = twin_node->next;
		}
		return twin_node;
	}

	// Establish the twin sa
	while (twin_node) {
		if (twin_node != node) {
			if (!twin_node->sa_action.twin_sa) {
				if ((node->sa_action.sa_flags.inbound && !twin_node->sa_action.sa_flags.inbound)
						|| (!node->sa_action.sa_flags.inbound && twin_node->sa_action.sa_flags.inbound)) {
					twin_node->sa_action.twin_sa = (void*)&node->sa_action;
					node->sa_action.twin_sa = (void*)&twin_node->sa_action;
					if (!node->sa_action.sa_flags.inbound) {
						memcpy ((void *)node->peerAddr, (void*)twin_node->peerAddr, SIZE_IPV6_ADDR);
					} else {
						memcpy ((void *)twin_node->peerAddr, (void*)node->peerAddr, SIZE_IPV6_ADDR);
					}
					break;
				}
			}
		}
		twin_node = twin_node->next;
	}
	return twin_node;
}


static int PaeAddIpsecAction (PAESA_NODE *sa_node)
{
	PAE_IPSEC_ACTION *sa_action;
	BDESC_HEADER *bdesc_hdr;
    uint32_t status = PAE_STATUS_OK;
	uint16_t l3_hdr_len;
    uint8_t *ptr;
	uint16_t action_mtu_subtract = 0;

	/* First move it from the pending list to the configured list */
    SubmitIpsecConfigured(sa_node);
	sa_action = &sa_node->sa_action;

	if (!(sa_action->sa_flags.inbound)) {
		action_mtu_subtract += 4 + 4 + 2; // SPI,  SEQ NUM, next proto 
		if (sa_action->sa_flags.ipv4) {
			action_mtu_subtract += SIZE_IPV4_HEADER;
		}
		if (sa_action->sa_flags.ipv6) {
			action_mtu_subtract += SIZE_IPV6_HEADER;
		}
		if (sa_action->sa_flags.encr) {
			action_mtu_subtract += 2 * sa_action->sa_flags.ivLen; // IV and Pad
		}
	}
	sa_action->action_len--;
	sa_action->action_type = PAE_ACTION_TYPE_IPSEC;
	sa_action->action_mtu = 1500 - action_mtu_subtract; 
	sa_action->mtu = 1500 - action_mtu_subtract; 
	sa_node->action_id = action_idx;

	/* Format the BDESC header */
	bdesc_hdr = (BDESC_HEADER *)((uint8_t*)sa_action->dma_ctx + sa_action->bdesc_offset);
	if (sa_action->sa_flags.tunnel) {
		l3_hdr_len = 0;
		if (sa_action->sa_flags.ipv4 && !sa_action->sa_flags.inbound) {
			l3_hdr_len += SIZE_IPV4_HEADER;
		}
		if (sa_action->sa_flags.ipv6 && !sa_action->sa_flags.inbound) {
			l3_hdr_len += SIZE_IPV6_HEADER;
		}
		bdesc_hdr->offsetMAC = htons(l3_hdr_len);
		bdesc_hdr->offsetIV = htons(l3_hdr_len  + 2*sizeof(uint32_t));
		bdesc_hdr->offsetCrypto = htons(l3_hdr_len  
					 + 2*sizeof(uint32_t) + sa_action->sa_flags.ivLen);
	}

	ptr = (uint8_t *)sa_action + 4;
    //DataDump ("Add sa_action", ptr, sa_action->action_len*4);

	// Sanity check the size of the IPsec actions
	if (!sa_action->sa_flags.inbound && (sa_action->action_len*4 > MAX_OUTBOUND_SA_ACTION_LEN)) {
		status = PAEIF_HOST_ERROR_SA_OUTBOUND_LEN;
        printk(KERN_LEVEL "IPsec outbound action len %d exceed max %d\n", 
				sa_action->action_len*4, MAX_OUTBOUND_SA_ACTION_LEN);
		goto error;
	}

	if (sa_action->sa_flags.inbound && (sa_action->action_len*4 > MAX_INBOUND_SA_ACTION_LEN)) {
		status = PAEIF_HOST_ERROR_SA_INBOUND_LEN;
        printk(KERN_LEVEL "IPsec inbound action len %d exceed max %d\n", 
				sa_action->action_len*4, MAX_INBOUND_SA_ACTION_LEN);
		goto error;
	}

	// Maintain a minimum size to be used for the Centurion DE registers
	if (sa_action->action_len*4 < MIN_SA_ACTION_LEN) {
		sa_action->action_len = MIN_SA_ACTION_LEN/4;
	}

	//printk("Adding Ipsec action %d mtu %u\n", sa_node->action_id, sa_action->action_mtu); 
    status = pae_add_action_with_mtu(action_idx, PAE_ACTION_TYPE_IPSEC, 
					sa_action->action_mtu, ptr, sa_action->action_len*4);
    if (status == PAE_ERR_INVALID_ACTION_IDX) {
		action_idx++;
		status = pae_add_action_with_mtu(action_idx, PAE_ACTION_TYPE_IPSEC, 
						sa_action->action_mtu, ptr, sa_action->action_len*4);
	}
    if (status) {
       printk(KERN_LEVEL "pae_add_action_with_mtu returned %d idx %d\n", status, action_idx);
	   goto error;
    }
	printk(KERN_LEVEL "Added Ipsec action %d length %d\n", action_idx, sa_action->action_len*4);

	if (PaeFindTwinNode(sa_node)) {
		printk("twin nodes SPI %x %x\n", sa_node->sa_action.said.spi, ((PAE_IPSEC_ACTION*)(sa_node->sa_action.twin_sa))->said.spi);
	} else {
		printk("No twin node\n");
	}
	// Zero out the crypto keys 
	memset(&sa_action->dma_ctx[SCTX_WORDS*4], 0, 4*(MAX_AUTH_KEY_WORDS+MAX_CIPHER_KEY_WORDS));
	action_idx++;

error:
    return status;
}

//=================== IPsec Stack PAE Interface =====================

/*-------------------------------------------------------------------------
 * Name    : PaeIpsecAddPayload
 * Purpose : Process an SA Payload
 * Input   : 
 * Return  : Error status
 * Remarks : 
 *------------------------------------------------------------------------*/
int PaeIpsecAddPayload(PAEIF_SAID *sa_id, TLVWORD *tlv)
{
    int status = PAEIF_STATUS_OK;
    void *ptr;
    PAESA_NODE *sa_node;
    uint32_t encap_len;
    int ivSize = 0;
	int n;

	ptr = (uint8_t*)tlv + 4;
    //printk(KERN_LEVEL "processing SA payload type %d\n", tlv->type);
	//DataDump ("payload", ptr, tlv->length);

    sa_node = GetSAPending(sa_id);
    if (!sa_node) {
        printk(KERN_LEVEL "SA not found\n");
        status = PAEIF_HOST_ERROR_SA_NOT_FOUND;
        goto error;
    }

    switch (tlv->type) {
	case TLV_SRC_ADDR_PAYLOAD:
		/* Copy it to the src address field */
		if (sa_node->sa_action.sa_flags.inbound) {
			/* only the inbound direction has the address of the peer */
			memcpy ((void *)sa_node->peerAddr, (void*)tlv + sizeof(TLVWORD), tlv->length);
		}

        sa_node->payload_flags &= ~SA_PENDING_SRC_ADDR_PAYLOAD;
		break;

	case TLV_L2HEADER_PAYLOAD:
		/* Write it to the L2 data location */
		memcpy ((void *)sa_node->sa_action.l2_data, (void*)tlv + sizeof(TLVWORD), tlv->length);  

        sa_node->payload_flags &= ~SA_PENDING_LOCAL_MAC_PAYLOAD;
		break;

	case TLV_TUNNEL_PAYLOAD:
		if (!sa_node->sa_action.sa_flags.inbound) {
		    /* Add the tunnel header at the end of the current DMA ctx */
			ptr = sa_node->sa_action.dma_ctx + sa_node->sa_action.dma_ctx_len;  
			memcpy ((void *)ptr, (void*)tlv + sizeof(TLVWORD), tlv->length);  
			sa_node->sa_action.dma_ctx_len += tlv->length;
			sa_node->action_length += tlv->length;

			/* add the IPSEC header */
			if (sa_node->sa_action.sa_flags.encr) {
				ivSize = sa_node->sa_action.sa_flags.ivLen;
			}

			ptr += tlv->length;
			/* Write the SPI and start sequence number */
			*(uint32_t*)ptr = sa_node->sa_action.said.spi;
			ptr += 4;
			*(uint32_t*)ptr = 0;
			ptr += 4;
			/* Write the first IV */
			for (n = 0; n < ivSize; n += 4) 
				*(uint32_t*)(ptr + n) = random32();

			/* skip space for SPI, sequence number and IV */
			encap_len = 2 * sizeof(uint32_t) + ivSize;
			sa_node->sa_action.dma_ctx_len += encap_len;
			sa_node->action_length += encap_len;
			
			sa_node->payload_flags &= ~SA_PENDING_TUNNEL_PAYLOAD;
		}
		break;

	case TLV_NAT_UDP_PAYLOAD:
		// TBD
		break;

    default:
        printk(KERN_LEVEL "Payload type %d not supported\n", tlv->type);
        status = PAE_ERROR_TLV_TYPE;
        goto error;
    }

	/* Action length is stored as action word length */
	sa_node->sa_action.action_len = sa_node->action_length/4;
	if (sa_node->action_length % 4) {
		sa_node->sa_action.action_len++;
	}

	//DataDump ("SA after payload", (void*)&sa_node->sa_action, sa_node->action_length);
    if (!sa_node->payload_flags) {
	    // This action is ready to be submitted
        status = PaeAddIpsecAction (sa_node);
        if (status) {
			goto error;
        }
    }
    return 0;

error:
    printk(KERN_LEVEL "%s returned %d\n", __func__, status);
    return status;
}

/*-------------------------------------------------------------------------
 * Name    : PaeIpsecDeleteSA
 * Purpose : Delete the IPsec action
 * Input   : 
 * Return  : Error status
 * Remarks : 
 *------------------------------------------------------------------------*/
int PaeIpsecDeleteSA(unsigned idx)
{
	PAESA_NODE *sa_node = NULL;
	int status;

    sa_node = pae_sa_configured;
	while (sa_node) {
		if (sa_node->action_id == idx) {
			RemoveConfiguredSA(sa_node);
			status = 0xff;
			break;
		}
		sa_node = sa_node->next;
	}
	if (status != 0xff) {
		printk (KERN_LEVEL "PaeIpsecDeleteSA SA %d not found\n", idx);
		return PAEIF_HOST_ERROR_SA_NOT_FOUND;
	}
    status = pae_delete_action(idx);
    if (status) {
       printk(KERN_LEVEL "pae_delete_action returned %d\n", status);
    }
    return status;
}

/*-------------------------------------------------------------------------
 * Name    : PaeIpsecAddSA
 * Purpose : Create the PAE Context for the SA and submit to the PAE 
 * Input   : 
 * Return  : Error status
 * Remarks : 
 *------------------------------------------------------------------------*/
int PaeIpsecAddSA(PAEIF_SAFLAGS *sa_flags, PAEIF_SAID *sa_id, PAEIF_CRYPTOOP *op)
{
    int status = PAEIF_STATUS_OK;
	uint8_t *dma_ctx;
    uint8_t *ptr;
    SPUHEADER *spuh;
    PAESA_NODE *sa_node;
    unsigned sa_length;
    uint32_t authAlg = 0;
    uint32_t encrAlg = 0;
    uint32_t protocol_bits = 0;
    uint32_t cipher_bits = 0;
    uint32_t ecf_bits = 0;
	uint8_t  sctx_words = 0;

	if (sa_flags->esp) {
		printk(KERN_LEVEL "SA ESP SPI %08x\n", htonl(sa_id->spi));
	}

    dma_ctx = (uint8_t *)PAE_ALLOC(MAX_DMA_CTX_LEN);
	if (!dma_ctx) {
	    printk(KERN_LEVEL "Cannot allocate memory");
	    status = PAEIF_HOST_ERROR_ALLOC;
	    goto error;
	}
	memset(dma_ctx, 0, MAX_DMA_CTX_LEN);
    /* SPU header immediately follows the add header */
    spuh = (SPUHEADER *)dma_ctx;
    /* set the message payload header parameters */
    sa_length = sizeof(SPUHEADER);

    /* format the SPU headers */

    /* format master header word */
	// Do not set the next bit even though the datasheet says to
    spuh->mh.opCode = SPU_CRYPTO_OPERATION_GENERIC;
    spuh->mh.flags.SCTX_PR = 1;
    spuh->mh.flags.BDESC_PR = 1;
    spuh->mh.flags.BD_PR = 1;

    /* Use the emh to store the SPI of the SA */
    spuh->emh = sa_id->spi;

    /* Format sctx word 0 */
    protocol_bits |= SPU_SCTX_TYPE_GENERIC << SCTX_TYPE_SHIFT;
    sctx_words = 3; /* size so far in words */

#ifdef USE_CENTURION_SPU
    if (sa_flags->esp) {
        protocol_bits |= 1 << PROTOCOL_ESP_SHIFT;
    }
    if (sa_flags->ah) {
        protocol_bits  |= 1 << PROTOCOL_AH_SHIFT;
    }

    if ((sa_flags->tunnel) && !(sa_flags->inbound)) {
        protocol_bits |= 1 << CAP_EN_SHIFT;
        protocol_bits |= 1 << PAD_EN_SHIFT;
//        protocol_bits |= 1 << GEN_ID_SHIFT;
        protocol_bits |= 1 << COPY_TOS_SHIFT;
        protocol_bits |= 1 << COPY_FLOW_SHIFT;
        protocol_bits |= 1 << UPDATE_EN_SHIFT;
	}

    if ((sa_flags->esp) && (!sa_flags->tunnel)) {
        protocol_bits |= 1 << TRANSPORT_MODE_SHIFT;
    }
    
	if (sa_flags->esn) {
        protocol_bits |= 1 << AUTH_SEQ64_SHIFT;
    }
#endif

    /* Format sctx word 1 */
    if (sa_flags->inbound) {
        cipher_bits |= 1 << CIPHER_INBOUND_SHIFT;
    }

    /* Format sctx word 2 */

    {
		// Outbound
		if (sa_flags->encr) {
			ecf_bits |= sa_flags->ivLen / 4; 
			// Use explicit IV
			ecf_bits |= 1 << EXPLICIT_IV_SHIFT;  
		}
	}

    /* Specify the authentication protocol if any */
    switch (op->authAlg) {
        case PAEIF_AUTH_ALG_NONE:
            break;
        case PAEIF_AUTH_ALG_MD5:
            if (op->authMode != PAEIF_AUTH_MODE_HMAC)
                goto autherror;
            authAlg = AALGINFO_HMAC_MD5;
            break;
        case PAEIF_AUTH_ALG_SHA1:
            if (op->authMode != PAEIF_AUTH_MODE_HMAC)
                goto autherror;
            authAlg = AALGINFO_HMAC_SHA1;
            break;
        case PAEIF_AUTH_ALG_SHA224:
            if (op->authMode != PAEIF_AUTH_MODE_HMAC)
                goto autherror;
            authAlg = AALGINFO_HMAC_SHA224;
            break;
        case PAEIF_AUTH_ALG_SHA256:
            if (op->authMode != PAEIF_AUTH_MODE_HMAC)
                goto autherror;
            authAlg = AALGINFO_HMAC_SHA256;
            break;
        case PAEIF_AUTH_ALG_AES:
            if (op->authMode == PAEIF_AUTH_MODE_XCBC)
                authAlg = AALGINFO_XCBC_MAC;
            else if (op->authMode == PAEIF_AUTH_MODE_GMAC) {
                switch (op->authKeyBits) {
                case 128:
                    authAlg = AALGINFO_AES128_GMAC;
                    break;
                case 192:
                    authAlg = AALGINFO_AES192_GMAC;
                    break;
                case 256:
                    authAlg = AALGINFO_AES256_GMAC;
                    break;
                default:
                    goto autherror;
                }
            } else {
                goto autherror;
            }
            break;
        default:
autherror:
            printk(KERN_LEVEL "Unsupported auth algorithm %d auth mode %d\n", op->authAlg, op->authMode);
            status = PAEIF_HOST_ERROR_AUTH_ALG;
            goto error;
    }    

    /* Set the encryption protocol */
    if (sa_flags->esp) {    
        switch (op->encrAlg) {
        case PAEIF_ENCR_ALG_NULL:
            encrAlg = EALGINFO_NULL;
           break;
        case PAEIF_ENCR_ALG_DES:
            if (op->encrMode != PAEIF_ENCR_MODE_CBC)
                goto encrerror;
            encrAlg = EALGINFO_DES_CBC;
            break;
        case PAEIF_ENCR_ALG_3DES:
            if (op->encrMode != PAEIF_ENCR_MODE_CBC)
                goto encrerror;
            encrAlg = EALGINFO_3DES_CBC;
            break;
        case PAEIF_ENCR_ALG_AES:
            switch (op->encrMode) {
            case PAEIF_ENCR_MODE_CBC:
                switch (op->encrKeyBits) {
                case 128:
                    encrAlg = EALGINFO_AES128_CBC;
                    break;
                case 192:
                    encrAlg = EALGINFO_AES192_CBC;
                    break;
                case 256:
                    encrAlg = EALGINFO_AES256_CBC;
                    break;
                default:
                    printk(KERN_LEVEL "Unsupported AES key length %d\n", op->encrKeyBits);
                    status = PAEIF_HOST_ERROR_KEY_LENGTH;
                    goto error;
                }
                break;
            case PAEIF_ENCR_MODE_CTR:
                switch (op->encrKeyBits) {
                case 128:
                    encrAlg = EALGINFO_AES128_CTR;
                    break;
                case 192:
                    encrAlg = EALGINFO_AES192_CTR;
                    break;
                case 256:
                    encrAlg = EALGINFO_AES256_CTR;
                    break;
                default:
                    printk(KERN_LEVEL "Unsupported AES key length %d\n", op->encrKeyBits);
                    status = PAEIF_HOST_ERROR_KEY_LENGTH;
                    goto error;
                }
                break;
            case PAEIF_ENCR_MODE_GCM:
                /* Encryption AND authentication algorithm rfc4106 */
                switch (op->encrKeyBits) {
                case 128:
                    encrAlg = EALGINFO_AES128_GCM;
                    break;
                case 192:
                    encrAlg = EALGINFO_AES192_GCM;
                    break;
                case 256:
                    encrAlg = EALGINFO_AES256_GCM;
                    break;
                default:
                    printk(KERN_LEVEL "Unsupported AES key length %d\n", op->encrKeyBits);
                    status = PAEIF_HOST_ERROR_KEY_LENGTH;
                    goto error;
                }
                break;
            default:
                goto encrerror;
            }
			break;
                
        default:
encrerror:
            printk(KERN_LEVEL "Unsupported encr alg %d mode %d\n", op->encrAlg, op->encrMode);
            status = PAEIF_HOST_ERROR_ENCR_ALG;
            goto error;
        }
    }	

    ptr = (uint8_t *)spuh + sizeof(SPUHEADER);

    /* Set the auth parameters in the cipher.flags */
    switch (authAlg) {
    case AALGINFO_NONE:
        /* Do nothing here */
        break;

#ifdef USE_FHMAC_MODE
        /* FHMAC uses inner and outer contexts not keys */
    case AALGINFO_FHMAC_MD5:
        cipher_bits |= HASH_ALG_MD5 << HASH_ALG_SHIFT;
        cipher_bits |= HASH_MODE_FHMAC << HASH_MODE_SHIFT;
        cipher_bits |= HASH_TYPE_FULL << HASH_TYPE_SHIFT;
        break; 
    case AALGINFO_FHMAC_SHA1:
        cipher_bits |= HASH_ALG_SHA1 << HASH_ALG_SHIFT;
        cipher_bits |= HASH_MODE_FHMAC << HASH_MODE_SHIFT;
        cipher_bits |= HASH_TYPE_FULL << HASH_TYPE_SHIFT;
        break;
#endif
        /* All other HMAC mode */
    case AALGINFO_HMAC_SHA1:
        cipher_bits |= HASH_ALG_SHA1 << HASH_ALG_SHIFT;
        cipher_bits |= HASH_MODE_HMAC << HASH_MODE_SHIFT;
        cipher_bits |= HASH_TYPE_FULL << HASH_TYPE_SHIFT;
        break;
    case AALGINFO_HMAC_MD5:
        cipher_bits |= HASH_ALG_MD5 << HASH_ALG_SHIFT;
        cipher_bits |= HASH_MODE_HMAC << HASH_MODE_SHIFT;
        cipher_bits |= HASH_TYPE_FULL << HASH_TYPE_SHIFT;
        break;
    case AALGINFO_HMAC_SHA224:
        cipher_bits |= HASH_ALG_SHA224 << HASH_ALG_SHIFT;
        cipher_bits |= HASH_MODE_HMAC << HASH_MODE_SHIFT;
        cipher_bits |= HASH_TYPE_FULL << HASH_TYPE_SHIFT;
        break;
    case AALGINFO_HMAC_SHA256:
        cipher_bits |= HASH_ALG_SHA256 << HASH_ALG_SHIFT;
        cipher_bits |= HASH_MODE_HMAC << HASH_MODE_SHIFT;
        cipher_bits |= HASH_TYPE_FULL << HASH_TYPE_SHIFT;
        break;
    default:
        printk(KERN_LEVEL "Unsupported auth algorithm %d\n", authAlg);
        status = PAEIF_HOST_ERROR_AUTH_ALG;
        goto error;
        break;
    }
    
	/* Write the authentication key material */
    switch (authAlg) {
    case AALGINFO_NONE:
        /* Do nothing here */
        break;

#ifdef USE_FHMAC_MODE
        /* FHMAC uses inner and outer contexts not keys */
    case AALGINFO_FHMAC_MD5:
    case AALGINFO_FHMAC_SHA1:
        create_hmac_context(op->authKey, ptr, HMAC_INNER_CONTEXT);
        ptr += op->authKeyBits/8;
        create_hmac_context(x, ptr, HMAC_OUTER_CONTEXT);
        ptr += op->authKeyBits/8;
        sa_length += 2 * op->authKeyBits/8;
        sctx_words += 2 * op->authKeyBits/32;
        ecf_bits |= 3 << ICV_SIZE_SHIFT;  
        break;
#endif
        /* All other HMAC mode */
    case AALGINFO_HMAC_SHA1:
    case AALGINFO_HMAC_MD5:
    case AALGINFO_HMAC_SHA224:
    case AALGINFO_HMAC_SHA256:
        memcpy(ptr, op->authKey, op->authKeyBits/8);
        ptr += op->authKeyBits/8;
        sctx_words += op->authKeyBits/32;
		if (authAlg == AALGINFO_HMAC_SHA256) {
			ecf_bits |= 4 << ICV_SIZE_SHIFT;  
		} else {
			ecf_bits |= 3 << ICV_SIZE_SHIFT;  
		}
        sa_length += op->authKeyBits/8;
        break;

    default:
        printk(KERN_LEVEL "Unsupported crypto algorithm %d\n", encrAlg);
        status = PAEIF_HOST_ERROR_AUTH_ALG;
        goto error;
        break;
    }

    /* Set the crypto parameters in the cipher.flags */
    switch (encrAlg) {
    case EALGINFO_NULL:
        /* Do nothing here */
        break;

    case EALGINFO_DES_CBC:
        cipher_bits |= CIPHER_ALG_DES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CBC << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_NONE << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_3DES_CBC:
        cipher_bits |= CIPHER_ALG_3DES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CBC << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_NONE << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES128_CBC:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CBC << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES128 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES192_CBC:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CBC << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES192 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES256_CBC:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CBC << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES256 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES128_CTR:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CTR << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES128 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES192_CTR:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CTR << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES192 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES256_CTR:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_CTR << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES256 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES128_GCM:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_GCM << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES128 << CIPHER_TYPE_SHIFT;
        break; 
    case EALGINFO_AES192_GCM:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_GCM << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES192 << CIPHER_TYPE_SHIFT;
        break;
    case EALGINFO_AES256_GCM:
        cipher_bits |= CIPHER_ALG_AES << CIPHER_ALG_SHIFT;
        cipher_bits |= CIPHER_MODE_GCM << CIPHER_MODE_SHIFT;
        cipher_bits |= CIPHER_TYPE_AES256 << CIPHER_TYPE_SHIFT;
        break;
    }

    /* copy the encryption keys in the SAD entry */
    if (encrAlg !=  EALGINFO_NULL) {
        memcpy(ptr, op->encrKey, op->encrKeyBits/8);
        ptr += op->encrKeyBits/8;
        sctx_words += op->encrKeyBits/32;
        sa_length += op->encrKeyBits/8;
        if (op->encrMode == PAEIF_ENCR_MODE_CTR) {
			/* AES CTR structure follows keyes */
			sctx_words += CTR_IV_SIZE/4;  
			sa_length += CTR_IV_SIZE;
			/* Create the counter NONCE value */
			*(uint32_t*)ptr = random32();
			ptr += 4;
			/* initialize the first IV */
			*(uint32_t*)ptr = random32();
			ptr += 4;
			*(uint32_t*)ptr = random32();
			ptr += 4;
			/* Initialize the counter value */
			*(uint32_t *)ptr = 1;  
			ptr += 4;
		}
    }
    protocol_bits |= sctx_words;
	spuh->sa.protocol.bits = htonl(protocol_bits);
	spuh->sa.cipher.bits = htonl(cipher_bits);
	spuh->sa.ecf.bits = htonl(ecf_bits);

    sa_node = GetSAPending (sa_id);
    if (!sa_node) {
        /* This is a new SA */
        sa_node = AddSAPending (sa_flags, sa_id);
        if (!sa_node) {
            status = PAEIF_HOST_ERROR_ALLOC;
            goto error;
        }
        memcpy(&sa_node->sa_action.said, sa_id, sizeof(PAEIF_SAID));
        memcpy(&sa_node->sa_action.dma_ctx, dma_ctx, sa_length);
		sa_node->sa_action.bdesc_offset = sa_length;
	    /* Add space for BD and BDESC */
        sa_length += sizeof(BD_HEADER) + sizeof(BDESC_HEADER);
		/* Formatting the BD and BDESC must happen after all payloads have been added */
		sa_node->action_length = sa_length + DMA_CTX_ACTION_OFFSET;
        sa_node->sa_action.dma_ctx_len = sa_length;
		sa_node->sa_action.action_len = sa_node->action_length/4;
		if (sa_node->action_length % 4) {
			sa_node->sa_action.action_len++;
		}

        //DataDump ("Initial SA", &sa_node->sa_action, sa_node->action_length);
        //DataDump ("dma_ctx", sa_node->sa_action.dma_ctx, sa_node->sa_action.dma_ctx_len);
    } else {
		printk(KERN_LEVEL "SAID already exists\n");
		goto error;
	}

    /* check if ready to send as is */
    if (!sa_node->payload_flags) {
        status = PaeAddIpsecAction (sa_node);
        if (status) {
			goto error;
        }
    }

error:
    if (status) 
        printk(KERN_LEVEL "%s is returning %d\n", __func__, status);
    if (dma_ctx)
        PAE_FREE (dma_ctx);
    return status;
}

int PaeIpsecGetSAIVLen(uint32_t spi, int inbound)
{
	PAESA_NODE *node = pae_sa_configured;
	PAE_IPSEC_ACTION *sa_action;
	int ivLen;
	
	while (node) {
		sa_action = &node->sa_action;
		if (sa_action->sa_flags.inbound && !inbound) {
			node = node->next;
			continue;
		}
		if (!sa_action->sa_flags.inbound && inbound) {
			node = node->next;
			continue;
		}
		if (sa_action->said.spi == spi) {
			ivLen = sa_action->sa_flags.ivLen;
			return ivLen;
		}
		node = node->next;
	}
	return -1;
}

PAESA_NODE *PaeIpsecGetMatchingSA(uint8_t inbound, uint8_t ip_ver) 
{
	PAESA_NODE *node = pae_sa_configured;
	// Look for the SA in the correct direction which does not have a snoop_id associated with it 
	while (node) {
		if ((node->sa_action.sa_flags.inbound && inbound) 
				|| (!node->sa_action.sa_flags.inbound && !inbound)) {
			printk("%s %sbound node SPI %08x\n", __FUNCTION__, inbound?"in":"out", node->sa_action.said.spi);
			if (!node->snoop_id) {
				break;
			}
		}
		node = node->next;
	}
	return node;
}

PAESA_NODE *PaeIpsecGetSnoopSA(uint8_t inbound, uint8_t ip_ver, uint8_t *src, uint8_t *dst)
{
	PAESA_NODE *node = pae_sa_pending;

	/* Search any configured SA for an SA of the same direction for which the snoop_id has not been updated */
	node = pae_sa_configured;
	while (node) {
		if (node->sa_action.sa_flags.inbound && !inbound) {
			node = node->next;
			continue;
		}
		if (!node->sa_action.sa_flags.inbound && inbound) {
			node = node->next;
			continue;
		}
		if (node->snoop_id) {
			if (node->snoop_id != SNOOP_ACTION_UPDATED) {
				break;
			}
		}
		node = node->next;
	}
	return node;
}

int PaeSnoopActionUpdate(uint8_t inbound, uint8_t version, uint8_t *need0, 
		uint8_t *need1, uint8_t *have0, uint8_t *have1)
{
    int status = PAEIF_STATUS_OK;
	PAE_SNOOP_ACTION *snoop_action;
	PAESA_NODE *node;
	int size, update_size; 

	if (version != 4) {
		// IPv6 is TBD
		return PAEIF_HOST_ERROR_IP_VER;
	}
	size = SIZE_IPV4_ADDR;
	node = PaeIpsecGetSnoopSA(inbound, version, have0, have1);
	if (!node) {
		printk("Did not get snoop action id for have0 %08x have1 %08x\n", *(uint32_t*)have0, *(uint32_t*)have1);
		goto done;
	}
	snoop_action = node->snoop_action;
	//printk("Snoop action id %08x:%x need %08x and %08x mask %08x\n", 
	//		snoop_action->snoopActionId, node->snoop_id, *(uint32_t*)need0, *(uint32_t*)need1, *(uint32_t*)snoop_action->subnetMask);

	snoop_action->switch_mode = 1;
	memcpy(snoop_action->ipAddr0, need0, size);
	memcpy(snoop_action->ipAddr1, need1, size);
	memcpy(snoop_action->peerAddr, node->peerAddr, size);
	update_size = 3 * size + 1;
	//DataDump("action update", (uint8_t*)snoop_action, 11 + update_size);
    status = pae_update_action(snoop_action->snoopActionId, 4 + 11, (uint8_t*)&snoop_action->switch_mode, update_size);
    if (status) {
       printk(KERN_LEVEL "pae_update_action returned %d\n", status);
	   goto done;
    }
	node->snoop_id = SNOOP_ACTION_UPDATED;
	//printk(KERN_LEVEL "Updated snoop action %d length %d\n", snoop_action->snoopActionId, sizeof(PAE_SNOOP_ACTION));
done:
	return status;
}

int PaeIpsecCreateSnoopAction(uint8_t inbound, uint8_t version, uint8_t *src, 
		uint8_t *dst, uint16_t sport, uint16_t dport, uint8_t proto, uint8_t masked)
{
    int status = PAEIF_STATUS_OK;
	PAESA_NODE *node, *twin_node;
	PAE_SNOOP_ACTION *snoop_action;
	int size;
	uint8_t *msrc = NULL;
	uint8_t *mdst = NULL;
	uint16_t msport = 0;
	uint16_t mdport = 0;
	uint32_t mask = 0xFFFFFFFF;
	int i;
	uint8_t *s;
	uint8_t *t;
	uint8_t flag = 0;

	snoop_action = kmalloc(sizeof(PAE_SNOOP_ACTION), GFP_KERNEL);
	memset ((void*)snoop_action, 0, sizeof(PAE_SNOOP_ACTION));
	if (!snoop_action) {
		status = -ENOMEM;
		goto done;
	}
	msrc = kmalloc(SIZE_IPV6_ADDR, GFP_KERNEL);
	if (!msrc) {
		status = -ENOMEM;
		goto done;
	}
	mdst = kmalloc(SIZE_IPV6_ADDR, GFP_KERNEL);
	if (!mdst) {
		status = -ENOMEM;
		goto done;
	}
	if (version == 4) {
		size = SIZE_IPV4_ADDR;
		mask = ipv4_mask.saddr;
		memcpy(msrc, src, SIZE_IPV4_ADDR);
		s = msrc;
		t = (uint8_t*)&ipv4_mask.saddr;
		for (i = 0; i < SIZE_IPV4_ADDR; i++, s++, t++) {
			*s = *s & *t;
		}
		memcpy(mdst, dst, SIZE_IPV4_ADDR);
		s = mdst;
		t = (uint8_t*)&ipv4_mask.daddr;
		for (i = 0; i < SIZE_IPV4_ADDR; i++, s++, t++) {
			*s = *s & *t;
		}
		msport = sport & ipv4_mask.sport;
		mdport = dport & ipv4_mask.dport;
	}
	if (version == 6) {
		size = SIZE_IPV6_ADDR;
		memcpy(msrc, src, SIZE_IPV6_ADDR);
		s = msrc;
		t = (uint8_t*)&ipv6_mask.saddr;
		for (i = 0; i < SIZE_IPV6_ADDR; i++, s++, t++) {
			*s = *s & *t;
		}
		memcpy(mdst, dst, SIZE_IPV6_ADDR);
		s = mdst;
		t = (uint8_t*)&ipv6_mask.daddr;
		for (i = 0; i < SIZE_IPV6_ADDR; i++, s++, t++) {
			*s = *s & *t;
		}
		msport = sport & ipv6_mask.sport;
		mdport = dport & ipv6_mask.dport;
	}
	if (inbound) {
		/* for inbound SA search the src addr */
		flag = 1;
	}
	node = PaeIpsecGetMatchingSA(inbound, version);
	if (node) {
		//printk("Got a matching SA spi %08x action id %d\n", node->sa_action.said.spi, node->action_id);
		//DataDump("action data l2 data", node->sa_action.l2_data, 20);
		// Get the SA of the other direction
		twin_node = PaeFindTwinNode(node);
		if (twin_node) {
			//printk("twin node SA spi %08x action id %d\n", twin_node->sa_action.said.spi, twin_node->action_id);
			//DataDump("action data l2 data", node->sa_action.l2_data, 20);
		} else { 
			//printk("No matching SA for snoop action\n");
			goto done;
		}
		if (node->snoop_id) {
			// This has already been sent to quite
			printk("A snoop action is already in progress\n");
			goto done;
		}
		node->snoop_id = action_idx;
		node->snoop_action = snoop_action;

		snoop_action->reverseActionId = twin_node->action_id;
		snoop_action->actionId = node->action_id;
		snoop_action->vlanId = node->action_id;
		snoop_action->snoopActionId = action_idx;
		memcpy(snoop_action->srcMasked, msrc, size);
		memcpy(snoop_action->dstMasked, mdst, size);
		memcpy(snoop_action->subnetMask, &mask, size);
		snoop_action->sportMask = msport;
		snoop_action->dportMask = mdport;
		snoop_action->proto = proto;
		//printk(KERN_LEVEL "Create snoop action %d inbound %d src %08x dst %08x masked %d mask %x\n", 
		//		action_idx, inbound, *(uint32_t*)src, *(uint32_t*)dst, masked, ipv4_mask.saddr); 
		if (masked) {
			snoop_action->proto = proto | (0x1 << 8);
			//printk("snoop action: masked %d proto %x ports %04x:%04x\n", masked, proto, msport, mdport);
		}
		memcpy(snoop_action->ipAddr0, src, size);
		memcpy(snoop_action->ipAddr1, dst, size);
		memcpy(snoop_action->peerAddr, node->peerAddr, size);
		// Fill in the action type and size before the action data
		// Add the snoop action
		//printk(KERN_LEVEL "Added snoop action %d length %d\n", action_idx, sizeof(PAE_SNOOP_ACTION));
		status = pae_add_action_with_mtu(action_idx, PAE_ACTION_TYPE_SNOOP, 
					1500, (uint8_t*)snoop_action, sizeof(PAE_SNOOP_ACTION));
		if (status == PAE_ERR_INVALID_ACTION_IDX) {
			action_idx++;
			snoop_action->snoopActionId = action_idx;
			status = pae_add_action_with_mtu(action_idx, PAE_ACTION_TYPE_SNOOP, 
						1500, (uint8_t*)snoop_action, sizeof(PAE_SNOOP_ACTION));
			if (status == PAE_ERR_INVALID_ACTION_IDX) {
				printk("Index not accepted\n");
				goto done;
			}
		}
		action_idx++;
	} else {
		printk("No matching SA for snoop action\n");
		goto done;
	}

done:
	if (msrc) {
		kfree(msrc);
	}
	if (mdst) {
		kfree(mdst);
	}
	return status;
}

static void ShowIPv4Mask(void)
{
	printk("IPv4 Mask\n");
	printk("src addr %08x\n", ipv4_mask.saddr);
	printk("dst addr %08x\n", ipv4_mask.daddr);
	printk("src port %04x\n", ipv4_mask.sport);
	printk("dst port %04x\n", ipv4_mask.dport);
	printk("proto %02x\n", ipv4_mask.proto);
}

static void ShowIPv6Mask(void)
{
	int i;
	printk("IPv6 Mask\n");
	printk("src addr");
	for (i = 0; i < 4; i++) {
		printk(" %08x\n", ipv6_mask.saddr[i]);
	}
	printk("\n");
	printk("dst addr");
	for (i = 0; i < 4; i++) {
		printk(" %08x\n", ipv6_mask.daddr[i]);
	}
	printk("\n");
	printk("src port %04x\n", ipv6_mask.sport);
	printk("dst port %04x\n", ipv6_mask.dport);
	printk("proto %02x\n", ipv6_mask.proto);
}

int PaeIpsecSetHashMask(uint8_t version, uint8_t *src, 
		uint8_t *dst, uint16_t sport, uint16_t dport, uint8_t proto)
{
    int status = PAEIF_STATUS_OK;

	if (version == 4) {
		if (ipv4_mask.ref_count) {
			// Compare the new mask to the existing one
			if (ipv4_mask.saddr == htonl(*(uint32_t*)src)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv4_done;
			}
			if (ipv4_mask.daddr == htonl(*(uint32_t*)dst)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv4_done;
			}
			if (sport != htons(ipv4_mask.sport)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv4_done;
			}
			if (dport != htons(ipv4_mask.dport)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv4_done;
			}
			if (proto != htonl(ipv4_mask.proto)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv4_done;
			}
		} else {
			ipv4_mask.saddr = htonl(*(uint32_t*)src);
			ipv4_mask.daddr = htonl(*(uint32_t*)dst);
			ipv4_mask.sport = htons(sport);
			ipv4_mask.dport = htons(dport);
			ipv4_mask.proto = proto;
			status = pae_set_tuple_mask(version, (uint8_t*)&ipv4_mask.saddr, (uint8_t*)&ipv4_mask.daddr, 
					ipv4_mask.sport, ipv4_mask.dport, ipv4_mask.proto); 
			return status;
		}
ipv4_done:
		if (status) {
			ShowIPv4Mask();
		}
		return status;
	}
	if (version == 6) {
		if (ipv6_mask.ref_count) {
			// Compare the new mask to the existing one
			if (memcmp(src, ipv6_mask.saddr, SIZE_IPV6_ADDR)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv6_done;
			}
			if (memcmp(dst, ipv6_mask.daddr, SIZE_IPV6_ADDR)) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv6_done;
			}
			if (sport != ipv6_mask.sport) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv6_done;
			}
			if (dport != ipv6_mask.dport) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv6_done;
			}
			if (proto != ipv6_mask.proto) {
				status = PAEIF_HOST_ERROR_HASH_MASK;
				goto ipv6_done;
			}
			memcpy(&ipv6_mask.saddr[0], src, SIZE_IPV6_ADDR);
			memcpy(&ipv6_mask.daddr[0], dst, SIZE_IPV6_ADDR);
			ipv6_mask.sport = sport;
			ipv6_mask.dport = dport;
			ipv6_mask.proto = proto;
			status = pae_set_tuple_mask(version, (uint8_t*)ipv6_mask.saddr, (uint8_t*)ipv6_mask.daddr, 
					ipv6_mask.sport, ipv6_mask.dport, ipv6_mask.proto); 
			return status;
		}
ipv6_done:
		if (status) {
			ShowIPv6Mask();
		}
		return status;
	}
	return status;
}

EXPORT_SYMBOL(PaeIpsecAddSA);
EXPORT_SYMBOL(PaeIpsecAddPayload);
EXPORT_SYMBOL(PaeIpsecDeleteSA); 
EXPORT_SYMBOL(PaeIpsecGetSAIVLen);
