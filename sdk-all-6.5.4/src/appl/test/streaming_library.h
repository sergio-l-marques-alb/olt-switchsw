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

#define MAC_DA {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc}
#define MAC_SA {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54}
#define TPID 0x8100
#define VLAN 0x0a00
#define IPV4_ADDR 0xc0a80001
#define IPV6_ADDR { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                    0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01 };

#define MIN_PKT_SIZE 64
#define MTU 9216
#define MTU_CELL_CNT 45

#define TARGET_CELL_COUNT 100

#define TX_CHAN 0
#define RX_CHAN 1

#define HG2_WC_PKT_SIZE 64
#define ENET_WC_PKT_SIZE 145
#define ENET_IPG 12
#define ENET_PREAMBLE 8
#define HG2_IPG 8

#define CELL_SIZE 208
#define FIRST_CELL_ENET 144
#define FIRST_CELL_HG2 148
#define RXDMA_TIMEOUT 100000

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

#define TD2P_FREQ_793 793
#define TD2P_FREQ_635 635
#define TD2P_FREQ_529 529
#define TD2P_FREQ_421 421
#define TD2P_BW_793 500000
#define TD2P_BW_635 380000
#define TD2P_BW_529 340000
#define TD2P_BW_421 260000

#define MISC_BW 10000

#define NUM_SUBP_OBM 4

#if defined(BCM_ESW_SUPPORT)

typedef struct rate_calc_s {
    uint32 ovs_ratio_x1000[SOC_MAX_NUM_PIPES];
    uint32 rand_pkt_sizes[SOC_MAX_NUM_PORTS][TARGET_CELL_COUNT];
    sal_usecs_t stime[SOC_MAX_NUM_PORTS];
    uint64 exp_rate[SOC_MAX_NUM_PORTS];
    uint64 tpkt_start[SOC_MAX_NUM_PORTS];
    uint64 tbyt_start[SOC_MAX_NUM_PORTS];
} rate_calc_t;

extern int get_pipe(int unit, int port);
extern uint32 num_cells(int unit, uint32 pkt_size, int port);
extern uint32 safe_pkt_size(int unit, int ovs_ratio_x1000);
extern uint32 lossless_flood_cnt(int unit, uint32 pkt_size, int port);
extern void validate_vlan(int unit, bcm_vlan_t vlan, int enable);
extern bcm_error_t dma_chan_check_done(int unit, int vchan, soc_dma_poll_type_t type,
                               int *detected);
extern void gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size,
                              uint8 mac_da[NUM_BYTES_MAC_ADDR],
                              uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 tpid,
                              uint16 vlan_id, uint32 seq_id);
extern void calc_oversub_ratio(int unit, uint32 pkt_size);
extern void turn_off_cmic_mmu_bkp(int unit);
extern void turn_off_fc(int unit, pbmp_t pbmp);
extern void set_port_property_arrays(int unit, pbmp_t pbmp, int pkt_size_param,
                                     int max_num_cells_param);
extern pbmp_t set_up_streams(int unit, pbmp_t pbmp, pbmp_t vlan_pbmp);
extern bcm_error_t soc_set_lpbk(int unit, pbmp_t pbmp, int loopback);
extern void send_pkts(int unit, pbmp_t pbmp, uint32 pkt_size_param,
                      uint32 flood_cnt_param, uint32 pkt_seed);
extern void start_rate_measurement(int unit, pbmp_t pbmp);
extern void set_up_ports(int unit, pbmp_t pbmp);
extern void set_exp_rates(int unit);
extern bcm_error_t check_mib_counters(int unit, pbmp_t pbmp);
extern bcm_error_t check_rates(int unit, pbmp_t pbmp, uint32 tolerance_lr,
                               uint32 tolerance_os);
extern bcm_error_t check_packet_integrity(int unit, pbmp_t pbmp,
                                          uint32 pkt_size_param,
                                          uint32 flood_cnt_param,
                                          uint32 pkt_seed);

#endif /* BCM_ESW_SUPPORT */
