/* $Id: pb_flow_control.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_flow_control.h
*
* MODULE PREFIX:  soc_pb_flow
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

#ifndef __SOC_PB_FLOW_CONTROL_INCLUDED__
/* { */
#define __SOC_PB_FLOW_CONTROL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_flow_control.h>

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
*     soc_pb_flow_control_init
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
  soc_pb_flow_control_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_inbnd_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Generation Configuration, based on
 *   Soc_petra Ingress state indications.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info -
 *     Inbound FC Generation mode and configuration.
 * REMARKS:
 *   1. This API is not applicable for Interlaken NIF 2. FC
 *   generation can be triggered by VSQ (statistics tag)
 *   and/or CNM. VSQ to NIF/Class mapping is predefined,
 *   according to the NIF type and FC mode (LL/CB). For CNM -
 *   use soc_pb_fc_gen_inbnd_cnm_map_set API to complete the
 *   configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info
  );

uint32
  soc_pb_fc_gen_inbnd_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info
  );

uint32
  soc_pb_fc_gen_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_gen_inbnd_set_unsafe" API.
 *     Refer to "soc_pb_fc_gen_inbnd_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_INFO           *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_inbnd_glb_hp_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable Flow Control generation, based on Ingress
 *   Global Resources - high priority, via NIF. Flow Control
 *   generation may be either Link Level or Class Based. For
 *   Link Level - Flow Control will be generated on all
 *   links. For Class Based - Flow Control will be generated
 *   on all Flow Control Classes.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode -
 *     Flow Control mode. If enabled, when Ingress Global
 *     Resources high priority Flow Control is indicated.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_glb_hp_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );

uint32
  soc_pb_fc_gen_inbnd_glb_hp_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );

uint32
  soc_pb_fc_gen_inbnd_glb_hp_get_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_gen_inbnd_glb_hp_set_unsafe" API.
 *     Refer to "soc_pb_fc_gen_inbnd_glb_hp_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_glb_hp_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_inbnd_cnm_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *   indication to generate when using Class Based Flow
 *   Control
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      cpid_tc_ndx -
 *     The 3-LSB of the CPID index. Expected to represent TC.
 *   SOC_SAND_IN  uint32                      fc_class -
 *     The FC Class of the generated CBFC. Note: according to
 *     the inheritance configuration of the relevant NIF, the
 *     specified class may affect also lower classes. Range: 0 -
 *     7.
 * REMARKS:
 *   Inband FC generation, including CNM, is configured per
 *   NIF by soc_pb_fc_gen_inbnd_set API. This API completes the
 *   CNM configuration when Class Based FC is used.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  );

uint32
  soc_pb_fc_gen_inbnd_cnm_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  );

uint32
  soc_pb_fc_gen_inbnd_cnm_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_gen_inbnd_cnm_map_set_unsafe" API.
 *     Refer to "soc_pb_fc_gen_inbnd_cnm_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rec_inbnd_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Reception Configuration
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info -
 *     Inbound FC Reception mode and configuration.
 * REMARKS:
 *   1. This API is not applicable for Interlaken NIF 2. For
 *   CB - use soc_pb_fc_rec_inbnd_ofp_map_set API to complete the
 *   configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info
  );

uint32
  soc_pb_fc_rec_inbnd_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info
  );

uint32
  soc_pb_fc_rec_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rec_inbnd_set_unsafe" API.
 *     Refer to "soc_pb_fc_rec_inbnd_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_INFO           *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rec_inbnd_ofp_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Selects the OFP, priority and Reaction Point for inband
 *   FC reception.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   SOC_SAND_IN  uint32                      fc_cls_ndx -
 *     FC Class. Range: 0 - 7. If FC-class inheritance is
 *     enabled on the NIF, it is sufficient to call this API
 *     for the lowest FC class for which FC reception is
 *     enabled.
 *   SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info -
 *     OFP, priority and Reaction Point for inband FC reception
 *     on the specified NIF, and FC class
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info
  );

uint32
  soc_pb_fc_rec_inbnd_ofp_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info
  );

uint32
  soc_pb_fc_rec_inbnd_ofp_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rec_inbnd_ofp_map_set_unsafe" API.
 *     Refer to "soc_pb_fc_rec_inbnd_ofp_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_OFP_MAP_INFO         *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rcy_ofp_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the configuration for handling Flow Control
 *   generated by (and effecting) recycling traffic. Can be
 *   triggered by crossing Virtual Statistics Queues (CT3CC),
 *   or by crossing High or Low Global Resources threshold.
 *   Handled on Outgoing FAP Port level - Scheduler HR or
 *   EGQ.
 * INPUT:
 *   SOC_SAND_IN  int             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32             trgr_ndx -
 *     FC trigger index. 1. For configuring FC based on VSQs:
 *     Category 3 Connection Class to map. The Connection Class
 *     will usually represent Recycling Outgoing FAP Port,
 *     which will receive Flow Control triggered by crossing
 *     the Flow Control threshold of Category 3 Ingress
 *     Statistics Queue. Range: 0 - 31. 2. For configuring FC
 *     based on Global Resources consumption - one of the
 *     following: SOC_PB_FC_RCY_GLB_RCS_ID_LP,
 *     SOC_PB_FC_RCY_GLB_RCS_ID_HP
 *   SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx -
 *     FC reaction point index.
 *     One of the following: SOC_PB_FC_RCY_OFP_HANDLER_SCH, SOC_PB_FC_RCY_OFP_HANDLER_EGQ
 *   SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx -
 *     FC priority index.
 *     One of the following: SOC_PB_FC_OFP_PRIORITY_LP, SOC_PB_FC_OFP_PRIORITY_HP
 *   SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info -
 *     OFP configuration as FC reaction point.
 *     A structure that contains one field representing a bitmap of the outgoing FAP ports to config.
 *     If the trgr_ndx is VSQ the bitmap should have at most one bit on, if the trgr_ndx is Global multiple bits can be set on.
 *     A zeroed bit map will switch off all FAP ports.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info
  );

uint32
  soc_pb_fc_rcy_ofp_set_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info
  );

uint32
  soc_pb_fc_rcy_ofp_get_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rcy_ofp_set_unsafe" API.
 *     Refer to "soc_pb_fc_rcy_ofp_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_OUT SOC_PB_FC_RCY_PORT_INFO   *info
  );


/*********************************************************************
* NAME:
 *   soc_pb_fc_rcy_hr_enable_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable reception of Flow Control for Recycling
 *   ports by end to end Scheduler HR-s 192-223, represented
 *   by Category 3 Connection Class in Ingress Statistics
 *   Queues, when the Statistics Queues threshold is crossed.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                      enable -
 *     Enable/disable FC reception by HR 192-223 (recycling
 *     HR-s).
 * REMARKS:
 *   Connection Classes 68-99 are mapped 1-1 to HR 192-223.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      enable
  );

uint32
  soc_pb_fc_rcy_hr_enable_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      enable
  );

uint32
  soc_pb_fc_rcy_hr_enable_get_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rcy_hr_enable_set_unsafe" API.
 *     Refer to "soc_pb_fc_rcy_hr_enable_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint8                      *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_cal_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures Calendar-based interface calendar for Flow
 *   Control Generation (OOB/ILKN-Inband TX).
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     generation: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC is
 *     generated. For OOB interface (both SPI and ILKN), only
 *     SOC_PB_FC_OOB_ID_B can be used. Additionally, ILKN-A can be
 *     used.
 *   SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf -
 *     Calendar-based (OOB) interface configuration -
 *     enable/disable, calendar configuration - length and
 *     repetitions.
 *   SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff -
 *     A buffer with cal_conf.cal_len entries. An allocation
 *     with the appropriate size should be made. Each entry
 *     configures a calendar channel, defining the source of
 *     the Flow Control that controls this channel.
 * REMARKS:
 *   1. For global resources - use indexes defined in
 *   SOC_PB_FC_CAL_GLB_RCS_ID.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff
  );

uint32
  soc_pb_fc_gen_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff
  );

uint32
  soc_pb_fc_gen_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_gen_cal_set_unsafe" API.
 *     Refer to "soc_pb_fc_gen_cal_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_GEN_CALENDAR             *cal_buff
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rec_cal_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures Calendar-based interface calendar for Flow
 *   Control Reception.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     reception: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC
 *     reception is handled.
 *   SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf -
 *     Calendar-based (OOB) interface configuration -
 *     enable/disable, calendar configuration - length and
 *     repetitions.
 *   SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff -
 *     A buffer with cal_conf.cal_len entries. Each entry
 *     configures a calendar channel, defining the destination
 *     that handles the Flow Control from this channel.
 * REMARKS:
 *   The get function is not symmetric; if the same SCH-based
 *   OFP HR appears both in low and high priority in the
 *   calendar in different entries. In this case, the
 *   returned calendar indicates for the first entry of this
 *   HR a low priority, and high priority for the next
 *   entries of this HR.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff
  );

uint32
  soc_pb_fc_rec_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff
  );

uint32
  soc_pb_fc_rec_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rec_cal_set_unsafe" API.
 *     Refer to "soc_pb_fc_rec_cal_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_REC_CALENDAR             *cal_buff
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_oob_phy_params_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures physical parameters for out-of-band Flow Control Reception.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     generation: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC reception is handled.
 *   SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO              *phy_params -
 *     Calendar-based (OOB) interface physical parameters configuration.
 * REMARKS:
 *   relevant only for SPI_OOB calendar mode (not Interlaken)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO          *phy_params
  );

uint32
  soc_pb_fc_oob_phy_params_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO          *phy_params
  );

uint32
  soc_pb_fc_oob_phy_params_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_oob_phy_params_set"
 *     API.
 *     Refer to "soc_pb_fc_oob_phy_params_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT  SOC_PB_FC_PHY_PARAMS_INFO         *phy_params
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_egr_rec_oob_stat_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Diagnostics - get the Out-Of-Band interface status
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_FC_OOB_ID                   oob_ndx -
 *     OOB Interface index. Range: 0 - 1.
 *   SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT             *status -
 *     The status of the OOB interface.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_egr_rec_oob_stat_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   oob_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT             *status
  );

uint32
  soc_pb_fc_egr_rec_oob_stat_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   oob_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_ilkn_llfc_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Defines if and how LLFC can be received/generated using
 *   Interlaken NIF.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use SOC_PB_NIF_ID(ILKN, 0 - 1).
 *   SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info -
 *     Link Level Flow Control configuration for the Interlaken
 *     NIF
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info
  );

uint32
  soc_pb_fc_ilkn_llfc_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info
  );

uint32
  soc_pb_fc_ilkn_llfc_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_ilkn_llfc_set_unsafe" API.
 *     Refer to "soc_pb_fc_ilkn_llfc_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *gen_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_nif_oversubscr_scheme_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Defines FC Oversubscription scheme on the NIFs. For up
 *   to 100GE traffic on the device NIFs should be set to
 *   'NONE'. Otherwise, should be set according to the
 *   oversubscription level. The scheme affects internal NIF
 *   FC thresholds each scheme defines a different thresholds
 *   preset.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme -
 *     FC Oversubscription scheme
 * REMARKS:
 *   Consult with Dune support when configuring NIF
 *   oversubscription scenarios (above 100GE)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme
  );

uint32
  soc_pb_fc_nif_oversubscr_scheme_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme
  );

uint32
  soc_pb_fc_nif_oversubscr_scheme_get_verify(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_nif_oversubscr_scheme_set_unsafe" API.
 *     Refer to "soc_pb_fc_nif_oversubscr_scheme_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    *scheme
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_nif_pause_quanta_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAC Lane index. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx -
 *     Selects whether the configuration refers to flow control
 *     generation, reception or both.
 *   SOC_SAND_IN  uint32                       pause_quanta -
 *     Pause quanta: - FC Reception - pause quanta to be used
 *     upon receiving link-level flow control via the BCT,
 *     ignored for other modes. - FC Generation - pause quanta
 *     to send. Units: 512 bit times (as defined in IEEE 802.3x
 *     standard). Range: 0 - 0xFFFF.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint32                       pause_quanta
  );

uint32
  soc_pb_fc_nif_pause_quanta_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint32                       pause_quanta
  );

uint32
  soc_pb_fc_nif_pause_quanta_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_nif_pause_quanta_set_unsafe" API.
 *     Refer to "soc_pb_fc_nif_pause_quanta_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pause_quanta
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_nif_pause_frame_src_addr_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAC Lane index. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr -
 *     Source MAC address, part of 802.3 pause frame.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );

uint32
  soc_pb_fc_nif_pause_frame_src_addr_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );

uint32
  soc_pb_fc_nif_pause_frame_src_addr_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_nif_pause_frame_src_addr_set_unsafe" API.
 *     Refer to "soc_pb_fc_nif_pause_frame_src_addr_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_vsq_by_incoming_nif_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable the mapping between the VSQ-Pointer and
 *   the Incoming-NIF port.
 * INPUT:
 *   SOC_SAND_IN  int unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8 enable -
 *     Enable/disable the mapping between the VSQ-Pointer and
 *     the Incoming-NIF port.
 * REMARKS:
 *   For PP customers, this API uses the Direct Table
 *   resources of the Field Processor module. If set, no
 *   other Direct Table based FP Databases can be created.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_set_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_vsq_by_incoming_nif_set_unsafe" API.
 *     Refer to "soc_pb_fc_vsq_by_incoming_nif_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *enable
  );

uint32
  SOC_PB_FC_GEN_INBND_CB_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_CB *info
  );

uint32
  SOC_PB_FC_GEN_INBND_LL_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_LL *info
  );

uint32
  SOC_PB_FC_GEN_INBND_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO *info
  );

uint32
  SOC_PB_FC_REC_INBND_CB_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_CB *info
  );

uint32
  SOC_PB_FC_REC_INBND_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO *info
  );

uint32
  SOC_PB_FC_REC_OFP_MAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO *info
  );

uint32
  SOC_PB_FC_RCY_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO *info
  );

uint32
  SOC_PB_FC_CAL_IF_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO *info,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE mode
  );

uint32
  SOC_PB_FC_CAL_REC_STAT_verify(
    SOC_SAND_IN  SOC_PB_FC_CAL_REC_STAT *info
  );

uint32
  SOC_PB_FC_GEN_CALENDAR_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR *info
  );

uint32
  SOC_PB_FC_REC_CALENDAR_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR *info
  );

uint32
  SOC_PB_FC_ILKN_LLFC_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO *info
  );

uint32
soc_pb_fc_gen_cal_src_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_GEN_CAL_SRC src_type
  );

#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_FLOW_CONTROL_INCLUDED__*/
#endif
