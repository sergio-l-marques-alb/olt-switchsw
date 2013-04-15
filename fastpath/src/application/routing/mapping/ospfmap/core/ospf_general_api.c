/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_general_api.c
*
* @purpose   Ospf General Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for general ospf
*            variables, which are global to the ospf process (rfc 1850)
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
#include "ospfv2_decode.ext"

extern L7_ospfMapCfg_t    *pOspfMapCfgData;
extern ospfInfo_t         *pOspfInfo;

                    
/*
***********************************************************************
*                     API FUNCTIONS  -  GLOBAL CONFIG
***********************************************************************
*/

/*********************************************************************
* @purpose  Get general OSPFv2 status information.
*
* @param    *status  @b{(output)}   contains output data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapStatusGet(L7_ospfStatus_t *status)
{
  return ospfMapExtenStatusGet(status);
}

/*********************************************************************
* @purpose  Gets the Ospf Router ID
*
* @param    *routerID  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the router in the
*           Autonomous System.
*           By convention, to ensure uniqueness, this should default
*           to the value of one of the router's IP interface addresses."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRouterIDGet(L7_uint32 *routerID)
{
    if (routerID != L7_NULLPTR)
    {
        *routerID = pOspfMapCfgData->rtr.routerID;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Router ID
*
* @param    routerID   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the router in the
*           Autonomous System.
*           
*           To set a non-zero router ID, either OSPF must be disabled or
*           the current router ID must be 0.
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRouterIDSet(L7_uint32 routerID)
{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 oldRouterId = pOspfMapCfgData->rtr.routerID;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER);

    /* verify this is actually a change */
    if (pOspfMapCfgData->rtr.routerID == routerID)
    {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_SUCCESS;
    }

    if (routerID && (oldRouterId != 0) && 
        (pOspfMapCfgData->rtr.ospfAdminMode == L7_ENABLE))
    {
      osapiSemaGive(ospfMapCtrl_g.cfgSema);
      return L7_FAILURE;
    }

    pOspfMapCfgData->rtr.routerID = routerID;
    rc = ospfMapRouterIDApply(routerID);

    /* only store the value if it successfully changes */
    if (rc == L7_SUCCESS)
    {
      ospfDataChangedSet(__FUNCTION__);
    }
    else
    {
      /* reapply the old routerID */
      (void)ospfMapRouterIDApply(oldRouterId);
    }

    osapiSemaGive(ospfMapCtrl_g.cfgSema);

    return rc;
}

/*********************************************************************
* @purpose  Gets the Ospf Admin Mode 
*
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " The administrative status of OSPF in the router. The value
*           'enabled' denotes that the OSPF process is active on at
*           least one interface; 'disabled' disables it on all
*           interfaces."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAdminModeGet(L7_uint32 *mode)
{
  *mode = pOspfMapCfgData->rtr.ospfAdminMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ospf Admin mode
*
* @param    mode  @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " The administrative status of OSPF in the router. The value
*           'enabled' denotes that the OSPF process is active on at
*           least one interface; 'disabled' disables it on all
*           interfaces."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAdminModeSet(L7_uint32 mode)
{
  L7_uint32 currMode;
  L7_RC_t rc = L7_SUCCESS;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

  if ((ospfMapOspfAdminModeGet(&currMode) == L7_SUCCESS) &&
      (mode == currMode))
  {
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
  }
  
  pOspfMapCfgData->rtr.ospfAdminMode = mode;
  ospfDataChangedSet(__FUNCTION__);
  if ((mode == L7_ENABLE) && ospfMayEnableOspf())
  {
    rc = ospfMapOspfEnable(L7_FALSE);
  }
  else if (mode == L7_DISABLE)
  {
    rc = ospfMapOspfAdminModeDisable();
  }

  osapiSemaGive(ospfMapCtrl_g.cfgSema);
  return rc;
}

/*********************************************************************
* @purpose  Get the current version number of the OSPF protocol.
*
* @param    *val        version number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Currently this is set to 2
*
* @end
*********************************************************************/
L7_RC_t ospfMapVersionNumberGet ( L7_int32 *val )
{
  *val = 2;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the ABR Admin Mode
*
* @param    *mode  @b{(output)}     L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " A flag to note whether the router is an area border
*             router."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAbrStatusGet(L7_uint32 *mode)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenAbrStatusGet(mode);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the ASBR Admin Mode
*
* @param    *mode  @b{(output)} L7_ENABLE if OSPF is configured to redistribute 
*                               from any source or if OSPF is configured to 
*                               originate a default route. L7_DISABLE otherwise.
*
* @returns  L7_SUCCESS 
*
* @notes    " A flag to note whether this router is configured as an 
*           Autonomous System Border Router."
*
* @notes    The user no longer configures a single ASBR
*           admin mode parameter. Instead, the user configures 
*           redistribution independently for each source.
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAsbrAdminModeGet(L7_uint32 *mode)
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (ospfMapIsAsbr()) {
        *mode = L7_ENABLE;
    }
    else {
        *mode = L7_DISABLE;
    }   
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}


/* Queries for Routing Table Entries for "Border Routers" */

/*********************************************************************
* @purpose  Get the ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ABR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAbrEntryGet ( L7_uint32 destinationIp,
                             L7_RtbEntryInfo_t *p_rtbEntry )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenAbrEntryGet(destinationIp, p_rtbEntry);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ASBR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAsbrEntryGet ( L7_uint32 destinationIp,
                              L7_RtbEntryInfo_t *p_rtbEntry )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenAsbrEntryGet(destinationIp, p_rtbEntry);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ABR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAbrEntryNext ( L7_uint32 *destinationIp,
                              L7_RtbEntryInfo_t *p_rtbEntry )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenAbrEntryNext(destinationIp, p_rtbEntry);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ASBR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAsbrEntryNext ( L7_uint32 *destinationIp,
                               L7_RtbEntryInfo_t *p_rtbEntry )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenAsbrEntryNext(destinationIp, p_rtbEntry);

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Retrieve the Ospf Opaque AS LSA Count
*          
* @param    *val    L7_uint32    LSA count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of opaque AS (LS type 11) link-state
*           advertisements in the link-state database."
*        
* @end
*********************************************************************/
L7_RC_t ospfMapOpaqueASLSACountGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return( ospfMapExtenOpaqueASLSACountGet(val));

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Retrieve the Ospf External LSA Count
*          
* @param    *val    L7_uint32   external LSA count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of external (LS type 5) link-state
*           advertisements in the link-state database."
*        
* @end
*********************************************************************/
L7_RC_t ospfMapExternalLSACountGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return( ospfMapExtenExternalLSACountGet(val));

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Retrieve the Ospf Self Origianated External LSA Count
*          
* @param    *val    L7_uint32   external LSA count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*        
* @end
*********************************************************************/
L7_RC_t ospfMapSelfOrigExternalLSACountGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return( ospfMapExtenSelfOrigExternalLSACountGet(val));

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Retrieve the Ospf External LSA Checksum
*          
* @param    *val    L7_uint32   external LSA checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The 32-bit unsigned sum of the LS checksums of the 
*           external link-state advertisements contained in the
*           link-state database.  This sum can be used to determine
*           if there has been a change in a router's link-state
*           database, and to compare the link-state database of two
*           routers."
*        
* @end
*********************************************************************/
L7_RC_t ospfMapExternalLSAChecksumGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return( ospfMapExtenExternalLSAChecksumGet(val));

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Retrieve the New LSAs Orignated
*          
* @param    *val    L7_uint32   new LSAs orignated
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of new link-state advertisements that have been
*           originated.  This number is incremented each time the
*           router originates a new LSA." 
*        
* @end
*********************************************************************/
L7_RC_t ospfMapNewLSAOrigGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return( ospfMapExtenNewLSAOrigGet(val));

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Retrieve the Number of LSAs Received
*          
* @param    *val    L7_uint32   number of LSAs received
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of link-state advertisements received determined
*           to be new instantiations.  This number does not include
*           newer instantiations of self-originated link-state
*           advertisements."
*        
* @end
*********************************************************************/
L7_RC_t ospfMapNumLSAReceivedGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return(ospfMapExtenNumLSAReceivedGet(val));

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the External LSA Limit
*
* @param    *mode  @b{(output)}     External LSA Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " The default value is -1. If the value is -1, then there
*           is no limit. When the number of non-default AS-external-LSAs
*           in a router's link-state database reaches ExtLsdbLimit, the
*           router enters overflow state. The router never holds more than
*           ExtLsdbLimit non-default AS-external-LSAs in it database
*           ExtLsdbLimit MUST be set identically in all routers attached
*           to the OSPF backbone and/or any regular OSPF area. (that is,
*           OSPF stub areas and NSSAs are excluded."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExternalLSALimitGet(L7_int32 *val)
{
  *val = pOspfMapCfgData->rtr.extLSALimit;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the maximum number of non-default AS-external-LSAs entries 
*           that can be stored in the link-state database.
*          
* @param    val         number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The default value is -1. If the value is -1, then there
*           is no limit. When the number of non-default AS-external-LSAs
*           in a router's link-state database reaches ExtLsdbLimit, the
*           router enters overflow state. The router never holds more than
*           ExtLsdbLimit non-default AS-external-LSAs in it database
*           ExtLsdbLimit MUST be set identically in all routers attached
*           to the OSPF backbone and/or any regular OSPF area. (that is,
*           OSPF stub areas and NSSAs are excluded."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbLimitSet ( L7_int32 val )
{
  pOspfMapCfgData->rtr.extLSALimit = val;
  ospfDataChangedSet(__FUNCTION__);

  return ospfMapExtLsdbLimitApply(val);
}

/*********************************************************************
* @purpose  Gets the Exit OverFlow Interval
*
* @param    *val  @b{(output)}     Exit Overflow Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " The number of seconds, that after entering Overflow
*             state, a router will attempt to leave OverflowState.
*             This allows the router to again originate non-default
*             AS-external-LSAs.  When set to 0, the router will
*             not leave OverflowState until restarted." 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExitOverflowIntervalGet(L7_uint32 *val)
{
  *val = pOspfMapCfgData->rtr.exitOverflowInterval;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Exit OverFlow Interval
*          
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds, that after entering Overflow
*             state, a router will attempt to leave OverflowState.
*             This allows the router to again originate non-default
*             AS-external-LSAs.  When set to 0, the router will
*             not leave OverflowState until restarted." 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExitOverflowIntervalSet ( L7_int32 val )
{
  pOspfMapCfgData->rtr.exitOverflowInterval = val;
  ospfDataChangedSet(__FUNCTION__);

  return ospfMapExitOverflowIntervalApply(val);
}

/*********************************************************************
* @purpose  Gets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    spfDelay    @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between when OSPF receives
*             a topology change and when it starts an SPF calculation."
*
* @end
*********************************************************************/
L7_RC_t ospfMapSpfDelayGet(L7_uint32 *spfDelay)
{
  *spfDelay = pOspfMapCfgData->rtr.spfDelay;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    spfDelay    @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between when OSPF receives
*             a topology change and when it starts an SPF calculation."
*
* @end
*********************************************************************/
L7_RC_t ospfMapSpfDelaySet(L7_uint32 spfDelay)
{
  pOspfMapCfgData->rtr.spfDelay = spfDelay;
  ospfDataChangedSet(__FUNCTION__);

  return ospfMapSpfDelayApply(spfDelay);
}

/*********************************************************************
* @purpose  Gets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    spfHoldtime @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between two consecutive
*             SPF calculations."
*
* @end
*********************************************************************/
L7_RC_t ospfMapSpfHoldtimeGet(L7_uint32 *spfHoldtime)
{
  *spfHoldtime = pOspfMapCfgData->rtr.spfHoldtime;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    spfHoldtime @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between two consecutive
*             SPF calculations."
*
* @end
*********************************************************************/
L7_RC_t ospfMapSpfHoldtimeSet(L7_uint32 spfHoldtime)
{
  pOspfMapCfgData->rtr.spfHoldtime = spfHoldtime;
  ospfDataChangedSet(__FUNCTION__);

  return ospfMapSpfHoldtimeApply(spfHoldtime);
}

/*********************************************************************
* @purpose  Get the default passive mode on the interfaces
*
* @param    passiveMode @b{(output)} passive mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapPassiveModeGet(L7_BOOL *passiveMode)
{
  *passiveMode = pOspfMapCfgData->rtr.defaultPassiveMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the default passive mode on the interfaces
*
* @param    passiveMode @b{(input)} passive mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapPassiveModeSet(L7_BOOL passiveMode)
{
  pOspfMapCfgData->rtr.defaultPassiveMode = passiveMode;
  ospfDataChangedSet(__FUNCTION__);

  return ospfMapPassiveModeApply(passiveMode);
}

/*********************************************************************
* @purpose  Set the maximum number of paths that OSPF can report for a given 
*                   destination.
*
* @param   maxPaths @b{(input)} the maximum number of paths allowed
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if maxPaths is out of range
*
* @notes    maxPaths may be from 1 to L7_RT_MAX_EQUAL_COST_ROUTES, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapMaxPathsSet(L7_uint32 maxPaths)
{
    if (maxPaths == pOspfMapCfgData->rtr.maxPaths)
        return L7_SUCCESS;   /* no change */

    pOspfMapCfgData->rtr.maxPaths = maxPaths;
    ospfDataChangedSet(__FUNCTION__);
    
    /* Tell protocol and poke OSPF to recompute all routes */
    return ospfMapMaxPathsApply(maxPaths);
}

/*********************************************************************
* @purpose  Get the maximum number of paths that OSPF can report for a given 
*                   destination.
*
* @param   maxPaths @b{(output)} the maximum number of paths allowed
* 
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if maxPaths is NULL
*
* @notes    maxPaths may be from 1 to L7_RT_MAX_EQUAL_COST_ROUTES, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapMaxPathsGet(L7_uint32 *maxPaths)
{
    if (maxPaths)
    {
        *maxPaths = pOspfMapCfgData->rtr.maxPaths;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the autocost reference bandwidth for OSPF links
*
* @param    autoCostRefBw @b{(input)} the autoCost reference bandwidth  allowed
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if autoCostRefBw is out of range
*
* @notes    autoCostRefBw  may be from L7_OSPF_MIN_REF_BW to L7_OSPF_MAX_REF_BW, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapAutoCostRefBwSet(L7_uint32 autoCostRefBw)
{
  if (autoCostRefBw == pOspfMapCfgData->rtr.autoCostRefBw)
    return L7_SUCCESS;   /* no change */

  pOspfMapCfgData->rtr.autoCostRefBw = autoCostRefBw;
  ospfDataChangedSet(__FUNCTION__);

  /* Tell protocol and poke OSPF to recompute cost */
  return ospfMapAutoCostRefBwApply();
}

/*********************************************************************
* @purpose  Get the autocost reference bandwidth for OSPF links
*
* @param    autoCostRefBw @b{(output)} the autocost referencebandwidth allowed
*
* @returns  L7_SUCCESS if configuration is successfully stored
* @returns  L7_FAILURE if autoCostRefBw is NULL
*
* @notes    autoCostRefBw  may be from L7_OSPF_MIN_REF_BW to L7_OSPF_MAX_REF_BW, inclusive
*
* @end
*********************************************************************/
L7_RC_t ospfMapAutoCostRefBwGet(L7_uint32 *autoCostRefBw)
{
  if (autoCostRefBw)
  {
    *autoCostRefBw= pOspfMapCfgData->rtr.autoCostRefBw;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the configured stub router advertisement mode.
*
* @param    stubRtrMode     @b{(input)} the configured stub router mode
* @param    startupDuration @b{(input)} If stubRtrMode is startup, how long
*                                       OSPF stays in stub router mode on 
*                                       startup (seconds)
*
* @returns  L7_SUCCESS if configuration is successfully stored
*           L7_ERROR if the startup duration is out of range
*           L7_FAILURE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubRtrModeSet(OSPF_STUB_ROUTER_CFG_t stubRtrMode, 
                              L7_uint32 startupDuration)
{
  if ((pOspfMapCfgData->rtr.stubRtrMode == stubRtrMode) &&
      ((stubRtrMode != OSPF_STUB_RTR_CFG_STARTUP) ||
       (startupDuration == pOspfMapCfgData->rtr.stubRtrStartupTime)))
  {
    /* no change */
    return L7_SUCCESS;
  }

  if (stubRtrMode == OSPF_STUB_RTR_CFG_STARTUP)
  {
    /* Enforce range on startup duration */
    if ((startupDuration < L7_OSPF_STUB_ROUTER_DURATION_MIN) || 
        (startupDuration > L7_OSPF_STUB_ROUTER_DURATION_MAX))
    {
      return L7_ERROR;
    }
  }

  pOspfMapCfgData->rtr.stubRtrMode = stubRtrMode;

  /* Only set the startup time if the new mode is stub router on startup. */
  if (stubRtrMode == OSPF_STUB_RTR_CFG_STARTUP)
    pOspfMapCfgData->rtr.stubRtrStartupTime = startupDuration;
  else
    pOspfMapCfgData->rtr.stubRtrStartupTime = 0;

  pOspfMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  /* Return success even if apply fails */
  ospfMapStubRtrModeApply(pOspfMapCfgData->rtr.stubRtrMode, 
                          pOspfMapCfgData->rtr.stubRtrStartupTime);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the configured stub router advertisement mode.
*
* @param    overrideSummaryMetric   @b{(input)} if L7_ENABLE, override the metric
*                                               in summary LSAs when in stub router mode.
* @param    stubRtrSummaryMetric    @b{(input)} metric value
*
* @returns  L7_SUCCESS if configuration is successfully stored
*           L7_ERROR if the metric is out of range
*           L7_FAILURE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfStubRtrSumLsaMetricOverride(L7_uint32 overrideSummaryMetric,
                                        L7_uint32 stubRtrSummaryMetric)
{
  if ((overrideSummaryMetric == pOspfMapCfgData->rtr.overrideSummaryMetric) &&
      (stubRtrSummaryMetric == pOspfMapCfgData->rtr.stubRtrSummaryMetric))
  {
    /* no change */
    return L7_SUCCESS;
  }

  if ((overrideSummaryMetric != L7_ENABLE) && (overrideSummaryMetric != L7_DISABLE))
  {
    return L7_ERROR;
  }
  if ((overrideSummaryMetric == L7_ENABLE) &&
      ((stubRtrSummaryMetric < L7_OSPF_STUB_ROUTER_SUMLSA_MET_MIN) ||
       (stubRtrSummaryMetric > L7_OSPF_STUB_ROUTER_SUMLSA_MET_MAX)))
  {
    return L7_ERROR;
  }

  pOspfMapCfgData->rtr.overrideSummaryMetric = overrideSummaryMetric;
  pOspfMapCfgData->rtr.stubRtrSummaryMetric = stubRtrSummaryMetric;
  pOspfMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  ospfMapStubRtrSummaryMetricApply(overrideSummaryMetric, stubRtrSummaryMetric);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the configured stub router advertisement mode.
*
* @param    stubRtrMode     @b{(output)} the configured stub router mode
* @param    startupDuration @b{(output)} If the configured stub router mode is
*                                        stub router at startup, the startup 
*                                        time in seconds is set here. May be NULL
*                                        if caller doesn't care about startup time.
* @param    overrideSummaryMetric @b{(output)}  L7_ENABLE or L7_DISABLE
* @param    summaryMetric   @b{(output)} Metric to set in summary LSAs in stub 
*                                        router mode, if overriding summary metric
*
* @returns  L7_SUCCESS if configuration is successfully retrieved
*           L7_FAILURE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubRtrModeGet(OSPF_STUB_ROUTER_CFG_t *stubRtrMode, 
                              L7_uint32 *startupDuration, 
                              L7_uint32 *overrideSummaryMetric, 
                              L7_uint32 *summaryMetric)
{
  if (stubRtrMode)
  {
    *stubRtrMode = pOspfMapCfgData->rtr.stubRtrMode;
    if (startupDuration && 
        (pOspfMapCfgData->rtr.stubRtrMode == OSPF_STUB_RTR_CFG_STARTUP))
    {
      *startupDuration = pOspfMapCfgData->rtr.stubRtrStartupTime;
    }
    if (overrideSummaryMetric)
    {
      *overrideSummaryMetric = pOspfMapCfgData->rtr.overrideSummaryMetric;
    }
    if (summaryMetric && (pOspfMapCfgData->rtr.overrideSummaryMetric == L7_ENABLE))
    {
      *summaryMetric = pOspfMapCfgData->rtr.stubRtrSummaryMetric;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the router's support for demand routing.
*          
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports demand routing
*           L7_FALSE: The router does not support demand routing
*
*           Virata Note: OSPF++ does not support demand routing
*           extensions, this field should always be set to FALSE
*
* @end
*********************************************************************/
L7_RC_t ospfMapDemandExtensionsGet ( L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenDemandExtensionsGet(val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the router's support for demand routing.
*          
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports demand routing
*           L7_FALSE: The router does not support demand routing
*
*           Virata Note: OSPF++ does not support demand routing
*           extensions, this field should always be set to FALSE
*
* @end
*********************************************************************/
L7_RC_t ospfMapDemandExtensionsSet ( L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenDemandExtensionsSet(val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain RFC 1583 Compatibility Mode
*
* @param    mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " If enabled, preference rules remain those as defined by
*             RFC 1583.  If disabled, preference rules are as stated 
*             in Section 16.4.1 of the OSPFv2 standard. When disabled,
*             greater protection is provided against routing loops."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRfc1583CompatibilityGet(L7_uint32 *mode)
{
  *mode = pOspfMapCfgData->rtr.rfc1583Compatibility;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures if Ospf is compatible with RFC 1583
*
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " If enabled, preference rules remain those as defined by
*             RFC 1583.  If disabled, preference rules are as stated 
*             in Section 16.4.1 of the OSPFv2 standard. When disabled,
*             greater protection is provided against routing loops."
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRfc1583CompatibilitySet(L7_uint32 mode)
{
  L7_uint32 prevMode;

  /* If the previous configuration is the same as that specified
  ** by the user return successfully 
  */
  if(ospfMapRfc1583CompatibilityGet(&prevMode) == L7_SUCCESS &&
    (prevMode == mode))
    return L7_SUCCESS;

  /* Save the configuration */
  pOspfMapCfgData->rtr.rfc1583Compatibility = mode;
  ospfDataChangedSet(__FUNCTION__);


  /* Turn on Rfc1583Compatibility flag in the vendor code */
  return ospfMapRfc1583CompatibilityApply(mode);
}

/*********************************************************************
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTrapModeSet(L7_uint32 trapType)
{                         
  /* Return success if OSPF has not been initialized. 
     This allows for dynamic configurability of OSPF. */
  if(ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapVendExtenTrapModeSet(trapType);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the default metric for OSPF. 
*
* @param    defMetric - new value for the default metric. 
*
* @returns  L7_SUCCESS if default metric is stored.
* @returns  L7_FAILURE if defMetric is out of range.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultMetricSet(L7_uint32 defMetric)
{               
    L7_BOOL change = L7_FALSE;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    if ((defMetric < L7_OSPF_DEFAULT_METRIC_MIN) || 
        (defMetric > L7_OSPF_DEFAULT_METRIC_MAX)) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }

    if (pOspfMapCfgData->rtr.defaultMetric != defMetric) {
        pOspfMapCfgData->rtr.defaultMetric = defMetric;
        change = L7_TRUE;
    }
    if (pOspfMapCfgData->rtr.defMetConfigured == L7_FALSE) {
        pOspfMapCfgData->rtr.defMetConfigured = L7_TRUE;
        change = L7_TRUE;
    }
    if (change) {
        ospfDataChangedSet(__FUNCTION__);
        ospfMapExtenRedistReevaluate();
    
        /* If this router originates a default route, this change
            may change its metric. */
        ospfMapDefaultRouteMetricChange();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear the default metric for OSPF, leaving no default 
*           metric configured.
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultMetricClear()
{  
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (pOspfMapCfgData->rtr.defMetConfigured) {
        pOspfMapCfgData->rtr.defMetConfigured = L7_FALSE;
        ospfDataChangedSet(__FUNCTION__);
        /* Routes OSPF is redistributing could be using the default 
            metric. So we need to update the metrics. */
        ospfMapExtenRedistReevaluate();

        /* If this router originates a default route, this change
            may change its metric. */
        ospfMapDefaultRouteMetricChange();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the default metric for OSPF. 
*
* @param    defMetric - return value. Only set if return value
*                       is L7_SUCCESS.
*
* @returns  L7_SUCCESS if a default metric is configured.
* @returns  L7_NOT_EXIST if no default metric is configured.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultMetricGet(L7_uint32 *defMetric)
{              
    L7_RC_t rc;
    if (defMetric == L7_NULLPTR) {
        return L7_FAILURE;
    }
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (pOspfMapCfgData->rtr.defMetConfigured) {
        *defMetric = pOspfMapCfgData->rtr.defaultMetric;
        rc = L7_SUCCESS;
    }
    else {
        rc = L7_NOT_EXIST;
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return rc;
}

/*********************************************************************
* @purpose  Set configuration parameters for origination of a default route.
*
* @param    originateDefault - L7_TRUE if OSPF should originate a 
*                              default route. Default is L7_FALSE.
* @param    always - L7_TRUE if OSPF should originate a default route
*                    even if the forwarding table does not contain a
*                    default route. Default is L7_FALSE.
* @param    defRouteMetric - Metric for default route. If no metric is
*                            configured, set to 
*                            FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC. Valid range
*                            is L7_OSPF_DEFAULT_METRIC_MIN to 
*                            L7_OSPF_DEFAULT_METRIC_MAX.
* @param    defRouteMetType - OSPF metric type for default route. If not 
*                             configured, set to 
*                             FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE.
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteSet(L7_BOOL originateDefault,
                               L7_BOOL always,
                               L7_int32 defRouteMetric,
                               L7_OSPF_EXT_METRIC_TYPES_t defRouteMetType)
{  
    L7_BOOL change = L7_FALSE;
    L7_BOOL wasAsbr;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    wasAsbr = ospfMapIsAsbr();

    if (pOspfMapCfgData->defRouteCfg.origDefRoute != originateDefault) {
        pOspfMapCfgData->defRouteCfg.origDefRoute = originateDefault;
        change = L7_TRUE;
    }
    if (pOspfMapCfgData->defRouteCfg.always != always) {
        pOspfMapCfgData->defRouteCfg.always = always;
        change = L7_TRUE;
    }
    if (pOspfMapCfgData->defRouteCfg.metric != defRouteMetric) {
        pOspfMapCfgData->defRouteCfg.metric = defRouteMetric;
        change = L7_TRUE;
    }
    if (pOspfMapCfgData->defRouteCfg.metType != defRouteMetType) {
        pOspfMapCfgData->defRouteCfg.metType = defRouteMetType;
        change = L7_TRUE;
    }

    if (change) {
      /* if OSPF was not but is now an ASBR, then register with RTO for 
       * best route changes. */
      if (!wasAsbr && ospfMapIsAsbr())
        ospfMapAsbrStatusApply(L7_ENABLE);
      else if (wasAsbr && !ospfMapIsAsbr())
        ospfMapAsbrStatusApply(L7_DISABLE);

      ospfDataChangedSet(__FUNCTION__);
      ospfMapDefaultRouteApply();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Specify whether OSPF should originate a default route. 
*
* @param    originateDefault - L7_TRUE if OSPF should originate a 
*                              default route. Default is L7_FALSE.
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteOrigSet(L7_BOOL originateDefault)
{
    L7_BOOL wasAsbr;
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    wasAsbr = ospfMapIsAsbr();

    if (pOspfMapCfgData->defRouteCfg.origDefRoute != originateDefault) {

        pOspfMapCfgData->defRouteCfg.origDefRoute = originateDefault;      
        ospfDataChangedSet(__FUNCTION__);

        /* if OSPF was not but is now an ASBR, then register with RTO for 
         * best route changes. */
        if (!wasAsbr && ospfMapIsAsbr())
          ospfMapAsbrStatusApply(L7_ENABLE);
        else if (wasAsbr && !ospfMapIsAsbr())
          ospfMapAsbrStatusApply(L7_DISABLE);

        ospfMapDefaultRouteApply();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Revert the default route origination configuration to
*           the defaults. 
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    Defaults are as follows:
*              originateDefault - L7_FALSE
*              always - L7_FALSE
*              defRouteMetric - not configured
*              defRouteMetType - type 2 external
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteOrigRevert()
{
    L7_BOOL wasAsbr;
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    wasAsbr = ospfMapIsAsbr();

    ospfDefaultRouteCfgInit(&pOspfMapCfgData->defRouteCfg);  
    /* if OSPF was not but is now an ASBR, then register with RTO for 
     * best route changes. */
    if (!wasAsbr && ospfMapIsAsbr())
      ospfMapAsbrStatusApply(L7_ENABLE);
    else if (wasAsbr && !ospfMapIsAsbr())
      ospfMapAsbrStatusApply(L7_DISABLE);
    ospfMapDefaultRouteApply();

    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get whether OSPF should originate a default route. 
*
* @param    originateDefault - return value.
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteOrigGet(L7_BOOL *originateDefault)
{
    if (originateDefault == L7_NULLPTR) {
        return L7_FAILURE;
    }
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    *originateDefault = pOspfMapCfgData->defRouteCfg.origDefRoute;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Specify whether OSPF's origination of a default route 
*           is conditioned on the presence of a default route in 
*           the forwarding table. 
*
* @param    always - L7_TRUE if OSPF should originate a default route
*                    even if the forwarding table does not contain a
*                    default route. Default is L7_FALSE.
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteAlwaysSet(L7_BOOL always)
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (pOspfMapCfgData->defRouteCfg.always != always) {
        pOspfMapCfgData->defRouteCfg.always = always;
        ospfDataChangedSet(__FUNCTION__);
        ospfMapDefaultRouteApply();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get whether OSPF's origination of a default route 
*           is conditioned on the presence of a default route in 
*           the forwarding table. 
*
* @param    always - return value.
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteAlwaysGet(L7_BOOL *always)
{
    if (always == L7_NULLPTR) {
        return L7_FAILURE;
    }
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    *always = pOspfMapCfgData->defRouteCfg.always;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Specify the metric advertised for the default route. 
*
* @param    defRouteMetric - Metric for default route. Valid range
*                            is L7_OSPF_DEFAULT_METRIC_MIN to 
*                            L7_OSPF_DEFAULT_METRIC_MAX. 
*
* @returns  L7_SUCCESS if successful.
* @returns  L7_FAILURE if defRouteMetric is out of range.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteMetricSet(L7_int32 defRouteMetric)
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if ((defRouteMetric < L7_OSPF_DEFAULT_METRIC_MIN) || 
        (defRouteMetric > L7_OSPF_DEFAULT_METRIC_MAX)) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    if (pOspfMapCfgData->defRouteCfg.metric != defRouteMetric) {
        pOspfMapCfgData->defRouteCfg.metric = defRouteMetric;        
        ospfDataChangedSet(__FUNCTION__);
        ospfMapDefaultRouteApply();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear the default route metric for OSPF. Call this function
*           if the user removes the metric option for default route 
*           origination.  
*
* @param    none 
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteMetricClear()
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (pOspfMapCfgData->defRouteCfg.metric != FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC) {
        pOspfMapCfgData->defRouteCfg.metric = FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC;        
        ospfDataChangedSet(__FUNCTION__);
        ospfMapDefaultRouteApply();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the metric advertised for the default route. 
*
* @param    defRouteMetric - return value. Only valid if return 
*                            value is L7_SUCCESS.
*
* @returns  L7_SUCCESS if a default route metric is configured.
* @returns  L7_NOT_EXIST if a default route metric is not configured.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteMetricGet(L7_int32 *defRouteMetric)
{
    if (defRouteMetric == L7_NULLPTR) {
        return L7_FAILURE;
    }
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (pOspfMapCfgData->defRouteCfg.metric == 
        FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_NOT_EXIST;
    }
    *defRouteMetric = pOspfMapCfgData->defRouteCfg.metric;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Specify the metric type for advertisement of the default route. 
*
* @param    defRouteMetType - OSPF metric type for default route. 
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteMetricTypeSet(L7_OSPF_EXT_METRIC_TYPES_t 
                                         defRouteMetType)
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (pOspfMapCfgData->defRouteCfg.metType != defRouteMetType) {
        pOspfMapCfgData->defRouteCfg.metType = defRouteMetType;
        ospfDataChangedSet(__FUNCTION__);
        ospfMapDefaultRouteApply();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Revert to the default metric type for advertisement 
*           of the default route. Call this function when the user 
*           removes the configuration of the metric type.
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteMetricTypeRevert()
{
    return ospfMapDefaultRouteMetricTypeSet(FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE);
}

/*********************************************************************
* @purpose  Get the metric type for advertisement of the default route. 
*
* @param    defRouteMetType - return value 
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDefaultRouteMetricTypeGet(L7_OSPF_EXT_METRIC_TYPES_t 
                                         *defRouteMetType)
{
    if (defRouteMetType == L7_NULLPTR) {
        return L7_FAILURE;
    }
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    *defRouteMetType = pOspfMapCfgData->defRouteCfg.metType;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get SPF stats
*
* @param    maxStats     Maximum number of stats to be reported
* @param    spfStats     Output buffer. Large enough for maxStats.
*
* @returns  Number of stats actually reported. Number available could
*           be less than number requested.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapSpfStatsGet(L7_uint32 maxStats, L7_OspfSpfStats_t *spfStats)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenSpfStatsGet(maxStats, spfStats);
  return 0;
}

/*********************************************************************
* @purpose  Get configuration for support of opaque LSAs.
*
* @param    opaqueEnabled - L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS 
* @returns  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfOpaqueLsaSupportGet(L7_uint32 *opaqueEnabled)
{
    *opaqueEnabled = pOspfMapCfgData->rtr.opaqueLSAMode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable support for opaque LSAs.
*
* @param    opaqueEnabled - L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS if support is present.
* @returns  L7_FAILURE if support is not present.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfOpaqueLsaSupportSet(L7_uint32 opaqueEnabled)
{
  pOspfMapCfgData->rtr.opaqueLSAMode = opaqueEnabled;
  ospfDataChangedSet(__FUNCTION__);

  return ospfMapExtenOspfOpaqueCapabilitySet(opaqueEnabled);
}

/*********************************************************************
* @purpose  Get the process's Area Opaque Link State Database entry.
*
* @param    AreaId      area Id
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Advertizing RouterId
* @param    p_Lsa       pointer to Opaque Lsa Structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAreaOpaqueLsdbEntryGet( L7_uint32 areaId, L7_uint32 type,
                              L7_uint32 lsid, L7_uint32 routerId,
                              L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenAreaOpaqueLsdbEntryGet(areaId,type,lsid,routerId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the process's Area Opaque Link State Database next entry.
*
* @param    AreaId      area Id
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Advertizing RouterId
* @param    p_Lsa       pointer to Opaque Lsa Structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAreaOpaqueLsdbEntryNext( L7_uint32 *areaId, L7_int32 *type,
                              L7_uint32 *lsid, L7_uint32 *routerId,
                              L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenAreaOpaqueLsdbEntryNext(areaId,type,lsid,routerId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    AreaId      Area
* @param    Type        Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapType10LsaSequenceGet ( L7_uint32 areaId, L7_int32 type,
                                 L7_uint32 lsid, L7_uint32 routerId,
                                 L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenType10LsaSequenceGet(areaId, type, lsid, routerId, val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Area Opaque LSA Age for the specified Area
*
* @param    areaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising RouterId
* @param    *val        seconds

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 ospfMapType10LsaAgeGet(L7_uint32 areaId, L7_int32 type,
                                 L7_uint32 lsid, L7_uint32 routerId,
                                 L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType10LsaAgeGet(areaId, type, lsid, routerId, val);
}

/*********************************************************************
* @purpose  Get the checksum of the Area Opaque LSA.
*
* @param    areaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising RouterId
* @param    *val        Checksum

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 ospfMapType10LsaChecksumGet(L7_uint32 areaId, L7_int32 type,
                                 L7_uint32 lsid, L7_uint32 routerId,
                                 L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType10LsaChecksumGet(areaId, type, lsid, routerId, val);
}

/*********************************************************************
* @purpose  Get the Type-10 LSA Advertisement.
*
* @param    areaId      area
* @param    Type        type
* @param    Lsid        Link State id
* @param    RouterId    Advertising RouterId
* @param    **buf       Buffer to store LSA Advertisement
* @param    len         length of LSA Advertisement

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 ospfMapType10LsaAdvertisementGet(L7_uint32 areaId, L7_int32 type,
                                 L7_uint32 lsid, L7_uint32 routerId,
                                 L7_char8 **buf, L7_uint32 *len)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType10LsaAdvertisementGet(areaId, type, 
                                               lsid, routerId, buf, len);
}

/*********************************************************************
* @purpose  Get the process's Link Opaque Link State Database entry.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfLinkOpaqueLsdbEntryGet(L7_uint32 ipAddr, L7_int32 ifIndex,
                              L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenLinkOpaqueLsdbEntryGet(ipAddr,ifIndex,type,lsId,rtrId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the process's Link Opaque Link State Database next entry.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfLinkOpaqueLsdbEntryNext(L7_uint32 *ipAddr, L7_int32 *ifIndex,
                                L7_int32 *type, L7_uint32 *lsId,
                                L7_uint32 *rtrId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa) 
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenLinkOpaqueLsdbEntryNext(ipAddr,ifIndex,type,lsId,rtrId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapType9LsaSequenceGet(L7_uint32 ipAddr, L7_int32 ifIndex,
                                   L7_int32 type, L7_uint32 lsId,
                                   L7_uint32 rtrId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenType9LsaSequenceGet(ipAddr,ifIndex,type,lsId,rtrId,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Link Opaque LSA Age for the specified Link
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    *val        LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapType9LsaAgeGet(L7_uint32 ipAddr, L7_int32 ifIndex,
                              L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_uint32 *val )
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType9LsaAgeGet(ipAddr,ifIndex,type,lsId,rtrId,val);
}

/*********************************************************************
* @purpose  Get the checksum of the Link Opaque LSA.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    *val        Checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapType9LsaChecksumGet(L7_uint32 ipAddr, L7_int32 ifIndex,
                              L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType9LsaChecksumGet(ipAddr,ifIndex,type,lsId,rtrId,val);
}

/*********************************************************************
* @purpose  Get the LSA Advertisement of the Link Opaque LSA.
*
* @param    ipAddr      Link IP Address
* @param    ifIndex     Link Interface Index
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    buf         Buffer to store LSA Advertisement
* @param    len         length of LSA Advertisement
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 ospfMapType9LsaAdvertisementGet(L7_uint32 ipAddr, L7_int32 ifIndex,
                                L7_int32 type, L7_uint32 lsId, 
                                L7_uint32 rtrId, L7_char8 **buf, L7_uint32 *len)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType9LsaAdvertisementGet(ipAddr,ifIndex,type, 
                                               lsId,rtrId,buf,len);
}

/*********************************************************************
* @purpose  Get the process's AsOpaque Link State Database entry.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAsOpaqueLsdbEntryGet(L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenAsOpaqueLsdbEntryGet(type,lsId,rtrId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the process's AsOpaque Link State Database next entry.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    p_Lsa       Pointer to LSA.

* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAsOpaqueLsdbEntryNext(L7_int32 *type, L7_uint32 *lsId,
                                L7_uint32 *rtrId,
                                L7_ospfOpaqueLsdbEntry_t *p_Lsa) 
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenAsOpaqueLsdbEntryNext(type,lsId,rtrId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapType11LsaSequenceGet(L7_int32 type, L7_uint32 lsId,
                                   L7_uint32 rtrId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenType11LsaSequenceGet(type,lsId,rtrId,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf AsOpaque LSA Age for the specified Link
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    *val        LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapType11LsaAgeGet(L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_uint32 *val )
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType11LsaAgeGet(type,lsId,rtrId,val);
}

/*********************************************************************
* @purpose  Get the checksum of the Link Opaque LSA.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    *val        Checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t ospfMapType11LsaChecksumGet(L7_int32 type, L7_uint32 lsId,
                              L7_uint32 rtrId, L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType11LsaChecksumGet(type,lsId,rtrId,val);
}

/*********************************************************************
* @purpose  Get the LSA Advertisement of the Link Opaque LSA.
*
* @param    type        Type
* @param    lsId        Link State id
* @param    rtrId       Advertising RouterId
* @param    buf         Buffer to store LSA Advertisement
* @param    len         length of LSA Advertisement
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   
*
* @end
*********************************************************************/
L7_uint32 ospfMapType11LsaAdvertisementGet(L7_int32 type, L7_uint32 lsId, 
                                L7_uint32 rtrId, L7_char8 **buf, L7_uint32 *len)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenType11LsaAdvertisementGet(type,lsId,rtrId,buf,len);
}

/*********************************************************************
* @purpose  Get the checksum SUM of the AS Opaque LSA.
*
* @param    *val        Checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfASOpaqueLSAChecksumSumGet(L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenASOpaqueLSAChecksumSumGet(val);
}

/*********************************************************************
 * @purpose  Is OSPF in stub router mode?
 *
 * @param    *isStubRtr        output value
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapIsStubRtrGet(L7_BOOL *isStubRtr)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenIsStubRtrGet(isStubRtr);
}

/*********************************************************************
 * @purpose  Is OSPF in external LSDB overflow?
 *
 * @param    *extLsdbOverflow        output value
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtLsdbOverflowGet(L7_BOOL *extLsdbOverflow)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenExtLsdbOverflowGet(extLsdbOverflow);
}

/*********************************************************************
* @purpose  To decode OSPF LSA's.
*
* @param    lsaInfo @b{(input)} L7_char8
* @param    output  @b{(input)} outputFuncPtr
* @param    context @b{(input)} context
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapLsaDecode(L7_char8 *lsaInfo, outputFuncPtr output,
    void * context)
{
  if(ospfMapOspfInitialized() == L7_TRUE)
    return (ospfLsaDecode(lsaInfo, output, context));

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Clear OSPF statistics
*
* @param    void
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapCountersClear(void)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenCountersClear();
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear stub router mode if entered as a result of a 
*           resource limitation or at startup.
*
* @param    void
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubRouterClear(void)
{
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
    return ospfMapExtenStubRouterClear();
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the graceful restart helper support configuration
*
* @param    helperSupport @b{(input)} whether this router will be a helpful neighbor
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfHelperSupportSet(OSPF_HELPER_SUPPORT_t helperSupport)
{
  if (pOspfMapCfgData->rtr.helperSupport == helperSupport)
    return L7_SUCCESS;

  pOspfMapCfgData->rtr.helperSupport = helperSupport;
  ospfDataChangedSet(__FUNCTION__);

  ospfMapExtenHelperSupportSet(helperSupport);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the graceful restart helper support configuration
*
* @param    helperSupport @b{(output)} whether this router will be a helpful neighbor
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfHelperSupportGet(OSPF_HELPER_SUPPORT_t *helperSupport)
{
  if (helperSupport)
  {
    *helperSupport = pOspfMapCfgData->rtr.helperSupport;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Configure whether OSPF should exit GR on a toopology change.
*
* @param    strictLsaChecking @b{(input)} L7_TRUE if topo change terminates GR
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfStrictLsaCheckingSet(L7_BOOL strictLsaChecking)
{
  if (pOspfMapCfgData->rtr.strictLsaChecking == strictLsaChecking)
    return L7_SUCCESS;

  pOspfMapCfgData->rtr.strictLsaChecking = strictLsaChecking;
  ospfDataChangedSet(__FUNCTION__);

  ospfMapExtenStrictLsaCheckingSet(strictLsaChecking);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Ask if OSPF is configured to exit GR on a topology change.
*
* @param    strictLsaChecking @b{(output)} L7_TRUE if topo change terminates GR
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfStrictLsaCheckingGet(L7_BOOL *strictLsaChecking)
{
  if (strictLsaChecking)
  {
    *strictLsaChecking = pOspfMapCfgData->rtr.strictLsaChecking;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the graceful restart helper status for the 
*           specified neighbor router on the specified interface 
*          
* @param    intIfNum   the interface number
* @param    nbrIpAddr  IPv4 address of the neighbour
* @param    nbrStatus  (output) status information      
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t ospfHelpfulNbrStatusGet(L7_uint32 intIfNum, L7_uint32 nbrIpAddr, 
                                L7_ospfHelpfulNbrStatus_t *nbrStatus)
{
  return ospfMapExtenHelpfulNbrStatusGet(intIfNum, nbrIpAddr, nbrStatus);
}


/* End Function Declaration */

