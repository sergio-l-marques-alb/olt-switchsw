/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garpcfg.h
* @purpose     GARP Configuration definitions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/

#ifndef INCLUDE_GARP_CFG_H_
#define INCLUDE_GARP_CFG_H_

#include "l7_product.h"
#include "nimapi.h"

#define L7_GARP_MAX_INTF    L7_MAX_INTERFACE_COUNT
#define L7_GARP_MAX_CFG_IDX L7_MAX_INTERFACE_COUNT


/******************************************************************************
 *     From 
 *     http://www.geocities.com/SiliconValley/Vista/8672/network/ethernet.html#A16:
 *
 *     The Format of 802.3 Frame that encapsulates GARP PDUs:
 *     
 *    +------------+-----------+-------------+-----------+
 *    | 802.3 MAC  | 802.2 LLC | Data        | CRC       |
 *    | Header     | Header    | (43 to      | Checksum  |
 *    | (14 bytes) | (3 bytes) | 1497 bytes) | (4 bytes) |
 *    +------------+-----------+-------------+-----------+
 *
 *    The first two components, MAC Header and LLC Header are further expanded below:
 * 
 *    802.3 MAC Header: 
 *
 *    +-------------+------------+-----------+
 *    | Destination | Source MAC | Length of |
 *    | MAC Address | Address    | the frame |
 *    | (6 bytes)   | (6 bytes)  | (2 bytes) |
 *    +-------------+------------+-----------+
 *
 *    802.2 LLC Header: 
 *
 *    +-------------+------------+------------+
 *    | Destination | Source     | Control    |
 *    | SAP         | SAP        | Byte       |
 *    | (1 byte)    | (1 byte)   | (1 byte)   |
 *    +-------------+------------+------------+
 ******************************************************************************/


/* Length of 802.3 MAC Header */
#define GARP_802_3_MAC_HEADER_LEN         14
                                 
/* Length of 802.2 LLC Heder (3 bytes) */
#define GARP_802_2_LLC_HEADER_LEN         3

/* Length of 802.3 MAC Header (14 bytes) + Length of 802.2 LLC Heder (3 bytes) */
#define GARP_802_3_FRAME_HEADER_LEN       GARP_802_3_MAC_HEADER_LEN + \
                                          GARP_802_2_LLC_HEADER_LEN

/* Max Length of Frame Header Portion + Data Portion + CRC*/                                       
#define GARP_MAX_PDU_SIZE                 (1497 + GARP_802_3_FRAME_HEADER_LEN) 


#define DEFAULT_GARPCONFIGJOINTIME        200
#define DEFAULT_GARPCONFIGLEAVETIME       600
#define DEAFULT_GARPCONFIGLEAVEALLTIME    10000

#define GARP_CFG_FILENAME  "garp.cfg"
#define GARP_CFG_VER_0        0x0
#define GARP_CFG_VER_1        0x1
#define GARP_CFG_VER_2        0x2
#define GARP_CFG_VER_3        0x3
#define GARP_CFG_VER_4        0x4
#define GARP_CFG_VER_CURRENT  GARP_CFG_VER_4

#define GMRP_CFG_FILENAME  "gmrp.cfg"
#define GMRP_CFG_VER_1        0x1
#define GMRP_CFG_VER_2        0x2
#define GMRP_CFG_VER_3        0x3
#define GMRP_CFG_VER_4        0x4
#define GMRP_CFG_VER_CURRENT  GMRP_CFG_VER_4

typedef L7_ushort16 Vlan_id;
typedef void        GarpPdu;

typedef enum {
  GARP_PHASE_INIT_0 = 0,
  GARP_PHASE_INIT_1,
  GARP_PHASE_INIT_2,
  GARP_PHASE_WMU,
  GARP_PHASE_INIT_3,
  GARP_PHASE_EXECUTE,
  GARP_PHASE_UNCONFIG_1,
  GARP_PHASE_UNCONFIG_2,
} garpCnfgrState_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uint32     join_time;
  L7_uint32     leave_time;
  L7_uint32     leaveall_time;
  L7_uint32     gvrp_enabled;
  L7_uint32     gmrp_enabled;
    /* applicable to GVRP only */
  L7_BOOL       vlanCreationForbid;
  L7_BOOL       registrationForbid;
} garpIntfCfgData_t;

typedef struct
{
  L7_fileHdr_t hdr;
  L7_BOOL           gvrp_enabled;
  L7_BOOL           gmrp_enabled;
  garpIntfCfgData_t intf[L7_GARP_MAX_INTF];
  L7_uint32         traceFlags;
#define     GARP_TRACE_TO_LOG       0x01    /* If set,trace to log msg. */
#define     GARP_TRACE_TO_CONSOLE   0x02    /* If set, print to console.  Else simply log the message */
#define     GARP_TRACE_INTERNAL     0x04    /* Generic internal component trace */
#define     GARP_TRACE_TIMERS       0x08    /* Trace the timer actions */
#define     GARP_TRACE_QUEUE        0x10    /* Trace messages sent to/from the queue */
#define     GARP_TRACE_PDU          0x20    /* Trace PDUs */
#define     GARP_TRACE_QUEUE_THRESH 0x40    /* Alert when queue usage above thresholds */
#define     GARP_TRACE_PROTOCOL     0x80    /* Trace internal protocol events */
L7_uint32    checkSum;
} garpCfg_t;

typedef struct gvrpMac_s
{
  L7_uchar8  mac_addr[L7_MAC_ADDR_LEN];
} gvrpMac_t;

/*********************************************************************
* @purpose  Saves GARP user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t garpSave(void);

/*********************************************************************
* @purpose  Unapply existing configuration  
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Resets active configuration without changing config parms
*       
* @end
*********************************************************************/
L7_RC_t garpUnapplyConfigData();

/*********************************************************************
* @purpose  Reset the garp active configuration to default values  
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Sets active configuration without changing config parms
*       
* @end
*********************************************************************/
L7_RC_t garpRestore(void);

/*********************************************************************
* @purpose  Checks if garp user config data is changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL garpHasDataChanged(void);
void garpResetDataChanged(void);
/*********************************************************************
* @purpose  Builds a GARP config file with defaults
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    Builds Garp, VLAN and port default data which will be
*           applied in garpApplyConfigData
*
* @end
*********************************************************************/
void garpBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply GARP config data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t garpApplyConfigData(void);

/*********************************************************************
* @purpose  Apply configuration (or defaults) for an interface
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invoked for an L7_ATTACH or during an apply config
*
* @end
*
*********************************************************************/
L7_RC_t garpApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dot1q interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL garpMapIntfIsConfigurable(L7_uint32 intIfNum, garpIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to snoop nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL garpMapIntfConfigEntryGet(L7_uint32 intIfNum, garpIntfCfgData_t **pCfg);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Allocates intf config storage if a config does not exist
*
* @end
*
*********************************************************************/
L7_RC_t garpIntfCreate(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Register garp interface counters with the stats manager
*
* @param    None    
*
* @returns  L7_SUCCESS or L7_FALIURE
*       
* @end
*********************************************************************/
L7_RC_t garpStatsIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t garpIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the join time for a port
*
* @param    first_port   start of gid ring
* @param    port         port to set
* @param    join_time    join time to set
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void setJoinTime(void *first_port, L7_uint32 port, L7_uint32 join_time);

/*********************************************************************
* @purpose  Set the leave time for a port
*
* @param    first_port   start of gid ring
* @param    port         port to set
* @param    join_time    leave time to set
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void setLeaveTime(void *first_port, L7_uint32 port, L7_uint32 leave_time);

/*********************************************************************
* @purpose  Set the leaveall time for a port
*
* @param    first_port   start of gid ring
* @param    port         port to set
* @param    join_time    leaveall time to set
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void setLeaveAllTime(void *first_port, L7_uint32 port, L7_uint32 leaveall_time);

/*********************************************************************
* @purpose  enable or disable gvrp application
*
* @param    enable    application mode
*
* @returns  none
*
* @notes     
*
* @end
*********************************************************************/
void setGvrpApp(L7_BOOL enable);

/*********************************************************************
* @purpose  enable or disable gmrp application
*
* @param    enable    application mode
*
* @returns  none
*
* @notes     
*
* @end
*********************************************************************/
void setGmrpApp(L7_BOOL enable);

/* garp_migrate.c */

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void garpMigrateConfigData(L7_uint32 oldVer , L7_uint32 ver, L7_char8 *pCfgBuffer);


/*********************************************************************
* @purpose  Apply GVRP interface mode
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
L7_RC_t garpApplyIntfGvrpMode(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Apply GMRP interface mode
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
L7_RC_t garpApplyIntfGmrpMode(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  setVlanCreationForbid(void *first_port, L7_uint32 interface, L7_BOOL forbid);

/*********************************************************************
* @purpose  Set the value of VLAN registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  setVlanRegistrationForbid(void *first_port, L7_uint32 interface, L7_BOOL forbid);

#endif

