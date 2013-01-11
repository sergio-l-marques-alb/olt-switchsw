/****************************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
*****************************************************************************
*
* @filename usmdb_dot1ad.h
*
* @purpose Provide interface to USMDB API's for dot1ad
*
* @component DOT1AD
*
* @comments 
*
* @create 04/14/2008
*
* @author Rajakrishna
* @end
*
****************************************************************************/

#ifndef USMDB_DOT1D_H
#define USMDB_DOT1D_H

#include "l7_common.h"

#include "usmdb_common.h"
#include "dot1ad_l2tp_api.h"

/****************************************************************************
* @purpose Get the Dot1ad interface type
*
* @param    UnitIndex @b((input)) Unit
* @param  BasePort    @b((input))  Internal interface number
* @param  intfType     @b((output)) Dot1ad interface type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adInterfaceTypeGet ( L7_uint32 UnitIndex, 
                         L7_int32 BasePort, 
                         DOT1AD_INTFERFACE_TYPE_t  *intfType );

/****************************************************************************
* @purpose Set the Dot1ad interface type
*
* @param    UnitIndex  @b((input)) Unit
* @param    BasePort   @b((input)) Internal interface number
* @param    intfType   @b((input)) Dot1ad interface type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adInterfaceTypeSet(L7_uint32 UnitIndex, 
                         L7_int32 BasePort, 
                         DOT1AD_INTFERFACE_TYPE_t  intfType);

/****************************************************************************
* @purpose	Get the Dot1ad tunneling action 
*
* @param    UnitIndex     @b((input))    Unit
* @param    BasePort      @b((input))    Internal interface number
* @param    mac             @b((input))    L2 protocol reserved MAC address.
* @param    protocolId    @b((input))    L2 protocol identifier in MAC header.
* @param    tunnelAction @b((output))  Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adL2ProtocolTunnelingGet ( L7_uint32 UnitIndex, 
                                L7_int32 BasePort, 
                                L7_uchar8 *mac, 
                                L7_int32 protocolId, 
                                DOT1AD_TUNNEL_ACTION_t *tunnelAction );
/****************************************************************************
* @purpose  Get the first Dot1ad tunneling action
*
* @param    UnitIndex     @b((input))    Unit
* @param    BasePort      @b((input))    Internal interface number
* @param    mac             @b((input))    L2 protocol reserved MAC address.
* @param    protocolId    @b((input))    L2 protocol identifier in MAC header.
* @param    tunnelAction @b((output))  Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adL2ProtocolTunnelingFirstGet ( L7_uint32 UnitIndex,
                                L7_int32 *BasePort,
                                L7_uchar8 *mac,
                                L7_int32 *protocolId,
                                DOT1AD_TUNNEL_ACTION_t *tunnelAction );

/****************************************************************************
* @purpose  Get the Next Dot1ad tunneling action 
*
* @param    UnitIndex     @b((input))    Unit
* @param    BasePort      @b((input))    Internal interface number
* @param    mac             @b((input))    L2 protocol reserved MAC address.
* @param    protocolId    @b((input))    L2 protocol identifier in MAC header.
* @param    tunnelAction @b((output))  Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adL2ProtocolTunnelingGetNext ( L7_uint32 UnitIndex, 
                                L7_int32 *BasePort, 
                                L7_uchar8 *mac, 
                                L7_int32 *protocolId, 
                                DOT1AD_TUNNEL_ACTION_t *tunnelAction );

/****************************************************************************
* @purpose Set the Dot1ad tunneling action based on port
*
* @param    UnitIndex         @b((input)) Unit
* @param    BasePort          @b((input)) Internal interface number
* @param    unconfig          @b((input)) unconfigure tunnel action.
* @param    mac                 @b((input)) L2 protocol reserved MAC address.
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
L7_RC_t usmDbDot1adL2ProtocolTunnelingSet ( L7_uint32 UnitIndex,
                         L7_uint32 BasePort,
                         L7_BOOL unconfig,
                         L7_uchar8 *mac,
                         L7_BOOL isProtoIdConfig,
                         L7_int32 protocolId,
                         DOT1AD_TUNNEL_ACTION_t tunnelAction );

/*********************************************************************
*
* @purpose  To get the index of a first configured tunnel action.
*
* @param    UnitIndex          @b((input)) Unit
* @param    BasePort           @b((input)) Internal interface number
* @param    *tunnelActionIndex @b{(output)} tunnel action index
*
* @returns  L7_SUCCESS     tunnel action index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmdbDot1adIntfTunnelActionIndexFirstGet ( L7_uint32 UnitIndex, 
                                   L7_int32 BasePort, 
                                   L7_uint32 *tunnelActionIndex);

/*********************************************************************
*
* @purpose  To get the next index of a configured tunnel action.
*
* @param    UnitIndex            @b((input)) Unit
* @param    BasePort             @b((input)) Internal interface number
* @param    oldTunnelActionIndex @b((input)) current action index
* @param    *tunnelActionIndex   @b{(output)} tunnel action index
*
* @returns  L7_SUCCESS     tunnel action index is retrieved
* @returns  L7_FAILURE     invalid parms, or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmdbDot1adIntfTunnelActionIndexNextGet ( L7_uint32 UnitIndex, 
                                   L7_int32 BasePort, 
                                   L7_uint32 oldTunnelActionIndex,
                                   L7_uint32 *tunnelActionIndex);

/****************************************************************************
* @purpose	Get the ProtocolMAC,Protocol ID and Tunneling action for the given Interface 
*
* @param    UnitIndex   @b((input)) Unit
* @param    BasePort    @b((input))	Internal interface number
* @param    intfCfg       @b((output)) Tunneling configuration of Interface.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adProtocolTunnelInfoPerIntfGet ( L7_uint32 UnitIndex, 
                                  L7_int32 BasePort, 
                                  dot1adIntfCfg_t *intfCfg );


/****************************************************************************
* @purpose Get the dot1ad configuration for all interfaces. 
*
* @param       UnitIndex       @b((input)) Unit
* @param      dot1adCfg       @b((output)) dot1ad configuration for all interfaces.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adProtocolTunnelCfgGet ( L7_uint32 UnitIndex, 
                                               dot1adCfg_t *dot1adCfg);
/****************************************************************************
* @purpose  Checks the interface is valid for dot1ad or not
*
* @param    intfNum  @b((input))  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adIntfIsValid  (L7_uint32 intfNum);

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

L7_RC_t usmDbDot1adNumActionsCountPerIntfGet(L7_uint32 UnitIndex, 
                                                           L7_uint32 intIfNum);

/**************************************************************************
* @purpose Get the ProtocolMAC,Protocol ID and Tunneling action for the given Interface
*                and action index
*
* @param   intfIfNum       @b((input))  Internal interface number
* @param   action_index  @b((input))  Tunnel action entry index
* @param   intfCfg          @b((output)) Tunneling configuration of Interface.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*
* @end
**************************************************************************/

L7_RC_t usmDbDot1adProtocolTunnelActionForIndexGet(L7_uint32 intIfNum,
                                                  L7_uint32 action_index,
                                                  dot1adProtocolTunnelingAction_t *actionCfg);

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
L7_RC_t usmDbDot1adIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Return Internal Interface Number of the next valid 
*          dot1ad port.
*
* @param L7_uint32 intIfNum       internal interface number
* @param L7_uint32 *nextIntIfNum  pointer to the next valid internal
*                                 interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDot1adIntfNextGet(L7_uint32 UnitIndex,
                               L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

/****************************************************************************
* @purpose Set the Dot1ad tunneling action based on Vlan
*
* @param    vlanId           @b((input)) vlan id.
* @param    mac              @b((input)) L2 protocol reserved MAC address.
* @param    protocolId       @b((input)) L2 protocol identifier in MAC header.
* @param    tunnelAction     @b((input)) Dot1ad tunneling action
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adL2ptActionAdd( L7_uint32 UnitIndex,
                         L7_ushort16 vlanId,
                         L7_uchar8 *mac,
                         L7_ushort16 protocolId,
                         DOT1AD_TUNNEL_ACTION_t tunnelAction );

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
L7_RC_t usmdbDot1adL2ptActionFirstEntryGet
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
L7_RC_t usmdbDot1adL2ptActionNextEntryGet(L7_ushort16 vlanId ,
                                     L7_enetMacAddr_t   protocolMAC,
                                     L7_ushort16 protocolId,
                                     dot1adL2ptEntryTreeNode_t  *nextEntry);

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
L7_RC_t usmdbDot1adL2ptActionEntryGet(L7_ushort16 vlanId ,
                                     L7_enetMacAddr_t   protocolMAC,
                                     L7_ushort16 protocolId,
                                     DOT1AD_TUNNEL_ACTION_t *nextEntry);

/****************************************************************************
* @purpose Deletes the Dot1ad tunneling action based on Vlan
*
* @param    vlanId           @b((input)) vlan id.
* @param    mac              @b((input)) L2 protocol reserved MAC address.
* @param    protocolId       @b((input)) L2 protocol identifier in MAC header.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  none
*
* @end
****************************************************************************/
L7_RC_t usmDbDot1adL2ptActionDelete( L7_uint32 UnitIndex,
                         L7_ushort16 vlanId,
                         L7_uchar8 *mac,
                         L7_ushort16 protocolId);


#endif


