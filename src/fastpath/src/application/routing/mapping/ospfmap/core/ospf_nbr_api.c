/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_nbr_api.c
*
* @purpose   Ospf Neighbor Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for ospf variables, 
*             which are specific to the Neighbor table
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
                    
/*
***********************************************************************
*                     API FUNCTIONS  -  NEIGHBOR CONFIG
***********************************************************************
*/

/*---------------------------------------------------------------------*/
/* OSPF Neighbor Table                                                 */
/* The OSPF Neighbor Table describes all neighbors in the locality of  */
/* the subject router.                                                 */
/*---------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get a single neighbor entry information.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The information regarding a single neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbrEntryGet ( L7_uint32 IpAddr, L7_uint32 intIfNum )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrEntryGet(IpAddr,intIfNum);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next neighbor entry information.
*
* @param    *IpAddr             Ip address
* @param    *intIfNum           internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The information regarding a single neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbrEntryNext ( L7_uint32 *IpAddr, L7_uint32 *intIfNum )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrEntryNext(IpAddr,intIfNum);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf IP Address of the specified Neighbor router Id
*           of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 IP Address of the Neighbor
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The IP Address this neighbor is using in its IP Source
*             Address.  Note that, on addressless links, this will
*             not be 0.0.0.0, but the address of another of the 
*             neighbor's interfaces."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNeighborIPAddrGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val)
{
  L7_uint32 rtrIfNum;
  L7_NIM_QUERY_DATA_t QueryData;

  QueryData.intIfNum = intIfNum;
  QueryData.request = L7_NIM_QRY_RQST_PRESENT;

  if (nimIntfQuery(&QueryData) != L7_SUCCESS)
    return L7_ERROR;

  if (QueryData.data.present == L7_TRUE)
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        return ospfMapExtenNbrIPAddrGet(intIfNum, routerId, val);
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Get the Ospf Interface index of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 interface index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " On an interface having an IP Address, zero.  On
*             addressless interfaces, the corresponding value of 
*             ifIndex in the Internet Standard MIB.  On row
*             creation, this can be derived from the instance."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNeighborIfIndexGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val)
{
  L7_uint32 rtrIfNum;
  L7_NIM_QUERY_DATA_t QueryData;

  QueryData.intIfNum = intIfNum;
  QueryData.request = L7_NIM_QRY_RQST_PRESENT;

  if (nimIntfQuery(&QueryData) != L7_SUCCESS)
    return L7_ERROR;

  if (QueryData.data.present == L7_TRUE)
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        return ospfMapExtenNbrIfIndexGet(intIfNum, routerId, val);
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Get the Ospf Router Id for the first Neighbor of the 
*           specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    ipAddress		 L7_uint32 ipAddress of the neighbor               
* @param    *p_routerId      L7_uint32 router Id of the Neighbor
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A 32-bit integer (represented as a type IpAddress)
*             uniquely identifying the neighboring router in the
*             Autonomous System.
*             Default Value: 0.0.0.0 "
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrRouterIdGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *p_routerId)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrRouterIdGet(intIfNum, ipAddress, p_routerId);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Router Id for the next Neighbor, after the
*           specified router Id of the Neighbor, of the 
*           specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *p_routerId      L7_uint32 next router Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_ERROR    if Error  
*
* @notes    " A 32-bit integer (represented as a type IpAddress)
*             uniquely identifying the neighboring router in the
*             Autonomous System.
*             Default Value: 0.0.0.0 "
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNeighborRouterIdGetNext(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *p_routerId)
{
  L7_uint32 rtrIfNum;
  L7_NIM_QUERY_DATA_t QueryData;

  QueryData.intIfNum = intIfNum;
  QueryData.request = L7_NIM_QRY_RQST_PRESENT;

  if (nimIntfQuery(&QueryData) != L7_SUCCESS)
    return L7_ERROR;

  if (QueryData.data.present == L7_TRUE)
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        return ospfMapExtenNbrRouterIdGetNext(intIfNum, routerId, p_routerId);
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Get the Ospf Options of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 options
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A Bit Mask corresponding to the neighbor's options
*             field.  
*
*             Bit 0, if set, indicates that the system
*             will operate on Type of Service metrics other than
*             TOS 0.  If zero, the neighbor will ignore all metrics
*             except the TOS 0 metric.
*             
*             Bit 1, if set, indicates that the associated area
*             accepts and operates on external information; if zero
*             it is a stub area.
*             
*             Bit 2, if set, indicates that the system is capable
*             of routing IP Multicast datagrams; i.e., that it
*             implements the Multicast Extensions to OSPF.
*
*             Bit 3, if set, indicates that the associated area
*             is an NSSA.  These areas are capable of carrying type
*             7 external advertisements, which are translated into
*             type 5 external advertisements at NSSA borders.
*
*             Default Value: 0 "
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrOptionsGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbrOptionsGet(intIfNum, ipAddress, val);
    return rc;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Area of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 area
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrAreaGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrAreaGet(intIfNum, ipAddress, val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf dead timer remaining of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 time in secs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrDeadTimerRemainingGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrDeadTimerRemainingGet(intIfNum, ipAddress, val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf dead timer remaining of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 time in secs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrUptimeGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrUptimeGet(intIfNum, routerId, val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Priority of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The priority of this neighbor in the designated router
*             election algorithm.  The value 0 signifies that the 
*             neighbor is not eligible to become the designated
*             router on this particular network."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrPriorityGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbrPriorityGet(intIfNum, ipAddress, val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the priority of this neighbor.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The priority of this neighbor in the designated router
*             election algorithm.  The value 0 signifies that the 
*             neighbor is not eligible to become the designated
*             router on this particular network."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbrPrioritySetTest ( L7_uint32 IpAddr,
                                L7_uint32 intIfNum, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    ospfMapExtenNbrPrioritySetTest(IpAddr,intIfNum,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the priority of this neighbor.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The priority of this neighbor in the designated router
*             election algorithm.  The value 0 signifies that the 
*             neighbor is not eligible to become the designated
*             router on this particular network."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbrPrioritySet ( L7_uint32 IpAddr,
                                L7_uint32 intIfNum, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    ospfMapExtenNbrPrioritySet(IpAddr,intIfNum,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf State of the specified Neighbor router Id of 
*           the specified interface
* 
* @param    intIfNum  L7_uint32 interface number
* @param    routerId  L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The State of the relationship with this Neighbor."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrStateGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbrStateGet(intIfNum, ipAddress, val);
    return rc;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Interface State of the specified Neighbor
*           router-Id of the specified interface
* 
* @param    intIfNum  L7_uint32 interface number
* @param    routerId  L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The State of the relationship with this Neighbor."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrIntfStateGet(L7_uint32 intIfNum, 
                                 L7_uint32 routerId, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbrIntfStateGet(intIfNum, routerId, val);
    return rc;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Events counter of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       	L7_uint32 interface number
* @param    ipAddress		ip address of the neighbour
* @param    *val      L7_uint32 events counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of times this neighbor relationship has
*             changed state, or an error has occurred."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrEventsCounterGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbrEventsCounterGet(intIfNum, ipAddress, val);
    return rc;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Events counter of all Neighbor on a specified
*           interface
* 
* @param    intIfNum       	L7_uint32 interface number
* @param    *val      L7_uint32 events counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of times this neighbor relationship has
*             changed state, or an error has occurred."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapRtrIntfAllNbrEventsCounterGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intNum, nbrIpAddr;
  L7_RC_t rc1;
  L7_uint32 val1;

  if (ospfMapOspfInitialized() != L7_TRUE) 
  {
    return rc;
  }
  intNum = 0;
  nbrIpAddr = 0;
  *val = 0;
  rc = ospfMapExtenNbrEntryGet(nbrIpAddr,intNum);
  if (rc == L7_FAILURE)
  {
	rc = ospfMapExtenNbrEntryNext(&nbrIpAddr,&intNum);
  }
  while (rc == L7_SUCCESS)
  {
	if (intNum == intIfNum)
	{
	  rc1 = ospfMapExtenNbrEventsCounterGet(intNum,nbrIpAddr,&val1);
	  if (rc1 == L7_SUCCESS)
	  {
		*val = *val + val1;
	  }
	}
	rc = ospfMapExtenNbrEntryNext(&nbrIpAddr,&intNum);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the current length of the retransmission queue of the 
*           specified Neighbor router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 LS Retransmission Queue Length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The current length of the retransmission queue."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrLSRetransQLenGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbrLSRetransQLenGet(intIfNum, ipAddress, val);
    return rc;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry. Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbmaNbrStatusGet ( L7_uint32 IpAddr,
                                  L7_uint32 intIfNum, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbmaNbrStatusGet(IpAddr,intIfNum,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface Number
* @param    val                 entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry. Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbmaNbrStatusSetTest ( L7_uint32 IpAddr,L7_uint32 intIfNum, 
								  L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbmaNbrStatusSetTest(IpAddr,intIfNum,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface Number
* @param    val                 entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry. Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapNbmaNbrStatusSet ( L7_uint32 IpAddr,L7_uint32 intIfNum, 
								  L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenNbmaNbrStatusSet(IpAddr,intIfNum,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Permanence value of the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 permanence value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry. 
*             'dynamic' and 'permanent' refer to how the 
*             neighbor became known."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbmaNbrPermanenceGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbmaNbrPermanenceGet( ipAddress, intIfNum, val);
    return rc;
  }
  return L7_FAILURE;

}

/*********************************************************************
* 
* @purpose  Check if Ospf Hellos are suppressed to the specified Neighbor 
*           router Id of the specified interface
* 
* @param    intIfNum       L7_uint32 interface number
* @param    routerId L7_uint32 the specified router Id of the Neighbor
* @param    *val      L7_uint32 Hellos Suppressed ? L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Indicates whether Hellos are being suppressed to the
*             neighbor."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapNbrHellosSuppressedGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized()) 
  {
    rc = ospfMapExtenNbrHellosSuppressedGet(intIfNum, ipAddress, val);
    return rc;
  }
  return L7_FAILURE;

}


/*********************************************************************
* @purpose  Clear OSPF neighbors.
*
* @param    intIfNum - If non-zero, restrict clear to neighbors on this interface.
* @param    routerId   If non-zero, restrict clear to neighbors with this router ID. 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_NOT_EXIST if OSPF not initialized
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNeighborClear(L7_uint32 intIfNum, L7_uint32 routerId)
{
  if (!ospfMapOspfInitialized())
  {
    return L7_NOT_EXIST;
  }
  return ospfMapExtenNeighborClear(intIfNum, routerId);
}


/*
***********************************************************************
*                     API FUNCTIONS  -  VIRTUAL NEIGHBOR CONFIG
***********************************************************************
*/

/*----------------------------------------------------------------------*/
/* OSPF Virtual Neighbor Table                                          */
/* This table describes all virtual neighbors. Since Virtual Links are  */
/* configured in the virtual interface table, this table is read-only   */
/*----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get the Virtual neighbor entry information.
*
* @param    Area        area id
* @param    RtrId       router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Virtual Neighbor Information."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrEntryGet ( L7_uint32 Area, L7_uint32 RtrId )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrEntryGet(Area,RtrId);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Virtual neighbor next entry information.
*
* @param    *Area       area id
* @param    *RtrId      router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Virtual Neighbor Information."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrEntryNext ( L7_uint32 *Area, L7_uint32 *RtrId )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrEntryNext(Area,RtrId);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the IP address this Virtual Neighbor is using.
*
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        Ip address         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The IP Address this Virtual Neighbor is using."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrIpAddrGet ( L7_uint32 Area,
                                  L7_uint32 RtrId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrIpAddrGet(Area,RtrId,val);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get a Bit Mask corresponding to the neighbor's options field.
*
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        bit mask         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A Bit Mask corresponding to the neighbor's options field
*
*             Bit 1, if set, indicates that the system will operate
*             on Type of Service metrics other than TOS 0.  If zero,
*             the neighbor will ignore all metrics except the TOS
*             0 metric.
*
*             Bit 2, if set, indicates that the system is Network
*             Multicast capable; i.e., that it implements OSPF
*             Multicast Routing."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrOptionsGet ( L7_uint32 Area,
                                   L7_uint32 RtrId, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrOptionsGet(Area,RtrId,val);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Virtual Neighbor Relationship state.
*
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The state of the Virtual Neighbor relationship.
*
*             Types of states:
*               down (1)
*               attempt (2)
*               init (3)
*               twoWay (4)
*               exchangeStart (5)
*               exchange (6)
*               loading (7)
*               full (8) "
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrStateGet ( L7_uint32 Area,
                                 L7_uint32 RtrId, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrStateGet(Area,RtrId,val);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of times this virtual link has changed its state,
*           or an error has occurred.
*
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of times this virtual link has changed its
*             state, or an error has occurred."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrEventsGet ( L7_uint32 Area,
                                  L7_uint32 RtrId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrEventsGet(Area,RtrId,val);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the current length of the retransmission queue.
*
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The current length of the retransmission queue."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrLsRetransQLenGet ( L7_uint32 Area,
                                         L7_uint32 RtrId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrLsRetransQLenGet(Area,RtrId,val);
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Indicate whether Hellos are being suppressed to the neighbor.
*
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Indicates whether Hellos are being suppressed to the
*             Neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapVirtNbrHelloSuppressedGet ( L7_uint32 Area,
                                           L7_uint32 RtrId, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenVirtNbrHelloSuppressedGet(Area,RtrId,val);
  
  return L7_FAILURE;
}


