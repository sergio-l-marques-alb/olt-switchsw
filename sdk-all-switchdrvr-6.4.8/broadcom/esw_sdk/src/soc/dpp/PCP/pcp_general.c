/* $Id: pcp_general.c,v 1.13 Broadcom SDK $
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

#include <soc/dpp/PCP/pcp_api_framework.h>

#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>

/* For trap support
#include <soc/dpp/PCP/pcp_api_trap_mgmt.h>
#include <soc/dpp/PCP/pcp_trap_mgmt.h>
*/

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_GENERAL_VAL_MAX                                  (SOC_SAND_U32_MAX)
#define PCP_GENERAL_COMMAND_VAL_MAX                          16
#define PCP_GENERAL_PUSH_PROFILE_MAX                         (7)
#define PCP_GENERAL_FRWRD_ACTION_STRENGTH_MAX                (SOC_SAND_U32_MAX)
#define PCP_GENERAL_SNOOP_ACTION_STRENGTH_MAX                (SOC_SAND_U32_MAX)
#define PCP_FRWRD_DECISION_INFO_TYPE_MAX                     (PCP_NOF_FRWRD_DECISION_TYPES-1)

#define PCP_ENCODE_NOF_BITS_EM_FLOW_ID                       (14)
#define PCP_ENCODE_NOF_BITS_EM_FEC_PTR                       (14)
#define PCP_ENCODE_NOF_BITS_EM_MC_ID                         (14)
#define PCP_ENCODE_NOF_BITS_EM_TRAP_CODE                     (8)
#define PCP_ENCODE_NOF_BITS_EM_TRAP_FWD                      (3)
#define PCP_ENCODE_NOF_BITS_EM_TRAP_SNOOP                    (2)
#define PCP_ENCODE_NOF_BITS_EM_LAG_ID                        (8)
#define PCP_ENCODE_NOF_BITS_EM_PHY_PORT                      (12)

#define PCP_ENCODE_NOF_BITS_16_FLOW_ID                       (15)

/*
 *	Prefix definition for each encoding
 */
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FLOW               (2)
#define PCP_ENCODE_DEST_PREFIX_VALUE_EM_FLOW                  (3)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FEC_PTR            (2)
#define PCP_ENCODE_DEST_PREFIX_VALUE_EM_FEC_PTR               (2)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_MC_ID              (2)
#define PCP_ENCODE_DEST_PREFIX_VALUE_EM_MC_ID                 (1)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_TRAP               (3)
#define PCP_ENCODE_DEST_PREFIX_VALUE_EM_TRAP                  (1)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_LAG_ID             (4)
#define PCP_ENCODE_DEST_PREFIX_VALUE_EM_LAG_ID                (1)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_PHY_PORT           (4)
#define PCP_ENCODE_DEST_PREFIX_VALUE_EM_PHY_PORT              (0)

#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_16_DROP               (16)
#define PCP_ENCODE_DEST_PREFIX_VALUE_16_DROP                  (0xFFFF)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_16_FLOW               (1)
#define PCP_ENCODE_DEST_PREFIX_VALUE_16_FLOW                  (1)

#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_DROP               (17)
#define PCP_ENCODE_DEST_PREFIX_VALUE_17_DROP                  (0x1FFFF)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_FEC_PTR            (3)
#define PCP_ENCODE_DEST_PREFIX_VALUE_17_FEC_PTR               (2)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_MC_ID              (3)
#define PCP_ENCODE_DEST_PREFIX_VALUE_17_MC_ID                 (1)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_LAG_ID             (5)
#define PCP_ENCODE_DEST_PREFIX_VALUE_17_LAG_ID                (1)
#define PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_PHY_PORT           (5)
#define PCP_ENCODE_DEST_PREFIX_VALUE_17_PHY_PORT              (0)

/*
 *	ASD constants
 */
/* ASD - ILM format */
#define PCP_ASD_ILM_BIT_POSITION_MPLS_LBL (0)
#define PCP_ASD_ILM_NOF_BITS_MPLS_LBL (20)
#define PCP_ASD_ILM_BIT_POSITION_MPLS_CMD (20)
#define PCP_ASD_ILM_NOF_BITS_MPLS_CMD (4)
#define PCP_ASD_ILM_BIT_POSITION_P2P (24)
#define PCP_ASD_ILM_NOF_BITS_P2P (1)
#define PCP_ASD_ILM_VALUE_P2P (0)

/* ASD - MAC format */
#define PCP_ASD_NOF_BITS_MPLS_LABEL                          (20)
#define PCP_ASD_NOF_BITS_MPLS_PUSH_PROFILE                   (3)
#define PCP_ASD_NOF_BITS_OUTLIF_VALUE                        (14)
#define PCP_ASD_BIT_POSITION_SA_DROP                         (23)
#define PCP_ASD_BIT_POSITION_IDENTIFIER                      (20)
#define PCP_ASD_BIT_POSITION_TRILL_MC                        (16)
#define PCP_ASD_TRILL_VS_OUTLIF_VALUE_NONE                   (0)
#define PCP_ASD_TRILL_VS_OUTLIF_VALUE_TRILL                  (1)
#define PCP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF                 (3)
#define PCP_ASD_NOF_BITS_TRILL_NICK                          (16)
#define PCP_ASD_OUTLIF_TYPE_EEP                              (0)
#define PCP_ASD_OUTLIF_TYPE_OUT_VSI                          (1)
#define PCP_ASD_OUTLIF_TYPE_AC                               (2)
#define PCP_ASD_NOF_BITS_OUTLIF_TYPE                         (2)
#define PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE                (2)
#define PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION            (18)

#define PCP_DEST_PREFIX_4_MSB_NOF_BITS                       (4)
#define PCP_DEST_ENCODE_5TH_BIT                              (5)

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
  PCP_DEST_ENCODED_PREFIX_TYPE_0000 = 0,
  PCP_DEST_ENCODED_PREFIX_TYPE_0001 = 1,
  PCP_DEST_ENCODED_PREFIX_TYPE_0010 = 2,
  PCP_DEST_ENCODED_PREFIX_TYPE_0011 = 3,
  PCP_DEST_ENCODED_PREFIX_TYPE_0100 = 4,
  PCP_DEST_ENCODED_PREFIX_TYPE_0101 = 5,
  PCP_DEST_ENCODED_PREFIX_TYPE_0110 = 6,
  PCP_DEST_ENCODED_PREFIX_TYPE_0111 = 7,
  PCP_DEST_ENCODED_PREFIX_TYPE_1000 = 8,
  PCP_DEST_ENCODED_PREFIX_TYPE_1001 = 9,
  PCP_DEST_ENCODED_PREFIX_TYPE_1010 = 10,
  PCP_DEST_ENCODED_PREFIX_TYPE_1011 = 11,
  PCP_DEST_ENCODED_PREFIX_TYPE_1100 = 12,
  PCP_DEST_ENCODED_PREFIX_TYPE_1101 = 13,
  PCP_DEST_ENCODED_PREFIX_TYPE_1110 = 14,
  PCP_DEST_ENCODED_PREFIX_TYPE_1111 = 15,
  /*
  *  Number of key types
  */
  PCP_DEST_ENCODED_NOF_PREFIXES = 16
}PCP_DEST_ENCODED_PREFIX_TYPE;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Pcp_procedure_desc_element_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */

  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FWD_DECISION_DEST_TYPE_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FWD_DECISION_IN_BUFFER_PARSE),
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Pcp_error_desc_element_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_VSI_ID_OUT_OF_RANGE_ERR,
      "PCP_VSI_ID_OUT_OF_RANGE_ERR",
      "The parameter of type 'PCP_VSI_ID' is out of range. \n\r "
      "The range is: 0 - 16*1024-1.\n\r ",
      SOC_SAND_SVR_ERR,
      FALSE
  },
  {
    PCP_VRF_ID_OUT_OF_RANGE_ERR,
    "PCP_VRF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_VRF_ID' is out of range or Routing disabled. \n\r "
    "The range is: 1 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FEC_ID_OUT_OF_RANGE_ERR,
    "PCP_FEC_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_FEC_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  {
    PCP_GENERAL_TYPE_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_OUTLIF_ENCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_PORT_OUT_OF_RANGE_ERR,
    "PCP_PORT_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_PORT' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_RIF_ID_OUT_OF_RANGE_ERR,
    "PCP_RIF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_RIF_ID' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_VAL_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_COMMAND_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'command' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_MPLS_COMMAND_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'push_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR",
    "The parameter 'dest_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'encode_type' of the destination is out of range. \n\r "
    "The range is: 0 - PCP_NOF_DEST_ENCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR",
    "The parameter EEI.type of the destination is out of range. \n\r "
    "Allowed values are empty, trill amd mpls.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR",
    "The parameter OutLIF.type of the destination is out of range. \n\r "
    "Allowed values are none, eep, out-vsi, ac, trap.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR",
    "The physical system port of the destination is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR",
    "The lag id of the destination is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR",
    "The multicast id of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR",
    "The FEC pointer of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR",
    "The id of the destination is out of range. \n\r "
    "When the action is 'trap', the id must be 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR",
    "The id of the destination is out of range. \n\r "
    "When the action is 'drop', the id must be 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_FEC_PTR_INVALID_ERR,
    "PCP_FRWRD_DEST_FEC_PTR_INVALID_ERR",
    "The FEC pointer of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for this destination encoding.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_EEI_TYPE_INVALID_ERR,
    "PCP_FRWRD_DEST_EEI_TYPE_INVALID_ERR",
    "The EEI type of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Multicast or FEC types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_CODE_INVALID_ERR,
      "PCP_FRWRD_DEST_TRAP_CODE_INVALID_ERR",
    "The trap code of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The Trap code of the destination is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_FWD_INVALID_ERR,
    "PCP_FRWRD_DEST_TRAP_FWD_INVALID_ERR",
    "The forward strength of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR",
    "The Trap forward strength of the destination is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR,
    "PCP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR",
    "The snoop strength of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR",
    "The Trap snoop strength of the destination is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_EEI_VAL_INVALID_ERR,
    "PCP_FRWRD_DEST_EEI_VAL_INVALID_ERR",
    "The EEI value of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non trill or mpls types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR,
    "PCP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR",
    "The OutLIF value of the destination is invalid (non zero). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR",
    "The nick value of the destination is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR",
    "The OutLIF value of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The push profile value of the destination is out of range. \n\r "
    "The range is: 1 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR,
      "PCP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR",
    "The MPLS label value of the destination is out of range. \n\r "
    "The range is: 0 - 1048575 (1M).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR,
      "PCP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR",
    "The MPLS command value of the destination is out of range. \n\r "
    "The range is: 0 - 0 (only push is accepted).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR,
    "PCP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR",
    "The OutLIF type of the destination is invalid. \n\r "
    "This parameter is invalid for non Unicast types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR,
      "PCP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR",
    "The flow id of the destination is out of range. \n\r "
    "The range is (according to the encoding): 0 - 16383 (or 32767).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_DROP_ILLEGAL_ERR,
    "PCP_FRWRD_DEST_DROP_ILLEGAL_ERR",
    "the destination cannot be drop in this place use trap. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
    "PCP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR",
    "The retrieved key has an unknown key prefix. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR",
    "'eei_type' is out of range. \n\r "
    "The range is: 0 to PCP_NOF_EEI_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to PCP_NOF_FRWRD_DECISION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "PCP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to PCP_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to PCP_FRWRD_DECISION_ASD_FORMAT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FEATURE_NOT_SUPPORTED_ERR,
    "PCP_FEATURE_NOT_SUPPORTED_ERR",
    "Feature is not supported in PCP. \n\r ",
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

STATIC uint32
  pcp_fwd_decision_dest_prefix_find(
    SOC_SAND_IN  PCP_DEST_ENCODE_TYPE    encode_type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_TYPE fwd_decision_type,
    SOC_SAND_OUT uint32                  *prefix_nof_bits,
    SOC_SAND_OUT uint32                  *prefix_value
    )
{
  uint32
    nof_bits,
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FWD_DECISION_DEST_PREFIX_FIND);

  SOC_SAND_CHECK_NULL_INPUT(prefix_nof_bits);
  SOC_SAND_CHECK_NULL_INPUT(prefix_value);

  switch(fwd_decision_type)
  {
    /*
     *  Single port
     */
  case PCP_FRWRD_DECISION_TYPE_UC_PORT:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_PHY_PORT;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_EM_PHY_PORT;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_PHY_PORT;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_17_PHY_PORT;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

    /*
     *  Explicit flow.
     */
  case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FLOW;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_EM_FLOW;
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_16_FLOW;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_16_FLOW;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 11, exit);
    }
    break;

    /*
     *  LAG - same value for each encoding
     */
  case PCP_FRWRD_DECISION_TYPE_UC_LAG:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_LAG_ID;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_EM_LAG_ID;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_LAG_ID;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_17_LAG_ID;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 12, exit);
    }
    break;

    /*
     *  LIF Group Destination  - same value for each encoding
     */
  case PCP_FRWRD_DECISION_TYPE_MC:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_MC_ID;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_EM_MC_ID;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_MC_ID;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_17_MC_ID;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 13, exit);
    }
    break;

    /*
     *  FEC Destination.
     */
  case PCP_FRWRD_DECISION_TYPE_FEC:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_FEC_PTR;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_EM_FEC_PTR;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_FEC_PTR;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_17_FEC_PTR;
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 14, exit);
    }
    break;

   /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  The id must be null.
     */
  case PCP_FRWRD_DECISION_TYPE_TRAP:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_EM_TRAP;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_EM_TRAP;
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
    case PCP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 15, exit);
    }
    break;

    /*
     *  Drop destination
     *  The id must be null.
     */
  case PCP_FRWRD_DECISION_TYPE_DROP:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_16_BITS:
    case PCP_DEST_ENCODE_TYPE_EM:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_16_DROP;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_16_DROP;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      nof_bits = PCP_ENCODE_DEST_PREFIX_NOF_BITS_17_DROP;
      value = PCP_ENCODE_DEST_PREFIX_VALUE_17_DROP;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 16, exit);
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 17, exit);
  }

  *prefix_value = value;
  *prefix_nof_bits = nof_bits;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_fwd_decision_dest_prefix_find()", 0, 0);
}

STATIC uint32
  pcp_fwd_decision_encode_type_get(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_OUT  PCP_DEST_ENCODE_TYPE     *encode_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(encode_type);
 
  switch(appl_type)
  {
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_FEC:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_16:
    *encode_type = PCP_DEST_ENCODE_TYPE_16_BITS;
    break;
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_IP:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_TRILL:
    *encode_type = PCP_DEST_ENCODE_TYPE_EM;
    break;

  case PCP_FRWRD_DECISION_APPLICATION_TYPE_TRAP:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_PMF:
  case PCP_FRWRD_DECISION_APPLICATION_TYPE_17:
    *encode_type = PCP_DEST_ENCODE_TYPE_17_BITS;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_fwd_decision_encode_type_get()", 0, 0);
}

STATIC uint32
  pcp_fwd_decision_dest_type_find(
      SOC_SAND_IN  PCP_DEST_ENCODE_TYPE     encode_type,
      SOC_SAND_IN  uint32                   dest_buffer,
      SOC_SAND_OUT PCP_FRWRD_DECISION_TYPE  *decision_type
  )
{
  uint8
    fifth_bit_17_encoding;
  uint32
    buffer_size,
    buffer_4_msb = 0,
    res = SOC_SAND_OK;
  PCP_FRWRD_DECISION_TYPE
    decision_type_lcl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FWD_DECISION_DEST_TYPE_FIND);

  SOC_SAND_CHECK_NULL_INPUT(decision_type);

  /*
   *	Decide according to the 4 msb, and then according to the encoding type
   */
  switch(encode_type)
  {
  case PCP_DEST_ENCODE_TYPE_EM:
    buffer_size = PCP_DEST_ENCODE_TOTAL_IN_BITS_EM;
    break;

  case PCP_DEST_ENCODE_TYPE_16_BITS:
    buffer_size = PCP_DEST_ENCODE_TOTAL_IN_BITS_16;
    break;

  case PCP_DEST_ENCODE_TYPE_17_BITS:
    buffer_size = PCP_DEST_ENCODE_TOTAL_IN_BITS_17;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_sand_bitstream_get_any_field(
          &(dest_buffer),
          buffer_size - PCP_DEST_PREFIX_4_MSB_NOF_BITS,
          PCP_DEST_PREFIX_4_MSB_NOF_BITS,
          &(buffer_4_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(buffer_4_msb)
  {
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1100:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1101:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1110:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1111:
    decision_type_lcl = PCP_FRWRD_DECISION_TYPE_UC_FLOW;
    break;

  case  PCP_DEST_ENCODED_PREFIX_TYPE_1000:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1001:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1010:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_1011:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_FEC;
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_UC_FLOW;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;
  case  PCP_DEST_ENCODED_PREFIX_TYPE_0110:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_0111:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_MC;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  case  PCP_DEST_ENCODED_PREFIX_TYPE_0100:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_0101:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_MC;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_FEC;
      break;

    /* must have default. Otherwise, compilation error */
    /* coverity[dead_error_begin : FALSE] */
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  case  PCP_DEST_ENCODED_PREFIX_TYPE_0010:
  case  PCP_DEST_ENCODED_PREFIX_TYPE_0011:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_TRAP;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_MC;
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  case  PCP_DEST_ENCODED_PREFIX_TYPE_0001:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_UC_LAG;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS:
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;
  case  PCP_DEST_ENCODED_PREFIX_TYPE_0000:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_16_BITS:
      decision_type_lcl = PCP_FRWRD_DECISION_TYPE_UC_PORT;
      break;

    case PCP_DEST_ENCODE_TYPE_17_BITS: /* Depends on the 5th bit*/
      fifth_bit_17_encoding = SOC_SAND_GET_BIT(dest_buffer, buffer_size - PCP_DEST_ENCODE_5TH_BIT);
        if (fifth_bit_17_encoding == 1)
        {
          decision_type_lcl = PCP_FRWRD_DECISION_TYPE_UC_LAG;
        }
        else
        {
          decision_type_lcl = PCP_FRWRD_DECISION_TYPE_UC_PORT;
        }
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }
    break;

  
  default:
    SOC_SAND_SET_ERROR_CODE(PCP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 20, exit);
  }

  *decision_type = decision_type_lcl;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_fwd_decision_dest_type_find()", 0, 0);
}
/*********************************************************************
*     Convert a forwarding decision to a buffer according to the
*     encoding type and the SA drop bit (for the ASD encoding)
*********************************************************************/

uint32
  pcp_fwd_decision_in_buffer_build(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO *fwd_decision,
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
  PCP_DEST_ENCODE_TYPE
    encode_type;
  PCP_TRAP_CODE_INTERNAL
    trap_code_internal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0/*PCP_FWD_DECISION_IN_BUFFER_BUILD*/);

  SOC_SAND_CHECK_NULL_INPUT(fwd_decision);
  SOC_SAND_CHECK_NULL_INPUT(dest_buffer);
  SOC_SAND_CHECK_NULL_INPUT(asd_buffer);

  /*
   *	Verify the inputs and their combination
   */
  PCP_FRWRD_DECISION_INFO_verify(fwd_decision);

  res = pcp_fwd_decision_encode_type_get(
          appl_type,
          &encode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = PCP_FRWRD_DECISION_INFO_with_encode_type_verify(
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
  res = pcp_fwd_decision_dest_prefix_find(
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
  case PCP_DEST_ENCODE_TYPE_EM:
    param_id_length = PCP_DEST_ENCODE_TOTAL_IN_BITS_EM - prefix_nof_bits;
    break;

  case PCP_DEST_ENCODE_TYPE_16_BITS:
    param_id_length = PCP_DEST_ENCODE_TOTAL_IN_BITS_16 - prefix_nof_bits;
    break;

  case PCP_DEST_ENCODE_TYPE_17_BITS:
    param_id_length = PCP_DEST_ENCODE_TOTAL_IN_BITS_17 - prefix_nof_bits;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 30, exit);
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
  case PCP_FRWRD_DECISION_TYPE_UC_PORT:
  case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
  case PCP_FRWRD_DECISION_TYPE_UC_LAG:
  case PCP_FRWRD_DECISION_TYPE_MC:
  case PCP_FRWRD_DECISION_TYPE_FEC:
  case PCP_FRWRD_DECISION_TYPE_DROP:
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
  case PCP_FRWRD_DECISION_TYPE_TRAP:
    res = pcp_trap_mgmt_trap_code_to_internal(
          fwd_decision->additional_info.trap_info.action_profile.trap_code,
          &trap_code_internal
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    temp = (uint32)trap_code_internal;

    res = soc_sand_bitstream_set_any_field(
              &(temp),
              0,
              PCP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    temp = fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength;
    res = soc_sand_bitstream_set_any_field(
              &(temp),
              PCP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              PCP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

    res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength),
              PCP_ENCODE_NOF_BITS_EM_TRAP_CODE + PCP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              PCP_ENCODE_NOF_BITS_EM_TRAP_SNOOP,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }



  if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM)
  {
    if (fwd_decision->additional_info.outlif.type == PCP_OUTLIF_ENCODE_TYPE_EEP)
    {
        outlif_or_trill_type = PCP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF;
        switch(fwd_decision->additional_info.outlif.type)
        {
        case PCP_OUTLIF_ENCODE_TYPE_AC:
          asd_outlif_type = PCP_ASD_OUTLIF_TYPE_AC;
          break;

        case PCP_OUTLIF_ENCODE_TYPE_VSI:
          asd_outlif_type = PCP_ASD_OUTLIF_TYPE_OUT_VSI;
          break;

        case PCP_OUTLIF_ENCODE_TYPE_EEP:
          asd_outlif_type = PCP_ASD_OUTLIF_TYPE_EEP;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 20, exit);
        }

        res = soc_sand_bitstream_set_any_field(
                &(outlif_or_trill_type),
                PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION,
                PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


        res = soc_sand_bitstream_set_any_field(
                &(asd_outlif_type),
                PCP_ASD_NOF_BITS_OUTLIF_VALUE,
                PCP_ASD_NOF_BITS_OUTLIF_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        res = soc_sand_bitstream_set_any_field(
                &(fwd_decision->additional_info.outlif.val),
                0,
                PCP_ASD_NOF_BITS_OUTLIF_VALUE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);

    }
    else
    {
      res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.eei.val.mpls_command.label),
              PCP_ASD_ILM_BIT_POSITION_MPLS_LBL,
              PCP_ASD_ILM_NOF_BITS_MPLS_LBL,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

      temp = fwd_decision->additional_info.eei.val.mpls_command.command;

      if (fwd_decision->additional_info.eei.val.mpls_command.command == PCP_MPLS_COMMAND_TYPE_PUSH)
      {
        temp = fwd_decision->additional_info.eei.val.mpls_command.push_profile;
      }
      
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              PCP_ASD_ILM_BIT_POSITION_MPLS_CMD,
              PCP_ASD_ILM_NOF_BITS_MPLS_CMD,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

      temp = PCP_ASD_ILM_VALUE_P2P;
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              PCP_ASD_ILM_BIT_POSITION_P2P,
              PCP_ASD_ILM_NOF_BITS_P2P,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);
    }
  }
  else if (
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT) ||
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P) ||
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP) ||
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED)
          )
  {
    /*
     *	Build the ASD buffer
     */
    res = soc_sand_bitstream_set_any_field(
            &(is_sa_drop_lcl),
            PCP_ASD_BIT_POSITION_SA_DROP,
            1,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if (fwd_decision->additional_info.eei.type == PCP_EEI_TYPE_MPLS)
    {
      temp = fwd_decision->additional_info.eei.val.mpls_command.command;

      if (fwd_decision->additional_info.eei.val.mpls_command.command == PCP_MPLS_COMMAND_TYPE_PUSH)
      {
        temp = fwd_decision->additional_info.eei.val.mpls_command.push_profile;
      }

      res = soc_sand_bitstream_set_any_field(
              &(temp),
              PCP_ASD_BIT_POSITION_IDENTIFIER,
              PCP_ASD_NOF_BITS_MPLS_PUSH_PROFILE,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

      res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.eei.val.mpls_command.label),
              0,
              PCP_ASD_NOF_BITS_MPLS_LABEL,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    }
    else
    {
      if (fwd_decision->additional_info.eei.type == PCP_EEI_TYPE_TRILL)
      {
        outlif_or_trill_type = PCP_ASD_TRILL_VS_OUTLIF_VALUE_TRILL;
        
        is_multicast = fwd_decision->additional_info.eei.val.trill_dest.is_multicast;
        res = soc_sand_bitstream_set_any_field(
                &(is_multicast),
                PCP_ASD_BIT_POSITION_TRILL_MC,
                1,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

         res = soc_sand_bitstream_set_any_field(
                &(fwd_decision->additional_info.eei.val.trill_dest.dest_nick),
                0,
                PCP_ASD_NOF_BITS_TRILL_NICK,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 81, exit);
      }
      else if (
                (fwd_decision->additional_info.outlif.type <= PCP_OUTLIF_ENCODE_TYPE_VSI)
                && (fwd_decision->additional_info.outlif.type >= PCP_OUTLIF_ENCODE_TYPE_AC)
               )
      {
        is_outlif = 1;
        outlif_or_trill_type = PCP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF;
   
        switch(fwd_decision->additional_info.outlif.type)
        {
        case PCP_OUTLIF_ENCODE_TYPE_AC:
          asd_outlif_type = PCP_ASD_OUTLIF_TYPE_AC;
          break;

        case PCP_OUTLIF_ENCODE_TYPE_VSI:
          asd_outlif_type = PCP_ASD_OUTLIF_TYPE_OUT_VSI;
          break;

        case PCP_OUTLIF_ENCODE_TYPE_EEP:
          asd_outlif_type = PCP_ASD_OUTLIF_TYPE_EEP;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 20, exit);
        }

        res = soc_sand_bitstream_set_any_field(
                &(asd_outlif_type),
                PCP_ASD_NOF_BITS_OUTLIF_VALUE,
                PCP_ASD_NOF_BITS_OUTLIF_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        res = soc_sand_bitstream_set_any_field(
                &(fwd_decision->additional_info.outlif.val),
                0,
                PCP_ASD_NOF_BITS_OUTLIF_VALUE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
      }

      /*
       *	Use of ASD and not outlif
       */
      if(
          (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT)
          ||(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP)
          ||(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED)
        )
      {
        res = soc_sand_bitstream_set_any_field(
                &(outlif_or_trill_type),
                PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION,
                PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE,
                &(asd_buffer_lcl)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      }
    
      /*
       *	Use of ASD and not outlif
       */
      if((appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED))
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

  else if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_FEC)
  {
    switch(fwd_decision->additional_info.outlif.type)
    {
    case PCP_OUTLIF_ENCODE_TYPE_AC:
      asd_outlif_type = PCP_ASD_OUTLIF_TYPE_AC;
      break;

    case PCP_OUTLIF_ENCODE_TYPE_VSI:
      asd_outlif_type = PCP_ASD_OUTLIF_TYPE_OUT_VSI;
      break;

    case PCP_OUTLIF_ENCODE_TYPE_EEP:
    default:
      asd_outlif_type = PCP_ASD_OUTLIF_TYPE_EEP;
      break;
    }

    res = soc_sand_bitstream_set_any_field(
            &(asd_outlif_type),
            PCP_ASD_NOF_BITS_OUTLIF_VALUE,
            PCP_ASD_NOF_BITS_OUTLIF_TYPE,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = soc_sand_bitstream_set_any_field(
            &(fwd_decision->additional_info.outlif.val),
            0,
            PCP_ASD_NOF_BITS_OUTLIF_VALUE,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
  }
  *dest_buffer = dest_buffer_lcl;
  *asd_buffer = asd_buffer_lcl;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_fwd_decision_in_buffer_build()", 0, 0);
}

/*********************************************************************
*     Parse an ASD and a destination buffer to get the forward
*     decision and the SA drop (from the ASD msb)
*********************************************************************/
uint32
  pcp_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  uint32                  dest_buffer,
    SOC_SAND_IN  uint32                  asd_buffer,
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT uint8                 *is_sa_drop
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_FRWRD_DECISION_TYPE
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
  PCP_DEST_ENCODE_TYPE
    encode_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FWD_DECISION_IN_BUFFER_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(fwd_decision);
  SOC_SAND_CHECK_NULL_INPUT(is_sa_drop);

  PCP_FRWRD_DECISION_INFO_clear(fwd_decision);

  res = pcp_fwd_decision_encode_type_get(
          appl_type,
          &encode_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  /*
   *	Parse the destination buffer, first find the type
   */
  fwd_decision->dest_id = 0;
  res = pcp_fwd_decision_dest_type_find(
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
  res = pcp_fwd_decision_dest_prefix_find(
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
  case PCP_DEST_ENCODE_TYPE_EM:
    param_id_length = PCP_DEST_ENCODE_TOTAL_IN_BITS_EM - prefix_nof_bits;
    break;

  case PCP_DEST_ENCODE_TYPE_16_BITS:
    param_id_length = PCP_DEST_ENCODE_TOTAL_IN_BITS_16 - prefix_nof_bits;
    break;

  case PCP_DEST_ENCODE_TYPE_17_BITS:
    param_id_length = PCP_DEST_ENCODE_TOTAL_IN_BITS_17 - prefix_nof_bits;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }
  /*
   *	Build the destination buffer according to the destination type
   *  and then the encoding type
   */
  switch(decision_type)
  {
  case PCP_FRWRD_DECISION_TYPE_UC_PORT:
  case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
  case PCP_FRWRD_DECISION_TYPE_MC:
  case PCP_FRWRD_DECISION_TYPE_FEC:
  case PCP_FRWRD_DECISION_TYPE_DROP:
     res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              0,
              param_id_length,
              &(fwd_decision->dest_id)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;
  case PCP_FRWRD_DECISION_TYPE_UC_LAG:
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
  case PCP_FRWRD_DECISION_TYPE_TRAP:
     res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              0,
              PCP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              &temp
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = pcp_trap_cpu_trap_code_from_internal_unsafe(0,temp,&fwd_decision->additional_info.trap_info.action_profile.trap_code);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              PCP_ENCODE_NOF_BITS_EM_TRAP_CODE,
              PCP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              &(fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

    res = soc_sand_bitstream_get_any_field(
              &(dest_buffer_lcl),
              PCP_ENCODE_NOF_BITS_EM_TRAP_CODE + PCP_ENCODE_NOF_BITS_EM_TRAP_FWD,
              PCP_ENCODE_NOF_BITS_EM_TRAP_SNOOP,
              &(fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 70, exit);
  }

  if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM)
  {
    uint32
      command = 0;

    fwd_decision->additional_info.eei.type = PCP_EEI_TYPE_MPLS;
    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            PCP_ASD_ILM_BIT_POSITION_MPLS_LBL,
            PCP_ASD_ILM_NOF_BITS_MPLS_LBL,
            &(fwd_decision->additional_info.eei.val.mpls_command.label)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            PCP_ASD_ILM_BIT_POSITION_MPLS_CMD,
            PCP_ASD_ILM_NOF_BITS_MPLS_CMD,
            &(command)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);
    fwd_decision->additional_info.eei.val.mpls_command.command = command;

    if (command <= 7)
    {
      fwd_decision->additional_info.eei.val.mpls_command.command = PCP_MPLS_COMMAND_TYPE_PUSH;
      fwd_decision->additional_info.eei.val.mpls_command.push_profile = command;
    }

    
  }
  else if (
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT) ||
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P) ||
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED) ||
           (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP)
          )
  {
    /*
     *	Get the ASD buffer
     */
    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            PCP_ASD_BIT_POSITION_SA_DROP,
            1,
            &is_sa_drop_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    *is_sa_drop = SOC_SAND_NUM2BOOL(is_sa_drop_lcl);

    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            PCP_ASD_BIT_POSITION_IDENTIFIER,
            PCP_ASD_NOF_BITS_MPLS_PUSH_PROFILE,
            &(push_profile)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (push_profile != 0)
    {
      fwd_decision->additional_info.eei.type = PCP_EEI_TYPE_MPLS;
      fwd_decision->additional_info.eei.val.mpls_command.push_profile = push_profile;
      fwd_decision->additional_info.eei.val.mpls_command.command = PCP_MPLS_COMMAND_TYPE_PUSH;

      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer_lcl),
              0,
              PCP_ASD_NOF_BITS_MPLS_LABEL,
              &(fwd_decision->additional_info.eei.val.mpls_command.label)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }
    else
    {
       res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_POSITION,
                PCP_ASD_NOF_BITS_OUTLIF_OR_TRILL_TYPE,
                &(outlif_or_trill_type)
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      /*
       *	Override in case of P2P (not written in its ASD-Outlif)
       */
      if (appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P)
      {
        outlif_or_trill_type = PCP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF;
      }

      if (outlif_or_trill_type == PCP_ASD_TRILL_VS_OUTLIF_VALUE_TRILL)
      {
        fwd_decision->additional_info.eei.type = PCP_EEI_TYPE_TRILL;
              
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                PCP_ASD_BIT_POSITION_TRILL_MC,
                1,
                &(is_multicast)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
        fwd_decision->additional_info.eei.val.trill_dest.is_multicast = SOC_SAND_NUM2BOOL(is_multicast);

         res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                0,
                PCP_ASD_NOF_BITS_TRILL_NICK,
                &(fwd_decision->additional_info.eei.val.trill_dest.dest_nick)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
      }
      else if (outlif_or_trill_type == PCP_ASD_TRILL_VS_OUTLIF_VALUE_OUTLIF)
      {
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                PCP_ASD_NOF_BITS_OUTLIF_VALUE,
                PCP_ASD_NOF_BITS_OUTLIF_TYPE,
                &(asd_outlif_type)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
        switch(asd_outlif_type)
        {
        case PCP_ASD_OUTLIF_TYPE_AC:
          fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_AC;
          break;

        case PCP_ASD_OUTLIF_TYPE_OUT_VSI:
          fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_VSI;
          break;

        case PCP_ASD_OUTLIF_TYPE_EEP:
          fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_EEP;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 150, exit);
        }

        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                0,
                PCP_ASD_NOF_BITS_OUTLIF_VALUE,
                &(fwd_decision->additional_info.outlif.val)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
      }
      else if(outlif_or_trill_type == PCP_ASD_TRILL_VS_OUTLIF_VALUE_NONE)
      {
        switch(decision_type)
        {
        case PCP_FRWRD_DECISION_TYPE_UC_PORT:
        case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
        case PCP_FRWRD_DECISION_TYPE_UC_LAG:
          fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_NONE;
          fwd_decision->additional_info.outlif.val = 0;
          break;

        case PCP_FRWRD_DECISION_TYPE_MC:
        case PCP_FRWRD_DECISION_TYPE_FEC:
          fwd_decision->additional_info.eei.type = PCP_EEI_TYPE_EMPTY;
          break;

        case PCP_FRWRD_DECISION_TYPE_DROP:
        case PCP_FRWRD_DECISION_TYPE_TRAP:
          break;

        /* must have default. Otherwise, compilation error */
        /* coverity[dead_error_begin : FALSE] */
        default:
          SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 70, exit);
        }
      }
    }
  }
  else if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_TRAP)
  {
    if(fwd_decision->type == PCP_FRWRD_DECISION_TYPE_MC || fwd_decision->type == PCP_FRWRD_DECISION_TYPE_FEC)
    {
      fwd_decision->additional_info.eei.type = PCP_EEI_TYPE_EMPTY;
    }
    else
    {
      fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_NONE;
    }
  }
  else if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_FEC)
  {
    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            PCP_ASD_NOF_BITS_OUTLIF_VALUE,
            PCP_ASD_NOF_BITS_OUTLIF_TYPE,
            &(asd_outlif_type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    switch(asd_outlif_type)
    {
    case PCP_ASD_OUTLIF_TYPE_AC:
      fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_AC;
      break;

    case PCP_ASD_OUTLIF_TYPE_OUT_VSI:
      fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_VSI;
      break;

    case PCP_ASD_OUTLIF_TYPE_EEP:
      fwd_decision->additional_info.outlif.type = PCP_OUTLIF_ENCODE_TYPE_EEP;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 150, exit);
    }

    res = soc_sand_bitstream_get_any_field(
            &(asd_buffer_lcl),
            0,
            PCP_ASD_NOF_BITS_OUTLIF_VALUE,
            &(fwd_decision->additional_info.outlif.val)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }
  /*
   *	Verify the parsing has a legal output
   
  res = PCP_FRWRD_DECISION_INFO_with_encode_type_verify(
          appl_type,
          fwd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_fwd_decision_in_buffer_parse()", 0, 0);
}
uint32
  PCP_TRAP_INFO_verify(
    SOC_SAND_IN  PCP_TRAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_ACTION_PROFILE, &(info->action_profile), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_TRAP_INFO_verify()",0,0);
}

uint32
  PCP_OUTLIF_verify(
    SOC_SAND_IN  PCP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_NOF_OUTLIF_ENCODE_TYPES, PCP_GENERAL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->val, PCP_GENERAL_VAL_MAX, PCP_GENERAL_VAL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OUTLIF_verify()",0,0);
}

uint32
  PCP_MPLS_COMMAND_verify(
    SOC_SAND_IN  PCP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->command, PCP_GENERAL_COMMAND_VAL_MAX, PCP_GENERAL_COMMAND_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, PCP_GENERAL_PUSH_PROFILE_MAX, PCP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MPLS_COMMAND_verify()",0,0);
}

uint32
  PCP_EEI_verify(
    SOC_SAND_IN  PCP_EEI *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_NOF_EEI_TYPES, PCP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  if (info->type == PCP_EEI_TYPE_TRILL)
  {
    PCP_STRUCT_VERIFY(SOC_SAND_PP_TRILL_DEST, &(info->val.trill_dest), 20, exit);
  }
  else if (info->type == PCP_EEI_TYPE_MPLS)
  {
    PCP_STRUCT_VERIFY(PCP_MPLS_COMMAND, &(info->val.mpls_command), 21, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_EEI_verify()",0,0);
}

uint32
  PCP_FRWRD_DECISION_TYPE_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_DECISION_TYPE       type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case PCP_FRWRD_DECISION_TYPE_DROP:
    break;
  case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
  case PCP_FRWRD_DECISION_TYPE_UC_LAG:
  case PCP_FRWRD_DECISION_TYPE_UC_PORT:
    PCP_STRUCT_VERIFY(PCP_OUTLIF, &(info->outlif), 11, exit);
    break;

  case PCP_FRWRD_DECISION_TYPE_MC:
  case PCP_FRWRD_DECISION_TYPE_FEC:
    PCP_STRUCT_VERIFY(PCP_EEI, &(info->eei), 10, exit);
    break;

  case PCP_FRWRD_DECISION_TYPE_TRAP:
    PCP_STRUCT_VERIFY(PCP_TRAP_INFO, &(info->trap_info), 12, exit);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(PCP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 18, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_DECISION_TYPE_INFO_verify()",0,0);
}

uint32
  PCP_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_FRWRD_DECISION_INFO_TYPE_MAX, PCP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  res = PCP_FRWRD_DECISION_TYPE_INFO_verify(
          info->type,
          &(info->additional_info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_DECISION_INFO_verify()",0,0);
}
uint32
  PCP_FRWRD_DECISION_INFO_with_encode_type_verify(
    SOC_SAND_IN  PCP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  PCP_FRWRD_DECISION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_DEST_ENCODE_TYPE    encode_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(appl_type, PCP_FRWRD_DECISION_APPLICATION_TYPES, PCP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR, 15, exit);

  res = pcp_fwd_decision_encode_type_get(
          appl_type,
          &encode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
 /*
  * EM encoding cannot hold drop
  */
  if (encode_type == PCP_DEST_ENCODE_TYPE_EM && info->type == PCP_FRWRD_DECISION_TYPE_DROP)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_DROP_ILLEGAL_ERR, 17, exit);
  }
  /*
   *	Verify the values of the destination id according to the encoding type
   */
  switch(info->type)
  {
    /*
     *  Single port - same value for each encoding
     */
  case PCP_FRWRD_DECISION_TYPE_UC_PORT:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<PCP_ENCODE_NOF_BITS_EM_PHY_PORT)-1, PCP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR, 12, exit);
    break;

    /*
     *  Explicit flow.
     */
  case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<PCP_ENCODE_NOF_BITS_EM_FLOW_ID)-1, PCP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR, 14, exit);
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
    case PCP_DEST_ENCODE_TYPE_17_BITS:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<PCP_ENCODE_NOF_BITS_16_FLOW_ID)-1, PCP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR, 16, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 18, exit);
    }
    break;

    /*
     *  LAG - same value for each encoding
     */
  case PCP_FRWRD_DECISION_TYPE_UC_LAG:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<PCP_ENCODE_NOF_BITS_EM_LAG_ID)-1, PCP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR, 20, exit);
    break;

    /*
     *  LIF Group Destination  - same value for each encoding
     */
  case PCP_FRWRD_DECISION_TYPE_MC:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<PCP_ENCODE_NOF_BITS_EM_MC_ID)-1, PCP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR, 22, exit);
    break;

    /*
     *  FEC Destination.
     */
  case PCP_FRWRD_DECISION_TYPE_FEC:
    switch(encode_type)
    {
    case PCP_DEST_ENCODE_TYPE_EM:
    case PCP_DEST_ENCODE_TYPE_17_BITS:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<PCP_ENCODE_NOF_BITS_EM_FEC_PTR)-1, PCP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR, 24, exit);
      break;

    case PCP_DEST_ENCODE_TYPE_16_BITS:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_FEC_PTR_INVALID_ERR, 26, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 28, exit);
    }

    break;

   /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  The id must be null.
     */
  case PCP_FRWRD_DECISION_TYPE_TRAP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, 0, PCP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR, 30, exit);
    break;

    /*
     *  Drop destination
     *  The id must be null.
     */
  case PCP_FRWRD_DECISION_TYPE_DROP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, 0, PCP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR, 32, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 34, exit);
  }

  /*
   *	Verify the value of the additional info: EEI, OutLIF and Trap info
   */
  switch(info->type)
  {
    /*
     *  Unicast
     */
  case PCP_FRWRD_DECISION_TYPE_UC_PORT:
  case PCP_FRWRD_DECISION_TYPE_UC_FLOW:
  case PCP_FRWRD_DECISION_TYPE_UC_LAG:

    switch(info->additional_info.outlif.type)
    {
    case PCP_OUTLIF_ENCODE_TYPE_NONE:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.outlif.val, 0, PCP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR, 44, exit);
      break;

    case PCP_OUTLIF_ENCODE_TYPE_AC:
    case PCP_OUTLIF_ENCODE_TYPE_EEP:
    case PCP_OUTLIF_ENCODE_TYPE_VSI:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.outlif.val, (1<<PCP_ASD_NOF_BITS_OUTLIF_VALUE)-1, PCP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR, 46, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 48, exit);
    }
   break;

    /*
     *  Multicast or FEC
     */
  case PCP_FRWRD_DECISION_TYPE_MC:
  case PCP_FRWRD_DECISION_TYPE_FEC:

    if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_ILM)
    {
      switch(info->additional_info.eei.val.mpls_command.command)
      {
        case PCP_MPLS_COMMAND_TYPE_PUSH:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM:
        case PCP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET:
        case PCP_MPLS_COMMAND_TYPE_SWAP:
          break;
        default:
          SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR, 74, exit);
      }
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.label, (1<<PCP_ASD_NOF_BITS_MPLS_LABEL)-1, PCP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR, 76, exit);
    }
    else if(appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_MACT || appl_type == PCP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED)
    {
      switch(info->additional_info.eei.type)
      {
      case PCP_EEI_TYPE_EMPTY:
        break;

      case PCP_EEI_TYPE_TRILL:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.trill_dest.dest_nick, (1<<PCP_ASD_NOF_BITS_TRILL_NICK)-1, PCP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR, 68, exit);
        break;

      case PCP_EEI_TYPE_MPLS:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.command, PCP_MPLS_COMMAND_TYPE_PUSH, PCP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR, 75, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.label, (1<<PCP_ASD_NOF_BITS_MPLS_LABEL)-1, PCP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR, 76, exit);
        SOC_SAND_ERR_IF_OUT_OF_RANGE(info->additional_info.eei.val.mpls_command.push_profile, 1, (1<<PCP_ASD_NOF_BITS_MPLS_PUSH_PROFILE)-1, PCP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR, 78, exit);
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR, 80, exit);
      }
    }
    
    break;

    /*
     *  Trap Destination. The format is snoop strength (2b); forward strength (3b); trap code (8b)
     *  The id must be null.
     */
  case PCP_FRWRD_DECISION_TYPE_TRAP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.trap_code, (1<<PCP_ENCODE_NOF_BITS_EM_TRAP_CODE)-1, PCP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR, 86, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.frwrd_action_strength, (1<<PCP_ENCODE_NOF_BITS_EM_TRAP_FWD)-1, PCP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR, 88, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.snoop_action_strength, (1<<PCP_ENCODE_NOF_BITS_EM_TRAP_SNOOP)-1, PCP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR, 90, exit);
  break;

    /*
     *  Drop destination
     */
  case PCP_FRWRD_DECISION_TYPE_DROP:
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 102, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_DECISION_INFO_with_encode_type_verify()",0,0);
}

uint32
  PCP_ACTION_PROFILE_verify(
    SOC_SAND_IN  PCP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_code, PCP_NOF_TRAP_CODES-1, PCP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_action_strength, PCP_GENERAL_FRWRD_ACTION_STRENGTH_MAX, PCP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->snoop_action_strength, PCP_GENERAL_SNOOP_ACTION_STRENGTH_MAX, PCP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 12, exit);

  /*SOC_SAND_MAGIC_NUM_VERIFY(info);*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_ACTION_PROFILE_verify()",0,0);
}
void
  PCP_TRAP_INFO_clear(
    SOC_SAND_OUT PCP_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_OUTLIF_clear(
    SOC_SAND_OUT PCP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_MPLS_COMMAND_clear(
    SOC_SAND_OUT PCP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_EEI_VAL_clear(
    SOC_SAND_OUT PCP_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_EEI_clear(
    SOC_SAND_OUT PCP_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_ACTION_PROFILE_clear(
    SOC_SAND_OUT PCP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_general_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_general_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_general;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_general_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_general;
}

/* ************************* pcp_trap_mgmt.c ************************* */

uint32
  pcp_trap_mgmt_trap_code_to_internal(
    SOC_SAND_IN  PCP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT PCP_TRAP_CODE_INTERNAL                  *trap_code_internal
  )
{
  int32
    diff;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0/*PCP_TRAP_MGMT_TRAP_CODE_TO_INTERNAL*/);
  
  switch (trap_code_ndx)
  {
  case  PCP_TRAP_CODE_PBP_SA_DROP_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0;
    break;
  case  PCP_TRAP_CODE_PBP_SA_DROP_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1;
    break;
  case  PCP_TRAP_CODE_PBP_SA_DROP_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2;
    break;
  case  PCP_TRAP_CODE_PBP_SA_DROP_3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3;
    break;
  case  PCP_TRAP_CODE_PBP_TE_TRANSPLANT:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT;
    break;
  case  PCP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL;
    break;
  case  PCP_TRAP_CODE_PBP_TRANSPLANT:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT;
    break;
  case  PCP_TRAP_CODE_PBP_LEARN_SNOOP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP;
    break;
  case  PCP_TRAP_CODE_SA_AUTHENTICATION_FAILED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED;
    break;
  case  PCP_TRAP_CODE_PORT_NOT_PERMITTED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED;
    break;
  case  PCP_TRAP_CODE_UNEXPECTED_VID:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID;
    break;
  case  PCP_TRAP_CODE_SA_MULTICAST:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST;
    break;
  case  PCP_TRAP_CODE_SA_EQUALS_DA:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA;
    break;
  case  PCP_TRAP_CODE_8021X:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_8021X;
    break;
  case  PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0;
    break;
  case  PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1;
    break;
  case  PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2;
    break;
  case  PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3;
    break;
  case  PCP_TRAP_CODE_MY_ARP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_MY_ARP;
    break;
  case  PCP_TRAP_CODE_ARP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ARP;
    break;
  case  PCP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY;
    break;
  case  PCP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG;
    break;
  case  PCP_TRAP_CODE_IGMP_UNDEFINED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  PCP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG;
    break;
  case  PCP_TRAP_CODE_ICMPV6_MLD_UNDEFINED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_DHCP_SERVER:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER;
    break;
  case  PCP_TRAP_CODE_DHCP_CLIENT:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT;
    break;
  case  PCP_TRAP_CODE_DHCPV6_SERVER:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER;
    break;
  case  PCP_TRAP_CODE_DHCPV6_CLIENT:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT;
    break;
  case  PCP_TRAP_CODE_PROG_TRAP_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0;
    break;
  case  PCP_TRAP_CODE_PROG_TRAP_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1;
    break;
  case  PCP_TRAP_CODE_PROG_TRAP_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2;
    break;
  case  PCP_TRAP_CODE_PROG_TRAP_3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3;
    break;
  case  PCP_TRAP_CODE_PORT_NOT_VLAN_MEMBER:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER;
    break;
  case  PCP_TRAP_CODE_HEADER_SIZE_ERR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR;
    break;
  case PCP_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS;
    break;
  case  PCP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL;
    break;
  case  PCP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID;
    break;
  case  PCP_TRAP_CODE_STP_STATE_BLOCK:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK;
    break;
  case  PCP_TRAP_CODE_STP_STATE_LEARN:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN;
    break;
  case  PCP_TRAP_CODE_IP_COMP_MC_INVALID_IP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP;
    break;
  case  PCP_TRAP_CODE_MY_MAC_AND_IP_DISABLE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE;
    break;
  case  PCP_TRAP_CODE_TRILL_VERSION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION;
    break;
  case  PCP_TRAP_CODE_TRILL_INVALID_TTL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL;
    break;
  case  PCP_TRAP_CODE_TRILL_CHBH:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH;
    break;
  case  PCP_TRAP_CODE_TRILL_NO_REVERSE_FEC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC;
    break;
  case  PCP_TRAP_CODE_TRILL_CITE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE;
    break;
  case  PCP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC;
    break;
  case  PCP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE;
    break;
  case  PCP_TRAP_CODE_MY_MAC_AND_ARP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP;
    break;
  case  PCP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3;
    break;
  case  PCP_TRAP_CODE_MPLS_LABEL_VALUE_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0;
    break;
  case  PCP_TRAP_CODE_MPLS_LABEL_VALUE_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1;
    break;
  case  PCP_TRAP_CODE_MPLS_LABEL_VALUE_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2;
    break;
  case  PCP_TRAP_CODE_MPLS_LABEL_VALUE_3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3;
    break;
  case  PCP_TRAP_CODE_MPLS_NO_RESOURCES:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES;
    break;
  case  PCP_TRAP_CODE_INVALID_LABEL_IN_RANGE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE;
    break;
  case  PCP_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM;
    break;
  case  PCP_TRAP_CODE_MPLS_LSP_BOS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS;
    break;
  case  PCP_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14;
    break;
  case  PCP_TRAP_CODE_MPLS_PWE_NO_BOS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS;
    break;
  case  PCP_TRAP_CODE_MPLS_VRF_NO_BOS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS;
    break;
  case  PCP_TRAP_CODE_MPLS_TERM_TTL_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0;
    break;
  case  PCP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP;
    break;
  case  PCP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_VERSION_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_TTL0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_HAS_OPTIONS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_TTL1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_DIP_ZERO:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO;
    break;
  case  PCP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    break;
  case  PCP_TRAP_CODE_IPV4_TERM_SIP_IS_MC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC;
    break;
  case  PCP_TRAP_CODE_CFM_ACCELERATED_INGRESS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS;
    break;
  case  PCP_TRAP_CODE_ILLEGEL_PFC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC;
    break;
  case  PCP_TRAP_CODE_SA_DROP_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP0;
    break;
  case  PCP_TRAP_CODE_SA_DROP_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP1;
    break;
  case  PCP_TRAP_CODE_SA_DROP_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP2;
    break;
  case  PCP_TRAP_CODE_SA_DROP_3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP3;
    break;
  case  PCP_TRAP_CODE_SA_NOT_FOUND_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0;
    break;
  case  PCP_TRAP_CODE_SA_NOT_FOUND_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1;
    break;
  case  PCP_TRAP_CODE_SA_NOT_FOUND_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2;
    break;
  case  PCP_TRAP_CODE_SA_NOT_FOUND_3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3;
    break;
  case  PCP_TRAP_CODE_ELK_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR;
    break;
  case  PCP_TRAP_CODE_DA_NOT_FOUND_0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND0;
    break;
  case  PCP_TRAP_CODE_DA_NOT_FOUND_1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND1;
    break;
  case  PCP_TRAP_CODE_DA_NOT_FOUND_2:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND2;
    break;
  case  PCP_TRAP_CODE_DA_NOT_FOUND_3:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND3;
    break;
  case  PCP_TRAP_CODE_P2P_MISCONFIGURATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION;
    break;
  case  PCP_TRAP_CODE_SAME_INTERFACE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE;
    break;
  case  PCP_TRAP_CODE_TRILL_UNKNOWN_UC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC;
    break;
  case  PCP_TRAP_CODE_TRILL_UNKNOWN_MC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC;
    break;
  case  PCP_TRAP_CODE_UC_LOOSE_RPF_FAIL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_DEFAULT_UCV6:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6;
    break;
  case  PCP_TRAP_CODE_DEFAULT_MCV6:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6;
    break;
  case  PCP_TRAP_CODE_MPLS_P2P_NO_BOS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS;
    break;
  case  PCP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP;
    break;
  case  PCP_TRAP_CODE_MPLS_CONTROL_WORD_DROP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP;
    break;
  case  PCP_TRAP_CODE_MPLS_UNKNOWN_LABEL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL;
    break;
    case  PCP_TRAP_CODE_MPLS_P2P_MPLSX4:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4;
    break;
  case  PCP_TRAP_CODE_ETH_L2CP_PEER:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER;
    break;
  case  PCP_TRAP_CODE_ETH_L2CP_DROP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP;
    break;
  case  PCP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    break;
  case  PCP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    break;
  case  PCP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    break;
  case  PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_IPV4_VERSION_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_CHECKSUM_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV4_TTL0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0;
    break;
  case  PCP_TRAP_CODE_IPV4_HAS_OPTIONS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS;
    break;
  case  PCP_TRAP_CODE_IPV4_TTL1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1;
    break;
  case  PCP_TRAP_CODE_IPV4_SIP_EQUAL_DIP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP;
    break;
  case  PCP_TRAP_CODE_IPV4_DIP_ZERO:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO;
    break;
  case  PCP_TRAP_CODE_IPV4_SIP_IS_MC:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC;
    break;
  case  PCP_TRAP_CODE_IPV6_VERSION_ERROR:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR;
    break;
  case  PCP_TRAP_CODE_IPV6_HOP_COUNT0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0;
    break;
  case  PCP_TRAP_CODE_IPV6_HOP_COUNT1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1;
    break;
  case  PCP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION;
    break;
  case  PCP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS;
    break;
  case  PCP_TRAP_CODE_IPV6_MULTICAST_SOURCE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE;
    break;
  case  PCP_TRAP_CODE_IPV6_NEXT_HEADER_NULL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL;
    break;
  case  PCP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE;
    break;
  case  PCP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION;
    break;
  case  PCP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION;
    break;
  case  PCP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE;
    break;
  case  PCP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE;
    break;
  case  PCP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION;
    break;
  case  PCP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION;
    break;
  case  PCP_TRAP_CODE_IPV6_MULTICAST_DESTINATION:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION;
    break;
  case  PCP_TRAP_CODE_MPLS_TTL0:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0;
    break;
  case  PCP_TRAP_CODE_MPLS_TTL1:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1;
    break;
  case  PCP_TRAP_CODE_TCP_SN_FLAGS_ZERO:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO;
    break;
  case  PCP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET;
    break;
  case  PCP_TRAP_CODE_TCP_SYN_FIN:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN;
    break;
  case  PCP_TRAP_CODE_TCP_EQUAL_PORTS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS;
    break;
  case  PCP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER;
    break;
  case  PCP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8;
    break;
  case  PCP_TRAP_CODE_UDP_EQUAL_PORTS:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS;
    break;
  case  PCP_TRAP_CODE_ICMP_DATA_GT_576:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576;
    break;
  case  PCP_TRAP_CODE_ICMP_FRAGMENTED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED;
    break;
  case  PCP_TRAP_CODE_GENERAL: 
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_PMF_GENERAL;
    break;
  case  PCP_TRAP_CODE_FACILITY_INVALID:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID;
    break;
  case  PCP_TRAP_CODE_FEC_ENTRY_ACCESSED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED;
    break;
  case  PCP_TRAP_CODE_UC_STRICT_RPF_FAIL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_MC_USE_SIP_ECMP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP;
    break;
  case  PCP_TRAP_CODE_ICMP_REDIRECT:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT;
    break;

  case  PCP_TRAP_CODE_USER_OAMP:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_OAMP;
    break;
  case  PCP_TRAP_CODE_USER_ETH_OAM_ACCELERATED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED:
    *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED;
    break;

  default:
    /* PCP_TRAP_CODE_RESERVED_MC_0-7 */
    if ((trap_code_ndx >= PCP_TRAP_CODE_RESERVED_MC_0) && (trap_code_ndx <= PCP_TRAP_CODE_RESERVED_MC_7))
    {
      diff = (trap_code_ndx - PCP_TRAP_CODE_RESERVED_MC_0);
      *trap_code_internal = PCP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX + diff;
      /* fwd/snp strength for reserved_mc is not per trap_code_ndx,
         but per reserved_mc DA and trap profile (see reserved mc table) */
    }
    /* PCP_TRAP_CODE_UNKNOWN_DA_0-7 */
    else if ((trap_code_ndx >= PCP_TRAP_CODE_UNKNOWN_DA_0) && (trap_code_ndx <= PCP_TRAP_CODE_UNKNOWN_DA_7))
    {
      diff = (trap_code_ndx - PCP_TRAP_CODE_UNKNOWN_DA_0);
      *trap_code_internal = PCP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX + diff;
     }
    /* PCP_TRAP_CODE_USER_DEFINED_0-59 */
    else if ((trap_code_ndx >= PCP_TRAP_CODE_USER_DEFINED_0) && (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_59))
    {
      if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_5)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_0);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5 + diff;
      }
      else if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_7)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_6);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7 + diff;
      }
      else if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_10)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_8);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10 + diff;
      }
      else if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_12)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_11);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12 + diff;
      }
      else if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_27)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_13);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27 + diff;
      }
      else if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_35)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_28);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35 + diff;
      }
      else if (trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_51)
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_36);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51 + diff;
      }
      else /* if ((trap_code_ndx <= PCP_TRAP_CODE_USER_DEFINED_63) */
      {
        diff = (trap_code_ndx - PCP_TRAP_CODE_USER_DEFINED_52);
        *trap_code_internal = PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_67 + diff;
      }
    }
    else /* Unknown */
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR/*PCP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR*/, 100, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_trap_mgmt_trap_code_to_internal()", trap_code_ndx, 0);
}

/************************************************************************
*   Convert CPU trap code as received on packet to API enumerator.     *
************************************************************************/
uint32
  pcp_trap_cpu_trap_code_from_internal_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int32                                 trap_code_internal,
    SOC_SAND_OUT PCP_TRAP_CODE                          *trap_code
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0/*PCP_TRAP_CPU_TRAP_CODE_FROM_INTERNAL_UNSAFE*/);

  switch (trap_code_internal)
  {
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0:
    *trap_code = PCP_TRAP_CODE_PBP_SA_DROP_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1:
    *trap_code = PCP_TRAP_CODE_PBP_SA_DROP_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2:
    *trap_code = PCP_TRAP_CODE_PBP_SA_DROP_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3:
    *trap_code = PCP_TRAP_CODE_PBP_SA_DROP_3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT:
    *trap_code = PCP_TRAP_CODE_PBP_TE_TRANSPLANT;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL:
    *trap_code = PCP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT:
    *trap_code = PCP_TRAP_CODE_PBP_TRANSPLANT;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP:
    *trap_code = PCP_TRAP_CODE_PBP_LEARN_SNOOP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED:
    *trap_code = PCP_TRAP_CODE_SA_AUTHENTICATION_FAILED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED:
    *trap_code = PCP_TRAP_CODE_PORT_NOT_PERMITTED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID:
    *trap_code = PCP_TRAP_CODE_UNEXPECTED_VID;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST:
    *trap_code = PCP_TRAP_CODE_SA_MULTICAST;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA:
    *trap_code = PCP_TRAP_CODE_SA_EQUALS_DA;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_8021X:
    *trap_code = PCP_TRAP_CODE_8021X;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0:
    *trap_code = PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1:
    *trap_code = PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2:
    *trap_code = PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3:
    *trap_code = PCP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_MY_ARP:
    *trap_code = PCP_TRAP_CODE_MY_ARP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ARP:
    *trap_code = PCP_TRAP_CODE_ARP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY:
    *trap_code = PCP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG:
    *trap_code = PCP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED:
    *trap_code = PCP_TRAP_CODE_IGMP_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code = PCP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG:
    *trap_code = PCP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED:
    *trap_code = PCP_TRAP_CODE_ICMPV6_MLD_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER:
    *trap_code = PCP_TRAP_CODE_DHCP_SERVER;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT:
    *trap_code = PCP_TRAP_CODE_DHCP_CLIENT;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER:
    *trap_code = PCP_TRAP_CODE_DHCPV6_SERVER;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT:
    *trap_code = PCP_TRAP_CODE_DHCPV6_CLIENT;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0:
    *trap_code = PCP_TRAP_CODE_PROG_TRAP_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1:
    *trap_code = PCP_TRAP_CODE_PROG_TRAP_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2:
    *trap_code = PCP_TRAP_CODE_PROG_TRAP_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3:
    *trap_code = PCP_TRAP_CODE_PROG_TRAP_3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER:
    *trap_code = PCP_TRAP_CODE_PORT_NOT_VLAN_MEMBER;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR:
    *trap_code = PCP_TRAP_CODE_HEADER_SIZE_ERR;
    break;
  case PCP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS:
    *trap_code = PCP_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL:
    *trap_code = PCP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID:
    *trap_code = PCP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK:
    *trap_code = PCP_TRAP_CODE_STP_STATE_BLOCK;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN:
    *trap_code = PCP_TRAP_CODE_STP_STATE_LEARN;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP:
    *trap_code = PCP_TRAP_CODE_IP_COMP_MC_INVALID_IP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE:
    *trap_code = PCP_TRAP_CODE_MY_MAC_AND_IP_DISABLE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION:
    *trap_code = PCP_TRAP_CODE_TRILL_VERSION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL:
    *trap_code = PCP_TRAP_CODE_TRILL_INVALID_TTL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH:
    *trap_code = PCP_TRAP_CODE_TRILL_CHBH;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC:
    *trap_code = PCP_TRAP_CODE_TRILL_NO_REVERSE_FEC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE:
    *trap_code = PCP_TRAP_CODE_TRILL_CITE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC:
    *trap_code = PCP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE:
    *trap_code = PCP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP:
    *trap_code = PCP_TRAP_CODE_MY_MAC_AND_ARP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3:
    *trap_code = PCP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0:
    *trap_code = PCP_TRAP_CODE_MPLS_LABEL_VALUE_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1:
    *trap_code = PCP_TRAP_CODE_MPLS_LABEL_VALUE_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2:
    *trap_code = PCP_TRAP_CODE_MPLS_LABEL_VALUE_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3:
    *trap_code = PCP_TRAP_CODE_MPLS_LABEL_VALUE_3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES:
    *trap_code = PCP_TRAP_CODE_MPLS_NO_RESOURCES;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE:
    *trap_code = PCP_TRAP_CODE_INVALID_LABEL_IN_RANGE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM:
    *trap_code = PCP_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS:
    *trap_code = PCP_TRAP_CODE_MPLS_LSP_BOS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14:
    *trap_code = PCP_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS:
    *trap_code = PCP_TRAP_CODE_MPLS_PWE_NO_BOS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS:
    *trap_code = PCP_TRAP_CODE_MPLS_VRF_NO_BOS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0:
    *trap_code = PCP_TRAP_CODE_MPLS_TERM_TTL_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP:
    *trap_code = PCP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP:
    *trap_code = PCP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_VERSION_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_TTL0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_HAS_OPTIONS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_TTL1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_DIP_ZERO;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC:
    *trap_code = PCP_TRAP_CODE_IPV4_TERM_SIP_IS_MC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    *trap_code = PCP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS:
    *trap_code = PCP_TRAP_CODE_CFM_ACCELERATED_INGRESS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC:
    *trap_code = PCP_TRAP_CODE_ILLEGEL_PFC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP0:
    *trap_code = PCP_TRAP_CODE_SA_DROP_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP1:
    *trap_code = PCP_TRAP_CODE_SA_DROP_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP2:
    *trap_code = PCP_TRAP_CODE_SA_DROP_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_DROP3:
    *trap_code = PCP_TRAP_CODE_SA_DROP_3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0:
    *trap_code = PCP_TRAP_CODE_SA_NOT_FOUND_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1:
    *trap_code = PCP_TRAP_CODE_SA_NOT_FOUND_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2:
    *trap_code = PCP_TRAP_CODE_SA_NOT_FOUND_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3:
    *trap_code = PCP_TRAP_CODE_SA_NOT_FOUND_3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR:
    *trap_code = PCP_TRAP_CODE_ELK_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND0:
    *trap_code = PCP_TRAP_CODE_DA_NOT_FOUND_0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND1:
    *trap_code = PCP_TRAP_CODE_DA_NOT_FOUND_1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND2:
    *trap_code = PCP_TRAP_CODE_DA_NOT_FOUND_2;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND3:
    *trap_code = PCP_TRAP_CODE_DA_NOT_FOUND_3;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION:
    *trap_code = PCP_TRAP_CODE_P2P_MISCONFIGURATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE:
    *trap_code = PCP_TRAP_CODE_SAME_INTERFACE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC:
    *trap_code = PCP_TRAP_CODE_TRILL_UNKNOWN_UC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC:
    *trap_code = PCP_TRAP_CODE_TRILL_UNKNOWN_MC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL:
    *trap_code = PCP_TRAP_CODE_UC_LOOSE_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6:
    *trap_code = PCP_TRAP_CODE_DEFAULT_UCV6;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6:
    *trap_code = PCP_TRAP_CODE_DEFAULT_MCV6;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS:
    *trap_code = PCP_TRAP_CODE_MPLS_P2P_NO_BOS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP:
    *trap_code = PCP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP:
    *trap_code = PCP_TRAP_CODE_MPLS_CONTROL_WORD_DROP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL:
    *trap_code = PCP_TRAP_CODE_MPLS_UNKNOWN_LABEL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4:
    *trap_code = PCP_TRAP_CODE_MPLS_P2P_MPLSX4;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER:
    *trap_code = PCP_TRAP_CODE_ETH_L2CP_PEER;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP:
    *trap_code = PCP_TRAP_CODE_ETH_L2CP_DROP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    *trap_code = PCP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    *trap_code = PCP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED:
    *trap_code = PCP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    *trap_code = PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    *trap_code = PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED:
    *trap_code = PCP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_VERSION_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_CHECKSUM_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0:
    *trap_code = PCP_TRAP_CODE_IPV4_TTL0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS:
    *trap_code = PCP_TRAP_CODE_IPV4_HAS_OPTIONS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1:
    *trap_code = PCP_TRAP_CODE_IPV4_TTL1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP:
    *trap_code = PCP_TRAP_CODE_IPV4_SIP_EQUAL_DIP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO:
    *trap_code = PCP_TRAP_CODE_IPV4_DIP_ZERO;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC:
    *trap_code = PCP_TRAP_CODE_IPV4_SIP_IS_MC;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR:
    *trap_code = PCP_TRAP_CODE_IPV6_VERSION_ERROR;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0:
    *trap_code = PCP_TRAP_CODE_IPV6_HOP_COUNT0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1:
    *trap_code = PCP_TRAP_CODE_IPV6_HOP_COUNT1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION:
    *trap_code = PCP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS:
    *trap_code = PCP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE:
    *trap_code = PCP_TRAP_CODE_IPV6_MULTICAST_SOURCE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL:
    *trap_code = PCP_TRAP_CODE_IPV6_NEXT_HEADER_NULL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE:
    *trap_code = PCP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION:
    *trap_code = PCP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION:
    *trap_code = PCP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE:
    *trap_code = PCP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE:
    *trap_code = PCP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION:
    *trap_code = PCP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION:
    *trap_code = PCP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION:
    *trap_code = PCP_TRAP_CODE_IPV6_MULTICAST_DESTINATION;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0:
    *trap_code = PCP_TRAP_CODE_MPLS_TTL0;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1:
    *trap_code = PCP_TRAP_CODE_MPLS_TTL1;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO:
    *trap_code = PCP_TRAP_CODE_TCP_SN_FLAGS_ZERO;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET:
    *trap_code = PCP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN:
    *trap_code = PCP_TRAP_CODE_TCP_SYN_FIN;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS:
    *trap_code = PCP_TRAP_CODE_TCP_EQUAL_PORTS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER:
    *trap_code = PCP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8:
    *trap_code = PCP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS:
    *trap_code = PCP_TRAP_CODE_UDP_EQUAL_PORTS;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576:
    *trap_code = PCP_TRAP_CODE_ICMP_DATA_GT_576;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED:
    *trap_code = PCP_TRAP_CODE_ICMP_FRAGMENTED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_PMF_GENERAL:
    *trap_code = PCP_TRAP_CODE_GENERAL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID:
    *trap_code = PCP_TRAP_CODE_FACILITY_INVALID;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED:
    *trap_code = PCP_TRAP_CODE_FEC_ENTRY_ACCESSED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL:
    *trap_code = PCP_TRAP_CODE_UC_STRICT_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL:
    *trap_code = PCP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL:
    *trap_code = PCP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL:
    *trap_code = PCP_TRAP_CODE_MC_USE_SIP_RPF_FAIL;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP:
    *trap_code = PCP_TRAP_CODE_MC_USE_SIP_ECMP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT:
    *trap_code = PCP_TRAP_CODE_ICMP_REDIRECT;
    break;

  case  PCP_TRAP_CODE_INTERNAL_USER_OAMP:
    *trap_code = PCP_TRAP_CODE_USER_OAMP;
    break;
  case  PCP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED:
    *trap_code = PCP_TRAP_CODE_USER_ETH_OAM_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED:
    *trap_code = PCP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    *trap_code = PCP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED:
    *trap_code = PCP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED;
    break;
  case  PCP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED:
    *trap_code = PCP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED;
    break;

  default:
    /* PCP_TRAP_CODE_RESERVED_MC_0-7 */
    if ((PCP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX + PCP_TRAP_CODE_RESERVED_MC_7 - PCP_TRAP_CODE_RESERVED_MC_0))
    {
      *trap_code = PCP_TRAP_CODE_RESERVED_MC_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX;
    }
    /* PCP_TRAP_CODE_UNKNOWN_DA_0-7 */
    else if ((PCP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX + 7))
    {
      *trap_code = PCP_TRAP_CODE_UNKNOWN_DA_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX;
    }
    /* PCP_TRAP_CODE_USER_DEFINED_0-63 */
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5 + PCP_TRAP_CODE_USER_DEFINED_5 - PCP_TRAP_CODE_USER_DEFINED_0))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5;
    }
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7 + PCP_TRAP_CODE_USER_DEFINED_7 - PCP_TRAP_CODE_USER_DEFINED_6))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7;
    }

    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10 + PCP_TRAP_CODE_USER_DEFINED_10 - PCP_TRAP_CODE_USER_DEFINED_8))
    {
      /* Old code, almost not in use - ignore coverity defects */
      /* coverity[dead_error_line] */
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10;
    }
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12 + PCP_TRAP_CODE_USER_DEFINED_12 - PCP_TRAP_CODE_USER_DEFINED_11))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12;
    }
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27 + PCP_TRAP_CODE_USER_DEFINED_27 - PCP_TRAP_CODE_USER_DEFINED_13))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27;
    }
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35 + PCP_TRAP_CODE_USER_DEFINED_35 - PCP_TRAP_CODE_USER_DEFINED_28))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35;
    }
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51 + PCP_TRAP_CODE_USER_DEFINED_51 - PCP_TRAP_CODE_USER_DEFINED_36))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51;
    }
    else if ((PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_67 <= trap_code_internal) &&
      (trap_code_internal <= PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_67 + 15 /* PCP_TRAP_CODE_USER_DEFINED_67 - PCP_TRAP_CODE_USER_DEFINED_52 */))
    {
      *trap_code = PCP_TRAP_CODE_USER_DEFINED_0 + trap_code_internal - PCP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_67;
    }
    /* Unknown */
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR/*PCP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR*/, 100, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_trap_mgmt_trap_code_from_internal()", trap_code_internal, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

