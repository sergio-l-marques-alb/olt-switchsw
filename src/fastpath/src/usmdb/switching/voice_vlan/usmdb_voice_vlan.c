/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  usmdb_voice_vlan.c
*
* @purpose   USMDB API's for Voice Vlan
*
* @component voice vlan
*
* @comments 
*
* @create    
*
* @author     
*
* @end
*             
**********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "l7_common.h"
#include "usmdb_util_api.h"
#include "usmdb_voice_vlan_api.h"
#include "osapi.h"

#include "voice_vlan_api.h"




/*********************************************************************
*
* @purpose  Set the Voice Vlan Admin Mode.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanAdminModeSet(L7_uint32 unitIndex,
                                    L7_uint32 mode )
{
  return voiceVlanAdminModeSet(mode);
}

/*********************************************************************
*
* @purpose  Get the Voice Vlan Admin Mode.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanAdminModeGet(L7_uint32 unitIndex,
                                    L7_uint32 *mode )
{
  return voiceVlanAdminModeGet( mode);
}


/*********************************************************************
*
* @purpose  Get the Voice Vlan Port Admin Mode.
*
* @param    intfNum   @b((input)) the unit for this operation
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortAdminModeGet(L7_uint32 unitIndex,
                                        L7_uint32 intfNum, 
                                        VOICE_VLAN_CONFIG_TYPE_t *mode)
{
  return voiceVlanPortAdminModeGet(intfNum,mode);
}


/*********************************************************************
*
* @purpose  Set the Voice Vlan <vlan-id>.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *vlanId   @b{(input)} voice vlan Id
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortVlanIdSet(L7_uint32 unitIndex,
                                     L7_uint32 intIfNum,
                                     L7_uint32 vlanId )
{
  return voiceVlanPortVlanIdSet(intIfNum,vlanId); 
}

/*********************************************************************
*
* @purpose  Get the Voice Vlan <vlan-id>.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *vlanId   @b{(output)} voice vlan Id
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortVlanIdGet(L7_uint32 unitIndex,
    L7_uint32 intIfNum,
    L7_uint32 *vlanId )
{
  return voiceVlanPortVlanIdGet(intIfNum,vlanId);
}

/*********************************************************************
*
* @purpose  Set the Voice Vlan dot1p <priority>.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *priority @b{(input)} voice vlan dot1p <priority>
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortVlanPrioritySet(L7_uint32 unitIndex,
                                     L7_uint32 intIfNum,
                                     L7_uint32 priority )
{
  return voiceVlanPortVlanPrioritySet(intIfNum,priority); 
}

/*********************************************************************
*
* @purpose  Get the Voice Vlan dot1p <priority>.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *priority @b{(output)} voice vlan dot1p <priority>
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortVlanPriorityGet(L7_uint32 unitIndex,
                                          L7_uint32 intIfNum,
                                          L7_uint32 *priority )
{
  return voiceVlanPortVlanPriorityGet(intIfNum,priority);
}

/*********************************************************************
*
* @purpose  Enable/Disable default configuration for Voice Vlan.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortNoneSet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_BOOL mode )
{
  return voiceVlanPortNoneSet(intIfNum,  mode);
}

/*********************************************************************
*
* @purpose  Get default configuration for Voice Vlan.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortNoneGet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_BOOL *mode )
{
  return voiceVlanPortNoneGet(intIfNum, mode);
}


/*********************************************************************
*
* @purpose  Enable/Disable tagging for Voice Vlan.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortUntagSet(L7_uint32 unitIndex,
                                    L7_uint32 intIfNum,
                                    L7_BOOL mode )
{
  return voiceVlanPortUntagSet(intIfNum,mode);
}

/*********************************************************************
*
* @purpose  Get voice vlan untagging mode.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortUntagGet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_BOOL *mode )
{
  return voiceVlanPortUntagGet(intIfNum, mode);
}


/*********************************************************************
*
* @purpose  Configure trust or untrust the data traffic arriving on the voice vlan port.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortCosOverrideSet(L7_uint32 unitIndex,
                                         L7_uint32 intIfNum,
                                         L7_BOOL mode )
{
  return voiceVlanPortCosOverrideSet(intIfNum,mode);
}

/*********************************************************************
*
* @purpose  Get the value of the cos mode.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortCosOverrideGet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_BOOL *mode )
{
  return voiceVlanPortCosOverrideGet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Get the value of the dscp value.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *dscp     @b{(input)} the configured dscp vlaue on the port
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortDscpGet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *dscp )
{
  return voiceVlanPortDscpGet(intIfNum, dscp);
}

/*********************************************************************
*
* @purpose  Get the value of the dscp value.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *dscp     @b{(input)} the dscp vlaue to be configured 
*                                 on the port
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, L7_NO_VALUE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPortDscpSet(L7_uint32 unitIndex,
                                   L7_uint32 intIfNum,
                                   L7_uint32 dscp )
{
  return voiceVlanPortDscpSet(intIfNum, dscp);
}

/*********************************************************************
*
* @purpose  Determine if the port is valid for voice vlan
*          
* @param unit         the unit index (for a stacking environment)
* @param iface        the interface to check
*
* @return   L7_SUCCESS the interface is valid
* @return   L7_FAILURE the interface is not a valid type for dot1s.
*
* @note An interface is considered valid for dot1s if it is one of
*       the following types:    USM_PHYSICAL_INTF
*       In case of failure, the failure message is written to the 
*       cli context.  The caller needs only to return.    
*
* @end
*
*********************************************************************/
L7_RC_t usmDbVoiceVlanInterfaceValidate(L7_uint32 unit, 
                                    L7_uint32 iface )
{
    L7_uint32 inclIntfTypes = USM_PHYSICAL_INTF;
    L7_uint32 rc = L7_SUCCESS;
    L7_uint32 intIfType;

    /* Get the interface type always returns L7_SUCCESS*/
    (void)usmDbIntIfNumTypeMaskGet(unit, iface, &intIfType); 

    /* Validate the interface type */      
    if ( (intIfType & inclIntfTypes) == 0 ) 
    {
        rc = L7_FAILURE;
    }

    return rc;
}

/*********************************************************************
*
* @purpose  Get the Port status
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} the boolean value of the mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbVoiceVlanPorStatusGet(L7_uint32 unitIndex,
                                    L7_uint32 intIfNum,
                                    L7_uint32 *mode )
{
  return voiceVlanApiPortStatusGet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Set the Port AdminMode
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    type      @b{(input)} type of parameter
* @param    val       @b{(input)} the value 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  usmdbvoiceVlanPortAdminModeSet(L7_uint32 unitIndex,
                                        L7_uint32 intIfNum,
                                        VOICE_VLAN_CONFIG_TYPE_t  type,
                                        L7_uint32 val)
{
  return voiceVlanPortAdminModeSet(intIfNum,type,val);
}





/*********************************************************************
*
* @purpose  Enable/Disable Auth State for Voice Vlan.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    state     @b{(input)} the boolean value of the auth state
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortAuthSet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_BOOL state)
{
  return voiceVlanPortAuthStateSet(intIfNum, state);
}

/*********************************************************************
*
* @purpose  Get voice vlan Auth State.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *state    @b{(input)} reference to the boolean value of the
*                                 auth state
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortAuthGet(L7_uint32 unitIndex,
                                  L7_uint32 intIfNum,
                                  L7_BOOL *state)
{
  return voiceVlanPortAuthStateGet(intIfNum, state);
}

/*********************************************************************
*
* @purpose  Get the first voip device identified on the port.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mac_addr @b{(output)} reference to mac address of the 
*                                  first voip device on the port 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortDeviceInfoFirstGet(L7_uint32 unitIndex,
                                             L7_uint32 intIfNum,
                                             L7_uchar8 *mac_addr)
{
  if (mac_addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  return voiceVlanPortDeviceInfoFirstGet(intIfNum,mac_addr);
}

/*********************************************************************
*
* @purpose  Get the first voip device identified on the port.
*
* @param    unitIndex @b((input)) the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mac_addr @b{(input/output)} reference to mac address of the 
*                                  next voip device on the port 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmdbVoiceVlanPortDeviceInfoNextGet(L7_uint32 unitIndex,
                                             L7_uint32 intIfNum,
                                             L7_uchar8 *mac_addr)
{
  if (mac_addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  return voiceVlanPortDeviceInfoNextGet(intIfNum,mac_addr);
}
