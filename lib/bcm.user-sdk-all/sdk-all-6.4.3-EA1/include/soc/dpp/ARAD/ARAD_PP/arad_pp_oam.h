/* $Id: arad_pp_oam.h,v 1.27 Broadcom SDK $
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

#ifndef __ARAD_PP_OAM_INCLUDED__
/* { */
#define __ARAD_PP_OAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_oam.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

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

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

#define ARAD_PP_SW_DB_OAM_MY_MAC_LSB_REF_COUNT_NUM 256*256 /*number of ports * number of lsbs*/

typedef struct
{
  uint16 ref_count[ARAD_PP_SW_DB_OAM_MY_MAC_LSB_REF_COUNT_NUM]; 
} ARAD_PP_SW_DB_OAM_MY_MAC_LSB_REF_COUNT;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_OAM_INIT = ARAD_PP_PROC_DESC_BASE_OAM_FIRST,
  ARAD_PP_OAM_INIT_UNSAFE,
  ARAD_PP_OAM_DEINIT,
  ARAD_PP_OAM_DEINIT_UNSAFE,
  ARAD_PP_OAM_ICC_MAP_REGISTER_SET,
  ARAD_PP_OAM_ICC_MAP_REGISTER_SET_UNSAFE,
  ARAD_PP_OAM_ICC_MAP_REGISTER_SET_VERIFY,
  ARAD_PP_OAM_ICC_MAP_REGISTER_GET,
  ARAD_PP_OAM_ICC_MAP_REGISTER_GET_UNSAFE,
  ARAD_PP_OAM_ICC_MAP_REGISTER_GET_VERIFY,
  ARAD_PP_OAM_MY_CFM_MAC_SET,
  ARAD_PP_OAM_MY_CFM_MAC_DELETE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_UNSAFE,  
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_UNSAFE, 
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_SET,
  ARAD_PP_OAM_CLASSIFIER_MEP_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_SET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_MEP_DELETE,
  ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_MEP_GET,
  ARAD_PP_OAM_CLASSIFIER_MEP_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_GET_VERIFY,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_VERIFY,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_VERIFY,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_SET,
  ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_RMEP_SET_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_GET,
  ARAD_PP_OAM_OAMP_RMEP_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_RMEP_GET_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_DELETE,
  ARAD_PP_OAM_OAMP_RMEP_DELETE_UNSAFE,
  ARAD_PP_OAM_OAMP_RMEP_DELETE_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_INDEX_GET,
  ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL,
  ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_VERIFY,
  ARAD_PP_OAM_OAMP_INIT_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_INIT_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_INTERNAL_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_INTERNAL_UNSAFE,
  ARAD_PP_OAM_COUNTER_RANGE_SET_UNSAFE,
  ARAD_PP_OAM_COUNTER_RANGE_SET_VERIFY,
  ARAD_PP_OAM_COUNTER_RANGE_SET,
  ARAD_PP_OAM_COUNTER_RANGE_GET_UNSAFE,
  ARAD_PP_OAM_COUNTER_RANGE_GET_VERIFY,
  ARAD_PP_OAM_COUNTER_RANGE_GET,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET,
  ARAD_PP_OAM_BFD_TX_RATE_SET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_RATE_SET_VERIFY,
  ARAD_PP_OAM_BFD_TX_RATE_SET,
  ARAD_PP_OAM_BFD_TX_RATE_GET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_RATE_GET_VERIFY,
  ARAD_PP_OAM_BFD_TX_RATE_GET,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_UNSAFE,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_VERIFY,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_UNSAFE,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_VERIFY,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_UNSAFE,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_VERIFY,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_SET,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_UNSAFE,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_VERIFY,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_GET,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_VERIFY,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_VERIFY,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_UNSAFE,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_VERIFY,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_UNSAFE,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_VERIFY,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_UNSAFE,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_VERIFY,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_UNSAFE,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_VERIFY,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_UNSAFE,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_VERIFY,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_UNSAFE,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_VERIFY,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_VERIFY,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET,
  ARAD_PP_OAM_BFD_TX_MPLS_SET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_MPLS_SET_VERIFY,
  ARAD_PP_OAM_BFD_TX_MPLS_SET,
  ARAD_PP_OAM_BFD_TX_MPLS_GET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_MPLS_GET_VERIFY,
  ARAD_PP_OAM_BFD_TX_MPLS_GET,
  ARAD_PP_OAM_BFDCC_TX_MPLS_SET_UNSAFE,
  ARAD_PP_OAM_BFDCC_TX_MPLS_SET_VERIFY,
  ARAD_PP_OAM_BFDCC_TX_MPLS_SET,
  ARAD_PP_OAM_BFDCC_TX_MPLS_GET_UNSAFE,
  ARAD_PP_OAM_BFDCC_TX_MPLS_GET_VERIFY,
  ARAD_PP_OAM_BFDCC_TX_MPLS_GET,
  ARAD_PP_OAM_BFDCV_TX_MPLS_SET_UNSAFE,
  ARAD_PP_OAM_BFDCV_TX_MPLS_SET_VERIFY,
  ARAD_PP_OAM_BFDCV_TX_MPLS_SET,
  ARAD_PP_OAM_BFDCV_TX_MPLS_GET_UNSAFE,
  ARAD_PP_OAM_BFDCV_TX_MPLS_GET_VERIFY,
  ARAD_PP_OAM_BFDCV_TX_MPLS_GET,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_VERIFY,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_VERIFY,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_VERIFY,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_VERIFY,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET,
  ARAD_PP_OAM_EVENT_FIFO_READ_UNSAFE,
  ARAD_PP_OAM_EVENT_FIFO_READ_VERIFY,
  ARAD_PP_OAM_EVENT_FIFO_READ,
  ARAD_PP_OAM_PP_PCT_PROFILE_SET_UNSAFE,
  ARAD_PP_OAM_PP_PCT_PROFILE_SET_VERIFY,
  ARAD_PP_OAM_PP_PCT_PROFILE_SET,
  ARAD_PP_OAM_PP_PCT_PROFILE_GET_UNSAFE,
  ARAD_PP_OAM_PP_PCT_PROFILE_GET_VERIFY,
  ARAD_PP_OAM_PP_PCT_PROFILE_GET,

  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_OAM_PROCEDURE_DESC_LAST
} ARAD_PP_OAM_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR = ARAD_PP_ERR_DESC_BASE_OAM_FIRST,
  ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR,
  ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR,
  ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR,
  ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR,
  ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR,
  ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_EM_FULL_ERR,
  ARAD_PP_OAM_EM_INTERNAL_ERR,
  ARAD_PP_OAM_EM_INSERTED_EXISTING_ERR,
  ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_FORWARDING_SRTENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_ACC_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR,
  ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR,
  ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR,
  ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR,
  ARAD_PP_OAM_INTERNAL_ERROR,

  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_OAM_ERR_LAST
} ARAD_PP_OAM_ERR;

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
  
/*********************************************************************
* NAME:
 *   arad_pp_oam_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_oam module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_oam_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_oam_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_oam module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_oam_get_errs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_oam_init_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Init OAM
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO init_trap_info_oam -
 *     Initial TRAP id info for OAM.
 *   SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO init_trap_info_bfd -
 *     Initial TRAP id info for BFD. 
 *	 SOC_SAND_IN  uint8                    is_bfd -
 *     Init OAM or BFD indication
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_init_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO * init_trap_info_oam,
	SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO * init_trap_info_bfd,
	SOC_SAND_IN  uint8                                  is_bfd
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_deinit_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Init OAM
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *	 SOC_SAND_IN  uint8                    is_bfd -
 *     Init OAM or BFD indication 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_deinit_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  is_bfd,
    SOC_SAND_IN  uint8                                  tcam_db_destroy
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_icc_map_register_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Icc Map Register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                      icc_ndx -
 *     Index of ICC register to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA     * data -
 *     Data to write to register. 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_icc_map_register_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA     * data
  );

uint32
  arad_pp_oam_icc_map_register_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA     * data
  );

uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
	SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map,
    SOC_SAND_IN  uint8                            is_bfd
  );

uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
	SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map,
    SOC_SAND_IN  uint8                            is_bfd
  );

uint32
  arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data_acc,
	SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map
  );

uint32
  arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data_acc,
	SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map
  );

uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  );

uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  );

uint32
  arad_pp_oam_classifier_oem_mep_add_unsafe(
    SOC_SAND_IN  int                   unit,
  	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry,
	SOC_SAND_IN  uint8                    update
  );

uint32
  arad_pp_oam_classifier_oem_mep_add_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry
  );

uint32
  arad_pp_oam_classifier_mep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );

uint32
  arad_pp_oam_classifier_mep_delete_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_set_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
	  SOC_SAND_IN  uint8                    allocate_icc_ndx,
	  SOC_SAND_IN  SOC_PPD_OAM_MA_NAME      name
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_set_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
	  SOC_SAND_IN  uint8                    allocate_icc_ndx,
	  SOC_SAND_IN  SOC_PPD_OAM_MA_NAME      name
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_delete_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	  SOC_SAND_IN  uint8                    is_last_mep
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_delete_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	  SOC_SAND_IN  uint8                    is_last_mep
  );

uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
    SOC_SAND_IN  uint8                    md_level,
    SOC_SAND_IN  uint8                    is_upmep
  );

uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
	SOC_SAND_IN  uint8                    md_level,
	SOC_SAND_IN  uint8                    is_upmep,
	SOC_SAND_OUT uint8                    *found_mep,
	SOC_SAND_OUT uint32                   *profile,
	SOC_SAND_OUT uint8                    *found_profile,
	SOC_SAND_OUT uint8                    *is_mp_type_flexible
  );

uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 your_discriminator,
    SOC_SAND_IN  uint32                                 in_lif,
    SOC_SAND_OUT  uint32*                               tcam_entry
  );

uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 your_discriminator,
    SOC_SAND_IN  uint32                                 in_lif,
    SOC_SAND_OUT  uint32*                               tcam_entry
  );

uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_delete_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 tcam_entry
  );

uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_delete_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 your_discriminator
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_get_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_get_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_set_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
  SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
	SOC_SAND_IN  uint8                    update
  );

uint32
  arad_pp_oam_oamp_rmep_set_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_get_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_get_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  );

uint32
  arad_pp_oam_oamp_rmep_delete_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  );

uint32
  arad_pp_oam_oamp_rmep_index_get_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  );

uint32
  arad_pp_oam_oamp_rmep_index_get_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
	SOC_SAND_OUT uint32                   *rmep_index,
	SOC_SAND_OUT  uint8                   *is_found
  );

uint32
  arad_pp_oam_classifier_oem1_entry_set_unsafe(
    SOC_SAND_IN   int                                     unit,
	  SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	  SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  );

uint32
  arad_pp_oam_classifier_oem1_entry_get_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );

uint32
  arad_pp_oam_classifier_oem1_entry_delete_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key
  );

uint32
  arad_pp_oam_classifier_oem2_entry_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   *oem2_payload
  );

uint32
  arad_pp_oam_classifier_oem2_entry_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );

uint32
  arad_pp_oam_classifier_oem2_entry_delete_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key
  );

uint32
  arad_pp_oam_counter_range_set_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  );

uint32
  arad_pp_oam_counter_range_set_verify(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  );

uint32
  arad_pp_oam_counter_range_get_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_OUT  uint32                                     *counter_range_min,
	SOC_SAND_OUT  uint32                                     *counter_range_max
  );

uint32
  arad_pp_oam_counter_range_get_verify(
    SOC_SAND_IN   int                                     unit
  );

uint32
  arad_pp_oam_event_fifo_read_unsafe(
    SOC_SAND_IN  int                                        unit,
	SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
	SOC_SAND_OUT  uint32                                       *event_id,
	SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *rmep_state 
  );

uint32
  arad_pp_oam_event_fifo_read_verify(
    SOC_SAND_IN  int                                        unit
  );

uint32
  arad_pp_oam_pp_pct_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  );

uint32
  arad_pp_oam_pp_pct_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  );

uint32
  arad_pp_oam_pp_pct_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx
  );

uint32
  arad_pp_oam_pp_pct_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                      *diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx
  );

/*********************************************************************
*     Set Bfd Flags Profile register
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_oam_bfd_flags_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                      *flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_eth_oam_opcode_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure mapping of network opcode to internal opcode.
 *   The information is taken from WB variables.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_eth_oam_opcode_map_set_unsafe(
    SOC_SAND_IN   int                                     unit
  );

/*
 * ARAD BFD APIs
*/

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_ipv4_tos_ttl_select_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Ipv4 Tos Ttl Select register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   ipv4_tos_ttl_select_index -
 *     Entry index.
 *   SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA      *tos_ttl_data -
 *     Data struct
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_OUT SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_ipv4_src_addr_select_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Ipv4 Src Addr Select register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   ipv4_src_addr_select_index -
 *     Entry index.
 *   SOC_SAND_IN  uint32                  src_addr -
 *     Data - src address.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_OUT uint32                                       *src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_tx_rate_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Tx Rate register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   bfd_tx_rate_index -
 *     Entry index.
 *   SOC_SAND_IN  uint32                  tx_rate -
 *     TX rate data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_tx_rate_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_OUT uint32                                       *tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_req_interval_pointer_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Req Interval Pointer register (MPLS Push profile)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   req_interval_pointer -
 *     Entry index.
 *   SOC_SAND_IN  uint32                  req_interval -
 *     Rate data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_req_interval_pointer_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_OUT uint32                                       *req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_req_interval_pointer_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Req Interval Pointer register (MPLS Push profile)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   push_profile -
 *     Entry index.
 *   SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA *push_data -
 *     Push profile data struct.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_mpls_pwe_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_OUT SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_mpls_udp_sport_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd MPLS UDP Sport register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   udp_sport -
 *     register data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT uint16                                       *udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_ipv4_udp_sport_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd IPV4 UDP Sport register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   udp_sport -
 *     register data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );


uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint16                                      *udp_sport
  );

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit

  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_pdu_static_register_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd pdu static register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER  bfd_pdu -
 *     register data struct.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_pdu_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_cc_packet_pdu_static_register_set / get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd pdu static register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER  bfd_pdu -
 *     register data struct.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_discriminator_range_registers_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd pdu static register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint16                   range -
 *     range of your discriminator
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint32  						              *range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_verify(
    SOC_SAND_IN  int                                       unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_my_bfd_dip_ipv4_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set My Bfd Dip table with IPv4 values register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   dip_index -
 *     Entry index
 *	SOC_SAND_IN  uint32						dip	 - 
 *     IPv4 address
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_my_bfd_dip_ipv4_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV4_ADDRESS					dip							
  );

uint32
  arad_pp_oam_bfd_my_bfd_dip_ipv4_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV4_ADDRESS					dip		
  );


uint32
  arad_pp_oam_bfd_my_bfd_dip_ipv4_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_OUT SOC_SAND_PP_IPV4_ADDRESS					*dip								
  );

uint32
  arad_pp_oam_bfd_my_bfd_dip_ipv4_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16							              dip_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Tx Ipv4 Multi Hop register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES  *tx_ipv4_multi_hop_att -
 *     Register values
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES               *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify(
    SOC_SAND_IN  int                                       unit
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_tx_priority_registers_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   SSet OAMP priority TC and DP registers
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    priority -
 *     Priority profile (0-7)
 *   SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES    *tx_oam_att -
 *     Register values
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_tx_priority_registers_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                     	                priority,
	  SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                       	              priority,
	  SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                       	              priority,
	  SOC_SAND_OUT  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_verify(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                     	                priority
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_enable_interrupt_message_event_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set oamp_enable_interrupt_message_event register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    interrupt_message_event_bmp -
 *     Bitmap of the events to set
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_verify(
    SOC_SAND_IN  int                                       unit
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint8                                        *interrupt_message_event_bmp
  );

/* ARAD+ functions */
/*********************************************************************
* NAME:
 *   arad_pp_oam_mep_passive_active_enable_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set oam_mep_passive_active_enable register - MP_type of each profile
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   profile_ndx -
 *     Profile index
 *   SOC_SAND_IN  uint8                    enable -
 *     Value to setin the enable register
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_mep_passive_active_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_oam_mep_passive_active_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_oam_mep_passive_active_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  );

uint32
  arad_pp_oam_mep_passive_active_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_punt_event_hendling_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set oamp_punt_event_hendling register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   profile_ndx -
 *     Profile index
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data -
 *     Profile fields
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_error_trap_id_and_destination_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set PORT2CPU OAMP register fields trap id and system port with the given
 *   values according to the error type.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type -
 *     Type of the error to set
 *   SOC_SAND_IN  uint32                                 trap_id -
 *     Id 0-255 to put in trap id field
 *   SOC_SAND_IN  uint32                                 dest_system_port -
 *     20bit system port to put in port field
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  uint32                                 dest_system_port
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  uint32                                 dest_system_port
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  uint32                                *dest_system_port
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_lm_dm_set/get
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Manage LM and DM, e.g add MEP DB entry if possible.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY     *mep_db_entry -
 *     mep db fields
 *   SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
 * REMARKS:
 *   Used for Arad+ only.
 *   Functions are shared by LM and DM due to the similarity in the implementation of the two functionalities.
 * lm_dm_set also maintains proper oredring. That is LM entries must always be before DM.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_lm_dm_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_lm_dm_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_lm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_LM_INFO_GET     *lm_info
  );

uint32
  arad_pp_oam_oamp_lm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_LM_INFO_GET     *lm_info
  );

uint32
  arad_pp_oam_oamp_dm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  );

uint32
  arad_pp_oam_oamp_dm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_next_index_get, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *   SOC_SAND_OUT uint32                               *next_index
 * Next available index for additional LM/DM entries. returns 0 if none available.
 * REMARKS:
 *   Used for Arad+.
 *   Function is used to create LM and DM entries. Function changes nothing in the HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_next_index_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  );
uint32
  arad_pp_oam_oamp_next_index_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_eth1731_and_oui_profiles_get, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *   SOC_SAND_IN  uint8                          remove_mode,
 *  SOC_SAND_OUT uint32                               *eth1731_prof
 *  SOC_SAND_OUT uint32                               *da_oui_prof
 *      Profiles associated with mep.
 * REMARKS:
 *   Used for Arad+. Function returns both because the oui profile cannot be recovered without
 * First finding the eth1731 profile.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  );

uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_nic_profile_get, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *  SOC_SAND_OUT uint32                               *da_nic_prof
 *      Profile associated with mep.
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
    arad_pp_oam_oamp_nic_profile_get_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       );

uint32
    arad_pp_oam_oamp_nic_profile_get_unsafe(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_search_for_lm_dm, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *   SOC_SAND_IN  uint8                          search_mode,
 *   1 to find out if a DM entry exists,  0 to find out if a LM entry exists, 2 for either one.
 *   SOC_SAND_OUT uint32                               * found_bitmap,
 *    For every mep type, SOC_PPC_OAM_MEP_TYPE_XXX,  (found_bitmap &  SOC_PPC_OAM_MEP_TYPE_XXX)
 * iff such an entry is associated with the given mep.
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
    arad_pp_oam_oamp_search_for_lm_dm_unsafe(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               * found_bitmap
       );

uint32
    arad_pp_oam_oamp_search_for_lm_dm_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               * found_bitmap
       );



/*********************************************************************
* NAME:
 *   soc_ppd_oam_oamp_create_new_eth1731_profile, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
 *      
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );

uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );

/*********************************************************************
* NAME:
 *   soc_ppd_oam_oamp_set_oui_nic_registers, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage OUI, NIC registers (MAC DA addresses)
 * INPUT:
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
 *      
 * REMARKS:
 *   Used for Arad+.
 *  OUI profile should be written seperately by the create_new_eth1731_profile() function.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_set_oui_nic_registers_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  );

uint32
  arad_pp_oam_oamp_set_oui_nic_registers_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_lm_dm_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *     SOC_SAND_IN uint32                                        endpoint_id -
 * Entry number of the endpoint.
 *   SOC_SAND_IN  uint8                          is_lm,
 *   1 if lm needs removing, 0 otherwise
 *   SOC_SAND_OUT uint8                               * num_removed,
 *    2 if LM + LM-STAT were removed, 1 if only an LM/DM was removed, 0 if none were removed.
 *   SOC_SAND_OUT uint32                              * removed_index
 * The index thatwas freed.
 * REMARKS:
 *   Used for Arad+.
 *   Functions are shared by LM and DM due to the similarity in the implementation of the two functionalities.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/

uint32
  arad_pp_oam_oamp_lm_dm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  );

uint32
  arad_pp_oam_oamp_lm_dm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  );


/**
 * Set/Get a given MEP to transmitt 1DM packets, as opposed to 
 *  DMMs, or vica-versa.
 *  Done via the MEP PE profile field in the endpoint's MEP DB
 *  entry.
 * 
 * @author sinai (08/07/2014)
 * 
 * @param unit 
 * @param endpoint_id -mep_db index
 * @param use_1dm - 1 iff 1dm is to be used.
 * 
 * @return soc_error_t 
 */
soc_error_t arad_pp_oam_oamp_1dm_set(
   int unit,
   int endpoint_id,
   uint8 use_1dm);

soc_error_t arad_pp_oam_oamp_1dm_get(
   int unit,
   int endpoint_id,
   uint8 *use_1dm);


  
/*********************************************************************
* NAME:
 *    arad_pp__oam_oamp_loopback_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Start an OAM Loopback session
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *     SOC_SAND_IN uint32                                        endpoint_id -
 * Entry number of the endpoint.
 *      SOC_SAND_IN  uint8                                                      tx_period,
 *   Tx period (same "units" as in CCMs)
 *     SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                mac_address,
 * DA MAC address of ioutgoing LBMs
 *      SOC_SAND_OUT uint8                                                   *is_added
 * 1 IF the loopback session was added, 0 if loopback session is used by other mep.
 * REMARKS:
 *   Used for Arad+.
 *   Only one loopback session may be used. If a session is already in use then is_added returns 0 and no further changes are made.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_set_unsafe(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_IN uint32                                                     endpoint_id,
       SOC_SAND_IN  uint8                                                      tx_period,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *mac_address,
       SOC_SAND_OUT uint8                                                   *is_added
       );

uint32 
    arad_pp_oam_oamp_loopback_set_verify(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_IN uint32                                                     endpoint_id,
       SOC_SAND_IN  uint8                                                      tx_period,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *mac_address,
       SOC_SAND_OUT uint8                                                   *is_added
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Start an OAM Loopback session
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *      SOC_SAND_OUT uint32                                              *packet_count,
 *     SOC_SAND_OUT uint32                                              *discard_count,
 *     SOC_SAND_OUT uint32                                              *fifo_count,
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_get_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *rx_packet_count,
       SOC_SAND_OUT uint32                                              *tx_packet_count,
       SOC_SAND_OUT uint32                                              *discard_count,
       SOC_SAND_OUT uint32                                              *fifo_count
       );

uint32 
    arad_pp_oam_oamp_loopback_get_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *rx_packet_count,
       SOC_SAND_OUT uint32                                              *tx_packet_count,
       SOC_SAND_OUT uint32                                              *discard_count,
       SOC_SAND_OUT uint32                                              *fifo_count
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_get_period
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Return period of LB session (in miliseconds)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
       SOC_SAND_OUT uint32                                              *period
 * REMARKS:
 *   Used for Arad+. Probably will be used only in one place.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_get_period_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       );
uint32 
    arad_pp_oam_oamp_loopback_get_period_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_get_period
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Return period of LB session (in miliseconds)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
       SOC_SAND_OUT uint32                                              *period
 * REMARKS:
 *   Used for Arad+. Probably will be used only in one place.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    soc_ppd_oam_oamp_loopback_get_period_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       );

uint32 
    soc_ppd_oam_oamp_loopback_get_period_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *    End an OAM Loopback session
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_remove_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
    arad_pp_oam_oamp_loopback_remove_unsafe(
       SOC_SAND_IN  int                                                  unit
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_dma_reset
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+. To be used after WB.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_dma_reset_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
    arad_pp_oam_dma_reset_unsafe(
       SOC_SAND_IN  int                                                  unit
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_dma_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_dma_clear_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
    arad_pp_oam_dma_clear_unsafe(
       SOC_SAND_IN  int                                                  unit
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_register_dma_event_handler_callback
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN        int (*event_handler_cb)(int)
 Function called upon DMA interrupt. Paramater is declared as SOC_SAND_INOUT as opposed to SOC_SAND_IN
 for compilation. * REMARKS:
 *   Used for Arad+. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_register_dma_event_handler_callback_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        int (*event_handler_cb)(int)
       );

uint32 
    arad_pp_oam_register_dma_event_handler_callback_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        int (*event_handler_cb)(int)
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_rx_trap_codes_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Configure OAMP supported trap codes per endpoint type
 * INPUT:
 *   SOC_SAND_IN  int                   unit
 *   SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE  mep_type -
 *                 Endpoint type that is associated with the given trap cpde 
 *    SOC_SAND_IN  uint32               trap_code -
 *                 Trap code that will be recognized by the OAMP as valid trap code.
 * REMARKS:
 *   Used for Arad+ only. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_rx_trap_codes_set_unsafe(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       );

uint32 
    arad_pp_oam_oamp_rx_trap_codes_set_verify(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       );

uint32 
    arad_pp_oam_oamp_rx_trap_codes_delete_unsafe(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       );

uint32 
    arad_pp_oam_oamp_rx_trap_codes_delete_verify(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_dma_event_handler
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+.  To be called when DMA related interrupt is triggered.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
   arad_pp_oam_dma_event_handler_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
   arad_pp_oam_dma_event_handler_unsafe(
       SOC_SAND_IN  int                                                  unit
       );


/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_counter_disable_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set classifier counter disable map.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                  packet_is_oam -
 *     Bit to signal OAM/Data packets setting
 *   SOC_SAND_IN  uint8                                  profile -
 *     OAMA MP-Profile to set 
 *   SOC_SAND_IN  uint8                                  counter_enable -
 *     Value of the map - counter enable/disable
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_counter_disable_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_lookup
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print lookup information from OAM exact matches.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   Used for oam diagnostics
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_lookup_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_lookup_unsafe(
     SOC_SAND_IN int unit
   );

/*********************************************************************
* NAME:
 *   arad_pp_get_crps_counter
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get value from crps crps counter register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    crps_counter_number -
 *     Number of the crps crps counter to display. Presumed to be 0,1,2 or 3.
 *   SOC_SAND_IN uint32          reg_number -
 *  Register number. Presumed to be between 0 and (32K-1).
 *     SOC_SAND_OUT uint64*           value
 *   Result goes here.
 * REMARKS:
 *   Used for oam diagnostics
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_get_crps_counter_verify(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      );

uint32
  arad_pp_get_crps_counter_unsafe(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      );

/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_rx
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oam rx information.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_rx_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_rx_unsafe(
     SOC_SAND_IN int unit
   );

/*********************************************************************
* NAME:
 *   arad_pp_oam_my_cfm_mac_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Given a port and a mac address, adds the entry to the my cfm mac table.
 * INPUT:
 *   int                        unit            - (IN) Device to be configured.
 *   SOC_SAND_PP_MAC_ADDRESS    dst_mac_address - (IN) Mac address to be configured to this port.
 *   uint32                     table_index     - (IN) Port to be configured.
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t 
    arad_pp_oam_my_cfm_mac_set(int unit, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

/*********************************************************************
* NAME:
 *   arad_pp_oam_my_cfm_mac_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Given a port and an LSB, returns the MSB assigned to this port. 
 * INPUT:
 *   int                        unit            - (IN) Device to be checked.
 *   SOC_SAND_PP_MAC_ADDRESS    dst_mac_address - (INOUT) Fill the LSB, will be filled with the MSB.
 *   uint32                     table_index     - (IN) Configured port.
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
  arad_pp_oam_my_cfm_mac_get(int unit, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);



/*********************************************************************
* NAME:
 *   arad_pp_oam_my_cfm_mac_delete
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Given a port and a mac address, removes one count from the {table_index, mac_msb, mac_lsb} combination. If all counts were removed from the
 *  combination, the lsb will be deleted. If all mappings were removed from {table_index, mac_msb}, the mac_msb will be deleted as well.
 * INPUT:
 *   int                        unit            - (IN) Device to be configured.
 *   SOC_SAND_PP_MAC_ADDRESS    dst_mac_address - (IN) Mac address to be configured to this port.
 *   uint32                     table_index     - (IN) Port to be configured.
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
  arad_pp_oam_my_cfm_mac_delete(int unit, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_OAM_INCLUDED__*/
#endif
