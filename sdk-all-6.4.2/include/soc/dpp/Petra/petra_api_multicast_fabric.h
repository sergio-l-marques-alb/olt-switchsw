/* $Id: petra_api_multicast_fabric.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_api_multicast_fabric.h
*
* MODULE PREFIX:  soc_petra_mult_fabric
*
* FILE DESCRIPTION: In the Fabric-Multicast scheme,
*                   the packets/cells are replicated at the FE stage.
*                   This file holds the API functions and Structures
*                   which implement the Soc_petra Fabric Multicast.
*                   The file contains the standard get/set, clear and print
*                   for configuration.
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PETRA_API_MULTICAST_FABRIC_INCLUDED__
/* { */
#define __SOC_PETRA_API_MULTICAST_FABRIC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_multicast_fabric.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PETRA_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS SOC_SAND_DIV_ROUND_UP(SOC_PETRA_FBR_NOF_LINKS,SOC_SAND_REG_SIZE_BITS)
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

#define SOC_PETRA_MULT_FABRIC_CLS_MIN                          SOC_TMC_MULT_FABRIC_CLS_MIN
#define SOC_PETRA_MULT_FABRIC_CLS_MAX                          SOC_TMC_MULT_FABRIC_CLS_MAX
typedef SOC_TMC_MULT_FABRIC_CLS_RNG                            SOC_PETRA_MULT_FABRIC_CLS_RNG;

/* Soc_petra Egress Multicast Fabric Class: 0-3.    */
typedef SOC_TMC_MULT_FABRIC_CLS SOC_PETRA_MULT_FABRIC_CLS;

typedef SOC_TMC_MULT_FABRIC_PORT_INFO                          SOC_PETRA_MULT_FABRIC_PORT_INFO;
typedef SOC_TMC_MULT_FABRIC_SHAPER_INFO                        SOC_PETRA_MULT_FABRIC_SHAPER_INFO;
typedef SOC_TMC_MULT_FABRIC_BE_CLASS_INFO                      SOC_PETRA_MULT_FABRIC_BE_CLASS_INFO;
typedef SOC_TMC_MULT_FABRIC_BE_INFO                            SOC_PETRA_MULT_FABRIC_BE_INFO;
typedef SOC_TMC_MULT_FABRIC_GR_INFO                            SOC_PETRA_MULT_FABRIC_GR_INFO;
typedef SOC_TMC_MULT_FABRIC_INFO                               SOC_PETRA_MULT_FABRIC_INFO;
typedef SOC_TMC_MULT_FABRIC_ACTIVE_LINKS                       SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS;

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
*     soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set
* TYPE:
*   PROC
* FUNCTION:
*   Maps the embedded traffic class in the packet header to
*   a multicast class (0..3). This multicast class will be
*   further used for egress/fabric replication.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_TR_CLS                              tr_cls_ndx -
*     the traffic class index, for which to map a new class.
*     Range: 0 - 7.
*   SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS                     new_mult_cls -
*     The new multicast class that is mapped to the
*     tr_cls_ndx. Range: 0 - 3.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get
* TYPE:
*   PROC
* FUNCTION:
*   Maps the embedded traffic class in the packet header to
*   a multicast class (0..3). This multicast class will be
*   further used for egress/fabric replication.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_TR_CLS                              tr_cls_ndx -
*     the traffic class index, for which to map a new class.
*     Range: 0 - 7.
*   SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS                     *new_mult_cls -
*     The new multicast class that is mapped to the
*     tr_cls_ndx. Range: 0 - 3.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS         tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS     *new_mult_cls
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_base_queue_set
* TYPE:
*   PROC
* FUNCTION:
*   This procedure configures the base queue of the
*   multicast egress/fabric.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                  queue_id -
*     the base queue of fabric or egress multicast packets.
*     Range: 0 - 32K-1.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_base_queue_get
* TYPE:
*   PROC
* FUNCTION:
*   This procedure configures the base queue of the
*   multicast egress/fabric.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT uint32                                  *queue_id -
*     the base queue of fabric or egress multicast packets.
*     Range: 0 - 32K-1.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_credit_source_set
* TYPE:
*   PROC
* FUNCTION:
*   Set the Fabric Multicast credit generator configuration.
*   The credits comes either directly to these queues or
*   according to a scheduler scheme.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO                    *info -
*     Credit source configuration.
*   SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO                    *exact_info -
*     Exact credit source configuration as written to the
*     device.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_credit_source_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *exact_info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_credit_source_get
* TYPE:
*   PROC
* FUNCTION:
*   Set the Fabric Multicast credit generator configuration.
*   The credits comes either directly to these queues or
*   according to a scheduler scheme.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO                    *info -
*     Credit source configuration.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_credit_source_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *info
  );

/*********************************************************************
* NAME:
*   soc_petra_mult_fabric_enhanced_set
* TYPE:
*   PROC
* FUNCTION:
*   Configure the Enhanced Fabric Multicast Queue
*   configuration: the fabric multicast queues are defined
*   in a configured range, and the credits are coming to
*   these queues according to a scheduler scheme.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range -
*     Range of the FMQs where the fabric multicast packets are
*     sent to. Range: 0 - 32K-1.
* REMARKS:
*   If the Enhanced Fabric Multicast Queue configuration is
*   set, the credits must come to the FMQs via the
*   scheduler. Besides, scheduler schemes must be set for
*   each of the virtual OFP ports which will receive credits
*   for the multicast packets with the corresponding fabric
*   multicast class. The Enhanced Fabric Multicast Queue
*   configuration can be disabled by configuring the
*   'queue_range' back to 0 - 3.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  );

/*********************************************************************
* NAME:
*   soc_petra_mult_fabric_enhanced_get
* TYPE:
*   PROC
* FUNCTION:
*   Configure the Enhanced Fabric Multicast Queue
*   configuration: the fabric multicast queues are defined
*   in a configured range, and the credits are coming to
*   these queues according to a scheduler scheme.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range -
*     Range of the FMQs where the fabric multicast packets are
*     sent to. Range: 0 - 32K-1.
* REMARKS:
*   If the Enhanced Fabric Multicast Queue configuration is
*   set, the credits must come to the FMQs via the
*   scheduler. Besides, scheduler schemes must be set for
*   each of the virtual OFP ports which will receive credits
*   for the multicast packets with the corresponding fabric
*   multicast class. The Enhanced Fabric Multicast Queue
*   configuration can be disabled by configuring the
*   'queue_range' back to 0 - 3.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_active_links_set
* TYPE:
*   PROC
* FUNCTION:
*   This procedure sets the FAP links that are eligible for
*   multicast fabric traffic.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS            *links -
*     A bitmap of the device links. if bit (0 -
*     SOC_PETRA_NOF_LINKS) is up, then its corresponding link (0 -
*     SOC_PETRA_NOF_LINKS) is eligible for spatial multicast
*     distribution.
*   SOC_SAND_IN  uint8                                 tbl_refresh_enable -
*     Since multicast distribution table does not exist, the
*     data is taken from link map the user supplies (links),
*     and may be combined with data from unicast distribution
*     table. This parameter sets the refresh rate in which, is
*     case of the combined data as explained above, the data
*     is taken from the Unicast distribution table. TRUE -
*     Combine calculated data from UC distribution table with
*     active_mc_links. FALSE - Do not combine calculated data
*     from UC distribution table with active_mc_links.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This is a manual per-link configuration.
*   Per-link configuration is also possible.
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_active_links_get
* TYPE:
*   PROC
* FUNCTION:
*   This procedure sets the FAP links that are eligible for
*   multicast fabric traffic.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS            *links -
*     A bitmap of the device links. if bit (0 -
*     SOC_PETRA_NOF_LINKS) is up, then its corresponding link (0 -
*     SOC_PETRA_NOF_LINKS) is eligible for spatial multicast
*     distribution.
*   SOC_SAND_OUT uint8                                 *tbl_refresh_enable -
*     Since multicast distribution table does not exist, the
*     data is taken from link map the user supplies (links),
*     and may be combined with data from unicast distribution
*     table. This parameter sets the refresh rate in which, is
*     case of the combined data as explained above, the data
*     is taken from the Unicast distribution table. TRUE -
*     Combine calculated data from UC distribution table with
*     active_mc_links. FALSE - Do not combine calculated data
*     from UC distribution table with active_mc_links.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This is a manual per-link configuration.
*   Per-link configuration is also possible.
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_OUT uint8                 *tbl_refresh_enable
  );

void
  soc_petra_PETRA_MULT_FABRIC_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_PORT_INFO *info
  );

void
  soc_petra_PETRA_MULT_FABRIC_SHAPER_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_SHAPER_INFO *info
  );

void
  soc_petra_PETRA_MULT_FABRIC_BE_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_CLASS_INFO *info
  );

void
  soc_petra_PETRA_MULT_FABRIC_BE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_INFO *info
  );

void
  soc_petra_PETRA_MULT_FABRIC_GR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_GR_INFO *info
  );

void
  soc_petra_PETRA_MULT_FABRIC_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO *info
  );

void
  soc_petra_PETRA_MULT_FABRIC_ACTIVE_LINKS_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_MULT_FABRIC_CLS_RNG_to_string(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_CLS_RNG enum_val
  );



void
  soc_petra_PETRA_MULT_FABRIC_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_PORT_INFO *info
  );



void
  soc_petra_PETRA_MULT_FABRIC_SHAPER_INFO_print(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_SHAPER_INFO *info
  );



void
  soc_petra_PETRA_MULT_FABRIC_BE_CLASS_INFO_print(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_BE_CLASS_INFO *info
  );



void
  soc_petra_PETRA_MULT_FABRIC_BE_INFO_print(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_BE_INFO *info
  );



void
  soc_petra_PETRA_MULT_FABRIC_GR_INFO_print(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_GR_INFO *info
  );



void
  soc_petra_PETRA_MULT_FABRIC_INFO_print(
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_INFO *info
  );



void
  soc_petra_PETRA_MULT_FABRIC_ACTIVE_LINKS_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_MULTICAST_FABRIC_INCLUDED__*/
#endif
