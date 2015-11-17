/* $Id: pb_api_tdm.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_tdm.h
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

#ifndef __SOC_PB_API_TDM_INCLUDED__
/* { */
#define __SOC_PB_API_TDM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_tdm.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* Queue type for TDM */
#define SOC_PB_TDM_PUSH_QUEUE_TYPE                            (SOC_TMC_ITM_QT_NDX_15)

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

#define SOC_PB_TDM_ING_ACTION_ADD                              SOC_TMC_TDM_ING_ACTION_ADD
#define SOC_PB_TDM_ING_ACTION_NO_CHANGE                        SOC_TMC_TDM_ING_ACTION_NO_CHANGE
#define SOC_PB_TDM_ING_ACTION_CUSTOMER_EMBED                   SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED
#define SOC_PB_TDM_NOF_ING_ACTIONS                             SOC_TMC_TDM_NOF_ING_ACTIONS
typedef SOC_TMC_TDM_ING_ACTION                                 SOC_PB_TDM_ING_ACTION;

#define SOC_PB_TDM_EG_ACTION_REMOVE                            SOC_TMC_TDM_EG_ACTION_REMOVE
#define SOC_PB_TDM_EG_ACTION_NO_CHANGE                         SOC_TMC_TDM_EG_ACTION_NO_CHANGE
#define SOC_PB_TDM_EG_ACTION_CUSTOMER_EXTRACT                  SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT
#define SOC_PB_TDM_NOF_EG_ACTIONS                              SOC_TMC_TDM_NOF_EG_ACTIONS
typedef SOC_TMC_TDM_EG_ACTION                                  SOC_PB_TDM_EG_ACTION;

typedef SOC_TMC_TDM_FTMH_OPT_UC                                SOC_PB_TDM_FTMH_OPT_UC;
typedef SOC_TMC_TDM_FTMH_OPT_MC                                SOC_PB_TDM_FTMH_OPT_MC;
typedef SOC_TMC_TDM_FTMH_STANDARD_UC                           SOC_PB_TDM_FTMH_STANDARD_UC;
typedef SOC_TMC_TDM_FTMH_STANDARD_MC                           SOC_PB_TDM_FTMH_STANDARD_MC;
typedef SOC_TMC_TDM_FTMH                                       SOC_PB_TDM_FTMH;
typedef SOC_TMC_TDM_FTMH_INFO                                  SOC_PB_TDM_FTMH_INFO;

typedef SOC_TMC_TDM_MC_STATIC_ROUTE_INFO                       SOC_PB_TDM_MC_STATIC_ROUTE_INFO;

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
 *   soc_pb_tdm_ftmh_set
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
  soc_pb_tdm_ftmh_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_tdm_ftmh_set" API.
 *     Refer to "soc_pb_tdm_ftmh_set" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PB_TDM_FTMH_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_opt_size_set
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
  soc_pb_tdm_opt_size_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_opt_size_set" API.
 *     Refer to "soc_pb_tdm_opt_size_set" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_opt_size_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_stand_size_range_set
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
  soc_pb_tdm_stand_size_range_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_stand_size_range_set" API.
 *     Refer to "soc_pb_tdm_stand_size_range_set" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_stand_size_range_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_tdm_ofp_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the OFP ports configured as TDM destination.
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
  soc_pb_tdm_mc_static_route_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_tdm_mc_static_route_set" API.
 *     Refer to "soc_pb_tdm_mc_static_route_set" API for details.
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_OUT SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  );
void
  SOC_PB_TDM_FTMH_OPT_UC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_OPT_UC *info
  );

void
  SOC_PB_TDM_FTMH_OPT_MC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_OPT_MC *info
  );

void
  SOC_PB_TDM_FTMH_STANDARD_UC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_STANDARD_UC *info
  );

void
  SOC_PB_TDM_FTMH_STANDARD_MC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_STANDARD_MC *info
  );

void
  SOC_PB_TDM_FTMH_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH *info
  );

void
  SOC_PB_TDM_FTMH_INFO_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_INFO *info
  );
void
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PB_TDM_MC_STATIC_ROUTE_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_TDM_ING_ACTION_to_string(
    SOC_SAND_IN  SOC_PB_TDM_ING_ACTION enum_val
  );

const char*
  SOC_PB_TDM_EG_ACTION_to_string(
    SOC_SAND_IN  SOC_PB_TDM_EG_ACTION enum_val
  );

void
  SOC_PB_TDM_FTMH_OPT_UC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_UC *info
  );

void
  SOC_PB_TDM_FTMH_OPT_MC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_MC *info
  );

void
  SOC_PB_TDM_FTMH_STANDARD_UC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_UC *info
  );

void
  SOC_PB_TDM_FTMH_STANDARD_MC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_MC *info
  );

void
  SOC_PB_TDM_FTMH_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH *info
  );

void
  SOC_PB_TDM_FTMH_INFO_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO *info
  );
void
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_TDM_INCLUDED__*/
#endif
