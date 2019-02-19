/* $Id: pb_api_ports.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_pb_tm/include/soc_pb_api_ports.h
*
* MODULE PREFIX:  soc_pb_ports
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

#ifndef __SOC_PB_API_PORTS_INCLUDED__
/* { */
#define __SOC_PB_API_PORTS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

#include <soc/dpp/TMC/tmc_api_ports.h>

#include <soc/dpp/Petra/petra_api_ports.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PORT_NOF_PP_PORTS                       (64)


/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PORT_FORWARDING_HEADER_PORT_NDX_FOR_TDM_RAW_GET(port_ndx)                  (port_ndx - SOC_PB_PORT_NOF_PP_PORTS)
/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

#define SOC_PB_PORTS_FC_TYPE_NONE                              SOC_TMC_PORTS_FC_TYPE_NONE
#define SOC_PB_PORTS_FC_TYPE_LL                                SOC_TMC_PORTS_FC_TYPE_LL
#define SOC_PB_PORTS_FC_TYPE_CB2                               SOC_TMC_PORTS_FC_TYPE_CB2
#define SOC_PB_PORTS_FC_TYPE_CB8                               SOC_TMC_PORTS_FC_TYPE_CB8
#define SOC_PB_PORTS_NOF_FC_TYPES                              SOC_TMC_PORTS_NOF_FC_TYPES
typedef SOC_TMC_PORTS_FC_TYPE                                  SOC_PB_PORTS_FC_TYPE;

typedef SOC_TMC_PORT_PP_PORT_INFO                              SOC_PB_PORT_PP_PORT_INFO;
typedef SOC_TMC_PORT_COUNTER_INFO                              SOC_PB_PORT_COUNTER_INFO;
typedef SOC_TMC_PORTS_FORWARDING_HEADER_INFO                   SOC_PB_PORTS_FORWARDING_HEADER_INFO;

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
 *   soc_pb_port_pp_port_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the Port profile for ports of type TM and Raw.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       pp_port_ndx -
 *     TM Port Profile Index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO        *info -
 *     Attributes of the TM Port Profile. Ignored for Raw
 *     Ports.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     If True, then the TM Port Profile is added. Otherwise,
 *     not enough resources may be available.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_pp_port_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );


uint32
  soc_pb_port_pp_port_set_dispatch(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_pp_port_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Port profile settings.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    pp_port_ndx -
 *     TM Port Profile Index. Range: 0 - 63.
 *   SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO     *info -
 *     Attributes of the TM Port Profile
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_pp_port_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int 					core_id,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO     *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_to_pp_port_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the Port to its Port profile for ports of type TM
 *   and Raw.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       port_ndx -
 *     TM Port Index. Range: 0 - 79.
 *   SOC_SAND_IN  uint32    pp_port -
 *     Mapping of the TM Port to its Profile
 * REMARKS:
 *   TM and Raw Ports can be mapped only to existing TM Port
 *   Profile. To add a new TM Port Profile, use the
 *   soc_pb_port_pp_port_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION         direction_ndx,
    SOC_SAND_IN  uint32                    pp_port
  );


uint32
  soc_pb_port_to_pp_port_map_set_dispatch(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION   direction_ndx
  );
/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_port_to_pp_port_map_set" API.
 *     Refer to "soc_pb_port_to_pp_port_map_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_forwarding_header_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *  Define the Forwarding header parameters for Raw ports.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       pp_port_ndx -
 *     TM Port Profile Index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info -
 *     Forwarding Header parameters.
 * REMARKS:
 *   This API can be used only if the TM Port Profile is for
 *   Raw Ports.
 *   For Packet Processing customers, the Trap Management and
 *   the Field Processing modules must be used.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_port_forwarding_header_set" API.
 *     Refer to "soc_pb_port_forwarding_header_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

void
  SOC_PB_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO *info
  );

void
  SOC_PB_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PORT_COUNTER_INFO *info
  );

void
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PORTS_FC_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PORTS_FC_TYPE enum_val
  );

void
  SOC_PB_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO *info
  );

void
  SOC_PB_PORT_COUNTER_INFO_print(
    SOC_SAND_IN  SOC_PB_PORT_COUNTER_INFO *info
  );

void
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_PORTS_INCLUDED__*/
#endif
