/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_util.c
*
* @purpose   MGMD Utility Functions
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create    02/07/2002
*
* @author    ramakrishna
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"

/*
**********************************************************************
*                    API FUNCTIONS  -  GLOBAL CONFIG
**********************************************************************
*/
/*********************************************************************
* @purpose  Enable the Mgmd on the router
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdAdminModeEnable(mgmdMapCB_t *mgmdMapCbPtr, L7_BOOL doInit)
{
   L7_uint32 intIfNum = L7_NULL;
   L7_uint32 rtrIfNum = L7_NULL;
   L7_uint32 mode = L7_NULL; 

   MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
 
   mgmdMapExtenMgmdAdminModeSet(mgmdMapCbPtr, L7_ENABLE, doInit);
   mgmdMapMgmdRouterAlertCheckApply(mgmdMapCbPtr, mgmdMapCbPtr->mgmdMapCfgData.checkRtrAlert);
   
   /* Enable all interfaces that have been enabled for MGMD */
   for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
   {
     if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                      rtrIfNum, &intIfNum) != L7_SUCCESS)
     {
       continue;
     }
     /* Check if mgmd is enabled on the interface */
     if ((mgmdMapInterfaceModeGet(mgmdMapCbPtr->familyType, intIfNum, 
          &mode) == L7_SUCCESS) && mode == L7_ENABLE)
     {
       if(mgmdMapIntfModeEnable(mgmdMapCbPtr, intIfNum,
                                MGMD_ROUTER_INTERFACE) != L7_SUCCESS)
       {
         MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nCould not" 
                        "enable MGMD interface \n");
       }
     }
     else
     {
       if ((mgmdMapProxyInterfaceModeGet(mgmdMapCbPtr->familyType, 
           intIfNum, &mode) == L7_SUCCESS) && mode == L7_ENABLE)
       {
         if(mgmdMapIntfModeEnable(mgmdMapCbPtr, intIfNum,
                                  MGMD_PROXY_INTERFACE) != L7_SUCCESS)
         {
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nCould not"
           "enable MGMD-PROXY interface \n");
         }
       }
     }
   }
   MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nCode Flow: SUCCESS\n");
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable the Mgmd on router
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdAdminModeDisable(mgmdMapCB_t *mgmdMapCbPtr, L7_BOOL doInit)
{ 
    L7_uint32 intIfNum = L7_NULL;
    L7_uint32 rtrIfNum = L7_NULL;
    L7_uint32 mode = L7_NULL; 

    /*------------------------------------------------------------------------------*/
    /* Disable router interfaces configured for MGMD */
    /*------------------------------------------------------------------------------*/
    for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
    {
      if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                      rtrIfNum, &intIfNum) != L7_SUCCESS)
      {
        continue;
      }
      /* Check if mgmd is enabled on the interface */
      if ((mgmdMapInterfaceModeGet(mgmdMapCbPtr->familyType, 
          intIfNum, &mode) == L7_SUCCESS) && mode == L7_ENABLE)
      {
        if(mgmdMapIntfModeDisable(mgmdMapCbPtr, intIfNum,
                                  MGMD_ROUTER_INTERFACE) != L7_SUCCESS)
        {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Could not disable MGMD interface\n");
        }
      }
      else
      {
        if ((mgmdMapProxyInterfaceModeGet(mgmdMapCbPtr->familyType, 
            intIfNum, &mode)== L7_SUCCESS) && mode == L7_ENABLE)
        {
          if (mgmdMapIntfModeDisable(mgmdMapCbPtr, intIfNum,
                                     MGMD_PROXY_INTERFACE) != L7_SUCCESS)
          {
            MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Could not disable MGMD-PROXY interface\n");
          }
        }
      }
    }
    mgmdMapMgmdRouterAlertCheckApply(mgmdMapCbPtr, L7_FALSE);
    /* now disable the admin mode for MGMD */    
    return mgmdMapExtenMgmdAdminModeSet(mgmdMapCbPtr, L7_DISABLE, doInit);
}


/*
**********************************************************************
*                    API FUNCTIONS  -  INTERFACE CONFIG
**********************************************************************
*/


/*********************************************************************
* @purpose  Enables the MGMD for the specified interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfModeEnable(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                              L7_MGMD_INTF_MODE_t intfType)
{
  L7_uint32      rtrIfNum = L7_NULL;
  L7_inet_addr_t ipAddr;
  L7_uint32      adminMode = L7_NULL;
  L7_uint32      mcastMode = L7_NULL;
  mgmdIntfCfgData_t *intfCfg = L7_NULLPTR;
  L7_RC_t        rc = L7_FAILURE;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  inetAddressReset(&ipAddr);
  if (mgmdMapAdminModeGet(mgmdMapCbPtr->familyType, &adminMode) == L7_SUCCESS)
  {
    if (adminMode != L7_ENABLE)
    {
      return L7_SUCCESS;
    }
  }  

  mcastMapMcastAdminModeGet(&mcastMode);
  if(mcastMode == L7_DISABLE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, " MGMD : Mcast is not operational");
    return L7_SUCCESS;
  }
  if (mcastIpMapRtrAdminModeGet(mgmdMapCbPtr->familyType) != L7_ENABLE)
  {

    return L7_SUCCESS;
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nAdmin Mode enabled\n");
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  { 
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n The rtrIfNum(%d) is unnumbered interface", rtrIfNum);
    return L7_SUCCESS;
  }
  if (mgmdMapIntfIsActive(intIfNum) != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  /* These pre-conditions has to be present as the support for
     unnumbered interfaces is not defined for MGMD */
  if (mcastIpMapIsRtrIntfUnnumbered(mgmdMapCbPtr->familyType,rtrIfNum) == L7_TRUE)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n The rtrIfNum(%d) is unnumbered interface", rtrIfNum);
    return L7_SUCCESS;
  }
  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum, 
                                &intfCfg) != L7_TRUE)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,"Interface(%d) is not initialzed "\
                                       "for MGMD configuration\n", intIfNum);
    return L7_FAILURE;
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nCode Flow\n");
  
  /* If no IP address has been assigned yet, just return a success.
   * The enable will be completed when we get notified of an IP address add.
   */
  if ((mcastIpMapRtrIntfIpAddressGet(mgmdMapCbPtr->familyType, 
       intIfNum, &ipAddr) != L7_SUCCESS) 
      || (inetIsAddressZero(&ipAddr) == L7_TRUE) 
      || (mgmdMapSubsystemIpIntfExists(mgmdMapCbPtr->familyType, 
                                                     intIfNum) != L7_TRUE))

  {
    return L7_SUCCESS;
  }
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nCode Flow\n");

  /* if it is already initialized, just return */
  switch (intfType)
  {
    case MGMD_ROUTER_INTERFACE:
         MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "Code Flow\n");
         if (mgmdMapCbPtr->pMgmdInfo->mgmdIntfInfo[rtrIfNum].mgmdInitialized == L7_TRUE)
         {
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, 
                          "Code Flow\n");
           return L7_SUCCESS;
         }
         break;

    case MGMD_PROXY_INTERFACE:
         MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,
                        "Code Flow\n");
         if (mgmdMapCbPtr->pMgmdInfo->mgmdProxyEnabled == L7_ENABLE)
         {
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,
                          "Code Flow\n");
           return L7_SUCCESS;    
         }
         break;

    default: /* MGMD_INTERFACE_MODE_NULL */
         return L7_SUCCESS;    
  }/*End-of-Switch*/

  /* Call vendor extension function to enable the interface */
  rc = mgmdMapExtenIntfModeSet(mgmdMapCbPtr, intIfNum, L7_ENABLE, intfType);
  return rc;
}

/*********************************************************************
* @purpose  Disables the MGMD for the specified interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/


L7_RC_t mgmdMapIntfModeDisable(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                               L7_MGMD_INTF_MODE_t intfType)
{
  L7_uint32 rtrIfNum = L7_NULL;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,
                 "Function Entered\n");
  
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, 
                 "IntIfNum->%d\n", intIfNum);

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, 
                   "ERROR\n");
    return L7_SUCCESS;
  }
  
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
                 "Code Flow\n", __FUNCTION__, __LINE__);
  switch (intfType)
  {
    case MGMD_ROUTER_INTERFACE:
         MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, 
            "\nEntered in MGMD Router Interface\n");
         if (mgmdMapCbPtr->pMgmdInfo->mgmdIntfInfo[rtrIfNum].mgmdInitialized == L7_FALSE)
         {
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, 
                          "Mgmd Intf is not intialized.\n"); 
           return L7_SUCCESS;
         }
         break;

    case MGMD_PROXY_INTERFACE:
         MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n %s, %d :"
         "Entered in MGMD Proxy Router Interface\n", __FUNCTION__, __LINE__);
         if (mgmdMapCbPtr->pMgmdInfo->mgmdProxyEnabled == L7_DISABLE)
         {
           MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, 
                          "MgmdProxy Intf is not intialized.\n");
           return L7_SUCCESS;    
         }
         break;

    default:
         return L7_SUCCESS;    
  }/*End-of-Switch*/

  if(mgmdMapExtenIntfModeSet(mgmdMapCbPtr, intIfNum, L7_DISABLE, intfType) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, 
                   "ERROR DISABBLING MGMD \n");
    return L7_FAILURE;
  }


   MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n Code Flow\n");
   return L7_SUCCESS;
 
}

/*********************************************************************
* @purpose  Sets the MGMD Proxy version for the specified interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    version         @b{(input)} MGMD version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfVersionApply(mgmdMapCB_t *mgmdMapCbPtr,
                       L7_uint32 intIfNum, L7_uint32 version)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfVersionSet(mgmdMapCbPtr, intIfNum, version);
  }
  else if (mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType,
                                            intIfNum) == L7_SUCCESS)
  {
    return (mgmdMapProxyExtenIntfVersionSet(mgmdMapCbPtr, intIfNum, version));
  }
  else
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, " IGMP or Proxy is not enabled on that interface");
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Sets the MGMD Router-alert check
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    mode            @b{(input)} Router-alert check optional 
*                                       or mandatory
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdRouterAlertCheckApply(mgmdMapCB_t *mgmdMapCbPtr, L7_BOOL mode)
{
  if (mgmdMapMgmdInitialized(mgmdMapCbPtr) != L7_TRUE)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nMGMD not enabled \n");
    return L7_FAILURE;
  }
 
  return mgmdMapExtenMgmdRouterAlertCheckSet(mgmdMapCbPtr, mode);
}
/*********************************************************************
* @purpose  Updates vendor with configured robustness.
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    robustness  @b{(input)} configured robustness
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfRobustnessApply(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                                    L7_uint32 robustness)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfRobustnessSet(mgmdMapCbPtr, intIfNum, robustness);
  }
  else if (mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapProxyExtenIntfRobustnessSet(mgmdMapCbPtr, intIfNum,
                                                        robustness);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured queryInterval
*
* @param    mgmdMapCB      @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    queryInterval  @b{(input)} configured query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfQueryIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                       L7_uint32 intIfNum,
                                       L7_uint32 queryInterval)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfQueryIntervalSet(mgmdMapCbPtr, intIfNum, queryInterval);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured query Maximum response time
*
* @param    mgmdMapCB         @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    queryMaxRespTime  @b{(input)} configured query Maximum
*                                         response time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfQueryMaxRespTimeApply(mgmdMapCB_t *mgmdMapCbPtr, 
                                          L7_uint32 intIfNum,
                                          L7_uint32 queryMaxRespTime)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfQueryMaxRespTimeSet(mgmdMapCbPtr, intIfNum, queryMaxRespTime);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured startup Query Interval
*
* @param    mgmdMapCB            @b{(input)} Mapping Control Block.
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    startupQueryInterval @b{(input)} configured startup queryInterval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapIntfStartupQueryIntervalApply(mgmdMapCB_t *mgmdMapCbPtr, 
                                              L7_uint32 intIfNum,
                                              L7_uint32 startupQueryInterval)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfStartupQueryIntervalSet(mgmdMapCbPtr, intIfNum,
                                                   startupQueryInterval);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured startup Query count
*
* @param    mgmdMapCB         @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    startupQueryCount @b{(input)} configured startup query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfStartupQueryCountApply(mgmdMapCB_t *mgmdMapCbPtr, 
                                          L7_uint32 intIfNum,
                                          L7_uint32 startupQueryCount)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfStartupQueryCountSet(mgmdMapCbPtr, intIfNum,
                                                startupQueryCount);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured Last member query interval
*
* @param    mgmdMapCB            @b{(input)} Mapping Control Block.
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    lastMemQueryInterval @b{(input)} configured last member
*                                            query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfLastMemberQueryIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                                L7_uint32 intIfNum,
                                                L7_uint32 lastMemQueryInterval)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfLastMemberQueryIntervalSet(mgmdMapCbPtr, intIfNum,
                                                      lastMemQueryInterval);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured Last member query count
*
* @param    mgmdMapCB          @b{(input)} Mapping Control Block.
* @param    intIfNum           @b{(input)} Internal Interface Number
* @param    lastMemQueryCount  @b{(input)} configured last member
*                                          query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfLastMemberQueryCountApply(mgmdMapCB_t *mgmdMapCbPtr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 lastMemQueryCount)
{
  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapExtenIntfLastMemberQueryCountSet(mgmdMapCbPtr, intIfNum,
                                                   lastMemQueryCount);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured unsolicited report interval
*
* @param    mgmdMapCB                 @b{(input)} Mapping Control Block.
* @param    intIfNum                  @b{(input)} Internal Interface Number
* @param    unsolicitedReportInterval @b{(input)} configured unsolicited
*                                                 report interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is specific to Proxy interface and hence applied only if the
*           proxy is enabled on the given interface 
*
* @end
*********************************************************************/
L7_RC_t mgmdMapIntfUnsolicitedReportIntervalApply(mgmdMapCB_t *mgmdMapCbPtr,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32 unsolicitedReportInterval)
{
  if (mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType,intIfNum) == L7_SUCCESS)
  {
    return mgmdMapProxyExtenIntfUnsolicitedReportIntervalSet(mgmdMapCbPtr, intIfNum,
                                                        unsolicitedReportInterval);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates Proxy MRT table for interface down event
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMgmdProxyIntfDownMRTApply(mgmdMapCB_t *mgmdMapCbPtr, 
                                          L7_uint32 intIfNum)
{
  return (mgmdMapProxyExtenIntfDownMRTUpdate(mgmdMapCbPtr, intIfNum));
}

/*********************************************************************
* @purpose  Determine if the MGMD has been initialized
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdMapMgmdInitialized(mgmdMapCB_t *mgmdMapCbPtr)
{
  if (mgmdMapCbPtr->pMgmdInfo == L7_NULL)
  {
    /* MGMD component was not initialized during system initialization */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid build - MGMD component was not initialized.\n");
    return L7_FALSE;
  }
  return mgmdMapCbPtr->pMgmdInfo->initialized;
}

/*********************************************************************
* @purpose  Determines if the specified internal interface is 
*           currently active, meaning the link is up and
*           in forwarding state
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL mgmdMapIntfIsActive(L7_uint32 intIfNum)
{
  L7_uint32 activeState = L7_NULL;

  if ((nimGetIntfActiveState(intIfNum, &activeState) == L7_SUCCESS) &&
      (activeState == L7_ACTIVE))
  {
    return L7_TRUE;
  }
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                  "The internal interface is not active\n");
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if IP interface has been created and initialized
*           in the sub-system.
*
* @param    familyType @b{(input)} Address Family type
* @param    intIfNum   @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE
* @returns  L7_DISABLE

* @comments This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_BOOL  mgmdMapSubsystemIpIntfExists(L7_uchar8 familyType, 
                                      L7_uint32 intIfNum)
{
  L7_uint32 ifState;

  if (familyType == L7_AF_INET)
  {
    return ((ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
            (ifState == L7_ENABLE));
  }
  else
  { 
    return mgmdMapV6IpIntfExists(intIfNum);
  }
}

/*********************************************************************
* @purpose  To get the mapping layer CB based on family Type
*
* @param    familyType      @b{(input)} Address Family type.
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCtrlBlockGet(L7_uchar8 familyType, 
                            mgmdMapCB_t **mgmdMapCbPtr)
{
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n Function Entered\n");
  switch (familyType)
  {
    case L7_AF_INET:
      *mgmdMapCbPtr = &mgmdMapCB_g[MGMD_MAP_IPV4_CB];
      break;
    case L7_AF_INET6:
      if (mgmdMapV6CtrlBlockGet(mgmdMapCbPtr) != L7_SUCCESS)
      {
        *mgmdMapCbPtr = L7_NULLPTR;
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Family Type Not Supported\n"); 
        return L7_FAILURE;
      }
      break;
    default:
     MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid Family Type\n"); 
     return L7_FAILURE;
  }
  return L7_SUCCESS;
}
