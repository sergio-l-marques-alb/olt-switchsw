/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  usmdb_mib_ripv2_api.h
*
* @purpose   Provide interface to API's for RFC1724 RipV2 MIB components
*
* @component unitmgr
*
* @comments
*
* @create    01/10/2001
*
* @author    rjindal
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef _USMDB_MIB_RIPV2_API_H_
#define _USMDB_MIB_RIPV2_API_H_

#include "l3_commdefs.h"

/*:ignore*/
/* IP Address entry get/next */
/*:end ignore*/

/*********************************************************************
* @purpose  Validate a specific IP Address entry as a RIP subnet 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IpAddr      @b{(input)} Subnet IP address to validate
*
* @returns  L7_SUCCESS  IP address is valid for RIP operation
* @returns  L7_FAILURE  IP address not valid for RIP operation
*
* @comments Caller's *IpAddr value remains unchanged.  Check return code
*           to determine result.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfEntryGet(L7_uint32 UnitIndex, L7_uint32 IpAddr);

/*********************************************************************
* @purpose  Get the next IP Address entry that is defined as a RIP subnet
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    *IpAddr     @b{(input)} Subnet IP Address to start search @line
*                       @b{(output)} Found next subnet IP address 
*
* @returns  L7_SUCCESS  Next IP Address was found
* @returns  L7_ERROR    No more RIP subnet IP Addresses available
* @returns  L7_FAILURE  Function failed
*
* @comments Use a value of 0 for the *IpAddr (contents) to search for    
*           first address entry.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfEntryNext(L7_uint32 UnitIndex, L7_uint32 *IpAddr);

/*********************************************************************
* @purpose  Convert a RIP subnet IP address to an internal interface number
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IpAddr      @b{(input)} Subnet IP Address of interest
* @param    *intIfNum   @b{(output)} Internal interface number for IP subnet
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments Can use this function prior to accessing USMDB rip2IfStat and
*           rip2IfConf functions, which require an internal intf num.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfIPAddrToIntIf(L7_uint32 UnitIndex, L7_uint32 IpAddr, L7_uint32 *IntIfNum);


/*:ignore*/
/* rip2Global */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the RIP administrative mode
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    *val        @b{(output)} RIP admin mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2AdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP administrative mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} RIP admin mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2AdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Get the number of route changes made by RIP to the 
*           IP Route Database
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    *val        @b{(output)} RIP route changes count
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2GlobalRouteChangesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of responses sent to RIP queries from other systems
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    *val        @b{(output)} RIP query response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2GlobalQueriesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the RIP split horizon mode
*
* @param    UnitIndex   @b{(input)}Unit for this operation
* @param    *val        @b{(output)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2SplitHorizonModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP split horizon mode
*
* @param    UnitIndex   @b{(input)}Unit for this operation
* @param    val         @b{(input)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2SplitHorizonModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP auto-summarization mode
*
* @param    UnitIndex   @b{(input)}Unit for this operation
* @param    *val        @b{(output)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2AutoSummarizationModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP auto-summarization mode
*
* @param    UnitIndex   @b{(input)}Unit for this operation
* @param    val         @b{(input)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2AutoSummarizationModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP host route accept mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    *val        @b{(output)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2HostRoutesAcceptModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP host route accept mode
*
* @param    UnitIndex   @b{(input)}Unit for this operation
* @param    val         @b{(input)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2HostRoutesAcceptModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Get the default metric value
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    *val         @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric is not configured.
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2DefaultMetricGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the default metric value
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    mode         @b{(input)} enable/disable default metric value
* @param    val          @b{(input)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The purpose of the parameter mode is to specify whether metric is
*            being configured or deconfigured. If mode is disable, the metric is
*            set to invalid value and not to default value. If mode is disable the
*            val parameter is ignored. If mode is enable, this function checks if val
*            is within aloowable parameters.
*            This function returns failure in the followign cases:
*                 1. If mode is L7_ENABLE and val is outside of range
*                 2. If the metric cannot be applied.
*                 
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2DefaultMetricSet(L7_uint32 UnitIndex, L7_uint32 mode, 
                                         L7_uint32 val);

/*********************************************************************
* @purpose  Clears the default metric value. Sets it to its default value.
*
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2DefaultMetricClear(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Get the RIP default route advertise mode
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    *val         @b{(output)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2DefaultRouteAdvertiseModeGet(L7_uint32 UnitIndex, L7_uint32 *val);


/*********************************************************************
* @purpose  Set the RIP default route advertise mode
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    val          @b{(input)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2DefaultRouteAdvertiseModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Clear the RIP default route advertise mode. Sets it to its default value.
*
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2DefaultRouteAdvertiseModeClear(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Get the filter used for filtering redistributed routes from
*           the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route.
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *val        @b{(output)} ACL number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If filter is not configured.
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeFilterGet(L7_uint32 UnitIndex, 
                                             L7_REDIST_RT_INDICES_t protocol, 
                                             L7_uint32 *val);


/*********************************************************************
* @purpose  Set the filter used for filtering redistributed routes from
*           the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    flag        @b{(input)} enable/disable route filter
* @param    val         @b{(input)} ACL number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The purpose of the parameter mode is to specify whether filter is
*            being configured or deconfigured. If mode is disable, the filter is
*            set to invalid value and not to default value. If mode is disable the
*            val parameter is ignored. If mode is enable, this function checks if val
*            is within aloowable parameters.
*            This function returns failure in the followign cases:
*                 1. If mode is L7_ENABLE and val is outside of range
*                 2. If the filter cannot be applied.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeFilterSet(L7_uint32 UnitIndex, 
                                             L7_REDIST_RT_INDICES_t protocol, 
                                             L7_uint32 mode, L7_uint32 val);

/*********************************************************************
* @purpose  Clear the filter used for filtering redistributed routes from
*           the specified source protocol. Sets it to its default value.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeFilterClear(L7_uint32 UnitIndex, 
                                               L7_REDIST_RT_INDICES_t protocol);

/*********************************************************************
* @purpose  Check if the source protocol exists in route redistribution table
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeSourceGet(L7_uint32 UnitIndex, 
                                                   L7_REDIST_RT_INDICES_t protocol);

/*********************************************************************
* @purpose  Get the next source protocol in route redistribution table
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    *protocol    @b{(input)} Source protocol of redist route
*                                    Type L7_REDIST_RT_INDICES_t.
*                        @b{(output)} Next valid Source protocol of redist route
*                                     Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeSourceGetNext(L7_uint32 UnitIndex, 
                                                       L7_REDIST_RT_INDICES_t *protocol);

/*********************************************************************
* @purpose  Get the metric and match type used for redistributing routes from
*           the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *metric     @b{(output)} Metric used for redistributing routes
* @param    *matchType  @b{(output)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric or matchType is not configured.
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeParmsGet(L7_uint32 UnitIndex, 
                                                  L7_REDIST_RT_INDICES_t protocol, 
                                                  L7_uint32 *metric, 
                                                  L7_OSPF_METRIC_TYPES_t *matchType);


/*********************************************************************
* @purpose  Set the route redistribution mode metric and match type used
*           for redistributing routes from the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    parmsMode   @b{(input)} Specify if parms are enabled/disabled for this protocol
* @param    metric      @b{(input)} Metric used for redistributing routes
* @param    matchType   @b{(input)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The purpose of the parameter parmsMode is to specify whether parms are
*            being configured or deconfigured. If parmsMode is disable, the parms are
*            set to invalid values and not to default values. If parmsMode is disable the
*            parms parameters is ignored. If parmsMode is enable, this function checks if
*            parms are within allowable parameters.
*            This function doesnot activate  or deactivate route redistribution for
*            the specified protocol. There's another function that has to be called to
*            activate or deactivate it.
*            This function returns failure in the followign cases:
*                 1. If parmsMode is L7_ENABLE and parms are outside of ranges
*                 2. If the parms cannot be applied.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeParmsSet(L7_uint32 UnitIndex, 
                                                  L7_REDIST_RT_INDICES_t protocol, 
                                                  L7_uint32 parmsMode, L7_uint32 metric, 
                                                  L7_uint32 matchType);


/*********************************************************************
* @purpose  Clear the route redistribution mode metric and match type used used 
*           for redistributing routes from the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeParmsClear(L7_uint32 UnitIndex, 
                                                    L7_uint32 protocol);


/*********************************************************************
* @purpose  Clear the filter used for filtering redistributed routes from
*           the specified source protocol. Sets it to its default value.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RRouteRedistributeFilterClear(L7_uint32 UnitIndex, 
                                                    L7_uint32 protocol);

/*********************************************************************
* @purpose  Get the metric for redistributing routes from
*           the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *metric     @b{(output)} Metric used for redistributing routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric is not configured.
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeMetricGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 protocol, L7_uint32 *metric);


/*********************************************************************
* @purpose  Set the route redistribution mode and metric used 
*           for redistributing routes from the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    metricMode  @b{(input)} Specify if metric is enabled/disabled for this protocol
* @param    *metric     @b{(input)} Metric used for redistributing routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The purpose of the parameter metricMode is to specify whether metric is
*            being configured or deconfigured. If metricMode is disable, the metric is
*            set to invalid value and not to default value. If metricMode is disable the
*            metric parameter is ignored. If metricMode is enable, this function checks if
*            metric is within allowable parameters.
*            This function doesnot activate  or deactivate route redistribution for
*            the specified protocol. There's another function that has to be called to
*            activate or deactivate it.
*            This function returns failure in the followign cases:
*                 1. If mode is L7_ENABLE and metric is outside of range
*                 2. If the metric cannot be applied.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeMetricSet(L7_uint32 UnitIndex, 
                                                   L7_REDIST_RT_INDICES_t protocol, 
                                                   L7_uint32 metricMode, L7_uint32 metric);


/*********************************************************************
* @purpose  Clear the route redistribution metric used 
*           for redistributing routes from the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeMetricClear(L7_uint32 UnitIndex, 
                                                     L7_REDIST_RT_INDICES_t protocol);


/*********************************************************************
* @purpose  Get the match type used for redistributing routes from
*           the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *matchType  @b{(output)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If matchType is not configured.
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeMatchTypeGet(L7_uint32 UnitIndex, 
                                                      L7_REDIST_RT_INDICES_t protocol, 
                                                      L7_uint32 *matchType);


/*********************************************************************
* @purpose  Set the route redistribution match type used
*           for redistributing routes from the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    matchMode   @b{(input)} Specify if matchType is enabled/disabled for this protocol
* @param    matchType   @b{(input)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The purpose of the parameter matchMode is to specify whether matchType is
*            being configured or deconfigured. If matchMode is disable, only those 
*            specified in the matchType are disabled. Then if matchType is same as 
*            stored config, the config is set to invalid value and not to default value. 
*            If metricMode is enable, this function checks if
*            matchType is within allowable parameters.
*            This function doesnot activate  or deactivate route redistribution for
*            the specified protocol. There's another function that has to be called to
*            activate or deactivate it.
*            This function returns failure in the followign cases:
*                 1. If matchMode is L7_ENABLE and matchType is outside of range
*                 2. If the matchType cannot be applied.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeMatchTypeSet(L7_uint32 UnitIndex, 
                                                      L7_REDIST_RT_INDICES_t protocol, 
                                                      L7_uint32 matchMode, 
                                                      L7_uint32 matchType);


/*********************************************************************
* @purpose  Clear the route redistribution match type used
*           for redistributing routes from the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeMatchTypeClear(L7_uint32 UnitIndex, 
                                                        L7_REDIST_RT_INDICES_t protocol);


/*********************************************************************
* @purpose  Get the redistribution mode for redistributing routes from
*           the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *mode       @b{(output)} Redistribution mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeModeGet(L7_uint32 UnitIndex, 
                                                 L7_REDIST_RT_INDICES_t protocol, 
                                                 L7_uint32 *mode);


/*********************************************************************
* @purpose  Set the route redistribution mode for the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    mode        @b{(input)} Specify if mode is enabled/disabled for this protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function does not reset other redistribution parms to their default
*            values or to invalid values. It only disables/enables route redistribution
*            for routes from specified protocol.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeModeSet(L7_uint32 UnitIndex, 
                                                 L7_REDIST_RT_INDICES_t protocol, 
                                                 L7_uint32 mode);


/*********************************************************************
* @purpose  Clear the route redistribution mode for the specified source protocol.
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2RouteRedistributeModeClear(L7_uint32 UnitIndex, 
                                                   L7_REDIST_RT_INDICES_t protocol);


/*:ignore*/
/* rip2IfStat */
/*:end ignore*/

/*********************************************************************
* @purpose  Validate a specific IP Address for RIP interface stats
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IpAddr      @b{(input)}  Subnet IP address to validate
*
* @returns  L7_SUCCESS  IP address is valid for RIP operation
* @returns  L7_FAILURE  IP address not valid for RIP operation
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatEntryGet(L7_uint32 UnitIndex, L7_uint32 IpAddr);

/*********************************************************************
* @purpose  Get the next IP Address entry for RIP interface stats
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    *IpAddr     @b{(input)}  Subnet IP Address to start search @line
*                       @b{(output)} Found next subnet IP address 
*
* @returns  L7_SUCCESS  Next IP Address was found
* @returns  L7_ERROR    No more RIP subnet IP Addresses available
* @returns  L7_FAILURE  Function failed
*
* @comments Use a value of 0 for the *IpAddr (contents) to search for    
*           first address entry.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatEntryNext(L7_uint32 UnitIndex, L7_uint32 *IpAddr);

/*********************************************************************
* @purpose  Get the number of RIP response packets received by the RIP 
*           process which were subsequently discarded for any reason
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal interface number
* @param    *val        @b{(output)} Bad Packets Received count
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatRcvBadPacketsGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of routes contained in valid RIP packets that 
*           were ignored for any reason
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Bad Routes Received count
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatRcvBadRoutesGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of triggered RIP updates actually sent 
*           on this interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Triggered Updates Sent count
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatSentUpdatesGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get current status of a RIP routing interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Status value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatStatusGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set current status of a RIP routing interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Status value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfStatStatusSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);


/*:ignore*/
/*rip2IfConf*/
/*:end ignore*/

/*********************************************************************
* @purpose  Validate a specific IP Address for RIP interface config
*
* @param    UnitIndex   Unit for this operation
* @param    IpAddr      @{(input)} Subnet IP address to validate
*
* @returns  L7_SUCCESS  IP address is valid for RIP operation
* @returns  L7_FAILURE  IP address not valid for RIP operation
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfEntryGet(L7_uint32 UnitIndex, L7_uint32 IpAddr);

/*********************************************************************
* @purpose  Get the next IP Address entry for RIP interface config
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    *IpAddr     @b{(input)}  Subnet IP Address to start search @line
*                       @b{(output)} Found next subnet IP address 
*
* @returns  L7_SUCCESS  Next IP Address was found
* @returns  L7_ERROR    No more RIP subnet IP Addresses available
* @returns  L7_FAILURE  Function failed
*
* @comments Use a value of 0 for the *IpAddr (contents) to search for    
*           first address entry.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfEntryNext(L7_uint32 UnitIndex, L7_uint32 *IpAddr);

/*********************************************************************
* @purpose  Get the administrative mode of a RIP routing interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Admin mode value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfAdminModeGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the administrative mode of a RIP routing interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Admin mode value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfAdminModeSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the routing domain value for this RIP interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Routing domain value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments This MIB object is listed as obsolete.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfDomainGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the routing domain value for this RIP interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Routing domain value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfDomainSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP Authentication information for the specified interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    type        @b{(output)} Authentication type
* @param    key         @b{(output)} Authentication key
* @param    keyId       @b{(output)} Authentication key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The keyId is only meaningful for MD5 authentication; 
*           otherwise, it should be ignored.
*       
* @comments This function outputs the actual key value (do not use
*           for RIPv2 MIB support).
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthenticationGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                                L7_uint32 *type, L7_uchar8 *key, 
                                                L7_uint32 *keyId);

/*********************************************************************
* @purpose  Set the RIP Authentication information for the specified interface 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    type        @b{(input)} Authentication type
* @param    key         @b{(input)} Authentication key
* @param    keyId       @b{(input)} Authentication key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The keyId is only used for MD5 authentication; it is otherwise
*           ignored.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthenticationSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                                L7_uint32 type, L7_uchar8 *key, 
                                                L7_uint32 keyId);

/*********************************************************************
* @purpose  Get the RIP Authentication Type for the specified interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Authentication type
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthTypeGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP Authentication Type for the specified interface 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Authentication type
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments The auth type must always be set before the key, since setting
*           the auth type causes the existing key to be cleared.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthTypeSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP Authentication Key for the specified interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *buf        @b{(output)} Authentication key string
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments RFC 1724 specifies that authentication may not be bypassed by 
*           reading the MIB, so this function must return an empty key of
*           length zero.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthKeyGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uchar8 *buf);

/*********************************************************************
* @purpose  Get the actual RIP Authentication Key for the specified interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *buf        @b{(output)} Authentication key string
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments This API outputs the actual key string, not an empty string.
*           Do not use this API to support the RIPv2 MIB.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthKeyActualGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uchar8 *buf);

/*********************************************************************
* @purpose  Set the RIP Authentication Key for the specified interface 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    *buf        @b{(input)} Authentication key string
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments The Authentication Key must always be set after the auth type.
*
* @comments The value to be used as the Authentication Key whenever the 
*           corresponding instance of rip2IfConfAuthType has a value 
*           other than noAuthentication.  A modification of the corresponding
*           instance of rip2IfConfAuthType does not modify the rip2IfConfAuthKey value. 
*           Reading this object always results in an OCTET STRING of length zero; 
*           authentication may not be bypassed by reading the MIB object.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthKeySet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uchar8 *buf);

/*********************************************************************
* @purpose  Get the RIP Authentication Key ID for the specified interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Authentication key id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthKeyIdGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP Authentication Key ID for the specified interface 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Authentication key id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfAuthKeyIdSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP version used for sending updates on the specified 
*           interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} RIP version used for sends
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfSendGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP version used for sending updates on the specified 
*           interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} RIP version used for sends
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfSendSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the RIP version used for receiving updates from the specified 
*           interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} RIP version used for receives
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfReceiveGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the RIP version used for receiving updates from the specified 
*           interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} RIP version used for receives
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfReceiveSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the metric used for default routes in RIP updates 
*           originated on the specified interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Default metric malue
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments This variable indicates the metric that is to
*           be used for the default route entry in RIP updates
*           originated on this interface.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfDefaultMetricGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the metric used for default routes in RIP updates 
*           originated on the specified interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Default metric value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments This variable indicates the metric that is to
*           be used for the default route entry in RIP updates
*           originated on this interface.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfDefaultMetricSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get current status of a RIP routing interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Status value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfStatusGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set current status of a RIP routing interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Status value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfStatusSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the IP source address used by the specified RIP interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Source IP address value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments The IP source address is the same as the IP address configured
*           for the routing interface.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfSrcIpAddrGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the IP source address used by the specified RIP interface
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IntIfNum    @b{(input)} Internal Interface Number
* @param    val         @b{(input)} Source IP Address value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments This code does not allow a source IP address to be set that is 
*           different from the RIP interface IP address, thus no 'set' 
*           actually takes place here.
*
* @comments The MIB definition states that the source IP address must be
*           set to one of the configured routing interfaces' IP address.
*           We restrict this further by requiring the source be the same
*           as the specified interface IP address.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IfConfSrcIpAddrSet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 val);


/*:ignore*/
/* rip2Peer */

/*---------------------------------------------------------------------------
 * NOTE:  The RIP Peer Group is optional for the MIB, and is not supported 
 *        at this time.
 *---------------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Validate the entry IP address of a single routing peer
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    IpAddr      @b{(input)} Peer router IP Address
* @param    *Domain     @b{(input)} Routing domain of peer
*
* @returns  L7_ERROR  Routing peer entry not found
*
* @comments Peer group not supported.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2PeerEntryGet(L7_uint32 UnitIndex, L7_IP_ADDR_t IpAddr, L7_char8 *Domain);

/*********************************************************************
* @purpose  Get the next entry IP address of a single routing peer
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    *IpAddr     @b{(input)}  Peer router IP Address to start search @line
*                       @b{(output)} Found next subnet IP address 
* @param    Domain      @b{(input)}  Routing domain of peer
*
* @returns  L7_ERROR  No additional routing peer entries available
*
* @comments Peer group not supported.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2PeerEntryNext(L7_uint32 UnitIndex, L7_IP_ADDR_t *IpAddr, L7_char8 *Domain);

/*********************************************************************
* @purpose  Get the value of sysUpTime when the most recent RIP update
*           was received from the specified peer router
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IpAddr      @b{(input)}  Peer router IP address
* @param    Domain      @b{(input)}  Routing domain of peer
* @param    *val        @b{(output)} Latest RIP update receive time
*
* @returns  L7_NOT_SUPPORTED
*
* @comments Peer group not supported. 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2PeerLastUpdateGet(L7_uint32 UnitIndex, L7_IP_ADDR_t IpAddr, L7_char8 *Domain, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the RIP version number from the header of the last RIP
*           packet received from the specified peer router
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IpAddr      @b{(input)}  Peer router IP Address
* @param    Domain      @b{(input)}  Routing domain of peer
* @param    *val        @b{(output)} RIP version number
*
* @returns  L7_NOT_SUPPORTED
*
* @comments Peer group not supported.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2PeerVersionGet(L7_uint32 UnitIndex, L7_IP_ADDR_t IpAddr, L7_char8 *Domain, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of RIP response packets from this peer that
*           were discarded as invalid
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IpAddr      @b{(input)}  Peer router IP Address
* @param    Domain      @b{(input)}  Routing domain of peer
* @param    *val        @b{(output)} Bad packets received count
*
* @returns  L7_NOT_SUPPORTED
*
* @comments Peer group not supported.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2PeerRcvBadPacketsGet(L7_uint32 UnitIndex, L7_IP_ADDR_t IpAddr, L7_char8 *Domain, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of routes received from this peer that were
*           discarded as invalid
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IpAddr      @b{(input)}  Peer router IP Address
* @param    Domain      @b{(input)}  Routing domain of peer
* @param    *val        @b{(output)} Bad routes received count
*
* @returns  L7_NOT_SUPPORTED
*
* @comments Peer group not supported.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2PeerRcvBadRoutesGet(L7_uint32 UnitIndex, L7_IP_ADDR_t IpAddr, L7_char8 *Domain, L7_uint32 *val);


/*:ignore*/
/* Additional APIs (non-MIB) */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the IP address of the specified RIP interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} RIP interface IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IntfIpAddrGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the subnet mask of the specified RIP interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} RIP interface subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IntfSubnetMaskGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the interface type for the specified RIP interface 
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Interface type (see nimIntfTypes for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IntfTypeGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the current state of the specified RIP interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} RIP interface state (L7_UP or L7_DOWN)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IntfStateGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the state flags of the specified RIP interface
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} RIP interface state flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The interface state flags are defined from a Un*x-like kernel
*           perspective.  The intent of this function is to allow a
*           show-type function to obtain them for display, primarily
*           as debug information.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IntfStateFlagsGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of times the specified RIP interface has changed 
*           state from up to down
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Interface up-down transition count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbRip2IntfLinkEventsCtrGet(L7_uint32 UnitIndex, L7_uint32 IntIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Check if an interface is valid for RIP
*
* @param    UnitIndex   @b{(input)}  Unit for this operation
* @param    IntIfNum    @b{(input)}  Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_BOOL usmDbRip2IsValidIntf(L7_uint32 UnitIndex, L7_uint32 IntIfNum);


/*********************************************************************
* @purpose  Get the current status of displaying rip packet debug info
*            
* @param    none
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
extern L7_BOOL usmDbRipMapPacketDebugTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of rip packet debug info
*            
* @param    flag         new value of the Packet Debug flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t usmDbRipMapPacketDebugTraceFlagSet(L7_BOOL flag);

#endif /* _USMDB_MIB_RIPV2_API_H_ */

