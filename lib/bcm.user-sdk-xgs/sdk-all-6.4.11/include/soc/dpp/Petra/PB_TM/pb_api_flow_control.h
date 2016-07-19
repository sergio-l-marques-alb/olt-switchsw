/* $Id: pb_api_flow_control.h,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_api_flow_control.h
*
* MODULE PREFIX:  soc_pb_fc
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

#ifndef __SOC_PB_API_FLOW_CONTROL_INCLUDED__
/* { */
#define __SOC_PB_API_FLOW_CONTROL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/petra_api_general.h>

#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Identifier for empty OOB Calendar Entry.                */
#define  SOC_PB_FC_OOB_CAL_EMPTY_SEL                  (0x3)

/*     Number of uint32s for FAP Port bitmap            */
#define  SOC_PB_FC_NOF_FAP_PORTS_UINT32S (3)

/*     Identifier for empty OOB Calendar Entry.                */
#define  SOC_PB_FC_OOB_CAL_EMPTY_ID                   (0x7F)

/*     The maximal index that can be used for calendar-based FC */

#define  SOC_PB_FC_OOB_CAL_HR_ID_MIN                  (128)

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

typedef SOC_TMC_FC_OFP_PRIORITY           SOC_PB_FC_OFP_PRIORITY;
typedef SOC_TMC_FC_CAL_MODE               SOC_PB_FC_CAL_MODE;
typedef SOC_TMC_FC_OOB_ID                 SOC_PB_FC_OOB_ID;
typedef SOC_TMC_FC_CAL_IF_INFO            SOC_PB_FC_CAL_IF_INFO;
typedef SOC_TMC_FC_GEN_CALENDAR           SOC_PB_FC_GEN_CALENDAR;
typedef SOC_TMC_FC_GEN_INBND_INFO         SOC_PB_FC_GEN_INBND_INFO;
typedef SOC_TMC_FC_INGR_GEN_GLB_HP_MODE   SOC_PB_FC_INGR_GEN_GLB_HP_MODE;
typedef SOC_TMC_FC_REC_CALENDAR           SOC_PB_FC_REC_CALENDAR;
typedef SOC_TMC_FC_GEN_INBND_CB           SOC_PB_FC_GEN_INBND_CB;
typedef SOC_TMC_FC_GEN_INBND_LL           SOC_PB_FC_GEN_INBND_LL;
typedef SOC_TMC_FC_REC_INBND_INFO         SOC_PB_FC_REC_INBND_INFO;
typedef SOC_TMC_FC_REC_INBND_CB           SOC_PB_FC_REC_INBND_CB;
typedef SOC_TMC_FC_REC_OFP_MAP_INFO       SOC_PB_FC_REC_OFP_MAP_INFO;

#define SOC_PB_FC_INBND_MODE_DISABLED     SOC_TMC_FC_INBND_MODE_DISABLED
#define SOC_PB_FC_INBND_MODE_LL           SOC_TMC_FC_INBND_MODE_LL
#define SOC_PB_FC_INBND_MODE_CB           SOC_TMC_FC_INBND_MODE_CB
#define SOC_PB_FC_NOF_INBND_MODES         SOC_TMC_FC_NOF_INBND_MODES_PB
typedef SOC_TMC_FC_INBND_MODE              SOC_PB_FC_INBND_MODE;

#define SOC_PB_FC_INBND_CB_INHERIT_DISABLED     SOC_TMC_FC_INBND_CB_INHERIT_DISABLED
#define SOC_PB_FC_INBND_CB_INHERIT_UP           SOC_TMC_FC_INBND_CB_INHERIT_UP
#define SOC_PB_FC_INBND_CB_INHERIT_DOWN         SOC_TMC_FC_INBND_CB_INHERIT_DOWN
#define SOC_PB_FC_NOF_INBND_CB_INHERITS         SOC_TMC_FC_NOF_INBND_CB_INHERITS
typedef SOC_TMC_FC_INBND_CB_INHERIT             SOC_PB_FC_INBND_CB_INHERIT;

typedef enum
{
  /*
   *  Flow Control Reception Reaction Point on OFP level -
   *  Scheduler Port. Note 1: this option is not recommended
   *  to stop the OFP! Typically, the OFP is stopped in the
   *  EGQ, which then backpressures the scheduler according to
   *  the EGQ FC threshold configuration. Note 2: this option
   *  can be used to stop HR 0 - 79, High or Low priority FC,
   *  also if the matching OFP is not used, and the HR
   *  scheduler is used inside the scheduling hierarchy.
   */
  SOC_PB_FC_REC_OFP_RP_SCH = 0x1,
  /*
   *  Flow Control Reception Reaction Point on OFP level -
   *  Egress Queues Manager (EGQ)
   */
  SOC_PB_FC_REC_OFP_RP_EGQ = 0x2,
  /*
   *  Number of types in SOC_PB_FC_REC_OFP_RP
   */
  SOC_PB_FC_NOF_REC_OFP_RPS = 3
}SOC_PB_FC_REC_OFP_RP;

typedef enum
{
  /*
   *  Recycling Flow Control on port-level is handled by end
   *  to end Scheduler.
   */
  SOC_PB_FC_RCY_OFP_HANDLER_SCH = 0,
  /*
   *  Recycling Flow Control on port-level is handled by the
   *  Egress Queuing block.
   */
  SOC_PB_FC_RCY_OFP_HANDLER_EGQ = 1,
  /*
   *  Number of types in SOC_PB_FC_RCY_OFP_HANDLER
   */
  SOC_PB_FC_NOF_RCY_OFP_HANDLERS = 2
}SOC_PB_FC_RCY_OFP_HANDLER;

typedef enum
{
  /*
   *  An Index used for Recycling FC Configuration based on
   *  Global Resources FC indication - Low Priority
   */
  SOC_PB_FC_RCY_GLB_RCS_ID_LP = 100,
  /*
   *  An Index used for Recycling FC Configuration based on
   *  Global Resources FC indication - High Priority
   */
  SOC_PB_FC_RCY_GLB_RCS_ID_HP = 200,
  /*
   *  Number of types in SOC_PB_FC_RCY_GLB_RCS_ID
   */
  SOC_PB_FC_NOF_RCY_GLB_RCS_IDS = 2
}SOC_PB_FC_RCY_GLB_RCS_ID;

#define SOC_PB_FC_GEN_CAL_SRC_STE           SOC_TMC_FC_GEN_CAL_SRC_STE
#define SOC_PB_FC_GEN_CAL_SRC_NIF           SOC_TMC_FC_GEN_CAL_SRC_NIF
#define SOC_PB_FC_GEN_CAL_SRC_GLB_HP        SOC_TMC_FC_GEN_CAL_SRC_GLB_HP
#define SOC_PB_FC_GEN_CAL_SRC_GLB_LP        SOC_TMC_FC_GEN_CAL_SRC_GLB_LP
#define SOC_PB_FC_GEN_CAL_SRC_NONE          SOC_TMC_FC_GEN_CAL_SRC_NONE
#define SOC_PB_FC_NOF_GEN_CAL_SRCS          SOC_TMC_FC_NOF_GEN_CAL_SRCS_PB
typedef SOC_TMC_FC_GEN_CAL_SRC               SOC_PB_FC_GEN_CAL_SRC;

typedef enum
{
  /*
   *  Global Resources Index- Buffer Descriptors
   */
  SOC_PB_FC_CAL_GLB_RCS_ID_BDB = 0,
  /*
   *  Global Resources Index- Unicast Data Buffers
   */
  SOC_PB_FC_CAL_GLB_RCS_ID_UNI = 1,
  /*
   *  Global Resources Index- Multicast Data Buffers
   */
  SOC_PB_FC_CAL_GLB_RCS_ID_MUL = 2,
  /*
   *  Total number of Global Resources Indexes.
   */
  SOC_PB_FC_NOF_CAL_GLB_RCS_IDS = 3
}SOC_PB_FC_CAL_GLB_RCS_ID;

#define SOC_PB_FC_REC_CAL_DEST_HR               SOC_TMC_FC_REC_CAL_DEST_HR
#define SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_LP       SOC_TMC_FC_REC_CAL_DEST_OFP_EGQ_LP
#define SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_HP       SOC_TMC_FC_REC_CAL_DEST_OFP_EGQ_HP
#define SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP    SOC_TMC_FC_REC_CAL_DEST_OFP_SCH_HR_LP
#define SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_HP    SOC_TMC_FC_REC_CAL_DEST_OFP_SCH_HR_HP
#define SOC_PB_FC_REC_CAL_DEST_NIF              SOC_TMC_FC_REC_CAL_DEST_NIF
#define SOC_PB_FC_REC_CAL_DEST_NONE             SOC_TMC_FC_REC_CAL_DEST_NONE
#define SOC_PB_FC_NOF_REC_CAL_DESTS             SOC_TMC_FC_NOF_REC_CAL_DESTS_PB
typedef SOC_TMC_FC_REC_CAL_DEST                  SOC_PB_FC_REC_CAL_DEST;

typedef SOC_TMC_FC_ILKN_CAL_LLFC SOC_PB_FC_ILKN_CAL_LLFC;

typedef enum
{
  /*
   *  No oversubscribtion on the NIF. Suitable for up to 100GE
   *  traffic expected on the device NIFs.
   */
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_NONE = 0,
  /*
   *  Oversubscribtion on the NIF - 120GE scheme 0. Suitable
   *  for up to 120GE traffic expected on the device NIFs.
   */
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_120GE_00 = 1,
  /*
   *  Oversubscribtion on the NIF - 160GE scheme 0. Suitable
   *  for up to 126GE traffic expected on the device NIFs.
   */
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_160GE_00 = 2,
  /*
   *  Number of types in SOC_PB_FC_NIF_OVERSUBSCR_SCHEME
   */
  SOC_PB_FC_NOF_NIF_OVERSUBSCR_SCHEMES = 3
}SOC_PB_FC_NIF_OVERSUBSCR_SCHEME;

typedef enum
{
  /*
   *  The number of FC-priorities per OFP: 2. This is the
   *  default configuration allowing High/Low OFP priority.
   *  Setting this value for an OFP-group "ungroups" it, i.e.
   *  each OFP represents a distinct destination with two
   *  priorities.
   */
  SOC_PB_FC_EXTEND_Q_NOF_PRIOS_2 = 0,
  /*
   *  The number of FC-priorities per OFP: 4. Consumes two
   *  consecutive OFP-s
   */
  SOC_PB_FC_EXTEND_Q_NOF_PRIOS_4 = 1,
  /*
   *  The number of FC-priorities per OFP: 6. Consumes three
   *  consecutive OFP-s
   */
  SOC_PB_FC_EXTEND_Q_NOF_PRIOS_6 = 2,
  /*
   *  The number of FC-priorities per OFP: 8. Consumes four
   *  consecutive OFP-s
   */
  SOC_PB_FC_EXTEND_Q_NOF_PRIOS_8 = 3
}SOC_PB_FC_EXTEND_Q_NOF_PRIOS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*  Bitmap of The Outgoing Fap Ports to accept the Flow
   *  Control (up to 80 ports). If the trigger is a VSQ, the
   *  bitmap must have only one destination FAP port.
   */
  uint32 ofp_bitmap[SOC_PB_FC_NOF_FAP_PORTS_UINT32S];

} SOC_PB_FC_RCY_PORT_INFO;

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

} SOC_PB_FC_CAL_REC_STAT;

typedef SOC_TMC_FC_ILKN_LLFC_INFO SOC_PB_FC_ILKN_LLFC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   * If set, then when OOB is in out-of-frame state 
   * (failed to lock, non-operational), it presents 
   * constant flow-control 'on' indication to the CFC core. 
   * Otherwise, then when in out-of-frame, a flow-control 
   * 'off' is presented. 
   */
  uint8 is_on_if_oof;
  /*
   * Determine the phase selection to sample the 
   * calendar signal. If set, sampled on the flow 
   * control clock rising edge. Otherwise, Stat is 
   * sampled on the flow control clock falling edge. 
   */
  uint8 is_sampled_rising_edge;

} SOC_PB_FC_PHY_PARAMS_INFO;

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
 *   soc_pb_fc_gen_inbnd_set
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
  soc_pb_fc_gen_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_gen_inbnd_set"
 *     API.
 *     Refer to "soc_pb_fc_gen_inbnd_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_INFO           *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_inbnd_glb_hp_set
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
  soc_pb_fc_gen_inbnd_glb_hp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_gen_inbnd_glb_hp_set" API.
 *     Refer to "soc_pb_fc_gen_inbnd_glb_hp_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_glb_hp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_inbnd_cnm_map_set
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
  soc_pb_fc_gen_inbnd_cnm_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_gen_inbnd_cnm_map_set" API.
 *     Refer to "soc_pb_fc_gen_inbnd_cnm_map_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rec_inbnd_set
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
  soc_pb_fc_rec_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_rec_inbnd_set"
 *     API.
 *     Refer to "soc_pb_fc_rec_inbnd_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_INFO           *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rec_inbnd_ofp_map_set
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
  soc_pb_fc_rec_inbnd_ofp_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rec_inbnd_ofp_map_set" API.
 *     Refer to "soc_pb_fc_rec_inbnd_ofp_map_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_get(
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
  soc_pb_fc_rcy_ofp_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_rcy_ofp_set"
 *     API.
 *     Refer to "soc_pb_fc_rcy_ofp_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_OUT SOC_PB_FC_RCY_PORT_INFO   *info
  );



/*********************************************************************
* NAME:
 *   soc_pb_fc_rcy_hr_enable_set
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
  soc_pb_fc_rcy_hr_enable_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_rcy_hr_enable_set" API.
 *     Refer to "soc_pb_fc_rcy_hr_enable_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint8                      *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_gen_cal_set
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
  soc_pb_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_gen_cal_set"
 *     API.
 *     Refer to "soc_pb_fc_gen_cal_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_GEN_CALENDAR             *cal_buff
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_rec_cal_set
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
  soc_pb_fc_rec_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_rec_cal_set"
 *     API.
 *     Refer to "soc_pb_fc_rec_cal_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_get(
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
  soc_pb_fc_oob_phy_params_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO          *phy_params
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_oob_phy_params_set"
 *     API.
 *     Refer to "soc_pb_fc_oob_phy_params_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT  SOC_PB_FC_PHY_PARAMS_INFO         *phy_params
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_egr_rec_oob_stat_get
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
  soc_pb_fc_egr_rec_oob_stat_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   oob_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT             *status
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_ilkn_llfc_set
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
  soc_pb_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_ilkn_llfc_set"
 *     API.
 *     Refer to "soc_pb_fc_ilkn_llfc_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *gen_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_nif_oversubscr_scheme_set
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
  soc_pb_fc_nif_oversubscr_scheme_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_nif_oversubscr_scheme_set" API.
 *     Refer to "soc_pb_fc_nif_oversubscr_scheme_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    *scheme
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_nif_pause_quanta_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAC Lane index. Range: 0 - 15.
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
  soc_pb_fc_nif_pause_quanta_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint32                       pause_quanta
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_nif_pause_quanta_set" API.
 *     Refer to "soc_pb_fc_nif_pause_quanta_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pause_quanta
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_nif_pause_frame_src_addr_set
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
  soc_pb_fc_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_nif_pause_frame_src_addr_set" API.
 *     Refer to "soc_pb_fc_nif_pause_frame_src_addr_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );

/*********************************************************************
* NAME:
 *   soc_pb_fc_vsq_by_incoming_nif_set
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
  soc_pb_fc_vsq_by_incoming_nif_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_fc_vsq_by_incoming_nif_set" API.
 *     Refer to "soc_pb_fc_vsq_by_incoming_nif_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *enable
  );

void
  SOC_PB_FC_GEN_INBND_CB_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_CB *info
  );

void
  SOC_PB_FC_GEN_INBND_LL_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_LL *info
  );

void
  SOC_PB_FC_GEN_INBND_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_INFO *info
  );

void
  SOC_PB_FC_REC_INBND_CB_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_CB *info
  );

void
  SOC_PB_FC_REC_INBND_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_INFO *info
  );

void
  SOC_PB_FC_REC_OFP_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_OFP_MAP_INFO *info
  );

void
  SOC_PB_FC_RCY_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_RCY_PORT_INFO *info
  );

void
  SOC_PB_FC_CAL_IF_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO *info
  );

void
  SOC_PB_FC_PHY_PARAMS_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_PHY_PARAMS_INFO *info
  );

void
  SOC_PB_FC_CAL_REC_STAT_clear(
    SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT *info
  );

void
  SOC_PB_FC_GEN_CALENDAR_clear(
    SOC_SAND_OUT SOC_PB_FC_GEN_CALENDAR *info
  );

void
  SOC_PB_FC_REC_CALENDAR_clear(
    SOC_SAND_OUT SOC_PB_FC_REC_CALENDAR *info
  );

void
  SOC_PB_FC_ILKN_LLFC_INFO_clear(
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_FC_OFP_PRIORITY_to_string(
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY enum_val
  );

const char*
  SOC_PB_FC_INBND_MODE_to_string(
    SOC_SAND_IN  SOC_PB_FC_INBND_MODE enum_val
  );

const char*
  SOC_PB_FC_INBND_CB_INHERIT_to_string(
    SOC_SAND_IN  SOC_PB_FC_INBND_CB_INHERIT enum_val
  );

const char*
  SOC_PB_FC_INGR_GEN_GLB_HP_MODE_to_string(
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE enum_val
  );

const char*
  SOC_PB_FC_REC_OFP_RP_to_string(
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_RP enum_val
  );

const char*
  SOC_PB_FC_RCY_OFP_HANDLER_to_string(
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER enum_val
  );

const char*
  SOC_PB_FC_RCY_GLB_RCS_ID_to_string(
    SOC_SAND_IN  SOC_PB_FC_RCY_GLB_RCS_ID enum_val
  );

const char*
  SOC_PB_FC_CAL_MODE_to_string(
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE enum_val
  );

const char*
  SOC_PB_FC_OOB_ID_to_string(
    SOC_SAND_IN  SOC_PB_FC_OOB_ID enum_val
  );

const char*
  SOC_PB_FC_GEN_CAL_SRC_to_string(
    SOC_SAND_IN  SOC_PB_FC_GEN_CAL_SRC enum_val
  );

const char*
  SOC_PB_FC_CAL_GLB_RCS_ID_to_string(
    SOC_SAND_IN  SOC_PB_FC_CAL_GLB_RCS_ID enum_val
  );

const char*
  SOC_PB_FC_REC_CAL_DEST_to_string(
    SOC_SAND_IN  SOC_PB_FC_REC_CAL_DEST enum_val
  );

const char*
  SOC_PB_FC_ILKN_CAL_LLFC_to_string(
    SOC_SAND_IN  SOC_PB_FC_ILKN_CAL_LLFC enum_val
  );

const char*
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_to_string(
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME enum_val
  );

const char*
  SOC_PB_FC_EXTEND_Q_NOF_PRIOS_to_string(
    SOC_SAND_IN  SOC_PB_FC_EXTEND_Q_NOF_PRIOS enum_val
  );

void
  SOC_PB_FC_GEN_INBND_CB_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_CB *info
  );

void
  SOC_PB_FC_GEN_INBND_LL_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_LL *info
  );

void
  SOC_PB_FC_GEN_INBND_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO *info
  );

void
  SOC_PB_FC_REC_INBND_CB_print(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_CB *info
  );

void
  SOC_PB_FC_REC_INBND_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO *info
  );

void
  SOC_PB_FC_PHY_PARAMS_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO *info
  );

void
  SOC_PB_FC_REC_OFP_MAP_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO *info
  );

void
  SOC_PB_FC_RCY_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO *info
  );

void
  SOC_PB_FC_CAL_IF_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO *info
  );

void
  SOC_PB_FC_CAL_REC_STAT_print(
    SOC_SAND_IN  SOC_PB_FC_CAL_REC_STAT *info
  );

void
  SOC_PB_FC_GEN_CALENDAR_print(
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR *info
  );

void
  SOC_PB_FC_REC_CALENDAR_print(
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR *info
  );

void
  SOC_PB_FC_ILKN_LLFC_INFO_print(
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_FLOW_CONTROL_INCLUDED__*/
#endif

