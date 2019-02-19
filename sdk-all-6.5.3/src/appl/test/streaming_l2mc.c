/*
 * $Id: l2mc.c,v 1.0 Broadcom SDK $
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
 * The goal of this test is to check basic multicast and broadcast features of
 * the chip.
 * Test Sequence:
 * 1. Group all active front panel ports into groups of (NumCopy + 1) ports.
 *    Each group constitutes 1 multicast "stream". All ports in a stream should
 *    either be Ethernet or HG2 and have the same line rate, although they may
 *    have different oversub ratios.
 * 2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be
 *    arranged such that the P1 is the slowest port, i.e. has the highest
 *    oversub ratio.
 * 3. Turn off all flow control, set MAC loopback on all ports and set
 *    IGNORE_MY_MODID to 1 on ING_CONFIG_64.
 * 4. For Ethernet streams:
 *      a.  Place all ports in the stream on a single VLAN, say VLAN0.
 *      b.  Place ports P0 and P1 on a second VLAN, say VLAN1.
 *      c.  Create a 3rd VLAN, say VLAN3 with no ports on it.
 *      d.  Add the following VLAN translations:
 *          i.  Port=P0 OldVlan=VLAN1 NewVlan=VLAN0
 *          ii. Port=P1 OldVlan=VLAN0 NewVlan=VLAN1
 *          iii.    Ports=P2, P3 ... Pn OldVlan=VLAN0 NewVlan=VLAN2
 * 5. For HG2 streams:
 *      a.  Place port P0 on VLAN0.
 *      b.  Place ports P0, P1 ... Pn on VLAN1.
 *      c.  Place all ports in the stream except P0, i.e. P1, P2 ... Pn on VLAN2
 *      d.  Create 3 entries, say L2MC0, L2MC1 and L2MC2.
 *          i.  L2MC0: PORT_BITMAP = P0
 *          ii. L2MC1: PORT_BITMAP = P0
 *          iii.    L2MC2: PORT_BITMAP = P1, P2 ... Pn
 *      e.  Add the following IFP entries:
 *          i.  Qualifier: OuterVlan=VLAN0, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC0
 *          ii. Qualifier: OuterVlan=VLAN2, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC1
 *          iii.Qualifier: OuterVlan=VLAN1, Actions: OuterVlanNew=VLAN2, RedirectMcast=L2MC2
 * 6. Send packets from CPU port on VLAN0 of each stream to flood the stream.
 *    If FloodCnt=0, the test will determine the number of packets to send based.
 *    These values are obtained empirically through trial and error for LR ports
 *    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.
 *    Embed the packet size and random seed used to generate each packet in the
 *    payload.
 * 7. Allow the packets to swill for a fixed amount of time decided by RateCalcInt.
 * 8. Snoop back all packets in each stream to the CPU using the following sequence:
 *      a.  Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an
 *          unknown VLAN to CPU.
 *      b.  Invalidate VLAN0 for Ethernet Streams and VLAN1 for HG2 streams by
 *          setting VALID=0 in the VLAN table
 * 9. Decode each received packet to obtain the random seed and packet size.
 *    Reconstruct the expected packet and check the received packet for integrity.
 *
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swill. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * NumCopy: Number of multicast copies in each stream (2 by default).
 * RateCalcInt: Interval in seconds over which rate is to be calculated
 * TolLr: Rate tolerance percentage for linerate ports (1% by default).
 * TolOv: Rate tolerance percentage for oversubscribed ports (3% by default).
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set
 *              to 0 for random.
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>

#include "testlist.h"
#include "gen_pkt.h"

#define MAC_DA {0x01,0x00,0x0c,0xde,0xff,0x00}
#define MAC_SA {0xfe,0xdc,0xba,0x98,0x76,0x54}
#define TPID 0x8100
#define VLAN 0x100
#define MIN_PKT_SIZE 64
#define MTU 1518
#define MTU_CELL_CNT 45

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define NUM_COPY_PARAM_DEFAULT 2
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define NUM_LPORT_TAB_ENTRIES 256
#define NUM_PORT_TAB_ENTRIES 136
#define CPU_PORT 0
#define MISC_BW 10000
#define NUM_SUBP_OBM 4
#define TARGET_CELL_COUNT 80
#define MAX_FP_PORTS 130
#define MAX_NUM_CELLS_PARAM_DEFAULT 4

#define TX_CHAN 0
#define RX_CHAN 1

#define HG2_WC_PKT_SIZE 64
#define ENET_WC_PKT_SIZE 145
#define ENET_IPG 12
#define ENET_PREAMBLE 8
#define HG2_IPG 8
#define HG2_HDR 12

#define CELL_SIZE 208
#define FIRST_CELL_ENET 144
#define FIRST_CELL_HG2 148
#define RXDMA_TIMEOUT 1000

#define NUM_BYTES_MAC_ADDR 6
#define NUM_BYTES_CRC 4

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

#define FLOOD_CNT_25G 9
#define FLOOD_CNT_40G 21
#define FLOOD_CNT_50G 28
#define FLOOD_CNT_100G 50

#define MAX_PKTS_PER_STREAM 500

#if defined(BCM_ESW_SUPPORT)

typedef struct l2mc_s {

    uint32 num_fp_ports;
    uint32 num_streams;
    int *port_speed;
    int *port_list;
    int **stream;
    uint32 *tx_vlan;
    int *hg_stream;
    uint32 *port_oversub;
    uint32 oversub_config;
    uint32 *port_used;
    uint32 num_pipes;
    uint32 total_chip_bw;
    uint32 bw_per_pipe;
    uint32 *ovs_ratio_x1000;
    uint64 *rate;
    uint64 *exp_rate;
    uint64 *tpkt_start;
    uint64 *tpkt_end;
    uint64 *tbyt_start;
    uint64 *tbyt_end;
    uint32 **rand_pkt_sizes;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 num_copy_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
} l2mc_t;

static l2mc_t *l2mc_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      l2mc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      l2mc_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */

static void
l2mc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    char tr511_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "The goal of this test is to check basic multicast and broadcast features of\n"
    "the chip.\n"
    "Test Sequence:\n"
    "1. Group all active front panel ports into groups of (NumCopy + 1) ports.\n"
    "    Each group constitutes 1 multicast \"stream\". All ports in a stream should\n"
    "    either be Ethernet or HG2 and have the same line rate, although they may\n"
    "    have different oversub ratios.\n"
    "2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be\n"
    "    arranged such that the P1 is the slowest port, i.e. has the highest\n"
    "    oversub ratio.\n"
    "3. Turn off all flow control, set MAC loopback on all ports and set\n"
    "    IGNORE_MY_MODID to 1 on ING_CONFIG_64.\n"
    "4. For Ethernet streams:\n"
    "    a.  Place all ports in the stream on a single VLAN, say VLAN0.\n"
    "    b.  Place ports P0 and P1 on a second VLAN, say VLAN1.\n"
    "    c.  Create a 3rd VLAN, say VLAN3 with no ports on it.\n"
    "    d.  Add the following VLAN translations:\n"
    "        i.  Port=P0 OldVlan=VLAN1 NewVlan=VLAN0\n"
    "        ii. Port=P1 OldVlan=VLAN0 NewVlan=VLAN1\n"
    "        iii.    Ports=P2, P3 ... Pn OldVlan=VLAN0 NewVlan=VLAN2\n"
    "5. For HG2 streams:\n"
    "    a.  Place port P0 on VLAN0.\n"
    "    b.  Place ports P0, P1 ... Pn on VLAN1.\n"
    "    c.  Place all ports in the stream except P0, i.e. P1, P2 ... Pn on VLAN2\n"
    "    d.  Create 3 entries, say L2MC0, L2MC1 and L2MC2.\n"
    "        i.  L2MC0: PORT_BITMAP = P0\n"
    "        ii. L2MC1: PORT_BITMAP = P0\n"
    "        iii.    L2MC2: PORT_BITMAP = P1, P2 ... Pn\n"
    "    e.  Add the following IFP entries:\n"
    "        i.  Qualifier: OuterVlan=VLAN0, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC0\n"
    "        ii. Qualifier: OuterVlan=VLAN2, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC1\n"
    "        iii.    Qualifier: OuterVlan=VLAN1, Actions: OuterVlanNew=VLAN2, RedirectMcast=L2MC2\n"
    "6. Send packets from CPU port on VLAN0 of each stream to flood the stream.\n"
    "    If FloodCnt=0, the test will determine the number of packets to send based.\n"
    "    These values are obtained empirically through trial and error for LR ports\n"
    "    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.\n"
    "    Embed the packet size and random seed used to generate each packet in the\n"
    "    payload.\n"
    "7. Allow the packets to swill for a fixed amount of time decided by RateCalcInt.\n"
    "8. Snoop back all packets in each stream to the CPU using the following sequence:\n"
    "    a.  Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an\n"
    "        unknown VLAN to CPU.\n"
    "    b.  Invalidate VLAN0 for Ethernet Streams and VLAN1 for HG2 streams by\n"
    "        setting VALID=0 in the VLAN table\n"
    "9. Decode each received packet to obtain the random seed and packet size.\n"
    "    Reconstruct the expected packet and check the received packet for integrity.\n"
    "\n"
    "Configuration parameters passed from CLI:\n"
    "PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all\n"
    "         ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes\n"
    "FloodCnt: Number of packets in each swill. Setting this to 0 will let the\n"
    "          test calculate the number of packets that can be sent to achieve\n"
    "          a lossless swirl at full rate. Set to 0 by default.\n"
    "NumCopy: Number of multicast copies in each stream (2 by default).\n"
    "RateCalcInt: Interval in seconds over which rate is to be calculated\n"
    "TolLr: Rate tolerance percentage for linerate ports (1% by default).\n"
    "TolOv: Rate tolerance percentage for oversubscribed ports (3% by default).\n"
    "ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).\n"
    "MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set\n"
    "             to 0 for random.\n";
#endif

    l2mc_p->bad_input = 0;

    l2mc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    l2mc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    l2mc_p->num_copy_param = NUM_COPY_PARAM_DEFAULT;
    l2mc_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    l2mc_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    l2mc_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    l2mc_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    l2mc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "NumCopy", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->num_copy_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->max_num_cells_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg(tr511_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        l2mc_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
        cli_out("\npkt_size_param = %0d", l2mc_p->pkt_size_param);
        cli_out("\nflood_pkt_cnt_param = %0d", l2mc_p->flood_pkt_cnt_param);
        cli_out("\nnum_copy_param = %0d", l2mc_p->num_copy_param);
        cli_out("\nrate_calc_interval_param = %0d",
                l2mc_p->rate_calc_interval_param);
        cli_out("\nrate_tolerance_lr_param = %0d",
                l2mc_p->rate_tolerance_lr_param);
        cli_out("\nrate_tolerance_ov_param = %0d",
                l2mc_p->rate_tolerance_ov_param);
        cli_out("\ncheck_packet_integrity_param = %0d",
                l2mc_p->check_packet_integrity_param);
        cli_out("\nmax_num_cells_param = %0d", l2mc_p->max_num_cells_param);
        cli_out("\n -----------------------------------------------------");
    }

    if (l2mc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        l2mc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (l2mc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (l2mc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (l2mc_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        l2mc_p->bad_input = 1;
    } else if (l2mc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Test Limitation)\n",
                MTU);
        l2mc_p->bad_input = 1;
    }

    if (l2mc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}

/*
 * Function:
 *      l2mc_soc_set_up_mac_lpbk
 * Purpose:
 *      Enable MAC loopback on all ports
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
l2mc_soc_set_up_mac_lpbk(int unit)
{
    int p;

    cli_out("\nSetting up MAC loopbacks");
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_MAC);
    }
}

/*
 * Function:
 *      l2mc_turn_off_cmic_mmu_bkp
 * Purpose:
 *      Turn off CMIC to MMU backpressure
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
l2mc_turn_off_cmic_mmu_bkp(int unit)
{
    int cmc, ch;

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            soc_pci_write(unit, CMIC_CMCx_CHy_RXBUF_THRESHOLD_CONFIG(cmc, ch),
                          0x20);
        }
    }
}

/*
 * Function:
 *      l2mc_soc_turn_off_fc
 * Purpose:
 *      Turn off flow control at the MAC, IDB and MMU.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
l2mc_soc_turn_off_fc(int unit)
{
    int p;
    int obm_subp;
    int idx;
    uint64 idb_fc;

    soc_reg_t idb_fcc_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE3r
    };

    soc_reg_t pgw_fcc_regs[] = {
        PGW_OBM_PORT0_FC_CONFIGr,
        PGW_OBM_PORT1_FC_CONFIGr,
        PGW_OBM_PORT2_FC_CONFIGr,
        PGW_OBM_PORT3_FC_CONFIGr,
        PGW_OBM_PORT4_FC_CONFIGr,
        PGW_OBM_PORT5_FC_CONFIGr,
        PGW_OBM_PORT6_FC_CONFIGr,
        PGW_OBM_PORT7_FC_CONFIGr,
        PGW_OBM_PORT8_FC_CONFIGr,
        PGW_OBM_PORT9_FC_CONFIGr,
        PGW_OBM_PORT10_FC_CONFIGr,
        PGW_OBM_PORT11_FC_CONFIGr,
        PGW_OBM_PORT12_FC_CONFIGr,
        PGW_OBM_PORT13_FC_CONFIGr,
        PGW_OBM_PORT14_FC_CONFIGr,
        PGW_OBM_PORT15_FC_CONFIGr
    };

    soc_field_t idb_flow_control_config_fields[] =
        { PORT_FC_ENf, LOSSLESS1_FC_ENf, LOSSLESS0_FC_ENf,
        LOSSLESS1_PRIORITY_PROFILEf, LOSSLESS0_PRIORITY_PROFILEf
    };
    uint32 idb_flow_control_config_values[] = { 0x0, 0x0, 0x0, 0xff, 0xff };

    cli_out("\nTurning off flow control at IDB/MMU");

    COMPILER_64_SET(idb_fc, 0x0, 0x0);

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        bcm_port_pause_set(unit, p, FALSE, FALSE);
        soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr, p,
                               PORT_PAUSE_ENABLEf, 0x0);
    }
    soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr, 0,
                           PORT_PAUSE_ENABLEf, 0x0);

    if (SOC_REG_IS_VALID(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r)) {
        soc_reg_fields32_modify(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
                                REG_PORT_ANY, 5, idb_flow_control_config_fields,
                                idb_flow_control_config_values);
        (void) soc_reg_get(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
                           0, 0, &idb_fc);
    }

    for (obm_subp = 0; obm_subp < NUM_SUBP_OBM; obm_subp++) {
        for(idx = 0; idx < (sizeof(idb_fcc_regs) / sizeof(soc_reg_t)); idx++) {
            if(SOC_REG_IS_VALID(unit, idb_fcc_regs[idx])) {
                (void) soc_reg_set(unit, idb_fcc_regs[idx], 0, obm_subp, idb_fc);
            }
        }
    }

    for (idx = 0; idx < (sizeof(pgw_fcc_regs) / sizeof(soc_reg_t)); idx++) {
        if (SOC_REG_IS_VALID(unit, pgw_fcc_regs[idx])) {
            soc_reg_field32_modify(unit, pgw_fcc_regs[idx], REG_PORT_ANY,
                               PORT_FC_ENABLEf, 0x0);
        }
    }
}

/*
 * Function:
 *      get_pipe
 * Purpose:
 *      Get pipe number for a given device port.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port #
 *
 * Returns:
 *     Pipe number.
 */

static int
get_pipe(int unit, int port)
{
    int pipe;
    soc_info_t *si = &SOC_INFO(unit);

    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        if (SOC_PBMP_MEMBER(si->pipe_pbm[pipe], port)) {
            break;
        }
    }
    return pipe;
}

/*
 * Function:
 *      num_cells
 * Purpose:
 *      Get the cell count for a given packet size.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port #
 *
 * Returns:
 *     Cell count
 */

static uint32
num_cells(int unit, uint32 pkt_size, int port)
{
    uint32 num_cells = 1;
    uint32 i;

    if (IS_HG_PORT(unit, port)) {
        i = FIRST_CELL_HG2;
    }
    else {
        i = FIRST_CELL_ENET;
    }
    while (i < pkt_size) {
        num_cells++;
        i += CELL_SIZE;
    }
    return num_cells;
}

/*
 * Function:
 *      safe_pkt_size
 * Purpose:
 *      Calculate safe packet size based on oversub ratio. The assumption is that
 *      a front panel port will behave essentially as a line rate port and show no
 *      bandwidth degradation for packet sizes above the safe packet size.
 *      This is based on input from device microarchitects. Broadcom internal
 *      users of this test can contact microarchitecture for more info.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      ovs_ratio_x1000: Oversub ratio x1000
 *
 * Returns:
 *     Safe packet size.
 */

static uint32
safe_pkt_size(int unit, int ovs_ratio_x1000)
{
    uint32 safe_size = 64;

    if (ovs_ratio_x1000 <= 1507) {
        safe_size =
            145 +
            ((((229000 - 144000) / (1507 - 1000)) * (ovs_ratio_x1000 -
                                                     1000)) / 1000);
    } else if ((ovs_ratio_x1000 > 1507) && (ovs_ratio_x1000 <= 1760)) {
        safe_size =
            353 +
            ((((416000 - 353000) / (1760 - 1508)) * (ovs_ratio_x1000 -
                                                     1508)) / 1000);
    } else if ((ovs_ratio_x1000 > 1760) && (ovs_ratio_x1000 <= 1912)) {
        safe_size =
            562 +
            ((((611000 - 562000) / (1912 - 1760)) * (ovs_ratio_x1000 -
                                                     1760)) / 1000);
    } else {
        safe_size = 770;
    }

    return safe_size;

}

/*
 * Function:
 *      calc_oversub_ratio
 * Purpose:
 *      Calculate oversub ratio for each pipe based on port config
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *    Nothing
 */


static void
calc_oversub_ratio(int unit)
{
    int i;
    int pipe;
    int pipe_bandwidth;
    int lr_bandwidth = 0;
    int ov_bandwidth = 0;
    uint16 dev_id;
    uint8 rev_id;
    soc_info_t *si = &SOC_INFO(unit);
    l2mc_t *l2mc_p = l2mc_parray[unit];

    l2mc_p->ovs_ratio_x1000 =
        (uint32 *) sal_alloc(si->num_pipe * sizeof(uint32), "ovs_ratio");

    cli_out("\nCalculating oversub ratios");
    cli_out("\nOperating Freq = %0d", si->frequency);

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56960_DEVICE_ID) {
        switch (si->frequency) {
        case TH_FREQ_850:
            pipe_bandwidth = TH_BW_850;
            break;
        case TH_FREQ_765:
            pipe_bandwidth = TH_BW_765;
            break;
        case TH_FREQ_672:
            pipe_bandwidth = TH_BW_672;
            break;
        case TH_FREQ_645:
            pipe_bandwidth = TH_BW_645;
            break;
        case TH_FREQ_545:
            pipe_bandwidth = TH_BW_545;
            break;
        default:
            pipe_bandwidth = TH_BW_850;
        }
    }
    else if (dev_id == BCM56860_DEVICE_ID || dev_id == BCM56850_DEVICE_ID) {
        switch (si->frequency) {
        case TD2P_FREQ_793:
            pipe_bandwidth = TD2P_BW_793;
            break;
        case TD2P_FREQ_635:
            pipe_bandwidth = TD2P_BW_635;
            break;
        case TD2P_FREQ_529:
            pipe_bandwidth = TD2P_BW_529;
            break;
        case TD2P_FREQ_421:
            pipe_bandwidth = TD2P_BW_421;
            break;
        default:
            pipe_bandwidth = TD2P_BW_793;
        }
    }
    else {
        pipe_bandwidth = TD2P_BW_793;
    }

    pipe_bandwidth = pipe_bandwidth - MISC_BW;

    cli_out("\nPer pipe BW = %0d", pipe_bandwidth);

    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        lr_bandwidth = 0;
        ov_bandwidth = 0;
        for (i = 0; i < l2mc_p->num_fp_ports; i++) {
            if (SOC_PBMP_MEMBER(si->pipe_pbm[pipe], l2mc_p->port_list[i])) {
                if (l2mc_p->port_oversub[i] == 1) {
                        ov_bandwidth += l2mc_p->port_speed[i];
                } else {
                    lr_bandwidth += l2mc_p->port_speed[i];
                }
            }
        }

        l2mc_p->ovs_ratio_x1000[pipe] =
            (ov_bandwidth) / ((pipe_bandwidth - lr_bandwidth) / 1000);

        if (l2mc_p->pkt_size_param >
            safe_pkt_size(unit, l2mc_p->ovs_ratio_x1000[pipe])) {
            l2mc_p->ovs_ratio_x1000[pipe] = 1000;
        }

        if (l2mc_p->ovs_ratio_x1000[pipe] < 1000) {
            l2mc_p->ovs_ratio_x1000[pipe] = 1000;
        }

        cli_out
            ("\nFor pipe %0d, LR BW = %0d, OV BW = %0d, ovs_ratio(x1000) = %0d",
             pipe, lr_bandwidth, ov_bandwidth,
             l2mc_p->ovs_ratio_x1000[pipe]);
    }
}

/*
 * Function:
 *      set_exp_rates
 * Purpose:
 *      Set expected rates array (l2mc_p->exp_rate). This is based on port
 *      speed and oversub ratio (for oversubscribed ports). The function also
 *      ensures that l2mc_p->stream[][1] is assigned the slowest port, i.e. the
 *      rate in each swill is limited by the most oversubscribed port in the
 *      swill.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


static void
set_exp_rates(int unit)
{
    int i, j;
    int temp;
    char exp_rate_str[32];
    uint64 ovs_ratio_x1000_64;
    uint64 lowest_rate_stream;
    uint32 lowest_rate_port_stream;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    cli_out("\nSetting expected rates");
    l2mc_p->exp_rate =
        (uint64 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint64),
                             "exp_rate");

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (l2mc_p->port_oversub[i] == 1) {
            COMPILER_64_SET(l2mc_p->exp_rate[i], 0,
                            l2mc_p->port_speed[i]);
            COMPILER_64_UMUL_32(l2mc_p->exp_rate[i], 1000000);
            COMPILER_64_SET(ovs_ratio_x1000_64, 0,
                            l2mc_p->ovs_ratio_x1000[get_pipe(unit,
                                                        l2mc_p->
                                                            port_list[i])]);
            COMPILER_64_UMUL_32(l2mc_p->exp_rate[i], 1000);
            COMPILER_64_UDIV_64(l2mc_p->exp_rate[i], ovs_ratio_x1000_64);
        } else {
            COMPILER_64_SET(l2mc_p->exp_rate[i], 0,
                            l2mc_p->port_speed[i]);
            COMPILER_64_UMUL_32(l2mc_p->exp_rate[i], 1000000);
        }
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (l2mc_p->port_used[i] == 0) {
            COMPILER_64_ZERO(l2mc_p->exp_rate[i]);
        }
    }
 
    for (i = 0; i < l2mc_p->num_streams; i++) {
        COMPILER_64_SET(lowest_rate_stream,
                        COMPILER_64_HI(l2mc_p->exp_rate[l2mc_p->stream[i][0]]),
                        COMPILER_64_LO(l2mc_p->exp_rate[l2mc_p->stream[i][0]]));
        lowest_rate_port_stream = 0;

        for (j = 0; j < (l2mc_p->num_copy_param + 1); j++) {
            if (COMPILER_64_GT(lowest_rate_stream,
                               l2mc_p->exp_rate[l2mc_p->stream[i][j]])) {
                lowest_rate_port_stream = j;
                COMPILER_64_SET(lowest_rate_stream,
                                COMPILER_64_HI(l2mc_p->
                                                exp_rate[l2mc_p->stream[i][j]]),
                                COMPILER_64_LO(l2mc_p->
                                                exp_rate[l2mc_p->stream[i][j]]));
            }
        }

        for (j = 0; j < (l2mc_p->num_copy_param + 1); j++) {
            COMPILER_64_SET(l2mc_p->exp_rate[l2mc_p->stream[i][j]],
                            COMPILER_64_HI(lowest_rate_stream),
                            COMPILER_64_LO(lowest_rate_stream));
        }

        if (lowest_rate_port_stream != 1) {
            temp = l2mc_p->stream[i][1];
             l2mc_p->stream[i][1] =  l2mc_p->stream[i][lowest_rate_port_stream];
             l2mc_p->stream[i][lowest_rate_port_stream] = temp;
        }
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nPRINTING EXPECTED RATES")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n=======================")));
    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        format_uint64_decimal(exp_rate_str, l2mc_p->exp_rate[i], 0);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, exp_rate[%0d] = %s"), i,
                  i, exp_rate_str));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n=======================")));
}

/*
 * Function:
 *      set_port_property_arrays
 * Purpose:
 *      Set port_list, port_speed and port_oversub arrays. Also call *      *      set_exp_rates
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */

static void
set_port_property_arrays(int unit)
{
    int p;
    int picked_port;
    int i, j, k;
    uint32 pkt_size;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 stream_possible = 0;
    uint32 ports_found = 0;
    uint32 ports_picked = 0;
    uint32 ports_accounted_for = 0;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    l2mc_p->num_fp_ports = 0;
    l2mc_p->num_streams = 0;
    l2mc_p->oversub_config = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        l2mc_p->num_fp_ports++;
    }

    l2mc_p->port_list =
        (int *)sal_alloc(l2mc_p->num_fp_ports * sizeof(int),
                         "port_list");
    l2mc_p->port_speed =
        (int *)sal_alloc(l2mc_p->num_fp_ports * sizeof(int),
                         "port_speed_array");
    l2mc_p->hg_stream =
        (int *)sal_alloc(l2mc_p->num_fp_ports * sizeof(int),
                         "hg_stream_array");
    l2mc_p->port_oversub =
        (uint32 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint32),
                             "port_oversub_array");
    l2mc_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->rand_pkt_sizes[i] =
            (uint32 *) sal_alloc(TARGET_CELL_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }
    l2mc_p->stream =
        (int **) sal_alloc(l2mc_p->num_fp_ports * sizeof(int *),
                              "stream_array*");

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->stream[i] =
            (int *) sal_alloc((l2mc_p->num_copy_param + 1) * sizeof(int),
                                "stream_array");
    }
    l2mc_p->port_used =
        (uint32 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint32),
                             "l2mc_p->port_used_array");

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        l2mc_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->port_speed[i] =
            si->port_speed_max[l2mc_p->port_list[i]];

        switch(l2mc_p->port_speed[i]) {
            case 11000:
                l2mc_p->port_speed[i] = 10600;
            break;
            case 27000:
                l2mc_p->port_speed[i] = 26500;
            break;
            case 42000:
                l2mc_p->port_speed[i] = 42400;
            break;
        }
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, l2mc_p->port_list[i])
            &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, l2mc_p->port_list[i])))) {
            l2mc_p->oversub_config = 1;
            l2mc_p->port_oversub[i] = 1;
        } else {
            l2mc_p->port_oversub[i] = 0;
        }
    }

    cli_out("\n=========== STREAM INFO ============");
    cli_out("\nStream\tPorts");

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->port_used[i] = 0;
        l2mc_p->hg_stream[i] = 0;
    }

    while (ports_accounted_for < l2mc_p->num_fp_ports) {
        do {
            i = sal_rand() % l2mc_p->num_fp_ports;
        }while (l2mc_p->port_used[i] != 0);
        stream_possible = 0;
        ports_found = 0;
        for (j = 0;j < l2mc_p->num_fp_ports; j++) {
            if((l2mc_p->port_used[j] == 0)
                    && (l2mc_p->port_speed[i]
                                == l2mc_p->port_speed[j])
                    && (i!=j)) {
                ports_found++;
            }

            if (ports_found >= l2mc_p->num_copy_param) {
                stream_possible = 1;
            }
        }

        ports_accounted_for++;

        if(stream_possible == 1) {
            l2mc_p->port_used[i] = 1;
            ports_picked = 0;
            l2mc_p->stream[l2mc_p->num_streams][0] = i;
            if (IS_HG_PORT(unit, l2mc_p->port_list[i])) {
                l2mc_p->hg_stream[l2mc_p->num_streams] = 1;
            }
            while (ports_picked < l2mc_p->num_copy_param) {
                do {
                    picked_port = sal_rand() % l2mc_p->num_fp_ports;
                }while((l2mc_p->port_speed[i] !=
                                l2mc_p->port_speed[picked_port])
                        || (l2mc_p->port_used[picked_port] != 0));

                ports_picked++;
                l2mc_p->port_used[picked_port] = 1;
                l2mc_p->stream[l2mc_p->num_streams][ports_picked]
                                                            = picked_port;
                ports_accounted_for++;
            }
            cli_out("\n%0d", l2mc_p->num_streams);
            cli_out("\t");
            for (k = 0; k < (l2mc_p->num_copy_param + 1); k++) {
                cli_out("%0d", l2mc_p->stream[l2mc_p->num_streams][k]);
                if (k != l2mc_p->num_copy_param) {
                    cli_out(", ");
                }
            }
            l2mc_p->num_streams++;
        }
    }

    calc_oversub_ratio(unit);

    cli_out("\n====================================");

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nPRINTING PORT SPEED")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================")));

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, port_speed[%0d] = %0d"), i,
                  i, l2mc_p->port_speed[i]));
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================\n")));

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nPRINTING PORT OVERSUB ARRAY")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================")));

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, port_oversub[%0d] = %0d"),
                  i, i, l2mc_p->port_oversub[i]));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================\n")));

    set_exp_rates(unit);

    cli_out("\n");

    for (i = 0; i < l2mc_p->num_streams; i++) {
        for (j = 0; j < (l2mc_p->num_copy_param + 1); j++) {
            cli_out("stream[%0d][%0d] = %0d", i, j, l2mc_p->stream[i][j]);
            if (j < (l2mc_p->num_copy_param + 1)) {
                cli_out(",");
            }
        }
        cli_out("\n");
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        for (j = 0; j < TARGET_CELL_COUNT; j++) {
            do {
                /* coverity[dont_call : FALSE] */
                pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
            } while (num_cells(unit, pkt_size, l2mc_p->port_list[i]) >
                            l2mc_p->max_num_cells_param);
            l2mc_p->rand_pkt_sizes[i][j] = pkt_size;
        }
    }
}

/*
 * Function:
 *      fp_chg_vlan_and_redirect
 * Purpose:
 *      Creates entry pointed to by l2mc_index in the L2MC table with
 *      PORT_BITMAP=redirect_pbmp
 *      Create the following FP actions:
 *          Qualifier: OuterVlan=old_vlan
 *          Actions: OuterVlanNew=VLAN1, RedirectMcast=l2mc_index
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      old_vlan : Old VLAN of incoming packet
 *      new_vlan: New VLAN after IFP modifies packet
 *      fg : Pointer to Field group for IFP entry
 *      l2mc_index: Index in L2MC table where entry is to be created
 *      redirect_pbmp: PORT_BITMAP field of L2MC entry created
 * Returns:
 *     Nothing
 */

static void
fp_change_vlan_redirect(int unit, bcm_vlan_t old_vlan, bcm_vlan_t new_vlan,
                        bcm_field_group_t *fg, uint32 l2mc_index,
                        soc_pbmp_t *redirect_pbmp) {
    bcm_field_entry_t fe;
    l2mc_entry_t l2mc_entry;

    soc_mem_read(unit, L2MCm, COPYNO_ALL, l2mc_index, l2mc_entry.entry_data);
    soc_mem_field32_set(unit, L2MCm, l2mc_entry.entry_data,
                        VALIDf, 0x1);
    soc_mem_pbmp_field_set(unit, L2MCm, l2mc_entry.entry_data,
                           PORT_BITMAPf, redirect_pbmp);
    soc_mem_write(unit, L2MCm, COPYNO_ALL, l2mc_index, l2mc_entry.entry_data);

    (void) bcm_field_entry_create(unit, *fg, &fe);
    bcm_field_qualify_OuterVlan(unit, fe, old_vlan, (bcm_vlan_t)(0xffff));
    bcm_field_action_add(unit, fe, bcmFieldActionOuterVlanNew,
                                                (uint32)(new_vlan), 0);
    bcm_field_action_add(unit, fe, bcmFieldActionRedirectMcast, l2mc_index, 0);
    (void) bcm_field_entry_install(unit, fe);
}

/*
 * Function:
 *      set_up_streams
 * Purpose:
 *      This function actually sets up the L2MC packet flow. Please refer to
 *      items 3-6 of the "Test Setup" section of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */

static void
set_up_streams(int unit)
{
    int i, j;
    pbmp_t pbm, ubm;
    soc_pbmp_t redirect_pbmp;
    uint32 vlan = VLAN;
    uint32 l2mc_index = 1;
    uint32 vlan_0;
    uint32 vlan_1;
    bcm_field_group_t fg;
    bcm_field_qset_t qset;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    l2mc_p->tx_vlan = (uint32*) sal_alloc(l2mc_p->num_streams * sizeof(uint32),
                                          "tx_vlan");
    BCM_PBMP_CLEAR(ubm);

    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlan);
    (void) bcm_field_group_create(unit, qset, l2mc_index, &fg);

    for (i = 0; i < l2mc_p->num_streams; i++) {
        if (l2mc_p->hg_stream[i] == 0) {
            /*Ethernet Stream*/
            BCM_PBMP_CLEAR(pbm);
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            for (j = 0; j < l2mc_p->num_copy_param + 1; j++) {
                BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_0 = vlan;
            vlan++;
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            BCM_PBMP_CLEAR(pbm);
            BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][0]]);
            BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][1]]);
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_1 = vlan;
            vlan++;
            bcm_vlan_translate_add(unit, l2mc_p->port_list[l2mc_p->stream[i][0]],
                                   vlan_1, vlan_0, 0);
            bcm_vlan_translate_add(unit, l2mc_p->port_list[l2mc_p->stream[i][1]],
                                   vlan_0, vlan_1, 0);

            for (j = 2; j < (l2mc_p->num_copy_param + 1); j++) {
                bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
                bcm_vlan_translate_add(unit,
                                       l2mc_p->port_list[l2mc_p->stream[i][j]],
                                       vlan_0, vlan, 0);
                vlan++;
            }
            l2mc_p->tx_vlan[i] = vlan_0;
        } else {
            /*HG2 Stream*/
            BCM_PBMP_CLEAR(pbm);
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][0]]);
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_0 = vlan;
            vlan++;
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            BCM_PBMP_CLEAR(pbm);
            for (j = 0; j < l2mc_p->num_copy_param + 1; j++) {
                BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_1 = vlan;
            vlan++;

            SOC_PBMP_CLEAR(redirect_pbmp);
            SOC_PBMP_PORT_ADD(redirect_pbmp,
                              l2mc_p->port_list[l2mc_p->stream[i][0]]);
            fp_change_vlan_redirect(unit, vlan_0, vlan_1, &fg,
                                    l2mc_index, &redirect_pbmp);
            l2mc_index++;

            BCM_PBMP_CLEAR(pbm);
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            for (j = 1; j < l2mc_p->num_copy_param + 1; j++) {
                BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            fp_change_vlan_redirect(unit, vlan, vlan_1, &fg,
                                    l2mc_index, &redirect_pbmp);
            l2mc_index++;
            SOC_PBMP_CLEAR(redirect_pbmp);
            for (j = 1; j < l2mc_p->num_copy_param + 1; j++) {
                SOC_PBMP_PORT_ADD(redirect_pbmp,
                                  l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            fp_change_vlan_redirect(unit, vlan_1, vlan, &fg,
                                    l2mc_index, &redirect_pbmp);
            l2mc_index++;
            vlan++;
            l2mc_p->tx_vlan[i] = vlan_0;
        }
    }
}

/*
 * Function:
 *      l2mc_dma_chan_check_done
 * Purpose:
 *      Check chain done or desc done bit in DMA status register.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vchan - Virtual channel number
 *      type -  check desc done or chain done
 *      detected - whether desc done or chain done was detected
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

static int
l2mc_dma_chan_check_done(int unit, int vchan, soc_dma_poll_type_t type,
                              int *detected)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    switch (type) {
    case SOC_DMA_POLL_DESC_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_DESC_DONE(chan));
        break;
    case SOC_DMA_POLL_CHAIN_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_CHAIN_DONE(chan));
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *      lossless_flood_cnt
 * Purpose:
 *      Calculates number of packets that need to be sent to a port for a
 *      lossless swirl.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_size : Packet size in bytes
 *      port: Test port no
 * Returns:
 *     Number of packets needed for lossless flooding
 *
 */

static uint32
lossless_flood_cnt(int unit, uint32 pkt_size, int port)
{
    uint32 flood_cnt = 0;
    uint32 ovs_ratio_x1000;

    l2mc_t *l2mc_p = l2mc_parray[unit];

    ovs_ratio_x1000 = l2mc_p->ovs_ratio_x1000[get_pipe(unit,
                                                       l2mc_p->port_list[port])];

    switch (l2mc_p->port_speed[port]) {
        case 10000:
        case 10600:
        case 25000:
            flood_cnt = 14;
            break;
        case 26500:
            flood_cnt = 25;
            break;
        case 40000:
            flood_cnt = 20;
            break;
        case 42400:
            flood_cnt = 38;
            break;
        case 50000:
            flood_cnt = 24;
            break;
        case 53000:
            flood_cnt = 45;
            break;
        case 100000:
            flood_cnt = 50;
            break;
        case 106000:
            flood_cnt = 94;
            break;
    }

    flood_cnt = (flood_cnt * 1000) / ovs_ratio_x1000;
    if (flood_cnt < 2) {
        flood_cnt = 2;
    }

    return(flood_cnt);
}

/*
 * Function:
 *      l2mc_gen_random_l2_pkt
 * Purpose:
 *      Generate random L2 packet with seq ID
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

void
l2mc_gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size,
                  uint8 mac_da[NUM_BYTES_MAC_ADDR],
                  uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 tpid,
                  uint16 vlan_id, uint32 seq_id)
{
    uint32 crc;
    tgp_gen_random_l2_pkt(pkt_ptr, pkt_size, mac_da, mac_sa, tpid, vlan_id);
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 6] = (seq_id >> 24) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 7] = (seq_id >> 16) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 8] = (seq_id >> 8) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 9] = (seq_id) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 10] = (pkt_size >> 8) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 11] = (pkt_size) & 0xff;

    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    pkt_ptr[pkt_size - NUM_BYTES_CRC + 3] = (crc >> 24) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 2] = (crc >> 16) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 1] = (crc >> 8) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC] = (crc) & 0xff;
}


/*
 * Function:
 *      send_pkts
 * Purpose:
 *      Send packets from CPU to create a swirl on each stream. Please refer
 *      to item 6 of the "Test Setup" portion of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint8 *packet_store_ptr;
    uint32 pkt_size;
    int i, j;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_tx;
    int channel_done;
    int flags = 0;
    uint32 pkt_count = 0;
    uint32 flood_cnt;
    uint32 use_random_packet_sizes = 0;
    uint32 num_pkts_tx;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    soc_dma_init(unit);

    dv_tx = soc_dma_dv_alloc(unit, DV_TX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nSending packets ...");

    for (i = 0; i < l2mc_p->num_streams; i++) {
        if (l2mc_p->pkt_size_param == 0) {
            if (l2mc_p->hg_stream[i] == 1) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
        } else {
            pkt_size = l2mc_p->pkt_size_param;
        }

        if (l2mc_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, l2mc_p->stream[i][0]);
        } else {
            flood_cnt = l2mc_p->flood_pkt_cnt_param;
        }

        if (pkt_size == 1) {
            use_random_packet_sizes = 1;
        }

        cli_out("\nflood_cnt for stream %0d = %0d", i, flood_cnt);

        if (l2mc_p->hg_stream[i] == 1) {
            num_pkts_tx = 2 * flood_cnt;
        } else {
            num_pkts_tx = flood_cnt;
        }

        if ((num_pkts_tx > MAX_PKTS_PER_STREAM) &&
            (l2mc_p->flood_pkt_cnt_param == 0)) {
            num_pkts_tx = MAX_PKTS_PER_STREAM;
        }
        for (j = 0; j < num_pkts_tx; j++) {
            if (use_random_packet_sizes == 1) {
                pkt_size = l2mc_p->rand_pkt_sizes[i][j];
            }
            packet_store_ptr =
                sal_dma_alloc(pkt_size * sizeof(uint8), "packet");

            pkt_count++;
            channel_done = 0;
            soc_dma_abort_dv(unit, dv_tx);
            sal_srand(l2mc_p->pkt_seed + i + j);
            l2mc_gen_random_l2_pkt(packet_store_ptr, pkt_size, mac_da,
                                        mac_sa, TPID,
                                        (uint16)(l2mc_p->tx_vlan[i]), j);
            soc_dma_dv_reset(DV_TX, dv_tx);
            soc_dma_desc_add(dv_tx, (sal_vaddr_t) (packet_store_ptr), pkt_size,
                             lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
            soc_dma_desc_end_packet(dv_tx);
            soc_dma_chan_config(unit, TX_CHAN, DV_TX, SOC_DMA_F_POLL);
            soc_dma_start(unit, TX_CHAN, dv_tx);

            while (channel_done == 0) {
                l2mc_dma_chan_check_done(unit, TX_CHAN,
                                              SOC_DMA_POLL_CHAIN_DONE,
                                              &channel_done);
            }
            sal_dma_free(packet_store_ptr);
        }
        cli_out("\n%0d Packets sent", pkt_count);
    }
    soc_dma_dv_free(unit, dv_tx);
}

/*
 * Function:
 *      get_rates
 * Purpose:
 *      Read packet counters in the MAC to measure rate over a given interval.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      rate_calc_int: Interval in seconds over which rate is measured
 *
 * Returns:
 *     Nothing
 *
 */

static void
get_rates(int unit, uint32 rate_calc_int)
{
    int p;
    int i;
    uint32 ipg, preamble;
    uint64 rdata;
    uint64 tpkt_delta;
    uint64 tbyt_delta;
    uint64 rate_calc_int_64;

    l2mc_t *l2mc_p = l2mc_parray[unit];

    COMPILER_64_SET(rate_calc_int_64, 0, rate_calc_int);

    cli_out("\nCalculating Rates:");

    l2mc_p->tpkt_start =
        (uint64 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint64),
                             "tpkt_start");
    l2mc_p->tpkt_end =
        (uint64 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint64),
                             "tpkt_end");
    l2mc_p->tbyt_start =
        (uint64 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint64),
                             "tbyt_start");
    l2mc_p->tbyt_end =
        (uint64 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint64),
                             "tbyt_end");
    l2mc_p->rate =
        (uint64 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint64),
                             "rate");

    cli_out("\nWait 2s for traffic to stabilize");
    sal_usleep(2000000);

    cli_out("\nMeasuring Rate over a period of %0ds", rate_calc_int);

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) soc_reg_get(unit, TPKTr, p, 0, &rdata);
        COMPILER_64_SET(l2mc_p->tpkt_start[i], COMPILER_64_HI(rdata),
                        COMPILER_64_LO(rdata));
        i++;
    }

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) soc_reg_get(unit, TBYTr, p, 0, &rdata);
        COMPILER_64_SET(l2mc_p->tbyt_start[i], COMPILER_64_HI(rdata),
                        COMPILER_64_LO(rdata));
        i++;
    }

    sal_usleep(rate_calc_int * 1000000);

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) soc_reg_get(unit, TBYTr, p, 0, &rdata);
        COMPILER_64_SET(l2mc_p->tbyt_end[i], COMPILER_64_HI(rdata),
                        COMPILER_64_LO(rdata));
        i++;
    }

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) soc_reg_get(unit, TPKTr, p, 0, &rdata);
        COMPILER_64_SET(l2mc_p->tpkt_end[i], COMPILER_64_HI(rdata),
                        COMPILER_64_LO(rdata));
        i++;
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (IS_HG_PORT(unit, l2mc_p->port_list[i])) {
            ipg = HG2_IPG;
            preamble = 0;
        } else {
            ipg = ENET_IPG;
            preamble = ENET_PREAMBLE;
        }
        COMPILER_64_DELTA(tbyt_delta, l2mc_p->tbyt_start[i],
                          l2mc_p->tbyt_end[i]);
        COMPILER_64_DELTA(tpkt_delta, l2mc_p->tpkt_start[i],
                          l2mc_p->tpkt_end[i]);
        COMPILER_64_UMUL_32(tpkt_delta, (ipg + preamble));
        COMPILER_64_ADD_64(tbyt_delta, tpkt_delta);
        COMPILER_64_UMUL_32(tbyt_delta, 8);
        COMPILER_64_SET(l2mc_p->rate[i], COMPILER_64_HI(tbyt_delta),
                        COMPILER_64_LO(tbyt_delta));
        COMPILER_64_UDIV_64(l2mc_p->rate[i], rate_calc_int_64);
    }
}

/*
 * Function:
 *      check_rates
 * Purpose:
 *      Check rates against expected rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
check_rates(int unit)
{
    uint64 min_rate;
    uint64 max_rate;
    uint32 tolerance;
    uint64 margin_of_error;
    uint64 port_line_rate;
    uint64 hundred_64;
    int i;
    int fail = 0;
    char avg_rate_str[32], min_rate_str[32], max_rate_str[32];
    l2mc_t *l2mc_p = l2mc_parray[unit];

    COMPILER_64_SET(hundred_64, 0, 100);

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (!(COMPILER_64_IS_ZERO(l2mc_p->exp_rate[i]))) {
            if (l2mc_p->port_oversub[i] == 1) {
                tolerance = l2mc_p->rate_tolerance_ov_param;
            } else {
                tolerance = l2mc_p->rate_tolerance_lr_param;
            }

            COMPILER_64_SET(margin_of_error,
                            COMPILER_64_HI(l2mc_p->exp_rate[i]),
                            COMPILER_64_LO(l2mc_p->exp_rate[i]));
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);

            COMPILER_64_DELTA(min_rate, margin_of_error,
                              l2mc_p->exp_rate[i]);

            COMPILER_64_SET(port_line_rate, 0, l2mc_p->port_speed[i]);
            COMPILER_64_UMUL_32(port_line_rate, 1000000);
            COMPILER_64_ZERO(max_rate);
            COMPILER_64_SET(margin_of_error,
                            COMPILER_64_HI(port_line_rate),
                            COMPILER_64_LO(port_line_rate));
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);
            COMPILER_64_SET(max_rate, COMPILER_64_HI(port_line_rate),
                            COMPILER_64_LO(port_line_rate));
            COMPILER_64_ADD_64(max_rate, margin_of_error);

            cli_out("\n");
            if ((COMPILER_64_LT(l2mc_p->rate[i], min_rate))
                || (COMPILER_64_GT(l2mc_p->rate[i], max_rate))) {
                cli_out("*ERROR: ");
                fail = 1;
            }
            format_uint64_decimal(avg_rate_str, l2mc_p->rate[i], 0);
            format_uint64_decimal(min_rate_str, min_rate, 0);
            format_uint64_decimal(max_rate_str, max_rate, 0);
            cli_out
                ("Test Port = %0d, Device Port = %0d, Rate = %s, "
                 "min_rate = %s, max_rate = %s",
                 i, l2mc_p->port_list[i],
                 avg_rate_str, min_rate_str, max_rate_str);
        }
    }
    if (fail == 1) {
        test_error(unit,
                   "\n*************** RATE CHECKS FAILED ***************\n");
        l2mc_p->test_fail = 1;
    } else {
        cli_out("\n****************** RATE CHECKS PASSED ******************\n");
    }
}

/*
 * Function:
 *      set_up_ports
 * Purpose:
 *      1. Enable IFP for CPU port
 *      2. Program ING_VLAN_TAG_ACTION_PROFILE to remove inner tag for double
 *         tagged packets
 *      3. Program MMU to not accept any packets from ports l2mc_p->stream[i][2..n]
 *         by setting THDI_INPUT_PORT_XON_ENABLES.INPUT_PORT_RX_ENABLE=0.
 *         This prevents the number of packets in each swill from multiplying.
 *      4. Set IGNORE_MY_MODID to 1 to prevent dropping HG2 packets after looping
 *         back.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
set_up_ports(int unit)
{
    int i, j;
    port_tab_entry_t port_tab_entry;
    l2mc_t *l2mc_p = l2mc_parray[unit];
    ing_vlan_tag_action_profile_entry_t ing_vlan_tag_action_profile_entry;
    uint64 ing_config;

    soc_mem_read(unit, PORT_TABm, COPYNO_ALL,
                    CPU_PORT, port_tab_entry.entry_data);
    soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                        FILTER_ENABLEf, 0x1);
    soc_mem_write(unit, PORT_TABm, COPYNO_ALL, CPU_PORT,
                    port_tab_entry.entry_data);

    soc_mem_read(unit, ING_VLAN_TAG_ACTION_PROFILEm, COPYNO_ALL,
                 0, ing_vlan_tag_action_profile_entry.entry_data);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm,
                        ing_vlan_tag_action_profile_entry.entry_data,
                        DT_ITAG_ACTIONf, 0x3);
    soc_mem_write(unit, ING_VLAN_TAG_ACTION_PROFILEm, COPYNO_ALL,
                 0, ing_vlan_tag_action_profile_entry.entry_data);

    for (i = 0; i < l2mc_p->num_streams; i++) {
        for (j = 2; j < (l2mc_p->num_copy_param + 1); j++) {
            soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr,
                                   l2mc_p->port_list[l2mc_p->stream[i][j]],
                                   INPUT_PORT_RX_ENABLEf, 0x0);
        }
    }

    (void) soc_reg_get(unit, ING_CONFIG_64r, REG_PORT_ANY, 0, &ing_config);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &ing_config,
                          IGNORE_MY_MODIDf, 0x1);
    (void) soc_reg_set(unit, ING_CONFIG_64r, REG_PORT_ANY, 0, ing_config);
}

/*
 * Function:
 *      check_mib_counters
 * Purpose:
 *      Checks Error counters in MAC
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
check_mib_counters(int unit)
{
    uint32 i, j;
    uint64 rdata;
    l2mc_t *l2mc_p = l2mc_parray[unit];
    soc_reg_t error_counters[] = {
                                    RFLRr,
                                    RFCSr,
                                    RJBRr,
                                    RMTUEr,
                                    RTRFUr,
                                    RERPKTr,
                                    RRPKTr,
                                    RUNDr,
                                    RFRGr,
                                    RRBYTr,
                                    TJBRr,
                                    TFCSr,
                                    TERRr,
                                    TFRGr,
                                    TRPKTr,
                                    TUFLr,
                                    TPCEr,
                                    RDISCr,
                                    RIPHE4r,
                                    RIPHE6r,
                                    RIPD4r,
                                    RIPD6r,
                                    RPORTDr
                                  };

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        for (j = 0; j < (sizeof(error_counters) / sizeof(soc_reg_t)); j++) {
            (void) soc_reg_get(unit, error_counters[j], l2mc_p->port_list[i], 0,
                        &rdata);
            if (!(COMPILER_64_IS_ZERO(rdata))) {
                test_error (unit, "\n*ERROR: Error counter %s has a non zero value "
                            "for device port %0d",
                            SOC_REG_NAME(unit, error_counters[j]),
                            l2mc_p->port_list[i]);
                l2mc_p->test_fail = 1;
            }
        }
    }
}

/*
 * Function:
 *      invalidate_vlan
 * Purpose:
 *      Invalidates VLAN in VLAN table
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN to be invalidated
 *
 * Returns:
 *     Nothing
 *
 */

static void
invalidate_vlan(int unit, uint32 vlan)
{
    vlan_tab_entry_t vlan_tab_entry;

    soc_mem_read(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
    soc_mem_field32_set(unit, VLAN_TABm, vlan_tab_entry.entry_data,
                        VALIDf, 0x0);
    soc_mem_write(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
}

/*
 * Function:
 *      check_packet_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
check_packet_integrity(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint8 *ref_pkt_ptr;
    uint8 *rx_pkt_ptr;
    uint32 pkt_size;
    int i, j, k;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_rx;
    int channel_done;
    int flags = 0;
    uint32 pkt_count = 0;
    uint32 pkt_count_port = 0;
    uint32 flood_cnt;
    int timeout = RXDMA_TIMEOUT;
    uint32 match = 1;
    uint32 seq_id;
    uint32 zero_crc = 1;
    uint32 num_pkts_rx;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    dv_rx = soc_dma_dv_alloc(unit, DV_RX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nRouting all packets back to CPU to check pkt integrity ...");

    soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(0, RX_CHAN),
                  0xffffffff);
    soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, RX_CHAN),
                  0xffffffff);

    soc_reg_field32_modify(unit, CPU_CONTROL_0r, 0, UVLAN_TOCPUf, 0x1);

    for (i = 0; i < l2mc_p->num_streams; i++) {
        if (l2mc_p->pkt_size_param == 0) {
            if (l2mc_p->hg_stream[i] == 1) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
        } else {
            pkt_size = l2mc_p->pkt_size_param;
        }

        if (l2mc_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, l2mc_p->stream[i][0]);
        } else {
            flood_cnt = l2mc_p->flood_pkt_cnt_param;
        }

        num_pkts_rx = 2 * flood_cnt;

        if ((num_pkts_rx > MAX_PKTS_PER_STREAM) &&
            (l2mc_p->flood_pkt_cnt_param == 0)) {
            num_pkts_rx = MAX_PKTS_PER_STREAM;
        }

        for (j = 0; j < num_pkts_rx; j++) {
            ref_pkt_ptr =
                sal_dma_alloc(MTU * sizeof(uint8), "ref_packet");
            rx_pkt_ptr =
                sal_dma_alloc(MTU * sizeof(uint8), "rx_pkt");

            for (k = 0; k < MTU; k++) {
                rx_pkt_ptr[k] = 0x00;
            }

            channel_done = 0;
            soc_dma_abort_dv(unit, dv_rx);

            soc_dma_dv_reset(DV_RX, dv_rx);
            soc_dma_desc_add(dv_rx, (sal_vaddr_t) (rx_pkt_ptr), MTU,
                             lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
            soc_dma_desc_end_packet(dv_rx);
            soc_dma_chan_config(unit, RX_CHAN, DV_RX, SOC_DMA_F_POLL);

            soc_dma_start(unit, RX_CHAN, dv_rx);

            if (l2mc_p->hg_stream[i] == 1) {
                invalidate_vlan(unit, l2mc_p->tx_vlan[i] + 1);
            } else {
                invalidate_vlan(unit, l2mc_p->tx_vlan[i]);
            }

            while (channel_done == 0 && timeout > 0) {
                l2mc_dma_chan_check_done(unit, RX_CHAN,
                                              SOC_DMA_POLL_CHAIN_DONE,
                                              &channel_done);
                timeout--;
            }

            if (timeout > 0) {
                pkt_count++;
                pkt_count_port++;
                seq_id = 0x00000000;
                pkt_size = 0x00000000;

                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 6] << 24);
                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 7] << 16);
                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 8] << 8);
                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 9]);
                pkt_size |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 10] << 8);
                pkt_size |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 11]);

                sal_srand(l2mc_p->pkt_seed + i + seq_id);
                l2mc_gen_random_l2_pkt(ref_pkt_ptr, pkt_size, mac_da,
                                            mac_sa, TPID,
                                            (uint16)(l2mc_p->tx_vlan[i] + 1),
                                            seq_id);

                for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                    if (rx_pkt_ptr[k] != ref_pkt_ptr[k]) {
                        match = 0;
                        test_error(unit, "\n*ERROR: Stream %0d "
                                         "has packet corruption"
                                         " on received pkt %0d",
                                          i, j);
                        l2mc_p->test_fail = 1;
                        break;
                    }
                }

                for(k = 0; k < NUM_BYTES_CRC; k++) {
                    if (rx_pkt_ptr[pkt_size - NUM_BYTES_CRC + k] != 0) {
                        zero_crc = 0;
                    }
                }

                if (zero_crc == 1) {
                    match = 0;
                    test_error(unit, "\n*ERROR: Test port %0d, "
                                        "Device Port %0d has packet corruption"
                                        "(zero CRC) on received pkt %0d",
                                        i, l2mc_p->port_list[i], j);
                    l2mc_p->test_fail = 1;
                }

                zero_crc = 1;
            }

            timeout = RXDMA_TIMEOUT;
            sal_dma_free(rx_pkt_ptr);
            sal_dma_free(ref_pkt_ptr);
        }
        cli_out("\n%0d Packets received", pkt_count);

        if (l2mc_p->oversub_config == 0) {
            if (pkt_count_port < num_pkts_rx) {
                test_error(unit, "\n*ERROR: Stream %0d expected %0d pkts, "
                                "received %0d packets",
                                i, num_pkts_rx,
                                pkt_count_port);
                l2mc_p->test_fail = 1;
            }
        }
        pkt_count_port = 0;
    }

    soc_dma_dv_free(unit, dv_rx);

    if (match == 1) {
        cli_out("\n***** PACKET INTEGRITY CHECKS PASSED *****\n");
    }
    else {
        test_error(unit, "\n***** PACKET INTEGRITY CHECKS FAILED *****\n");
    }
}

/*
 * Function:
 *      l2mc_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */


int
l2mc_test_init(int unit, args_t *a, void **pa)
{
    l2mc_t *l2mc_p;

    l2mc_p = l2mc_parray[unit];

    l2mc_p = sal_alloc(sizeof(l2mc_t), "l2mc_test");
    sal_memset(l2mc_p, 0, sizeof(l2mc_t));
    l2mc_parray[unit] = l2mc_p;
    cli_out("\nCalling l2mc_test_init");
    l2mc_parse_test_params(unit, a);

    l2mc_p->test_fail = 0;

    if (l2mc_p->bad_input == 1) {
        goto done;
    }

    l2mc_soc_set_up_mac_lpbk(unit);
    l2mc_turn_off_cmic_mmu_bkp(unit);
    l2mc_soc_turn_off_fc(unit);
    /* coverity[dont_call : FALSE] */
    l2mc_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      l2mc_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */


int
l2mc_test(int unit, args_t *a, void *pa)
{
    l2mc_t *l2mc_p;

    l2mc_p = l2mc_parray[unit];

    if (l2mc_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling l2mc_test");
    set_port_property_arrays(unit);
    set_up_ports(unit);
    set_up_streams(unit);
    send_pkts(unit);

done:
    return 0;
}

/*
 * Function:
 *      l2mc_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

int
l2mc_test_cleanup(int unit, void *pa)
{
    l2mc_t *l2mc_p;
    int rv;
    int i;

    l2mc_p = l2mc_parray[unit];

    if (l2mc_p->bad_input == 1) {
        goto done;
    }
    cli_out("\nCalling l2mc_test_cleanup");

    get_rates(unit, l2mc_p->rate_calc_interval_param);
    check_mib_counters(unit);
    check_rates(unit);

    if (l2mc_p->check_packet_integrity_param != 0) {
        check_packet_integrity(unit);
    }

    sal_free(l2mc_p->port_speed);
    sal_free(l2mc_p->port_list);
    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        sal_free(l2mc_p->stream[i]);
    }
    sal_free(l2mc_p->stream);
    sal_free(l2mc_p->tx_vlan);
    sal_free(l2mc_p->hg_stream);
    sal_free(l2mc_p->port_oversub);
    sal_free(l2mc_p->ovs_ratio_x1000);
    sal_free(l2mc_p->rate);
    sal_free(l2mc_p->exp_rate);
    sal_free(l2mc_p->tpkt_start);
    sal_free(l2mc_p->tpkt_end);
    sal_free(l2mc_p->tbyt_start);
    sal_free(l2mc_p->tbyt_end);

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        sal_free(l2mc_p->rand_pkt_sizes[i]);
    }
    sal_free(l2mc_p->rand_pkt_sizes);

done:
    if (l2mc_p->bad_input == 1) {
        l2mc_p->test_fail = 1;
    }

    if (l2mc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }
    else {
        rv = BCM_E_NONE;
    }

    sal_free(l2mc_p);

    cli_out("\n");

    return rv;
}
#endif /* BCM_ESW_SUPPORT */
