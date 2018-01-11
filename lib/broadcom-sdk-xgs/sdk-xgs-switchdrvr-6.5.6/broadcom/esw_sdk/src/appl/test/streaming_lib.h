/*
 * $Id: streaming_library.h,v 1.0 Broadcom SDK $
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
 *
 * Extern declarations for test functions in streaming_library.c 
 */

#ifndef STREAMING_LIB_H
#define STREAMING_LIB_H

/* test type */
#define PKT_TYPE_L2 0
#define PKT_TYPE_IPMC 1
#define PKT_TYPE_L3UC 2

/* tx/rx channel */
#define TX_CHAN 0
#define RX_CHAN 1

#define TPID 0x8100
#define NUM_BYTES_MAC_ADDR 6
#define NUM_BYTES_CRC 4

#define MIN_PKT_SIZE 64
#define MTU 9216
#define MTU_CELL_CNT 45
#define TARGET_CELL_COUNT 100

#define NUM_SUBP_OBM 4

/* packet size */
#define HG2_WC_PKT_SIZE 64
#define ENET_WC_PKT_SIZE 145
#define ENET_IPG 12
#define ENET_PREAMBLE 8
#define HG2_IPG 8
#define HG2_HDR 12

/* cell size */
#define CELL_SIZE 208
#define FIRST_CELL_ENET 144
#define FIRST_CELL_HG2 148

#define TXDMA_TIMEOUT 100000
#define RXDMA_TIMEOUT 100000

/* ANCL bandwidth */
#define TH_MISC_BW 10000
#define TD2P_MISC_BW 10000
#define TH2_MISC_BW 25000
#define TD3_MISC_BW 30000

/* freq and bandwidth : TH */
#define TH_FREQ_850 850
#define TH_FREQ_765 765
#define TH_FREQ_672 672
#define TH_FREQ_645 645
#define TH_FREQ_545 545
#define TH_BW_850 561000
#define TH_BW_765 504000
#define TH_BW_672 442000
#define TH_BW_645 423000
#define TH_BW_545 358000

/* freq and bandwidth : TD2P */
#define TD2P_FREQ_793 793
#define TD2P_FREQ_635 635
#define TD2P_FREQ_529 529
#define TD2P_FREQ_421 421
#define TD2P_BW_793 500000
#define TD2P_BW_635 380000
#define TD2P_BW_529 340000
#define TD2P_BW_421 260000

/* freq and bandwidth : TH2 */
#define TH2_FREQ_1700 1700
#define TH2_FREQ_1625 1625
#define TH2_FREQ_1525 1525
#define TH2_FREQ_1425 1425
#define TH2_FREQ_1325 1325
#define TH2_FREQ_1275 1275
#define TH2_FREQ_1225 1225
#define TH2_FREQ_1133 1133
#define TH2_FREQ_1125 1125
#define TH2_FREQ_1050 1050
#define TH2_FREQ_950 950
#define TH2_FREQ_850 850
#define TH2_BW_1700 1122000
#define TH2_BW_1625 1072000
#define TH2_BW_1525 1006000
#define TH2_BW_1425 940000
#define TH2_BW_1325 874000
#define TH2_BW_1275 841000
#define TH2_BW_1225 808000
#define TH2_BW_1133 747000
#define TH2_BW_1125 742000
#define TH2_BW_1050 693000
#define TH2_BW_950 627000
#define TH2_BW_850 561000

/* freq and bandwidth : TD3 */
#define TD3_FREQ_1700 1700
#define TD3_FREQ_1625 1625
#define TD3_FREQ_1525 1525
#define TD3_FREQ_1425 1425
#define TD3_FREQ_1325 1325
#define TD3_FREQ_1275 1275
#define TD3_FREQ_1133 1133
#define TD3_FREQ_1125 1125
#define TD3_FREQ_1083 1083
#define TD3_FREQ_1016 1016
#define TD3_FREQ_950 950
#define TD3_FREQ_883 883
#define TD3_FREQ_850 
#define TD3_BW_1700 1122000
#define TD3_BW_1625 1072000
#define TD3_BW_1525 1006000
#define TD3_BW_1425 940000
#define TD3_BW_1325 874000
#define TD3_BW_1275 841000
#define TD3_BW_1133 747000
#define TD3_BW_1125 742000
#define TD3_BW_1083 714000
#define TD3_BW_1016 670000
#define TD3_BW_950 627000
#define TD3_BW_883 582000
#define TD3_BW_850 561000

#if defined(BCM_ESW_SUPPORT)

/* 
 * Struct:
 *      stream_rate_t
 *
 * Description:
 */
typedef struct stream_rate_calc_s {
    pbmp_t pbmp;         /* port bitmap */
    uint32 mode;         /* 0-> check by max rx speed;
                            1-> check by actual tx speed */
    uint32 pkt_size;     /* packet size */
    uint32 interval_len; /* length of interval in second */
    uint32 tolerance_lr; /* rate tolerance percentage for linerate */
    uint32 tolerance_os; /* rate tolerance percentage for oversub */
    uint32 src_port[SOC_MAX_NUM_PORTS];/* source port array */
    uint32 num_rep[SOC_MAX_NUM_PORTS]; /* number of same-port replications */
} stream_rate_t;

/* 
 * Struct:
 *      stream_integrity_t
 *
 * Description:
 */
typedef struct stream_pkt_intg_s{
    int type;       /* packet type: PKT_TYPE_L2, PKT_TYPE_IPMC, etc. */
    int ipv6_en;    /* enable ipv6, 1->enable, 0->disable  */
    pbmp_t tx_pbmp; /* tx port bitmap */
    pbmp_t rx_pbmp; /* rx port bitmap */
    uint32 tx_vlan[SOC_MAX_NUM_PORTS];        /* tx vlan array */
    uint32 rx_vlan[SOC_MAX_NUM_PORTS];        /* tx vlan array */
    uint32 port_pkt_seed[SOC_MAX_NUM_PORTS];  /* port packet seed array */
    uint32 port_flood_cnt[SOC_MAX_NUM_PORTS]; /* port flood count array */
    uint8 mac_da[SOC_MAX_NUM_PORTS][NUM_BYTES_MAC_ADDR];
    uint8 mac_sa[SOC_MAX_NUM_PORTS][NUM_BYTES_MAC_ADDR];
    uint32 ip_da[SOC_MAX_NUM_PORTS]; /* port ip DA array */
    uint32 ip_sa[SOC_MAX_NUM_PORTS]; /* port ip SA array */
} stream_integrity_t;

/* 
 * Struct:
 *      stream_pkt_t
 *
 * Parameters:
 */
typedef struct stream_pkt_s{
    int port;
    int num_pkt;           /* expected number of tx/rx packets */
    int cnt_pkt;           /* actual number of tx/rx packets */
    uint32 pkt_seed;       /* random seed for pkt generation */
    uint32 pkt_size;       /* constant pkt size */
    int rand_pkt_size_en;  /* enable random pkt size */
    uint32 *rand_pkt_size; /* random pkt size array */
    int l3_en;             /* 1 -> enable, 0 -> disable (default) */
    uint8 ipv6_en;         /* 1 -> enable, 0 -> disable (default) */
    uint32 tx_vlan;
    uint32 rx_vlan;
    uint8 mac_da[NUM_BYTES_MAC_ADDR];
    uint8 mac_sa[NUM_BYTES_MAC_ADDR];
    uint32 ip_da;
    uint32 ip_sa;
    uint8 ttl;
} stream_pkt_t;

extern void stream_print_port_config(int unit, pbmp_t pbmp);

extern uint32 stream_get_port_pipe(int unit, int port);
extern uint32 stream_get_pkt_cell_cnt(int unit, uint32 pkt_size, int port);
extern uint32 stream_get_safe_pkt_size(int unit, int ovs_ratio_x1000);
extern uint32 stream_get_ll_flood_cnt(int unit, int port, uint32 pkt_size,
                                      uint32 *pkt_size_rand);
extern uint32 stream_get_pipe_bandwidth(int unit, uint32 flag);
extern uint32 stream_get_ancl_bandwidth(int unit, uint32 flag);

extern void stream_set_vlan(int unit, bcm_vlan_t vlan, int enable);
extern void stream_set_vlan_valid(int unit, bcm_vlan_t vlan);
extern void stream_set_vlan_invalid(int unit, bcm_vlan_t vlan);
extern bcm_error_t stream_set_lpbk(int unit, pbmp_t pbmp, int loopback);
extern void stream_set_mac_lpbk(int unit, pbmp_t pbmp);
extern void stream_set_phy_lpbk(int unit, pbmp_t pbmp);
extern void stream_set_no_lpbk(int unit, pbmp_t pbmp);

extern void stream_turn_off_cmic_mmu_bkp(int unit);
extern void stream_turn_off_fc(int unit, pbmp_t pbmp);

extern void stream_gen_random_l2_pkt(uint8 *pkt_ptr,
                                     uint32 pkt_size,uint32 seq_id,
                                     uint8 mac_da[NUM_BYTES_MAC_ADDR],
                                     uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                                     uint16 tpid, uint16 vlan_id);
extern void stream_gen_random_l3_pkt(uint8 *pkt_ptr, uint8 ipv6_en,
                                     uint32 pkt_size, uint32 seq_id,
                                     uint8 mac_da[NUM_BYTES_MAC_ADDR],
                                     uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                                     uint16 vlan_id,
                                     uint32 ip_da, uint32 ip_sa, uint8 ttl);

extern bcm_error_t stream_pktdma_tx(int unit, dv_t *dv, uint8 *pkt, uint16 cnt);
extern bcm_error_t stream_pktdma_rx(int unit, int rst_vlan, bcm_vlan_t vlan,
                                    dv_t *dv, uint8 *pkt, uint16 cnt);

extern bcm_error_t stream_tx_pkt(int unit, stream_pkt_t *tx_pkt);

extern bcm_error_t stream_chk_dma_chain_done(int unit, int vchan,
                                             soc_dma_poll_type_t type,
                                             int *detected);
extern bcm_error_t stream_chk_mib_counters(int unit, pbmp_t pbmp, int flag);
extern bcm_error_t stream_chk_port_rate(int unit, pbmp_t pbmp,
                                        stream_rate_t *rate_calc);
extern bcm_error_t stream_chk_pkt_integrity(int unit,
                                            stream_integrity_t *pkt_intg);
#endif /* BCM_ESW_SUPPORT */

#endif /* STREAMING_LIB_H */
