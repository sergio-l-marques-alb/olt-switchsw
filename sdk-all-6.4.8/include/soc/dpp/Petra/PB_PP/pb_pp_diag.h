/* $Id: pb_pp_diag.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_diag.h
*
* MODULE PREFIX:  soc_pb_pp
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PB_PP_DIAG_INCLUDED__
/* { */
#define __SOC_PB_PP_DIAG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_diag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/petra_chip_defines.h>

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
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Register address of the register this field belongs to.
   */
  SOC_PETRA_REG_ADDR addr;
  /*
   *  Field Most Significant Bit in the register.
   */
  uint32 msb;
  /*
   *  Field Least Significant Bit in the register.
   */
  uint32 lsb;

} SOC_PB_PP_DIAG_REG_FIELD;



typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET = SOC_PB_PP_PROC_DESC_BASE_DIAG_FIRST,
  SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE,
  SOC_PB_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY,
  SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET,
  SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY,
  SOC_PB_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE,
  SOC_PB_PP_DIAG_MODE_INFO_SET,
  SOC_PB_PP_DIAG_MODE_INFO_SET_UNSAFE,
  SOC_PB_PP_DIAG_MODE_INFO_SET_VERIFY,
  SOC_PB_PP_DIAG_MODE_INFO_GET,
  SOC_PB_PP_DIAG_MODE_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_MODE_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_PKT_TRACE_CLEAR,
  SOC_PB_PP_DIAG_PKT_TRACE_CLEAR_UNSAFE,
  SOC_PB_PP_DIAG_PKT_TRACE_CLEAR_VERIFY,
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET,
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_PARSING_INFO_GET,
  SOC_PB_PP_DIAG_PARSING_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_PARSING_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_TERMINATION_INFO_GET,
  SOC_PB_PP_DIAG_TERMINATION_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_TERMINATION_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET,
  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET,
  SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE,
  SOC_PB_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY,
  SOC_PB_PP_DIAG_TRAPS_INFO_GET,
  SOC_PB_PP_DIAG_TRAPS_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_TRAPS_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_TRAPPED_PACKET_INFO_GET,
  SOC_PB_PP_DIAG_TRAPPED_PACKET_INFO_GET_PRINT,
  SOC_PB_PP_DIAG_TRAPPED_PACKET_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_TRAPPED_PACKET_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU,
  SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU_UNSAFE,
  SOC_PB_PP_DIAG_TRAPS_ALL_TO_CPU_VERIFY,
  SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE,
  SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE_UNSAFE,
  SOC_PB_PP_DIAG_TRAPS_STAT_RESTORE_VERIFY,
  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET,
  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE,
  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY,
  SOC_PB_PP_DIAG_LEARNING_INFO_GET,
  SOC_PB_PP_DIAG_LEARNING_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_LEARNING_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET,
  SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET,
  SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_ENCAP_INFO_GET,
  SOC_PB_PP_DIAG_ENCAP_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_ENCAP_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_EG_DROP_LOG_GET,
  SOC_PB_PP_DIAG_EG_DROP_LOG_GET_UNSAFE,
  SOC_PB_PP_DIAG_EG_DROP_LOG_GET_VERIFY,
  SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET,
  SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET,
  SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET,
  SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_DB_TCAM_LKUP_INFO_GET_VERIFY,
  SOC_PB_PP_DIAG_PKT_SEND,
  SOC_PB_PP_DIAG_PKT_SEND_UNSAFE,
  SOC_PB_PP_DIAG_PKT_SEND_VERIFY,
  SOC_PB_PP_DIAG_GET_PROCS_PTR,
  SOC_PB_PP_DIAG_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_DIAG_DBG_VAL_GET_UNSAFE,
  SOC_PB_PP_DIAG_LIF_DB_ID_TO_DB_TYPE_MAP_GET,
  SOC_PB_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE,
  SOC_PB_PP_DIAG_PKT_ETH_HEADER_BUILD,
  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_DIAG_PROCEDURE_DESC_LAST
} SOC_PB_PP_DIAG_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_DIAG_FIRST,
  SOC_PB_PP_DIAG_MAX_SIZE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_VALID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_BUFF_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_FLAVOR_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_PP_CONTEXT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_CODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_IP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_VRF_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_METER1_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_METER2_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_CUD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_EEP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */


  SOC_PB_PP_DIAG_RESTORE_NOT_SAVED_ERR,
  SOC_PB_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR,
  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_DIAG_ERR_LAST
} SOC_PB_PP_DIAG_ERR;

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

/* length of debug value */
#define SOC_PB_PP_DIAG_DBG_VAL_LEN        (24)

uint32
  soc_pb_pp_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32               blk,
    SOC_SAND_IN  SOC_PB_PP_DIAG_REG_FIELD   *fld,
    SOC_SAND_OUT uint32               val[SOC_PB_PP_DIAG_DBG_VAL_LEN]
);

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_sample_enable_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable diagnostic APIs.affects only APIs with
 *   type: need_sample
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                 enable -
 *     TRUE: diag APIs are enabled, FALSE diag APIs are
 *     disabled.
 * REMARKS:
 *   - when enabled will affect device power consuming
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  soc_pb_pp_diag_sample_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  soc_pb_pp_diag_sample_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_diag_sample_enable_set_unsafe" API.
 *     Refer to "soc_pb_pp_diag_sample_enable_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_mode_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mode configuration for diag module, including
 *   diag-flavor
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO                      *mode_info -
 *     Mode of diagnsotcis
 * REMARKS:
 *   Diag Type: All-Packets
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO                      *mode_info
  );

uint32
  soc_pb_pp_diag_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO                      *mode_info
  );

uint32
  soc_pb_pp_diag_mode_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_diag_mode_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_diag_mode_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_diag_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_MODE_INFO                      *mode_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_pkt_trace_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the trace of transmitted packet, so next trace
 *   info will relate to next packets to transmit
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  pkt_trace -
 *     Packet traces type as encoded by SOC_PPD_DIAG_PKT_TRACE.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_pkt_trace_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  );

uint32
  soc_pb_pp_diag_pkt_trace_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_received_packet_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the header of last received packet entered the
 *   device and the its association to TM/system/PP ports.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info -
 *     Information regards Last received packet
 * REMARKS:
 *   Diag Type: Last-Packet, need-sampleif sample is disabled
 *   then: last-packet, clear-on-read
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_received_packet_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info
  );

uint32
  soc_pb_pp_diag_received_packet_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_parsing_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns network headers qualified on packet upon
 *   parsing, including packet format, L2 headers fields,...
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_PARSING_INFO                   *pars_info -
 *     Information obtained from parsing including L2 headers,
 *     packet format,...
 * REMARKS:
 *   Diag Type: Last Packet, need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_parsing_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_PARSING_INFO                   *pars_info
  );

uint32
  soc_pb_pp_diag_parsing_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_termination_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns information obtained by termination including
 *   terminated headers
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_TERM_INFO                      *term_info -
 *     Includes terminated headers,
 * REMARKS:
 *   Diag Type: Last Packet, need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_termination_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TERM_INFO                      *term_info
  );

uint32
  soc_pb_pp_diag_termination_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_frwrd_lkup_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the forwarding lookup performed including:
 *   forwarding type (bridging, routing, ILM, ...), the key
 *   used for the lookup and the result of the lookup
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_FRWRD_LKUP_INFO                *frwrd_info -
 *     forwarding lookup information including key and result
 * REMARKS:
 *   Diag Type: Last Packet, need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_frwrd_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_FRWRD_LKUP_INFO                *frwrd_info
  );

uint32
  soc_pb_pp_diag_frwrd_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_frwrd_lpm_lkup_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Simulate IP lookup in the device tables and return
 *   FEC-pointer
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY              *lpm_key -
 *     forwarding lookup information including key and result
 *   SOC_SAND_OUT uint32                             *fec_ptr -
 *     FEC pointer
 *   SOC_SAND_OUT uint8                            *found -
 *     Was key found
 * REMARKS:
 *   Diag Type: lookup a key, don't need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_frwrd_lpm_lkup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found
  );

uint32
  soc_pb_pp_diag_frwrd_lpm_lkup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY              *lpm_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_traps_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns information regard packet trapping/snooping,
 *   including which traps/snoops were fulfilled, which
 *   trap/snoop was committed, and whether packet was
 *   forwarded/processed according to trap or according to
 *   normal packet processing flow.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_TRAPS_INFO                     *traps_info -
 *     Information regarding the trapping
 * REMARKS:
 *   Diag Type: Last Packet, Clear-on-read. When called after
 *   injecting more than one packet then 'trap_stack' will
 *   hold the status for all injected packets from last call.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_traps_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TRAPS_INFO                     *traps_info
  );

uint32
  soc_pb_pp_diag_traps_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_trapped_packet_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns packet trapped to CPU with additional
 *   information: identifying the reason of trapping (if
 *   any), the source system port, and pointer to the packet
 *   payload.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT uint8                              *buff -
 *     Buffer to include the packet, has to be allocated by
 *     caller.
 *   SOC_SAND_IN  uint32                             max_size -
 *     Maximum number of bytes to read.
 *   SOC_SAND_OUT uint32                             *buff_len -
 *     Number of return bytes in buff
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_TRAP_PACKET_INFO          *packet_info -
 *     Information retrieved by parsing the packet.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_trapped_packet_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT uint8                              *buff,
    SOC_SAND_IN  uint32                             max_size,
    SOC_SAND_OUT uint32                             *buff_len,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TRAP_PACKET_INFO          *packet_info
  );

uint32
  soc_pb_pp_diag_trapped_packet_info_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT uint8                              *buff,
    SOC_SAND_IN  uint32                             max_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_traps_all_to_cpu_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the destination of all traps to be CPU, so trapped
 *   packet can be captured and examined by CPU
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO               *trap_info -
 *     Information needed to configure trap destination as CPU,
 *     e.g. CPU destination.
 * REMARKS:
 *   Diag Type: any Packets.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_traps_all_to_cpu_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO               *trap_info
  );

uint32
  soc_pb_pp_diag_traps_all_to_cpu_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO               *trap_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_traps_stat_restore_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Restore the configuration of all traps, to be as
 *   configured before last calling for
 *   soc_ppd_diag_traps_all_to_cpu()
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   Diag Type: any Packet.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_traps_stat_restore_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_diag_traps_stat_restore_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_frwrd_decision_trace_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Return the trace (changes) for forwarding decision for
 *   last packet in several phases in processing
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info -
 *     Forwarding decision in several phases in the processing
 * REMARKS:
 *   Diag Type: Last Packet, Need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_frwrd_decision_trace_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info
  );

uint32
  soc_pb_pp_diag_frwrd_decision_trace_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_learning_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the information to be learned for the incoming
 *   packet. This is the information that the processing
 *   determine to be learned, the MACT supposed to learn this
 *   information.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_LEARN_INFO                     *learn_info -
 *     Learning information including key and value
 *     <destination and additional info (AC, EEP, MPLS command
 *     etc...)>
 * REMARKS:
 *   Diag Type: Last-Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_learning_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEARN_INFO                     *learn_info
  );

uint32
  soc_pb_pp_diag_learning_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_ing_vlan_edit_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the result of ingress vlan editing,
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_VLAN_EDIT_RES                  *vec_res -
 *     Vlan edit command result, removed tags and build tags
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_ing_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_VLAN_EDIT_RES                  *vec_res
  );

uint32
  soc_pb_pp_diag_ing_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_pkt_associated_tm_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The Traffic management information associated with the
 *   packet including meter, DP, TC, etc...
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_PKT_TM_INFO                    *pkt_tm_info -
 *     Traffic management information associated with the
 *     packet
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_pkt_associated_tm_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_PKT_TM_INFO                    *pkt_tm_info
  );

uint32
  soc_pb_pp_diag_pkt_associated_tm_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_encap_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the encapsulation and editing information applied to
 *   last packet
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_ENCAP_INFO                     *encap_info -
 *     SOC_SAND_OUT SOC_PPD_DIAG_ENCAP_INFO *encap_info
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_encap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_ENCAP_INFO                     *encap_info
  );

uint32
  soc_pb_pp_diag_encap_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_eg_drop_log_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the reason for packet discard
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_EG_DROP_LOG_INFO               *eg_drop_log -
 *     Egress drop log, reason why packets were dropped.
 * REMARKS:
 *   Diag Type: all Packets since last clear.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_eg_drop_log_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  );

uint32
  soc_pb_pp_diag_eg_drop_log_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_db_lif_lkup_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the lookup key and result used in the LIF DB
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info -
 *     The specific use of the DB, for example the lookup
 *     number
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_LIF_LKUP_INFO                  *lkup_info -
 *     Lookup information, key and result
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_db_lif_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LIF_LKUP_INFO                  *lkup_info
  );

uint32
  soc_pb_pp_diag_db_lif_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_db_lem_lkup_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the lookup key and result used in the LEM DB
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info -
 *     The specific use of the DB, for example the lookup
 *     number
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_LKUP_TYPE                  *type -
 *     Lookup type
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_KEY                        *key -
 *     Lookup key
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_VALUE                      *val -
 *     Lookup result
 *   SOC_SAND_OUT uint8                                 *valid -
 *     Is lookup result valid.
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_db_lem_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                                 *valid
  );

uint32
  soc_pb_pp_diag_db_lem_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO                    *db_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_db_tcam_lkup_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the lookup key and result used in the TCAM DB
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO                  *use_info -
 *     The specific use of the DB, for example the lookup
 *     number and lookup Purpose(Forwarding/PMF/Egress ACL)
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_LKUP_TYPE                  *type -
 *     Lookup type
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_TCAM_KEY                       *key -
 *     Lookup key
 *   SOC_SAND_OUT SOC_PB_PP_DIAG_TCAM_VALUE                     *val -
 *     Lookup result
 *   SOC_SAND_OUT uint8                                 *valid -
 *     Is lookup result valid.
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_db_tcam_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO                  *use_info,
    SOC_SAND_OUT SOC_PB_PP_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TCAM_KEY                       *key,
    SOC_SAND_OUT SOC_PB_PP_DIAG_TCAM_VALUE                     *val,
    SOC_SAND_OUT uint8                                 *valid
  );

uint32
  soc_pb_pp_diag_db_tcam_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO                  *use_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_pkt_send_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inject packets from CPU port to device.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO                    *pkt_info -
 *     Packet to send,
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_diag_pkt_send_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO                    *pkt_info
  );

uint32
  soc_pb_pp_diag_pkt_send_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO                    *pkt_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_diag module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_diag_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_diag_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_diag module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_diag_get_errs_ptr(void);

uint32
  SOC_PB_PP_DIAG_BUFFER_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_BUFFER *info
  );

uint32
  SOC_PB_PP_DIAG_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_MODE_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_DB_USE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_DB_USE_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TCAM_USE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_USE_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TRAP_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_PACKET_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TRAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_SNOOP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_SNOOP_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TRAPS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAPS_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TRAP_TO_CPU_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_LEM_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LEM_KEY *info
  );

uint32
  SOC_PB_PP_DIAG_LIF_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LIF_KEY *info
  );

uint32
  SOC_PB_PP_DIAG_LIF_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LIF_VALUE *info
  );

uint32
  SOC_PB_PP_DIAG_LIF_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LIF_LKUP_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TCAM_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_KEY *info
  );

uint32
  SOC_PB_PP_DIAG_TCAM_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TCAM_VALUE *info
  );

uint32
  SOC_PB_PP_DIAG_LEM_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LEM_VALUE *info
  );

uint32
  SOC_PB_PP_DIAG_PARSING_L2_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_L2_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_PARSING_MPLS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_MPLS_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_PARSING_IP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_IP_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_PARSING_HEADER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_HEADER_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_PARSING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PARSING_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TERM_MPLS_LABEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERM_MPLS_LABEL_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TERMINATED_MPLS_LABEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERMINATED_MPLS_LABEL_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TERM_MPLS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERM_MPLS_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_TERM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_TERM_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_IPV4_VPN_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV4_VPN_KEY *info
  );

uint32
  SOC_PB_PP_DIAG_IPV6_VPN_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_IPV6_VPN_KEY *info
  );

uint32
  SOC_PB_PP_DIAG_FRWRD_LKUP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_LKUP_KEY *info
  );

uint32
  SOC_PB_PP_DIAG_FRWRD_LKUP_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_LKUP_VALUE *info
  );

uint32
  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_LKUP_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_LEARN_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_LEARN_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_VLAN_EDIT_RES_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_VLAN_EDIT_RES *info
  );

uint32
  SOC_PB_PP_DIAG_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_DECISION_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_FRWRD_DECISION_TRACE_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_PKT_TM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PKT_TM_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_ENCAP_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_EG_DROP_LOG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_EG_DROP_LOG_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_ETH_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_ETH_PACKET_INFO *info
  );

uint32
  SOC_PB_PP_DIAG_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_DIAG_PACKET_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_DIAG_INCLUDED__*/
#endif

