/*
 * $Id: l3uc.c,v 1.0 Broadcom SDK $
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
 * The l3uc test checks TDM and L3 unicast functionality and performance by
 * streaming IPv4/IPv6 packets on all ports at maximum rate. All ports are
 * configured in MAC loopback mode and each port is paired with a same speed
 * port. The test calculates the number of packets needed to saturate the ports
 * and send the IPv4/IPv6 packets from the CPU to each port pairs initially.
 * Then the packets are L3 switched between the port pairs indefinitely. The
 * MAC DA, MAC SA, and VLAN are changed every time a packet is L3 switched. Once
 * the traffic reaches steady state, rate calculation is done by dividing the
 * transmit packet count changes and transmit byte count changes over a
 * programmable interval. The rates are checked against expected rates based on
 * port configuration and oversubscription ratio. Finally, packet integrity
 * check is achieved by redirecting the packets back to the CPU and compared
 * against expected packets.
 *
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swill. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * RateCalcInt: Interval in seconds over which rate is to be calculated
 * TolLr: Rate tolerance percentage for linerate ports (1% by default).
 * TolOv: Rate tolerance percentage for oversubscribed ports (3% by default).
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable
                (default).
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set
 *              to 0 for random.
 * IPv6: Set to 1 to send IPv6 packets, 0 to send IPv4 packets (default).
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

#include "testlist.h"
#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3)

#include "gen_pkt.h"
#include "streaming_library.h"

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define IPV6_PARAM_DEFAULT 0

typedef struct l3uc_s {
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 loopback_mode_param;
    uint32 ipv6_param;
    uint32 dst_port[SOC_MAX_NUM_PORTS];
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
} l3uc_t;

static l3uc_t *l3uc_parray[SOC_MAX_NUM_DEVICES];
static rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES];

char l3uc_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "L3UC test usage:\n"
    " \n"
    "PktSize:     Packet size in bytes. Set to 1 for random packet sizes.\n"
    "             Set to 0 (default) for worst case packet sizes on all ports\n"
    "             (145B for ENET, 76B for HG2).\n"
    "FloodCnt:    Number of packets swirl between each port pair.\n"
    "             Set to 0 (default) for a lossless swirl at full rate.\n"
    "RateCalcInt: Interval in seconds over which rate is to be calculated.\n"
    "TolLr:       Rate tolerance percentage for line rate ports.\n"
    "             (1% by default)\n"
    "TolOv:       Rate tolerance percentage for oversubscribed ports.\n"
    "             (3% by default).\n"
    "ChkPktInteg: Set to 0 to disable packet integrity checks.\n"
    "             Set to 1 to enable (default).\n"
    "MaxNumCells: Maximum number of cells for random packet sizes.\n"
    "             Set to 0 for random cell sizes. (default is 4)\n"
    "LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.\n"
    "              (default is MAC loopback)\n"
    "IPv6:        Set to 1 to send IPv6 packets; 0 to send IPv4 (default).\n"
    ;
#endif

/*
 * Function:
 *      l3uc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      l3uc_p->bad_input set from here - tells test to crash out in case 
 *      CLI input combination is invalid.
 */

static void
l3uc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    l3uc_t *l3uc_p = l3uc_parray[unit];
    uint32 min_pkt_size = l3uc_p->ipv6_param ? MIN_IPV6_PKT_SIZE : MIN_PKT_SIZE;

    l3uc_p->bad_input = 0;

    l3uc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    l3uc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    l3uc_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    l3uc_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    l3uc_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    l3uc_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    l3uc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    l3uc_p->loopback_mode_param = BCM_PORT_LOOPBACK_MAC;
    l3uc_p->ipv6_param = IPV6_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "LoopbackMode", PQ_INT|PQ_DFL, 0,
                    &l3uc_p->loopback_mode_param, NULL);
    parse_table_add(&parse_table, "IPv6", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->ipv6_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        cli_out("%s", l3uc_test_usage);
        test_error(unit, "\n*ERROR PARSING ARGS\n");
    }

    cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
    cli_out("\nPktSize      = %0d", l3uc_p->pkt_size_param);
    cli_out("\nRateCalcInt  = %0d", l3uc_p->rate_calc_interval_param);
    cli_out("\nFloodCnt     = %0d", l3uc_p->flood_pkt_cnt_param);
    cli_out("\nTolLr        = %0d", l3uc_p->rate_tolerance_lr_param);
    cli_out("\nTolOv        = %0d", l3uc_p->rate_tolerance_ov_param);
    cli_out("\nChkPktInteg  = %0d", l3uc_p->check_packet_integrity_param);
    cli_out("\nMaxNumCells  = %0d", l3uc_p->max_num_cells_param);
    cli_out("\nLoopbackMode = %0d", l3uc_p->loopback_mode_param);
    cli_out("\nIPv6         = %0d", l3uc_p->ipv6_param);
    cli_out("\n -----------------------------------------------------");

    if (l3uc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        l3uc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (l3uc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (l3uc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (l3uc_p->pkt_size_param < min_pkt_size) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                min_pkt_size);
        l3uc_p->bad_input = 1;
    } else if (l3uc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size higher than %0dB (Device MTU)\n",
                MTU);
        l3uc_p->bad_input = 1;
    }

    if (l3uc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}


/*
 * Function:
 *      l3uc_set_up_streams
 * Purpose:
 *      VLAN programming for l3uc. Each port is put on an unique VLAN.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */

static bcm_error_t
l3uc_set_up_streams(int unit)
{
    int p, j;
    int src_port, dst_port;
    int l2_id;
    pbmp_t pbm, ubm;
    bcm_l2_station_t l2_station;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_id;
    bcm_l3_host_t l3_host;
    bcm_field_qset_t f_qset;
    bcm_field_group_t f_group;
    bcm_field_entry_t f_entry;
    l3uc_t *l3uc_p = l3uc_parray[unit];
    bcm_mac_t nhop_mac = MAC_DA;
    bcm_mac_t router_mac = MAC_SA;
    bcm_vlan_t vlan = VLAN;
    bcm_ip_t ipv4_da = IPV4_ADDR;
    bcm_ip6_t ipv6_da = IPV6_ADDR;

    BCM_PBMP_CLEAR(ubm);
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, 0, bcmPortControlIP4, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, 0, bcmPortControlIP6, 1));
    BCM_IF_ERROR_RETURN(bcm_field_init(unit));
    BCM_FIELD_QSET_INIT(f_qset);
    BCM_FIELD_QSET_ADD(f_qset, bcmFieldQualifyOuterVlanId);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, f_qset,
                                               BCM_FIELD_GROUP_PRIO_ANY,
                                               &f_group));

    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            src_port = p;
            dst_port = l3uc_p->dst_port[p];
            nhop_mac[2] = dst_port;
            router_mac[2] = src_port;
            vlan = VLAN + src_port;
            ipv4_da = IPV4_ADDR | (src_port << 8);
            ipv6_da[14] = src_port;
    
            /* Create VLAN */
            BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));
            BCM_PBMP_CLEAR(pbm);
            BCM_PBMP_PORT_ADD(pbm, src_port);
            BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbm, ubm));
    
            /* Add L2 Station */
            bcm_l2_station_t_init(&l2_station);
            sal_memcpy(l2_station.dst_mac, nhop_mac, 6);
            sal_memcpy(l2_station.dst_mac_mask, "ff:ff:ff:ff:ff:ff", 6);
            l2_station.vlan = vlan;
            l2_station.vlan_mask = 0xfff;
            l2_station.flags |= BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
            BCM_IF_ERROR_RETURN(bcm_l2_station_add(unit, &l2_id, &l2_station));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L2 Station: ")));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x, vlan 0x%x"),
                                 l2_station.dst_mac[5], l2_station.dst_mac[4],
                                 l2_station.dst_mac[3], l2_station.dst_mac[2],
                                 l2_station.dst_mac[1], l2_station.dst_mac[0],
                                 l2_station.vlan));
    
            /* Create L3 Interfaces */
            bcm_l3_intf_t_init(&l3_intf);
            l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
            sal_memcpy(l3_intf.l3a_mac_addr, router_mac, 6);
            l3_intf.l3a_vid = vlan;
            BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L3 Interface: intf %0d, "),
                                 l3_intf.l3a_intf_id));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x, vlan 0x%x"),
                                 l3_intf.l3a_mac_addr[5], l3_intf.l3a_mac_addr[4],
                                 l3_intf.l3a_mac_addr[3], l3_intf.l3a_mac_addr[2],
                                 l3_intf.l3a_mac_addr[1], l3_intf.l3a_mac_addr[0],
                                 l3_intf.l3a_vid));
    
            /* Create Next Hop Egress Object */
            bcm_l3_egress_t_init(&l3_egress);
            sal_memcpy(l3_egress.mac_addr, nhop_mac, 6);
            l3_egress.intf = l3_intf.l3a_intf_id;
            l3_egress.port = src_port;
            BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress,
                                                     &egr_obj_id));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L3 Next Hop: nhop_id %0d, intf %0d, "),
                                 egr_obj_id, l3_egress.intf));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x, port %0d"),
                                 l3_egress.mac_addr[5], l3_egress.mac_addr[4],
                                 l3_egress.mac_addr[3], l3_egress.mac_addr[2],
                                 l3_egress.mac_addr[1], l3_egress.mac_addr[0],
                                 l3_egress.port));
    
            /* Associate IP Address with Egress Object */
            bcm_l3_host_t_init(&l3_host);
            if (l3uc_p->ipv6_param) {
                l3_host.l3a_flags = BCM_L3_IP6;
                sal_memcpy(l3_host.l3a_ip6_addr, ipv6_da, 16);
            } else {
                l3_host.l3a_ip_addr = ipv4_da;
            }
            l3_host.l3a_intf = egr_obj_id;
            BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &l3_host));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L3 Table: nhop_id %0d, ip "),
                                 egr_obj_id));
            if (l3uc_p->ipv6_param) {
                for (j = 0; j < 16; j++) {
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U(unit, "%0d."),
                                         l3_host.l3a_ip6_addr[j]));
                }
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "%0d.%0d.%0d.%0d"),
                                     (l3_host.l3a_ip_addr) >> 24 & 0xff,
                                     (l3_host.l3a_ip_addr) >> 16 & 0xff,
                                     (l3_host.l3a_ip_addr) >> 8 & 0xff,
                                     l3_host.l3a_ip_addr & 0xff));
            }
    
            /* Program FP to L3 Switch */
            if (1) {
                BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, f_group, &f_entry));
                BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, f_entry,
                                                           VLAN + dst_port, 0xfff));
                BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, f_entry,
                                                         bcmFieldActionL3Switch,
                                                         egr_obj_id, 0));
                BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, f_entry));
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nAdded FP L3 Switch vlan 0x%x to "),
                                     VLAN + dst_port));
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "nhop_id %0d\n"), egr_obj_id));
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      l3uc_gen_random_ip_pkt
 * Purpose:
 *      Generate random IPv4/6 packet with seq ID
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

void
l3uc_gen_random_ip_pkt(int unit, uint8 *pkt_ptr, uint32 pkt_size,
                       uint8 mac_da[NUM_BYTES_MAC_ADDR],
                       uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                       uint16 vlan_id, uint32 ip_da, uint32 ip_sa,
                       uint32 seq_id, uint8 ttl)
{
    int i, offset;
    uint32 crc;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    tgp_gen_random_ip_pkt(l3uc_p->ipv6_param, pkt_ptr, pkt_size, mac_da, mac_sa,
                          vlan_id, ip_da, ip_sa, ttl);

    /* Replace First 4 Payload Bytes with Sequence ID */
    if (l3uc_p->ipv6_param) {
        offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV6_HDR;
    } else {
        offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV4_HDR;
    }

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
 *      l3uc_send_pkts
 * Purpose:
 *      Send packets to flood VLANs and create a swirl on each port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */

static void
l3uc_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint16 vlan;
    uint32 ip_da, ip_sa;
    uint8 *packet_store_ptr;
    uint32 pkt_size;
    int p, i, j;
    int src_port, dst_port;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_tx;
    int channel_done;
    int flags = 0;
    uint32 pkt_count = 0;
    uint32 flood_cnt;
    uint32 use_random_packet_sizes = 0;
    l3uc_t *l3uc_p = l3uc_parray[unit];
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    soc_dma_init(unit);

    dv_tx = soc_dma_dv_alloc(unit, DV_TX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nSending packets ...");

    i = 0;
    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            src_port = p;
            dst_port = l3uc_p->dst_port[p];
    
            if (l3uc_p->pkt_size_param == 0) {
                if (IS_HG_PORT(unit, p)) {
                    pkt_size = HG2_WC_PKT_SIZE;
                } else {
                    pkt_size = ENET_WC_PKT_SIZE;
                }
            } else {
                pkt_size = l3uc_p->pkt_size_param;
            }
    
            if (i % 2) {
                flood_cnt = 0;
            } else if (l3uc_p->flood_pkt_cnt_param == 0) {
                flood_cnt = lossless_flood_cnt(unit, pkt_size, p);
            } else {
                flood_cnt = l3uc_p->flood_pkt_cnt_param;
            }
            i++;
    
            if (pkt_size == 1) {
                use_random_packet_sizes = 1;
            }
    
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nflood_cnt from port %0d to port %0d = %0d"),
                      src_port, dst_port, flood_cnt));
    
            for (j = 0; j < flood_cnt; j++) {
                if (use_random_packet_sizes == 1) {
                    pkt_size = rate_calc_p->rand_pkt_sizes[p][j];
                }
                packet_store_ptr =
                    sal_dma_alloc(pkt_size * sizeof(uint8), "packet");
    
                pkt_count++;
                channel_done = 0;
                soc_dma_abort_dv(unit, dv_tx);
                sal_srand(l3uc_p->pkt_seed + p + j * 128);
                mac_da[2] = dst_port;
                mac_sa[2] = src_port;
                vlan = VLAN + src_port;
                ip_da = IPV4_ADDR | (dst_port << 8);
                ip_sa = IPV4_ADDR | (src_port << 8);
                l3uc_gen_random_ip_pkt(unit, packet_store_ptr, pkt_size, mac_da, mac_sa,
                                       vlan, ip_da, ip_sa, j, 255);
                soc_dma_dv_reset(DV_TX, dv_tx);
                soc_dma_desc_add(dv_tx, (sal_vaddr_t) (packet_store_ptr), pkt_size,
                                 lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
                soc_dma_desc_end_packet(dv_tx);
                soc_dma_chan_config(unit, TX_CHAN, DV_TX, SOC_DMA_F_POLL);
                soc_dma_start(unit, TX_CHAN, dv_tx);
    
                while (channel_done == 0) {
                    dma_chan_check_done(unit, TX_CHAN, SOC_DMA_POLL_CHAIN_DONE,
                                        &channel_done);
                }
                sal_dma_free(packet_store_ptr);
            }
        }
    }
    cli_out("\n%0d packets sent", pkt_count);
    soc_dma_abort_dv(unit, dv_tx);
    soc_dma_dv_reset(DV_TX, dv_tx);
    soc_dma_dv_free(unit, dv_tx);
}


/*
 * Function:
 *      l3uc_check_mib_counters
 * Purpose:
 *      Checks MIB counters in MAC, IP, and EP
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 *
 */

static bcm_error_t
l3uc_check_mib_counters(int unit)
{
    uint32 p, j;
    uint64 rdata;
    bcm_error_t rv = BCM_E_NONE;
    soc_reg_t good_counters[] = {
                                    RUCr,
                                    RPKTr,
                                    RUCAr,
                                    RVLNr,
                                    RBYTr,
                                    RIPC4r,
                                    TPKTr,
                                    TUCAr,
                                    TVLNr,
                                    TBYTr
                                };
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

    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            for (j = 0; j < (sizeof(good_counters) / sizeof(soc_reg_t)); j++) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, good_counters[j], p, 0,
                                                &rdata));
                if (COMPILER_64_IS_ZERO(rdata)) {
                    test_error (unit, "\n*ERROR: Counter %s has a zero value "
                                "for device port %0d ",
                                SOC_REG_NAME(unit, good_counters[j]), p);
                    rv = BCM_E_FAIL;
                }
            }
            for (j = 0; j < (sizeof(error_counters) / sizeof(soc_reg_t)); j++) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, error_counters[j], p, 0,
                                                &rdata));
                if (!COMPILER_64_IS_ZERO(rdata)) {
                    test_error (unit, "\n*ERROR: Error counter %s has a non zero value "
                                "for device port %0d ",
                                SOC_REG_NAME(unit, error_counters[j]), p);
                    rv = BCM_E_FAIL;
                }
            }
        }
    }
    if (rv != BCM_E_NONE) {
        test_error(unit, "********* COUNTER CHECK FAILED *********");
    } else {
        cli_out("\n********* COUNTER CHECK PASSED *********");
    }
    return rv;
}


/*
 * Function:
 *      l3uc_check_packet_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 *
 */

static bcm_error_t
l3uc_check_packet_integrity(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint16 vlan;
    uint32 ip_da, ip_sa;
    uint8 *ref_pkt_ptr;
    uint8 *rx_pkt_ptr;
    uint32 pkt_size;
    int i, j, k, p;
    int seq_id_offset;
    int src_port, dst_port;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_rx;
    int channel_done;
    int flags = 0;
    mtu_values_entry_t l3_mtu_values_entry;
    uint32 exp_pkt_count = 0;
    uint32 pkt_count = 0;
    uint32 pkt_count_port = 0;
    uint32 flood_cnt;
    uint32 timeout = RXDMA_TIMEOUT;
    uint32 pkt_match = 1;
    uint32 cnt_match = 1;
    uint32 seq_id;
    uint8 ttl;
    l3uc_t *l3uc_p = l3uc_parray[unit];

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
    soc_reg_field32_modify(unit, CPU_CONTROL_1r, 0, L3_MTU_FAIL_TOCPUf, 0x1);

    if (l3uc_p->ipv6_param) {
        seq_id_offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV6_HDR;
    } else {
        seq_id_offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV4_HDR;
    }

    ref_pkt_ptr = sal_dma_alloc(MTU * sizeof(uint8), "ref_packet");
    rx_pkt_ptr = sal_dma_alloc(MTU * sizeof(uint8), "rx_pkt");
    if (ref_pkt_ptr == NULL || rx_pkt_ptr == NULL) {
        if (ref_pkt_ptr) {
            sal_dma_free(ref_pkt_ptr);
        }
        if (rx_pkt_ptr) {
            sal_dma_free(rx_pkt_ptr);
        }
        test_error(unit, "Failed to allocate DMA memory for packet integrity check\n");
        return BCM_E_FAIL;
    }

    i = 0;
    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            src_port = p;
            dst_port = l3uc_p->dst_port[p];
    
            if (l3uc_p->pkt_size_param == 0) {
                if (IS_HG_PORT(unit, p)) {
                    pkt_size = HG2_WC_PKT_SIZE;
                } else {
                    pkt_size = ENET_WC_PKT_SIZE;
                }
            } else {
                pkt_size = l3uc_p->pkt_size_param;
            }
    
            if (i % 2) {
                flood_cnt = 0;
            } else if (l3uc_p->flood_pkt_cnt_param == 0) {
                flood_cnt = lossless_flood_cnt(unit, pkt_size, p);
            } else {
                flood_cnt = l3uc_p->flood_pkt_cnt_param;
            }
            i++;
            exp_pkt_count += flood_cnt;
    
            for (j = 0; j < flood_cnt; j++) {
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
                if (j == 0) {
                    SOC_IF_ERROR_RETURN(READ_L3_MTU_VALUESm(unit, MEM_BLOCK_ANY,
                                        VLAN + dst_port, &l3_mtu_values_entry));
                    soc_L3_MTU_VALUESm_field32_set(unit, &l3_mtu_values_entry,
                                                   MTU_SIZEf, 63);
                    SOC_IF_ERROR_RETURN(WRITE_L3_MTU_VALUESm(unit, MEM_BLOCK_ALL, 
                                                             VLAN + dst_port,
                                                             &l3_mtu_values_entry));
                }
    
                while (channel_done == 0 && timeout > 0) {
                    dma_chan_check_done(unit, RX_CHAN, SOC_DMA_POLL_CHAIN_DONE,
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
                    ttl = rx_pkt_ptr[NUM_BYTES_L2_HDR + 8];
                    sal_srand(l3uc_p->pkt_seed + p + seq_id * 128);
                    mac_da[2] = dst_port;
                    mac_sa[2] = src_port;
                    vlan = VLAN + src_port;
                    ip_da = IPV4_ADDR | (dst_port << 8);
                    ip_sa = IPV4_ADDR | (src_port << 8);
                    l3uc_gen_random_ip_pkt(unit, ref_pkt_ptr, pkt_size, mac_da, mac_sa,
                                           vlan, ip_da, ip_sa, seq_id, ttl);
    
                    for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                        if (rx_pkt_ptr[k] != ref_pkt_ptr[k]) {
                            pkt_match = 0;
                            test_error(unit, "\n*ERROR: Test port %0d, "
                                             "Device Port %0d has packet corruption"
                                             " on received pkt %0d",
                                              i, p, j);
                            cli_out("\nExpected Packet:");
                            tgp_print_pkt(ref_pkt_ptr, pkt_size);
                            cli_out("\nReceived Packet:");
                            tgp_print_pkt(rx_pkt_ptr, pkt_size);
                            l3uc_p->test_fail = 1;
                            break;
                        }
                    }
                }
    
                timeout = RXDMA_TIMEOUT;
    
                if (l3uc_p->test_fail) {
                    break;
                }
            }
    
            if (pkt_count_port < flood_cnt) {
                cli_out("\n*ERROR: Device Port %0d expected %0d pkts, received %0d packets",
                        p, flood_cnt, pkt_count_port);
                cnt_match = 0;
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nDevice port %0d received %0d packets"),
                          p, pkt_count_port));
            }
            pkt_count_port = 0;
    
            if (l3uc_p->test_fail) {
                break;
            }
        }
    }

    cli_out("\n%0d packets received", pkt_count);
    soc_dma_abort_dv(unit, dv_rx);
    soc_dma_dv_reset(DV_RX, dv_rx);
    soc_dma_dv_free(unit, dv_rx);

    if ((pkt_count == exp_pkt_count) && (pkt_match == 1) && (cnt_match == 1)) {
        cli_out("\n**** PACKET INTEGRITY CHECK PASSED *****");
    } else {
        test_error(unit, "**** PACKET INTEGRITY CHECK FAILED *****");
        l3uc_p->test_fail = 1;
        rv = BCM_E_FAIL;
    }

    sal_dma_free(rx_pkt_ptr);
    sal_dma_free(ref_pkt_ptr);
    return rv;
}


/*
 * Function:
 *      l3uc_test_init
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
l3uc_test_init(int unit, args_t *a, void **pa)
{
    int i, p, src_port;
    l3uc_t *l3uc_p;
    rate_calc_t *rate_calc_p;

    l3uc_p = l3uc_parray[unit];
    l3uc_p = sal_alloc(sizeof(l3uc_t), "l3uc_test");
    sal_memset(l3uc_p, 0, sizeof(l3uc_t));
    l3uc_parray[unit] = l3uc_p;

    rate_calc_p = rate_calc_parray[unit];
    rate_calc_p = sal_alloc(sizeof(rate_calc_t), "streaming_library");
    sal_memset(rate_calc_p, 0, sizeof(rate_calc_t));
    rate_calc_parray[unit] = rate_calc_p;

    cli_out("\nCalling l3uc_test_init");
    l3uc_parse_test_params(unit, a);

    for (p = 0; p < SOC_MAX_NUM_PORTS; p++) {
        l3uc_p->dst_port[p] = 0;
    }

    i = 0;
    src_port = 0;
    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            if (i % 2 == 0) {
                src_port = p;
            } else {
                l3uc_p->dst_port[src_port] = p;
                l3uc_p->dst_port[p] = src_port;
            }
            i++;
        }
    }

    l3uc_p->test_fail = 0;

    return BCM_E_NONE;
}    


/*
 * Function:
 *      l3uc_test
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
l3uc_test(int unit, args_t *a, void *pa)
{
    l3uc_t *l3uc_p;
    int rv = BCM_E_NONE;

    l3uc_p = l3uc_parray[unit];

    if (l3uc_p->bad_input) {
        goto done;
    }

    cli_out("\nCalling l3uc_test");

    soc_set_lpbk(unit, PBMP_PORT_ALL(unit), l3uc_p->loopback_mode_param);
    turn_off_cmic_mmu_bkp(unit);
    turn_off_fc(unit, PBMP_PORT_ALL(unit));
    l3uc_p->pkt_seed = sal_rand();
    set_port_property_arrays(unit, PBMP_PORT_ALL(unit), l3uc_p->pkt_size_param,
                             l3uc_p->max_num_cells_param);
    set_up_ports(unit, PBMP_PORT_ALL(unit));
    l3uc_set_up_streams(unit);
    l3uc_send_pkts(unit);

    cli_out("\nWait 2s for traffic to stabilize");
    sal_sleep(2);
    start_rate_measurement(unit, PBMP_PORT_ALL(unit));
    cli_out("\nMeasuring Rate over a period of %0ds",
            l3uc_p->rate_calc_interval_param);
    sal_sleep(l3uc_p->rate_calc_interval_param);

    if (l3uc_check_mib_counters(unit) != BCM_E_NONE) {
        l3uc_p->test_fail = 1;
    }
    if (check_rates(unit, PBMP_E_ALL(unit),
                   l3uc_p->rate_tolerance_lr_param,
                   l3uc_p->rate_tolerance_ov_param) != BCM_E_NONE) {
        l3uc_p->test_fail = 1;
    }
    if (l3uc_p->check_packet_integrity_param != 0) {
        rv = l3uc_check_packet_integrity(unit);
        if (rv != BCM_E_NONE) {
            l3uc_p->test_fail = 1;
        }
    }

done:
    return rv;
}


/*
 * Function:
 *      l3uc_test_cleanup
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
l3uc_test_cleanup(int unit, void *pa)
{
    l3uc_t *l3uc_p;
    int rv = BCM_E_NONE;

    l3uc_p = l3uc_parray[unit];

    cli_out("\nCalling l3uc_test_cleanup");

    if (l3uc_p->bad_input == 1) {
        l3uc_p->test_fail = 1;
    }

    if (l3uc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }

    sal_free(l3uc_p);
    bcm_vlan_init(unit);

    cli_out("\n");

    return rv;
}
#endif /* BCM_ESW_SUPPORT && INCLUDE_L3 */
