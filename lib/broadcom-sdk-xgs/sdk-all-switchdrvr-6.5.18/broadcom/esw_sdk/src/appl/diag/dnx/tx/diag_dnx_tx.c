/** \file diag_dnx_tx.c
 *
 * Main diagnostics for tx applications
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_TX

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <sal/appl/sal.h>

/** allow drv.h include excplictly for system*/
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h> /** needed */
#include <appl/diag/diag.h>

/* DNX TX Includes */
#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include "diag_dnx_tx.h"
#include <bcm/stack.h>
#include <bcm/trunk.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

/*************
 * TYPEDEFS  *
 *************/

/* Struct for TX/RX parameters */
typedef struct xd_s
{
    int xd_unit;                /* Unit # */
    int xd_tx_unit;             /* TX device */
    int xd_ppsm;                /* true --> per port src macs */
    enum
    {
        XD_IDLE,                /* Not doing anything */
        XD_RUNNING,             /* Running */
        XD_STOP                 /* Stop requested */
    } xd_state;
    uint32 xd_tot_cnt;          /* # to send/receive */
    uint32 xd_cur_cnt;          /* # sent/received */
    int xd_pkt_len;             /* Packet length */
    char *xd_file;              /* File name of packet data */
    sal_mac_addr_t xd_mac_dst;  /* Destination mac address */
    sal_mac_addr_t xd_mac_src;  /* Source mac address */
    sal_mac_addr_t xd_mac_src_base;     /* Source mac address for pps */
    /*
     * port source mac 
     */
    uint32 xd_mac_dst_inc;      /* Destination mac increment */
    uint32 xd_mac_src_inc;      /* Source mac increment */
    uint32 xd_pat;              /* XMIT pattern */
    uint32 xd_pat_inc;          /* Pattern Increment */
    int xd_pat_random;          /* Use Random Pattern */
    pbmp_t xd_ppsm_pbm;         /* Saved port bit map for per */
    /*
     * port source mac 
     */
    uint32 xd_vlan;             /* vlan id (0 if untagged) */
    uint32 xd_prio;             /* vlan prio */
    uint32 xd_prio_int;         /* Internal priority */
    int xd_untag;               /* Send Untagged */

    uint32 xd_crc;

    /*
     * Packet info is now in bcm_pkt_t structure 
     */
    bcm_pkt_t pkt_info;

    char *xd_data;              /* packet data */

    /** Parameters for PTCH_2 headers on JR2 */
    uint16 xd_ptch_src_port;    /* PTCH src port */
    uint16 xd_in_tm_port;       /* Speified in TM port */
    uint32 xd_ptch_header_2;    /* Speified PTCH_2 Header */

    /** Parameters for ITMH headers on JR2 */
    uint32 xd_itmh_present;
    uint32 itmh_fwd_action_strength;
    uint32 itmh_as_ext_present;
    uint32 itmh_pph_type;
    uint32 itmh_in_mirr_disable;
    uint32 itmh_dp;
    uint32 itmh_fwd_type;
    uint32 itmh_fec_ptr;
    uint32 itmh_action_profile_idx;
    uint32 itmh_dest_sys_port;
    uint32 itmh_flow_id;
    uint32 itmh_multicast_id;
    uint32 itmh_snoop_profile;
    uint32 itmh_traffic_class;
    uint32 itmh_base_ext_present;
    uint32 itmh_base_ext_type;
    uint32 itmh_base_ext_value;

    uint32 xd_visibility;
} xd_t;

/*
 * ITMH base header
 */
typedef union diag_dnx_tx_itmh_base_u
{
    struct
    {
        uint8 bytes[5];
    } raw;
    struct
    {
#if defined(LE_HOST)
        unsigned inbound_mirror_disable:1;
        unsigned pph_type:2;
        unsigned injected_as_extension_present:1;
        unsigned forward_action_strength:3;
        unsigned reserved:1;
        unsigned destination_hi:6;
        unsigned drop_precedence:2;
        unsigned destination_mi:8;
        unsigned snoop_profile_hi:1;
        unsigned destination_lo:7;
        unsigned itmh_base_ext_present:1;
        unsigned traffic_class:3;
        unsigned snoop_profile_lo:4;
#else
        unsigned reserved:1;
        unsigned forward_action_strength:3;
        unsigned injected_as_extension_present:1;
        unsigned pph_type:2;
        unsigned inbound_mirror_disable:1;
        unsigned drop_precedence:2;
        unsigned destination_hi:6;
        unsigned destination_mi:8;
        unsigned destination_lo:7;
        unsigned snoop_profile_hi:1;
        unsigned snoop_profile_lo:4;
        unsigned traffic_class:3;
        unsigned itmh_base_ext_present:1;
#endif
    } jr2_common;

    struct
    {
        unsigned _rsvd_0:8;
#if defined(LE_HOST)
        unsigned destination_hi:6;
        unsigned _rsvd_1:2;
        unsigned destination_mi:8;
        unsigned _rsvd_2:1;
        unsigned destination_lo:7;
#else
        unsigned _rsvd_1:2;
        unsigned destination_hi:6;
        unsigned destination_mi:8;
        unsigned destination_lo:7;
        unsigned _rsvd_2:1;
#endif
        unsigned _rsvd_3:8;
    } jr2_dest_info;
} diag_dnx_tx_itmh_base_t;

typedef union diag_dnx_tx_itmh_base_ext_u
{
    struct
    {
        uint8 bytes[3];
    } raw;

    struct
    {
#if defined(LE_HOST)
        unsigned value1_hi:6;
        unsigned type:1;
        unsigned reserved:1;
        unsigned value1_mi:8;
        unsigned value1_lo:8;
#else
        unsigned reserved:1;
        unsigned type:1;
        unsigned value1_hi:6;
        unsigned value1_mi:8;
        unsigned value1_lo:8;
#endif
    } base_ext;
} diag_dnx_tx_itmh_base_ext_t;

typedef struct diag_dnx_tx_itmh_s
{
    /*
     * ITMH base header
     */
    diag_dnx_tx_itmh_base_t base;
    /*
     * ITMH base ext header
     */
    diag_dnx_tx_itmh_base_ext_t base_ext;
} diag_dnx_tx_itmh_t;

/*
 * ITMH base header on JR1 mode
 */
typedef union diag_dnx_tx_itmh_base_jr1_u
{
    struct
    {
        uint8 bytes[4];
    } raw;
    struct
    {
#if defined(LE_HOST)
        unsigned fwd_type_hi:3;
        unsigned dp:2;
        unsigned in_mirr_flag:1;
        unsigned pph_type:2;
        unsigned fwd_type_lo:16;
        unsigned itmh_base_ext:1;
        unsigned traffic_class_lo:2;
        unsigned traffic_class_hi:1;
        unsigned snoop_cmd:4;
#else
        unsigned pph_type:2;
        unsigned in_mirr_flag:1;
        unsigned dp:2;
        unsigned fwd_type_hi:3;
        unsigned fwd_type_lo:16;
        unsigned snoop_cmd:4;
        unsigned traffic_class_hi:1;
        unsigned traffic_class_lo:2;
        unsigned itmh_base_ext:1;
#endif
        unsigned __CONTAINER__:16;      /* 2 & 3 */
    } jer_common;

    struct
    {
#if defined(LE_HOST)
        unsigned dest_info_hi:3;
        unsigned _rsvd_0:5;
        unsigned dest_info_mi:8;
        unsigned dest_info_lo:8;
#else
        unsigned _rsvd_0:5;
        unsigned dest_info_hi:3;
        unsigned dest_info_mi:8;
        unsigned dest_info_lo:8;
#endif
        unsigned _rsvd_1:8;
    } jer_dest_info;
} diag_dnx_tx_itmh_base_jr1_t;

typedef union diag_dnx_tx_itmh_base_ext_jr1_u
{
    struct
    {
        uint8 bytes[3];
    } raw;

    struct
    {
#if defined(LE_HOST)
        unsigned value1_hi:4;
        unsigned reserved:1;
        unsigned type:3;
        unsigned value1_mi:8;
        unsigned value1_lo:8;
#else
        unsigned type:3;
        unsigned reserved:1;
        unsigned value1_hi:4;
        unsigned value1_mi:8;
        unsigned value1_lo:8;
#endif
    } base_ext;
} diag_dnx_tx_itmh_base_ext_jr1_t;

typedef struct diag_dnx_tx_itmh_jr1_s
{
    /*
     * ITMH base header on JR1 mode
     */
    diag_dnx_tx_itmh_base_jr1_t base;
    /*
     * ITMH base ext header on JR1 mode
     */
    diag_dnx_tx_itmh_base_ext_jr1_t base_ext;
} diag_dnx_tx_itmh_jr1_t;

typedef enum
{
    TX_DNX_ITMH_NONE,
    TX_DNX_ITMH_BASE,           /* ITMH base header only */
    TX_DNX_ITMH_BASE_EXT,       /* ITMH base + base extension */
    TX_DNX_ITMH_PMF_EXT,        /* ITMH base + PMF extension */
    TX_DNX_ITMH_BASE_JR1,       /* ITMH base header only on JR1 mode */
    TX_DNX_ITMH_BASE_EXT_JR1,   /* ITMH base + base extension on JR1 mode */
    TX_DNX_ITMH_PMF_EXT_JR1,    /* ITMH base + PMF extension on JR1 mode */
    TX_DNX_ITMH_TYPE_COUNT
} tx_dnx_itmh_type_t;

static xd_t *xd_units[SOC_MAX_NUM_DEVICES];
static void diag_dnx_xd_init(
    int unit);

#define ENCAP_IEEE      0

#define XD_FILE(xd)     ((xd)->xd_file != NULL && (xd)->xd_file[0] != 0)
#define XD_STRING(xd)     ((xd)->xd_data != NULL && (xd)->xd_data[0] != 0)

#define TAGGED_PACKET_LENGTH           68
#define UNTAGGED_PACKET_LENGTH         64

#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
/** Size of module header on DNX devices, it should be written at the start of the packet */
#define DNX_MODULE_HEADER_SIZE         16
#else
/** No Module Header on ADAPTER and non DNX*/
#define DNX_MODULE_HEADER_SIZE         0
#endif
#define TX_DNX_PTCH_TYPE2_LEN                   2
#define TX_DNX_ITMH_BASE_LEN                    5
#define TX_DNX_ITMH_BASE_EXT_LEN                3
#define TX_DNX_ITMH_BASE_LEN_JR1                4
#define TX_DNX_ITMH_BASE_EXT_LEN_JR1            3

/** ITMH Destination Encoding(21)  */
/** FEC pointer(20)*/
#define TX_DNX_ITMH_FWD_TYPE_FEC_POINTER        1
/** System Port Agg(16)*/
#define TX_DNX_ITMH_FWD_TYPE_UNICAST_DIRECT     2
/** SNS(3), FWS(4), Action-Priofile-IDX(9) */
#define TX_DNX_ITMH_FWD_TYPE_ACTION_PROFILE_IDX 3
/** TM-Flow(19) */
#define TX_DNX_ITMH_FWD_TYPE_UNICAST_FLOW       4
/** MC-ID(19) */
#define TX_DNX_ITMH_FWD_TYPE_SYSTEM_MULTICAST   5

#define _XD_INIT(unit, xd) do { \
            diag_dnx_xd_init(unit); \
        if (xd_units[unit] == NULL) \
            return CMD_FAIL; \
        xd = xd_units[unit]; } while (0)

char cmd_dnx_tx_usage[] = "Parameters: <Count> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  Transmit the specified number of packets, if the contents of the\n"
    "  packets is important, they may be specified using options.\n"
    "  Supported options are:\n"
    "      Untagged[yes/no]           - Specify packet should be sent untagged(XGS3)\n"
    "      TXUnit=<val>               - Transmit unit number\n"
    "      PortBitMap=<pbmp>          - Specify port bitmap packet is sent to.\n"
    "      UntagBitMap=<pbmp>         - Specify untag bitmap used for DMA.\n"
    "      File=<filename>            - Load hex packet data from file and ignore\n"
    "                                   various other pattern parameters below.\n"
    "      DATA=<value>               - Packet value (Hex).\n"
    "      Length=<value>             - Specify the total length of the packet,\n"
    "                                   including header, possible tag, and CRC.\n"
    "      VLantag=<value>            - Specify the VLAN tag used, only the low\n"
    "                                   order 16-bits are used (VLANID=0 for none)\n"
    "      VlanPrio                   - VLAN Priority.\n"
    "      PrioInt                    - Internal Priority.\n"
    "      Pattern=<value>            - Specify 32-bit data pattern used.\n"
    "      PatternInc=<value>         - Value by which each word of the data\n"
    "                                   pattern is incremented\n"
    "      PatternRandom=[0|1]        - Use Random data pattern\n"
    "      PerPortSrcMac=[0|1]        - Associate specific (different) src macs\n"
    "                                   with each source port.\n"
    "      SourceMac=<value>          - Source MAC address in packet\n"
    "      SourceMacInc=<val>         - Source MAC increment\n"
    "      DestMac=<value>            - Destination MAC address in packet.\n"
    "      DestMacInc=<value>         - Destination MAC increment.\n"
    "      PtchSRCport=<value>        - PTCH source port (For ARAD only).\n"
    "      PORT=<value>               - Specific source TM_PORT \n"
    "      VISibility=<value>         - Specify visibility resume, default is enabled\n"
    "\n"
    "  Ingress TM Header Format-specific options:\n"
    "      Itmh_Present=[0|1]         - ITMH header is present\n"
    "      Fwd_Act_Strength=<value>   - Forward action strength\n"
    "      As_Ext_Present=[0|1]       - FTMH Application Specific Extension\n"
    "      Pph_Type=<value>           - Packet processing header is present\n"
    "      In_Mirr_Disable=[0|1]      - Disable in-bound port mirroring\n"
    "      Drop_Precedence=<value>    - Drop precedence for WRED\n"
    "      Forward_Type=<value>       - Forwarding destination info type \n"
    "      Fec_Ptr=<value>            - Fec pointer id \n"
    "      Act_Profile_Idx=<value>    - Action profile idx including SNS,FWS \n"
    "      Dest_Sys_Port=<value>      - System level Destination system port \n"
    "      Flow_ID=<value>            - Unicast flow id \n"
    "      Multicast_ID=<value>       - Multicast ID identifying group \n"
    "      Snoop_Profile=<value>      - One of 15 snoop cmds \n"
    "      Traffic_Class=<value>      - System level TM traffic class\n"
    "      Itmh_Base_Ext_Present=[0|1] - ITMH base extention is present\n"
    "      Itmh_Base_Ext_Type=<value>  - ITMH base extention type\n"
    "      Itmh_Base_Ext_Value=<value> - ITMH base extention value\n"
#endif
    ;

/*************
* FUNCTIONS *
*************/

/**
 * \brief - Parse TX command from BCM shell
 */
static cmd_result_t
diag_dnx_tx_parse(
    int u,
    args_t * a,
    xd_t * xd)
{
    char *crc_list[] = { "None", "Recompute", "Append", NULL };
    char *fwd_type[] = { "None", "Fec", "SPA", "Action", "TMFlow", "MCID", NULL };
    parse_table_t pt;
    int min_len, tagged;
    char *xfile, *xdata;
    uint32 port_ndx;
    uint32 ptch_src_port = 0;
    uint32 in_tm_port = 0;
    uint8 has_cpu_port = FALSE;

    /*
     * First arg is count
     */
    if (!ARG_CNT(a) || !isint(ARG_CUR(a)))
    {
        return (CMD_USAGE);
    }

    /*
     * Initialize ptch_src_port
     * 1. set ptch_src_port as firt cpu port
     * 2. return error if there is no cpu port
     */
    BCM_PBMP_ITER(PBMP_CMIC(u), port_ndx)
    {
        /** Initialized as first CPU port */
        ptch_src_port = port_ndx;
        in_tm_port = port_ndx;
        has_cpu_port = TRUE;
        break;
    }
    if (!has_cpu_port)
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("No cpu port\n")));
        return CMD_FAIL;
    }

    _XD_INIT(u, xd);
    xd->xd_tot_cnt = parse_integer(ARG_GET(a));

    parse_table_init(u, &pt);

    parse_table_add(&pt, "Untagged", PQ_DFL | PQ_BOOL, 0, &xd->xd_untag, NULL);
    parse_table_add(&pt, "TXUnit", PQ_DFL | PQ_INT, 0, &xd->xd_tx_unit, NULL);
    parse_table_add(&pt, "PortBitMap", PQ_DFL | PQ_PBMP | PQ_BCM, 0, &xd->pkt_info.tx_pbmp, NULL);
    if (!SOC_IS_XGS_FABRIC(u))
    {
        parse_table_add(&pt, "UntagBitMap", PQ_DFL | PQ_PBMP | PQ_BCM, 0, &xd->pkt_info.tx_upbmp, NULL);
    }

    parse_table_add(&pt, "Filename", PQ_DFL | PQ_STRING, 0, &xd->xd_file, NULL);
    parse_table_add(&pt, "Length", PQ_DFL | PQ_INT, 0, &xd->xd_pkt_len, NULL);
    parse_table_add(&pt, "VLantag", PQ_DFL | PQ_HEX, 0, &xd->xd_vlan, NULL);
    parse_table_add(&pt, "VlanPrio", PQ_DFL | PQ_INT, 0, &xd->xd_prio, NULL);
    parse_table_add(&pt, "PrioInt", PQ_DFL | PQ_INT, 0, &xd->xd_prio_int, NULL);
    parse_table_add(&pt, "Pattern", PQ_DFL | PQ_HEX, 0, &xd->xd_pat, NULL);
    parse_table_add(&pt, "PatternInc", PQ_DFL | PQ_INT, 0, &xd->xd_pat_inc, NULL);
    parse_table_add(&pt, "PatternRandom", PQ_DFL | PQ_BOOL, 0, &xd->xd_pat_random, NULL);
    parse_table_add(&pt, "PerPortSrcMac", PQ_DFL | PQ_INT, 0, &xd->xd_ppsm, NULL);
    parse_table_add(&pt, "SourceMac", PQ_DFL | PQ_MAC, 0, &xd->xd_mac_src, NULL);
    parse_table_add(&pt, "SourceMacInc", PQ_DFL | PQ_INT, 0, &xd->xd_mac_src_inc, NULL);
    parse_table_add(&pt, "DestMac", PQ_DFL | PQ_MAC, 0, &xd->xd_mac_dst, NULL);
    parse_table_add(&pt, "DestMacInc", PQ_DFL | PQ_INT, 0, &xd->xd_mac_dst_inc, NULL);
    parse_table_add(&pt, "CRC", PQ_DFL | PQ_MULTI, 0, &xd->xd_crc, crc_list);
    parse_table_add(&pt, "PtchSRCport", PQ_DFL | PQ_INT, 0, &ptch_src_port, NULL);
    parse_table_add(&pt, "PtchHeader", PQ_DFL | PQ_INT, 0, &xd->xd_ptch_header_2, NULL);
    parse_table_add(&pt, "DATA", PQ_STRING, 0, &xd->xd_data, NULL);
    parse_table_add(&pt, "PORT", PQ_DFL | PQ_INT, 0, &in_tm_port, NULL);
    parse_table_add(&pt, "VISibility ", PQ_BOOL, (void *) TRUE, &xd->xd_visibility, NULL);

    /*
     * Parse ITMH header contents for JR2
     */
    parse_table_add(&pt, "Itmh_Present", PQ_DFL | PQ_BOOL, 0, &xd->xd_itmh_present, NULL);
    parse_table_add(&pt, "Fwd_Act_Strength", PQ_DFL | PQ_INT, 0, &xd->itmh_fwd_action_strength, NULL);
    parse_table_add(&pt, "As_Ext_Present", PQ_DFL | PQ_BOOL, 0, &xd->itmh_as_ext_present, NULL);
    parse_table_add(&pt, "Pph_Type", PQ_DFL | PQ_INT, 0, &xd->itmh_pph_type, NULL);
    parse_table_add(&pt, "In_Mirr_Disable", PQ_DFL | PQ_BOOL, 0, &xd->itmh_in_mirr_disable, NULL);
    parse_table_add(&pt, "Drop_Precedence", PQ_DFL | PQ_INT, 0, &xd->itmh_dp, NULL);
    /*
     * Destination
     */
    parse_table_add(&pt, "Forward_Type", PQ_DFL | PQ_MULTI, 0, &xd->itmh_fwd_type, fwd_type);
    parse_table_add(&pt, "Fec_Ptr", PQ_DFL | PQ_INT, 0, &xd->itmh_fec_ptr, NULL);
    parse_table_add(&pt, "Dest_Sys_Port", PQ_DFL | PQ_INT, 0, &xd->itmh_dest_sys_port, NULL);
    parse_table_add(&pt, "Act_Profile_Idx", PQ_DFL | PQ_INT, 0, &xd->itmh_action_profile_idx, NULL);
    parse_table_add(&pt, "Flow_ID", PQ_DFL | PQ_INT, 0, &xd->itmh_flow_id, NULL);
    parse_table_add(&pt, "Multicast_ID", PQ_DFL | PQ_INT, 0, &xd->itmh_multicast_id, NULL);

    parse_table_add(&pt, "Snoop_Profile", PQ_DFL | PQ_INT, 0, &xd->itmh_snoop_profile, NULL);
    parse_table_add(&pt, "Traffic_Class", PQ_DFL | PQ_INT, 0, &xd->itmh_traffic_class, NULL);
    parse_table_add(&pt, "Itmh_Base_Ext_Present", PQ_DFL | PQ_BOOL, 0, &xd->itmh_base_ext_present, NULL);
    parse_table_add(&pt, "Itmh_Base_Ext_Type", PQ_DFL | PQ_INT, 0, &xd->itmh_base_ext_type, NULL);
    parse_table_add(&pt, "Itmh_Base_Ext_Value", PQ_DFL | PQ_INT, 0, &xd->itmh_base_ext_value, NULL);

    /*
     * Parse remaining arguments
     */
    if (0 > parse_arg_eq(a, &pt))
    {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return (CMD_FAIL);
    }
    if (xd->xd_file)
    {
        xfile = sal_strdup(xd->xd_file);
    }
    else
    {
        xfile = NULL;
    }
    if (xd->xd_data)
    {
        xdata = sal_strdup(xd->xd_data);
    }
    else
    {
        xdata = NULL;
    }

    parse_arg_eq_done(&pt);
    xd->xd_file = xfile;
    xd->xd_data = xdata;
    xd->xd_ptch_src_port = (uint16) ptch_src_port;
    xd->xd_in_tm_port = (uint16) in_tm_port;

    /** Make sure that source TM port is one of the CPU port */
    has_cpu_port = FALSE;
    BCM_PBMP_ITER(PBMP_CMIC(u), port_ndx)
    {
        if (xd->xd_in_tm_port == port_ndx)
        {
            has_cpu_port = TRUE;
            break;
        }
    }
    if (!has_cpu_port)
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("No cpu port\n")));
        return CMD_FAIL;
    }

    tagged = (xd->xd_vlan != 0);
    min_len = (tagged ? TAGGED_PACKET_LENGTH : UNTAGGED_PACKET_LENGTH);

    if (!XD_FILE(xd) && !XD_STRING(xd) && !tagged)
    {
        cli_out("%s: Warning: Sending untagged packets from CPU " "not recommended\n", ARG_CMD(a));
    }

    if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pkt_len < min_len)
    {
        cli_out("%s: Warning: Length %d too small for %s packet (min %d)\n",
                ARG_CMD(a), xd->xd_pkt_len, tagged ? "tagged" : "untagged", min_len);
    }

    if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pkt_len < min_len)
    {
        cli_out("%s: Warning: Length %d too small for %s packet (min %d)\n",
                ARG_CMD(a), xd->xd_pkt_len, tagged ? "tagged" : "untagged", min_len);
    }

    /** Verify ITMH parameters */
    if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_FEC_POINTER)
    {
        if (xd->itmh_fec_ptr == 0)
        {
            cli_out("Warning: Fec_Ptr is not given when Forward_Type is Fec\n");
        }
        if ((xd->itmh_fec_ptr & 0xc0000) == 0xc0000)
        {
            cli_out("Warning: Fec_Ptr is out of range when Forward_Type is Fec\n");
        }
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_UNICAST_DIRECT)
    {
        if (xd->itmh_dest_sys_port == 0)
        {
            cli_out("Warning: Dest_Sys_Port is not given when Forward_Type is SPA\n");
        }
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_ACTION_PROFILE_IDX)
    {
        if (xd->itmh_action_profile_idx == 0)
        {
            cli_out("Warning: Act_Profile_Idx is not given when Forward_Type is Action\n");
        }
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_UNICAST_FLOW)
    {
        if (xd->itmh_flow_id == 0)
        {
            cli_out("Warning: Flow_ID is not given when Forward_Type is TMFlow\n");
        }
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_SYSTEM_MULTICAST)
    {
        if (xd->itmh_multicast_id == 0)
        {
            cli_out("Warning: Multicast_ID is not given when Forward_Type is MCID\n");
        }
    }

    if (xd->xd_untag)
    {
        xd->xd_vlan = 0;
        xd->xd_prio = 0;
    }

    return CMD_OK;
}

/**
 * \brief - Initialize the TX/RX parameters struct
 */
static void
diag_dnx_xd_init(
    int unit)
{
    xd_t *xd;
    sal_mac_addr_t default_mac_src = { 0x00, 0x00, 0x00, 0x07, 0x00, 0x00 };
    sal_mac_addr_t default_mac_dst = { 0x00, 0x00, 0x00, 0xe3, 0x00, 0x00 };

    /*
     * Allocate memory for the xd struct 
     */
    xd = xd_units[unit];
    if (xd == NULL)
    {
        xd = sal_alloc(sizeof(xd_t), "xd");
        if (xd == NULL)
        {
            cli_out("WARNING: xd memory allocation failed\n");
            return;
        }
        sal_memset(xd, 0, sizeof(xd_t));
        xd_units[unit] = xd;
    }

    /*
     * Init struct parameters 
     */
    xd->xd_unit = unit;
    xd->xd_tx_unit = unit;
    xd->xd_state = XD_IDLE;
    xd->xd_file = NULL;
    xd->xd_pkt_len = TAGGED_PACKET_LENGTH;
    xd->xd_pat = 0x12345678;
    xd->xd_pat_inc = 1;
    xd->xd_pat_random = 0;
    sal_memset(&xd->pkt_info.tx_pbmp, 0, sizeof(bcm_pbmp_t));
    BCM_PBMP_ASSIGN(xd->xd_ppsm_pbm, PBMP_ALL(unit));
    xd->xd_vlan = 0x1;
    xd->xd_prio = 0;
    xd->xd_prio_int = -1;
    xd->xd_ppsm = 0;

    ENET_SET_MACADDR(xd->xd_mac_dst, default_mac_dst);
    ENET_SET_MACADDR(xd->xd_mac_src, default_mac_src);
    /*
     * Append CRC by default
     */
    xd->xd_crc = 1;

    if (xd->pkt_info.pkt_data)
    {
        /*
         * Has been setup before 
         */
        soc_cm_sfree(unit, xd->pkt_info._pkt_data.data);
        xd->pkt_info.pkt_data = NULL;
    }
    xd->pkt_info.flags = 0;

    if ((xd->pkt_info._pkt_data.data = (uint8 *) soc_cm_salloc(unit, xd->xd_pkt_len, "xd tx")) == NULL)
    {
        cli_out("WARNING: xd tx packet memory allocation failed\n");
        xd->pkt_info.pkt_data = NULL;
        xd->pkt_info._pkt_data.len = 0;
    }
    else
    {
        xd->pkt_info.pkt_data = &xd->pkt_info._pkt_data;
        xd->pkt_info.blk_count = 1;
        xd->pkt_info._pkt_data.len = xd->xd_pkt_len;
    }

    /*
     * Clear PTCH/ITMH header contents
     */
    xd->xd_ptch_src_port = 0;
    xd->xd_ptch_header_2 = 0;
    xd->xd_in_tm_port = 0;
    xd->xd_itmh_present = 0;
    xd->itmh_fwd_action_strength = 0;
    xd->itmh_as_ext_present = 0;
    xd->itmh_pph_type = 0;
    xd->itmh_in_mirr_disable = 0;
    xd->itmh_dp = 0;
    xd->itmh_fwd_type = 0;
    xd->itmh_fec_ptr = 0;
    xd->itmh_action_profile_idx = 0;
    xd->itmh_dest_sys_port = 0;
    xd->itmh_flow_id = 0;
    xd->itmh_multicast_id = 0;
    xd->itmh_snoop_profile = 0;
    xd->itmh_traffic_class = 0;
    xd->itmh_base_ext_present = 0;
    xd->itmh_base_ext_type = 0;
    xd->itmh_base_ext_value = 0;
}

#ifndef NO_FILEIO
#define TX_LOAD_MAX             4096

/**
 * \brief - Discard a single line from input file
 */
static int
diag_dnx_discard_line(
    FILE * fp)
{
    int c;
    do
    {
        if ((c = getc(fp)) == EOF)
        {
            return c;
        }
    }
    while (c != '\n');
    return c;
}

/**
 * \brief - Load a single byte from input file
 */
static int
diag_dnx_tx_load_byte(
    FILE * fp,
    uint8 *byte)
{
    int c, d;

    do
    {
        if ((c = getc(fp)) == EOF)
        {
            return -1;
        }
        else if (c == '#')
        {
            if ((c = diag_dnx_discard_line(fp)) == EOF)
            {
                return -1;
            }
        }
    }
    while (!isxdigit(c));

    do
    {
        if ((d = getc(fp)) == EOF)
        {
            return -1;
        }
        else if (d == '#')
        {
            if ((d = diag_dnx_discard_line(fp)) == EOF)
            {
                return -1;
            }
        }
    }
    while (!isxdigit(d));

    *byte = (xdigit2i(c) << 4) | xdigit2i(d);

    return 0;
}

/**
 * \brief - Load full packet from file
 */
static uint8 *
diag_dnx_tx_load_packet(
    int unit,
    char *fname,
    int *length)
{
    uint8 *p;
    FILE *fp;
    int i;

    if ((p = soc_cm_salloc(unit, TX_LOAD_MAX, "tx_packet")) == NULL)
        return p;

    if ((fp = sal_fopen(fname, "r")) == NULL)
    {
        soc_cm_sfree(unit, p);
        return NULL;
    }

    for (i = 0; i < TX_LOAD_MAX; i++)
    {
        if (diag_dnx_tx_load_byte(fp, &p[i]) < 0)
            break;
    }

    *length = i;

    sal_fclose(fp);

    return p;
}
#endif /* NO_FILEIO */

/**
 * \brief - Parse packet information from bcm shell
 */
static uint8 *
diag_dnx_parse_user_packet_payload(
    int unit,
    char *packet_data,
    int *length)
{
    uint8 *p;
    char tmp, data_iter;
    int data_len, byte_len, i, pkt_len, data_base;

    /*
     * If string data starts with 0x or 0X, skip it
     */
    if ((packet_data[0] == '0') && ((packet_data[1] == 'x') || (packet_data[1] == 'X')))
    {
        data_base = 2;
    }
    else
    {
        data_base = 0;
    }

    data_len = strlen(packet_data) - data_base;
    byte_len = (data_len + 1) / 2;
    /*
     * Set input length as minimum length 64 if it's smaller  
     */
    if (*length < UNTAGGED_PACKET_LENGTH)
    {
        *length = UNTAGGED_PACKET_LENGTH;
    }
    pkt_len = byte_len > *length ? byte_len : *length;

    if ((p = soc_cm_salloc(unit, pkt_len, "tx_string_packet")) == NULL)
        return p;

    sal_memset(p, 0, pkt_len);

    /*
     * Convert char to value
     */
    i = 0;
    while (i < data_len)
    {
        data_iter = packet_data[data_base + i];
        if (('0' <= data_iter) && (data_iter <= '9'))
        {
            tmp = data_iter - '0';
        }
        else if (('a' <= data_iter) && (data_iter <= 'f'))
        {
            tmp = data_iter - 'a' + 10;
        }
        else if (('A' <= data_iter) && (data_iter <= 'F'))
        {
            tmp = data_iter - 'A' + 10;
        }
        else
        {
            cli_out("Unexpected char: %c\n", data_iter);
            return NULL;
        }

        /*
         * String input is in 4b unit. Below we're filling in 8b:
         * offset is /2, and we shift by 4b if the input char is odd 
         */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
    }

    *length = pkt_len;
    return p;
}

/**
 * \brief - If data was read from file, extract to XD structure
 */
static void
diag_dnx_check_pkt_fields(
    xd_t * xd)
{
    /*
     * Ethernet packet header 
     */
    enet_hdr_t *ep;
    bcm_pkt_t *pkt_info = &xd->pkt_info;
    uint16 tpid;

    ep = (enet_hdr_t *) BCM_PKT_IEEE(pkt_info);

    if (XD_FILE(xd) || XD_STRING(xd))
    {
        /*
         * Loaded from file or string
         * Also set parameters to file data so incrementing works 
         */

        ENET_COPY_MACADDR(&ep->en_dhost, xd->xd_mac_dst);
        ENET_COPY_MACADDR(&ep->en_shost, xd->xd_mac_src);

        tpid = soc_ntohs(ep->en_tag_tpid);
        if ((tpid != 0x8100) && (tpid != 0x9100))
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META("Warning:  Untagged packet read from file or string for tx.\n")));
            xd->xd_vlan = VLAN_ID_NONE;
        }
        else
        {
            xd->xd_vlan = VLAN_CTRL_ID(bcm_ntohs(ep->en_tag_ctrl));
            xd->xd_prio = VLAN_CTRL_PRIO(bcm_ntohs(ep->en_tag_ctrl));
        }
    }
    else
    {
        if (xd->xd_vlan)
        {
            /*
             * Tagged format
             */
            ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(xd->xd_prio, 0, xd->xd_vlan));
            ep->en_tag_len = bcm_htons(xd->xd_pkt_len) - ENET_TAGGED_HDR_LEN - ENET_CHECKSUM_LEN;
            ep->en_tag_tpid = bcm_htons(0x8100);
        }
        else
        {
            /*
             * Untagged format 
             */
            LOG_WARN(BSL_LOG_MODULE, (BSL_META("Warning:  Sending untagged packet.\n")));
            ep->en_untagged_len = bcm_htons(xd->xd_pkt_len) - ENET_UNTAGGED_HDR_LEN - ENET_CHECKSUM_LEN;
        }
        ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
        ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);
    }
}

/**
 * \brief - Get the first port in a bitmap
 */
static void
diag_dnx_tx_first_pbm(
    int unit,
    bcm_pbmp_t * pbm,
    bcm_pbmp_t * rpbm)
{
    bcm_port_t p;

    BCM_PBMP_CLEAR(*rpbm);
    BCM_PBMP_ITER(*pbm, p)
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "First to port %d\n"), p));
        BCM_PBMP_PORT_ADD(*rpbm, p);
        return;
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Warning: first pbm null\n")));
}

/**
 * \brief - Build ITMH base header for JR2 mode
 */
static int
diag_dnx_tx_setup_itmh_tx_jr2(
    xd_t * xd)
{
    bcm_pkt_t *pkt_info = &xd->pkt_info;
    diag_dnx_tx_itmh_t itmh;
    uint32 itmh_tc = 0;
    int itmh_size = 0;

    sal_memset(&itmh, 0, sizeof(diag_dnx_tx_itmh_t));

    if (-1 != xd->xd_prio_int)
    {
        itmh_tc = xd->xd_prio_int;
    }
    else
    {
        itmh_tc = xd->itmh_traffic_class;
    }

    itmh.base.jr2_common.forward_action_strength = xd->itmh_fwd_action_strength;
    itmh.base.jr2_common.injected_as_extension_present = xd->itmh_as_ext_present;
    itmh.base.jr2_common.pph_type = xd->itmh_pph_type;
    itmh.base.jr2_common.inbound_mirror_disable = xd->itmh_in_mirr_disable;
    itmh.base.jr2_common.snoop_profile_hi = xd->itmh_snoop_profile >> 4 & 0x1;
    itmh.base.jr2_common.snoop_profile_lo = xd->itmh_snoop_profile & 0xf;
    itmh.base.jr2_common.traffic_class = itmh_tc;
    itmh.base.jr2_common.drop_precedence = xd->itmh_dp;
    itmh.base.jr2_common.itmh_base_ext_present = xd->itmh_base_ext_present;

    if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_FEC_POINTER)
    {
        /*
         * 0(1) + FEC-PTR(20)
         */
        if ((xd->itmh_fec_ptr & 0xc0000) == 0xc0000)
        {
            /*
             * 0(1) + 11(2)+ FEC-PTR -> 0(1) + 10(2)+ FEC-PTR
             */
            xd->itmh_fec_ptr &= 0xfffbffff;
        }
        itmh.base.jr2_dest_info.destination_hi = (xd->itmh_fec_ptr >> 15) & 0x1f;
        itmh.base.jr2_dest_info.destination_mi = (xd->itmh_fec_ptr >> 7) & 0xff;
        itmh.base.jr2_dest_info.destination_lo = xd->itmh_fec_ptr & 0x7f;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_ACTION_PROFILE_IDX)
    {
        /*
         * 01111(5) + SNS(3) + FWS(4) + Action-Profile-Idx(9)
         */
        itmh.base.jr2_dest_info.destination_hi = 0x1e | ((xd->itmh_action_profile_idx >> 15) & 0x1);
        itmh.base.jr2_dest_info.destination_mi = xd->itmh_action_profile_idx >> 7 & 0xff;
        itmh.base.jr2_dest_info.destination_lo = xd->itmh_action_profile_idx & 0x7f;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_UNICAST_DIRECT)
    {
        /*
         * 01100(5) + System_Port_Agg(16)
         */
        itmh.base.jr2_dest_info.destination_hi = 0x18 | ((xd->itmh_dest_sys_port >> 15) & 0x1);
        itmh.base.jr2_dest_info.destination_mi = xd->itmh_dest_sys_port >> 7 & 0xff;
        itmh.base.jr2_dest_info.destination_lo = xd->itmh_dest_sys_port & 0x7f;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_UNICAST_FLOW)
    {
        /*
         * 10(2) + TM-Flow(19)
         */
        itmh.base.jr2_dest_info.destination_hi = 0x20 | ((xd->itmh_flow_id >> 15) & 0xf);
        itmh.base.jr2_dest_info.destination_mi = xd->itmh_flow_id >> 7 & 0xff;
        itmh.base.jr2_dest_info.destination_lo = xd->itmh_flow_id & 0x7f;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_SYSTEM_MULTICAST)
    {
        /*
         * 11(2) + MC-ID(19)
         */
        itmh.base.jr2_dest_info.destination_hi = 0x30 | ((xd->itmh_multicast_id >> 15) & 0xf);
        itmh.base.jr2_dest_info.destination_mi = xd->itmh_multicast_id >> 7 & 0xff;
        itmh.base.jr2_dest_info.destination_lo = xd->itmh_multicast_id & 0x7f;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE;
    }
    else
    {
        /*
         * Default is System_Port_Agg(16)
         */
        cli_out("Warning: Invalid Forward_Type(%d), Set Destination as SPA\n", xd->itmh_fwd_type);
        itmh.base.jr2_dest_info.destination_hi = 0x18 | ((xd->itmh_dest_sys_port >> 15) & 0x1);
        itmh.base.jr2_dest_info.destination_mi = xd->itmh_dest_sys_port >> 7 & 0xff;
        itmh.base.jr2_dest_info.destination_lo = xd->itmh_dest_sys_port & 0x7f;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE;
    }

    if (xd->itmh_base_ext_present)
    {
        itmh.base_ext.base_ext.reserved = 0;
        itmh.base_ext.base_ext.type = xd->itmh_base_ext_type;
        itmh.base_ext.base_ext.value1_hi = (xd->itmh_base_ext_value >> 16) & 0x3f;
        itmh.base_ext.base_ext.value1_mi = (xd->itmh_base_ext_value >> 8) & 0xff;
        itmh.base_ext.base_ext.value1_lo = xd->itmh_base_ext_value & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_EXT;
    }

    sal_memcpy(&(pkt_info->_dpp_hdr[0]), &itmh.base.raw.bytes[0], TX_DNX_ITMH_BASE_LEN);
    if (TX_DNX_ITMH_BASE_EXT == pkt_info->_dpp_hdr_type)
    {
        sal_memcpy(&(pkt_info->_dpp_hdr[TX_DNX_ITMH_BASE_LEN]), &itmh.base_ext.raw.bytes[0], TX_DNX_ITMH_BASE_EXT_LEN);
    }
    LOG_INFO(BSL_LS_APPL_TX,
             (BSL_META("ITMH: 0x%02x-%02x-%02x-%02x-%02x\n"), pkt_info->_dpp_hdr[0], pkt_info->_dpp_hdr[1],
              pkt_info->_dpp_hdr[2], pkt_info->_dpp_hdr[3], pkt_info->_dpp_hdr[4]));

    if (TX_DNX_ITMH_BASE == pkt_info->_dpp_hdr_type)
    {
        itmh_size = TX_DNX_ITMH_BASE_LEN;
    }
    else if (TX_DNX_ITMH_BASE_EXT == pkt_info->_dpp_hdr_type)
    {
        itmh_size = TX_DNX_ITMH_BASE_LEN + TX_DNX_ITMH_BASE_EXT_LEN;
    }

    return itmh_size;
}

/**
 * \brief - Build ITMH base header for JR1 mode
 */
STATIC int
diag_dnx_tx_setup_itmh_tx_jr1(
    xd_t * xd)
{
    bcm_pkt_t *pkt_info = &xd->pkt_info;
    diag_dnx_tx_itmh_jr1_t itmh;
    uint32 itmh_tc = 0;
    int itmh_size = 0;

    sal_memset(&itmh, 0, sizeof(diag_dnx_tx_itmh_jr1_t));
    if (-1 != xd->xd_prio_int)
    {
        itmh_tc = xd->xd_prio_int;
    }
    else
    {
        itmh_tc = xd->itmh_traffic_class;
    }

    itmh.base.jer_common.pph_type = xd->itmh_pph_type;
    itmh.base.jer_common.in_mirr_flag = xd->itmh_in_mirr_disable;
    itmh.base.jer_common.dp = xd->itmh_dp;

    if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_UNICAST_DIRECT)
    {
        itmh.base.jer_dest_info.dest_info_hi = 0x1;
        itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_dest_sys_port >> 8) & 0xff;
        itmh.base.jer_dest_info.dest_info_lo = xd->itmh_dest_sys_port & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_JR1;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_UNICAST_FLOW)
    {
        itmh.base.jer_dest_info.dest_info_hi = 0x6 | ((xd->itmh_flow_id >> 16) & 0x1);
        itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_flow_id >> 8) & 0xff;
        itmh.base.jer_dest_info.dest_info_lo = xd->itmh_flow_id & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_JR1;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_FEC_POINTER)
    {
        itmh.base.jer_dest_info.dest_info_hi = 0x2 | ((xd->itmh_fec_ptr >> 16) & 0x1);
        itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_fec_ptr >> 8) & 0xff;
        itmh.base.jer_dest_info.dest_info_lo = xd->itmh_fec_ptr & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_JR1;
    }
    else if (xd->itmh_fwd_type == TX_DNX_ITMH_FWD_TYPE_SYSTEM_MULTICAST)
    {
        itmh.base.jer_dest_info.dest_info_hi = 0x4 | ((xd->itmh_multicast_id >> 16) & 0x1);
        itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_multicast_id >> 8) & 0xff;
        itmh.base.jer_dest_info.dest_info_lo = xd->itmh_multicast_id & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_JR1;
    }
    else
    {
        cli_out("Warning: Invalid Forward_Type(%d), Set Destination as SPA\n", xd->itmh_fwd_type);
        itmh.base.jer_dest_info.dest_info_hi = 0x1;
        itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_dest_sys_port >> 8) & 0xff;
        itmh.base.jer_dest_info.dest_info_lo = xd->itmh_dest_sys_port & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_JR1;
    }

    itmh.base.jer_common.snoop_cmd = xd->itmh_snoop_profile;
    itmh.base.jer_common.traffic_class_hi = (itmh_tc >> 2) & 0x1;
    itmh.base.jer_common.traffic_class_lo = itmh_tc & 0x3;
    itmh.base.jer_common.itmh_base_ext = xd->itmh_base_ext_present;

    if (xd->itmh_base_ext_present)
    {
        itmh.base_ext.base_ext.reserved = 0;
        itmh.base_ext.base_ext.type = xd->itmh_base_ext_type;
        itmh.base_ext.base_ext.value1_hi = (xd->itmh_base_ext_value >> 16) & 0xf;
        itmh.base_ext.base_ext.value1_mi = (xd->itmh_base_ext_value >> 8) & 0xff;
        itmh.base_ext.base_ext.value1_lo = xd->itmh_base_ext_value & 0xff;
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_BASE_EXT_JR1;
    }

    sal_memcpy(&(pkt_info->_dpp_hdr[0]), &itmh.base.raw.bytes[0], TX_DNX_ITMH_BASE_LEN_JR1);
    if (TX_DNX_ITMH_BASE_EXT_JR1 == pkt_info->_dpp_hdr_type)
    {
        sal_memcpy(&(pkt_info->_dpp_hdr[TX_DNX_ITMH_BASE_LEN_JR1]), &itmh.base_ext.raw.bytes[0],
                   TX_DNX_ITMH_BASE_EXT_LEN_JR1);
    }
    LOG_DEBUG(BSL_LS_APPL_TX, (BSL_META("ITMH: 0x%02x-%02x-%02x-%02x \n"),
                               pkt_info->_dpp_hdr[0],
                               pkt_info->_dpp_hdr[1], pkt_info->_dpp_hdr[2], pkt_info->_dpp_hdr[3]));

    if (TX_DNX_ITMH_BASE_JR1 == pkt_info->_dpp_hdr_type)
    {
        itmh_size = TX_DNX_ITMH_BASE_LEN_JR1;
    }
    else if (TX_DNX_ITMH_BASE_EXT_JR1 == pkt_info->_dpp_hdr_type)
    {
        itmh_size = TX_DNX_ITMH_BASE_LEN_JR1 + TX_DNX_ITMH_BASE_EXT_LEN_JR1;
    }

    return itmh_size;
}

/**
 * \brief - Build ITMH base header
 */
static int
diag_dnx_tx_setup_itmh_tx(
    xd_t * xd,
    int system_headers_mode)
{
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        return diag_dnx_tx_setup_itmh_tx_jr1(xd);
    }
    else
    {
        return diag_dnx_tx_setup_itmh_tx_jr2(xd);
    }
}

/**
 * \brief - Build ITMH base header, overrite Destination
 */
static void
diag_dnx_tx_itmh_dest_port_set(
    int unit,
    uint8 *itmh_header,
    uint32 dest_port,
    int system_headers_mode)
{
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        /*
         * 001(3) + System_Port_Agg(16)
         */
        diag_dnx_tx_itmh_jr1_t *itmh_jr1 = (diag_dnx_tx_itmh_jr1_t *) itmh_header;
        itmh_jr1->base.jer_dest_info.dest_info_hi = 0x1;
        itmh_jr1->base.jer_dest_info.dest_info_mi = (dest_port >> 8) & 0xff;
        itmh_jr1->base.jer_dest_info.dest_info_lo = dest_port & 0xff;
    }
    else
    {
        diag_dnx_tx_itmh_t *itmh_jr2 = (diag_dnx_tx_itmh_t *) itmh_header;

        /*
         * 01100(5) + System_Port_Agg(16)
         */
        itmh_jr2->base.jr2_dest_info.destination_hi = 0x18 | ((dest_port >> 15) & 0x1);
        itmh_jr2->base.jr2_dest_info.destination_mi = (dest_port >> 7) & 0xff;
        itmh_jr2->base.jr2_dest_info.destination_lo = dest_port & 0x7f;
    }
    return;
}

/**
 * \brief - strip VLAN tag from packet marked by parameter "data"
 */
static void
diag_dnx_tx_tag_strip(
    int unit,
    uint8 *data)
{
    xd_t *xd = xd_units[unit];
    bcm_pkt_t *pkt_info = &xd->pkt_info;
    enet_hdr_t *ep;

    ep = (enet_hdr_t *) data;

    if (!ENET_TAGGED(ep))
    {
        return;
    }
    sal_memcpy(data + 2 * ENET_MAC_SIZE, data + 2 * ENET_MAC_SIZE + ENET_TAG_SIZE, xd->xd_pkt_len - 16);
    xd->xd_pkt_len -= ENET_TAG_SIZE;
    pkt_info->pkt_data[0].len -= ENET_TAG_SIZE;

    return;
}

/**
 * \brief - Recover VLAN tag from packet marked by parameter "data"
 */
static void
diag_dnx_tx_tag_recover(
    int unit,
    uint8 *data)
{
    xd_t *xd = xd_units[unit];
    bcm_pkt_t *pkt_info = &xd->pkt_info;
    enet_hdr_t *ep;

    ep = (enet_hdr_t *) data;
    if (!xd->xd_vlan)
    {
        return;
    }
    sal_memcpy(data + 2 * ENET_MAC_SIZE + ENET_TAG_SIZE, data + 2 * ENET_MAC_SIZE, xd->xd_pkt_len - 2 * ENET_MAC_SIZE);
    ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(xd->xd_prio, 0, xd->xd_vlan));
    ep->en_tag_tpid = bcm_htons(ENET_DEFAULT_TPID);
    xd->xd_pkt_len += ENET_TAG_SIZE;
    pkt_info->pkt_data[0].len += ENET_TAG_SIZE;

    return;
}

/**
 * \brief - Get the next port in a bitmap
 */
static int
diag_dnx_tx_next_port(
    int unit,
    bcm_pbmp_t * newpbm,
    bcm_pbmp_t * allpbm)
{
    bcm_port_t p;
    bcm_port_t np;
    int found = FALSE;
    char pfmt[SOC_PBMP_FMT_LEN];

    BCM_PBMP_ITER(*newpbm, np)
    {
        BCM_PBMP_ITER(*allpbm, p)
        {
            if (found)
            {
                LOG_INFO(BSL_LS_APPL_TX, (BSL_META_U(unit, "Next to port %d\n"), p));
                BCM_PBMP_CLEAR(*newpbm);
                BCM_PBMP_PORT_ADD(*newpbm, p);
                return 0;
            }
            if (np == p)
            {
                found = TRUE;
            }
        }
    }

    /*
     * If we get here, must be resetting. 
     */
    diag_dnx_tx_first_pbm(unit, allpbm, newpbm);
    LOG_INFO(BSL_LS_APPL_TX, (BSL_META_U(unit, "Resetting to pbm %s\n"), SOC_PBMP_FMT(*newpbm, pfmt)));
    return 1;
}

/**
 * \brief - Get the switch header type of in port
 */
static cmd_result_t
diag_dnx_tx_switch_header_type_in_get(
    int unit,
    bcm_port_t port,
    int *incoming_header_type)
{
    dbal_enum_value_field_port_termination_ptc_profile_e prt_profile;
    int rv;

    /** Get the Prt ptc profile */
    rv = dnx_port_prt_ptc_profile_get(unit, port, &prt_profile);
    if (BCM_E_NONE != rv)
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to get ptc profile %s\n"), bcm_errmsg(rv)));
        return (CMD_FAIL);
    }

    switch (prt_profile)
    {
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET:
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE:
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PON:
        {
            *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
            break;
        }
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2:
        {
            if (SOC_PBMP_MEMBER(PBMP_E_ALL(unit), port))
            {
                *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
            }
            else
            {
                *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2;
            }
            break;
        }
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1:
        {
            *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE;
            break;
        }
        default:
        {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("Invalid prt_profile =%d \n"), prt_profile));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/**
 * \brief - Transmit packet from BCM shell using bcm_tx API
 */
static cmd_result_t
diag_dnx_do_tx(
    xd_t * xd)
{
    uint8 *pkt_data;            /* Packet */
    uint8 *payload;             /* data - packet payload */
    enet_hdr_t *ep;             /* Ethernet packet header */
    bcm_pkt_t *pkt_info = &xd->pkt_info;
    int rv = BCM_E_INTERNAL;
    int payload_len;
    int bitmap_not_null = FALSE;
    uint32 port_ndx;
    bcm_gport_t sysport_gport;
    int header_size = 0;
    uint8 *pkt_data_with_header;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t ptch_ssp;
    bcm_port_t in_tm_port;
    bcm_port_t in_tm_port_pp;
    bcm_core_t ptch_ssp_core;
    bcm_core_t in_tm_port_core;
    bcm_core_t core;
    uint8 is_in_tm_port_set = FALSE;
    uint8 is_ptch_ssp_set = FALSE;
    uint8 is_ptch_ssp_cpu = FALSE;
    uint8 is_in_tm_port_cpu = FALSE;
    uint8 has_cpu_port = FALSE;
    int header_type_incoming;
    int cpu_channel;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int system_headers_mode;

    pkt_data = NULL;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(xd->xd_unit);

    /*
     * Allocate the packet; use tx_load if reading from file 
     */
    if (XD_FILE(xd))
    {
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
        return CMD_FAIL;
#else
        if ((pkt_data = diag_dnx_tx_load_packet(xd->xd_tx_unit, xd->xd_file, &xd->xd_pkt_len)) == NULL)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Unable to load packet from file %s\n"), xd->xd_file));
            return CMD_FAIL;
        }
#endif
    }

    /*
     * Read from user string input
     */
    if (XD_STRING(xd))
    {
        if ((pkt_data = diag_dnx_parse_user_packet_payload(xd->xd_tx_unit, xd->xd_data, &xd->xd_pkt_len)) == NULL)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Unable to allocate memory for packet %s\n"), xd->xd_data));
            return CMD_FAIL;
        }
    }

    /*
     * Make sure packet allocation size is right size
     */
    if (pkt_info->pkt_data[0].len != xd->xd_pkt_len)
    {
        if (pkt_info->pkt_data[0].data)
        {
            soc_cm_sfree(xd->xd_tx_unit, pkt_info->pkt_data[0].data);
        }
        if ((pkt_info->pkt_data[0].data = (uint8 *) soc_cm_salloc(xd->xd_tx_unit, xd->xd_pkt_len, "TX")) == NULL)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Unable to allocate packet memory\n")));
            return (CMD_FAIL);
        }
        pkt_info->pkt_data[0].len = xd->xd_pkt_len;
    }

    sal_memset(pkt_info->pkt_data[0].data, 0, pkt_info->pkt_data[0].len);

    /*
     * Data was read from file or string, copy into pkt_info
     */
    if (pkt_data)
    {
        sal_memcpy(pkt_info->pkt_data[0].data, pkt_data, xd->xd_pkt_len);
        soc_cm_sfree(xd->xd_tx_unit, pkt_data);
    }

    /*
     * Setup the packet 
     */
    pkt_info->flags &= ~BCM_TX_CRC_FLD;
    pkt_info->flags |= (xd->xd_crc == 1 ? BCM_TX_CRC_REGEN : 0) | (xd->xd_crc == 2 ? BCM_TX_CRC_APPEND : 0);

    pkt_info->flags &= ~BCM_TX_NO_PAD;
    if (xd->xd_pkt_len < (xd->xd_vlan != 0 ? TAGGED_PACKET_LENGTH : UNTAGGED_PACKET_LENGTH))
    {
        pkt_info->flags |= BCM_TX_NO_PAD;
    }

    /** Get PP Port of ptch_src_port and corresponding core */
    rv = bcm_port_get(xd->xd_unit, xd->xd_ptch_src_port, &flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != rv)
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to get pp port and core %s\n"), bcm_errmsg(rv)));
        return (CMD_FAIL);
    }

    ptch_ssp = mapping_info.pp_port;
    ptch_ssp_core = mapping_info.core;

    /** Set a flag to indicate that the configured PtchSRCport is CPU port */
    if (interface_info.interface == _SHR_PORT_IF_CPU)
    {
        is_ptch_ssp_cpu = TRUE;
    }

    /** Check if the given PtchSRCport is equal to the ssp in given PTCH header */
    if (xd->xd_ptch_header_2)
    {
        if (ptch_ssp != (xd->xd_ptch_header_2 & 0xff))
        {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("PtchSRCport is inconsistent with ssp of given PtchHeader\n")));
            return (CMD_FAIL);
        }

    }

    /** Get PP Port of in_tm_port and corresponding core */
    rv = bcm_port_get(xd->xd_unit, xd->xd_in_tm_port, &flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != rv)
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to get pp port and core %s\n"), bcm_errmsg(rv)));
        return (CMD_FAIL);
    }
    /** Record pp port and core id of configured Source TM port */
    in_tm_port_pp = mapping_info.pp_port;
    in_tm_port_core = mapping_info.core;
    in_tm_port = xd->xd_in_tm_port;
    /** Set a flag to inidcate that configured source TM port is CPU port */
    if (interface_info.interface == _SHR_PORT_IF_CPU)
    {
        is_in_tm_port_cpu = TRUE;
    }

    /** Check whether parameter PORT and PtchSRCport are set */
    BCM_PBMP_ITER(PBMP_CMIC(xd->xd_unit), port_ndx)
    {
        if (xd->xd_ptch_src_port == port_ndx)
        {
            /** Parameter PtchSRCport is not specified, since first CPU port is default value */
            is_ptch_ssp_set = FALSE;
            if (xd->xd_ptch_header_2)
            {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("PtchSRCport must be given when PtchHeader is set\n")));
                return (CMD_FAIL);
            }
        }
        else
        {
            /** Parameter PtchSRCport is set */
            is_ptch_ssp_set = TRUE;
        }
        if (xd->xd_in_tm_port == port_ndx)
        {
            /** Parameter PORT is not specified, since first CPU port is default value */
            is_in_tm_port_set = FALSE;
        }
        else
        {
            /** Parameter PORT is set*/
            is_in_tm_port_set = TRUE;
        }
        break;
    }
    /** Check whether parameter PORT and PtchSRCport are on the same core */
    if (is_in_tm_port_set)
    {
        /** Both source TM port and ssp in PTCH are set, they must be on the same core */
        if (is_ptch_ssp_set)
        {
            if (ptch_ssp_core != in_tm_port_core)
            {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META
                           ("TX failed because source TM port(%d,%d) and SSP(%d,%d) in PTCH are not in the same core\n"),
                           xd->xd_in_tm_port, in_tm_port_core, xd->xd_ptch_src_port, ptch_ssp_core));
                return (CMD_FAIL);
            }
        }
        else
        {
            /*
             * Take source TM port(PORT) as TX port when PtchSRCport is not specified,
             * If source TM port is CPU port, PtchSRCport is equal to source TM port,
             * Otherwise return error. 
             */
            if (is_in_tm_port_cpu)
            {
                ptch_ssp = in_tm_port_pp;
                ptch_ssp_core = in_tm_port_core;
            }
            else
            {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META
                           ("TX failed because source TM port(%d,%d) is not CPU port\n"),
                           xd->xd_in_tm_port, in_tm_port_core));
                return (CMD_FAIL);
            }
        }
    }
    else
    {
        if (is_ptch_ssp_set)
        {
            /*
             * Take PtchSRCport as TX port when source TM port(PORT) is not specified,
             * If Tx port is CPU port, source TM port is equal to PtchSRCport,
             * Otherwise find first CPU port on the same core as source TM port.
             */
            if (is_ptch_ssp_cpu)
            {
                in_tm_port = xd->xd_ptch_src_port;
                in_tm_port_core = ptch_ssp_core;
                has_cpu_port = TRUE;
            }
            else
            {
                if (ptch_ssp_core != in_tm_port_core)
                {
                    cli_out("Warning: source TM port(%d,%d) and SSP(%d,%d) in PTCH are not in the same core\n",
                            xd->xd_in_tm_port, in_tm_port_core, xd->xd_ptch_src_port, ptch_ssp_core);

                    BCM_PBMP_ITER(PBMP_CMIC(xd->xd_unit), port_ndx)
                    {
                        rv = dnx_algo_port_core_get(xd->xd_unit, port_ndx, &core);
                        if (BCM_E_NONE != rv)
                        {
                            LOG_ERROR(BSL_LS_APPL_SHELL,
                                      (BSL_META("TX failed to get core id for CPU port(%d) %s\n"), port_ndx,
                                       bcm_errmsg(rv)));
                            return (CMD_FAIL);
                        }

                        if (core == ptch_ssp_core)
                        {
                            /** Change source TM port to first CPU port which is on the same core with SSP */
                            in_tm_port = port_ndx;
                            in_tm_port_core = core;
                            has_cpu_port = TRUE;
                            break;
                        }
                    }
                    if (!has_cpu_port)
                    {
                        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("No cpu port\n")));
                        return CMD_FAIL;
                    }
                }
            }
        }
    }

    /** Check if psrc is trunk */
    if (ptch_ssp == 0xFFFFFFFF)
    {
        bcm_gport_t gport;
        bcm_trunk_t tid_find;
        bcm_trunk_pp_port_allocation_info_t allocation_info;
        /** Switch local to sysport */
        rv = dnx_algo_gpm_gport_phy_info_get
            (xd->xd_unit, xd->xd_ptch_src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);

        if (BCM_E_NONE != rv)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to switch local to sysport %s\n"), bcm_errmsg(rv)));
            return (CMD_FAIL);
        }
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, gport_info.sys_port);

        /** Get trunk id from sysport */
        rv = bcm_trunk_find(xd->xd_unit, -1, gport, &tid_find);
        if (BCM_E_NONE != rv)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to find trunk id %s\n"), bcm_errmsg(rv)));
            return (CMD_FAIL);
        }
        /** Get the pp port allocation info of a given trunk */
        rv = bcm_trunk_pp_port_allocation_get(xd->xd_unit, tid_find, 0, &allocation_info);
        if (BCM_E_NONE != rv)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("TX failed to get pp port allocation info per given trunk %s\n"), bcm_errmsg(rv)));
            return (CMD_FAIL);
        }
        /** Check that trunk has a core on this member port */
        if ((SAL_BIT(ptch_ssp_core) & allocation_info.core_bitmap) == 0)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("PtchSRCport %d is not a valid member of Trunk %d\n"), xd->xd_ptch_src_port, tid_find));
            return (CMD_FAIL);
        }
        else
        {
            ptch_ssp = allocation_info.pp_port_per_core_array[ptch_ssp_core];
        }
    }

    diag_dnx_check_pkt_fields(xd);

    if (CMD_OK != diag_dnx_tx_switch_header_type_in_get(xd->xd_unit, in_tm_port, &header_type_incoming))
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to get incoming tm port header type %s\n"), bcm_errmsg(rv)));
        return (CMD_FAIL);
    }

    if (!xd->xd_visibility)
    {
        /** Disable visibility resume */
        pkt_info->flags |= BCM_TX_NO_VISIBILITY_RESUME;
    }

    if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
    {
        pkt_info->_dpp_hdr_type = TX_DNX_ITMH_NONE;
        if (BCM_PBMP_NOT_NULL(pkt_info->tx_pbmp))
        {
            bitmap_not_null = TRUE;
            /*
             * Build ITMH header then update Destination via diag_dnx_tx_itmh_dest_port_set
             */
            xd->xd_prio_int = -1;
            xd->itmh_traffic_class = xd->xd_prio;
            xd->itmh_fwd_type = TX_DNX_ITMH_FWD_TYPE_UNICAST_DIRECT;
            header_size = diag_dnx_tx_setup_itmh_tx(xd, system_headers_mode);
        }
        else if (xd->xd_itmh_present)
        {
            /*
             * Build ITMH header per parameters
             */
            header_size = diag_dnx_tx_setup_itmh_tx(xd, system_headers_mode);
        }
        /** PTCH2 Header */
        header_size += TX_DNX_PTCH_TYPE2_LEN;
        /** Module Header */
        header_size += DNX_MODULE_HEADER_SIZE;

        pkt_data_with_header =
            (uint8 *) soc_cm_salloc(xd->xd_tx_unit, xd->xd_pkt_len + header_size, "TX packet with ptch");
        if (pkt_data_with_header == NULL)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("Unable to allocate packet memory\n")));
            return (CMD_FAIL);
        }

        /** Zero bytes of Module header */
        sal_memset(pkt_data_with_header, 0, DNX_MODULE_HEADER_SIZE);

        if ((TX_DNX_ITMH_BASE == pkt_info->_dpp_hdr_type) || (TX_DNX_ITMH_BASE_EXT == pkt_info->_dpp_hdr_type))
        {
            /*
             * Parser-Program-Control = 0, indicate next header is ITMH
             */
            if (xd->xd_ptch_header_2)
            {
                /** Check Parser-Program-Control when whole PTCH header is specified */
                if ((xd->xd_ptch_header_2 >> 15) & 0x1)
                {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META("Incorrect PTCH header, Parser-Program-Control should be 0\n")));
                    return (CMD_FAIL);
                }
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = (xd->xd_ptch_header_2 >> 8) & 0xff;
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = xd->xd_ptch_header_2 & 0xff;
            }
            else
            {
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = 0x50 | ((ptch_ssp >> 8) & 0x3);
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
            }
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN] = pkt_info->_dpp_hdr[0];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 1] = pkt_info->_dpp_hdr[1];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 2] = pkt_info->_dpp_hdr[2];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 3] = pkt_info->_dpp_hdr[3];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 4] = pkt_info->_dpp_hdr[4];

            if (TX_DNX_ITMH_BASE_EXT == pkt_info->_dpp_hdr_type)
            {
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + TX_DNX_ITMH_BASE_LEN] =
                    pkt_info->_dpp_hdr[5];
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + TX_DNX_ITMH_BASE_LEN + 1] =
                    pkt_info->_dpp_hdr[6];
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + TX_DNX_ITMH_BASE_LEN + 2] =
                    pkt_info->_dpp_hdr[7];
            }
        }
        else if (TX_DNX_ITMH_BASE_JR1 == pkt_info->_dpp_hdr_type
                 || (TX_DNX_ITMH_BASE_EXT_JR1 == pkt_info->_dpp_hdr_type))
        {
            /*
             * Parser-Program-Control = 0, indicate next header is ITMH
             */
            if (xd->xd_ptch_header_2)
            {
                /** Check Parser-Program-Control when whole PTCH header is specified */
                if ((xd->xd_ptch_header_2 >> 15) & 0x1)
                {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META("Incorrect PTCH header, Parser-Program-Control should be 0\n")));
                    return (CMD_FAIL);
                }
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = (xd->xd_ptch_header_2 >> 8) & 0xff;
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = xd->xd_ptch_header_2 & 0xff;
            }
            else
            {
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = 0x50;
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
            }
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN] = pkt_info->_dpp_hdr[0];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 1] = pkt_info->_dpp_hdr[1];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 2] = pkt_info->_dpp_hdr[2];
            pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + 3] = pkt_info->_dpp_hdr[3];

            if (TX_DNX_ITMH_BASE_EXT_JR1 == pkt_info->_dpp_hdr_type)
            {
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + TX_DNX_ITMH_BASE_LEN_JR1] =
                    pkt_info->_dpp_hdr[4];
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + TX_DNX_ITMH_BASE_LEN_JR1 + 1] =
                    pkt_info->_dpp_hdr[5];
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN + TX_DNX_ITMH_BASE_LEN_JR1 + 2] =
                    pkt_info->_dpp_hdr[6];
            }
        }
        else
        {
            /*
             * Parser-Program-Control = 1, next header should be deduced from the SSP
             */
            if (xd->xd_ptch_header_2)
            {
                /** Check Parser-Program-Control when whole PTCH header is specified */
                if (0x1 != ((xd->xd_ptch_header_2 >> 15) & 0x1))
                {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META("Incorrect PTCH header, Parser-Program-Control should be 1\n")));
                    return (CMD_FAIL);
                }
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = (xd->xd_ptch_header_2 >> 8) & 0xff;
                pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = xd->xd_ptch_header_2 & 0xff;
            }
            else
            {
                if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = 0xd0;
                    pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
                }
                else
                {
                    pkt_data_with_header[DNX_MODULE_HEADER_SIZE] = (0xd0 | ((ptch_ssp >> 8) & 0x3));
                    pkt_data_with_header[DNX_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
                }
            }

        }
        /*
         * Copy packet payload to new packet and delete old packet
         */
        sal_memcpy(&pkt_data_with_header[header_size], pkt_info->pkt_data[0].data, pkt_info->pkt_data[0].len);
        soc_cm_sfree(xd->xd_tx_unit, pkt_info->pkt_data[0].data);
        pkt_info->pkt_data[0].data = pkt_data_with_header;
        pkt_info->pkt_data[0].len += header_size;

        /*
         * Set higig flag to indicate Module Header is present
         * Set the incoming CPU channel into dpp_header
         */
        pkt_info->flags |= BCM_PKT_F_HGHDR;
        rv = dnx_algo_port_channel_get(xd->xd_unit, in_tm_port, &cpu_channel);
        if (BCM_E_NONE != rv)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to get cpu channel %s\n"), bcm_errmsg(rv)));
            return (CMD_FAIL);
        }
        pkt_info->_dpp_hdr[0] = cpu_channel;
#ifdef ADAPTER_SERVER_MODE
        /*
         * PTC is got from pkt->src_gport on Cmodel
         * Given PTC value is equal to value of source TM port
         * If source TM port is not specified, it will be set as PtchSRCport
         */
        pkt_info->src_gport = in_tm_port;
#endif

        ep = (enet_hdr_t *) (&pkt_info->pkt_data[0].data[header_size]);
    }
    else
    {
        if (xd->xd_ptch_src_port != 0)
        {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Can't add PTCH src port because src port doen't support PTCH header\n")));
            return (CMD_FAIL);
        }
        ep = (enet_hdr_t *) (pkt_info->pkt_data[0].data);
        header_size = 0;
    }
    payload = &pkt_info->pkt_data[0].data[header_size + sizeof(enet_hdr_t)];
    payload_len = pkt_info->pkt_data[0].len - header_size - sizeof(enet_hdr_t);

    /*
     * Save base info. setup first port.
     */
    if (xd->xd_ppsm)
    {
        LOG_INFO(BSL_LS_APPL_TX, (BSL_META("Per port source is active\n")));
        xd->xd_ppsm_pbm = pkt_info->tx_pbmp;
        diag_dnx_tx_first_pbm(xd->xd_unit, &pkt_info->tx_pbmp, &pkt_info->tx_pbmp);
        ENET_COPY_MACADDR(xd->xd_mac_src, xd->xd_mac_src_base);
    }

    /*
     * XMIT all the required packets
     */
    for (xd->xd_cur_cnt = 0; xd->xd_cur_cnt < xd->xd_tot_cnt; xd->xd_cur_cnt++)
    {

        if (xd->xd_state != XD_RUNNING)
        {
            break;
        }

        /*
         * Generate pattern on first time through, or every time if
         * pattern is incrementing.
         */

        if (xd->xd_mac_dst_inc != 0)
        {
            ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
        }
        if (xd->xd_mac_src_inc != 0)
        {
            ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);
        }

        /*
         * Store pattern
         */
        if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pat_random)
        {
            packet_random_store(payload, payload_len);
        }
        else if (!XD_FILE(xd) && !XD_STRING(xd) && (xd->xd_cur_cnt == 0 || xd->xd_pat_inc != 0))
        {
            xd->xd_pat = packet_store(payload, payload_len, xd->xd_pat, xd->xd_pat_inc);
        }

        /*
         * Put packet to member port of tx_pbmp one by one via ITMH header
         */
        BCM_PBMP_ITER(pkt_info->tx_pbmp, port_ndx)
        {
            /*
             * Bypass ILKN ports, SFI ports
             */
            if (IS_IL_PORT(xd->xd_tx_unit, port_ndx) || IS_SFI_PORT(xd->xd_tx_unit, port_ndx))
            {
                continue;
            }

            /*
             * Get the system port from logical port as destination port
             */
            rv = dnx_algo_gpm_gport_phy_info_get
                (xd->xd_tx_unit, port_ndx, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
            if (BCM_E_NONE != rv)
            {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META("TX failed to get system port %s\n"), bcm_errmsg(rv)));
                return (CMD_FAIL);
            }
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, gport_info.sys_port);

            /*
             * Set destination port into ITMH header
             */
            diag_dnx_tx_itmh_dest_port_set(xd->xd_tx_unit,
                                           pkt_info->pkt_data[0].data + DNX_MODULE_HEADER_SIZE + TX_DNX_PTCH_TYPE2_LEN,
                                           BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport), system_headers_mode);

            if (BCM_PBMP_MEMBER(pkt_info->tx_upbmp, port_ndx))
            {
                /*
                 * Strip VLAN tag for member of tx_upbmp
                 */
                diag_dnx_tx_tag_strip(xd->xd_tx_unit, pkt_info->pkt_data[0].data + header_size);
            }
            rv = bcm_tx(xd->xd_tx_unit, pkt_info, NULL);
            if (BCM_PBMP_MEMBER(pkt_info->tx_upbmp, port_ndx))
            {
                /*
                 * add VLAN tag back after packet transmisstion for member of tx_upbmp
                 */
                diag_dnx_tx_tag_recover(xd->xd_tx_unit, pkt_info->pkt_data[0].data + header_size);
            }
            if (rv != BCM_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("bcm_tx failed: TX Unit %d: %s\n"), xd->xd_tx_unit, bcm_errmsg(rv)));
                break;
            }
        }
        if (FALSE == bitmap_not_null)
        {
            if ((rv = bcm_tx(xd->xd_tx_unit, pkt_info, NULL)) != BCM_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("bcm_tx failed: TX Unit %d: %s\n"), xd->xd_tx_unit, bcm_errmsg(rv)));
                break;
            }
        }

        increment_macaddr(xd->xd_mac_dst, xd->xd_mac_dst_inc);
        increment_macaddr(xd->xd_mac_src, xd->xd_mac_src_inc);

        /*
         * Per port source mac
         */
        if (xd->xd_ppsm)
        {
            /*
             * change xd_pbm to next port. reset src mac if at base port 
             */
            if (diag_dnx_tx_next_port(xd->xd_tx_unit, &pkt_info->tx_pbmp, &xd->xd_ppsm_pbm))
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META("resetting mac\n")));
                ENET_COPY_MACADDR(xd->xd_mac_src_base, xd->xd_mac_src);
            }
        }
    }

    /*
     * Replace original params 
     */
    if (xd->xd_ppsm)
    {
        ENET_COPY_MACADDR(xd->xd_mac_src_base, xd->xd_mac_src);
        pkt_info->tx_pbmp = xd->xd_ppsm_pbm;
    }

    return (rv == BCM_E_NONE ? CMD_OK : CMD_FAIL);
}

/**
 * \brief - Process a tx command from the bcm shell
 *
 * \param [in] u - Relevant unit
 * \param [in] a - Pointer to struct with command content
 *
 * \return
 *   cmd_result_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
cmd_result_t
cmd_dnx_tx(
    int u,
    args_t * a)
{
    xd_t *xd;
    volatile cmd_result_t rv;
    jmp_buf ctrl_c;

    if (!sh_check_attached(ARG_CMD(a), u))
    {
        return (CMD_FAIL);
    }

    /*
     * Init the tx/rx struct 
     */
    _XD_INIT(u, xd);
    if (xd->xd_state == XD_RUNNING)
    {
        cli_out("%s: Error: already active\n", ARG_CMD(a));
        return (CMD_FAIL);
    }

    /*
     * Parse the argument line 
     */
    if (CMD_OK != (rv = diag_dnx_tx_parse(u, a, xd)))
    {
        return (rv);
    }

#ifndef NO_CTRL_C
    if (!setjmp(ctrl_c))
    {
#endif
        sh_push_ctrl_c(&ctrl_c);
        xd->xd_state = XD_RUNNING;
        /*
         * Perform the TX operation 
         */
        rv = diag_dnx_do_tx(xd);
#ifndef NO_CTRL_C
    }
    else
    {
        rv = CMD_INTR;
    }
#endif

    sh_pop_ctrl_c();
    xd->xd_state = XD_IDLE;

    return (rv);
}
