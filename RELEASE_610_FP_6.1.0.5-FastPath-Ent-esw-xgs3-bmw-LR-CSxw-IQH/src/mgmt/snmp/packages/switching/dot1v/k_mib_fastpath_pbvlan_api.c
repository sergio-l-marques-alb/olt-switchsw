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
snmpAgentProtocolGroupCreateSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  L7_RC_t rc = L7_FAILURE;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      if (usmDbStringAlphaNumericCheck(buf) == L7_SUCCESS)
        rc = usmDbPbVlanGroupCreate(UnitIndex, buf);
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
snmpAgentProtocolGroupProtocolIPGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      if (usmDbPbVlanGroupProtocolGet(UnitIndex, groupId, protocol_buffer) == L7_SUCCESS)
      {
        *val = D_agentProtocolGroupProtocolIP_disable;
        for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
        {
          if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_IP)
            *val = D_agentProtocolGroupProtocolIP_enable;
        }
        rc = L7_SUCCESS;
      }
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupProtocolIPSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempVal;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    rc = snmpAgentProtocolGroupProtocolIPGet(UnitIndex, groupId, &tempVal);
    if(rc == L7_SUCCESS && tempVal != val)
    {
      switch (val)
      {
        case D_agentProtocolGroupProtocolIP_enable:
          rc = usmDbPbVlanGroupProtocolAdd(UnitIndex, groupId, L7_USMDB_PROTOCOL_IP);
          break;

        case D_agentProtocolGroupProtocolIP_disable:
          rc = usmDbPbVlanGroupProtocolDelete(UnitIndex, groupId, L7_USMDB_PROTOCOL_IP);
          break;

        default:
          break;
      }
    }
  }
  return rc;
}

L7_RC_t
snmpAgentProtocolGroupProtocolARPGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      if (usmDbPbVlanGroupProtocolGet(UnitIndex, groupId, protocol_buffer) == L7_SUCCESS)
      {
        *val = D_agentProtocolGroupProtocolARP_disable;
        for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
        {
          if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_ARP)
            *val = D_agentProtocolGroupProtocolARP_enable;
        }
        rc = L7_SUCCESS;
      }
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupProtocolARPSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempVal;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    rc = snmpAgentProtocolGroupProtocolARPGet(UnitIndex, groupId, &tempVal);
    if(rc == L7_SUCCESS && tempVal != val)
    {
      switch (val)
      {
        case D_agentProtocolGroupProtocolARP_enable:
          rc = usmDbPbVlanGroupProtocolAdd(UnitIndex, groupId, L7_USMDB_PROTOCOL_ARP);
          break;

        case D_agentProtocolGroupProtocolARP_disable:
          rc = usmDbPbVlanGroupProtocolDelete(UnitIndex, groupId, L7_USMDB_PROTOCOL_ARP);
          break;

        default:
          break;
      }
    }
  }
  return rc;
}

L7_RC_t
snmpAgentProtocolGroupProtocolIPXGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
      if (usmDbPbVlanGroupProtocolGet(UnitIndex, groupId, protocol_buffer) == L7_SUCCESS)
      {
        *val = D_agentProtocolGroupProtocolIPX_disable;
        for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
        {
          if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_IPX)
            *val = D_agentProtocolGroupProtocolIPX_enable;
        }
        rc = L7_SUCCESS;
      }
  }

  return rc;
}

L7_RC_t
snmpAgentProtocolGroupProtocolIPXSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempVal;

  if (usmDbComponentPresentCheck(UnitIndex, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    rc = snmpAgentProtocolGroupProtocolIPXGet(UnitIndex, groupId, &tempVal);
    if(rc == L7_SUCCESS && tempVal != val)
    {
      switch (val)
      {
        case D_agentProtocolGroupProtocolIPX_enable:
          rc = usmDbPbVlanGroupProtocolAdd(UnitIndex, groupId, L7_USMDB_PROTOCOL_IPX);
          break;

        case D_agentProtocolGroupProtocolIPX_disable:
          rc = usmDbPbVlanGroupProtocolDelete(UnitIndex, groupId, L7_USMDB_PROTOCOL_IPX);
          break;

        default:
          break;
      }
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


