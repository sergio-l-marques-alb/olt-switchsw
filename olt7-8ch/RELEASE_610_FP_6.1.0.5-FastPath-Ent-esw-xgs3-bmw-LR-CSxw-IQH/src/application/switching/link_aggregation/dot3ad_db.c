
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot3ad_db.c
* @purpose     802.3ad link aggregation, agg, port, system retrival
* @component   dot3ad
* @comments    none
* @create      06/04/2001
* @author      djohnson
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <dot3ad_include.h>

dot3ad_agg_t dot3adAgg[L7_MAX_NUM_LAG_INTF];
dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];


L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1]; 
extern dot3ad_system_t dot3adSystem;
/*********************************************************************
* @purpose  Find an aggregator record based on an aggregator key
*
* @param    agg_key         aggregator key
* @param    *p              pointer to record
*
* @returns  L7_SUCCESS      if retrieved
* @returns  L7_FAILURE      if key not found
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_agg_t *dot3adAggKeyFind(L7_uint32 agg_key)
{
  L7_uint32 i;

  for (i=0;i<L7_MAX_NUM_LAG_INTF;i++)
  {
    if (dot3adAgg[i].actorAdminAggKey == agg_key)
      return &dot3adAgg[i];
  }
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Find an aggregator record based on an internal interface
*
* @param    agg_intf        internal interface
*
* @returns  L7_NULL         if interface not found
* @returns  dot3ad_agg_t pointer to record if found
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_agg_t *dot3adAggIntfFind(L7_uint32 agg_intf)
{
  L7_INTF_TYPES_t type;

  /* zero is used as a terminating integer in lists within dot3ad */
  if (agg_intf == 0) 
    return L7_NULLPTR;
  else if (nimGetIntfType(agg_intf, &type) != L7_SUCCESS)
    return L7_NULLPTR;
  else if (type != L7_LAG_INTF)
    return L7_NULLPTR;
  else
    return (&dot3adAgg[dot3adAggIdx[agg_intf]]);
}

/*********************************************************************
* @purpose  Find an aggregator record based on an aggregator port
*
* @param    agg_key         aggregator port
*
* @returns  L7_NULL         if port not a member of an aggregator
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_agg_t *dot3adAggPortFind(L7_uint32 agg_port)
{
  L7_uint32 agg_intf;

  agg_intf = dot3adPort[dot3adPortIdx[agg_port]].actorPortWaitSelectedAggId;
  return dot3adAggIntfFind(agg_intf);
}

/*********************************************************************
* @purpose  Find an port record based on a port key
*
* @param    port_key        port key
* @param    *p              pointer to record
*
* @returns  L7_SUCCESS      if retrieved
* @returns  L7_FAILURE      if key not found or not a member
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adPortKeyFind(L7_uint32 port_key)
{
  L7_uint32 i,max;

  max = L7_MAX_PORT_COUNT + 1;

  for (i=0;i < max;i++)
  {
    if (dot3adPort[i].actorAdminPortKey == port_key)
      return &dot3adPort[i];
  }
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Find a port record based on a port interface number
*
* @param    intf            aggregator key
* @param    *p              pointer to record
*
* @returns  L7_SUCCESS      if retrieved
* @returns  L7_FAILURE      if intf not a member of an aggregator
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adPortIntfFind(L7_uint32 intf)
{
  L7_INTF_TYPES_t type;

  if (intf == 0)
    return L7_NULLPTR;
  else if (nimGetIntfType(intf, &type) != L7_SUCCESS)
    return L7_NULLPTR;
  else if (type != L7_PHYSICAL_INTF)
    return L7_NULLPTR;
  else if (dot3adPortIdx[intf] == 0)
    return L7_NULLPTR;
  else
    return(&dot3adPort[dot3adPortIdx[intf]]);

}


/*********************************************************************
* @purpose  Get the first port attached to a particular aggregator
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggAttachedGetFirst(L7_uint32 agg_intf)
{
  L7_uint32 port;

  port = dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[0];

  if (port == 0) /* no attached ports */
  {
    return L7_NULLPTR;
  }

  else
  {
    return &dot3adPort[dot3adPortIdx[port]];
  }

}

/*********************************************************************
* @purpose  Given a first port attached to a particular aggregator
*           find the next
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggAttachedGetNext(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx = 0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[idx];

    if (port == 0)
      return L7_NULLPTR;

    if (port == intf)
    {
      idx++;
      if (idx < L7_MAX_MEMBERS_PER_LAG)
      {
        return dot3adPortIntfFind(dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[idx]);
      }
      else
        return L7_NULLPTR;
    }

    idx++;
  }

  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Add an interface to the attached list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggAttachedAdd(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx=0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[idx];

    if (port == intf) /*already in the attached list return success*/
      return L7_SUCCESS;
    else if (port != 0)
    {
      idx++; 
    }
    else
    {
      dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[idx] = intf;
      dot3adAgg[dot3adAggIdx[agg_intf]].currNumMembers++;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete an interface from the attached list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggAttachedDelete(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 i,j,port;
  L7_uint32 tmpList[L7_MAX_MEMBERS_PER_LAG];

  j=0;
  bzero((char*)&tmpList, (L7_int32)sizeof(tmpList));

  for (i=0;i<L7_MAX_MEMBERS_PER_LAG;i++)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[i];

    if (port != intf)     /* skip over the one to delete */
    {
      tmpList[j] = port;
      j++;
    }
  }

  if (i == j)
    return L7_FAILURE;    /* intf not found */


  bcopy((char*)&tmpList, 
        (char*)&dot3adAgg[dot3adAggIdx[agg_intf]].aggPortList[0],
        L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32));

  dot3adAgg[dot3adAggIdx[agg_intf]].currNumMembers--;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the first port selected to a particular aggregator
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggSelectedGetFirst(L7_uint32 agg_intf)
{
  L7_uint32 port;

  port = dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[0];

  if (port == 0) /* no attached ports */
  {
    return L7_NULLPTR;
  }

  else
  {
    return &dot3adPort[dot3adPortIdx[port]];
  }

}

/*********************************************************************
* @purpose  Given a first port selected by a particular aggregator
*           find the next
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggSelectedGetNext(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx = 0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[idx];

    if (port == 0)
      return L7_NULLPTR;

    if (port == intf)
    {
      idx++;
      if (idx < L7_MAX_MEMBERS_PER_LAG)
      {
        return dot3adPortIntfFind(dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[idx]);
      }
      else
        return L7_NULLPTR;
    }

    idx++;
  }

  return L7_NULLPTR;

}

/*********************************************************************
* @purpose  Add an interface to the selected list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added or if already present
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggSelectedAdd(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx=0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[idx];

    if (port == intf) /*already in the selected list return success*/
      return L7_SUCCESS;
    else if (port != 0)
    {
      idx++; 
    }
    else
    {
      dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[idx] = intf;
      dot3adAgg[dot3adAggIdx[agg_intf]].currNumSelectedMembers++;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete an interface from the selected list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggSelectedDelete(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 i,j,port;
  L7_uint32 tmpList[L7_MAX_MEMBERS_PER_LAG];

  j=0;
  bzero((char*)&tmpList, (L7_int32)sizeof(tmpList));

  /* build a new selected list minus intf */
  for (i=0;i<L7_MAX_MEMBERS_PER_LAG;i++)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[i];

    if (port != intf)     /* skip over the one to delete */
    {
      tmpList[j] = port;
      j++;
    }
  }

  if (i == j)
    return L7_FAILURE;    /* intf not found */


  bcopy((char*)&tmpList, 
        (char*)&dot3adAgg[dot3adAggIdx[agg_intf]].aggSelectedPortList[0],
        L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32));

  dot3adAgg[dot3adAggIdx[agg_intf]].currNumSelectedMembers--;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the first port waiting to be selected
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggWaitSelectedGetFirst(L7_uint32 agg_intf)
{
  L7_uint32 port;

  port = dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[0];

  if (port == 0) /* no attached ports */
  {
    return L7_NULLPTR;
  }

  else
  {
    return &dot3adPort[dot3adPortIdx[port]];
  }

}

/*********************************************************************
* @purpose  Given a port in the wait list, find the next
*
* @param    agg_intf        aggregator interface number
* @param    intf            starting intf
*
* @returns  dot3ad_port_t*  next port record, if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggWaitSelectedGetNext(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx = 0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[idx];

    if (port == 0)
      return L7_NULLPTR;

    if (port == intf)
    {
      idx++;
      if (idx < L7_MAX_MEMBERS_PER_LAG)
      {
        return dot3adPortIntfFind(dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[idx]);
      }
      else
        return L7_NULLPTR;
    }

    idx++;
  }

  return L7_NULLPTR;

}

/*********************************************************************
* @purpose  Add an interface to the wait list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added or if already present
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggWaitSelectedAdd(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx=0;
  dot3ad_usp_t memUsp;
  L7_RC_t rc;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[idx];

    if (port == intf) /*already in the wait list return success*/
      return L7_SUCCESS;
    else if (port != 0)
    {
      idx++; 
    }
    else
    {
      dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[idx] = intf;
      dot3adAgg[dot3adAggIdx[agg_intf]].currNumWaitSelectedMembers++;
      rc = dot3adLihIntfUspGet(intf,&memUsp);
      dot3adAgg[dot3adAggIdx[agg_intf]].aggPortListUsp[idx].unit = memUsp.unit;
      dot3adAgg[dot3adAggIdx[agg_intf]].aggPortListUsp[idx].slot = memUsp.slot;
      dot3adAgg[dot3adAggIdx[agg_intf]].aggPortListUsp[idx].port = memUsp.port;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete an interface from the wait list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggWaitSelectedDelete(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32     i,j,port;
  L7_uint32     tmpList[L7_MAX_MEMBERS_PER_LAG];
  dot3ad_usp_t  tmpListUsp[L7_MAX_MEMBERS_PER_LAG];
  L7_RC_t       rc;

  j=0;
  bzero((char*)&tmpList, (L7_int32)sizeof(tmpList));
  bzero((char*)&tmpListUsp, (L7_int32)sizeof(tmpListUsp));

  /* build a new selected list minus intf */
  for (i=0;i<L7_MAX_MEMBERS_PER_LAG;i++)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[i];

    if (port != intf)     /* skip over the one to delete */
    {
      tmpList[j] = port;
	  /* Unused members are stored as zeros, hence only get the usp 
	   * for non-zero port numbers. This will prevent unnecessary 
	   * LOG_MSGs and failures
	   */
	  if (port != 0)
	  {
	    rc = dot3adLihIntfUspGet(port,&tmpListUsp[j]);
	    if (rc == L7_FAILURE)
	    {
		  LOG_MSG("Interface to USP failure Port %u\n", port);
		  return L7_FAILURE;
	    }
	  }
      j++;
    }
  }

  if (i == j)
    return L7_FAILURE;    /* intf not found */


  bcopy((char*)&tmpList, 
        (char*)&dot3adAgg[dot3adAggIdx[agg_intf]].aggWaitSelectedPortList[0],
        L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32));
  bcopy((char*)&tmpListUsp,
		(char*)&dot3adAgg[dot3adAggIdx[agg_intf]].aggPortListUsp[0],
		(L7_int32)sizeof(tmpListUsp));

  dot3adAgg[dot3adAggIdx[agg_intf]].currNumWaitSelectedMembers--;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the first port actively participating in a particular aggregator
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggActivePortGetFirst(L7_uint32 agg_intf)
{
  L7_uint32 port;

  port = dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[0];

  if (port == 0) /* no attached ports */
  {
    return L7_NULLPTR;
  }

  else
  {
    return &dot3adPort[dot3adPortIdx[port]];
  }

}

/*********************************************************************
* @purpose  Given a first port actively participating in a particular aggregator
*           find the next
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggActivePortGetNext(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx = 0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[idx];

    if (port == 0)
      return L7_NULLPTR;

    if (port == intf)
    {
      idx++;
      if (idx < L7_MAX_MEMBERS_PER_LAG)
      {
        return dot3adPortIntfFind(dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[idx]);
      }
      else
        return L7_NULLPTR;
    }

    idx++;
  }

  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Add an interface to the actively participating list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggActivePortAdd(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 port;
  L7_uint32 idx=0;

  while (idx < L7_MAX_MEMBERS_PER_LAG)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[idx];

    if (port == intf) /*already in the attached list return success*/
      return L7_SUCCESS;
    else if (port != 0)
    {
      idx++; 
    }
    else
    {
      dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[idx] = intf;
      dot3adAgg[dot3adAggIdx[agg_intf]].activeNumMembers++;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete an interface from the actively participating list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggActivePortDelete(L7_uint32 agg_intf, L7_uint32 intf)
{
  L7_uint32 i,j,port;
  L7_uint32 tmpList[L7_MAX_MEMBERS_PER_LAG];

  j=0;
  bzero((char*)&tmpList, (L7_int32)sizeof(tmpList));

  for (i=0;i<L7_MAX_MEMBERS_PER_LAG;i++)
  {
    port = dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[i];

    if (port != intf)     /* skip over the one to delete */
    {
      tmpList[j] = port;
      j++;
    }
  }

  if (i == j)
    return L7_FAILURE;    /* intf not found */


  bcopy((char*)&tmpList, 
        (char*)&dot3adAgg[dot3adAggIdx[agg_intf]].aggActivePortList[0],
        L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32));

  dot3adAgg[dot3adAggIdx[agg_intf]].activeNumMembers--;

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Checks to see if a LAG interface is static or not
*
* @param    agg_intf      aggregator interface number
* @param    *isStatic     
*           
* @returns  L7_SUCCESS      
* @returns  L7_FAILURE      
*
* @notes    A LAG with no active members will have the static variale set to false
*       
* @end
*********************************************************************/
L7_RC_t dot3adIsStaticLag(L7_uint32 agg_intf, L7_BOOL *isStatic)
{
  dot3ad_agg_t *a;

  a = dot3adAggIntfFind(agg_intf);
  
  if (a == L7_NULLPTR)
  {
	return L7_FAILURE;
  }

  *isStatic = a->isStatic;

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Get the list of active member interfaces for the specified LAG interface.
*            
* @param    intIfNum       internal interface number of the target LAG
* @param    *pListCount    pointer to array element count
*                            (@b{Input:} max number of elements in caller's 
*                             memberList[] array)
*                            (@b{Output:} current number of LAG members)
* @param    memberList[]   array to be filled in with densely-packed list of
*                            active LAG member interface numbers
*                            (@b{Output:} LAG member intIfNum array)
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function outputs the current number of LAG members along
*           with a densely-packed array of LAG member internal interface
*           numbers.  Unused array entries are set to 0.
*
* @notes    To obtain all available LAG members, the caller should ensure the
*           memberList[] array contains at least L7_MAX_MEMBERS_PER_LAG entries.
*           This function limits itself to the smaller of L7_MAX_MEMBERS_PER_LAG
*           and the caller's initial *pListCount value.
*
* @notes    Only the active members
*           
*
* @end
*********************************************************************/
L7_RC_t dot3adActiveMemberListGet(L7_uint32 intIfNum, L7_uint32 *pListCount, 
                                  L7_uint32 memberList[])
{
  dot3ad_agg_t *a;
  L7_uint32 i;

  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i=0;i<a->activeNumMembers;i++)
  {
    /*return the added members in waiting to be selected list*/
    memberList[i] = a->aggActivePortList[i];
  }

  *pListCount = a->activeNumMembers; 

  return L7_SUCCESS;
}

