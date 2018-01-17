/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_mgmd.c
*
* @purpose  Provide interface to  MGMD components
*
* @component unitmgr
*
* @comments
*
* @create
*
* @author Nitish
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_util_api.h"
#include "l7_mgmd_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#endif


#include "nimapi.h"
#include "mgmd_debug_api.h"
/*********************************************************************
* @purpose  Get the administrative mode of MGMD in the router.
*
* @param    UnitIndex   @b{(input)}  unit
* @param    family      @b{(input)}  family Type.
* @param    mode        @b{(output)} MGMD admin mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    The value 'enabled' denotes that the MGMD Object is active
*           on at least one interface. 'disabled' means disabled on all 
*           interfaces.
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdAdminModeGet(L7_uint32 UnitIndex, L7_uchar8 family,
                              L7_uint32* mode)
{
    return mgmdMapAdminModeGet(family, mode);
}

/*********************************************************************
* @purpose  Set the administrative mode of MGMD in the router.
*
* @param    UnitIndex   @b{(input)}  unit
* @param    family      @b{(input)}  family Type.
* @param    mode        @b{(input)}  MGMD Admin mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdAdminModeSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                              L7_uint32 mode)
{
    return mgmdMapAdminModeSet(family, mode);
}


/*********************************************************************
* @purpose  Get the Interface mode of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    mode            @b{(output)} Interface Mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceModeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                  L7_uint32 intIfNum, L7_uint32* mode)
{
    return mgmdMapInterfaceModeGet(family, intIfNum, mode);
}

/*********************************************************************
* @purpose  Get the Interface Operational State of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbMgmdInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                          L7_uchar8 family, L7_uint32 intIfNum)
{
    if (mgmdMapMgmdIntfInitialized(family, intIfNum) == L7_SUCCESS)
    {
      return L7_TRUE;
    }
    else
    {
      return L7_FALSE;
    }
}

/*********************************************************************
* @purpose  Sets whether the optional Router Alert field is required.
*
* @param    checkRtrAlert       @b{(input)}   True if Router Alert required
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdRouterAlertMandatorySet(L7_uint32 UnitIndex,
                                         L7_uchar8 family, L7_BOOL checkRtrAlert)
{
  return (mgmdMapRouterAlertMandatorySet(family, checkRtrAlert));
}

/*********************************************************************
* @purpose  Indicates whether the optional Router Alert field is required.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdRouterAlertMandatoryGet(L7_uint32 UnitIndex,
                                         L7_uchar8 family, L7_BOOL *checkRtrAlert)
{
  return (mgmdMapRouterAlertMandatoryGet(family, checkRtrAlert));
}

/*********************************************************************
* @purpose  Set the Interface mode of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    mode            @b{(input)}  Interface Mode
* @param    proxy_or_router @b{(input)}  proxy or router interface
* @param    errCode         @b{(output)}  error Code 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceModeSet(L7_uint32 UnitIndex, L7_uchar8 family, 
 L7_uint32 intIfNum, L7_uint32 mode,L7_MGMD_INTF_MODE_t proxy_or_router, 
 L7_MGMD_ERROR_CODE_t *errCode)
{
    return mgmdMapInterfaceModeSet(family, intIfNum, mode,proxy_or_router, 
                                   errCode);
}

/*********************************************************************
* @purpose  Gets the  MGMD Interface Startup Query interval
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    startupInterval @b{(output)} startup query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryIntervalGet(L7_uint32 UnitIndex, 
           L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *startupInterval)
{
    return mgmdMapInterfaceStartupQueryIntervalGet(family, intIfNum, 
                                                   startupInterval);
}

/*********************************************************************
* @purpose  Sets the  MGMD Interface Startup Query interval
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    startupInterval   @b{(input)}  startup query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryIntervalSet(L7_uint32 UnitIndex, 
              L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 startupInterval)
{
    return mgmdMapInterfaceStartupQueryIntervalSet(family, intIfNum, 
                                                   startupInterval);
}


/***********************************************************************
* @purpose  Gets Querier / Non-Querier Status of MGMD on this interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  family Type.
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    status                 @b{(output)} Enable / Disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierStatusGet(L7_uint32 UnitIndex, 
                      L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *status)
{
    return  mgmdMapInterfaceQuerierStatusGet(family, intIfNum, status);
}



/*********************************************************************
* @purpose  Gets the  MGMD Interface Startup Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    startupCount    @b{(output)}  startup query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryCountGet(L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *startupCount)
{
    return mgmdMapInterfaceStartupQueryCountGet(family, intIfNum, startupCount);
}


/*********************************************************************
* @purpose  Sets the  MGMD Interface Startup Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    startupCount    @b{(input)}  startup query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryCountSet(L7_uint32 UnitIndex, 
                L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 startupCount)
{
    return mgmdMapInterfaceStartupQueryCountSet(family, intIfNum, startupCount);
}

/*********************************************************************
* @purpose  Gets the  MGMD Interface last member Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    queryCount      @b{(output)}  last member query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdLastMembQueryCountGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                    L7_uint32 intIfNum, L7_uint32 *queryCount)
{
        return mgmdMapInterfaceLastMembQueryCountGet(family, intIfNum, queryCount);
}

/*********************************************************************
* @purpose  Sets the  MGMD Interface last member Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    queryCount      @b{(input)}  last member querry count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdLastMembQueryCountSet(L7_uint32 UnitIndex, L7_uchar8 family,
                                     L7_uint32 intIfNum, L7_uint32 queryCount)
{
    return mgmdMapInterfaceLastMembQueryCountSet(family, intIfNum, queryCount);
}


/*********************************************************************
* @purpose  Checks whether MGMD can use an interface
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbMgmdIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
    return mgmdIntfIsValid(intIfNum);
}

/**************************************************************************
* @purpose  Gets Current Number of membership in host cache table for this 
*            interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  family Type.
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    numGroups              @b{(output)} current  Number of memberships 
*                                               on this interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceNumMcastGroupsGet(L7_uint32 UnitIndex, 
                   L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *numGroups)
{
    return mgmdMapProxyInterfaceNumMcastGroupsGet(family, intIfNum,  numGroups);

}


/**************************************************************************
* @purpose  sets unsolicited interval for the host interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  family Type.
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    unsolicitedInterval    @b{(output)} unsolicited interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceUnsolicitedIntervalSet(L7_uint32 UnitIndex, 
          L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 unsolicitedInterval)
{
    return  mgmdMapProxyInterfaceUnsolicitedIntervalSet(family, 
                                                        intIfNum, 
                                                        unsolicitedInterval);
}


/**************************************************************************
* @purpose  Gets unsolicited interval for the host interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  family Type.
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    unsolicitedInterval    @b{(output)} unsolicited interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceUnsolicitedIntervalGet(L7_uint32 UnitIndex, 
          L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *unsolicitedInterval)
{
    return  mgmdMapProxyInterfaceUnsolicitedIntervalGet(family, 
                                                        intIfNum, 
                                                        unsolicitedInterval);
}


/****************************************************************************
* @purpose  Gets count of number of times the host interface has been restarted
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  family Type.
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    count                  @b{(output)} restart count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceRestartCountGet(L7_uint32 UnitIndex, 
                        L7_uchar8 family, L7_uint32 intIfNum, L7_uint32* count)
{
    return mgmdMapProxyInterfaceRestartCountGet(family, intIfNum,  count);
}

/*************************************************************************
* @purpose  Get the host interface number
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(output)}  interface Number.

* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceGet (L7_uint32 UnitIndex, L7_uchar8 family, 
                                    L7_uint32 *intIfNum)
{
  L7_uint32 interface;
  L7_uint32 val = L7_DISABLE;
  L7_uint32 nextInterface ;

  if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS)
  {
    while (interface)
    {
      /* display only the visible interfaces */
      if  (mgmdIntfIsValid(interface) == L7_TRUE)
      {
        if ((usmDbMgmdProxyInterfaceModeGet(UnitIndex, family, 
             interface, &val) == L7_SUCCESS) && val == L7_ENABLE)
        {
          *intIfNum = interface;
          return L7_SUCCESS;    
        }
      }    
      /* Get the next interface */
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
      }
      else
      {
         interface   = 0;
      }
    }
  }
  return L7_FAILURE;
}

/***********************************************************************
* @purpose  Get the number of v3 queries received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v3QueriesRcvd     @b{(output)} v3 quries received.
*  
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV3QueriesRecvdGet (L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum, L7_uint32 *v3QueriesRcvd)
{
  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
      
  {
      return mgmdMapProxyV3QueriesRecvdGet(family, intIfNum, v3QueriesRcvd);
  }

  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Get the number of v3 reports received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v3QueriesRcvd     @b{(output)} v3 quries received.
* 
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV3ReportsRecvdGet ( L7_uint32 UnitIndex, 
                 L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *v3ReportsRcvd)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV3ReportsRecvdGet(family, intIfNum, v3ReportsRcvd);

    }
    return L7_FAILURE;                                          
}
   
/*************************************************************************
* @purpose  Get the number of v3 reports sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v3ReportsSent     @b{(output)} v3 reports sent.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV3ReportsSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum, L7_uint32 *v3ReportsSent)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
       return mgmdMapProxyV3ReportsSentGet(family, intIfNum,v3ReportsSent);
        
    }
    return L7_FAILURE;
}

/***********************************************************************
* @purpose  Get the number of v2 queries received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v3QueriesRcvd     @b{(output)} v3 queries recieved.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2QueriesRecvdGet ( L7_uint32 UnitIndex, 
                L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *v2QueriesRcvd)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV2QueriesRecvdGet(family, intIfNum, v2QueriesRcvd);
    }
    return L7_FAILURE;                                          
}

/***********************************************************************
* @purpose  Get the number of v2 reports received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v3ReportsRcvd     @b{(output)} v3 reports received.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2ReportsRecvdGet ( L7_uint32 UnitIndex, 
                 L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *v2ReportsRcvd)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV2ReportsRecvdGet(family, intIfNum, v2ReportsRcvd);
    }
    return L7_FAILURE;                                          
}

/**********************************************************************
* @purpose  Get the number of v2 reports sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v3ReportsSent     @b{(output)} v3 reports sent.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2ReportsSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum, L7_uint32 *v2ReportsSent)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV2ReportsSentGet(family, intIfNum,v2ReportsSent);
    }
    return L7_FAILURE;
}

/**********************************************************************
* @purpose  Get the number of v2 leaves received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v2LeavesRcvd      @b{(output)} v2  Leaves received.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2LeavesRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum, L7_uint32 *v2LeavesRcvd)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
    {
        return mgmdMapProxyV2LeavesRecvdGet(family, intIfNum,v2LeavesRcvd);
    }
    return L7_FAILURE;
}

/************************************************************************
* @purpose  Get the number of v2 leaves sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v2LeavesSent      @b{(output)} v2 Leaves sent.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2LeavesSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_uint32 *v2LeavesSent)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV2LeavesSentGet(family, intIfNum,v2LeavesSent);
    }
    return L7_FAILURE;                                          
}

/**************************************************************************
* @purpose  Get the number of v1 queries received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v1QueriesRcvd     @b{(output)} v1 queries received.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV1QueriesRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 intIfNum, L7_uint32 *v1QueriesRcvd)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV1QueriesRecvdGet(family, intIfNum, v1QueriesRcvd);
    }
    return L7_FAILURE;                                          
} 

/***********************************************************************
* @purpose  Get the number of v1 reports received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v1ReportsRcvd     @b{(output)} v1 reports received.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV1ReportsRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 intIfNum, L7_uint32 *v1ReportsRcvd)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV1ReportsRecvdGet(family, intIfNum,v1ReportsRcvd);
    }
    return L7_FAILURE;                                          
}

/************************************************************************
* @purpose  Get the number of v3 reports sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    v1ReportsSent     @b{(output)} v1 reports sent.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV1ReportsSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum, L7_uint32 *v1ReportsSent)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyV1ReportsSentGet(family, intIfNum,v1ReportsSent);
    }
    return L7_FAILURE;                                          
}

/**********************************************************************
* @purpose  Resets the mgmd-proxy statistics parameters 
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyResetHostStatus(L7_uint32 UnitIndex, L7_uchar8 family,
                                      L7_uint32 intIfNum)
{
    if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)

    {
        return mgmdMapProxyResetHostStatus(family, intIfNum);
    }
    return L7_FAILURE;
}

#ifdef MGMD_TBD /*  CLI command is not there  */
/**********************************************************************
* @purpose  Set the MGMD interface as a QUERIER or NON-QUERIER 
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    mode              @b{(input)} enable or disable 
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                           L7_uint32 intIfNum, L7_uint32 mode)
{
    return mgmdMapInterfaceQuerierStatusSet(family, intIfNum,mode);
}


/**************************************************************************
* @purpose  Enables or disables  the MGMD PROXY as a multicast data forwarder 
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    mode              @b{(input)} enable or disable 
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyUpstreamFwdDisable(L7_uint32 UnitIndex, L7_uchar8 family,
                                         L7_uint32 intIfNum, L7_uint32 mode)
{
    return mgmdMapProxyUpstreamFwdDisable(family,intIfNum,mode);
}

/*************************************************************************
* @purpose  Get the status of the MGMD PROXY as a multicast data forwarder 
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  interface Number.
* @param    mode              @b{(input)} enable or disable 
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyUpstreamFwdDisableGet(L7_uint32 UnitIndex, 
                           L7_uchar8 family, L7_uint32 intIfNum, L7_BOOL *mode)
{
    return mgmdMapProxyUpstreamFwdDisableGet(family,intIfNum,mode);
}

#endif
/*********************************************************************
* @purpose  Get the Interface mode of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    mode            @b{(output)} Interface Mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceModeGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                       L7_uint32 intIfNum, L7_uint32* mode)
{
    return mgmdMapProxyInterfaceModeGet(family, intIfNum, mode);
}

/*********************************************************************
* @purpose  Get the Interface Operational State of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)}  family Type.
* @param    intIfNum        @b{(input)}  internal Interface number
*
* @returns  L7_TRUE  if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbMgmdProxyInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                         L7_uchar8 family, L7_uint32 intIfNum)
{
    if (mgmdMapMgmdProxyIntfInitialized(family, intIfNum) == L7_SUCCESS)
    {
      return L7_TRUE;
    }
    else
    {
      return L7_FALSE;
    }
}
/*********************************************************************
* @purpose  Clear the  Counters for MLD.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    family      @b{(input)} family Type.
* @param    intIfNum    @b{(input)} interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Resets packet counters only
*
* @end
*********************************************************************/
L7_RC_t usmDbMldCountersClear(L7_uint32 UnitIndex, 
                              L7_uchar8 family, L7_uint32 intIfNum)
{
  return mgmdMapCountersClear(family, intIfNum);
}
/*********************************************************************
* @purpose  Clear the  Traffic Counters for MLD.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    family      @b{(input)} family Type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Resets packet counters only
*
* @end
*********************************************************************/
L7_RC_t usmDbMldTrafficCountersClear(L7_uint32 UnitIndex, L7_uchar8 family)
{
  return mgmdMapTrafficCountersClear(family);
}

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Received for the router.
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)}  family Type.
* @param    val         @b{(output)} Valid Packets Received.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbValidPacketsRcvdGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_uint32* val)
{
    return mgmdMapValidPacketsRcvdGet(family, val);
}
/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Sent for the router.
*
* @param    UnitIndex  @b{(input)} unit
* @param    family     @b{(input)} Address Family type
* @param    val        @b{(output)} Valid Packets Sent.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbValidPacketsSentGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_uint32* val)
{
    return mgmdMapValidPacketsSentGet(family, val);
}
/*********************************************************************
* @purpose  Get the Queries Received for the router.
*
* @param    UnitIndex @b{(input)} unit
* @param    family    @b{(input)} Address Family type
* @param    val       @b{(output)} Queries Rcvd.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbQueriesReceivedGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                L7_uint32* val)
{
    return mgmdMapValidQueriesRcvdGet(family, val);
}
/*********************************************************************
* @purpose  Get the Queries Sent for the router.
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)} Address Family type
* @param    val         @b{(output)} Queries Sent.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbQueriesSentGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                            L7_uint32* val)
{
    return mgmdMapValidQueriesSentGet(family, val);
}
/*********************************************************************
* @purpose  Get the Reports Received for the router.
*
* @param    UnitIndex  @b{(input)} unit
* @param    family     @b{(input)} Address Family type
* @param    val        @b{(output)} Reports Rcvd.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbReportsReceivedGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                L7_uint32* val)
{
    return mgmdMapReportsReceivedGet(family, val);
}
/*********************************************************************
* @purpose  Get the Reports Sent for the router.
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)} Address Family type
* @param    val         @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbReportsSentGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                            L7_uint32* val)
{
    return mgmdMapReportsSentGet(family, val);
}
/*********************************************************************
* @purpose  Get the Leaves Received for the router.
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)} Address Family type
* @param    val         @b{(output)} Leaves Rcvd.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbLeavesReceivedGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                               L7_uint32* val)
{
    return mgmdMapLeavesReceivedGet(family, val);
}
/*********************************************************************
* @purpose  Get the Leaves Sent for the router.
*
* @param    UnitIndex  @b{(input)} unit
* @param    family     @b{(input)} Address Family type
* @param    val        @b{(output)} Leaves Sent.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbLeavesSentGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                           L7_uint32* val)
{
    return mgmdMapLeavesSentGet(family, val);
}
/*********************************************************************
* @purpose  Get the Bad Checksum packets for the router.
*
* @param    UnitIndex  @b{(input)} unit
* @param    family     @b{(input)} Address Family type
* @param    val        @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbBadCheckSumPktsGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                L7_uint32* val)
{
    return mgmdMapBadCheckSumPktsGet(family, val);
}
/*********************************************************************
* @purpose  Get the Malformed packets for the router.
*
* @param    UnitIndex  @b{(input)} unit
* @param    family     @b{(input)} Address Family type
* @param    val        @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
*
* @end
*********************************************************************/
L7_RC_t usmDbMalformedPktsGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                              L7_uint32* val)
{
    return mgmdMapMalformedPktsGet(family, val);
}
/*********************************************************************
* @purpose  Turns on/off the displaying of mgmd packet debug info
*            
* @param    family     @b{(input)} Address Family type
* @param    rxFlag     @b{(input)} Receive Trace Flag
* @param    txFlag     @b{(input)} Transmit Trace Flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdPacketDebugTraceFlagSet(L7_uchar8 family, L7_BOOL rxFlag,
                                         L7_BOOL txFlag)
{
  if (mgmdDebugTraceFlagSet(family, MGMD_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (mgmdDebugTraceFlagSet(family, MGMD_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the mgmd packet debug info
*            
* @param    family     @b{(input)} Address Family type
* @param    rxFlag     @b{(output)} Receive Trace Flag
* @param    txFlag     @b{(output)} Transmit Trace Flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdPacketDebugTraceFlagGet(L7_uchar8 family, L7_BOOL *rxFlag,
                                         L7_BOOL *txFlag)
{
  if (mgmdDebugTraceFlagGet(family, MGMD_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (mgmdDebugTraceFlagGet(family, MGMD_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
