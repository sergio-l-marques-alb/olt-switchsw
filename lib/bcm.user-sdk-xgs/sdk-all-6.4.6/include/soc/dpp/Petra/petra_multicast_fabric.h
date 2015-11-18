/* $Id: petra_multicast_fabric.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_multicast_fabric.h
*
* MODULE PREFIX:  soc_petra_mult_fabric
*
* FILE DESCRIPTION: In the Fabric-Multicast scheme,
*                   the packets/cells are replicated at the FE stage.
*                   This file contains standard unsafe get/set and
*                   verify functions for configuration purposes.
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PETRA_MULTICAST_FABRIC_INCLUDED__
/* { */
#define __SOC_PETRA_MULTICAST_FABRIC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/Petra/petra_api_multicast_fabric.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_MULT_FABRIC_SHAPER_MAX_BURST_SHIFT  26
#define SOC_PETRA_MULT_FABRIC_SHAPER_MAX_BURST_SIZE   6

#define SOC_PETRA_MULT_FABRIC_SHAPER_RATE_SHIFT       0
#define SOC_PETRA_MULT_FABRIC_SHAPER_RATE_SIZE        26

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
*     soc_petra_multicast_fabric_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_multicast_fabric_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_TR_CLS         tr_cls_ndx -
*     tr_cls_ndx-the traffic class index, for which to map a
*     new class. SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls -
*     new_mult_cls - The new multicast class that is mapped to
*     the tr_cls_ndx.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe
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
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS         tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe
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
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS         tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS     *new_mult_cls
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_base_queue_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure configures the base queue of the
*     multicast egress/fabric.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  queue_id -
*     the base queue of fabric or egress multicast packets.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_base_queue_set_unsafe
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
  soc_petra_mult_fabric_base_queue_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_base_queue_get_unsafe
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
  soc_petra_mult_fabric_base_queue_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_credit_source_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Set the Fabric Multicast credit generator configuration.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info -
*     Credit source configuration. SOC_SAND_IN
*     SOC_PETRA_MULT_FABRIC_INFO *exact_info - Exact credit source
*     configuration as written to the device.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_credit_source_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_credit_source_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Set the Fabric Multicast credit generator configuration
*   for the Default Fabric Multicast Queue configuration.
*   The fabric multicast queues are 0 - 3, and the credits
*   comes either directly to these queues or according to a
*   scheduler scheme.
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
  soc_petra_mult_fabric_credit_source_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *exact_info
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_credit_source_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Set the Fabric Multicast credit generator configuration
*   for the Default Fabric Multicast Queue configuration.
*   The fabric multicast queues are 0 - 3, and the credits
*   comes either directly to these queues or according to a
*   scheduler scheme.
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
  soc_petra_mult_fabric_credit_source_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *info
  );

/*********************************************************************
* NAME:
*   soc_petra_mult_fabric_enhanced_set_unsafe
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
  soc_petra_mult_fabric_enhanced_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  );

/*********************************************************************
* NAME:
*   soc_petra_mult_fabric_enhanced_set_verify
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
  soc_petra_mult_fabric_enhanced_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  );

/*********************************************************************
* NAME:
*   soc_petra_mult_fabric_enhanced_get_unsafe
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
  soc_petra_mult_fabric_enhanced_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_active_links_verify
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     This procedure sets the FAP links that are eligible for
*     multicast fabric traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links -
*     A bitmap of the device links. if bit (0 -
*     SOC_PETRA_FBR_NOF_LINKS) is up, then its corresponding link (0 -
*     SOC_PETRA_FBR_NOF_LINKS) is eligible for spatial multicast
*     distribution. SOC_SAND_IN
*  SOC_SAND_IN  uint8                 tbl_refresh_enable -
*     tbl_refresh_enable - Since multicast distribution table
*     does not exist, the data is taken from link map the user
*     supplies (active_mc_links), and may be combined with
*     data from unicast distribution table. This parameter
*     sets the refresh rate in which, is case of the combined
*     data as explained above, the data is taken from the
*     unicast distribution table. TRUE - Combine calculated
*     data from UC distribution table with active_mc_links.
*     FALSE - Do not combine calculated data from UC
*     distribution table with active_mc_links.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  );

/*********************************************************************
* NAME:
*   soc_petra_mult_fabric_active_links_set_unsafe
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
  soc_petra_mult_fabric_active_links_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mult_fabric_active_links_get_unsafe
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
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_OUT uint8                 *tbl_refresh_enable
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_MULTICAST_FABRIC_INCLUDED__*/
#endif
