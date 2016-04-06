/* $Id: pb_pp_llp_parse.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_llp_parse.h
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

#ifndef __SOC_PB_PP_LLP_PARSE_INCLUDED__
/* { */
#define __SOC_PB_PP_LLP_PARSE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_parser.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


/* Vlan compression defined indexes */
#define SOC_PB_PP_LLP_PARSE_VLAN_COMPRESSION_OUTER_COMP_NDX        (0)
#define SOC_PB_PP_LLP_PARSE_VLAN_COMPRESSION_INNER_COMP_NDX        (1)

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
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET = SOC_PB_PP_PROC_DESC_BASE_LLP_PARSE_FIRST,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_PRINT,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_SET_VERIFY,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_PRINT,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_VERIFY,
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_GET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_SET_VERIFY,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_VERIFY,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_GET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_VERIFY,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_VERIFY,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_SET_VERIFY,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_VERIFY,
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_GET_UNSAFE,
  SOC_PB_PP_LLP_PARSE_GET_PROCS_PTR,
  SOC_PB_PP_LLP_PARSE_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_PARSE_PROCEDURE_DESC_LAST
} SOC_PB_PP_LLP_PARSE_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LLP_PARSE_FIRST,
  SOC_PB_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_LLP_PARSE_TYPE_ILLEGAL_ERR,
  SOC_PB_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_PARSE_ERR_LAST
} SOC_PB_PP_LLP_PARSE_ERR;

typedef enum
{
  /*
  *  no TPID was found
  */
  SOC_PB_PP_LLP_PARSE_TAG_FORMAT_NONE = 0,
  /*
  *  TPID1 was found
  */
  SOC_PB_PP_LLP_PARSE_TAG_FORMAT_TPID1 = 1,
  /*
  *  TPID2 was found
  */
  SOC_PB_PP_LLP_PARSE_TAG_FORMAT_TPID2 = 2,
  /*
  *  TPID3 was found (for ITPID)
  */
  SOC_PB_PP_LLP_PARSE_TAG_FORMAT_TPID3 = 3,
  /*
  *  Number of types in SOC_PB_PP_LLP_PARSE_TAG_FORMAT
  */
  SOC_PB_PP_NOF_LLP_PARSE_TAG_FORMATS = 4
}SOC_PB_PP_LLP_PARSE_TAG_FORMAT;
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

uint32
  soc_pb_pp_llp_parse_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_parse_tpid_values_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the global information for link layer parsing,
 *   including TPID values. Used in ingress to identify VLAN
 *   tags on incoming packets, and used in egress to
 *   construct VLAN tags on outgoing packets.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES                   *tpid_vals -
 *     The global information for link-layer parsing.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_values_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES                   *tpid_vals
  );

uint32
  soc_pb_pp_llp_parse_tpid_values_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES                   *tpid_vals
  );

uint32
  soc_pb_pp_llp_parse_tpid_values_get_verify(
    SOC_SAND_IN  int                                     unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_parse_tpid_values_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_parse_tpid_values_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_values_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_PARSE_TPID_VALUES                   *tpid_vals
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_parse_tpid_profile_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the TPID profile selection of two TPIDs from the
 *   Global TPIDs.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      tpid_profile_ndx -
 *     TPID Profile ID. Range: Soc_petraB: 0 - 3. T20E: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info -
 *     TPID profile information.
 * REMARKS:
 *   - set TPID profile (combination of TPID values), to be
 *   used in ingress/egress parsing and editing.- in Soc_petra-B
 *   Mapping a tpid-profile to a local-port is: using
 *   soc_ppd_port_info_set(local_port_ndx, tpid_profile)- in T20E
 *   Mapping a tpid-profile to a local-port is a two-stage
 *   process: 1. Assign a port-profile to the local-port
 *   using: soc_ppd_port_info_set(local_port_ndx, port_profile)
 *   2. Map assigned port-profile to a tpid-profile using:
 *   soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set(port_profile_ndx,
 *   tpid_profile)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

uint32
  soc_pb_pp_llp_parse_tpid_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

uint32
  soc_pb_pp_llp_parse_tpid_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_parse_tpid_profile_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_parse_tpid_profile_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_tpid_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Maps from Port profile to TPID Profile.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  port_profile_ndx -
 *     Port Profile ID. Range 0 - 7. set by
 *     soc_ppd_port_info_set().
 *   SOC_SAND_IN  uint32                                  tpid_profile_id -
 *     TPID Profile ID. Used for packet parsing/editing. Range:
 *     Soc_petraB: 0 - 3. T20E: 0 - 7.
 * REMARKS:
 *   - T20E only. In Soc_petra use
 *   soc_ppd_port_info_set(local_port_ndx, tpid_profile)- TPID
 *   profile is used to select TPID values.- Assign a
 *   port-profile to the local-port using: 1.
 *   soc_ppd_port_info_set(local_port_ndx, port_profile)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  );

uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  );

uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_port_profile_to_tpid_profile_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT uint32                                  *tpid_profile_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_parse_packet_format_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Given the port profile and the parsing information
 *   determine: - Whether this packet format accepted or
 *   denied. - The tag structure of the packet, i.e. what
 *   vlan tags exist on the packet (S-tag, S-C-tag, etc...).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  port_profile_ndx -
 *     Port Profile ID. Range 0 - 7. Set by
 *     soc_ppd_port_info_set().
 *   SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key -
 *     Parsing information specifying what TPIDs exist on the
 *     packet
 *   SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info -
 *     Packet format information, including whether this format
 *     is acceptable or not, and what vlan tag structure to
 *     assign to this packet
 * REMARKS:
 *   - Soc_petra-B only.- Assign a port-profile to the local-port
 *   using: 1. soc_ppd_port_info_set(local_port_ndx,
 *   port_profile)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_packet_format_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

uint32
  soc_pb_pp_llp_parse_packet_format_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

uint32
  soc_pb_pp_llp_parse_packet_format_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_parse_packet_format_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_parse_packet_format_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_parse_packet_format_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_parse_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_llp_parse module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_parse_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_parse_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_llp_parse module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_parse_get_errs_ptr(void);

uint32
  SOC_PB_PP_LLP_PARSE_TPID_VALUES_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_VALUES *info
  );

uint32
  SOC_PB_PP_LLP_PARSE_TPID_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_INFO *info
  );

uint32
  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_TPID_PROFILE_INFO *info
  );

uint32
  SOC_PB_PP_LLP_PARSE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO *info
  );

uint32
  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_PACKET_FORMAT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LLP_PARSE_INCLUDED__*/
#endif

