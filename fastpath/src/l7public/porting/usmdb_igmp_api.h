/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
**********************************************************************
* @filename  usmdb_mgmd_api.h
*
* @purpose   USMDB mgmd api Include files
*
*
* @create
*
* @author Nitish
*
* @end
*
**********************************************************************/        
#ifndef USMDB_MGMD_API_H
#define USMDB_MGMD_API_H

#include "l7_common.h"
#include "l3_mcast_commdefs.h"

/*********************************************************************
* @purpose  Get the administrative mode of MGMD in the router.
*
* @param    UnitIndex   @b{(input)}   unit
* @param    family      @b{(input)} Address Family type
* @param    mode        @b{(output)}  MGMD admin mode
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
                              L7_uint32* mode);

/*********************************************************************
* @purpose  Set the administrative mode of MGMD in the router.
*
* @param    UnitIndex   @b{(input)}   unit
* @param    family      @b{(input)} Address Family type
* @param    mode        @b{(input)}   MGMD Admin mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdAdminModeSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                              L7_uint32 mode);

/*********************************************************************
* @purpose  Get the Interface mode of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    *mode           @b{(output)} Interface Mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceModeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                  L7_uint32 intIfNum, L7_uint32* mode);

/*********************************************************************
* @purpose  Get the Interface Operational State of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
*
* @returns  L7_TRUE  if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbMgmdInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                         L7_uchar8 family, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the Interface mode of MGMD in the router.
*
* @param    UnitIndex       @b{(input)}  unit
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    mode            @b{(input)}  Interface Mode
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
  L7_MGMD_ERROR_CODE_t *errCode);

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
                                         L7_uchar8 family, L7_BOOL checkRtrAlert);

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
                                         L7_uchar8 family, L7_BOOL *checkRtrAlert);

/*********************************************************************
* @purpose  Gets the  MGMD Interface Startup Query interval
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    startupInterval   @b{(output)} startup query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryIntervalGet(L7_uint32 UnitIndex, 
     L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *startupInterval);


/*********************************************************************
* @purpose  Sets the  MGMD Interface Startup Query interval
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    startupInterval @b{(input)}  startup query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryIntervalSet(L7_uint32 UnitIndex, 
      L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 startupInterval);


/**********************************************************************
* @purpose  Gets Querier / Non-Querier Status of MGMD on this interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)} Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    status                 @b{(output)} Enable / Disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*************************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierStatusGet(L7_uint32 UnitIndex, 
                     L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *status);


/*********************************************************************
* @purpose  Gets the  MGMD Interface Startup Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)} Address Family type
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
               L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *startupCount);


/*********************************************************************
* @purpose  Sets the  MGMD Interface Startup Query count
*
* @param    UnitIndex      @b{(input)}  Unit
* @param    family         @b{(input)} Address Family type
* @param    intIfNum       @b{(input)}  internal Interface number
* @param    startupCount   @b{(input)}  startup query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceStartupQueryCountSet(L7_uint32 UnitIndex, 
                L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 startupCount);


/*********************************************************************
* @purpose  Gets the  MGMD Interface last member Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)} Address Family type
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
                                   L7_uint32 intIfNum, L7_uint32 *queryCount);

/*********************************************************************
* @purpose  Sets the  MGMD Interface last member Query count
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)} Address Family type
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
                                   L7_uint32 intIfNum, L7_uint32 queryCount);

/*********************************************************************
* @purpose  Checks whether MGMD can use an interface
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbMgmdIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Gets Current Number of membership in host cache table for this 
*           interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    numGroups              @b{(output)} current  Number of 
*                                               memberships on this interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceNumMcastGroupsGet(L7_uint32 UnitIndex, 
                   L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *numGroups);

/*************************************************************************
* @purpose  sets unsolicited interval for the host interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    unsolicitedInterval    @b{(input)} unsolicited interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
***********************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceUnsolicitedIntervalSet(L7_uint32 UnitIndex, 
          L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 unsolicitedInterval);

/*************************************************************************
* @purpose  Gets unsolicited interval for the host interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    unsolicitedInterval    @b{(output)} unsolicited interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceUnsolicitedIntervalGet(L7_uint32 UnitIndex, 
         L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *unsolicitedInterval);

/****************************************************************************
* @purpose  Gets count of number of times the host interface has been restarted
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    count                  @b{(output)} restart count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceRestartCountGet(L7_uint32 UnitIndex, 
                       L7_uchar8 family, L7_uint32 intIfNum, L7_uint32* count);

/********************************************************************
* @purpose  Get the host interface number
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(output)}  internal Interface Number
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyInterfaceGet (L7_uint32 UnitIndex, L7_uchar8 family, 
                                    L7_uint32 *intIfNum);

/***********************************************************************
* @purpose  Get the number of v3 queries received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v3QueriesRcvd     @b{(output)}  Queries Received
* @param    
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV3QueriesRecvdGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum,L7_uint32 *v3QueriesRcvd);


/***************************************************************************
* @purpose  Get the number of v3 reports received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v3ReportsRcvd     @b{(output)}  Reports Received
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV3ReportsRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum,L7_uint32 *v3ReportsRcvd);


/************************************************************************
* @purpose  Get the number of v3 reports sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v3ReportsSent     @b{(output)}  Reports Sent on the interface    
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV3ReportsSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 intIfNum,L7_uint32 *v3ReportsSent);


/**************************************************************************
* @purpose  Get the number of v2 queries received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v2QueriesRcvd     @b{(output)}  v2 Queries Received 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2QueriesRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 intIfNum,L7_uint32 *v2QueriesRcvd);



/*************************************************************************
* @purpose  Get the number of v2 reports received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v2ReportsRcvd     @b{(output)}  v2 Reports Received 
* @param    
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2ReportsRecvdGet (L7_uint32 UnitIndex, L7_uchar8 family,
                                L7_uint32 intIfNum,L7_uint32 *v2ReportsRcvd);



/*************************************************************************
* @purpose  Get the number of v2 reports sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v2ReportsSent     @b{(output)}  v2 Reports Sent 
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2ReportsSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 intIfNum,L7_uint32 *v2ReportsSent);



/**************************************************************************
* @purpose  Get the number of v2 leaves received on the host interface
* 
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v2LeavesRcvd     @b{(output)}  v2 Reports Received.
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2LeavesRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 intIfNum,L7_uint32 *v2LeavesRcvd);


/************************************************************************
* @purpose  Get the number of v2 leaves sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v2LeavesSent      @b{(output)}  v2 Leaves Sent 
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV2LeavesSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum,L7_uint32 *v2LeavesSent);


/*************************************************************************
* @purpose  Get the number of v1 queries received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v1QueriesRcvd     @b{(output)}  v1 Queries Received 
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV1QueriesRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                L7_uint32 intIfNum,L7_uint32 *v1QueriesRcvd);


/*************************************************************************
* @purpose  Get the number of v1 reports received on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v1ReportsRcvd     @b{(output)}  v1 Reports Received 
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV1ReportsRecvdGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                L7_uint32 intIfNum,L7_uint32 *v1ReportsRcvd);


/***************************************************************************
* @purpose  Get the number of v3 reports sent on the host interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface Number
* @param    v1ReportsSent     @b{(output)}  v1 Reports Sent 
* 
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE   
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyV1ReportsSentGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                  L7_uint32 intIfNum,L7_uint32 *v1ReportsSent);

/**********************************************************************
* @purpose  Resets the mgmd-proxy statistics parameters
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfNum          @b{(input)}  internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyResetHostStatus(L7_uint32 UnitIndex, L7_uchar8 family,
                                      L7_uint32 intIfNum);

/*******************************************************************
* @purpose  Set the MGMD interface as a QUERIER or NON-QUERIER
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  internal Interface Number
* @param    mode              @b{(input)} enable or disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                     L7_uint32 intIfNum, L7_uint32 mode);

/**********************************************************************
* @purpose  Enables or disables  the MGMD PROXY as a multicast data forwarder
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  family Type.
* @param    intIfnum          @b{(input)}  internal Interface Number
* @param    mode              @b{(input)} enable or disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdProxyUpstreamFwdDisable(L7_uint32 UnitIndex, L7_uchar8 family,
                                         L7_uint32 intIfNum, L7_uint32 mode);

/************************************************************************
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
************************************************************************/
L7_RC_t usmDbMgmdProxyUpstreamFwdDisableGet(L7_uint32 UnitIndex, 
                         L7_uchar8 family, L7_uint32 intIfNum, L7_BOOL *mode);

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
                                       L7_uint32 intIfNum, L7_uint32* mode);

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
                                        L7_uchar8 family, L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Clear the  Counters for MLD.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    family      @b{(input)}  family Type.
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
                              L7_uchar8 family, L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Clear the  Traffic Counters for MLD.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    family      @b{(input)}  family Type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Resets packet counters only
*
* @end
*********************************************************************/
L7_RC_t usmDbMldTrafficCountersClear(L7_uint32 UnitIndex, L7_uchar8 family);

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Received for the router.
*
* @param    UnitIndex  @b{(input)} unit
* @param    family     @b{(input)} Address Family type
* @param    val        @b{(output)} Valid Packets Received.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t usmDbValidPacketsRcvdGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_uint32* val);
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
                                 L7_uint32* val);
/*********************************************************************
* @purpose  Get the Queries Received for the router.
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)} Address Family type
* @param    vala        @b{(output)} Queries Rcvd.
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
                                L7_uint32* val);
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
                            L7_uint32* val);
/*********************************************************************
* @purpose  Get the Reports Received for the router.
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)} Address Family type
* @param    val         @b{(output)} Reports Rcvd.
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
                                L7_uint32* val);
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
                            L7_uint32* val);

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
                               L7_uint32* val);

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
                           L7_uint32* val);

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
                                L7_uint32* val);
/*********************************************************************
* @purpose  Get the Malformed packets for the router.
*
* @param    UnitIndex @b{(input)} unit
* @param    family    @b{(input)} Address Family type
* @param    val       @b{(output)} Reports Sent.
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
                              L7_uint32* val);
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
                                         L7_BOOL txFlag);
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
                                         L7_BOOL *txFlag);


#endif
