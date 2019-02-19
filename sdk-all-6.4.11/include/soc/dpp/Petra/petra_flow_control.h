/* $Id: petra_flow_control.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_flow_control.h
*
* MODULE PREFIX:  soc_petra_fc
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


#ifndef __SOC_PETRA_FLOW_CONTROL_INCLUDED__
/* { */
#define __SOC_PETRA_FLOW_CONTROL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_flow_control.h>
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
*     soc_petra_flow_control_init
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
  soc_petra_flow_control_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_vsq_via_nif_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info -
*     Flow Control configuration.
* REMARKS:
*     1. Using Link Level and Class Based flow control at the
*     same time is not according to the standard, though
*     supported by Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_vsq_via_nif_verify
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info -
*     Flow Control configuration.
* REMARKS:
*     1. Using Link Level and Class Based flow control at the
*     same time is not according to the standard, though
*     supported by Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_vsq_via_nif_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info -
*     Flow Control configuration.
* REMARKS:
*     1. Using Link Level and Class Based flow control at the
*     same time is not according to the standard, though
*     supported by Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_hp_via_nif_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable Flow Control generation, based on Ingress
*     Global Resources - high priority, via NIF. Flow Control
*     generation may be either Link Level or Class Based. For
*     Link Level - Flow Control will be generated on all
*     links. For Class Based - Flow Control will be generated
*     on all Flow Control Classes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode -
*     Flow Control mode. If enabled, when Ingress Global
*     Resources high priority Flow Control is indicated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fc_ingr_gen_glb_hp_via_nif_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_hp_via_nif_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable Flow Control generation, based on Ingress
*     Global Resources - high priority, via NIF. Flow Control
*     generation may be either Link Level or Class Based. For
*     Link Level - Flow Control will be generated on all
*     links. For Class Based - Flow Control will be generated
*     on all Flow Control Classes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode -
*     Flow Control mode. If enabled, when Ingress Global
*     Resources high priority Flow Control is indicated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fc_ingr_gen_glb_hp_via_nif_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_hp_via_nif_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable Flow Control generation, based on Ingress
*     Global Resources - high priority, via NIF. Flow Control
*     generation may be either Link Level or Class Based. For
*     Link Level - Flow Control will be generated on all
*     links. For Class Based - Flow Control will be generated
*     on all Flow Control Classes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE *fc_mode -
*     Flow Control mode. If enabled, when Ingress Global
*     Resources high priority Flow Control is indicated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fc_ingr_gen_glb_hp_via_nif_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE *fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  uint32                 cls_ndx -
*     Defines Flow Control Class/Classes on which the Flow
*     Control will be generated upon Global Resources Low
*     Priority FC indication. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info -
*     Flow Control configuration.
* REMARKS:
*     1. This configuration is applicable to Class Based Flow
*     Control only. 2. For SGMII interface,
*     info.class_and_below must be TRUE.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  uint32                 cls_ndx -
*     Defines Flow Control Class/Classes on which the Flow
*     Control will be generated upon Global Resources Low
*     Priority FC indication. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info -
*     Flow Control configuration.
* REMARKS:
*     1. This configuration is applicable to Class Based Flow
*     Control only. 2. For SGMII interface,
*     info.class_and_below must be TRUE.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  uint32                 cls_ndx -
*     Defines Flow Control Class/Classes on which the Flow
*     Control will be generated upon Global Resources Low
*     Priority FC indication. Range: 0 - 7.
*  SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info -
*     Flow Control configuration.
* REMARKS:
*     1. This configuration is applicable to Class Based Flow
*     Control only. 2. For SGMII interface,
*     info.class_and_below must be TRUE.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_oob_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf -
*     Out Of Band (OOB) interface configuration -
*     enable/disable, calendar configuration - length and
*     repetitions.
*  SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff -
*     A buffer with oob_conf.cal_len entries. Each entry
*     configures a calendar channel, defining the source of
*     the Flow Control that controls this channel.
* REMARKS:
*     1. The OOB interface that is configured is
*     SOC_PETRA_FC_OOB_ID_B, since only OOB B can be used for Flow
*     Control generation. 2. For global resources - use
*     indexes defined in SOC_PETRA_FC_OOB_GLB_RCS_ID.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_oob_verify
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf -
*     Out Of Band (OOB) interface configuration -
*     enable/disable, calendar configuration - length and
*     repetitions.
*  SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff -
*     A buffer with oob_conf.cal_len entries. Each entry
*     configures a calendar channel, defining the source of
*     the Flow Control that controls this channel.
* REMARKS:
*     1. The OOB interface that is configured is
*     SOC_PETRA_FC_OOB_ID_B, since only OOB B can be used for Flow
*     Control generation. 2. For global resources - use
*     indexes defined in SOC_PETRA_FC_OOB_GLB_RCS_ID.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_oob_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf -
*     Out Of Band (OOB) interface configuration -
*     enable/disable, calendar configuration - length and
*     repetitions.
*  SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff -
*     A buffer with oob_conf.cal_len entries.
*     If actual cal_len is unknown when calling the API,
*     call_buff must be of maximal calendar size - 512. Each entry
*     configures a calendar channel, defining the source of
*     the Flow Control that controls this channel.
* REMARKS:
*     1. The OOB interface that is configured is
*     SOC_PETRA_FC_OOB_ID_B, since only OOB B can be used for Flow
*     Control generation.
*     2. For global resources - use
*     indexes defined in SOC_PETRA_FC_OOB_GLB_RCS_ID.
*     3. If number of repetitions (oob_conf.cal_rep) is more then one,
*     only first repetition will be read into the cal_buff buffer.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_stat_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Diagnostics - get the Out-Of-Band interface status
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx -
*     OOB Interface index. Range: 0 - 1.
*  SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *status -
*     The status of the OOB interface.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_fc_egr_rec_oob_stat_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *status
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx -
*     OOB Interface index. Range: 0 - 1.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf -
*     Out Of Band (OOB) interface configuration -
*     enable/disable, calendar configuration - length and
*     repetitions.
*  SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff -
*     A buffer with oob_conf.cal_len entries. Each entry
*     configures a calendar channel, defining the destination
*     that handles the Flow Control from this channel.
* REMARKS:
*     If the same SCH-based OFP HR appears both in low and high
*     priority in the calendar, the get function will not be
*     symmetric since the returned calendar will indicate for the
*     first entry like this low priority, and high priority for
*     the next entries with this OFP HR index.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_verify
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx -
*     OOB Interface index. Range: 0 - 1.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf -
*     Out Of Band (OOB) interface configuration -
*     enable/disable, calendar configuration - length and
*     repetitions.
*  SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff -
*     A buffer with oob_conf.cal_len entries. Each entry
*     configures a calendar channel, defining the destination
*     that handles the Flow Control from this channel.
* REMARKS:
*     If the same SCH-based OFP HR appears both in low and high
*     priority in the calendar, the get function will not be
*     symmetric since the returned calendar will indicate for the
*     first entry like this low priority, and high priority for
*     the next entries with this OFP HR index.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx -
*     OOB Interface index. Range: 0 - 1.
*  SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf -
*     Out Of Band (OOB) interface configuration -
*     enable/disable, calendar configuration - length and
*     repetitions.
*  SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff -
*     A buffer with oob_conf.cal_len entries. Each entry
*     configures a calendar channel, defining the destination
*     that handles the Flow Control from this channel.
* REMARKS:
*     If the same SCH-based OFP HR appears both in low and high
*     priority in the calendar, the get function will not be
*     symmetric since the returned calendar will indicate for the
*     first entry like this low priority, and high priority for
*     the next entries with this OFP HR index.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_vsq_ofp_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 cc_ndx -
*     Category 3 Connection Class to map. The Connection Class
*     will usually represent Recycling Outgoing FAP Port, that
*     will receive Flow Control triggered by crossing the Flow
*     Control threshold of Category 3 Ingress Statistics
*     Queue. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info -
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_vsq_ofp_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 cc_ndx -
*     Category 3 Connection Class to map. The Connection Class
*     will usually represent Recycling Outgoing FAP Port, that
*     will receive Flow Control triggered by crossing the Flow
*     Control threshold of Category 3 Ingress Statistics
*     Queue. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info -
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_vsq_ofp_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 cc_ndx -
*     Category 3 Connection Class to map. The Connection Class
*     will usually represent Recycling Outgoing FAP Port, that
*     will receive Flow Control triggered by crossing the Flow
*     Control threshold of Category 3 Ingress Statistics
*     Queue. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO    *info -
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_glb_ofp_hr_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     Outgoing FAP port index of the port HR. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode -
*     The Flow Control mode - enable/disable, and Flow Control
*     priority if enabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_glb_ofp_hr_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     Outgoing FAP port index of the port HR. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode -
*     The Flow Control mode - enable/disable, and Flow Control
*     priority if enabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_glb_ofp_hr_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     Outgoing FAP port index of the port HR. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_FC_ON_GLB_RCS_MODE  *fc_mode -
*     The Flow Control mode - enable/disable, and Flow Control
*     priority if enabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_ON_GLB_RCS_MODE  *fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_hr_enable_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     enable/disable FC reception by HR 192-223 (recycling
*     HR-s).
* REMARKS:
*     Connection Classes 68-99 are mapped 1-1 to HR 192-223.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_hr_enable_verify
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     enable/disable FC reception by HR 192-223 (recycling
*     HR-s).
* REMARKS:
*     Connection Classes 68-99 are mapped 1-1 to HR 192-223.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_hr_enable_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Mar 14 2008
* FUNCTION:
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8                 *enable -
*     enable/disable FC reception by HR 192-223 (recycling
*     HR-s).
* REMARKS:
*     Connection Classes 68-99 are mapped 1-1 to HR 192-223.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_FLOW_CONTROL_INCLUDED__*/
#endif
