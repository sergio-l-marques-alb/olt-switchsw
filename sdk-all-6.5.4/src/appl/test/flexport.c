/*
 * $Id: flexport.c,v 1.0 Broadcom SDK $
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
 * The flexport test checks the flexport and TDM functionalities by streaming
 * L2UC packets on all ports at maximum rate. All ports are configured in
 * MAC or PHY loopback mode and each port is associated with one VLAN. The test
 * calculates the number of packets needed to saturate each port and send the
 * L2UC packets with the port's VLAN tag from the CPU to each port initially.
 * Then the packets DLF and flood to the VLAN which contains one port. Port
 * bridging is enabled to allow the DLF packet to go back to its ingress port.
 * Thus, the packets keep looping back within each port indefinitely. The
 * flexport functionality is checked by flexing one or more port macros to
 * a different mode/speed/encapsulation. The port macros and mode/speed/
 * encapsulation can be specify either by command line or through a series of
 * config.bcm files. Once the traffic reaches steady state, rate and packet
 * integrity are checked on the flex ports. Then, traffic will be stopped on the
 * flex ports before calling the BCM FlexPort API to performance the flexport
 * operation. Traffic on the non-flex ports will not be interrupted. The rate
 * calculation is done by dividing the transmit packet count changes and
 * transmit byte count changes over a programmable interval. The rates are
 * checked against expected rates based on port configuration and
 * oversubscription ratio. Packet integrity check is achieved by disabling the
 * VLAN so the packets go back to the CPU as unknown VLAN. The packets are
 * compared against expected packets to ensure packet integrity.
 *
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swirl. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * RateCalcInt: Interval in seconds over which rate is to be calculated
 * TolLr: Rate tolerance percentage for line rate ports (1% by default).
 * TolOv: Rate tolerance percentage for oversubscribed ports (3% by default).
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable
                (default).
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set
 *              to 0 for random.
 * LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.
 *               (default is MAC loopback).
 * ConfigFile: Config files for each succession flexport operation.
 *             (default is none)
 * Tsc<num>: TSC to be flexed and port mode to flex to, e.g. Tsc5=4x10G.
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
#include <sal/appl/io.h>
#include <appl/diag/parse.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>

#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_library.h"

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define MAX_NUM_CELLS_PARAM_DEFAULT 4

#if defined(BCM_ESW_SUPPORT) && !defined(NO_SAL_APPL)

/* TSC Info */
#define TDMV_NUM_TSC 32
#define NUM_PORT_MODULES 33
#define TDMV_NUM_TSC_LANES 4

/* Encap types */
#define PM_ENCAP__HIGIG2 999
#define PM_ENCAP__ETHRNT 998

enum port_state_e {
    PORT_STATE__DISABLED    = 0x0,
    PORT_STATE__LINERATE    = 0x1,
    PORT_STATE__OVERSUB     = 0x2,
    PORT_STATE__COMBINE     = 0x3,
    PORT_STATE__LINERATE_HG = 0x5,
    PORT_STATE__OVERSUB_HG  = 0x6,
    PORT_STATE__COMBINE_HG  = 0x7,
    PORT_STATE__MANAGEMENT  = 0x9,
    PORT_STATE__MUTABLE     = 0x11,
    PORT_STATE__CARDINAL    = 0x19
};

/* Speed set (Gbps) */
typedef enum {
    SPEED_UI_0        = 0,
    SPEED_UI_1G       = 1,
    SPEED_UI_2p5G     = 2,
    SPEED_UI_10G      = 10,
    SPEED_UI_20G      = 20,
    SPEED_UI_21G_DUAL = 21,
    SPEED_UI_25G      = 25,
    SPEED_UI_40G      = 40,
    SPEED_UI_41G_DUAL = 41,
    SPEED_UI_42G      = 42,
    SPEED_UI_50G      = 50,
    SPEED_UI_100G     = 100,
    SPEED_UI_120G     = 120
} port_speed_t;

typedef struct flexport_s {
    pbmp_t vlan_pbmp;
    pbmp_t port_down_pbmp;
    pbmp_t port_up_pbmp;
    pbmp_t speed_change_pbmp;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 loopback_mode_param;
    uint32 flex_by_command_line;
    char *config_file_name;
    char *tsc_param[TDMV_NUM_TSC];
    uint32 num_del;
    uint32 num_add;
    bcm_port_resource_t del_resource[SOC_MAX_NUM_PORTS];
    bcm_port_resource_t add_resource[SOC_MAX_NUM_PORTS];
    uint32 last_assigned_port;
    int linkscan_enable;
    uint32 counter_flags;
    int counter_interval;
    pbmp_t counter_pbm;
    uint32 bad_input;
    uint32 test_fail;
    uint32 pkt_seed;
    int sockfd;
} flexport_t;

static flexport_t *flexport_parray[SOC_MAX_NUM_DEVICES];


static rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES];

char flexport_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "\nFlexPort test usage:\n"
    " \n"
    "PktSize:      Packet size in bytes. Set to 1 for random packet sizes.\n"
    "              Set to 0 (default) for worst case packet sizes on all ports\n"
    "              (145B for ENET, 76B for HG2).\n"
    "FloodCnt:     Number of packets in each swirl between.\n"
    "              Set to 0 (default) for a lossless swirl at full rate.\n"
    "RateCalcInt:  Interval in seconds over which rate is to be calculated.\n"
    "TolLr:        Rate tolerance percentage for line rate ports.\n"
    "              (1% by default)\n"
    "TolOv:        Rate tolerance percentage for oversubscribed ports.\n"
    "              (3% by default).\n"
    "ChkPktInteg:  Set to 0 to disable packet integrity checks.\n"
    "              Set to 1 to enable (default).\n"
    "MaxNumCells:  Maximum number of cells for random packet sizes.\n"
    "              Set to 0 for random cell sizes. (default is 4)\n"
    "LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.\n"
    "              (default is MAC loopback)\n"
    "ConfigFile:   Config files for each succession flexport operation.\n"
    "              (default is none)\n"
    "Tsc<num>:     TSC to be flexed and port mode to flex to, e.g. Tsc5=4x10G\n"
    "              The supported mode/speed in Ethernet encapsulation are:\n"
    ;
#endif

char td2p_supported_mode[] =
    "              1x100G, 1x120G, 10x10G, 12x10G,\n"
    "              1x40G, 1x20G, 1x10G, 2x20G, 2x10G,\n"
    "              20G+2x10G, 2x10G+20G,\n"
    "              4x10G, 4x2.5G, and 4x1G\n"
    "              The supported mode/speed in HiGig2 encapsulation are:\n"
    "              1x106G, 1x127G, 10x11G, 12x11G,\n"
    "              1x42G, 1x21G, 1x11G, 2x21G, and 4x11G\n"
    ;


/*
 * Function:
 *      flexport_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      flexport_p->bad_input set from here - tells test to crash out in case 
 *      CLI input combination is invalid.
 */

static void
flexport_parse_test_params(int unit, args_t *a)
{
    int i;
    parse_table_t parse_table;
    flexport_t *flexport_p = flexport_parray[unit];

    flexport_p->bad_input = 0;

    flexport_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    flexport_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    flexport_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    flexport_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    flexport_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    flexport_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    flexport_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    flexport_p->loopback_mode_param = BCM_PORT_LOOPBACK_MAC;
    flexport_p->flex_by_command_line = 0;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &flexport_p->pkt_size_param, NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &flexport_p->flood_pkt_cnt_param, NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rate_calc_interval_param, NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rate_tolerance_lr_param, NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rate_tolerance_ov_param, NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &flexport_p->check_packet_integrity_param, NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &flexport_p->max_num_cells_param, NULL);
    parse_table_add(&parse_table, "LoopbackMode", PQ_INT|PQ_DFL, 0,
                    &flexport_p->loopback_mode_param, NULL);
    parse_table_add(&parse_table, "ConfigFile", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->config_file_name, NULL);
    parse_table_add(&parse_table, "Tsc0", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[0], NULL);
    parse_table_add(&parse_table, "Tsc1", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[1], NULL);
    parse_table_add(&parse_table, "Tsc2", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[2], NULL);
    parse_table_add(&parse_table, "Tsc3", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[3], NULL);
    parse_table_add(&parse_table, "Tsc4", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[4], NULL);
    parse_table_add(&parse_table, "Tsc5", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[5], NULL);
    parse_table_add(&parse_table, "Tsc6", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[6], NULL);
    parse_table_add(&parse_table, "Tsc7", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[7], NULL);
    parse_table_add(&parse_table, "Tsc8", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[8], NULL);
    parse_table_add(&parse_table, "Tsc9", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[9], NULL);
    parse_table_add(&parse_table, "Tsc10", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[10], NULL);
    parse_table_add(&parse_table, "Tsc11", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[11], NULL);
    parse_table_add(&parse_table, "Tsc12", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[12], NULL);
    parse_table_add(&parse_table, "Tsc13", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[13], NULL);
    parse_table_add(&parse_table, "Tsc14", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[14], NULL);
    parse_table_add(&parse_table, "Tsc15", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[15], NULL);
    parse_table_add(&parse_table, "Tsc16", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[16], NULL);
    parse_table_add(&parse_table, "Tsc17", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[17], NULL);
    parse_table_add(&parse_table, "Tsc18", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[18], NULL);
    parse_table_add(&parse_table, "Tsc19", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[19], NULL);
    parse_table_add(&parse_table, "Tsc20", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[20], NULL);
    parse_table_add(&parse_table, "Tsc21", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[21], NULL);
    parse_table_add(&parse_table, "Tsc22", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[22], NULL);
    parse_table_add(&parse_table, "Tsc23", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[23], NULL);
    parse_table_add(&parse_table, "Tsc24", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[24], NULL);
    parse_table_add(&parse_table, "Tsc25", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[25], NULL);
    parse_table_add(&parse_table, "Tsc26", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[26], NULL);
    parse_table_add(&parse_table, "Tsc27", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[27], NULL);
    parse_table_add(&parse_table, "Tsc28", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[28], NULL);
    parse_table_add(&parse_table, "Tsc29", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[29], NULL);
    parse_table_add(&parse_table, "Tsc30", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[30], NULL);
    parse_table_add(&parse_table, "Tsc31", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[31], NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        if (SOC_IS_TRIDENT2X(unit)) {
            cli_out("%s%s", flexport_test_usage, td2p_supported_mode);
        }
        test_error(unit, "\n*ERROR PARSING ARGS\n");
    }

    cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
    cli_out("\nPktSize      = %0d", flexport_p->pkt_size_param);
    cli_out("\nRateCalcInt  = %0d", flexport_p->rate_calc_interval_param);
    cli_out("\nFloodCnt     = %0d", flexport_p->flood_pkt_cnt_param);
    cli_out("\nTolLr        = %0d", flexport_p->rate_tolerance_lr_param);
    cli_out("\nTolOv        = %0d", flexport_p->rate_tolerance_ov_param);
    cli_out("\nChkPktInteg  = %0d", flexport_p->check_packet_integrity_param);
    cli_out("\nMaxNumCells  = %0d", flexport_p->max_num_cells_param);
    cli_out("\nLoopbackMode = %0d", flexport_p->loopback_mode_param);
    cli_out("\nConfigFile   = %s",  flexport_p->config_file_name);
    for (i = 0; i < TDMV_NUM_TSC; i++) {
       if (flexport_p->tsc_param[i] != NULL) {
           cli_out("\nTsc%2d       = %s", i, flexport_p->tsc_param[i]);
           flexport_p->flex_by_command_line = 1;
       }
    }
    if (flexport_p->flex_by_command_line == 0) {
        cli_out("\nTsc<num>    = (null)");
    }
    cli_out("\n -----------------------------------------------------");

    if (flexport_p->max_num_cells_param == 0) {
        flexport_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (flexport_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet and "
             "76B for HG2");
    } else if (flexport_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (flexport_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        flexport_p->bad_input = 1;
    } else if (flexport_p->pkt_size_param > MTU) {
        test_error(unit,"*ERROR: Packet size higher than %0dB (Device MTU)\n",
                MTU);
        flexport_p->bad_input = 1;
    }

    if (flexport_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, automatically calculate number of packets to "
                "flood each port");
    }

    if ((flexport_p->loopback_mode_param != BCM_PORT_LOOPBACK_MAC) &&
        (flexport_p->loopback_mode_param != BCM_PORT_LOOPBACK_PHY)) {
        test_error(unit,"*ERROR: Loopback mode must be either 1 or 2\n");
        flexport_p->bad_input = 1;
    }

    if ((flexport_p->config_file_name != NULL &&
         flexport_p->flex_by_command_line != 0) ||
        (flexport_p->config_file_name == NULL &&
         flexport_p->flex_by_command_line == 0)) {
        test_error(unit,"*ERROR: Please specify ConfigFile or Tsc\n");
        flexport_p->bad_input = 1;
    }
}

#if defined(BCM_56860_A0) && !defined(NO_FILEIO)
/*
 * Function:
 *      tdm_chomp
 * Purpose:
 *      Removes newline from portmap strings
 * Parameters:
 *      str: Pointer to a string
 *
 * Returns:
 *     Nothing
 */

static
void tdm_chomp(char *str)
{
    while (*str && *str != '\n' && *str != '\r') {
        str++;
    }
    *str = 0;
}


/*
 * Function:
 *      convert_hex2bin
 * Purpose:
 *      Converts hex string to 4-bit binary map
 * Parameters:
 *      str: single hex digit string
 *      bmp: 4 bit bitmap
 *
 * Returns:
 *     Nothing
 */

static
void convert_hex2bin(char str, unsigned char bmp[4]){
    char str_tmp[2];
    long num;
    str_tmp[0]=str; str_tmp[1]='\0';
    num = strtol(str_tmp, NULL, 16);
    
    switch(num){
        case 0:
                bmp[3]=0; bmp[2]=0; bmp[1]=0; bmp[0]=0;
                break;
        case 1:
                bmp[3]=0; bmp[2]=0; bmp[1]=0; bmp[0]=1;
                break;                                  
        case 2:
                bmp[3]=0; bmp[2]=0; bmp[1]=1; bmp[0]=0;
                break;                                  
        case 3:
                bmp[3]=0; bmp[2]=0; bmp[1]=1; bmp[0]=1;
                break;                          
        case 4:
                bmp[3]=0; bmp[2]=1; bmp[1]=0; bmp[0]=0;
                break;                          
        case 5:
                bmp[3]=0; bmp[2]=1; bmp[1]=0; bmp[0]=1;
                break;                          
        case 6:
                bmp[3]=0; bmp[2]=1; bmp[1]=1; bmp[0]=0;
                break;                          
        case 7:
                bmp[3]=0; bmp[2]=1; bmp[1]=1; bmp[0]=1;
                break;                          
        case 8:
                bmp[3]=1; bmp[2]=0; bmp[1]=0; bmp[0]=0;
                break;                          
        case 9:
                bmp[3]=1; bmp[2]=0; bmp[1]=0; bmp[0]=1;
                break;                          
        case 0xa:
                bmp[3]=1; bmp[2]=0; bmp[1]=1; bmp[0]=0;
                break;                          
        case 0xb:
                bmp[3]=1; bmp[2]=0; bmp[1]=1; bmp[0]=1;
                break;                          
        case 0xc:
                bmp[3]=1; bmp[2]=1; bmp[1]=0; bmp[0]=0;
                break;                          
        case 0xd:
                bmp[3]=1; bmp[2]=1; bmp[1]=0; bmp[0]=1;
                break;                          
        case 0xe:
                bmp[3]=1; bmp[2]=1; bmp[1]=1; bmp[0]=0;
                break;                  
        case 0xf:
                bmp[3]=1; bmp[2]=1; bmp[1]=1; bmp[0]=1;
                break;
        default: 
                bmp[3]=0; bmp[2]=0; bmp[1]=0; bmp[0]=0;
                break;
    }
}
#endif

/*
 * Function:
 *      get_lanes
 * Purpose:
 *      Converts hex string to 4-bit binary map
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      num_subport: number of subports
 *      speed: pointer to a port speed array
 *      lanes: pointer to a lane array
 *
 * Returns:
 *     Nothing
 */

static void
get_lanes(int unit, int num_subport, int *speed, int *lanes)
{
    int i;
    int total_bw = 0;

    for (i = 0; i < num_subport; i++) {
        total_bw += speed[i];
    }
    for (i = 0; i < num_subport; i++) {
        lanes[i] = num_subport * speed[i] / total_bw;
        if (SOC_IS_TRIDENT2X(unit) && speed[i] == 100000) {
            lanes[i] = 10;
        }
    }
}


/*
 * Function:
 *      flex_tsc
 * Purpose:
 *      Prepare speed_change_pbmp, port_down_pbmp, port_up_pbmp for one TSC
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      num_subport: number of subports
 *      first_phy_port: first physical port number of the TSC
 *      encap: Ethernet or HiGig2
 *
 * Returns:
 *     SOC_E_XXXX
 */

static bcm_error_t
flex_tsc(int unit, int num_subport, int first_phy_port, int *speed, int encap)
{
    soc_info_t *si;
    flexport_t *flexport_p = flexport_parray[unit];
    int i;
    int cur_speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cur_lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cur_encap = BCM_PORT_ENCAP_IEEE;
    int lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int lanes_changed = 0;
    int assigned_lport[12];
    bcm_port_t lport, pport;
    int rv;

    si = &SOC_INFO(unit);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nFlexing port %0d..%0d to"),
             first_phy_port, first_phy_port + num_subport - 1));
    for (i = 0; i < num_subport; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " %0dG"), speed[i] / 1000));
    }
    if (encap == BCM_PORT_ENCAP_IEEE) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " Ethernet")));
    } else if (encap == BCM_PORT_ENCAP_HIGIG2) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " Higig2")));
    }

    lport = si->port_p2l_mapping[first_phy_port];
    if (lport != -1 && IS_HG2_ENABLED_PORT(unit, lport)) {
        cur_encap = BCM_PORT_ENCAP_HIGIG2;
    }

    for (i = 0; i < num_subport; i++) {
        pport = first_phy_port + i;
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            rv = bcm_port_speed_get(unit, lport, &cur_speed[i]);
            if (BCM_FAILURE(rv) && rv != SOC_E_PORT) {
                cli_out("\nbcm_port_speed_get %0d %0d: %s",
                        lport, rv, bcm_errmsg(rv));
                return rv;
            }
            if (IS_HG_PORT(unit, lport)) {
                switch (cur_speed[i]) {
                    case 120000: cur_speed[i] = 127000; break;
                    case 100000: cur_speed[i] = 106000; break;
                    case 50000:  cur_speed[i] = 53000;  break;
                    case 40000:  cur_speed[i] = 42000;  break;
                    case 25000:  cur_speed[i] = 27000;  break;
                    case 20000:  cur_speed[i] = 21000;  break;
                    case 10000:  cur_speed[i] = 11000;  break;
                    default: break;
                }
            }
        }
    }

    get_lanes(unit, num_subport, cur_speed, cur_lanes);
    get_lanes(unit, num_subport, speed, lanes);
    for (i = 0; i < num_subport; i++) {
        if (lanes[i] != cur_lanes[i]) {
            lanes_changed = 1;
            break;
        }
    }

    if (lanes_changed == 0 && encap == cur_encap) {
        for (i = 0; i < num_subport; i++) {
            if (speed[i] != cur_speed[i]) {
                pport = first_phy_port + i;
                lport = si->port_p2l_mapping[pport];
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nChanging port %0d from %0d G to %0d G"),
                                            pport, cur_speed[i] / 1000,
                                            speed[i] / 1000));
                rv = bcm_port_speed_set(unit, lport, speed[i]);
                if (BCM_FAILURE(rv) && rv != SOC_E_PORT) {
                    cli_out("\nbcm_port_speed_set %0d %0d: %s",
                            lport, speed[i], bcm_errmsg(rv));
                    return rv;
                }
                SOC_PBMP_PORT_ADD(flexport_p->speed_change_pbmp, lport);
            }
        }
        return SOC_E_NONE;
    }

    for (i = 0; i < num_subport; i++) {
        pport = first_phy_port + i;
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            if (cur_speed[i] > 0 &&
                (speed[i] != cur_speed[i] || encap != cur_encap)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nRemoving %0d G port %0d"),
                                     cur_speed[i] / 1000, pport));
                bcm_port_resource_t_init(
                    &flexport_p->del_resource[flexport_p->num_del]);
                flexport_p->del_resource[flexport_p->num_del].physical_port = -1;
                flexport_p->del_resource[flexport_p->num_del].port = lport;
                flexport_p->num_del++;
                SOC_PBMP_PORT_ADD(flexport_p->port_down_pbmp, lport);
            }
        }
    }

    for (i = 0; i < num_subport; i++) {
        if (speed[i] > 0 && (speed[i] != cur_speed[i] || encap != cur_encap)) {
            pport = first_phy_port + i;
            assigned_lport[i] = si->port_p2l_mapping[pport];
            if (assigned_lport[i] == -1) {
                for (lport = flexport_p->last_assigned_port + 1;
                     lport < SOC_MAX_NUM_PORTS;
                     lport++) {
                    if (si->port_l2p_mapping[lport] == -1) {
                        assigned_lport[i] = lport;
                        flexport_p->last_assigned_port = lport;
                        LOG_INFO(BSL_LS_APPL_TESTS,
                                 (BSL_META_U(unit, "\nAssigned logical port")));
                        LOG_INFO(BSL_LS_APPL_TESTS,
                                 (BSL_META_U(unit, " %0d to physical port %0d"),
                                             lport, pport));
                        break;
                    }
                }
            }
            bcm_port_resource_t_init(
                &flexport_p->add_resource[flexport_p->num_add]);
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdding %0d G port %0d lane %0d"),
                                 speed[i] / 1000, pport, lanes[i]));
            flexport_p->add_resource[flexport_p->num_add].physical_port = pport;
            flexport_p->add_resource[flexport_p->num_add].port  = assigned_lport[i];
            flexport_p->add_resource[flexport_p->num_add].speed = speed[i];
            flexport_p->add_resource[flexport_p->num_add].lanes = lanes[i];
            flexport_p->add_resource[flexport_p->num_add].encap = encap;
            flexport_p->num_add++;
            SOC_PBMP_PORT_ADD(flexport_p->port_up_pbmp, assigned_lport[i]);
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      flexport_call_bcm_api
 * Purpose:
 *      Call the bcm_port_resource_multi_set API
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */

static bcm_error_t
flexport_call_bcm_api(int unit)
{
    flexport_t *flexport_p = flexport_parray[unit];
    int i;
    bcm_port_t lport;
    uint32 nports;
    bcm_port_resource_t *resource;
    bcm_error_t rv;

    nports = flexport_p->num_del + flexport_p->num_add;
    resource = (bcm_port_resource_t *) sal_alloc(nports *
                                                 sizeof(bcm_port_resource_t),
                                                 "flexport_resource_array");

    for (i = 0; i < flexport_p->num_del; i ++) {
        resource[i] = flexport_p->del_resource[i];
    }
    for (i = 0; i < flexport_p->num_add; i ++) {
        resource[flexport_p->num_del + i] = flexport_p->add_resource[i];
    }

    PBMP_ITER(flexport_p->port_down_pbmp, lport) {
        if (lport < SOC_MAX_NUM_PORTS) {
            rv = bcm_port_enable_set(unit, lport, 0);
            if (BCM_FAILURE(rv)) {
                cli_out("\nbcm_port_disable_set %0d: %s", lport, bcm_errmsg(rv));
                sal_free(resource);
                return rv;
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling bcm_port_resource_multi:")));
    for (i = 0; i < nports; i ++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: physical_port = %0d"),
                             i, resource[i].physical_port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: port  = %0d"),
                             i, resource[i].port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: speed = %0d"),
                             i, resource[i].speed));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: lanes = %0d"),
                             i, resource[i].lanes));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: encap = %0d"),
                             i, resource[i].encap));
    }
    rv = bcm_port_resource_multi_set(unit, nports, resource);
    if (BCM_FAILURE(rv)) {
        cli_out("\nbcm_port_resource_multi_set: %s", bcm_errmsg(rv));
        sal_free(resource);
        return rv;
    }

    PBMP_ITER(flexport_p->port_up_pbmp, lport) {
        if (lport < SOC_MAX_NUM_PORTS) {
            rv = bcm_port_enable_set(unit, lport, 1);
            if (BCM_FAILURE(rv)) {
                cli_out("\nbcm_port_enable_set %0d: %s", lport, bcm_errmsg(rv));
                sal_free(resource);
                return rv;
            }
        }
    }

    sal_free(resource);

    return SOC_E_NONE;
}

#if defined(BCM_56860_A0) && !defined(NO_FILEIO)
/*
 * Function:
 *      flexport_by_config_file
 * Purpose:
 *      Load a BCM config file and flex accordingly
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      fname: filename of the bcm config file
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_by_config_file(int unit, char *fname)
{
    int port_speed;
    int port_speeds[SOC_MAX_NUM_PORTS];
    int port_states[SOC_MAX_NUM_PORTS];
    int traffic[NUM_PORT_MODULES];
    char line[256], line_cpy1[256], line_cpy2[32], line_cpy3[32];
    char *port_str[32], *port_data[32], *l2p_str[16];
    char *split1 = NULL, *split2 = NULL, *split3 = NULL;
    const char *dic_portmap, *dic_pbmp_xe, *dic_pbmp_os, *dic_dev_bw;
    const char *dic_higig;
    int i, j, port_idx, l2p_idx, port_lgcID, port_data_idx, tmp_port, tmp_speed;
    int port_phyID, highest_lgcID = 0, clk_freq, lgc_2_phy_portmap[SOC_MAX_NUM_PORTS];
    unsigned char os_bmp[128], xe_bmp[128], bmp_4bit[4];
    long int hg_bmp=0;
    FILE *fp;
    soc_info_t *si;
    int p;
    char eth_hig_str[3], line_ovsb_str[4];
    int speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int encap;
    int num_subport;
    int rv = BCM_E_NONE;

    si = &SOC_INFO(unit);
    cli_out("\nCurrent bcm config:");
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            bcm_port_speed_get(unit, p, &port_speed);
            if (SOC_PBMP_MEMBER(PBMP_E_ALL(unit), p)) {
                sal_sprintf(eth_hig_str, "ETH");
            } else {
                sal_sprintf(eth_hig_str, "HG2");
            }
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), p)) {
                sal_sprintf(line_ovsb_str, "OVSB");
            } else {
                sal_sprintf(line_ovsb_str, "LINE");
            }
            cli_out("\nDev Port %3d, Phy Port %3d, %3d G %s %s",
                    p, si->port_l2p_mapping[p], port_speed / 1000,
                    eth_hig_str, line_ovsb_str);
        }
    }

    /* Open bcm config file */
    fp = sal_fopen(fname, "r");
    if (fp == NULL) {
        test_error(unit, "*ERROR: Cannot open bcm config file: '%s'\n", fname);
        return BCM_E_FAIL;
    } else {
        cli_out("\nLoading bcm config file: '%s'", fname);
    }

    /* Dictionary */
    dic_portmap = "portmap_";
    dic_pbmp_xe = "pbmp_xport_xe";
    dic_pbmp_os = "pbmp_oversubscribe";
    dic_dev_bw  = "device_bandwidth";
    dic_higig   = "bmp_higig";

    /* Initialize parameters */
    for (i=0; i < 130; i++) { port_states[i] = 0; }
    for (i=1; i < 130; i++) { port_speeds[i] = SPEED_UI_0; }
    port_speeds[0] = SPEED_UI_10G; port_speeds[129] = SPEED_UI_1G;
    for (i=0; i < (NUM_PORT_MODULES); i++) { traffic[i] = PM_ENCAP__ETHRNT; }
    for (i=0; i < SOC_MAX_NUM_PORTS; i++) { lgc_2_phy_portmap[i] = 9999; }
    for (i=0; i < 128; i++) { os_bmp[i] = 0; xe_bmp[i] = 0; }

    while (sal_fgets(line, sizeof(line), fp)) {
        if (sal_strstr(line, "#") != line) {
            if (sal_strlen(line) > 256) {
                sal_fclose(fp);
                return BCM_E_FAIL;
            }
            sal_strcpy(line_cpy1, line);
            split1 = sal_strtok(line_cpy1,"=");
            port_idx = 0;
            while (split1) {
                port_str[port_idx++] = split1;
                split1 = sal_strtok(NULL,"=");
            }
            tdm_chomp(port_str[1]);
            if (sal_strstr(line, dic_portmap)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    return BCM_E_FAIL;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2,":");
                port_data_idx = 0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL,":");
                }
                tmp_port = sal_atoi(port_data[0]);
                tmp_speed = sal_atoi(port_data[1]);
                if (tmp_port<129 && tmp_port>0) {
                    port_speeds[tmp_port] = tmp_speed;
                    port_states[tmp_port-1] = 9;
                }
                if (sal_strlen(port_str[0]) > 32) {
                    sal_fclose(fp);
                    return BCM_E_FAIL;
                }
                sal_strcpy(line_cpy3, port_str[0]);
                split3 = sal_strtok(line_cpy3,"_");
                l2p_idx = 0;
                while (split3) {
                    l2p_str[l2p_idx++] = split3;
                    split3 = sal_strtok(NULL,"_");
                }
                port_lgcID = sal_atoi(l2p_str[1]);
                if (port_lgcID > 0 && port_lgcID < SOC_MAX_NUM_PORTS) {
                    lgc_2_phy_portmap[port_lgcID] = tmp_port;
                    highest_lgcID = (port_lgcID>highest_lgcID) ?
                                    (port_lgcID) : (highest_lgcID);
                }
            }
            if (sal_strstr(line, dic_dev_bw)) {
                clk_freq = sal_atoi(port_str[1]);
                clk_freq /= 1000;
                /* Check clock frequency */
                if (clk_freq != si->frequency) {
                    test_error(unit, "*ERROR: Cannot change freq from %0d to %0d\n",
                                      si->frequency, clk_freq);
                }
            }
            if (sal_strstr(line, dic_pbmp_xe)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    return BCM_E_FAIL;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2,"x");
                port_data_idx=0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL,"x");
                }
                /* Potentially 128 bit, so cannot just strtol entire string */
                j = 0;
                for (i = (strlen(port_data[1])-1); i >= 0; i--) {
                    if ((j * 4 + 3) < 128) {
                        convert_hex2bin(port_data[1][i], bmp_4bit);
                        xe_bmp[j * 4    ] = bmp_4bit[0];
                        xe_bmp[j * 4 + 1] = bmp_4bit[1];
                        xe_bmp[j * 4 + 2] = bmp_4bit[2];
                        xe_bmp[j * 4 + 3] = bmp_4bit[3];
                        j++;
                    }
                }
            }
            if (sal_strstr(line, dic_pbmp_os)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    return BCM_E_FAIL;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2, "x");
                port_data_idx = 0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL, "x");
                }
                /* Potentially 128 bit, so cannot just strtol entire string */
                j = 0;
                for (i = (strlen(port_data[1]) - 1); i >= 0; i--) {
                    if ((j * 4 + 3) < 128) {
                        convert_hex2bin(port_data[1][i], bmp_4bit);
                        os_bmp[j * 4    ] = bmp_4bit[0];
                        os_bmp[j * 4 + 1] = bmp_4bit[1];
                        os_bmp[j * 4 + 2] = bmp_4bit[2];
                        os_bmp[j * 4 + 3] = bmp_4bit[3];
                        j++;
                    }
                }
            }
            if (sal_strstr(line, dic_higig)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    return BCM_E_FAIL;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2, "x");
                port_data_idx = 0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL, "x");
                }
                hg_bmp = strtol(port_data[1], NULL, 16);
            }
        }
    }

    /* Set port state: oversub, linerate, combined */
    for (i = 1; i < 128; i++) {
        port_phyID = lgc_2_phy_portmap[i];
        if (port_phyID > 0 && port_phyID < 128) {
            if (port_states[port_phyID - 1] == 9 && xe_bmp[i] == 1) {
                /* linerate/oversub */
                port_states[port_phyID - 1] = (os_bmp[i] == 1) ?
                    (PORT_STATE__OVERSUB) : (PORT_STATE__LINERATE);
            }
        }
    }

    /* Set tsc type: higig, ethernet */
    if (hg_bmp > 0) {
        for (i = 0; i < TDMV_NUM_TSC; i++) {
            if ((((hg_bmp >> i) & 0x1) == 0x1)) {
                traffic[i] = PM_ENCAP__HIGIG2;
            }
        }
    }

    /* Transfer port state to TDM-5 specification */
    for (i = 1; i < (SOC_MAX_NUM_PORTS - 7); i++) {
        if (traffic[(i - 1) / TDMV_NUM_TSC_LANES] == PM_ENCAP__HIGIG2) {
            switch (port_states[i - 1]) {
            case PORT_STATE__LINERATE:
                port_states[i-1] = PORT_STATE__LINERATE_HG;
                break;
            case PORT_STATE__OVERSUB:
                port_states[i-1] = PORT_STATE__OVERSUB_HG;
                break;
            default:
                break;
            }
        }
    }

    sal_fclose(fp);

    for (i = 1; i < (SOC_MAX_NUM_PORTS - 1); i++) {
        port_phyID = lgc_2_phy_portmap[i];
        if (port_phyID > 0 && port_phyID < 130) {
            if ((port_states[port_phyID-1] == PORT_STATE__LINERATE_HG) ||
                (port_states[port_phyID-1] == PORT_STATE__OVERSUB_HG)) {
                switch (port_speeds[port_phyID]) {
                    case 120: port_speeds[port_phyID] = 127; break;
                    case 100: port_speeds[port_phyID] = 106; break;
                    case 50:  port_speeds[port_phyID] = 53;  break;
                    case 40:  port_speeds[port_phyID] = 42;  break;
                    case 25:  port_speeds[port_phyID] = 27;  break;
                    case 20:  port_speeds[port_phyID] = 21;  break;
                    case 10:  port_speeds[port_phyID] = 11;  break;
                    default: break;
                }
            }
            if ((port_states[port_phyID-1] == PORT_STATE__LINERATE) ||
                (port_states[port_phyID-1] == PORT_STATE__OVERSUB)) {
                sal_sprintf(eth_hig_str, "ETH");
            } else {
                sal_sprintf(eth_hig_str, "HG2");
            }
            if ((port_states[port_phyID-1] == PORT_STATE__OVERSUB_HG) ||
                (port_states[port_phyID-1] == PORT_STATE__OVERSUB)) {
                sal_sprintf(line_ovsb_str, "OVSB");
            } else {
                sal_sprintf(line_ovsb_str, "LINE");
            }
            cli_out("\nDev Port %3d, Phy Port %3d, %3d G %s %s",
                    i, port_phyID, port_speeds[port_phyID],
                    eth_hig_str, line_ovsb_str);
        }
    }

    for (i = 0; i < TDMV_NUM_TSC; i++) {
        port_idx = i * TDMV_NUM_TSC_LANES + 1;
        num_subport = 4;
        if (SOC_IS_TRIDENT2X(unit) && port_speeds[port_idx] >= 100) {
            num_subport = 12;
            i += 2;
        }
        for (j = 0; j < num_subport; j++) {
            speed[j] = port_speeds[port_idx + j] * 1000;
        }
        if (traffic[(port_idx - 1) / TDMV_NUM_TSC_LANES] == PM_ENCAP__HIGIG2) {
            encap = BCM_PORT_ENCAP_HIGIG2;
        } else {
            encap = BCM_PORT_ENCAP_IEEE;
        }
        rv = flex_tsc(unit, num_subport, port_idx, speed, encap);
        if (BCM_FAILURE(rv)) {
            cli_out("\nflex_tsc %0d: %s", port_idx, bcm_errmsg(rv));
            break;
        }
    }

    return rv;
}
#endif

/*
 * Function:
 *      flexport_by_tsc
 * Purpose:
 *      Flex one TSC according to the command line inputs
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tsc: TSC #
 *      mode: target port speed/mode
 *
 * Returns:
 *     BCM_E_XXXX
 */

static bcm_error_t
flexport_by_tsc(int unit, uint32 tsc, char *mode)
{
    int rv = BCM_E_NONE;
    int speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int num_subport = 4;
    int encap_enum = BCM_PORT_ENCAP_IEEE;

    cli_out("\nFlexing TSC %d to %s", tsc, mode);

    if (sal_strstr(mode, "1x100G") || sal_strstr(mode, "1x106G")) {
        speed[0] = 100000;
        if (SOC_IS_TRIDENT2X(unit)) {
            num_subport = 12;
        }
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "1x120G") || sal_strstr(mode, "1x126G"))) {
        speed[0] = 120000;
        num_subport = 12;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "10x10G") || sal_strstr(mode, "10x11G"))) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 10000; speed[3] = 10000;
        speed[4] = 10000; speed[5] = 10000; speed[6] = 10000; speed[7] = 10000;
        speed[8] = 10000; speed[9] = 10000;
        num_subport = 12;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "12x10G") || sal_strstr(mode, "12x11G"))) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 10000; speed[3] = 10000;
        speed[4] = 10000; speed[5] = 10000; speed[6] = 10000; speed[7] = 10000;
        speed[8] = 10000; speed[9] = 10000; speed[10] = 10000; speed[11] = 10000;
        num_subport = 12;
    } else if (sal_strstr(mode, "20G+2x10G")) {
        speed[0] = 20000; speed[2] = 10000; speed[3] = 10000;
    } else if (sal_strstr(mode, "2x10G+20G")) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 20000;
    } else if (sal_strstr(mode, "1x40G") || sal_strstr(mode, "1x42G")) {
        speed[0] = 40000;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "1x20G"))) {
        speed[0] = 20000;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "1x10G") || sal_strstr(mode, "1x11G"))) {
        speed[0] = 10000;
    } else if (sal_strstr(mode, "2x20G") || sal_strstr(mode, "2x21G")) {
        speed[0] = 20000; speed[2] = 20000;
    } else if (sal_strstr(mode, "2x10G")) {
        speed[0] = 10000; speed[2] = 10000;
    } else if (sal_strstr(mode, "4x25G") || sal_strstr(mode, "4x27G")) {
        speed[0] = 25000; speed[1] = 25000; speed[2] = 25000; speed[3] = 25000;
    } else if (sal_strstr(mode, "4x10G") || sal_strstr(mode, "4x11G")) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 10000; speed[3] = 10000;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "4x2.5G"))) {
        speed[0] = 2500; speed[1] = 2500; speed[2] = 2500; speed[3] = 2500;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "4x1G"))) {
        speed[0] = 1000; speed[1] = 1000; speed[2] = 1000; speed[3] = 1000;
    } else {
        test_error(unit, "*ERROR: Mode %s is not supported\n", mode);
    }

    if (sal_strstr(mode, "11G") || sal_strstr(mode, "21G") ||
        sal_strstr(mode, "27G") || sal_strstr(mode, "53G") ||
        sal_strstr(mode, "42G") || sal_strstr(mode, "106G") ||
        sal_strstr(mode, "127G")) {
        encap_enum = BCM_PORT_ENCAP_HIGIG2;
    } else {
        encap_enum = BCM_PORT_ENCAP_IEEE;
    }

    rv = flex_tsc(unit, num_subport, tsc * TDMV_NUM_TSC_LANES + 1, speed,
                  encap_enum);
    if (BCM_FAILURE(rv)) {
        cli_out("\nflex_tsc %0d: %s",
                tsc * TDMV_NUM_TSC_LANES + 1, bcm_errmsg(rv));
    }

    return rv;
}


/*
 * Function:
 *      flexport_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

int
flexport_test_init(int unit, args_t *a, void **pa)
{
    flexport_t *flexport_p;
    rate_calc_t *rate_calc_p;

    flexport_p = flexport_parray[unit];
    flexport_p = sal_alloc(sizeof(flexport_t), "flexport_test");
    sal_memset(flexport_p, 0, sizeof(flexport_t));
    flexport_parray[unit] = flexport_p;

    rate_calc_p = rate_calc_parray[unit];
    rate_calc_p = sal_alloc(sizeof(rate_calc_t), "streaming_library");
    sal_memset(rate_calc_p, 0, sizeof(rate_calc_t));
    rate_calc_parray[unit] = rate_calc_p;

    cli_out("\nCalling flexport_test_init");
    flexport_parse_test_params(unit, a);

    flexport_p->test_fail = 0;

    BCM_IF_ERROR_RETURN(soc_counter_status(unit, &flexport_p->counter_flags,
                                                 &flexport_p->counter_interval,
                                                 &flexport_p->counter_pbm));
    if (flexport_p->counter_interval > 0) {
        cli_out("\nDisabling counter collection");
        soc_counter_stop(unit);
    }
    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit,
                                                &flexport_p->linkscan_enable));
    if (flexport_p->linkscan_enable) {
        cli_out("\nDisabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      flexport_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

int
flexport_test(int unit, args_t *a, void *pa)
{
    int i;
    pbmp_t stop_traffic_pbmp;
    pbmp_t start_traffic_pbmp;
    flexport_t *flexport_p;
    int rv = BCM_E_NONE;

    flexport_p = flexport_parray[unit];

    if (flexport_p->bad_input) {
        goto done;
    }

    cli_out("\nCalling flexport_test");

    flexport_p->pkt_seed = sal_rand();
    soc_set_lpbk(unit, PBMP_PORT_ALL(unit), flexport_p->loopback_mode_param);
    turn_off_cmic_mmu_bkp(unit);
    turn_off_fc(unit, PBMP_PORT_ALL(unit));
    set_port_property_arrays(unit, PBMP_PORT_ALL(unit),
                             flexport_p->pkt_size_param,
                             flexport_p->max_num_cells_param);
    set_up_ports(unit, PBMP_PORT_ALL(unit));
    flexport_p->vlan_pbmp = set_up_streams(unit, PBMP_PORT_ALL(unit),
                                           flexport_p->vlan_pbmp);
    send_pkts(unit, PBMP_PORT_ALL(unit), flexport_p->pkt_size_param,
              flexport_p->flood_pkt_cnt_param, flexport_p->pkt_seed);

    cli_out("\nWait 2s for traffic to stabilize");
    sal_sleep(2);
    start_rate_measurement(unit, PBMP_PORT_ALL(unit));
    cli_out("\nMeasuring Rate over a period of %0ds",
            flexport_p->rate_calc_interval_param);
    sal_sleep(flexport_p->rate_calc_interval_param);

    flexport_p->num_del = 0;
    flexport_p->num_add = 0;
    flexport_p->last_assigned_port = 0;
    SOC_PBMP_CLEAR(flexport_p->port_down_pbmp);
    SOC_PBMP_CLEAR(flexport_p->port_up_pbmp);
    SOC_PBMP_CLEAR(flexport_p->speed_change_pbmp);
    if (flexport_p->flex_by_command_line) {
        for (i = 0; i < TDMV_NUM_TSC; i++) {
            if (flexport_p->tsc_param[i] != NULL) {
                BCM_IF_ERROR_RETURN(flexport_by_tsc(unit, i,
                                                    flexport_p->tsc_param[i]));
            }
        }
    } else if (flexport_p->config_file_name != NULL){
#if defined(BCM_56860_A0) && !defined(NO_FILEIO)
        BCM_IF_ERROR_RETURN(flexport_by_config_file(unit,
                                                    flexport_p->config_file_name));
#else
        cli_out("no filesystem\n");
#endif
    }

    stop_traffic_pbmp = flexport_p->port_down_pbmp;
    BCM_PBMP_OR(stop_traffic_pbmp, flexport_p->speed_change_pbmp);
    if (BCM_PBMP_NOT_NULL(stop_traffic_pbmp)) {
        if (check_mib_counters(unit, stop_traffic_pbmp) != BCM_E_NONE) {
            flexport_p->test_fail = 1;
        }
        if (check_rates(unit, stop_traffic_pbmp,
                       flexport_p->rate_tolerance_lr_param,
                       flexport_p->rate_tolerance_ov_param) != BCM_E_NONE) {
            flexport_p->test_fail = 1;
        }
    }
    if (BCM_PBMP_NOT_NULL(flexport_p->port_down_pbmp)) {
        if (flexport_p->check_packet_integrity_param != 0) {
            rv = check_packet_integrity(unit, flexport_p->port_down_pbmp,
                                        flexport_p->pkt_size_param,
                                        flexport_p->flood_pkt_cnt_param,
                                        flexport_p->pkt_seed);
            if (rv != BCM_E_NONE) {
                flexport_p->test_fail = 1;
            }
        }
    }

    if (flexport_p->num_del + flexport_p->num_add > 0) {
        soc_set_lpbk(unit, flexport_p->port_down_pbmp, BCM_PORT_LOOPBACK_NONE);
        flexport_call_bcm_api(unit);
        soc_set_lpbk(unit, flexport_p->port_up_pbmp, flexport_p->loopback_mode_param);
    }

    start_traffic_pbmp = flexport_p->port_up_pbmp;
    BCM_PBMP_OR(start_traffic_pbmp, flexport_p->speed_change_pbmp);
    turn_off_fc(unit, flexport_p->port_up_pbmp);
    set_port_property_arrays(unit, start_traffic_pbmp,
                             flexport_p->pkt_size_param,
                             flexport_p->max_num_cells_param);
    set_up_ports(unit, flexport_p->port_up_pbmp);
    flexport_p->vlan_pbmp = set_up_streams(unit, start_traffic_pbmp,
                                           flexport_p->vlan_pbmp);
    send_pkts(unit, start_traffic_pbmp, flexport_p->pkt_size_param,
              flexport_p->flood_pkt_cnt_param, flexport_p->pkt_seed);

    cli_out("\nWait 2s for traffic to stabilize");
    sal_sleep(2);

    start_rate_measurement(unit, start_traffic_pbmp);

    cli_out("\nMeasuring Rate over a period of %0ds",
            flexport_p->rate_calc_interval_param);
    sal_sleep(flexport_p->rate_calc_interval_param);

    if (check_mib_counters(unit, PBMP_PORT_ALL(unit)) != BCM_E_NONE) {
        flexport_p->test_fail = 1;
    }
    if (check_rates(unit, PBMP_PORT_ALL(unit),
                   flexport_p->rate_tolerance_lr_param,
                   flexport_p->rate_tolerance_ov_param) != BCM_E_NONE) {
        flexport_p->test_fail = 1;
    }
    if (flexport_p->check_packet_integrity_param != 0) {
        rv = check_packet_integrity(unit, PBMP_PORT_ALL(unit),
                                    flexport_p->pkt_size_param,
                                    flexport_p->flood_pkt_cnt_param,
                                    flexport_p->pkt_seed);
        if (rv != BCM_E_NONE) {
            flexport_p->test_fail = 1;
        }
    }

done:
    return rv;
}


/*
 * Function:
 *      flexport_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

int
flexport_test_cleanup(int unit, void *pa)
{
    flexport_t *flexport_p;
    int rv = BCM_E_NONE;

    cli_out("\nCalling flexport_test_cleanup");

    flexport_p = flexport_parray[unit];

    if (flexport_p->linkscan_enable) {
        cli_out("\nEnabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set_pbm(unit, PBMP_PORT_ALL(unit),
                                                      BCM_LINKSCAN_MODE_SW));
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 1));
    }
    if (flexport_p->counter_interval > 0) {
        cli_out("\nEnabling counter collection");
        BCM_IF_ERROR_RETURN(soc_counter_start(unit, flexport_p->counter_flags,
                                                 flexport_p->counter_interval,
                                                 PBMP_PORT_ALL(unit)));
    }

    if (flexport_p->bad_input == 1) {
        flexport_p->test_fail = 1;
    }

    if (flexport_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }

    sal_free(flexport_p);
    bcm_vlan_init(unit);

    cli_out("\n");

    return rv;
}
#endif /* BCM_ESW_SUPPORT && !NO_SAL_APPL */
