/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename hpc.h
*
* @purpose HPC component's internal data types and functions
*
* @component hpc
*
* @comments none
*
* @create 02/16/2003
*
* @author jeffr
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef INCLUDE_HPC
#define INCLUDE_HPC

#include "sysapi_hpc_chassis.h"
#include "hpc_sfp_diagnostics.h"

#define HPC_CONFIG_VER_1       0x01
#define HPC_CONFIG_VER_CURRENT HPC_CONFIG_VER_1

/*******************************************************************************
** hpc data structures
*******************************************************************************/


typedef struct
{
  L7_uint32   registrarID;
  void        (*notify)(L7_uchar8* buffer, L7_uint32 msg_len);
  L7_uint32   tx;
  L7_uint32   rx;
} hpcMessageReceiveNotifyList_t;

typedef struct
{
  void        (*notify)(hpcStackEventMsg_t eventMsg);
} hpcStackEventNotifyList_t;

typedef struct
{
  L7_BOOL     inuse;
  L7_uint32   slot;
  L7_uint32   port;
  L7_BOOL     enabled;
} SYSAPI_STACK_PORT_t;

typedef struct
{
  L7_uint32               version;
  L7_uint32               local_unit_number;
  L7_uint32               admin_pref;
  SYSAPI_STACK_ROLE_t     role;
  SYSAPI_STACK_PORT_t     stack_ports[L7_MAX_STACK_PORTS_PER_UNIT];
  L7_uint32               crc;
} hpcCfgData_t;

typedef struct
{
  slotPower_t lmSlotPower[L7_MAX_LINE_MODULES_PER_UNIT];
} lmSlotPowerCfgData_t;

typedef struct
{
   L7_BOOL                 bcast_transport_is_ready;
   L7_uchar8               asfCurrentMode;
   L7_uchar8               asfConfiguredMode;
} hpcOperationalData_t;

/**************************************************************************
* @purpose  Return the number of supported units.
*
* @param    none
*
* @returns  Number of supported units.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcSupportedUnitsNumGet(void);

/**************************************************************************
* @purpose  Return the number of supported cards.
*
* @param    none
*
* @returns  Number of supported units.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcSupportedCardsNumGet(void);

/**************************************************************************
*
* @purpose  Perform any needed runtime system unit and card descriptor database
*           initialization.
*
* @param    none
*
* @returns  L7_SUCCESS  operation completed with no problems
* @returns  L7_FAILURE  an error occurred while running the DAPI
*                       data initializer functions
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcDescriptorDbInit(void);

/**************************************************************************
*
* @purpose  Initialize the HPC system service.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems in initialization
* @returns  L7_ERROR    encountered error in initialization
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcInit(void);

/**************************************************************************
*
* @purpose  Gets the MAC address allocation policy for the platform.  The
*           policy returned is one of the ENUM SYSAPI_MAC_POLICY_t.
*
* @param    none
*
* @returns  SYSAPI_MAC_POLICY_t
*
* @notes
*
* @end
*
*************************************************************************/
SYSAPI_MAC_POLICY_t hpcMacAllocationPolicyGet(void);

/**************************************************************************
*
* @purpose  Gets the MAC address for a given (slot, port) on the local
*           unit.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems getting MAC address
* @returns  L7_ERROR    (slot, port) data invalid, or other error in MAC
*                       address retrieval
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port, L7_uchar8 *mac_addr);

/**************************************************************************
*
* @purpose  Retrieves a card descriptor from the database.
*
* @param    cardTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes
*
* @end
*
*************************************************************************/
HPC_CARD_DESCRIPTOR_t *hpcCardDbLookup(L7_uint32 cardTypeId);

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid cardTypeIdIndex
*           value.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardDbEntryIndexCheck(L7_uint32 cardTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns the next valid cardTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
* @returns  cardTypeIdIndex value of next index if found
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardDbEntryIndexNextGet(L7_uint32 *cardTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns card type identifier that corresponds to
*           the index provided.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *cardTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardDbCardIdGet(L7_uint32 cardTypeIdIndex, L7_uint32 *cardTypeId);

/**************************************************************************
*
* @purpose  Retrieves a unit descriptor from the database.
*
* @param    unitTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes
*
* @end
*
*************************************************************************/
HPC_UNIT_DESCRIPTOR_t *hpcUnitDbLookup(L7_uint32 unitTypeId);

/**************************************************************************
*
* @purpose  Returns the next valid unitTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
* @returns  unitTypeIdIndex value of next index if found
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcUnitDbEntryIndexNextGet(L7_uint32 *unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Register a callback function to be invoked when a card event
*           occurs.
*
* @param    notify       pointer to function
* @param    registrarID  one of enum L7_COMPONENT_IDS_t identifying the component
*                        to be called back
*
* @returns  L7_SUCCESS   if registrarID is valid
* @returns  L7_FAILURE   if registrarID is invalid
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcCardEventCallbackRegister(void (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId, L7_uint32 event),
                                     L7_uint32 registrarID);

/**************************************************************************
*
* @purpose  Reports the current state of interunit message transport.
*
* @param    none
*
* @returns  L7_TRUE  transport is ready to send messages
* @returns  L7_FALSE transport is not ready to send messages
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL hpcBcastTransportIsReady(void);

/**************************************************************************
*
* @purpose  Saves the unit number assigned to the local unit by unit manager.
*
* @param    lookup_type  one of enum HPC_LKUP_REQ_t indicating what type of lookup
*                        the caller wants
* @param    cardTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes    This function is restricted to be called ONLY by sysapiHpcUnitNumberSet().
*
* @end
*
*************************************************************************/
L7_RC_t hpcUnitNumberSet(L7_uint32 unit);

/**************************************************************************
*
* @purpose  Gets the unit number assigned to the local unit by unit manager.
*
* @param    none
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes    This function is restricted to be called ONLY by sysapiHpcUnitNumberGet().
*
* @end
*
*************************************************************************/
L7_uint32 hpcUnitNumberGet(void);

/******************************************************************************
* @purpose  To turn on/off power to a line module slot
*
* @param    slotNum - @b{(input)}  Slot Number
*           status  - @b{(input)}  Power status (On/Off) of the LM slot
*           source  - @b((input))  Initiator of this request
*
* @returns  L7_SUCCESS,   if the setting of power status of a slot
*                         is successful
*           L7_FAILURE,   Otherwise
*
* @notes    This function is restricted to be called ONLY by
*           sysapiHpcSlotPowerStatusSet().
*
* @end
******************************************************************************/
L7_RC_t hpcSlotPowerStatusSet(L7_uint32 phySlot,
                              POWER_STATUS_t status, POWER_CONTROL_t source);

/******************************************************************************
* @purpose  To get the power status of a line module slot
*
* @param    slotNum - @b{(input)}  Slot Number
*           pStatus - @b{(output)} Power status (On/Off) of the LM slot
*           pSource - @b((output)) To store the source responsible for
*                                  current power status of a slot
*
* @returns  L7_SUCCESS,   if power status is retrived successfully
*           L7_FAILURE,   Otherwise
*
* @notes    This function is restricted to be called ONLY by
*           sysapiHpcSlotPowerStatusGet().
*
* @end
******************************************************************************/
L7_uint32 hpcSlotPowerStatusGet(L7_uint32 phySlot,
                                POWER_STATUS_t *pStatus, POWER_CONTROL_t *pSource);

L7_RC_t hpcAdminPrefSet(L7_uint32 admin_pref);
L7_uint32 hpcAdminPrefGet(void);
/*********************************************************************
* @purpose  Given a unit, gets the unit's system identifier key
*
* @param    unit_number unit number
* @param    *key   pointer to unit key storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_HPC_UNIT_ID_KEY_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t hpcUnitIdentifierKeyGet(L7_uint32 unit, L7_enetMacAddr_t *key);

/**************************************************************************
*
* @purpose  Informs HPC of the local unit's role in the stack.
*           HPC should take any steps necessary based on this fact.
*
* @param    role  one of enum SYSAPI_STACK_ROLE_t indicating this unit's role
                  in the stack
*
* @returns  L7_SUCCESS  all necessary processing was successful
* @returns  L7_ERROR    some problem encountered
*
* @notes    This function is restricted to be called ONLY by sysapiHpcTopOfStackSet().
*
* @end
*
*************************************************************************/
L7_RC_t hpcTopOfStackSet(SYSAPI_STACK_ROLE_t role);

/**************************************************************************
*
* @purpose  Retrieves local unit's stacking role.
*
* @returns    One of enum SYSAPI_STACK_ROLE_t indicating this unit's role
              in the stack
*
*
* @notes    This function is restricted to be called ONLY by sysapiHpcTopOfStackGet().
*
* @end
*
*************************************************************************/
SYSAPI_STACK_ROLE_t hpcTopOfStackGet(void);

/*********************************************************************
* @purpose  Gets whether network port is used for stacking.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - OUT: boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if specified port is not found in the database.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcStackPortGet(L7_uint32 slot, L7_uint32 port, L7_BOOL *stacking_enable);

/*********************************************************************
* @purpose  Enables or disables a network port for use in stacking.
*           This information is saved in the HPC component's persistent
*           storage configuration so it is available after reboots.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if problem occurs
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcStackPortSet(L7_uint32 slot, L7_uint32 port, L7_BOOL stacking_enable);

/*********************************************************************
* @purpose  Queries the platform component as to the largest message
*           size in bytes that the transport can handle.
*
* @param    none
*
* @returns  the max size of message payload in bytes
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 hpcTransportMaxMessageLengthGet(void);

/*********************************************************************
* @purpose  Register a routine to be called when an intrastack message
*           is received for the registrar.
*
* @param    *notify      Notification routine with the following parm
*                        @param    *buffer             pointer to buffer containing message
*                        @param    msg_len             length in bytes of message
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcReceiveCallbackRegister(void (*notify)(L7_uchar8* buffer, L7_uint32 msg_len),
                                   L7_uint32 registrarID);

/**************************************************************************
*
* @purpose  Function registered with HPC to get stack event callbacks.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    event        Stack event to register
* @param    *notify      Notification routine with the hpcStackEventMsg_t parm
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID or invalid eventId
*
* @end
*
*************************************************************************/
L7_RC_t hpcStackEventRegisterCallback(L7_uint32 registrarId,
                                      HPC_EVENT_t event,
                                      void (*notify)(hpcStackEventMsg_t eventMsg));

/*********************************************************************
* @purpose  Send a message to all other units in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in any of the
*           remote systems, the message is silently discarded there and
*           no notification is sent to the sending caller.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in buffer including the HPC header
* @param    buffer  pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcBroadcastMessageSend(L7_uint32 registrarID, L7_uint32 msg_length, L7_uchar8* buffer);

/*********************************************************************
* @purpose  Send a message to a specific unit in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in payload buffer
* @param    buffer  pointer to the payload to be sent
*
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcMessageSend(L7_uint32 registrarID, L7_uint32 unit, L7_uint32 msg_length, L7_uchar8* buffer);

/*********************************************************************
* @purpose  Resets the Driver to a know state
*
* @param    void
*
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverReset(void);

/*********************************************************************
* @purpose  Determine whether the driver is responsible for resyncing new devices
*
* @param    void
*
* @returns  L7_TRUE   if the driver is responsible
* @returns  L7_FALSE  if the driver is not responsible
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL hpcDriverSyncSupported(void);

/*********************************************************************
* @purpose  Inform the driver that this unit is the standby of the stack
*
* @param    stbyStatus L7_TRUE: This unit is the standby of the stack
* @param               L7_FALSE: This unit is not the standby of the stack
*
*                                       
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcDriverNotifyStandbyStatus(L7_BOOL stbyStatus);

/*********************************************************************
* @purpose  On Standby, UM informs the driver that a new stack topology 
*           is available at the lower layer. Driver analyzes the new
*           stack topology to check if the current manager of the stack 
*           has failed. If so then the standby of the stack takes over 
*           as the manager.
*           
*
* @returns  L7_BOOL *managerFailed L7_TRUE: Manager has failed
*                                  L7_FALSE: Manager is present
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcDriverProcessStackTopoChange(L7_BOOL *managerFailed);

/*********************************************************************
* @purpose  On Standby, UM receives stack port link down notifications 
*           from immediate neighbours of the Manager. UM in-turn passes
*           the information to the driver which analyzes the current
*           stack topology to check if the current manager of the stack 
*           has failed. If so then the standby of the stack takes over 
*           as the manager.
*
* @param    reportingUnit: Unit number of Reporting Manager neighbour
* @param    reportingUnitKey: Pointer to CPU key of the Manager neighbour
* @param    stkPortHpcIndex: Hpc index of the stack port that is down
* @param    nhopCpuKey: Pointer to CPU key of the reporting unit neighbour 
*                       (should be the current manager)
*           
*
* @returns  L7_BOOL *managerFailed L7_TRUE: Manager has failed
*                                  L7_FALSE: Manager is present
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcDriverProcessManagerLinkDown(L7_uint32 reportingUnit, L7_enetMacAddr_t *reportingUnitKey, 
                                       L7_uint32 stkPortHpcIndex, L7_enetMacAddr_t *nhopCpuKey, 
                                       L7_BOOL *managerFailed);
/*********************************************************************
* @purpose  UM on each unit informs the driver that the current manager 
*           of the stack is no longer present. Driver tries to remove 
*           the manager key from ATP/Next-hop transport layers so that 
*           pending/future communication with the failed manager is
*           prevented.
*
* @param    managerKey: CPU key of the Manager 
*           
*
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcDriverRemoveStackManager(L7_enetMacAddr_t managerKey);


/*********************************************************************
* @purpose  Instruct the driver to shutdown all the front-panel ports 
*           for a unit.
*
* @param    unitNumber {(input)} Unit for which the ports are to be shut.
*                                L7_ALL_UNITS indicates all the valid
*                                stack members.
*
*
* @returns  L7_RC_t
*
* @comments Called by UM before move-management
*
* @end
*********************************************************************/
L7_RC_t hpcDriverShutdownPorts(L7_uint32 unitNumber);

/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode.
*
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverAsfModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode.
*
* @param    currMode    @b{(input)} Current mode L7_ENABLE/L7_DISABLE
* @param    configMode  @b{(input)} Configured mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcDriverAsfModeGet(L7_uint32 *currMode, L7_uint32 *configMode);

#ifdef L7_POE_PACKAGE
/*********************************************************************
* @purpose   To handle all poe Controller messages
*
* @param   SYSAPI_POE_MSG_t *msg  pointer to PoE message
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hpcPoeControllerMsg(SYSAPI_POE_MSG_t *const msg);
#endif  /* L7_POE_PACKAGE */

/* Box Services */
#if L7_FEAT_TEMPERATURE
/* The following functions must be defined in the platform if this feature is supported. */
extern L7_uint32 hpcLocalTempSensorCountGet(void);
extern L7_RC_t   hpcLocalTempSensorDataGet(L7_uint32 id, HPC_TEMP_SENSOR_DATA_t* data);
#define HPC_LOCAL_TEMP_SENSOR_COUNT_GET()        hpcLocalTempSensorCountGet()
#define HPC_LOCAL_TEMP_SENSOR_DATA_GET(id, data) hpcLocalTempSensorDataGet(id, data)
#else
#define HPC_LOCAL_TEMP_SENSOR_COUNT_GET()        0
#define HPC_LOCAL_TEMP_SENSOR_DATA_GET(id, data) L7_NOT_SUPPORTED
#endif

#if L7_FEAT_PWR_SUPPLY_STATUS
/* The following function must be defined in the platform if this feature is supported. */
extern L7_RC_t hpcLocalPowerSupplyDataGet(L7_uint32 id, HPC_POWER_SUPPLY_DATA_t* data);
#define HPC_LOCAL_POWER_SUPPLY_DATA_GET(id, data) hpcLocalPowerSupplyDataGet(id, data)
#else
#define HPC_LOCAL_POWER_SUPPLY_DATA_GET(id, data) L7_NOT_SUPPORTED
#endif

#if L7_FEAT_FAN_STATUS
/* The following function must be defined in the platform if this feature is supported. */
extern L7_RC_t hpcLocalFanDataGet(L7_uint32 id, HPC_FAN_DATA_t* data);
#define HPC_LOCAL_FAN_DATA_GET(id, data)  hpcLocalFanDataGet(id, data)
#else
#define HPC_LOCAL_FAN_DATA_GET(id, data)  L7_NOT_SUPPORTED
#endif

#if L7_FEAT_SFP
/* The following function must be defined in the platform if this feature is supported. */
extern L7_uint32 hpcLocalSfpCountGet(void);
extern L7_RC_t hpcLocalSfpDataGet(L7_uint32 id, HPC_SFP_DATA_t *data);
#define HPC_LOCAL_SFP_COUNT_GET()        hpcLocalSfpCountGet()
#define HPC_LOCAL_SFP_DATA_GET(id, data) hpcLocalSfpDataGet(id, data)
#else
#define HPC_LOCAL_SFP_COUNT_GET()        0
#define HPC_LOCAL_SFP_DATA_GET(id, data) L7_NOT_SUPPORTED
#endif

#if L7_FEAT_PLUGIN_XFP_MODULE
/* The following function must be defined in the platform if this feature is supported. */
extern L7_uint32 hpcLocalXfpCountGet(void);
extern L7_RC_t hpcLocalXfpDataGet(L7_uint32 id, HPC_XFP_DATA_t *data);
#define HPC_LOCAL_XFP_COUNT_GET()        hpcLocalXfpCountGet()
#define HPC_LOCAL_XFP_DATA_GET(id, data) hpcLocalXfpDataGet(id, data)
#else
#define HPC_LOCAL_XFP_COUNT_GET()        0
#define HPC_LOCAL_XFP_DATA_GET(id, data) L7_NOT_SUPPORTED
#endif

#endif  /* INCLUDE_HPC */
