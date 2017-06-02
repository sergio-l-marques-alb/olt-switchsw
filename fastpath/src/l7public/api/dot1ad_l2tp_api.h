
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
**********************************************************************
*
* @filename       dot1ad_l2tp_api.h
* @purpose        Dot1ad L2 protocol Tunnel API definitions and functions
* @component    dot1ad
* @comments    none
* @create         04/14/2008
* @author         Rajakrishna
* @end
*
**********************************************************************/

#ifndef INCLUDE_DOT1AD_L2TP_API_H
#define INCLUDE_DOT1AD_L2TP_API_H

extern dot1adCfg_t *dot1adCfg;
extern L7_uint32 traceFlags;

/* Structure to represent a dot1ad L2tp entry key in each tree node. */
typedef struct dot1adL2ptEntryKey_s
{
  /* VLAN station is in. */
  L7_ushort16 vlanId;

  /* A MAC address uniquely identifies a node in the bindings tree. */
  L7_enetMacAddr_t macAddr;

  /* Protocol ID */
  L7_ushort16 protocolId;
}dot1adL2ptEntryTreeKey_t;


/* Structure to represent a dot1ad L2TP entry in each tree node.
 * Key is the VLAN, MAC address and protocol id */
typedef struct dot1adL2ptEntryTreeNode_s
{

  dot1adL2ptEntryTreeKey_t l2ptEntryKey;
  DOT1AD_TUNNEL_ACTION_t   tunnelAction;

  void *next;                /* AVL next */

} dot1adL2ptEntryTreeNode_t;


/*********************************************************************
* @purpose  Determine if the interface is valid and capable of supporting dot1ad
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1adIsValidIntf(L7_uint32 intIfNum);

/**************************************************************************
* @purpose Get the Dot1ad interface type
*
* @param   intIfNum  @b((input))  Internal interface number
* @param   intfType  @b((output)) Dot1ad interface type

* @param   intfType  @b((input))  Dot1ad interface type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
**************************************************************************/

L7_RC_t  dot1adInterfaceTypeGet(L7_uint32 intIfNum,
                                 DOT1AD_INTFERFACE_TYPE_t  *intfType);


/**************************************************************************
* @purpose Get the NNI interface 
* @param   *intIfNum  @b((input))  NNI interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes        none
*
* @end
**************************************************************************/
L7_RC_t  dot1adNniInterfaceGet(L7_uint32 *intIfNum);

/**************************************************************************
* @purpose Set the Dot1ad interface type
*
* @param   intIfNum            @b((input))  Internal interface number
* @param   intfType            @b((output)) Dot1ad interface type
* @param   verifyPreviousCong  @b((input)) Verify previous configuration
*                              before applying the request configuration.
* @param   detachRequest       @b((input)) Truth value to be verified for
*                              requesting the DVLAN component.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes	none
*
* @end
**************************************************************************/
L7_RC_t  dot1adInterfaceTypeSet(L7_uint32 intIfNum,
                                DOT1AD_INTFERFACE_TYPE_t  intfType,
                                L7_BOOL verifyPreviousConf,
                                L7_BOOL detachRequest);

/****************************************************************************
* @purpose Set the Dot1ad tunneling action 
*
* @param    intIfNum          @b((input)) Internal interface number
* @param    unconfig          @b((input)) unconfigure tunnel action.
* @param    protocolMAC       @b((input)) L2 protocol reserved MAC address.
* @param    isProtoIdConfig @b((input)) L2 protocol Id configured or not.
* @param    protocolId         @b((input)) L2 protocol identifier in MAC header.
* @param    tunnelAction      @b((input)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t dot1adL2ProtocolTunnelingSet(L7_uint32 intIfNum,
                               L7_BOOL  unconfig,
                               L7_enetMacAddr_t   protocolMAC,
                               L7_BOOL   isProtoIdConfig,
                               L7_uint32   protocolId,
                               DOT1AD_TUNNEL_ACTION_t  tunnelAction);

/**************************************************************************
* @purposeGet the Dot1ad tunneling action 
*
* @param   intfIfNum       @b((input)) Internal interface number
* @param   protocolMAC  @b((input)) L2 protocol reserved MAC address.
* @param   protocolId     @b((input)) L2 protocol identifier in MAC header.
* @param   tunnelAction  @b((output)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  The same is used for protocolMAC, 
*             protocolId and interface specific display purpose.
*
* @end
**************************************************************************/
L7_RC_t dot1adL2ProtocolTunnelingGet(L7_uint32 intIfNum,
                                      L7_enetMacAddr_t   protocolMAC,
                                      L7_uint32  protocolId,
                                      DOT1AD_TUNNEL_ACTION_t  *tunnelAction);

/**************************************************************************
* @purposeGet the First configured Dot1ad tunneling action
*
* @param   intfIfNum       @b((input)) Internal interface number. Send 0
*                                      to get the first valid intIfNum
* @param   protocolMAC  @b((input)) L2 protocol reserved MAC address. Send
*                                   NULL MAC to get the first valid mac
* @param   protocolId     @b((output)) L2 protocol identifier in MAC header.
*                                     Send 0 to get the first valid proto ID
* @param   tunnelAction  @b((output)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*           
*
* @end
**************************************************************************/
L7_RC_t dot1adL2ProtocolTunnelingFirstGet(L7_uint32 *intIfNum,
                                          L7_uchar8 *protocolMAC,
                                          L7_uint32 *protocolId,
                                          DOT1AD_TUNNEL_ACTION_t  *tunnelAction);

/**************************************************************************
* @purposeGet the Next Dot1ad tunneling action 
*
* @param   intfIfNum       @b((input)) Internal interface number
* @param   protocolMAC  @b((input)) L2 protocol reserved MAC address.
* @param   protocolId     @b((input)) L2 protocol identifier in MAC header.
* @param   tunnelAction  @b((output)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  The same is used for protocolMAC, 
*             protocolId and interface specific display purpose.
*
* @end
**************************************************************************/

L7_RC_t dot1adL2ProtocolTunnelingGetNext(L7_uint32 *intIfNum,
                                      L7_uchar8   *protocolMAC,
                                      L7_uint32  *protocolId,
                                      DOT1AD_TUNNEL_ACTION_t  *tunnelAction);

/*********************************************************************
*
* @purpose  To get the index of a first configured tunnel action.
*
* @param    intfIfNum   @b((input))  Internal interface number
* @param    *tunnelActionIndex  @b{(output)} tunnel action index
*
* @returns  L7_SUCCESS     tunnel action index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adIntfTunnelActionIndexFirstGet(L7_uint32 intIfNum, 
                                   L7_uint32 *tunnelActionIndex);


/*********************************************************************
*
* @purpose  To get the next index of a configured tunnel action.
*
* @param    intfIfNum   @b((input))  Internal interface number
* @param    oldTunnelActionIndex   @b((input)) current action index
* @param    *tunnelActionIndex  @b{(output)} next tunnel action index
*
* @returns  L7_SUCCESS     tunnel action index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adIntfTunnelActionIndexNextGet(L7_uint32 intIfNum, 
                           L7_uint32 oldTunnelActionIndex, 
                                   L7_uint32 *tunnelActionIndex);

/**************************************************************************
* @purpose Get the ProtocolMAC,Protocol ID and Tunneling action for the given Interface 
*
* @param   intfIfNum   @b((input))  Internal interface number
* @param   intfCfg       @b((output)) Tunneling configuration of Interface.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
**************************************************************************/

L7_RC_t dot1adProtocolTunnelInfoPerIntfGet(L7_uint32 intIfNum,
                                                  dot1adIntfCfg_t *intfCfg);

/**************************************************************************
* @purpose Get the dot1ad configuration for all interfaces. 
*
* @param   dot1adCfg    @b((output)) dot1ad configuration for all interfaces.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
**************************************************************************/

L7_RC_t dot1adProtocolTunnelCfg(dot1adCfg_t *pDot1adCfg);

/**************************************************************************
* @purpose show the dot1ad statastics per Interface. 
*
* @param   intfIfNum        @b((input)) Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
**************************************************************************/
L7_RC_t dot1adDebugIntfStatsShow(L7_uint32 intIfNum);

/**************************************************************************
* @purpose Get the number of configured actions.
*
* @param   intfIfNum       @b((input))  Internal interface number
*
* @returns  number of actions count
*
* @notes   None
*
* @end
**************************************************************************/

L7_RC_t dot1adNumActionsCountPerIntfGet(L7_uint32 intIfNum);


/**************************************************************************
* @purpose Get the ProtocolMAC,Protocol ID and Tunneling action for the given Interface
*                and action index
*
* @param   intfIfNum     @b((input))  Internal interface number
* @param   action_index  @b((input))  Tunnel action entry index
* @param   actionCfg     @b((output)) Tunneling configuration of Interface.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
**************************************************************************/

L7_RC_t dot1adProtocolTunnelActionForIndexGet(L7_uint32 intIfNum,
                                                  L7_uint32 action_index,
                                                  dot1adProtocolTunnelingAction_t *actionCfg);

/**************************************************************************
* @purpose Dumps the Dot1ad tunneling Entries for a given interface
*
* @param   intfIfNum  @b((input))  Internal interface number
*
* @returns  None
*
* @notes
*
* @end
**************************************************************************/
void dot1adDebugProtocolTunnelActionShow(L7_uint32 intIfNum);

/* VLAN based L2PT routines */
/****************************************************************************
* @purpose Set the Dot1ad tunneling action based on VLAN
*
* @param    vlanId            @b((input)) VLAN id 
* @param    protocolMAC       @b((input)) L2 protocol reserved MAC address.
* @param    protocolId        @b((input)) L2 protocol identifier in MAC header.
* @param    tunnelAction      @b((input)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t dot1adL2ptActionAdd (L7_ushort16  vlanId,
                             L7_enetMacAddr_t   protocolMAC,
                             L7_ushort16   protocolId,
                             DOT1AD_TUNNEL_ACTION_t  tunnelAction);

/****************************************************************************
* @purpose Deletes the Dot1ad tunneling action based on VLAN
*
* @param    vlanId            @b((input)) VLAN id 
* @param    protocolMAC       @b((input)) L2 protocol reserved MAC address.
* @param    protocolId        @b((input)) L2 protocol identifier in MAC header.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t dot1adL2ptActionDelete (L7_ushort16  vlanId,
                                L7_enetMacAddr_t   protocolMAC,
                                L7_ushort16   protocolId);


/**************************************************************************
* @purpose Get the  protocol tunneling action entry
*
* @param   vlanId         @b((input)) vlan ID.
* @param   protocolMAC    @b((input)) L2 protocol reserved MAC address.
* @param   protocolId     @b((input)) L2 protocol identifier in MAC header.
* @param   tunnelAction   @b((output)) L2 protocol tunnel Action.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
**************************************************************************/
L7_RC_t dot1adL2ptActionEntryGet(L7_ushort16 vlanId ,
                                 L7_enetMacAddr_t   protocolMac,
                                 L7_ushort16 protocolId,
                                 DOT1AD_TUNNEL_ACTION_t *tunnelAction);




/*********************************************************************
*
* @purpose  To get the index of a first configured tunnel action that
*           is configured based on VLAN. 
*
* @param    *dot1adL2tpEntryTreeNode_t  @b((output)) First Entry
*
* @returns  L7_SUCCESS     tunnel action index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1adL2ptActionFirstEntryGet
                              (dot1adL2ptEntryTreeNode_t  *firstEntry);

/**************************************************************************
* @purpose Get the Next protocol tunneling action entry
*
* @param   vlanId         @b((input)) vlan ID.
* @param   protocolMAC    @b((input)) L2 protocol reserved MAC address.
* @param   protocolId     @b((input)) L2 protocol identifier in MAC header.
* @param   nextEntry      @b((output)) nextEntry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  The same is used for protocolMAC,
*             protocolId and interface specific display purpose.
*
* @end
**************************************************************************/
L7_RC_t dot1adL2ptActionNextEntryGet(L7_ushort16 vlanId ,
                                     L7_enetMacAddr_t   protocolMac,
                                     L7_ushort16 protocolId,
                                     dot1adL2ptEntryTreeNode_t  *nextEntry);


#endif

