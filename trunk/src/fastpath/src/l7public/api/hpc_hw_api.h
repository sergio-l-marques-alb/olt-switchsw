/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename hpc_hw_api.h
*
* @purpose Prototypes HPC component's platform-specific function APIs
*
* @component hpc
*
* @comments none
*
* @create 01/05/2004
*
* @author jeffr
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef INCLUDE_HPC_HW_API
#define INCLUDE_HPC_HW_API

#include "l7_common.h"
#include "sysapi_hpc.h"
#include "ms_api.h"

/* RPC Definitions.
*/

/* Macro for generating RPC function IDs.
*/
#define HPC_RPC_FUNCTION_ID(component, x) (((component) * 1000) + (x))

typedef enum {
  HPC_HW_RPC_OK = 0,  /* RPC successfully initiated and completed */
  HPC_HW_RPC_NO_UNIT, /* Unit is not in the stack or has code/config mismatch */
  HPC_HW_RPC_TIMEOUT, /* RPC was attempted to this unit, but response timed out */
  HPC_HW_RPC_TX_ERROR, /* Couldn't send RPC to the target unit. */
  HPC_HW_RPC_NO_SERVER, /* Specified server function is not registered. */
  HPC_HW_RPC_RX_ERROR /* Some problem on the server side. */
} hpcHwRpcStatus_t;


/* RPC Data struct */
typedef struct
{
  L7_uchar8 *buf;         /* Buffer for RPC data */
  L7_uint32  buf_size;    /* Size of Buffer - max data that can be copied */
  L7_uint32  data_len;    /* Actual length of RPC data */
} hpcHwRpcData_t;

/* RPC Callback function type. */
typedef L7_RC_t (*hpc_rpc_callback_t) (L7_uint32 transaction_id,
                                       hpcHwRpcData_t  *rpc_data,
                                       L7_int32   *status,
                                       hpcHwRpcData_t  *rpc_resp);

/* ATP Client ID's used by FastPath */
typedef enum {
 LVL7_NH_TRANSPORT_PORT_NUMBER  = 777, /* ID for next-hop transport by HPC.  */
 LVL7_RELIABLE_TRANSPORT_PORT_NUMBER,  /* ID for reliable transport by HPC.  */
 LVL7_RPC_TRANSPORT_PORT_NUMBER,       /* ID for the RPC reliable transport. */
 LVL7_MSG_SVC_RELIABLE_PORT_NUMBER,    /* ID for msg svc reliable transport  */
 LVL7_MSG_SVC_UNRELIABLE_PORT_NUMBER,  /* ID for msg svc unreliable transport*/
 LVL7_MSG_SVC_NH_PORT_NUMBER,
} fastpath_client_id_t;

/**************************************************************************
*
* @purpose  Reports the local unit's MAC address for use in identifying
*           this unit across the distributed system.
*
* @param    mac   pointer to storage inwhich to store the mac address
*
* @returns  L7_SUCCESS  address retrieved with no problems
* @returns  L7_ERROR  problem encountered in platform specific retrieval function
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareLocalUnitIdentifierMacGet(L7_enetMacAddr_t *mac);

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
SYSAPI_MAC_POLICY_t hpcHardwareMacAllocationPolicyGet(void);

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
L7_RC_t hpcHardwareIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port,
                               L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr,
                               HPC_UNIT_DESCRIPTOR_t *local_unit_desc);
/**************************************************************************
*
* @purpose  Initialize the hardware-specific HPC system service.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems in initialization
* @returns  L7_FAILURE  encountered error in initialization
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareInit(void (*stack_event_callback_func)(hpcStackEventMsg_t event), 
                        void (*msg_recv_notify)(L7_enetMacAddr_t src_key,
                                                L7_uint32 receive_id, 
                                                L7_uchar8* buffer, 
                                                L7_uint32 msg_len));
   
/*********************************************************************
* @purpose  Init the ASF mode.
*          
* @param    mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareAsfInit(L7_uint32 mode);

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
L7_RC_t hpcHardwareAsfModeSet(L7_uint32 mode);

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
L7_RC_t hpcHardwareCardEventCallbackRegister(void (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId, L7_uint32 event),
                                           L7_COMPONENT_IDS_t registrarID);

/**************************************************************************
*
* @purpose  Returns a boolean indicating whether the platform requires
*           HPC to use saved configuration data.
*
* @param    none
*
* @returns  L7_TRUE     saved configuration required
* @returns  L7_FALSE    saved configuration not required
*
* @notes 
*
* @end
*
*************************************************************************/
L7_BOOL hpcHardwareCfgDataRequired(void);

/*********************************************************************
* @purpose  Saves HPC's configuration data.
*
* @param    data        pointer to configuration data to be saved
* @param    num_bytes   size of data in bytes
*
* @returns  L7_SUCCESS  no problems in save
* @returns  L7_ERROR    encountered error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareCfgDataSave(void *data, L7_uint32 num_bytes);

/**************************************************************************
*
* @purpose  Function used to retrieve HPC's saved configuration 
*           data.
*
* @param    data        pointer to configuration data to be retrieve
* @param    num_bytes   size of data in bytes
*
* @returns  L7_SUCCESS  no problems configuration retrieval
* @returns  L7_ERROR    encountered error 
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareCfgDataRetrieve(void *data, L7_uint32 num_bytes);

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
L7_RC_t hpcHardwareUnitIdentifierKeyGet(L7_uint32 unit, L7_enetMacAddr_t *key);

/*********************************************************************
* @purpose  Saves local unit's assigned unit number.
*
* @param    unit        the unit number assigned to the local unit
*
* @returns  L7_SUCCESS  always, no hardware specific steps required
*                       for this technology
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareUnitNumberSet(L7_uint32 unit);

/*********************************************************************
* @purpose  Saves management preference
*
* @param    admin_pref  preference
*
* @returns  L7_SUCCESS  always, no hardware specific steps required
*                       for this technology
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareAdminPrefSet(L7_uint32 admin_pref);

/*********************************************************************
* @purpose  Sets the local unit's current role in the stack.  If setting
*           represents a change in role, reconfigures the vendor driver
*           stacking code to comply with caller's intended role.
*
*           If the role being set is SYSAPI_STACK_ROLE_MANAGEMENT_UNIT
*           and the vendor driver stacking code is not currently in 
*           that configuration, this function returns L7_ERROR and
*           takes steps to reconfigure the vendor stack code. The
*           caller should retry the set after a delay.  (Unit manager
*           state machine handles this.)
*
* @param    role        local unit's assigned role in the stack
*
* @returns  L7_SUCCESS  no problems in save, vendor stacking code state
*                       matches caller's intended role
* @returns  L7_ERROR    encountered error or caller requested 
*                       SYSAPI_STACK_ROLE_MANAGEMENT_UNIT and vendor
*                       driver needs to be reconfigured; retry after delay
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareTopOfStackSet(SYSAPI_STACK_ROLE_t role);

/*********************************************************************
* @purpose  Designates a port to be used as a stacking port.
*
* @param    slot             slot number of port to be used for stacking interface
* @param    port             port number of port to be used for stacking interface
* @param    stacking_enabled L7_TRUE indicates the referenced port should be a 
*                            stacking interface, L7_FALSE indicates it should not
*
* @returns  L7_SUCCESS  no problems in save
* @returns  L7_ERROR    encountered error
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareStackPortSet(L7_uint32 slot, L7_uint32 port, L7_BOOL stacking_enable);

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
L7_uint32 hpcHardwareTransportMaxMessageLengthGet(void);

/*********************************************************************
* @purpose  Queries the platform component as to the number of buffers
*           avaiable to the transport.
*
* @param    none
*                                       
* @returns  the max size of message payload in bytes
*
* @comments    
*       
* @end
*********************************************************************/
L7_uint32 hpcHardwareTransportNumberBuffersGet(void);

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
* @param    msg_length  number of bytes in buffer 
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
L7_RC_t hpcHardwareBroadcastMessageSend(L7_COMPONENT_IDS_t registrarID,
                                        L7_uint32          msg_length,
                                        L7_uchar8         *buffer);

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
* @param    registrarID routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    unit        destination unit
* @param    msg_length  number of bytes in payload buffer
* @param    buffer      pointer to the payload to be sent
* @param    hdr_length  number of bytes in optional payload header
* @param    header      pointer to the optional payload header
* @param    atpClientId ATP port number to use for message transmission
*                                       
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareMessageSend( L7_COMPONENT_IDS_t registrarID, L7_uint32 unit,
                                L7_uint32 msg_length, L7_uchar8* buffer,
                                L7_uint32 hdr_length, L7_uchar8* header,
                                fastpath_client_id_t atpClientId);

/*********************************************************************
* @purpose  Resets the Driver to a known state
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
L7_RC_t hpcHardwareDriverReset(void);

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
L7_BOOL hpcHardwareDriverSyncSupported(void);

/*********************************************************************
* @purpose  Initialize RPC transport.
* 
* @comments
*
* @end
*********************************************************************/
void hpcBroadRpcInit (void);

/*********************************************************************
* @purpose    Maximum size of the data portion of the RPC message.
*           
* @comments
*
* @end
*********************************************************************/
L7_uint32 hpcHardwareRpcMaxMessageLengthGet (void);

/*********************************************************************
* @purpose  Register an RPC service function.
* 
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareRpcRegister (L7_uint32 rpc_transaction_id,
                                hpc_rpc_callback_t  rpc_callback);

/******************************************************************************
* @purpose    Execute the RPC transaction on a given unit or on all 
*             eligible units. Eligible units are units that are found 
*             in the stack and that don't have code or configuration 
*             mismatch.
*
*  target_unit_number - Indicates the unit number on which the RPC transaction
*                       is to be executed. If the target_unit_number is 
*                       L7_ALL_UNITS then the RPC transaction is executed 
*                       on all eligible units.
*
*  rpc_transaction_id - Unique identifier for the RPC function.
*                       On the server side some code must register
*                       a handler for this transaction_id.
*
*  rpc_data           - This data is passed from the client to the server.
*
*  rpc_status         - Pointer to an array where RPC puts its status. 
*                       If the target_unit_number is L7_ALL_UNITS, then the array 
*                       is indexed by unit number and the caller must 
*                       allocate at least L7_MAX_UNITS_PER_STACK + 1) element 
*                       in the array.
*
*  app_status        -  A 4-byte status returned by the remote application. 
*                       If the target_unit_number is L7_ALL_UNITS, then the caller 
*                       must allocate (L7_MAX_UNITS_PER_STACK + 1) elements 
*                       in the array.
*                       The caller may set this parameter to NULL. In that case
*                       no information is returned from the remote server, which
*                       improves RPC performce 2x.
*
*  app_resp         -   If the RPC transaction is expected to receive data from
*                       from server, this points to a response buffer to store
*                       received data.If the target_unit_number is L7_ALL_UNITS, then
*                       the caller must allocate (L7_MAX_UNITS_PER_STACK + 1)
*                       elements to receive data. If no data is expected in response
*                       then this pointer must be set to L7_NULL.
*
* @comments
*
* @end
***************************************************************************************/
L7_RC_t hpcHardwareRpc  (L7_uint32          target_unit_number,
                         L7_uint32          rpc_transaction_id,
                         hpcHwRpcData_t    *rpc_data,
                         hpcHwRpcStatus_t  *rpc_status,
                         L7_int32          *app_status,
                         hpcHwRpcData_t    *app_resp);


/*********************************************************************
* @purpose  This functions returns the card type associated with Slot.
*
* @param    slotNum     Slot Number.
* @param    cardTypeId  Card Type Id.
*                                       
* @returns  L7_SUCCESS   On retrieving the catd type ID from Slot number.
* @returns  L7_FAILURE   Slot Number is not present in the database.
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareCardTypeGetBySlotNum(L7_uint32 slotNum, 
                L7_uint32 *cardTypeId);

/**************************************************************************
*
* @purpose  Notifies the plug-in events for physical card & logical card  
*           plug-in to callback function.
*
* @param    notify       pointer to function
*
* @returns  L7_SUCCESS   if registrarID is valid
* @returns  L7_FAILURE   if registrarID is invalid
*
* @notes 
*
* @end
* 
*************************************************************************/
L7_RC_t hpcHardwareCardEventCallbackNotify(void (*notify)(L7_uint32 slotNum, 
                                   L7_uint32 cardTypeId, hpcEvent_t eventInfo));


/*********************************************************************
* @purpose  This routine determines if there are any stack ports.
*           If no stack ports, then unit manager can use this
*           info to speed up it's state machines.
*
* @returns  L7_BOOL
*
* @comments    
*       
* @end
*********************************************************************/
L7_BOOL hpcHardwareNoStackPorts();

/*********************************************************************
* @purpose  Inform the driver that this unit is the standby of the stack
*
* @param    stby_present {(input)} L7_TRUE: There is a valid stby
*                                L7_FALSE: There is no valid stby
* @param   
* @param    stby_key {(input)} Key of the stby unit
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareNotifyStandbyStatus(L7_BOOL stby_present, 
                                       L7_enetMacAddr_t stby_key);

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
void hpcHardwareRemoveStackManager(L7_enetMacAddr_t managerKey);

/*********************************************************************
* @purpose  Routine for application helpers to send packets on the local
*           unit.
*          
* @param    frame              @b{(input)} 
* @param    frameSize          @b{(input)} 
* @param    priority           @b{(input)} 
* @param    slot               @b{(input)} 
* @param    port               @b{(input)} 
* @param    ignoreEgressRules  @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareHelperSend(L7_uchar8   *frame,
			                  L7_uint32    frameSize,
			                  L7_uint8     priority,
			                  L7_uint8     slot,
			                  L7_ushort16  port,
			                  L7_BOOL      ignoreEgressRules);

/*********************************************************************
*
* @purpose Allocate memory for the CPU send stats for HPC helper.
*          Zero out the stat values.
*
* @returns 
*
* @notes   none
*
* @end
*
*********************************************************************/
void hpcHardwareHelperStatsInit();


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
L7_RC_t hpcHardwareShutdownPorts(L7_uint32 unitNumber);



#define L7_RPC_REQ_BUFF_HDR_SIZE (sizeof(L7_uint32))


/*********************************************************************
* @purpose  Get a buffer to send RPC request message
*
* @params   none
*
* @returns  none
*
* @returns  L7_NULLPTR if the buffer isn't allocated, otherwise a pointer to buffer
*       
* @end
*********************************************************************/
L7_uchar8* l7_rpc_req_buffer_get(void);

/*********************************************************************
* @purpose  Free the RPC request buffer
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_req_buffer_free(L7_uchar8 *buffer);


/*********************************************************************
* @purpose  Get the RPC req buffer size
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_req_buffer_size_get();

/*********************************************************************
* @purpose  Get a buffer to get RPC response message
*
* @params   none
*
* @returns  none
*
* @returns  L7_NULLPTR if the buffer isn't allocated, otherwise a pointer to buffer
*       
* @end
*********************************************************************/
hpcHwRpcData_t* l7_rpc_resp_buffer_get(void);

/*********************************************************************
* @purpose  Free the RPC response buffer
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @end
*********************************************************************/
L7_RC_t l7_rpc_resp_buffer_free(hpcHwRpcData_t *buffer);

/*********************************************************************
* @purpose  Send a message to specified units in the stack.  Message is delivered
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
* @param    registrarID routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    unitMask    destination unit(s) mask
* @param    msg_length  number of bytes in payload buffer
* @param    buffer      pointer to the payload to be sent
* @param    hdr_length  number of bytes in optional payload header
* @param    header      pointer to the optional payload header
* @param    next_hop    flag to request message transmit using next hop
*
* @returns  number of units the message successfully was successfully 
*           dispatched to harware transport
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 hpcHardwareMcastSend(L7_uint32 registrarID, L7_uint32 unitMask,
                               L7_uint32 msg_length,  L7_uchar8* buffer,
                               L7_uint32 hdr_length,  L7_uchar8* header,
                               L7_BOOL   nextHop,   msXmitStruct_t *xmitDataPtr);


#endif
