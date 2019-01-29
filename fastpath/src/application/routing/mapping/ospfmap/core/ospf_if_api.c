/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_if_api.c
*
* @purpose   Ospf Interface Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for ospf variables, 
*             which are specific to the If Group table
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
extern L7_uint32           *ospfMapMapTbl;
/*
***********************************************************************
*                     API FUNCTIONS  -  IF CONFIG
***********************************************************************
*/

/*-----------------------------------------------------------------------*/
/* The OSPF Interface Table augments the ipAddrTable with OSPF specific  */
/* information.                                                          */
/*-----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Determines if the interface entry exists
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS  if the entry exists
* @returns  L7_FAILURE  if the entry does not exist
*
* @notes    " The OSPF Interface Entry describes one interface from
*             the viewpoint of OSPF."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEntryGet ( L7_uint32 ipAddr, L7_int32 intIfNum )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIfEntryGet(ipAddr, intIfNum);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Interface Entry.
*
* @param    *IpAddr     Ip address
* @param    *intIfNum   internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The OSPF Interface Entry describes one interface from
*             the viewpoint of OSPF."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEntryNext ( L7_uint32 *IpAddr, L7_int32 *intIfNum)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    /* call vendor code to get the next entry */
    if (ospfMapExtenIfEntryNext(IpAddr, intIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Ospf IP Address for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The IP Address of the OSPF interface." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 rtrIfNum;
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        return ospfMapExtenIntfIPAddrGet(intIfNum, val);
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Subnet Mask for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 rtrIfNum;
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        return ospfMapExtenIntfSubnetMaskGet(intIfNum, val);
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *area  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the area to
*             which the interface connects.  Area ID 0.0.0.0 is
*             used for the OSPF backbone." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAreaIdGet(L7_uint32 intIfNum, L7_uint32 *area)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *area = pCfg->area;
    return L7_SUCCESS;
  }
  
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Test if the area id is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the area to
*             which the interface connects.  Area ID 0.0.0.0 is
*             used for the OSPF backbone." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAreaIdSetTest ( L7_uint32 ipAddress, L7_int32 intIfNum,
                                   L7_uint32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfAreaIdSetTest(ipAddress, intIfNum, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the area to
*             which the interface connects.  Area ID 0.0.0.0 is
*             used for the OSPF backbone." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAreaIdSet(L7_uint32 intIfNum, L7_uint32 area)
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 oldAreaId;
  L7_uint32 rc = L7_FAILURE;

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

  if  (ospfMapIntfAreaIdGet(intIfNum, &oldAreaId) == L7_SUCCESS &&
       (oldAreaId == area))
  {
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
  }

  /* Checks if the interface exists and if it has been configured
     for routing */
    if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      pCfg->area = area;
      ospfDataChangedSet(__FUNCTION__);
      (void)ospfMapIntfModeAndAreaCompute(intIfNum);
       rc = L7_SUCCESS;
  }

  osapiSemaGive(ospfMapCtrl_g.cfgSema);
  
  return rc;
}

/*********************************************************************
* @purpose  Gets the Ospf Interface Type
*
* @param    intIfNum    internal interface number  
* @param    *val        integer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *type)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *type = pCfg->ifType;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Test if the OSPF interface type is settable
*
* @param    intIfNum    internal interface number
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The OSPF interface type.  By way of default, this field
*             may be intuited from the corresponding value of ifType
*             Broadcast LANs, such as Ethernet and IEEE 802.5, take
*             the value 'broadcast', X.25 and similar technologies
*             take the value 'nbma', and links that are definitively
*             point to point take the value 'pointToPoint'."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTypeSetTest ( L7_int32 intIfNum,
                                 L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIfTypeSetTest(intIfNum, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the OSPF interface type.
*
* @param    intIfNum    internal interface number
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The OSPF interface type.  By way of default, this field
*             may be intuited from the corresponding value of ifType
*             Broadcast LANs, such as Ethernet and IEEE 802.5, take
*             the value 'broadcast', X.25 and similar technologies
*             take the value 'nbma', and links that are definitively
*             point to point take the value 'pointToPoint'."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTypeSet ( L7_int32 intIfNum, L7_int32 val )
{
	ospfCfgCkt_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 old_val = 0;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    old_val = pCfg->ifType;

    if (val == L7_OSPF_INTF_DEFAULT)
    {
      /* Get the default OSPF network type for this type of interface */
      (void) ospfDefaultIfType(intIfNum, &val);
    }

    if (val != old_val)
  {
    pCfg->ifType = val;
    ospfDataChangedSet(__FUNCTION__);

      if ((rc = ospfMapIntfTypeApply(intIfNum, val)) != L7_SUCCESS)
        return rc;

      /* Changing the interface type can affect redistributed routes */
      ospfMapExtenRedistReevaluate();
    }
  } 

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the default ospf network type of the given interface
*
* @param    intIfNum  internal interface ID
* @param    ospfNetType   return value for OSPF network type
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultIfType(L7_uint32 intIfNum, L7_uint32 *ospfNetType)
{
  return ospfDefaultIfType(intIfNum, ospfNetType);
}

/*********************************************************************
* @purpose  Gets the Ospf Admin mode for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *mode  @b{(output)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The OSPF interface's administrative status.  The value
*             formed on the interface, and the interface will be
*             advertised as an internal route to some area.  The
*             value 'disabled' denotes that the interface is
*             external to OSPF." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAdminModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *mode = pCfg->adminMode;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Tests if the Ospf Admin mode for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The OSPF interface's administrative status.  The value
*             formed on the interface, and the interface will be
*             advertised as an internal route to some area.  The
*             value 'disabled' denotes that the interface is
*             external to OSPF." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAdminModeSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                     L7_uint32 mode)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfAdminModeSetTest(ipAddr, intIfNum, mode);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Admin mode for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The OSPF interface's administrative status.  The value
*             formed on the interface, and the interface will be
*             advertised as an internal route to some area.  The
*             value 'disabled' denotes that the interface is
*             external to OSPF." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 currMode;
  L7_RC_t rc = L7_FAILURE;

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

  if ((ospfMapIntfAdminModeGet(intIfNum, &currMode) == L7_SUCCESS) &&
     (currMode == mode))
  {
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
  }
  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* update configuration */
    pCfg->adminMode = mode;
    ospfDataChangedSet(__FUNCTION__);

    /* Apply configuration. Interface must both exist and be a router interface */
    rc = ospfMapIntfAdminModeProcess(intIfNum, mode);
  }
  osapiSemaGive(ospfMapCtrl_g.cfgSema);
  return rc;
}

/*********************************************************************
* @purpose  Get the SecondariesFlag that indicates the advertisability
*           of the secondary addresses in the OSPF domain.
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    *secondaries @b{(output)} secondaries flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfSecondariesFlagGet(L7_uint32 intIfNum, L7_uint32 *secondaries)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *secondaries = pCfg->advertiseSecondaries;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Set the SecondariesFlag for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    secondaries @b{(input)} secondaries flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfSecondariesFlagSet(L7_uint32 intIfNum, L7_uint32 secondaries)
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 prevSecondaries;
  L7_RC_t rc = L7_FAILURE;

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

  if ((ospfMapIntfSecondariesFlagGet(intIfNum, &prevSecondaries) == L7_SUCCESS) &&
     (prevSecondaries == secondaries))
  {
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
  }

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* update configuration */
    pCfg->advertiseSecondaries = secondaries;
    ospfDataChangedSet(__FUNCTION__);

    /* Apply configuration. Interface must both exist and be a router interface */
    rc = L7_SUCCESS;
    if (ospfMapOspfIntfInitialized(intIfNum) == L7_TRUE)
      rc = ospfMapIntfSecondariesFlagApply(intIfNum, secondaries);
  }
  osapiSemaGive(ospfMapCtrl_g.cfgSema);
  return rc;
}

/*********************************************************************
* @purpose  Gets the Ospf Priority for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *priority  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The priority of this interface.  Used in multi-access
*             networks, this field is used in the designated router
*             election algorithm.  The value 0 signifies that the
*             router is not eligible to become the designated router
*             on this particular network.  In the event of a tie in
*             this value, router will use their Router ID as a tie
*             breaker." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPriorityGet(L7_uint32 intIfNum, L7_uint32 *priority)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *priority = pCfg->priority;
    return L7_SUCCESS;
  }
  return L7_ERROR;

}

/*********************************************************************
* @purpose  Test if the priority of this interface is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    val         Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The priority of this interface.  Used in multi-access
*             networks, this field is used in the designated router
*             election algorithm.  The value 0 signifies that the
*             router is not eligible to become the designated router
*             on this particular network.  In the event of a tie in
*             this value, router will use their Router ID as a tie
*             breaker." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPrioritySetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                     L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfPrioritySetTest(ipAddr, intIfNum, val);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Priority for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    priority   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The priority of this interface.  Used in multi-access
*             networks, this field is used in the designated router
*             election algorithm.  The value 0 signifies that the
*             router is not eligible to become the designated router
*             on this particular network.  In the event of a tie in
*             this value, router will use their Router ID as a tie
*             breaker." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPrioritySet(L7_uint32 intIfNum, L7_uint32 priority)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
   {
    pCfg->priority = priority;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfPriorityApply( intIfNum,  priority));
  }

  return L7_ERROR;

}

/*********************************************************************
* @purpose  Gets the Ospf Transit Delay for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The estimated number of seconds it takes to transmit
*             a link state update packet over this interface." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTransitDelayGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *seconds = pCfg->ifTransitDelay;
    return L7_SUCCESS;
  }
  return L7_ERROR;

}

/*********************************************************************
* @purpose  Tests if the Ospf Transit Delay for the specified interface 
*           is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The estimated number of seconds it takes to transmit
*             a link state update packet over this interface." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTransitDelaySetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                       L7_uint32 seconds)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfTransitDelaySetTest(ipAddr, intIfNum, seconds);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Transit Delay for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The estimated number of seconds it takes to transmit
*             a link state update packet over this interface." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTransitDelaySet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->ifTransitDelay = seconds;
    ospfDataChangedSet(__FUNCTION__);
    return( ospfMapIntfTransitDelayApply( intIfNum,  seconds) );
  }

  return L7_ERROR;

}


/*********************************************************************
* @purpose  Gets the LSA Ack Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds, between LSA Acknowledgement 
*             packet transmissions. The value must be less than 
*             retransmit interval."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLsaAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *seconds = pCfg->lsaAckInterval;
    return L7_SUCCESS;
  }
  return L7_ERROR;

}

/*********************************************************************
* @purpose  Sets the LSA Ack Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds, between LSA Acknowledgement 
*             packet transmissions. The value must be less than 
*             retransmit interval."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLsaAckIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->lsaAckInterval = seconds;
    ospfDataChangedSet(__FUNCTION__);
    return( ospfMapIntfLsaAckIntervalApply( intIfNum,  seconds) );
  }

  return L7_ERROR;

}


/*********************************************************************
* @purpose  Gets the Ospf Retransmit Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfRxmtIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *seconds = pCfg->rxmtInterval;
    return L7_SUCCESS;
  }
  return L7_ERROR;

}

/*********************************************************************
* @purpose  Tests if the Ospf Retransmit Interval for the specified interface 
*           is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfRxmtIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                       L7_uint32 seconds)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfRxmtIntervalSetTest(ipAddr, intIfNum, seconds);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Retransmit Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfRxmtIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->rxmtInterval = seconds;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfRxmtIntervalApply(intIfNum, seconds));
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Hello Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds between the Hello
*             packets that the router sends on the interface.
*             This value must be the same for all router attached
*             to a common network." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfHelloIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *seconds = pCfg->helloInterval;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Test if the Ospf Hello Interval for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} ip Address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds between the Hello
*             packets that the router sends on the interface.
*             This value must be the same for all router attached
*             to a common network." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfHelloIntervalSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 seconds)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfHelloIntervalSetTest(ipAddr, intIfNum, seconds);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Hello Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds between the Hello
*             packets that the router sends on the interface.
*             This value must be the same for all router attached
*             to a common network." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfHelloIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->helloInterval = seconds;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfHelloIntervalApply(intIfNum, seconds));
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Dead Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of the
*             Hello interval.  This value must be the same for all
*             routers attached to a common network." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDeadIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *seconds = pCfg->deadInterval;
    return L7_SUCCESS;
  }
  return L7_ERROR;

}

/*********************************************************************
* @purpose  Test if the Ospf Dead Interval for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of the
*             Hello interval.  This value must be the same for all
*             routers attached to a common network." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDeadIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                       L7_uint32 seconds)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfDeadIntervalSetTest(ipAddr, intIfNum, seconds);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Dead Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of the
*             Hello interval.  This value must be the same for all
*             routers attached to a common network." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDeadIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;
  if ((seconds < L7_OSPF_INTF_DEAD_MIN) || (seconds>L7_OSPF_INTF_DEAD_MAX))
  {
    return L7_FAILURE;
  }
  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->deadInterval = seconds;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfDeadIntervalApply(intIfNum, seconds));
  }

  return L7_ERROR;

}

/*********************************************************************
* @purpose  Gets the Ospf Nbma Poll Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The larger time interval, in seconds, between the
*             Hello packets sent to an inactive non-broadcast multi-
*             access neighbor." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfNbmaPollIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *seconds = pCfg->nbmaPollInterval;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the effective passive mode of the interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *passiveMode  @b{(output)} Passive Mode
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
*
* @notes    This function is called at the OSPF interface's initialization
*           after reboot, and also when a new OSPF interface is created
*           during the run
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectivePassiveModeGet(L7_uint32 intIfNum, L7_BOOL *passiveMode)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if(pCfg->passiveMode == L7_OSPF_INTF_PASSIVE)
       *passiveMode = L7_TRUE;
    else
       *passiveMode = L7_FALSE;

    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the passive mode of the interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *passiveMode  @b{(output)} Passive Mode
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST, if passive mode is not configured
*           L7_ERROR, if interface has not been enabled for routing
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPassiveModeGet(L7_uint32 intIfNum, L7_BOOL *passiveMode)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if(pCfg->passiveMode == L7_OSPF_INTF_PASSIVE)
       *passiveMode = L7_TRUE;
    else if(pCfg->passiveMode == L7_OSPF_INTF_NOT_PASSIVE)
       *passiveMode = L7_FALSE;
    else
        return L7_NOT_EXIST; /* don't return success if there is no passive-mode
                              * effective on an interface */

    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Sets the passive mode of the interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    passiveMode   @b{(input)} passive mode of interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPassiveModeSet(L7_uint32 intIfNum, L7_BOOL passiveMode)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if(((pCfg->passiveMode == L7_OSPF_INTF_PASSIVE) && (passiveMode == L7_TRUE)) ||
       ((pCfg->passiveMode == L7_OSPF_INTF_NOT_PASSIVE) && (passiveMode == L7_FALSE)))
      return L7_SUCCESS; /* no change */

    if(passiveMode == L7_TRUE)
       pCfg->passiveMode = L7_OSPF_INTF_PASSIVE;
    else
       pCfg->passiveMode = L7_OSPF_INTF_NOT_PASSIVE;

    ospfDataChangedSet(__FUNCTION__);

    return ospfMapIntfPassiveModeApply(intIfNum, passiveMode);
  }

  return L7_ERROR;

}

/*********************************************************************
* @purpose  Test if the Ospf Nbma Poll Interval for the specified interface 
*           is settable.
*
* @param    ipAddr    @b{(input)} ip address of the interface
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The larger time interval, in seconds, between the
*             Hello packets sent to an inactive non-broadcast multi-
*             access neighbor." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfNbmaPollIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                           L7_uint32 seconds)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfNbmaPollIntervalSetTest(ipAddr, intIfNum, seconds);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Nbma Poll Interval for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The larger time interval, in seconds, between the
*             Hello packets sent to an inactive non-broadcast multi-
*             access neighbor." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfNbmaPollIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->nbmaPollInterval = seconds;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfNbmaPollIntervalApply(intIfNum,seconds));
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf State for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The OSPF Interface state:
*
*               down (1)
*               loopback (2)
*               waiting (3)
*               pointToPoint (4)
*               designatedRouter (5)
*               backupDesignatedRouter (6)
*               otherDesignatedRouter (7) " 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfStateGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 rtrIfNum, ipAddr, ipMask;
  ospfCfgCkt_t *pCfg = L7_NULLPTR;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) ==
            L7_SUCCESS)
        {
          return ospfMapExtenIntfStateGet(ipAddr, intIfNum, val);
        }
      }
    }
  }
  
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Get OSPFv2 interface statistics 
*
* @param    intIfNum  @b{(input)}    interface whose stats are to be retrieved
* @param    intfStats @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_NOT_EXIST if OSPF is not enabled on the interface
* @returns  L7_FAILURE  if failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfStatsGet(L7_uint32 intIfNum, L7_OspfIntfStats_t *intfStats)
{
  if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
  {
    return ospfMapExtenIntfStatsGet(intIfNum, intfStats);
  }
  
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf LSA Acknowledgement Interval for the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *seconds  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLSAAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  L7_uint32 index;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
        return L7_ERROR;

  index = ospfMapMapTbl[intIfNum];


  *seconds = pOspfMapCfgData->ckt[index].lsaAckInterval;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSAs for the specified 
*           interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLocalLinkOpaqueLSAsGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 rtrIfNum;
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        return ospfMapExtenIntfLocalLinkOpaqueLSAsGet(intIfNum, val);
      }
    }
  }
  
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSA Checksum for the 
*           specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLocalLinkOpaqueLSACksumGet(L7_uint32 intIfNum, L7_uint32 *val)
{
    L7_uint32 rtrIfNum;
    ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
          return ospfMapExtenIntfLocalLinkOpaqueLSACksumGet(intIfNum, val);
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Designated Router Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The IP Address of the Designated Router.  
*             Default value: 0.0.0.0." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDrIdGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 rtrIfNum, ipAddr, ipMask;
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum)== L7_TRUE) 
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) ==
            L7_SUCCESS)
        {
          return ospfMapExtenIntfDrIdGet(ipAddr, intIfNum, val);
        }
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Backup Designated Router Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The IP Address of the Backup Designated Router.
*             Default value: 0.0.0.0." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfBackupDrIdGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 rtrIfNum, ipAddr, ipMask;
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) ==
            L7_SUCCESS)
        {
          return ospfMapExtenIntfBackupDrIdGet(ipAddr, intIfNum, val);
        }
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the number of times this  OSPF  interface  has 
*            changed its state 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *val  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of times this OSPF interface has changed
*             its state, or an error has occurred." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLinkEventsCounterGet(L7_uint32 intIfNum, L7_uint32 *val)
{
    L7_uint32 rtrIfNum, ipAddr, ipMask;
    ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) 
  {
    /* Interface must both exist and be a router interface */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE) 
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) ==
            L7_SUCCESS)
        {
          return ospfMapExtenIntfLinkEventsCounterGet(ipAddr, intIfNum, val);
        }
      }
    }
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfStatusGet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 *val )
{
  L7_uint32 rtrIfNum;

  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      return ospfMapExtenIfStatusGet(ipAddr, intIfNum, val);
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To indicate whether Demand OSPF procedures should be
*           performed on this interface.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_NOT_SUPPORTED
*
* @notes    " Indicates whether Demand OSPF procedures (hello
*             supression to FULL neighbors and setting the DoNotAge
*             flag on propogated LSAs) should be performed on
*             this interface."
*
* @notes    This function is not supported.
* @notes    This function returns the default truth value.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandGet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 *val )
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *val = pCfg->ifDemandMode;
    return L7_SUCCESS;
  }
  return L7_ERROR;

  /*return L7_NOT_SUPPORTED;*/
}

/*********************************************************************
* @purpose  Test if the ospf demand value is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_NOT_SUPPORTED
*
* @notes    " Indicates whether Demand OSPF procedures (hello
*             supression to FULL neighbors and setting the DoNotAge
*             flag on propogated LSAs) should be performed on
*             this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandSetTest( L7_uint32 ipAddr, L7_int32 intIfNum,
                                  L7_int32 val )
{
    /*
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfDemandAgingSetTest(ipAddr, intIfNum, val);
  }
  */

  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Set the ospf demand value
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_NOT_SUPPORTED
*
* @notes    " Indicates whether Demand OSPF procedures (hello
*             supression to FULL neighbors and setting the DoNotAge
*             flag on propogated LSAs) should be performed on
*             this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandSet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 val )
{
   /* ospfCfgCkt_t *pCfg; */

    /*
    if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_SUCCESS)
    {
      pCfg->ifDemandMode = val;
      ospfDataChangedSet(__FUNCTION__);
      return(ospfMapIntfDemandApply(intIfNum, val));
    }
    */

  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the Ospf Authentication Type for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *authType  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally."
*
*             Authentication Types: See L7_AUTH_TYPES_t
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthTypeGet(L7_uint32 intIfNum, L7_uint32 *authType)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *authType = pCfg->authType;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Test if the Ospf Authentication Type for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} IP adress of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    authType @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally."
*
*             Authentication Types: See L7_AUTH_TYPES_t
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthTypeSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                    L7_uint32 authType)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfAuthTypeSetTest(ipAddr, intIfNum, authType);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Type for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    authType   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing 
*                                           or
*                     if the new authType is different from the areaAuth
*                     type when running in RFC 1583 compliant mode.
* @returns  L7_FAILURE
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally."
*
*             Authentication Types: See L7_AUTH_TYPES_t
*       
* @notes    For security reasons, the authentication key is always
*           cleared whenever the auth type is changed.  If setting a 
*           new key value, be sure to do it AFTER setting the auth type. 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthTypeSet(L7_uint32 intIfNum, L7_uint32 authType)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (pCfg->authType == authType)
      return L7_SUCCESS;

    /* always clear the key when the auth type changes
     *
     * NOTE: This is to reduce the possibility of transmitting an 
     *       MD5 secret key in the clear when the auth type changes
     *       from MD5 to Simple.
     */
    memset(pCfg->authKey, 0, (size_t)L7_AUTH_MAX_KEY_OSPF);
    pCfg->authKeyLen = 0;

    pCfg->authType = authType;
    ospfDataChangedSet(__FUNCTION__);

    return(ospfMapIntfAuthTypeApply(intIfNum, authType));
  }

  return L7_ERROR;

}

/*********************************************************************
* @purpose  Gets the configured Ospf Metric value for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *metric  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR if interface does not exist
*
* @notes    This function returns the value of the user configured OSPF
*           interface metric.  This value is used to calculate the cost
*           associated with the interface.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricGet(L7_uint32 intIfNum, L7_uint32 *metric)
{
  L7_uint32 index;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
        return L7_ERROR;

  index = ospfMapMapTbl[intIfNum];

  *metric = pOspfMapCfgData->ckt[index].metric;

  if(*metric != L7_OSPF_INTF_METRIC_NOT_CONFIGURED)
    return L7_SUCCESS;

  if(ospfMapIntfMetricCalculate(intIfNum, metric) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Authentication Key for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *key  @b{(output)}   Authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
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
L7_RC_t ospfMapIntfAuthKeyGet(L7_uint32 intIfNum, L7_uchar8 *key)
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 keyLen;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* output the key contents as a string */
    keyLen = pCfg->authKeyLen;
    memcpy(key, pCfg->authKey, keyLen);
    /* terminate the key value with a '\0' to make it an ASCIIZ string */
    *(key+keyLen) = L7_EOS;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    key      @b{(input)} Authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
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
L7_RC_t ospfMapIntfAuthKeySetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                   L7_uchar8 *key)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenIntfAuthKeySetTest(ipAddr, intIfNum, key);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    key   @b{(input)} Authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
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
L7_RC_t ospfMapIntfAuthKeySet(L7_uint32 intIfNum, L7_uchar8 *key)
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 keyLen, authType, authLenMax;
  L7_uchar8 newKey[L7_AUTH_MAX_KEY_OSPF];
  L7_uchar8 *pCfgKey;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* Convert incoming key string to a left-justified array, padded with hex zeroes */
    if(key != L7_NULLPTR)
      keyLen = strlen(key);
    else
      keyLen = 0;

    /* use current auth type from the configuration */
    authType = pCfg->authType;

    /* maximum allowed key length depends on current authType value */
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
      memcpy(&newKey, key, (size_t)keyLen);

    /* if incoming key is same as configured key, just return successfully */
    pCfgKey = pCfg->authKey;
    if (memcmp(pCfgKey, &newKey, (size_t)L7_AUTH_MAX_KEY_OSPF) == 0)
      return L7_SUCCESS;

    /* update config struct with latest key and length */
    memcpy(pCfgKey, newKey, (size_t)L7_AUTH_MAX_KEY_OSPF);
    pCfg->authKeyLen = keyLen;

    ospfDataChangedSet(__FUNCTION__);

    return ospfMapIntfAuthKeyApply(intIfNum, pCfgKey);
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the Ospf Authentication Key Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *keyId   @b{(output)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthKeyIdGet(L7_uint32 intIfNum, L7_uint32 *keyId)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *keyId = pCfg->authKeyId;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key Id for the specified interface 
*           is settable.
*
* @param    ipAddr   @b{(input)} IP adress of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    keyId    @b{(input)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthKeyIdSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                     L7_uint32 keyId)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    if ((keyId < L7_OSPF_AUTH_KEY_ID_MIN) ||
        (keyId > L7_OSPF_AUTH_KEY_ID_MAX))
      return L7_FAILURE;

    return ospfMapExtenIntfAuthKeyIdSetTest(ipAddr, intIfNum, keyId);
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    keyId    @b{(input)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthKeyIdSet(L7_uint32 intIfNum, L7_uint32 keyId)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if ((keyId < L7_OSPF_AUTH_KEY_ID_MIN) ||
        (keyId > L7_OSPF_AUTH_KEY_ID_MAX))
      return L7_FAILURE;

    if (pCfg->authKeyId == keyId)
      return L7_SUCCESS;

    pCfg->authKeyId = keyId;
    ospfDataChangedSet(__FUNCTION__);

    return(ospfMapIntfAuthKeyIdApply(intIfNum, keyId));
  }

  return L7_ERROR;
}


/*--------------------------------------------------------------------------*/
/* The OSPF Interface Metric Table:                                         */
/* The Metric Table describes the metrics to be advertised for a specified  */
/* interface at the various types of service. As such, this table is an     */
/* adjunct of the OSPF Interface Table.                                     */
/*--------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get a particular TOS metric for a non-virtual interface.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A particular TOS metric for a non-virtual interface
*             identified by the interface index."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricEntryGet ( L7_uint32 ipAddr, L7_int32 intIfNum, L7_int32 TOS )
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized() == L7_TRUE) 
  {
    rc = ospfMapExtenIfMetricEntryGet(ipAddr, intIfNum, TOS);
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next TOS metric for a non-virtual interface.
*
* @param    *ipAddr        Ip Address of the interface
* @param    *intIfNum      interface
* @param    *TOS           TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A particular TOS metric for a non-virtual interface
*             identified by the interface index."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricEntryNext ( L7_uint32 *ipAddr, L7_int32 *intIfNum, L7_int32 *TOS )
{
  L7_RC_t rc;

  if (ospfMapOspfInitialized() == L7_TRUE) 
  {
    rc = ospfMapExtenIfMetricEntryNext(ipAddr, intIfNum, TOS);
    return rc;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF interface metric value.
*
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    *val        metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The type of service metric being referenced.  On row
*             creation, this can be derived from the instance."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricValueGet ( L7_int32 intIfNum, L7_int32 TOS, L7_int32 *val )
{
  return ospfMapIntfMetricGet(intIfNum, (L7_uint32 *)val);
}

/*********************************************************************
* @purpose  Determine if the OSPF metric has been configured
*
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number  
* @param    TOS         TOS metric
*
* @returns  L7_TRUE     if the metric has been configured by the user
* @returns  L7_FALSE    if the metric has been computed based on the
*                       link speed
*
* @notes    The default value of the TOS 0 Metric is 10^8 / ifSpeed.
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsIntfMetricConfigured (L7_uint32 IpAddress, L7_int32 intIfNum, L7_int32 TOS)
{
  L7_uint32 index;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
        return L7_FALSE;

  index = ospfMapMapTbl[intIfNum];

  return ((pOspfMapCfgData->ckt[index].metric == 
             L7_OSPF_INTF_METRIC_NOT_CONFIGURED)? L7_FALSE : L7_TRUE);
  
}

/*********************************************************************
* @purpose  Test to set the metric value.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The type of service metric being referenced.  On row
*             creation, this can be derived from the instance."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricValueSetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                  L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenIfMetricValueSetTest(ipAddr, intIfNum,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the metric value.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The type of service metric being referenced.  On row
*             creation, this can be derived from the instance."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricValueSet ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                  L7_int32 TOS, L7_int32 val )
{
  L7_uint32 ifMetric;
  ospfCfgCkt_t *pCfg;
    
  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;
  
  pCfg->metric = val;
  ospfDataChangedSet(__FUNCTION__);

  if(ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  ifMetric = val;
  if (pCfg->metric == L7_OSPF_INTF_METRIC_NOT_CONFIGURED)
  {
    if (ospfMapIntfMetricCalculate(intIfNum, &ifMetric) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return ospfMapExtenIfMetricValueSet(intIfNum, TOS, ifMetric);
}

/*********************************************************************
* @purpose  Get the OSPF interface metric value status.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricStatusGet ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                   L7_int32 TOS, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenIfMetricStatusGet(ipAddr, intIfNum,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricStatusSetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                   L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenIfMetricStatusSetTest(ipAddr, intIfNum,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricStatusSet ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                   L7_int32 TOS, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenIfMetricStatusSet(ipAddr, intIfNum,TOS,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Ospf Virtual Transit Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *area  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The Transit Area that the Virtual Link traverses.  By
*             definition, this is not 0.0.0.0 " 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfVirtTransitAreaIdGet(L7_uint32 intIfNum, L7_uint32 *area)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *area = pCfg->virtTransitAreaID;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Sets the Ospf Virtual Transit Area Id for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The Transit Area that the Virtual Link traverses.  By
*             definition, this is not 0.0.0.0 " 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfVirtTransitAreaIdSet(L7_uint32 intIfNum, L7_uint32 area)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->virtTransitAreaID = area;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfVirtTransitAreaIdApply( intIfNum, area));
  }

  return L7_ERROR;

}

/*********************************************************************
* @purpose  Gets the Ospf Virtual Neighbor for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *neighbor  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    " The Router ID of the Virtual Neighbor." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfVirtIntfNeighborGet(L7_uint32 intIfNum, L7_uint32 *neighbor)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *neighbor = pCfg->virtIntfNeighbor;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Sets the Ospf Virtual Neighbor for the specified interface 
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    neighbor   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The Router ID of the Virtual Neighbor." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfVirtIntfNeighborSet(L7_uint32 intIfNum, L7_uint32 areaId,
                                       L7_uint32 neighbor)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  { 
    pCfg->virtTransitAreaID = areaId;
    pCfg->virtIntfNeighbor = neighbor;
    ospfDataChangedSet(__FUNCTION__);
    return( ospfMapIntfVirtIntfNeighborApply( intIfNum, areaId, neighbor));
  }

  return L7_ERROR;

}

/*********************************************************************
* @purpose  Sets the Ospf Ip Mtu Max
*
* @param    ipMtu   @b{(input)}
* @param    intIfNum   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfIpMtuSet(L7_uint32 intIfNum, L7_uint32 ipMtu)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    return(ospfMapInftIpMtuApply(intIfNum, ipMtu));
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Gets the max Ip Mtu size.
*
* @param    intIfNum   @b{(input)}
* @param    *maxIpMtu  @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfIpMtuGet( L7_uint32 intIfNum, L7_uint32 *maxIpMtu )
{
  return ipMapIntfEffectiveIpMtuGet(intIfNum, maxIpMtu);
}


/*********************************************************************
* @purpose  Get the value of the MtuIgnore flag.
*
* @param    intIfNum   @b{(input)}
* @param    *val       @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router ignores the MTU value while forming
*                    adjacencies
*           L7_FALSE: The router forms adjacency taking into account
*                     the MTU value.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMtuIgnoreGet( L7_uint32 intIfNum, L7_BOOL *val )
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    *val = pCfg->mtuIgnore;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Set the vlaue of MtuIgnore Flag.
*
* @param    intIfNum   @b{(input)}
* @param    val        @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router ignores the MTU value while forming
*                    adjacencies
*           L7_FALSE: The router forms adjacency taking into account
*                     the MTU value.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMtuIgnoreSet( L7_uint32 intIfNum, L7_BOOL val )
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 prevVal;

  /* If the previous configuration is the same as that specified
  ** by the user return successfully
  */
  if(ospfMapIntfMtuIgnoreGet(intIfNum, &prevVal) == L7_SUCCESS &&
    (prevVal == val))
    return L7_SUCCESS;

  /* Save the configuration */
  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->mtuIgnore = val;
    ospfDataChangedSet(__FUNCTION__);
    return(ospfMapIntfMtuIgnoreApply(intIfNum, val));
  }

  return L7_ERROR;

}


