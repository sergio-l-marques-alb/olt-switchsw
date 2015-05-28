/* $Id: dpp_vtt.h, v 1.95 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifndef __ARAD_PP_VTT_INCLUDED__
#define __ARAD_PP_VTT_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>


/* VTT logical DataBasesprefixes */
#define  ARAD_PP_ISEM_ACCESS_NULL_PREFIX               (0) /* Means that this value is irrelevant the IS is not searched*/
#define  ARAD_PP_ISEM_ACCESS_BRIDGE_PREFIX             (0) /* ISA and ISB DBs includes: VDxInitial-VID, VDxOuter-VID, VDxOuterxInner*/
#define  ARAD_PP_ISEM_ACCESS_OVERLAY_SIP_PREFIX        (1) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_IP_PREFIX                 (1) /* ISB */
#define  ARAD_PP_ISEM_ACCESS_FLEXIBLE_Q_IN_Q_DOUBLE    (2) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_BRIDGE_DT                 (2) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_FLEXIBLE_Q_IN_Q_SINGLE    (2) /* ISB */
#define  ARAD_PP_ISEM_ACCESS_BRIDGE_ST                 (2) /* ISB */
#define  ARAD_PP_ISEM_ACCESS_ISID_DOMAIN_ISID_PREFIX   (3) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_FC_PREFIX                 (3)/* ISB (TRILL_VSI isnt used in this case)  */
#define  ARAD_PP_ISEM_ACCESS_TRILL_VSI_PREFIX          (3) /* ISB (FC_PREFIX isnt used in this case) */
#define  ARAD_PP_ISEM_ACCESS_TRILL_FINE_A_GRAINED_PREFIX (4)/* ISA, ISB (IPV4_SPOOF_PREFIX isnt used in this case) */
#define  ARAD_PP_ISEM_ACCESS_VD_OUTER_PCP_PREFIX       (5) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_RPA_ID_PREFIX             (6) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_BFD_STAT_TT_ISB_PREFIX    (6)  /* ISB - BFD*/
#define  ARAD_PP_ISEM_ACCESS_INNER_TPID_PREFIX         (7) /* ISA - TRILL */
#define  ARAD_PP_ISEM_ACCESS_BFD_STAT_VT_PREFIX        (7)  /* ISB - BFD*/
#define  ARAD_PP_ISEM_ACCESS_TRILL_DESIGNATED_VID_PREFIX (8) /* ISB - TRILL Designated-VLAN */
#define  ARAD_PP_ISEM_ACCESS_BFD_STAT_TT_ISA_PREFIX    (8)  /* ISA - BFD*/
#define  ARAD_PP_ISEM_ACCESS_IP_VSI_PREFIX             (9) /* ISB  (PON not supported with overlay) */
#define  ARAD_PP_ISEM_ACCESS_OAM_STAT_VT_PREFIX        (9)  /* ISA - OAm statistics per MEP*/
#define  ARAD_PP_ISEM_ACCESS_OAM_STAT_TT_PREFIX        (10)  /* ISA, ISB OAm statistics per MEP*/
#define  ARAD_PP_ISEM_ACCESS_MPLS_L1ELI_PREFIX         (11) /* ISA or ISB (PON not supported with Indexed ELI) */
#define  ARAD_PP_ISEM_ACCESS_MPLS_L1L3ELI_PREFIX       (11) /* ISA or ISB (PON not supported with Indexed ELI) */
#define  ARAD_PP_ISEM_ACCESS_MPLS_L1L2ELI_PREFIX       (11) /* ISA or ISB (PON not supported with Indexed ELI) */
#define  ARAD_PP_ISEM_ACCESS_MPLS_ELI_UNINDEXED_PREFIX (12) /* ISA or ISB */
#define  ARAD_PP_ISEM_ACCESS_MPLS_L1_PREFIX            (12) /* ISA or ISB cant exist ARAD_PP_ISEM_ACCESS_MPLS_L1L2_PREFIX*/
#define  ARAD_PP_ISEM_ACCESS_MPLS_L1L2_PREFIX          (12) /* ISA or ISB cant exist ARAD_PP_ISEM_ACCESS_MPLS_L1_PREFIX*/
#define  ARAD_PP_ISEM_ACCESS_MPLS_L2_PREFIX            (13) /* ISA or ISB */
#define  ARAD_PP_ISEM_ACCESS_MPLS_L3_PREFIX            (13) /* ISB or ISA */
#define  ARAD_PP_ISEM_ACCESS_MPLS_L1L3_PREFIX          (13) /* ISB or ISA */
#define  ARAD_PP_ISEM_ACCESS_MPLS_UNINDEXED_PREFIX     (13) /* ISA or ISB */
#define  ARAD_PP_ISEM_ACCESS_MPLS_L2ELI_PREFIX         (14) /* ISB or ISA (PON not supported with Indexed sELI) */
#define  ARAD_PP_ISEM_ACCESS_TRILL_NICK_PREFIX         (15) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_MPLS_PWE_GAL              (15) /* ISB or ISA (Trill and PWE GAL cant exist toghether */

#define ARAD_PP_ISEM_ACCESS_QINQ_COMPRESSED_TPID1_PREFIX     (0) /*ISA*/
#define ARAD_PP_ISEM_ACCESS_QINQ_COMPRESSED_TPID2_PREFIX     (1) /*ISA*/
#define ARAD_PP_ISEM_ACCESS_QINANY_TPID1_PREFIX              (2) /*ISB*/
#define ARAD_PP_ISEM_ACCESS_QINANY_TPID2_PREFIX              (3) /*ISB*/
#define ARAD_PP_ISEM_ACCESS_QINANY_PCP_TPID1_PREFIX          (4) /*ISB*/
#define ARAD_PP_ISEM_ACCESS_QINANY_PCP_TPID2_PREFIX          (5) /*ISB*/
#define ARAD_PP_ISEM_ACCESS_1Q_TPID1_PREFIX                  (6) /*ISA*/
#define ARAD_PP_ISEM_ACCESS_1Q_TPID2_PREFIX                  (7) /*ISA*/
#define ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_TPID1_PREFIX       (8) /*ISB*/
#define ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_TPID2_PREFIX       (9) /*ISB*/
#define ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_PCP_TPID1_PREFIX   (10)/*ISA*/
#define ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_PCP_TPID2_PREFIX   (11)/*ISA*/


/* PON application prefix value  */
#define  ARAD_PP_ISEM_ACCESS_PON_UNTAGGED_PREFIX       (1) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_PON_TLS_PREFIX            (1) /* ISB (Not supported with CUS_PON_TWO_TAGS) */
#define  ARAD_PP_ISEM_ACCESS_PON_ONE_TAG_PREFIX        (2) /* ISA & ISB overwrite TRILL (isnt supported with PON) */
#define  ARAD_PP_ISEM_ACCESS_PON_TWO_TAGS_PREFIX       (4) /* ISA (cover prefixes 4-7) */
#define  ARAD_PP_ISEM_ACCESS_CUS_PON_TWO_TAGS_PREFIX   (4) /* ISB (cover prefixes 4-7) */
#define  ARAD_PP_ISEM_ACCESS_PON_VD_OUTER_PCP_PREFIX   (3) /* ISA */
#define  ARAD_PP_ISEM_ACCESS_IPV4_SPOOF_PREFIX         (8) /* ISB (cover prefixes 8-15) */


/*
 * VTT TCAM defines }
 */


/*
 * Program selection defines {
 */
#define ARAD_PP_ISEM_PFC_PART_NONE               0x0
#define ARAD_PP_ISEM_PFC_PART_E                  0x1
#define ARAD_PP_ISEM_PFC_PART_IPV4               0x2
#define ARAD_PP_ISEM_PFC_PART_TRILL              0x4
#define ARAD_PP_ISEM_PFC_PART_MPLS1              0x5
#define ARAD_PP_ISEM_PFC_PART_MPLS2              0x6
#define ARAD_PP_ISEM_PFC_PART_MPLS3              0x7

/* 3 LSBs tells which is above Ethernet (second header), 3 MSBs which is above second header */
#define ARAD_PP_ISEM_PFC_SET(MSB_HEAD,LSB_HEAD) \
  ((MSB_HEAD << 3) | LSB_HEAD)

/* Packet format codes:                                   [5:3]                [2:0] */
#define ARAD_PP_ISEM_PFC_E            (ARAD_PP_ISEM_PFC_SET(ARAD_PP_ISEM_PFC_PART_NONE , ARAD_PP_ISEM_PFC_PART_NONE))
#define ARAD_PP_ISEM_PFC_IPv4oMPLS2oE (ARAD_PP_ISEM_PFC_SET(ARAD_PP_ISEM_PFC_PART_IPV4 , ARAD_PP_ISEM_PFC_PART_MPLS2))
#define ARAD_PP_ISEM_PFC_IPv4oMPLS2oE (ARAD_PP_ISEM_PFC_SET(ARAD_PP_ISEM_PFC_PART_IPV4 , ARAD_PP_ISEM_PFC_PART_MPLS2))
#define ARAD_PP_ISEM_PFC_EoTRILLoE    (ARAD_PP_ISEM_PFC_SET(ARAD_PP_ISEM_PFC_PART_E    , ARAD_PP_ISEM_PFC_PART_TRILL))
/* traped in VTT:*/

/*
 * Qualifier - [10:7] Next Protocol, [6:5] Encapsulation, [4:3] Outer Tag Format, [2] Priority Pkt, [1:0] Inner tag format
 * Tag Formats are: 2b00 None 2b01 TPID1 2b10 TPID2 2b11 TPID3
 * MPLS Coupling is defined by NXT_PROTOCOL = (7) (one of the user defined)
 */

#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL               (0x00)  /* Untagged Pkts*/
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1        (0x08)  /* Assuming Next Protocol is Maksed */
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2        (0x10)  /* Assuming Next Protocol is Maksed*/
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1        (0x01)  /* Assuming Next Protocol is Maksed*/
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2        (0x02)  /* Assuming Next Protocol is Maksed*/
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP          (0x04)  /* Assuming Next Protocol is Maksed*/

#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL  (0x780)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP         (0x060)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID    (0x018)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP     (0x004)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID    (0x003)


#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS (0xF << 7)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS_COUPLING (7 << 7)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MAC_IN_MAC (9 << 7)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_ARP (10 << 7)
#define ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MISS (0 << 7)

/* Bits 1:2 Range1, 3:4 Range2, 5:6 Range 3 */
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L1                (0x006)  /* 00=Range1 (ELI), 01=Range2(None), 10=Range3(GAL), 11=Not in Range */
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2ELI             (0x000)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L3ELI             (0x000)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L1GAL             (0x010)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2GAL             (0x040)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_GAL               (0x004)

#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT  (0x781)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT      (0x780)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1             (0x006)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2             (0x018)
#define ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3             (0x060)

#define ARAD_PP_ISEM_ACCESS_QLFR_IP_NEXT_PROTOCOL_MPLS    (0xF << 7)
#if 0
#define ARAD_PP_ISEM_ACCESS_16B_INST_PP_PORT_KEY_VAR_X_LSBS(X) (0x448 | ((X-1) << 12))
#else
#define ARAD_PP_ISEM_ACCESS_16B_INST_PP_PORT_KEY_VAR_X_LSBS(X) ((SOC_IS_JERICHO(unit) ? 0x4A8 : 0x448) | ((X-1) << 12))
#endif

#define ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK                     (SOC_IS_JERICHO(unit) ? 0x1F:0x7)
#define ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID  (SOC_IS_JERICHO(unit) ? 0x1E:0x6)
#define ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID         (0x1)
#define ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_DOUBLE_TAG          (0x2)

typedef enum
{
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TM                     = 0x0,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MIM                    = 0x1,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_COMPATIBLE_MC          = 0x2,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP                     = 0x3,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TRILL                  = 0x4,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TRILL_DESIGNATED_VLAN  = 0x5,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS                   = 0x6,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_ARP                    = 0x7,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_UNKNOWN_L3             = 0xa,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE                 = 0xb,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MY_B_MAC_MC_BRIDGE     = 0xc,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_CUSTOM_1               = 0x8,
  ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_CUSTOM_2               = 0x9
} ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE;


/* Initialize VTT */
uint32 arad_pp_vtt_init(int unit);

/* Return DBAL SW DB */
uint32 arad_pp_dbal_vtt_sw_db_get(uint32 unit, ARAD_PP_ISEM_ACCESS_KEY  *isem_key,
                                               uint32                   *nof_tables,
                                               SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[3]);




/*
 * VT and TT Programs
 * Put VT program in VT section and TT program in TT section
 */
typedef enum
{
/* Start of VT Programs */
    ARAD_PP_DBAL_VT_PROG_INVALID = -1,
    /* TM: Used for both 1st and 2nd . No lookups */
    ARAD_PP_DBAL_VT_PROG_TM,
    /* Unindex MPLS: ISA: VD x Initial VLAN , ISB: MPLS L1 */
    ARAD_PP_DBAL_VT_PROG_VT_VDxINITIALVID_L1,
    /* Unindex MPLS: ISA:MPLS L1  , ISB: VD x Initial VLAN */
    ARAD_PP_DBAL_VT_PROG_VT_VDxOUTERVID_L1,
    /* Bridge: ISB: VD x Outer x Inner (Double Tagging) */
    ARAD_PP_DBAL_VT_PROG_VT_VDxOUTER_INNER_VID_L1,

    ARAD_PP_DBAL_VT_PROG_VT_VDxOUTER_INNER_VID_OR_OUTER_VID_L1,

    ARAD_PP_DBAL_VT_PROG_VT_OUTER_INNER_PCP_1_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_INNER_PCP_2_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_PCP_1_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_PCP_2_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_INNER_1_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_INNER_2_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_1_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_OUTER_2_TST2,
    ARAD_PP_DBAL_VT_PROG_VT_UNTAGGED_TST2,

    ARAD_PP_DBAL_VT_NOF_PROGRAMS,  /*Have to be last in VT Programs*/



/* START of TT Programs */
    ARAD_PP_DBAL_TT_PROG_INVALID = -1,
    /* TM: Used for both 1st and 2nd . No lookups */
    ARAD_PP_DBAL_TT_PROG_TM,
    ARAD_PP_DBAL_TT_PROG_MPLS_L2,
    ARAD_PP_DBAL_TT_PROG_ARP,
    ARAD_PP_DBAL_TT_PROG_BRIDGE_STAR,
    ARAD_PP_DBAL_TT_PROG_TT_UNKNOWN_L3,
    ARAD_PP_DBAL_TT_NOF_PROGRAMS

} ARAD_PP_DBAL_VTT_PROGRAMS;

/*
 * VT and TT Program Selection
 * Put VT program selection in VT section and TT program selection in TT section
 */
typedef enum
{
/* Start of VT Program Selection */
    ARAD_PP_DBAL_VT_PROG_SELECTION_INVALID = -1,
    ARAD_PP_DBAL_VT_PROG_SELECTION_TM,
    ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID,
    ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID,
    ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_VID,
    ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_VID_OR_OUTER_VID,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_1_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_2_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_1_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_2_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_1_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_2_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_1_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_2_TST2,
    ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_TST2,
    ARAD_PP_DBAL_VT_NOF_PROGRAM_SELECTION,

/* Start of TT Program Selection */
    ARAD_PP_DBAL_TT_PROG_SELECTION_INVALID = -1,
    ARAD_PP_DBAL_TT_PROG_SELECTION_TM,
    ARAD_PP_DBAL_TT_PROG_SELECTION_MPLS_2,
    ARAD_PP_DBAL_TT_PROG_SELECTION_ARP,
    ARAD_PP_DBAL_TT_PROG_SELECTION_BRIDGE_STAR,
    ARAD_PP_DBAL_TT_PROG_SELECTION_UNKNOWN_L3,
    ARAD_PP_DBAL_TT_NOF_PROGRAM_SELECTION

} ARAD_PP_DBAL_VTT_PROGRAM_SELECTION;


/***************************************************************************************
 *  Additional info for DB Table
 **************************************************************************************/
typedef enum
{
    MPLS_LABEL_0,
    MPLS_LABEL_1,
    MPLS_LABEL_2,
    MPLS_LABEL_3
}ARAD_PP_DBAL_VTT_TABLE_MPLS_LABELS;

typedef enum
{
    INITIAL_VID,
    RESERVED_VID,
    OUTER_VID,
    OUTER_INNER_VID
}ARAD_PP_DBAL_VTT_TABLE_VID;

typedef enum
{
    IP_NOT_COMPRESSED,
    IP_COMPRESSED
}ARAD_PP_DBAL_VTT_IP_TYPE;

typedef enum
{
    TPID1,
    TPID2
}ARAD_PP_DBAL_VTT_TPID;

typedef enum
{
    NO_PCP_PORT,
    PCP_PORT
}ARAD_PP_DBAL_VTT_PCP_PORT;

/* table info */
typedef struct
{
    uint32 prefix;        /*table prefix */
    int nof_qulifiers;    /* number of qualifiers */
    SOC_DPP_DBAL_QUAL_INFO   qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; /*qualifiers */
}ARAD_PP_DBAL_VTT_SW_DB_PROPERTY;

typedef struct
{
    uint32    valid_bitmask;  /* Sign if db is valid or not(bit n = '1' => db in unit n is valid; bit n = '0' => db in unit n is not valid */
    char* name;               /* Name of db */
    SOC_DPP_DBAL_SW_TABLE_IDS dbal_sw_db_id;  /* dbal enum*/
    void   (*vtt_table_property_set) (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 additional_info); /* pointer to function that initialize db property */
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES mem_type;    /* type of memory where db is resides (SEMA/SEMB/TCAM) */
    ARAD_PP_ISEM_ACCESS_KEY_TYPE key_type;     /* relevant for TCAM only */
    uint32 table_additional_info;  /* Depend on table type */
} ARAD_PP_DBAL_VTT_TABLE;

typedef struct
{
   uint32 key_and_mask;
   uint32 key_or_mask;
   uint32 lookup_enable;
}ARAD_PP_DBAL_KEY_PROPERTY;

typedef union
{
    uint8  qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; /* copy instruction id's in case of implicit_flag is '1' */
    uint8  use_32_bit_ce     [SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
}ARAD_PP_QUALIFIER_TO_CE_INFO;

typedef struct
{
    uint32 nof_keys;            /*number of keys in program ( could be 1/2/3)*/
    uint32 vtt_table_index[3];  /* array of vtt tables that used in program */
    uint32 implicit_flag;       /* sign if we need implicit program to table assotiating */
    ARAD_PP_QUALIFIER_TO_CE_INFO qual_to_ce_info;
    ARAD_PP_DBAL_KEY_PROPERTY key_property[2]; /*key_property[0] - for semA, key_property[1] - for semB */
    uint32 tcam_profile;       /* tcam profile */
    uint32 pd_bitmap_0;
    uint32 pd_bitmap_1;
    uint32 processing_profile;
    uint32 key_program_variable;

    /* Relevant only for TT stage */
    uint32 isa_key_initial_from_vt;
    uint32 isb_key_initial_from_vt;
    uint32 tcam_key_initial_from_vt;
    uint32 result_to_use_0;
    uint32 result_to_use_1;
    uint32 result_to_use_2;
    uint32 processing_code;
    uint32 second_stage_parsing;

}ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY;

typedef struct
{
    ARAD_PP_DBAL_VTT_PROGRAMS   prog_name;  /* Program Name */
    void   (*vtt_program_property_set)  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY *prog, uint32 additional_info); /* Pointer to function that initialize program property */
    uint32 prog_used[BCM_MAX_NUM_UNITS];   /* actual program used by unit */
    uint32 additional_info;
}ARAD_PP_DBAL_VTT_PROGRAM_INFO;

typedef struct
{
	    ARAD_PP_DBAL_VTT_PROGRAM_SELECTION vtt_prog_selection_name;  /* Program Selection name */
    int32   (*vt_program_selection_set) (int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 additional_info); /* Pointer to function that set Program selection(HW) */
    uint32  priority[BCM_MAX_NUM_UNITS];                         /* Line in CAM for first program selection by unit, if = '-1' it means that current program selection not in use*/
    ARAD_PP_DBAL_VTT_PROGRAMS prog_name[BCM_MAX_NUM_UNITS];      /* Program name by unit, if = '-1' it means that current program selection not in use */
    uint32 additional_info;
} ARAD_PP_DBAL_VT_PROGRAM_SELECTION_INFO;

typedef struct
{
    ARAD_PP_DBAL_VTT_PROGRAM_SELECTION vtt_prog_selection_name;  /* Program Selection name */
    int32   (*tt_program_selection_set) (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 additional_info); /* Pointer to function that set Program selection(HW) */
    uint32  priority[BCM_MAX_NUM_UNITS];                         /* Line in CAM for first program selection by unit, if = '-1' it means that current program selection not in use*/
    ARAD_PP_DBAL_VTT_PROGRAMS prog_name[BCM_MAX_NUM_UNITS];      /* Program name by unit, if = '-1' it means that current program selection not in use */
    uint32 additional_info;
} ARAD_PP_DBAL_TT_PROGRAM_SELECTION_INFO;

#endif /* __ARAD_PP_VTT_INCLUDED__*/

