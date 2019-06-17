/* $Id: jer2_arad_fabric.h,v 1.30 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __JER2_ARAD_FABRIC_INCLUDED__
/* { */
#define __JER2_ARAD_FABRIC_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/ARAD/arad_chip_defines.h>
#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>
#include <soc/dnx/legacy/TMC/tmc_api_ports.h>
#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnxc/legacy/fabric.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define JER2_ARAD_FBC_PRIORITY_NDX_MASK          (0xff)
#define JER2_ARAD_FBC_PRIORITY_NDX_NOF           (256)

#define JER2_ARAD_FBC_PRIORITY_NDX_IS_TDM_MASK       (0x1)
#define JER2_ARAD_FBC_PRIORITY_NDX_IS_TDM_OFFSET     (0)
#define JER2_ARAD_FBC_PRIORITY_NDX_TC_MASK           (0xE)
#define JER2_ARAD_FBC_PRIORITY_NDX_TC_OFFSET         (1)
#define JER2_ARAD_FBC_PRIORITY_NDX_DP_MASK           (0x30)
#define JER2_ARAD_FBC_PRIORITY_NDX_DP_OFFSET         (4)
#define JER2_ARAD_FBC_PRIORITY_NDX_IS_HP_MASK        (0x80)
#define JER2_ARAD_FBC_PRIORITY_NDX_IS_HP_OFFSET      (7)

#define JER2_ARAD_FBC_PRIORITY_NOF                   (0x4)
#define JER2_ARAD_FBC_PRIORITY_LENGTH                (2)



/*GCI backoff random mask configuration*/
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_0       (0x1)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_1       (0xf)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_2       (0x7f)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_3       (0x3ff)

#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_0       (0x3)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_1       (0x1f)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_2       (0xff)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_3       (0x7ff)

#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_0       (0x7)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_1       (0x3f)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_2       (0x1ff)
#define JER2_ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_3       (0xfff)


/*     Maximal number of devices in coexist mode               */
#define  JER2_ARAD_FABRIC_NOF_COEXIST_DEVICES (SOC_DNX_FABRIC_NOF_COEXIST_DEVICES)*
/*     Coexist mode devices - each entry identifies 2
 *     consecutive devices                                     */
#define  JER2_ARAD_FABRIC_NOF_COEXIST_DEV_ENTRIES (SOC_DNX_FABRIC_NOF_COEXIST_DEV_ENTRIES)

/*     Maximal number of fap20v devices in the system          */
#define  JER2_ARAD_FABRIC_NOF_FAP20_DEVICES (SOC_DNX_FABRIC_NOF_FAP20_DEVICES)

/*     Maximal number of devices in mesh mode                  */
#define  JER2_ARAD_FABRIC_MESH_DEVICES_MAX (SOC_DNX_FABRIC_MESH_DEVICES_MAX)

/*      Maximal value of BYTES between consecutive comma bursts (when Cm_Tx_Byte_Mode is enabled).       */
#define JER2_ARAD_FABRIC_SHAPER_BYTES_MAX  (SOC_DNX_FABRIC_SHAPER_BYTES_MAX)

/*      Maximal value of CELLS between consecutive comma bursts (when Cm_Tx_Byte_Mode is disabled).       */
#define JER2_ARAD_FABRIC_SHAPER_CELLS_MAX  (SOC_DNX_FABRIC_SHAPER_CELLS_MAX)
/* } */

/*************
 * MACROS    *
 *************/
/* { */
#ifdef FIXME_DNX_LEGACY /** SOC_DNX_DEFS not supported */
#define JER2_ARAD_FBC_LINK_IN_MAC(unit, link_id) ((link_id) % SOC_DNX_DEFS_GET(unit, nof_fabric_links_in_mac))
#define JER2_ARAD_FBC_MAC_OF_LINK(unit, link_id) ((link_id) / SOC_DNX_DEFS_GET(unit, nof_fabric_links_in_mac))
#define JER2_ARAD_FBC_IS_MAC_COMBO_NIF_NOT_FBR(mac_id, is_combo_nif_not_fabric) \
  ( (mac_id == SOC_DNX_DEFS_GET(unit, nof_instances_fmac)-1) && (is_combo_nif_not_fabric) )
#endif
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

#define JER2_ARAD_LINK_ERR_TYPE_CRC                            SOC_DNX_LINK_ERR_TYPE_CRC
#define JER2_ARAD_LINK_ERR_TYPE_SIZE                           SOC_DNX_LINK_ERR_TYPE_SIZE
#define JER2_ARAD_LINK_ERR_TYPE_MISALIGN                       SOC_DNX_LINK_ERR_TYPE_MISALIGN
#define JER2_ARAD_LINK_ERR_TYPE_CODE_GROUP                     SOC_DNX_LINK_ERR_TYPE_CODE_GROUP
typedef SOC_DNX_LINK_ERR_TYPE                                  JER2_ARAD_LINK_ERR_TYPE;

#define JER2_ARAD_LINK_INDICATE_TYPE_SIG_LOCK                  SOC_DNX_LINK_INDICATE_TYPE_SIG_LOCK
#define JER2_ARAD_LINK_INDICATE_TYPE_ACCEPT_CELL               SOC_DNX_LINK_INDICATE_TYPE_ACCEPT_CELL
#define JER2_ARAD_LINK_INDICATE_INTRNL_FIXED                   SOC_DNX_LINK_INDICATE_INTRNL_FIXED
typedef SOC_DNX_LINK_INDICATE_TYPE                             JER2_ARAD_LINK_INDICATE_TYPE;

#define JER2_ARAD_LINK_STATE_ON                                SOC_DNX_LINK_STATE_ON
#define JER2_ARAD_LINK_STATE_OFF                               SOC_DNX_LINK_STATE_OFF
#define JER2_ARAD_LINK_NOF_STATES                              SOC_DNX_LINK_NOF_STATES
typedef SOC_DNX_LINK_STATE                                     JER2_ARAD_LINK_STATE;

#define JER2_ARAD_FABRIC_LINE_CODING_8_10                      SOC_DNX_FABRIC_LINE_CODING_8_10
#define JER2_ARAD_FABRIC_LINE_CODING_8_9_FEC                   SOC_DNX_FABRIC_LINE_CODING_8_9_FEC
#define JER2_ARAD_FABRIC_NOF_LINE_CODINGS                      SOC_DNX_FABRIC_NOF_LINE_CODINGS
typedef SOC_DNX_FABRIC_LINE_CODING                             JER2_ARAD_FABRIC_LINE_CODING;

typedef SOC_DNX_FABRIC_LINKS_CONNECT_MAP_STAT_INFO             JER2_ARAD_FABRIC_LINKS_CONNECT_MAP_STAT_INFO;
typedef SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR                 JER2_ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR;
typedef SOC_DNX_FABRIC_CELL_FORMAT                             JER2_ARAD_FABRIC_CELL_FORMAT;
typedef SOC_DNX_FABRIC_FC                                      JER2_ARAD_FABRIC_FC;
typedef SOC_DNX_FABRIC_COEXIST_INFO                            JER2_ARAD_FABRIC_COEXIST_INFO;
typedef SOC_DNX_FABRIC_LINKS_STATUS_SINGLE                     JER2_ARAD_FABRIC_LINKS_STATUS_SINGLE;
typedef SOC_DNX_FABRIC_LINKS_STATUS_ALL                        JER2_ARAD_FABRIC_LINKS_STATUS_ALL;
typedef SOC_DNX_LINK_STATE_INFO                                JER2_ARAD_LINK_STATE_INFO;

#define JER2_ARAD_FAR_NOF_DEVICE_TYPES                         SOC_DNX_FAR_NOF_DEVICE_TYPES

#define JER2_ARAD_SRD_POWER_STATE_UP                           SOC_DNX_SRD_POWER_STATE_UP      
#define JER2_ARAD_SRD_POWER_STATE_DOWN                         SOC_DNX_SRD_POWER_STATE_DOWN  
#define JER2_ARAD_SRD_POWER_STATE_UP_AND_RELOCK                SOC_DNX_SRD_POWER_STATE_UP_AND_RELOCK
#define JER2_ARAD_SRD_NOF_POWER_STATES                         SOC_DNX_SRD_NOF_POWER_STATES                           
typedef SOC_DNX_SRD_POWER_STATE                                JER2_ARAD_SRD_POWER_STATE;


#define JER2_ARAD_FABRIC_SHAPER_BYTES_MODE                     SOC_DNX_FABRIC_SHAPER_BYTES_MODE
#define JER2_ARAD_FABRIC_SHAPER_CELLS_MODE                     SOC_DNX_FABRIC_SHAPER_CELLS_MODE
#define JER2_ARAD_FABRIC_SHAPER_NOF_MODES                      SOC_DNX_FABRIC_SHAPER_NOF_MODES
typedef SOC_DNX_FABRIC_SHAPER_MODE                             JER2_ARAD_FABRIC_SHAPER_MODE;

typedef SOC_DNX_FABRIC_FC_SHAPER_MODE_INFO                     JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO;

typedef SOC_DNX_SHAPER_INFO                                    JER2_ARAD_SHAPER_INFO;

typedef SOC_DNX_FABRIC_FC_SHAPER                              JER2_ARAD_FABRIC_FC_SHAPER;

#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0                    SOC_DNX_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1                    SOC_DNX_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2                    SOC_DNX_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH              SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH              SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH              SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH              SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL                       SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL                       SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL                       SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL                       SOC_DNX_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL
#define JER2_ARAD_FABRIC_GCI_CONFIG_TYPE_NOF                             SOC_DNX_FABRIC_GCI_CONFIG_TYPE_NOF
typedef SOC_DNX_FABRIC_GCI_CONFIG_TYPE                              JER2_ARAD_FABRIC_GCI_CONFIG_TYPE;

#define JER2_ARAD_FABRIC_GCI_TYPE_LEAKY_BUCKET                           SOC_DNX_FABRIC_GCI_TYPE_LEAKY_BUCKET
#define JER2_ARAD_FABRIC_GCI_TYPE_RANDOM_BACKOFF                         SOC_DNX_FABRIC_GCI_TYPE_RANDOM_BACKOFF
#define JER2_ARAD_FABRIC_GCI_TYPE_NOF                                    SOC_DNX_FABRIC_GCI_TYPE_NOF
typedef SOC_DNX_FABRIC_GCI_TYPE                                     JER2_ARAD_FABRIC_GCI_TYPE;

#define JER2_ARAD_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH                     SOC_DNX_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH
#define JER2_ARAD_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE                 SOC_DNX_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE
#define JER2_ARAD_FABRIC_RCI_CONFIG_TYPE_NOF                             SOC_DNX_FABRIC_RCI_CONFIG_TYPE_NOF
typedef SOC_DNX_FABRIC_RCI_CONFIG_TYPE                              JER2_ARAD_FABRIC_RCI_CONFIG_TYPE;

typedef enum
{
  /*
   * FTMH load balancing extension is disabled.
   */
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED = 0,
  /*
   * FTMH load balancing extension is enabled, and contains an 8-bit
   * load balancing key and an 8-bit stacking route history bitmap.
   */
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY = 1,
  /*
   * FTMH load balancing extension is enabled, and contains a 16-bit
   * stacking route history.
   */
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY = 2,
  /*
   * FTMH load balancing extension is enabled.
   */
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE_ENABLED = 3,
   /*
   * FTMH load balancing extension is enabled, end 2 lb-key range table are used.
   */
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB = 4,
   /*
   * FTMH load balancing extension is enabled, and contains a 16-bit, Transmit on the 8b FTMH LB-Key Hash[15:8] , and on User-Header-2 .
   */  
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH = 5,
  /*
   *  Number of types in JER2_ARAD_MGMT_FTMH_LB_EXT_MODE
   */
  JER2_ARAD_MGMT_NOF_FTMH_LB_EXT_MODES = 6

} JER2_ARAD_MGMT_FTMH_LB_EXT_MODE;


typedef enum
{
  /*
   * Use 8 bits LAG-LB-Key.
   */

  JER2_ARAD_MGMT_TRUNK_HASH_FORMAT_NORMAL = 0,  
  /*
   *  Use 16 bits LAG-LB-Key, Switch the LAG-LB-Key transmitted from PP to TM: {LAG-LB-Key[7:0], LAG-LB-Key[15:8]}.  
   */
  JER2_ARAD_MGMT_TRUNK_HASH_FORMAT_INVERTED = 1,
  /*
   *  Use 16 bits LAG-LB-Key, Duplicate the TM LAG-LB-Key to {LAG-LB-Key[15:8], LAG-LB-Key[15:8]}. 
   */
  JER2_ARAD_MGMT_TRUNK_HASH_FORMAT_DUPLICATED = 2,
  /*
   *  Number of types in JER2_ARAD_MGMT_TRUNK_HASH_FORMAT
   */
  JER2_ARAD_MGMT_NOF_TRUNK_HASH_FORMAT = 3
  
} JER2_ARAD_MGMT_TRUNK_HASH_FORMAT;

typedef struct
{
  /*
   *  Enable/disable fabric configuration
   */
  uint8 enable;
  /*
   *  FTMH Header configuration: always allow, never allow,
   *  allow only when the packet is multicast.
   */
  SOC_DNX_PORTS_FTMH_EXT_OUTLIF ftmh_extension;
  /*
   *  FTMH load balancing mode.
   */
  JER2_ARAD_MGMT_FTMH_LB_EXT_MODE ftmh_lb_ext_mode;
   /*
   *  FTMH Trunk Hash format.
   */
  JER2_ARAD_MGMT_TRUNK_HASH_FORMAT trunk_hash_format;
  /*
   *  FTMH Stacking mode.
   */
  uint8 ftmh_stacking_ext_mode;
  /* 
   * Presence of DNX_SAND_FE600 in the system
   */ 
  uint8 is_fe600;

  /*
   *  Dual Pipe mode
   */
  uint8 dual_pipe_tdm_packet;

  /*
   *  Presence of Dual Pipe mode in system
   */
  uint8 is_dual_mode_in_system;

  /*
   * Fabric cell segmentation enable
   */
  uint8 segmentation_enable;
  /*
  * Scheduler adaptation to links' states
  */
  uint8 scheduler_adapt_to_links;
  /*
  * System contains a device with multiple pipes
  */
  uint8 system_contains_multiple_pipe_device;

  /*
   *  fabric PCP enable: enable / disable packet cell packing
   */
  uint8 fabric_pcp_enable;

  /*
   *  fabric min TDM priority: NONE / 0-3
   */
  uint8 fabric_tdm_priority_min;

  /*
   *  fabric links to core mapping mode: SHARED / DEDICATED 
   */
  uint8 fabric_links_to_core_mapping_mode;
  
  /*
   *  fabric mesh multicast enable: enable / disable multicast in MESH mode
   */
  uint8 fabric_mesh_multicast_enable;

  /*
   *  mesh_topology fast - should be used for debug only
   */
  uint8 fabric_mesh_topology_fast;

}JER2_ARAD_INIT_FABRIC;

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
*     jer2_arad_fabric_init
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
int
  jer2_arad_fabric_init(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_INIT_FABRIC*      fabric
  );

/*********************************************************************
* NAME:
*    jer2_arad_fabric_regs_init
* FUNCTION:
*   Initialization of the Arad blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
int
  jer2_arad_fabric_regs_init(
    DNX_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_fc_enable_set
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION    direction_ndx -
*     Direction index - RX, TX or Both.
*  DNX_SAND_IN  JER2_ARAD_FABRIC_FC           *info -
*     Struct holds the enable/disable link level flow control
*     information.
* REMARKS:
*     None.
* RETURNS:
*     The get function is not entirely symmetric to the set function
*     (where only rx, tx or both directions can be defined). The get
*     function returns the both directions.
*********************************************************************/
int
  jer2_arad_fabric_fc_enable_set(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC            *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_fc_enable_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION    direction_ndx -
*     Direction index - RX, TX or Both.
*  DNX_SAND_IN  JER2_ARAD_FABRIC_FC           *info -
*     Struct holds the enable/disable flow control
*     information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_fabric_fc_enable_verify(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC            *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_fc_enable_get
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure enables/disables flow-control on fabric
*     links.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_OUT JER2_ARAD_FABRIC_FC           *info_rx -
*     Struct holds the enable/disable flow control
*     information for rx.
*  DNX_SAND_OUT JER2_ARAD_FABRIC_FC           *info_tx -
*     Struct holds the enable/disable flow control
*     information for tx.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     Not entirely symmetric to the set function (where only rx,
*     tx or both directions can be defined). The get function returns
*     the both directions.
*********************************************************************/
int
  jer2_arad_fabric_fc_enable_get(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_OUT JER2_ARAD_FABRIC_FC            *info_rx,
    DNX_SAND_OUT JER2_ARAD_FABRIC_FC            *info_tx
  );

/*********************************************************************  
* NAME:
*     jer2_arad_fabric_flow_control_init
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure init fabric flow-control on fabric
*     links.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION    direction_ndx -
*     Direction index - RX, TX or Both.
*  DNX_SAND_IN  JER2_ARAD_FABRIC_FC           *info -
*     Struct holds the enable/disable link level flow control
*     information.
* REMARKS:
*     None.
*********************************************************************/
int
  jer2_arad_fabric_flow_control_init(
    DNX_SAND_IN  int                  unit,
    DNX_SAND_IN  JER2_ARAD_CONNECTION_DIRECTION direction_ndx,
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC            *info
  );


/*********************************************************************  
* NAME:
*     jer2_arad_fabric_gci_backoff_masks_init
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure init fabric flow-control on fabric
*     links.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     init gci backoff masks
* REMARKS:
*     None.
*********************************************************************/
shr_error_e
  jer2_arad_fabric_gci_backoff_masks_init(
    DNX_SAND_IN  int                 unit
  );
/*********************************************************************  
* NAME:
*     jer2_arad_fabric_scheduler_adaptation_init
* DATE:
*   December 18 2013
* FUNCTION:
*     This procedure sets the scheduler adaptation to links' states
* INPUT:
*   DNX_SAND_IN  int                unit
* REMARKS:
*     None.
*********************************************************************/
int
    jer2_arad_fabric_scheduler_adaptation_init(
       DNX_SAND_IN int    unit
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_fc_shaper_set
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure sets parameters of both fabric link 
*     data shapers and flow control shapers. 
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  shaper_mode -
*    The mode of the shapers - shape transmitted bytes
*    or transmitted cells.
*  DNX_SAND_IN JER2_ARAD_FABRIC_FC_SHAPER    *info -
*     Struct holds the data shaper and
*     flow control shapers information.
*  DNX_SAND_OUT JER2_ARAD_FABRIC_FC_SHAPER    *exact_info -
*     Struct holds the exact data shaper and
*     flow control shapers information that was set.
*  
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     
*********************************************************************/
int
  jer2_arad_fabric_fc_shaper_set(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  uint32                        link_ndx,
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER            *info,
    DNX_SAND_OUT JER2_ARAD_FABRIC_FC_SHAPER            *exact_info
  );


/*********************************************************************
* NAME:
*     jer2_arad_fabric_fc_shaper_verify
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure sets parameters of both fabric link 
*     data shapers and flow control shapers. 
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  shaper_mode -
*    The mode of the shapers - shape transmitted bytes
*    or transmitted cells.
*  DNX_SAND_IN JER2_ARAD_FABRIC_FC_SHAPER    *info -
*     Struct holds the data shaper and
*     flow control shapers information.
*  DNX_SAND_OUT JER2_ARAD_FABRIC_FC_SHAPER    *exact_info -
*     Struct holds the exact data shaper and
*     flow control shapers information that was set.
*  
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     
*********************************************************************/
int
  jer2_arad_fabric_fc_shaper_verify(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  uint32                        link_ndx,
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER            *info,
    DNX_SAND_OUT JER2_ARAD_FABRIC_FC_SHAPER            *exact_info
  );


/*********************************************************************
* NAME:
*     jer2_arad_fabric_fc_shaper_get
* TYPE:
*   PROC
* DATE:
*   Feb 12 2008
* FUNCTION:
*     This procedure sets parameters of both fabric link 
*     data shapers and flow control shapers. 
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  DNX_SAND_OUT JER2_ARAD_FABRIC_FC_SHAPER    *info -
*     Struct holds the data shaper and
*     flow control shaper information of the link.
*  DNX_SAND_IN  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  shaper_mode -
*    The mode of the shapers - shape transmitted bytes
*    or transmitted cells.
*  
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     
*********************************************************************/
uint32
  jer2_arad_fabric_fc_shaper(
    DNX_SAND_IN  int                         unit,
    DNX_SAND_IN  uint32                         link_ndx,
    DNX_SAND_OUT  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    DNX_SAND_OUT  JER2_ARAD_FABRIC_FC_SHAPER            *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_topology_status_connectivity_get
* TYPE:
*   PROC
* FUNCTION:
*     Retrieve the connectivity map from the device.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  int                 link_index_min -
*     The first link index which this API reterives the info
*  DNX_SAND_IN  int                 link_index_max -
*     The last link index which this API reterives the info
*  DNX_SAND_OUT JER2_ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map -
*     The connectivity map
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_fabric_topology_status_connectivity_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  int                    link_index_min,
    DNX_SAND_IN  int                    link_index_max,
    DNX_SAND_OUT JER2_ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );
/*********************************************************************
* NAME:
*     jer2_arad_link_on_off_set
* TYPE:
*   PROC
* FUNCTION:
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  DNX_SAND_IN  JER2_ARAD_LINK_STATE_INFO     *info -
*     Fabric link on/off state.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_link_on_off_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  soc_port_t               port,
    DNX_SAND_IN  JER2_ARAD_LINK_STATE_INFO     *info
  );

shr_error_e
    jer2_arad_link_power_set(
        int unit, 
        soc_port_t port, 
        uint32 flags, 
        soc_dnxc_port_power_t power
        );

/*********************************************************************
* NAME:
*     jer2_arad_link_on_off_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  DNX_SAND_IN  JER2_ARAD_LINK_STATE_INFO     *info -
*     Fabric link on/off state.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_link_on_off_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 link_ndx,
    DNX_SAND_IN  JER2_ARAD_LINK_STATE_INFO     *info
  );

/*********************************************************************
* NAME:
*     jer2_arad_link_on_off_get
* TYPE:
*   PROC
* FUNCTION:
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 link_ndx -
*     The fabric link index. Range: 0 - 35.
*  DNX_SAND_OUT JER2_ARAD_LINK_STATE_INFO     *info -
*     Fabric link on/off state.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_link_on_off_get(
    DNX_SAND_IN  int                    unit,
    DNX_SAND_IN  soc_port_t             port,
    DNX_SAND_OUT JER2_ARAD_LINK_STATE_INFO   *info
  );

shr_error_e
    jer2_arad_link_power_get(
        int unit, 
        soc_port_t port, 
        soc_dnxc_port_power_t* power
        );

/*
 * Function:
 *      jer2_arad_fabric_link_status_all_get
 * Purpose:
 *      Get all links status
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      links_array_max_size - (IN)  max szie of link_status array
 *      link_status          - (OUT) array of link status per link
 *      errored_token_count  - (OUT) array error token count per link
 *      links_array_count    - (OUT) array actual size
 * Returns:
 *      _SHR_E_xxx
 */

shr_error_e
jer2_arad_fabric_link_status_all_get(
  int unit,
  int links_array_max_size,
  uint32* link_status,
  uint32* errored_token_count,
  int* links_array_count
);

/*
 * Function:
 *      jer2_arad_fabric_link_status_clear
 * Purpose:
 *      clear link status
 * Parameters:
 *      unit                - (IN)  Unit number
 *      link                - (IN) Link #
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
jer2_arad_fabric_link_status_clear(
    int unit,
    soc_port_t link
);


/*
 * Function:
 *      jer2_arad_dnx_fabric_link_status_get
 * Purpose:
 *      Get link status
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      link_id             - (IN)  Link
 *      link_status         - (OUT) According to link status get
 *      errored_token_count - (OUT) Errored token count
 * Returns:
 *      _SHR_E_xxx
 */

shr_error_e
jer2_arad_fabric_link_status_get(
  int unit,
  bcm_port_t link_id,
  uint32 *link_status,
  uint32 *errored_token_count
);

/*
* Function:
*      jer2_arad_fabric_loopback_set
* Purpose:
*      Set port loopback
* Parameters:
*      unit      - (IN)  Unit number.
*      port      - (IN)  port number 
*      loopback  - (IN)  portmod_loopback_mode_t
* Returns:
*      _SHR_E_xxx
* Notes:
*/

shr_error_e
jer2_arad_fabric_loopback_set(
  int unit,
  soc_port_t port,
  portmod_loopback_mode_t loopback
);

/*
* Function:
*      jer2_arad_fabric_loopback_get
* Purpose:
*      Get port loopback
* Parameters:
*      unit      - (IN)  Unit number.
*      port      - (IN)  port number 
*      loopback  - (OUT) portmod_loopback_mode_t
* Returns:
*      _SHR_E_xxx
* Notes:
*/

shr_error_e
jer2_arad_fabric_loopback_get(
  int unit,
  soc_port_t port,
  portmod_loopback_mode_t* loopback
);

shr_error_e jer2_arad_link_control_strip_crc_set(int unit, soc_port_t port, int strip_crc);
shr_error_e jer2_arad_link_control_strip_crc_get(int unit, soc_port_t port, int* strip_crc);

shr_error_e jer2_arad_link_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
shr_error_e jer2_arad_link_control_tx_enable_set(int unit, soc_port_t port, int enable);
shr_error_e jer2_arad_link_control_rx_enable_get(int unit, soc_port_t port, int* enable);
shr_error_e jer2_arad_link_control_tx_enable_get(int unit, soc_port_t port, int* enable);

/*
 * Function:
 *      jer2_arad_link_port_fault_get
 * Purpose:
 *      Get port loopback
 * Parameters:
 *      unit -  (IN)  BCM device number 
 *      port -  (IN)  Device or logical port number .
 *      flags - (OUT) Flags to indicate fault type 
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
shr_error_e 
jer2_arad_link_port_fault_get(
  int unit,
  bcm_port_t link_id,
  uint32* flags
);

shr_error_e jer2_arad_link_port_bucket_fill_rate_validate(int unit, uint32 bucket_fill_rate);

/*
 * Function:
 *      jer2_arad_fabric_links_nof_links_get
 * Purpose:
 *      Get number of links
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      nof_links  - (OUT) Number of links.
 * Returns:
 *      Number of links
 */
shr_error_e 
jer2_arad_fabric_links_nof_links_get(int unit, int* nof_links);

/*
 * Function:
 *      jer2_arad_fabric_force_signal_detect_set
 * Purpose:
 *      enable force signal detect in a mac
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      mac_instance  - (IN) MAC index.
 * Returns:
 *      Number of links
 */
shr_error_e
jer2_arad_fabric_force_signal_detect_set(int unit, int mac_instance);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_cell_format_set
* TYPE:
*   PROC
* DATE:
*   Mar 25 2012
* FUNCTION:
*     This procedure sets fabric links operation mode.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  JER2_ARAD_FABRIC_CELL_FORMAT  *info -
*     Fabric links operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  jer2_arad_fabric_cell_format_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  JER2_ARAD_FABRIC_CELL_FORMAT  *info
  );


/*********************************************************************
* NAME:
*     jer2_arad_fabric_link_up_get
* TYPE:
*   PROC
* DATE:
*   Apr 25 2012
* FUNCTION:
*     This procedure get link status up/down
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_IN  uint32                 link_idx -
*  DNX_SAND_OUT uint32                 *up -
*     
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_fabric_link_up_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  soc_port_t          port,
    DNX_SAND_OUT int                 *up
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_nof_links_get
* TYPE:
*   PROC
* DATE:
*   Apr 25 2013
* FUNCTION:
*     Get nof fabric links
* INPUT:
*  DNX_SAND_IN  int                 unit -
*  DNX_SAND_OUT uint32                 *nof_links -
*     
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_fabric_nof_links_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT int                    *nof_links
  );

/*
 * Function:
 *      jer2_arad_fabric_port_wcmod_ucode_load
 * Purpose:
 *      optimizing loading firmware time
 * Parameters:
 *      unit - Unit #.
 *      port - port #. (check if the firmware is already loaded for the specific port, but load firmware fo all ports if necessary
 *      arr - wcmod table
 *      arr_len - wcmod table length
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */
uint32
jer2_arad_fabric_wcmod_ucode_load(
    int unit,
    int port, 
    uint8 *arr, 
    int arr_len
);
/*********************************************************************
* NAME:
*     jer2_arad_fabric_gci_enable_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*      Set enbale state of the Local GCI mechanism (random backoff or leaky bucket)
* INPUT:
*      unit - unit #.
*      type - GCI mechanism (random backoff or leaky bucket)
*      value - 
* REMARKS:
*     Enable/disable per pipe is not supported.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_gci_enable_set ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_IN  JER2_ARAD_FABRIC_GCI_TYPE                   type,
    DNX_SAND_OUT int                                    value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_gci_enable_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*      Get enbale state of the Local GCI mechanism (random backoff or leaky bucket)
* INPUT:
*      unit - unit #.
*      type - GCI mechanism (random backoff or leaky bucket)
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_gci_enable_get ( 
    DNX_SAND_IN  int                                 unit,
    DNX_SAND_IN  JER2_ARAD_FABRIC_GCI_TYPE                   type,  
    DNX_SAND_OUT int                                    *value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_gci_config_set
* TYPE:
*   PROC
* DATE:
*   April 30 2012
* FUNCTION:
*      Set GCI  related parameters.
* INPUT:
*      unit - unit #.
*      type - The relevant parameter
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_gci_config_set ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_IN JER2_ARAD_FABRIC_GCI_CONFIG_TYPE             type,
    DNX_SAND_OUT int                                    value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_gci_config_get
* TYPE:
*   PROC
* DATE:
*   April 30 2012
* FUNCTION:
*      Get GCI related parameters
* INPUT:
*      unit - unit #.
*      type - The relevant parameter
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_gci_config_get ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_IN JER2_ARAD_FABRIC_GCI_CONFIG_TYPE             type,
    DNX_SAND_OUT int                                    *value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_llfc_threshold_set
* TYPE:
*   PROC
* DATE:
*   April 30 2012
* FUNCTION:
*      Set llfc related thresholds.
* INPUT:
*      unit - unit #.
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_llfc_threshold_set ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_OUT int                                    value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_llfc_threshold_get
* TYPE:
*   PROC
* DATE:
*   May 2012
* FUNCTION:
*      Get llfc related thresholds.
* INPUT:
*      unit - unit #.
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_llfc_threshold_get ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_OUT int                                    *value
);
/*********************************************************************
* NAME:
*     jer2_arad_fabric_rci_enable_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*      Set enbale state of the Local RCI (FDR FIFOs)
* INPUT:
*      unit - unit #.
*      value - 
* REMARKS:
*     Enable/disable per pipe is not supported.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_rci_enable_set ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_OUT soc_dnxc_fabric_control_source_t        value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_rci_enable_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*      Get enbale state of the Local RCI (FDR FIFOs)
* INPUT:
*      unit - unit #.
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_rci_enable_get ( 
    DNX_SAND_IN  int                                 unit,  
    DNX_SAND_OUT soc_dnxc_fabric_control_source_t       *value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_rci_config_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*      Set local rci related parameters.
* INPUT:
*      unit - unit #.
*      rci_config_type - 
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_rci_config_set ( 
    DNX_SAND_IN int                                unit,  
    DNX_SAND_IN JER2_ARAD_FABRIC_RCI_CONFIG_TYPE           rci_config_type,
    DNX_SAND_IN int                                   value
);

/*********************************************************************
* NAME:
*     jer2_arad_fabric_rci_config_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*      Get local rci related parameters.
* INPUT:
*      unit - unit #.
*      rci_config_type - 
*      value - 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_rci_config_get ( 
    DNX_SAND_IN  int                                 unit,
    DNX_SAND_IN  JER2_ARAD_FABRIC_RCI_CONFIG_TYPE            rci_config_type,  
    DNX_SAND_OUT int                                    *value
  );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_empty_cell_size_set
* FUNCTION:
*   Empty cell and LLFC cell size configuration.
*   Available per FMAC instance.
*   Supported by JER2_ARAD_PLUS only.
*   The remote reapeater can process a limited number of cells (according to remote reapeater core clock),
*   Setting larger cell size will reduce the empty cells rate
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                 fmac_index -
*     FMAC #
*DNX_SAND_IN  uint32                    cell_size - 
*     The empty cells size in bytes.          
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
int
jer2_arad_fabric_empty_cell_size_set (
   DNX_SAND_IN  int                                 unit,
   DNX_SAND_IN  uint32                                 fmac_index,  
   DNX_SAND_IN  uint32                                 cell_size
);



/*********************************************************************
* NAME:
*     jer2_arad_fabric_minimal_links_to_dest_set
* FUNCTION:
*   Configure minimum links to destination
* REMARKS: 
*   JER2_ARAD_PLUS Only 
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN soc_module_t         module_id -
*     FAP id to configure (SOC_MODID_ALL configures all FAPs) 
*  DNX_SAND_IN  uint32                 minimum_links
*     Minimum number of links to configure
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/

int jer2_arad_fabric_minimal_links_to_dest_set(
    DNX_SAND_IN int                                  unit,
    DNX_SAND_IN soc_module_t                         module_id,
    DNX_SAND_IN int                                  minimum_links
   );

/*********************************************************************
* NAME:
*     jer2_arad_fabric_minimal_links_to_dest_get
* FUNCTION:
*     get minimum number of links, as configured
* REMARKS: 
*   JER2_ARAD_PLUS Only 
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN soc_module_t         module_id -
*     FAP id to configure (SOC_MODID_ALL configures all FAPs) 
*  DNX_SAND_IN  uint32                * minimum_links
*     sets minimum links to the number of minimal links as configured(0 if option is disabled)
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/


int jer2_arad_fabric_minimal_links_to_dest_get(
    DNX_SAND_IN int                                 unit,
    DNX_SAND_IN soc_module_t                        module_id,
    DNX_SAND_OUT int                                *minimum_links
    );



/*********************************************************************
* NAME:
*     jer2_arad_fabric_mesh_check
* FUNCTION:
*     check mesh status
* INPUT:
*  DNX_SAND_IN  int                                  unit -
*     Identifier of the device to access.
*  DNX_SAND_IN uint8                                    stand_alone,
*     Is device stand alone.
*  DNX_SAND_OUT uint8 *success - 
*     mesh status check.
* RETURNS:
*   OK or ERROR indication.
* REMARKS: 
*   None. 
*********************************************************************/

int jer2_arad_fabric_mesh_check(int unit, uint8 stand_alone, uint8 *success);



shr_error_e jer2_arad_fabric_prbs_polynomial_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
shr_error_e jer2_arad_fabric_prbs_polynomial_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value);

void
  jer2_arad_JER2_ARAD_FABRIC_FC_clear(
    DNX_SAND_IN uint32 unit,
    DNX_SAND_OUT JER2_ARAD_FABRIC_FC *info
  );

void
  jer2_arad_JER2_ARAD_FABRIC_FC_SHAPER_clear(
      DNX_SAND_OUT JER2_ARAD_FABRIC_FC_SHAPER *shaper
  );

void
  jer2_arad_JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO_clear(
    DNX_SAND_OUT  JER2_ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode
  );

void
  jer2_arad_JER2_ARAD_FABRIC_CELL_FORMAT_clear(
    DNX_SAND_OUT JER2_ARAD_FABRIC_CELL_FORMAT *info
  );

void
  jer2_arad_JER2_ARAD_LINK_STATE_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_LINK_STATE_INFO *info
  );
const char*
  jer2_arad_SOC_DNX_FABRIC_CONNECT_MODE_to_string(
    DNX_SAND_IN SOC_DNX_FABRIC_CONNECT_MODE enum_val
  );


/* } __JER2_ARAD_FABRIC_INCLUDED__*/
#endif

