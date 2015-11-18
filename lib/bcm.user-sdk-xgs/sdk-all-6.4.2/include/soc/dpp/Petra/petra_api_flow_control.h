/* $Id: petra_api_flow_control.h,v 1.8 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_FLOW_CONTROL_INCLUDED__
/* { */
#define __SOC_PETRA_API_FLOW_CONTROL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal length of Out Of Bound interface calendar.      */
#define  SOC_PETRA_FC_OOB_CAL_LEN_MAX SOC_TMC_FC_OOB_CAL_LEN_MAX

#define  SOC_PETRA_FC_OOB_CAL_REP_MIN SOC_TMC_FC_OOB_CAL_REP_MIN
#define  SOC_PETRA_FC_OOB_CAL_REP_MAX SOC_TMC_FC_OOB_CAL_REP_MAX

/* $Id: petra_api_flow_control.h,v 1.8 Broadcom SDK $
 *  Out Of Band Flow Control Reception:
 *  Minimal/Maximal index for non-OFP HR scheduler to handle the Flow Control indication.
 *  Note: OFP-HRs (0-79) can also handle Flow Control indication, High/Low priority.
 */
#define  SOC_PETRA_FC_OOB_CAL_HR_ID_MIN 128
#define  SOC_PETRA_FC_OOB_CAL_HR_ID_MAX 255

/*     Identifier for empty OOB Calendar Entry.                */
#define  SOC_PETRA_FC_OOB_CAL_EMPTY_SEL 0x3

/*     Identifier for empty OOB Calendar Entry.                */
#define  SOC_PETRA_FC_OOB_CAL_EMPTY_ID 0x7F

/*     Maximal weight for building Out Of Band Flow Control
 *     Calendar.                                               */

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
   *  Low priority Flow Control.
   */
  SOC_PETRA_FC_PRIORITY_LOW=0,
  /*
   *  High priority Flow Control.
   */
  SOC_PETRA_FC_PRIORITY_HIGH=1,
  /*
   *  Total number of Flow Control priorities.
   */
  SOC_PETRA_FC_NOF_PRIORITYS=2
}SOC_PETRA_FC_PRIORITY;


typedef enum
{
  /*
   *  Soc_petra Out Of Band Flow Control Interface A. Used for Flow
   *  Control reception.
   */
  SOC_PETRA_FC_OOB_ID_A=0,
  /*
   *  Soc_petra Out Of Band Flow Control Interface B. Used for Flow
   *  Control reception or generation.
   */
  SOC_PETRA_FC_OOB_ID_B=1,
  /*
   *  Total Number of Soc_petra Out Of Band Flow Control
   *  Interfaces
   */
  SOC_PETRA_FC_NOF_OOB_IFS=2
}SOC_PETRA_FC_OOB_ID;


typedef enum
{
  /*
   *  Disable Ingress Flow Control generation via NIF, Link
   *  Level.
   */
  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_DISABLE=0,
  /*
   *  Enable Ingress Flow Control generation based on
   *  Statistics Flag - STF, (VSQ 164-195) via NIF, Link
   *  Level. Statistics Flag represents the link on which FC
   *  will be generated.
   */
  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_ENABLE=1,
  /*
   *  Total number of Ingress Flow Control generation via NIF
   *  enable states.
   */
  SOC_PETRA_FC_INGR_NOF_GEN_NIF_LL_STATES=2
}SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE;


typedef enum
{
  /*
   *  Enable Ingress Flow Control generation via NIF based on
   *  Statistics Flag - STF (VSQ 100-163) via NIF, Class
   *  Based. Statistics Flag represents the NIF x Flow Control
   *  Class (FCC). For XAUI/SPAUI: NIF in range 0..7 (MAC
   *  lane), FCC in range 0..7. For SGMII: NIF in range 0..31
   *  (MAC lane), FCC in range 0..1.
   */
  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_STF=0,
  /*
   *  Enable Ingress Flow Control generation via NIF based on
   *  Category 2-3 (CT) and Connection Class (CC) (VSQ 36-99)
   *  via NIF, Class Based. CTxCC represent the NIF x Flow
   *  Control Class (FCC). For XAUI/SPAUI: NIF in range 0..7
   *  (MAC lane), FCC in range 0..7. For SGMII: NIF in range
   *  0..31 (MAC lane), FCC in range 0..1.
   */
  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_CC=1,
  /*
   *  Total number of Ingress Flow Control generation via NIF
   *  enable states.
   */
  SOC_PETRA_FC_INGR_NOF_GEN_NIF_CB_STATES=2
}SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE;


typedef enum
{
  /*
   *  Disable Ingress Flow Control generation via NIF, upon
   *  Global Resources High Priority FC indication.
   */
  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_NONE=0,
  /*
   *  Enable Ingress Flow Control generation via NIF, upon
   *  Global Resources High Priority FC indication, Link
   *  Level.
   */
  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_LL=1,
  /*
   *  Enable Ingress Flow Control generation via NIF, upon
   *  Global Resources High Priority FC indication, Class
   *  Based.
   */
  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_CB=2,
  /*
   *  Total number of Ingress Flow Control generation via NIF
   *  modes.
   */
  SOC_PETRA_FC_NOF_INGR_GEN_GLB_HP_MODES=3
}SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE;


typedef enum
{
  /*
   *  Soc_petra Out Of Band Flow Control source - Ingress
   *  Statistics Engine. Corresponds to VSQs 0 - 355.
   */
  SOC_PETRA_FC_GEN_OOB_SRC_STE=0,
  /*
   *  Soc_petra Out Of Band Flow Control source - Network
   *  Interface.
   */
  SOC_PETRA_FC_GEN_OOB_SRC_NIF=1,
  /*
   *  Soc_petra Out Of Band Flow Control source - Global Resource,
   *  High Priority FC.
   */
  SOC_PETRA_FC_GEN_OOB_SRC_GLB_HP=2,
  /*
   *  Soc_petra Out Of Band Flow Control source - Global Resource,
   *  Low Priority FC.
   */
  SOC_PETRA_FC_GEN_OOB_SRC_GLB_LP=3,
  /*
   *  Invalid/non-existing source. Serves as "empty calendar
   *  entry" indication.
   */
  SOC_PETRA_FC_GEN_OOB_SRC_NONE=4,
  /*
   *  Total number of Of Band Flow Control sources.
   */
  SOC_PETRA_FC_NOF_GEN_OOB_SRCS=5
}SOC_PETRA_FC_GEN_OOB_SRC;


typedef enum
{
  /*
   *  Global Resources Index- Buffer Descriptors
   */
  SOC_PETRA_FC_OOB_GLB_RCS_ID_BDB=0,
  /*
   *  Global Resources Index- Unicast Data Buffers
   */
  SOC_PETRA_FC_OOB_GLB_RCS_ID_UNI=1,
  /*
   *  Global Resources Index- Multicast Data Buffers
   */
  SOC_PETRA_FC_OOB_GLB_RCS_ID_MUL=2,
  /*
   *  Total number of Global Resources Indexes.
   */
  SOC_PETRA_FC_NOF_OOB_GLB_RCS_IDS=3
}SOC_PETRA_FC_OOB_GLB_RCS_ID;


typedef enum
{
  /*
   *  Soc_petra Out Of Band Flow Control destination - HR
   *  scheduling element.
   */
  SOC_PETRA_FC_REC_OOB_DEST_HR=0,
  /*
   *  Soc_petra Out Of Band Flow Control destination - Outgoing
   *  FAP Port, Egress (EGQ)
   */
  SOC_PETRA_FC_REC_OOB_DEST_OFP_EGQ=1,
  /*
   *  Soc_petra Out Of Band Flow Control destination - Outgoing
   *  FAP Port, HR scheduling element, Low Priority Flow
   *  Control.
   */
  SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP=2,
  /*
   *  Soc_petra Out Of Band Flow Control destination - Outgoing
   *  FAP Port, HR scheduling element, High Priority Flow
   *  Control.
   */
  SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP=3,
  /*
   *  Soc_petra Out Of Band Flow Control destination - Network
   *  Interface.
   */
  SOC_PETRA_FC_REC_OOB_DEST_NIF=4,
  /*
   *  Invalid/non-existing destination. Serves as "empty
   *  calendar entry" indication.
   */
  SOC_PETRA_FC_REC_OOB_DEST_NONE=5,
  /*
   *  Total number of Of Band Flow Control destinations.
   */
  SOC_PETRA_FC_NOF_REC_OOB_DESTS=6
}SOC_PETRA_FC_REC_OOB_DEST;


typedef enum
{
  /*
   *  Recycling Flow Control on port-level is not handled.
   */
  SOC_PETRA_FC_RCY_OFP_HANDLER_NONE=0,
  /*
   *  Recycling Flow Control on port-level is handled by end
   *  to end Scheduler as Low Priority FC.
   */
  SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_LP=1,
  /*
   *  Recycling Flow Control on port-level is handled by end
   *  to end Scheduler as High Priority FC.
   */
  SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_HP=2,
  /*
   *  Recycling Flow Control on port-level is handled by the
   *  Egress Queuing block.
   */
  SOC_PETRA_FC_RCY_OFP_HANDLER_EGQ=3,
  /*
   *  Recycling Flow Control on port-level is handled by end
   *  to end Scheduler as Low Priority FC.
   */
  SOC_PETRA_FC_NOF_RCY_OFP_HANDLERS=4
}SOC_PETRA_FC_RCY_OFP_HANDLER;


typedef enum
{
  /*
   *  Do not indicate Flow Control to Scheduler port HR when
   *  Global Resources (Low Priority) Flow Control threshold
   *  is crossed.
   */
  SOC_PETRA_FC_ON_GLB_RCS_MODE_DISABLE=0,
  /*
   *  Indicate Low Priority Flow Control to Scheduler port HR
   *  when Global Resources (Low Priority) Flow Control
   *  threshold is crossed.
   */
  SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_LP=1,
  /*
   *  Indicate High Priority Flow Control to Scheduler port HR
   *  when Global Resources (Low Priority) Flow Control
   *  threshold is crossed.
   */
  SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_HP=2,
  /*
   *  Total number of Flow Control on global resources
   *  modes. Note: applicable for recycling Flow Control.
   */
  SOC_PETRA_FC_NOF_ON_GLB_RCS_MODES=3
}SOC_PETRA_FC_ON_GLB_RCS_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/Disable the OOB interface for FC generation (TX)
   *  or reception (RX). TX enable is only valid for OOB
   *  interface B.
   */
  uint8 enable;
  /*
   *  Number of channels in calendar. Range: 0 - 511.
   */
  uint32 cal_len;
  /*
   *  The number of calendar repetitions within a status
   *  frame. The actual Calendar is composed of adjacent
   *  sections of 'cal_len' length.'cal_len' * 'cal_reps' must
   *  not exceed 512 (total calendar length). Range: 1 - 15.
   */
  uint32 cal_reps;
}SOC_PETRA_FC_OOB_IF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interface not locked. Did not find a correct framing
   *  pattern.
   */
  uint8 not_locked;
  /*
   *  Out-Of-Frame error counter is above OutOfFrmTh (3 by
   *  default).
   */
  uint8 out_of_frame;
  /*
   *  DIP2 error counter is above Dip2AlrmTh (3 by default).
   */
  uint8 dip2_alarm;
  /*
   *  Number of frame errors since last read.
   */
  uint32 nof_frame_errors;
  /*
   *  Number ofDIP2 errors since last read.
   */
  uint32 nof_dip2_errors;
}SOC_PETRA_FC_EGR_REC_OOB_STAT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Controls enabling Ingress Flow Control generation via
   *  NIF, Link Level.
   */
  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE ll_enable_state;
  /*
   *  Controls Ingress Flow Control generation via NIF, Class
   *  Based. Note: effective only if Class Based flow control
   *  is enabled at the NIF.
   */
  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE cb_enable_state;
}SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Controls enabling Ingress Flow Control generation via
   *  NIF, upon Global Resources Low Priority FC indication,
   *  Class Based.
   */
  uint8 enable;
  /*
   *  If set, the 'class_for_fc' defines the highest Flow
   *  Control Class on which the Flow Control will be
   *  generated upon Global Resources Low Priority FC
   *  indication. Value k means that Flow Control will be
   *  generated on Flow Control Classes 0-k. Otherwise - only
   *  on Class k. Note: must be TRUE for SGMII interface.
   */
  uint8 class_and_below;
}SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Soc_petra Out Of Band Flow Control source.
   */
  SOC_PETRA_FC_GEN_OOB_SRC source;
  /*
   *  The index of the entity (STE/NIF/global source high/low
   *  priority), according to the source specified), to
   *  trigger the appropriate OOB calendar entry for FC.
   */
  uint32 id;
}SOC_PETRA_FC_GEN_OOB_CAL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The trigger (source) for FC indication.
   */
  SOC_PETRA_FC_GEN_OOB_CAL_INFO conf;
  /*
   *  A relative weight reflecting the number of channels that
   *  will be dedicated to the specified FC source. Range: 1-
   *  FC_OOB_WEIGHT_MAX
   */
  uint32 weight;
}SOC_PETRA_FC_GEN_OOB_CAL_INPT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Soc_petra Out Of Band Flow Control destination.
   */
  SOC_PETRA_FC_REC_OOB_DEST destination;
  /*
   *  The index of the entity (HR/OFP/link, according to the
   *  destination specified), that will receive the Flow
   *  Control indication when the appropriate OOB calendar
   *  entry is triggered for FC.
   */
  uint32 id;
}SOC_PETRA_FC_REC_OOB_CAL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The handler (destination) for FC indication.
   */
  SOC_PETRA_FC_REC_OOB_CAL_INFO conf;
  /*
   *  A relative weight reflecting the number of channels that
   *  will be dedicated to the specified FC destination. Range:
   *  1- FC_OOB_WEIGHT_MAX
   */
  uint32 weight;
}SOC_PETRA_FC_REC_OOB_CAL_INPT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If not NONE, selects the way the Flow Control will be
   *  handled - by the scheduler (prioritized) or at the
   *  egress (EGQ).
   */
  SOC_PETRA_FC_RCY_OFP_HANDLER handler;
  /*
   *  The Outgoing Fap Port to accept the Flow Control. Range:
   *  0-79.
   */
  SOC_PETRA_FAP_PORT_ID ofp_id;
}SOC_PETRA_FC_RCY_PORT_INFO;


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
*     soc_petra_fc_ingr_gen_vsq_via_nif_set
* TYPE:
*   PROC
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
  soc_petra_fc_ingr_gen_vsq_via_nif_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_vsq_via_nif_get
* TYPE:
*   PROC
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
  soc_petra_fc_ingr_gen_vsq_via_nif_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_hp_via_nif_set
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_hp_via_nif_get
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE *fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set
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
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get
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
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_oob_set
* TYPE:
*   PROC
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
  soc_petra_fc_ingr_gen_oob_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_ingr_gen_oob_get
* TYPE:
*   PROC
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
  soc_petra_fc_ingr_gen_oob_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_stat_get
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
  soc_petra_fc_egr_rec_oob_stat_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *status
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_set
* TYPE:
*   PROC
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
  soc_petra_fc_egr_rec_oob_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_egr_rec_oob_get
* TYPE:
*   PROC
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
  soc_petra_fc_egr_rec_oob_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_vsq_ofp_set
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
  soc_petra_fc_rcy_vsq_ofp_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_vsq_ofp_get
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
  soc_petra_fc_rcy_vsq_ofp_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_glb_ofp_hr_set
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
  soc_petra_fc_rcy_glb_ofp_hr_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_glb_ofp_hr_get
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
  soc_petra_fc_rcy_glb_ofp_hr_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_ON_GLB_RCS_MODE  *fc_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_hr_enable_set
* TYPE:
*   PROC
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
  soc_petra_fc_rcy_hr_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_fc_rcy_hr_enable_get
* TYPE:
*   PROC
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
  soc_petra_fc_rcy_hr_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );


void
  soc_petra_PETRA_FC_OOB_IF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO *info
  );

void
  soc_petra_PETRA_FC_EGR_REC_OOB_STAT_clear(
    SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *info
  );

void
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );

void
  soc_petra_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );

void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *info
  );

void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INPT_clear(
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INPT *info
  );

void
  soc_petra_PETRA_FC_REC_OOB_CAL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *info
  );

void
  soc_petra_PETRA_FC_REC_OOB_CAL_INPT_clear(
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INPT *info
  );

void
  soc_petra_PETRA_FC_RCY_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_FC_PRIORITY_to_string(
    SOC_SAND_IN SOC_PETRA_FC_PRIORITY enum_val
  );



const char*
  soc_petra_PETRA_FC_OOB_ID_to_string(
    SOC_SAND_IN SOC_PETRA_FC_OOB_ID enum_val
  );



const char*
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE enum_val
  );



const char*
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE enum_val
  );



const char*
  soc_petra_PETRA_FC_INGR_GEN_GLB_HP_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE enum_val
  );



const char*
  soc_petra_PETRA_FC_GEN_OOB_SRC_to_string(
    SOC_SAND_IN SOC_PETRA_FC_GEN_OOB_SRC enum_val
  );



const char*
  soc_petra_PETRA_FC_OOB_GLB_RCS_ID_to_string(
    SOC_SAND_IN SOC_PETRA_FC_OOB_GLB_RCS_ID enum_val
  );



const char*
  soc_petra_PETRA_FC_REC_OOB_DEST_to_string(
    SOC_SAND_IN SOC_PETRA_FC_REC_OOB_DEST enum_val
  );



const char*
  soc_petra_PETRA_FC_RCY_OFP_HANDLER_to_string(
    SOC_SAND_IN SOC_PETRA_FC_RCY_OFP_HANDLER enum_val
  );



const char*
  soc_petra_PETRA_FC_ON_GLB_RCS_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_FC_ON_GLB_RCS_MODE enum_val
  );



void
  soc_petra_PETRA_FC_OOB_IF_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_OOB_IF_INFO *info
  );



void
  soc_petra_PETRA_FC_EGR_REC_OOB_STAT_print(
    SOC_SAND_IN SOC_PETRA_FC_EGR_REC_OOB_STAT *info
  );



void
  soc_petra_PETRA_FC_INGR_GEN_VSQ_NIF_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  );



void
  soc_petra_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  );



void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_GEN_OOB_CAL_INFO *info
  );



void
  soc_petra_PETRA_FC_GEN_OOB_CAL_INPT_print(
    SOC_SAND_IN SOC_PETRA_FC_GEN_OOB_CAL_INPT *info
  );



void
  soc_petra_PETRA_FC_REC_OOB_CAL_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_REC_OOB_CAL_INFO *info
  );



void
  soc_petra_PETRA_FC_REC_OOB_CAL_INPT_print(
    SOC_SAND_IN SOC_PETRA_FC_REC_OOB_CAL_INPT *info
  );



void
  soc_petra_PETRA_FC_RCY_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_FC_RCY_PORT_INFO *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_FLOW_CONTROL_INCLUDED__*/
#endif
