
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
* @create
*
* @author
*
* @end
**********************************************************************/

#ifndef INCLUDE_VOICE_VLAN_API_H
#define INCLUDE_VOICE_VLAN_API_H

#include "l7_common.h"
#include "dot1x_exports.h"
#include "voicevlan_exports.h"

/*********************************************************************
* @purpose  Set voice vlan admin mode
*
* @param    mode @b{(input)} voice vlan mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanAdminModeSet (L7_uint32 adminMode);

/*********************************************************************
* @purpose  Get administrative mode setting for voice vlan
*
* @param    *adminMode @b{(output)} voice admin mode
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanAdminModeGet(L7_uint32 *adminMode);

/*********************************************************************
* @purpose  Get administrative mode setting for voice vlan
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    *mode        @b{(output)} voice vlan admin mode
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAdminModeGet(L7_uint32 intIfNum,VOICE_VLAN_CONFIG_TYPE_t *mode);

/*********************************************************************
* @purpose  Set the Voice Vlan Id for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *voiceVlanId  @b{(input)} voice vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanIdSet(L7_uint32 intIfNum,L7_uint32 voiceVlanId);

/*********************************************************************
* @purpose Get the Voice Vlan Id for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *voiceVlanId  @b{(intput)} voice vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanIdGet(L7_uint32 intIfNum,L7_uint32 *voiceVlanId);

/*********************************************************************
* @purpose Set the Voice Vlan dot1p priority for the port.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    priority  @b{(intput)} priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanPrioritySet(L7_uint32 intIfNum,L7_uint32 priority);

/*********************************************************************
* @purpose Get the Voice Vlan dot1p priority for the port.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    priority  @b{(intput)} priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanPriorityGet(L7_uint32 intIfNum,L7_uint32 *priority);

/*********************************************************************
* @purpose Set the Voice Vlan configuration as none.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} voice vlan none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortNoneSet(L7_uint32 intIfNum,L7_BOOL mode);

/*********************************************************************
* @purpose Get the Voice Vlan configuration as none.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode @b{(output)} voice vlan none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortNoneGet(L7_uint32 intIfNum,L7_BOOL *mode);

/*********************************************************************
* @purpose Set the Voice Vlan configuration as none.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} voice vlan none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortUntagSet(L7_uint32 intIfNum,L7_BOOL mode);

/*********************************************************************
* @purpose Get the Voice Vlan configuration as none.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode @b{(output)} voice vlan none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortUntagGet(L7_uint32 intIfNum,L7_BOOL *mode);

/*********************************************************************
* @purpose Set the default cos override for the data traffic.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} voice vlan cos override
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortCosOverrideSet(L7_uint32 intIfNum,L7_BOOL mode);

/*********************************************************************
* @purpose Set the default cos override for the data traffic.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} voice vlan cos override
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortCosOverrideGet(L7_uint32 intIfNum,L7_BOOL *mode);
/*********************************************************************
* @purpose Get the Port status
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} voice vlan status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanApiPortStatusGet(L7_uint32 intIfNum,L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the Voice Vlan AdminMode for the port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    type         @b{(input)} type of parameter
* @param    val          @b{(input)} value for the parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAdminModeSet(L7_uint32 intIfNum,
                                  VOICE_VLAN_CONFIG_TYPE_t  type,
                                  L7_uint32 val);

/*********************************************************************
* @purpose Get the Port DSCP val
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val       @b{(input)} dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDscpGet(L7_uint32 intIfNum,L7_uint32 *val);

/*********************************************************************
* @purpose Set the Port DSCP val
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val       @b{(input)} dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Right now there is no implementation for DSCP
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDscpSet(L7_uint32 intIfNum,L7_uint32 val);

/*********************************************************************
* @purpose  Set the Voice Vlan Auth State for the port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    state        @b{(input)} auth value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAuthStateSet(L7_uint32 intIfNum, L7_BOOL state);

/*********************************************************************
* @purpose  Get the Voice Vlan Auth State for the port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    state        @b{(input)} reference to auth value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAuthStateGet(L7_uint32 intIfNum, L7_BOOL *state);

/*********************************************************************
* @purpose  Set the Voice Vlan Auth State for a client on a port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    mac          @b{(input)} mac address of the client
* @param    state        @b{(input)} auth value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function clears the mac address
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortClientAuthSet(L7_uint32 intIfNum,
                                   L7_uchar8 *mac,
                                   L7_BOOL   state);

/*********************************************************************
* @purpose  So dot1x can inform voice VLAN if the port control mode
*           has changed.
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    mode         @b{(input)} the port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sets the operational state of unauthorized
*           voice VLAN.
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1xPortControlCallback(L7_uint32 intIfNum,
                                          L7_DOT1X_PORT_CONTROL_t mode);


/*********************************************************************
* @purpose  So dot1x can inform voice VLAN if the admin mode has changed.
*
* @param    mode         @b{(input)} the port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sets the administrative state of unauthorized
*           voice VLAN.
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1xAdminModeCallback(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the first voice vlan device on the given interface.
*
* @param    intIfNum      @b{(input)} the interface
* @param    mac_addr      @b{(output)} the mac address of the first
*                                      VOIP device detected on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceInfoFirstGet(L7_uint32 intIfNum,L7_uchar8 *mac_addr);

/*********************************************************************
* @purpose  Get the next voice vlan device on the given interface.
*
* @param    intIfNum      @b{(input)} the interface
* @param    mac_addr      @b{(input/output)} the mac address of the first
*                                      VOIP device detected on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceInfoNextGet(L7_uint32 intIfNum,L7_uchar8 *mac_addr);

/*********************************************************************
* @purpose  Get the voice vlan for a device on the given interface.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    compId        @b{(input)} the component ID of the requestor
* @param    mac_addr      @b{(input)} the mac address of the VoIP
*                                     device detected on the interface
* @param    voiceVlan     @b{(output)} the voice VLAN for the device,
*                                      if the RC == L7_SUCCESS
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceVlanGet(L7_uint32 intIfNum, L7_uchar8 *mac_addr, L7_short16 *voiceVlan);

/*********************************************************************
* @purpose  Add a voice device
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    compId        @b{(input)} the component ID of the requestor
* @param    mac_addr      @b{(input)} the mac address of the VoIP
*                                     device detected on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceVlanAdd(L7_uint32 intIfNum, L7_COMPONENT_IDS_t compId, L7_uchar8 *mac_addr);

#endif  /* INCLUDE_VOICE_VLAN_API_H */
