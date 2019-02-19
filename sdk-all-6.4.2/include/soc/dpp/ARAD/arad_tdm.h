/* $Id: arad_tdm.h,v 1.4 Broadcom SDK $
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

#ifndef __ARAD_TDM_INCLUDED__
/* { */
#define __ARAD_TDM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_tdm.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* Define Number of TDM Context MAP */
#define ARAD_NOF_TDM_CONTEXT_MAP (2)

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
   *  Optimize Unicast FTMH header.
   */
  ARAD_TDM_FTMH_INFO_MODE_OPT_UC = 0,
  /*
   *  Optimize Multicast FTMH header.
   */
  ARAD_TDM_FTMH_INFO_MODE_OPT_MC = 1,
  /*
   *  Standard Unicast ftmh header.
   */
  ARAD_TDM_FTMH_INFO_MODE_STANDARD_UC = 2,
  /*
   *  Standard Multicast ftmh header.
   */
  ARAD_TDM_FTMH_INFO_MODE_STANDARD_MC = 3,
  /*
   *  Number of types in ARAD_TDM_FTMH_INFO_MODE
   */
  ARAD_TDM_NOF_FTMH_INFO_MODE = 4
}ARAD_TDM_FTMH_INFO_MODE;

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
*     arad_tdm_init
* FUNCTION:
*     Initialization of the TDM configuration depends on the tdm mode.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Called as part of the initialization sequence.
*********************************************************************/
uint32
  arad_tdm_init(
    SOC_SAND_IN  int  unit
  );

/*********************************************************************
* NAME:
 *   arad_tdm_ftmh_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the FTMH header operation
 *   (added/unchanged/removed) at the ingress and egress,
 *   with the FTMH fields if added.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     FAP Port index. Range: 0 - 79.
 *   SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info -
 *     Attributes of the FTMH operation functionality
 * REMARKS:
 *   This API is relevant only under a TDM traffic mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tdm_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  );

uint32
  arad_tdm_ftmh_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  );

uint32
  arad_tdm_ftmh_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_ftmh_set_unsafe" API.
 *     Refer to "arad_tdm_ftmh_set_unsafe" API for details.
*********************************************************************/
uint32
  arad_tdm_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO            *info
  );

/*********************************************************************
* NAME:
 *   arad_tdm_opt_size_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the size limitations for the TDM cells in the
 *   Optimized FTMH TDM traffic mode.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    cell_size -
 *     Cell constant size for the TDM cells (includes the
 *     Optimized FTMH). Unit: Bytes. Range: 65 - 128.
 * REMARKS:
 *   Relevant only for an Optimized FTMH TDM traffic mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tdm_opt_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

uint32
  arad_tdm_opt_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

uint32
  arad_tdm_opt_size_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_opt_size_set_unsafe" API.
 *     Refer to "arad_tdm_opt_size_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_tdm_opt_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  );

/*********************************************************************
* NAME:
 *   arad_tdm_stand_size_range_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the size limitations for the TDM cells in the
 *   Standard FTMH TDM traffic mode.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range -
 *     TDM cell size range (includes the Standard FTMH). Unit:
 *     Bytes. Range: 65 - 128.
 * REMARKS:
 *   Relevant only for an Standard FTMH TDM traffic mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tdm_stand_size_range_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

uint32
  arad_tdm_stand_size_range_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

uint32
  arad_tdm_stand_size_range_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_stand_size_range_set_unsafe" API.
 *     Refer to "arad_tdm_stand_size_range_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_tdm_stand_size_range_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  );

/*********************************************************************
* NAME:
 *   arad_tdm_ofp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the OFP ports configured as TDM destination.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     FAP Port index. Range: 0 - 79.
 *   SOC_SAND_IN  uint8                   is_tdm -
 *     If True, then the OFP port is configured as a TDM
 *     destination.
 * REMARKS:
 *   Relevant only for a Packet traffic mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  arad_tdm_ofp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  );

uint32
  arad_tdm_ofp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  arad_tdm_ofp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_ofp_set_unsafe" API.
 *     Refer to "arad_tdm_ofp_set_unsafe" API for details.
*********************************************************************/
uint32
  arad_tdm_ofp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  );

/*********************************************************************
* NAME:
 *   arad_tdm_ifp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the IFP ports configured as TDM destination.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     FAP Port index. Range: 0 - 79.
 *   SOC_SAND_IN  uint8                   is_tdm -
 *     If True, then the IFP port is configured as a TDM
 *     destination.
 * REMARKS:
 *   Relevant only for a Packet traffic mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  arad_tdm_ifp_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  );

uint32
  arad_tdm_ifp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  arad_tdm_ifp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_ifp_set_unsafe" API.
 *     Refer to "arad_tdm_ifp_set_unsafe" API for details.
*********************************************************************/
int
  arad_tdm_ifp_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port,
    SOC_SAND_OUT uint8           *is_tdm
  );

/*********************************************************************
* NAME:
 *   arad_tdm_direct_routing_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the TDM direct routing configuration. Up to
 *   36 routing profiles can be defined. 
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   direct_routing_profile -
 *     TDM direct routing profile index. Mapping profile
 *     index. Range: 0 - 35.
 *   SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info -
 *     TDM direct routing configuration.
 *   SOC_SAND_IN  uint8 enable_rpt_reachable -
 *      Enables looking at the RPT reachable bitmap.
 * REMARKS:
 *   Each Incoming-Port is mapped to a direct routing profile by the
 *   arad_tdm_direct_routing_profile_map_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tdm_direct_routing_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_IN  uint8 enable_rpt_reachable
  );

uint32
  arad_tdm_direct_routing_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info
  );

uint32
  arad_tdm_direct_routing_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_direct_routing_set_unsafe" API.
 *     Refer to "arad_tdm_direct_routing_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_tdm_direct_routing_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_OUT ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_OUT uint8 *enable
  );

/*********************************************************************
* NAME:
 *   arad_tdm_direct_routing_profile_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Incoming FAP Port (IFP) routing profile type, per port.
 *   36 routing profiles can be defined. 
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     Incoming Fap Port index. Range: 0 - 255.
 *   SOC_SAND_IN  uint32                   direct_routing_profile -
 *     TDM direct routing profile index. Range: 0-35
 * REMARKS:
 *   The profile configuration is set by the
 *   arad_tdm_direct_routing_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tdm_direct_routing_profile_map_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint32                     direct_routing_profile 
  );

uint32
  arad_tdm_direct_routing_profile_map_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint32                     direct_routing_profile 
  );

uint32
  arad_tdm_direct_routing_profile_map_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_direct_routing_profile_map_set_unsafe" API.
 *     Refer to "arad_tdm_direct_routing_profile_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_tdm_direct_routing_profile_map_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   uint32                     port_ndx,
    SOC_SAND_OUT  uint32                    *direct_routing_profile
  );

/*********************************************************************
* NAME:
 *   arad_tdm_port_packet_crc_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable generating and removing fabric CRC Per FAP Port.   
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     Incoming Fap Port index. Range: 0 - 255.
 *   SOC_SAND_IN  uint8                   is_enable -
 *     Enable / Disable Packet CRC.
 * REMARKS:
 *   API is relavant only when port is in bypass mode (TDM).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tdm_port_packet_crc_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint8                      is_enable,         /* value to configure (is CRC added in fabric) */
    SOC_SAND_IN  uint8                      configure_ingress, /* should ingress be configured */
    SOC_SAND_IN  uint8                      configure_egress   /* should egress be configured */
  );

uint32
  arad_tdm_port_packet_crc_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint8                     is_enable 
  );

uint32
  arad_tdm_port_packet_crc_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_tdm_port_packet_crc_set_unsafe" API.
 *     Refer to "arad_tdm_port_packet_crc_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_tdm_port_packet_crc_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   uint32                     port_ndx,
    SOC_SAND_OUT  uint8                      *is_ingress_enabled,
    SOC_SAND_OUT  uint8                      *is_egress_enabled
  );

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_TDM_FTMH_OPT_UC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_UC *info
  );

uint32
  ARAD_TDM_FTMH_OPT_MC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_MC *info
  );

uint32
  ARAD_TDM_FTMH_STANDARD_UC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_UC *info
  );

uint32
  ARAD_TDM_FTMH_STANDARD_MC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_MC *info
  );

uint32
  ARAD_TDM_FTMH_INFO_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO *info
  );
uint32
  ARAD_TDM_MC_STATIC_ROUTE_INFO_verify(
    SOC_SAND_IN  ARAD_TDM_MC_STATIC_ROUTE_INFO *info
  );
uint32
  ARAD_TDM_DIRECT_ROUTING_INFO_verify(
    SOC_SAND_IN  uint32 unit,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *info
  );
#endif /* ARAD_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_TDM_INCLUDED__*/
#endif

