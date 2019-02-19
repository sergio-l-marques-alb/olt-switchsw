/* $Id: ppd_api_llp_parse.h,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_llp_parse.h
*
* MODULE PREFIX:  soc_ppd_llp
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

#ifndef __SOC_PPD_API_LLP_PARSE_INCLUDED__
/* { */
#define __SOC_PPD_API_LLP_PARSE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_llp_parse.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of global TPID values in device.                 */
#define  SOC_PPD_LLP_PARSE_NOF_TPID_VALS (SOC_PPC_LLP_PARSE_NOF_TPID_VALS)

#define SOC_PPD_LLP_PARSE_FLAGS_OUTER_C   (SOC_PPC_LLP_PARSE_FLAGS_OUTER_C)
#define SOC_PPD_LLP_PARSE_FLAGS_INNER_C   (SOC_PPC_LLP_PARSE_FLAGS_INNER_C)


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
  SOC_PPD_LLP_PARSE_TPID_VALUES_SET = SOC_PPD_PROC_DESC_BASE_LLP_PARSE_FIRST,
  SOC_PPD_LLP_PARSE_TPID_VALUES_SET_PRINT,
  SOC_PPD_LLP_PARSE_TPID_VALUES_GET,
  SOC_PPD_LLP_PARSE_TPID_VALUES_GET_PRINT,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT,
  SOC_PPD_LLP_PARSE_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PPD_LLP_PARSE_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_PARSE_PROCEDURE_DESC;

#define SOC_PPD_LLP_PARSE_TPID_INDEX_NONE                      SOC_PPC_LLP_PARSE_TPID_INDEX_NONE
#define SOC_PPD_LLP_PARSE_TPID_INDEX_TPID1                     SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1
#define SOC_PPD_LLP_PARSE_TPID_INDEX_TPID2                     SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2
#define SOC_PPD_LLP_PARSE_TPID_INDEX_ISID_TPID                 SOC_PPC_LLP_PARSE_TPID_INDEX_ISID_TPID
#define SOC_PPD_NOF_LLP_PARSE_TPID_INDEXS                      SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS
typedef SOC_PPC_LLP_PARSE_TPID_INDEX                           SOC_PPD_LLP_PARSE_TPID_INDEX;

typedef SOC_PPC_LLP_PARSE_TPID_VALUES                          SOC_PPD_LLP_PARSE_TPID_VALUES;
typedef SOC_PPC_LLP_PARSE_TPID_INFO                            SOC_PPD_LLP_PARSE_TPID_INFO;
typedef SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO                    SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO;
typedef SOC_PPC_LLP_PARSE_INFO                                 SOC_PPD_LLP_PARSE_INFO;
typedef SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO                   SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO;

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
 *   soc_ppd_llp_parse_tpid_values_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the global information for link layer parsing,
 *   including TPID values. Used in ingress to identify VLAN
 *   tags on incoming packets, and used in egress to
 *   construct VLAN tags on outgoing packets.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals -
 *     The global information for link-layer parsing.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_values_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_llp_parse_tpid_values_set" API.
 *     Refer to "soc_ppd_llp_parse_tpid_values_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_values_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_parse_tpid_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the TPID profile selection of two TPIDs from the
 *   Global TPIDs.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                tpid_profile_ndx -
 *     TPID Profile ID. Range: Soc_petraB: 0 - 3. T20E: 0 - 7.
 *   SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info -
 *     TPID profile information.
 * REMARKS:
 *   - set TPID profile (combination of TPID values), to be
 *   used in ingress/egress parsing and editing.- in Soc_petra-B
 *   Assigning a tpid-profile to a local-port is: using
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
  soc_ppd_llp_parse_tpid_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_llp_parse_tpid_profile_info_set" API.
 *     Refer to "soc_ppd_llp_parse_tpid_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_llp_parse_tpid_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Maps from Port profile to TPID Profile.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                port_profile_ndx -
 *     Port Profile ID. Range 0 - 7. set by
 *     soc_ppd_port_info_set().
 *   SOC_SAND_IN  uint32                                tpid_profile_id -
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
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint32                                tpid_profile_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set"
 *     API.
 *     Refer to
 *     "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_OUT uint32                                *tpid_profile_id
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_parse_packet_format_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Given the port profile and the parsing information
 *   determine: - Whether this packet format accepted or
 *   denied. - The tag structure of the packet, i.e. what
 *   vlan tags exist on the packet (S-tag, S-C-tag, etc...).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access. Range 0- 3.
 *   SOC_SAND_IN  uint32                          eg_acceptable_frames_port_profile -
 *     Egress Acceptable frames profile per port.
 *   SOC_SAND_IN  uint32                          llvp_port_profile -
 *     LLVP profile per port. Range 0-7.
 *   SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key -
 *     Parsing information specifying what TPIDs exist on the
 *     packet
 *   SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info -
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
  soc_ppd_llp_parse_packet_format_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_llp_parse_packet_format_info_set" API.
 *     Refer to "soc_ppd_llp_parse_packet_format_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_llp_parse_packet_format_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_llp_parse_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_llp_parse module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_llp_parse_get_procs_ptr(void);

void
  SOC_PPD_LLP_PARSE_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_VALUES *info
  );

void
  SOC_PPD_LLP_PARSE_TPID_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_INFO *info
  );

void
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO *info
  );

void
  SOC_PPD_LLP_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_INFO *info
  );

void
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LLP_PARSE_TPID_INDEX_to_string(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_INDEX enum_val
  );

void
  SOC_PPD_LLP_PARSE_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES *info
  );

void
  SOC_PPD_LLP_PARSE_TPID_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_INFO *info
  );

void
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO *info
  );

void
  SOC_PPD_LLP_PARSE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO *info
  );

void
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_llp_parse_tpid_values_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_VALUES               *tpid_vals
  );

void
  soc_ppd_llp_parse_tpid_values_get_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_llp_parse_tpid_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

void
  soc_ppd_llp_parse_tpid_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx
  );

void
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint32                                tpid_profile_id
  );

void
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx
  );

void
  soc_ppd_llp_parse_packet_format_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

void
  soc_ppd_llp_parse_packet_format_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_PARSE_INFO                      *parse_key
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_LLP_PARSE_INCLUDED__*/
#endif

