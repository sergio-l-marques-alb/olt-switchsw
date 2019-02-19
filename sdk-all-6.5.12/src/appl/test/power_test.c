/*
 * $Id: l3uc.c,v 1.0 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * The l3uc test checks TDM and L3 unicast functionality and performance by
 * streaming IPv4 packets on all ports at maximum rate. All ports are
 * configured in MAC loopback mode and each port is paired with a same speed
 * port. The test calculates the number of packets needed to saturate the ports
 * and send the IPv4 packets from the CPU to each port pairs initially.
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
 * TrafficLoad: Percentage of traffic load are allowed to pass through based on
 * port speed.
 * VfpEnable: Enable Vfp and fill it with miss entries.
 * EfpEnable: Enable Efp and fill it with miss entries.
 * VfpMatch: Program a hit entry in Vfp.
 * EfpMatch: Program a hit entry in Efp.
 * L3EcmpEnable: Enable ecmp for l3 routing.
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
#include "gen_pkt.h"
#include "streaming_lib.h"

#define RMAC_ADDR {0x00, 0x11, 0x40, 0x88, 0x00, 0xff}
#define MAC_BASE {0x00, 0x11, 0x40, 0x88, 0x00, 0xff}
#define MAC_DA {0x00, 0x11, 0x40, 0x88, 0x00, 0xff}
#define MAC_SA {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54}
#define VLAN 0x0a00
#define TTL 255
#define IPV4_ADDR 0xc0a80001
#define IPV6_ADDR { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                    0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01 };

#if defined(BCM_TRIDENT3_SUPPORT)

#define PKT_SIZE_PARAM_DEFAULT 1
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 0
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define IPV6_PARAM_DEFAULT 0
#define EMULATION_PARAM_DEFAULT 0
#define TEST_MAX_NUM_PORTS SOC_MAX_NUM_PORTS

typedef struct power_test_s {
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 loopback_mode_param;
    uint32 emulation_param;
    uint32 scaling_factor_param;
    uint32 traffic_load_param;
    uint32 max_num_ports_param;
    uint32 ipv6_param;
    uint32 dst_port[SOC_MAX_NUM_PORTS];
    uint32 dip[SOC_MAX_NUM_PORTS];
    uint32 bad_input;
    uint32 vfp_enable_param;
    uint32 efp_enable_param;
    uint32 vfp_match_param;
    uint32 efp_match_param;
    uint32 enable_l3_ecmp_param;
    uint32 enable_defip_param;
    uint32 enable_vlan_xlate_1_param;
    uint32 enable_l3_tunnel_param;
    int test_fail;
    uint32 pkt_seed;
    uint32 **rand_pkt_sizes;
} power_test_t;

static power_test_t *power_test_parray[SOC_MAX_NUM_DEVICES];
/* static rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES]; */

char power_test_usage[] =
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
    "TrafficLoad: Percentage of traffic load are allowed to pass through based on\n"
    "             port speed. (default is 100%).\n"
    "VfpEnable:   Enable Vfp and fill it with miss entries.\n"
    "             (default is on)\n"
    "EfpEnable:   Enable Efp and fill it with miss entries.\n"
    "             (default is on).\n"
    "VfpMatch:    Program a hit entry in Vfp.\n"
    "             (default is on).\n"
    "EfpMatch:    Program a hit entry in Efp.\n"
    "             (default is on).\n"
    "L3EcmpEnable:Enable ecmp for l3 routing\n"
    "             (default is on).\n"
    "DefipEnable: Enable defip for l3 routing and fill it with miss entries\n"
    "             (default is off).\n"
    "L3TunnelEnable: Enable l3 tunnel and fill it with miss entries\n"
    "             (default is off).\n"
    "VlanXlate1Enable : Enable vlan xlate 1 and fill it with miss entries \n"
    "             (default is off).\n"
    ;
#endif


/*
 * Function:
 *      power_test_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      power_test_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
power_test_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    power_test_t *power_test_p = power_test_parray[unit];
    uint32 min_pkt_size;

    /* default parameter value */
    power_test_p->bad_input = 0;
    power_test_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    power_test_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    power_test_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    power_test_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    power_test_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    power_test_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    power_test_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    power_test_p->max_num_ports_param = 132;
    power_test_p->loopback_mode_param = BCM_PORT_LOOPBACK_MAC;
    power_test_p->ipv6_param = IPV6_PARAM_DEFAULT;
    power_test_p->emulation_param = EMULATION_PARAM_DEFAULT;
    power_test_p->scaling_factor_param = 0;
    power_test_p->traffic_load_param = 0;
    power_test_p->vfp_enable_param = 1;
    power_test_p->efp_enable_param = 1;
    power_test_p->vfp_match_param  = 1;
    power_test_p->efp_match_param  = 1;
    power_test_p->enable_l3_ecmp_param = 1;
    power_test_p->enable_l3_tunnel_param = 0;
    power_test_p->enable_defip_param = 0;
    power_test_p->enable_vlan_xlate_1_param = 0;
    /*Parse CLI opts */
    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "MaxNumPorts", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->max_num_ports_param), NULL);
    parse_table_add(&parse_table, "LoopbackMode", PQ_INT|PQ_DFL, 0,
                    &power_test_p->loopback_mode_param, NULL);
    parse_table_add(&parse_table, "Emulation", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->emulation_param), NULL);
    parse_table_add(&parse_table, "ScalingFactor", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->scaling_factor_param), NULL);
    parse_table_add(&parse_table, "TrafficLoad", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->traffic_load_param), NULL);
    parse_table_add(&parse_table, "VfpEnable", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->vfp_enable_param), NULL);
    parse_table_add(&parse_table, "EfpEnable", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->efp_enable_param), NULL);
    parse_table_add(&parse_table, "VfpMatch", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->vfp_match_param), NULL);
    parse_table_add(&parse_table, "EfpMatch", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->efp_match_param), NULL);
    parse_table_add(&parse_table, "L3EcmpEnable", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->enable_l3_ecmp_param), NULL);
    parse_table_add(&parse_table, "L3TunnelEnable", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->enable_l3_tunnel_param), NULL);
    parse_table_add(&parse_table, "DefipEnable", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->enable_defip_param), NULL);
    parse_table_add(&parse_table, "VlanXlate1Enable", PQ_INT|PQ_DFL, 0,
                    &(power_test_p->enable_vlan_xlate_1_param), NULL);

    min_pkt_size = power_test_p->ipv6_param ? MIN_IPV6_PKT_SIZE : MIN_PKT_SIZE;

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        cli_out("%s", power_test_usage);
        test_error(unit, "\n*ERROR PARSING ARGS\n");
    }

    cli_out("\n");
    cli_out("------------- PRINTING TEST PARAMS ------------------\n");
    cli_out("PktSize        = %0d\n", power_test_p->pkt_size_param);
    cli_out("RateCalcInt    = %0d\n", power_test_p->rate_calc_interval_param);
    cli_out("FloodCnt       = %0d\n", power_test_p->flood_pkt_cnt_param);
    cli_out("TolLr          = %0d\n", power_test_p->rate_tolerance_lr_param);
    cli_out("TolOv          = %0d\n", power_test_p->rate_tolerance_ov_param);
    cli_out("ChkPktInteg    = %0d\n", power_test_p->check_packet_integrity_param);
    cli_out("MaxNumCells    = %0d\n", power_test_p->max_num_cells_param);
    cli_out("MaxNumPorts    = %0d\n", power_test_p->max_num_ports_param);
    cli_out("LoopbackMode   = %0d\n", power_test_p->loopback_mode_param);
    cli_out("Emulation      = %0d\n", power_test_p->emulation_param);
    cli_out("ScalingFactor  = %0d\n", power_test_p->scaling_factor_param);
    cli_out("TrafficLoad    = %0d\n", power_test_p->traffic_load_param);
    cli_out("VfpEnable      = %0d\n", power_test_p->vfp_enable_param);
    cli_out("EfpEnable      = %0d\n", power_test_p->efp_enable_param);
    cli_out("VfpMatch       = %0d\n", power_test_p->vfp_match_param);
    cli_out("EfpMatch       = %0d\n", power_test_p->efp_match_param);
    cli_out("L3EcmpEnable   = %0d\n", power_test_p->enable_l3_ecmp_param);
    cli_out("L3TunnelEnable = %0d\n", power_test_p->enable_l3_tunnel_param);
    cli_out("DefipEnable    = %0d\n", power_test_p->enable_defip_param);
    cli_out("VlanXlate1Enable    = %0d\n", power_test_p->enable_vlan_xlate_1_param);

    cli_out("-----------------------------------------------------\n");

    if (power_test_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        power_test_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (power_test_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 64B for Ethernet and HG2");
    } else if (power_test_p->pkt_size_param == 1) {
        cli_out("\nUsing max power packet sizes 115B + 251B");
    } else if (power_test_p->pkt_size_param < min_pkt_size) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                min_pkt_size);
        power_test_p->bad_input = 1;
    } else if (power_test_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size higher than %0dB (Device MTU)\n",
                MTU);
        power_test_p->bad_input = 1;
    }

    if (power_test_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}

/*
 * Function:
 *      power_test_set_port_property
 * Purpose:
 *      Set rand_pkt_sizes arrays.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      pkt_size_param - Packet size in bytes
 *      max_num_cells_param - Maximum number of cells
 *
 * Returns:
 *     Nothing
 */
static void
power_test_set_port_property(int unit, pbmp_t pbmp, int pkt_size_param,
                         int max_num_cells_param)
{
    int p, j;
    power_test_t *power_test_p = power_test_parray[unit];

    power_test_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (p = 0; p < SOC_MAX_NUM_PORTS; p++) {
        power_test_p->rand_pkt_sizes[p] =
            (uint32 *) sal_alloc(TARGET_CELL_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            for (j = 0; j < TARGET_CELL_COUNT; j++) {
                power_test_p->rand_pkt_sizes[p][j] = (j%2) ? 115: 251;
            }
        }
    }
}

/*
 * Function:
 *      power_test_set_up_ports
 * Purpose:
 *      Enable port bridging and HiGig lookup for HG2 ports
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *
 * Returns:
 *     Nothing
 *
 */
/*
static void
power_test_set_up_ports(int unit, pbmp_t pbmp)
{
    int i, p;
    lport_tab_entry_t lport_tab_entry;
    port_tab_entry_t port_tab_entry;
    soc_field_t ihg_lookup_fields[] = {
        HYBRID_MODE_ENABLEf,
        USE_MH_PKT_PRIf,
        USE_MH_VIDf,
        HG_LOOKUP_ENABLEf,
        REMOVE_MH_SRC_PORTf
    };
    uint32 ihg_lookup_values[] = { 0x0, 0x1, 0x1, 0x1, 0x0 };

    cli_out("\nEnabling HG_LOOKUP on HG ports");
    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS && IS_HG_PORT(unit, p)) {
            soc_reg_fields32_modify(unit, IHG_LOOKUPr, p, 5,
                                    ihg_lookup_fields, ihg_lookup_values);
        }
    }

    cli_out("\nEnabling Port bridging");
    for (i = 0; i < soc_mem_index_max(unit, LPORT_TABm); i++) {
        (void) soc_mem_read(unit, LPORT_TABm, COPYNO_ALL, i,
                     lport_tab_entry.entry_data);
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            ALLOW_SRC_MODf, 0x1);
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        (void) soc_mem_write(unit, LPORT_TABm, COPYNO_ALL, i,
                      lport_tab_entry.entry_data);
    }

    for (i = 0; i < soc_mem_index_max(unit, PORT_TABm); i++) {
        (void) soc_mem_read(unit, PORT_TABm, COPYNO_ALL, i,
                     port_tab_entry.entry_data);
        soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        (void) soc_mem_write(unit, PORT_TABm, COPYNO_ALL, i,
                          port_tab_entry.entry_data);
    }
}
*/

/*
 * Function:
 *      power_test_lossless_flood_cnt
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
power_test_lossless_flood_cnt(int unit, int port)
{
    uint32 pkt_size, flood_cnt = 0;
    int param_flood_cnt, param_pkt_size;
    power_test_t *power_test_p = power_test_parray[unit];

    param_flood_cnt = power_test_p->flood_pkt_cnt_param;
    param_pkt_size = power_test_p->pkt_size_param;

    if (param_flood_cnt == 0) {
        if (param_pkt_size == 0) {
            pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
        } else {
            pkt_size = param_pkt_size;
        }
        flood_cnt = stream_get_ll_flood_cnt(unit, port, pkt_size, NULL);
    } else {
        flood_cnt = param_flood_cnt;
    }

    return (flood_cnt);
}

int enable_flex_ctr(int unit) {
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    uint64 rval, val;
    int pool;

    for(pool = 0; pool < 20; pool++) {
        /* coverity[dont_call : FALSE] */
        switch(pool) {
            case 1 : mem = ING_FLEX_CTR_OFFSET_TABLE_1m; break;
            case 2 : mem = ING_FLEX_CTR_OFFSET_TABLE_2m; break;
            case 3 : mem = ING_FLEX_CTR_OFFSET_TABLE_3m; break;
            case 4 : mem = ING_FLEX_CTR_OFFSET_TABLE_4m; break;
            case 5 : mem = ING_FLEX_CTR_OFFSET_TABLE_5m; break;
            case 6 : mem = ING_FLEX_CTR_OFFSET_TABLE_6m; break;
            case 7 : mem = ING_FLEX_CTR_OFFSET_TABLE_7m; break;
            case 8 : mem = ING_FLEX_CTR_OFFSET_TABLE_8m; break;
            case 9 : mem = ING_FLEX_CTR_OFFSET_TABLE_9m; break;
            case 10 : mem = ING_FLEX_CTR_OFFSET_TABLE_10m; break;
            case 11 : mem = ING_FLEX_CTR_OFFSET_TABLE_11m; break;
            case 12 : mem = ING_FLEX_CTR_OFFSET_TABLE_12m; break;
            case 13 : mem = ING_FLEX_CTR_OFFSET_TABLE_13m; break;
            case 14 : mem = ING_FLEX_CTR_OFFSET_TABLE_14m; break;
            case 15 : mem = ING_FLEX_CTR_OFFSET_TABLE_15m; break;
            case 16 : mem = ING_FLEX_CTR_OFFSET_TABLE_16m; break;
            case 17 : mem = ING_FLEX_CTR_OFFSET_TABLE_17m; break;
            case 18 : mem = ING_FLEX_CTR_OFFSET_TABLE_18m; break;
            case 19 : mem = ING_FLEX_CTR_OFFSET_TABLE_19m; break;
            default : mem = ING_FLEX_CTR_OFFSET_TABLE_0m;
        }

        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, COUNT_ENABLEf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
        /* coverity[dont_call : FALSE] */
        switch(pool) {
            case 1 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r; break;
            case 2 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r; break;
            case 3 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r; break;
            case 4 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_4r; break;
            case 5 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_5r; break;
            case 6 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_6r; break;
            case 7 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_7r; break;
            case 8 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_8r; break;
            case 9 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_9r; break;
            case 10 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_10r; break;
            case 11 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_11r; break;
            case 12 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_12r; break;
            case 13 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_13r; break;
            case 14 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_14r; break;
            case 15 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_15r; break;
            case 16 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_16r; break;
            case 17 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_17r; break;
            case 18 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_18r; break;
            case 19 : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_19r; break;
            default : reg = ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r;
        }
        COMPILER_64_ZERO(rval); COMPILER_64_SET(val, 0, 1);
        soc_reg64_field_set(unit, reg, &rval, COUNTER_POOL_ENABLEf, val);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    for(pool = 0; pool < 4; pool++) {
        /* coverity[dont_call : FALSE] */
        switch(pool) {
            case 1 : mem = EGR_FLEX_CTR_OFFSET_TABLE_1m; break;
            case 2 : mem = EGR_FLEX_CTR_OFFSET_TABLE_2m; break;
            case 3 : mem = EGR_FLEX_CTR_OFFSET_TABLE_3m; break;
            default : mem = EGR_FLEX_CTR_OFFSET_TABLE_0m;
        }

        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, COUNT_ENABLEf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
        /* coverity[dont_call : FALSE] */
        switch(pool) {
            case 1 : reg = EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r; break;
            case 2 : reg = EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r; break;
            case 3 : reg = EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r; break;
            default : reg = EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r;
        }
        COMPILER_64_ZERO(rval); COMPILER_64_SET(val, 0, 1);
        soc_reg64_field_set(unit, reg, &rval, COUNTER_POOL_ENABLEf, val);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    mem = FLEX_CTR_ACTION_MASK_PROFILEm;

    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xfffff;
    soc_mem_field_set(unit, mem, entry, POOL_ENABLEf, memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    return SOC_E_NONE;
}

int setup_cos_map(int unit) {
    int i;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];

    mem = PORT_COS_MAPm;

    for(i = 0; i < 8; i++) {
      sal_memset(entry, 0, sizeof(entry));

      sal_memset(memfld, 0, sizeof(memfld));
      memfld[0] = i & 0x7;
      soc_mem_field_set(unit, mem, entry, UC_COS1f, memfld);

      sal_memset(memfld, 0, sizeof(memfld));
      memfld[0] = i & 0x7;
      soc_mem_field_set(unit, mem, entry, MC_COS1f, memfld);

      SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, entry));
    }

    return SOC_E_NONE;
}

static int get_mtro_refresh_cnt(int unit, int pipe, int mport, int load) {
    int ps_gbps, encap, refresh_count, port_speed, lport;

    if(mport >= 66) {
        assert(0);
        lport = 0;
    } else if(mport == 64) {
        lport = (pipe * 66);
    } else if(mport == 65) {
        lport = mport + (pipe * 66);
    } else {
        lport = (pipe * 66) + (mport + 1);
    }
    bcm_port_speed_get(unit, lport, &port_speed);
    encap = (IS_HG_PORT(unit, lport) | IS_HG2_ENABLED_PORT(unit, lport)) ?
                 BCM_PORT_ENCAP_HIGIG2 : BCM_PORT_ENCAP_IEEE;
    ps_gbps = 0;
    if (!SOC_IS_TOMAHAWKX(unit)) {
        ps_gbps = stream_get_exact_speed(port_speed, encap) / 1000;
    }

    refresh_count = (97657 * load * ps_gbps) / 10000;
    return refresh_count;
}

int enable_mtro(int unit) {
    int pipe, port, queue, index;
    int load, refresh_count;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    power_test_t *power_test_p = power_test_parray[unit];

    load = power_test_p->traffic_load_param;
    load = load ? load : 100;
    cli_out("Traffic Load used: %0d\n", load);

    for(pipe = 0; pipe < 2; pipe++) {
      for(port = 0; port < 64; port++) {
        refresh_count = get_mtro_refresh_cnt(unit, pipe, port, load);

        index = port;
        mem = pipe ? MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m : MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x7;
        soc_mem_field_set(unit, mem, entry, METER_GRANf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = refresh_count;
        soc_mem_field_set(unit, mem, entry, REFRESHf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x20;
        soc_mem_field_set(unit, mem, entry, THD_SELf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
      }
    }

    /*L0*/
    for(pipe = 0; pipe < 2; pipe++) {
      for(queue = 0; queue < 650; queue++) {
        refresh_count = get_mtro_refresh_cnt(unit, pipe, (queue/10), load);

        index = queue;
        mem = pipe ? MMU_MTRO_L0_MEM_PIPE1m : MMU_MTRO_L0_MEM_PIPE0m;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x7;
        soc_mem_field_set(unit, mem, entry, MAX_METER_GRANf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, MIN_METER_GRANf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = refresh_count;
        soc_mem_field_set(unit, mem, entry, MIN_REFRESHf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = refresh_count;
        soc_mem_field_set(unit, mem, entry, MAX_REFRESHf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x20;
        soc_mem_field_set(unit, mem, entry, MIN_THD_SELf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x20;
        soc_mem_field_set(unit, mem, entry, MAX_THD_SELf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
      }
    }

    /*L1*/
    for(pipe = 0; pipe < 2; pipe++) {
      for(queue = 0; queue < 650; queue++) {
        refresh_count = get_mtro_refresh_cnt(unit, pipe, (queue/10), load);

        index = queue;
        mem = pipe ? MMU_MTRO_L1_MEM_PIPE1m : MMU_MTRO_L1_MEM_PIPE0m;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x7;
        soc_mem_field_set(unit, mem, entry, MAX_METER_GRANf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, MIN_METER_GRANf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = refresh_count;
        soc_mem_field_set(unit, mem, entry, MIN_REFRESHf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = refresh_count;
        soc_mem_field_set(unit, mem, entry, MAX_REFRESHf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x20;
        soc_mem_field_set(unit, mem, entry, MIN_THD_SELf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x20;
        soc_mem_field_set(unit, mem, entry, MAX_THD_SELf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
      }
    }

    return SOC_E_NONE;
}

int enable_tcb(int unit) {
    int port;
    uint64 rval, val;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, TCB_CONFIGr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, TCB_ENABLEf, val);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, TRACK_CFAP_DROPf, val);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, TRACK_WRED_DROPf, val);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, TRACK_IADMN_DROPf, val);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, TRACK_EADMN_DROPf, val);
    COMPILER_64_SET(val, 0, 20);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, REV_COUNTERf, val);
    COMPILER_64_SET(val, 0, 65535);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, WD_TIMER_PRESETf, val);
    COMPILER_64_SET(val, 0, 15);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, PRE_TRIGGER_SAMPLE_RATEf, val);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, POST_TRIGGER_SAMPLE_RATEf, val);
    COMPILER_64_SET(val, 0, 4);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, SCOPE_ENTITY_TYPEf, val);
    COMPILER_64_SET(val, 0, 0);
    soc_reg64_field_set(unit, TCB_CONFIGr, &rval, SCOPE_ENTITYf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, TCB_CONFIGr, REG_PORT_ANY, 0, rval));

    for(port = 0; port < 8; port++) {
      SOC_IF_ERROR_RETURN(soc_reg_get(unit, TCB_THRESHOLD_PROFILEr, REG_PORT_ANY, port, &rval));
      COMPILER_64_SET(val, 0, 5);
      soc_reg64_field_set(unit, TCB_THRESHOLD_PROFILEr, &rval, CAPTURE_START_THRESHOLDf, val);
      COMPILER_64_SET(val, 0, 2);
      soc_reg64_field_set(unit, TCB_THRESHOLD_PROFILEr, &rval, CAPTURE_END_THRESHOLDf, val);
      SOC_IF_ERROR_RETURN(soc_reg_set(unit, TCB_THRESHOLD_PROFILEr, REG_PORT_ANY, port, rval));
    }

    return SOC_E_NONE;
}

int enable_wred(int unit) {
    int pipe, port, queue, index;
    uint64 rval, val;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, TIME_DOMAINr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, WRED_REFRESH_CONTROLr, REG_PORT_ANY, 0, rval));

    COMPILER_64_SET(rval, 0, 0xfff);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0x3ffff);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_0r, &rval, POOL_HI_CONG_LIMITf, val);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_0r, &rval, POOL_LOW_CONG_LIMITf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, WRED_POOL_INST_CONG_LIMIT_0r, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0x3ffff);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_1r, &rval, POOL_HI_CONG_LIMITf, val);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_1r, &rval, POOL_LOW_CONG_LIMITf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, WRED_POOL_INST_CONG_LIMIT_1r, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0x3ffff);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_2r, &rval, POOL_HI_CONG_LIMITf, val);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_2r, &rval, POOL_LOW_CONG_LIMITf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, WRED_POOL_INST_CONG_LIMIT_2r, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0x3ffff);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_3r, &rval, POOL_HI_CONG_LIMITf, val);
    soc_reg64_field_set(unit, WRED_POOL_INST_CONG_LIMIT_3r, &rval, POOL_LOW_CONG_LIMITf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, WRED_POOL_INST_CONG_LIMIT_3r, REG_PORT_ANY, 0, rval));

    mem = MMU_WRED_DROP_CURVE_PROFILE_0m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_0_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_0_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));


    mem = MMU_WRED_DROP_CURVE_PROFILE_1m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_1_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_1_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_2m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_2_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_2_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_3m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_3_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_3_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_4m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_4_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_4_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_5m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_5_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_5_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_6m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_6_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_6_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_7m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_7_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_7_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_8m;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_8_Am;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    mem = MMU_WRED_DROP_CURVE_PROFILE_8_Bm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MIN_THDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3ffff;
    soc_mem_field_set(unit, mem, entry, MAX_THDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    for(pipe = 0; pipe < 2; pipe++) {
      for(port = 0; port < 64; port++) {
        for(queue = 0; queue < 10; queue++) {
          index = port * 10 + queue;
          mem = pipe ? MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE1m : MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE0m;
          sal_memset(entry, 0, sizeof(entry));

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x1;
          soc_mem_field_set(unit, mem, entry, WRED_ENf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x1;
          soc_mem_field_set(unit, mem, entry, ECN_MARKINGf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, DROP_THD_NONTCPf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, DROP_THD_TCPf, memfld);

          SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));

          mem = pipe ? MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE1m : MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE0m;
          sal_memset(entry, 0, sizeof(entry));

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x1;
          soc_mem_field_set(unit, mem, entry, WRED_ENf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x1;
          soc_mem_field_set(unit, mem, entry, ECN_MARKINGf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, DROP_THD_NONTCPf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, DROP_THD_TCPf, memfld);

          SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));

          mem = pipe ? MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE1m : MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE0m;
          sal_memset(entry, 0, sizeof(entry));

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x1;
          soc_mem_field_set(unit, mem, entry, WRED_ENf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x1;
          soc_mem_field_set(unit, mem, entry, ECN_MARKINGf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, MARK_REDf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, MARK_YELLOWf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, MARK_GREENf, memfld);

          SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));

/*
          mem = pipe ? MMU_WRED_CONFIG_PIPE1m : MMU_WRED_CONFIG_PIPE0m;
          sal_memset(entry, 0, sizeof(entry));

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, WRED_ENf, memfld);

          sal_memset(memfld, 0, sizeof(memfld));
          memfld[0] = 0x0;
          soc_mem_field_set(unit, mem, entry, ECN_MARKING_ENf, memfld);

          SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
*/
        }
      }
    }
    return SOC_E_NONE;
}

int enable_refresh(int unit) {
    uint64 rval, val;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, IDB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, IDB_SER_SCAN_CONFIGr, &rval, ENABLEf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, IDB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, rval));

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, AUX_ARB_CONTROLr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, AUX_ARB_CONTROLr, &rval, FP_REFRESH_ENABLEf, val);
    COMPILER_64_SET(val, 0, 0);
    soc_reg64_field_set(unit, AUX_ARB_CONTROLr, &rval, FP_REFRESH_MODEf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, AUX_ARB_CONTROLr, REG_PORT_ANY, 0, rval));

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, EFP_METER_CONTROLr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, EFP_METER_CONTROLr, &rval, EFP_REFRESH_ENABLEf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, EFP_METER_CONTROLr, REG_PORT_ANY, 0, rval));

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, PKTAGINGTIMERr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, PKTAGINGTIMERr, &rval, DURATIONSELECTf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, PKTAGINGTIMERr, REG_PORT_ANY, 0, rval));

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, MMU_GCFG_MISCCONFIGr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, REFRESH_ENf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, MMU_GCFG_MISCCONFIGr, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

int enable_uft(int unit) {
    int index;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];

    index = 0;

    mem = L2_ENTRY_HASH_CONTROLm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3;
    soc_mem_field_set(unit, mem, entry, HASH_TABLE_BANK_CONFIGf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));


    mem = L3_ENTRY_HASH_CONTROLm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xffc;
    soc_mem_field_set(unit, mem, entry, HASH_TABLE_BANK_CONFIGf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 2;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 3;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 4;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 5;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 6;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 7;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 8;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 9;
    soc_mem_field_set(unit, mem, entry, LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));


    mem = UFT_SHARED_BANKS_CONTROLm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xf;
    soc_mem_field_set(unit, mem, entry, BANK_DISABLE_LPf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));

    return SOC_E_NONE;
}

int enable_vfp(int unit) {
    int i, index;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    uint64 rval, val;

    for(i = 0; i < 2; i++) {
        index = (i == 0) ? 1 : 3;
        mem = LPORT_TABm;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, VFP_ENABLEf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
    }

    COMPILER_64_ZERO(rval); COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_0f, val);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_1f, val);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_2f, val);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_3f, val);

    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_0f, val);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_1f, val);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_2f, val);
    soc_reg64_field_set(unit, VFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_3f, val);

    SOC_IF_ERROR_RETURN(soc_reg_set(unit, VFP_SLICE_CONTROLr, REG_PORT_ANY, 0, rval));

    for (i=0; i< 4*256; i++) {

        mem = VFP_TCAMm;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0;
        soc_mem_field_set(unit, mem, entry, F2f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0xffffffff;
        soc_mem_field_set(unit, mem, entry, F2_MASKf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, entry));

    }
    return SOC_E_NONE;
}

int enable_ifp(int unit) {
    int i, index;
    int slice;
    soc_mem_t mem;
    uint64 rval, val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];

    for(i = 0; i < 2; i++) {
        index = (i == 0) ? 1 : 3;
        mem = LPORT_TABm;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, FILTER_ENABLEf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
    }

    for(slice = 0; slice < 12; slice++) {
        COMPILER_64_ZERO(rval); COMPILER_64_SET(val, 0, 1);
        soc_reg64_field_set(unit, IFP_CONFIGr, &rval, IFP_SLICE_LOOKUP_ENABLEf, val);
        soc_reg64_field_set(unit, IFP_CONFIGr, &rval, IFP_SLICE_ENABLEf, val);
        soc_reg64_field_set(unit, IFP_CONFIGr, &rval, IFP_SLICE_MODEf, val);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, IFP_CONFIGr, REG_PORT_ANY, slice, rval));


        mem = IFP_LOGICAL_TABLE_SELECTm;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x400;
        soc_mem_field_set(unit, mem, entry, PKT_FLOW_ID_1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3fff;
        soc_mem_field_set(unit, mem, entry, PKT_FLOW_ID_1_MASKf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x1;
        soc_mem_field_set(unit, mem, entry, ENABLEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x2;
        soc_mem_field_set(unit, mem, entry, TOS_FN_SELf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, slice * 32, entry));
    }

    for(i = 0; i < 32; i++) {
        COMPILER_64_ZERO(rval); COMPILER_64_SET(val, 0, 0xfff);
        soc_reg64_field_set(unit, IFP_LOGICAL_TABLE_CONFIGr, &rval, LOGICAL_PARTITION_MAPf, val);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, IFP_LOGICAL_TABLE_CONFIGr, REG_PORT_ANY, i, rval));
    }

    COMPILER_64_SET(rval, 0 , 0xffffffff);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, IFP_SLICE_METER_MAP_ENABLEr, REG_PORT_ANY, i, rval));

    for (i=0; i< 12 * 768; i++) {

        mem = IFP_TCAM_WIDEm;

        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0;
        memfld[1] = 0;
        soc_mem_field_set(unit, mem, entry, KEYf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0xffffffff;
        memfld[1] = 0xffffffff;
        soc_mem_field_set(unit, mem, entry, MASKf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, entry));

    }

    return SOC_E_NONE;
}

int enable_efp(int unit) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    uint64 rval, val, i;

    mem = EGR_LPORT_PROFILEm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 2, entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 1;
    soc_mem_field_set(unit, mem, entry, EFP_FILTER_ENABLEf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 2, entry));

    COMPILER_64_ZERO(rval); COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_0f, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_1f, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_2f, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_ENABLE_SLICE_3f, val);

    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_0f, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_1f, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_2f, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, LOOKUP_ENABLE_SLICE_3f, val);

    SOC_IF_ERROR_RETURN(soc_reg_set(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 0, rval));
    for(i = 0; i < 4*0x200;i++) {

        mem = EFP_TCAMm;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 3;
        soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[8] = 0;
        memfld[4] = 0;
        soc_mem_field_set(unit, mem, entry, KEYf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[8] = 0xffff;
        memfld[4] = 0xffff;
        soc_mem_field_set(unit, mem, entry, KEY_MASKf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                    i, entry));
    }
    return SOC_E_NONE;
}

int enable_defip(int unit) {
    soc_mem_t mem = L3_DEFIPm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5],i;
    soc_mem_info_t          *meminfo;

    meminfo         = &SOC_MEM_INFO(unit, mem);
    cli_out("defip table size    = %0d\n", meminfo->index_max);
    for (i=0; i<  8192; i++) {
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x1;
        soc_mem_field_set(unit, mem, entry, VALID0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x1;
        soc_mem_field_set(unit, mem, entry, VALID1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = sal_rand() & 0x3ff;
        soc_mem_field_set(unit, mem, entry, VRF_ID_0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = sal_rand() & 0x3ff;
        soc_mem_field_set(unit, mem, entry, VRF_ID_1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3ff;
        soc_mem_field_set(unit, mem, entry, VRF_ID_MASK0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3ff;
        soc_mem_field_set(unit, mem, entry, VRF_ID_MASK1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, KEY_MODE_MASK0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;

        soc_mem_field_set(unit, mem, entry, KEY_MODE_MASK1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = i;
        soc_mem_field_set(unit, mem, entry, IP_ADDR0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = i;

        soc_mem_field_set(unit, mem, entry, IP_ADDR1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0xffffffff;
        soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0xffffffff;

        soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x2;
        soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, entry));
    }
    return SOC_E_NONE;
}

int enable_l3_tunnel(int unit) {
    soc_mem_t mem = L3_TUNNELm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5],i;
    soc_mem_info_t          *meminfo;

    meminfo         = &SOC_MEM_INFO(unit, mem);

    for (i=0; i< meminfo->index_max; i++) {
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x1;
        soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, KEY_TYPE_MASKf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = sal_rand();

        soc_mem_field_set(unit, mem, entry, DIPf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0xffffffff;
        soc_mem_field_set(unit, mem, entry, DIP_MASKf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, entry));
    }
    return SOC_E_NONE;
}

int enable_vlan_xlate_1(int unit) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5],i;
    soc_mem_info_t          *meminfo;

    mem = VLAN_XLATE_1_DOUBLEm;
    meminfo         = &SOC_MEM_INFO(unit, mem);
    cli_out("Vlan xlate 1 table size    = %0d\n", meminfo->index_max);
    if (meminfo->index_max) memfld[0]=0;
    for (i=0; i< 8192; i++) {
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, BASE_VALID_0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x7;
        soc_mem_field_set(unit, mem, entry, BASE_VALID_1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, KEY_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = i;
        memfld[1] = 0xaaaa;
        soc_mem_field_set(unit, mem, entry, MAC__MAC_ADDRf, memfld);


        SOC_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry));
    }
    return SOC_E_NONE;
}

int ifp__rule_setup(int unit, uint8 tos) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    int index, slice, pair_index;

    mem = IFP_KEY_GEN_PROGRAM_PROFILEm;

    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 29;
    soc_mem_field_set(unit, mem, entry, L1_E16_SEL_0f, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 8;
    soc_mem_field_set(unit, mem, entry, L2_E16_SEL_0f, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 11;
    soc_mem_field_set(unit, mem, entry, L1_E8_SEL_0f, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 24;
    soc_mem_field_set(unit, mem, entry, L3_E4_SEL_2f, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 25;
    soc_mem_field_set(unit, mem, entry, L3_E4_SEL_3f, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));


    for(slice = 0; slice < 12; slice++) {
        index = (slice * 768) + tos;

        mem = IFP_TCAM_WIDEm;

        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x0800;
        memfld[1] = (tos << 8);
        soc_mem_field_set(unit, mem, entry, KEYf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0xffff;
        memfld[1] = (0xff << 8);
        soc_mem_field_set(unit, mem, entry, MASKf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));


        mem = IFP_POLICY_TABLE_WIDEm;

        sal_memset(entry, 0, sizeof(entry));


        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x0;
        soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        pair_index = ((3 * (slice & 0x7)) << 7) | (tos & 0x7f);
        memfld[11] = (0x1 << 13) | (0x1 << 11) | (pair_index >> 1);
        memfld[10] = ((pair_index & 0x1) << 31) | (0x1 << 28) | (tos << 14) | (slice << 7) | (0x1 << 5) | (0x1 << 3) | (0x1 << 1) | (0x1 << 0);
        soc_mem_field_set(unit, mem, entry, POLICY_DATAf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
    }


    mem = IFP_POLICY_ACTION_PROFILEm;

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x1;
    soc_mem_field_set(unit, mem, entry, METER_SETf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x1;
    soc_mem_field_set(unit, mem, entry, TTL_OVERRIDE_SETf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x1;
    soc_mem_field_set(unit, mem, entry, COUNTER_SETf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    return SOC_E_NONE;
}

int setup_l3_bank_sel(int unit) {
    uint64 rval, val;

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0xff);
    soc_reg64_field_set(unit, INITIAL_ING_L3_NEXT_HOP_BANK_SELr, &rval, BANK_SELf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, INITIAL_ING_L3_NEXT_HOP_BANK_SELr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0xff);
    soc_reg64_field_set(unit, ING_L3_NEXT_HOP_BANK_SELr, &rval, BANK_SELf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, ING_L3_NEXT_HOP_BANK_SELr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0xff);
    soc_reg64_field_set(unit, EGR_L3_NEXT_HOP_BANK_SELr, &rval, BANK_SELf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, EGR_L3_NEXT_HOP_BANK_SELr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0xf);
    soc_reg64_field_set(unit, EGR_L3_INTF_BANK_SELr, &rval, BANK_SELf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, EGR_L3_INTF_BANK_SELr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0xf);
    soc_reg64_field_set(unit, ING_L3_ECMP_GROUP_BANK_SELr, &rval, BANK_SELf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, ING_L3_ECMP_GROUP_BANK_SELr, REG_PORT_ANY, 0, rval));

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 0xf);
    soc_reg64_field_set(unit, ING_L3_ECMP_BANK_SELr, &rval, BANK_SELf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

int setup_my_station_tcam_2(int unit, bcm_mac_t mac_da, int my_station_profile_index) {
    const soc_mem_t mem = MY_STATION_TCAM_2m;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nAdded L2 Station: ")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x\n"),
                         mac_da[0], mac_da[1],
                         mac_da[2], mac_da[3],
                         mac_da[4], mac_da[5]));
    /* Setup MY_STATION_TCAM*/
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x3;
    soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[1] = (mac_da[0] << 8) | (mac_da[1] << 0);
    memfld[0] = (mac_da[2] << 24) | (mac_da[3] << 16) | (mac_da[4] << 8) | (mac_da[5] << 0);
    soc_mem_field_set(unit, mem, entry, MAC_ADDRf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[1] = 0xffff;
    memfld[0] = 0xffff00ff;
    soc_mem_field_set(unit, mem, entry, MAC_ADDR_MASKf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x0;
    soc_mem_field_set(unit, mem, entry, MODULE_IDf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xff;
    soc_mem_field_set(unit, mem, entry, MODULE_ID_MASKf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x1800 | my_station_profile_index;
    soc_mem_field_set(unit, mem, entry, DESTINATIONf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    return SOC_E_NONE;
}

int setup_my_station_2_profile(int unit, int my_station_profile_index) {
    const soc_mem_t mem = MY_STATION_PROFILE_2m;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];

    cli_out("setup my station 2 porifle entry prof index: %0d\n", my_station_profile_index);
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x1;
    soc_mem_field_set(unit, mem, entry, IPV4_TERMINATION_ALLOWEDf, memfld);
    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x1;
    soc_mem_field_set(unit, mem, entry, IPV6_TERMINATION_ALLOWEDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, my_station_profile_index, entry));

    return SOC_E_NONE;
}

int fill_l3_entry_table(int unit) {
    int i, j, k;
    soc_mem_t mem = L3_ENTRY_SINGLEm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    power_test_t *power_test_p = power_test_parray[unit];

    if (power_test_p->ipv6_param) {
        for(i = 0; i < 256; i++) {
            for(j = 0; j < 256; j++) {
                mem = L3_ENTRY_DOUBLEm;
                k = 0;
                sal_memset(entry, 0, sizeof(entry));

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x3;
                soc_mem_field_set(unit, mem, entry, BASE_VALID_0f, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x4;
                soc_mem_field_set(unit, mem, entry, BASE_VALID_1f, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x2;
                soc_mem_field_set(unit, mem, entry, KEY_TYPEf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x2;
                soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = (0xc0 << 24) | (i << 16) | (0xff << 8) | j;
                soc_mem_field_set(unit, mem, entry, IPV6UC__IP_ADDR_LWR_64f, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = k;
                soc_mem_field_set(unit, mem, entry, IPV6UC__VRF_IDf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = (0x3 << 16) | 0xfff;
                soc_mem_field_set(unit, mem, entry, IPV6UC__DESTINATIONf, memfld);

                soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
            }
        }
    }
    else {
        for(i = 0; i < 256; i++) {
            for(j = 0; j < 256; j++) {
                k = 0;
                sal_memset(entry, 0, sizeof(entry));

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x1;
                soc_mem_field_set(unit, mem, entry, BASE_VALIDf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x0;
                soc_mem_field_set(unit, mem, entry, KEY_TYPEf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 0x0;
                soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = (0xc0 << 24) | (i << 16) | (0xff << 8) | j;
                soc_mem_field_set(unit, mem, entry, IPV4UC__IP_ADDRf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = k;
                soc_mem_field_set(unit, mem, entry, IPV4UC__VRF_IDf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = (0x3 << 16) | 0xfff;
                soc_mem_field_set(unit, mem, entry, IPV4UC__DESTINATIONf, memfld);

                soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
            }
        }
    }

    return SOC_E_NONE;
}

int add_l3_uc_entry(int unit, bcm_ip_t ip_addr, int vrf_id, int nhi) {
    soc_mem_t mem = L3_ENTRY_SINGLEm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5], ecmp;
    power_test_t *power_test_p = power_test_parray[unit];

    ecmp = nhi;

    if (power_test_p->ipv6_param) {
        cli_out("setup l3 ipv6 uc entry ip: %0x ecmp: %0d\n", ip_addr, ecmp);
        mem = L3_ENTRY_DOUBLEm;
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x3;
        soc_mem_field_set(unit, mem, entry, BASE_VALID_0f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x4;
        soc_mem_field_set(unit, mem, entry, BASE_VALID_1f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x2;
        soc_mem_field_set(unit, mem, entry, KEY_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x2;
        soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0;
        memfld[1] = 0xfe800000;
        soc_mem_field_set(unit, mem, entry, IPV6UC__IP_ADDR_UPR_64f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = ip_addr;
        soc_mem_field_set(unit, mem, entry, IPV6UC__IP_ADDR_LWR_64f, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = vrf_id;
        soc_mem_field_set(unit, mem, entry, IPV6UC__VRF_IDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        if (power_test_p->enable_l3_ecmp_param) memfld[0] = (0x1 << 14) | ecmp;
        else memfld[0] = (0x3 << 16) | (nhi + 0x8000);
        soc_mem_field_set(unit, mem, entry, IPV6UC__DESTINATIONf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry));

    }
    else {
        cli_out("setup l3 uc ipv4 entry ip: %0x ecmp: %0d\n", ip_addr, ecmp);
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x1;
        soc_mem_field_set(unit, mem, entry, BASE_VALIDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x0;
        soc_mem_field_set(unit, mem, entry, KEY_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x0;
        soc_mem_field_set(unit, mem, entry, DATA_TYPEf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = ip_addr;
        soc_mem_field_set(unit, mem, entry, IPV4UC__IP_ADDRf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = vrf_id;
        soc_mem_field_set(unit, mem, entry, IPV4UC__VRF_IDf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        if (power_test_p->enable_l3_ecmp_param) memfld[0] = (0x1 << 14) | ecmp;
        else memfld[0] = (0x3 << 16) | (nhi + 0x8000);
        soc_mem_field_set(unit, mem, entry, IPV4UC__DESTINATIONf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry));
    }

    return SOC_E_NONE;
}

int setup_hier_ecmp(int unit, int port) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    uint32 ecmp_offset, ecmp_grp_offset;
    uint32 ecmp1, ecmp2;
    uint32 ecmp_base_ptr1, ecmp_base_ptr2, ecmp_member_cnt;
    uint32 nhi, i;

    nhi = port + 0x8000; /*Offset to BANK_B*/

    ecmp_offset = 0x800;
    ecmp_grp_offset = 0x4000;
    ecmp_member_cnt = 1;

    ecmp1 = port;
    ecmp_base_ptr1 = port * (ecmp_member_cnt + 1);

    ecmp2 = ecmp_offset + port;
    ecmp_base_ptr2 = ecmp_grp_offset + port * (ecmp_member_cnt + 1);

    mem = L3_ECMP_COUNTm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = ecmp_base_ptr1;
    soc_mem_field_set(unit, mem, entry, BASE_PTRf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = ecmp_member_cnt;
    soc_mem_field_set(unit, mem, entry, COUNTf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, ecmp1, entry));

    mem = L3_ECMPm;
    for(i = 0; i < (ecmp_member_cnt + 1); i++) {
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = (0x1 << 14) | ecmp2;
        soc_mem_field_set(unit, mem, entry, DESTINATIONf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x1;
        soc_mem_field_set(unit, mem, entry, ECMP_FLAGf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, ecmp_base_ptr1 + i, entry));
    }

    mem = L3_ECMP_COUNTm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = ecmp_base_ptr2;
    soc_mem_field_set(unit, mem, entry, BASE_PTRf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = ecmp_member_cnt;
    soc_mem_field_set(unit, mem, entry, COUNTf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, ecmp2, entry));

    mem = L3_ECMPm;
    for(i = 0; i < (ecmp_member_cnt + 1); i++) {
        sal_memset(entry, 0, sizeof(entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = (0x3 << 16) | nhi;
        soc_mem_field_set(unit, mem, entry, DESTINATIONf, memfld);

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x0;
        soc_mem_field_set(unit, mem, entry, ECMP_FLAGf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, ecmp_base_ptr2 + i, entry));
    }

    return SOC_E_NONE;
}

int setup_next_hop(int unit, int nhi, int intf_num, int port, bcm_mac_t mac_addr) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];

    /*Add offset to BANK_B*/
    nhi = nhi + 0x8000;

    cli_out("setup l3 next hop entry if: %0d nhi: %0d\n", intf_num, nhi);
    mem = ING_L3_NEXT_HOPm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0x20000 | port;
    soc_mem_field_set(unit, mem, entry, DESTINATIONf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = intf_num;
    soc_mem_field_set(unit, mem, entry, L3_OIFf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, nhi, entry));

    intf_num = intf_num + 0x2000;

    mem = EGR_L3_NEXT_HOPm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = intf_num;
    soc_mem_field_set(unit, mem, entry, L3__INTF_NUMf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[1] = (mac_addr[0] << 8) | (mac_addr[1] << 0);
    memfld[0] = (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) | (mac_addr[5] << 0);
    soc_mem_field_set(unit, mem, entry, L3__MAC_ADDRESSf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, nhi, entry));

    mem = L3_MTU_VALUESm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 9428;
    soc_mem_field_set(unit, mem, entry, MTU_SIZEf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, intf_num, entry));

    return SOC_E_NONE;
}

int setup_l3_if(int unit, int intf_num, bcm_vlan_t vlan, bcm_mac_t mac_addr) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];

    intf_num = intf_num + 0x2000;

    mem = L3_IIF_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 1;
    soc_mem_field_set(unit, mem, entry, IPV4L3_ENABLEf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 1;
    soc_mem_field_set(unit, mem, entry, IPV6L3_ENABLEf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

    cli_out("setup l3 if entry if: %0d vlan: %0d\n", intf_num, vlan);
    mem = L3_IIFm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, intf_num, entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = vlan;
    soc_mem_field_set(unit, mem, entry, VRFf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 1;
    soc_mem_field_set(unit, mem, entry, VRF_VALIDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, intf_num, entry));

    mem = EGR_L3_INTFm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[1] = (mac_addr[0] << 8) | (mac_addr[1] << 0);
    memfld[0] = (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) | (mac_addr[5] << 0);
    soc_mem_field_set(unit, mem, entry, MAC_ADDRESSf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = vlan;
    soc_mem_field_set(unit, mem, entry, OVIDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, intf_num, entry));

    return SOC_E_NONE;
}

int enable_hier_ecmp(int unit) {
    uint64 rval, val;

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, ECMP_CONFIG_2r, &rval, ECMP_MODEf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, ECMP_CONFIG_2r, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

int enable_ip_switch(int unit) {
    int i, index;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];

    cli_out("set ipv4 enable\n");
    for(i = 0; i < 2; i++) {
        index = (i == 0) ? 1 : 3;
        mem = LPORT_TABm;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, V4L3_ENABLEf, memfld);
        memfld[0] = 1;
        soc_mem_field_set(unit, mem, entry, V6L3_ENABLEf, memfld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
    }

    return SOC_E_NONE;
}

int add_port_to_vlan(int unit, bcm_vlan_t vlan, int port) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    uint32 intf_num;
    pbmp_t pbmp;

    intf_num = vlan + 0x2000;

    cli_out("add port %0d to vlan 0x%0x - 0x%0x\n", port, vlan, intf_num);
    mem = VLAN_TABm;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, vlan, entry));

    soc_mem_pbmp_field_get(unit, mem, &entry, PORT_BITMAPf, &pbmp);
    SOC_PBMP_PORT_ADD(pbmp, port);
    soc_mem_pbmp_field_set(unit, mem, &entry, PORT_BITMAPf, &pbmp);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = intf_num;
    soc_mem_field_set(unit, mem, entry, L3_IIFf, memfld);

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 1;
    soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, vlan, entry));

    mem = EGR_VLANm;
    sal_memset(entry, 0, sizeof(entry));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 1;
    soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, vlan, entry));

    return SOC_E_NONE;
}

int vfp__rule_setup(int unit, uint8 tos) {
    uint64 rval, val;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    int sel, slice, index;
    power_test_t *power_test_p = power_test_parray[unit];

    COMPILER_64_ZERO(rval);
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_0_DOUBLE_WIDE_MODEf, val);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_1_DOUBLE_WIDE_MODEf, val);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_2_DOUBLE_WIDE_MODEf, val);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_3_DOUBLE_WIDE_MODEf, val);
    COMPILER_64_SET(val, 0, 3);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_0_F2f, val);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_1_F2f, val);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_2_F2f, val);
    soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &rval, SLICE_3_F2f, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, VFP_KEY_CONTROL_1r, REG_PORT_ANY, 0, rval));

    if (power_test_p->vfp_match_param) {
        for(slice = 0; slice < 4; slice++) {
            index = (slice * 256) + (tos & 0x7f);

            mem = VFP_TCAMm;
            sal_memset(entry, 0, sizeof(entry));

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 1;
            soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 0x0800 << 16;
            soc_mem_field_set(unit, mem, entry, F2f, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 0xffff << 16;
            soc_mem_field_set(unit, mem, entry, F2_MASKf, memfld);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));


            sal_memset(entry, 0, sizeof(entry));

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 1;
            soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = tos << 16;
            soc_mem_field_set(unit, mem, entry, F2f, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 0xff << 16;
            soc_mem_field_set(unit, mem, entry, F2_MASKf, memfld);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, (128 + index), entry));


            mem = VFP_POLICY_TABLEm;
            sal_memset(entry, 0, sizeof(entry));

            sel = (tos == 0x55) ? slice : (slice + 2) % 4;
            switch(sel) {
            case 0 :
                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 1;
                soc_mem_field_set(unit, mem, entry, CHANGE_INT_PRIORITYf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = tos & 0xf;
                soc_mem_field_set(unit, mem, entry, NEW_INT_PRIORITYf, memfld);
                break;

            case 1 :
                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 1;
                soc_mem_field_set(unit, mem, entry, USE_VFP_CLASS_ID_Hf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 7;
                soc_mem_field_set(unit, mem, entry, VFP_CLASS_ID_Hf, memfld);
                break;

            case 2 :
                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 1;
                soc_mem_field_set(unit, mem, entry, USE_VFP_CLASS_ID_Lf, memfld);

                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 7;
                soc_mem_field_set(unit, mem, entry, VFP_CLASS_ID_Lf, memfld);
                break;

            case 3 :
                sal_memset(memfld, 0, sizeof(memfld));
                memfld[0] = 1;
                soc_mem_field_set(unit, mem, entry, DO_NOT_LEARNf, memfld);
                break;

            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
        }
    }

    return SOC_E_NONE;
}

int efp__rule_setup(int unit, uint8 tos) {
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[16];
    uint64 rval, val;
    int index, slice;
    power_test_t *power_test_p = power_test_parray[unit];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 0, &rval));
    COMPILER_64_SET(val, 0, 1);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_0_MODEf, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_1_MODEf, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_2_MODEf, val);
    soc_reg64_field_set(unit, EFP_SLICE_CONTROLr, &rval, SLICE_3_MODEf, val);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 0, rval));

    if (power_test_p->efp_match_param) {
        for(slice = 0; slice < 4; slice++) {
            index = (slice * 0x200) + tos;

            mem = EFP_TCAMm;
            sal_memset(entry, 0, sizeof(entry));

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 3;
            soc_mem_field_set(unit, mem, entry, VALIDf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[8] = 0x1 << 12;
            memfld[4] = tos << 1;
            soc_mem_field_set(unit, mem, entry, KEYf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[8] = 0xf << 12;
            memfld[4] = 0xff << 1;
            soc_mem_field_set(unit, mem, entry, KEY_MASKf, memfld);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                    index, entry));

            mem = EFP_POLICY_TABLEm;
            sal_memset(entry, 0, sizeof(entry));

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 1;
            soc_mem_field_set(unit, mem, entry, METER_PAIR_MODEf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 1;
            soc_mem_field_set(unit, mem, entry, METER_UPDATE_ODDf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 1;
            soc_mem_field_set(unit, mem, entry, METER_UPDATE_EVENf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 2 + slice * 2 + 1;
            soc_mem_field_set(unit, mem, entry, METER_INDEX_ODDf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 2 + slice * 2;
            soc_mem_field_set(unit, mem, entry, METER_INDEX_EVENf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = 1;
            soc_mem_field_set(unit, mem, entry, FLEX_COUNTER_DSCP_SELf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = tos;
            soc_mem_field_set(unit, mem, entry, FLEX_CTR_BASE_COUNTER_IDXf, memfld);

            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = slice;
            soc_mem_field_set(unit, mem, entry, FLEX_CTR_POOL_NUMBERf, memfld);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                    index, entry));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      power_test_set_up_streams
 * Purpose:
 *      VLAN programming for l3uc. Each port is put on an unique VLAN.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
power_test_set_up_streams(int unit)
{
    int i, p, vrf;
    uint8 tos;
    int src_port, dst_port;
    int my_station_profile_index;
    bcm_mac_t nhop_mac = MAC_BASE;
    bcm_mac_t router_mac = RMAC_ADDR;
    bcm_vlan_t vlan = VLAN;
    bcm_ip_t ipv4_da = IPV4_ADDR;
    power_test_t *power_test_p = power_test_parray[unit];

    bcm_vlan_destroy_all(unit);

    BCM_IF_ERROR_RETURN(setup_cos_map(unit));
    BCM_IF_ERROR_RETURN(enable_tcb(unit));
    BCM_IF_ERROR_RETURN(enable_wred(unit));
    BCM_IF_ERROR_RETURN(enable_mtro(unit));
    BCM_IF_ERROR_RETURN(enable_refresh(unit));
    BCM_IF_ERROR_RETURN(enable_uft(unit));
    if (power_test_p->vfp_enable_param) BCM_IF_ERROR_RETURN(enable_vfp(unit));
    BCM_IF_ERROR_RETURN(enable_ifp(unit));
    if (power_test_p->efp_enable_param) BCM_IF_ERROR_RETURN(enable_efp(unit));
    if (power_test_p->enable_l3_ecmp_param) BCM_IF_ERROR_RETURN(enable_hier_ecmp(unit));
    if (power_test_p->enable_vlan_xlate_1_param) BCM_IF_ERROR_RETURN(enable_vlan_xlate_1(unit));
    BCM_IF_ERROR_RETURN(enable_ip_switch(unit));
    BCM_IF_ERROR_RETURN(enable_flex_ctr(unit));
    BCM_IF_ERROR_RETURN(setup_l3_bank_sel(unit));


    if (power_test_p->enable_defip_param) BCM_IF_ERROR_RETURN(enable_defip(unit));
    if (power_test_p->enable_l3_tunnel_param) BCM_IF_ERROR_RETURN(enable_l3_tunnel(unit));

    for(i = 0; i <2; i++) {
        tos = i ? 0x55 : 0xaa;
        BCM_IF_ERROR_RETURN(vfp__rule_setup(unit, tos));
        BCM_IF_ERROR_RETURN(ifp__rule_setup(unit, tos));
        BCM_IF_ERROR_RETURN(efp__rule_setup(unit, tos));
    }

    my_station_profile_index = 0;
    BCM_IF_ERROR_RETURN(setup_my_station_2_profile(unit, my_station_profile_index));
    BCM_IF_ERROR_RETURN(setup_my_station_tcam_2(unit, router_mac, my_station_profile_index));

    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < power_test_p->max_num_ports_param) {
            if (IS_MANAGEMENT_PORT(unit,p)) {
                continue;
            }
            src_port = p;
            dst_port = power_test_p->dst_port[p];
            nhop_mac[4] = src_port;
            router_mac[4] = dst_port;
            vlan = VLAN + src_port;
            vrf = VLAN + dst_port;
            ipv4_da = power_test_p->dip[src_port];

            BCM_IF_ERROR_RETURN(add_port_to_vlan(unit, vlan, src_port));

            cli_out("port: %0d, vlan: 0x%0x, dip: 0x%8x\n", src_port, vlan, power_test_p->dip[src_port]);
            cli_out("mac_addr : %2x.%2x.%2x.%2x.%2x.%2x \n", nhop_mac[5], nhop_mac[4], nhop_mac[3], nhop_mac[2], nhop_mac[1], nhop_mac[0]);
            cli_out("ip_addr : %2x.%2x.%2x.%2x - %3x \n", (ipv4_da) >> 24 & 0xff, (ipv4_da) >> 16 & 0xff,
                    (ipv4_da) >> 8 & 0xff, (ipv4_da) >> 0 & 0xff, vrf);
            cli_out("vlan: %3x \n", vlan);
            BCM_IF_ERROR_RETURN(add_l3_uc_entry(unit, ipv4_da, vrf, src_port));

            if (power_test_p->enable_l3_ecmp_param) BCM_IF_ERROR_RETURN(setup_hier_ecmp(unit, src_port));
            BCM_IF_ERROR_RETURN(setup_next_hop(unit, src_port, vlan, src_port, nhop_mac));
            BCM_IF_ERROR_RETURN(setup_l3_if(unit, vlan, vlan, router_mac));

        }
    }

    BCM_IF_ERROR_RETURN(fill_l3_entry_table(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      power_test_send_pkts
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
power_test_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_BASE;
    uint8 mac_sa[] = MAC_BASE;
    int port, i = 0;
    uint32 pkt_size = 0,  pkt_count = 0, flood_cnt = 0;
    uint8 src_port;
    uint8 dst_port;
    uint32 use_random_packet_sizes = 0;
    power_test_t *power_test_p = power_test_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    if (power_test_p->pkt_size_param == 1) {
        use_random_packet_sizes = 1;
    }

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if (port < power_test_p->max_num_ports_param) {
            if (IS_MANAGEMENT_PORT(unit,port)) {
                continue;
            }
            i++;
            src_port = port;
            dst_port = power_test_p->dst_port[port];

            if (i % 2 == 0) {
                flood_cnt = 0;
                continue;
            } else {
                flood_cnt = power_test_lossless_flood_cnt(unit, port);
            }

            if (power_test_p->pkt_size_param == 0) {
                pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
            } else {
                pkt_size = power_test_p->pkt_size_param;
            }
            cli_out("%0d -> %0d : pkt_size: %0d, flood_cnt: %0d\n", src_port, dst_port, pkt_size, flood_cnt);

            mac_da[4] = dst_port;
            mac_sa[4] = src_port;
            tx_pkt->port = port;
            tx_pkt->num_pkt = flood_cnt;
            tx_pkt->pkt_seed = power_test_p->pkt_seed + port;
            tx_pkt->pkt_size = pkt_size;
            tx_pkt->rand_pkt_size_en = use_random_packet_sizes;
            tx_pkt->rand_pkt_size = NULL;
            tx_pkt->tx_vlan = VLAN + src_port;
            sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
            if (tx_pkt->rand_pkt_size_en) {
                tx_pkt->rand_pkt_size = power_test_p->rand_pkt_sizes[port];
            }
            tx_pkt->l3_en   = 1;
            tx_pkt->ipv6_en = power_test_p->ipv6_param;
            tx_pkt->ip_da   = IPV4_ADDR | (dst_port << 8);
            tx_pkt->ip_sa   = IPV4_ADDR | (src_port << 8);
            tx_pkt->ttl     = TTL;
            stream_tx_pkt(unit, tx_pkt);

            /* print */
            cli_out("%0d -> %0d : sent %0d packets out.\n", src_port, dst_port, tx_pkt->cnt_pkt);
            pkt_count += tx_pkt->cnt_pkt;
        }
    }
    cli_out("\nTotal packet sent: %d\n\n", pkt_count);
    sal_free(tx_pkt);
}

/*
 * Function:
 *      power_test_chk_port_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */
static bcm_error_t
power_test_chk_port_rate(int unit)
{
    int port,src_port,dst_port,i;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    power_test_t *power_test_p = power_test_parray[unit];

    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    i=0;
    PBMP_ITER(PBMP_E_ALL(unit), port) {
       if (port < power_test_p->max_num_ports_param) {
            if (IS_MANAGEMENT_PORT(unit,port)) {
                continue;
            }
            i++;
            src_port = port;
            dst_port = power_test_p->dst_port[port];

            if (i % 2 == 0) {
            continue;
            }
            rate_calc->src_port[dst_port]=src_port;
       }
    }
    rate_calc->traffic_load = power_test_p->traffic_load_param;
    rate_calc->mode         = 0; /* check act_rate against config_rate */
    rate_calc->pkt_size     = power_test_p->pkt_size_param;
    rate_calc->interval_len = power_test_p->rate_calc_interval_param;
    rate_calc->tolerance_lr = power_test_p->rate_tolerance_lr_param;
    rate_calc->tolerance_os = power_test_p->rate_tolerance_ov_param;
    rate_calc->emulation_param = power_test_p->emulation_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if (port < power_test_p->max_num_ports_param) {
            if (IS_MANAGEMENT_PORT(unit,port)) {
                continue;
            }
            SOC_PBMP_PORT_ADD(rate_calc->pbmp, port);
        }
    }

    rv = stream_chk_port_rate(unit, PBMP_PORT_ALL(unit), rate_calc);

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      power_test_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 *
 */
static bcm_error_t
power_test_chk_pkt_integrity(int unit)
{
    int idx = 0, port, dst_port, src_port, is_even;
    int param_pkt_seed;
    uint32 flood_cnt;
    uint8 mac_da[] = MAC_BASE;
    uint8 mac_sa[] = MAC_BASE;
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    power_test_t *power_test_p = power_test_parray[unit];

    param_pkt_seed = power_test_p->pkt_seed;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_L3UC;
    pkt_intg->ipv6_en = (power_test_p->ipv6_param > 0) ? 1 : 0;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if (port < TEST_MAX_NUM_PORTS) {
            if (IS_MANAGEMENT_PORT(unit,port)) {
                continue;
            }
            is_even = (idx % 2 == 0) ? (1) : (0);
            src_port = port;
            dst_port = power_test_p->dst_port[port];
            /* rx_pbmp */
            if (is_even == 1) {
                SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);
            }
            /* rx_vlan: used to forward pkt to CPU */
            pkt_intg->rx_vlan[port] = VLAN + dst_port;
            /* tx_vlan: used to re-generate ref_pkt */
            pkt_intg->tx_vlan[port] = VLAN + src_port;
            /* port_flood_cnt */
            if (is_even) {
                flood_cnt = power_test_lossless_flood_cnt(unit, port);
            } else {
                flood_cnt = 0;
            }
            pkt_intg->port_flood_cnt[port] = flood_cnt;
            /* port_pkt_seed */
            pkt_intg->port_pkt_seed[port] = param_pkt_seed + port;
            /* mac_da, mac_sa */
            mac_da[4] = dst_port;
            mac_sa[4] = src_port;
            sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
            /* ip_da, ip_sa */
            pkt_intg->ip_da[port] = IPV4_ADDR | (dst_port << 8);
            pkt_intg->ip_sa[port] = IPV4_ADDR | (src_port << 8);

            idx++;
        }
    }

    if (stream_chk_pkt_integrity(unit, pkt_intg) != BCM_E_NONE) {
        rv = BCM_E_FAIL;
    }

    sal_free(pkt_intg);
    return rv;
}


/*
 * Function:
 *      power_test_init
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
power_test_init(int unit, args_t *a, void **pa)
{
    int i, p, src_port;
    power_test_t *power_test_p;

    power_test_p = power_test_parray[unit];
    power_test_p = sal_alloc(sizeof(power_test_t), "power_test");
    sal_memset(power_test_p, 0, sizeof(power_test_t));
    power_test_parray[unit] = power_test_p;

    cli_out("\npower_test_init");
    power_test_parse_test_params(unit, a);
    start_cmic_timesync(unit);

    for (p = 0; p < SOC_MAX_NUM_PORTS; p++) {
        power_test_p->dst_port[p] = 0;
    }

    i = 0;
    src_port = 0;
    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < power_test_p->max_num_ports_param) {
            if (IS_MANAGEMENT_PORT(unit,p)) {
                continue;
            }
            if (i % 2 == 0) {
                src_port = p;
            } else {
                power_test_p->dst_port[src_port] = p;
                power_test_p->dst_port[p] = src_port;

                power_test_p->dip[src_port] = IPV4_ADDR | (p << 8);
                power_test_p->dip[p] = IPV4_ADDR | (p << 8);
            }
            i++;
        }
    }

    power_test_p->test_fail = 0;

    return BCM_E_NONE;
}


/*
 * Function:
 *      power_test
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
power_test(int unit, args_t *a, void *pa)
{
    int rv = BCM_E_NONE;
    power_test_t *power_test_p = power_test_parray[unit];
    int config_n_send, check_n_stop;

    if (power_test_p->bad_input) {
        goto done;
    }

    config_n_send = 1; check_n_stop = 1;
    if(power_test_p->scaling_factor_param == 10) {
        config_n_send = 1;
        check_n_stop = 0;
    }
    if(power_test_p->scaling_factor_param == 20) {
        config_n_send = 0;
        check_n_stop = 1;
    }

    cli_out("\nCalling power_test");

    if(config_n_send == 1) {
        stream_set_lpbk(unit, PBMP_PORT_ALL(unit), power_test_p->loopback_mode_param);
        stream_turn_off_cmic_mmu_bkp(unit);
        stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
        power_test_p->pkt_seed = sal_rand();
        power_test_set_port_property(unit, PBMP_PORT_ALL(unit), power_test_p->pkt_size_param,
                                 power_test_p->max_num_cells_param);
        power_test_set_up_streams(unit);

        power_test_send_pkts(unit);
    }

    if(check_n_stop == 1) {
        /* check counter */
        if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
            power_test_p->test_fail = 1;
        }
        /* check rate */
        if (power_test_chk_port_rate(unit) != BCM_E_NONE) {
            power_test_p->test_fail = 1;
        }
        /* check integrity */
        if(power_test_p->check_packet_integrity_param == 1) {
            if (power_test_chk_pkt_integrity(unit) != BCM_E_NONE) {
                power_test_p->test_fail = 1;
            }
        }
        else if(power_test_p->check_packet_integrity_param == 0) {
        }
        else {
            power_test_chk_pkt_integrity(unit);
            cli_out("\n**** PACKET INTEGRITY CHECK IGNORED *****\n");
        }
    }
done:
    return rv;
}


/*
 * Function:
 *      power_test_cleanup
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
power_test_cleanup(int unit, void *pa)
{
    bcm_error_t rv = BCM_E_NONE;
    power_test_t *power_test_p = power_test_parray[unit];

    cli_out("\nCalling power_test_cleanup");

    if (power_test_p->bad_input == 1) {
        power_test_p->test_fail = 1;
    }
    if (power_test_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }

    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (power_test_p->test_fail == 1) {
        cli_out("\n[POWER L3UC test failed]\n\n");
    } else {
        cli_out("\n[POWER L3UC test passed]\n\n");
    }

    /* free mem */
    /*
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        sal_free(power_test_p->rand_pkt_sizes[i]);
    }
    sal_free(power_test_p->rand_pkt_sizes);
    */
    sal_free(power_test_p);
    return rv;
}

#endif /* BCM_TRIDENT3_SUPPORT */

