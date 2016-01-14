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
*  @file    pae_spu.h
*
*  @brief   PAE SPU structures and prototypes
*
*  @note
*
*****************************************************************************/

#ifndef _PAE_SPU_H
#define _PAE_SPU_H

typedef enum cipher_alg {
	CIPHER_ALG_NULL = 0x0,
	CIPHER_ALG_RC4  = 0x1,
	CIPHER_ALG_DES  = 0x2,
	CIPHER_ALG_3DES = 0x3,
	CIPHER_ALG_AES  = 0x4,
} CIPHER_ALG;

typedef enum cipher_mode {
	CIPHER_MODE_NONE = 0x0,
	CIPHER_MODE_ECB  = 0x0,
	CIPHER_MODE_CBC  = 0x1,
	CIPHER_MODE_OFB  = 0x2,
	CIPHER_MODE_CFB  = 0x3,
	CIPHER_MODE_CTR  = 0x4,
	CIPHER_MODE_CCM  = 0x5,
	CIPHER_MODE_GCM  = 0x6,
	CIPHER_MODE_XTS  = 0x7,
} CIPHER_MODE;

typedef enum cipher_type {
	CIPHER_TYPE_NONE    = 0x0,
	CIPHER_TYPE_DES     = 0x0,
	CIPHER_TYPE_3DES    = 0x0,
	CIPHER_TYPE_AES128  = 0x0,
	CIPHER_TYPE_AES192  = 0x1,
	CIPHER_TYPE_AES256  = 0x2,
} CIPHER_TYPE;

#define EALGINFO_NULL				 0
#define EALGINFO_DES_CBC			 1
#define EALGINFO_3DES_CBC		 	 2
#define EALGINFO_AES128_CBC			 3
#define EALGINFO_AES192_CBC			 4
#define EALGINFO_AES256_CBC			 5
#define EALGINFO_AES128_CTR			 6
#define EALGINFO_AES192_CTR			 7
#define EALGINFO_AES256_CTR			 8
#define EALGINFO_AES128_XTS			 9
#define EALGINFO_AES256_XTS			 10
#define EALGINFO_AES192_XTS			 11
#define EALGINFO_RC4			 	 12
#define EALGINFO_AES128_GCM			 13
#define EALGINFO_AES256_GCM			 14
#define EALGINFO_AES192_GCM			 15
#define EALGINFO_UNKNOWN             0xFF

typedef enum hash_alg {
	HASH_ALG_NULL   = 0x0,
	HASH_ALG_MD5    = 0x1,
	HASH_ALG_SHA1   = 0x2,
	HASH_ALG_SHA224 = 0x3,
	HASH_ALG_SHA256 = 0x4,
	HASH_ALG_AES    = 0x5,
} HASH_ALG;

typedef enum hash_mode {
	HASH_MODE_HASH  = 0x0,
	HASH_MODE_XCBC  = 0x0,
	HASH_MODE_CMAC  = 0x1,
	HASH_MODE_CTXT  = 0x1,
	HASH_MODE_HMAC  = 0x2,
	HASH_MODE_FHMAC = 0x6,
	HASH_MODE_CCM   = 0x5,
	HASH_MODE_GCM   = 0x6,
} HASH_MODE;

typedef enum hash_type {
	HASH_TYPE_FULL  = 0x0,
	HASH_TYPE_INIT  = 0x1,
	HASH_TYPE_UPDT  = 0x2,
	HASH_TYPE_FIN   = 0x3,
	HASH_TYPE_AES   = 0x0,
} HASH_TYPE;

#define AALGINFO_NONE                0
#define AALGINFO_HMAC_SHA1           1
#define AALGINFO_HMAC_MD5            2
#define AALGINFO_HMAC_SHA224         3
#define AALGINFO_HMAC_SHA256         4
#define AALGINFO_FHMAC_SHA1          5
#define AALGINFO_FHMAC_MD5           6
#define AALGINFO_CTXT_SHA1           7
#define AALGINFO_CTXT_MD5            8
#define AALGINFO_HASH_UPDT_SHA1      9
#define AALGINFO_HASH_UPDT_MD5       10
#define AALGINFO_HASH_UPDT_SHA224    11
#define AALGINFO_HASH_UPDT_SHA256    12
#define AALGINFO_HASH_FIN_SHA1       13
#define AALGINFO_HASH_FIN_MD5        14
#define AALGINFO_HASH_FIN_SHA224     15
#define AALGINFO_HASH_FIN_SHA256     16
#define AALGINFO_XCBC_MAC            17
#define AALGINFO_AES128_GMAC         18
#define AALGINFO_AES192_GMAC         19
#define AALGINFO_AES256_GMAC         20
#define AALGINFO_UNKNOWN             0xFF

#define SPU_CRYPTO_OPERATION_GENERIC	0x1
#define SPU_SCTX_TYPE_GENERIC          	0x0

#define MD5_STATE_SIZE	                16
#define SHA1_STATE_SIZE	                20

/* SPU error codes */
#define SPU_STATUS_MASK                 0x0000FF00
#define SPU_STATUS_SUCCESS              0x00000000
#define SPU_STATUS_INVALID_KEY          0x00000200
#define SPU_STATUS_INVALID_KEY_HANDLE   0x00000300
#define SPU_STATUS_INVALID_ICV          0x00000400

#define SPU_STATUS_ERROR_FLAG           0x00020000


/* Buffer Descriptor Header [BDESC] */
typedef struct BDESC_header_t {
	uint16_t	offsetMAC;     /* word 0 [31-16] */
	uint16_t	lengthMAC;     /* word 0 [15-0]  */
	uint16_t	offsetCrypto;  /* word 1 [31-16] */
	uint16_t	lengthCrypto;  /* word 1 [15-0]  */
	uint16_t	offsetICV;     /* word 2 [31-16] */
	uint16_t	offsetIV;      /* word 2 [15-0]  */
} BDESC_HEADER;

#define BDESC_HDR_OFFSET_CRYPTO_SHIFT   16 
#define BDESC_HDR_OFFSET_MAC_SHIFT      16 
#define BDESC_HDR_OFFSET_ICV_SHIFT      16 

/* Buffer Data Header [BD] */
typedef struct BD_header_t {
	uint16_t	size;
	uint16_t	PrevLength;
} BD_HEADER;

#define BD_HDR_SIZE_SHIFT               16 

/* Generic Mode Security Context Structure [SCTX] */
typedef struct _SCTX_ICV_MAC_flags {
	uint8_t insert_icv_mac:1;	/* [13] */
	uint8_t check_icv_mac:1;	/* [12] */
} SCTX_ICV_MAC_flags;

typedef struct _SCTX_IV_flags {
	uint8_t SCTX_IV:1;	    /* [7] */
	uint8_t explicit_IV:1;	/* [6] */
	uint8_t gen_IV:1;	    /* [5] */
} SCTX_IV_flags;

typedef struct {
	uint32_t type:2;	    /* [31:30] */
	uint32_t cacheable:1;	/* [29] */		/* Reserved in SPU-M: Do not set */
	uint32_t update_en:1;	/* [28] */		/* Reserved in SPU-M: Do not set */
	uint32_t lock_en:1;		/* [27] */		/* Reserved in SPU-M: Do not set */
	uint32_t cap_en:1;		/* [26] */		/* Reserved in SPU-M: Do not set */
	uint32_t reserved_2:1;	/* [25:24] */
	uint32_t transport:1;	/* [23] */		/* Reserved in SPU-M: Do not set */
	uint32_t ah:1;			/* [22] */		/* Reserved in SPU-M: Do not set */
	uint32_t esp:1;			/* [21] */		/* Reserved in SPU-M: Do not set */
	uint32_t authSeq64:1;	/* [20] */		/* Reserved in SPU-M: Do not set */
	uint32_t udp_en:1;		/* [19] */		/* Reserved in SPU-M: Do not set */
	uint32_t reserved_1:3;	/* [18:16] */
	uint32_t pad_en:1;		/* [15] */		/* Reserved in SPU-M: Do not set */
	uint32_t gen_id:1;		/* [14] */		/* Reserved in SPU-M: Do not set */
	uint32_t twoDestOpts:1;	/* [13] */		/* Reserved in SPU-M: Do not set */
	uint32_t ipv4cksum:1;	/* [12] */		/* Reserved in SPU-M: Do not set */
	uint32_t decrement_ttl:1;/* [11] */		/* Reserved in SPU-M: Do not set */
	uint32_t copy_tos:1;	/* [10] */		/* Reserved in SPU-M: Do not set */
	uint32_t copy_flow:1;	/* [9] */		/* Reserved in SPU-M: Do not set */
	uint32_t copy_df:1;		/* [8] */		/* Reserved in SPU-M: Do not set */
	uint32_t SCTX_size:8;	/* [7:0] */
} SCTX_sctx_info;

#define CENTURION_BIT_MASK  0xC00000FF  

/* Word 0: Procotol bit shifts */
#define  COPY_FLOW_SHIFT                 9
#define  COPY_TOS_SHIFT                 10
#define  GEN_ID_SHIFT                   14
#define  PAD_CHECK_SHIFT                15
#define  PAD_EN_SHIFT                   15
#define  UDP_EN_SHIFT                   19
#define  AUTH_SEQ64_SHIFT               20
#define  PROTOCOL_ESP_SHIFT             21
#define  PROTOCOL_AH_SHIFT              22
#define  TRANSPORT_MODE_SHIFT           23
#define  CAP_EN_SHIFT                   26
#define  UPDATE_EN_SHIFT                28
#define  SCTX_TYPE_SHIFT                30

typedef struct CCH_field_flags {
	uint8_t SUPDT_PR:1;	        /* [24] */
	uint8_t reserved1:1;	    /* [25] */
	uint8_t HASH_PR:1;	        /* [26] */
	uint8_t BD_PR:1;	        /* [27] */
	uint8_t MFM_PR:1;	        /* [28] */
	uint8_t BDESC_PR:1;	        /* [29] */
	uint8_t reserved2:1;    	/* [30] */
	uint8_t SCTX_PR:1;	        /* [31] */
} CCH_field_flags;

#define SPU_BD_BIT                (1 << 3)
#define SPU_HASH_BIT              (1 << 2)


/* Command Context Header */
typedef struct MessageHeader_t {
	CCH_field_flags flags;	    /* [31:24] */
	uint8_t opCode;		        /* [23:16] */
	uint16_t reserved;	        /* [15:0] */
} MHEADER;

typedef struct {
	uint32_t inbound:1;	    /* [31] */
	uint32_t order:1;	    /* [30] */
	uint32_t reserved:6;	/* [29:24] */
	uint32_t cryptoAlg:3;	/* [23:21] */
	uint32_t cryptoMode:3;	/* [20:18] */
	uint32_t cryptoType:2;	/* [17:16] */
	uint32_t hashAlg:3;	    /* [15:13] */
	uint32_t hashMode:3;	/* [12:10] */
	uint32_t hashType:2;	/* [9:8] */
	uint32_t UPDT_ofst:8;	/* [7:0] */
} SCTX_cipher_flags;

/* Word 1: Cipher bit shifts */
        
#define  HASH_TYPE_SHIFT                 8
#define  HASH_MODE_SHIFT                10
#define  HASH_ALG_SHIFT                 13
#define  CIPHER_TYPE_SHIFT              16
#define  CIPHER_MODE_SHIFT              18
#define  CIPHER_ALG_SHIFT               21
#define  CIPHER_INBOUND_SHIFT           31

typedef struct {
	uint32_t PK:1;		            /* [31] */
	uint32_t KEK:1;		            /* [30] */
	uint32_t reserved1:2;	        /* [29:28] */
	uint32_t AKC_handle:8;	        /* [27:20] */
	uint32_t reserved2:6;	        /* [19:14] */
	uint32_t insert_icv:1;	        /* [13] */
	uint32_t check_icv:1;	        /* [12] */
	uint32_t ICV_size:4;	        /* [11:8] */
	uint32_t SCTX_IV:1;	            /* [7] */
	uint32_t explicit_IV:1;	        /* [6] */
	uint32_t gen_IV:1;	            /* [5] */
	uint32_t iv_offset:2;	        /* [4:3] */
	uint32_t explicit_IV_size:3;	/* [2:0] */
} SCTX_extended_cipher_flags;

/* Word 2: extended cipher bit shifts */
#define  IV_OFFSET_SHIFT                 3
#define  GEN_IV_SHIFT                    5
#define  EXPLICIT_IV_SHIFT               6
#define  ICV_SIZE_SHIFT                  8 
#define  CHECK_ICV_SHIFT                12
#define  INSERT_ICV_SHIFT               13

#define  CTR_IV_SIZE                    16

/* Generic Mode Security Context Structure [SCTX] */
typedef struct SecurityContext_t {
/* word 0: protocol flags */
	union {
		uint32_t bits;
		SCTX_sctx_info flags;
	} protocol;
/* word 1: cipher flags */
	union {
		uint32_t bits;
	    SCTX_cipher_flags flags;
	} cipher;
/* word 2: Extended cipher flags */
	union {
		uint32_t bits;
		SCTX_extended_cipher_flags flags;
	} ecf;
} SCTX;

/* Status Header */
typedef struct StatusHeader_t {
	uint32_t errCategory:6;    /* [31:26] */
	uint32_t pktIndex:8;	   /* [25:18]   for users discretion */
	uint32_t error:1;	       /* [17]    */
	uint32_t reserved:1;	   /* [16]    */
	uint32_t errDetail:8;      /* [15:8]  */
	uint32_t pktTotal:8;	   /* [7:0]     for users discretion */
} STAT_HEADER;

typedef struct SPUHeader_t {
	MHEADER mh;
	uint32_t emh;
	SCTX sa;
} SPUHEADER;

// Boundries
#define MAX_WATCH_DOG	100
#define MAX_OPTION_SIZE	1500
#define MAX_IPV6_HEADER	1000

#define	ErrorWarn_ESET	0x80000000	// 31 Always set to 1 when an error or warning condition
					// needs to be reported.
#define ERROR_CATAGORY	0x7c000000	// 30:26 Relevant Error Categories:
#define IPSec_Out_ESET	0x00000000	// (1)00000: Outbound IPSec Protocol Processing Error
#define IPSec_Inb_ESET	0x04000000	// (1)00001: Inbound IPSec Protocol Processing Error
#define	SCTX_Fmt_ESET	0x10000000	// (1)00100: SCTX Format Error
#define	SCTX_Mgm_ESET	0x14000000	// (1)00101: SCTX Management Error (time out etc, out-of-sync etc.)
#define	Replay_ESET	0x18000000	// (1)00110: Anti-replay Error
#define	Auth_ESET	0x40000000	// (1)10000: Authentication Error
#define	Encr_ESET	0x44000000	// (1)10001: Encryption Error
#define Pad_ESET	0x48000000	// (1)10010: Pad Checking Error
#define Prot_ESET	0x54000000	// (1)10101: BroadSAFE Key Protection Error
#define Warn_ESET	0x7c000000	// (1)11111: Warning
#define Pkt_INDEX	0x03fc0000	// 25:18 Packet Index: Packet Index of the First Packet that
					// produces an Error in an MCR.
#define	Error_SET	0x00020000	// 17 Error
#define Done_SET	0x00010000	// 16 Done
#define Detail_MASK	0x0000ff00	// 15:8 Error Details
#define	Pktnum_MASK	0x000000ff	// 7:0 Number of Packets (unchanged from input)
#define Detail_SHIFT	8

// OPU Packet Error Details
#define EXCEEDED_MAX_IPV6_HEADER	(ErrorWarn_ESET | Error_SET | (0x54 << Detail_SHIFT) | IPSec_Out_ESET)
#define IP_INNER_VERSION_ERROR		(ErrorWarn_ESET | Error_SET | (0x56 << Detail_SHIFT) | IPSec_Inb_ESET)
#define IP_OUTER_VERSION_ERROR		(ErrorWarn_ESET | Error_SET | (0x58 << Detail_SHIFT) | IPSec_Inb_ESET)
#define IP_VERSION_ERROR		(ErrorWarn_ESET | Error_SET | (0x5A << Detail_SHIFT) | IPSec_Out_ESET)
#define IPV6_OUTER_HDR_ERROR_01		(ErrorWarn_ESET | Error_SET | (0x5C << Detail_SHIFT) | IPSec_Inb_ESET)
#define IPV6_OUTER_HDR_ERROR_02		(ErrorWarn_ESET | Error_SET | (0x5E << Detail_SHIFT) | IPSec_Inb_ESET)
#define IPV6_OUTER_HDR_ERROR_03		(ErrorWarn_ESET | Error_SET | (0x60 << Detail_SHIFT) | IPSec_Inb_ESET)
#define IPV6_OUTER_HDR_ERROR_04		(ErrorWarn_ESET | Error_SET | (0x61 << Detail_SHIFT) | IPSec_Inb_ESET)
#define IPV6_OUTER_HDR_ERROR_05		(ErrorWarn_ESET | Error_SET | (0x62 << Detail_SHIFT) | IPSec_Inb_ESET)
#define IPV6_INNER_HDR_ERROR		(ErrorWarn_ESET | Error_SET | (0x63 << Detail_SHIFT) | IPSec_Inb_ESET)
#define LAST_BDATA_DESC_NOT_FOUND_OB    (ErrorWarn_ESET | Error_SET | (0x64 << Detail_SHIFT) | IPSec_Out_ESET)
#define LAST_BDATA_DESC_NOT_FOUND_IB    (ErrorWarn_ESET | Error_SET | (0x65 << Detail_SHIFT) | IPSec_Inb_ESET)
#define	BAD_BDESC_TYPE_OB		(ErrorWarn_ESET | Error_SET | (0x66 << Detail_SHIFT) | IPSec_Out_ESET)
#define	BAD_BDESC_TYPE_IB		(ErrorWarn_ESET | Error_SET | (0x67 << Detail_SHIFT) | IPSec_Inb_ESET)
#define BAD_ESN_ALIGNMENT_OB		(ErrorWarn_ESET | Error_SET | (0x68 << Detail_SHIFT) | IPSec_Out_ESET)
#define BAD_ESN_ALIGNMENT_IB		(ErrorWarn_ESET | Error_SET | (0x69 << Detail_SHIFT) | IPSec_Inb_ESET)
#define MFM_ERROR			(ErrorWarn_ESET | Error_SET | (0x6A << Detail_SHIFT) | IPSec_Out_ESET)
// IPU Packet Error Details
#define IPV6_OUTER_HDR_ERROR_OB		(ErrorWarn_ESET | Error_SET | (0x2C << Detail_SHIFT) | IPSec_Out_ESET)
#define IPV6_OUTER_HDR_ERROR_IB		(ErrorWarn_ESET | Error_SET | (0x2E << Detail_SHIFT) | IPSec_Inb_ESET)
#define BAD_TUNNEL_IPV4_HEADER_LENGTH	(ErrorWarn_ESET | Error_SET | (0x31 << Detail_SHIFT) | IPSec_Out_ESET)
#define BAD_TUNNEL_IPV6_HEADER_LENGTH	(ErrorWarn_ESET | Error_SET | (0x32 << Detail_SHIFT) | IPSec_Out_ESET)

// Debug Error codes
#ifdef CHECK_PKT_LENGTH
#define PACKET_TOO_LONG_OB		(ErrorWarn_ESET | Error_SET | (0x6C << Detail_SHIFT) | IPSec_Out_ESET)
#define PACKET_TOO_LONG_IB		(ErrorWarn_ESET | Error_SET | (0x6D << Detail_SHIFT) | IPSec_Inb_ESET)
#define PACKET_TOO_SHORT_OB		(ErrorWarn_ESET | Error_SET | (0x6E << Detail_SHIFT) | IPSec_Out_ESET)
#define PACKET_TOO_SHORT_IB		(ErrorWarn_ESET | Error_SET | (0x6F << Detail_SHIFT) | IPSec_Inb_ESET)
#endif // CHECK_PKT_LENGTH

// OPU IPSec Protocol Error Details
#define PROD_IPSEC_DISABLED_OB		(ErrorWarn_ESET | Error_SET | (0x70 << Detail_SHIFT) | IPSec_Out_ESET)
#define PROD_IPSEC_DISABLED_IB		(ErrorWarn_ESET | Error_SET | (0x71 << Detail_SHIFT) | IPSec_Inb_ESET)
#define TTL_HOP_ZERO_ERROR		(ErrorWarn_ESET | Error_SET | (0x74 << Detail_SHIFT) | IPSec_Inb_ESET)
#define	UNKNOWN_TSPT_VERSION_ERROR	(ErrorWarn_ESET | Error_SET | (0x75 << Detail_SHIFT) | IPSec_Inb_ESET)
#define	BAD_TUNNEL_VERSION_ERROR	(ErrorWarn_ESET | Error_SET | (0x76 << Detail_SHIFT) | IPSec_Inb_ESET)
#define	MISSING_BCT_ON_ESP		(ErrorWarn_ESET | Error_SET | (0x77 << Detail_SHIFT) | IPSec_Inb_ESET)
#define	VERIFY_BAD_PAD_CHECK		(ErrorWarn_ESET | Error_SET | (0x7D << Detail_SHIFT) | Pad_ESET)

#define VERIFY_CHECKSUM_ERROR_OB	(ErrorWarn_ESET | Error_SET | (0x40 << Detail_SHIFT) | IPSec_Out_ESET)
#define VERIFY_CHECKSUM_ERROR_IB	(ErrorWarn_ESET | Error_SET | (0x41 << Detail_SHIFT) | IPSec_Inb_ESET)
#define VERIFY_IKE_SPI_ERROR		(ErrorWarn_ESET | Error_SET | (0x42 << Detail_SHIFT) | IPSec_Inb_ESET)
#define UDP_MISMATCH_ERROR		(ErrorWarn_ESET | Error_SET | (0x48 << Detail_SHIFT) | IPSec_Inb_ESET)
#define AH_MISMATCH_ERROR		(ErrorWarn_ESET | Error_SET | (0x49 << Detail_SHIFT) | IPSec_Inb_ESET)
#define ESP_MISMATCH_ERROR		(ErrorWarn_ESET | Error_SET | (0x4A << Detail_SHIFT) | IPSec_Inb_ESET)


#define EXP_OVERFLOW_ERROR		(ErrorWarn_ESET | Error_SET | (0x01 << Detail_SHIFT) | SCTX_Mgm_ESET)
#define PBC_OVERFLOW_ERROR		(ErrorWarn_ESET | Error_SET | (0x02 << Detail_SHIFT) | SCTX_Mgm_ESET)
#define SEQ_OVERFLOW_ERROR		(ErrorWarn_ESET | Error_SET | (0x03 << Detail_SHIFT) | SCTX_Mgm_ESET)

#define	WARNING_EXP_OVERFLOW_ERROR	(ErrorWarn_ESET | Warn_ESET | (0x01 << Detail_SHIFT))	// Outbound SA soft timer expiry
#define	WARNING_PBC_OVERFLOW_ERROR	(ErrorWarn_ESET | Warn_ESET | (0x02 << Detail_SHIFT))	// Outbound SA soft byte count expiry
#define	WARNING_SEQ_OVERFLOW_ERROR	(ErrorWarn_ESET | Warn_ESET | (0x03 << Detail_SHIFT))	// Outbound SA soft sequence number expiry

#define CJMP_BYPASS                     	(0x1)
#define CJMP_ERROR                      	(0x3)
#define CJMP_OB_TRANSPORT_PRE_SPU          	(0x4)
#define CJMP_OB_TUNNEL_PRE_SPU             	(0x5)
#define CJMP_IB_TRANSPORT_PRE_SPU           (0x6)
#define CJMP_IB_TUNNEL_PRE_SPU              (0x7)
#define CJMP_OB_TRANSPORT_POST_SPU         	(0x8)
#define CJMP_OB_TUNNEL_POST_SPU            	(0x9)
#define CJMP_IB_TRANSPORT_POST_SPU          (0xa)
#define CJMP_IB_TUNNEL_POST_SPU             (0xb)

#define BDESC_AH                        	(0x1)
#define BDESC_ESP_CRYPT_DES             	(0x2)
#define BDESC_ESP_CRYPT_AES_CTR			    (0x3)	/* New */
#define BDESC_ESP_CRYPT_AES             	(0x4)
#define BDESC_ESP_AUTH                  	(0x5)
#define BDESC_ESP_BOTH_DES              	(0x6)
#define BDESC_ESP_BOTH_AES_CTR			    (0x7)	/* New */
#define BDESC_ESP_BOTH_AES              	(0x8)

enum SEGMENT_TYPE { \
		JMP_NONE = 0, \
		JMP_BYPASS = CJMP_BYPASS, \
		JMP_ERROR = CJMP_ERROR, \
		JMP_OB_TRANSPORT_PRE_SPU = CJMP_OB_TRANSPORT_PRE_SPU, \
		JMP_OB_TUNNEL_PRE_SPU = CJMP_OB_TUNNEL_PRE_SPU, \
		JMP_IB_TRANSPORT_PRE_SPU = CJMP_IB_TRANSPORT_PRE_SPU, \
		JMP_IB_TUNNEL_PRE_SPU = CJMP_IB_TUNNEL_PRE_SPU, \
		JMP_OB_TRANSPORT_POST_SPU = CJMP_OB_TRANSPORT_POST_SPU, \
		JMP_OB_TUNNEL_POST_SPU = CJMP_OB_TUNNEL_POST_SPU, \
		JMP_IB_TRANSPORT_POST_SPU = CJMP_IB_TRANSPORT_POST_SPU, \
		JMP_IB_TUNNEL_POST_SPU = CJMP_IB_TUNNEL_POST_SPU, \
};

enum DESC_TYPE { \
		NONE = 0, \
		AHB = BDESC_AH, \
		ESP_CRYPTO_DES = BDESC_ESP_CRYPT_DES, \
		ESP_CRYPTO_AES_CTR = BDESC_ESP_CRYPT_AES_CTR, \
		ESP_CRYPTO_AES = BDESC_ESP_CRYPT_AES, \
		ESP_AUTH = BDESC_ESP_AUTH, \
		ESP_BOTH_DES = BDESC_ESP_BOTH_DES, \
		ESP_BOTH_AES_CTR = BDESC_ESP_BOTH_AES_CTR, \
		ESP_BOTH_AES = BDESC_ESP_BOTH_AES, \
};

/************** R5 PAE registers **********************/
#define R5_R_FA_UNIMAC0_COMMAND_CONFIG       0x18024808
#define R5_R_FA_UNIMAC0_PAUSE_CONTROL        0x18024b30
#define R5_R_CTF_DEBUG_CONTROL               0x18025ca0
#define R5_R_CTF_FIFO_DEBUG0_BASE            0x18025cb4
#define R5_R_CTF_FIFO_DEBUG1_BASE            0x18025cb8
#define R5_R_CTF_SPU_DEBUG0_BASE             0x18025cbc
#define R5_R_CTF_SPU_DEBUG1_BASE             0x18025cc0
#define R5_R_CTF_SPU_DEBUG2_BASE             0x18025cc4
#define R5_R_CTF_SPU_DEBUG3_BASE             0x18025cc8
#define R5_R_CTF_SPU_DEBUG4_BASE             0x18025ccc
#define R5_R_CTF_SPU_DEBUG5_BASE             0x18025cd0

#define R5_R_SPU_CONTROL				     0x1802f000
#define R5_R_PAE_BUFFER_CONFIG               0x18049010
#define R5_R_PAE_SCRATCHPAD_ALLOCATION       0x18049014
#define R5_R_PAE_BUFFER_ALLOCATION0          0x18049018
#define R5_R_PAE_BUFFER_ALLOCATION1          0x1804901c
#define R5_R_PAE_BUFFER_BACKPRESSURE_CONFIG0 0x18049020
#define R5_R_PAE_BUFFER_BACKPRESSURE_CONFIG1 0x18049024
#define R5_R_PAE_BUFFER_CONGESTION_CONFIG    0x18049028
#define R5_R_PAE_BUFFER_BACKPRESSURE_MAP0    0x1804902c
#define R5_R_PAE_BUFFER_BACKPRESSURE_MAP1    0x18049030
#define R5_R_PAE_ENQ_PKT_COUNTER0		     0x18049054
#define R5_R_PAE_ENQ_PKT_COUNTER1		     0x18049058
#define R5_R_PAE_DEQ_PKT_COUNTER0		     0x1804905c
#define R5_R_PAE_DEQ_PKT_COUNTER1		     0x18049060
#define R5_R_R5_CONFIG0                      0x180490d8

#define UNIMAC0_ETHSPEED_2GBYTE                 3
#define UNIMAC0_ETHSPEED_1GBYTE                 2 
#define UNIMAC0_COMMAND_CONFIG_TX_ENA_SHIFT     0  
#define UNIMAC0_COMMAND_CONFIG_RX_ENA_SHIFT     1  
#define UNIMAC0_COMMAND_CONFIG_ETHSPEED_SHIFT   2  
#define UNIMAC0_COMMAND_CONFIG_PROMIS_SHIFT     4  
#define UNIMAC0_COMMAND_CONFIG_CRC_FWD_SHIFT    6  
#define UNIMAC0_COMMAND_CONFIG_OOB_EFC_EN_SHIFT 29  
#define UNIMAC0_COMMAND_CONFIG_IGNORE_TX_PAUSE_SHIFT 28  
#define CTF_DEBUG_CONTROL_DM_FIFO_LEVEL_SHIFT  10
#define PAE_BUFFER_BACKPRESS_XOFF_SHIFT        16
#define SPU_CONTROL__OUT_ENDIAN_SHIFT          12
#define SPU_CONTROL__IN_ENDIAN_SHIFT           11
#define SPU_CONTROL__SOFT_RST_SHIFT             1


/************** PAE SPU Functions Prototypes ***********/

int pae_spu_init(void);
int pae_ipsec_offload_init(void);
int check_ipsec_firmware_version(int verbose);
#endif /* _PAE_SPU_H */
