/* $Id: arad_pp_isem_access.h,v 1.57 Broadcom SDK $
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

#ifndef __ARAD_PP_ISEM_ACCESS_INCLUDED__
/* { */
#define __ARAD_PP_ISEM_ACCESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>



/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_ISEM_ACCESS_KEY_SIZE                                  (2)

#define ARAD_PP_ISEM_ACCESS_NOF_TABLES                                (3)
#define ARAD_PP_ISEM_ACCESS_ID_ISEM_A                                 (0)
#define ARAD_PP_ISEM_ACCESS_ID_ISEM_B                                 (1)
#define ARAD_PP_ISEM_ACCESS_ID_TCAM                                   (2)

#define DPP_PP_ISEM_PROG_SEL_1ST_CAM_DEF_OFFSET(unit)                (SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines) - 6)
#define DPP_PP_ISEM_PROG_SEL_1ST_CAM_USR_LAST_ENTRY(unit)            (DPP_PP_ISEM_PROG_SEL_1ST_CAM_DEF_OFFSET(unit) -1)

/* ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE */
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_NONE                             (0)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_COUPLING                         (1)

/* ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE */
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE                             (0)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MAC_IN_MAC                       (1)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MAC_IN_MAC_MC_FALLBACK           (2)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC                               (3)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PWEoGRE                          (4)

/* ARAD PON flags */
#define ARAD_PP_ISEM_ACCESS_L2_PON_TLS                                         (0x1)

/* ARAD Trill flags */
#define ARAD_PP_ISEM_ACCESS_TRILL_VSI_VL                                       (0x1)
#define ARAD_PP_ISEM_ACCESS_TRILL_VSI_FGL                                      (0x2)
#define ARAD_PP_ISEM_ACCESS_TRILL_VSI_TTS                                      (0x4)

/* Manage programs */
typedef enum
{
    /* TM: Used for both 1st and 2nd . No lookups */
    ARAD_PP_ISEM_ACCESS_PROG_VT_TM_PROG, 
    /* Unindex MPLS: ISA: VD x Initial VLAN , ISB: MPLS L1 */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VDxINITIALVID_L1,
    /* Unindex MPLS: ISA: VD x Outer Tag , ISB: MPLS 1 */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VDxOUTERVID_L1,
    /* Bridge: ISB: VD x Outer x Inner (Double Tagging) */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_INNER,
    /* Q-in-Q: ISA: High Priority(VD x OuterTag x InnerTag), ISB: Low Priority(VD x Outer Tag) */
    ARAD_PP_ISEM_ACCESS_PROG_VT_HIGH_VD_OUTER_INNER_OR_LOW_VD_OUTER,
    /* Unindex MPLS: ISA: VD x initial VLAN , ISB: MPLS L1-ELI */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VDxINITIALVID_L1_L2ELI,
    /* Unindex MPLS: ISA: VD x outer VLAN , ISB: MPLS L1-ELI */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VDxOUTERVID_L1_L2ELI,
    /* Trill */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_INITIALVID_TRILL,
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_TRILL,
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_INNER_TRILL,
    /* Unindex MPLS FRR: ISA: MPLS-FRR(L1+L2), ISB: MPLS L1, TCAM: VD x initial VLAN */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_INITIALVID_L1FRR,
    /* Unindex MPLS FRR: ISA: MPLS-FRR(L1+L2), ISB: MPLS L1, TCAM: VD x outer VLAN */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTERVID_L1FRR,
    /* Unindex MPLS Coupling: ISA: Coupling(L1+L2), ISB: Coupling(L1+L2), TCAM: VD x Initial VLAN */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_INITIALVID_L1L2,
    /* Unindex MPLS : ISA: Coupling(L1+L2), ISB: Coupling(L1+L2), TCAM: VD x Outer VLAN */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTERVID_L1L2,    
    /* Q-in-Q: ISA: VD x Outer Tag x Priority , ISB: VD x Outer Tag */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_OR_VD_OUTER_PCP,
    /* Q-in-Q: ISA: VDxOuterxInner , TCAM: VDxOuterxInnerxOuter-PCP */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_INNER_OR_VD_OUTER_INNER_OUTERPCP,
    /* Q-in-Q: ISA: VDxOuterxInner, ISB: VDxOuter, TCAM: High priority VDxOuterxInnerxOuter-PCP */
    ARAD_PP_ISEM_ACCESS_PROG_HIGH_VD_OUTER_INNER_OUTERPCP_OR_VD_OUTER_INNER_OR_VD_OUTER,
    /* Q-in-Q full DB ISA: VDxInitial-VID , ISB: VDxInitial-VID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_INITIALVID_OR_VD_INITIALVID,
    /* Q-in-Q full DB ISA: VDxOuter, ISB: VDxOuter */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_OR_VD_OUTER,
    /* Q-in-Q full DB ISA: VDxOuterxInner , ISB: VDxOuterxInner */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_OUTER_INNER_OR_VD_OUTER_INNER,
    /* Indexed MPLS: ISA: VD x Initial-VID, ISB: L1 MPLS DB */
    ARAD_PP_ISEM_ACCESS_PROG_VT_INDX_VD_INITIALVID_L1,
    /* Indexed MPLS: ISA: VD x Outer-VID, ISB: L1 MPLS DB */
    ARAD_PP_ISEM_ACCESS_PROG_VT_INDX_VD_OUTERVID_L1,
    /* Indexed MPLS FRR: ISA: FRR(L1+L2), ISB: L3, TCAM: VD x Initial-VID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_INDX_VD_INITIALVID_FRR_L1,
    /* Indexed MPLS FRR: ISA: FRR(L1+L2), ISB: L3, TCAM: VD x Outer-VID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_INDX_VD_OUTERVID_FRR_L1,
    /* Indexed MPLS and indexed-in-rif (mpls_context=interface/port_and_interface) P1: ISA: VD x Initial-VID, ISB: L3 MPLS DB */
    ARAD_PP_ISEM_ACCESS_PROG_VT_INDX_VD_INITIALVID_L3,
    /* Indexed MPLS ISA: VD x Outer-VID, ISB: L3 MPLS DB */
    ARAD_PP_ISEM_ACCESS_PROG_VT_INDX_VD_OUTERVID_L3,
    /* PON ISB: VD(3bits) x Tunnel-ID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_PON_UNTAGGED,
    /* PON ISA : VD(3bits) x Tunnel-ID x Outer-VID , ISB: VD(3bits) x Tunnel-ID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_PON_ONE_TAG,
    /* PON ISA : VD(3bits) x Tunnel-ID x Outer-VID x Inner-VID , ISB: VD(3bits) x Tunnel-ID x Outer-VID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_PON_TWO_TAGS,
    /* ISA: VD(3bits) x Tunnel-ID x Outer-VID x Inner-VID , ISB: VD (3bits) x Tunnel-ID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_PON_TWO_TAGS_VS_TUNNEL_ID,
    /* IP : for DIP IP lookup at VT for (NVGRE | VXLAN | IP-Tunnel-termination) enabled */
    ARAD_PP_ISEM_ACCESS_PROG_VT_IPV4_INITIAL_VID,
    /* IP : for DIP IP lookup at VT for (NVGRE | VXLAN | IP-Tunnel-termination) enabled ISA: VD x Outer-VID ISB: SIP */
    ARAD_PP_ISEM_ACCESS_PROG_VT_IPV4_OUTER_VID,
    /* EVB: ISB : VD x S-Channel x Initial-C-VID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_EVB_S_TAG,
    /* ISB : VD x Default-S-Channel x Initial-C-VID */
    ARAD_PP_ISEM_ACCESS_PROG_VT_EVB_UN_C_TAG,
    /* Q-in-Q: ISA: High priority (VD x Initial-VID x Inner-VLAN) Low priority (VD x Initial-VID) */
    ARAD_PP_ISEM_ACCESS_PROG_VT_DOUBLE_TAG_PRIORITY_INITIAL_VID,
    /* Trill designated-VLAN */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_DESIGNATED_VID_TRILL,
    /* Q-in-Q 5 tuple: ISA: High priority (VD x Initial-VID x Inner-VLAN) Low priority (VD x Initial-VID) TCAM: 5-tuple */
    ARAD_PP_ISEM_ACCESS_PROG_VT_DOUBLE_TAG_5_IP_TUPLE_Q_IN_Q,
    /* Single tag 5 tuple: ISB (VD x Initial-VID) TCAM: 5-tuple */
    ARAD_PP_ISEM_ACCESS_PROG_VT_SINGLE_TAG_5_IP_TUPLE_Q_IN_Q,
    /* IP-LIF-DUMMY : for DIP IP lookup at VT for (NVGRE | VXLAN | IP-Tunnel-termination) enabled */
    ARAD_PP_ISEM_ACCESS_PROG_VT_IPV4_INITIAL_VID_AFTER_RECYCLE,
    /* IP-LIF-DUMMY: for DIP IP lookup at VT for (NVGRE | VXLAN | IP-Tunnel-termination) enabled ISA: VD x Outer-VID ISB: DIP */
    ARAD_PP_ISEM_ACCESS_PROG_VT_IPV4_OUTER_VID_AFTER_RECYCLE,
	/* VT stage AC only SEM-1: lookup VD x Initial-VID, TCAM: Initial-VID x VD x Ethertype */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_INITIALVID,
	/* VT stage AC only SEM-1: lookup VD x Compressed-Outer, SEM-2: VD x Initial-VID, 
	   TCAM: Initial-VIDxVDxCompressed-OuterxPCP_DEIxEthertype */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_SINGLE_TAG,
	/* VT stage AC only SEM-1: lookup VD x Compresed-Outer x Compressed-Inner
	   SEM-2: VD x Compressed-Outer. 
	   TCAM: Initial-VIDxInner-VIDxVDxCompressed-OuterxComprssed-InnerxPCP-DEI, Ethertype */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_DOUBLE_TAG,
	/* Same as VD_InitialVID but different TCAM: lookup Explicit NULL reserved label */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_INITIALVID_EXPLICIT_NULL,
	/* Same as VD Single TAG but different TCAM: lookup Explicit NULL reserved label */ 
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_SINGLE_TAG_EXPLICIT_NULL,
	/* Same as VD Double TAG but different TCAM: lookup Explicit NULL reserved label */
    ARAD_PP_ISEM_ACCESS_PROG_VT_VD_DOUBLE_TAG_EXPLICIT_NULL,
    /* nof progs */
    ARAD_PP_ISEM_ACCESS_PROG_VT_NOF_PROGS
} ARAD_PP_ISEM_ACCESS_PROG_VT_PROGRAMS;

typedef enum
{
    /* TM: Used for both 1st and 2nd . No lookups */
    ARAD_PP_ISEM_ACCESS_PROG_TT_TM_PROG, 
    /* Unindex MPLS FRR: P1: ISB: MPLS L2 in case of not found FRR or regular process of MPLS */
    ARAD_PP_ISEM_ACCESS_PROG_TT_L2,
    /* Unindex MPLS ISB: MPLS L3 in case of found FRR, port property FRR */
    ARAD_PP_ISEM_ACCESS_PROG_TT_L3,
    /* Unindex MPLS ELI: ISB: MPLS L2+ELI in case of regular process of MPLS */
    ARAD_PP_ISEM_ACCESS_PROG_TT_L2_L3ELI,
    /* Unindex MPLS Coupling */
    ARAD_PP_ISEM_ACCESS_PROG_TT_COUPLING_L3_L4,
    /* ARP */
    ARAD_PP_ISEM_ACCESS_PROG_TT_ARP_2ND_PROG,
    /* Bridging */
    ARAD_PP_ISEM_ACCESS_PROG_TT_BRIDGE_STAR_2ND_PROG,
    /* IPV4: DIP found or dont care. */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IPV4_ROUTER_DIP_FOUND_PROG,
    /* PWEoGRE: DIP found and signal to FLP it is PWEoGRE packet */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IPV4_ROUTER_PWE_GRE_DIP_FOUND_PROG,
    /* IPV4: DIP not found. Only used in case of RPA */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IPV4_ROUTER_MC_DIP_NOT_FOUND_PROG,
    /* IPV4: DIP not found. Only used in case of RPA */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IPV4_ROUTER_UC_DIP_NOT_FOUND_PROG,
    /* IP Router */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IP_ROUTER_2ND_PROG,
    /* IPV6 */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IPV6_ROUTER_2ND_PROG,
    /* IP compatible MC */
    ARAD_PP_ISEM_ACCESS_PROG_TT_IP_ROUTER_CompatibleMc_2ND_PROG,
    /* Unknown L3 */
    ARAD_PP_ISEM_ACCESS_PROG_TT_UNKNOWN_L3_PROG,
    /* MIM: with BTAG: ISA: ISID */
    ARAD_PP_ISEM_ACCESS_PROG_TT_MAC_IN_MAC_WITH_BTAG_PROG,
    /* MIM: MC lookup : ISA: ISID */
    ARAD_PP_ISEM_ACCESS_PROG_TT_MAC_IN_MAC_MC_PROG,
    /* FCoE with VFT : ISA: FC-D-ID */
    ARAD_PP_ISEM_ACCESS_PROG_TT_FC_WITH_VFT_PROG,
    /* FCoE wo VFT : ISA: FC-D-ID */
    ARAD_PP_ISEM_ACCESS_PROG_TT_FC_PROG,
    /* MPLS indexed with RIF */
    /* ISA: L2, ISB: L1+INRIF */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L2_INRIF,
    /* ISA: L2+ELI, ISB: L1+INRIF */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L2_L3ELI_INRIF,
    /* ISA: L2+ELI, ISB: L1+INRIF */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L2ELI_INRIF,
    /* MPLS indexed */
    /* ISA: L2, ISB: L3 */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L3_L2,
    /* ISA: L3 (L2 MPLS DB), ISB: (L3 MPLS DB) L4 */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L3_L4,
    /* ISA: L2-ELI (L2 MPLS DB) */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L2_L3ELI, 
    /* MPLS tunnel termination only in TT stage { */
    /* Two lookups in SEM-A and B same Label. Done only in case MPLS1oE is defined */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L1,
    /* Two lookups in SEM-A and B same Label. Done only in case MPLS2oE and second label is GAL. */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L1_GAL,    
    /* Two lookups in SEM-A L1 and SEM-B L2 */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L2,    
    /* Two lookups in SEM-A and B same Label. Done only in case MPLS3oE and third label is GAL. */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L1_L2_GAL,
    /* Two lookups in ISEM-A and B same Label (2). Done only in case MPLS2oE and first label is NULL (VT TCAM found) */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L2_L2,
    /* Two lookups in ISEM-A L2 and B L3. Done only in case 4 labels we have and the last one is GAL. First label is NULL (VT TCAM found) */
    ARAD_PP_ISEM_ACCESS_PROG_TT_INDX_L2_L3_GAL,
    
    
    /* MPLS tunnel termination only in TT stage } */
    /* Trill */
    ARAD_PP_ISEM_ACCESS_PROG_TT_TRILL_2ND_PROG,
    ARAD_PP_ISEM_ACCESS_PROG_TT_TRILL_2ND_PROG_TWO_VLANS_AT_NATIVE_ETH,
    ARAD_PP_ISEM_ACCESS_PROG_TT_TRILL_2ND_PROG_TRILL_TRAP_PROG,    
    /* nof progs */
    ARAD_PP_ISEM_ACCESS_PROG_TT_NOF_PROGS
} ARAD_PP_ISEM_ACCESS_PROG_TT_PROGRAMS;

/* length of the key stream that
* is big enough to contain either ipv6 uc / ipvc mc / ipv4 mc
* key stream
*/
#define ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES 20
/* VTT TCAM DEFINES } */
/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_ISEM_ACCESS_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_ISEM_ACCESS_FIRST,
  ARAD_PP_ISEM_ACCESS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_ISEM_ACCESS_PROCEDURE_DESC_LAST
} ARAD_PP_ISEM_ACCESS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */
  ARAD_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR = ARAD_PP_ERR_DESC_BASE_ISEM_ACCESS_FIRST,
  ARAD_PP_ISEM_ACCESS_NOT_READY_ERR,
  ARAD_PP_ISEM_ACCESS_MPLS_IN_RIF_NOT_SUPPORTED_ERR,  
  ARAD_PP_ISEM_ACCESS_LABEL_INDEX_NOT_SUPPORTED_ERR,
  ARAD_PP_ISEM_ACCESS_LABEL_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_ISEM_ACCESS_PROG_SEL_INCORRECT_DEF_ALLOCATION_ERR,
  ARAD_PP_ISEM_ACCESS_PROG_SEL_USR_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_ISEM_ACCESS_PROG_SEL_INVALID_LIF_KEY_RANGE_ERR,
  ARAD_PP_ISEM_ACCESS_AC_KEY_SET_NOT_SUPPORTED_ERR,
  ARAD_PP_ISEM_ACCESS_SOC_PROPERTIES_ERR,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TCAM_INVALID_ERR,
  ARAD_PP_ISEM_ACCESS_PROGRAMS_FULL_ERR,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_ISEM_ACCESS_ERR_LAST
} ARAD_PP_ISEM_ACCESS_ERR;

typedef enum
{
  /* VLAN domain lookup, done nothing on isem. Configured using Port-Default */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD = 0,                                
  /* VLAN domain x Outer-VID type (includes Compression VLAN) */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID,                             
  /* VLAN domain x Outer-VID x Inner-VID type (includes Compression VLAN) */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID,                         
  /* VLAN domain x Initial-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID,                     
  /* VLAN domain x Outer-PCP x Outer-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID,                         
  /* VLAN domain x Outer-PCP x Outer-VID x Inner-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID_VID,   
  /* PON VLAN domain x Tunnel-ID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL,
  /* PON VLAN domain x Tunnel-ID x Outer-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID,
  /* PON VLAN domain x Tunnel-ID x Outer-VID x Inner-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID_VID,
  /* PON VLAN domain x Tunnel-ID x Ethertype x Outer-PCP x Outer-VID x Inner-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID,
  /* (MPLS/PWE) unindexed type */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS,                               
  /* (MPLS/PWE) indexed label outermost */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1,                            
  /* (MPLS/PWE) indexed label with In-RIF outermost */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF,                     
  /* (MPLS/PWE) indexed label 2 (one after the outermost) */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2,                            
  /* (MPLS/PWE) indexed label 3 */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3,                            
  /* Coupling - upstream label assignment */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_COUPLING,                      
  /* FRR */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_FRR,                           
  /* (MPLS/PWE) unindexed type with ELI */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_ELI,                           
  /* (MPLS/PWE) indexed label outermost with ELI */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_ELI,                        
  /* (MPLS/PWE) indexed label 2 (one after the outermost) with ELI */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2_ELI,                       
  /* (MPLS/PWE) indexed label outermost with In-RIF and ELI */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF_ELI,                
  /* ISID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB,                               
  /* SIP - Learn info Tunnel */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP,
  /* IP tunnel standard DIP in sem*/
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP,                     
  /* IP tunnel extent in TCAM */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP,
  /* TRILL NICK */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK,  
  /* TRILL Appointed Forward */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_APPOINTED_FORWARDER,
  /* TRILL native inner TPID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NATIVE_INNER_TPID,
  /* TRILL global to local VSI mapping */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_VSI,
  /* FCoE */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_FCOE,                              
  /* Spoof with IPV4 */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV4,                        
  /* Spoof with IPV6 */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV6,                         
  /* EoIP for SIP,DIP,Inner-VLAN */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_EoIP,                               
  /* GRE Key - 8 bytes */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE,                                
  /* IPv6 tunnel */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP,                                
  /* VNI Key */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI,  
  /* RPA key */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_RPA,  
  /* TRILL Designated-VID */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_DESIGNATED_VID,
  /* Flexible QinQ single tag */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_SINGLE_TAG, 
  /* Flexible QinQ double tag */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_DOUBLE_TAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_INITIAL_TAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_COMPRESSED_TAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_UNTAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_DOUBLE_TAG,                          
  /* (MPLS/PWE) indexed label 2 (one after the outermost) with GAL */
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_PWE_L2_GAL,   
  /* Explicit NULL lookup (done in TCAM) */    
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_EXPLICIT_NULL,                 
   

  ARAD_PP_ISEM_ACCESS_NOF_KEY_TYPES                                 
} ARAD_PP_ISEM_ACCESS_KEY_TYPE;

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



typedef union
{
  /* L2 Ethernet includes all L2 AC lookups */
  struct
  {
    uint32 vlan_domain;
    uint32 outer_vid; /* outer_vid/initial_vid/bvid */
    uint32 inner_vid;
    uint32 outer_pcp;
    uint32 outer_dei; /* one bit for DEI */
    int    core_id;
  } l2_eth;

  struct
  {
    uint32 in_rif;  
    uint32 label;
    uint32 label2; /* used in case of Coupling and FRR */
    uint32 is_bos; /* indication label is bos */
  } mpls;

  struct
  {
    uint32 dip;
    uint32 sip;    
    uint32 sip_prefix_len;
    uint32 dip_prefix_len;    
    SOC_SAND_PP_IPV6_SUBNET dip6;
    uint32 ipv4_next_protocol;
    uint32 ipv4_next_protocol_prefix_len;
    uint32 port_property; /* VTT Key var (only 4 lsbs) */
    uint8  port_property_en; /* VTT Key var enable - take into account */
    uint32 flags; /* used to identify the entry is IP-LIF-dummy. */
  } ip_tunnel;

  struct
  {
    uint32 nick_name;
    uint32 ing_nick;
    uint32 dist_tree;
    uint32 link_adj_id;
    uint32 native_inner_tpid;
    uint32 port;
    uint32 vsi;
    uint32 high_vid;
    uint32 low_vid;
    uint32 flags;
  } trill;

  struct
  {
    uint32 isid_domain;
    uint32 isid;
  } pbb;

  struct
  {
    uint32 gre_key;
  } l2_gre;

  struct
  {
    uint32 vni_key;
  } l2_vni;

  struct
  {
    uint32 d_id; /* FC-D_ID, only bits [23:16] will be inserted to DB */
  } fcoe;

  /* PON */
  struct
  {
    uint32 port; /* In sem converts to vlan domain */
    uint8  port_valid;
    uint32 outer_vid; 
    uint8  outer_vid_valid;
    uint32 inner_vid;
    uint8  inner_vid_valid;
    uint32 outer_pcp;
    uint8  outer_pcp_valid;
    uint32 outer_dei; 
    uint8  outer_dei_valid; 
    uint32 ether_type;
    uint8  ether_type_valid;
    uint32 tunnel_id;
    uint8  tunnel_id_valid;
    uint32 flags;
	int    core;
  } l2_pon;

  struct
  {
    SOC_SAND_PP_MAC_ADDRESS smac; /* SA , only bits [47:41] will be inserted to DB */
    uint32 sip;     
  } spoof_v4;

  struct
  {
    SOC_SAND_PP_MAC_ADDRESS smac; /* SA , only bits [47:32] will be inserted to DB */
    SOC_SAND_PP_IPV6_SUBNET sip6;
  } spoof_v6;

  struct
  {
    uint32 dip; /* only the 28 lsbs */
    uint32 vrf; /* only the 9 lsbs */
  } rpa;

} ARAD_PP_ISEM_ACCESS_KEY_INFO;

typedef struct
{
  ARAD_PP_ISEM_ACCESS_KEY_TYPE key_type;
  ARAD_PP_ISEM_ACCESS_KEY_INFO key_info;
} ARAD_PP_ISEM_ACCESS_KEY;

typedef struct
{
  uint32 sem_result_ndx;

} ARAD_PP_ISEM_ACCESS_ENTRY;

#define ARAD_PP_ISEM_ACCESS_NOF_LABEL_HANDLES 3

typedef struct
{
  int prog_used; /* prog ID in HW, -1 if not used */    
} ARAD_PP_ISEM_ACCESS_PROGRAM_INFO;


/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

extern ARAD_PP_ISEM_ACCESS_PROGRAM_INFO vt_programs[SOC_MAX_NUM_DEVICES][ARAD_PP_ISEM_ACCESS_PROG_VT_NOF_PROGS];
extern ARAD_PP_ISEM_ACCESS_PROGRAM_INFO tt_programs[SOC_MAX_NUM_DEVICES][ARAD_PP_ISEM_ACCESS_PROG_TT_NOF_PROGS];


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

void 
arad_pp_isem_access_deinit(int unit);
uint32
  arad_pp_isem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_isem_access_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY                   *isem_key,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_ENTRY                 *isem_entry,
    SOC_SAND_OUT uint8                                 *success
  );

uint32
  arad_pp_isem_access_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY                   *isem_key,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_ENTRY                 *isem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_isem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY                   *isem_key
  );

uint32
  arad_pp_isem_access_key_from_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 *buffer,
    SOC_SAND_IN  uint32                                 lookup_num,
    SOC_SAND_IN  uint32                                 tables_access_id,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_KEY                *isem_key
  );

uint32
  arad_pp_isem_access_entry_from_buffer(
    SOC_SAND_IN  uint32                                  *buffer,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_ENTRY                 *isem_entry
  );

uint32
  arad_pp_isem_access_key_to_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY                   *isem_key,
    SOC_SAND_IN  uint32                                 table_access_id,
    SOC_SAND_IN  uint32                                 insert_index, /* Inseration iteration */
    SOC_SAND_OUT uint32                                  *buffer
  );

uint32
  arad_pp_l2_lif_ac_key_to_sem_key_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_KEY                       *isem_key
  );


/* 
 * Retreive Enable / disable in rif key types (being set on init)
 */
uint32
  arad_pp_isem_access_prog_sel_in_rif_key_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *with_in_rif
  );

/* 
 * Set different ethernet programs than default configuration. 
 */
uint32
  arad_pp_isem_access_prog_sel_ac_key_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 entry_ndx,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER           *qual_key,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE            *key_mapping
  );

uint32
  arad_pp_isem_access_prog_sel_ac_key_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 entry_ndx,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER           *qual_key,
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE            *key_mapping    
  );

void
  arad_pp_isem_access_print_all_programs_data(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_isem_access_print_last_vtt_program_data
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Dump last VT&TT program invoked.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   This API must be called during a continuous stream of
 *   the identical packets coming from the same source.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_isem_access_print_last_vtt_program_data(
    SOC_SAND_IN   int                 unit
  );


/*********************************************************************
* NAME:
 *   arad_pp_isem_access_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_isem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_isem_access_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_isem_access_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_isem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_isem_access_get_errs_ptr(void);

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_ISEM_ACCESS_INCLUDED__*/
#endif


