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
*  @file    pae_ipsec.h
*
*  @brief   PAE IPsec header file 
*
*  @note
*
*****************************************************************************/

#ifndef __PAE_IPSEC_H__
#define __PAE_IPSEC_H__

/* PAE status host errors */
#define PAEIF_STATUS_OK                  0
#define PAEIF_HOST_ERROR_INPUT           1001
#define PAEIF_HOST_ERROR_AUTH_ALG        1002
#define PAEIF_HOST_ERROR_ENCR_ALG        1003
#define PAEIF_HOST_ERROR_KEY_LENGTH      1004
#define PAEIF_HOST_ERROR_ALLOC           1005
#define PAEIF_HOST_ERROR_SA_NOT_FOUND    1006
#define PAEIF_HOST_ERROR_SPD_PAYLOAD     1007
#define PAEIF_HOST_ERROR_TUNNEL_PAYLOAD  1008
#define PAEIF_HOST_ERROR_NAT_PAYLOAD     1009
#define PAEIF_HOST_ERROR_L2TP_PAYLOAD    1010
#define PAEIF_HOST_ERROR_SA_FLAGS        1011
#define PAEIF_HOST_ERROR_MESSAGE_TYPE    1012
#define PAEIF_HOST_ERROR_MODE_NOT_SUPPORTED 1013
#define PAEIF_HOST_ERROR_SA_INBOUND_LEN  1014
#define PAEIF_HOST_ERROR_SA_OUTBOUND_LEN 1015
#define PAEIF_HOST_ERROR_IP_VER          1016
#define PAEIF_HOST_ERROR_HASH_MASK		 1017
#define PAEIF_HOST_ERROR_LAN_NOT_FOUND	 1018
#define PAEIF_HOST_ERROR_WAN_NOT_FOUND	 1019
#define PAEIF_HOST_ERROR_LOCAL_IF_MATCH	 1020


#define PAE_STATUS_OK                    0
#define PAE_ERROR_IP_PKT                 401
#define PAE_ERROR_SA_NOT_FOUND           402
#define PAE_ERROR_TLV_TYPE               403
#define PAE_ERROR_MESSAGE_TYPE           404
#define PAE_ERROR_CRYPTO_TYPE            405
#define PAE_ERROR_CRYPTO_OPERATION       406
#define PAE_ERROR_CRYPTO_KEYLEN          407
#define PAE_ERROR_MAX_CACHED_SA          408
#define PAE_ERROR_SA_ID                  409
#define PAE_ERROR_SA_IP_VERSION          410

#define NUM_IPSEC_ACTION_ERRORS			5
#define IPSEC_ACTION_ERR_SPI_MISMATCH   0
#define IPSEC_ACTION_ERR_IP_HDR			1
#define IPSEC_ACTION_ERR_NEXT_PROTO		2
#define IPSEC_ACTION_ERR_PKT_IN_QUEUE   3
#define IPSEC_ACTION_ERR_OUTBOUND_RX    4

/* PAE SA status values */
#define PAESA_STATUS_ACTIVE              1
#define PAESA_STATUS_NOT_FOUND           2 
#define PAESA_STATUS_REJECTED            3 
#define PAESA_STATUS_DELETED             4 

#define SIZE_IPV4_HEADER          20
#define SIZE_IPV6_HEADER          40

#define IP_VERSION_4               4
#define IP_VERSION_6               6

#define SIZE_IPV4_ADDR             4
#define SIZE_IPV6_ADDR            16
#define SIZE_MAC_ADDR              6 

#define IP_PROTOCOL_ESP           50
#define IP_PROTOCOL_AH            51

#ifdef __KERNEL__
#define PAE_ALLOC(m)                 kmalloc(m,GFP_KERNEL)
#define PAE_FREE(m)                  kfree(m)
#endif

/* Bits of the BCMPKT_CTX flags */
typedef struct PaeSAFlags_s {
    uint32_t esp:1;				/* IPsec Protocol: ESP 50 */
    uint32_t ah:1;				/* IPsec Protocol: AH 51 */
    uint32_t ipv4:1;            /* SA is for IPv4 */
    uint32_t ipv6:1;            /* SA is for IPv6 */
    uint32_t tunnel:1;          /* SA is for tunnel mode */
    uint32_t inbound:1;         /* SA is inbound */
    uint32_t auth:1;            /* authentication required */
    uint32_t encr:1;            /* encryption required */
    uint32_t esn:1;             /* SA requires extended sequence number */
    uint32_t nat:1;             /* SA specifies NAT traversal */
    uint32_t subnet:1;          /* SA is for a subnet, possible multiple hosts on LAN */
    uint32_t ivLen:5;           /* Length of the crypto IV */
    uint32_t icvTruncLen:5;     /* Length of the truncated ICV data */
    uint32_t multiHostLan:1;    /* LAN has multiple hosts */
    uint32_t reserved:2;        /* reserved */
	uint32_t newDstMacAddr:8;   /* the dst MAC addr for this SA has been dynamically updated */
} PAEIF_SAFLAGS;

#define  SA_UPDATE_L2_DATA			0x01

#define VLAN_TAG_LEN				4
#define ETHERTYPE_SIZE				2
#define L2_ADDR_LEN					6
#define BRCM_HEADER_LEN             4
#define L2_HEADER_LEN			   (2*L2_ADDR_LEN+ETHERTYPE_SIZE+VLAN_TAG_LEN)
#define ALIGNED_L2_LEN			   (2*L2_ADDR_LEN+VLAN_TAG_LEN)     /* L2 header minus eth_type */
#define L2_DATA_LEN				   (ALIGNED_L2_LEN+BRCM_HEADER_LEN)

#define MAX_ENC_KEY_SIZE           (256)  /* Max Encryption key size supported */
#define MAX_AUTH_KEY_SIZE          (256)  /* Max Authentication key supported */
#define MAX_ENC_KEY_SIZE_BYTES     (MAX_ENC_KEY_SIZE/8)   /* Max Encryption key size supported */
#define MAX_AUTH_KEY_SIZE_BYTES    (MAX_AUTH_KEY_SIZE/8)  /* Max Authentication key supported */
#define REQUIRED_IPSEC_ACTION_SIZE (256) 

/* SPU supported encryption algorithms for IPsec */
#define PAEIF_ENCR_ALG_NULL        0x0
#define PAEIF_ENCR_ALG_DES         0x2
#define PAEIF_ENCR_ALG_3DES        0x3
#define PAEIF_ENCR_ALG_AES         0x4

/* SPU supported encryption modes for IPsec */
#define PAEIF_ENCR_MODE_CBC        0x0
#define PAEIF_ENCR_MODE_CTR        0x1
#define PAEIF_ENCR_MODE_CCM        0x2
#define PAEIF_ENCR_MODE_GCM        0x3

/* SPU supported authentication algorithms for IPsec */
#define PAEIF_AUTH_ALG_NONE        0x0
#define PAEIF_AUTH_ALG_NULL        0x0
#define PAEIF_AUTH_ALG_MD5         0x1
#define PAEIF_AUTH_ALG_SHA1        0x2
#define PAEIF_AUTH_ALG_SHA224      0x3
#define PAEIF_AUTH_ALG_SHA256      0x4
#define PAEIF_AUTH_ALG_AES         0x5

/* SPU supported authentication modes for IPsec */
#define PAEIF_AUTH_MODE_HASH       0x0
#define PAEIF_AUTH_MODE_XCBC       0x0
#define PAEIF_AUTH_MODE_GMAC       0x1
#define PAEIF_AUTH_MODE_CTXT       0x1
#define PAEIF_AUTH_MODE_HMAC       0x2
#define PAEIF_AUTH_MODE_CCM        0x5
#define PAEIF_AUTH_MODE_GCM        0x6

#define PAEIF_DE_SIZE              (26*4)   /* 26 registers */ 

/* SA ID structure: the triplet that uniquely identifies the SA */
typedef struct paeif_sa_id_t {
    uint32_t spi;
    union {
        uint8_t v4dstAddr[SIZE_IPV4_ADDR];
        uint8_t v6dstAddr[SIZE_IPV6_ADDR];
    };
} PAEIF_SAID;

#define SCTX_WORDS                   (5)
#define MAX_AUTH_KEY_WORDS			 (256/32)     // Not using FHMAC
#define MAX_CIPHER_KEY_WORDS		 (256/32)
#define MAX_SCTX_LEN                 ((SCTX_WORDS + MAX_AUTH_KEY_WORDS + MAX_CIPHER_KEY_WORDS)*4)

#define MIN_SA_ACTION_LEN            (13*4)
#define MAX_DMA_CTX_LEN              300
#define DMA_CTX_ACTION_OFFSET        56   // offset of the start of DMA ctx in the action structure 
#define MAX_IPSEC_DMA_CTX_LEN        (MAX_DMA_CTX_LEN - DMA_CTX_ACTION_OFFSET)  // Maintain action size to less 300 bytes
#define MAX_DMA_CTX_LEN_OUTBOUND     148
#define MAX_DMA_CTX_LEN_INBOUND      84
#define MAX_INBOUND_SA_ACTION_LEN    (MAX_DMA_CTX_LEN_INBOUND + DMA_CTX_ACTION_OFFSET)
#define MAX_OUTBOUND_SA_ACTION_LEN   (MAX_DMA_CTX_LEN_OUTBOUND + DMA_CTX_ACTION_OFFSET)

#define MAX_IPSEC_CONNECTIONS        (4)         // Full duplex connection
#define MAX_IPSEC_CONNECTION_LEN  (MAX_INBOUND_SA_ACTION_LEN + MAX_OUTBOUND_SA_ACTION_LEN) 
#define IPSEC_SCRATCH_AREA_SIZE      (MAX_IPSEC_CONNECTIONS*MAX_IPSEC_CONNECTION_LEN)


/* structure of the PAE IPsec SA action */
typedef struct pae_ipsec_action_t {
	uint8_t action_type;
	uint8_t action_len;
	uint16_t action_mtu;
    PAEIF_SAFLAGS sa_flags;
	PAEIF_SAID said;
	void *twin_sa;					 /* SPI/addr of the twin SA */
    uint32_t spuh_start;             /* Address of the start of the DMA context */
    uint8_t dma_ctx_len;             /* Size of the DMA context including outer header, NAT payload */
    uint8_t bdesc_offset;            /* Size of SPU header: MH, EMH and SCTX, BDESC and BD not included */
	uint16_t mtu;
	uint8_t l2_data[L2_DATA_LEN];    /* L2 data to be prepended to the packets out of deq1 */
	uint8_t dma_ctx[MAX_IPSEC_DMA_CTX_LEN];
} PAE_IPSEC_ACTION;

typedef struct snoop_flags_s {
	uint8_t ipv4:1;
	uint8_t unused:2;
	uint8_t vlan_wan:1;
	uint8_t peerSnooped:1;
	uint8_t lanSnooped:1;
	uint8_t done:1;
	uint8_t updated:1;                       // No longer dummy mode
} PAE_SNOOP_FLAGS;

typedef struct snoop_action_ipv4_s {
	uint16_t ip_ver;
	uint16_t actionId;
	uint16_t reverseActionId;
	uint16_t snoopActionId;
	uint16_t vlanId;
	PAE_SNOOP_FLAGS flags;
	uint8_t switch_mode;
    uint8_t ipAddr0[SIZE_IPV4_ADDR];
    uint8_t ipAddr1[SIZE_IPV4_ADDR];
    uint8_t peerAddr[SIZE_IPV4_ADDR];
    uint8_t srcMasked[SIZE_IPV4_ADDR];
    uint8_t dstMasked[SIZE_IPV4_ADDR];
    uint8_t subnetMask[SIZE_IPV4_ADDR];
	uint16_t sportMask;
	uint16_t dportMask;
	uint32_t proto;
} PAE_SNOOP_ACTION;

typedef struct snoop_action_ipv6_s {
	uint16_t ip_ver;
	uint16_t actionId;
	uint16_t reverseActionId;
	uint16_t snoopActionId;
	uint16_t vlanId;
	PAE_SNOOP_FLAGS flags;
	uint8_t switch_mode;
    uint8_t ipAddr0[SIZE_IPV6_ADDR];
    uint8_t ipAddr1[SIZE_IPV6_ADDR];
    uint8_t peerAddr[SIZE_IPV6_ADDR];
    uint8_t srcMasked[SIZE_IPV6_ADDR];
    uint8_t dstMasked[SIZE_IPV6_ADDR];
    uint8_t subnetMask[SIZE_IPV6_ADDR];
	uint16_t sportMask;
	uint16_t dportMask;
	uint32_t proto;
} PAE_SNOOP_V6_ACTION;

struct lan_entry_t {
	uint8_t ipAddr[SIZE_IPV4_ADDR];
	uint8_t macAddr[SIZE_MAC_ADDR];
	uint16_t vlan_id;
};

struct lan_entry_ipv6_t {
	uint8_t ipAddr[SIZE_IPV6_ADDR];
	uint8_t macAddr[SIZE_MAC_ADDR];
	uint16_t vlan_id;
};

#define MAX_LAN_ENTRY_COUNT     4

/* Crypto Operation Data Structure */
typedef struct paeif_crypto_op_t {
    uint8_t encrAlg;        /* Encryption algorithm PAEIF_ENCR_ALG_XX */
    uint8_t authAlg;        /* Authentication Algorithm PAEIF_AUTH_ALG_XX */
    uint8_t encrMode;       /* Encryption mode PAEIF_ENCR_MODE_XX */
    uint8_t authMode;       /* Authentication hash mode PAEIF_AUTH_MODE_XX */
    uint16_t encrKeyBits;   /* Encryption algorithm key length in bits */
    uint16_t authKeyBits;   /* Authentication algorithm key length in bits */
    uint16_t icvTruncBits;  /* Length of the truncated ICV in bits */
    uint16_t reserved;        
    uint8_t encrKey[MAX_ENC_KEY_SIZE_BYTES];    /* Encryption key */
    uint8_t authKey[MAX_AUTH_KEY_SIZE_BYTES];   /* Authentication key */
} PAEIF_CRYPTOOP;

typedef struct pae_spd_flags_t {
    uint32_t ipv4:1;             /* policy is for IPV4 addresses */
    uint32_t ipv6:1;             /* policy is for IPV6 addresses */
    uint32_t saddr_en:1;         /* enforce src IP address */ 
    uint32_t saddr_en_mask:1;    /* enforce masked src IP address */
    uint32_t saddr_mask_bits:4;  /* mask to apply to IP address */
    uint32_t daddr_en:1;         /* enforce dest IP address */ 
    uint32_t daddr_en_mask:1;    /* enforce masked dest IP address */
    uint32_t daddr_mask_bits:4;  /* mask to apply to IP address */
    uint32_t dport_en:1;         /* enforce dest port */
    uint32_t sport_en:1;         /* enforce src port */
    uint32_t protocol:8;         /* enforce protocol if non-zero */
    uint32_t unused:8;
} SPDFLAGS;

typedef struct pae_spdv4_s {
    SPDFLAGS flags;
    uint8_t srcAddr[SIZE_IPV4_ADDR];
    uint8_t dstAddr[SIZE_IPV4_ADDR];
    uint16_t srcPort;
    uint16_t dstPort;
} PAEIF_SPDV4;

typedef struct pae_spdv6_s {
    SPDFLAGS flags;
    uint8_t srcAddr[SIZE_IPV6_ADDR];
    uint8_t dstAddr[SIZE_IPV6_ADDR];
    uint16_t srcPort;
    uint16_t dstPort;
} PAEIF_SPDV6;

typedef union {
    PAEIF_SPDV4 v4;
    PAEIF_SPDV6 v6;
} PAEIF_SPD;

typedef struct pae_tunnel_flags_t {
    uint32_t ipv4:1;             /* tunnel header is IPV4 */
    uint32_t ipv6:1;             /* tunnel header is IPV6 */
    uint32_t unused:30;
} TUNNELFLAGS;

typedef union pae_tunnel_s {
    TUNNELFLAGS flags;
    union {
        uint8_t v4[SIZE_IPV4_HEADER];
        uint8_t v6[SIZE_IPV6_HEADER];
    };
} PAEIF_TUNNEL;

typedef struct {
    uint16_t udp_src;
    uint16_t udp_dst;
    uint16_t udp_len;
    uint16_t udp_chksum;
} UDP_HEADER;

typedef struct {
    uint64_t packet_count;
    uint64_t byte_count;
} PAEIF_SAU;

typedef struct deDesc {
    uint *r_desc_mcw;
    uint *r_desc_sad;
    uint *r_desc_sau;
    uint r_bdata_encap_length;    // Encap data length
    uint *r_desc_bdata_encap;     // Encap descriptor 
    volatile uint r_bdata_length;         // Bdata length
    volatile uint *r_desc_bdata;          // Bdata descriptor
    uint *r_desc_post;            // POST desc ptr
} DEDESC, *DEDESCPTR;
  
// Bdesc Descriptor, arguments and results
typedef struct bdCalc {
    uint r_desc_type;
    uint r_bda_tlength;
    uint r_ipsec_offset;
    uint r_64b_auth;
} BDCALC, *BDCALCPTR;

#define SAD_SIZE_WORDS           (3) 
#define SAU_SIZE_WORDS           (3) 
#define DE_SIZE_WORDS            (8) 

typedef struct ipsec_runtime_data_t {
	uint r_mcw;                   // Swaped copy of MCW register
	uint r_sad[SAD_SIZE_WORDS];   // Swaped copies of SAD registers
	uint de_data[DE_SIZE_WORDS];  // Runtime DE structure
	uint r_sau[SAU_SIZE_WORDS];   // SAU data
} IPSEC_SA_DATA;

/*-------------------- Centurion view of the SPU -------------------------------------*/

#define MAX_UINT32_SEQ_NUM	(0xffffffff)

// MCW Masks
#define SCTX_PR_SET		0x80	// 31 Input: when set, the SCTX structure is present.
#define BCT_PR_SET		0x40	// 30 Input: when set, the BlockCipherTrailer field is present.
#define BDESC_PR_SET	0x20	// 29 Input: when set the BufferDescriptor field is present.
#define MFM_PR_SET		0x10	// 28 Input: when set, the Mutable field mask is present.
#define BD_PR_SET		0x08	// 27 When set, the BufferData field is present.
#define HASH_PR_SET		0x04	// 26 Output: when set, the HASH field is present.
#define SPS_PR_SET		0x02	// 25 Output: when set, the Security Policy Selector field is present.
#define SUPDT_PR_SET	0x01	// 24 Ouput: when set the SCTX Update field is present.
#define SCTX_PR_CLR		0x7f	// 31 Input: when set, the SCTX structure is present.
#define BCT_PR_CLR		0xbf	// 30 Input: when set, the BlockCipherTrailer field is present.
#define BDESC_PR_CLR	0xdf	// 29 Input: when set the BufferDescriptor field is present.
#define MFM_PR_CLR		0xef	// 28 Input: when set, the Mutable field mask is present.
#define BD_PR_CLR		0xf7	// 27 When set, the BufferData field is present.
#define HASH_PR_CLR		0xfb	// 26 Output: when set, the HASH field is present.
#define SPS_PR_CLR		0xfd	// 25 Output: when set, the Security Policy Selector field is present.
#define SUPDT_PR_CLR	0xfe	// 24 Ouput: when set the SCTX Update field is present.

// SAD Flags
#define TYPE		0xc0000000	// [0] [31:30] The type of the SCTX structure
					// 2’b00 - Generic
					// 2’b01 - IPSec
					// 2’b10 - SSL/TLS
					// Must be set to 2’b01 for IPSec processing
#define Cacheable	0x20000000	// [0] [29] ‘1’ indicates the SCTX structure can be cached
					// by the BCM5862 onchip memory.
#define Update_En	0x10000000	// [0] [28] ‘1’ indicates that BCM5862 can update the content
					// of the SCTX structure. If the ‘Cacheable’ flag is not set,
					// the updated image must be returned to the host.
#define Lock_En		0x08000000	// [0] [27] The cached state is locked in the cache. This bit is
					// ignored when ‘Cacheable’ bit is cleared.
#define Cap_En		0x04000000	// [0] [26] ‘1’ indicates BCM5862 can perform encap/decap on data.
#define Reserved0	0x03000000	// [0] [25:24] Reserved
#define Transport	0x00800000	// [0] [23] ‘1’ indicates the SA is a transport mode SA.
					// ‘0’ indicates the SA is a tunnel mode SA.
#define AH_BIT		0x00400000	// [0] [22] AH PROTOCOL: Indicates that this packet must contain
					// an AH protocol header that is parsed by the BCM5862.a
#define ESP_BIT	0x00200000	// [0] [21] ESP PROTOCOL: Indicates that this packet contains an
					// ESP protocol header that is parsed by the BCM5862.
#define AuthSeq64	0x00100000	// [0] [20] ‘1’ indicates that the 64-bit sequence number is used
					// in authentication.
#define	UDP_En		0x00080000	// [0] [19] UDP DECAPSULATION: Indicates that UDP encapsulation
					// is used for this packet. The UDP length field will be
					// calculated by the BCM5862 for outbound packets. The UDP
					// encapsulation header is stored as part of the encapsulation
					// headers in the SCTX structure. For decapsulation, the BCM5862
					// will remove the UDP security header from the packet.
#define UDP_Port_Sel	0x00040000	// [0] [18] UDP port number used for outbound UDP encapsulation.
					// The source port and destination port are set to the same value.
					// This bit is valid when ‘UDP_En’ is set. The port numbers are
					// programmed into OPU configuration registers.
					// ‘0’ - use port number 0 (default register value to 500)
					// ‘1’ - use port number 1 (default register value to 4500)
#define Reserved1	0x00030000	// [0] [17:16] Reserved
#define Pad_En		0x00008000	// [0] [15] Bit results in no pad on outbound traffic. The ESP
					// padding can’t be completed without ‘NxtHdr’ field, which is
					// generated when ‘Cap_En’ is set.
					//‘1’ indicates Pad checking is enabled for inbound packet.
					// The ESP padding is removed if Cap_En is set.
#define Gen_ID		0x00004000	// [0] [14] Generate the ID field for outbound tunnel IPv4 header
					// BCM5862 Advance Data Sheet 12/29/04 Broadcom Corporation
					// Page 76 Cryptographic Operations in MCR1@ Channel Document
					// 5862-DS100-D4
#define TwoDestOpts	0x00002000	// [0] [13] ‘1’ indicates the IPv6 header contains two Destination
					// Options headers. The security headers (AH/ESP) should be
					// inserted before the second IPv6 Destination Options Header.
					// Otherwise, the security headers should be inserted before the
					// upper layer headers. This bit is not applicable to IPv4 packet.
#define	IPv4ChkSum	0x00001000	// [0] [12] ‘1’ indicates that IPv4 header checksum is recomputed
					// and updated for outbound packet and verified for inbound packet.
#define	Decrement_TTL	0x00000800	// [0] [11] When set, it indicates that the TTL or Hop Limit field
					// in the IP header must be decremented by the BCM5862. In
					// transport mode, this option is applied to the IP header. In
					// tunnel mode, this option is applied to the inner IP header.
					// This bit applies to both inbound and outbound case. The IPv4
					// checksum for the IPv4 header is re-calculated by the BCM5862
					// whenever it changes the IPv4 header TTL value. If the TTL is
					// zero, the BCM5862 will tag the packet as an error.
#define	Copy_TOS	0x00000400	// [0] [10] TOS COPY: Enables the TOS or CLASS field to be copied
					// from the inner header to the outer header. This bit is only
					// valid for tunnel mode packets.
#define	Copy_Flow	0x00000200	// [0] [9] FLOW ID COPY: Enables the copy of the Flow ID from the
					// inner header to the outer header when set. This bit is only
					// valid for tunnel mode packets. When enabled and the inner header
					// is IPv4, the FlowID is set to zero.
#define	DF_Copy		0x00000100	// [0] [8] DON’T FRAGMENT BIT: Enables the copy of the DF bit from
					// the inner IPv4 header to the outer header for Tunnel mode
					// packets.
					// 0 = Disabled (do not change DF bit)
					// 1 = Copy DF bit from inner header
					// This bit is only used when the outer and inner IP header is IPv4.
					// When DF_Copy is enabled and the inner header is IPv6, the DF bit
					// will be set in the outer header.
#define	SCTX_Size	0x000000ff	// [0] [7:0] The size of the entire SCTX structure in 32-bit words.
#define	Inbound		0x80000000	// [1] [31] ‘1’ indicates the SA is for inbound packet processing.
#define	Order		0x40000000	// [1] [30] The order the authentication and encryption should be
					// applied. For IPSec, this bit is always set to zero, meaning for
					// outbound packet, the packet is encrypted first then the
					// encrypted packet authenticated. For inbound packet, the packet
					// is authentication then decrypted.
#define Reserved2	0x3f000000	// [1] [29:24] Reserved
#define	CryptoAlgo	0x00e00000	// [1] [23:21] Encryption/decryption algorithms
#define	CryptoMode	0x001c0000	// [1] [20:18] Cipher mode of operations
#define	CryptoType	0x00030000	// [1] [17:16] Encryption/decryption processing type
#define	HashAlgo	0x0000e000	// [1] [15:13] Hash algorithms
#define	HashMode	0x00001c00	// [1] [12:10] Hash mode of operations
#define	HashType	0x00000300	// [1] [9:8] Hash processing type
#define	UPDT_Ofst	0x000000ff	// [1] [7:0] The word offset of the updateable SCTX fields starting
					// from the beginning of SCTX structure. In IPSec SCTX, this offset
					// points to SAU Flags.

// SPD Flags
#define	IPv6_En		0x80000000	// [0] [31] The Policy data is for IPv6 packet.
#define	DADR_En		0x60000000	// [0] [30:29] DESTINATION ADDRESS TYPE: Denotes the type of
					// comparison that is required for the DestAddress/DestMask fields.
					// 00 - Wildcard (ignored)
					// 01 - Fixed (exact match with DestAddress)
					// 10 - Range (DestAddress <=source addr <= DestMask)
					// 11 - Mask (DestAddress & DestMask = source addr & DestMask)
#define	SADR_En		0x18000000	// [0] [28:27] SOURCE ADDRESS TYPE: Denotes the type of comparison
					// that is required for the SrcAddress/SrcMask fields.
					// 00 - Wildcard (ignored)
					// 01 - Fixed (exact match with SrcAddress)
					// 10 - Range (SrcAddress <=source addr <= SrcMask)
					// 11 - Mask (SrcAddress & SrcMask = source addr & SrcMask)
#define	DPort_ICMP_En	0x04000000	// [0] [26] DESTINATION PORT ENABLE: Enables checking the DestPort
					// field against the data in the packet as part of the policy
					// verification if the upper layer is UDP or TCP. If the upper
					// layer is ICMP or ICMPv6, this flag enables ICMP Code checking.
#define	SPort_ICMP_En	0x02000000	// [0] [25] SOURCE PORT ENABLE: Enables checking the SrcPort field
					// against the data in the packet as part of the policy
					// verification if the upper layer is UDP or TCP. If the upper
					// layer is ICMP or ICMPv6, this flag enables ICMP Type checking.
#define	Prot_En		0x01000000	// [0] [24] PROTOCOL ENABLE: Enables checking of the Protocol
					// field against the data in the packet as part of the policy
					// verification.

enum CRYPTO_ALGO_TYPE { \
		CryptoAlgoNull = 0, \
		CryptoAlgoArcFour = 1, \
		CryptoAlgoDes = 2, \
		CryptoAlgo3Des = 3, \
		CryptoAlgoAes = 4 \
		};
#define CRYPTO_ALGO_SHIFT	21

enum CRYPTO_MODE_TYPE { \
		CryptoModeECB = 0, \
		CryptoModeCBC = 1, \
		CryptoModeOFB = 2, \
		CryptoModeCFB = 3, \
		CryptoModeCTR = 4, \
		CryptoModeCCM = 5, \
		CryptoModeGCM = 6 \
		};
#define CRYPTO_MODE_SHIFT	18

enum AUTH_ALGO_TYPE { \
		HashAlgoNull = 0, \
		HashAlgoMD5 = 1, \
		HashAlgoSHA1 = 2, \
		HashAlgoSHA224 = 3, \
		HashAlgoSHA256 = 4 \
		};
#define HASH_ALGO_SHIFT		13

enum AUTH_MODE_TYPE { \
		HashModeHASH = 0, \
		HashModeCTXT = 1, \
		HashModeHMAC = 2, \
//		HashModeSSLMAC = 3,  Not supported in SPU-M 
		HashModeCCM = 4,  \
		HashModeXCBC = 5, \
		HashModeFHMAC = 6 \
		};
#define HASH_MODE_SHIFT		10

enum ESP_HDR_OFFSETS { \
		ESP_SPI_OFFSET = 0, \
		ESP_SEQ_OFFSET = 1, \
		ESP_AES_CTR_IV0_OFFSET = 2, \
		ESP_AES_CTR_IV1_OFFSET = 3 \
		};

#define	MFM_FREE_OFFSET	4		// Offset within the FREE space for storing MFM
#define	MFM_GENERAL_ERR	0x80000000	// MFM general error flag

					// PROD_CLASS[1:0] - this bond-out option configures the the network
					// protocol suite supported by the BCM5862 family products
#define	PROD_NO_BULK	0x00000000	// - 32’b00 - reserved for product without bulk processing
#define PROD_IPSEC	0x00000001	// - 32’b01 - IPSec-only product (no direct SSL/TLS processing)

// SAU Flags
#define	Expired		0x80000000	// [0] [31] The SA timer-based or byte count lifetime has expired.
#define	Soft_Expired	0x40000000	// [0] [30] The SA timer-based or byte count lifetime has exceeded
					// the soft limit.
#define	Reserved3			// [0] [29:28] Reserved
#define	SeqNum_En	0x08000000	// [0] [27] ‘1’ indicates that BCM5862 increments the sequence
					// number for outbound packet.
//#define Timer_En	0x04000000	// [0] [26] ‘1’ indicates the timer-based SA lifetime is enforced.
//#define BC_En		0x02000000	// [0] [25] ‘1’ indicates the byte count SA life time is enforced.
#define Timer_En	0x00000000	// SA life time check in SPUM does not use this setting
#define BC_En		0x00000000  // SA life time check in SPUM does not use this setting 

#define SALifeTime	0x1		// [1] [31:0] This is the time stamp in time tick of the SA
					// expiration.
#define ByteCountMSW	0x2		// [2-3] [31:0] PROTECTED BYTE COUNT: The protected byte count
#define ByteCountLSW	0x3		// remaining. [2] = MSW, [3] = LSW
#define SeqNumMSW	0x4		// [4-5] [31:0] SEQUENCE NUMBER [4] = MSW, [5] = LSW
#define SeqNumLSW	0x5		// For outbound SA: this is the 64 bit sequence number for this packet
					// that is incremented for every packet associated with this SA.
					// For inbound SA: this is the last maximum sequence number of the
					// packet the chip receives for this SA passing ICV check.
					// The sequence number is never allowed to roll beyond 64 bits. The
					// BCM5862 treats the SA as expired when the sequence number is at
					// the maximum value.
#define TIMER		0x0

// SPD Flags
#define DPort_En	0x04000000	// [0] [26] DESTINATION PORT ENABLE: Enables checking the DestPort
					// field against the data in the packet as part of the policy
					// verification.
#define SPort_En	0x02000000	// [0] [25] SOURCE PORT ENABLE: Enables checking the SrcPort field
					// against the data in the packet as part of the policy verification.
#define Prot_En		0x01000000	// [0] [24] PROTOCOL ENABLE: Enables checking of the Protocol field
					// against the data in the packet as part of the policy verification.
#define Reserved4	0x00ff0000	// [0] [23:16] Reserved
#define Protocol	0x0000ff00	// [0] [15:8] The transport layer protocol for policy verification.
					// This field is reserved when SPD_En = 0.
#define Reserved5	0x000000ff	// [0] [7:0] Reserved
#define SrcAddress	1		// [1]/[1-4] [31:0] The source IP address or the lower bound of the
					// source address range
#define SrcMsk		2		// [2]/[5-8] [31:0] The source IP address mask (mask) or the upper
					// bound of the source address range (range)
#define DestAddress	3		// [3]/[9-12] [31:0] The destination IP address or the lower bound
					// of the destination address range
#define DestMsk		4		// [4]/[13-16] [31:0] The destination IP address mask (mask) or the
					// upper bound of the destination address range (range)
#define SrcPort		5		// [5]/[17] [31:16] The source port for policy verification
#define DestPort	5		// [5]/[17] [15:0] The destination port for policy verification

// Encapsulation Masks
#define CAP_EN_MASK	Cap_En
#define UDP_EN_MASK	UDP_En
#define COPY_DF_MASK	DF_Copy
#define SET_DF_BMASK	0x40
#define	CLEAR_DF_BMASK	0xbf
#define COPY_FLOW_MASK	Copy_Flow
#define COPY_TOS_MASK	Copy_TOS
#define DEC_TTL_MASK	Decrement_TTL
#define AUTH_64_MASK	AuthSeq64
#define PAD_EN_MASK	Pad_En
#define PORT_MASK	0x0400

// Crypto Masks
#define IN_BOUND	Inbound
#define ENCR_ALG_MASK	CryptoAlgo
#define ENCR_MODE_MASK	CryptoMode
#define AUTH_ALG_MASK	HashAlgo
#define ESP_MASK	ESP_BIT
#define AH_MASK		AH_BIT
#define CHKSUM_MASK	IPv4ChkSum

// IPv4/6 Option Extension Header Codes         // See RFC 1011, RFC 1700
#define HBH_OPT		0x0	/*  0 */
#define ICMP4_OPT       0x1     /*  1 */        // RFC 792
#define IP4_OPT		0x4	/*  4 */        // RFC 791
#define TCP_OPT		0x6	/*  6 */        // RFC 793
#define UDP_OPT		0x11	/* 17 */        // RFC 768
#define IP6_OPT		0x29	/* 41 */        // RFC 2460
#define RTE_OPT		0x2B	/* 43 */
#define FRG_OPT		0x2C	/* 44 */
#define ESP_OPT		0x32	/* 50 */        // RFC 2406
#define AH_OPT		0x33	/* 51 */        // RFC 2402
#define ICMP6_OPT       0x3A    /* 58 */        // RFC 2463
#define NUL_OPT		0x3B	/* 59 */
#define DST_OPT		0x3C	/* 60 */

// SPS Offsets, Masks and Macros
#define SPS_PTCL_SHFT	8
#define SPS_PTCL_IPV4	(IP4_OPT << SPS_PTCL_SHFT)
#define SPS_PTCL_IPV6	(IP6_OPT << SPS_PTCL_SHFT)

#define	Extract_Err	0x80000000	// [0] [31] The SPS field can not be extracted successfully
#define IPv6		0x40000000	// [0] [30] The SPD data is for IPv6 packet.
#define	SeqNum_Ext	0x20000000	// [0] [29] The sequence number has been extracted successfully.
#define Extract_Err_CLR	0x7fffffff
#define IPv6_CLR	0xbfffffff
#define SeqNum_Ext_CLR	0xdfffffff
#define	Reserved6	0x1fff0000	// [0] [28:16] Reserved
#define	Protocol	0x0000ff00	// [0] [15:8] The extracted transport layer protocol
#define	SPS_Size	0x000000ff	// [0] [7:0] The size of SPS field in 32-bit words

#define	SpsFlags	0
#define	SrcAddrIPv4_0	0		// [1]/[1-4] [31:0] The extracted source IP address
#define SrcAddrIPv6_0	0
#define SrcAddrIPv6_1	1
#define SrcAddrIPv6_2	2
#define	SrcAddrIPv6_3	3
#define	DestAddrIPv4_0	1		// [2]/[5-8] [31:0] The extracted destination IP address
#define	DestAddrIPv6_0	4
#define	DestAddrIPv6_1	5
#define	DestAddrIPv6_2	6
#define	DestAddrIPv6_3	7
#define	SrcPortIPv4	0		// [3]/[9] [31:16] The extracted source port
#define	SrcPortIPv6	0
#define	DestPortIPv4	0		// [3]/[9] [15:0] The extracted destination port
#define	DestPortIPv6	0
#define	ByteCountIPv4	1		// [4]/[10] [31:0] The computed protected byte count.
#define	ByteCountIPv6	1
#define SeqNumMswIPv4	2		// [5-6]/[11-12] [31:0] SEQUENCE NUMBER [7/[12] = LSW,
#define SeqNumMswIPv6	2		// [6]/[11] = MSW The extacted packet sequence number.
#define	SeqNumLswIPv4	3		// The MSW is adjusted for the packet based on SCTX.
#define SeqNumLswIPv6	3

#define SPS_FLAG_WD_SZE	1		// Total flag SPS words
#define	SPS_IPV4_AD_SZE	2		// Total IPv4 SPS address words
#define SPS_IPV6_AD_SZE	8		// Total IPv6 SPS address words
#define	SPS_IPV4_DA_SZE	4		// Total IPv4 SPS data words
#define SPS_IPV6_DA_SZE	4		// Total IPv6 SPS data words
#define	SPS_IPV4_WD_SZE	(SPS_FLAG_WD_SZE+SPS_IPV4_AD_SZE+SPS_IPV4_DA_SZE)
#define	SPS_IPV6_WD_SZE	(SPS_FLAG_WD_SZE+SPS_IPV6_AD_SZE+SPS_IPV6_DA_SZE)

/* Use Centurion processing on the PAE */
#undef USE_CENTURION_SPU
#define PAE_IPSEC_SNOOP
/* Logging Control */
#define CONFIG_BCM_PRINT

/************** PAE IPsec Functions Prototypes ***********/
int  pae_ipsec_pre_spu(DEDESCPTR de);
#endif /* __PAE_IPSEC_H__*/
