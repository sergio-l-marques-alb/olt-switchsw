/*
 * $Id: pktdma_soc_test.c,v 1.0 Broadcom SDK $
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
 * CMIC Packet DMA performance test.
 * This test is intended to test the CMIC packet DMA performance. It can also
 * also be used as a feature test. The test can simultaneously exercise multiple
 * TX and RX channels across multiple CMCs.
 *
 * Parameters passed from the CLI:
 * TxBitmap : 12-bit bitmap of TX channels. For example 0x333 signifies channels
 *            0 and 1 of all 3 CMCs.
 * RxBitmap : 12-bit bitmap of RX channels. For example 0xccc signifies channels
 *            2 and 3 of all 3 CMCs. Please note that the Tx and Rx bitmaps
 *            should be mutually exclusive and there should be at
 *            least 1 Tx channel enabled if Rx is enabled.
 * PktSize :  Packet size in bytes. Use 0 for random packet sizes.
 * StreamTx : Continuously stream packets on TX.
 * StreamRx : Continuously stream packets on RX.
 * NumPktsTx : Number of packets for which descriptors are set up at one time.
 *             If StreamTx=0, this is the number of packets transmitted. If
 *             chaining is enabled by the TestTypeTx parameter, this is also the
 *             number of packets-per-chain.
 * NumPktsRx : Number of packets for which descriptors are set up at one time.
 *             If StreamRx=0, this is the number of packets transmitted. If
 *             chaining is enabled by the TestTypeRx parameter, this is also the
 *             number of packets-per-chain.
 * TestTypeTx : Used to set the descriptor type or test type.
 *              0: Simple Packet DMA: Each descriptor points to a single packet
 *              1: Scatter/Gather without packet chaining: Each packet is split
 *                 among 2 descriptors. However there is only 1 packet per chain.
 *              2: Chained DMA: Each descriptor points to 1 packet. However we
 *                 have descriptor chaining. Number of packets per chain is
 *                 specified by NumPktsTx.
 *              3: Scatter/Gather with packet chaining: Each packet split among
 *                 2 descriptors. Number of packets per chain specified by
 *                 NumPktsTx.
 *              4: Reload in infinite loop: Reload descriptor added at the end
 *                 that points to the top of the descriptor chain whose length
 *                 is specified by NumPktsTx. This creates an infinite HW loop.
 *                 This is very useful for measuring the raw HW performance of
 *                 the DMA engine..
 * TestTypeRx : Used to set the descriptor type.
 *              0: Simple Packet DMA: Each descriptor points to a single packet
 *              1: Scatter/Gather without packet chaining: Each packet is split
 *                 among 2 descriptors. However there is only 1 packet per chain.
 *              2: Chained DMA: Each descriptor points to 1 packet. However we
 *                 have descriptor chaining. Number of packets per chain
 *                 specified by NumPktsRx.
 *              3: Scatter/Gather with packet chaining: Each packet is split
 *                 among 2 descriptors. Number of packets per chain specified by
 *                 NumPktsRx.
 *              4: Reload in infinite loop: Reload descriptor added at the end
 *                 that points to the top of the descriptor chain whose length
 *                 is specified by NumPktsRx. This creates an infinite HW loop.
 *                 This is very useful for measuring the raw HW performance of
 *                 the DMA engine.
 * ChkPktInteg : Enable packet integrity checks
 * RateCalcInt : Interval in seconds over which TX/RX rates are calculated.
 * PollIntr : Set to 0 for polling mode, 1 to enable chain done interrupts and
 *            2 to enable both descriptor done and chain done interrupts
 * Verbose : Print descriptors and packet pointers
 * SwThOff: Turn off memscan, linkscan and counter collection threads
 * SkipCosBmp: Skip COS Ctrl bitmap programming in CMIC
 * SVOverride: Special override for internal SV testing
 * PktChkInt: Interval between successive packet content checks in streaming
 *            mode. It is specified in terms of NumPktsRx. For example setting
 *            this parameterto 100 with NumPktsRx=20 means 20 pkts will be
 *            checked for content every 20*100=2000 received packets.
 * MaxPktSize: Max packet size if random packet sizes are used (PktSize=0).
 * NumLbPorts: Number of front panel ports used for looping back packets if
 *             streaming enabled.
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <sal/core/alloc.h>
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

#define MAC_DA {0x12,0x34,0x56,0x78,0x9a,0xbc}
#define MAC_SA {0xfe,0xdc,0xba,0x98,0x76,0x54}
#define TPID 0x8100
#define VLAN 0x0abc
#define XLATE_VLAN 0xdef

#define DESC_TX_SINGLE_PKT 0x0
#define DESC_TX_SINGLE_PKT_SPLIT 0x1
#define DESC_TX_CHAIN_PKT 0x2
#define DESC_TX_CHAIN_PKT_SPLIT 0x3
#define DESC_TX_CHAIN_PKT_RELOAD 0x4
#define DESC_RX_SINGLE_PKT 0x0
#define DESC_RX_SINGLE_PKT_SPLIT 0x1
#define DESC_RX_CHAIN_PKT 0x2
#define DESC_RX_CHAIN_PKT_SPLIT 0x3
#define DESC_RX_CHAIN_PKT_RELOAD 0x4
#define NUM_BYTES_PER_DESC 64
#define MAX_MTU_CHAIN_LENGTH 30
#define MAX_DESC_CHAIN_LENGTH 150
#define MTU 9216
#define MAX_SPLITS 4
#define MAX_CHANNELS 12
#define POLL 0
#define CHAIN_DONE_INTR_ONLY 1
#define BOTH_DESC_CHAIN_INTR 2
#define ENET_IPG 12
#define ENET_PREAMBLE 8

#define TX_BITMAP_PARAM_DEFAULT 0x001
#define RX_BITMAP_PARAM_DEFAULT 0x002
#define PKT_SIZE_PARAM_DEFAULT 250
#define STREAM_PARAM_DEFAULT_TX 0
#define STREAM_PARAM_DEFAULT_RX 0
#define NUM_PKTS_PARAM_DEFAULT_TX 2
#define NUM_PKTS_PARAM_DEFAULT_RX 2
#define TEST_TYPE_PARAM_DEFAULT_TX 0x2
#define TEST_TYPE_PARAM_DEFAULT_RX 0x2
#define DISABLE_RX_FC_PARAM_DEFAULT 0x1
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 0x0;
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10;
#define POLL_INTR_PARAM_DEFAULT 0
#define VERBOSE_PARAM_DEFAULT 0
#define SW_THREADS_OFF_PARAM_DEFAULT 1
#define SKIP_COS_CTRL_BMP_PARAM_DEFAULT 0
#define SV_OVERRIDE_PARAM_DEFAULT 0
#define PKT_CHK_INT_PARAM_DEFAULT 100
#define MAX_PKT_SIZE_PARAM_DEFAULT 9216
#define NUM_LB_PORTS_PARAM_DEFAULT 20

#define NUM_CONT_DMA_DV 12
#define CH0_IN_HALT 27

#define MAX_COS 16
#define MIN_PKT_SIZE 64

#define NUM_SUBP_OBM 4
#define NUM_BYTES_MAC_ADDR 6
#define NUM_BYTES_CRC 4

#define TD2P_PGWS_PER_DEV 8

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT)

typedef void (*isr_chain_done_t) (int, dv_t *);
typedef void (*isr_reload_done_t) (int, dv_t *);
typedef void (*isr_desc_done_t) (int, dv_t *, dcb_t *);

typedef struct pktdma_s {
    uint32 tx_bitmap_param;
    uint32 rx_bitmap_param;
    uint32 pkt_size_param;
    uint32 stream_param_tx;
    uint32 stream_param_rx;
    uint32 num_pkts_param_tx;
    uint32 num_pkts_param_rx;
    uint32 test_type_param_tx;
    uint32 test_type_param_rx;
    uint32 check_packet_integrity_param;
    uint32 rate_calc_interval_param;
    uint32 poll_intr_param;
    uint32 verbose_param;
    uint32 sw_threads_off_param;
    uint32 skip_cos_ctrl_bmp_param;
    uint32 sv_override_param;
    uint32 pkt_chk_int_param;
    uint32 max_pkt_size_param;
    uint32 num_lb_ports_param;
    uint8 ***pkt_tx_array;
    uint8 ***pkt_rx_array;
    uint32 desc_done_count_tx[MAX_CHANNELS];
    uint32 desc_done_count_rx[MAX_CHANNELS];
    uint32 chain_done_count_tx[MAX_CHANNELS];
    uint32 chain_done_count_rx[MAX_CHANNELS];
    uint32 reload_done_count_tx[MAX_CHANNELS];
    uint32 reload_done_count_rx[MAX_CHANNELS];
    uint32 chain_done_tx[MAX_CHANNELS];
    uint32 chain_done_rx[MAX_CHANNELS];
    uint32 reload_done_tx[MAX_CHANNELS];
    uint32 reload_done_rx[MAX_CHANNELS];
    uint32 **random_packet_sizes;
    uint32 source_ch[MAX_CHANNELS];
    uint32 kill_dma;
    uint32 chain_tx;
    uint32 sg_tx;
    uint32 reload_tx;
    uint32 chain_rx;
    uint32 sg_rx;
    uint32 reload_rx;
    uint32 *cmic_tx_counters;
    uint32 *cmic_rx_counters;
    uint32 cont_dma;
    uint32 pkt_seed;
    isr_desc_done_t desc_done_intr_tx_table[MAX_CHANNELS];
    isr_desc_done_t desc_done_intr_rx_table[MAX_CHANNELS];
    isr_chain_done_t chain_done_intr_tx_table[MAX_CHANNELS];
    isr_chain_done_t chain_done_intr_rx_table[MAX_CHANNELS];
    isr_chain_done_t reload_done_intr_tx_table[MAX_CHANNELS];
    isr_chain_done_t reload_done_intr_rx_table[MAX_CHANNELS];
    sal_thread_t pid_tx;
    sal_thread_t pid_rx;
    dv_t *dv_tx;
    dv_t *dv_rx;
    dv_t ***dv_tx_array[NUM_CONT_DMA_DV];
    dv_t ***dv_rx_array[NUM_CONT_DMA_DV];
    uint32 bad_input;
    int test_fail;
    uint32 tx_thread_done;
    uint32 rx_thread_done;
} pktdma_t;

static pktdma_t *pktdma_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      pktdma_soc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      pktdma_p->bad_input set from here - tells test to crash out in case CLI
 *      input combination is invalid.
 */

static void
pktdma_soc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    pktdma_t *pktdma_p = pktdma_parray[unit];
    int32 min_pkt_size = MIN_PKT_SIZE;
    int32 num_tx_channels_active = 0;
    int32 num_rx_channels_active = 0;
    int32 i;

    char tr500_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "CMIC Packet DMA performance test.\n"
    "This test is intended to test the CMIC packet DMA performance. It can also\n"
    "also be used as a feature test. The test can simultaneously exercise multiple\n"
    "TX and RX channels across multiple CMCs.\n"
    "\n"
    "Parameters passed from the CLI:\n"
    "TxBitmap : 12-bit bitmap of TX channels. For example 0x333 signifies channels\n"
    "           0 and 1 of all 3 CMCs.\n"
    "RxBitmap : 12-bit bitmap of RX channels. For example 0xccc signifies channels\n"
    "           2 and 3 of all 3 CMCs. Please note that the Tx and Rx bitmaps\n"
    "           should be mutually exclusive and there should be at\n"
    "           least 1 Tx channel enabled if Rx is enabled.\n"
    "PktSize :  Packet size in bytes. Use 0 for random packet sizes.\n"
    "StreamTx : Continuously stream packets on TX.\n"
    "StreamRx : Continuously stream packets on RX.\n"
    "NumPktsTx : Number of packets for which descriptors are set up at one time.\n"
    "            If StreamTx=0, this is the number of packets transmitted. If\n"
    "            chaining is enabled by the TestTypeTx parameter, this is also the\n"
    "            number of packets-per-chain.\n"
    "NumPktsRx : Number of packets for which descriptors are set up at one time.\n"
    "            If StreamRx=0, this is the number of packets transmitted. If\n"
    "            chaining is enabled by the TestTypeRx parameter, this is also the\n"
    "            number of packets-per-chain.\n"
    "TestTypeTx : Used to set the descriptor type or test type.\n"
    "             0: Simple Packet DMA: Each descriptor points to a single packet\n"
    "             1: Scatter/Gather without packet chaining: Each packet is split\n"
    "                among 2 descriptors. However there is only 1 packet per chain.\n"
    "             2: Chained DMA: Each descriptor points to 1 packet. However we\n"
    "                have descriptor chaining. Number of packets per chain is\n"
    "                specified by NumPktsTx.\n"
    "             3: Scatter/Gather with packet chaining: Each packet split among\n"
    "                2 descriptors. Number of packets per chain specified by\n"
    "                NumPktsTx.\n"
    "             4: Reload in infinite loop: Reload descriptor added at the end\n"
    "                that points to the top of the descriptor chain whose length\n"
    "                is specified by NumPktsTx. This creates an infinite HW loop.\n"
    "                This is very useful for measuring the raw HW performance of\n"
    "                the DMA engine..\n"
    "TestTypeRx : Used to set the descriptor type.\n"
    "             0: Simple Packet DMA: Each descriptor points to a single packet\n"
    "             1: Scatter/Gather without packet chaining: Each packet is split\n"
    "                among 2 descriptors. However there is only 1 packet per chain.\n"
    "             2: Chained DMA: Each descriptor points to 1 packet. However we\n"
    "                have descriptor chaining. Number of packets per chain\n"
    "                specified by NumPktsRx.\n"
    "             3: Scatter/Gather with packet chaining: Each packet is split\n"
    "                among 2 descriptors. Number of packets per chain specified by\n"
    "                NumPktsRx.\n"
    "             4: Reload in infinite loop: Reload descriptor added at the end\n"
    "                that points to the top of the descriptor chain whose length\n"
    "                is specified by NumPktsRx. This creates an infinite HW loop.\n"
    "                This is very useful for measuring the raw HW performance of\n"
    "                the DMA engine.\n"
    "ChkPktInteg : Enable packet integrity checks\n"
    "RateCalcInt : Interval in seconds over which TX/RX rates are calculated.\n"
    "PollIntr : Set to 0 for polling mode, 1 to enable chain done interrupts and\n"
    "           2 to enable both descriptor done and chain done interrupts\n"
    "Verbose : Print descriptors and packet pointers\n"
    "SwThOff: Turn off memscan, linkscan and counter collection threads\n"
    "SkipCosBmp: Skip COS Ctrl bitmap programming in CMIC\n"
    "SVOverride: Special override for internal SV testing\n"
    "PktChkInt: Interval between successive packet content checks in streaming\n"
    "           mode. It is specified in terms of NumPktsRx. For example setting\n"
    "           this parameterto 100 with NumPktsRx=20 means 20 pkts will be\n"
    "           checked for content every 20100=2000 received packets.\n"
    "MaxPktSize: Max packet size if random packet sizes are used (PktSize=0).\n"
    "NumLbPorts: Number of front panel ports used for looping back packets if\n"
    "            streaming enabled.\n";
#endif
    pktdma_p->bad_input = 0;

    /*Assign Default values */
    pktdma_p->tx_bitmap_param = TX_BITMAP_PARAM_DEFAULT;
    pktdma_p->rx_bitmap_param = RX_BITMAP_PARAM_DEFAULT;
    pktdma_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    pktdma_p->stream_param_tx = STREAM_PARAM_DEFAULT_TX;
    pktdma_p->stream_param_rx = STREAM_PARAM_DEFAULT_RX;
    pktdma_p->num_pkts_param_tx = NUM_PKTS_PARAM_DEFAULT_TX;
    pktdma_p->num_pkts_param_rx = NUM_PKTS_PARAM_DEFAULT_RX;
    pktdma_p->test_type_param_tx = TEST_TYPE_PARAM_DEFAULT_TX;
    pktdma_p->test_type_param_rx = TEST_TYPE_PARAM_DEFAULT_RX;
    pktdma_p->check_packet_integrity_param =
        CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    pktdma_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    pktdma_p->poll_intr_param = POLL_INTR_PARAM_DEFAULT;
    pktdma_p->verbose_param = VERBOSE_PARAM_DEFAULT;
    pktdma_p->sw_threads_off_param = SW_THREADS_OFF_PARAM_DEFAULT;
    pktdma_p->skip_cos_ctrl_bmp_param = SKIP_COS_CTRL_BMP_PARAM_DEFAULT;
    pktdma_p->sv_override_param = SV_OVERRIDE_PARAM_DEFAULT;
    pktdma_p->pkt_chk_int_param = PKT_CHK_INT_PARAM_DEFAULT;
    pktdma_p->max_pkt_size_param = MAX_PKT_SIZE_PARAM_DEFAULT;
    pktdma_p->num_lb_ports_param = NUM_LB_PORTS_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "TxBitmap", PQ_HEX|PQ_DFL, 0,
                    &(pktdma_p->tx_bitmap_param), NULL);
    parse_table_add(&parse_table, "RxBitmap", PQ_HEX|PQ_DFL, 0,
                    &(pktdma_p->rx_bitmap_param), NULL);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "StreamTx", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->stream_param_tx), NULL);
    parse_table_add(&parse_table, "StreamRx", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->stream_param_rx), NULL);
    parse_table_add(&parse_table, "NumPktsTx", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->num_pkts_param_tx), NULL);
    parse_table_add(&parse_table, "NumPktsRx", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->num_pkts_param_rx), NULL);
    parse_table_add(&parse_table, "TestTypeTx", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->test_type_param_tx), NULL);
    parse_table_add(&parse_table, "TestTypeRx", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->test_type_param_rx), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "PollIntr", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->poll_intr_param), NULL);
    parse_table_add(&parse_table, "Verbose", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->verbose_param), NULL);
    parse_table_add(&parse_table, "SwThOff", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->sw_threads_off_param), NULL);
    parse_table_add(&parse_table, "SkipCosBmp", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->skip_cos_ctrl_bmp_param), NULL);
    parse_table_add(&parse_table, "SVOverride", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->sv_override_param), NULL);
    parse_table_add(&parse_table, "PktChkInt", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->pkt_chk_int_param), NULL);
    parse_table_add(&parse_table, "MaxPktSize", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->max_pkt_size_param), NULL);
    parse_table_add(&parse_table, "NumLbPorts", PQ_INT|PQ_DFL, 0,
                    &(pktdma_p->num_lb_ports_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg(tr500_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        pktdma_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
        cli_out("\ntx_bitmap_param = 0x%03x", pktdma_p->tx_bitmap_param);
        cli_out("\nrx_bitmap_param = 0x%03x", pktdma_p->rx_bitmap_param);
        cli_out("\npkt_size_param = %0d", pktdma_p->pkt_size_param);
        cli_out("\nstream_param_tx = %0d", pktdma_p->stream_param_tx);
        cli_out("\nstream_param_rx = %0d", pktdma_p->stream_param_rx);
        cli_out("\nnum_pkts_param_tx = %0d", pktdma_p->num_pkts_param_tx);
        cli_out("\nnum_pkts_param_rx = %0d", pktdma_p->num_pkts_param_tx);
        cli_out("\ntest_type_param_tx = %0d", pktdma_p->test_type_param_tx);
        cli_out("\ntest_type_param_rx = %0d", pktdma_p->test_type_param_rx);
        cli_out("\ncheck_packet_integrity_param = %0d",
                pktdma_p->check_packet_integrity_param);
        cli_out("\nrate_calc_interval_param = %0d",
                pktdma_p->rate_calc_interval_param);
        cli_out("\npoll_intr_param = %0d", pktdma_p->poll_intr_param);
        cli_out("\nverbose_param = %0d", pktdma_p->verbose_param);
        cli_out("\nsw_threads_off_param = %0d", pktdma_p->sw_threads_off_param);
        cli_out("\nskip_cos_ctrl_bmp_param = %0d",
                pktdma_p->skip_cos_ctrl_bmp_param);
        cli_out("\nsv_override_param = %0d", pktdma_p->sv_override_param);
        cli_out("\npkt_chk_int_param = %0d", pktdma_p->pkt_chk_int_param);
        cli_out("\nmax_pkt_size_param = %0d", pktdma_p->max_pkt_size_param);
        cli_out("\nnum_lb_ports_param = %0d", pktdma_p->num_lb_ports_param);
        cli_out("\n -----------------------------------------------------");
    }

    if (pktdma_p->cont_dma == 1) {
        if ((!
             (pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT
              || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_SPLIT))
            ||
            (!(pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT
               || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_SPLIT))) {
            pktdma_p->bad_input = 1;
            test_error
                (unit, ("\n*ERROR: Only TestTypeTx/Rx= 2 or 3 supported by test"
                 " in Cont DMA mode\n"));
        }
    }

    if (pktdma_p->stream_param_rx == 0
        && pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
        test_error
            (unit,
             "\n*WARNING: TestTypeRx=4 sets up an infinite descriptor loop"
             " in hardware. Test will be treated as a performance test"
             " even though StreamTx=0\n");
    }

    if (pktdma_p->stream_param_tx == 0
        && pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD) {
        test_error
            (unit,
             "\n*WARNING: TestTypeTx=4 sets up an infinite descriptor loop in"
             " hardware. Test will be treated as a performance test"
             " even though StreamRx=0\n");
    }

    if ((pktdma_p->stream_param_tx == 0)
        && (pktdma_p->test_type_param_tx != DESC_TX_CHAIN_PKT_RELOAD)
        && (pktdma_p->rx_bitmap_param != 0)) {
        if (pktdma_p->stream_param_rx == 1
            || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
            pktdma_p->bad_input = 1;
            test_error(unit,
                       "\n*ERROR: Streaming enabled on RX but not on TX\n");
        }
    }

    if (pktdma_p->pkt_size_param == 0) {
        cli_out("\nUsing random packet sizes");
        if (pktdma_p->max_pkt_size_param > MTU) {
            pktdma_p->bad_input = 1;
            test_error(unit,
                   "\n*ERROR: Packet size cannot be higher than %0dB "
                   "(Device MTU)\n",  MTU);
        }
    } else if (pktdma_p->pkt_size_param < min_pkt_size) {
        pktdma_p->bad_input = 1;
        test_error
            (unit,
             "\n*ERROR: Packet size cannot be lower than %0dB\n",
             MIN_PKT_SIZE);
    } else if (pktdma_p->pkt_size_param > MTU) {
        pktdma_p->bad_input = 1;
        test_error(unit,
                   "\n*ERROR: Packet size cannot be higher than %0dB (Device MTU)\n",
                    MTU);
    }

    if ((pktdma_p->poll_intr_param != 0)
        && (pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
            || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD)) {
        pktdma_p->bad_input = 1;
        test_error
            (unit,
             "\n*ERROR: Interrupts cannot be enabled with TestTypeTx/Rx=4 "
             "since this sets up an infinite loop in hardware.\n");
    }

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (((pktdma_p->tx_bitmap_param >> i) % 2) != 0) {
            num_tx_channels_active++;
        }
        if (((pktdma_p->rx_bitmap_param >> i) % 2) != 0) {
            num_rx_channels_active++;
        }
    }

    if (!
        (pktdma_p->stream_param_tx == 1
         || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
         || pktdma_p->stream_param_rx == 1
         || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD)) {
        if (num_tx_channels_active != num_rx_channels_active) {
            pktdma_p->bad_input = 1;
            test_error
                (unit,
                 "\n*ERROR: Test is set up as a feature test with finite "
                 "packet counts. Cannot do counter or packet integrity"
                 " checks if the number of active TX channels do not match"
                 " the number of active RX channels, since they cannot"
                 " be paired\n");
        }
    }

    if (pktdma_p->check_packet_integrity_param == 1) {
        if (pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
            || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
            pktdma_p->bad_input = 1;
            test_error
                (unit,
                 "\n*ERROR: Cannot do packet integrity check with "
                 "in desc loop mode\n");
        }
        if (pktdma_p->num_pkts_param_tx != pktdma_p->num_pkts_param_rx) {
            pktdma_p->bad_input = 1;
            test_error
                (unit,
                 "\n*ERROR: Cannot do packet integrity check in a finite "
                 "packet count test unless NumPktsTx==NumPktsRx\n");
        }
    }

}

/*
 * Function:
 *      pktdma_soc_gen_random_l2_pkt
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
pktdma_soc_gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size,
                  uint8 mac_da[NUM_BYTES_MAC_ADDR],
                  uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 tpid,
                  uint16 vlan_id, uint32 seq_id, uint32 channel)
{
    uint32 crc;
    tgp_gen_random_l2_pkt(pkt_ptr, pkt_size, mac_da, mac_sa, tpid, vlan_id);
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 6] = (seq_id >> 24) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 7] = (seq_id >> 16) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 8] = (seq_id >> 8) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 9] = (seq_id) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 10] = (pkt_size >> 8) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 11] = (pkt_size) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 12] = (channel) & 0xff;

    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    pkt_ptr[pkt_size - NUM_BYTES_CRC + 3] = (crc >> 24) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 2] = (crc >> 16) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 1] = (crc >> 8) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC] = (crc) & 0xff;
}

/*
 * Function:
 *      pktdma_soc_set_source_ch_array
 * Purpose:
 *      Fill up pktdma_p->source_ch_array. Provides the TX channel for
 *      correspoding RX channels when channels are paired in a non-streaming
 *      mode.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      tx_bitmap - 12 bit bitmap of TX channels
 *      rx_bitmap - 12 bit bitmap of RX channels
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_set_source_ch_array(int unit, uint32 tx_bitmap, uint32 rx_bitmap)
{
    uint32 tx_ch;
    uint32 rx_ch;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    tx_ch = 0;

    for (rx_ch = 0; rx_ch < MAX_CHANNELS; rx_ch++) {
        pktdma_p->source_ch[rx_ch] = 99;
    }

    for (rx_ch = 0; rx_ch < MAX_CHANNELS; rx_ch++) {
        if (((rx_bitmap >> rx_ch) % 2) != 0) {
            while (tx_ch < MAX_CHANNELS) {
                if (((tx_bitmap >> tx_ch) % 2) != 0) {
                    pktdma_p->source_ch[rx_ch] = tx_ch;
                    tx_ch++;
                    break;
                }
                tx_ch++;
            }
        }
    }
}

/*
 * Function:
 *      store_l2_packets
 * Purpose:
 *      Store L2 packets for TX, allocate space for received packets
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
store_l2_packets(int unit)
{
    pktdma_t *pktdma_p;
    uint32 min_pkt_size = MIN_PKT_SIZE;
    int32 i, j;
    uint32 use_random_packet_sizes = 0;
    int32 prio = 0;
    int32 num_rx_channels_active = 0;
    uint8 mac_sa[] = MAC_SA;
    uint8 mac_da[] = MAC_DA;

    pktdma_p = pktdma_parray[unit];

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling store_l2_packets")));

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nStoring random packet sizes")));

    pktdma_p->random_packet_sizes =
        (uint32 **) sal_alloc(MAX_CHANNELS * sizeof(uint32 *),
                              "pktdma_p->random_packet_sizes_ptr");
    for (i = 0; i < MAX_CHANNELS; i++) {
        pktdma_p->random_packet_sizes[i] =
            (uint32 *) sal_alloc(MAX_DESC_CHAIN_LENGTH * sizeof(uint32),
                                 "pktdma_p->random_packet_sizes");
    }

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (((pktdma_p->rx_bitmap_param >> i) % 2) != 0) {
            num_rx_channels_active++;
        }
    }

    for (i = 0; i < MAX_CHANNELS; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nChannel %0d"), i));
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n-----------")));
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
        for (j = 0; j < MAX_DESC_CHAIN_LENGTH; j++) {
            /* coverity[dont_call : FALSE] */
            pktdma_p->random_packet_sizes[i][j] =
                min_pkt_size +
                    (sal_rand() % (pktdma_p->max_pkt_size_param - min_pkt_size));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%0d "),
                      pktdma_p->random_packet_sizes[i][j]));
        }
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nDone Storing random packet sizes")));

    if (pktdma_p->pkt_size_param == 0) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nUsing Random Packet Sizes")));
        use_random_packet_sizes = 1;
    }

    pktdma_p->pkt_tx_array =
                    soc_cm_salloc(unit, MAX_CHANNELS * sizeof(uint8 **),
                                  "pktdma_p->pkt_tx_array***");

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nStoring TX packets")));

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (((pktdma_p->tx_bitmap_param >> i) % 2) != 0) {
            pktdma_p->pkt_tx_array[i] =
                            soc_cm_salloc(unit, pktdma_p->num_pkts_param_tx *
                                         sizeof(uint8 *),
                                         "pktdma_p->pkt_tx_array**");
            for (j = 0; j < pktdma_p->num_pkts_param_tx; j++) {
                if (use_random_packet_sizes == 1) {
                    pktdma_p->pkt_tx_array[i][j] =
                                    soc_cm_salloc(unit, pktdma_p->
                                                random_packet_sizes[i][j] *
                                                sizeof(uint8),
                                                "pktdma_p->pkt_tx_array*");
                    sal_srand(pktdma_p->pkt_seed + i + j);
                    pktdma_soc_gen_random_l2_pkt(pktdma_p->pkt_tx_array[i][j],
                                      pktdma_p->random_packet_sizes[i][j],
                                      mac_da, mac_sa, TPID,
                                      (VLAN | ((prio & 0x00000007) << 13) |
                                       (((prio >> 3) & 0x00000001) << 12)),
                                        j, i);
                } else {
                    pktdma_p->pkt_tx_array[i][j] =
                                    soc_cm_salloc(unit, pktdma_p->pkt_size_param *
                                                sizeof(uint8),
                                                "pktdma_p->pkt_tx_array*");
                    sal_srand(pktdma_p->pkt_seed + i + j);
                    pktdma_soc_gen_random_l2_pkt(pktdma_p->pkt_tx_array[i][j],
                                      pktdma_p->pkt_size_param, mac_da, mac_sa,
                                      TPID,
                                      (VLAN | ((prio & 0x00000007) << 13) |
                                       (((prio >> 3) & 0x00000001) << 12)),
                                        j, i);
                }
                if (pktdma_p->rx_bitmap_param != 0x000
                    && (pktdma_p->stream_param_tx == 1
                        || pktdma_p->stream_param_rx == 1
                        || pktdma_p->test_type_param_tx ==
                        DESC_TX_CHAIN_PKT_RELOAD
                        || pktdma_p->test_type_param_rx ==
                        DESC_RX_CHAIN_PKT_RELOAD)) {
                    prio++;
                    prio = prio % num_rx_channels_active;
                }
            }
            if (pktdma_p->rx_bitmap_param == 0x000
                ||
                (!(pktdma_p->stream_param_tx == 1
                   || pktdma_p->stream_param_rx == 1
                   || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
                   || pktdma_p->test_type_param_rx ==
                   DESC_RX_CHAIN_PKT_RELOAD))) {
                prio++;
            }
        }
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nDone Storing TX packets")));

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nAllocating memory for RX packets")));

    pktdma_p->pkt_rx_array =
                    soc_cm_salloc(unit, MAX_CHANNELS * sizeof(uint8 **),
                                  "pktdma_p->pkt_rx_array***");

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (((pktdma_p->rx_bitmap_param >> i) % 2) != 0) {
            pktdma_p->pkt_rx_array[i] =
                            soc_cm_salloc(unit, pktdma_p->num_pkts_param_rx *
                                         sizeof(uint8 *),
                                         "pktdma_p->pkt_rx_array**");
            for (j = 0; j < pktdma_p->num_pkts_param_rx; j++) {
                if (use_random_packet_sizes) {
                    pktdma_p->pkt_rx_array[i][j] =
                                    soc_cm_salloc(unit, pktdma_p->
                                                max_pkt_size_param *
                                                sizeof(uint8),
                                                "pktdma_p->pkt_rx_array*");
                } else {
                    pktdma_p->pkt_rx_array[i][j] =
                                    soc_cm_salloc(unit, pktdma_p->pkt_size_param *
                                                sizeof(uint8),
                                                "pktdma_p->pkt_rx_array*");
                }
            }
        }
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nDone Allocating memory for RX packets")));
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nDone storing packets")));
}

/*
 * Functions:
 *      desc_done_intr_tx_0..11
 * Purpose:
 *      Interrupt level routine called from soc_dma_done_desc for TX descriptors
 *      for virtual channels 0..11.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - Descriptor vector raising interrupt
 *      dcb - Descriptor raising interrupt
 *
 * Returns:
 *     Nothing
 *
 */

static void
desc_done_intr_tx_0(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[0]++;
}

static void
desc_done_intr_tx_1(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[1]++;
}

static void
desc_done_intr_tx_2(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[2]++;
}

static void
desc_done_intr_tx_3(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[3]++;
}

static void
desc_done_intr_tx_4(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[4]++;
}

static void
desc_done_intr_tx_5(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[5]++;
}

static void
desc_done_intr_tx_6(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[6]++;
}

static void
desc_done_intr_tx_7(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[7]++;
}

static void
desc_done_intr_tx_8(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[8]++;
}

static void
desc_done_intr_tx_9(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[9]++;
}

static void
desc_done_intr_tx_10(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[10]++;
}

static void
desc_done_intr_tx_11(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_tx[11]++;
}


/*
 * Functions:
 *      desc_done_intr_rx_0..11
 * Purpose:
 *      Interrupt level routine called from soc_dma_done_desc for RX descriptors
 *      for virtual channels 0..11.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - Descriptor vector raising interrupt
 *      dcb - Descriptor raising interrupt
 *
 * Returns:
 *     Nothing
 *
 */

static void
desc_done_intr_rx_0(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[0]++;
}

static void
desc_done_intr_rx_1(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[1]++;
}

static void
desc_done_intr_rx_2(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[2]++;
}

static void
desc_done_intr_rx_3(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[3]++;
}

static void
desc_done_intr_rx_4(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[4]++;
}

static void
desc_done_intr_rx_5(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[5]++;
}

static void
desc_done_intr_rx_6(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[6]++;
}

static void
desc_done_intr_rx_7(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[7]++;
}

static void
desc_done_intr_rx_8(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[8]++;
}

static void
desc_done_intr_rx_9(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[9]++;
}

static void
desc_done_intr_rx_10(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[10]++;
}

static void
desc_done_intr_rx_11(int unit, dv_t *dv, dcb_t *dcb)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->desc_done_count_rx[11]++;
}

/*
 * Functions:
 *      chain_done_intr_tx_0..11
 * Purpose:
 *      Interrupt level routine called from soc_dma_done_chain for TX descriptors
 *      for virtual channels 0..11.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - Descriptor vector raising interrupt
 *
 * Returns:
 *     Nothing
 *
 */

static void
chain_done_intr_tx_0(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[0]++;
    pktdma_p->chain_done_tx[0] = 1;
}

static void
chain_done_intr_tx_1(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[1]++;
    pktdma_p->chain_done_tx[1] = 1;
}

static void
chain_done_intr_tx_2(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[2]++;
    pktdma_p->chain_done_tx[2] = 1;
}

static void
chain_done_intr_tx_3(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[3]++;
    pktdma_p->chain_done_tx[3] = 1;
}

static void
chain_done_intr_tx_4(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[4]++;
    pktdma_p->chain_done_tx[4] = 1;
}

static void
chain_done_intr_tx_5(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[5]++;
    pktdma_p->chain_done_tx[5] = 1;
}

static void
chain_done_intr_tx_6(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[6]++;
    pktdma_p->chain_done_tx[6] = 1;
}

static void
chain_done_intr_tx_7(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[7]++;
    pktdma_p->chain_done_tx[7] = 1;
}

static void
chain_done_intr_tx_8(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[8]++;
    pktdma_p->chain_done_tx[8] = 1;
}

static void
chain_done_intr_tx_9(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[9]++;
    pktdma_p->chain_done_tx[9] = 1;
}

static void
chain_done_intr_tx_10(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[10]++;
    pktdma_p->chain_done_tx[10] = 1;
}

static void
chain_done_intr_tx_11(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_tx[11]++;
    pktdma_p->chain_done_tx[11] = 1;
}

/*
 * Functions:
 *      chain_done_intr_rx_0..11
 * Purpose:
 *      Interrupt level routine called from soc_dma_done_chain for RX descriptors
 *      for virtual channels 0..11.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - Descriptor vector raising interrupt
 *
 * Returns:
 *     Nothing
 *
 */

static void
chain_done_intr_rx_0(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[0]++;
    pktdma_p->chain_done_rx[0] = 1;
}

static void
chain_done_intr_rx_1(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[1]++;
    pktdma_p->chain_done_rx[1] = 1;
}

static void
chain_done_intr_rx_2(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[2]++;
    pktdma_p->chain_done_rx[2] = 1;
}

static void
chain_done_intr_rx_3(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[3]++;
    pktdma_p->chain_done_rx[3] = 1;
}

static void
chain_done_intr_rx_4(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[4]++;
    pktdma_p->chain_done_rx[4] = 1;
}

static void
chain_done_intr_rx_5(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[5]++;
    pktdma_p->chain_done_rx[5] = 1;
}

static void
chain_done_intr_rx_6(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[6]++;
    pktdma_p->chain_done_rx[6] = 1;
}

static void
chain_done_intr_rx_7(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[7]++;
    pktdma_p->chain_done_rx[7] = 1;
}

static void
chain_done_intr_rx_8(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[8]++;
    pktdma_p->chain_done_rx[8] = 1;
}

static void
chain_done_intr_rx_9(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[9]++;
    pktdma_p->chain_done_rx[9] = 1;
}

static void
chain_done_intr_rx_10(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[10]++;
    pktdma_p->chain_done_rx[10] = 1;
}

static void
chain_done_intr_rx_11(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->chain_done_count_rx[11]++;
    pktdma_p->chain_done_rx[11] = 1;
}

/*
 * Functions:
 *      reload_done_intr_tx_0..11
 * Purpose:
 *      Used only for continuous DMA.
 *      Interrupt level routine called from soc_dma_done_desc for TX descriptors
 *      for virtual channels 0..11, when the reload desc at the end of a desc
 *      chain is done.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - Descriptor vector raising interrupt
 *
 * Returns:
 *     Nothing
 *
 */

static void
reload_done_intr_tx_0(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[0]++;
    pktdma_p->reload_done_tx[0] = 1;
}

static void
reload_done_intr_tx_1(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[1]++;
    pktdma_p->reload_done_tx[1] = 1;
}

static void
reload_done_intr_tx_2(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[2]++;
    pktdma_p->reload_done_tx[2] = 1;
}

static void
reload_done_intr_tx_3(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[3]++;
    pktdma_p->reload_done_tx[3] = 1;
}

static void
reload_done_intr_tx_4(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[4]++;
    pktdma_p->reload_done_tx[4] = 1;
}

static void
reload_done_intr_tx_5(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[5]++;
    pktdma_p->reload_done_tx[5] = 1;
}

static void
reload_done_intr_tx_6(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[6]++;
    pktdma_p->reload_done_tx[6] = 1;
}

static void
reload_done_intr_tx_7(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[7]++;
    pktdma_p->reload_done_tx[7] = 1;
}

static void
reload_done_intr_tx_8(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[8]++;
    pktdma_p->reload_done_tx[8] = 1;
}

static void
reload_done_intr_tx_9(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[9]++;
    pktdma_p->reload_done_tx[9] = 1;
}

static void
reload_done_intr_tx_10(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[10]++;
    pktdma_p->reload_done_tx[10] = 1;
}

static void
reload_done_intr_tx_11(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_tx[11]++;
    pktdma_p->reload_done_tx[11] = 1;
}


/*
 * Functions:
 *      reload_done_intr_rx_0..11
 * Purpose:
 *      Used only for continuous DMA.
 *      Interrupt level routine called from soc_dma_done_desc for RX descriptors
 *      for virtual channels 0..11, when the reload desc at the end of a desc
 *      chain is done.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - Descriptor vector raising interrupt
 *
 * Returns:
 *     Nothing
 *
 */

static void
reload_done_intr_rx_0(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[0]++;
    pktdma_p->reload_done_rx[0] = 1;
}

static void
reload_done_intr_rx_1(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[1]++;
    pktdma_p->reload_done_rx[1] = 1;
}

static void
reload_done_intr_rx_2(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[2]++;
    pktdma_p->reload_done_rx[2] = 1;
}

static void
reload_done_intr_rx_3(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[3]++;
    pktdma_p->reload_done_rx[3] = 1;
}

static void
reload_done_intr_rx_4(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[4]++;
    pktdma_p->reload_done_rx[4] = 1;
}

static void
reload_done_intr_rx_5(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[5]++;
    pktdma_p->reload_done_rx[5] = 1;
}

static void
reload_done_intr_rx_6(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[6]++;
    pktdma_p->reload_done_rx[6] = 1;
}

static void
reload_done_intr_rx_7(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[7]++;
    pktdma_p->reload_done_rx[7] = 1;
}

static void
reload_done_intr_rx_8(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[8]++;
    pktdma_p->reload_done_rx[8] = 1;
}

static void
reload_done_intr_rx_9(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[9]++;
    pktdma_p->reload_done_rx[9] = 1;
}

static void
reload_done_intr_rx_10(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[10]++;
    pktdma_p->reload_done_rx[10] = 1;
}

static void
reload_done_intr_rx_11(int unit, dv_t *dv)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pktdma_p->reload_done_count_rx[11]++;
    pktdma_p->reload_done_rx[11] = 1;
}

/*
 * Function:
 *      set_up_isr_table
 * Purpose:
 *      Sets up interrupt related function pointers in test struct.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
set_up_isr_table(int unit)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nSetting up ISR table")));
    pktdma_p->desc_done_intr_tx_table[0] = &desc_done_intr_tx_0;
    pktdma_p->desc_done_intr_tx_table[1] = &desc_done_intr_tx_1;
    pktdma_p->desc_done_intr_tx_table[2] = &desc_done_intr_tx_2;
    pktdma_p->desc_done_intr_tx_table[3] = &desc_done_intr_tx_3;
    pktdma_p->desc_done_intr_tx_table[4] = &desc_done_intr_tx_4;
    pktdma_p->desc_done_intr_tx_table[5] = &desc_done_intr_tx_5;
    pktdma_p->desc_done_intr_tx_table[6] = &desc_done_intr_tx_6;
    pktdma_p->desc_done_intr_tx_table[7] = &desc_done_intr_tx_7;
    pktdma_p->desc_done_intr_tx_table[8] = &desc_done_intr_tx_8;
    pktdma_p->desc_done_intr_tx_table[9] = &desc_done_intr_tx_9;
    pktdma_p->desc_done_intr_tx_table[10] = &desc_done_intr_tx_10;
    pktdma_p->desc_done_intr_tx_table[11] = &desc_done_intr_tx_11;

    pktdma_p->desc_done_intr_rx_table[0] = &desc_done_intr_rx_0;
    pktdma_p->desc_done_intr_rx_table[1] = &desc_done_intr_rx_1;
    pktdma_p->desc_done_intr_rx_table[2] = &desc_done_intr_rx_2;
    pktdma_p->desc_done_intr_rx_table[3] = &desc_done_intr_rx_3;
    pktdma_p->desc_done_intr_rx_table[4] = &desc_done_intr_rx_4;
    pktdma_p->desc_done_intr_rx_table[5] = &desc_done_intr_rx_5;
    pktdma_p->desc_done_intr_rx_table[6] = &desc_done_intr_rx_6;
    pktdma_p->desc_done_intr_rx_table[7] = &desc_done_intr_rx_7;
    pktdma_p->desc_done_intr_rx_table[8] = &desc_done_intr_rx_8;
    pktdma_p->desc_done_intr_rx_table[9] = &desc_done_intr_rx_9;
    pktdma_p->desc_done_intr_rx_table[10] = &desc_done_intr_rx_10;
    pktdma_p->desc_done_intr_rx_table[11] = &desc_done_intr_rx_11;

    pktdma_p->chain_done_intr_tx_table[0] = &chain_done_intr_tx_0;
    pktdma_p->chain_done_intr_tx_table[1] = &chain_done_intr_tx_1;
    pktdma_p->chain_done_intr_tx_table[2] = &chain_done_intr_tx_2;
    pktdma_p->chain_done_intr_tx_table[3] = &chain_done_intr_tx_3;
    pktdma_p->chain_done_intr_tx_table[4] = &chain_done_intr_tx_4;
    pktdma_p->chain_done_intr_tx_table[5] = &chain_done_intr_tx_5;
    pktdma_p->chain_done_intr_tx_table[6] = &chain_done_intr_tx_6;
    pktdma_p->chain_done_intr_tx_table[7] = &chain_done_intr_tx_7;
    pktdma_p->chain_done_intr_tx_table[8] = &chain_done_intr_tx_8;
    pktdma_p->chain_done_intr_tx_table[9] = &chain_done_intr_tx_9;
    pktdma_p->chain_done_intr_tx_table[10] = &chain_done_intr_tx_10;
    pktdma_p->chain_done_intr_tx_table[11] = &chain_done_intr_tx_11;

    pktdma_p->chain_done_intr_rx_table[0] = &chain_done_intr_rx_0;
    pktdma_p->chain_done_intr_rx_table[1] = &chain_done_intr_rx_1;
    pktdma_p->chain_done_intr_rx_table[2] = &chain_done_intr_rx_2;
    pktdma_p->chain_done_intr_rx_table[3] = &chain_done_intr_rx_3;
    pktdma_p->chain_done_intr_rx_table[4] = &chain_done_intr_rx_4;
    pktdma_p->chain_done_intr_rx_table[5] = &chain_done_intr_rx_5;
    pktdma_p->chain_done_intr_rx_table[6] = &chain_done_intr_rx_6;
    pktdma_p->chain_done_intr_rx_table[7] = &chain_done_intr_rx_7;
    pktdma_p->chain_done_intr_rx_table[8] = &chain_done_intr_rx_8;
    pktdma_p->chain_done_intr_rx_table[9] = &chain_done_intr_rx_9;
    pktdma_p->chain_done_intr_rx_table[10] = &chain_done_intr_rx_10;
    pktdma_p->chain_done_intr_rx_table[11] = &chain_done_intr_rx_11;

    pktdma_p->reload_done_intr_tx_table[0] = &reload_done_intr_tx_0;
    pktdma_p->reload_done_intr_tx_table[1] = &reload_done_intr_tx_1;
    pktdma_p->reload_done_intr_tx_table[2] = &reload_done_intr_tx_2;
    pktdma_p->reload_done_intr_tx_table[3] = &reload_done_intr_tx_3;
    pktdma_p->reload_done_intr_tx_table[4] = &reload_done_intr_tx_4;
    pktdma_p->reload_done_intr_tx_table[5] = &reload_done_intr_tx_5;
    pktdma_p->reload_done_intr_tx_table[6] = &reload_done_intr_tx_6;
    pktdma_p->reload_done_intr_tx_table[7] = &reload_done_intr_tx_7;
    pktdma_p->reload_done_intr_tx_table[8] = &reload_done_intr_tx_8;
    pktdma_p->reload_done_intr_tx_table[9] = &reload_done_intr_tx_9;
    pktdma_p->reload_done_intr_tx_table[10] = &reload_done_intr_tx_10;
    pktdma_p->reload_done_intr_tx_table[11] = &reload_done_intr_tx_11;

    pktdma_p->reload_done_intr_rx_table[0] = &reload_done_intr_rx_0;
    pktdma_p->reload_done_intr_rx_table[1] = &reload_done_intr_rx_1;
    pktdma_p->reload_done_intr_rx_table[2] = &reload_done_intr_rx_2;
    pktdma_p->reload_done_intr_rx_table[3] = &reload_done_intr_rx_3;
    pktdma_p->reload_done_intr_rx_table[4] = &reload_done_intr_rx_4;
    pktdma_p->reload_done_intr_rx_table[5] = &reload_done_intr_rx_5;
    pktdma_p->reload_done_intr_rx_table[6] = &reload_done_intr_rx_6;
    pktdma_p->reload_done_intr_rx_table[7] = &reload_done_intr_rx_7;
    pktdma_p->reload_done_intr_rx_table[8] = &reload_done_intr_rx_8;
    pktdma_p->reload_done_intr_rx_table[9] = &reload_done_intr_rx_9;
    pktdma_p->reload_done_intr_rx_table[10] = &reload_done_intr_rx_10;
    pktdma_p->reload_done_intr_rx_table[11] = &reload_done_intr_rx_11;

}

/*
 * Function:
 *      set_global_desc_attr
 * Purpose:
 *      Sets variables in test struct for desc_attributes. Sets chain_tx,
 *      chain_rx (For chaining), sg_tx, sg_rx (Scatter/Gather) and reload_tx,
 *      reload_rx (Chain has reload desc at the end).
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
set_global_desc_attr(int unit)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nSetting global attributes")));

    pktdma_p->chain_tx = (pktdma_p->test_type_param_tx == DESC_TX_SINGLE_PKT
                          || pktdma_p->test_type_param_tx ==
                          DESC_TX_SINGLE_PKT_SPLIT) ? 0 : 1;
    pktdma_p->sg_tx = (pktdma_p->test_type_param_tx == DESC_TX_SINGLE_PKT_SPLIT
                       || pktdma_p->test_type_param_tx ==
                       DESC_TX_CHAIN_PKT_SPLIT) ? 1 : 0;
    pktdma_p->reload_tx =
        (pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD) ? 1 : 0;

    pktdma_p->chain_rx = (pktdma_p->test_type_param_rx == DESC_RX_SINGLE_PKT
                          || pktdma_p->test_type_param_rx ==
                          DESC_RX_SINGLE_PKT_SPLIT) ? 0 : 1;
    pktdma_p->sg_rx = (pktdma_p->test_type_param_rx == DESC_RX_SINGLE_PKT_SPLIT
                       || pktdma_p->test_type_param_rx ==
                       DESC_RX_CHAIN_PKT_SPLIT) ? 1 : 0;
    pktdma_p->reload_rx =
        (pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) ? 1 : 0;
}

/*
 * Function:
 *      set_up_dv
 * Purpose:
 *      Sets up DV arrays based on input params.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv_array - Array of DV pointers (dv_t*).
 *      dv_type - DV type (TX or RX).
 *      chain - Chaining enabled if chain is 1
 *      sg - Scatter/Gather, split each pkt into 2 descriptors if sg is 1
 *      reload - Last desc in chain is a reload desc if set to 1
 *      poll_intr - Set to 0 for polling, 1 for chain_done interrupts, 0 for
 *                  desc_done interrupts.
 *      channel - Virtual channel number (0..11)
 *      pkt_array - 2D array of packet pointers generated by store_l2_packets
 *      pkt_size - Packet size in bytes
 *      num_pkts - Number of packets in DV array
 *      random_packet_sizes - Random packet size array created by store_l2_packets
 *      source_ch - source_ch_array
 *      cont_dma - 1 of cont_dma is enabled
 * Returns:
 *     Nothing
 *
 */

static void
set_up_dv(int unit, dv_t **dv_array, dvt_t dv_type, uint32 chain,
          uint32 sg, uint32 reload, uint32 poll_intr, uint32 channel,
          uint8 ***pkt_array, uint32 pkt_size, uint32 num_pkts,
          uint32 **random_packet_sizes, uint32 *source_ch, uint32 cont_dma)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    int32 i;
    uint32 dma_bytes;
    uint32 pkt_size_test = 0;
    int32 flags = 0;
    dv_t *dv = 0;
    int32 dv_num = 0;

    BCM_PBMP_CLEAR(lp_pbm);
    BCM_PBMP_CLEAR(empty_pbm0);
    BCM_PBMP_CLEAR(empty_pbm1);
    BCM_PBMP_PORT_ADD(lp_pbm, 1);

    for (i = 0; i < num_pkts; i++) {
        dv = dv_array[dv_num];

        if (pkt_size == 0) {
            if (dv_type == DV_TX) {
                pkt_size_test = random_packet_sizes[channel][i];
            } else if (dv_type == DV_RX) {
                pkt_size_test = pktdma_p->max_pkt_size_param;
            }
        } else {
            pkt_size_test = pkt_size;
        }

        if (sg == 0) {
            dma_bytes = pkt_size_test;
        } else {
            /* coverity[dont_call : FALSE] */
            dma_bytes = ((sal_rand() % ((pkt_size_test / 4) - 4)) * 4) + 4;
        }

        soc_dma_desc_add(dv, (sal_vaddr_t) (pkt_array[channel][i]), dma_bytes,
                         lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
        if (sg == 1) {
            soc_dma_desc_add(dv,
                             (sal_vaddr_t) (pkt_array[channel][i] + dma_bytes),
                             pkt_size_test - dma_bytes, lp_pbm, empty_pbm0,
                             empty_pbm1, flags, NULL);
        }
        soc_dma_desc_end_packet(dv);
        if (poll_intr == POLL) {
            flags = 0;
        } else if (poll_intr == CHAIN_DONE_INTR_ONLY) {
            flags |= DV_F_NOTIFY_CHN;
        } else {
            flags |= DV_F_NOTIFY_DSC;
            flags |= DV_F_NOTIFY_CHN;
        }
        dv->dv_flags = flags;

        if (dv_type == DV_TX) {
            dv->dv_done_desc = pktdma_p->desc_done_intr_tx_table[channel];
            dv->dv_done_chain = pktdma_p->chain_done_intr_tx_table[channel];
            dv->dv_done_reload = pktdma_p->reload_done_intr_tx_table[channel];
        } else {
            dv->dv_done_desc = pktdma_p->desc_done_intr_rx_table[channel];
            dv->dv_done_chain = pktdma_p->chain_done_intr_rx_table[channel];
            dv->dv_done_reload = pktdma_p->reload_done_intr_rx_table[channel];
        }
        if (chain == 0) {
            dv_num++;
        }
    }
    if (cont_dma == 1) {
        soc_dma_rld_desc_add(dv, 0);
        SOC_DCB_CHAIN_SET(unit,
                          SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt - 1),
                          1);
    } else if (reload == 1) {
        soc_dma_rld_desc_add(dv, (sal_vaddr_t) dv->dv_dcb);
        SOC_DCB_CHAIN_SET(unit,
                          SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt - 1),
                          1);
    }

}

/*
 * Function:
 *      config_dma_chan
 * Purpose:
 *      Calls soc_dma_chan_config to configure DMA channels
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      tx_bitmap - 12-bit bitmap of virtual channels enabled for TX
 *      rx_bitmap - 12-bit bitmap of virtual channels enabled for RX
 *      poll_intr - Set to 0 for polling, 1 for chain done interrupts only and
                    2 for both desc and chain done interrupts
 * Returns:
 *     Nothing
 *
 */

static void
config_dma_chan(int unit, uint32 tx_bitmap, uint32 rx_bitmap, uint32 poll_intr)
{
    int32 channel;
    int rv = SOC_E_NONE;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling config_dma_chan")));
    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        if (((tx_bitmap >> channel) % 2) != 0) {
            if (poll_intr != POLL) {
                cli_out("\nConfiguring TX DMA channel %0d for interrupts",
                        channel);
                rv = soc_dma_chan_config(unit, channel, DV_TX, SOC_DMA_F_INTR);
            } else {
                cli_out("\nConfiguring TX DMA channel %0d for polling",
                        channel);
                rv = soc_dma_chan_config(unit, channel, DV_TX, SOC_DMA_F_POLL);
            }
        } else if (((rx_bitmap >> channel) % 2) != 0) {
            if (poll_intr != POLL) {
                cli_out("\nConfiguring RX DMA channel %0d for interrupts",
                        channel);
                rv = soc_dma_chan_config(unit, channel, DV_RX, SOC_DMA_F_INTR);
            } else {
                cli_out("\nConfiguring RX DMA channel %0d for polling",
                        channel);
                rv = soc_dma_chan_config(unit, channel, DV_RX, SOC_DMA_F_POLL);
            }
        }

        if (SOC_FAILURE(rv)) {
            test_error(unit, "\nError calling soc_dma_chan_config");
            pktdma_p->test_fail = 1;
        }
    }
}

/*
 * Function:
 *      soc_dma_chan_check_done
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
soc_dma_chan_check_done(int unit, int32 vchan, soc_dma_poll_type_t type,
                        int32 *detected)
{
    int32 rv = SOC_E_NONE;
    int32 cmc = vchan / N_DMA_CHAN;
    int32 chan = vchan % N_DMA_CHAN;

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
 *      soc_check_cont_dma_in_halt
 * Purpose:
 *      Check DMA_STAT_HI register to see if channel is halted - used for
 *      continuous DMA
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vchan - Virtual channel number
 *      detected - whether desc done or chain done was detected
 *
 * Returns:
 *    Nothing
 *
 */

static void
soc_check_cont_dma_in_halt(int unit, int32 vchan, int32 *detected)
{
    int32 cmc = vchan / N_DMA_CHAN;
    int32 chan = vchan % N_DMA_CHAN;
    uint32 rdata;

    rdata = soc_pci_read(unit, CMIC_CMCx_DMA_STAT_HI_OFFSET(cmc));

    *detected = 0;

    if (((rdata >> (CH0_IN_HALT + chan)) % 2) != 0) {
        *detected = 1;
    }
}

/*
 * Function:
 *      txdma_thread
 * Purpose:
 *      TX DMA thread - one thread takes care of all active channels
 * Parameters:
 *      up - Pointer to StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
txdma_thread(void *up)
{
    int unit = PTR_TO_INT(up);
    uint32 performance_test = 0;
    int32 first_time[MAX_CHANNELS];
    int32 channel;
    int32 dv_num[MAX_CHANNELS];
    int32 channel_all_dv_done[MAX_CHANNELS];
    int32 poll_chain_done_detected = 0;
    int32 all_channels_done = 0;
    int32 dv_cont[MAX_CHANNELS];

    pktdma_t *pktdma_p = pktdma_parray[unit];

    if ((pktdma_p->stream_param_tx == 1)
        || (pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD)) {
        performance_test = 1;
    } else {
        performance_test = 0;
    }

    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        first_time[channel] = 1;
        dv_cont[channel] = 0;
        dv_num[channel] = 0;
        channel_all_dv_done[channel] = 0;
    }

    do {
        for (channel = 0; channel < MAX_CHANNELS; channel++) {
            if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
                if (channel_all_dv_done[channel] == 0 || performance_test) {
                    if (performance_test && channel_all_dv_done[channel] == 1) {
                        channel_all_dv_done[channel] = 0;
                        dv_num[channel] = 0;
                        if (pktdma_p->cont_dma == 1) {
                            dv_cont[channel]++;
                            dv_cont[channel] =
                                dv_cont[channel] % NUM_CONT_DMA_DV;
                        }
                    }
                    if (pktdma_p->poll_intr_param == POLL || pktdma_p->cont_dma) {
                        if (pktdma_p->cont_dma) {
                            soc_check_cont_dma_in_halt(unit, channel,
                                                       &poll_chain_done_detected);
                        } else {
                            soc_dma_chan_check_done(unit, channel,
                                                    SOC_DMA_POLL_CHAIN_DONE,
                                                    &poll_chain_done_detected);
                        }
                    }
                    if (first_time[channel] || poll_chain_done_detected
                        || pktdma_p->chain_done_tx[channel]) {
                        pktdma_p->chain_done_tx[channel] = 0;
                        if (pktdma_p->poll_intr_param == POLL
                            && poll_chain_done_detected
                            && (pktdma_p->cont_dma == 0)) {
                            if (dv_num[channel] == 0) {
                                if (pktdma_p->chain_tx == 1) {
                                    soc_dma_abort_dv(unit,
                                                     pktdma_p->
                                                     dv_tx_array[dv_cont
                                                                 [channel]]
                                                     [channel][0]);
                                } else {
                                    soc_dma_abort_dv(unit,
                                                     pktdma_p->
                                                     dv_tx_array[dv_cont
                                                                 [channel]]
                                                     [channel][pktdma_p->
                                                               num_pkts_param_tx
                                                               - 1]);
                                }
                            } else {
                                soc_dma_abort_dv(unit,
                                                 pktdma_p->
                                                 dv_tx_array[dv_cont[channel]]
                                                 [channel][dv_num[channel] -
                                                           1]);
                            }
                        }
                        soc_dma_dv_reset(DV_TX,
                                         pktdma_p->
                                         dv_tx_array[dv_cont[channel]][channel]
                                         [dv_num[channel]]);
                        set_up_dv(unit,
                                  pktdma_p->
                                  dv_tx_array[dv_cont[channel]][channel], DV_TX,
                                  pktdma_p->chain_tx, pktdma_p->sg_tx,
                                  pktdma_p->reload_tx,
                                  pktdma_p->poll_intr_param, channel,
                                  pktdma_p->pkt_tx_array,
                                  pktdma_p->pkt_size_param,
                                  pktdma_p->num_pkts_param_tx,
                                  pktdma_p->random_packet_sizes,
                                  pktdma_p->source_ch, pktdma_p->cont_dma);
                        soc_dma_start(unit, channel,
                                      pktdma_p->
                                      dv_tx_array[dv_cont[channel]][channel]
                                      [dv_num[channel]]);
                        dv_num[channel]++;
                        if ((pktdma_p->chain_tx && dv_num[channel] == 1)
                            || (dv_num[channel] == pktdma_p->num_pkts_param_tx)) {
                            channel_all_dv_done[channel] = 1;
                        }
                    }
                    first_time[channel] = 0;
                }
            }
            poll_chain_done_detected = 0;
        }
        if (performance_test == 0) {
            all_channels_done = 1;
            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                if (channel_all_dv_done[channel] == 0
                    && (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0)) {
                    all_channels_done = 0;
                }
            }
        }
    } while (all_channels_done != 1 && pktdma_p->kill_dma == 0);

    pktdma_p->tx_thread_done = 1;
}

/*
 * Function:
 *      pktdma_soc_check_packet_content
 * Purpose:
 *      Check received packets for content
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      channel : RX channel
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_check_packet_content(int unit, uint32 channel)
{
    int32 j, k;
    int32 match = 1;
    uint8 *packet;
    uint8 *exp_packet;
    uint32 seq_id;
    uint32 pkt_size;
    uint32 tx_channel;
    uint16 vlan_prio;
    uint8 mac_sa[] = MAC_SA;
    uint8 mac_da[] = MAC_DA;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    if (pktdma_p->pkt_size_param != 0) {
        packet = sal_alloc(pktdma_p->pkt_size_param * sizeof(uint8), "Packet");
        exp_packet = sal_alloc(pktdma_p->pkt_size_param * sizeof(uint8),
                                                                "Exp Packet");
    } else {
        packet = sal_alloc(pktdma_p->max_pkt_size_param * sizeof(uint8),
                                                                "Packet");
        exp_packet = sal_alloc(pktdma_p->max_pkt_size_param * sizeof(uint8),
                                                               "Exp Packet");
    }

    if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
        for (j = 0; j < pktdma_p->num_pkts_param_rx; j++) {

            if (pktdma_p->pkt_size_param != 0) {
                for (k = 0; k < pktdma_p->pkt_size_param; k++) {
                    packet[k] = pktdma_p->pkt_rx_array[channel][j][k];
                }
            } else {
                for (k = 0; k < pktdma_p->max_pkt_size_param; k++) {
                    packet[k] = pktdma_p->pkt_rx_array[channel][j][k];
                }
            }
            packet[(2 * NUM_BYTES_MAC_ADDR) + 2] &= 0xf0;
            packet[(2 * NUM_BYTES_MAC_ADDR) + 2] |= (VLAN >> 8) & 0x000f;
            packet[(2 * NUM_BYTES_MAC_ADDR) + 3] = VLAN & 0x00ff;
            seq_id = 0x00000000;
            pkt_size = 0x00000000;
            tx_channel = 0x00000000;
            vlan_prio = 0x0000;

            vlan_prio |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 2] << 8);
            vlan_prio |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 3]);
            seq_id |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 6] << 24);
            seq_id |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 7] << 16);
            seq_id |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 8] << 8);
            seq_id |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 9]);
            pkt_size |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 10] << 8);
            pkt_size |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 11]);
            tx_channel |= (packet[(2 * NUM_BYTES_MAC_ADDR) + 12]);

            sal_srand(pktdma_p->pkt_seed + tx_channel + seq_id);
            pktdma_soc_gen_random_l2_pkt(exp_packet, pkt_size, mac_da, mac_sa,
                                         TPID, vlan_prio, seq_id, tx_channel);

            for (k = 0; k < pkt_size; k++) {
                if (packet[k] != exp_packet[k]) {
                    test_error(unit, "\nCorrupt packet received on "
                                        "channel %0d at location %p", channel,
                                        pktdma_p->pkt_rx_array[channel][j]);
                    pktdma_p->test_fail = 1;
                    match = 0;
                    break;
                }
            }
        }
    }

    if (match == 0) {
        test_error(unit, "\n*ERROR: Packet content check failed for channel %0d",
                            channel);
        pktdma_p->test_fail = 1;
    }
    sal_free(packet);
    sal_free(exp_packet);
}

/*
 * Function:
 *      rxdma_thread
 * Purpose:
 *      RX DMA thread - one thread takes care of all active channels
 * Parameters:
 *      up - Pointer to StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
rxdma_thread(void *up)
{
    int unit = PTR_TO_INT(up);
    uint32 performance_test = 0;
    int32 first_time[MAX_CHANNELS];
    int32 channel;
    int32 dv_num[MAX_CHANNELS];
    int32 channel_all_dv_done[MAX_CHANNELS];
    int32 poll_chain_done_detected = 0;
    int32 all_channels_done = 0;
    int32 dv_cont[MAX_CHANNELS];
    int32 skip_chain_cnt[MAX_CHANNELS];
    int32 first_chain_done[MAX_CHANNELS];

    pktdma_t *pktdma_p = pktdma_parray[unit];

    if ((pktdma_p->stream_param_rx == 1)
        || (pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD)) {
        performance_test = 1;
    } else {
        performance_test = 0;
    }

    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        first_time[channel] = 1;
        dv_cont[channel] = 0;
        dv_num[channel] = 0;
        channel_all_dv_done[channel] = 0;
        skip_chain_cnt[channel] = 0;
        first_chain_done[channel] = 0;
    }

    do {
        for (channel = 0; channel < MAX_CHANNELS; channel++) {
            if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
                if (channel_all_dv_done[channel] == 0 || performance_test) {
                    if (performance_test && channel_all_dv_done[channel] == 1) {
                        channel_all_dv_done[channel] = 0;
                        dv_num[channel] = 0;
                        if (pktdma_p->cont_dma == 1) {
                            dv_cont[channel]++;
                            dv_cont[channel] =
                                dv_cont[channel] % NUM_CONT_DMA_DV;
                        }
                    }
                    if (pktdma_p->poll_intr_param == POLL || pktdma_p->cont_dma) {
                        if (pktdma_p->cont_dma) {
                            soc_check_cont_dma_in_halt(unit, channel,
                                                       &poll_chain_done_detected);
                        } else {
                            soc_dma_chan_check_done(unit, channel,
                                                    SOC_DMA_POLL_CHAIN_DONE,
                                                    &poll_chain_done_detected);
                        }
                    }
                    if (first_time[channel] || poll_chain_done_detected
                        || pktdma_p->chain_done_rx[channel]) {
                        pktdma_p->chain_done_rx[channel] = 0;
                        if (pktdma_p->poll_intr_param == POLL
                            && poll_chain_done_detected
                            && (pktdma_p->cont_dma == 0)) {
                            if (dv_num[channel] == 0) {
                                if (pktdma_p->chain_rx == 1) {
                                    soc_dma_abort_dv(unit,
                                                     pktdma_p->
                                                     dv_rx_array[dv_cont
                                                                 [channel]]
                                                     [channel][0]);
                                } else {
                                    soc_dma_abort_dv(unit,
                                                     pktdma_p->
                                                     dv_rx_array[dv_cont
                                                                 [channel]]
                                                     [channel][pktdma_p->
                                                               num_pkts_param_rx
                                                               - 1]);
                                }
                            } else {
                                soc_dma_abort_dv(unit,
                                                 pktdma_p->
                                                 dv_rx_array[dv_cont[channel]]
                                                 [channel][dv_num[channel] -
                                                           1]);
                            }
                        }
                        soc_dma_dv_reset(DV_RX,
                                         pktdma_p->
                                         dv_rx_array[dv_cont[channel]][channel]
                                         [dv_num[channel]]);
                        set_up_dv(unit,
                                  pktdma_p->
                                  dv_rx_array[dv_cont[channel]][channel], DV_RX,
                                  pktdma_p->chain_rx, pktdma_p->sg_rx,
                                  pktdma_p->reload_rx,
                                  pktdma_p->poll_intr_param, channel,
                                  pktdma_p->pkt_rx_array,
                                  pktdma_p->pkt_size_param,
                                  pktdma_p->num_pkts_param_rx,
                                  pktdma_p->random_packet_sizes,
                                  pktdma_p->source_ch, pktdma_p->cont_dma);

                        dv_num[channel]++;

                        if ((pktdma_p->chain_rx && dv_num[channel] == 1)
                            || (dv_num[channel] == pktdma_p->num_pkts_param_rx)) {
                            channel_all_dv_done[channel] = 1;
                            if ((pktdma_p->check_packet_integrity_param == 1)
                                && (skip_chain_cnt[channel] == 0)
                                && (first_time[channel] == 0)
                                && (first_chain_done[channel] == 1)) {
                                pktdma_soc_check_packet_content(unit, channel);
                            }
                            skip_chain_cnt[channel]
                                        = (skip_chain_cnt[channel] + 1)
                                                % pktdma_p->pkt_chk_int_param;
                            first_chain_done[channel] = 1;
                        }
                        soc_dma_start(unit, channel,
                                      pktdma_p->
                                      dv_rx_array[dv_cont[channel]][channel]
                                      [dv_num[channel] - 1]);
                    }
                    first_time[channel] = 0;
                }
            }
            poll_chain_done_detected = 0;
        }
        if (performance_test == 0) {
            all_channels_done = 1;
            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                if (channel_all_dv_done[channel] == 0
                    && (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0)) {
                    all_channels_done = 0;
                }
            }
        }

    } while ((all_channels_done != 1) && pktdma_p->kill_dma == 0);

    pktdma_p->rx_thread_done = 1;
}

/*
 * Function:
 *      program_hostmem_addr_remap
 * Purpose:
 *      Program CMIC_CMCx_HOSTMEM_ADDR_REMAP regs for all CMCs
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
program_hostmem_addr_remap(int unit)
{
    int32 cmc;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id (unit, &dev_id, &rev_id);

    if (dev_id == BCM56960_DEVICE_ID) {
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc),
                        0x144d2450);
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc),
                        0x19617595);
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc),
                        0x1e75c6da);
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc), 0x1f);
        }
    }
    else if (dev_id == BCM56860_DEVICE_ID) {
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc),
                        0x2b49ca30);
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc),
                        0x37ace2f6);
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc),
                        0xffbbc);
        }
    }
}

/*
 * Function:
 *      turn_off_cmic_mmu_bkp
 * Purpose:
 *      Turn off CMIC to MMU backpressure
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
turn_off_cmic_mmu_bkp(int unit)
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
 *      set_up_cos_ctrl
 * Purpose:
 *      Program COS_CTRL bitmaps.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      rx_bitmap: 12-bit bitmap of virtual channels active for RX
 *
 * Returns:
 *     Nothing
 */

static void
set_up_cos_ctrl(int unit, uint32 rx_bitmap)
{
    int32 cmc, ch;
    uint32 max_cos_num = 0;
    uint32 cos_bmp0, cos_bmp1;
    uint32 i;

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, ch),
                          0x0);
            soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, ch),
                          0x0);
            if (((rx_bitmap >> (N_DMA_CHAN * cmc + ch)) % 2) != 0) {
                if (max_cos_num < 32) {
                    soc_pci_write(unit,
                                  CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, ch),
                                  (0x1 << max_cos_num));
                } else {
                    soc_pci_write(unit,
                                  CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, ch),
                                  (0x1 << (max_cos_num - 32)));
                }
                max_cos_num++;
            }
        }
    }

    cos_bmp0 =
        soc_pci_read(unit,
                     CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET((SOC_CMCS_NUM(unit) -
                                                         1), (N_DMA_CHAN - 1)));
    cos_bmp1 =
        soc_pci_read(unit,
                     CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET((SOC_CMCS_NUM(unit) -
                                                         1), (N_DMA_CHAN - 1)));

    if (max_cos_num < 32) {
        for (i = max_cos_num; i < 32; i++) {
            cos_bmp0 |= (0x1 << i);
        }
        max_cos_num = 32;
    }

    for (i = max_cos_num; i < 64; i++) {
        cos_bmp1 |= (0x1 << (i - 32));
    }

    soc_pci_write(unit,
                  CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET((SOC_CMCS_NUM(unit) - 1),
                                                     (N_DMA_CHAN - 1)),
                  cos_bmp0);
    soc_pci_write(unit,
                  CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET((SOC_CMCS_NUM(unit) - 1),
                                                     (N_DMA_CHAN - 1)),
                  cos_bmp1);
}

/*
 * Function:
 *      pktdma_init
 * Purpose:
 *      Basic initialization for packet DMA
 * Parameters:
 *      unit: StrataSwitch Unit #
 *      disable_rx_fc: Set to 1 to disable CMIC to MMU backpressure
 *      rx_bitmap - 12-bit bitmap of active RX virtual channels
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_init(int unit, uint32 disable_rx_fc, uint32 rx_bitmap,
            uint32 skip_cos_prog)
{
    program_hostmem_addr_remap(unit);
    if (disable_rx_fc == 1) {
        turn_off_cmic_mmu_bkp(unit);
    }

    if (skip_cos_prog != 1) {
        set_up_cos_ctrl(unit, rx_bitmap);
    }
}

/*
 * Function:
 *      clear_all_dma_ctrl_regs
 * Purpose:
 *      Clear all CMIC_CMCx_CHy_DMA_CTRL regs
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
clear_all_dma_ctrl_regs(int unit)
{
    int32 cmc, ch;
    uint32 rdata, ctrl_reg;

    rdata = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(0, 0));
    rdata &= (PKTDMA_BIG_ENDIAN | PKTDMA_DESC_BIG_ENDIAN);
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nrdata = 0x%08x"), rdata));

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, ch), rdata);
            ctrl_reg =
                soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, ch));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nCMIC_CMC%0d_CH%0d_DMA_CTRL = 0x%08x"),
                      cmc, ch, ctrl_reg));
        }
    }
}

/*
 * Function:
 *      clear_dma_status_regs
 * Purpose:
 *      Clear all CMIC_CMCx_DMA_STAT regs
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
clear_dma_status_regs(int unit)
{
    int32 cmc;

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        soc_pci_write(unit, CMIC_CMCx_DMA_STAT_CLR_OFFSET(cmc), 0x0);
    }
}

/*
 * Function:
 *      clear_cmic_pkt_counters
 * Purpose:
 *      Clear CMIC packet counters
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
clear_cmic_pkt_counters(int unit)
{
    int32 cmc, ch;

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            soc_pci_write(unit, CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, ch),
                          0x0);
            soc_pci_write(unit, CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, ch),
                          0x0);
        }
    }
}

/*
 * Function:
 *      pktdma_soc_test_clean
 * Purpose:
 *      Cleanup routine. Aborts all packet DMAs and clears ctrl and status regs
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      abort_dma: Set to 1 to abort all packet DMA operations
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_test_clean(int unit, int abort_dma)
{
    cli_out("\nCalling pktdma_soc_test_clean");
    cli_out("\nClearing DMA status regs");
    clear_dma_status_regs(unit);
    if (abort_dma != 0) {
        cli_out("\nAborting all DMA ops");
        soc_dma_abort(unit);
    }
    sal_usleep(100000);
    cli_out("\nClearing DMA ctrl regs");
    clear_all_dma_ctrl_regs(unit);
    cli_out("\nClearing CMIC packet ctrs");
    clear_cmic_pkt_counters(unit);
    cli_out("\nClearing DMA status regs");
    clear_dma_status_regs(unit);
}

/*
 * Function:
 *      pktdma_soc_program_cpu_cos_map
 * Purpose:
 *      Program CPU_COS_MAP table for 16 priorities. Maps each internal priority
 *      to an unique COS queue.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_program_cpu_cos_map(int unit)
{
    cpu_cos_map_entry_t cpu_cos_map_entry;
    uint32 entry;

    for (entry = 0; entry < MAX_COS; entry++) {
        (void) soc_mem_read(unit, CPU_COS_MAPm, COPYNO_ALL, entry,
                     cpu_cos_map_entry.entry_data);
        soc_mem_field32_set(unit, CPU_COS_MAPm, cpu_cos_map_entry.entry_data,
                            VALIDf, 0x1);
        soc_mem_field32_set(unit, CPU_COS_MAPm, cpu_cos_map_entry.entry_data,
                            INT_PRI_MASKf, 0xf);
        soc_mem_field32_set(unit, CPU_COS_MAPm, cpu_cos_map_entry.entry_data,
                            INT_PRI_KEYf, entry);
        soc_mem_field32_set(unit, CPU_COS_MAPm, cpu_cos_map_entry.entry_data,
                            COSf, entry);
        soc_mem_field32_set(unit, CPU_COS_MAPm, cpu_cos_map_entry.entry_data,
                            RQE_CPU_COSf, (entry % 2));
        (void) soc_mem_write(unit, CPU_COS_MAPm, COPYNO_ALL, entry,
                      cpu_cos_map_entry.entry_data);
    }
}

/*
 * Function:
 *      pktdma_soc_program_cos_map_pri16
 * Purpose:
 *      Misc table programming to map 16 internal priorities to corresponding COS
 *      queues. For example prio 0 is mapped to queue 0, priority 1 to queue 1
 *      and so on.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_program_cos_map_pri16(int unit)
{
    uint32 i, j, p;
    uint64 cpu_lo_rqe_q_num;
    uint64 cpu_hi_rqe_q_num;
    uint64 mirror_rqe_q_num;
    ing_pri_cng_map_entry_t ing_pri_cng_map_entry;
    ing_outer_dot1p_mapping_table_entry_t ing_outer_dot1p_mapping_table_entry;
    egr_pri_cng_map_entry_t egr_pri_cng_map_entry;
    port_cos_map_entry_t port_cos_map_entry;

    soc_field_t egr_vlan_control_1_fields[] =
        { CFI_AS_CNGf, VT_MISS_DROPf, VT_ENABLEf };
    uint32 egr_vlan_control_1_values[] = { 0x1, 0x0, 0x0 };
    int rv;

    pktdma_t *pktdma_p = pktdma_parray[unit];

    COMPILER_64_SET(cpu_lo_rqe_q_num, 0x0, 0x8);
    COMPILER_64_SET(cpu_hi_rqe_q_num, 0x0, 0x9);
    COMPILER_64_SET(mirror_rqe_q_num, 0x0, 0xa);

    for (i = 0; i < soc_mem_index_max(unit, ING_PRI_CNG_MAPm); i++) {
        for (j = 0;
             j < (sizeof(ing_pri_cng_map_entry.entry_data) / sizeof(uint32));
             j++) {
            ing_pri_cng_map_entry.entry_data[j] = 0x00000000;
        }

        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm,
                            ing_pri_cng_map_entry.entry_data, PRIf,
                            ((8 * (i % 2)) + ((i % 16) / 2)));
        (void) soc_mem_write(unit, ING_PRI_CNG_MAPm, COPYNO_ALL, i,
                      ing_pri_cng_map_entry.entry_data);
    }

    for (i = 0; i < soc_mem_index_max(unit, ING_OUTER_DOT1P_MAPPING_TABLEm);
         i++) {
        for (j = 0;
             j <
             (sizeof(ing_outer_dot1p_mapping_table_entry.entry_data) /
              sizeof(uint32)); j++) {
            ing_outer_dot1p_mapping_table_entry.entry_data[j] = 0x00000000;
        }

        soc_mem_field32_set(unit, ING_OUTER_DOT1P_MAPPING_TABLEm,
                            ing_outer_dot1p_mapping_table_entry.entry_data,
                            NEW_CFIf, (i % 2));
        soc_mem_field32_set(unit, ING_OUTER_DOT1P_MAPPING_TABLEm,
                            ing_outer_dot1p_mapping_table_entry.entry_data,
                            NEW_DOT1Pf, ((i % 16) / 2));
        (void) soc_mem_write(unit, ING_OUTER_DOT1P_MAPPING_TABLEm, COPYNO_ALL, i,
                      ing_outer_dot1p_mapping_table_entry.entry_data);
    }


    for (i = 0; i < soc_mem_index_max(unit, EGR_PRI_CNG_MAPm); i++) {
        for (j = 0;
             j < (sizeof(egr_pri_cng_map_entry.entry_data) / sizeof(uint32));
             j++) {
            egr_pri_cng_map_entry.entry_data[j] = 0x00000000;
        }

        soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm,
                            egr_pri_cng_map_entry.entry_data, CFIf,
                            ((i / 32) % 2));
        soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm,
                            egr_pri_cng_map_entry.entry_data, PRIf,
                            ((i % 32) / 4));
        (void) soc_mem_write(unit, EGR_PRI_CNG_MAPm, COPYNO_ALL, i,
                      egr_pri_cng_map_entry.entry_data);
    }

    for (i = 0; i < soc_mem_index_max(unit, PORT_COS_MAPm); i++) {
        for (j = 0;
             j < (sizeof(port_cos_map_entry.entry_data) / sizeof(uint32));
             j++) {
            port_cos_map_entry.entry_data[j] = 0x00000000;
        }

        soc_mem_field32_set(unit, PORT_COS_MAPm, port_cos_map_entry.entry_data,
                            UC_COS1f, ((i % 16) % 10));
        soc_mem_field32_set(unit, PORT_COS_MAPm, port_cos_map_entry.entry_data,
                            MC_COS1f, ((i % 16) % 10));
        soc_mem_field32_set(unit, PORT_COS_MAPm, port_cos_map_entry.entry_data,
                            RQE_Q_NUMf, ((i % 16) % 11));
        (void) soc_mem_write(unit, PORT_COS_MAPm, COPYNO_ALL, i,
                      port_cos_map_entry.entry_data);
    }

    if (SOC_IS_TOMAHAWK(unit)) {
        rv = soc_reg_set(unit, CPU_LO_RQE_Q_NUMr, 0, 0, cpu_lo_rqe_q_num);
        if (SOC_FAILURE(rv)) {
            test_error(unit, "\nError writing register CPU_LO_RQE_Q_NUM");
            pktdma_p->test_fail = 1;
        }
        rv = soc_reg_set(unit, CPU_HI_RQE_Q_NUMr, 0, 0, cpu_hi_rqe_q_num);
        if (SOC_FAILURE(rv)) {
            test_error(unit, "\nError writing register CPU_HI_RQE_Q_NUM");
            pktdma_p->test_fail = 1;
        }
        rv = soc_reg_set(unit, MIRROR_RQE_Q_NUMr, 0, 0, mirror_rqe_q_num);
        if (SOC_FAILURE(rv)) {
            test_error(unit, "\nError writing register MIRROR_RQE_Q_NUM");
            pktdma_p->test_fail = 1;
        }
    }

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        soc_reg_fields32_modify(unit, EGR_VLAN_CONTROL_1r, p, 3,
                                egr_vlan_control_1_fields,
                                egr_vlan_control_1_values);
    }
}

/*
 * Function:
 *      pktdma_soc_set_up_vlan
 * Purpose:
 *      VLAN programming to set up packet flows.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tx_bitmap: 12-bit TX virtual channel bitmap.
 *      rx_bitmap: 12-bit RX virtual channel bitmap
 *      pkt_size: Packet Size
 *      test_vlan: VLAN on which CPU sends out pkts
 *      xlate_vlan_start: Starting VLAN feom which 10 consecutive VLANs are
 *                        available for VLAN xlation.
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_set_up_vlan(int unit, uint32 tx_bitmap, uint32 rx_bitmap,
                       uint32 pkt_size, bcm_vlan_t test_vlan,
                       bcm_vlan_t xlate_vlan_start)
{
    pbmp_t pbm, ubm;
    int32 port, i;
    uint32 *port_list;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    port_list = (uint32 *)(sal_alloc(pktdma_p->num_lb_ports_param *
                                        sizeof(uint32), "port_list"));
    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        if (i < pktdma_p->num_lb_ports_param) {
            port_list[i] = port;
            i++;
        }
    }

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_CLEAR(ubm);

    cli_out("\nSetting up VLANs");


    if (rx_bitmap == 0x000) {
        cli_out("\nrx_bitmap = 0x000, Test is a TX only test");
        BCM_PBMP_PORT_ADD(pbm, port_list[1]);
        BCM_PBMP_PORT_ADD(pbm, 0);
        bcm_vlan_create(unit, test_vlan);
        bcm_vlan_create(unit, xlate_vlan_start);
        bcm_vlan_port_add(unit, xlate_vlan_start, pbm, ubm);
        bcm_vlan_port_add(unit, test_vlan, pbm, ubm);
        bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
        bcm_vlan_translate_add(unit, port_list[1], test_vlan,
                               xlate_vlan_start, 0);
    } else if (tx_bitmap == 0x000) {
        cli_out
            ("\ntx_bitmap = 0x000, Test is a RX only test."
             "The test needs to be done with a TG");
        BCM_PBMP_CLEAR(pbm);
        BCM_PBMP_PORT_ADD(pbm, port_list[1]);
        BCM_PBMP_PORT_ADD(pbm, 0);
        BCM_PBMP_PORT_ADD(pbm, port_list[2]);
        bcm_vlan_create(unit, test_vlan);
        bcm_vlan_port_add(unit, test_vlan, pbm, ubm);
    } else {
        cli_out("\nThis is a loopback test");
        if (pktdma_p->stream_param_tx == 1 || pktdma_p->stream_param_rx == 1
            || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
            || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
            cli_out("\nSetting up VLAN xlates for perf test");
            bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
            for (port = 0; port < pktdma_p->num_lb_ports_param; port++) {
                BCM_PBMP_CLEAR(pbm);
                BCM_PBMP_PORT_ADD(pbm, 0);
                BCM_PBMP_PORT_ADD(pbm, port_list[port]);
                bcm_vlan_create(unit, (xlate_vlan_start + port));
                bcm_vlan_port_add(unit, (xlate_vlan_start + port), pbm, ubm);
                bcm_vlan_translate_add(unit, port_list[port], test_vlan,
                                       xlate_vlan_start + port, 0);
            }
        }

        BCM_PBMP_CLEAR(pbm);
        if (pktdma_p->stream_param_tx == 1 || pktdma_p->stream_param_rx == 1
            || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
            || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
            for (port = 0; port < pktdma_p->num_lb_ports_param; port++) {
                BCM_PBMP_PORT_ADD(pbm, port_list[port]);
            }
        } else {
            BCM_PBMP_PORT_ADD(pbm, port_list[1]);
            BCM_PBMP_PORT_ADD(pbm, 0);
        }
        bcm_vlan_create(unit, test_vlan);
        bcm_vlan_port_add(unit, test_vlan, pbm, ubm);
    }
}

/*
 * Function:
 *      pktdma_soc_turn_off_fc
 * Purpose:
 *      Turn off flow control at the MAC, IDB and MMU.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_turn_off_fc(int unit)
{
    int p;
    int obm_subp;
    int idx;
    uint32 i;
    int rv;
    uint64 idb_fc;
    pktdma_t *pktdma_p = pktdma_parray[unit];

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
        rv = soc_reg_get(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
                         0, 0, &idb_fc);

        if (SOC_FAILURE(rv)) {
            test_error(unit, "\nError writing reg "
                                "IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0");
            pktdma_p->test_fail = 1;
        }
    }

    for (obm_subp = 0; obm_subp < NUM_SUBP_OBM; obm_subp++) {
        for (idx = 0; idx < (sizeof(idb_fcc_regs) / sizeof(soc_reg_t)); idx++) {
            if(SOC_REG_IS_VALID(unit, idb_fcc_regs[idx])) {
                rv = soc_reg_set(unit, idb_fcc_regs[idx], 0, obm_subp, idb_fc);

                if (SOC_FAILURE(rv)) {
                    test_error(unit, "\nError writing reg %s",
                                        SOC_REG_NAME(unit, idb_fcc_regs[idx]));
                    pktdma_p->test_fail = 1;
                }
            }
        }
    }

    for (idx = 0; idx < (sizeof(pgw_fcc_regs) / sizeof(soc_reg_t)); idx++) {
        for (i = 0; i < TD2P_PGWS_PER_DEV; i++) {
            if (SOC_REG_IS_VALID(unit, pgw_fcc_regs[idx])) {
                soc_reg_field32_modify(unit, pgw_fcc_regs[idx],
                                       (i | SOC_REG_ADDR_INSTANCE_MASK),
                                        PORT_FC_ENABLEf, 0x0);
            }
        }
    }
}

typedef struct pkt_per_channel_s {
    uint32 txpkt_per_channel_start[N_DMA_CHAN];
    uint32 rxpkt_per_channel_start[N_DMA_CHAN];
    uint32 txpkt_per_channel_end[N_DMA_CHAN];
    uint32 rxpkt_per_channel_end[N_DMA_CHAN];
} pkt_per_channel_t;

/*
 * Function:
 *      pktdma_soc_check_for_stall
 * Purpose:
 *      Check for stalls on TX/RX DMA channels
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pkt_size: Packet size in bytes
 *      interval_in_seconds: Interval over which rate is measured
 *
 * Returns:
 *     Nothing
 */
static int
pktdma_soc_check_for_stall(int unit)
{
    uint32 cmc, ch;
    pkt_per_channel_t *pkt_per_ch;
    int pkt_per_ch_size;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pkt_per_ch_size = sizeof(pkt_per_channel_t) * SOC_CMCS_NUM(unit);
    pkt_per_ch = sal_alloc(pkt_per_ch_size, "pkt_per_channel");
    if (pkt_per_ch == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(pkt_per_ch, 0x0, pkt_per_ch_size);

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            pkt_per_ch[cmc].txpkt_per_channel_start[ch] =
                soc_pci_read(unit,
                             CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, ch));
            pkt_per_ch[cmc].rxpkt_per_channel_start[ch] =
                soc_pci_read(unit,
                             CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, ch));
        }
    }

    cli_out("\nChecking for stalls, wait 1s");
    sal_usleep(1000000);

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            pkt_per_ch[cmc].txpkt_per_channel_end[ch] =
                soc_pci_read(unit,
                             CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, ch));
            pkt_per_ch[cmc].rxpkt_per_channel_end[ch] =
                soc_pci_read(unit,
                             CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, ch));

            if ((pkt_per_ch[cmc].txpkt_per_channel_end[ch] != 0)
                && (pkt_per_ch[cmc].txpkt_per_channel_end[ch]
                        == pkt_per_ch[cmc].txpkt_per_channel_start[ch])) {
                test_error(unit, "\nTXDMA on CMC %0d, CH %0d stalled", cmc, ch);
                pktdma_p->test_fail = 1;
            }

            if ((pkt_per_ch[cmc].rxpkt_per_channel_end[ch] != 0)
                && (pkt_per_ch[cmc].rxpkt_per_channel_end[ch]
                        == pkt_per_ch[cmc].rxpkt_per_channel_start[ch])) {
                test_error(unit, "\nRXDMA on CMC %0d, CH %0d stalled", cmc, ch);
                pktdma_p->test_fail = 1;
            }
        }
    }
    sal_free(pkt_per_ch);
    return SOC_E_NONE;
}

/*
 * Function:
 *      ctr_to_bps
 * Purpose:
 *      Calculate rate in bps from packet counter values
 * Parameters:
 *      initial: initial packet counter value
 *      final: final packet counter value
 *      bytes: bytes per pkt (includes IPG and preamble)
 *      interval_in_seconds: Interval over which rate is measured
 *
 * Returns:
 *     Rate in bps
 */

static void
ctr_to_bps(uint64 initial, uint64 final, uint32 bytes_per_pkt,
           uint32 interval_in_seconds, uint64 *bps) {
    uint64 ctr_delta;
    uint64 interval_64;

    COMPILER_64_SET(ctr_delta, COMPILER_64_HI(final), COMPILER_64_LO(final));
    COMPILER_64_SUB_64(ctr_delta, initial);
    COMPILER_64_SET(interval_64, 0x0, interval_in_seconds);

    COMPILER_64_SET(*bps, COMPILER_64_HI(ctr_delta), COMPILER_64_LO(ctr_delta));
    COMPILER_64_UMUL_32(*bps, (bytes_per_pkt * 8));
    COMPILER_64_UDIV_64(*bps, interval_64);
}

/*
 * Function:
 *      ctr_to_pps
 * Purpose:
 *      Calculate rate in pps from packet counter values
 * Parameters:
 *      initial: initial packet counter value
 *      final: final packet counter value
 *      interval_in_seconds: Interval over which rate is measured
 *
 * Returns:
 *     Rate in pps
 */

static void
ctr_to_pps(uint64 initial, uint64 final, uint32 interval_in_seconds,
           uint64 *pps) {
    uint64 ctr_delta;
    uint64 interval_64;

    COMPILER_64_SET(ctr_delta, COMPILER_64_HI(final), COMPILER_64_LO(final));
    COMPILER_64_SUB_64(ctr_delta, initial);
    COMPILER_64_SET(interval_64, 0x0, interval_in_seconds);

    COMPILER_64_SET(*pps, COMPILER_64_HI(ctr_delta), COMPILER_64_LO(ctr_delta));
    COMPILER_64_UDIV_64(*pps, interval_64);
}

typedef struct pkt_per_channel64_s {
    uint64 txpkt_per_channel_start[N_DMA_CHAN];
    uint64 rxpkt_per_channel_start[N_DMA_CHAN];
    uint64 txpkt_per_cmc_start;
    uint64 rxpkt_per_cmc_start;
    uint64 txpkt_per_channel_end[N_DMA_CHAN];
    uint64 rxpkt_per_channel_end[N_DMA_CHAN];
    uint64 txpkt_per_cmc_end;
    uint64 rxpkt_per_cmc_end;
} pkt_per_channel64_t;

/*
 * Function:
 *      pktdma_soc_calc_rate_bps
 * Purpose:
 *      Calculate TX and RX data rates in bps for fixed size packets and in
 *      pps (packets per second) for random sized packets
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pkt_size: Packet size in bytes
 *      interval_in_seconds: Interval over which rate is measured
 *
 * Returns:
 *     Nothing
 */
static int
pktdma_soc_calc_rate_bps(int unit, uint32 pkt_size, uint32 interval_in_seconds)
{
    uint32 cmc, ch;
    pkt_per_channel64_t *pkt_per_ch;
    int pkt_per_ch_size;
    uint64 total_txpkt_start;
    uint64 total_rxpkt_start;
    uint64 total_txpkt_end;
    uint64 total_rxpkt_end;
    uint64 bps, pps;
    char bps_str[32], pps_str[32];
    pktdma_t *pktdma_p = pktdma_parray[unit];

    pkt_per_ch_size = sizeof(pkt_per_channel64_t) * SOC_CMCS_NUM(unit);
    pkt_per_ch = sal_alloc(pkt_per_ch_size, "pkt_per_channel");
    if (pkt_per_ch == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(pkt_per_ch, 0x0, pkt_per_ch_size);

    COMPILER_64_ZERO(total_txpkt_start);
    COMPILER_64_ZERO(total_rxpkt_start);
    COMPILER_64_ZERO(total_txpkt_end);
    COMPILER_64_ZERO(total_rxpkt_end);

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            COMPILER_64_SET(pkt_per_ch[cmc].txpkt_per_channel_start[ch], 0x0,
                            soc_pci_read(unit,
                                CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, ch)));
            COMPILER_64_SET(pkt_per_ch[cmc].rxpkt_per_channel_start[ch], 0x0,
                            soc_pci_read(unit,
                                CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, ch)));
        }
        COMPILER_64_SET(pkt_per_ch[cmc].txpkt_per_cmc_start, 0x0,
                        soc_pci_read(unit,
                                     CMIC_CMCx_PKT_COUNT_TXPKT_OFFSET(cmc)));
        COMPILER_64_SET(pkt_per_ch[cmc].rxpkt_per_cmc_start, 0x0,
                        soc_pci_read(unit,
                                     CMIC_CMCx_PKT_COUNT_RXPKT_OFFSET(cmc)));

        COMPILER_64_ADD_64(total_txpkt_start, pkt_per_ch[cmc].txpkt_per_cmc_start);
        COMPILER_64_ADD_64(total_rxpkt_start, pkt_per_ch[cmc].rxpkt_per_cmc_start);
    }

    cli_out("\nMeasuring rate over a interval of %0d seconds ...",
            interval_in_seconds);
    sal_usleep(interval_in_seconds * 1000000);

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            COMPILER_64_SET(pkt_per_ch[cmc].txpkt_per_channel_end[ch], 0x0,
                            soc_pci_read(unit,
                                CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, ch)));
            COMPILER_64_SET(pkt_per_ch[cmc].rxpkt_per_channel_end[ch], 0x0,
                            soc_pci_read(unit,
                                CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, ch)));
        }
        COMPILER_64_SET(pkt_per_ch[cmc].txpkt_per_cmc_end, 0x0,
                        soc_pci_read(unit,
                                     CMIC_CMCx_PKT_COUNT_TXPKT_OFFSET(cmc)));
        COMPILER_64_SET(pkt_per_ch[cmc].rxpkt_per_cmc_end, 0x0,
                        soc_pci_read(unit,
                                     CMIC_CMCx_PKT_COUNT_RXPKT_OFFSET(cmc)));

        COMPILER_64_ADD_64(total_txpkt_end, pkt_per_ch[cmc].txpkt_per_cmc_end);
        COMPILER_64_ADD_64(total_rxpkt_end, pkt_per_ch[cmc].rxpkt_per_cmc_end);
    }

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            if ((((pktdma_p->tx_bitmap_param >> (N_DMA_CHAN * cmc + ch)) % 2)
                != 0) && COMPILER_64_IS_ZERO(pkt_per_ch[cmc].txpkt_per_channel_end[ch])) {
                test_error(unit, "\nTXDMA enabled on CMC %0d, CH %0d, but "
                                 "no packets sent", cmc, ch);
                pktdma_p->test_fail = 1;
            }

            if ((((pktdma_p->rx_bitmap_param >> (N_DMA_CHAN * cmc + ch)) % 2)
                != 0) && COMPILER_64_IS_ZERO(pkt_per_ch[cmc].rxpkt_per_channel_end[ch])) {
                test_error(unit, "\nRXDMA enabled on CMC %0d, CH %0d, but "
                                 "no packets received", cmc, ch);
                pktdma_p->test_fail = 1;
            }
        }
    }

    if (pktdma_p->pkt_size_param != 0) {

        cli_out("\n------------------ TX Rates -------------------\n");
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
            for (ch = 0; ch < N_DMA_CHAN; ch++) {
                ctr_to_bps(pkt_per_ch[cmc].txpkt_per_channel_start[ch],
                           pkt_per_ch[cmc].txpkt_per_channel_end[ch],
                           (pkt_size + ENET_IPG + ENET_PREAMBLE),
                           interval_in_seconds, &bps);
                format_uint64_decimal(bps_str, bps, 0);
                cli_out("\nCMC%0d, CH%0d : %s bps", cmc, ch, bps_str);
            }
            ctr_to_bps(pkt_per_ch[cmc].txpkt_per_cmc_start,
                       pkt_per_ch[cmc].txpkt_per_cmc_end,
                       (pkt_size + ENET_IPG + ENET_PREAMBLE),
                       interval_in_seconds, &bps);
            format_uint64_decimal(bps_str, bps, 0);
            cli_out("\n\nCMC%0d : %s bps\n\n", cmc, bps_str);
        }
        ctr_to_bps(total_txpkt_start,
                            total_txpkt_end,
                            (pkt_size + ENET_IPG + ENET_PREAMBLE),
                            interval_in_seconds, &bps);
        format_uint64_decimal(bps_str, bps, 0);
        cli_out("\n****Total TX rate from CMIC : %s bps", bps_str);

        cli_out("\n------------------ RX Rates -------------------\n");
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
            for (ch = 0; ch < N_DMA_CHAN; ch++) {
                ctr_to_bps(pkt_per_ch[cmc].rxpkt_per_channel_start[ch],
                           pkt_per_ch[cmc].rxpkt_per_channel_end[ch],
                           (pkt_size + ENET_IPG + ENET_PREAMBLE),
                           interval_in_seconds, &bps);
                format_uint64_decimal(bps_str, bps, 0);
                cli_out("\nCMC%0d, CH%0d : %s bps", cmc, ch, bps_str);
            }
            ctr_to_bps(pkt_per_ch[cmc].rxpkt_per_cmc_start,
                       pkt_per_ch[cmc].rxpkt_per_cmc_end,
                       (pkt_size + ENET_IPG + ENET_PREAMBLE),
                       interval_in_seconds, &bps);
            format_uint64_decimal(bps_str, bps, 0);
            cli_out("\n\nCMC%0d : %s bps\n\n", cmc, bps_str);
        }
        ctr_to_bps(total_rxpkt_start,
                            total_rxpkt_end,
                            (pkt_size + ENET_IPG + ENET_PREAMBLE),
                            interval_in_seconds, &bps);
        format_uint64_decimal(bps_str, bps, 0);
        cli_out("\n****Total RX rate from CMIC : %s bps\n\n", bps_str);
    } else {
        cli_out("\n------------------ TX Packet Rates -------------------\n");
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
            for (ch = 0; ch < N_DMA_CHAN; ch++) {
                ctr_to_pps(pkt_per_ch[cmc].txpkt_per_channel_start[ch],
                           pkt_per_ch[cmc].txpkt_per_channel_end[ch],
                           interval_in_seconds, &pps);
                format_uint64_decimal(pps_str, pps, 0);
                cli_out("\nCMC%0d, CH%0d : %s pps", cmc, ch, pps_str);
            }
            ctr_to_pps(pkt_per_ch[cmc].txpkt_per_cmc_start,
                       pkt_per_ch[cmc].txpkt_per_cmc_end,
                       interval_in_seconds, &pps);
            format_uint64_decimal(pps_str, pps, 0);
            cli_out("\n\nCMC%0d : %s pps\n\n", cmc, pps_str);
        }
        ctr_to_pps(total_txpkt_start, total_txpkt_end,
                         interval_in_seconds, &pps);
        format_uint64_decimal(pps_str, pps, 0);
        cli_out("\n****Total TX rate from CMIC : %s pps", pps_str);

        cli_out("\n------------------ RX Packet Rates -------------------\n");
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
            for (ch = 0; ch < N_DMA_CHAN; ch++) {
                ctr_to_pps(pkt_per_ch[cmc].rxpkt_per_channel_start[ch],
                           pkt_per_ch[cmc].rxpkt_per_channel_end[ch],
                           interval_in_seconds, &pps);
                format_uint64_decimal(pps_str, pps, 0);
                cli_out("\nCMC%0d, CH%0d : %s pps", cmc, ch, pps_str);
            }
            ctr_to_pps(pkt_per_ch[cmc].rxpkt_per_cmc_start,
                       pkt_per_ch[cmc].rxpkt_per_cmc_end,
                       interval_in_seconds, &pps);
            format_uint64_decimal(pps_str, pps, 0);
            cli_out("\n\nCMC%0d : %s pps\n\n", cmc, pps_str);
        }
        ctr_to_pps(total_rxpkt_start, total_rxpkt_end,
                         interval_in_seconds, &pps);
        format_uint64_decimal(pps_str, pps, 0);
        cli_out("\n****Total TX rate from CMIC : %s pps", pps_str);
    }
    sal_free(pkt_per_ch);

    pktdma_soc_check_for_stall(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *      pktdma_soc_set_up_mac_lpbk
 * Purpose:
 *      Enable MAC loopback on all ports
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_set_up_mac_lpbk(int unit)
{
    int32 p;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_MAC);
    }
}

/*
 * Function:
 *      dump_first_dv
 * Purpose:
 *      Dump first DV for each channel
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
dump_first_dv(int unit)
{
    int32 channel;
    int32 dv_cont;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    cli_out("\nDumping first DV of each Channel:");
    cli_out("\n=================================");
    for (dv_cont = 0; dv_cont < NUM_CONT_DMA_DV; dv_cont++) {
        if (!((dv_cont > 0) && (pktdma_p->cont_dma == 0))) {
            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                cli_out("\nChannel %0d", channel);
                cli_out("\n-----------");
                cli_out("\n");
                if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
                    soc_dma_dump_dv(unit, "[dv_tx_dump]",
                                    pktdma_p->dv_tx_array[dv_cont][channel][0]);
                }
                if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
                    soc_dma_dump_dv(unit, "[dv_rx_dump]",
                                    pktdma_p->dv_rx_array[dv_cont][channel][0]);
                }
            }
        }
    }
    cli_out("\nEnd dumping first DV");
    cli_out("\n=====================");
}

/*
 * Function:
 *      dump_packet_pointers
 * Purpose:
 *      Dump packet pointers for each channel
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
dump_packet_pointers(int unit)
{
    int32 channel, i;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    cli_out("\nDumping packet pointers for each channel");
    cli_out("\n****************************************");

    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        cli_out("\nDumping TX packet pointers for channel %0d", channel);
        cli_out("\n------------------------------------------\n");

        if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
            for (i = 0; i < pktdma_p->num_pkts_param_tx; i++) {
                cli_out("%p ", pktdma_p->pkt_tx_array[channel][i]);
            }
        }
        cli_out("\nDumping RX packet pointers for channel %0d", channel);
        cli_out("\n------------------------------------------\n");
        if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
            for (i = 0; i < pktdma_p->num_pkts_param_rx; i++) {
                cli_out("%p ", pktdma_p->pkt_rx_array[channel][i]);
            }
        }
        cli_out("\n");
    }
    cli_out("\nDone dumping packet pointers");
    cli_out("\n*****************************************");
}

/*
 * Function:
 *      pktdma_soc_free_all_memory
 * Purpose:
 *      Free all allocated memory
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


static void
pktdma_soc_free_all_memory(int unit)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];
    int32 i, j, dv_cont;

    cli_out("\nFreeing all allocated memory");
    for (i = 0; i < MAX_CHANNELS; i++) {
        sal_free(pktdma_p->random_packet_sizes[i]);
    }
    sal_free(pktdma_p->random_packet_sizes);

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (((pktdma_p->tx_bitmap_param >> i) % 2) != 0) {
            for (j = 0; j < pktdma_p->num_pkts_param_tx; j++) {
                soc_cm_sfree(unit, pktdma_p->pkt_tx_array[i][j]);
            }
            soc_cm_sfree(unit, pktdma_p->pkt_tx_array[i]);
        }
    }
    soc_cm_sfree(unit, pktdma_p->pkt_tx_array);

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (((pktdma_p->rx_bitmap_param >> i) % 2) != 0) {
            for (j = 0; j < pktdma_p->num_pkts_param_rx; j++) {
                soc_cm_sfree(unit, pktdma_p->pkt_rx_array[i][j]);
            }
            soc_cm_sfree(unit, pktdma_p->pkt_rx_array[i]);
        }
    }
    soc_cm_sfree(unit, pktdma_p->pkt_rx_array);

    for (dv_cont = 0; dv_cont < NUM_CONT_DMA_DV; dv_cont++) {
        if (!((dv_cont > 0) && (pktdma_p->cont_dma == 0))) {
            for (i = 0; i < MAX_CHANNELS; i++) {
                if (((pktdma_p->tx_bitmap_param >> i) % 2) != 0) {
                    if (pktdma_p->chain_tx == 0) {
                        for (j = 0; j < pktdma_p->num_pkts_param_tx; j++) {
                            soc_dma_dv_free(unit,
                                            pktdma_p->
                                            dv_tx_array[dv_cont][i][j]);
                        }
                    } else {
                        soc_dma_dv_free(unit,
                                        pktdma_p->dv_tx_array[dv_cont][i][0]);
                    }
                    soc_at_free(unit, pktdma_p->dv_tx_array[dv_cont][i]);
                }
            }
            soc_at_free(unit, pktdma_p->dv_tx_array[dv_cont]);

            for (i = 0; i < MAX_CHANNELS; i++) {
                if (((pktdma_p->rx_bitmap_param >> i) % 2) != 0) {
                    if (pktdma_p->chain_rx == 0) {
                        for (j = 0; j < pktdma_p->num_pkts_param_rx; j++) {
                            soc_dma_dv_free(unit,
                                            pktdma_p->
                                            dv_rx_array[dv_cont][i][j]);
                        }
                    } else {
                        soc_dma_dv_free(unit,
                                        pktdma_p->dv_rx_array[dv_cont][i][0]);
                    }
                    soc_at_free(unit, pktdma_p->dv_rx_array[dv_cont][i]);
                }
            }
            soc_at_free(unit, pktdma_p->dv_rx_array[dv_cont]);
        }
    }

    sal_free(pktdma_p->cmic_tx_counters);
    sal_free(pktdma_p->cmic_rx_counters);
}

/*
 * Function:
 *      dump_interrupt_counts
 * Purpose:
 *      Dump number of times each interrupt was raised.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
dump_interrupt_counts(int unit)
{
    int32 channel;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    cli_out("\nPrinting Interrupt counts");

    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
            cli_out("\nChannel %0d", channel);
            cli_out("\n-----------");
            cli_out("\ndesc_done_count_tx[%0d] = %0d", channel,
                    pktdma_p->desc_done_count_tx[channel]);
            cli_out("\nchain_done_count_tx[%0d] = %0d", channel,
                    pktdma_p->chain_done_count_tx[channel]);
            cli_out("\nreload_done_count_tx[%0d] = %0d", channel,
                    pktdma_p->reload_done_count_tx[channel]);
            cli_out("\n");
        }

        if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
            cli_out("\nChannel %0d", channel);
            cli_out("\n-----------");
            cli_out("\ndesc_done_count_rx[%0d] = %0d", channel,
                    pktdma_p->desc_done_count_rx[channel]);
            cli_out("\nchain_done_count_rx[%0d] = %0d", channel,
                    pktdma_p->chain_done_count_rx[channel]);
            cli_out("\nreload_done_count_rx[%0d] = %0d", channel,
                    pktdma_p->reload_done_count_rx[channel]);
            cli_out("\n");
        }
    }
}

/*
 * Function:
 *      set_up_all_dv
 * Purpose:
 *      Set up all DV arrays for all virtual channels.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
set_up_all_dv(int unit)
{
    int32 channel;
    int32 dv_cont;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    for (dv_cont = 0; dv_cont < NUM_CONT_DMA_DV; dv_cont++) {
        if (!((dv_cont > 0) && (pktdma_p->cont_dma == 0))) {
            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
                    set_up_dv(unit, pktdma_p->dv_tx_array[dv_cont][channel],
                              DV_TX, pktdma_p->chain_tx, pktdma_p->sg_tx,
                              pktdma_p->reload_tx, pktdma_p->poll_intr_param,
                              channel, pktdma_p->pkt_tx_array,
                              pktdma_p->pkt_size_param,
                              pktdma_p->num_pkts_param_tx,
                              pktdma_p->random_packet_sizes,
                              pktdma_p->source_ch, pktdma_p->cont_dma);
                } else if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
                    set_up_dv(unit, pktdma_p->dv_rx_array[dv_cont][channel],
                              DV_RX, pktdma_p->chain_rx, pktdma_p->sg_rx,
                              pktdma_p->reload_rx, pktdma_p->poll_intr_param,
                              channel, pktdma_p->pkt_rx_array,
                              pktdma_p->pkt_size_param,
                              pktdma_p->num_pkts_param_rx,
                              pktdma_p->random_packet_sizes,
                              pktdma_p->source_ch, pktdma_p->cont_dma);
                }
            }
        }
    }
}

/*
 * Function:
 *      pktdma_soc_check_packet_integrity
 * Purpose:
 *      Check packet integrity for finite packet counts
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
pktdma_soc_check_packet_integrity(int unit)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];
    int32 i, j, k;
    int32 num_bytes_compared;
    int32 match = 1;

    if (pktdma_p->rx_bitmap_param == 0x0 || pktdma_p->tx_bitmap_param == 0x0) {
        test_error
            (unit,
             "\n*ERROR: Cannot check packet integrity on a non-loopback test\n");
    } else if (pktdma_p->num_pkts_param_tx != pktdma_p->num_pkts_param_rx) {
        test_error
            (unit,
             "\n*ERROR: Cannot check packet integrity unless NumPktsTx == NumPktsRx\n");
    } else {
        for (i = 0; i < MAX_CHANNELS; i++) {
            if (((pktdma_p->rx_bitmap_param >> i) % 2) != 0) {
                if (pktdma_p->source_ch[i] != 99) {
                    cli_out
                        ("\nChecking Packet integrity for rx_ch = %0d"
                         " with source_ch = %0d",
                         i, pktdma_p->source_ch[i]);
                    for (j = 0; j < pktdma_p->num_pkts_param_rx; j++) {
                        if (pktdma_p->pkt_size_param <= 0) {
                            num_bytes_compared =
                                pktdma_p->random_packet_sizes[pktdma_p->
                                                              source_ch[i]][j];
                        } else {
                            num_bytes_compared = pktdma_p->pkt_size_param;
                        }
                        for (k = 0; k < num_bytes_compared; k++) {
                            if (pktdma_p->
                                pkt_tx_array[pktdma_p->source_ch[i]][j][k] !=
                                pktdma_p->pkt_rx_array[i][j][k]) {
                                match = 0;
                                cli_out
                                    ("\nMismatch : channel = %0d, "
                                     "packet = %0d, byte = %0d, "
                                     "tx_location = %p, rx_location = %p "
                                     "tx_byte = %02x, rx_byte = %02x",
                                     i, j, k,
                                     &pktdma_p->pkt_tx_array[pktdma_p->
                                                             source_ch[i]][j]
                                     [k], &pktdma_p->pkt_rx_array[i][j][k],
                                     pktdma_p->pkt_tx_array[pktdma_p->
                                                            source_ch[i]][j][k],
                                     pktdma_p->pkt_rx_array[i][j][k]);
                            }
                        }
                    }
                }
            }
        }
        if (match == 0) {
            test_error(unit,
                       "\n*ERROR: PACKET INTEGRITY CHECK FAILED\n");
            pktdma_p->test_fail = 1;
        } else {
            cli_out("\nPACKET INTEGRITY CHECK PASSED");
        }
    }
}

/*
 * Function:
 *      gather_cmic_packet_counters
 * Purpose:
 *      Gather CMIC TX and RX packet counters
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tx_counters - TX packet counter array
 *      rx_counters - RX packet counter array
 *
 * Returns:
 *     Nothing
 */

static void
gather_cmic_packet_counters(int unit, uint32 *tx_counters,
                            uint32 *rx_counters)
{
    int32 cmc, ch;

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            tx_counters[N_DMA_CHAN * cmc + ch] =
                soc_pci_read(unit,
                             CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(cmc, ch));
            rx_counters[N_DMA_CHAN * cmc + ch] =
                soc_pci_read(unit,
                             CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(cmc, ch));
        }
    }
}

/*
 * Function:
 *      check_pkt_counters
 * Purpose:
 *      Test end check for packet counter values.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
check_pkt_counters(int unit)
{
    int32 cmc, ch;
    int32 match = 1;
    int32 disable_tx_chk = 0;
    int32 disable_rx_chk = 0;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    gather_cmic_packet_counters(unit, pktdma_p->cmic_tx_counters,
                                pktdma_p->cmic_rx_counters);

    if (pktdma_p->stream_param_tx == 1
        || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD) {
        disable_tx_chk = 1;
        cli_out("\nStreaming enabled on TX, Disabling TX Pkt counter checks");
    }

    if (pktdma_p->stream_param_rx == 1
        || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
        disable_rx_chk = 1;
        cli_out("\nStreaming enabled on RX, Disabling RX Pkt counter checks");
    }

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            if ((((pktdma_p->tx_bitmap_param >> (N_DMA_CHAN * cmc + ch)) % 2) !=
                 0) && (disable_tx_chk == 0)) {
                if (pktdma_p->cmic_tx_counters[N_DMA_CHAN * cmc + ch] !=
                    pktdma_p->num_pkts_param_tx) {
                    cli_out("\n*ERROR: TX Counter mismatch for cmc=%0d, ch=%0d",
                            cmc, ch);
                    match = 0;
                }
            }

            if ((((pktdma_p->rx_bitmap_param >> (N_DMA_CHAN * cmc + ch)) % 2) !=
                 0) && (disable_rx_chk == 0)) {
                if (pktdma_p->cmic_rx_counters[N_DMA_CHAN * cmc + ch] !=
                    pktdma_p->num_pkts_param_rx) {
                    cli_out("\n*ERROR: RX Counter mismatch for cmc=%0d, ch=%0d",
                            cmc, ch);
                    match = 0;
                }
            }
        }
    }
    if ((disable_tx_chk == 0) || (disable_rx_chk == 0)) {
        if (match == 0) {
            test_error(unit, "\n*ERROR: PACKET COUNTER CHECKS FAILED\n");
            pktdma_p->test_fail = 1;
        } else {
            cli_out("\nPACKET COUNTER CHECKS PASSED");
        }
    }
}

/*
 * Function:
 *      check_interrupt_counters
 * Purpose:
 *      Test end check for interrupt count values.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
check_interrupt_counters(int unit)
{
    int32 cmc, ch;
    int32 match = 1;
    int32 disable_tx_chk = 0;
    int32 disable_rx_chk = 0;
    int32 sg_factor_tx;
    int32 sg_factor_rx;
    int32 exp_desc_done_cnt_tx;
    int32 exp_chain_done_cnt_tx;
    int32 exp_desc_done_cnt_rx;
    int32 exp_chain_done_cnt_rx;
    pktdma_t *pktdma_p = pktdma_parray[unit];

    if (pktdma_p->stream_param_tx == 1
        || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD) {
        disable_tx_chk = 1;
        cli_out("\nStreaming enabled on TX, Disabling TX Intr counter checks");
    }

    if (pktdma_p->stream_param_rx == 1
        || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
        disable_rx_chk = 1;
        cli_out("\nStreaming enabled on RX, Disabling RX Intr counter checks");
    }

    if (pktdma_p->sg_tx == 0) {
        sg_factor_tx = 1;
    }
    else {
        sg_factor_tx = 2;
    }

    if (pktdma_p->sg_rx == 0) {
        sg_factor_rx = 1;
    }
    else {
        sg_factor_rx = 2;
    }

    exp_desc_done_cnt_tx = pktdma_p->num_pkts_param_tx * sg_factor_tx;

    if (pktdma_p->chain_tx == 1) {
        exp_chain_done_cnt_tx = 1;
    }
    else {
        exp_chain_done_cnt_tx = pktdma_p->num_pkts_param_tx;
    }

    exp_desc_done_cnt_rx = pktdma_p->num_pkts_param_rx * sg_factor_rx;

    if (pktdma_p->chain_rx == 1) {
        exp_chain_done_cnt_rx = 1;
    }
    else {
        exp_chain_done_cnt_rx = pktdma_p->num_pkts_param_rx;
    }

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            if ((((pktdma_p->tx_bitmap_param >> (N_DMA_CHAN * cmc + ch)) % 2) !=
                 0) && (disable_tx_chk == 0)
                 && (pktdma_p->poll_intr_param == BOTH_DESC_CHAIN_INTR)) {
                if (pktdma_p->desc_done_count_tx[N_DMA_CHAN * cmc + ch] !=
                    exp_desc_done_cnt_tx) {
                    cli_out("\n*ERROR: Desc done interrupt count mismatch for"
                            " cmc=%0d, ch=%0d",
                            cmc, ch);
                    match = 0;
                }
                if( pktdma_p->cont_dma == 0) {
                    if (pktdma_p->chain_done_count_tx[N_DMA_CHAN * cmc + ch]
                            != exp_chain_done_cnt_tx) {
                        cli_out("\n*ERROR: Chain done interrupt count "
                        "mismatch for cmc=%0d, ch=%0d",
                        cmc, ch);
                        match = 0;
                    }
                }
            }
            if ((((pktdma_p->rx_bitmap_param >> (N_DMA_CHAN * cmc + ch)) % 2) !=
                 0) && (disable_rx_chk == 0)
                 && (pktdma_p->poll_intr_param == BOTH_DESC_CHAIN_INTR)) {
                if (pktdma_p->desc_done_count_rx[N_DMA_CHAN * cmc + ch] !=
                    exp_desc_done_cnt_rx) {
                    cli_out("\n*ERROR: Desc done interrupt count mismatch for"
                            " cmc=%0d, ch=%0d",
                            cmc, ch);
                    match = 0;
                }
                if( pktdma_p->cont_dma == 0) {
                    if (pktdma_p->chain_done_count_rx[N_DMA_CHAN * cmc + ch]
                            != exp_chain_done_cnt_rx) {
                        cli_out("\n*ERROR: Chain done interrupt count "
                        "mismatch for cmc=%0d, ch=%0d",
                        cmc, ch);
                        match = 0;
                    }
                }
            }
        }
    }
    if ((disable_tx_chk == 0) || (disable_rx_chk == 0)) {
        if (match == 0) {
            test_error(unit, "\n*ERROR: INTERRUPT COUNT CHECKS FAILED\n");
            pktdma_p->test_fail = 1;
        } else {
            cli_out("\nINTERRUPT COUNT CHECKS PASSED");
        }
    }
}

/*
 * Function:
 *      pktdma_soc_test_checks
 * Purpose:
 *      Test end checks.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


int
pktdma_soc_test_checks(int unit)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];
    int rv;

    if (pktdma_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling pktdma_soc_test_checks");

    if (pktdma_p->stream_param_rx == 1 || pktdma_p->stream_param_tx == 1
        || pktdma_p->test_type_param_tx == DESC_TX_CHAIN_PKT_RELOAD
        || pktdma_p->test_type_param_rx == DESC_RX_CHAIN_PKT_RELOAD) {
        cli_out("\nStreaming Enabled");
        cli_out("\nWait 2s for traffic to stabilize");
        sal_usleep(2000000);

        SOC_IF_ERROR_RETURN(pktdma_soc_calc_rate_bps(unit,
            pktdma_p->pkt_size_param, pktdma_p->rate_calc_interval_param));
        check_pkt_counters(unit);
        if (pktdma_p->poll_intr_param != POLL) {
            check_interrupt_counters(unit);
        }
    } else {
        sal_usleep(2000000);
        check_pkt_counters(unit);
        if (pktdma_p->check_packet_integrity_param == 1) {
            pktdma_soc_check_packet_integrity(unit);
        }
        if (pktdma_p->poll_intr_param != POLL) {
            check_interrupt_counters(unit);
        }
    }

    cli_out("\nKilling all DMA ops");

    pktdma_p->kill_dma = 1;

    sal_usleep(1000000);
    if(pktdma_p->tx_thread_done == 0) {
        test_error(unit, "\nTX thread still running");
        pktdma_p->test_fail = 1;
    }

    if(pktdma_p->rx_thread_done == 0) {
        test_error(unit, "\nRX thread still running");
        pktdma_p->test_fail = 1;
    }

    bcm_vlan_destroy_all(unit);
    if (pktdma_p->poll_intr_param != 0) {
        dump_interrupt_counts(unit);
    }

done:
    if(pktdma_p->bad_input == 1) {
        pktdma_p->test_fail = 1;
    }

    if (pktdma_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }
    else {
        rv = BCM_E_NONE;
    }

    cli_out("\n");
    return rv;
}

/*
 * Function:
 *      pktdma_soc_test_init
 * Purpose:
 *      Test init.Parses CLI params and allocates desc memory. Initializes most
 *      of test struct.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

int
pktdma_soc_test_init(int unit, args_t *a, void **pa)
{
    uint32 i;
    uint32 dv_cont;
    pktdma_t *pktdma_p;
    int32 channel = 0;

    pktdma_p = pktdma_parray[unit];

    pktdma_p = sal_alloc(sizeof(pktdma_t), "pktdma_test");

    if (pktdma_p == NULL) {
        test_error(unit, "\nERROR: Memory allocation unsuccessful\n");
        return -1;
    }
    sal_memset(pktdma_p, 0, sizeof(pktdma_t));
    pktdma_parray[unit] = pktdma_p;

    pktdma_p->kill_dma = 0;
    pktdma_p->tx_thread_done = 0;
    pktdma_p->rx_thread_done = 0;


    cli_out("\nStart pktdma_test");
    cli_out("\nCalling pktdma_soc_test_init\n");

    pktdma_p->test_fail = 0;
    /* coverity[dont_call : FALSE] */
    pktdma_p->pkt_seed = sal_rand();

    if (soc_property_get(unit, spn_PDMA_CONTINUOUS_MODE_ENABLE, 0)) {
        cli_out("\nContinuous DMA enabled");
        pktdma_p->cont_dma = 1;
    } else {
        cli_out("\nLegacy DMA - Cont DMA not enabled");
        pktdma_p->cont_dma = 0;
    }

    pktdma_soc_parse_test_params(unit, a);

    if (pktdma_p->bad_input == 1) {
        goto done;
    }

    if (pktdma_p->sv_override_param == 0) {
        bcm_vlan_destroy_all(unit);
        pktdma_soc_test_clean(unit, 0);
    }

    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        pktdma_p->chain_done_tx[channel] = 0;
        pktdma_p->chain_done_rx[channel] = 0;
        pktdma_p->reload_done_tx[channel] = 0;
        pktdma_p->reload_done_rx[channel] = 0;
        pktdma_p->chain_done_count_tx[channel] = 0;
        pktdma_p->chain_done_count_rx[channel] = 0;
        pktdma_p->reload_done_count_tx[channel] = 0;
        pktdma_p->reload_done_count_rx[channel] = 0;
    }

    set_global_desc_attr(unit);

    if(pktdma_p->sw_threads_off_param == 1) {
        cli_out("\nTurning off memscan");
        soc_mem_scan_stop(unit);

        cli_out("\nPausing linkscan");
        bcm_linkscan_enable_set(unit, 0);
    }

    cli_out("\nTurning off IDB/MMU FC");
    pktdma_soc_turn_off_fc(unit);

    if(pktdma_p->sw_threads_off_param == 1) {
        cli_out("\nStopping counter collection");
        soc_counter_stop(unit);
    }

    if (pktdma_p->sv_override_param == 0) {
        cli_out("\nSetting up MAC loopback on all ports");
        pktdma_soc_set_up_mac_lpbk(unit);

        cli_out("\nProgramming CPU_COS_MAP");
        pktdma_soc_program_cpu_cos_map(unit);

        cli_out("\nProgramming COS_MAP for 16 priorities");
        pktdma_soc_program_cos_map_pri16(unit);

        cli_out("\nSetting up VLANs/streams");
        pktdma_soc_set_up_vlan(unit, pktdma_p->tx_bitmap_param,
                            pktdma_p->rx_bitmap_param, pktdma_p->pkt_size_param,
                            (bcm_vlan_t) (VLAN), (bcm_vlan_t) (XLATE_VLAN));

        pktdma_init(unit, 1, pktdma_p->rx_bitmap_param,
                    pktdma_p->skip_cos_ctrl_bmp_param);
    }

    pktdma_soc_set_source_ch_array(unit, pktdma_p->tx_bitmap_param,
                                   pktdma_p->rx_bitmap_param);

    set_up_isr_table(unit);

    store_l2_packets(unit);

    soc_dma_init(unit);


    for (dv_cont = 0; dv_cont < NUM_CONT_DMA_DV; dv_cont++) {
        if (!((dv_cont > 0) && (pktdma_p->cont_dma == 0))) {
            pktdma_p->dv_tx_array[dv_cont] =
                (dv_t ***) soc_at_alloc(unit, MAX_CHANNELS * sizeof(dv_t **),
                                        "dv_tx_array[dv_cont]_alloc");
            pktdma_p->dv_rx_array[dv_cont] =
                (dv_t ***) soc_at_alloc(unit, MAX_CHANNELS * sizeof(dv_t **),
                                        "dv_rx_array[dv_cont]_alloc");

            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
                    pktdma_p->dv_tx_array[dv_cont][channel] =
                        (dv_t **) soc_at_alloc(unit,
                                               pktdma_p->num_pkts_param_tx *
                                               sizeof(dv_t *),
                                               "dv_tx_array[dv_cont]_alloc");
                    if (pktdma_p->chain_tx == 0) {
                        for (i = 0; i < pktdma_p->num_pkts_param_tx; i++) {
                            pktdma_p->dv_tx_array[dv_cont][channel][i] =
                                soc_dma_dv_alloc(unit, DV_TX,
                                             ((pktdma_p->sg_tx == 1) ? 3 : 2));
                        }
                    } else {
                        pktdma_p->dv_tx_array[dv_cont][channel][0] =
                            soc_dma_dv_alloc(unit, DV_TX,
                                                 ((pktdma_p->sg_tx ==
                                                   1) ? (2 *
                                                         (pktdma_p->
                                                          num_pkts_param_tx +
                                                          1)) : (pktdma_p->
                                                                 num_pkts_param_tx
                                                                 + 1)));
                    }
                }

                if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
                    pktdma_p->dv_rx_array[dv_cont][channel] =
                        (dv_t **) soc_at_alloc(unit,
                                               pktdma_p->num_pkts_param_rx *
                                               sizeof(dv_t *),
                                               "dv_rx_array[dv_cont]_alloc");
                    if (pktdma_p->chain_rx == 0) {
                        for (i = 0; i < pktdma_p->num_pkts_param_rx; i++) {
                            pktdma_p->dv_rx_array[dv_cont][channel][i] =
                                soc_dma_dv_alloc(unit, DV_RX,
                                                 ((pktdma_p->sg_rx ==
                                                   1) ? 3 : 2));
                        }
                    } else {
                        pktdma_p->dv_rx_array[dv_cont][channel][0] =
                            soc_dma_dv_alloc(unit, DV_RX,
                                             ((pktdma_p->sg_rx ==
                                               1) ? (2 *
                                                     (pktdma_p->
                                                      num_pkts_param_rx +
                                                      1)) : (pktdma_p->
                                                             num_pkts_param_rx +
                                                             1)));
                    }
                }
            }
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\npktdma_p->dv_tx_array[%0d] = %p"),
                      dv_cont, pktdma_p->dv_tx_array[dv_cont]));
            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                if (((pktdma_p->tx_bitmap_param >> channel) % 2) != 0) {
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U
                              (unit, "\npktdma_p->dv_tx_array[%0d][%0d] = %p"),
                              dv_cont, channel,
                              pktdma_p->dv_tx_array[dv_cont][channel]));
                    if (pktdma_p->chain_tx == 0) {
                        for (i = 0; i < pktdma_p->num_pkts_param_tx; i++) {
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                     (BSL_META_U
                                      (unit,
                                       "\npktdma_p->dv_tx_array[%0d][%0d][%0d]"
                                       "= %p"),
                                      dv_cont, channel, i,
                                      pktdma_p->
                                      dv_tx_array[dv_cont][channel][i]));
                        }
                    } else {
                        LOG_INFO(BSL_LS_APPL_TESTS,
                                 (BSL_META_U
                                  (unit,
                                   "\npktdma_p->dv_tx_array[%0d][%0d][%0d]"
                                   "= %p"),
                                  dv_cont, channel, 0,
                                  pktdma_p->dv_tx_array[dv_cont][channel][0]));
                    }
                }
            }

            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\npktdma_p->dv_rx_array[%0d] = %p"),
                      dv_cont, pktdma_p->dv_rx_array[dv_cont]));
            for (channel = 0; channel < MAX_CHANNELS; channel++) {
                if (((pktdma_p->rx_bitmap_param >> channel) % 2) != 0) {
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U
                              (unit, "\npktdma_p->dv_rx_array[%0d][%0d] = %p"),
                              dv_cont, channel,
                              pktdma_p->dv_rx_array[dv_cont][channel]));
                    if (pktdma_p->chain_rx == 0) {
                        for (i = 0; i < pktdma_p->num_pkts_param_rx; i++) {
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                     (BSL_META_U
                                      (unit,
                                       "\npktdma_p->dv_rx_array[%0d][%0d][%0d]"
                                       "= %p"),
                                      dv_cont, channel, i,
                                      pktdma_p->
                                      dv_rx_array[dv_cont][channel][i]));
                        }
                    } else {
                        LOG_INFO(BSL_LS_APPL_TESTS,
                                 (BSL_META_U
                                  (unit,
                                   "\npktdma_p->dv_rx_array[%0d][%0d][%0d]"
                                   "= %p"),
                                  dv_cont, channel, 0,
                                  pktdma_p->dv_rx_array[dv_cont][channel][0]));
                    }
                }
            }
        }
    }

    pktdma_p->cmic_tx_counters =
        (uint32 *) sal_alloc(SOC_CMCS_NUM(unit) * N_DMA_CHAN * sizeof(uint32),
                             "CMIC TX counters");
    pktdma_p->cmic_rx_counters =
        (uint32 *) sal_alloc(SOC_CMCS_NUM(unit) * N_DMA_CHAN * sizeof(uint32),
                             "CMIC RX counters");

    for (channel = 0; channel < MAX_CHANNELS; channel++) {
        pktdma_p->cmic_tx_counters[channel] = 0;
        pktdma_p->cmic_rx_counters[channel] = 0;
    }

done:
    return 0;
}

/*
 * Function:
 *      pktdma_soc_test
 * Purpose:
 *      Actual test. Kicks of TX and RX DMA threads.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
pktdma_soc_test(int unit, args_t *a, void *pa)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];
    int rv = 0;

    if (pktdma_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling pktdma_soc_test");
    set_up_all_dv(unit);
    config_dma_chan(unit, pktdma_p->tx_bitmap_param, pktdma_p->rx_bitmap_param,
                    pktdma_p->poll_intr_param);

    if (pktdma_p->verbose_param == 1) {
        dump_first_dv(unit);
        dump_packet_pointers(unit);
    }

    pktdma_p->pid_tx =
        sal_thread_create("TXDMA thread", 16 * 1024 * 1024, 200, txdma_thread,
                          INT_TO_PTR(unit));
    pktdma_p->pid_rx =
        sal_thread_create("RXDMA thread", 16 * 1024 * 1024, 200, rxdma_thread,
                          INT_TO_PTR(unit));

    cli_out("\npid_tx = %p, pid_rx= %p", (pktdma_p->pid_tx),
                                         (pktdma_p->pid_rx));

    rv = pktdma_soc_test_checks(unit);

done:
    return rv;
}

/*
 * Function:
 *      pktdma_soc_test_cleanup
 * Purpose:
 *      Test cleanup. Called at test end. Frees all allocated memory.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

int
pktdma_soc_test_cleanup(int unit, void *pa)
{
    pktdma_t *pktdma_p = pktdma_parray[unit];
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 vchan;
    sdc_t *sc;

    if (pktdma_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling pktdma_soc_test_cleanup\n");

    if (pktdma_p->cont_dma == 0) {
        soc_dma_abort(unit);
    } else {
        for (vchan = 0; vchan < MAX_CHANNELS; vchan++) {
            sc = &soc->soc_channels[vchan];
            while (sc->sc_q != NULL) {
                sc->sc_q->dv_channel = -sc->sc_q->dv_channel;
                sc->sc_q = sc->sc_q->dv_next;
                sc->sc_q_cnt--;
            }
            sc->sc_dv_active = NULL;
            sc->sc_q_tail   = NULL;
            sc->sc_q = NULL;
            sc->sc_q_cnt = 0;
        }
        (void)soc_reset_init(unit);
    }

    pktdma_soc_free_all_memory(unit);

done:

    sal_free(pktdma_p);

    cli_out("\n");
    return 0;
}
#endif /* BCM_ESW_SUPPORT && BCM_CMICM_SUPPORT */
