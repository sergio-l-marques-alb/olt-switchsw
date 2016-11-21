/*
 * $Id: ipmc.c,v 1.0 Broadcom SDK $
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
 * Streaming test to check TDM table and programming. Each port loops traffic
 * back to itself using port bridging and higig lookup for HG ports. Test test
 * calculates expected rates based on port config and oversub ratio and checks
 * against it.
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swill. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * Setup and Test Sequence:
 * 1. Group all active front panel ports into groups of
 *    (NumL2Copy + NumL3Copy+ 1) ports. Each group constitutes 1 IPMC "stream".
 *    All ports in a stream should have the same line rate, although they may
 *    have different oversub ratios.
 * 2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be
 *    arranged such that the P1 is the slowest port, i.e. has the highest oversub
 *    ratio and the highest number of same port replications.
 * 3. Turn off all flow control, set MAC loopback on all ports and disable TTL
 *    decrement by setting EGR_IPMC_CFG2.DISABLE_TTL_DECREMENT=1.
 * 4. Randomly decide whether each receiving port in the stream (P1, P2, ... Pn)
 *    should receive L2 or L3 copies. If port P1 receives L2 copies, the IPMC
 *    stream is designated an "L2 stream". If it receives L3 copies the stream
 *    is designated an "L3 stream".
 * 5. For each stream:
 *      a. Place port P0 on VLAN0. Create an empty VLAN called DROP_VLAN with
 *         no ports
 *      b. Add all ports receiving L2 copies to VLAN1 and multicast group MC0
 *         using bcm_multicast_egress_add(). This programs the L2MC table.
 *      c. Add the following VLAN translations:
 *          i. For L2 streams: Port=P2, P3 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN
 *          ii. For L3 streams: Port=P1, P2 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN
 *          iii. Port=P0 OldVlan=VLAN0 NewVlan=VLAN1
 *          iv. For L2 streams: Port=P1 OldVlan=VLAN1 NewVlan=DROP_VLAN
 *      d. For each port Pk receiving L3 copies, let the number of L3 copies received at a port be m (=NumRep+1).
 *          i. Create m VLANs (VLANk0, VLANk1, .... VLANk(m-1)), each with port Pk
 *          ii. For L3 streams: If k=1, VLAN2=VLANk0
 *          iii. For L3 streams: Create the following VLAN translation: Port=Pk OldVlan=VLAN2 NewVlan=VLAN0
 *          iv. For L2 streams: Create the following VLAN translation: Port=Pk OldVlan= VLANk0, VLANk1, ... VLANk(m-1)) NewVlan=DROP_VLAN
 *          v. Create m L3 interfaces I0, I1 ... I(m-1) with VLANk0, VLANk1, .... VLANk(m-1)
 *          vi. Add interface to multicat group MC1 using bcm_multicast_egress_add().
 *          vii. Program IPMC table with necessary SIP, DIP and multicast group MC1.
 * 6. Send packets from CPU port on VLAN0 of each stream to flood the stream.
 *    If FloodCnt=0, the test will determine the number of packets to send based.
 *    These values are obtained empirically through trial and error for LR ports
 *    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.
 *    Embed the packet size and random seed used to generate each packet in the
 *    payload.
 * 7. Allow the packets to swill for a fixed amount of time decided by RunTime.
 * 8. Snoop back all packets in each stream to the CPU using the following sequence:
 *      a. Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an unknown VLAN to CPU.
 *      b. Invalidate VLAN0 for all streams by setting VALID=0 in the VLAN table
 * 9. Decode each received packet to obtain the random seed and packet size.
 *    Reconstruct the expected packet and check the received packet for integrity.
 * Checks:
 * 1. Packet counts on each port.
 * 2. No packet drops on any stream for LR configs.
 * 3. All packets in a given stream snooped back to CPU port and checked for integrity.
 * 
 * Test Limitations
 * 1. FloodCnt automatically computed in the test supports packet sizes upto
 *    1518B and NumCopy=2. Any deviations may result in packet drops for LR
 *    configs or false rate errors being reported. For custom configs the user is
 *    encouraged to tweak the FloodCnt value as per their needs.
 *
 * PktSize : Packet size in bytes not including HG2 header. Set to 0 for worst
 *           case packet sizes (145B for Ethernet and 76B for HG2). Set to 1 for
 *           random packet sizes.
 * FloodCnt: Number of packets sent from CPU to flood each stream (packet swill).
 *           Set to 0 for test to automatically calculate.
 * NumL2Copy: Number of ports receiving L2 copies in each stream (2 by default).
 * NumL3Copy:  Number of ports receiving L3 copies in each stream (2 by default).
 * NumRep: Number of same-port replications on ports receiving . Set to 0 for
 *         random.
 * MaxNumRep: Max number of replications if NumRep=0. Set both MaxNumRep and
 *            NumRep to 0 for no same port replication.
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4.
 *              Set to 0 for random.
 * RunTime: Interval in seconds over which swill is allowed to
 *          run (10s by default).
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
#include <bcm/ipmc.h>
#include <bcm/multicast.h>

#include "testlist.h"
#include "gen_pkt.h"

#define MAC_DA {0x01,0x00,0x5e,0x02,0x03,0x04}
#define MAC_SA {0x00,0x00,0x00,0x00,0x00,0x01}
#define TPID 0x8100
#define VLAN 0x2
#define MIN_PKT_SIZE 64
#define MTU 9216
#define MTU_CELL_CNT 45

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define NUM_L2_COPY_PARAM_DEFAULT 2
#define NUM_L3_COPY_PARAM_DEFAULT 2
#define NUM_REP_PARAM_DEFAULT 1
#define MAX_NUM_REP_PARAM_DEFAULT 3
#define RUN_TIME_PARAM_DEFAULT 10
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define CPU_PORT 0
#define MISC_BW 10000
#define NUM_SUBP_OBM 4
#define TARGET_CELL_COUNT 40
#define TARGET_PKT_COUNT 30
#define MAX_FP_PORTS 130

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

#define DROP_VLAN 0xabc
#define FIRST_MC_GROUP 0x2000001
#define FIRST_INTF_ID 0x2

#define SIP 0x02030405
#define DIP 0xe0020304
#define TTL 5

#define MAX_PKTS_PER_STREAM 50
#define PKT_COUNT_CHECK_TOL 5

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3)

typedef struct ipmc_s {
    uint32 num_fp_ports;
    uint32 num_streams;
    int *port_speed;
    int *port_list;
    int **stream;
    uint32 *num_rep;
    uint32 *l3_copy;
    uint32 *tx_vlan;
    uint32 *exp_vlan;
    uint8 (*exp_mac_addr)[NUM_BYTES_MAC_ADDR];
    uint32 *port_oversub;
    uint32 oversub_config;
    uint32 *port_used;
    uint32 num_pipes;
    uint32 total_chip_bw;
    uint32 bw_per_pipe;
    uint32 *ovs_ratio_x1000;
    uint64 *exp_rate;
    uint64 *tpkt_start;
    uint64 *tpkt_end;
    uint32 **rand_pkt_sizes;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 num_l2_copy_param;
    uint32 num_l3_copy_param;
    uint32 check_packet_integrity_param;
    uint32 num_rep_param;
    uint32 max_num_rep_param;
    uint32 max_num_cells_param;
    uint32 run_time_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
    uint32 num_copy;
} ipmc_t;

static ipmc_t *ipmc_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      ipmc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      ipmc_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */

static void
ipmc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    char tr513_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "Streaming test to check TDM table and programming. Each port loops traffic\n"
    "back to itself using port bridging and higig lookup for HG ports. Test test\n"
    "calculates expected rates based on port config and oversub ratio and checks\n"
    "against it.\n"
    "Configuration parameters passed from CLI:\n"
    "PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all\n"
    "        ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes\n"
    "FloodCnt: Number of packets in each swill. Setting this to 0 will let the\n"
    "        test calculate the number of packets that can be sent to achieve\n"
    "        a lossless swirl at full rate. Set to 0 by default.\n"
    "Setup and Test Sequence:\n"
    "1. Group all active front panel ports into groups of\n"
    "    (NumL2Copy + NumL3Copy+ 1) ports. Each group constitutes 1 IPMC \"stream\".\n"
    "    All ports in a stream should have the same line rate, although they may\n"
    "    have different oversub ratios.\n"
    "2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be\n"
    "    arranged such that the P1 is the slowest port, i.e. has the highest oversub\n"
    "    ratio and the highest number of same port replications.\n"
    "3. Turn off all flow control, set MAC loopback on all ports and disable TTL\n"
    "    decrement by setting EGR_IPMC_CFG2.DISABLE_TTL_DECREMENT=1.\n"
    "4. Randomly decide whether each receiving port in the stream (P1, P2, ... Pn)\n"
    "    should receive L2 or L3 copies. If port P1 receives L2 copies, the IPMC\n"
    "    stream is designated an \"L2 stream\". If it receives L3 copies the stream\n"
    "    is designated an \"L3 stream\".\n"
    "5. For each stream:\n"
    "    a. Place port P0 on VLAN0. Create an empty VLAN called DROP_VLAN with\n"
    "        no ports\n"
    "    b. Add all ports receiving L2 copies to VLAN1 and multicast group MC0\n"
    "        using bcm_multicast_egress_add(). This programs the L2MC table.\n"
    "    c. Add the following VLAN translations:\n"
    "        i. For L2 streams: Port=P2, P3 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN\n"
    "        ii. For L3 streams: Port=P1, P2 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN\n"
    "        iii. Port=P0 OldVlan=VLAN0 NewVlan=VLAN1\n"
    "        iv. For L2 streams: Port=P1 OldVlan=VLAN1 NewVlan=DROP_VLAN\n"
    "    d. For each port Pk receiving L3 copies, let the number of L3 copies received at a port be m (=NumRep+1).\n"
    "        i. Create m VLANs (VLANk0, VLANk1, .... VLANk(m-1)), each with port Pk\n"
    "        ii. For L3 streams: If k=1, VLAN2=VLANk0\n"
    "        iii. For L3 streams: Create the following VLAN translation: Port=Pk OldVlan=VLAN2 NewVlan=VLAN0\n"
    "        iv. For L2 streams: Create the following VLAN translation: Port=Pk OldVlan= VLANk0, VLANk1, ... VLANk(m-1)) NewVlan=DROP_VLAN\n"
    "        v. Create m L3 interfaces I0, I1 ... I(m-1) with VLANk0, VLANk1, .... VLANk(m-1)\n"
    "        vi. Add interface to multicat group MC1 using bcm_multicast_egress_add().\n"
    "        vii. Program IPMC table with necessary SIP, DIP and multicast group MC1.\n"
    "6. Send packets from CPU port on VLAN0 of each stream to flood the stream.\n"
    "    If FloodCnt=0, the test will determine the number of packets to send based.\n"
    "    These values are obtained empirically through trial and error for LR ports\n"
    "    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.\n"
    "    Embed the packet size and random seed used to generate each packet in the\n"
    "    payload.\n"
    "7. Allow the packets to swill for a fixed amount of time decided by RunTime.\n"
    "8. Snoop back all packets in each stream to the CPU using the following sequence:\n"
    "    a. Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an unknown VLAN to CPU.\n"
    "    b. Invalidate VLAN0 for all streams by setting VALID=0 in the VLAN table\n"
    "9. Decode each received packet to obtain the random seed and packet size.\n"
    "    Reconstruct the expected packet and check the received packet for integrity.\n"
    "Checks:\n"
    "1. Packet counts on each port.\n"
    "2. No packet drops on any stream for LR configs.\n"
    "3. All packets in a given stream snooped back to CPU port and checked for integrity.\n"
    "\n"
    " Test Limitations\n"
    " 1. FloodCnt automatically computed in the test supports packet sizes upto\n"
    "    1518B and NumCopy=2. Any deviations may result in packet drops for LR\n"
    "    configs or false rate errors being reported. For custom configs the user is\n"
    "    encouraged to tweak the FloodCnt value as per their needs.\n"
    "\n"
    " PktSize : Packet size in bytes not including HG2 header. Set to 0 for worst\n"
    "           case packet sizes (145B for Ethernet and 76B for HG2). Set to 1 for\n"
    "           random packet sizes.\n"
    " FloodCnt: Number of packets sent from CPU to flood each stream (packet swill).\n"
    "           Set to 0 for test to automatically calculate.\n"
    " NumL2Copy: Number of ports receiving L2 copies in each stream (2 by default).\n"
    " NumL3Copy:  Number of ports receiving L3 copies in each stream (2 by default).\n"
    " NumRep: Number of same-port replications on ports receiving . Set to 0 for\n"
    "         random.\n"
    " MaxNumRep: Max number of replications if NumRep=0. Set both MaxNumRep and\n"
    "            NumRep to 0 for no same port replication.\n"
    " MaxNumCells: Max number of cells for random packet sizes. Default = 4.\n"
    "              Set to 0 for random.\n"
    " RunTime: Interval in seconds over which swill is allowed to\n"
    "          run (10s by default).\n";
#endif
    ipmc_p->bad_input = 0;

    ipmc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    ipmc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    ipmc_p->num_l2_copy_param = NUM_L2_COPY_PARAM_DEFAULT;
    ipmc_p->num_l3_copy_param = NUM_L3_COPY_PARAM_DEFAULT;
    ipmc_p->num_rep_param = NUM_REP_PARAM_DEFAULT;
    ipmc_p->max_num_rep_param = MAX_NUM_REP_PARAM_DEFAULT;
    ipmc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    ipmc_p->run_time_param = RUN_TIME_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "NumL2Copy", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->num_l2_copy_param), NULL);
    parse_table_add(&parse_table, "NumL3Copy", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->num_l3_copy_param), NULL);
    parse_table_add(&parse_table, "NumRep", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->num_rep_param), NULL);
    parse_table_add(&parse_table, "MaxNumRep", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->max_num_rep_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "RunTime", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->run_time_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg(tr513_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        ipmc_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
        cli_out("\npkt_size_param = %0d", ipmc_p->pkt_size_param);
        cli_out("\nflood_pkt_cnt_param = %0d", ipmc_p->flood_pkt_cnt_param);
        cli_out("\nnum_l2_copy_param = %0d", ipmc_p->num_l2_copy_param);
        cli_out("\nnum_l3_copy_param = %0d", ipmc_p->num_l3_copy_param);
        cli_out("\nnum_rep_param = %0d", ipmc_p->num_rep_param);
        cli_out("\nmax_num_rep_param = %0d", ipmc_p->max_num_rep_param);
        cli_out("\nmax_num_cells_param = %0d", ipmc_p->max_num_cells_param);
        cli_out("\nrun_time_param = %0d", ipmc_p->run_time_param);
        cli_out("\n -----------------------------------------------------");
    }

    if (ipmc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        ipmc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (ipmc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (ipmc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (ipmc_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        ipmc_p->bad_input = 1;
    } else if (ipmc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Device MTU)\n",
                MTU);
        ipmc_p->bad_input = 1;
    }

    if (ipmc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }

    if (ipmc_p->num_rep_param > MAX_NUM_REP_PARAM_DEFAULT) {
        ipmc_p->max_num_rep_param = MAX_NUM_REP_PARAM_DEFAULT;
        cli_out("\nMax replications supported by this test = %0d,"
                "forcing number of replications to %0d",
                MAX_NUM_REP_PARAM_DEFAULT, MAX_NUM_REP_PARAM_DEFAULT);
    }

    ipmc_p->num_copy = ipmc_p->num_l2_copy_param + ipmc_p->num_l3_copy_param;
}

/*
 * Function:
 *      ipmc_soc_set_up_mac_lpbk
 * Purpose:
 *      Enable MAC loopback on all ports
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
ipmc_soc_set_up_mac_lpbk(int unit)
{
    int p;

    cli_out("\nSetting up MAC loopbacks");
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_MAC);
    }
}

/*
 * Function:
 *      ipmc_turn_off_cmic_mmu_bkp
 * Purpose:
 *      Turn off CMIC to MMU backpressure
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
ipmc_turn_off_cmic_mmu_bkp(int unit)
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
 *      ipmc_soc_turn_off_fc
 * Purpose:
 *      Turn off flow control at the MAC, IDB and MMU.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


static void
ipmc_soc_turn_off_fc(int unit)
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
    ipmc_t *ipmc_p = ipmc_parray[unit];

    ipmc_p->ovs_ratio_x1000 =
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
        for (i = 0; i < ipmc_p->num_fp_ports; i++) {
            if (SOC_PBMP_MEMBER(si->pipe_pbm[pipe], ipmc_p->port_list[i])) {
                if (ipmc_p->port_oversub[i] == 1) {
                    if (ipmc_p->port_used[i] == 1) {
                        ov_bandwidth += ipmc_p->port_speed[i];
                    }
                } else {
                    lr_bandwidth += ipmc_p->port_speed[i];
                }
            }
        }

        ipmc_p->ovs_ratio_x1000[pipe] =
            (ov_bandwidth) / ((pipe_bandwidth - lr_bandwidth) / 1000);

        if (ipmc_p->pkt_size_param >
            safe_pkt_size(unit, ipmc_p->ovs_ratio_x1000[pipe])) {
            ipmc_p->ovs_ratio_x1000[pipe] = 1000;
        }

        if (ipmc_p->ovs_ratio_x1000[pipe] < 1000) {
            ipmc_p->ovs_ratio_x1000[pipe] = 1000;
        }

        cli_out
            ("\nFor pipe %0d, LR BW = %0d, OV BW = %0d, ovs_ratio(x1000) = %0d",
             pipe, lr_bandwidth, ov_bandwidth,
             ipmc_p->ovs_ratio_x1000[pipe]);
    }
}

/*
 * Function:
 *      set_exp_rates
 * Purpose:
 *      Set expected rates array (ipmc_p->exp_rate). This is based on port
 *      speed and oversub ratio (for oversubscribed ports). The function basically
 *      ensures that ipmc_p->stream[][1] is assigned the slowest, i.e. the
 *      rate in each stream is limited by the most oversubscribed port in the
 *      stream.
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
    ipmc_t *ipmc_p = ipmc_parray[unit];

    cli_out("\nSetting expected rates");
    ipmc_p->exp_rate =
        (uint64 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint64),
                             "exp_rate");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        if (ipmc_p->port_oversub[i] == 1) {
            COMPILER_64_SET(ipmc_p->exp_rate[i], 0,
                            ipmc_p->port_speed[i]);
            COMPILER_64_UMUL_32(ipmc_p->exp_rate[i], 1000000);
            COMPILER_64_SET(ovs_ratio_x1000_64, 0,
                            ipmc_p->ovs_ratio_x1000[get_pipe(unit,
                                                        ipmc_p->
                                                            port_list[i])]);
            COMPILER_64_UMUL_32(ipmc_p->exp_rate[i], 1000);
            COMPILER_64_UDIV_64(ipmc_p->exp_rate[i], ovs_ratio_x1000_64);
        } else {
            COMPILER_64_SET(ipmc_p->exp_rate[i], 0,
                            ipmc_p->port_speed[i]);
            COMPILER_64_UMUL_32(ipmc_p->exp_rate[i], 1000000);
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        if (ipmc_p->port_used[i] == 0) {
            COMPILER_64_ZERO(ipmc_p->exp_rate[i]);
        }
    }

    for (i = 0; i < ipmc_p->num_streams; i++) {
        COMPILER_64_SET(lowest_rate_stream,
                        COMPILER_64_HI(ipmc_p->exp_rate[ipmc_p->stream[i][0]]),
                        COMPILER_64_LO(ipmc_p->exp_rate[ipmc_p->stream[i][0]]));
        lowest_rate_port_stream = 0;

        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (COMPILER_64_GT(lowest_rate_stream,
                               ipmc_p->exp_rate[ipmc_p->stream[i][j]])) {
                lowest_rate_port_stream = j;
                COMPILER_64_SET(lowest_rate_stream,
                                COMPILER_64_HI(ipmc_p->
                                                exp_rate[ipmc_p->stream[i][j]]),
                                COMPILER_64_LO(ipmc_p->
                                                exp_rate[ipmc_p->stream[i][j]]));
            }
        }

        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            COMPILER_64_SET(ipmc_p->exp_rate[ipmc_p->stream[i][j]],
                            COMPILER_64_HI(lowest_rate_stream),
                            COMPILER_64_LO(lowest_rate_stream));
        }

        if (lowest_rate_port_stream != 1) {
            temp = ipmc_p->stream[i][1];
             ipmc_p->stream[i][1] =  ipmc_p->stream[i][lowest_rate_port_stream];
             ipmc_p->stream[i][lowest_rate_port_stream] = temp;
        }
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nPRINTING EXPECTED RATES")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n=======================")));
    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        format_uint64_decimal(exp_rate_str, ipmc_p->exp_rate[i], 0);
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
 *      Set port_list, port_speed, port_oversub arrays and l3_copy.
 *      Also call set_exp_rates.
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
    uint32 temp;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    ipmc_p->num_fp_ports = 0;
    ipmc_p->num_streams = 0;
    ipmc_p->oversub_config = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        ipmc_p->num_fp_ports++;
    }

    ipmc_p->port_list =
        (int *)sal_alloc(ipmc_p->num_fp_ports * sizeof(int),
                         "port_list");
    ipmc_p->port_speed =
        (int *)sal_alloc(ipmc_p->num_fp_ports * sizeof(int),
                         "port_speed_array");
    ipmc_p->port_oversub =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                             "port_oversub_array");

    ipmc_p->l3_copy =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                             "l3_copy");
    ipmc_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->rand_pkt_sizes[i] =
            (uint32 *) sal_alloc(TARGET_PKT_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }

    ipmc_p->num_rep =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                              "num_rep_array");


    ipmc_p->stream =
        (int **) sal_alloc(ipmc_p->num_fp_ports * sizeof(int *),
                              "stream_array*");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->stream[i] =
            (int *) sal_alloc((ipmc_p->num_copy + 1) * sizeof(int),
                                "stream_array");
    }
    ipmc_p->port_used =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                             "ipmc_p->port_used_array");

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        ipmc_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->port_speed[i] =
            si->port_speed_max[ipmc_p->port_list[i]];

        switch(ipmc_p->port_speed[i]) {
            case 11000:
                ipmc_p->port_speed[i] = 10600;
            break;
            case 27000:
                ipmc_p->port_speed[i] = 26500;
            break;
            case 42000:
                ipmc_p->port_speed[i] = 42400;
            break;
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, ipmc_p->port_list[i])
            &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, ipmc_p->port_list[i])))) {
            ipmc_p->oversub_config = 1;
            ipmc_p->port_oversub[i] = 1;
        } else {
            ipmc_p->port_oversub[i] = 0;
        }
    }

    cli_out("\n=========== STREAM INFO ============");
    cli_out("\nStream\tPorts");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->port_used[i] = 0;
    }

    while (ports_accounted_for < ipmc_p->num_fp_ports) {
        do {
            i = sal_rand() % ipmc_p->num_fp_ports;
        }while (ipmc_p->port_used[i] != 0);
        stream_possible = 0;
        ports_found = 0;
        for (j = 0;j < ipmc_p->num_fp_ports; j++) {
            if((ipmc_p->port_used[j] == 0)
                    && (ipmc_p->port_speed[i]
                                == ipmc_p->port_speed[j])
                    && (i!=j)) {
                ports_found++;
            }

            if (ports_found >= ipmc_p->num_copy) {
                stream_possible = 1;
            }
        }

        ports_accounted_for++;

        if(stream_possible == 1) {
            ipmc_p->port_used[i] = 1;
            ports_picked = 0;
            ipmc_p->stream[ipmc_p->num_streams][0] = i;
            while (ports_picked < ipmc_p->num_copy) {
                do {
                    picked_port = sal_rand() % ipmc_p->num_fp_ports;
                }while((ipmc_p->port_speed[i] !=
                                ipmc_p->port_speed[picked_port])
                        || (ipmc_p->port_used[picked_port] != 0));

                ports_picked++;
                ipmc_p->port_used[picked_port] = 1;
                ipmc_p->stream[ipmc_p->num_streams][ports_picked]
                                                            = picked_port;
                ports_accounted_for++;
            }
            cli_out("\n%0d", ipmc_p->num_streams);
            cli_out("\t");
            for (k = 0; k < (ipmc_p->num_copy + 1); k++) {
                cli_out("%0d", ipmc_p->stream[ipmc_p->num_streams][k]);
                if (k != ipmc_p->num_copy) {
                    cli_out(", ");
                }
            }
            ipmc_p->num_streams++;
        }
    }

    calc_oversub_ratio(unit);

    cli_out("\n====================================");

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nPRINTING PORT SPEED")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================")));

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, port_speed[%0d] = %0d"), i,
                  i, ipmc_p->port_speed[i]));
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================\n")));

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nPRINTING PORT OVERSUB ARRAY")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================")));

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, port_oversub[%0d] = %0d"),
                  i, i, ipmc_p->port_oversub[i]));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================\n")));

    set_exp_rates(unit);

    cli_out("\n");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->l3_copy[i] = 0;
    }

    for (i = 0; i < ipmc_p->num_streams; i++) {
        for (j = 0; j < ipmc_p->num_l3_copy_param; j++) {
            do {
                k = (sal_rand() % ipmc_p->num_copy) + 1;
            } while(ipmc_p->l3_copy[ipmc_p->stream[i][k]] == 1);

            ipmc_p->l3_copy[ipmc_p->stream[i][k]] = 1;
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        if (ipmc_p->l3_copy[i] == 1) {
            if (ipmc_p->num_rep_param == 0) {
                ipmc_p->num_rep[i] = sal_rand()
                                            % (ipmc_p->max_num_rep_param + 1);
            } else {
                ipmc_p->num_rep[i] = ipmc_p->num_rep_param;
            }
        }
        else {
            ipmc_p->num_rep[i] = 0;
        }
    }

    for (i = 0; i < ipmc_p->num_streams; i++) {
        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (ipmc_p->l3_copy[ipmc_p->stream[i][1]] == 0) {
                ipmc_p->num_rep[ipmc_p->stream[i][j]] = 0;
            } else if (ipmc_p->num_rep[ipmc_p->stream[i][j]] >
                            ipmc_p->num_rep[ipmc_p->stream[i][1]]) {
                temp = ipmc_p->num_rep[ipmc_p->stream[i][j]];
                ipmc_p->num_rep[ipmc_p->stream[i][j]]
                                = ipmc_p->num_rep[ipmc_p->stream[i][1]];
                ipmc_p->num_rep[ipmc_p->stream[i][1]] = temp;
            }
        }
    }

    for (i = 0; i < ipmc_p->num_streams; i++) {
        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            cli_out("stream[%0d][%0d] = %0d", i, j, ipmc_p->stream[i][j]);
            if (j < (ipmc_p->num_copy + 1)) {
                cli_out(",");
            }
        }
        cli_out("\n");
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        for (j = 0; j < TARGET_PKT_COUNT; j++) {
            do {
                /* coverity[dont_call : FALSE] */
                pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
            } while (num_cells(unit, pkt_size, ipmc_p->port_list[i]) >
                            ipmc_p->max_num_cells_param);
            ipmc_p->rand_pkt_sizes[i][j] = pkt_size;
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        cli_out("\nl3_copy[%0d] = %0d, num_rep[%0d] = %0d", i,
                ipmc_p->l3_copy[i], i, ipmc_p->num_rep[i]);
    }
}

/*
 * Function:
 *      set_up_streams
 * Purpose:
 *      This function actually sets up the IPMC packet flow. Please refer to
 *      items 4-6 of the "Test Setup" section of the test description at the
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
    int i, j, k, p;
    pbmp_t pbm, ubm;
    uint32 vlan = VLAN;
    uint32 vlan_0 = 0;
    uint32 vlan_1 = 0;
    uint32 vlan_2 = 0;
    uint32 l3_stream;
    bcm_multicast_t mc_group;
    uint32 mc_flags = 0;
    bcm_l3_intf_t l3_intf;
    bcm_if_t intf_id;
    bcm_if_t encap_id;
    uint32 intf_flags = 0;
    uint8 intf_mac[] = {0x00,0x00,0x00,0x00,0x00,0x00};
    uint8 mac_0[NUM_BYTES_MAC_ADDR];
    bcm_ipmc_addr_t ipmc_addr;
    uint32 ipmc_flags = 0;
    uint8 mac_sa[] = MAC_SA;
    bcm_gport_t gport;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    ipmc_p->tx_vlan = (uint32*) sal_alloc(ipmc_p->num_streams * sizeof(uint32),
                                          "tx_vlan");

    ipmc_p->exp_vlan = (uint32*) sal_alloc(ipmc_p->num_streams * sizeof(uint32),
                                          "exp_vlan");

    ipmc_p->exp_mac_addr = sal_alloc(ipmc_p->num_streams *
                                        sizeof(uint8) * NUM_BYTES_MAC_ADDR,
                                                "exp_mac_addr");
    BCM_PBMP_CLEAR(ubm);

    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
    (void) bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) bcm_port_control_set(unit, p, bcmPortControlIP4, TRUE);
        (void) bcm_port_control_set(unit, p, bcmPortControlIP6, TRUE);
        (void) bcm_port_control_set(unit, p, bcmSwitchL3McIdxRetType, TRUE);
        (void) bcm_port_control_set(unit, p,
                                    bcmSwitchIpmcReplicationSharing, TRUE);
    }

    bcm_ipmc_enable(unit, TRUE);

    bcm_vlan_create(unit, (bcm_vlan_t)(DROP_VLAN));

    mc_group = (bcm_multicast_t)(FIRST_MC_GROUP);
    mc_flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID;

    intf_id = (bcm_if_t)(FIRST_INTF_ID);
    intf_flags |= BCM_L3_WITH_ID;

    ipmc_flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;

    for (i = 0; i < ipmc_p->num_streams; i++) {
        if (ipmc_p->l3_copy[ipmc_p->stream[i][1]] == 0) {
            l3_stream = 0;
            cli_out("\nStream %0d is an L2 stream", i);
        } else {
            cli_out("\nStream %0d is an L3 stream", i);
            l3_stream = 1;
        }
        (void) bcm_multicast_create(unit, mc_flags, &mc_group);
        bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
        BCM_PBMP_CLEAR(pbm);
        BCM_PBMP_PORT_ADD(pbm, ipmc_p->port_list[ipmc_p->stream[i][0]]);
        bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
        vlan_0 = vlan;
        vlan++;

        bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
        BCM_PBMP_CLEAR(pbm);
        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (ipmc_p->l3_copy[ipmc_p->stream[i][j]] == 0) {
                BCM_PBMP_PORT_ADD(pbm, ipmc_p->port_list[ipmc_p->stream[i][j]]);
                BCM_GPORT_LOCAL_SET(gport,
                                    ipmc_p->port_list[ipmc_p->stream[i][j]]);
                bcm_multicast_egress_add(unit, mc_group,
                                         gport, (bcm_if_t)(-1));
                if (((j > 0) && (l3_stream == 1))
                    || ((j > 1) && (l3_stream == 0))) {
                    bcm_vlan_translate_add(unit,
                                            ipmc_p->port_list[ipmc_p->stream[i][j]],
                                            (bcm_vlan_t)(vlan),
                                            (bcm_vlan_t)(DROP_VLAN), 0);
                }
            }
        }
        bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
        vlan_1 = vlan;
        vlan++;

        bcm_vlan_translate_add(unit, ipmc_p->port_list[ipmc_p->stream[i][0]],
                               (bcm_vlan_t)(vlan_0), (bcm_vlan_t)(vlan_1), 0);

        if (l3_stream == 0) {
            bcm_vlan_translate_add(unit, ipmc_p->port_list[ipmc_p->stream[i][1]],
                               (bcm_vlan_t)(vlan_1), (bcm_vlan_t)(vlan_0), 0);
        } else {
            bcm_vlan_translate_add(unit, ipmc_p->port_list[ipmc_p->stream[i][1]],
                               (bcm_vlan_t)(vlan_1), (bcm_vlan_t)(DROP_VLAN), 0);
        }

        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (ipmc_p->l3_copy[ipmc_p->stream[i][j]] == 1) {
                for (k = 0;
                     k < (ipmc_p->num_rep[ipmc_p->stream[i][j]] + 1);
                     k++) {
                    BCM_PBMP_CLEAR(pbm);
                    BCM_PBMP_PORT_ADD(pbm,
                                      ipmc_p->port_list[ipmc_p->stream[i][j]]);
                    bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
                    bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
                    intf_mac[NUM_BYTES_MAC_ADDR - 1] = intf_id;
                    if (l3_stream == 1 && j == 1 && k == 0) {
                        vlan_2 = vlan;
                        bcm_vlan_translate_add(unit,
                                            ipmc_p->port_list[ipmc_p->stream[i][1]],
                                            (bcm_vlan_t)(vlan_2),
                                            (bcm_vlan_t)(vlan_0), 0);
                        sal_memcpy(mac_0, intf_mac, 6);
                    }
                    else {
                        bcm_vlan_translate_add(unit,
                                            ipmc_p->port_list[ipmc_p->stream[i][j]],
                                            (bcm_vlan_t)(vlan),
                                            (bcm_vlan_t)(DROP_VLAN), 0);
                    }
                    bcm_l3_intf_t_init(&l3_intf);
                    sal_memcpy(l3_intf.l3a_mac_addr, intf_mac, 6);
                    l3_intf.l3a_vid = vlan;
                    l3_intf.l3a_intf_id = intf_id;
                    l3_intf.l3a_flags = intf_flags;
                    bcm_l3_intf_create(unit, &l3_intf);
                    bcm_multicast_l3_encap_get(unit, mc_group,
                                               ipmc_p->port_list
                                                        [ipmc_p->stream[i][j]],
                                               intf_id, &encap_id);
                    BCM_GPORT_LOCAL_SET(gport,
                                    ipmc_p->port_list[ipmc_p->stream[i][j]]);
                    bcm_multicast_egress_add(unit, mc_group,
                                             gport, encap_id);
                    vlan++;
                    intf_id++;
                }
            }
        }

        bcm_ipmc_addr_t_init(&ipmc_addr);
        ipmc_addr.mc_ip_addr = DIP;
        ipmc_addr.s_ip_addr = SIP;
        ipmc_addr.vid = vlan_1;
        ipmc_addr.group = mc_group;
        ipmc_addr.flags = ipmc_flags;
        bcm_ipmc_add(unit, &ipmc_addr);
        mc_group++;

        ipmc_p->tx_vlan[i] = vlan_0;

        if (l3_stream == 0) {
            ipmc_p->exp_vlan[i] = vlan_1;
            sal_memcpy(ipmc_p->exp_mac_addr[i], mac_sa, 6);
        } else {
            ipmc_p->exp_vlan[i] = vlan_2;
            sal_memcpy(ipmc_p->exp_mac_addr[i], mac_0, 6);
        }
    }
}

/*
 * Function:
 *      ipmc_dma_chan_check_done
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
ipmc_dma_chan_check_done(int unit, int vchan, soc_dma_poll_type_t type,
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
    soc_info_t *si = &SOC_INFO(unit);

    if (pkt_size > 4608) {
        flood_cnt = TARGET_PKT_COUNT - ((TH_FREQ_850 - si->frequency)/20);
    } else {
        flood_cnt = TARGET_PKT_COUNT;
    }

    return(flood_cnt);
}

/*
 * Function:
 *      ipmc_gen_random_ip_pkt
 * Purpose:
 *      Generate random L3 packet with seq ID
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
ipmc_gen_random_ip_pkt(int unit, uint8 *pkt_ptr, uint32 pkt_size,
                       uint8 mac_da[NUM_BYTES_MAC_ADDR],
                       uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                       uint16 vlan_id, uint32 ip_da, uint32 ip_sa, uint32 seq_id)
{
    int i, offset;
    uint32 crc;

    tgp_gen_random_ip_pkt(0, pkt_ptr, pkt_size, mac_da, mac_sa,
                          vlan_id, ip_da, ip_sa, TTL);

    /* Replace First 4 Payload Bytes with Sequence ID */
    offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV4_HDR;

    for (i = 3; i >= 0; i--) {
        pkt_ptr[offset++] = (seq_id >> (i * 8)) & 0x000000ff;
    }

    /* Replace Next 2  Payload Bytes with Packet Size */
    for (i = 1; i >= 0; i--) {
        pkt_ptr[offset++] = (pkt_size >> (i * 8)) & 0x00ff;
    }

    /* Recalculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    offset = pkt_size - NUM_BYTES_CRC;
    for (i = NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }
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
    ipmc_t *ipmc_p = ipmc_parray[unit];

    soc_dma_init(unit);

    dv_tx = soc_dma_dv_alloc(unit, DV_TX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nSending packets ...");

    for (i = 0; i < ipmc_p->num_streams; i++) {
        if (ipmc_p->pkt_size_param == 0) {
            pkt_size = ENET_WC_PKT_SIZE;
        } else {
            pkt_size = ipmc_p->pkt_size_param;
        }

        if (ipmc_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, ipmc_p->stream[i][0]);
        } else {
        flood_cnt = ipmc_p->flood_pkt_cnt_param;
        }

        if (pkt_size == 1) {
            use_random_packet_sizes = 1;
        }

        cli_out("\nflood_cnt for stream %0d = %0d", i, flood_cnt);

        num_pkts_tx = flood_cnt;

        if ((num_pkts_tx > MAX_PKTS_PER_STREAM) &&
            (ipmc_p->flood_pkt_cnt_param == 0)) {
            num_pkts_tx = MAX_PKTS_PER_STREAM;
        }
        for (j = 0; j < num_pkts_tx; j++) {
            if (use_random_packet_sizes == 1) {
                pkt_size = ipmc_p->rand_pkt_sizes[i][j];
            }
            packet_store_ptr =
                sal_dma_alloc(pkt_size * sizeof(uint8), "packet");

            pkt_count++;
            channel_done = 0;
            soc_dma_abort_dv(unit, dv_tx);
            sal_srand(ipmc_p->pkt_seed + i + j);
            ipmc_gen_random_ip_pkt(unit, packet_store_ptr, pkt_size, mac_da,
                                        mac_sa,
                                        (uint16)(ipmc_p->tx_vlan[i]),
                                        DIP, SIP, j);
            soc_dma_dv_reset(DV_TX, dv_tx);
            soc_dma_desc_add(dv_tx, (sal_vaddr_t) (packet_store_ptr), pkt_size,
                             lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
            soc_dma_desc_end_packet(dv_tx);
            soc_dma_chan_config(unit, TX_CHAN, DV_TX, SOC_DMA_F_POLL);
            soc_dma_start(unit, TX_CHAN, dv_tx);

            while (channel_done == 0) {
                ipmc_dma_chan_check_done(unit, TX_CHAN,
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
 *      set_up_ports
 * Purpose:
 *      Disable TTL decrement and TTL check. This makes sure the stream does not
 *      die out.
 *
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
    int p;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        soc_reg_field32_modify(unit, EGR_IPMC_CFG2r, p,
                            DISABLE_TTL_DECREMENTf, 0x1);
        soc_reg_field32_modify(unit, EGR_IPMC_CFG2r, p,
                            DISABLE_TTL_CHECKf, 0x1);
    }
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
    ipmc_t *ipmc_p = ipmc_parray[unit];
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
                                    RPORTDr
                                  };

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        for (j = 0; j < (sizeof(error_counters) / sizeof(soc_reg_t)); j++) {
            (void) soc_reg_get(unit, error_counters[j], ipmc_p->port_list[i], 0,
                        &rdata);
            if (!(COMPILER_64_IS_ZERO(rdata))) {
                test_error (unit, "\n*ERROR: Error counter %s has a non zero value "
                            "for device port %0d",
                            SOC_REG_NAME(unit, error_counters[j]),
                            ipmc_p->port_list[i]);
                ipmc_p->test_fail = 1;
            }
        }
    }
}

/*
 * Function:
 *      check_mib_counters
 * Purpose:
 *      IPMC test end check. This function basically checks the packet counters
 *      for each port to make sure each port that should receive packets does
 *      receive them and that the packet counters are proportional to the number
 *      of replications.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      run_time - Time in seconds for which packets are allowed to swill.
 * Returns:
 *     Nothing
 *
 */

static void
ipmc_test_check(int unit, uint32 run_time)
{
    int p;
    int i, j;
    uint64 rdata;
    uint64 tpkt_delta_src_port;
    uint64 tpkt_delta;
    uint64 tpkt_delta_expected;
    uint64 max_tpkt_delta;
    uint64 min_tpkt_delta;
    uint64 margin_of_error;
    uint32 tolerance;
    uint64 hundred_64;
    uint32 fail = 0;
    char tpkt_delta_str[32], min_tpkt_delta_str[32], max_tpkt_delta_str[32];

    ipmc_t *ipmc_p = ipmc_parray[unit];

    COMPILER_64_SET(hundred_64, 0, 100);

    tolerance = PKT_COUNT_CHECK_TOL;

    cli_out("\nDoing Packet Counter checks");

    ipmc_p->tpkt_start =
        (uint64 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint64),
                             "tpkt_start");
    ipmc_p->tpkt_end =
        (uint64 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint64),
                             "tpkt_end");

    cli_out("\nWait 2s for traffic to stabilize");
    sal_usleep(2000000);

    cli_out("\nSwilling Packets over a period of %0ds", run_time);

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) soc_reg_get(unit, TPKTr, p, 0, &rdata);
        COMPILER_64_SET(ipmc_p->tpkt_start[i], COMPILER_64_HI(rdata),
                        COMPILER_64_LO(rdata));
        i++;
    }

    sal_usleep(run_time * 1000000);

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) soc_reg_get(unit, TPKTr, p, 0, &rdata);
        COMPILER_64_SET(ipmc_p->tpkt_end[i], COMPILER_64_HI(rdata),
                        COMPILER_64_LO(rdata));
        i++;
    }

    for (i = 0; i < ipmc_p->num_streams; i++) {
        COMPILER_64_DELTA(tpkt_delta_src_port,
                          ipmc_p->tpkt_start[ipmc_p->stream[i][0]],
                          ipmc_p->tpkt_end[ipmc_p->stream[i][0]]);

        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            COMPILER_64_DELTA(tpkt_delta,
                              ipmc_p->tpkt_start[ipmc_p->stream[i][j]],
                              ipmc_p->tpkt_end[ipmc_p->stream[i][j]]);
            COMPILER_64_SET(tpkt_delta_expected,
                            COMPILER_64_HI(tpkt_delta_src_port),
                            COMPILER_64_LO(tpkt_delta_src_port));
            COMPILER_64_UMUL_32(tpkt_delta_expected,
                                (ipmc_p->num_rep[ipmc_p->stream[i][j]] + 1));
            COMPILER_64_SET(margin_of_error,
                            COMPILER_64_HI(tpkt_delta_expected),
                            COMPILER_64_LO(tpkt_delta_expected));
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);

            COMPILER_64_DELTA(min_tpkt_delta, margin_of_error,
                              tpkt_delta_expected);
            COMPILER_64_SET(max_tpkt_delta, COMPILER_64_HI(tpkt_delta_expected),
                            COMPILER_64_LO(tpkt_delta_expected));
            COMPILER_64_ADD_64(max_tpkt_delta, margin_of_error);

            cli_out("\n");
            if ((COMPILER_64_LT(tpkt_delta, min_tpkt_delta))
                || (COMPILER_64_GT(tpkt_delta, max_tpkt_delta))) {
                cli_out("*ERROR: ");
                fail = 1;
            }

            format_uint64_decimal(tpkt_delta_str, tpkt_delta, 0);
            format_uint64_decimal(min_tpkt_delta_str, min_tpkt_delta, 0);
            format_uint64_decimal(max_tpkt_delta_str, max_tpkt_delta, 0);

            cli_out
                ("Test Port = %0d, Device Port = %0d, PktCnt = %s, "
                "MinPktCnt  = %s, MaxPktCnt = %s",
                ipmc_p->stream[i][j], ipmc_p->port_list[ipmc_p->stream[i][j]],
                tpkt_delta_str, min_tpkt_delta_str, max_tpkt_delta_str);
        }
    }
    if (fail == 1) {
        test_error(unit,
                   "\n*************** COUNTER CHECKS FAILED ***************\n");
        ipmc_p->test_fail = 1;
    } else {
        cli_out("\n**************** COUNTER CHECKS PASSED ****************\n");
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
    uint32 num_pkts_rx;
    uint32 seq_id_offset;
    uint32 zero_crc = 1;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    seq_id_offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV4_HDR;

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

    for (i = 0; i < ipmc_p->num_streams; i++) {
        if (ipmc_p->pkt_size_param == 0) {
            pkt_size = ENET_WC_PKT_SIZE;
        } else {
            pkt_size = ipmc_p->pkt_size_param;
        }

        if (ipmc_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, ipmc_p->stream[i][0]);
        } else {
        flood_cnt = ipmc_p->flood_pkt_cnt_param;
        }
        num_pkts_rx = flood_cnt;

        if ((num_pkts_rx > MAX_PKTS_PER_STREAM) &&
            (ipmc_p->flood_pkt_cnt_param == 0)) {
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

            invalidate_vlan(unit, ipmc_p->tx_vlan[i]);

            while (channel_done == 0 && timeout > 0) {
                ipmc_dma_chan_check_done(unit, RX_CHAN,
                                              SOC_DMA_POLL_CHAIN_DONE,
                                              &channel_done);
                timeout--;
            }

            if (timeout > 0) {
                pkt_count++;
                pkt_count_port++;
                seq_id = 0x00000000;
                pkt_size = 0x00000000;

                seq_id |= (rx_pkt_ptr[seq_id_offset] << 24);
                seq_id |= (rx_pkt_ptr[seq_id_offset + 1] << 16);
                seq_id |= (rx_pkt_ptr[seq_id_offset + 2] << 8);
                seq_id |= (rx_pkt_ptr[seq_id_offset + 3]);
                pkt_size |= (rx_pkt_ptr[seq_id_offset + 4] << 8);
                pkt_size |= (rx_pkt_ptr[seq_id_offset + 5]);

                sal_srand(ipmc_p->pkt_seed + i + seq_id);
                ipmc_gen_random_ip_pkt(unit, ref_pkt_ptr, pkt_size, mac_da,
                                            ipmc_p->exp_mac_addr[i],
                                            (uint16)(ipmc_p->exp_vlan[i]),
                                            DIP, SIP, seq_id);

                for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                    if (rx_pkt_ptr[k] != ref_pkt_ptr[k]) {
                        match = 0;
                        test_error(unit, "\n*ERROR: Stream %0d "
                                         "has packet corruption"
                                         " on received pkt %0d",
                                          i, j);
                        ipmc_p->test_fail = 1;
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
                                        i, ipmc_p->port_list[i], j);
                    ipmc_p->test_fail = 1;
                }

                zero_crc = 1;
            }

            timeout = RXDMA_TIMEOUT;
            sal_dma_free(rx_pkt_ptr);
            sal_dma_free(ref_pkt_ptr);
        }
        cli_out("\n%0d Packets received", pkt_count);

        if (ipmc_p->oversub_config == 0) {
            if (pkt_count_port < num_pkts_rx) {
                test_error(unit, "\n*ERROR: Stream %0d expected %0d pkts, "
                                "received %0d packets",
                                i, num_pkts_rx,
                                pkt_count_port);
                ipmc_p->test_fail = 1;
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
 *      ipmc_free_all_memory
 * Purpose:
 *      Free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
ipmc_free_all_memory(int unit)
{
    int i;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    sal_free(ipmc_p->port_speed);
    sal_free(ipmc_p->port_list);
    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        sal_free(ipmc_p->stream[i]);
    }
    sal_free(ipmc_p->stream);
    sal_free(ipmc_p->num_rep);
    sal_free(ipmc_p->l3_copy);
    sal_free(ipmc_p->tx_vlan);
    sal_free(ipmc_p->exp_vlan);
    sal_free(ipmc_p->exp_mac_addr);
    sal_free(ipmc_p->port_oversub);
    sal_free(ipmc_p->port_used);
    sal_free(ipmc_p->ovs_ratio_x1000);
    sal_free(ipmc_p->exp_rate);
    sal_free(ipmc_p->tpkt_start);
    sal_free(ipmc_p->tpkt_end);
    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        sal_free(ipmc_p->rand_pkt_sizes[i]);
    }
    sal_free(ipmc_p->rand_pkt_sizes);
}

/*
 * Function:
 *      ipmc_test_init
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
ipmc_test_init(int unit, args_t *a, void **pa)
{
    ipmc_t *ipmc_p;

    ipmc_p = ipmc_parray[unit];

    ipmc_p = sal_alloc(sizeof(ipmc_t), "ipmc_test");
    sal_memset(ipmc_p, 0, sizeof(ipmc_t));
    ipmc_parray[unit] = ipmc_p;
    cli_out("\nCalling ipmc_test_init");
    ipmc_parse_test_params(unit, a);

    ipmc_p->test_fail = 0;

    if (ipmc_p->bad_input == 1) {
        goto done;
    }

    ipmc_soc_set_up_mac_lpbk(unit);
    ipmc_turn_off_cmic_mmu_bkp(unit);
    ipmc_soc_turn_off_fc(unit);
    /* coverity[dont_call : FALSE] */
    ipmc_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      ipmc_test
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
ipmc_test(int unit, args_t *a, void *pa)
{
    ipmc_t *ipmc_p;

    ipmc_p = ipmc_parray[unit];

    if (ipmc_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling ipmc_test");
    set_port_property_arrays(unit);
    set_up_ports(unit);
    set_up_streams(unit);
    send_pkts(unit);

done:
    return 0;
}

/*
 * Function:
 *      ipmc_test_cleanup
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
ipmc_test_cleanup(int unit, void *pa)
{
    ipmc_t *ipmc_p;
    int rv;

    ipmc_p = ipmc_parray[unit];

    if (ipmc_p->bad_input == 1) {
        goto done;
    }
    cli_out("\nCalling ipmc_test_cleanup");

    ipmc_test_check(unit, ipmc_p->run_time_param);
    check_mib_counters(unit);

    check_packet_integrity(unit);

done:
    if (ipmc_p->bad_input == 1) {
        ipmc_p->test_fail = 1;
    }

    if (ipmc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }
    else {
        rv = BCM_E_NONE;
    }

    ipmc_free_all_memory(unit);
    sal_free(ipmc_p);

    cli_out("\n");

    return rv;
}

#endif /* BCM_ESW_SUPPORT && INCLUDE_L3 */
