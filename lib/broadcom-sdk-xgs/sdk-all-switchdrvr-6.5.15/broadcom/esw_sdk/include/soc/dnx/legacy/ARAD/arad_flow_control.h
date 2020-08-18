
/* $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __JER2_ARAD_FLOW_CONTROL_INCLUDED__
/* { */
#define __JER2_ARAD_FLOW_CONTROL_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dnx/legacy/TMC/tmc_api_flow_control.h>

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
  JER2_ARAD_FC_OFP_PRIORITY_NONE = 0x0,
  /*
   *  OFP FC priority - Low
   */
  JER2_ARAD_FC_OFP_PRIORITY_LP = 0x1,
  /*
   *  OFP FC priority - High. HP-FC triggers also LP-FC
   */
  JER2_ARAD_FC_OFP_PRIORITY_HP = 0x3,
  /*
   *  Number of types in JER2_ARAD_FC_OFP_PRIORITY
   */
  JER2_ARAD_FC_NOF_OFP_PRIORITYS = 3
}JER2_ARAD_FC_OFP_PRIORITY;

typedef enum
{
  /* Arad ILKN Interface ID A */
  JER2_ARAD_FC_ILKN_ID_A = 0,

  /* Arad ILKN Interface ID B */
  JER2_ARAD_FC_ILKN_ID_B = 1,

  /* Arad number of ILKN Interfaces */
  JER2_ARAD_FC_ILKN_IDS = 2
}JER2_ARAD_FC_ILKN_ID;

typedef enum
{
  /* Reception Calendar Type */
  JER2_ARAD_FC_CAL_TYPE_RX = 0,

  /* Generation Calendar Type */
  JER2_ARAD_FC_CAL_TYPE_TX = 1,

  /* Calendar Types */
  JER2_ARAD_FC_CAL_TYPES = 2
}JER2_ARAD_FC_CAL_TYPE;

typedef SOC_DNX_FC_ILKN_CAL_LLFC JER2_ARAD_FC_ILKN_CAL_LLFC;

typedef SOC_DNX_FC_ILKN_LLFC_INFO JER2_ARAD_FC_ILKN_LLFC_INFO;


typedef struct
{

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

}JER2_ARAD_FC_ILKN_RETRANSMIT_INFO;

typedef struct
{

  /* Enable the ILKN Retransmit calendar */
  uint32 enable;

  /* Calendar length (0-2) */
  uint32 length;

}JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_CFG;

typedef struct
{

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

}JER2_ARAD_FC_SCH_OOB_WD_INFO;

typedef struct
{

  /* Enable the Scheduler OOB Interface 
   * (otherwise the FC indication won't pass to the EGQ) */
  uint32 enable;

  /* The CL range of the FC indication vector
   * The actual range is from 0 to the given value
   * This should be equal to the calendar length of the SCH OOB Interface
   * Max value - 4K-1 */
  uint32 sch_range;

  /* Hold the WatchDog info for the SCH OOB Interface */
  JER2_ARAD_FC_SCH_OOB_WD_INFO wd_info;

}JER2_ARAD_FC_SCH_OOB_INFO;

typedef struct  
{

  /* The queue pair for which to set the PFC Bitmap
   * Range: 0-255 */
  uint32 queue_pair;

  /* The PFC Bitmap: 8 bits, one for each TC */
  uint8 pfc_bm;

}JER2_ARAD_FC_PFC_BM_INFO;

typedef enum  
{
  JER2_ARAD_FC_OOB_TX_SPEED_CORE_2 = SOC_DNX_FC_OOB_TX_SPEED_CORE_2,
  JER2_ARAD_FC_OOB_TX_SPEED_CORE_4 = SOC_DNX_FC_OOB_TX_SPEED_CORE_4,
  JER2_ARAD_FC_OOB_TX_SPEED_CORE_8 = SOC_DNX_FC_OOB_TX_SPEED_CORE_8
}JER2_ARAD_FC_OOB_TX_SPEED;

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
*     jer2_arad_flow_control_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
shr_error_e
  jer2_arad_flow_control_init(
    DNX_SAND_IN  int                 unit
  );

shr_error_e
  jer2_arad_flow_control_init_oob_tx(
    DNX_SAND_IN  int                 unit
  );

shr_error_e
  jer2_arad_flow_control_init_oob_rx(
    DNX_SAND_IN  int                 unit
  );

shr_error_e
   jer2_arad_fc_enables_set(
      DNX_SAND_IN int                           unit,
      DNX_SAND_IN SOC_DNX_FC_ENABLE_BITMAP      *cfc_enables,
      DNX_SAND_IN SOC_DNX_FC_ENABLE_BITMAP      *ena_info
    );

shr_error_e
   jer2_arad_fc_enables_get(
      DNX_SAND_IN int                           unit,
      DNX_SAND_IN SOC_DNX_FC_ENABLE_BITMAP      *cfc_enables,
      DNX_SAND_OUT SOC_DNX_FC_ENABLE_BITMAP     *ena_info
    );

shr_error_e
  jer2_arad_fc_init_pfc_mapping(
      DNX_SAND_IN int  unit
    );


uint32
  jer2_arad_fc_inbnd_max_nof_priorities_get(
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              nif_ndx
#else 
    DNX_SAND_IN  JER2_ARAD_INTERFACE_ID             nif_ndx
#endif
  );

/*********************************************************************
* NAME:
 *   jer2_arad_fc_gen_inbnd_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Generation Configuration, based on
 *   Arad Ingress state indications.
 * INPUT:
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  JER2_ARAD_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   DNX_SAND_IN  SOC_DNX_FC_GEN_INBND_INFO           *info -
 *     Inbound FC Generation mode and configuration.
 * REMARKS:
 *   1. This API is not applicable for Interlaken NIF 2. FC
 *   generation can be triggered by VSQ (statistics tag)
 *   and/or CNM. VSQ to NIF/Class mapping is predefined,
 *   according to the NIF type and FC mode (LL/CB). For CNM -
 *   use jer2_arad_fc_gen_inbnd_cnm_map_set API to complete the
 *   configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
shr_error_e
  jer2_arad_fc_gen_inbnd_set(
    DNX_SAND_IN  int                                unit,
    DNX_SAND_IN  soc_port_t                         port,
    DNX_SAND_IN  SOC_DNX_FC_GEN_INBND_INFO          *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_gen_inbnd_set" API.
 *     Refer to "jer2_arad_fc_gen_inbnd_set" API for details.
*********************************************************************/
shr_error_e
  jer2_arad_fc_gen_inbnd_get(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  soc_port_t                 port,
    DNX_SAND_OUT SOC_DNX_FC_GEN_INBND_INFO  *info
  );

shr_error_e
  jer2_arad_fc_clear_calendar(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_CAL_TYPE                 cal_type,
    DNX_SAND_IN  SOC_DNX_FC_OOB_ID                   if_ndx
  );

/*********************************************************************
* NAME:
 *   jer2_arad_fc_gen_inbnd_glb_hp_set
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
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  SOC_DNX_FC_INGR_GEN_GLB_HP_MODE     fc_mode -
 *     Flow Control mode. If enabled, when Ingress Global
 *     Resources high priority Flow Control is indicated.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_fc_gen_inbnd_glb_hp_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );

int
  jer2_arad_fc_gen_inbnd_glb_hp_set_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );

int
  jer2_arad_fc_gen_inbnd_glb_hp_get_verify(
    DNX_SAND_IN  int                      unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_gen_inbnd_glb_hp_set" API.
 *     Refer to "jer2_arad_fc_gen_inbnd_glb_hp_set" API for
 *     details.
*********************************************************************/
int
  jer2_arad_fc_gen_inbnd_glb_hp_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_OUT SOC_DNX_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  );

/*********************************************************************
* NAME:
 *   jer2_arad_fc_gen_inbnd_cnm_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *   indication to generate when using Class Based Flow
 *   Control
 * INPUT:
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  uint32                      cpid_tc_ndx -
 *     The 3-LSB of the CPID index. Expected to represent TC.
 *   DNX_SAND_IN  uint32                      fc_class -
 *     The FC Class of the generated CBFC. Note: according to
 *     the inheritance configuration of the relevant NIF, the
 *     specified class may affect also lower classes. Range: 0 -
 *     7.
 * REMARKS:
 *   Inband FC generation, including CNM, is configured per
 *   NIF by jer2_arad_fc_gen_inbnd_set API. This API completes the
 *   CNM configuration when Class Based FC is used.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_fc_gen_inbnd_cnm_map_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      cpid_tc_ndx,
    DNX_SAND_IN  uint8                      enable_ll,
    DNX_SAND_IN  uint32                      fc_class
  );

int
  jer2_arad_fc_gen_inbnd_cnm_map_set_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      cpid_tc_ndx,
    DNX_SAND_IN  uint8                      enable_ll,
    DNX_SAND_IN  uint32                      fc_class
  );

int
  jer2_arad_fc_gen_inbnd_cnm_map_get_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      cpid_tc_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_gen_inbnd_cnm_map_set" API.
 *     Refer to "jer2_arad_fc_gen_inbnd_cnm_map_set" API for
 *     details.
*********************************************************************/
int
  jer2_arad_fc_gen_inbnd_cnm_map_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      cpid_tc_ndx,
    DNX_SAND_OUT uint8                      *enable_ll,
    DNX_SAND_OUT uint32                      *fc_class
  );
/*********************************************************************
* NAME:
 *   jer2_arad_fc_rec_inbnd_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inband Flow Control Reception Configuration
 * INPUT:
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  JER2_ARAD_INTERFACE_ID             nif_ndx -
 *     Network Interface index. Range: 0 - 63.
 *   DNX_SAND_IN  SOC_DNX_FC_REC_INBND_INFO           *info -
 *     Inbound FC Reception mode and configuration.
 * REMARKS:
 *   1. This API is not applicable for Interlaken NIF 2. For
 *   CB - use jer2_arad_fc_rec_inbnd_ofp_map_set API to complete the
 *   configuration
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/


shr_error_e
  jer2_arad_fc_rec_inbnd_get_verify(
    DNX_SAND_IN  int                      unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              nif_ndx
#else 
    DNX_SAND_IN  JER2_ARAD_INTERFACE_ID             nif_ndx
#endif
  );

/*********************************************************************
* NAME:
 *   jer2_arad_fc_gen_cal_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures Calendar-based interface calendar for Flow
 *   Control Generation (OOB/ILKN-Inband TX).
 * INPUT:
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     generation: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   DNX_SAND_IN  SOC_DNX_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC is
 *     generated. For OOB interface (both SPI and ILKN), only
 *     SOC_DNX_FC_OOB_ID_B can be used. Additionally, ILKN-A can be
 *     used.
 *   DNX_SAND_IN  SOC_DNX_FC_CAL_IF_INFO              *cal_conf -
 *     Calendar-based (OOB) interface configuration -
 *     enable/disable, calendar configuration - length and
 *     repetitions.
 *   DNX_SAND_IN  SOC_DNX_FC_GEN_CALENDAR             *cal_buff -
 *     A buffer with cal_conf.cal_len entries. An allocation
 *     with the appropriate size should be made. Each entry
 *     configures a calendar channel, defining the source of
 *     the Flow Control that controls this channel.
 * REMARKS:
 *   1. For global resources - use indexes defined in
 *   JER2_ARAD_FC_CAL_GLB_RCS_ID.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
shr_error_e
  jer2_arad_fc_gen_cal_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_INFO              *cal_conf,
    DNX_SAND_IN  SOC_DNX_FC_GEN_CALENDAR             *cal_buff
  );

shr_error_e
  jer2_arad_fc_gen_cal_set_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_INFO              *cal_conf,
    DNX_SAND_IN  SOC_DNX_FC_GEN_CALENDAR             *cal_buff
  );

shr_error_e
  jer2_arad_fc_gen_cal_get_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_gen_cal_set" API.
 *     Refer to "jer2_arad_fc_gen_cal_set" API for details.
*********************************************************************/
shr_error_e
  jer2_arad_fc_gen_cal_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx,
    DNX_SAND_OUT SOC_DNX_FC_CAL_IF_INFO              *cal_conf,
    DNX_SAND_OUT SOC_DNX_FC_GEN_CALENDAR             *cal_buff
  );

/*********************************************************************
* NAME:
 *   jer2_arad_fc_rec_cal_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configures Calendar-based interface calendar for Flow
 *   Control Reception.
 * INPUT:
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx -
 *     The interface and mode used for FC calendar-based
 *     reception: SPI-OOB, ILKN-OOB or ILKN-Inband
 *   DNX_SAND_IN  SOC_DNX_FC_OOB_ID                   if_ndx -
 *     The Interface Index of the interface on which the FC
 *     reception is handled.
 *   DNX_SAND_IN  SOC_DNX_FC_CAL_IF_INFO              *cal_conf -
 *     Calendar-based (OOB) interface configuration -
 *     enable/disable, calendar configuration - length and
 *     repetitions.
 *   DNX_SAND_IN  SOC_DNX_FC_REC_CALENDAR             *cal_buff -
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
shr_error_e
  jer2_arad_fc_rec_cal_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_INFO              *cal_conf,
    DNX_SAND_IN  SOC_DNX_FC_REC_CALENDAR             *cal_buff
  );

shr_error_e
  jer2_arad_fc_rec_cal_set_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_INFO              *cal_conf,
    DNX_SAND_IN  SOC_DNX_FC_REC_CALENDAR             *cal_buff
  );

shr_error_e
  jer2_arad_fc_rec_cal_get_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_rec_cal_set" API.
 *     Refer to "jer2_arad_fc_rec_cal_set" API for details.
*********************************************************************/
shr_error_e
  jer2_arad_fc_rec_cal_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_MODE                 cal_mode_ndx,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID                if_ndx,
    DNX_SAND_OUT SOC_DNX_FC_CAL_IF_INFO              *cal_conf,
    DNX_SAND_OUT SOC_DNX_FC_REC_CALENDAR             *cal_buff
  );
/*********************************************************************
* NAME:
 *   jer2_arad_fc_ilkn_llfc_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Defines if and how LLFC can be received/generated using
 *   Interlaken NIF.
 * INPUT:
 *   DNX_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use JER2_ARAD_NIF_ID(ILKN, 0 - 1).
 *   DNX_SAND_IN  JER2_ARAD_FC_ILKN_LLFC_INFO           *info -
 *     Link Level Flow Control configuration for the Interlaken
 *     NIF
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
shr_error_e
  jer2_arad_fc_ilkn_llfc_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID          ilkn_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_DIRECTION             direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_ILKN_LLFC_INFO           *info
  );

shr_error_e
  jer2_arad_fc_ilkn_llfc_set_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID          ilkn_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_DIRECTION             direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_ILKN_LLFC_INFO           *info
  );

shr_error_e
  jer2_arad_fc_ilkn_llfc_get_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID             ilkn_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_ilkn_llfc_set" API.
 *     Refer to "jer2_arad_fc_ilkn_llfc_set" API for details.
*********************************************************************/
shr_error_e
  jer2_arad_fc_ilkn_llfc_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID             ilkn_ndx,
    DNX_SAND_OUT JER2_ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    DNX_SAND_OUT JER2_ARAD_FC_ILKN_LLFC_INFO           *gen_info
  );

shr_error_e
  jer2_arad_fc_pfc_generic_bitmap_verify(
    DNX_SAND_IN   int   unit,
    DNX_SAND_IN   int       priority,
    DNX_SAND_IN   uint32    pfc_bitmap_index
  );
shr_error_e
  jer2_arad_fc_pfc_generic_bitmap_set(
    DNX_SAND_IN   int   unit,
    DNX_SAND_IN   int       priority,
    DNX_SAND_IN   uint32    pfc_bitmap_index,
    DNX_SAND_IN   SOC_DNX_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );
shr_error_e
  jer2_arad_fc_pfc_generic_bitmap_get(
    DNX_SAND_IN   int   unit,
    DNX_SAND_IN   int       priority,
    DNX_SAND_IN   uint32    pfc_bitmap_index,
    DNX_SAND_OUT  SOC_DNX_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  );

shr_error_e
jer2_arad_fc_pfc_generic_bitmap_valid_update(
    DNX_SAND_IN   int                               unit,
    DNX_SAND_IN   SOC_DNX_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    DNX_SAND_IN   int                               priority,
    DNX_SAND_OUT  uint32                           *is_valid
    );

shr_error_e
jer2_arad_fc_pfc_generic_bitmap_used_update(
    DNX_SAND_IN   int                               unit,
    DNX_SAND_IN   SOC_DNX_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    DNX_SAND_IN   int                               priority,
    DNX_SAND_IN   int                               pfc_bitmap_index,
    DNX_SAND_OUT  uint32                            is_set
    );

shr_error_e
  jer2_arad_fc_cat_2_tc_hcfc_bitmap_set(
    DNX_SAND_IN   int                           unit,
    DNX_SAND_IN   int                           core,
    DNX_SAND_IN   int                           tc,
    DNX_SAND_IN   int                           is_ocb_only,
    DNX_SAND_OUT  SOC_DNX_FC_HCFC_BITMAP        *hcfc_bitmap
  );

shr_error_e
  jer2_arad_fc_cat_2_tc_hcfc_bitmap_get(
    DNX_SAND_IN   int                           unit,
    DNX_SAND_IN   int                           core,
    DNX_SAND_IN   int                           tc,
    DNX_SAND_IN   int                           is_ocb_only,
    DNX_SAND_OUT  SOC_DNX_FC_HCFC_BITMAP        *hcfc_bitmap
  );

shr_error_e
  jer2_arad_fc_glb_hcfc_bitmap_set(
    DNX_SAND_IN   int                           unit,
    DNX_SAND_IN   int                           core,
    DNX_SAND_IN   int                           is_high_prio,
    DNX_SAND_IN   int                           is_ocb_only,
    DNX_SAND_IN   int                           pool_id,
    DNX_SAND_OUT  SOC_DNX_FC_HCFC_BITMAP        *hcfc_bitmap
  );

shr_error_e
  jer2_arad_fc_glb_hcfc_bitmap_get(
    DNX_SAND_IN   int                           unit,
    DNX_SAND_IN   int                           core,
    DNX_SAND_IN   int                           is_high_prio,
    DNX_SAND_IN   int                           is_ocb_only,
    DNX_SAND_IN   int                           pool_id,
    DNX_SAND_OUT  SOC_DNX_FC_HCFC_BITMAP        *hcfc_bitmap
  );

shr_error_e
  jer2_arad_fc_hcfc_watchdog_set(
    DNX_SAND_IN   int                           unit,
    DNX_SAND_IN   SOC_DNX_FC_CAL_IF_ID          if_ndx,
    DNX_SAND_IN   SOC_DNX_FC_HCFC_WATCHDOG      *hcfc_watchdog
  );

shr_error_e
  jer2_arad_fc_hcfc_watchdog_get(
    DNX_SAND_IN   int                           unit,
    DNX_SAND_IN   SOC_DNX_FC_CAL_IF_ID          if_ndx,
    DNX_SAND_OUT  SOC_DNX_FC_HCFC_WATCHDOG      *hcfc_watchdog
  );


int
  jer2_arad_fc_ilkn_retransmit_set(
    DNX_SAND_IN  int                        unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif
    DNX_SAND_IN  JER2_ARAD_FC_ILKN_RETRANSMIT_INFO     *ret_info
  );


int
  jer2_arad_fc_ilkn_retransmit_get(
    DNX_SAND_IN  int                        unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif

    DNX_SAND_OUT JER2_ARAD_FC_ILKN_RETRANSMIT_INFO     *ret_info
  );



int
  jer2_arad_fc_ilkn_mub_rec_set(
    DNX_SAND_IN  int                           unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif

    DNX_SAND_IN  uint8                            bitmap
  );

int
  jer2_arad_fc_ilkn_mub_rec_get(
    DNX_SAND_IN  int                           unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif
    DNX_SAND_OUT uint8                            *bitmap
  );

shr_error_e
  jer2_arad_fc_ilkn_mub_channel_set(
    DNX_SAND_IN  int                            unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID           ilkn_ndx,
    DNX_SAND_IN  SOC_DNX_FC_DIRECTION           direction_ndx,
    DNX_SAND_OUT uint8                          bitmap
  );

shr_error_e
  jer2_arad_fc_ilkn_mub_channel_get(
    DNX_SAND_IN  int                            unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID           ilkn_ndx,
    DNX_SAND_IN  SOC_DNX_FC_DIRECTION           direction_ndx,
    DNX_SAND_OUT uint8                          *bitmap
  );

shr_error_e
  jer2_arad_fc_ilkn_mub_gen_cal_set(
    DNX_SAND_IN  int                           unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID             ilkn_ndx,
    DNX_SAND_IN  SOC_DNX_FC_ILKN_MUB_GEN_CAL      *cal_info
  );

shr_error_e
  jer2_arad_fc_ilkn_mub_gen_cal_get(
    DNX_SAND_IN  int                           unit,
    DNX_SAND_IN  SOC_DNX_FC_CAL_IF_ID             ilkn_ndx,
    DNX_SAND_OUT SOC_DNX_FC_ILKN_MUB_GEN_CAL      *cal_info
  );

int
jer2_arad_fc_rec_cal_dest_type_to_val_internal(
    DNX_SAND_IN SOC_DNX_FC_REC_CAL_DEST dest
  );

int
jer2_arad_fc_gen_cal_src_type_to_val_internal(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN SOC_DNX_FC_GEN_CAL_SRC src_type
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_fc_nif_pause_frame_src_addr_set" API.
 *     Refer to "jer2_arad_fc_nif_pause_frame_src_addr_set" API
 *     for details.
*********************************************************************/

uint32
  JER2_ARAD_FC_ILKN_LLFC_INFO_verify(
    DNX_SAND_IN  JER2_ARAD_FC_ILKN_LLFC_INFO *info
  );

shr_error_e
  jer2_arad_fc_pfc_mapping_set(
    DNX_SAND_IN int  unit,
    DNX_SAND_IN uint32                        nif_id,
    DNX_SAND_IN uint32                        src_pfc_ndx,
    DNX_SAND_IN SOC_DNX_FC_PFC_MAP            *pfc_map
  );

shr_error_e
  jer2_arad_fc_pfc_mapping_get(
    DNX_SAND_IN int  unit,
    DNX_SAND_IN uint32                        nif_id,
    DNX_SAND_IN uint32                        src_pfc_ndx,
    DNX_SAND_OUT SOC_DNX_FC_PFC_MAP           *pfc_map
  );

int
  jer2_arad_fc_port_fifo_threshold_set(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  soc_port_t                 port,
    DNX_SAND_IN  SOC_DNX_FC_PORT_FIFO_TH    *info
  );

int
  jer2_arad_fc_port_fifo_threshold_get(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  soc_port_t                 port,
    DNX_SAND_OUT SOC_DNX_FC_PORT_FIFO_TH    *info
  );

shr_error_e
  jer2_arad_fc_inbnd_mode_set(
      DNX_SAND_IN int                           unit,
      DNX_SAND_IN soc_port_t                    port,
      DNX_SAND_IN int                           is_generate,
      DNX_SAND_IN SOC_DNX_FC_INBND_MODE         mode
  );

shr_error_e
  jer2_arad_fc_inbnd_mode_get(
      DNX_SAND_IN int                           unit,
      DNX_SAND_IN soc_port_t                    port,
      DNX_SAND_IN int                           is_generate,
      DNX_SAND_OUT SOC_DNX_FC_INBND_MODE        *mode
  );

shr_error_e
  jer2_arad_fc_vsq_index_group2global(
    DNX_SAND_IN int                    unit,
    DNX_SAND_IN  SOC_DNX_ITM_VSQ_GROUP vsq_group,
    DNX_SAND_IN  uint32                vsq_in_group_ndx,
    DNX_SAND_IN  int                   cosq,
    DNX_SAND_IN  uint8                 is_ocb_only,
    DNX_SAND_IN  uint32                src_port,
    DNX_SAND_OUT uint32                *vsq_fc_ndx
  );

shr_error_e
  jer2_arad_fc_cmic_rx_set(
      DNX_SAND_IN int                           unit,
      DNX_SAND_IN soc_port_t                    port,
      DNX_SAND_IN uint32                        priority_bmp,
      DNX_SAND_IN int                           is_ena
  );

shr_error_e
  jer2_arad_fc_cmic_rx_get(
      DNX_SAND_IN int                           unit,
      DNX_SAND_IN soc_port_t                    port,
      DNX_SAND_OUT uint32                       *priority_bmp,
      DNX_SAND_OUT int                          *is_ena
  );

shr_error_e
  jer2_arad_fc_status_info_get(
    DNX_SAND_IN int                      unit,
    DNX_SAND_IN SOC_DNX_FC_STATUS_KEY   *fc_status_key,
    DNX_SAND_OUT SOC_DNX_FC_STATUS_INFO *fc_status_info
  );


int
  jer2_arad_fc_ilkn_retransmit_cal_set(
    DNX_SAND_IN  int                             unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif
    DNX_SAND_IN  JER2_ARAD_FC_DIRECTION                  direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

int
  jer2_arad_fc_ilkn_retransmit_cal_get(
    DNX_SAND_IN  int                             unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif
    DNX_SAND_IN  JER2_ARAD_FC_DIRECTION                  direction_ndx,
    DNX_SAND_OUT JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

int
  jer2_arad_fc_ilkn_retransmit_cal_set_verify(
    DNX_SAND_IN  int                             unit,
#ifndef FIXME_DNX_LEGACY
    DNX_SAND_IN  int              ilkn_ndx,
#else 
    DNX_SAND_IN  JER2_ARAD_NIF_ILKN_ID             ilkn_ndx,
#endif
    DNX_SAND_IN  JER2_ARAD_FC_DIRECTION                  direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

void 
  JER2_ARAD_FC_ILKN_RETRANSMIT_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_FC_ILKN_RETRANSMIT_INFO *info
  );

void 
  JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_CFG_clear(
    DNX_SAND_OUT JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_CFG *info
  );

int jer2_arad_fc_shr_mapping(int unit, int fcv_bit, int cl_index, int select, int valid);
int jer2_arad_fc_init_shr_mapping(int unit);

/* } */


/* } __JER2_ARAD_FLOW_CONTROL_INCLUDED__*/
#endif


