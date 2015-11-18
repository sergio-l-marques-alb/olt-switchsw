
/* $Id: arad_api_flow_control.h,v 1.14 Broadcom SDK $
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

#ifndef __ARAD_API_FLOW_CONTROL_INCLUDED__
/* { */
#define __ARAD_API_FLOW_CONTROL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>
#include <soc/dpp/ARAD/arad_api_nif.h>


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
   *  OFP FC priority - No FC
   */
  ARAD_FC_OFP_PRIORITY_NONE = 0x0,
  /*
   *  OFP FC priority - Low
   */
  ARAD_FC_OFP_PRIORITY_LP = 0x1,
  /*
   *  OFP FC priority - High. HP-FC triggers also LP-FC
   */
  ARAD_FC_OFP_PRIORITY_HP = 0x3,
  /*
   *  Number of types in ARAD_FC_OFP_PRIORITY
   */
  ARAD_FC_NOF_OFP_PRIORITYS = 3
}ARAD_FC_OFP_PRIORITY;

typedef enum
{
  /* Arad ILKN Interface ID A */
  ARAD_FC_ILKN_ID_A = 0,

  /* Arad ILKN Interface ID B */
  ARAD_FC_ILKN_ID_B = 1,

  /* Arad number of ILKN Interfaces */
  ARAD_FC_ILKN_IDS = 2
}ARAD_FC_ILKN_ID;

typedef enum
{
  /* Reception Calendar Type */
  ARAD_FC_CAL_TYPE_RX = 0,

  /* Generation Calendar Type */
  ARAD_FC_CAL_TYPE_TX = 1,

  /* Calendar Types */
  ARAD_FC_CAL_TYPES = 2
}ARAD_FC_CAL_TYPE;

typedef SOC_TMC_FC_ILKN_CAL_LLFC ARAD_FC_ILKN_CAL_LLFC;

typedef SOC_TMC_FC_ILKN_LLFC_INFO ARAD_FC_ILKN_LLFC_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /* Enable the ILKN Retransmit option */
  uint32 enable;

  /* RX Polarity of the retransmit indication
   * 0 - Normal
   * 1 - Inverted */
  uint32 rx_polarity;
  /* TX Polarity of the retransmit indication
   * 0 - Normal
   * 1 - Inverted */
  uint32 tx_polarity;

  /* Raise en error indication upon CRC errors */
  uint32 error_indication;

}ARAD_FC_ILKN_RETRANSMIT_INFO;

#if defined(BCM_88650_B0)
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /* Enable the ILKN Retransmit calendar */
  uint32 enable;

  /* Calendar length (0-2) */
  uint32 length;

}ARAD_FC_ILKN_RETRANSMIT_CAL_CFG;
#endif

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /* Enable the Scheduler OOB Watchdog */
  uint32 enable;

  /* When no good message was received during the last configured period,
   * an error indication is raised  */
  uint32 period;

  /* Enabling raising an error when no good message 
   * was received during the last period */
  uint32 error_indication;

  /* Raise an error on the first CRC error */
  uint32 crc_error;

}ARAD_FC_SCH_OOB_WD_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /* Enable the Scheduler OOB Interface 
   * (otherwise the FC indication won't pass to the EGQ) */
  uint32 enable;

  /* The CL range of the FC indication vector
   * The actual range is from 0 to the given value
   * This should be equal to the calendar length of the SCH OOB Interface
   * Max value - 4K-1 */
  uint32 sch_range;

  /* Hold the WatchDog info for the SCH OOB Interface */
  ARAD_FC_SCH_OOB_WD_INFO wd_info;

}ARAD_FC_SCH_OOB_INFO;

typedef struct  
{
  SOC_SAND_MAGIC_NUM_VAR

  /* The queue pair for which to set the PFC Bitmap
   * Range: 0-255 */
  uint32 queue_pair;

  /* The PFC Bitmap: 8 bits, one for each TC */
  uint8 pfc_bm;

}ARAD_FC_PFC_BM_INFO;

typedef enum  
{
  ARAD_FC_OOB_TX_SPEED_CORE_2,
  ARAD_FC_OOB_TX_SPEED_CORE_4,
  ARAD_FC_OOB_TX_SPEED_CORE_8
}ARAD_FC_OOB_TX_SPEED;


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

void 
  ARAD_FC_ILKN_RETRANSMIT_INFO_clear(
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_INFO *info
  );

#if defined(BCM_88650_B0)
void 
  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG_clear(
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG *info
  );
#endif


int arad_fc_shr_mapping(int unit, int fcv_bit, int cl_index, int select, int valid);
int arad_fc_init_shr_mapping(int unit);

uint32
  arad_fc_init_pfc_mapping(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
* NAME:
 *   arad_fc_pfc_timer_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Generation PFC refresh timer Configuration
 *
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_TMC_FC_PFC_TIMER_INFO           *info -
 *      refresh timer configuration.
 * REMARKS:
 *  Only applicable for PFC configuration (see arad_fc_gen_inbnd_set_unsafe)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/

uint32
  arad_fc_pfc_timer_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_PFC_TIMER_INFO   *info
  );

uint32
  arad_fc_pfc_timer_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_TIMER_INFO   *info
  );


/*********************************************************************
* NAME:
 *   arad_fc_gen_inbnd_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Generation Configuration, based on
 *   Arad Ingress state indications.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO           *info -
 *     Inbound FC Generation mode and configuration.
 * REMARKS:
 *   1. This API is not applicable for Interlaken NIF 2. FC
 *   generation can be triggered by VSQ (statistics tag)
 *   and/or CNM. VSQ to NIF/Class mapping is predefined,
 *   according to the NIF type and FC mode (LL/CB). For CNM -
 *   use arad_fc_gen_inbnd_cnm_map_set API to complete the
 *   configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_fc_gen_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_fc_gen_inbnd_set"
 *     API.
 *     Refer to "arad_fc_gen_inbnd_set" API for details.
*********************************************************************/
uint32
  arad_fc_gen_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO           *info
  );

/*********************************************************************
* NAME:
 *   arad_fc_gen_inbnd_glb_hp_set
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
 *   SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode -
 *     Flow Control mode. If enabled, when Ingress Global
 *     Resources high priority Flow Control is indicated.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_fc_gen_inbnd_glb_hp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "arad_fc_gen_inbnd_glb_hp_set" API.
 *     Refer to "arad_fc_gen_inbnd_glb_hp_set" API for details.
*********************************************************************/
uint32
  arad_fc_gen_inbnd_glb_hp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  );
/*********************************************************************
* NAME:
 *   arad_fc_rec_inbnd_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Reception Configuration
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info -
 *     Inbound FC Reception mode and configuration.
 * REMARKS:
 *   1. This API is not applicable for Interlaken NIF 2. For
 *   CB - use arad_fc_rec_inbnd_ofp_map_set API to complete the
 *   configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_fc_rec_inbnd_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_fc_rec_inbnd_set"
 *     API.
 *     Refer to "arad_fc_rec_inbnd_set" API for details.
*********************************************************************/
uint32
  arad_fc_rec_inbnd_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO           *info
  );
/*********************************************************************
* NAME:
 *   arad_fc_gen_cal_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures Calendar-based interface calendar for Flow
 *   Control Generation (OOB/ILKN-Inband TX).
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     generation: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC is
 *     generated. For OOB interface (both SPI and ILKN), only
 *     SOC_TMC_FC_OOB_ID_B can be used. Additionally, ILKN-A can be
 *     used.
 *   SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf -
 *     Calendar-based (OOB) interface configuration -
 *     enable/disable, calendar configuration - length and
 *     repetitions.
 *   SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff -
 *     A buffer with cal_conf.cal_len entries. An allocation
 *     with the appropriate size should be made. Each entry
 *     configures a calendar channel, defining the source of
 *     the Flow Control that controls this channel.
 * REMARKS:
 *   1. For global resources - use indexes defined in
 *   ARAD_FC_CAL_GLB_RCS_ID.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  );

/*********************************************************************
*     Gets the configuration set by the "arad_fc_gen_cal_set"
 *     API.
 *     Refer to "arad_fc_gen_cal_set" API for details.
*********************************************************************/

uint32
  arad_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  );


/*********************************************************************
* NAME:
 *   arad_fc_rec_cal_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures Calendar-based interface calendar for Flow
 *   Control Reception.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     reception: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC
 *     reception is handled.
 *   SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf -
 *     Calendar-based (OOB) interface configuration -
 *     enable/disable, calendar configuration - length and
 *     repetitions.
 *   SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff -
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
  arad_fc_rec_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  );

/*********************************************************************
*     Gets the configuration set by the "arad_fc_rec_cal_set"
 *     API.
 *     Refer to "arad_fc_rec_cal_set" API for details.
*********************************************************************/
uint32
  arad_fc_rec_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR             *cal_buff
  );

uint32
  arad_fc_pfc_generic_bitmap_set(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );
uint32
  arad_fc_pfc_generic_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );


/*********************************************************************
* NAME:
 *   arad_fc_ilkn_llfc_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Defines if and how LLFC can be received/generated using
 *   Interlaken NIF.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use ARAD_NIF_ID(ILKN, 0 - 1).
 *   SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info -
 *     Link Level Flow Control configuration for the Interlaken
 *     NIF
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_fc_ilkn_llfc_set"
 *     API.
 *     Refer to "arad_fc_ilkn_llfc_set" API for details.
*********************************************************************/
uint32
  arad_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  );
uint32
  arad_fc_hcfc_bitmap_set(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *hcfc_bitmap
  );
uint32
  arad_fc_hcfc_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    bitmap_index,
    SOC_SAND_OUT   SOC_TMC_FC_PFC_GENERIC_BITMAP    *hcfc_bitmap
  );


/*********************************************************************
* NAME:
 *   arad_fc_nif_pause_frame_src_addr_set
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
  arad_fc_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_fc_nif_pause_frame_src_addr_set" API.
 *     Refer to "arad_fc_nif_pause_frame_src_addr_set" API for
 *     details.
*********************************************************************/
uint32
  arad_fc_nif_pause_frame_src_addr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );


#if defined(BCM_88650_B0)

/*********************************************************************
* NAME:
 *   arad_fc_ilkn_retransmit_cal_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx -
 *     ILKN index (ARAD_NIF_ID_ILKN_0, ARAD_NIF_ID_ILKN_1)
 *   SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx -
 *     FC Direction (RX or TX)
 *   SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg -
 *      The enable and length of the calendar
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/

uint32
  arad_fc_ilkn_retransmit_cal_set(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

uint32
  arad_fc_ilkn_retransmit_cal_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

#endif




/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_API_FLOW_CONTROL_INCLUDED__*/
#endif

