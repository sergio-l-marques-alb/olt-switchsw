/* $Id: pb_tdm.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_tdm.h
*
* MODULE PREFIX:  soc_pb_tdm
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

#ifndef __SOC_PB_TDM_INCLUDED__
/* { */
#define __SOC_PB_TDM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_tdm.h>

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

typedef enum
{
  /*
   *  Optimize Unicast FTMH header.
   */
  SOC_PB_TDM_FTMH_INFO_MODE_OPT_UC = 0,
  /*
   *  Optimize Multicast FTMH header.
   */
  SOC_PB_TDM_FTMH_INFO_MODE_OPT_MC = 1,
  /*
   *  Standard Unicast ftmh header.
   */
  SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_UC = 2,
  /*
   *  Standard Multicast ftmh header.
   */
  SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_MC = 3,
  /*
   *  Number of types in SOC_PB_TDM_FTMH_INFO_MODE
   */
  SOC_PB_TDM_NOF_FTMH_INFO_MODE = 4
}SOC_PB_TDM_FTMH_INFO_MODE;

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
*     soc_pb_tdm_init
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
  soc_pb_tdm_init(
    SOC_SAND_IN  int  unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_ftmh_set_unsafe
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
 *   SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info -
 *     Attributes of the FTMH operation functionality
 * REMARKS:
 *   This API is relevant only under a TDM traffic mode.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info
  );

uint32
  soc_pb_tdm_ftmh_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info
  );

uint32
  soc_pb_tdm_ftmh_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_ftmh_set_unsafe" API.
 *     Refer to "soc_pb_tdm_ftmh_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PB_TDM_FTMH_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_opt_size_set_unsafe
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
  soc_pb_tdm_opt_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

uint32
  soc_pb_tdm_opt_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

uint32
  soc_pb_tdm_opt_size_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_opt_size_set_unsafe" API.
 *     Refer to "soc_pb_tdm_opt_size_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_tdm_opt_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_stand_size_range_set_unsafe
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
  soc_pb_tdm_stand_size_range_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

uint32
  soc_pb_tdm_stand_size_range_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

uint32
  soc_pb_tdm_stand_size_range_get_verify(
    SOC_SAND_IN  int                   unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_stand_size_range_set_unsafe" API.
 *     Refer to "soc_pb_tdm_stand_size_range_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_tdm_stand_size_range_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_ofp_set_unsafe
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
uint32
  soc_pb_tdm_ofp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  soc_pb_tdm_ofp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  soc_pb_tdm_ofp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_ofp_set_unsafe" API.
 *     Refer to "soc_pb_tdm_ofp_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_ofp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_ifp_set_unsafe
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
uint32
  soc_pb_tdm_ifp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  soc_pb_tdm_ifp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  soc_pb_tdm_ifp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_ifp_set_unsafe" API.
 *     Refer to "soc_pb_tdm_ifp_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_ifp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_mc_static_route_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the TDM multicast static route configuration. Up to
 *   16 routes can be defined. For a TDM Multicast cell, its
 *   selected Route is done according to the 4 MSB of its
 *   Multicast-ID.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   mc_id_route_ndx -
 *     TDM Multicast route index. This route is selected if the
 *     4 MSB of the Multicast-ID is equal to it. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info -
 *     TDM multicast static route configuration.
 * REMARKS:
 *   1. Relevant for Soc_petra-B B0 only.2. This feature is
 *   enabled only if configured in the soc_pb_mgmt_rev_b0_set
 *   API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  );

uint32
  soc_pb_tdm_mc_static_route_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  );

uint32
  soc_pb_tdm_mc_static_route_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_mc_static_route_set_unsafe" API.
 *     Refer to "soc_pb_tdm_mc_static_route_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_OUT SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  );


uint32
  SOC_PB_TDM_FTMH_OPT_UC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_UC *info
  );

uint32
  SOC_PB_TDM_FTMH_OPT_MC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_MC *info
  );

uint32
  SOC_PB_TDM_FTMH_STANDARD_UC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_UC *info
  );

uint32
  SOC_PB_TDM_FTMH_STANDARD_MC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_MC *info
  );

uint32
  SOC_PB_TDM_FTMH_INFO_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO *info
  );
uint32
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_TDM_INCLUDED__*/
#endif
