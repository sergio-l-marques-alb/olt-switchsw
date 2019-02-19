/* $Id: pcp_oam_eth.c,v 1.11 Broadcom SDK $
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
*/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_exact_match_hash.h>

#include <soc/dpp/PCP/pcp_api_tbl_access.h>

#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_oam_general.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_sw_db.h>

#include <soc/dpp/PCP/pcp_oam_eth.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_OAM_ETH_PRIO_NDX_MAX                               (7)
#define PCP_OAM_ETH_HEADER_MAX                                 (SOC_SAND_U32_MAX)
#define PCP_OAM_ETH_RMEP_MAX                                   (8192 - 1)
#define PCP_OAM_ETH_RMEP_NDX_MAX                               (8192 - 1)
#define PCP_OAM_ETH_CUSTOM_HEADER_SIZE_MAX                     (4)
#define PCP_OAM_ETH_TC_MAX                                     (7)
#define PCP_OAM_ETH_DP_MAX                                     (3)
#define PCP_OAM_ETH_DIRECTION_MAX                              (PCP_OAM_NOF_ETH_MEP_DIRECTIONS-1)
#define PCP_OAM_ETH_UP_VID_MAX                                 (4096 - 1)
#define PCP_OAM_ETH_DN_VID_MAX                                 (4096 - 1)
#define PCP_OAM_ETH_AC_MAX                                     (16384 - 1)
#define PCP_OAM_ETH_CCM_INTERVAL_MAX                           (PCP_OAM_NOF_ETH_INTERVALS-1)
#define PCP_OAM_ETH_CCM_PRIORITY_MAX                               (7)
#define PCP_OAM_ETH_MA_ID_MAX                                  (65536 - 1)
#define PCP_OAM_ETH_MEP_ID_MAX                                 (8192 - 1)
#define PCP_OAM_ETH_RMEP_ID_MAX                                (8192 - 1)
#define PCP_OAM_ETH_INTERVAL_MAX                               (PCP_OAM_NOF_ETH_INTERVALS-1)
#define PCP_OAM_ETH_TX_FCF_CURR_MAX                            (SOC_SAND_U32_MAX)
#define PCP_OAM_ETH_TX_FCF_PREV_MAX                            (SOC_SAND_U32_MAX)
#define PCP_OAM_ETH_RX_FCB_CURR_MAX                            (SOC_SAND_U32_MAX)
#define PCP_OAM_ETH_RX_FCB_PREV_MAX                            (SOC_SAND_U32_MAX)
#define PCP_OAM_ETH_AC_NDX_MAX                                 (16384)
#define PCP_OAM_ETH_ACC_MEP_ID_MAX                             (4096 - 1)
#define PCP_OAM_ETH_MP_LEVEL_MAX                               (7)
#define PCP_OAM_ETH_COS_PROFILE_ID_MAX                         (3)
#define PCP_OAM_ETH_ACC_LMM_ID_MAX                             (127)
#define PCP_OAM_ETH_ACC_RX_COUNTER_MAX                   (2048 -  1)
#define PCP_OAM_ETH_ACC_TX_COUNTER_MAX                   (2048 -  1)
#define PCP_OAM_ETH_ACC_DLM_ID_MAX                             (127)

/* Destanation Type - flow - parameters encoding */ 
#define PCP_OAM_ETH_DEST_TYPE_FLOW_BASE              (0x8000)
#define PCP_OAM_ETH_DEST_TYPE_FLOW_LSB                (0)
#define PCP_OAM_ETH_DEST_TYPE_FLOW_MSB               (14)
#define PCP_OAM_ETH_DEST_TYPE_FLOW_SHIFT             (PCP_OAM_ETH_DEST_TYPE_FLOW_LSB)
#define PCP_OAM_ETH_DEST_TYPE_FLOW_MASK             (SOC_SAND_BITS_MASK(PCP_OAM_ETH_DEST_TYPE_FLOW_MSB, PCP_OAM_ETH_DEST_TYPE_FLOW_LSB))

/* HW-SW interface parametrs for 41 high bits mac sas */
#define PCP_OAM_ETH_GENERAL_INFO_MAC_SA_LSB_SHIFT                   7
#define PCP_OAM_ETH_GENERAL_INFO_MAC_SA_MSB_SHIFT                   25

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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_oam_eth[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GENERAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_COS_MAPPING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_LMM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_DLM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_RMEP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_ETH_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_oam_eth[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_OAM_ETH_NO_VID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_NO_VID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_NO_VID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_NO_OUT_AC_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_NO_OUT_AC_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_NO_OUT_AC' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_COS_PROFILE_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_COS_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_COS_PROFILE_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_MP_LEVEL_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_MP_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_MP_LEVEL' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_ACC_MEP_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_ACC_LMM_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_ETH_ACC_DLM_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_PRIO_NDX_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_PRIO_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'prio_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_HEADER_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_HEADER_OUT_OF_RANGE_ERR",
    "The parameter 'header' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_RMEP_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_RMEP_OUT_OF_RANGE_ERR",
    "The parameter 'rmep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_RMEP_NDX_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_RMEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'rmep_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_RDI_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_RDI_OUT_OF_RANGE_ERR",
    "The parameter 'rdi' is out of range. \n\r "
    "The range is: 0 - No max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_DM_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_DM_OUT_OF_RANGE_ERR",
    "The parameter 'dm' is out of range. \n\r "
    "The range is: 0 - No max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_CUSTOM_HEADER_SIZE_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_CUSTOM_HEADER_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'custom_header_size' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_TC_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_TC_OUT_OF_RANGE_ERR",
    "The parameter 'tc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_DP_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_DP_OUT_OF_RANGE_ERR",
    "The parameter 'dp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_DIRECTION_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_DIRECTION_OUT_OF_RANGE_ERR",
    "The parameter 'direction' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_ETH_MEP_DIRECTIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_DN_VID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_DN_VID_OUT_OF_RANGE_ERR",
    "The parameter 'dn_vid' is out of range. \n\r "
    "The range is: 0 - 4k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_AC_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_AC_OUT_OF_RANGE_ERR",
    "The parameter 'ac' is out of range. \n\r "
    "The range is: 0 - 16k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_UP_VID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_UP_VID_OUT_OF_RANGE_ERR",
    "The parameter 'up_vid' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_CCM_INTERVAL_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_CCM_INTERVAL_OUT_OF_RANGE_ERR",
    "The parameter 'ccm_interval' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_ETH_INTERVALS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_CCM_PRIORITY_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_CCM_PRIORITY_OUT_OF_RANGE_ERR",
    "The parameter 'ccm_priority' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_MA_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_MA_ID_OUT_OF_RANGE_ERR",
    "The parameter 'ma_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_MEP_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_MEP_ID_OUT_OF_RANGE_ERR",
    "The parameter 'mep_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_RMEP_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_RMEP_ID_OUT_OF_RANGE_ERR",
    "The parameter 'rmep_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_INTERVAL_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_INTERVAL_OUT_OF_RANGE_ERR",
    "The parameter 'interval' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_ETH_INTERVALS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_PRIORITY_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_PRIORITY_OUT_OF_RANGE_ERR",
    "The parameter 'priority' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_TX_FCF_CURR_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_TX_FCF_CURR_OUT_OF_RANGE_ERR",
    "The parameter 'tx_fcf_curr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_TX_FCF_PREV_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_TX_FCF_PREV_OUT_OF_RANGE_ERR",
    "The parameter 'tx_fcf_prev' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_RX_FCB_CURR_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_RX_FCB_CURR_OUT_OF_RANGE_ERR",
    "The parameter 'rx_fcb_curr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_ETH_RX_FCB_PREV_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_RX_FCB_PREV_OUT_OF_RANGE_ERR",
    "The parameter 'rx_fcb_prev' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
   {
    PCP_OAM_ETH_DESTINATION_TYPE_UNKNOWN_ERR,
    "PCP_OAM_ETH_DESTINATION_TYPE_UNKNOWN_ERR",
    "The parameter 'dest_type' value in Unknown. \n\r "
    "The possible values are: port, queue, multicast...\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
   },
   {
    PCP_OAM_ETH_RMEP_ADD_FAILURE_ERR,
    "PCP_OAM_ETH_RMEP_ADD_FAILURE_ERR",
    "unable to add rmep by key to table \n\r "
    "possibly out of resources\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
   },
   {
    PCP_OAM_ETH_RMEP_REMOVE_FOUND_FAILURE_ERR,
    "PCP_OAM_ETH_RMEP_REMOVE_FOUND_FAILURE_ERR",
    "unable to find key in table.\n\r "
    "possibly trying to remove unadded key\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
   },
   {
    PCP_OAM_ETH_ACC_RX_COUNTER_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_ACC_RX_COUNTER_OUT_OF_RANGE_ERR",
    "The parameter 'rx_counter' is out of range. \n\r "
    "The range is: 0 - (2048 - 1).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
   },
   {
    PCP_OAM_ETH_ACC_TX_COUNTER_OUT_OF_RANGE_ERR,
    "PCP_OAM_ETH_ACC_TX_COUNTER_OUT_OF_RANGE_ERR",
    "The parameter 'tx_counter' is out of range. \n\r "
    "The range is: 0 - (2048 - 1).\n\r ",
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

/*********************************************************************
*     This function sets general configuration of Ethernet
 *     OAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_general_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_sa_base[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    mac_sa_base_lsb = 0,
    mac_sa_base_msb = 0;
  PCP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_GENERAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = pcp_regs();

  PCP_FLD_SET(regs->oam.ptch_size_reg.ptch_size , info->custom_header_size, 10, exit);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          &info->mep_sa_msb,
          mac_sa_base
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mac_sa_base_lsb   = (mac_sa_base[0] >> PCP_OAM_ETH_GENERAL_INFO_MAC_SA_LSB_SHIFT) | (mac_sa_base[1] << PCP_OAM_ETH_GENERAL_INFO_MAC_SA_MSB_SHIFT);
  mac_sa_base_msb =  mac_sa_base[1] >> PCP_OAM_ETH_GENERAL_INFO_MAC_SA_LSB_SHIFT;

  PCP_FLD_SET(regs->oam.mac_sa_base_reg_0.mac_sa_base, mac_sa_base_lsb  , 20, exit);
  PCP_FLD_SET(regs->oam.mac_sa_base_reg_1.mac_sa_base, mac_sa_base_msb, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_general_info_set_unsafe()", 0, 0);
}

uint32
  pcp_oam_eth_general_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_GENERAL_INFO_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_GENERAL_INFO, info, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_general_info_set_verify()", 0, 0);
}

uint32
  pcp_oam_eth_general_info_get_verify(
    SOC_SAND_IN  int                       unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_GENERAL_INFO_GET_VERIFY);

  /* IMPLEMENTED */

  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_general_info_get_verify()", 0, 0);
}

/*********************************************************************
*     This function sets general configuration of Ethernet
 *     OAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_general_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_ETH_GENERAL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg,
    mac_sa_base[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    mac_sa_base_lsb = 0,
    mac_sa_base_msb = 0;
  PCP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_GENERAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_GENERAL_INFO_clear(info);

  regs = pcp_regs();

  PCP_FLD_GET(regs->oam.ptch_size_reg.ptch_size , reg, 10, exit);
  info->custom_header_size = (uint8)reg;

  PCP_FLD_GET(regs->oam.mac_sa_base_reg_0.mac_sa_base, mac_sa_base_lsb, 20, exit);
  PCP_FLD_GET(regs->oam.mac_sa_base_reg_1.mac_sa_base, mac_sa_base_msb, 30, exit);

  mac_sa_base[0]  = mac_sa_base_lsb   << PCP_OAM_ETH_GENERAL_INFO_MAC_SA_LSB_SHIFT;
  mac_sa_base[1]  = mac_sa_base_lsb   >> PCP_OAM_ETH_GENERAL_INFO_MAC_SA_MSB_SHIFT;
  mac_sa_base[1] |= mac_sa_base_msb << PCP_OAM_ETH_GENERAL_INFO_MAC_SA_LSB_SHIFT;

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          mac_sa_base,
          &info->mep_sa_msb
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_general_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This function maps the cos profile and the user priority
 *     (both are MEP attributes) to TC and DP for the ITMH
 *     (relevant for down MEP injection)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK,
    table_entry_offset;
  PCP_OAM_PR2_TCDP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_COS_MAPPING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  table_entry_offset = 0;
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(prio_ndx,2,0);
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(cos_ndx,4,3);

  tbl_data.dp = info->dp;
  tbl_data.tc = info->tc;

  res = pcp_oam_pr2_tcdp_tbl_set_unsafe(
          unit,
          table_entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_cos_mapping_info_set_unsafe()", cos_ndx, prio_ndx);
}

uint32
  pcp_oam_eth_cos_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_COS_MAPPING_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cos_ndx, PCP_OAM_ETH_COS_PROFILE_ID_MAX, PCP_OAM_ETH_COS_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prio_ndx, PCP_OAM_ETH_PRIO_NDX_MAX, PCP_OAM_ETH_PRIO_NDX_OUT_OF_RANGE_ERR, 20, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_COS_MAPPING_INFO, info, 30, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_cos_mapping_info_set_verify()", cos_ndx, prio_ndx);
}

uint32
  pcp_oam_eth_cos_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_COS_MAPPING_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cos_ndx, PCP_OAM_ETH_COS_PROFILE_ID_MAX, PCP_OAM_ETH_COS_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prio_ndx, PCP_OAM_ETH_PRIO_NDX_MAX, PCP_OAM_ETH_PRIO_NDX_OUT_OF_RANGE_ERR, 20, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_cos_mapping_info_get_verify()", cos_ndx, prio_ndx);
}

/*********************************************************************
*     This function maps the cos profile and the user priority
 *     (both are MEP attributes) to TC and DP for the ITMH
 *     (relevant for down MEP injection)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_COS_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK,
    table_entry_offset;
  PCP_OAM_PR2_TCDP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_COS_MAPPING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_COS_MAPPING_INFO_clear(info);

  table_entry_offset = 0;
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(prio_ndx,2,0);
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(cos_ndx,4,3);

  res = pcp_oam_pr2_tcdp_tbl_get_unsafe(
          unit,
          table_entry_offset,
          &tbl_data
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->dp = tbl_data.dp;
  info->tc = tbl_data.tc;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_cos_mapping_info_get_unsafe()", cos_ndx, prio_ndx);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an Attachment-Circuit (incoming/outgoing), MD-level,
 *     and MP type (up/down). If the MP is one of the 4K
 *     accelerated MEPs, the function configures the related
 *     OAMP databases and associates the AC, MD-Level and the
 *     MP type with a user-provided handle. This handle is
 *     later used by user to access OAMP database for this MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mp_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK,
    table_entry_offset;
  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mp_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  table_entry_offset = 0;
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(mp_key->level,2,0);
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(mp_key->ac_ndx,16,3);
  /* If direction is UP set bit, if direction is DOWN bit is cleared (already is - do nothing) */
  if (mp_key->direction == PCP_OAM_ETH_MEP_DIRECTION_UP)
  {
      table_entry_offset |= SOC_SAND_SET_BITS_RANGE(0x1,17,17);
  }
  
  tbl_data.accelerate = SOC_SAND_BOOL2NUM(info->valid);
  tbl_data.mep_db_ptr = info->acc_mep_id;

  res = pcp_oam_ext_mep_index_db_tbl_set_unsafe(
          unit,
          table_entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mp_info_set_unsafe()", 0, 0);
}

uint32
  pcp_oam_mp_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MP_INFO_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_MP_KEY, mp_key, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_MP_INFO, info, 20, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mp_info_set_verify()", 0, 0);
}

uint32
  pcp_oam_mp_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MP_INFO_GET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_MP_KEY, mp_key, 10, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mp_info_get_verify()", 0, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an Attachment-Circuit (incoming/outgoing), MD-level,
 *     and MP direction (up/down). If the MP is one of the 4K
 *     accelerated MEPs, the function configures the related
 *     OAMP databases and associates the AC, MD-Level and the
 *     MP type with a user-provided handle. This handle is
 *     later used by user to access OAMP database for this MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mp_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_OUT PCP_OAM_ETH_MP_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK,
    table_entry_offset;
  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mp_key);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_MP_INFO_clear(info);

  table_entry_offset = 0;
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(mp_key->level,2,0);
  table_entry_offset |= SOC_SAND_SET_BITS_RANGE(mp_key->ac_ndx,16,3);
  /* If direction is UP set bit, if direction is DOWN bit is cleared (already is - do nothing) */
  if (mp_key->direction == PCP_OAM_ETH_MEP_DIRECTION_UP)
  {
      table_entry_offset |= SOC_SAND_SET_BITS_RANGE(0x1,17,17);
  }

  res = pcp_oam_ext_mep_index_db_tbl_get_unsafe(
          unit,
          table_entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->valid      = SOC_SAND_NUM2BOOL(tbl_data.accelerate);
  info->acc_mep_id = tbl_data.mep_db_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mp_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This function configures an accelerated MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_MEP_DB_TBL_DATA
    mep_tbl_data;
  PCP_OAM_EXT_MEP_DB_TBL_DATA
    ext_mep_tbl_data;
  SOC_SAND_PP_DESTINATION_ID
    dest_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_mep_db_tbl_get_unsafe(
          unit,
          acc_mep_ndx,
          &mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_oam_ext_mep_db_tbl_get_unsafe(
          unit,
          acc_mep_ndx,
          &ext_mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  mep_tbl_data.maid            = info->ma_id;
  mep_tbl_data.md_level        = info->md_level;
  ext_mep_tbl_data.mepid       = info->mep_id;
  mep_tbl_data.up1down0        = info->direction == PCP_OAM_ETH_MEP_DIRECTION_UP ? 1 : 0; /* if direction is up than '1' should be written to HW */
  
  if (info->direction == PCP_OAM_ETH_MEP_DIRECTION_UP)
  {
    mep_tbl_data.sa_lsb  = SOC_SAND_GET_BITS_RANGE(info->frwd_info.up_mep.up_sa_lsb.address[0],6,0);
    ext_mep_tbl_data.vid = info->frwd_info.up_mep.up_vid;
  }
  else /* PCP_OAM_ETH_MEP_DIRECTION_DOWN */
  {
    mep_tbl_data.sa_lsb      = SOC_SAND_GET_BITS_RANGE(info->frwd_info.down_mep.dn_sa_lsb.address[0],6,0);
    ext_mep_tbl_data.vid     = info->frwd_info.down_mep.dn_vid;
    ext_mep_tbl_data.out_ac  = info->frwd_info.down_mep.ac;
    mep_tbl_data.cos_profile = info->frwd_info.down_mep.cos_profile;

    /* Encode destination type and id */
    if (info->frwd_info.down_mep.destination.type == PCP_DEST_TYPE_QUEUE) {
        mep_tbl_data.destination = PCP_OAM_ETH_DEST_TYPE_FLOW_BASE; 
        mep_tbl_data.destination |= SOC_SAND_SET_FLD_IN_PLACE(info->frwd_info.down_mep.destination.id,PCP_OAM_ETH_DEST_TYPE_FLOW_SHIFT, PCP_OAM_ETH_DEST_TYPE_FLOW_MASK);
    } else {
        switch(info->frwd_info.down_mep.destination.type)
        {
        case PCP_DEST_TYPE_MULTICAST:
          dest_info.dest_type = SOC_SAND_PP_DEST_MULTICAST;
          break;
        case PCP_DEST_TYPE_SYS_PHY_PORT:
          dest_info.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
          break;
        case PCP_DEST_TYPE_LAG:
          dest_info.dest_type = SOC_SAND_PP_DEST_LAG;
          break;
        default:
          SOC_SAND_SET_ERROR_CODE(PCP_OAM_ETH_DESTINATION_TYPE_UNKNOWN_ERR, 50, exit);
          break;
        }

        dest_info.dest_val  = info->frwd_info.down_mep.destination.id;
        res = SOC_SAND_PP_DESTINATION_ID_encode(&dest_info, &mep_tbl_data.destination);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
  
  /* CCM info */
  mep_tbl_data.ccm_interval    = info->peformance_monitoring_info.ccm_info.ccm_interval;
  mep_tbl_data.ccm_pr          = info->peformance_monitoring_info.ccm_info.ccm_priority;

  /* LMM info */
  mep_tbl_data.lm_db_index     = info->peformance_monitoring_info.lmm_info.lmm_index;
  if (info->peformance_monitoring_info.lmm_info.lmm_enable)
  {
    if (info->peformance_monitoring_info.lmm_info.is_dual)
    {
      mep_tbl_data.lm_enable       = 0x1;
      mep_tbl_data.lmm_enable      = 0x0;
    }
    else
    {
      mep_tbl_data.lm_enable       = 0x0;
      mep_tbl_data.lmm_enable      = 0x1;
    }
  }
  else
  {
    mep_tbl_data.lm_enable       = 0x0;
    mep_tbl_data.lmm_enable      = 0x0;
  }
  mep_tbl_data.lmr_enable     = info->peformance_monitoring_info.lmm_info.lmr_enable;
  
  /* DLM info */
  mep_tbl_data.dm_enable       = SOC_SAND_BOOL2NUM(info->peformance_monitoring_info.dlm_info.dlm_enable);
  mep_tbl_data.dm_db_index     = info->peformance_monitoring_info.dlm_info.dlm_index;
  
  res = pcp_oam_mep_db_tbl_set_unsafe(
          unit,
          acc_mep_ndx,
          &mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = pcp_oam_ext_mep_db_tbl_set_unsafe(
          unit,
          acc_mep_ndx,
          &ext_mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_info_set_unsafe()", acc_mep_ndx, 0);
}

uint32
  pcp_oam_eth_acc_mep_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_mep_ndx, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_INFO, info, 20, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_info_set_verify()", acc_mep_ndx, 0);
}

uint32
  pcp_oam_eth_acc_mep_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_mep_ndx, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_info_get_verify()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function configures an accelerated MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_MEP_DB_TBL_DATA
    mep_tbl_data;
  PCP_OAM_EXT_MEP_DB_TBL_DATA
    ext_mep_tbl_data;
  SOC_SAND_PP_DESTINATION_ID
    dest_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_ACC_MEP_INFO_clear(info);

  res = pcp_oam_mep_db_tbl_get_unsafe(
    unit,
    acc_mep_ndx,
    &mep_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_oam_ext_mep_db_tbl_get_unsafe(
    unit,
    acc_mep_ndx,
    &ext_mep_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->ma_id     = mep_tbl_data.maid;
  info->md_level  = mep_tbl_data.md_level;
  info->mep_id    = ext_mep_tbl_data.mepid;
  info->direction = mep_tbl_data.up1down0 == 0x1 ? PCP_OAM_ETH_MEP_DIRECTION_UP : PCP_OAM_ETH_MEP_DIRECTION_DOWN;

  if (info->direction == PCP_OAM_ETH_MEP_DIRECTION_UP)
  {
    info->frwd_info.up_mep.up_sa_lsb.address[0] = (uint8)mep_tbl_data.sa_lsb;
    info->frwd_info.up_mep.up_vid               = ext_mep_tbl_data.vid;
  }
  else /* PCP_OAM_ETH_MEP_DIRECTION_DOWN */
  {
    info->frwd_info.down_mep.dn_sa_lsb.address[0] = (uint8)mep_tbl_data.sa_lsb;
    info->frwd_info.down_mep.dn_vid               = ext_mep_tbl_data.vid;
    info->frwd_info.down_mep.ac                   = ext_mep_tbl_data.out_ac;
    info->frwd_info.down_mep.cos_profile          = mep_tbl_data.cos_profile;

    /* Encode destination type and id */
    if ((mep_tbl_data.destination & PCP_OAM_ETH_DEST_TYPE_FLOW_BASE) == PCP_OAM_ETH_DEST_TYPE_FLOW_BASE) {
        info->frwd_info.down_mep.destination.type = PCP_DEST_TYPE_QUEUE;
        info->frwd_info.down_mep.destination.id     = SOC_SAND_GET_FLD_FROM_PLACE(mep_tbl_data.destination, PCP_OAM_ETH_DEST_TYPE_FLOW_SHIFT, PCP_OAM_ETH_DEST_TYPE_FLOW_MASK);
    } else {
                
        res = SOC_SAND_PP_DESTINATION_ID_decode(mep_tbl_data.destination, &dest_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
        info->frwd_info.down_mep.destination.id = dest_info.dest_val;
    
        switch(dest_info.dest_type)
        {
        case SOC_SAND_PP_DEST_MULTICAST:
          info->frwd_info.down_mep.destination.type = PCP_DEST_TYPE_MULTICAST;
          break;
        case SOC_SAND_PP_DEST_SINGLE_PORT:
          info->frwd_info.down_mep.destination.type = PCP_DEST_TYPE_SYS_PHY_PORT;
         break;
        case SOC_SAND_PP_DEST_LAG:
          info->frwd_info.down_mep.destination.type = PCP_DEST_TYPE_LAG;
          break;
        default:
          SOC_SAND_SET_ERROR_CODE(PCP_OAM_ETH_DESTINATION_TYPE_UNKNOWN_ERR, 50, exit);
          break;
        }
    }
  }

  /* CMM info */
  info->peformance_monitoring_info.ccm_info.ccm_interval  = mep_tbl_data.ccm_interval;
  info->peformance_monitoring_info.ccm_info.ccm_priority  = (uint8)mep_tbl_data.ccm_pr;

  /* LMM info */
  info->peformance_monitoring_info.lmm_info.lmm_enable = mep_tbl_data.lmm_enable;
  info->peformance_monitoring_info.lmm_info.lmm_index  = mep_tbl_data.lm_db_index;
  info->peformance_monitoring_info.lmm_info.is_dual = mep_tbl_data.lm_enable;
  info->peformance_monitoring_info.lmm_info.lmr_enable = mep_tbl_data.lmr_enable;

  /* DLM info */
  info->peformance_monitoring_info.dlm_info.dlm_enable = SOC_SAND_NUM2BOOL(mep_tbl_data.dm_enable);
  info->peformance_monitoring_info.dlm_info.dlm_index  = mep_tbl_data.dm_db_index;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_info_get_unsafe()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets user custom header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint32                        header
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_EXT_MEP_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_UNSAFE);

  res = pcp_oam_ext_mep_db_tbl_get_unsafe(
          unit,
          acc_mep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.ptc_header = header;

  res = pcp_oam_ext_mep_db_tbl_set_unsafe(
          unit,
          acc_mep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_custom_header_set_unsafe()", acc_mep_ndx, 0);
}

uint32
  pcp_oam_eth_acc_mep_custom_header_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint32                        header
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_mep_ndx, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(header, PCP_OAM_ETH_HEADER_MAX, PCP_OAM_ETH_HEADER_OUT_OF_RANGE_ERR, 20, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_custom_header_set_verify()", acc_mep_ndx, 0);
}

uint32
  pcp_oam_eth_acc_mep_custom_header_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_mep_ndx, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_custom_header_get_verify()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets user custom header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint32                        *header
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_EXT_MEP_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(header);

  res = pcp_oam_ext_mep_db_tbl_get_unsafe(
          unit,
          acc_mep_ndx,
          &tbl_data
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *header = tbl_data.ptc_header;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_custom_header_get_unsafe()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets LMM info for the specified LMM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_LMDB_CMN_TBL_DATA
    cmn_tbl_data;
  PCP_OAM_LMDB_TX_TBL_DATA
    tx_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_LMM_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_lmdb_cmn_tbl_get_unsafe(
          unit,
          acc_lmm_ndx,
          &cmn_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_oam_lmdb_tx_tbl_get_unsafe(
          unit,
          acc_lmm_ndx,
          &tx_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          &info->da,
          cmn_tbl_data.lmm_da
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tx_tbl_data.lmm_interval = info->interval;
  tx_tbl_data.lm_pr        = info->priority;

  cmn_tbl_data.rx_fcb_indx_msb =  info->rx_counter;
  cmn_tbl_data.tx_fcf_indx_msb =  info->tx_counter;

  res = pcp_oam_lmdb_cmn_tbl_set_unsafe(
          unit,
          acc_lmm_ndx,
          &cmn_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_oam_lmdb_tx_tbl_set_unsafe(
          unit,
          acc_lmm_ndx,
          &tx_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_lmm_set_unsafe()", acc_lmm_ndx, 0);
}

uint32
  pcp_oam_eth_acc_lmm_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_LMM_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_lmm_ndx, PCP_OAM_ETH_ACC_LMM_ID_MAX, PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_LMM_INFO, info, 20, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_lmm_set_verify()", acc_lmm_ndx, 0);
}

uint32
  pcp_oam_eth_acc_lmm_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_LMM_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_lmm_ndx, PCP_OAM_ETH_ACC_LMM_ID_MAX, PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_lmm_get_verify()", acc_lmm_ndx, 0);
}

/*********************************************************************
*     This function sets LMM info for the specified LMM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_LMM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_LMDB_CMN_TBL_DATA
    cmn_tbl_data;
  PCP_OAM_LMDB_TX_TBL_DATA
    tx_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_LMM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_ACC_LMM_INFO_clear(info);

  res = pcp_oam_lmdb_cmn_tbl_get_unsafe(
          unit,
          acc_lmm_ndx,
          &cmn_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_oam_lmdb_tx_tbl_get_unsafe(
          unit,
          acc_lmm_ndx,
          &tx_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          cmn_tbl_data.lmm_da,
          &info->da
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->interval = tx_tbl_data.lmm_interval;
  info->priority = (uint8)tx_tbl_data.lm_pr;

  info->rx_counter = cmn_tbl_data.rx_fcb_indx_msb;
  info->tx_counter = cmn_tbl_data.tx_fcf_indx_msb;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_lmm_get_unsafe()", acc_lmm_ndx, 0);
}

/*********************************************************************
*     This function sets DLM info for the specified DLM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_DMDB_TX_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_DLM_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_oam_dmdb_tx_tbl_get_unsafe(
    unit,
    acc_dlm_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    &info->da,
    tbl_data.dm_da
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl_data.dm_interval = info->interval;
  tbl_data.dm_pr = info->priority;

  res = pcp_oam_dmdb_tx_tbl_set_unsafe(
    unit,
    acc_dlm_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_dlm_set_unsafe()", acc_dlm_ndx, 0);
}

uint32
  pcp_oam_eth_acc_dlm_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_DLM_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_dlm_ndx, PCP_OAM_ETH_ACC_DLM_ID_MAX, PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_DLM_INFO, info, 20, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_dlm_set_verify()", acc_dlm_ndx, 0);
}

uint32
  pcp_oam_eth_acc_dlm_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_DLM_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_dlm_ndx, PCP_OAM_ETH_ACC_DLM_ID_MAX, PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR, 10, exit);

 /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_dlm_get_verify()", acc_dlm_ndx, 0);
}

/*********************************************************************
*     This function sets DLM info for the specified DLM
 *     identifier.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_DLM_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_DMDB_TX_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_DLM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_ACC_DLM_INFO_clear(info);

  res = pcp_oam_dmdb_tx_tbl_get_unsafe(
          unit,
          acc_dlm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          tbl_data.dm_da,
          &info->da
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->interval = tbl_data.dm_interval;
  info->priority = (uint8)tbl_data.dm_pr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_dlm_get_unsafe()", acc_dlm_ndx, 0);
}

/*********************************************************************
*     This function adds a remote MEP to list of remote MEPs
 *     associated with the accelerated MEP identified by
 *     acc_mep_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_add_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key,
    SOC_SAND_IN  uint32                        rmep
  )
{
  uint32
    res = SOC_SAND_OK,
    rmep_key_hash_index = 0,
    tbl_id,
    entry_indx,
    nof_steps;
  SOC_SAND_EXACT_MATCH_INFO
    *rmep_exact_match;
  uint8
    exact_match_key[4];
  uint8
    exact_match_payload[4];
  uint8
    is_success;
  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA
    hash_0_tbl_data;
  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA
    hash_1_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rmep_key);

  res = pcp_sw_db_rmep_exact_match_get(
          unit,
          &rmep_exact_match
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  rmep_key_hash_index = SOC_SAND_SET_BITS_RANGE(rmep_key->rmep_id,12,0);
  rmep_key_hash_index |= SOC_SAND_SET_BITS_RANGE(rmep_key->acc_mep_id,24,13);

  res = soc_sand_U32_to_U8(
          &rmep_key_hash_index,
          4,
          exact_match_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_U32_to_U8(
          &rmep,
          4,
          exact_match_payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_exact_match_entry_add(
          rmep_exact_match,
          exact_match_key,
          exact_match_payload,
          &tbl_id,
          &entry_indx,
          &nof_steps,
          &is_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (is_success == TRUE)
  {
    if (tbl_id == 0)
    {
      res = pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
        unit,
        entry_indx,
        &hash_0_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      hash_0_tbl_data.valid_hash0 = SOC_SAND_BOOL2NUM(TRUE);

      res = pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
        unit,
        entry_indx,
        &hash_0_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
    else /* tbl_ndx == 1*/
    {
      res = pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
        unit,
        entry_indx,
        &hash_1_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      hash_1_tbl_data.valid_hash1 = SOC_SAND_BOOL2NUM(TRUE);

      res = pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
        unit,
        entry_indx,
        &hash_1_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    }
  }
   else
  {
    SOC_SAND_SET_ERROR_CODE(PCP_OAM_ETH_RMEP_ADD_FAILURE_ERR, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_add_unsafe()", 0, 0);
}

uint32
  pcp_oam_eth_rmep_add_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key,
    SOC_SAND_IN  uint32                        rmep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_ADD_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_RMEP_KEY, rmep_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(rmep, PCP_OAM_ETH_RMEP_MAX, PCP_OAM_ETH_RMEP_OUT_OF_RANGE_ERR, 20, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_add_verify()", 0, 0);
}

/*********************************************************************
*     This function removes a remote MEP to list of remote
 *     MEPs associated with the accelerated MEP identified by
 *     acc_mep_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_remove_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key
  )
{
  uint32
    res = SOC_SAND_OK,
    rmep_key_hash_index = 0,
    tbl_id,
    entry_indx;
  SOC_SAND_EXACT_MATCH_INFO
    *rmep_exact_match;
  uint8
    exact_match_key[4];
  uint8
    exact_match_payload[4];
  uint8
    found;
  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA
    hash_0_tbl_data;
  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA
    hash_1_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rmep_key);

  res = pcp_sw_db_rmep_exact_match_get(
    unit,
    &rmep_exact_match
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  rmep_key_hash_index = SOC_SAND_SET_BITS_RANGE(rmep_key->acc_mep_id,11,0);
  rmep_key_hash_index |= SOC_SAND_SET_BITS_RANGE(rmep_key->rmep_id,24,12);

  res = soc_sand_U32_to_U8(
          &rmep_key_hash_index,
          4,
          exact_match_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_exact_match_entry_lookup(
          rmep_exact_match,
          exact_match_key,
          &tbl_id,
          &entry_indx,
          exact_match_payload,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (!found)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_OAM_ETH_RMEP_REMOVE_FOUND_FAILURE_ERR, 19, exit);
  }

  res = soc_sand_exact_match_entry_remove(
          rmep_exact_match,
          exact_match_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (tbl_id == 0)
  {
    res = pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
            unit,
            entry_indx,
            &hash_0_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    hash_0_tbl_data.valid_hash0 = SOC_SAND_BOOL2NUM(TRUE);

    res = pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
            unit,
            entry_indx,
            &hash_0_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else /* tbl_ndx == 1*/
  {
    res = pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
            unit,
            entry_indx,
            &hash_1_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    hash_1_tbl_data.valid_hash1 = SOC_SAND_BOOL2NUM(FALSE);

    res = pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
            unit,
            entry_indx,
            &hash_1_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_remove_unsafe()", 0, 0);
}

uint32
  pcp_oam_eth_rmep_remove_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_REMOVE_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_RMEP_KEY, rmep_key, 10, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_remove_verify()", 0, 0);
}

/*********************************************************************
*     This API sets a remote MEP info.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_RMEP_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rmep_info);

  res = pcp_oam_rmep_db_tbl_get_unsafe(
          unit,
          rmep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.rcvd_rdi           = SOC_SAND_BOOL2NUM(rmep_info->rdi_received);
  tbl_data.ccm_interval       = rmep_info->interval;
  tbl_data.ccm_defect         = SOC_SAND_BOOL2NUM(rmep_info->local_defect);

  res = pcp_oam_rmep_db_tbl_set_unsafe(
          unit,
          rmep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_info_set_unsafe()", rmep_ndx, 0);
}

uint32
  pcp_oam_eth_rmep_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_ndx, PCP_OAM_ETH_RMEP_NDX_MAX, PCP_OAM_ETH_RMEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_RMEP_INFO, rmep_info, 20, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_info_set_verify()", rmep_ndx, 0);
}

uint32
  pcp_oam_eth_rmep_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_ndx, PCP_OAM_ETH_RMEP_NDX_MAX, PCP_OAM_ETH_RMEP_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_info_get_verify()", rmep_ndx, 0);
}

/*********************************************************************
*     This API sets a remote MEP info.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_INFO           *rmep_info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_RMEP_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_RMEP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rmep_info);

  PCP_OAM_ETH_RMEP_INFO_clear(rmep_info);

  res = pcp_oam_rmep_db_tbl_get_unsafe(
          unit,
          rmep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  rmep_info->rdi_received = SOC_SAND_NUM2BOOL(tbl_data.rcvd_rdi);
  rmep_info->interval     = tbl_data.ccm_interval;
  rmep_info->local_defect = SOC_SAND_NUM2BOOL(tbl_data.ccm_defect);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_rmep_info_get_unsafe()", rmep_ndx, 0);
}

/*********************************************************************
*     This function sets/clears remote defect indication on
 *     outgoing CCM messages
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint8                       rdi
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_MEP_DB_TBL_DATA
    mep_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_UNSAFE);

  res = pcp_oam_mep_db_tbl_get_unsafe(
          unit,
          acc_mep_ndx,
          &mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mep_tbl_data.rdi_indicator        = SOC_SAND_BOOL2NUM(rdi);

  res = pcp_oam_mep_db_tbl_set_unsafe(
          unit,
          acc_mep_ndx,
          &mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_tx_rdi_set_unsafe()", acc_mep_ndx, 0);
}

uint32
  pcp_oam_eth_acc_mep_tx_rdi_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint8                       rdi
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_mep_ndx, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_tx_rdi_set_verify()", acc_mep_ndx, 0);
}

uint32
  pcp_oam_eth_acc_mep_tx_rdi_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_mep_ndx, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_tx_rdi_get_verify()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function sets/clears remote defect indication on
 *     outgoing CCM messages
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint8                       *rdi
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_MEP_DB_TBL_DATA
    mep_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rdi);


  res = pcp_oam_mep_db_tbl_get_unsafe(
          unit,
          acc_mep_ndx,
          &mep_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *rdi = SOC_SAND_NUM2BOOL(mep_tbl_data.rdi_indicator);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_acc_mep_tx_rdi_get_unsafe()", acc_mep_ndx, 0);
}

/*********************************************************************
*     This function returns the last measured delay. In case
 *     DM was not defined on the provided MEP, an error is
 *     thrown
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_mep_delay_measurement_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT uint32                        *dm
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_DMDB_RX_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dm);
  
  res = pcp_oam_dmdb_rx_tbl_get_unsafe(
          unit,
          acc_dlm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *dm = tbl_data.dm_delay;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_mep_delay_measurement_get_unsafe()", acc_dlm_ndx, 0);
}

uint32
  pcp_oam_eth_mep_delay_measurement_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_dlm_ndx, PCP_OAM_ETH_ACC_DLM_ID_MAX, PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_mep_delay_measurement_get_verify()", acc_dlm_ndx, 0);
}

/*********************************************************************
*     This function returns the loss measurement counters of
 *     the device (both local and remote). In case LM was not
 *     defined on the provided MEP, an error is thrown
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_eth_mep_loss_measurement_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_LMDB_CMN_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_clear(info);

   res = pcp_oam_lmdb_cmn_tbl_get_unsafe(
          unit,
          acc_lmm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->local.tx_fcf_curr = tbl_data.tx_fcf_c;
  info->local.tx_fcf_prev = tbl_data.tx_fcf_p;
  info->peer.rx_fcb_curr = tbl_data.rx_fcb_peer_c;
  info->peer.rx_fcb_prev = tbl_data.rx_fcb_peer_p;
  info->local.rx_fcb_curr = tbl_data.rx_fcb_c;
  info->local.rx_fcb_prev = tbl_data.rx_fcb_p;
  info->peer.tx_fcf_curr = tbl_data.tx_fcb_peer_c;
  info->peer.tx_fcf_prev = tbl_data.tx_fcb_peer_p;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_mep_loss_measurement_get_unsafe()", acc_lmm_ndx, 0);
}

uint32
  pcp_oam_eth_mep_loss_measurement_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(acc_lmm_ndx, PCP_OAM_ETH_ACC_LMM_ID_MAX, PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_eth_mep_loss_measurement_get_verify()", acc_lmm_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_eth_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_oam_eth;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_eth_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_oam_eth;
}
uint32
  PCP_OAM_ETH_GENERAL_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->custom_header_size, PCP_OAM_ETH_CUSTOM_HEADER_SIZE_MAX, PCP_OAM_ETH_CUSTOM_HEADER_SIZE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_GENERAL_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_COS_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, PCP_OAM_ETH_TC_MAX, PCP_OAM_ETH_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, PCP_OAM_ETH_DP_MAX, PCP_OAM_ETH_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_COS_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_MP_KEY_verify(
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->level, PCP_OAM_ETH_MP_LEVEL_MAX, PCP_OAM_ETH_MP_LEVEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ac_ndx, PCP_OAM_ETH_AC_NDX_MAX, PCP_OAM_ETH_DIRECTION_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->direction, PCP_OAM_ETH_DIRECTION_MAX, PCP_OAM_ETH_DIRECTION_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_MP_KEY_verify()",0,0);
}

uint32
  PCP_OAM_ETH_MP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->acc_mep_id, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_MP_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dn_vid, PCP_OAM_ETH_DN_VID_MAX, PCP_OAM_ETH_DN_VID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ac, PCP_OAM_ETH_AC_MAX, PCP_OAM_ETH_AC_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, PCP_OAM_ETH_COS_PROFILE_ID_MAX, PCP_OAM_ETH_COS_PROFILE_ID_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->up_vid, PCP_OAM_ETH_UP_VID_MAX, PCP_OAM_ETH_UP_VID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO, &(info->up_mep), 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO, &(info->down_mep), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_FWD_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_CCM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ccm_interval, PCP_OAM_ETH_CCM_INTERVAL_MAX, PCP_OAM_ETH_CCM_INTERVAL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ccm_priority, PCP_OAM_ETH_CCM_PRIORITY_MAX, PCP_OAM_ETH_CCM_PRIORITY_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_CCM_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LMM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->lmm_index, PCP_OAM_ETH_ACC_LMM_ID_MAX, PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_LMM_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_DLM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dlm_index, PCP_OAM_ETH_ACC_DLM_ID_MAX, PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_DLM_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_CCM_INFO, &(info->ccm_info), 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_LMM_INFO, &(info->lmm_info), 11, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_DLM_INFO, &(info->dlm_info), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ma_id, PCP_OAM_ETH_MA_ID_MAX, PCP_OAM_ETH_MA_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mep_id, PCP_OAM_ETH_MEP_ID_MAX, PCP_OAM_ETH_MEP_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->md_level, PCP_OAM_ETH_MP_LEVEL_MAX, PCP_OAM_ETH_MP_LEVEL_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->direction, PCP_OAM_ETH_DIRECTION_MAX, PCP_OAM_ETH_DIRECTION_OUT_OF_RANGE_ERR, 13, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_FWD_INFO, &(info->frwd_info), 14, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO, &(info->peformance_monitoring_info), 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_RMEP_KEY_verify(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->acc_mep_id, PCP_OAM_ETH_ACC_MEP_ID_MAX, PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->rmep_id, PCP_OAM_ETH_RMEP_ID_MAX, PCP_OAM_ETH_RMEP_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_RMEP_KEY_verify()",0,0);
}

uint32
  PCP_OAM_ETH_RMEP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->interval, PCP_OAM_ETH_INTERVAL_MAX, PCP_OAM_ETH_INTERVAL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_RMEP_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_DLM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->interval, PCP_OAM_ETH_INTERVAL_MAX, PCP_OAM_ETH_INTERVAL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_DLM_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_LMM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->interval, PCP_OAM_ETH_INTERVAL_MAX, PCP_OAM_ETH_INTERVAL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->rx_counter, PCP_OAM_ETH_ACC_RX_COUNTER_MAX, PCP_OAM_ETH_ACC_RX_COUNTER_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_counter, PCP_OAM_ETH_ACC_TX_COUNTER_MAX, PCP_OAM_ETH_ACC_TX_COUNTER_OUT_OF_RANGE_ERR, 16, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_LMM_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_fcf_curr, PCP_OAM_ETH_TX_FCF_CURR_MAX, PCP_OAM_ETH_TX_FCF_CURR_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_fcf_prev, PCP_OAM_ETH_TX_FCF_PREV_MAX, PCP_OAM_ETH_TX_FCF_PREV_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->rx_fcb_curr, PCP_OAM_ETH_RX_FCB_CURR_MAX, PCP_OAM_ETH_RX_FCB_CURR_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->rx_fcb_prev, PCP_OAM_ETH_RX_FCB_PREV_MAX, PCP_OAM_ETH_RX_FCB_PREV_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_verify()",0,0);
}

uint32
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO, &(info->local), 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO, &(info->peer), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

