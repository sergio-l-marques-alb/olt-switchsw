/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_virtif_api.c
*
* @purpose   Ospf Virtual Interface Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for ospf variables, 
*             which are specific to the Virtual Interface table
*
* @create    07/10/2001
*
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_ospfinclude.h"
#include "l7_ospf_api.h"

extern L7_ospfMapCfg_t    *pOspfMapCfgData;
extern ospfVlinkInfo_t    *pOspfVlinkInfo;
                    
/*
***********************************************************************
*                     API FUNCTIONS  -  VIRT IF CONFIG
***********************************************************************
*/

/*********************************************************************
* @purpose  Get a single Virtual Interface entry information.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Information about a single Virtual interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfEntryGet ( L7_uint32 AreaId, L7_uint32 Neighbor )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtIfEntryGet(AreaId,Neighbor);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Virtual Interface entry information.
*
* @param    intIfNum    internal interface number
* @param    *AreaId     Id
* @param    *Neighbor   entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Information about a single Virtual interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfEntryNext ( L7_uint32 *AreaId, L7_uint32 *Neighbor )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenVirtIfEntryNext(AreaId,Neighbor);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link-state 
*           update packet over this interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The estimated number of seconds it takes to transmit 
*             a link-state update packet over this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfTransitDelayGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                       L7_int32 *val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      *val = pOspfMapCfgData->virtNbrs[i].ifTransitDelay;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tests if the number of seconds it takes to transmit a link-state 
*           update packet over this interface is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The estimated number of seconds it takes to transmit 
*             a link-state update packet over this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfTransitDelaySetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                           L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfTransitDelaySetTest(AreaId, Neighbor, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the number of seconds it takes to transmit a link-state 
*           update packet over this interface.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The estimated number of seconds it takes to transmit 
*             a link-state update packet over this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfTransitDelaySet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                       L7_int32 val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      pOspfMapCfgData->virtNbrs[i].ifTransitDelay = val;
      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfTransitDelaySet(i, val);

      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.   
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets.
*             This value should be well over the expected routnd
*             trip time."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfRetransIntervalGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 *val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      *val = pOspfMapCfgData->virtNbrs[i].rxmtInterval;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tests if the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets.
*             This value should be well over the expected routnd
*             trip time."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfRetransIntervalSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                              L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfRetransIntervalSetTest(AreaId, Neighbor, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.   
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets.
*             This value should be well over the expected routnd
*             trip time."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfRetransIntervalSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      pOspfMapCfgData->virtNbrs[i].rxmtInterval = val;
      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfRetransIntervalSet(i,val);

      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the time, in seconds, between the Hello packets that 
*           the router sends on the interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time, in seconds, between the Hello
*             packets that the router sends on the interface. 
*             This value must be the same for the virtual
*             neighbor." 
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfHelloIntervalGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_int32 *val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      *val = pOspfMapCfgData->virtNbrs[i].helloInterval;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tests if the time, in seconds, between the Hello packets that 
*           the router sends on the interface is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time, in seconds, between the Hello
*             packets that the router sends on the interface. 
*             This value must be the same for the virtual
*             neighbor." 
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfHelloIntervalSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                            L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfHelloIntervalSetTest(AreaId, Neighbor, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the time, in seconds, between the Hello packets that 
*           the router sends on the interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time, in seconds, between the Hello
*             packets that the router sends on the interface. 
*             This value must be the same for the virtual
*             neighbor." 
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfHelloIntervalSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_int32 val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      pOspfMapCfgData->virtNbrs[i].helloInterval = val;
      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfHelloIntervalSet(i, val);

      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of
*             the Hello interval.  This value must be the same
*             for the virtual neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfRtrDeadIntervalGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 *val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      *val = pOspfMapCfgData->virtNbrs[i].deadInterval;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tests if the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down
*           is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of
*             the Hello interval.  This value must be the same
*             for the virtual neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfRtrDeadIntervalSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                              L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfRtrDeadIntervalSetTest(AreaId, Neighbor, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of
*             the Hello interval.  This value must be the same
*             for the virtual neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfRtrDeadIntervalSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  if ((val<L7_OSPF_INTF_DEAD_MIN) ||(val>L7_OSPF_INTF_DEAD_MAX))
  {
    return L7_FAILURE;
  }
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      pOspfMapCfgData->virtNbrs[i].deadInterval = val;
      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfRtrDeadIntervalSet(i, val);

      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the OSPF virtual interface state.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        interface state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    "OSPF Virtual interface states:
*               down (1)          These use the same coding
*               pointToPoint (4)  as the ospfIfTable "
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfStateGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtIfStateGet(AreaId,Neighbor,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF virtual interface metric. This is the sum of
*           the OSPF metrics of the links in the path to the virtual 
*           neighbor.
*
* @param    intIfNum    internal interface number
* @param    areaId      The OSPF area the virtual link traverses
* @param    neighbor    The router ID of the virtual neighbor
* @param    *metric     virtual link metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfMetricGet ( L7_uint32 areaId, L7_uint32 neighbor,
                                L7_uint32 *metric )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
  {
      L7_uint32 i;
      for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
      {
          if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == neighbor &&
             pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == areaId)      
          {
              if (pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
              {
                  void *IFO_Id = pOspfVlinkInfo[i].IFO_Id;
                  return ospfMapExtenVirtIfMetricGet(IFO_Id, metric);
              }
          }
      }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of state changes or error events on this Virtual Link.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of state changes or error events on this
*             virtual link."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfEventsGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                 L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtIfEventsGet(AreaId,Neighbor,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of state changes or error events on this
* @purpose  interface for all virtual links
*
* @param    intIfNum	The internal interface number
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of state changes or error events on this
*             virtual link."
*
* @end
*********************************************************************/
L7_RC_t ospfMapRtrIntfAllVirtIfEventsGet ( L7_uint32 intIfNum, L7_int32 *val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 areaId;
  L7_uint32 nbrIpAddr;
  L7_uint32 intNum;
  L7_uint32 nbrRtrId;
  L7_int32 val1;
  L7_RC_t rc1;

  if (ospfMapOspfInitialized() != L7_TRUE) 
    return rc;
  rc = ospfMapIntfEffectiveAreaIdGet(intIfNum, &areaId);
  if (rc != L7_SUCCESS)
  {
	return L7_FAILURE;
  }
	
  *val = 0;
  nbrIpAddr = 0;
  intNum = 0;
  rc = ospfMapExtenNbrEntryGet(nbrIpAddr,intNum);
  if (rc != L7_SUCCESS)
  {
	rc = ospfMapExtenNbrEntryNext(&nbrIpAddr,&intNum);
  }
  while (rc == L7_SUCCESS)
  {
	if (intNum == intIfNum)
	{
	  rc1 = ospfMapExtenNbrRouterIdGet(intNum,nbrIpAddr, &nbrRtrId);
	  if (rc1 == L7_SUCCESS)
	  {
		rc1 = ospfMapExtenVirtIfEventsGet(areaId,nbrRtrId, &val1);
		if (rc1 == L7_SUCCESS)
		{
		  *val = *val + val1;
		}
	  }
	}
	rc = ospfMapExtenNbrEntryNext(&nbrIpAddr,&intNum);
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperative
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfStatusGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                 L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtIfStatusGet(AreaId,Neighbor,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the entry status is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperative
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfStatusSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                     L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfStatusSetTest(AreaId, Neighbor, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperative
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfStatusSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                 L7_int32 val )
{
  L7_uint32 i;

  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    /* Find the entry in the configuration */
    for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
    {
      if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
         pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
      {
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfStatusSet(i, val);

        break;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the authentication type specified for a virtual interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally.
*
*             Authentication Types:
*               none (0)
*               simplePassword (1)
*               md5 (2)
*               reserved for specification by IANA (>2) " 
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthTypeGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                   L7_int32 *val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      *val = pOspfMapCfgData->virtNbrs[i].authType;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the authentication type is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally.
*
*             Authentication Types:
*               none (0)
*               simplePassword (1)
*               md5 (2)
*               reserved for specification by IANA (>2) " 
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthTypeSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                       L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfAuthTypeSetTest(AreaId, Neighbor, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the authentication type.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally.
*
*             Authentication Types:
*               none (0)
*               simplePassword (1)
*               md5 (2)
*               reserved for specification by IANA (>2) " 
*
* @notes    For security reasons, the authentication key is always
*           cleared whenever the auth type is changed.  If setting a 
*           new key value, be sure to do it AFTER setting the auth type. 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthTypeSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                   L7_int32 val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      if (pOspfMapCfgData->virtNbrs[i].authType == val)
        return L7_SUCCESS;

      /* always clear the key when the auth type changes
       *
       * NOTE: This is to reduce the possibility of transmitting an 
       *       MD5 secret key in the clear when the auth type changes
       *       from MD5 to Simple.
       */
      memset(pOspfMapCfgData->virtNbrs[i].authKey, 0, (size_t)L7_AUTH_MAX_KEY_OSPF);
      pOspfMapCfgData->virtNbrs[i].authKeyLen = 0;

      pOspfMapCfgData->virtNbrs[i].authType = val;
      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfAuthTypeSet(i, val);

      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Authentication Key.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The key value is output as an ASCIIZ string of whose length
*           is that of the key without any padding.
*
* @notes    This function always outputs the current key value, despite
*           instructions to the contrary specified in the MIB.  The USMDB
*           layer should not call this function if it wants to keep the
*           key value hidden.
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                  L7_uchar8 *buf )
{
  L7_uint32 i;
  L7_uint32 keyLen;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      keyLen = pOspfMapCfgData->virtNbrs[i].authKeyLen;
      memcpy(buf, pOspfMapCfgData->virtNbrs[i].authKey, keyLen);
      /* terminate the key value with a '\0' to make it an ASCIIZ string */
      *(buf+keyLen) = L7_EOS;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tests if the Authentication Key is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Authentication Key.  If the Area's Authorization
*             Type is simplePassword, and the key length is
*             shorter than 8 octets, the agent will left adjust
*             and zero fill to 8 octets.
*             
*             Note that unauthenticated interfaces need no 
*             authentication key, and simple password authentication
*             cannot use a key of more than 8 octets.  Larger keys
*             are useful only with authetication mechanisms not
*             specified in this document.
*
*             When read, ospfIfAuthKey always returns an Octet
*             String of length zero.
*   
*             Default value: 0.0.0.0.0.0.0.0 "
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeySetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                      L7_uchar8 *buf )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenVirtIfAuthKeySetTest(AreaId, Neighbor, buf);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the Authentication Key.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Authentication Key.  If the Area's Authorization
*             Type is simplePassword, and the key length is
*             shorter than 8 octets, the agent will left adjust
*             and zero fill to 8 octets.
*             
*             Note that unauthenticated interfaces need no 
*             authentication key, and simple password authentication
*             cannot use a key of more than 8 octets.  Larger keys
*             are useful only with authetication mechanisms not
*             specified in this document.
*
*             When read, ospfIfAuthKey always returns an Octet
*             String of length zero.
*   
*             Default value: 0.0.0.0.0.0.0.0 "
*
* @notes    The authentication key is passed in here as an ASCII string 
*           for the convenience of the user interface functions.  The MIB
*           key definition, however, is an OCTET STRING, which means it
*           is treated as a byte array, not an ASCII string.  The
*           conversion is done here for use in lower layer code.
*
* @notes    The key type is based on the current value of the authType 
*           code.  This implies that the authType field must be set
*           before the authKey.
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeySet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                  L7_uchar8 *buf )
{
  L7_uint32 i;
  L7_uint32 keyLen, authType, authLenMax;
  L7_char8  newKey[L7_AUTH_MAX_KEY_OSPF];
  L7_char8  *pCfgKey;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      /* Convert incoming key string to a left-justified array, padded with hex zeroes */
      if(buf != L7_NULLPTR)
        keyLen = strlen(buf);
      else
        keyLen = 0;
      
      /* use current auth type from the configuration */
      authType = pOspfMapCfgData->virtNbrs[i].authType;

      /* Maximum allowed key length depends on current authType value */
      switch (authType)
      {
      case L7_AUTH_TYPE_NONE:
        authLenMax = 0;
        break;

      case L7_AUTH_TYPE_SIMPLE_PASSWORD:
        authLenMax = L7_AUTH_MAX_KEY_OSPF_SIMPLE;
        break;

      case L7_AUTH_TYPE_MD5:
        authLenMax = L7_AUTH_MAX_KEY_OSPF_MD5;
        break;

      default:
        return L7_FAILURE;
        break;
      }

      /* The only time keyLen is greater than the max is if the caller 
       * specified a key value that is longer than the allowed maximum
       * for the current auth type -- reject it
       */
      if (keyLen > authLenMax)
        return L7_FAILURE;

      /* build a left-adjusted, padded new key to compare against current key */
      memset(&newKey, 0, (size_t)L7_AUTH_MAX_KEY_OSPF);
      if (keyLen > 0)
        memcpy(&newKey, buf, (size_t)keyLen);

      /* if incoming key is same as configured key, just return successfully */
      pCfgKey = pOspfMapCfgData->virtNbrs[i].authKey;
      if (memcmp(pCfgKey, &newKey, (size_t)L7_AUTH_MAX_KEY_OSPF) == 0)
        return L7_SUCCESS;

      /* update config struct with latest key and length */
      memcpy(pCfgKey, newKey, (size_t)L7_AUTH_MAX_KEY_OSPF);
      pOspfMapCfgData->virtNbrs[i].authKeyLen = keyLen;

      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfAuthKeySet(i, pCfgKey);

      break;                            /* exit 'for' loop */
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the authentication key id specified for a virtual interface.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyIdGet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                    L7_uint32 *val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      *val = pOspfMapCfgData->virtNbrs[i].authKeyId;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the authentication key id is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyIdSetTest ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_uint32 val )
{
  L7_uint32 i;

  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    /* Find the entry in the configuration */
    for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
    {
      if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
         pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
      {
        if ((val < L7_OSPF_AUTH_KEY_ID_MIN) ||
            (val > L7_OSPF_AUTH_KEY_ID_MAX))
          return L7_FAILURE;

        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfAuthKeyIdSetTest(i, val);

        break;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the authentication key id.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfAuthKeyIdSet ( L7_uint32 AreaId, L7_uint32 Neighbor,
                                    L7_uint32 val )
{
  L7_uint32 i;

  /* Find the entry in the configuration */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == Neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == AreaId)      
    {
      if ((val < L7_OSPF_AUTH_KEY_ID_MIN) ||
          (val > L7_OSPF_AUTH_KEY_ID_MAX))
        return L7_FAILURE;

      if (pOspfMapCfgData->virtNbrs[i].authKeyId == val)
        return L7_SUCCESS;

      pOspfMapCfgData->virtNbrs[i].authKeyId = val;
      ospfDataChangedSet(__FUNCTION__);

      if (ospfMapOspfInitialized() == L7_TRUE)
        if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
          return ospfMapExtenVirtIfAuthKeyIdSet(i, val);

      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a virtual link to the specified neighbor across
*           the given transit area.
*
* @param    areaId      transit area
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_ERROR    if the remote router-id = our router-id
* @returns  L7_REQUEST_DENIED  if transit area is stub or nssa area
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfCreate (L7_uint32 areaId, L7_uint32 neighbor)
{
  L7_uint32 i, index = 0, areaIndex;
  L7_BOOL   flag = L7_FALSE;

  /* A virtual link cannot be configured across the backbone area */
  if(areaId == L7_OSPF_AREA_BACKBONE)
    return L7_FAILURE;

  /* A virtual link cannot be configured with neighbor's router-id
   * as my router-id */
  if(neighbor == pOspfMapCfgData->rtr.routerID)
    return L7_ERROR;

  /* Transit area can't be a stub area or nssa area */
  if(ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS)
  {
    if((pOspfMapCfgData->area[areaIndex].extRoutingCapability ==
                                        L7_OSPF_AREA_IMPORT_NSSA) ||
       (pOspfMapCfgData->area[areaIndex].extRoutingCapability ==
                                        L7_OSPF_AREA_IMPORT_NO_EXT))
      return L7_REQUEST_DENIED;
  }

  /* First save the configuration */

  /* Check if the entry already exists */
  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == areaId)      
    {
      index = i;
      flag = L7_TRUE;

      /* If the virtual link has already been created previously exit
         gracefully */
      if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
        return L7_SUCCESS;

      break;
    }
  }

  /* Find the next available index */
  if(flag == L7_FALSE)
  {
    for (index = 0;  index < L7_OSPF_MAX_VIRT_NBRS; index++)
    {
      if (pOspfMapCfgData->virtNbrs[index].virtIntfNeighbor == L7_NULL_IP_ADDR)
      {
        break;
      }
    }

    if(index < L7_OSPF_MAX_VIRT_NBRS)
    {
      /* Try to find the index for the specified area, if it is not found
      ** create a new area since this is a read-create parm
      */
      if((ospfMapAreaIndexGet(areaId, &areaIndex) != L7_SUCCESS) && 
         (ospfMapAreaCreate(areaId, &areaIndex) != L7_SUCCESS))
      {
        return L7_FAILURE;
      }

      bzero((L7_char8 *)&pOspfMapCfgData->virtNbrs[index], sizeof(ospfVirtNbrCfg_t));
      pOspfMapCfgData->virtNbrs[index].virtIntfNeighbor = neighbor;
      pOspfMapCfgData->virtNbrs[index].virtTransitAreaID = areaId;
      pOspfMapCfgData->virtNbrs[index].deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
      pOspfMapCfgData->virtNbrs[index].helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
      pOspfMapCfgData->virtNbrs[index].ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
      pOspfMapCfgData->virtNbrs[index].rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
      ospfDataChangedSet(__FUNCTION__);
    }
    /* We have run out of virtual neighbor entries */
    else
      return L7_FAILURE;
  }

  /* If we reached here we have an entry in the configuration & do not have a virtual
     link for the specified neighbor, transit area tuple */
  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    if (ospfMapOspfVirtIntfCreate(index, areaId, neighbor) == L7_SUCCESS)
    {
      pOspfMapCfgData->virtNbrs[index].intIfNum = (L7_MAX_INTERFACE_COUNT + index);
      pOspfVlinkInfo[index].vlinkCreated = L7_TRUE;
      pOspfVlinkInfo[index].areaId = areaId;
      pOspfVlinkInfo[index].neighbor = neighbor;
      return L7_SUCCESS;
    }
  }

  /* OSPF has not yet been initialized so save the configuration and return successfully
     The changes will be applied when OSPF is initialized */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a virtual link to the specified neighbor across
*           the given transit area.
*
* @param    areaId      transit area
* @param    neighbor    remote router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtIfDelete (L7_uint32 areaId, L7_uint32 neighbor)
{
  L7_uint32 i;

  for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    if(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor == neighbor &&
       pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == areaId)      
    {
      /* Remove the previous configuration */
      bzero((L7_char8 *)&pOspfMapCfgData->virtNbrs[i], sizeof(ospfVirtNbrCfg_t));
      ospfDataChangedSet(__FUNCTION__);

      /* delete the virtual link if it exists */
      if(pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
      {
        /* Issue an virtual interface down event */
        ospfMapOspfVirtIntfDown(i);

        if(ospfMapOspfVirtIntfDelete(i, areaId, neighbor) == L7_SUCCESS)
        {
          /* remove all virtual link information from the configuration */
          bzero((L7_char8 *)&pOspfVlinkInfo[i], sizeof(ospfVlinkInfo_t));
          break;
        }
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if a virtual link has been configured across the
*           specified area.
*
* @param    areaId      transit area
*
* @returns  L7_TRUE   if a virtual link has been configured across the
*                     specified area
* @returns  L7_FALSE  if a virtual link has not been configured across
*                     the specified area
*
* @notes    none.
*
* @end
*********************************************************************/
L7_BOOL ospfMapVirtIfCheck (L7_uint32 areaId)
{
  L7_uint32 i;

  if (areaId != 0)   /* BB can't be a transit area */
  {
    for(i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
      if (pOspfMapCfgData->virtNbrs[i].virtTransitAreaID == areaId) 
        return L7_TRUE;
  }

  return L7_FALSE;
}

