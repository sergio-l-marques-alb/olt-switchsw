/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_outcalls.c
*
* @purpose    Contains definitions to APIs used to interract with 
*             external components
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/

#include "nimapi.h"
#include "dot3ad_api.h"
#include "dot1q_api.h"
#include "dot1s_api.h"
#include "l7_ip_api.h"
#include "cnfgr.h"

#include "snooping.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid for Snooping
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE   Valid interface type
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL snoopIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LAG_INTF:
      return L7_TRUE;
    default:
      break;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for Snooping
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE   Supported interface
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL snoopIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;
  L7_RC_t         rc = L7_FALSE;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  { 
    return rc;
  }

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
      case L7_CAPWAP_TUNNEL_INTF:
      case L7_PHYSICAL_INTF:
        rc = L7_TRUE;
        break;

      case L7_LAG_INTF:
        if (dot3adIsLagConfigured(intIfNum) == L7_FALSE)
        {
          rc = L7_FALSE;
        }
        else
        {
          rc = L7_TRUE;
        }
        break;

      default:
        rc = L7_FALSE;
        break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Get next valid vlan id
*
* @param    vid      @b{(input)}  current vlan id
* @param    nextvid  @b{(output)} next valid vlan id
*
* @returns  L7_SUCCESS  When a valid vid is found
* @returns  L7_FAILURE  no more vlans
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t snoopNextVlanGet(L7_uint32 vid, L7_uint32 *nextvid)
{
  return dot1qNextVlanGet(vid, nextvid);
}
/*********************************************************************
* @purpose  Check if VLAN is eligible for snooping configuration
*
* @param    vid      @b{(input)}  VLAN ID in question
*
* @returns  L7_SUCCESS  Valid VLAN
* @returns  L7_FAILURE  Invalid VLAN for configuration
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanCheckValid(L7_uint32 vid)
{
  if (cnfgrIsFeaturePresent(L7_SNOOPING_COMPONENT_ID, 
                            L7_SNOOPING_FEATURE_VLAN_PRECONFIG) 
                            == L7_TRUE)
  {
    return (vid < L7_DOT1Q_MIN_VLAN_ID || vid > L7_DOT1Q_MAX_VLAN_ID) ? L7_FAILURE : L7_SUCCESS;
  }
  else 
  {
    return dot1qVlanQueueSyncCheckValid(vid);
  }
}
/*********************************************************************
* @purpose  Get the next vlan id eligible for Snooping configuration, given
*           the current vlan id
*
* @param    vlanId   @b((input)) VLAN Id
* @param    nextVlan @b((output)) next vlan id
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopVlanNextGet(L7_uint32 vlanId, L7_uint32 *nextVlan)
{
  if (cnfgrIsFeaturePresent(L7_SNOOPING_COMPONENT_ID,
                            L7_SNOOPING_FEATURE_VLAN_PRECONFIG)
                            == L7_TRUE)
  {
    if (vlanId == L7_NULL) 
    {
     *nextVlan = L7_DOT1Q_MIN_VLAN_ID;
    }
    else if (vlanId >= L7_DOT1Q_MAX_VLAN_ID)
    {
      return L7_FAILURE;
    }
    else
    {
      *nextVlan = vlanId + 1;
    }
    return L7_SUCCESS;
  }
  else
  {
    return dot1qNextVlanGet(vlanId, nextVlan);
  }
}

/*********************************************************************
* @purpose  Checks if the interface/VLAN ID can be enabled for Snooping
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    vlanId    @b{(input)} VLAN ID
*
* @returns  L7_TRUE, if interface can be enabled
* @returns  L7_FALSE, if interface cannot be enabled
*
* @notes    Acceptability criteria
*              1. Must not be the mirror dest port   &&
*              2. Must not be a LAG member           &&
*              3. Must not be enabled for routing    &&
*              4. Must be in the Forwarding state
*           This function only checks the above criteria.  All other
*           checking (i.e. validity of interface number) can be done
*           in snoopIntfCheck() or snoopIntfValidate().  This function
*           can be used to determine when an interface can be configured,
*           but, not enabled.
*
*
* @end
*********************************************************************/
L7_BOOL snoopIntfCanBeEnabled(L7_uint32 intIfNum, L7_uint32 vlanId)
{
  L7_uint32     activeState;
  L7_uint32     mstID;
  L7_uint32     currAcquired;
  snoop_eb_t   *pSnoopEB = L7_NULLPTR;
  L7_uint32     routingEnabled = L7_DISABLE;

  pSnoopEB = snoopEBGet();
  /* check if previously acquired */
  COMPONENT_ACQ_NONZEROMASK(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, 
                            currAcquired);
  if (currAcquired == L7_TRUE)
  {
    return L7_FALSE;
  }

  /* If this port is enabled for routing, it can be configured but not enabled */
  if ((ipMapRtrIntfModeGet(intIfNum, &routingEnabled) == L7_SUCCESS) &&
      (routingEnabled == L7_ENABLE))
  {
     return L7_FALSE;
  }
   
  if (dot1sModeGet() == L7_TRUE)
  {  
    /* If this port is not in the forwarding state, it can be configured */
    /* but not enabled */
    mstID = dot1sVlanToMstiGet(vlanId);
    if (dot1sMstiPortStateGet(mstID, intIfNum) != L7_DOT1S_FORWARDING &&
        dot1sMstiPortStateGet(mstID, intIfNum) != L7_DOT1S_MANUAL_FWD)
    {
       return L7_FALSE;
    }
  }
  else /* dot1d Support */
  {
    /* If this port is not in the active state (up & forwarding), it can be */
    /* configured but not enabled */
    if ( (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) || 
         (activeState != L7_ACTIVE) )
      return L7_FALSE;
  }

  return L7_TRUE;
}

