/* $Id: pb_pp_llp_trap.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_llp_trap.h
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

#ifndef __SOC_PB_PP_LLP_TRAP_INCLUDED__
/* { */
#define __SOC_PB_PP_LLP_TRAP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_trap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_TRAP_NOF_UD_L3_PROTOCOLS (7)

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
  SOC_PB_PP_LLP_TRAP_PORT_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_LLP_TRAP_FIRST,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_PRINT,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_VERIFY,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_GET,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_PRINT,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_VERIFY,
  SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_SET,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_PRINT,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_VERIFY,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_GET,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_PRINT,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_VERIFY,
  SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_PRINT,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_VERIFY,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_PRINT,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_VERIFY,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_PRINT,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_VERIFY,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_PRINT,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_VERIFY,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_UNSAFE,
  SOC_PB_PP_LLP_TRAP_GET_PROCS_PTR,
  SOC_PB_PP_LLP_TRAP_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_L3_PRTCL_PROCCESS,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_TRAP_PROCEDURE_DESC_LAST
} SOC_PB_PP_LLP_TRAP_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LLP_TRAP_FIRST,
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_IP_PROTOCOL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_SRC_PORT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_SRC_PORT_BITMAP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_DEST_PORT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_DEST_PORT_BITMAP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_ENABLE_BITMAP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_INVERSE_BITMAP_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR,
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR,
  SOC_PB_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_TRAP_ERR_LAST
} SOC_PB_PP_LLP_TRAP_ERR;

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
  soc_pb_pp_llp_trap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_port_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets port information for Link Layer Traps, including
 *   which reserved Multicast profile and which Traps are
 *   enabled...
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info -
 *     Port information for Link-layer trapping.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info
  );

uint32
  soc_pb_pp_llp_trap_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info
  );

uint32
  soc_pb_pp_llp_trap_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_port_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_trap_port_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_arp_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets information for ARP trapping, including My-IP
 *   addresses (used to Trap ARP Requests)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info -
 *     ARP trap information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_arp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info
  );

uint32
  soc_pb_pp_llp_trap_arp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info
  );

uint32
  soc_pb_pp_llp_trap_arp_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_arp_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_trap_arp_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_arp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_reserved_mc_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Trap information for IEEE reserved multicast
 *   (Ethernet Header. DA matches 01-80-c2-00-00-XX where XX =
 *   8'b00xx_xxxx.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key -
 *     Reserved Multicast key including Destination MAC address
 *     lsb (the msb are constant) and profile.
 *   SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *trap_action -
 *     Trap information including snoop/forwarding action.
 *     trap_code range: SOC_PPD_TRAP_CODE_RESERVED_MC_0-SOC_PPD_TRAP_CODE_RESERVED_MC_7.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_reserved_mc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *trap_action
  );

uint32
  soc_pb_pp_llp_trap_reserved_mc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *trap_action
  );

uint32
  soc_pb_pp_llp_trap_reserved_mc_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_reserved_mc_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_trap_reserved_mc_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_reserved_mc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *trap_action
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_prog_trap_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets a programmable trap, a trap that may be set to
 *   packets according to L2/L3/L4 attributes.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  prog_trap_ndx -
 *     Trap ID. Range: 0 - 3.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual -
 *     L2/L3/L4 attributes of the packets including (packets
 *     MAC-DA, Ether-Type, IP protocol, L4 ports).
 * REMARKS:
 *   - User can specify per port which programmable traps are
 *   enabled on the port - See soc_ppd_llp_trap_port_info_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_prog_trap_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

uint32
  soc_pb_pp_llp_trap_prog_trap_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

uint32
  soc_pb_pp_llp_trap_prog_trap_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_prog_trap_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_trap_prog_trap_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_prog_trap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_llp_trap module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_trap_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_llp_trap module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_trap_get_errs_ptr(void);

uint32
  SOC_PB_PP_LLP_TRAP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO *info
  );

uint32
  SOC_PB_PP_LLP_TRAP_ARP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO *info
  );

uint32
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY *info
  );

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO *info
  );

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO *info
  );

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO *info
  );

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LLP_TRAP_INCLUDED__*/
#endif

