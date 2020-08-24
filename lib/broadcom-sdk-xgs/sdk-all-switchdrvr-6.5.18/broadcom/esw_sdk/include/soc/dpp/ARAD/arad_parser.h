/* $Id: arad_parser.h,v 1.28 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __ARAD_PARSER_INCLUDED__
/* { */
#define __ARAD_PARSER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define ARAD_PARSER_NOF_PARSER_PROGRAMS_REPLICATIONS       (3)

#define ARAD_ITMH_DEST_ISQ_FLOW_ID_PREFIX                  (0x4) /*3b'100*/
#define ARAD_ITMH_DEST_OUT_LIF_ID_PREFIX                   (0x5) /*3b'101*/
#define ARAD_ITMH_DEST_MC_FLOW_ID_PREFIX                   (0xD) /*4b'1101*/

/* User defined Next Protocol values for Ipv4 and IPv6 Macros */
#define DPP_PARSER_IP_NEXT_PROTOCOL_UDP_L2TP            6

/* Hard-coded Next Protocol values for Ipv4 and IPv6 Macros */
#define DPP_PARSER_IP_NEXT_PROTOCOL_TCP                 8
#define DPP_PARSER_IP_NEXT_PROTOCOL_UDP                 9
#define DPP_PARSER_IP_NEXT_PROTOCOL_IGMP                10
#define DPP_PARSER_IP_NEXT_PROTOCOL_ICMP                11
#define DPP_PARSER_IP_NEXT_PROTOCOL_ICMPv6              12
#define DPP_PARSER_IP_NEXT_PROTOCOL_IPv4                13
#define DPP_PARSER_IP_NEXT_PROTOCOL_IPv6                14
#define DPP_PARSER_IP_NEXT_PROTOCOL_MPLS                15

/* Hard-coded Next Protocol values for ETH Macro */
#define DPP_PARSER_ETH_NEXT_PROTOCOL_TRILL              8
#define DPP_PARSER_ETH_NEXT_PROTOCOL_MinM               9
#define DPP_PARSER_ETH_NEXT_PROTOCOL_ARP                10
#define DPP_PARSER_ETH_NEXT_PROTOCOL_CFM                11
#define DPP_PARSER_ETH_NEXT_PROTOCOL_FCoE               12
#define DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4               13
#define DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6               14
#define DPP_PARSER_ETH_NEXT_PROTOCOL_MPLS               15

/* Number of Next Protocol values for Eth, IPv4 and IPv6 Macros */
#define DPP_PARSER_NEXT_PROTOCOL_NOF                    16

#define ARAD_PARSER_CUSTOM_MACRO_PROTO_1                   (0x0) /* Ingres shaping - start */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_2                   (0x1) /* Ingres shaping - end */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_3                   (0x2) /* OutLIF - start */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_4                   (0x3) /* OutLIF - end */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_5                   (0x4) /* MC-Flow-ID - start */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_6                   (0x5) /* MC-Flow-ID - end */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_7                   (0x6) /* BFD single hop */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_8                   (0x7) /* VXLAN */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_9                   (0x8) /* micro BFD : Used in Jericho only*/
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_10                  (0x9) /* PTP (1588) - udp-319 or UDP_Tunnel Dummy */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_11                  (0xa) /* PTP (1588) - udp-320 */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_12                  (0xb) /* GTP store value 4 */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_13                  (0xc) /* UDP Tunnel IPv4 dst port or GTP store value 6 */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_14                  (0xd) /* FCoE VFT or UDP_Tunnel IPv6 dst port or GTP port 2152 */
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_15                  (0xe) /* FCoE encap or UDP_Tunnel MPLS dst port or GTP port 2123 or BFD multi hop*/

#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_5_6           ((ARAD_PARSER_CUSTOM_MACRO_PROTO_5 + 1) << 1)  + 1
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_7_8           ((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1)  + 1
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_9_10          ((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1)  + 1
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_AFT_15        ((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1) + 1

/* IHP_PARSER_CUSTOM_MACRO_PROTOCOLS table size */
#define ARAD_PARSER_CUSTOM_PROTO_NOF_ENTRIES                    15

#define ARAD_PARSER_PROG_RAW_ADDR_START                          (0x0000)
#define ARAD_PARSER_PROG_ETH_ADDR_START                          (0x0001)
#define ARAD_PARSER_PROG_TM_ADDR_START                           (0x0002)
#define ARAD_PARSER_PROG_PPH_TM_ADDR_START                       (0x0003)
#define ARAD_PARSER_PROG_FTMH_ADDR_START                         (0x0004)
#define ARAD_PARSER_PROG_RAW_MPLS_ADDR_START                     (0x0005)

/* Dummy DST port for UDP_tunnel, bigger than any legal UDP DST port */
#define ARAD_PP_UDP_DUMMY_DST_PORT      (0x10000)

 /* Base TM custom macro - base set per PP-Port */
#define ARAD_PARSER_MACRO_SEL_TM_BASE                      (0x0004)

/* IS_PROTO flags per PFC */
#define IS_ETH      0x01
#define IS_IPV4     0x02
#define IS_IPV6     0x04
#define IS_MPLS     0x08
#define IS_TRILL    0x10
#define IS_UD       0x20

typedef enum
{
    /*
     * Mark places where PLC has no meaning - not end leafs in parser chain
     */
    DPP_PLC_NA              = -1,
    /*
     * Used in places that theoretically can be reached but PLC assignment has no meaning, used only for SW sake to mark such place
     */
    DPP_PLC_FIRST, /* =0x00 */
    DPP_PLC_DONTCARE = DPP_PLC_FIRST, /* =0x00 */
    DPP_PLC_TM, /* =0x01 */
    DPP_PLC_TM_IS, /* =0x02 */
    DPP_PLC_TM_MC_FLOW, /* =0x03 */
    DPP_PLC_TM_OUT_LIF, /* =0x04 */
    DPP_PLC_RAW, /* =0x05 */
    DPP_PLC_FTMH, /* =0x06 */
    DPP_PLC_VXLAN, /* =0x07 */
    DPP_PLC_BFD_SINGLE_HOP, /* =0x08 */
    DPP_PLC_IP_UDP_GTP1, /* =0x09 */
    DPP_PLC_IP_UDP_GTP2, /* =0x0a */
    DPP_PLC_PP_L4, /* =0x0b */
    DPP_PLC_PP, /* =0x0c */
    DPP_PLC_MPLS_5, /* =0x0d */
    DPP_PLC_GAL_GACH_BFD, /* =0x0e */
    DPP_PLC_FCOE, /* =0x0f */
    DPP_PLC_FCOE_VFT, /* =0x10 */
    /*
     * From this place PLC groups start
     */
    DPP_PLC_ANY, /* =0x11 */
    DPP_PLC_ANY_PP, /* =0x12 */
    DPP_PLC_ANY_PP_AND_BFD, /* =0x13 */
    DPP_PLC_PP_AND_PP_L4, /* =0x14 */
    DPP_PLC_ANY_FCOE, /* =0x15 */
    /*
     * From this place New PLC - start
     */
    /*
     * PLC that together with PFC MPLSxN replace PFC IPv4_MPLSxN
     */
    DPP_PLC_IPv4isH3, /* =0x16 */
    DPP_PLC_IPv6isH3, /* =0x17 */
    DPP_PLC_IPv4isH4, /* =0x18 */
    DPP_PLC_IPv6isH4, /* =0x19 */
    DPP_PLC_IPv4isH5, /* =0x1a */
    DPP_PLC_IPv6isH5, /* =0x1b */
    DPP_PLC_L4_IPv4isH3, /* =0x1c */
    DPP_PLC_L4_IPv6isH3, /* =0x1d */
    DPP_PLC_L4_IPv4isH4, /* =0x1e */
    DPP_PLC_L4_IPv6isH4, /* =0x1f */
    DPP_PLC_L4_IPv4isH5, /* =0x20 */
    DPP_PLC_L4_IPv6isH5, /* =0x21 */
    DPP_PLC_ETHisH3, /* =0x22 */
    DPP_PLC_ETHisH4, /* =0x23 */
    /*
     * New Group PLC
     */
    /* Group uniting DPP_PLC_IPV4isH3 & DPP_PLC_IPV4isH4 */
    DPP_PLC_IPv4isHAny, /* =0x24 */
    /* Group uniting DPP_PLC_IPV6isH3 & DPP_PLC_IPV6isH4 */
    DPP_PLC_IPv6isHAny, /* =0x25 */
    /* Group uniting DPP_PLC_IPV4isH3 & DPP_PLC_L4_IPV4isH3 */
    DPP_PLC_Any_IPv4isH3, /* =0x26 */
    /* Group uniting DPP_PLC_IPV6isH3 & DPP_PLC_L4_IPV6isH3 */
    DPP_PLC_Any_IPv6isH3, /* =0x27 */

    /* Group uniting DPP_PLC_IPV4isH4 & DPP_PLC_L4_IPV4isH4 */
    DPP_PLC_Any_IPv4isH4, /* =0x28 */
    /* Group uniting DPP_PLC_IPV6isH4 & DPP_PLC_L4_IPV6isH4 */
    DPP_PLC_Any_IPv6isH4, /* =0x29 */
    /* Group uniting DPP_PLC_IPV4isH5 & DPP_PLC_L4_IPV4isH5 */
    DPP_PLC_Any_IPv4isH5, /* =0x2a */
    /* Group uniting DPP_PLC_IPV6isH5 & DPP_PLC_L4_IPV6isH5 */
    DPP_PLC_Any_IPv6isH5, /* =0x2b */
    /* Group uniting DPP_PLC_ETHisH3 & DPP_PLC_ETHisH4 */
    DPP_PLC_EthisHAny, /* =0x2c */
    DPP_PLC_NOF /* =0x2d */
} DPP_PLC_E;

typedef enum
{
    DPP_PLC_PROFILE_NA    = -1,
    DPP_PLC_PROFILE_FIRST =  0,
    DPP_PLC_PROFILE_TM    = DPP_PLC_PROFILE_FIRST,
    DPP_PLC_PROFILE_TM_IS,
    DPP_PLC_PROFILE_TM_MC_FLOW,
    DPP_PLC_PROFILE_TM_OUT_LIF,
    DPP_PLC_PROFILE_RAW_FTMH,
    DPP_PLC_PROFILE_PP,
    DPP_PLC_PROFILE_MPLS,
    DPP_PLC_PROFILE_MPLS_EXT,
    DPP_PLC_PROFILE_FCOE,
    DPP_PLC_PROFILE_GLOBAL,
    DPP_PLC_PROFILE_NOF
} DPP_PLC_PROFILE_E;

/* ParserLeafContext */
#define ARAD_PARSER_PLC_TM                     (0x0)
#define ARAD_PARSER_PLC_RAW                    (0x7)
#define ARAD_PARSER_PLC_FTMH                   (0x8)
#define ARAD_PARSER_PLC_TM_OUT_LIF             (0xC)
#define ARAD_PARSER_PLC_TM_MC_FLOW             (0xD)
#define ARAD_PARSER_PLC_TM_IS                  (0xD)

#define ARAD_PARSER_PLC_FCOE                   (0x4) /* for both encap and not encap when VFT not exist */
#define ARAD_PARSER_PLC_FCOE_VFT               (0x5) /* for both encap and not encap when VFT exist */

#define ARAD_PARSER_PLC_VXLAN                  (0x1)
#define ARAD_PARSER_PLC_BFD_SINGLE_HOP         (0xa)
#define ARAD_PARSER_PLC_IP_UDP_GTP1            (0xC)
#define ARAD_PARSER_PLC_IP_UDP_GTP2            (0xD)
#define ARAD_PARSER_PLC_PP_L4                  (0xE) /* The _L4 leaf context was created to separate the handling
                                                        of IP packets with and without L4 headers. Until this was
                                                        added, TCP/UDP packets were also parsed the same as IP
                                                        packets with no layer 4, which caused wrong load-balancing. */
#define ARAD_PARSER_PLC_PP                     (0xF)

#define ARAD_PARSER_PLC_MPLS_5                 (0xC)    /* At least 5 MPLS labels, to be used only when PFC is ARAD_PARSER_PFC_MPLS3_ETH */
#define ARAD_PARSER_PLC_GAL_GACH_BFD           (0xD)

#define ARAD_PARSER_PLC_NOF                    (16)
#define ARAD_PARSER_PLC_MAX                    (ARAD_PARSER_PLC_NOF - 1)

#define ARAD_PARSER_PLC_MATCH_ONE              0x0
#define ARAD_PARSER_PLC_MATCH_ANY_FCOE         0x1
#define ARAD_PARSER_PLC_MATCH_PP_AND_PP_L4     0x1
/* ignore 2 LSBs because 4 higher Parser-Leaf-Contexts are considered regular PP */
#define ARAD_PARSER_PLC_MATCH_ANY_PP           0x3
#define ARAD_PARSER_PLC_MATCH_ANY_PP_BFD       0x7
#define ARAD_PARSER_PLC_MATCH_ANY              0xF

/* This Ethertype is used as WA for additional TPID in single-tag packets.
 * It is read from Outer Tag field if there is no match to port tag.
 */
/* IHP_PARSER_ETHER_PROTOCOLS table size */
#define ARAD_PARSER_ETHER_PROTO_NOF_ENTRIES                    7

#define ARAD_PARSER_ETHER_PROTO_2_PPPoES                   (0x1)
#define ARAD_PARSER_ETHER_PROTO_2_PPPoES_ETHER_TYPE        (0x8864) /* PPPoE Session Stage */
#define ARAD_PARSER_ETHER_PROTO_5_PPPoES_CONTROL           (0x5) /* PPPoE Session Control packets */


#define ARAD_PARSER_ETHER_PROTO_4_ADD_TPID                 (0x3)
#define ARAD_PARSER_ETHER_PROTO_4_ADD_TPID_TYPE            (0x88a8) /* Additional TPID */
 
#define ARAD_PARSER_ETHER_PROTO_5_EoE                      (0x4)
#define ARAD_PARSER_ETHER_PROTO_5_EoE_TPID_TYPE            (0xE0E0)

#define ARAD_PARSER_ETHER_PROTO_3_EoE                      (0x2)
#define ARAD_PARSER_ETHER_PROTO_3_EoE_TPID_TYPE            (0xE0EC)


/* Coupling */
#define ARAD_PARSER_ETHER_PROTO_6_1588                     (0x5) 
#define ARAD_PARSER_ETHER_PROTO_6_1588_ETHER_TYPE          (0x88f7) /* 1588 */ 
#define ARAD_PARSER_ETHER_PROTO_7_MPLS_MC                  (0x6) 
#define ARAD_PARSER_ETHER_PROTO_7_MPLS_MC_ETHER_TYPE       (0x8848) /* MPLS multicast */ 


/* IHP_PARSER_IP_PROTOCOLS table size */
#define ARAD_PARSER_IP_PROTO_NOF_ENTRIES                    7

/* IPv6 Extension headers - Protocol values taken from:
 * http://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml
 */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_HOP_BY_HOP            0       /* Hop-by-Hop (0) */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_DEST_OPTIONS          60      /* Destination-Options (60) */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_ROUTING               43      /* Routing (43) */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_FRAGMENT              44      /* Fragment (44) */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_MOBILITY              135     /* Mobility (135) */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_HIP                   139     /* Host-Identity-Protocol (HIP) (139) */
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_SHIM6                 140     /* Shim6 (140) */

typedef enum
{
  /*
   * Used to sign placed that will never be last in parser tree
   */
  DPP_PFC_NA             = -1,
  /*
   * Used in places that theoretically can be reached but PFC assignment has no meaning, used only for SW sake to mark such place
   */
  DPP_PFC_FIRST, /*=0x00 */
  /*
   *  Ethernet
   */
  DPP_PFC_ETH = DPP_PFC_FIRST, /* =0x00 */
  /*
   *  MAC-in-MAC
   */
  DPP_PFC_ETH_ETH, /* =0x01 */
  /*
   *  IPv4 over Ethernet
   */
  DPP_PFC_IPV4_ETH, /* =0x02 */
  /*
   *  IPv6 over Ethernet
   */
  DPP_PFC_IPV6_ETH, /* =0x03 */
  /*
   *  1 MPLS tag over Ethernet
   */
  DPP_PFC_MPLS1_ETH, /* =0x04 */
  /*
   *  MPLS x 2 over Ethernet
   */
  DPP_PFC_MPLS2_ETH, /* =0x05 */
  /*
   *  MPLS x 3 over Ethernet
   */
  DPP_PFC_MPLS3_ETH, /* =0x06 */
  /*
   *  FC standard over Ethernet
   */
  DPP_PFC_FCOE_STD_ETH, /* =0x07 */
  /*
   *  FC with Encap over Ethernet
   */
  DPP_PFC_FCOE_ENCAP_ETH, /* =0x08 */
  /*
   *  Ethernet over IP over Ethernet
   */
  DPP_PFC_ETH_IPV4_ETH, /* =0x09 */
  /*
   *  Ethernet over TRILL over Ethernet
   */
  DPP_PFC_ETH_TRILL_ETH, /* =0x0a */
  /*
   *  Ethernet over MPLS over Ethernet
   */
  DPP_PFC_ETH_MPLS1_ETH, /* =0x0b */
  /*
   *  Ethernet over MPLSx2 over Ethernet
   */
  DPP_PFC_ETH_MPLS2_ETH, /* =0x0c */
  /*
   *  Ethernet over MPLSx3 over Ethernet
   */
  DPP_PFC_ETH_MPLS3_ETH, /* =0x0d */
  /*
   *  IPv4 over IPv4 over Ethernet
   */
  DPP_PFC_IPV4_IPV4_ETH, /* =0x0e */
  /*
   *  IPv4 over IPv6 over Ethernet
   */
  DPP_PFC_IPV4_IPV6_ETH, /* =0x0f */
  /*
   *  IPv4 over MPLS over Ethernet
   */
  DPP_PFC_IPV4_MPLS1_ETH, /* =0x10 */
  /*
   *  IPv4 over MPLSx2 over Ethernet
   */
  DPP_PFC_IPV4_MPLS2_ETH, /* =0x11 */
  /*
   *  IPv4 over MPLSx3 over Ethernet
   */
  DPP_PFC_IPV4_MPLS3_ETH, /* =0x12 */
  /*
   *  IPv6 over IPv4 over Ethernet
   */
  DPP_PFC_IPV6_IPV4_ETH, /* =0x13 */
  /*
   *  IPv6 over IPv6 over Ethernet
   */
  DPP_PFC_IPV6_IPV6_ETH, /* =0x14 */
  /*
   *  IPv6 over MPLS over Ethernet
   */
  DPP_PFC_IPV6_MPLS1_ETH, /* =0x15 */
  /*
   *  IPv6 over MPLSx2 over Ethernet
   */
  DPP_PFC_IPV6_MPLS2_ETH, /* =0x16 */
  /*
   *  IPv6 over MPLSx3 over Ethernet
   */
  DPP_PFC_IPV6_MPLS3_ETH, /* =0x17 */
  /*
   * Packet format code - non Ethernet
   */
  DPP_PFC_RAW_AND_FTMH, /* =0x18 */
  /*
   * TM without extensions, identical to Raw - The PMF program selection is per PP-port profile
   */
  DPP_PFC_TM, /* =0x19 */
  /*
   * TM with IS
   */
  DPP_PFC_TM_IS, /* =0x1a */
  /*
   * TM with MC-FLOW
   */
  DPP_PFC_TM_MC_FLOW, /* =0x1b */
  /*
   * TM with OUTLIF
   */
  DPP_PFC_TM_OUT_LIF, /* =0x1c */
  DPP_PFC_LAST_OLD = DPP_PFC_TM_OUT_LIF, /* =0x1c */
  /*
   * From this place PFC groups start
   */
  DPP_PFC_ANY, /* =0x1d */
  /*
   * Group for all NON-TM PFCs
   */
  DPP_PFC_ANY_ETH, /* =0x1e */
  /*
   * Group for all TM PFCs
   */
  DPP_PFC_ANY_TM, /* =0x1f */
  /*
   * Group for all PFCs that have IP-ETH tunnel either v4 or v6
   */
  DPP_PFC_ANY_IP, /* =0x20 */
  /*
   * Group for all PFCs that have IPv4-ETH tunnel
   */
  DPP_PFC_ANY_IPV4, /* =0x21 */
  /*
   * Group for all PFCs that have IPv6-ETH tunnel
   */
  DPP_PFC_ANY_IPV6, /* =0x22 */
  /*
   * Group for all PFCs that have tunnel with 1 MPLS label
   */
  DPP_PFC_ANY_MPLS1, /* =0x23 */
  /*
   * Group for all PFCs that have tunnel with 2 MPLS labels
   */
  DPP_PFC_ANY_MPLS2, /* =0x24 */
  /*
   * Group for all PFCs that have tunnel with 3 MPLS labels
   */
  DPP_PFC_ANY_MPLS3, /* =0x25 */
  /*
   * Group for all PFCs that have tunnel any number of MPLS labels
   */
  DPP_PFC_ANY_MPLS, /* =0x26 */
  /*
   * Group for all PFCs for Fiber Channel over Ethernet
   */
  DPP_PFC_ANY_FCOE, /* =0x27 */
  /*
   * New PFC
   */
  /*
   *  MPLS x 4 and more over Ethernet
   */
  DPP_PFC_MPLS4P_ETH, /* =0x28 */
  /*
   *  Ethernet over IP over Ethernet
   */
  DPP_PFC_ETH_IPV6_ETH, /* =0x29 */

  /* PFC For secondary parser only, mapped from PLC with the same name */
  DPP_PFC_IPV4isH3, /* =0x2a */
  DPP_PFC_IPV6isH3, /* =0x2b */
  DPP_PFC_L4_IPV4isH3, /* =0x2c */
  DPP_PFC_L4_IPV6isH3, /* =0x2d */
  DPP_PFC_ETHisH3, /* =0x2e */
  DPP_PFC_IPV4isH4_ETHisH3, /* =0x2f */
  DPP_PFC_IPV6isH4_ETHisH3, /* =0x30 */
  /*
   * Group for all Secondary PFCs that have tunnel any number of MPLS labels
   */
  DPP_PFC_ANY_EXT_MPLS, /* =0x31 */
  DPP_PFC_NOF /* =0x32 */
} DPP_PFC_E;

typedef enum {
    /*
     * Type for single PFC generated by programmable parser
     */
    DPP_PLC_TYPE_PRIMARY    = 0,
    /*
     * Type for PFC groups - not to be taken into account when initializing HW
     */
    DPP_PLC_TYPE_GROUP      = 2
} DPP_PLC_TYPE_E;

typedef struct
{
    DPP_PLC_E       sw;
    char*           name;
    int             hw;
    int             mask;
    DPP_PLC_TYPE_E  type;
    int             lb_set;
}  dpp_parser_plc_info_t;

typedef struct
{
    DPP_PLC_PROFILE_E       id;
    char*                   name;
    dpp_parser_plc_info_t*  plc_info;
    int                     nof_plc;
}  dpp_parser_plc_profile_t;

typedef enum {
    /*
     * Type for single PFC generated by programmable parser
     */
    DPP_PFC_TYPE_PRIMARY    = 0,
    /*
     * Type for single PFC generated by secondary parser
     */
    DPP_PFC_TYPE_SECONDARY  = 1,
    /*
     * Type for PFC groups - not to be taken into account when initializing HW
     */
    DPP_PFC_TYPE_GROUP      = 2
} DPP_PFC_TYPE_E;

typedef struct
{
    DPP_PFC_E   sw;
    char*       name;
    uint32      hw;
    uint32      mask;
    int         is_proto;
    int         l4_location;
    int         type;
    uint32      hdr_type_map;
    uint32      vtt;
    uint32      pmf;
    DPP_PLC_PROFILE_E plc_profile_id;
    DPP_PLC_PROFILE_E jer_plc_profile_id;
}  dpp_parser_pfc_info_t;

extern dpp_parser_pfc_info_t dpp_parser_pfc_info[];
extern dpp_parser_plc_profile_t dpp_parser_plc_profiles[];
extern int dpp_parser_plc_profile_nof;

extern char *parser_segment_name[];

#define ARAD_PARSER_PFC_NOF_BITS            6

#define ARAD_PARSER_PFC_NOF                 (1 << ARAD_PARSER_PFC_NOF_BITS)

typedef struct {
    DPP_PFC_E pfc_map[ARAD_PARSER_PFC_NOF];
} dpp_parser_info_t;

#define ARAD_PARSER_PFC_ETH                 0x00
#define ARAD_PARSER_PFC_ETH_ETH             0x01
#define ARAD_PARSER_PFC_IPV4_ETH            0x02
#define ARAD_PARSER_PFC_IPV6_ETH            0x03
#define ARAD_PARSER_PFC_MPLS4P_ETH          0x04
#define ARAD_PARSER_PFC_MPLS1_ETH           0x05
#define ARAD_PARSER_PFC_MPLS2_ETH           0x06
#define ARAD_PARSER_PFC_MPLS3_ETH           0x07
#define ARAD_PARSER_PFC_FCOE_STD_ETH        0x08
#define ARAD_PARSER_PFC_PPPOE_ETH	        0x08 /* overloaded with FCOE */
#define ARAD_PARSER_PFC_FCOE_ENCAP_ETH      0x09
#define ARAD_PARSER_PFC_ETH_IPV4_ETH        0x0A
#define ARAD_PARSER_PFC_ETH_IPV6_ETH        0x0B
#define ARAD_PARSER_PFC_ETH_TRILL_ETH       0x0C
#define ARAD_PARSER_PFC_ETH_MPLS1_ETH       0x0D
#define ARAD_PARSER_PFC_ETH_MPLS2_ETH       0x0E
#define ARAD_PARSER_PFC_ETH_MPLS3_ETH       0x0F
#define ARAD_PARSER_PFC_IPV4_IPV4_ETH       0x12
#define ARAD_PARSER_PFC_IPV4_IPV6_ETH       0x13
#define ARAD_PARSER_PFC_IPV4_MPLS1_ETH      0x15
#define ARAD_PARSER_PFC_IPV4_MPLS2_ETH      0x16
#define ARAD_PARSER_PFC_IPV4_MPLS3_ETH      0x17
#define ARAD_PARSER_PFC_IPV6_IPV4_ETH       0x1A
#define ARAD_PARSER_PFC_IPV6_IPV6_ETH       0x1B
#define ARAD_PARSER_PFC_IPV6_MPLS1_ETH      0x1D
#define ARAD_PARSER_PFC_IPV6_MPLS2_ETH      0x1E
#define ARAD_PARSER_PFC_IPV6_MPLS3_ETH      0x1F
#define ARAD_PARSER_PFC_RAW_AND_FTMH        0x20 /* LSB must be 0 to avoid conflict with TM */
#define ARAD_PARSER_PFC_TM                  0x30 /* TM without extensions, identical to Raw - The PMF program selection is per PP-port profile */
#define ARAD_PARSER_PFC_TM_IS               0x31 /* TM with IS */
#define ARAD_PARSER_PFC_TM_MC_FLOW          0x3A /* TM with MC-FLOW */
#define ARAD_PARSER_PFC_TM_OUT_LIF          0x3C /* TM with OUTLIF */

/* Parser mode = 1 PFCs */
#define ARAD_PARSER_1_PFC_ETH_MPLS_ETH ARAD_PARSER_PFC_IPV4_MPLS2_ETH

/* MASKS for program selections choices 0 - bit is meaningful, 1 don't care */
#define ARAD_PARSER_PFC_MATCH_ONE                        0x00
#define ARAD_PARSER_PFC_MATCH_ANY_FCOE                   0x01
#define ARAD_PARSER_PFC_MATCH_IPVx                       0x01
#define ARAD_PARSER_PFC_MATCH_MPLS_ENCAP_IPVx            0x08
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP                  0x18
#define ARAD_PARSER_PFC_MATCH_ANY_MPLS                   0x1B
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_IP           0x19
#define ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2             0x19
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE         0x38
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP  0x39
#define ARAD_PARSER_PFC_MATCH_TYPE                       0x1F
#define ARAD_PARSER_PFC_MATCH_ANY                        0x3F

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
   /*
    * IPv4 over UDP tunnel
    */
   ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_IPV4 = 0,
    /*
    * IPv6 over UDP tunnel
    */
   ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_IPV6 = 1,
   /*
    * MPLS over UDP tunnel
    */
   ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_MPLS = 2
}ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */
/* Return pointer to string describing MACRO */
char *arad_parser_get_macro_str(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 macro_sel
  );

/*
 * Init the PFC info table:
 * indicate the location of the L4 header according to the Packet Format Code
 * Init all the values according to dpp_parser_pfc_info table in arad_parser.c
 * Header-4 (e.g., for IPoMPLSoEth). Exceptions:
 * Header-3 for IPoEth, but not IPoIPoEth
 */
uint32
dpp_parser_pfc_l4_location_init(int unit);

/*
 * Initialize VTT, PMF ACL and PS mapping per pfc
 */
uint32
dpp_parser_pfc_map_init(int unit);

dpp_parser_pfc_info_t *parser_pfc_info_get_by_sw(
    int unit,
    DPP_PFC_E pfc_sw);

/*
 * Return hw pfc generated by parser
 */
soc_error_t dpp_parser_pfc_get_hw_by_sw(
    int       unit,
    DPP_PFC_E pfc_sw,
    uint32 *pfc_hw_p,
    uint32 *pfc_hw_mask_p);

/*
 * Return SW PFC based on pfc_map
 */
soc_error_t dpp_parser_pfc_get_sw_by_hw(
        int unit,
        uint32 pfc_hw,
        DPP_PFC_E *pfc_sw_p);

/*
 * Activate predefined HW PFC for specific SW PFC
 */
soc_error_t dpp_parser_pfc_activate_hw_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_hw_p);

/*
 * Return vtt pfc generated by parser
 */
soc_error_t dpp_parser_pfc_get_vtt_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_vtt_p,
    uint32*     pfc_vtt_mask_p);

/*
 * Given sw pfc and mask return pfc and mask mapped for ACL
 */
soc_error_t dpp_parser_pfc_get_acl_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_pmf_p,
    uint32*     pfc_pmf_mask_p);

/*
 * Given acl pfc and mask return sw pfc and mask
 */
soc_error_t dpp_parser_pfc_get_sw_by_acl(
    int         unit,
    uint32      pfc_pmf,
    uint32      pfc_pmf_mask,
    DPP_PFC_E*  pfc_sw_p);

/*
 * Given sw pfc and mask return pfc and mask mapped for pmf program selection
 */
soc_error_t dpp_parser_pfc_get_pmf_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_pmf_p,
    uint32*     pfc_pmf_mask_p);

/*
 * Given hw pfc return pointer to pfc name
 */
char *dpp_parser_pfc_string_by_hw(
    int         unit,
    int         pfc_hw);

/*
 * Given sw pfc return pointer to pfc name
 */
char *dpp_parser_pfc_string_by_sw(
    int unit,
    DPP_PFC_E   pfc_sw);

/*
 * Get HW PLC and HW PLC mask by SW PFC and SW PLC
 * If no match was found, return SOC_SAND_ERR
 */
soc_error_t dpp_parser_plc_hw_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    DPP_PLC_E   plc_sw,
    uint32*     plc_hw_p,
    uint32*     plc_hw_mask_p);

/*
 * Get ACL PLC and ACl PLC mask by SW PFC and SW_PLC
 * If no match was found, return SOC_SAND_ERR
 * When PFC is now known or not important use DPP_PFC_ANY
 */
soc_error_t dpp_parser_plc_acl_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    DPP_PLC_E   plc_sw,
    uint32*     plc_acl_p,
    uint32*     plc_acl_mask_p);

/*
 * Get SW PLC by SW PFC and ACL PLC and MASK
 * If no match was found, return SOC_SAND_ERR
 * When PFC is now known or not important use DPP_PFC_ANY
 */
soc_error_t dpp_parser_plc_sw_by_acl(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32      plc_acl,
    uint32      plc_acl_mask,
    DPP_PLC_E*  plc_sw_p);

/*
 * Given sw plc return pointer to plc name
 */
char *dpp_parser_plc_string_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    DPP_PLC_E   plc_sw);

char *dpp_parser_plc_string_by_hw(
    int         unit,
    int         pfc_hw,
    int         plc_hw);

char *dpp_parser_plc_profile_string_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw);

SOC_PPC_PKT_HDR_TYPE dpp_parser_pfc_hdr_type_at_index(
    int         pfc_hw,
    uint32      hdr_index);

uint32
  arad_parser_init(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_parser_ingress_shape_state_set(
     SOC_SAND_IN int                                 unit,
     SOC_SAND_IN uint8                                 enable,
     SOC_SAND_IN uint32                                  q_low,
     SOC_SAND_IN uint32                                  q_high
  );

int
  arad_parser_nof_bytes_to_remove_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core,
    SOC_SAND_IN uint32      tm_port,
    SOC_SAND_IN uint32      nof_bytes_to_skip
  );

/* use to update cfg attributes of the vxlan program, e.g. upd-dest port */
uint32
arad_parser_vxlan_program_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint16 udp_dest_port
 );

uint32
arad_parser_vxlan_program_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT uint16 *udp_dest_port
 );

int
  arad_parser_nof_bytes_to_remove_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core,
    SOC_SAND_IN uint32      tm_port,
    SOC_SAND_OUT uint32     *nof_bytes_to_skip
  );

uint32
  arad_parser_pp_port_nof_bytes_to_remove_set(
    SOC_SAND_IN int      unit,
    SOC_SAND_IN int      core,
    SOC_SAND_IN uint32      pp_port_ndx,
    SOC_SAND_IN uint32      nof_bytes_to_skip
  );

uint32
arad_parser_udp_tunnel_dst_port_set(
    SOC_SAND_IN int                                      unit,
    SOC_SAND_IN ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE   udp_dst_port_type,
    SOC_SAND_IN int                                      udp_dst_port_val
  );

uint32
arad_parser_udp_tunnel_dst_port_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE     udp_dst_port_type,
    SOC_SAND_OUT int                                        *udp_dst_port_val
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PARSER_INCLUDED__*/
#endif
