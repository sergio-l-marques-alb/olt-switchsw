/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_pbvlan_api.c
*
* @purpose    Wrapper functions for Fastpath Protocol based VLAN
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     akulkarni
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpath_pbvlan_api.h"
#include "usmdb_pbvlan_api.h"
#include "usmdb_util_api.h"

/**************************************************************************************************************/

L7_RC_t
snmpAgentProtocolGroupCreateSet(L7_uint32 UnitIndex, L7_uint32 groupID)
{
  L7_RC_t rc = L7_FAILURE;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
       rc = usmDbPbVlanGroupCreate(UnitIndex, groupID);
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupVlanIdSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 vlanId;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      switch (val)
      {
      case 0:
        /* ignore if setting to 0 and is already set to 0 */
        if (usmDbPbVlanGroupVIDGet(UnitIndex, groupId, &vlanId) == L7_SUCCESS && vlanId != 0)
          rc = usmDbPbVlanGroupVlanIDDelete(UnitIndex, groupId, vlanId);
        break;

      default:
        rc = usmDbPbVlanGroupVlanIDAdd(UnitIndex, groupId, val);
        break;
      }
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupStatusSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      switch (val)
      {
      case D_agentProtocolGroupStatus_active:
        rc = L7_SUCCESS;
        break;

      case D_agentProtocolGroupStatus_createAndGo:
        rc = usmDbPbVlanGroupCreate(UnitIndex, groupId);        
        break;

      case D_agentProtocolGroupStatus_destroy:
        rc = usmDbPbVlanGroupDelete(UnitIndex, groupId);
        break;

      default:
        break;
      }
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupPortEntryGetExact(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 ifIndex)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      if (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) == L7_SUCCESS)
        rc = usmDbPbVlanGroupPortGetExact(UnitIndex, groupId, intIfNum);
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupPortEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *groupId, L7_uint32 *ifIndex)
{
  L7_uint32 intIfNum=0;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
  {
      return L7_FAILURE;
  }

  /* if this group doesn't exist, get the next valid group */
  if (usmDbPbVlanGroupGetExact(UnitIndex, *groupId) != L7_SUCCESS &&
      usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* if the requested external interface isn't visible or doesn't have an internal interface*/
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, *ifIndex) != L7_SUCCESS ||
      usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) != L7_SUCCESS)
  {
    /* find the next external interface */
    if (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) != L7_SUCCESS)
    {
      /* no more interface for this group */
      if (usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) != L7_SUCCESS)
        return L7_FAILURE;

      /* start at the begining of the group */
      *ifIndex = 0;
      intIfNum = 0;
    }
  }

  /* while the selected group is valid */
  while (usmDbPbVlanGroupGetExact(UnitIndex, *groupId) == L7_SUCCESS)
  {
    /* check to see if the current index is in the group */
    if (usmDbPbVlanGroupPortGetExact(UnitIndex, *groupId, intIfNum) != L7_SUCCESS)
    {
      /* find the next port in the group */
      if (usmDbPbVlanGroupPortGetNext(UnitIndex, *groupId, intIfNum, &intIfNum) != L7_SUCCESS)
      {
        /* no more ports in this group found, go on to next group */
        if (usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) == L7_SUCCESS)
        {
          intIfNum = 0;
          continue;
        }
        else
        {
          /* no more groups found */
          return L7_FAILURE;
        }
      }
    }

    /* we've found a port, convert to external*/
    return usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentProtocolGroupProtocolEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *groupId, L7_uint32 *proto)
{
  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
  {
      return L7_FAILURE;
  }
  
  /* if this group doesn't exist, get the next valid group */
  if (usmDbPbVlanGroupGetExact(UnitIndex, *groupId) != L7_SUCCESS) 
  {
    if (usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    else
    { 
      /*start from the first one*/
      *proto = 0; 
    }
  }
  
  /* while the selected group is valid */
  while (usmDbPbVlanGroupGetExact(UnitIndex, *groupId) == L7_SUCCESS)
  {
    /* find the next port in the group */
    if (usmDbPbVlanGroupProtocolGetNext(UnitIndex, *groupId, *proto, proto) != L7_SUCCESS)
    {
      /* no more ports in this group found, go on to next group */
      if (usmDbPbVlanGroupGetNext(UnitIndex, *groupId, groupId) == L7_SUCCESS)
      {
        *proto= 0;
        continue;
      }
      else
      {
        /* no more groups found */
        return L7_FAILURE;
      }
    }
    else
    {
       return L7_SUCCESS;
    } 
  }

  return L7_FAILURE;
}
