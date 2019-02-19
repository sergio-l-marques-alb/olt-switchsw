/* $Id$
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
 * $
*/

#ifndef __ARAD_EGR_PROG_EDITOR_INCLUDED__
/* { */
#define __ARAD_EGR_PROG_EDITOR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/TMC/tmc_api_stack.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_EGR_PROG_EDITOR_CE_TBL_SIZE   (16)

#define ARAD_EGR_PROG_EDITOR_PRGE_MEMORY_NOF_DATA_ENTRIES  (256)

/* Define the size of the additional pre-pended header
 * that is used to interface with Ports extender line card.
 * The additional byte contains the OUT-PP-Port value 
 * A port is mapped based on custom feature soc property 
 * and is set into the out-port.prog_editor_value 
 * In this way we can add to different applicatio PRGE, if required, 
 * a set of instructions to pre-pend the additional byte. 
 */ 
#define ARAD_EGR_PROG_EDITOR_PORT_EXTENDER_HEADER_SIZE  (0x1)

/* lsb of data entry */
#define ARAD_PRGE_DATA_ENTRY_LSBS_ERSPAN           (2)
#define ARAD_PRGE_DATA_ENTRY_LSBS_RSPAN            (1)
#define ARAD_PRGE_DATA_ENTRY_LSBS_MPLS_SWAP        (2)
#define ARAD_PRGE_DATA_ENTRY_LSBS_IPV6_TUNNEL      (3)
#define ARAD_PRGE_DATA_ENTRY_LSBS_ROO_VXLAN        (2) /* prge data entry type  */
#define ARAD_PRGE_DATA_ENTRY_LSBS_PON_DSCP_RE      (2)


/* second encapsulation type encoding */
#define ARAD_PRGE_EPNI_ETPP_SCND_ENC_TYPE_NONE                     (0)
#define ARAD_PRGE_EPNI_ETPP_SCND_ENC_TYPE_IP                       (1)
#define ARAD_PRGE_EPNI_ETPP_SCND_ENC_TYPE_TRILL                    (2)
#define ARAD_PRGE_EPNI_ETPP_SCND_ENC_TYPE_MPLS                     (3)

/*Data memory indexes*/
#define ARAD_PRGE_DATA_MEMORY_ENTRY_L2_REMOTE_CPU           (0xff)

/* max instruction entries */
/* There are 42 entries, only 0-31 are used for program pointers. The rest are for jumps and are not dynamic */
#define ARAD_EGR_PROG_EDITOR_NOF_INSTRUCTION_ENTRIES  (32) 

#define ARAD_PRGE_PP_SELECT_VALUE_1_VMAC_DOWN_ADD    (1)
#define ARAD_PRGE_PP_SELECT_VALUE_1_VMAC_UP_ADD      (2)

/* Maximum number of LFEMs in a single table accross all device types */
#define ARAD_EGR_PRGE_MAX_NOF_LFEMS   (SOC_DPP_DEFS_MAX(EGR_PRGE_NOF_LFEMS))

/* Maximum no. of LFEM tables accross all device types*/
#define ARAD_EGR_PRGE_MAX_NOF_LFEM_TABLES   (SOC_DPP_DEFS_MAX(EGR_PRGE_NOF_LFEM_TABLES))

/* Maximum no. of instruction in a program accross all device types */
#define ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS     (SOC_DPP_DEFS_MAX(EGR_PRGE_NOF_PROGRAM_INSTRUCTIONS))

/* these defines denote upper and lower bits of ecn dm threshold
 * BASIC_SHIFT denotes the initial shift made in order to adjust 32 bits to nano scale (30 bits which 
 * symbolize the nano scale). 
 * MAJOR_SHIFT denotes the bit from which we take the adjusted threshold range 
 */
#define ARAD_PRGE_ECN_DM_NTP_LOWER_BIT     (13)
#define ARAD_PRGE_ECN_DM_NTP_UPPER_BIT     (28)
#define ARAD_PRGE_ECN_DM_NTP_BASIC_SHIFT   (2)
#define ARAD_PRGE_ECN_DM_NTP_MAJOR_SHIFT   (ARAD_PRGE_ECN_DM_NTP_LOWER_BIT - ARAD_PRGE_ECN_DM_NTP_BASIC_SHIFT)
#define ARAD_PRGE_ECN_DM_NTP_LOWER_BOUND   ((uint32)(1 << ((ARAD_PRGE_ECN_DM_NTP_LOWER_BIT) - (ARAD_PRGE_ECN_DM_NTP_BASIC_SHIFT))))
#define ARAD_PRGE_ECN_DM_NTP_UPPER_BOUND   ((uint32)(((1 << ((ARAD_PRGE_ECN_DM_NTP_UPPER_BIT) - (ARAD_PRGE_ECN_DM_NTP_BASIC_SHIFT))) - 1)&(~(ARAD_PRGE_ECN_DM_NTP_LOWER_BOUND-1))))
#define ARAD_PRGE_ECN_DM_NTP_MAX_RANGE     ((1 << ((ARAD_PRGE_ECN_DM_NTP_UPPER_BIT) - (ARAD_PRGE_ECN_DM_NTP_LOWER_BIT))) - 1)

#define ARAD_PP_EG_PROG_NOF_FEM(_unit)   (SOC_DPP_DEFS_GET((_unit),egr_prge_nof_lfems))
#define ARAD_PP_EG_PROG_NOF_LFEM_TABLES(_unit) (SOC_DPP_DEFS_GET((_unit),egr_prge_nof_lfem_tables))
#define ARAD_PP_EG_PROG_NOF_INSTR(_unit) (SOC_DPP_DEFS_GET((_unit),egr_prge_nof_program_instructions))
#define ARAD_PP_EG_PROG_NOF_INSTR_MEMS(_unit) (SOC_DPP_DEFS_GET((_unit),egr_prge_nof_instruction_mems))

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/*
 * Get the OTMH extension profile
 */
#define ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD(is_src, is_dest, cud_type) \
  (ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV + (cud_type) + ((is_dest) * 3) + ((is_src) * 6))

#define ARAD_EGR_PROG_TM_PORT_PROFILE_TM_CUD_PRESENT_GET(tm_port_profile, is_tm_cud_to_add)                                                                                     \
    if ((tm_port_profile - ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) % ARAD_PORTS_FTMH_NOF_EXT_OUTLIFS == 0)          \
    {                                                                                                                               \
      /* Never add CUD */                                                                                                           \
      is_tm_cud_to_add = FALSE;                                                                                                     \
    }                                                                                                                               \
    else if ((tm_port_profile - ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) % ARAD_PORTS_FTMH_NOF_EXT_OUTLIFS == 1)     \
    {                                                                                                                               \
      /* Add CUD if Multicast */                                                                                                    \
      is_tm_cud_to_add = system_mc;                                                                                                 \
    }                                                                                                                               \
    else                                                                                                                            \
    {                                                                                                                               \
      is_tm_cud_to_add = TRUE;                                                                                                      \
    }

#define ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_GET(tm_port_profile, is_src)  \
  is_src = SOC_SAND_NUM2BOOL(((tm_port_profile - ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) / 6) % 2)

#define ARAD_EGR_PROG_TM_PORT_PROFILE_TM_DEST_GET(tm_port_profile, is_dest)  \
  is_dest = SOC_SAND_NUM2BOOL(((tm_port_profile - ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) / 3) % 2)

/*
 * The jump-to entry is 2*pointer (==pointer<<1) or 2*pointer+1 for odd entries.
 * The jump instruction has a format of {entry[5:1], 4'b0, entry[0]} .
 * So pointer-to-address mapping is: address = (pointer<<1)<<4 [ +1 for odd entries ]
 */
#define ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ADDR(_branch_pointer, _is_entry_odd) \
  (((_branch_pointer)<<5) + ((_is_entry_odd) ? 1 : 0))

/*
 * Calculates the argument given as an ECN DM threshold to the relevant program var. 
 * first, we shift arg to fit the nano scale only if it is lower than 1 << 30      .
 * Second and third , we test if the argument fits the pre determined range.                                                                                .
 * Fourth and most important, we shift arg by a pretermined range of bits and truncate it according to the bit                                                                                                                                                         .
 * preceding the lowest bit in the calculated range.                                                                                                                                                                                                                                                                    .
 */
#define ARAD_PRGE_ECN_DM_PROGRAM_VAR_SET(arg) \
    if (arg < ARAD_PRGE_ECN_DM_NTP_LOWER_BOUND) \
    { \
        LOG_WARN(BSL_LS_BCM_OTHER, (BSL_META_U(unit, \
             "warning: In current time resolution lowest threshold is %d nano. Given argument is smaller. Will be truncated to 0.\n"),ARAD_PRGE_ECN_DM_NTP_LOWER_BOUND)); \
        arg = 0;  \
    } else if (arg > ARAD_PRGE_ECN_DM_NTP_UPPER_BOUND) \
    { \
        LOG_WARN(BSL_LS_BCM_OTHER, (BSL_META_U(unit, \
             "warning: In current time resolution upper threshold is %d nano. Given argument is bigger . Will be truncated to upper bound.\n"),ARAD_PRGE_ECN_DM_NTP_UPPER_BOUND));  \
        arg = ARAD_PRGE_ECN_DM_NTP_MAX_RANGE; \
    } else \
    {   \
        arg = ((arg >> (ARAD_PRGE_ECN_DM_NTP_MAJOR_SHIFT)) & (ARAD_PRGE_ECN_DM_NTP_MAX_RANGE));  \
    }

/*
 * Returns the argument given as an ECN DM threshold to the relevant program var. 
 * If the value is different from 0 and ARAD_PRGE_ECN_DM_NTP_MAX_RANGE, we calcaulate the inverse of the var_set macro.                                                                                                                                                                                                                                                                   .
 * Otherwise, we return the placed value as is (no shift manipulation).                                                                                                                                                                                                                                                                                                                                                                 .
 */
#define ARAD_PRGE_ECN_DM_PROGRAM_VAR_GET(arg) \
    if ((arg != 0) && (arg != ARAD_PRGE_ECN_DM_NTP_MAX_RANGE)) \
    { \
        arg <<= (ARAD_PRGE_ECN_DM_NTP_MAJOR_SHIFT);  \
    } 
    
/* Converts an allocated pointer (0-20/41(jericho)) to an even entry in the instruction memory table (0-40/82(jericho)) */
#define _ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ENTRY_EVEN(_pointer)    ((_pointer)<<1)

/* Converts an allocated pointer (0-20/41(jericho)) to an odd entry in the instruction memory table (0-40/82(jericho)) */
#define _ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ENTRY_ODD(_pointer)     (((_pointer)<<1)+1)



/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
    ARAD_EGR_PROG_EDITOR_PROG_VXLAN, /* epni_prge_config_vxlan */
    /* ERSPAN WA = change ERSPAN packet flow / egress editor program to work in ARAD-A0 + B0 with IPv4 routed packet */
    ARAD_EGR_PROG_EDITOR_PROG_ERSPAN,                   /* ERPAN without XGS without WA */
    ARAD_EGR_PROG_EDITOR_PROG_ERSPAN_WA,                /* ERPAN without XGS with    WA */
    ARAD_EGR_PROG_EDITOR_PROG_ERSPAN_XGS_PE_FROM_FTMH,  /* ERPAN with    XGS with    WA */
    ARAD_EGR_PROG_EDITOR_PROG_ERSPAN_XGS_NO_WA,         /* ERPAN with    XGS without WA */
    ARAD_EGR_PROG_EDITOR_PROG_RSPAN, /* epni_prge_config_rspan */
    ARAD_EGR_PROG_EDITOR_PROG_RSPAN_XGS,
    ARAD_EGR_PROG_EDITOR_PROG_MPLS_SWAP_COUPLING, /* MPLS SWAP - Coupling */
    ARAD_EGR_PROG_EDITOR_PROG_XGS_FROM_FRC_LITE, /* XGS from FRC LITE  (XGS TM, diff serv) */
    ARAD_EGR_PROG_EDITOR_PROG_XGS_FROM_FTMH, /* XGS from FTMH (Port extender) */
    ARAD_EGR_PROG_EDITOR_PROG_XGS_FROM_FTMH_TM, /* XGS from FTMH in TM mode */
    ARAD_EGR_PROG_EDITOR_PROG_XGS_FROM_FTMH_ARP, /* XGS from FTMH (Port extender) with support for ARP extender */
    ARAD_EGR_PROG_EDITOR_PROG_XGS_ADD, /* XGS_MAC_EXT_AND_SPAN */
    ARAD_EGR_PROG_EDITOR_PROG_ETH_WITH_LLID_TAG, /* ETH_WITH_LLID_TAG (EPON) */
    ARAD_EGR_PROG_EDITOR_PROG_IPV6_TUNNEL, /* IPV6_TUNNEL_PROGRAM */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_LOOPBACK, /* ETH_OAM_LOOPBACK */
    /** Note that the program  OAM DM 1588/LM  Program (OamSubType
     *  == 1/ 2) Has been split up into 2 programs: OAM_TS for
     *  subtype 1,3 and OAM_TS_1588 for subtype 2 */
    /** Likewise the following 4 programs were originally 2. */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_DM_DOWN_TRAP, /* for ARAD+. Uses 1588 time stamping only. */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_DM_DOWN_TRAP_NTP, /* for ARAD+. Uses NTP time stamping only. */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_DM_UP_RX_TRAP_NTP, /* for ARAD+. Uses NTP time stamping only. */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_DM_UP_RX_TRAP, /* for ARAD+. Uses 1588 time stamping only. */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_LM_UP_TRAP, /* for ARAD+. OAM subtype = 1 */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM, /* OAM OamSubType = 4 (OAM CCM) */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM_UP_MEP_TRAP_2_FHEI, /* OAM OamSubType = 4 (OAM CCM) with 2 FHEI */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_EGRESS_SNOOP_WITH_OUTLIF_INFO, /* OAM egress snoop WA for MIP */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_DROP_1ST_ENC, /* Drop 1st encapsulation as it is created by OAMP */
    ARAD_EGR_PROG_EDITOR_PROG_OAM_UPMEP_LOOPBACK_SAT,
    ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_HEADER, /* PP_COPY_HEADER (Default PP) */
    ARAD_EGR_PROG_EDITOR_PROG_PP_COE_ITMH_COPY_HEADER, /* PP_COPY_HEADER in case using COE port extender with ITMH (no EGR PP) */
    ARAD_EGR_PROG_EDITOR_PROG_PP_FULL_MYMAC_L3_COPY_HEADER, /* Update FULL MyMAC SA and rest is PP_COPY_HEADER */
    ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_ARP_EXT, /* Support for ARP extender for Arad LC - Default PP */
    ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_TM_TRAFFIC_REMOVE_USER_HEADER, /* Perform PP-Copy on Traffic with TM fwd-code and no PPH to show wher the UDH ends */
    ARAD_EGR_PROG_EDITOR_PROG_OTMH,
    ARAD_EGR_PROG_EDITOR_PROG_OTMH_CUD, /* OTMH program with 24bit CUD extention */
    ARAD_EGR_PROG_EDITOR_PROG_REMOVE_SYSTEM_HEADER, /* Removing system headers */
    ARAD_EGR_PROG_EDITOR_PROG_REMOVE_TDM_FTMH, /* Remove TDM FTMH */
    ARAD_EGR_PROG_EDITOR_PROG_TDM_PMM, /* TDM PMM */
    ARAD_EGR_PROG_EDITOR_PROG_TDM_STAMP_CUD, /* TDM_STAMP_CUD */
    ARAD_EGR_PROG_EDITOR_PROG_TDM_STAMP_CUD_OVER_OUTER_FTMH,
    ARAD_EGR_PROG_EDITOR_PROG_STACKING, 
    ARAD_EGR_PROG_EDITOR_PROG_STACKING_UC, 
    ARAD_EGR_PROG_EDITOR_PROG_2_HOP_SCHEDULING,
    ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_HEADER_WITH_USER_HEADER, /* PP_COPY_HEADER (Default PP) + the user defined headers */
    ARAD_EGR_PROG_EDITOR_PROG_L2_REMOTE_CPU,
    ARAD_EGR_PROG_EDITOR_PROG_MIM_PTCH2,
    ARAD_EGR_PROG_EDITOR_PROG_VMAC_DOWNSTREAM,
    ARAD_EGR_PROG_EDITOR_PROG_VMAC_UPSTREAM,
    ARAD_EGR_PROG_EDITOR_PROG_OLP_PETRA_A_RESET_IS_KEY_BIT,
    ARAD_EGR_PROG_EDITOR_PROG_ADD_PACKET_SIZE_HEADER,
    ARAD_EGR_PROG_EDITOR_PROG_OTMH_KEEP_PPH,
	ARAD_EGR_PROG_EDITOR_PROG_DC_OVERLAY_VXLAN,
    ARAD_EGR_PROG_EDITOR_PROG_JER_ROUTING_OVER_OVERLAY, /* Routing over overlay in Jericho */
	ARAD_EGR_PROG_EDITOR_PROG_REFLECTOR_IP_AND_ETH, /* swap DA with SA, SIP with DIP, add PTCH.*/
	ARAD_EGR_PROG_EDITOR_PROG_REFLECTOR_ETH, /* swap DA with SA, add PTCH.*/
	ARAD_EGR_PROG_EDITOR_PROG_REFLECTOR_ETH_PROGRAMMABLE, /* swap DA with SA.*/
	ARAD_EGR_PROG_EDITOR_PROG_TM_MODE_HW_PROCESSING, 
	ARAD_EGR_PROG_EDITOR_PROG_PP_MODE_HW_PROCESSING,
	ARAD_EGR_PROG_EDITOR_PROG_4_LABEL_PUSH, /* 4 label push*/
    ARAD_EGR_PROG_EDITOR_PROG_IP_TUNNEL_PRESERVE_DSCP, /* bridge into vxlan copy dscp */
    ARAD_EGR_PROG_EDITOR_PROG_PON_DSCP_REMARKING_IPV4,
    ARAD_EGR_PROG_EDITOR_PROG_PON_DSCP_REMARKING_IPV6,
    ARAD_EGR_PROG_EDITOR_PROG_ADD_48_BYTES_SYS_HDRS, /* add 48 bytes of system headers, keep original packet*/
    ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_PTAGGED,     /* ptagged programs */
    ARAD_EGR_PROG_EDITOR_PROG_SERVICE_MODE_PTAGGED,  /* ptagged programs */
    ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_NOT_PTAGGED, /* ptagged programs */
	ARAD_EGR_PROG_EDITOR_PROG_BFD_ECHO,	/*bfd echo*/
    ARAD_EGR_PROG_EDITOR_PROG_MPLS_RAW,
	ARAD_EGR_PROG_EDITOR_PROG_END_TLV,
    ARAD_EGR_PROG_EDITOR_PROG_END_TLV_MAID_INT,
    ARAD_EGR_PROG_EDITOR_PROG_MAID_EXT_FPGA,
    ARAD_EGR_PROG_EDITOR_PROG_DELAY_BASED_ECN2,
	ARAD_EGR_PROG_EDITOR_PROG_L2_ENCAP_EXTERNAL_CPU,
	ARAD_EGR_PROG_EDITOR_PROG_TEST1, 
    ARAD_EGR_PROG_EDITOR_PROG_TEST2, 
    ARAD_EGR_PROG_EDITOR_PROG_TEST2_BRIDGE, 
    ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI_FRR,
    ARAD_EGR_PROG_EDITOR_PROG_EVPN_ROO_FRR,
    ARAD_EGR_PROG_EDITOR_PROG_MPLS_FRR,
    ARAD_EGR_PROG_EDITOR_PROG_MPLS_ELI_JER,
    /* NPV N_PORT and NP_PORT programs */
    ARAD_EGR_PROG_EDITOR_PROG_FCOE_NP_PORT,
    ARAD_EGR_PROG_EDITOR_PROG_FCOE_N_PORT,
    ARAD_EGR_PROG_EDITOR_PROG_BFD_CLEAR_FLAGS,
    ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP,
    ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_OAMTS,
    ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_IPV6,
    ARAD_EGR_PROG_EDITOR_PROG_MIM_SPB_BRIDGE_TO_OVERLAY,
    ARAD_EGR_PROG_EDITOR_PROG_MIM_SPB_UC_ROUTE_TO_OVERLAY,
    ARAD_EGR_PROG_EDITOR_PROG_MIM_SPB_MC_ROUTE_TO_OVERLAY,

    /* nof progs */
    ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS
} ARAD_EGR_PROG_EDITOR_PROGRAMS;

typedef enum
{
    ARAD_PRGE_TM_SELECT_NONE = 0,
    ARAD_PRGE_TM_SELECT_PP_COPY_HEADER = 0, /*dont care*/
    ARAD_PRGE_TM_SELECT_TTL_DECREMENT = 0, /*dont care*/
    ARAD_PRGE_TM_SELECT_INTO_BACKCONE_WITH_TTL_PORT = 0, /*dont care*/
    ARAD_PRGE_TM_SELECT_ETH_OAM_LOOPBACK = 0, /*dont care*/
    ARAD_PRGE_TM_SELECT_EoE_PROGRAM = 0, /*dont care*/
    ARAD_PRGE_TM_SELECT_XGS_PE_FROM_FRC_LITE = 0,
    ARAD_PRGE_TM_SELECT_XGS_PE_FROM_FTMH = 15,
    ARAD_PRGE_TM_SELECT_REMOVE_SYSTEM_HEADER = 2,
    ARAD_PRGE_TM_SELECT_OTMH_BASE = 1,
    ARAD_PRGE_TM_SELECT_OTMH_WITH_CUD_EXT = 6,
    ARAD_PRGE_TM_SELECT_REMOVE_TDM_OPT_FTMH = 3,
    ARAD_PRGE_TM_SELECT_TDM_PMM_HEADER = 4,
    ARAD_PRGE_TM_SELECT_RAW_MPLS = 5,
    ARAD_PRGE_TM_SELECT_TDM_STAMP_CUD = 8,
    ARAD_PRGE_TM_SELECT_FCOE_N_PORT = 9,
    ARAD_PRGE_TM_SELECT_STACKING = 10,
    ARAD_PRGE_TM_SELECT_PP_RCY_PORT = 11,
    ARAD_PRGE_TM_SELECT_L2_REMOTE_CPU = 12,
    ARAD_PRGE_TM_SELECT_ADD_PACKET_SIZE_HEADER = 14,
    ARAD_PRGE_TM_SELECT_ETH_WITH_LLID_TAG = 13, /* remove system headers */
    ARAD_PRGE_TM_SELECT_ETH_REMOVE_SYSTEM_HEADERS,
    ARAD_PRGE_TM_SELECT_REFLECTOR_IP_AND_ETH,
    ARAD_PRGE_TM_SELECT_REFLECTOR_ETH,
    ARAD_PRGE_TM_SELECT_TM_MODE_HW_PROCESSING,
    ARAD_PRGE_TM_SELECT_PP_MODE_HW_PROCESSING,
	ARAD_PRGE_TM_SELECT_L2_ENCAP_EXTERNAL_CPU,
    ARAD_PRGE_TM_SELECT_TDM_STAMP_CUD_OVER_OUTER_FTMH,

    ARAD_PRGE_TM_SELECT_NOF_PROFILES
} e_tm_program_select;

typedef enum
{
    ARAD_PRGE_PP_SELECT_NONE = 0, 
    ARAD_PRGE_PP_SELECT_PP_OAM_PORT = 1, /* Must have value 1*/
    /* port profile to select (R)SPAN - 2 or ERSPAN - 3*/
    ARAD_PRGE_PP_SELECT_PP_COPY_HEADER      = 2, /* adding another program name with same value:
                                                    the "_SPAN" defenition will be used only when there is a related to RSPAN */
    ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_SPAN = 2,
    ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_ERSPAN = 3,
    ARAD_PRGE_PP_SELECT_INTO_BACKCONE_WITH_TTL_PORT = 4,
    ARAD_PRGE_PP_SELECT_PP_EXT_FPGA_PORT = 5,
    ARAD_PRGE_PP_SELECT_ETH_OAM_LOOPBACK = 6,
    ARAD_PRGE_PP_SELECT_EoE_PROGRAM = 7,
    ARAD_PRGE_PP_SELECT_PRESERVING_DSCP = 7, /* Preserving DSCP on a per out-port bases */
    ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FRC_LITE = 8,
    ARAD_PRGE_PP_SELECT_MIM_SPB = 9,
    /* port profile to select (R)SPAN - 10 or ERSPAN - 11*/
    ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH      = 10, /* adding another program name with same value:
                                                    the "_SPAN" defenition will be used only when there is a related to RSPAN */
    ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_SPAN = 10,
    ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_ERSPAN = 11,
    ARAD_PRGE_PP_SELECT_RCY_OVERLAY = 12,
    ARAD_PRGE_PP_SELECT_PP_OLP_PORT = 13,
    ARAD_PRGE_PP_SELECT_OTMH_BASE_KEEP_PPH = 14,
    ARAD_PRGE_PP_SELECT_REMOVE_SYSTEM_HEADER = 0,
    ARAD_PRGE_PP_SELECT_OTMH_BASE = 0,
    ARAD_PRGE_PP_SELECT_OTMH_WITH_SRC_EXT = 0,
    ARAD_PRGE_PP_SELECT_OTMH_WITH_CUD_EXT = 0,
    ARAD_PRGE_PP_SELECT_OTMH_WITH_MC_ID_EXT = 0,
    ARAD_PRGE_PP_SELECT_OTMH_WITH_SRC_AND_CUD_EXT = 0,
    ARAD_PRGE_PP_SELECT_OTMH_WITH_SRC_AND_MC_ID_EXT = 0,
    ARAD_PRGE_PP_SELECT_REMOVE_TDM_OPT_FTMH = 0,
    ARAD_PRGE_PP_SELECT_TDM_PMM_HEADER = 0,
    ARAD_PRGE_PP_SELECT_TDM_STAMP_CUD = 0,
    ARAD_PRGE_PP_SELECT_L2_REMOTE_CPU = 0,
	ARAD_PRGE_PP_SELECT_L2_ENCAP_EXTERNAL_CPU = 0,
    ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_WITH_USER_HEADERS = 15
} e_pp_program_select;

typedef enum
{
  /*
   *  CPU without editing maybe need to add the stamping of the cud
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_CPU,
  /*
   *  Raw without editing remove all system headers including PPH
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_RAW,
  /*
   * Stacking Port With/Without CUD ext
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_STACK1,
  /*
   *  fap20b rcy only increase the packet in two bytes
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_FAP20,
  /*
   *     TDM  mode: removing FTMH
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TDM1,
  /*
   * TDM  mode: remove standard FTMH and generating external header
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TDM2,
  /*
   *Remove system headers and old Network header and Add new Network Header
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_ETH,
  /*
   *  CPU without editing but add the stamping of the cud
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_CPU_CUD,
  /*
   *  No Source, No Destination, Never CUD.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV,
  /*
   *  No source, No Destination, CUD if Multicast.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC,
  /*
   *    No Source, No Destination, CUD always.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW,
  /*
   *  No Source, Destination, Never CUD.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_NEV,
  /*
   *  No source, Destination, CUD if Multicast.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_MC,
  /*
   *    No Source, Destination, CUD always.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_ALW,
  /*
   *  Source, No Destination, Never CUD.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV,
  /*
   *  Source, No Destination, CUD if Multicast.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC,
  /*
   *    Source, No Destination, CUD always.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW,
  /*
   *  Source, Destination, Never CUD.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_NEV,
  /*
   *  Source, Destination, CUD if Multicast.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_MC,
  /*
   *    Source, Destination, CUD always.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_ALW,
  /*
   *  No Source, No Destination, Never CUD, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV_WITH_PPH,
  /*
   *  No source, No Destination, CUD if Multicast, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC_WITH_PPH,
  /*
   *    No Source, No Destination, CUD always, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW_WITH_PPH,
  /*
   *  No Source, Destination, Never CUD, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_NEV_WITH_PPH,
  /*
   *  No source, Destination, CUD if Multicast, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_MC_WITH_PPH,
  /*
   *    No Source, Destination, CUD always, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_ALW_WITH_PPH,
  /*
   *  Source, No Destination, Never CUD, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV_WITH_PPH,
  /*
   *  Source, No Destination, CUD if Multicast, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC_WITH_PPH,
  /*
   *    Source, No Destination, CUD always, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW_WITH_PPH,
  /*
   *  Source, Destination, Never CUD, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_NEV_WITH_PPH,
  /*
   *  Source, Destination, CUD if Multicast, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_MC_WITH_PPH,
  /*
   *    Source, Destination, CUD always, Keep PPH.
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_ALW_WITH_PPH,
  /*
   * Stacking Port With/Without CUD ext
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_STACK2,
  /*
   * Injected: FTMH (6B) + Out-LIF Extension (2B) + PPH
   */
  ARAD_EGR_PROG_TM_PORT_PROFILE_INJECTED,
  /*
   *  Number of types in ARAD_EGR_PROG_TM_PORT_PROFILE
   */
  ARAD_EGR_NOF_PROG_TM_PORT_PROFILES
} ARAD_EGR_PROG_TM_PORT_PROFILE;


/* 
 * LFEM actions
 */
typedef enum
{
    /* LSB masks */
    ARAD_EGR_PROG_EDITOR_LFEM_2_LSBS,
    ARAD_EGR_PROG_EDITOR_LFEM_3_LSBS, /* XGS FRC LITE Extract PPD type - bits 2:0   and     OAM TS Extract oam sub type bit [2:0] other zeros ARAD_EGR_PROG_EDITOR_LFEM_XGS_FRC_LITE_EXTRACT_PPD_OAM_TS */
    ARAD_EGR_PROG_EDITOR_LFEM_4_LSBS, /* TDM CUD, STACKING Extract FTMH [3:0] and CUD [15:12] */ /* ARAD_EGR_PROG_EDITOR_LFEM_TDM_CUD_STACKING_EXTRACT_FTMH, ARAD_EGR_PROG_EDITOR_LFEM_MPLS_SWAP_EXTRACT_BOS */
    ARAD_EGR_PROG_EDITOR_LFEM_5_LSBS, /* extract ees data1[20:16] ARAD_EGR_PROG_EDITOR_LFEM_OTMH_EXTRACT_EES_DATA */
    ARAD_EGR_PROG_EDITOR_LFEM_6_LSBS, /* OAM Loopback extract IVE command */ /* ARAD_EGR_PROG_EDITOR_LFEM_OAM_LOOPBACK_EXTRACT_IVE, ARAD_EGR_PROG_EDITOR_LFEM_OAM_CCM_EXTRACT_IVE */
    ARAD_EGR_PROG_EDITOR_LFEM_7_LSBS, /* IPV6 Extract sys and nwk header size   and   OAM Loopback  ARAD_EGR_PROG_EDITOR_LFEM_IPV6_EXTRACT_SYS_HEADER_OAM_LOOPBACK */
    ARAD_EGR_PROG_EDITOR_LFEM_8_LSBS, /* Extract 8 LSBs from input */ /*extract EEI [23:16] to [7:0]*/ /* ARAD_EGR_PROG_EDITOR_LFEM_ARP_EXT_EXTRACT_EEI, ARAD_EGR_PROG_EDITOR_LFEM_EXTRACT_HEADER_SIZE */
    ARAD_EGR_PROG_EDITOR_LFEM_12_LSBS,
    ARAD_EGR_PROG_EDITOR_LFEM_13_LSBS, /* Petra-B: OTMH BASE - extract src system port bit [12:0] (FTMH shifted by 32 bits) others zeros ARAD_EGR_PROG_EDITOR_LFEM_OTMH_BASE_EXTRACT_SRC_PORT_PB */
    ARAD_EGR_PROG_EDITOR_LFEM_14_LSBS, /* Petra-B: TDM CUD - extract 14 bits CUD ARAD_EGR_PROG_EDITOR_LFEM_TDM_CUD_EXTRACT_CUD_PB  &  ARAD_EGR_PROG_EDITOR_LFEM_EXTRACT_FTMH_SIZE_2 */

    ARAD_EGR_PROG_EDITOR_LFEM_8_MSBS, /* extract 8 msb, doesnt shift the bits, another description will be clearing the 8 lsbs */

    /* Bit extraction */
    ARAD_EGR_PROG_EDITOR_LFEM_BIT_0, /* OTMH CUD -extract the outlif resolved data bit ARAD_EGR_PROG_EDITOR_LFEM_OTMH_CUD_EXTRACT_OUTLIF, ARAD_EGR_PROG_EDITOR_LFEM_OAM_CCM_EXTRACT_PPH_IS_CONTROL, ARAD_EGR_PROG_EDITOR_LFEM_2_HOP_SCHEDULING_EXTRACT_BIT_FROM_OUT_LIF_EVEN*/
    ARAD_EGR_PROG_EDITOR_LFEM_BIT_2,
    ARAD_EGR_PROG_EDITOR_LFEM_BIT_3, /* OTMH extract FTMH.TM-action-is-MC (FTMH shifted by 52 bits) ARAD_EGR_PROG_EDITOR_LFEM_OTMH_EXTRACT_FTMH */


    /* General masking */
    ARAD_EGR_PROG_EDITOR_LFEM_ZERO_BIT0_TO_BIT1,  /* XGS FRC LITE bits 1:0 zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_ZERO_BIT8_TO_BIT10, /* XGS FRC LITE - bits 10:8 zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_ONE_BIT9_TO_BIT10,  /* OAM TS Stamp pph type = 3 in bits [10:9] */
    ARAD_EGR_PROG_EDITOR_LFEM_MASK_HEX_81, /* Data[15:0] & 0x81 - Valid bit and profile of FRR (MPLS extended label)*/
    ARAD_EGR_PROG_EDITOR_LFEM_ZERO_BIT3, /* Data[15:0] & ~(0x0008) - Clear BFD C-Flag */
    ARAD_EGR_PROG_EDITOR_LFEM_ZERO_BIT8, /* Data[15:0] & ~(0x0100) - Clear BOS of MPLS label[15:0]*/
    ARAD_EGR_PROG_EDITOR_LFEM_8_LSBS_OR_3, /* {8'h00, Data[7:1], 1'b1} */
    ARAD_EGR_PROG_EDITOR_LFEM_8_CENTRAL_BITS, /* get the 8 bits in the center, clear the peripheral bits */
    ARAD_EGR_PROG_EDITOR_LFEM_8_PERIPHERAL_BITS, /* get the 4 lsbs and 4 msbs, clear the central bits */
     
    /* 2 bit vals, multiplied by 4*/
    ARAD_EGR_PROG_EDITOR_LFEM_BITS_2_3_M, /* Extract bits 2,3 multiply the value of 2bits by 4*/
    ARAD_EGR_PROG_EDITOR_LFEM_BITS_1_0_M, /* Extract bits 0,1 multiply the value of 2bits by 4*/

    /* Shifts */
    ARAD_EGR_PROG_EDITOR_LFEM_LSHIFT_1, /* OAM CCM shift 1 left */
    ARAD_EGR_PROG_EDITOR_LFEM_LSHIFT_4, /* EVPN - Used for constructing MPLS label from another mpls label's EXP,S */
    ARAD_EGR_PROG_EDITOR_LFEM_LSHIFT_12,

    /* Petra-B */
    ARAD_EGR_PROG_EDITOR_LFEM_TDM_PMM_PB, /* Petra-B: TDM PMM */
    ARAD_EGR_PROG_EDITOR_LFEM_OTMH_BASE_EXTRACT_TC_PB, /* Petra-B: OTMH BASE - extract traffic class - take bits 3:1 (FTMH shifted by 20 bits) to 2:0, others zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_OTMH_BASE_EXTRACT_DP_PB, /* Petra-B: OTMH BASE - extract dp from bits 11:10 (FTMH shifted by 52 bits) to 5:4, action_type from bits 9:8 to 7:6, mc from bit 4 to bit 3 others zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_TDM_CUD_EXTRACT_FTMH_PB, /* Petra-B: TDM CUD - extract bits 15:14 from FTMH */
    ARAD_EGR_PROG_EDITOR_LFEM_OTMH_EXTRACT_FTMH_PB, /* Petra-B: OTMH extract FTMH.TM-action-is-MC (FTMH shifted by 52 bits) */
    
    /* Arad */
    ARAD_EGR_PROG_EDITOR_LFEM_OTMH_BASE_EXTRACT_TC, /* OTMH BASE - extract traffic class - take bits 5:3 (FTMH shifted by 20 bits) to 2:0, others zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_OTMH_BASE_EXTRACT_DP, /* OTMH BASE - extract dp from bits 10:9 (FTMH shifted by 52 bits) to 5:4, action_type from bits 8:7 to 7:6, mc from bit 3 to bit 3 others zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_XGS_FRC_LITE_EXTRACT_DSP, /* XGS FRC LITE Extract DSP - bits 10:3 */
    ARAD_EGR_PROG_EDITOR_LFEM_XGS_FRC_LITE_EXTRACT_MC, /* XGS FRC LITE Extract MC bit */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS_CCM_LOOPBACK_EXTRACT_FHEI, /* OAM TS , OAM CCM, OAM Loopback Extract fhei size bits [1:0] to {[1:0],1'b1} */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_LOOPBACK_EXTRACT_TC, /* OAM Loopback Extract TC bits [29:27] to bits [8:6], DP bits [2:1] to bits [5:4] and add bits 4'b1100 to lsb */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_LOOPBACK_EXTRACT_TC_JER, /* OAM Loopback Extract TC  DP bits {3'b000, DP,3'b001, 4'b0000,TC,1'b0} */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_CCM_EXTRACT_PPH_OFFSET, /* OAM CCM extract pph.offset bits [7:1] to [6:0] */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS_CCM_FHEI, /* OAM CCM set FHEI size = 3, set FWD_CODE = CPU_TRAP, fhei size 3bytes, as fhei trap is 3B */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS_STAMP_PPH, /* OAM TS Stamp pph type = 3 in bits [2:1] */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_CCM_PPH_FWD_CODE, /*set FWD_CODE = {1'b1, fwd_code[2:0]} , the rest is the same*/
    ARAD_EGR_PROG_EDITOR_LFEM_STACKING_8, /* user configured */
    ARAD_EGR_PROG_EDITOR_LFEM_STACKING_10, /* user configured */
    ARAD_EGR_PROG_EDITOR_LFEM_2_HOP_SCHEDULING, 
    ARAD_EGR_PROG_EDITOR_LFEM_2_HOP_SCHEDULING_EXTRACT_FLOW_ID, /* {12 MSB of ITMH, 4'b1010} */
    ARAD_EGR_PROG_EDITOR_LFEM_2_HOP_SCHEDULING_EXTRACT_TC,
    ARAD_EGR_PROG_EDITOR_LFEM_PP_MIM_32K_IVEC_CMD, /* take bit 1 of In-LIF profile from FHEI to bit 4, others zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_OLP_PETRA_A,
    ARAD_EGR_PROG_EDITOR_LFEM_EXTRACT_FTMH_SIZE,
    ARAD_EGR_PROG_EDITOR_LFEM_FTMH_2_LSBYTES, /*FEM-0-1 : FTMH 2LSBytes {12'h0,1'b1 ftmh[2:0]}*/
    ARAD_EGR_PROG_EDITOR_LFEM_FTMH_2_BYTES, /*FEM-0-3 :  FTMH 2Byte {FTMH[39],8'h0,FTMH[30:27],2'b00,FTMH[24] }*/
    ARAD_EGR_PROG_EDITOR_LFEM_FTMH_2_BYTES_2, /*FEM-1-3 FTMH 2Byte {1'b0, out_pp_port[7:0],7'h0 }*/
    ARAD_EGR_PROG_EDITOR_LFEM_EXTRACT_FTMH_TM_ACTION_IS_MC,  /*FEM-0-3 :extract FTMH.TM-action-is-MC (FTMH shifted by 52 bits)*/
    ARAD_EGR_PROG_EDITOR_LFEM_OUT_PP_PORT,  /*FEM-0-2 : {1'b0, 3'b000, OutPpPort[5:0], 6'b110000}*/
    ARAD_EGR_PROG_EDITOR_LFEM_FTMH_DP,  /*FEM-0-5 : {8'h00,1'b1,2'b00,FTMH.DP[1:0],3'b000}*/
    ARAD_EGR_PROG_EDITOR_LFEM_OTMH_MC_CUD_EXTRACT_MC_CUD, /* OTMH MC-CUD - extract MC-CUD[18:16] from bits 18:16 (FTMH shifted by 68 bits) to 2:0, others zeros */
    ARAD_EGR_PROG_EDITOR_LFEM_XGS_FRC_LITE_FRC_FROM_FTMH_EXTRACT_TC, /* XGS FRC LITE, XGS FRC FROM FTMH Extract TC - bits 5:3 */
    ARAD_EGR_PROG_EDITOR_LFEM_XGS_FRC_FROM_FTMH_EXTRACT_DP, /* XGS FRC FROM FTMH Extract DP - bits [2:1] (FTMH shifted by 44 bits) to bits [7:6] */
    ARAD_EGR_PROG_EDITOR_LFEM_TDM_CUD_STACKING_EXTRACT_CUD, /* TDM CUD, STACKING Extract CUD [11:0] and FTMH [31:20] */
    ARAD_EGR_PROG_EDITOR_LFEM_IPV6_EXTRACT_FTMH, /* IPV6 extract FTMH size */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS_EXTRACT_TOD, /* OAM TS extract [33:32] to [1:0] of tod_ntp  */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS_EXTRACT_TOD_APLUS, /* OAM TS extract [33:32] to [1:0] of tod_ntp on Arad+ */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS_EXTRACT_TOD_NTP, /* OAM TS extract [33:32] to [1:0] of tod_ntp */
    ARAD_EGR_PROG_EDITOR_LFEM_OAM_TS, /* OAM TS [16:1] */
    ARAD_EGR_PROG_EDITOR_LFEM_DA_COPY, /* DA 1 1/2 copy {[15:4],4'h0} */
    ARAD_EGR_PROG_EDITOR_LFEM_FTMH_TC, /* FEM-1-2 : {1'b0, FTMH.TC[3:0],12'h000} */
    ARAD_EGR_PROG_EDITOR_LFEM_SP_5_0, /*FEM-1-4 : {8'h28,2'b10,SP[5:0]} */
	ARAD_EGR_PROG_EDITOR_LFEM_LSB_MPLS, /*4_label_push*/
	ARAD_EGR_PROG_EDITOR_LFEM_MSB_MPLS, /*4_label_push*/
	ARAD_EGR_PROG_EDITOR_LFEM_BITS_1_2,
    ARAD_EGR_PROG_EDITOR_LFEM_SET_BITS_7_8,
    ARAD_EGR_PROG_EDITOR_LFEM_SVID, 
    ARAD_EGR_PROG_EDITOR_LFEM_EID,
    ARAD_EGR_PROG_EDITOR_LFEM_FAP_ID, 
    ARAD_EGR_PROG_EDITOR_LFEM_SA_FAP,
    ARAD_EGR_PROG_EDITOR_LFEM_SA_LAG, 
    ARAD_EGR_PROG_EDITOR_LFEM_IS_LAG,
    ARAD_EGR_PROG_EDITOR_LFEM_4_MSB_AND_LSB,
    ARAD_EGR_PROG_EDITOR_LFEM_TOS,
    ARAD_EGR_PROG_EDITOR_LFEM_MASK_BIT_2_3,
    ARAD_EGR_PROG_EDITOR_LFEM_FTMH_TC_AS_MSB, /* FTM[3],1'b0,12'h000*/
	
    /* Jericho */
    ARAD_EGR_PROG_EDITOR_LFEM_FRR_EXP_S, /* {12'h000,Data[3:1],1'b0} */
    ARAD_EGR_PROG_EDITOR_LFEM_ESI_LSB, /* { Data[3:0], EXP[2:0]=0, S=1, TTL[7:0]=1 } */

    ARAD_EGR_PROG_EDITOR_LFEM_FIRST_FIFTH_NIBBLES, /* {nibble7,nibble6,nibble5,nibble4,nibble3,nibble2,nibble1,nibble0} -> {4'b0000,4'b0000,nibble4,nibble0} */

    ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS,

    /* Special type - Must come after ARAD_EGR_PROG_EDITOR_PROG_NOF_LFEMS !!! */
    ARAD_EGR_PROG_EDITOR_LFEM_NULL

} ARAD_EGR_PROG_EDITOR_LFEM;

typedef ARAD_EGR_PROG_EDITOR_LFEM lfem_maps_shadow_t[ARAD_EGR_PRGE_MAX_NOF_LFEM_TABLES][ARAD_EGR_PRGE_MAX_NOF_LFEMS];

typedef enum
{

    /*
     * General usage branches
     */
    ARAD_EGR_PROG_EDITOR_BRANCH_DROP_PACKET, /* Used by ARAD_EGR_PROG_EDITOR_PROG_EVC_FILTER */

    /*
     * Program specific branches
     */
    ARAD_EGR_PROG_EDITOR_BRANCH_BASE_OTMH_CUD_AND_SOURCE_PORT,
    ARAD_EGR_PROG_EDITOR_BRANCH_BASE_OTMH_CUD_MC,
    ARAD_EGR_PROG_EDITOR_BRANCH_BFD_ECHO,
    ARAD_EGR_PROG_EDITOR_BRANCH_PP_COPY_HEADER,
    ARAD_EGR_PROG_EDITOR_BRANCH_2_HOP_SCHEDULING,
    ARAD_EGR_PROG_EDITOR_BRANCH_STACKING,
    ARAD_EGR_PROG_EDITOR_BRANCH_DELAY_BASE_ECN2,
    ARAD_EGR_PROG_EDITOR_BRANCH_OAM_STRING_MAID,
    ARAD_EGR_PROG_EDITOR_BRANCH_PP_FULL_MYMAC_L3_COPY_HEADER,
    /* Number of branch IDs */
    ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF

} ARAD_EGR_PROG_EDITOR_BRANCH;


/*
 * Programs branch usage
 */
typedef enum
{

    /* ARAD_EGR_PROG_EDITOR_PROG_EVC_FILTER */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_EVC_FILTER,

    /* ARAD_EGR_PROG_EDITOR_PROG_OTMH */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_OTMH_SPLIT1,
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_OTMH_SPLIT2,
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_OTMH_JUMP_BACK,

    /* ARAD_EGR_PROG_EDITOR_PROG_BFD_ECHO */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_BFD_ECHO,

    /* ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_HEADER */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_PP_COPY_HEADER,

    /* ARAD_EGR_PROG_EDITOR_PROG_2_HOP_SCHEDULING */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_2_HOP_SCHEDULING,

    /* ARAD_EGR_PROG_EDITOR_PROG_STACKING */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_STACKING,

    /* ARAD_EGR_PROG_EDITOR_PROG_DELAY_BASE_ECN2 */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_DELAY_BASE_ECN2,

    /* ARAD_EGR_PROG_EDITOR_PROG_END_TLV_MAID_INT */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_OAM_MAID_INT,



    /* Number of branch IDs */
    ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF

} ARAD_EGR_PROG_EDITOR_JUMP_POINT;


typedef struct
{
  uint32 lfem_program;
  uint32 bitcount;
  uint32 niblle_field_offset;
  uint32 header_offset_select;
  uint32 source_select;
  uint32 valid;
} ARAD_EGR_PROG_EDITOR_CE_INSTRUCTION;

typedef struct
{
  int prog_used; /* prog ID in HW, -1 if not used */
  ARAD_EGR_PROG_EDITOR_LFEM lfem_bk[ARAD_EGR_PRGE_MAX_NOF_LFEM_TABLES][ARAD_EGR_PRGE_MAX_NOF_LFEMS]; /* book keeping for diag */
  uint32 program_pointer; 
  uint32 tm_profile;
  uint8 num_value_1_offset_profiles; /* number of profiles used for value 1 offset. */
  uint8 value_1_offset_first_profile; /* Should be set dynamically in arad_egr_prg_editor_set_value_1_offset().*/
} ARAD_EGR_PROG_EDITOR_PROGRAM_INFO;

typedef struct
{
  uint8 otmh_base;
  uint8 vxlan;
  uint8 erspan;
  uint8 rspan;
  uint8 mpls_swap;
  uint8 xgs_tm; /* XGS from FRC LITE  (XGS TM, diff serv) */
  uint8 xgs_pp; /* XGS from FTMH (Port extender) */
  uint8 epon;
  uint8 ipv6_tunnel;
  uint8 oam_loopback; /* ETH_OAM_LOOPBACK, OAM OamSubType = 5 (ETH_OAM_LOOPBACK) */
  uint8 oam_ts_ccm; /* OAM OamSubType = 1/3 (OAM TS), OAM OamSubType = 4 (OAM CCM) */
  uint8 oam_dm_down; /* add 4 MSB's of time.*/
  uint8 oam_dm_up_rx; /* Delay Meassurement up */
  uint8 oam_lm_upmep; /* Loss Messurment upmep */
  uint8 oam_upmep; /* CCM Upmep */
  uint8 oam_dm_ntp; /* Use NTP for DM */
  uint8 oam_ts_ccm_fhei_2; /* OAM CCM preserve ingress packet edits when OAM packet is trapped at egress */
  uint8 tdm; /* Remove TDM FTMH */
  uint8 tdm_stamp_cud; /* Program stamps CUD into FTMH.MC-ID in TDM MC packet. Used in case user wants more channels than our supported OTM ports */
  uint8 tdm_pmm; /* TDM PMM: in case there's Petra-B in system */
  uint8 stacking;
  uint8 two_hop_scheduling; /* 2_HOP_SCHEDULING */
  uint8 l2_remote_cpu; /* l2 header for remote CPU */
  uint8 otmh_cud; /* OTMH program with 24bit CUD extention */
  uint8 trill;
  uint8 vmac_enable;
  uint8 olp_petra;
  uint8 size_header;
  uint8 user_header_always_remove; 
  uint8 bfd_echo;
  uint8 mpls_raw;
  uint8 delay_based_ecn2;
  uint8 l2_encap_external_cpu;
  uint8 npv_without_my_mac;
  uint8 evpn;
  uint8 evpn_frr;
  uint8 mpls_eli;
  uint8 mim_spb;

} ARAD_EGR_PROG_EDITOR_PROGRAMS_USAGE_INFO;

typedef struct{
    /* 
     * tm profiles 
     * -1 means this profile is not loaded
     */
    int tm_profiles[ARAD_PRGE_TM_SELECT_NOF_PROFILES];

    /* 
     * all the programs info:
     */
    ARAD_EGR_PROG_EDITOR_PROGRAM_INFO programs[ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS];

} ARAD_EGR_PROG_EDITOR_INFO;



/* } */
/*************
 * GLOBALS   *
 *************/

extern  ARAD_EGR_PROG_EDITOR_PROGRAM_INFO programs[SOC_MAX_NUM_DEVICES][ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS];
extern  ARAD_EGR_PROG_EDITOR_PROGRAMS_USAGE_INFO programs_usage[SOC_MAX_NUM_DEVICES];
extern  int last_program_id[SOC_MAX_NUM_DEVICES];
extern  int last_program_pointer[SOC_MAX_NUM_DEVICES];
extern  int branch_entries[SOC_MAX_NUM_DEVICES][ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF];

/* 
 * Shadow of the FEM actions loaded to the HW.
 * ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS means this slot is empty, other values represent the FEM action.
 */
extern  lfem_maps_shadow_t lfem_maps_shadow[SOC_MAX_NUM_DEVICES];



/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

int
  arad_egr_prog_editor_profile_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     eg_tm_profile,
    SOC_SAND_OUT uint32                     *internal_profile
  );

uint32
  arad_egr_prog_editor_tm_profile_enum_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        internal_profile,
    SOC_SAND_OUT uint32                        *eg_tm_profile
  );

uint32
  arad_egr_prog_editor_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_egr_prog_editor_config_dut_by_queue_database(
    SOC_SAND_IN int unit
  );


uint32
  arad_egr_prog_editor_stacking_lfems_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN uint32 bitmap);

/* use to update cfg attributes of the l2_ecnap_to_cpu program */
uint32
 arad_egr_prog_l2_encap_external_cpu_program_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 pp_port_var,
   SOC_SAND_IN uint64 prge_var
   
 );

/* get cfg attributes of the l2_ecnap_to_cpu program */
uint32
arad_egr_prog_l2_encap_external_cpu_program_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT uint32 *pp_port_var,
   SOC_SAND_OUT uint64 *prge_var
);

/* use to update cfg attributes of the vxlan program, e.g. upd-dest port */
uint32
arad_egr_prog_vxlan_program_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint16 udp_dest_port
 );

int
  arad_egr_prog_editor_program_pointer_get(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   ARAD_EGR_PROG_EDITOR_PROGRAMS       program_id,
    SOC_SAND_OUT  uint32                              *program_pointer
  );

uint32
  arad_egr_prog_editor_pp_pct_var_set(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN uint32 port,
    SOC_SAND_IN uint32 value
  );

uint32
  arad_egr_prog_editor_pp_pct_var_get(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN uint32 port,
    SOC_SAND_OUT uint32 *value
  );

int
  arad_egr_prog_editor_profile_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     q_pair,
    SOC_SAND_IN  uint32                     prog_editor_profile
  );


/*
 * Allocates instruction entries for a program's branch.
 */
uint32
  arad_egr_prog_editor_branch_pointer_allocate(
     SOC_SAND_IN    int                             unit,
     SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_BRANCH     branch_id,
     SOC_SAND_OUT   uint32                         *entry
  );

/*
 * Sets a program var for a given program
 */
int
  arad_pp_prge_program_var_set(
     SOC_SAND_IN    int                             unit,
     SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
     SOC_SAND_IN    uint64                          var
  );

/*
 * Gets a program var from a given program
 */
int
  arad_pp_prge_program_var_get(
     SOC_SAND_IN    int                             unit,
     SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
     SOC_SAND_OUT   uint64                          *var
  );

/* In case the PRGE is out of LFEMs, prints duplications as part of the error output
   to help solve the situation */
void
  arad_egr_prog_editor_print_lfem_duplicates(int unit);


/* ***************************************************************************************
                                      PRGE management functions
   *************************************************************************************** */


/*
 * arad_egr_prog_editor_supply_lfem_idx
 * Used for LFEM management during instruction loading to the HW.
 * If the needed LFEM is already allocated an entry in the LFEM map table
 * used from the instruction's memory table, the index is returned and no new
 * allocation is made.
 * If the LFEM is not available in the LFEM map table available from the
 * instruction's memory table, the function tries to allocate an entry for
 * the LFEM.
 * PARAMS:
 *   SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_PROGRAMS   prog - the program in which
 *                                      the LFEM is used (for diagnostics only)
 *   SOC_SAND_IN    int                             mem  - Instruction's memory table
 *   SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_LFEM       lfem - the needed LFEM action
 *   SOC_SAND_OUT   uint32                         *index- returned index.
 *
 * if unable to allocate the entry needed, Prints hints for
 * instructions reordering in the form of duplicate LFEM loads.
 */
uint32
  arad_egr_prog_editor_supply_lfem_idx(
     SOC_SAND_IN    int                             unit,
     SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_LFEM       lfem,
     SOC_SAND_IN    int                             mem,
     SOC_SAND_IN    ARAD_EGR_PROG_EDITOR_PROGRAMS   prog,
     SOC_SAND_OUT   uint32                         *index
  );
/* 
 * set Program selection valid bit by program id: 
 * Parameters                                   :
 * in   device_id                                           :
 * in   prog - program id                                                       :
 * in   prog_selection_valid - set program selection valid or invalidate                                                                             :
 *           is set (>0) set the program selection valid
 *           else (==0) invalidate the program selection
 */ 
/*  SOC_SAND_IN  ARAD_EGR_PROG_EDITOR_PROGRAMS      prog ,   */
    /* SOC_SAND_IN  int                             prog , */
uint32
  arad_egr_prog_editor_invalidate_program_by_id(
    SOC_SAND_IN  uint32                          unit ,
    SOC_SAND_IN  ARAD_EGR_PROG_EDITOR_PROGRAMS      prog ,
    SOC_SAND_IN  int                             prog_selection_valid
  );
/*
 * arad_egr_prog_editor_cat_nops
 * Concatenates nop instructions to the end of a program with less
 * than the needed instructions.
 * PARAMS:
 *   SOC_SAND_IN int mem        - first instructions table to write to
 *   SOC_SAND_IN int entry      - first entry in instruction table to write to
 *   SOC_SAND_IN int entry_even - even entries for this program
 *   SOC_SAND_IN int entry_odd  - odd entries for this program
 */
uint32
  arad_egr_prog_editor_cat_nops(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int first_mem,
    SOC_SAND_IN int first_entry,
    SOC_SAND_IN int entry_even,
    SOC_SAND_IN int entry_odd
  );


/*
 * arad_egr_prog_editor_null_prog_init
 * Initializing the deafult NULL program 
 * In teh context of port_extender destination need to add port extender. 
 * Concatenates nop instructions to the end of a program with less
 * than the needed instructions.
 * PARAMS:
 *   SOC_SAND_IN int entry_even - even entries for this program
 *   SOC_SAND_IN int entry_odd  - odd entries for this program
 */
uint32
  arad_egr_prog_editor_null_prog_init(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int entry_even,
    SOC_SAND_IN int entry_odd
  );

/*
 * Fixes the program selection of programs that need outlif profiles.
 * Necessary since outlif profile database is initialized after the egress editor is.
 * 
 * Parameters:
 *   outlif_app - The application that uses the outlif profile bits by which the program should be selected.
 *   program - Egress editor program which selection should be updated (All selection TCAM lines that select this program will be updated).
 *   selection_value - the value set in the relevant bits of the outlif program that should select the program.
 *   outlif_profile_index - which outlif profile should contain the value for the selection (outlif-profile1/2/3/4). Range: 1-4 .
 */
uint32
  arad_egr_prog_editor_prog_sel_outlif_profile_set(int unit,
                                                   SOC_OCC_MGMT_OUTLIF_APP outlif_app,
                                                   ARAD_EGR_PROG_EDITOR_PROGRAMS program,
                                                   uint32 selection_value,
                                                   uint32 outlif_profile_index);

/* ***************************************************************************************
                                             Diags
   *************************************************************************************** */

/* Gets the program name */
void
  arad_egr_prog_editor_prog_name_get_by_id(
     uint32 program_id,
     char** prog_name
  );

/* Gets the branch name */
void
  arad_egr_prog_editor_branch_name_get_by_id(
     uint32 branch_id,
     char** branch_name
  );

/* Given a name, gets the program ID */
uint32
  arad_egr_prog_editor_program_by_name_find(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  char                          *prog_name,
    SOC_SAND_OUT ARAD_EGR_PROG_EDITOR_PROGRAMS *prog
  );


int
  arad_egr_prog_editor_print_all_programs_data(int unit);

int
  arad_egr_prog_editor_print_chosen_program(int unit, uint32 first_instruction, uint8 raw_output);


#if ARAD_DEBUG_IS_LVL1

#endif /* ARAD_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_EGR_PROG_EDITOR_INCLUDED__*/
#endif



