
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  voice_vlan.h
*
* @purpose   voice vlan
*
* @component voice vlan
*
* @comments  none
*
* @create    Jan 3 2007
*
* @author    PKB
*
* @end
**********************************************************************/

#ifndef INCLUDE_VOICE_VLAN_H
#define INCLUDE_VOICE_VLAN_H

#include "voice_vlan_cfg.h"

#include "commdefs.h"
#include "lldp_api.h"
#include "dot1q_api.h"
#include "sysnet_api.h"
#include "voicevlan_exports.h"

typedef enum
{
    VOICE_VLAN_SOURCE_LLDP = 0,
    VOICE_VLAN_SOURCE_CDP,
    VOICE_VLAN_SOURCE_DHCP,

    VOICE_VLAN_SOURCE_TYPES
} VOICE_VLAN_SOURCE_TYPES_t;
   
typedef struct voiceVlanPortData_s
{
  L7_uint32       vlanId;  /* Operational vlan id */
  L7_uint32       portStatus; 
  L7_uint32       referenceCount; /* Number of voip devices on this interface */
  L7_char8        voipMac[L7_MAC_ADDR_LEN];
  L7_BOOL         portParticipation;
  L7_timespec     ctime;
  AcquiredMask    acquiredList; /* Mask of components "acquiring" an interface */
  L7_BOOL         authState;
  L7_BOOL         lldpPduAuthFlag;
} voiceVlanPortData_t;

typedef struct voiceVlanInfo_s
{
  void*                 voiceVlanLock; /* semaphore to sync the access */   
  voiceVlanPortData_t   portData[L7_VOICE_VLAN_INTF_MAX_COUNT];
  L7_uint32             overridePorts; /* keep track of count on number of override ports */
} voiceVlanInfo_t;

/*New*/
typedef struct voiceVlanDeviceInfo_s
{
  L7_char8  voipMac[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 intIfNum;
  L7_BOOL   clientAuthState;
  VOICE_VLAN_SOURCE_TYPES_t sourceType;

  void     *next;
} voiceVlanDeviceInfo_t;

extern voiceVlanPortCnfgrState_t voiceVlanPortCnfgrState;
extern voiceVlanCfg_t           *voiceVlanCfg;
extern voiceVlanInfo_t          *voiceVlanInfo;
extern L7_uint32                *voiceVlanMapTbl;

/*********************************************************************
* @purpose  Determine if the interface is valid to participate in voice 
*           vlan
*
* @param    intIfNum     @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
L7_BOOL voiceVlanIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voiceVlan port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL voiceVlanIntfIsConfigurable(L7_uint32 intIfNum,voiceVlanPortCfg_t **pCfg);

/*********************************************************************
* @purpose  Processes Voice Vlan-related event initiated by Dot1Q  
*
* @param (in)    intIfNum  Interface Number  
* @param (in)    event     
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t voiceVlanLLDPNotificationCallback(L7_uint32 intIfNum,  lldpXMedNotify_t *data);

/*********************************************************************
* @purpose  Processes Voice Vlan-related event initiated by Dot1Q  
*
* @param (in)    vlanId    Virtual LAN Id  
* @param (in)    intIfNum  Interface Number  
* @param (in)    event     
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1QNotificationCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);

/*********************************************************************
* @purpose  Process Link state changes
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    intIfEvent @b{(input)) interface event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intfEvent, NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase    Activate/Create
*
* @returns  void
*
* @notes    Transfer to Voice VLAN thread
*
* @end
*********************************************************************/
void voiceVlanStartupCallback(NIM_STARTUP_PHASE_t startupPhase);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voiceVlan port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL voiceVlanIntfConfigEntryGet(L7_uint32 intIfNum, voiceVlanPortCfg_t **pCfg);

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t voiceVlanIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Enable  voice vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    status @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  voiceVlanPortStatusSet(L7_uint32 intfNum, L7_uint32 status);

/*********************************************************************
* @purpose  Enable  voice vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    status @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  voiceVlanPortStatusGet(L7_uint32 intfNum, L7_uint32 *status);

/*********************************************************************
* @purpose  Processes  Vlan Add Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
 L7_RC_t voiceVlanDot1qAdd(L7_uint32 intIfNum,L7_uint32 vlanId);

/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface for
*           voice vlan.
*
* @param    pFirstIntIfNum @b{(output)}  pointer to first internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
 L7_RC_t voiceVlanFirstValidIntfNumber(L7_uint32 *pFirstIntIfNum);

/*********************************************************************
* @purpose  Return Internal Interface Number of next valid interface for
*           voice vlan.
*
* @param    intIfNum  @b{(input)}   Internal Interface Number
* @param    pNextintIfNum @b{(output)}  pointer to Next Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanNextValidIntf(L7_uint32 intIfNum, L7_uint32 *pNextIntIfNum);

/*********************************************************************
* @purpose  Processes  Vlan Delete Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1qDelete(L7_uint32 intIfNum,L7_uint32 vlanId);


/*********************************************************************
* @purpose  Set the Count for the number of Override Ports
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    flag       @b{(input)} True/false
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortOverrideCount(L7_uint32 intIfNum,L7_BOOL flag);

/*********************************************************************
* @purpose  Get the number of voice vlan devices identified on 
*           a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    count      @b{(output)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  voiceVlanPortReferenceCountGet(L7_uint32 intfNum, L7_uint32 *refcount);

/*TBD*/
/*********************************************************************
* @purpose Configure Voice VLAN Auth Admin State
*
* @param   state   - L7_ENABLE/L7_DISABLE
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   L7_ENABLE will allow the unauthorized vlan function.
*          This is not a saved state as such, it is called whenever
*          one of the states this feature depends upon changes.
*          I.e., Voice VLAN Admin Mode
*                Dot1x Admin Mode
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanAuthAdminModeUpdate(L7_uint32 state);

/*********************************************************************
* @purpose Configure Voice VLAN Auth State
*
* @param   intfNum - InterfaceID
*          state   - L7_ENABLE/L7_DISABLE
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanPortAuthSet(L7_uint32 intfNum, L7_uint32 state);

/*********************************************************************
* @purpose  Get voice vlan Auth Status for a specified interface
*
* @param    intfNum   @b{(input)) internal interface number
* @param    state      @b{(input)} reference to a pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAuthGet(L7_uint32 intfNum, L7_uint32 *state);

/*********************************************************************
* @purpose  Process DHCP Packet and Discard a unwanted packet not useful
*           for Voice VLAN Auth
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED    if frame has been consumed
* @returns  SYSNET_PDU_RC_IGNORED     continue processing this frame
*
* @notes    Verify the DHCP packet is destined with the option 176
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t voiceVlanDhcpPduReceiveCallback(L7_uint32 hookId,
                                                L7_netBufHandle bufHandle,
                                                sysnet_pdu_info_t *pduInfo,
                                                L7_FUNCPTR_t continueFunc);

/*******************************************************************************
* @purpose  Send a message to the voiceVlanTask to process incoming CDP PDU
*
* @param    bufHandle          buffer  @b((input)) handle to the lldpdu received
* @param    sysnet_pdu_info_t *pduInfo @b((input)) pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
********************************************************************************/
L7_RC_t voiceVlanCdpPduReceiveCallback(L7_netBufHandle bufHandle,
                                       sysnet_pdu_info_t *pduInfo);

/*******************************************************************************
* @purpose  Process incoming LLDP PDU
*
* @param    bufHandle          buffer  @b((input)) handle to the lldpdu received
* @param    sysnet_pdu_info_t *pduInfo @b((input)) pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
********************************************************************************/
L7_RC_t voiceVlanAuthLLDPduRcvCallback(L7_netBufHandle bufHandle,
                                       sysnet_pdu_info_t *pduInfo);

/*******************************************************************************
* @purpose  Deactivating Voice VLAN LLDP Auth Rcv PDU Flag
*
* @param    intIfNum
*
* @returns  none
*
* @notes    none
*
* @end
********************************************************************************/
void voiceVlanAuthLLDPduDeactivate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Add a Voip Device Mac address to the voice vlan database.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac 
* @param    vlanId    @b{(input)} voice vlan Id associated with this device
* @param    clientAuthState @b{(input)} clientAuthState of the device
* @param    sourceType      @b{(input)} sourceType of this device (LLDP, etc.)
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceAddDB(L7_uint32 intIfNum,L7_uchar8 *mac, L7_uint32 vlanId,
                                 L7_BOOL clientAuthState, VOICE_VLAN_SOURCE_TYPES_t sourceType);

/*********************************************************************
* @purpose  Add a Voip Device Mac address.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac 
* @param    vlanId    @b{(input)} voice vlan Id associated with this device
* @param    clientAuthState @b{(input)} clientAuthState of the device
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceRemoveDB(L7_uint32 intIfNum,L7_uchar8 *mac);

/*********************************************************************
* @purpose  Add a Voip Device Mac address.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac 
* @param   clientAuthestate @b{(input)} checks if voip device is unauthenticated voip device
* @param    sourceType       @b{(input)} source type for this entry (LLDP, CDP, etc.)
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceAdd(L7_uint32 intIfNum, L7_uchar8 *mac,
                                   L7_BOOL clientAuthState,
                                   VOICE_VLAN_SOURCE_TYPES_t sourceType);

/*********************************************************************
* @purpose  Remove a Voip Device Mac address.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac 
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceRemove(L7_uint32 intIfNum,L7_uchar8 *mac);

/*********************************************************************
* @purpose  Remove all the Voip Devices on a interface.
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceRemoveAll(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Remove all the Voip Devices on a interface.
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceAddAll(L7_uint32 intIfNum);

/*Voice Vlan DB functions*/

/*********************************************************************
* @purpose  Initialize voice vlan Device Info Database
*
* @param    nodeCount    @b{(input)} The number of nodes to be created. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoDBInit(L7_uint32 nodeCount);

/*********************************************************************
* @purpose  DeInitialize voice vlan device Info Database
*
* @param    none 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoDBDeInit(void);

/*********************************************************************
* @purpose  To Take lock for the voice vlan device Info Node
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments This lock needs to be taken only the API functions not running in 
*           the dot1x threads context.
*       
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoTakeLock(void);

/*********************************************************************
* @purpose  To Giveup lock for the voice vlan device  Info Node
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments This lock needs to be taken only the API functions not running in 
*           the dot1x threads context.
*       
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoGiveLock(void);

/*********************************************************************
* @purpose  To allocate a voice vlan Device Info Node
*
* @param    mac_addr    @b{(input)} Voip device mac address
* @param    vlanId      @b{(input)} Voice Vlan Id associated with this mac address
* @param    intIfNum    @b{(input)} The internal interface on which this 
*                                   voip device was identified
* @param    clientAuthState @b{(input)} indicates if this device is an unauthenticated device
* @param    sourceType      @b{(input)} sourceType of this device (LLDP, etc.)
*
* @returns  L7_SUCCESS    if the entry was created sucessfully 
*           L7_FAILURE    
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoAlloc(L7_uchar8 *mac_addr,
                                  L7_uint32  vlanId,
                                  L7_uint32  intIfNum,
                                 L7_BOOL                    clientAuthState,
                                 VOICE_VLAN_SOURCE_TYPES_t  sourceType);

/*********************************************************************
* @purpose  To Deallocate a voice Vlan Device Info Node
*
* @param    node  pointer to voiceVlanDeviceInfo structure 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoDeAlloc(voiceVlanDeviceInfo_t *node);

/*********************************************************************
* @purpose  To Get a voice vlan Device Info Node
*
* @param    mac_addr  @b{(input)} The mac address of the voip device
*
* @returns  voice vlan device info node
*
* @comments none
*       
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoGet(L7_uchar8* mac_addr_key);

/*********************************************************************
* @purpose  To Get Next voice vlan Device Info Node
*
* @param    mac_addr_key  @b{(input)} The mac address of the voice vlan device
*
* @returns  Logical Internal Interface node
*
* @comments none
*       
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoGetNext(L7_uchar8 *mac_addr_key);

/*********************************************************************
* @purpose  To get First voice vlan device  for a physical interface
*
* @param    intIfNum  @b{(input)} The internal interface 
*
* @returns  Voice Vlan Device node
*
* @comments 
*       
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoFirstGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  To iterate all the voice vlan devices of a physical interface
*
* @param    intIfNum  @b{(input)} The internal interface 
* @param    mac_addr  @b{(input/output)} The mac address of the last learnt voip device
*
* @returns  voice vlan device node
*
* @comments Give the last learnt mac address on that interface to get the next one
*       
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoGetNextNode(L7_uint32 intIfNum,
                                                        L7_uchar8 *mac_addr);

/*********************************************************************
* @purpose  Debug Info of the voice vlan DB
*
* @param    None
*
* @returns  None
*
* @comments none
*       
* @end
*********************************************************************/
void voiceVlanDebugDeviceInfo(void);

#endif  /* INCLUDE_VOICE_VLAN_H */
