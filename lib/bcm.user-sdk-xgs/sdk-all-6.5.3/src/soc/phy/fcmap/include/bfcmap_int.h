/*
 * $Id: bfcmap_int.h,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 */

#ifndef BFCMAP_INTERNAL_H
#define BFCMAP_INTERNAL_H

#include <bfcmap_sal.h>
#include <bfcmap_util.h>
#include <bfcmap_vlan.h>
#include <bfcmap_stat.h>
#include <bfcmap_ident.h>
#include <bfcmap_drv.h>
#include <bfcmap_ctrl.h>


/* bmacsec equiv of reg/mem is in mem.h */
#define BFCMAP_REG64_READ(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_RD, (a), 2, 1, (pv))

#define BFCMAP_REG32_READ(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_RD, (a), 1, 1, (pv))

#define BFCMAP_REG64_WRITE(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_WR, (a), 2, 1, (pv))

#define BFCMAP_REG32_WRITE(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_WR, (a), 1, 1, (pv))


#define BFCMAP_WWN_HI(mac)          BMAC_TO_32_HI(mac)
#define BFCMAP_WWN_LO(mac)          BMAC_TO_32_LO(mac)
#define BFCMAP_WWN_BUILD(mac,hi,lo) BMAC_BUILD_FROM_32(mac,hi,lo)

#define BFCMAP_PID_TO_32(pid)  \
   ((((buint8_t *)(pid))[0] << 16 )|\
    (((buint8_t *)(pid))[1] << 8  )|\
    (((buint8_t *)(pid))[2] << 0  ))

#define BFCMAP_PID_BUILD(pid, val32)\
   ((buint8_t *)(pid))[0] = ((val32) >> 16) & 0xff ;\
   ((buint8_t *)(pid))[1] = ((val32) >> 8 ) & 0xff ;\
   ((buint8_t *)(pid))[2] = ((val32) >> 0 ) & 0xff ;


typedef enum {
    BFCMAP_UC_FIRMWARE_INIT = 0xA,
    BFCMAP_UC_LINK_RESET,
    BFCMAP_UC_LINK_BOUNCE,
    BFCMAP_UC_LINK_ENABLE,
    BFCMAP_UC_LINK_SPEED,
    BFCMAP_UC_RX_BBC_SET
} bfcmap_lmi_uc_cmds_t ;

typedef enum {
    BFCMAP_UC_LINK_UP = 0x1A,
    BFCMAP_UC_LINK_FAILURE
} bfcmap_uc_lmi_cmds_t ;

#if defined(INCLUDE_PHY_8806X)
/* Inband packet handling */

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a)    __attribute__ (_a)
#endif /* COMPILER_ATTRIBUTE */

typedef struct bfcmap_mibs_s {
    buint64_t fc_c2_rxgoodframes;
    buint64_t fc_c2_rxinvalidcrc;  
    buint64_t fc_c2_rxframetoolong;
    buint64_t fc_c3_rxgoodframes;
    buint64_t fc_c3_rxinvalidcrc;
    buint64_t fc_c3_rxframetoolong;
    buint64_t fc_cf_rxgoodframes;
    buint64_t fc_cf_rxinvalidcrc;
    buint64_t fc_cf_rxframetoolong;
    buint64_t fc_rxdelimitererr;
    buint64_t fc_rxbbc0drop;
    buint64_t fc_rxbbcredit0;
    buint64_t fc_rxlinkfail;
    buint64_t fc_rxsyncfail;
    buint64_t fc_rxlosssig;
    buint64_t fc_rxprimseqerr;
    buint64_t fc_rxinvalidset;
    buint64_t fc_rxlpicount;
    buint64_t fc_rxencodedisparity;
    buint64_t fc_rxbadxword;
    buint64_t fc_rx_c2_runtframes;
    buint64_t fc_rx_c3_runtframes;
    buint64_t fc_rx_cf_runtframes;
    buint64_t fc_c2_rxbyt;
    buint64_t fc_c3_rxbyt;
    buint64_t fc_cf_rxbyt;
    buint64_t fc_tx_c2_frames;
    buint64_t fc_tx_c3_frames;
    buint64_t fc_tx_cf_frames;
    buint64_t fc_tx_c2_oversized_frames;
    buint64_t fc_tx_c3_oversized_frames;
    buint64_t fc_tx_cf_oversized_frames;
    buint64_t fc_txbbcredit0;
    buint64_t fc_c2_txbyt;
    buint64_t fc_c3_txbyt;
    buint64_t fc_cf_txbyt;
} COMPILER_ATTRIBUTE((packed)) bfcmap_mibs_t;

#define FC_INBAND_FRAME_SIGNATURE 0xFC0E
#define FC_INBAND_NUM_TLV_MAX 0x3

typedef struct bfcmap_inb_signature_s {
    buint16_t sign;
    buint8_t num_tlv;
    buint8_t frm_seq;
} COMPILER_ATTRIBUTE((packed)) bfcmap_inb_signature_t;

typedef enum bfcmap_inb_pld_type {
    FC_INB_PLD_TYPE_FC_MIBS    = 1,
    FC_INB_PLD_TYPE_FC_EVENTS  = 2,
    FC_INB_PLD_TYPE_ETH_MIBS   = 3
} bfcmap_inb_pld_type_t;

typedef struct bfcmap_inb_tlv_s {
    buint16_t pld_type;
    buint16_t pld_len;
} COMPILER_ATTRIBUTE((packed)) bfcmap_inb_tlv_t;
         
typedef struct bfcmap_inb_pl_s {
    bfcmap_inb_tlv_t tlv;
    buint64_t word[1];
} COMPILER_ATTRIBUTE((packed)) bfcmap_inb_pl_t;

/* Inband FC frame Payload */
typedef struct bfcmap_inband_fr_payload_s {
    bfcmap_inb_signature_t fc_inb_signature;
    /* payloads */
    bfcmap_inb_pl_t pl1;
    bfcmap_inb_pl_t pl2;
    bfcmap_inb_pl_t pl3;
} COMPILER_ATTRIBUTE((packed)) bfcmap_inband_fr_payload_t;

void bfcmap88060_fc_mibs_update(bfcmap_mibs_t *bfcmap_port_mibs, buint64_t *cur_fc_mibs);
#endif /* INCLUDE_PHY_8806X */

#endif /* BFCMAP_INTERNAL_H */
