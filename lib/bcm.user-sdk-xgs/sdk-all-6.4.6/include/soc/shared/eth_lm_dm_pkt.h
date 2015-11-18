/* 
 * $Id: eth_lm_dm_pkt.h,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        eth_lm_dm_pkt.h
 * Purpose:     ETH_LM_DM Packet Format definitions
 *              common to SDK and uKernel.
 */

#ifndef   _ETH_LM_DM_PKT_H_
#define   _ETH_LM_DM_PKT_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>
#include <shared/pack.h>



/*******************************************
 * Pack/Unpack Macros
 * 
 * Data is packed/unpacked in/from Network byte order
 */
#define SHR_ETH_LM_DM_ENCAP_PACK_U8(_buf, _var)     _SHR_PACK_U8(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_PACK_U16(_buf, _var)    _SHR_PACK_U16(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_PACK_U32(_buf, _var)    _SHR_PACK_U32(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_UNPACK_U8(_buf, _var)   _SHR_UNPACK_U8(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_UNPACK_U16(_buf, _var)  _SHR_UNPACK_U16(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_UNPACK_U32(_buf, _var)  _SHR_UNPACK_U32(_buf, _var)


/******************************************
 * Network Packet Format definitions
 *
 * Note: LENGTH is specified in bytes unless noted otherwise
 */

/* ETH_LM_DM Control Packet lengths */
#define SHR_ETH_LM_DM_HEADER_LENGTH 						4               

/*Macros for Packet replacement offset */
#define ETH_LM_DM_RPL_OFFSET_START              14
#define ETH_LM_DM_RPL_OFFSET_DIVIDER_BY_SHIFT   1


/* Ether Type */
#define SHR_ETH_LM_DM_L2_VLAN_ETYPE                  0x8100
#define SHR_ETH_LM_DM_L2_ETYPE_OAM	                 0x8902
#define SHR_ETH_LM_DM_L2_HEADER_TAGGED_LENGTH        18
#define SHR_ETH_LM_DM_L2_HEADER_LENGTH        		 12
#define SHR_ETH_LM_DM_L2_HEADER_TAGGED_ETYPE_OFFSET  16

/* ETH_LM_DM op-code */
#define SHR_ETH_LM_DM_OPCODE_LEN                     1
#define SHR_ETH_LM_DM_OPCODE_LM_PREFIX              42
#define SHR_ETH_LM_DM_OPCODE_DM_PREFIX              44


#define SHR_ETH_LM_DM_OLP_L2_HEADER_LENGTH			18
#define SHR_ETH_LM_DM_OLP_RX_HEADER_LENGTH			20



/*************************************
 * ETH_LM_DM Control Packet Format
 */

/*
 * ETH_LM_DM Y.1731 Header
 */
typedef struct shr_eth_lm_dm_header_s {
    /* Mandatory */
#ifdef LE_HOST
    uint32 tlv_offset:8,
    	   flags_type:1,
           flags_rsvd:7,
           opcode:8,
           version:5,
           mel:3;
#else
    uint32 mel:3,
           version:5,
           opcode:8,
           flags_rsvd:7,
           flags_type:1,
           tlv_offset:8;
#endif  /* LE_HOST */
} shr_eth_lm_dm_header_t;

typedef struct shr_eth_lm_dm_slm_info_header_s {
	
	uint16 src_mep_id;
	uint16 resp_mep_id;
	uint32 test_id;

  
} shr_eth_lm_dm_slm_info_header_t;


/*
 * ETH_LM_DM OLP Header
 */

typedef struct soc_olp_rx_hdr olp_rx_hdr_t;

typedef	struct soc_olp_tx_hdr olp_tx_hdr_t;

/* L2 Header */
typedef struct soc_olp_l2_hdr_s  _olp_l2_header_t;




#endif /* _ETH_LM_DM_PKT_H_ */
