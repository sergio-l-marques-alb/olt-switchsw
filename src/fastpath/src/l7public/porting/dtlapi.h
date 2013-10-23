/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dtlapi.h
*
* @purpose This files contains the necessary prototypes,
*          structures, and #defines for the Device Transformation Layer
*          interface.
*
* @component Device Transformation Layer
*
* @comments none
*
* @create 9/14/2000
*
* @author Alex Vasquez
* @end
*
**********************************************************************/

#ifndef INCLUDE_DTL_API_H
#define INCLUDE_DTL_API_H

#include "platform_counters.h"
#include "dapi.h"
#include "registry.h"
#include "statsapi.h"
#include "osapi.h"
#include "nimapi.h"
#include "sysnet_api.h"
#include "sysapi_hpc.h"
#include "comm_mask.h"
#include "fdb_exports.h"
#include "mirror_exports.h"
#include "dot1ad_api.h"
#include "l7_cnfgr_api.h"

/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/

/*
* The following datatype represents the events used by the DTL event
* handling engine.
*/

/*********************************************************************
* @purpose     DTL event definitions
*
* @notes none
*
*********************************************************************/

typedef enum
{
  DTLCTRL_E1 = 1,            /* Event 1                        */
  DTLCTRL_E2,                /*       2                        */
  DTLCTRL_E3,                /*       3                        */
  DTLCTRL_E4,                /*       4                        */
  DTLCTRL_E5,                /*       5                        */

  DTLCTRL_E99 = 99           /*       99                       */
} DTLCTRL_EVENT_t, *P_DTLCTRL_EVENT_t;

#define DTLCTRL_LNK_UP_EVNT     DTLCTRL_E1   /* Link is up        */
#define DTLCTRL_LNK_DWN_EVNT    DTLCTRL_E2   /* Link is down      */
#define DTLCTRL_INTF_ADD_EVNT   DTLCTRL_E3   /* Interface created */
#define DTLCTRL_INTF_DEL_EVNT   DTLCTRL_E4   /* Interface deleted */

/*  Layer 2 Forwarding Data Base Manager definition */

#define DTLCTRL_MAC_ADD_EVNT    DTLCTRL_E1   /* Add MAC Address   */
#define DTLCTRL_MAC_DEL_EVNT    DTLCTRL_E2   /* Delete MAC Address*/

/*  Unknown event definition                       */

#define DTLCTRL_UNKWN_EVNT      DTLCTRL_E99  /* Unknown event     */

#define L7_DTL_PORT_IF          "dtl"        /* Interface Name    */
#define L7_DTL_PORT_IF_LEN      4            /* Interface Name length  */
#define L7_DTL_MAX_PRIORITY     7            /* Maximum number of traffic class priorities */

#define L7_DTL_PKT_F_TIMESYNC   0x10000000   /* PTIN added: PTP Timestamp BCM_PKT_F_xxx flags. Packet is for Time Sync */


/* Layer 2 Protocol-Based Vlan definitions */

#define DTL_PROTOCOL_HEX_IP      0x0800
#define DTL_PROTOCOL_HEX_ARP     0x0806
#define DTL_PROTOCOL_HEX_IPX     0xffff


typedef enum
{
  DTL_L2_DOT1Q_PORT_CONFIG_VID_SET           = 1,
  DTL_L2_DOT1Q_PORT_CONFIG_ACCEPT_FRAME_TYPE = 2,
  DTL_L2_DOT1Q_PORT_CONFIG_INGRESS_FILTER    = 4,
  DTL_L2_DOT1Q_PORT_CONFIG_PROTOCOL          = 8

} DTL_L2_DOT1Q_PORT_CONFIG_PARAMETER_MASK_t;


/*********************************************************************
* @purpose  DTL family definitions
*
* @notes    Maps to similar family in dapi.h
*
*********************************************************************/
typedef enum
{

  DTL_FAMILY_CONTROL = 1,
  DTL_FAMILY_SYSTEM,
  DTL_FAMILY_INTF_MGMT,
  DTL_FAMILY_LAG_MGMT,
  DTL_FAMILY_ADDR_MGMT,
  DTL_FAMILY_QVLAN_MGMT,
  DTL_FAMILY_GARP_MGMT,
  DTL_FAMILY_ROUTING_MGMT,
  DTL_FAMILY_ROUTING_ARP_MGMT,
  DTL_FAMILY_ROUTING_INTF_MGMT,
  DTL_FAMILY_FRAME,
  DTL_FAMILY_POE,
  DTL_FAMILY_QOS_ACL_MGMT,
  DTL_FAMILY_POE_MGMT,
  DTL_FAMILY_SERVICES

}DTL_FAMILY_t;


/*********************************************************************
* @purpose  Defining the possible DTL events that components can register for
*
* @notes
*
*********************************************************************/
typedef enum
{
  DTL_EVENT_QVLAN_MEMBER_QUERY,
  DTL_EVENT_ADDR_INTF_MAC_QUERY,
  DTL_EVENT_ROUTING_ARP_ENTRY_QUERY,
  DTL_EVENT_PORT_DOT1S_STATE_QUERY,
  DTL_EVENT_SERVICES_VLAN_TAG_QUERY,
  DTL_EVENT_PORT_DOT1S_STATE_SET_RESPONSE, 

  /*always the last in the enum*/
  DTL_NUM_EVENT
}DTL_EVENT_t;

/*********************************************************************
* @purpose  typedef of the registering function with DTL
*
* @notes    All attribute callback functions must be of this type
*
*********************************************************************/
typedef L7_RC_t (*DTL_ATTR_FUNC_t)(L7_uint32 intfNum, DTL_EVENT_t dtlEvent, void *data, L7_uint32 *numOfElements);

/*********************************************************************
* @purpose  structure for broadcast storm recovery information
*
* @notes
*
*********************************************************************/
typedef struct DTL_BCAST_s
{
  L7_uint32 status;
  L7_uint32 highThreshold;
  L7_uint32 lowThreshold;

}DTL_BCAST_t;


/*********************************************************************
* @purpose  DTL Send Frame Type definitions
*
* @notes    Maps to similar frame types in dapi.h
*
*********************************************************************/
typedef enum
{
  DTL_NORMAL_UNICAST,
  DTL_VLAN_MULTICAST,
  DTL_L2RAW_UNICAST,
  DTL_L2RAW_VLAN_MULTICAST,

  DTL_NUM_OF_FRAME_TYPE   /*always the last in the enum*/

}DTL_FRAME_TYPE_t;



/*********************************************************************
* @purpose   DTL system MAC address types
*
* @notes    Maps to similar flag types in dapi.h
*
*********************************************************************/
typedef enum
{

  DTL_MAC_TYPE_BURNED_IN,
  DTL_MAC_TYPE_LOCAL_ADMIN

} DTL_MAC_TYPE_t;



/*********************************************************************
* @purpose  DTL command types
*
* @notes
*
*********************************************************************/

typedef enum
{
  DTL_CMD_TX_L2,
  DTL_CMD_TX_L3,

  DTL_CMD_NUM_CMDS  /*always the last */

}DTL_CMD_t;

/*********************************************************************
* @purpose  DTL DHCP and BootP config enum
*
* @notes    Maps to similar flag types in dapi.h
*
*********************************************************************/
typedef enum
{
  DTL_DHCP_CONFIG_NONE,
  DTL_DHCP_CONFIG_DHCP,
  DTL_DHCP_CONFIG_BOOTP,
  DTL_DHCP_CONFIG_DHCP_AND_BOOTP

} DTL_DHCP_CONFIG_t;

typedef struct
{
  L7_uint32              intIfNum;
  L7_MIRROR_DIRECTION_t  probeType;
}dtlMirrorSrcPortInfo_t;

/*********************************************************************
* @purpose  Port Mirroring information structure
*
* @notes    Map to 2 similar structures in dapi.h
*
*********************************************************************/
typedef struct
{
  union
  {
    struct
    {
      dtlMirrorSrcPortInfo_t *srcPortInfo;
      L7_uint32          numOfSrcPortEntries;
      L7_uint32          destPort; 
      L7_BOOL            enable;
      L7_uint32          probeType;
      L7_BOOL            probeSpecified;
      L7_uint32          sessionNum;
    } config;

    struct
    {
      L7_uint32          srcPort;
      L7_BOOL            add;
      L7_uint32          probeType;
      L7_uint32          sessionNum;
    } modify;

  } cmdType;

} DTL_MIRROR_t;


/*********************************************************************
* @purpose  Transmission information structure
*
* @notes    to lower layer
*
*********************************************************************/
typedef struct
{
  L7_uint32 intfNum;
  L7_uint32 priority;
  DTL_FRAME_TYPE_t typeToSend;

  union
  {
    struct
    {
      L7_uint32 domainId;
      L7_uint32 vlanId;
      L7_uint32 flags;    /* PTIN added: PTP Timestamp BCM_PKT_F_xxx flags. */

    }L2;

  }cmdType;


}DTL_CMD_TX_INFO_t;

/*********************************************************************
* @purpose   DTL USP structure
*
* @notes
*
*********************************************************************/
typedef struct dtlUsp_s
{
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
}DTL_USP_t;


typedef enum
{
  DTL_L2_MCAST_STATIC,
  DTL_L2_MCAST_DYNAMIC,
  DTL_L2_MCAST_ABSOLUTE

}DTL_L2_MCAST_TYPE_t;

/*********************************************************************
* @purpose   DTL L2 Multicast structure
*
* @notes
*
*********************************************************************/
typedef struct dtll2Multi_s
{
  L7_uint32             vlanId;
  L7_uchar8             *macAddr;
  NIM_INTF_MASK_t       *fwdMask;
  NIM_INTF_MASK_t       *filterMask;
  DTL_L2_MCAST_TYPE_t   type;

}DTL_L2_MCAST_ENTRY_t;



typedef enum
{
  DTL_CARD_INSERT,
  DTL_CARD_REMOVE,
  DTL_CARD_PLUGIN_START,
  DTL_CARD_PLUGIN_FINISH,
  DTL_CARD_UNPLUG_START,
  DTL_CARD_UNPLUG_FINISH

} DTL_CARD_CMD_t;

typedef struct dvlantag_dtl_s
{
  L7_uint32 mode;
  L7_BOOL   enable;
  L7_uint32 etherType;
  L7_uint32 etherMask;
  L7_uint32 custId;
  DOT1AD_INTFERFACE_TYPE_t dot1adIntfType;
}DVLANTAG_DTL_t;

typedef struct dvlantag_dtl_global_s
{
  L7_uint32 etherType;
  L7_uint32 tpidIdx;
}DVLANTAG_DTL_GLOBAL_t;

/*
********************************************************************
*                           FUNCTION PROTOTYPES
********************************************************************
*/



/*
/------------------------------------------------------------------\
*                            CONTROL APIs                          *
\------------------------------------------------------------------/
*/


/* DTL  Interface APIs    */

/*********************************************************************
* @purpose  Sets the speed of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    intfSpeed   @b{(input)} Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_AUTO_NEG,
*                             L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfSpeedSet(L7_uint32 intIfNum,
                                L7_uint32 intfSpeed);


/*********************************************************************
* @purpose  Gets the speed of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *intfSpeed  @b{(output)} Pointer to Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_AUTO_NEG,
*                             L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfSpeedGet(L7_uint32 intIfNum,
                                L7_uint32 *intfSpeed);

/*********************************************************************
* @purpose  Test the copper cable and retrieve test results.
*           The length values are reported in meters.
*
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    *cableStatus         @b{(output)} status of the copper cable.
* @param    *lengthKnown         @b{(output)} when L7_TRUE the length is known.
* @param    *shortestLength      @b{(output)} low end of the length estimate.
* @param    *longestLength       @b{(output)} high end of the length estimate.
* @param    *cableFailureLength  @b{(output)} failure location.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The function may take a couple of seconds to execute.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlintfCableStatusGet(L7_uint32 intIfNum,
                                      L7_CABLE_STATUS_t *cableStatus,
                                      L7_BOOL   *lengthKnown,
                                      L7_uint32 *shortestLength,
                                      L7_uint32 *longestLength,
                                      L7_uint32 *cableFailureLength
                                     );


/*********************************************************************
* @purpose  Sets the administrative state of the specified interface to
*           either enabled or disabled
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    adminState  @b{(input)} Admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfAdminStateSet(L7_uint32 intIfNum,
                                     L7_uint32 adminState);


/*********************************************************************
* @purpose  Sets the (Layer 2) Maximum Framesize for the interface
*           based upon the type of interface and any submembers per port
*
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    length      @b{(input)} Maximum frame size of the interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfMaxFrameSizeSet(L7_uint32 intIfNum,
                                       L7_uint32 length);


/*********************************************************************
* @purpose  Creates a logical interface in the driver
*
* @param    intIfNum    @b{(input)} Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine can only be used to create a logical interface
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfLogicalIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Deletes a logical interface in the driver
*
* @param    intIfNum    @b{(input)}  Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine can only be used to create a logical interface
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfLogicalIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Sets the interface in loopback mode.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} Link state
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE }}
* @param    usp         @b{(input)} NIM usp structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    None.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfLoopbackModeSet(L7_uint32 intIfNum,
                                       L7_uint32 mode,
                                       nimUSP_t usp);



/*********************************************************************
* @purpose  Puts the specified interface into the equivalent of STP
*           Disabled mode
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    "Soft Disable" is a quiescent state whereby an interface
*           discards all incoming and outgoing packets.  It is
*           operationally equivalent to STP Disabled mode.  When issued
*           to a macroport interface, the device driver puts all
*           macroport members into "Soft Disabled".
*
* @notes    There is not a separate DTL command to release an interface
*           from "Soft Disable".  This happens naturally when commands
*           are issued to the interface during normal course of operation,
*           such as when told to go into forwarding mode.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfSoftDisable(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Configure port mirroring feature
*
* @param    *dtlCmd   @b{(input)} Port Mirroring configuration data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlMirrorSessionConfig(DTL_MIRROR_t *dtlCmd);

/*********************************************************************
* @purpose  Modify a mirror session
*
* @param   *dtlMirrorCmd  @b{(input)} Mirroring configuration data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/

EXT_API L7_RC_t dtlMirrorPortModify(DTL_MIRROR_t *dtlMirrorCmd);

/*********************************************************************
* @purpose  Gets the Maximum Transfer Unit for the interface
*
* @param        intIfNum  @b{(input)} Internal Interface Number
* @param        *mtuSize  @b{(output)} MTU of the interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlIntfMtuSizeGet(L7_uint32 intIfNum,
                                  L7_uint32 *mtuSize);


/*********************************************************************
* @purpose  Test the copper cable and retrieve test results.
*           The length values are reported in meters.
*
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    *sfpLinkUp           @b{(output)} sfp Link on interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlintfSfpLinkUpGet(L7_uint32 intIfNum, L7_BOOL *sfpLinkUp);

/*********************************************************************
* @purpose  Gets the Link State of the specified interface
*
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    *isLinkUp   @b{(output)}  The link state of the interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfLinkStateGet(L7_uint32 intIfNum, L7_BOOL *isLinkUp);

/*********************************************************************
* @purpose  Gets the Rx and Tx pause State of the specified interface
*
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    *isRxPauseAgreed   @b{(output)}  L7_TRUE if the link partners
*                                     have agreed to use RX Pause
* @param    *isTxPauseAgreed   @b{(output)}  L7_TRUE if the link partners
*                                     have agreed to use TX Pause
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfPauseStateGet(L7_uint32 intIfNum, L7_BOOL *isRxPauseAgreed, L7_BOOL *isTxPauseAgreed);

/* DTL IP APIs             */

/*********************************************************************
* @purpose  Sets the IP Address for the system
*
* @param    ipAddr   @b{(input)} IP Address for the system in 32 bit notation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIPAddrSystemSet(L7_ulong32 ipAddr);


/*********************************************************************
* @purpose  Configures BOOTP and/or DHCP for the system
*
* @param    config    @b{(input)} enumerates type of configuration
*                     @b{ DTL_DHCP_CONFIG_NONE,
*                         DTL_DHCP_CONFIG_DHCP,
*                         DTL_DHCP_CONFIG_BOOTP,
*                         DTL_DHCP_CONFIG_DHCP_AND_BOOTP
*                       }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_t config);


/* DTL IPx APIs            */



/* DTL Driver APIs         */

/*********************************************************************
* @purpose  Initializes the card
*
* @param    unitNum     @b{(input)} Unit number
* @param    slotNum     @b{(input)} Slot number
* @param    *pSR        @b{(input)} Pointer to the slot registry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlCardCmd(L7_ushort16    unitNum,
                   L7_ushort16    slotNum,
                   DTL_CARD_CMD_t cardCmd,
                   L7_ulong32     cardTypeID);

/*********************************************************************
* @purpose  Start the driver based on the startupReason code
*
*
* @params  systemStartupreason {(input)} Type of startup
* @params  mgrFailover         {(input)} Indicates that the unit is
*                                        becoming mgr after a failover
*          lastMgrUnitId       {(input)} Unit number of the last mgr
*          lastMgrKey          {(input)} Key of the last mgr
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Driver translates various reason codes into either:
*            - warm start
*            - cold start
*           
*            If warm start was requested but could not be performed
*            then cold start is initiated.
*
* @end
*********************************************************************/
L7_RC_t dtlDriverStart(L7_LAST_STARTUP_REASON_t systemStartupReason,
                       L7_BOOL                  mgrFailover,
                       L7_uint32                lastMgrUnitId,
                       L7_enetMacAddr_t         lastMgrKey);

/*********************************************************************
* @purpose  Notify the driver that Hw updates are complete for a phase
*
* @param    phase {(input)} Phase for which Hw Update is complete
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes  
* @end
*********************************************************************/

L7_RC_t dtlDriverHwApplyNotify(L7_CNFGR_HW_APPLY_t phase);


/*********************************************************************
* @purpose  Notify the driver that a unit has been disconnected from
*           the stack
*
* @param    unit {(input)} unit number 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes  
* @end
*********************************************************************/
L7_RC_t dtlDriverUnitDisconnectNotify(L7_uint32 unit);

/*********************************************************************
* @purpose  Initialize the Driver
*
* @param    cpuBoardID   @b{(input)} ID of the CPU Board
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlDriverInit(L7_ulong32 cardTypeID);

/*********************************************************************
* @purpose  Execute commands on a driver shell
*
* @param    str    string needed to be executed on the debug shell
*
* @returns 
*
* @notes
*
* @end
*********************************************************************/
void dtlDriverShell(const L7_uchar8 str[]);


/* DTL Statistics APIs     */


/*********************************************************************
* @purpose  Gets a specific counter
*
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    statsMgrCounterID   @b{(input)} Statistics Manager Counter ID
* @param    counterSize         @b{(input)} Size of counter
* @param    *counterValue       @b{(output)} Pointer to returned counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlStatsGet (L7_uint32 intIfNum,
                             L7_uint32 statsMgrCounterID,
                             L7_uint32 counterSize,
                             L7_ulong64 *counterValue);



/* DTL PBVLAN APIs       */


/*********************************************************************
* @purpose  Configures PVID, based on the protocol
*
* @param    intIfNum    @b{(input)} Internal Interface Number to config
* @param    *protocol   @b{(input)} protocol to add or delete
* @param    vlanid      @b{(input)} VLAN ID
* @param    pbvlan_cmd  @b{(input)} L7_TRUE - Add protocol
*                                   L7_FALSE - Delete protocol
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPbVlanPortConfig(L7_uint32 intIfNum,
                                    L7_uint32 prtl,
                                    L7_uint32 vlanid,
                                    L7_BOOL   pbvlan_cmd);

/*********************************************************************
* @purpose  Configures PVID, based on the protocol
*
* @param    protoIndex  @b{(input)} Protocol index in which protocol is added
* @param    protoId     @b{(input)} Protocol to add or delete.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPbVlanProtocolConfig(L7_uint32 protoId,
                            L7_uint32 protoIndex);

/*********************************************************************
* @purpose  Maps the hex value of a protocol to a decimal value.
*
* @param    prtl  @b{(input)} Ether type of the protocol.
* @param    *prot      @b{(output)} DAPI protocol type.
*                                   0 - ip protocol
*                                   1 - arp protocol
*                                   2 - ipx protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPbVlanProtocolGet(L7_uint32 prtl,
                             L7_uint32 *prot);



/* DTL L2 DOT1Q APIs        */

/*********************************************************************
* @purpose  Deletes port(s) from an existing VLAN
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
* @param    *nMS        @b{(input)} Memberset, list of slot/ports to delete
* @param    nmsNum      @b{(input)} Number of members to delete
* @param    *nTS        @b{(input)} Taggedset, list of slot/ports to be untagged
* @param    ntsNum      @b{(input)} Number of members to be flagged as untagged
* @param    *nGF        @b{(input)} Slot/ports not in groupfilter
* @param    ngfNum      @b{(input)} Number of slot/ports not in groupfilter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qDeletePort(L7_ulong32 fdbID,
                                   L7_ushort16 vlanID,
                                   L7_ushort16 *nMS,
                                   L7_uint32 nmsNum,
                                   L7_ushort16 *nTS,
                                   L7_uint32 ntsNum,
                                   L7_ushort16 *nGF,
                                   L7_uint32 ngfNum);


/*********************************************************************
* @purpose  Adds port(s) to an existing VLAN
*
* @param     fdbID       @b{(input)} Filtering DB ID
* @param     vlanID      @b{(input)} VLAN ID
* @param     *MS_intf    @b{(input)} Memberset, list of interfaces to add
* @param     msNum       @b{(input)} Number of members in memberset
* @param     *TS_intf    @b{(input)} list of interfaces to be tagged
* @param     tsNum       @b{(input)} Number of members in taggedset
* @param     *GF_intf    @b{(input)} Groupfilter
* @param     gfNum       @b{(input)} Number of members in groupfilter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The interfaces can be physical or logical
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qAddPort(L7_ulong32 fdbID,
                                L7_ushort16 vlanID,
                                L7_ushort16 *MS_intf,
                                L7_uint32 msNum,
                                L7_ushort16 *TS_intf,
                                L7_uint32 tsNum,
                                L7_ushort16 *GF_intf,
                                L7_uint32 gfNum);


/*********************************************************************
* @purpose  Deletes a VLAN and its members
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qPurge(L7_ulong32 fdbID,
                              L7_ushort16 vlanID);


/*********************************************************************
* @purpose  Creates a VLAN with no members
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qCreate(L7_ulong32 fdbID,
                               L7_ushort16 vlanID);
/*********************************************************************
* @purpose  Creates VLANs with no members
*
* @param    vlanMask      @b{(input)} VLAN ID Mask
* @param    numVlans      @b{(input)} Num of vlans set in the mask
* @param    *vlanMaskFailure @b{(output)} Vlan mask of vlans that were not created
* @param    *vlanFailureCount @b{(output)} Num of vlan that were not created
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qCreateMask(L7_VLAN_MASK_t *vlanMask, L7_uint32 numVlans, L7_VLAN_MASK_t* vlanMaskFailure, L7_uint32 *vlanFailureCount);
/*********************************************************************
* @purpose  Deletes VLANs and its members
*
* @param    vlanMask      @b{(input)} VLAN ID Mask
* @param    numVlans      @b{(input)} Num of vlans set in the mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qPurgeMask(L7_VLAN_MASK_t *vlanMask, L7_uint32 numVlans);
/*********************************************************************
* @purpose  Configures PVID, acceptable frame type and ingress filtering
*
* @param    intIfNum       @b{(input)} Internal Interface Number to config
* @param    parameterMask  @b{(input)} Which is being modified
*                                      bit 0 - VID
*                                      bit 1 - ingress filter mode
*                                      bit 2 - acceptable frame types
*                                      bit 3 - dot1q mode
* @param    pvid           @b{(input)} Port VLAN ID
* @param    type           @b{(input)} Acceptable frame type
*                          L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
* @param    filter         @b{(input)} Ingress filtering
*                          L7_ENABLE, L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qPortConfig(L7_uint32 intIfNum,
                                   DTL_L2_DOT1Q_PORT_CONFIG_PARAMETER_MASK_t parameterMask,
                                   L7_uint32 pvid,
                                   L7_uint32 type,
                                   L7_uint32 filter);


/*********************************************************************
* @purpose  Adds port(s) to an existing VLAN
*
* @param     fdbID       @b{(input)} Filtering DB ID
* @param     vlanID      @b{(input)} VLAN ID
* @param     *MS_intf    @b{(input)} Memberset, list of interfaces to add
* @param     msNum       @b{(input)} Number of members in memberset
* @param     *TS_intf    @b{(input)} list of interfaces to be tagged
* @param     tsNum       @b{(input)} Number of members in taggedset
* @param     *GF_intf    @b{(input)} Groupfilter
* @param     gfNum       @b{(input)} Number of members in groupfilter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The interfaces can be physical or logical
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qConfigInterface(L7_uint32 intIfNum,  
                                L7_uchar8 *memberSet, 
                                L7_uchar8 *taggedSet);


/*********************************************************************
* @purpose  Adds port(s) to an existing VLAN
*
* @param     fdbID       @b{(input)} Filtering DB ID
* @param     vlanID      @b{(input)} VLAN ID
* @param     *MS_intf    @b{(input)} Memberset, list of interfaces to add
* @param     msNum       @b{(input)} Number of members in memberset
* @param     *TS_intf    @b{(input)} list of interfaces to be tagged
* @param     tsNum       @b{(input)} Number of members in taggedset
* @param     *GF_intf    @b{(input)} Groupfilter
* @param     gfNum       @b{(input)} Number of members in groupfilter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The interfaces can be physical or logical
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qUnConfigInterface(L7_uint32 intIfNum,  
                                L7_uchar8 *memberSet);


/*********************************************************************
* @purpose  Disables GVRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qGVRPDisable(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Enables GVRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qGVRPEnable(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Configures GVRP / GMRP globally
*
* @param    appl        @b{(input)} Application to configure
*                       @b{{GARP_GVRP_APP
*                           GARP_GMRP_APP}}
*
* @param    mode        @b{(input)} Enable/Disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1qGvrpGmrpConfig(L7_uint32 appl, L7_BOOL mode);

/*********************************************************************
* @purpose  Sets the traffic class for the specified priority
*
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    priority      @b{(input)} traffic class priority
* @param    trafficClass  @b{(input)} traffic class
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1dTrafficClassSet(L7_uint32 intIfNum,
                                        L7_uint32 priority,
                                        L7_uint32 trafficClass);



/*********************************************************************
* @purpose  Sets the default port priority.
*
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    priority      @b{(input)} traffic class priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1dDefaultPortPrioritySet(L7_uint32 intIfNum,
                                       L7_uint32 priority);


/* DTL L2 DOT1D APIs        */

/*********************************************************************
* @purpose  Disables GMRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1dGMRPDisable(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Enables GMRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1dGMRPEnable(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Adds a Group Registration Entry in the Multicast Table
*
* @param    *grpEntry           @b{(input)} Pointer to Group Entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    If the entry does not exist, a new entry is created.
*           if an entry exists the entry is replaced with this one.
*           This API assumes that the deafult behaviour is normal registration (for static)
*           i.e. all the ports not in the fwd or filter mask will be assumed to be
*           set to 'use dynamic information' Refer IEEE 802.1D section 7.9 for
*           futher details.
*           and filter for dynamic registration
*           i.e. for a dynamic entry type all ports not in forward mask will be
*           set to filter and the filter mask will be ig
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2McastEntryAdd(DTL_L2_MCAST_ENTRY_t *grpEntry);

/*********************************************************************
* @purpose  Deletes a Group Registration Entry from the Multicast table
*
* @param    vlanId              @b{(input)} VLAN ID for this entry
* @param    macAddr             @b{(input)} Pointer to the MAC Addr of this entry
* @param    type                @b{(input)} type of entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2McastEntryDelete(L7_uint32 vlanId,
                                      L7_uchar8 *macAddr,
                                      DTL_L2_MCAST_TYPE_t type);

/*********************************************************************
* @purpose  Sets the mcast flood mode for a VLAN
*
* @param	vlanId      		@b{(input)} VLAN ID for this entry 
* @param	mode                @b{(input)} flood mode
*           
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL2McastVlanFloodModeSet(L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t mode);

/* DTL L2 FDB APIs     */

/*********************************************************************
* @purpose  Sets the system MAC address
*
* @param    *macAddr    @b{(input)} Pointer to MAC address to be added
* @param    type        @b{(input)} Type of system MAC address
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacAddrSystemSet(L7_uchar8 *macAddr,
                                       L7_ushort16 vlanId,
                                       DTL_MAC_TYPE_t type);

/*********************************************************************
* @purpose  Clear the last system MAC that was set.
*
* @param    none
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    none
*           
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacAddrSystemClear(void);

/*********************************************************************
* @purpose  Adds a mac address to the address management table
*
* @param    *macAddr    @b{(input)} MAC address to be added
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    filterDbID  @b{(input)} Filter database ID
* @param    flag        @b{(input)} Type of flag for MAC address
* @param    *dtlPtr     @b{(input)} void pointer for passing a structure
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    The dtlPtr is for future use so that this API's
* @notes    interface need not change
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacAddrAdd(L7_uchar8 *macAddr,
                                 L7_uint32 intIfNum,
                                 L7_ulong32 filterDbID,
                                 L7_FDB_ADDR_FLAG_t flag,
                                 void *dtlPtr);


/*********************************************************************
* @purpose  Modifies a mac address in the address management table
*
* @param    *macAddr    @b{(input)} MAC address to be added
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    filterDbID  @b{(input)} Filter database ID
* @param    flag        @b{(input)} Type of flag for MAC address
* @param    *dtlPtr     @b{(input)} void pointer for passing a structure
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    The dtlPtr is for future use so that this API's
* @notes    interface need not change
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacAddrModify(L7_uchar8 *macAddr,
                                    L7_uint32 intIfNum,
                                    L7_ulong32 filterDbID,
                                    L7_FDB_ADDR_FLAG_t flag,
                                    void *dtlPtr);


/*********************************************************************
* @purpose  Deletes a mac address from the address management table
*
* @param    *macAddr    @b{(input)} Pointer to MAC address to be added
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    flag        @b{(input)} Type of flag for MAC address
* @param    filterDbID  @b{(input)} Filter database ID
* @param    *dtlPtr     @b{(input)} void pointer for passing a structure
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    The dtlPtr is for future use so that this API's
* @notes    interface need not change
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacAddrDelete(L7_uchar8 *macAddr,
                                    L7_uint32 intIfNum,
                                    L7_ulong32 filterDbID,
                                    L7_FDB_ADDR_FLAG_t flag,
                                    void *dtlPtr);


/**************************************************************************
* @purpose  Changes the system network mac address between Burned-In or
* @purpose  Locally Administered Mac Address
*
* @param    *newMac  @b{(input)} L7_uchar8 pointer to a mac address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none.
*
* @end
*************************************************************************/
EXT_API L7_int32 dtlFdbMacAddrChange( L7_uchar8 *newMac );


/*********************************************************************
* @purpose  Sets the FDB address aging time out
*
* @param    filterDbID       @b{(input)} Filter data base id
* @param    time             @b{(input)} Aging time to be set
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    time is in seconds
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbAddressAgingTimeOutSet(L7_uint32 filterDbID,
                                             L7_uint32 time);

/*********************************************************************
* @purpose  Flushes specific VLAN entries in fdb.
*
* @param    vlanId @b((input)) VLAN number.
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbFlushByVlan(L7_ushort16 vlanId);

/*********************************************************************
* @purpose  Flushes All MAC specific entries in fdb.
*
* @param    mac @b((input)) MAC address
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbFlushByMac(L7_enetMacAddr_t mac);

/*********************************************************************
* @purpose  Sets the MAC addr type for the interface
*
* @param        intIfNum  @b{(input)} Internal Interface Number
* @param        addrType  @b{(input)} Type of MAC address
*                     @b{DTL_MAC_TYPE_BURNED_IN,
*                        DTL_MAC_TYPE_LOCAL_ADMIN
*                       }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacAddrTypeSet(L7_uint32 intIfNum,
                                     DTL_MAC_TYPE_t addrType);

/*********************************************************************
* @purpose  Retrives the internal interface number associated with a MAC address
*
* @param    *mac        pointer to the mac address to search for
* @param    *intIfNum   pointer to the interface number to be returned
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    wrapper routine to retrive the internal interface number associated
*           with a MAC address from the arp table maintained in DTL
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbMacToIntfGet(L7_uchar8 *mac, L7_uint32 *intIfNum);


/*********************************************************************
* @purpose  Configures Port MAC Locking on an interface
*
* @param    intfNum          @b{(input)} Internal intf number
* @param    lockEnabled      @b{(input)} locking configuration
* @param    dynamicLimit     @b{(input)} dynamic limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlPmlLockConfig(L7_uint32 intfNum, L7_BOOL lockEnabled);

/*********************************************************************
* @purpose  Indicates to the driver that it should synchronize
*           all of it's dynamic FDB entries with the application.
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @end
*********************************************************************/
L7_RC_t dtlFdbSync();

/*********************************************************************
* @purpose  Enable/Disable LLDP for an interface
*
* @param    mode  @b{(input)} lldp mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlLldpModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/* DTL L2 LAG APIs          */

/*********************************************************************
* @purpose  Informs the Advanced Network Device Software and microcode about the latest
*           set of members for a LAG
*
* @param    intIfNum       @b{(input)} Internal Interface Number of the LAG
* @param    memberCount    @b{(input)} Total number of member interfaces in the LAG
* @param    memberList[]   @b{(input)} Array of 'memberCount' member interfaces in the LAG
* @param    activeCount    @b{(input)} Number of active member interfaces in the LAG
* @param    activeList[]   @b{(input)} Array of 'activeCount' member interfaces in the LAG
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if any specified interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This one command is used for all LAG configuration operations.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlLagConfig(L7_uint32 intIfNum,
                             L7_uint32 memberCount,
                             L7_uint32 memberList[],
                             L7_uint32 activeCount,
                             L7_uint32 activeList[]);

/*********************************************************************
* @purpose  Enables or disables LACP PDU and Marker Response PDU
*           to be sent to the CPU from an interface
*
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    mode      @b{(input)} L7_ENABLE to enable LACP PDU and Marker
*                                 Response PDU to be SENT TO THE cpu
*                                 L7_DISABLE to drop these PDUs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot3adLacpSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the driver to finish its processing before application 
*           tells the rest of the system.
*           
*
* @param    timeout {(input)} Time (secs) to wait for driver to finish
*                             pending operations 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot3adSynchronize(L7_int32 timeout);

/*********************************************************************
* @purpose  Sets the Hash Mode for a LAG
*
*
* @param    intIfNum    @b{(input)} Internal Interface Number of the interface
* @param    hashMode    @b{(input)} Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adHashModeSet (L7_uint32 lagIfNum, L7_uint32 hashMode);

/*********************************************************************
* @purpose  Creates a new LAG
*
*
* @param    lagIfNum  @b{(input)} Internal Interface Number of the lag interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot3adCreate(DTL_USP_t *lagUsp);
/*********************************************************************
* @purpose
*
*
* @param    lagIfNum  @b{(input)} Internal Interface Number of the lag interface
* @param    numPort   @b{(input)} Total number of ports to be added
* @param    portIntf  @b{(input)} Internal Interface Number of the port to be added
* @param    hashMode  @b{(input)} Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot3adPortAdd(L7_uint32 lagIfNum,
                                 L7_uint32 numPort,
                                 L7_uint32 portIntf[],
                                 L7_uint32 hashMode);
/*********************************************************************
* @purpose  Enables or disables LACP PDU and Marker Response PDU
*           to be sent to the CPU from an interface
*
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    mode      @b{(input)} L7_ENABLE to enable LACP PDU and Marker
*                                 Response PDU to be SENT TO THE cpu
*                                 L7_DISABLE to drop these PDUs
* @param    portIntf  @b{(input)} Internal Interface Number of the port to be added
* @param    hashMode  @b{(input)} Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot3adPortDelete(L7_uint32 lagIfNum,
                                    L7_uint32 numPort,
                                    L7_uint32 portIntf[],
                                    L7_uint32 hashMode);
/*********************************************************************
* @purpose  Enables or disables LACP PDU and Marker Response PDU
*           to be sent to the CPU from an interface
*
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    mode      @b{(input)} L7_ENABLE to enable LACP PDU and Marker
*                                 Response PDU to be SENT TO THE cpu
*                                 L7_DISABLE to drop these PDUs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot3adLagDelete(L7_uint32 lagIfNum);

/* DTL MultiCast APIs       */



/* DTL MPLS APIs            */



/* DTL Policy APIs          */


/*********************************************************************
* @purpose  Enables or disables flow control for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
*                       or MAX_INTERFACE_COUNT to apply configuration
*                       to all interfaces
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfFlowCtrlModeSet(L7_uint32 intIfNum,
                                             L7_uint32 mode);


/*********************************************************************
* @purpose  Enables or disables flow control for the entire switch i.e. all
*           interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfAllFlowCtrlModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Enables or disables the broadcast rate limit for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
*                       or MAX_INTERFACE_COUNT to apply configuration
*                       to all interfaces
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Broadcast Threshold
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfBcastCtrlModeSet(L7_uint32 intIfNum,
                                              L7_uint32 mode,
                                              L7_uint32 threshold,
                                              L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Enables or disables the multicast rate limit for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
*                       or MAX_INTERFACE_COUNT to apply configuration
*                       to all interfaces
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Multicast Threshold
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfMcastCtrlModeSet(L7_uint32 intIfNum,
                                              L7_uint32 mode,
                                              L7_uint32 threshold,
                                              L7_RATE_UNIT_t rate_unit);


/*********************************************************************
* @purpose  Enables or disables the destination lookup failure rate limit for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
*                       or MAX_INTERFACE_COUNT to apply configuration
*                       to all interfaces
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Unknown Unicast Threshold
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfUcastCtrlModeSet(L7_uint32 intIfNum,
                                              L7_uint32 mode,
                                              L7_uint32 threshold,
                                              L7_RATE_UNIT_t rate_unit);


/*********************************************************************
* @purpose  Enables or disables the broadcast rate limit for the system
*           i.e. all interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Broadcast Threshold
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfAllBcastCtrlModeSet(L7_uint32 mode,
                                                 L7_uint32 threshold,
                                                 L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Enables or disables the multicast rate limit for the system
*           i.e. all interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Multicast Threshold
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfAllMcastCtrlModeSet(L7_uint32 mode,
                                                 L7_uint32 threshold,
                                                 L7_RATE_UNIT_t rate_unit);


/*********************************************************************
* @purpose  Enables or disables the destination lookup failure rate limit for the system
*           i.e. all interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Unknown Unicast Threshold
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyIntfAllUcastCtrlModeSet(L7_uint32 mode,
                                                 L7_uint32 threshold,
                                                 L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Gets the Downstream Threshold discarded packet count
*
* @param    counter @b{(input)} pCounterValue_entry_t structure
*
* @returns  value in counter->cValue
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API void dtlPolicyDomDownstreamThresholdDiscardsGet(pCounterValue_entry_t counter);


/*********************************************************************
* @purpose  Gets the Upstream Ingress Threshold discarded packet count
*
* @param    counter @b{(input)} pCounterValue_entry_t structure
*
* @returns  value in counter->cValue
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API void dtlPolicyIntfUpstreamIngressThresholdDiscardsGet(pCounterValue_entry_t counter);


/*********************************************************************
* @purpose  Gets the Upstream Ingress Policing discarded packet count
*
* @param    counter @b{(input)} pCounterValue_entry_t structure
*
* @returns  value in counter->cValue
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API void dtlPolicyIntfUpstreamIngressPoliceDiscardsGet(pCounterValue_entry_t counter);


/*******************************************************************************
* @purpose   Sets the traffic policing time quantum
*
* @param     msec      @b{(input)} Time in milliseconds
* @param     intIfNum   @b{(input)} Internal Interface number
*
* @returns   L7_SUCCESS if success
* @returns   L7_FAILURE if failure
*
* @notes
*
* @end
**********************************************************************/
EXT_API L7_RC_t dtlPolicyTimeQuantumSet(L7_uint32 msec,
                                        L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Sets traffic policing credit
*
* @param    intIfNum       @b{(input)} Internal Interface Number in application corresponding
*                                      to a unit, slot and port in the device driver
* @param    rate           @b{(input)} New policing credit rate
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPolicyCreditSet(L7_uint32 intIfNum,
                                   L7_uint32 rate);

/* DTL IPv6 APIs */
/*********************************************************************
* @purpose  Enable/Disable forwarding of ipv6 packets between ports or copying
            of ipv6 packets to CPU
*
* @param    srcIfNum       @b{(input)} Internal Interface Number in application
*                                     corresponding to a unit, slot and port in
*                                     the device driver from which ipv6 packets *                                     must be forwarded
* @param    dstIfNum       @b{(input)} Internal Interface Number in application
*                                     corresponding to a unit, slot and port in
*                                     the device driver to which ipv6 packets
*                                     must be forwarded
* @param    mode          @b{(input)} L7_ENABLE or L7_DISABLE
*
* @param    flag          @b{(input)} L7_ENABLE - To enable copying of packets
*                                     to CPU when bridging between ports
*                                     L7_DISABLE - Copying of packets to CPU
*                                     will not be enabled while bridging between
*                                     ports
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpv6ProvConfig(L7_uint32 srcIfNum, L7_uint32 dstIfNum, L7_uint32 mode, L7_uint32 copyToCpu);


/*
/------------------------------------------------------------------\
*                             DATA APIs                            *
\------------------------------------------------------------------/
*/



/*********************************************************************
* @purpose  Transmits the PDU
*
* @param    bufHandle   @b{(input)}  Handle to buffer
* @param    dtlCmd      @b{(input)}  Type of command
* @param    *dtlCmdInfo @b{(input)}  Pointer to DTL transmit command info structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    provide traffic parameterization
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPduTransmit( L7_netBufHandle bufHandle,
                                DTL_CMD_t dtlCmd,
                                DTL_CMD_TX_INFO_t *dtlCmdInfo );


/********************************************************************
* @purpose   Processes received ARP packets
*
* @param     bufHandle @b{(input)} Handle to the buffer
* @param     *pduInfo   @b{(input)} Pointer to pdu info structure
*
* @returns   L7_TRUE
*
* @notes
*
* @end
**********************************************************************/
EXT_API L7_RC_t dtlARPProtoRecv(L7_netBufHandle bufHandle,
                                sysnet_pdu_info_t *pduInfo);


/*******************************************************************************
* @purpose   Processes received IP packets
*
* @param     bufHandle @b{(input)} Handle to the buffer
* @param     *pduInfo   @b{(input)} Pointer to pdu info structure
*
* @returns   L7_TRUE
*
* @notes
*
* @end
**********************************************************************/
EXT_API L7_RC_t dtlIPProtoRecv (L7_netBufHandle bufHandle,
                                sysnet_pdu_info_t *pduInfo);

/*******************************************************************************
* @purpose   Processes received IP packets
*
* @param     bufHandle @b{(input)} Handle to the buffer
* @param     *pduInfo   @b{(input)} Pointer to pdu info structure
*
* @returns   L7_TRUE
*
* @notes     PTIN Added
*
* @end
**********************************************************************/
EXT_API L7_RC_t dtlIPProtoRecvAny(L7_netBufHandle bufHandle, char *data, L7_uint32 nbytes, sysnet_pdu_info_t *pduInfo);


/* DTL MPI APIs */


/* DTL Static MAC Filter APIs*/
/*********************************************************************
* @purpose  Creates a Static MAC Filter
*
* @param    macAddr         @b{(input)} MAC Address of the Filter
* @param    vlanId          @b{(input)} vlan ID associated
* @param    numSrcIntf      @b{(input)} Number of source interfaces in the list
* @param    srcIntfList[]   @b{(input)} Array of source interfaces in the Filter
* @param    numDstList      @b{(input)} Number of destination interfaces in the list
* @param    dstIntfList[]   @b{(input)} Array of destination interfaces in the Filter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if any specified interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine is used to modify the filter as well
* @notes    Ensure that all required interfaces are in the two lists.
* @notes    Do not send just the changes, send all the data as if creating a new filter
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFilterAdd(L7_uchar8* macAddr,
                             L7_uint32  vlanId,
                             L7_uint32  numSrcIntf,
                             L7_uint32  srcIntfList[],
                             L7_uint32  numDstIntf,
                             L7_uint32  dstIntfList[]);

/*********************************************************************
* @purpose  Deletes a given MAC filter
*
* @param    macAddr         @b{(input)} MAC Address of the Filter
* @param    vlanId          @b{(input)} vlan ID associated
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if any specified interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFilterDelete(L7_uchar8* macAddr,
                                L7_uint32  vlanId);


/*
/------------------------------------------------------------------\
*                          UTILITY APIs                            *
\------------------------------------------------------------------/
*/

/*********************************************************************
* @purpose  Generic dtl callback registration
*
* @param    family       @b{(input)} DTL family type
*                           DTL_FAMILY_CONTROL,
*                           DTL_FAMILY_SYSTEM,
*                           DTL_FAMILY_INTF_MGMT,
*                           DTL_FAMILY_LAG_MGMT,
*                           DTL_FAMILY_ADDR_MGMT,
*                           DTL_FAMILY_QVLAN_MGMT,
*                           DTL_FAMILY_GARP_MGMT,
*                           DTL_FAMILY_ROUTING_MGMT,
*                           DTL_FAMILY_ROUTING_ARP_MGMT,
*                           DTL_FAMILY_ROUTING_INTF_MGMT,
*                           DTL_FAMILY_FRAME
* @param    (*funcPtr)   @b{(input)} Pointer to the function to be registered
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    passes this information to the device driver for actual registration
* @notes    function must be of type (*DAPI_CALLBACK_t)
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlCallbackRegistration(DTL_FAMILY_t family,
                                        L7_RC_t (*funcPtr)(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, void *eventInfoPtr));

/*********************************************************************
* @purpose  Generic dtl callback de-registration
*
* @param    family       @b{(input)} DTL family type
*                           DTL_FAMILY_CONTROL,
*                           DTL_FAMILY_SYSTEM,
*                           DTL_FAMILY_INTF_MGMT,
*                           DTL_FAMILY_LAG_MGMT,
*                           DTL_FAMILY_ADDR_MGMT,
*                           DTL_FAMILY_QVLAN_MGMT,
*                           DTL_FAMILY_GARP_MGMT,
*                           DTL_FAMILY_ROUTING_MGMT,
*                           DTL_FAMILY_ROUTING_ARP_MGMT,
*                           DTL_FAMILY_ROUTING_INTF_MGMT,
*                           DTL_FAMILY_FRAME
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    passes this information to the device driver for actual registration
* @notes    function must be of type (*DAPI_CALLBACK_t)
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlCallbackDeRegistration(DTL_FAMILY_t family);

/*********************************************************************
* @purpose  Learns entry callback from received frames puts it in a message queue
* @purpose  and releases the interrupt thread
*
* @param    ddusp          @b{(input)} Device driver reference to unit slot and port
* @param      family         @b{(input)} Device Driver family type
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    event          @b{(input)} Event of type DAPI_EVENT_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlFdbReceiveCallback(DAPI_USP_t *ddusp,
                                            DAPI_FAMILY_t family,
                                            DAPI_CMD_t cmd,
                                            DAPI_EVENT_t event,
                                            void *dapiEventInfo);



/*********************************************************************
* @purpose  PoE notification callback, call trapManager to report
*
* @param    ddusp          @b{(input)} Device driver reference to unit slot and port
* @param    family         @b{(input)} Device Driver family type
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    event          @b{(input)} Event of type DAPI_EVENT_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPoeNotificationCallback(DAPI_USP_t *ddusp,
                                           DAPI_FAMILY_t family,
                                           DAPI_CMD_t cmd,
                                           DAPI_EVENT_t event,
                                           void *dapiEventInfo);



/*********************************************************************
* @purpose  Registers for port event notifications from the Advanced Network Processing Layer
*
* @param    function to be called with the following parameters
*               - usp     @b{(input)} NIM USP structure
*               - event   @b{(input)} Event type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    provide traffic parameterization
*
* @end
*********************************************************************/
EXT_API void dtlRegister(void (*notifyChange)(nimUSP_t *usp, L7_uint32 event,
                                              void * dapiIntmgmt));



/*********************************************************************
* @purpose  Registers functions with DTL which are called depending on
*           the type of event they register for.
*
* @param        dtlFamily @b{(input)} DTL family type
*                                 {DTL_FAMILY_CONTROL = 1,
*                                  DTL_FAMILY_SYSTEM,
*                                  DTL_FAMILY_INTF_MGMT,
*                                  DTL_FAMILY_LAG_MGMT,
*                                  DTL_FAMILY_ADDR_MGMT,
*                                  DTL_FAMILY_QVLAN_MGMT,
*                                  DTL_FAMILY_GARP_MGMT,
*                                  DTL_FAMILY_ROUTING_MGMT,
*                                  DTL_FAMILY_FRAME
*                                  }
* @param    dtlEvent  @b{(input)} Type of event for which the function is registered
*                                 {DTL_EVENT_FRAME_RX_ARP,
*                                  DTL_EVENT_FRAME_RX_IP,
*                                  DTL_EVENT_FRAME_RX_DOT1D,
*                                  DTL_EVENT_FRAME_RX_GARP,
*                                  DTL_EVENT_INTF,
*                                  DTL_EVENT_ADDR,
*                                  DTL_EVENT_QVLAN_MEMBER_QUERY,
*                                  }
* @param    funcPtr   @b{(input)} Pointer to the function that is registered
*
* @returns  L7_SUCCESS on success
* @returns  L7_FAILURE on failure
*
* @notes none
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlGenericRegistration(DTL_FAMILY_t dtlFamily,
                                       DTL_EVENT_t dtlEvent,
                                       L7_FUNCPTR_t funcPtr);

/*********************************************************************
* @purpose  Deregisters functions to receive notifications
*
* @param        dtlFamily @b{(input)} DTL family type
*                                 {DTL_FAMILY_CONTROL = 1,
*                                  DTL_FAMILY_SYSTEM,
*                                  DTL_FAMILY_INTF_MGMT,
*                                  DTL_FAMILY_LAG_MGMT,
*                                  DTL_FAMILY_ADDR_MGMT,
*                                  DTL_FAMILY_QVLAN_MGMT,
*                                  DTL_FAMILY_GARP_MGMT,
*                                  DTL_FAMILY_ROUTING_MGMT,
*                                  DTL_FAMILY_FRAME
*                                  }
* @param    dtlEvent  @b{(input)} Type of event for which the function is deregistered
*                                 {DTL_EVENT_FRAME_RX_ARP,
*                                  DTL_EVENT_FRAME_RX_IP,
*                                  DTL_EVENT_FRAME_RX_DOT1D,
*                                  DTL_EVENT_FRAME_RX_GARP,
*                                  DTL_EVENT_INTF,
*                                  DTL_EVENT_ADDR,
*                                  DTL_EVENT_QVLAN_MEMBER_QUERY,
*                                  }
* @param    funcPtr   @b{(input)} Pointer to the function that is deregistered
*
* @returns  L7_SUCCESS on success
* @returns  L7_FAILURE on failure
*
* @notes none
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlGenericDeregistration(DTL_FAMILY_t dtlFamily,
                                         DTL_EVENT_t dtlEvent,
                                         L7_FUNCPTR_t funcPtr);

/*********************************************************************
* @purpose  Loopbacks a pdu to a specified interface
*
* @param    destIntf        @b{(input)} the interface number the pdu should be received in
* @param    netBufHandle    @b{(input)} Handle to buffer
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
*
* @notes    This is a routine just for testing. The pdu IS NOT sent to the lower
*           layers hence there is no egress from the device. This routine just
*           simulates that a particular pdu has been received on a particular interface
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPduLoopback(L7_uint32 destIntf, L7_netBufHandle bufHandle);

/*********************************************************************
* @purpose  Enables IGMP/MLD Snooping
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlSnoopingEnable(L7_uchar8 family, L7_uint16 vlanId /* PTin added: IGMP snooping */);

/*********************************************************************
* @purpose  Disables IGMP/MLD Snooping
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlSnoopingDisable(L7_uchar8 family, L7_uint16 vlanId /* PTin added: IGMP snooping */);
/*********************************************************************
* @purpose  Notifies L3 about a change in the outport list
*
* @param    L7_uchar8       Multicast Mac Address of received group
* @param    L7_BOOL         L7_TRUE -> Upstream Routing interface intIfNum
*                           L7_FALSE  -> Downstream Routing interface intIfNum
* @param    L7_uint32       Upstream/Downstream Routing interface intIfNum
* @param    L7_INTF_MASK_t  Interface mask of member ports of the
*                           group
* @param    L7_BOOL         snoopVlanOperState -> L7_TRUE if snooping
*                           is operational on atleast one interface of the
*                           vlan rtrportNum.
*                           L7_FALSE otherwise.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlSnoopingL3Notify(L7_uchar8 *mcastMacAddr,
                                    L7_BOOL srcIntfFlag,
                                    L7_uint32 outRtrPortNum,
                                    L7_INTF_MASK_t *outPortListMask,
                                    L7_BOOL snoopVlanOperState);
/*********************************************************************
*
* @purpose  Sets the MSTP state for the port in a particular instance
*           of spanning tree
*
*
* @param    *mstp   @b((input)) pointer to the mstp state structure
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlDot1sStateSet(L7_uint32 instNumber, L7_uint32 intIfNum, L7_uint32 state, L7_uint32 appRef);

/*********************************************************************
*
* @purpose  Creates an instance of spanning tree
*
*
*
* @param    instNumber  @b((input)) user assigned instance number
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlDot1sInstanceCreate(L7_uint32 instNumber);
/*********************************************************************
*
* @purpose  Deletes an instance of spanning tree
*
*
*
* @param    instNumber  @b((input)) user assigned instance number to be deleted
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t dtlDot1sInstanceDelete(L7_uint32 instNumber);

/*********************************************************************
* @purpose  Add VLAN to an already created instance of spanning tree 
*          
* @param    instNumber	@b((input)) User assigned instance number
* @param	vlanId	    @b{(input)} vlan id
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sInstVlanIdAdd(L7_uint32 instNumber, L7_ushort16 vlanId);

/*********************************************************************
* @purpose  Remove VLAN from an existing instance of spanning tree 
*          
* @param    instNumber	@b((input)) User assigned instance number
* @param	vlanId	    @b{(input)} vlan id
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sInstVlanIdRemove(L7_uint32 instNumber, L7_ushort16 vlanId);

///* PTin added: MAC learning */
///*********************************************************************
//* @purpose  Change Learn Mode
//*
//* @param    intIfNum @b((input))internal interface number
//*           learnEnable => Learn mode: L7_ENABLE or L7_DISABLE
//*
//* @returns  L7_SUCCESS on a successful operation
//* @returns  L7_FAILURE for any error
//*
//* @comments
//*
//* @notes
//*
//* @end
//*********************************************************************/
//EXT_API L7_RC_t dtlDot1sLearnModeSet(L7_uint32 intIfNum, L7_BOOL learnEnable);
//
///*********************************************************************
//* @purpose  Get Learn Mode
//*
//* @param    intIfNum @b((input)) internal interface number
//*           learnEnable => Learn mode: L7_ENABLE or L7_DISABLE
//*
//* @returns  L7_SUCCESS on a successful operation
//* @returns  L7_FAILURE on any error
//*
//* @comments
//*
//* @notes
//*
//* @end
//*********************************************************************/
//EXT_API L7_RC_t dtlDot1sLearnModeGet(L7_uint32 intIfNum, L7_BOOL *learnEnable);
///* PTin end */

/*********************************************************************
* @purpose  Flushes all entries in fdb learnt on this interface
*
* @param    intIfNum    @b((input)) internal interface number
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sFlush(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the BPDU filter for a port
*
* @param    intIfNum    @b((input)) internal interface number			   
* @param    bpduFilter	@b((input)) Filter value to be set  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sBpduFilterSet(L7_uint32 intIfNum, L7_BOOL bpduFilter);

/*********************************************************************
* @purpose  Set the BPDU guard mode for a port
*			   
* @param    intIfNum    @b((input)) internal interface number			   
* @param    bpduGuard	@b((input)) Guard mode to be set  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sBpduGuardSet(L7_uint32 intIfNum, L7_BOOL bpduGuard);

/*********************************************************************
* @purpose  Set the BPDU flood for a port
*			   
* @param    intIfNum    @b((input)) internal interface number
* @param    bpduFlood	@b((input)) Filter value to be set  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sBpduFloodSet(L7_uint32 intIfNum, L7_BOOL bpduFlood);

/*********************************************************************
* @purpose  Enable/disable dot1x
*
* @param    mode  @b{(input)} dot1x mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1xModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Set the authorization status of the specified interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    status    @b{(input)} dot1x port status
*                                 (L7_DOT1X_PORT_AUTHORIZED/L7_DOT1X_PORT_UNAUTHORIZED)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1xIntfStatusSet(L7_uint32 intIfNum, L7_DOT1X_PORT_STATUS_t status, L7_BOOL violationCallbackEnabled);


/*********************************************************************
* @purpose  Add an authorized client to an unauthorized port.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of authorized client
* @param    vlanId     @b{(input)}  set to non-zero value to assign this client to a VLAN
* @param    pTLV       @b{(input)}  pass a non-NULL pointer to apply a policy for this client
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1xIntfClientAdd(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId, L7_tlv_t *pTLV);

/*********************************************************************
* @purpose  Remove an authorized client from an unauthorized port.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of authorized client
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1xIntfClientRemove(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr);



/*********************************************************************
* @purpose  Set port authentication mode
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    mode        @b{(input)} port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfAuthModeSet( L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t mode);

/*********************************************************************
* @purpose  Check if a client has timeout i.e. no acitivity from that client 
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of authorized client
* @param    timeout    @b{(output)} Flag indicating if client has timed out. 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientTimeoutGet(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr,L7_BOOL *timeout);

/*********************************************************************
* @purpose  Drop packets coming from the specified MAC/VLAN pair on intIfNum.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of blocked client
* @param    vlanId     @b{(input)}  vlan ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientBlock(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
* @purpose  Allow packets coming from the specified MAC/VLAN pair on intIfNum.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of blocked client
* @param    vlanId     @b{(input)}  vlan ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientUnblock(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
* @purpose  Send netBuf on the specified interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    vlanId    @b{(input)} vlan id
* @param    intIfNum  @b{(input)} net buf handle
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpBufSend(L7_uint32 intIfNum, L7_uint32 vlanId,L7_netBufHandle  bufHandle);

/*********************************************************************
* @purpose  Applies the Double vlan tagging configuration for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *dTag    @b{(input)} Pointer to the Double Vlan Tag stucture
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDvlantagIntfApply(L7_uint32 intIfNum, DVLANTAG_DTL_t *dTag);
EXT_API L7_RC_t dtlDvlantagIntfMultiTpidApply(L7_uint32 intIfNum, DVLANTAG_DTL_t *dTag);
EXT_API L7_RC_t dtlDvlantagApply(DVLANTAG_DTL_GLOBAL_t *dTag);
EXT_API L7_RC_t dtlDvlantagDefaultTpidApply(DVLANTAG_DTL_t *dTag);

/*********************************************************************
* @purpose  Create a ipsubnet based vlan
*
* @param    subnet   @b{(input)} subnet address
* @param    netmask  @b{(input)} netmask address
* @param    vlanid   @b{(input)} vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlVlanIpSubnetCreate(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanid);


/*********************************************************************
* @purpose  Delete a ipsubnet based vlan
*
* @param    subnet   @b{(input)} subnet address
* @param    netmask  @b{(input)} netmask address
* @param    vlanid   @b{(input)} vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlVlanIpSubnetDelete(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanid);

/*********************************************************************
* @purpose  Create a mac based vlan
*
* @param    mac      @b{(input)} Mac address
* @param    vlanid   @b{(input)} vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlVlanMacEntryCreate(L7_enetMacAddr_t mac, L7_uint32 vlanid);


/*********************************************************************
* @purpose  Delete a mac based vlan
*
* @param    mac      @b{(input)} Mac address
* @param    vlanid   @b{(input)} vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlVlanMacEntryDelete(L7_enetMacAddr_t mac, L7_uint32 vlanid);


/*********************************************************************
* @purpose Sets/Resets the interface list for protected port
*
* @param   groupId    @b{(input)} groupId
* @param   portMask   @b{(input)} interface mask
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
************************************************************************/
EXT_API L7_RC_t dtlProtectedPortGroupCreate(L7_uint32 groupId, L7_INTF_MASK_t portMask);


/*************************************************************************
*
* @purpose Delete the protected port
*
* @param   groupId    @b{(input)} Protected port GroupId
* @param   intIfNum   @b{(input)} interface number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Since a port can only be protected in a single group, removing
*        a port from a group means that it can forward to all ports.
*
* @end
************************************************************************/
EXT_API L7_RC_t dtlProtectedPortDelete(L7_uint32 groupId, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Configures DHCP Filtering on an interface
*
* @param    intfNum          @b{(input)} Internal intf number
* @param    trusted          @b{(input)} TRUE if the port is trusted
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlDhcpFilterIntfModeSet( L7_uint32 intfNum, L7_BOOL trusted );

/*********************************************************************
 * @purpose  Configures DHCP Filtering 
 *
 * @param    enabe          @b{(input)} TRUE if the admin mode is enable
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @end
 *********************************************************************/
L7_RC_t dtlDhcpFilterModeSet(L7_uint32 adminMode);

/* Denial of Service */
/*********************************************************************
* @purpose  Sets DoS SIP=DIP mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSSIPDIPSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS SMAC=DMAC mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSSMACDMACSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the DoS First Fragment mode and minTCPHdrSize
*
* @param    mode
* @param    minTCPHdrSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSFirstFragSet(L7_uint32 mode, L7_int32 minTCPHdrSize);

/*********************************************************************
* @purpose  Sets DoS TCP Fragment mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPFragSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Offset mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPOffsetSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Flag mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPFlagSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Flag & Sequence mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPFlagSeqSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPPortSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS UDP Port mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSUDPPortSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP SYN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPSynSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP SYN&FIN mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPSynFinSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS TCP FIN&URG&PSH mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSTCPFinUrgPshSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS L4 Port Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSL4PortSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets DoS ICMP Mode and maxICMPSize
*
* @param    mode
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSICMPSet(L7_uint32 mode, L7_int32 maxICMPSize);

/*********************************************************************
* @purpose  Sets DoS ICMPv6 Mode and maxICMPSize
*
* @param    mode
* @param    maxICMPSize
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSICMPv6Set(L7_uint32 mode, L7_int32 maxICMPSize);

/*********************************************************************
* @purpose  Sets DoS ICMP Fragment Mode
*
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDoSICMPFragSet(L7_uint32 mode);

/* END - Denial of Service */

/* Voice Vlan*/
/*********************************************************************
* @purpose  Add a voice device on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mac_addr @b{(input)} mac address of the voip device
* @param    vlanID   @b{(input)} vlan Id associated with this device
*                                 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanDeviceAdd(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_uint32 vlanId);


/*********************************************************************
* @purpose  Remove a voice device on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mac_addr @b{(input)} mac address of the voip device
* @param    vlanID   @b{(input)} vlan Id associated with this device
*                                 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanDeviceRemove(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_uint32 vlanId);

/*********************************************************************
* @purpose  Set the COS Override mode on the specified interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    flag    @b{(input)}  set or reset
* @param    mac      @b{(input)} Mac address of the voip device
*                                 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanCosOverrideSet(L7_uint32 intIfNum,L7_BOOL flag);

/*********************************************************************
* @purpose  Set the Auth State on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    flag     @b{(input)} Either set or reset
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanAuthSet(L7_uint32 intIfNum, L7_BOOL flag);

/*END - Voice Vlan*/

/*********************************************************************
* @purpose  Get the cable status attached to port
*
* @param    unit        @b{{input}}   Unit id of operation
* @param    interface   @b{(input)}   Internal Interface Number
* @param    *status     @b{(output)}  The cable status attached to port
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlPortCableDiagGet(L7_uint32 unit, L7_uint32 interface, void *status);

/*********************************************************************
* @purpose  Set mode for Ping flooding Dos Attack
*
* @param    interface
* @param    mode
* @param    param
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDosIntfPingFlooding(L7_uint32 interface,L7_uint32 mode,L7_uint32 param);

/*********************************************************************
* @purpose  Set mode for Smurf Dos Attack
*
* @param    interface
* @param    mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDosIntfSmurfAttack(L7_uint32 interface,L7_uint32 mode);

/*********************************************************************
* @purpose  Set mode for SYN ACK Flooding Dos Attack
*
* @param    interface
* @param    mode
* @param    param
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDosIntfSynAckFlooding(L7_uint32 interface,L7_uint32 mode,L7_uint32 param);

/*********************************************************************
* @purpose  Configure DHCP Snooping on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    enabled     @b{(input)} Intf is enabled for DHCP Snooping
* @param    trusted     @b{(input)} Intf is trusted for DHCP Snooping
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDhcpSnoopingConfig(L7_uint32 intIfNum,
                                      L7_BOOL   enabled);

/*********************************************************************
* @purpose  Configure Dynamic ARP Inspection on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    trusted     @b{(input)} Intf is trusted for ARP
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDynamicArpInspectionConfig(L7_uint32 intIfNum,
                                              L7_BOOL   trusted);

/*********************************************************************
* @purpose  Configure IP Source Guard on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    enabled     @b{(input)} Intf is enabled for IP Source Guard
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpsgConfig(L7_uint32 intIfNum,
                              L7_BOOL   enabled);

/*********************************************************************
* @purpose  Get the number of dropped packets due to IPSG on an interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    *droppedPkts @b{(input)} pointer to dropped packets counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpsgDroppedPktsGet(L7_uint32  intIfNum,
                                      L7_uint32 *droppedPkts);

/*********************************************************************
* @purpose  Configure an allowable IP/MAC pair for IP Source Guard on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv4 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpsgIpv4ClientAdd(L7_uint32        intIfNum,
                                     L7_in_addr_t     ipv4Addr,
                                     L7_enetMacAddr_t macAddr);

/*********************************************************************
* @purpose  Configure an allowable IP/MAC pair for IP Source Guard on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv6 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpsgIpv6ClientAdd(L7_uint32        intIfNum,
                                     L7_in6_addr_t    ipv6Addr,
                                     L7_enetMacAddr_t macAddr);

/*********************************************************************
* @purpose  Remove an allowable IP/MAC pair for IP Source Guard from an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv4 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpsgIpv4ClientRemove(L7_uint32        intIfNum,
                                        L7_in_addr_t     ipv4Addr);

/*********************************************************************
* @purpose  Remove an allowable IP/MAC pair for IP Source Guard from an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipv4Addr    @b{(input)} IPv6 address of allowable client
* @param    macAddr     @b{(input)} Optional MAC address of allowable client
*                                   Set to all zero's if the MAC is not used.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpsgIpv6ClientRemove(L7_uint32        intIfNum,
                                        L7_in6_addr_t    ipv6Addr);
/*********************************************************************
* @purpose  Sets the negotiation capabilities of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    negoCapabilities
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfNegotiationCapabilitiesSet(L7_uint32 intIfNum, L7_uint32 negoCapabilities);
#if L7_FEAT_DUAL_PHY_COMBO
/*******************************************************************************
* @purpose  Get auto-negotiation and the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    negoCapabilities
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************************/
EXT_API L7_RC_t dtlIntfNegotiationCapabilitiesGet(L7_uint32 intIfNum, L7_uint32 *negoCapabilities);
#endif
/*********************************************************************
* @purpose  Test the fiber cable and retrieve test results.
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    *cableType       @b((output)) copper or fiber.
* @param    *temperature     @b{(output)} degrees C
* @param    *voltage         @b{(output)} milliVolts
* @param    *current         @b{(output)} milliAmps
* @param    *powerOut        @b{(output)} microWatts
* @param    *powerIn         @b{(output)} microWatts
* @param    *txFault         @b((output)) true or false
* @param    *LOS             @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The function may take a couple of seconds to execute.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfCableFiberDiagGet( L7_uint32              intIfNum,
                                          L7_PORT_CABLE_MEDIUM_t *cableType,
                                          L7_int32               *temperature,
                                          L7_uint32              *voltage,
                                          L7_uint32              *current,
                                          L7_uint32              *powerOut,
                                          L7_uint32              *powerIn,
                                          L7_BOOL                *txFault,
                                          L7_BOOL                *los );

/*********************************************************************
* @purpose  Flushes all learned entries in the FDB
*
* @param    none
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1sFlushAll(void);



/*dot1ad */
/*********************************************************************
* @purpose  Set the Dot1ad interface type
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1adInterfaceTypeSet(L7_uint32 intIfNum,
                      DOT1AD_INTFERFACE_TYPE_t  intfType);


/*************************************************************************
* @purpose Set the Dot1ad tunneling action 
*
* @param    unconfig          @b((input)) unconfigure tunnel action.
* @param    vlanId            @b((input)) valn Id.
* @param    mac                 @b((input)) L2 protocol reserved MAC address.
* @param    protocolId         @b((input)) L2 protocol identifier in MAC header.
* @param    tunnelAction      @b((input)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
*************************************************************************/
L7_RC_t dtlDot1adTunnelActionSet(L7_BOOL     unconfig,
                                 L7_ushort16 vlanId,
                               L7_uchar8   *protocolMAC,
                               L7_uint32   protocolId,
                                 DOT1AD_TUNNEL_ACTION_t  tunnelAction);


/*********************************************************************
* @purpose  show the Dot1ad debug interface stats.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
8 @param   stats          @b{(output) pointer to stats.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1adTunnelIntfStatsShow(L7_uint32 intIfNum,
                 dot1adDebugIntfStats_t *stats);

/*********************************************************************
* @purpose  initialize the dot1ad interface config.
*
* @param    intIfNum                 @b{(input)} Internal Interface Number
*.@param    dot1adIntfCfg          @b{(input)} Interface Configuration.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1adApplyIntfConfigData(L7_uint32 intIfNum,
                  dot1adIntfCfg_t *dot1adIntfCfg);

/*********************************************************************
* @purpose  Create a new dot1ad instance
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1adCreate(L7_uint32 intIfNum, L7_uchar8 *pTLV, 
                                     L7_uint32 tlvTotalSize);

/*********************************************************************
* @purpose  Enable VLAN translations based on UNI settings 
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1adSetVlanXlate(L7_uint32 intIfNum, L7_BOOL enable);

/*********************************************************************
* @purpose  Delete an dot1ad instance
*
* @param    intIfNum       @b{(input)} Interface number
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlDot1adDelete(L7_uint32 intIfNum, 
                        L7_ushort16 svid,
                        L7_ushort16 cvid,
                        L7_ushort16 isolatedvid,
                        dot1adTlvNniInterfaceList_t    *nniInterfaceList);


/*********************************************************************
* @purpose  Enables /disables blinking the ports.
*
* @param    enable       @b{(input)} L7_TRUE - for enabling
*                                    L7_FALSE - for disabling      
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This API to blink all port when an error 
*           situation arises.
*
* @end
*********************************************************************/
L7_RC_t dtlLedBlinkSet(L7_BOOL enable);

/*********************************************************************
* @purpose  Enable/disable Captive Portal on an Interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} L7_TRUE enable, L7_FALSE disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlCaptivePortalConfig(L7_uint32 intIfNum, L7_BOOL mode);

/*********************************************************************
* @purpose  Block/Unblock an Interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)}  L7_TRUE block, L7_FALSE unblock
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlCaptivePortalBlock(L7_uint32 intIfNum, L7_BOOL mode);

/*********************************************************************
* @purpose  Enables /disables ISDP the ports.
*
* @param    enable       @b{(input)} L7_TRUE - for enabling
*                                    L7_FALSE - for disabling      
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIsdpPortStatusSet(L7_uint32 intIfNum, L7_BOOL enable);


/*********************************************************************
* @purpose  Set mode for llpf block 
*
* @param    interface        @b{(input)} interface type
* @param    blockType        @b{(input)} LLPF block type
* @param    mode             @b{(input)} mode
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlLlpfIntfBlockConfig(L7_uint32 interface,
                           L7_LLPF_BLOCK_TYPE_t blockType,L7_BOOL mode );

#endif   /* INCLUDE_DTL_API_H */
