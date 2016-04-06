/* $Id: pb_pp_general.c,v 1.14 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_GENERAL_TYPE_MAX                                 (SOC_PB_PP_NOF_OUTLIF_ENCODE_TYPES-1)
#define SOC_PB_PP_GENERAL_VAL_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_GENERAL_COMMAND_VAL_MAX                          16
#define SOC_PB_PP_GENERAL_PUSH_PROFILE_MAX                         (7)
#define SOC_PB_PP_GENERAL_FRWRD_ACTION_STRENGTH_MAX                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_GENERAL_SNOOP_ACTION_STRENGTH_MAX                (SOC_SAND_U32_MAX)
#define SOC_PB_PP_GENERAL_TPID1_INDEX_MAX                          (3)
#define SOC_PB_PP_GENERAL_TPID2_INDEX_MAX                          (3)
#define SOC_PB_PP_GENERAL_ENTRIES_TO_SCAN_MAX                      (SOC_SAND_U32_MAX)
#define SOC_PB_PP_GENERAL_ENTRIES_TO_ACT_MAX                       (SOC_SAND_U32_MAX)

#define SOC_PB_PP_FRWRD_DECISION_INFO_TYPE_MAX                     (SOC_PB_PP_NOF_FRWRD_DECISION_TYPES-1)

#define SOC_PB_PP_ENCODE_NOF_BITS_EM_FLOW_ID                       (14)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_FEC_PTR                       (14)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_MC_ID                         (14)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE                     (8)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_FWD                      (3)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_SNOOP                    (2)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_LAG_ID                        (8)
#define SOC_PB_PP_ENCODE_NOF_BITS_EM_PHY_PORT                      (12)

#define SOC_PB_PP_ENCODE_NOF_BITS_16_FLOW_ID                       (15)


/* $Id: pb_pp_general.c,v 1.14 Broadcom SDK $
 *	Prefix definition for each encoding
 */
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FLOW               (2)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_FLOW                  (3)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FEC_PTR            (2)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_FEC_PTR               (2)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_MC_ID              (2)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_MC_ID                 (1)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_TRAP               (3)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_TRAP                  (1)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_LAG_ID             (4)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_LAG_ID                (1)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_PHY_PORT           (4)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_PHY_PORT              (0)

#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_16_DROP               (16)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_16_DROP                  (0xFFFF)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_16_FLOW               (1)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_16_FLOW                  (1)

#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_DROP               (17)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_DROP                  (0x1FFFF)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_FEC_PTR            (3)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_FEC_PTR               (2)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_MC_ID              (3)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_MC_ID                 (1)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_LAG_ID             (5)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_LAG_ID                (1)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_PHY_PORT           (5)
#define SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_PHY_PORT              (0)

/*
 *	ASD constants
 */
/* ASD - ILM format */
#define SOC_PB_PP_ASD_ILM_BIT_POSITION_MPLS_LBL (0)
#define SOC_PB_PP_ASD_ILM_NOF_BITS_MPLS_LBL (20)
#define SOC_PB_PP_ASD_ILM_BIT_POSITION_MPLS_CMD (20)
#define SOC_PB_PP_ASD_ILM_NOF_BITS_MPLS_CMD (4)
#define SOC_PB_PP_ASD_ILM_BIT_POSITION_P2P (24)
#define SOC_PB_PP_ASD_ILM_NOF_BITS_P2P (1)
#define SOC_PB_PP_ASD_ILM_VALUE_P2P (0)

/* ASD - MAC format */
#define SOC_PB_PP_ASD_NOF_BITS_MPLS_LABEL                          (20)
#define SOC_PB_PP_ASD_NOF_BITS_MPLS_PUSH_PROFILE                   (3)
#define SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE                        (14)
#define SOC_PB_PP_ASD_BIT_POSITION_SA_DROP                         (23)
#define SOC_PB_PP_ASD_BIT_POSITION_IDENTIFIER                      (20)
#define SOC_PB_PP_ASD_BIT_POSITION_TRILL_MC                        (16)
#define SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_NONE                   (0)
#define SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_TRILL                  (1)
#define SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF                 (3)
#define SOC_PB_PP_ASD_NOF_BITS_TRILL_NICK                          (16)
#define SOC_PB_PP_ASD_OUTLIF_TYPE_EEP                              (0)
#define SOC_PB_PP_ASD_OUTLIF_TYPE_OUT_VSI                          (1)
#define SOC_PB_PP_ASD_OUTLIF_TYPE_AC                               (2)
#define SOC_PB_PP_ASD_NOF_BITS_OUTLIF_TYPE                         (2)
#define SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE                (2)
#define SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION            (18)

#define SOC_PB_PP_DEST_PREFIX_4_MSB_NOF_BITS                       (4)
#define SOC_PB_PP_DEST_ENCODE_5TH_BIT                              (5)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PP_L2_NEXT_PRTCL_UD_NDX_TO_TBL_NDX(ndx) (ndx-1)
#define SOC_PB_PP_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(ndx) (ndx+1)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0000 = 0,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0001 = 1,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0010 = 2,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0011 = 3,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0100 = 4,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0101 = 5,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0110 = 6,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0111 = 7,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1000 = 8,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1001 = 9,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1010 = 10,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1011 = 11,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1100 = 12,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1101 = 13,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1110 = 14,
  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1111 = 15,
  /*
  *  Number of key types
  */
  SOC_PB_PP_DEST_ENCODED_NOF_PREFIXES = 16
}SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE;


typedef enum
{
  /*
   *  MAC-in-MAC. MAC in MAC header will be identified
   *  according special TPID, ISID-TPID.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC = 9,
  /*
   *  Trill packet.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_TRILL = 15,
  /*
   *  IPv4 packet.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV4 = 12,
  /*
   *  IPv6 packet
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV6 = 13,
  /*
   *  ARP packet.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_ARP = 10,
  /*
   *  CFM (Connectivity Fault Management) packet.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_CFM = 11,
  /*
   *  MPLS packet.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MPLS = 8,
  /*
   *  Fiber Channel over Ethernet
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_FC_ETH = 14,
  /*
   *  Other Next protocol type, not one of the above.
   */
  SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER = 0x0,
  /*
   *  Number of types in SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE
   */
  SOC_PB_PP_INTERNAL_NOF_L2_NEXT_PRTCL_TYPES = 9
}SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE;


/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_GENERAL_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_GENERAL_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FWD_DECISION_DEST_PREFIX_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FWD_DECISION_DEST_TYPE_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FWD_DECISION_IN_BUFFER_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FWD_DECISION_IN_BUFFER_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PKT_HDR_TYPE_TO_INTERANL_VAL_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_TO_INTERANL_VAL_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_NEXT_PRTCL_INTERANL_VAL_TO_TYPE_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_ALLOCATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_DEALLOCATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FROM_INTERNAL_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L3_NEXT_PROTOCOL_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L3_NEXT_PROTOCOL_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L3_NEXT_PROTOCOL_FIND),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_TM_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_TM_PORT_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_TM_PORT' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORT_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_PORT' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_FID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_VSI_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_VSI_ID' is out of range. \n\r "
    "this feature supported only to 4K VSIs. \n\r "
    "The range is: 1 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_SYS_VSI_ID' is out of range. \n\r "
    "The range is: 0 - 64*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_FEC_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_VRF_ID' is out of range or Routing disabled. \n\r "
    "The range is: 1 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_AC_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_RIF_ID' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_LIF_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MP_LEVEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MP_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_MP_LEVEL' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_OUTLIF_ENCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_VAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'command' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MPLS_COMMAND_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'push_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR",
    "The parameter 'dest_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid1_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid2_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_scan' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_act' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_ISID_DOMAIN' is out of range. \n\r "
    "The range is: 0 or 16-63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ISID_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_ISID_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PB_PP_ISID_ID' is out of range. \n\r "
    "The range is: 0 - 24*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

    {
    SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'encode_type' of the destination is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_DEST_ENCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR",
    "The parameter EEI.type of the destination is out of range. \n\r "
    "Allowed values are empty, trill amd mpls.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR",
    "The parameter OutLIF.type of the destination is out of range. \n\r "
    "Allowed values are none, eep, out-vsi, ac, trap.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR",
    "The physical system port of the destination is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR",
    "The lag id of the destination is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR",
    "The multicast id of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR",
    "The FEC pointer of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR",
    "The id of the destination is out of range. \n\r "
    "When the action is 'trap', the id must be 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR",
    "The id of the destination is out of range. \n\r "
    "When the action is 'drop', the id must be 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR",
    "The FEC pointer of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for this destination encoding.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_EEI_TYPE_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_EEI_TYPE_INVALID_ERR",
    "The EEI type of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Multicast or FEC types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_CODE_INVALID_ERR,
      "SOC_PB_PP_FRWRD_DEST_TRAP_CODE_INVALID_ERR",
    "The trap code of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The Trap code of the destination is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_FWD_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_TRAP_FWD_INVALID_ERR",
    "The forward strength of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR",
    "The Trap forward strength of the destination is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR",
    "The snoop strength of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR",
    "The Trap snoop strength of the destination is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_EEI_VAL_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_EEI_VAL_INVALID_ERR",
    "The EEI value of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non trill or mpls types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR",
    "The OutLIF value of the destination is invalid (non zero). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR",
    "The nick value of the destination is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR",
    "The OutLIF value of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The push profile value of the destination is out of range. \n\r "
    "The range is: 1 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR",
    "The MPLS label value of the destination is out of range. \n\r "
    "The range is: 0 - 1048575 (1M).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR",
    "The MPLS command value of the destination is out of range. \n\r "
    "The range is: 0 - 0 (only push is accepted).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR,
    "SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR",
    "The OutLIF type of the destination is invalid. \n\r "
    "This parameter is invalid for non Unicast types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR",
    "The flow id of the destination is out of range. \n\r "
    "The range is (according to the encoding): 0 - 16383 (or 32767).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_DROP_ILLEGAL_ERR,
    "SOC_PB_PP_FRWRD_DEST_DROP_ILLEGAL_ERR",
    "the destination cannot be drop in this place use trap. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
    "SOC_PB_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR",
    "The retrieved key has an unknown key prefix. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR,
    "SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR",
    "Feature is not supported in Soc_petra-B. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GEN_NUM_CLEAR_ERR,
    "SOC_PB_PP_GEN_NUM_CLEAR_ERR",
    ". \n\r "
    ".\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MAX_BELOW_MIN_ERR,
    "SOC_PB_PP_MAX_BELOW_MIN_ERR",
    "Max value must be >= than Min value. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR",
    "'eei_type' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_NOF_EEI_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_NOF_FRWRD_DECISION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_FRWRD_DECISION_ASD_FORMAT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_PKT_HDR_TYPE_NOT_SUPPORTED_ERR,
    "SOC_PB_PP_PKT_HDR_TYPE_NOT_SUPPORTED_ERR",
    "packet header stack not supported/expected\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_L2_NEXT_PRTCL_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_L2_NEXT_PRTCL_TYPE_OUT_OF_RANGE_ERR",
    "Layer 2 next header is out of range \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR",
    "system port type can be physical or LAG \n\r",
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
  soc_pb_pp_ce_instruction_build(
    SOC_SAND_IN  SOC_PB_PP_CE_INSTRUCTION         *cek
  )
{
  uint32
    data = 0;

  soc_sand_bitstream_set_any_field(&cek->valid, 0, 1, &data);
  soc_sand_bitstream_set_any_field(&cek->source_select, 1, 1, &data);
  soc_sand_bitstream_set_any_field(&cek->header_offset_select, 2, 3, &data);
  soc_sand_bitstream_set_any_field(&cek->niblle_field_offset, 5, 8, &data);
  soc_sand_bitstream_set_any_field(&cek->bitcount, 13, 6, &data);

  return data;
}

STATIC uint32
  soc_pb_pp_fwd_decision_dest_prefix_find(
    SOC_SAND_IN  SOC_PB_PP_DEST_ENCODE_TYPE    encode_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE fwd_decision_type,
    SOC_SAND_OUT uint32                  *prefix_nof_bits,
    SOC_SAND_OUT uint32                  *prefix_value
    )
{
  uint32
    nof_bits,
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FWD_DECISION_DEST_PREFIX_FIND);

  SOC_SAND_CHECK_NULL_INPUT(prefix_nof_bits);
  SOC_SAND_CHECK_NULL_INPUT(prefix_value);

  switch(fwd_decision_type)
  {
    /*
     *  Single port
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_PHY_PORT;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_PHY_PORT;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_PHY_PORT;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_PHY_PORT;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

    /*
     *  Explicit flow.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FLOW;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_FLOW;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_16_FLOW;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_16_FLOW;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 11, exit);
    }
    break;

    /*
     *  LAG - same value for each encoding
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_LAG_ID;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_LAG_ID;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_LAG_ID;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_LAG_ID;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 12, exit);
    }
    break;

    /*
     *  LIF Group Destination  - same value for each encoding
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_MC_ID;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_MC_ID;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_MC_ID;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_MC_ID;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 13, exit);
    }
    break;

    /*
     *  FEC Destination.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FEC_PTR;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_FEC_PTR;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_FEC_PTR;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_FEC_PTR;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 14, exit);
    }
    break;

   /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  The id must be null.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_EM_TRAP;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_EM_TRAP;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 15, exit);
    }
    break;

    /*
     *  Drop destination
     *  The id must be null.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_16_DROP;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_16_DROP;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = SOC_PB_PP_ENCODE_DEST_PREFIX_NOF_BITS_17_DROP;
      value = SOC_PB_PP_ENCODE_DEST_PREFIX_VALUE_17_DROP;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 16, exit);
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 17, exit);
  }

  *prefix_value = value;
  *prefix_nof_bits = nof_bits;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fwd_decision_dest_prefix_find()", 0, 0);
}

STATIC uint32
  soc_pb_pp_fwd_decision_encode_type_get(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_OUT  SOC_PB_PP_DEST_ENCODE_TYPE     *encode_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(encode_type);
 
  switch(appl_type)
  {
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_16:
    *encode_type = SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS;
    break;
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_IP:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL:
    *encode_type = SOC_PB_PP_DEST_ENCODE_TYPE_EM;
    break;

  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRAP:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_PMF:
  case SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_17:
    *encode_type = SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fwd_decision_encode_type_get()", 0, 0);
}

STATIC uint32
  soc_pb_pp_fwd_decision_dest_type_find(
      SOC_SAND_IN  SOC_PB_PP_DEST_ENCODE_TYPE     encode_type,
      SOC_SAND_IN  uint32                   dest_buffer,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_TYPE  *decision_type
  )
{
  uint8
    fifth_bit_17_encoding;
  uint32
    buffer_size,
    buffer_4_msb = 0,
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_DECISION_TYPE
    decision_type_lcl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FWD_DECISION_DEST_TYPE_FIND);

  SOC_SAND_CHECK_NULL_INPUT(decision_type);

  /*
   *	Decide according to the 4 msb, and then according to the encoding type
   */
  switch(encode_type)
  {
  case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    buffer_size = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_EM;
    break;

  case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    buffer_size = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_16;
    break;

  case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    buffer_size = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_17;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_sand_bitstream_get_any_field(
          &(dest_buffer),
          buffer_size - SOC_PB_PP_DEST_PREFIX_4_MSB_NOF_BITS,
          SOC_PB_PP_DEST_PREFIX_4_MSB_NOF_BITS,
          &(buffer_4_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(buffer_4_msb)
  {
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1100:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1101:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1110:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1111:
    decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW;
    break;

  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1000:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1001:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1010:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_1011:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0110:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0111:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_MC;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0100:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0101:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_MC;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC;
      break;
    /* must have default. Otherwise, compilation error */
    /* coverity[dead_error_begin : FALSE] */
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0010:
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0011:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_MC;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0001:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;
  case  SOC_PB_PP_DEST_ENCODED_PREFIX_TYPE_0000:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT;
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS: /* Depends on the 5th bit*/
      fifth_bit_17_encoding = SOC_SAND_GET_BIT(dest_buffer, buffer_size - SOC_PB_PP_DEST_ENCODE_5TH_BIT);
        if (fifth_bit_17_encoding == 1)
        {
          decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG;
        }
        else
        {
          decision_type_lcl = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT;
        }
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 20, exit);
  }

  *decision_type = decision_type_lcl;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fwd_decision_dest_type_find()", 0, 0);
}

/*********************************************************************
*     Convert a forwarding decision to a buffer according to the
*     encoding type and the SA drop bit (for the ASD encoding)
*********************************************************************/

uint32
  soc_pb_pp_fwd_decision_in_buffer_build(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_IN  uint8                 is_sa_drop,
    SOC_SAND_OUT uint32                   *dest_buffer,
    SOC_SAND_OUT uint32                   *asd_buffer
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp = 0,
    outlif_or_trill_type = 0,
    asd_outlif_type,
    dest_buffer_lcl = 0,
    asd_buffer_lcl = 0,
    param_id_length,
    prefix_value,
    is_outlif = 0,
    prefix_nof_bits,
    is_sa_drop_lcl = is_sa_drop,
    is_multicast = 0;
  SOC_PB_PP_DEST_ENCODE_TYPE
    encode_type;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FWD_DECISION_IN_BUFFER_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(fwd_decision);
  SOC_SAND_CHECK_NULL_INPUT(dest_buffer);
  SOC_SAND_CHECK_NULL_INPUT(asd_buffer);

  /*
   *	Verify the inputs and their combination
   */
  SOC_PB_PP_FRWRD_DECISION_INFO_verify(fwd_decision);

  res = soc_pb_pp_fwd_decision_encode_type_get(
          appl_type,
          &encode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = SOC_PB_PP_FRWRD_DECISION_INFO_with_encode_type_verify(
          appl_type,
          fwd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Find the right prefix according to the destination type
   *  and then the encoding type
   *  The good id length is the total length - prefix length
   */
  prefix_value = 0;
  prefix_nof_bits = 0;
  res = soc_pb_pp_fwd_decision_dest_prefix_find(
          encode_type,
          fwd_decision->type,
          &(prefix_nof_bits),
          &(prefix_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	Set the good parameter length (except for the trap in the EM encoding)
   */
  switch(encode_type)
  {
  case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    param_id_length = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_EM - prefix_nof_bits;
    break;

  case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    param_id_length = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_16 - prefix_nof_bits;
    break;

  case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    param_id_length = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_17 - prefix_nof_bits;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }

  /*
   *	Set the prefix (also for the trap)
   */
   res = soc_sand_bitstream_set_any_field(
            &(prefix_value),
            param_id_length,
            prefix_nof_bits,
            &(dest_buffer_lcl)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *	Build the destination buffer according to the destination type
   *  and then the encoding type
   */
  switch(fwd_decision->type)
  {
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
     res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->dest_id),
              0,
              param_id_length,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;

    /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  Only for the EM encoding (already verified).
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
    res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
          fwd_decision->additional_info.trap_info.action_profile.trap_code,
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    temp = (uint32)trap_code_internal;

    res = soc_sand_bitstream_set_any_field(
              &(temp),
              0,
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    temp = fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength;
    res = soc_sand_bitstream_set_any_field(
              &(temp),
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

    res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength),
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE + SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_SNOOP,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }



  if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM)
  {
    if (fwd_decision->additional_info.outlif.type == SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP)
    {
        outlif_or_trill_type = SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF;
        switch(fwd_decision->additional_info.outlif.type)
        {
        case SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC:
          asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_AC;
          break;

        case SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI:
          asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_OUT_VSI;
          break;

        case SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP:
          asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_EEP;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 20, exit);
        }

        res = soc_sand_bitstream_set_any_field(
                &(outlif_or_trill_type),
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


        res = soc_sand_bitstream_set_any_field(
                &(asd_outlif_type),
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        res = soc_sand_bitstream_set_any_field(
                &(fwd_decision->additional_info.outlif.val),
                0,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);

    }
    else
    {
      res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.eei.val.mpls_command.label),
              SOC_PB_PP_ASD_ILM_BIT_POSITION_MPLS_LBL,
              SOC_PB_PP_ASD_ILM_NOF_BITS_MPLS_LBL,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

      temp = fwd_decision->additional_info.eei.val.mpls_command.command;

      if (fwd_decision->additional_info.eei.val.mpls_command.command == SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH)
      {
        temp = fwd_decision->additional_info.eei.val.mpls_command.push_profile;
      }
      
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              SOC_PB_PP_ASD_ILM_BIT_POSITION_MPLS_CMD,
              SOC_PB_PP_ASD_ILM_NOF_BITS_MPLS_CMD,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

      temp = SOC_PB_PP_ASD_ILM_VALUE_P2P;
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              SOC_PB_PP_ASD_ILM_BIT_POSITION_P2P,
              SOC_PB_PP_ASD_ILM_NOF_BITS_P2P,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);
    }
  }
  else if (
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT) ||
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P) ||
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP) ||
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED)
          )
  {
    /*
     *	Build the ASD buffer
     */
    res = soc_sand_bitstream_set_any_field(
            &(is_sa_drop_lcl),
            SOC_PB_PP_ASD_BIT_POSITION_SA_DROP,
            1,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if (fwd_decision->additional_info.eei.type == SOC_PB_PP_EEI_TYPE_MPLS)
    {
      temp = fwd_decision->additional_info.eei.val.mpls_command.command;

      if (fwd_decision->additional_info.eei.val.mpls_command.command == SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH)
      {
        temp = fwd_decision->additional_info.eei.val.mpls_command.push_profile;
      }

      res = soc_sand_bitstream_set_any_field(
              &(temp),
              SOC_PB_PP_ASD_BIT_POSITION_IDENTIFIER,
              SOC_PB_PP_ASD_NOF_BITS_MPLS_PUSH_PROFILE,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

      res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.eei.val.mpls_command.label),
              0,
              SOC_PB_PP_ASD_NOF_BITS_MPLS_LABEL,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    }
    else
    {
      if (fwd_decision->additional_info.eei.type == SOC_PB_PP_EEI_TYPE_TRILL)
      {
        outlif_or_trill_type = SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_TRILL;
        
        is_multicast = fwd_decision->additional_info.eei.val.trill_dest.is_multicast;
        res = soc_sand_bitstream_set_any_field(
                &(is_multicast),
                SOC_PB_PP_ASD_BIT_POSITION_TRILL_MC,
                1,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

         res = soc_sand_bitstream_set_any_field(
                &(fwd_decision->additional_info.eei.val.trill_dest.dest_nick),
                0,
                SOC_PB_PP_ASD_NOF_BITS_TRILL_NICK,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 81, exit);
      }
      else if (fwd_decision->additional_info.eei.type == SOC_PB_PP_EEI_TYPE_MIM)
      {
          asd_buffer_lcl = fwd_decision->additional_info.eei.val.isid;
      }
      else if (
                (fwd_decision->additional_info.outlif.type <= SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI)
                && (fwd_decision->additional_info.outlif.type >= SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC)
               )
      {
        is_outlif = 1;
        outlif_or_trill_type = SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF;
   
        switch(fwd_decision->additional_info.outlif.type)
        {
        case SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC:
          asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_AC;
          break;

        case SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI:
          asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_OUT_VSI;
          break;

        case SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP:
          asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_EEP;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 20, exit);
        }

        res = soc_sand_bitstream_set_any_field(
                &(asd_outlif_type),
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        res = soc_sand_bitstream_set_any_field(
                &(fwd_decision->additional_info.outlif.val),
                0,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
      }

      /*
       *	Use of ASD and not outlif
       */
      if(
          (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT)
          ||(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP)
          ||(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED)
        )
      {
        res = soc_sand_bitstream_set_any_field(
                &(outlif_or_trill_type),
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      }
    
      /*
       *	Use of ASD and not outlif
       */
      if((appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED))
      {
        res = soc_sand_bitstream_set_any_field(
                &(is_outlif),
                16,
                1,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
      }

    }
  }

  else if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC)
  {
    switch(fwd_decision->additional_info.outlif.type)
    {
    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC:
      asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_AC;
      break;

    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI:
      asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_OUT_VSI;
      break;

    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP:
    default:
      asd_outlif_type = SOC_PB_PP_ASD_OUTLIF_TYPE_EEP;
      break;
    }

    res = soc_sand_bitstream_set_any_field(
            &(asd_outlif_type),
            SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
            SOC_PB_PP_ASD_NOF_BITS_OUTLIF_TYPE,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = soc_sand_bitstream_set_any_field(
            &(fwd_decision->additional_info.outlif.val),
            0,
            SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
  }
  *dest_buffer = dest_buffer_lcl;
  *asd_buffer = asd_buffer_lcl;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fwd_decision_in_buffer_build()", 0, 0);
}

/*********************************************************************
*     Parse an ASD and a destination buffer to get the forward
*     decision and the SA drop (from the ASD msb)
*********************************************************************/
uint32
  soc_pb_pp_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  uint32                  dest_buffer,
    SOC_SAND_IN  uint32                  asd_buffer,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT uint8                 *is_sa_drop
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_DECISION_TYPE
    decision_type=0;
  uint32
    temp = 0,
    push_profile = 0,
    outlif_or_trill_type = 0,
    asd_outlif_type = 0,
    dest_buffer_lcl = dest_buffer,
    asd_buffer_lcl = asd_buffer,
    param_id_length,
    prefix_value,
    prefix_nof_bits,
    is_sa_drop_lcl = 0,
    is_multicast = 0;
  SOC_PB_PP_DEST_ENCODE_TYPE
    encode_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FWD_DECISION_IN_BUFFER_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(fwd_decision);
  SOC_SAND_CHECK_NULL_INPUT(is_sa_drop);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(fwd_decision);

  res = soc_pb_pp_fwd_decision_encode_type_get(
          appl_type,
          &encode_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  /*
   *	Parse the destination buffer, first find the type
   */
  fwd_decision->dest_id = 0;
  res = soc_pb_pp_fwd_decision_dest_type_find(
          encode_type,
          dest_buffer,
          &(decision_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /*
   *	Find the other parameters of the forward decision
   */
  fwd_decision->type = decision_type;

  /*
   *	Find the right prefix according to the destination type
   *  and then the encoding type
   *  The good id length is the total length - prefix length
   */
  prefix_value = 0;
  prefix_nof_bits = 0;
  res = soc_pb_pp_fwd_decision_dest_prefix_find(
          encode_type,
          decision_type,
          &(prefix_nof_bits),
          &(prefix_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /*
   *	Set the good parameter length (except for the trap in the EM encoding)
   */
  switch(encode_type)
  {
  case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    param_id_length = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_EM - prefix_nof_bits;
    break;

  case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    param_id_length = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_16 - prefix_nof_bits;
    break;

  case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    param_id_length = SOC_PB_PP_DEST_ENCODE_TOTAL_IN_BITS_17 - prefix_nof_bits;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }
  /*
   *	Build the destination buffer according to the destination type
   *  and then the encoding type
   */
  switch(decision_type)
  {
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
     res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              0,
              param_id_length,
              &(fwd_decision->dest_id)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
     res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              0,
              8,
              &(fwd_decision->dest_id)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;

    /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  Only for the EM encoding (already verified).
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
     res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              0,
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              &temp
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(0,temp,&fwd_decision->additional_info.trap_info.action_profile.trap_code);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              &(fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

    res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_CODE + SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_SNOOP,
              &(fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 70, exit);
  }

  if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM)
  {
    uint32
      command = 0;

    fwd_decision->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MPLS;
    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            SOC_PB_PP_ASD_ILM_BIT_POSITION_MPLS_LBL,
            SOC_PB_PP_ASD_ILM_NOF_BITS_MPLS_LBL,
            &(fwd_decision->additional_info.eei.val.mpls_command.label)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            SOC_PB_PP_ASD_ILM_BIT_POSITION_MPLS_CMD,
            SOC_PB_PP_ASD_ILM_NOF_BITS_MPLS_CMD,
            &(command)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);
    fwd_decision->additional_info.eei.val.mpls_command.command = command;

    if (command <= 7)
    {
      fwd_decision->additional_info.eei.val.mpls_command.command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;
      fwd_decision->additional_info.eei.val.mpls_command.push_profile = command;
    }

    
  }
  else if (
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT) ||
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P) ||
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED) ||
           (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP)
          )
  {
    /*
     *	Get the ASD buffer
     */
    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            SOC_PB_PP_ASD_BIT_POSITION_SA_DROP,
            1,
            &is_sa_drop_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    *is_sa_drop = SOC_SAND_NUM2BOOL(is_sa_drop_lcl);

    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            SOC_PB_PP_ASD_BIT_POSITION_IDENTIFIER,
            SOC_PB_PP_ASD_NOF_BITS_MPLS_PUSH_PROFILE,
            &(push_profile)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (push_profile != 0)
    {
      fwd_decision->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MPLS;
      fwd_decision->additional_info.eei.val.mpls_command.push_profile = push_profile;
      fwd_decision->additional_info.eei.val.mpls_command.command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;

      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer_lcl),
              0,
              SOC_PB_PP_ASD_NOF_BITS_MPLS_LABEL,
              &(fwd_decision->additional_info.eei.val.mpls_command.label)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }
    else
    {
       res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE,
                &(outlif_or_trill_type)
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      /*
       *	Override in case of P2P (not written in its ASD-Outlif)
       */
      if (appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P)
      {
        outlif_or_trill_type = SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF;
      }

      if (outlif_or_trill_type == SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_TRILL)
      {
        fwd_decision->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_TRILL;
              
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                SOC_PB_PP_ASD_BIT_POSITION_TRILL_MC,
                1,
                &(is_multicast)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
        fwd_decision->additional_info.eei.val.trill_dest.is_multicast = SOC_SAND_NUM2BOOL(is_multicast);

         res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                0,
                SOC_PB_PP_ASD_NOF_BITS_TRILL_NICK,
                &(fwd_decision->additional_info.eei.val.trill_dest.dest_nick)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
      }
      else if (outlif_or_trill_type == SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF)
      {
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_TYPE,
                &(asd_outlif_type)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
        switch(asd_outlif_type)
        {
        case SOC_PB_PP_ASD_OUTLIF_TYPE_AC:
          fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC;
          break;

        case SOC_PB_PP_ASD_OUTLIF_TYPE_OUT_VSI:
          fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI;
          break;

        case SOC_PB_PP_ASD_OUTLIF_TYPE_EEP:
          fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 150, exit);
        }

        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                0,
                SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
                &(fwd_decision->additional_info.outlif.val)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
      }
      else if(outlif_or_trill_type == SOC_PB_PP_ASD_TRILL_VS_OUTLIF_VALUE_NONE)
      {
        switch(decision_type)
        {
        case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
        case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
        case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
          fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;
          fwd_decision->additional_info.outlif.val = 0;
          break;

        case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
        case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:
          fwd_decision->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;
          break;

        case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
        case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
          break;

        /* must have default. Otherwise, compilation error */
        /* coverity[dead_error_begin : FALSE] */
        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 70, exit);
        }
      }
    }
  }
  else if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRAP)
  {
    if(fwd_decision->type == SOC_PB_PP_FRWRD_DECISION_TYPE_MC || fwd_decision->type == SOC_PB_PP_FRWRD_DECISION_TYPE_FEC)
    {
      fwd_decision->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;
    }
    else
    {
      fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;
    }
  }
  else if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC)
  {
    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
            SOC_PB_PP_ASD_NOF_BITS_OUTLIF_TYPE,
            &(asd_outlif_type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    switch(asd_outlif_type)
    {
    case SOC_PB_PP_ASD_OUTLIF_TYPE_AC:
      fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC;
      break;

    case SOC_PB_PP_ASD_OUTLIF_TYPE_OUT_VSI:
      fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI;
      break;

    case SOC_PB_PP_ASD_OUTLIF_TYPE_EEP:
      fwd_decision->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 150, exit);
    }

    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            0,
            SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE,
            &(fwd_decision->additional_info.outlif.val)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }
  /*
   *	Verify the parsing has a legal output
   
  res = SOC_PB_PP_FRWRD_DECISION_INFO_with_encode_type_verify(
          appl_type,
          fwd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fwd_decision_in_buffer_parse()", 0, 0);
}


uint32
  soc_pb_pp_pkt_hdr_type_to_interanl_val_map(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_STK_TYPE    pkt_hdr_type,
    SOC_SAND_OUT uint32                  *internal_val
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PKT_HDR_TYPE_TO_INTERANL_VAL_MAP);

  SOC_SAND_CHECK_NULL_INPUT(internal_val);
  switch(pkt_hdr_type)
  {
  case SOC_PB_PP_PKT_HDR_STK_TYPE_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS1_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS1_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS2_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS2_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS3_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS3_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_TRILL_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_TRILL_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH;
  break;
  case SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH:
    *internal_val = SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH;
  break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_PKT_HDR_TYPE_NOT_SUPPORTED_ERR, 100, exit);
}exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_pkt_hdr_type_to_interanl_val_map()", 0, 0);
}

uint32
  soc_pb_pp_pkt_hdr_interanl_val_to_type_map(
    SOC_SAND_IN  uint32                  internal_val,
    SOC_SAND_OUT SOC_PB_PP_PKT_HDR_STK_TYPE    *pkt_hdr_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP);

  SOC_SAND_CHECK_NULL_INPUT(pkt_hdr_type);

  switch(internal_val)
  {
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS1_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS1_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS2_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS2_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_MPLS3_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS3_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_TRILL_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_TRILL_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH;
  break;
  case SOC_PB_PP_INTERNAL_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH:
    *pkt_hdr_type = SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH;
  break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_PKT_HDR_TYPE_NOT_SUPPORTED_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_pkt_hdr_interanl_val_to_type_map()", 0, 0);
}

uint32
  soc_pb_pp_l2_next_prtcl_type_to_interanl_val_map(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_NEXT_PRTCL_TYPE    l2_next_prtcl_type,
    SOC_SAND_OUT uint32                    *internal_val,
    SOC_SAND_OUT uint8                   *found
  )
{
  uint32
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP);

  SOC_SAND_CHECK_NULL_INPUT(internal_val);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->eci.ethernet_type_trill_configuration_reg_reg.ethertype_trill, fld_val, 10, exit);
  *found = FALSE;
  if(fld_val == l2_next_prtcl_type)
  {
    *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_TRILL;
    *found = TRUE;
  }
  else 
  {
    SOC_PB_PP_FLD_GET(regs->eci.ethernet_type_mpls_configuration_reg_reg.ethertype_mpls0, fld_val, 20, exit);
    if(fld_val == l2_next_prtcl_type)
    {
      *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MPLS;
      *found = TRUE;
    }
  }    

  if(*found == FALSE)
  {
    *found = TRUE;
    switch(l2_next_prtcl_type)
    {
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC;
      break;
      break;
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_IPV4:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV4;
      break;
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_IPV6:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV6;
      break;
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_ARP:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_ARP;
      break;
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_CFM:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_CFM;
      break;
      break;
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FC_ETH:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_FC_ETH;
      break;
      case SOC_PB_PP_L2_NEXT_PRTCL_TYPE_OTHER:
        *internal_val = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER;
      break;
      default:
        *found = FALSE;
      break;;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_next_prtcl_type_to_interanl_val_map()", 0, 0);
}

uint32
  soc_pb_pp_l2_next_prtcl_interanl_val_to_type_map(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  internal_val,
    SOC_SAND_OUT SOC_PB_PP_L2_NEXT_PRTCL_TYPE    *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                   *found
  )
{
  uint32
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_NEXT_PRTCL_INTERANL_VAL_TO_TYPE_MAP);

  SOC_SAND_CHECK_NULL_INPUT(l2_next_prtcl_type);

  regs = soc_pb_pp_regs();
  *found = TRUE;

  switch(internal_val)
  {
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_TRILL:
      SOC_PB_PP_FLD_GET(regs->eci.ethernet_type_trill_configuration_reg_reg.ethertype_trill, fld_val, 10, exit);
      *l2_next_prtcl_type = fld_val;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV4:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_IPV4;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV6:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_IPV6;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_ARP:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_ARP;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_CFM:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_CFM;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MPLS:
      SOC_PB_PP_FLD_GET(regs->eci.ethernet_type_mpls_configuration_reg_reg.ethertype_mpls0, fld_val, 20, exit);
      *l2_next_prtcl_type = fld_val;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_FC_ETH:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FC_ETH;
    break;
    case SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER:
      *l2_next_prtcl_type = SOC_PB_PP_L2_NEXT_PRTCL_TYPE_OTHER;
    break;
    default:
      *found = FALSE;
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_pkt_hdr_interanl_val_to_type_map()", 0, 0);
}

uint32
  soc_pb_pp_l2_next_prtcl_type_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT uint8                 *found
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_val,
    prtcl_tbl_ndx,
    ref_count;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FIND);

  SOC_SAND_CHECK_NULL_INPUT(internal_ndx);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_next_prtcl_type_to_interanl_val_map(
          unit,
          l2_next_prtcl_type,
          &internal_val,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
    *internal_ndx = internal_val;
  }
  else
  {
    /* l2_next_prtcl_type is not one of the fixed values. Search in sw_db */
    res = soc_pb_sw_db_multiset_lookup(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_ETHER_TYPE,
            l2_next_prtcl_type,
            &prtcl_tbl_ndx,
            &ref_count
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ref_count == 0)
    {
      *found = FALSE;
    }
    else
    {
      *internal_ndx = SOC_PB_PP_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(prtcl_tbl_ndx);
      *found = TRUE;
    }
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_next_prtcl_type_allocate()", 0, 0);
}

uint32
  soc_pb_pp_l2_next_prtcl_type_from_internal_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  internal_ndx,
    SOC_SAND_OUT uint32                  *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                 *found
  )
{
  uint32
    res = SOC_SAND_OK,
    prtcl_tbl_ndx;
  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
    tbl_data;
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE
    l2_next_prtcl_type_enum;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FROM_INTERNAL_FIND);

  SOC_SAND_CHECK_NULL_INPUT(l2_next_prtcl_type);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_next_prtcl_interanl_val_to_type_map(
          unit,
          internal_ndx,
          &l2_next_prtcl_type_enum,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
    *l2_next_prtcl_type = (uint32)l2_next_prtcl_type_enum;
  }
  else
  {
    prtcl_tbl_ndx = SOC_PB_PP_L2_NEXT_PRTCL_UD_NDX_TO_TBL_NDX(internal_ndx);

    /* l2_next_prtcl_type is not one of the fixed values. Get from table */
    res = soc_pb_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
            unit,
            prtcl_tbl_ndx,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    *l2_next_prtcl_type = tbl_data.eth_type_protocol;
    *found = TRUE;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_next_prtcl_type_allocate()", 0, 0);
}

uint32
  soc_pb_pp_l2_next_prtcl_type_allocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE      *success
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_val,
    prtcl_tbl_ndx;
  uint8
    first_appear,
    found = 0xff; /* putting extrene value for case of using uninitialized variable */
  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_ALLOCATE);

  SOC_SAND_CHECK_NULL_INPUT(internal_ndx);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_next_prtcl_type_to_interanl_val_map(
          unit,
          l2_next_prtcl_type,
          &internal_val,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (found)
  {
    *internal_ndx = internal_val;
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    /* This is not a predefined protocol type. Add to SW DB */
    res = soc_pb_sw_db_multiset_add(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_ETHER_TYPE,
            l2_next_prtcl_type,
            &prtcl_tbl_ndx,
            &first_appear,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if ((*success == SOC_SAND_SUCCESS) && first_appear)
    {
      /* Add new user defined ether_type to ihp eth protocol table */
      res = soc_pb_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      tbl_data.eth_type_protocol = l2_next_prtcl_type;
      tbl_data.eth_sap_protocol = l2_next_prtcl_type; /* set for both ETh/SAP-DSAP*/

      res = soc_pb_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    *internal_ndx = SOC_PB_PP_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(prtcl_tbl_ndx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_next_prtcl_type_allocate()", 0, 0);
}

uint32
  soc_pb_pp_l2_next_prtcl_type_deallocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_val,
    prtcl_tbl_ndx;
  uint8
    last_appear,
    found = 0xff; /* putting extrene value for case of using uninitialized variable */
  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_NEXT_PRTCL_TYPE_DEALLOCATE);

  res = soc_pb_pp_l2_next_prtcl_type_to_interanl_val_map(
          unit,
          l2_next_prtcl_type,
          &internal_val,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!found)
  {
    /* This is not a predefined protocol type. Remove from SW DB */
    res = soc_pb_sw_db_multiset_remove(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_ETHER_TYPE,
            l2_next_prtcl_type,
            &prtcl_tbl_ndx,
            &last_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (last_appear)
    {
      /* Add new user defined ether_type to ihp eth protocol table */
      res = soc_pb_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      tbl_data.eth_type_protocol = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER;
      tbl_data.eth_sap_protocol = SOC_PB_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER;

      res = soc_pb_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }
  else
  {
    /* Nothing to do. Ether_type is a predefined value */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_next_prtcl_type_allocate()", 0, 0);
}

#define SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(prtcl_suffix) \
  case (SOC_PB_PP_L3_NEXT_PRTCL_TYPE_##prtcl_suffix): \
    *ndx = SOC_PB_PP_L3_NEXT_PRTCL_NDX_##prtcl_suffix; \
    break;

uint32
  soc_pb_pp_l3_prtcl_to_ndx(
    SOC_SAND_IN  SOC_PB_PP_L3_NEXT_PRTCL_TYPE prtcl_type,
    SOC_SAND_OUT uint8 *ndx
  )
{
  switch (prtcl_type)
  {
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(NONE)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(TCP)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(UDP)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(IGMP)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(ICMP)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(ICMPV6)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(IPV4)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(IPV6)
  SOC_PB_PP_L3_PRTCL_TO_NDX_CASE(MPLS)
  default:
    *ndx = SOC_PB_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED;
    break;
  }

  return SOC_SAND_OK;
}

#define SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(prtcl_suffix) \
  case (SOC_PB_PP_L3_NEXT_PRTCL_NDX_##prtcl_suffix): \
  *prtcl_type = SOC_PB_PP_L3_NEXT_PRTCL_TYPE_##prtcl_suffix; \
  break;

uint32
  soc_pb_pp_llp_trap_ndx_to_l3_prtcl(
    SOC_SAND_IN  uint8 ndx,
    SOC_SAND_OUT SOC_PB_PP_L3_NEXT_PRTCL_TYPE *prtcl_type,
    SOC_SAND_OUT uint8 *is_found
  )
{
  *is_found = TRUE;

  switch (ndx)
  {
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(NONE)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(TCP)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(UDP)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(IGMP)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(ICMP)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(ICMPV6)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(IPV4)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(IPV6)
  SOC_PB_PP_L3_NDX_TO_PRTCL_CASE(MPLS)
  default:
    *is_found = FALSE;
    break;
  }

  return SOC_SAND_OK;
}


uint32
  soc_pb_pp_l3_next_protocol_add(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8               *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32
    res = SOC_SAND_OK,
    data;
  uint8
    ip_protocol_ndx;
  uint8
    ip_protocol_add_to_hw;
  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA
    ihp_parser_ip_protocols_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L3_NEXT_PROTOCOL_ADD);

  *success = SOC_SAND_SUCCESS;

  /* Check that l3 protocol is a pre-defined one, or user-defined but previously
  allocated, or that there's space for one more */

  res = soc_pb_pp_l3_prtcl_to_ndx(
          next_protocol_ndx,
          internal_ndx
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (*internal_ndx == SOC_PB_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED)
  {
    /* Add new user-defined protocol */
    res = soc_pb_sw_db_multiset_add(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_L3_PROTOCOL,
            next_protocol_ndx,
            &data,
            &ip_protocol_add_to_hw,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    ip_protocol_ndx = (uint8)(SOC_PB_PP_IP_PROTOCOL_UD_NDX_TO_FIELD_VAL(data));

    if (ip_protocol_add_to_hw)
    {
      /* First appearance in db. Add to HW */
      ihp_parser_ip_protocols_tbl_data.ip_protocol = next_protocol_ndx;

      res = soc_pb_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
              unit,
              data,
              &ihp_parser_ip_protocols_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

    *internal_ndx = ip_protocol_ndx;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l3_next_protocol_add()", 0, 0);
}


uint32
  soc_pb_pp_l3_next_protocol_remove(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    internal_ndx;
  uint8
    is_found,
    last_appear;
  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA
    ihp_parser_ip_protocols_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L3_NEXT_PROTOCOL_REMOVE);

  /* Check that l3 protocol is a pre-defined one, or user-defined but previously
  allocated, or that there's space for one more */

  res = soc_pb_pp_l3_next_protocol_find(
          unit,
          next_protocol_ndx,
          &internal_ndx,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_found)
  {
    /* Protocol for current trap index is different. Let's remove it */
    res = soc_pb_sw_db_multiset_remove_by_index(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_L3_PROTOCOL,
            internal_ndx,
            &last_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (last_appear)
    {
      /* Remove from HW */
      ihp_parser_ip_protocols_tbl_data.ip_protocol =
        SOC_PB_PP_L3_NEXT_PRTCL_NDX_NONE;

      res = soc_pb_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
              unit,
              SOC_PB_PP_IP_FIELD_VAL_TO_PROTOCOL_UD_NDX(internal_ndx),
              &ihp_parser_ip_protocols_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l3_next_protocol_remove()", 0, 0);
}


uint32
  soc_pb_pp_l3_next_protocol_find(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8               *internal_ndx,
    SOC_SAND_OUT uint8             *is_found
  )
{
  uint32
    res = SOC_SAND_OK,
    ref_count,
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L3_NEXT_PROTOCOL_FIND);

  /* Check that l3 protocol is a pre-defined one, or user-defined but previously
  allocated, or that there's space for one more */

  res = soc_pb_pp_l3_prtcl_to_ndx(
          next_protocol_ndx,
          internal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  *is_found = TRUE;

  if (*internal_ndx == SOC_PB_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED)
  {
    *is_found = FALSE;
    res = soc_pb_sw_db_multiset_lookup(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_L3_PROTOCOL,
            next_protocol_ndx,
            &data,
            &ref_count
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

    if (ref_count > 0)
    {
      *is_found = TRUE;
      *internal_ndx = (uint8)(SOC_PB_PP_IP_PROTOCOL_UD_NDX_TO_FIELD_VAL(data));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l3_next_protocol_find()", 0, 0);
}



/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_general module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_general_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_general;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_general module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_general_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_general;
}

uint32
  SOC_PB_PP_TRAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, &(info->action_profile), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_OUTLIF_verify(
    SOC_SAND_IN  SOC_PB_PP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_NOF_OUTLIF_ENCODE_TYPES, SOC_PB_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->val, SOC_PB_PP_GENERAL_VAL_MAX, SOC_PB_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_OUTLIF_verify()",0,0);
}

uint32
  SOC_PB_PP_MPLS_COMMAND_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->command, SOC_PB_PP_GENERAL_COMMAND_VAL_MAX, SOC_PB_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, SOC_PB_PP_GENERAL_PUSH_PROFILE_MAX, SOC_PB_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MPLS_COMMAND_verify()",0,0);
}

uint32
  SOC_PB_PP_EEI_verify(
    SOC_SAND_IN  SOC_PB_PP_EEI *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_NOF_EEI_TYPES, SOC_PB_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  if (info->type == SOC_PB_PP_EEI_TYPE_TRILL)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_PP_TRILL_DEST, &(info->val.trill_dest), 20, exit);
  }
  else if (info->type == SOC_PB_PP_EEI_TYPE_MPLS)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MPLS_COMMAND, &(info->val.mpls_command), 21, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EEI_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE       type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
    break;
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_OUTLIF, &(info->outlif), 11, exit);
    break;

  case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EEI, &(info->eei), 10, exit);
    break;

  case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRAP_INFO, &(info->trap_info), 12, exit);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 18, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_DECISION_INFO_TYPE_MAX, SOC_PB_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  res = SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_verify(
          info->type,
          &(info->additional_info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_DECISION_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_DECISION_INFO_with_encode_type_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_DEST_ENCODE_TYPE    encode_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(appl_type, SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPES, SOC_PB_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR, 15, exit);

  res = soc_pb_pp_fwd_decision_encode_type_get(
          appl_type,
          &encode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
 /*
  * EM encoding cannot hold drop
  */
  if (encode_type == SOC_PB_PP_DEST_ENCODE_TYPE_EM && info->type == SOC_PB_PP_FRWRD_DECISION_TYPE_DROP)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_DROP_ILLEGAL_ERR, 17, exit);
  }
  /*
   *	Verify the values of the destination id according to the encoding type
   */
  switch(info->type)
  {
    /*
     *  Single port - same value for each encoding
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_PHY_PORT)-1, SOC_PB_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR, 12, exit);
    break;

    /*
     *  Explicit flow.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_FLOW_ID)-1, SOC_PB_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR, 14, exit);
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<SOC_PB_PP_ENCODE_NOF_BITS_16_FLOW_ID)-1, SOC_PB_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR, 16, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 18, exit);
    }
    break;

    /*
     *  LAG - same value for each encoding
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_LAG_ID)-1, SOC_PB_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR, 20, exit);
    break;

    /*
     *  LIF Group Destination  - same value for each encoding
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_MC_ID)-1, SOC_PB_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR, 22, exit);
    break;

    /*
     *  FEC Destination.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:
    switch(encode_type)
    {
    case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_FEC_PTR)-1, SOC_PB_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR, 24, exit);
      break;

    case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR, 26, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 28, exit);
    }

    break;

   /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  The id must be null.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, 0, SOC_PB_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR, 30, exit);
    break;

    /*
     *  Drop destination
     *  The id must be null.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, 0, SOC_PB_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR, 32, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 34, exit);
  }

  /*
   *	Verify the value of the additional info: EEI, OutLIF and Trap info
   */
  switch(info->type)
  {
    /*
     *  Unicast
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:

    switch(info->additional_info.outlif.type)
    {
    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.outlif.val, 0, SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR, 44, exit);
      break;

    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC:
    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP:
    case SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.outlif.val, (1<<SOC_PB_PP_ASD_NOF_BITS_OUTLIF_VALUE)-1, SOC_PB_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR, 46, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 48, exit);
    }
   break;

    /*
     *  Multicast or FEC
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
  case SOC_PB_PP_FRWRD_DECISION_TYPE_FEC:

    if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM)
    {
      switch(info->additional_info.eei.val.mpls_command.command)
      {
        case SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET:
        case SOC_PB_PP_MPLS_COMMAND_TYPE_SWAP:
          break;
        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR, 74, exit);
      }
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.label, (1<<SOC_PB_PP_ASD_NOF_BITS_MPLS_LABEL)-1, SOC_PB_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR, 76, exit);
    }
    else if(appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT || appl_type == SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED)
    {
      switch(info->additional_info.eei.type)
      {
      case SOC_PB_PP_EEI_TYPE_EMPTY:
        break;

      case SOC_PB_PP_EEI_TYPE_TRILL:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.trill_dest.dest_nick, (1<<SOC_PB_PP_ASD_NOF_BITS_TRILL_NICK)-1, SOC_PB_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR, 68, exit);
        break;

      case SOC_PB_PP_EEI_TYPE_MPLS:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.command, SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH, SOC_PB_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR, 75, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.label, (1<<SOC_PB_PP_ASD_NOF_BITS_MPLS_LABEL)-1, SOC_PB_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR, 76, exit);
        SOC_SAND_ERR_IF_OUT_OF_RANGE(info->additional_info.eei.val.mpls_command.push_profile, 1, (1<<SOC_PB_PP_ASD_NOF_BITS_MPLS_PUSH_PROFILE)-1, SOC_PB_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR, 78, exit);
        break;
        
      case SOC_PB_PP_EEI_TYPE_MIM:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.isid, SOC_PB_PP_ISID_ID_MAX, SOC_PB_PP_ISID_ID_OUT_OF_RANGE_ERR, 75, exit);
        break;
          
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR, 80, exit);
      }
    }
    
    break;

    /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  The id must be null.
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.trap_code, SOC_PB_PP_NOF_TRAP_CODES-1, SOC_PB_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR, 86, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.frwrd_action_strength, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_FWD)-1, SOC_PB_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR, 88, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.snoop_action_strength, (1<<SOC_PB_PP_ENCODE_NOF_BITS_EM_TRAP_SNOOP)-1, SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR, 90, exit);
  break;

    /*
     *  Drop destination
     */
  case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 102, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_DECISION_INFO_with_encode_type_verify()",0,0);
}

uint32
  SOC_PB_PP_ACTION_PROFILE_verify(
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_code, SOC_PB_PP_NOF_TRAP_CODES-1, SOC_PB_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_action_strength, SOC_PB_PP_GENERAL_FRWRD_ACTION_STRENGTH_MAX, SOC_PB_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->snoop_action_strength, SOC_PB_PP_GENERAL_SNOOP_ACTION_STRENGTH_MAX, SOC_PB_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 12, exit);

  /*SOC_SAND_MAGIC_NUM_VERIFY(info);*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_ACTION_PROFILE_verify()",0,0);
}

uint32
  SOC_PB_PP_TPID_PROFILE_verify(
    SOC_SAND_IN  SOC_PB_PP_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid1_index, SOC_PB_PP_GENERAL_TPID1_INDEX_MAX, SOC_PB_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid2_index, SOC_PB_PP_GENERAL_TPID2_INDEX_MAX, SOC_PB_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TPID_PROFILE_verify()",0,0);
}

uint32
  SOC_PB_PP_PEP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->port, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_PEP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_verify(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_GENERAL_TYPE_MAX, SOC_PB_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_IP_ROUTING_TABLE_ITER_verify()",0,0);
}

uint32
  SOC_PB_PP_IP_ROUTING_TABLE_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_RANGE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_IP_ROUTING_TABLE_ITER, &(info->start), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_to_scan, SOC_PB_PP_GENERAL_ENTRIES_TO_SCAN_MAX, SOC_PB_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_to_act, SOC_PB_PP_GENERAL_ENTRIES_TO_ACT_MAX, SOC_PB_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_IP_ROUTING_TABLE_RANGE_verify()",0,0);
}

uint32
  soc_pb_pp_SAND_PP_SYS_PORT_ID_verify(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_port_type, SOC_SAND_PP_SYS_PORT_TYPE_LAG, SOC_PB_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR, 11, exit);

  if (info->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_id, 255, SOC_PB_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR, 12, exit);
  }

  if (info->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_id, SOC_PB_PP_SYS_PORT_MAX, SOC_PB_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR, 20, exit);
  }
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_IP_ROUTING_TABLE_RANGE_verify()",0,0);
}

const char*
  SOC_PB_PP_DEST_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_DEST_ENCODE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_PP_DEST_ENCODE_TYPE_EM:
    str = "em";
  break;
  case SOC_PB_PP_DEST_ENCODE_TYPE_16_BITS:
    str = "16_bits";
  break;
  case SOC_PB_PP_DEST_ENCODE_TYPE_17_BITS:
    str = "17_bits";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

