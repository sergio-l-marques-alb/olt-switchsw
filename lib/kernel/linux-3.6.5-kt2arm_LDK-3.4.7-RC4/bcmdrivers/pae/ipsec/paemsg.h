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
*  @file    paemsg.h
*
*  @brief   PAE IPsec header file 
*
*  @note
*
*****************************************************************************/

#ifndef __PAEMSG_H__
#define __PAEMSG_H__

#include "pae_ipsec.h"

typedef struct PaeIPsecConfigFlags_s {
    uint32_t cp_id:1;   /* Copy ID field for ipv4 tunnel mode */
    uint32_t cp_tos:1;  /* Copy TOS field for ipv4 tunnel mode */
    uint32_t cp_ttl:1;  /* Copy TTL field for ipv4 tunnel mode */
    uint32_t ipv4_ttl:8;/* ipv4 TTL value if cp_ttl is not set */
    uint32_t ipv4_tos:8;/* ipv4 TOS value if cp_tos is not set */
    uint32_t unused:13;
} PAEIPSEC_FLAGS;

typedef struct pae_ipsec_config_s {
    PAEIPSEC_FLAGS flags;    /* IN: flags of operations details */
    uint32_t sadb_esp_sa_max; /* Max Number of ESP SA */
    uint32_t sadb_ah_sa_max;  /* Max Number AH SA  */
    uint8_t sa_expire_thrsh; /* Threshold of SA expiration */
    uint8_t debug_level;     /* Default Debug Level */
    uint16_t debug_buffer_size; /* size of the debug buffer */
} PAEIPSEC_CONFIG;

typedef struct pae_ipsec_stats {
  uint32_t rx_esp_in_packets;       /* Number of inbound ESP packets received by the PAE */
  uint32_t tx_esp_in_packets;       /* Number of inbound ESP packets processed by the PAE */
  uint32_t rx_esp_out_packets;      /* Number of outbound ESP packets received by the PAE */
  uint32_t tx_esp_out_packets;      /* Number of outbound ESP packets processed by the PAE */
  uint32_t rx_ah_in_packets;        /* Number of inbound AH packets received by the PAE */
  uint32_t tx_ah_in_packets;        /* Number of inbound AH packets processed by the PAE */
  uint32_t rx_ah_out_packets;       /* Number of outbound AH packets received by the PAE */
  uint32_t tx_ah_out_packets;       /* Number of outbound AH packets processed by the PAE */
  uint32_t ing_spu_esp_in_packets;  /* Number of inbound ESP packets sent to the SPU */
  uint32_t egr_spu_esp_in_packets;  /* Number of inbound ESP packets processed by the SPU */
  uint32_t ing_spu_esp_out_packets; /* Number of outbound ESP packets sent to the SPU */
  uint32_t egr_spu_esp_out_packets; /* Number of outbound ESP packets processed by the SPU */
  uint32_t ing_spu_ah_in_packets;   /* Number of inbound AH packets sent to the SPU */
  uint32_t egr_spu_ah_in_packets;   /* Number of inbound AH packets processed by the SPU */
  uint32_t ing_spu_ah_out_packets;  /* Number of outbound AH packets sent to the SPU */
  uint32_t egr_spu_ah_out_packets;  /* Number of outbound AH packets processed by the SPU */
  uint32_t spu_esp_in_packet_error; /* Number of inbound ESP packet errors in the SPU */
  uint32_t spu_esp_out_packet_error;/* Number of outbound ESP packet errors in the SPU */
  uint32_t spu_ah_in_packet_error;  /* Number of inbound AH packet errors in the SPU */
  uint32_t spu_ah_out_packet_error; /* Number of outbound AH packet errors in the SPU */
  uint32_t pae_esp_in_packet_error; /* Number of inbound ESP packet errors in the PAE */
  uint32_t pae_esp_out_packet_error;/* Number of outbound ESP packet errors in the PAE */
  uint32_t pae_ah_in_packet_error;  /* Number of inbound AH packet errors in the PAE */
  uint32_t pae_ah_out_packet_error; /* Number of outbound AH packet errors in the PAE */
} PAE_IPSEC_STATS;


/* PAE Message types */
#define PAE_MSG_TYPE_IPSEC_INIT				1
#define PAE_MSG_TYPE_IPSEC_SHUTDOWN			2
#define PAE_MSG_TYPE_IPSEC_GET_STATS		3 
#define PAE_MSG_TYPE_IPSEC_ADD_SA			4
#define PAE_MSG_TYPE_IPSEC_DELETE_SA		5
#define PAE_MSG_TYPE_IPSEC_LIST_SA			6
#define PAE_MSG_TYPE_IPSEC_CLEAR_SA			7
#define PAE_MSG_TYPE_IPSEC_GET_SA_STATS		8 
#define PAE_MSG_TYPE_IPSEC_SA_EXPIRE	    9	
#define PAE_MSG_TYPE_IPSEC_GET_DEBUG		10
#define PAE_MSG_TYPE_IPSEC_SET_DEBUG		11

typedef struct TLVWord_t {
    uint16_t type;
    uint16_t length;
} TLVWORD;

typedef struct PAEHeader_t {
    uint16_t sequenceID;
    uint16_t flags;
    TLVWORD tlv;
} PAEHEADER;

/* Added Payloads to a pending SA */
#define TLV_POLICY_PAYLOAD    1
#define TLV_TUNNEL_PAYLOAD    2
#define TLV_NAT_UDP_PAYLOAD   3
#define TLV_SRC_ADDR_PAYLOAD  4 
#define TLV_L2HEADER_PAYLOAD  5 

/* Some tunnel headers default values */
#define IPV4_TOS_DEFAULT      0
#define IPV4_TTL_DEFAULT      64
#define IPV6_HOP_DEFAULT      64
#define IPV6_CLASS_DEFAULT    0
#define IPV6_FLABEL_DEFAULT   0

#undef USE_IPSEC_NAT_TRAVERSAL

#define MAX_SA_COUNT	256
#define UNKNOWN_SA_IDX  MAX_SA_COUNT

int GetSAIndex (PAEIF_SAID *said);
int pae_add_action_stats_attribute(struct attribute *attr);
void pae_remove_action_stats_attribute(struct attribute *attr);

#endif /* __PAEMSG_H__*/
