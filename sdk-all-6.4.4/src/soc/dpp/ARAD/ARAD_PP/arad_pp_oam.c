/* $Id: arad_pp_oam.c,v 1.111 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/swstate/sw_state_access.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>


#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h> 
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_mpls_term.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/hwstate/hw_log.h>

/* programmable editor, for arad+*/
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>

/*Used for diagnostics*/
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_diag.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#ifdef PLISIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#else
  #include <soc/dpp/ARAD/arad_sim_em.h>
#endif
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* ARAD Only */
#define ARAD_PP_OAM_PROFILE_DEFAULT              0
#define ARAD_PP_OAM_PROFILE_PASSIVE              3

/* ARAD Only */
#define _ARAD_PP_OAM_NON_ACC_PROFILES_NUM_ARAD 4




#define _ARAD_PP_OAM_MAX_MD_LEVEL                7

#define _ARAD_PP_OAM_RMEP_EMC_SHIFT_BFD (SOC_IS_JERICHO(unit)?13: 16)
#define _ARAD_PP_OAM_RMEP_EMC_SHIFT_ETH 13

#define _ARAD_PP_OAM_TC_NOF_BITS 3
#define _ARAD_PP_OAM_TC_MAX ((1<<_ARAD_PP_OAM_TC_NOF_BITS)-1)
#define _ARAD_PP_OAM_DP_NOF_BITS 2
#define _ARAD_PP_OAM_DP_MAX ((1<<_ARAD_PP_OAM_DP_NOF_BITS)-1)
#define _ARAD_PP_OAM_TX_RATE_INDEX_MAX 7
#define _ARAD_PP_OAM_REQ_INTERVAL_POINTER_MAX 7
#define _ARAD_PP_OAM_DIP_INDEX_MAX 15

#define _ARAD_PP_OAM_BFD_VERS_MAX 7
#define _ARAD_PP_OAM_BFD_DIAG_MAX 63
#define _ARAD_PP_OAM_BFD_STA_MAX 3
#define _ARAD_PP_OAM_BFD_YOUR_DISC_MAX   SOC_PPD_BFD_DISCRIMINATOR_TO_LIF_START_MASK

#define _ARAD_PP_OAM_EXP_MAX 7
#define _ARAD_PP_OAM_TTL_MAX 255
#define _ARAD_PP_OAM_TOS_MAX 255

#define _ARAD_PP_OAM_PTCH_OPAQUE_VALUE 7

/* Additional error for Jericho: RFC */
#define _ARAD_PP_OAM_NUMBER_OF_OAMP_ERROR_TYPES (15 + SOC_IS_JERICHO(unit))
#define _ARAD_PP_OAM_ITMH_DEST_INFO_WIDTH 20

#define _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO 20

/* Set to be the same size as  the interrupt message register - 80 bytes*/
#define _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA 80

/*Channel 3 is used in complience with the ECI general configuration register.*/
#define ARAD_OAM_DMA_CHANNEL_USED SOC_MEM_FIFO_DMA_CHANNEL_3

#define LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY 0x4000
#define SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY 0x20

#define SOC_DPP_INVALID_GLOBAL_LIF(unit)    (SOC_DPP_DEFS_GET(unit, nof_global_lifs) - 1)

#define ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK ((1 << SOC_DPP_IMP_DEFS_GET(unit, oam_id_nof_bit)) - 1)

typedef enum {
    _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD = 0,
    _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_100,
    _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_10000
} _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS;

typedef enum {
    _ARAD_PP_OAM_ACTION_TRAP_ERROR_LEVEL = 0,
    _ARAD_PP_OAM_ACTION_TRAP_ERROR_PASSIVE,
    _ARAD_PP_OAM_ACTION_RECYCLE,
    _ARAD_PP_OAM_ACTION_TRAP_CPU,
    _ARAD_PP_OAM_ACTION_TRAP_OAMP,
    _ARAD_PP_OAM_ACTION_FRWRD,
    _ARAD_PP_OAM_ACTION_SNOOP
} _ARAD_PP_OAM_ACTION;

typedef enum {
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM = 0, /* BFD */
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_PW_ACH_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_DLM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_ILM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_DM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV4_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV6_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM, /* BFD */
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM, /* BFD */
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_ON_DEMAND_CV_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM, /*G8113*/
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_FAULT_OAM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_NOF
} _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM;

#define _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM 0

#define _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID 127

#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731 0
#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW 1
#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_OAM_UNKNOWN 3

#define _ARAD_PP_OAM_FWD_CODE_MPLS 0x5
#define _ARAD_PP_OAM_FWD_CODE_CPU 0x7
#define _ARAD_PP_OAM_FWD_CODE_ETHERNET 0

#define _ARAD_PP_OAM_FWD_CODE_IPV4_UC 1
#define _ARAD_PP_OAM_IDENTIFICATION_TCAM_MIN_BANKS 0
#define _ARAD_PP_OAM_IDENTIFICATION_PREFIX_SIZE 0

#define _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL 7

/* Maximal LIF.OAM-Trap-Profile value */
#define _BCM_OAM_LIF_PROFILE_MAX 3


#define _ARAD_PP_OAM_FTMH_BASE_SIZE 9 /* 9 bytes */
#define _ARAD_PP_OAM_FTMH_STACKING_EXTENSION_SIZE 2 /* 16 bits */
#define _ARAD_PP_OAM_FTMH_LB_EXTENSION_SIZE 1 /* 8 bits */
#define _ARAD_PP_OAM_FTMH_DSP_EXTENSION_SIZE 2 /*16 bits*/
#define _ARAD_PP_OAM_OAM_TS_SIZE 6 /* 6 bytes */
#define _ARAD_PP_OAM_PPH_BASE_SIZE 7 /* 7 bytes */
#define _ARAD_PP_OAM_FHEI_3B_SIZE 3 /* 3 bytes */
#define _ARAD_PP_OAM_PACKET_OFFSET_SET(header_size, offset_byte, offset_word, offset_bit) \
    offset_byte -= header_size; \
    offset_word = offset_byte / 4; \
    offset_bit = (offset_byte % 4) * 8; 
    

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(channel_type_ndx, channel_type)       \
  do {                                                                                               \
    switch (channel_type_ndx) {                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:                                              \
         channel_type = soc_property_get(unit, spn_MPLSTP_BFD_CONTROL_CHANNEL_TYPE, 0x0007);    \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PW_ACH_ENUM:                                                   \
         channel_type = soc_property_get(unit, spn_MPLSTP_PW_ACH_CHANNEL_TYPE, 0x0007);         \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DLM_ENUM:                                                      \
         channel_type = soc_property_get(unit, spn_MPLSTP_DLM_CHANNEL_TYPE, 0x000A);            \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ILM_ENUM:                                                      \
         channel_type = soc_property_get(unit, spn_MPLSTP_ILM_CHANNEL_TYPE, 0x000B);            \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DM_ENUM:                                                       \
         channel_type = soc_property_get(unit, spn_MPLSTP_DM_CHANNEL_TYPE, 0x000C);             \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV4_ENUM:                                                     \
         channel_type = soc_property_get(unit, spn_MPLSTP_IPV4_CHANNEL_TYPE, 0);                \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV6_ENUM:                                                     \
         channel_type = soc_property_get(unit, spn_MPLSTP_IPV6_CHANNEL_TYPE, 0);                \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM:                                                       \
         channel_type = soc_property_get(unit, spn_MPLSTP_CC_CHANNEL_TYPE, 0x0022);             \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM:                                                       \
         channel_type = soc_property_get(unit, spn_MPLSTP_CV_CHANNEL_TYPE, 0x0023);             \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ON_DEMAND_CV_ENUM:                                             \
         channel_type = soc_property_get(unit, spn_MPLSTP_ON_DEMAND_CV_CHANNEL_TYPE, 0x0025);   \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM:                                               \
         channel_type = soc_property_get(unit, spn_MPLSTP_PWE_OAM_CHANNEL_TYPE, 0x0027);        \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:                                                    \
         channel_type = soc_property_get(unit, spn_MPLSTP_G8113_CHANNEL_TYPE, 0x8902);          \
         break;                                                                                      \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_FAULT_OAM:                                                    \
         channel_type = soc_property_get(unit, spn_MPLSTP_FAULT_OAM_CHANNEL_TYPE, 0x8902);          \
         break;                                                                                      \
    default:                                                                                         \
         channel_type = 0;                                                                           \
    }                                                                                                \
  } while (0) 

#define _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_FORMAT(channel_type, channel_format)               \
  do {                                                                                               \
    switch (channel_type) {                                                                          \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:                                                    \
        channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731 ; break;                             \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM:                                                       \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM:                                                       \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:                                              \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV4_ENUM:                                                     \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV6_ENUM:                                                     \
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ON_DEMAND_CV_ENUM:                                             \
        channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW ; break;                           \
    default:                                                                                         \
         channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_OAM_UNKNOWN;                              \
    }                                                                                                \
  } while (0)

#define _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(opcode)                                                                   \
    (( (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) ||            \
      (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR) || (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM) ||            \
      (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM) ||            \
      (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM)) ? 4 : (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR)? 12 : \
       (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR)? 20 :(opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM)? \
	   /* Stamping on CCMs is only done in the case of piggy-backed CCMs. in which case offset is 4 + 4 (sequence #), + 2 (MEP-ID) + 48 (MEG-ID) */ 58 :  0)

#define _ARAD_PP_OAM_MPLS_TCAM_SUPPORTED_OPCODE(opcode) \
  ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) || \
   (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM) || \
   (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM) || \
   (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR))

#define _ARAD_PP_OAM_MIM_TCAM_SUPPORTED_OPCODE(opcode) \
  ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) || \
   (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM) || \
   (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR))

#define _ARAD_PP_OAM_TCAM_NEW_ENTRY(oam_tcam_entry_key, oam_tcam_entry_action, tcam_entry_id)                      \
  do{                                                                                                              \
      ARAD_TCAM_ENTRY _tcam_entry;                                                                                 \
      ARAD_TCAM_ACTION _tcam_action;                                                                               \
      uint32 _success;                                                                                             \
      ARAD_TCAM_ENTRY_clear(&_tcam_entry);                                                                         \
      ARAD_TCAM_ACTION_clear(&_tcam_action);                                                                       \
      /* map action to tcam action buffer */                                                                       \
      res = arad_pp_oam_tcam_entry_to_action(&oam_tcam_entry_action ,&_tcam_action);                               \
      SOC_SAND_CHECK_FUNC_RESULT(res, 9991, exit);                                                                 \
      /* map key to tcam key buffer */                                                                             \
      arad_pp_oam_tcam_entry_build(unit , &oam_tcam_entry_key, &_tcam_entry);                                      \
      res = arad_tcam_managed_db_entry_add_unsafe(                                                                 \
                unit,                                                                                              \
                ARAD_PP_OAM_IDENTIFICATION,                                                                        \
                tcam_entry_id,                                                                                     \
                FALSE,                                                                                             \
                1,                                                                                                 \
                &_tcam_entry,                                                                                      \
                &_tcam_action,                                                                                     \
                &_success                                                                                          \
            );                                                                                                     \
      SOC_SAND_CHECK_FUNC_RESULT(res, 9992, exit);                                                                 \
      if(_success != SOC_SAND_SUCCESS) {                                                                           \
            LOG_ERROR(BSL_LS_SOC_OAM,                                                                              \
                      (BSL_META_U(unit,                                                                            \
                                  "   Error in OAM database entry add. \n\r")));                                   \
            LOG_ERROR(BSL_LS_SOC_OAM,                                                                              \
                      (BSL_META_U(unit,                                                                            \
                                  "   Tcam_last_entry_id:%d\n\r"), tcam_entry_id));                                \
            res = SOC_SAND_FAILURE_OUT_OF_RESOURCES;                                                               \
            SOC_SAND_CHECK_FUNC_RESULT(res, 9993, exit);                                                           \
      }                                                                                                            \
  }while (0)

#define _ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address) \
  (table_index+(dst_mac_address->address[0]<<8))

/*ACTIVE = ( EGRESS && UP ) || (INGRESS && DOWN )*/
#define IS_INGRESS_WHEN_ACTIVE(is_upmep)  (is_upmep == 0) 

#define _ARAD_PP_OAM_IS_SHORT_FORMAT(icc_ndx)          (icc_ndx == 15)

#define _ARAD_PP_OAM_IS_OEM1_ENTRY(oam_id)          (oam_id & 0x10000)

#define _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET(mep_type, mep_index, rmep_key, rmep_id)                                                        \
  do {                                                                                                                            \
      uint64 reg_val64, field64;                                                                                                \
         soc_field_t field_name;\
      uint16 field;                                                                                                                \
      if ((mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) ||                                                                             \
          (mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||                                                                            \
          (mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) {                                                                            \
          rmep_key = _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET_FROM_MEP_INDEX_AND_RMEP_ID(mep_index, rmep_id, _ARAD_PP_OAM_RMEP_EMC_SHIFT_ETH);\
      }                                                                                                                            \
      else {                                                                                                                    \
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 9999, exit, READ_OAMP_BFD_EMC_CONSTr(unit, &reg_val64));                        \
          switch (mep_type) {                                                                                                    \
          case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:                                                                            \
          case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:                                                                            \
              field64 = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, BFD_IPV4_EMC_CONSTf);                    \
              break;                                                                                                            \
          case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE:                                                                                    \
              field64 = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, BFD_PWE_EMC_CONSTf);                        \
              break;                                                                                                            \
          case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS:                                                                                    \
              field64 = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, BFD_MPLS_EMC_CONSTf);                    \
              break;                                                                                                            \
          case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP:                                                                                \
               field_name = SOC_IS_ARADPLUS(unit)? BFD_PWE_EMC_CONSTf : BFDCC_EMC_CONSTf; /*In Arad+ MPLS-TP is used via a PWE entry */\
              field64 = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, field_name);                        \
              break;                                                                                                            \
          default:                                                                                                                \
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 9999, exit);                                                    \
          }                                                                                                                        \
          field = COMPILER_64_LO(field64);                                                                                        \
          rmep_key = _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET_FROM_MEP_INDEX_AND_RMEP_ID(mep_index, field, _ARAD_PP_OAM_RMEP_EMC_SHIFT_BFD);\
      }                                                                                                                            \
  } while (0)

#define _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET_FROM_MEP_INDEX_AND_RMEP_ID(local_mep_db_index, rmep_id, shift) \
    (((local_mep_db_index) << shift) + (rmep_id))

/* ARAD+ new values in OEMA and OAMA */

/* Key is { {your-disrc(1, Jericho only), OAM-LIF (16 for Arad, 18 for Jericho) ingress(1)}*/
#define _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY(key_struct, key) \
    key = (key_struct.ingress) + (key_struct.oam_lif << 1) + SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit) * (key_struct.your_discr <<19)

#define _ARAD_PP_OAM_OEM1_KEY_TO_KEY_STRUCT(key_struct, key) \
	   do {\
       key_struct.ingress = key & 1; \
       if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
          key_struct.oam_lif = (key & 0x7FFFE) >>1;\
          key_struct.your_discr = (key & 0x80000) >> 19;\
       } else {\
          key_struct.oam_lif = (key & 0xfffe ) >>1;}\
       } while (0)
    
/* OEM1 payload:
    Jericho: { profile(4) ,counter (15), mp-type_vector(16)}
    Arad+: { profile[0:1](2) ,counter (13), mep-bitmap(8),mip-bitmap(6),profile[3:2](2)}
    Arad: { profile(2) ,counter (13), mep-bitmap(8),mip-bitmap(8)}
   */
#define _ARAD_PP_OAM_OEM1_PAYLOAD_STRUCT_TO_PAYLOAD(payload_struct, payload)                                                                                \
  do {                                                                                                                                                      \
      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
           payload[0] = (payload_struct.mp_type_vector) | (payload_struct.counter_ndx << 16) | ((payload_struct.mp_profile &1) <<31);\
          payload[1] = payload_struct.mp_profile >>1;\
      }\
      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)){                                                                                                                     \
          payload[0] = (((payload_struct.mp_profile & 0x3) << 29) + (payload_struct.counter_ndx << 16) + (payload_struct.mep_bitmap << 8) +                    \
                     (payload_struct.mip_bitmap << 2) + ((payload_struct.mp_profile & 0xc) >> 2));                                                          \
      }                                                                                                                                                     \
        else {                                                                                                                                                \
          payload[0] = (payload_struct.mp_profile << 29) + (payload_struct.counter_ndx << 16) + (payload_struct.mep_bitmap << 8) + (payload_struct.mip_bitmap);\
      }                                                                                                                                                     \
  } while (0) 
     

#define _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(payload, payload_struct)                    \
  do {                                                                                          \
      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
          payload_struct.counter_ndx = (payload[0] >> 16) & 0x7fff;\
          payload_struct.mp_type_vector = payload[0] & 0xffff;\
          payload_struct.mp_profile = (payload[0] >>31) | ((payload[1] & 0x7)<<1);\
      }\
      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {                                                         \
          payload_struct.mp_profile  = (((payload[0] >> 29) & 0x3) + ((payload[0] & 0x3) << 2)) ;       \
          payload_struct.counter_ndx = (payload[0] >> 16) & 0x1fff;    \
          payload_struct.mep_bitmap  = (payload[0] >> 8)  & 0xff;      \
          payload_struct.mip_bitmap  = (payload[0] >> 2)  & 0x3f;      \
      }                                                             \
      else {                                                        \
        payload_struct.mp_profile  = (payload[0] >> 29) & 0x3;         \
        payload_struct.counter_ndx = (payload[0] >> 16) & 0x1fff;      \
        payload_struct.mep_bitmap  = (payload[0] >> 8)  & 0xff;        \
        payload_struct.mip_bitmap  = (payload[0])       & 0xff;        \
      }                                                                                        \
  } while (0)

#define OAM_PROCESS_ACTION_INGRESS_STAMP_1588 1
#define OAM_PROCESS_ACTION_EGRESS_STAMP_1588 2
#define OAM_PROCESS_ACTION_INGRESS_STAMP_NTP 3
#define OAM_PROCESS_ACTION_EGRESS_STAMP_NTP 4
#define OAM_PROCESS_ACTION_STAMP_COUNTER 5
#define OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP 6

/* Macro writes action type in the OAM_PROCESS_MAP register according to the action_type, one of the defines above.*/
#define OAM_PROCESS_ADD(subtype, up_1_down_0, action_type, reg_to_write_on ) \
 do {\
    int index = subtype <<2 | up_1_down_0 << 1;\
    uint32 what_to_write = action_type;\
    SHR_BITCOPY_RANGE(reg_to_write_on, index*3, &what_to_write, 0, 3 );\
} while (0); 

/* OEM2 key: {your-discr(1), OAM-LIF(16 for Arad, 18 for Jericho) mdl(3) ingress(1)}*/
#define _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY(key_struct, key) \
     key = (key_struct.your_disc << (20 + 2 * SOC_IS_JERICHO(unit))) + (key_struct.oam_lif << 4) + (key_struct.mdl << 1) + (key_struct.ingress)                     
      
#define _ARAD_PP_OAM_OEM2_KEY_TO_KEY_STRUCT(key_struct, key)\
do { key_struct.ingress = key & 0x1;\
	key_struct.mdl = (key & 0xe) >>1;\
	key_struct.oam_lif = (key & (SOC_IS_JERICHO(unit)? 0x3ffff0: 0xffff0)) >>4;\
	key_struct.your_disc = ((key ) >>(20 + 2 * SOC_IS_JERICHO(unit))) & 1 ;\
} while (0)

/* OEM2 payload: {OAM-ID (14 for Jericho, 13 for Arad),profile(4)}*/
#define _ARAD_PP_OAM_OEM2_PAYLOAD_STRUCT_TO_PAYLOAD(payload_struct, payload) \
    payload = (payload_struct.mp_profile) + (payload_struct.oam_id << 4)  

#define _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(payload, payload_struct) \
  do {                                                                       \
    payload_struct.mp_profile  = (payload)       & 0xf;                      \
    payload_struct.oam_id       = (payload >> 4)  & 0x1fff;                   \
  } while (0)

/*Arad: Map {Inject(1), My-CFM-MAC(1), OAM-is-BFD (1), OAM-Opcode (4), Direction(1), MIP-MP-Type (2), MEP-MP-Type (2), OAM-1-Acc-MEP-Profile (2)} 
  Jericho: Map {Inject(1), My-CFM-MAC(1) , MP-Type[2], OAM-Opcode (4), Direction(1), MP-Type [1:0], OAM-1-Acc-MEP-Profile (4)}*/
#define _ARAD_PP_OAM_OAM1_KEY_STRUCT_TO_KEY(key_struct, key)                                                                            \
  do {                                                                                                                                  \
  if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
       key = (key_struct.mp_profile) + ((key_struct.mp_type_jr &0x3) << 4) + (key_struct.ingress << 6) +  (key_struct.opcode << 7)  + (((key_struct.mp_type_jr &0x4)>>2) << 11) +\
                        (key_struct.inject << 13) + (key_struct.my_cfm_mac << 12);\
  } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {                                                                                                   \
        key = (key_struct.inject << 13) + (key_struct.my_cfm_mac << 12) + (key_struct.is_bfd << 11) + (key_struct.opcode << 7) +        \
             (key_struct.ingress << 6) + (key_struct.mp_type << 4) + (key_struct.mp_profile);                                           \
    }                                                                                                                                   \
    else {                                                                                                                              \
        key = (key_struct.inject << 13) + (key_struct.my_cfm_mac << 12) + (key_struct.is_bfd << 11) + (key_struct.opcode << 7) +        \
             (key_struct.ingress << 6) + (key_struct.mip_type << 4) + (key_struct.mep_type << 2) + (key_struct.mp_profile);             \
    }                                                                                                                                   \
  } while (0)

#define _ARAD_PP_OAM_OAM1_KEY_TO_KEY_STRUCT(key, key_struct)                                                                            \
      do {                                                                                                                                  \
      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
           key_struct.mp_profile = key & 0xf; \
           key_struct.mp_type_jr = ((key >> 4) & 0x3) | (((key >> 11) & 0x1) << 2); \
           key_struct.ingress = (key >> 6) & 0x1; \
           key_struct.opcode = (key >> 7) & 0xf; \
           key_struct.my_cfm_mac = (key >> 12) & 0x1; \
		   key_struct.inject = (key >> 13) & 0x1; \
      } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {                                                                                                   \
           key_struct.mp_profile = key & 0xf; \
           key_struct.mp_type = ((key >> 4) & 0x3); \
           key_struct.ingress = (key >> 6) & 0x1; \
           key_struct.opcode = (key >> 7) & 0xf; \
           key_struct.is_bfd = (key >> 11) & 0x1; \
           key_struct.my_cfm_mac = (key >> 12) & 0x1; \
		   key_struct.inject = (key >> 13) & 0x1; \
        }                                                                                                                                   \
        else {                                                                                                                              \
           key_struct.mp_profile = key & 0x3; \
           key_struct.mep_type = ((key >> 2) & 0x3); \
           key_struct.mp_type = ((key >> 4) & 0x3); \
           key_struct.ingress = (key >> 6) & 0x1; \
           key_struct.opcode = (key >> 7) & 0xf; \
           key_struct.is_bfd = (key >> 11) & 0x1; \
           key_struct.my_cfm_mac = (key >> 12) & 0x1; \
		   key_struct.inject = (key >> 13) & 0x1; \
        }                                                                                                                                   \
      } while (0)


/*Map {Inject(1), My-CFM-MAC (1), OAM-is-BFD (1), OAM-Opcode (4), Direction(1), OAM-2-Prof (4)}*/
#define _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY(key_struct, key)                                                         \
    key = (key_struct.inject << 11) + (key_struct.my_cfm_mac << 10) + (key_struct.is_bfd << 9) + (key_struct.opcode << 5) + \
          (key_struct.ingress << 4) + (key_struct.mp_profile) 

/*msb = dst_mac_address[47:8]*/
#define _ARAD_PP_OAM_MAC_ADD_MSB_GET(mac_add_U32, msb_U64)       \
  do {                                                        \
    COMPILER_64_SET(msb_U64, mac_add_U32[1], mac_add_U32[0]); \
    COMPILER_64_SHR(msb_U64, 8);                              \
  } while (0)

#define _ARAD_PP_OAM_MAC_ADD_MSB_SET(msb_U64, mac_add_U32)                           \
  do {                                                                                \
    uint64 msb_U64_temp;                                                             \
    COMPILER_64_SET(msb_U64_temp, COMPILER_64_HI(msb_U64), COMPILER_64_LO(msb_U64)); \
    COMPILER_64_SHL(msb_U64_temp, 8);                                                 \
    mac_add_U32[0] = COMPILER_64_LO(msb_U64_temp);                                     \
    mac_add_U32[1] = COMPILER_64_HI(msb_U64_temp);                                     \
  } while (0)

#define _ARAD_PP_OAM_MAC_ADD_LSB_SET(lsb, mac_add_U32) \
  mac_add_U32[0] |= lsb /*LSB = bits 0:7*/


/* Complete RMEP scan that is 16384 * 20 * clock-period */
#define _ARAD_PP_OAM_RMEP_SCAN(unit, freq_arad_khz)    \
      SOC_SAND_DIV_ROUND_DOWN(SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit) * 20, SOC_SAND_DIV_ROUND_DOWN(freq_arad_khz, 1000))
 
#define _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(mep_type)  \
  ((mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||       \
   (mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ||       \
   (mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS && SOC_IS_JERICHO(unit)) )
 
#define _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(data, addr)        \
  do {                                                                                                   \
      uint32 tod_reg = 0;                                                                                \
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9999,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_TOD_COMMAND_WR_DATAr, REG_PORT_ANY, 0, TOD_COMMAND_WR_DATAf,  data));                    \
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 9999, exit, READ_ECI_TOD_COMMANDr(unit, &tod_reg));        \
      soc_reg_field_set(unit, ECI_TOD_COMMANDr, &tod_reg, TOD_COMMAND_TRIGGERf, 1);                \
      soc_reg_field_set(unit, ECI_TOD_COMMANDr, &tod_reg, TOD_COMMAND_OP_CODEf, 1);                \
      soc_reg_field_set(unit, ECI_TOD_COMMANDr, &tod_reg, TOD_COMMAND_ADDRESSf, addr);                \
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 9999, exit, WRITE_ECI_TOD_COMMANDr(unit, tod_reg));        \
  } while (0)
     

#define _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit, trap_code, internal_trap_code)                                                       \
  do {                                                                                                                            \
      ARAD_SOC_REG_FIELD                          strength_fld_fwd;                                                               \
      ARAD_SOC_REG_FIELD                          strength_fld_snp;                                                               \
      res = arad_pp_trap_mgmt_trap_code_to_internal(unit, trap_code, &internal_trap_code, &strength_fld_fwd, &strength_fld_snp);      \
      SOC_SAND_CHECK_FUNC_RESULT(res, 9999, exit);                                                                               \
  } while (0)

/* MEP SCAN in Arad is 3.33 MS, 1.67 MS in Jericho */
#define ARAD_PP_OAM_NUM_CLOCKS_IN_MEP_SCAN  SOC_IS_JERICHO(unit)? (167*arad_chip_kilo_ticks_per_sec_get(unit))/100 :( 333*arad_chip_kilo_ticks_per_sec_get(unit))/100

/* LMMs and related packets are always counted according to the COUNTER_INCREMENT register (counter_disable must be set for stamping),
   other packets through the counter disable field. */                    
#define _ARAD_PP_OAM_SET_COUNTER_DISABLE(opcode, internal_opcode, oam_payload, profile_data, is_piggy_backed) \
  do { \
      uint32 counter_inc_reg;       \
      if ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR) ||  \
          (opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM)  ) { \
           oam_payload->counter_disable = 0; /* Counter disable must be set to 0 to allow stamping for LMM and related packet types (inclding piggy-backed CCMs)*/ \
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9999,  exit, ARAD_REG_ACCESS_ERR,\
					soc_reg_above_64_field32_read(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, REG_PORT_ANY, 0, OAM_COUNTER_INCREMENTf, &counter_inc_reg));   \
               if (SHR_BITGET(&profile_data.counter_disable, internal_opcode) != 0) {       \
                   counter_inc_reg &= (0xff & (~(1 << _ARAD_PP_OAM_SUBTYPE_LM))); /* Piggy-backed CCMs included (uses same sub-type)*/ \
                  }              \
              else {          \
                   counter_inc_reg |= (1 << _ARAD_PP_OAM_SUBTYPE_LM);         \
               }       \
               SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9999,  exit, ARAD_REG_ACCESS_ERR,\
					soc_reg_above_64_field32_modify(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, REG_PORT_ANY, 0, OAM_COUNTER_INCREMENTf,  counter_inc_reg));  \
    }  else if ( (is_piggy_backed && opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM)) {  \
  	  oam_payload->counter_disable = 0; /* Counter disable must be set to 0 to allow stamping for LMM and related packet types (inclding piggy-backed CCMs)*/ \
	 } else {\
       oam_payload->counter_disable = (SHR_BITGET(&profile_data.counter_disable, internal_opcode) != 0);  \
    }\
} while (0)

#define _ARAD_PP_DEFAULT_MIP_BEHAVIOR_GET(opcode, is_my_cfm_mac, action)                                                    \
  do {                                                                                                                        \
      if (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM) {                                                                            \
             if (is_my_cfm_mac) {                                                                                                \
               action = _ARAD_PP_OAM_ACTION_RECYCLE;                                                                        \
          }                                                                                                                    \
          else {                                                                                                            \
              action = _ARAD_PP_OAM_ACTION_FRWRD;                                                                            \
          }                                                                                                                    \
      }                                                                                                                         \
      else if (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTM) {                                                                        \
             if (is_my_cfm_mac) {                                                                                                \
               action = _ARAD_PP_OAM_ACTION_TRAP_CPU;                                                                        \
          }                                                                                                                    \
          else {                                                                                                            \
              action = _ARAD_PP_OAM_ACTION_SNOOP;                                                                            \
          }                                                                                                                    \
      }                                                                                                                        \
      else {                                                                                                                \
          action = _ARAD_PP_OAM_ACTION_FRWRD;                                                                                \
      }                                                                                                                        \
  } while (0)

/* Sets a variable in the WB engine and adds a tcam entry.*/
#define ADD_TCAM_ENTRY_AND_SET_WB_VAR(oam_tcam_entry_key, oam_tcam_entry_action) \
  do {  \
        uint32 tcam_last_id;  \
        res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID, &tcam_last_id); \
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);  \
        _ARAD_PP_OAM_TCAM_NEW_ENTRY(oam_tcam_entry_key, oam_tcam_entry_action, tcam_last_id); \
        ++tcam_last_id; \
        res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID, &tcam_last_id); \
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);  \
  } while (0)


/**
 * In the passive side the mep bitmep isn't a bit mep, rather (3
 * bits for the one level ) <<3 | 3 bits for another level)
 */
#define SET_MIP_BITMEP_FROM_MEP_BITMEP(mip_bitmep, mep_bitmep)\
    do{\
    uint32 i=2,mep_entered =0,level ;\
    mip_bitmep = 0;\
    for (level=1; level<=7 ;i<<=1 , ++level) {\
        if (mep_bitmep & i) {\
            mip_bitmep |= level << (3 * mep_entered);\
            ++mep_entered;\
        }\
    }\
    if (mep_entered >2) {\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Only 2 endpoints may be entered per direction per LIF.")));\
    }\
}while (0)

#define _NUMBER_OF_COUNTER_PROCESSORS(_u) ((SOC_IS_PETRAB(_u))? 2 : 4)



/* parse the OEMA/B_MANAGEMENT_UNIT_FAILURE*/
#define ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_on_reg,inserted_key ,number_on_fail_reason_reg, oem_table) \
    if (fail_key_on_reg!=inserted_key) { \
        LOG_WARN(BSL_LS_SOC_OAM, \
                 (BSL_META_U(unit, \
                             "Warning: write to OEM%d failed with different key\n"), \
                  oem_table)); \
        goto exit; \
    }\
    if (number_on_fail_reason_reg & 1) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change non-exist from self."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<1)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change non-exist from other."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<2)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change request over static."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<3)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change fail non existent."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<4)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Learn over existing ."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<5)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Learn request over static."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<6)) {\
		  LOG_DEBUG(BSL_LS_SOC_OAM, \
                            (BSL_META_U(unit, \
                                        "OEM%d warning: replacing existing entry, key: 0x%x\n"),oem_table, inserted_key)); \
    }\
    if (number_on_fail_reason_reg & (1<<7)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Reach max entry limit."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<8)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: delete unknown key."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<9)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Table coherency."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<10)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Cam table full."),oem_table));\
    }


/*
 * Macro inserts a value into the header stack (pkt_header_reg). 
 *  bytes_before_cur_header - location of header in stack.
 *  header_size - size of header (in bytes).
 *  bit_no_in_header - starting bit number of value
 *  field_size - size of field inserted
 *  value - value inserted
 *  
*/
#define INSERT_INTO_REG(pkt_header_reg, bytes_before_cur_header, header_size , bit_no_in_header, field_size, value) \
 do {\
    uint32 field = value;\
    INSERT_INTO_REG_BUFFER(pkt_header_reg, bytes_before_cur_header, header_size , bit_no_in_header, field_size, &field);\
} while (0)


/*
 * Like previous macro but here last paramter is a buffer (i.e. uint32*)
*/
#define INSERT_INTO_REG_BUFFER(pkt_header_reg, bytes_before_cur_header, header_size, bit_no_in_header, field_size, value_buffer) \
 do {\
    uint32 _offset = 640 - (bytes_before_cur_header *8   + header_size *8) + bit_no_in_header ;\
    SHR_BITCOPY_RANGE(pkt_header_reg, _offset,(uint32*) value_buffer, 0,field_size );\
} while (0)



#define OAM_LIF_MAX_VALUE(unit)  ((1 <<SOC_PPC_OAM_SIZE_OF_OAM_KEY_IN_BITS(unit)) -1)


/**************************LM/DM macros*/


/**
 * Macro enters what type of entry entry #__entry_id is. result 
 * goes in first parameter. 
 */
#define GET_TYPE_OF_OAMP_MEP_DB_ENTRY(__type, __entry_id, __reg_above_64)\
        SOC_REG_ABOVE_64_CLEAR(__reg_above_64);\
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,__entry_id, __reg_above_64));\
        __type = soc_OAMP_MEP_DB_DM_STATm_field32_get(unit, __reg_above_64, MEP_TYPEf) 

/**
 * Macro clears an entry in the OAMP MEP DB.
 */
#define CLEAR_OAMP_MEP_DB_ENTRY(__entry_id, _reg_above_64) \
        SOC_REG_ABOVE_64_CLEAR(_reg_above_64);\
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, __entry_id, _reg_above_64))

/**
 * Macro retreives mep_profile field from OAMP_MEP_DB entry.
 */
#define GET_MEP_AND_DA_NIC_PROFILES_FROM_OAMP_MEP_DB_ENTRY(__profile, da_nic_profile,  __mep_type ,__endpoint_id, __reg_data) \
    do{\
        soc_mem_t mem;\
         switch (__mep_type) {\
        case SOC_PPD_OAM_MEP_TYPE_ETH_OAM:\
            mem = OAMP_MEP_DBm;\
            break;\
        case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP:\
            mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;\
            break;\
        case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE:\
            mem = OAMP_MEP_DB_Y_1731_ON_PWEm;\
            break;\
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:\
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:\
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE:\
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE_GAL:\
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS:\
        case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP:\
        case SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP:\
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Accelerated LM/DM in HW usupported for BFD.")));\
            break;\
        default:\
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error: CCM entry type invalid"))); break;\
        }\
        SOC_REG_ABOVE_64_CLEAR(__reg_data); \
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_mem_read(unit, mem, MEM_BLOCK_ANY, __endpoint_id, __reg_data));        \
       __profile =   soc_mem_field32_get(unit, mem, __reg_data, MEP_PROFILEf); \
    da_nic_profile = (__mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM)?soc_OAMP_MEP_DBm_field32_get(unit, __reg_data, LMM_DA_NIC_PROFILEf):-1;\
    }while (0); 

          /**
 * In lieu of a table for the eth_1731_mep_profile table, we 
 * must use these macros. 
 */
#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_LMM_RATE(_field, _index) \
do {\
        soc_reg_t lmm_rate_fields[] = {PROF_0_LMM_RATEf, PROF_1_LMM_RATEf, PROF_2_LMM_RATEf, PROF_3_LMM_RATEf, PROF_4_LMM_RATEf, PROF_5_LMM_RATEf, PROF_6_LMM_RATEf, \
        PROF_7_LMM_RATEf, PROF_8_LMM_RATEf, PROF_9_LMM_RATEf, PROF_10_LMM_RATEf, PROF_11_LMM_RATEf, PROF_12_LMM_RATEf, PROF_13_LMM_RATEf, PROF_14_LMM_RATEf, \
        PROF_15_LMM_RATEf};\
        _field = lmm_rate_fields[_index];\
} while (0); 


#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_DMM_RATE(_field, _index) \
do {\
    soc_reg_t dmm_rate_fields[] = {PROF_0_DMM_RATEf,PROF_1_DMM_RATEf,PROF_2_DMM_RATEf,PROF_3_DMM_RATEf,PROF_4_DMM_RATEf,PROF_5_DMM_RATEf,\
        PROF_6_DMM_RATEf,PROF_7_DMM_RATEf,PROF_8_DMM_RATEf,PROF_9_DMM_RATEf,PROF_10_DMM_RATEf,PROF_11_DMM_RATEf,PROF_12_DMM_RATEf,PROF_13_DMM_RATEf,\
        PROF_14_DMM_RATEf,PROF_15_DMM_RATEf};\
    _field = dmm_rate_fields[_index];\
} while (0); 

#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_PIGGY_BACK(_field, _index) \
do {\
    soc_reg_t piggy_back_lm_fields[] = {PROF_0_PIGGYBACK_LMf, PROF_1_PIGGYBACK_LMf, PROF_2_PIGGYBACK_LMf, PROF_3_PIGGYBACK_LMf, PROF_4_PIGGYBACK_LMf, PROF_5_PIGGYBACK_LMf, \
        PROF_6_PIGGYBACK_LMf, PROF_7_PIGGYBACK_LMf, PROF_8_PIGGYBACK_LMf, PROF_9_PIGGYBACK_LMf, PROF_10_PIGGYBACK_LMf, PROF_11_PIGGYBACK_LMf, PROF_12_PIGGYBACK_LMf, \
        PROF_13_PIGGYBACK_LMf, PROF_14_PIGGYBACK_LMf,PROF_15_PIGGYBACK_LMf };\
    _field = piggy_back_lm_fields[_index];\
} while (0); 

#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_DA_OUI_PROF(_field, _index) \
do {\
    soc_reg_t lmm_da_oui_prof_fields[] = {PROF_0_LMM_DA_OUI_PROFILEf, PROF_1_LMM_DA_OUI_PROFILEf, PROF_2_LMM_DA_OUI_PROFILEf, PROF_3_LMM_DA_OUI_PROFILEf, \
        PROF_4_LMM_DA_OUI_PROFILEf, PROF_5_LMM_DA_OUI_PROFILEf, PROF_6_LMM_DA_OUI_PROFILEf, PROF_7_LMM_DA_OUI_PROFILEf, PROF_8_LMM_DA_OUI_PROFILEf, \
        PROF_9_LMM_DA_OUI_PROFILEf, PROF_10_LMM_DA_OUI_PROFILEf, PROF_11_LMM_DA_OUI_PROFILEf, PROF_12_LMM_DA_OUI_PROFILEf, PROF_13_LMM_DA_OUI_PROFILEf, \
        PROF_14_LMM_DA_OUI_PROFILEf, PROF_15_LMM_DA_OUI_PROFILEf};\
     _field = lmm_da_oui_prof_fields[_index];\
} while (0); 

#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_RDI_GEN_METHOD(_field, _index) \
do {\
    soc_reg_t rdi_fields_fields[]  = { PROF_0_RDI_GEN_METHODf,PROF_1_RDI_GEN_METHODf,PROF_2_RDI_GEN_METHODf,PROF_3_RDI_GEN_METHODf,PROF_4_RDI_GEN_METHODf,\
    PROF_5_RDI_GEN_METHODf,PROF_6_RDI_GEN_METHODf,PROF_7_RDI_GEN_METHODf,PROF_8_RDI_GEN_METHODf,PROF_9_RDI_GEN_METHODf,PROF_10_RDI_GEN_METHODf,\
        PROF_11_RDI_GEN_METHODf,PROF_12_RDI_GEN_METHODf,PROF_13_RDI_GEN_METHODf,PROF_14_RDI_GEN_METHODf,PROF_15_RDI_GEN_METHODf,};\
     _field = rdi_fields_fields[_index];\
} while (0); 


    
#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMM_OFFSET(field,index) \
do {\
    soc_reg_t lmm_fields[] = {LMM_OFFSET_PROFILE_0f,LMM_OFFSET_PROFILE_1f,LMM_OFFSET_PROFILE_2f,LMM_OFFSET_PROFILE_3f,\
    LMM_OFFSET_PROFILE_4f,LMM_OFFSET_PROFILE_5f,LMM_OFFSET_PROFILE_6f,LMM_OFFSET_PROFILE_7f,LMM_OFFSET_PROFILE_7f,\
    LMM_OFFSET_PROFILE_8f,LMM_OFFSET_PROFILE_9f,LMM_OFFSET_PROFILE_10f,LMM_OFFSET_PROFILE_11f,LMM_OFFSET_PROFILE_12f,\
    LMM_OFFSET_PROFILE_13f,LMM_OFFSET_PROFILE_14f,LMM_OFFSET_PROFILE_15f};\
     field = lmm_fields[index];\
} while (0); 


#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMM_OFFSET(field, index) \
do {\
    soc_reg_t dmm_fields[] = {DMM_OFFSET_PROFILE_0f,DMM_OFFSET_PROFILE_1f,DMM_OFFSET_PROFILE_2f,DMM_OFFSET_PROFILE_3f,\
    DMM_OFFSET_PROFILE_4f,DMM_OFFSET_PROFILE_5f,DMM_OFFSET_PROFILE_6f,DMM_OFFSET_PROFILE_7f,DMM_OFFSET_PROFILE_7f,\
    DMM_OFFSET_PROFILE_8f,DMM_OFFSET_PROFILE_9f,DMM_OFFSET_PROFILE_10f,DMM_OFFSET_PROFILE_11f,DMM_OFFSET_PROFILE_12f,\
    DMM_OFFSET_PROFILE_13f,DMM_OFFSET_PROFILE_14f,DMM_OFFSET_PROFILE_15f};\
     field = dmm_fields[index];\
} while (0); 

#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMR_OFFSET(field, index) \
do {\
    soc_reg_t dmr_fields[] = {DMR_OFFSET_PROFILE_0f,DMR_OFFSET_PROFILE_1f,DMR_OFFSET_PROFILE_2f,DMR_OFFSET_PROFILE_3f,\
    DMR_OFFSET_PROFILE_4f,DMR_OFFSET_PROFILE_5f,DMR_OFFSET_PROFILE_6f,DMR_OFFSET_PROFILE_7f,DMR_OFFSET_PROFILE_7f,\
    DMR_OFFSET_PROFILE_8f,DMR_OFFSET_PROFILE_9f,DMR_OFFSET_PROFILE_10f,DMR_OFFSET_PROFILE_11f,DMR_OFFSET_PROFILE_12f,\
    DMR_OFFSET_PROFILE_13f,DMR_OFFSET_PROFILE_14f,DMR_OFFSET_PROFILE_15f};\
     field = dmr_fields[index];\
} while (0); 

#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMR_OFFSET(field, index) \
do {\
    soc_reg_t lmr_fields[] = {LMR_OFFSET_PROFILE_0f,LMR_OFFSET_PROFILE_1f,LMR_OFFSET_PROFILE_2f,LMR_OFFSET_PROFILE_3f,\
    LMR_OFFSET_PROFILE_4f,LMR_OFFSET_PROFILE_5f,LMR_OFFSET_PROFILE_6f,LMR_OFFSET_PROFILE_7f,LMR_OFFSET_PROFILE_7f,\
    LMR_OFFSET_PROFILE_8f,LMR_OFFSET_PROFILE_9f,LMR_OFFSET_PROFILE_10f,LMR_OFFSET_PROFILE_11f,LMR_OFFSET_PROFILE_12f,\
    LMR_OFFSET_PROFILE_13f,LMR_OFFSET_PROFILE_14f,LMR_OFFSET_PROFILE_15f};\
     field = lmr_fields[index];\
} while (0); 


/********************************************* Diagnostics macros */

#define PRINT_FOUND(found) \
     if (COMPILER_64_LO(found)) { \
         LOG_CLI((BSL_META_U(unit, \
                             ".\n"))); \
     } else { \
         LOG_CLI((BSL_META_U(unit, \
                             " (not found).\n"))); \
     }    


/* Following 2 macros were mostly copied shamelessly from arad_pp_diag.c*/
#define ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb)  \
          (prm_fld)->base = (prm_addr_msb << 16) + prm_addr_lsb;  \
          (prm_fld)->msb = prm_fld_msb;  \
          (prm_fld)->lsb= prm_fld_lsb;

/*This macro additionaly turns off all bits outside range start_bit - end_bit*/
#define ARAD_PP_OAM_DIAG_FLD_READ(prm_fld, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb,start_bit, end_bit)  \
    do {\
    uint32 __zero=0;\
    ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
      res = arad_pp_diag_dbg_val_get_unsafe(unit, prm_blk, prm_fld, regs_val);  \
      SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);\
     SHR_BITCOPY_RANGE(regs_val,end_bit+1,&__zero,0,31-end_bit-start_bit);\
         if (start_bit !=0) {\
             SHR_BITCOPY_RANGE(regs_val,0,&__zero,0,start_bit);\
         }\
    } while (0); 

/**
 * Presumably only arad and arad+ will use the following macros.
 */
#define GET_SIGNAL_FOR_DIRECTION(fld)                                   \
      if (SOC_IS_JERICHO(unit)) {                                       \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_IHP_ID, 4, 6, 113, 113, 0, 0);  \
      } else  if (SOC_IS_ARADPLUS(unit)) {                              \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,6,18,18,0,0 );      \
      }else {                                                           \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,5,136,136,0,0 );    \
      }

#define GET_SIGNAL_FOR_OAM_ID_UP(fld) \
      if (SOC_IS_JERICHO(unit)) {                                       \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_EPNI_ID, 9, 1, 180, 164, 0, 16);  \
      }else {                                                           \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_EPNI_ID, 9,1,25,9,0,16 );     \
      }
   


#define GET_SIGNAL_FOR_OAM_ID_DOWN(fld) \
      if (SOC_IS_JERICHO(unit)) {                                           \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_IHP_ID, 4, 6, 148, 132, 0, 16);   \
      } else  if (SOC_IS_ARADPLUS(unit)) {                                      \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,6,53,37,0,16);          \
      } else {                                                                  \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,5,171,155,0,16);        \
      }


#define GET_SIGNAL_FOR_SUBTYPE_UP(fld)\
      if (SOC_IS_JERICHO(unit)) {                                       \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_EPNI_ID, 9, 1, 163, 161, 0, 2);  \
      }else {                                                           \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_EPNI_ID, 9,1,8,6,0,2);     \
      }
        

#define GET_SIGNAL_FOR_SUBTYPE_DOWN(fld)\
      if (SOC_IS_JERICHO(unit)) {                                               \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_IHB_ID, 0, 9, 224, 222, 0, 2);      \
      } else  if (SOC_IS_ARADPLUS(unit)) {                                      \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 9,9,203,201 ,0,2);            \
      } else {                                                                  \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,5,139,137,0,2 );            \
      }

#define GET_SIGNAL_FOR_TRAP_CODE(fld)\
      if (SOC_IS_JERICHO(unit)) {                                                   \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_IHP_ID, 4, 8, 151, 144, 0, 7);       \
      } else  if (SOC_IS_ARADPLUS(unit)) {                                          \
        ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,7,208,201,0,7 );              \
      } else {                                                                      \
        ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,7,59,52,0,7 );                \
      }

#define GET_SIGNAL_FOR_OAM_SNOOP_STRENGTH(fld)\
      if (SOC_IS_JERICHO(unit)) {                                               \
          ARAD_PP_OAM_DIAG_FLD_READ(&fld, ARAD_IHP_ID, 4, 1, 251, 250, 0, 1);      \
      } else  if (SOC_IS_ARADPLUS(unit)) {                                      \
        ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,1,172,171,0,1 );              \
      } else {                                                                  \
        ARAD_PP_OAM_DIAG_FLD_READ(&fld,ARAD_IHB_ID, 0,1,34,33,0,1 );                \
      }

#define SUB_TYPE_MSG_FROM_CODE(str, code) \
    switch (code) {\
    case (0): str = "none"; break;\
    case (1):  str = "LM"; break;\
    case (2):  str = "DM 1588"; break;\
    case (3):  str = "DM NTP"; break;\
    case (4):  str = "CCM"; break;\
    case (5):  str = "LB"; break;\
    default: str = "";\
        LOG_ERROR(BSL_LS_SOC_OAM, \
                  (BSL_META_U(unit, \
                              "Illegal subtype retreived\n"))); \
                   break;\
}


                      /**/
#define _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key)\
do {\
    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
        LOG_CLI((BSL_META_U(unit,\
                                  "\tOEMA key: ingress: %d, Your-Discriminator: %d ,OAM LIF: 0x%x\n"),  oem1_key.ingress, oem1_key.your_discr,oem1_key.oam_lif)); \
    } else {\
        LOG_CLI((BSL_META_U(unit,\
                                  "\tOEMA key: ingress: %d, OAM LIF: 0x%x\n"),  oem1_key.ingress, oem1_key.oam_lif)); \
    }\
} while (0)


#define _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oam1_payload)\
do {\
    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
    int mdl, mdl_mp_type;\
        LOG_CLI((BSL_META_U(unit,\
                            "\t\tOEMA payload: MP profile: 0x%x, MP-type-vector: 0x%x, counter index: 0x%x\n"), oem1_payload.mp_profile, oem1_payload.mp_type_vector,\
                                        oem1_payload.counter_ndx)); \
                          for (mdl=0; mdl<8 ;++mdl) {\
                              mdl_mp_type =  JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, mdl);\
                              if (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP) {\
                                  LOG_CLI((BSL_META_U(unit, "\t\t\tMIP on Level %d\n"),mdl));\
                              }\
                              if (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH) {\
                                  LOG_CLI((BSL_META_U(unit, "\t\t\tActive MEP on Level %d\n"),mdl));\
                              }\
                              if (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH) {\
                                  LOG_CLI((BSL_META_U(unit, "\t\t\tPassive MEP on Level %d\n"),mdl));\
                              }\
                         }\
} else {\
            LOG_CLI((BSL_META_U(unit,\
                                "\t\tOEMA payload: MP profile: 0x%x, MEP bitmap: 0x%x, MIP bitmap: 0x%x, counter index: 0x%x\n"), oem1_payload.mp_profile, oem1_payload.mep_bitmap,oem1_payload.mip_bitmap,\
                                            oem1_payload.counter_ndx)); \
    }\
} while (0)


/* Enters the CCM entry into reg_data.
   Entry_type will be memory type, i.e. OAMP_MEP_DBm or OAMP_MEP_DB_Y_1731_ON_MPLSTPm etc.
   As it is should only be called when entry type is one of SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE,
   SOC_PPD_OAM_MEP_TYPE_ETH_OAM, SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP
   (this may be extended later)*/
#define GET_CCM_ENTRY(entry_index,  reg_data, entry_type)\
do {\
    uint32 ccm_type;\
    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(ccm_type, entry_index, reg_data); \
    switch (ccm_type) {\
    case SOC_PPD_OAM_MEP_TYPE_ETH_OAM:\
        entry_type = OAMP_MEP_DBm;\
        break;\
    case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP:\
        entry_type = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;\
        break;\
    case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE:\
        entry_type = OAMP_MEP_DB_Y_1731_ON_PWEm;\
        break;\
    default:\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Unsopported MEP DB entry type."))); break;\
    }\
    SOC_REG_ABOVE_64_CLEAR(reg_data); \
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_mem_read(unit, entry_type, MEM_BLOCK_ANY, entry_index, reg_data));       \
} while (0)



/* Macro rounds N up to the nearest power of 2.*/
#define ROUND_UP_TO_NEAREST_POWER_OF_2(N) \
    --N;  N |= N>>1 ; N |= N>>2 ; N|=N>>4  ; \
     N |= N>>8 ; N |= N>>16; ++N;

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 * Struct represents fields from PPH BASE 
 */
typedef struct {
    uint8   fhei_size;
    uint8   fwd_code;
    uint8   fwd_header_offset;
    uint8   pkt_is_ctrl;
    uint8   snoop_cpu_code;
    uint8   inLif_orintation;
    uint8   unknown_da_addr;
    uint8   learn_allowed;
    uint16  vsi_vrf;
    uint32  inLif_inRif; 
} PPH_base;



/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_oam[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_DEINIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_DEINIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MY_CFM_MAC_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MY_CFM_MAC_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_UNSAFE),  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_UNSAFE), 
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_EVENT_FIFO_READ_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_EVENT_FIFO_READ_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_EVENT_FIFO_READ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_GET),

  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_oam[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR,
    "ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR",
    "All MEPs on the same PP Port must share same DA[47:8] suffix.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR,
    "ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR",
    " All MEPs in a device must share same SA[47:8] suffix.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR,
    "ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR",
    "Different MEPs on same LIF must have same direction and counter index.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR,
    "ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR",
    "All mips should be above maps. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR,
    "ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR",
    "Adding existing entry to the classifier. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR,
    "ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR",
    "Entry of OEM1 doesn't exist. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'mep_index' is out of range. \n\r "
    "The range is: 0 - 8k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'rmep_index' is out of range. \n\r "
    "The range is: 0 - 8k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_EM_FULL_ERR,
    "ARAD_PP_OAM_EM_FULL_ERR",
    "Exact match is full. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_EM_INTERNAL_ERR,
    "ARAD_PP_OAM_EM_INTERNAL_ERR",
    "Exact match internal error. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_FTL,
    FALSE
  },
  {
    ARAD_PP_OAM_EM_INSERTED_EXISTING_ERR,
    "ARAD_PP_OAM_EM_INSERTED_EXISTING_ERR",
    "Exact match error: Key exists. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_FTL,
    FALSE
  },
  {
    ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter 'md_level' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_FORWARDING_SRTENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_FORWARDING_SRTENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'forwarding_strength' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ma_index' is out of range. \n\r "
    "The range is: 0 - 8k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'mep_type' is out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'non_acc_profile' is out of range. \n\r "
    "The range is: 0 - 4 for ARAD and 1-15 for ARAD+.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_ACC_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'non_acc_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR,
    "ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR",
    "ARAD_PP_OAM opcode to trap code map value is out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR,
    "ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR",
    "ARAD_PP_OAM trap code to mirror profile map value is out of range. \n\r "
    "The range is: 0 - 16.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM tx rate index is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM req interval pointer is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM DIP index is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM EXP is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM TOS is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM TTL is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM DP is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM TC is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR,
    "ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR",
    "ARAD_PP_OAM some of the values of bfd pdu static register are out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR,
    "ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR",
    "ARAD_PP_OAM your discriminator value is out of range. \n\r "
    "The range is: 0 - 32k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_INTERNAL_ERROR,
    "ARAD_PP_OAM_INTERNAL_ERROR",
    "ARAD_PP_OAM internal error \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  /*
   * } Auto generated. Do not edit previous section.
   */


  /*
   * Last element. Do no touch.
   */
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static soc_reg_above_64_val_t interrupt_message[SOC_MAX_NUM_DEVICES] = {{0}}; /* No need for WB - used only inside interrupt read context */
static int last_interrupt_message_num[SOC_MAX_NUM_DEVICES] ; /* No need for WB - used only inside interrupt read context */
static int last_dma_interrupt_message_num[SOC_MAX_NUM_DEVICES] ; /* No need for WB - used only inside interrupt read context. Pointer to the current location in dma_host_memory */
static uint32    *dma_host_memory[SOC_MAX_NUM_DEVICES] ; /* No need for WB. should be freed and alloced at WB.*/
static uint32    *buffer_copied_from_dma_host_memory[SOC_MAX_NUM_DEVICES] ; /* No need for WB. should be freed and alloced at WB. Buffer copied from dma_host_memory.*/
static int num_entries_available_in_local_buffer[SOC_MAX_NUM_DEVICES] ; /* No need for WB. should be freed and alloced at WB. Size of  buffer_copied_from_dma_host_memory.*/
static int num_entries_read_in_local_buffer[SOC_MAX_NUM_DEVICES] ; /* No need for WB. should be freed and alloced at WB. Pointer to current location in buffer_copied_from_dma_host_memory*/

static int arad_pp_num_entries_in_dma_host_memory[SOC_MAX_NUM_DEVICES] ; /* No need for WB. should be freed and alloced at WB. Pointer to current location in buffer_copied_from_dma_host_memory*/

static int (*dma_event_handler_cb[SOC_MAX_NUM_DEVICES])(int) ; /* No need for warmboot. */

static SOC_PPC_OAM_REPORT_MODE rx_report_event_mode[SOC_MAX_NUM_DEVICES]; /* No need for warmboot. */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/******* decleration of internal functions  (see functions for documentation)*/

int _insert_pph(soc_reg_above_64_val_t pkt_header_reg,const PPH_base * pph_fields, int offset);
int _insert_itmh_ing_dest_info_extension(soc_reg_above_64_val_t pkt_header_reg,int fwd_dst_info);
int _insert_itmh(soc_reg_above_64_val_t pkt_header_reg, int fwd_dst_info , int fwd_dp, int fwd_traffic_class, int snoop_cmd,int  in_mirr_flag,int pph_type) ;
int _insert_ptch2(soc_reg_above_64_val_t pkt_header_reg, int next_header_is_itmh, int opaque_value, int pp_ssp);
int _insert_fhei(soc_reg_above_64_val_t pkt_header_reg, int cpu_trap_code_qualifier, int cpu_trap_code, int offset);
uint32 arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(SOC_SAND_IN int);


/****** Struct verify functions ******/
uint32
  SOC_PPD_OAM_MEP_PROFILE_DATA_verify(
    SOC_SAND_IN  SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data
  )
{
  uint32 i;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(profile_data);

  for (i=0; i<SOC_PPD_OAM_OPCODE_MAP_COUNT; i++) {
      SOC_SAND_ERR_IF_ABOVE_MAX(profile_data->opcode_to_trap_code_unicast_map[i], ARAD_PP_NOF_TRAP_CODES-1, ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR, 10, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(profile_data->opcode_to_trap_code_multicast_map[i], ARAD_PP_NOF_TRAP_CODES-1, ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR, 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(profile_data);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_OAM_MEP_PROFILE_DATA_verify()",0,0);
}

uint32
  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->md_level, _ARAD_PP_OAM_MAX_MD_LEVEL, ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->mep_type, SOC_PPD_OAM_MEP_TYPE_COUNT, ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->port, (ARAD_PORT_NOF_PP_PORTS-1), ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->acc_profile, SOC_PPD_OAM_ACC_PROFILES_NUM, ARAD_PP_OAM_ACC_PROFILE_OUT_OF_RANGE_ERR, 70, exit);
  if (classifier_mep_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP && classifier_mep_entry->mep_type !=SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS &&
	  classifier_mep_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP  && classifier_mep_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE_GAL  &&
	  classifier_mep_entry->mep_type !=SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP  && classifier_mep_entry->mep_type !=SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP  &&
	  classifier_mep_entry->mep_type !=SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE  ) {/* For BFD endpoints this value may be used as the my-discr.*/
	  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->ma_index, SOC_PPD_OAM_MAX_NUMBER_OF_MAS(unit), ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR, 80, exit); 
  }
  SOC_SAND_ERR_IF_ABOVE_NOF(classifier_mep_entry->counter, SOC_DPP_DEFS_GET(unit, nof_counters), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 90, exit);
  if (_ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->your_discriminator, _ARAD_PP_OAM_BFD_YOUR_DISC_MAX, ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR, 100, exit);
  }
  else {
      SOC_SAND_ERR_IF_ABOVE_NOF(classifier_mep_entry->lif, OAM_LIF_MAX_VALUE(unit), ARAD_PP_LIF_ID_OUT_OF_RANGE_ERR, 110, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_verify()",0,0);
}

uint32
  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_verify(
    SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES *tx_mpls_att
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(tx_mpls_att);

  SOC_SAND_ERR_IF_ABOVE_MAX(tx_mpls_att->dp, _ARAD_PP_OAM_DP_MAX, ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tx_mpls_att->tc, _ARAD_PP_OAM_TC_MAX, ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(tx_mpls_att);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_verify()",0,0);
}

uint32
  SOC_PPD_BFD_PDU_STATIC_REGISTER_verify(
    SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER *bfd_pdu
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_pdu->bfd_vers, _ARAD_PP_OAM_BFD_VERS_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_pdu->bfd_diag, _ARAD_PP_OAM_BFD_DIAG_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_pdu->bfd_sta, _ARAD_PP_OAM_BFD_STA_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 30, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(bfd_pdu);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_BFD_PDU_STATIC_REGISTER_verify()",0,0);
}

uint32
  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER_verify(
    SOC_SAND_IN  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER *bfd_cc_packet
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bfd_cc_packet);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_cc_packet->bfd_static_reg_fields.bfd_vers, _ARAD_PP_OAM_BFD_VERS_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_cc_packet->bfd_static_reg_fields.bfd_diag, _ARAD_PP_OAM_BFD_DIAG_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_cc_packet->bfd_static_reg_fields.bfd_sta, _ARAD_PP_OAM_BFD_STA_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 30, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(bfd_cc_packet);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_BFD_PDU_STATIC_REGISTER_verify()",0,0);
}

uint32
  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA_verify(
    SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA *tos_ttl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);
  /* comparison is always false due to limited range of data type
  SOC_SAND_ERR_IF_ABOVE_MAX(tos_ttl_data->tos, _ARAD_PP_OAM_TOS_MAX, ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tos_ttl_data->ttl, _ARAD_PP_OAM_TTL_MAX, ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR, 20, exit);
  */
  SOC_SAND_MAGIC_NUM_VERIFY(tos_ttl_data);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA_verify()",0,0);
}

uint32
  SOC_PPD_MPLS_PWE_PROFILE_DATA_verify(
    SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA *mpls_pwe_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mpls_pwe_profile);
  SOC_SAND_ERR_IF_ABOVE_MAX(mpls_pwe_profile->exp, _ARAD_PP_OAM_EXP_MAX, ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR, 10, exit);
  /* comparison is always false due to limited range of data type
  SOC_SAND_ERR_IF_ABOVE_MAX(mpls_pwe_profile->ttl, _ARAD_PP_OAM_TTL_MAX, ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR, 20, exit);*/

  SOC_SAND_MAGIC_NUM_VERIFY(mpls_pwe_profile);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPD_MPLS_PWE_PROFILE_DATA_verify()",0,0);
}

/*********************************************************************
*     Divide OAM name
*********************************************************************/
void _arad_pp_oam_ma_name_divide(
    SOC_SAND_IN   SOC_PPD_OAM_MA_NAME      name, /*uint8 array*/
    SOC_SAND_IN   uint8                    is_ma_short,
    SOC_SAND_OUT  soc_reg_above_64_val_t   umc_name,
    SOC_SAND_OUT  SOC_PPD_OAM_ICC_MAP_DATA *icc_map_data, /*uint64*/
    SOC_SAND_OUT  uint32                   *short_name
    )
{
    COMPILER_64_ZERO(*icc_map_data);

    if (is_ma_short) {
        SOC_REG_ABOVE_64_CLEAR(umc_name);
        *short_name = name[1] + (name[0] << 8);
    }
    else
    {
        SOC_REG_ABOVE_64_CLEAR(umc_name);
        umc_name[0]=name[12]+(name[11]<<8)+(name[10]<<16)+(name[9]<<24);
        umc_name[1]=name[8];
        *short_name = name[7] + (name[6] << 8);
        SOC_PPD_OAM_GROUP_NAME_TO_ICC_MAP_DATA(name, *icc_map_data);
    }
}

STATIC
  uint32
    arad_pp_oam_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    )
{
  uint32
    res = SOC_SAND_OK,
    tcam_db_id,
    access_profile_id_0,
    access_profile_id_1,
    flp_program;
  uint8  prog_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* Set access_profile id depending on TCAM DB ID */
  tcam_db_id = user_data;

  if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vt_use_small_banks",0) == 0 )
  {
        res = arad_sw_db_tcam_db_access_profile_id_get(
          unit,
          tcam_db_id,
          0, /* Use 320b DB */
          &access_profile_id_0
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  else
  {
      access_profile_id_0=0x3F;
  }


  res = arad_sw_db_tcam_db_access_profile_id_get(
      unit,
      tcam_db_id,
      1, /* Use 320b DB */
      &access_profile_id_1
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);


  for (flp_program = 1; flp_program <= ARAD_PP_FLP_KEY_PROGRAM_LAST; flp_program ++) {
      int indx=0, found=0;
	  /* quick check that the program is active and find its index*/
	  for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++indx) {
		  uint8 prog_usage;
		  res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, indx, &prog_usage);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
		  if (flp_program == prog_usage) {
			  found = 1;
			  prog_index = indx;
			  break;
		  }
	  }
      if (found==1) {
          res = arad_pp_flp_lookups_oam(
                unit,
                access_profile_id_0,
                access_profile_id_1,
                prog_index 
          );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_tcam_callback()", 0, 0);
}

/*********************************************************************
*     Init OAM tcam database
*********************************************************************/
uint32 
  arad_pp_oam_tcam_database_create(
       SOC_SAND_IN uint32 unit
  ) 
{
  ARAD_TCAM_ACCESS_INFO tcam_access_info;
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE success;
  uint32 tcam_last_entry_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

  tcam_last_entry_id = 0;
  res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID, &tcam_last_entry_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS |
                                                         ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS | ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS; /*40b action*/
  tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TRAPS;    
  tcam_access_info.callback                            = arad_pp_oam_tcam_callback;
  tcam_access_info.entry_size                          = ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "vt_use_small_banks",0) ) ? ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS : ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS;
  tcam_access_info.is_direct                           = FALSE; 
  tcam_access_info.min_banks                           = _ARAD_PP_OAM_IDENTIFICATION_TCAM_MIN_BANKS; /*0*/
  tcam_access_info.prefix_size                         = _ARAD_PP_OAM_IDENTIFICATION_PREFIX_SIZE; /* 0 because key is exactly 320 bits - bank cannot be shared with another DB */
  tcam_access_info.user_data                           = ARAD_PP_OAM_IDENTIFICATION;
  tcam_access_info.use_small_banks                     = ARAD_TCAM_SMALL_BANKS_FORCE;

  res = arad_tcam_access_create_unsafe(
            unit,
            ARAD_PP_OAM_IDENTIFICATION,
            &tcam_access_info,
            &success
  );

  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  if(success != SOC_SAND_SUCCESS) {
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "   Error in OAM database create. \n\r")));
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "   Parameters: Prefix size %d, entry size %s, bank owner (0-IngPMF, 3-EgrPMF) %d, Action bitmap %d \n\r"), tcam_access_info.prefix_size, 
                 SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(tcam_access_info.entry_size), tcam_access_info.bank_owner, tcam_access_info.action_bitmap_ndx));
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "\n\r")));
        res = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_tcam_database_create()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_oam module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_oam_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_oam;
}

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_oam module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_oam_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_oam;
}

/*********************************************************************/
/*     
*     OAM1 table
*********************************************************************/

/**
 * Allocate SW buffer for writing on OAM-A table. 
 * 
 * @author sinai (01/07/2014)
 * 
 * @param unit 
 * @param buffer 
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_classifier_oam1_allocate_sw_buffer(
    SOC_SAND_IN  uint32                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t        *oama_buffer
  )
{
    uint32  res = 0;
    int mem_size;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    oama_buffer->use_dma = 
#ifdef PLISIM
0;
#else 
         soc_mem_dmaable(unit,IHP_OAMAm, SOC_MEM_BLOCK_ANY(unit, IHP_OAMAm));
#endif
    mem_size =  SOC_MEM_TABLE_BYTES(unit, IHP_OAMAm);

    oama_buffer->is_allocated=0;
    oama_buffer->buffer=NULL;
    if (oama_buffer->use_dma) {
        oama_buffer->buffer = soc_cm_salloc(unit,mem_size, "IHB_OAMA buffer" );
    } else {
        ARAD_ALLOC_ANY_SIZE(oama_buffer->buffer, uint32, mem_size, "IHB_OAMA buffer");
    }

    if (oama_buffer->buffer == NULL) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
    }

    res = soc_mem_read_range(unit, IHP_OAMAm, MEM_BLOCK_ANY, 0, soc_mem_index_max(unit, IHP_OAMAm), oama_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res); 

    oama_buffer->is_allocated = 1;


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/**
 * Set OAM1 table in buffer, according to input. 
 * Function writes values on SW buffer, not on HW.
 * To write on HW use 
 * arad_pp_oam_classifier_oam1_set_hw_unsafe() 
 *  
 *  
 * @param unit 
 * @param oam1_key 
 * @param oam_payload 
 * @param oama_buffer -assumed to be a large enough buffer of 
 *               memory, allocated by
 *               arad_pp_oam_classifier_oam1_allocate_sw_buffer()
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_classifier_oam1_entry_set_on_buffer(
    SOC_SAND_IN  uint32                                     unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY      *oam1_key,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam_payload,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t                           *oama_buffer
  )
{
  uint32  key = 0;
  uint32  field_val = 0;
  uint32* where_to_write; /* Indicates the start of the entry in the buffer*/
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oam1_key);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);
  SOC_SAND_CHECK_NULL_INPUT(oama_buffer);

  _ARAD_PP_OAM_OAM1_KEY_STRUCT_TO_KEY((*oam1_key), key);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OAM1: key: %d\n\r"), key));

  where_to_write = oama_buffer->buffer + key * (soc_mem_entry_words(unit, IHP_OAMAm)  /* subtracting 1 for parity*/);

  if (oam1_key->ingress) {
      field_val = (uint32)oam_payload->snoop_strength;
      soc_IHP_OAMAm_field_set(unit, where_to_write, SNOOP_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " snoop_strength: %d\n\r"), oam_payload->snoop_strength));
      field_val = oam_payload->cpu_trap_code;
      soc_IHP_OAMAm_field_set(unit, where_to_write, CPU_TRAP_CODEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " cpu_trap_code: %d\n\r"), oam_payload->cpu_trap_code));
      field_val = (uint32)oam_payload->forwarding_strength;
      soc_IHP_OAMAm_field_set(unit, where_to_write, FORWARDING_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forwarding_strength: %d\n\r"), oam_payload->forwarding_strength));
      field_val = oam_payload->up_map;
      soc_IHP_OAMAm_field_set(unit, where_to_write, UP_MEPf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " up_map: %d\n\r"), oam_payload->up_map));
      field_val = (uint32)oam_payload->meter_disable;
      soc_IHP_OAMAm_field_set(unit, where_to_write, METER_DISABLEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " meter_disable: %d\n\r"), oam_payload->meter_disable));
  }
  else {
      if (SOC_IS_JERICHO(unit)) {
          field_val = !oam_payload->forward_disable;
          soc_IHP_OAMAm_field_set(unit, where_to_write, FORWARD_ENABLEf, &field_val);

          /*  oam_payload->mirror_enable, mirror_strength  unused at this stage. */
          field_val =oam_payload->mirror_enable; 
          soc_IHP_OAMAm_field_set(unit,where_to_write, MIRROR_ENABLEf, &field_val);

          field_val =  _JER_PP_OAM_MIRROR_STRENGTH;
          soc_IHP_OAMAm_field_set(unit, where_to_write, MIRROR_STRENGTHf, &field_val);

		  field_val =  _JER_PP_OAM_FORWARD_STRENGTH;
		  soc_IHP_OAMAm_field_set(unit, where_to_write, FWD_STRENGTHf, &field_val);


      } else {
          field_val = (uint32)oam_payload->forward_disable;
          soc_IHP_OAMAm_field_set(unit, where_to_write, FORWARD_DISABLEf, &field_val);
      }
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forward_disable: %d\n\r"), oam_payload->forward_disable));
      field_val = (uint32)oam_payload->mirror_profile;
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " mirror_profile: %d\n\r"), oam_payload->mirror_profile));
      soc_IHP_OAMAm_field_set(unit, where_to_write, MIRROR_COMMANDf, &field_val);
  }
  field_val = (uint32)oam_payload->sub_type;
  soc_IHP_OAMAm_field_set(unit, where_to_write, SUB_TYPEf, &field_val);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " sub_type: %d\n\r"), oam_payload->sub_type));
  field_val = (uint32)oam_payload->counter_disable;
  soc_IHP_OAMAm_field_set(unit, where_to_write, COUNTER_DISABLEf, &field_val);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " counter_disable: %d\n\r"), oam_payload->counter_disable));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entry_set_on_buffer()", 0, 0);
}


/**
 * Write a given buffer on the OAMA table. 
 * 
 * @author sinai (01/07/2014)
 * 
 * @param unit 
 * @param oama_buffer - assumed to have been filled by 
 *               arad_pp_oam_classifier_oam1_entry_set_on_buffer()
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_classifier_oam1_set_hw_unsafe(
    SOC_SAND_IN  uint32                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t          *oama_buffer
  )
{
    uint32  res = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_mem_write_range(unit, IHP_OAMAm, MEM_BLOCK_ANY, 0, soc_mem_index_max(unit, IHP_OAMAm), oama_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res); 


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}


/** 
 * Frees a  _oam_oam_a_b_table_buffer_t entry. 
 * 
 * @author sinai (01/07/2014)
 * 
 * @param buffer 
 */
void arad_pp_oam_classifier_oam1_2_buffer_free(
   SOC_SAND_IN int                                           unit, 
   SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t * buffer
   )
{
    if (buffer->is_allocated) {
        if (buffer->use_dma) {
            soc_cm_sfree(unit, buffer->buffer);
        } else {
            ARAD_FREE(buffer->buffer);
        }

    }
}

/*********************************************************************/
/*     
*     OAM2 table
*********************************************************************/


/**
 * Allocate SW buffer for writing on OAM-B table. 
 * 
 * @author sinai (01/07/2014)
 * 
 * @param unit 
 * @param buffer 
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_classifier_oam2_allocate_sw_buffer(
    SOC_SAND_IN  uint32                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t           *oamb_buffer
  )
{
    uint32  res = 0;
    int mem_size;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    oamb_buffer->use_dma =
#ifdef PLISIM
0;
#else 
         soc_mem_dmaable(unit,IHP_OAMBm, SOC_MEM_BLOCK_ANY(unit, IHP_OAMBm));
#endif
    mem_size =  SOC_MEM_TABLE_BYTES(unit, IHP_OAMBm);

    oamb_buffer->buffer=NULL;
    oamb_buffer->is_allocated=0;
    if (oamb_buffer->use_dma) {
        oamb_buffer->buffer = soc_cm_salloc(unit,mem_size, "IHB_OAMB buffer" );
    } else {
        ARAD_ALLOC_ANY_SIZE(oamb_buffer->buffer, uint32, mem_size, "IHB_OAMB buffer");
    }

    if (oamb_buffer->buffer == NULL) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
    }

    res = soc_mem_read_range(unit, IHP_OAMBm, MEM_BLOCK_ANY, 0,soc_mem_index_max(unit, IHP_OAMBm), oamb_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,res); 

    oamb_buffer->is_allocated = 1; 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}


/**
 * Set OAM2 table in buffer, according to input. 
 * Function writes values on SW buffer, not on HW.
 * To write on HW use 
 * arad_pp_oam_classifier_oam2_set_hw_unsafe() 
 *  
 *  
 * @param unit 
 * @param oam1_key 
 * @param oam_payload 
 * @param buffer -assumed to be a large enough buffer of memory, 
 *               allocated by
 *               arad_pp_oam_classifier_oam2_allocate_sw_buffer()
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_classifier_oam2_entry_set_on_buffer(
    SOC_SAND_IN  uint32                                     unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OAM2_ENTRY_KEY      *oam2_key,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam2_payload,
    SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t                      *oamb_buffer
  )
{
  uint32 * where_to_write; 
  uint32  key;
  uint32  field_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oam2_key);
  SOC_SAND_CHECK_NULL_INPUT(oam2_payload);
  SOC_SAND_CHECK_NULL_INPUT(oamb_buffer);

  _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY((*oam2_key), key);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OAM2: key:%d\n\r"), key));
  where_to_write = oamb_buffer->buffer + key * (soc_mem_entry_words(unit, IHP_OAMBm)  );

  if (oam2_key->ingress) {
      field_val = (uint32)oam2_payload->snoop_strength;
      soc_IHP_OAMBm_field_set(unit, where_to_write, SNOOP_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " snoop_strength: %d\n\r"), oam2_payload->snoop_strength));
      field_val = (uint32)oam2_payload->cpu_trap_code;
      soc_IHP_OAMBm_field_set(unit, where_to_write, CPU_TRAP_CODEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " cpu_trap_code: %d\n\r"), oam2_payload->cpu_trap_code));
      field_val = (uint32)oam2_payload->forwarding_strength;
      soc_IHP_OAMBm_field_set(unit, where_to_write, FORWARDING_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forwarding_strength: %d\n\r"), oam2_payload->forwarding_strength));
      field_val = (uint32)oam2_payload->up_map;
      soc_IHP_OAMBm_field_set(unit,where_to_write, UP_MEPf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " up_map: %d\n\r"), oam2_payload->up_map));
      field_val = (uint32)oam2_payload->meter_disable;
      soc_IHP_OAMBm_field_set(unit, where_to_write, METER_DISABLEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " meter_disable: %d\n\r"), oam2_payload->meter_disable));
  }
  else {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          field_val = oam2_payload->forward_disable;
          soc_IHP_OAMBm_field_set(unit, where_to_write, FORWARD_DISABLEf, &field_val);
      } else {
          field_val = !oam2_payload->forward_disable;
          soc_IHP_OAMBm_field_set(unit, where_to_write, FORWARD_ENABLEf, &field_val);

          /*  oam_payload->mirror_enable, mirror_strength  unused at this stage. */
          field_val = 1; 
          soc_IHP_OAMBm_field_set(unit, where_to_write, MIRROR_ENABLEf, &field_val);

		  field_val =  _JER_PP_OAM_MIRROR_STRENGTH;
		  soc_IHP_OAMBm_field_set(unit, where_to_write, MIRROR_STRENGTHf, &field_val); 

          field_val =  _JER_PP_OAM_FORWARD_STRENGTH;
          soc_IHP_OAMBm_field_set(unit, where_to_write, FWD_STRENGTHf, &field_val);

	  }
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forward_disable: %d\n\r"), oam2_payload->forward_disable));
      field_val = (uint32)oam2_payload->mirror_profile;
      soc_IHP_OAMBm_field_set(unit, where_to_write, MIRROR_COMMANDf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " mirror_profile: %d\n\r"), oam2_payload->mirror_profile));
  }
  field_val = (uint32)oam2_payload->sub_type;
  soc_IHP_OAMBm_field_set(unit, where_to_write, SUB_TYPEf, &field_val);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " sub_type: %d\n\r"), oam2_payload->sub_type));
  field_val = (uint32)oam2_payload->counter_disable;
  soc_IHP_OAMBm_field_set(unit,where_to_write, COUNTER_DISABLEf, &field_val);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " counter_disable: %d\n\r"), oam2_payload->counter_disable));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam2_entry_set_on_buffer()", 0, 0);
}


/**
 * Write a given buffer on the OAMB table. 
 * 
 * @author sinai (01/07/2014)
 * 
 * @param unit 
 * @param buffer - assumed to have been filled by 
 *               arad_pp_oam_classifier_oam2_entry_set_on_buffer()
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_classifier_oam2_set_hw_unsafe(
    SOC_SAND_IN  uint32                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t                           *oamb_buffer
  )
{
    uint32  res = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_mem_write_range(unit, IHP_OAMBm, MEM_BLOCK_ANY, 0, soc_mem_index_max(unit, IHP_OAMBm), oamb_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam1_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY      *oam1_key,
    SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam_payload
  )
{
  uint32  res;
  uint32  entry;
  uint32  key;
  uint32  field_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oam1_key);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);

  _ARAD_PP_OAM_OAM1_KEY_STRUCT_TO_KEY((*oam1_key), key);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAMAm(unit, MEM_BLOCK_ANY, key, &entry));
  if (oam1_key->ingress) {
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, SNOOP_STRENGTHf, &field_val);
      oam_payload->snoop_strength = (uint8)field_val;
      soc_IHP_OAMAm_field_get(unit, &entry, CPU_TRAP_CODEf, &oam_payload->cpu_trap_code);
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, FORWARDING_STRENGTHf, &field_val);
      oam_payload->forwarding_strength = (uint8)field_val;
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, UP_MEPf, &field_val);
      oam_payload->up_map = (uint8)field_val;
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, METER_DISABLEf, &field_val);
      oam_payload->meter_disable = (uint8)field_val;
  }
  else {
      field_val = 0;
      if (SOC_IS_JERICHO(unit)) {
          soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_STRENGTHf, &field_val);
          oam_payload->mirror_strength = field_val;
          field_val=0;

          soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_ENABLEf, &field_val);
          oam_payload->mirror_enable = field_val;
          field_val=0;

          soc_IHP_OAMAm_field_get(unit, &entry, FORWARD_ENABLEf, &field_val);
          field_val = !field_val;
      } else {
          soc_IHP_OAMAm_field_get(unit, &entry, FORWARD_DISABLEf, &field_val);
      }
      oam_payload->forward_disable = (uint8)field_val;
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_COMMANDf, &field_val);
      oam_payload->mirror_profile = (uint8)field_val;
  }
  field_val = 0;
  soc_IHP_OAMAm_field_get(unit, &entry, SUB_TYPEf, &field_val);
  oam_payload->sub_type = (uint8)field_val;
  field_val = 0;
  soc_IHP_OAMAm_field_get(unit, &entry, COUNTER_DISABLEf, &field_val);
  oam_payload->counter_disable = (uint8)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entry_get_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam2_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OAM2_ENTRY_KEY      *oam2_key,
    SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam2_payload
  )
{
    uint32  res;
    uint32  entry;
    uint32  key;
    uint32  field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_GET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(oam2_key);
    SOC_SAND_CHECK_NULL_INPUT(oam2_payload);

    _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY((*oam2_key), key);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAMBm(unit, MEM_BLOCK_ANY, key, &entry));
    if (oam2_key->ingress) {
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, SNOOP_STRENGTHf, &field_val);
        oam2_payload->snoop_strength = (uint8)field_val;
        soc_IHP_OAMBm_field_get(unit, &entry, CPU_TRAP_CODEf, &oam2_payload->cpu_trap_code);
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, FORWARDING_STRENGTHf, &field_val);
        oam2_payload->forwarding_strength = (uint8)field_val;
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, UP_MEPf, &field_val);
        oam2_payload->up_map = (uint8)field_val;
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, METER_DISABLEf, &field_val);
        oam2_payload->meter_disable = (uint8)field_val;
    }
    else {
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, FORWARD_DISABLEf, &field_val);
        oam2_payload->forward_disable = (uint8)field_val;
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, MIRROR_COMMANDf, &field_val);
        oam2_payload->mirror_profile = (uint8)field_val;
    }
    field_val = 0;
    soc_IHP_OAMBm_field_get(unit, &entry, SUB_TYPEf, &field_val);
    oam2_payload->sub_type = (uint8)field_val;
    field_val = 0;
    soc_IHP_OAMBm_field_get(unit, &entry, COUNTER_DISABLEf, &field_val);
    oam2_payload->counter_disable = (uint8)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam2_entry_get_unsafe()", 0, 0);
}

    

/**
 * Internal function. Sets the DMA to be used by the event FIFO. 
 * (Instead of the Interrupt Message register). 
 * 
 * @author sinai (28/08/2013)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_dma_set_unsafe(
	 SOC_SAND_IN  int                                 unit
	 )
{
	uint32 res;
	uint32 reg, dma_threshold;
    uint32 host_mem_num_entries , timeout;
    uint32 host_mem_size_in_bytes;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    dma_threshold = soc_property_get(unit, spn_OAMP_FIFO_DMA_THRESHOLD, 0x20);
    host_mem_size_in_bytes = soc_property_get(unit, spn_OAMP_FIFO_DMA_BUFFER_SIZE, 0);
    timeout = soc_property_get(unit, spn_OAMP_FIFO_DMA_TIMEOUT, 0);

    /*some sanity checks + calculating number of entries.*/
    /* add 1 to aviod division by zero*/
    host_mem_num_entries = (host_mem_size_in_bytes+1) / _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA;

    dma_threshold = (dma_threshold > LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: dma_threshold;
    host_mem_num_entries =  (host_mem_num_entries < dma_threshold) ? dma_threshold  :host_mem_num_entries; 

    ROUND_UP_TO_NEAREST_POWER_OF_2(host_mem_num_entries);
    host_mem_num_entries = (host_mem_num_entries <SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY )? SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY : host_mem_num_entries;
    host_mem_num_entries = ( host_mem_num_entries >LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: host_mem_num_entries;

    arad_pp_num_entries_in_dma_host_memory[unit] = host_mem_num_entries;

    host_mem_size_in_bytes = host_mem_num_entries * _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA;
    /* Connect DMA engine #3 to OAMP. */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_ECI_GENERAL_CONFIGURATION_2r(unit, &reg));
    soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_3_SELf, 1);    
    soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_7_SELf, 0);    
    soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_11_SELf, 0); 
    soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, CMICM_BISR_BYPASS_ENABLEf, 0); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_ECI_GENERAL_CONFIGURATION_2r(unit, reg));


	dma_host_memory[unit] = soc_cm_salloc(unit, host_mem_size_in_bytes, "oam DMA"); 
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Allocating %d memory \n"),host_mem_size_in_bytes ));
    if (!dma_host_memory[unit]) {
		SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Memory allocation failed!")));
    }

	buffer_copied_from_dma_host_memory[unit] = soc_cm_salloc(unit, dma_threshold *_ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA , "oam DMA local "); 
    if (!buffer_copied_from_dma_host_memory[unit]) {
		SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Memory allocation failed!")));
    }


	sal_memset(dma_host_memory[unit], 0,  host_mem_size_in_bytes);
    

	/* stop the DMA in case it is running and then then start again*/
    res = _soc_mem_sbus_fifo_dma_stop(unit, ARAD_OAM_DMA_CHANNEL_USED);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    res = _soc_mem_sbus_fifo_dma_start_memreg(unit, ARAD_OAM_DMA_CHANNEL_USED,
                                             FALSE /*is_mem*/, 0, OAMP_INTERRUPT_MESSAGEr, MEM_BLOCK_ALL,
                                             0, host_mem_num_entries, dma_host_memory[unit]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);



    /* Setting registers not properly set by dma_start_memreg()*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_CMIC_CMC0_FIFO_CH3_RD_DMA_HOSTMEM_THRESHOLDr(unit, &reg));
    
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH3_RD_DMA_HOSTMEM_THRESHOLDr, &reg, THRESHOLDf, dma_threshold);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_CMIC_CMC0_FIFO_CH3_RD_DMA_HOSTMEM_THRESHOLDr(unit, reg)); 

    reg=0;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_CMIC_CMC0_FIFO_CH3_RD_DMA_CFGr(unit, &reg));

    MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(timeout);
    timeout &= 0x3fff;

    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH3_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, timeout);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH3_RD_DMA_CFGr, &reg, ENDIANESSf, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_CMIC_CMC0_FIFO_CH3_RD_DMA_CFGr(unit, reg)); 


    /* Unmask fifo dma channle 3 interrupts*/
    soc_cmicm_intr0_enable(unit, IRQ_CMCx_FIFO_CH_DMA(ARAD_OAM_DMA_CHANNEL_USED));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/**
 * Self explanatory.
 * 
 * @author sinai (11/11/2013)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_dma_unset_unsafe(
	 SOC_SAND_IN  int                                 unit
	 )
{
	uint32 res, reg;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    if (dma_host_memory[unit] != NULL) {
        soc_cm_sfree(unit, dma_host_memory[unit]);
    }

    if (buffer_copied_from_dma_host_memory[unit] != NULL) {
        soc_cm_sfree(unit, buffer_copied_from_dma_host_memory[unit]);
    }

    res = _soc_mem_sbus_fifo_dma_stop(unit, ARAD_OAM_DMA_CHANNEL_USED);
    SOC_SAND_CHECK_FUNC_RESULT(res, 234 ,exit);

    /* Disonnect DMA engine #3 from OAMP. */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_ECI_GENERAL_CONFIGURATION_2r(unit, &reg));
    soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_3_SELf, 0);    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_ECI_GENERAL_CONFIGURATION_2r(unit, reg));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/**
 * First octet in TLV is type (2 for port, 4 for interface), 
 * next two octets are the length (length represents the length 
 * of the value field, should always be 1 in Arad+. 
 */
#define _ARAD_PP_OAMP_SET_FIRST_THREE_OCTETS_IN_TLV(tlv_value, type, length) \
	tlv_value = (( type & 0xff)<<16) | (length   & 0xffff)

/*********************************************************************
*     Init OAMP
*     Parameters:
*       SOC_SAND_IN  int                                 unit,
*       SOC_SAND_IN  uint8                                    is_bfd -
*               indication whether we init oam or bfd
*       SOC_SAND_IN  uint32                                  cpu_system_port -
*               system port of the cpu
*       SOC_SAND_IN  uint8                                    is_second_init -
*               was this function called already from the other (oam/bfd) module
*********************************************************************/
uint32
  arad_pp_oam_oamp_init_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint8                                  is_bfd,
      SOC_SAND_IN  uint32                                 cpu_system_port,
      SOC_SAND_IN  uint8                                  is_second_init
  )
{
    uint32 res;
    uint32 reg;
    uint64 reg64;
    soc_reg_above_64_val_t  reg_above_64;
    uint8 interrupt_message_event[SOC_PPD_OAM_EVENT_COUNT] = {0};
    SOC_PPD_BFD_PDU_STATIC_REGISTER              bfd_pdu;
    ARAD_DEST_INFO            destination;
    uint32 destination_for_itmh;
    uint32 i, bfd_gach_cc, bfd_gach_cv, y1731_gach, bfd_pwe_cw, oam_pwe_cw, oam_pwe_channel;
    uint32 num_clocks_per_mep_scan;
    uint32 bfd_cc_channel, bfd_cv_channel, y1731_channel, bfd_pwe_channel;
    uint32 use_dma, event_ndx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_INIT_UNSAFE);

    /* init OAMP registers */
    /* enable all types of events*/
    for (event_ndx=0; event_ndx<SOC_PPD_OAM_EVENT_COUNT; event_ndx++) {
        interrupt_message_event[event_ndx]=1;
    }
    res = arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(unit, interrupt_message_event); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_fifo_dma;

	/*  Interrupt messages are either managed through the DMA or "by hand" (directly from the register). */
    if  (use_dma && SOC_IS_ARADPLUS(unit)) {
        res = arad_pp_oam_dma_set_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    /* Initialize global variables.*/
    for (i=0 ; i< SOC_MAX_NUM_DEVICES ; ++i) {
        last_interrupt_message_num[i] = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
         
    reg = 0;
    soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, TRANSMIT_ENABLEf, 1);
    soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, SCANNER_ENABLEf, 1);
    soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, RX_2_CPUf, 0);
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        /* No such field in Arad+ */
        soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, UPDATE_STATE_EVEN_IF_EVENT_FIFO_FULLf, 1);
	} else if (SOC_IS_ARADPLUS(unit)) {
		soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, DMR_LMR_RESPONSE_ENABLEf, 1);
        if (SOC_IS_JERICHO(unit)) {
            soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, REPORT_MODEf, SOC_PPC_OAM_REPORT_MODE_NORMAL); /* Normal report mode.*/
            rx_report_event_mode[unit] = SOC_PPC_OAM_REPORT_MODE_NORMAL;
        }
	}

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_MODE_REGISTERr(unit, reg)); 

    reg = 0;
    soc_reg_field_set(unit, OAMP_DOWN_PTCHr, &reg, DOWN_PTCH_PP_SSPf, 0);
    soc_reg_field_set(unit, OAMP_DOWN_PTCHr, &reg, DOWN_PTCH_OPAQUE_PT_ATTRf, _ARAD_PP_OAM_PTCH_OPAQUE_VALUE);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_DOWN_PTCHr(unit, reg)); 

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  51,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_ENABLEf,  1));

    if (is_bfd) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  52,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_MY_DISCRIMINATOR_RANGE_STARTr, REG_PORT_ANY, 0, BFD_MY_DISCRIMINATOR_RANGE_STARTf,  0));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_IPV4_UDP_SPORTr, REG_PORT_ANY, 0, BFD_IPV4_UDP_SPORTf,  0));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_MPLS_UDP_SPORTr, REG_PORT_ANY, 0, BFD_MPLS_UDP_SPORTf,  0));

        /* Configure GAL and GACH config registers for bfd cc and then bfd cv*/
        /*{0001b, Version= 0x0(4b), Reserved= 0x00 (8b), Channel-Type(16b)} */
        _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(_ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM, bfd_cc_channel);
        bfd_gach_cc = 0x10000000 |  bfd_cc_channel;
        _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(_ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM, bfd_cv_channel);
        bfd_gach_cv = 0x10000000 |  bfd_cv_channel;
        _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(_ARAD_PP_OAM_MPLS_TP_CHANNEL_PW_ACH_ENUM, bfd_pwe_channel);
        bfd_pwe_cw = 0x10000000 |  bfd_pwe_channel;
        _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(_ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM, oam_pwe_channel);
        oam_pwe_cw = 0x10000000 |  oam_pwe_channel;

        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_CC_MPLSTP_GALr, REG_PORT_ANY, 0, BFD_CC_MPLSTP_GALf,  0x0000D140));/*MPLS: Label=13;Exp=0,BOS=1,TTL=64*/
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_CC_MPLSTP_GACHr, REG_PORT_ANY, 0, BFD_CC_MPLSTP_GACHf,  bfd_gach_cc));

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_CV_MPLSTP_GALr, REG_PORT_ANY, 0, BFD_CV_MPLSTP_GALf,  0x0000D140));/*MPLS: Label=13;Exp=0,BOS=1,TTL=64*/
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_CV_MPLSTP_GACHr, REG_PORT_ANY, 0, BFD_CV_MPLSTP_GACHf,  bfd_gach_cv));

        }

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_Y_1731O_PWE_GACHr, REG_PORT_ANY, 0, Y_1731O_PWE_GACHf,  oam_pwe_cw));


        /* BFD_PWE_CW is used for both pwe and mplstp, so only one of them can be set */
        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BFD_ENCAPSULATION_MODE, 0)) {
            /* CW is used for BFDCCoGACHoGALoLSP WA as GACH label */
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_CW_0r, REG_PORT_ANY, 0, BFD_PWE_CW_0f,  bfd_gach_cc));
        }
        else {
            /* Deafult CW for PWE */
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_CW_0r, REG_PORT_ANY, 0, BFD_PWE_CW_0f,  bfd_pwe_cw));
        }

        /* Setting PR2 FWDtc to 1:1 mapping of priority field */
        reg = 0;
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_0f, 0);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_1f, 1);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_2f, 2);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_3f, 3);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_4f, 4);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_5f, 5);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_6f, 6);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, &reg, CCM_CLASS_7f, 7);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_PR_2_FW_DTCr(unit, reg)); 

        SOC_PPD_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
        bfd_pdu.bfd_vers = SOC_PPD_BFD_PDU_VERSION;
        bfd_pdu.bfd_sta = 3;
        bfd_pdu.bfd_flags = 1; /* C = 1 */
        bfd_pdu.bfd_length = SOC_PPD_BFD_PDU_LENGTH;
        res = arad_pp_oam_bfd_pdu_static_register_set_unsafe(unit, &bfd_pdu);
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

        if (SOC_IS_ARADPLUS(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_ROUTER_ALERTr, REG_PORT_ANY, 0, BFD_PWE_ROUTER_ALERTf,  1)); /* Router Alert is label 1 */
        }

        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            /* The destination IP address MUST be randomly chosen from the 127/8 range for IPv4*/
            /* In Arad+ and above it is set by the OAMP, in Arad a/b this must be configured through a static register.*/
            uint32 dip = (soc_sand_os_rand() % 0x800000) + (0x7f <<24);
            soc_reg_field_set(unit, OAMP_BFD_MPLS_DST_IPr, &reg, BFD_MPLS_DST_IPf,dip);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_BFD_MPLS_DST_IPr(unit, reg)); 
        }
    } else {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)) {
            /* When the above is set the OAMP may not preform MDL validity checks. This is somewhat redundant anyways.*/
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, READ_OAMP_ETH_OAM_CONTROLr(unit, &reg));
            soc_reg_field_set(unit, OAMP_ETH_OAM_CONTROLr, &reg, ETH_OAM_CHECK_MDLf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_ETH_OAM_CONTROLr(unit, reg));

            if (SOC_IS_JERICHO(unit)) {
                SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("soc_property oam_over_mpls_ignore_mdl unavailable for Jericho.")));
            }
        }
    }

    if (is_second_init) {
        goto exit;
    }


    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    /* clock register set - Number of system clock cycles in 3.33 ms */
    num_clocks_per_mep_scan = ARAD_PP_OAM_NUM_CLOCKS_IN_MEP_SCAN;
    if (SOC_IS_JERICHO(unit)) {
        uint64 reg64 ;
        COMPILER_64_ZERO(reg64);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, READ_OAMP_TIMER_CONFIGr(unit, &reg64)); 
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,NUM_CLOCKS_SCANf, num_clocks_per_mep_scan);
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,NUM_CLOCKS_SECf, arad_chip_kilo_ticks_per_sec_get(unit)*1000);
        /* other fields should remain with default values*/
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, WRITE_OAMP_TIMER_CONFIGr(unit, reg64)); 
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_TIMER_CONFIGr, REG_PORT_ANY, 0, NUM_CLOCKS_SCANf,  num_clocks_per_mep_scan));
    }

    arad_ARAD_DEST_INFO_clear(&destination);
    destination.id = cpu_system_port;
    destination.type = ARAD_DEST_TYPE_SYS_PHY_PORT;

    res = arad_hpu_itmh_fwd_dest_info_build(unit, destination, FALSE, FALSE, &destination_for_itmh);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_CPUPORTr(unit, reg_above_64));
    for (i=0; i < _ARAD_PP_OAM_NUMBER_OF_OAMP_ERROR_TYPES; i++) {
        SHR_BITCOPY_RANGE(reg_above_64, _ARAD_PP_OAM_ITMH_DEST_INFO_WIDTH*i, &destination_for_itmh, 0, _ARAD_PP_OAM_ITMH_DEST_INFO_WIDTH); 
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_OAMP_CPUPORTr(unit, reg_above_64)); 
     
    reg = 0;
    soc_reg_field_set(unit, OAMP_DOWN_PTCHr, &reg, DOWN_PTCH_PP_SSPf, 0);
    soc_reg_field_set(unit, OAMP_DOWN_PTCHr, &reg, DOWN_PTCH_OPAQUE_PT_ATTRf, _ARAD_PP_OAM_PTCH_OPAQUE_VALUE);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_DOWN_PTCHr(unit, reg)); 

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  51,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_ENABLEf,  1));

    soc_reg_field_set(unit, OAMP_Y_1731O_MPLSTP_GALr, &reg, Y_1731O_MPLSTP_GALf, 0x0000D140); /*MPLS: Label=13;Exp=0,BOS=1,TTL=64*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 140, exit, WRITE_OAMP_Y_1731O_MPLSTP_GALr(unit, reg));
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(_ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM, y1731_channel);
    y1731_gach = 0x10000000 |  y1731_channel; /*GACH: 0x1000 - Reserved+version; + Channel type */
    soc_reg_field_set(unit, OAMP_Y_1731O_MPLSTP_GACHr, &reg, Y_1731O_MPLSTP_GACHf, y1731_gach); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, WRITE_OAMP_Y_1731O_MPLSTP_GACHr(unit, reg));

    if (SOC_IS_ARADPLUS(unit)) {
		uint32 tlv, reg_index, field_name;
        res = arad_pp_oamp_pe_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

		/* Set static configuration for port/Interface TLV*/
		_ARAD_PP_OAMP_SET_FIRST_THREE_OCTETS_IN_TLV(tlv,2,1);
		soc_reg_field_set(unit,OAMP_PORT_STAT_TLVr, &reg, PORT_STAT_TLVf, tlv);
		SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_PORT_STAT_TLVr(unit, reg)); 

		_ARAD_PP_OAMP_SET_FIRST_THREE_OCTETS_IN_TLV(tlv,4,1);
		soc_reg_field_set(unit,OAMP_INTERFACE_STAT_TLVr, &reg, INTERFACE_STAT_TLVf, tlv);
		SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_INTERFACE_STAT_TLVr(unit, reg)); 

        /* Init RX Trap table entries 1-3 to 0.
           Entry 0 is reserved for traps from local classifier. */
        /* Thus we will be able to know if we still have free resources */
        reg = 0;

        for (field_name=TRAP_N_1731O_MPLSTPf; field_name<TRAP_N_1731O_MPLSTPf+8; field_name++) {
            for (reg_index=1; reg_index<=3; reg_index++) {
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, 
                                                 ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, reg));  
            }
        }
    }

   
    COMPILER_64_ZERO(reg64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit,READ_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, &reg64));
    soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &reg64, STAMP_DSP_EXTf, 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit,WRITE_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, reg64));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_global_init_unsafe()", 0, 0);
}

/**
 * According to Y.1731 DMM/DMRs use version 1. 
 * OAMP uses version 0. 
 * Support Version 1,0 for these packets.
 */
#define _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(opcode) ((opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM || \
                                                                                                  opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM || opcode==SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM)?\
                                                                                                   0 :0xffffffff)

STATIC
  void
    arad_pp_oam_tcam_entry_build(
      SOC_SAND_IN     int                                  unit,
      SOC_SAND_IN  SOC_PPD_OAM_TCAM_ENTRY_KEY *key,
      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  )
{
	uint32
	   mask,
	   val;

	uint32 gal_begin_bit;
	uint32 cfm_begin_bit = 0;
	uint32 ip_begin_bit = 0;
	uint32 tcam_entry_key_header_start;
	uint32 ether_type_begin_bit = 0;

	ARAD_TCAM_ENTRY_clear(entry);

	entry->valid = TRUE;
	entry->is_for_update = 0;
	entry->is_inserted_top = 1;

	/* Inner Ethernet */
	if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification",0) == 1) &&
        key->fwd_code == _ARAD_PP_OAM_FWD_CODE_ETHERNET && key->ttc >= SOC_PPC_PKT_TERM_TYPE_MPLS_ETH && key->ttc <= SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH) {
		uint32 ether_type_begin_bit;
        /** Note that this may interfere with other types of TCAM
         *  entries. */

		/* Tunnel-Termination-Code: TTC=[ TTC-MPLSoE|TTC-MPLSx2oE|TTC-MPLSx3oE] without CW but adding don't cares because not enough place in TCAM*/
		mask = 0xFFFFFFFE;  /*last bit is masked to enable CW with/without */
		soc_sand_bitstream_set_any_field(&key->ttc, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 4, 4, entry->value);
		soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 4, 4, entry->mask);

		/* Forwarding Code */
		mask = 0xFFFFFFFF; /*fixed value */
		soc_sand_bitstream_set_any_field(&key->fwd_code, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 8, 4, entry->value);
		soc_sand_bitstream_set_any_field(&mask,          sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 8, 4, entry->mask);

		mask = 0;
		val = 0; /*In-LIF-Profile = X */
		soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 11, 3, entry->value);
		soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 11, 3, entry->mask);

		mask = 0;
		val = 0; /*VTT-OAM-LIF-Valid= X (dont care)*/
		soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 12, 1, entry->value);
		soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 12, 1, entry->mask);

		tcam_entry_key_header_start = 12;
		ether_type_begin_bit = tcam_entry_key_header_start + (12 * 8) + (key->channel_type * 4 * 8);
		cfm_begin_bit =  ether_type_begin_bit + 2*8 /*comes after the protocol id */;


		mask = 0xFFFFFFFF; /* Ether-type 0x8902 */
		val = 0x8902;
		soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ether_type_begin_bit - 16, 16, entry->value);
		soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ether_type_begin_bit - 16, 16, entry->mask);

		mask = 0xFFFFFFFF; /* MDL(3b) */
		soc_sand_bitstream_set_any_field(&key->mdl, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 3, 3, entry->value);
		soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 3, 3, entry->mask);
	} else {
		if (key->ttc == SOC_PPC_NOF_PKT_TERM_TYPES) { /* MIM */
			mask = 0;
			val = 0; /*TTC = X */
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 4, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 4, 4, entry->mask);
		} else {
			/* Tunnel-Termination-Code: TTC=[ TTC-MPLSoE|TTC-MPLSx2oE|TTC-MPLSx3oE] without CW but adding don't cares because not enough place in TCAM*/
			mask = 0xFFFFFFFE;  /*last bit is masked to enable CW with/without */
			soc_sand_bitstream_set_any_field(&key->ttc, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 4, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 4, 4, entry->mask);
		}

		mask = 0xFFFFFFFF; /*fixed value */
		soc_sand_bitstream_set_any_field(&key->fwd_code, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 8, 4, entry->value);
		soc_sand_bitstream_set_any_field(&mask,          sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 8, 4, entry->mask);

		mask = 0;
		val = 0; /*In-LIF-Profile = X */
		soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 11, 3, entry->value);
		soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 11, 3, entry->mask);

		mask = 0;
		val = 0; /*VTT-OAM-LIF-Valid= X (dont care)*/
		soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 12, 1, entry->value);
		soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - 12, 1, entry->mask);

		tcam_entry_key_header_start = 12;

		if (key->fwd_code == _ARAD_PP_OAM_FWD_CODE_IPV4_UC) {
			ip_begin_bit = tcam_entry_key_header_start; /* IP label - no offset */
		} else {
			switch (key->ttc) {
			case SOC_PPC_NOF_PKT_TERM_TYPES:
				ether_type_begin_bit = tcam_entry_key_header_start + 12 * 8;
				cfm_begin_bit = tcam_entry_key_header_start + 14 * 8;
				break;
			case SOC_PPD_PKT_TERM_TYPE_MPLS_ETH:
			case SOC_PPD_PKT_TERM_TYPE_CW_MPLS_ETH:
				gal_begin_bit = tcam_entry_key_header_start + 4 * 8; /*4Byte MPLS label*8bits in byte + 12bits key length till now*/
				break;
			case SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH:
			case SOC_PPD_PKT_TERM_TYPE_CW_MPLS2_ETH:
				gal_begin_bit = tcam_entry_key_header_start + 2 * 4 * 8;
				break;
			default:
				gal_begin_bit = tcam_entry_key_header_start;
			}
		}

		if (ip_begin_bit > 0) {
            int ip_offset=0; /* in bits*/
            /* Entry for BFD over IP over MPLS.*/
			mask = 0xFFFFFFFF;
			val = 4; /* IPv4.version(4) = 4 */
            ip_offset +=4; /*IP version size*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 4, entry->mask);

			mask =   0xFFFFFFFF;
			val = 5; /* IPv4.IHL(4) = 5 */
            ip_offset +=4; /*IHL size*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 4, entry->mask);

			mask = 0xFFFFFFFF;
			val = 1; /* IPv4.TTL(8) = 1 */
            ip_offset +=64; /*TTL size: 1 octet + 7 octets offset from IHL*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 8, entry->mask);

			mask = 0xFFFFFFFF;
			val = 17; /* IPv4.protocol(8) = UPD(17) */
            ip_offset +=8; /*Protocol: 1 octet*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 8, entry->mask);

            mask = 0xffffffff;
            val = 127; /* IPv4.DIP must be in the 127/8 range (127.0.0.0 - 127.255.255.255) */
            ip_offset += 56;  /*6 octets offset from protocol + 1 octet in the DIP.*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 8, entry->mask);

			mask = 0xFFFFFFFF;
			val = 3; /* UDP.source-prot-msbs(2) = 3: The source  port MUST be in the range 49152 through 65535 */
            ip_offset += 24 +2;  /*3 octets of the IP DIP + 2 bits in the UDP source port.*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 2, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 2, entry->mask);

			mask = 0xFFFFFFFF;
			val = 0x0ec8; /* UDP.dest-port(16) = 0x0ec8 */
            ip_offset += 30; /* 14 remaining bits from the source port + 2 octets for the UPD dest port.*/
			soc_sand_bitstream_set_any_field(&val,  sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ip_begin_bit - ip_offset, 16, entry->mask);



		} else if (cfm_begin_bit > 0) { /* MIM */
			mask = 0xFFFFFFFF; /* Ether-type 0x8902 */
			val = 0x8902;
			soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ether_type_begin_bit - 16, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - ether_type_begin_bit - 16, 16, entry->mask);

			mask = 0xFFFFFFFF; /* MDL(3b) */
			soc_sand_bitstream_set_any_field(&key->mdl, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 3, 3, entry->value);
			soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 3, 3, entry->mask);

			mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(key->opcode);
			val = 0; /* Version(5b) = 0 */
			soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 8, 5, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 8, 5, entry->mask);

			if (key->opcode == (SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) {
				mask = 0; /* OpCode(8b) - dont cares */
				val = 0;
				soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 16, 8, entry->value);
				soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 16, 8, entry->mask);
			} else {
				mask = 0xFFFFFFFF; /* OpCode(8b) */
				soc_sand_bitstream_set_any_field(&key->opcode, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 16, 8, entry->value);
				soc_sand_bitstream_set_any_field(&mask,        sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - cfm_begin_bit - 16, 8, entry->mask);
			}
		} else { /*MPLS*/
			mask = 0xFFFFFFFF; /*4B MPLS label with labe=GAL==13(20b)*/
			soc_sand_bitstream_set_any_field(&key->mpls_label, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 20, 20, entry->value);
			soc_sand_bitstream_set_any_field(&mask,            sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 20, 20, entry->mask);

			mask = 0; /*4B MPLS label with all the rest - dont care */
			val = 0;
			soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 32, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 32, 8, entry->mask);

			mask = 0xFFFFFFFF;
			val = 0x1000; /*G-Ach== i.0001b ii.Version=0(4b) iii.Reserved=0(8b) */
			soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 48, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 48, 16, entry->mask);

			/* Channel-type= [16 configurable values](16b) */
			soc_sand_bitstream_set_any_field(&key->channel_type, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 64, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask,              sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 64, 16, entry->mask);

			if (key->mdl == _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL) {
				mask = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)? 0 :0xFFFFFFFF; /* MDL(3b) */
                        /* May be masked according to soc property*/
				soc_sand_bitstream_set_any_field(&key->mdl, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 67, 3, entry->value);
				soc_sand_bitstream_set_any_field(&mask,     sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 67, 3, entry->mask);

				mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(key->opcode);
				val = 0; /* Version(5b) = 0 */
				soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 72, 5, entry->value);
				soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 72, 5, entry->mask);

				if (key->opcode == (SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) {
					mask = 0; /* OpCode(8b) - dont cares */
					val = 0;
					soc_sand_bitstream_set_any_field(&val, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 80, 8, entry->value);
					soc_sand_bitstream_set_any_field(&mask, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 80, 8, entry->mask);
				} else {
					mask = 0xFFFFFFFF; /* OpCode(8b) */
					soc_sand_bitstream_set_any_field(&key->opcode, sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 80, 8, entry->value);
					soc_sand_bitstream_set_any_field(&mask,        sizeof(uint32) * 8 * ARAD_TCAM_ENTRY_MAX_LEN - gal_begin_bit - 80, 8, entry->mask);
				}
			}
		}
	}

}

/*
`define IHP_FLP_TCAM_TRAP_TYPE_BITS                        1:0     // 2b
`define IHP_FLP_TCAM_TRAP_PACKET_IS_OAM_BIT                2       // 1b
`define IHP_FLP_TCAM_TRAP_OAM_IS_BFD_BIT                   3       // 1b
`define IHP_FLP_TCAM_TRAP_OAM_OPCODE_BITS                  7:4     // 4b
`define IHP_FLP_TCAM_TRAP_OAM_MD_LEVEL_BITS                10:8    // 3b
`define IHP_FLP_TCAM_TRAP_OAM_YOUR_DISCR_BIT               11      // 1b
`define IHP_FLP_TCAM_TRAP_OAM_LIF_BITS                     27:12   // 16b
`define IHP_FLP_TCAM_TRAP_OAM_LIF_VALID_BIT                28      // 1b
`define IHP_FLP_TCAM_TRAP_OAM_MY_CFM_MAC_BIT               29      // 1b
`define IHP_FLP_TCAM_TRAP_OAM_STAMP_OFFSET_BITS            36:30   // 7b
`define IHP_FLP_TCAM_TRAP_OAM_OFFSET_BITS                  43:37   // 7b
`define IHP_FLP_TCAM_TRAP_OAM_PCP_BITS                     46:44   // 3b
*/
STATIC
uint32
  arad_pp_oam_tcam_entry_to_action(
      SOC_SAND_IN  SOC_PPD_OAM_TCAM_ENTRY_ACTION  *entry,
      SOC_SAND_OUT ARAD_TCAM_ACTION               *action
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);


  ARAD_TCAM_ACTION_clear(action);
  SHR_BITCOPY_RANGE(action->value, 0, &entry->type, 0, 2); /*Type(2)*/
  SHR_BITCOPY_RANGE(action->value, 2, &entry->is_oam, 0, 1); /*Packet-is-OAM(1)*/
  SHR_BITCOPY_RANGE(action->value, 3, &entry->is_bfd, 0, 1); /*OAM-is-BFD(1)*/
  SHR_BITCOPY_RANGE(action->value, 4, &entry->opcode, 0, 4); /*OAM-Opcode(4)*/
  SHR_BITCOPY_RANGE(action->value, 8, &entry->mdl, 0, 3); /*OAM-MD-Level(3)*/
  SHR_BITCOPY_RANGE(action->value, 11, &entry->your_discr, 0, 1); /*OAM-Your-Discr(1)*/
  SHR_BITCOPY_RANGE(action->value, 12, &entry->oam_lif_tcam_result, 0, 16); /*OAM-LIF-TCAM-Result(16)*/
  SHR_BITCOPY_RANGE(action->value, 28, &entry->oam_lif_tcam_result_valid, 0, 1); /*OAM-LIF-TCAM-Result-Valid(1)*/
  SHR_BITCOPY_RANGE(action->value, 29, &entry->my_cfm_mac, 0, 1); /*My-CFM-MAC(1)*/
  SHR_BITCOPY_RANGE(action->value, 30, &entry->oam_stamp_offset, 0, 7); /*OAM-Stamp-Offset(7)*/
  SHR_BITCOPY_RANGE(action->value, 37, &entry->oam_offset, 0, 7); /*OAM-Offset(7)*/
  SHR_BITCOPY_RANGE(action->value, 44, &entry->oam_pcp, 0, 3); /*OAM-PCP(3)*/
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_entry_to_action()",0,0);
}


/*********************************************************************
*     Add default static entries to OAM1-2 tables
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_profile_reset(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile,
    SOC_SAND_IN  uint8                                    is_bfd
  )
{
  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY      oam1_key;
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD   oam_payload;
  uint32 is_ingress, mep_type, mip_type, is_my_cfm_mac, opcode, action;
  uint8 oam_mirror_profile[4];  /* Stored as 8 uint8's on uit32's*/
  uint32 oam_trap_code;
  uint32 res;
  uint8 internal_opcode;
  _oam_oam_a_b_table_buffer_t oama_buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

  res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit,&oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  oam1_key.inject = 0;
  oam1_key.is_bfd = is_bfd;
  oam_payload.counter_disable = 1;
  oam_payload.sub_type = 0; /*not relevant because sub_type is not used in static cases */
  /*ingress*/
  oam_payload.meter_disable = 1;
  oam_payload.forwarding_strength = 7;
  /*egress*/
  oam_payload.mirror_profile = 0;
  oam_payload.forward_disable = 1;

  /* Active and passive static entries - according to profile */
  for (is_ingress = 0; is_ingress <=1; is_ingress++) {
      oam1_key.ingress = is_ingress;
      for (mep_type = 0; mep_type < SOC_PPD_OAM_MP_TYPE_COUNT; mep_type++) {
          oam1_key.mep_type = mep_type;
          for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
              oam1_key.my_cfm_mac = is_my_cfm_mac;
              for (mip_type = 0; mip_type < SOC_PPD_OAM_MP_TYPE_COUNT; mip_type++) {
                  oam1_key.mip_type = mip_type;
                  for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
                      res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                      oam1_key.opcode = internal_opcode;
                      if (profile == ARAD_PP_OAM_PROFILE_PASSIVE) { /* Passive static entries */
                          if (mep_type == SOC_PPD_OAM_MP_TYPE_ABOVE) {
                              if (mip_type == SOC_PPD_OAM_MP_TYPE_MATCH) { 
                                  /* default MIP match behavior */
                                  _ARAD_PP_DEFAULT_MIP_BEHAVIOR_GET(opcode, is_my_cfm_mac, action);
                              }
                              else { /* mip_type is not SOC_PPD_OAM_MP_TYPE_MATCH */
                                  action = _ARAD_PP_OAM_ACTION_FRWRD;
                              }
                          }
                          else { /* mep_type is not SOC_PPD_OAM_MP_TYPE_ABOVE */
                              action = _ARAD_PP_OAM_ACTION_TRAP_ERROR_PASSIVE;
                          }
                      }
                      else { /* Active static entries */
                          if (mep_type == SOC_PPD_OAM_MP_TYPE_ABOVE) {
                              if (mip_type == SOC_PPD_OAM_MP_TYPE_MATCH) {
                                  /* default MIP match behavior */
                                  _ARAD_PP_DEFAULT_MIP_BEHAVIOR_GET(opcode, is_my_cfm_mac, action);
                              }
                              else { /* mip_type is not SOC_PPD_OAM_MP_TYPE_MATCH */
                                  action = _ARAD_PP_OAM_ACTION_FRWRD;
                              }
                          }
                          else { /* Other actions should be configured when adding a new mep */
                              action = _ARAD_PP_OAM_ACTION_TRAP_ERROR_LEVEL;      
                          }
                      }
                      switch (action) {
                             case _ARAD_PP_OAM_ACTION_TRAP_ERROR_PASSIVE: {
                              oam_payload.forward_disable = 1;
                              oam_payload.snoop_strength = 0;                                      
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile[0];
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_TRAP_ERROR_LEVEL: {
                              oam_payload.forward_disable = 1;
                              oam_payload.snoop_strength = 0;
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_LEVEL, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile[0];
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }

                          /* _ARAD_PP_DEFAULT_MIP_BEHAVIOR_GET may assign _ARAD_PP_OAM_ACTION_RECYCLE to action */
                          /* coverity[dead_error_begin : FALSE] */
                          case _ARAD_PP_OAM_ACTION_RECYCLE: {
                              oam_payload.forward_disable = 1;
                              oam_payload.snoop_strength = 0;
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_RECYCLE, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile[0];
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_RECYCLE, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_FRWRD: {
                              oam_payload.forward_disable = 0;
                              oam_payload.snoop_strength = 0;
                              oam_payload.mirror_profile = 0;
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_SNOOP: {
                              oam_payload.forward_disable = 0;
                              oam_payload.snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_SNOOP_TO_CPU, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile[0];
                              res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_SNOOP, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 249, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_TRAP_CPU:
                          case _ARAD_PP_OAM_ACTION_TRAP_OAMP: {
                              continue;
                          }
                      }
                      oam1_key.mp_profile = profile;

                      res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
                  }
              }
          }
      }
  }

  res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit,&oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);


exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_classifier_profile_reset()",0,0);
}

/*********************************************************************
* NAME:
 *   arad_pp_oam_tcam_init_inner_eth
 * TYPE:
 *   PROC
 * FUNCTION: *
 *   Init mpls. 
 * INPUT:
 *   SOC_SAND_IN  uint32                   unit
 * REMARKS:
 *   Used internally.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_tcam_init_inner_eth(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  uint32 mdl, tags;
  SOC_PPD_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPD_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  SOC_PPD_PKT_TERM_TYPE tunnel_termination_code;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (tunnel_termination_code = 0; tunnel_termination_code < SOC_PPD_NOF_PKT_TERM_TYPES; tunnel_termination_code++) {
      if ((tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS_ETH) ||
          (tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH) ||
          (tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS3_ETH)) {

          for (tags = 0; tags <2; tags++) {
              /* Need a per-mdl entries */
              for (mdl = 0; mdl <= _ARAD_PP_OAM_MAX_MD_LEVEL; mdl++) {
                  SOC_PPD_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
                  SOC_PPD_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);

                  /* fill key */
                  oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_ETHERNET; /* Ethernet */
                  oam_tcam_entry_key.ttc = tunnel_termination_code;
                  oam_tcam_entry_key.mdl = mdl;
                  oam_tcam_entry_key.channel_type = tags; 	/* dummy to indicate location of ethertype and CFM */

                  /* fill action*/
                  oam_tcam_entry_action.is_oam = 1;
                  oam_tcam_entry_action.opcode = 1;     /* Treat all as CCM */
                  oam_tcam_entry_action.mdl = mdl;
                  oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
                  oam_tcam_entry_action.oam_offset = 14 + (tags*4); /* skip Ethernet */
                  ADD_TCAM_ENTRY_AND_SET_WB_VAR(oam_tcam_entry_key, oam_tcam_entry_action);
              }
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_init_inner_eth()",0,0);
}

/*********************************************************************
* NAME:
 *   arad_pp_oam_tcam_init_mpls
 * TYPE:
 *   PROC
 * FUNCTION: *
 *   init mpls *
 * INPUT:
 *   SOC_SAND_IN  int                   unit
 *   SOC_SAND_IN int8                   is_bfd -
 *     1 if is bfd, zero otherwise
 * REMARKS:
 *   Used internally.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_tcam_init_mpls(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint8                                       is_bfd
  )
{
  uint32 res;
  uint32 opcode;
  SOC_PPD_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPD_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 channel_type_ndx, channel_format;
  SOC_PPD_PKT_TERM_TYPE tunnel_termination_code, nof_tunnels;
  uint8 internal_opcode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* From the 16 channel-types, we should support 5 types: Y1731, BFD-Raw, BFD-IPv4 or BFD-IPv6 and 6374 */
  for (tunnel_termination_code = 0; tunnel_termination_code < SOC_PPD_NOF_PKT_TERM_TYPES; tunnel_termination_code++) {
      if ((tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS_ETH) ||
          (tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH) ||
          (tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS3_ETH)) {

          nof_tunnels = (tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS_ETH) ? 1 :
                        (tunnel_termination_code == SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH) ? 2 : 3;

          for (channel_type_ndx = 0; channel_type_ndx < _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_NOF; channel_type_ndx++) {
              /*fill key, action*/
              SOC_PPD_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
              SOC_PPD_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
              oam_tcam_entry_action.is_oam = 1;
              oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
              oam_tcam_entry_key.mpls_label = SOC_PPD_MPLS_TERM_RESERVED_LABEL_GAL;
              oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_MPLS; /* Forwarding-Code MPLS */
              oam_tcam_entry_key.ttc = tunnel_termination_code;
              if (nof_tunnels == 3) {
                    oam_tcam_entry_action.oam_offset = 8; /*skip GAL+GAch + number of tunnels * length of a tunnel header */
                    oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) + 8; 
              } else {
                    oam_tcam_entry_action.oam_offset = 8 + nof_tunnels * 4; /*skip GAL+GAch + number of tunnels * length of a tunnel header */
                    oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) + 8 + nof_tunnels * 4;
              }
              _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(channel_type_ndx, oam_tcam_entry_key.channel_type);
              if (oam_tcam_entry_key.channel_type != 0) {
                  _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_FORMAT(channel_type_ndx, channel_format);
                  if (is_bfd) {
                      if (channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW) {
                          oam_tcam_entry_key.mdl = 0;
                          oam_tcam_entry_key.opcode = 0;
                          oam_tcam_entry_action.is_bfd = 1;
                          oam_tcam_entry_action.opcode = 0; 
                          oam_tcam_entry_action.mdl = 0;
                          ADD_TCAM_ENTRY_AND_SET_WB_VAR(oam_tcam_entry_key, oam_tcam_entry_action);
                      }
                  } else { /* oam */
                      if (channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731 || channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_OAM_UNKNOWN) {
                          oam_tcam_entry_key.mdl = _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL;
                          oam_tcam_entry_action.is_bfd = 0;
                          oam_tcam_entry_action.mdl = _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL;
                          if (channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731) {
                              for (opcode = 0; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
                                  if ((_ARAD_PP_OAM_MPLS_TCAM_SUPPORTED_OPCODE(opcode)) || (opcode == (SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT - 1))) {
                                      oam_tcam_entry_key.opcode = opcode;
                                      res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
                                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                                      oam_tcam_entry_action.opcode = (uint32)internal_opcode;
                                      if (nof_tunnels == 3) {
                                          oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(opcode) + 8;
                                      } else {
                                          oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(opcode) + 8 + nof_tunnels * 4;
                                      }
                                      ADD_TCAM_ENTRY_AND_SET_WB_VAR(oam_tcam_entry_key, oam_tcam_entry_action);
                                  }
                              }
                          } else {
                             /* Mapping unknown channel types to opcode CCM */
                              res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                              oam_tcam_entry_key.opcode = SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM;
                              oam_tcam_entry_action.opcode = (uint32)internal_opcode;
                              ADD_TCAM_ENTRY_AND_SET_WB_VAR(oam_tcam_entry_key, oam_tcam_entry_action);
                          }
                      }
                  }
              }
          }
      }
  }

  if (is_bfd && SOC_DPP_CONFIG(unit)->pp.bfd_mpls_lsp_support ) {
      res = arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_init_mpls()",0,0);
}

/*********************************************************************
*     Init TCAM MIM DB
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_tcam_init_mim(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  uint8  mdl;
  uint32 opcode;
  SOC_PPD_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPD_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint8 internal_opcode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* CFM over MIM */
  for (mdl = 0; mdl < _ARAD_PP_OAM_MAX_MD_LEVEL; mdl++) {
      for (opcode=0; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
          if ((_ARAD_PP_OAM_MIM_TCAM_SUPPORTED_OPCODE(opcode)) || (opcode==(SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT-1))) {
              /*fill key, action*/
              SOC_PPD_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
              oam_tcam_entry_key.ttc = SOC_PPC_NOF_PKT_TERM_TYPES;
              oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_ETHERNET; /* Forwarding-Code Ethernet */
              oam_tcam_entry_key.mdl = mdl;
              oam_tcam_entry_key.opcode = opcode;

              SOC_PPD_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
              oam_tcam_entry_action.is_oam = 1;
              oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
              res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
              oam_tcam_entry_action.opcode = (uint32)internal_opcode;
              oam_tcam_entry_action.mdl = mdl;
              oam_tcam_entry_action.oam_lif_tcam_result = 0;
              oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
              oam_tcam_entry_action.oam_offset = 14;
              oam_tcam_entry_action.oam_pcp = 0;
              oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(opcode);
              if ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTM) ||
                 (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR) ||
                 (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTR)) {

                  oam_tcam_entry_action.my_cfm_mac = 0;
              }
              else {
                  oam_tcam_entry_action.my_cfm_mac = 1;
              }

              ADD_TCAM_ENTRY_AND_SET_WB_VAR(oam_tcam_entry_key, oam_tcam_entry_action);
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_init_mim()",0,0);
}

/*********************************************************************
*     Add static entry for BFD LSP.
*     
*********************************************************************/


uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  SOC_PPD_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPD_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 tcam_entry_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*fill key, action*/
  SOC_PPD_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
  oam_tcam_entry_key.ttc = SOC_PPC_PKT_TERM_TYPE_MPLS_ETH;
  oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_IPV4_UC;
  oam_tcam_entry_key.mdl = 0;
  oam_tcam_entry_key.opcode = 0;
  oam_tcam_entry_key.your_disc = 0;

  SOC_PPD_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
  oam_tcam_entry_action.is_oam = 1;
  oam_tcam_entry_action.is_bfd = 1;
  oam_tcam_entry_action.your_discr = 0;/*Lif solution*/
  oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
  oam_tcam_entry_action.opcode = 0;
  oam_tcam_entry_action.mdl = 0;
  oam_tcam_entry_action.oam_lif_tcam_result = 0;/*use original LIF*/
  oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
  oam_tcam_entry_action.oam_offset = 20+8; /*IPV4+UDP*/
  oam_tcam_entry_action.oam_pcp = 0;
  oam_tcam_entry_action.oam_stamp_offset = 20+8 + _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(oam_tcam_entry_action.opcode); /*IPV4+UDP*/
  oam_tcam_entry_action.my_cfm_mac = 0;

  res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID, &tcam_entry_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);


  if (tcam_entry_id > _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Maximum number of TCAM entries exceeded.")));
  }

  _ARAD_PP_OAM_TCAM_NEW_ENTRY(oam_tcam_entry_key, oam_tcam_entry_action, tcam_entry_id);

  ++tcam_entry_id;
  res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID, &tcam_entry_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_bfd_entry_add()",0,0);
}


uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_delete_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 tcam_entry_index

  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = arad_tcam_managed_db_entry_remove_unsafe(unit, ARAD_PP_OAM_IDENTIFICATION, tcam_entry_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_bfd_entry_delete()",0,0);
}


uint32
  arad_pp_oam_default_simple_in_lif_profile_mapping_set_unsafe(
    SOC_SAND_IN  int                                 unit
    ) {
  uint32 res;
  uint32 inlif_profile;
  uint32 oam_trap_profile = 0;
  uint32 inlif_oam_map = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (inlif_profile = 0; inlif_profile < (1<<SOC_OCC_MGMT_INLIF_SIZE); inlif_profile++) {
      /* For each profile, extract the oam profile bits and set the same value in the mapping (1:1) */
      res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_OAM,
                                 &inlif_profile, &oam_trap_profile));
      SOC_SAND_IF_ERR_EXIT(res);
      inlif_oam_map |= (oam_trap_profile << (2*inlif_profile));
  }

  ARAD_DEVICE_CHECK(unit, exit);
  /* Write mapping to the HW */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHB_OAM_IN_LIF_PROFILE_MAPr(unit, inlif_oam_map));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_default_simple_in_lif_profile_mapping_set_unsafe()",0,0);
}


/*********************************************************************
*     Init OAM classifier
*     Parameters:
*       SOC_SAND_IN  int                                 unit,
*       SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO * init_trap_info_oam -
*               oam trap info
*       SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO * init_trap_info_bfd -
*               bfd trap info
*       SOC_SAND_IN  uint8                                    is_bfd -
*               indication whether we init oam or bfd
*       SOC_SAND_IN  uint8                                    is_second_init -
*               was this function called already from the other (oam/bfd) module
*********************************************************************/


uint32
  arad_pp_oam_classifier_init_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO * init_trap_info_oam,
    SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO * init_trap_info_bfd,
    SOC_SAND_IN  uint8                                    is_bfd,
    SOC_SAND_IN  uint8                                    is_second_init
  )
{
  uint32 res;
  uint32 reg;
  uint32 entry, field;
  uint32 port_index;
  uint32 profile;
  uint64 reg_64;
  uint64 field_64;
  soc_reg_above_64_val_t  reg_above_64;
  uint32 mirror_trap_code;
  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO   profile_info;
  uint32 mapped_fap_id, mapped_fap_port_id;
  uint32 channel_type_ndx, channel_format, oam_map_channel_cam_entry;
  uint32 oam_trap_code;
  uint32 channel_type;
  uint32 opcode;
  uint8  internal_opcode;
  uint32 egress_count=0, ingress_count=0, counter_proc;
  uint8 tdm_bugfix;
  soc_mem_t
      pinfo_flp_mem = SOC_IS_JERICHO(unit) ? IHP_PINFO_FLP_0m : IHB_PINFO_FLPm;
  int   core, block;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_INIT_UNSAFE);
  COMPILER_64_ZERO(reg_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_OAM_IDENTIFICATION_ENABLEr(unit, 0, &reg));
  if (is_bfd) {
      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_MULTI_HOP_BFD_ENABLEf, 1);
      if (SOC_IS_JERICHO(unit)) {
                soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_ONE_HOP_BFD_ENABLEf, 1);
      } else {
          soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_ONE_HOP_BFD_ENABLEf,
                            !SOC_DPP_CONFIG(unit)->pp.bfd_mpls_lsp_support); 
      }

      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_MPLS_TP_OR_BFD_ENABLE_MAPf, 0xff); 
      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_BFD_VALIDITY_CHECKf, 1);
  }
  else { /* Ehernet OAM */
      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_ETHERNET_ENABLEf, 1);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHP_OAM_IDENTIFICATION_ENABLEr(unit, SOC_CORE_ALL, reg));

  /* Setting traps */
  if (is_bfd) {
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP, oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_DROP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP, oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_bfd->trap_ids[SOC_PPD_BFD_TRAP_ID_CPU], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_TRAP_TO_CPU, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_bfd->trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_CC_MPLS_TP], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_CC_MPLS_TP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_bfd->trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_IPV4], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_IPV4, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_bfd->trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_MPLS], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_MPLS, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_bfd->trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_PWE], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_OAMP_BFD_PWE, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
  }
  else {  /* OAM */
      /* Set LB program on recycle port */
      ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
      res = arad_pp_trap_frwrd_profile_info_get(unit,  /*ARAD_PP_TRAP_CODE*/init_trap_info_oam->trap_ids[SOC_PPD_OAM_TRAP_ID_RECYCLE], &profile_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);
      if (profile_info.dest_info.frwrd_dest.type != ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT) {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 220, exit);
      }
      res = arad_sys_phys_to_local_port_map_get_unsafe(unit, profile_info.dest_info.frwrd_dest.dest_id, &mapped_fap_id, &mapped_fap_port_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);
      SOC_REG_ABOVE_64_CLEAR(reg_above_64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm(unit, MEM_BLOCK_ANY, mapped_fap_port_id, &reg_above_64));
      reg = 6; /* pp_profile of loopback program */
      soc_EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm_field_set(unit, &reg_above_64, PRGE_PROFILEf, &reg);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 235, exit, WRITE_EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm(unit, MEM_BLOCK_ANY, mapped_fap_port_id, &reg_above_64));

      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP, oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_DROP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP, oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_oam->trap_ids[SOC_PPD_OAM_TRAP_ID_RECYCLE], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_RECYCLE, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_oam->trap_ids[SOC_PPD_OAM_TRAP_ID_SNOOP], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_SNOOP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_oam->trap_ids[SOC_PPD_OAM_TRAP_ID_ERR_LEVEL], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_oam->trap_ids[SOC_PPD_OAM_TRAP_ID_ERR_PASSIVE], oam_trap_code);
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);

      /* Verify that ppd trap id is mapped to the corret internal id which is used by pmf */
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_oam->upmep_trap_ids[SOC_PPD_OAM_UPMEP_TRAP_ID_CPU], mirror_trap_code);

#ifdef  _ARAD_PP_OAM_LOOPBACK_UPMEP_SUPPORTED /* LoopBack upmep is not supported in ARAD */

      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,init_trap_info_oam->upmep_trap_ids[SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE], mirror_trap_code);

      if (mirror_trap_code != SOC_PPD_TRAP_CODE_INTERNAL_OAM_ITMH) {
           SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 245, exit);
      }
#endif /* LoopBack upmep is not supported in ARAD */

      /* Mirror profiles */
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_RECYCLE, &init_trap_info_oam->mirror_ids[SOC_PPD_OAM_MIRROR_ID_RECYCLE]);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_SNOOP_TO_CPU, &init_trap_info_oam->mirror_ids[SOC_PPD_OAM_MIRROR_ID_SNOOP]);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_LEVEL, &init_trap_info_oam->mirror_ids[SOC_PPD_OAM_MIRROR_ID_ERR_LEVEL]);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE, &init_trap_info_oam->mirror_ids[SOC_PPD_OAM_MIRROR_ID_ERR_PASSIVE]);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      if (SOC_IS_ARADPLUS(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r(unit, REG_PORT_ANY, &reg));
          tdm_bugfix = !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_tdm_bugfix", 0));
          soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg, CFG_BUG_FIX_87_DISABLEf, tdm_bugfix);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r(unit, REG_PORT_ANY, reg));
      }
  }

  if (is_bfd) {
      /* BFD registers */
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg_64));
      COMPILER_64_SET(field_64,0,0xFFFF );
      soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg_64, BFD_INVALID_YOUR_DISCR_LIFf, field_64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, WRITE_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, SOC_CORE_ALL, reg_64));

      COMPILER_64_SET(field_64,0,0xec8 );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IHP_OAM_BFD_OVER_IP_CFGr, REG_PORT_ANY, 0, UDP_DST_PORT_BFD_ONE_HOPf,  field_64)); /* Fixing udp dest port default value */
  }

  if (!is_second_init) {
	  /* Mapping default Y1731 opcodes to internal opcodes mapping */
	  for (opcode = 0; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT;  opcode++) {
		  switch (opcode) {
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_CCM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LBR;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LBM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTR:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LTR;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LTM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_AIS:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_AIS;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LCK:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LCK;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LINEAR_APS:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LINEAR_APS;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LMR;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_LMM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_1DM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_DMR;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_DMM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_SLM;
			  break;
		  case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_SLR;
			  break;
		  default:
			  internal_opcode = SOC_PPD_OAM_OPCODE_MAP_DEFAULT;
		  }
		  res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
		  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
	  }

	  /* 
	   * Ethernet Oam Opcode Map
	   */
	  res = arad_pp_oam_eth_oam_opcode_map_set_unsafe(unit);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 236, exit);

	  /* Fill OAM Channel types CAM */
	  oam_map_channel_cam_entry = 0;
	  for (channel_type_ndx = 0; channel_type_ndx < _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_NOF; channel_type_ndx++) {
		  _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_CHANNEL_TYPE(channel_type_ndx, channel_type);
		  _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_TO_FORMAT(channel_type_ndx, channel_format);
		  switch (channel_format) {
		  case _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731:
		  case _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW:
			  entry = 0;
			  soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, CHANNEL_TYPEf, &channel_type);
			  field = 1;
			  soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, VALIDf, &field);
			  SHR_BITCOPY_RANGE(&entry, 17, &channel_format, 0, 3); /* wrong fields documentation of this register , do not use the regular function */
			  /*SHR_BITCOPY_RANGE(entry, 20, opcode, 0, 4); */ 
			  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_entry, &entry));
			  oam_map_channel_cam_entry++;
			  break;
		  default:
			  break;
		  }
	  }

	  /* TCAM init */
	  res = arad_pp_oam_tcam_database_create(unit);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  res = arad_pp_oam_counter_range_set_unsafe(unit, 0, 0);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

	  /* 
	   * Init Classifier registers
	   */
	  if (SOC_IS_ARADPLUS_A0(unit)) { /* Arad plus and only arad plus*/
		  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_FLP_GENERAL_CFGr(unit, 0, &reg_64));
		  COMPILER_64_SET(field_64, 0, SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit));
		  soc_reg64_field_set(unit, IHP_FLP_GENERAL_CFGr, &reg_64, OAM_MP_TYPE_DECODING_ENABLEf, field_64);
		  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_ALL, reg_64));
	  }

	  /* Set ENABLE_PP_INJECT=1 to enable injection from all ports */
      SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
          block = SOC_IS_JERICHO(unit) ? IHP_BLOCK(unit, core) : MEM_BLOCK_ANY;
    	  for (port_index = 0; port_index < ARAD_PORT_NOF_PP_PORTS; port_index++) {
              res = soc_mem_read(unit, pinfo_flp_mem, block, port_index, &entry);
              SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
              soc_mem_field32_set(unit, pinfo_flp_mem, &entry, ENABLE_PP_INJECTf, 1);
              res = soc_mem_write(unit, pinfo_flp_mem, block, port_index, &entry);
              SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    	  }
      }

	  /* Ethernet invalid lif */
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, EPNI_OAM_INVALID_LIFr, SOC_CORE_ALL, 0, OAM_INVALID_LIFf,  0xffff));

      COMPILER_64_ZERO(reg_64);
      COMPILER_64_SET(field_64, 0, 11);
	  soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_0f, field_64);
      if (SOC_IS_JERICHO(unit)) { /* support only one value for now*/
          soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_1f, field_64);
          soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_2f, field_64);
          soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_3f, field_64);
      }
      COMPILER_64_SET(field_64, 0, SOC_DPP_INVALID_GLOBAL_LIF(unit)); 
	  soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, OAM_LIF_INVALID_HEADERf, field_64);
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_OAM_ETHERNET_CFGr(unit,SOC_CORE_ALL, reg_64));

	  reg = 0;
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_IN_RANGE_CFGr, &reg, OAM_COUNTER_IN_RANGE_CFG_BASEf, 0);
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_IN_RANGE_CFGr, &reg, OAM_COUNTER_IN_RANGE_CFG_SHIFTf, 0);
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_IN_RANGE_CFGr, &reg, OAM_COUNTER_IN_RANGE_CFG_ADD_PCPf, 0);
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_IHP_OAM_COUNTER_IN_RANGE_CFGr(unit, SOC_CORE_ALL, reg));

	  reg = 0;
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr, &reg, OAM_COUNTER_NOT_IN_RANGE_CFG_BASEf, 0);
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr, &reg, OAM_COUNTER_NOT_IN_RANGE_CFG_SHIFTf, 3);
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr, &reg, OAM_COUNTER_NOT_IN_RANGE_CFG_ADD_PCPf, 1);
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr(unit, SOC_CORE_ALL, reg));

	  reg = 0;
	  /* All the sub-types increment is enabled by default beside LM.
	   * LM packet count is enabled / disabled through this register.
	   * All other packers count is enabled / disabled through counter_disable OAM1,2 payload field.
	   *  The reason for this is that counter_disable field in LM can not be 1 (it disables the stamping).        
	   */
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg, OAM_COUNTER_INCREMENTf, 0x3d);
	  /* Using data subtype for which increment counter value is 1 */
	  soc_reg_field_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg, OAM_EGRESS_SUB_TYPEf, 0);
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHP_OAM_COUNTER_INCREMENT_BITMAPr(unit, SOC_CORE_ALL, reg));

	  /* Setting CRPS count mode of OAM counters to 4:
	   * each entry counts IHB commands - might increment the value or not, according to the IHB command.*/
	  for (counter_proc = 0; counter_proc < _NUMBER_OF_COUNTER_PROCESSORS(unit); ++counter_proc) {
		  char *propval = soc_property_port_get_str(unit, counter_proc, spn_COUNTER_ENGINE_SOURCE);
		  if (propval == NULL) {
			  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("soc_property_port_get_str failed for COUNTER_ENGINE_SOURCE.")));
		  }
		  if (sal_strcmp(propval, "INGRESS_OAM") == 0  || sal_strcmp(propval, "EGRESS_OAM") == 0) {
			  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  counter_proc, CRPS_N_COUNT_MODEf,  4));
			  if (sal_strcmp(propval, "INGRESS_OAM") == 0) {
				  LOG_DEBUG(BSL_LS_SOC_OAM,
                                            (BSL_META_U(unit,
                                                        "CRPS %d to be used for ingress OAM counter.\n"), counter_proc));
				  ++ingress_count;
			  } else {
				  LOG_DEBUG(BSL_LS_SOC_OAM,
                                            (BSL_META_U(unit,
                                                        "CRPS %d to be used for egress OAM counter.\n"), counter_proc));
				  ++egress_count;
			  }
		  }
	  }
	  if (!is_bfd && ((egress_count * ingress_count) == 0) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
		  LOG_WARN(BSL_LS_SOC_OAM,
                           (BSL_META_U(unit,
                                       "Warning: a counter must be allocated for egress OAM and ingress OAM each.\n")));
	  }

	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHP_OAM_IN_LIF_PROFILE_MAPr, REG_PORT_ANY, 0, OAM_IN_LIF_PROFILEf,  0));
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_BUILD_OAM_TS_HEADERr, SOC_CORE_ALL, 0, BUILD_OAM_TS_HEADERf,  0xe)); /*LM&DM*/

      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_MIP_BITMAPr(unit,  0));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_MEP_BITMAPr(unit,  0));
      }
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_SUB_TYPE_MAPr(unit, SOC_CORE_ALL,  0x393)); /* Map each OAM sub type to 2 bit data type. 3 - none, 0 - counter-value, 1 - TOD-1588, 2 - NTP */

	  /* 
	   * Setting OAM_PROCESS_MAP to 0x0000000000000c4000146000:
	   * key 6 (subtype 1 mep type 1 upmep) - 101 Stamp counter
	   * key 4 (subtype 1 mep type 0 downmep) - 110 Read counter and stamp
	   * key 14 (subtype 3 mep type 1 upmep) - 011 Ingress stamp, NTP ToD (Time of Day)
	   * key 12 (subtype 3 mep type 0 downmep) - 100 Egress stamp, NTP ToD (Time of Day) 
	   *  
	   *  
	   */
	  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
	  OAM_PROCESS_ADD(1, 1, OAM_PROCESS_ACTION_STAMP_COUNTER, reg_above_64);
	  OAM_PROCESS_ADD(1, 0, OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP, reg_above_64);
	  OAM_PROCESS_ADD(3, 1, OAM_PROCESS_ACTION_INGRESS_STAMP_NTP, reg_above_64);
	  OAM_PROCESS_ADD(3, 0, OAM_PROCESS_ACTION_EGRESS_STAMP_NTP, reg_above_64);
	  OAM_PROCESS_ADD(2, 1, OAM_PROCESS_ACTION_INGRESS_STAMP_1588, reg_above_64);
	  OAM_PROCESS_ADD(2, 0, OAM_PROCESS_ACTION_EGRESS_STAMP_1588, reg_above_64);


	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, WRITE_EPNI_OAM_PROCESS_MAPr(unit, REG_PORT_ANY, reg_above_64));

	  /* Enable injection for Packet Format Qualifier0 = 7 */
	  reg = 1 << _ARAD_PP_OAM_PTCH_OPAQUE_VALUE;
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  191,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHP_OAM_PACKET_INJECTED_BITMAPr, REG_PORT_ANY, 0, PACKET_INJECTED_BITMAPf,  reg));

	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  195,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OEMA_MNGMNT_UNIT_ENABLEf,  1));
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OEMB_MNGMNT_UNIT_ENABLEf,  1));

      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          /*TOD*/
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  205,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, TOD_MODEf,  3/*Both 1588 and NTP*/));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_TOD_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TOD_SYNC_ENABLEf,  1));
          /* Enabling NTP TOD */
          /* write 0 to control register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0, 1);
          /* write value to frac sec lower register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x13576543, 3);
          /* write value to frac sec upper register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1ffff00, 4);
          /* write value to frequency register (4 nS in binary fraction) */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x44b82fa1, 2);
          /* write value to time sec register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x80804560, 5);
          /* write to control register to load values */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f, 1);
          /* write to control register to disable load values */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x0, 1);
          /* write to control register to enable counter */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20, 1);

          /* Now 1588 TOD */
          /* write 0 to control register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0, 6);
          /* write value to frac sec lower register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x13576543, 8);
          /* write value to frac sec upper register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1ffff00, 9);
          /* write value to frequency register (4 nS) */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x10000000, 7);
          /* write value to time sec register */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x80804560, 10);
          /* write to control register to load values */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f, 6);
          /* write to control register to disable load values */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x0, 6);
          /* write to control register to enable counter */
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20, 6);
      } 
/* TOD for Jericho handled separatly*/


	  if (SOC_IS_ARADPLUS_A0(unit)) {
		  /* Setting the following register fixes a HW bug that causes injected LMMs to get counted.*/
		  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 32, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_REG_0092r, SOC_CORE_ALL, 0, ITEM_2_2f,  1));
	  }


	  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_dm_tod_msb_add_enable", 1) == 0) {
		  LOG_WARN(BSL_LS_SOC_OAM,
                           (BSL_META_U(unit,
                                       "Warning: OAMP acceleration of DM packets will not work correctly when custom_feature_oam_dm_tod_msb_add_enable is 0\n")));
	  }

      if (soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0) > 0) { /* Simple mode */
          /* Create 1:1 mapping between the Inlif-Profile (4) to the OAM-Trap-Profile(2) */
          res = arad_pp_oam_default_simple_in_lif_profile_mapping_set_unsafe(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }

  }

  if (!is_bfd && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mim", 0) == 1)) {
	  /* TCAM initialization - MIM */
	  res = arad_pp_oam_tcam_init_mim(unit);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      /* TCAM initialization - MPLS */
      res = arad_pp_oam_tcam_init_mpls(unit, is_bfd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); 
  }


  if (!is_bfd && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 0) == 1)) {
	  /* TCAM initialization - Inner Ethernet */
	  res = arad_pp_oam_tcam_init_inner_eth(unit);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }



  /* Init non-accelerated profiles - only in ARAD. In ARAD+ no need to init */
  if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
      for (profile = 0; profile < _ARAD_PP_OAM_NON_ACC_PROFILES_NUM_ARAD; profile++) {
          res = arad_pp_oam_classifier_profile_reset(unit, profile, is_bfd);
          SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Init OAM
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_init_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO * init_trap_info_oam,
    SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO * init_trap_info_bfd,
    SOC_SAND_IN  uint8                                  is_bfd
  )
{
    uint32 res = 0;
    ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
    uint32 cpu_trap_code;
    uint8 is_second_init=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_INIT_UNSAFE);

    if (is_bfd) {
        res = arad_pp_sw_db_oam_enable_get(unit, &is_second_init);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else {
        res = arad_pp_sw_db_bfd_enable_get(unit, &is_second_init);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
    cpu_trap_code = is_bfd ? init_trap_info_bfd->trap_ids[SOC_PPD_BFD_TRAP_ID_CPU] : init_trap_info_oam->trap_ids[SOC_PPD_OAM_TRAP_ID_CPU];
    res = arad_pp_trap_frwrd_profile_info_get(unit, cpu_trap_code, &profile_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_oam_oamp_init_unsafe(unit, is_bfd, profile_info.dest_info.frwrd_dest.dest_id, is_second_init);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_oam_classifier_init_unsafe(unit, init_trap_info_oam, init_trap_info_bfd, is_bfd, is_second_init);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (SOC_IS_JERICHO(unit) && !is_second_init) {
        res = soc_jer_pp_oam_init(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222,exit,res);

        res = soc_jer_pp_oam_init_eci_tod(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222,exit,res);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_init_unsafe()", 0, 0);
}

/*********************************************************************
*     De-Init OAM
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_deinit_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint8                                  is_bfd,
    SOC_SAND_IN  uint8                                  tcam_db_destroy
  )
{
    uint32 res = 0;
    uint32 entry_id;
    uint32 tcam_last_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_DEINIT_UNSAFE);

    if (!is_bfd) {
        res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.free(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }

    /* TCAM management */
    if (tcam_db_destroy) {
        res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TCAM_LAST_ENTRY_ID, &tcam_last_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
        for (entry_id = 0; entry_id <= tcam_last_id; entry_id++) {
            res = arad_tcam_managed_db_entry_remove_unsafe(unit, ARAD_PP_OAM_IDENTIFICATION, entry_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
        res = arad_tcam_access_destroy_unsafe(unit, ARAD_PP_OAM_IDENTIFICATION);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_deinit_unsafe()", 0, 0);
}


/*********************************************************************
*     Set Icc Map Register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_icc_map_register_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA     * data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32  field_32[2] = {0};
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  field_32[1] = COMPILER_64_HI(*data);
  field_32[0] = COMPILER_64_LO(*data);

  if (icc_ndx < 10) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ICC_MAP_REG_1r(unit, reg_val));
      SHR_BITCOPY_RANGE(reg_val, SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS*icc_ndx, field_32, 0, SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_ICC_MAP_REG_1r(unit, reg_val));
  }
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_ICC_MAP_REG_2r(unit, reg_val));
      SHR_BITCOPY_RANGE(reg_val, (SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS*icc_ndx-10), field_32, 0, SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_ICC_MAP_REG_2r(unit, reg_val));
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_icc_map_register_set_unsafe()", icc_ndx, 0);
}

uint32
  arad_pp_oam_icc_map_register_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA   *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_icc_map_register_set_verify()", 0, 0);

}

uint32
  arad_pp_oam_icc_map_register_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_OUT  SOC_PPD_OAM_ICC_MAP_DATA     * data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32  field_32[2] = {0};
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  if (icc_ndx < 10) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ICC_MAP_REG_1r(unit, reg_val));
      SHR_BITCOPY_RANGE(field_32, 0, reg_val, SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS*icc_ndx, SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS);
  }
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_ICC_MAP_REG_2r(unit, reg_val));
      SHR_BITCOPY_RANGE(field_32, 0, reg_val, (SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS*icc_ndx-10), SOC_PPD_OAM_ICC_MAP_DATA_NOF_BITS);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_ICC_MAP_REG_2r(unit, reg_val));
  }

  COMPILER_64_SET(*data, field_32[1], field_32[0]);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_icc_map_register_get_unsafe()", icc_ndx, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_oam_my_cfm_mac_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address,
    SOC_SAND_IN  uint32                    table_index
  )
{
  uint32 res = SOC_SAND_OK;
  soc_reg_above_64_val_t entry;
  soc_reg_above_64_val_t lsb_bitmap, lsb_bitmap_new;
  soc_reg_above_64_val_t msb;
  soc_reg_above_64_val_t msb_prev;
  int is_first_mep;
  uint64 msb_64;
  uint32 mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT        *ref_count = NULL;
  int       core=0; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MY_CFM_MAC_SET);
  SOC_SAND_CHECK_NULL_INPUT(dst_mac_address);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap_new);
  SOC_REG_ABOVE_64_CLEAR(entry);
  SOC_REG_ABOVE_64_CLEAR(msb);
  COMPILER_64_ZERO(msb_64);

  /* Insert into my-cfm-mac table only unicast mac addresses */
  if ((dst_mac_address->address[5] & 0x1) != 1) {

      /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
      /* coverity[overrun-buffer-val : FALSE] */   
      res = soc_sand_pp_mac_address_struct_to_long(dst_mac_address, mac_add_U32);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      _ARAD_PP_OAM_MAC_ADD_MSB_GET(mac_add_U32, msb_64); /*msb = dst_mac_address[47:8]*/
      msb[0] = COMPILER_64_LO(msb_64);
      msb[1] = COMPILER_64_HI(msb_64);

      /*ingress: PP-Port.My-CFM-MAC-msb = DA[47:8] */
      /*         PP-Port.My-CFM-MAC-lsb-bitmap[DA[7:0]] = 1*/
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_IHP_OAM_MY_CFM_MACm(unit, MEM_BLOCK_ANY, table_index, &entry));
      soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &entry, LSB_BITMAPf, lsb_bitmap);
      if (SOC_REG_ABOVE_64_IS_ZERO(lsb_bitmap)) {
          is_first_mep = TRUE;
          soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &entry, MSBf, msb);
      }
      else {
          is_first_mep = FALSE;
          /*The DA must comply with the constraint that all MEPs on the same PP-Port must share same DA[47:8] suffix*/
          SOC_REG_ABOVE_64_CLEAR(msb_prev);
          soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &entry, MSBf, msb_prev);
          if (!SOC_REG_ABOVE_64_IS_EQUAL(msb_prev,msb)) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR, 20, exit);
          }
      }
      /* Increasing reference count of number of meps of same port+lsb*/
      ARAD_ALLOC(ref_count, ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT, 1, "arad_pp_oam_my_cfm_mac_set.ref_count");
      res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.get(unit, ref_count);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      ref_count->ref_count[_ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address)]++;
      res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.set(unit, ref_count);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      SOC_REG_ABOVE_64_CREATE_MASK(lsb_bitmap_new, 1, dst_mac_address->address[0]);
      SOC_REG_ABOVE_64_OR(lsb_bitmap, lsb_bitmap_new);
      soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &entry, LSB_BITMAPf, lsb_bitmap);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_OAM_MY_CFM_MACm(unit, MEM_BLOCK_ANY, table_index, entry));

      /*egress*/
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_EPNI_MY_CFM_MAC_TABLEm(unit, EPNI_BLOCK(unit, core), table_index, &entry));
      if (is_first_mep) {
          soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &entry, MAC_MSBSf, msb);
      } 
      soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &entry, MAC_LSBS_BITMAPf, lsb_bitmap);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_EPNI_MY_CFM_MAC_TABLEm(unit, EPNI_BLOCK(unit, core), table_index, entry));
  }
   
exit:
  ARAD_FREE(ref_count);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_my_cfm_mac_set_unsafe()", 0, table_index);
}

soc_error_t 
    arad_pp_oam_my_cfm_mac_set(int unit, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_pp_oam_my_cfm_mac_set_unsafe(unit, dst_mac_address, table_index);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
********************************************************************/

soc_error_t
  arad_pp_oam_my_cfm_mac_get(int unit, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index){
    int rv, i;
    uint8 lsb;
    uint32 mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] = {0};
    soc_reg_above_64_val_t ihb_entry;
    ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT*        ref_count = NULL;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(dst_mac_address);
    SOC_REG_ABOVE_64_CLEAR(ihb_entry);

    SOCDNX_ALLOC(ref_count, ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT, 1, "arad_pp_oam_my_cfm_mac_delete.ref_count");

    /* Get counters to check if this lsb is configured for this port. */
    rv = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.get(unit, ref_count);
    SOCDNX_IF_ERR_EXIT(rv);

    /* If lsb is not configured, don't read HW. */
    if (ref_count->ref_count[_ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address)] == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("Entry doesn't exist.")));
    }

    /* Get the MSBs */
    rv = READ_IHP_OAM_MY_CFM_MACm(unit, MEM_BLOCK_ANY, table_index, &ihb_entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &ihb_entry, MSBf, mac_in_longs);

    /* Copy the mac address. */
    lsb = dst_mac_address->address[0];
    rv = soc_sand_pp_mac_address_long_to_struct(mac_in_longs, dst_mac_address);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    for (i = SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 1 ; i > 0 ; i--) {
        dst_mac_address->address[i] = dst_mac_address->address[i - 1];
    }
    
    dst_mac_address->address[0] = lsb;

exit:
    SOCDNX_FREE(ref_count);
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_my_cfm_mac_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address,
    SOC_SAND_IN  uint32                    table_index
  )
{
  uint32 res = SOC_SAND_OK;
  soc_reg_above_64_val_t msb;
  soc_reg_above_64_val_t ihb_entry;
  soc_reg_above_64_val_t epni_entry;
  soc_reg_above_64_val_t lsb_bitmap;
  soc_reg_above_64_val_t lsb_bitmap_mask;
  ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT*        ref_count = NULL;
  int core=0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MY_CFM_MAC_DELETE);
  SOC_SAND_CHECK_NULL_INPUT(dst_mac_address);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap);
  SOC_REG_ABOVE_64_CLEAR(ihb_entry);
  SOC_REG_ABOVE_64_CLEAR(epni_entry);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap_mask);
  SOC_REG_ABOVE_64_CLEAR(msb);

  ARAD_ALLOC(ref_count, ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT, 1, "arad_pp_oam_my_cfm_mac_delete.ref_count");
  res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.get(unit, ref_count);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  ref_count->ref_count[_ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address)]--;
  res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.set(unit, ref_count);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (ref_count->ref_count[_ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address)]==0) {
      /* this is the last mep with current LSB - decreasing LSB bitmap field */
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_OAM_MY_CFM_MACm(unit, MEM_BLOCK_ANY, table_index, &ihb_entry));
      soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &ihb_entry, LSB_BITMAPf, lsb_bitmap);
      SOC_REG_ABOVE_64_CREATE_MASK(lsb_bitmap_mask, 1, dst_mac_address->address[0]);
      SOC_REG_ABOVE_64_NOT(lsb_bitmap_mask);
      SOC_REG_ABOVE_64_AND(lsb_bitmap, lsb_bitmap_mask);
      soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &ihb_entry, LSB_BITMAPf, lsb_bitmap);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EPNI_MY_CFM_MAC_TABLEm(unit, EPNI_BLOCK(unit, core), table_index, &epni_entry));
      soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &epni_entry, MAC_LSBS_BITMAPf, lsb_bitmap);
      if (SOC_REG_ABOVE_64_IS_ZERO(lsb_bitmap)) {
          /* this is the last mep with current MSB - reset MSB field */
          soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &ihb_entry, MSBf, msb);
          soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &epni_entry, MAC_MSBSf, msb);
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_OAM_MY_CFM_MACm(unit, MEM_BLOCK_ANY, table_index, ihb_entry));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_EPNI_MY_CFM_MAC_TABLEm(unit, EPNI_BLOCK(unit, core), table_index, epni_entry));
  }
  
exit:
  ARAD_FREE(ref_count);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_my_cfm_mac_delete()", 0, table_index);
}

soc_error_t
  arad_pp_oam_my_cfm_mac_delete(int unit, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_pp_oam_my_cfm_mac_delete_unsafe(unit, dst_mac_address, table_index);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem1_entry_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  )
{
  uint32  res;
  uint32  reg_val; 
  uint32  entry_buffer[3];
  uint32  key[1];
  uint32  payload[2] = {0};
  uint8  found;
#if SOC_DPP_IS_EM_HW_ENABLE
  uint32  failure, fail_valid_val, fail_key_val, fail_reason_val;
#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);

  _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY((*oem1_key), *key);
  _ARAD_PP_OAM_OEM1_PAYLOAD_STRUCT_TO_PAYLOAD((*oem1_payload), payload);    
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OEM1: key %d payload: %d\n\r"), *key, payload[0]));

  ARAD_PP_CLEAR(entry_buffer, uint32, 3);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));
  reg_val = 1; /*insert*/
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_TYPEf, &reg_val);
  reg_val = 0; /*STAMP*/
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_STAMPf, &reg_val);
  reg_val = 0; /*SELF*/
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_SELFf, &reg_val);
  reg_val = 0;
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_RESERVED_LOWf, &reg_val);
  reg_val = 3;/*AGE_STATUS*/
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_AGE_STATUSf, &reg_val);
  reg_val = *key;
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_KEYf, &reg_val);
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_PAYLOADf, payload);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_add_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMA_BASE,
              key,
              ARAD_CHIP_SIM_OEMA_KEY,
              payload,/* payload_data */
              ARAD_CHIP_SIM_OEMA_PAYLOAD,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }
#if SOC_DPP_IS_EM_HW_ENABLE
  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMA_INTERRUPT_REGISTER_ONEr,
          REG_PORT_ANY,
          0,
          OEMA_MANAGEMENT_COMPLETEDf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_VALIDf);

  if (fail_valid_val){
      /*
       *    Failure indication is on.
       *  Check if it is on the last operation
       */       
      fail_key_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_REASONf);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 1);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_entry_set_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem1_entry_get_unsafe(
    SOC_SAND_IN   int                                     unit,
    SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
    SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
    SOC_SAND_OUT  uint8                                      *is_found
  )
{

  uint32  res;
  uint32  key[1];

  uint32  data_out[2];
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  uint64  reg_buffer_64;
  uint64  fld_val;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  COMPILER_64_ZERO(reg_buffer_64);
#endif
  _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY((*oem1_key), *key);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_DIAGNOSTICS_KEYr, REG_PORT_ANY, 0, OEMA_DIAGNOSTICS_KEYf, *key));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_DIAGNOSTICSr, REG_PORT_ANY, 0, OEMA_DIAGNOSTICS_LOOKUPf,  1));

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit) || ((SOC_IS_DONE_INIT(unit)) && (BCM_UNIT_DO_HW_READ_WRITE(unit))))
  {
  /* HW Log is on - get the value from the sw db, and exit. No need to proceed. */
      res = chip_sim_exact_match_entry_get_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMA_BASE,
              key,
              ARAD_CHIP_SIM_OEMA_KEY,
              data_out,
              ARAD_CHIP_SIM_OEMA_PAYLOAD,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      if (*is_found) {
          _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(data_out, (*oem1_payload));
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEM1: key %d payload: %d\n\r"), *key, data_out[0]));
      }
      else {
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEM1: key %d not found\n\r"), *key));
      }


      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
#endif
  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMA_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          OEMA_DIAGNOSTICS_LOOKUPf,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_A_OEMA_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_buffer_64)); 

  ARAD_FLD_FROM_REG64(PPDB_A_OEMA_DIAGNOSTICS_LOOKUP_RESULTr, OEMA_ENTRY_FOUNDf, fld_val, reg_buffer_64, 50, exit);
  *is_found = SOC_SAND_NUM2BOOL(COMPILER_64_LO(fld_val));

  if (*is_found) {
      ARAD_FLD_FROM_REG64(PPDB_A_OEMA_DIAGNOSTICS_LOOKUP_RESULTr, OEMA_ENTRY_PAYLOADf, fld_val, reg_buffer_64, 60, exit);
      data_out[0] = COMPILER_64_LO(fld_val);
      data_out[1] = COMPILER_64_HI(fld_val);
      _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(data_out, (*oem1_payload));
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " Getting entry to OEM1: key %d payload: %d\n\r"), *key, COMPILER_64_LO(fld_val)));
  }
  else {
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " Getting entry to OEM1: key %d not found\n\r"), *key));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_entry_get_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem1_entry_delete_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY       *oem1_key
  )
{

  uint32  res;
  uint32  reg_val;
  uint32  entry_buffer[2];
  uint32  key[1];
#if SOC_DPP_IS_EM_HW_ENABLE
  uint32  failure, fail_valid_val, fail_key_val, fail_reason_val;
#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));
  reg_val = 0; /*delete*/
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_TYPEf, &reg_val);
  reg_val = 0; /*STAMP*/
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_STAMPf, &reg_val);
  _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY((*oem1_key), *key);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Deleting entry to OEM1: key %d \n\r"), *key));
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_KEYf, key);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_remove_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMA_BASE,
              key,
              ARAD_CHIP_SIM_OEMA_KEY
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMA_INTERRUPT_REGISTER_ONEr,
                     REG_PORT_ANY, 0, OEMA_MANAGEMENT_COMPLETEDf, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_VALIDf);

  if (fail_valid_val) {
      /*
       *    Failure indication is on.
       *  Check if it is on the last operation
       */
      fail_key_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_REASONf);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 1);

  }

#endif
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_entry_delete_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem2_entry_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY     * oem2_key,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD * oem2_payload
  )
{
  uint32  res;
  uint32  entry_buffer[2];
  uint32  reg_val; 
  uint32  key[1];
  uint32  payload[1];
  uint8  found;
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  failure, fail_valid_val, fail_reason_val64 , fail_key_val64 ;
  uint32  fail_reason_val , fail_key_val;

#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);

  _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY((*oem2_key), *key);
  _ARAD_PP_OAM_OEM2_PAYLOAD_STRUCT_TO_PAYLOAD((*oem2_payload), *payload);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OEM2: key %d payload: %d\n\r"), *key, *payload));

  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));
  reg_val = 1; /*insert*/
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_TYPEf, &reg_val);
  reg_val = 0; /*STAMP*/
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_STAMPf, &reg_val);
  reg_val = 0; /*SELF*/
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_SELFf, &reg_val);
  reg_val = 0;
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_RESERVED_LOWf, &reg_val);
  reg_val = 3;/*AGE_STATUS*/
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_AGE_STATUSf, &reg_val);
  reg_val = *key;
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_KEYf, &reg_val);
  reg_val = *payload; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_PAYLOADf, &reg_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_add_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMB_BASE,
              key,
              ARAD_CHIP_SIM_OEMB_KEY,
              payload,/* payload_data */
              ARAD_CHIP_SIM_OEMB_PAYLOAD,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMB_INTERRUPT_REGISTER_ONEr,
          REG_PORT_ANY,
          0,
          OEMB_MANAGEMENT_COMPLETEDf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_VALIDf);

  if (COMPILER_64_LO(fail_valid_val)){
      /*
       *    Failure indication is on.
       *  Check if it is on the last operation
       */       
      fail_key_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_REASONf);
      fail_key_val = COMPILER_64_LO(fail_key_val64);
      fail_reason_val = COMPILER_64_LO(fail_reason_val64);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 2);
  }
#endif
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem2_entry_set_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem2_entry_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
    SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
    SOC_SAND_OUT  uint8                                      *is_found
  )
{

  uint32  res;
  uint32  key[1];
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  uint32  data_out[1];
#endif
  uint32  reg_buffer;
  uint32  fld_val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY((*oem2_key), *key);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMB_DIAGNOSTICS_KEYr, REG_PORT_ANY, 0, OEMB_DIAGNOSTICS_KEYf, *key));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMB_DIAGNOSTICSr, REG_PORT_ANY, 0, OEMB_DIAGNOSTICS_LOOKUPf,  1));

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit) || ((SOC_IS_DONE_INIT(unit)) && (BCM_UNIT_DO_HW_READ_WRITE(unit))))
  {
  /* HW Log is on - get the value from the sw db, and exit. No need to proceed. */

      res = chip_sim_exact_match_entry_get_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMB_BASE,
              key,
              ARAD_CHIP_SIM_OEMB_KEY,
              data_out,
              ARAD_CHIP_SIM_OEMB_PAYLOAD,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      if (*is_found) {
          _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(*data_out, (*oem2_payload));
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEMq: key %d payload: %d\n\r"), *key, *data_out));
      }
      else {
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEM2: key %d not found\n\r"), *key));
      }

      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
#endif
/*#else*/
   res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMB_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          OEMB_DIAGNOSTICS_LOOKUPf,
          0
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_A_OEMB_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_buffer)); 

  ARAD_FLD_FROM_REG(PPDB_A_OEMB_DIAGNOSTICS_LOOKUP_RESULTr, OEMB_ENTRY_FOUNDf, fld_val, reg_buffer, 50, exit);
  *is_found = SOC_SAND_NUM2BOOL(fld_val);

  if (*is_found) {
      ARAD_FLD_FROM_REG(PPDB_A_OEMB_DIAGNOSTICS_LOOKUP_RESULTr, OEMB_ENTRY_PAYLOADf, fld_val, reg_buffer, 60, exit);
      _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(fld_val, (*oem2_payload));
  }
/*#endif*/
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem2_entry_get_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem2_entry_delete_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key
  )
{

  uint32  res;
  uint32  reg_val;
  uint32  entry_buffer[2];
  uint32  key[1];
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  fail_valid_val, fail_key_val64, fail_reason_val64;
  uint64  failure;
  uint32  fail_reason_val ,fail_key_val;

#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));
  reg_val = 0; /*delete*/
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_TYPEf, &reg_val);
  reg_val = 0; /*STAMP*/
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_STAMPf, &reg_val);
  _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY((*oem2_key), *key);
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_KEYf, key);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_remove_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMB_BASE,
              key,
              ARAD_CHIP_SIM_OEMB_KEY
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMB_INTERRUPT_REGISTER_ONEr,
                     REG_PORT_ANY, 0, OEMB_MANAGEMENT_COMPLETEDf, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_VALIDf);

  if (COMPILER_64_LO(fail_valid_val)) {
      /*
       *    Failure indication is on.
       *  Check if it is on the last operation
       */
 
      fail_key_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_REASONf);
      fail_key_val = COMPILER_64_LO(fail_key_val64);
      fail_reason_val = COMPILER_64_LO(fail_reason_val64);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 2); 
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem2_entry_delete_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map,
    SOC_SAND_IN  uint8                            is_bfd
  )
{
    uint32 i;
    uint32  res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_VERIFY);

    ARAD_STRUCT_VERIFY(SOC_PPD_OAM_MEP_PROFILE_DATA, &profile_data->mep_profile_data, 10, exit);
    ARAD_STRUCT_VERIFY(SOC_PPD_OAM_MEP_PROFILE_DATA, &profile_data->mip_profile_data, 20, exit);
    if (!is_bfd) {
        for (i=0; i<ARAD_PP_NOF_TRAP_CODES; i++) {
            SOC_SAND_ERR_IF_ABOVE_MAX(cpu_trap_code_to_mirror_profile_map[i], DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR, 30, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify()", 0, 0);
}

uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map,
    SOC_SAND_IN  uint8                            is_bfd
  )
{
  uint32 res;
  uint32 mep_type;
  uint32 is_my_cfm_mac;
  uint32 mip_type;
  uint32 opcode;
  uint8  internal_opcode;
  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key;
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD   oam_payload;
  uint32 oam_trap_code;
  uint32 meter_disable_lcl[1];
  _oam_oam_a_b_table_buffer_t oama_buffer;

  uint32 nof_mp_types_to_set = 1;

    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_UNSAFE);

  res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit,&oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  /* Clear default profile */
  if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && (!profile_data->is_default)) {
      res = arad_pp_oam_classifier_profile_reset(unit, ARAD_PP_OAM_PROFILE_DEFAULT, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

      SOC_SAND_EXIT_NO_ERROR;
  }

  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

  /* 
   * Add default profile handling - OAM1 entries
   */

  /* Active : trap to destination */
  oam1_key.inject = 0;
  oam1_key.is_bfd = is_bfd;
  oam1_key.mp_profile = classifier_mep_entry->non_acc_profile;
  oam1_key.ingress = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)==0);

  oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
  oam_payload.snoop_strength = 0;
  oam_payload.forwarding_strength = 7;
  oam_payload.forward_disable = 1;
  oam_payload.up_map = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);

  if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_ACTIVE_MATCH;
  }
  else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
      oam1_key.mp_type = SOC_PPD_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
  }
  else { /* Arad_B0 and below mode */
      oam1_key.mp_profile = ARAD_PP_OAM_PROFILE_DEFAULT;
      oam1_key.ingress = 1; /* Do not support default upmep */
      nof_mp_types_to_set = SOC_PPD_OAM_MP_TYPE_COUNT;
  }

  for (mep_type = 0; mep_type <nof_mp_types_to_set; mep_type++) {
      oam1_key.mep_type = mep_type;
      for (mip_type = 0; mip_type <nof_mp_types_to_set; mip_type++) {
          oam1_key.mip_type = mip_type;
          if (is_bfd) {
              oam1_key.my_cfm_mac = 0;
              oam1_key.opcode = 0;
              res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
              _ARAD_PP_OAM_SET_COUNTER_DISABLE(SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM, internal_opcode, (&oam_payload), profile_data->mep_profile_data, 0);
              *meter_disable_lcl = profile_data->mip_profile_data.meter_disable;
              oam_payload.meter_disable = (SHR_BITGET(meter_disable_lcl, internal_opcode) != 0);
              _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode], oam_payload.cpu_trap_code);

              oam_payload.mirror_profile = 0;
              res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          }
          else {
              for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
                  oam1_key.my_cfm_mac = is_my_cfm_mac;
                  for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT;  opcode++) {
                      res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                      oam1_key.opcode = internal_opcode;
                      _ARAD_PP_OAM_SET_COUNTER_DISABLE(opcode, internal_opcode, (&oam_payload), profile_data->mep_profile_data, 0);
                      *meter_disable_lcl = profile_data->mip_profile_data.meter_disable; 
                      oam_payload.meter_disable = (SHR_BITGET(meter_disable_lcl, internal_opcode) != 0);
                      if (is_my_cfm_mac) {
                          _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode], oam_payload.cpu_trap_code);
                          oam_payload.mirror_profile = cpu_trap_code_to_mirror_profile_map[profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode]];
                      }
                      else {
                          _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode], oam_payload.cpu_trap_code);
                          oam_payload.mirror_profile = cpu_trap_code_to_mirror_profile_map[profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode]];
                      }
                      res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                  }
              }
          }
      }
  }

  /* Above : forward */
  oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_ABOVE_ALL;
  oam1_key.mp_type = SOC_PPD_OAM_MP_TYPE_ABOVE_PLUS;
  oam1_key.mep_type = SOC_PPD_OAM_MP_TYPE_ABOVE;
  oam1_key.mip_type = SOC_PPD_OAM_MP_TYPE_ABOVE;

  res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 281, exit);
  oam_payload.cpu_trap_code = oam_trap_code;
  oam_payload.forward_disable = 0;
  oam_payload.mirror_profile = 0;

  for (is_my_cfm_mac = 0; is_my_cfm_mac <= 1; is_my_cfm_mac++) {
      oam1_key.my_cfm_mac = is_my_cfm_mac;
      for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT;  opcode++) {
          res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
          oam1_key.opcode = internal_opcode;
          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

  res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit); 

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe()", 0, 0);
}


/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(
    SOC_SAND_IN  int                                              unit,
    SOC_SAND_IN  uint32                                           opcode,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY                 *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA                     *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map,
    SOC_SAND_IN  uint8                                            is_unicast,
    SOC_SAND_INOUT  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD      *oam_payload
  )
{
    uint32 res;
    uint32 oam_trap_code;
    SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;
    uint32 oam_trap_code_level_err;
    uint8 internal_opcode;
    uint8 ccm_subtype = (profile_data->is_piggybacked)? _ARAD_PP_OAM_SUBTYPE_LM: _ARAD_PP_OAM_SUBTYPE_CCM;
    uint32 meter_disable_lcl[1];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

    *meter_disable_lcl = profile_data->mep_profile_data.meter_disable;
    oam_payload->meter_disable = (SHR_BITGET(meter_disable_lcl, internal_opcode) != 0);
    oam_payload->forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
    oam_payload->up_map = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);

    switch (opcode) {
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM:
        oam_payload->sub_type = ccm_subtype;
        break;
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR:
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM:
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM:
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR:
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_LM;
        break;
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM:
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR:
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM:
        oam_payload->sub_type = profile_data->is_1588 ? _ARAD_PP_OAM_SUBTYPE_DM_1588 : _ARAD_PP_OAM_SUBTYPE_DM_NTP;
        break;
    case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM:
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK;
        break;
    default:
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE; 
    }

    /* The destination trap might not be init. In this case trap to CPU */
    oam_trap_code = is_unicast ? profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] :
                                 profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
    if (oam_trap_code > 0) {
        _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,oam_trap_code, oam_payload->cpu_trap_code); 
    }
    else {
        res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_TRAP_CODE_TRAP_TO_CPU, &oam_trap_code);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
        oam_payload->cpu_trap_code = oam_trap_code;
    }

    if ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM  || opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR)
        && oam_trap_code == SOC_PPC_TRAP_CODE_OAM_OAMP_MIRROR) {
        /*      The egress PRGE should apply the CCM program for LBM/LBR packets when they are sent to the oamp (up meps)*/
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_CCM;
    }

    oam_payload->mirror_profile = cpu_trap_code_to_mirror_profile_map[oam_trap_code];
    res = soc_ppd_trap_snoop_profile_info_get(unit, oam_trap_code, &snoop_profile_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
    if (!oam_payload->up_map && (snoop_profile_info.snoop_cmnd > 0)) { /*ingress snooping*/
        oam_payload->snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
    }

    _ARAD_PP_OAM_SET_COUNTER_DISABLE(opcode, internal_opcode, oam_payload, profile_data->mep_profile_data, profile_data->is_piggybacked);

    res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL, &oam_trap_code_level_err);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);

    /* Disable counter if error level */
    if (oam_trap_code == oam_trap_code_level_err) {
        oam_payload->counter_disable = 1;
    }

    if ((oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP) || (oam_payload->snoop_strength > 0)) {
        /* In FWD and snoop dont do fwd disable */
        oam_payload->forward_disable = 0;
    }
    else {
        oam_payload->forward_disable = 1;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_mep_match_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN uint8                         is_server,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data_acc,
    SOC_SAND_IN  SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map
  )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_VERIFY);

    ARAD_STRUCT_VERIFY(SOC_PPD_OAM_MEP_PROFILE_DATA, &profile_data->mep_profile_data, 10, exit);
    ARAD_STRUCT_VERIFY(SOC_PPD_OAM_MEP_PROFILE_DATA, &profile_data->mip_profile_data, 15, exit);
    if (((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) && profile_data_acc) {
        ARAD_STRUCT_VERIFY(SOC_PPD_OAM_MEP_PROFILE_DATA, &profile_data_acc->mep_profile_data, 20, exit);
    }
    res = SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res , 30, exit);
    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
    }
    else {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_verify()", 0, 0);
}

uint32
  arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN uint8                         is_server,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data_acc,
    SOC_SAND_IN  SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map
  )
{
  uint32 res;
  uint32 mp_type;
  uint32 mep_type;
  uint32 is_my_cfm_mac;
  uint32 mip_type;
  uint32 opcode;
  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key;
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD   oam_payload;
  SOC_PPD_OAM_CLASSIFIER_OAM2_ENTRY_KEY       oam2_key;
  uint32 oam_trap_code;
  uint8 oam_mirror_profile[4];  /* Stored as 8 uint8's on uint32's*/
  uint8 is_accelerated, is_upmep;
  SOC_PPD_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;
  uint8 internal_opcode;
  uint8 ccm_subtype = (profile_data_acc && profile_data_acc->is_piggybacked)? _ARAD_PP_OAM_SUBTYPE_LM: _ARAD_PP_OAM_SUBTYPE_CCM;
  uint32 meter_disable_lcl[1];
  uint8 egress_snooping_advanced_mode;
  _oam_oam_a_b_table_buffer_t oama_buffer, oamb_buffer;
  uint8 is_passive;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_UNSAFE);
  oama_buffer.is_allocated = 0;
  oamb_buffer.is_allocated = 0;
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit); 

  res = arad_pp_oam_classifier_oam2_allocate_sw_buffer(unit, &oamb_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit); 


  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
  SOC_PPD_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&oam2_key);
  SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

  is_passive = (profile_data_acc == NULL);
  is_accelerated = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0  || is_server) ;
  is_upmep = is_passive^((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
  egress_snooping_advanced_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0);

  /*set OAM1 active & passive and OAM2 non-accelerated entries */

  /* BFD */
  if ((SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) && (!is_accelerated)) {
      oam1_key.inject = 0;
      oam1_key.is_bfd = 1;
      oam1_key.ingress = 1;
      oam1_key.mp_profile = is_passive ? classifier_mep_entry->non_acc_profile_passive : classifier_mep_entry->non_acc_profile;
      oam1_key.my_cfm_mac = 0;

      res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
      oam1_key.opcode = 0;
      oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
      oam_payload.up_map = is_upmep;
      oam_payload.meter_disable = 0;

      oam_trap_code = profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
      _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,oam_trap_code, oam_payload.cpu_trap_code);

       oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_CCM; 
       oam1_key.mip_type = SOC_PPD_OAM_MP_TYPE_ABOVE;

       if (SOC_IS_JERICHO(unit)) {
           oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_BFD;
           res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
           SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit); 
       } else {
           /*LOOP OVER ALL POSIBILITES OF mp_type */
           for (mp_type = 0; mp_type < SOC_PPD_OAM_MP_TYPE_COUNT_PLUS; mp_type++) {
               if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                   oam1_key.mep_type = mp_type; /*mp_type here is just a number, so can be used for ARAD as well*/
               } else {
                   oam1_key.mp_type = mp_type;
               }

               res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
               SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
           }
       }
  }

  /* OAM */
  else if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) {
      /*active*/
	  uint32 trap_code_level;
      oam1_key.inject = 0;
      oam1_key.is_bfd = SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type);
      oam1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
      oam1_key.mp_profile = is_passive ? classifier_mep_entry->non_acc_profile_passive : classifier_mep_entry->non_acc_profile;

      if (IS_INGRESS_WHEN_ACTIVE(is_upmep)) { /*ingress*/
          oam_payload.snoop_strength = 0;
          oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
      }
      oam_payload.up_map = is_upmep;

	  /* When the trap is set to level, subtype must be 0.  Get the level trap code.*/
	  res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, &trap_code_level, SOC_PPD_OAM_TRAP_ID_ERR_LEVEL);
	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit); 

      for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
          oam1_key.my_cfm_mac = is_my_cfm_mac;

          for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
              res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

              oam1_key.opcode = internal_opcode;
              for (mp_type = 0; mp_type < SOC_PPD_OAM_MP_TYPE_COUNT_PLUS; mp_type++) {
                  oam_payload.mirror_enable = 1; /* Jericho only. Relavant for egress entries.*/
                  oam_payload.forward_disable = 1; /* default */
                  oam_payload.counter_disable = 1; /* default */
                  /* 3 modes to handle: Jericho , Arad, Arad+ ("advanced mode"). In some cases these are handled separatly and in others not.
                     */
                  switch (mp_type) {
                  case SOC_PPD_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS: /* Mep active match */
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) { /* Arad mode*/
                          /* Only possibility of active match - mep_type=MATCH, mip_type=BELOW/ABOVE */
                          oam1_key.mep_type = SOC_PPD_OAM_MP_TYPE_MATCH;
                          for (oam1_key.mip_type = SOC_PPD_OAM_MP_TYPE_ABOVE; oam1_key.mip_type <= SOC_PPD_OAM_MP_TYPE_BELOW; oam1_key.mip_type++) {
                              res = arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(unit,
                                                                                               opcode,
                                                                                               classifier_mep_entry,
                                                                                               profile_data,
                                                                                               cpu_trap_code_to_mirror_profile_map,
                                                                                               is_my_cfm_mac,
                                                                                               &oam_payload
                                                                                               );
                              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); 
                              res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                              SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit); 
                          }
                      }
                      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                          /* Active match is actually the Passive side */
                          oam1_key.mp_type  = SOC_PPD_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
						  oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
																			 "oam_additional_FTMH_on_error_packets", 0) ?
							 _ARAD_PP_OAM_SUBTYPE_CCM : _ARAD_PP_OAM_SUBTYPE_DEFAULT;
						  oam_payload.snoop_strength = 0;
						  res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE, &oam_mirror_profile);
						  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit); 
                          oam_payload.mirror_profile = oam_mirror_profile[0];
                          res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE, &oam_trap_code);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                          oam_payload.cpu_trap_code = oam_trap_code;
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
                      }
                      else { /* Arad+ or Jericho*/
                          oam1_key.mp_type  = SOC_PPD_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
                          oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_ACTIVE_MATCH;
                          res = arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(unit,
                                                                                           opcode,
                                                                                           classifier_mep_entry,
                                                                                           profile_data,
                                                                                           cpu_trap_code_to_mirror_profile_map,
                                                                                           is_my_cfm_mac,
                                                                                           &oam_payload
                                                                                           ); 
                          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit); 
                      }

                      break;
                  case SOC_PPD_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS:
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          /*In ARAD this is mip match . Go over all the possibilities of active match - mip_type=MATCH, mep_type = above */
                          oam1_key.mip_type = SOC_PPD_OAM_MP_TYPE_MATCH;
                          oam1_key.mep_type  = SOC_PPD_OAM_MP_TYPE_ABOVE;
                      }
                      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                          oam1_key.mp_type  = SOC_PPD_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS;
                      }

                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) || (!profile_data->mp_type_passive_active_mix) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) { /* MIP */
                          oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_MIP_MATCH; /* Used only by Jericho.*/
    					  _ARAD_PP_OAM_SET_COUNTER_DISABLE(opcode, internal_opcode, (&oam_payload), profile_data->mip_profile_data, 0);
                          *meter_disable_lcl = profile_data->mip_profile_data.meter_disable;
                          oam_payload.meter_disable = (SHR_BITGET(meter_disable_lcl, internal_opcode) != 0);
                          oam_trap_code = is_my_cfm_mac ? profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] :
                                                          profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
						  /* When packets are to be forwarded or trapped with error-level subtype should be 0.  */
                          oam_payload.sub_type = (oam_trap_code==trap_code_level || oam_trap_code==SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP)?
								_ARAD_PP_OAM_SUBTYPE_DEFAULT: _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
                          _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,oam_trap_code, oam_payload.cpu_trap_code);
                          oam_payload.mirror_profile = cpu_trap_code_to_mirror_profile_map[oam_trap_code];
                          res = soc_ppd_trap_snoop_profile_info_get(unit, oam_trap_code, &snoop_profile_info);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
                          if (snoop_profile_info.snoop_cmnd > 0) {
                              oam_payload.snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
                              oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                          }
                          if ((oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP) || ((oam_payload.snoop_strength) && !egress_snooping_advanced_mode)) {
                              /* In FWD and snoop dont do fwd disable */
                              oam_payload.forward_disable = 0;
                          }
                          if (egress_snooping_advanced_mode && !oam1_key.ingress && oam_payload.snoop_strength) {
                              /* special subtype to for loading the egress snooping program*/
                              oam_payload.sub_type = ARAD_PP_OAM_SUBTYPE_EGRESS_SNOOP;
                          }
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

                          /* MIP is active from both sides - egress and ingress.
                             In ARAD Passive side should be added separately (egress) */
                          if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                              oam1_key.mp_profile = ARAD_PP_OAM_PROFILE_PASSIVE;
                          }
                          oam1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
                          if (egress_snooping_advanced_mode && !oam1_key.ingress && oam_payload.snoop_strength) {
                              /* special subtype to for loading the egress snooping program*/
                              oam_payload.sub_type = ARAD_PP_OAM_SUBTYPE_EGRESS_SNOOP;
                          }
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);
                          oam1_key.mp_profile = is_passive ? classifier_mep_entry->non_acc_profile_passive : classifier_mep_entry->non_acc_profile;
                          oam1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
                      }

                      if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (profile_data->mp_type_passive_active_mix))) {
                          /* Passive: For Jericho inserted static entries.
                             For Arad one entry for both (MIP , inserted above).
                             For Arad+ either passive or MIP, depending on the passive_active_mix*/
                          oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                             "oam_additional_FTMH_on_error_packets", 0) ?
                              _ARAD_PP_OAM_SUBTYPE_CCM : _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                          oam_payload.snoop_strength = 0;
                          oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_PASSIVE_MATCH; /* Used only by Jericho.*/
                          res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE, &oam_mirror_profile);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                          oam_payload.mirror_profile = oam_mirror_profile[0];
                          res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE, &oam_trap_code);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                          oam_payload.cpu_trap_code = oam_trap_code;

                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
                      }
                      break;
                  case SOC_PPD_OAM_MP_TYPE_BELOW_PLUS: /* Drop Level */
                      oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, 
                                                                         "oam_additional_FTMH_on_error_packets", 0)? 
                          _ARAD_PP_OAM_SUBTYPE_CCM:_ARAD_PP_OAM_SUBTYPE_DEFAULT;
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          /* Go over all the possibilities of Drop Level (passive has it's profile) - mep_type=BELOW/BETWEEN, mip_type = all */
                          for (oam1_key.mep_type = SOC_PPD_OAM_MP_TYPE_BELOW; oam1_key.mep_type < SOC_PPD_OAM_MP_TYPE_COUNT; oam1_key.mep_type++) {
                              for (oam1_key.mip_type = SOC_PPD_OAM_MP_TYPE_MATCH; oam1_key.mip_type < SOC_PPD_OAM_MP_TYPE_COUNT; oam1_key.mip_type++) {
                                  oam_payload.snoop_strength = 0;                                      
                                  res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_LEVEL, &oam_mirror_profile);
                                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                                  oam_payload.mirror_profile = oam_mirror_profile[0];
                                  res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL, &oam_trap_code);
                                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                                  oam_payload.cpu_trap_code = oam_trap_code;
                                  res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                                  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
                              }
                          }
                      }
                      else {  /* Arad+, Jericho*/
                          oam1_key.mp_type  = SOC_PPD_OAM_MP_TYPE_BELOW_PLUS; /* Used only by A+.*/
                          oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_BELLOW_HIGHEST_MEP; /* Used only by Jericho.*/
                          oam_payload.snoop_strength = 0;                                      
                          res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_LEVEL, &oam_mirror_profile);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                          oam_payload.mirror_profile = oam_mirror_profile[0];
                          res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_LEVEL, &oam_trap_code);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                          oam_payload.cpu_trap_code = oam_trap_code;
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
                      }
                      break;
                  case SOC_PPD_OAM_MP_TYPE_ABOVE_PLUS: /* Pass */
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          /* Go over all the possibilities of Pass - mep_type=above, mip_type = !match */
                          oam1_key.mep_type = SOC_PPD_OAM_MP_TYPE_ABOVE;
                          for (oam1_key.mip_type = SOC_PPD_OAM_MP_TYPE_ABOVE; oam1_key.mip_type < SOC_PPD_OAM_MP_TYPE_COUNT; oam1_key.mip_type++) {
                              /* Forward packet - change only counter and meter fields */
                              res = arad_pp_oam_classifier_oam1_entry_get_unsafe(unit, &oam1_key, &oam_payload);
                              SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);
                              oam_payload.counter_disable = (SHR_BITGET(&profile_data->mep_profile_data.counter_disable, internal_opcode) != 0);  
                              *meter_disable_lcl = profile_data->mep_profile_data.meter_disable;
                              oam_payload.meter_disable = (SHR_BITGET(meter_disable_lcl, internal_opcode) != 0);
                              res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                              SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);
                          }
                      }
                      else {  /* Arad+, Jericho mode*/
                          oam1_key.mp_type  = SOC_PPD_OAM_MP_TYPE_ABOVE_PLUS;
                          oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_ABOVE_ALL;
                          /* Forward packet */
                          oam_payload.forward_disable = 0;
                          oam_payload.mirror_enable =0;
                          oam_payload.snoop_strength = 0;
                          oam_payload.mirror_profile = 0;
                          oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                          res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                          oam_payload.cpu_trap_code = oam_trap_code;
                          oam_payload.counter_disable = (SHR_BITGET(&profile_data->mep_profile_data.counter_disable, internal_opcode) != 0);  
                          *meter_disable_lcl = profile_data->mep_profile_data.meter_disable; 
                          oam_payload.meter_disable = (SHR_BITGET(meter_disable_lcl, internal_opcode) != 0);
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);
                      }
                  }
              }
          }
      }
  }

  /* OAM2 - accelerated entries */
  if (!is_passive && (is_accelerated || 
       (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && 
        ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0) && 
        !SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)))) {

      SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

      /* active */
      oam2_key.inject = 0;
      oam2_key.is_bfd = SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type);
      oam2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
      oam2_key.mp_profile = classifier_mep_entry->acc_profile;
      if (oam2_key.is_bfd) {
          oam2_key.my_cfm_mac = 0;
          res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
          oam2_key.opcode = 0;
          oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
          oam_payload.up_map = is_upmep;
          oam_payload.meter_disable = 0;

          oam_trap_code = profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
          _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,oam_trap_code, oam_payload.cpu_trap_code);

           oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_CCM; 

          res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, &oamb_buffer);
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      }
      else { /* Eth OAM */
          for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
              oam2_key.my_cfm_mac = is_my_cfm_mac;
              for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
                  res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                  oam2_key.opcode = internal_opcode;

                  res = arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(unit,
                                                                                   opcode,
                                                                                   classifier_mep_entry,
                                                                                   profile_data_acc,
                                                                                   cpu_trap_code_to_mirror_profile_map,
                                                                                   is_my_cfm_mac,
                                                                                   &oam_payload
                                                                                   ); 

                  res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, &oamb_buffer);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
              }
          }
      }
  }


  /* "Inject" entries: Only relevant for UpMep Ethernet */
  if (is_upmep) {
      oam1_key.inject = 1;
      oam1_key.is_bfd = SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type);

      oam_payload.forward_disable = 0;
      res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 281, exit);
      oam_payload.cpu_trap_code = oam_trap_code;
      oam_payload.mirror_profile = 0;
      oam_payload.meter_disable = 1;
      oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
      oam_payload.snoop_strength = 0;
      oam_payload.up_map = is_upmep;
      oam1_key.ingress = TRUE; /* Injected bit is '1' only in ingress */

      for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
          oam1_key.my_cfm_mac = is_my_cfm_mac;
          for (opcode = 1; opcode <SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
              res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
              oam1_key.opcode = internal_opcode;

              for (mep_type = 0; mep_type < SOC_PPD_OAM_MP_TYPE_COUNT; mep_type++) {
                  for (mip_type = 0; mip_type < SOC_PPD_OAM_MP_TYPE_COUNT; mip_type++) {
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          oam1_key.mep_type = mep_type;
                          oam1_key.mip_type = mip_type;
                      }
                      else {
                          if (mep_type == SOC_PPD_OAM_MP_TYPE_MATCH) {
                              oam1_key.mp_type = SOC_PPD_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
                          }
                          else if (mip_type == SOC_PPD_OAM_MP_TYPE_MATCH) {
                              oam1_key.mp_type = SOC_PPD_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS;
                          }
                          else if (mip_type == SOC_PPD_OAM_MP_TYPE_ABOVE) {
                              oam1_key.mp_type = SOC_PPD_OAM_MP_TYPE_ABOVE_PLUS;
                          }
                          else if (mip_type == SOC_PPD_OAM_MP_TYPE_BELOW) {
                              oam1_key.mp_type = SOC_PPD_OAM_MP_TYPE_BELOW_PLUS;
                          }
                      }

                      switch (opcode) {
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM:
                          oam_payload.sub_type = ccm_subtype; 
                          break;
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR:
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM:
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR:
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM:
                          oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_LM; 
                          break;
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM:
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR:
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM:
                          oam_payload.sub_type = profile_data->is_1588 ? _ARAD_PP_OAM_SUBTYPE_DM_1588 : _ARAD_PP_OAM_SUBTYPE_DM_NTP;
                          break;
                      case SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM:
                          oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK;
                          break;
                      default:
                          oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE; 
                      }

                      if (is_accelerated && !is_passive) {
                          _ARAD_PP_OAM_SET_COUNTER_DISABLE(opcode, internal_opcode, (&oam_payload), profile_data_acc->mep_profile_data, profile_data_acc->is_piggybacked);
                      } else {
                          _ARAD_PP_OAM_SET_COUNTER_DISABLE(opcode, internal_opcode, (&oam_payload), profile_data->mep_profile_data, profile_data->is_piggybacked);
                      }
                          
                      /* Add two entries - one for passive and one for active */
                      oam1_key.mp_profile = (!is_passive) ? classifier_mep_entry->non_acc_profile_passive : classifier_mep_entry->non_acc_profile;
                      res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          oam1_key.mp_profile =  ARAD_PP_OAM_PROFILE_PASSIVE;
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
                      }

                      if ((oam1_key.mep_type==SOC_PPD_OAM_MP_TYPE_MATCH) && 
                          (is_accelerated || (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && 
                                              ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0))) && 
                          !is_passive) {
                          oam2_key.mp_profile = classifier_mep_entry->acc_profile;
                          oam2_key.ingress = oam1_key.ingress;
                          oam2_key.inject = oam1_key.inject;
                          oam2_key.is_bfd = oam1_key.is_bfd;
                          oam2_key.my_cfm_mac = oam1_key.my_cfm_mac;
                          oam2_key.opcode = oam1_key.opcode;
                          res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, &oamb_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);                                        
                      }
                  }
              }
          }
      }
  }
  
  res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  res = arad_pp_oam_classifier_oam2_set_hw_unsafe(unit, &oamb_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit); 

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oamb_buffer);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_unsafe()", 0, 0);
}
  
/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_VERIFY);

    res = SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
    }
    else { /* arad mode*/
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_profile_replace_verify()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  )
{
  uint32 res;
  uint8  found;
  uint8  passive_active_enable, passive_active_enable_passive;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_prev_payload, oem1_payload_active, oem1_payload_passive;
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       oem2_key;
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   oem2_payload;
  uint8 is_upmep;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_prev_payload);
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);

  if (SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) {
    LOG_ERROR(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " Only Ethernet and Y1731 type supported.\n\r")));
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  }

  is_upmep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);

  /* OEM1 - non accelerated entries */
  oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
  oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) * SOC_IS_JERICHO(unit); /* should always be 0 anyways*/

  if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)){
      res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile, &passive_active_enable);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile_passive, &passive_active_enable_passive);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      /* Check that passive_active_enable are the same for passive and active sides of same endpoint */
      if (passive_active_enable != passive_active_enable_passive) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  " Error: passive_active_enable should be the same for passive and active sides of same endpoint.\n\r")));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
      }

      /* 
       *  If MP_Type is changing -> is was nutral before -> only endpoints on one direction.
       *  If changing from up_down to mep_mip we should delete the passive indication for the meps (in mip_bitmap)
       *  If changing from mep_mip to up_down we should add passive indication for the meps.
       *  if update_mp_type is set to 0 then simply update the mp profile. 
       */ 
      if (passive_active_enable) {
          /* Changing from mep_mip to up_down */

          /* Active - only the mp_profile is to be updated. */
          oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          if (!found) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
          }
          oem1_payload_active.mp_profile =  classifier_mep_entry->non_acc_profile;
          oem1_payload_active.mep_bitmap = oem1_prev_payload.mep_bitmap;
          oem1_payload_active.mip_bitmap = oem1_prev_payload.mip_bitmap;
          oem1_payload_active.counter_ndx = oem1_prev_payload.counter_ndx;

          /* Passive - add passive indication for the meps. */
          oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          /* In case of mp_type update there might not be an existing entry */
          if (!found && !update_mp_type) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
          }
          oem1_payload_passive.mp_profile =  classifier_mep_entry->non_acc_profile_passive;
          oem1_payload_passive.mep_bitmap = found ? oem1_prev_payload.mep_bitmap : 0;
		  if (update_mp_type) {
              /* in this case active endpoints should be added as passive on mip bitmap.*/
              SET_MIP_BITMEP_FROM_MEP_BITMEP(oem1_payload_passive.mip_bitmap, oem1_payload_active.mep_bitmap);
		  } else {
              oem1_payload_passive.mip_bitmap = oem1_prev_payload.mip_bitmap;
          }
          oem1_payload_passive.counter_ndx = found ? oem1_prev_payload.counter_ndx : oem1_payload_active.counter_ndx;

          /* Active and passive entries set HW */
          oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_active);
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
          oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_passive);
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
      else {
          /* Changing from up_down to mep_mip */

          /* Active */
          oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
          if (!found) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
          }
          oem1_payload_active.mp_profile = classifier_mep_entry->non_acc_profile;
          oem1_payload_active.mip_bitmap = update_mp_type ? 0 : oem1_prev_payload.mip_bitmap;
          oem1_payload_active.mep_bitmap = oem1_prev_payload.mep_bitmap;
          oem1_payload_active.counter_ndx = oem1_prev_payload.counter_ndx;

          /* Passive */
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
          oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
          if (!found) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
          }
          oem1_payload_passive.mp_profile = classifier_mep_entry->non_acc_profile_passive;
          oem1_payload_passive.mep_bitmap = oem1_prev_payload.mep_bitmap;
          oem1_payload_passive.mip_bitmap = update_mp_type? 0 : oem1_prev_payload.mip_bitmap;
          oem1_payload_passive.counter_ndx = oem1_prev_payload.counter_ndx;

          /* Active and passive entries set HW */
          oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_active);
          SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_passive);
          SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
      }
  }
  else {
      /* ARAD/Jericho - Adding only active entries - passive entry already inserted and shouldn't change */

      oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);

      res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      if (!found) {
        LOG_ERROR(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
      }

      if (oem1_payload.mp_profile != classifier_mep_entry->non_acc_profile) {
          oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile;
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
  }

  /* OEM2 - accelerated entries */
  if (((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && 
        (classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) != 0) && (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type))) ||
        ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0)) {

        oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
        oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
        oem2_key.mdl = classifier_mep_entry->md_level;
        oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type); /* should always be 0 anyways*/

        res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        if (!found) {
              LOG_ERROR(BSL_LS_SOC_OAM,
                        (BSL_META_U(unit,
                                    " OEM entry not found.\n\r")));
                 SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 60, exit);
        }

        if (oem2_payload.mp_profile != classifier_mep_entry->acc_profile) {
              oem2_payload.mp_profile = classifier_mep_entry->acc_profile;
              res = arad_pp_oam_classifier_oem2_entry_set_unsafe(unit, &oem2_key, &oem2_payload);
              SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_profile_replace_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem_mep_add_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                           mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_VERIFY);

    if ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) {
        SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
    }
    res = SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
    }
    else { /* Arad mode*/
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_add_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_classifier_oem_mep_add_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                           mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            update
  )
{
  uint32 res;
  uint8  new_mip_bitmap = 0;
  uint8  new_mep_bitmap = 0;
  uint8  found;
  uint32 highest_mep;
  uint8 passive_active_enable;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_prev_payload;
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       oem2_key;
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   oem2_payload;
  uint8 is_upmep, is_mep;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_prev_payload);
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);

  is_upmep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
  is_mep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);

  if ((classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) && !update) {
      res = arad_pp_oam_my_cfm_mac_set_unsafe(unit, &classifier_mep_entry->dst_mac_address, classifier_mep_entry->port);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }

  /* Inserting entries to OEM1 in Ethernet or non-accelerated BFD */
  if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type) ||
      ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0)) { 

      /* 
       * Insert ingress and egress entries to O-EM1
       */
      oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
      oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) && SOC_IS_JERICHO(unit) ; 
      /*active*/ 
      oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);

      res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

      if (update) {
          oem1_payload = oem1_prev_payload;
      }

      oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile;

      /* Some quick checks:*/
      if (update && !found) {
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("MEP to update does not exist")));
      }
      if (SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type) && !update && found) { /* BFD */
          LOG_ERROR(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Error: MEP with given your discriminator exists.\n\r")));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR, 19, exit);
      }


      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
          /* Fill the O-EM1 separately*/
          res = soc_jer_pp_oam_oem1_mep_add(unit,classifier_mep_entry, &oem1_prev_payload,&oem1_payload,1,update);
          SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222, exit, res); 
      } else {
          if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) { /* OAM */
              if (!update) {
                  if (found) {
                      /* ARAD: If found a passive entry with mep on same level it should be deleted.
                                 If found a passive entry with mep on different level error should be generated.
                                  Otherwise, checking collisions and updating mep bitmap.
                           ARAD+: no need for special solution for 2 meps on same level */

                      /* ARAD+ new classifier: Active: dummy entries for counting purpose;
                       *                       Passive: insert into mep bitmep;
                       *                       MIPs: insert into mip bitmap
                       */
                      new_mep_bitmap = oem1_prev_payload.mep_bitmap;
                      new_mip_bitmap = oem1_prev_payload.mip_bitmap; 
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || 
                          (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && ((oem1_prev_payload.mp_profile != ARAD_PP_OAM_PROFILE_PASSIVE) || (!is_mep)))) {

                              /*checking that fields do not collide*/
                              if (oem1_prev_payload.mp_profile != classifier_mep_entry->non_acc_profile) {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Collision in mep parameters direction with existing endpoints on the lif (direction/TOD mode).\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR, 21, exit);
                              }
                              if ((is_mep) &&
                                  (classifier_mep_entry->counter != 0) && 
                                  (oem1_prev_payload.counter_ndx != 0) && 
                                  (oem1_prev_payload.counter_ndx != classifier_mep_entry->counter)) {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Counter existing on lif:%d. New counter: %d\n\r"), oem1_prev_payload.counter_ndx, classifier_mep_entry->counter));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR, 25, exit);
                              }
                      }
                      else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          if ((oem1_prev_payload.mep_bitmap & (~(1 << classifier_mep_entry->md_level))) != 0) { /* passive level is different from the inserted mep */
                              if (is_upmep) {
                                    LOG_WARN(BSL_LS_SOC_OAM,
                                             (BSL_META_U(unit,
                                                         " Adding upmep endpoint - behavior might be incosistant because downmep exists on different level.\n\r")));
                                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR, 25, exit);
                              }
                              else {
                                    LOG_WARN(BSL_LS_SOC_OAM,
                                             (BSL_META_U(unit,
                                                         " Adding downmep endpoint - behavior might be incosistant because upmep exists on different level.\n\r")));
                                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR, 25, exit);
                              }
                          }
                      }
                  }
                  if (is_mep) {
                      new_mep_bitmap += (1 << classifier_mep_entry->md_level);
                  }
                  else {
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          new_mip_bitmap += (1 << classifier_mep_entry->md_level);
                      }
                      else {
                          /* ARAD+ : Check if mip1 is busy */
                          if ((new_mip_bitmap & 7) == 0) {
                              new_mip_bitmap += classifier_mep_entry->md_level;
                          }
                          /* ARAD+ : Check if mip2 is busy */
                          else if ((new_mip_bitmap >> 3) == 0) {
                              new_mip_bitmap += classifier_mep_entry->md_level << 3;
                          }
                          else {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Two mips on same lif already exist\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 25, exit);
                          }
                      }
                  }

                  /*verify mip-mep level validity*/
                  highest_mep = 0;
                  while ((new_mep_bitmap >> (highest_mep+1)) != 0) {
                      highest_mep ++;
                  }
                  if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) { 
                      if (((new_mip_bitmap >> highest_mep) << highest_mep) != new_mip_bitmap) {
                          /*all mips should be above meps*/
                        LOG_ERROR(BSL_LS_SOC_OAM,
                                  (BSL_META_U(unit,
                                              " New endpoint in level:%d can not be inserted with existing endpoints.\n\r"), classifier_mep_entry->md_level));
                          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR, 30, exit);
                      }
                  }
                  oem1_payload.mep_bitmap = new_mep_bitmap;
                  oem1_payload.mip_bitmap = new_mip_bitmap;
              }

              if (is_mep) {
                  oem1_payload.counter_ndx = classifier_mep_entry->counter;
              }
              LOG_DEBUG(BSL_LS_SOC_OAM,
                       (BSL_META_U(unit,
                                   " Inserting counter to OEM1: lif: %d counter: %d\n\r"), oem1_key.oam_lif, oem1_payload.counter_ndx));
          }
      }

      res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /* Passive - only Ethernet and Y1731 */
      if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) {
          oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;

          /* Looking for entries on passive side */
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit); 

          if (update) {
              oem1_payload = oem1_prev_payload;
              oem1_payload.counter_ndx = classifier_mep_entry->counter;

              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); 
          } else {
              if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
                  res = soc_jer_pp_oam_oem1_mep_add(unit, classifier_mep_entry, &oem1_prev_payload, &oem1_payload, 0 /* passive */, update);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222, exit, res);

                  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); 
              } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                      /*ARAD+ : If mip, add the mip to the mip0,1. 
                        Else if mep_mip type -> do nothing
                        else add the mep to the mip0,1 */
                  oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile_passive;
                  res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile_passive, &passive_active_enable);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                  if ((!(is_mep)) || (passive_active_enable) || SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                      /* In case of new classifier we always add Passive entry */

                      if (found) {
                          new_mep_bitmap = oem1_prev_payload.mep_bitmap;
                          new_mip_bitmap = oem1_prev_payload.mip_bitmap;
                      }
                      else {
                          new_mep_bitmap = 0;
                          new_mip_bitmap = 0;
                      }

                      if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) || !is_mep) {
                          /* Check if mip1 is busy */
                          if ((new_mip_bitmap & 7) == 0) {
                              new_mip_bitmap += classifier_mep_entry->md_level;
                          }
                          /* Check if mip2 is busy */
                          else if ((new_mip_bitmap >> 3) == 0) {
                              new_mip_bitmap += classifier_mep_entry->md_level << 3;
                          }
                          else {
                              if (is_mep) {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Two meps on same lif already exist\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 25, exit);
                              }
                              else {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Two mips on same lif already exist\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 25, exit);
                              }
                          }
                      }
                      else {
                          /* In new classifier mode passive mep should be inserted into the mep bitmap */
                          new_mep_bitmap += (1 << classifier_mep_entry->md_level);
                      }

                      oem1_payload.mep_bitmap = new_mep_bitmap;
                      oem1_payload.mip_bitmap = new_mip_bitmap;

                      res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload); 
                      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                  }
              } else {
                  /*ARAD: If active entry found on same level delete it. 
                          If active entry found different level add warning.*/
                  oem1_payload.mp_profile = ARAD_PP_OAM_PROFILE_PASSIVE;
                  if (found && is_mep && (oem1_prev_payload.mep_bitmap & (~(1 << classifier_mep_entry->md_level))) != 0) {
                      LOG_WARN(BSL_LS_SOC_OAM,
                               (BSL_META_U(unit,
                                           " Skipping passive entry insert because of existing meps with different direction."
                                           "It is expected that another mep with opposite direction will be added next.\n\r")));
                  } else {
                      /* Update passive entry*/
                      res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                  }
              }
          }
      }
  }

  /* OEM2 - accelerated entries (no need to update) */
  if (((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && is_mep && (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type))) ||
        ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) ||
       SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(mep_index)) && !update) {
      /*insert one active entry to O-EM2*/
      oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
      oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
      oem2_key.mdl = classifier_mep_entry->md_level;
      oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type);
      if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0)) {
          /* In case of new classifier and non-accelerated endpoints , oam_id is the lif */
          oem2_payload.oam_id = classifier_mep_entry->lif & ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK;
      }
      else {
          oem2_payload.oam_id = mep_index & ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK;
      }
      oem2_payload.mp_profile = classifier_mep_entry->acc_profile;
      res = arad_pp_oam_classifier_oem2_entry_set_unsafe(unit, &oem2_key, &oem2_payload);
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_add_unsafe()", 0, 0);
}

/*********************************************************************
 * OAM default profiles add/remove
 *********************************************************************/

soc_error_t
  arad_pp_oam_classifier_default_profile_add(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                              update_action_only
  )
{
    int res;
    uint32 reg32;
    soc_reg_above_64_val_t reg_above_64;

    uint32 mep_bitmap;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    if (!update_action_only) {
        if (mep_index >= ARAD_PP_OAM_DEFAULT_EP_EGRESS_0) {
            /* Egress */

            /* Set counter index */
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, READ_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));
            soc_reg_above_64_field32_set(unit, IHB_OAM_DEFAULT_COUNTERSr, reg_above_64, EGRESS_DEFAULT_COUNTERf,
                                         classifier_mep_entry->counter);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 26, exit, WRITE_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));

            /* Set MEP bitmap */
            mep_bitmap = (1 << classifier_mep_entry->md_level) | ((1 << classifier_mep_entry->md_level) - 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, READ_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, &reg32));
            soc_reg_field_set(unit, IHB_OAM_DEFAULT_EGRESS_BITMAPSr, &reg32, EGRESS_DEFAULT_MEP_BITMAPf, mep_bitmap);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 56, exit, WRITE_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, reg32));

        } /* Egress end */
        else {
            /* Ingress */

            /* Set counter index */
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, READ_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));
            soc_reg_above_64_field32_set(unit, IHB_OAM_DEFAULT_COUNTERSr, reg_above_64, COUNTER_FOR_PROFILE_0f + (int)mep_index,
                                         classifier_mep_entry->counter);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 26, exit, WRITE_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));

            /* Set MEP bitmap */
            mep_bitmap = (1 << classifier_mep_entry->md_level) | ((1 << classifier_mep_entry->md_level) - 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHB_OAM_MEP_BITMAPr(unit, &reg32));
            soc_reg_field_set(unit, IHB_OAM_MEP_BITMAPr, &reg32, MEP_BITMAP_FOR_PROFILE_0f + (int)mep_index, mep_bitmap);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 51, exit, WRITE_IHB_OAM_MEP_BITMAPr(unit, reg32));
        }
    }

    res = arad_pp_oam_classifier_default_profile_action_set(unit,mep_index,classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_default_profile_add()", mep_index, 0);
}


soc_error_t
  arad_pp_oam_classifier_default_profile_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index
  ) {
    int res;
    uint32 reg32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (mep_index >= ARAD_PP_OAM_DEFAULT_EP_EGRESS_0) {
        /* Egress */

        /* Set MEP bitmap */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, READ_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_EGRESS_BITMAPSr, &reg32, EGRESS_DEFAULT_MEP_BITMAPf, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 56, exit, WRITE_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, reg32));

    }
    else {
        /* Ingress */

        /* Set MEP bitmap */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHB_OAM_MEP_BITMAPr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_MEP_BITMAPr, &reg32, MEP_BITMAP_FOR_PROFILE_0f + (int)mep_index, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 51, exit, WRITE_IHB_OAM_MEP_BITMAPr(unit, reg32));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_default_profile_remove()", 0, 0);
}


soc_error_t
  arad_pp_oam_classifier_default_profile_action_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
    int res;
    uint32 reg32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

    if (mep_index >= ARAD_PP_OAM_DEFAULT_EP_EGRESS_0) {
        /* Egress */

        /* Set MP-Profile (2 msb) */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_EGRESS_BITMAPSr, &reg32, EGRESS_DEFAULT_MIP_BITMAPf,
                          ((uint32)classifier_mep_entry->non_acc_profile) >> 2);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 36, exit, WRITE_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, reg32));

        /* Set MP-Profile (2 lsb) */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, READ_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_ACC_MEP_PROFILESr, &reg32, EGRESS_DEFAULT_ACC_MEP_PROFILEf,
                          ((uint32)classifier_mep_entry->non_acc_profile) & 0x3);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 46, exit, WRITE_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, reg32));

    } /* Egress end */
    else {
        /* Ingress */

        /* Set MP-Profile (2 msb) */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_OAM_MIP_BITMAPr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_MIP_BITMAPr, &reg32, MIP_BITMAP_FOR_PROFILE_0f + (int)mep_index,
                          ((uint32)classifier_mep_entry->non_acc_profile) >> 2);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_IHB_OAM_MIP_BITMAPr(unit, reg32));

        /* Set MP-Profile (2 lsb) */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_ACC_MEP_PROFILESr, &reg32, ACC_MEP_PROFILE_FOR_PROFILE_0f + (int)mep_index,
                          ((uint32)classifier_mep_entry->non_acc_profile) & 0x3);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 41, exit, WRITE_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, reg32));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_default_profile_action_set()", mep_index, 0);
}


/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_mep_delete_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                           mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_VERIFY);
  res = SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res , 10, exit);
  if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
  }
  else {
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPD_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_delete_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_classifier_mep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
  uint32  res;
  uint8  new_mip_bitmap = 0;
  uint8  new_mep_bitmap = 0;
  uint8  found;
  uint8  is_mep, is_upmep, passive_active_enable;
  uint32 new_mp_type_vector=0;

  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       oem2_key;
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   oem2_payload;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_UNSAFE);

  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);

  /* delete OEM1-2 entries */

  is_upmep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
  is_mep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);

  /* look for OEM2 entry */
  oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
  oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
  oem2_key.mdl = classifier_mep_entry->md_level; 
  oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type);
  res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* delete OEM2 entry if exists */
  if (found) {
      res = arad_pp_oam_classifier_oem2_entry_delete_unsafe(unit, &oem2_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) { 
          /* BFD - If OEM2 entry found, there is no OEM1 entry */
          SOC_SAND_EXIT_NO_ERROR;
      }
  }

  /* look for OEM1 entries */
  oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
  oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
  oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) * SOC_IS_JERICHO(unit);
  res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if (!found) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 50, exit); 
  }


  if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) { /* Ethernet and Y1731 */
      if (classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { /* Ethernet only use MY_MAC */ 
         res = arad_pp_oam_my_cfm_mac_delete_unsafe(unit, &classifier_mep_entry->dst_mac_address, classifier_mep_entry->port); 
         SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }

      /* In case of new classifier delete only MIPs in OEM1 */
      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
          if (JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, classifier_mep_entry->md_level) == 0) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
          }
          new_mp_type_vector = oem1_payload.mp_type_vector;
          JERICHO_PP_OAM_SET_MDL_MP_TYPE_VECTOR_BY_LEVEL(new_mp_type_vector, 0, classifier_mep_entry->md_level);
      } else {
          new_mep_bitmap = oem1_payload.mep_bitmap;
          new_mip_bitmap = oem1_payload.mip_bitmap;
          /*active*/
          if (is_mep) {
              new_mep_bitmap &= ~(1 << classifier_mep_entry->md_level);
          } else {
              if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                  new_mip_bitmap &= ~(1 << classifier_mep_entry->md_level);
              } else {
                  /* Check if this level is saved in mip1 */
                  if ((new_mip_bitmap & 7) == classifier_mep_entry->md_level) {
                      /* Empty mip1 */
                      new_mip_bitmap &= 0x38;
                  } else if ((new_mip_bitmap >> 3) == classifier_mep_entry->md_level) {
                      /* Chek it is saved in mip 2 and empty mip2 */
                      new_mip_bitmap &= 0x7;
                  } else {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
                  }
              }
          }
      }

      if ((new_mip_bitmap != 0) || (new_mep_bitmap != 0) || new_mp_type_vector) {
          /* update*/
          /*insert new entry with modified bitmap*/
          oem1_payload.mep_bitmap = new_mep_bitmap;
          oem1_payload.mip_bitmap = new_mip_bitmap;
          oem1_payload.mp_type_vector = new_mp_type_vector;
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      } else {
          /*delete the entry*/
          res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      }

      /*passive*/
      oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
      oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->passive_side_lif;
      oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) && SOC_IS_JERICHO(unit);
      res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
          if (!found) { 
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit); 
          }

          if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
              if (JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, classifier_mep_entry->md_level) == 0) {
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
              }
              new_mp_type_vector = oem1_payload.mp_type_vector;
              JERICHO_PP_OAM_SET_MDL_MP_TYPE_VECTOR_BY_LEVEL(new_mp_type_vector, 0, classifier_mep_entry->md_level);
          }

          if ((new_mip_bitmap!=0) || (new_mep_bitmap!=0) || new_mp_type_vector ) {
              /*update entry with modified bitmap*/
              /* Same bitmap used in Arad mode as in the active side.*/
              oem1_payload.mep_bitmap = new_mep_bitmap;
              oem1_payload.mip_bitmap = new_mip_bitmap;
              oem1_payload.mp_type_vector = new_mp_type_vector;
              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          } else {
              /*delete the entry*/
              res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
          }

      }
      else {
          /*ARAD+ : If mip, delete the mip from the mip0,1. 
                    Else if mep_mip type -> do nothing
                         else delete the mep from the mip0,1 */
          /* If new classifer, delete passive MEP */
          
          res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile, &passive_active_enable);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          if ((!is_mep) || (passive_active_enable) || SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
              new_mep_bitmap = oem1_payload.mep_bitmap;
              new_mip_bitmap = oem1_payload.mip_bitmap;

              if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && is_mep) {
                  new_mep_bitmap &= ~(1 << classifier_mep_entry->md_level);
              }
              else {
                  /* Check if this level is saved in mip1 */
                  if ((new_mip_bitmap & 7) == classifier_mep_entry->md_level) {
                      /* Empty mip1 */
                      new_mip_bitmap &= 0x38;
                  }
                  else if ((new_mip_bitmap >> 3) == classifier_mep_entry->md_level) {
                      /* Chek it is saved in mip 2 and empty mip2 */
                      new_mip_bitmap &= 0x7;
                  }
                  else {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
                  }
              }

              oem1_payload.mep_bitmap = new_mep_bitmap;
              oem1_payload.mip_bitmap = new_mip_bitmap;

              /* Either update the existing entry or delete it.*/
              if ((new_mip_bitmap != 0) || (new_mep_bitmap != 0)) {
                  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
              } else {
                  res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
              }

          }
      }
  } else {
      /* BFD: simply delete the entry*/
      res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_delete_unsafe()", mep_index, 0);
}



/*********************************************************************
*     Enter MEP DB entry according to type
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_set_internal_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  uint32                   short_name,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32  res;
  soc_mem_t mem;
  uint32  reg_val;
  soc_reg_above_64_val_t entry_above_64;
  uint32  inner_tpid, inner_vid_dei_pcp, outer_tpid, outer_vid_dei_pcp;
  uint32 eep_shift;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_INTERNAL_UNSAFE);

  switch (mep_db_entry->mep_type) {
  case SOC_PPD_OAM_MEP_TYPE_ETH_OAM:
      mem = OAMP_MEP_DBm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP:
      mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE:
      mem = OAMP_MEP_DB_Y_1731_ON_PWEm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
      mem = OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
      mem = OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE:
      mem = OAMP_MEP_DB_BFD_ON_PWEm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS:
      mem = OAMP_MEP_DB_BFD_ON_MPLSm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
      mem = OAMP_MEP_DB_BFD_CC_ON_MPLSTPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP: /* Do not support BFDCV_O_MPLSTP */
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  }
  
  SOC_REG_ABOVE_64_CLEAR(entry_above_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_mem_read(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));

  reg_val = mep_db_entry->mep_type;
  soc_mem_field_set(unit, mem, entry_above_64, MEP_TYPEf, &reg_val);
  if (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
      reg_val = mep_db_entry->priority; 
      soc_mem_field_set(unit, mem, entry_above_64, PRIORITYf, &reg_val);
  }
  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      reg_val = mep_db_entry->local_port; 
      soc_mem_field_set(unit, mem, entry_above_64, LOCAL_PORTf, &reg_val);
  }
  else {
      reg_val = mep_db_entry->system_port; 
      soc_mem_field_set(unit, mem, entry_above_64, DEST_SYS_PORT_AGRf, &reg_val);
  }

  eep_shift = SOC_IS_ARADPLUS_A0(unit) ? 1 : 0; /* Outlif is alyays even. Using it in ARAD+ to save bits */
  if ((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) ||
      (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
      (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) { /* Only relevant for Ethernet and Y1731 */

      reg_val = mep_db_entry->ccm_interval; 
      soc_mem_field_set(unit, mem, entry_above_64, CCM_INTERVALf, &reg_val);
      reg_val = mep_db_entry->mdl; 
      soc_mem_field_set(unit, mem, entry_above_64, MDLf, &reg_val);
      reg_val = mep_db_entry->icc_ndx; 
      soc_mem_field_set(unit, mem, entry_above_64, ICC_INDEXf, &reg_val);
      reg_val = short_name; 
      soc_mem_field_set(unit, mem, entry_above_64, MAIDf, &reg_val);
      reg_val = mep_db_entry->mep_id; 
      soc_mem_field_set(unit, mem, entry_above_64, MEP_IDf, &reg_val);
	  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
		  /* in Arad+ and above this bit is mostly controlled by the HW.*/
		  reg_val = mep_db_entry->rdi; 
		  soc_mem_field_set(unit, mem, entry_above_64, RDIf, &reg_val);
	  } else {
		  uint32 rdi=0;
		  reg_val = mep_db_entry->counter_pointer;
		  soc_mem_field_set(unit, mem, entry_above_64, COUNTER_POINTERf, &reg_val);
          reg_val = mep_db_entry->port_status_tlv_en;
          soc_mem_field_set(unit, mem, entry_above_64, PORT_STATUS_TLV_ENf, &reg_val);
          reg_val = mep_db_entry->port_status_tlv_val;
          soc_mem_field_set(unit, mem, entry_above_64, PORT_STATUS_TLV_VALf, &reg_val);
          reg_val = mep_db_entry->interface_status_tlv_control;
          soc_mem_field_set(unit, mem, entry_above_64, INTERFACE_STATUS_TLV_CODEf, &reg_val);

		  soc_mem_field_get(unit, mem, entry_above_64, RDIf, &rdi);


		  reg_val = mep_db_entry->rdi <<1 | (rdi &1); /* bit 0 is set by LOC, bit 1 may be set/unset by the user as well as incoming CCMs (punt profile dependent).*/
		  soc_mem_field_set(unit, mem, entry_above_64, RDIf, &reg_val);

		}

      if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { /* Only relevant for Ethernet */
          reg_val = mep_db_entry->is_upmep; 
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, UP_1_DOWN_0f, &reg_val);
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              reg_val = 7;
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, UP_PTCH_OPAQUE_PT_ATTRf, &reg_val);
          } else {
              /*jericho*/
              reg_val =0; /* Always use profile 0 (value 7, set in init).*/
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, UP_PTCH_OPAQUE_PT_ATTR_PROFILEf, &reg_val);
              /* SA gen profile: bits [0:7] represent the SA LSB, bit 8 represetns which profile is taken from OAMP_CCM_SA_MAC*/
              reg_val = mep_db_entry->src_mac_lsb + ((mep_db_entry->src_mac_msb_profile &0x1)<<8); 
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, SA_GEN_PROFILEf, &reg_val);
          }

          switch (mep_db_entry->tags_num) {
          case 0: 
              inner_tpid = 0;
              inner_vid_dei_pcp = 0;
              outer_tpid = 0;
              outer_vid_dei_pcp = 0;
              break;
          case 1:
              /* In case of single tag HW uses inner tpid fields instead of outer */
              inner_tpid = mep_db_entry->outer_tpid;
              inner_vid_dei_pcp = mep_db_entry->outer_vid_dei_pcp;
              outer_tpid = 0;
              outer_vid_dei_pcp = 0;
              break;
          case 2:
              inner_tpid = mep_db_entry->inner_tpid; 
              inner_vid_dei_pcp = mep_db_entry->inner_vid_dei_pcp; 
              outer_tpid = mep_db_entry->outer_tpid;
              outer_vid_dei_pcp = mep_db_entry->outer_vid_dei_pcp; 
              break;
          default:
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 82, exit);
          }
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, INNER_TPIDf, &inner_tpid);
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, INNER_VID_DEI_PCPf, &inner_vid_dei_pcp);
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, OUTER_TPIDf, &outer_tpid);
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, OUTER_VID_DEI_PCPf, &outer_vid_dei_pcp);
          reg_val = mep_db_entry->tags_num;
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, ENCAPSULATIONf, &reg_val);

          if (SOC_IS_ARADPLUS_A0(unit)) {
              if (mep_db_entry->is_11b_maid) {
                  /* TLV + 11BMAID + LSB of src MAC fix  (the latter is Up MEP only)*/
                  arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV, &reg_val);
                  soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              }
              else {
                  /* TLV  + LSB of src MAC fix  (the latter is Up MEP only)*/
                  ARAD_PP_OAMP_PE_PROGRAMS prog_used = mep_db_entry->is_upmep? ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC :
                       ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX;
                  arad_pp_oamp_pe_program_profile_get(unit, prog_used, &reg_val);
                  soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              }
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP){
          reg_val = mep_db_entry->egress_if >> eep_shift; 
          soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
          reg_val = mep_db_entry->label; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
      }
      else { /*SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE*/
          reg_val = mep_db_entry->egress_if >> eep_shift; 
          soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
          reg_val = mep_db_entry->label; 
          soc_mem_field_set(unit, mem, entry_above_64, PWE_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, PWE_PUSH_PROFILEf, &reg_val);
      }

      if (mep_db_entry->remote_recycle_port) {
          arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER,&reg_val);
          soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);


      }
  }
  else { /* BFD */
      reg_val = mep_db_entry->ccm_interval; 
      soc_mem_field_set(unit, mem, entry_above_64, TX_RATEf, &reg_val);
      reg_val = mep_db_entry->egress_if >> eep_shift; 
      soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
          reg_val = mep_db_entry->pbit; 
          soc_mem_field_set(unit, mem, entry_above_64, PBITf, &reg_val);
          reg_val = mep_db_entry->fbit; 
          soc_mem_field_set(unit, mem, entry_above_64, FBITf, &reg_val);
          reg_val = mep_db_entry->local_detect_mult; 
      }
      else {
          reg_val = mep_db_entry->diag_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, DIAG_PROFILEf, &reg_val);
          reg_val = mep_db_entry->flags_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, FLAGS_PROFILEf, &reg_val);
          reg_val = mep_db_entry->sta; 
          soc_mem_field_set(unit, mem, entry_above_64, STAf, &reg_val);
      }
      reg_val = mep_db_entry->local_detect_mult; 
      soc_mem_field_set(unit, mem, entry_above_64, DETECT_MULTf, &reg_val);
      reg_val = mep_db_entry->min_rx_interval_ptr; 
      soc_mem_field_set(unit, mem, entry_above_64, MIN_RX_INTERVAL_PTRf, &reg_val);
      reg_val = mep_db_entry->min_tx_interval_ptr; 
      soc_mem_field_set(unit, mem, entry_above_64, MIN_TX_INTERVAL_PTRf, &reg_val);

      if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) {
          if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
              reg_val = mep_db_entry->tunnel_is_mpls; 
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_set(unit, entry_above_64, TUNNEL_IS_MPLSf, &reg_val);
          }
          if (SOC_IS_JERICHO(unit)) {
              reg_val = mep_db_entry->dst_ip_add;
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_set(unit, entry_above_64, DST_IPf, &reg_val);
              reg_val = mep_db_entry->ip_subnet_len;
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_set(unit, entry_above_64, IP_SUBNET_LENf, &reg_val);
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
          reg_val = mep_db_entry->tos_ttl_profile; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_set(unit, entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
          reg_val = mep_db_entry->dst_ip_add; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_set(unit, entry_above_64, DST_IPf, &reg_val);
      }
      else if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE) {
          reg_val = mep_db_entry->label; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, PWE_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, PWE_PUSH_PROFILEf, &reg_val);

          if (SOC_IS_ARADPLUS((unit))) {
              reg_val = (mep_db_entry->bfd_pwe_router_alert)>0;
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, ROUTER_ALERTf, &reg_val);
              reg_val = (mep_db_entry->bfd_pwe_ach>0);
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, ACHf, &reg_val);
              /* In Jericho ACH_SEL should always be 0, only one ACH used.*/
              if (mep_db_entry->bfd_pwe_gal) {
                  /* bfd_pwe_gal should only be positive for Jericho.*/
                  reg_val = 1; 
                  soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, GALf, &reg_val); 
              }
          }
      }
      else if ((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS) || (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP)){
          reg_val = mep_db_entry->label; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
          if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS) {
              reg_val = mep_db_entry->tos_ttl_profile; 
              soc_mem_field_set(unit, mem, entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
          }
      }

      if (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP) {
          reg_val = mep_db_entry->remote_discr; 
          soc_mem_field_set(unit, mem, entry_above_64, YOUR_DISCf, &reg_val);
          if (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE){
              reg_val = mep_db_entry->src_ip_add_ptr; 
              soc_mem_field_set(unit, mem, entry_above_64, SRC_IP_PTRf, &reg_val);
          }
      }
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_mem_write(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_set_internal_unsafe()", mep_index, 0);
}

/*********************************************************************
*     Read MEP DB entry according to type
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   uint32                   mep_index,
    SOC_SAND_OUT  uint32                   *short_name,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32  res;
  soc_mem_t mem;
  uint32  reg_val;
  soc_reg_above_64_val_t entry_above_64;
  uint32 inner_tpid, outer_tpid, inner_vid_dei_pcp, outer_vid_dei_pcp;
  uint32 eep_shift;
  uint32 short_name_lcl[1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_INTERNAL_UNSAFE);

  mep_db_entry->mep_type = 0; /* not really needed. Just to avoid coverity defect */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_index, entry_above_64)); 
  soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, MEP_TYPEf, &mep_db_entry->mep_type);

  switch (mep_db_entry->mep_type) {
  case SOC_PPD_OAM_MEP_TYPE_ETH_OAM:
      mem = OAMP_MEP_DBm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP:
      mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE:
      mem = OAMP_MEP_DB_Y_1731_ON_PWEm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
      mem = OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
      mem = OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE:
      mem = OAMP_MEP_DB_BFD_ON_PWEm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS:
      mem = OAMP_MEP_DB_BFD_ON_MPLSm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
      mem = OAMP_MEP_DB_BFD_CC_ON_MPLSTPm;
      break;
  case SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP: /* Do not support BFDCV_O_MPLSTP */
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  }

  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      reg_val = 0; 
      soc_mem_field_get(unit, mem, entry_above_64, LOCAL_PORTf, &reg_val);
      mep_db_entry->local_port = reg_val;
  }
  else {
      reg_val = 0; 
      soc_mem_field_get(unit, mem, entry_above_64, DEST_SYS_PORT_AGRf, &reg_val);
      mep_db_entry->local_port = reg_val;
  }

  if (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, PRIORITYf, &reg_val);
      mep_db_entry->priority = (uint8)reg_val;
  }

  eep_shift = SOC_IS_ARADPLUS_A0(unit) ? 1 : 0;
  if ((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) ||
      (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
      (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) { /* Only relevant for Ethernet and Y1731 */

      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, CCM_INTERVALf, &reg_val);
      mep_db_entry->ccm_interval = (uint8)reg_val;
      soc_mem_field_get(unit, mem, entry_above_64, MAIDf, short_name_lcl);
      *short_name = *short_name_lcl;
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, MDLf, &reg_val);
      mep_db_entry->mdl = (uint8)reg_val; 
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, ICC_INDEXf, &reg_val);
      mep_db_entry->icc_ndx = (uint8)reg_val;
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, MEP_IDf, &reg_val);
      mep_db_entry->mep_id = (uint16)reg_val;
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, RDIf, &reg_val); 
      mep_db_entry->rdi = (uint8)reg_val;

      if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
          reg_val = 0;
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, UP_1_DOWN_0f, &reg_val);
          mep_db_entry->is_upmep = (uint8)reg_val;
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, INNER_TPIDf, &inner_tpid); 
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, INNER_VID_DEI_PCPf, &inner_vid_dei_pcp);
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, OUTER_TPIDf, &outer_tpid); 
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, OUTER_VID_DEI_PCPf, &outer_vid_dei_pcp);
          reg_val = 0;
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, ENCAPSULATIONf, &reg_val);
          mep_db_entry->tags_num = (uint8)reg_val;
          switch (mep_db_entry->tags_num) {
          case 0: 
              mep_db_entry->inner_tpid = 0;
              mep_db_entry->inner_vid_dei_pcp = 0;
              mep_db_entry->outer_tpid = 0;
              mep_db_entry->outer_vid_dei_pcp = 0;
              break;
          case 1:
              /* In case of single tag HW uses inner tpid fields instead of outer */
              mep_db_entry->outer_tpid = (uint8)inner_tpid;
              mep_db_entry->outer_vid_dei_pcp = (uint16)inner_vid_dei_pcp;
              mep_db_entry->inner_tpid = 0;
              mep_db_entry->inner_vid_dei_pcp = 0;
              break;
          case 2:
              mep_db_entry->inner_tpid = (uint8)inner_tpid; 
              mep_db_entry->inner_vid_dei_pcp = (uint16)inner_vid_dei_pcp; 
              mep_db_entry->outer_tpid = (uint8)outer_tpid;
              mep_db_entry->outer_vid_dei_pcp = (uint16)outer_vid_dei_pcp; 
              break;
          default:
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 82, exit);
          }

          if (SOC_IS_JERICHO(unit)) {
              uint32 sa_gen_profile;
              sa_gen_profile = soc_OAMP_MEP_DBm_field32_get(unit,&entry_above_64,SA_GEN_PROFILEf);
              /* SA gen profile: bits [0:7] represent the SA LSB, bit 8 represetns which profile is taken from OAMP_CCM_SA_MAC*/
              mep_db_entry->src_mac_msb_profile = (sa_gen_profile >> 8 ) & 0x1;
              mep_db_entry->src_mac_lsb = sa_gen_profile & 0xff;
          }

      }
      else if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP){
          reg_val = 0;        
          soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
          mep_db_entry->egress_if = reg_val << eep_shift;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          mep_db_entry->label = reg_val;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val;
      }
      else { /*SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE*/
          reg_val = 0;        
          soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
          mep_db_entry->egress_if = reg_val << eep_shift;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, PWE_LABELf, &reg_val);
          mep_db_entry->label = reg_val;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, PWE_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val;
      }

	  if (SOC_IS_ARADPLUS(unit)) {
		  reg_val = 0; 
		  soc_mem_field_get(unit, mem, entry_above_64, COUNTER_POINTERf, &reg_val);
		  mep_db_entry->counter_pointer = reg_val;

          reg_val = 0;
		  soc_mem_field_get(unit, mem, entry_above_64, PORT_STATUS_TLV_ENf, &reg_val);
		  mep_db_entry->port_status_tlv_en = reg_val;

		  reg_val = 0;
		  soc_mem_field_get(unit, mem, entry_above_64, PORT_STATUS_TLV_VALf, &reg_val);
		  mep_db_entry->port_status_tlv_val = reg_val;

		  reg_val = 0;
		  soc_mem_field_get(unit, mem, entry_above_64, INTERFACE_STATUS_TLV_CODEf, &reg_val);
		  mep_db_entry->interface_status_tlv_control = reg_val; 
	  }
  }
  else { /* BFD */
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, TX_RATEf, &reg_val);
      mep_db_entry->ccm_interval = (uint8)reg_val;
      reg_val = 0;        
      soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
      mep_db_entry->egress_if = reg_val << eep_shift;
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, PBITf, &reg_val);
          mep_db_entry->pbit = reg_val; 
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, FBITf, &reg_val);
          mep_db_entry->fbit = reg_val; 
      }
      else {
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, DIAG_PROFILEf, &reg_val);
          mep_db_entry->diag_profile = reg_val;
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, FLAGS_PROFILEf, &reg_val);
          mep_db_entry->flags_profile = reg_val;
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, STAf, &reg_val);
          mep_db_entry->sta = reg_val;
      }
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, DETECT_MULTf, &reg_val);
      mep_db_entry->local_detect_mult = reg_val; 
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, MIN_RX_INTERVAL_PTRf, &reg_val);
      mep_db_entry->min_rx_interval_ptr = reg_val; 
      reg_val = 0; 
      soc_mem_field_get(unit, mem, entry_above_64, MIN_TX_INTERVAL_PTRf, &reg_val);
      mep_db_entry->min_tx_interval_ptr = reg_val;
      if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) {
          if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
              reg_val = 0; 
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, TUNNEL_IS_MPLSf, &reg_val);
              mep_db_entry->tunnel_is_mpls = reg_val;
              if (SOC_IS_JERICHO(unit)) {
                  soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, DST_IPf, &reg_val);
                  mep_db_entry->dst_ip_add = reg_val;
                  soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, IP_SUBNET_LENf, &reg_val);
                   mep_db_entry->ip_subnet_len = reg_val ;
              }

          }
      }
      else if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
          mep_db_entry->tos_ttl_profile = reg_val;
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, DST_IPf, &reg_val);
          mep_db_entry->dst_ip_add = reg_val;
      }
      else if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE){
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, PWE_LABELf, &reg_val);
          mep_db_entry->label = reg_val;
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, PWE_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val;
          if (SOC_IS_JERICHO(unit)) {
              /* Some of these fields are also available for A+, but are only applicable for Jericho anyways.*/
              reg_val = 0; 
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, ACHf, &reg_val);
              mep_db_entry->bfd_pwe_ach = reg_val;
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, ROUTER_ALERTf, &reg_val);
              mep_db_entry->bfd_pwe_router_alert = reg_val;
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, GALf, &reg_val);
              mep_db_entry->bfd_pwe_gal = reg_val;
          }
      }
      else if ((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS) || (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP)){
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          mep_db_entry->label = reg_val; 
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val; 
          if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS) {
              reg_val = 0; 
              soc_mem_field_get(unit, mem, entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
              mep_db_entry->tos_ttl_profile = reg_val;

          }
      }

      if (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP) {
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, YOUR_DISCf, &reg_val);
          mep_db_entry->remote_discr = reg_val;
          if (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE){
              reg_val = 0;
              soc_mem_field_get(unit, mem, entry_above_64, SRC_IP_PTRf, &reg_val);
              mep_db_entry->src_ip_add_ptr = reg_val; 
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe()", mep_index, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_MA_NAME      name
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mep_db_entry->mdl, _ARAD_PP_OAM_MAX_MD_LEVEL, ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_set_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_oamp_mep_db_entry_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_MA_NAME      name
  )
{
  uint32  res;
  uint32  short_name = 0;
  uint32  mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  uint32  entry;
  uint32  reg_val;
  uint64  msb_mac, msb_mac_existing;
  uint32  entry_buffer[2];

  SOC_PPD_OAM_ICC_MAP_DATA  icc_map_data;
  soc_reg_above_64_val_t  umc_name_above_64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  COMPILER_64_ZERO(msb_mac);
  COMPILER_64_ZERO(msb_mac_existing);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

 LOG_DEBUG(BSL_LS_SOC_OAM,
           (BSL_META_U(unit,
                       " Inserting entry to MEPDB: key %d\n\r"), mep_index));
   
  if ((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) ||
      (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
      (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) { /* Only relavant for Ethernet and Y1731 */
  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */    
      res = soc_sand_pp_mac_address_struct_to_long(&mep_db_entry->src_mac_address, mac_add_U32);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM && SOC_IS_ARADPLUS_AND_BELOW(unit) ) {
          /* SA MSB handled separatly in Jericho*/
		  _ARAD_PP_OAM_MAC_ADD_MSB_GET(mac_add_U32, msb_mac);

		  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf, &msb_mac_existing));

		  if (COMPILER_64_IS_ZERO(msb_mac_existing)) { 
			  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf,  msb_mac));
		  }
		  else if (COMPILER_64_NE(msb_mac_existing,msb_mac)) {
				  /* All MEPs in a device must share same SA[47:8] suffix */
				  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR, 35, exit); 
		  }
	  }

      /*name handling*/
      SOC_REG_ABOVE_64_CLEAR(umc_name_above_64);
      _arad_pp_oam_ma_name_divide(name, _ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry->icc_ndx), umc_name_above_64, &icc_map_data, &short_name);
      if (!_ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry->icc_ndx)) {
          if (allocate_icc_ndx) {
              arad_pp_oam_icc_map_register_set_unsafe(unit, mep_db_entry->icc_ndx, &icc_map_data);
          }
          /*add UMC entry*/
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
          soc_OAMP_UMC_TABLEm_field_set(unit, entry_buffer, UMCf, umc_name_above_64);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
      }
  }

  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      if ( (SOC_IS_ARAD_B0_AND_ABOVE(unit) && !mep_db_entry->is_upmep) || SOC_PPD_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type) ||
           mep_db_entry->remote_recycle_port) {/* remote_recycle_port should be set only in the case of server up mep. In which case store it in the table.*/
          /*local to system port table: Used only by down MEPs, BFD. In Arad A0 the system port is taken directly from the local port field in the MEP_DB. */
          reg_val = mep_db_entry->remote_recycle_port? mep_db_entry->remote_recycle_port : mep_db_entry->system_port ; 
          if (SOC_IS_ARADPLUS(unit)) { 
              entry=0;
              soc_OAMP_MEM_20000m_field32_set(unit, &entry, ITEM_0_15f, reg_val);  
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_OAMP_MEM_20000m(unit, MEM_BLOCK_ANY,  mep_db_entry->local_port, &entry));
          } else {
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_LOCAL_PORT_2_SYSTEM_PORTm(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
              soc_OAMP_LOCAL_PORT_2_SYSTEM_PORTm_field_set(unit, &entry, SYS_PORTf, &reg_val);
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_OAMP_LOCAL_PORT_2_SYSTEM_PORTm(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
          }
      }
  }


  /*enter MEP DB entry*/ 
  res =   arad_pp_oam_oamp_mep_db_entry_set_internal_unsafe(unit, mep_index,short_name, mep_db_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_set_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_get_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_oamp_mep_db_entry_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32  res = 0;
  uint32  mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S]={0};
  uint32  entry = 0;
  uint32  short_name;
  uint64  msb_mac;
  uint32  entry_buffer[2];

  SOC_PPD_OAM_ICC_MAP_DATA  icc_map_data;
  soc_reg_above_64_val_t  umc_name_above_64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  COMPILER_64_ZERO(msb_mac);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);
   
  /*read MEP DB entry*/  
  res = arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(unit, mep_index,&short_name, mep_db_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type)) { /* Only relavant for Ethernet and Y1731 */
       
      if (!_ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry->icc_ndx)) {
          res = arad_pp_oam_icc_map_register_get_unsafe(unit, mep_db_entry->icc_ndx, &icc_map_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          /*add UMC entry*/
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
          soc_OAMP_UMC_TABLEm_field_get(unit, entry_buffer, UMCf, umc_name_above_64);
      }

	  if (mep_db_entry->mep_type ==SOC_PPD_OAM_MEP_TYPE_ETH_OAM ) {
          /* SA MSB handled separatly in Jericho*/
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field64_read(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf, &msb_mac)); 

              _ARAD_PP_OAM_MAC_ADD_MSB_SET(msb_mac, mac_add_U32);
              _ARAD_PP_OAM_MAC_ADD_LSB_SET(mep_db_entry->local_port, mac_add_U32);
          } 

	  }

      /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
      /* coverity[overrun-buffer-val : FALSE] */   
      res = soc_sand_pp_mac_address_long_to_struct(mac_add_U32, &mep_db_entry->src_mac_address);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      if (!mep_db_entry->is_upmep || SOC_PPD_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type)) {
          if (SOC_IS_ARAD_B0_AND_ABOVE(unit) || SOC_PPD_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type)) {
              /*get system port from local to system port table*/
              if (SOC_IS_ARADPLUS(unit)) { 
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_MEM_20000m(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
                  mep_db_entry->system_port = soc_OAMP_MEM_20000m_field32_get(unit, &entry, ITEM_0_15f); 
              } else {
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_OAMP_LOCAL_PORT_2_SYSTEM_PORTm(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
                  soc_OAMP_LOCAL_PORT_2_SYSTEM_PORTm_field_get(unit, &entry, SYS_PORTf, &mep_db_entry->system_port);
              }
          } else {
              /* Due to a HW bug in Arad A0 (Ethernet OAM only) this table isn't used. the system port is taken directly from the local port (set in the bcm layer)*/
              mep_db_entry->system_port = mep_db_entry->local_port;
          }
      } 
  }
  else {
     mep_db_entry->system_port = mep_db_entry->local_port;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_get_unsafe()", 0, 0);
}


/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_delete_verify(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   mep_index,
      SOC_SAND_IN  uint8                    deallocate_icc_ndx,
      SOC_SAND_IN  uint8                    is_last_mep
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_delete_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_oamp_mep_db_entry_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  uint8                    deallocate_icc_ndx,
    SOC_SAND_IN  uint8                    is_last_mep
  )
{
    uint32  res;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry;
    soc_reg_above_64_val_t entry_above_64;
    uint32  entry_buffer[2];
    SOC_PPD_OAM_ICC_MAP_DATA icc_data;
    uint64  val64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_UNSAFE);

    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    ARAD_PP_CLEAR(entry_buffer, uint32, 2);

    res = arad_pp_oam_oamp_mep_db_entry_get_unsafe(unit, mep_index, &mep_db_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_REG_ABOVE_64_CLEAR(entry_above_64);

    /* Delete MEP DB entry */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_index, entry_above_64));

    /* MA name handling - only relevant for Ethernet and Y1731 */
    if (!SOC_PPD_OAM_IS_MEP_TYPE_BFD(mep_db_entry.mep_type)) {

        if (deallocate_icc_ndx) {
            COMPILER_64_ZERO(icc_data);
            arad_pp_oam_icc_map_register_set_unsafe(unit, (int)mep_db_entry.icc_ndx, &icc_data);
        }
        if (is_last_mep && mep_db_entry.mep_type ==SOC_PPD_OAM_MEP_TYPE_ETH_OAM &&SOC_IS_ARADPLUS_AND_BELOW(unit) ) {
            /* SA address handled seperatly in Jericho.*/          
            COMPILER_64_ZERO(val64);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf,  val64));
        }
        if (!_ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry.icc_ndx)) {
            /*delete UMC entry*/
            SOC_REG_ABOVE_64_CLEAR(entry_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
            soc_OAMP_UMC_TABLEm_field_set(unit, entry_buffer, UMCf, entry_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
        }
    }

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_delete_unsafe()", 0, 0);
}


/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_index, SOC_PPD_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 20, exit);  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_set_verify()", rmep_index, 0);
}

uint32
  arad_pp_oam_oamp_rmep_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
    SOC_SAND_IN  uint8                    update
  )
{
  uint32  res;
  uint32  entry;
  uint32  entry_buffer[2];
  uint32  reg_val;
  uint32  rmep_key;
  uint32  rmep_scan_time, lifetime, lifetime_units;
  uint64  entry_64, val64;
  uint8   found;
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  fail_reason_val, fail_key_val;
  uint64  fail_valid_val;
  uint64  failure;

#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE); 
  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);

  entry = 0;
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  if (!update) { /* Insert into RMEP-EM only if it's a new entry */
      _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET(mep_type, mep_index, rmep_key, rmep_id);

     LOG_DEBUG(BSL_LS_SOC_OAM,
               (BSL_META_U(unit,
                           " Inserting entry to RMEPEM: key %d payload: %d\n\r"), rmep_key, rmep_index));

      /*enter RMEP index DB entry*/
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));
      reg_val = 1; /*insert*/
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_TYPEf, &reg_val);
      reg_val = 0; /*STAMP*/
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_STAMPf, &reg_val);
      reg_val = 0; /*SELF*/
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_SELFf, &reg_val);
      reg_val = 0;
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_RESERVED_LOWf, &reg_val);
      reg_val = 3;/*AGE_STATUS*/
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_AGE_STATUSf, &reg_val);
      reg_val = rmep_key;
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_KEYf, &reg_val);
      reg_val = rmep_index; 
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_PAYLOADf, &reg_val);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));

      if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
          res = chip_sim_exact_match_entry_add_unsafe(
              unit,
              ARAD_CHIP_SIM_RMAPEM_BASE,
              &rmep_key,
              ARAD_CHIP_SIM_RMAPEM_KEY,
              &rmep_index,/* payload_data */
              ARAD_CHIP_SIM_RMAPEM_PAYLOAD,
              &found
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      }

#if SOC_DPP_IS_EM_HW_ENABLE

      res = arad_polling(
              unit,
              ARAD_TIMEOUT,
              ARAD_MIN_POLLS,
              OAMP_REMOTE_MEP_EXACT_MATCH_INTERRUPT_REGISTER_ONEr,
              REG_PORT_ANY,
              0,
              RMAPEM_MANAGEMENT_COMPLETEDf,
              1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 75, exit, READ_OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
      fail_valid_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_VALIDf);

      if (COMPILER_64_LO(fail_valid_val)) {
          /*
           *    Failure indication is on.
           *  Check if it is on the last operation
           */       
          fail_key_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_KEYf);

          if (COMPILER_64_LO(fail_key_val) != rmep_key)
          {
              goto exit;
          }

          fail_reason_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_REASONf);
          if (COMPILER_64_LO(fail_reason_val)==0x7) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_EM_FULL_ERR, 80, exit);
          }
          else {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_EM_INTERNAL_ERR, 90, exit);
          }
      }
#endif
  }

  /*CALLCULATE lifetime & lifetime_units*/
  rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
  if (SOC_SAND_DIV_ROUND_DOWN(rmep_db_entry->ccm_period, rmep_scan_time) <= 0x3FF) {
      lifetime_units = _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD;
      lifetime = ((rmep_db_entry->ccm_period/ rmep_scan_time) < 1) ? 1 : (rmep_db_entry->ccm_period / rmep_scan_time);
  }
  else if (SOC_SAND_DIV_ROUND_DOWN(rmep_db_entry->ccm_period/100, rmep_scan_time) <= 0x3FF){
      lifetime_units = _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_100;
      lifetime =(((rmep_db_entry->ccm_period/100) / rmep_scan_time) < 1) ? 1 : ((rmep_db_entry->ccm_period/100) / rmep_scan_time);
  }    
  else {
      lifetime_units = _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_10000;
      lifetime = (((((rmep_db_entry->ccm_period/1000))) / (rmep_scan_time*10)) < 1) ? 1 : ((((rmep_db_entry->ccm_period/1000))) / (rmep_scan_time*10));
  }

  /*enter RMEP DB entry*/  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry));
  reg_val = lifetime; 
  soc_OAMP_RMEP_DBm_field_set(unit, &entry, LIFETIMEf, &reg_val);
  reg_val = lifetime_units; 
  soc_OAMP_RMEP_DBm_field_set(unit, &entry, LIFETIME_UNITSf, &reg_val);
  if (rmep_db_entry->loc_clear_threshold == 0) {
      reg_val = 3; 
      soc_OAMP_RMEP_DBm_field_set(unit, &entry, LOC_CLEAR_LIMTf, &reg_val);
      reg_val = 0; 
      soc_OAMP_RMEP_DBm_field_set(unit, &entry, LOC_CLEAR_ENABLEf, &reg_val);
  }
  else {
      reg_val = rmep_db_entry->loc_clear_threshold; 
      soc_OAMP_RMEP_DBm_field_set(unit, &entry, LOC_CLEAR_LIMTf, &reg_val);
      reg_val = 1; 
      soc_OAMP_RMEP_DBm_field_set(unit, &entry, LOC_CLEAR_ENABLEf, &reg_val);
  }
  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      reg_val = rmep_db_entry->is_event_mask; 
      soc_OAMP_RMEP_DBm_field_set(unit, &entry, EVENT_MASKf, &reg_val);
      reg_val = rmep_db_entry->is_state_auto_handle; 
      soc_OAMP_RMEP_DBm_field_set(unit, &entry, STATE_AUTO_HANDLEf, &reg_val);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry));

  if (SOC_IS_ARADPLUS(unit)) {
      COMPILER_64_ZERO(entry_64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry_64));
      COMPILER_64_SET(val64, 0, rmep_db_entry->punt_profile);
      soc_mem_field64_set(unit, OAMP_RMEP_DBm, &entry_64, PUNT_PROFILEf, val64);

      COMPILER_64_SET(val64, 0, rmep_db_entry->rmep_state);
      soc_mem_field64_set(unit, OAMP_RMEP_DBm, &entry_64, REMOTE_STATEf, val64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry_64));
  }

   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_set_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_index, SOC_PPD_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_get_verify()", rmep_index, 0);
}

uint32
  arad_pp_oam_oamp_rmep_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{
  uint32  res;
  uint32  entry;
  uint64  entry_64, val64;
  uint32  lifetime, lifetime_units, rmep_scan_time;
  uint32  field;
  uint32  loc_clear_enabled;
     
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_GET_UNSAFE); 
  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);

  entry= 0;

  /*get RMEP DB entry*/  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry));
  soc_OAMP_RMEP_DBm_field_get(unit, &entry, LIFETIMEf, &lifetime);
  soc_OAMP_RMEP_DBm_field_get(unit, &entry, LIFETIME_UNITSf, &lifetime_units);
  rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
  if (lifetime_units == _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD) {
      rmep_db_entry->ccm_period = lifetime*rmep_scan_time;
  }
  else if (lifetime_units == _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_100){
      rmep_db_entry->ccm_period = (lifetime*rmep_scan_time)*100;
  }
  else {
      rmep_db_entry->ccm_period = SOC_SAND_DIV_ROUND(lifetime*rmep_scan_time, 100);
  } 

  soc_OAMP_RMEP_DBm_field_get(unit, &entry, LOC_CLEAR_ENABLEf, &loc_clear_enabled);
  if (loc_clear_enabled) {
      soc_OAMP_RMEP_DBm_field_get(unit, &entry, LOC_CLEAR_LIMTf, &(rmep_db_entry->loc_clear_threshold)); 
  }
  else {
      rmep_db_entry->loc_clear_threshold = 0;
  }

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      soc_OAMP_RMEP_DBm_field_get(unit, &entry, EVENT_MASKf, &field);
      rmep_db_entry->is_event_mask = (uint8)field;
      soc_OAMP_RMEP_DBm_field_get(unit, &entry, STATE_AUTO_HANDLEf, &field);
      rmep_db_entry->is_state_auto_handle = (uint8)field;
  }
  else {
      COMPILER_64_ZERO(entry_64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry_64));
      soc_mem_field64_get(unit, OAMP_RMEP_DBm, &entry_64, PUNT_PROFILEf, &val64);
      rmep_db_entry->punt_profile = (uint8)COMPILER_64_LO(val64);

	  soc_mem_field64_get(unit, OAMP_RMEP_DBm, &entry_64, REMOTE_STATEf, &val64);
      rmep_db_entry->rmep_state = COMPILER_64_LO(val64);

  }

  soc_OAMP_RMEP_DBm_field_get(unit, &entry, RDI_RECEIVEDf, &field);
  rmep_db_entry->rdi_received = (uint8)field;

  soc_OAMP_RMEP_DBm_field_get(unit, &entry, LOCf, &field);
  rmep_db_entry->loc =field; 
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_get_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_delete_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_DELETE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_index, SOC_PPD_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_delete_verify()", rmep_index, 0);
}

uint32
  arad_pp_oam_oamp_rmep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  )
{
  uint32  res;
  uint32  reg_val;
  uint32  entry;
  uint32  entry_buffer[2];
  uint32  rmep_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_DELETE_UNSAFE);

  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET(mep_type, mep_index, rmep_key, rmep_id);

  /*delete RMEP index DB entry*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));
  reg_val = 0; /*delete*/
  soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_TYPEf, &reg_val);
  reg_val = 0; /*STAMP*/
  soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_STAMPf, &reg_val);
  soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_KEYf, &rmep_key);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0/*table with 1 entry*/, entry_buffer));

  /*delete RMEP DB entry*/ 
  entry = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        res = chip_sim_exact_match_entry_remove_unsafe(
                    unit,
                    ARAD_CHIP_SIM_RMAPEM_BASE,
                    &rmep_key,
                    ARAD_CHIP_SIM_RMAPEM_KEY
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_delete_unsafe()", 0, 0);
}

/*********************************************************************
*     
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_index_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_delete_verify()", 0, 0);

}

uint32
  arad_pp_oam_oamp_rmep_index_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
    SOC_SAND_OUT uint32                   *rmep_index,
    SOC_SAND_OUT  uint8                   *is_found
  )
{
  uint32  res;
  uint32  rmep_key;
  uint32  fld_val;
  uint32  reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(rmep_index);

  _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET(mep_type, mep_index, rmep_key, rmep_id);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_KEYr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_KEYf,  rmep_key));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICSr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUPf,  1));

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit) || ((SOC_IS_DONE_INIT(unit)) && (BCM_UNIT_DO_HW_READ_WRITE(unit))))
  {
      /* HW Log is on - get the value from the sw db, and exit. No need to proceed. */

      res = chip_sim_exact_match_entry_get_unsafe(
              unit,
              ARAD_CHIP_SIM_RMAPEM_BASE,
              &rmep_key,
              ARAD_CHIP_SIM_RMAPEM_KEY,
              &fld_val,
              ARAD_CHIP_SIM_RMAPEM_PAYLOAD,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      if (*is_found) {
          *rmep_index = fld_val;
      }
      else {
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to RMAPEM: key %d not found\n\r"), rmep_key));
          }
      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
#endif

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUPf,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_val)); 

  ARAD_FLD_FROM_REG(OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUP_RESULTr, REMOTE_MEP_EXACT_MATCH_ENTRY_FOUNDf, fld_val, reg_val, 50, exit);
  *is_found = fld_val;

  if (*is_found) {
      ARAD_FLD_FROM_REG(OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUP_RESULTr, REMOTE_MEP_EXACT_MATCH_ENTRY_PAYLOADf, fld_val, reg_val, 60, exit);
      *rmep_index = fld_val;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_index_get_unsafe()", 0, 0);
}


/*********************************************************************
*     Find MEP according to lif & mdlevel
 *    Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   lif,
    SOC_SAND_IN  uint8                    md_level,
    SOC_SAND_IN  uint8                    is_upmep
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_VERIFY);

      SOC_SAND_ERR_IF_ABOVE_MAX(md_level, _ARAD_PP_OAM_MAX_MD_LEVEL, ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(lif, OAM_LIF_MAX_VALUE(unit), ARAD_PP_LIF_ID_OUT_OF_RANGE_ERR, 20, exit); 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify()", 0, 0);
}

uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   lif,
    SOC_SAND_IN  uint8                    md_level,
    SOC_SAND_IN  uint8                    is_upmep,
    SOC_SAND_OUT uint8                    *found_mep,
    SOC_SAND_OUT uint32                   *profile,
    SOC_SAND_OUT uint8                    *found_profile,
    SOC_SAND_OUT uint8                    *is_mp_type_flexible,
    SOC_SAND_OUT  uint8                    *is_mip
  )
{
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key;
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload;
    SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY      oem2_key;
    SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload;
    uint8 passive_active_enable = 0;
    uint32  res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_UNSAFE);

    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
    /* OAM only. your_discr bit is always off.*/
    oem1_key.oam_lif = lif;
    oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
    res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, found_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *found_mep = FALSE;
    *is_mip = FALSE;
    *is_mp_type_flexible = TRUE;

    /*Check if md_level exists*/
    if (*found_profile) {
        *profile = oem1_payload.mp_profile;
        if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
			int mdl;
            int mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, md_level);
			*found_mep = mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH || mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP;
			*found_profile = 2; 
			for (mdl=0 ; mdl <8 ; ++ mdl) {
				/* Check if any active entries exist. on this OEM1 entry*/
				mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, mdl);
				if (mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH || mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP) {
					/* found an active entry, say yes and get out.*/
					*found_profile = 1;
					break;
				}
			}
		} else {
            *found_mep = ((oem1_payload.mep_bitmap >> md_level) & 1);
            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                 res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, *profile, &passive_active_enable);
                 SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
	    }

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (!(*found_mep)) && (md_level != 0)) {
                /* No active MEPs on this LIF. MIPs will only be configured if using the passive model. */
                /* mdl 0 can not be on passive/mip side */
                if (!passive_active_enable) {
                   *found_mep |= (((oem1_payload.mip_bitmap & 0x7) == md_level) || ((oem1_payload.mip_bitmap >> 3) & 0x7) == md_level);
                   if (*found_mep) {
                       *is_mip = TRUE;
                   }
                }
            }
            else {
                if ((oem1_payload.mip_bitmap >> md_level) & 1) {
                    *found_mep = *is_mip = TRUE;
                }
            }
        }

        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
            /* If profile is passive, endpoint does not exist on this direction */
            if ((*found_mep) && (*profile==ARAD_PP_OAM_PROFILE_PASSIVE)) {
                *found_mep = FALSE;
            }
        }
        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
            /* Need to check OEM2 to find if MEP is active */
            if (*found_mep) {
                SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
                SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);
                
                oem2_key.oam_lif = lif;
                oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
                oem2_key.mdl = md_level;
                res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, found_mep); 
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            }
        }
        else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
            /* ARAD+ If no mips configured or all meps configured on same direction -> mp type can be changed */
            if (oem1_payload.mip_bitmap == 0) {
                *is_mp_type_flexible = TRUE;
            }
            else if (passive_active_enable && (oem1_payload.mep_bitmap == 0)) {
                *is_mp_type_flexible = TRUE;
            }
            else {
                *is_mp_type_flexible = FALSE;
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_counter_range_set_unsafe(
    SOC_SAND_IN   int                                     unit,
    SOC_SAND_IN   uint32                                     counter_range_min,
    SOC_SAND_IN   uint32                                     counter_range_max
  )
{
  uint32  res;
  uint32  reg;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_COUNTER_RANGEr(unit, 0, &reg));
  soc_reg_field_set(unit, IHP_OAM_COUNTER_RANGEr, &reg, OAM_COUNTER_MINf, counter_range_min);
  soc_reg_field_set(unit, IHP_OAM_COUNTER_RANGEr, &reg, OAM_COUNTER_MAXf, counter_range_max);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_OAM_COUNTER_RANGEr(unit, SOC_CORE_ALL, reg));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_counter_range_set_verify(
    SOC_SAND_IN   int                                     unit,
    SOC_SAND_IN   uint32                                     counter_range_min,
    SOC_SAND_IN   uint32                                     counter_range_max
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_NOF(counter_range_min, SOC_DPP_DEFS_GET(unit, nof_counters), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(counter_range_max, SOC_DPP_DEFS_GET(unit, nof_counters), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_set_verify()", 0, 0);

}

uint32
  arad_pp_oam_counter_range_get_unsafe(
    SOC_SAND_IN   int                                     unit,
    SOC_SAND_OUT  uint32                                     *counter_range_min,
    SOC_SAND_OUT  uint32                                     *counter_range_max
  )
{
  uint32  res;
  uint32  reg;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_GET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_COUNTER_RANGEr(unit, 0, &reg));
  *counter_range_min = soc_reg_field_get(unit, IHP_OAM_COUNTER_RANGEr, reg, OAM_COUNTER_MINf);
  *counter_range_max = soc_reg_field_get(unit, IHP_OAM_COUNTER_RANGEr, reg, OAM_COUNTER_MAXf);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_counter_range_get_verify(
    SOC_SAND_IN   int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_get_verify()", 0, 0);

}

/*********************************************************************
*     Configure mapping of network opcode to internal opcode. 
*     Offset is the offset that the pipeline needs to stamp the packet.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_eth_oam_opcode_map_set_unsafe(
    SOC_SAND_IN   int                                     unit
  )
{
  uint8 internal_opcode;
  uint32 ethernet_opcode;
  uint32 reg;
  uint32 entry;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (ethernet_opcode=1; ethernet_opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; ethernet_opcode++) {
      res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, ethernet_opcode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_ETHERNET_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
      reg = internal_opcode;
      soc_IHP_ETHERNET_OAM_OPCODE_MAPm_field_set(unit, &entry, OPCODEf, &reg);
      reg = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(ethernet_opcode);
      soc_IHP_ETHERNET_OAM_OPCODE_MAPm_field_set(unit, &entry, OFFSETf, &reg);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_ETHERNET_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_ETH_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
      reg = internal_opcode;
      soc_EPNI_ETH_OAM_OPCODE_MAPm_field_set(unit, &entry, ETH_OAM_OPCODE_MAP_DATAf, &reg);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EPNI_ETH_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_eth_oam_opcode_map_set_unsafe()", 0, 0);
}


/*
 * ARAD BFD APIs
*/
/*********************************************************************
*     Set Ipv4 Tos Ttl Select register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
    SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  entry_val = tos_ttl_data->ttl + (tos_ttl_data->tos << SOC_PPD_OAMP_LENGTH_TTL);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_TOS_TTL_SELECTr(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPD_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS*ipv4_tos_ttl_select_index, &entry_val, 0, SOC_PPD_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_IPV4_TOS_TTL_SELECTr(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe()", ipv4_tos_ttl_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
    SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA, tos_ttl_data, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify()", 0, 0);

}

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
    SOC_SAND_OUT SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val = 0;
  uint32 tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_TOS_TTL_SELECTr(unit, reg_val));
  SHR_BITCOPY_RANGE(&entry_val, 0, reg_val, SOC_PPD_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS*ipv4_tos_ttl_select_index, SOC_PPD_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS);
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, 0, SOC_PPD_OAMP_LENGTH_TTL);
  tos_ttl_data->ttl = tmp & 0xff;
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, SOC_PPD_OAMP_IPV4_TOS_TTL_LENGTH_TOS, SOC_PPD_OAMP_LENGTH_TTL);
  tos_ttl_data->tos = tmp & 0xff;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe()", ipv4_tos_ttl_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Ipv4 Src Addr Select register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
    SOC_SAND_IN  uint32                                       src_addr
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_SRC_ADDR_SELECTr(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPD_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS*ipv4_src_addr_select_index, &src_addr, 0, SOC_PPD_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_IPV4_SRC_ADDR_SELECTr(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe()", ipv4_src_addr_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
    SOC_SAND_IN  uint32                                       src_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
    SOC_SAND_OUT uint32                                       *src_addr
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(src_addr);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_SRC_ADDR_SELECTr(unit, reg_val));
  SHR_BITCOPY_RANGE(src_addr, 0, reg_val, SOC_PPD_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS*ipv4_src_addr_select_index, SOC_PPD_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe()", ipv4_src_addr_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Bfd Tx Rate register
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_oam_bfd_tx_rate_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
    SOC_SAND_IN  uint32                                       tx_rate
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  soc_field_t bfd_rate_field[] = {BFD_TX_RATE_0f,BFD_TX_RATE_1f,BFD_TX_RATE_2f,BFD_TX_RATE_3f,BFD_TX_RATE_4f,BFD_TX_RATE_5f,BFD_TX_RATE_6f,
      BFD_TX_RATE_7f};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_RATEr(unit, reg_val));

  soc_reg_above_64_field32_set(unit,OAMP_BFD_TX_RATEr, reg_val,bfd_rate_field[bfd_tx_rate_index], tx_rate);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_TX_RATEr(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_set_unsafe()", bfd_tx_rate_index, 0);
}

uint32
  arad_pp_oam_bfd_tx_rate_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
    SOC_SAND_IN  uint32                                       tx_rate
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_tx_rate_index, _ARAD_PP_OAM_TX_RATE_INDEX_MAX, ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_rate_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
    SOC_SAND_OUT uint32                                       *tx_rate
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  soc_field_t bfd_rate_field[] = { BFD_TX_RATE_0f, BFD_TX_RATE_1f, BFD_TX_RATE_2f, BFD_TX_RATE_3f, BFD_TX_RATE_4f, BFD_TX_RATE_5f, BFD_TX_RATE_6f,
      BFD_TX_RATE_7f };

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(tx_rate);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_RATEr(unit, reg_val));
  *tx_rate = soc_reg_above_64_field32_get(unit,OAMP_BFD_TX_RATEr, reg_val,bfd_rate_field[bfd_tx_rate_index]);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_get_unsafe()", bfd_tx_rate_index, 0);
}

uint32
  arad_pp_oam_bfd_tx_rate_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_tx_rate_index, _ARAD_PP_OAM_TX_RATE_INDEX_MAX, ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Bfd Req Interval Pointer register (MPLS Push profile)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_req_interval_pointer_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer,
    SOC_SAND_IN  uint32                                       req_interval
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_REQ_INTERVAL_POINTERr(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPD_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS*req_interval_pointer, &req_interval, 0, SOC_PPD_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_REQ_INTERVAL_POINTERr(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_set_unsafe()", req_interval_pointer, 0);
}

uint32
  arad_pp_oam_bfd_req_interval_pointer_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer,
    SOC_SAND_IN  uint32                                       req_interval
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(req_interval_pointer, _ARAD_PP_OAM_REQ_INTERVAL_POINTER_MAX, ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer,
    SOC_SAND_OUT uint32                                       *req_interval
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(req_interval);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_REQ_INTERVAL_POINTERr(unit, reg_val));
  SHR_BITCOPY_RANGE(req_interval, 0, reg_val, SOC_PPD_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS*req_interval_pointer, SOC_PPD_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_get_unsafe()", req_interval_pointer, 0);
}

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(req_interval_pointer, _ARAD_PP_OAM_REQ_INTERVAL_POINTER_MAX, ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Mpls Pwe Profile register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_mpls_pwe_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile,
    SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(push_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  entry_val = push_data->ttl + (push_data->exp << SOC_PPD_OAMP_LENGTH_TTL);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MPLS_PWE_PROFILEr(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPD_OAMP_MPLS_PWE_PROFILE_NOF_BITS*push_profile, &entry_val, 0, SOC_PPD_OAMP_MPLS_PWE_PROFILE_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_MPLS_PWE_PROFILEr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_set_unsafe()", push_profile, 0);
}

uint32
  arad_pp_oam_mpls_pwe_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile,
    SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPD_MPLS_PWE_PROFILE_DATA, push_data, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_mpls_pwe_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile,
    SOC_SAND_OUT SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val = 0;
  uint32 tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(push_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MPLS_PWE_PROFILEr(unit, reg_val));
  SHR_BITCOPY_RANGE(&entry_val, 0, reg_val, SOC_PPD_OAMP_MPLS_PWE_PROFILE_NOF_BITS*push_profile, SOC_PPD_OAMP_MPLS_PWE_PROFILE_NOF_BITS);
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, 0, SOC_PPD_OAMP_LENGTH_TTL);
  push_data->ttl = tmp & 0xff;
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, SOC_PPD_OAMP_MPLS_PWE_PROFILE_LENGTH_EXP, SOC_PPD_OAMP_LENGTH_TTL);
  push_data->exp = tmp & 0xff;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_get_unsafe()", push_profile, 0);
}

uint32
  arad_pp_oam_mpls_pwe_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_get_verify()", 0, 0);
}


/*********************************************************************
*     Set Bfd MPLS UDP Sport register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32  res;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_BFD_MPLS_UDP_SPORTr(unit, udp_sport));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT uint16                                       *udp_sport
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(udp_sport);
  reg = 0;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_MPLS_UDP_SPORTr(unit, &reg));

  *udp_sport = (uint16)reg;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Bfd IPV4 UDP Sport register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_UNSAFE);

  reg = udp_sport;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_BFD_IPV4_UDP_SPORTr(unit, reg));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT  uint16                                      *udp_sport
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(udp_sport);
  reg = 0;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_IPV4_UDP_SPORTr(unit, &reg));
  *udp_sport = (uint16)reg;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_get_verify()", 0, 0);
}


/*********************************************************************
*     Set Bfd pdu static register
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_oam_bfd_pdu_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32  res;
  uint64  reg, field64;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_UNSAFE);
  COMPILER_64_ZERO(reg);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_PDU_STATIC_REGISTERr(unit, &reg));
  COMPILER_64_SET(field64,0, bfd_pdu->bfd_vers);
  soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_VERSf, field64);

  COMPILER_64_SET(field64,0, bfd_pdu->bfd_length);
  soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_LENGTHf, field64);

  COMPILER_64_SET(field64,0, bfd_pdu->bfd_req_min_echo_rx_interval);
  soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_REQ_MIN_ECHO_RX_INTERVALf, field64);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      COMPILER_64_SET(field64,0, bfd_pdu->bfd_diag);
      soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_DIAGf, field64);

      COMPILER_64_SET(field64,0, bfd_pdu->bfd_sta);
      soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_STAf, field64);

      COMPILER_64_SET(field64,0, bfd_pdu->bfd_flags);
      soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_FLAGSf, field64);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_PDU_STATIC_REGISTERr(unit, reg));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_pdu_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPD_BFD_PDU_STATIC_REGISTER, bfd_pdu, 10, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_pdu_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32  res;
  uint64  reg,val64;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_UNSAFE);
  COMPILER_64_ZERO(reg);
  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_PDU_STATIC_REGISTERr(unit, &reg));
  val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_VERSf);
  bfd_pdu->bfd_vers = COMPILER_64_LO(val64);

  val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_LENGTHf);
  bfd_pdu->bfd_length = COMPILER_64_LO(val64);

  val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_REQ_MIN_ECHO_RX_INTERVALf);
  bfd_pdu->bfd_req_min_echo_rx_interval = COMPILER_64_LO(val64);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_DIAGf);
      bfd_pdu->bfd_diag = COMPILER_64_LO(val64);

      val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_STAf);
      bfd_pdu->bfd_sta = COMPILER_64_LO(val64);

      val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_FLAGSf);
      bfd_pdu->bfd_flags = COMPILER_64_LO(val64);
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_pdu_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
    soc_reg_above_64_val_t reg;
    uint32 res;
    uint8 first_word_in_reg[4] = {0};
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_CC_PACKETr(unit,reg));
    /* bits 0-2, 3-7*/
    first_word_in_reg[0] = (bfd_cc_packet->bfd_static_reg_fields.bfd_vers) << 5 | (bfd_cc_packet->bfd_static_reg_fields.bfd_diag & 0x1f);
    /*bits 8-9, 12-15*/
    first_word_in_reg[1] = bfd_cc_packet->bfd_static_reg_fields.bfd_sta <<6 | (bfd_cc_packet->bfd_static_reg_fields.bfd_flags &0x3f );
    /*bits 24-31*/
    first_word_in_reg[3] = bfd_cc_packet->bfd_static_reg_fields.bfd_length;
    SHR_BITCOPY_RANGE(reg,160, (uint32*)first_word_in_reg,0,32);
    /*note that the register is backwards. First the sixth word then the fifth word an so on*/
       
   /*bits 32-63*/
   SHR_BITCOPY_RANGE(reg, 128, &(bfd_cc_packet->bfd_my_discr), 0, 32);
   /*bits 64-95*/
   SHR_BITCOPY_RANGE(reg, 96, &(bfd_cc_packet->bfd_your_discr), 0, 32); 
   /*bits 160 -191*/
   SHR_BITCOPY_RANGE(reg, 0, &(bfd_cc_packet->bfd_static_reg_fields.bfd_req_min_echo_rx_interval), 0, 32);

   SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_BFD_CC_PACKETr(unit,reg));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_STRUCT_VERIFY(SOC_PPD_BFD_PDU_STATIC_REGISTER, &(bfd_cc_packet->bfd_static_reg_fields), 10, exit);
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
    soc_reg_above_64_val_t reg;
    uint32 res, just_a_num=0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_CC_PACKETr(unit,reg));
    /*note that the register is backwards. First the sixth word then the fifth word an so on*/

    /* bits 0-2*/
    SHR_BITCOPY_RANGE(&(just_a_num),0,reg,189,3);
    bfd_cc_packet->bfd_static_reg_fields.bfd_vers = just_a_num;
    /* bits 3-7*/
    just_a_num = 0;
    SHR_BITCOPY_RANGE(&just_a_num, 0,reg, 184,5);
    bfd_cc_packet->bfd_static_reg_fields.bfd_diag = just_a_num;
    /*bits 8-9*/
    just_a_num = 0;
   SHR_BITCOPY_RANGE(&just_a_num,0,reg,182,2);
   bfd_cc_packet->bfd_static_reg_fields.bfd_sta = just_a_num;
   /*bits 12-15*/
   just_a_num = 0;
   SHR_BITCOPY_RANGE(&just_a_num,0,reg, 176,6); 
   bfd_cc_packet->bfd_static_reg_fields.bfd_flags = just_a_num;
   /*bits 24-31*/
   just_a_num = 0;
   SHR_BITCOPY_RANGE( &just_a_num,0,reg, 160, 8);
   bfd_cc_packet->bfd_static_reg_fields.bfd_length  = just_a_num;
   /*bits 32-63*/
   SHR_BITCOPY_RANGE(&(bfd_cc_packet->bfd_my_discr), 0, reg, 128, 32);
   /*bits 64-95*/
   SHR_BITCOPY_RANGE(&(bfd_cc_packet->bfd_your_discr), 0, reg,96, 32); 
   /*bits 160 -191*/
   SHR_BITCOPY_RANGE(&(bfd_cc_packet->bfd_static_reg_fields.bfd_req_min_echo_rx_interval) ,0,reg, 0, 32);




exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  }
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN uint32                                      range
  )
{
  uint32  res;
  uint64  reg;
  uint32  field_value;
  uint64  field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_UNSAFE);
  COMPILER_64_ZERO(reg);

  /* my-discr TX*/
  field_value = range << SOC_PPD_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_MY_DISCRIMINATOR_RANGE_STARTr, REG_PORT_ANY, 0, BFD_MY_DISCRIMINATOR_RANGE_STARTf,  field_value));

  /* your-discr RX*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg));
  field_value = range >>  (SOC_PPD_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPD_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START  );

  COMPILER_64_SET(field64,0,field_value);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MINf, field64);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MAXf, field64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, SOC_CORE_ALL, reg));

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_set_unsafe()", 0, 0);
}

soc_error_t arad_pp_oam_bfd_discriminator_rx_range_set(
   int unit,
   int  range_min,
   int  range_max)
{
    uint64  reg;
    uint64  field64;
    SOCDNX_INIT_FUNC_DEFS;


    COMPILER_64_ZERO(reg);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg));


  COMPILER_64_SET(field64,0,range_min);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MINf, field64);
  COMPILER_64_SET(field64,0,range_max);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MAXf, field64);

  SOCDNX_SAND_IF_ERR_EXIT( WRITE_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, SOC_CORE_ALL, reg));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t arad_pp_oam_bfd_discriminator_rx_range_get(
   int unit,
   int * range_min,
   int * range_max)
{
    uint64  reg;
    SOCDNX_INIT_FUNC_DEFS;


    COMPILER_64_ZERO(reg);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg));
    
    *range_min =  soc_reg64_field32_get(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, reg, BFD_YOUR_DISCR_RANGE_MINf);
    *range_max =  soc_reg64_field32_get(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, reg, BFD_YOUR_DISCR_RANGE_MAXf); 


exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                        range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT uint32                                      *range
  )
{
  uint32  res;
  uint32  field_value;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(range);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_BFD_MY_DISCRIMINATOR_RANGE_STARTr, REG_PORT_ANY, 0, BFD_MY_DISCRIMINATOR_RANGE_STARTf, &field_value));
  *range = field_value >> SOC_PPD_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS                   *ipv6_addr                            
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_above_64;
  uint32  valid;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_UNSAFE);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);

  valid = ipv6_addr->address[0] || ipv6_addr->address[1] || ipv6_addr->address[2] || ipv6_addr->address[3] ; /* If the given address is zero, valid bit should be zero.*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MY_BFD_DIPm(unit, MEM_BLOCK_ANY, dip_index, reg_above_64));
  soc_IHP_MY_BFD_DIPm_field_set(unit, reg_above_64, DIPf, (uint32*)ipv6_addr->address);
  soc_IHP_MY_BFD_DIPm_field_set(unit, reg_above_64, VALIDf, &valid);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHP_MY_BFD_DIPm(unit, MEM_BLOCK_ANY, dip_index, reg_above_64));

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                       dip_index,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS                     * dip    
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(dip);
  SOC_SAND_ERR_IF_ABOVE_MAX(dip_index, _ARAD_PP_OAM_DIP_INDEX_MAX, ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_set_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
    SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS                    *ipv6_addr                            
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_above_64;
  uint32  valid;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ipv6_addr);  
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(ipv6_addr);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MY_BFD_DIPm(unit, MEM_BLOCK_ANY, dip_index, reg_above_64));
  soc_IHP_MY_BFD_DIPm_field_get(unit, reg_above_64, VALIDf, &valid);
  if (valid) {
      soc_IHP_MY_BFD_DIPm_field_get(unit, reg_above_64, DIPf, ipv6_addr->address);
  }


  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                       dip_index
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(dip_index, _ARAD_PP_OAM_DIP_INDEX_MAX, ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Bfd Tx Ipv4 Multi Hop register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_UNSAFE);
  reg = 0;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_IPV4_MULTI_HOPr(unit, &reg));
  soc_reg_field_set(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, &reg, BFD_IPV4_MULTI_HOP_TCf, tx_ipv4_multi_hop_att->tc);
  soc_reg_field_set(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, &reg, BFD_IPV4_MULTI_HOP_DPf, tx_ipv4_multi_hop_att->dp);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_TX_IPV4_MULTI_HOPr(unit, reg));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES, tx_ipv4_multi_hop_att, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES               *tx_ipv4_multi_hop_att
  )
{
    uint32  res;
    uint32  reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_UNSAFE);
    reg = 0;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_IPV4_MULTI_HOPr(unit, &reg));
    tx_ipv4_multi_hop_att->tc = soc_reg_field_get(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, reg, BFD_IPV4_MULTI_HOP_TCf);
    tx_ipv4_multi_hop_att->dp = soc_reg_field_get(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, reg, BFD_IPV4_MULTI_HOP_DPf);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify()", 0, 0);
}


/*********************************************************************
*     Set OAMP priority TC and DP registers
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_tx_priority_registers_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority,
    SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  )
{
  uint32  res;
  uint32  reg;
  uint32  field;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_UNSAFE);
  reg = 0;

  field = (uint32)tx_oam_att->tc;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PR_2_FW_DTCr(unit, &reg));
  SHR_BITCOPY_RANGE(&reg, (_ARAD_PP_OAM_TC_NOF_BITS+1)*priority, &field, 0, _ARAD_PP_OAM_TC_NOF_BITS); /*+1 because there is one more bit in HW */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_PR_2_FW_DTCr(unit, reg));
 
  field = (uint32)tx_oam_att->dp;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_PR_2_FWDDPr(unit, &reg));
  SHR_BITCOPY_RANGE(&reg, _ARAD_PP_OAM_DP_NOF_BITS*priority, &field, 0, _ARAD_PP_OAM_DP_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_PR_2_FWDDPr(unit, reg));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_set_unsafe()", priority, 0);
}

uint32
  arad_pp_oam_oamp_tx_priority_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority,
    SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_VERIFY);

  ARAD_STRUCT_VERIFY(SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES, tx_oam_att, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(priority, 7, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority,
    SOC_SAND_OUT  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  )
{
    uint32  res;
    uint32  reg;
    uint32  field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_UNSAFE);
    reg = 0;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PR_2_FW_DTCr(unit, &reg));
    SHR_BITCOPY_RANGE(&field, 0, &reg, (_ARAD_PP_OAM_TC_NOF_BITS+1)*priority, _ARAD_PP_OAM_TC_NOF_BITS);  /*+1 because there is one more bit in HW */
    tx_oam_att->tc = (uint8)field;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_PR_2_FWDDPr(unit, &reg));
    SHR_BITCOPY_RANGE(&field, 0, &reg, _ARAD_PP_OAM_DP_NOF_BITS*priority, _ARAD_PP_OAM_DP_NOF_BITS);
    tx_oam_att->dp = (uint8)field;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(priority, 7, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  )
{
    uint32  res;
    uint32  reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_UNSAFE);

    reg = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, &reg));
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_LOC_SET_EVENTf, interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_LOC_SET]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_LOC_CLEAR_EVENTf, interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_LOC_CLR]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_RDI_SET_EVENTf, interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_RDI_SET]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_RDI_CLEAR_EVENTf, interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_RDI_CLR]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_ALMOST_LOC_SET_EVENTf, interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_ALMOST_LOC_SET]);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, reg)); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  )
{
  uint32 i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_VERIFY);
  for (i=0; i<SOC_PPD_OAM_EVENT_COUNT; i++) {
      if (i != SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE) {
          SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_message_event_bmp[i], 1, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT  uint8                                       *interrupt_message_event_bmp
  )
{
    uint32  res;
    uint32  reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_UNSAFE);

    reg = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, &reg));
    interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_NULL] = 0;
    interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_LOC_SET] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_LOC_SET_EVENTf);
    interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_LOC_CLR] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_LOC_CLEAR_EVENTf);
    interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_RDI_SET] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_RDI_SET_EVENTf);
    interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_RDI_CLR] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_RDI_CLEAR_EVENTf);
    interrupt_message_event_bmp[SOC_PPD_OAM_EVENT_ALMOST_LOC_SET] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_ALMOST_LOC_SET_EVENTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_event_fifo_read_verify(
    SOC_SAND_IN  int                                        unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_EVENT_FIFO_READ_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_get_verify()", 0, 0);
}


/**
 * Function fills the global memory "last_interrupt_message_num" 
 * with message from last interrupt message, taken from the host 
 * memory written by the DMA. If there is a  fresh entry and the 
 * current entry has been exhausted then copy the fresh entry. 
 *  
 * @author sinai (10/11/2013)
 * 
 * @return uint32 
 */
uint32 arad_pp_oam_event_fifo_get_interrupt_mesage_dma_internal(
   SOC_SAND_IN  int                                        unit) {
    uint32 field_32;
    int  num_entries=0,res;
    uint32 empty_fifo_value = 0xffffffff;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


            
    if (num_entries_available_in_local_buffer[unit] == num_entries_read_in_local_buffer[unit]) { /* In this case try copying from the host_memory*/
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_CMIC_CMC0_FIFO_CH3_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr(unit, &field_32));
        num_entries = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH3_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr, field_32, ENTRYCOUNTf);
        if (num_entries > 0) {
            /* copy num_entries from the host_buff, inform the DMA  and update static variables*/
            if ((last_dma_interrupt_message_num[unit] + num_entries) > arad_pp_num_entries_in_dma_host_memory[unit] ) {
                /* Some of the entries written by the DMA were wrapped around*/
                    int entries_to_write = arad_pp_num_entries_in_dma_host_memory[unit]  - last_dma_interrupt_message_num[unit];
                    int entries_written;
                    sal_memcpy(buffer_copied_from_dma_host_memory[unit], dma_host_memory[unit] + last_dma_interrupt_message_num[unit] * 20,
                           _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA * entries_to_write );
                    entries_written = entries_to_write;
                    entries_to_write = num_entries -  entries_written;
                    sal_memcpy(buffer_copied_from_dma_host_memory[unit] + entries_written*20, dma_host_memory[unit],
                                entries_to_write * _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA );
            } else {
                /* No wrap around in current write. */
                sal_memcpy(buffer_copied_from_dma_host_memory[unit], dma_host_memory[unit] + last_dma_interrupt_message_num[unit] * 20,
                           _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA * num_entries);
            }
            
            num_entries_available_in_local_buffer[unit] = num_entries;
            num_entries_read_in_local_buffer[unit] = 0;
            last_dma_interrupt_message_num[unit] = (last_dma_interrupt_message_num[unit] + num_entries) % arad_pp_num_entries_in_dma_host_memory[unit] ;

            res = _soc_mem_sbus_fifo_dma_set_entries_read(unit, ARAD_OAM_DMA_CHANNEL_USED, num_entries);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2134, exit);

            /* Clear the interrupt*/
            soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH3_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_TIMEOUTf, 1);
            soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH3_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_OVERFLOWf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_CMIC_CMC0_FIFO_CH3_RD_DMA_STAT_CLRr(unit, field_32));
            sal_memset(interrupt_message[unit], 0xff, _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
            /*enable the interrupt*/
            soc_cmicm_intr0_enable(unit, IRQ_CMCx_FIFO_CH_DMA(ARAD_OAM_DMA_CHANNEL_USED));

        } 
    }
    
    if (last_interrupt_message_num[unit] == _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO ||
        (interrupt_message[unit][last_interrupt_message_num[unit]] == empty_fifo_value && 
         (num_entries_available_in_local_buffer[unit] -num_entries_read_in_local_buffer[unit] ) > 0)) {
        /* either we just got an interrupt or the last chunk we read has been exhausted. */
        /*if the buffer_copied_from_dma_host_memory has been exhausted, copy the previous entry, which has been set to "empty fifo value"*/
        num_entries_read_in_local_buffer[unit] = (num_entries_read_in_local_buffer[unit]== num_entries_available_in_local_buffer[unit])?
            num_entries_read_in_local_buffer[unit]-1:  num_entries_read_in_local_buffer[unit];

        sal_memcpy(interrupt_message[unit], buffer_copied_from_dma_host_memory[unit] + num_entries_read_in_local_buffer[unit] * 20
                   , _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
        /* imitating the "clear on read" behavior*/
        sal_memset(buffer_copied_from_dma_host_memory[unit] + num_entries_read_in_local_buffer[unit] * 20, 0xff
                   , _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);

        ++num_entries_read_in_local_buffer[unit];
        last_interrupt_message_num[unit] = 0;

        LOG_DEBUG(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              "gonna read from the %d-th chunk in the host memory"), last_dma_interrupt_message_num[unit]));
    } 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/**
 * Function fills the global memory "last_interrupt_message_num" 
 * with message from last interrupt message, taken directly from 
 * OAMP_INTERRUPT_MESSAGE register. 
 * 
 * @author sinai (10/11/2013)
 * 
 * @return uint32 
 */
uint32 arad_pp_oam_event_fifo_get_interrupt_mesage_register_internal(
   SOC_SAND_IN  int                                        unit) {
    uint32 res;
    soc_reg_above_64_val_t data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (last_interrupt_message_num[unit] == _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_INTERRUPT_MESSAGEr(unit, data));
        soc_reg_above_64_field_get(unit, OAMP_INTERRUPT_MESSAGEr, data, INTERRUPT_MESSAGEf, interrupt_message[unit]);
        last_interrupt_message_num[unit] = 0;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 arad_pp_oam_event_fifo_read_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
    SOC_SAND_OUT  uint32                                       *event_id,
    SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *event_buffer
  ) 
{
    uint32 res;
    SOC_PPD_OAM_EVENT_DATA event_data; 
    SOC_PPD_OAM_RX_REPORT_EVENT_DATA rx_report_data;
    uint32 field_32;
    uint32 empty_fifo_value;
    uint32 start_of_message;
    uint32 event_ndx;
    uint32 use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_fifo_dma;
    uint32 rmeb_db_ndx_lcl[1];
    uint32 (*fill_interrupt_message) (int) = (use_dma && SOC_IS_ARADPLUS(unit))?
            arad_pp_oam_event_fifo_get_interrupt_mesage_dma_internal:
            arad_pp_oam_event_fifo_get_interrupt_mesage_register_internal;

    uint32 report_mode = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_EVENT_FIFO_READ_UNSAFE);


    empty_fifo_value = SOC_IS_ARADPLUS(unit) ? 0xffffffff : 0xffffff;

    /* Empty bitmap */
    for (event_ndx=0; event_ndx<SOC_PPD_OAM_EVENT_COUNT; event_ndx++) {
        event_id[event_ndx]=0;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, fill_interrupt_message(unit));                                

    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "Reading from the interrupt message. here are the first few bytes: 0x%x%x%x%x%x%x%x\n"),interrupt_message[unit][0],interrupt_message[unit][1],interrupt_message[unit][2],
                          interrupt_message[unit][3],interrupt_message[unit][4],interrupt_message[unit][5],interrupt_message[unit][6]));

    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Here is the interruprt : %d \n"),interrupt_message[unit][0] ));


    if (interrupt_message[unit][last_interrupt_message_num[unit]] == empty_fifo_value || 
        (SOC_IS_ARAD_B1_AND_BELOW(unit) && interrupt_message[unit][last_interrupt_message_num[unit]] ==0) ) {
        *valid = 0;
        last_interrupt_message_num[unit] = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;
    } else {   
        *valid = 1;
        *rmeb_db_ndx_lcl = 0;
        start_of_message = last_interrupt_message_num[unit]*32;
        SHR_BITCOPY_RANGE(rmeb_db_ndx_lcl, 0, interrupt_message[unit], start_of_message, 16);
        *rmeb_db_ndx = *rmeb_db_ndx_lcl;

        if (SOC_IS_JERICHO(unit)) {
            /* Check if the message is a LM/DM report */
            SHR_BITCOPY_RANGE(&report_mode, 0, interrupt_message[unit], start_of_message + 21, 2);

        }

        if (!report_mode) {
            if (SOC_IS_ARADPLUS(unit)) {
                SOC_PPD_OAM_EVENT_DATA_clear(&event_data);
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+16, 2);
                event_data.loc = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+18, 1);
                event_data.rdi_set = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+19, 1);
                event_data.rdi_clear = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+20, 1);
                event_data.rmep_state_change = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&(event_data.rmep_state), 0, interrupt_message[unit], start_of_message+23, 9);
                event_buffer[0] = event_data.rmep_state;
                if (event_data.loc == 1) {
                    event_id[SOC_PPD_OAM_EVENT_LOC_SET] = 1;
                }
                else if (event_data.loc == 2) {
                    event_id[SOC_PPD_OAM_EVENT_ALMOST_LOC_SET] = 1;
                }
                else if (event_data.loc == 3) {
                    event_id[SOC_PPD_OAM_EVENT_LOC_CLR] = 1;
                }
                if (event_data.rdi_set) {
                    event_id[SOC_PPD_OAM_EVENT_RDI_SET] = 1;
                }
                else if (event_data.rdi_clear) {
                    event_id[SOC_PPD_OAM_EVENT_RDI_CLR] = 1;
                }
                if (event_data.rmep_state_change) {
                    event_id[SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE] = 1;
                }
            }
            else {
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+16, 8);
                switch (field_32) {
                case SOC_PPD_OAM_EVENT_LOC_SET:
                    event_id[SOC_PPD_OAM_EVENT_LOC_SET] = 1;
                    break;
                case SOC_PPD_OAM_EVENT_ALMOST_LOC_SET:
                    event_id[SOC_PPD_OAM_EVENT_ALMOST_LOC_SET] = 1;
                    break;
                case SOC_PPD_OAM_EVENT_LOC_CLR:
                    event_id[SOC_PPD_OAM_EVENT_LOC_CLR] = 1;
                    break;
                case SOC_PPD_OAM_EVENT_RDI_SET:
                    event_id[SOC_PPD_OAM_EVENT_RDI_SET] = 1;
                    break;
                case SOC_PPD_OAM_EVENT_RDI_CLR:
                    event_id[SOC_PPD_OAM_EVENT_RDI_CLR] = 1;
                    break;
                }
            }
        }
        else {
            /* Jericho only - LM/DM report */
            if (report_mode != 2) {
                /* MEP performance event - not supported */
                *valid = 0;
                last_interrupt_message_num[unit] = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;
                LOG_DEBUG(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " MEP performance event received. Not supported.\n")));
                SOC_SAND_EXIT_NO_ERROR;
            }
            SOC_PPD_OAM_RX_REPORT_EVENT_DATA_clear(&rx_report_data);
            field_32 = 0;
            SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+16, 4);
            rx_report_data.event_size = field_32;
            field_32 = 0;
            SHR_BITCOPY_RANGE(&field_32, 0, interrupt_message[unit], start_of_message+20, 1);
            rx_report_data.event_type = (uint8)field_32;
            /* Extract the content according to size */
            SHR_BITCOPY_RANGE(event_buffer, 0, interrupt_message[unit], start_of_message + 32, 32 * (rx_report_data.event_size - 1));
            last_interrupt_message_num[unit] += rx_report_data.event_size;
            /* Set the event IDs */
            if (rx_report_data.event_type == 1 /* DM */) {
                if (rx_report_event_mode[unit] == SOC_PPC_OAM_REPORT_MODE_COMPACT) {
                    /* In compact mode, the MSB of the delay is in Event[0][31:23] */
                    event_buffer[1] = 0;
                    SHR_BITCOPY_RANGE(&event_buffer[1], 0, interrupt_message[unit], start_of_message+23, 31 - 23 + 1);
                }
                event_id[SOC_PPD_OAM_EVENT_REPORT_RX_DM] = 1;
            }
            else {
                if (rx_report_event_mode[unit] == SOC_PPC_OAM_REPORT_MODE_COMPACT) {
                    /* In compact mode, delta_FCb, delta_FCf is in {Event[0][31:16], Event[0][15:0]} respectively  */
                    event_buffer[1] = event_buffer[0] & 0xFFFF;
                    event_buffer[0] = (event_buffer[0] >> 16) & 0xFFFF;
                }
                event_id[SOC_PPD_OAM_EVENT_REPORT_RX_LM] = 1;
            }
        }

        last_interrupt_message_num[unit]++;
        LOG_DEBUG(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              " rmeb_db_ndx=%d\n"), *rmeb_db_ndx));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_event_fifo_read()", 0, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_SET_UNSAFE);
  
  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  tbl_data.oam_port_profile = oam_profile;

  res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core_id, local_port_ndx, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(oam_profile, 1, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_get_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  )
{
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;
  soc_error_t
    rv;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oam_profile);

  rv = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  *oam_profile = tbl_data.oam_port_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_get_unsafe()", local_port_ndx, 0);
}

/* ARAD+ functions */
/*********************************************************************
*     Set Bfd Diag Profile register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_diag_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       diag_profile
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_DIAG_PROFILEr(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPD_OAMP_BFD_DIAG_PROFILE_NOF_BITS*profile_ndx, &diag_profile, 0, SOC_PPD_OAMP_BFD_DIAG_PROFILE_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_DIAG_PROFILEr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_diag_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       diag_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, .., .., 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(diag_profile, .., .., 10, exit);*/
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_diag_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_OUT  uint32                                      *diag_profile
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(diag_profile);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_DIAG_PROFILEr(unit, reg_val));
  SHR_BITCOPY_RANGE(diag_profile, 0, reg_val, SOC_PPD_OAMP_BFD_DIAG_PROFILE_NOF_BITS*profile_ndx, SOC_PPD_OAMP_BFD_DIAG_PROFILE_NOF_BITS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_get_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_diag_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, .., .., 10, exit);*/
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Bfd Flags Profile register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_flags_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       flags_profile
  )
{
  uint32  res;
  uint64  reg_val;
  soc_reg_above_64_val_t  reg_above_64_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  COMPILER_64_ZERO(reg_val);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_FLADS_PROFILEr(unit, &reg_val));
  reg_above_64_val[0] = COMPILER_64_LO(reg_val);
  reg_above_64_val[1] = COMPILER_64_HI(reg_val);
  SHR_BITCOPY_RANGE(reg_above_64_val, SOC_PPD_OAMP_BFD_FLAGS_PROFILE_NOF_BITS*profile_ndx, &flags_profile, 0, SOC_PPD_OAMP_BFD_FLAGS_PROFILE_NOF_BITS);
  COMPILER_64_SET(reg_val, reg_above_64_val[1], reg_above_64_val[0]);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_FLADS_PROFILEr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_flags_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       flags_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, .., .., 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(flags_profile, .., .., 10, exit);*/
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_flags_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_OUT  uint32                                      *flags_profile
  )
{
  uint32  res;
  uint64  reg_val;
  soc_reg_above_64_val_t  reg_above_64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(flags_profile);
  COMPILER_64_ZERO(reg_val);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_FLADS_PROFILEr(unit, reg_val));
  reg_above_64_val[0] = COMPILER_64_LO(reg_val);
  reg_above_64_val[1] = COMPILER_64_HI(reg_val);
  SHR_BITCOPY_RANGE(flags_profile, 0, reg_above_64_val, SOC_PPD_OAMP_BFD_FLAGS_PROFILE_NOF_BITS*profile_ndx, SOC_PPD_OAMP_BFD_DIAG_PROFILE_NOF_BITS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_get_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_flags_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, .., .., 10, exit);*/
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 reg, field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  reg = 0;
  field = (uint32)enable;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, &reg));
  SHR_BITCOPY_RANGE(&reg, profile_ndx, &field, 0, 1); 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, reg)); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(enable, 1, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_get_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32 reg, field=0;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  reg = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, &reg)); 
  SHR_BITCOPY_RANGE(&field, 0, &reg, profile_ndx, 1); 
  *enable = (uint8)field;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_get_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint64 reg, field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  COMPILER_64_ZERO(reg);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PUNT_EVENT_HENDLINGr(unit, &reg));
  switch (profile_ndx) {
  case 0:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  case 1:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  case 2:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  case 3:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error\n")));
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_PUNT_EVENT_HENDLINGr(unit, reg));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPD_OAM_OAMP_NUMBER_OF_PUNT_PROFILES, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPD_OAM_OAMP_NUMBER_OF_PUNT_PROFILES, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_get_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint64 reg, field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  COMPILER_64_ZERO(reg);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PUNT_EVENT_HENDLINGr(unit, &reg));
  switch (profile_ndx) {
  case 0:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  case 1:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  case 2:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  case 3:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error\n")));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe()", profile_ndx, 0);
}

/**
 * Set the protection packet header. Format is PTCH+ITMH+PPH+ 
 * FHEI 
 * 
 * @author sinai (13/02/2014)
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32
  arad_pp_oam_oamp_protection_packet_header_set_unsafe(
      SOC_SAND_IN  uint32                   unit,
	  SOC_SAND_IN  uint32                                 dest_system_port,
	  SOC_SAND_IN  uint32                                 trap_id
  )
{
	uint32 res = SOC_SAND_OK, reg32=0;
	uint32 header_size =0;
	soc_reg_above_64_val_t      pkt_header, reg_data, reg_field;
	PPH_base pph_header = {0};
	soc_reg_above_64_val_t  data_above_64;
	uint64 reg64;
	int tc, dp, ssp;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	SOC_REG_ABOVE_64_CLEAR(pkt_header);
	SOC_REG_ABOVE_64_CLEAR(reg_data);
	SOC_REG_ABOVE_64_CLEAR(reg_field);

	SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_CPUPORTr(unit, data_above_64));

	/* In case these are ever configured...*/
	ssp = soc_reg_above_64_field32_get(unit,OAMP_CPUPORTr,data_above_64, CPU_PTCH_PP_SSPf);
	tc = soc_reg_above_64_field32_get(unit,OAMP_CPUPORTr,data_above_64, CP_UPORT_TCf);
	dp = soc_reg_above_64_field32_get(unit,OAMP_CPUPORTr,data_above_64, CP_UPORT_DPf);

	header_size += _insert_ptch2(pkt_header,1,_ARAD_PP_OAM_PTCH_OPAQUE_VALUE,ssp);
	header_size += _insert_itmh(pkt_header,0xc0000 | dest_system_port ,dp,tc ,0,0, 1 /*PPH type: 1 --> pph only*/  );
	/* fill the PPH header*/
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TX_PPHr(unit, &reg64));

	pph_header.fhei_size = 3; /* FHEI*/
	pph_header.fwd_code =_ARAD_PP_OAM_FWD_CODE_CPU;
	pph_header.fwd_header_offset = 0;
	pph_header.pkt_is_ctrl = 0;
	pph_header.snoop_cpu_code = 0;
	pph_header.inLif_orintation = soc_reg64_field32_get(unit, OAMP_TX_PPHr, reg64, TX_PPH_IN_LIF_ORIENTATIONf);
	pph_header.unknown_da_addr = soc_reg64_field32_get(unit, OAMP_TX_PPHr, reg64, TX_PPH_UNKNOWN_DAf);
	pph_header.learn_allowed = 0;
	pph_header.vsi_vrf = soc_reg64_field32_get(unit, OAMP_TX_PPHr, reg64, TX_PPH_VSI_OR_VRFf);
	pph_header.inLif_inRif = soc_reg64_field32_get(unit, OAMP_TX_PPHr, reg64, TX_PPH_IN_LIF_OR_IN_RIFf); 
	header_size += _insert_pph(pkt_header, &pph_header, header_size);

	header_size += _insert_fhei(pkt_header, 0, trap_id, header_size );

	SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, WRITE_OAMP_PROTECTION_PACKET_HEADERr(unit,pkt_header));

	soc_reg_field_set(unit,OAMP_PROTECTION_PACKET_CONFr,&reg32, PROTECTION_PACKET_HEADER_NUM_OF_BYTESf, 64 /* Final packet size will be 64 bytes (PTCH, ITMH will be replace by FTMH*/  );
	/* edit program field remains blank.*/
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, WRITE_OAMP_PROTECTION_PACKET_CONFr(unit,reg32));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  uint32                                 dest_system_port
  )
{
  soc_reg_above_64_val_t  data_above_64, dest_above_64, trap_above_64;
  uint32
    res = SOC_SAND_OK;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  SOC_REG_ABOVE_64_CLEAR(dest_above_64);
  SOC_REG_ABOVE_64_CLEAR(trap_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_CPUPORTr(unit, data_above_64));

  dest_above_64[0] = 0xc0000 | dest_system_port;
  trap_above_64[0] = trap_id;

  switch (trap_type) {
  case SOC_PPD_OAM_OAMP_TRAP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TRAP_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TRAP_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_TYPE_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TYPE_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TYPE_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_RMEP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_EMC_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_EMC_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_MAID_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MAID_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MAID_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_MDL_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MDL_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MDL_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_CCM_INTERVAL_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_CCM_INTRVL_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_CCM_INTRVL_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_MY_DISC_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_MY_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_MY_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_SRC_IP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_SRC_IP_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_SRC_IP_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_YOUR_DISC_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_YOUR_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_YOUR_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_SRC_PORT_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_SRC_PORT_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_SRC_PORT_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_RMEP_STATE_CHANGE:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_RMEP_STATE_CHANGEf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_RMEP_STATE_CHANGEf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_PARITY_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_PARITY_ERRORf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_PARITY_ERRORf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_TIMESTAMP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TIMESTAMP_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TIMESTAMP_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_PROTECTION:
	  /* handled seperatly*/
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, arad_pp_oam_oamp_protection_packet_header_set_unsafe(unit, dest_system_port ,trap_id));
      break;  
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  }
    
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_OAMP_CPUPORTr(unit, data_above_64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe()", trap_type, 0);
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  uint32                                 dest_system_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  } 
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_id, 255, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type, SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dest_system_port, (1<<20)/*field lenght*/, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_set_verify()", trap_type, 0);
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  uint32                                *dest_system_port
  )
{
  soc_reg_above_64_val_t  data_above_64, dest_above_64, trap_above_64;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  SOC_REG_ABOVE_64_CLEAR(dest_above_64);
  SOC_REG_ABOVE_64_CLEAR(trap_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_CPUPORTr(unit, data_above_64));

  switch (trap_type) {
  case SOC_PPD_OAM_OAMP_TRAP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TRAP_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TRAP_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_TYPE_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TYPE_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TYPE_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_RMEP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_EMC_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_EMC_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_MAID_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MAID_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MAID_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_MDL_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MDL_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MDL_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_CCM_INTERVAL_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_CCM_INTRVL_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_CCM_INTRVL_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_MY_DISC_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_MY_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_MY_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_SRC_IP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_SRC_IP_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_SRC_IP_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_YOUR_DISC_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_YOUR_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_YOUR_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_SRC_PORT_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_SRC_PORT_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_SRC_PORT_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_RMEP_STATE_CHANGE:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_RMEP_STATE_CHANGEf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_RMEP_STATE_CHANGEf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_PARITY_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_PARITY_ERRORf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_PARITY_ERRORf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_TIMESTAMP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TIMESTAMP_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TIMESTAMP_MISSf, trap_above_64);   
      break;  
  case SOC_PPD_OAM_OAMP_PROTECTION:
         
      break;  
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  }
    
  *dest_system_port = dest_above_64[0];
  *trap_id = trap_above_64[0];
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe()", trap_type, 0);
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  } 
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type, SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_get_verify()", trap_type, 0);
}


/*********************************************************************
*     Manage LM, Dm functionalities.
*     Details: in the H file. (search for prototype)
*********************************************************************/



/**
 * Copies one entry in the OAMP MEP DB to another location. 
 * Also removes the entry from its original location.
 * 
 * @param unit 
 * @param src 
 * @param dst 
 * 
 * @return uint32 
 */
uint32 
    arad_pp_oam_move_entry_in_oamp_mep_db_internal(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 src,
    SOC_SAND_IN  uint32                                 dst
       )
{
    soc_reg_above_64_val_t entry;
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,src, entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, dst, entry));

    CLEAR_OAMP_MEP_DB_ENTRY(src, entry);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/**
 * Function clears all fileds in a oamp mep db entry.
 * MEP TYPE remains as is.
 * 
 * @param unit 
 * @param entry_id - entry number to be cleared
 * 
 * @return uint32 -error or success.
 */
uint32
   arad_pp_reset_oamp_mep_db_entry_internal(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                             entry_id
      )
{
    uint32 res = SOC_SAND_OK;
    uint32 mep_type;
    soc_reg_above_64_val_t reg_data;
  
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, entry_id,reg_data);
    
    soc_OAMP_MEP_DB_DM_STATm_field32_set(unit, reg_data, MEP_TYPEf, mep_type);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, entry_id, reg_data)); 



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/**
 * Function writes to the DA NIC table and the DA OUI  register, 
 * according to given profile. If profile is -1 function does 
 * not write. 
 * 
 * @author sinai (02/09/2013)
 * 
 * @param unit 
 * @param lsb_to_nic 
 * @param msb_to_oui 
 * @param nic_profile - Must be between 0 and 127, or -1 to 
 *                    disable write.
 * @param oui_profile Must be between 0 and 7, or -1 to disable 
 *                    write.
 * 
 * @return uint32 
 */
uint32
    arad_pp_oam_oamp_write_to_nic_and_oui_tables_internal(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                             lsb_to_nic,
      SOC_SAND_IN  uint32                             msb_to_oui,
      SOC_SAND_IN  int                             nic_profile,
      SOC_SAND_IN  int                             oui_profile
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg32_data, reg32_field;
    soc_reg_t oui_fields[]  = {LMM_DA_OUI_0f,LMM_DA_OUI_1f,LMM_DA_OUI_2f,LMM_DA_OUI_3f,LMM_DA_OUI_4f,LMM_DA_OUI_5f,LMM_DA_OUI_6f,LMM_DA_OUI_7f};
    soc_reg_above_64_val_t reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    /*First, write into the da nic/out registers*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_LMM_DA_OUIr(unit, reg_data));
    reg_field[0] = msb_to_oui;
    soc_reg_above_64_field_set(unit, OAMP_LMM_DA_OUIr, reg_data, oui_fields[oui_profile], reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_LMM_DA_OUIr(unit, reg_data));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_LMM_DA_NIC_TABLEm(unit, MEM_BLOCK_ANY, nic_profile, &reg32_data));
    reg32_field = lsb_to_nic;
    soc_OAMP_LMM_DA_NIC_TABLEm_field_set(unit, &reg32_data, LMM_DA_NICf, &reg32_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_LMM_DA_NIC_TABLEm(unit, MEM_BLOCK_ALL, nic_profile, &reg32_data)); 


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/**
 * Function simply writes on the eth-1731 mep profile, according 
 * to the parameters. 
 * 
 * @author sinai (02/09/2013)
 * 
 * @param unit 
 * @param profile_indx  - Must be between 0 and 15.
 * @param eth1731_profile 
 * 
 * @return uint32 
 */
uint32
    arad_pp_oam_oamp_write_eth1731_profile_internal(
       SOC_SAND_IN  int                                              unit,
       SOC_SAND_IN  uint8                             profile_indx,
       SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY      *eth1731_profile
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    soc_reg_t eth_1731_field;
    soc_reg_t offset_field; 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,READ_OAMP_ETH_1731_MEP_PROFILEr(unit, reg_data));

    reg_field[0] = eth1731_profile->lmm_rate;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_LMM_RATE(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->dmm_rate;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_DMM_RATE(eth_1731_field,profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);


    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->piggy_back_lm;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_PIGGY_BACK(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->lmm_da_oui_prof;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_DA_OUI_PROF(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->rdi_gen_method;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_RDI_GEN_METHOD(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_OAMP_ETH_1731_MEP_PROFILEr(unit, reg_data)); 

    /* Write the offset as well since this register uses the same profile as the eth1731 profile*/
    SOC_REG_ABOVE_64_CLEAR(reg_data); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,READ_OAMP_OAM_TS_OFFSET_PROFILEr(unit, reg_data));

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMM_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile -> dmm_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "dmm offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMR_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile -> dmr_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "dmr offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMM_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile->lmm_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "lmm offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMR_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile->lmr_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "lmr offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,WRITE_OAMP_OAM_TS_OFFSET_PROFILEr(unit, reg_data));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_lm_dm_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 reg32_field;
    soc_mem_t mem;
    SOC_PPD_OAM_MEP_TYPE base_mep_type;
    
        
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data); 
    SOC_REG_ABOVE_64_CLEAR(reg_field); 


    /* First, make the changes on the original CCM entry. - only the mep pe profile for 1DM*/
    if (mep_db_entry->is_1DM) {
        GET_CCM_ENTRY( mep_db_entry->mep_id,  reg_data, mem);
        arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_1DM,&reg32_field);
        if (reg32_field ==-1) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("1DM OAMP PE program not loaded.")));
        }
        soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,soc_mem_write(unit,mem,MEM_BLOCK_ALL,mep_db_entry->mep_id, reg_data));
    } else {
        if (mep_db_entry->is_piggyback_down && SOC_IS_ARADPLUS_A0(unit) ) {
            reg32_field = 0;
            /* In this case the mep-pe profile should be 0 so as not to select the UP MEP MAC program */
            GET_CCM_ENTRY(mep_db_entry->mep_id,  reg_data, mem);
            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, soc_mem_write(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, reg_data));
        }
    }


    if (mep_db_entry->is_update==0) {
        /*Now, write on the omap mep db (only the type)*/
        /*First, if the entry after the CCM is of type DM then that entry has got to move.*/
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(base_mep_type, mep_db_entry->mep_id +1 ,reg_data);
        if (base_mep_type== SOC_PPD_OAM_MEP_TYPE_DM) { /*DM has to be last entry for a MEP, so if the base is a DM then it has got to move down.*/
            int offset = (mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT)? 2:1;
            SOC_REG_ABOVE_64_CLEAR(reg_data); 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, 
                                         arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit,mep_db_entry->mep_id +1,mep_db_entry->mep_id +1 + offset));
        }

        SOC_REG_ABOVE_64_CLEAR(reg_data); 
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        if (mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_DM) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, READ_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ANY, mep_db_entry->allocated_id  , reg_data)); /*may be after CCM or other type of entry.*/
            reg_field[0] = SOC_PPD_OAM_MEP_TYPE_DM;
            soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MEP_TYPEf, reg_field );
            reg_field[0] =0xffffffff;
            reg_field[1] = 0x3ff; /* Filling 42 bit field to maximum value.*/
            soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MIN_DELAYf, reg_field );
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, WRITE_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ALL, mep_db_entry->allocated_id , reg_data));
        }  else {/*LM entry type*/
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, READ_OAMP_MEP_DB_LM_DBm(unit, MEM_BLOCK_ANY, mep_db_entry->mep_id + 1, reg_data)); /*always directly after the CCM entry*/
            reg_field[0] = SOC_PPD_OAM_MEP_TYPE_LM; 
            soc_OAMP_MEP_DB_LM_DBm_field_set(unit,reg_data, MEP_TYPEf, reg_field );
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DB_LM_DBm(unit, MEM_BLOCK_ALL, mep_db_entry->mep_id + 1, reg_data)); 
            if (mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT) { /*Need another entry*/
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, READ_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ANY, mep_db_entry->mep_id + 2, reg_data));
                reg_field[0] = SOC_PPD_OAM_MEP_TYPE_LM_STAT;
                soc_OAMP_MEP_DB_LM_DBm_field_set(unit, reg_data, MEP_TYPEf, reg_field);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL, mep_db_entry->mep_id + 2, reg_data));
            }
        }
    } else if (mep_db_entry->entry_type != SOC_PPD_OAM_LM_DM_ENTRY_TYPE_DM) { /*update*/
        /* The only thing that can change is adding/removing LM STAT entry.*/
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(base_mep_type, mep_db_entry->mep_id + 2, reg_data); /* If LM stat is included it is only in this slot.*/
        if (base_mep_type == SOC_PPD_OAM_MEP_TYPE_LM_STAT && mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM) {
            /* LM stat is to be removed*/
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(base_mep_type, mep_db_entry->mep_id + 3, reg_data);
            if (base_mep_type == SOC_PPD_OAM_MEP_TYPE_DM) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 43, exit,
                                             arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit, mep_db_entry->mep_id + 3, mep_db_entry->mep_id + 2));
            } else {
                CLEAR_OAMP_MEP_DB_ENTRY(mep_db_entry->mep_id + 2, reg_data);
            }
        } else if (base_mep_type != SOC_PPD_OAM_MEP_TYPE_LM_STAT && mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
            /* LM stat is to be added*/
            if (base_mep_type == SOC_PPD_OAM_MEP_TYPE_DM) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 43, exit,
                                             arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit, mep_db_entry->mep_id + 2, mep_db_entry->mep_id + 3));
            }
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            reg_field[0] = SOC_PPD_OAM_MEP_TYPE_LM_STAT;
            soc_OAMP_MEP_DB_LM_STATm_field_set(unit, reg_data, MEP_TYPEf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL, mep_db_entry->mep_id + 2, reg_data));
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_lm_dm_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);
    SOC_SAND_MAGIC_NUM_VERIFY(mep_db_entry);


    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_lm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_LM_INFO_GET     *lm_info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(lm_info);
    SOC_SAND_MAGIC_NUM_VERIFY(lm_info);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

    

uint32
  arad_pp_oam_oamp_lm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_LM_INFO_GET     *lm_info
  )
{
    soc_reg_above_64_val_t reg_data;
    uint32 res = SOC_SAND_OK;
    uint32 type;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,12,exit,READ_OAMP_MEP_DB_LM_DBm(unit,MEM_BLOCK_ANY, lm_info->entry_id +1,reg_data));
    type = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data, MEP_TYPEf) ;

    if ( type != SOC_PPD_OAM_MEP_TYPE_LM ) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No LM entry for endpoint %d"), lm_info->entry_id));
    }

    lm_info->my_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,MY_TXf);
    lm_info->my_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,MY_RXf);
    lm_info->peer_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,PEER_TXf);
    lm_info->peer_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,PEER_RXf);


    /*Now see if extended statistics are available.*/

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, READ_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ANY, lm_info->entry_id + 2, reg_data));

    if (soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, MEP_TYPEf) == SOC_PPD_OAM_MEP_TYPE_LM_STAT) {
        lm_info->is_extended = 1;
        lm_info->last_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,LAST_LM_NEARf);
        lm_info->last_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,LAST_LM_FARf);
        lm_info->acc_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,ACC_LM_NEARf);
        lm_info->acc_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,ACC_LM_FARf);
        lm_info->max_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,MAX_LM_NEARf);
        lm_info->max_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,MAX_LM_FARf);
        /* acc/max fields are to be reset when read */
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, MAX_LM_NEARf , 0);                
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, MAX_LM_FARf , 0);                
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, ACC_LM_NEARf , 0);                
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, ACC_LM_FARf , 0);        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL, lm_info->entry_id + 2, reg_data));

    } else {
        lm_info->is_extended =0;
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_dm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(dm_info);

    SOC_SAND_MAGIC_NUM_VERIFY(dm_info);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_dm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  )
{
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 offset, type, oneDM_program;
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    /*first, find delay entry.*/
    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(type, dm_info->entry_id + 1, reg_data);
    if (type == SOC_PPD_OAM_MEP_TYPE_DM) {
        offset = 1;
    } else if (type == SOC_PPD_OAM_MEP_TYPE_LM)  { /*strike one*/
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(type, dm_info->entry_id + 2, reg_data);
            if (type == SOC_PPD_OAM_MEP_TYPE_DM) {
                offset = 2;
            } else if  (type == SOC_PPD_OAM_MEP_TYPE_LM_STAT) { /*strike 2*/ 
                GET_TYPE_OF_OAMP_MEP_DB_ENTRY(type, dm_info->entry_id + 3, reg_data);
                if (type == SOC_PPD_OAM_MEP_TYPE_DM) {
                    offset = 3;
                } else { /*three strikes - you're out!*/
                    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No DM entry for endpoint %d "),dm_info->entry_id)); 
                    }
            } else {
                SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No DM entry for endpoint %d "),dm_info->entry_id));
            }
    } else {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No DM entry for endpoint %d "),dm_info->entry_id));
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res,12,exit,READ_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ANY, dm_info->entry_id + offset,reg_data));

    if (soc_OAMP_MEP_DB_DM_STATm_field32_get(unit, reg_data, MEP_TYPEf) != SOC_PPD_OAM_MEP_TYPE_DM) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error: profile index incorrect for DM ")));
    }

    soc_OAMP_MEP_DB_DM_STATm_field_get(unit,reg_data,LAST_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->last_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->last_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STATm_field_get(unit,reg_data,MAX_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->max_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->max_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STATm_field_get(unit,reg_data,MIN_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->min_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->min_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field); 
    soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MAX_DELAYf, reg_field );
    reg_field[0] =0xffffffff;
    reg_field[1] = 0x3ff; /* Filling 42 bit field to maximum value.*/
    soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MIN_DELAYf, reg_field );

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, WRITE_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ALL, dm_info->entry_id + offset , reg_data));

    /* Now return is_1DM*/
    arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_1DM,&oneDM_program);
    GET_CCM_ENTRY( dm_info->entry_id,  reg_data, type);
    *is_1dm =  (soc_mem_field32_get(unit, type, reg_data, MEP_PE_PROFILEf) == oneDM_program);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_next_index_get, 
    More details in header file. 
*********************************************************************/
uint32
  arad_pp_oam_oamp_next_index_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  )
{
  uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(next_index);
    SOC_SAND_CHECK_NULL_INPUT(has_dm);
    
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_next_index_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  )
{
        soc_reg_above_64_val_t reg_data;
    uint32 res = SOC_SAND_OK;
    uint32  mep_type, next_type;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+1, reg_data);

    if (mep_type == 0) {
        *next_index = endpoint_id+1; /*This is a CCM entry or this is free. Either way this is what we return.*/
        *has_dm = 0;
    } else if (mep_type == SOC_PPD_OAM_MEP_TYPE_DM) {
        *next_index =endpoint_id+2; /*next index may be taken, but this is the best we can do here*/ 
        *has_dm = 1;
    } else if (mep_type == SOC_PPD_OAM_MEP_TYPE_LM) {
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+2, reg_data);
        if (mep_type ==SOC_PPD_OAM_MEP_TYPE_LM_STAT ) {
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(next_type, endpoint_id+3, reg_data);
            if ( next_type ==SOC_PPD_OAM_MEP_TYPE_DM ) {
              *next_index =endpoint_id+4; 
              *has_dm = 1;
            } else {
              *next_index =endpoint_id+3; 
              *has_dm = 0;
            }
        } else {
            if (mep_type == SOC_PPD_OAM_MEP_TYPE_DM) {
                *next_index = endpoint_id + 3;
                *has_dm = 1;
            } else {
                *next_index = endpoint_id + 2;
                *has_dm = 0;
            }
        }
    } else {
        *next_index = 0; 
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_eth1731_and_oui_profiles_get, 
 more details in the header file.
*********************************************************************/


uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  )
{
      uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(eth1731_prof);
    SOC_SAND_CHECK_NULL_INPUT(da_oui_prof);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  )
{
       uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    int nic_profile;
    soc_reg_t lmm_da_oui_prof_field;
    uint32 mep_type;
        
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    COMPILER_REFERENCE(nic_profile);
    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id, reg_data);
    GET_MEP_AND_DA_NIC_PROFILES_FROM_OAMP_MEP_DB_ENTRY(*eth1731_prof, nic_profile,  mep_type, endpoint_id, reg_data);

    if (mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
        /* we must also get the OUI*/
        if (SOC_IS_JERICHO(unit)) {
            SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth_1731_prof_entry={0};
            res = soc_jer_pp_oam_oamp_eth1731_profile_get(unit,*eth1731_prof,&eth_1731_prof_entry );
            SOC_SAND_SOC_IF_ERROR_RETURN(res,2322,exit,res);
            *da_oui_prof = eth_1731_prof_entry.lmm_da_oui_prof;
        } else {
            SOC_REG_ABOVE_64_CLEAR(reg_data); 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ETH_1731_MEP_PROFILEr(unit, reg_data)); 
            GET_OAMP_ETH_1731_FIELD_BY_INDEX_DA_OUI_PROF(lmm_da_oui_prof_field, *eth1731_prof);
            soc_reg_above_64_field_get(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, lmm_da_oui_prof_field, reg_field);
            *da_oui_prof = reg_field[0];
        }
    }
        
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/***************************************** 
 arad_pp_oam_oamp_nic_profile_get
  ********************************************/


uint32
    arad_pp_oam_oamp_nic_profile_get_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       )
{
      uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(da_nic_prof);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
    arad_pp_oam_oamp_nic_profile_get_unsafe(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(da_nic_prof);


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,endpoint_id, reg_data));

    *da_nic_prof = soc_OAMP_MEP_DBm_field32_get(unit, reg_data, LMM_DA_NIC_PROFILEf);


    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/******************************************************************* 
  *       find LM, DM entries
  *  
  *****************************************************************/

uint32
    arad_pp_oam_oamp_search_for_lm_dm_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               * found_bitmap
       )
{
      uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(found_bitmap);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
    arad_pp_oam_oamp_search_for_lm_dm_unsafe(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               * found_bitmap
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    uint32  mep_type;
    uint32 cur=endpoint_id+1 ;
    uint32 found_bitmap_lcl[1];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found_bitmap_lcl=0;


    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, cur, reg_data);
    while (mep_type == SOC_PPD_OAM_MEP_TYPE_LM || mep_type==SOC_PPD_OAM_MEP_TYPE_DM
           || mep_type == SOC_PPD_OAM_MEP_TYPE_LM_STAT) {
        ++cur;
        SHR_BITSET(found_bitmap_lcl,mep_type );
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, cur, reg_data);
    }
    *found_bitmap = *found_bitmap_lcl;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




/*********************************************************************
*     change eth1731 profiles
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    SOC_SAND_CHECK_NULL_INPUT(eth1731_profile);
    if (profile_indx > SOC_PPC_OAM_MAX_NUMBER_OF_ETH1731_MEP_PROFILES(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Profile index must be between 0 and %d."),SOC_PPC_OAM_MAX_NUMBER_OF_ETH1731_MEP_PROFILES(unit) ));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    uint32 res = SOC_SAND_OK, reg32_field;
    soc_reg_above_64_val_t reg_data;
    soc_mem_t mem;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /*update profile pointer in the mep db entry and then the profile itself.*/
    GET_CCM_ENTRY(endpoint_id ,  reg_data, mem);
    reg32_field = profile_indx; 
    soc_mem_field_set(unit, mem, reg_data, MEP_PROFILEf, &reg32_field);

    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,soc_mem_write(unit,mem,MEM_BLOCK_ALL,endpoint_id, reg_data));

    if (!was_previously_alloced) {
        if (SOC_IS_JERICHO(unit)) {
            res = soc_jer_pp_oam_oamp_eth1731_profile_set(unit,profile_indx, eth1731_profile);
            SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222,exit,res);
        } else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 452, exit, arad_pp_oam_oamp_write_eth1731_profile_internal(unit, profile_indx,  eth1731_profile)); 
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
 *                                   OUI, NIC PROFILES
 **********************************************************************/

uint32
  arad_pp_oam_oamp_set_oui_nic_registers_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}

uint32
  arad_pp_oam_oamp_set_oui_nic_registers_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit,
                                 arad_pp_oam_oamp_write_to_nic_and_oui_tables_internal(unit, lsb_to_nic, msb_to_oui,
                                                                                       profile_indx_nic, profile_indx_oui));
    SOC_REG_ABOVE_64_CLEAR(reg_data);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, reg_data));
    soc_OAMP_MEP_DBm_field32_set(unit, reg_data, LMM_DA_NIC_PROFILEf,profile_indx_nic );
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, endpoint_id, reg_data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
*     Remove LM, DM entries
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_oam_oamp_lm_dm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    SOC_SAND_CHECK_NULL_INPUT(num_removed);
    SOC_SAND_CHECK_NULL_INPUT(removed_index);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_lm_dm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  )
{
    soc_reg_above_64_val_t   entry, reg_field;
    uint32 mep_type, next_type;
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+1,entry);

    /* Traverse the next 3 MEP_DB entries, clear what needs clearing and find out what needs removing.*/
    if (mep_type== SOC_PPD_OAM_MEP_TYPE_DM) { 
        /*We may safely assume that only a DM entry needs removing.*/
        CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id + 1, entry);
        *num_removed = 1;
        *removed_index = endpoint_id + 1; 
    } else if (mep_type == SOC_PPD_OAM_MEP_TYPE_LM) {
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+2,entry);
        /* In this case the treatment of lm/dm acts differntly, even though the internal ifs are the same. Treat each case seperatly*/
        if (is_lm) { /* cases are: LM, DM ; LM,LM-STAT,DM ; LM,LM-STAT, LM*/
            if (mep_type== SOC_PPD_OAM_MEP_TYPE_DM) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 12,exit,arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit,endpoint_id + 2, endpoint_id+1));
                *num_removed  =1;
                *removed_index = endpoint_id+2;
            }
            else if (mep_type== SOC_PPD_OAM_MEP_TYPE_LM_STAT) {
                GET_TYPE_OF_OAMP_MEP_DB_ENTRY(next_type, endpoint_id+3,entry);
                *num_removed  = 2;
                if (next_type == SOC_PPD_OAM_MEP_TYPE_DM) {
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit, endpoint_id + 3, endpoint_id + 1));
                    *removed_index = endpoint_id+2;
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +2,entry);
                } else {/*Only LM followed by LM STAT associated with this EP.*/
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +1,entry);
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +2,entry);
                    *removed_index = endpoint_id+1;
                }
            } else { /*only an LM for this EP.*/
                *num_removed  = 1;
                *removed_index = endpoint_id+1;
                CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id + 1, entry); 
            }
        } else { /*is_dm*/
            *num_removed  =1;
            /* only possible cases are: LM, DM ; LM,LM-STAT,DM */
            if (mep_type== SOC_PPD_OAM_MEP_TYPE_DM) {
                CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +2,entry);
                *removed_index = endpoint_id+2;
            }
            else if (mep_type== SOC_PPD_OAM_MEP_TYPE_LM_STAT) {
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +3,entry);
                    *removed_index = endpoint_id+3;
                } 
            }
    }

    if (!is_lm) { /* Reset the MEP_PE_PROFILE*/
        uint32 mep_pe_prof=0;
        SOC_REG_ABOVE_64_CLEAR(entry);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry));
        if (SOC_IS_ARADPLUS_A0(unit) && !exists_piggy_back_down) {
            /* For Arad, the MEP profile should this one:*/
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX, &mep_pe_prof); 
        }
        soc_OAMP_MEP_DBm_field_set(unit, entry, MEP_PE_PROFILEf, &mep_pe_prof);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry)); 
    }
  
    /*Clear a bunch of registers that need clearing. First find out which ones.*/
    SOC_REG_ABOVE_64_CLEAR(entry); 
    SOC_REG_ABOVE_64_CLEAR(reg_field); 

    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id, entry);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


soc_error_t arad_pp_oam_oamp_1dm_set(
   int unit,
   int endpoint_id,
   uint8 use_1dm)
{
    int rv;
   soc_reg_above_64_val_t   entry;
   uint32 pe_prof=0;
    SOCDNX_INIT_FUNC_DEFS;


    SOC_REG_ABOVE_64_CLEAR(entry);
    rv = READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv);


    if (use_1dm) {
        arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_1DM,&pe_prof);
    }
    soc_OAMP_MEP_DBm_field32_set(unit, entry, MEP_PE_PROFILEf, pe_prof) ;

    rv =  WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,endpoint_id, entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

   

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t arad_pp_oam_oamp_1dm_get(
   int unit,
   int endpoint_id,
   uint8 *use_1dm)
{
    int rv;
    soc_reg_above_64_val_t   entry;
    uint32 one_dm_pe_prof=0;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entry);
    rv = READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv); 

    arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_1DM,&one_dm_pe_prof);

    *use_1dm = soc_OAMP_MEP_DBm_field32_get(unit,entry,MEP_PE_PROFILEf ) == one_dm_pe_prof;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_oamp_pe_use_1dm_check(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              endpoint_id
  )
{
    uint32  res;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry = {0};
    uint32 whatever;
    SOCDNX_INIT_FUNC_DEFS;

    /* Read MEP DB entry */

    res = arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(unit, endpoint_id, &whatever, &mep_db_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res); 

    /* First, check that the TLV is unused.*/
    if (mep_db_entry.port_status_tlv_en || mep_db_entry.interface_status_tlv_control ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE,(_BSL_SOCDNX_MSG("Feauture unavailable together with Port/Interface TLV")));
    }

    if (mep_db_entry.is_upmep) {
        /* In this case verify that the LSB of the src MAC address indded matches the local port.
           The Counter pointer holds the LSB of the lsrc mac address, shifted by one.*/
        if (mep_db_entry.local_port != (mep_db_entry.counter_pointer >>1) ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE,(_BSL_SOCDNX_MSG("LSB of the src mac address must match the local port (both set in endpoint create)")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*   arad_pp_oam_classifier_counter_disable_map_set,
*   Set classifier counter disable map
*   More details in header file. 
*********************************************************************/
uint32
  arad_pp_oam_classifier_counter_disable_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  if (!SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(profile, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(packet_is_oam, 1, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(counter_enable, 1, ARAD_PP_OAM_INTERNAL_ERROR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_classifier_counter_disable_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg;
    uint32 map_key;
    uint32 field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_MAPr(unit, 0, &reg));
    map_key = profile + (packet_is_oam << 4);
    field_val = (uint32)counter_enable;
    SHR_BITCOPY_RANGE(&reg, map_key, &field_val, 0, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,20,exit,WRITE_IHP_OAM_COUNTER_DISABLE_MAPr(unit, SOC_CORE_ALL, reg));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/************************* 
 
  Counter disable map get
  ***************************/

uint32
  arad_pp_oam_classifier_counter_disable_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  if (!SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(profile, SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(packet_is_oam, 1, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_classifier_counter_disable_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg;
    uint32 map_key;
    uint32 field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_MAPr(unit, 0, &reg));
    map_key = profile + (packet_is_oam << 4);
    SHR_BITCOPY_RANGE(&field_val, 0, &reg, map_key, 1);
    *counter_enable = (uint8)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/************************************* 
        Loopback set
  *************************************/


uint32 
    arad_pp_oam_oamp_loopback_set_verify(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_IN uint32                                                     endpoint_id,
       SOC_SAND_IN  uint8                                                      tx_period,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *mac_address,
       SOC_SAND_OUT uint8                                                   *is_added
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(is_added);
    SOC_SAND_CHECK_NULL_INPUT(mac_address);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only used only on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



/**
 * Enters the PTCH2 header into pkt_header_reg. PTCH2 is always 
 * first, that is in bits 639:624  of pkt_header_reg 
 * Function always succeeds.
 * @author sinai (20/10/2013)
 * 
 * 
 * @return int -Size of PTCH2 in bytes
 */
int _insert_ptch2(soc_reg_above_64_val_t pkt_header_reg, int next_header_is_itmh, int opaque_value, int pp_ssp) {
    INSERT_INTO_REG(pkt_header_reg, 0,2, 0,8,pp_ssp);
    INSERT_INTO_REG(pkt_header_reg, 0,2, 12,3,opaque_value);
    INSERT_INTO_REG(pkt_header_reg, 0,2, 15,1,!next_header_is_itmh);
    return 2;
}

/**
 * Enters ITMH into pkt_header_reg. ITMH will always follow 
 * PTCH2, that is in bits  623-591
 * Function always succeds.
 * @author sinai (20/10/2013)
 * 
 * 
 * @return int Size of ITMH
 */
int _insert_itmh(soc_reg_above_64_val_t pkt_header_reg, int fwd_dst_info , int fwd_dp, int fwd_traffic_class, int snoop_cmd,int  in_mirr_flag,int pph_type) {
    INSERT_INTO_REG(pkt_header_reg, 2,4, 0,20,fwd_dst_info);
    INSERT_INTO_REG(pkt_header_reg, 2,4, 20,2,fwd_dp);
    INSERT_INTO_REG(pkt_header_reg, 2,4, 22,3,fwd_traffic_class);
    INSERT_INTO_REG(pkt_header_reg, 2,4, 25,4,snoop_cmd);
    INSERT_INTO_REG(pkt_header_reg, 2,4, 30,2,pph_type);
    return 4;
}

/**
 * Inserts ITMH ingress destination info extension. 
 * Always follows ITMH (nturally). 
 * 
 * @author sinai (23/10/2013)
 * 
 * @param pkt_header_reg 
 * 
 * @return int 
 */
int _insert_itmh_ing_dest_info_extension(soc_reg_above_64_val_t pkt_header_reg,int fwd_dst_info){
    INSERT_INTO_REG(pkt_header_reg, 6,3, 0,20,fwd_dst_info);
    return 3;
}

/**
 * Enters PPH base into pkt_header_reg. Whenever PPH appears it 
 * follows the PTCH-2 and the ITMH. 
 *  
 * PPH_FHEI_extensions field is optional. 
 *  
 * @author sinai (20/10/2013) 
 *  
 * @return int - size of PPH includeing extensions.
 */
int _insert_pph(soc_reg_above_64_val_t pkt_header_reg,const PPH_base * pph_fields, int offset) {
    /* missing fields are always zero anyways.*/
    INSERT_INTO_REG(pkt_header_reg,offset,7, 52, 2 ,pph_fields->fhei_size);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 48, 4 ,pph_fields->fwd_code);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 41, 7, pph_fields->fwd_header_offset);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 40, 1, pph_fields->pkt_is_ctrl);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 38, 2, pph_fields->snoop_cpu_code);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 36, 2, pph_fields->inLif_orintation);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 35, 1, pph_fields->unknown_da_addr);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 34, 1, pph_fields->learn_allowed);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 18, 16, pph_fields->vsi_vrf);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 0, 18, pph_fields->inLif_inRif);

    return 7;
}

/**
 * Enters the ETH header + vlan tag into pkt_header_reg. Format 
 * is etiher PTCH2oETh or PTCH2oITMHoETH , depending on 
 * parameters. 
 * 
 * @author sinai (21/10/2013)
 * 
 * @param pkt_header_reg 
 * @param dest_addr 
 * @param src_addr  
 * @param vlan - an array of length no_tags. includes vlan 
 *             priority and vlan ID (may be NULL if no_tags ==0)
 * @param no_tags 
 * @param next_protocol 
 * @param follows_itmh 
 * 
 * @return int - size in bytes.
 */
int _insert_eth(soc_reg_above_64_val_t pkt_header_reg,const uint32 * dest_addr,const uint32 * src_addr,const  uint16 *vlans, const uint16 *tpids,
              int no_tags,   int follows_itmh) {
    int offset = follows_itmh? 6 : 2;
    int size = 14 + no_tags*4, bit_offset=0, i=0;
    INSERT_INTO_REG(pkt_header_reg,offset, size,bit_offset, 16, SOC_PPD_L2_NEXT_PRTCL_TYPE_CFM);  /* CFM. hard coded.*/ 
    bit_offset +=16;
    while (no_tags) {
        INSERT_INTO_REG(pkt_header_reg,offset,size,bit_offset , 16, vlans[i]); 
        bit_offset +=16;
        INSERT_INTO_REG(pkt_header_reg,offset,size, bit_offset, 16, tpids[i]); 
        bit_offset +=16;
        --no_tags;
        ++i;
    }
    INSERT_INTO_REG_BUFFER(pkt_header_reg,offset,size,bit_offset,48,src_addr); 
    bit_offset += 48;
    INSERT_INTO_REG_BUFFER(pkt_header_reg,offset,size,bit_offset,48,dest_addr);
    return size ;
}

/**
 * Enters the MPLS label into pkt_header_reg.
 * May be used for GAL/GACH/PWE as well.
 *  
 * MPLS label follows PTCH2, ITMH, PPH (+FHEI) and possibly 
 * another MPLS label. This should be determined by label_no. 
 *  
 * @author sinai (21/10/2013)
 * 
 * @param pkt_header_reg 
 * @param label 
 * @param exp 
 * @param bos - bottom of stack 
 * @param ttl 
 * @param offset bytes before this guy
 * 
 * @return int 
 */
int _insert_mpls_label(soc_reg_above_64_val_t pkt_header_reg, int label, uint8 exp, uint8 bos, uint8 ttl, int offset) {
    INSERT_INTO_REG(pkt_header_reg,offset,4,0,8,ttl); 
    INSERT_INTO_REG(pkt_header_reg,offset,4,8,1,bos); /*like a bos*/
    INSERT_INTO_REG(pkt_header_reg,offset,4,9,3,exp);
    INSERT_INTO_REG(pkt_header_reg,offset,4,12,20,label); 
    return 4;
}

/**
 * Enters the MPLS label into pkt_header_reg. 
 * Should generally be used for gal/gach. 
 * 
 * @author sinai (23/10/2013)
 * 
 * @param pkt_header_reg 
 * @param stuff 4 bytes to inseert
 * @param offset bytes before this guy
 * 
 * @return int 
 */
int _insert_4_bytes(soc_reg_above_64_val_t pkt_header_reg, uint32 stuff, int offset) {
    INSERT_INTO_REG(pkt_header_reg,offset,4,0,32,stuff); 
    return 4;
}

/**
 * Enters the OAM PDU header int pkt_header_reg. 
 * data is inserted according to offset. 
 * flags and version are always 0. 
 * opcode is determined by is_lbm (LB or TST)
 * 
 * @author sinai (21/10/2013)
 * 
 * @param pkt_header_reg 
 * @param level 
 * @param is_lbm 
 * @param offset 
 * 
 * @return int 
 */
int _insert_oam_pdu_header(soc_reg_above_64_val_t pkt_header_reg, int level,int is_lbm, int offset){
    int opcode = is_lbm? SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM: 
        SOC_PPD_OAM_ETHERNET_PDU_OPCODE_TST; 
    INSERT_INTO_REG(pkt_header_reg,offset,4, 0,8, 0x4);
    INSERT_INTO_REG(pkt_header_reg,offset,4, 16,8, opcode);
    INSERT_INTO_REG(pkt_header_reg,offset,4, 29,3,level);
    return 4;
}


/**
 * Enters an FHEI in pkt_header starting at offset.
 * 
 * @author sinai (13/02/2014)
 * 
 * @param pkt_header_reg 
 * @param cpu_trap_code_qualifier 
 * @param cpu_trap_code 
 * @param offset 
 * 
 * @return int 
 */
int _insert_fhei(soc_reg_above_64_val_t pkt_header_reg, int cpu_trap_code_qualifier, int cpu_trap_code, int offset){
	INSERT_INTO_REG(pkt_header_reg,offset,3,8,16, cpu_trap_code_qualifier);
	INSERT_INTO_REG(pkt_header_reg,offset,3,0,8, cpu_trap_code);
	return 3;
}


uint32 
    arad_pp_oam_oamp_loopback_set_unsafe(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_IN uint32                                                     endpoint_id,
       SOC_SAND_IN  uint8                                                      tx_period,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *mac_address,
       SOC_SAND_OUT uint8                                                   *is_added
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t      pkt_header, reg_data, reg_field;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry;
    uint32 short_name, entry;
    int header_size=0;
    int next_is_itmh;
    uint32  ssp;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(pkt_header);
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TST_LBM_CTRLr(unit, reg_data));
    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_GEN_ENf, reg_field);

    if (reg_field[0] && is_added==0) { /*Already taken. */
        goto exit;
    }

    SOC_REG_ABOVE_64_CLEAR(reg_data); /* put zeros in whatever is not configured below.*/

    reg_field[0] = 1;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_GEN_ENf, reg_field);
    reg_field[0] = (( tx_period * arad_chip_kilo_ticks_per_sec_get(unit)) > 0xffffff)? 0xffffff : ( tx_period * arad_chip_kilo_ticks_per_sec_get(unit));
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_PERIODf, reg_field);
    reg_field[0]=1;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_0_LBM_1f, reg_field);

    reg_field[0] = 0; /* Set to 0 since the tlv iremains 0 anyways.*/
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_TLV_LENf, reg_field);
    reg_field[0] = 0x8; /*default*/
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_THRESHOLDf, reg_field);

    /* let the rest be for now.*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_CTRLr(unit, reg_data));



    res = arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(unit,endpoint_id,&short_name,&mep_db_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 234, exit);

    /* Now build the packet stack, according to type, direction.*/


    next_is_itmh =  (mep_db_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE || mep_db_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP ||
        (mep_db_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM && !mep_db_entry.is_upmep ) );

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_DOWN_PTCHr, REG_PORT_ANY, 0, DOWN_PTCH_PP_SSPf, &ssp));
    ssp =  (mep_db_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM && mep_db_entry.is_upmep )? mep_db_entry.local_port : ssp;
    header_size += _insert_ptch2(pkt_header,next_is_itmh,_ARAD_PP_OAM_PTCH_OPAQUE_VALUE ,
                               ssp ); 
        

    if (next_is_itmh) {
        soc_field_t  fwd_tc[] = {CCM_CLASS_0f,CCM_CLASS_1f,CCM_CLASS_2f,CCM_CLASS_3f,CCM_CLASS_4f,CCM_CLASS_5f,CCM_CLASS_6f,CCM_CLASS_7f};
        soc_field_t fwd_dp[] = {CCM_DP_0f, CCM_DP_1f, CCM_DP_2f, CCM_DP_3f, CCM_DP_4f, CCM_DP_5f, CCM_DP_6f, CCM_DP_7f};
        uint32 tc, dp, fwd_dst_info;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 2344, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_PR_2_FW_DTCr, REG_PORT_ANY, 0, fwd_tc[mep_db_entry.priority], &tc));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 2344, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_PR_2_FWDDPr, REG_PORT_ANY, 0, fwd_dp[mep_db_entry.priority], &dp));
        if (mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_MEM_20000m(unit, MEM_BLOCK_ANY, mep_db_entry.local_port, &entry)); /*  LOCAL_PORT_2_SYSTEM_PORT register*/
            fwd_dst_info =  0xc0000 | entry;
        } else {
            fwd_dst_info = 0xa0000 | mep_db_entry.egress_if;
        }
        header_size += _insert_itmh(pkt_header,fwd_dst_info,dp,tc,0,0,(mep_db_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP  || mep_db_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE));
    }

    if (mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
        uint16 vlans[2], tpids[2];
        uint32  src_mac_for_real[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S], dst_addr_in_uint32s[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
        uint64 src_addr;
        soc_field_t tpid_field[] = {CCM_TPID_0f, CCM_TPID_1f, CCM_TPID_2f, CCM_TPID_3f};
        uint64 uint64_field;


        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf, &src_addr));

        _ARAD_PP_OAM_MAC_ADD_MSB_SET(src_addr, src_mac_for_real);
        _ARAD_PP_OAM_MAC_ADD_LSB_SET(mep_db_entry.local_port ,src_mac_for_real);
        dst_addr_in_uint32s[0] = mac_address->address[0] | mac_address->address[1] <<8 | mac_address->address[2] <<16 
                | mac_address->address[3] <<24;
        dst_addr_in_uint32s[1] = mac_address->address[4] | mac_address->address[5]<<8;
 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  234,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_TPID_MAPr, REG_PORT_ANY, 0, tpid_field[mep_db_entry.outer_tpid], &uint64_field));
        tpids[0] = COMPILER_64_LO(uint64_field);

        
        vlans[0]= mep_db_entry.outer_vid_dei_pcp;
        if (mep_db_entry.tags_num == 2) {
            vlans[1]= mep_db_entry.inner_vid_dei_pcp;
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  234,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_TPID_MAPr, REG_PORT_ANY, 0, tpid_field[mep_db_entry.inner_tpid], &uint64_field));
            tpids[1] = COMPILER_64_LO(uint64_field);

        } 
       
        header_size += _insert_eth(pkt_header,dst_addr_in_uint32s,src_mac_for_real, vlans, tpids,mep_db_entry.tags_num, !(mep_db_entry.is_upmep) );
    } else {
        /* MPLS TP or PWE. Either way the PPH is the same.*/
        PPH_base pph;
        uint64 reg64;
        uint8 exp, ttl;
        soc_field_t mpls_pwe_prf_fields[] = {MPLS_PWE_PROFILE_0f, MPLS_PWE_PROFILE_1f, MPLS_PWE_PROFILE_2f, MPLS_PWE_PROFILE_3f, MPLS_PWE_PROFILE_4f, 
        MPLS_PWE_PROFILE_5f, MPLS_PWE_PROFILE_6f, MPLS_PWE_PROFILE_7f, MPLS_PWE_PROFILE_8f, MPLS_PWE_PROFILE_9f, MPLS_PWE_PROFILE_10f, 
        MPLS_PWE_PROFILE_11f, MPLS_PWE_PROFILE_12f, MPLS_PWE_PROFILE_13f, MPLS_PWE_PROFILE_14f, MPLS_PWE_PROFILE_15f, };


        header_size += _insert_itmh_ing_dest_info_extension(pkt_header, 0xc0000 | mep_db_entry.local_port);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TX_PPHr(unit, &reg64));

        pph.fhei_size = 0;
        pph.fwd_code = _ARAD_PP_OAM_FWD_CODE_MPLS; /*mpls/PWE*/
        pph.fwd_header_offset = 0;
        pph.pkt_is_ctrl = 0;
        pph.snoop_cpu_code = 0;
        pph.inLif_orintation = soc_reg64_field32_get(unit, OAMP_TX_PPHr,reg64, TX_PPH_IN_LIF_ORIENTATIONf);
        pph.unknown_da_addr = soc_reg64_field32_get(unit, OAMP_TX_PPHr,reg64, TX_PPH_UNKNOWN_DAf);
        pph.learn_allowed = 0;
        pph.vsi_vrf = soc_reg64_field32_get(unit, OAMP_TX_PPHr,reg64, TX_PPH_VSI_OR_VRFf);
        pph.inLif_inRif = soc_reg64_field32_get(unit, OAMP_TX_PPHr,reg64, TX_PPH_IN_LIF_OR_IN_RIFf);


        header_size += _insert_pph(pkt_header, &pph, header_size);


        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MPLS_PWE_PROFILEr(unit, reg_data));
        soc_reg_above_64_field_get(unit, OAMP_MPLS_PWE_PROFILEr, reg_data, mpls_pwe_prf_fields[mep_db_entry.push_profile],reg_field); 

        ttl = reg_field[0] & 0xff; /*first 8 bits*/
        exp = (reg_field[0] & 0x700) >> 8 ; /* next 3 bits*/
         
        if (mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) {
            /* Packet format: PTCH2 o ITMH o PPH o LSP o GAL+GACH o OAM*/
            uint32 gal_gach;

            header_size += _insert_mpls_label(pkt_header, mep_db_entry.label,exp, 0, ttl, header_size);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1234, exit  , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_Y_1731O_MPLSTP_GALr, REG_PORT_ANY, 0, Y_1731O_MPLSTP_GALf, &gal_gach));
            header_size += _insert_4_bytes(pkt_header ,gal_gach,header_size);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1234, exit  , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_Y_1731O_MPLSTP_GACHr, REG_PORT_ANY, 0, Y_1731O_MPLSTP_GACHf, &gal_gach));
            header_size += _insert_4_bytes(pkt_header,gal_gach ,header_size);
        }

        /* PWE unsopported in Arad +*/
    }

    header_size += _insert_oam_pdu_header(pkt_header, mep_db_entry.mdl, 1, header_size);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  124,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_TST_LBM_PACKET_HEADER_NUM_OF_BYTESr, REG_PORT_ANY, 0, TST_LBM_PACKET_HEADER_NUM_OF_BYTESf,  header_size));

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    soc_reg_above_64_field_set(unit, OAMP_TST_LBM_PACKET_HEADERr, reg_data, TST_LBM_PACKET_HEADERf, pkt_header);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_PACKET_HEADERr(unit, reg_data));


    *is_added=1;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



/************************************* 
        Loopback get
  *************************************/

uint32 
    arad_pp_oam_oamp_loopback_get_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *rx_packet_count,
       SOC_SAND_OUT uint32                                              *tx_packet_count,
       SOC_SAND_OUT uint32                                              *discard_count,
       SOC_SAND_OUT uint32                                              *fifo_count
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(rx_packet_count);
    SOC_SAND_CHECK_NULL_INPUT(tx_packet_count);
    SOC_SAND_CHECK_NULL_INPUT(discard_count);
    SOC_SAND_CHECK_NULL_INPUT(fifo_count);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    arad_pp_oam_oamp_loopback_get_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *rx_packet_count,
       SOC_SAND_OUT uint32                                              *tx_packet_count,
       SOC_SAND_OUT uint32                                              *discard_count,
       SOC_SAND_OUT uint32                                              *fifo_count
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t      reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_RXB_DEBUGr, REG_PORT_ANY, 0, RX_LBR_COUNTf, rx_packet_count));

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);   
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TST_LBM_STATUSr(unit, reg_data));

    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_STATUSr, reg_data, TST_LBM_PKT_CNTf, reg_field);
    *tx_packet_count = reg_field[0];

    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_STATUSr, reg_data, TST_LBM_DISCARD_PKT_CNTf, reg_field);
    *discard_count = reg_field[0];

    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_STATUSr, reg_data, TST_LBM_FIFO_CNTf, reg_field);
    *fifo_count = reg_field[0];


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/*  a distant cousin - loopback_get_period*/

uint32 
    arad_pp_oam_oamp_loopback_get_period_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(period);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    arad_pp_oam_oamp_loopback_get_period_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t      reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TST_LBM_CTRLr(unit, reg_data));
    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_PERIODf, reg_field);

    *period = reg_field[0] / arad_chip_kilo_ticks_per_sec_get(unit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/************************************ 
  loopback remove
  ***********************************/
   

uint32 
    arad_pp_oam_oamp_loopback_remove_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    arad_pp_oam_oamp_loopback_remove_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 field; 
    soc_reg_above_64_val_t      reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    /* clear statistics, ctrl and packet_header registers.*/

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_RXB_DEBUGr, REG_PORT_ANY, 0, RX_LBR_COUNTf, &field)); /*register is clear on read. clear it.*/
  
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);      

    reg_field[0] =1;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_RSTf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_CTRLr(unit, reg_data)); 


    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  124,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_TST_LBM_PACKET_HEADER_NUM_OF_BYTESr, REG_PORT_ANY, 0, TST_LBM_PACKET_HEADER_NUM_OF_BYTESf,  0));
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_PACKET_HEADERr(unit, reg_data));
   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/*********************************************************************
* OAMP Server 
*********************************************************************/
uint32 
    arad_pp_oam_oamp_rx_trap_codes_set_unsafe(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPD_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 field; 
    uint32 field_name;
    uint32 reg_index;
    uint32 internal_trap_code;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    switch (mep_type) {
    case SOC_PPD_OAM_MEP_TYPE_ETH_OAM: field_name = TRAP_N_ETHf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP: field_name = TRAP_N_1731O_MPLSTPf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE: field_name = TRAP_N_1731O_PWEf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP: field_name = TRAP_N_BFDO_IPV4f;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS: field_name = TRAP_N_BFDO_MPLSf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE: field_name = TRAP_N_BFDO_PWEf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP: field_name = TRAP_N_BFDO_PWEf;
        break;
    default:
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Illegal mep type\n")));
    }

    _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,trap_code,internal_trap_code);

    /* Check if there is a free trap code and set it's value */
    /* There are 4 trap codes per type, one is reserved for local accelerated packets */
    for (reg_index=1; reg_index<=3; reg_index++) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, 
                                             ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, &field));

        if (field == 0) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, 
                                             ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, internal_trap_code));  
            break;          
        }
    }

    if (reg_index > 3) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No free trap code resources in OAMP\n")));
    } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    arad_pp_oam_oamp_rx_trap_codes_set_verify(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPD_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

     SOC_SAND_ERR_IF_ABOVE_MAX(mep_type, SOC_PPD_OAM_MEP_TYPE_COUNT, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
     SOC_SAND_ERR_IF_ABOVE_MAX(trap_code, ARAD_PP_NOF_TRAP_CODES-1, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    arad_pp_oam_oamp_rx_trap_codes_delete_unsafe(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPD_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 field; 
    uint32 field_name;
    uint32 reg_index;
    uint32 internal_trap_code;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    switch (mep_type) {
    case SOC_PPD_OAM_MEP_TYPE_ETH_OAM: field_name = TRAP_N_ETHf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP: field_name = TRAP_N_1731O_MPLSTPf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE: field_name = TRAP_N_1731O_PWEf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP: field_name = TRAP_N_BFDO_IPV4f;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS: field_name = TRAP_N_BFDO_MPLSf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE: field_name = TRAP_N_BFDO_PWEf;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP: field_name = TRAP_N_BFDO_PWEf;
        break;
    default:
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Illegal mep type\n")));
    }

    _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,trap_code,internal_trap_code);

    /* Check if there is a trap code with the given value and reset it to 0 */
    /* There are 4 trap codes per type, one is reserved for local accelerated packets */
    for (reg_index=1; reg_index<=3; reg_index++) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, 
                                             ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, &field));

        if (field == internal_trap_code) {
            field = 0;
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, 
                                             ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, field));  
            break;          
        }
    }

    if (reg_index > 3) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Given trap not found\n")));
    } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    arad_pp_oam_oamp_rx_trap_codes_delete_verify(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPD_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

     SOC_SAND_ERR_IF_ABOVE_MAX(mep_type, SOC_PPD_OAM_MEP_TYPE_COUNT, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
     SOC_SAND_ERR_IF_ABOVE_MAX(trap_code, ARAD_PP_NOF_TRAP_CODES-1, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

/**************************
 * Default profiles
 **************************/

/*
 * arad_pp_oam_set_inlif_profile_map
 * Adds a mapping from an inlif profile (4b) to an OAM lif profile (2b)
 */
soc_error_t
  arad_pp_oam_inlif_profile_map_set(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_IN  uint32                                               oam_profile
  ) {
    uint32
      res = SOC_SAND_OK,
      profile_map;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(inlif_profile, ARAD_PP_LIF_LIF_PROFILE_MAX, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(oam_profile, _BCM_OAM_LIF_PROFILE_MAX, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, READ_IHB_OAM_IN_LIF_PROFILE_MAPr(unit,  &profile_map));
    SHR_BITCOPY_RANGE(&profile_map, (inlif_profile*2), &oam_profile, 0, 2);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_IN_LIF_PROFILE_MAPr(unit,  profile_map));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


soc_error_t
  arad_pp_oam_inlif_profile_map_get(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_OUT uint32                                               *oam_profile
  ) {
    uint32
      res = SOC_SAND_OK,
      profile_map;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(inlif_profile, ARAD_PP_LIF_LIF_PROFILE_MAX, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
    SOC_SAND_CHECK_NULL_INPUT(oam_profile);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, READ_IHB_OAM_IN_LIF_PROFILE_MAPr(unit,  &profile_map));
    *oam_profile = (profile_map >> (inlif_profile*2)) & 0x3;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/************************ 
    reset, clear DMA
 ***********************/

uint32 
    arad_pp_oam_dma_reset_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 
}

uint32 
    arad_pp_oam_dma_reset_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 use_dma;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_fifo_dma;


    if (use_dma) {
        res = arad_pp_oam_dma_set_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
    } 

    last_dma_interrupt_message_num[unit]=0 ; 
    num_entries_read_in_local_buffer[unit]=0 ; 
    num_entries_read_in_local_buffer[unit]=0 ; 


    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/*  ** *********/

uint32 
    arad_pp_oam_dma_clear_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 
}

uint32 
    arad_pp_oam_dma_clear_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 use_dma;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_fifo_dma;

    if (use_dma) {
        res = arad_pp_oam_dma_unset_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
    }

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/**** ******************** 
  Register event handler callbeck, DMA
  *********************/
uint32 
    arad_pp_oam_register_dma_event_handler_callback_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        int (*event_handler_cb)(int)
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 

}

uint32 
    arad_pp_oam_register_dma_event_handler_callback_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        int (*event_handler_cb)(int)
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    dma_event_handler_cb[unit] = event_handler_cb; 

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 

}

/***************************** 
  DMA event handler
  ***************************/


uint32 
   arad_pp_oam_dma_event_handler_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 
}


uint32 
   arad_pp_oam_dma_event_handler_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    return dma_event_handler_cb[unit](unit); /* Long way for one line... */
}



/*******************************          DIAGNOSTICS ****************/

uint32
  arad_pp_oam_diag_print_lookup_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_lookup_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
    uint64 oema_ll_reg, oemb_ll_reg, rmapem_ll_reg;
    uint32 res, printed =0;
    uint64 key, result, found;
    uint32 key32, result32[2];
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};
    SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY     oem2_key= {0};
    SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};
    soc_reg_t
        last_lookup_regs[2];
    uint32
        lookup_reg_idx,
        nof_last_lookup_regs;
	soc_field_t key_field[2]={0}, result_field[2]={0}, found_field[2]={0};
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    /*oema last lookup*/
    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMA_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMA_LAST_LOOKUP_SLAVEr;
		key_field[0] = OEMA_LAST_LOOKUP_MASTER_KEYf;
		key_field[1] = OEMA_LAST_LOOKUP_SLAVE_KEYf;
		result_field[0] = OEMA_LAST_LOOKUP_MASTER_RESULTf;
		result_field[1] = OEMA_LAST_LOOKUP_SLAVE_RESULTf;
		found_field[0] = OEMA_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
		found_field[1] = OEMA_LAST_LOOKUP_SLAVE_FOUNDf;
		nof_last_lookup_regs = 2;

    } else {
        last_lookup_regs[0] = IHB_OEMA_LAST_LOOKUPr;
		key_field[0] = OEMA_LAST_LOOKUP_KEYf;
		result_field[0] = OEMA_LAST_LOOKUP_RESULTf;
		found_field[0] = OEMA_LAST_LOOKUP_RESULT_FOUNDf;

		nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oema_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        key = soc_reg64_field_get(unit,last_lookup_regs[lookup_reg_idx], oema_ll_reg,key_field[lookup_reg_idx]);
        result = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oema_ll_reg, result_field[lookup_reg_idx]);
        found = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oema_ll_reg, found_field[lookup_reg_idx]);
        if (COMPILER_64_LO(found) || COMPILER_64_LO(key)) {
            key32 = COMPILER_64_LO(key);
            result32[0] = COMPILER_64_LO(result);
            result32[1] = COMPILER_64_HI(result);
            _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(result32, oem1_payload);
            _ARAD_PP_OAM_OEM1_KEY_TO_KEY_STRUCT(oem1_key, key32);
            LOG_CLI((BSL_META_U(unit,
                                "IHB OEMA last lookup: Key=0X%x, result=0X%x%x\n"), key32,result32[1],result32[0]));
            _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key);
            if (COMPILER_64_LO(found)) {
                _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oem1_payload);

            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\tNot found.\n")));
            }
            printed = 1;
        }
    }

    /*oemb last lookup*/
    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMB_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMB_LAST_LOOKUP_SLAVEr;
		key_field[0] = OEMB_LAST_LOOKUP_MASTER_KEYf;
		key_field[1] = OEMB_LAST_LOOKUP_SLAVE_KEYf;
		result_field[0] = OEMB_LAST_LOOKUP_MASTER_RESULTf;
		result_field[1] = OEMB_LAST_LOOKUP_SLAVE_RESULTf;
		found_field[0] = OEMB_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
		found_field[1] = OEMB_LAST_LOOKUP_SLAVE_FOUNDf;
        nof_last_lookup_regs = 2;
    } else {
        last_lookup_regs[0] = IHB_OEMB_LAST_LOOKUPr;
		key_field[0] = OEMB_LAST_LOOKUP_KEYf;
		result_field[0] = OEMB_LAST_LOOKUP_RESULTf;
		found_field[0] = OEMB_LAST_LOOKUP_RESULT_FOUNDf;
        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oemb_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
        key =soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, key_field[lookup_reg_idx]);
        result = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, result_field[lookup_reg_idx]);
        found =  soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, found_field[lookup_reg_idx]);
        if (COMPILER_64_LO(found) || COMPILER_64_LO(key)) {
    		key32 = COMPILER_64_LO(key);
    		result32[0] = COMPILER_64_LO(result); 
    		_ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(result32[0], oem2_payload);
    		_ARAD_PP_OAM_OEM2_KEY_TO_KEY_STRUCT(oem2_key, key32);
            LOG_CLI((BSL_META_U(unit,
                                "IHB OEMB last lookup: Key=0X%x  , result=0X%x\n"),key32,result32[0]));
    		LOG_CLI((BSL_META_U(unit,
                                    "\tOEMB key: ingress: %d, MDL: %d, OAM LIF: 0x%x, your disc: %d\n"),  oem2_key.ingress, oem2_key.mdl,  oem2_key.oam_lif, oem2_key.your_disc)); 
    		if (COMPILER_64_LO(found)) {
    			LOG_CLI((BSL_META_U(unit,
                                            "\t\tOEMB payload: MP profile: 0x%x,OAM ID: 0x%x\n"), oem2_payload.mp_profile, oem2_payload.oam_id));
    		} else {
    			LOG_CLI((BSL_META_U(unit,
                                            "\tNot found.\n")));
    		}
            printed = 1;
        }
    }

    /*RMAPEM last lookup*/
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,exit, READ_OAMP_RMAPEM_LAST_LOOKUPr(unit,&rmapem_ll_reg));

   key = soc_reg64_field_get(unit, OAMP_RMAPEM_LAST_LOOKUPr, rmapem_ll_reg, RMAPEM_LAST_LOOKUP_KEYf);
   result = soc_reg64_field_get(unit, OAMP_RMAPEM_LAST_LOOKUPr, rmapem_ll_reg, RMAPEM_LAST_LOOKUP_RESULTf);
   found = soc_reg64_field_get(unit, OAMP_RMAPEM_LAST_LOOKUPr, rmapem_ll_reg, RMAPEM_LAST_LOOKUP_RESULT_FOUNDf); 
   if (COMPILER_64_LO(found)|| COMPILER_64_LO(key)) {
       LOG_CLI((BSL_META_U(unit,
                           "\tOAMP RMAPEM last lookup: Key=0X%x , result=0X%x"),COMPILER_64_LO(key), COMPILER_64_LO(result)));
       PRINT_FOUND(found);
       printed = 1;
   }

   if (printed == 0) {
       LOG_CLI((BSL_META_U(unit,
                           "\tNothing to display.\n")));    
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}

uint32
  arad_pp_get_crps_counter_verify(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      )
{
  uint32
    res = SOC_SAND_OK;
      SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
  if (crps_counter_number > 3) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("crps_counter number must be 0,1,2 0r 3, value given is %d"),crps_counter_number));
  }
  if (reg_number >= 0x800) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("register number must be between 0 and (32K-1), value given is %d"),reg_number));
  }
  SOC_SAND_CHECK_NULL_INPUT(value); 



  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_get_crps_counter_verify()", 0, 0);
}


uint32
  arad_pp_get_crps_counter_unsafe(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      )
{
    soc_reg_above_64_val_t crps_reg;
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



   switch (crps_counter_number) {
   case 0:
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,exit, READ_CRPS_CRPS_0_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
       soc_CRPS_CRPS_0_CNTS_MEMm_field_get(unit,crps_reg, PACKETS_COUNTERf, value );
       break;
   case 1:
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,exit, READ_CRPS_CRPS_1_CNTS_MEMm(unit,MEM_BLOCK_ANY, reg_number, &crps_reg));
       soc_CRPS_CRPS_1_CNTS_MEMm_field_get(unit,crps_reg, PACKETS_COUNTERf, value );
       break;
   case 2:
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,exit, READ_CRPS_CRPS_1_CNTS_MEMm(unit,MEM_BLOCK_ANY, reg_number, &crps_reg));
       soc_CRPS_CRPS_2_CNTS_MEMm_field_get(unit,crps_reg, PACKETS_COUNTERf, value );
       break;
   case 3:
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,exit, READ_CRPS_CRPS_1_CNTS_MEMm(unit,MEM_BLOCK_ANY, reg_number, &crps_reg));
           soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit,crps_reg, PACKETS_COUNTERf, value );
       break;
   default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("The impossible happend (for real)\n")));
   }
   

    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_get_crps_counter_unsafe()", crps_counter_number, reg_number);    
}

/* Set report mode for LM/DM reports (Normal/Compact) */
soc_error_t
  arad_pp_oamp_report_mode_set(int unit, SOC_PPC_OAM_REPORT_MODE mode) {

    uint32 rv;
    uint32 reg;

    SOCDNX_INIT_FUNC_DEFS;

    if (mode < SOC_PPC_OAM_REPORT_MODE_COUNT) {

        
        if (mode == SOC_PPC_OAM_REPORT_MODE_RAW) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error - Raw report mode unsupported")));
        }

        rv = READ_OAMP_MODE_REGISTERr(unit, &reg);
        SOCDNX_SAND_IF_ERR_EXIT(rv);

        soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, REPORT_MODEf, mode);
        rx_report_event_mode[unit] = mode;

        rv = WRITE_OAMP_MODE_REGISTERr(unit,reg);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Get the report mode for LM/DM reports (Normal/Compact) */
soc_error_t
  arad_pp_oamp_report_mode_get(int unit, SOC_PPC_OAM_REPORT_MODE *mode) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(mode);

    *mode = rx_report_event_mode[unit];

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_oam_diag_print_rx_verify(
     SOC_SAND_IN int unit
   )
{
    uint32
       res = SOC_SAND_OK;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

  

/**  
 * 
 * Function checks if something was found at the IHB OEMA/B last 
 * lookup register 
 * 
 * @param unit 
 * @param val - returns yes or no.
 * 
 * @return uint32 - OK or error message. 
 */
uint32 
    arad_pp_oam_last_lookup_found(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint8 *val
    )
{
    uint32
        res; 
    uint64 oema_ll_reg, oemb_ll_reg, found;
    soc_reg_t
        last_lookup_regs[2];
    soc_field_t
        last_lookup_fields[2];
    uint32
        lookup_reg_idx,
        nof_last_lookup_regs;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *val = 0;
    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMA_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMA_LAST_LOOKUP_SLAVEr;
        last_lookup_fields[0] = OEMA_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
        last_lookup_fields[1] = OEMA_LAST_LOOKUP_SLAVE_FOUNDf;
        nof_last_lookup_regs = 2;
    } else {
        last_lookup_regs[0] = IHB_OEMA_LAST_LOOKUPr;
        last_lookup_fields[0] = OEMA_LAST_LOOKUP_RESULT_FOUNDf;
        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oema_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        found = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oema_ll_reg, last_lookup_fields[lookup_reg_idx]);
        if (COMPILER_64_LO(found)) {
            /* Found something. Say yes and get outta here.*/
            *val = 1;
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }

    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMB_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMB_LAST_LOOKUP_SLAVEr;
        last_lookup_fields[0] = OEMB_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
        last_lookup_fields[1] = OEMB_LAST_LOOKUP_SLAVE_FOUNDf;
        nof_last_lookup_regs = 2;
    } else {
        last_lookup_regs[0] = IHB_OEMB_LAST_LOOKUPr;
        last_lookup_fields[0] = OEMB_LAST_LOOKUP_RESULT_FOUNDf;
        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oemb_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        found = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, last_lookup_fields[lookup_reg_idx]);
        if (COMPILER_64_LO(found)) {
            /* Found something. Say yes and get outta here.*/
            *val = 1;
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_diag_print_rx_unsafe(
     SOC_SAND_IN int unit
   )
{ 
    uint32
       res = SOC_SAND_OK;
    uint32 reg=0, opaque_pt_attr=0, next_header_is_not_itmh=0, recycle_port;
    ARAD_PP_DIAG_RECEIVED_PACKET_INFO rpi;
    soc_port_if_t port_type;
    int i, dir_printed=0, trap_code ;
    soc_reg_t sticky_fields[] = {STICKY_PORT_ERR_PACKETf, STICKY_PORT_TRAP_MISSf, STICKY_PORT_TYPE_MISSf, STICKY_PORT_EMC_MISSf, STICKY_PORT_MAID_MISSf,
    STICKY_PORT_MDL_MISSf, STICKY_PORT_CCM_INTRVL_MISSf, STICKY_PORT_ERR_MY_DISC_MISSf, STICKY_PORT_ERR_SRC_IP_MISSf  ,STICKY_PORT_ERR_YOUR_DISC_MISSf,
        STICKY_PORT_SRC_PORT_MISSf , STICKY_PORT_BFD_FBITf, STICKY_PORT_BFD_PBITf, STICKY_PARITY_ERRORf};
    char *stikcy_port_msg[] = {"error packet", "trap miss", "type miss", "emc miss", "maid miss", "mdl miss", "ccm interval miss", "my discriminator miss","error src IP miss" , "error your discriminator miss"  ,
        "source port miss", "BFD f-bit", "BFD p-bit", "parity error"};
    uint8 ll_found;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN] = {0};
    uint32 oam_trap_code, oam_trap_code_forward;
    char *sub_type, *trap_word;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

     if (SOC_IS_ARADPLUS(unit)) {
         /* some fields are different for ARAD+*/
         sticky_fields[11] = STICKY_PORT_RMEP_STATE_CHANGEf;
         sticky_fields[12] = STICKY_PORT_PARITY_ERRORf;
         sticky_fields[13] = STICKY_PORT_TIMESTAMP_MISSf;
        
         stikcy_port_msg[11] = "RMEP state change";
         stikcy_port_msg[12] = "parity error";
         stikcy_port_msg[13] = "time stamp miss"; 
     }

    SOC_PPD_DIAG_RECEIVED_PACKET_INFO_clear(&rpi);
    res = arad_pp_diag_received_packet_info_get(unit,&rpi);
     if (soc_sand_update_error_code(res, &ex ) != no_err ) { /* Not using SOC_SAND_.. macros so as not to print an error message.*/
        LOG_CLI((BSL_META_U(unit,
                            "Diagnostic Failed. Perhaps no packet was sent.\n")));
        goto exit;
    }

    /*given the port, we get the interface type do determine if the port was OAMP.*/
    res = soc_port_sw_db_interface_type_get(unit,rpi.in_tm_port, &port_type);
    SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

    if (port_type == SOC_PORT_IF_OAMP) { 
        /*If this is the case there is a PTCH-2 header, and this is regardless of the pp_context.  */
        /*Get the opaque pt -attr which is on the PTCH-2, the beginning of the buffer in rpi.*/
        SHR_BITCOPY_RANGE(&opaque_pt_attr, 0, rpi.packet_header.buff, 12, 3);
        /*While we're here, might as well check if the next header is ITMH */
        SHR_BITCOPY_RANGE(&next_header_is_not_itmh, 0, rpi.packet_header.buff, 15, 3);
        LOG_DEBUG(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              "The opaque pt-attribute taken from the PTCH header is %d, the opaque value is %d\n"),opaque_pt_attr,_ARAD_PP_OAM_PTCH_OPAQUE_VALUE));
    } 


    if ((port_type == SOC_PORT_IF_OAMP) &&  (opaque_pt_attr != _ARAD_PP_OAM_PTCH_OPAQUE_VALUE)) {
        /* This was an error packet. Find out what kind of error*/
        reg = 0;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_VALIDITY_CHECK_STICKYr(unit, &reg));
        /*First, a quick internal check*/
        if (sizeof(sticky_fields) / sizeof(soc_reg_t) != sizeof(stikcy_port_msg) / sizeof(char *)) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  "Something went wrong with internal arrays\n")));
        }
        /*Now for each sticky bit, if it is on display the message.*/
        for (i = 0; i < (sizeof(sticky_fields) / sizeof(soc_reg_t)); ++i) {
            if (soc_reg_field_get(unit, OAMP_VALIDITY_CHECK_STICKYr, reg, sticky_fields[i])) {
                LOG_CLI((BSL_META_U(unit,
                                    "Packet was trapped to CPU due to %s.\n"), stikcy_port_msg[i]));
            }
        }
        
    } else {
        if (opaque_pt_attr == _ARAD_PP_OAM_PTCH_OPAQUE_VALUE) {
            LOG_CLI((BSL_META_U(unit,
                                "Packet was injected, ")));
            dir_printed = 1;
            if (next_header_is_not_itmh) {
                LOG_CLI((BSL_META_U(unit,
                                    "direction: up\n")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "direction: down\n")));
            }
        } else { /* go here only if port_type != OAMP */
            recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);
            if (rpi.in_tm_port == recycle_port) {
                LOG_CLI((BSL_META_U(unit,
                                    "Packet is Loopback\n")));
            } else if (port_type == SOC_PORT_IF_RCY) {
                LOG_CLI((BSL_META_U(unit,
                                    "Packet was mirrored => Direction is up.\n")));
            }
        }

        res = arad_pp_oam_last_lookup_found(unit, &ll_found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        regs_val[0] = 0;
        GET_SIGNAL_FOR_TRAP_CODE(fld);
        trap_code = regs_val[0];


        if (ll_found) {
            GET_SIGNAL_FOR_DIRECTION(fld);
            if (regs_val[0] == 0) { /*direction is down*/
                regs_val[0] = 0;
                GET_SIGNAL_FOR_SUBTYPE_DOWN(fld);
                SUB_TYPE_MSG_FROM_CODE(sub_type, regs_val[0]);
                regs_val[0] = 0;
                GET_SIGNAL_FOR_OAM_ID_DOWN(fld);
                if (dir_printed == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Direction is down, ")));
                }
                /*We have the actual ID only if the indicates that the packet was sent from the OAMP*/
                if ((trap_code == ARAD_PP_TRAP_CODE_TRAP_ETH_OAM) ||  (trap_code == ARAD_PP_TRAP_CODE_TRAP_Y1731_O_MPLS_TP) || (trap_code == ARAD_PP_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP) ||
                    (trap_code == ARAD_PP_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP) || (trap_code == ARAD_PP_TRAP_CODE_TRAP_BFD_O_PWE) || (trap_code == ARAD_PP_TRAP_CODE_TRAP_BFD_O_MPLS) ||
                    (trap_code == ARAD_PP_TRAP_CODE_TRAP_BFD_O_IPV4) || (trap_code == ARAD_PP_TRAP_CODE_TRAP_Y1731_O_PWE) || (trap_code == ARAD_PP_TRAP_CODE_TRAP_Y1731_O_MPLS_TP) ||
                    (trap_code == ARAD_PP_TRAP_CODE_OAM_LEVEL)) {

                    LOG_CLI((BSL_META_U(unit,
                                        "OAM ID is 0X%x. "), regs_val[0]));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "packet sent from non accelerated end point, ID cannot be retrieved. ")));
                }

                LOG_CLI((BSL_META_U(unit,
                                    "OAM subtype is %s\n"),  sub_type));
            } else {
                
                regs_val[0] = 0;

                GET_SIGNAL_FOR_SUBTYPE_UP(fld);
                SUB_TYPE_MSG_FROM_CODE(sub_type, regs_val[0]);
                regs_val[0] = 0;
                GET_SIGNAL_FOR_OAM_ID_UP(fld);
                if (dir_printed == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Direction is up, ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "OAM subtype is %s.\n"),sub_type));
            }
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Lookup not found: OAM subtype, ID cannot be retrieved.\n")));
        }

        if ((trap_code >= ARAD_PP_TRAP_CODE_USER_DEFINED_0 ) && (trap_code<=ARAD_PP_TRAP_CODE_USER_DEFINED_59)) {
            trap_word = "user defined";
        } else if ((trap_code >= ARAD_PP_TRAP_CODE_TRAP_ETH_OAM ) && (trap_code<=ARAD_PP_TRAP_CODE_OAM_LEVEL)) {
            trap_word = "oam";
        } else if (trap_code==ARAD_PP_TRAP_CODE_OAM_PASSIVE) {
            trap_word = "oam passive";
        } else if ((trap_code >= ARAD_PP_TRAP_CODE_PBP_SA_DROP_0 ) && (trap_code<=ARAD_PP_TRAP_CODE_PBP_SA_DROP_3)) {
            trap_word = "drop";
        } else {
            trap_word = "other";
        }
        

        LOG_CLI((BSL_META_U(unit,
                            "Trap code is %d (%s).\n"), trap_code, trap_word));
        regs_val[0] = 0;
        GET_SIGNAL_FOR_OAM_SNOOP_STRENGTH(fld);
        if (regs_val[0] != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "Packet was snooped.\n")));
        }
        _ARAD_PP_OAM_TRAP_CODE_TO_INTERNAL(unit,trap_code,oam_trap_code);
        /*Now check if the packet was forwarded.*/
      res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_FRWRD, &oam_trap_code_forward);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);
      if (oam_trap_code == oam_trap_code_forward) {
          LOG_CLI((BSL_META_U(unit,
                              "Packet was forwarded.\n")));
      }


    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_diag_print_em_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   )
{
    uint32
       res = SOC_SAND_OK;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32
  arad_pp_oam_diag_print_em_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   )
{
    uint32
       res = SOC_SAND_OK;
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      oem2_key = {0};
    SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};
    SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};
    int ingress,mdl,your_discr;
    uint8 found;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (ingress=0; ingress<=1 ; ++ingress) {
        oem1_key.oam_lif = LIF;
        oem1_key.ingress = ingress;
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key);
          if (found) {
              _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oem1_payload);
               if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                  uint8 passive_active_enable;
                  res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit,  oem1_payload.mp_profile, &passive_active_enable);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                  LOG_CLI((BSL_META_U(unit,
                                      "\t\tPassive active enable: %d\n"),  passive_active_enable)); 

              }
          } else {
               LOG_CLI((BSL_META_U(unit,
                              "\tEntry not found\n"))); 
          }
    }

    oem2_key.oam_lif = LIF;
    for (ingress=0 ; ingress<=1 ; ++ingress) {
        for (mdl = 0; mdl < 8; ++mdl) {
            for (your_discr=0 ; your_discr<=1; ++your_discr) {
                oem2_key.ingress = ingress;
                oem2_key.mdl = mdl;
                oem2_key.your_disc = your_discr;
                res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
                if (found) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\tOEMB key: ingress: %d, Your-Discriminator: %d ,OAM LIF: 0x%x, MD-Level: %d\n"),  oem2_key.ingress, oem2_key.your_disc,oem2_key.oam_lif,oem2_key.mdl));
                    LOG_CLI((BSL_META_U(unit,
                                        "\t\tOEMB payload: acc-MP-profile: %d, OAM-ID: 0x%x\n"), oem2_payload.mp_profile, oem2_payload.oam_id));              
                }
            }
        }
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_diag_mp_type_resolution_simulate(
     SOC_SAND_IN    int unit,
     SOC_SAND_IN    ARAD_PP_OAM_ACTION_KEY_PARAMS *key_params,
     SOC_SAND_IN    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
     SOC_SAND_OUT   SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY *act_key
  ) {

    uint32 res;
    uint32 mask;
    uint32 index;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        /* Jericho MP type resolution:
            MDL-MP-Type (2) =  OAM-1-MP-Type-Vector[OAM-MD-Level(3)]

            Note: MDL-MP-Type is encoded as follows:
            2'b00 - No-MP
            2'b01 - MIP
            2'b10 - Active-MEP
            2'b11 - Passive-MEP

            MEP-Bitmap (8) =  {OAM-1-MP-Type-Vector[15], OAM-1-MP-Type-Vector[13],
                                ..OAM-1-MP-Type-Vector[3], OAM-1-MP-Type-Vector[1]}
            Note: MEP-Bitmap is a concatenation of the odds bits from OAM-1-MP-Type-Vector[15:0]

            Not-Above-MDL-MEP-Bitmap-OR (1) =  (|MEP-Bitmap[7:OAM-MD-Level])

            MP-Type(3) = Cfg-MP-Type-Map[OAM-is-BFD, MDL-MP-Type(2), Not-Above-MDL-MEP-Bitmap-OR]

            Note: MP-Type is encoded as follows:
            3'd0 - MIP-Match
            3'd1 - Active-Match
            3'd2 - Passive-Match
            3'd3 - Below-Highest-MEP
            3'd4 - Above-All
            3'd5 - BFD
        */

        uint32 mdl_mp_type;
        uint32 mep_bitmap;
        uint32 not_above;

        uint64 cfg_mp_type_map;

        /* MDL-MP-Type (2) =  OAM-1-MP-Type-Vector[OAM-MD-Level(3)] */
        mdl_mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload->mp_type_vector, key_params->level);

        /* MEP-Bitmap is a concatenation of the odds bits from OAM-1-MP-Type-Vector[15:0] */
        mep_bitmap = 0;
        for (mask = (1<<15); mask > 0; mask >>= 2) {
            mep_bitmap <<= 1;
            mep_bitmap |= ((oem1_payload->mp_type_vector) & mask) ? 1 : 0;
        }

        /* Not-Above-MDL-MEP-Bitmap-OR (1) =  (|MEP-Bitmap[7:OAM-MD-Level]) */
        not_above = (mep_bitmap >> ((key_params->level)-1)) ? 1 : 0;

        /* Read Cfg-MP-Type-Map */
        COMPILER_64_ZERO(cfg_mp_type_map);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_MP_TYPE_MAPr(unit, SOC_CORE_DEFAULT, &cfg_mp_type_map));

        /* MP-Type(3) = Cfg-MP-Type-Map[OAM-is-BFD, MDL-MP-Type(2), Not-Above-MDL-MEP-Bitmap-OR] */
        index = 3 * (((key_params->bfd) << 3) | (mdl_mp_type << 1) | not_above );
        if (index < 32) {
            act_key->mp_type_jr = (COMPILER_64_LO(cfg_mp_type_map) >> index) & 0x7;
        }
        else {
            act_key->mp_type_jr = (COMPILER_64_HI(cfg_mp_type_map) >> (index-32)) & 0x7;
        }
    }
    else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
        /* Arad+ (Advanced classifier) MP type resolution */

        uint8 mip_0 = oem1_payload->mip_bitmap & 0x7;
        uint8 mip_1 = (oem1_payload->mip_bitmap >> 3) & 0x7;

        uint32 passive_active_en = 0;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, &passive_active_en));

        if (passive_active_en & (1 << (oem1_payload->mp_profile))) {
            /*  If cfg-MEP-Passive-Active-Enable[Acc-MEP-Profile(4)] == 0 than
                        MP-Type (2) = MEP-Bitmap[OAM-MD-Level] is set ?  MEP-Match (2'b00) :
                            |MEP-Bitmap[7:OAM-MD-Level] == 1 ? Below-Upper-MEP (2'b01) :
                                (Packet-MD-Level == MIP-0 & MIP-0 != 0)
                                OR (Packet-MD-Level == MIP-1 & MIP-1 != 0) ? MIP-Match (2'b10) :
                                    Above-Upper-MEP (2'b11)
            */
            if (oem1_payload->mep_bitmap & (1 << (key_params->level))) {
                /* MEP-Bitmap[OAM-MD-Level] is set */
                act_key->mp_type = 0; /* MEP-Match (2'b00) */
            }
            else if (((oem1_payload->mep_bitmap) & 0xFF) >> (key_params->level - 1)) {
                /* |MEP-Bitmap[7:OAM-MD-Level] == 1 */
                act_key->mp_type = 1; /* Below-Upper-MEP (2'b01) */
            }
            else if (((key_params->level == mip_0) && mip_0)
                     || ((key_params->level == mip_1) && mip_1)){
                /* (Packet-MD-Level == MIP-0 & MIP-0 != 0) OR (Packet-MD-Level == MIP-1 & MIP-1 != 0) */
                act_key->mp_type = 2; /* MIP-Match (2'b10) */
            }
            else {
                act_key->mp_type = 3; /* Above-Upper-MEP (2'b11) */
            }
        }
        else {
            /* Else
                    MP-Type (2) = MEP-Bitmap[OAM-MD-Level] is set ? Active-Match (2'b00) :
                        (Packet-MD-Level == MIP-0 & MIP-0 != 0)
                        OR (Packet-MD-Level == MIP-1 & MIP-1 != 0) ? Passive-Match (2'b10) :
                            (|MEP-Bitmap[7:OAM-MD-Level] == 1)
                            OR Packet-MD-Level < MIP-0
                            OR Packet-MD-Level < MIP-1 ? Below-Upper-Passive-Or-Active (2'b01) :
                                Above-All (2'b11)
            */
            if (oem1_payload->mep_bitmap & (1 << (key_params->level))) {
                /* MEP-Bitmap[OAM-MD-Level] is set */
                act_key->mp_type = 0; /* Active-Match (2'b00) */
            }
            else if (((key_params->level == mip_0) && mip_0)
                     || ((key_params->level == mip_1) && mip_1)){
                /* (Packet-MD-Level == MIP-0 & MIP-0 != 0) OR (Packet-MD-Level == MIP-1 & MIP-1 != 0) */
                act_key->mp_type = 2; /* Passive-Match (2'b10) */
            }
            else if ((((oem1_payload->mep_bitmap) & 0xFF) >> (key_params->level - 1))
                     || (key_params->level < mip_0)
                     || (key_params->level < mip_1)) {
                /* |MEP-Bitmap[7:OAM-MD-Level] == 1
                   OR Packet-MD-Level < MIP-0
                   OR Packet-MD-Level < MIP-1 */
                act_key->mp_type = 1; /* Below-Upper-Passive-Or-Active (2'b01) */
            }
            else {
                act_key->mp_type = 3; /* Above-All (2'b11) */
            }
        }
    }
    else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
        
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(
           "Adavnced classifier with multiple MEPs on LIF is not supported for this diag yet.")));
    }
    else {
        /* Arad MEP and MIP type resolution
            For each MEP/MIP:
                MP-Type (2) =
                    Bitmap[OAM-MD-Level] is set ? MEP/MIP (2'b00) :
                        Bitmap is NULL ? ABOVE (2'b01) :
                            Bitmap[7:OAM-MD-Level] is NULL ? ABOVE (2'b01) :
                                Bitmap[OAM-MD-Level:0] is NULL ? BELOW (2'b10) :
                                    BETWEEN (2'b11)
        */

#define __ARAD_SET_MP_TYPE__(mp_type, bitmap) \
        do { \
            if ((bitmap)&(1 << (key_params->level))) { \
                /* Bitmap[OAM-MD-Level] is set */ \
                (mp_type) = 0; /* MEP/MIP (2'b00) */ \
            } \
            else if ((bitmap) == 0) { \
                /* Bitmap is NULL */ \
                (mp_type) = 1; /* ABOVE (2'b01) */ \
            } \
            else if ((((bitmap) & 0x7) >> (key_params->level - 1)) == 0) { \
                /* Bitmap[7:OAM-MD-Level] is NULL */ \
                (mp_type) = 1; /* ABOVE (2'b01) */ \
            } \
            else if (((bitmap) & ((1 << key_params->level)-1)) == 0) { \
                /* Bitmap[OAM-MD-Level:0] is NULL */ \
                (mp_type) = 2; /* BELOW (2'b10) */ \
            } \
            else { \
                (mp_type) = 3; /* BETWEEN (2'b11) */ \
            } \
        } while(0)

        __ARAD_SET_MP_TYPE__(act_key->mep_type, oem1_payload->mep_bitmap);
        __ARAD_SET_MP_TYPE__(act_key->mip_type, oem1_payload->mip_bitmap);

#undef __ARAD_SET_MP_TYPE__

    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


/*
 * Does the actual action key lookup.
 *
 * If a key is found, *found would hold the table indicator (1 for OAM-1 or 2 for OAM-2).
 * Else, *found would be 0.
 */
uint32
  arad_pp_oam_diag_action_key_get_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_OAM_ACTION_KEY_PARAMS *key_params,
     SOC_SAND_OUT uint32 keys[],
     SOC_SAND_OUT uint8  *found
  ) {
    uint32
       res = SOC_SAND_OK;

    uint8 my_cfm_mac;

    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};

    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      oem2_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = 0;

    /* Start by looking in OEM-2 (Top priority) */
    oem2_key.oam_lif = key_params->lif;
    oem2_key.ingress = key_params->ing;
    oem2_key.mdl = key_params->level;
    oem2_key.your_disc = key_params->your_disc;
    res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (*found) { /* Entry found in OEM-2, construct OAM-2 key and print */
        SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY act_key;
        SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&act_key);

        /* Populate key struct*/
        act_key.ingress = key_params->ing;
        act_key.is_bfd = key_params->bfd;
        act_key.mp_profile = oem2_payload.mp_profile;
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP,
                                        &act_key.opcode, key_params->opcode);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        for (my_cfm_mac = 0; my_cfm_mac <= 1; my_cfm_mac++) {
            act_key.my_cfm_mac = my_cfm_mac;
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            /* Construct key */
            _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY(act_key, keys[my_cfm_mac]);
        }

        *found = 2; /* For printing that the keys are for OAM-2 */
    }
    else { /* Not found in OEM-2, try OEM-1 */
        oem1_key.oam_lif = key_params->lif;
        oem1_key.ingress = key_params->ing;
        oem1_key.your_discr = key_params->your_disc;
        res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (*found) {
            SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY act_key;
            SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&act_key);

            /* Populate key struct*/
            act_key.ingress = key_params->ing;
            act_key.is_bfd = key_params->bfd;
            act_key.mp_profile = oem1_payload.mp_profile;
            res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP,
                                            &act_key.opcode, key_params->opcode);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            res = arad_pp_oam_diag_mp_type_resolution_simulate(unit, key_params, &oem1_payload, &act_key);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            for (my_cfm_mac = 0; my_cfm_mac <= 1; my_cfm_mac++) {
                act_key.my_cfm_mac = my_cfm_mac;

                /* Construct key */
                _ARAD_PP_OAM_OAM1_KEY_STRUCT_TO_KEY(act_key, keys[my_cfm_mac]);
            }

            *found = 1; /* For printing that the keys are for OAM-1 */
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_ak_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_OAM_ACTION_KEY_PARAMS *key_params
   ) {
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(key_params);
    SOC_SAND_ERR_IF_ABOVE_MAX(key_params->level, (key_params->bfd ? 0 : 7), ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 10, exit);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_ak_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_OAM_ACTION_KEY_PARAMS *key_params
   ){
    uint32
       res = SOC_SAND_OK;

    uint8 my_cfm_mac;

    uint32 keys[2]; /* The keys to print */
    uint8 found = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_action_key_get_unsafe(unit, key_params, keys, &found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (found) {
        for (my_cfm_mac = 0; my_cfm_mac <= 1; my_cfm_mac++) {
            LOG_CLI((BSL_META_U(unit,
                                "\tOAM-%d Key (for My-CFM-MAC = %d): 0x%08x (%d)\n"),
                                found, my_cfm_mac, keys[my_cfm_mac], keys[my_cfm_mac]));
        }
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                       "\tEntry not found\n")));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_oamp_counter_verify(
     SOC_SAND_IN int unit
   )
{
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_oamp_counter_unsafe(
     SOC_SAND_IN int unit
   )
{
    uint32
       res = SOC_SAND_OK;
    uint32 reg_val;
    uint32 rx_counter;
    uint32 tx_counter;
        
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0168r(unit, &reg_val));
        rx_counter = soc_reg_field_get(unit, OAMP_REG_0168r, reg_val, FIELD_0_15f);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0169r(unit, &reg_val));
        tx_counter = soc_reg_field_get(unit, OAMP_REG_0169r, reg_val, FIELD_0_15f);
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0154r(unit, &reg_val));
        rx_counter = soc_reg_field_get(unit, OAMP_REG_0154r, reg_val, FIELD_0_15f);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0155r(unit, &reg_val));
        tx_counter = soc_reg_field_get(unit, OAMP_REG_0155r, reg_val, FIELD_0_15f);
    }
    
    LOG_CLI((BSL_META_U(unit,
                              "Rx: %d\n"),  rx_counter));
    LOG_CLI((BSL_META_U(unit,
                              "Tx: %d\n"),  tx_counter));        

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_debug_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int mode
   )
{
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_OUT_OF_RANGE(mode, 0, 1, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_debug_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int cfg,
     SOC_SAND_IN int mode
   )
{
    uint32
       res = SOC_SAND_OK;
    int core;
	uint32 oam_id_arad_mode; 
    uint64 reg_val, field_oam_id_arad_mode;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_JERICHO(unit)) {
        if (!cfg) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr(unit, 0, &reg_val));
            field_oam_id_arad_mode = soc_reg64_field_get(unit, IHP_FLP_GENERAL_CFGr, reg_val, OAM_ID_ARAD_MODEf);
			COMPILER_64_TO_32_LO(oam_id_arad_mode, field_oam_id_arad_mode);
            LOG_CLI((BSL_META_U(unit,
                                      "OAM ID ARAD mode: %d\n"),  oam_id_arad_mode));
        } else {
            for (core = 0; core <= 1; core++) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr(unit, core, &reg_val));
				oam_id_arad_mode = (uint32)mode;   
                COMPILER_64_SET(field_oam_id_arad_mode, 0, oam_id_arad_mode);     
                soc_reg64_field_set(unit, IHP_FLP_GENERAL_CFGr, &reg_val, OAM_ID_ARAD_MODEf, field_oam_id_arad_mode);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_FLP_GENERAL_CFGr(unit, core, reg_val));
            }
        }           
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_oam_id_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_oam_id_unsafe(
     SOC_SAND_IN int unit
   )
{ 
    uint32
       res = SOC_SAND_OK;
    ARAD_PP_DIAG_RECEIVED_PACKET_INFO rpi;
    uint32 offset_byte;
    uint32 offset_word;    
	uint32 offset_bit;
	uint32 pph_type = 0;	
	uint32 dsp_ext_present = 0;
	uint32 oam_ts_type = 0;
	uint32 mep_type = 0;
	uint32 fhei_size = 0;
	uint32 fwd_code = 0;
	uint32 rpi_found = 0;
    uint8 ll_found;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 oam_id;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN] = {0};    
    uint64 reg_val64;
	uint32 oam_id_arad_mode = 0;
    uint64 field_oam_id_arad_mode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	COMPILER_64_SET(field_oam_id_arad_mode, 0, 0);

    /* First Lookup IHB OEMA/B to find the DOWN MEP */  
    res = arad_pp_oam_last_lookup_found(unit, &ll_found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ll_found) {
        GET_SIGNAL_FOR_DIRECTION(fld);
        if (regs_val[0] == 0) { /*direction is down*/
            regs_val[0] = 0;
            GET_SIGNAL_FOR_OAM_ID_DOWN(fld);
            oam_id = regs_val[0];
        } 
    }

    /* Second Lookup RPI to find the UP MEP */ 
    res = arad_pp_diag_received_packet_info_get(unit,&rpi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    offset_byte = SOC_PPC_DIAG_BUFF_MAX_SIZE * 4;
    _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_BASE_SIZE, offset_byte, offset_word, offset_bit);    
    SHR_BITCOPY_RANGE(&pph_type, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 25, 2);
    SHR_BITCOPY_RANGE(&dsp_ext_present, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 3, 1);
    if (pph_type == 3) { /* PPH_Base + OAM_TS */        
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_stacking_ext_mode) {
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_STACKING_EXTENSION_SIZE, offset_byte, offset_word, offset_bit); 
        }
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode) {
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_LB_EXTENSION_SIZE, offset_byte, offset_word, offset_bit); 
        }
        if (dsp_ext_present) {
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_DSP_EXTENSION_SIZE, offset_byte, offset_word, offset_bit); 
        }
        _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_OAM_TS_SIZE, offset_byte, offset_word, offset_bit); 
        SHR_BITCOPY_RANGE(&oam_ts_type, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 46, 2);
        SHR_BITCOPY_RANGE(&mep_type, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 42, 1);
        if (!oam_ts_type && mep_type) { /* Packet and is OAM and UP_MEP */
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_PPH_BASE_SIZE, offset_byte, offset_word, offset_bit); 
            SHR_BITCOPY_RANGE(&fhei_size, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 52, 2);
            SHR_BITCOPY_RANGE(&fwd_code, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 48, 4);
            if ((fhei_size == 1) && (fwd_code == 7)) { /* CPU-Trap */
                _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FHEI_3B_SIZE, offset_byte, offset_word, offset_bit); 
                oam_id = 0;
                SHR_BITCOPY_RANGE(&oam_id, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 8, 16); 
                rpi_found = 1;
            }
        }
    } 

    if (ll_found || rpi_found ) { 
        LOG_CLI((BSL_META_U(unit,
                                "OAM ID is 0x%x.\n"), oam_id));
        /* Print the OAM ID info when OEM-A match and oam_id_arad_mode = 1 */
        if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr(unit, 0, &reg_val64));
            field_oam_id_arad_mode = soc_reg64_field_get(unit, IHP_FLP_GENERAL_CFGr, reg_val64, OAM_ID_ARAD_MODEf);
			COMPILER_64_TO_32_LO(oam_id_arad_mode, field_oam_id_arad_mode);
        }

		/* 
		 * The OAM1 entry key can be extracted when all these conditions have been met:
		 *     1) Chip is Jericho
		 *     2) ARAD-mode-OAM-id is disabled
		 *     3) bit 15 in the OAM ID is 1. This signifies that there was a match in the OEM-1 table.
		 * If those conditions have not been met simply print the OAM-ID.
		 * Otherwise print all the other information have.
		 */    
        if (SOC_IS_JERICHO(unit) && !oam_id_arad_mode &&
            _ARAD_PP_OAM_IS_OEM1_ENTRY(oam_id)) {     
            SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY oam1_key;

            SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
            _ARAD_PP_OAM_OAM1_KEY_TO_KEY_STRUCT(oam_id, oam1_key);
       
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "MEP-Profile (4)", oam1_key.mp_profile));
            LOG_CLI((BSL_META_U(unit,
                            "%16s: %d.\n"), "MP-Type (3)", oam1_key.mp_type_jr));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "Ingress (1)", oam1_key.ingress));  
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "OAM-Opcode (4)", oam1_key.opcode));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "My-CFM-MAC (1)", oam1_key.my_cfm_mac));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "Inject (1)", oam1_key.inject));              
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Lookup not found: OAM subtype, ID cannot be retrieved.\n")));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_ecn_init( SOC_SAND_IN  int unit) 
{

    uint32 _rv = SOC_SAND_OK;
    uint32 res = SOC_SAND_OK;
    uint32 reg32, aux_reg32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    _rv = READ_IHB_BUILD_OAM_TS_HEADERr(unit, SOC_CORE_ALL, &reg32);
    SOCDNX_IF_ERR_EXIT(_rv);

    reg32 = soc_reg_field_get(unit,IHB_BUILD_OAM_TS_HEADERr, reg32, BUILD_OAM_TS_HEADERf);

    /* turn on 7'th bit for oam subtype ecn dm and place it back in the register*/
    aux_reg32 = 128;
    soc_reg_field_set(unit,IHB_BUILD_OAM_TS_HEADERr, &reg32, BUILD_OAM_TS_HEADERf, reg32 | aux_reg32);
    _rv = WRITE_IHB_BUILD_OAM_TS_HEADERr(unit, SOC_CORE_ALL, reg32);
    SOCDNX_IF_ERR_EXIT(_rv);

    /* map sub type 7 to NTP == 2 */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_SUB_TYPE_MAPr(unit, SOC_CORE_ALL,  0x8000)); /* Map each OAM sub type to 2 bit data type. 3 - none, 0 - counter-value, 1 - TOD-1588, 2 - NTP */


    /*TOD*/
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  205,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, TOD_MODEf,  3/*Both 1588 and NTP*/));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_TOD_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TOD_SYNC_ENABLEf,  1));
    /* Enabling NTP TOD */
    /* write 0 to control register */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0, 1);
    /* write value to frac sec lower register */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x13576543, 3);
    /* write value to frac sec upper register */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1ffff00, 4);
    /* write value to frequency register (4 nS in binary fraction) */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x44b82fa1, 2);
    /* write value to time sec register */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x80804560, 5);
    /* write to control register to load values */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f, 1);
    /* write to control register to disable load values */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x0, 1);
    /* write to control register to enable counter */
    _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ecn_init()", 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


