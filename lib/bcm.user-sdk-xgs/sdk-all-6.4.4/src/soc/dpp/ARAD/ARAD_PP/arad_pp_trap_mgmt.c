
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_trap_mgmt.c,v 1.42 Broadcom SDK $
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
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP
#include <soc/mem.h>


/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <shared/swstate/sw_state_access.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX                      (ARAD_PP_NOF_TRAP_CODES-1)
#define ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN                  (1)
#define ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX                  (7)
#define ARAD_PP_TRAP_MGMT_BUFF_LEN_MAX                           (ARAD_PP_TRAP_EVENT_BUFF_MAX_SIZE)
#define ARAD_PP_TRAP_MGMT_VSI_SHIFT_MAX                          (3)
#define ARAD_PP_TRAP_MGMT_COUNTER_SELECT_MAX                     (0) /* as not relevant for ARAD */
#define ARAD_PP_TRAP_MGMT_COUNTER_ID_MAX                         (32*1024)
#define ARAD_PP_TRAP_MGMT_METER_SELECT_MAX                       (0) /* as not relevant for ARAD */
#define ARAD_PP_TRAP_MGMT_METER_COMMAND_MAX                      (3)
#define ARAD_PP_TRAP_MGMT_ETHERNET_POLICE_ID_MAX                 (2 * 1024)
#define ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_MAX                 (7)
#define ARAD_PP_TRAP_MGM_DA_TYPE_MAX                             (SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1)
#define ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_MAX             (31)
#define ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_MIN             (-31)
#define ARAD_PP_TRAP_MGMT_STRENGTH_MAX                           (7)
#define ARAD_PP_TRAP_MGMT_BITMAP_MASK_MAX                        (SOC_SAND_U32_MAX)
#define ARAD_PP_TRAP_MGMT_SNOOP_CMND_MAX                         (15)
#define ARAD_PP_TRAP_MGMT_CUD_MAX                                (0xFFFF)
#define ARAD_PP_TRAP_MGMT_TYPE_MAX                               (ARAD_PP_NOF_TRAP_MACT_EVENT_TYPES-1)
#define ARAD_PP_TRAP_MGMT_CPU_TRAP_CODE_MAX                      (ARAD_PP_NOF_TRAP_CODES-1)
#define ARAD_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_MAX                 (4095)
#define ARAD_PP_TRAP_MGMT_SRC_SYS_PORT_MAX                       (SOC_SAND_U32_MAX)
#define ARAD_PP_TRAP_MGMT_LL_HEADER_PTR_MAX                      (SOC_SAND_U32_MAX)

#define ARAD_PP_TRAP_MGMT_SNP_STRENGTH_MAX                       (PPC_TRAP_MGMT_SNP_STRENGTH_MAX)

#define ARAD_PP_TRAP_MGMT_EPNI_PMF_MIRROR_PROFILE_TABLE_INDEX_MAX  (16)

#define ARAD_PP_TRAP_NOF_UD_BLOCKS (8)

#define ARAD_PP_TRAP_MGMT_FTMH_SIZE_BYTE               (9)
#define ARAD_PP_TRAP_MGMT_FTMH_DEST_EXT_SIZE_BYTE      (2)
#define ARAD_PP_TRAP_MGMT_FTMH_LB_EXT_SIZE_BYTE        (1)
#define ARAD_PP_TRAP_MGMT_FTMH_STACKING_SIZE_BYTE      (2)
#define ARAD_PP_TRAP_MGMT_FTMH_TDM_EXT_SIZE_BYTE       (4)
#define ARAD_PP_TRAP_MGMT_PPH_SIZE_BYTE                (7)
#define ARAD_PP_TRAP_MGMT_PPH_LEARN_EXT_SIZE_BYTE      (5)
#define ARAD_PP_TRAP_MGMT_PPH_EEI_EXT_SIZE_BYTE        (3)
#define ARAD_PP_TRAP_MGMT_PPH_OAM_EXT_SIZE_BYTE        (6)



#define ARAD_PP_TRAP_MGMT_FTMH_PKT_SIZE_MSB            (0)
#define ARAD_PP_TRAP_MGMT_FTMH_PKT_SIZE_NOF_BITS       (14)
#define ARAD_PP_TRAP_MGMT_FTMH_TC_MSB                  (14)
#define ARAD_PP_TRAP_MGMT_FTMH_TC_NOF_BITS             (3)
#define ARAD_PP_TRAP_MGMT_FTMH_SRC_SYS_PORT_MSB        (17)
#define ARAD_PP_TRAP_MGMT_FTMH_SRC_SYS_PORT_NOF_BITS   (16)
#define ARAD_PP_TRAP_MGMT_FTMH_PP_DSP_MSB              (33)
#define ARAD_PP_TRAP_MGMT_FTMH_PP_DSP_NOF_BITS         (8)
#define ARAD_PP_TRAP_MGMT_FTMH_DP_MSB                  (41)
#define ARAD_PP_TRAP_MGMT_FTMH_DP_NOF_BITS             (2)
#define ARAD_PP_TRAP_MGMT_FTMH_ACTION_TYPE_MSB         (43)
#define ARAD_PP_TRAP_MGMT_FTMH_ACTION_TYPE_NOF_BITS    (2)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_MSB                 (45)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_NOF_BITS            (2)
#define ARAD_PP_TRAP_MGMT_FTMH_OUTLIF_MCID_MSB         (49)
#define ARAD_PP_TRAP_MGMT_FTMH_OUTLIF_MCID_NOF_BITS    (19)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_EXIST_MSB       (68)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_EXIST_NOF_BITS  (1)
#define ARAD_PP_TRAP_MGMT_FTMH_FIRST_EXT_MSB           (72)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_LB_KEY_NOF_BITS     (8)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_NOF_BITS        (16)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_STACKING_NOF_BITS   (16)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_LEN_NOF_BITS    (8)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_TYPE_NOF_BITS   (23)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_RESEVED_NOF_BITS  (4)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_DST_FAP_NOF_BITS  (11)
#define ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_OTM_PORT_NOF_BITS (8)

/* trap info */
#define ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_MSB            (16)
#define ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_NOF_BITS       (8)
#define ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_QUAL_MSB       (0)
#define ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_QUAL_NOF_BITS  (16)


#define ARAD_PP_TRAP_MGMT_PPH_VSI_MSB       (22)
#define ARAD_PP_TRAP_MGMT_PPH_VSI_NOF_BITS  (16)
#define ARAD_PP_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_MSB      (1)
#define ARAD_PP_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_NOF_BITS (1)
#define ARAD_PP_TRAP_MGMT_PPH_EEI_EXT_PRSNT_MSB        (0)
#define ARAD_PP_TRAP_MGMT_PPH_EEI_EXT_PRSNT_NOF_BITS   (1)
#define ARAD_PP_TRAP_MGMT_PPH_SC_MSB                   (16)
#define ARAD_PP_TRAP_MGMT_PPH_SC_NOF_BITS              (2)
#define ARAD_PP_TRAP_MGMT_PPH_UNKNOWN_ADDR_MSB         (20)
#define ARAD_PP_TRAP_MGMT_PPH_UNKNOWN_ADDR_NOF_BITS    (1)
#define ARAD_PP_TRAP_MGMT_PPH_FWD_MSB                  (4)
#define ARAD_PP_TRAP_MGMT_PPH_FWD_NOF_BITS             (4)
#define ARAD_PP_TRAP_MGMT_PPH_FWD_HDR_OFFSET_MSB       (8)
#define ARAD_PP_TRAP_MGMT_PPH_FWD_HDR_OFFSET_NOF_BITS  (7)
#define ARAD_PP_TRAP_MGMT_PPH_AC_MSB          (38) /* can be inRIF */
#define ARAD_PP_TRAP_MGMT_PPH_AC_NOF_BITS     (18)
#define ARAD_PP_TRAP_MGMT_PPH_APP_EXT_PRSNT_MSB        (2)
#define ARAD_PP_TRAP_MGMT_PPH_APP_EXT_PRSNT_NOF_BITS   (2)

#define ARAD_PP_TRAP_MGMT_OAM_TYPE_MSB                  (0)
#define ARAD_PP_TRAP_MGMT_OAM_TYPE_NOF_BITS             (2)
#define ARAD_PP_TRAP_MGMT_OAM_SUB_TYPE_MSB              (2)
#define ARAD_PP_TRAP_MGMT_OAM_SUB_TYPE_NOF_BITS         (3)
#define ARAD_PP_TRAP_MGMT_OAM_MEP_TYPE_MSB              (5)
#define ARAD_PP_TRAP_MGMT_OAM_MEP_TYPE_NOF_BITS         (1)
#define ARAD_PP_TRAP_MGMT_OAM_DATA_MSB                  (6)
#define ARAD_PP_TRAP_MGMT_OAM_DATA_NOF_BITS             (32)
#define ARAD_PP_TRAP_MGMT_OAM_1588_DATA_MSB             (38)
#define ARAD_PP_TRAP_MGMT_OAM_1588_DATA_NOF_BITS        (1)
#define ARAD_PP_TRAP_MGMT_OAM_OFFSET_MSB                (40)
#define ARAD_PP_TRAP_MGMT_OAM_OFFSET_NOF_BITS           (8)


#define ARAD_PP_TRAP_MGMT_HDR_MAX_SIZE (40)

#define ARAD_PP_TRAP_MGMT_IPV4_UC_ROUTE_VAL 	(1)
#define ARAD_PP_TRAP_MGMT_IPV4_MC_ROUTE_VAL 	(2)
#define ARAD_PP_TRAP_MGMT_IS_TRAP_VAL 			(7)

#define ARAD_INIT_DEFAULT_FWD_STR_VTT_NO_SLB                 2
#define ARAD_INIT_DEFAULT_FWD_STR_FLP_NO_SLB                 3
#define ARAD_INIT_DEFAULT_FWD_STR_SLB_NO_SLB                 1
#define ARAD_INIT_DEFAULT_FWD_STR_PMF_0_NO_SLB               4
#define ARAD_INIT_DEFAULT_FWD_STR_PMF_1_NO_SLB               5

#define ARAD_INIT_DEFAULT_FWD_STR_VTT_WITH_SLB               2
#define ARAD_INIT_DEFAULT_FWD_STR_FLP_WITH_SLB               3
#define ARAD_INIT_DEFAULT_FWD_STR_SLB_WITH_SLB               4
#define ARAD_INIT_DEFAULT_FWD_STR_PMF_0_WITH_SLB             5
#define ARAD_INIT_DEFAULT_FWD_STR_PMF_1_WITH_SLB             6


/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define ARAD_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile) \
  (valid)?(ARAD_PP_FLDS_TO_BUFF_2(valid, 1, eg_action_profile, 3)):(0)

#define ARAD_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile) \
  do { \
    valid = SOC_SAND_GET_BIT(fld_val, 3); \
    eg_action_profile = (valid)?SOC_SAND_GET_BITS_RANGE(fld_val, 2, 0):0; \
  } while (0)


#define ARAD_PP_TRAP_FILL_FIELD(__fld_ptr, __reg_num, __field_num) \
  do { \
    (__fld_ptr)->reg = __reg_num; \
    (__fld_ptr)->field = __field_num; \
  } while (0)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint32 trap_code;
  
  struct
  {
    uint32 strength;

    struct
    {
      uint8 is_oam_dest;
      /*ARAD_PP_FRWRD_DECISION_INFO frwrd_dest;*/
      uint8 add_vsi;
      uint32  vsi_shift;
    } info;
  } frwrd;

  struct
  {
    uint32 strength;

    struct
    {
      uint32 snoop_cmnd;
    } info;
  } snoop;
}ARAD_PP_TRAP_MGMT_ACTION;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static ARAD_PP_TRAP_MGMT_ACTION
  Arad_pp_trap_mgmt_action[] =
{
  {ARAD_PP_TRAP_CODE_PBP_SA_DROP_0, {0, {FALSE, FALSE, 0}}, {0, {0}}}
};


static uint32
Arad_pp_trap_mgmt_ud_start[] ={
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX|10),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX|1),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX|7),
    (ARAD_PP_TRAP_CODE_INTERNAL_OAM_PREFIX|8),
    (ARAD_PP_TRAP_CODE_INTERNAL_BLOCK_PREFIX|5),
    (ARAD_PP_TRAP_CODE_INTERNAL_USER_PREFIX|6),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX|15),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_TIMNA_PREFIX|0) /* Last traps used for ITMH.Snoop field */
};

static uint32
Arad_pp_trap_mgmt_ud_end[] ={
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX|15),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX|15),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX|15),
    (ARAD_PP_TRAP_CODE_INTERNAL_OAM_PREFIX|15),
    (ARAD_PP_TRAP_CODE_INTERNAL_BLOCK_PREFIX|7),
    (ARAD_PP_TRAP_CODE_INTERNAL_USER_PREFIX|7),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX|15),
    (ARAD_PP_TRAP_CODE_INTERNAL_IHP_TIMNA_PREFIX|15) /* Last traps used for ITMH.Snoop field */
};

static uint8
Arad_pp_fer_sb_to_internal[] =
{
  ARAD_PP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID,
  ARAD_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL,
  ARAD_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL,
  ARAD_PP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT,
  ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL,
  ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL,
  ARAD_PP_TRAP_CODE_INTERNAL_FER_SIP_TRANSPLANT_DETECTION
  };

static uint8
Arad_pp_vtt_sb_to_internal[] =
{
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_ETHER_IP_VERSION_ERROR,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_DISABLE_BRIDGE_IF_DESIGNTAED,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LIF_ENTRY,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_UNEXPECTED_BOS,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_UNEXPECTED_NO_BOS,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_1,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_PROTECTION_PATH_INVALID,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION,
  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_ADJACENT,
  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_MC_CONTINUE
};

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_trap_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_EG_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_PARSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MACT_EVENT_PARSE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MGMT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_MGMT_TRAP_CODE_TO_INTERNAL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_CPU_TRAP_CODE_FROM_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_PACKET_PARSE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_PACKET_PARSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SB_TO_TRAP_CODE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_TRAP_SB_VTT_TO_INTERNAL_TRAP_CODE_MAP_GET),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_trap_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_type_ndx' is out of range. \n\r "
    "ARAD_PP_TRAP_EG_TYPE_CFM_TRAP & ARAD_PP_TRAP_EG_TYPE_NO_VSI_TRANSLATION not supported\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'eg_action_profile' is out of range. \n\r "
    "The range is: 1 - 7 or 0xffffffff (ARAD_PP_TRAP_EG_NO_ACTION).\n\r "
    "Note 0 also is out of range as it indicates no action",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 1 - 7.\n\r "
    "Note 0 also is out of range as it indicates no action",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_BUFF_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 4 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR",
    "The parameter 'vsi_shift' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR",
    "The parameter 'counter_select' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR",
    "The parameter 'counter_id' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR",
    "The parameter 'meter_select' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR",
    "The parameter 'meter_id' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'meter_command' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'ethernet_police_id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_offset_index' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_DA_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_DA_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'da_type' is out of range. \n\r "
    "The range is: 0 - 2, MC,UC,BC.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_offset_bytes_fix' is out of range. \n\r "
    "The range is: -31 - 31.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_FRWD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_type' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'strength' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'bitmap_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_SIZE_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'size' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_TRAP_SNOOP_ACTION_SIZES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_SAMPLING_PROBABILITY_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_SAMPLING_PROBABILITY_OUT_OF_RANGE_ERR",
    "The parameter 'sampling_probability' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_cmnd' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR",
    "The parameter 'cud' is out of range. \n\r "
    "The range is: 0 - 0xFFFF.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_TRAP_MACT_EVENT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_trap_code' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_trap_qualifier' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'src_sys_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR",
    "The parameter 'll_header_ptr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    ARAD_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR,
    "ARAD_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR",
    "Trap code is not supported. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR",
    "'strength' is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR,
    "ARAD_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR",
    "More than one bit in bitmap is on.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  
  

  /*
   * Last element. Do no touch.
   */
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  arad_pp_trap_mgmt_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    table_entry[5],
    idx;
  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO
    frwrd_action_profile_info;
  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO
    snoop_action_profile_info;
  uint64 
      field_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  table_entry[4] = table_entry[1] = 0; /* all 32 relevant bits initialized to 0 */
  table_entry[3] = 0;
  table_entry[2] = table_entry[0] = 0;
  
  res = arad_fill_table_with_entry(unit, IHB_FWD_ACT_PROFILEm, MEM_BLOCK_ANY, table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (idx = 0; idx < sizeof(Arad_pp_trap_mgmt_action) / sizeof(ARAD_PP_TRAP_MGMT_ACTION); ++idx)
  {
    ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&frwrd_action_profile_info);
    frwrd_action_profile_info.bitmap_mask = ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
    frwrd_action_profile_info.strength = Arad_pp_trap_mgmt_action[idx].frwrd.strength;
    ARAD_PP_FRWRD_DECISION_DROP_SET(unit,&frwrd_action_profile_info.dest_info.frwrd_dest);
    frwrd_action_profile_info.dest_info.add_vsi = Arad_pp_trap_mgmt_action[idx].frwrd.info.add_vsi;
    frwrd_action_profile_info.dest_info.is_oam_dest = Arad_pp_trap_mgmt_action[idx].frwrd.info.is_oam_dest;
    frwrd_action_profile_info.dest_info.vsi_shift = Arad_pp_trap_mgmt_action[idx].frwrd.info.vsi_shift;
    frwrd_action_profile_info.processing_info.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
    frwrd_action_profile_info.meter_info.meter_id = 0;
    frwrd_action_profile_info.meter_info.meter_command = ARAD_PP_MTR_RES_USE_NONE;
    frwrd_action_profile_info.meter_info.meter_select = 0;
    frwrd_action_profile_info.count_info.counter_id = 0;

    res = arad_pp_trap_frwrd_profile_info_set_unsafe(
            unit,
            Arad_pp_trap_mgmt_action[idx].trap_code,
            &frwrd_action_profile_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&frwrd_action_profile_info);
    snoop_action_profile_info.strength = Arad_pp_trap_mgmt_action[idx].snoop.strength;
    snoop_action_profile_info.snoop_cmnd = Arad_pp_trap_mgmt_action[idx].snoop.info.snoop_cmnd;
    res = arad_pp_trap_snoop_profile_info_set_unsafe(
            unit,
            Arad_pp_trap_mgmt_action[idx].trap_code,
            &snoop_action_profile_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  }

  /* Block strength of forwarding. */
  {
      uint32 vtt_str;
      uint32 flp_str;
#ifdef BCM_88660_A0
      uint32 slb_str;
#endif /* BCM_88660_A0 */
      uint32 pmf_0_str;
      uint32 pmf_1_str;

      vtt_str = ARAD_INIT_DEFAULT_FWD_STR_VTT_NO_SLB;
      flp_str = ARAD_INIT_DEFAULT_FWD_STR_FLP_NO_SLB;
#ifdef BCM_88660_A0
      /* The condition is commented out only because of an 'slb_str may be used uninitialized' warning. */
      /* if (SOC_IS_ARADPLUS(unit)) { */
          slb_str = ARAD_INIT_DEFAULT_FWD_STR_SLB_NO_SLB;
      /* } */
#endif /* BCM_88660_A0 */
      pmf_0_str = ARAD_INIT_DEFAULT_FWD_STR_PMF_0_NO_SLB;
      pmf_1_str = ARAD_INIT_DEFAULT_FWD_STR_PMF_1_NO_SLB;

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, "resilient_hash_enable", 0)) {
          vtt_str = ARAD_INIT_DEFAULT_FWD_STR_VTT_WITH_SLB;
          flp_str = ARAD_INIT_DEFAULT_FWD_STR_FLP_WITH_SLB;
          slb_str = ARAD_INIT_DEFAULT_FWD_STR_SLB_WITH_SLB;
          pmf_0_str = ARAD_INIT_DEFAULT_FWD_STR_PMF_0_WITH_SLB;
          pmf_1_str = ARAD_INIT_DEFAULT_FWD_STR_PMF_1_WITH_SLB;
      }
#endif /* BCM_88660_A0 */

      vtt_str = soc_property_get(unit, "block_trap_strength_vtt", vtt_str);
      flp_str = soc_property_get(unit, "block_trap_strength_flp", flp_str);
#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit)) {
          slb_str = soc_property_get(unit, "block_trap_strength_hash", slb_str);
      }
#endif /* BCM_88660_A0 */
      pmf_0_str = soc_property_get(unit, "block_trap_strength_pmf_0", pmf_0_str);
      pmf_1_str = soc_property_get(unit, "block_trap_strength_pmf_1", pmf_1_str);
      if (!SOC_IS_ARDON(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  490,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_VTT_GENERAL_CONFIGS_1r, SOC_CORE_ALL, 0, VT_FORWARDING_STRENGTHf,  vtt_str));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  491,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_VTT_GENERAL_CONFIGS_1r, SOC_CORE_ALL, 0, TT_FORWARDING_STRENGTHf,  vtt_str));

          if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  492,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_FLP_GENERAL_CFGr, REG_PORT_ANY, 0, FORWARDING_STRENGTHf,  flp_str));
          } else {
              COMPILER_64_SET(field_val, 0, flp_str);
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  493,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IHP_FLP_GENERAL_CFGr, REG_PORT_ANY, 0, FORWARDING_STRENGTHf,  field_val));
          }
#ifdef BCM_88660_A0
          if (SOC_IS_ARADPLUS(unit)) {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  494,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_VARIABLESr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_STRENGTHf,  slb_str));
          }
#endif /* BCM_88660_A0 */
      }

      /* Should this be done after blocks ? Can the PMF change these values at init ? */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  495,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_PMF_GENERALr, SOC_CORE_ALL, 0, PMF_STRENGTH_1ST_PASSf,  pmf_0_str));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  496,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_PMF_GENERALr, SOC_CORE_ALL, 0, PMF_STRENGTH_2ND_PASSf,  pmf_1_str));
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mgmt_init_unsafe()", 0, 0);
}

  uint32
  arad_pp_trap_mgmt_trap_code_to_internal(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_CODE_INTERNAL                  *trap_code_internal,
    SOC_SAND_OUT ARAD_SOC_REG_FIELD                          *strength_fld_fwd,
    SOC_SAND_OUT ARAD_SOC_REG_FIELD                          *strength_fld_snp
  )
{
  uint32
    indx,
    diff,
    tmp;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MGMT_TRAP_CODE_TO_INTERNAL);

  strength_fld_fwd->reg = INVALIDr;
  strength_fld_snp->reg = INVALIDr;

  switch (trap_code_ndx)
  {
  case  ARAD_PP_TRAP_CODE_PBP_SA_DROP_0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_SA_DROP_0_FWDf;
    strength_fld_fwd->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_SA_DROP_0_SNPf;
    strength_fld_snp->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_SA_DROP_1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_SA_DROP_1_FWDf;
    strength_fld_fwd->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_SA_DROP_1_SNPf;
    strength_fld_snp->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_SA_DROP_2:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_SA_DROP_2_FWDf;
    strength_fld_fwd->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_SA_DROP_2_SNPf;
    strength_fld_snp->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_SA_DROP_3:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_SA_DROP_3_FWDf;
    strength_fld_fwd->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_SA_DROP_3_SNPf;
    strength_fld_snp->reg = IHP_ACTION_PROFILE_PBP_SA_DROP_MAPr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_TE_TRANSPLANT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_TE_TRANSPLANT_FWDf;
    strength_fld_fwd->reg = IHP_PBP_ACTION_PROFILESr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_TE_TRANSPLANT_SNPf;
    strength_fld_snp->reg = IHP_PBP_ACTION_PROFILESr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_TE_UNKNOWN_TUNNEL_FWDf;
    strength_fld_fwd->reg = IHP_PBP_ACTION_PROFILESr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_TE_UNKNOWN_TUNNEL_SNPf;
    strength_fld_snp->reg = IHP_PBP_ACTION_PROFILESr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_TRANSPLANT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_TE_TRANSPLANT_FWDf;
    strength_fld_fwd->reg = IHP_PBP_ACTION_PROFILESr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_TE_TRANSPLANT_SNPf;
    strength_fld_snp->reg = IHP_PBP_ACTION_PROFILESr;
    break;
  case  ARAD_PP_TRAP_CODE_PBP_LEARN_SNOOP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP;
    strength_fld_fwd->field = ACTION_PROFILE_PBP_LEARN_SNOOP_FWDf;
    strength_fld_fwd->reg = IHP_PBP_ACTION_PROFILESr;
    strength_fld_snp->field = ACTION_PROFILE_PBP_LEARN_SNOOP_SNPf;
    strength_fld_snp->reg = IHP_PBP_ACTION_PROFILESr;
    break;
  case  ARAD_PP_TRAP_CODE_SA_AUTHENTICATION_FAILED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED;
    strength_fld_fwd->field = ACTION_PROFILE_SA_AUTHENTICATION_FAILED_FWDf;
    strength_fld_fwd->reg = IHP_INITIAL_VLAN_ACTION_PROFILESr;
    strength_fld_snp->field = ACTION_PROFILE_SA_AUTHENTICATION_FAILED_SNPf;
    strength_fld_snp->reg = IHP_INITIAL_VLAN_ACTION_PROFILESr;
    break;
  case  ARAD_PP_TRAP_CODE_PORT_NOT_PERMITTED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED;
    strength_fld_fwd->field = ACTION_PROFILE_PORT_NOT_PERMITTED_FWDf;
    strength_fld_fwd->reg = IHP_INITIAL_VLAN_ACTION_PROFILESr;
    strength_fld_snp->field = ACTION_PROFILE_PORT_NOT_PERMITTED_SNPf;
    strength_fld_snp->reg = IHP_INITIAL_VLAN_ACTION_PROFILESr;
    break;
  case  ARAD_PP_TRAP_CODE_UNEXPECTED_VID:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_INITIAL_VLAN_ACTION_PROFILESr, ACTION_PROFILE_UNEXPECTED_VID_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_INITIAL_VLAN_ACTION_PROFILESr, ACTION_PROFILE_UNEXPECTED_VID_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_MULTICAST:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_FILTERING_ACTION_PROFILESr, ACTION_PROFILE_SA_MULTICAST_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_FILTERING_ACTION_PROFILESr, ACTION_PROFILE_SA_MULTICAST_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_EQUALS_DA:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_FILTERING_ACTION_PROFILESr, ACTION_PROFILE_SA_EQUALS_DA_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_FILTERING_ACTION_PROFILESr, ACTION_PROFILE_SA_EQUALS_DA_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_8021X:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_8021X;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_FILTERING_ACTION_PROFILESr, ACTION_PROFILE_8021X_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_FILTERING_ACTION_PROFILESr, ACTION_PROFILE_8021X_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_2_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_2_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_3_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr, ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_3_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_ARP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_MY_ARP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_ARPr, ACTION_PROFILE_MY_ARP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_ARPr, ACTION_PROFILE_MY_ARP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ARP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ARP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_ARPr, ACTION_PROFILE_ARP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_ARPr, ACTION_PROFILE_ARP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_IGMPr, ACTION_PROFILE_IGMP_MEMBERSHIP_QUERY_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_IGMPr, ACTION_PROFILE_IGMP_MEMBERSHIP_QUERY_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_IGMPr, ACTION_PROFILE_IGMP_REPORT_LEAVE_MSG_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_IGMPr, ACTION_PROFILE_IGMP_REPORT_LEAVE_MSG_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IGMP_UNDEFINED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_IGMPr, ACTION_PROFILE_IGMP_UNDEFINED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_IGMPr, ACTION_PROFILE_IGMP_UNDEFINED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_ICMPr, ACTION_PROFILE_ICMPV6_MLD_MC_LISTENER_QUERY_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_ICMPr, ACTION_PROFILE_ICMPV6_MLD_MC_LISTENER_QUERY_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_ICMPr, ACTION_PROFILE_ICMPV6_MLD_REPORT_DONE_MSG_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_ICMPr, ACTION_PROFILE_ICMPV6_MLD_REPORT_DONE_MSG_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ICMPV6_MLD_UNDEFINED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_ICMPr, ACTION_PROFILE_ICMPV6_MLD_UNDEFINED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_ICMPr, ACTION_PROFILE_ICMPV6_MLD_UNDEFINED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_DHCP_SERVER:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCP_SERVER_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCP_SERVER_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_DHCP_CLIENT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCP_CLIENT_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCP_CLIENT_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_DHCPV6_SERVER:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCPV6_SERVER_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCPV6_SERVER_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_DHCPV6_CLIENT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCPV6_CLIENT_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MAC_LAYER_TRAP_DHCPr, ACTION_PROFILE_DHCPV6_CLIENT_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_PROG_TRAP_0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_PROG_TRAP_1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_PROG_TRAP_2:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_PROG_TRAP_3:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_GENERAL_TRAPr, GENERAL_TRAP_ACTION_PROFILE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_PORT_NOT_VLAN_MEMBER:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_0r, PORT_NOT_VLAN_MEMBER_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_0r, PORT_NOT_VLAN_MEMBER_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_HEADER_SIZE_ERR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_0r, HEADER_SIZE_ERR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_0r, HEADER_SIZE_ERR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_0r, MY_BMAC_AND_LEARN_NULL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_0r, MY_BMAC_AND_LEARN_NULL_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_0r, MY_BMAC_UNKNOWN_ISID_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_0r, MY_BMAC_UNKNOWN_ISID_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_STP_STATE_BLOCK:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_0r, STP_STATE_BLOCK_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_0r, STP_STATE_BLOCK_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_STP_STATE_LEARN:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_1r, STP_STATE_LEARN_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_1r, STP_STATE_LEARN_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IP_COMP_MC_INVALID_IP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_1r, IP_COMP_MC_INVALID_IP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_1r, IP_COMP_MC_INVALID_IP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_MAC_AND_IP_DISABLE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_1r, MY_MAC_AND_IP_DISABLE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_1r, MY_MAC_AND_IP_DISABLE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_VERSION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_1r, TRILL_VERSION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_1r, TRILL_VERSION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_INVALID_TTL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_1r, TRILL_INVALID_TTL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_1r, TRILL_INVALID_TTL_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_CHBH:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_CHBH_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_CHBH_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_NO_REVERSE_FEC:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_NO_REVERSE_FEC_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_NO_REVERSE_FEC_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_CITE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_CITE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_CITE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_ILLEGEL_INNER_MC_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_2r, TRILL_ILLEGEL_INNER_MC_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_DISABLE_BRIDGE_IF_DESIGNATED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_DISABLE_BRIDGE_IF_DESIGNTAED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_1r, TRILL_DISABLE_BRIDGE_IF_DESIGNATED_VLAN_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_1r, TRILL_DISABLE_BRIDGE_IF_DESIGNATED_VLAN_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_2r, MY_MAC_AND_MPLS_DISABLE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_2r, MY_MAC_AND_MPLS_DISABLE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_MAC_AND_ARP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_2r, MY_MAC_AND_ARP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_2r, MY_MAC_AND_ARP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_3r, MY_MAC_AND_UNKNOWN_L_3_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_3r, MY_MAC_AND_UNKNOWN_L_3_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETHER_IP_VERSION_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_ETHER_IP_VERSION_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_3r, ETHER_IP_VERSION_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_3r, ETHER_IP_VERSION_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_LIF_PROTECT_PATH_INVALID:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_PROTECTION_PATH_INVALID;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_9r, PACKET_ACTION_PROTECTION_PATH_INVALID_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_9r, PACKET_ACTION_PROTECTION_PATH_INVALID_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_NO_ADJACENT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_ADJACENT;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_10r, TRILL_NO_ADJACENT_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_10r, TRILL_NO_ADJACENT_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_INVALID_LIF:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LIF_ENTRY;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_4r, MPLS_INVALID_LIF_ENTRY_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_4r, MPLS_INVALID_LIF_ENTRY_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_TERM_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TERMINATION_ERR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_10r, MPLS_TERMINATION_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_10r, MPLS_TERMINATION_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_UNEXPECTED_NO_BOS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_4r, MPLS_UNEXPECTED_NO_BOS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_4r, MPLS_UNEXPECTED_NO_BOS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_UNEXPECTED_BOS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_UNEXPECTED_BOS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_4r, MPLS_UNEXPECTED_BOS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_4r, MPLS_UNEXPECTED_BOS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MY_B_MAC_MC_CONTINUE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_MC_CONTINUE;
    if (!SOC_IS_ARAD_A0(unit)) {
        ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_10r, MY_BMAC_MC_CONTINUE_FWDf);
        ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_10r, MY_BMAC_MC_CONTINUE_SNPf);
    }
    break;


  case  ARAD_PP_TRAP_CODE_MPLS_TERM_TTL_0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_TTL_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_TTL_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_TERM_TTL_1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_TTL_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_TTL_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_CONTROL_WORD_TRAP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_CONTROL_WORD_TRAP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_CONTROL_WORD_DROP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_3r, MPLS_CONTROL_WORD_DROP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_FCOE_SRC_SA_MISMATCH:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_FABRIC_PROVIDED_SECURITY;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_FC_ACTION_PROFILESr, ACTION_PROFILE_FABRIC_PROVIDED_SECURITY_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_FC_ACTION_PROFILESr, ACTION_PROFILE_FABRIC_PROVIDED_SECURITY_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_FCOE_FCF_FLP_LOOKUP_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_FCF;
    break;
  case  ARAD_PP_TRAP_CODE_SIP_TRANSLANT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_SIP_TRANSPLANT_DETECTION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_SIP_TRANSPLANT_DETECTION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_SIP_TRANSPLANT_DETECTION_SNPf);
    break;

  case  ARAD_PP_TRAP_CODE_TRAP_ETH_OAM:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_ETH_OAM;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_Y1731_O_MPLS_TP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_Y1731_O_MPLSTP;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_Y1731_O_PWE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_Y1731_O_PWE;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_BFD_O_IPV4:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFD_O_IPV4;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_BFD_O_MPLS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFD_O_MPLS;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_BFD_O_PWE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFD_O_PWE;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFDCC_O_MPLSTP;
    break;
  case  ARAD_PP_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFDCV_O_MPLSTP;
    break; 
  case  ARAD_PP_TRAP_CODE_OAM_LEVEL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_LEVEL;
    break;
  case  ARAD_PP_TRAP_CODE_OAM_PASSIVE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_OAM_PASSIVE;
    break;
            
  case  ARAD_PP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_6r, IPV4_FRAGMENTED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_6r, IPV4_FRAGMENTED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ILLEGEL_PFC:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_VTT_TRAP_STRENGTHS_0r, ILLEGAL_PFC_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_VTT_TRAP_STRENGTHS_0r, ILLEGAL_PFC_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_DROP_0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_DROP_1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_DROP_2:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP2;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_2_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_2_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_DROP_3:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP3;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_3_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_DROP_MAPr, ACTION_PROFILE_SA_DROP_3_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_NOT_FOUND_0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_NOT_FOUND_1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_NOT_FOUND_2:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_2_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_2_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_SA_NOT_FOUND_3:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_3_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, ACTION_PROFILE_SA_NOT_FOUND_3_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ELK_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_FLP_GENERAL_CFGr, ACTION_PROFILE_ELK_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_FLP_GENERAL_CFGr, ACTION_PROFILE_ELK_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ELK_REJECTED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ELK_REJECTED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_FLP_GENERAL_CFGr, ACTION_PROFILE_ELK_REJECTED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_FLP_GENERAL_CFGr, ACTION_PROFILE_ELK_REJECTED_SNPf);
    break;

  case  ARAD_PP_TRAP_CODE_P2P_MISCONFIGURATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION;
    break;
  case  ARAD_PP_TRAP_CODE_ARP_FLP_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ARP;
    break;

  case  ARAD_PP_TRAP_CODE_SAME_INTERFACE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ACTION_PROFILE_SAME_INTERFACEr, ACTION_PROFILE_SAME_INTERFACE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ACTION_PROFILE_SAME_INTERFACEr, ACTION_PROFILE_SAME_INTERFACE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_UNKNOWN_UC:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC;
    break;
  case  ARAD_PP_TRAP_CODE_TRILL_UNKNOWN_MC:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC;
    break;
  case  ARAD_PP_TRAP_CODE_UC_LOOSE_RPF_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_RPF_CFGr, ACTION_PROFILE_UC_LOOSE_RPF_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_RPF_CFGr, ACTION_PROFILE_UC_LOOSE_RPF_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_DEFAULT_UCV6:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6; 
    break;
  case  ARAD_PP_TRAP_CODE_DEFAULT_MCV6:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_P2P_NO_BOS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_P2P_NO_BOS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_P2P_NO_BOS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_CONTROL_WORD_TRAP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_CONTROL_WORD_TRAP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_CONTROL_WORD_DROP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_CONTROL_WORD_DROP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_CONTROL_WORD_DROP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_UNKNOWN_LABEL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL;
    break;
    case  ARAD_PP_TRAP_CODE_MPLS_P2P_MPLSX4:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_P2P_MPLSX_4_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_LSR_ACTION_PROFILEr, ACTION_PROFILE_MPLS_P2P_MPLSX_4_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_L2CP_PEER:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_MEF_L_2_CP_PEER_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_MEF_L_2_CP_PEER_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_L2CP_DROP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_MEF_L_2_CP_DROP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_MEF_L_2_CP_DROP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_IGMP_MEMBERSHIP_QUERY_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_IGMP_MEMBERSHIP_QUERY_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_IGMP_REPORT_LEAVE_MSG_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_IGMP_REPORT_LEAVE_MSG_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_IGMP_UNDEFINED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_IGMP_UNDEFINED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_ICMPV6_MLD_REPORT_DONE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_ICMPV6_MLD_REPORT_DONE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_ICMPV6_MLD_UNDEFINED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ETHERNET_ACTION_PROFILESr, ACTION_PROFILE_ETH_FL_ICMPV6_MLD_UNDEFINED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_VERSION_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_VERSION_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_VERSION_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_CHECKSUM_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_CHECKSUM_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_CHECKSUM_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_HEADER_LENGTH_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_HEADER_LENGTH_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_TOTAL_LENGTH_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_TOTAL_LENGTH_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_TTL0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_TTL_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_TTL_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_HAS_OPTIONS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_HAS_OPTIONS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_HAS_OPTIONS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_TTL1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_TTL_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_TTL_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_SIP_EQUAL_DIP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_SIP_EQUAL_DIP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_SIP_EQUAL_DIP_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_DIP_ZERO:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_DIP_ZERO_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_DIP_ZERO_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV4_SIP_IS_MC:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_SIP_IS_MC_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV4_ACTION_PROFILESr, ACTION_PROFILE_IPV4_SIP_IS_MC_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_VERSION_ERROR:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_VERSION_ERROR_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_VERSION_ERROR_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_HOP_COUNT0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_HOP_COUNT_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_HOP_COUNT_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_HOP_COUNT1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_HOP_COUNT_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_HOP_COUNT_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_UNSPECIFIED_DESTINATION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_UNSPECIFIED_DESTINATION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_LOOPBACK_ADDRESS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_LOOPBACK_ADDRESS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_MULTICAST_SOURCE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_MULTICAST_SOURCE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_MULTICAST_SOURCE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_NEXT_HEADER_NULL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_NEXT_HEADER_NULL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_NEXT_HEADER_NULL_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_UNSPECIFIED_SOURCE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_UNSPECIFIED_SOURCE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_LINK_LOCAL_DESTINATION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_LINK_LOCAL_DESTINATION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_SITE_LOCAL_DESTINATION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_SITE_LOCAL_DESTINATION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_LINK_LOCAL_SOURCE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_LINK_LOCAL_SOURCE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_SITE_LOCAL_SOURCE_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_0r, ACTION_PROFILE_IPV6_SITE_LOCAL_SOURCE_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_1r, ACTION_PROFILE_IPV6_IPV4_COMPATIBLE_DESTINATION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_1r, ACTION_PROFILE_IPV6_IPV4_COMPATIBLE_DESTINATION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_1r, ACTION_PROFILE_IPV6_IPV4_MAPPED_DESTINATION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_1r, ACTION_PROFILE_IPV6_IPV4_MAPPED_DESTINATION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_IPV6_MULTICAST_DESTINATION:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_IPV6_ACTION_PROFILES_1r, ACTION_PROFILE_IPV6_MULTICAST_DESTINATION_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_IPV6_ACTION_PROFILES_1r, ACTION_PROFILE_IPV6_MULTICAST_DESTINATION_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_TTL0:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MPLS_ACTION_PROFILESr, ACTION_PROFILE_MPLS_TTL_0_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MPLS_ACTION_PROFILESr, ACTION_PROFILE_MPLS_TTL_0_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MPLS_TTL1:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_MPLS_ACTION_PROFILESr, ACTION_PROFILE_MPLS_TTL_1_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_MPLS_ACTION_PROFILESr, ACTION_PROFILE_MPLS_TTL_1_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TCP_SN_FLAGS_ZERO:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_SN_FLAGS_ZERO_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_SN_FLAGS_ZERO_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_SN_ZERO_FLAGS_SET_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_SN_ZERO_FLAGS_SET_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TCP_SYN_FIN:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_SYN_FIN_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_SYN_FIN_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TCP_EQUAL_PORTS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_EQUAL_PORTS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_EQUAL_PORTS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_FRAGMENT_INCOMPLETE_HEADER_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_FRAGMENT_INCOMPLETE_HEADER_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_FRAGMENT_OFFSET_LT_8_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_TCP_ACTION_PROFILESr, ACTION_PROFILE_TCP_FRAGMENT_OFFSET_LT_8_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_UDP_EQUAL_PORTS:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_UDP_ACTION_PROFILESr, ACTION_PROFILE_UDP_EQUAL_PORTS_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_UDP_ACTION_PROFILESr, ACTION_PROFILE_UDP_EQUAL_PORTS_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ICMP_DATA_GT_576:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ICMP_ACTION_PROFILESr, ACTION_PROFILE_ICMP_DATA_GT_576_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ICMP_ACTION_PROFILESr, ACTION_PROFILE_ICMP_DATA_GT_576_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_ICMP_FRAGMENTED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHP_ICMP_ACTION_PROFILESr, ACTION_PROFILE_ICMP_FRAGMENTED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHP_ICMP_ACTION_PROFILESr, ACTION_PROFILE_ICMP_FRAGMENTED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_GENERAL: 
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_PMF_GENERAL;
    break;
  case  ARAD_PP_TRAP_CODE_FACILITY_INVALID:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_FACILITY_INVALID_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_FACILITY_INVALID_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_FEC_ENTRY_ACCESSED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_FEC_ENTRY_ACCESSED_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_FEC_ENTRY_ACCESSED_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_UC_STRICT_RPF_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_RPFr, ACTION_PROFILE_UC_STRICT_RPF_FAIL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_RPFr, ACTION_PROFILE_UC_STRICT_RPF_FAIL_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_RPFr, ACTION_PROFILE_MC_EXPLICIT_RPF_FAIL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_RPFr, ACTION_PROFILE_MC_EXPLICIT_RPF_FAIL_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_RPFr, ACTION_PROFILE_MC_USE_SIP_RPF_FAIL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_RPFr, ACTION_PROFILE_MC_USE_SIP_RPF_FAIL_SNPf);
    break;

/* Arad not supported
  case  ARAD_PP_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_MC_RPFr, ACTION_PROFILE_MC_USE_SIP_RPF_FAIL_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_MC_RPFr, ACTION_PROFILE_MC_USE_SIP_RPF_FAIL_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_MC_USE_SIP_ECMP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_MC_RPFr, ACTION_PROFILE_MC_USE_SIP_ECMP_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_MC_RPFr, ACTION_PROFILE_MC_USE_SIP_ECMP_SNPf);
    break;
 */
  case  ARAD_PP_TRAP_CODE_ICMP_REDIRECT:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT;
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_ICMP_REDIRECT_FWDf);
    ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_GENERALr, ACTION_PROFILE_ICMP_REDIRECT_SNPf);
    break;
  case  ARAD_PP_TRAP_CODE_USER_OAMP:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_USER_OAMP;
    break;
  case  ARAD_PP_TRAP_CODE_USER_ETH_OAM_ACCELERATED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED:
    *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED;
    break;

  default:
    /* ARAD_PP_TRAP_CODE_RESERVED_MC_0-7 */
    if ((trap_code_ndx >= ARAD_PP_TRAP_CODE_RESERVED_MC_0) && (trap_code_ndx <= ARAD_PP_TRAP_CODE_RESERVED_MC_7))
    {
      diff = (trap_code_ndx - ARAD_PP_TRAP_CODE_RESERVED_MC_0);
      *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX + diff;
      /* fwd/snp strength for reserved_mc is not per trap_code_ndx,
         but per reserved_mc DA and trap profile (see reserved mc table) */
    }
    /* ARAD_PP_TRAP_CODE_1588_PACKET_0-7 */
    else if ((trap_code_ndx >= ARAD_PP_TRAP_CODE_1588_PACKET_0) && (trap_code_ndx <= ARAD_PP_TRAP_CODE_1588_PACKET_7))
    {
      diff = (trap_code_ndx - ARAD_PP_TRAP_CODE_1588_PACKET_0);
      *trap_code_internal = ARAD_PP_TRAP_CODE_INTERNAL_FLP_IEEE_1588_PREFIX + diff;
      switch(trap_code_ndx) 
      {
      case ARAD_PP_TRAP_CODE_1588_PACKET_0:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_0_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_0_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_1:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_1_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_1_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_2:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_2_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_2_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_3:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_3_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_3_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_4:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_4_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_4_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_5:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_5_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_5_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_6:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_6_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_6_SNPf);
      break;
      case ARAD_PP_TRAP_CODE_1588_PACKET_7:
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_fwd, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_7_FWDf);
          ARAD_PP_TRAP_FILL_FIELD(strength_fld_snp, IHB_ACTION_PROFILE_IEEE_1588r, INDEX_7_SNPf);
      break;
      /* must have default. Otherrwise, compilation error */
      /* coverity[dead_error_begin : FALSE] */
      default:
          break;
      }
    }
    /* ARAD_PP_TRAP_CODE_USER_DEFINED_0-59 */
    else if ((trap_code_ndx >= ARAD_PP_TRAP_CODE_USER_DEFINED_0) && (trap_code_ndx <= SOC_PPC_TRAP_CODE_USER_DEFINED_LAST))
    {
      tmp = trap_code_ndx - ARAD_PP_TRAP_CODE_USER_DEFINED_0;

      for(indx =0; indx < ARAD_PP_TRAP_NOF_UD_BLOCKS; ++indx) 
      {
          if(tmp <= Arad_pp_trap_mgmt_ud_end[indx] - Arad_pp_trap_mgmt_ud_start[indx])
          {
              *trap_code_internal = tmp + Arad_pp_trap_mgmt_ud_start[indx];
              break;
          }
          tmp -= (Arad_pp_trap_mgmt_ud_end[indx] - Arad_pp_trap_mgmt_ud_start[indx] + 1);
      }
      if(indx == ARAD_PP_TRAP_NOF_UD_BLOCKS) 
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR, 90, exit);
      }
    }
    else /* Unknown */
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR, 100, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mgmt_trap_code_to_internal()", trap_code_ndx, 0);
}


STATIC
 int
  arad_pp_trap_mgmt_trap_internal_to_reg_index(
    SOC_SAND_IN  ARAD_PP_TRAP_CODE_INTERNAL                  trap_code_internal
  )
{
    /* Special cases, with register array */
    if ((ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0 <= trap_code_internal) 
        && (trap_code_internal <= ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3)) {
        return (trap_code_internal - ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0);
    }
    else {
        return 0;
    }

}

/************************************************************************
*   Convert CPU trap code as received on packet to API enumerator.     *
************************************************************************/
uint32
  arad_pp_trap_cpu_trap_code_from_internal_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int32                                 trap_code_internal,
    SOC_SAND_OUT ARAD_PP_TRAP_CODE                        *trap_code
  )
{
  uint32
    tmp,
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_CPU_TRAP_CODE_FROM_INTERNAL_UNSAFE);

  switch (trap_code_internal)
  {
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_SA_DROP_0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_SA_DROP_1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_SA_DROP_2;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_SA_DROP_3;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_TE_TRANSPLANT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_TRANSPLANT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP:
    *trap_code = ARAD_PP_TRAP_CODE_PBP_LEARN_SNOOP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED:
    *trap_code = ARAD_PP_TRAP_CODE_SA_AUTHENTICATION_FAILED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED:
    *trap_code = ARAD_PP_TRAP_CODE_PORT_NOT_PERMITTED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID:
    *trap_code = ARAD_PP_TRAP_CODE_UNEXPECTED_VID;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST:
    *trap_code = ARAD_PP_TRAP_CODE_SA_MULTICAST;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA:
    *trap_code = ARAD_PP_TRAP_CODE_SA_EQUALS_DA;
    break;
  case  ARAD_PP_TRAP_CODE_8021X:
    *trap_code = ARAD_PP_TRAP_CODE_INTERNAL_LLR_8021X;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0:
    *trap_code = ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1:
    *trap_code = ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2:
    *trap_code = ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3:
    *trap_code = ARAD_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_MY_ARP:
    *trap_code = ARAD_PP_TRAP_CODE_MY_ARP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ARP:
    *trap_code = ARAD_PP_TRAP_CODE_ARP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY:
    *trap_code = ARAD_PP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG:
    *trap_code = ARAD_PP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED:
    *trap_code = ARAD_PP_TRAP_CODE_IGMP_UNDEFINED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code = ARAD_PP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG:
    *trap_code = ARAD_PP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED:
    *trap_code = ARAD_PP_TRAP_CODE_ICMPV6_MLD_UNDEFINED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER:
    *trap_code = ARAD_PP_TRAP_CODE_DHCP_SERVER;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT:
    *trap_code = ARAD_PP_TRAP_CODE_DHCP_CLIENT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER:
    *trap_code = ARAD_PP_TRAP_CODE_DHCPV6_SERVER;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT:
    *trap_code = ARAD_PP_TRAP_CODE_DHCPV6_CLIENT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0:
    *trap_code = ARAD_PP_TRAP_CODE_PROG_TRAP_0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1:
    *trap_code = ARAD_PP_TRAP_CODE_PROG_TRAP_1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2:
    *trap_code = ARAD_PP_TRAP_CODE_PROG_TRAP_2;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3:
    *trap_code = ARAD_PP_TRAP_CODE_PROG_TRAP_3;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER:
    *trap_code = ARAD_PP_TRAP_CODE_PORT_NOT_VLAN_MEMBER;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR:
    *trap_code = ARAD_PP_TRAP_CODE_HEADER_SIZE_ERR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL:
    *trap_code = ARAD_PP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID:
    *trap_code = ARAD_PP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK:
    *trap_code = ARAD_PP_TRAP_CODE_STP_STATE_BLOCK;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN:
    *trap_code = ARAD_PP_TRAP_CODE_STP_STATE_LEARN;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP:
    *trap_code = ARAD_PP_TRAP_CODE_IP_COMP_MC_INVALID_IP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE:
    *trap_code = ARAD_PP_TRAP_CODE_MY_MAC_AND_IP_DISABLE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_VERSION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_INVALID_TTL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_CHBH;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_NO_REVERSE_FEC;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_CITE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_DISABLE_BRIDGE_IF_DESIGNTAED:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_DISABLE_BRIDGE_IF_DESIGNATED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE:
    *trap_code = ARAD_PP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP:
    *trap_code = ARAD_PP_TRAP_CODE_MY_MAC_AND_ARP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3:
    *trap_code = ARAD_PP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_ETHER_IP_VERSION_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_ETHER_IP_VERSION_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_PROTECTION_PATH_INVALID:
    *trap_code = ARAD_PP_TRAP_CODE_LIF_PROTECT_PATH_INVALID;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_ADJACENT:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_NO_ADJACENT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LIF_ENTRY:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_INVALID_LIF;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TERMINATION_ERR:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TERM_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_UNEXPECTED_NO_BOS:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_UNEXPECTED_BOS:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_UNEXPECTED_BOS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_MC_CONTINUE:
    *trap_code = ARAD_PP_TRAP_CODE_MY_B_MAC_MC_CONTINUE;
    break;



  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TERM_TTL_0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_1:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TERM_TTL_1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_FABRIC_PROVIDED_SECURITY:
    *trap_code = ARAD_PP_TRAP_CODE_FCOE_SRC_SA_MISMATCH;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_FCF:
    *trap_code = ARAD_PP_TRAP_CODE_FCOE_FCF_FLP_LOOKUP_FAIL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_SIP_TRANSPLANT_DETECTION:
    *trap_code = ARAD_PP_TRAP_CODE_SIP_TRANSLANT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_ETH_OAM:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_ETH_OAM;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_Y1731_O_MPLSTP:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_Y1731_O_MPLS_TP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_Y1731_O_PWE:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_Y1731_O_PWE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFD_O_IPV4:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_BFD_O_IPV4;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFD_O_MPLS:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_BFD_O_MPLS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFD_O_PWE:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_BFD_O_PWE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFDCC_O_MPLSTP:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_BFDCV_O_MPLSTP:
    *trap_code = ARAD_PP_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_LEVEL:
    *trap_code= ARAD_PP_TRAP_CODE_OAM_LEVEL;
    break;    
  case  ARAD_PP_TRAP_CODE_INTERNAL_OAM_PASSIVE:
    *trap_code= ARAD_PP_TRAP_CODE_OAM_PASSIVE;
    break;

  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC:
    *trap_code = ARAD_PP_TRAP_CODE_ILLEGEL_PFC;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP0:
    *trap_code = ARAD_PP_TRAP_CODE_SA_DROP_0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP1:
    *trap_code = ARAD_PP_TRAP_CODE_SA_DROP_1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP2:
    *trap_code = ARAD_PP_TRAP_CODE_SA_DROP_2;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP3:
    *trap_code = ARAD_PP_TRAP_CODE_SA_DROP_3;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0:
    *trap_code = ARAD_PP_TRAP_CODE_SA_NOT_FOUND_0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1:
    *trap_code = ARAD_PP_TRAP_CODE_SA_NOT_FOUND_1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2:
    *trap_code = ARAD_PP_TRAP_CODE_SA_NOT_FOUND_2;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3:
    *trap_code = ARAD_PP_TRAP_CODE_SA_NOT_FOUND_3;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_ELK_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ELK_REJECTED:
    *trap_code = ARAD_PP_TRAP_CODE_ELK_REJECTED;
    break;

  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION:
    *trap_code = ARAD_PP_TRAP_CODE_P2P_MISCONFIGURATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ARP:
    *trap_code = ARAD_PP_TRAP_CODE_ARP_FLP_FAIL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE:
    *trap_code = ARAD_PP_TRAP_CODE_SAME_INTERFACE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_UNKNOWN_UC;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC:
    *trap_code = ARAD_PP_TRAP_CODE_TRILL_UNKNOWN_MC;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL:
    *trap_code = ARAD_PP_TRAP_CODE_UC_LOOSE_RPF_FAIL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6:
    *trap_code = ARAD_PP_TRAP_CODE_DEFAULT_UCV6;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6:
    *trap_code = ARAD_PP_TRAP_CODE_DEFAULT_MCV6;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_P2P_NO_BOS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_CONTROL_WORD_DROP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_UNKNOWN_LABEL;
    break;
    case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_P2P_MPLSX4;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_L2CP_PEER;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_L2CP_DROP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED:
    *trap_code = ARAD_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_VERSION_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_CHECKSUM_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_TTL0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_HAS_OPTIONS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_TTL1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_SIP_EQUAL_DIP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_DIP_ZERO;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC:
    *trap_code = ARAD_PP_TRAP_CODE_IPV4_SIP_IS_MC;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_VERSION_ERROR;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_HOP_COUNT0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_HOP_COUNT1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_MULTICAST_SOURCE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_NEXT_HEADER_NULL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION:
    *trap_code = ARAD_PP_TRAP_CODE_IPV6_MULTICAST_DESTINATION;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TTL0;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1:
    *trap_code = ARAD_PP_TRAP_CODE_MPLS_TTL1;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO:
    *trap_code = ARAD_PP_TRAP_CODE_TCP_SN_FLAGS_ZERO;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET:
    *trap_code = ARAD_PP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN:
    *trap_code = ARAD_PP_TRAP_CODE_TCP_SYN_FIN;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS:
    *trap_code = ARAD_PP_TRAP_CODE_TCP_EQUAL_PORTS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER:
    *trap_code = ARAD_PP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8:
    *trap_code = ARAD_PP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS:
    *trap_code = ARAD_PP_TRAP_CODE_UDP_EQUAL_PORTS;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576:
    *trap_code = ARAD_PP_TRAP_CODE_ICMP_DATA_GT_576;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED:
    *trap_code = ARAD_PP_TRAP_CODE_ICMP_FRAGMENTED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_PMF_GENERAL:
    *trap_code = ARAD_PP_TRAP_CODE_GENERAL; ;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID:
    *trap_code = ARAD_PP_TRAP_CODE_FACILITY_INVALID;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED:
    *trap_code = ARAD_PP_TRAP_CODE_FEC_ENTRY_ACCESSED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL:
    *trap_code = ARAD_PP_TRAP_CODE_UC_STRICT_RPF_FAIL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL:
    *trap_code = ARAD_PP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL:
    *trap_code = ARAD_PP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL;
    break;

/* Arad not supported
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL:
    *trap_code = ARAD_PP_TRAP_CODE_MC_USE_SIP_RPF_FAIL;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP:
    *trap_code = ARAD_PP_TRAP_CODE_MC_USE_SIP_ECMP;
    break;
 */
  case  ARAD_PP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT:
    *trap_code = ARAD_PP_TRAP_CODE_ICMP_REDIRECT;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_USER_OAMP:
    *trap_code = ARAD_PP_TRAP_CODE_USER_OAMP;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED:
    *trap_code = ARAD_PP_TRAP_CODE_USER_ETH_OAM_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED:
    *trap_code = ARAD_PP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    *trap_code = ARAD_PP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED:
    *trap_code = ARAD_PP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED;
    break;
  case  ARAD_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED:
    *trap_code = ARAD_PP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED;
    break;

  default:
    /* ARAD_PP_TRAP_CODE_RESERVED_MC_0-7 */
    if ((trap_code_internal >= ARAD_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX) && (trap_code_internal <= ARAD_PP_TRAP_CODE_RESERVED_MC_0+7))
    {
      *trap_code = ARAD_PP_TRAP_CODE_RESERVED_MC_0 + trap_code_internal - ARAD_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX;
    }
    /* ARAD_PP_TRAP_CODE_UNKNOWN_DA_0-7 */
    else if ((trap_code_internal >= ARAD_PP_TRAP_CODE_INTERNAL_FLP_IEEE_1588_PREFIX) && (trap_code_internal <= ARAD_PP_TRAP_CODE_INTERNAL_FLP_IEEE_1588_PREFIX+7))
    {
      *trap_code = ARAD_PP_TRAP_CODE_1588_PACKET_0 + trap_code_internal - ARAD_PP_TRAP_CODE_INTERNAL_FLP_IEEE_1588_PREFIX;
    }
    /* ARAD_PP_TRAP_CODE_USER_DEFINED_0-59 */
    else
    {
      tmp = ARAD_PP_TRAP_CODE_USER_DEFINED_0;

      for(indx =0; indx < ARAD_PP_TRAP_NOF_UD_BLOCKS; ++indx) 
      {
          if(trap_code_internal <= Arad_pp_trap_mgmt_ud_end[indx] && trap_code_internal >= Arad_pp_trap_mgmt_ud_start[indx])
          {
              *trap_code = tmp + (trap_code_internal - Arad_pp_trap_mgmt_ud_start[indx]);
              break;
          }
          tmp += (Arad_pp_trap_mgmt_ud_end[indx] - Arad_pp_trap_mgmt_ud_start[indx] + 1);
      }
      if(indx == ARAD_PP_TRAP_NOF_UD_BLOCKS) 
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR, 90, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mgmt_trap_code_to_internal()", trap_code_internal, 0);
}


uint32
  arad_pp_trap_sb_vtt_to_internal_trap_code_map_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN   uint32            sb_trap_indx,
    SOC_SAND_OUT  uint32            *internal_trap_code
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SB_VTT_TO_INTERNAL_TRAP_CODE_MAP_GET);
  SOC_SAND_CHECK_NULL_INPUT(internal_trap_code);
  if(sb_trap_indx >= (sizeof(Arad_pp_vtt_sb_to_internal) / sizeof(uint8)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  *internal_trap_code = Arad_pp_vtt_sb_to_internal[sb_trap_indx];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_trap_sb_vtt_to_internal_trap_code_map_get()",0,0);
}

uint32
  arad_pp_trap_sb_to_trap_code_map_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN   uint32                   rng_num,
    SOC_SAND_IN   uint32                   sb_trap_indx,
    SOC_SAND_OUT  ARAD_PP_TRAP_CODE            *trap_code
  )
{
  uint32
    res;
  uint32
    inter_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SB_TO_TRAP_CODE_MAP_GET);
  SOC_SAND_CHECK_NULL_INPUT(trap_code);

  switch(rng_num)
  {
    case 0:/* llr*/ /* 1 to 1 no need to map*/
      inter_val = ARAD_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + sb_trap_indx;
    break;
    case 1:
      res = arad_pp_trap_sb_vtt_to_internal_trap_code_map_get(unit,sb_trap_indx,&inter_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
    case 2:/* 1 to 1 no need to map*/
      inter_val = ARAD_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + sb_trap_indx;
    break;
    default:
    case 3:
      inter_val = Arad_pp_fer_sb_to_internal[sb_trap_indx];
    break;
  }
  res = arad_pp_trap_cpu_trap_code_from_internal_unsafe(
          unit,
          inter_val,
          trap_code
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_trap_sb_to_trap_code_map_get()",0,0);
}
/*********************************************************************
*     Fill information required to parse packet arriving to CPU.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_trap_packet_parse_info_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT ARAD_PP_TRAP_MGMT_PACKET_PARSE_INFO  *packet_parse_info
  )
{

  soc_error_t rv;
  uint8 system_headers_mode;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_PACKET_PARSE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(packet_parse_info);
 
  SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO_clear(packet_parse_info);

  rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.system_headers_mode.get(unit, &system_headers_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  if(system_headers_mode != ARAD_PP_SYSTEM_HEADERS_MODE_ARAD  &&  system_headers_mode !=ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.ftmh_lb_key_ext_en.get(unit, &(packet_parse_info->lb_ext_en));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
  rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.ftmh_stacking_ext_enable.get(unit, &(packet_parse_info->stacking_enabled));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);

  /* always LSB to MSB according to CMIC*/
  packet_parse_info->recv_mode = SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB;

  /* Get the user-header size */
#ifndef BCM_ARAD_PARSE_PACKET_IN_INTERRUPT_CONTEXT
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(rv,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_PP_CONFIGr, SOC_CORE_ALL, 0, NETWORK_OFFSETf, &packet_parse_info->user_header_size));
#endif

  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in trap_packet_parse_info_get_unsafe()",0,0);
}

uint32
  arad_pp_trap_packet_parse_oam_ts(
    SOC_SAND_IN  int							      unit,
    SOC_SAND_IN  uint8                                hdr_buff[],
    SOC_SAND_IN  ARAD_PP_TRAP_MGMT_PACKET_PARSE_INFO  *packet_parse_info,
    SOC_SAND_OUT ARAD_PP_TRAP_PACKET_INFO             *packet_info
  )
{
    uint32
      res = SOC_SAND_OK,
     header_ptr;
    uint32
        fld_val,
        fld_val1;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_PACKET_PARSE_OAM);


    header_ptr = 0;
    /* type */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_OAM_TYPE_MSB,
            ARAD_PP_TRAP_MGMT_OAM_TYPE_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    packet_info->oam_type = fld_val;


    /* offset */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_OAM_OFFSET_MSB,
            ARAD_PP_TRAP_MGMT_OAM_OFFSET_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
    packet_info->oam_offset = fld_val;


    /* sub_type / TP cmd */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_OAM_SUB_TYPE_MSB,
            ARAD_PP_TRAP_MGMT_OAM_SUB_TYPE_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    /* MEP_type / TS-Encaps */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_OAM_MEP_TYPE_MSB,
            ARAD_PP_TRAP_MGMT_OAM_MEP_TYPE_NOF_BITS,
            &fld_val1
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

    switch (packet_info->oam_type)
    {
    case 0:  /* OAM  */
        packet_info->oam_sub_type = fld_val;
        packet_info->mep_type = fld_val1;
        break;
    case 1: /* 1588v2 */
        packet_info->oam_tp_cmd = fld_val;
        packet_info->oam_ts_encaps = fld_val1;
        break;
    default:
        break;
    }

    /* Data - 32 bits */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_OAM_DATA_MSB,
            ARAD_PP_TRAP_MGMT_OAM_DATA_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    packet_info->oam_data = fld_val;

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_OAM_1588_DATA_MSB,
            ARAD_PP_TRAP_MGMT_OAM_1588_DATA_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
    packet_info->ext_mac_1588=fld_val;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_packet_parse_oam()", 0, 0);
    
}


uint32
  arad_pp_trap_packet_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                *buff,
    SOC_SAND_IN  uint32                               buff_len,
    SOC_SAND_IN  ARAD_PP_TRAP_MGMT_PACKET_PARSE_INFO  *packet_parse_info,
    SOC_SAND_OUT ARAD_PP_TRAP_PACKET_INFO             *packet_info
  )
{
  uint32
    res = SOC_SAND_OK,
    header_ptr,
    ext_msb;
  uint8
    hdr_buff[ARAD_PP_TRAP_MGMT_HDR_MAX_SIZE],
	is_routed;
  uint32
    fld_val,
    eei_ext = 0,
    app_ext = 0,
    ac_present=FALSE,
    pph_type = 0,
    indx,
    hdr_size,
    dsp_ext_exist=0;
  ARAD_PP_TRAP_CODE
    trap_code;

  uint32  trap_id;
  uint32  dest_system_port;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_PACKET_PARSE_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff);
  SOC_SAND_CHECK_NULL_INPUT(packet_info);
  SOC_SAND_CHECK_NULL_INPUT(packet_parse_info);


  ARAD_PP_TRAP_PACKET_INFO_clear(packet_info);

  trap_id = 0;
  dest_system_port =0;

  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "trap_packet_parse_test", 0)){
    res = arad_pp_oam_oamp_error_trap_id_and_destination_get(unit, SOC_PPD_OAM_OAMP_RMEP_ERR, &trap_id, &dest_system_port);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
	dest_system_port = dest_system_port & 0x3FFFF;
  }

  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "trap_packet_parse_test", 0)){
	    if ( dest_system_port != packet_info->otm_port) {
		  /* If D_OAMP_CPU_PORT then its Second Pass parsing, skip clear */
		  ARAD_PP_TRAP_PACKET_INFO_clear(packet_info);
		}
  }

  if (buff_len == 0)
  {
    packet_info->size_in_bytes = 0;
    goto exit;
  }

  /* header to examine */
  hdr_size = SOC_SAND_MIN(ARAD_PP_TRAP_MGMT_HDR_MAX_SIZE, buff_len);
  /* align header byte order, only for examined header */
  if (packet_parse_info->recv_mode == SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
  {
    for (indx = 0; indx < hdr_size; ++indx)
    {
      hdr_buff[indx] = buff[buff_len - indx - 1];
    }
  }
  else
  {
    sal_memcpy(hdr_buff,buff,hdr_size);
  }

  packet_info->frwrd_type = ARAD_PP_TRAP_MGMT_PKT_FRWRD_TYPE_NORMAL;

  header_ptr = 0;
  /*
   * FTMH
   */
  if (header_ptr + ARAD_PP_TRAP_MGMT_FTMH_SIZE_BYTE > buff_len)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 5, exit);
  }

   /* Advance header ptr to end of FTMH. Note that during this functions, header_ptr
     always points to the beginning of the next header (FTMH/PPH), since it used as
     an offset in hdr_buff, when reading fields from the header, and packet_info->ll_header_ptr
     is used to add the different extensions, if exist */
  packet_info->ntwrk_header_ptr = ARAD_PP_TRAP_MGMT_FTMH_SIZE_BYTE;

  /* packet size */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_PKT_SIZE_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_PKT_SIZE_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  packet_info->size_in_bytes = fld_val;
  /* action type */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_ACTION_TYPE_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_ACTION_TYPE_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  switch(fld_val)
  {
  case 0:
      packet_info->frwrd_type = ARAD_PP_TRAP_MGMT_PKT_FRWRD_TYPE_NORMAL;
  break;
  case 1:
      packet_info->frwrd_type = ARAD_PP_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP;
  break;
  case 2:
      packet_info->frwrd_type = ARAD_PP_TRAP_MGMT_PKT_FRWRD_TYPE_IN_MIRROR;
  break;
  }
  
  /* Source system port */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_SRC_SYS_PORT_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_SRC_SYS_PORT_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  packet_info->src_sys_port = fld_val;
 /* TC */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_TC_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_TC_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
  packet_info->tc = fld_val;
  /* DP */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_DP_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_DP_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
  packet_info->dp = fld_val;
  /* PP_DSP */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_PP_DSP_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_PP_DSP_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  


  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "trap_packet_parse_test", 0)){
	  if ( dest_system_port != packet_info->otm_port) {
		  /* Skip this setting if we are parsing second time for OAMP */
		  packet_info->otm_port = fld_val;
	  }
  }
  else
  {
	  packet_info->otm_port = fld_val;
  }
  

  /* Outlif or MCid */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_FTMH_OUTLIF_MCID_MSB,
          ARAD_PP_TRAP_MGMT_FTMH_OUTLIF_MCID_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  packet_info->outlif_mcid = fld_val;


  ext_msb = ARAD_PP_TRAP_MGMT_FTMH_FIRST_EXT_MSB;
  /* Advance header ptr to end of ftmh load balancing extension, if exists */
  if (packet_parse_info->lb_ext_en)
  {
    packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_FTMH_LB_EXT_SIZE_BYTE;
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_LB_KEY_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->lb_key = fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_LB_KEY_NOF_BITS;
  }
  /* DSP ext*/
  fld_val = 0;
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
     unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_EXIST_MSB,
    ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_EXIST_NOF_BITS,
    &dsp_ext_exist
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (dsp_ext_exist)
  {
    packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_FTMH_DEST_EXT_SIZE_BYTE;
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->dsp_ext = fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_DSP_NOF_BITS;
 }
    /* add stacking headers */
  if (packet_parse_info->stacking_enabled)
  {

    packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_FTMH_STACKING_SIZE_BYTE;
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_STACKING_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->stacking_ext = fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_STACKING_NOF_BITS;

  }

  /* add tdm headers */
  if (packet_parse_info->tdm_enabled)
  {
    packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_FTMH_TDM_EXT_SIZE_BYTE;
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_LEN_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->tdm_pkt_size=fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_LEN_NOF_BITS;

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_TYPE_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->tdm_pkt_size=fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_TYPE_NOF_BITS;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_RESEVED_NOF_BITS;

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_DST_FAP_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->tdm_dest_fap_id =fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_DST_FAP_NOF_BITS;

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ext_msb,
            ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_OTM_PORT_NOF_BITS,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    packet_info->tdm_otm_port =fld_val;
    ext_msb += ARAD_PP_TRAP_MGMT_FTMH_EXT_TDM_OTM_PORT_NOF_BITS;

  }


  /* pph type */
  fld_val = 0;
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_FTMH_EXT_MSB,
    ARAD_PP_TRAP_MGMT_FTMH_EXT_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  pph_type = fld_val;
  SOC_SAND_ERR_IF_ABOVE_MAX(pph_type, 3, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 10, exit);

  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "trap_packet_parse_test", 0)){
	  if(dest_system_port == packet_info->otm_port){
		  pph_type = 1; 
	  }
  }


  switch (pph_type) {
  case 0: /* NO PPH*/
        goto exit; 
  case 2: /*PH OAM-TS only*/
        header_ptr = packet_info->ntwrk_header_ptr;
        res = arad_pp_trap_packet_parse_oam_ts(unit,&hdr_buff[header_ptr],packet_parse_info, packet_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
         goto exit; 
  case 3: /*PPH Base + OAM-TS*/
      header_ptr = packet_info->ntwrk_header_ptr;
      res = arad_pp_trap_packet_parse_oam_ts(unit,&hdr_buff[header_ptr],packet_parse_info, packet_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_PPH_OAM_EXT_SIZE_BYTE;
      break;
  default:
      break;

  }
 
  /* Both ptr's now points to the PPH */
  header_ptr = packet_info->ntwrk_header_ptr;


  /*
   * PPH
   */
  /* is trapped packet */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_PPH_FWD_MSB,
          ARAD_PP_TRAP_MGMT_PPH_FWD_NOF_BITS,
          &fld_val
       );
  packet_info->pph_fwd_code = fld_val;
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);
  if (packet_info->frwrd_type != ARAD_PP_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP)
  {
    packet_info->is_trapped = (uint8)(fld_val == ARAD_PP_TRAP_MGMT_IS_TRAP_VAL);
    if(packet_info->is_trapped)
    {
      packet_info->frwrd_type = ARAD_PP_TRAP_MGMT_PKT_FRWRD_TYPE_TRAP;
    }
  }

  /* Determine whether the the packet was routed*/
  is_routed = ((fld_val == ARAD_PP_TRAP_MGMT_IPV4_UC_ROUTE_VAL) ||
			   (fld_val == ARAD_PP_TRAP_MGMT_IPV4_MC_ROUTE_VAL));

  /* vsi/VRF  */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_PPH_VSI_MSB,
          ARAD_PP_TRAP_MGMT_PPH_VSI_NOF_BITS,
          &fld_val
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (is_routed) {
    packet_info->vrf = fld_val;
  }
  else {
    packet_info->vrf = ARAD_PP_VRF_INVALID;
    packet_info->rif = fld_val;
  }

  /* inLIF/RIF  */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
          unit,
          &hdr_buff[header_ptr],
          ARAD_PP_TRAP_MGMT_PPH_AC_MSB,
          ARAD_PP_TRAP_MGMT_PPH_AC_NOF_BITS,
          &fld_val
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  packet_info->pph_inlif_inrif = fld_val;
  if (is_routed) {
      packet_info->in_ac = ARAD_PP_AC_ID_INVALID;
      packet_info->rif = fld_val;
  }
  else {
      packet_info->in_ac = fld_val;
  }

  /* snoop extension */
  /* Only two msb bits of snoop code exists here */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_PPH_SC_MSB,
    ARAD_PP_TRAP_MGMT_PPH_SC_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

  packet_info->snoop_cmnd = fld_val << 2;

  /* fwd_hdr_offset */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_PPH_FWD_HDR_OFFSET_MSB,
    ARAD_PP_TRAP_MGMT_PPH_FWD_HDR_OFFSET_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

  packet_info->pph_fwd_hdr_offset = fld_val;

  /* unknown addr */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_PPH_UNKNOWN_ADDR_MSB,
    ARAD_PP_TRAP_MGMT_PPH_UNKNOWN_ADDR_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

  packet_info->pph_unknown_addr = fld_val;

  /* Learn extension */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_MSB,
    ARAD_PP_TRAP_MGMT_PPH_LEARN_EXT_PRSNT_NOF_BITS,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  ac_present = fld_val;

  /* EEI extension */
  res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
    unit,
    &hdr_buff[header_ptr],
    ARAD_PP_TRAP_MGMT_PPH_EEI_EXT_PRSNT_MSB,
    ARAD_PP_TRAP_MGMT_PPH_EEI_EXT_PRSNT_NOF_BITS,
    &eei_ext
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

   res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
       unit,
		   &hdr_buff[header_ptr],
		   ARAD_PP_TRAP_MGMT_PPH_APP_EXT_PRSNT_MSB,
		   ARAD_PP_TRAP_MGMT_PPH_APP_EXT_PRSNT_NOF_BITS,
		   &app_ext
		   );
   SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
 

   if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "trap_packet_parse_test", 0)){
	   if (packet_info->otm_port  == dest_system_port)
	   {
		   app_ext= 1;
	   }
   }

  /* Advance header ptr to end of pph */
  packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_PPH_SIZE_BYTE;
  header_ptr = packet_info->ntwrk_header_ptr;
  /*
  if (pph_type == 3) {
      res = arad_pp_trap_packet_parse_oam_ts(&hdr_buff[header_ptr], packet_parse_info, packet_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  */
    /* trap info */
  if (packet_info->is_trapped && app_ext)
  {
    /* CPU trap code qualifier */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            &hdr_buff[header_ptr],
            ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_QUAL_MSB,
            ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_QUAL_NOF_BITS,
            &fld_val
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    packet_info->cpu_trap_qualifier = fld_val;

    /* CPU trap code */
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      &hdr_buff[header_ptr],
      ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_MSB,
      ARAD_PP_TRAP_MGMT_PPH_TRAP_CODE_NOF_BITS,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, fld_val, &trap_code);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    packet_info->cpu_trap_code = trap_code;
  }


  /* go over app */
  switch(app_ext) {
  case 1:
      packet_info->ntwrk_header_ptr += 3;
      break;
  case 2:
      packet_info->ntwrk_header_ptr += 5;
      break;
  default:
      break;
  }
  
  /* Advance header ptr to end of pph eei extension, if exists */
  if (eei_ext)
  {
    packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_PPH_EEI_EXT_SIZE_BYTE;
  }
  
  if(ac_present) {
      packet_info->ntwrk_header_ptr += ARAD_PP_TRAP_MGMT_PPH_LEARN_EXT_SIZE_BYTE;
  }

  
  /* Add the user header size */
  packet_info->user_header_ptr = packet_info->ntwrk_header_ptr;
  if (packet_parse_info->user_header_size) {
      packet_info->ntwrk_header_ptr += packet_parse_info->user_header_size;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_packet_parse_unsafe()", buff_len, 0);
}

/*********************************************************************
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_frwrd_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fwd_dest_buff,
    fwd_asd_buff;
  ARAD_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;
  soc_reg_above_64_val_t data;
  soc_reg_above_64_val_t fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_mgmt_trap_code_to_internal(
          unit,
          trap_code_ndx,
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          &profile_info->dest_info.frwrd_dest,
          &fwd_dest_buff,
          &fwd_asd_buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  fwd_act_profile_tbl_data.fwd_act_destination_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_traffic_class_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_fwd_offset_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_POLICER) ? 1 : 0);
  /* new for arad */
  fwd_act_profile_tbl_data.fwd_act_eei_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST_EEI) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_out_lif_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST_OUTLIF) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_forwarding_code_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_TYPE) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_da_type_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DA_TYPE) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_meter_pointer_a_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_METER_0) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_meter_pointer_b_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_METER_1) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_counter_pointer_a_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_0) ? 1 : 0);
  fwd_act_profile_tbl_data.fwd_act_counter_pointer_b_overwrite =
    ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_1) ? 1 : 0);

  if(profile_info->bitmap_mask & (ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_0|ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_1))
  {
    fwd_act_profile_tbl_data.fwd_act_counter_pointer_update = 1;
  }
  else
  {
    fwd_act_profile_tbl_data.fwd_act_counter_pointer_update = 0;
  }
  if(profile_info->bitmap_mask & (ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_METER_0|ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_METER_1))
  {
    fwd_act_profile_tbl_data.fwd_act_meter_pointer_update = 1;
  }
  else
  {
    fwd_act_profile_tbl_data.fwd_act_meter_pointer_update = 0;
  }

  fwd_act_profile_tbl_data.fwd_act_trap =
    SOC_SAND_BOOL2NUM(profile_info->processing_info.is_trap);
  fwd_act_profile_tbl_data.fwd_act_bypass_filtering =
    SOC_SAND_BOOL2NUM(profile_info->processing_info.is_control);
  fwd_act_profile_tbl_data.fwd_act_destination =
    fwd_dest_buff;
  fwd_act_profile_tbl_data.fwd_act_destination_add_vsi =
    SOC_SAND_BOOL2NUM(profile_info->dest_info.add_vsi);
  fwd_act_profile_tbl_data.fwd_act_destination_add_vsi_shift =
    profile_info->dest_info.vsi_shift;
  fwd_act_profile_tbl_data.fwd_act_traffic_class =
    profile_info->cos_info.tc;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence =
    profile_info->cos_info.dp;
  fwd_act_profile_tbl_data.fwd_act_meter_pointer =
    profile_info->meter_info.meter_id;
  fwd_act_profile_tbl_data.fwd_act_counter_pointer =
    profile_info->count_info.counter_id;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command =
    profile_info->meter_info.meter_command;
  fwd_act_profile_tbl_data.fwd_act_fwd_offset_index =
    profile_info->processing_info.frwrd_offset_index;
  fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer =
    profile_info->policing_info.ethernet_police_id;
  fwd_act_profile_tbl_data.fwd_act_learn_disable =
    SOC_SAND_BOOL2NUM_INVERSE(profile_info->processing_info.enable_learning);
  /* arad new */
  fwd_act_profile_tbl_data.fwd_act_eei = fwd_asd_buff;
  fwd_act_profile_tbl_data.fwd_act_out_lif = fwd_asd_buff;
  /* da-type and frwrd-tyep same encoding in hw/sw*/
  fwd_act_profile_tbl_data.fwd_act_da_type = profile_info->processing_info.da_type;
  fwd_act_profile_tbl_data.fwd_act_forwarding_code = profile_info->processing_info.frwrd_type;
  if(profile_info->processing_info.frwrd_offset_bytes_fix >= 0)
  {
    fwd_act_profile_tbl_data.fwd_act_fwd_offset_fix = profile_info->processing_info.frwrd_offset_bytes_fix;
  }
  else
  {
    fwd_act_profile_tbl_data.fwd_act_fwd_offset_fix = ((~(uint32)(-profile_info->processing_info.frwrd_offset_bytes_fix))&0x3f)+1;
  }
  
  res = arad_pp_ihb_fwd_act_profile_tbl_set_unsafe(
          unit,
          trap_code_internal,
          &fwd_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (strength_fld_fwd.reg != INVALIDr)
  {
    SOC_REG_ABOVE_64_CLEAR(fld_val);

    fld_val[0] = profile_info->strength;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, soc_reg_above_64_get(unit,strength_fld_fwd.reg,REG_PORT_ANY, arad_pp_trap_mgmt_trap_internal_to_reg_index(trap_code_internal), data));
    soc_reg_above_64_field_set(unit, strength_fld_fwd.reg, data,strength_fld_fwd.field,fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, soc_reg_above_64_set(unit,strength_fld_fwd.reg,REG_PORT_ANY, arad_pp_trap_mgmt_trap_internal_to_reg_index(trap_code_internal), data));
  }
  else{
      /* maybe they are in trap mgmt */
      res = arad_pp_flp_trap_config_update(
              unit,
              trap_code_internal,
              profile_info->strength,
              -1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_frwrd_profile_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_trap_frwrd_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  res = ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_verify(unit, profile_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_frwrd_profile_info_set_verify()", trap_code_ndx, 0);
}

uint32
  arad_pp_trap_frwrd_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_frwrd_profile_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_frwrd_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;
  soc_reg_above_64_val_t data;
  soc_reg_above_64_val_t fld_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(profile_info);

  res = arad_pp_trap_mgmt_trap_code_to_internal(
    unit,
    trap_code_ndx,
    &trap_code_internal,
    &strength_fld_fwd,
    &strength_fld_snp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_fwd_act_profile_tbl_get_unsafe(
    unit,
    trap_code_internal,
    &fwd_act_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (strength_fld_fwd.reg != INVALIDr)
  {
    SOC_REG_ABOVE_64_CLEAR(fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg_above_64_get(unit,strength_fld_fwd.reg,REG_PORT_ANY, arad_pp_trap_mgmt_trap_internal_to_reg_index(trap_code_internal), data));
    soc_reg_above_64_field_get(unit, strength_fld_fwd.reg, data,strength_fld_fwd.field,fld_val);
    profile_info->strength = fld_val[0];
  }
  else{
      /* maybe they are in trap mgmt */
      res = arad_pp_flp_trap_config_get(
              unit,
              trap_code_internal,
              &profile_info->strength,
              0 /* null for ignore */
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }

  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          fwd_act_profile_tbl_data.fwd_act_destination,
          fwd_act_profile_tbl_data.fwd_act_eei,
          ARAD_PP_FWD_DECISION_PARSE_DEST|ARAD_PP_FWD_DECISION_PARSE_EEI|ARAD_PP_FWD_DECISION_PARSE_OULTIF,
          &profile_info->dest_info.frwrd_dest
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  profile_info->bitmap_mask = 0;

  profile_info->processing_info.is_trap =
    SOC_SAND_NUM2BOOL(fwd_act_profile_tbl_data.fwd_act_trap);
  profile_info->processing_info.is_control =
    SOC_SAND_NUM2BOOL(fwd_act_profile_tbl_data.fwd_act_bypass_filtering);
  profile_info->dest_info.add_vsi =
    SOC_SAND_NUM2BOOL(fwd_act_profile_tbl_data.fwd_act_destination_add_vsi);
  profile_info->dest_info.vsi_shift =
    fwd_act_profile_tbl_data.fwd_act_destination_add_vsi_shift;
  profile_info->cos_info.tc =
    (SOC_SAND_PP_TC)fwd_act_profile_tbl_data.fwd_act_traffic_class;
  profile_info->cos_info.dp =
    (SOC_SAND_PP_DP)fwd_act_profile_tbl_data.fwd_act_drop_precedence;
  profile_info->meter_info.meter_id =
    fwd_act_profile_tbl_data.fwd_act_meter_pointer;
  profile_info->count_info.counter_id =
    fwd_act_profile_tbl_data.fwd_act_counter_pointer;
  profile_info->meter_info.meter_command =
    fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command;
  profile_info->processing_info.frwrd_offset_index =
    fwd_act_profile_tbl_data.fwd_act_fwd_offset_index;
  profile_info->policing_info.ethernet_police_id =
    fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer;
  profile_info->processing_info.enable_learning =
    SOC_SAND_NUM2BOOL_INVERSE(fwd_act_profile_tbl_data.fwd_act_learn_disable);


  profile_info->bitmap_mask = 0;
  if (fwd_act_profile_tbl_data.fwd_act_destination_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  }
  if (fwd_act_profile_tbl_data.fwd_act_traffic_class_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC;
  }
  if (fwd_act_profile_tbl_data.fwd_act_drop_precedence_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP;
  }
  if (fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD;
  }
  if (fwd_act_profile_tbl_data.fwd_act_fwd_offset_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;
  }
  if (fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_POLICER;
  }
/* new for arad */
    if (fwd_act_profile_tbl_data.fwd_act_eei_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST_EEI;
  }
  if (fwd_act_profile_tbl_data.fwd_act_out_lif_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST_OUTLIF;
  }
  if (fwd_act_profile_tbl_data.fwd_act_forwarding_code_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_TYPE;
  }
  if (fwd_act_profile_tbl_data.fwd_act_da_type_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DA_TYPE;
  }
  if (fwd_act_profile_tbl_data.fwd_act_meter_pointer_a_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_METER_0;
  }
  if (fwd_act_profile_tbl_data.fwd_act_meter_pointer_b_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_METER_1;
  }
  if (fwd_act_profile_tbl_data.fwd_act_counter_pointer_a_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_0;
  }
  if (fwd_act_profile_tbl_data.fwd_act_counter_pointer_b_overwrite)
  {
    profile_info->bitmap_mask |=ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_1;
  }

  /* da-type and frwrd-tyep same encoding in hw/sw*/
  profile_info->processing_info.da_type = fwd_act_profile_tbl_data.fwd_act_da_type;
  profile_info->processing_info.frwrd_type = fwd_act_profile_tbl_data.fwd_act_forwarding_code;

  if(fwd_act_profile_tbl_data.fwd_act_fwd_offset_fix & SOC_SAND_BIT(5))/* negative take first 5 bits */
  {
    profile_info->processing_info.frwrd_offset_bytes_fix = -(((~fwd_act_profile_tbl_data.fwd_act_fwd_offset_fix)&0x3f)+1);
  }
  else/* positive */
  {
    profile_info->processing_info.frwrd_offset_bytes_fix = fwd_act_profile_tbl_data.fwd_act_fwd_offset_fix;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_frwrd_profile_info_get_unsafe()", trap_code_ndx, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_snoop_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA
    tbl_data;
  ARAD_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;
  soc_reg_above_64_val_t data;
  soc_reg_above_64_val_t fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_mgmt_trap_code_to_internal(
    unit,
    trap_code_ndx,
    &trap_code_internal,
    &strength_fld_fwd,
    &strength_fld_snp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.snoop_action = profile_info->snoop_cmnd;

  res = arad_pp_ihb_snoop_action_tbl_set_unsafe(
    unit,
    trap_code_internal,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (strength_fld_snp.reg != INVALIDr)
  {
    SOC_REG_ABOVE_64_CLEAR(fld_val);

    fld_val[0] = profile_info->strength;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit,soc_reg_above_64_get(unit,strength_fld_snp.reg,REG_PORT_ANY, arad_pp_trap_mgmt_trap_internal_to_reg_index(trap_code_internal), data));
    soc_reg_above_64_field_set(unit, strength_fld_snp.reg, data,strength_fld_snp.field,fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_set(unit,strength_fld_snp.reg,REG_PORT_ANY, arad_pp_trap_mgmt_trap_internal_to_reg_index(trap_code_internal), data));
  }
  else{
      /* maybe they are in trap mgmt */
      res = arad_pp_flp_trap_config_update(
              unit,
              trap_code_internal,
              -1,
              profile_info->strength
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_snoop_profile_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_trap_snoop_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_snoop_profile_info_set_verify()", 0, 0);
}

uint32
  arad_pp_trap_snoop_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code_ndx, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_MAX, ARAD_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_snoop_profile_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_snoop_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA
    tbl_data;
  ARAD_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;
  soc_reg_above_64_val_t data;
  soc_reg_above_64_val_t fld_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(profile_info);

  res = arad_pp_trap_mgmt_trap_code_to_internal(
    unit,
    trap_code_ndx,
    &trap_code_internal,
    &strength_fld_fwd,
    &strength_fld_snp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihb_snoop_action_tbl_get_unsafe(
    unit,
    trap_code_internal,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  profile_info->snoop_cmnd = tbl_data.snoop_action;

  if (strength_fld_snp.reg != INVALIDr)
  {
    SOC_REG_ABOVE_64_CLEAR(fld_val);
    res = soc_reg_above_64_get(unit,strength_fld_snp.reg,REG_PORT_ANY, arad_pp_trap_mgmt_trap_internal_to_reg_index(trap_code_internal), data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    soc_reg_above_64_field_get(unit, strength_fld_snp.reg, data,strength_fld_snp.field,fld_val);
    profile_info->strength = fld_val[0];
  }
  else{
      /* maybe they are in trap mgmt */
      res = arad_pp_flp_trap_config_get(
              unit,
              trap_code_internal,
              0,/* null for ignore */
              &profile_info->strength
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_snoop_profile_info_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_to_eg_action_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                  eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    valid,
    fld_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET_UNSAFE);

  valid = (eg_action_profile == ARAD_PP_TRAP_EG_NO_ACTION) ? 0 : 1;
  fld_val = ARAD_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_TO_FLD(valid, eg_action_profile);

  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_VSI_MEMBERSHIP)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_VLAN_MEMBERSHIPf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_ACC_FRM)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_UNACCEPTABLE_FRAME_TYPEf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_HAIR_PIN)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_SRC_EQUAL_DESTf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_UNKNOWN_DA)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_UNKNOWN_DAf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_SPLIT_HORIZON)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_SPLIT_HORIZONf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_PRIVATE_VLAN)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_PRIVATE_VLANf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TTL_SCOPE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_TTL_SCOPEf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_MTU_VIOLATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_MTU_VIOLATIONf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TRILL_TTL_0)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_TRILL_TTL_ZEROf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TRILL_SAME_INTERFACE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_TRILL_SAME_INTERFACEf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TRILL_BOUNCE_BACK)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_BOUNCE_BACKf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_DSS_STACKING)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_DSS_STACKINGf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_LAG_MULTICAST)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_LAG_MULTICASTf,  fld_val));
  }
  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_EXCLUDE_SRC)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_EXCLUDE_SRCf,  fld_val));
  }
  
  if(trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_ALL)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_INVALID_OTMf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  161,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_CNM_INTERCEPTf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  162,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_VERSION_ERRORf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  163,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_CHECKSUM_ERRORf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  164,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_HEADER_LENGTH_ERRORf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  165,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_TOTAL_LENGTH_ERRORf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  166,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_TTL_EQUALS_ZEROf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  167,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_OPTIONSf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  168,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_TTL_EQUALS_ONEf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  169,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_SIP_EQUALS_DIPf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_DIP_EQUALS_ZEROf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  171,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_SIP_IS_MCf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  172,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_UNSPECIFIED_DSTf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  173,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LOOPBACKf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  174,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SIP_IS_MCf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  175,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_UNSPECIFIED_SRCf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  176,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LINK_LOCAL_DSTf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  177,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SITE_LOCAL_DSTf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  178,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LINK_LOCAL_SRCf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  179,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SITE_LOCAL_SRCf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_IPV4_COMPATIBLE_DSTf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  181,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_IPV4_MAPPED_DSTf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  182,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_DIP_IS_MCf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  183,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_HOP_BY_HOPf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_INVALID_OUT_PORT)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_INVALID_OTMf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_CNM_PACKET)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  161,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_CNM_INTERCEPTf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_VERSION_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  162,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_VERSION_ERRORf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_CHECKSUM_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  163,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_CHECKSUM_ERRORf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_HEADER_LENGTH_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  164,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_HEADER_LENGTH_ERRORf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_TOTAL_LENGTH_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  165,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_TOTAL_LENGTH_ERRORf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IP_TTL0)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  166,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_TTL_EQUALS_ZEROf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IP_HAS_OPTIONS)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  167,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_OPTIONSf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IP_TTL1)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  168,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_TTL_EQUALS_ONEf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_SIP_EQUAL_DIP)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  169,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_SIP_EQUALS_DIPf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_DIP_ZERO)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_DIP_EQUALS_ZEROf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_SIP_IS_MC)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  171,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_SIP_IS_MCf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_UNSPECIFIED_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  172,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_UNSPECIFIED_DSTf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOOPBACK_ADDRESS)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  173,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LOOPBACKf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_MULTICAST_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  174,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SIP_IS_MCf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_UNSPECIFIED_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  175,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_UNSPECIFIED_SRCf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_LINK_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  176,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LINK_LOCAL_DSTf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_SITE_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  177,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SITE_LOCAL_DSTf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_LINK_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  178,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LINK_LOCAL_SRCf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_SITE_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  179,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SITE_LOCAL_SRCf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_IPV4_COMPATIBLE_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_IPV4_COMPATIBLE_DSTf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_IPV4_MAPPED_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  181,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_IPV4_MAPPED_DSTf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_MULTICAST_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  182,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_DIP_IS_MCf,  fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_HOP_BY_HOP)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  183,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_HOP_BY_HOPf,  fld_val));
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_to_eg_action_map_set_unsafe()", trap_type_bitmap_ndx, eg_action_profile);
}

uint32
  arad_pp_trap_to_eg_action_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                  eg_action_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET_VERIFY);

  if ((trap_type_bitmap_ndx != ARAD_PP_TRAP_EG_TYPE_ALL) && trap_type_bitmap_ndx & (ARAD_PP_TRAP_EG_TYPE_CFM_TRAP|ARAD_PP_TRAP_EG_TYPE_NO_VSI_TRANSLATION)) 
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

  if((trap_type_bitmap_ndx != ARAD_PP_TRAP_EG_TYPE_ALL) && trap_type_bitmap_ndx > ARAD_PP_TRAP_EG_TYPE_IPV6_HOP_BY_HOP)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 11, exit);
  }

  if (eg_action_profile != ARAD_PP_TRAP_EG_NO_ACTION)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(eg_action_profile, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_to_eg_action_map_set_verify()", 0, 0);
}

uint32
  arad_pp_trap_to_eg_action_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx
  )
{
  uint32
    tmp;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET_VERIFY);

  if(trap_type_bitmap_ndx < SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT && (trap_type_bitmap_ndx != 0)) {
      tmp = trap_type_bitmap_ndx & 0xffff;
      if (soc_sand_bitstream_get_nof_on_bits(&tmp, 1) != 1)
      {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR, 10, exit);
      }
  }
  if(trap_type_bitmap_ndx > SOC_PPC_TRAP_EG_TYPE_HIGHEST_ARAD)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 11, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_to_eg_action_map_get_verify()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_to_eg_action_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                  *eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    valid,
    fld_val=0,
    eg_action_profile_from_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET_UNSAFE);

  valid = 0;
  eg_action_profile_from_fld = 0;

  if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_VSI_MEMBERSHIP)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_VLAN_MEMBERSHIPf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_ACC_FRM)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_UNACCEPTABLE_FRAME_TYPEf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_HAIR_PIN)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_SRC_EQUAL_DESTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_UNKNOWN_DA)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_UNKNOWN_DAf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_SPLIT_HORIZON)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_SPLIT_HORIZONf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_PRIVATE_VLAN)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_PRIVATE_VLANf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TTL_SCOPE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_TTL_SCOPEf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_MTU_VIOLATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_MTU_VIOLATIONf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TRILL_TTL_0)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_TRILL_TTL_ZEROf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TRILL_SAME_INTERFACE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_2r, SOC_CORE_ALL, 0, ACTION_PROFILE_TRILL_SAME_INTERFACEf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_TRILL_BOUNCE_BACK)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_BOUNCE_BACKf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_DSS_STACKING)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_DSS_STACKINGf, &fld_val));
  }
  else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_LAG_MULTICAST)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_LAG_MULTICASTf, &fld_val));
  }
 else if (trap_type_bitmap_ndx & ARAD_PP_TRAP_EG_TYPE_EXCLUDE_SRC)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_EXCLUDE_SRCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_INVALID_OUT_PORT)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_1r, SOC_CORE_ALL, 0, ACTION_PROFILE_INVALID_OTMf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_CNM_PACKET)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  161,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_CNM_INTERCEPTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_VERSION_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  162,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_VERSION_ERRORf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_CHECKSUM_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  163,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_CHECKSUM_ERRORf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_HEADER_LENGTH_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  164,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_HEADER_LENGTH_ERRORf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_TOTAL_LENGTH_ERROR)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  165,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_TOTAL_LENGTH_ERRORf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IP_TTL0)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  166,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_TTL_EQUALS_ZEROf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IP_HAS_OPTIONS)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  167,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_OPTIONSf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IP_TTL1)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  168,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_IP_TTL_EQUALS_ONEf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_SIP_EQUAL_DIP)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  169,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_SIP_EQUALS_DIPf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_DIP_ZERO)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_DIP_EQUALS_ZEROf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV4_SIP_IS_MC)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  171,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV4_SIP_IS_MCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_UNSPECIFIED_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  172,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_UNSPECIFIED_DSTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOOPBACK_ADDRESS)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  173,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LOOPBACKf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_MULTICAST_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  174,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SIP_IS_MCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_UNSPECIFIED_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  175,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_4r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_UNSPECIFIED_SRCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_LINK_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  176,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LINK_LOCAL_DSTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_SITE_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  177,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SITE_LOCAL_DSTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_LINK_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  178,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_LINK_LOCAL_SRCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_LOCAL_SITE_SOURCE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  179,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_SITE_LOCAL_SRCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_IPV4_COMPATIBLE_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_IPV4_COMPATIBLE_DSTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_IPV4_MAPPED_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  181,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_IPV4_MAPPED_DSTf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_MULTICAST_DESTINATION)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  182,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_DIP_IS_MCf, &fld_val));
  }
  else if (trap_type_bitmap_ndx == ARAD_PP_TRAP_EG_TYPE_IPV6_HOP_BY_HOP)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  183,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_ACTION_PROFILE_5r, SOC_CORE_ALL, 0, ACTION_PROFILE_IPV6_HOP_BY_HOPf, &fld_val));
  }

  
  ARAD_PP_TRAP_MGMT_EG_ACTION_MAP_VALS_FROM_FLD(fld_val, valid, eg_action_profile_from_fld);
  *eg_action_profile = (valid ? eg_action_profile_from_fld : ARAD_PP_TRAP_EG_NO_ACTION);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_to_eg_action_map_get_unsafe()", trap_type_bitmap_ndx, 0);
}

void
  arad_pp_trap_eg_profile_info_hdr_data_to_cud(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_PP_TRAP_EG_ACTION_HDR_DATA    profile_hdr_data,
    SOC_SAND_OUT uint32                             *cud
  )
{
  *cud = profile_hdr_data.cud;
  *cud |= (profile_hdr_data.dsp_ptr << 16);
  *cud |= (profile_hdr_data.cos_profile << 24);
  *cud |= (profile_hdr_data.mirror_profile << 28);
}

void
  arad_pp_trap_eg_profile_info_cud_to_hdr_data(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             cud,
    SOC_SAND_OUT ARAD_PP_TRAP_EG_ACTION_HDR_DATA    *profile_hdr_data
  )
{

    profile_hdr_data->cud = (cud & 0xffff);
    profile_hdr_data->dsp_ptr = (uint8)((cud >> 16) & 0xff);
    profile_hdr_data->cos_profile = (uint8)((cud >> 24) & 0xff);
    profile_hdr_data->mirror_profile = (uint8)((cud >> 28) & 0xff);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_eg_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  tbl_data.out_tm_port_valid =
   ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST) ? 0x1 : 0x0);
  tbl_data.out_tm_port = (profile_info->out_tm_port == ARAD_PP_TRAP_ACTION_PKT_DISCARD_ID ? 0x0 : profile_info->out_tm_port);
  tbl_data.discard = (profile_info->out_tm_port == ARAD_PP_TRAP_ACTION_PKT_DISCARD_ID ? 0x1 : 0x0);
  tbl_data.tc = profile_info->cos_info.tc;
  tbl_data.tc_valid = ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC) ? 0x1 : 0x0);
  tbl_data.dp = profile_info->cos_info.dp;
  tbl_data.dp_valid = ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP) ? 0x1 : 0x0);

  arad_pp_trap_eg_profile_info_hdr_data_to_cud(unit, profile_info->header_data, &tbl_data.cud);
  tbl_data.cud_valid = ((profile_info->bitmap_mask & ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_CUD) ? 0x1 : 0x0);

  res = arad_pp_egq_action_profile_table_tbl_set_unsafe(
          unit,
          profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_eg_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_trap_eg_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(profile_ndx, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX, ARAD_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_eg_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_trap_eg_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(profile_ndx, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MIN, ARAD_PP_TRAP_MGMT_EG_ACTION_PROFILE_MAX, ARAD_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_eg_profile_info_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_eg_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO_clear(profile_info);

  res = arad_pp_egq_action_profile_table_tbl_get_unsafe(
    unit,
    profile_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  profile_info->bitmap_mask = 0;
  if (tbl_data.out_tm_port_valid)
  {
    profile_info->bitmap_mask |= ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  }
  if (tbl_data.discard)
  {
    profile_info->out_tm_port = ARAD_PP_TRAP_ACTION_PKT_DISCARD_ID;
  }
  else
  {
    profile_info->out_tm_port = tbl_data.out_tm_port;
  }
  profile_info->cos_info.tc = (SOC_SAND_PP_TC)tbl_data.tc;
  if (tbl_data.tc_valid)
  {
    profile_info->bitmap_mask |= ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC;
  }
  profile_info->cos_info.dp = (SOC_SAND_PP_DP)tbl_data.dp;
  if (tbl_data.dp_valid)
  {
    profile_info->bitmap_mask |= ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP;
  }
  arad_pp_trap_eg_profile_info_cud_to_hdr_data(unit, tbl_data.cud, &(profile_info->header_data));
  if (tbl_data.cud_valid)
  {
    profile_info->bitmap_mask |= ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_CUD;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_eg_profile_info_get_unsafe()", profile_ndx, 0);
}

/*********************************************************************
*     Read MACT event from the events FIFO into buffer.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_mact_event_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                  buff[ARAD_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                  *buff_len
  )
{
  uint32
    res = SOC_SAND_OK,
      nof_event_fifos,
      event_fifo_ndx,
    fld_val;
  soc_reg_above_64_val_t reg_above_64;
  soc_reg_t
      event_ready_r,
      mact_event_r;
  soc_field_t
      event_ready_f;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(buff_len);

  fld_val = 0;

  /* 2 Event FIFOs in Jericho */
  nof_event_fifos = SOC_IS_JERICHO(unit)? 2: 1;

  for (event_fifo_ndx = 0; event_fifo_ndx < nof_event_fifos; event_fifo_ndx++) {
      event_ready_r = SOC_IS_JERICHO(unit)? PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr: IHP_MACT_INTERRUPT_REGISTER_TWOr;
      event_ready_f = SOC_IS_JERICHO(unit)? ((event_fifo_ndx == 0)? LARGE_EM_MASTER_EVENT_READYf: LARGE_EM_SLAVE_EVENT_READYf): MACT_EVENT_READYf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, event_ready_r, REG_PORT_ANY, 0, event_ready_f, &fld_val));
      if (fld_val)
      {
        /* Event is ready in FIFO */
        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        mact_event_r = SOC_IS_JERICHO(unit)? ((event_fifo_ndx == 0)? PPDB_B_LARGE_EM_MASTER_EVENTr: PPDB_B_LARGE_EM_SLAVE_EVENTr): IHP_MACT_EVENTr;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_get(unit, mact_event_r, REG_PORT_ANY, 0, reg_above_64));
        
        /* Read large_em_event_reg.reg_0 last, since lsb read advances the FIFO */
        buff[4] = reg_above_64[4];
        buff[3] = reg_above_64[3];
        buff[2] = reg_above_64[2];
        buff[1] = reg_above_64[1];
        buff[0] = reg_above_64[0];
        *buff_len = 5;
        break; /* event found */
      }
      else
      {
        /* Event FIFO is empty */
        *buff_len = 0;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mact_event_get_unsafe()", 0, 0);
}

uint32
  arad_pp_trap_mact_event_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_GET_VERIFY);

  /* Nothing to verify */
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mact_event_get_verify()", 0, 0);
}

/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  buff[ARAD_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                  buff_len,
    SOC_SAND_OUT ARAD_PP_TRAP_MACT_EVENT_INFO                *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_PARSE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  ARAD_PP_TRAP_MACT_EVENT_INFO_clear(mact_event);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
  SOC_SAND_TODO_IMPLEMENT_WARNING;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mact_event_parse_unsafe()", 0, 0);
}

uint32
  arad_pp_trap_mact_event_parse_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  buff[ARAD_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                  buff_len
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_PARSE_VERIFY);

  if (buff_len != ARAD_PP_TRAP_MGMT_BUFF_LEN_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mact_event_parse_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_trap_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_trap_mgmt_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_trap_mgmt;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_trap_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_trap_mgmt_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_trap_mgmt;
}
uint32
  ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_DECISION_INFO, &(info->frwrd_dest), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_shift, ARAD_PP_TRAP_MGMT_VSI_SHIFT_MAX, ARAD_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->counter_select, ARAD_PP_TRAP_MGMT_COUNTER_SELECT_MAX, ARAD_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->counter_id, ARAD_PP_TRAP_MGMT_COUNTER_ID_MAX, ARAD_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter_select, ARAD_PP_TRAP_MGMT_METER_SELECT_MAX, ARAD_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->meter_id, 1 << SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), ARAD_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->meter_command, ARAD_PP_TRAP_MGMT_METER_COMMAND_MAX, ARAD_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ethernet_police_id, ARAD_PP_TRAP_MGMT_ETHERNET_POLICE_ID_MAX, ARAD_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_offset_index, 
                        ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_MAX, 
                        ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR, 13, exit);

  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_type, 
                           ARAD_PP_TRAP_MGM_DA_TYPE_MAX,
                           ARAD_PP_TRAP_MGMT_DA_TYPE_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->frwrd_offset_bytes_fix, 
                           ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_MIN, ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_MAX, 
                           ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_type, 
                           ARAD_PP_PKT_FRWRD_TYPE_TM, 
                           ARAD_PP_TRAP_MGMT_FRWRD_OFFSET_BYTES_FIX_OUT_OF_RANGE_ERR, 15, exit);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, ARAD_PP_TRAP_MGMT_STRENGTH_MAX, ARAD_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO, &(info->dest_info), 12, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO, &(info->cos_info), 13, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO, &(info->count_info), 14, exit);
  res = ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO_verify(unit, &(info->meter_info));
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO, &(info->policing_info), 16, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO, &(info->processing_info), 17, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, ARAD_PP_TRAP_MGMT_SNP_STRENGTH_MAX, ARAD_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->snoop_cmnd, ARAD_PP_TRAP_MGMT_SNOOP_CMND_MAX, ARAD_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->bitmap_mask, ARAD_PP_TRAP_MGMT_BITMAP_MASK_MAX, ARAD_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR, 10, exit);
  
  if (info->out_tm_port != ARAD_PP_TRAP_ACTION_PKT_DISCARD_ID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->out_tm_port, ARAD_PP_TM_PORT_MAX, ARAD_PP_TM_PORT_OUT_OF_RANGE_ERR, 11, exit);
  }
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO, &(info->cos_info), 12, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_data.cud, ARAD_PP_TRAP_MGMT_CUD_MAX, ARAD_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR, 13, exit);

  /* FIX: add verification that TM-port equals DSP-ptr in CUD? */
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_TRAP_MACT_EVENT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_TRAP_MGMT_TYPE_MAX, ARAD_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  res = ARAD_PP_FRWRD_MACT_ENTRY_KEY_verify(unit, &(info->key));
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_MACT_ENTRY_VALUE, &(info->val), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_MACT_EVENT_INFO_verify()",0,0);
}

uint32
  ARAD_PP_TRAP_PACKET_INFO_verify(
    SOC_SAND_IN  ARAD_PP_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_trap_code, ARAD_PP_TRAP_MGMT_CPU_TRAP_CODE_MAX, ARAD_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_trap_qualifier, ARAD_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_MAX, ARAD_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_sys_port, ARAD_PP_TRAP_MGMT_SRC_SYS_PORT_MAX, ARAD_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ntwrk_header_ptr, ARAD_PP_TRAP_MGMT_LL_HEADER_PTR_MAX, ARAD_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_TRAP_PACKET_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

