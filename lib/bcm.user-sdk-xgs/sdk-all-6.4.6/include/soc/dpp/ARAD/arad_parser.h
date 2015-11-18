/* $Id: arad_parser.h,v 1.28 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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

#define ARAD_PARSER_PROG_RAW_ADDR_START                          (0x0000)
#define ARAD_PARSER_PROG_ETH_ADDR_START                          (0x0001)
#define ARAD_PARSER_PROG_TM_ADDR_START                           (0x0002)
#define ARAD_PARSER_PROG_PPH_TM_ADDR_START                       (0x0003)
#define ARAD_PARSER_PROG_FTMH_ADDR_START                         (0x0004)
#define ARAD_PARSER_PROG_RAW_MPLS_ADDR_START                     (0x0005)

#define ARAD_PARSER_DEFAULT_KEY_PROGRAM_PROFILE                  (0)
#define ARAD_PARSER_INJECTED_KEY_PROGRAM_PROFILE                 (1)
#define ARAD_PARSER_NON_ETH_KEY_PROGRAM_PROFILE                  (2)

#define ARAD_PARSER_PARSER_PROGRAM_POINTER_RAW                               (ARAD_PARSER_PROG_RAW_ADDR_START)
#define ARAD_PARSER_PARSER_PROGRAM_POINTER_ETH                               (ARAD_PARSER_PROG_ETH_ADDR_START)
#define ARAD_PARSER_PARSER_PROGRAM_POINTER_ITMH                              (ARAD_PARSER_PROG_TM_ADDR_START)
#define ARAD_PARSER_PARSER_PROGRAM_POINTER_FTMH                              (ARAD_PARSER_PROG_FTMH_ADDR_START)
#define ARAD_PARSER_PARSER_PROGRAM_POINTER_RAW_MPLS                          (ARAD_PARSER_PROG_RAW_MPLS_ADDR_START)

/* Packet format code - non ethernet */
#define ARAD_PARSER_PFC_RAW_AND_FTMH                       (0x0020)/*6'b10_0000*/ /* LSB must be 0 to avoid conflict with TM */

/* Parser PFC encoding */
#define ARAD_PARSER_PFC_TM                                 (0x0030)/*6'b11_0000*/ /* TM without extensions, identical to Raw - 
                                                                                    The PMF program selection is per PP-port profile */
#define ARAD_PARSER_PFC_TM_MC_FLOW                         (0x003A)/*6'b11_1010*/ /* TM with MC-FLOW */
#define ARAD_PARSER_PFC_TM_IS                              (0x0031)/*6'b11_0001*/ /* TM with IS */
#define ARAD_PARSER_PFC_TM_OUT_LIF                         (0x003C)/*6'b11_1100*/ /* TM with OUTLIF */

/* ParserLeafContext */
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_TM                     (0x0)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_VXLAN                  (0x1)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_FCoE                   (0x4) /* for both encap and not encap when VFT not exist */
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_FCOE_VFT               (0x5) /* for both encap and not encap when VFT exist */
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_TM_MC_FLOW             (0xD)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_TM_IS                  (0xD)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_TM_OUT_LIF             (0xC)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_PP                     (0xf)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_GAL_GACH_BFD           (0xd)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_PP_L4                  (0xe) /* The _L4 leaf context was created to separate the handling 
                                                                        of IP packets with and without L4 headers. Until this was 
                                                                        added, TCP/UDP packets were also parsed the same as IP 
                                                                        packets with no layer 4, which caused wrong load-balancing. */ 
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_RAW                    (0x7)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_FTMH                   (0x8)
#define ARAD_PMF_PGM_SEL_PARSER_PROFILE_BFD_SINGLE_HOP		   (0x6)

#define ARAD_PMF_PGM_SEL_PARSER_NOF_PROFILE_PP                 (2)
#define ARAD_PMF_PGM_SEL_PARSER_NOF_PARSER_LEAF_CONTEXT_VALS   (16)


/* This ethertype is used as WA for additional TPID in single-tag packets.
 * It is read from Outer Tag field if there is no match to port tag.
 */
#define ARAD_PARSER_ETHER_PROTO_4_ADD_TPID                 (0x3) 
#define ARAD_PARSER_ETHER_PROTO_4_ADD_TPID_TYPE            (0x88a8) /* Additional TPID */ 

/* Coupling */
#define ARAD_PARSER_ETHER_PROTO_6_1588                     (0x5) 
#define ARAD_PARSER_ETHER_PROTO_6_1588_ETHER_TYPE          (0x88f7) /* 1588 */ 
#define ARAD_PARSER_ETHER_PROTO_7_MPLS_MC                  (0x6) 
#define ARAD_PARSER_ETHER_PROTO_7_MPLS_MC_ETHER_TYPE       (0x8848) /* MPLS multicast */ 


#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH                 SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH           
#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_ETH             SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_ETH       
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH            SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH      
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH            SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH     
#define ARAD_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH        SOC_TMC_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH     
#define ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH           SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH     
#define ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH           SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH     
#define ARAD_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH           SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH     
#define ARAD_PARSER_PKT_HDR_STK_TYPE_FC_STD_ETH          SOC_TMC_PARSER_PKT_HDR_STK_TYPE_FC_STD_ETH     
#define ARAD_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH        SOC_TMC_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV6_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV6_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH      SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH      SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH      SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV6_ETH       SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV6_ETH 
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH      SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH      SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH      SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH
#define ARAD_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH        SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH
#define ARAD_PARSER_NOF_PKT_HDR_STK_TYPES                SOC_TMC_PARSER_NOF_PKT_HDR_STK_TYPES          
typedef SOC_TMC_PARSER_PKT_HDR_STK_TYPE                    ARAD_PARSER_PKT_HDR_STK_TYPE;

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */


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

uint32 arad_parser_fcoe_programs_set(SOC_SAND_IN int unit);


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


uint32 arad_parser_trill_programs_set(SOC_SAND_IN int unit);

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


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PARSER_INCLUDED__*/
#endif


